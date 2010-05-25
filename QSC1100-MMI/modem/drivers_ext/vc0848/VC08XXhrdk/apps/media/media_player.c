/*======================================================================
*                                                                       
*                Copyright (C) 2007 Vimicro CO.,LTD     		 
* File Name: 	media_player.c
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
*	MP_U32 media_player_file_fastplay(VIM_MEDIA_PLAYER_FILE *pfile);
*	MP_U32 media_player_file_pause(VIM_MEDIA_PLAYER_FILE *pfile);
*	MP_U32 media_player_file_stop(VIM_MEDIA_PLAYER_FILE *pfile);
*	MP_U32 media_player_file_resume(VIM_MEDIA_PLAYER_FILE *pfile);
*	MP_U32 media_player_file_seek(VIM_MEDIA_PLAYER_FILE *pfile, MP_U32 pos, MP_U32 *accesspos);
*	MP_U32 media_player_file_gettotaltime(VIM_MEDIA_PLAYER_FILE *pfile, MP_U32 *totaltime);
*	MP_U32 media_player_file_getplaytime(VIM_MEDIA_PLAYER_FILE *pfile, MP_U32 *playtime);
*	MP_U32 media_player_file_getproperty(VIM_MEDIA_PLAYER_FILE* pfile,  VIM_MEDIA_FILE_PROPERTY*pProperty);
*	MP_U32 media_player_file_setproperty(VIM_MEDIA_PLAYER_FILE* pfile,  VIM_MEDIA_FILE_PROPERTY*pProperty);
*	
*	MR_U32 media_recorder_file_open(MP_U32 inputfile,, VIM_MEDIA_RECFILE *rfilehandle)
*	MR_U32 media_recorder_file_close(VIM_MEDIA_RECORDER_FILE* rfile);
*	MR_U32 media_recorder_file_start(VIM_MEDIA_RECORDER_FILE* rfile);
*	MR_U32 media_recorder_file_stop(VIM_MEDIA_RECORDER_FILE* rfile);
*	
*	MP_U32 media_set_video_displaypos(MP_U32 xStartPos, MP_U32 yStartPos);
*	MP_U32 media_set_video_displaysize(MP_U32 uWidth, MP_U32 uHeight);
*	MR_U32 media_set_video_previewpos(MR_U32 xStartPos, MR_U32 yStartPos);
*	MR_U32 media_set_video_previewsize(MR_U32 uWidth, MR_U32 uHeight);
*	
*	MP_U32 media_set_audio_outvolume(MP_U32 uLeftVolume, MP_U32 uRightVolume);
*	MP_U32 media_set_audio_speakervolume(MP_U32 uVolume);
*	MP_U32 media_set_audio_headphonevolume(MP_U32 uVolume);
*	
*	MR_U32 media_set_audio_inputvolume(MR_U32 uLeftVolume, MR_U32 uRightVolume);
*	MR_U32 media_set_audio_lineinvolume(MR_U32 uVolume);
*	MR_U32 media_set_audio_microphonevolume(MR_U32 uVolume);
*		
* release history:
* 		version 0.1 release by Amanda Deng (Deng Naili --Zhang Zhongliang AE)
=======================================================================*/
#include "media_player.h"	

#include "codec.h"

#include "msg.h"

#include "AEEStdlib.h"

VIM_MEDIA_PLYFILE_INPUTINFOR    	g_srcmediafile;
VIM_MEDIA_RECFILE_INPUTINFOR    	g_recmediafile;
VIM_MEDIA_PLYFILE_OUTINFOR    	g_plymediafile;
VIM_MEDIA_FILE_PROPERTY			g_mpfileproperty;
VIM_MEDIA_EFFECTPROC_PARAM	g_postprcparam;
VIM_MEDIA_PREVIEW_INFOR		g_previewinfor;
VIM_MEDIA_SYSTEM_INFOR* 		media_sysstatus;

Func_Callback g_plyCallback=NULL;
Func_RecCallback g_recCallback=NULL;
extern ME_U32 g_bufferindex_video;
extern ME_U32 g_bufferindex_audio;
extern ME_U32 endduration;
extern const MMD_U32 gD2H_STATUS_MemAddr[];
extern MR_U32 iFristWrite;
extern unsigned int g_vol_ctl_enable;	//	For volume Adjust Test
extern unsigned int g_vol_ctl_gain;
extern unsigned int DeviceMallocTimes;
extern unsigned int DeviceFreeTimes;
extern MP_U32  in_timerprocess;//audio read flag/* Serial IO being used */
extern volatile MR_U32 utime;
//extern MR_U32 time;
extern ME_U32 id3_mp3_register(void);

MEMORY_FILE		*mem_filehandle;
MMD_U32 uSerPicCount = 0;
volatile MMD_U32 recorder_stop = 0;
MMD_U32 uPhotoCount=0;
MEMORY_FILE		SerPicfilehandle[MAX_PIC_COUNT];
MMD_U32 Frame_Info[2];
/*MMD_U08 *TestBuffer=0;
MMD_U32 TestBufferSize=1000*1024;
MMD_U32 TestBufferIndex=0;*/

#define MP3_RECORDER_SRC_BUFFER_LENGTH  27648
#define MP3_RECORDER_DST_BUFFER_LENGTH 5500
#define AMR_RECORDER_SRC_BUFFER_LENGTH  8000
#define AMR_RECORDER_DST_BUFFER_LENGTH  2000


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
//by dnn 2008/4/21
MP_U32	media_work_mode_initialize(MP_U32 work_mode)
{	
    FARF(MSG,("media_work_mode_initialize=%d",work_mode));
	switch(work_mode)
	{
		case VIM_MEDIA_MODE_PLAY_AUDIO_ONLY:  	
			mp_debug(("VIM_MEDIA_MODE_PLAY_AUDIO_ONLY " ));
			media_sysstatus->work_mode|=VIM_MEDIA_MODE_PLAY_AUDIO_ONLY;
			break;
		case VIM_MEDIA_MODE_PLAY_VIDEO_ONLY:  	
			mp_debug(("VIM_MEDIA_MODE_PLAY_VIDEO_ONLY " ));
			media_sysstatus->work_mode|=VIM_MEDIA_MODE_PLAY_VIDEO_ONLY;
			break;
		case VIM_MEDIA_MODE_PLAY_AUDIO_AND_VIDEO:  	
			mp_debug(("MMD_MEDIA_MODE_PLAY_AUDIO_AND_VIDEO " ));
			media_sysstatus->work_mode|=VIM_MEDIA_MODE_PLAY_AUDIO_AND_VIDEO;
			break;
		case VIM_MEDIA_MODE_RECORD_AUDIO_ONLY:  	
			mp_debug(("VIM_MEDIA_MODE_RECORD_AUDIO_ONLY " ));
			media_sysstatus->work_mode|=VIM_MEDIA_MODE_RECORD_AUDIO_ONLY;
			break;
		case VIM_MEDIA_MODE_RECORD_VIDEO_ONLY:  	
			mp_debug(("VIM_MEDIA_MODE_RECORD_VIDEO_ONLY " ));
			media_sysstatus->work_mode|=VIM_MEDIA_MODE_RECORD_VIDEO_ONLY;
			break;
		case VIM_MEDIA_MODE_RECORD_AUDIO_AND_VIDEO:  	
			mp_debug(("VIM_MEDIA_MODE_RECORD_AUDIO_AND_VIDEO " ));
			media_sysstatus->work_mode|=VIM_MEDIA_MODE_RECORD_AUDIO_AND_VIDEO;
			break;
		case VIM_MEDIA_MODE_CAPTURE:  	
			mp_debug(("VIM_MEDIA_MODE_CAPTURE " ));
			media_sysstatus->work_mode|=VIM_MEDIA_MODE_CAPTURE;
			break;
		case VIM_MEDIA_MODE_VIDEOCONFERENCE:  	
			mp_debug(("VIM_MEDIA_MODE_VIDEOCONFERENCE " ));
			media_sysstatus->work_mode|=VIM_MEDIA_MODE_VIDEOCONFERENCE;
			break;
		case VIM_MEDIA_MODE_IDLE:
			break;
    		default:
    			mp_debug(("VIM_MEDIA_MODE_ERROR " ));
    			break;
	}
	return MP_SUCCESS;
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
//by dnn 2008/4/21
MP_U32	media_work_mode_exit(MP_U32 work_mode)
{
	switch(work_mode)
	{
		case VIM_MEDIA_MODE_PLAY_AUDIO_ONLY:  	
			mp_debug(("VIM_MEDIA_MODE_PLAY_AUDIO_ONLY " ));
			media_sysstatus->work_mode&=~VIM_MEDIA_MODE_PLAY_AUDIO_ONLY;
			break;
		case VIM_MEDIA_MODE_PLAY_VIDEO_ONLY:  	
			mp_debug(("MMD_MEDIA_MODE_PLAY_VIDEO_ONLY " ));
			media_sysstatus->work_mode&=~VIM_MEDIA_MODE_PLAY_VIDEO_ONLY;
			break;
		case VIM_MEDIA_MODE_PLAY_AUDIO_AND_VIDEO:  	
			mp_debug(("VIM_MEDIA_MODE_PLAY_AUDIO_AND_VIDEO " ));
			media_sysstatus->work_mode&=~VIM_MEDIA_MODE_PLAY_AUDIO_AND_VIDEO;
			break;
		case VIM_MEDIA_MODE_RECORD_AUDIO_ONLY:  	
			mp_debug(("VIM_MEDIA_MODE_RECORD_AUDIO_ONLY " ));
			media_sysstatus->work_mode&=~VIM_MEDIA_MODE_RECORD_AUDIO_ONLY;
//			audio_codec.codec_stop_recordmode();
			break;
		case VIM_MEDIA_MODE_RECORD_VIDEO_ONLY:  	
			mp_debug(("VIM_MEDIA_MODE_RECORD_VIDEO_ONLY " ));
			media_sysstatus->work_mode&=~VIM_MEDIA_MODE_RECORD_VIDEO_ONLY;
			break;
		case VIM_MEDIA_MODE_RECORD_AUDIO_AND_VIDEO:  	
			mp_debug(("VIM_MEDIA_MODE_RECORD_AUDIO_AND_VIDEO " ));
			media_sysstatus->work_mode&=~VIM_MEDIA_MODE_RECORD_AUDIO_AND_VIDEO;
//			audio_codec.codec_stop_recordmode();
			break;
		case VIM_MEDIA_MODE_CAPTURE:  	
			mp_debug(("VIM_MEDIA_MODE_CAPTURE " ));
			media_sysstatus->work_mode&=~VIM_MEDIA_MODE_CAPTURE;
			break;
		case VIM_MEDIA_MODE_VIDEOCONFERENCE:  	
			mp_debug(("VIM_MEDIA_MODE_VIDEOCONFERENCE " ));
			media_sysstatus->work_mode&=~VIM_MEDIA_MODE_VIDEOCONFERENCE;
			break;
		case VIM_MEDIA_MODE_IDLE:
			break;
    		default:
    			mp_debug(("VIM_MEDIA_MODE_ERROR " ));
    			break;
	}
	return MP_SUCCESS;
}

/*==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-
				structor and macro define for video confige [only in host system] 
-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==*/
MP_U32  media_set_effect(MP_U32 effect)
{
	VIM_HAPI_SetWorkMode(VIM_HAPI_MODE_CAMERAON);
	VIM_HAPI_SetEffect((VIM_HAPI_SPECIAL_EFFECT)effect);
	return MP_SUCCESS;
}

MR_U32  media_set_preview_parameter(MR_U32 preview_outinfo)
{
	VIM_MEDIA_PREVIEW_OUTINFOR* preview_out= (VIM_MEDIA_PREVIEW_OUTINFOR*) preview_outinfo;
	media_sysstatus->preview_infor.preview_dispwidth = preview_out->display_width;
	media_sysstatus->preview_infor.preview_dispheight = preview_out->display_height;
	media_sysstatus->preview_infor.preview_dispxpos = preview_out->display_xpos;
	media_sysstatus->preview_infor.preview_dispypos = preview_out->display_ypos;
	media_sysstatus->preview_infor.preview_effect = preview_out->display_effect;
	return MP_SUCCESS;
}

MR_U32  media_set_capture_resolution(MR_U32 width, MR_U32 height)
{
	media_sysstatus->preview_infor.capture_width = width;
	media_sysstatus->preview_infor.capture_height = height;
	return MP_SUCCESS;
}

MR_U32  media_set_thumbnail_resolution(MR_U32 width, MR_U32 height)
{
	MR_U32 result;
	media_sysstatus->preview_infor.thumbnail_width  	= width;
	media_sysstatus->preview_infor.thumbnail_height	 	= height;
	result=VIM_HAPI_SetThumbnailSize(width,height);
	if(result) 
	{
		mp_printf("VIM_HAPI_SetThumbnailSize=0x%08x", result);
		return result;
	}
	return MP_SUCCESS;
}

MR_U32  media_change_capture_resolution(MR_U32 width, MR_U32 height)
{
	MR_U32 result;
	
	media_sysstatus->preview_infor.capture_width = width;
	media_sysstatus->preview_infor.capture_height = height;
	result=VIM_HAPI_SetCaptureParameter(width,height);
	if(result) 
	{
		mp_printf("VIM_HAPI_SetCaptureParameter=0x%08x", result);
		return result;
	}
	VIM_HAPI_SetCaptureParameterChangeFlag();
	return MP_SUCCESS;
}

MR_U32  media_set_capture_quality(MR_U32 quality_class)
{
	media_sysstatus->preview_infor.capture_quality = quality_class;
	return MP_SUCCESS;
}

MR_U32  media_set_video_resolution(MR_U32 width, MR_U32 height)
{
	media_sysstatus->preview_infor.recvideo_width = width;
	media_sysstatus->preview_infor.recvideo_height = height;
	return MP_SUCCESS;
}
MR_U32  media_set_video_quality(MR_U32 quality_class)
{
	media_sysstatus->preview_infor.recvideo_quality = quality_class;
	return MP_SUCCESS;
}

MP_U32  media_set_audio_mode(MR_U32 audio_mode)
{
	media_sysstatus->audio_mode= audio_mode;
	
	return MP_SUCCESS;
}

MR_U32  media_preview_on(MR_U32 previewinfor)
{
	VIM_MEDIA_PREVIEW_INFOR* preview_infor;
	MR_U32 result;
	preview_infor=(VIM_MEDIA_PREVIEW_INFOR*)previewinfor;
	DeviceMallocTimes=0;
	DeviceFreeTimes=0;
	if(media_sysstatus->work_mode&VIM_MEDIA_MODE_CAPTURE)
	{

      result = MMD_SYS_Mode_Switch(SCENE_MEDIAPLAY);//zhangxiang add 
		if(result) 
		{
			mp_printf("MMD_SYS_Mode_Switch=0x%08x", result);
			return result;
		}
      result = MMD_SYS_Codec_Switch(CODEC_PCM);//zhangxiang add 
      MMD_SYS_SetModeClk(CLK_CPU216M_BUS72M);
      MSG_FATAL("MMD_SYS_Codec_Switch %d ", result, 0, 0);
		VIM_HAPI_SetWorkMode(VIM_HAPI_MODE_CAMERAON);
        #if 1
       // VIM_HAPI_SetLCDWorkMode(VIM_HAPI_LCDMODE_OVERLAY,0xffff);   //zhangxiang add
	    VIM_HAPI_SetLCDColordep(VIM_HAPI_COLORDEP_16BIT);//zhangxiang add
        VIM_HAPI_SetLCDWorkMode(VIM_HAPI_LCDMODE_BLONLY,0xffff);   //zhangxiang add
        VIM_HAPI_SetLCDSize(VIM_HAPI_B1_LAYER,0,0,0,0);//zhangxiang add
        VIM_HAPI_SetLCDSize(VIM_HAPI_B0_LAYER,0,0,176,220);//zhangxiang add
        VIM_HAPI_DrawLCDPureColor(VIM_HAPI_B0_LAYER,0,0,176,220,0xffff);//zhangxiang add
        MMD_LCD_SetOverlay (VM_ENABLE,MMD_LCD_B0LAYER,OVL_TSP, 0xffffff);//zhangxiang add
        #endif
		VIM_HAPI_SetEffect((VIM_HAPI_SPECIAL_EFFECT)preview_infor->preview_effect);
		result=VIM_HAPI_SetCaptureParameter(preview_infor->capture_width,preview_infor->capture_height);
      
		if(result) 
		{
			mp_printf("VIM_HAPI_SetCaptureParameter=0x%08x", result);
			return result;
		}
		result=VIM_HAPI_SetThumbnailSize(preview_infor->thumbnail_width,preview_infor->thumbnail_height);
        
        if(result) 
        {
           mp_printf("VIM_HAPI_SetThumbnailSize=0x%08x", result);
           return result;
        }
		result=VIM_HAPI_SetCaptureQuality(preview_infor->capture_quality);
		if(result) 
		{
			mp_printf("VIM_HAPI_SetCaptureQuality=0x%08x", result);
			return result;
		}
		
		result=VIM_HAPI_SetPreviewParameter(preview_infor->preview_dispxpos,
								     		preview_infor->preview_dispypos,
								     		preview_infor->preview_dispwidth,
								     		preview_infor->preview_dispheight);
		if(result) 
		{
			mp_printf("VIM_HAPI_SetPreviewParameter=0x%08x", result);
			return result;
		}
		if(preview_infor->preview_frameon==0)
		{
		result=VIM_HAPI_SetPreviewMode(VIM_HAPI_PREVIEW_ON,VIM_HAPI_HALFPIXEL);
		if(result) 
		{
			mp_printf("VIM_HAPI_SetPreviewMode=0x%08x", result);
			return result;
			}
		}
		else
		{
			result=VIM_HAPI_SetPreviewMode(VIM_HAPI_PREVIEW_FRAMEON,VIM_HAPI_HALFPIXEL);
			if(result) 
			{
				mp_printf("VIM_HAPI_SetPreviewMode=0x%08x", result);
				return result;
			}
		}
	}
	else if((media_sysstatus->work_mode&VIM_MEDIA_MODE_RECORD_AUDIO_AND_VIDEO)||
		(media_sysstatus->work_mode&VIM_MEDIA_MODE_RECORD_VIDEO_ONLY))
	{
		result = MMD_SYS_Mode_Switch(SCENE_REC_AMR);
       
		if(result) 
		{
			mp_printf("MMD_SYS_Mode_Switch=0x%08x", result);
			return result;
		}
        result=MMD_SYS_Codec_Switch(CODEC_SAMR); //zhangxiang add 
        MSG_FATAL("MMD_SYS_Codec_Switch %d ", result, 0, 0);
        MMD_SYS_SetModeClk(CLK_CPU216M_BUS72M);
            
		VIM_HAPI_SetWorkMode(VIM_HAPI_MODE_CAMERAON);
        #if 1
       // VIM_HAPI_SetLCDWorkMode(VIM_HAPI_LCDMODE_OVERLAY,0xffff);   //zhangxiang add
	    VIM_HAPI_SetLCDColordep(VIM_HAPI_COLORDEP_16BIT);//zhangxiang add
        VIM_HAPI_SetLCDWorkMode(VIM_HAPI_LCDMODE_BLONLY,0xffff);   //zhangxiang add
        VIM_HAPI_SetLCDSize(VIM_HAPI_B1_LAYER,0,0,0,0);//zhangxiang add
        result=VIM_HAPI_SetLCDSize(VIM_HAPI_B0_LAYER,0,0,176,220);//zhangxiang add
        MSG_FATAL("VIM_HAPI_SetLCDSize %d ", result, 0, 0);
        VIM_HAPI_DrawLCDPureColor(VIM_HAPI_B0_LAYER,0,0,176,220,0xffff);//zhangxiang add
        MMD_LCD_SetOverlay (VM_ENABLE,MMD_LCD_B0LAYER,OVL_TSP, 0xffffff);//zhangxiang add
        #endif
		VIM_HAPI_SetEffect((VIM_HAPI_SPECIAL_EFFECT)preview_infor->preview_effect);
		result=VIM_HAPI_SetCaptureParameter(preview_infor->recvideo_width,preview_infor->recvideo_height);
		if(result) 
		{
			mp_printf("VIM_HAPI_SetCaptureParameter=0x%08x", result);
			return result;
		}
		
		result=VIM_HAPI_SetPreviewParameter(preview_infor->preview_dispxpos,
								     		preview_infor->preview_dispypos,
								     		preview_infor->preview_dispwidth,
								     		preview_infor->preview_dispheight);
		if(result) 
		{
			mp_printf("VIM_HAPI_SetPreviewParameter=0x%08x", result);
			return result;
		}
		
		result=VIM_HAPI_SetPreviewMode(VIM_HAPI_MP4_PREVIEW_ON,VIM_HAPI_HALFPIXEL);
		if(result) 
		{
			mp_printf("VIM_HAPI_SetPreviewMode=0x%08x", result);
			return result;
		}
	}
	return MP_SUCCESS;
}

MR_U32  media_preview_off(void)
{
	MP_U32 result;
	
	result=VIM_HAPI_SetPreviewMode(VIM_HAPI_PREVIEW_OFF,VIM_HAPI_HALFPIXEL);
	if(result) 
	{
		mp_printf("VIM_HAPI_SetPreviewMode=0x%08x", result);
		return result;
	}
	
	return MP_SUCCESS;
}

MR_U32  media_preview_zoomin(void)
{
 	MP_U32 result;
	result=VIM_HAPI_SetPreviewZoom(VIM_HAPI_CHANGE_ADD, 1);

	if(result)
	{
		return result;
	}

	return MP_SUCCESS;
}

MR_U32  media_preview_zoomout(void)
{
	MP_U32 result;
	result=VIM_HAPI_SetPreviewZoom(VIM_HAPI_CHANGE_DEC, 1);

	if(result)
	{
		return result;
	}

	return MP_SUCCESS;
}
/*******************************************************************************************
Description:
	media engine initalize(include object register)
Parameters:
Return:	
	MP_SUCCESS: initialize ok
	Others:			error code	
NOTE: 
       this function call in main thread
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.04.02		create first version
	   0.2			Dang Ningna			2008.04.11		second version for 848 
********************************************************************************************/
MP_U32 media_engine_initialize(void)
{
	MP_U32 			result;
	MP_U32 			i;
	me_node_t		*pnod			= NULL;
	me_parser_t		*parser_cls		= NULL;
	me_muxer_t		*muxer_cls		= NULL;
	me_decoder_t		*decoder_cls	= NULL;
	me_encoder_t		*encoder_cls	= NULL;
	
	mp_print(("-----me interface initialize start:"));	
	
	result = media_engine_creat();
	mp_print(("-----media_engine_creat() return = 0x%08X",result));
	mp_print(("-----_g_media_engine.parserlist =0x%08X",(MP_U32)_g_media_engine.parserlist ));
	mp_print(("-----_g_media_engine.parserlist->counter = 0x%08X",_g_media_engine.parserlist->counter));	
	mp_print(("-----_g_media_engine.muxerlist =0x%08X",(MP_U32)_g_media_engine.muxerlist ));
	mp_print(("-----_g_media_engine.parserlist->counter = 0x%08X",_g_media_engine.parserlist->counter));	
	mp_print(("-----_g_media_engine.decoderlist =0x%08X",(MP_U32)_g_media_engine.decoderlist ));
	mp_print(("-----_g_media_engine.parserlist->counter = 0x%08X",_g_media_engine.parserlist->counter));	
	mp_print(("-----_g_media_engine.encoderlist =0x%08X",(MP_U32)_g_media_engine.encoderlist ));
	mp_print(("-----_g_media_engine.parserlist->counter = 0x%08X",_g_media_engine.parserlist->counter));	
	if(result )
		return result;

	mp_print(("-----parser interface test start: "));
#if ME_KM_PARSER_3GP_EN
	result = parser_3gp_register();
	mp_print(("-----parser_3gp_register() return = 0x%08X ",result));
	if(result )
		return result;
#endif
#if ME_KM_PARSER_MP4_EN
	result = parser_mp4_register();
	mp_print(("-----parser_mp4_register() return = 0x%08X ",result));
	if(result )
		return result;
#endif

#if ME_KM_PARSER_AVI_EN
	result = parser_avi_register();		//added by dnn 2007/8/21
	mp_print(("-----parser_avi_register() return = 0x%08X ",result));
	if(result )
		return result;
#endif
#if ME_KM_PARSER_AMR_EN
	result = parser_amr_register();
	mp_print(("-----parser_amr_register() return = 0x%08X ",result));
	if(result )
		return result;
#endif
#if ME_KM_PARSER_FLAC_EN
	result = parser_flac_register();	//yangl 2007 08 30
	mp_print(("-----parser_flac_register() return = 0x%08X ",result));
	if(result )
		return result;
#endif
#if ME_KM_PARSER_APE_EN
	result = parser_ape_register();
	mp_print(("-----parser_ape_register() return = 0x%08X ",result));
	if(result )
		return result;
#endif
#if ME_KM_PARSER_RM_EN
	result = parser_rm_register();		/* Zhang Xuecheng,2007.11.02 */
	mp_print(("-----parser_rm_register() return = 0x%08X ",result));
	if(result )
		return result;
#endif
#if ME_KM_PARSER_JPG_EN
	result = parser_jpg_register();	/* Add By Amanda Deng,2007.11.20 */
	mp_print(("-----parser_jpg_register() return = 0x%08X ",result));
	if(result )
		return result;
#endif

#if ME_KM_PARSER_MP3_EN
	result = parser_mp3_register();
	mp_print(("-----parser_mp3_register() return = 0x%08X ",result));
	if(result )
		return result;
#endif

#if ME_KM_PARSER_MIDI_EN
	result = parser_mid_register();
	mp_print(("-----parser_mid_register() return = 0x%08X ",result));
	if(result )
		return result;
#endif

#if ME_KM_PARSER_AAC_EN
	result = parser_aac_register();
	mp_print(("-----parser_aac_register() return = 0x%08X ",result));
	if(result )
		return result;
#endif
#if ME_KM_PARSER_OGG_EN
	result = parser_ogg_register();	/* Add By Amanda Deng,2007.12.14 */
	mp_print(("-----parser_ogg_register() return = 0x%08X ",result));
	if(result )
		return result;
#endif
#if ME_KM_PARSER_WMA_EN
	result = parser_wma_register();	/* Add By Amanda Deng,2007.12.14 */
	mp_print(("-----parser_wma_register() return = 0x%08X ",result));
	if(result )
		return result;
#endif
#if ME_KM_PARSER_WAV_EN
	result = parser_wav_register();	/* Add By Amanda Deng,2007.12.14 */
	mp_print(("-----parser_wav_register() return = 0x%08X ",result));
	if(result )
		return result;
#endif
#if ME_KM_PARSER_GIF_EN
	result = parser_gif_register();	/* Add By lhl,2007.12.28 */
	mp_print(("-----parser_wav_register() return = 0x%08X ",result));
	if(result )
		return result;
#endif

	mp_print(("-----_g_media_engine.parserlist->counter = 0x%08X ",_g_media_engine.parserlist->counter));	
	pnod = _g_media_engine.parserlist->first;
	for (i=0;i<_g_media_engine.parserlist->counter;i++)
	{
		parser_cls = (me_parser_t*)pnod->content;
		mp_print(("parser ID : %c%c%c%c", parser_cls->id[0], 
										 parser_cls->id[1], 
										 parser_cls->id[2], 
										 parser_cls->id[3]));
		pnod = pnod->next;
	}

	mp_print(("-----muxer interface test start: "));
#if ME_KM_MUXER_3GP_EN
	result = muxer_3gp_register();
	mp_print(("-----muxer_3gp_register() return = 0x%08X ",result));
	if(result )
		return result;
#endif
#if ME_KM_MUXER_MP4_EN
	result = muxer_mp4_register();
	mp_print(("-----muxer_mp4_register() return = 0x%08X ",result));
	if(result )
		return result;
#endif
#if ME_KM_MUXER_AMR_EN
	result = muxer_amr_register();
	mp_print(("-----muxer_amr_register() return = 0x%08X ",result));
	if(result )
		return result;
#endif
#if ME_KM_MUXER_JPG_EN
	result = muxer_jpg_register();
	mp_print(("-----muxer_jpg_register() return = 0x%08X ",result));
	if(result )
		return result;
#endif
#if ME_KM_MUXER_MP3_EN
	result = muxer_mp3_register();
	mp_print(("-----muxer_mp3_register() return = 0x%08X ",result));
	if(result )
		return result;
#endif
	mp_print(("-----_g_media_engine.muxerlist->counter = 0x%08X ",_g_media_engine.muxerlist->counter));	
	pnod = _g_media_engine.muxerlist->first;
	for (i=0;i<_g_media_engine.muxerlist->counter;i++)
	{
		muxer_cls = (me_muxer_t*)pnod->content;
		mp_print(("muxer ID : %c%c%c%c", muxer_cls->id[0], 
										 muxer_cls->id[1], 
										 muxer_cls->id[2], 
										 muxer_cls->id[3]));
		pnod = pnod->next;
	}
	
	mp_print(("-----decoder interface test start: "));
#if ME_KM_DECODE_S263_EN	
	result = decoder_s263_register();
	mp_print(("-----decoder_s263_register() return = 0x%08X ",result));
	if(result )
		return result;
#endif
#if ME_KM_DECODE_MP4V_EN
	result = decoder_mp4v_register();
	mp_print(("-----decoder_mp4v_register() return = 0x%08X ",result));
	if(result )
		return result;
#endif
#if ME_KM_DECODE_SAMR_EN
	result = decoder_samr_register();
	mp_print(("-----decoder_samr_register() return = 0x%08X ",result));
	if(result )
		return result;
#endif
#if ME_KM_DECODE_MP4A_EN
	result = decoder_mp4a_register();
	mp_print(("-----decoder_mp4a_register() return = 0x%08X ",result));
	if(result )
		return result;
#endif
#if ME_KM_DECODE_AMR_EN
	result = decoder_amr_register();
	mp_print(("-----decoder_amr_register() return = 0x%08X ",result));
	if(result )
		return result;
#endif
#if ME_KM_DECODE_AAC_EN
	result = decoder_aac_register();
	mp_print(("-----decoder_aac_register() return = 0x%08X ",result));
	if(result )
		return result;
#endif
#if ME_KM_DECODE_PCM_EN
	result = decoder_pcm_register();
	mp_print(("-----decoder_pcm_register() return = 0x%08X ",result));
	if(result )
		return result;
#endif
#if ME_KM_DECODE_MP3_EN
	result = decoder_mp3_register();
	mp_print(("-----decoder_mp3_register() return = 0x%08X ",result));
	if(result )
		return result;
#endif
#if ME_KM_DECODE_FLAC_EN
	result = decoder_flac_register();	//yangl 2007 08 30
	mp_print(("-----decoder_flac_register() return = 0x%08X ",result));
	if(result )
		return result;
#endif
#if ME_KM_DECODE_APE_EN
	result = decoder_ape_register();
	mp_print(("-----decoder_ape_register() return = 0x%08X ",result));
	if(result )
		return result;
#endif
#if ME_KM_DECODE_RA_EN
	result = decoder_ra_register();	/* Zhang Xuecheng,2007.12.17 */
	mp_print(("-----decoder_ra_register() return = 0x%08X ",result));
	if(result )
		return result;
#endif
#if ME_KM_DECODE_RV_EN
	result = decoder_rv_register();	/* Zhang Xuecheng,2007.12.17 */
	mp_print(("-----decoder_rv_register() return = 0x%08X ",result));
	if(result )
		return result;
#endif
#if ME_KM_DECODE_OGG_EN
	result = decoder_ogg_register();	/* Add By Amanda Deng,2007.12.14 */
	mp_print(("-----decoder_ogg_register() return = 0x%08X ",result));
	if(result )
		return result;
#endif
#if ME_KM_DECODE_WMA_EN
	result = decoder_wma_register();	/* Add By Amanda Deng,2007.12.14 */
	mp_print(("-----decoder_wma_register() return = 0x%08X ",result));
	if(result )
		return result;
#endif
#if ME_KM_DECODE_SJPG_EN
	result = decoder_sjpg_register();	/* Add By Amanda Deng,2007.12.14 */
	mp_print(("-----decoder_sjpg_register() return = 0x%08X ",result));
	if(result )
		return result;
#endif
#if ME_KM_DECODE_JPG_EN	
	result = decoder_jpg_register();	/* Add By Amanda Deng,2007.11.20 */
	mp_print(("-----decoder_jpg_register() return = 0x%08X ",result));
	if(result )
		return result;
#endif
#if ME_KM_DECODE_GIF_EN	
	result = decoder_gif_register();	/* Add By lhl,2007.12.28 */
	mp_print(("-----decoder_jpg_register() return = 0x%08X ",result));
	if(result )
		return result;
#endif
	mp_print(("-----_g_media_engine.decoderlist->counter = 0x%08X ",_g_media_engine.decoderlist->counter));	
	pnod = _g_media_engine.decoderlist->first;
	for (i=0;i<_g_media_engine.decoderlist->counter;i++)
	{
		decoder_cls = (me_decoder_t*)pnod->content;
		mp_print(("decoder ID : %c%c%c%c", decoder_cls->id[0], 
										   decoder_cls->id[1], 
										   decoder_cls->id[2], 
										   decoder_cls->id[3]));
		pnod = pnod->next;
	}
#if ME_KM_ENCODE_S263_EN
	result = encoder_s263_register();
	mp_print(("-----encoder_s263_register() return = 0x%08X ",result));
	if(result )
		return result;
#endif
#if ME_KM_ENCODE_MP4V_EN
	result = encoder_mp4v_register();
	mp_print(("-----encoder_mp4v_register() return = 0x%08X ",result));
	if(result )
		return result;
#endif
#if ME_KM_ENCODE_SAMR_EN
	result = encoder_samr_register();
	mp_print(("-----encoder_samr_register() return = 0x%08X ",result));
	if(result )
		return result;
#endif
#if ME_KM_ENCODE_AMR_EN
	result = encoder_amr_register();
	mp_print(("-----encoder_amr_register() return = 0x%08X ",result));
	if(result )
		return result;
#endif
#if ME_KM_ENCODE_MP3_EN
	result = encoder_mp3_register();
	mp_print(("-----encoder_mp3_register() return = 0x%08X ",result));
	if(result )
		return result;
#endif
#if ME_KM_ENCODE_JPG_EN
	result = encoder_jpg_register();
	mp_print(("-----encoder_jpg_register() return = 0x%08X ",result));
	if(result )
		return result;
#endif
#if ME_KM_ID3_MP3_EN
	id3_mp3_register();
#endif
	mp_print(("-----_g_media_engine.encoderlist->counter = 0x%08X ",_g_media_engine.encoderlist->counter));	
	pnod = _g_media_engine.encoderlist->first;
	for (i=0;i<_g_media_engine.encoderlist->counter;i++)
	{
		encoder_cls = (me_encoder_t*)pnod->content;
		mp_print(("encoder ID : %c%c%c%c", encoder_cls->id[0], 
										   encoder_cls->id[1], 
										   encoder_cls->id[2], 
										   encoder_cls->id[3]));
		pnod = pnod->next;
	}
	
	mp_print(("-----runtime status initialize! "));	
	
	media_sysstatus= (VIM_MEDIA_SYSTEM_INFOR*)mp_malloc(sizeof(VIM_MEDIA_SYSTEM_INFOR));
	if(media_sysstatus ==NULL)
	{
		mp_error(("mp_malloc(sizeof(VIM_MEDIA_SYSTEM_INFOR) return NULL"));
		return MP_ERR_MALLOC_MEMORY_ERROR;
	}
	mp_memset((unsigned char*)media_sysstatus, 0, sizeof(VIM_MEDIA_SYSTEM_INFOR));

/* by dnn 2008/4/10
#if (VIM_MEDIA_MAX_SUPPORT_FILE_NUMBER>1)
	// dma buffer initialize because system support play more than one file at a time
	media_sysstatus->ply_dmaaddr0 = (MP_U32)mp_malloc(PLAYER_DMA_BUFFER_SIZE);
	if(!media_sysstatus->ply_dmaaddr0)
	{
		mp_error(("mp_malloc(PLAYER_DMA_BUFFER_SIZE) return NULL"));
		return MP_ERR_MALLOC_MEMORY_ERROR;
	}
	mp_memset((unsigned char*)media_sysstatus->ply_dmaaddr1, 0, PLAYER_DMA_BUFFER_SIZE);

	media_sysstatus->ply_dmaaddr1= (MP_U32)mp_malloc(PLAYER_DMA_BUFFER_SIZE);
	if(!media_sysstatus->ply_dmaaddr1)
	{
		mp_error(("mp_malloc(PLAYER_DMA_BUFFER_SIZE) return NULL"));
		return MP_ERR_MALLOC_MEMORY_ERROR;
	}
	mp_memset((unsigned char*)media_sysstatus->ply_dmaaddr1, 0, PLAYER_DMA_BUFFER_SIZE);
#endif
*/

	mp_print(("-----me interface initialize finish! "));	
	mp_print((" "));	
	
/* by dnn 2008/4/10
	video_syn_timer_creat();
	video_rec_timer_creat();
*/
#if 0 //by lizg,moved to media_work_mode_initialize
	media_player_timer_creat();
	media_player_jpgdis_timer_creat();
	media_recorder_timer_creat();
	media_recorder_jpgcap_timer_creat();
#endif 
	return MP_SUCCESS;
}

/*******************************************************************************************
Description:
	media engine release(include object release)
Parameters:
Return:	
	MP_SUCCESS: release ok
	Others:			error code
NOTE: 
       this function call in main thread
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.04.02		create first version
	   0.2			Dang Ningna			2008.04.11		second version for 848
********************************************************************************************/
MP_U32 media_engine_release(void)
{
	MP_U32 result;

	if(media_sysstatus==NULL)
		return MP_SUCCESS;
	
	if(media_sysstatus->ply_filecounter)
	{
		mp_print(("-----media system %d player file have not closed!", media_sysstatus->ply_filecounter));
		return MP_ERROR;
	}

/* by dnn 2008/4/10	
	video_rec_timer_release();
	video_syn_timer_release();
*/	
#if 0// by lizg moved to media_player_file_close and media_recorder_file_close
	media_player_timer_release();
	media_player_jpgdis_timer_release();
	media_recorder_timer_release();
	media_recorder_jpgcap_timer_release();
#endif

	// quit media engine 
	mp_print(("-----me interface free start: "));	
	mp_print(("  "));	
	
	result = media_engine_destroy();
	mp_print(("-----media_engine_destroy() return = 0x%08X ",result));
	mp_print(("-----_g_media_engine.parserlist =0x%08X ",(int)_g_media_engine.parserlist ));
	mp_print(("-----_g_media_engine.parserlist->counter = 0x%08X ",_g_media_engine.parserlist->counter));	
	mp_print(("-----_g_media_engine.muxerlist =0x%08X ",(int)_g_media_engine.muxerlist ));
	mp_print(("-----_g_media_engine.muxerlist->counter = 0x%08X ",_g_media_engine.muxerlist->counter));	
	mp_print(("-----_g_media_engine.decoderlist =0x%08X ",(int)_g_media_engine.decoderlist ));
	mp_print(("-----_g_media_engine.decoderlist->counter = 0x%08X ",_g_media_engine.decoderlist->counter));	
	mp_print(("-----_g_media_engine.encoderlist =0x%08X ",(int)_g_media_engine.encoderlist ));
	mp_print(("-----_g_media_engine.encoderlist->counter = 0x%08X ",_g_media_engine.encoderlist->counter));	
	if(result )
		return result;
	
	mp_print(("-----me interface free finish! "));	
	mp_print(("  "));	

/* by dnn 2008/4/10
#if (VIM_MEDIA_MAX_SUPPORT_FILE_NUMBER>1)
	mp_free((void*)media_sysstatus->ply_dmaaddr0);
	mp_free((void*)media_sysstatus->ply_dmaaddr1);
#endif
*/
	mp_free(media_sysstatus);	
	media_sysstatus= NULL;
	
	return MP_SUCCESS;
}

MP_U32  media_player_change_displaysize(MP_U32 width,MP_U32 height)
{
	VIM_HAPI_ChangeDisplaySize(width,height);
	return MP_SUCCESS;
}

MP_U32  media_player_get_displaysize(MP_PU32 pwidth,MP_PU32 pheight)
{
	VIM_HAPI_GetDisplaySize(pwidth,pheight);
	return MP_SUCCESS;
}

/*******************************************************************************************
Description:
	media player check file handle is ok?
Parameters:
	VIM_MEDIA_PLAYER_FILE* pthis: player file handle
Return:	
	MP_SUCCESS: the player file handle is ok 
	MP_ERR_FILE_HANDLE_ERROR: error player file handle
NOTE: 
       this function maybe call in all  thread
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.04.02		create first version
********************************************************************************************/
MP_U32 media_player_check_filehandle(VIM_MEDIA_PLAYER_FILE* pthis)
{
	MP_U32 i;
	
	if(pthis==NULL)
		return MP_ERR_FILE_HANDLE_ERROR;
	
	for(i=0;i<VIM_MEDIA_MAX_SUPPORT_FILE_NUMBER;i++)
	{
		if(media_sysstatus->plyfilelst[i]==pthis)
		{
			return MP_SUCCESS;
		}
	}
	return MP_ERR_FILE_HANDLE_ERROR;
}

/*******************************************************************************************
Description:
<<<<<<< .mine
=======
	read video data to src buffer(src buffer is pingpang buffer)
Parameters:
	MP_FILE* pthis: player file handle
	MP_U32 buffer_index: src buffer index
Return:	
	MP_SUCCESS: read data ok
	Others: parser->read() function result
NOTE: 
       this function call in file  thread
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.07.02		create first version
********************************************************************************************/
#if 0
MP_U32 media_player_video_read_oneframe(VIM_MEDIA_PLAYER_FILE* pfile, MP_U32 buffer_index)
{
	MP_FILE			*mpfile;
	void 		  	*parserinfor;
	me_parser_t	  	*parser;
	video_stream_infor	*buffer;
	MP_U32  			read_bytes, datalength, result;
	MP_U32 			src_address;
#if VIDEO_FRAMES_READ_TIME_OUTPUT
	MP_U32  		s_time, e_time;
#endif
	result = media_player_check_filehandle(pfile);
	if(result)			
		return result;
	mpfile = pfile->mp_filehandle;
	parserinfor  = mpfile->parserinfor;
	parser	   = mpfile->parser;
	buffer	   = &mpfile->video_data.src.buffer[buffer_index];
	if(buffer->isnot_empty )
		return MP_SUCCESS;	
	
	src_address = (MP_U32)buffer->buffer_address;
#if READ_VIDEO_DATA_TO_VC0578B	
	if(pfile->v_countbak)
	{
		mp_memcpy((char*)src_address, 
		                      (const char*)&pfile->v_databak, 
		                      pfile->v_countbak);
		src_address += pfile->v_countbak;
	}
#endif
#if VIDEO_FRAMES_READ_TIME_OUTPUT
	s_time = get_system_time_ticks();	//(us) 
#endif
	if(!pfile->f_playmode)
		result = parser->read_video_keyframe(parserinfor, 
								     (char*)src_address,
								     &read_bytes,
								     buffer->buffer_size);
	else 
		result = parser->read_video_oneframe(parserinfor, 
								     (char*)src_address,
								     &read_bytes,
								     buffer->buffer_size);
#if VIDEO_FRAMES_READ_TIME_OUTPUT
	e_time = get_system_time_ticks();	//(us) 
	mp_debug(("read video buffer = %d, read_bytes = %d, time = %d ", buffer_index, read_bytes, e_time-s_time));
#endif
	datalength = read_bytes+pfile->v_countbak;
#if READ_VIDEO_DATA_TO_VC0578B	
	pfile->v_countbak = datalength % 4;
	if(pfile->v_countbak)
	{
		datalength -= pfile->v_countbak;
		src_address = (MP_U32)buffer->buffer_address;
		src_address += datalength;
		mp_memcpy((char*)&pfile->v_databak, 
					 (const char*)src_address, 
		                      pfile->v_countbak);
	}
#endif
	buffer->cursor_read	= 0;	
	buffer->data_length	= datalength;
	buffer->isnot_empty 	= read_bytes;
	buffer->is_over 		= result;
	if(result)
	{	//if read file error, end video play
		pfile->f_playcontrol |= MP_VIDEO_STREAM_END;
		mp_debug(("video data read end, file status = 0x%08X ", pfile->f_playcontrol ));
	}
	return result;
}
#endif
/*******************************************************************************************
Description:
	read video data to src buffer(src buffer is pingpang buffer)
Parameters:
	MP_FILE* pthis: player file handle
	MP_U32 buffer_index: src buffer index
Return:	
	MP_SUCCESS: read data ok
	Others: parser->read() function result
NOTE: 
       this function call in file  thread
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.09.17		create first version
********************************************************************************************/
MP_U32 media_player_video_read_frames(VIM_MEDIA_PLAYER_FILE* pfile, MP_U32 buffer_index)
{
	MP_FILE			*mpfile;
	void 		  	*parserinfor;
	me_parser_t	  	*parser;
	video_stream_infor	*buffer;
	MEDIA_FIFO  		frmduration;
	MP_U32  			result;
	MP_U32		cnt=0;
#if VIDEO_FRAMES_READ_TIME_OUTPUT
	MP_U32  		s_time, e_time;
#endif
	result = media_player_check_filehandle(pfile);
	if(result)			
		return result;
	if(pfile->f_playcontrol & MP_VIDEO_STREAM_END)
		return MP_SUCCESS;	
	mpfile = pfile->mp_filehandle;
	parserinfor = mpfile->parserinfor;
	parser	   = mpfile->parser;
	buffer	   = &mpfile->video_data.src.buffer[buffer_index];
	if(buffer->isnot_empty )
		return MP_SUCCESS;	
#if VIDEO_FRAMES_READ_TIME_OUTPUT
	s_time = get_system_time_ticks();	//(us) 
#endif

	if(pfile->fs_filestore==FILE_IN_CORE_FILE) {
		if(!pfile->f_playmode)
			result = parser->read_video_keyframes(parserinfor, buffer);
		else {
			if(mpfile->video_infor.v_isvalid==1){
				result = parser->read_video_frames_reg(parserinfor, buffer, IDX_VID_STATUS, buffer_index);
			} else {
				result = parser->read_video_frames(parserinfor, buffer);
			}
		}	
	} else {
		if(!pfile->f_playmode)
			result = parser->read_video_keyframes(parserinfor, buffer);
		else
			result = parser->read_video_frames(parserinfor, buffer);
	}

#if VIDEO_FRAMES_READ_TIME_OUTPUT
	e_time = get_system_time_ticks();	//(us) 
	mp_debug(("read video buffer = %d, read_bytes = %d, frames = %d, time = %d ", 
		             buffer_index, buffer->data_length, buffer->frame_count,e_time-s_time));
#endif

	if(pfile->fs_filestore!=FILE_IN_CORE_FILE) {
		if(mpfile->video_infor.v_isvalid==1){
			cnt = buffer->data_length%4;
			if(cnt!=0){
				me_memset_h2d(buffer->buffer_address+buffer->cursor_read+buffer->data_length, 4-cnt);
			}
		}
	} 
	
	//form here this is important for video and audio syn Amanda.Deng  2007/10/25
	frmduration.address = (char*)mpfile->video_data.src.duration;
	frmduration.pread = mpfile->video_data.src.duration_read*4;
	frmduration.pwrite = mpfile->video_data.src.duration_write*4;
	frmduration.size =MAX_VIDEO_FRAMECNT_IN_BUFFER*4*4;
	mp_writetofifo(&frmduration, (const char*)buffer->frame_duration, buffer->frame_count*4);
	mpfile->video_data.src.duration_write += buffer->frame_count;
	if(mpfile->video_data.src.duration_write>=MAX_VIDEO_FRAMECNT_IN_BUFFER*4)
	{
		//mp_print(("WRITE: %d!", mpfile->video_data.src.duration_write));
		mpfile->video_data.src.duration_write = mpfile->video_data.src.duration_write-MAX_VIDEO_FRAMECNT_IN_BUFFER*4;
	}
	//end here this is important for video and audio syn Amanda.Deng  2007/10/25
//	buffer->cursor_read	= 0;	
	if(pfile->fs_filestore==FILE_IN_CORE_FILE) {
		if(mpfile->video_infor.v_isvalid==3){
			buffer->isnot_empty 	= buffer->data_length;	
		}
	} else {
		buffer->isnot_empty 	= buffer->data_length;
	}
	buffer->is_over 		= result;
	if(result)
	{	//if read file error, end video play
		pfile->f_playcontrol |= MP_VIDEO_STREAM_END;
		mp_debug(("video data read end, file status = 0x%08X ", pfile->f_playcontrol ));
	}
	return result;
}

/*******************************************************************************************
Description:
	read audio data to src buffer(src buffer is pingpang buffer)
Parameters:
	MP_FILE* pthis: player file handle
	MP_U32 buffer_index: src buffer index
Return:	
	MP_SUCCESS: read data ok
	Others: parser->read() function result
NOTE: 
       this function call in file  thread	
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.07.02		create first version
********************************************************************************************/
#if 0
MP_U32 media_player_audio_read_oneframe(VIM_MEDIA_PLAYER_FILE* pfile, MP_U32 buffer_index)
{
	MP_FILE			*mpfile;
	void 		  	*parserinfor;
	me_parser_t	  	*parser;
	audio_stream_infor	*buffer;
	MP_U32  		read_bytes, result;
	
#if AUDIO_FRAMES_READ_TIME_OUTPUT
	MP_U32  		s_time, e_time;
#endif
	result = media_player_check_filehandle(pfile);
	if(result)			
		return result;
	mpfile = pfile->mp_filehandle;
	parserinfor  = mpfile->parserinfor;
	parser	   = mpfile->parser;
	buffer	   = &mpfile->audio_data.src.buffer[buffer_index];
	
#if AUDIO_FRAMES_READ_TIME_OUTPUT
	s_time = get_system_time_ticks();	//(us) 
#endif
	result = parser->read_audio_oneframe(parserinfor, 
								     (char*)buffer->buffer_address,
								     &read_bytes,
								     buffer->buffer_size);
#if AUDIO_FRAMES_READ_TIME_OUTPUT
	e_time = get_system_time_ticks();	//(us) 
	mp_debug(("read audio buffer = %d, read_bytes = %d, time = %d ", buffer_index, read_bytes, e_time-s_time));
#endif
	if(result)
	{	//if read file error, end audio play
		pfile->f_playcontrol |= MP_AUDIO_STREAM_END;
		mp_debug(("audio data read end, file status = 0x%08X ", pfile->f_playcontrol ));
	}
	buffer->cursor_read	= 0;	
	buffer->data_length	= read_bytes;
	buffer->isnot_empty	= read_bytes;
	buffer->is_over 		= result;
	return result;
}
#endif
/*******************************************************************************************
Description:
	read audio data to src buffer(src buffer is pingpang buffer)
Parameters:
	MP_FILE* pthis: player file handle
	MP_U32 buffer_index: src buffer index
Return:	
	MP_SUCCESS: read data ok
	Others: parser->read() function result
NOTE: 
       this function call in file  thread	
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.09.17		create first version
********************************************************************************************/
MP_U32 media_player_audio_read_frames(VIM_MEDIA_PLAYER_FILE* pfile, MP_U32 buffer_index)
{
	MP_FILE			*mpfile;
	void 		  	*parserinfor;
	me_parser_t	  	*parser;
	MP_U32  	result=0;
#if AUDIO_FRAMES_READ_TIME_OUTPUT
	MP_U32  		s_time, e_time;
#endif

	result = media_player_check_filehandle(pfile);
	if(result)			
		return result;
	if(pfile->f_playcontrol & MP_AUDIO_STREAM_END)
		return MP_SUCCESS;	
	mpfile = pfile->mp_filehandle;
	parserinfor  = mpfile->parserinfor;
	parser	   = mpfile->parser;

#if AUDIO_FRAMES_READ_TIME_OUTPUT
	s_time = get_system_time_ticks();	//(us) 
#endif
	if(pfile->fs_filestore==FILE_IN_CORE_FILE) {
		result = parser->read_audio_frames_reg(parserinfor, &(mpfile->audio_data.src.buffer[buffer_index]),IDX_AUD_STATUS,buffer_index);
	} else {
		result = parser->read_audio_frames(parserinfor, &(mpfile->audio_data.src.buffer[buffer_index]));
	}
#if AUDIO_FRAMES_READ_TIME_OUTPUT
	e_time = get_system_time_ticks();	//(us) 
	mp_debug(("read audio buffer = %d, read_bytes = %d, frames = %d, time = %d ", 
		              buffer_index, mpfile->audio_data.src.buffer[buffer_index].data_length, mpfile->audio_data.src.buffer[buffer_index].frame_count,e_time-s_time));
#endif	
	//printf("media_player_audio_read_frames once!\n");
	if(result)
	{	//if read file error, end audio play
		pfile->f_playcontrol |= MP_AUDIO_STREAM_END;
		//printf("audio data read end, file status = 0x%08X ", pfile->f_playcontrol );
	} 
	return result;
}

/*******************************************************************************************
Description:
	decode src buffer data to dst buffer(src\dst buffer is pingpang buffer)
Parameters:
	MP_FILE* pthis: player file handle
	MP_U32 src_index: src buffer index
	MP_U32 dst_index: dst buffer index
Return:	
	MP_SUCCESS: decode data ok
	Others: decoder->decode() function result
NOTE: 
       this function call in audio thread	
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.07.02		create first version
********************************************************************************************/
MP_U32 media_player_audio_decode_oneframe(VIM_MEDIA_PLAYER_FILE* pfile, MP_U32 src_index, MP_U32 dst_index)
{
#if 0
	MP_FILE			*mpfile;
	audio_stream_infor	*src_buffer;
	audio_pcm_infor	*dst_buffer;
	MP_U32 		dst_len, dec_len, result;
	MP_U32 		src_address, buffer_index;
#ifndef _WIN32
	VIM_MSG	msg;
#endif
#if AUDIO_FRAMES_DECODE_TIME_OUTPUT
	MP_U32  		s_time, e_time;
#endif
	result = media_player_check_filehandle(pfile);
	if(result)			
		return result;
	mpfile = pfile->mp_filehandle;
	src_buffer = &mpfile->audio_data.src.buffer[src_index];
	dst_buffer = &mpfile->audio_data.dst.buffer[dst_index];
	src_address   = src_buffer->buffer_address;
	src_address += src_buffer->cursor_read;
#if AUDIO_FRAMES_DECODE_TIME_OUTPUT
	s_time = get_system_time_ticks();	//(us) 
#endif
	result=mpfile->audio_decoder->decode_oneframe(mpfile->audio_decinfor,
											      (char*)src_address, src_buffer->data_length,
											      (char*)dst_buffer->buffer_address, &dst_len, &dec_len);
#if AUDIO_FRAMES_DECODE_TIME_OUTPUT
	e_time = get_system_time_ticks();	//(us) 
	mp_debug(("audio decode time = %d ", e_time-s_time));
	mp_debug(("audio src buffer %d, dst buffer %d, stream len=%d, pcm len=%d", src_index, dst_index, dec_len, dst_len));
#endif
	
	if(result)
	{
		//update audio stream fifo infor
		src_buffer->is_over = result;
		src_buffer->data_length = 0;
		dst_len = dst_buffer->buffer_size;
	}
	else
	{
		//update audio stream fifo infor
		src_buffer->cursor_read+= dec_len;
		src_buffer->data_length -= dec_len;
	}
	if(!src_buffer->data_length)
	{
		src_buffer->isnot_empty = 0;
		src_buffer->cursor_read = 0;
		if(src_buffer->is_over)
		{
			dst_buffer->is_over = 1;
			pfile->f_playcontrol |= MP_AUDIO_DECODE_END;
			mp_debug(("file status = 0x%08X ", pfile->f_playcontrol ));
		}
		else
		{
			buffer_index = src_index+1;
			if(buffer_index>=mpfile->audio_data.src.buffer_count)
				buffer_index = 0;
			mpfile->audio_data.src.buffer_read = buffer_index;
			mpfile->audio_data.src.buffer_write = src_index;
#ifndef _WIN32
			msg.CMD = MSG_MAJ_PLY|MIN_PLY_AUDIO_READ_ONEFRAME;
			msg.OP1 = (MP_U32)pfile;
			msg.OP2 = src_index;
			#ifdef MESSAGE_PROCESS_IN_MEFILETHREAD
				mmd_send_mefile_msg(&msg, 0);
			#else
				mmd_send_msg_file(&msg, 0);
			#endif
#endif
		}
	}	
	//update pcm fifo info
	dst_buffer->cursor_read = 0;
	dst_buffer->data_length = dst_len;
	dst_buffer->isnot_empty = dst_len;
	dst_buffer->is_valid = 1;
	return result;
#else
	return MP_SUCCESS;
#endif
}

MP_U32 media_player_video_decode_frames(VIM_MEDIA_PLAYER_FILE* pfile, MP_U32 src_index, MP_U32 dst_index)
{
#if 0
	MP_FILE			*mpfile;
	video_stream_infor	*src_buffer;
	video_rawdata_infor*dst_buffer;
	MP_U32 		result;
	MP_U32 		buffer_index, frameduration;
#ifndef _WIN32
	VIM_MSG	msg;
#endif
#if VIDEO_FRAMES_DECODE_TIME_OUTPUT
	MP_U32  		s_time, e_time;
#endif
	result = media_player_check_filehandle(pfile);
	if(result)			
		return result;
	mpfile = pfile->mp_filehandle;
	src_buffer = &mpfile->video_data.src.buffer[src_index];
	dst_buffer = &mpfile->video_data.dst.buffer[dst_index];
#if VIDEO_FRAMES_DECODE_TIME_OUTPUT
	s_time = get_system_time_ticks();	//(us) 
#endif
	result=mpfile->video_decoder->decode_frames(mpfile->video_decinfor,
											(void*)src_buffer,
											(void*)dst_buffer);
#if VIDEO_FRAMES_DECODE_TIME_OUTPUT
	e_time = get_system_time_ticks();	//(us) 
	mp_debug(("video decode time = %d ", e_time-s_time));
	mp_debug(("video src buffer %d, dst buffer %d, stream len=%d, pcm len=%d", src_index, dst_index, dec_len, dst_buffer->data_length));
#endif
	//check decode return value
	if(result)
	{
		//update video stream fifo infor
		src_buffer->is_over = result;
		src_buffer->cursor_read = src_buffer->data_length;
		pfile->f_playcontrol |= MP_VIDEO_DECODE_END;
		pfile->f_playcontrol |= MP_VIDEO_PLAY_END;
	}
	//software decode video, need update v_time and lastdone flag
	if(mpfile->video_infor.v_isvalid>1)
	{	//software decode need set this flag in here, hardware decode set this flag in interrput
		pfile->v_lastdone =1;
		//get current frame duration
		mpfile->video_decoder->get_frameduration((void*)mpfile->video_decinfor, &frameduration);

		//for rmvb,zhang xuecheng,2008-1-23 15:57:23
		if(strcmp(mpfile->video_infor.v_codec , "rv") == 0)
			pfile->v_time = frameduration;
		else
			pfile->v_time += frameduration;
	}
	//check src buffer data length
	if(src_buffer->cursor_read == src_buffer->data_length)
	{
		src_buffer->isnot_empty	= 0;
		src_buffer->data_length 	= 0;
		src_buffer->cursor_read  	= 0;
		src_buffer->cursor_write 	= 0;
		if(!src_buffer->is_over)
		{
			buffer_index = src_index+1;
			if(buffer_index>=mpfile->video_data.src.buffer_count)
				buffer_index = 0;
			mpfile->video_data.src.buffer_read = buffer_index;
			mpfile->video_data.src.buffer_write = src_index;
#ifdef _WIN32
			media_player_video_read_frames(pfile, src_index);
#else
			msg.CMD = MSG_MAJ_PLY|MIN_PLY_VIDEO_READ_FRAMES;
			msg.OP1 = (MP_U32)pfile;
			msg.OP2 = src_index;
			#ifdef MESSAGE_PROCESS_IN_MEFILETHREAD
				mmd_send_mefile_msg(&msg, 0);
			#else
				mmd_send_msg_file(&msg, 0);
			#endif
#endif
		}
	}	
	return result;
#else
	return MP_SUCCESS;
#endif	
}

/*******************************************************************************************
Description:
>>>>>>> .r59
	media player src and out buffer initalize(now use malloc)
Parameters:
	MP_FILE* pthis: player file handle
Return:	
	MP_SUCCESS: initialize ok
	Others:		 error code
NOTE: 
       this function call in main thread(initialize when first play the file)	
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.04.02		create first version
	   0.2			Dang Ningna			2008.04.11		second version for 848
********************************************************************************************/
MP_U32 media_player_buffer_initialize(MP_FILE* pthis)
{
	MP_U32 ret, size, dstsize;
	MP_S32 i;
	void	*parserinfor,  *decoderinfo;

	AUD_BUF_INFO Abuf_Info;
	AUD_BUF_ADDR Abuf_Addr ; 
	VID_BUF_INFO Vbuf_Info;
	//add by yjz 2009/2/27
	MP_U32 srcsize;
	
	parserinfor = pthis->parserinfor;
	if(pthis->audio_infor.a_isvalid)
	{
		ret=pthis->parser->get_audio_frame_buffersize(parserinfor, &size, (void*)(&pthis->audio_infor) );
		if(ret)
		{
			mp_error(("get_audio_frame_buffersize() retrun =0x%08X",ret));
			return ret;
		}
		//size=pthis->audio_infor.a_maxfrmsize;
		mp_print(("audio stream buffer size=%d",size));
		//add by yjz 2009/2/27
		srcsize = size;
		///:~
		size += 3;
		size &= 0xfffffffc;
		pthis->audio_data.src.buffer_count = AUDIO_PLAY_SRC_BUFFER;
		#if 1
		if(srcsize > 10*1024)
		{
			dstsize = srcsize;
		}
		else
		{
			if((strcmp(pthis->fs_filetype,"3gp")==0)||(strcmp(pthis->fs_filetype,"3GP")==0) ||(strcmp(pthis->fs_filetype,"mp4")==0)||(strcmp(pthis->fs_filetype,"MP4")==0))
			{
				dstsize = 8*1024;					
			}
			else
			{
				dstsize = 10*1024;	
			}
		}
		#else
			dstsize = 10*1024;		
		#endif
//		dstsize += 3;
//		dstsize &= 0xfffffffc;
		pthis->audio_data.dst.buffer_count = AUDIO_PLAY_DST_BUFFER;

		Abuf_Info.AUDIO_CODEC = (pthis->audio_infor.a_codec[0])|(pthis->audio_infor.a_codec[1]<<8)|
							(pthis->audio_infor.a_codec[2]<<16)|(pthis->audio_infor.a_codec[3]<<24);
		Abuf_Info.SRC_SIZE = size;
		Abuf_Info.SRC_NUM = AUDIO_PLAY_SRC_BUFFER;
		Abuf_Info.PCM_SIZE = dstsize;
		Abuf_Info.PCM_NUM = AUDIO_PLAY_DST_BUFFER;
		ret = media_player_audio_bufinit_smtd(&Abuf_Info, &Abuf_Addr);
		if(ret) {
			MMD_ERROR_Deal(CMD_FAILED,MAJ_PLY);
			mp_error(("media_player_audio_bufinit_smtd() return = 0x%08X",ret));
			return MP_ERR_MALLOC_MEMORY_ERROR;
		}
		for(i=0;i<AUDIO_PLAY_SRC_BUFFER;i++)
		{
			pthis->audio_data.src.buffer[i].buffer_address = Abuf_Addr.SRC_ADDR[i];
			pthis->audio_data.src.buffer[i].buffer_size =size;
		}
		for(i=0;i<AUDIO_PLAY_DST_BUFFER;i++)
		{
			pthis->audio_data.dst.buffer[i].buffer_address = Abuf_Addr.PCM_ADDR[i];
			pthis->audio_data.dst.buffer[i].buffer_size = dstsize;
		}

		//pthis->audio_infor.a_others = NULL;
		//pthis->audio_infor.a_otherslen = 0;

		//: Add by antonliu, 2008/06/11
		ret = media_player_audio_paraminit_smtd(pthis->audio_outinfor.a_outsmplrate, pthis->audio_outinfor.a_outmode);
        	if(ret) {
        		MMD_ERROR_Deal(CMD_FAILED,MAJ_PLY);
        		ME_ERROR(("media_player_audio_paraminit_smtd() return = 0x%08X", ret));
        		return ret;
        	}
			
		/*ret = media_player_audio_decinit_smtd(&(pthis->audio_infor), &(pthis->audio_outinfor), (pthis->audio_infor).a_otherslen);
		if(ret) {
			MMD_ERROR_Deal(CMD_FAILED,MAJ_PLY);
			ME_ERROR(("media_player_audio_decinit_smtd() return = 0x%08X", ret));
			return ret;
		}*/
	}
	if(pthis->video_infor.v_isvalid)
	{
		ret=pthis->parser->get_video_frame_buffersize(parserinfor, &size);
		if(ret)
		{
			mp_error(("get_video_frame_buffersize() return = 0x%08X",ret));
			return ret;
		}
		mp_print(("video stream buffer size=%d",size));
		
		size += 3;
		size &= 0xfffffffc;
		pthis->video_data.src.buffer_count = VIDEO_PLAY_SRC_BUFFER;

		for(i=0;i<pthis->video_data.src.buffer_count;i++)
		{
			pthis->video_data.src.buffer[i].buffer_size = size;
			if(pthis->video_infor.v_isvalid==3)
			{
				pthis->video_data.src.buffer[i].buffer_address = (MP_U32)mp_malloc(size);
			}
			else if(pthis->video_infor.v_isvalid==1)
			{
				pthis->video_data.src.buffer[i].buffer_address = (MP_U32)mp_malloc_h2d(size);
			}
			if(pthis->video_data.src.buffer[i].buffer_address == NULL)
				return MP_ERR_MALLOC_MEMORY_ERROR;
//			else
//				mp_memset_h2d((char*)pthis->video_data.src.buffer[i].buffer_address, 0, size);
		}

		if(pthis->video_infor.v_isvalid==1) {
			Vbuf_Info.SRC_NUM = pthis->video_data.src.buffer_count;
			Vbuf_Info.SRC_SIZE = size;
			for(i=0;i<pthis->video_data.src.buffer_count;i++){
				Vbuf_Info.SRC_ADDR[i] = pthis->video_data.src.buffer[i].buffer_address;
			}
			ret = media_player_video_bufinit_smtd(&Vbuf_Info);
			if(ret) {
				MMD_ERROR_Deal(CMD_FAILED,MAJ_PLY);
				mp_error(("media_player_video_bufinit_smtd() return = 0x%08X",ret));
				return ret;
			}
		}
		
		decoderinfo= pthis->video_decinfor;
		ret=pthis->video_decoder->dst_buffersize(decoderinfo, size, &dstsize);
		if(ret)
		{
			mp_error(("get_max_frame_size() return = 0x%08X",ret));
			return ret;
		}
		
		pthis->video_data.dst.buffer_count = VIDEO_PLAY_DST_BUFFER;
		for(i=0;i<VIDEO_PLAY_DST_BUFFER;i++)
		{
			pthis->video_data.dst.buffer[i].buffer_size = dstsize;
		}
#if	0	
		if(pthis->video_infor.v_isvalid>1)
		{
			if(strcmp(pthis->video_infor.v_codec,"rv") != 0)
			for(i=0;i<VIDEO_PLAY_DST_BUFFER;i++)
			{
				pthis->video_data.dst.buffer[i].buffer_address = (MP_U32)mp_malloc(dstsize);
				if(pthis->video_data.dst.buffer[i].buffer_address == NULL)
					return MP_ERR_MALLOC_MEMORY_ERROR;
				else
					mp_memset((unsigned char*)pthis->video_data.dst.buffer[i].buffer_address, 0, dstsize);
			}
		}
		else
		{
			for(i=0;i<VIDEO_PLAY_DST_BUFFER;i++)
			{
				pthis->video_data.dst.buffer[i].buffer_address = 0;
			}
		}
#endif 		
		for(i=0;i<VIDEO_PLAY_DST_BUFFER;i++)
		{
			pthis->video_data.dst.buffer[i].buffer_address = 0;
		}

		mp_print(("yuv buffer size=%d",dstsize));
	}
	return MP_SUCCESS;
}
/*******************************************************************************************
Description:
	media player src and out buffer release(now use malloc)
Parameters:
	MP_FILE* pthis: player file handle
Return:	
	MP_SUCCESS: release ok
	Others:		 error code	
NOTE: 
       this function call in file thread(release when close file)
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.04.02		create first version
	   0.2			Dang Ningna			2008.04.11		second version for 848
********************************************************************************************/
MP_U32 media_player_buffer_release(MP_FILE* pthis)
{
	MP_S32 i;
	MP_U32 result=0;

#if 0	
	if(pthis->video_infor.v_isvalid>1)
	{	//only software decode video need dst buffer
		for(i=pthis->video_data.dst.buffer_count-1;i>=0;i--)
			mp_free((void*)pthis->video_data.dst.buffer[i].buffer_address);
	}
#endif	
	if(pthis->audio_infor.a_isvalid) {
		result = media_player_audio_bufrelease_smtd();
		if(result){
			mp_error(("media_player_audio_bufrelease_smtd=0x%08x",result));
			return result;
		}
	}
	for(i=pthis->video_data.src.buffer_count-1;i>=0;i--) {
		if(pthis->video_infor.v_isvalid==3)
		{
			mp_free((void*)pthis->video_data.src.buffer[i].buffer_address);
		}
		else if(pthis->video_infor.v_isvalid==1)
		{
			mp_free_h2d((void*)pthis->video_data.src.buffer[i].buffer_address);
		}
		pthis->video_data.src.buffer[i].buffer_address = NULL;
		if(pthis->video_infor.v_isvalid==1){
			result = media_player_video_bufrelease_smtd();
			if(result){
				mp_error(("media_player_video_bufrelease_smtd=0x%08x",result));
				return result;
			}
		}
	}
	//if audio pcm buffer use SRAM, need not free. Amanda Deng 2007/10/31
	//for(i=pthis->audio_data.dst.buffer_count-1;i>=0;i--)
	//	mp_free((void*)pthis->audio_data.dst.buffer[i].buffer_address);
//	for(i=pthis->audio_data.dst.buffer_count-1;i>=0;i--)
//		mp_free_h2d((void*)pthis->audio_data.dst.buffer[i].buffer_address);
//	for(i=pthis->audio_data.src.buffer_count-1;i>=0;i--)
//		mp_free_h2d((void*)pthis->audio_data.src.buffer[i].buffer_address);
		
	return MP_SUCCESS;
}

#if 0
/*******************************************************************************************
Description:
	read video data to src buffer(src buffer is pingpang buffer)
Parameters:
	MP_FILE* pthis: player file handle
	MP_U32 buffer_index: src buffer index
Return:	
	MP_SUCCESS: read data ok
	Others: parser->read() function result
NOTE: 
       this function call in file  thread
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.09.17		create first version
********************************************************************************************/
MP_U32 media_player_video_read_frames(VIM_MEDIA_PLAYER_FILE* pfile, MP_U32 buffer_index)
{
	MP_FILE			*mpfile;
	void 		  	*parserinfor;
	me_parser_t	  	*parser;
	video_stream_infor	*buffer;
	MEDIA_FIFO  		frmduration;
	MP_U32  			result;
#if VIDEO_FRAMES_READ_TIME_OUTPUT
	MP_U32  		s_time, e_time;
#endif
	result = media_player_check_filehandle(pfile);
	if(result)			
		return result;
	if(pfile->f_playcontrol & MP_VIDEO_STREAM_END)
		return MP_SUCCESS;	
	mpfile = pfile->mp_filehandle;
	parserinfor = mpfile->parserinfor;
	parser	   = mpfile->parser;
	buffer	   = &mpfile->video_data.src.buffer[buffer_index];
	if(buffer->isnot_empty)
		return MP_SUCCESS;	
#if VIDEO_FRAMES_READ_TIME_OUTPUT
	s_time = get_system_time_ticks();	//(us) 
#endif
	if(!pfile->f_playmode)
		result = parser->read_video_keyframes(parserinfor, buffer);
	else 
		result = parser->read_video_frames(parserinfor, buffer);
#if VIDEO_FRAMES_READ_TIME_OUTPUT
	e_time = get_system_time_ticks();	//(us) 
	mp_debug(("read video buffer = %d, read_bytes = %d, frames = %d, time = %d ", 
		             buffer_index, buffer->data_length, buffer->frame_count,e_time-s_time));
#endif
	if(buffer->data_length+4<buffer->buffer_size)
		mp_memset_h2d((char *)(buffer->buffer_address)+buffer->data_length,3,4);

	//form here this is important for video and audio syn Amanda.Deng  2007/10/25
	frmduration.address = (char*)mpfile->video_data.src.duration;
	frmduration.pread = mpfile->video_data.src.duration_read*4;
	frmduration.pwrite = mpfile->video_data.src.duration_write*4;
	frmduration.size =MAX_VIDEO_FRAMECNT_IN_BUFFER*4*4;
	mp_writetofifo(&frmduration, (const char*)buffer->frame_duration, buffer->frame_count*4);
	mpfile->video_data.src.duration_write += buffer->frame_count;
	if(mpfile->video_data.src.duration_write>=MAX_VIDEO_FRAMECNT_IN_BUFFER*4)
	{
		//mp_print(("WRITE: %d!", mpfile->video_data.src.duration_write));
		mpfile->video_data.src.duration_write = mpfile->video_data.src.duration_write-MAX_VIDEO_FRAMECNT_IN_BUFFER*4;
	}
	//end here this is important for video and audio syn Amanda.Deng  2007/10/25
	buffer->cursor_read	= 0;	
	buffer->isnot_empty 	= buffer->data_length;
	buffer->is_over 		= result;
	if(result)
	{	//if read file error, end video play
		pfile->f_playcontrol |= MP_VIDEO_STREAM_END;
		mp_debug(("video data read end, file status = 0x%08X ", pfile->f_playcontrol ));
	}
	return result;
}

#endif
/*******************************************************************************************
Description:
	media player audio buffer src and out buffer filled()
Parameters:
	MP_FILE* pthis: player file handle
Return:	
	MP_SUCCESS: filled audio stream buffer and pcm buffer ok
	Others:		 error code
NOTE: 
       this function call in file thread(fill data after seek or start play)	
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.04.02		create first version
********************************************************************************************/
MP_U32 media_player_audio_buffer_filled(VIM_MEDIA_PLAYER_FILE* pfile)
{
	MP_U32 result,times;
	MP_U32 r_cur,w_cur;
	MP_U32 buffercount=0;
	MP_U32 audiostatus=0, filestatus=0;
	MP_U32 statusflag=0;
	MP_FILE * mpfile=NULL;

	//init status
	mpfile = pfile->mp_filehandle;
	mpfile->audio_data.src.buffer_read = mpfile->audio_data.src.buffer_write;
	//wait for read2core operate over
	while (1) {
		MMD_BIU_Get_Device_Status(IDX_FILE_STATUS, &filestatus);
		if (!(filestatus & 0x1))
			break;
		MMD_USER_WaitMICRS(50);//ms
	}
	//clear error bits (high 16 bits) of file operation
	filestatus &= 0xFFFF;
	MMD_BIU_Set_Device_Status(IDX_FILE_STATUS, filestatus);
		
	MMD_BIU_Get_Device_Status(IDX_AUD_STATUS, &audiostatus);
	audiostatus |= (0xffffffff>>(32-mpfile->audio_data.src.buffer_count));//audio buffer status bit 0~2/4, default 1 empty.
	MMD_BIU_Set_Device_Status(IDX_AUD_STATUS, audiostatus);
	//fill buffer
	r_cur = mpfile->audio_data.src.buffer_read;	//just flag for read audio data, do not need to cooperate with 848 
	w_cur = mpfile->audio_data.src.buffer_write;	//need to cooperate with 848
	while((buffercount<mpfile->audio_data.src.buffer_count) && (!(pfile->f_playcontrol&MP_AUDIO_STREAM_END))) {
		mpfile->audio_data.src.buffer[w_cur].isnot_empty	= 0;
		mpfile->audio_data.src.buffer[w_cur].data_length 	= 0;
		mpfile->audio_data.src.buffer[w_cur].cursor_read  = 0;
		mpfile->audio_data.src.buffer[w_cur].cursor_write = 0;
		result = media_player_audio_read_frames(pfile,w_cur);
		if(result>=ME_FILE_END_OF_AUDIO) {
			pfile->f_playcontrol |= MP_AUDIO_READ_ERROR;
			return result;
		}
		
		if(pfile->fs_filestore==FILE_IN_CORE_FILE) {
			if(!result)
				statusflag |= (1<<w_cur);
		} else {
			result = media_player_audio_fillsrcbuf_smtd(w_cur,mpfile->audio_data.src.buffer[w_cur].data_length, mpfile->audio_data.src.buffer[w_cur].cursor_read, mpfile->audio_data.src.buffer[w_cur].is_over);
			if(result) {
				MMD_ERROR_Deal(CMD_FAILED,MAJ_PLY);
				pfile->f_playcontrol |= MP_AUDIO_STREAM_END;
				mp_error(("audio data read error when media_player_audio_fillsrcbuf_smtd()=0x%08x, file status = 0x%08X ", result, pfile->f_playcontrol ));
				return result;
			} 
		}
		w_cur++;
		if(w_cur>=mpfile->audio_data.src.buffer_count)
			w_cur = 0;
		buffercount++;
	}
	if(pfile->fs_filestore==FILE_IN_CORE_FILE) {
		result = MMD_FILE_StartRead2Core();
		if(result) {
			MMD_ERROR_Deal(CMD_FAILED,MAJ_PLY);
			mp_error(("MMD_FILE_StartRead2Core = 0x%08x\n",result));
			return result;
		}
		times = 100;
		while(times) {
			MMD_USER_WaitMICRS(50);//ms
			MMD_BIU_Get_Device_Status(IDX_FILE_STATUS, &filestatus);
			if(filestatus >> 16) {
				mp_error(("ME_ERR_READ_FILE_ERROR in read audio data! error code = 0x%x", (filestatus >> 16)));
				return ME_ERR_READ_FILE_ERROR;
			}
			MMD_BIU_Get_Device_Status(IDX_AUD_STATUS, &audiostatus);
			if(!(audiostatus & statusflag)){//all audio buffers have filled
				if(!(filestatus & 0x1))//read2core operate over
					break;
			}
			times--;
		}	
		if(!times)
		{
			mp_printf("Load Buffer Out Of One Sencond!\n");
		}
		buffercount = 0;
		while(buffercount<mpfile->audio_data.src.buffer_count) {		
			result = mpfile->parser->read_audio_frames_post(mpfile->parserinfor,&(mpfile->audio_data.src.buffer[r_cur]));
			if(result) {
				
				pfile->f_playcontrol |= MP_AUDIO_STREAM_END;
				mp_error(("read_audio_frames_post=0x%08x",result));
				return result;
			}	
			result = media_player_audio_fillsrcbuf_smtd(r_cur,mpfile->audio_data.src.buffer[r_cur].data_length, mpfile->audio_data.src.buffer[r_cur].cursor_read, mpfile->audio_data.src.buffer[r_cur].is_over);
			if(result) {
				MMD_ERROR_Deal(CMD_FAILED,MAJ_PLY);
				pfile->f_playcontrol |= MP_AUDIO_STREAM_END;
				mp_error(("audio data read error when media_player_audio_fillsrcbuf_smtd()=0x%08x, file status = 0x%08X ", result, pfile->f_playcontrol ));
				return result;
			} 
			r_cur++;
			if(r_cur>=mpfile->audio_data.src.buffer_count)
				r_cur = 0;
			buffercount++;
		}
	}
	mpfile->audio_data.src.buffer_write = w_cur;
	mpfile->audio_data.src.buffer_read = r_cur;
	
	result = media_player_audio_decstart_smtd();
	if(result)
	{
		MMD_ERROR_Deal(CMD_FAILED,MAJ_PLY);
		pfile->f_playcontrol |= MP_AUDIO_DECODE_ERROR;
		return result;
	}
	result = media_player_audio_fillpcmbuf_smtd();
	if(result)
	{
		MMD_ERROR_Deal(CMD_FAILED,MAJ_PLY);
		pfile->f_playcontrol |= MP_AUDIO_DECODE_ERROR;
		return result;
	}

	//need to...
	MMD_BIU_Get_Device_Status(IDX_AUD_STATUS, &audiostatus);
	buffercount = 0;
	statusflag = 0;
	while((audiostatus & (1<<w_cur)) && (buffercount<mpfile->audio_data.src.buffer_count) && (!(pfile->f_playcontrol&MP_AUDIO_STREAM_END))) {
		mpfile->audio_data.src.buffer[w_cur].isnot_empty	= 0;
		mpfile->audio_data.src.buffer[w_cur].data_length 	= 0;
		mpfile->audio_data.src.buffer[w_cur].cursor_read  = 0;
		mpfile->audio_data.src.buffer[w_cur].cursor_write = 0;
		result = media_player_audio_read_frames(pfile,w_cur);
		if(result>ME_FILE_END_OF_AUDIO) {
			pfile->f_playcontrol |= MP_AUDIO_READ_ERROR;
			return result;
		}
		if(pfile->fs_filestore==FILE_IN_CORE_FILE) {
			statusflag |= (1<<w_cur);
		} else {
			result = media_player_audio_fillsrcbuf_smtd(w_cur,mpfile->audio_data.src.buffer[w_cur].data_length, mpfile->audio_data.src.buffer[w_cur].cursor_read, mpfile->audio_data.src.buffer[w_cur].is_over);
			if(result) {
				MMD_ERROR_Deal(CMD_FAILED,MAJ_PLY);
				pfile->f_playcontrol |= MP_AUDIO_STREAM_END;
				mp_error(("audio data read error when media_player_audio_fillsrcbuf_smtd()=0x%08x, file status = 0x%08X ", result, pfile->f_playcontrol ));
				return result;
			} 
		}
		w_cur++;
		if(w_cur>=mpfile->audio_data.src.buffer_count)
			w_cur = 0;
		buffercount++;
	}
	if(pfile->fs_filestore==FILE_IN_CORE_FILE) {
		result = MMD_FILE_StartRead2Core();
		if(result) {
			MMD_ERROR_Deal(CMD_FAILED,MAJ_PLY);
			mp_error(("MMD_FILE_StartRead2Core = 0x%08x\n",result));
			return result;
		}
		times=100;
		while(times) {
			MMD_USER_WaitMICRS(50);//ms
			MMD_BIU_Get_Device_Status(IDX_FILE_STATUS, &filestatus);
			if(filestatus >> 16) {
				mp_error(("ME_ERR_READ_FILE_ERROR in read audio data! error code = 0x%x", (filestatus >> 16)));
				return ME_ERR_READ_FILE_ERROR;
			}
			MMD_BIU_Get_Device_Status(IDX_AUD_STATUS, &audiostatus);
			if(!(audiostatus & statusflag)){//all empty audio buffers have filled
				if(!(filestatus & 0x1))//read2core operate over
					break;
			}
			times--;
		}
		if(!times)
		{
			mp_printf("Load Buffer Out Of One Sencond!\n");
		}
		while(!(audiostatus&(1<<r_cur)) && buffercount)	{		
			result = mpfile->parser->read_audio_frames_post(mpfile->parserinfor,&(mpfile->audio_data.src.buffer[r_cur]));
			if(result) {
				pfile->f_playcontrol |= MP_AUDIO_STREAM_END;
				mp_error(("read_audio_frames_post=0x%08x",result));
				return result;
			}	
			result = media_player_audio_fillsrcbuf_smtd(r_cur,mpfile->audio_data.src.buffer[r_cur].data_length, mpfile->audio_data.src.buffer[r_cur].cursor_read, mpfile->audio_data.src.buffer[r_cur].is_over);
			if(result) {
				MMD_ERROR_Deal(CMD_FAILED,MAJ_PLY);
				pfile->f_playcontrol |= MP_AUDIO_STREAM_END;
				mp_error(("audio data read error when media_player_audio_fillsrcbuf_smtd()=0x%08x, file status = 0x%08X ", result, pfile->f_playcontrol ));
				return result;
			} 
			r_cur++;
			if(r_cur>=mpfile->audio_data.src.buffer_count)
				r_cur = 0;
			buffercount--;
		}
	}
	mpfile->audio_data.src.buffer_write = w_cur;
	mpfile->audio_data.src.buffer_read = r_cur;

	pfile->f_playcontrol |= MP_AUDIO_PCM_READY;
	return MP_SUCCESS;
}
/*******************************************************************************************
Description:
	media player video buffer src and out buffer filled()
Parameters:
	MP_FILE* pthis: player file handle
Return:	
	MP_SUCCESS: filled video stream buffer and pcm buffer ok
	Others:		 error code
NOTE: 
       this function call in file thread(fill data after seek or start play)	
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.04.02		create first version
********************************************************************************************/
MP_U32 media_player_video_buffer_filled(VIM_MEDIA_PLAYER_FILE* pfile)
{
	MP_U32   i, result;
	MP_U32 videostatus=0;
	//MP_U32 audiostatus=0;
	MP_U32 filestatus=0;
	MP_U32 statusflag=0;
	VID_BUF_DATA BufData_Vid;
	
	MP_FILE *mpfile = pfile->mp_filehandle;
	//initialize video frames duration 
	mpfile->video_data.src.duration_read = 0;
	mpfile->video_data.src.duration_write = 0;
	mp_memset((unsigned char*)mpfile->video_data.src.duration, 0, MAX_VIDEO_FRAMECNT_IN_BUFFER*4*4);
	//fill video stream buffer
	mpfile->video_data.src.buffer_read = 0;
	mpfile->video_data.src.buffer_write =0;
	
	if(mpfile->video_infor.v_isvalid==1){
		MMD_BIU_Set_Device_Status(IDX_VID_STATUS,0);//0x0000ffff);//all video buffer is empty(1),and the low 16 bits is status of buffer.
		do {//wait for read2core operate over
			MMD_BIU_Get_Device_Status(IDX_FILE_STATUS, &filestatus);
			MMD_USER_WaitMICRS(50);//ms
		} while (filestatus & 0x1);		
	}

	for(i=0;i<mpfile->video_data.src.buffer_count && (!(pfile->f_playcontrol & MP_VIDEO_STREAM_END));i++)
	{
		mpfile->video_data.src.buffer[i].isnot_empty=0;
		mpfile->video_data.src.buffer[i].data_length=0;
		mpfile->video_data.src.buffer[i].cursor_read=0;
		mpfile->video_data.src.buffer[i].cursor_write=0;
		mpfile->video_data.src.buffer[i].is_over=0;
		result = media_player_video_read_frames(pfile, i);
		if(result>ME_FILE_END_OF_VIDEO)
		{
			pfile->f_playcontrol |= MP_VIDEO_READ_ERROR;
			return result;
		}	
		if(pfile->fs_filestore==FILE_IN_CORE_FILE) {
			if(mpfile->video_infor.v_isvalid==1){
				statusflag |= (1<<i);
			}
		}
	}
	if(pfile->fs_filestore==FILE_IN_CORE_FILE) {
		if(mpfile->video_infor.v_isvalid==1){
			result = MMD_FILE_StartRead2Core();
			if(result) {
				MMD_ERROR_Deal(CMD_FAILED,MAJ_PLY);
				mp_error(("MMD_FILE_StartRead2Core = 0x%08x\n",result));
				return result;
			}
			while(1) {
				MMD_USER_WaitMICRS(50);//ms
				MMD_BIU_Get_Device_Status(IDX_FILE_STATUS, &filestatus);
				if(filestatus >> 16) {
					mp_error(("ME_ERR_READ_FILE_ERROR in read video data! error code = 0x%x", (filestatus >> 16)));
					return ME_ERR_READ_FILE_ERROR;
				}
				MMD_BIU_Get_Device_Status(IDX_VID_STATUS, &videostatus);
				if(!(videostatus & statusflag))
					if(!(filestatus & 0x1))//read2core operate over
						break;
			}
			for(i=0;i<mpfile->video_data.src.buffer_count;i++)
			{
				result = mpfile->parser->read_video_frames_post(mpfile->parserinfor,&(mpfile->video_data.src.buffer[i]));
				if(result) {
					mp_error(("read_video_frames_post=0x%08x",result));
					return result;
				}
				mpfile->video_data.src.buffer[i].isnot_empty	= mpfile->video_data.src.buffer[i].data_length;
			}
		}
	}
	
	if(mpfile->video_infor.v_isvalid==1){
		for(i=0;i<mpfile->video_data.src.buffer_count;i++)
		{
			BufData_Vid.idx = i;
			BufData_Vid.cursor_read = mpfile->video_data.src.buffer[i].cursor_read;
			BufData_Vid.datalength = mpfile->video_data.src.buffer[i].data_length;
			BufData_Vid.lastblock = mpfile->video_data.src.buffer[i].is_over;
			result = media_player_video_fillsrcbuf_smtd(&BufData_Vid);
			if(result){
				MMD_ERROR_Deal(CMD_FAILED,MAJ_PLY);
				mp_error(("media_player_video_fillsrcbuf_smtd=0x%08x",result));
				return result;
			}
		}	
	}
	
	pfile->f_playcontrol |= MP_VIDEO_STREAM_READY;
	return MP_SUCCESS;;
}

/*******************************************************************************************
Description:
	media player audio start playing(start DMA transfer)
Parameters:
	MP_FILE* pthis: player file handle
Return:	
	MP_SUCCESS: start ok
	Others:		 error code
NOTE: 
       this function maybe call in all thread
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.04.02		create first version
********************************************************************************************/
MP_U32 media_player_audio_start(VIM_MEDIA_PLAYER_FILE* pfile)
{
	MP_U32 result;



	media_sysstatus->ply_audiocounter++;
	result = media_player_audio_dmastart_smtd();
	if(result) {
		MMD_ERROR_Deal(CMD_FAILED,MAJ_PLY);
		mp_debug(("media_player_audio_dmastart_smtd=0x%08x",result));
		return result;
	}
	media_sysstatus->ply_dmastatus = MEDIA_DMA_TRANSFER;
	pfile->f_playtime = pfile->a_time;			//(ms)	
	pfile->f_basetime = MMD_SYS_GetTimer2Value();//us//get_system_time_ticks();
	
	return MP_SUCCESS;
}

/*******************************************************************************************
Description:
	media player video start playing(start video output)
Parameters:
	MP_FILE* pthis: player file handle
Return:	
	MP_SUCCESS: start ok
	Others:		 error code
NOTE: 
       this function maybe call in all thread
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.04.02		create first version
	   0.2			Dang Ningna			2008.04.11		second version for 848  *not start video_sync_timer
********************************************************************************************/
extern MP_U32   flg_vdecode;//video decoder flag
extern MP_U32   flg_vread;//video read flag
MP_U32 media_player_video_start(VIM_MEDIA_PLAYER_FILE* pfile)
{
	MP_U32 	read_buffer, write_buffer;
	MP_U32	result;
	MP_FILE* mpfile;

	media_sysstatus->ply_videocounter++;
	mpfile = pfile->mp_filehandle;
	//get current decode buffer index
	read_buffer = mpfile->video_data.src.buffer_read;
	write_buffer = mpfile->video_data.dst.buffer_write;

	flg_vdecode = 0;//video decoder flag
	flg_vread = 0;//video read flag
	
	//start video playing
	result = mpfile->video_decoder->start(mpfile->video_decinfor);
	if(result)
	{
		mp_error(("video_decoder->start() retrun =0x%08X",result));
		//MSG_FATAL("video_decoder->start() retrun =0x%x",result, 0, 0);
		return result;
	}
	if(mpfile->video_infor.v_isvalid>=1)
	{	//hardware decode video(3gp, mp4, jpg) 
		result = media_player_video_decstart_smtd();
		if(result){
			MMD_ERROR_Deal(CMD_FAILED,MAJ_PLY);
			mp_error(("media_player_video_decstart_smtd() =0x%08X",result));
			return result;
		}
		mpfile->video_decoder->decode_frames((void*)mpfile->video_decinfor,
									     	     (void*)&mpfile->video_data.src.buffer[read_buffer],
									            (void*)&mpfile->video_data.dst.buffer[write_buffer]);
		if(result)
		{
			mp_error(("video_decoder->decode_frames() retrun =0x%08X",result));
			return result;
		}
	}
	else
	{	//software decode video(jpg,rmvb, jif, bmp, png)
#if 0
		msg.CMD = MSG_MAJ_PLY|MIN_PLY_VIDEO_DECODE_FRAMES;
		msg.OP1 = (MMD_U32)pfile;
		msg.OP2 = 0;
		#ifdef MESSAGE_PROCESS_IN_MEFILETHREAD
			mmd_send_mefile_msg(&msg, 1);
		#else
			mmd_send_msg_file(&msg, 1);
		#endif
#endif		
       }
	//update playcontrol status
	pfile->f_playcontrol |= MP_VIDEO_YUV_READY;
	pfile->f_basetime = MMD_SYS_GetTimer2Value();//us//get_system_time_ticks();	 
	return MP_SUCCESS;
}

/*******************************************************************************************
Description:
	media player audio stop playing(stop DMA transfer)
Parameters:
	MP_FILE* pthis: player file handle
Return:	
	MP_SUCCESS: stop ok
	Others:		 error code
NOTE: 
       this function maybe call in all thread
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.04.02		create first version
********************************************************************************************/
extern MP_U32 flg_aread;//audio read flag
MMD_U32 rotateStauts=0;

MP_U32 media_player_audio_stop(VIM_MEDIA_PLAYER_FILE* pfile)
{
	MP_U32	result= 0;
	
	result = media_player_audio_decstop_smtd();
	//if(result)
	//	MMD_ERROR_Deal(CMD_FAILED,MAJ_PLY);
	media_sysstatus->ply_audiocounter--;
	pfile->mp_filehandle->audio_data.src.buffer_write = 0;//by dnn 2008/5/15
	flg_aread = 0;//audio read flag


//: Add by antonliu, for volume adjust
	if(((pfile->ring_info >> 16)&0xffff) == VOL_ADJUST_START)	//	Need adjust
	{
		if(g_vol_ctl_enable == 2)
		{
			pfile->ring_info = ((MP_U32)(VOL_ADJUST_OVER) << 16) & 0xffff0000;	//	Set to Adjust over
			pfile->ring_info |= (g_vol_ctl_gain & 0x0000ffff);	//	Set adjust gain
			//me_printf("media_player_audio_stop, SET pfile->ring_info=0x%x\n", pfile->ring_info);
		}
	}
	
	return result;
}

/*******************************************************************************************
Description:
	media player video stop playing(stop video output)
Parameters:
	MP_FILE* pthis: player file handle
Return:	
	MP_SUCCESS: stop ok
	Others:		 error code
NOTE: 
       this function maybe call in all thread
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.04.02		create first version
********************************************************************************************/
MP_U32 media_player_video_stop(VIM_MEDIA_PLAYER_FILE* pfile)
{
	MP_FILE* mpfile = pfile->mp_filehandle;
	MP_U32	result = 0;
	
	result = mpfile->video_decoder->stop(mpfile->video_decinfor);
	media_sysstatus->ply_videocounter--;

	return result;
}

/*******************************************************************************************
Description:
	media player start video fastplay
Parameters:
	MP_FILE* pthis: player file handle
Return:	
	MP_SUCCESS: start fastplaying ok
	Others:		 error code
NOTE: 
       this function call in file thread(start fastplay)	
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.04.02		create first version
********************************************************************************************/
MP_U32 media_player_start_fastplay(VIM_MEDIA_PLAYER_FILE* pfile)
{
	MP_U32 result;
	MP_FILE*	mpfile = pfile->mp_filehandle;
	// initialize media file src, dst buffer
	if(!pfile->f_playbufinit)
	{
		pfile->f_playbufinit = 1;
		result = media_player_buffer_initialize(pfile->mp_filehandle);
		if(result)
		{
			mp_error(("media_player_buffer_initialize() retrun =0x%08X",result));
			return result;
		}
	}
	//check video last play status
	if(pfile->v_status == MEDIA_VIDEO_PAUSE)
	{
		pfile->f_playtime = pfile->v_time;			//(ms)	
		pfile->f_basetime = MMD_SYS_GetTimer2Value();//us//get_system_time_ticks();	//(us) 
		pfile->v_status = MEDIA_VIDEO_PLAYING;
		// video last status is pause, so resume it
		result = mpfile->video_decoder->resume(mpfile->video_decinfor);
		if(result)
		{
			mp_error(("video_decoder->resume() retrun =0x%08X",result));
			return result;
		}
		// start syn timer
		media_player_timer_start();
		// set file status
		pfile->f_playstatus = MEDIA_FILE_FAST_PLAYING;
		mp_debug(("media_sysstatus->ply_audiocounter = %d ", media_sysstatus->ply_audiocounter));
		mp_debug(("file status = 0x%08X ", pfile->f_playcontrol ));
	}
	else if(pfile->v_status == MEDIA_VIDEO_STOP)
	{
		pfile->f_playcontrol|= MP_VIDEO_PLAY_END;
		if(mpfile->video_infor.v_isvalid)
		{
			pfile->f_playcontrol &= ~ MP_VIDEO_PLAY_END;
			pfile->v_countbak = 0;
			pfile->v_duration = 0;
			pfile->v_lastdone = 0;
			pfile->v_samples = 1;
			pfile->v_framesdone = 1;
			result = media_player_video_buffer_filled(pfile);
			if(result)
			{
				mp_error(("media_player_video_buffer_filled() retrun =0x%08X",result));
				return result;
			}
			else
			{
				pfile->f_playtime = pfile->v_time;			//(ms)	
				pfile->f_basetime = MMD_SYS_GetTimer2Value();//us//get_system_time_ticks();	//(us) 
				//start playing video 
				result = media_player_video_start(pfile);
				if(result)
				{
					mp_error(("media_player_video_start() retrun =0x%08X",result));
					return result;
				}
				media_player_timer_start();//by dnn 2008/4/11
				pfile->v_status = MEDIA_VIDEO_PLAYING;
				// set file status
				pfile->f_playstatus = MEDIA_FILE_FAST_PLAYING;
				mp_debug(("media_sysstatus->ply_audiocounter = %d ", media_sysstatus->ply_audiocounter));
				mp_debug(("file status = 0x%08X ", pfile->f_playcontrol ));
			}
		}
	}
	return MP_SUCCESS;
}
/*******************************************************************************************
Description:
	media player start video playing
Parameters:
	MP_FILE* pthis: player file handle
Return:	
	MP_SUCCESS: start playing ok
	Others:		 error code
NOTE: 
       this function call in file thread(start playing)	
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.04.02		create first version
	   0.2			Dang Ningna			2008.04.11		second version for 848  *for media_player_timer
********************************************************************************************/
MP_U32 media_player_start_play(VIM_MEDIA_PLAYER_FILE* pfile)
{
	MP_U32	accesspos, asscessframe, result;
	MP_FILE*	mpfile = pfile->mp_filehandle;
#if ME_AUDIO_POSTPROC_ENABLE
	me_node_t		*pnod	= NULL;
	me_effectproc_t *effectproc= NULL;
#endif
	//add by yjz 2009/2/11
	SEEK_INFO* SeekInfo;
	VM_MIDI_PARSER_INFO *parsefile;
	///:~

	if(((pfile->ring_info >> 16) & 0xffff) == VOL_ADJUST_START)	//	Ring file , need preplay
	{
		me_printf("\nThis is a ring file, <0x8000>!!!\n");
		g_vol_ctl_enable = 1;
		g_vol_ctl_gain = 0;	//	Adjust gain value
	}
	else if(((pfile->ring_info >> 16) & 0xffff) == VOL_ADJUST_OVER)	//	Ring file, adjust over
	{
		me_printf("\nThis is a ring file, <0x8001>!!!\n");
		g_vol_ctl_enable = 2;
		g_vol_ctl_gain = pfile->ring_info & 0xffff;	
	}
	else	//	Not ring file
	{
		g_vol_ctl_enable = 0;
		g_vol_ctl_gain = 0;
	}
	rotateStauts=0;
	
	// initialize media file src, dst buffer
	if(!pfile->f_playbufinit)
	{
		pfile->f_playbufinit = 1;
		result = media_player_buffer_initialize(pfile->mp_filehandle);
		if(result)
		{
			mp_error(("media_player_buffer_initialize() retrun =0x%08X",result));
			//MSG_FATAL("media_player_buffer_initialize() retrun =0x%x",result, 0, 0);
			return result;
		}
	}
	//check file last play status
	if(pfile->v_status==MEDIA_VIDEO_PLAYING)
	{
		if(!pfile->f_playmode)
		{	//only I vop frame playing
			pfile->v_status = MEDIA_VIDEO_STOP;
			result = media_player_video_stop(pfile);			
			if(result)
			{
				mp_error(("media_player_video_stop() retrun =0x%08X",result));
				return result;
			}
			media_player_timer_stop();
			
			//only I vop stream playing
			result = mpfile->parser->seek(mpfile->parserinfor,
								          pfile->v_time,
								         &accesspos,
								  	   	 &asscessframe);
			if(result)
			{
				mp_error(("parser->seek() retrun =0x%08X",result));
				return result;
			}
			pfile->v_frames = asscessframe;
			pfile->v_time = pfile->a_time = accesspos;
		}
		else 
		{	//speed is more times as normal, such as 1, 2,3,4,5,6,7,8,9
			pfile->v_status = MEDIA_VIDEO_PAUSE;
			result = mpfile->video_decoder->pause(mpfile->video_decinfor);
			if(result)
			{
				mp_error(("video_decoder->pause() retrun =0x%08X",result));
				return result;
			}
			
			media_player_timer_stop();
			result = mpfile->parser->seek_audio_syntime(mpfile->parserinfor,
									            pfile->v_time);
			if(result)
			{
				mp_error(("parser->seek_audio_syntime() retrun =0x%08X",result));
				return result;
			}
		}
		pfile->a_duration	= 0;
		pfile->a_samples	= 0;
		if(mpfile->audio_infor.a_isvalid)
		{
			pfile->a_time = pfile->v_time;	
			if(pfile->mp_filetype == VIM_MEDIA_FILE_TYPE_MIDI)			
			{
				parsefile = pfile->mp_filehandle->parserinfor;
				SeekInfo = (SEEK_INFO*)(parsefile->vm_input.SeekInfo);
				result = media_player_audio_decseek_smtd((MP_U32)SeekInfo,sizeof(SEEK_INFO));
			}
			else
			{
				result = media_player_audio_decseek_smtd(0,0);				
			}		
			if(result)
			{
				MMD_ERROR_Deal(CMD_FAILED,MAJ_PLY);
				mp_error(("media_player_audio_decseek_smtd() retrun =0x%08X",result));
				return result;
			}
			#if ME_AUDIO_POSTPROC_ENABLE
			pnod = mpfile->audio_postproc->first;
			while(pnod)
			{
				effectproc = (me_effectproc_t*)pnod->content;
				effectproc->reset(effectproc);
				pnod = pnod->next;
			}
			#endif
		}
	}
	//set video normal playing mode
	pfile->f_playmode = 1;
	//fill audio stream buffer and pcm buffer
	if(pfile->a_status == MEDIA_AUDIO_STOP)
	{
		pfile->f_playcontrol|= MP_AUDIO_PLAY_END;
		if(mpfile->audio_infor.a_isvalid)
		{		
			pfile->f_playcontrol &= ~ MP_AUDIO_PLAY_END;
			pfile->a_dmasamples = 0;
			pfile->a_lastleftsmpls = 0;
			result = media_player_audio_buffer_filled(pfile);
			if(result)
			{
				mp_error(("media_player_audio_buffer_filled() retrun =0x%08X",result));
				return result;
			}
			mp_debug(("###media_player_audio_buffer_filled() OK retrun =0x%08X",result));

		}
	}
	//start video and audio at same time
	if(pfile->v_status == MEDIA_VIDEO_PAUSE)
	{
		mp_debug(("###pfile->v_status = MEDIA_VIDEO_PAUSE"));
	//video last status is pause, means last status is fastplaying mode, so resume playing video
		pfile->f_playtime  = pfile->v_time;				//(ms)	
		pfile->f_basetime = MMD_SYS_GetTimer2Value();//us//get_system_time_ticks();	//(us) 
		result = mpfile->video_decoder->resume(mpfile->video_decinfor);
		if(result)
		{
			mp_error(("video_decoder->resume() retrun =0x%08X",result));
			return result;
		}

		media_player_timer_start();
		pfile->f_playstatus = MEDIA_FILE_PLAYING;
		pfile->v_status = MEDIA_VIDEO_PLAYING;

		//if have audio, start playing audio at same time
		if(mpfile->audio_infor.a_isvalid)
		{
			// start audio playing, update audio and file status
			pfile->a_status = MEDIA_AUDIO_PLAYING;
			result = media_player_audio_start(pfile);
			if(result) {
				mp_debug(("media_player_audio_start=0x%08x",result));
				return result;
			}
		}
	}
	else if(pfile->v_status == MEDIA_VIDEO_STOP)
	{
		mp_debug(("###pfile->v_status = MEDIA_VIDEO_STOP"));
	//video last status is stop, filled video data first
		pfile->f_playcontrol|= MP_VIDEO_PLAY_END;
		if(mpfile->video_infor.v_isvalid)
		{
			mp_debug(("###mpfile->video_infor.v_isvalid"));
		// the file have video track, filled video stream buffer first
			pfile->f_playcontrol &= ~ MP_VIDEO_PLAY_END;
			pfile->v_countbak = 0;
			pfile->v_duration = 0;
			pfile->v_lastdone = 0;
			pfile->v_samples = 1;
			pfile->v_framesdone = 1;
			result = media_player_video_buffer_filled(pfile);
			if(result)
			{
				mp_error(("media_player_video_buffer_filled() retrun =0x%08X",result));
				//MSG_FATAL("media_player_video_buffer_filled() retrun =0x%x",result, 0, 0);
				return result;
			}
			// filled video data ok, start video playing, update player status
			result = media_player_video_start(pfile);
			if(result)
			{
				mp_error(("media_player_video_start() retrun =0x%08X",result));
				//MSG_FATAL("media_player_video_start() retrun =0x%x",result, 0, 0);
				return result;
			}
			pfile->v_status = MEDIA_VIDEO_PLAYING;
			pfile->f_playstatus = MEDIA_FILE_PLAYING;
			
			if(mpfile->video_infor.v_isvalid==1)
			{
				media_player_timer_start();
			}
			else if(mpfile->video_infor.v_isvalid==3)
			{
				media_player_jpgdis_timer_start();
			}
		}
		mp_debug(("media_sysstatus->ply_videocounter = %d ", media_sysstatus->ply_videocounter));
		if(mpfile->audio_infor.a_isvalid)
		{	
			mp_debug(("###mpfile->video_infor.a_isvalid"));
			// filled audio data ok, start audio playing, update audio and file status
			pfile->a_status = MEDIA_AUDIO_PLAYING;
			result = media_player_audio_start(pfile);
			if(result) {
				mp_debug(("media_player_audio_start=0x%08x",result));
				return result;
			}
			mp_debug(("###media_player_audio_start OK"));
			if(!(mpfile->video_infor.v_isvalid)) {
				pfile->f_playstatus = MEDIA_FILE_PLAYING;
				media_player_timer_start();
				mp_debug(("###media_player_timer_start()"));
			}
		}	
		mp_debug(("media_sysstatus->ply_audiocounter = %d ", media_sysstatus->ply_audiocounter));	
		// update audio and file status
		mp_debug(("file status = 0x%08X ", pfile->f_playcontrol ));
	}
	return MP_SUCCESS;
}
/*******************************************************************************************
Description:
	media recorder src and out buffer initalize
Parameters:
	MR_FILE* pthis: recorder file handle
Return:	
	MP_SUCCESS: initialize ok
	Others:			error code
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.07.27		create first version
********************************************************************************************/
MR_U32 media_recorder_buffer_initialize(MR_FILE* pthis)
{
	MR_U32 recvtype, size,ret,dstsize;
	MR_U32 srcbufferlen,dstbufferlen;
	MP_S32 i;
	void	    *encoderinfo;
	
	AUD_BUF_INFO Abuf_Info;
	AUD_BUF_ADDR Abuf_Addr ; 
	

	VID_BUF_INFO Vbuf_Info;
	//VID_FRAME_INFO *pVframe_Info;
	MR_U32 frameinfosize;
		
	if(pthis->audio_infor.a_isvalid)
	{
		encoderinfo= pthis->audio_encinfor;
		//160*2*encoderinfo->input_chn*samr_infor->smp_frm*samr_infor->smp_cnt;	
		//ret=pthis->audio_encoder->src_buffersize(encoderinfo, &size);
		//MMD_REC_Audio_GetBuffSize(&size);
		/*ret=pthis->parser->get_audio_frame_buffersize(parserinfor, &size);
		if(ret)
		{
			mp_error(("src_buffersize() return = 0x%08X",ret));
			return ret;
		}
		mp_print(("pcm buffer size=%d",size));*/
		if((pthis->audio_infor.a_codec[0]=='s')&&(pthis->audio_infor.a_codec[1]=='a')
				&&(pthis->audio_infor.a_codec[2]=='m')&&(pthis->audio_infor.a_codec[3]=='r') ||(pthis->audio_infor.a_codec[0]=='a')
				&&(pthis->audio_infor.a_codec[1]=='m')&&(pthis->audio_infor.a_codec[2]=='r'))
		{
			srcbufferlen=AMR_RECORDER_SRC_BUFFER_LENGTH;
			dstbufferlen=AMR_RECORDER_DST_BUFFER_LENGTH;
		}
		else if((pthis->audio_infor.a_codec[0]=='m')&&(pthis->audio_infor.a_codec[1]=='p')&&(pthis->audio_infor.a_codec[2]=='3'))//lily added mp3 support
		{
			srcbufferlen=MP3_RECORDER_SRC_BUFFER_LENGTH;
			dstbufferlen=MP3_RECORDER_DST_BUFFER_LENGTH;
		}
		size = 8*1024;
		size += 3;
		size &= 0xfffffffc;

		pthis->audio_data.src.buffer_count = AUDIO_RECORD_SRC_BUFFER;
		
		Abuf_Info.SRC_SIZE =srcbufferlen+1920;//9216+1920;// 8000+1920;//16000;//size;
		Abuf_Info.SRC_NUM = AUDIO_RECORD_SRC_BUFFER;
		
		size += 3;
		size &= 0xfffffffc;
		pthis->audio_data.dst.buffer_count = AUDIO_RECORD_DST_BUFFER;

		dstsize = 8*1024;	
		
		Abuf_Info.AUDIO_CODEC = (pthis->audio_infor.a_codec[0])|(pthis->audio_infor.a_codec[1]<<8)|
							(pthis->audio_infor.a_codec[2]<<16)|(pthis->audio_infor.a_codec[3]<<24);
		
		
		Abuf_Info.PCM_SIZE =dstbufferlen;//dstsize;//dstsize;
		Abuf_Info.PCM_NUM = AUDIO_RECORD_DST_BUFFER;
		mp_printf("Abuf_Info.SRC_SIZE(%d) ...............\n",Abuf_Info.SRC_SIZE);
		ret = media_recorder_audio_bufinit_smtd(&Abuf_Info, &Abuf_Addr);
		if(ret) {
			MMD_ERROR_Deal(CMD_FAILED,MAJ_REC);
			mp_error(("media_recorder_audio_bufinit_smtd() return = 0x%08X",ret));
			return MP_ERR_MALLOC_MEMORY_ERROR;
		}
		for(i=0;i<AUDIO_RECORD_SRC_BUFFER;i++)
		{
			pthis->audio_data.src.buffer[i].buffer_address = Abuf_Addr.SRC_ADDR[i];
			pthis->audio_data.src.buffer[i].buffer_size =srcbufferlen;//8000;//16000;//size;
		}
		for(i=0;i<AUDIO_RECORD_DST_BUFFER;i++)
		{
			pthis->audio_data.dst.buffer[i].buffer_address = Abuf_Addr.PCM_ADDR[i];
			pthis->audio_data.dst.buffer[i].buffer_size = dstbufferlen;//dstsize;
		}
		pthis->audio_infor.a_others = NULL;
		pthis->audio_infor.a_otherslen = 0;
		mp_printf("audio_inputinfor(%d)!!!!!!!!!!! \n",pthis->audio_inputinfor.a_inputsmplrate);
		ret = media_recorder_audio_encinit_smtd(&(pthis->audio_infor), &(pthis->audio_inputinfor), pthis->audio_infor.a_otherslen);
		if(ret) {
			MMD_ERROR_Deal(CMD_FAILED,MAJ_REC);
			ME_ERROR(("media_recorder_audio_encinit_smtd() return = 0x%08X", ret));
			return ret;
		}
		g_bufferindex_audio=0;
		MMD_BIU_Set_Device_Status(IDX_AUD_STATUS,0);
	}
	
	if(pthis->video_infor.v_isvalid)
	{
		encoderinfo= pthis->video_encinfor;
		ret=pthis->video_encoder->src_buffersize(encoderinfo, &size);
		if(ret)
		{
			mp_error(("src_buffersize() return = 0x%08X",ret));
			return ret;
		}
		pthis->video_data.src.buffer_count = VIDEO_RECORD_SRC_BUFFER;
		for(i=0;i<pthis->video_data.src.buffer_count;i++)
			pthis->video_data.src.buffer[i].buffer_size = size;
		//mp_print(("yuv buffer size=%d",size));
		ret=pthis->video_encoder->dst_buffersize(encoderinfo, &size);
		if(ret)
		{
			mp_error(("dst_buffersize() return = 0x%08X",ret));
			return ret;
		}
		mp_print(("video stream buffer size=%d",size));
		size += 3;
		size &= 0xfffffffc;
		
		//memory size limited check
		if(size>VIDEO_RECORD_DST_BUFFER_MAXSIZE)
			size = VIDEO_RECORD_DST_BUFFER_MAXSIZE;
		/*TestBuffer=MMD_Malloc(TestBufferSize);
		TestBufferIndex=0;
		if(!TestBuffer)
		{
			mp_error(("MMD_Malloc() TestBuffer failed"));
			return 1;
		}*/
		/*else
		{
			mp_printf("TestBuffer addr is %x\n",TestBuffer);
		}*/
		//video type check then buffer counter set
		mp_memcpy((char*)&recvtype, (const char *)pthis->video_encoder->id, 4);
		if(recvtype==VIM_MEDIA_VIDEO_TYPE_JPG)
	  	{
	  		pthis->video_data.dst.buffer_count = 1;
			for(i=0;i<pthis->video_data.dst.buffer_count;i++)
			{
				pthis->video_data.dst.buffer[i].buffer_size = size;
				pthis->video_data.dst.buffer[i].buffer_address = (MP_U32)mp_malloc(size);
				if(pthis->video_data.dst.buffer[i].buffer_address == NULL)
					return MP_ERR_MALLOC_MEMORY_ERROR;
				else
					mp_memset((unsigned char*)pthis->video_data.dst.buffer[i].buffer_address, 0, size);
			}
		}
		else
 		{
 			pthis->video_data.dst.buffer_count = VIDEO_RECORD_DST_BUFFER;
			for(i=0;i<pthis->video_data.dst.buffer_count;i++)
			{
				pthis->video_data.dst.buffer[i].buffer_size = size;
				pthis->video_data.dst.buffer[i].buffer_address = (MP_U32)mp_malloc_h2d(size);
				if(pthis->video_data.dst.buffer[i].buffer_address == NULL)
					return MP_ERR_MALLOC_MEMORY_ERROR;
				else
					mp_memset_h2d((char*)pthis->video_data.dst.buffer[i].buffer_address, 0, size);
				frameinfosize=sizeof(VID_FRAME_INFO);
				Frame_Info[i]=(MP_U32)mp_malloc_h2d(frameinfosize);
				mp_printf("addr is %x,size is %d\n",Frame_Info[i],frameinfosize);
				if(Frame_Info[i] == NULL)
					return MP_ERR_MALLOC_MEMORY_ERROR;
				else
					mp_memset_h2d((char*)Frame_Info[i], 0, frameinfosize);
			}	
#if VIDEORECORDERIN848			
			Vbuf_Info.SRC_NUM = pthis->video_data.dst.buffer_count;
			Vbuf_Info.SRC_SIZE = size;
			for(i=0;i<pthis->video_data.dst.buffer_count;i++)
			{
				Vbuf_Info.SRC_ADDR[i] = pthis->video_data.dst.buffer[i].buffer_address;
			}
			ret = media_recorder_video_bufinit_smtd(&Vbuf_Info);
			if(ret) 
			{
				MMD_ERROR_Deal(CMD_FAILED,MAJ_REC);
				mp_error(("media_recorder_video_bufinit_smtd() return = 0x%08X",ret));
				return ret;
			}
			
			ret = media_recorder_video_frameinfoinit_smtd(Frame_Info);
			if(ret) 
			{
				MMD_ERROR_Deal(CMD_FAILED,MAJ_REC);
				mp_error(("media_recorder_video_frameinfoinit_smtd() return = 0x%08X",ret));
				return ret;
			}			
			MMD_BIU_Set_Device_Status(IDX_VID_STATUS,0);
			MMD_BIU_Set_Device_Status(5,0);
			MMD_BIU_Set_Device_Status(6,0);
			MMD_BIU_Set_Device_Status(7,0);
			g_bufferindex_video=0;
			endduration=0;
#endif
		}
		
	}
	
	return MP_SUCCESS;
}
/*******************************************************************************************
Description:
	media recorder src and out buffer release(now not use malloc)
Parameters:
	MR_FILE* pthis: recorder file handle
Return:	
	MP_SUCCESS: release ok
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.07.02		create first version
********************************************************************************************/
MR_U32 media_recorder_buffer_release(MR_FILE* pthis)
{
	MP_S32 i;
	MP_U32 result=0;

	if(pthis->video_infor.v_isvalid)
	{
		if(pthis->video_infor.v_isvalid==1)
		{
			/*if(TestBuffer)
			{
				MMD_Free(TestBuffer);
				TestBuffer=0;
			}*/
			for(i=pthis->video_data.dst.buffer_count-1;i>=0;i--)
			{
				mp_free_h2d((void*)pthis->video_data.dst.buffer[i].buffer_address);
				mp_free_h2d((void*)Frame_Info[i]);
				Frame_Info[i]=0;
			}				

#if VIDEORECORDERIN848	
			result = media_recorder_video_bufrelease_smtd();
			if(result){
				MMD_ERROR_Deal(CMD_FAILED,MAJ_REC);
				mp_error(("media_recorder_video_bufrelease_smtd=0x%08x",result));
				return result;
			}
			
			result = media_recorder_video_frameinforelease_smtd();
			if(result){
				MMD_ERROR_Deal(CMD_FAILED,MAJ_REC);
				mp_error(("media_recorder_video_frameinforelease_smtd=0x%08x",result));
				return result;
			}
#endif
		}
		if(pthis->video_infor.v_isvalid==3)
		{
			for(i=pthis->video_data.dst.buffer_count-1;i>=0;i--)
				mp_free((void*)pthis->video_data.dst.buffer[i].buffer_address);
		}		
	}
	if(pthis->audio_infor.a_isvalid) {
		i = media_recorder_audio_bufrelease_smtd();
		if(i){
			MMD_ERROR_Deal(CMD_FAILED,MAJ_REC);
			mp_debug(("MMD_Ply_Audio_BufRelease=0x%08x",i));
			return i;
		}
	}
	
	/*for(i=pthis->audio_data.dst.buffer_count-1;i>=0;i--)
		mp_free((void*)pthis->audio_data.dst.buffer[i].buffer_address);
	for(i=pthis->audio_data.src.buffer_count-1;i>=0;i--)
		mp_free((void*)pthis->audio_data.src.buffer[i].buffer_address);*/

	return MP_SUCCESS;
}



MR_U32 media_recorder_audio_buffer_initialize(VIM_MEDIA_RECORDER_FILE* rfile)
{
	MR_U32   i;
	MR_FILE *mrfile = rfile->mr_filehandle;

	for(i=0;i<mrfile->audio_data.src.buffer_count;i++)
	{
		mrfile->audio_data.src.buffer[i].data_length=0;
		mrfile->audio_data.src.buffer[i].isnot_empty=0;
		mrfile->audio_data.src.buffer[i].cursor_read = 480*4;	//encoder use read cursor
		mrfile->audio_data.src.buffer[i].cursor_write = 480*4;	//dma use write cursor
		mrfile->audio_data.src.buffer[i].is_over = 0;
		//mp_memset((unsigned char*)mrfile->audio_data.src.buffer[i].buffer_address, 0, 
			                                   //   mrfile->audio_data.src.buffer[i].buffer_size);
		//_cpu_dcache_intellectual_clean_and_invalidate();
	}
	mrfile->audio_data.src.buffer_write =0;
	mrfile->audio_data.src.buffer_read=0;
	
	for(i=0;i<mrfile->audio_data.dst.buffer_count;i++)
	{
		mrfile->audio_data.dst.buffer[i].data_length=0;
		mrfile->audio_data.dst.buffer[i].isnot_empty=0;
		mrfile->audio_data.dst.buffer[i].is_over = 0;
		if(mrfile->video_infor.v_isvalid)
			mrfile->audio_data.dst.buffer[i].cursor_write = 0;	//dma use write cursor
		else
			mrfile->audio_data.dst.buffer[i].cursor_write = 6;	//dma use write cursor
		//mp_memset((unsigned char*)mrfile->audio_data.dst.buffer[i].buffer_address, 0, 
			                                      //mrfile->audio_data.dst.buffer[i].buffer_size);

	}
	mrfile->audio_data.dst.buffer_write =0;
	mrfile->audio_data.dst.buffer_read=0;
	return MP_SUCCESS;
}


MP_U32 media_recorder_video_buffer_initialize(VIM_MEDIA_RECORDER_FILE* rfile)
{
	MR_U32   i;
	MR_FILE *mrfile = rfile->mr_filehandle;

	for(i=0;i<mrfile->video_data.src.buffer_count;i++)
	{
		mrfile->video_data.src.buffer[i].isnot_empty=0;
		mrfile->video_data.src.buffer[i].data_length=0;
		mrfile->video_data.src.buffer[i].is_over=0;
	}
	mrfile->video_data.src.buffer_write =0;
	mrfile->video_data.src.buffer_read=0;
	
	for(i=0;i<mrfile->video_data.dst.buffer_count;i++)
	{
		mrfile->video_data.dst.buffer[i].isnot_empty=0;
		mrfile->video_data.dst.buffer[i].data_length=0;
		mrfile->video_data.dst.buffer[i].is_over=0;
		//mp_memset((unsigned char*)mrfile->video_data.dst.buffer[i].buffer_address, 0, 
			                                     // mrfile->video_data.dst.buffer[i].buffer_size);
		//_cpu_dcache_intellectual_clean_and_invalidate();
	}
	mrfile->video_data.dst.buffer_write =0;
	mrfile->video_data.dst.buffer_read =0;
	return MP_SUCCESS;
}






/*MP_U32 media_recorder_audio_start(VIM_MEDIA_RECORDER_FILE* rfile)
{
	MP_U32 	write_buffer, data_address;
	MR_FILE* mrfile;
	mrfile = rfile->mr_filehandle;
	mrfile->audio_encoder->start(mrfile->audio_encinfor);
	write_buffer = mrfile->audio_data.src.buffer_write;
	rfile->a_dmasamples =  mrfile->audio_data.src.buffer[write_buffer].buffer_size;
	//because mp3 encode need 480 smaple back in buffer start, 
	//so pcm->buffer[next_pcm].cursor_write = 480*2*2
	data_address = mrfile->audio_data.src.buffer[write_buffer].buffer_address+mrfile->audio_data.src.buffer[write_buffer].cursor_write;
	audio_pcm_input_dma_start((void*)data_address, 
							      mrfile->audio_data.src.buffer[write_buffer].buffer_size);
	return MP_SUCCESS;
}*/

MP_U32 media_recorder_audio_start(VIM_MEDIA_RECORDER_FILE* rfile)
{
	//MMD_MSG Msg;
	MP_U32 	write_buffer, data_address,ret=0;
	MR_FILE* mrfile;
	mrfile = rfile->mr_filehandle;
	
	ret = media_recorder_audio_start_smtd();
	if(ret)
	{
		MMD_ERROR_Deal(CMD_FAILED,MAJ_REC);
		return ret;
	}
	MMD_USER_WaitMICRS(1000);
	//mrfile->audio_encoder->start(mrfile->audio_encinfor);
	write_buffer = mrfile->audio_data.src.buffer_write;
	rfile->a_dmasamples =  mrfile->audio_data.src.buffer[write_buffer].buffer_size;
	//because mp3 encode need 480 smaple back in buffer start, 
	//so pcm->buffer[next_pcm].cursor_write = 480*2*2
	data_address = mrfile->audio_data.src.buffer[write_buffer].buffer_address+mrfile->audio_data.src.buffer[write_buffer].cursor_write;
	mp_printf("data_address(0x%x) cursor_write(%d) write_buffer(%d) buffer_size(%d)\n",(MP_U32)data_address,mrfile->audio_data.src.buffer[write_buffer].cursor_write,(MP_U32)write_buffer,mrfile->audio_data.src.buffer[write_buffer].buffer_size);
	/*Msg.CMD=MAJ_REC|MIN_MEDIA_REC_AUDIO_INPUT_DMA;
	Msg.OP1= (MMD_U32)data_address;
	Msg.OP2= mrfile->audio_data.src.buffer[write_buffer].buffer_size;
	//audio_pcm_input_set_param(rfile);
	MMD_BIU_Send_H2D_Cmd(&Msg);*/
	media_recorder_audio_input_smtd((MMD_U32)data_address,mrfile->audio_data.src.buffer[write_buffer].buffer_size);
	MMD_USER_WaitMICRS(1000);
	rfile->f_basetime = get_system_time_ticks();
	if(((mrfile->fs_filetype[0]=='m')&&(mrfile->fs_filetype[1]=='p')&&(mrfile->fs_filetype[2]=='4'))
		||((mrfile->fs_filetype[0]=='3')&&(mrfile->fs_filetype[1]=='g')&&(mrfile->fs_filetype[2]=='p')))
	{
		if(mrfile->video_infor.v_isvalid==0)
		{
			media_recorder_timer_start();
		}
	}
	else
	{
		media_recorder_jpgcap_timer_start();
	}
	/*Msg.CMD=MAJ_PLY|MIN_AUDIO_TEST;
	Msg.OP1= 0;
	Msg.OP2= 0;
	MMD_BIU_Send_H2D_Cmd(&Msg);*/
	//audio_pcm_input_dma_start((void*)data_address, 
							//      mrfile->audio_data.src.buffer[write_buffer].buffer_size);
	return MP_SUCCESS;
}


MP_U32 media_recorder_video_start(VIM_MEDIA_RECORDER_FILE* rfile)
{
	MR_U32 	write_buffer;
	MR_FILE* mrfile;
	mrfile = rfile->mr_filehandle;
	write_buffer = mrfile->video_data.dst.buffer_write;

	mrfile->video_encoder->start(mrfile->video_encinfor);
	mrfile->video_encoder->encode_frames((void*)mrfile->video_encinfor,
									     NULL,
									    (void*)&mrfile->video_data.dst.buffer[write_buffer]);

	
	//update playcontrol status
	rfile->f_recordcontrol |= MR_VIDEO_YUV_READY;
	rfile->f_basetime = get_system_time_ticks();
	
#if	VIDEORECORDERIN848
	media_player_videorecorder_starttimer_smtd();
#endif
	//video_rec_timer_start();
	media_recorder_timer_start();
	return MP_SUCCESS;
}



MP_U32 media_recorder_audio_stop(VIM_MEDIA_RECORDER_FILE* rfile)
{
	MR_FILE*	mrfile = rfile->mr_filehandle;
	MR_U32 state=0;

	if((mrfile->fs_filetype[0]=='m')&&(mrfile->fs_filetype[1]=='p')&&(mrfile->fs_filetype[2]=='3'))
	{
		
		while(state!=3)
		{
			state = media_recoder_state_detect();
			printf("mp3 encoder not done\n");
		}
	}
	media_recorder_audiorecorder_stop_smtd();
   if((mrfile->fs_filetype[0]=='m')&&(mrfile->fs_filetype[1]=='p')&&(mrfile->fs_filetype[2]=='3'))
	{
		MMD_Media_AudioCodec_FM_Mode(44100, MMD_AUDIOCODEC_FM_MIX_EN);
	}
   
	MMD_USER_WaitMICRS(5000);
	if(((mrfile->fs_filetype[0]=='m')&&(mrfile->fs_filetype[1]=='p')&&(mrfile->fs_filetype[2]=='4'))
		||((mrfile->fs_filetype[0]=='3')&&(mrfile->fs_filetype[1]=='g')&&(mrfile->fs_filetype[2]=='p')))
	{
		if(mrfile->video_infor.v_isvalid==0)
		{
			media_recorder_timer_stop();
		}
	}
	else
	{
		media_recorder_jpgcap_timer_stop();
	}
	
	return MP_SUCCESS;
}

extern MMD_U32 capture_width; //zhangxiang add for 320x240 capture fail
#define THUMBJPGLEN 30720
MP_U32 media_jpg_capture_start(VIM_MEDIA_RECORDER_FILE* rfile)
{
	MP_U32 jpgcaptureflag,ret,jpegfilelength;
	MP_U32 	write_buffer,timecount=0x1fff00;
	MR_PU08 thumbYUVaddr=0,thumbJPGaddr=0;
	MR_U32 thumbYUVlen,thumbJPGlen;
	MR_FILE* mrfile;
	video_stream_t*	dst;
	//MR_U32 stime,etime;
	
	if(((media_sysstatus->preview_infor.preview_dispwidth==176)&&(media_sysstatus->preview_infor.preview_dispheight==132))
	    &&((media_sysstatus->preview_infor.capture_width==320)&&(media_sysstatus->preview_infor.capture_height==240)))
	{
		timecount=0x15000;
	}
	//stime=MMD_SYS_GetTimer2Value();
	mrfile = rfile->mr_filehandle;
	write_buffer = mrfile->video_data.dst.buffer_write;
	if(g_recCallback) 
	{
		(*g_recCallback)(MMD_CAP_AUD_PLY,0,0);
	}
	mrfile->video_encoder->start(mrfile->video_encinfor);
	rfile->f_capturedone=0;
	FARF(MSG,("media_jpg_capture_start1\n"));
	ret=mrfile->video_encoder->encode_frames((void*)mrfile->video_encinfor,
									     NULL,
									    (void*)&mrfile->video_data.dst.buffer[write_buffer]);    
    FARF(MSG,("media_jpg_capture_start2=%d\n",ret));
	if(ret)
		return ret;
    FARF(MSG,("media_jpg_capture_start3=%d\n",media_sysstatus->preview_infor.capture_width));
	VIM_HAPI_SetJPEGCaptureStatus(0);
	if(capture_width <=640)  //zhangxiang mod for 320x240 capture fail
       timecount=60000;
	while(timecount)
	//while(1)
	{
		jpgcaptureflag=VIM_HAPI_GetJPEGCaptureStatus();
		if(jpgcaptureflag)
		{	
			VIM_HAPI_SetJPEGCaptureStatus(0);
			if(jpgcaptureflag&VIM_MARB_FIFOTHRESUP)
        	{
        		VIM_HAPI_Cap_FifoCont();
         		FARF(MSG,("Capture JFIFO Thresup\n"));
        	}
        	if(jpgcaptureflag&VIM_MARB_FIFOLARGE)
        	{
        		VIM_HAPI_Cap_Recapture();
        		FARF(MSG,("Capture Recapture\n"));
        	}
           	if(jpgcaptureflag&VIM_MARB_FIFOFULL)
        	{
         		VIM_HAPI_Cap_Recapture();
         		FARF(MSG,("Capture FIFOFULL\n"));
         	}	
        	if(jpgcaptureflag&VIM_MARB_FIFOFRMCNT)
        	{
        		jpegfilelength=VIM_HAPI_Cap_JbufDone();
        		if(jpegfilelength)
			{
    				FARF(MSG,("Capture Done! Capture Jpeg File Length is %d\n",jpegfilelength));
    				break;
			}
			else
			{
					FARF(MSG,("Capture Error!Capture Recapture\n"));
                    return MP_FAILE; //zhangxiang add
			}
        	}	
			jpgcaptureflag=0;
		}
		timecount--;
			
	}
	if(timecount==0)
	{
		mp_printf("Capture Timeout!\n");
		jpegfilelength=VIM_HAPI_Cap_JbufDone();
	}
	if((media_sysstatus->preview_infor.thumbnail_height*media_sysstatus->preview_infor.thumbnail_width)>0)
	{
		thumbYUVlen=media_sysstatus->preview_infor.thumbnail_height*media_sysstatus->preview_infor.thumbnail_width*2;
		thumbJPGlen=THUMBJPGLEN;
		thumbYUVaddr=mp_malloc(thumbYUVlen);
		thumbJPGaddr=mp_malloc(thumbJPGlen);
		if((thumbYUVaddr==0)||(thumbJPGaddr==0))
		{
			return MP_ERR_MALLOC_MEMORY_ERROR;
		}
		
		thumbYUVlen=VIM_HAPI_ReadThumbnailData(thumbYUVaddr,thumbYUVlen);
		if(thumbYUVlen==0)
      {
		FARF(MSG,("Capture Timeout!\n"));
        return MP_FAILE; //zhangxiang add
      }
	}
	mrfile->video_encoder->stop(mrfile->video_encinfor);
	//mp_printf("Get Jpg timecount(%d)\n",timecount);
	///media_recorder_jpgcap_timer_stop();

	dst = &mrfile->video_data.dst;
	if((media_sysstatus->preview_infor.thumbnail_height*media_sysstatus->preview_infor.thumbnail_width)>0)
	{
        	ret =VIM_HAPI_EncodeJpeg(thumbJPGaddr,thumbJPGlen,&thumbJPGlen);
        	if(ret)
        	{
        		mp_printf("VIM_HAPI_EncodeJpeg() error,ret is %d\n",ret);
        		return MP_FAILE;
        	}		
        	VIM_HAPI_EncodeJpeg_Release();
		        	
        	ret =VIM_HAPI_JPEG_MakeExif((MR_PU08)dst->buffer[write_buffer].buffer_address,jpegfilelength,dst->buffer[write_buffer].buffer_size,thumbJPGaddr,thumbJPGlen,&jpegfilelength);
        	if(ret)
        	{
        		mp_printf("VIM_HAPI_JPEG_MakeExif() error,ret is %d\n",ret);
        		return MP_FAILE;
        	}
		mp_printf("thumbnail Done! Capture Jpeg File Length is %d\n",jpegfilelength);
		mp_free(thumbYUVaddr);
		mp_free(thumbJPGaddr);
	}
	
	dst->buffer[write_buffer].data_length+=jpegfilelength;
	//dst->buffer[write_buffer].frame_duration[dst->buffer[write_buffer].frame_count] = rfile->v_duration; 
	//dst->buffer[write_buffer].frame_count++;
	dst->buffer[write_buffer].isnot_empty	= 1;
	dst->buffer[write_buffer].is_over =1;

	ret=media_recorder_video_write_frames(rfile,write_buffer);
	if(ret)
		return ret;
	if(rfile->fs_filestore==FILE_IN_BASEBAND_RAM)
	{
		if(g_recCallback) 
		{
			(*g_recCallback)(MMD_CAP_RAM_FILELEN,jpegfilelength,0);
		}
	}
	//etime=MMD_SYS_GetTimer2Value();
	//printf("media_recorder_video_write_frames() use %dms\n",(etime-stime)/1000);
	if((rfile->f_recordcontrol & MR_FILE_RECORD_END)==MR_FILE_RECORD_END)
	{
		//media_recorder_file_stop((VIM_MEDIA_RECFILE)rfile);
	}
	if(jpegfilelength==0)
		return MP_FAILE;
	else
	return MP_SUCCESS;
}


/*MP_U32 media_recorder_audio_stop(VIM_MEDIA_RECORDER_FILE* rfile)
{
	MR_FILE*	mrfile = rfile->mr_filehandle;
	
	audio_pcm_input_dma_stop();
	mrfile->audio_encoder->stop(mrfile->audio_encinfor);
	return MP_SUCCESS;
}*/

MP_U32 media_recorder_video_stop(VIM_MEDIA_RECORDER_FILE* rfile)
{
	MR_FILE*	mrfile = rfile->mr_filehandle;
	//video_rec_timer_stop();
	mrfile->video_encoder->stop(mrfile->video_encinfor);
	media_recorder_timer_stop();	
	return MP_SUCCESS;
}

MP_U32 media_recorder_start_record(VIM_MEDIA_RECORDER_FILE* rfile)
{
	MR_FILE*	mrfile = rfile->mr_filehandle;
	//MMD_MSG Msg;
	MMD_U32 ret=0;

	if(rfile->a_status == MEDIA_AUDIO_STOP)
	{
		rfile->f_recordcontrol|= MR_AUDIO_RECORD_END;
		if(mrfile->audio_infor.a_isvalid)
		{
			rfile->f_recordcontrol &= ~ MR_AUDIO_RECORD_END;
			rfile->a_dmasamples = 0;
			rfile->a_lastleftsmpls = 0;
			//audio_pcm_input_set_param(rfile);
			media_recorder_audio_setparam_smtd(rfile->mr_filehandle->audio_inputinfor.a_inputsmplrate,rfile->mr_filehandle->audio_inputinfor.a_inputchn);
			
			rfile->a_status = MEDIA_AUDIO_RECORDING;
			MMD_USER_WaitMICRS(1000);
			mp_printf("media_recorder_strupdata_smtd! MEDIA_AUDIO_RECORDING\n");
			ret = media_recorder_strupdata_smtd(rfile);
			if(ret)
			{
				MMD_ERROR_Deal(CMD_FAILED,MAJ_REC);
				return ret;
			}
			//MMD_REC_StrInit(rfile);
			media_recorder_audio_start(rfile);
		}
	}

	if(rfile->v_status == MEDIA_VIDEO_STOP)
	{
		rfile->f_recordcontrol|= MR_VIDEO_RECORD_END;
		if(mrfile->video_infor.v_isvalid==1)
		{
			rfile->f_recordcontrol &= ~ MR_VIDEO_RECORD_END;
			rfile->v_duration = 0;
			rfile->v_lastdone = 0;
			rfile->v_framesdone = 1;
			//start recording video 
			rfile->v_status = MEDIA_VIDEO_RECORDING;
			media_recorder_video_start(rfile);
			
		}
		else if(mrfile->video_infor.v_isvalid==3)
		{
			rfile->f_recordcontrol &= ~ MR_VIDEO_RECORD_END;
			//rfile->v_duration = 0;
			//rfile->v_lastdone = 0;
			//rfile->v_framesdone = 1;
			//start recording video 
			rfile->v_status = MEDIA_VIDEO_RECORDING;
			return media_jpg_capture_start(rfile);
			
		}
	}
	
	// update player status
	rfile->f_recordstatus = MEDIA_FILE_RECORDING;
	
	return MP_SUCCESS;
}

/*******************************************************************************************
Description:
	media player check recorder file handle is ok?
Parameters:
	VIM_MEDIA_RECORDER_FILE* pthis: recorder file handle
Return:	
	MP_SUCCESS:  file handle is ok
	MP_ERR_FILE_HANDLE_ERROR: file handle is error
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.07.02		create first version
********************************************************************************************/
MR_U32 media_recorder_check_filehandle(VIM_MEDIA_RECORDER_FILE* pthis)
{
	if(pthis==NULL)
		return MP_ERR_FILE_HANDLE_ERROR;
	if(media_sysstatus->rcfilelst[0]==pthis)
	{
		return MP_SUCCESS;
	}
	return MP_ERR_FILE_HANDLE_ERROR;
}

/*******************************************************************************************
Description:
	read video data to src buffer(src buffer is pingpang buffer)
Parameters:
	MP_FILE* pthis: player file handle
	MP_U32 buffer_index: src buffer index
Return:	
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.09.17		create first version
********************************************************************************************/
MP_U32 media_recorder_video_write_frames(VIM_MEDIA_RECORDER_FILE* rfile, MR_U32 buffer_index)
{
	void 		  	*muxerinfor;
	me_muxer_t	  	*muxer;
	video_stream_infor	*buffer;
	MR_U32  			result;

#if VIDEO_FRAMES_WRITE_TIME_OUTPUT
	MR_U32  			s_time, e_time;
#endif
	MR_FILE* mrfile= rfile->mr_filehandle;
	result = media_recorder_check_filehandle(rfile);
	if(result)			
		return 0;
	muxerinfor  = mrfile->muxerinfor;
	muxer	   = mrfile->muxer;
	buffer	   = &mrfile->video_data.dst.buffer[buffer_index];
	
#if VIDEO_FRAMES_WRITE_TIME_OUTPUT
	s_time = get_system_time_ticks();	//(us) 
#endif
	/*VIM_HAPI_ReadVideoRawData(buffer->buffer_address,TestBuffer+TestBufferIndex,buffer->data_length);
	TestBufferIndex+=buffer->data_length;
	
	mp_printf("TestBufferIndex addr is %x,rawdata length is %x\n",TestBuffer,TestBufferIndex);*/
	
	if(!uSerPicCount)
		result = muxer->write_video_frames(muxerinfor, buffer);
	else
	{			
		SerPicfilehandle[uPhotoCount].FileAddress = mp_malloc(buffer->data_length);
		SerPicfilehandle[uPhotoCount].FileTotalLength = buffer->data_length;
		memcpy(SerPicfilehandle[uPhotoCount].FileAddress,(char *)buffer->buffer_address,buffer->data_length);
		uPhotoCount++;
		mp_printf("SerPicfilehandle[%d] FileAddress(0x%x) FileTotalLength(%d) \n",uPhotoCount-1,(MP_U32)(SerPicfilehandle[uPhotoCount-1].FileAddress),SerPicfilehandle[uPhotoCount-1].FileTotalLength);
	}
#if VIDEO_FRAMES_WRITE_TIME_OUTPUT
	e_time = get_system_time_ticks();	//(us) 
	mp_debug(("write video buffer = %d, write_bytes =%d, time = %d ", buffer_index, buffer->data_length, e_time-s_time));
#endif
	rfile->f_recordsize 	+= buffer->data_length;
	//update stream buffer information for next time use			
	buffer->cursor_write	= 0;	
	buffer->cursor_read	= 0;
	buffer->data_length	= 0;
	buffer->frame_count	= 0;
	buffer->isnot_empty 	= result;
	if(result)
	{	//if read file error, end video play
		rfile->f_recordcontrol |= MR_VIDEO_STREAM_END;
		rfile->f_recordcontrol |= MR_VIDEO_DECODE_END;
		mp_debug(("video data write end, file status = 0x%08X ", rfile->f_recordcontrol ));
	}
	if(buffer->is_over)
	{
		buffer->is_over=0;
		if(mrfile->video_infor.v_isvalid==3)
		{
			rfile->f_recordcontrol |= MR_FILE_RECORD_END;
		}
		else
		{
			rfile->f_recordcontrol |=MR_VIDEO_RECORD_END;
		}
	}
	
	return result;
}

/*******************************************************************************************
Description:
	read video data to src buffer(src buffer is pingpang buffer)
Parameters:
	MP_FILE* pthis: player file handle
	MP_U32 buffer_index: src buffer index
Return:	
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.09.17		create first version
********************************************************************************************/
MP_U32 media_recorder_audio_write_frames(VIM_MEDIA_RECORDER_FILE* rfile, MR_U32 buffer_index)
{
	void 		  	*muxerinfor;
	me_muxer_t	  	*muxer;
	audio_stream_infor	*buffer;
	MR_U32			  result;
	MR_FILE			*mrfile;
#if AUDIO_FRAMES_WRITE_TIME_OUTPUT
	MR_U32  		s_time, e_time;
#endif
	result = media_recorder_check_filehandle(rfile);
	if(result)			
		return 0;
	mrfile = rfile->mr_filehandle;
	muxerinfor  = mrfile->muxerinfor;
	muxer	   = mrfile->muxer;
	buffer	   = &mrfile->audio_data.dst.buffer[buffer_index];
#if AUDIO_FRAMES_WRITE_TIME_OUTPUT
	s_time = get_system_time_ticks();	//(us) 
#endif
	result = muxer->write_audio_frames(muxerinfor, buffer);
#if AUDIO_FRAMES_WRITE_TIME_OUTPUT
	e_time = get_system_time_ticks();	//(us) 
	mp_debug(("write audio buffer = %d, write_bytes =%d, time = %d ", buffer_index, buffer->data_length, e_time-s_time));
#endif
	rfile->f_recordsize 	+= buffer->data_length;
	//update stream buffer information for next time use
	buffer->cursor_write	= 0;	
	buffer->cursor_read	= 0;
	buffer->data_length	= 0;
	buffer->frame_count	= 0;
	buffer->isnot_empty 	= result;
	if(result)
	{	//if write file error, end audio recording
		rfile->f_recordcontrol |= MR_AUDIO_STREAM_END;
		mp_debug(("audio data writte end, file status = 0x%08X ", rfile->f_recordcontrol ));
	}
	//check the buffer data is last or not
	if(buffer->is_over)
	{
		buffer->is_over=0;
		rfile->f_recordcontrol |= MR_AUDIO_RECORD_END;
	}
	return result;
}


/*******************************************************************************************
Description:
	media player file open
Parameters:
	VIM_MEDIA_PLYFILE_INPUTINFOR *srcfile: 
	VIM_MEDIA_PLAYER_FILE **pfile:
Return:	
	MP_SUCCESS: open player file ok
	Others:			error code
NOTE: 
       this function call in file thread(open file)	
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.04.02		create first version
********************************************************************************************/
MP_U32 media_player_file_open(MP_U32 inputfile, VIM_MEDIA_PLYFILE*pfilehandle)
{
	MP_U32 	 		result, i;
	me_player_infor 	ply_infor;
	void				*fs_filehandle;
	MEMORY_FILE		*mem_filehandle;
	VIM_MEDIA_PLAYER_FILE 		*pthis;
	VIM_MEDIA_PLYFILE_INPUTINFOR *srcfile;

	
	srcfile=(VIM_MEDIA_PLYFILE_INPUTINFOR *)inputfile;	
	FARF(MSG,("COME IN media_player_file_open"));
    
	// check open file number
	if(media_sysstatus->ply_filecounter>=VIM_MEDIA_MAX_SUPPORT_FILE_NUMBER)
	{
		mp_print(("-----media player only support open %d file at same time!",VIM_MEDIA_MAX_SUPPORT_FILE_NUMBER));
		return MP_ERROR;
	}
	mp_print((" entry media_player_file_open!"));

	//process src file parameter get file handle
	if(srcfile->fs_filestore==FILE_IN_CORE_FILE)
	{	
#if PLAY_FILENAME_INPUT_UART
		//check if file name is Unicode format
		if(fat_filelength((char *)srcfile->fs_filename)==0)
		{
			mp_print((" fat_filelength()=0!"));
			for(i=512-1; i>0; i--)
			{
				srcfile->fs_filename[2*i+1] = 0;
				srcfile->fs_filename[2*i]=srcfile->fs_filename[i];
			}
			srcfile->fs_filename[1] = 0;
		}
		//open file 
		mp_print((" entry filesys_fileopen!"));
		fs_filehandle =filesys_fileopen((void *)srcfile->fs_filename, MEDIA_FOPEN_READ_BINARY);
		if(!fs_filehandle)
		{
			ME_ERROR(("MP_ERR_FS_OPEN_FILE_ERROR"));
			//MSG_FATAL("COME IN MP_ERR_FS_OPEN_FILE_ERROR L3398", 0, 0, 0);
			FARF(MSG,("COME IN MP_ERR_FS_OPEN_FILE_ERROR L3398"));
			return MP_ERR_FS_OPEN_FILE_ERROR;
		}	
		mp_print((" filesys_fileopen()=ok!"));
#else
		fs_filehandle =filesys_fileopen((void *)srcfile->fs_filename, MEDIA_FOPEN_READ_BINARY);
        FARF(MSG,("filesys_fileopen()=%d", fs_filehandle));
		if(!fs_filehandle)
		{
			ME_ERROR(("MP_ERR_FS_OPEN_FILE_ERROR"));
			FARF(MSG,("MP_ERR_FS_OPEN_FILE_ERROR L3408"));			
			return MP_ERR_FS_OPEN_FILE_ERROR;
		}
		pthis= (VIM_MEDIA_PLAYER_FILE*)mp_malloc( sizeof(VIM_MEDIA_PLAYER_FILE));
		if(!pthis)
		{
			filesys_fileclose((void*)fs_filehandle);
			ME_ERROR(("MP_ERR_MALLOC_MEMORY_ERROR"));
			return MP_ERR_MALLOC_MEMORY_ERROR;
		}
#endif
		MMD_BIU_Set_Device_Status(IDX_FILE_STATUS, 0);
	}
	else if(srcfile->fs_filestore==FILE_IN_BASEBAND_RAM)
	{	
		memory_set_fileread_mode(VC_OFM_READ); // set file create flag for memory_fileopen and memory_filewrite
		mem_filehandle = (MEMORY_FILE * )memory_fileopen((void *)srcfile->fs_filename,(void *)&srcfile->mem_filelength);
		if(!mem_filehandle)
		{
			memory_fileclose(NULL);
			ME_ERROR(("MP_ERR_MALLOC_MEMORY_ERROR"));
			return MP_ERR_MALLOC_MEMORY_ERROR;
		}
    	pthis= (VIM_MEDIA_PLAYER_FILE*)mp_malloc( sizeof(VIM_MEDIA_PLAYER_FILE));    
        FARF(MSG,("mp_malloc()"));
    	if(!pthis)
    	{
    		memory_fileclose((void*)mem_filehandle);
    		ME_ERROR(("MP_ERR_MALLOC_MEMORY_ERROR"));
            FARF(MSG,("MP_ERR_MALLOC_MEMORY_ERROR"));
    		return MP_ERR_MALLOC_MEMORY_ERROR;
    	}
	}
	else    //zhangxiang add
	{
		mem_filehandle = (MEMORY_FILE * )TCL_RAM_fileopen((void *)srcfile->mem_fileaddr,(void *)&srcfile->mem_filelength);
		if(!mem_filehandle)
		{
			TCL_RAM_fileclose(NULL);
			ME_ERROR(("MP_ERR_MALLOC_MEMORY_ERROR"));
			return MP_ERR_MALLOC_MEMORY_ERROR;
		}
	    	pthis= (VIM_MEDIA_PLAYER_FILE*)mp_malloc( sizeof(VIM_MEDIA_PLAYER_FILE));    
	        FARF(MSG,("mp_malloc()"));
	    	if(!pthis)
	    	{
	    		TCL_RAM_fileclose((void*)mem_filehandle);
	    		ME_ERROR(("MP_ERR_MALLOC_MEMORY_ERROR"));
	            FARF(MSG,("MP_ERR_MALLOC_MEMORY_ERROR"));
	    		return MP_ERR_MALLOC_MEMORY_ERROR;
	    	}
	}	
	mp_memset((unsigned char*)pthis, 0, sizeof(VIM_MEDIA_PLAYER_FILE));
	ply_infor.v_srcwidth = srcfile->v_srcwidth;
	ply_infor.v_srcheight = srcfile->v_srcheight;
	ply_infor.v_srcxpos = srcfile->v_srcxpos;
	ply_infor.v_srcypos = srcfile->v_srcypos;
	ply_infor.v_outwidth = srcfile->v_outwidth;
	ply_infor.v_outheight = srcfile->v_outheight;
	ply_infor.v_outxpos = srcfile->v_outxpos;
	ply_infor.v_outypos = srcfile->v_outypos;
	ply_infor.v_outeffect= srcfile->v_outeffect;
	ply_infor.v_outrotation= srcfile->v_outrotation;
	ply_infor.v_jpgmode= srcfile->v_jpgmode;
	ply_infor.v_jpgdecodeaddr = srcfile->v_jpgdecodeaddr;
	ply_infor.v_jpgdecodelen   = srcfile->v_jpgdecodelen;
	ply_infor.a_outbits= srcfile->a_outbits;
	ply_infor.a_outchn= srcfile->a_outchn;
	ply_infor.a_outsmplrate= srcfile->a_outsmplrate;
	ply_infor.a_outmode=  media_sysstatus->audio_mode;
	if(srcfile->fs_filestore==FILE_IN_CORE_FILE)
	{	
		pthis->fs_filestore = FILE_IN_CORE_FILE;
		pthis->fs_filehandle =(MP_U32)fs_filehandle; 
		ply_infor.fs_filehandle = (void*)fs_filehandle;
		ply_infor.fs_filefunc = &g_filesys_file;
	}
	else if(srcfile->fs_filestore==FILE_IN_BASEBAND_RAM)
	{	
		pthis->fs_filestore = FILE_IN_BASEBAND_RAM;
		pthis->fs_filehandle =(MP_U32)mem_filehandle; 
		ply_infor.fs_filehandle = (void*)mem_filehandle;
		ply_infor.fs_filefunc = &g_memory_baseband_file;
	}
	else //zhangxiang add
	{
		pthis->fs_filestore = FILE_IN_TCL_RAM;
		pthis->fs_filehandle =(MP_U32)mem_filehandle; 
		ply_infor.fs_filehandle = (void*)mem_filehandle;
		ply_infor.fs_filefunc = &g_TCL_RAM_file;
	}	
	pthis->f_jpgmode = srcfile->v_jpgmode;
	pthis->ring_info = srcfile->ring_info;	// Add by antonliu, for volume adjust
	mp_memcpy((char*)ply_infor.fs_filetype, (const char*)&srcfile->fs_filetype, 4);
	//add by yjz 2009/2/10
	if(((strcmp(ply_infor.fs_filetype,"aac") == 0 )|| (strcmp(ply_infor.fs_filetype,"AAC") == 0 ))&& (media_sysstatus->work_mode & VIM_MEDIA_MODE_AAC_DISABLE)) 
	{
		mp_print("aac file  is not supported\n");
		mp_free((void*)pthis);
		return 1;
	}	
	// media engine file open 
	if(srcfile->fs_filestore==FILE_IN_CORE_FILE)
	{	
		result = me_player_open(&ply_infor, &pthis->mp_filehandle);
        FARF(MSG,("me_player_open() result =%d",result));
		mp_print(("-----me_player_open() return = 0x%08X",result));
		if(result)
		{
			filesys_fileclose((void*)fs_filehandle);
			mp_free((void*)pthis);
			return result;
		}
	}
	else if(srcfile->fs_filestore==FILE_IN_BASEBAND_RAM)
	{//srcfile->StoreType==FILE_IN_BASEBAND_RAM
		result =me_player_open(&ply_infor, &pthis->mp_filehandle);
		mp_print(("-----me_player_open() return = 0x%08X",result));
		if(result)
		{
			memory_fileclose((void*)mem_filehandle);
			mp_free((void*)pthis);
			return result;
		}
	}
	else 
	{
		result =me_player_open(&ply_infor, &pthis->mp_filehandle);
		mp_print(("-----me_player_open() return = 0x%08X",result));
		if(result)
		{
			TCL_RAM_fileclose((void*)mem_filehandle);
			mp_free((void*)pthis);
			return result;
		}
	}
	//get player file type and kind
	mp_memcpy((char*)&pthis->mp_filetype, (const char*)pthis->mp_filehandle->fs_filetype, 4);

	// Create OS dependent timer
	if(pthis->mp_filehandle->video_infor.v_isvalid==3) 
		media_player_jpgdis_timer_creat(); //JPEG
	else
		media_player_timer_creat();		// other type file

	//initialize meida file syn time for audio and video
	pthis->v_duration = 0;
	pthis->v_time 	= 0;
	pthis->v_frames	= 0;
	pthis->v_samples	= 1;
	pthis->a_duration	= 0;
	pthis->a_samples	= 0;
	pthis->a_time 	= 0;
	//update play file status
	pthis->a_status 	 = MEDIA_AUDIO_STOP;
	pthis->v_status 	 = MEDIA_VIDEO_STOP;
	pthis->f_playstatus = MEDIA_FILE_STOP;
	//update media system file lst
	for(i=0;i<VIM_MEDIA_MAX_SUPPORT_FILE_NUMBER;i++)
	{
		if(media_sysstatus->plyfilelst[i]==NULL)
		{
			media_sysstatus->plyfilelst[i] = pthis;
			break;
		}
	}
	media_sysstatus->ply_filecounter++;
	(*pfilehandle) = (VIM_MEDIA_PLYFILE)pthis;
	return MP_SUCCESS; 
}

/*******************************************************************************************
Description:
	media player file close and all buffer free
Parameters:
	MP_FILE* pthis: player file handle
Return:	
	MP_SUCCESS: close ok
	Others:			error code
NOTE: 
       this function call in file thread(close file)
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.04.02		create first version
********************************************************************************************/
MP_U32 media_player_file_close(VIM_MEDIA_PLYFILE pfilehandle)
{
	MP_U32 result, i, return_value = MP_SUCCESS;
	VIM_MEDIA_PLAYER_FILE* pfile;
	pfile = (VIM_MEDIA_PLAYER_FILE*)pfilehandle;
	if(pfile==NULL)
		return MP_ERR_FILE_HANDLE_ERROR;
	for(i=0;i<VIM_MEDIA_MAX_SUPPORT_FILE_NUMBER;i++)
	{
		if(media_sysstatus->plyfilelst[i]==pfile)
		{
			//check audio status and stop audio playing
			if(pfile->a_status!=MEDIA_AUDIO_STOP)
			{
				media_player_audio_stop(pfile);
				pfile->a_status = MEDIA_AUDIO_STOP;
			}
			//check video status and stop video playing
			if(pfile->v_status!=MEDIA_VIDEO_STOP)
			{
				media_player_video_stop(pfile);
				pfile->v_status = MEDIA_VIDEO_STOP;
			}
			if(pfile->f_playstatus!=MEDIA_FILE_STOP)
			{
				media_player_timer_stop();
			}
			// if data buffer have initialize need release data buffer
			if(pfile->f_playbufinit)
			{
				result = media_player_buffer_release(pfile->mp_filehandle);
				if(result)
				{
					mp_error(("-----media_player_buffer_release() return = 0x%08X ",result));
					return_value = result;
				}
				pfile->f_playbufinit = 0;
			}
			// release media player file struct
			result = me_player_close(pfile->mp_filehandle);
			if(result)
			{
				mp_error(("-----me_player_close() return = 0x%08X ",result));
				return_value = result;
			}
			// close source file 
			if(pfile->fs_filestore==FILE_IN_CORE_FILE)
			{	
				result = filesys_fileclose((void*)pfile->fs_filehandle);
				if(result)
				{
					ME_ERROR(("MP_ERR_FS_CLOSE_FILE_ERROR"));
					return_value = MP_ERR_FS_CLOSE_FILE_ERROR;
				}
			}
			else if(pfile->fs_filestore==FILE_IN_BASEBAND_RAM)
			{//srcfile->StoreType==FILE_IN_BASEBAND_RAM
			    memory_fileclose((void *)pfile->fs_filehandle);
				mp_free((void*)pfile->fs_filehandle);
			}
			else 
			{
				TCL_RAM_fileclose((void *)pfile->fs_filehandle);
				mp_free((void*)pfile->fs_filehandle);
			}
			mp_free(pfile);
			media_sysstatus->plyfilelst[i] = NULL;
			media_sysstatus->ply_filecounter--;
			// Release OS dependent timer
			MMD_USER_Timer_Release();
			return return_value;
		}
	}
	return MP_ERR_FILE_HANDLE_ERROR;
}
/*******************************************************************************************
Description:
	media player file stop
Parameters:
	MP_FILE* pthis: player file handle
Return:	
	MP_SUCCESS: stop player file ok
	Others:		  error code
NOTE: 
       this function call in main thread(stop file playing)
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.04.02		create first version
********************************************************************************************/
MP_U32 media_player_file_stop(VIM_MEDIA_PLYFILE pfilehandle)
{
	MP_U32   result,timeout_stop=20,readstatus;
#if 0	
#ifndef _WIN32
	VIM_MSG msg;
#endif
#endif
	VIM_MEDIA_PLAYER_FILE* pfile;
	pfile = (VIM_MEDIA_PLAYER_FILE*)pfilehandle;
	//check player file handle
	result = media_player_check_filehandle(pfile);
	if(result)			
		return result;
	//check file status
	if (pfile->f_playstatus == MEDIA_FILE_STOP)
		return MP_SUCCESS; 
	
	if(pfile->f_playstatus!=MEDIA_FILE_STOP) 
	{
		if(pfile->mp_filehandle->video_infor.v_isvalid==0) {
			media_player_timer_stop();
		}
		else if(pfile->mp_filehandle->video_infor.v_isvalid==1)
		{
			media_player_timer_stop();
		}
		else if(pfile->mp_filehandle->video_infor.v_isvalid==3)
		{
			media_player_jpgdis_timer_stop();
		}
		
		pfile->f_playstatus=MEDIA_FILE_STOP;
	}
	
	timeout_stop=10;
	
	while(timeout_stop)
	{
		MMD_BIU_Get_Device_Status(IDX_FILE_STATUS, &readstatus);
 		if(readstatus & 0x1)
 		{
 			MMD_USER_WaitMICRS(1000);//mp_print("Last Write Is Not Flished!writeflag(0x%x)\n",writestatus);
 		}
 		else
 		{
 			break;
 		}
		timeout_stop--;	
 	}           
	
 	if(!timeout_stop)
 	{
 		ME_ERROR(("MP_ERR_FS_WRITE_FILE_ERROR\n"));
 		return  MP_ERR_FILE_STATUS_ERROR;
 	}
	
	//check audio status and stop audio playing	
	if(pfile->a_status!=MEDIA_AUDIO_STOP)
	{
		pfile->a_status = MEDIA_AUDIO_STOP;
		media_player_audio_stop(pfile);
	}
	//check video status and stop video playing
	if(pfile->v_status!=MEDIA_VIDEO_STOP)
	{
		pfile->v_status = MEDIA_VIDEO_STOP;
		media_player_video_stop(pfile);
	}
	
	media_player_file_seek((MP_U32)pfile, 0, &result);
//#else
//	msg.CMD = MAJ_PLY|MIN_PLY_VIDEO_AUDIO_RESET;
//	msg.OP1 = (MP_U32)pfile;
//	msg.OP2 = 0;
//	#ifdef MESSAGE_PROCESS_IN_MEFILETHREAD
//		mmd_send_mefile_msg(&msg, 0);
//	#else
//		mmd_send_msg_file(&msg, 0);
//	#endif
//#endif
	//update file status
//	pfile->f_playstatus = MEDIA_FILE_STOP;
	mp_debug(("media_sysstatus->ply_videocounter = %d ", media_sysstatus->ply_videocounter));
	mp_debug(("media_sysstatus->ply_audiocounter = %d ", media_sysstatus->ply_audiocounter));
	return MP_SUCCESS;
}

/*******************************************************************************************
Description:
	media player file pause
Parameters:
	MP_FILE* pthis: player file handle
Return:	
	MP_SUCCESS: pause player file ok
	Others:		  error code
NOTE: 
       this function call in main thread(pause file playing)	
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.04.02		create first version
********************************************************************************************/
MP_U32 media_player_file_pause(VIM_MEDIA_PLYFILE pfilehandle)
{
	MP_U32   result;
	MP_FILE*	mpfile = NULL;
	VIM_MEDIA_PLAYER_FILE* pfile = (VIM_MEDIA_PLAYER_FILE*)pfilehandle;
	
	//check player file handle
	result = media_player_check_filehandle(pfile);
	if(result)			
		return result;
	//check file status
	if(pfile->f_playstatus!=MEDIA_FILE_PLAYING)
	{
		ME_ERROR(("MP_ERR_FILE_STATUS_ERROR, pfile->f_playstatus=%d", pfile->f_playstatus));
		return MP_ERR_FILE_STATUS_ERROR;
	}
	mpfile = pfile->mp_filehandle;
	//pause playing video
	if(pfile->v_status==MEDIA_VIDEO_PLAYING)
	{
		mpfile->video_decoder->pause(mpfile->video_decinfor);
		pfile->v_status = MEDIA_VIDEO_PAUSE;
		media_sysstatus->ply_videocounter--;
		media_player_timer_stop();	
	}
	//pause playing audio
	if(pfile->a_status==MEDIA_AUDIO_PLAYING)
	{
		pfile->a_status = MEDIA_AUDIO_PAUSE;
		media_sysstatus->ply_audiocounter--;
		if(!(mpfile->video_infor.v_isvalid)) 
		{
			media_player_timer_stop();	
		}
		media_player_audio_decpause_smtd();
	}
	//set file pause status
	pfile->f_playstatus = MEDIA_FILE_PAUSE;
	return MP_SUCCESS;
}

/*******************************************************************************************
Description:
	media player file resume
Parameters:
	MP_FILE* pthis: player file handle
Return:	
	MP_SUCCESS: resume player file ok
	Others:		  error code
NOTE: 
       this function call in main thread(resume file playing)	
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.04.02		create first version
********************************************************************************************/
MP_U32 media_player_file_resume(VIM_MEDIA_PLYFILE pfilehandle)
{
	MP_U32     result;
	MP_FILE*	mpfile = NULL;
	VIM_MEDIA_PLAYER_FILE* pfile= (VIM_MEDIA_PLAYER_FILE*)pfilehandle;
	
	//check player file handle
	result = media_player_check_filehandle(pfile);
	if(result)			
		return result;
	//check file status
	if(pfile->f_playstatus != MEDIA_FILE_PAUSE)
	{
		ME_ERROR(("MP_ERR_FILE_STATUS_ERROR, pfile->f_playstatus=%d", pfile->f_playstatus));
		return MP_ERR_FILE_STATUS_ERROR;
	}	
	mpfile = pfile->mp_filehandle;
	//resume video playing
	if(pfile->v_status == MEDIA_VIDEO_PAUSE)
	{
		mpfile->video_decoder->resume(mpfile->video_decinfor);
		pfile->f_playtime  =  pfile->v_time;				//(ms)	
		pfile->f_basetime = MMD_SYS_GetTimer2Value();//us//get_system_time_ticks();	//(us) 
		pfile->v_status = MEDIA_VIDEO_PLAYING;
		media_player_timer_start();
        media_player_time_dsr();//zhangxiang add 09/03/04
	}
	//resume audio playing
	if(pfile->a_status == MEDIA_AUDIO_PAUSE)
	{
		pfile->a_status = MEDIA_AUDIO_PLAYING;
//		media_player_audio_start(pfile);
		if(!(mpfile->video_infor.v_isvalid)) 
		{
			media_player_timer_start();	
		}
		result = media_player_audio_decresume_smtd();
        media_player_time_dsr();//zhangxiang add 09/03/04
		if(result)
			MMD_ERROR_Deal(CMD_FAILED,MAJ_PLY);
	}
	//set file pause status
	pfile->f_playstatus=MEDIA_FILE_PLAYING;
	return MP_SUCCESS;
}

/*******************************************************************************************
Description:
	media player file play
Parameters:
	MP_FILE* pthis: player file handle
Return:	
	MP_SUCCESS: start playing file ok
	Others:		  error code
NOTE: 
       this function call in main thread(start playing)		
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.04.02		create first version
********************************************************************************************/
MP_U32 media_player_file_play(VIM_MEDIA_PLYFILE pfilehandle, MP_U32 Callback)
{
	MP_U32  	result;
#if 0	
#ifndef _WIN32
	VIM_MSG msg;
#endif
#endif

	VIM_MEDIA_PLAYER_FILE* pfile;
	
	pfile = (VIM_MEDIA_PLAYER_FILE*)pfilehandle;
	//check player file handle
	result = media_player_check_filehandle(pfile);
	if(result)			
		return result;
	//check file status
	if(!((pfile->f_playstatus==MEDIA_FILE_STOP)
	    |(pfile->f_playstatus==MEDIA_FILE_FAST_PLAYING)))
	{
		ME_ERROR(("MP_ERR_FILE_STATUS_ERROR, pfile->f_playstatus=%d", pfile->f_playstatus));
		// MSG_FATAL("MP_ERR_FILE_STATUS_ERROR, pfile->f_playstatus=%d",pfile->f_playstatus, 0, 0);
		return MP_ERR_FILE_STATUS_ERROR;
	}

	flg_vdecode = 0;
	flg_vread = 0;
	flg_aread = 0;//audio read flag
	if(Callback)
		g_plyCallback = (Func_Callback)Callback;
	result = media_player_start_play(pfile);	//by dnn 2008/4/10
	if(result != ME_SUCCESS)
	{
		if(g_plyCallback)
		{
			(*g_plyCallback)(MMD_PLY_FILE_END);
		}
	}
	return result;//MP_SUCCESS;
}
/*******************************************************************************************
Description:
	media player file fastplay
Parameters:
	MP_FILE* pthis: player file handle
Return:	
	MP_SUCCESS: start fastplaying file ok
	Others:		  error code
NOTE: 
       this function call in main thread(start fastplaying)	
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.04.02		create first version
********************************************************************************************/
MP_U32 media_player_file_fastplay(VIM_MEDIA_PLYFILE pfilehandle, MP_U32  mode)
{
	MP_U32   result;
	MP_FILE *mpfile = NULL;
#if 0	
#ifndef _WIN32
	VIM_MSG msg;
#endif	
#endif
	VIM_MEDIA_PLAYER_FILE* pfile;
		
	pfile = (VIM_MEDIA_PLAYER_FILE*)pfilehandle;
	//check player file handle
	result = media_player_check_filehandle(pfile);
	if(result)			
		return result;
	//check file status
	if(!((pfile->f_playstatus==MEDIA_FILE_STOP)
	    |(pfile->f_playstatus==MEDIA_FILE_PLAYING)
	    |(pfile->f_playstatus==MEDIA_FILE_FAST_PLAYING)))
	{
		ME_ERROR(("MP_ERR_FILE_STATUS_ERROR, pfile->f_playstatus=%d", pfile->f_playstatus));
		return MP_ERR_FILE_STATUS_ERROR;
	}
	mpfile = pfile->mp_filehandle;
	//check file if have video???
	if(!mpfile->video_infor.v_isvalid)
	{
		ME_ERROR(("MP_ERR_FILE_STATUS_ERROR"));
		return MP_ERR_FILE_STATUS_ERROR;
	}
	//check audio status, stop audio playing if audio is playing
	if(pfile->a_status==MEDIA_AUDIO_PLAYING)
	{
#if ADD_AUDIO_CODE	//by dnn 22008/4/11
		mpfile->audio_decoder->stop(mpfile->audio_decinfor);
		pfile->a_status = MEDIA_AUDIO_STOP;
		media_player_audio_stop(pfile);
#endif		
	}
	//check video status, pause video play if video is playing
	if(pfile->v_status==MEDIA_VIDEO_PLAYING)
	{
//by dnn 2008/4/11		video_syn_timer_stop();
		mpfile->video_decoder->pause(mpfile->video_decinfor);
		pfile->v_status = MEDIA_VIDEO_PAUSE;
	}
	media_player_timer_stop();
	
	pfile->f_playmode = mode;
	pfile->f_playcontrol|= MP_AUDIO_PLAY_END;
//#ifdef _WIN32
	media_player_start_fastplay(pfile);
//#else
//	msg.CMD = MAJ_PLY|MIN_PLY_START_FASTPLAY;
//	msg.OP1 = (MP_U32)pfile;
//	msg.OP2 = 0;
//	#ifdef MESSAGE_PROCESS_IN_MEFILETHREAD
//		mmd_send_mefile_msg(&msg, 0);
//	#else
//		mmd_send_msg_file(&msg, 0);
//	#endif
//#endif
	return MP_SUCCESS;
}
/*******************************************************************************************
Description:
	media player file close and all buffer free
Parameters:
	MP_FILE* pthis: player file handle
	MP_U32 pos:            user want seek time pos(s)
	MP_U32*accesspos:  file access time pos(s)
Return:	
	MP_SUCCESS: seek ok
	Others:			error code
NOTE: 
       this function call in file thread(seek)
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.04.02		create first version
********************************************************************************************/
MP_U32 media_player_file_seek(VIM_MEDIA_PLYFILE pfilehandle, MP_U32 pos, MP_U32*accesspos)
{
	MP_U32	seekpos, asscessframe, result;
	MP_FILE *pthis = NULL;
	#if ME_AUDIO_POSTPROC_ENABLE
	me_node_t		*pnod	= NULL;
	me_effectproc_t	*effectproc= NULL;
	#endif
	VIM_MEDIA_PLAYER_FILE* pfile;
	MP_U32 sample_rate=0;
	MP_U32 dma_count=0;
	//add by yjz 2009/2/11
	SEEK_INFO* SeekInfo;
	VM_MIDI_PARSER_INFO *parsefile;
	///:~
	
	pfile = (VIM_MEDIA_PLAYER_FILE*)pfilehandle;
	result = media_player_check_filehandle(pfile);
	if(result)			
		return result;
	pthis = pfile->mp_filehandle;
	
	//before seek must stop decode and read data
	if(pfile->f_playstatus==MEDIA_FILE_PLAYING) 
	{
		media_player_timer_stop();	//by dnn 2008/4/11
	}
	if(pfile->a_status==MEDIA_AUDIO_PLAYING)
	{//stop audio output and decode
		media_player_audio_stop(pfile);
		pfile->a_status = MEDIA_AUDIO_STOP;
	}
	if(pfile->v_status==MEDIA_VIDEO_PLAYING)
	{//stop video output and decode
		media_player_video_stop(pfile);
		pfile->v_status = MEDIA_VIDEO_STOP;
	}

	//seek audio and video time position by parser
	result = pthis->parser->seek(pthis->parserinfor, 
							  pos, &seekpos, &asscessframe);
	if(result)
		return result;
	pfile->v_frames = asscessframe;
	*accesspos = pfile->f_playtime = seekpos;
	//seek audio decoder
	if(pthis->audio_infor.a_isvalid)
	{
		pfile->a_time = pfile->f_playtime;	
		pfile->a_duration	= 0;
		//maybe need to...by dnn 2008/4/28
		//modified by yjz 2009/2/10
		sample_rate	= pfile->mp_filehandle->audio_outinfor.a_outsmplrate;
			
		dma_count = sample_rate*4*(pfile->a_time/1000);		
		if(pfile->a_time%1000)
		{
			dma_count+=sample_rate*4*(pfile->a_time%1000)/1000;	
		}
		MMD_BIU_Set_Device_Status(IDX_AUD_DMA_COUNT, dma_count);
		if((pthis->fs_filetype[0] == 'm')&&(pthis->fs_filetype[1] == 'i')&&(pthis->fs_filetype[2] == 'd'))
		{
			parsefile = pthis->parserinfor;
			SeekInfo = (SEEK_INFO*)(parsefile->vm_input.SeekInfo);			
			result = media_player_audio_decseek_smtd((MP_U32)SeekInfo,sizeof(SEEK_INFO));
		}
		else
		{
			result = media_player_audio_decseek_smtd(0,0);			
		}		
		/*if(result)
		{	MMD_ERROR_Deal(CMD_FAILED,MAJ_PLY);
			return result;
		}*/
	}
	//seek video decoder
	if(pthis->video_infor.v_isvalid)
	{
		pfile->v_time = pfile->f_playtime;
		pfile->v_duration 	= 0;
		pthis->video_decoder->seek(pthis->video_decinfor, seekpos);
	}
	// post process reset
	#if ME_AUDIO_POSTPROC_ENABLE
	if(pthis->audio_infor.a_isvalid)
	{
		pnod = pthis->audio_postproc->first;
		while(pnod)
		{
			effectproc = (me_effectproc_t*)pnod->content;
			effectproc->reset(effectproc);
			pnod = pnod->next;
		}
	}
	#endif
	//file seek ok, if is stop then return success
	if(pfile->f_playstatus==MEDIA_FILE_STOP)
	{	 
		mp_debug(("media_sysstatus->ply_videocounter = %d ", media_sysstatus->ply_videocounter));
		mp_debug(("media_sysstatus->ply_audiocounter = %d ", media_sysstatus->ply_audiocounter));
		return MP_SUCCESS;
	}
	//other status need fill all data buffer(PLAYING, FASTPLAYING,PAUSE)
	pfile->f_playcontrol|= MP_AUDIO_PLAY_END;
	if(pthis->audio_infor.a_isvalid)
	{
		pfile->f_playcontrol &= ~ MP_AUDIO_PLAY_END;
		pfile->a_dmasamples = 0;
		pfile->a_lastleftsmpls = 0;
		media_player_audio_buffer_filled(pfile);
	}
	pfile->f_playcontrol|= MP_VIDEO_PLAY_END;
	if(pthis->video_infor.v_isvalid)
	{
		pfile->f_playcontrol &= ~ MP_VIDEO_PLAY_END;
		pfile->v_countbak = 0;
		pfile->v_duration = 0;
		pfile->v_lastdone = 0;
		pfile->v_samples = 1;
		pfile->v_framesdone = 1;
		media_player_video_buffer_filled(pfile);
	}
	//resume file status befor seek
	if(pfile->f_playstatus==MEDIA_FILE_PAUSE)
	{//before seek, file status is pause 
		if(pthis->video_infor.v_isvalid)
		{
			pfile->v_status=MEDIA_VIDEO_PAUSE;
			// video do not decoding now, so need clear this flag, Amanda Deng 2008/02/14
			pfile->v_framesdone =0;	
		}
		if(pthis->audio_infor.a_isvalid)
		{ 	
			pfile->a_status=MEDIA_AUDIO_PAUSE;
		}
	}
	else if(pfile->f_playstatus==MEDIA_FILE_PLAYING)
	{//before seek, file status is playing
		if(pthis->video_infor.v_isvalid)
		{
			pfile->v_status=MEDIA_VIDEO_PLAYING;
			media_player_video_start(pfile);
			media_player_timer_start();
		}
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
	{//before seek, file status is fastplaying
		pfile->f_playcontrol|= MP_AUDIO_PLAY_END;
		if(pthis->video_infor.v_isvalid)
		{
			pfile->v_status=MEDIA_VIDEO_PLAYING;
			media_player_video_start(pfile);
			media_player_timer_start();
		}
	}
	mp_debug(("media_sysstatus->ply_videocounter = %d ", media_sysstatus->ply_videocounter));
	mp_debug(("media_sysstatus->ply_audiocounter = %d ", media_sysstatus->ply_audiocounter));
	return MP_SUCCESS;
}

/*******************************************************************************************
Description:
	media player file close and all buffer free
Parameters:
	MP_FILE* pthis: player file handle
	MP_U32*accesspos:  file access time pos(s)
Return:	
	MP_SUCCESS: ok
	Others:			error code
NOTE: 
       this function maybe call in all thread
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.04.02		create first version
********************************************************************************************/
MP_U32 media_player_file_gettotaltime(VIM_MEDIA_PLYFILE pfilehandle, MP_U32*totaltime)
{
	MP_U32  result;
	MP_FILE *pthis = NULL;
	VIM_MEDIA_PLAYER_FILE* pfile;
	
	pfile = (VIM_MEDIA_PLAYER_FILE*)pfilehandle;
	result = media_player_check_filehandle(pfile);
	if(result)			
		return result;
	pthis = pfile->mp_filehandle;
	if(!pthis->audio_infor.a_isvalid)
		{	pthis->audio_infor.a_totaltime = 0;	}
	if(!pthis->video_infor.v_isvalid)
		{	pthis->video_infor.v_totaltime = 0;	}
//	*totaltime = (pthis->audio_infor.a_totaltime>pthis->video_infor.v_totaltime) ? pthis->audio_infor.a_totaltime : pthis->video_infor.v_totaltime;	
//by dnn 2008/4/11
	*totaltime = 0;
	if(pthis->video_infor.v_isvalid) {
		*totaltime = pthis->video_infor.v_totaltime;
	} else {
		*totaltime = pthis->audio_infor.a_totaltime;
	}
	return MP_SUCCESS;
}

/*******************************************************************************************
Description:
	media player file close and all buffer free
Parameters:
	MP_FILE* pthis: player file handle
	MP_U32*accesspos:  file access time pos(s)
Return:	
	MP_SUCCESS: ok
	Others:			error code
NOTE: 
       this function maybe call in all thread
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.04.02		create first version
********************************************************************************************/
MP_U32 media_player_file_getplaytime(VIM_MEDIA_PLYFILE pfilehandle, MP_U32*playtime)
{
	MP_U32  result, current_ticks,delta_ticks;
	VIM_MEDIA_PLAYER_FILE* pfile;
	
	pfile = (VIM_MEDIA_PLAYER_FILE*)pfilehandle;
	result = media_player_check_filehandle(pfile);
	if(result)			
		return result;

	if(pfile->a_status!=MEDIA_AUDIO_STOP)
	{
		//delay for current frame duration
		current_ticks 	= MMD_SYS_GetTimer2Value();//us//get_system_time_ticks();	
		delta_ticks 	= pfile->f_basetime;
		if(current_ticks<pfile->f_basetime)
		{
			delta_ticks = 0xffffffff -pfile->f_basetime;
			delta_ticks += current_ticks;
		}
		else
			{delta_ticks = current_ticks-pfile->f_basetime;}
		pfile->f_basetime = current_ticks;			
		pfile->f_playtime += delta_ticks/1000;//1000us //delta_ticks*10;		//(n*10ms ticks to ms)
	}
	else
	{
		pfile->f_playtime = pfile->v_time;
	}
	
	*playtime = pfile->f_playtime;
	return MP_SUCCESS;
}

/*******************************************************************************************
Description:
	get media player file property(include audio infor, video ifnor and other ID3 infor)
Parameters:
	MP_FILE* pthis: 			player file handle
	VIM_MEDIA_FILE_PROPERTY*pProperty:  property structor point
Return:	
	MP_SUCCESS: ok
	Others:			error code
NOTE: 
       this function maybe call in all thread
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.08.14		create first version
********************************************************************************************/
MP_U32 media_player_file_getproperty(VIM_MEDIA_PLYFILE pfilehandle,  VIM_MEDIA_FILE_PROPERTY *property)
{
	MP_U32   	  			result;
	me_property      			property_infor;
	MP_FILE 		  			*pthis = NULL;
	VIM_MEDIA_PLAYER_FILE* pfile;
	
	pfile = (VIM_MEDIA_PLAYER_FILE*)pfilehandle;
	result = media_player_check_filehandle(pfile);
	if(result)			
		return result;

	pthis = pfile->mp_filehandle;
	
	if(property_type_is(property->stype,VIM_MEDIA_FILE_PROPERTY_TYPE))
	{
		mp_memcpy((char*)property->property.filetype,
					 (const char*)pthis->fs_filetype, 4);
		property->size = 5;
		result = MP_SUCCESS;
	}
	else if(property_type_is(property->stype,VIM_MEDIA_FILE_PROPERTY_AUDIO_INFOR))
	{
		property->property.audioinfor.a_bits			= pthis->audio_infor.a_bits;
		property->property.audioinfor.a_chn			= pthis->audio_infor.a_chn;
		property->property.audioinfor.a_smplrate		= pthis->audio_infor.a_smplrate;
		property->property.audioinfor.a_totalframe	= pthis->audio_infor.a_totalframe;
		property->property.audioinfor.a_totaltime		= pthis->audio_infor.a_totaltime;
		property->property.audioinfor.a_bitrate		= pthis->audio_infor.a_bitrate;
		mp_memcpy((char*)property->property.audioinfor.a_codec, 
					 (const char*)pthis->audio_infor.a_codec, 4);
		property->size = sizeof(VIM_AUDIO_BASEINFOR);
		result = MP_SUCCESS;
	}
	else if(property_type_is(property->stype,VIM_MEDIA_FILE_PROPERTY_VIDEO_INFOR))
	{
		property->property.videoinfor.v_imagewidth		= pthis->video_infor.v_imagewidth;
		property->property.videoinfor.v_imageheight		= pthis->video_infor.v_imageheight;
		property->property.videoinfor.v_fps			= pthis->video_infor.v_fps;
		property->property.videoinfor.v_totalframe	= pthis->video_infor.v_totalframe;
		property->property.videoinfor.v_totaltime		= pthis->video_infor.v_totaltime;
		mp_memcpy((char*)property->property.videoinfor.v_codec, 
					 (const char*)pthis->video_infor.v_codec, 4);
		property->size = sizeof(VIM_VIDEO_BASEINFOR);
		result = MP_SUCCESS;
	}
	else if(property_type_is(property->stype,VIM_MEDIA_FILE_PROPERTY_AUDIO_OUTINFOR))
	{
		property->property.audioout.a_outbits		= pthis->audio_outinfor.a_outbits;
		property->property.audioout.a_outchn	= pthis->audio_outinfor.a_outchn;
		property->property.audioout.a_outsmplrate	= pthis->audio_outinfor.a_outsmplrate;
		property->size = sizeof(VIM_AUDIO_OUTINFOR);
		result = MP_SUCCESS;
	}
	else if(property_type_is(property->stype,VIM_MEDIA_FILE_PROPERTY_VIDEO_OUTINFOR))
	{
		//here maybe need get from decoder 
		if(pthis->video_infor.v_isvalid)
		{
			pthis->video_decoder->get_outputparam(pthis->video_decinfor, (void*)&pthis->video_outinfor);
		}
		property->property.videoout.v_outwidth	= pthis->video_outinfor.v_outwidth;
		property->property.videoout.v_outheight	= pthis->video_outinfor.v_outheight;
		property->property.videoout.v_outxpos	= pthis->video_outinfor.v_outxpos;
		property->property.videoout.v_outypos	= pthis->video_outinfor.v_outypos;
		property->property.videoout.v_outrotation= pthis->video_outinfor.v_outrotation;
		property->property.videoout.v_outeffect	= pthis->video_outinfor.v_outeffect;
		property->size = sizeof(VIM_VIDEO_OUTINFOR);
		result = MP_SUCCESS;
	}
	else
	{
		property_infor.paddress = (ME_U32)property->property.title;
		property_infor.psize = PROPERTY_MAX_SIZE;
		mp_memcpy((char*)property_infor.type, (const char*)property->stype, 8);
		result = pthis->id3->get_property(pthis->id3infor, &property_infor);
		property->size = property_infor.size;
	}
	
	return result;
}
/*******************************************************************************************
Description:
	get mp3 file's ID3 infor
Parameters:
	VIM_MEDIA_PLYFILE pfilehandle: 			player file handle
	VIM_MEDIA_FILE_PROPERTY*pProperty:  property structor point
Return:	
	MP_SUCCESS: ok
	Others:			error code
NOTE: 
       this function maybe call in all thread
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			yjz			             2009.02.12		create first version
********************************************************************************************/
MP_U32 media_player_file_get_mp3_id3property(VIM_MEDIA_PLYFILE pfilehandle,  VIM_MEDIA_FILE_PROPERTY *property)
{
	MP_U32   	  			result;
	me_property      			property_infor;
	MP_FILE 		  			*pthis = NULL;
	VIM_MEDIA_PLAYER_FILE* pfile;
	
	pfile = (VIM_MEDIA_PLAYER_FILE*)pfilehandle;
	result = media_player_check_filehandle(pfile);
	if(result)			
		return result;
	pthis = pfile->mp_filehandle;
	//judge that whether it is a mps file
	if(!((pthis->fs_filetype[0]=='m')&&(pthis->fs_filetype[1]=='p')&&(pthis->fs_filetype[2]=='3')))
	{
		printf("Just Supoort mp3 file\n");
		return ME_FAILE;
	}	
	//init
	property_infor.paddress = (ME_U32)property->property.title;
	property_infor.psize = PROPERTY_MAX_SIZE;
	mp_memcpy((char*)property_infor.type, (const char*)property->stype, 8);
	result = pthis->id3->get_property(pthis->id3infor, &property_infor);
	property->size = property_infor.size;	
	if(!me_cmp((ME_PU08)property->stype, (ME_PU08)VIM_MEDIA_FILE_PROPERTY_TITLE, 5))
	{			
		me_memcpy((ME_S08 *)property->property.title,(char *)property_infor.paddress,property_infor.size);
	}
	else if(!me_cmp((ME_PU08)property->stype, (ME_PU08)VIM_MEDIA_FILE_PROPERTY_ARSTIST, 6))
	{		
		me_memcpy((ME_S08 *)property->property.artist,(char *)property_infor.paddress,property_infor.size);
	}
	else if(!me_cmp((ME_PU08)property->stype, (ME_PU08)VIM_MEDIA_FILE_PROPERTY_ALBUM, 5))
	{			
		me_memcpy((ME_S08 *)property->property.album,(char *)property_infor.paddress,property_infor.size);
	}
	else if(!me_cmp((ME_PU08)property->stype, (ME_PU08)VIM_MEDIA_FILE_PROPERTY_YEAR, 4))
	{		
		me_memcpy((ME_S08 *)property->property.year,(char *)property_infor.paddress,property_infor.size);
	}
	else if(!me_cmp((ME_PU08)property->stype, (ME_PU08)VIM_MEDIA_FILE_PROPERTY_COMMENT, 7))
	{		
		me_memcpy((ME_S08 *)property->property.comment,(char *)property_infor.paddress,property_infor.size);
	}
	else if(!me_cmp((ME_PU08)property->stype, (ME_PU08)VIM_MEDIA_FILE_PROPERTY_TRACK, 5))
	{			
		property->property.track = ((char *)property_infor.paddress)[0];		
	}
	else if(!me_cmp((ME_PU08)property->stype, (ME_PU08)VIM_MEDIA_FILE_PROPERTY_GENRE, 5))
	{			
		me_memcpy((ME_S08 *)property->property.genre,(char *)property_infor.paddress,property_infor.size);
	}
	return result;
	
}


/*******************************************************************************************
Description:
	get media player file property(include audio infor, video ifnor and other ID3 infor)
Parameters:
	MP_FILE* pthis: 			player file handle
	VIM_MEDIA_FILE_PROPERTY*pProperty:  property structor point
Return:	
	MP_SUCCESS: ok
	Others:			error code
NOTE: 
       this function maybe call in all thread
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.08.14		create first version
********************************************************************************************/
MP_U32 media_player_file_setproperty(VIM_MEDIA_PLYFILE pfilehandle,  VIM_MEDIA_FILE_PROPERTY *property)
{
	me_video_outinfor			video_outinfor;
	me_audio_outinfor			audio_outinfor;
	MP_U32					tmpvalue;
	MP_FILE 		  			*pthis = NULL;
	MP_U32   	  			result = MP_SUCCESS;
	
	VIM_MEDIA_PLAYER_FILE* pfile;
	
	pfile = (VIM_MEDIA_PLAYER_FILE*)pfilehandle;
	result = media_player_check_filehandle(pfile);
	if(result)			
		return result;

	pthis = pfile->mp_filehandle;
	
	if(!((pfile->f_playstatus==MEDIA_FILE_STOP)
		|(pfile->f_playstatus==MEDIA_FILE_PLAYING)
		|(pfile->f_playstatus==MEDIA_FILE_PAUSE)))
	{
		ME_ERROR(("MP_ERR_FILE_STATUS_ERROR"));
		return MP_ERR_FILE_STATUS_ERROR;
	}

	if(property_type_is(property->stype,VIM_MEDIA_FILE_PROPERTY_AUDIO_OUTINFOR))
	{
		if(property->size!=sizeof(VIM_AUDIO_OUTINFOR))
			return MP_ERR_PROPERTY_SIZE_ERROR;
		audio_outinfor.a_outbits		= pthis->audio_outinfor.a_outbits 	= property->property.audioout.a_outbits;
		audio_outinfor.a_outchn			= pthis->audio_outinfor.a_outchn	= property->property.audioout.a_outchn;
		audio_outinfor.a_outsmplrate	= pthis->audio_outinfor.a_outsmplrate= property->property.audioout.a_outsmplrate;
		//here maybe need update decoder output paramter
		if(pthis->audio_infor.a_isvalid)
		{
			pthis->audio_decoder->set_outputparam(pthis->audio_decinfor, (void*)&audio_outinfor);
		}
		result = MP_SUCCESS;
	}
	else if(property_type_is(property->stype,VIM_MEDIA_FILE_PROPERTY_VIDEO_OUTINFOR))
	{
		mp_print(("video_out->srcwidth = %d",property->property.videoout.v_srcwidth));
		mp_print(("video_out->srcheight = %d",property->property.videoout.v_srcheight));
		mp_print(("video_out->srcxpos = %d",property->property.videoout.v_srcxpos));
		mp_print(("video_out->srcypos = %d",property->property.videoout.v_srcypos));
		mp_print(("video_out->width = %d",property->property.videoout.v_outwidth));
		mp_print(("video_out->height = %d",property->property.videoout.v_outheight));
		mp_print(("video_out->xpos = %d",property->property.videoout.v_outxpos));
		mp_print(("video_out->ypos = %d",property->property.videoout.v_outypos));
		mp_print(("video_out->rotation = %d",property->property.videoout.v_outrotation));
		mp_print(("video_out->effect = %d",property->property.videoout.v_outeffect));

		if(property->size!=sizeof(VIM_VIDEO_OUTINFOR))
			return MP_ERR_PROPERTY_SIZE_ERROR;
		video_outinfor.v_srcwidth=pthis->video_outinfor.v_srcwidth=property->property.videoout.v_srcwidth;
		video_outinfor.v_srcheight=pthis->video_outinfor.v_srcheight=property->property.videoout.v_srcheight;
		video_outinfor.v_srcxpos=pthis->video_outinfor.v_srcxpos=property->property.videoout.v_srcxpos;
		video_outinfor.v_srcypos=pthis->video_outinfor.v_srcypos=property->property.videoout.v_srcypos;
		tmpvalue = property->property.videoout.v_outwidth;
		video_outinfor.v_outwidth	= pthis->video_outinfor.v_outwidth	=(tmpvalue)?tmpvalue:80;
		tmpvalue = property->property.videoout.v_outheight;
		video_outinfor.v_outheight	= pthis->video_outinfor.v_outheight	=(tmpvalue)?tmpvalue:80;  
		video_outinfor.v_outxpos		= pthis->video_outinfor.v_outxpos	=property->property.videoout.v_outxpos;
		video_outinfor.v_outypos		= pthis->video_outinfor.v_outypos	=property->property.videoout.v_outypos;
		video_outinfor.v_outrotation	= pthis->video_outinfor.v_outrotation=property->property.videoout.v_outrotation;
		video_outinfor.v_outeffect	= pthis->video_outinfor.v_outeffect	=property->property.videoout.v_outeffect;
		
		//here maybe need update decoder output paramter
		if(pthis->video_infor.v_isvalid)
		{
			pthis->video_decoder->set_outputparam(pthis->video_decinfor, (void*)&video_outinfor);
		}
		result = MP_SUCCESS;
	}
	return result;
}


F_FILE *file_rec;


MR_U32 media_Write_TmpToFile(MR_CHAR * fileName,MR_U32 writeIndex)
{
	void *fs_filehandle;
	MR_U32 iWriteRet=0,result=0;
	fs_filehandle =filesys_fileopen((void *)fileName, MEDIA_FOPEN_WRITE_BINARY);
	if(!fs_filehandle)
	{
		ME_ERROR(("MP_ERR_FS_OPEN_FILE_ERROR"));
        FARF(MSG,("MP_ERR_FS_OPEN_FILE_ERROR  L4450"));
		return MP_ERR_FS_OPEN_FILE_ERROR;
	}
	iWriteRet = filesys_filewrite(fs_filehandle, SerPicfilehandle[writeIndex].FileAddress, SerPicfilehandle[writeIndex].FileTotalLength);
	if(iWriteRet != SerPicfilehandle[writeIndex].FileTotalLength)
	{
		ME_ERROR(("MP_ERR_FS_WRITE_FILE_ERROR"));
        FARF(MSG,("MP_ERR_FS_OPEN_FILE_ERROR  L4457"));

		return MP_ERR_FS_OPEN_FILE_ERROR;
	}
	result =filesys_fileclose((void*)fs_filehandle);
	if(result)
	{
		ME_ERROR(("MP_ERR_FS_CLOSE_FILE_ERROR"));
		return MP_ERR_FS_CLOSE_FILE_ERROR;
	}
	return MP_SUCCESS;
}

/*******************************************************************************************
Description:
	media recorder file open
Parameters:
	VIM_MEDIA_RECFILE_INPUTINFOR *recfile: recorde file infor structor point
	VIM_MEDIA_RECORDER_FILE** pthis: recorder file handle point's point
Return: 
	MP_SUCCESS: open player file ok
	Others: 		error code
Modify History:
	Version 		Modifyby				DataTime			modified
	   0.1			Amanda Deng 		2007.07.02		create first version
********************************************************************************************/
MR_U32 media_recorder_file_open(MP_U32 inputfile, VIM_MEDIA_RECFILE *rfilehandle,MR_U32 Callback)
{
	MR_U32			result,memsize;
	me_record_infor 	rec_infor;
	void				*fs_filehandle;
	MEMORY_FILE		*mem_filehandle;
	VIM_MEDIA_RECORDER_FILE 	*pthis;
	VIM_MEDIA_RECFILE_INPUTINFOR *recfile;
	
	recfile=(VIM_MEDIA_RECFILE_INPUTINFOR *)inputfile;
	
	if(Callback)
		g_recCallback = (Func_RecCallback)Callback;
	
	// 1. check open file number
	if(media_sysstatus->rcfilelst[0]!=NULL)
	{
		mp_print(("-----media recorder only support record 1 file at same time!"));
		return MP_ERROR;
	}
	// 2. process src file parameter get file handle
	if(recfile->fs_filestore==FILE_IN_CORE_FILE)
	{
		if(!uSerPicCount)
		{
			fs_filehandle =filesys_fileopen((void *)recfile->fs_filename, MEDIA_FOPEN_WRITE_BINARY);
			if(!fs_filehandle)
			{
				ME_ERROR(("MP_ERR_FS_OPEN_FILE_ERROR"));
                FARF(MSG,("MP_ERR_FS_OPEN_FILE_ERROR  L4514"));

				return MP_ERR_FS_OPEN_FILE_ERROR;
			}
		}
		else
		{
			fs_filehandle = (void *)&SerPicfilehandle[uPhotoCount];
		}
		pthis= (VIM_MEDIA_RECORDER_FILE*)mp_malloc( sizeof(VIM_MEDIA_RECORDER_FILE));
		if(!pthis)
		{
			filesys_fileclose((void*)fs_filehandle);
			ME_ERROR(("MP_ERR_MALLOC_MEMORY_ERROR"));
			return MP_ERR_MALLOC_MEMORY_ERROR;
		}
		MMD_BIU_Set_Device_Status(IDX_FILE_STATUS, 0);
	}
	else if(recfile->fs_filestore==FILE_IN_BASEBAND_RAM) // open and write file on base band
	{
		//MMD_FILE_Open((MMD_U08 *)recfile->fs_filename,(MMD_U08 *)MEDIA_FOPEN_WRITE_BINARY,&file_rec);
		memsize = 0x7D000;
#if 0        
		mem_filehandle = (MEMORY_FILE*)memory_fileopen((void*)0x40000000, &memsize);
#else
		memory_set_fileread_mode(VC_OFM_CREATE); // set file create flag for memory_fileopen and memory_filewrite
		mem_filehandle = (MEMORY_FILE*)memory_fileopen((void*)recfile->fs_filename, &memsize);
#endif
		fs_filehandle = mem_filehandle/*->FileAddress*/;
		mp_printf("FileAddress (0x%x) \n",(MR_U32)mem_filehandle->FileAddress);
		pthis= (VIM_MEDIA_RECORDER_FILE*)mp_malloc( sizeof(VIM_MEDIA_RECORDER_FILE));
		if(!pthis)
		{
			memory_fileclose((void*)mem_filehandle);
			ME_ERROR(("MP_ERR_MALLOC_MEMORY_ERROR"));
			return MP_ERR_MALLOC_MEMORY_ERROR;
		}
	}
	// 3. media engine file creat
	
	mp_memset((unsigned char*)pthis, 0, sizeof(VIM_MEDIA_RECORDER_FILE));
	pthis->fs_filehandle =(MR_U32)fs_filehandle; 
	pthis->f_limitedsize = recfile->f_limitedsize;
	pthis->f_limitedtime = recfile->f_limitedtime;
    
	// 4. set video record information
	rec_infor.v_recwidth =	recfile->v_recwidth;
	rec_infor.v_recheight =  recfile->v_recheight;
	rec_infor.v_recfps =  recfile->v_recfps;
	mp_memcpy((char*)rec_infor.v_codec, (const char*)&recfile->v_codec, 4);
    
	// 5. set video input information(preview information)
	rec_infor.v_previewwidth = media_sysstatus->preview_infor.preview_dispwidth;
	rec_infor.v_previewheight = media_sysstatus->preview_infor.preview_dispheight;
	rec_infor.v_previewxpos = media_sysstatus->preview_infor.preview_dispxpos;
	rec_infor.v_previewypos = media_sysstatus->preview_infor.preview_dispypos;
	rec_infor.v_inputwidth = media_sysstatus->preview_infor.recvideo_width;
	rec_infor.v_inputheight= media_sysstatus->preview_infor.recvideo_height;
	rec_infor.v_inputeffect =  media_sysstatus->preview_infor.preview_effect;
	rec_infor.v_inputrotation = VIM_HAPI_ROTATE_0;	
	rec_infor.v_inputfps  = 30;
	recorder_stop=0;
	// 6. set audio record information
	rec_infor.a_recbits = recfile->a_recbits;
	rec_infor.a_recchn = recfile->a_recchn; // debug for vedio file recording 
	rec_infor.a_recsmplrate = recfile->a_recsmplrate;
	rec_infor.a_recbitrate	= recfile->a_recbitrate;
	mp_memcpy((char*)rec_infor.a_codec, (const char*)&recfile->a_codec, 4);
	/*if(rec_infor.a_recchn )
	{
		media_recorder_set_samplerate_smtd(recfile->a_recsmplrate);
	}*/
	 media_sysstatus->codec_inputinfor.a_inputsmplrate = recfile->a_recsmplrate;
	rec_infor.a_inputbits = media_sysstatus->codec_inputinfor.a_inputbits;
	rec_infor.a_inputchn = media_sysstatus->codec_inputinfor.a_inputchn;
	rec_infor.a_inputsmplrate =  media_sysstatus->codec_inputinfor.a_inputsmplrate;
	// 8.set file information
	pthis->fs_filestore   = recfile->fs_filestore;
	if(recfile->fs_filestore==FILE_IN_CORE_FILE)
	{	
		pthis->fs_filehandle =(MP_U32)fs_filehandle; 
		rec_infor.fs_filehandle = (void*)fs_filehandle;
		rec_infor.fs_filefunc = &g_filesys_file;
	}
	else //srcfile->StoreType==FILE_IN_BASEBAND_RAM
	{	
		pthis->fs_filehandle =(MP_U32)mem_filehandle; 
		rec_infor.fs_filehandle = (void*)mem_filehandle;
		rec_infor.fs_filefunc = &g_memory_file;
	}
	mp_memcpy((char*)rec_infor.fs_filetype, (const char*)&recfile->fs_filetype, 4);
	mp_printf("fs_filetype(%s)\n",rec_infor.fs_filetype);
	// 9. media engine file open 
	result = me_recorder_open(&rec_infor, &pthis->mr_filehandle);
	mp_print(("-----me_recorder_open() return = 0x%08X",result));
	if(result)
	{
		if(recfile->fs_filestore==FILE_IN_CORE_FILE)
		{
			filesys_fileclose((void*)fs_filehandle);
			mp_free(pthis);
		}
		else
		{
			memory_fileclose((void*)mem_filehandle);
			mp_free(pthis);
		}
		return result;
	}
	//8000 sample audio need noice reduction process
	if(rec_infor.a_recchn )
	{
		media_recorder_set_samplerate_smtd(recfile->a_recsmplrate);
	}
	// 10. update meida engine file status
	pthis->f_recordstatus = MEDIA_FILE_STOP;
	// 11. update media system file lst
	media_sysstatus->rcfilelst[0] = pthis;
	if(pthis->mr_filehandle->audio_infor.a_isvalid)
	{
		MMD_USER_WaitMICRS(1000);
		media_recorder_strinit_smtd((void *)pthis);
		mp_printf("a_inputsmplrate (%d) \n",pthis->mr_filehandle->audio_inputinfor.a_inputsmplrate);
	}
	(*rfilehandle) = (VIM_MEDIA_RECFILE)pthis;
	// 12. create timer, by lizg
	if(((pthis->mr_filehandle->fs_filetype[0]=='a')&&(pthis->mr_filehandle->fs_filetype[1]=='m')&&
		(pthis->mr_filehandle->fs_filetype[2]=='r')) ||((pthis->mr_filehandle->fs_filetype[0]=='m')&&(pthis->mr_filehandle->fs_filetype[1]=='p')&&
		(pthis->mr_filehandle->fs_filetype[2]=='3')))

		media_recorder_jpgcap_timer_creat();  //voice recoder
	else
		media_recorder_timer_creat();  //video recoder
	return MP_SUCCESS; 
}
/*******************************************************************************************
Description:
	media recorder file start record
Parameters:
	VIM_MEDIA_RECORDER_FILE* pthis: recorder file handle
Return: 
Modify History:
	Version 		Modifyby				DataTime			modified
	   0.1			Amanda Deng 		2007.07.02		create first version
********************************************************************************************/
MR_U32 media_recorder_file_start(VIM_MEDIA_RECFILE rfilehandle, MR_U32 Callback)
{
	MR_U32	result;
	MR_FILE* pthis;
	VIM_MEDIA_RECORDER_FILE* rfile;
	
	rfile = (VIM_MEDIA_RECORDER_FILE*)rfilehandle;
	result = media_recorder_check_filehandle(rfile);
	if(result)			
		return result;
	pthis = rfile->mr_filehandle;
	if(rfile->f_recordstatus!=MEDIA_FILE_STOP)
	{
		ME_ERROR(("MP_ERR_FILE_STATUS_ERROR"));
		return MP_ERR_FILE_STATUS_ERROR;
	}
	// initialize media file src, dst buffer
	if(!rfile->f_recordbufinit)
	{
		rfile->f_recordbufinit = 1;
		// initialize media file src, dst buffer
		result = media_recorder_buffer_initialize(pthis);
		if(result)
		{
			mp_error(("media_recorder_buffer_initialize() retrun =0x%08X",result));
			return result;
		}
	}
	//initialize audio status and audio buffer
	rfile->f_recordcontrol|= MR_AUDIO_RECORD_END;
	if(pthis->audio_infor.a_isvalid)
	{
		rfile->f_recordcontrol &= ~ MR_AUDIO_RECORD_END;
		pthis->muxer->set_audio_info(pthis->muxerinfor, &pthis->audio_infor);
		
		media_recorder_audio_buffer_initialize(rfile);
	}
	
	rfile->f_recordcontrol|= MR_VIDEO_RECORD_END;
	if(pthis->video_infor.v_isvalid)
	{
		rfile->f_recordcontrol &= ~ MR_VIDEO_RECORD_END;
		pthis->muxer->set_video_info(pthis->muxerinfor, &pthis->video_infor);
		media_recorder_video_buffer_initialize(rfile);
	}
	
	utime=0;
	result=media_recorder_start_record(rfile);
    if(result) //zhangxiang add
        return result;
	mp_debug(("file status = 0x%08X ", rfile->f_recordcontrol));
	// update player status
	rfile->f_recordstatus = MEDIA_FILE_RECORDING;

	return MP_SUCCESS;
}

/*******************************************************************************************
Description:
	media recorder file close and all buffer free
Parameters:
	VIM_MEDIA_RECORDER_FILE* pthis: recorder file handle
Return: 
	MP_SUCCESS: close ok
	Others: 		error code
Modify History:
	Version 		Modifyby				DataTime			modified
	   0.1			Amanda Deng 		2007.07.02		create first version
********************************************************************************************/
MR_U32 media_recorder_file_close(VIM_MEDIA_RECFILE rfilehandle)
{
	MR_U32 result, return_value = MP_SUCCESS;
	MR_U32			writestatus,timeout=0x1000;
	VIM_MEDIA_RECORDER_FILE* rfile;
	
	rfile = (VIM_MEDIA_RECORDER_FILE*)rfilehandle;
	//check recorder file handle
	if(media_sysstatus->rcfilelst[0]!=rfile)
		return MP_ERR_FILE_HANDLE_ERROR;
	//check audio status and stop audio recording	
	if(rfile->a_status==MEDIA_AUDIO_RECORDING)
	{
		while(!iFristWrite)
		{
			mp_printf("Not Write Can Not Quit!\n");
			MMD_USER_WaitMICRS(1000);
		}
		
		rfile->a_status = MEDIA_AUDIO_STOP;
		media_recorder_audio_stop(rfile);
		//media_recorder_jpgcap_timer_stop();

	}
	//check video status and stop video recording
	if(rfile->v_status!=MEDIA_VIDEO_STOP)
	{
		rfile->v_status = MEDIA_VIDEO_STOP;
		media_recorder_video_stop(rfile);
		MMD_BIU_Get_Device_Status(5, &endduration);
		mp_printf("endduration is %x\n",endduration);		
		
	}
	rfile->f_recordstatus = MEDIA_FILE_STOP;
	
	//if buffer have initialize then release buffer
	if(rfile->mr_filehandle->video_infor.v_isvalid==1)
	{
               while(timeout)
               {
                   	MMD_BIU_Get_Device_Status(IDX_FILE_STATUS, &writestatus);
                     if(writestatus & 0x1)
                     {
                         	//mp_print("Last Write Is Not Flished!writeflag(0x%x)\n",writestatus);
                         	timeout--;	
                     }
                     else
                     {
                         	break;
                     }
               }                   		
               if(!timeout)
               {
                     ME_ERROR(("MP_ERR_FS_WRITE_FILE_ERROR"));
                     return MP_ERR_FS_CLOSE_FILE_ERROR;
               }
	}

	if(rfile->f_recordbufinit /*&& !uSerPicCount*/)
		media_recorder_buffer_release(rfile->mr_filehandle);
	
	// release media player file struct
	
	result = me_recorder_close(rfile->mr_filehandle);
	if(result)
	{
		mp_print(("-----me_recorder_close() return = 0x%08X ",result));
		return_value = result;
	}

	
	
	if(rfile->fs_filestore==FILE_IN_CORE_FILE)
	{
		if(!uSerPicCount )
		{
			result =filesys_fileclose((void*)rfile->fs_filehandle);
			if(result)
			{
				ME_ERROR(("MP_ERR_FS_CLOSE_FILE_ERROR"));
				return_value = MP_ERR_FS_CLOSE_FILE_ERROR;
			}
		}
	}
	else
	{
		result =memory_fileclose((void*)rfile->fs_filehandle);
		if(result)
		{
			ME_ERROR(("MP_ERR_FS_CLOSE_FILE_ERROR"));
			return_value = MP_ERR_FS_CLOSE_FILE_ERROR;
		}
	}
	mp_free(rfile);
	media_sysstatus->rcfilelst[0] = NULL;
	// Release timer, by liizg
	MMD_USER_Timer_Release();
	g_recCallback=NULL;	
	return return_value;
}

extern MR_U32 iFristWrite;
/*******************************************************************************************
Description:
	media recorder file stop recorde
Parameters:
	VIM_MEDIA_RECORDER_FILE* pthis: recorder file handle
Return: 
Modify History:
	Version 		Modifyby				DataTime			modified
	   0.1			Amanda Deng 		2007.07.02		create first version
********************************************************************************************/
MR_U32 media_recorder_file_stop(VIM_MEDIA_RECFILE rfilehandle)
{
	MR_U32	result;
	//audio_stream_infor	*audiobuffer;
	//video_stream_infor	*videobuffer;
	//MR_U32			buffer_index;
	//VIM_MSG msg;
	MR_U32			writestatus,timeout=50,timeout_stop=300;
	MR_U32  current_ticks, delta_ticks;
	VIM_MEDIA_RECORDER_FILE* rfile;
	MR_FILE* pthis;
		
	rfile = (VIM_MEDIA_RECORDER_FILE*)rfilehandle;

	//check recorder file handle
	result = media_recorder_check_filehandle(rfile);
	if(result)			
		return result;
	//check file status
		
	if (rfile->f_recordstatus == MEDIA_FILE_STOP)
		return MP_SUCCESS; 

	pthis = rfile->mr_filehandle;
       if(pthis->video_infor.v_isvalid)
       {
		if((rfile->f_recordtime <rfile->f_limitedtime)&(rfile->f_recordsize< rfile->f_limitedsize))
		{		
       	media_recorder_timer_stop();
       
       	current_ticks 	= get_system_time_ticks();		
           	if(current_ticks<rfile->f_basetime)
           	{
           		delta_ticks = 0xffffffff -rfile->f_basetime;
           		delta_ticks += current_ticks;
           	}
        	else
        	{
        		delta_ticks = current_ticks-rfile->f_basetime;
        	}
        	
        	rfile->f_basetime = current_ticks;		
        	rfile->f_recordtime += delta_ticks/1000;	//(n*10ms ticks to ms)
        	utime+=delta_ticks%1000;
		if(utime>=1000)
		{
			rfile->f_recordtime +=1;
			utime-=1000;
		}
       
       	rfile->f_limitedtime=(rfile->f_recordtime-(rfile->f_recordtime%1000)+1000);     
        MSG_ERROR("rfile->f_limitedtime(%d), rfile->f_basetime(%d), delta_ticks(%d)", rfile->f_limitedtime, rfile->f_basetime, delta_ticks);
       	media_recorder_timer_start();	
		recorder_stop=1;

		while(timeout_stop)
		{
			if(recorder_stop==1)
			{
				MMD_USER_WaitMICRS(1000);
                media_recorder_time_dsr(); //zhangxiang add 09/01/09
			}
			else
			{
				break;
			}
			timeout_stop--;
		}

		if(!timeout_stop)
 		{
 			ME_ERROR(("not recorder timer stop HISR\n"));
 			return  MP_ERROR;
 		}
		}		       	
       }
	
	//check audio status and stop audio recording	
	
	if(rfile->a_status==MEDIA_AUDIO_RECORDING)
	{
		while(!iFristWrite)
		{
			mp_printf("Not Write Can Not Quit!\n");
			MMD_USER_WaitMICRS(1000);
            media_recorder_time_dsr(); //zhangxiang add 09/01/09
		}
		rfile->a_status = MEDIA_AUDIO_STOP;
		
		media_recorder_audio_stop(rfile);
      
		
		}
	
	//check video status and stop video recording
	
	if(rfile->v_status!=MEDIA_VIDEO_STOP)
	{
		rfile->v_status = MEDIA_VIDEO_STOP;
		media_recorder_video_stop(rfile);
		MMD_BIU_Get_Device_Status(5, &endduration);
		mp_printf("endduration is %x\n",endduration);		
		mp_debug(("media_recorder_video_stop "));
	}
	
	//update file status
	rfile->f_recordstatus = MEDIA_FILE_STOP;
	mp_debug(("rfile->f_recordstatus = %d ", rfile->f_recordstatus));
	
	while(timeout)
	{
		MMD_BIU_Get_Device_Status(IDX_FILE_STATUS, &writestatus);
 		if(writestatus & 0x1)
 		{
 			MMD_USER_WaitMICRS(1000);//mp_print("Last Write Is Not Flished!writeflag(0x%x)\n",writestatus);
 		}
 		else
 		{
 			break;
 		}
		timeout--;	
 	}           
	
 	if(!timeout)
 	{
 		ME_ERROR(("MP_ERR_FS_WRITE_FILE_ERROR"));
 		return  MP_ERR_FS_CLOSE_FILE_ERROR;
 	}
	//check file record control status
	if(g_recCallback) 
	{
		(*g_recCallback)(MMD_REC_FILE_END,0,0);		
	}
	if((rfile->f_recordcontrol & MR_AUDIO_RECORD_END)==MR_AUDIO_RECORD_END)
	{
		return MP_SUCCESS;
	}
	return MP_SUCCESS;
}

// temp use later combile into media_player_file_open
#ifdef RV_ENABLE 
MP_U32 g_Fifo_Addr,g_FileHandle,g_oldaddr=0;
VIM_MEDIA_PLAYER_FILE* g_PlayFileHandle;
fs_callbacks*		g_pFilefunc;
MP_U32 gTotalTime;
MP_U32 g_RV_State;//播放状态标志(防止重入)
MP_U32 g_RV_Isr_Count = 0;//
MP_U32 g_RV_Lock;

extern TIMER_STATE	emMMDTimerState;
extern UPDATE_STATE emMMDUpdateState;
extern BLAYER_PARAMS_FILL strMMDFillBlParams;
extern void RVDemo_FlushTime_Isr(void);
void media_rv_time_dsr(void)
{
   	MP_U32 ret,addr,len,realsize;

   	MP_U32 read_idx, read_size;
	MMD_RV_BUF *pCurRVBuf;
	MMD_MSG msg;
#ifdef RV_BASEBAND_MEM
	MP_S08* pBuf;
#endif	
	mp_print(("media_rv_time_dsr-"));
//	RVDemo_FlushTime_Isr();
	if(media_sysstatus->plyfilelst[0]==NULL)
		{
		mp_print(("e1\n"));
		return;
		}
	if(media_sysstatus->plyfilelst[0]->f_playstatus==MEDIA_FILE_STOP)
		{
		mp_print(("e2\n"));
		return;
		}

   	ret = RV_Fifo_Get_Empty(&addr, &len);
	MMD_ASSERT(ret==0);
	mp_print(("s1-"));
	if(len ==0)
		{
		mp_print(("e3\n"));
		return;
		}
	if(g_oldaddr==addr)
		{
		mp_print(("e4\n"));
		return;
		}
	g_oldaddr = addr;
	mp_debug(("addr=0x%x,len=0x%x\n", addr,len));
	
#ifdef RV_BASEBAND_MEM
	pBuf = mp_malloc(len);
	MMD_ASSERT(pBuf!=NULL);
	realsize = g_pFilefunc->read_func((void*)g_PlayFileHandle->fs_filehandle, pBuf, len);
	if(realsize==0)
		{
		return;
		}
	Vm_WrSram(addr, pBuf, realsize);
	mp_free(pBuf);
#else
	mp_print(("s2(addr=0x%x,len=0x%x)-",addr,len));
	read_idx = g_RV_Stream_Buf.buffer_read;
	pCurRVBuf = &g_RV_Stream_Buf.buffer[read_idx];
	// no data in buffer
	if(pCurRVBuf->data_length==0)
	{
		if(g_plyCallback) 
		{
			(*g_plyCallback)(MMD_PLY_FILE_END);
		}
		mp_error(("rv file buffer empty\n"));
		mp_print(("e5\n"));
		return;
	}

	realsize = 0;
	while(len)
	{
		mp_print(("s3-"));
		read_idx = g_RV_Stream_Buf.buffer_read;
		pCurRVBuf = &g_RV_Stream_Buf.buffer[read_idx];
		// no data in buffer
		if(pCurRVBuf->data_length==0)
			{
			mp_print(("s4-"));
			break;
			}
		if(pCurRVBuf->data_length>len)
		{
			// data in one buffer
			mp_print(("s5-"));
			read_size = len;
			Vm_WrSram(addr+realsize, (MP_S08*)(pCurRVBuf->buffer_address+pCurRVBuf->cursor_read), read_size);			
			mp_print(("s6-"));
			pCurRVBuf->data_length -= read_size;
			pCurRVBuf->cursor_read += read_size;
			len -= read_size;
			realsize += read_size;
			mp_print(("s7-"));
		}
		else
		{
			read_size = pCurRVBuf->data_length;
			Vm_WrSram(addr+realsize, (MP_S08*)(pCurRVBuf->buffer_address+pCurRVBuf->cursor_read), read_size);			
			pCurRVBuf->data_length -= read_size;
			pCurRVBuf->cursor_read += read_size;
			len -= read_size;
			realsize += read_size;
			if(pCurRVBuf->is_over)
			{
				mp_error(("rv file stream end\n"));
				break;
			}	
			msg.CMD = MMD_RV_FILE_READ;
			msg.OP1 = read_idx;
			MMD_USER_Msg_Send(&msg);
			read_idx++;
			if(read_idx>=RV_BUF_NUM)
				read_idx = 0;
			g_RV_Stream_Buf.buffer_read = read_idx;
		}
	}
#endif

	mp_debug(("addr=0x%x,realsize=0x%x\n", addr,realsize));
	if(realsize!=0)
	{
		ret = RV_Fifo_Write_Notify(addr, realsize);
		MMD_ASSERT(ret==0);
	}
	if(g_RV_Lock)
		return;

	mp_debug(("media_rv_time_dsr()"));
}

void media_rv_refresh(void)
{
	if(emMMDTimerState == NO_TIMER_START)	return;
	if(emMMDUpdateState == NEED_UPDATE && !g_BIU_Count)
	{
		MMD_LCD_Refresh_In_Timer();
		emMMDUpdateState = UPDATED;
	}
}

MP_U32 media_rv_file_open(MP_U32 inputfile, VIM_MEDIA_PLYFILE*pfilehandle)
{
	VIM_MEDIA_PLAYER_FILE 		*pthis;
	fs_callbacks*		pFilefunc;
	VIM_MEDIA_PLYFILE_INPUTINFOR *srcfile;
	MP_U32 ret,addr,len,i,filelength,offset;
	MP_S08* pBuf;
	void* filehandle;
		
	srcfile=(VIM_MEDIA_PLYFILE_INPUTINFOR *)inputfile;
	g_RV_State = MEDIA_FILE_STOP;
	gTotalTime = 0;
	g_RV_Lock = 0;
	// check open file number
	if(media_sysstatus->ply_filecounter>=VIM_MEDIA_MAX_SUPPORT_FILE_NUMBER)
	{
		mp_print(("-----media player only support open %d file at same time!",VIM_MEDIA_MAX_SUPPORT_FILE_NUMBER));
		return MP_ERROR;
	}

	// set file operation function	
	if(srcfile->fs_filestore == FILE_IN_BASEBAND_RAM)
	{
        // Note: for rv_file_open, the feature RV_ENABLE is not defined in VC0848 system.
        memory_set_fileread_mode(VC_OFM_CREATE); // set file create flag for memory_fileopen and memory_filewrite
		filehandle = memory_fileopen((void*)srcfile->mem_fileaddr, (void*)&srcfile->mem_filelength);
		pFilefunc = &g_memory_baseband_file;
		filelength = srcfile->mem_filelength;
	}
	else if(srcfile->fs_filestore == FILE_IN_BASEBAND_FILE)
	{
		filehandle = MMD_USER_FS_File_Open(srcfile->fs_filename, 0);
		MMD_ASSERT(filehandle!=NULL);
		filelength = MMD_USER_FS_File_Get_Length(filehandle);
		MMD_ASSERT(filelength!=0);
		pFilefunc = &g_filesys_baseband_file;
	}

	// allocate file handle
	pthis= (VIM_MEDIA_PLAYER_FILE*)mp_malloc( sizeof(VIM_MEDIA_PLAYER_FILE));
	if(!pthis)
	{
		ME_ERROR(("MP_ERR_MALLOC_MEMORY_ERROR"));
		return MP_ERR_MALLOC_MEMORY_ERROR;
	}
	mp_memset((unsigned char*)pthis, 0, sizeof(VIM_MEDIA_PLAYER_FILE));
	pthis->fs_filehandle = (MP_U32)filehandle;
	pthis->fs_filestore = srcfile->fs_filestore;
	
	//Create memory pool
	ret = RV_Mem_Poll_Create();
	MMD_ASSERT(ret==0);
	
	//Init Fifo
	ret = RV_Fifo_Init(RV_FILE_FIFO_SIZE, &g_Fifo_Addr);
	MMD_ASSERT(ret==0);

	ret = RV_Fifo_Get_Empty(&addr, &len);
	MMD_ASSERT(ret==0);

	pBuf = mp_malloc(len);
	MMD_ASSERT(pBuf!=NULL);

	len = pFilefunc->read_func((void*)pthis->fs_filehandle, pBuf, len);
	MMD_ASSERT(len!=0);
#if 0
	MMDPrintf("Buffer end data\n");
	for(i=0;i<16;i++)
	{
		MMDPrintf("0x%02X  ", pBuf[len-i-1]);
	}
	MMDPrintf("\n");
#endif	
	Vm_WrSram(addr, pBuf, len);

	ret = RV_Fifo_Write_Notify(addr, len);
	MMD_ASSERT(ret==0);

	mp_free(pBuf);
	
	ret = RV_File_Open(filelength, &g_FileHandle, &offset);
	MMD_ASSERT(ret==0);
	MMDPrintf("index offset=0x%x\n", offset);
	
#ifdef RV_BASEBAND_MEM
#else
	// Alloc RV stream buffer memory
	for(i=0;i<RV_BUF_NUM;i++)
	{
		g_RV_Stream_Buf.buffer[i].buffer_address = (MMD_U32)mp_malloc(RV_BUF_SIZE);
		MMD_ASSERT(g_RV_Stream_Buf.buffer[i].buffer_address!=0);
		g_RV_Stream_Buf.buffer[i].buffer_size = RV_BUF_SIZE;
		g_RV_Stream_Buf.buffer[i].data_length= 0;
		g_RV_Stream_Buf.buffer[i].cursor_read = 0;
		g_RV_Stream_Buf.buffer[i].cursor_write = 0;
		g_RV_Stream_Buf.buffer[i].is_over = 0;
	}
	// fill stream buffer
	for(i=0;i<RV_BUF_NUM;i++)
	{
		len = pFilefunc->read_func((void*)pthis->fs_filehandle, (MMD_S08*)g_RV_Stream_Buf.buffer[i].buffer_address, RV_BUF_SIZE);
		g_RV_Stream_Buf.buffer[i].data_length= len ;
		g_RV_Stream_Buf.buffer[i].cursor_read = 0;
		g_RV_Stream_Buf.buffer[i].cursor_write = len;
		if(len==0)
			g_RV_Stream_Buf.buffer[i].is_over = 1;
	}
	g_RV_Stream_Buf.buffer_write = 0;
	g_RV_Stream_Buf.buffer_read = 0;
#endif

	media_sysstatus->plyfilelst[0] = pthis;
	media_sysstatus->ply_filecounter++;
	
	*pfilehandle = (VIM_MEDIA_PLYFILE)pthis;
	pthis->f_playstatus = MEDIA_FILE_STOP;

	// remember file handle and file funciton which will be used in dsr
	g_PlayFileHandle = pthis;
	g_pFilefunc = pFilefunc;
	
//	MMD_USER_Timer_Set(media_rv_time_dsr, RV_TIMER_INTERVAL);
	MMD_RV_TIMER_SET(RV_TIMER_DATA, media_rv_time_dsr, RV_TIMER_INTERVAL);
	MMD_RV_TIMER_SET(RV_TIMER_DISP, RVDemo_FlushTime_Isr, RV_FLUSH_TIMER_INTERVAL);
	MMD_RV_TIMER_SET(RV_TIMER_DispFresh, media_rv_refresh, RV_FRESH_TIMER_INTERVAL);
	return MMD_RET_NOERR;

}

/*******************************************************************************************
Description:
	media player file play
Parameters:
	MP_FILE* pthis: player file handle
Return:	
	MP_SUCCESS: start playing file ok
	Others:		  error code
NOTE: 
       this function call in main thread(start playing)		
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.04.02		create first version
********************************************************************************************/
extern TIMER_STATE	emMMDTimerState;
MP_U32 media_rv_file_play(VIM_MEDIA_PLYFILE pfilehandle, MP_U32 Callback)
{
	MP_U32  	result;

	VIM_MEDIA_PLAYER_FILE* pfile;
	
	pfile = (VIM_MEDIA_PLAYER_FILE*)pfilehandle;
	//check player file handle
	result = media_player_check_filehandle(pfile);
	if(result)			
		return result;
	if(g_RV_State == MEDIA_FILE_PLAYING)
		return MP_SUCCESS;
	g_RV_State = MEDIA_FILE_PLAYING;
	//check file status
	if(!((pfile->f_playstatus==MEDIA_FILE_STOP)
	    |(pfile->f_playstatus==MEDIA_FILE_FAST_PLAYING)))
	{
		ME_ERROR(("MP_ERR_FILE_STATUS_ERROR, pfile->f_playstatus=%d", pfile->f_playstatus));
		return MP_ERR_FILE_STATUS_ERROR;
	}

	if(Callback)
		g_plyCallback = (Func_Callback)Callback;


//	MMD_USER_Timer_Start();
	MMD_RV_TIMER_START(RV_TIMER_DATA);
	MMD_RV_TIMER_START(RV_TIMER_DISP);
	MMD_RV_TIMER_START(RV_TIMER_DispFresh);
//	emMMDTimerState = TIMER_STARTED;//add by ahy for chenge overlay update lcd mode
	result = RV_File_Play(g_FileHandle);
#if 1
	
#else
	MMD_ASSERT(result==0);
#endif
	pfile->f_playstatus = MEDIA_FILE_PLAYING;

	return result;//MP_SUCCESS;
}


/*******************************************************************************************
Description:
	media player file pause
Parameters:
	MP_FILE* pthis: player file handle
Return:	
	MP_SUCCESS: pause player file ok
	Others:		  error code
NOTE: 
       this function call in main thread(pause file playing)	
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.04.02		create first version
********************************************************************************************/
MP_U32 media_rv_file_pause(VIM_MEDIA_PLYFILE pfilehandle)
{
	MP_U32   result;
	VIM_MEDIA_PLAYER_FILE* pfile = (VIM_MEDIA_PLAYER_FILE*)pfilehandle;
	
	//check player file handle
	result = media_player_check_filehandle(pfile);
	if(result)			
		return result;
	if(g_RV_State == MEDIA_FILE_PAUSE)
		return MP_SUCCESS;
	g_RV_State = MEDIA_FILE_PAUSE;
	//check file status
	if(pfile->f_playstatus!=MEDIA_FILE_PLAYING)
	{
		ME_ERROR(("MP_ERR_FILE_STATUS_ERROR, pfile->f_playstatus=%d", pfile->f_playstatus));
		return MP_ERR_FILE_STATUS_ERROR;
	}

	result = RV_File_Pause(g_FileHandle);
#if 1
	
#else
	MMD_ASSERT(result==0);
#endif
//	MMD_USER_Timer_Stop();	
	MMD_RV_TIMER_STOP(RV_TIMER_DATA);
	MMD_RV_TIMER_STOP(RV_TIMER_DISP);
//	emMMDTimerState = NO_TIMER_START;//add by ahy for chenge overlay update lcd mode
	//set file pause status
	pfile->f_playstatus = MEDIA_FILE_PAUSE;
	return result;//MP_SUCCESS;
}

/*******************************************************************************************
Description:
	media player file resume
Parameters:
	MP_FILE* pthis: player file handle
Return:	
	MP_SUCCESS: resume player file ok
	Others:		  error code
NOTE: 
       this function call in main thread(resume file playing)	
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.04.02		create first version
********************************************************************************************/
MP_U32 media_rv_file_resume(VIM_MEDIA_PLYFILE pfilehandle)
{
	MP_U32     result;
	VIM_MEDIA_PLAYER_FILE* pfile= (VIM_MEDIA_PLAYER_FILE*)pfilehandle;
	
	//check player file handle
	result = media_player_check_filehandle(pfile);
	if(result)			
		return result;
	if(g_RV_State == MEDIA_FILE_PLAYING)
		return MP_SUCCESS;
	g_RV_State = MEDIA_FILE_PLAYING;
	//check file status
	if(pfile->f_playstatus != MEDIA_FILE_PAUSE)
	{
		ME_ERROR(("MP_ERR_FILE_STATUS_ERROR, pfile->f_playstatus=%d", pfile->f_playstatus));
		return MP_ERR_FILE_STATUS_ERROR;
	}	
	
	result = RV_File_Resume(g_FileHandle);
#if 1
#else
	MMD_ASSERT(result==0);
#endif
//	MMD_USER_Timer_Start();	
	MMD_RV_TIMER_START(RV_TIMER_DATA);
	MMD_RV_TIMER_START(RV_TIMER_DISP);
//	emMMDTimerState = TIMER_STARTED;//add by ahy for chenge overlay update lcd mode
	//set file pause status
	pfile->f_playstatus=MEDIA_FILE_PLAYING;
	return result;//MP_SUCCESS;
}

/*******************************************************************************************
Description:
	media player file stop
Parameters:
	MP_FILE* pthis: player file handle
Return:	
	MP_SUCCESS: stop player file ok
	Others:		  error code
NOTE: 
       this function call in main thread(stop file playing)
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.04.02		create first version
********************************************************************************************/
MP_U32 media_rv_file_stop(VIM_MEDIA_PLYFILE pfilehandle)
{
	MP_U32   result;

	VIM_MEDIA_PLAYER_FILE* pfile;
	pfile = (VIM_MEDIA_PLAYER_FILE*)pfilehandle;
	//check player file handle
	result = media_player_check_filehandle(pfile);
	if(result)			
		return result;
	if(g_RV_State == MEDIA_FILE_STOP)
		return MP_SUCCESS;
	g_RV_State = MEDIA_FILE_STOP;
	//check file status
	if (pfile->f_playstatus == MEDIA_FILE_STOP)
		return MP_SUCCESS; 

	pfile->f_playstatus=MEDIA_FILE_STOP;

	result = RV_File_Stop(g_FileHandle);
#if 1
#else
	MMD_ASSERT(result==0);
#endif
//	MMD_USER_Timer_Stop();
	MMD_RV_TIMER_STOP(RV_TIMER_DATA);
//	emMMDTimerState = NO_TIMER_START;//add by ahy for chenge overlay update lcd mode
	return result;//MP_SUCCESS;
}

/*******************************************************************************************
Description:
	media player file close and all buffer free
Parameters:
	MP_FILE* pthis: player file handle
Return:	
	MP_SUCCESS: close ok
	Others:			error code
NOTE: 
       this function call in file thread(close file)
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.04.02		create first version
********************************************************************************************/
MP_U32 media_rv_file_close(VIM_MEDIA_PLYFILE pfilehandle)
{
	MP_U32 result, i;
	VIM_MEDIA_PLAYER_FILE* pfile;
	pfile = (VIM_MEDIA_PLAYER_FILE*)pfilehandle;
	
	if(pfile==NULL)
		return MP_ERR_FILE_HANDLE_ERROR;
	
	if(pfile->f_playstatus!=MEDIA_FILE_STOP)
	{
		pfile->f_playstatus = MEDIA_FILE_STOP;
		result = RV_File_Stop(g_FileHandle);
		MMD_ASSERT(result==0);
//		MMD_USER_Timer_Stop();
		MMD_RV_TIMER_STOP(RV_TIMER_DATA);
	}

	result = RV_File_Close(g_FileHandle);
#if 1
	if(result != 0)
		goto L_media_rv_file_close;
#else
	MMD_ASSERT(result==0);
#endif
	result = RV_Fifo_Release(g_Fifo_Addr);
#if 1
	if(result != 0)
		goto L_media_rv_file_close;
#else
	MMD_ASSERT(result==0);
#endif
	// close source file 
	result = g_pFilefunc->close_func((void*)g_PlayFileHandle->fs_filehandle);
#if 1
#else
	MMD_ASSERT(result==0);
#endif
L_media_rv_file_close:
	mp_free(pfile);
	media_sysstatus->plyfilelst[0] = NULL;
	media_sysstatus->ply_filecounter--;

#ifdef RV_BASEBAND_MEM
#else
	// Release RV stream buffer memory
	for(i=0;i<RV_BUF_NUM;i++)
	{
		mp_free((void*)g_RV_Stream_Buf.buffer[i].buffer_address);
		g_RV_Stream_Buf.buffer[i].buffer_size = 0;
		g_RV_Stream_Buf.buffer[i].data_length= 0;
		g_RV_Stream_Buf.buffer[i].cursor_read = 0;
		g_RV_Stream_Buf.buffer[i].cursor_write = 0;
		g_RV_Stream_Buf.buffer[i].is_over = 0;
	}
	g_RV_Stream_Buf.buffer_write = 0;
	g_RV_Stream_Buf.buffer_read = 0;
#endif

	// Release OS dependent timer
//	MMD_USER_Timer_Release();
	MMD_RV_TIMER_RELEASE(RV_TIMER_DATA);
	RV_Mem_Poll_Delete();
	
	return result;//MP_SUCCESS;
}
/*******************************************************************************************
Description:
	media player file close and all buffer free
Parameters:
	MP_FILE* pthis: player file handle
	MP_U32 pos:            user want seek time pos(s)
	MP_U32*accesspos:  file access time pos(s)
Return:	
	MP_SUCCESS: seek ok
	Others:			error code
NOTE: 
       this function call in file thread(seek)
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.04.02		create first version
********************************************************************************************/
MP_U32 media_rv_file_seek(VIM_MEDIA_PLYFILE pfilehandle, MP_U32 pos, MP_U32*accesspos)
{
	MP_U32	result;//seekpos, asscessframe, 
	MP_FILE *pthis = NULL;
	VIM_MEDIA_PLAYER_FILE* pfile;
	//MP_U32 sample_rate=0;
	//MP_U32 dma_count=0;
	
	pfile = (VIM_MEDIA_PLAYER_FILE*)pfilehandle;
	result = media_player_check_filehandle(pfile);
	if(result)			
		return result;
	pthis = pfile->mp_filehandle;
#if 1
	if(pfile->f_playstatus == MEDIA_FILE_PAUSE)
		return MP_ERROR;//暂停时不支持SEEK
	result = RV_File_Pause(g_FileHandle);
	MMD_ASSERT(result==0);
//	MMD_USER_Timer_Stop();	
	MMD_RV_TIMER_STOP(RV_TIMER_DATA);
//	emMMDTimerState = NO_TIMER_START;//add by ahy for chenge overlay update lcd mode
	//set file pause status
	pfile->f_playstatus = MEDIA_FILE_PAUSE;

	result = RV_File_Seek(g_FileHandle, pos, accesspos);
	MMD_ASSERT(result==0);

	result = RV_File_Resume(g_FileHandle);
	MMD_ASSERT(result==0);

//	MMD_USER_Timer_Start();	
	MMD_RV_TIMER_START(RV_TIMER_DATA);
//	emMMDTimerState = TIMER_STARTED;//add by ahy for chenge overlay update lcd mode
	//set file pause status
	pfile->f_playstatus=MEDIA_FILE_PLAYING;
	
#else
	//before seek must stop decode and read data
	if(pfile->f_playstatus!=MEDIA_FILE_STOP)
	{
		pfile->f_playstatus = MEDIA_FILE_STOP;
		result = RV_File_Stop(g_FileHandle);
#if 1
#else
		MMD_ASSERT(result==0);
#endif
		MMD_USER_Timer_Stop();
	}

	result = RV_File_Seek(g_FileHandle, pos, accesspos);
#if 1
#else
	MMD_ASSERT(result==0);
#endif
#endif
	return MP_SUCCESS;
}


/*******************************************************************************************
Description:
	media player file get total time
Parameters:
	MP_FILE* pthis: player file handle
	MP_U32*accesspos:  file access time pos(s)
Return:	
	MP_SUCCESS: ok
	Others:			error code
NOTE: 
       this function maybe call in all thread
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			aihouyin				2008.10.29		create first version
********************************************************************************************/
MP_U32 media_rv_file_gettotaltime(VIM_MEDIA_PLYFILE pfilehandle, MP_U32*totaltime)
{
	MP_U32	result;//seekpos, asscessframe, 
	MP_FILE *pthis = NULL;
	VIM_MEDIA_PLAYER_FILE* pfile;

	
	pfile = (VIM_MEDIA_PLAYER_FILE*)pfilehandle;
	result = media_player_check_filehandle(pfile);
	if(result)			
		return result;
	if(g_RV_State == MEDIA_FILE_STOP)
	{
		*totaltime = -1;
		return MP_ERROR;
	}
	pthis = pfile->mp_filehandle;
	
	result = RV_File_GetTotalTime(g_FileHandle,totaltime);
#if 1
#else
	MMD_ASSERT(result==0);
#endif
	gTotalTime = *totaltime;
	return result;//MP_SUCCESS;
}

/*******************************************************************************************
Description:
	media player file get play time
Parameters:
	MP_FILE* pthis: player file handle
	MP_U32*accesspos:  file access time pos(s)
Return:	
	MP_SUCCESS: ok
	Others:			error code
NOTE: 
       this function maybe call in all thread
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			aihouyin				2008.10.29		create first version
********************************************************************************************/
MP_U32 media_rv_file_getplaytime(VIM_MEDIA_PLYFILE pfilehandle, MP_U32*playtime)
{
	MP_U32	result;//seekpos, asscessframe, 
	MP_FILE *pthis = NULL;
	VIM_MEDIA_PLAYER_FILE* pfile;

	
	pfile = (VIM_MEDIA_PLAYER_FILE*)pfilehandle;
	result = media_player_check_filehandle(pfile);
	if(result)			
		return result;
	if(g_RV_State == MEDIA_FILE_STOP)
	{
		*playtime = -1;
		return MP_ERROR;
	}
	pthis = pfile->mp_filehandle;
	
	result = RV_File_GetPlayTime(g_FileHandle,playtime);
#if 1
#else
	MMD_ASSERT(result==0);
#endif
	return result;//MP_SUCCESS;
}

/*******************************************************************************************
Description:
	get media player file property(include audio infor, video ifnor and other ID3 infor)
Parameters:
	MP_FILE* pthis: 			player file handle
	VIM_MEDIA_FILE_PROPERTY*pProperty:  property structor point
Return:	
	MP_SUCCESS: ok
	Others:			error code
NOTE: 
       this function maybe call in all thread
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			aihouyin				2008.10.29		create first version
********************************************************************************************/
MP_U32 media_rv_file_getproperty(VIM_MEDIA_PLYFILE pfilehandle,  VIM_MEDIA_FILE_PROPERTY *property)
{
	MP_U32	result;//seekpos, asscessframe, 
	MP_FILE *pthis = NULL;
	VIM_MEDIA_PLAYER_FILE* pfile;
	MP_U32	propertyaddr = NULL;

	pfile = (VIM_MEDIA_PLAYER_FILE*)pfilehandle;
	result = media_player_check_filehandle(pfile);
	if(result)			
		return result;
	pthis = pfile->mp_filehandle;
	
	result = RV_File_GetProperty(g_FileHandle,  &propertyaddr);
#if 1
#else
	MMD_ASSERT(result==0);
#endif
	Vm_RdSram(propertyaddr, (MMD_S08*)property, sizeof(VIM_MEDIA_FILE_PROPERTY));

	return result;//MP_SUCCESS;

}

#endif


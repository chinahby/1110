/*======================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	interrupt_video.c 
* User for : 	820
* Creat By : 	Amanda Deng()                                                                   
* CrateDate: 	2007/04/02
* Last modify : Amanda Deng 2007-xx-xx     
*
* version 0.1: 
*	have 1 interface API for user .
*	void video_decode_interrupt_init(void);
*	void video_syn_timer_start(void);
*	MMD_U32 get_system_time_ticks(void);
* release history:
* 		version 0.1 release by Amanda Deng (Deng Naili --Zhang Zhongliang AE)
=======================================================================*/

#include "mplayer_internal.h"					
#include "msg.h"
#include "vc0848.h"
#include "AEEStdlib.h"

/*******************************************************************************************
		Use timer 2 as system time, SW timer as video out timer and video input timer
********************************************************************************************/
//NU_TIMER media_player_timer;
//NU_TIMER media_player_jpgdis_timer;
//NU_TIMER media_recorder_jpgcap_timer;

MP_U32   flg_vdecode = 0;//video decoder flag
MP_U32   flg_vread = 0;//video read flag
MP_U32  flg_aread=0;//audio read flag
MP_U32  in_timerprocess=0;//audio read flag

extern Func_Callback g_plyCallback;
extern TIMER_STATE	emMMDTimerState;
extern UPDATE_STATE emMMDUpdateState;
extern BLAYER_PARAMS_FILL	strMMDFillBlParams;
extern MMD_U32 timerRun ;

MMD_U08 play_timer_flag=0;
MMD_U08 lcd_refresh_flag=0;

extern MMD_U32 MMD_LCD_Refresh_MP4(MMD_U32 layer,MMD_U32 sx,MMD_U32 sy,MMD_U32 width,MMD_U32 height,MMD_U08 *pdata,MMD_U32 updateFlag);
void media_player_time_dsr(void)
{
	MP_U32   ret;
//	MP_U32   current;
	video_stream_t*src=NULL;
	MP_U32   current_ticks, delta_ticks;
	MP_U32   current_duration;
	MP_U32	vstatus = 0;//video int flag
	MP_U32	vdatasta=0;	//video data read status
	VIM_MEDIA_PLAYER_FILE* pfile=NULL;
	MP_FILE* mpfile=NULL;
	MP_U32  stime,etime;
	MP_U32 dma_count;
	MP_U32 sample_rate;
	//MP_U32 flg_acount=0;//whether audio dma_count has changed.
	MP_U32 audiostatus=0, filestatus=0;
	MP_U32 r_cur,w_cur;
	MP_U32 flg_start2readcore=0;
	VID_BUF_DATA BufData_Vid;


    while(lcd_refresh_flag==1)
   {
      rex_sleep(1);
   }
   play_timer_flag=1;

	if(emMMDTimerState == NO_TIMER_START)	return;
   #if 0
	if(g_BIU_Count)
		return;
	timerRun =1;
	in_timerprocess=1;
	if(emMMDUpdateState == NEED_UPDATE && !g_BIU_Count)
	{
		//MMD_LCD_Refresh_In_Timer();
		emMMDUpdateState = UPDATED;
	}
#endif
   
   if(emMMDUpdateState == NEED_UPDATE )
    {
		if(VC_GetCurrentDevice() == VC_DEV_PLY_AMR)
		{
			MMD_LCD_Refresh_In_Timer_MP4(); //zhangxiang del for mp4 refresh lcd 09/03/03
            emMMDUpdateState=UPDATED;
		}
		else
		{
                 MMD_LCD_Refresh_In_Timer();
                 emMMDUpdateState = UPDATED;
		}
    }
	stime = MMD_SYS_GetTimer2Value();//get_system_time_ticks();
	mp_debug(("time_dsr---->"));

	//get player file handle(media player handle and me file handle)
	pfile = media_sysstatus->plyfilelst[0];
	mpfile = pfile->mp_filehandle;
	if(pfile->f_playstatus!=MEDIA_VIDEO_PLAYING)
		return;
	src = &(mpfile->video_data.src);

	MMD_BIU_Get_Device_Status(IDX_FILE_STATUS, &filestatus);
	//first process audio data
	if(mpfile->audio_infor.a_isvalid) {
		//mp_debug(("### timer###mpfile->audio_infor.a_isvalid"));
		if(pfile->a_status == MEDIA_AUDIO_PLAYING) {
			MMD_BIU_Get_Device_Status(IDX_AUD_STATUS,&audiostatus);
			if(pfile->fs_filestore==FILE_IN_CORE_FILE) {
				//process readed data
				if(filestatus >> 16) {
					pfile->f_playcontrol |= MP_AUDIO_READ_ERROR;
					//MSG_FATAL("ME_ERR_READ_FILE_ERROR in read audio data! error code = 0x%x", (filestatus >> 16),0,0);
				} else {
					w_cur = mpfile->audio_data.src.buffer_write;
					r_cur = mpfile->audio_data.src.buffer_read;
					//ERR("\t\t\t\tacur-w-%d-r-%d",w_cur,r_cur,0);
					while((!(audiostatus&(1<<r_cur))) && flg_aread) {
						mp_debug(("### audiostatus while loop1"));
						ret = mpfile->parser->read_audio_frames_post(mpfile->parserinfor,&(mpfile->audio_data.src.buffer[r_cur]));
						if(ret) {
							pfile->f_playcontrol |= MP_AUDIO_READ_ERROR;//MP_AUDIO_STREAM_END;
							mp_error(("read_audio_frames_post=0x%08x",ret));
							break;
						}
						//MSG_FATAL("\t\t\t\tabuf-%d-readed-0x%08x-offset-0x%08x", r_cur,mpfile->audio_data.src.buffer[r_cur].data_length,mpfile->audio_data.src.buffer[r_cur].cursor_read);
						ret = media_player_audio_fillsrcbuf_smtd(r_cur,mpfile->audio_data.src.buffer[r_cur].data_length, mpfile->audio_data.src.buffer[r_cur].cursor_read, mpfile->audio_data.src.buffer[r_cur].is_over);
						if(ret) {
							pfile->f_playcontrol |= MP_AUDIO_READ_ERROR;//MP_AUDIO_STREAM_END;
							mp_error(("audio data read error when media_player_audio_fillsrcbuf_smtd()=0x%08x, file status = 0x%08X ", ret, pfile->f_playcontrol ));
							break;
						} 
						r_cur++;
						if(r_cur>=mpfile->audio_data.src.buffer_count)
							r_cur = 0;
						flg_aread--;
					}	
					mpfile->audio_data.src.buffer_read = r_cur;
					//prepare to send cmd to read data
					if(!(filestatus & 0x1)){//read2core operate over
						mp_debug(("### read2core operate over"));
						while((audiostatus & (1<<w_cur)) && (flg_aread< mpfile->audio_data.src.buffer_count) && (!(pfile->f_playcontrol & MP_AUDIO_STREAM_END))) {
							mpfile->audio_data.src.buffer[w_cur].isnot_empty	= 0;
							mpfile->audio_data.src.buffer[w_cur].data_length 	= 0;
							mpfile->audio_data.src.buffer[w_cur].cursor_read  = 0;
							mpfile->audio_data.src.buffer[w_cur].cursor_write = 0;
							ret = media_player_audio_read_frames(pfile,w_cur);
							if(ret>ME_FILE_END_OF_AUDIO) {
								pfile->f_playcontrol |= MP_AUDIO_READ_ERROR;
								mp_error(("media_player_audio_read_frames = 0x%08x",ret));
								break;
							} 
							if(mpfile->audio_data.src.buffer[w_cur].data_length==0) {
								mp_error(("audio data length is zero!"));
							}
							mp_debug(("\t\t\t\tabuf-%d-reading...", w_cur));
							flg_aread++;
	 						w_cur++;
							if(w_cur>=mpfile->audio_data.src.buffer_count)
								w_cur = 0;
							flg_start2readcore++;
						}
						mpfile->audio_data.src.buffer_write = w_cur; 
					}
				}
			}else {
				w_cur = mpfile->audio_data.src.buffer_write;
				flg_aread = 0;
				while((audiostatus & (1<<w_cur)) && (flg_aread< mpfile->audio_data.src.buffer_count) && (!(pfile->f_playcontrol & MP_AUDIO_STREAM_END))) {
					mpfile->audio_data.src.buffer[w_cur].isnot_empty	= 0;
					mpfile->audio_data.src.buffer[w_cur].data_length 	= 0;
					mpfile->audio_data.src.buffer[w_cur].cursor_read  = 0;
					mpfile->audio_data.src.buffer[w_cur].cursor_write = 0;
					ret = media_player_audio_read_frames(pfile,w_cur);
					if(ret>ME_FILE_END_OF_AUDIO) {
						pfile->f_playcontrol |= MP_AUDIO_READ_ERROR;
						mp_error(("media_player_audio_read_frames = 0x%08x",ret));
						break;
					} 
					if(mpfile->audio_data.src.buffer[w_cur].data_length==0) {
						mp_error(("audio data length is zero!"));
					}
					mp_debug(("\t\t\t\tabuf-%d-reading...", w_cur));
					ret = media_player_audio_fillsrcbuf_smtd(w_cur,mpfile->audio_data.src.buffer[w_cur].data_length, mpfile->audio_data.src.buffer[w_cur].cursor_read, mpfile->audio_data.src.buffer[w_cur].is_over);
					if(ret) {
						pfile->f_playcontrol |= MP_AUDIO_READ_ERROR;//MP_AUDIO_STREAM_END;
						mp_error(("audio data read error when media_player_audio_fillsrcbuf_smtd()=0x%08x, file status = 0x%08X ", ret, pfile->f_playcontrol ));
						break;
					} 
					w_cur++;
					if(w_cur>=mpfile->audio_data.src.buffer_count)
						w_cur = 0;
					flg_aread++;
				}
				mpfile->audio_data.src.buffer_write = w_cur; 
			}

			//get audio play status and update playtime
			MMD_BIU_Get_Device_Status(IDX_AUD_DMA_COUNT, &dma_count);
			if(dma_count!=pfile->a_dmasamples) {
				//modified by yjz
				sample_rate	= pfile->mp_filehandle->audio_outinfor.a_outsmplrate;
				pfile->a_time = (MP_U64)(dma_count)*250/sample_rate;		//ms			
				pfile->f_playtime	= pfile->a_time;	
				mp_debug(("\ta_time=%d,f_playtime=%d\n",pfile->a_time,pfile->f_playtime));
				pfile->f_basetime	= MMD_SYS_GetTimer2Value();//us//get_system_time_ticks();       	//ms
				pfile->a_dmasamples = dma_count;
			}
		}
		mp_debug(("###End of mpfile->audio_infor.a_isvalid"));
	}

//then process video
	if(mpfile->video_infor.v_isvalid) {	
		mp_debug(("###mpfile->video_infor.v_isvalid"));
		if(!(pfile->f_playcontrol&MP_VIDEO_DECODE_END)) {						
			if(pfile->fs_filestore==FILE_IN_CORE_FILE) {
				MMD_BIU_Get_Device_Status(IDX_VID_STATUS,&vdatasta);	
				if(filestatus >> 16) {
					pfile->f_playcontrol |= MP_VIDEO_READ_ERROR;
					mp_error(("read video data file error! error code = 0x%x", (filestatus >> 16)));
				}else {
					w_cur = mpfile->video_data.src.buffer_write;
					r_cur = mpfile->video_data.src.buffer_read;
					while((!(vdatasta&(1<<r_cur))) && flg_vread) {
						ret = mpfile->parser->read_video_frames_post(mpfile->parserinfor,&(src->buffer[r_cur]));
						if(ret) {
							//print error and stop timer
							pfile->f_playcontrol |= MP_VIDEO_READ_ERROR;
							mp_error(("read_video_frames_post=0x%08x",ret));
							break;
						} else { 
							src->buffer[r_cur].isnot_empty = src->buffer[r_cur].data_length;
						}
						BufData_Vid.idx = r_cur;
						BufData_Vid.cursor_read = src->buffer[r_cur].cursor_read;
						BufData_Vid.datalength = src->buffer[r_cur].data_length;
						BufData_Vid.lastblock = src->buffer[r_cur].is_over;
						mp_debug(("\t\t\t\tvbuf-%d-readed-0x%08x-offset-0x%08x", r_cur,src->buffer[r_cur].data_length,src->buffer[r_cur].cursor_read));
						ret = media_player_video_fillsrcbuf_smtd(&BufData_Vid);
						if(ret) {
							pfile->f_playcontrol |= MP_VIDEO_READ_ERROR;
							mp_error(("video data read error when media_player_video_fillsrcbuf_smtd()=0x%08x, file status = 0x%08X ", ret, pfile->f_playcontrol ));
							break;
						} 
						mp_debug(("[r]\tfill vbuf-%d",r_cur));
						r_cur++;
						if(r_cur>=mpfile->video_data.src.buffer_count)
							r_cur = 0;
						flg_vread--;
					} 
					mpfile->video_data.src.buffer_read = r_cur;
					//prepare to send cmd to read data
					
					if(!(filestatus & 0x1)){//read2core operate over
						mp_debug(("### read2core operate over"));
						while((vdatasta & (1<<w_cur)) && (flg_vread< mpfile->video_data.src.buffer_count) && (!(pfile->f_playcontrol & MP_VIDEO_STREAM_END))) {
							mpfile->video_data.src.buffer[w_cur].isnot_empty	= 0;
							mpfile->video_data.src.buffer[w_cur].data_length 	= 0;
							mpfile->video_data.src.buffer[w_cur].cursor_read  = 0;
							mpfile->video_data.src.buffer[w_cur].cursor_write = 0;
							ret = media_player_video_read_frames(pfile, w_cur);
							if(ret>ME_FILE_END_OF_VIDEO) {
								//print error and stop timer
								pfile->f_playcontrol |= MP_VIDEO_READ_ERROR;
								mp_error(("media_player_video_read_frames=0x%08x",ret));
								break;
							} 
							mp_debug(("[r]\tsr vbuf-%d...", w_cur));
							flg_vread++;
	 						w_cur++;
							if(w_cur>=mpfile->video_data.src.buffer_count)
								w_cur = 0;
							flg_start2readcore++;
						}
						mpfile->video_data.src.buffer_write = w_cur; 
					}
				}
			}else {
				MMD_BIU_Get_Device_Status(IDX_VID_STATUS,&vdatasta);	
				w_cur = mpfile->video_data.src.buffer_write;
				flg_vread = 0;
				while((vdatasta & (1<<w_cur)) && (flg_vread< mpfile->video_data.src.buffer_count) && (!(pfile->f_playcontrol & MP_VIDEO_STREAM_END))) {
					mpfile->video_data.src.buffer[w_cur].isnot_empty	= 0;
					mpfile->video_data.src.buffer[w_cur].data_length 	= 0;
					mpfile->video_data.src.buffer[w_cur].cursor_read  = 0;
					mpfile->video_data.src.buffer[w_cur].cursor_write = 0;
					ret = media_player_video_read_frames(pfile, w_cur);
					if(ret>ME_FILE_END_OF_VIDEO) {
						//print error and stop timer
						pfile->f_playcontrol |= MP_VIDEO_READ_ERROR;
						mp_error(("media_player_video_read_frames=0x%08x",ret));
						break;
					} 
					mp_debug(("[r]\tsr vbuf-%d...", w_cur));
					BufData_Vid.idx = w_cur;
					BufData_Vid.cursor_read = mpfile->video_data.src.buffer[w_cur].cursor_read;
					BufData_Vid.datalength = mpfile->video_data.src.buffer[w_cur].data_length;
					BufData_Vid.lastblock = mpfile->video_data.src.buffer[w_cur].is_over;
					mp_debug(("\t\t\t\tvbuf-%d-readed-0x%08x-offset-0x%08x", w_cur,mpfile->video_data.src.buffer[w_cur].data_length,mpfile->video_data.src.buffer[w_cur].cursor_read));
					ret = media_player_video_fillsrcbuf_smtd(&BufData_Vid);
					if(ret) {
						pfile->f_playcontrol |= MP_VIDEO_READ_ERROR;
						mp_error(("video data read error when media_player_video_fillsrcbuf_smtd()=0x%08x, file status = 0x%08X ", ret, pfile->f_playcontrol ));
						break;
					} 
					mp_debug(("[r]\tfill vbuf-%d",w_cur));
					flg_vread++;
					w_cur++;
					if(w_cur>=mpfile->video_data.src.buffer_count)
						w_cur = 0;
				}
				mpfile->video_data.src.buffer_write = w_cur; 
			}
			
			//get decode status
			mp_debug(("\tframesdone-%d",pfile->v_framesdone));
			if(pfile->v_framesdone) {
				if(mpfile->video_infor.v_isvalid==1) {
					//mp4/3pg/avi
					vstatus = VIM_HAPI_GetMPEGDecIntStatus();
					if(vstatus)
					{
						VIM_HAPI_SetMPEGDecIntStatus(0);					
        				if(vstatus & VIM_MPEG4_VOP_END){
        					mp4v_decinfor.vop_start_flag = TRUE;
        					pfile->v_frames ++;
        					pfile->v_samples++;
        					current_duration = src->duration_read;
        					pfile->v_duration = src->duration[current_duration];
    						pfile->v_time += pfile->v_duration;	
    						src->duration_read++;
    						if(src->duration_read>=MAX_VIDEO_FRAMECNT_IN_BUFFER*4)
    						{
    							src->duration_read = 0;
    						}
    						pfile->v_lastdone = 1;
        					flg_vdecode++;
        					pfile->v_framesdone = 0;
        				}
        				// decoder done interrupt process
        				if(vstatus&VIM_MPEG4_MP4_FINISH)
        				{
        					pfile->v_frames ++;
        					pfile->v_samples++;
        					current_duration = src->duration_read;
        					pfile->v_duration = src->duration[current_duration];
        					pfile->v_time += pfile->v_duration;	
        					
        					pfile->f_playcontrol |= MP_VIDEO_DECODE_END;
        					pfile->f_playtime = pfile->v_time;
        					pfile->v_framesdone = 0;
        					mp_error(("MP_VIDEO_DECODE_END"));
        				}
        				// decoder error interrupt process
        				if(vstatus&VIM_MPEG4_STUCK_ERROR)
        				{
        					pfile->f_playcontrol |= MP_VIDEO_DECODE_ERROR;
        					mp_error(("MP_VIDEO_DECODE_ERROR = 0x%08x",vstatus));
        					pfile->v_framesdone = 0;
        				}
					}
				}else {
					//other sw decoder, such as rm
					mp_error(("need SW decoder!"));
				}
			}
			if(flg_vdecode) {
				//delay for current frame duration
				current_ticks 	= MMD_SYS_GetTimer2Value();//us			
				if(current_ticks<pfile->f_basetime)
				{
						delta_ticks = 0xffffffff -pfile->f_basetime;
						delta_ticks += current_ticks;
				}
				else
					{delta_ticks = current_ticks-pfile->f_basetime;}
				//each mode playtime caculate
				if(!pfile->f_playmode)			
				{//only I VOP frames playing	
					if(delta_ticks>30000)			//delta time >=30ms==3*10ms
					{
							pfile->f_playtime = pfile->v_time;
							pfile->f_basetime = current_ticks;	
					}
				}	
				else										
				{//speed video playing	
						delta_ticks = delta_ticks*pfile->f_playmode;
						pfile->f_basetime = current_ticks;		
						pfile->f_playtime += delta_ticks/1000;//us//delta_ticks*10;
				}	

				//if have arrival next frame video output time then return
				if(pfile->f_playtime>=pfile->v_time) {
					mp_debug(("[d]\t\tst vop...%d",pfile->v_frames+1));
					//start video decode vop
					flg_vdecode = 0;
					pfile->v_lastdone =0;
					pfile->v_framesdone = 1;
					if(mpfile->video_infor.v_isvalid==1)
					{	//hardware decode video(3gp, mp4, jpg) 
						mpfile->video_decoder->decode_frames((void*)mpfile->video_decinfor,
														     	     (void*)&src->buffer[src->buffer_read],
														            NULL);
					}
					else
					{	//software decode video(jpg,rmvb, jif, bmp, png)
#if 0	//by dnn 2008/4/11				
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
				}
			}
		}
	}	

	if(pfile->fs_filestore==FILE_IN_CORE_FILE) {
		if(flg_start2readcore) {
			mp_debug(("###flg_start2readcore"));
			ret = MMD_FILE_StartRead2Core();
			if(ret) {
				//print error and stop timer
				pfile->f_playcontrol |= MP_AUDIO_READ_ERROR;
				pfile->f_playcontrol |= MP_VIDEO_READ_ERROR;
				mp_error(("MMD_FILE_StartRead2Core=0x%08x",ret));
			}
		}
	}

	if(pfile->fs_filestore==FILE_IN_CORE_FILE)
		MMD_BIU_Get_Device_Status(IDX_AUD_STATUS, &audiostatus);
	else		
		audiostatus = 0x0f;
		
	//decoder end or error, stop timer
	//add by yjz 2009/2/5	
	/*if(audiostatus & AUD_STA_NO_SRC_DATA || audiostatus & AUD_STA_NO_PCM_DATA)
	{
		printf("\naudio status = AUD_STA_NO_SRC_DATA\n");
		pfile->f_playcontrol |= MP_AUDIO_STREAM_END;
	}
	else if((audiostatus & 0x0f) == 0x0f)
	{
		//printf("\n(audiostatus & 0x0f) == 0x0f\n");		
	}
	else if(audiostatus & AUD_STA_DEC_ERR)
	{
		printf("\naudiostatus & AUD_STA_DEC_ERR\n");
		pfile->f_playcontrol |= MP_AUDIO_READ_ERROR;
	}*/
	//---------------
	if((pfile->f_playcontrol & MP_AUDIO_READ_ERROR)==MP_AUDIO_READ_ERROR
		||(pfile->f_playcontrol & MP_VIDEO_READ_ERROR)==MP_VIDEO_READ_ERROR
		||(pfile->f_playcontrol & MP_VIDEO_DECODE_ERROR)==MP_VIDEO_DECODE_ERROR) {
		//stop timer
		//mp_error(("\tmedia file play error!f_playtime=%d,a_time=%d,v_time=%d,v_frames=%d",pfile->f_playtime,pfile->a_time,pfile->v_time,pfile->v_frames));
		media_player_file_stop((MP_U32)pfile);
		if(g_plyCallback) {
			(*g_plyCallback)(MMD_PLY_FILE_ERR);
		}
	} 
	else if((pfile->f_playcontrol & MP_VIDEO_DECODE_END)
		&& (pfile->f_playcontrol & MP_AUDIO_STREAM_END) && (((audiostatus & 0x0f) == 0x0f) || (audiostatus & AUD_STA_NO_SRC_DATA))) {
		//stop timer
		//printf("\tmedia file play finish!f_playtime=%d,a_time=%d,v_time=%d,v_frames=%d f_playcontrol=0x%x audiostatus(0x%x)\n",pfile->f_playtime,pfile->a_time,pfile->v_time,pfile->v_frames,pfile->f_playcontrol,audiostatus);
      FARF(MSG,("\tmedia file play finish!f_playtime=%d,a_time=%d,v_time=%d,v_frames=%d f_playcontrol=0x%x audiostatus(0x%x)\n",pfile->f_playtime,pfile->a_time,pfile->v_time,pfile->v_frames,pfile->f_playcontrol,audiostatus));
      media_player_file_stop((MP_U32)pfile);		
		if(g_plyCallback) {			
			(*g_plyCallback)(MMD_PLY_FILE_END);
		}
	}

	etime = MMD_SYS_GetTimer2Value();//get_system_time_ticks();
	if(etime<stime) {
		etime = etime+0xffffffff -stime;
	} else {
		etime = etime-stime;
	}
	mp_debug(("-->end time_dsr! time=%dus",etime));
	timerRun = 0;
	in_timerprocess=0;
   play_timer_flag=0;
}



void media_player_timer_creat(void)
{
	MMD_USER_Timer_Set(media_player_time_dsr, 20);  
}

void media_player_timer_release(void)
{
	//MMD_USER_Timer_Release();
}

void media_player_timer_start(void)
{
	MMD_USER_Timer_Start();
	emMMDTimerState = TIMER_STARTED;
}

void media_player_timer_stop(void)
{
	MMD_USER_Timer_Stop();
    play_timer_flag=0; //zhangxiang  add 09/01/08 
	emMMDTimerState = NO_TIMER_START;
}
void media_player_jpgdis_time_dsr(void)
{
	MP_U32	 ret;
	MP_U32	 current,next;
	video_stream_t*src=NULL;
	MP_U32	destatus = 0;//video int flag
	MP_U32	 flg_vread = 0;//video read flag
   		
	VIM_MEDIA_PLAYER_FILE* pfile=NULL;
 	MP_FILE* mpfile=NULL;
   	
	//get player file handle(media player handle and me file handle)
 	pfile = media_sysstatus->plyfilelst[0];
 	mpfile = pfile->mp_filehandle;
 	if(pfile->f_playstatus!=MEDIA_VIDEO_PLAYING)
 		return;
 	src = &(mpfile->video_data.src);	

	if(pfile->f_jpgmode==0)
	{
		destatus=VIM_HAPI_GetJpegDecStatus();
		if(destatus&VIM_JPEG_DECODE_DONE)
        	{
        		pfile->f_playcontrol |= MP_VIDEO_DECODE_END;
        	}
        	else
        	{
        		current = src->buffer_read;        				
        		if(!src->buffer[current].is_over)
        		{
        			if(src->buffer[current].cursor_read >= src->buffer[current].data_length)
        			{
        				src->buffer[current].isnot_empty	= 0;
        				src->buffer[current].data_length 	= 0;
        				src->buffer[current].cursor_read  = 0;
        				src->buffer[current].cursor_write = 0;
        				src->buffer_write = current;
        				flg_vread ++;//by dnn 2008/4/11						
        					
        				next = current+1;
        				if(next>=src->buffer_count)
        					next = 0;
						
        				if(!(src->buffer[next].isnot_empty))	 
        				{
        					ret =media_player_video_read_frames(pfile, next);
        					if(ret>ME_FILE_END_OF_VIDEO) 
        					{
        						//print error and stop timer
        						FARF(MSG,("media_player_video_read_frames=0x%08x",ret));
        						//mp_error(("media_player_video_read_frames=0x%08x",ret));
        						media_player_file_stop((MP_U32)pfile);
                        if(g_plyCallback) {
			            (*g_plyCallback)(MMD_PLY_FILE_END);
		                  }
        					}        					
        				}						
					src->buffer_read= next;
        				if(src->buffer[src->buffer_read].is_over)
        				{
        					flg_vread --;
        				}
        			}
        		}
        		mpfile->video_decoder->decode_frames((void*)mpfile->video_decinfor,
        											(void*)&src->buffer[src->buffer_read], NULL);
        		if(flg_vread)
        		{
        			ret = media_player_video_read_frames(pfile, src->buffer_write);
        			if(ret>ME_FILE_END_OF_VIDEO) 
        			{
        				//print error and stop timer
        				FARF(MSG,("media_player_video_read_frames1=0x%08x",ret));
        				//mp_error(("media_player_video_read_frames=0x%08x",ret));
        				media_player_file_stop((MP_U32)pfile);
                  if(g_plyCallback) {
			            (*g_plyCallback)(MMD_PLY_FILE_END);
		                  }
        			}
        		}
        	}
	}	
	else
	{
		current = src->buffer_read;        				
    		if(!src->buffer[current].is_over)
    		{
    			if(src->buffer[current].cursor_read >= src->buffer[current].data_length)
    			{
    				src->buffer[current].isnot_empty	= 0;
    				src->buffer[current].data_length 	= 0;
    				src->buffer[current].cursor_read  = 0;
    				src->buffer[current].cursor_write = 0;
    				src->buffer_write = current;
    				flg_vread ++;//by dnn 2008/4/11						
    					
    				next = current+1;
    				if(next>=src->buffer_count)
    					next = 0;
				
    				if(!(src->buffer[next].isnot_empty))	 
    				{
    					ret =media_player_video_read_frames(pfile, next);
    					if(ret>ME_FILE_END_OF_VIDEO) 
    					{
    						//print error and stop timer
    						//mp_error(("media_player_video_read_frames=0x%08x",ret));
                     FARF(MSG,("media_player_video_read_frames2=0x%08x",ret));
    						media_player_file_stop((MP_U32)pfile);
                     if(g_plyCallback) {
			            (*g_plyCallback)(MMD_PLY_FILE_END);
		                  }
    					}
    				}
				src->buffer_read= next;
    				if(src->buffer[src->buffer_read].is_over)
    				{
    					flg_vread --;
    				}
    			}
    		}
    		mpfile->video_decoder->decode_frames((void*)mpfile->video_decinfor,
    												(void*)&src->buffer[src->buffer_read], NULL);
    		if(flg_vread)
    		{
    			ret = media_player_video_read_frames(pfile, src->buffer_write);
    			if(ret>ME_FILE_END_OF_VIDEO) 
    			{
    				//print error and stop timer
    				//mp_error(("media_player_video_read_frames=0x%08x",ret));
               FARF(MSG,("media_player_video_read_frames3=0x%08x",ret));
    				media_player_file_stop((MP_U32)pfile);
               if(g_plyCallback) {
			            (*g_plyCallback)(MMD_PLY_FILE_END);
		                  }
    			}
    		}
			
		destatus=VIM_HAPI_GetLbufReadStatus();
		if(destatus)
		{
			VIM_HAPI_SetLbufReadStatus(0);
			destatus=VIM_HAPI_LbufRead();
			if(destatus)
			{
				if(destatus==VIM_ERROR_DECODE)
				{
					pfile->f_playcontrol |= MP_VIDEO_DECODE_ERROR;
				}
				else
				{
					pfile->f_playcontrol |= MP_VIDEO_DECODE_END;
				}
			}				
		}
		
	}
	
	if((pfile->f_playcontrol & MP_VIDEO_DECODE_ERROR)==MP_VIDEO_DECODE_ERROR) 
	{
      FARF(MSG,("media_player_video_read_frames4"));
		media_player_file_stop((MP_U32)pfile);
		if(g_plyCallback) {
			(*g_plyCallback)(MMD_PLY_FILE_END);
		}
	} 
	else if((pfile->f_playcontrol & MP_VIDEO_DECODE_END)&&(pfile->f_playcontrol & MP_VIDEO_STREAM_END)) 
	{
		//stop timer
		//mp_debug(("\tmedia file play finish!f_playtime=%d,a_time=%d,v_time=%d,v_frames=%d",pfile->f_playtime,pfile->a_time,pfile->v_time,pfile->v_frames));
      FARF(MSG,("media_player_video_read_frames5"));
      media_player_file_stop((MP_U32)pfile);
		if(g_plyCallback) {
			(*g_plyCallback)(MMD_PLY_FILE_END);
		}
	}
	mp_debug(("media_player_jpgdis_time_dsr()"));

}

void media_player_jpgdis_timer_creat(void)
{
#if 0	
	STATUS	status;
      //video syn timer create (SW timer)
 		
	status = NU_Create_Timer(&media_player_jpgdis_timer, 	//NU_TIMER *timer
 						      "media player timer", 		//CHAR *name
 						      	media_player_jpgdis_time_dsr,	//VOID (*expiration_routine)(UNSIGNED), 
							3, 					//UNSIGNED id  = 2
							1, 					//UNSIGNED initial_time = 10ms 
							1, 					//UNSIGNED reschedule_time = 10ms intervale
							NU_DISABLE_TIMER);	//OPTION enable
	if (status != NU_SUCCESS)
	{
		mp_print(("media_player_jpgdis_timer_creat error = %d", (int)status));
	}

#else
	MMD_USER_Timer_Set(media_player_jpgdis_time_dsr, 10);
#endif
}

void media_player_jpgdis_timer_release(void)
{
#if 0
	STATUS	status;
      	//video syn timer delete (SW timer)
 	status = NU_Delete_Timer(&media_player_jpgdis_timer);
	if (status != NU_SUCCESS)
	{
		mp_print(("media_player_jpgdis_timer_release error = %d", (int)status));
	}
#else
	//MMD_USER_Timer_Release();
#endif
}

void media_player_jpgdis_timer_start(void)
{
#if 0
//	g_syntimecnt = 0;
	NU_Control_Timer(&media_player_jpgdis_timer, NU_ENABLE_TIMER);
	mp_print(("[media_player_jpgdis_timer_start]"));
#else
	MMD_USER_Timer_Start();
#endif 
}

void media_player_jpgdis_timer_stop(void)
{
#if 0
	NU_Control_Timer(&media_player_jpgdis_timer, NU_DISABLE_TIMER);
	mp_print(("[media_player_jpgdis_timer_stop]"));
#else
	MMD_USER_Timer_Stop();
#endif
}
/*
MR_U32 iStartWrite=0;
MR_U32 pSramStartWrite=0;
void media_recorder_jpgcap_time_dsr(UNSIGNED id)
{
	VIM_MEDIA_RECORDER_FILE* rfile=NULL;
	audio_stream_infor	  buffer;
	
	//get player file handle(media player handle and me file handle)
	rfile = media_sysstatus->rcfilelst[0];

	if(rfile->f_capturedone==0)
	{
		rfile->f_capturedone=1;
	}
	if(pSramStartWrite)
	{
		Vm_RdSram(pSramStartWrite, (MMD_S08*)&iStartWrite, sizeof(MR_U32));
		if(iStartWrite)
		{
			//mp_printf("iStartWrite(0x%x)\n",iStartWrite);
			Vm_RdSram(iStartWrite,(MMD_S08 *)&buffer,sizeof(audio_stream_infor));
			buffer.data_length = 1600;
			//mp_printf("\n MMD_REC_MUXER:buffer_address(0x%x) buffer_size(%d) frame_size(%d) data_length(%d)\n",buffer.buffer_address,buffer.buffer_size,buffer.frame_size,buffer.data_length);
			rfile->mr_filehandle->muxer->write_audio_frames(rfile->mr_filehandle->muxerinfor,&buffer);
			iStartWrite = 0;
			Vm_WrSram(pSramStartWrite, (MMD_S08*)&iStartWrite, sizeof(MR_U32));
		}
	}
}

void media_recorder_jpgcap_timer_creat(void)
{
	STATUS	status;
      //video syn timer create (SW timer)
 		
	status = NU_Create_Timer(&media_recorder_jpgcap_timer, 	//NU_TIMER *timer
 						      "media recorder timer", 		//CHAR *name
 						      	media_recorder_jpgcap_time_dsr,	//VOID (*expiration_routine)(UNSIGNED), 
							3, 					//UNSIGNED id  = 2
							100, 					//UNSIGNED initial_time = 10ms 
							1, 					//UNSIGNED reschedule_time = 10ms intervale
							NU_DISABLE_TIMER);	//OPTION enable
	if (status != NU_SUCCESS)
	{
		mp_print(("media_player_jpgdis_timer_creat error = %d", (int)status));
	}
}

void media_recorder_jpgcap_timer_release(void)
{
	STATUS	status;
      	//video syn timer delete (SW timer)
 	status = NU_Delete_Timer(&media_recorder_jpgcap_timer);
	if (status != NU_SUCCESS)
	{
		mp_print(("media_recorder_jpgcap_timer_release error = %d", (int)status));
	}
}

void media_recorder_jpgcap_timer_start(void)
{
//	g_syntimecnt = 0;
	NU_Control_Timer(&media_recorder_jpgcap_timer, NU_ENABLE_TIMER);
	mp_print(("[media_recorder_jpgcap_timer_start]"));
}

void media_recorder_jpgcap_timer_stop(void)
{
	NU_Control_Timer(&media_recorder_jpgcap_timer, NU_DISABLE_TIMER);
	mp_print(("[media_recorder_jpgcap_timer_stop]"));
}
*/



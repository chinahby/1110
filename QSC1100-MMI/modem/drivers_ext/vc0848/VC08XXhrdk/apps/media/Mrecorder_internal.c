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

#include "Mrecorder_internal.h"

#include "msg.h"

#define SAMR_ENCODE_FRAME_ONCE 5
#define VIDEO_ENCODE_FRAME_ONCE 16


MR_U32 iFristWrite=0;
volatile MR_U32 utime=0;
//MR_U32 timecount=0;
extern Func_RecCallback g_recCallback;
extern ME_U32 g_bufferindex_video;
extern ME_U32 g_bufferindex_audio;
extern MMD_U32 recorder_stop;
extern ME_U32 endduration;
extern MMD_U32 Frame_Info[2];
extern UPDATE_STATE emMMDUpdateState;
extern TIMER_STATE	emMMDTimerState;
extern void MMD_LCD_Refresh_In_Timer_Video(void);

void media_recorder_jpgcap_time_dsr(void)
{
	VIM_MEDIA_RECORDER_FILE* rfile=NULL;
	//audio_stream_infor	  buffer;
	MR_U32 bit,index,writeflag,buffsize,RegBit;
	//get player file handle(media player handle and me file handle)
	rfile = media_sysstatus->rcfilelst[0];
	if(emMMDUpdateState == NEED_UPDATE && !g_BIU_Count)
	{
		MMD_LCD_Refresh_In_Timer();
		emMMDUpdateState = UPDATED;
		
	}
	
#if 1
	if(rfile->mr_filehandle->audio_infor.a_isvalid) 
	{
		MMD_BIU_Get_Device_Status(IDX_AUD_STATUS, &bit);
		index = 2&bit;
		buffsize = bit & 4;
		bit = bit & 1;
		if(bit)
		{
			MMD_BIU_Get_Device_Status(IDX_FILE_STATUS, &writeflag);
			if (writeflag & 0x1)
			{
				//mp_printf("Last Write Is Not Flished!writeflag(0x%x)\n",writeflag);
				return;
			}
			
			index = index>>1;
			
			//mp_printf("Get Bit (0x%x) Bufferindex(0x%x)\n",bit,index);
			
			//printf("Get Bit (0x%x) buffer_address(0x%x) buffsize(%d)\n",bit,rfile->mr_filehandle->audio_data.dst.buffer[index].buffer_address,buffsize);
			if((rfile->mr_filehandle->audio_infor.a_codec[0]=='s')&&(rfile->mr_filehandle->audio_infor.a_codec[1]=='a')
				&&(rfile->mr_filehandle->audio_infor.a_codec[2]=='m')&&(rfile->mr_filehandle->audio_infor.a_codec[3]=='r') ||(rfile->mr_filehandle->audio_infor.a_codec[0]=='a')
				&&(rfile->mr_filehandle->audio_infor.a_codec[1]=='m')&&(rfile->mr_filehandle->audio_infor.a_codec[2]=='r'))
			{
				if(buffsize)
					rfile->mr_filehandle->audio_data.dst.buffer[index].data_length = 1600;
				else
					rfile->mr_filehandle->audio_data.dst.buffer[index].data_length = 800;
			}
			else if((rfile->mr_filehandle->audio_infor.a_codec[0]=='m')&&
					(rfile->mr_filehandle->audio_infor.a_codec[1]=='p')&&(rfile->mr_filehandle->audio_infor.a_codec[2]=='3'))//lily added mp3 support
			{
				MMD_BIU_Get_Device_Status(IDX_MP3_LENGTH, &(rfile->mr_filehandle->audio_data.dst.buffer[index].data_length));
			}
/*#ifndef SUPPORT_MP3  //lily modified
			
#else
			
#endif*/
			rfile->mr_filehandle->muxer->write_audio_frames(rfile->mr_filehandle->muxerinfor,&rfile->mr_filehandle->audio_data.dst.buffer[index]);

			//MMD_BIU_Get_Device_Status(IDX_AUD_STATUS, &RegBit);
			MMD_BIU_Get_Device_Status(IDX_AUD_STATUS, &RegBit);
			MMD_FILE_StartWriteFrCore();
			//mp_printf("1.MMD_FILE_StartWriteFrCore!\n");
			iFristWrite = 1;
			//bit = 0;
			RegBit&=(0xfffffffe);
			MMD_BIU_Set_Device_Status(IDX_AUD_STATUS, RegBit);
			
		}
	}

#else
	if(rfile->mr_filehandle->audio_infor.a_isvalid) 
	{
		MMD_BIU_Get_Device_Status(IDX_AUD_STATUS, &bit);
		index = 2&bit;
		buffsize = bit & 4;
		bit = bit & 1;
		if(bit)
		{
			MMD_BIU_Get_Device_Status(IDX_FILE_STATUS, &writeflag);
			if(writeflag & 0x1)
			{
				mp_printf("Last Write Is Not Flished!writeflag(0x%x)\n",writeflag);
				return;
			}
			else if(writeflag & 1<<29)
			{
				mp_printf("Last Time Have Not Start Write!\n");
				MMD_FILE_StartWriteFrCore();
				iFristWrite = 1;
				bit = 0;
				MMD_BIU_Set_Device_Status(IDX_AUD_STATUS, bit);
				return ;
			}
			index = index>>1;
			
			//mp_printf("Get Bit (0x%x) Bufferindex(0x%x)\n",bit,index);
			
			//mp_printf("Get Bit (0x%x) buffer_address(0x%x) buffsize(%d)\n",bit,rfile->mr_filehandle->audio_data.dst.buffer[index].buffer_address,buffsize);
			if(buffsize)
				rfile->mr_filehandle->audio_data.dst.buffer[index].data_length = 1600;
			else
				rfile->mr_filehandle->audio_data.dst.buffer[index].data_length = 800;
			rfile->mr_filehandle->muxer->write_audio_frames(rfile->mr_filehandle->muxerinfor,&rfile->mr_filehandle->audio_data.dst.buffer[index]);

			MMD_BIU_Get_Device_Status(IDX_VID_STATUS, &RegBit);
			/*ret = 200;
			while(!(RegBit & 1<<29) &&ret)
			{
				mp_printf("RegBit(0x%x) \n",RegBit);
				MMD_BIU_Get_Device_Status(IDX_VID_STATUS, &RegBit);
				MMD_USER_WaitMSEC(10);
				ret--;
			}*/
			if(RegBit & 1<<29)
			{
				MMD_FILE_StartWriteFrCore();
				iFristWrite = 1;
				bit = 0;
				MMD_BIU_Set_Device_Status(IDX_AUD_STATUS, bit);
			}
		}
	}
#endif
}


void media_recorder_jpgcap_timer_creat(void)
{
#if 0
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
#else
	MMD_USER_Timer_Set(media_recorder_jpgcap_time_dsr, 10);
#endif
}

void media_recorder_jpgcap_timer_release(void)
{
#if 0
	STATUS	status;
      	//video syn timer delete (SW timer)
 	status = NU_Delete_Timer(&media_recorder_jpgcap_timer);
	if (status != NU_SUCCESS)
	{
		mp_print(("media_recorder_jpgcap_timer_release error = %d", (int)status));
	}
#else
	//MMD_USER_Timer_Release();
#endif
}

void media_recorder_jpgcap_timer_start(void)
{
	MR_U32 filestatus = 0;

//	g_syntimecnt = 0;
	iFristWrite = 0;
	MMD_BIU_Set_Device_Status(IDX_AUD_STATUS, 0);
	filestatus &= ~(0x1); /* clear busy bit of file operation status */
	filestatus &= 0xFFFF; /* clear error bits of file operation status */
	MMD_BIU_Set_Device_Status(IDX_FILE_STATUS, filestatus);
	
#if 0
	NU_Control_Timer(&media_recorder_jpgcap_timer, NU_ENABLE_TIMER);
#else
	MMD_USER_Timer_Start();
#endif
	mp_print(("[media_recorder_jpgcap_timer_start]"));
}

void media_recorder_jpgcap_timer_stop(void)
{
	MR_U32 writeflag,bit,timeout,filestatus;
	MMD_BIU_Get_Device_Status(IDX_VID_STATUS, &writeflag);
	MMD_BIU_Get_Device_Status(IDX_AUD_STATUS, &bit);
	MMD_BIU_Get_Device_Status(IDX_FILE_STATUS, &filestatus);
	
	for(timeout=0;timeout<20;timeout--)
	{
		if(!bit&&!writeflag&&!filestatus)
			break;
		MMD_BIU_Get_Device_Status(IDX_VID_STATUS, &writeflag);
		MMD_BIU_Get_Device_Status(IDX_AUD_STATUS, &bit);
		MMD_BIU_Get_Device_Status(IDX_FILE_STATUS, &filestatus);
		mp_printf("Wirte Not End! bit(0x%x) writeflag(0x%x)\n",bit,writeflag);
		MMD_USER_WaitMICRS(1000);
	}
	mp_printf("Time Stop --writeflag (0x%x) bit(0x%x)\n",writeflag,bit);
#if 0
	NU_Control_Timer(&media_recorder_jpgcap_timer, NU_DISABLE_TIMER);
#else
	MMD_USER_Timer_Stop();
#endif
	
	mp_print(("[media_recorder_jpgcap_timer_stop]"));
}

#if VIDEORECORDERIN848
extern MMD_U08 play_timer_flag;
extern MMD_U08 lcd_refresh_flag;

void media_recorder_time_dsr(void)
{
	MR_U32  current;
	//MR_U32  bit=1;
	//MR_U32  videobuffercount=0;
	//MR_S32	current_decode=0;
	MR_U32  current_ticks, delta_ticks;
	//MR_U32  Flag=0;
	MR_U32 	videostatus=0,audiostatus=0,filestatus=0,ret;
	//MR_U32 	framesize=0;
	MR_U32  flg_start2writecore=0;
	MR_U32  framesdonestatus=0;
	//MR_U32  frame0size=0,frame1size=0;
	MR_U32  frameinfo=0,i;

	video_stream_t *dstvideo;
	audio_stream_t *dstaudio;
	VIM_MEDIA_RECORDER_FILE*	rfile;
	MR_FILE* mrfile=NULL;

	//if(emMMDUpdateState == NEED_UPDATE  && !g_BIU_Count)
	//{
	//	MMD_LCD_Refresh_In_Timer();
	//	emMMDUpdateState = UPDATED;
	//}
	
	while(lcd_refresh_flag==1)
   {
      rex_sleep(1);
   }
   play_timer_flag=1;
	if(emMMDUpdateState == NEED_UPDATE)
	{
		MMD_LCD_Refresh_In_Timer_Video();
		emMMDUpdateState = UPDATED;
	}
	
	rfile = media_sysstatus->rcfilelst[0];
	mrfile = rfile->mr_filehandle;
	dstvideo = &(rfile->mr_filehandle->video_data.dst);
	dstaudio = &(rfile->mr_filehandle->audio_data.dst);

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
	//rfile->f_recordtime+=delta_ticks;
	MMD_BIU_Get_Device_Status(IDX_FILE_STATUS,&filestatus);
	
	if(filestatus >> 16)
	{
		if(!((filestatus >> 16)==0x40))
		{
			mp_printf("\nwrite video data is error, 0x%x\n", (filestatus >> 16));
			rfile->f_recordcontrol|=MR_VIDEO_WRITE_ERROR;
			media_recorder_timer_stop();
			//media_recorder_file_stop((VIM_MEDIA_RECFILE)rfile);
		if(g_recCallback) 
		{
			(*g_recCallback)(MMD_REC_FILE_END,0,0);		
		}
		g_recCallback=NULL;		
			return;
		}			
	}	
	
	if(mrfile->audio_infor.a_isvalid)
	{
		MMD_BIU_Get_Device_Status(IDX_AUD_STATUS,&audiostatus);
		current = dstaudio->buffer_write;		
		if((rfile->f_recordtime >= rfile->f_limitedtime)|(rfile->f_recordsize>= rfile->f_limitedsize))
		{ 
			//mp_printf("f_recordtime(%d) f_limitedtime(%d) f_recordsize(%d) f_limitedsize(%d)\n",rfile->f_recordtime,rfile->f_limitedtime,rfile->f_recordsize,rfile->f_limitedsize);
			dstaudio->buffer[current].is_over=1;
			if(audiostatus &(1<<current))
			{
				if(filestatus & 0x1)
	    			{
	    				MSG_ERROR("For the last audio data write,Now SD is buzy!writeflag(0x%x)\n",filestatus, 0, 0);
				}
	    			else
	    			{
	    				dstaudio->buffer[current].cursor_write=1600;
					dstaudio->buffer[current].data_length=1600;
					dstaudio->buffer[current].frame_count=10;
					dstaudio->buffer[current].frame_size=SAMR_ENCODE_FRAME_ONCE*32;
					dstaudio->buffer[current].frame_duration=SAMR_ENCODE_FRAME_ONCE*160;
					g_bufferindex_audio=current;
					MSG_ERROR("encode last %d audio frames,write buf %x\n",dstaudio->buffer[current].frame_count,g_bufferindex_audio, 0);
					ret=media_recorder_audio_write_frames(rfile,current);
					if(ret)
					{
						rfile->f_recordcontrol|=MR_VIDEO_WRITE_ERROR;
                            		media_recorder_timer_stop();
                            		//media_recorder_file_stop((VIM_MEDIA_RECFILE)rfile);
		if(g_recCallback) 
		{
			(*g_recCallback)(MMD_REC_FILE_END,0,0);		
		}
		g_recCallback=NULL;		
                            		return;
					}					
					flg_start2writecore++;
					current++;
					if(current>=dstaudio->buffer_count)
						current = 0;
					dstaudio->buffer_write= current;
	    			}
			}
			if((rfile->f_recordcontrol & MR_AUDIO_RECORD_END)==MR_AUDIO_RECORD_END)
				MSG_ERROR("record audio data is finished,rawdata length is %d\n",rfile->f_recordsize, 0, 0);
		}
		else
		{
			//mp_printf("audio writeflag(0x%x)\n",audiostatus);
			if(audiostatus &(1<<current))
	    		{
				if(filestatus & 0x1)
				{
					MSG_ERROR("For audio data write,Now SD is buzy!writeflag(0x%x)\n",filestatus, 0, 0);
				}
	    			else
	    			{
					dstaudio->buffer[current].cursor_write=1600;
					dstaudio->buffer[current].data_length=1600;
					dstaudio->buffer[current].frame_count=10;
					dstaudio->buffer[current].frame_size=SAMR_ENCODE_FRAME_ONCE*32;
					dstaudio->buffer[current].frame_duration=SAMR_ENCODE_FRAME_ONCE*160;
					g_bufferindex_audio=current;
					MSG_ERROR("encode %d audio frames,write buf %x\n",dstaudio->buffer[current].frame_count,g_bufferindex_audio, 0);
					ret=media_recorder_audio_write_frames(rfile,current);
					if(ret)
					{
						rfile->f_recordcontrol|=MR_VIDEO_WRITE_ERROR;
                            		media_recorder_timer_stop();
		if(g_recCallback) 
		{
			(*g_recCallback)(MMD_REC_FILE_END,0,0);		
		}
		g_recCallback=NULL;		
                            		return;
					}
					flg_start2writecore++;
					current++;
					if(current>=dstaudio->buffer_count)
						current = 0;
					dstaudio->buffer_write= current;
	    			}
	    		}
		}
	}	
	
	if(mrfile->video_infor.v_isvalid)
	{
		current = dstvideo->buffer_write;
       	//check record time limited and size limited
		if((rfile->f_recordtime >= rfile->f_limitedtime)|(rfile->f_recordsize>= rfile->f_limitedsize))
		{    
			
			if(rfile->v_framesdone ==1)
			{
				MSG_ERROR("f_recordtime(%d) f_limitedtime(%d)\n",rfile->f_recordtime,rfile->f_limitedtime, 0);
				rfile->mr_filehandle->video_encoder->stop(rfile->mr_filehandle->video_encinfor);
				dstvideo->buffer[current].is_over= 1;
				rfile->f_recordcontrol |=MR_VIDEO_RECORD_END;					
			}

#if 0			
			MMD_BIU_Get_Device_Status(IDX_VID_STATUS, &videostatus);
			//while(videostatus &(1<<current))
			if(videostatus &(1<<current))
			{
				if(filestatus & 0x1)
				{
					//rfile->v_framesdone = 0;
					mp_printf("For the last video data write,Now SD is buzy!writeflag(0x%x)\n",filestatus);
					//break;
				}
				else
				{
					/*index=7+current;
					MMD_BIU_Get_Device_Status(index, &framesize);
					//mp_printf("framesize is %x\n",framesize);
					//HAL_READ_UINT32(gD2H_STATUS_MemAddr[index], framesize);
					frame0size=(framesize&0x0000ffff);
					frame1size=((framesize&0xffff0000)>>16);
					dstvideo->buffer[current].frame_size[0] = frame0size;
					dstvideo->buffer[current].frame_size[1] = frame1size;
					dstvideo->buffer[current].data_length = frame1size+frame0size;
					dstvideo->buffer[current].cursor_write= frame1size+frame0size;
					dstvideo->buffer[current].frame_count=2;
					rfile->v_frames +=2;*/	
					frameinfo=Frame_Info[current];
					MMD_SYS_Mem_Read(frameinfo,(MR_S08 *)&(dstvideo->buffer[current].frame_count),4);
					frameinfo+=4;
					rfile->v_frames +=dstvideo->buffer[current].frame_count;
					for(i=0;i<dstvideo->buffer[current].frame_count;i++)
					{
						MMD_SYS_Mem_Read(frameinfo,(MR_S08 *)&(dstvideo->buffer[current].frame_size[i]),4);
						frameinfo+=4;
						dstvideo->buffer[current].data_length += dstvideo->buffer[current].frame_size[i];
						dstvideo->buffer[current].cursor_write+= dstvideo->buffer[current].frame_size[i];
					}
					//mp4v_encinfor.key_vop_flag = (rfile->v_frames%3) ? VIM_MPEG4_VOP_P : VIM_MPEG4_VOP_I;
					//mp4v_encinfor.enc_ticks_inc=100;
					g_bufferindex_video=current;
					mp_printf("encode last %d frames,size is %d\n",dstvideo->buffer[current].frame_count,dstvideo->buffer[current].data_length);
					
					ret=media_recorder_video_write_frames(rfile,current);
					flg_start2writecore++;
					current++;
					if(current>=dstvideo->buffer_count)
						current = 0;
					dstvideo->buffer_write= current;
				} 
			}
#endif

			if(rfile->v_framesdone ==1)
			{
				MMD_BIU_Get_Device_Status(5, &endduration);
				MSG_ERROR("endduration is %d\n",endduration, 0, 0);
				rfile->v_framesdone = 0;
			}
		}
		else
		{
   			MMD_BIU_Get_Device_Status(IDX_VID_STATUS, &videostatus);
			if(videostatus &(1<<current))
			//while((videostatus &(1<<current))&&(videobuffercount<dstvideo->buffer_count))
			{
				//MMD_BIU_Get_Device_Status(IDX_AUD_STATUS,&audiostatus);
				if(filestatus & 0x1)
				{
					rfile->v_framesdone = 0;
					MSG_ERROR("For video data write,Now SD is buzy!writeflag(0x%x)\n",filestatus, 0, 0);
					/*current_decode=current-1;
					if(current_decode<0)
						current_decode=(dstvideo->buffer_count)-1;*/
					/*current_decode++;
					if(current_decode>=dstvideo->buffer_count)
						current_decode=0;	
					MMD_BIU_Get_Device_Status(6,&framesdonestatus);
					
					if((!(videostatus &(1<<current_decode)))&&(framesdonestatus&(1<<current_decode)))
	        			{
	        				mp_printf("Also SD is buzy,but should used buffer %d to start encode,because last time is buzy\n",current_decode);
	        				MMD_BIU_Set_Device_Status(6,0);
	        				rfile->mr_filehandle->video_encoder->encode_frames(rfile->mr_filehandle->video_encinfor,NULL,(void*)&(dstvideo->buffer[current_decode]));
	        			}*/
					//break;
				}
				else
				{
					/*index=7+current;
					MMD_BIU_Get_Device_Status(index, &framesize);
					//mp_printf("framesize is %x\n",framesize);
					//HAL_READ_UINT32(gD2H_STATUS_MemAddr[index], framesize);
					frame0size=(framesize&0x0000ffff);
					frame1size=((framesize&0xffff0000)>>16);
					dstvideo->buffer[current].frame_size[0] = frame0size;
					dstvideo->buffer[current].frame_size[1] = frame1size;
		            	   	dstvideo->buffer[current].data_length = frame1size+frame0size;
		            		dstvideo->buffer[current].cursor_write= frame1size+frame0size;
		            		dstvideo->buffer[current].frame_count=2;*/
		            		frameinfo=Frame_Info[current];
		            		MMD_SYS_Mem_Read(frameinfo,(MR_S08 *)&(dstvideo->buffer[current].frame_count),4);
					frameinfo+=4;
					rfile->v_frames +=dstvideo->buffer[current].frame_count;
					//mp_printf("video frames is %d\n",rfile->v_frames);
					for(i=0;i<dstvideo->buffer[current].frame_count;i++)
					{
						MMD_SYS_Mem_Read(frameinfo,(MR_S08 *)&(dstvideo->buffer[current].frame_size[i]),4);
						//mp_printf("frame %d , size is %d\n",i,dstvideo->buffer[current].frame_size[i]);
						dstvideo->buffer[current].data_length += dstvideo->buffer[current].frame_size[i];
            					dstvideo->buffer[current].cursor_write+= dstvideo->buffer[current].frame_size[i];
						frameinfo+=4;
					}
					mp4v_encinfor.key_vop_flag = (rfile->v_frames%VIDEO_ENCODE_FRAME_ONCE) ? VIM_MPEG4_VOP_P : VIM_MPEG4_VOP_I;
					mp4v_encinfor.enc_ticks_inc=100;
					g_bufferindex_video=current;
					MSG_ERROR("encode %d frames,size is %d\n",dstvideo->buffer[current].frame_count,dstvideo->buffer[current].data_length, 0);
					/*mp_printf("encode %d video frames,size is %d and %d,write buf %d\n",dstvideo->buffer[current].frame_count,
																		dstvideo->buffer[current].frame_size[0],
																		dstvideo->buffer[current].frame_size[1],
																		g_bufferindex_video);*/
					ret=media_recorder_video_write_frames(rfile,current);
					if(ret)
					{
						rfile->f_recordcontrol|=MR_VIDEO_WRITE_ERROR;
                            		media_recorder_timer_stop();
		if(g_recCallback) 
		{
			(*g_recCallback)(MMD_REC_FILE_END,0,0);		
		}
		g_recCallback=NULL;		
                            		return;
					}
					flg_start2writecore++;
					current++;
					if(current>=dstvideo->buffer_count)
						current = 0;
					dstvideo->buffer_write= current;
					//videobuffercount++;
				} 				
			}

			if(rfile->v_framesdone == 0)
			{
				rfile->v_framesdone=1;
			}
			else
			{
				MMD_BIU_Get_Device_Status(6,&framesdonestatus);
				//mp_printf("framesdonestatus is %x\n",framesdonestatus);
        		if(framesdonestatus&(1<<current))
        		{
        			MSG_ERROR("now should used buffer %d to start encode,because last time is buzy\n",current, 0, 0);
        			MMD_BIU_Set_Device_Status(6,0);
        			rfile->mr_filehandle->video_encoder->encode_frames(rfile->mr_filehandle->video_encinfor,NULL,(void*)&(dstvideo->buffer[current]));
        		}
			}
			
		}
	}
	
	if(flg_start2writecore) 
	{  
		ret = MMD_FILE_StartWriteFrCore();
		iFristWrite = 1;
		if(ret) 
		{
			MSG_ERROR("MMD_FILE_StartWriteFrCore=0x%08x",ret, 0, 0);
		}
	}
	MSG_ERROR("rfile->f_recordcontrol(0x%x)", rfile->f_recordcontrol, 0, 0);
	//if((rfile->f_recordcontrol & MR_AUDIO_RECORD_END)==MR_AUDIO_RECORD_END) ||
	//if((rfile->f_recordcontrol & MR_VIDEO_RECORD_END)==MR_VIDEO_RECORD_END)
	if((rfile->f_recordcontrol & MR_FILE_RECORD_END)==MR_FILE_RECORD_END)
	{
		//media_recorder_file_stop((VIM_MEDIA_RECFILE)rfile);
		//timecount=0;
		if(recorder_stop==1)
		{
			recorder_stop=0;
		}
        
		MSG_ERROR("recorder_stop(%d) before timer_stop", recorder_stop, 0 ,0);
		media_recorder_timer_stop();
      
		
		if(g_recCallback) 
		{
			(*g_recCallback)(MMD_REC_FILE_END,0,0);		
		}
		g_recCallback=NULL;		
	}
    play_timer_flag=0;
}

#else
void media_recorder_time_dsr(void)
{
	MR_U32  current, frame_index;
	MR_U32  current_ticks, delta_ticks;
	MR_U32  Flag=0;
	MR_U32 	filestatus=0,ret;
	MR_U32 	framesize=0;
	MR_U32  flg_start2writecore=0;
	
	video_stream_t*	dst;
	VIM_MEDIA_RECORDER_FILE*	rfile;
	MR_FILE* mrfile=NULL;
	
	rfile = media_sysstatus->rcfilelst[0];
	mrfile = rfile->mr_filehandle;
	dst = &(rfile->mr_filehandle->video_data.dst);

	MMD_BIU_Get_Device_Status(IDX_FILE_STATUS, &filestatus);
	
	if(mrfile->video_infor.v_isvalid)
	{
		//decide if it has happened write error
		if(filestatus >> 16)
		{
			mp_printf("write video data is error, 0x%x\n", (filestatus >> 16));
			rfile->f_recordcontrol|=MR_VIDEO_WRITE_ERROR;
			media_recorder_file_stop((VIM_MEDIA_RECFILE)rfile);
			return;
		}

		//check if it did not write data at last time
		if(rfile->v_framesdone == 0)
		{
			current = dst->buffer_write;
			//MMD_BIU_Get_Device_Status(IDX_VID_STATUS, &writeflag);
			//if(writeflag & 1<<28)
			//if((writeflag & 1<<28)||(writeflag & 1<<27))
			if(filestatus & 0x1)
			{
				rfile->v_framesdone = 0;
				mp_printf("Last Write Is Not Flished!writeflag(0x%x)\n",filestatus);
				return;
			}
			else
			{
				media_recorder_video_write_frames(rfile,current);
				flg_start2writecore++;
				rfile->v_framesdone = 1;
				if(dst->buffer[current].is_over == 1)
				{
					mp_printf("record video data is finished,frame count is %d,rawdata length is %d\n",rfile->v_frames,rfile->f_recordsize);
					rfile->f_recordcontrol|=MR_VIDEO_RECORD_END;
					media_recorder_file_stop((VIM_MEDIA_RECFILE)rfile);									
				}
				else
				{
					current++;
					if(current>=dst->buffer_count)
						current = 0;
					dst->buffer_write= current; 			
					rfile->mr_filehandle->video_encoder->encode_frames(rfile->mr_filehandle->video_encinfor,NULL,(void*)&(dst->buffer[current]));
				}
			
			}
		}
		
		//check the mp4 encode module status
		Flag=VIM_HAPI_GetMPEGEncIntStatus();    
       	if(Flag)
       	{
       		VIM_HAPI_SetMPEGEncIntStatus(0);
   			if(Flag&VIM_MPEG4_ENCVOP_FINISH)
            {
          		framesize=VIM_HAPI_GetMp4EncVopLength();  	
				
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
          		rfile->f_recordtime += delta_ticks*10;	//(n*10ms ticks to ms)
          		rfile->v_duration = rfile->f_recordtime-rfile->v_time;
      			rfile->v_time += rfile->v_duration;	
          			
          		//update current video size, duration, synflag
            	current = dst->buffer_write;
            	frame_index = dst->buffer[current].frame_count;
            	dst->buffer[current].frame_duration[frame_index] = rfile->v_duration; 
            	dst->buffer[current].frame_size[frame_index] = framesize; 
            	dst->buffer[current].frame_synflag[frame_index] =(mp4v_encinfor.key_vop_flag) ? 0 : 1;
            	dst->buffer[current].data_length += framesize;
            	dst->buffer[current].cursor_write += framesize;
            	dst->buffer[current].frame_count++;
            	rfile->v_frames ++;
            	//check record time limited and size limited
            	if((rfile->f_recordtime >= rfile->f_limitedtime)|(rfile->f_recordsize>= rfile->f_limitedsize))
            	{    
            		mp_printf("f_recordtime(%d) f_limitedtime(%d) f_recordsize(%d) f_limitedsize(%d)\n",rfile->f_recordtime,rfile->f_limitedtime,rfile->f_recordsize,rfile->f_limitedsize);
            		rfile->mr_filehandle->video_encoder->stop(rfile->mr_filehandle->video_encinfor);	
            		dst->buffer[current].is_over= 1;
            		dst->buffer[current].isnot_empty	= 1;

					//MMD_BIU_Get_Device_Status(IDX_VID_STATUS, &writeflag);
					//if(writeflag & 1<<28)
					//if((writeflag & 1<<28)||(writeflag & 1<<27))
					if(filestatus & 0x1)
					{
						rfile->v_framesdone = 0;
						mp_printf("Last Write Is Not Flished!writeflag(0x%x)\n",filestatus);
					}
					else
					{
						media_recorder_video_write_frames(rfile,current);
						flg_start2writecore++;
						rfile->v_framesdone = 1;
						mp_printf("record video data is finished,frame count is %d,rawdata length is %d\n",rfile->v_frames,rfile->f_recordsize);
						rfile->f_recordcontrol|=MR_VIDEO_RECORD_END;
						media_recorder_file_stop((VIM_MEDIA_RECFILE)rfile);
					}        			
            	}
				else
				{
                  	//update encoder parameter
                  	mp4v_encinfor.enc_ticks_inc = rfile->v_duration;
                  	mp4v_encinfor.last_frame_size = framesize;
                  	mp4v_encinfor.key_vop_flag = (rfile->v_frames%3) ? VIM_MPEG4_VOP_P : VIM_MPEG4_VOP_I;	
              		if((dst->buffer[current].frame_count>=VIM_USER_MPEG4_ENC_OUTBUF_FRAMES_NUM)|(Flag&2))
              			/*|(dst->buffer[current].data_length*4>dst->buffer[current].buffer_size*3)
              			|(Flag&2))	//last frame encode finish*/
              		{
              			dst->buffer[current].isnot_empty	= 1;
    					mp_printf("encode %d frames\n",dst->buffer[current].frame_count);
    
    					//MMD_BIU_Get_Device_Status(IDX_VID_STATUS, &writeflag);
    					//if(writeflag & 1<<28)
    					//if((writeflag & 1<<28)||(writeflag & 1<<27))
    					if(filestatus & 0x1)
    					{
    						rfile->v_framesdone = 0;
    						mp_printf("Last Write Is Not Flished!writeflag(0x%x)\n",filestatus);
    						
    					}
    					else
    					{
    						ret=media_recorder_video_write_frames(rfile,current);
							flg_start2writecore++;
    						current++;
            				if(current>=dst->buffer_count)
            					current = 0;
            				dst->buffer_write= current;
    						rfile->v_framesdone = 1;
    						rfile->mr_filehandle->video_encoder->encode_frames(rfile->mr_filehandle->video_encinfor,NULL,(void*)&(dst->buffer[current]));
    					}
                	}
                	else
                	{
                		rfile->mr_filehandle->video_encoder->encode_frames(rfile->mr_filehandle->video_encinfor,NULL,(void*)&(dst->buffer[current]));
                	}
    			}
            }
          	else
          	{
          		if(Flag&VIM_MPEG4_ENCSTUFF_ERROR)
          		{
          			if(Flag&0x20)
          		    {
          				mp_printf("Sensor is faster than mp4 encoder's speed\n");
          	       	}
          			else if(Flag & 0x08)
          			{
          				mp_printf("VENC is blocking on LBUF pingpang status mismatch\n");
          			}
          			else if(Flag & 0x10)
          			{
          				mp_printf("VENC is blocking on LBUF vsync status mismatch\n");
          			}
          			else 
          			{
          				mp_printf("VENC is blocking on timeout stuck error\n");
          			}
          			
              		return;
          		}
       		}
       	}
	}
	
	if(flg_start2writecore) 
	{  
		ret = MMD_FILE_StartWriteFrCore();
		if(ret) 
		{
			mp_error(("MMD_FILE_StartRead2Core=0x%08x",ret));
		}
	}

/*	if((rfile->f_recordcontrol & MR_VIDEO_RECORD_END))
		//&& (rfile->f_playcontrol & MP_AUDIO_STREAM_END)) 
	{
		//mp_error(("MMD_FILE_StartRead2Core=0x%08x",ret));
		media_recorder_file_stop((VIM_MEDIA_RECFILE)&rfile);
		return;
	}*/
}
#endif
void media_recorder_timer_creat(void)
{
#if 0 
	STATUS	status;
      //video syn timer create (SW timer)
 		
	status = NU_Create_Timer(&media_recorder_timer, 	//NU_TIMER *timer
 						      "media recorder timer", 		//CHAR *name
 						      	media_recorder_time_dsr,	//VOID (*expiration_routine)(UNSIGNED), 
							3, 					//UNSIGNED id  = 2
							100, 					//UNSIGNED initial_time = 10ms 
							2, 					//UNSIGNED reschedule_time = 10ms intervale
							NU_DISABLE_TIMER);	//OPTION enable
	if (status != NU_SUCCESS)
	{
		mp_print(("media_recorder_timer_creat error = %d", (int)status));
	}
#else
	MMD_USER_Timer_Set(media_recorder_time_dsr, 20);
#endif
}

void media_recorder_timer_release(void)
{
#if 0
	STATUS	status;
      	//video syn timer delete (SW timer)
 	status = NU_Delete_Timer(&media_recorder_timer);
	if (status != NU_SUCCESS)
	{
		mp_print(("media_recorder_timer_release error = %d", (int)status));
	}
#else
	//MMD_USER_Timer_Release();
#endif
}

void media_recorder_timer_start(void)
{
#if 0
//	g_syntimecnt = 0;
	NU_Control_Timer(&media_recorder_timer, NU_ENABLE_TIMER);
	mp_print(("[media_recorder_timer_start]"));
#else
	MMD_USER_Timer_Start();
    emMMDTimerState=TIMER_STARTED;
#endif
}

void media_recorder_timer_stop(void)
{
#if 0
	NU_Control_Timer(&media_recorder_timer, NU_DISABLE_TIMER);
	mp_print(("[media_recorder_timer_stop]"));
#else
	MMD_USER_Timer_Stop();
    emMMDTimerState=NO_TIMER_START;
    play_timer_flag=0;
#endif
}


MR_U32 media_recoder_state_detect(void)
{
	MR_U32 state;
	MMD_BIU_Get_Device_Status(IDX_MP3_STATUS,&state);
	return state;
}




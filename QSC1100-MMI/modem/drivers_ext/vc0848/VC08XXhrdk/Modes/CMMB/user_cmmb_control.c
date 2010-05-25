/*======================================================================
*                                                                       
*                Copyright (C) 2008 Vimicro CO.,LTD     		 
* File Name: 	user_cmmb_control.c 
* User for : 	820
* Creat By : 	anton liu                                                                   
* CrateDate: 	2008/10/13
* Last modify : anton liu   2008-xx-xx     
*
* version 0.1: 
*
* release history:
* 		version 0.1 release by anton liu
=======================================================================*/
#ifdef CMMB_ENABLE	

#include "user_cmmb_control.h"
#include "mmp_rdk.h"
#include "media_utils.h"

#define ESG_TIMEOUT	2000


MMD_U32 user_cmmb_esg_type = 0;
CMMB_ESG_Buf_Info user_cmmb_esg_buf_info;
ESG_BUF_ADDR user_cmmb_esg_buf_addr;
CMMB_ServiceElement user_cmmb_service_element;
CMMB_ServiceSchedule user_cmmb_service_schedule;
MMD_U32 esg_element_count = 0;
USER_CMMB_SERVICE_LIST* gp_service_list;
USER_CMMB_SCHEDULE_LIST* gp_schedule_list;

USER_CMMB_PLAY_LIST* gp_play_list;
MMD_U32 user_cmmb_timer_count;

cmmbTimer_node_list* gp_cmmb_timer_list;
MMD_U32 g_max_esg_num = 10;
MMD_U32 cmmb_timer_run;

void default_esg_save(MMD_S08* buf, MMD_U32 len, MMD_U32 esg_element_num, MMD_U32 type, MMD_U32* result)
{
	
	*result = USER_CMMB_NO_ERROR;
}

ESG_SAVE_CALLBACK gp_esg_save_callback = default_esg_save;
//: Only timer dsr in CMMB
void cmmb_timer_dsr(void)
{
	cmmbTimer_node_t* cmmb_timer_node;
	cmmbTimer_routine_t* cmmb_timer_routine;
	cmmb_timer_node = gp_cmmb_timer_list->first;
	while(cmmb_timer_node)
	{
		cmmb_timer_routine = (cmmbTimer_routine_t*)cmmb_timer_node->content;
		if(cmmb_timer_routine->enable)
		{
			if((++(cmmb_timer_routine->intr_cnt) * CMMB_TIMER_INTERVAL) == (cmmb_timer_routine->interval))
			{
				cmmb_timer_routine->timer_process();
				cmmb_timer_routine->intr_cnt = 0;
			}
		}
		else
		{
			cmmb_timer_routine->intr_cnt = 0;
		}
		cmmb_timer_node = cmmb_timer_node->next;
	}
}

void user_cmmb_esg_timer_dsr(void)
{
	MMD_U32 buf_index = 0;
	MMD_U32 element_index = 0;
	MMD_S08* tmp_buf;
	MMD_U32 ret;
	MMD_S32 i = 0;
	MMD_U32 save_result;
	user_cmmb_timer_count++;
	if(user_cmmb_timer_count >= ESG_TIMEOUT)	//	ESG timeout
	{
		CMMB_PRINTF("ESG Time Out!\n");
		user_cmmb_timer_count = 0;
		MMD_ESG_STOP();	//	Force Stop ESG processing
	}
	
	if(MMD_CMMB_ESG_DEMUX_OVER())	//	End of ESG demux
	{
	esg_demux_over:
		CMMB_PRINTF("End of ESG demux\n");
		MMD_CMMB_ESG_BUF_RELEASE();
		MMD_MemSet((void*)&user_cmmb_esg_buf_info, 0, sizeof(CMMB_ESG_Buf_Info));
		//MMD_USER_Timer_Stop();
		USER_CMMB_Timer_Stop("esgt");
		CMMB_PRINTF("cmmb_esg_timer stop\n");
		if(user_cmmb_esg_type == ESG_TYPE_SERVICE)
		{
			gp_service_list->fill_over = 1;
		}
		else if(user_cmmb_esg_type == ESG_TYPE_SCHEDULE)
		{
			gp_schedule_list->fill_over = 1;
		}
		else {;}
			
		return;
	}
	for(buf_index=0; buf_index<user_cmmb_esg_buf_info.buffer_count; buf_index++)
	{
		if(MMD_CMMB_CHECK_BUF_FULL(buf_index))	//buffer full
		{		
			//CMMB_PRINTF("Timer buffer[%d] full\n", buf_index);
			tmp_buf = (MMD_S08*)(MMD_Malloc(user_cmmb_esg_buf_info.esg_buffer[buf_index].buffer_size));
			MMD_MemSet(tmp_buf, 0, user_cmmb_esg_buf_info.esg_buffer[buf_index].buffer_size);
			//CMMB_PRINTF("Get buffer from addr=0x%x\n", user_cmmb_esg_buf_addr.BUF_ADDR[buf_index]);	
			ret = MMD_CMMB_ESG_GET_BUF(user_cmmb_esg_buf_addr.BUF_ADDR[buf_index], &(user_cmmb_esg_buf_info.esg_buffer[buf_index]), tmp_buf);
			//CMMB_PRINTF("MMD_CMMB_ESG_GET_BUF Over, data_length=%d, element_num=%d\n", user_cmmb_esg_buf_info.esg_buffer[buf_index].data_length, user_cmmb_esg_buf_info.esg_buffer[buf_index].element_num);

			switch(user_cmmb_esg_type)	
			{
			case ESG_TYPE_SERVICE:
				for(element_index=0; element_index<user_cmmb_esg_buf_info.esg_buffer[buf_index].element_num; element_index++)
				{
					MMD_MemSet(&user_cmmb_service_element, 0, sizeof(CMMB_ServiceElement));
					MMD_MemCpy(&user_cmmb_service_element, (tmp_buf+user_cmmb_esg_buf_info.esg_buffer[buf_index].cursor_read), sizeof(CMMB_ServiceElement));
					user_cmmb_esg_buf_info.esg_buffer[buf_index].cursor_read += sizeof(CMMB_ServiceElement);
					MMD_MemCpy(&(gp_service_list->service_element[esg_element_count]), &user_cmmb_service_element, sizeof(CMMB_ServiceElement));
					gp_service_list->service_num++;
					for(i=0; i<gp_play_list->play_num; i++)
					{
						if(gp_play_list->play_info[i].service_id == gp_service_list->service_element[esg_element_count].id)
						{
							gp_play_list->play_info[i].service_info = &(gp_service_list->service_element[esg_element_count]);
							break;
						}
					}
					esg_element_count++;
				}
				break;
			case ESG_TYPE_SCHEDULE:
				esg_element_count += user_cmmb_esg_buf_info.esg_buffer[buf_index].element_num;
				gp_schedule_list->schedule_num += user_cmmb_esg_buf_info.esg_buffer[buf_index].element_num;
				save_result = 0;
				if(gp_esg_save_callback)
				{
					 gp_esg_save_callback(tmp_buf, user_cmmb_esg_buf_info.esg_buffer[buf_index].data_length, user_cmmb_esg_buf_info.esg_buffer[buf_index].element_num, ESG_TYPE_SCHEDULE, &ret);
					 if(ret)
					 {
					 	CMMB_PRINTF("gp_esg_save_callback ret=%d\n", ret);
						if(ret == USER_CMMB_ESG_SAVEBUF_FULL)
						{
							MMD_ESG_STOP();	//	Force Stop ESG processing
							goto esg_demux_over;
						}
					 }
				}
				break;
			default:
				break;
			}

#if 0
			for(element_index=0; element_index<user_cmmb_esg_buf_info.esg_buffer[buf_index].element_num; element_index++)
			{
				switch(user_cmmb_esg_type)	
				{
				case ESG_TYPE_SERVICE:
					MMD_MemSet(&user_cmmb_service_element, 0, sizeof(CMMB_ServiceElement));
					MMD_MemCpy(&user_cmmb_service_element, (tmp_buf+user_cmmb_esg_buf_info.esg_buffer[buf_index].cursor_read), sizeof(CMMB_ServiceElement));
					user_cmmb_esg_buf_info.esg_buffer[buf_index].cursor_read += sizeof(CMMB_ServiceElement);
					MMD_MemCpy(&(gp_service_list->service_element[esg_element_count]), &user_cmmb_service_element, sizeof(CMMB_ServiceElement));
					gp_service_list->service_num++;
					for(i=0; i<gp_play_list->play_num; i++)
					{
						if(gp_play_list->play_info[i].service_id == gp_service_list->service_element[esg_element_count].id)
						{
							gp_play_list->play_info[i].service_info = &(gp_service_list->service_element[esg_element_count]);
							break;
						}
					}
				
					break;
					
				case ESG_TYPE_SCHEDULE:
					MMD_MemSet(&user_cmmb_service_schedule, 0, sizeof(CMMB_ServiceSchedule));
					MMD_MemCpy(&user_cmmb_service_schedule, (tmp_buf+user_cmmb_esg_buf_info.esg_buffer[buf_index].cursor_read), sizeof(CMMB_ServiceSchedule));
					user_cmmb_esg_buf_info.esg_buffer[buf_index].cursor_read += sizeof(CMMB_ServiceSchedule);
					MMD_MemCpy(&(gp_schedule_list->service_schedule[esg_element_count]), &user_cmmb_service_schedule, sizeof(CMMB_ServiceSchedule));
					gp_schedule_list->schedule_num++;
					break;
					
				default:
					break;
				}


			}
#endif
			if(esg_element_count >= (g_max_esg_num-1))
			{
				CMMB_PRINTF("esg_element_count overlap, stop ESG processing!\n");
				MMD_ESG_STOP();	//	Force Stop ESG processing
				goto esg_demux_over;						
			}				
			
			ret = MMD_CMMB_SET_BUF_Empty(buf_index);
			if(ret) CMMB_PRINTF("MMD_CMMB_SET_ESGBUF_Empty Fail!, ret=0x%x\n", ret);

			MMD_Free((void*)tmp_buf);
		}

	}

}

void  USER_CMMB_SET_ESG_SAVE_CALLBACK(ESG_SAVE_CALLBACK callback)
{
	gp_esg_save_callback = callback;
}

MMD_U32 USER_CMMB_SEARCH(CMMB_INFO*  pcmmb_info, MMD_U32 try_times, USER_CMMB_PLAY_LIST* pplay_list)
{
	MMD_U32 i, j;
	MMD_U32 ret = 0;
	MMD_U32 ply_count = 0;
	MMD_MemSet(pplay_list, 0, sizeof(USER_CMMB_PLAY_LIST));
	for(i=0; i<try_times; i++)
	{
		//CMMB_PRINTF("MMD_CMMB_SearchChannel once\n");
		MMD_MemSet(&(pcmmb_info->ch_info), 0, sizeof(tCMCT));	
		ret = MMD_CMMB_SearchChannel(pcmmb_info);
		if(ret) 
		{
			CMMB_PRINTF("MMD_CMMB_SearchChannel fail, ret=%d\n", ret);
			CMMB_PRINTF("Search %d times fail\n", i+1);
			continue;
		}
		if(pcmmb_info->ch_info.nMfNum > 0)
			break;
		else
			CMMB_PRINTF("nMfNum=0, Search %d times fail\n", i+1);
	}
	CMMB_PRINTF("pcmmb_info->ch_info.nMfNum=%d\n", pcmmb_info->ch_info.nMfNum);
	if(i < try_times)
	{
		for(j=0; j<pcmmb_info->ch_info.nMfNum; j++)
		{
			CMMB_PRINTF("info=0x%x, start slot=%d\n", pcmmb_info->ch_info.tFrameDesc[j].chnl_info, ((pcmmb_info->ch_info.tFrameDesc[j].chnl_info >> 16)&0xff));
			if(((pcmmb_info->ch_info.tFrameDesc[j].chnl_info >> 16)&0xff) < 3)	//	Avoid TS1 and TS2
			{
				continue;
			}	
			//: Fill play_list
			pplay_list->play_num++;
			pplay_list->play_info[ply_count].channel_no = pcmmb_info->channel_no;
			pplay_list->play_info[ply_count].service_id = pcmmb_info->ch_info.tFrameDesc[j].wServiceId;
			pplay_list->play_info[ply_count].start_slot = (pcmmb_info->ch_info.tFrameDesc[j].chnl_info >> 16) & 0xff;
			pplay_list->play_info[ply_count].slot_count = (pcmmb_info->ch_info.tFrameDesc[j].chnl_info >> 8) & 0xff;
			pplay_list->play_info[ply_count].demod = (pcmmb_info->ch_info.tFrameDesc[j].chnl_info) & 0xff;
			ply_count++;
			///:~

		}
		CMMB_PRINTF("USER_CMMB_SEARCH: pplay_list->play_num=%d\n", pplay_list->play_num);
		return USER_CMMB_NO_ERROR;
	}
	else
		return USER_CMMB_SEARCH_FAIL;
}

MMD_U32 USER_CMMB_Timer_Start(MMD_S08* id)
{
	return cmmbTimer_enable(gp_cmmb_timer_list, id);;
}

MMD_U32 USER_CMMB_Timer_Stop(MMD_S08* id)
{
	return cmmbTimer_disable(gp_cmmb_timer_list, id);;
}

MMD_U32 USER_CMMB_Timer_Register(MMD_S08* id, TimerISR pTimerISR, MMD_U32 interval)
{
	cmmbTimer_routine_t* timer_routine = NULL;

	if(cmmb_timer_run)
	{
		MMD_USER_Timer_Stop();
		cmmb_timer_run = 0;
	}

	timer_routine = (cmmbTimer_routine_t*)MMD_Malloc(sizeof(cmmbTimer_routine_t));
	if(!timer_routine)
	{
		CMMB_PRINTF("Malloc timer_routine fail\n");
		return USER_CMMB_ERR_MALLOC;
	}
	MMDSPrintf(timer_routine->id, id);
	CMMB_PRINTF("Register timer: %s\n", timer_routine->id);
	timer_routine->enable = 0;
	timer_routine->intr_cnt = 0;
	timer_routine->interval = interval;
	timer_routine->timer_process = pTimerISR;
	cmmbTimer_list_append_content(gp_cmmb_timer_list, timer_routine);	

	if(!cmmb_timer_run)
	{
		MMD_USER_Timer_Start();
		cmmb_timer_run = 1;
	}	
	return USER_CMMB_NO_ERROR;
}



#if !FW_DOWNLOAD_CORE
//extern MMD_U08 cmmb_fw_data[24576];
#endif

MMD_U32 USER_CMMB_INIT(CMMB_INFO*  pcmmb_info, MMD_U32 fw_option)
{
	MMD_U32 ret = 0;
	MMD_U32 fw_addr, fw_len;
	MMD_U08* fw_buf = NULL;
	void* mem_file_handle;
	MMD_U32 i = 0;

	//: Create CMMB timer list
	cmmb_timer_run = 0;
	gp_cmmb_timer_list = cmmbTimer_list_create();
	MMD_USER_Timer_Set(cmmb_timer_dsr, CMMB_TIMER_INTERVAL);
	MMD_USER_Timer_Start();
	
	user_cmmb_esg_type = 0;
	//: Select GPIO before CMMB_INIT, May need to close pin functions before MMD_CMMB_GPIO_SELECT according to different PIN.
	//ret = MMD_CMMB_GPIO_SELECT(CMMB_RESET_PIN, CMMB_INTR_PIN);
	ret = MMD_USER_CMMB_GPIO_SELECT(USER_CMMB_RESET_PIN, USER_CMMB_INTR_PIN);
	if(ret)	
	{
		CMMB_PRINTF("MMD_CMMB_GPIO_SELECT fail, ret=0x%x\n", ret);
		return USER_CMMB_MMD_FAIL;
	}

	USER_CMMB_Timer_Register("esgt", user_cmmb_esg_timer_dsr, 20);
	
#if FW_DOWNLOAD_CORE
	return MMD_CMMB_INIT(pcmmb_info, CMMB_MODE_INNO_PLAY, fw_option, NULL, NULL);
#else
#if 1
	fw_addr = (MMD_U32)CMMB_FW_START;
	fw_len = CMMB_FW_END - CMMB_FW_START;
	CMMB_PRINTF("Firmware data length=%d\n", fw_len);
#else
	fw_buf = cmmb_fw_data;
	fw_len = 24576;
#endif
	fw_buf = (MMD_U08*)(MMD_Malloc(24576));
	if(!fw_buf)
		return USER_CMMB_ERR_MALLOC;
	mem_file_handle = memory_fileopen((void*)fw_addr,(void*)&fw_len);
	memory_fileread(mem_file_handle, (char *)fw_buf, fw_len);
	if((fw_len%4) != 0)
	{
		for(i=0; i<(4 - (fw_len%4)); i++)
		{
			fw_buf[fw_len + i] = 0xff;
		}
		fw_len += i;	
	}
	CMMB_PRINTF("Download fw_len=%d\n", fw_len);
	ret = MMD_CMMB_INIT(pcmmb_info, CMMB_MODE_INNO_PLAY, fw_option, fw_buf, fw_len);
	MMD_Free((void*)fw_buf);
	memory_fileclose(mem_file_handle);
	return ret;
#endif
}

MMD_U32 USER_CMMB_SET_DISPLAY_PARA(CMMB_LCD_INFO* lcd_info)
{
	return MMD_CMMB_LCD_INIT(lcd_info);
}

MMD_U32 USER_CMMB_PLAY(CMMB_Play_Info* play_info)
{
	MMD_U32 channel_info;
	channel_info = ( ((play_info->channel_no) << 24) | ((play_info->start_slot) << 16)
		| ((play_info->slot_count) << 8) | (play_info->demod) );
	CMMB_PRINTF("###USER_CMMB_PLAY!\n");
	return MMD_CMMB_PLAY(channel_info, CMMB_RECORD_DISABLE);
}
MMD_U32 USER_CMMB_STOP(void)
{
	return MMD_CMMB_STOP();
}
MMD_U32 USER_CMMB_QUIT(void)
{
	MMD_U32 ret;
	CMMB_PRINTF("USER_CMMB_QUIT!!!\n");
	if(cmmb_timer_run)
	{
		MMD_USER_Timer_Stop();
		cmmb_timer_run = 0;
	}
	cmmbTimer_list_free(gp_cmmb_timer_list);
	ret = MMD_CMMB_QUIT();
	return ret;
}


MMD_U32 USER_CMMB_GetService(CMMB_INFO*  pcmmb_info, USER_CMMB_SERVICE_LIST* pservice_list, USER_CMMB_PLAY_LIST* pplay_list)
{
	MMD_U32 ret = 0;
	MMD_U32 i = 0;
	ESG_BUF_INFO buf_info;
	MMD_U32 service_ch_info = 0;;
	buf_info.BUF_NUM = 2;
	buf_info.BUF_SIZE = 1024;
	user_cmmb_esg_type = ESG_TYPE_SERVICE;
	esg_element_count = 0;
	gp_service_list = pservice_list;
	gp_play_list = pplay_list;
	user_cmmb_timer_count = 0;
	g_max_esg_num = USER_CMMB_MAX_SERVICE_ELEMENT;
	MMD_MemSet((void*)&user_cmmb_esg_buf_info, 0, sizeof(CMMB_ESG_Buf_Info));
	MMD_MemSet((void*)&user_cmmb_service_element, 0, sizeof(CMMB_ServiceElement));
	MMD_MemSet((void*)&user_cmmb_esg_buf_addr, 0, sizeof(ESG_BUF_ADDR));
	MMD_MemSet((void*)pservice_list, 0, sizeof(USER_CMMB_SERVICE_LIST));
	
	//: Initialise  ESG buffer status register 
	MMD_BIU_Set_Device_Status(IDX_AUD_STATUS, 0x0);	

	
		
	if(MMD_CMMB_FIND_ESG_SLOT(pcmmb_info, &service_ch_info) == MMD_CMMB_NO_ERROR)
	{
		CMMB_PRINTF("Get service at 0x%x\n", service_ch_info);
		ret = MMD_CMMB_ESG_BUF_INIT(&buf_info, &user_cmmb_esg_buf_addr);
		if(ret)
		{
			CMMB_PRINTF("MMD_CMMB_ESG_BUF_INIT fail, ret = 0x%x\n", ret);
			return USER_CMMB_ESG_BUF_ERR;
		}

		user_cmmb_esg_buf_info.buffer_count = buf_info.BUF_NUM;
		user_cmmb_esg_buf_info.buffer_read = user_cmmb_esg_buf_info.buffer_write = 0;
		for(i=0; i<user_cmmb_esg_buf_info.buffer_count; i++)
		{
			//aasp_esg_buf_info.esg_buffer[i].buffer_address = buf_addr.BUF_ADDR[i];
			user_cmmb_esg_buf_info.esg_buffer[i].buffer_size = buf_info.BUF_SIZE;
			user_cmmb_esg_buf_info.esg_buffer[i].cursor_read = 0;
			user_cmmb_esg_buf_info.esg_buffer[i].cursor_write = 0;
			user_cmmb_esg_buf_info.esg_buffer[i].data_length = 0;
			user_cmmb_esg_buf_info.esg_buffer[i].element_num = 0;
			user_cmmb_esg_buf_info.esg_buffer[i].is_over = 0;
			MMD_CMMB_SET_BUF_Empty(i);
		}
		
		ret = MMD_CMMB_GetService(service_ch_info, &user_cmmb_esg_buf_info);
		if(ret)	
		{
			CMMB_PRINTF("MMD_CMMB_GetService fail, ret = 0x%x\n", ret);
			return USER_CMMB_MMD_FAIL;
		}
	}
	else
	{
		CMMB_PRINTF("No TS1 found!\n");
		return USER_CMMB_SEARCH_FAIL;
	}
	//: Launch Timer for read ESG
	#if 0
	MMD_USER_Timer_Set(user_cmmb_esg_timer_dsr, 20);
	MMD_USER_Timer_Start();
	#else
	USER_CMMB_Timer_Start("esgt");
	#endif
	CMMB_PRINTF("Start user_cmmb_esg_timer!\n");
	return USER_CMMB_NO_ERROR;

}

MMD_U32 USER_CMMB_GetSchedule(CMMB_INFO*  pcmmb_info, USER_CMMB_SCHEDULE_LIST* pschedule_list)
{
	MMD_U32 ret = 0;
	MMD_U32 i = 0;
	ESG_BUF_INFO buf_info;
	MMD_U32 service_ch_info = 0;;
	buf_info.BUF_NUM = 4;
	buf_info.BUF_SIZE = 40 * sizeof(CMMB_ServiceSchedule);
	user_cmmb_esg_type = ESG_TYPE_SCHEDULE;
	esg_element_count = 0;	
	gp_schedule_list = pschedule_list;
	user_cmmb_timer_count = 0;
	g_max_esg_num = USER_CMMB_MAX_SERVICE_SCHEDULE;
	MMD_MemSet((void*)&user_cmmb_esg_buf_info, 0, sizeof(CMMB_ESG_Buf_Info));
	MMD_MemSet((void*)&user_cmmb_service_schedule, 0, sizeof(CMMB_ServiceSchedule));
	MMD_MemSet((void*)&user_cmmb_esg_buf_addr, 0, sizeof(ESG_BUF_ADDR));
	MMD_MemSet((void*)pschedule_list, 0, sizeof(USER_CMMB_SCHEDULE_LIST));

	//: Initialise  ESG buffer status register 
	MMD_BIU_Set_Device_Status(IDX_AUD_STATUS, 0x0);	

		
	if(MMD_CMMB_FIND_ESG_SLOT(pcmmb_info, &service_ch_info) == MMD_CMMB_NO_ERROR)
	{
		CMMB_PRINTF("Get service at 0x%x\n", service_ch_info);
		ret = MMD_CMMB_ESG_BUF_INIT(&buf_info, &user_cmmb_esg_buf_addr);
		if(ret)
		{
			CMMB_PRINTF("MMD_CMMB_ESG_BUF_INIT fail, ret = 0x%x\n", ret);
			return USER_CMMB_ESG_BUF_ERR;
		}


		user_cmmb_esg_buf_info.buffer_count = buf_info.BUF_NUM;
		user_cmmb_esg_buf_info.buffer_read = user_cmmb_esg_buf_info.buffer_write = 0;
		for(i=0; i<user_cmmb_esg_buf_info.buffer_count; i++)
		{
			user_cmmb_esg_buf_info.esg_buffer[i].buffer_size = buf_info.BUF_SIZE;
			user_cmmb_esg_buf_info.esg_buffer[i].cursor_read = 0;
			user_cmmb_esg_buf_info.esg_buffer[i].cursor_write = 0;
			user_cmmb_esg_buf_info.esg_buffer[i].data_length = 0;
			user_cmmb_esg_buf_info.esg_buffer[i].element_num = 0;
			user_cmmb_esg_buf_info.esg_buffer[i].is_over = 0;
			MMD_CMMB_SET_BUF_Empty(i);
		}
		
		ret = MMD_CMMB_GetSchedule(service_ch_info, &user_cmmb_esg_buf_info);
		if(ret)	
		{
			CMMB_PRINTF("MMD_CMMB_GetService fail, ret = 0x%x\n", ret);
			return USER_CMMB_MMD_FAIL;
		}
	}
	else
	{
		CMMB_PRINTF("No TS1 found!\n");
		return USER_CMMB_SEARCH_FAIL;
	}
	//: Launch Timer for read ESG
	#if 0
	MMD_USER_Timer_Set(user_cmmb_esg_timer_dsr, 20);
	MMD_USER_Timer_Start();
	#else
	USER_CMMB_Timer_Start("esgt");
	#endif
	CMMB_PRINTF("Start user_cmmb_esg_timer_dsr!\n");
	return USER_CMMB_NO_ERROR;

}


MMD_U32 USER_CMMB_GET_SYS_STATUS(CMMB_SysStatus* cmmb_status)
{
	return MMD_CMMB_GET_SYS_STATUS(cmmb_status);
}
MMD_U32 USER_CMMB_SET_VOLUME(MMD_U32 volume)
{
	return MMD_CMMB_SET_VOLUME(volume);
}

MMD_U32 USER_CMMB_ESG_STOP(void)
{
	return MMD_ESG_STOP();
}

#endif

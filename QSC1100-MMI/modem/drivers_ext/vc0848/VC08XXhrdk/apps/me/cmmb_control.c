/*======================================================================
*                                                                       
*                Copyright (C) 2008 Vimicro CO.,LTD     		 
* File Name: 	cmmb_control.c 
* User for : 	820
* Creat By : 	anton liu                                                                   
* CrateDate: 	2008/09/02
* Last modify : anton liu   2008-xx-xx     
*
* version 0.1: 
*
* release history:
* 		version 0.1 release by anton liu
=======================================================================*/

#include "cmmb_control.h"
#include "Davincicode.h"
#include "mmp_rdk.h"
#include "media_utils.h"
#ifdef CMMB_ENABLE	

unsigned int g_cmmb_file_end;
CMMB_FILE* gp_cmmb_file;
CMMB_INFO* gp_cmmb_info;

void cmmb_player_time_dsr(void)
{
	unsigned int buf_status = 0;
	int ret = 0;
	int index = 0;
	
	unsigned int readbyte = 0;
	char* tmp_buf;
	unsigned int cmmb_frame_length;
	unsigned int seek_offset = 0;
	
	if(gp_cmmb_file->file_end)
	{
		ctl_CMMB_file_stop();
		return;
	}
	
	MMD_BIU_Get_Device_Status(IDX_AUD_STATUS, &buf_status);
	while (index < CMMB_BUF_COUNT)
	{
		if(buf_status & (1 << index)) 
		{
			//printf("Load buffer[%d]\n", index);
			tmp_buf = gp_cmmb_file->tmp_buf;
			readbyte = filesys_fileread(gp_cmmb_file->file_handle, tmp_buf, 0xfc08);
			if(readbyte < 0xfc08)
			{
				CMMB_PRINTF("Read file END!!!\n");
				gp_cmmb_file->file_end = 1;
				return;
			}

			if((tmp_buf[0]==tmp_buf[0xfc00])&&(tmp_buf[1]==tmp_buf[0xfc01])&&(tmp_buf[2]==tmp_buf[0xfc02])&&(tmp_buf[3]==tmp_buf[0xfc03]))
			{
				cmmb_frame_length = 0xfc00;
			}
			else if((tmp_buf[0]==tmp_buf[0xd800])&&(tmp_buf[1]==tmp_buf[0xd801])&&(tmp_buf[2]==tmp_buf[0xd802])&&(tmp_buf[3]==tmp_buf[0xd803]))
			{
				cmmb_frame_length = 0xd800;
			}
			else if((tmp_buf[0]==tmp_buf[0x6c00])&&(tmp_buf[1]==tmp_buf[0x6c01])&&(tmp_buf[2]==tmp_buf[0x6c02])&&(tmp_buf[3]==tmp_buf[0x6c03]))
			{
				cmmb_frame_length = 0x6c00;
			}
			else if((tmp_buf[0]==tmp_buf[0x21c0])&&(tmp_buf[1]==tmp_buf[0x21c1])&&(tmp_buf[2]==tmp_buf[0x21c2])&&(tmp_buf[3]==tmp_buf[0x21c3]))
			{
				cmmb_frame_length = 0x21c0;
			}
			else
			{
				CMMB_PRINTF("Frame header err!!!\n");
				cmmb_frame_length = 0;
			}
			
			seek_offset = 0xfc08 - cmmb_frame_length;
			seek_offset = filesys_filetell(gp_cmmb_file->file_handle) - seek_offset;
			filesys_fileseek(gp_cmmb_file->file_handle, seek_offset, F_SEEK_SET);
			
			Vm_WrSram((gp_cmmb_info->core_cmmb_buffer)[index], tmp_buf, cmmb_frame_length);

			
			MMD_BIU_Get_Device_Status(IDX_AUD_STATUS, &buf_status);
			buf_status &= ~(1 << index);
			MMD_BIU_Set_Device_Status(IDX_AUD_STATUS, buf_status);

			//printf("load_cmmb_buf, index=%d, cmmb_frame_length=0x%x\n", index, cmmb_frame_length);
			ret = MMD_CMMB_LOADBLOCK(index, readbyte, 0);
			if(ret)
				CMMB_PRINTF("MMD_FILE_CMMB_LOADBLOCK fail, ret=0x%x\n", ret);			
			break;
		}
		index++;
	}

}

unsigned int ctl_CMMB_init(CMMB_INFO * pcmmb_info, unsigned int play_mode, unsigned int fw_option)
{
	int ret;
#if 1	//	For debug in Core
	ret = MMD_SYS_Mode_Switch(SCENE_CMMB);
#else
	ret = 0;
#endif	
	if(ret)
	{
		CMMB_PRINTF("MMD_SYS_Mode_Switch SCENE_CMMB fail\n");
		return CMMB_CTL_DOWNCORE_ERROR;
	}
	MMD_USER_WaitMICRS(5000);
	MMD_USER_CMMB_GPIO_SELECT(USER_CMMB_RESET_PIN, USER_CMMB_INTR_PIN);
	if(play_mode == CMMB_MODE_FILE_PLAY)
	{
		MMD_USER_Timer_Set(cmmb_player_time_dsr, 20);
		gp_cmmb_info = pcmmb_info;	
	}
	ret = MMD_CMMB_INIT(pcmmb_info, play_mode, fw_option, NULL, NULL);
	if(ret)
	{
		CMMB_PRINTF("MMD_CMMB_INIT  err\n");
		return ret;
	}	
	return CMMB_CTL_NOERR;
}

unsigned int ctl_CMMB_release(unsigned int play_mode)
{
	int ret;
	if(play_mode == CMMB_MODE_FILE_PLAY)
	{
		MMD_USER_Timer_Release();
	}

	ret = MMD_CMMB_QUIT();
	if(ret)
	{
		CMMB_PRINTF("MMD_CMMB_QUIT  fail\n");
		return ret;
	}
	
	return CMMB_CTL_NOERR;
}

unsigned int ctl_CMMB_file_open(CMMB_FILE* pcmmb_file, CMMB_INFO * pcmmb_info)
{
	void* fs_filehandle;
	
	pcmmb_file->tmp_buf = NULL;

	fs_filehandle = (void*)filesys_fileopen((void *)(pcmmb_file->file_name), "r");
	if(!fs_filehandle)
	{
		CMMB_PRINTF("FileSys Open file error\n");
		return CMMB_CTL_FILE_ERROR;
	}
	pcmmb_file->file_handle = fs_filehandle;
	filesys_fileseek(pcmmb_file->file_handle, 0L, F_SEEK_SET);
	filesys_fileseek(pcmmb_file->file_handle, 0L, F_SEEK_END);	
	pcmmb_file->file_len = filesys_filetell(pcmmb_file->file_handle);
	filesys_fileseek(pcmmb_file->file_handle, 0L, F_SEEK_SET);
	pcmmb_file->file_end = 0;
	
	gp_cmmb_file = pcmmb_file;

	pcmmb_file->tmp_buf = (char*)(MMD_Malloc(pcmmb_info->core_cmmb_buflen));
	if(!pcmmb_file->tmp_buf)	
	{
		CMMB_PRINTF("Malloc tmp buffer fail\n");
		return CMMB_CTL_MALLOC_ERROR;
	}	
	
	return CMMB_CTL_NOERR;
}

unsigned int ctl_CMMB_file_close(CMMB_FILE* pcmmb_file)
{
	int ret;
	
	MMD_Free((void*)pcmmb_file->tmp_buf);

	ret = filesys_fileclose(pcmmb_file->file_handle);
	if(ret)
	{
		CMMB_PRINTF("FileSys Close file error\n");
		return CMMB_CTL_FILE_ERROR;
	}
	return CMMB_CTL_NOERR;
}

unsigned int ctl_CMMB_file_play(CMMB_FILE* pcmmb_file, CMMB_INFO* pcmmb_info)
{
	int ret = 0;
	unsigned int buf_status = 0, file_status = 0;
		
	do {
		MMD_BIU_Get_Device_Status(IDX_FILE_STATUS, &file_status);
		MMD_USER_WaitMICRS(50); //ms
	} while (file_status & 0x1); //wait for read2core operate over
	
	MMD_BIU_Get_Device_Status(IDX_AUD_STATUS, &buf_status);
	buf_status |= (0xffffffff>>(32-CMMB_BUF_COUNT));//audio buffer status bit 0~2/4, default 1 empty.
	MMD_BIU_Set_Device_Status(IDX_AUD_STATUS, buf_status);
	CMMB_PRINTF("IDX_AUD_STATUS Init over\n");

	MMD_USER_Timer_Start();
	CMMB_PRINTF("Start cmmb timer...\n");

	ret = MMD_CMMB_PLAY(0, 0);
	if(ret)
	{
		CMMB_PRINTF("MMD_FILE_CMMB_PLAY fail, ret=0x%x\n", ret);
		return ret;
	}
	
	return CMMB_CTL_NOERR;
}

unsigned int ctl_CMMB_file_stop()
{
	int ret = 0;
	MMD_USER_Timer_Stop();
	CMMB_PRINTF("Stop cmmb timer...\n");
	ret = MMD_CMMB_STOP();
	if(ret)
	{
		CMMB_PRINTF("MMD_FILE_CMMB_STOP fail, ret=0x%x\n", ret);
		return ret;
	}
	return CMMB_CTL_NOERR;
}
#endif

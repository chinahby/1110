/*******************************************************************************
*  All rights reserved, Copyright (C) vimicro LIMITED 2008													 
* ------------------------------------------------------------------------------
* [File Name]		 	 : mmd_cmmb_driver.c
* [Description]		 : File API defined for CMMB operation
* [Author]			 : anton liu
* [Date Of Creation]	 : 2008-09-01
* [Platform]			 : Platform independent
* [Note]				 : None
*
* Modification History : 
* ------------------------------------------------------------------------------
* Date				 Author 			 Modifications
* ------------------------------------------------------------------------------
* 2008-09-01		 	anton liu 		 	Creat
*******************************************************************************/
/*******************************************************************************
* This source code has been made available to you by VIMICRO on an
* AS-IS basis. Anyone receiving this source code is licensed under VIMICRO
* copyrights to use it in any way he or she deems fit, including copying it,
* modifying it, compiling it, and redistributing it either with or without
* modifications. Any person who transfers this source code or any derivative
* work must include the VIMICRO copyright notice and this paragraph in
* the transferred software.
*******************************************************************************/


/*
 * INCLUDE FILE                                              
 */
 
#include "mmd_cmmb_driver.h"
#ifdef CMMB_ENABLE	

/*
 * INTERNAL MACRO DEFINITION
 */
//#define MMD_CMMB_DEBUG_INFO
#define MMD_CMMB_DEBUG_ERR
#define MMD_CMMB_DEBUG_NOTE
#define MMD_CMMB_DEBUG_WARNING

#ifdef MMD_CMMB_DEBUG_INFO
    #define PRINTF(WriteVal)    MMD_PRINTF(WriteVal) //general print out
#else
    #define PRINTF(WriteVal)
#endif

#ifdef MMD_CMMB_DEBUG_ERR
    #define PRINTFE(WriteVal)   MMD_PRINTFE(WriteVal) //error
#else
    #define PRINTFE(WriteVal)
#endif

#ifdef MMD_CMMB_DEBUG_NOTE
    #define PRINTFN(WriteVal)   MMD_PRINTFN(WriteVal) //note
#else
    #define PRINTFN(WriteVal)
#endif

#ifdef MMD_CMMB_DEBUG_WARNING
    #define PRINTFW(WriteVal)   MMD_PRINTFW(WriteVal) //warning
#else
    #define PRINTFW(WriteVal)
#endif

/*
 * INTERNAL ENUM DEFINITION
 */


/*
 * GLOBAL VARIABLES DEFINITION                                          
 */
extern volatile MMD_U32 g_Ack_MinCmd;
extern volatile MMD_U32 g_Ack_OP1;
extern volatile MMD_U32 g_Ack_OP2;

#define MAX_ESG_BUF_NUM	2
#define MAX_ESG_BUF_SIZE	(1024*4)

/*************************************************
  Function: 	  
	MMD_Mode_CMMB_Lisr
  Description:	  
	file mode command process call back function, called in LISR
  Input:
	pMsg:	Host to device cmd msg
  Output:		 
	NULL	
  Return:		  
	Error code 
  History:		  
	  <author>			<time>			<desc>
	  Chen Jing'en			07/04/24		create this function  
*************************************************/
MMD_U32 MMD_Mode_CMMB_Lisr(MMD_U32 cmd)
{
	MMD_MSG msg;
	MMD_U08 min;

	// get command and parameter 
	MMD_BIU_Get_D2H_Ack(&msg);
	MMD_ASSERT(msg.CMD==cmd);
	
	min = msg.CMD & CMD_MASK_MIN;
	
	g_Ack_MinCmd = min;
	g_Ack_OP1 = msg.OP1;
	g_Ack_OP2 = msg.OP2;

	switch(min)
	{
		default:
			break;
	}

	return MMD_RET_NOERR;
}

/*************************************************
  Function: 	  
	MMD_Mode_CMMB_Hisr
  Description:	  
	file mode command process call back function, called in HISR
  Input:
	pMsg:	Host to device cmd msg
  Output:		 
	NULL	
  Return:		  
	Error code 
  History:		  
	  <author>			<time>			<desc>
	  Chen Jing'en			07/04/24		create this function  
*************************************************/
MMD_U32 MMD_Mode_CMMB_Hisr(MMD_U32 cmd)
{
	MMD_MSG msg;
	MMD_U08 min;

	// get command and parameter 
	MMD_BIU_Get_D2H_Ack(&msg);
	MMD_ASSERT(msg.CMD==cmd);
	
	min = msg.CMD & CMD_MASK_MIN;

	switch(min)
	{	
		default:
			break;
	}
	
	return MMD_RET_NOERR;
}


tCMCT cmmb_channel_info;

MMD_U32 MMD_CMMB_SearchChannel(CMMB_INFO*  pcmmb_info)
{
	MMD_U32 timeout =10000;
	MMD_MSG msg;
	//MMD_U32 i = 0;
	//MMD_U32 ret;
	
	g_Ack_MinCmd = MIN_RESERVED;
	msg.CMD = MAJ_CMMB|MIN_CMMB_SEARCH;
	msg.OP1 = pcmmb_info->channel_no;
	MMD_BIU_Send_H2D_Cmd(&msg);
		
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
			MMD_USER_WaitMICRS(10);
		else
		{
			Vm_RdSram(g_Ack_OP1, (MMD_S08 *)(&(pcmmb_info->ch_info)), sizeof(tCMCT));
		#if 0
			CMMB_PRINTF("Search_channel, read from 0x%x, size=%d, nMfNum=%d\n", g_Ack_OP1, sizeof(tCMCT), pcmmb_info->ch_info.nMfNum);
			for(i=0; i<pcmmb_info->ch_info.nMfNum; i++)
				CMMB_PRINTF("info=0x%x\n", pcmmb_info->ch_info.tFrameDesc[i].chnl_info);
		#endif
			if(g_Ack_OP2 != 0)
				pcmmb_info->ch_info.nMfNum = 0;
			break;
		}
		timeout--;
	}

	if(!timeout)
	{
		return MMD_CMMB_ERR_ACKTIMEOUT;
	}
	
	return MMD_RET_NOERR;
}


MMD_U32 MMD_CMMB_INIT(CMMB_INFO*  pcmmb_info, MMD_U32 play_mode, MMD_U32 fw_option, MMD_U08* pFW, MMD_U32 fw_len)
{
	MMD_U32 timeout = 20000;
	MMD_MSG msg;
	MMD_U32 fw_addr = 0;

#if !FW_DOWNLOAD_CORE
	if(fw_option == FW_DOWNLOAD_AP)
	{
		fw_addr = MMD_SYS_Malloc(fw_len);
		if(!fw_addr)
		{
			CMMB_PRINTF("MMD_SYS_Malloc fw_addr fail!\n");
			return MMD_CMMB_ERR_MALLOC;
		}
		Vm_WrSram(fw_addr, (MMD_S08*)pFW, fw_len);
	}
#endif

	/* send command to start reading SD sectors */
	g_Ack_MinCmd = MIN_RESERVED;
	msg.CMD = MAJ_CMMB|MIN_CMMB_INIT;
	msg.OP1 = (play_mode << 24) | (fw_option << 16) | (fw_len & 0xffff);
	msg.OP2 = fw_addr;
	MMD_BIU_Send_H2D_Cmd(&msg);
		
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
			MMD_USER_WaitMICRS(10);
		else
		{
			pcmmb_info->core_cmmb_buflen = g_Ack_OP2;
			pcmmb_info->core_cmmb_buffer[0] = g_Ack_OP1;
			pcmmb_info->core_cmmb_buffer[1] = pcmmb_info->core_cmmb_buffer[0] + pcmmb_info->core_cmmb_buflen;
			pcmmb_info->core_cmmb_buffer[2] = pcmmb_info->core_cmmb_buffer[1] + pcmmb_info->core_cmmb_buflen;
			break;
		}
		timeout--;
	}

	if(!timeout)
	{
#if !FW_DOWNLOAD_CORE
	if(fw_option == FW_DOWNLOAD_AP)
	{
		if(fw_addr)	MMD_SYS_Free(fw_addr);
	}
#endif
		return MMD_CMMB_ERR_ACKTIMEOUT;
	}
	
#if !FW_DOWNLOAD_CORE
	if(fw_option == FW_DOWNLOAD_AP)
	{
		if(fw_addr)	MMD_SYS_Free(fw_addr);
	}
#endif	
	return MMD_CMMB_NO_ERROR;
}

MMD_U32 MMD_CMMB_LCD_INIT(CMMB_LCD_INFO* lcd_info)
{
	MMD_U32 timeout = 10000;
	MMD_MSG msg;

	/* send command to start reading SD sectors */
	g_Ack_MinCmd = MIN_RESERVED;
	msg.CMD = MAJ_CMMB|MIN_CMMB_LCDINIT;
	
	msg.OP1 = (lcd_info->size_x << 16) | (lcd_info->size_y);
	msg.OP2 = lcd_info->full_screen;
	MMD_BIU_Send_H2D_Cmd(&msg);
		
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
			MMD_USER_WaitMICRS(10);
		else
		{
			break;
		}
		timeout--;
	}

	if(!timeout)
	{
		return MMD_CMMB_ERR_ACKTIMEOUT;
	}
	return MMD_CMMB_NO_ERROR;

}



MMD_U32 MMD_CMMB_SET_BUF_Empty(MMD_U32 index)
{
#if 1
	MMD_U32 timeout = 4000;
	MMD_MSG msg;
	
	g_Ack_MinCmd = MIN_RESERVED;
	msg.CMD = MAJ_CMMB|MIN_CMMB_ESG_SET_EMPTY;
	msg.OP1 = index;
	MMD_BIU_Send_H2D_Cmd(&msg);
		
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
			MMD_USER_WaitMICRS(10);
		else
		{
			break;
		}
		timeout--;
	}

	if(!timeout)
	{
		return MMD_CMMB_ERR_ACKTIMEOUT;
	}
	
	return MMD_RET_NOERR;
#else
	unsigned int status;
	MMD_CMMB_LOCK_ESGBUF_STATUS(1);

	MMD_BIU_Get_Device_Status(IDX_AUD_STATUS, &status);
	status |= (1 << index);
	MMD_BIU_Set_Device_Status(IDX_AUD_STATUS, status);

	MMD_CMMB_LOCK_ESGBUF_STATUS(0);
	return 0;

#endif
}

MMD_U32 MMD_CMMB_CHECK_BUF_FULL(MMD_U32 index)
{
	MMD_U32 status;
	MMD_BIU_Get_Device_Status(IDX_AUD_STATUS, &status);
	if(status & (1 << index))	//	1 for empty, 0 for full
		return 0;	//	Buffer empty
	else
		return 1;	//	Buffer full
}


MMD_U32 MMD_CMMB_ESG_BUF_INIT(ESG_BUF_INFO* pInfo, ESG_BUF_ADDR* pAddr)
{
	MMD_MSG Msg;
	MMD_U32 timeout=10000;
	MMD_U32 i = 0;

	g_Ack_MinCmd = MIN_RESERVED;
	Msg.CMD = MAJ_CMMB|MIN_CMMB_ESG_BUF_INIT;
	Msg.OP1 = (pInfo->BUF_NUM << 16) | pInfo->BUF_SIZE;
	Msg.OP2 = 0;
	
	MMD_BIU_Send_H2D_Cmd(&Msg);

	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
		{
			MMD_USER_WaitMICRS(10);
		}
		else
		{
			break;
		}
		timeout--;
	}

	if(timeout)
	{
		CMMB_PRINTF("Get ESG_BUF_INIT ACK, g_Ack_OP1=0x%x\n", g_Ack_OP1);
		Vm_RdSram(g_Ack_OP1, (MMD_S08*)pAddr, sizeof(ESG_BUF_ADDR));
		for(i=0; i<pInfo->BUF_NUM; i++)
		{
			pAddr->BUF_ADDR[i] |= 0x40000000;
		}

		return MMD_CMMB_NO_ERROR;
	}	
	else
	{
		return MMD_CMMB_ERR_ACKTIMEOUT;
	}	
	
	return MMD_CMMB_NO_ERROR;
}

MMD_U32 MMD_CMMB_ESG_BUF_RELEASE(void)
{
	MMD_U32 timeout = 2000;
	MMD_MSG msg;

	g_Ack_MinCmd = MIN_RESERVED;
	msg.CMD = MAJ_CMMB|MIN_CMMB_ESG_BUF_RELEASE;
	MMD_BIU_Send_H2D_Cmd(&msg);
		
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
			MMD_USER_WaitMICRS(10);
		else
		{
			break;
		}
		timeout--;
	}

	if(!timeout)
	{
		return MMD_CMMB_ERR_ACKTIMEOUT;
	}
	
	return MMD_CMMB_NO_ERROR;
}

MMD_U32 MMD_CMMB_ESG_GET_BUF(MMD_U32 buf_addr, CMMB_ESG_Buf* cmmb_esg_buf, MMD_S08* esg_data)
{
	Vm_RdSram(buf_addr, (MMD_S08*)cmmb_esg_buf, sizeof(CMMB_ESG_Buf));
	//CMMB_PRINTF("CMMB_ESG_Buf:x%x, %d, %d, %d\n", cmmb_esg_buf->buffer_address, cmmb_esg_buf->buffer_size, cmmb_esg_buf->cursor_read, cmmb_esg_buf->cursor_write);
	Vm_RdSram(cmmb_esg_buf->buffer_address, esg_data, cmmb_esg_buf->data_length);	
	
	return MMD_CMMB_NO_ERROR;	
}

MMD_U32 MMD_CMMB_ESG_DEMUX_OVER(void)
{
	MMD_U32 status;
	
	MMD_BIU_Get_Device_Status(IDX_AUD_STATUS, &status);	

	if((status >> 16)&0x01)	//	End of ESG demux
		return 1;
	else
		return 0;
}

MMD_U32 MMD_CMMB_FIND_ESG_SLOT(CMMB_INFO* cmmb_info, MMD_U32* esg_channel_info)
{
	MMD_U32 i = 0;
	*esg_channel_info = 0;
	for(i=0; i<(cmmb_info->ch_info.nMfNum); i++)
	{
		if(((cmmb_info->ch_info.tFrameDesc[i].chnl_info >> 16) & 0xff) == 0x01)	//	TS1
		{
			*esg_channel_info = cmmb_info->ch_info.tFrameDesc[i].chnl_info;
			*esg_channel_info = ((cmmb_info->channel_no)<<24) | (*esg_channel_info);
			break;
		}		
	}
	if(*esg_channel_info)	
		return MMD_CMMB_NO_ERROR;
	else
		return MMD_CMMB_ERR_SLOT;
		
}


MMD_U32 MMD_CMMB_GetService(MMD_U32 service_ch_info, CMMB_ESG_Buf_Info* esg_buf_info)
{
	MMD_U32 timeout = 2000;
	MMD_MSG msg;

	g_Ack_MinCmd = MIN_RESERVED;
	msg.CMD = MAJ_CMMB|MIN_CMMB_GET_SERVICE;
	msg.OP1 = service_ch_info;
	msg.OP2 = (MMD_U32)esg_buf_info;
	MMD_BIU_Send_H2D_Cmd(&msg);
		
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
			MMD_USER_WaitMICRS(10);
		else
		{
			break;
		}
		timeout--;
	}

	if(!timeout)
	{
		return MMD_CMMB_ERR_ACKTIMEOUT;
	}
	
	return MMD_CMMB_NO_ERROR;

}

MMD_U32 MMD_CMMB_GetSchedule(MMD_U32 service_ch_info, CMMB_ESG_Buf_Info* esg_buf_info)
{
	MMD_U32 timeout = 2000;
	MMD_MSG msg;

	g_Ack_MinCmd = MIN_RESERVED;
	msg.CMD = MAJ_CMMB|MIN_CMMB_GET_SCHEDULE;
	msg.OP1 = service_ch_info;
	msg.OP2 = (MMD_U32)esg_buf_info;
	MMD_BIU_Send_H2D_Cmd(&msg);
		
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
			MMD_USER_WaitMICRS(10);
		else
		{
			break;
		}
		timeout--;
	}

	if(!timeout)
	{
		return MMD_CMMB_ERR_ACKTIMEOUT;
	}
	
	return MMD_CMMB_NO_ERROR;

}

MMD_U32 MMD_CMMB_PLAY(MMD_U32 ch_info, MMD_U32 record_option)
{
	MMD_U32 timeout = 2000;
	MMD_MSG msg;

	g_Ack_MinCmd = MIN_RESERVED;
	msg.CMD = MAJ_CMMB|MIN_CMMB_PLAY;
	msg.OP1 = ch_info;
	msg.OP2 = record_option;
	MMD_BIU_Send_H2D_Cmd(&msg);
		
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
			MMD_USER_WaitMICRS(10);
		else
		{
			break;
		}
		timeout--;
	}

	if(!timeout)
	{
		return MMD_CMMB_ERR_ACKTIMEOUT;
	}
	
	return MMD_CMMB_NO_ERROR;
}

MMD_U32 MMD_CMMB_STOP(void)
{
	MMD_U32 timeout = 8000;
	MMD_MSG msg;

	g_Ack_MinCmd = MIN_RESERVED;
	msg.CMD = MAJ_CMMB|MIN_CMMB_STOP;
	MMD_BIU_Send_H2D_Cmd(&msg);
		
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
			MMD_USER_WaitMICRS(10);
		else
		{
			break;
		}
		timeout--;
	}

	if(!timeout)
	{
		return MMD_CMMB_ERR_ACKTIMEOUT;
	}
	
	return MMD_CMMB_NO_ERROR;
}

MMD_U32 MMD_CMMB_QUIT(void)
{
	MMD_U32 timeout = 10000;
	MMD_MSG msg;

	g_Ack_MinCmd = MIN_RESERVED;
	msg.CMD = MAJ_CMMB|MIN_CMMB_QUIT;
	MMD_BIU_Send_H2D_Cmd(&msg);
		
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
			MMD_USER_WaitMICRS(10);
		else
		{
			break;
		}
		timeout--;
	}

	if(!timeout)
	{
		return MMD_CMMB_ERR_ACKTIMEOUT;
	}
	
	return MMD_CMMB_NO_ERROR;
}

MMD_U32 MMD_ESG_STOP(void)
{
	MMD_U32 timeout = 2000;
	MMD_MSG msg;

	g_Ack_MinCmd = MIN_RESERVED;
	msg.CMD = MAJ_CMMB|MIN_CMMB_ESG_STOP;
	MMD_BIU_Send_H2D_Cmd(&msg);
		
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
			MMD_USER_WaitMICRS(10);
		else
		{
			break;
		}
		timeout--;
	}

	if(!timeout)
	{
		return MMD_CMMB_ERR_ACKTIMEOUT;
	}
	
	return MMD_CMMB_NO_ERROR;
}


MMD_U32 MMD_CMMB_GET_SYS_STATUS(CMMB_SysStatus* cmmb_status)
{
	MMD_U32 timeout = 10000;
	MMD_MSG msg;
	MMD_U32 addr = 0;

	MMD_MemSet((void*)cmmb_status, 0, sizeof(CMMB_SysStatus));
	addr = MMD_SYS_Malloc(sizeof(CMMB_SysStatus));
	if(!addr) 
	{
		CMMB_PRINTF(("MMD_SYS_Malloc CMMB_SysStatus fail\n"));
		return MMD_CMMB_ERR_MALLOC;
	}
	
	g_Ack_MinCmd = MIN_RESERVED;
	msg.CMD = MAJ_CMMB|MIN_CMMB_GET_SYS_STATUS;
	msg.OP1 = addr;
	MMD_BIU_Send_H2D_Cmd(&msg);
		
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
			MMD_USER_WaitMICRS(10);
		else
		{
			break;
		}
		timeout--;
	}

	if(!timeout)
	{
		//CMMB_PRINTF("MMD_CMMB_GET_SYS_STATUS timeout!, g_Ack_MinCmd=0x%x\n", g_Ack_MinCmd);
		if(addr)	MMD_SYS_Free(addr);
		return MMD_CMMB_ERR_ACKTIMEOUT;
	}
	
	if(g_Ack_OP1 == 0)
	{
		//CMMB_PRINTF("MMD_CMMB_GET_SYS_STATUS succeed!, size=%d\n", sizeof(CMMB_SysStatus));
		Vm_RdSram((MMD_U32)addr, (MMD_S08*)cmmb_status, sizeof(CMMB_SysStatus));	
	}
	else
	{
		CMMB_PRINTF(("CMMB_GET_SYS_STATUS fail\n"));
		return MMD_CMMB_ERR_COREFAIL;
	}	
	
	if(addr)	MMD_SYS_Free(addr);
	return MMD_CMMB_NO_ERROR;
}


MMD_U32 MMD_CMMB_SET_VOLUME(MMD_U32 volume)
{
	MMD_U32 timeout = 2000;
	MMD_MSG msg;

	g_Ack_MinCmd = MIN_RESERVED;
	msg.CMD = MAJ_CMMB|MIN_CMMB_SET_VOLUME;
	msg.OP1 = volume;	//	In range [0, 30]
	MMD_BIU_Send_H2D_Cmd(&msg);
		
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
			MMD_USER_WaitMICRS(10);
		else
		{
			break;
		}
		timeout--;
	}

	if(!timeout)
	{
		return MMD_CMMB_ERR_ACKTIMEOUT;
	}
	
	return MMD_CMMB_NO_ERROR;
}

MMD_U32 MMD_CMMB_GPIO_SELECT(MMD_U32 reset_pin, MMD_U32 interrupt_pin)
{
	MMD_U32 timeout = 2000;
	MMD_MSG msg;

	g_Ack_MinCmd = MIN_RESERVED;
	msg.CMD = MAJ_CMMB|MIN_CMMB_GPIO_SELECT;
	msg.OP1 = reset_pin;
	msg.OP2 = interrupt_pin;
	MMD_BIU_Send_H2D_Cmd(&msg);
		
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
			MMD_USER_WaitMICRS(10);
		else
		{
			break;
		}
		timeout--;
	}

	if(!timeout)
	{
		return MMD_CMMB_ERR_ACKTIMEOUT;
	}
	
	return MMD_CMMB_NO_ERROR;
}


MMD_U32 MMD_CMMB_LOADBLOCK(unsigned int index, unsigned int data_len, unsigned int cur_read)
{
	MMD_U32 timeout = 10000;
	MMD_MSG msg;

	g_Ack_MinCmd = MIN_RESERVED;
	msg.CMD = MAJ_CMMB|MIN_CMMB_LOADBLOCK;
	msg.OP1 = (index << 16) | (cur_read & 0x0000ffff);
	msg.OP2 = data_len;
	
	MMD_BIU_Send_H2D_Cmd(&msg);
		
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
			MMD_USER_WaitMICRS(10);
		else
		{
			break;
		}
		timeout--;
	}

	if(!timeout)
	{
		return MMD_CMMB_ERR_ACKTIMEOUT;
	}
	
	return MMD_CMMB_NO_ERROR;
}

MMD_U32 MMD_CMMB_RECORD_BUFINIT(CMMB_INFO* cmmb_info, CMMB_MFS_Buf_Info* mfs_buf_info)
{
	MMD_U32 i = 0;
	mfs_buf_info->buffer_count = CMMB_BUF_COUNT;
	mfs_buf_info->buffer_read = mfs_buf_info->buffer_write = 0;
	for(i=0; i<mfs_buf_info->buffer_count; i++)
	{
		mfs_buf_info->mfs_buffer[i].buffer_address = cmmb_info->core_cmmb_buffer[i];
		mfs_buf_info->mfs_buffer[i].buffer_size = cmmb_info->core_cmmb_buflen;
		mfs_buf_info->mfs_buffer[i].cursor_read = 0;
		mfs_buf_info->mfs_buffer[i].cursor_write = 0;
		mfs_buf_info->mfs_buffer[i].data_length = 0;
		mfs_buf_info->mfs_buffer[i].is_over = 0;
	}
	
	return MMD_CMMB_NO_ERROR;
}


MMD_U32 MMD_CMMB_RECORD_START(MMD_U32 record_mode)
{
	MMD_U32 timeout = 2000;
	MMD_MSG msg;

	g_Ack_MinCmd = MIN_RESERVED;
	msg.CMD = MAJ_CMMB|MIN_CMMB_RECORD_START;
	msg.OP1 = record_mode;
	msg.OP2 = 0;
	
	MMD_BIU_Send_H2D_Cmd(&msg);
		
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
			MMD_USER_WaitMICRS(10);
		else
		{
			break;
		}
		timeout--;
	}

	if(!timeout)
	{
		return MMD_CMMB_ERR_ACKTIMEOUT;
	}
	
	return MMD_CMMB_NO_ERROR;	
}

MMD_U32 MMD_CMMB_RECORD_STOP()
{
	MMD_U32 timeout = 2000;
	MMD_MSG msg;

	g_Ack_MinCmd = MIN_RESERVED;
	msg.CMD = MAJ_CMMB|MIN_CMMB_RECORD_STOP;
	msg.OP1 = 0;
	msg.OP2 = 0;
	
	MMD_BIU_Send_H2D_Cmd(&msg);
		
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
			MMD_USER_WaitMICRS(10);
		else
		{
			break;
		}
		timeout--;
	}

	if(!timeout)
	{
		return MMD_CMMB_ERR_ACKTIMEOUT;
	}
	
	return MMD_CMMB_NO_ERROR;	
}

#endif

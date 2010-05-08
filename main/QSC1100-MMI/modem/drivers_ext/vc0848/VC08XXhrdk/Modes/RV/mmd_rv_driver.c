/*******************************************************************************
*  All rights reserved, Copyright (C) vimicro LIMITED 2008													 
* ------------------------------------------------------------------------------
* [File Name]		 	 : mmd_rv_driver.c
* [Description]		 : File API defined for RV operation
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
#ifdef RV_ENABLE

/*
 * INCLUDE FILE                                              
 */
#include "mmd_rv_driver.h"
#include	"media_player.h"


/*
 * INTERNAL MACRO DEFINITION
 */

#define MMD_RV_DEBUG_INFO
#define MMD_RV_DEBUG_ERR
#define MMD_RV_DEBUG_NOTE
#define MMD_RV_DEBUG_WARNING

#ifdef MMD_RV_DEBUG_INFO
    #define PRINTF(WriteVal)    MMD_PRINTF(WriteVal) //general print out
#else
    #define PRINTF(WriteVal)
#endif

#ifdef MMD_RV_DEBUG_ERR
    #define PRINTFE(WriteVal)   MMD_PRINTFE(WriteVal) //error
#else
    #define PRINTFE(WriteVal)
#endif

#ifdef MMD_RV_DEBUG_NOTE
    #define PRINTFN(WriteVal)   MMD_PRINTFN(WriteVal) //note
#else
    #define PRINTFN(WriteVal)
#endif

#ifdef MMD_RV_DEBUG_WARNING
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


/*************************************************
  Function: 	  
	Mode_RV_Lisr
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
MMD_U32 Mode_RV_Lisr(MMD_U32 cmd)
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
volatile MMD_U32 g_File_Task_Flag, g_RV_File_End;  //0: ready, others busy
MMD_RV_BUF_INFO g_RV_Stream_Buf;
extern VIM_MEDIA_PLAYER_FILE* g_PlayFileHandle;
extern fs_callbacks*		g_pFilefunc;
MMD_RV_TIMER_STATE g_Timer;
extern TIMER_STATE emMMDTimerState;
void RV_File_Task_Func(void* para)
{
	MMD_MSG* pMsg=(MMD_MSG*)para;
	MMD_U08 min;
	MMD_U32 buf_idx,len;
	
	min = pMsg->CMD & CMD_MASK_MIN;
	switch(min)
	{
	case MMD_RV_FS_INIT:
		MMD_USER_FS_Init();
		g_File_Task_Flag= 0;
		g_RV_File_End=0;
		break;
		
	case MMD_RV_FILE_READ:
		if(g_RV_File_End!=0)
			break;
		
		buf_idx = pMsg->OP1;
		len = g_pFilefunc->read_func((void*)g_PlayFileHandle->fs_filehandle, (MMD_S08*)g_RV_Stream_Buf.buffer[buf_idx].buffer_address,RV_BUF_SIZE);
		g_RV_Stream_Buf.buffer[buf_idx].data_length= len ;
		g_RV_Stream_Buf.buffer[buf_idx].cursor_read = 0;
		g_RV_Stream_Buf.buffer[buf_idx].cursor_write = len;
		if(len<RV_BUF_SIZE)
		{
			g_RV_Stream_Buf.buffer[buf_idx].is_over = 1;
			g_RV_File_End=1;
		}	

		break;
		
	case MMD_RV_FILE_CLOSE:
		break;

	default:
		break;
	}
}
void MMD_RV_TIMER_Init(void)
{
	MMD_U32 i;
	for(i=0;i<RV_TIMER_NUM;i++)
	{
		g_Timer.state[i] = 0;
		g_Timer.pISR[i] = 0;
		g_Timer.count[i] = 0;
		g_Timer.interval[i] = 0;
	}
	PRINTF(("RVTimer init.\n"));
}
void MMD_RV_TIMER_Isr(void)
{
	MMD_U32 i;
	//void * func_isr;
	for(i = 0;i<RV_TIMER_NUM;i++)
	{
		if(g_Timer.state[i] == RV_TIMER_START)
		{
			g_Timer.count[i] += RV_TIMER_INV;
			if(g_Timer.interval[i]<=g_Timer.count[i])
			{
				(*g_Timer.pISR[i])();
				g_Timer.count[i] = 0;
			}
		}
	}
}
void MMD_RV_TIMER_SET(MMD_U32 Timer,MMD_RV_TIMER Timercallback,MMD_U32 Interval)
{
	MMD_U32 i,count;
	if(Timer > RV_TIMER_NUM)
	{
		PRINTF(("RVTimer set number error!(set)\n"));
		return;
	}
	if((g_Timer.state[Timer] != 0)||(Interval<RV_TIMER_INV))
	{
		PRINTF(("RVTimer state or interval error!(set)\n"));
		return;
	}
	count = 0;
	for(i=0;i<RV_TIMER_NUM;i++)
	{
		if(g_Timer.state[i] == 0)
			count += 1;
	}
	if(RV_TIMER_NUM == count)
	{
		PRINTF(("RVTimer setl.\n"));
		MMD_USER_Timer_Set(MMD_RV_TIMER_Isr,RV_TIMER_INV);
	}

	g_Timer.pISR[Timer] = Timercallback;
	g_Timer.state[Timer] = RV_TIMER_SET;
	g_Timer.interval[Timer] = Interval;
	g_Timer.count[Timer] = 0;
	
}
void MMD_RV_TIMER_START(MMD_U32 Timer)
{
	MMD_U32 i;
	if(Timer > RV_TIMER_NUM)
	{
		PRINTF(("RVTimer start number error!(start)\n"));
		return;
	}
	if((g_Timer.state[Timer] == RV_TIMER_START)||(g_Timer.state[Timer] == 0))
	{
		PRINTF(("RVTimer%x state error!(start)\n",Timer));
		return;
	}
	for(i=0;i<RV_TIMER_NUM;i++)
	{
		if(g_Timer.state[i] == RV_TIMER_START)
		{
			g_Timer.state[Timer] = RV_TIMER_START;
			return;
		}
	}
	g_Timer.state[Timer] = RV_TIMER_START;
	emMMDTimerState = TIMER_STARTED;//add by ahy for chenge overlay update lcd mode
	MMD_USER_Timer_Start();
	PRINTF(("RVTimer start.\n"));
}
void MMD_RV_TIMER_STOP(MMD_U32 Timer)
{
	MMD_U32 i;
	if(Timer > RV_TIMER_NUM)
	{
		PRINTF(("RVTimer stop number error!(stop)\n"));
		return;
	}
	if(g_Timer.state[Timer] != RV_TIMER_START)
	{
		PRINTF(("RVTimer%x state error!(stop)\n",Timer));
		return;
	}
	g_Timer.state[Timer] = RV_TIMER_STOP;
	g_Timer.count[Timer] = 0;
	for(i=0;i<RV_TIMER_NUM;i++)
	{
		if(g_Timer.state[i] == RV_TIMER_START)
			return;
	}
	emMMDTimerState = NO_TIMER_START;//add by ahy for chenge overlay update lcd mode
	PRINTF(("RVTimer stop.\n"));
	MMD_USER_Timer_Stop();
}
void MMD_RV_TIMER_RELEASE(MMD_U32 Timer)
{
	MMD_U32 i;
	if(Timer > RV_TIMER_NUM)
	{
		PRINTF(("RVTimer stop number error!\n"));
		return;
	}
	if((g_Timer.state[Timer] == 0)||(g_Timer.state[Timer] == RV_TIMER_START))
	{
		PRINTF(("RVTimer%x state error!(release)\n",Timer));
		return;
	}
	g_Timer.state[Timer] = 0;
	g_Timer.pISR[Timer] = 0;
	g_Timer.count[Timer] = 0;
	g_Timer.interval[Timer] = 0;
	for(i=0;i<RV_TIMER_NUM;i++)
	{
		if(g_Timer.state[Timer] != 0)
			return;
	}
	PRINTF(("RVTimer release.\n"));
	MMD_USER_Timer_Stop();
	MMD_USER_Timer_Release();
}
void MMD_RV_TIMER_RELEASEALL(void)
{
	MMD_U32 i,count;
	for(i=0;i<RV_TIMER_NUM;i++)
	{
		if(g_Timer.state[i] != 0)
		{
			g_Timer.state[i] = 0;
			g_Timer.pISR[i] = 0;
			g_Timer.count[i] = 0;
			g_Timer.interval[i] = 0;
			count += 1;
		}
	}
	if(count == RV_TIMER_NUM)
	{
		PRINTF(("RVTimer release all.\n"));
		MMD_USER_Timer_Stop();
		MMD_USER_Timer_Release();
	}
}
MMD_U32 MMD_RV_Init(void)
{
	MMD_MSG msg;
	MMD_U32 timeout=5000, reg,ret;
//	MMD_SYS_Reset_8XX();
	
	MMD_BIU_Init(1);

//	MMD_LCD_Init_Bypass();
	MMD_USER_SetChipMode(MMD_CHIP_MODE_NORMAL);
       MMD_USER_WakeupInit();

	MMD_SYS_Sdram_Init();

	MMD_SYS_Set_TriggerMode();

 	MMD_SYS_Scene_Info_Init();
	
	ret = MMD_SYS_Mode_Switch(SCENE_DEFAULT);
	if(ret!=MMD_RET_NOERR)
	{
		PRINTFE(("Download code error, ret=0x%08X!\n",ret));
		return ret;	
	}		
	ret = MMD_SYS_Codec_Switch(CODEC_DEFAULT);
	if(ret!=MMD_RET_NOERR)
	{
		PRINTFE(("Codec code switch error, ret=0x%08X!\n",ret));
		return ret;	
	}		

	MMD_MediaEngine_Init();
#if 1 	
	MMD_SYS_Mode_Switch(SCENE_RV);
#else
	MMD_SYS_Mode_Switch(SCENE_RV_OVERLAY);
	MMD_SYS_Codec_Switch(RV_CODEC_RV8);
#endif	
	RV_LCD_Set(MMD_RV_ROTATE_90,LCD_WIDTH, LCD_HEIGHT);

	// Set CPU_CS2 as GPIO
	reg = Vm_GetReg(0x6017030C);
	reg |=(1<<21);
	Vm_SetReg(0x6017030C, reg);
	PRINTFE(("0x6017030C=0x%x\n", reg));
	reg = Vm_GetReg(0x60170318);
	reg &=~(1<<21);
	Vm_SetReg(0x60170318, reg);
	PRINTFE(("0x60170318=0x%x\n", reg));
	
	MMD_USER_Task_Create(RV_File_Task_Func);

	// Init baseband file system
	g_File_Task_Flag = MMD_RV_FS_INIT;
	msg.CMD = MMD_RV_FS_INIT;
	msg.OP1 = msg.OP2 = 0;
	MMD_USER_Msg_Send(&msg);
	MMD_RV_TIMER_Init();//
	while(timeout)
	{
		if(g_File_Task_Flag !=0)
			MMD_Check_Irq_Staus_And_Wait(1);
		else
			break;
		timeout--;
	}
	PRINTF(("timeout=%d\n", timeout));
	if(timeout==0)
	{
		return MMD_RET_TIMEOUT;
	}	

	return MMD_RET_NOERR;
}

MMD_U32 MMD_RV_Quit(void)
{
	// Init baseband file system
	MMD_USER_Task_Release();
	RV_LCD_Set(MMD_RV_ROTATE_0,LCD_WIDTH, LCD_HEIGHT);
	MMD_SYS_Mode_Switch(SCENE_MEDIAPLAY);
	MMD_SYS_Codec_Switch(CODEC_PCM);
	MMD_RV_TIMER_RELEASEALL();
#if 0
	MMD_LCD_Init_Bypass();
	MMD_USER_SetChipMode(MMD_CHIP_MODE_NORMAL);
	//External SDRAM initialization
	MMD_SYS_Sdram_Init();
//	MMD_SYS_Mode_Switch(SCENE_MEDIAPLAY);
	MMD_SYS_ReDownCore(SCENE_MEDIAPLAY, 1);

	MMD_SYS_Codec_Switch(CODEC_PCM);
#endif	
	return MMD_RET_NOERR;
}

MMD_U32 RV_Mem_Poll_Create(void)
{
	MMD_MSG Msg;
	MMD_U32 timeout=CMD_TIME_OUT_LIMIT;
	
	g_Ack_MinCmd = MIN_RESERVED;
	
	Msg.CMD=MAJ_RV|MIN_RV_CREATE_MEM_POOL;
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
	
	if(timeout==0)
	{
		return MMD_RET_TIMEOUT;
	}	
	
	return MMD_RET_NOERR;

}

MMD_U32 RV_Mem_Poll_Delete(void)
{
	MMD_MSG Msg;
	MMD_U32 timeout=CMD_TIME_OUT_LIMIT;
	
	g_Ack_MinCmd = MIN_RESERVED;
	
	Msg.CMD=MAJ_RV|MIN_RV_DELETE_MEM_POOL;
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
	
	if(timeout==0)
	{
		return MMD_RET_TIMEOUT;
	}	
	
	return MMD_RET_NOERR;

}

MMD_U32 RV_Fifo_Init(MMD_U32 size, MMD_U32* addr)
{
	MMD_MSG Msg;
	MMD_U32 timeout=CMD_TIME_OUT_LIMIT;
	
	g_Ack_MinCmd = MIN_RESERVED;
	
	Msg.CMD=MAJ_RV|MIN_RV_FIFO_INIT;
	Msg.OP1=size;
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
	
	if(timeout==0)
	{
		*addr = NULL;
		return MMD_RET_TIMEOUT;
	}	
	
	*addr = g_Ack_OP1;
	return MMD_RET_NOERR;

}

MMD_U32 RV_Fifo_Release(MMD_U32 addr)
{
	MMD_MSG Msg;
	MMD_U32 timeout=CMD_TIME_OUT_LIMIT;
	
	g_Ack_MinCmd = MIN_RESERVED;
	
	Msg.CMD=MAJ_RV|MIN_RV_FIFO_RELEASE;
	Msg.OP1=addr;
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
	
	if(timeout==0)
	{
		return MMD_RET_TIMEOUT;
	}	
	
	return MMD_RET_NOERR;

}

MMD_U32 RV_Fifo_Write_Notify(MMD_U32 addr, MMD_U32 len)
{
	MMD_MSG Msg;
	MMD_U32 timeout=CMD_TIME_OUT_LIMIT;
	
	g_Ack_MinCmd = MIN_RESERVED;
	
	Msg.CMD=MAJ_RV|MIN_RV_FIFO_WRITE;
	Msg.OP1=addr;
	Msg.OP2=len;
	MMD_BIU_Send_H2D_Cmd(&Msg);
	
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
			MMD_Check_Irq_Staus_And_Wait(1);
		else
			break;
		timeout--;
	}
	
	if(timeout==0)
	{
		return MMD_RET_TIMEOUT;
	}	
	
	return MMD_RET_NOERR;
}

MMD_U32 RV_Fifo_Get_Empty(MMD_U32* addr, MMD_U32* len)
{
	MMD_BIU_Get_Device_Status(IDX_VID_STATUS, addr);
	MMD_BIU_Get_Device_Status(IDX_AUD_STATUS, len);
	return MMD_RET_NOERR;
}

MMD_U32 RV_File_Open(MMD_U32 filelen, MMD_U32* handle,MMD_U32 *indexoffset)
{
	MMD_MSG Msg;
	MMD_U32 timeout=CMD_TIME_OUT_LIMIT;
	
	g_Ack_MinCmd = MIN_RESERVED;
	
	Msg.CMD=MAJ_RV|MIN_RV_FILE_OPEN;
	Msg.OP1=filelen;
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
	
	if(timeout==0)
	{
		*handle = NULL;
		*indexoffset = 0;
		return MMD_RET_TIMEOUT;
	}	
	
	*indexoffset = g_Ack_OP1;
	*handle = g_Ack_OP2;
	return MMD_RET_NOERR;
}

MMD_U32 RV_File_Play(MMD_U32 handle)
{
	MMD_MSG Msg;
	MMD_U32 timeout=CMD_TIME_OUT_LIMIT;
	
	g_Ack_MinCmd = MIN_RESERVED;
	
	Msg.CMD=MAJ_RV|MIN_RV_FILE_PLAY;
	Msg.OP1=handle;
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
	
	if(timeout==0)
	{
		return MMD_RET_TIMEOUT;
	}	
	
	return MMD_RET_NOERR;
}

MMD_U32 RV_File_Seek(MMD_U32 handle, MMD_U32 pos, MMD_U32* realpos)
{
	MMD_MSG Msg;
	MMD_U32 timeout=CMD_TIME_OUT_LIMIT;
	
	g_Ack_MinCmd = MIN_RESERVED;
	
	Msg.CMD=MAJ_RV|MIN_RV_FILE_PLAY;
	Msg.OP1=handle;
	Msg.OP2=pos;
	MMD_BIU_Send_H2D_Cmd(&Msg);
	
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
			MMD_Check_Irq_Staus_And_Wait(1);
		else
			break;
		timeout--;
	}
	
	if(timeout==0)
	{
		return MMD_RET_TIMEOUT;
	}	
	
	*realpos= g_Ack_OP2;
	return MMD_RET_NOERR;
}
MMD_U32 RV_File_GetIndex(MMD_U32 handle,MMD_U32 *indexoffset)//seek step 1
{
	MMD_MSG Msg;
	MMD_U32 timeout=CMD_TIME_OUT_LIMIT;
	
	g_Ack_MinCmd = MIN_RESERVED;
	
	Msg.CMD=MAJ_RV|MIN_RV_FILE_GETINDEX;
	Msg.OP1=handle;
	MMD_BIU_Send_H2D_Cmd(&Msg);
	
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
			MMD_Check_Irq_Staus_And_Wait(1);
		else
			break;
		timeout--;
	}
	
	if(timeout==0)
	{
		return MMD_RET_TIMEOUT;
	}	
	
	*indexoffset= g_Ack_OP1;
	return MMD_RET_NOERR;
}

MMD_U32 RV_File_GetOffset(MMD_U32 handle, MMD_U32 pos, MMD_U32* fileoffset)//seek step 2
{
	MMD_MSG Msg;
	MMD_U32 timeout=CMD_TIME_OUT_LIMIT;
	
	g_Ack_MinCmd = MIN_RESERVED;
	
	Msg.CMD=MAJ_RV|MIN_RV_FILE_GETOFFSET;
	Msg.OP1=handle;
	Msg.OP2=pos;
	MMD_BIU_Send_H2D_Cmd(&Msg);
	
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
			MMD_Check_Irq_Staus_And_Wait(1);
		else
			break;
		timeout--;
	}
	
	if(timeout==0)
	{
		return MMD_RET_TIMEOUT;
	}	
	
	*fileoffset= g_Ack_OP2;
	return MMD_RET_NOERR;
}

MMD_U32 RV_File_Pause(MMD_U32 handle)
{
	MMD_MSG Msg;
	MMD_U32 timeout=CMD_TIME_OUT_LIMIT;
	
	g_Ack_MinCmd = MIN_RESERVED;
	
	Msg.CMD=MAJ_RV|MIN_RV_FILE_PAUSE;
	Msg.OP1=handle;
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
	
	if(timeout==0)
	{
		return MMD_RET_TIMEOUT;
	}	
	
	return MMD_RET_NOERR;
}

MMD_U32 RV_File_Resume(MMD_U32 handle)
{
	MMD_MSG Msg;
	MMD_U32 timeout=CMD_TIME_OUT_LIMIT;
	
	g_Ack_MinCmd = MIN_RESERVED;
	
	Msg.CMD=MAJ_RV|MIN_RV_FILE_RESUME;
	Msg.OP1=handle;
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
	
	if(timeout==0)
	{
		return MMD_RET_TIMEOUT;
	}	
	
	return MMD_RET_NOERR;
}

MMD_U32 RV_File_Stop(MMD_U32 handle)
{
	MMD_MSG Msg;
	MMD_U32 timeout=CMD_TIME_OUT_LIMIT;
	
	g_Ack_MinCmd = MIN_RESERVED;
	
	Msg.CMD=MAJ_RV|MIN_RV_FILE_STOP;
	Msg.OP1=handle;
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
	
	if(timeout==0)
	{
		return MMD_RET_TIMEOUT;
	}	
	
	return MMD_RET_NOERR;
}

MMD_U32 RV_File_Close(MMD_U32 handle)
{
	MMD_MSG Msg;
	MMD_U32 timeout=CMD_TIME_OUT_LIMIT;
	
	g_Ack_MinCmd = MIN_RESERVED;
	
	Msg.CMD=MAJ_RV|MIN_RV_FILE_CLOSE;
	Msg.OP1=handle;
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
	
	if(timeout==0)
	{
		return MMD_RET_TIMEOUT;
	}	
	
	return MMD_RET_NOERR;
}

MMD_U32 RV_File_GetTotalTime(MMD_U32 handle,MMD_U32 * totaltime)//add by aihouyin
{
	MMD_MSG Msg;
	MMD_U32 timeout=CMD_TIME_OUT_LIMIT;
	
	g_Ack_MinCmd = MIN_RESERVED;
	
	Msg.CMD=MAJ_RV|MIN_RV_FILE_TOTALTIME;
	Msg.OP1=handle;
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
	
	if(timeout==0)
	{
		return MMD_RET_TIMEOUT;
	}	
	*totaltime = g_Ack_OP1;
//	printf("total time is 0x%x\n",g_Ack_OP1);
	return MMD_RET_NOERR;
}

MMD_U32 RV_File_GetProperty(MMD_U32 handle,  MMD_U32 *propertyaddr)
{
	MMD_MSG Msg;
	MMD_U32 timeout=CMD_TIME_OUT_LIMIT;
	
	g_Ack_MinCmd = MIN_RESERVED;
	
	Msg.CMD=MAJ_RV|MIN_RV_FILE_PROPERTY;
	Msg.OP1=handle;
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
	
	if(timeout==0)
	{
		return MMD_RET_TIMEOUT;
	}	
	*propertyaddr = g_Ack_OP1;
	return MMD_RET_NOERR;
}
MMD_U32 RV_File_GetPlayTime(MMD_U32 handle,MMD_U32 * playtime)//add by aihouyin end
{
#if 1
	MMD_BIU_Get_Device_Status(IDX_PT_STATUS,playtime);
//	printf("play time is 0x%x\n",*playtime);
#else
	MMD_MSG Msg;
	MMD_U32 timeout=CMD_TIME_OUT_LIMIT;
	
	g_Ack_MinCmd = MIN_RESERVED;
	
	Msg.CMD=MAJ_RV|MIN_RV_FILE_PLAYTIME;
	Msg.OP1=handle;
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
	
	if(timeout==0)
	{
		return MMD_RET_TIMEOUT;
	}	
	*playtime = g_Ack_OP1;
#endif
	return MMD_RET_NOERR;
}

MMD_U32 RV_LCD_Set(MMD_U32 degree, MMD_U32 width, MMD_U32 height)
{
	MMD_MSG Msg;
	MMD_U32 timeout=CMD_TIME_OUT_LIMIT;

	// Call user rotation function
	//MMD_USER_LCD_Rotation(degree);
	CMMB_MMD_LCDRotate(degree);
	g_Ack_MinCmd = MIN_RESERVED;
	
	Msg.CMD=MAJ_RV|MIN_RV_LCD_SET_SIZE;
	
	if((degree==MMD_RV_ROTATE_0)|(degree==MMD_RV_ROTATE_180))
	{
		Msg.OP1=width;
		Msg.OP2=height;
	}
	else if((degree==MMD_RV_ROTATE_90)|(degree==MMD_RV_ROTATE_270))
	{
		Msg.OP1=height;
		Msg.OP2=width;
	}
		
	MMD_BIU_Send_H2D_Cmd(&Msg);
	
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
			MMD_Check_Irq_Staus_And_Wait(1);
		else
			break;
		timeout--;
	}
	
	if(timeout==0)
	{
		return MMD_RET_TIMEOUT;
	}	
	
	return MMD_RET_NOERR;
}
extern void RVERRCallBack(void);
MMD_U32 MMD_RV_File_Open(PMEDIA_FILE *plyfile, MMD_PLYFILE*pfile)
{
	MMD_U32  ret;
	
	*pfile = 0;	
	ret = media_rv_file_open((MMD_U32)plyfile, pfile);
	PRINTF(("MMD_RV_File_Open=0x%x\n, filehandle=0x%x\n",ret,(unsigned int)pfile));
	return ret;
}

MMD_U32  MMD_RV_File_Play(MMD_PLYFILE pfile, PLYCallback Callback)
{
	MMD_U32 ret;

	ret =media_rv_file_play((MMD_U32)pfile, (MMD_U32)Callback);
	if(ret != 0)
		RVERRCallBack();
	return ret;

}

MMD_U32 MMD_RV_File_Seek(MMD_PLYFILE pfile, MMD_U32 pos, MMD_U32 *accesspos)
{
	MMD_U32 ret;

	ret =media_rv_file_seek((MMD_U32)pfile, pos, accesspos);
	if(ret != 0)
		RVERRCallBack();
	return ret;
}

MMD_U32  MMD_RV_File_Pause(MMD_PLYFILE pfile)
{
	MMD_U32 ret;

	ret =media_rv_file_pause((MMD_U32)pfile);
	if(ret != 0)
		RVERRCallBack();
	return ret;

}

MMD_U32  MMD_RV_File_Resume(MMD_PLYFILE pfile)
{
	MMD_U32 ret;

	ret =media_rv_file_resume((MMD_U32)pfile);
	if(ret != 0)
		RVERRCallBack();
	return ret;

}

MMD_U32  MMD_RV_File_Stop(MMD_PLYFILE pfile)
{
	MMD_U32 ret;

	ret =media_rv_file_stop((MMD_U32)pfile);
	if(ret != 0)
		RVERRCallBack();
	return ret;
}

MMD_U32  MMD_RV_File_Close(MMD_PLYFILE pfile)
{
	MMD_U32 ret;

	ret =media_rv_file_close((MMD_U32)pfile);
	if(ret != 0)
		RVERRCallBack();
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
MMD_U32 MMD_RV_File_GetPlayTime(MMD_PLYFILE pfile, MMD_U32*uPlayTime)
{
	MMD_U32 ret;

	ret = media_rv_file_getplaytime((MMD_U32)pfile, uPlayTime);
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
MMD_U32 MMD_RV_File_GetProperty(MMD_PLYFILE pfile, FILE_PROPERTY*property)
{
	MMD_U32 		 ret;

	//intialize property size
	property->size = 0;
	ret = media_rv_file_getproperty((VIM_MEDIA_PLYFILE)pfile, (VIM_MEDIA_FILE_PROPERTY *)property);
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
MMD_U32 MMD_RV_File_GetTotalTime(MMD_PLYFILE pfile, MMD_U32*uTotalTime)
{
	MMD_U32 		 ret;

	ret = media_rv_file_gettotaltime((MMD_U32)pfile, uTotalTime);
	return ret;
}

#endif

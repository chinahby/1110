/*******************************************************************************
 *  All rights reserved, Copyright (C) vimicro LIMITED 2007                                                     
 * --------------------------------------------------------------------------------------------------------------
 * [File Name]          : VIM_USER_Info.c
 * [Description]        : 适用于578b.所有客户需要定义的参数
 *				    和函数。和平台相关的东西都在这里定义。
 *                       	   
 *				
 * [Author]                 : angela	
 * [Date Of Creation]  : 2007-03-10 16:01
 * [Platform]               : 平台相关
 * [Note]                 : 
 *
 * Modification History : 
 * ------------------------------------------------------------------------------
 * Date        			Author     		Modifications
 * ------------------------------------------------------------------------------
 * 2007-03-10  		angela      		 Created
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
//#include "nucleus.h"
#include "VIM_COMMON.h"



//#define DEMOTEST	 // by lizg
#ifndef DEMOTEST	
UINT8 FastPreview;
UINT8 g_Reverse;
#endif
#if 0
/***************************************************************
Description:
		Read File based on customer's file system.

Parameters:
		pFile_Ptr:  File Handler
		pbBuffer:   data buffer
		pFile_Size: number of bytes to be read	

Returns:
		VIM_SUCCEED: means read file ok
		VIM_ERROR_USER_READFILE: means have something error in read
		file  process
****************************************************************/
UINT32 VIM_USER_ReadFile(HUGE void* pFile_Ptr,  HUGE UINT8* pbBuffer, UINT32 dFile_Size)
{

	UINT32 LENGTH=0;
#ifdef DEMOTEST		
	LENGTH=f_read(pbBuffer, 1, dFile_Size, (F_FILE *)pFile_Ptr);
#endif
	return LENGTH;

}


/***************************************************************
Description:
		Seek File based on customer's file system.

Parameters:
		pFile_Ptr:  File Handler
		dOffset:	   File offset

Returns:
		VIM_SUCCEED: means read file ok
		VIM_ERROR_USER_SEEKFILE: means have something error in seek
		file  process

****************************************************************/

VIM_RESULT VIM_USER_SeekFile(HUGE void* pFile_Ptr, UINT32 dOffset)
{
#ifdef DEMOTEST	
UINT8 ret;
	ret=f_seek((F_FILE *)pFile_Ptr,dOffset,0);
	if(ret)
		return VIM_ERROR_USER_SEEKFILE;
	else
#endif
		return VIM_SUCCEED;
}

/***************************************************************
Description:
		Write File based on customer's file system.

Parameters:
		pFile_Ptr:  File Handler
		pbBuffer:   data buffer
		pFile_Size: number of bytes to be read	

Returns:
		VIM_SUCCEED: means read file ok
		VIM_ERROR_USER_WRITEFILE: means have something error in write
		file  process
****************************************************************/
VIM_RESULT VIM_USER_WriteFile(HUGE void* pFile_Ptr, const HUGE  UINT8* pbBuffer, UINT32 dFile_Size)
{
#ifdef DEMOTEST
	UINT32 LENGTH;
	//F_SPACE space;
	#if(VIM_USER_DEBUG)&&(VIM_5XRDK_DEBUG)
		VIM_USER_PrintHex("\n VIM_USER_WriteFile ,FILEID =",(int)pFile_Ptr);
	#endif	
	//fn_getfreespace(&space);
	//if(space.free<dFile_Size+10000)
		//return VIM_ERROR_USER_WRITEFILE;

	//VIM_USER_PrintDec(" space.free  =",space.free);
	
	LENGTH=f_write(pbBuffer, 1, dFile_Size, (F_FILE *)pFile_Ptr);
	#if(VIM_USER_DEBUG)&&(VIM_5XRDK_DEBUG)
		VIM_USER_PrintDec("\n LENGTH  =",LENGTH);
		VIM_USER_PrintDec("\n real size  =",dFile_Size);
	#endif	
	if(LENGTH!=dFile_Size)
		return VIM_ERROR_USER_WRITEFILE;
	else
#endif
		return VIM_SUCCEED;
}
#endif
/***************************************************************
Description:
		Malloc Function

Parameters:
		UINT32 dwbyte :the length of buffer which is needed

Returns:
		void *: return the head point of buf (any format)
		if return 0: means malloc error

****************************************************************/
extern void* MemMalloc(UINT32 size);
HUGE void* VIM_USER_MallocMemory(UINT32 dwbyte)
{
#ifdef DEMOTEST		
#if(VIM_USER_DEBUG)&&(VIM_5XRDK_DEBUG)
	VIM_USER_PrintDec("[VC0578][USER]********* malloc memory:****************  ",dwbyte);
#endif
	return MemMalloc(dwbyte);
#else
	return 0;
#endif
}

/***************************************************************
Description:
		Free Function

Parameters:
		void* mem :the head point of buf
		
Returns:
		VIM_SUCCEED: means free ok
		VIM_ERROR_USER_FREEMEM: means have something error in free process

****************************************************************/
extern void MemFree(void *Prl);
VIM_RESULT VIM_USER_FreeMemory(void* mem)
{
#ifdef DEMOTEST		
	MemFree(mem);
#if(VIM_USER_DEBUG)&&(VIM_5XRDK_DEBUG)
	VIM_USER_PrintString("[VC0578][USER]************  free memory !************");
#endif
#endif
	return VIM_SUCCEED;

}

/***************************************************************
Description:
		Sets buffers to a specified character.

Parameters:
		pbSrc		: Pointer to destination
		bValue		: Character to set
		dCount		: Number of characters

Returns:
		void

****************************************************************/
void	VIM_USER_MemSet(HUGE UINT8 *pbDest,  UINT8 bValue, UINT32 dCount)
{
	UINT32	dSize = dCount;

	while (dSize --)
	{
		pbDest[dSize] = bValue;
	}
}


/***************************************************************
Description:
		Copies count bytes from pbSrc to pbDest.

Parameters:
		pbDest		: New buffer
		pbSrc		: Buffer to copy from
		dCount		: Number of bytes to copy	

Returns:
		void

****************************************************************/
void	VIM_USER_MemCpy(HUGE UINT8 *pbDest, const HUGE UINT8 *pbSrc, UINT32 dCount)
{
#if(BOARD_TYPE==VC0820_FPGA)
	MMD_MemCpy(pbDest, pbSrc, dCount);
#else
	UINT32	dSize = 0;

	while (dSize < dCount)
	{
		pbDest[dSize] = pbSrc[dSize];
		dSize++;
	}
#endif
}

#if(BOARD_TYPE==ARM9DEMO||(BOARD_TYPE==ARM9OLDDEMO))


//#define iprintf VIM_UART_Printf

#define Uart_Printf VIM_UART_Printf
#else
extern void Uart_Printf(char *fmt,...);

#endif
/***************************************************************
Description:
		Print data

Parameters:
		string		:the head point of sting
		data:  the data needed  to print (0x...)

Returns:
		void

****************************************************************/
#define  Uart_Printf MMDPrintf		// by lizg
void VIM_USER_PrintHex(char *string,UINT32 data)
{
	//Uart_Printf("\n%s0x%x",string,data);
}
/***************************************************************
Description:
		Print data

Parameters:
		string		:the head point of sting
		data:  the data needed  to print (dec format)

Returns:
		void

****************************************************************/
void VIM_USER_PrintDec(char *string,UINT32 data)
{
	//MMDPrintf("\n%s%d",string,data);
}

/***************************************************************
Description:
		Print data

Parameters:
		string		:the head point of sting

Returns:
		void

****************************************************************/
void VIM_USER_PrintString(char *string)
{
	//Uart_Printf("\n%s",string);
}
/***************************************************************
Description:
		Delay function

Parameters:
		ms		:the num ms want to be delay

Returns:
		void

****************************************************************/
//extern void Delay(int time); // 1ms 
#if 0
void VIM_Delay(int time) // 1ms, by lizg
{
	int i,j;
	if(time>=10)
		NU_Sleep(time/10);
	else
	{
		for(i=0;i<time;i++)
			for(j=0;j<0x500;j++);
	}	
}	
#endif
extern void MMD_USER_WaitMSEC(MMD_U32 msec);
extern void MMD_USER_WaitMICRS(MMD_U32 msec);
void VIM_USER_DelayMs(UINT32 ms)
{
	MMD_USER_WaitMICRS(ms*10);
	
}


/***************************************************************
Description:
		Check the System is Big Endian or Little Endian
		
Parameters:
		void
		
Returns:
		1 :	Big Endian
		0 :	Little Endian
****************************************************************/
UINT8 VIM_USER_CheckBigendian(void)
{
 	union{
   		UINT32	l;
		UINT8	c[4];
	} u;

 	u.l = 1;
 	return ( u.c[3]==1 );
}

/***************************************************************
Description:
		Start Timer and set timer interval

Parameters:
		interval : the interval of every timer(ms)

Returns:
		0: true
		1-255: error

****************************************************************/
extern UINT8 g_DemoFlage;
#ifdef DEMOTEST	
NU_TIMER video_timer;
extern void 	InitTimer0ForCommandParase(void (*func)(),int interval);
extern void User_TimerProcess(UNSIGNED val);
extern void Av_TimerProcess(UNSIGNED val);
extern UINT8 CAPTUREEND;
#endif

/*VIM_RESULT VIM_USER_StartTimer(UINT32 Intervel)
{
#ifdef DEMOTEST	
int status;	
#if(VIM_USER_DEBUG)&&(VIM_5XRDK_DEBUG)
	VIM_USER_PrintDec("  start timer =",Intervel);
#endif
	if(!g_DemoFlage)
		InitTimer0ForCommandParase(VIM_HAPI_Timer,Intervel);
	else
	{
		if(Intervel<15)
			Intervel=15;
		if(CAPTUREEND)
			status=NU_Create_Timer(&video_timer,"display video",Av_TimerProcess,1,1,(unsigned long)(Intervel/15),NU_DISABLE_TIMER);
		else 
			status=NU_Create_Timer(&video_timer,"display video",Av_TimerProcess,1,1,(unsigned long)(1),NU_DISABLE_TIMER);

		status=NU_Control_Timer(&video_timer,NU_ENABLE_TIMER);
	}
#endif
	return VIM_SUCCEED;
}*/
/***************************************************************
Description:
		cancel Timer 
Parameters:

Returns:
		0: true
		1-255: error

****************************************************************/
extern void Timer0_stop(void);
/*VIM_RESULT VIM_USER_StopTimer(void)
{
#ifdef DEMOTEST	
int status;
	
#if(VIM_USER_DEBUG)&&(VIM_5XRDK_DEBUG)
	VIM_USER_PrintString("  stop timer");
#endif
	if(!g_DemoFlage)
		Timer0_stop();
	else
	{
		status=NU_Control_Timer(&video_timer,NU_DISABLE_TIMER);
		status=NU_Delete_Timer(&video_timer);
	}
#endif
	return VIM_SUCCEED;
}*/
//#if (BOARD_TYPE==VC0820_FPGA)//add by zhoujian 20080218
UINT32 VIM_USER_GetSystemTime_1us(void)
{
	return timer_getvalue(2);

}
UINT32 VIM_USER_GetSystemTime_10ms(void)
{
	return MMD_SYS_GetTimer2Value()/100;//NU_Retrieve_Clock();	//10ms

}
//#endif

/***************************************************************
Description:
	chip reset 
Parameters:

Returns:

****************************************************************/
//extern void Port_V5ChipReset(void);
void VIM_USER_Reset(void)
{
	//UINT8 TEMP;
	//reset jpeg ic.
	//Port_V5ChipReset();
	/*//TEMP=(*(volatile unsigned *)0x1d20044);
	(*(volatile unsigned *)0x1d20044) = 0x7f;
	Delay(100);
	(*(volatile unsigned *)0x1d20044) = 0xff;
	//Delay(100);
	Uart_Printf("&&&&&&&&*******%x*************",TEMP);*/
}
#if 0
volatile UINT32 g_dma_flag;
NU_HISR	g_578_HISR;
int HISR_STACK_578[256];		// 1k byte

void dma_video_isr(SINT32 vector)
{
	g_dma_flag = 0;
}
void hisr_578(void)   
{
	int i;
	for(i=0;i<VIM_FIRST_LEVEL_INT_NUM;i++)
	{
		if(gVc0578b_Isr.byFirstLevelInt[i]==1)
		{
			//Uart_Printf("\n1stIrq=0x%x,2ncIrq=0x%x\n", i, gVc0578b_Isr.bSecondLevelIntFlag[i] );

			VIM_HIF_HandleISR(i, gVc0578b_Isr.bSecondLevelIntFlag[i] );
			gVc0578b_Isr.byFirstLevelInt[i]=0;
		}	
	}
}


/*************************************************
  Function:       
  	lisr_578
  	
  Description:    
  	578 1st level interrupt LISR function
  	
  Input:
	vector
		interrupt vector 
  Output:        
	NULL
	
  Return:         
  	Error code 
  	
  History:        

      <author>  	<time>     	<desc>
      lizg    		07/05/04     	create this function  
*************************************************/
void lisr_578(int vector)    //isr sub routine 
{	

	VIM_INT_Mask(vector);

	//读取二级中断状态寄存器,用于判断
	gVc0578b_Isr.byFirstLevelInt[vector]=1;
	gVc0578b_Isr.bSecondLevelIntFlag[vector] = VIM_HIF_GetIntFlagSec((VIM_HIF_INT_TYPE)vector);	

	//清除中断标志	
	VIM_INT_Acknowledge(vector);	
	VIM_INT_Unmask(vector);

	// active HISR process
	VIM_INT_Active_Hisr(&g_578_HISR);
}

/*************************************************
  Function:       
  	VIM_HAPI_Init_IRQ
  	
  Description:    
  	578 1st level interrupt HISR function
  	
  Input:
	NULL
	
Output:        
	NULL
	
  Return:         
  	Error code 
  	
  History:        

      <author>  	<time>     	<desc>
      lizg    		07/05/04     	create this function  
*************************************************/
UINT32 VIM_USER_Init_IRQ(void)
{
	STATUS			status;
	UINT32 i;
	UINT32 vect_578[]={VM_HIF_INT_SIF, VM_HIF_INT_ISP, VM_HIF_INT_LBUF, VM_HIF_INT_JPEG,
					VM_HIF_INT_IPP, VM_HIF_INT_MARB, VM_HIF_INT_LCDC, VM_HIF_INT_TVDC,
					VM_HIF_INT_VDEC, VM_HIF_INT_VENC}; 

	//Create HISR	
	status= VIM_INT_HISR_Create(&g_578_HISR,
								"578",
								hisr_578,
								0,
								HISR_STACK_578,
								sizeof(HISR_STACK_578));
	if(status!=NU_SUCCESS)
	{
		//error
		return ERR_CREATE_HISR;
	}
	for(i=0;i<sizeof(vect_578)/sizeof(UINT32);i++)
	{
		VIM_INT_LISR_Create(vect_578[i],lisr_578);
		VIM_INT_Acknowledge(vect_578[i]);
	}
	// used for memcpy
	VIM_DMA_Lisr_Register(0, dma_video_isr, NULL);

	return MMD_RET_NOERR;
}
#endif
//#if VIM_SENSOR_AUTO_FOCUS
UINT8 g_checkid=0;
//extern UINT8 SensorIdChk_MT9T012_BAYER(void);
/***************************************************************
Description:
	AF function init
Parameters:
	void
Returns:

****************************************************************/
/*void VIM_USER_AutoFocusInit(void)
{

if(!g_checkid)
{
	if(SensorIdChk_MT9T012_BAYER()==FAILED)
		g_checkid=1;
	else
		g_checkid=2;
}

if(g_checkid==1)
{

	VIM_HIF_SetGpioInfo(VIM_GPIO_PUTOUT,4);
	VIM_HIF_SetGpioValue(4,1);
}
else
{
	VIM_SIF_SetPwmInit(VIM_SIF_PWM_NUM1,VIM_SIF_PWM_HIGHACTIVE);
	VIM_SIF_SetPwmClock(50);
	VIM_SIF_SetPwmDuty(2,VIM_SENSOR_AF_MAXSTEP);
	VIM_SIF_SetPwmEn(ENABLE);
	VIM_HIF_SetGpioInfo(VIM_GPIO_PUTOUT,3);
	VIM_HIF_SetGpioValue(3,1);
}
}*/

/***************************************************************
Description:
	AF function init
Parameters:
	void
Returns:

****************************************************************/
void VIM_USER_AutoFocusEnable(BOOL Enable)
{
if(g_checkid==1)
{
	if(Enable==ENABLE)
	{
		//VIM_HIF_SetGpioValue(4,1);
		VIM_SIF_InitI2c(0x18,300);
	}
	else
	{
		//VIM_HIF_SetGpioValue(4,0);
		VIM_SIF_InitI2c(0x60,100);
	}
}
else
{
	if(Enable==ENABLE)
	{
		VIM_HIF_SetGpioValue(3,1);
	}
	else
	{
		VIM_HIF_SetGpioValue(3,0);
	}
}
}


/***************************************************************
Description:
	AF function
Parameters:
	Step:0-14;
Returns:

****************************************************************/
void VIM_USER_SetAutoFocusStep(UINT8 Step)
{
UINT32 Value;
if(g_checkid==1)
{
	VIM_USER_AutoFocusEnable(1);
	Value=(613-202)*Step/20+202;
	Value&=0x3ff;
	VIM_SIF_I2cWriteByte((UINT32)(Value>>4),(UINT8)(Value<<4),1);
	VIM_USER_AutoFocusEnable(0);
	//VIM_SIF_I2cWriteByte((UINT32)(Value>>4)|0x80,(UINT8)(Value<<4),1);	
}
else
{
	VIM_HIF_SetGpioValue(3,1);
	VIM_SIF_SetPwmDuty(Step,VIM_SENSOR_AF_MAXSTEP);
}
}
//#endif

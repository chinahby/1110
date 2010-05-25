/*******************************************************************************
 *  All rights reserved, Copyright (C) vimicro LIMITED 2007                                                     
 * --------------------------------------------------------------------------------------------------------------
 * [File Name]          : VIM_HIF_Driver.c
 * [Description]        : 适用于578b.定义了所有跟host interface有关的
 *				     函数。 还包括一些pll模块。
 *                       	     相关模块包括(hif ；pmu；clockrest)
 *				
 * [Author]                 : angela	
 * [Date Of Creation]  : 2007-03-10 16:01
 * [Platform]               : 平台无关
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

#include "VIM_COMMON.h"
#include "v8_biu.h"
//----------------------------------------------------------------
//---------------------------------------------------------------------------------
// Internal functions
#if(VIM_BUSTYPE != VIM_VC0820_MULTI32)
//static  INT32 g_delayfor;
#endif

#if VIM_BUSTYPE == VIM_MULTI16
#if 1
/***************************************************************
Description:
		set 8bit register use multi16

Parameters:
		adr: register address
		val:	   register value

Returns:
		void
****************************************************************/
 void VIM_HIF_SetReg8(UINT32 adr, UINT8 val)       
{
	Host_HIF_SetReg8(adr,val);
}
/***************************************************************
Description:
		get 8bit register use multi16

Parameters:
		adr: register address
		val:	   register value

Returns:
		return uint 8
****************************************************************/

UINT8 VIM_HIF_GetReg8(UINT32 adr)
{
	return Host_HIF_GetReg8(adr);
}
/***************************************************************
Description:
		set 16bit register use multi16

Parameters:
		adr: register address
		val:	   register value

Returns:
		void
****************************************************************/
void VIM_HIF_SetReg16(UINT32 adr, UINT16 val)       
{
	Host_HIF_SetReg16(adr,val);
}
/***************************************************************
Description:
		get 16bit register use multi16

Parameters:
		adr: register address
		val:	   register value

Returns:
		return uint 16
****************************************************************/
UINT16 VIM_HIF_GetReg16(UINT32 adr)
{
	return Host_HIF_GetReg16(adr);
}


/***************************************************************
Description:
		set 32bit register use multi16

Parameters:
		adr: register address
		val:	   register value

Returns:
		void
****************************************************************/
void VIM_HIF_SetReg32(UINT32 adr, UINT32 val)       
{

	Host_HIF_SetReg32(adr,val);
	
}
/***************************************************************
Description:
		get 32bit register use multi16

Parameters:
		adr: register address
		val:	   register value

Returns:
		return uint 16
****************************************************************/
UINT32 VIM_HIF_GetReg32(UINT32 adr)
{
	return Host_HIF_GetReg32(adr);

}

/***************************************************************
Description:
		set bypass register use multi16

Parameters:
		adr: register address
		val:	   register value

Returns:
		void
****************************************************************/
void VIM_HIF_SetSpecialReg(UINT8 val)       
{
}

/***************************************************************
Description:
		read sdram 8bit

Parameters:
		adr: sdram start address
		buf: the head point of buf
		size:	  the lenght of data(byte)

Returns:
		void
****************************************************************/

UINT32 VIM_HIF_ReadSram(UINT32 adr,  HUGE UINT8 *buf, UINT32 size,VIM_HIF_SRAMMODE Mode)
{
	UINT32 FifoNum, ToEnd,addr,len;
	
	switch(Mode)
	{
	case VIM_HIF_SRAM_MODE:
		//V8_Mul16RdSramForCap(adr,(char *)buf,size);
		V8_Mul16RdSram(adr,(char *)buf,size);
		break;

	case VIM_HIF_FIFO_0_MODE:
	case VIM_HIF_FIFO_1_MODE:		
		//(1) get fifo number
		FifoNum = Mode-3;
		//(2) get size from read cursor to the end of fifo
		ToEnd = gVc0578b_Info.MarbStatus.VFifo[FifoNum].length-gVc0578b_Info.MarbStatus.VFifo[FifoNum].read_cur;
		addr = gVc0578b_Info.MarbStatus.VFifo[FifoNum].address+gVc0578b_Info.MarbStatus.VFifo[FifoNum].read_cur;

		//(3) read data from fifo
		if(size<=ToEnd)
		{
			VIM_USER_PrintDec("[VC0578B][HIF] copy size= ",size);
			//get_video_data(buf,(UINT8*)addr, size);
			V8_Mul16RdSram(addr,(char *)buf, size);
			gVc0578b_Info.MarbStatus.VFifo[FifoNum].read_cur+=size;
		}
		else
		{
			VIM_USER_PrintDec("[VC0578B][HIF] copy size= ",ToEnd);
			//get_video_data(buf,(UINT8*)addr, size);
			V8_Mul16RdSram(addr,(char *)buf, size);
			len = size - ToEnd;
			addr = gVc0578b_Info.MarbStatus.VFifo[FifoNum].address;
			VIM_USER_PrintDec("[VC0578B][HIF] copy size= ",len);
			//get_video_data(buf,(UINT8*)addr, size);
			V8_Mul16RdSram(addr,(char *)(buf+ToEnd), size);
			gVc0578b_Info.MarbStatus.VFifo[FifoNum].read_cur=len;
		}

		//(4) set fifo count 
		VIM_USER_PrintDec("[VC0578B][HIF] set count= ",size);

		VIM_MARB_SetFifoCount(FifoNum*2,MARB_MODE_ARMD_READ,size);

		break;

	}

	return size;
}
/***************************************************************
Description:
		write sdram 8bit reverse

Parameters:
		adr: sdram start address
		buf: the head point of buf
		size:	  the lenght of data(byte)

Returns:
		void
****************************************************************/
UINT32 VIM_HIF_WriteSramReverse(UINT32 adr, const HUGE UINT8 *buf, UINT32 size)
{
#if 0
	UINT32 uActsize=0, uTmp=0,uWaitlen=VIM_WAITMAX;

	adr >>= 2;		/*!< address must 4 byte align, read buffer */
	adr <<= 2;
	adr+=V5_1TSRAM_BASE;
	
	size >>= 2;
	uActsize = size << 2;

	while(uWaitlen--)
	{
		if(VIM_HIF_GetReg32(V5_REG_BIU_STATUS)&0x1)		/*!< wait untile ready */
			break;
	}
#if(VIM_HIF_DEBUG)&&(VIM_5XRDK_ERROR)
	if(!uWaitlen)
		VIM_USER_PrintString("[VC0578B][BIU] BIU is not ready for write [ERROR] ");
#endif
	adr|=1;    //write enable
	VIM_HIF_SetReg32(V5_REG_BIU_BASE_ADDR, adr);				/*!< set burst base address */
	VIM_HIF_SetReg32(V5_REG_BIU_BURST_LEN, size);				/*!< set burst length, word size */

	//这里需要delay 50个hclock
	for(g_delayfor=0; g_delayfor<VIM_READRAM_DELAYFOR; g_delayfor++);

	/*! read data continously */
	//gVc0578b_Isr.byIntFlag=1;
	VIM_SET_RSHIGH_BY_GPIO();
	while(size--)
	{
		uTmp = (UINT32)(buf[2]<<8);
		uTmp |= (UINT32)buf[3];
		*(volatile UINT16*)(VIM_REG_VALUE) = uTmp;
		uTmp = (UINT32)(buf[0]<<8);
		uTmp |= (UINT32)buf[1];
		*(volatile UINT16*)(VIM_REG_VALUE) = uTmp;		
		buf+=4;
	}
	//gVc0578b_Isr.byIntFlag=0;
	while(uWaitlen--)
	{
		if(VIM_HIF_GetReg32(V5_REG_BIU_STATUS)&0x1)		/*!< wait untile ready */
			break;
	}
	return uActsize;
#endif
	return 0;
}
/***************************************************************
Description:
		write sdram 8bit

Parameters:
		adr: sdram start address
		buf: the head point of buf
		size:	  the lenght of data(byte)

Returns:
		void
****************************************************************/

 UINT32 VIM_HIF_WriteSram(UINT32 adr, const HUGE UINT8 *buf, UINT32 size,VIM_HIF_SRAMMODE Mode)
{
	UINT32 FifoNum, ToEnd,addr,len;
	
	switch(Mode)
	{
		case VIM_HIF_SRAM_MODE:
			V8_Mul16WrSram(adr,(char *)buf,size);
			break;
		
		case VIM_HIF_FIFO_0_MODE:
		case VIM_HIF_FIFO_1_MODE:		
			//(1) get fifo number
			FifoNum = Mode-3;
			//(2) get size from write cursor to the end of fifo
			ToEnd = gVc0578b_Info.MarbStatus.VFifo[FifoNum].length-gVc0578b_Info.MarbStatus.VFifo[FifoNum].write_cur;
			addr = gVc0578b_Info.MarbStatus.VFifo[FifoNum].address+gVc0578b_Info.MarbStatus.VFifo[FifoNum].write_cur;
			//(3) write data to fifo
			if(size<=ToEnd)
			{
				V8_Mul16WrSram(addr,(char *)buf,size);
				gVc0578b_Info.MarbStatus.VFifo[FifoNum].write_cur+=size;
			}
			else
			{
				V8_Mul16WrSram(addr,(char *)buf,ToEnd);
				len = size - ToEnd;
				addr = gVc0578b_Info.MarbStatus.VFifo[FifoNum].address;
				V8_Mul16WrSram(addr,(char *)(buf+ToEnd), len);
				gVc0578b_Info.MarbStatus.VFifo[FifoNum].write_cur=len;
			}
			//(4) set fifo count 
			VIM_MARB_SetFifoCount(FifoNum*2,MARB_MODE_ARMD_WRITE,size);
			break;
	}
	
	return size;
}
#if VIM_USER_SUPPORT_SDRAM
/***************************************************************
Description:
		write sdram 8bit

Parameters:
		adr: sdram start address
		buf: the head point of buf
		size:	  the lenght of data(byte)

Returns:
		void
****************************************************************/

 UINT32 VIM_HIF_WriteSdram(UINT32 adr, const HUGE UINT8 *buf, UINT32 size,VIM_HIF_SRAMMODE Mode)
{
	UINT32 uActsize=0, uTmp=0,uWaitlen=VIM_WAITMAX;

	adr >>= 2;		/*!< address must 4 byte align, read buffer */
	adr <<= 2;
	adr+=V5_SDRAM_BASE;
	
	size >>= 2;
	uActsize = size << 2;

	while(uWaitlen--)
	{
		if(VIM_HIF_GetReg32(V5_REG_BIU_STATUS)&0x1)		/*!< wait untile ready */
			break;
	}
#if(VIM_HIF_DEBUG)&&(VIM_5XRDK_ERROR)
	if(!uWaitlen)
		VIM_USER_PrintString("[VC0578B][BIU] BIU is not ready for write [ERROR] ");
#endif
	adr|=1;    //write enable
	VIM_HIF_SetReg32(V5_REG_BIU_BASE_ADDR, adr|Mode);				/*!< set burst base address */
	VIM_HIF_SetReg32(V5_REG_BIU_BURST_LEN, size);				/*!< set burst length, word size */

	//这里需要delay 50个hclock
	for(g_delayfor=0; g_delayfor<VIM_READRAM_DELAYFOR; g_delayfor++);

	/*! read data continously */
	//gVc0578b_Isr.byIntFlag=1;
	VIM_SET_RSHIGH_BY_GPIO();
	while(size--)
	{
		uTmp = (UINT32)(buf[3]<<8);
		uTmp |= (UINT32)buf[2];
		*(volatile UINT16*)(VIM_REG_VALUE) = uTmp;
		uTmp = (UINT32)(buf[1]<<8);
		uTmp |= (UINT32)buf[0];
		*(volatile UINT16*)(VIM_REG_VALUE) = uTmp;		
		buf+=4;
	}
	//gVc0578b_Isr.byIntFlag=0;
	while(uWaitlen--)
	{
		if(VIM_HIF_GetReg32(V5_REG_BIU_STATUS)&0x1)		/*!< wait untile ready */
			break;
	}
	return uActsize;
}
/***************************************************************
Description:
		read sdram 8bit

Parameters:
		adr: sdram start address
		buf: the head point of buf
		size:	  the lenght of data(byte)

Returns:
		void
****************************************************************/

UINT32 VIM_HIF_ReadSdram(UINT32 adr,  HUGE UINT8 *buf, UINT32 size,VIM_HIF_SRAMMODE Mode)
{
	UINT32 uActsize=0, uTmp=0,uWaitlen=VIM_WAITMAX;

	adr >>= 2;		/*!< address must 4 byte align, read buffer */
	adr <<= 2;
	adr+=V5_SDRAM_BASE;
	
	size >>= 2;
	uActsize = size << 2;

	while(uWaitlen--)
	{
		if(VIM_HIF_GetReg32(V5_REG_BIU_STATUS)&0x1)		/*!< wait untile ready */
			break;
	}
#if(VIM_HIF_DEBUG)&&(VIM_5XRDK_ERROR)
	if(!uWaitlen)
		VIM_USER_PrintString("[VC0578B][BIU] BIU is not ready for read [ERROR] ");
#endif
				/*!< wait untile ready */
	VIM_HIF_SetReg32(V5_REG_BIU_BASE_ADDR, adr|Mode);				/*!< set burst base address */
	VIM_HIF_SetReg32(V5_REG_BIU_BURST_LEN, size);				/*!< set burst length, word size */

	//这里需要delay 50个hclock
	for(g_delayfor=0; g_delayfor<VIM_READRAM_DELAYFOR; g_delayfor++);

	//gVc0578b_Isr.byIntFlag=1;
	/*! read data continously */
	VIM_SET_RSHIGH_BY_GPIO();
	while(size--)
	{
		uTmp = *(volatile UINT16*)(VIM_REG_VALUE);
		buf[2] = (UINT8)uTmp;
		buf[3] = (UINT8)(uTmp>>8);
		uTmp = *(volatile UINT16*)(VIM_REG_VALUE);
		buf[0] = (UINT8)uTmp;
		buf[1] = (UINT8)(uTmp>>8);
		buf+=4;
	}
	//gVc0578b_Isr.byIntFlag=0;
	while(uWaitlen--)
	{
		if(VIM_HIF_GetReg32(V5_REG_BIU_STATUS)&0x1)		/*!< wait untile ready */
			break;
	}
	return uActsize;
}
#endif
/***************************************************************
Description:
		write sdram 8bit on word

Parameters:
		adr: sdram start address
		buf: the head point of buf
		size:	  the lenght of data(byte)

Returns:
		void
****************************************************************/
// size in byte unit

UINT32 VIM_HIF_WriteSramOnWord(UINT32 adr, UINT16 dat, UINT32 size)
{
	UINT32 uActsize;
	uActsize = V8_Mul16WrSramOnWord(adr,dat,size);
	return uActsize;
}

void get_video_data(UINT8*des, UINT8 *scr, UINT32 size)
{

	VIM_HIF_ReadSram((UINT32)scr,des, size,VIM_HIF_SRAM_MODE);
	return;

}

/***************************************************************
Description:
		set multi 16 mode

Parameters:
		Timing:
			abi delay (0~32ns)max is 0x18
			wr delay (0~32ns)max is 0xb
			cs delay (0~32ns)max is 0x6

Returns:
		void
****************************************************************/
#if 0
void VIM_HIF_SetMulti16(void)
{


UINT32	temp,i;
	//VIM_SET_NORMAL_BY_GPIO();

	VIM_SET_RSLOW_BY_GPIO();
	*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((V5_REG_CPM_ABI_TEST>>24)&0xff);
	*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((V5_REG_CPM_ABI_TEST>>16)&0xff);
	*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((V5_REG_CPM_ABI_TEST>>8)&0xff);
	*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((V5_REG_CPM_ABI_TEST&0xfc));			
	VIM_SET_RSHIGH_BY_GPIO();
	for(i=0; i<10; i++);
		
	temp = ((UINT32)(*(volatile UINT8 *)VIM_REG_VALUE)<<24);
	temp |= ((UINT32)(*(volatile UINT8 *)VIM_REG_VALUE)<<16);
	temp |= ((UINT32)(*(volatile UINT8 *)VIM_REG_VALUE)<<8);
	temp |= ((UINT32)(*(volatile UINT8 *)VIM_REG_VALUE));
	
#if(VIM_HIF_DEBUG)&&(VIM_5XRDK_DEBUG)
		VIM_USER_PrintHex("V5_REG_CPM_ABI_TEST=",temp);
#endif

	VIM_SET_RSLOW_BY_GPIO();
	*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((V5_REG_CPM_ABI_CFG>>24)&0xff);
	*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((V5_REG_CPM_ABI_CFG>>16)&0xff);
	*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((V5_REG_CPM_ABI_CFG>>8)&0xff);
	*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((V5_REG_CPM_ABI_CFG&0xfc));			/*!< 地址是4的倍数 */
	VIM_SET_RSHIGH_BY_GPIO();
	for(i=0; i<10; i++);
		
	temp = ((UINT32)(*(volatile UINT8 *)VIM_REG_VALUE)<<24);
	temp |= ((UINT32)(*(volatile UINT8 *)VIM_REG_VALUE)<<16);
	temp |= ((UINT32)(*(volatile UINT8 *)VIM_REG_VALUE)<<8);
	temp |= ((UINT32)(*(volatile UINT8 *)VIM_REG_VALUE));

#if(VIM_HIF_DEBUG)&&(VIM_5XRDK_DEBUG)
	VIM_USER_PrintHex("V5_REG_CPM_ABI_CFG=",temp);
#endif

	VIM_SET_RSLOW_BY_GPIO();
	*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((V5_REG_CPM_ABI_CFG&0xff000000)>>24);
	*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((V5_REG_CPM_ABI_CFG&0xff0000)>>16);
	*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((V5_REG_CPM_ABI_CFG&0xff00)>>8);
	*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((V5_REG_CPM_ABI_CFG&0xfc)|1);	//地址是4的倍数

	VIM_SET_RSHIGH_BY_GPIO();

	*(volatile UINT8 *)(VIM_REG_VALUE) = (UINT8)(0x01);    	//set multi16	
	*(volatile UINT8 *)(VIM_REG_VALUE) = (UINT8)(0x18);    		
	*(volatile UINT8 *)(VIM_REG_VALUE) = (UINT8)(0x0b);    		
	*(volatile UINT8 *)(VIM_REG_VALUE) = (UINT8)(0x06);     


	temp=VIM_HIF_GetReg32(V5_REG_CPM_ABI_CFG);

	
#if(VIM_HIF_DEBUG)&&(VIM_5XRDK_DEBUG)
		VIM_USER_PrintHex("V5_REG_CPM_ABI_CFG=",temp);
#endif

	//VIM_SET_BYPASS_BY_GPIO();

}
#endif


#else
/***************************************************************
Description:
		set 8bit register use multi16

Parameters:
		adr: register address
		val:	   register value

Returns:
		void
****************************************************************/
 void VIM_HIF_SetReg8(UINT32 adr, UINT8 val)       
{
	UINT32 uTemp=0;
	//gVc0578b_Isr.byIntFlag=1;
	VIM_SET_RSLOW_BY_GPIO();
	*(volatile UINT16 *)(VIM_REG_INDEX) = (UINT16)((adr&0xffff0000)>>16);
	*(volatile UINT16 *)(VIM_REG_INDEX) = (UINT16)(adr&0xfffc);	//地址是4的倍数 1表示写寄存器
	VIM_SET_RSHIGH_BY_GPIO();

	for(g_delayfor=0; g_delayfor<VIM_READREG_DELAYFOR; g_delayfor++);

	
	uTemp = (UINT32)(*(volatile UINT16 *)(VIM_REG_VALUE)<<16);         
	uTemp |= (UINT32)(*(volatile UINT16 *)(VIM_REG_VALUE));   

	VIM_SET_RSLOW_BY_GPIO();
	
	*(volatile UINT16 *)(VIM_REG_INDEX) = (UINT16)((adr&0xffff0000)>>16);
	*(volatile UINT16 *)(VIM_REG_INDEX) = (UINT16)(adr&0xfffc|1);	//地址是4的倍数 1表示写寄存器
	VIM_SET_RSHIGH_BY_GPIO();

	
	if((adr & 0x3) == 0)
	{
		*(volatile UINT16 *)(VIM_REG_VALUE) = (UINT16)((uTemp&0xffff0000)>>16);    		
		*(volatile UINT16 *)(VIM_REG_VALUE) = (UINT16)((uTemp&0xff00)|val);     
	}
	else if((adr & 0x3) == 1)
	{
		*(volatile UINT16 *)(VIM_REG_VALUE) = (UINT16)((uTemp&0xffff0000)>>16);    		
		*(volatile UINT16 *)(VIM_REG_VALUE) = (UINT16)((uTemp&0x00ff)|((UINT16)(val)<<8));     
	}
	else if((adr & 0x3) == 2)
	{
		*(volatile UINT16 *)(VIM_REG_VALUE) = (UINT16)(((uTemp&0xff000000)>>16)|val);     
		*(volatile UINT16 *)(VIM_REG_VALUE) = (UINT16)(uTemp&0xffff);    		
	}	
	else if((adr & 0x3) == 3)
	{
		*(volatile UINT16 *)(VIM_REG_VALUE) = (UINT16)(((uTemp&0x00ff0000)>>16)|((UINT16)(val)<<8));     
		*(volatile UINT16 *)(VIM_REG_VALUE) = (UINT16)(uTemp&0xffff);    		
	}	
	//gVc0578b_Isr.byIntFlag=0;
}
/***************************************************************
Description:
		get 8bit register use multi16

Parameters:
		adr: register address
		val:	   register value

Returns:
		return uint 8
****************************************************************/

UINT8 VIM_HIF_GetReg8(UINT32 adr)
{
UINT32 uTemp;
	//gVc0578b_Isr.byIntFlag=1;
	VIM_SET_RSLOW_BY_GPIO();
	*(volatile UINT16 *)(VIM_REG_INDEX) = (UINT16)((adr&0xffff0000)>>16);
	*(volatile UINT16 *)(VIM_REG_INDEX) = (UINT16)(adr&0xfffc);	//地址是4的倍数
	VIM_SET_RSHIGH_BY_GPIO();
	
	for(g_delayfor=0; g_delayfor<VIM_READREG_DELAYFOR; g_delayfor++);

	uTemp = (UINT32)(*(volatile UINT16 *)(VIM_REG_VALUE)<<16);         
	uTemp |= (UINT32)(*(volatile UINT16 *)(VIM_REG_VALUE));   
	//gVc0578b_Isr.byIntFlag=0;
	if((adr & 0x3) == 0)
	{
		return (UINT8)(uTemp&0xff);
	}
	else if((adr & 0x3) == 1)
	{
		return (UINT8)((uTemp&0xff00)>>8);
	}
	else if((adr & 0x3) == 2)
	{
		return (UINT8)((uTemp&0xff0000)>>16);
	}	
	else 
	{
		return (UINT8)((uTemp&0xff000000)>>24);
	}	

}
/***************************************************************
Description:
		set 16bit register use multi16

Parameters:
		adr: register address
		val:	   register value

Returns:
		void
****************************************************************/
void VIM_HIF_SetReg16(UINT32 adr, UINT16 val)       
{
	UINT32 uTemp;
	//gVc0578b_Isr.byIntFlag=1;	
	if((adr & 0x3) == 3)
	{
		VIM_SET_RSLOW_BY_GPIO();
		*(volatile UINT16 *)(VIM_REG_INDEX) = (UINT16)((adr&0xffff0000)>>16);
		*(volatile UINT16 *)(VIM_REG_INDEX) = (UINT16)(adr&0xfffc);	//地址是4的倍数
		VIM_SET_RSHIGH_BY_GPIO();
		for(g_delayfor=0; g_delayfor<VIM_READREG_DELAYFOR; g_delayfor++);

		uTemp = (UINT32)(*(volatile UINT16 *)(VIM_REG_VALUE)<<16);         
		uTemp |= (UINT32)(*(volatile UINT16 *)(VIM_REG_VALUE));   
		
		VIM_SET_RSLOW_BY_GPIO();
		*(volatile UINT16 *)(VIM_REG_INDEX) = (UINT16)((adr&0xffff0000)>>16);
		*(volatile UINT16 *)(VIM_REG_INDEX) = (UINT16)(adr&0xfffc|1);	//地址是4的倍数 1表示写寄存器
		VIM_SET_RSHIGH_BY_GPIO();

		*(volatile UINT16 *)(VIM_REG_VALUE) = (UINT16)(((uTemp&0x00ff0000)>>16)|((UINT16)(val&0xff)<<8));     
		*(volatile UINT16 *)(VIM_REG_VALUE) = (UINT16)(uTemp&0xffff);    		

		VIM_SET_RSLOW_BY_GPIO();
		*(volatile UINT16 *)(VIM_REG_INDEX) = (UINT16)((adr&0xffff0000)>>16);
		*(volatile UINT16 *)(VIM_REG_INDEX) = (UINT16)((adr+1)&0xfffc);	//地址是4的倍数
		VIM_SET_RSHIGH_BY_GPIO();
		for(g_delayfor=0; g_delayfor<VIM_READREG_DELAYFOR; g_delayfor++);


		uTemp = (UINT32)(*(volatile UINT16 *)(VIM_REG_VALUE)<<16);         
		uTemp |= (UINT32)(*(volatile UINT16 *)(VIM_REG_VALUE));   

		VIM_SET_RSLOW_BY_GPIO();
		*(volatile UINT16 *)(VIM_REG_INDEX) = (UINT16)((adr&0xffff0000)>>16);
		*(volatile UINT16 *)(VIM_REG_INDEX) = (UINT16)(((adr+1)&0xfffc)|1);	//地址是4的倍数
		VIM_SET_RSHIGH_BY_GPIO();

		*(volatile UINT16 *)(VIM_REG_VALUE) = (UINT16)((uTemp&0xffff0000)>>16);    		
		*(volatile UINT16 *)(VIM_REG_VALUE) = (UINT16)((uTemp&0xff00)|((UINT16)(val&0xff00)>>8));     
	

	}
	else
	{
		VIM_SET_RSLOW_BY_GPIO();
		*(volatile UINT16 *)(VIM_REG_INDEX) = (UINT16)((adr&0xffff0000)>>16);
		*(volatile UINT16 *)(VIM_REG_INDEX) = (UINT16)(adr&0xfffc);	//地址是4的倍数
		VIM_SET_RSHIGH_BY_GPIO();

		for(g_delayfor=0; g_delayfor<VIM_READREG_DELAYFOR; g_delayfor++);

		uTemp = (UINT32)(*(volatile UINT16 *)(VIM_REG_VALUE)<<16);         
		uTemp |= (UINT32)(*(volatile UINT16 *)(VIM_REG_VALUE));   

		VIM_SET_RSLOW_BY_GPIO();
		*(volatile UINT16 *)(VIM_REG_INDEX) = (UINT16)((adr&0xffff0000)>>16);
		*(volatile UINT16 *)(VIM_REG_INDEX) = (UINT16)(adr&0xfffc|1);	//地址是4的倍数 1表示写寄存器
		VIM_SET_RSHIGH_BY_GPIO();
		
		if((adr & 0x3) == 0)
		{
			*(volatile UINT16 *)(VIM_REG_VALUE) = (UINT16)((uTemp&0xffff0000)>>16);    		
			*(volatile UINT16 *)(VIM_REG_VALUE) = (UINT16)(val);     
		}
		else if((adr & 0x3) == 1)
		{
			*(volatile UINT16 *)(VIM_REG_VALUE) = (UINT16)((uTemp&0xff000000)>>16)|((UINT16)(val&0xff00)>>8);    		
			*(volatile UINT16 *)(VIM_REG_VALUE) = (UINT16)((uTemp&0x00ff)|((UINT16)(val&0xff)<<8));     
		}
		else if((adr & 0x3) == 2)
		{
			*(volatile UINT16 *)(VIM_REG_VALUE) = (UINT16)(val);     
    			*(volatile UINT16 *)(VIM_REG_VALUE) = (UINT16)(uTemp&0xffff);    		
		}	
		
	}
	//gVc0578b_Isr.byIntFlag=0;
}
/***************************************************************
Description:
		get 16bit register use multi16

Parameters:
		adr: register address
		val:	   register value

Returns:
		return uint 16
****************************************************************/
UINT16 VIM_HIF_GetReg16(UINT32 adr)
{
	UINT32 uTemp,uTemp1;
	//gVc0578b_Isr.byIntFlag=1;	
	if((adr & 0x3) == 3)
	{
		VIM_SET_RSLOW_BY_GPIO();
		*(volatile UINT16 *)(VIM_REG_INDEX) = (UINT16)((adr&0xffff0000)>>16);
		*(volatile UINT16 *)(VIM_REG_INDEX) = (UINT16)(adr&0xfffc);	//地址是4的倍数
		VIM_SET_RSHIGH_BY_GPIO();

		for(g_delayfor=0; g_delayfor<VIM_READREG_DELAYFOR; g_delayfor++);

		uTemp = (UINT32)(*(volatile UINT16 *)(VIM_REG_VALUE)<<16);         
		uTemp |= (UINT32)(*(volatile UINT16 *)(VIM_REG_VALUE));   


		VIM_SET_RSLOW_BY_GPIO();
		*(volatile UINT16 *)(VIM_REG_INDEX) = (UINT16)((adr&0xffff0000)>>16);
		*(volatile UINT16 *)(VIM_REG_INDEX) = (UINT16)((adr+1)&0xfffc);	//地址是4的倍数
		VIM_SET_RSHIGH_BY_GPIO();

		for(g_delayfor=0; g_delayfor<VIM_READREG_DELAYFOR; g_delayfor++);


		uTemp1 = (UINT32)(*(volatile UINT16 *)(VIM_REG_VALUE)<<16);         
		uTemp1|= (UINT32)(*(volatile UINT16 *)(VIM_REG_VALUE));   
		
		//gVc0578b_Isr.byIntFlag=0;
		return 	(UINT16)((uTemp1&0xff)<<8)|((uTemp&0xff000000)>>24);

	}
	else
	{
		VIM_SET_RSLOW_BY_GPIO();
		*(volatile UINT16 *)(VIM_REG_INDEX) = (UINT16)((adr&0xffff0000)>>16);
		*(volatile UINT16 *)(VIM_REG_INDEX) = (UINT16)(adr&0xfffc);	//地址是4的倍数

		for(g_delayfor=0; g_delayfor<VIM_READREG_DELAYFOR; g_delayfor++);

		VIM_SET_RSHIGH_BY_GPIO();
		uTemp = (UINT32)(*(volatile UINT16 *)(VIM_REG_VALUE)<<16);         
		uTemp |= (UINT32)(*(volatile UINT16 *)(VIM_REG_VALUE));   

		//gVc0578b_Isr.byIntFlag=0;
		if((adr & 0x3) == 0)
		{
			return   (UINT16)(uTemp&0xffff);
		}
		else if((adr & 0x3) == 1)
		{
			return   (UINT16)((uTemp&0xffff00)>>8);
		}
		else 
		{
			return   (UINT16)((uTemp&0xffff0000)>>16);
		}	
		
	}
}


/***************************************************************
Description:
		set 32bit register use multi16

Parameters:
		adr: register address
		val:	   register value

Returns:
		void
****************************************************************/
void VIM_HIF_SetReg32(UINT32 adr, UINT32 val)       
{

	//gVc0578b_Isr.byIntFlag=1;
	if((adr & 0x3) == 0)
	{
		VIM_SET_RSLOW_BY_GPIO();
		*(volatile UINT16 *)(VIM_REG_INDEX) = (UINT16)((adr&0xffff0000)>>16);
		*(volatile UINT16 *)(VIM_REG_INDEX) = (UINT16)((adr&0xfffc)|1);	//地址是4的倍数
		VIM_SET_RSHIGH_BY_GPIO();

		*(volatile UINT16 *)(VIM_REG_VALUE) = (UINT16)(((val&0xffff0000)>>16));     
		*(volatile UINT16 *)(VIM_REG_VALUE) = (UINT16)(val&0xffff);    		
	}
	else 
	{
		VIM_HIF_SetReg16(adr,(UINT16)val);
		VIM_HIF_SetReg16(adr+2,(UINT16)(val>>16));
	}
	
	//gVc0578b_Isr.byIntFlag=0;
}
/***************************************************************
Description:
		get 32bit register use multi16

Parameters:
		adr: register address
		val:	   register value

Returns:
		return uint 16
****************************************************************/
UINT32 VIM_HIF_GetReg32(UINT32 adr)
{
	UINT32 uTemp;
	//gVc0578b_Isr.byIntFlag=1;
	if((adr & 0x3) == 0)
	{
		VIM_SET_RSLOW_BY_GPIO();
		*(volatile UINT16 *)(VIM_REG_INDEX) = (UINT16)((adr&0xffff0000)>>16);
		*(volatile UINT16 *)(VIM_REG_INDEX) = (UINT16)(adr&0xfffc);	//地址是4的倍数
		VIM_SET_RSHIGH_BY_GPIO();
		
		for(g_delayfor=0; g_delayfor<VIM_READREG_DELAYFOR; g_delayfor++);

		uTemp = (UINT32)(*(volatile UINT16 *)(VIM_REG_VALUE)<<16);         
		uTemp |= (UINT32)(*(volatile UINT16 *)(VIM_REG_VALUE));   
	}
	else 
	{
		uTemp=(VIM_HIF_GetReg16(adr+2)<<16);
		uTemp|=VIM_HIF_GetReg16(adr);

	}
	//gVc0578b_Isr.byIntFlag=0;
	return uTemp;

}

/***************************************************************
Description:
		set bypass register use multi16

Parameters:
		adr: register address
		val:	   register value

Returns:
		void
****************************************************************/
void VIM_HIF_SetSpecialReg(UINT8 val)       
{
	//gVc0578b_Isr.byIntFlag=1;
	*(volatile UINT16 *)(VIM_REG_VALUE) = (UINT16)(((VIM_BYPASS_VALUE&0xffff0000)>>16));     
	*(volatile UINT16 *)(VIM_REG_VALUE) = (UINT16)((VIM_BYPASS_VALUE&0xff00)|val);    		
	*(volatile UINT16 *)(VIM_REG_INDEX) = (UINT16)(((VIM_BYPASS_ADDR&0xffff0000)>>16));     
	*(volatile UINT16 *)(VIM_REG_INDEX) = (UINT16)((VIM_BYPASS_ADDR&0xff00)|val);    
	//gVc0578b_Isr.byIntFlag=0;
}

/***************************************************************
Description:
		read sdram 8bit

Parameters:
		adr: sdram start address
		buf: the head point of buf
		size:	  the lenght of data(byte)

Returns:
		void
****************************************************************/

UINT32 VIM_HIF_ReadSram(UINT32 adr,  HUGE UINT8 *buf, UINT32 size,VIM_HIF_SRAMMODE Mode)
{
	UINT32 uActsize=0, uTmp=0,uWaitlen=VIM_WAITMAX;

	adr >>= 2;		/*!< address must 4 byte align, read buffer */
	adr <<= 2;
	adr+=V5_1TSRAM_BASE;
	
	size >>= 2;
	uActsize = size << 2;

	while(uWaitlen--)
	{
		if(VIM_HIF_GetReg32(V5_REG_BIU_STATUS)&0x1)		/*!< wait untile ready */
			break;
	}
#if(VIM_HIF_DEBUG)&&(VIM_5XRDK_ERROR)
	if(!uWaitlen)
		VIM_USER_PrintString("[VC0578B][BIU] BIU is not ready for read [ERROR] ");
#endif
				/*!< wait untile ready */
	VIM_HIF_SetReg32(V5_REG_BIU_BASE_ADDR, adr|Mode);				/*!< set burst base address */
	VIM_HIF_SetReg32(V5_REG_BIU_BURST_LEN, size);				/*!< set burst length, word size */

	//这里需要delay 50个hclock
	for(g_delayfor=0; g_delayfor<VIM_READRAM_DELAYFOR; g_delayfor++);

	//gVc0578b_Isr.byIntFlag=1;
	/*! read data continously */
	VIM_SET_RSHIGH_BY_GPIO();
	while(size--)
	{
		uTmp = *(volatile UINT16*)(VIM_REG_VALUE);
		buf[2] = (UINT8)uTmp;
		buf[3] = (UINT8)(uTmp>>8);
		uTmp = *(volatile UINT16*)(VIM_REG_VALUE);
		buf[0] = (UINT8)uTmp;
		buf[1] = (UINT8)(uTmp>>8);
		buf+=4;
	}
	//gVc0578b_Isr.byIntFlag=0;
	while(uWaitlen--)
	{
		if(VIM_HIF_GetReg32(V5_REG_BIU_STATUS)&0x1)		/*!< wait untile ready */
			break;
	}
	return uActsize;
}
/***************************************************************
Description:
		write sdram 8bit reverse

Parameters:
		adr: sdram start address
		buf: the head point of buf
		size:	  the lenght of data(byte)

Returns:
		void
****************************************************************/
UINT32 VIM_HIF_WriteSramReverse(UINT32 adr, const HUGE UINT8 *buf, UINT32 size)
{
	UINT32 uActsize=0, uTmp=0,uWaitlen=VIM_WAITMAX;

	adr >>= 2;		/*!< address must 4 byte align, read buffer */
	adr <<= 2;
	adr+=V5_1TSRAM_BASE;
	
	size >>= 2;
	uActsize = size << 2;

	while(uWaitlen--)
	{
		if(VIM_HIF_GetReg32(V5_REG_BIU_STATUS)&0x1)		/*!< wait untile ready */
			break;
	}
#if(VIM_HIF_DEBUG)&&(VIM_5XRDK_ERROR)
	if(!uWaitlen)
		VIM_USER_PrintString("[VC0578B][BIU] BIU is not ready for write [ERROR] ");
#endif
	adr|=1;    //write enable
	VIM_HIF_SetReg32(V5_REG_BIU_BASE_ADDR, adr);				/*!< set burst base address */
	VIM_HIF_SetReg32(V5_REG_BIU_BURST_LEN, size);				/*!< set burst length, word size */

	//这里需要delay 50个hclock
	for(g_delayfor=0; g_delayfor<VIM_READRAM_DELAYFOR; g_delayfor++);

	/*! read data continously */
	//gVc0578b_Isr.byIntFlag=1;
	VIM_SET_RSHIGH_BY_GPIO();
	while(size--)
	{
		uTmp = (UINT32)(buf[2]<<8);
		uTmp |= (UINT32)buf[3];
		*(volatile UINT16*)(VIM_REG_VALUE) = uTmp;
		uTmp = (UINT32)(buf[0]<<8);
		uTmp |= (UINT32)buf[1];
		*(volatile UINT16*)(VIM_REG_VALUE) = uTmp;		
		buf+=4;
	}
	//gVc0578b_Isr.byIntFlag=0;
	while(uWaitlen--)
	{
		if(VIM_HIF_GetReg32(V5_REG_BIU_STATUS)&0x1)		/*!< wait untile ready */
			break;
	}
	return uActsize;
}
/***************************************************************
Description:
		write sdram 8bit

Parameters:
		adr: sdram start address
		buf: the head point of buf
		size:	  the lenght of data(byte)

Returns:
		void
****************************************************************/

 UINT32 VIM_HIF_WriteSram(UINT32 adr, const HUGE UINT8 *buf, UINT32 size,VIM_HIF_SRAMMODE Mode)
{
	UINT32 uActsize=0, uTmp=0,uWaitlen=VIM_WAITMAX;

	adr >>= 2;		/*!< address must 4 byte align, read buffer */
	adr <<= 2;
	adr+=V5_1TSRAM_BASE;
	
	size >>= 2;
	uActsize = size << 2;

	while(uWaitlen--)
	{
		if(VIM_HIF_GetReg32(V5_REG_BIU_STATUS)&0x1)		/*!< wait untile ready */
			break;
	}
#if(VIM_HIF_DEBUG)&&(VIM_5XRDK_ERROR)
	if(!uWaitlen)
		VIM_USER_PrintString("[VC0578B][BIU] BIU is not ready for write [ERROR] ");
#endif
	adr|=1;    //write enable
	VIM_HIF_SetReg32(V5_REG_BIU_BASE_ADDR, adr|Mode);				/*!< set burst base address */
	VIM_HIF_SetReg32(V5_REG_BIU_BURST_LEN, size);				/*!< set burst length, word size */

	//这里需要delay 50个hclock
	for(g_delayfor=0; g_delayfor<VIM_READRAM_DELAYFOR; g_delayfor++);

	/*! read data continously */
	//gVc0578b_Isr.byIntFlag=1;
	VIM_SET_RSHIGH_BY_GPIO();
	while(size--)
	{
		uTmp = (UINT32)(buf[3]<<8);
		uTmp |= (UINT32)buf[2];
		*(volatile UINT16*)(VIM_REG_VALUE) = uTmp;
		uTmp = (UINT32)(buf[1]<<8);
		uTmp |= (UINT32)buf[0];
		*(volatile UINT16*)(VIM_REG_VALUE) = uTmp;		
		buf+=4;
	}
	//gVc0578b_Isr.byIntFlag=0;
	while(uWaitlen--)
	{
		if(VIM_HIF_GetReg32(V5_REG_BIU_STATUS)&0x1)		/*!< wait untile ready */
			break;
	}
	return uActsize;
}
#if VIM_USER_SUPPORT_SDRAM
/***************************************************************
Description:
		write sdram 8bit

Parameters:
		adr: sdram start address
		buf: the head point of buf
		size:	  the lenght of data(byte)

Returns:
		void
****************************************************************/

 UINT32 VIM_HIF_WriteSdram(UINT32 adr, const HUGE UINT8 *buf, UINT32 size,VIM_HIF_SRAMMODE Mode)
{
	UINT32 uActsize=0, uTmp=0,uWaitlen=VIM_WAITMAX;

	adr >>= 2;		/*!< address must 4 byte align, read buffer */
	adr <<= 2;
	adr+=V5_SDRAM_BASE;
	
	size >>= 2;
	uActsize = size << 2;

	while(uWaitlen--)
	{
		if(VIM_HIF_GetReg32(V5_REG_BIU_STATUS)&0x1)		/*!< wait untile ready */
			break;
	}
#if(VIM_HIF_DEBUG)&&(VIM_5XRDK_ERROR)
	if(!uWaitlen)
		VIM_USER_PrintString("[VC0578B][BIU] BIU is not ready for write [ERROR] ");
#endif
	adr|=1;    //write enable
	VIM_HIF_SetReg32(V5_REG_BIU_BASE_ADDR, adr|Mode);				/*!< set burst base address */
	VIM_HIF_SetReg32(V5_REG_BIU_BURST_LEN, size);				/*!< set burst length, word size */

	//这里需要delay 50个hclock
	for(g_delayfor=0; g_delayfor<VIM_READRAM_DELAYFOR; g_delayfor++);

	/*! read data continously */
	//gVc0578b_Isr.byIntFlag=1;
	VIM_SET_RSHIGH_BY_GPIO();
	while(size--)
	{
		uTmp = (UINT32)(buf[3]<<8);
		uTmp |= (UINT32)buf[2];
		*(volatile UINT16*)(VIM_REG_VALUE) = uTmp;
		uTmp = (UINT32)(buf[1]<<8);
		uTmp |= (UINT32)buf[0];
		*(volatile UINT16*)(VIM_REG_VALUE) = uTmp;		
		buf+=4;
	}
	//gVc0578b_Isr.byIntFlag=0;
	while(uWaitlen--)
	{
		if(VIM_HIF_GetReg32(V5_REG_BIU_STATUS)&0x1)		/*!< wait untile ready */
			break;
	}
	return uActsize;
}
/***************************************************************
Description:
		read sdram 8bit

Parameters:
		adr: sdram start address
		buf: the head point of buf
		size:	  the lenght of data(byte)

Returns:
		void
****************************************************************/

UINT32 VIM_HIF_ReadSdram(UINT32 adr,  HUGE UINT8 *buf, UINT32 size,VIM_HIF_SRAMMODE Mode)
{
	UINT32 uActsize=0, uTmp=0,uWaitlen=VIM_WAITMAX;

	adr >>= 2;		/*!< address must 4 byte align, read buffer */
	adr <<= 2;
	adr+=V5_SDRAM_BASE;
	
	size >>= 2;
	uActsize = size << 2;

	while(uWaitlen--)
	{
		if(VIM_HIF_GetReg32(V5_REG_BIU_STATUS)&0x1)		/*!< wait untile ready */
			break;
	}
#if(VIM_HIF_DEBUG)&&(VIM_5XRDK_ERROR)
	if(!uWaitlen)
		VIM_USER_PrintString("[VC0578B][BIU] BIU is not ready for read [ERROR] ");
#endif
				/*!< wait untile ready */
	VIM_HIF_SetReg32(V5_REG_BIU_BASE_ADDR, adr|Mode);				/*!< set burst base address */
	VIM_HIF_SetReg32(V5_REG_BIU_BURST_LEN, size);				/*!< set burst length, word size */

	//这里需要delay 50个hclock
	for(g_delayfor=0; g_delayfor<VIM_READRAM_DELAYFOR; g_delayfor++);

	//gVc0578b_Isr.byIntFlag=1;
	/*! read data continously */
	VIM_SET_RSHIGH_BY_GPIO();
	while(size--)
	{
		uTmp = *(volatile UINT16*)(VIM_REG_VALUE);
		buf[2] = (UINT8)uTmp;
		buf[3] = (UINT8)(uTmp>>8);
		uTmp = *(volatile UINT16*)(VIM_REG_VALUE);
		buf[0] = (UINT8)uTmp;
		buf[1] = (UINT8)(uTmp>>8);
		buf+=4;
	}
	//gVc0578b_Isr.byIntFlag=0;
	while(uWaitlen--)
	{
		if(VIM_HIF_GetReg32(V5_REG_BIU_STATUS)&0x1)		/*!< wait untile ready */
			break;
	}
	return uActsize;
}
#endif
/***************************************************************
Description:
		write sdram 8bit on word

Parameters:
		adr: sdram start address
		buf: the head point of buf
		size:	  the lenght of data(byte)

Returns:
		void
****************************************************************/
// size in byte unit
UINT32 VIM_HIF_WriteSramOnWord(UINT32 adr, UINT16 dat, UINT32 size)
{
	UINT32 uActsize=0, uWaitlen=VIM_WAITMAX;

	adr >>= 2;		/*!< address must 4 byte align, read buffer */
	adr <<= 2;
	adr+=V5_1TSRAM_BASE;
	
	size >>= 2;
	uActsize = size << 2;

	while(uWaitlen--)
	{
		if(VIM_HIF_GetReg32(V5_REG_BIU_STATUS)&0x1)		/*!< wait untile ready */
			break;
	}
#if(VIM_HIF_DEBUG)&&(VIM_5XRDK_ERROR)
	if(!uWaitlen)
		VIM_USER_PrintString("[VC0578B][BIU] BIU is not ready for write [ERROR] ");
#endif
	adr|=1;    //write enable
	VIM_HIF_SetReg32(V5_REG_BIU_BASE_ADDR, adr);				/*!< set burst base address */
	VIM_HIF_SetReg32(V5_REG_BIU_BURST_LEN, size);				/*!< set burst length, word size */

	//这里需要delay 50个hclock
	for(g_delayfor=0; g_delayfor<VIM_READRAM_DELAYFOR; g_delayfor++);

	/*! read data continously */
	//gVc0578b_Isr.byIntFlag=1;
	VIM_SET_RSHIGH_BY_GPIO();
	while(size--)
	{
		*(volatile UINT16*)(VIM_REG_VALUE) = dat;
		*(volatile UINT16*)(VIM_REG_VALUE) = dat;		
	}
	//gVc0578b_Isr.byIntFlag=0;
	return uActsize;
}


/***************************************************************
Description:
		set multi 16 mode

Parameters:
		Timing:
			abi delay (0~32ns)max is 0x18
			wr delay (0~32ns)max is 0xb
			cs delay (0~32ns)max is 0x6

Returns:
		void
****************************************************************/

void VIM_HIF_SetMulti16(void)
{
UINT32	temp,i;
	//VIM_SET_NORMAL_BY_GPIO();

	VIM_SET_RSLOW_BY_GPIO();
	*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((V5_REG_CPM_ABI_TEST>>24)&0xff);
	*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((V5_REG_CPM_ABI_TEST>>16)&0xff);
	*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((V5_REG_CPM_ABI_TEST>>8)&0xff);
	*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((V5_REG_CPM_ABI_TEST&0xfc));			
	VIM_SET_RSHIGH_BY_GPIO();
	for(i=0; i<10; i++);
		
	temp = ((UINT32)(*(volatile UINT8 *)VIM_REG_VALUE)<<24);
	temp |= ((UINT32)(*(volatile UINT8 *)VIM_REG_VALUE)<<16);
	temp |= ((UINT32)(*(volatile UINT8 *)VIM_REG_VALUE)<<8);
	temp |= ((UINT32)(*(volatile UINT8 *)VIM_REG_VALUE));
	
#if(VIM_HIF_DEBUG)&&(VIM_5XRDK_DEBUG)
		VIM_USER_PrintHex("V5_REG_CPM_ABI_TEST=",temp);
#endif

	VIM_SET_RSLOW_BY_GPIO();
	*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((V5_REG_CPM_ABI_CFG>>24)&0xff);
	*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((V5_REG_CPM_ABI_CFG>>16)&0xff);
	*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((V5_REG_CPM_ABI_CFG>>8)&0xff);
	*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((V5_REG_CPM_ABI_CFG&0xfc));			/*!< 地址是4的倍数 */
	VIM_SET_RSHIGH_BY_GPIO();
	for(i=0; i<10; i++);
		
	temp = ((UINT32)(*(volatile UINT8 *)VIM_REG_VALUE)<<24);
	temp |= ((UINT32)(*(volatile UINT8 *)VIM_REG_VALUE)<<16);
	temp |= ((UINT32)(*(volatile UINT8 *)VIM_REG_VALUE)<<8);
	temp |= ((UINT32)(*(volatile UINT8 *)VIM_REG_VALUE));

#if(VIM_HIF_DEBUG)&&(VIM_5XRDK_DEBUG)
	VIM_USER_PrintHex("V5_REG_CPM_ABI_CFG=",temp);
#endif

	VIM_SET_RSLOW_BY_GPIO();
	*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((V5_REG_CPM_ABI_CFG&0xff000000)>>24);
	*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((V5_REG_CPM_ABI_CFG&0xff0000)>>16);
	*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((V5_REG_CPM_ABI_CFG&0xff00)>>8);
	*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((V5_REG_CPM_ABI_CFG&0xfc)|1);	//地址是4的倍数

	VIM_SET_RSHIGH_BY_GPIO();

	*(volatile UINT8 *)(VIM_REG_VALUE) = (UINT8)(0x01);    	//set multi16	
	*(volatile UINT8 *)(VIM_REG_VALUE) = (UINT8)(0x18);    		
	*(volatile UINT8 *)(VIM_REG_VALUE) = (UINT8)(0x0b);    		
	*(volatile UINT8 *)(VIM_REG_VALUE) = (UINT8)(0x06);     


	temp=VIM_HIF_GetReg32(V5_REG_CPM_ABI_CFG);

	
#if(VIM_HIF_DEBUG)&&(VIM_5XRDK_DEBUG)
		VIM_USER_PrintHex("V5_REG_CPM_ABI_CFG=",temp);
#endif

	//VIM_SET_BYPASS_BY_GPIO();
}
#endif

#elif VIM_BUSTYPE == VIM_MULTI8
/***************************************************************
Description:
		set 8bit register use multi8

Parameters:
		adr: register address
		val:	   register value

Returns:
		void
****************************************************************/
void VIM_HIF_SetReg8(UINT32 adr, UINT8 val)     
{
	UINT32 uTemp=0;
	
	//gVc0578b_Isr.byIntFlag=1;
	VIM_SET_RSLOW_BY_GPIO();
	*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((adr>>24)&0xff);
	*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((adr>>16)&0xff);
	*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((adr>>8)&0xff);
	*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((adr&0xfc));			/*!< 地址是4的倍数 */
	VIM_SET_RSHIGH_BY_GPIO();

	for(g_delayfor=0; g_delayfor<VIM_READREG_DELAYFOR; g_delayfor++);
	
	uTemp = ((UINT32)(*(volatile UINT8 *)VIM_REG_VALUE)<<24);
	uTemp |= ((UINT32)(*(volatile UINT8 *)VIM_REG_VALUE)<<16);
	uTemp |= ((UINT32)(*(volatile UINT8 *)VIM_REG_VALUE)<<8);
	uTemp |= ((UINT32)(*(volatile UINT8 *)VIM_REG_VALUE));

	VIM_SET_RSLOW_BY_GPIO();
	*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((adr>>24)&0xff);
	*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((adr>>16)&0xff);
	*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((adr>>8)&0xff);
	*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((adr&0xfc)|1);		/*!< 地址是4的倍数 */
	VIM_SET_RSHIGH_BY_GPIO();
	
	switch(adr&0x03)
	{
	case 0:	uTemp&=0xffffff00;uTemp|=(UINT32)val;		break;
	case 1: uTemp&=0xffff00ff;uTemp|=((UINT32)val<<8);	break;
	case 2:	uTemp&=0xff00ffff;uTemp|=((UINT32)val<<16);	break;
	case 3:	uTemp&=0x00ffffff;uTemp|=((UINT32)val<<24);	break;
	}
	*(volatile UINT8 *)(VIM_REG_VALUE) = (UINT8)((uTemp>>24)&0xff);
	*(volatile UINT8 *)(VIM_REG_VALUE) = (UINT8)((uTemp>>16)&0xff);
	*(volatile UINT8 *)(VIM_REG_VALUE) = (UINT8)((uTemp>>8)&0xff);
	*(volatile UINT8 *)(VIM_REG_VALUE) = (UINT8)((uTemp)&0xff);
	
	//gVc0578b_Isr.byIntFlag=0;
}
/***************************************************************
Description:
		get 8bit register use multi8

Parameters:
		adr: register address
		val:	   register value

Returns:
		return uint 8
****************************************************************/
UINT8 VIM_HIF_GetReg8(UINT32 adr)
{
	UINT32 uTemp=0;
	//gVc0578b_Isr.byIntFlag=1;
	VIM_SET_RSLOW_BY_GPIO();
	*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((adr>>24)&0xff);
	*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((adr>>16)&0xff);
	*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((adr>>8)&0xff);
	*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((adr&0xfc));			/*!< 地址是4的倍数 */
	VIM_SET_RSHIGH_BY_GPIO();
	for(g_delayfor=0; g_delayfor<VIM_READREG_DELAYFOR; g_delayfor++);
		
	uTemp = ((UINT32)(*(volatile UINT8 *)VIM_REG_VALUE)<<24);
	uTemp |= ((UINT32)(*(volatile UINT8 *)VIM_REG_VALUE)<<16);
	uTemp |= ((UINT32)(*(volatile UINT8 *)VIM_REG_VALUE)<<8);
	uTemp |= ((UINT32)(*(volatile UINT8 *)VIM_REG_VALUE));
	//gVc0578b_Isr.byIntFlag=0;
	if((adr & 0x3) == 0){
		return (UINT8)uTemp;
	}else if((adr & 0x3) == 1){
		return (UINT8)(uTemp>>8);
	}else if((adr & 0x3) == 2){
		return (UINT8)(uTemp>>16);
	}else{
		return (UINT8)(uTemp>>24);
	}	
}
/***************************************************************
Description:
		set 16bit register use multi8

Parameters:
		adr: register address
		val:	   register value

Returns:
		void
****************************************************************/
void VIM_HIF_SetReg16(UINT32 adr, UINT16 val)       
{
	UINT32 uTemp=0;
	//gVc0578b_Isr.byIntFlag=1;
	VIM_SET_RSLOW_BY_GPIO();
	*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((adr>>24)&0xff);
	*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((adr>>16)&0xff);
	*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((adr>>8)&0xff);
	*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((adr&0xfc));			/*!< 地址是4的倍数 */
	VIM_SET_RSHIGH_BY_GPIO();

	for(g_delayfor=0; g_delayfor<VIM_READREG_DELAYFOR; g_delayfor++);
	
	uTemp = ((UINT32)(*(volatile UINT8 *)VIM_REG_VALUE)<<24);
	uTemp |= ((UINT32)(*(volatile UINT8 *)VIM_REG_VALUE)<<16);
	uTemp |= ((UINT32)(*(volatile UINT8 *)VIM_REG_VALUE)<<8);
	uTemp |= ((UINT32)(*(volatile UINT8 *)VIM_REG_VALUE));

	VIM_SET_RSLOW_BY_GPIO();
	*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((adr>>24)&0xff);
	*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((adr>>16)&0xff);
	*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((adr>>8)&0xff);
	*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((adr&0xfc)|1);				/*!< 地址是4的倍数 */
	VIM_SET_RSHIGH_BY_GPIO();
	
	if((adr & 0x3) == 0){	
		*(volatile UINT8 *)(VIM_REG_VALUE) = (UINT8)((uTemp>>24)&0xff);    		
		*(volatile UINT8 *)(VIM_REG_VALUE) = (UINT8)((uTemp>>16)&0xff);    		
		*(volatile UINT8 *)(VIM_REG_VALUE) = (UINT8)((val>>8)&0xff);    		
		*(volatile UINT8 *)(VIM_REG_VALUE) = (UINT8)(val&0xff);     
	}else if((adr & 0x3) == 1){
		*(volatile UINT8 *)(VIM_REG_VALUE) = (UINT8)((uTemp&0xff000000)>>24);    		
		*(volatile UINT8 *)(VIM_REG_VALUE) = (UINT8)((val&0xff00)>>8);    		
		*(volatile UINT8 *)(VIM_REG_VALUE) = (UINT8)(val);  
		*(volatile UINT8 *)(VIM_REG_VALUE) = (UINT8)(uTemp);     
	}else if((adr & 0x3) == 2){
		*(volatile UINT8 *)(VIM_REG_VALUE) = (UINT8)((val&0xff00)>>8);    		
		*(volatile UINT8 *)(VIM_REG_VALUE) = (UINT8)(val);  
		*(volatile UINT8 *)(VIM_REG_VALUE) = (UINT8)((uTemp&0xff00)>>8);    		
		*(volatile UINT8 *)(VIM_REG_VALUE) = (UINT8)(uTemp);  	
	}else if((adr & 0x3) == 3){
		*(volatile UINT8 *)(VIM_REG_VALUE) = (UINT8)(val);  
		*(volatile UINT8 *)(VIM_REG_VALUE) = (UINT8)((uTemp&0xff0000)>>16);
		*(volatile UINT8 *)(VIM_REG_VALUE) = (UINT8)((uTemp&0xff00)>>8);	
		*(volatile UINT8 *)(VIM_REG_VALUE) = (UINT8)(uTemp);  

		VIM_SET_RSLOW_BY_GPIO();
		*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((adr&0xff000000)>>24);
		*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((adr&0xff0000)>>16);
		*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((adr&0xff00)>>8);
		*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((adr+3)&0xfc);				/*!< 地址是4的倍数 */
		VIM_SET_RSHIGH_BY_GPIO();
		
		uTemp = (UINT32)(*(volatile UINT8 *)(VIM_REG_VALUE)<<24);
		uTemp |= (UINT32)(*(volatile UINT8 *)(VIM_REG_VALUE)<<16);
		uTemp |=(UINT32)(*(volatile UINT8 *)(VIM_REG_VALUE)<<8);
		uTemp |= (UINT32)(*(volatile UINT8 *)(VIM_REG_VALUE));

		VIM_SET_RSLOW_BY_GPIO();
		*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((adr&0xff000000)>>24);
		*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((adr&0xff0000)>>16);
		*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((adr&0xff00)>>8);
		*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)(((adr+3)&0xfc)|1);			/*!< 地址是4的倍数 */
		VIM_SET_RSHIGH_BY_GPIO();

		*(volatile UINT8 *)(VIM_REG_VALUE) = (UINT8)((uTemp&0xff000000)>>24);
		*(volatile UINT8 *)(VIM_REG_VALUE) = (UINT8)((uTemp&0xff0000)>>16);	
		*(volatile UINT8 *)(VIM_REG_VALUE) = (UINT8)((uTemp&0xff00)>>8);
		*(volatile UINT8 *)(VIM_REG_VALUE) = (UINT8)((val&0xff00)>>8);
	}	
	//gVc0578b_Isr.byIntFlag=0;
}
/***************************************************************
Description:
		get 16bit register use multi8

Parameters:
		adr: register address
		val:	   register value

Returns:
		return uint 16 
****************************************************************/
UINT16 VIM_HIF_GetReg16(UINT32 adr)
{
	UINT32 uTemp=0,uTemp1=0;
	//gVc0578b_Isr.byIntFlag=1;
	VIM_SET_RSLOW_BY_GPIO();
	*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((adr>>24)&0xff);
	*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((adr>>16)&0xff);
	*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((adr>>8)&0xff);
	*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((adr&0xfc));			/*!< 地址是4的倍数 */

	VIM_SET_RSHIGH_BY_GPIO();
	
	for(g_delayfor=0; g_delayfor<VIM_READREG_DELAYFOR; g_delayfor++);
	
	uTemp = (UINT32)(*(volatile UINT8 *)(VIM_REG_VALUE)<<24);         
	uTemp |= (UINT32)(*(volatile UINT8 *)(VIM_REG_VALUE)<<16);
	uTemp |= (UINT32)(*(volatile UINT8 *)(VIM_REG_VALUE)<<8);  
	uTemp |= (UINT32)(*(volatile UINT8 *)(VIM_REG_VALUE));   

	//gVc0578b_Isr.byIntFlag=0;
	if((adr & 0x3) == 0)
	{	
		return (UINT16)uTemp;
	}
	else if((adr & 0x3) == 1)
	{
		return (UINT16)(uTemp>>8);
	}
	else if((adr & 0x3) == 2)
	{
		return (UINT16)(uTemp>>16);
	}	
	else 
	{
		uTemp>>=24;		//保留高8位
		//gVc0578b_Isr.byIntFlag=1;
		VIM_SET_RSLOW_BY_GPIO();
		adr += 3;
		adr >>= 2;
		adr <<= 2;
		*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((adr>>24)&0xff);
		*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((adr>>16)&0xff);
		*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((adr>>8)&0xff);
		*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((adr&0xfc));			/*!< 地址是4的倍数 */
		VIM_SET_RSHIGH_BY_GPIO();
		
		for(g_delayfor=0; g_delayfor<VIM_READREG_DELAYFOR; g_delayfor++);

		uTemp1 = (UINT32)(*(volatile UINT8 *)(VIM_REG_VALUE)<<24);
		uTemp1 |= (UINT32)(*(volatile UINT8 *)(VIM_REG_VALUE)<<16);
		uTemp1 |= (UINT32)(*(volatile UINT8 *)(VIM_REG_VALUE)<<8);  
		uTemp1 |= (UINT32)(*(volatile UINT8 *)(VIM_REG_VALUE)); 
		//gVc0578b_Isr.byIntFlag=0;
		return (UINT16)(uTemp|((uTemp1&0xff)<<8));
		
	}	
}


/***************************************************************
Description:
		set 32bit register use multi8

Parameters:
		adr: register address
		val:	   register value

Returns:
		void
****************************************************************/
void VIM_HIF_SetReg32(UINT32 adr, UINT32 val)       
{
	//gVc0578b_Isr.byIntFlag=1;
	if((adr&0x3)==0){
		VIM_SET_RSLOW_BY_GPIO();
		*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((adr>>24)&0xff);
		*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((adr>>16)&0xff);
		*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((adr>>8)&0xff);
		*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((adr&0xfc)|1);				/*!< 地址是4的倍数 */
		VIM_SET_RSHIGH_BY_GPIO();
		
		*(volatile UINT8 *)(VIM_REG_VALUE) = (UINT8)((val&0xff000000)>>24);    		
		*(volatile UINT8 *)(VIM_REG_VALUE) = (UINT8)((val&0xff0000)>>16);    		
		*(volatile UINT8 *)(VIM_REG_VALUE) = (UINT8)((val&0xff00)>>8);    		
		*(volatile UINT8 *)(VIM_REG_VALUE) = (UINT8)(val);     
	}else{
		VIM_HIF_SetReg16(adr,(UINT16)val);
		VIM_HIF_SetReg16(adr+2,(UINT16)(val>>16));
	}
	//gVc0578b_Isr.byIntFlag=0;
}
/***************************************************************
Description:
		get 32bit register use multi16

Parameters:
		adr: register address
		val:	   register value

Returns:
		return uint 16
****************************************************************/
UINT32 VIM_HIF_GetReg32(UINT32 adr)
{
	UINT32 uTemp=0;
	//gVc0578b_Isr.byIntFlag=1;
	if((adr&0x3)==0){
		VIM_SET_RSLOW_BY_GPIO();
		*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((adr>>24)&0xff);
		*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((adr>>16)&0xff);
		*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((adr>>8)&0xff);
		*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)(adr&0xfc);					/*!< 地址是4的倍数 */
		VIM_SET_RSHIGH_BY_GPIO();
		
		for(g_delayfor=0; g_delayfor<VIM_READREG_DELAYFOR; g_delayfor++);
		
		uTemp = (UINT32)(*(volatile UINT8 *)(VIM_REG_VALUE)<<24);
		uTemp |= (UINT32)(*(volatile UINT8 *)(VIM_REG_VALUE)<<16);
		uTemp |= (UINT32)(*(volatile UINT8 *)(VIM_REG_VALUE)<<8);
		uTemp |= (UINT32)(*(volatile UINT8 *)(VIM_REG_VALUE));
	}else
	{
		uTemp=(VIM_HIF_GetReg16(adr+2)<<16);
		uTemp|=VIM_HIF_GetReg16(adr);
	}
	//gVc0578b_Isr.byIntFlag=0;
	return uTemp;
}

/***************************************************************
Description:
		set bypass register use multi8

Parameters:
		adr: register address
		val:	   register value

Returns:
		void
****************************************************************/
void VIM_HIF_SetSpecialReg(UINT8 val)       
{
	//gVc0578b_Isr.byIntFlag=1;
	*(volatile UINT8 *)(VIM_REG_VALUE) = (UINT8)((VIM_BYPASS_VALUE&0xff000000)>>24);    		
	*(volatile UINT8 *)(VIM_REG_VALUE) = (UINT8)((VIM_BYPASS_VALUE&0xff0000)>>16);    		
	*(volatile UINT8 *)(VIM_REG_VALUE) = (UINT8)((VIM_BYPASS_VALUE&0xff00)>>8);    		
	*(volatile UINT8 *)(VIM_REG_VALUE) = (UINT8)(val);     

	
	*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((VIM_BYPASS_ADDR&0xff000000)>>24);
	*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((VIM_BYPASS_ADDR&0xff0000)>>16);
	*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((VIM_BYPASS_ADDR&0xff00)>>8);
	*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)(VIM_BYPASS_ADDR&0xfc);	//地址是4的倍数
	//gVc0578b_Isr.byIntFlag=0;
}
/***************************************************************
Description:
		read sdram 8bit

Parameters:
		adr: sdram start address
		buf: the head point of buf
		size:	  the lenght of data(byte)

Returns:
		void
****************************************************************/
 UINT32 VIM_HIF_ReadSram(UINT32 adr, HUGE UINT8 *buf, UINT32 size,VIM_HIF_SRAMMODE Mode)
{
	UINT32 uActsize=0;
	SINT32 uWaitlen=VIM_WAITMAX;

	adr >>= 2;		/*!< address must 4 byte align, read buffer */
	adr <<= 2;
	adr+=V5_1TSRAM_BASE;

	size >>= 2;
	uActsize = size << 2;

	while(uWaitlen--)
	{
		if(VIM_HIF_GetReg32(V5_REG_BIU_STATUS)&0x1)		/*!< wait untile ready */
			break;
	}
#if(VIM_HIF_DEBUG)&&(VIM_5XRDK_ERROR)
	if(!uWaitlen)
		VIM_USER_PrintString("[VC0578B][BIU] BIU is not ready for read [ERROR] ");
#endif
				/*!< wait untile ready */
	VIM_HIF_SetReg32(V5_REG_BIU_BASE_ADDR, adr|Mode);				/*!< set burst base address */
	VIM_HIF_SetReg32(V5_REG_BIU_BURST_LEN, size);				/*!< set burst length, word size */

	//这里需要delay 50个hclock
	for(g_delayfor=0; g_delayfor<VIM_READREG_DELAYFOR; g_delayfor++);

	/*! read data continously */
	//gVc0578b_Isr.byIntFlag=1;
	VIM_SET_RSHIGH_BY_GPIO();
	while(size--)
	{
		buf[3]  = *((volatile UINT8*)(VIM_REG_VALUE));
		buf[2]  = *((volatile UINT8*)(VIM_REG_VALUE));
		buf[1]  = *((volatile UINT8*)(VIM_REG_VALUE));
		buf[0]  = *((volatile UINT8*)(VIM_REG_VALUE));

		buf+=4;
	}
	//gVc0578b_Isr.byIntFlag=0;
	while(uWaitlen--)
	{
		if(VIM_HIF_GetReg32(V5_REG_BIU_STATUS)&0x1)		/*!< wait untile ready */
			break;
	}
	return uActsize;
}
/***************************************************************
Description:
		write sdram 8bit reverse

Parameters:
		adr: sdram start address
		buf: the head point of buf
		size:	  the lenght of data(byte)

Returns:
		void
****************************************************************/
UINT32 VIM_HIF_WriteSramReverse(UINT32 adr, const HUGE UINT8 *buf, UINT32 size)
{
	UINT32 uActsize=0, uWaitlen=VIM_WAITMAX;

	adr >>= 2;		/*!< address must 4 byte align, read buffer */
	adr <<= 2;
	adr+=V5_1TSRAM_BASE;

	size >>= 2;
	uActsize = size << 2;

	while(uWaitlen--)
	{
		if((VIM_HIF_GetReg8(V5_REG_BIU_STATUS) & 0x1))
			break;
	}
#if(VIM_HIF_DEBUG)&&(VIM_5XRDK_ERROR)
	if(!uWaitlen)
		VIM_USER_PrintString("[VC0578B][BIU] BIU is not ready for write [ERROR] ");
#endif
	adr|=1;    //write enable
	VIM_HIF_SetReg32(V5_REG_BIU_BASE_ADDR, adr);				/*!< set burst base address */
	VIM_HIF_SetReg32(V5_REG_BIU_BURST_LEN, size);				/*!< set burst length, word size */

	//这里需要delay 50个hclock
	for(g_delayfor=0; g_delayfor<VIM_READREG_DELAYFOR; g_delayfor++);

	/*! read data continously */
	//gVc0578b_Isr.byIntFlag=1;
	VIM_SET_RSHIGH_BY_GPIO();
	while(size--)
	{
		*(volatile UINT8*)(VIM_REG_VALUE) = buf[2];
		*(volatile UINT8*)(VIM_REG_VALUE) = buf[3];
		*(volatile UINT8*)(VIM_REG_VALUE) = buf[0];
		*(volatile UINT8*)(VIM_REG_VALUE) = buf[1];
		buf+=4;
	}
	//gVc0578b_Isr.byIntFlag=0;
	while(uWaitlen--)
	{
		if(VIM_HIF_GetReg32(V5_REG_BIU_STATUS)&0x1)		/*!< wait untile ready */
			break;
	}
	return uActsize;
}
/***************************************************************
Description:
		write sdram 8bit

Parameters:
		adr: sdram start address
		buf: the head point of buf
		size:	  the lenght of data(byte)

Returns:
		void
****************************************************************/
UINT32 VIM_HIF_WriteSram(UINT32 adr, const HUGE UINT8 *buf, UINT32 size,VIM_HIF_SRAMMODE Mode)
{
	UINT32 uActsize=0, uWaitlen=VIM_WAITMAX;

	adr >>= 2;		/*!< address must 4 byte align, read buffer */
	adr <<= 2;
	adr+=V5_1TSRAM_BASE;
	
	size >>= 2;
	uActsize = size << 2;

	while(uWaitlen--)
	{
		if(VIM_HIF_GetReg32(V5_REG_BIU_STATUS)&0x1)		/*!< wait untile ready */
			break;
	}
#if(VIM_HIF_DEBUG)&&(VIM_5XRDK_ERROR)
	if(!uWaitlen)
		VIM_USER_PrintString("[VC0578B][BIU] BIU is not ready for write [ERROR] ");
#endif
	adr|=1;    //write enable
	VIM_HIF_SetReg32(V5_REG_BIU_BASE_ADDR, adr|Mode);				/*!< set burst base address */
	VIM_HIF_SetReg32(V5_REG_BIU_BURST_LEN, size);				/*!< set burst length, word size */

	//这里需要delay 50个hclock
	for(g_delayfor=0; g_delayfor<VIM_READREG_DELAYFOR; g_delayfor++);

	/*! read data continously */
	//gVc0578b_Isr.byIntFlag=1;
	VIM_SET_RSHIGH_BY_GPIO();
	while(size--)
	{
		*(volatile UINT8*)(VIM_REG_VALUE) = buf[3];
		*(volatile UINT8*)(VIM_REG_VALUE) = buf[2];
		*(volatile UINT8*)(VIM_REG_VALUE) = buf[1];
		*(volatile UINT8*)(VIM_REG_VALUE) = buf[0];
		buf+=4;
	}
	//gVc0578b_Isr.byIntFlag=0;
	while(uWaitlen--)
	{
		if(VIM_HIF_GetReg32(V5_REG_BIU_STATUS)&0x1)		/*!< wait untile ready */
			break;
	}
	return uActsize;
}
/***************************************************************
Description:
		write sdram 8bit on word

Parameters:
		adr: sdram start address
		buf: the head point of buf
		size:	  the lenght of data(byte)

Returns:
		void
****************************************************************/
// size in byte unit
UINT32 VIM_HIF_WriteSramOnWord(UINT32 adr, UINT16 dat, UINT32 size)
{
	UINT32 uActsize=0, uWaitlen=VIM_WAITMAX;

	adr >>= 2;		/*!< address must 4 byte align, read buffer */
	adr <<= 2;
	adr+=V5_1TSRAM_BASE;
	
	size >>= 2;
	uActsize = size << 2;

	while(uWaitlen--)
	{
		if(VIM_HIF_GetReg32(V5_REG_BIU_STATUS)&0x1)		/*!< wait untile ready */
			break;
	}
#if(VIM_HIF_DEBUG)&&(VIM_5XRDK_ERROR)
	if(!uWaitlen)
		VIM_USER_PrintString("[VC0578B][BIU] BIU is not ready for write [ERROR] ");
#endif
	adr|=1;    //write enable
	VIM_HIF_SetReg32(V5_REG_BIU_BASE_ADDR, adr);				/*!< set burst base address */
	VIM_HIF_SetReg32(V5_REG_BIU_BURST_LEN, size);				/*!< set burst length, word size */

	//这里需要delay 50个hclock
	for(g_delayfor=0; g_delayfor<VIM_READREG_DELAYFOR; g_delayfor++);

	/*! read data continously */
	//gVc0578b_Isr.byIntFlag=1;
	VIM_SET_RSHIGH_BY_GPIO();
	while(size--)
	{
		*(volatile UINT8*)(VIM_REG_VALUE) = (UINT8)(dat>>8);
		*(volatile UINT8*)(VIM_REG_VALUE) = (UINT8)(dat);		
		*(volatile UINT8*)(VIM_REG_VALUE) = (UINT8)(dat>>8);
		*(volatile UINT8*)(VIM_REG_VALUE) = (UINT8)(dat);	
	}
	//gVc0578b_Isr.byIntFlag=0;
	while(uWaitlen--)
	{
		if(VIM_HIF_GetReg32(V5_REG_BIU_STATUS)&0x1)		/*!< wait untile ready */
			break;
	}
	return uActsize;
}

#if VIM_USER_SUPPORT_SDRAM
/***************************************************************
Description:
		write sdram 8bit

Parameters:
		adr: sdram start address
		buf: the head point of buf
		size:	  the lenght of data(byte)
		Mode: fifo or normal sram

Returns:
		void
****************************************************************/
UINT32 VIM_HIF_WriteSdram(UINT32 adr, const HUGE UINT8 *buf, UINT32 size,VIM_HIF_SRAMMODE Mode)
{
	UINT32 uActsize=0, uTmp=0,uWaitlen=VIM_WAITMAX;
	SINT32 i=0;

	adr >>= 2;		/*!< address must 4 byte align, read buffer */
	adr <<= 2;
	adr+=V5_SDRAM_BASE;
	
	size >>= 2;
	uActsize = size << 2;

	while(uWaitlen--)
	{
		if(!(VIM_HIF_GetReg8(V5_REG_BIU_STATUS) & 0x1))
			break;
	}
#if(VIM_HIF_DEBUG)&&(VIM_5XRDK_ERROR)
	if(!uWaitlen)
		VIM_USER_PrintString("[VC0578B][BIU] BIU is not ready for write [ERROR] ");
#endif
	adr|=1;    //write enable
	VIM_HIF_SetReg32(V5_REG_BIU_BASE_ADDR, adr|Mode);				/*!< set burst base address */
	VIM_HIF_SetReg32(V5_REG_BIU_BURST_LEN, size);				/*!< set burst length, word size */

	//这里需要delay 50个hclock
	for(i=0;i<10;i++);

	/*! read data continously */
	VIM_SET_RSHIGH_BY_GPIO();
	while(size--)
	{
		uTmp = (UINT32)(buf[3]<<8);
		uTmp |= (UINT32)buf[2];
		*(volatile UINT16 *)(VIM_REG_INDEX|((VIM_READADD_LOCOSTO&0xfffc)<<1))= uTmp;
		uTmp = (UINT32)(buf[1]<<8);
		uTmp |= (UINT32)buf[0];
		*(volatile UINT16 *)(VIM_REG_INDEX|((VIM_READADD_LOCOSTO&0xfffc)<<1))= uTmp;		
		buf+=4;
	}
	while(uWaitlen--)
	{
		if(!(VIM_HIF_GetReg8(V5_REG_BIU_STATUS) & 0x1))
			break;
	}
	return uActsize;
}

/***************************************************************
Description:
		read sdram 8bit use locosto

Parameters:
		adr: sdram start address
		buf: the head point of buf
		size:	  the lenght of data(byte)
		Mode: fifo or normal sram
Returns:
		void
****************************************************************/
 UINT32 VIM_HIF_ReadSdram(UINT32 adr,  HUGE UINT8 *buf, UINT32 size,VIM_HIF_SRAMMODE Mode)
{
	UINT32 uActsize=0, uTmp=0,uWaitlen=VIM_WAITMAX;
	SINT32 i=0;

	adr >>= 2;		/*!< address must 4 byte align, read buffer */
	adr <<= 2;
	adr+=V5_SDRAM_BASE;
	
	size >>= 2;
	uActsize = size << 2;

	while(uWaitlen--)
	{
		if(!(VIM_HIF_GetReg8(V5_REG_BIU_STATUS) & 0x1))
			break;
	}
#if(VIM_HIF_DEBUG)&&(VIM_5XRDK_ERROR)
	if(!uWaitlen)
		VIM_USER_PrintString("[VC0578B][BIU] BIU is not ready for read [ERROR] ");
#endif
				/*!< wait untile ready */
	VIM_HIF_SetReg32(V5_REG_BIU_BASE_ADDR, adr|Mode);				/*!< set burst base address */
	VIM_HIF_SetReg32(V5_REG_BIU_BURST_LEN, size);				/*!< set burst length, word size */

	//这里需要delay 50个hclock
	for(i=0;i<10;i++);

	/*! read data continously */
	VIM_SET_RSHIGH_BY_GPIO();
	while(size--)
	{
		uTmp = *(volatile UINT16 *)(VIM_REG_INDEX|((VIM_READADD_LOCOSTO&0xfffc)<<1));
		buf[2] = (UINT8)uTmp;
		buf[3] = (UINT8)(uTmp>>8);
		uTmp = *(volatile UINT16 *)(VIM_REG_INDEX|((VIM_READADD_LOCOSTO&0xfffc)<<1));
		buf[0] = (UINT8)uTmp;
		buf[1] = (UINT8)(uTmp>>8);
		buf+=4;
	}
	while(uWaitlen--)
	{
		if(!(VIM_HIF_GetReg8(V5_REG_BIU_STATUS) & 0x1))
			break;
	}
	return uActsize;
}
/***************************************************************
Description:
		set multi 16 mode

Parameters:
		Timing:
			abi delay (0~32ns)max is 0x18
			wr delay (0~32ns)max is 0xb
			cs delay (0~32ns)max is 0x6

Returns:
		void
****************************************************************/

void VIM_HIF_SetMulti16(void)
{
	return;
}
#endif
#elif VIM_BUSTYPE == VIM_LOCOSTO
/***************************************************************
Description:
		set 8bit register use locosto

Parameters:
		adr: register address
		val:	   register value

Returns:
		void
****************************************************************/
 void VIM_HIF_SetReg8(UINT32 adr, UINT8 val)       
{
	UINT32 uTemp=0;

	uTemp=*(volatile UINT16 *)(VIM_REG_INDEX|(((adr>>16)&0xffff)<<1));
	uTemp=*(volatile UINT16 *)(VIM_REG_INDEX|((adr&0xfffc)<<1)) ;
	uTemp>>=16;
	uTemp|=*(volatile UINT16 *)(VIM_REG_INDEX|((VIM_READADD_LOCOSTO&0xfffc)<<1)) ;
	
	if((adr & 0x3) == 0)
	{
		*(volatile UINT16 *)(VIM_REG_INDEX|(((adr>>16)&0xffff)<<1)) = (UINT16)((uTemp&0xffff0000)>>16);
		*(volatile UINT16 *)(VIM_REG_INDEX|(((adr&0xfffc)|1)<<1)) = (UINT16)((uTemp&0xff00)|val);
	}
	else if((adr & 0x3) == 1)
	{
		*(volatile UINT16 *)(VIM_REG_INDEX|(((adr>>16)&0xffff)<<1)) = (UINT16)((uTemp&0xffff0000)>>16);
		*(volatile UINT16 *)(VIM_REG_INDEX|(((adr&0xfffc)|1)<<1)) =(UINT16) ((uTemp&0x00ff)|((UINT16)(val)<<8));
	}
	else if((adr & 0x3) == 2)
	{
		*(volatile UINT16 *)(VIM_REG_INDEX|(((adr>>16)&0xffff)<<1)) = (UINT16)(((uTemp&0xff000000)>>16)|val);     
		*(volatile UINT16 *)(VIM_REG_INDEX|(((adr&0xfffc)|1)<<1))= (UINT16)(uTemp&0xffff);    		
	}	
	else if((adr & 0x3) == 3)
	{
		*(volatile UINT16 *)(VIM_REG_INDEX|(((adr>>16)&0xffff)<<1))  = (UINT16)(((uTemp&0x00ff0000)>>16)|((UINT16)(val)<<8));     
		*(volatile UINT16 *)(VIM_REG_INDEX|(((adr&0xfffc)|1)<<1))= (UINT16)(uTemp&0xffff);    		
	}	
}
/***************************************************************
Description:
		get 8bit register use locosto

Parameters:
		adr: register address
		val:	   register value

Returns:
		return uint 8
****************************************************************/
UINT8 VIM_HIF_GetReg8(UINT32 adr)
{
UINT32 uTemp;

	uTemp=*(volatile UINT16 *)(VIM_REG_INDEX|(((adr>>16)&0xffff)<<1));
	uTemp=*(volatile UINT16 *)(VIM_REG_INDEX|((adr&0xfffc)<<1)) ;
	uTemp>>=16;
	uTemp|=*(volatile UINT16 *)(VIM_REG_INDEX|((VIM_READADD_LOCOSTO&0xfffc)<<1)) ;
	
	if((adr & 0x3) == 0)
	{
		return (UINT8)(uTemp&0xff);
	}
	else if((adr & 0x3) == 1)
	{
		return (UINT8)((uTemp&0xff00)>>8);
	}
	else if((adr & 0x3) == 2)
	{
		return (UINT8)((uTemp&0xff0000)>>16);
	}	
	else 
	{
		return (UINT8)((uTemp&0xff000000)>>24);
	}	

}

/***************************************************************
Description:
		set 16bit register use locosto

Parameters:
		adr: register address
		val:	   register value

Returns:
		void
****************************************************************/
void VIM_HIF_SetReg16(UINT32 adr, UINT16 val)       
{
	UINT32 uTemp;

	if((adr & 0x3) == 3)
	{
		VIM_HIF_SetReg8(adr,(UINT8)val);
		VIM_HIF_SetReg8(adr+1,(UINT8)(val>>8));
	}
	else
	{
		uTemp=*(volatile UINT16 *)(VIM_REG_INDEX|(((adr>>16)&0xffff)<<1));
		uTemp=*(volatile UINT16 *)(VIM_REG_INDEX|((adr&0xfffc)<<1)) ;
		uTemp>>=16;
		uTemp|=*(volatile UINT16 *)(VIM_REG_INDEX|((VIM_READADD_LOCOSTO&0xfffc)<<1)) ;

		if((adr & 0x3) == 0)
		{
			*(volatile UINT16 *)(VIM_REG_INDEX|(((adr>>16)&0xffff)<<1)) = (UINT16)((uTemp&0xffff0000)>>16);
			*(volatile UINT16 *)(VIM_REG_INDEX|(((adr&0xfffc)|1)<<1)) = (UINT16)(val);
		}
		else if((adr & 0x3) == 1)
		{
			*(volatile UINT16 *)(VIM_REG_INDEX|(((adr>>16)&0xffff)<<1))  = (UINT16)((uTemp&0xff000000)>>16)|((UINT16)(val&0xff00)>>8);    		
			*(volatile UINT16 *)(VIM_REG_INDEX|(((adr&0xfffc)|1)<<1))  = (UINT16)((uTemp&0x00ff)|((UINT16)(val&0xff)<<8));     
		}
		else if((adr & 0x3) == 2)
		{
			*(volatile UINT16 *)(VIM_REG_INDEX|(((adr>>16)&0xffff)<<1))  = (UINT16)(val);     
    			*(volatile UINT16 *)(VIM_REG_INDEX|(((adr&0xfffc)|1)<<1))   = (UINT16)(uTemp&0xffff);    		
		}	
		
	}
}

/***************************************************************
Description:
		get 16bit register use locosto

Parameters:
		adr: register address
		val:	   register value

Returns:
		return uint 16
****************************************************************/
UINT16 VIM_HIF_GetReg16(UINT32 adr)
{
	UINT32 uTemp;

	if((adr & 0x3) == 3)
	{
		uTemp=(UINT32)(VIM_HIF_GetReg8(adr+1)<<8);
		uTemp|=(UINT32)(VIM_HIF_GetReg8(adr));
		return 	(UINT16)uTemp;
	}
	else
	{
		uTemp=*(volatile UINT16 *)(VIM_REG_INDEX|(((adr>>16)&0xffff)<<1));
		uTemp=*(volatile UINT16 *)(VIM_REG_INDEX|((adr&0xfffc)<<1)) ;
		uTemp>>=16;
		uTemp|=*(volatile UINT16 *)(VIM_REG_INDEX|((VIM_READADD_LOCOSTO&0xfffc)<<1)) ;

		if((adr & 0x3) == 0)
		{
			return   (UINT16)(uTemp&0xffff);
		}
		else if((adr & 0x3) == 1)
		{
			return   (UINT16)((uTemp&0xffff00)>>8);
		}
		else 
		{
			return   (UINT16)((uTemp&0xffff0000)>>16);
		}	
		
	}
}


/***************************************************************
Description:
		set 32bit register use locosto

Parameters:
		adr: register address
		val:	   register value

Returns:
		void
****************************************************************/
void VIM_HIF_SetReg32(UINT32 adr, UINT32 val)       
{
	

	if((adr & 0x3) == 0)
	{
		*(volatile UINT16 *)(VIM_REG_INDEX|(((adr>>16)&0xffff)<<1)) = (UINT16)((val&0xffff0000)>>16);
		*(volatile UINT16 *)(VIM_REG_INDEX|(((adr&0xfffc)|1)<<1)) = (UINT16)(val);
	}
	else 
	{
		VIM_HIF_SetReg16(adr,(UINT16)val);
		VIM_HIF_SetReg16(adr+2,(UINT16)(val>>16));
	}
}
/***************************************************************
Description:
		get 32bit register use multi16

Parameters:
		adr: register address
		val:	   register value

Returns:
		return uint 16
****************************************************************/
UINT32 VIM_HIF_GetReg32(UINT32 adr)
{
	UINT32 uTemp;

	if((adr & 0x3) == 0)
	{
		uTemp=*(volatile UINT16 *)(VIM_REG_INDEX|(((adr>>16)&0xffff)<<1));
		uTemp=*(volatile UINT16 *)(VIM_REG_INDEX|((adr&0xfffc)<<1)) ;
		uTemp>>=16;
		uTemp|=*(volatile UINT16 *)(VIM_REG_INDEX|((VIM_READADD_LOCOSTO&0xfffc)<<1)) ;
	}
	else 
	{
		uTemp=(VIM_HIF_GetReg16(adr+2)<<16);
		uTemp|=VIM_HIF_GetReg16(adr);
	}
	return uTemp;

}

/***************************************************************
Description:
		set bypass register use locosto

Parameters:
		adr: register address
		val:	   register value

Returns:
		void
****************************************************************/
void VIM_HIF_SetSpecialReg(UINT8 val)       
{
	return VIM_SUCCEED;
}
/***************************************************************
Description:
		read sdram 8bit use locosto

Parameters:
		adr: sdram start address
		buf: the head point of buf
		size:	  the lenght of data(byte)
		Mode: fifo or normal sram
Returns:
		void
****************************************************************/
 UINT32 VIM_HIF_ReadSram(UINT32 adr,  HUGE UINT8 *buf, UINT32 size,VIM_HIF_SRAMMODE Mode)
{
	UINT32 uActsize=0, uTmp=0,uWaitlen=VIM_WAITMAX;
	SINT32 i=0;

	adr >>= 2;		/*!< address must 4 byte align, read buffer */
	adr <<= 2;
	adr+=V5_1TSRAM_BASE;
	
	size >>= 2;
	uActsize = size << 2;

	while(uWaitlen--)
	{
		if(!(VIM_HIF_GetReg8(V5_REG_BIU_STATUS) & 0x1))
			break;
	}
#if(VIM_HIF_DEBUG)&&(VIM_5XRDK_ERROR)
	if(!uWaitlen)
		VIM_USER_PrintString("[VC0578B][BIU] BIU is not ready for read [ERROR] ");
#endif
				/*!< wait untile ready */
	VIM_HIF_SetReg32(V5_REG_BIU_BASE_ADDR, adr|Mode);				/*!< set burst base address */
	VIM_HIF_SetReg32(V5_REG_BIU_BURST_LEN, size);				/*!< set burst length, word size */

	//这里需要delay 50个hclock
	for(i=0;i<10;i++);

	/*! read data continously */
	VIM_SET_RSHIGH_BY_GPIO();
	while(size--)
	{
		uTmp = *(volatile UINT16 *)(VIM_REG_INDEX|((VIM_READADD_LOCOSTO&0xfffc)<<1));
		buf[2] = (UINT8)uTmp;
		buf[3] = (UINT8)(uTmp>>8);
		uTmp = *(volatile UINT16 *)(VIM_REG_INDEX|((VIM_READADD_LOCOSTO&0xfffc)<<1));
		buf[0] = (UINT8)uTmp;
		buf[1] = (UINT8)(uTmp>>8);
		buf+=4;
	}
	while(uWaitlen--)
	{
		if(!(VIM_HIF_GetReg8(V5_REG_BIU_STATUS) & 0x1))
			break;
	}
	return uActsize;
}
/***************************************************************
Description:
		write sdram 8bit reverse

Parameters:
		adr: sdram start address
		buf: the head point of buf
		size:	  the lenght of data(byte)

Returns:
		void
****************************************************************/
UINT32 VIM_HIF_WriteSramReverse(UINT32 adr, const HUGE UINT8 *buf, UINT32 size)
{
	UINT32 uActsize=0, uTmp=0,uWaitlen=VIM_WAITMAX;
	SINT32 i=0;

	adr >>= 2;		/*!< address must 4 byte align, read buffer */
	adr <<= 2;
	adr+=V5_1TSRAM_BASE;
	
	size >>= 2;
	uActsize = size << 2;

	while(uWaitlen--)
	{
		if(!(VIM_HIF_GetReg8(V5_REG_BIU_STATUS) & 0x1))
			break;
	}
#if(VIM_HIF_DEBUG)&&(VIM_5XRDK_ERROR)
	if(!uWaitlen)
		VIM_USER_PrintString("[VC0578B][BIU] BIU is not ready for write [ERROR] ");
#endif
	adr|=1;    //write enable
	VIM_HIF_SetReg32(V5_REG_BIU_BASE_ADDR, adr);				/*!< set burst base address */
	VIM_HIF_SetReg32(V5_REG_BIU_BURST_LEN, size);				/*!< set burst length, word size */

	//这里需要delay 50个hclock
	for(i=0;i<10;i++);

	/*! read data continously */
	VIM_SET_RSHIGH_BY_GPIO();
	while(size--)
	{
		uTmp = (UINT32)(buf[2]<<8);
		uTmp |= (UINT32)buf[3];
		*(volatile UINT16 *)(VIM_REG_INDEX|((VIM_READADD_LOCOSTO&0xfffc)<<1))= uTmp;
		uTmp = (UINT32)(buf[0]<<8);
		uTmp |= (UINT32)buf[1];
		*(volatile UINT16 *)(VIM_REG_INDEX|((VIM_READADD_LOCOSTO&0xfffc)<<1))= uTmp;		
		buf+=4;
	}
	while(uWaitlen--)
	{
		if(!(VIM_HIF_GetReg8(V5_REG_BIU_STATUS) & 0x1))
			break;
	}
	return uActsize;
}
/***************************************************************
Description:
		write sdram 8bit

Parameters:
		adr: sdram start address
		buf: the head point of buf
		size:	  the lenght of data(byte)
		Mode: fifo or normal sram

Returns:
		void
****************************************************************/
UINT32 VIM_HIF_WriteSram(UINT32 adr, const HUGE UINT8 *buf, UINT32 size,VIM_HIF_SRAMMODE Mode)
{
	UINT32 uActsize=0, uTmp=0,uWaitlen=VIM_WAITMAX;
	SINT32 i=0;

	adr >>= 2;		/*!< address must 4 byte align, read buffer */
	adr <<= 2;
	adr+=V5_1TSRAM_BASE;
	
	size >>= 2;
	uActsize = size << 2;

	while(uWaitlen--)
	{
		if(!(VIM_HIF_GetReg8(V5_REG_BIU_STATUS) & 0x1))
			break;
	}
#if(VIM_HIF_DEBUG)&&(VIM_5XRDK_ERROR)
	if(!uWaitlen)
		VIM_USER_PrintString("[VC0578B][BIU] BIU is not ready for write [ERROR] ");
#endif
	adr|=1;    //write enable
	VIM_HIF_SetReg32(V5_REG_BIU_BASE_ADDR, adr|Mode);				/*!< set burst base address */
	VIM_HIF_SetReg32(V5_REG_BIU_BURST_LEN, size);				/*!< set burst length, word size */

	//这里需要delay 50个hclock
	for(i=0;i<10;i++);

	/*! read data continously */
	VIM_SET_RSHIGH_BY_GPIO();
	while(size--)
	{
		uTmp = (UINT32)(buf[3]<<8);
		uTmp |= (UINT32)buf[2];
		*(volatile UINT16 *)(VIM_REG_INDEX|((VIM_READADD_LOCOSTO&0xfffc)<<1))= uTmp;
		uTmp = (UINT32)(buf[1]<<8);
		uTmp |= (UINT32)buf[0];
		*(volatile UINT16 *)(VIM_REG_INDEX|((VIM_READADD_LOCOSTO&0xfffc)<<1))= uTmp;		
		buf+=4;
	}
	while(uWaitlen--)
	{
		if(!(VIM_HIF_GetReg8(V5_REG_BIU_STATUS) & 0x1))
			break;
	}
	return uActsize;
}
/***************************************************************
Description:
		write sdram 8bit on word

Parameters:
		adr: sdram start address
		buf: the head point of buf
		size:	  the lenght of data(byte)

Returns:
		void
****************************************************************/
// size in byte unit
UINT32 VIM_HIF_WriteSramOnWord(UINT32 adr, UINT16 dat, UINT32 size)
{
	UINT32 uActsize=0, uWaitlen=VIM_WAITMAX;
	SINT32 i=0;

	adr >>= 2;		/*!< address must 4 byte align, read buffer */
	adr <<= 2;
	adr+=V5_1TSRAM_BASE;
	
	size >>= 2;
	uActsize = size << 2;

	while(uWaitlen--)
	{
		if(!(VIM_HIF_GetReg8(V5_REG_BIU_STATUS) & 0x1))
			break;
	}
#if(VIM_HIF_DEBUG)&&(VIM_5XRDK_ERROR)
	if(!uWaitlen)
		VIM_USER_PrintString("[VC0578B][BIU] BIU is not ready for write [ERROR] ");
#endif
	adr|=1;    //write enable
	VIM_HIF_SetReg32(V5_REG_BIU_BASE_ADDR, adr);				/*!< set burst base address */
	VIM_HIF_SetReg32(V5_REG_BIU_BURST_LEN, size);				/*!< set burst length, word size */

	//这里需要delay 50个hclock
	for(i=0;i<10;i++);

	/*! read data continously */
	VIM_SET_RSHIGH_BY_GPIO();
	while(size--)
	{
		*(volatile UINT16 *)(VIM_REG_INDEX|((VIM_READADD_LOCOSTO&0xfffc)<<1)) = dat;
		*(volatile UINT16 *)(VIM_REG_INDEX|((VIM_READADD_LOCOSTO&0xfffc)<<1)) = dat;
	}
	while(uWaitlen--)
	{
		if(!(VIM_HIF_GetReg8(V5_REG_BIU_STATUS) & 0x1))
			break;
	}
	return uActsize;
}
/***************************************************************
Description:
		set multi 16 mode

Parameters:
		Timing:
			abi delay (0~32ns)max is 0x18
			wr delay (0~32ns)max is 0xb
			cs delay (0~32ns)max is 0x6

Returns:
		void
****************************************************************/

void VIM_HIF_SetMulti16(void)
{
	return;
}
#elif VIM_BUSTYPE == VIM_HIFAMR9
/** Module register definition **/
#define VC0888_HIF_REG_BASE				0x601e0000

#define VC0888_HIF_ADDR_CMD_REG			(VC0888_HIF_REG_BASE + 0x0c)
#define VC0888_HIF_WDATA_CMD_REG		(VC0888_HIF_REG_BASE + 0x10)
#define VC0888_HIF_RDATA_CMD_REG		(VC0888_HIF_REG_BASE + 0x14)
#define VC0888_HIF_RDATA_REG			(VC0888_HIF_REG_BASE + 0x18)
#define VC0888_HIF_WRITE_DATA_REG		(VC0888_HIF_REG_BASE + 0x1c)
#define VC0888_HIF_WAIT_WCNUM_REG		(VC0888_HIF_REG_BASE + 0x20)
#define VC0888_HIF_WAIT_RCNUM_REG		(VC0888_HIF_REG_BASE + 0x24)
#define VC0888_HIF_READ_CNUM_REG		(VC0888_HIF_REG_BASE + 0x28)
#define VC0888_HIF_STATUS_CSN_REG		(VC0888_HIF_REG_BASE + 0x2C)
#define VC0888_HIF_BURST_EN_REG			(VC0888_HIF_REG_BASE + 0x30)
#define VC0888_HIF_BURST_W_R_REG		(VC0888_HIF_REG_BASE + 0x34)
#define VC0888_HIF_S_ADDR_REG			(VC0888_HIF_REG_BASE + 0x38)
#define VC0888_HIF_BURST_WORDNUM_REG	(VC0888_HIF_REG_BASE + 0x3C)
#define VC0888_HIF_BURST_BREAK_REG		(VC0888_HIF_REG_BASE + 0x40)
#define VC0888_HIF_RS_REG				(VC0888_HIF_REG_BASE + 0x44)
#define VC0888_HIF_CS2N_REG				(VC0888_HIF_REG_BASE + 0x48)
#define VC0888_HIF_WBEN_REG				(VC0888_HIF_REG_BASE + 0x4C)
#define VC0888_HIF_I_DELAY_REG			(VC0888_HIF_REG_BASE + 0x50)
#define VC0888_HIF_AHB_BNUM_REG			(VC0888_HIF_REG_BASE + 0x54)
#define VC0888_HIF_BC_BNUM_REG			(VC0888_HIF_REG_BASE + 0x58)
#define VC0888_HIF_BURST_FINISH_REG		(VC0888_HIF_REG_BASE + 0x5c)
void Hif_Wait_Ready(void)
{
	UINT32 val;
	val = ((UINT32)(*(volatile UINT32 *)VC0888_HIF_BURST_EN_REG));

	while(0x0 != val)
	{
	val = ((UINT32)(*(volatile UINT32 *)VC0888_HIF_BURST_EN_REG));
	}
}
 void VIM_HIF_SetReg8(UINT32 adr, UINT8 val)       
{
	UINT32 uTemp,uTemp1;
	uTemp = 0;
	uTemp1 = 0;

	*(volatile UINT16 *)(VC0888_HIF_ADDR_CMD_REG) = (UINT16)((adr&0xffff0000)>>16);
	Hif_Wait_Ready();
	*(volatile UINT16 *)(VC0888_HIF_ADDR_CMD_REG) = (UINT16)((adr&0xfffc));
	Hif_Wait_Ready();
				
				
	uTemp1 = ((UINT16)(*(volatile UINT16 *)VC0888_HIF_RDATA_CMD_REG));
	Hif_Wait_Ready();
	uTemp = ((UINT16)(*(volatile UINT16 *)VC0888_HIF_RDATA_REG)<<16);	
	Hif_Wait_Ready();
		
	uTemp1 = ((UINT16)(*(volatile UINT16 *)VC0888_HIF_RDATA_CMD_REG));
	Hif_Wait_Ready();
	uTemp |= ((UINT16)(*(volatile UINT16 *)VC0888_HIF_RDATA_REG));	
	Hif_Wait_Ready();				

		
	*(volatile UINT16 *)(VC0888_HIF_ADDR_CMD_REG) = (UINT16)((adr&0xffff0000)>>16);
	Hif_Wait_Ready();
	*(volatile UINT16 *)(VC0888_HIF_ADDR_CMD_REG) = (UINT16)(adr&0xfffc|1);
	Hif_Wait_Ready();
	
	if((adr & 0x3) == 0)
	{
		*(volatile UINT16 *)(VC0888_HIF_WDATA_CMD_REG) = (UINT16)((uTemp&0xffff0000)>>16);    		
		Hif_Wait_Ready();				
		*(volatile UINT16 *)(VC0888_HIF_WDATA_CMD_REG) = (UINT16)((uTemp&0xff00)|val);     
		Hif_Wait_Ready();				
	}
	else if((adr & 0x3) == 1)
	{
		*(volatile UINT16 *)(VC0888_HIF_WDATA_CMD_REG) = (UINT16)((uTemp&0xffff0000)>>16);    		
		Hif_Wait_Ready();				
		*(volatile UINT16 *)(VC0888_HIF_WDATA_CMD_REG) = (UINT16)((uTemp&0x00ff)|((UINT16)(val)<<8));     
		Hif_Wait_Ready();				
	}
	else if((adr & 0x3) == 2)
	{
		*(volatile UINT16 *)(VC0888_HIF_WDATA_CMD_REG) = (UINT16)(((uTemp&0xff000000)>>16)|val);     
		Hif_Wait_Ready();				
		*(volatile UINT16 *)(VC0888_HIF_WDATA_CMD_REG) = (UINT16)(uTemp&0xffff);    		
		Hif_Wait_Ready();				
	}	
	else if((adr & 0x3) == 3)
	{
		*(volatile UINT16 *)(VC0888_HIF_WDATA_CMD_REG) = (UINT16)(((uTemp&0x00ff0000)>>16)|((UINT16)(val)<<8));     
		Hif_Wait_Ready();				
		*(volatile UINT16 *)(VC0888_HIF_WDATA_CMD_REG) = (UINT16)(uTemp&0xffff);    		
		Hif_Wait_Ready();				
	}	
	//gVc0578b_Isr.byIntFlag=0;
}

UINT8 VIM_HIF_GetReg8(UINT32 adr)
{

	UINT32 uTemp,uTemp1;
	uTemp = 0;
	uTemp1 = 0;

	*(volatile UINT16 *)(VC0888_HIF_ADDR_CMD_REG) = (UINT16)((adr&0xffff0000)>>16);
	Hif_Wait_Ready();
	*(volatile UINT16 *)(VC0888_HIF_ADDR_CMD_REG) = (UINT16)((adr&0xfffc));
	Hif_Wait_Ready();
				
				
	uTemp1 = ((UINT16)(*(volatile UINT16 *)VC0888_HIF_RDATA_CMD_REG));
	Hif_Wait_Ready();
	uTemp = ((UINT16)(*(volatile UINT16 *)VC0888_HIF_RDATA_REG)<<16);	
	Hif_Wait_Ready();
		
	uTemp1 = ((UINT16)(*(volatile UINT16 *)VC0888_HIF_RDATA_CMD_REG));
	Hif_Wait_Ready();
	uTemp |= ((UINT16)(*(volatile UINT16 *)VC0888_HIF_RDATA_REG));	
	Hif_Wait_Ready();		
	//gVc0578b_Isr.byIntFlag=0;
	if((adr & 0x3) == 0)
	{
		return (UINT8)(uTemp&0xff);
	}
	else if((adr & 0x3) == 1)
	{
		return (UINT8)((uTemp&0xff00)>>8);
	}
	else if((adr & 0x3) == 2)
	{
		return (UINT8)((uTemp&0xff0000)>>16);
	}	
	else 
	{
		return (UINT8)((uTemp&0xff000000)>>24);
	}	

}

UINT32 VIM_HIF_GetReg32(UINT32 addr)
{
	static volatile UINT32 uTemp,uTemp1;
	//gVc0578b_Isr.byIntFlag=1;
	if((addr & 0x3) == 0)
	{

		*(volatile UINT16 *)(VC0888_HIF_ADDR_CMD_REG) = (UINT16)((addr&0xffff0000)>>16);
		Hif_Wait_Ready();
		
//		for(g_delayfor=0; g_delayfor<VIM_READREG_DELAYFOR; g_delayfor++);		
		*(volatile UINT16 *)(VC0888_HIF_ADDR_CMD_REG) = (UINT16)((addr&0xfffc));
		Hif_Wait_Ready();
				
		for(g_delayfor=0; g_delayfor<VIM_READREG_DELAYFOR; g_delayfor++);				
		uTemp = ((UINT16)(*(volatile UINT16 *)VC0888_HIF_RDATA_CMD_REG));
		Hif_Wait_Ready();
		uTemp1 = ((UINT16)(*(volatile UINT16 *)VC0888_HIF_RDATA_REG)<<16);	
		Hif_Wait_Ready();
		
//		for(g_delayfor=0; g_delayfor<VIM_READREG_DELAYFOR; g_delayfor++);				
		
		uTemp = ((UINT16)(*(volatile UINT16 *)VC0888_HIF_RDATA_CMD_REG));
		Hif_Wait_Ready();
		uTemp1 |= ((UINT16)(*(volatile UINT16 *)VC0888_HIF_RDATA_REG));	
		Hif_Wait_Ready();			
		
		return uTemp1;
		
	}
	else 
	{
		uTemp=(VIM_HIF_GetReg16(addr+2)<<16);
		uTemp|=VIM_HIF_GetReg16(addr);

	}
	//gVc0578b_Isr.byIntFlag=0;
	return uTemp1;

}


void VIM_HIF_SetReg32(UINT32 adr, UINT32 val)       
{

	//gVc0578b_Isr.byIntFlag=1;
	if((adr & 0x3) == 0)
	{
		*(volatile UINT16 *)(VC0888_HIF_ADDR_CMD_REG) = (UINT16)((adr&0xffff0000)>>16);
		Hif_Wait_Ready();
		*(volatile UINT16 *)(VC0888_HIF_ADDR_CMD_REG) = (UINT16)((adr&0xfffc)|1);
		Hif_Wait_Ready();

		
		
		*(volatile UINT16 *)(VC0888_HIF_WDATA_CMD_REG) = (UINT16)(((val&0xffff0000)>>16));
		Hif_Wait_Ready();	

		*(volatile UINT16 *)(VC0888_HIF_WDATA_CMD_REG) = (UINT16)(val&0xffff);
		Hif_Wait_Ready();
	}
	else 
	{
		VIM_HIF_SetReg16(adr,(UINT16)val);
		VIM_HIF_SetReg16(adr+2,(UINT16)(val>>16));
	}
	
	//gVc0578b_Isr.byIntFlag=0;
}

void VIM_HIF_SetReg16(UINT32 adr, UINT16 val)       
{
	UINT32 uTemp,uTemp1;
	//gVc0578b_Isr.byIntFlag=1;	
	if((adr & 0x3) == 3)
	{

		*(volatile UINT16 *)(VC0888_HIF_ADDR_CMD_REG) = (UINT16)((adr&0xffff0000)>>16);
		Hif_Wait_Ready();
		*(volatile UINT16 *)(VC0888_HIF_ADDR_CMD_REG) = (UINT16)((adr&0xfffc));
		Hif_Wait_Ready();
				
				
		uTemp1 = ((UINT16)(*(volatile UINT16 *)VC0888_HIF_RDATA_CMD_REG));
		Hif_Wait_Ready();
		uTemp = ((UINT16)(*(volatile UINT16 *)VC0888_HIF_RDATA_REG)<<16);	
		Hif_Wait_Ready();
		
		uTemp1 = ((UINT16)(*(volatile UINT16 *)VC0888_HIF_RDATA_CMD_REG));
		Hif_Wait_Ready();
		uTemp |= ((UINT16)(*(volatile UINT16 *)VC0888_HIF_RDATA_REG));	
		Hif_Wait_Ready();			
	

		*(volatile UINT16 *)(VC0888_HIF_ADDR_CMD_REG) = (UINT16)((adr&0xffff0000)>>16);
		Hif_Wait_Ready();
		*(volatile UINT16 *)(VC0888_HIF_ADDR_CMD_REG) = (UINT16)(adr&0xfffc|1);	//地址是4的倍数 1表示写寄存器
		Hif_Wait_Ready();
		
		*(volatile UINT16 *)(VC0888_HIF_WDATA_CMD_REG) = (UINT16)(((uTemp&0x00ff0000)>>16)|((UINT16)(val&0xff)<<8));
		Hif_Wait_Ready();	

		*(volatile UINT16 *)(VC0888_HIF_WDATA_CMD_REG) = (UINT16)(uTemp&0xffff);
		Hif_Wait_Ready();		
	

		*(volatile UINT16 *)(VC0888_HIF_ADDR_CMD_REG) = (UINT16)((adr&0xffff0000)>>16);
		Hif_Wait_Ready();
		*(volatile UINT16 *)(VC0888_HIF_ADDR_CMD_REG) = (UINT16)((adr+1)&0xfffc);	//地址是4的倍数
		Hif_Wait_Ready();	
		
		uTemp1 = ((UINT16)(*(volatile UINT16 *)VC0888_HIF_RDATA_CMD_REG));
		Hif_Wait_Ready();
		uTemp = ((UINT16)(*(volatile UINT16 *)VC0888_HIF_RDATA_REG)<<16);	
		Hif_Wait_Ready();
		
		uTemp1 = ((UINT16)(*(volatile UINT16 *)VC0888_HIF_RDATA_CMD_REG));
		Hif_Wait_Ready();
		uTemp |= ((UINT16)(*(volatile UINT16 *)VC0888_HIF_RDATA_REG));	
		Hif_Wait_Ready();			
		

		*(volatile UINT16 *)(VC0888_HIF_ADDR_CMD_REG) = (UINT16)((adr&0xffff0000)>>16);
		Hif_Wait_Ready();
		*(volatile UINT16 *)(VC0888_HIF_ADDR_CMD_REG) = (UINT16)(adr&0xfffc|1);	//地址是4的倍数 1表示写寄存器
		Hif_Wait_Ready();
		
		
		*(volatile UINT16 *)(VC0888_HIF_WDATA_CMD_REG) = (UINT16)((uTemp&0xffff0000)>>16);
		Hif_Wait_Ready();	
		*(volatile UINT16 *)(VC0888_HIF_WDATA_CMD_REG) = (UINT16)((uTemp&0xff00)|((UINT16)(val&0xff00)>>8));
		Hif_Wait_Ready();			

	}
	else
	{
	
		*(volatile UINT16 *)(VC0888_HIF_ADDR_CMD_REG) = (UINT16)((adr&0xffff0000)>>16);
		Hif_Wait_Ready();
		*(volatile UINT16 *)(VC0888_HIF_ADDR_CMD_REG) = (UINT16)(adr&0xfffc);	//地址是4的倍数 1表示写寄存器
		Hif_Wait_Ready();	
		
		uTemp1 = ((UINT16)(*(volatile UINT16 *)VC0888_HIF_RDATA_CMD_REG));
		Hif_Wait_Ready();
		uTemp = ((UINT16)(*(volatile UINT16 *)VC0888_HIF_RDATA_REG)<<16);	
		Hif_Wait_Ready();
		
		uTemp1 = ((UINT16)(*(volatile UINT16 *)VC0888_HIF_RDATA_CMD_REG));
		Hif_Wait_Ready();
		uTemp |= ((UINT16)(*(volatile UINT16 *)VC0888_HIF_RDATA_REG));	
		Hif_Wait_Ready();					
		
		*(volatile UINT16 *)(VC0888_HIF_ADDR_CMD_REG) = (UINT16)((adr&0xffff0000)>>16);
		Hif_Wait_Ready();
		*(volatile UINT16 *)(VC0888_HIF_ADDR_CMD_REG) = (UINT16)(adr&0xfffc|1);	//地址是4的倍数 1表示写寄存器
		Hif_Wait_Ready();
						
		
		if((adr & 0x3) == 0)
		{
			*(volatile UINT16 *)(VC0888_HIF_WDATA_CMD_REG) = (UINT16)((uTemp&0xffff0000)>>16);    		
			Hif_Wait_Ready();
			*(volatile UINT16 *)(VC0888_HIF_WDATA_CMD_REG) = (UINT16)(val);     
			Hif_Wait_Ready();
		}
		else if((adr & 0x3) == 1)
		{
			*(volatile UINT16 *)(VC0888_HIF_WDATA_CMD_REG) = (UINT16)((uTemp&0xff000000)>>16)|((UINT16)(val&0xff00)>>8);    		
			Hif_Wait_Ready();
			*(volatile UINT16 *)(VC0888_HIF_WDATA_CMD_REG) = (UINT16)((uTemp&0x00ff)|((UINT16)(val&0xff)<<8));     
			Hif_Wait_Ready();
		}
		else if((adr & 0x3) == 2)
		{
			*(volatile UINT16 *)(VC0888_HIF_WDATA_CMD_REG) = (UINT16)(val);     
			Hif_Wait_Ready();
   			*(volatile UINT16 *)(VC0888_HIF_WDATA_CMD_REG) = (UINT16)(uTemp&0xffff);    		
   			Hif_Wait_Ready();
		}	
		
	}
	//gVc0578b_Isr.byIntFlag=0;
}

UINT16 VIM_HIF_GetReg16(UINT32 addr)
{
	volatile UINT32 temp,temp1;
	
	if((addr & 0x3) == 3)
	{

		*(volatile UINT16 *)(VC0888_HIF_ADDR_CMD_REG) = (UINT16)((addr&0xffff0000)>>16);
		Hif_Wait_Ready();
		*(volatile UINT16 *)(VC0888_HIF_ADDR_CMD_REG) = (UINT16)((addr&0xfffc));
		Hif_Wait_Ready();
				
				
		temp = ((UINT16)(*(volatile UINT16 *)VC0888_HIF_RDATA_CMD_REG));
		Hif_Wait_Ready();
		temp1 = ((UINT16)(*(volatile UINT16 *)VC0888_HIF_RDATA_REG)<<16);	
		Hif_Wait_Ready();
		
		temp = ((UINT16)(*(volatile UINT16 *)VC0888_HIF_RDATA_CMD_REG));
		Hif_Wait_Ready();
		temp1 |= ((UINT16)(*(volatile UINT16 *)VC0888_HIF_RDATA_REG));	
		Hif_Wait_Ready();			
		
		return 	(UINT16)((temp1&0xff)<<8)|((temp1&0xff000000)>>24);
	}
	else
	{
	
		*(volatile UINT16 *)(VC0888_HIF_ADDR_CMD_REG) = (UINT16)((addr&0xffff0000)>>16);
		Hif_Wait_Ready();
		*(volatile UINT16 *)(VC0888_HIF_ADDR_CMD_REG) = (UINT16)((addr&0xfffc));
		Hif_Wait_Ready();
		
		temp = ((UINT16)(*(volatile UINT16 *)VC0888_HIF_RDATA_CMD_REG));
		Hif_Wait_Ready();
		temp1 = ((UINT16)(*(volatile UINT16 *)VC0888_HIF_RDATA_REG)<<16);	
		Hif_Wait_Ready();
		
		temp = ((UINT16)(*(volatile UINT16 *)VC0888_HIF_RDATA_CMD_REG));
		Hif_Wait_Ready();
		temp1 |= ((UINT16)(*(volatile UINT16 *)VC0888_HIF_RDATA_REG));	
		Hif_Wait_Ready();			
		
		if((addr & 0x3) == 0)
		{
			return   (UINT16)(temp1&0xffff);
		}
		else if((addr & 0x3) == 1)
		{
			return   (UINT16)((temp1&0xffff00)>>8);
		}
		else 
		{
			return   (UINT16)((temp1&0xffff0000)>>16);
		}	
		
	}
	
}

 UINT32 VIM_HIF_WriteSram(UINT32 adr, const HUGE UINT8 *buf, UINT32 size,VIM_HIF_SRAMMODE Mode)
{
	UINT32 uActsize=0, uTmp=0,uWaitlen;
	
	uWaitlen=VIM_WAITMAX;

	adr >>= 2;		/*!< address must 4 byte align, read buffer */
	adr <<= 2;
	adr+=V5_1TSRAM_BASE;
	
	size >>= 2;
	uActsize = size << 2;

	while(uWaitlen--)
	{
		uTmp = VIM_HIF_GetReg32(V5_REG_BIU_STATUS);

		if(uTmp&0x1)		/*!< wait untile ready */
			break;
		VIM_USER_DelayMs(20);
	}
#if(VIM_HIF_DEBUG)&&(VIM_5XRDK_ERROR)
	if(!uWaitlen)
		VIM_USER_PrintString("[VC0578B][BIU] BIU is not ready for write [ERROR] ");
#endif
	adr|=1;    //write enable
	VIM_HIF_SetReg32(V5_REG_BIU_BASE_ADDR, adr|Mode);				/*!< set burst base address */
	VIM_HIF_SetReg32(V5_REG_BIU_BURST_LEN, size);				/*!< set burst length, word size */

	//这里需要delay 50个hclock
	for(g_delayfor=0; g_delayfor<VIM_READRAM_DELAYFOR; g_delayfor++);

	/*! read data continously */
	//gVc0578b_Isr.byIntFlag=1;
	VIM_SET_RSHIGH_BY_GPIO();
	while(size--)
	{
		uTmp = (UINT32)(buf[3]<<8);
		uTmp |= (UINT32)buf[2];
//		*(volatile UINT16*)(VIM_REG_VALUE) = uTmp;
		*(volatile UINT16 *)(VC0888_HIF_WDATA_CMD_REG) = (UINT16)(uTmp);
		Hif_Wait_Ready();	


		uTmp = (UINT32)(buf[1]<<8);
		uTmp |= (UINT32)buf[0];
//		*(volatile UINT16*)(VIM_REG_VALUE) = uTmp;		
		*(volatile UINT16 *)(VC0888_HIF_WDATA_CMD_REG) = (UINT16)(uTmp);
		Hif_Wait_Ready();	
		buf+=4;
	}
	//gVc0578b_Isr.byIntFlag=0;
	while(1)//(uWaitlen--)
	{
		if(VIM_HIF_GetReg32(V5_REG_BIU_STATUS)&0x1)		/*!< wait untile ready */
			break;
	}
	return uActsize;
}


UINT32 VIM_HIF_ReadSram(UINT32 adr,  HUGE UINT8 *buf, UINT32 size,VIM_HIF_SRAMMODE Mode)
{
	UINT32 uActsize=0, uTmp=0,uWaitlen=VIM_WAITMAX,uTemp=0;

	adr >>= 2;		/*!< address must 4 byte align, read buffer */
	adr <<= 2;
	adr+=V5_1TSRAM_BASE;
	
	size >>= 2;
	uActsize = size << 2;

	while(uWaitlen--)
	{
		if(VIM_HIF_GetReg32(V5_REG_BIU_STATUS)&0x1)		/*!< wait untile ready */
			break;
		VIM_USER_DelayMs(20);
	}
#if(VIM_HIF_DEBUG)&&(VIM_5XRDK_ERROR)
	if(!uWaitlen)
		VIM_USER_PrintString("[VC0578B][BIU] BIU is not ready for read [ERROR] ");
#endif
				/*!< wait untile ready */
	VIM_HIF_SetReg32(V5_REG_BIU_BASE_ADDR, adr|Mode);				/*!< set burst base address */
	VIM_HIF_SetReg32(V5_REG_BIU_BURST_LEN, size);				/*!< set burst length, word size */

	//这里需要delay 50个hclock
	for(g_delayfor=0; g_delayfor<VIM_READRAM_DELAYFOR; g_delayfor++);

	//gVc0578b_Isr.byIntFlag=1;
	/*! read data continously */
	VIM_SET_RSHIGH_BY_GPIO();
	while(size--)
	{
//		uTmp = *(volatile UINT16*)(VIM_REG_VALUE);
		
		uTemp = ((UINT16)(*(volatile UINT16 *)VC0888_HIF_RDATA_CMD_REG));
		Hif_Wait_Ready();
		uTmp = ((UINT16)(*(volatile UINT16 *)VC0888_HIF_RDATA_REG));	
		Hif_Wait_Ready();			


		buf[2] = (UINT8)uTmp;
		buf[3] = (UINT8)(uTmp>>8);
//		uTmp = *(volatile UINT16*)(VIM_REG_VALUE);
		uTemp = ((UINT16)(*(volatile UINT16 *)VC0888_HIF_RDATA_CMD_REG));
		Hif_Wait_Ready();
		uTmp = ((UINT16)(*(volatile UINT16 *)VC0888_HIF_RDATA_REG));	
		Hif_Wait_Ready();			
		
		buf[0] = (UINT8)uTmp;
		buf[1] = (UINT8)(uTmp>>8);
		buf+=4;
	}
	//gVc0578b_Isr.byIntFlag=0;
	while(1)//(uWaitlen--)
	{
		if(VIM_HIF_GetReg32(V5_REG_BIU_STATUS)&0x1)		/*!< wait untile ready */
			break;
	}
	return uActsize;
}



void VIM_HIF_SetSpecialReg(UINT8 val)       
{
	//gVc0578b_Isr.byIntFlag=1;
	/*
	*(volatile UINT16 *)(VIM_REG_VALUE) = (UINT16)(((VIM_BYPASS_VALUE&0xffff0000)>>16));     
	*(volatile UINT16 *)(VIM_REG_VALUE) = (UINT16)((VIM_BYPASS_VALUE&0xff00)|val);
	*(volatile UINT16 *)(VIM_REG_INDEX) = (UINT16)(((VIM_BYPASS_ADDR&0xffff0000)>>16));     
	*(volatile UINT16 *)(VIM_REG_INDEX) = (UINT16)((VIM_BYPASS_ADDR&0xff00)|val);    
	*/
	*(volatile UINT16 *)(VC0888_HIF_WDATA_CMD_REG) = (UINT16)(((VIM_BYPASS_VALUE&0xffff0000)>>16));
	Hif_Wait_Ready();
	*(volatile UINT16 *)(VC0888_HIF_WDATA_CMD_REG) = (UINT16)((VIM_BYPASS_VALUE&0xff00)|val);
	Hif_Wait_Ready();
	
	*(volatile UINT16 *)(VC0888_HIF_ADDR_CMD_REG) = (UINT16)(((VIM_BYPASS_ADDR&0xffff0000)>>16));     
	Hif_Wait_Ready();
	*(volatile UINT16 *)(VC0888_HIF_ADDR_CMD_REG) = (UINT16)((VIM_BYPASS_ADDR&0xff00)|val);
	Hif_Wait_Ready();
	
	//gVc0578b_Isr.byIntFlag=0;
}

UINT32 VIM_HIF_WriteSramReverse(UINT32 adr, const HUGE UINT8 *buf, UINT32 size)
{
return 0;
}
UINT32 VIM_HIF_WriteSramOnWord(UINT32 adr, UINT16 dat, UINT32 size)
{
UINT32 uActsize=0, uTmp=0,uWaitlen;
	
	uWaitlen=VIM_WAITMAX;

	adr >>= 2;		/*!< address must 4 byte align, read buffer */
	adr <<= 2;
	adr+=V5_1TSRAM_BASE;
	
	size >>= 2;
	uActsize = size << 2;

	while(uWaitlen--)
	{
		uTmp = VIM_HIF_GetReg32(V5_REG_BIU_STATUS);

		if(uTmp&0x1)		/*!< wait untile ready */
			break;
		VIM_USER_DelayMs(20);
	}
#if(VIM_HIF_DEBUG)&&(VIM_5XRDK_ERROR)
	if(!uWaitlen)
		VIM_USER_PrintString("[VC0578B][BIU] BIU is not ready for write [ERROR] ");
#endif
	adr|=1;    //write enable
	VIM_HIF_SetReg32(V5_REG_BIU_BASE_ADDR, adr);				/*!< set burst base address */
	VIM_HIF_SetReg32(V5_REG_BIU_BURST_LEN, size);				/*!< set burst length, word size */

	//这里需要delay 50个hclock
	for(g_delayfor=0; g_delayfor<VIM_READRAM_DELAYFOR; g_delayfor++);

	/*! read data continously */
	//gVc0578b_Isr.byIntFlag=1;
	VIM_SET_RSHIGH_BY_GPIO();
	while(size--)
	{
		*(volatile UINT16 *)(VC0888_HIF_WDATA_CMD_REG) = (UINT16)(dat);
		Hif_Wait_Ready();	
		*(volatile UINT16 *)(VC0888_HIF_WDATA_CMD_REG) = (UINT16)(dat);
		Hif_Wait_Ready();	

	}
	while(1)//(uWaitlen--)
	{
		if(VIM_HIF_GetReg32(V5_REG_BIU_STATUS)&0x1)		/*!< wait untile ready */
			break;
	}
	return uActsize;

}
/***************************************************************
Description:
		set multi 16 mode

Parameters:
		Timing:
			abi delay (0~32ns)max is 0x18
			wr delay (0~32ns)max is 0xb
			cs delay (0~32ns)max is 0x6

Returns:
		void
****************************************************************/
void VIM_HIF_SetMulti16(void)
{
	
	static volatile UINT32	temp,temp1;

	Hif_Wait_Ready();
	*(volatile UINT8 *)(VC0888_HIF_ADDR_CMD_REG) = (UINT8)((V5_REG_CPM_ABI_TEST>>24)&0xff);
	Hif_Wait_Ready();	
	*(volatile UINT8 *)(VC0888_HIF_ADDR_CMD_REG) = (UINT8)((V5_REG_CPM_ABI_TEST>>16)&0xff);
	Hif_Wait_Ready();	
	*(volatile UINT8 *)(VC0888_HIF_ADDR_CMD_REG) = (UINT8)((V5_REG_CPM_ABI_TEST>>8)&0xff);
	Hif_Wait_Ready();	
	*(volatile UINT8 *)(VC0888_HIF_ADDR_CMD_REG) = (UINT8)((V5_REG_CPM_ABI_TEST&0xfc));			
	Hif_Wait_Ready();	


	temp = ((UINT32)(*(volatile UINT8 *)VC0888_HIF_RDATA_CMD_REG));
	Hif_Wait_Ready();	
	
	temp1 = ((UINT32)(*(volatile UINT8 *)VC0888_HIF_RDATA_REG)<<24);	
	Hif_Wait_Ready();
	
	temp = ((UINT32)(*(volatile UINT8 *)VC0888_HIF_RDATA_CMD_REG));
	Hif_Wait_Ready();
	
	temp1 |= ((UINT32)(*(volatile UINT8 *)VC0888_HIF_RDATA_REG)<<16);	
	Hif_Wait_Ready();
	
	temp = ((UINT32)(*(volatile UINT8 *)VC0888_HIF_RDATA_CMD_REG));
	Hif_Wait_Ready();
	
	temp1 |= ((UINT32)(*(volatile UINT8 *)VC0888_HIF_RDATA_REG)<<8);
	Hif_Wait_Ready();
	
	temp = ((UINT32)(*(volatile UINT8 *)VC0888_HIF_RDATA_CMD_REG));
	Hif_Wait_Ready();
	temp1 |= ((UINT32)(*(volatile UINT8 *)VC0888_HIF_RDATA_REG));		
	Hif_Wait_Ready();

		

	*(volatile UINT8 *)(VC0888_HIF_ADDR_CMD_REG) = (UINT8)((V5_REG_CPM_ABI_CFG>>24)&0xff);
	Hif_Wait_Ready();	
	
	*(volatile UINT8 *)(VC0888_HIF_ADDR_CMD_REG) = (UINT8)((V5_REG_CPM_ABI_CFG>>16)&0xff);
	Hif_Wait_Ready();	
	*(volatile UINT8 *)(VC0888_HIF_ADDR_CMD_REG) = (UINT8)((V5_REG_CPM_ABI_CFG>>8)&0xff);
	Hif_Wait_Ready();
	*(volatile UINT8 *)(VC0888_HIF_ADDR_CMD_REG) = (UINT8)((V5_REG_CPM_ABI_CFG&0xfc));			/*!< 地址是4的倍数 */
	Hif_Wait_Ready();

		

	temp = ((UINT32)(*(volatile UINT8 *)VC0888_HIF_RDATA_CMD_REG));
	Hif_Wait_Ready();
	temp1 = ((UINT32)(*(volatile UINT8 *)VC0888_HIF_RDATA_REG)<<24);	
	Hif_Wait_Ready();
	
	temp = ((UINT32)(*(volatile UINT8 *)VC0888_HIF_RDATA_CMD_REG));
	Hif_Wait_Ready();
	temp1 |= ((UINT32)(*(volatile UINT8 *)VC0888_HIF_RDATA_REG)<<16);	
	Hif_Wait_Ready();
	
	temp = ((UINT32)(*(volatile UINT8 *)VC0888_HIF_RDATA_CMD_REG));
	Hif_Wait_Ready();
	temp1 |= ((UINT32)(*(volatile UINT8 *)VC0888_HIF_RDATA_REG)<<8);
	Hif_Wait_Ready();
	
	temp = ((UINT32)(*(volatile UINT8 *)VC0888_HIF_RDATA_CMD_REG));
	Hif_Wait_Ready();
	temp1 |= ((UINT32)(*(volatile UINT8 *)VC0888_HIF_RDATA_REG));		
	Hif_Wait_Ready();
	
	
	*(volatile UINT8 *)(VC0888_HIF_ADDR_CMD_REG) = (UINT8)((V5_REG_CPM_ABI_CFG&0xff000000)>>24);
	Hif_Wait_Ready();
	*(volatile UINT8 *)(VC0888_HIF_ADDR_CMD_REG) = (UINT8)((V5_REG_CPM_ABI_CFG&0xff0000)>>16);
	Hif_Wait_Ready();
	*(volatile UINT8 *)(VC0888_HIF_ADDR_CMD_REG) = (UINT8)((V5_REG_CPM_ABI_CFG&0xff00)>>8);
	Hif_Wait_Ready();
	*(volatile UINT8 *)(VC0888_HIF_ADDR_CMD_REG) = (UINT8)((V5_REG_CPM_ABI_CFG&0xfc)|1);	//地址是4的倍数
	Hif_Wait_Ready();



	*(volatile UINT8 *)(VC0888_HIF_WDATA_CMD_REG) = (UINT8)(0x01);    	//set multi16		
	Hif_Wait_Ready();	

	*(volatile UINT8 *)(VC0888_HIF_WDATA_CMD_REG) = (UINT8)(0x18);    		
	Hif_Wait_Ready();	

	*(volatile UINT8 *)(VC0888_HIF_WDATA_CMD_REG) = (UINT8)(0x0b);    
	Hif_Wait_Ready();				

	*(volatile UINT8 *)(VC0888_HIF_WDATA_CMD_REG) = (UINT8)(0x06);     	
	Hif_Wait_Ready();
	
	temp1 = VIM_HIF_GetReg32(V5_REG_CPM_ABI_CFG);
}
#elif VIM_BUSTYPE == VIM_VC0820_MULTI32
/***************************************************************
Description:
		set 8bit register use multi16

Parameters:
		adr: register address
		val:	   register value

Returns:
		void
****************************************************************/
 void VIM_HIF_SetReg8(UINT32 adr, UINT8 val)       
{
	UINT32 uTemp, uAdr;

	uTemp=*(volatile UINT32 *)(adr&0xfffffffc);
	uAdr=adr&0xfffffffc;
	
	if((adr & 0x3) == 0)
	{
		*(volatile UINT32 *)(uAdr)=(uTemp & 0xffffff00)|val;
	}
	else if((adr & 0x3) == 1)
	{
		*(volatile UINT32 *)(uAdr)=(uTemp & 0xffff00ff)|(val<<8);
	}
	else if((adr & 0x3) == 2)
	{
		*(volatile UINT32 *)(uAdr)=(uTemp & 0xff00ffff)|(val<<16);
	}	
	else if((adr & 0x3) == 3)
	{
		*(volatile UINT32 *)(uAdr)=(uTemp & 0x00ffffff)|(val<<24);
	}	
}
/***************************************************************
Description:
		get 8bit register use multi16

Parameters:
		adr: register address
		val:	   register value

Returns:
		return uint 8
****************************************************************/

UINT8 VIM_HIF_GetReg8(UINT32 adr)
{
	UINT32 uTemp;
	
	uTemp=*(volatile UINT32 *)(adr&0xfffffffc);
	if((adr & 0x3) == 0)
	{
		return (UINT8)(uTemp&0xff);
	}
	else if((adr & 0x3) == 1)
	{
		return (UINT8)((uTemp&0xff00)>>8);
	}
	else if((adr & 0x3) == 2)
	{
		return (UINT8)((uTemp&0xff0000)>>16);
	}	
	else 
	{
		return (UINT8)((uTemp&0xff000000)>>24);
	}	
}
/***************************************************************
Description:
		set 16bit register use multi16

Parameters:
		adr: register address
		val:	   register value

Returns:
		void
****************************************************************/
void VIM_HIF_SetReg16(UINT32 adr, UINT16 val)       
{
	UINT32 uTemp,uAdr;

	uTemp=*(volatile UINT32 *)(adr&0xfffffffc);
	uAdr=adr&0xfffffffc;

	if((adr & 0x3) == 0)
	{
		*(volatile UINT32 *)(uAdr)=(uTemp & 0xffff0000)|val;
	}
	else if((adr & 0x3) == 2)
	{
		*(volatile UINT32 *)(uAdr)=(uTemp & 0x0000ffff)|(val<<16);
	}
	else
		VIM_USER_Print("adr error adr=0x%x,val=0x%x\n",adr, val);
}
/***************************************************************
Description:
		get 16bit register use multi16

Parameters:
		adr: register address
		val:	   register value

Returns:
		return uint 16
****************************************************************/
UINT16 VIM_HIF_GetReg16(UINT32 adr)
{
	UINT32 uTemp=0;

	uTemp=*(volatile UINT32 *)(adr&0xfffffffc);

	if((adr & 0x3) == 0)
	{
		return (UINT16)(uTemp & 0x0000ffff);
	}
	else if((adr & 0x3) == 2)
	{
		return (UINT16)((uTemp & 0xffff0000)>>16);
	}
	else
	{
		VIM_USER_Print("adr error adr=0x%x\n",adr);
		return 0;
	}	
}

/***************************************************************
Description:
		set 32bit register use multi16

Parameters:
		adr: register address
		val:	   register value

Returns:
		void
****************************************************************/
void VIM_HIF_SetReg32(UINT32 adr, UINT32 val)       
{
	*(volatile UINT32 *)(adr)=val;   
}
/***************************************************************
Description:
		get 32bit register use multi16

Parameters:
		adr: register address
		val:	   register value

Returns:
		return uint 16
****************************************************************/
UINT32 VIM_HIF_GetReg32(UINT32 adr)
{
	return *(volatile UINT32 *)(adr);
}

/***************************************************************
Description:
		set bypass register use multi16

Parameters:
		adr: register address
		val:	   register value

Returns:
		void
****************************************************************/
void VIM_HIF_SetSpecialReg(UINT8 val)       
{
}

extern volatile UINT32 g_dma_flag;
void get_video_data(UINT8*des, UINT8 *scr, UINT32 size)
{
#if 0 // use dma
	UINT32 dma_ctrl;
	g_dma_flag = 1;
	dma_ctrl = TW_WORD|FC_M2M|BS_32BEATS|CEN_ENA;
	VIM_DMA_Start((UINT32)scr,(UINT32)des,size,dma_ctrl,0);    
	//VIM_DMA_Start((UINT32)scr,(UINT32)des,size,0x8101,0);    
	while(g_dma_flag);						  
#else
	VIM_USER_MemCpy(des, scr, size);
#endif
}

/***************************************************************
Description:
		read sdram 8bit

Parameters:
		adr: sdram start address
		buf: the head point of buf
		size:	  the lenght of data(byte)

Returns:
		void
****************************************************************/

UINT32 VIM_HIF_ReadSram(UINT32 adr,  HUGE UINT8 *buf, UINT32 size,VIM_HIF_SRAMMODE Mode)
{
	UINT32 FifoNum, ToEnd,addr,len;
	
	switch(Mode)
	{
	case VIM_HIF_SRAM_MODE:
		VIM_USER_MemCpy(buf,(UINT8*)adr+V5_1TSRAM_BASE, size);
		break;

	case VIM_HIF_FIFO_0_MODE:
	case VIM_HIF_FIFO_1_MODE:		
		//(1) get fifo number
		FifoNum = Mode-3;
		//(2) get size from read cursor to the end of fifo
		ToEnd = gVc0578b_Info.MarbStatus.VFifo[FifoNum].length-gVc0578b_Info.MarbStatus.VFifo[FifoNum].read_cur;
		addr = gVc0578b_Info.MarbStatus.VFifo[FifoNum].address+gVc0578b_Info.MarbStatus.VFifo[FifoNum].read_cur;

		//(3) read data from fifo
		if(size<=ToEnd)
		{
			VIM_USER_PrintDec("[VC0578B][HIF] copy size= ",size);
			get_video_data(buf,(UINT8*)addr, size);
			gVc0578b_Info.MarbStatus.VFifo[FifoNum].read_cur+=size;
		}
		else
		{
			VIM_USER_PrintDec("[VC0578B][HIF] copy size= ",ToEnd);
			get_video_data(buf,(UINT8*)addr, ToEnd);
			len = size - ToEnd;
			addr = gVc0578b_Info.MarbStatus.VFifo[FifoNum].address;
			VIM_USER_PrintDec("[VC0578B][HIF] copy size= ",len);
			get_video_data(buf+ToEnd,(UINT8*)addr, len);
			gVc0578b_Info.MarbStatus.VFifo[FifoNum].read_cur=len;
		}

		//(4) set fifo count 
		VIM_USER_PrintDec("[VC0578B][HIF] set count= ",size);

		VIM_MARB_SetFifoCount(FifoNum*2,MARB_MODE_ARMD_READ,size);

		break;

	}

	return size;
}
/***************************************************************
Description:
		write sdram 8bit reverse

Parameters:
		adr: sdram start address
		buf: the head point of buf
		size:	  the lenght of data(byte)

Returns:
		void
****************************************************************/
UINT32 VIM_HIF_WriteSramReverse(UINT32 adr, const HUGE UINT8 *buf, UINT32 size)
{
	UINT32 uTmp=0,uActsize;

	uActsize= size>>2;
	size=uActsize;
	
	if(size==0)
		return size;
	
	while(size--)
	{
		uTmp = (UINT32)(buf[0]<<24)|(UINT32)(buf[1]<<16)|(UINT32)(buf[2]<<8)|(UINT32)(buf[3]);
		*(volatile UINT32*)(adr+V5_1TSRAM_BASE) = uTmp;		
		buf+=4;
		adr+=4;
	}

	return uActsize;
}
/***************************************************************
Description:
		write sdram 8bit

Parameters:
		adr: sdram start address
		buf: the head point of buf
		size:	  the lenght of data(byte)

Returns:
		void
****************************************************************/

 UINT32 VIM_HIF_WriteSram(UINT32 adr, const HUGE UINT8 *buf, UINT32 size,VIM_HIF_SRAMMODE Mode)
{
	UINT32 FifoNum, ToEnd,addr,len;
	
	switch(Mode)
	{
		case VIM_HIF_SRAM_MODE:
			VIM_USER_MemCpy((UINT8*)adr+V5_1TSRAM_BASE, buf,size);
			break;
		
		case VIM_HIF_FIFO_0_MODE:
		case VIM_HIF_FIFO_1_MODE:		
			//(1) get fifo number
			FifoNum = Mode-3;
			//(2) get size from write cursor to the end of fifo
			ToEnd = gVc0578b_Info.MarbStatus.VFifo[FifoNum].length-gVc0578b_Info.MarbStatus.VFifo[FifoNum].write_cur;
			addr = gVc0578b_Info.MarbStatus.VFifo[FifoNum].address+gVc0578b_Info.MarbStatus.VFifo[FifoNum].write_cur;
			//(3) write data to fifo
			if(size<=ToEnd)
			{
				VIM_USER_MemCpy((UINT8*)addr, buf,size);
				gVc0578b_Info.MarbStatus.VFifo[FifoNum].write_cur+=size;
			}
			else
			{
				VIM_USER_MemCpy((UINT8*)addr, buf,ToEnd);
				len = size - ToEnd;
				addr = gVc0578b_Info.MarbStatus.VFifo[FifoNum].address;
				VIM_USER_MemCpy((UINT8*)addr,buf+ToEnd, len);
				gVc0578b_Info.MarbStatus.VFifo[FifoNum].write_cur=len;
			}
			//(4) set fifo count 
			VIM_MARB_SetFifoCount(FifoNum*2,MARB_MODE_ARMD_WRITE,size);
			break;
	}
	
	return size;
}
#if VIM_USER_SUPPORT_SDRAM
/***************************************************************
Description:
		write sdram 8bit

Parameters:
		adr: sdram start address
		buf: the head point of buf
		size:	  the lenght of data(byte)

Returns:
		void
****************************************************************/

 UINT32 VIM_HIF_WriteSdram(UINT32 adr, const HUGE UINT8 *buf, UINT32 size,VIM_HIF_SRAMMODE Mode)
{
	VIM_USER_MemCpy((UINT8*)adr+V5_1TSRAM_BASE, buf,size);
	return size;
}
/***************************************************************
Description:
		read sdram 8bit

Parameters:
		adr: sdram start address
		buf: the head point of buf
		size:	  the lenght of data(byte)

Returns:
		void
****************************************************************/

UINT32 VIM_HIF_ReadSdram(UINT32 adr,  HUGE UINT8 *buf, UINT32 size,VIM_HIF_SRAMMODE Mode)
{
	VIM_USER_MemCpy(buf,(UINT8*)adr+V5_1TSRAM_BASE, size);
	return size;
}
#endif
/***************************************************************
Description:
		write sdram 8bit on word, 
		Note: the high 16bit is no use, for the parameter is UINT16

Parameters:
		adr: sdram start address
		buf: the head point of buf
		size:	  the lenght of data(byte)

Returns:
		void
****************************************************************/
// size in byte unit
UINT32 VIM_HIF_WriteSramOnWord(UINT32 adr, UINT16 dat, UINT32 size)
{
	UINT32 uActsize;
	uActsize = size;
	
	while(size)
	{
		*(volatile UINT16*)(adr+V5_1TSRAM_BASE) = dat;
		adr+=2;
		*(volatile UINT16*)(adr+V5_1TSRAM_BASE) = dat;		
		adr+=2;
		size-=4;
	}
	
	return uActsize;
}

#endif // VIM_BUSTYPE

////////////////////////end internal function/////////////////////

static const UINT8 g_bFirstLevelIntPriority[VIM_FIRST_LEVEL_INT_NUM]= 
{
	VM_HIF_INT_SIF, VM_HIF_INT_ISP, VM_HIF_INT_LBUF, VM_HIF_INT_JPEG,
	VM_HIF_INT_IPP, VM_HIF_INT_MARB, VM_HIF_INT_LCDC, VM_HIF_INT_TVDC,
	VM_HIF_INT_VDEC, VM_HIF_INT_HWT,VM_HIF_INT_VENC, VM_HIF_INT_GPIO
};

/***************************************************************
Description:
		set multi 16 mode

Parameters:
		Timing:
			abi delay (0~32ns)max is 0x18
			wr delay (0~32ns)max is 0xb
			cs delay (0~32ns)max is 0x6

Returns:
		void
****************************************************************/

void VIM_HIF_SetMulti16(void)
{
#if (BOARD_TYPE==VC0820_FPGA)
	// no need set, by lizg
#else
	UINT32	temp,i;
	//VIM_SET_NORMAL_BY_GPIO();

	VIM_SET_RSLOW_BY_GPIO();
	*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((V5_REG_CPM_ABI_TEST>>24)&0xff);
	*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((V5_REG_CPM_ABI_TEST>>16)&0xff);
	*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((V5_REG_CPM_ABI_TEST>>8)&0xff);
	*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((V5_REG_CPM_ABI_TEST&0xfc));			
	VIM_SET_RSHIGH_BY_GPIO();
	for(i=0; i<10; i++);
		
	temp = ((UINT32)(*(volatile UINT8 *)VIM_REG_VALUE)<<24);
	temp |= ((UINT32)(*(volatile UINT8 *)VIM_REG_VALUE)<<16);
	temp |= ((UINT32)(*(volatile UINT8 *)VIM_REG_VALUE)<<8);
	temp |= ((UINT32)(*(volatile UINT8 *)VIM_REG_VALUE));
	
#if(VIM_HIF_DEBUG)&&(VIM_5XRDK_DEBUG)
		VIM_USER_PrintHex("V5_REG_CPM_ABI_TEST=",temp);
#endif

	VIM_SET_RSLOW_BY_GPIO();
	*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((V5_REG_CPM_ABI_CFG>>24)&0xff);
	*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((V5_REG_CPM_ABI_CFG>>16)&0xff);
	*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((V5_REG_CPM_ABI_CFG>>8)&0xff);
	*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((V5_REG_CPM_ABI_CFG&0xfc));			/*!< 地址是4的倍数 */
	VIM_SET_RSHIGH_BY_GPIO();
	for(i=0; i<10; i++);
		
	temp = ((UINT32)(*(volatile UINT8 *)VIM_REG_VALUE)<<24);
	temp |= ((UINT32)(*(volatile UINT8 *)VIM_REG_VALUE)<<16);
	temp |= ((UINT32)(*(volatile UINT8 *)VIM_REG_VALUE)<<8);
	temp |= ((UINT32)(*(volatile UINT8 *)VIM_REG_VALUE));

#if(VIM_HIF_DEBUG)&&(VIM_5XRDK_DEBUG)
	VIM_USER_PrintHex("V5_REG_CPM_ABI_CFG=",temp);
#endif

	VIM_SET_RSLOW_BY_GPIO();
	*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((V5_REG_CPM_ABI_CFG&0xff000000)>>24);
	*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((V5_REG_CPM_ABI_CFG&0xff0000)>>16);
	*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((V5_REG_CPM_ABI_CFG&0xff00)>>8);
	*(volatile UINT8 *)(VIM_REG_INDEX) = (UINT8)((V5_REG_CPM_ABI_CFG&0xfc)|1);	//地址是4的倍数

	VIM_SET_RSHIGH_BY_GPIO();

	*(volatile UINT8 *)(VIM_REG_VALUE) = (UINT8)(0x01);    	//set multi16	
	*(volatile UINT8 *)(VIM_REG_VALUE) = (UINT8)(0x18);    		
	*(volatile UINT8 *)(VIM_REG_VALUE) = (UINT8)(0x0b);    		
	*(volatile UINT8 *)(VIM_REG_VALUE) = (UINT8)(0x06);     


	temp=VIM_HIF_GetReg32(V5_REG_CPM_ABI_CFG);

	
#if(VIM_HIF_DEBUG)&&(VIM_5XRDK_DEBUG)
		VIM_USER_PrintHex("V5_REG_CPM_ABI_CFG=",temp);
#endif

	//VIM_SET_BYPASS_BY_GPIO();
#endif	
}

/***************************************************************
Description:
		set hif internal delay sel

Parameters:
		Timing:
			abi delay (0~32ns)max is 0x18
			wr delay (0~32ns)max is 0xb
			cs delay (0~32ns)max is 0x6

Returns:
		void
****************************************************************/

void VIM_HIF_SetDelay(PVIM_TIMING Timing)
{
#if (BOARD_TYPE==VC0820_FPGA)
	// no need set, by lizg
#else
UINT32 uTemp=0;
	uTemp=VIM_HIF_GetReg32(V5_REG_BIU_CONFIG);
	uTemp&=0xff000000;
	uTemp|=((UINT32)Timing->AbiDelay<<16);

	uTemp|=((UINT32)Timing->WrDelay<<8);
	uTemp|=(UINT32)Timing->CsDelay;
	VIM_HIF_SetReg32(V5_REG_BIU_CONFIG, uTemp);
#endif	
}



/***************************************************************
Description:
		init bypass mode
		
Parameters:
		byAddr: bypass address (if register mode)
		Type: bypass type (use gpio or register)
Returns:
		void
****************************************************************/
void VIM_HIF_InitBypass(void)
{
#if (BOARD_TYPE==VC0820_FPGA)
	// no need set, by lizg
#else
	UINT32 uTemp;
	// Set bypass type
   	 VIM_HIF_SetReg32(V5_REG_CPM_BYPASS_SEL, VIM_BYPASSMODE);
	// Set Address of bypass register
	VIM_HIF_SetReg32(V5_REG_CPM_BYPASSMODE_ADDR, VIM_BYPASS_ADDR);
	
	uTemp=VIM_HIF_GetReg32(V5_REG_CPM_BP_LCD_CFG);
	uTemp&=(~BIT1);
	uTemp&=(~BIT6);
	uTemp|=VIM_BYPASS_SUBPANEL;
	VIM_HIF_SetReg32(V5_REG_CPM_BP_LCD_CFG, uTemp);
#endif	
}
/***************************************************************
Description:
		set bypass mode or normal mode
		
Parameters:
		byOrNormal: 需要设置进去的special register的值
		BIT2表示是否是bypass模式
		Type: bypass type (use gpio or register)
Returns:
		void
****************************************************************/
void VIM_HIF_SetBypassOrNormal(UINT32 byOrNormal)
{
#if (BOARD_TYPE==VC0820_FPGA)
	// no need set, by lizg
#else
  	if(VIM_BYPASSMODE==VIM_BYPASS_USE_REG)
  	{
		VIM_HIF_SetSpecialReg(byOrNormal);
		//VIM_HIF_SetReg32(VIM_BYPASS_ADDR,byOrNormal);
  	}
  	else
  	{	
  		if((byOrNormal&BIT2)==VIM_HIF_NORMALTYPE)
  			VIM_SET_NORMAL_BY_GPIO();
  		else
  			VIM_SET_BYPASS_BY_GPIO();
  	}
#endif	
}

/***************************************************************
Description:
		switch panel cs
		
Parameters:
		byAddr: bypass address (if register mode)
		Type: bypass type (use gpio or register)
Returns:
		void
****************************************************************/
void VIM_HIF_SwitchPanelCs(UINT8 Panel)
{
#if (BOARD_TYPE==VC0820_FPGA)
	// TBD, by lizg
#else
	UINT32 bValue;
	if(VIM_BYPASS_SUBPANEL==VIM_BYPASS_USEGPIO_SUBPANEL)
	{
		if(Panel==VIM_LCDIF_CS1)
			VIM_CHANGETO_MAINPANEL();
		else
			VIM_CHANGETO_SUBPANEL();
	}
	else if(VIM_BYPASS_SUBPANEL==VIM_BYPASS_USEREGISTER_SUBPANEL)
	{
			bValue=VIM_HIF_GetReg32(V5_REG_CPM_BP_LCD_CFG);
			bValue&=(~BIT2);
			bValue|=(Panel);
			VIM_HIF_SetReg32(V5_REG_CPM_BP_LCD_CFG,bValue);
	}
#endif	
}

/***************************************************************
Description:
		switch panel cs
		
Parameters:
		VIM_HIF_LCD_0_15 = 0x0,
		VIM_HIF_LCD_2_17= 0x1,
		VIM_HIF_LCD_1_8_10_17=0X02,
		VIM_HIF_LCD_0_7_8_15=0X03,
		VIM_HIF_LCD_1_16=0X04
Returns:
		void
****************************************************************/
void VIM_HIF_SetLcdDataBus(VIM_HIF_LCDBYPASSDATA Mode)
{
#if (BOARD_TYPE==VC0820_FPGA)
	// TBD, by lizg
#else
	UINT8 bValue;
	bValue=VIM_HIF_GetReg8(V5_REG_CPM_BP_LCD_CFG);
	bValue&=(~(BIT3|BIT4|BIT5));
	bValue|=(Mode<<3);
	VIM_HIF_SetReg8(V5_REG_CPM_BP_LCD_CFG,bValue);
#endif		
}
/***************************************************************
Description:
		set lcd p clock (only for rgb panel)
		
Parameters:
		clock(k hz)
Returns:
		result of init pll
		VIM_ERROR_PLL_ROVER;
		VIM_ERROR_PLL_TVOUT
		VIM_SUCCEED
****************************************************************/
VIM_RESULT VIM_HIF_SetLcdPclock(UINT32 clock)
{

#if (BOARD_TYPE==VC0820_FPGA)
	// TBD, by lizg
	return VIM_SUCCEED;
#else
	UINT32 bDivider;
	bDivider=VIM_PLLOUT/clock;
	if(bDivider>255)
		return VIM_ERROR_PLL_LCDPCLOCK;
	VIM_HIF_SetReg8(V5_REG_CPM_CLK_CTRL+3,bDivider);
	return VIM_SUCCEED;
#endif	

	    
			
}

/***************************************************************
Description:
		init pll 
		pllout=pllin*n/r
		0.2mhz<pllin/r<6mhz
		500mhz<(pllin/r)*2*n<900mhz
		2<=R<=63
		2<=N<=2047		
Parameters:
		Information of pll
		UINT32 ClockIn;	
		UINT32 ClockOut;	
		UINT8 Mclk;		
		UINT8 Hclk;		
		UINT8 TVLowClk;			
		UINT8 TVHighClk;	

Returns:
		result of init pll
		VIM_ERROR_PLL_ROVER;
		VIM_ERROR_PLL_TVOUT
		VIM_SUCCEED
****************************************************************/
VIM_RESULT VIM_HIF_InitPll(PVIM_CLK ClkInfo)
{
#if (BOARD_TYPE==VC0820_FPGA)
	// TBD, by lizg
	return VIM_SUCCEED;
#else

	UINT32 dwValue;
	UINT8 R=ClkInfo->ClockIn/1000-1;
	UINT16 N=0;
	while(R++)
	{
		N=R*VIM_PLLOUT/ClkInfo->ClockIn;
		if(2<=N<=2047)
		{
			if((N*ClkInfo->ClockIn)==(R*VIM_PLLOUT))
			{
				dwValue=(ClkInfo->ClockIn/R);
				if((200<=dwValue<=6000)&&(500000<=(dwValue*2*N)<=900000))
				{
#if(VIM_HIF_DEBUG)&&(VIM_5XRDK_DEBUG)
					VIM_USER_PrintDec("[VC0578B][BIU]pll caculate end R=  ",R);
					VIM_USER_PrintDec("[VC0578B][BIU]pll caculate end N=  ",N);
#endif
					break;
				}
			}
		}
		if(R>63)
		{
#if(VIM_HIF_DEBUG)&&(VIM_5XRDK_ERROR)
			VIM_USER_PrintString("[VC0578B][BIU]pll caculate error [ERROR] ");
#endif
			return VIM_ERROR_PLL_ROVER;
		}
	}

	VIM_HIF_SetReg32(V5_REG_CPM_PLL_F_MCU,N);
	VIM_HIF_SetReg32(V5_REG_CPM_PLL_R_MCU,R);
	VIM_HIF_SetReg8(V5_REG_CPM_CLK_CTRL+1,ClkInfo->Hclk);
	VIM_HIF_SetReg8(V5_REG_CPM_CLK_CTRL+2,ClkInfo->Mclk);
	if(VIM_PLLOUT/(ClkInfo->TVLowClk+ClkInfo->TVHighClk)!=54000)
	{
#if(VIM_HIF_DEBUG)&&(VIM_5XRDK_ERROR)
		VIM_USER_PrintString("[VC0578B][BIU]pll TV out  [ERROR] ");
#endif	
		//return VIM_ERROR_PLL_TVOUT;
	}
	VIM_HIF_SetReg8(V5_REG_CPM_CLK_CTRL,ClkInfo->TVLowClk);
	VIM_HIF_SetReg32(V5_REG_CPM_TVHIGH_CTRL,ClkInfo->TVHighClk);
	return VIM_SUCCEED;
#endif	
}

/********************************************************************************
  Description:
    	返回工作得时钟
  Parameters:
  	khz
  Returns:
  	void
  Remarks: 
*********************************************************************************/

UINT32 VIM_HIF_GetMclock(void)
{
#if (BOARD_TYPE==VC0820_FPGA)
	// clock not sure on FPGA board just make it work same as davinci test.
UINT32 cur_pll,value,CON_L,CON_H;	
	cur_pll=VIM_Sys_Get_current_pll();
	value=VIM_HIF_GetReg32(V5_REG_SYS_CKDVMCON);
	value>>=16;
	CON_L=(0x1f)&value;
	CON_H=((0x1f<<8)&value)>>8;
	value = CON_H+CON_L+2;
	return (cur_pll*1000/(value*2));	
#else

	UINT32 dwOutValue=0,temp;
	temp=VIM_HIF_GetReg8(V5_REG_CPM_CLK_CTRL+2)*2;
	if (temp)
		dwOutValue=VIM_PLLOUT/temp;
	else
	{
#if(VIM_HIF_DEBUG)&&(VIM_5XRDK_ERROR)
		VIM_USER_PrintHex("[VC0578B][BIU] &&&&&& VIM_HIF_GetMclock uValue=",0);

#endif	
		dwOutValue=24000;
	}
	return dwOutValue;
#endif	
}
/********************************************************************************
  Description:
    	返回工作得时钟
  Parameters:
  	khz
  Returns:
  	void
  Remarks: 
*********************************************************************************/

UINT32 VIM_HIF_GetHclock(void)
{
#if (BOARD_TYPE==VC0820_FPGA)
	// TBD, by lizg
	return VIM_SUCCEED;
#else
	UINT32 dwOutValue=0,temp;
	temp=VIM_HIF_GetReg8(V5_REG_CPM_CLK_CTRL+1)*2;
	if (temp)
		dwOutValue=VIM_PLLOUT/temp;
	else
	{
#if(VIM_HIF_DEBUG)&&(VIM_5XRDK_ERROR)
		VIM_USER_PrintHex("[VC0578B][BIU] &&&&&& VIM_HIF_GetMclock uValue=",0);

#endif	
		dwOutValue=24000;
	}
	return dwOutValue;
#endif	
}
/********************************************************************************
  Description:
    	设置GPIO的状态
  Parameters:
  	Num :gpio 几 0－8
  Returns:
  	void
  Remarks: 
*********************************************************************************/
void VIM_HIF_SetGpioInfo(VIM_HIF_GPIOMODE Mode,UINT8 Num)
{
#if (BOARD_TYPE==VC0820_FPGA)
	// TBD, by lizg
#else

	UINT32 temp;
	temp=VIM_HIF_GetReg32(V5_REG_PAD_GPIO_CFG);
	temp|=(1<<Num);
	VIM_HIF_SetReg32(V5_REG_PAD_GPIO_CFG,temp);

	temp=VIM_HIF_GetReg32(V5_REG_PAD_GPIO_MODE);
	temp|=(1<<Num);
	VIM_HIF_SetReg32(V5_REG_PAD_GPIO_MODE,temp);

	temp=VIM_HIF_GetReg32(V5_REG_PAD_GPIO_DIR);
	if(Mode==VIM_GPIO_PUTIN)
		temp|=(1<<Num);
	else 
		temp&=~(1<<Num);
	VIM_HIF_SetReg32(V5_REG_PAD_GPIO_DIR,temp);
#endif	

}

/********************************************************************************
  Description:
    	设置GPIO的值
  Parameters:
  	Num :gpio 几 0－8
  Returns:
  	void
  Remarks: 
*********************************************************************************/
void VIM_HIF_SetGpioValue(UINT8 Num,BOOL Value)
{
#if (BOARD_TYPE==VC0820_FPGA)
	// TBD, by lizg
#else

UINT32 temp;
	temp=VIM_HIF_GetReg32(V5_REG_PAD_GPIO_REN_PC);
	temp|=(Value<<Num);
	VIM_HIF_SetReg32(V5_REG_PAD_GPIO_REN_PC,temp);
#endif	
}
/***************************************************************
Description:
		Set pll status
		
Parameters:
		Mode:
			VIM_HIF_PLLPOWERON
			VIM_HIF_PLLPOWERDOWN
			VIM_HIF_PLLBYPASS
			
Returns:
		void
****************************************************************/

void VIM_HIF_SetPllStatus(VIM_HIF_PLLMODE Mode)
{
#if (BOARD_TYPE==VC0820_FPGA)
	// TBD, by lizg
#else

	switch(Mode)
	{
		case VIM_HIF_PLLPOWERON:
			VIM_HIF_SetReg32(V5_REG_CPM_XCLK_DISABLE,1);//disable output
			VIM_USER_DelayMs(1); 
			VIM_HIF_SetReg32(V5_REG_CPM_PLL_RST_MCU,1);
			VIM_USER_DelayMs(1); 
			VIM_HIF_SetReg32(V5_REG_CPM_PLL_RST_MCU,0);
			VIM_USER_DelayMs(1);
			VIM_HIF_SetReg32(V5_REG_CPM_CLK_BYPASS,0);//bypass disable
			VIM_HIF_SetReg32(V5_REG_CPM_PLL_PD_MCU,0);//power down disable
			VIM_USER_DelayMs(1); 
			VIM_HIF_SetReg32(V5_REG_CPM_XCLK_DISABLE,0);//xclk enable
			VIM_USER_DelayMs(10); 
			break;
		case VIM_HIF_PLLPOWERDOWN:
			VIM_HIF_SetReg32(V5_REG_CPM_XCLK_DISABLE,1);//disable output
			VIM_USER_DelayMs(1); 
			VIM_HIF_SetReg32(V5_REG_CPM_CLK_BYPASS,1);//bypass enable
			VIM_HIF_SetReg32(V5_REG_CPM_PLL_PD_MCU,1);//power down enalbe
			VIM_USER_DelayMs(10); 
			break;
		case VIM_HIF_PLLBYPASS:
			VIM_HIF_SetReg32(V5_REG_CPM_XCLK_DISABLE,1);//disable output
			VIM_USER_DelayMs(1); 
			VIM_HIF_SetReg32(V5_REG_CPM_PLL_RST_MCU,1);
			VIM_USER_DelayMs(1); 
			VIM_HIF_SetReg32(V5_REG_CPM_PLL_RST_MCU,0);
			VIM_USER_DelayMs(1);
			VIM_HIF_SetReg32(V5_REG_CPM_CLK_BYPASS,1);//bypass enable
			VIM_HIF_SetReg32(V5_REG_CPM_PLL_PD_MCU,0);//power down disable
			VIM_USER_DelayMs(1);
			VIM_HIF_SetReg32(V5_REG_CPM_XCLK_DISABLE,0);//xclk enable
			VIM_USER_DelayMs(10); 
			break;
		default:
			break;
	}
#endif	
}



/***************************************************************
Description:
		Set core ldo status including pll power on or off
		
Parameters:
		Val: ldo status
			VIM_LDO_ON
			VIM_LDO_OFF
Returns:
		void
****************************************************************/
void VIM_HIF_SetLdoStatus(VIM_LDOMODE Val)
{

#if (BOARD_TYPE==VC0820_FPGA)
	// TBD, by lizg
#else
	if(Val==VIM_LDO_OFF)
	{
		VIM_HIF_SetSpecialReg(VIM_HIF_NORMALTYPE|VIM_HIF_ISOLATE_CORE|VIM_HIF_POWERUP_VR);
		VIM_USER_DelayMs(1);
		VIM_HIF_SetSpecialReg(VIM_HIF_NORMALTYPE|VIM_HIF_ISOLATE_CORE|VIM_HIF_POWERDOWN_VR);
		VIM_USER_DelayMs(1);
		VIM_HIF_SetReg32(V5_REG_CPM_VDAC_SLEEP_MCU,0);
		VIM_USER_DelayMs(1);		
		VIM_DISABLE_LDO_FORCORE();
		VIM_USER_DelayMs(1);
		VIM_HIF_SetPllStatus(VIM_HIF_PLLPOWERDOWN);
	}
	else
	{
		VIM_HIF_SetSpecialReg(VIM_HIF_NORMALTYPE|VIM_HIF_ISOLATE_CORE|VIM_HIF_POWERDOWN_VR);
		VIM_USER_DelayMs(1);
		VIM_HIF_SetReg32(V5_REG_CPM_CORE_ASYN_RST_MCU,1);
		VIM_USER_DelayMs(1);
		VIM_ENABLE_LDO_FORCORE();
		VIM_USER_DelayMs(1);
		VIM_HIF_SetSpecialReg(VIM_HIF_NORMALTYPE|VIM_HIF_ISOLATE_CORE|VIM_HIF_POWERUP_VR);
		VIM_USER_DelayMs(1);
		VIM_HIF_SetPllStatus(VIM_HIF_PLLPOWERON);
		VIM_HIF_SetReg32(V5_REG_CPM_VDAC_SLEEP_MCU,1);
		VIM_USER_DelayMs(1);		
		VIM_HIF_SetReg32(V5_REG_CPM_CORE_ASYN_RST_MCU,0);
		VIM_USER_DelayMs(1);		
		VIM_HIF_SetSpecialReg(VIM_HIF_NORMALTYPE|VIM_HIF_RELATE_CORE|VIM_HIF_POWERUP_VR);
		VIM_USER_DelayMs(10);
	}
#endif	
}
/***************************************************************
Description:
		set pins value in the bypass mode
		当isolate置上的时候，这些值则马上起效

Parameters:

Returns:
		void
****************************************************************/
void VIM_HIF_SetExterPinMode(PVIM_EXTERNPINS_INFO PinsMode)
{
#if (BOARD_TYPE==VC0820_FPGA)
	// TBD, by lizg
#else
	UINT32 uValue=0;
		//uValue=((PinsMode->Interrupt.Mode)<<3);
		uValue|=((PinsMode->Interrupt.Type)<<1);
		uValue|=(PinsMode->Interrupt.OutValue);
		VIM_HIF_SetReg32(V5_REG_CPM_INT_MCU,uValue);
		//uValue=((PinsMode->LcdReset.Mode)<<2);
		uValue|=((PinsMode->LcdReset.Type&BIT0)<<1);
		uValue|=(PinsMode->LcdReset.OutValue);
		VIM_HIF_SetReg32(V5_REG_CPM_LCD_RST_MCU,uValue);	
	
		uValue=0;
		uValue|=((PinsMode->SensorClk.Type&BIT0)<<8);
		uValue|=((PinsMode->SensorReset.Type&BIT0)<<9);
		uValue|=((PinsMode->SensorPowerDown.Type&BIT0)<<10);
		uValue|=((PinsMode->SensorEnable.Type&BIT0)<<11);
		uValue|=((PinsMode->SensorSck.Type&BIT0)<<12);
		uValue|=((PinsMode->SensorSda.Type&BIT0)<<13);
		uValue|=((PinsMode->SensorVsyn.Type&BIT0)<<14);
		uValue|=((PinsMode->SensorHsyn.Type&BIT0)<<15);


		uValue|=(((~(PinsMode->SensorClk.Type&BIT1))&BIT1)<<15);//这个特殊，0为上拉，1为disable
		uValue|=((PinsMode->SensorReset.Type&BIT1)<<16);
		uValue|=((PinsMode->SensorPowerDown.Type&BIT1)<<17);
		uValue|=((PinsMode->SensorEnable.Type&BIT1)<<18);
		uValue|=((PinsMode->SensorSck.Type&BIT1)<<19);
		uValue|=((PinsMode->SensorSda.Type&BIT1)<<20);
		uValue|=((PinsMode->SensorVsyn.Type&BIT1)<<21);
		uValue|=((PinsMode->SensorHsyn.Type&BIT1)<<22);


		uValue|=((PinsMode->SensorClk.OutValue&BIT0));
		uValue|=((PinsMode->SensorReset.OutValue&BIT0)<<1);
		uValue|=((PinsMode->SensorPowerDown.OutValue&BIT0)<<2);
		uValue|=((PinsMode->SensorEnable.OutValue&BIT0)<<3);
		uValue|=((PinsMode->SensorSck.OutValue&BIT0)<<4);
		uValue|=((PinsMode->SensorSda.OutValue&BIT0)<<5);
		uValue|=((PinsMode->SensorVsyn.OutValue&BIT0)<<6);
		uValue|=((PinsMode->SensorHsyn.OutValue&BIT0)<<7);

		
		VIM_HIF_SetReg32(V5_REG_CPM_SENSOR_MCU,uValue);	

			
		/*//uValue=((PinsMode->SensorReset.Mode)<<2);
		uValue|=((PinsMode->SensorReset.Type&BIT1)<<1);
		uValue|=(PinsMode->SensorReset.OutValue);
		VIM_HIF_SetReg32(V5_REG_CPM_CS_RSTN_MCU,uValue);	
		//uValue=((PinsMode->SensorPowerDown.Mode)<<2);
		uValue|=((PinsMode->SensorPowerDown.Type&BIT1)<<1);
		uValue|=(PinsMode->SensorPowerDown.OutValue);
		VIM_HIF_SetReg32(V5_REG_CPM_CS_POWDN_MCU,uValue);	
		//uValue=((PinsMode->SensorEnable.Mode)<<2);
		uValue|=((PinsMode->SensorEnable.Type&BIT1)<<1);
		uValue|=(PinsMode->SensorEnable.OutValue);
		VIM_HIF_SetReg32(V5_REG_CPM_CS_ENABLE_MCU,uValue);*/
		uValue=VIM_HIF_GetReg32(V5_REG_CPM_GPIO_MCU);
#if(VIM_HIF_DEBUG)&&(VIM_5XRDK_DEBUG)
		VIM_USER_PrintHex("[VC0578B][BIU]  V5_REG_CPM_GPIO_MCU uValue=",uValue);
#endif		
		uValue=0;
		uValue|=((PinsMode->GPIO0.Type&BIT0)<<9);
		uValue|=((PinsMode->GPIO1.Type&BIT0)<<10);
		uValue|=((PinsMode->GPIO2.Type&BIT0)<<11);
		uValue|=((PinsMode->GPIO3.Type&BIT0)<<12);
		uValue|=((PinsMode->GPIO4.Type&BIT0)<<13);
		uValue|=((PinsMode->GPIO5.Type&BIT0)<<14);
		uValue|=((PinsMode->GPIO6.Type&BIT0)<<15);
		uValue|=((PinsMode->GPIO7.Type&BIT0)<<16);
		uValue|=((PinsMode->GPIO8.Type&BIT0)<<17);

		uValue|=(((~(PinsMode->GPIO0.Type&BIT1))&BIT1)<<18);//这个特殊，0为上拉，1为disable
		uValue|=((PinsMode->GPIO1.Type&BIT1)<<19);
		uValue|=((PinsMode->GPIO2.Type&BIT1)<<20);
		uValue|=((PinsMode->GPIO3.Type&BIT1)<<21);
		uValue|=((PinsMode->GPIO4.Type&BIT1)<<22);
		uValue|=((PinsMode->GPIO5.Type&BIT1)<<23);
		uValue|=((PinsMode->GPIO6.Type&BIT1)<<24);
		uValue|=((PinsMode->GPIO7.Type&BIT1)<<25);
		uValue|=((PinsMode->GPIO8.Type&BIT1)<<26);

		uValue|=((PinsMode->GPIO0.OutValue&BIT0));
		uValue|=((PinsMode->GPIO1.OutValue&BIT0)<<1);
		uValue|=((PinsMode->GPIO2.OutValue&BIT0)<<2);
		uValue|=((PinsMode->GPIO3.OutValue&BIT0)<<3);
		uValue|=((PinsMode->GPIO4.OutValue&BIT0)<<4);
		uValue|=((PinsMode->GPIO5.OutValue&BIT0)<<5);
		uValue|=((PinsMode->GPIO6.OutValue&BIT0)<<6);
		uValue|=((PinsMode->GPIO7.OutValue&BIT0)<<7);
		uValue|=((PinsMode->GPIO8.OutValue&BIT0)<<8);

#if(VIM_HIF_DEBUG)&&(VIM_5XRDK_DEBUG)
		VIM_USER_PrintHex("[VC0578B][BIU]  V5_REG_CPM_GPIO_MCU uValue=",uValue);
#endif		
		VIM_HIF_SetReg32(V5_REG_CPM_GPIO_MCU,uValue);
	/*	uValue=0;
		uValue|=((PinsMode->GPIO0.Mode&BIT1));
		uValue|=((PinsMode->GPIO1.Mode&BIT1)<<1);
		uValue|=((PinsMode->GPIO2.Mode&BIT1)<<2);
		uValue|=((PinsMode->GPIO3.Mode&BIT1)<<3);
		uValue|=((PinsMode->GPIO4.Mode&BIT1)<<4);
		uValue|=((PinsMode->GPIO5.Mode&BIT1)<<5);
		uValue|=((PinsMode->GPIO6.Mode&BIT1)<<6);
		uValue|=((PinsMode->GPIO7.Mode&BIT1)<<7);
		uValue|=((PinsMode->GPIO8.Mode&BIT1)<<8);
		VIM_HIF_SetReg32(V5_REG_CPM_GPIOSEL_MCU,uValue);*/
#endif
}

/***********************************************************************************
Description:
		software reset vc0578b sub module registers.
		
Parameters:
		module	: one of the following <U>VM_HIF_RESETMODULE</U>
 				enumeration values that specify vc0578b sub module.
		
Returns:
		success	: VM_SUCCEED.
		failure	: other error value
		
Remarks:
		
************************************************************************************/
void VIM_HIF_ResetSubModule(VIM_HIF_RESETMODULE wVal)
{
#if (BOARD_TYPE==VC0820_FPGA)
	UINT32 mode;
	// reset sif
	VIM_HIF_SetReg32(V5_REG_SW_RESET, (UINT32)(wVal));
	VIM_USER_DelayMs(1);
	VIM_HIF_SetReg32(V5_REG_SW_RESET, 0);	
//revised by youhai begin 2008.1.3
       if(wVal==VIM_HIF_RESET_SIF)
       	{
	// reset sensor
	#if 0     //zhangxiang del  09/01/14
	mode = VIM_HIF_GetReg32(V5_REG_SIF_SNR_MODE);
	VIM_HIF_SetReg32(V5_REG_SIF_SNR_MODE, (mode|0x01));
	VIM_USER_DelayMs(1);
	VIM_HIF_SetReg32(V5_REG_SIF_SNR_MODE, (mode&0xfffffffe));
	VIM_USER_DelayMs(200);
   #endif
	 }
//revised by youhai begin 2008.1.3
#else
	VIM_HIF_SetReg32(V5_REG_CLK_RST_CTRL, (UINT32)(wVal));
	VIM_USER_DelayMs(1);
	VIM_HIF_SetReg32(V5_REG_CLK_RST_CTRL, 0);	
#endif	
}

/***********************************************************************************
Description:
		software 开关各个模块的clock
		
Parameters:
		module	: one of the following <U>VM_HIF_RESETMODULE</U>
 				enumeration values that specify vc0578b sub module.
 	VIM_HIF_CLKGATE_SIF		= (1<<0),		
	VIM_HIF_CLKGATE_ISP		= (1<<1),		
	VIM_HIF_CLKGATE_IPP		= (1<<2),		
	VIM_HIF_CLKGATE_LBUF		= (1<<3),		
	VIM_HIF_CLKGATE_LCD		= (1<<4),		
	VIM_HIF_CLKGATE_VDEC		= (1<<5),		
	VIM_HIF_CLKGATE_VENC		= (1<<6),		
	VIM_HIF_CLKGATE_JPEG		= (1<<7),		
	VIM_HIF_CLKGATE_ISPCLK	= (1<<8),		
	VIM_HIF_CLKGATE_LBUFCLK	= (1<<9),		
	VIM_HIF_CLKGATE_MARB		= (1<<10),		
	VIM_HIF_CLKGATE_RAM1T0	= (1<<11),		
	VIM_HIF_CLKGATE_RAM1T1	= (1<<12),		
	VIM_HIF_CLKGATE_RAM1T2	= (1<<13),		
	VIM_HIF_CLKGATE_RAM1T3	= (1<<14),		
	VIM_HIF_CLKGATE_SDRC		= (1<<15),		
	VIM_HIF_CLKGATE_BIU		= (1<<16),		
	VIM_HIF_CLKGATE_PADC		= (1<<18),		
	VIM_HIF_CLKGATE_IRQ		= (1<<19),		
	VIM_HIF_CLKGATE_TV		= (1<<20),		
	VIM_HIF_CLKGATE_TVFCLK	= (1<<21),		
	
	VIM_HIF_CLKGATE_ALL		= 0x3FFFFF		
		
Returns:
		success	: VM_SUCCEED.
		failure	: other error value
		
Remarks:
		
************************************************************************************/
void VIM_HIF_SetModClkOnOff(UINT32 wVal,BOOL OnOff)
{
#if (BOARD_TYPE==VC0820_FPGA)
	// TBD, by lizg
#else
	UINT32 wClockSetting = 0;
	wClockSetting = VIM_HIF_GetReg32(V5_REG_CLK_GATE_CTRL);//VIM_HIF_GetModClkVal();
#if(BOARD_TYPE==FPGA)	
	if(OnOff==DISABLE)
#else
	if(OnOff==ENABLE)
#endif
		wClockSetting &= ~wVal;
	else
		wClockSetting |= wVal;
	VIM_HIF_SetReg32(V5_REG_CLK_GATE_CTRL, (UINT32)(wClockSetting));
#endif	
}



/***************************************************************
Description:
		init  interrupt 

Parameters:
		Enable:	
			use PinsMode 's information
		Disable:
			all pin's use for core 
Returns:
		void
****************************************************************/
VIM_RESULT VIM_HIF_InitInerruptLevel(PVIM_INTERRUPT Interrupt)
{
#if (BOARD_TYPE==VC0820_FPGA)
	// TBD, by lizg
	return VIM_SUCCEED;
#else
	UINT32 ctrl=0;
	if(Interrupt->LeverWidth>16)
		return VIM_ERROR_INTERRUPT_INIT;
	switch(Interrupt->Int_Type)
	{
		case VIM_INT_LEVER_LOW:
			ctrl = (1)|(1<<1)|(0<<2)|(Interrupt->LeverWidth<<3);
		// levertype 1 ; activelow(lever) 1; edgeonly 0, sigwidth leverwidth
		break;
		case VIM_INT_LEVER_HIGH:
			ctrl = (1)|(0<<1)|(0<<2)|(Interrupt->LeverWidth<<3);
		// levertype 1 ; activehigh(lever)  0; edgeonly 0, sigwidth leverwidth
		break;
		case VIM_INT_ADGE_DOWN:
			ctrl = (0)|(0<<1)|(1<<2)|(Interrupt->LeverWidth<<3);
		// triggerType 0 ; activehigh(lever only) 0; edgeActiveDown 1, sigwidth leverwidth
		break;
		case VIM_INT_ADGE_UP:
			ctrl = (0)|(0<<1)|(0<<2)|(Interrupt->LeverWidth<<3);
		// triggerType 0 ; activehigh(lever only) 0; edgeActiveUp 0, sigwidth leverwidth
		break;
		default:
		break;
		}
	VIM_HIF_SetReg32(V5_REG_INT_CTRL,ctrl);
	return VIM_SUCCEED;
#endif	
}


/********************************************************************************
  Description:
  	disable or enable interrupt by module
  Parameters:
  Returns:
  	void
  Remarks:
*********************************************************************************/

void VIM_HIF_SetIntModuleEn(VIM_HIF_INT_TYPE byIntEnEnum,BOOL Enable)
{
#if (BOARD_TYPE==VC0820_FPGA)
	if(Enable)
		VIM_INT_Enable(byIntEnEnum);
	else
		VIM_INT_Disable(byIntEnEnum);
#else

	UINT32 bTemp;
	bTemp = VIM_HIF_GetReg32(V5_REG_INT_ENABLE_FIRST);
	if(Enable==DISABLE)
	{
		if(byIntEnEnum==INT_ALL)
			bTemp=0;
		else
			bTemp &=(~ (0x1<<byIntEnEnum));
	}
	else
	{
		if(byIntEnEnum==INT_ALL)
			bTemp=0xff;
		else
			bTemp |=(0x1<<byIntEnEnum);		
	}
	VIM_HIF_SetReg32(V5_REG_INT_ENABLE_FIRST, bTemp);
#endif		
}
/********************************************************************************
  Description:
  	get the second lever interrupt flag 
  Parameters:
  Returns:
  	void
  Remarks:
*********************************************************************************/
#if 1
UINT32 VIM_HIF_GetIntFlagSec(VIM_HIF_INT_TYPE byOffset)        //二级  中断标志
{
#if (BOARD_TYPE==VC0820_FPGA)
	return VIM_INT_Get2ndStatus(byOffset);

#else

	UINT8 x;
	x = VIM_HIF_GetReg32(V5_REG_INT_FLAG_SIF+(byOffset<<2));

	return  x;
#endif		

}


/********************************************************************************
  Description:
  	get the second lever enable
  Parameters:
  Returns:
  	void
  Remarks:
*********************************************************************************/

UINT16 VIM_HIF_GetIntEnableSec(VIM_HIF_INT_TYPE byOffset)
{
#if (BOARD_TYPE==VC0820_FPGA)
	UINT32 en_flag;
	// byOffset should be first level interrupt number
	en_flag = VIM_INT_Get2ndEnable(byOffset);
	return (UINT16)en_flag;
#else
	UINT16 x;
	x = (UINT16)VIM_HIF_GetReg16(V5_REG_INT_ENABLE_SIF+(byOffset<<2));
    return x;
#endif		
}
/********************************************************************************
  Description:
  	set the second lever enable
  Parameters:
  Returns:
  	void
  Remarks:
*********************************************************************************/

void VIM_HIF_SetIntEnableSec(VIM_HIF_INT_TYPE byOffset, UINT16 uVal)
{
#if (BOARD_TYPE==VC0820_FPGA)
	// byOffset should be first level interrupt number
	VIM_INT_Set2ndEnable(byOffset,uVal);
#else
	VIM_HIF_SetReg32(V5_REG_INT_ENABLE_SIF+(byOffset<<2), (UINT32)uVal);
#endif
}

/********************************************************************************
  Description:
  	clear interrupt by module
  Parameters:
  Returns:
  	void
  Remarks:
*********************************************************************************/

void VIM_HIF_ClearIntModule(VIM_HIF_INT_TYPE byIntEnEnum)
{
#if (BOARD_TYPE==VC0820_FPGA)
	// TBD, by lizg
#else
	UINT8  i = 0;
	VIM_HIF_SetIntModuleEn(byIntEnEnum,DISABLE);
	if(byIntEnEnum<INT_ALL)
	{
		VIM_HIF_SetIntEnableSec(byIntEnEnum,DISABLE);	
		gVc0578b_Isr.bSecondLevelIntFlag[byIntEnEnum]=0;
		VIM_HIF_SetReg32(V5_REG_INT_SERV_FIRST, 0x1<<byIntEnEnum);		/*!< opne int_serv */
		VIM_HIF_GetIntFlagSec((VIM_HIF_INT_TYPE)byIntEnEnum);
		VIM_HIF_SetReg32(V5_REG_INT_SERV_FIRST, 0);	
	}
	else
	{
		VIM_USER_MemSet((UINT8 *)&gVc0578b_Isr,0,sizeof(VIM_HIF_TISR));// ??addd?
		 for(i=0; i<VIM_FIRST_LEVEL_INT_NUM; i++)
		{
			VIM_HIF_SetIntEnableSec((VIM_HIF_INT_TYPE)i,DISABLE);	//angela 2006-12-13
			VIM_HIF_SetReg32(V5_REG_INT_SERV_FIRST, 0x1<<byIntEnEnum);		/*!< opne int_serv */
			VIM_HIF_GetIntFlagSec((VIM_HIF_INT_TYPE)i);
			VIM_HIF_SetReg32(V5_REG_INT_SERV_FIRST, 0);	
		}
	}
#endif	
}
/********************************************************************************
  Description:
  	中断服务函数
  Parameters:
  Returns:
  	void
  Remarks:
*********************************************************************************/

void VIM_HIF_HandleISR(VIM_HIF_INT_TYPE byFirstLevelInt, UINT16 bSecondLevelIntFlag)
{
#if (BOARD_TYPE==VC0820_FPGA)
	// TBD, by lizg
#else
	switch (byFirstLevelInt)
	{
		case VM_HIF_INT_SIF:
			_ISR_SifSifIntHandle(bSecondLevelIntFlag);
			break;
#if (BOARD_TYPE==VC0820_FPGA)
#else	
		case VM_HIF_INT_ISP:
			_ISR_SifIspIntHandle(bSecondLevelIntFlag);
			break;
#endif
		case VM_HIF_INT_LBUF:
			_ISR_JpegLbufIntHandle(bSecondLevelIntFlag);
			break;
		case VM_HIF_INT_JPEG:
			_ISR_JpegJpegIntHandle(bSecondLevelIntFlag);
			break;
		case VM_HIF_INT_IPP:
			break;
		case VM_HIF_INT_MARB:
			_ISR_MarbIntHandle(bSecondLevelIntFlag);
			break;
		case VM_HIF_INT_LCDC:
			_ISR_DispIntHandle(bSecondLevelIntFlag);
			break;
		case VM_HIF_INT_TVDC:
			break;
		case VM_HIF_INT_VDEC:
			_ISR_Mpeg4DecIntHandle(bSecondLevelIntFlag);
			break;
		case VM_HIF_INT_VENC:
			_ISR_Mpeg4EncIntHandle(bSecondLevelIntFlag);
			break;
		case VM_HIF_INT_HWT:
			  _ISR_Mpeg4TimerIntHandle(bSecondLevelIntFlag);
			break;
		case VM_HIF_INT_GPIO:
			break;
		default:
			break;
	}
#endif
}
/*Interrupt process*/
/***************************************************************
  Summary:
	Interrupt inr handle
  Parameters:
	void
  Note: 
  	
  Returns:
  Remarks:

	
**************************************************************/
VIM_RESULT _ISR_HIF_IntHandle(void)
{
#if 0
#if (BOARD_TYPE==VC0820_FPGA)
	UINT32  i, intEn, intFlg,x, mask_578;
	UINT16 byIntNum = 0;
#if VIM_EXTERN_INTERRUPT
	UINT32 uWaitlen=VIM_WAITMAX;
#endif
	/*if(gVc0578b_Isr.byIntFlag)
	{
#if(VIM_HIF_DEBUG)&&(VIM_5XRDK_ERROR)
		VIM_USER_PrintString("[VC0578B][BIU]  _ISR_HIF_IntHandle enter return");
#endif	
		return VIM_SUCCEED;	
	}*/

	//interrupt protect
#if VIM_EXTERN_INTERRUPT
	VIM_HIF_SetReg32(V5_REG_BIU_BREAK_OCCUR, 0x01);
#endif

	mask_578 = (1<<VM_HIF_INT_SIF)|(1<<VM_HIF_INT_ISP)|(1<<VM_HIF_INT_LBUF)
				|(1<<VM_HIF_INT_JPEG)|(1<<VM_HIF_INT_IPP)|(1<<VM_HIF_INT_MARB)
				|(1<<VM_HIF_INT_LCDC)|(1<<VM_HIF_INT_TVDC)|(1<<VM_HIF_INT_VDEC)
				|(1<<VM_HIF_INT_VENC);
	//intEn = VIM_HIF_GetReg16(V5_REG_INT_ENABLE_FIRST);
	intEn = VIM_INT_Get1stEnable();
	//intFlg = VIM_HIF_GetReg16(V5_REG_INT_FLAG_FIRST);
	intFlg = VIM_INT_Get1stStatus();

	intEn &= mask_578;
	intFlg &= mask_578;
	
	if(0 == intEn || 0 == intFlg)
	{
#if VIM_EXTERN_INTERRUPT
		//interrupt protect
		while(uWaitlen--){
			if(VIM_HIF_GetReg32(V5_REG_BIU_BREAK_FINISH))
				break;
		}
#endif

		return VIM_SUCCEED;
	}
#if(VIM_HIF_DEBUG)&&(VIM_5XRDK_DEBUG)
		VIM_USER_PrintHex("[VC0578B][BIU]  intFlg=",intFlg);
#endif	
	byIntNum = 0;
#if(BOARD_TYPE!=VC0820_FPGA)
	VIM_HIF_SetReg32(V5_REG_INT_SERV_FIRST, intFlg);		/*!< opne int_serv */
#endif
	for(i=0; i<VIM_FIRST_LEVEL_INT_NUM; i++)
	{
		x = (UINT32)(0x1<<g_bFirstLevelIntPriority[i]);
		if((intEn&x) && (intFlg&x))
		{
			gVc0578b_Isr.byFirstLevelInt[byIntNum]=g_bFirstLevelIntPriority[i];
			gVc0578b_Isr.bSecondLevelIntFlag[byIntNum] = VIM_HIF_GetIntFlagSec((VIM_HIF_INT_TYPE)g_bFirstLevelIntPriority[i]);	
			byIntNum++;
		}
	}
#if(BOARD_TYPE==VC0820_FPGA)
	// clear 1st and 2nd level status
	for(i=0; i<VIM_FIRST_LEVEL_INT_NUM; i++)
	{
		x = (UINT32)(0x1<<g_bFirstLevelIntPriority[i]);
		if((intEn&x) && (intFlg&x))
		{
			VIM_INT_Acknowledge(g_bFirstLevelIntPriority[i]);
		}
	}
#else
	VIM_HIF_SetReg32(V5_REG_INT_SERV_FIRST, 0);	
#endif
	for(i=0; i< byIntNum; i++)
	{
#if(VIM_HIF_DEBUG)&&(VIM_5XRDK_DEBUG)
		VIM_USER_PrintHex("[VC0578B][BIU]  interrupt gVc0528_Isr.byFirstLevelInt[byIntNum]=",gVc0578b_Isr.byFirstLevelInt[i]);
		VIM_USER_PrintHex("[VC0578B][BIU] interrupt gVc0528_Isr.bSecondLevelIntFlag[i]=",gVc0578b_Isr.bSecondLevelIntFlag[i]);
#endif
		VIM_HIF_HandleISR((VIM_HIF_INT_TYPE)gVc0578b_Isr.byFirstLevelInt[i],gVc0578b_Isr.bSecondLevelIntFlag[i]);
	}

	//interrupt protect
#if VIM_EXTERN_INTERRUPT
	while(uWaitlen--){
		if(VIM_HIF_GetReg32(V5_REG_BIU_BREAK_FINISH))
			break;
	}
	if(!uWaitlen)
	{
#if(VIM_HIF_DEBUG)&&(VIM_5XRDK_ERROR)
		VIM_USER_PrintString("[VC0578B][BIU] interrupt break off over time [ERROR] ");
#endif	
		return VIM_ERROR_INTERRUPT_BREAKOFF;
	}
#endif
	return VIM_SUCCEED;

#else
	UINT16  i, intEn, intFlg,x;
	UINT16 byIntNum = 0;
#if VIM_EXTERN_INTERRUPT
	UINT32 uWaitlen=VIM_WAITMAX;
#endif
	/*if(gVc0578b_Isr.byIntFlag)
	{
#if(VIM_HIF_DEBUG)&&(VIM_5XRDK_ERROR)
		VIM_USER_PrintString("[VC0578B][BIU]  _ISR_HIF_IntHandle enter return");
#endif	
		return VIM_SUCCEED;	
	}*/

	//interrupt protect
#if VIM_EXTERN_INTERRUPT
	VIM_HIF_SetReg32(V5_REG_BIU_BREAK_OCCUR, 0x01);
#endif

	intEn = VIM_HIF_GetReg16(V5_REG_INT_ENABLE_FIRST);
	intFlg = VIM_HIF_GetReg16(V5_REG_INT_FLAG_FIRST);
	
	if(0 == intEn || 0 == intFlg)
	{
#if VIM_EXTERN_INTERRUPT
		//interrupt protect
		while(uWaitlen--){
			if(VIM_HIF_GetReg32(V5_REG_BIU_BREAK_FINISH))
				break;
		}
#endif

		return VIM_SUCCEED;
	}
#if(VIM_HIF_DEBUG)&&(VIM_5XRDK_DEBUG)
		VIM_USER_PrintHex("[VC0578B][BIU]  intFlg=",intFlg);
#endif	
	byIntNum = 0;
	VIM_HIF_SetReg32(V5_REG_INT_SERV_FIRST, intFlg);		/*!< opne int_serv */
	for(i=0; i<VIM_FIRST_LEVEL_INT_NUM; i++)
	{
		x = (UINT16)(0x1<<g_bFirstLevelIntPriority[i]);
		if((intEn&x) && (intFlg&x))
		{
			gVc0578b_Isr.byFirstLevelInt[byIntNum]=g_bFirstLevelIntPriority[i];
			gVc0578b_Isr.bSecondLevelIntFlag[byIntNum] = VIM_HIF_GetIntFlagSec((VIM_HIF_INT_TYPE)g_bFirstLevelIntPriority[i]);	
			byIntNum++;
		}
	}
	VIM_HIF_SetReg32(V5_REG_INT_SERV_FIRST, 0);	

	for(i=0; i< byIntNum; i++)
	{
#if(VIM_HIF_DEBUG)&&(VIM_5XRDK_DEBUG)
		VIM_USER_PrintHex("[VC0578B][BIU]  interrupt gVc0528_Isr.byFirstLevelInt[byIntNum]=",gVc0578b_Isr.byFirstLevelInt[i]);
		VIM_USER_PrintHex("[VC0578B][BIU] interrupt gVc0528_Isr.bSecondLevelIntFlag[i]=",gVc0578b_Isr.bSecondLevelIntFlag[i]);
#endif
		VIM_HIF_HandleISR((VIM_HIF_INT_TYPE)gVc0578b_Isr.byFirstLevelInt[i],gVc0578b_Isr.bSecondLevelIntFlag[i]);
	}

	//interrupt protect
#if VIM_EXTERN_INTERRUPT
	while(uWaitlen--){
		if(VIM_HIF_GetReg32(V5_REG_BIU_BREAK_FINISH))
			break;
	}
	if(!uWaitlen)
	{
#if(VIM_HIF_DEBUG)&&(VIM_5XRDK_ERROR)
		VIM_USER_PrintString("[VC0578B][BIU] interrupt break off over time [ERROR] ");
#endif	
		return VIM_ERROR_INTERRUPT_BREAKOFF;
	}
#endif
	return VIM_SUCCEED;

#endif
#endif
	return VIM_SUCCEED;
}




#endif
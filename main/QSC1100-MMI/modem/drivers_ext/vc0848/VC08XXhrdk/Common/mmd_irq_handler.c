/*============================================================================================================
  Sample Code
  
  This code is for VC0820 as Host.
  Modify to match your system.                                                                    
============================================================================================================*/

/***********************************************************
  INCLUDE FILE                                              
***********************************************************/
#include "mmp_rdk.h"

#define MMD_IRQ_DEBUG_INFO
#define MMD_IRQ_DEBUG_ERR

#ifdef MMD_IRQ_DEBUG_INFO
    #define PRINTF(WriteVal)        MMD_PRINTF(WriteVal) //general print out
#else
    #define PRINTF(WriteVal)
#endif

#ifdef MMD_IRQ_DEBUG_ERR
    #define PRINTFE(WriteVal)   MMD_PRINTFE(WriteVal) //error
#else
    #define PRINTFE(WriteVal)
#endif

const ModeCallback g_fnLISR[RDK_MODE_NUM_MAX]={
							MMD_Mode_Sys_Lisr, //MAJ_SYS		0x0000
							NULL, //MAJ_PIC		0x0100
							MMD_Mode_Ply_Lisr, //MAJ_PLY		0x0200
							NULL, //MAJ_VID		0x0300
							NULL, //MAJ_IMG		0x0400
							MMD_Mode_File_Lisr, //MAJ_FIL		0x0500
							MMD_Mode_Usb_Lisr,//NULL, //MAJ_USB		0x0600
							NULL, //MAJ_PWR		0x0700
							NULL, //MAJ_AREC		0x0800 /* Audio recode */
							NULL, //MAJ_MREC		0x0900 /* Movie(MPEG4+AMR) recode */
							NULL, //MAJ_LCD		0x0A00 
							MMD_Mode_Rec_Lisr, //MAJ_UNDEFINE	0x0B00 
				
#ifdef CMMB_ENABLE
							MMD_Mode_CMMB_Lisr, //MAJ_UNDEFINE	0x0C00 
#else
							NULL, // 0x0C00
#endif
#ifdef RV_ENABLE
							Mode_RV_Lisr, //0x0D00 
#else
							NULL , //0x0D00 
#endif
							NULL, //MAJ_UNDEFINE	0x0E00 
							NULL, //MAJ_UNDEFINE	0x0F00 
							NULL, //MAJ_UNDEFINE	0x1000 
							NULL, //MAJ_UNDEFINE	0x1100 
							NULL, //MAJ_UNDEFINE	0x1200 
							NULL, //MAJ_UNDEFINE	0x1300 
							NULL, //MAJ_UNDEFINE	0x1400 
							NULL, //MAJ_UNDEFINE	0x1500 
							NULL, //MAJ_UNDEFINE	0x1600 
							NULL, //MAJ_UNDEFINE	0x1700 
							NULL, //MAJ_UNDEFINE	0x1800 
							NULL, //MAJ_UNDEFINE	0x1900 
							NULL, //MAJ_UNDEFINE	0x1A00 
							NULL, //MAJ_UNDEFINE	0x1B00 
							NULL, //MAJ_UNDEFINE	0x1C00 
							NULL, //MAJ_UNDEFINE	0x1D00 
							NULL, //MAJ_UNDEFINE	0x1E00 
							NULL, //Mode_DBG_Process //MAJ_DBG		0x1F00
};

void V820_interrupt_mask(MMD_U32 vector)
{
	MMD_U32 mask;
	mask = Vm_GetReg(DAVINCI_BIUINTEN);
	mask &= ~(1<<vector);
	Vm_SetReg(DAVINCI_BIUINTEN, mask);
}
void V820_interrupt_acknowledge(MMD_U32 vector)
{
	MMD_U32 second_st,data;
	second_st = DAVINCI_SIA2B_ST;

	data=Vm_GetReg(second_st);
	Vm_SetReg(second_st, data);
	Vm_SetReg(DAVINCI_1LvINTClr, (1<<vector));
}
void V820_interrupt_unmask(MMD_U32 vector)
{
	MMD_U32 mask;
	//davinci_interrupt_unmask
	//ARM9
	mask = Vm_GetReg(DAVINCI_BIUINTEN);
	mask |= (1<<vector);
	Vm_SetReg(DAVINCI_BIUINTEN, mask); 
}

/*-----------------------------------------------------------------------------------------------------------
[FUNCTION NAME]
  MMD_IRQ_Handler_Top
[DESCRIPTION]
  Get H/W irq status and wait a time unit is 0.1 ms.
-----------------------------------------------------------------------------------------------------------*/
MMD_U32 MMD_Check_Irq_Staus_And_Wait(MMD_U32 time)
{
#ifdef IRQ_BY_TIMER
	MMD_U32 IRQStatus=0;
	MMD_U32 irq_status;
	MMD_U32 irq_num,cmd;

	irq_status = Vm_GetReg(DAVINCI_BIUIRQST);
	if(irq_status==0)
	{
		MMD_USER_WaitMICRS(time);
		return 0;
	}
	for(irq_num=0;irq_num<30;irq_num++)
	{
		if(irq_status&1)
		{
			break;                  
		}
		irq_status>>=1;
	}
	if(irq_num==22)
	{
		IRQStatus = Vm_GetReg(DAVINCI_SIA2B_ST);
		if((IRQStatus&0x1)==1)
		{
			// only process A2B soft interrupt
			HAL_READ_UINT32(gD2H_ACK_MemAddr[0], cmd );
			g_Ack_MinCmd = cmd & CMD_MASK_MIN;
			HAL_READ_UINT32(gD2H_ACK_MemAddr[1], g_Ack_OP1);
			HAL_READ_UINT32(gD2H_ACK_MemAddr[2], g_Ack_OP2);
		}
		else if((IRQStatus&0x4)==4)
		{
			 g_IPC_A2B0=Vm_GetReg(0x60041000);
		}
		else
			PRINTFE(("test interrupt IRQStatus = 0x%08X\n", IRQStatus));	
	}
	else
	{
		//PRINTFE(("Unknown interrupt irq_status = 0x%08X\n", irq_status));
	}
	V820_interrupt_acknowledge(irq_num);
	return IRQStatus;
#else
	MMD_USER_WaitMICRS(time);
#endif	

}

/*-----------------------------------------------------------------------------------------------------------
[FUNCTION NAME]
  MMD_IRQ_Handler_Top
[DESCRIPTION]
  Get H/W irq status
-----------------------------------------------------------------------------------------------------------*/
MMD_U32 MMD_IRQ_Handler_Top(void)
{
	MMD_U32 IRQStatus=0;
	MMD_U32 irq_status;
	MMD_U32 irq_num,major,cmd;

	irq_status = Vm_GetReg(DAVINCI_BIUIRQST);

	for(irq_num=0;irq_num<30;irq_num++)
	{
		if(irq_status&1)
		{
			break;                  
		}
		irq_status>>=1;
	}
	if(irq_num==22)
	{
		IRQStatus = Vm_GetReg(DAVINCI_SIA2B_ST);
		if((IRQStatus&0x1)==1)
		{
			// only process A2B soft interrupt
			HAL_READ_UINT32(gD2H_ACK_MemAddr[0], cmd );
			major = (cmd & CMD_MASK_MAJOR)>>8;
			if(g_fnLISR[major]!=NULL)
				g_fnLISR[major](cmd);
		}
		else if((IRQStatus&0x4)==4)
		{
			 g_IPC_A2B0=Vm_GetReg(0x60041000);
		}
		else
			PRINTFE(("test interrupt IRQStatus = 0x%08X\n", IRQStatus));	
	}
	else
	{
		PRINTFE(("Unknown interrupt irq_status = 0x%08X\n", irq_status));
	}
	V820_interrupt_acknowledge(irq_num);
	return IRQStatus;
}


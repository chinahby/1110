/*------------------------------------------------------------------------

				COPYRIGHT (C) 2005, Vimicro Corporation
                        	ALL RIGHTS RESERVED

	This source code has been made available to you by VIMICRO on an 
	AS-IS basis. Anyone receiving this source code is licensed under
	VIMICRO copyrights to use it in any way he or she deems fit, including 
	copying it,  modifying it, compiling it, and redistributing it either
	with or without  modifications. Any person who transfers this source 
	code or any derivative  work must include the VIMICRO copyright notice 
	and	this paragraph in the transferred software.


	Overview:
		This module is used for SDIO control.

	
	Updata:

	2006.3.23		Established by Calilon Huang.
	2006.4.13		Need to support byte bases DMA
	2006.4.20		Support none 4 bytes aligned SDIO RW
	2006.08.16		Support eCos Cache Enable
	2006.09.25		Support SDIO none 4byte aligned DMA.
	2006.09.26		Support SDIO multi block read/write.
	2006.09.30		Support 4G sd card, replace old byte addressing model with sector addressing
	2006.10.01		Support sandisk DRM.
	2006.12.12		Support 2 device socket.
	2007.1.16       for single end prefech bug  
-------------------------------------------------------------------------*/
#if 0
#include "def.h"
#include "platform.h"

#if (CURSYSTEM == AASP||CURSYSTEM == ECOS)
#include <AASP_Shell.h>
#include <aasp.h>
#include <interrupt.h>
#endif
#endif

#ifndef ASSERT_FATAL
#define ASSERT_FATAL(x)
#endif 

#include "mmp_rdk.h"
#include "VIM_SD_Driver.h"
#include "Vhostctrl.h"

extern void Uart_Printf(char *fmt,...);
//Module Name
#define MOD_NAME "SD_MODULE"
//Module Version
#define MOD_VER	"0.1"
//Module Description
#define MOD_DESC "SD test"
//Module Owner
#define MOD_OWNER "Calilon Huang"

SD_DEV   g_SdDev;

//SD_DEV   g_SdioDev;
MMD_U16   g_BlockCnt;	//Muti RW BlockCnt
MMD_U08    g_RdAutoCmd12; //fix read multi bug
//MMD_U08	  g_bBuffer[512];
MMD_U08	  g_bSDRspLen[6] = {0, 2, 3, 1, 2, 2};
MMD_U32   g_CurBlocksize=0;
MMD_U32   g_MaxBlocksize=0;

void SD_DisableOtherInt(void);
void SD_EnableOtherInt(void);

#if (SD_HISPEED == 1)
__align(4) MMD_U08 gSDData[64];
#endif

#if 0
#if (SD_HISPEED == 1)
MMD_U08	__HAL_ALIGN(4) gSDData[64] CYGBLD_ATTRIB_SECTION_NCNB_SRAM;
#endif
#if (CURSYSTEM == AASP||CURSYSTEM == ECOS)
aasp_handle_t      SD_handle;
aasp_interrupt     SD_intr;
MMD_U32 g_IntFlag;
#endif
#endif

#define   MAXBUFLEN	1024


void SD_EnableInt(void)
{
	Vhost_WriteReg(0x6005201c,0xff2b|(CARDDETECT<<4)); //disable card detect for Atheros
}

void SD_DisableInt(void)
{
	Vhost_WriteReg(0x6005201c,0x0);
}


#if 0
#if (CURSYSTEM == AASP)
void SD_ISR(void)

{
	AASP_DRV_INTERRUPT_MASK(CYGNUM_HAL_INTERRUPT_STO);
	
	#if 1
	Vsd_IntHandle();
	Vsd_ErrorHandle();
	#endif
	AASP_DRV_INTERRUPT_ACKNOWLEDGE(CYGNUM_HAL_INTERRUPT_STO);
	AASP_DRV_INTERRUPT_UNMASK(CYGNUM_HAL_INTERRUPT_STO);
}
#endif

#if (CURSYSTEM ==ECOS)
MMD_U32 SD_ISR( cyg_vector_t vector, cyg_addrword_t data )
{
//	MMD_U32 i;
	AASP_DRV_INTERRUPT_MASK(CYGNUM_HAL_INTERRUPT_STO);
//	SDMSG(("int sto\n"));
	#if 1
	Vsd_IntHandle();
	Vsd_ErrorHandle();
	#endif
//	SDMSG(("int ok\n"));
	AASP_DRV_INTERRUPT_ACKNOWLEDGE(CYGNUM_HAL_INTERRUPT_STO);
	
	AASP_DRV_INTERRUPT_UNMASK(CYGNUM_HAL_INTERRUPT_STO);
	//i=Vhost_ReadReg(0x60051000);
	//SDMSG(("%x\n",i));
	return CYG_ISR_HANDLED;
}
#endif

void SD_DisableOtherInt(void)
{
	#if (CURSYSTEM ==ECOS)
	
	#ifdef __NEW_SYS__
	HAL_DISABLE_INTERRUPTS(g_IntFlag);
	
	#else
	MMD_U32 en;
	en=Vhost_ReadReg(DAVINCI_HINTEN);
	en &= 0xffffdfff;			//Disable timer
	Vhost_WriteReg(DAVINCI_HINTEN, en);	
	#endif
    #endif
}
void SD_EnableOtherInt(void)
{
	#if (CURSYSTEM ==ECOS)
	#ifdef __NEW_SYS__
	HAL_RESTORE_INTERRUPTS(g_IntFlag);
	#else
	MMD_U32 en;
	en=Vhost_ReadReg(DAVINCI_HINTEN);
    	en |= 0x2000;			//Enable timer
    	Vhost_WriteReg(DAVINCI_HINTEN, en);	
	#endif
    #endif
}
#endif

/********************************************************************
 Description:
		Select socket

 Parameters:
		bSocketID:	
		
		
 Returns:
		
Note:
		

********************************************************************/
void SD_SelectSocket(MMD_U08 socketid)
{
#if (SD_INTERRUPT == 1)
	MMD_U32 timeout = 0;
#endif
	socketid &= 0x1;
#if (SD_INTERRUPT == 1)
	while(g_SdDev.sd_state[g_SdDev.current_socket]!=SD_State_Idle)
	{
		timeout++;
		if(timeout > SD_READDATA_TIMEOUT_CNT)
		{
			SDERR(("Select Socket fail\n"));
			Vhost_WriteReg(VDEV_REG_SD_RST, 0x0F);
			Vhost_WriteReg(VDEV_REG_SD_INT_FLAG, 0xffffffff);
		}
	}
#endif
	Vhost_WriteReg(VDEV_REG_SD_DEIVCE_SELECT, socketid);
	g_SdDev.current_socket = socketid;
}

/********************************************************************
 Description:
		Send general command to STO.

 Parameters:
		bCmdID:	Command id.
		dArgu:	Argument.
		
 Returns:
		Reponse VSD_SUCCESS if success.	
Note:
		CMD 4, 7, 9, 10, 13, 15 , pls pass 16bit argument

********************************************************************/
SDSTATE Vsd_SendCommand(MMD_U08 bCmdID, MMD_U32 dArgu)
{
	MMD_U08 bCmdReg, bRepeat, bResult;
	MMD_U32 dArgReg;
	PSD_DEV pDev;
	MMD_U08 socketid ;
	
	socketid = g_SdDev.current_socket;
	socketid &= 0x1;
	if(g_SdDev.sd_power[socketid] == SD_Power_Work)
	{
		pDev = &g_SdDev;
	}
	else
		return VSD_WORKMODE_ERR;
	
	bRepeat = 0;
	while(bRepeat<SD_SEND_CMD_REPEAT_COUNT)
	{
		dArgReg = dArgu;
		//Default:
		//Auto Stop Mode
		//No data block
		//SD_DIRCTION_WRITE 
		//CRC disable 
		//NoResponse
		bCmdReg = 0;  
		
		//bCmdReg = (SD_CMD_IDX_CHECK|SD_CMD_CRC_CHECK);		
		
		switch(bCmdID)
		{
			case CMD0_GO_IDLE_STATE:
				bCmdReg |= SD_RESPONSE_NONE ;				
				break;
			case CMD1_SD_SEND_OP_COND:
				bCmdReg |= SD_RESPONSE_R3;
				break;
			case CMD2_ALL_SEND_CID:	
				bCmdReg |= SD_RESPONSE_R2;
				break;
			case CMD3_SEND_RELATIVE_ADDR:
				bCmdReg |= SD_RESPONSE_R6;
				bCmdReg |= SD_CMD_CRC_CHECK;
				bCmdReg |= SD_CMD_IDX_CHECK;
				break;
			case CMD4_SET_DSR:		
				bCmdReg |= SD_RESPONSE_NONE;
				dArgReg = (dArgu&0xffff)<<16;				
				break;
			case CMD5_SET_OP_COND:	
				bCmdReg |= SD_RESPONSE_R4;
				//bCmdReg |= SD_CMD_CRC_CHECK;
				dArgReg &= 0x00ffffff; //24bit					
				break;
			case CMD6_SWITCH_FUNC:	
				bCmdReg |= SD_RESPONSE_R1;
				bCmdReg |= (SD_CMD_WITH_DATA_BLK|SD_DATA_READ);
				bCmdReg |= SD_CMD_CRC_CHECK;
				bCmdReg |= SD_CMD_IDX_CHECK;
				Vhost_WriteReg(VDEV_REG_SD_BLK_SIZE, (64-1)); 
				dArgReg &= 0x00ffffff; //24bit					
				break;
			case CMD6_MMC_SWITCH:
				bCmdID =6;
				bCmdReg |= SD_RESPONSE_R1B;			
				bCmdReg |= SD_CMD_CRC_CHECK;
				bCmdReg |= SD_CMD_IDX_CHECK;
				break;
			case CMD8_MMC_SENDEXTCSD:
				bCmdReg |= SD_RESPONSE_R1;
				bCmdReg |= (SD_CMD_WITH_DATA_BLK|SD_DATA_READ);
				bCmdReg |= SD_CMD_CRC_CHECK;
				bCmdReg |= SD_CMD_IDX_CHECK;
				Vhost_WriteReg(VDEV_REG_SD_BLK_SIZE, (SD_BLOCK_SIZE-1)); 			
				bCmdID =8;
				break;	
			case CMD7_SELECT_CARD:	
				bCmdReg |= SD_RESPONSE_R1B;
				dArgReg = (dArgu&0xffff)<<16;
				bCmdReg |= SD_CMD_CRC_CHECK;
				bCmdReg |= SD_CMD_IDX_CHECK;
				break;
			case CMD8_SEND_IF_COND:	
				bCmdReg |= SD_RESPONSE_R7;
				dArgReg = dArgu;
				bCmdReg |= SD_CMD_CRC_CHECK;
				bCmdReg |= SD_CMD_IDX_CHECK;
				break;
			case CMD9_SEND_CSD:		
				bCmdReg |= SD_RESPONSE_R2;
				dArgReg = (dArgu&0xffff)<<16;
				break;
			case CMD10_SEND_CID:		
				bCmdReg |= SD_RESPONSE_R2;
				dArgReg = (dArgu&0xffff)<<16;
				break;
			case CMD12_STOP_TRANSMISSION:
				Vsd_SendAutoCmd(0x0,	0x0);
				bCmdReg |= SD_RESPONSE_R1B;
				bCmdReg |= SD_CMD_CRC_CHECK;
				bCmdReg |= SD_CMD_IDX_CHECK;
				break;
			case CMD13_SEND_STATUS:		
				bCmdReg |= SD_RESPONSE_R1;
				bCmdReg |= SD_CMD_CRC_CHECK;
				bCmdReg |= SD_CMD_IDX_CHECK;
				dArgReg = (dArgu&0xffff)<<16;
				break;
			case CMD15_GO_INACTIVE_STATE:
				bCmdReg |= SD_RESPONSE_NONE;
				dArgReg = (dArgu&0xffff)<<16;
				break;
			case CMD16_SET_BLOCKLEN:	
				bCmdReg |= SD_RESPONSE_R1;
				bCmdReg |= SD_CMD_CRC_CHECK;
				bCmdReg |= SD_CMD_IDX_CHECK;
				break;
			case CMD17_READ_SINGLE_BLOCK:	
				bCmdReg |= SD_RESPONSE_R1;
				bCmdReg |= (SD_CMD_WITH_DATA_BLK|SD_DATA_READ);
				bCmdReg |= SD_CMD_CRC_CHECK;
				bCmdReg |= SD_CMD_IDX_CHECK;
				//Set STO block size to 512 bytes
				Vhost_WriteReg(VDEV_REG_SD_BLK_SIZE, (SD_BLOCK_SIZE-1)); 
				break;
			case CMD18_READ_MULTIPLE_BLOCK:	
				bCmdReg |= SD_RESPONSE_R1;
				bCmdReg |= (SD_CMD_WITH_DATA_BLK|SD_DATA_READ);
				bCmdReg |= SD_CMD_CRC_CHECK;
				bCmdReg |= SD_CMD_IDX_CHECK;
				//Set STO block size to 512 bytes
				Vhost_WriteReg(VDEV_REG_SD_BLK_SIZE, (SD_BLOCK_SIZE-1)); 
				g_RdAutoCmd12 = 0;
				break;
			case CMD24_WRITE_BLOCK:		
				bCmdReg |= SD_RESPONSE_R1;
				bCmdReg |= SD_CMD_WITH_DATA_BLK;
				bCmdReg |= SD_CMD_CRC_CHECK;
				bCmdReg |= SD_CMD_IDX_CHECK;
				//Set STO block size to 512 bytes
				Vhost_WriteReg(VDEV_REG_SD_BLK_SIZE, (SD_BLOCK_SIZE-1)); 
				//for single end prefech bug  
				Vhost_WriteReg(VDEV_REG_SD_RST, 0x08);
				break;
			case CMD25_WRITE_MULTIPLE_BLOCK:
				bCmdReg |= SD_RESPONSE_R1;
				bCmdReg |= SD_CMD_WITH_DATA_BLK;
				bCmdReg |= SD_CMD_CRC_CHECK;
				bCmdReg |= SD_CMD_IDX_CHECK;
				//Set STO block size to 512 bytes
				Vhost_WriteReg(VDEV_REG_SD_BLK_SIZE, (SD_BLOCK_SIZE-1)); 
				g_RdAutoCmd12 = 0;
				break; 
			case CMD27_PROGRAM_CSD:		
				bCmdReg |= SD_RESPONSE_R1;
				bCmdReg |= SD_CMD_WITH_DATA_BLK;
				bCmdReg |= SD_CMD_CRC_CHECK;
				bCmdReg |= SD_CMD_IDX_CHECK;
				//Set STO block size to 32 bytes
				Vhost_WriteReg(VDEV_REG_SD_BLK_SIZE, (32-1)); 
				break;
			case CMD28_SET_WRITE_PROT:	
				bCmdReg |= SD_RESPONSE_R1B;
				bCmdReg |= SD_CMD_CRC_CHECK;
				bCmdReg |= SD_CMD_IDX_CHECK;
				break;
			case CMD29_CLR_WRITE_PROT:	
				bCmdReg |= SD_RESPONSE_R1B;
				bCmdReg |= SD_CMD_CRC_CHECK;
				bCmdReg |= SD_CMD_IDX_CHECK;
				break;
			case CMD30_SEND_WRITE_PROT:	
				bCmdReg |= SD_RESPONSE_R1;
				bCmdReg |= (SD_CMD_WITH_DATA_BLK|SD_DATA_READ);
				bCmdReg |= SD_CMD_CRC_CHECK;
				bCmdReg |= SD_CMD_IDX_CHECK;
				//Set STO block size to 4 bytes
				Vhost_WriteReg(VDEV_REG_SD_BLK_SIZE, (4-1)); 
				break;
			case CMD32_ERASE_WR_BLK_START:	
				bCmdReg |= SD_RESPONSE_R1;
				bCmdReg |= SD_CMD_CRC_CHECK;
				bCmdReg |= SD_CMD_IDX_CHECK;
				break;
			case CMD33_ERASE_WR_BLK_END:	
				bCmdReg |= SD_RESPONSE_R1;
				bCmdReg |= SD_CMD_CRC_CHECK;
				bCmdReg |= SD_CMD_IDX_CHECK;
				break;
			case CMD38_ERASE:		
				bCmdReg |= SD_RESPONSE_R1B;
				bCmdReg |= SD_CMD_CRC_CHECK;
				bCmdReg |= SD_CMD_IDX_CHECK;
				break;
			case CMD42_LOCK_UNLOCK:		
				bCmdReg |= SD_RESPONSE_R1;
				bCmdReg |= SD_CMD_WITH_DATA_BLK;
				bCmdReg |= SD_CMD_CRC_CHECK;
				bCmdReg |= SD_CMD_IDX_CHECK;
				//Set STO block size to 512 bytes
				Vhost_WriteReg(VDEV_REG_SD_BLK_SIZE, (SD_BLOCK_SIZE-1)); 
				break;
			case CMD52_IORW_DIRECT:	
				bCmdReg |= SD_RESPONSE_R5;
				bCmdReg |= SD_CMD_CRC_CHECK;
				bCmdReg |= SD_CMD_IDX_CHECK;				
				break;
			case CMD53_IORW_EXT:	
				bCmdReg |= SD_RESPONSE_R5;
				bCmdReg |= SD_CMD_WITH_DATA_BLK;
				bCmdReg |= SD_CMD_CRC_CHECK;
				bCmdReg |= SD_CMD_IDX_CHECK;
				if (!( dArgReg & 0x80000000))
				{
					//Read
					bCmdReg |=SD_DATA_READ;
				}
				break;				
			case CMD55_APP_CMD:		
				bCmdReg |= SD_RESPONSE_R1;
				dArgReg = (dArgu&0xffff)<<16;
				bCmdReg |= SD_CMD_CRC_CHECK;
				bCmdReg |= SD_CMD_IDX_CHECK;
				break;
			case CMD56_GEN_CMD:
				bCmdReg |= SD_RESPONSE_R1;
				bCmdReg |= (SD_CMD_WITH_DATA_BLK|SD_DATA_READ);
				bCmdReg |= SD_CMD_CRC_CHECK;
				bCmdReg |= SD_CMD_IDX_CHECK;
				//bRspID = SD_RESPONSE_R1;
				dArgReg = dArgu&0x01;
				//Set STO block size to 512 bytes
				Vhost_WriteReg(VDEV_REG_SD_BLK_SIZE, (SD_BLOCK_SIZE-1)); 
				break;			
			default:
				return VSD_ERROR;;
		}

		//Reset STO
		//Vhost_WriteReg(VDEV_REG_SD_RST, SD_CTRL_RESET);

		//Clear interrupt flag
		//Vhost_WriteReg(VDEV_REG_SD_NORMAL_INT_FLAG, 0xff);

		Vhost_WriteReg(VDEV_REG_SD_CMD_ARGU, dArgReg); //Write argument
		dArgReg = Vhost_ReadReg(VDEV_REG_SD_CMD_ARGU); //Write argument
		
		Vhost_WriteReg(VDEV_REG_SD_CMDREG1, bCmdReg); 
		switch(bCmdID)
		{
			case CMD18_READ_MULTIPLE_BLOCK:	
			case CMD25_WRITE_MULTIPLE_BLOCK:
				//AutoCmd 12
				Vsd_SendAutoCmd(CMD12_STOP_TRANSMISSION, 0xffffffff);
				break;
		}
		Vhost_WriteReg(VDEV_REG_SD_CMD, bCmdID); //Start do command
		//Cmd Sent Out		
		if(bCmdReg & SD_CMD_WITH_DATA_BLK)
		{
			if (bCmdReg & SD_DATA_READ)
					pDev->sd_state[socketid]  = SD_State_Cmd_RdWithData;
			else
				pDev->sd_state[socketid]  = SD_State_Cmd_WrWithData;
		}
		else
		{
				pDev->sd_state[socketid]  = SD_State_Cmd;
		}
				
		bResult = Vsd_WaitCmdCompleted();

		if(bResult&SD_RES_CMD_FINISH)//success
		{
			
			return bResult;
		}
		
		bRepeat++;
	}
	
	return bResult; //Error
}

/********************************************************************
 Description:
		Send Auto Cmd after a cmd.

 Parameters:
		bCmdID:	Command id.
		dArgu:	Argument.
		
 Returns:
		Reponse.		

********************************************************************/

SDSTATE Vsd_SendAutoCmd(MMD_U08 bCmdID, MMD_U32 dArgu)
{
	MMD_U32 dCmdReg = 0, dCmdDirection;
	
	dCmdReg = bCmdID;
	dCmdReg <<= 8;
	
	dCmdDirection = Vhost_ReadReg(VDEV_REG_SD_CMDREG1);
	dCmdDirection &= SD_DATA_READ;

	switch(bCmdID)
	{
		case CMD12_STOP_TRANSMISSION:	
			dCmdReg |=  SD_RESPONSE_R1B|SD_CMD_CRC_CHECK|SD_CMD_IDX_CHECK
						|dCmdDirection|0x8000;			
			break;		
		default:
			dCmdReg = 0;
			break;
	}
	
	Vhost_WriteReg(VDEV_REG_SD_AUTOCMD, dCmdReg);
	Vhost_WriteReg(VDEV_REG_SD_AUTOARG, dArgu);
	return VSD_SUCCESS;
}

/********************************************************************
 Description:
		Waiting the command

 Parameters:
		Void.
		
 Returns:
		Reponse id.		

********************************************************************/
SDSTATE Vsd_WaitCmdCompleted(void)
{
	//MMD_U08 bRegVal;
	MMD_U32 repeat;
	PSD_DEV pDev;
	SD_STATE_MODE stat;
	MMD_U08 socketid ;
	
	socketid = g_SdDev.current_socket;
	socketid &= 0x1;
	if(g_SdDev.sd_power[socketid]  == SD_Power_Work)
	{
		pDev = &g_SdDev;
	}
	else
		return VSD_WORKMODE_ERR;

	stat = pDev->sd_state[socketid] ;
	repeat = 0;
	
#if (SD_INTERRUPT == 1)
	// enable_interrupts();   
	SD_EnableInt();
#endif

	do
	{
#if (STRESS_TEST == 1)
		memcpy((void *)0x20052200, (void *)0x20052200, 512);
#endif

#if (SD_INTERRUPT == 0)		
		Vsd_ErrorHandle();		
		Vsd_IntHandle();
#else
		//call Vsd_ErrorHandle by int
#endif
		repeat ++;
		if (repeat > SD_CMDOK_WAIT_COUNT)
		{
			SD_DisableInt();			
			return VSD_CMDOK_TIMEOUT;
		}
		if( pDev->sd_state[socketid]  == SD_State_Cmd_Err)
		{
			SD_DisableInt();				
			return VSD_RESPONSE_TIMEOUT;
		}
	}while(pDev->sd_state[socketid] == stat);//command finished
	
#if (SD_SHOWTIME == 1)
	SDMSG(("WaitCmd %d\r\n",repeat));
#endif

 	SD_DisableInt();
	
	if(SD_State_Cmd_Err != pDev->sd_state[socketid] )
		return SD_RES_CMD_FINISH;
	else
		return VSD_RESPONSE_TIMEOUT;
}

/********************************************************************
 Description:
		Send application command to STO.

 Parameters:
		bCmdID:	Command id.
		dArgu:	Argument.
		
 Returns:
		Reponse VSD_SUCCESS if success.		

********************************************************************/
SDSTATE Vsd_SendAppCommand(MMD_U08 bCmdID, MMD_U32 dArgu)
{
	MMD_U08 bCmdReg, bRepeat, bResult;
	MMD_U32 dArgReg;
	PSD_DEV pDev;
	MMD_U08 socketid ;
	
	socketid=g_SdDev.current_socket;
	socketid&=0x1;
	if(g_SdDev.sd_power[socketid]  == SD_Power_Work)
	{
		pDev = &g_SdDev;
	}
	else
		return VSD_WORKMODE_ERR;

	bRepeat = 0;
	while(bRepeat<SD_SEND_CMD_REPEAT_COUNT)
	{
		dArgReg = dArgu;
		bCmdReg = 0; 
		//bCmdReg = (SD_CMD_IDX_CHECK|SD_CMD_CRC_CHECK);
			
		switch(bCmdID)
		{
			case ACMD6_SET_BUS_WIDTH:
				bCmdReg |= SD_RESPONSE_R1;
				dArgReg = (dArgu&0x03);
				break;
			case ACMD13_SD_STATUS:	
				bCmdReg |= SD_RESPONSE_R1;
				bCmdReg |= (SD_CMD_WITH_DATA_BLK|SD_DATA_READ);
				bCmdReg |= SD_CMD_CRC_CHECK;
				bCmdReg |= SD_CMD_IDX_CHECK;
				//Set STO block size to 64 bytes
				Vhost_WriteReg(VDEV_REG_SD_BLK_SIZE, (64-1));
				//Set blk cnt 1 
				Vhost_WriteReg(VDEV_REG_SD_BLK_COUNT, 0x01); 
				break;
			case ACMD22_SEND_NUM_WR_BLOCKS:
				bCmdReg |= SD_RESPONSE_R1;
				bCmdReg |= (SD_CMD_WITH_DATA_BLK|SD_DATA_READ);
				bCmdReg |= SD_CMD_CRC_CHECK;
				bCmdReg |= SD_CMD_IDX_CHECK;
				//Set STO block size to 4 bytes
				Vhost_WriteReg(VDEV_REG_SD_BLK_SIZE, (4-1));
				//Set blk cnt 1  
				Vhost_WriteReg(VDEV_REG_SD_BLK_COUNT, 0x01); 
				break;
			case ACMD23_SET_WR_BLK_ERASE_COUNT:		
				bCmdReg |= SD_RESPONSE_R1;
				dArgReg = (dArgu&0x3FFFFF);
				break;
			case ACMD41_SD_SEND_OP_COND:	
				bCmdReg |= SD_RESPONSE_R3;
				break;
			case ACMD42_SET_CLR_CARD_DETECT:		
				bCmdReg |= SD_RESPONSE_R1;
				dArgReg = (dArgu&0x01);
				break;
			case ACMD51_SEND_SCR:		
				bCmdReg |= SD_RESPONSE_R1;
				bCmdReg |= (SD_CMD_WITH_DATA_BLK|SD_DATA_READ);
				bCmdReg |= SD_CMD_CRC_CHECK;
				bCmdReg |= SD_CMD_IDX_CHECK;
				//Set STO block size to 8 bytes
				Vhost_WriteReg(VDEV_REG_SD_BLK_SIZE, (8-1)); 
				//Set blk cnt 1 
				Vhost_WriteReg(VDEV_REG_SD_BLK_COUNT, 0x01); 
				break;
			default:
				return VSD_ERROR;;
		}

	
		//Reset STO
		//Vhost_WriteReg(VDEV_REG_SD_RST, SD_CTRL_RESET);
		
		//Clear interrupt flag
		//Vhost_WriteReg(VDEV_REG_SD_NORMAL_INT_FLAG, 0xff);

		Vhost_WriteReg(VDEV_REG_SD_CMD_ARGU, dArgReg); //Write argument
		dArgReg = Vhost_ReadReg(VDEV_REG_SD_CMD_ARGU); //Write argument
		


		Vhost_WriteReg(VDEV_REG_SD_CMDREG1, bCmdReg);
		Vhost_WriteReg(VDEV_REG_SD_CMD, bCmdID); //Start do command

		//Cmd Sent Out
		
		if( bCmdReg & SD_CMD_WITH_DATA_BLK)
		{
			if (bCmdReg & SD_DATA_READ)
				pDev->sd_state[socketid]  = SD_State_Cmd_RdWithData;
			else
				pDev->sd_state[socketid]  = SD_State_Cmd_WrWithData;
		}
		else
		{
			pDev->sd_state[socketid]  = SD_State_Cmd;
		}
		
		
		bResult = Vsd_WaitCmdCompleted();

		if(bResult&SD_RES_CMD_FINISH)//success
		{
			return bResult;
		}

		bRepeat++;
	}
	
	return bResult; //Error
	
}

/********************************************************************
 Description:
		Get the card's capacity. Unit:block.

 Parameters:
		pbCSD: Pointer to the CSD data.
		
 Returns:
		Block number.		

********************************************************************/
MMD_U32 Vsd_GetCapacity(void)
{
	SDIO_CARD_INFORMATION info;
	Vsd_GetCardInfo(&info);
	
	return info.capacity;
}

/********************************************************************
 Description:
		Get the size of an erasable sector.

 Parameters:		
		
 Returns:
		The unit size in block of data to be erased.
		
********************************************************************/
MMD_U32 Vsd_GetMinErasableSize(void)
{
	SDIO_CARD_INFORMATION info;
	Vsd_GetCardInfo(&info);

	if (info.EraseBlkEn)
		return 1;
	else
		return (info.Eraselen+1);
}


/********************************************************************
 Description:
		Get the card's info

 Parameters:
		pbCSD: Pointer to the CSD data.
		
 Returns:
		Block number.		

********************************************************************/
void Vsd_GetCardInfo(PSDIO_CARD_INFORMATION info)
{
	MMD_U32 csd_structure,    blocknr;
	MMD_U08 i;
	MMD_U08 socketid ;
	
	socketid = g_SdDev.current_socket;
	socketid &= 0x1;
	if(g_SdDev.sd_power[socketid] != SD_Power_Work)
		return;
	//[126:127]
	if(g_SdDev.MemType[socketid] == TYPE_MMC)
		csd_structure = 0;
	else		
		csd_structure = (MMD_U32)(g_SdDev.bCSD[socketid] [15]>>6);
	info->version = g_SdDev.version[socketid] ;
	info->capacitymode = g_SdDev.capacitymode[socketid] ;
	info->speedmode = g_SdDev.speedmode[socketid] ;
	if(csd_structure == 0)
	{//CSD 1.0
		//min read[61:59] 
		info->minRead = (MMD_U08)(g_SdDev.bCSD[socketid] [7]>>3)&0x7;
		//max read[58:56]
		info->maxRead = (MMD_U08)(g_SdDev.bCSD[socketid] [7])&0x7;
		//	minWrite[55:53]
		info->minWrite = (MMD_U08)(g_SdDev.bCSD[socketid] [6]>>5)&0x7;
		//maxWrite [52:50]
		info->maxWrite = (MMD_U08)(g_SdDev.bCSD[socketid] [6]>>2)&0x7; 
		//read block len[83:80]
		info->ReadBlkLen = 	(MMD_U08)(g_SdDev.bCSD[socketid] [10])&0xf;
		//EraseBlkEn [46]
		info->EraseBlkEn = (MMD_U08)(g_SdDev.bCSD[socketid][5]>>6)&0x1;
		//Eraselen [45:39]
		info->Eraselen = ((MMD_U08)(g_SdDev.bCSD[socketid][5]<<1)&0x7e)+(MMD_U08)(g_SdDev.bCSD[socketid][4]>>7)&0x1; 
		//info->Eraselen = ((MMD_U08)(g_SdDev.bCSD[socketid] [5]<<1)&0x6e)+(MMD_U08)(g_SdDev.bCSD[socketid] [4]>>7)&0x1; 
		//speed [103:96]
		info->speed = (MMD_U08)(g_SdDev.bCSD[socketid] [12])&0xff;
	//	SDMSG(("csd ver %x current min %x max %x\n",csd_structure,minRead,maxRead));
		
		//c_size [73:62]
		info->c_size = (MMD_U32)(g_SdDev.bCSD[socketid] [7]>>6)
				+((MMD_U32)(g_SdDev.bCSD[socketid] [8])<<2)
				+((MMD_U32)(g_SdDev.bCSD[socketid] [9]&0x03)<<10);
		
		//c_size_mult[49:47]		
		info->c_size_mult = (MMD_U32)(g_SdDev.bCSD[socketid] [5]>>7)
					 +((MMD_U32)(g_SdDev.bCSD[socketid] [6]&0x03)<<1);
		blocknr = (info->c_size+1)*((MMD_U32)1<<(info->c_size_mult+2));
		
		info->capacity = (blocknr*((MMD_U32)1<<info->ReadBlkLen))>>9;	
	}
	else if(csd_structure == 1)
	{//CSD 2.0
#if (SD_HICAPACITY == 1)
		//csize [69:48]
		info->minRead = 0xff;//not support
		info->maxRead = 0xff;//not support
		info->minWrite = 0xff;//not support
		info->maxWrite = 0xff;//not support
		info->c_size_mult=0xff;//not support
		//read block len[83:80]
		info->ReadBlkLen = 	(MMD_U08)(g_SdDev.bCSD[socketid] [10])&0xf; 
		//EraseBlkEn [46]
		info->EraseBlkEn = (MMD_U08)(g_SdDev.bCSD[socketid][5]>>6)&0x1;
		//Eraselen [45:39]
		info->Eraselen = ((MMD_U08)(g_SdDev.bCSD[socketid] [5]<<1)&0x7e)+(MMD_U08)(g_SdDev.bCSD[socketid] [4]>>7)&0x1; 
		//info->Eraselen = ((MMD_U08)(g_SdDev.bCSD[socketid] [5]<<1)&0x6e)+(MMD_U08)(g_SdDev.bCSD[socketid] [4]>>7)&0x1; 
		//speed [103:96]
		info->speed = (MMD_U08)(g_SdDev.bCSD[socketid] [12])&0xff;
		//c size[69:48]
		info->c_size=(MMD_U32)(g_SdDev.bCSD[socketid] [8]&0x3f);
		info->c_size <<=8;
		info->c_size+=	(MMD_U32)(g_SdDev.bCSD[socketid] [7]);	
		info->c_size <<=8; 
		info->c_size+=	(MMD_U32)(g_SdDev.bCSD[socketid] [6]);
		if(g_SdDev.MemType[socketid] == TYPE_SD)
			info->capacity = (info->c_size+1)*512*2;
		else if(g_SdDev.MemType[socketid] == TYPE_MMC)
			info->capacity = g_SdDev.capacity[socketid];
#endif	
	}	
	else
	{
		//ASSERT("not support csd version\n");
		ASSERT(0xFFFF); //to avoid warning by lizg
	}
	//MID [127:120]
	info->MID = (MMD_U08)(g_SdDev.bCID[socketid] [15])&0xff;
	//OID [119:104]
	info->OID = ((MMD_U16)g_SdDev.bCID[socketid] [14]<<8)&0xff00 + 
				((MMD_U16)g_SdDev.bCID[socketid] [13] &0xff);
	//PNM [103:64]	
	for(i=0;i<5;i++)
	{
		info->PNM[i] = (MMD_U08)(g_SdDev.bCID[socketid] [i+8])&0xff; 		
	}
	//PRV [63:56]
	info->PRV = (MMD_U08)(g_SdDev.bCID[socketid] [7])&0xff; 	
	//PSN [55:24]
	
	for(i=6,info->PSN=0;i>=3;i--)
	{
		info->PSN<<=8;
		info->PSN +=g_SdDev.bCID[socketid] [i];
		
	}
	//	MDT[19:8]
	info->MDT = (MMD_U16)g_SdDev.bCID[socketid] [2]&0xf;
	info->MDT <<= 8;
	info->MDT += (MMD_U16)g_SdDev.bCID[socketid] [1];
	
	return ;
}

/********************************************************************
 Description:
		Send command response from to STO.

 Parameters:
		bRspID:	Response id.
		pRsp:	Pointer to the response.
		
 Returns:
		Void.		

********************************************************************/
void Vsd_GetResponse(MMD_U08 bCmdID, PSD_RESPONSE pRsp)
{

	switch(bCmdID)
	{
		case CMD2_ALL_SEND_CID: 
		case CMD9_SEND_CSD:
		case CMD10_SEND_CID:
			pRsp->bRsp[0] = Vhost_ReadReg(VDEV_REG_SD_RESPONSE+0);
			
			pRsp->bRsp[1] = Vhost_ReadReg(VDEV_REG_SD_RESPONSE+4);
			
			pRsp->bRsp[2] = Vhost_ReadReg(VDEV_REG_SD_RESPONSE+8);
			
			pRsp->bRsp[3] = Vhost_ReadReg(VDEV_REG_SD_RESPONSE+12);
			#if (RESPONSEBUG ==1)
			pRsp->bRsp[0] <<=8;
			#endif
			break;

		case CMD3_SEND_RELATIVE_ADDR:
			
			pRsp->bRsp[0] = Vhost_ReadReg(VDEV_REG_SD_RESPONSE+12);
			pRsp->bRsp[0] >>=16;
			break;
		case CMD5_SET_OP_COND:
		case CMD52_IORW_DIRECT:
		case CMD8_SEND_IF_COND:
		case CMD6_SWITCH_FUNC:
			pRsp->bRsp[0] = Vhost_ReadReg(VDEV_REG_SD_RESPONSE+12);
			;
			break;
		default:
			pRsp->bRsp[0] = Vhost_ReadReg(VDEV_REG_SD_RESPONSE+12);
			
			break;		
	}
	/*
	 Vmd_LSwapAB(pRsp->bRsp);
	 Vmd_LSwapAB(pRsp->bRsp+1);
	 Vmd_LSwapAB(pRsp->bRsp+2);
	 Vmd_LSwapAB(pRsp->bRsp+3);
	 */
}

/********************************************************************
 Description:
		Verify command response .

 Parameters:
		 bCmdID: cmd id
		pRsp:	Pointer to the response.
		
 Returns:
		SDSTATE.		

********************************************************************/
SDSTATE Vsd_VerifyResponse(MMD_U08 bCmdID, PSD_RESPONSE pRsp)
{
	SDSTATE ret;
	ret = VSD_SUCCESS;
	switch(bCmdID)
	{
		case CMD7_SELECT_CARD:
			if( pRsp->sdio_stat.crc_err | (pRsp->sdio_stat.cur_stat!=0xf) |
			pRsp->sdio_stat.general_err | pRsp->sdio_stat.illegal_cmd |
			pRsp->sdio_stat.out_of_range_err)
			{
				ret = VSD_ERROR;
			}
			break;
		case CMD6_MMC_SWITCH:
			if(pRsp->mmc_stat.switch_err)
			{
				ret = VSD_ERROR;
			}
			break;
		default:
			break;
	}
	return ret;
}


/********************************************************************
 Description:
		change mmc extcsd

 Parameters:
		id:  group number
 		value: function number
		
 Returns:
		VSD_SUCCESS		

********************************************************************/
SDSTATE Vsd_SwitchCSD(MMD_U08 id, MMD_U08 value)
{	
	MMCARG arg;
	SD_RESPONSE res;
	SDSTATE ret;
	
	arg.arg.access = 0x3;
	arg.arg.index = id;
	arg.arg.zero = 0;
	arg.arg.cmdset = 0;
	arg.arg.zero = 0;
	arg.arg.value = value;
	ret = Vsd_SendCommand((MMD_U08)CMD6_MMC_SWITCH, (MMD_U32)arg.reg);
	if(ret <= VSD_SUCCESS) 
		return ret;
	Vsd_GetResponse(CMD6_MMC_SWITCH, &res);
	ret = Vsd_VerifyResponse(CMD6_MMC_SWITCH, &res);
	//switch error R1b bit7 ==1
	return ret;
}

/********************************************************************
 Description:
		Write data to STO.

 Parameters:
 		dAddr:	Address write to.
		pData:	Pointer to the data.
		dLen:	Data size.
		
 Returns:
		The number of byte real write to STO.		

********************************************************************/
#if 0
MMD_U32 Vsd_WriteData(MMD_U32 dAddr, MMD_U08 *pData, MMD_U32 dLen)
{
	MMD_U32 dHdOffset, dEdOffset, dNum, i, dReal;

	dHdOffset = (dAddr%SD_BLOCK_SIZE); //dAddr%512
	dAddr &=~(SD_BLOCK_SIZE-1);
	dReal = 0;

	if(dHdOffset+dLen<SD_BLOCK_SIZE)
	{
		dReal = Vsd_ReadOneBlock(dAddr,  g_bBuffer);
		if(dReal<SD_BLOCK_SIZE)
			return VSD_ERROR;;

		for(i=0; i<dLen;i++)
		{
			g_bBuffer[dHdOffset+i] = *pData++;
		}

		Vsd_WriteOneBlock(dAddr,  g_bBuffer);
		return dLen;
	}

	// dHdOffset+dLen>=SD_BLOCK_SIZE
	if(dHdOffset)
	{
		dReal = Vsd_ReadOneBlock(dAddr,  g_bBuffer);
		if(dReal<SD_BLOCK_SIZE)
			return VSD_ERROR;;

		dNum = SD_BLOCK_SIZE - dHdOffset;
		
		for(i=0; i<dNum;i++)
		{
			g_bBuffer[dHdOffset+i] = *pData++;
		}

		Vsd_WriteOneBlock(dAddr,  g_bBuffer);

		dAddr += SD_BLOCK_SIZE;
		
		dNum = (dLen + dHdOffset)/SD_BLOCK_SIZE - 1;
		dReal = SD_BLOCK_SIZE - dHdOffset;
	}
	else
	{
		dNum = (dLen + dHdOffset)/SD_BLOCK_SIZE;
		dReal = 0;
	}
	
	dEdOffset = (dLen + dHdOffset)%SD_BLOCK_SIZE;

	for(i=0; i<dNum; i++)
	{
		dReal += Vsd_WriteOneBlock(dAddr,  pData);
		pData += SD_BLOCK_SIZE;
		dAddr += SD_BLOCK_SIZE;
	}

	if(dEdOffset)
	{
		//End offset
		 Vsd_ReadOneBlock(dAddr,  g_bBuffer);
		 for(i=0; i<dEdOffset;i++)
		{
			 g_bBuffer[i] = *pData++;
		}
		 dReal +=dEdOffset;
		Vsd_WriteOneBlock(dAddr,  g_bBuffer);
	}
	 return dReal;
	
}
#endif

/********************************************************************
 Description:
		Write one block to STO, the block size is 512 bytes.

 Parameters:
 		dBlock: 			Block address to write to.
		dSRAMAddr: 		Start address of VC0848 SRAM that stored the data to write.
		
 Returns:
		Success or Error.

********************************************************************/
SDSTATE Vsd_WriteOneBlock(MMD_U32 dBlock, MMD_U32 dSRAMAddr)
{
	MMD_U32 dAddr, regVal;
	SDSTATE Result;
	SD_RESPONSE Response;
	MMD_U08 socketid;
	socketid = g_SdDev.current_socket;
	socketid &= 0x1;
#if (SD_HICAPACITY == 1)
	dAddr=dBlock;	
	if(g_SdDev.capacitymode[socketid]<1)
#endif
	{
		dAddr = dBlock*512;
	}
	//set block num
	g_BlockCnt  = 1;
	Vhost_WriteReg(VDEV_REG_SD_BLK_COUNT, g_BlockCnt);

	regVal = Vhost_ReadReg(VDEV_REG_SD_RST);
	regVal |= 0x08;
	Vhost_WriteReg(VDEV_REG_SD_RST, regVal);
	
	Result = Vsd_SendCommand(CMD24_WRITE_BLOCK, dAddr); //Send write command	
		
	if (Result != SD_RES_CMD_FINISH)
	{
		//Error
		// reset state machine & buffer
		ASSERT(0xabc4);
		Vhost_WriteReg(VDEV_REG_SD_RST, 0x0c);
		Vhost_WriteReg(VDEV_REG_SD_INT_FLAG, 0xffffffff);
		return VSD_ERROR;
	}

	Vsd_GetResponse(CMD24_WRITE_BLOCK, &Response);

	Vsd_StartTrans(dSRAMAddr, SD_BLOCK_SIZE);
	
	Result = Vsd_WaitTransCompleted(); //Wait buffer is empty!
	
	if(Result!=SD_RES_DAT_FINISH) 
		return VSD_ERROR;
	
	Result = Vsd_WaitControlerIdle();
	if(Result!=VSD_SUCCESS) 
		return VSD_ERROR;
	
	//dNum = Vhost_ReadReg(VDEV_REG_SD_DMA_PROCESS); 	
	//dmalen=Vhost_ReadReg(VDEV_REG_SD_MODE); 
	//dmalen&=0xf0;
	//dmalen>>=4;
	//dmalen=1<<dmalen;
	
	//return dNum*4*dmalen;
	return VSD_SUCCESS;
}

/********************************************************************
 Description:
		Write Muti block to STO, until User call Stop 

 Parameters:
 		blk:	blk Address write to. (Sector )
		wBlockCnt:	Sector Num , 0 & 0xffffff: unknown lenth  
		
 Returns:
		Success or Error	
Note:   Muti Write Length No more than 0xffff * SectorSize

********************************************************************/
SDSTATE Vsd_WriteMultiBlock(MMD_U32 blk, MMD_U16 wBlockCnt)
{
	MMD_U32 dAddr, regVal;
	SDSTATE Result;
	SD_RESPONSE Response;
	MMD_U08 socketid;
	socketid = g_SdDev.current_socket;
	socketid &= 0x1;
#if (SD_HICAPACITY == 1)	
	dAddr = blk;	
	if(g_SdDev.capacitymode[socketid] < 1)
#endif
	{
		dAddr = blk*512;
	}
	
	//set block num
	if( wBlockCnt == 0 )
		g_BlockCnt = 0xffff;
	else
		g_BlockCnt = wBlockCnt;
		
	Vhost_WriteReg(VDEV_REG_SD_BLK_COUNT, g_BlockCnt);
	
	regVal = Vhost_ReadReg(VDEV_REG_SD_RST);
	regVal |= 0x08;
	Vhost_WriteReg(VDEV_REG_SD_RST, regVal);
	
	Result = Vsd_SendCommand(CMD25_WRITE_MULTIPLE_BLOCK, dAddr); //Send write command	
		
	if (Result != SD_RES_CMD_FINISH)
	{
		//Error
		// reset state machine & buffer
		ASSERT(0xabc1);
		Vhost_WriteReg(VDEV_REG_SD_RST, 0x0c);
		Vhost_WriteReg(VDEV_REG_SD_INT_FLAG, 0xffffffff);
		return VSD_ERROR;
	}

	Vsd_GetResponse(CMD25_WRITE_MULTIPLE_BLOCK, &Response);

	return VSD_SUCCESS;
}

/********************************************************************
 Description:
		Start DMA write data to card

 Parameters:
 		dAddr:	buffer addr
		wLen:	buffer byte len, sector aligned.
		
 Returns:
		bytes writen or Error =0
Note:   buffer should be 4 bytes aligned

********************************************************************/
MMD_U32 Vsd_WriteMultiData(MMD_U32 dAddr, MMD_U32 dLen)
{
	MMD_U16 curBlkCnt;
	MMD_U32 dNum,dmalen=1;
	SDSTATE Response;
	if(dLen == 0 || (dLen & 0x3) != 0)
		return VSD_ERROR;
	curBlkCnt = dLen/SD_BLOCK_SIZE;

	// Can't write data more than Vsd_WriteMultiBlock set
	if (g_BlockCnt < curBlkCnt)
		curBlkCnt = g_BlockCnt;
	
	Response = Vsd_StartTrans(dAddr , dLen);

	if(Response != VSD_SUCCESS) 
		return 0;
	Response = Vsd_WaitTransCompleted(); //Wait buffer is empty!
	if(Response != SD_RES_DAT_FINISH) 
		return 0;
	dmalen=Vhost_ReadReg(VDEV_REG_SD_MODE); 
	dmalen&=0xf0;
	dmalen>>=4;
	dmalen=1<<dmalen;
	g_BlockCnt -= curBlkCnt;
	dNum = Vhost_ReadReg(VDEV_REG_SD_DMA_PROCESS);
	
	return dNum*4*dmalen;
}

/********************************************************************
 Description:
		read multi & start dma 

 Parameters:
 		blk:	Block num
		pData:	buffer 32bit aligned
		blkcnt:	Block cnt.  (blkcnt<0xffff)
		
 Returns:
		Success 0 ; 
		Fail 	: Other value 
Note:   buffer should be 4 bytes aligned
We use this function to run Vsd_ReadMultBlock VsdRWMultiData VsdReadMultiStop sequencely. 
The limitation is you have to allocate a large block mem. We recommend call this if you have more than 2K buffer. 
Or you only have 512byte buffer, you also can use Vsd_ReadMultBlock & VsdRWMultiData to achieve higher performance


(N sector) Vsd_RdMultiAtOnce 
= Vsd_ReadMultBlock + N*(1 sector) VsdRWMultiData+ VsdReadMultiStop 
= Vsd_ReadMultBlock + (N sector) VsdRWMultiData+ VsdReadMultiStop


********************************************************************/
MMD_U32 Vsd_RdMultiAtOnce(MMD_U32 dBlock, MMD_U32 dSRAMAddr, MMD_U32 dCnt)
{
	SDSTATE ret;
	MMD_U32 num;
	
	//SDMSG(("Rd %x %x %x\n",blk,pData,blkcnt));
	ret = Vsd_ReadMultiBlock(dBlock, dCnt);
	if(ret <0)
		return 1;
	dCnt *= 512;
	num = Vsd_RWMultiData(dSRAMAddr, dCnt);
	if(num == 0)
		return 1;
	ret = Vsd_ReadMultiStop(dSRAMAddr, dCnt);
	if(ret < 0)
		return 1;
	
	return 0;
}

/********************************************************************
 Description:
		write multi & start dma 

 Parameters:
 		blk:	Block num
		pData:	buffer 32bit aligned
		blkcnt:	Block count.  (blkcnt<0xffff)
		
 Returns:
		Success 0
		fail 	:other value
Note:   buffer should be 4 bytes aligned

********************************************************************/
MMD_U32 Vsd_WrMultiAtOnce(MMD_U32 dBlock, MMD_U32 dSRAMAddr, MMD_U32 dBlockCnt)
{
	SDSTATE ret;
	MMD_U32 num;
	//SDMSG(("Wr %x %x %x\n",blk,pData,blkcnt));
	ret = Vsd_WriteMultiBlock(dBlock, dBlockCnt);
	if(ret < 0) {
		return 1;
	}
	dBlockCnt *= SD_BLOCK_SIZE;
	num = Vsd_RWMultiData(dSRAMAddr, dBlockCnt);
	if(num == 0) {
		return 1;
	}
	ret = Vsd_WriteMutiStop(dSRAMAddr, dBlockCnt);
	if(ret < 0) {
		return 1;
	}
	
	return 0;
}

/********************************************************************
 Description:
		Erase sectors
		
 Parameters:
		dwStartSector:	
		dwEndSector:
		
 Returns:
		total erased sectors
Note:
		SD may have min erased block size, so we'll erase erased block size aligned.
		for example 64K aligned.

********************************************************************/
MMD_U32 Vsd_Erase(MMD_U32 dwStartSector, MMD_U32 dwEndSector)
{
	SDIO_CARD_INFORMATION info;
	MMD_U32 minErase, totalErase;
	SDSTATE ret;
	MMD_U08 socketid;
	socketid = g_SdDev.current_socket;
	socketid &= 0x1;
	Vsd_GetCardInfo(&info);	

	minErase = Vsd_GetMinErasableSize();
	if (minErase > 1) {
		dwStartSector = ((dwStartSector + minErase - 1) / minErase) * minErase;
		if (dwEndSector <= dwStartSector)
			return 0;
		
		totalErase = (dwEndSector - dwStartSector + 1) / minErase;
		if (!totalErase)
			return 0;

		dwEndSector = dwStartSector + (totalErase * minErase) - 1;
	} else {
		if (dwEndSector < dwStartSector)
			return 0;
		totalErase = (dwEndSector - dwStartSector + 1);		
	}



	if(!(g_SdDev.capacitymode[socketid]))
	{
		//normal
		dwStartSector *= 512;
		dwEndSector *= 512;
	}
	
	ret=Vsd_SendCommand(CMD32_ERASE_WR_BLK_START, dwStartSector);
	if(ret <0)
		return 0;
	
	ret=Vsd_SendCommand(CMD33_ERASE_WR_BLK_END, dwEndSector);
	if(ret <0)
		return 0;
	
	ret=Vsd_SendCommand(CMD38_ERASE, 0);
	if(ret <0)
		return 0;

	return totalErase;
}

#if 0
/********************************************************************
 Description:
		Start DMA Read data from card

 Parameters:
 		dAddr:	buffer addr
		wLen:	buffer byte len, sector aligned.
		
 Returns:
		bytes writen or Error =0
Note:   buffer should be sector aligned

********************************************************************/
SDSTATE Vsd_ReadMultiData(MMD_U32 dAddr, MMD_U32 dLen)
{
	MMD_U16 curBlkCnt;
	MMD_U32 dNum;
	SDSTATE Response;
	if( dLen == 0 || (dLen & 0x1ff) != 0)
		return VSD_ERROR;
	curBlkCnt = dLen/SD_BLOCK_SIZE;

	// Can't write data more than Vsd_WriteMultiBlock set
	if ( g_BlockCnt < curBlkCnt)
		curBlkCnt = g_BlockCnt;
	
	Response = Vsd_StartTrans(dAddr , dLen);
	if(Response!=VSD_SUCCESS) 
		return VSD_ERROR;
	Response = Vsd_WaitTransCompleted(); //Wait buffer is empty!
	if(Response!=SD_RES_DAT_FINISH) 
		return VSD_ERROR;
	
	g_BlockCnt -= curBlkCnt;
	dNum = Vhost_ReadReg(VDEV_REG_SD_DMA_PROCESS); 	
	
	
	return dNum*4;
}
#endif

/********************************************************************
 Description:
		Stop Muti Write

 Parameters:
 		dAddr:	buffer addr
		wLen:	buffer byte len, sector aligned.
		
		
 Returns:
	Success or Error
Note:
	Stop Muti Write need 1 sectors data. 
	Muti Write can't stop immediatelly.

********************************************************************/
SDSTATE Vsd_WriteMutiStop(MMD_U32 dAddr, MMD_U32 dLen)
{	
	SDSTATE Response;
	MMD_U32 ctrlstat, sdioconf, dNum, dCurBlk, dRepeat, dmalen = 1;
	PSD_DEV pDev;
	MMD_U08 socketid ;
	
	socketid = g_SdDev.current_socket;
	socketid &= 0x1;
	if(g_SdDev.sd_power[socketid] == SD_Power_Work)
	{
		pDev = &g_SdDev;
	}
	else
		return VSD_WORKMODE_ERR;
	dmalen = Vhost_ReadReg(VDEV_REG_SD_MODE); 
	dmalen &= 0xf0;
	dmalen >>= 4;
	dmalen = 1<<dmalen;
	if (g_BlockCnt == 0)
	{
		//Wait idle
	}
	else if(g_BlockCnt ==1)
	{
		if (dLen != 1* SD_BLOCK_SIZE)
			return VSD_ERROR;
		//WriteData 
		if(Vsd_WriteMultiData(dAddr, dLen) == 0)
		
		{
			return VSD_ERROR;
		}
	
		dNum = Vhost_ReadReg(VDEV_REG_SD_DMA_PROCESS); 	
		dCurBlk = dNum*4*dmalen/SD_BLOCK_SIZE;
		g_BlockCnt -=dCurBlk;
		if (g_BlockCnt != 0)
		{
			ASSERT(1);
			
		}
		//Wait idle
	}
	else 
	{
		//Stop after 1 sector Multi Write

		do
		{	// full
			ctrlstat = Vhost_ReadReg(VDEV_REG_SD_PRESENT_STATE);
		}while ( (ctrlstat & VDEV_SD_BUF_READ_STATE) == 1);
		
		//Fifo Empty
		//StartDMA
		if( Vsd_WriteMultiData(dAddr, 4)==0)		
		{
			return VSD_ERROR;
		}
		
		do
		{	// full
			ctrlstat = Vhost_ReadReg(VDEV_REG_SD_PRESENT_STATE);
		}while((ctrlstat & VDEV_SD_BUF_READ_STATE) == 1);
		
		//Fifo Empty
		//disable next block
		sdioconf = Vhost_ReadReg(VDEV_REG_SDIO_CONFIG);
		sdioconf |= SDIO_DATA_START_DISABLE;
		Vhost_WriteReg(VDEV_REG_SDIO_CONFIG, sdioconf);
		//StartDMA
		dAddr+=4;
		dLen-=4;
		if( Vsd_WriteMultiData(dAddr, dLen)==0)		
		{
			return VSD_ERROR;
		}
		
		Vsd_SendCommand(CMD12_STOP_TRANSMISSION, 0xffffffff);
		
		//enable next block
		sdioconf = Vhost_ReadReg(VDEV_REG_SDIO_CONFIG);
		sdioconf &= (~SDIO_DATA_START_DISABLE);
		Vhost_WriteReg(VDEV_REG_SDIO_CONFIG, sdioconf);		

		dNum = Vhost_ReadReg(VDEV_REG_SD_DMA_PROCESS); 	
		dCurBlk = dNum*4*dmalen/SD_BLOCK_SIZE;
		g_BlockCnt -=dCurBlk;
	}	
	
	//Wait Auto Cmd12 send out
	dRepeat = 0;
	
#if (SD_INTERRUPT == 1)
	SD_EnableInt();   
#endif

	while (dRepeat<SD_WRCMD_WAIT_COUNT)
	{
#if (SD_INTERRUPT ==0)
		Vsd_IntHandle();
		Vsd_ErrorHandle();
#endif
		if (g_RdAutoCmd12) {
			break;
		}

		dRepeat++;
	}

#if (SD_SHOWTIME == 1)
	SDMSG(("Mul_W Auto12 %d\r\n",dRepeat));
#endif

#if (SD_INTERRUPT == 1)
	SD_DisableInt();   
#endif

	Response = Vsd_WaitControlerIdle();
	//Disable AUTO CMD12
	Vsd_SendAutoCmd(0x0, 0x0);
	pDev->sd_state[socketid] = SD_State_Idle;
	if (Response != VSD_SUCCESS) 
		return VSD_ERROR;
	return VSD_SUCCESS;
}

/********************************************************************
 Description:
		Read data from STO.

 Parameters:
 		dAddr:	Address read from.
		pData:	Pointer to the data.
		dLen:	Data size.
		
 Returns:
		The number of byte real read from STO.		

********************************************************************/
#if 0
MMD_U32 Vsd_ReadData(MMD_U32 dAddr, MMD_U08 *pData, MMD_U32 dLen)
{
	MMD_U32 dHdOffset, dEdOffset, dNum, i, dReal;

	dHdOffset = (dAddr%SD_BLOCK_SIZE); //dAddr%512
	dAddr &=~(SD_BLOCK_SIZE-1);
	dReal = 0;


	if(dHdOffset+dLen<SD_BLOCK_SIZE)
	{
		dReal = Vsd_ReadOneBlock(dAddr,  g_bBuffer);
		if(dReal<SD_BLOCK_SIZE)
			return VSD_ERROR;;

		for(i=0; i<dLen;i++)
		{
			*pData++ = g_bBuffer[dHdOffset+i];
		}

		return dLen;
	}

	//dHdOffset+dLen>=SD_BLOCK_SIZE
	
	if(dHdOffset)
	{
		dReal = Vsd_ReadOneBlock(dAddr,  g_bBuffer);
		if(dReal<SD_BLOCK_SIZE)
			return VSD_ERROR;;

		dNum = SD_BLOCK_SIZE - dHdOffset;
		
		for(i=0; i<dNum;i++)
		{
			*pData++ = g_bBuffer[dHdOffset+i];
		}
		dAddr += SD_BLOCK_SIZE;
		
		dNum = (dLen + dHdOffset)/SD_BLOCK_SIZE - 1;
		dReal = SD_BLOCK_SIZE - dHdOffset;
	}
	else
	{
		dNum = (dLen + dHdOffset)/SD_BLOCK_SIZE;
		dReal = 0;
	}
	
	dEdOffset = (dLen + dHdOffset)%SD_BLOCK_SIZE;

	for(i=0; i<dNum; i++)
	{
		dReal += Vsd_ReadOneBlock(dAddr,  pData);
		pData += SD_BLOCK_SIZE;
		dAddr += SD_BLOCK_SIZE;
	}

	if(dEdOffset)
	{
		 Vsd_ReadOneBlock(dAddr,  g_bBuffer);
		 for(i=0; i<dEdOffset;i++)
		{
			*pData++ = g_bBuffer[i];
		}
		 dReal +=dEdOffset;
	}

	 return dReal;
	
}
#endif

/********************************************************************
 Description:
		Read one block from STO, the block size is 512 bytes.

 Parameters:
 		dBlock: 			Block address to read from.
		dSRAMAddr: 		Start address of VC0848 SRAM to save the read data.
		
 Returns:
		Success 0 ; 
		Fail 	: Other value 
********************************************************************/
SDSTATE Vsd_ReadOneBlock(MMD_U32 dBlock, MMD_U32 dSRAMAddr)
{
	MMD_U32 dAddr, regVal;
	SDSTATE Result;
	SD_RESPONSE Response;
	MMD_U08 socketid;
	socketid = g_SdDev.current_socket;
	socketid &= 0x1;

#if (SD_HICAPACITY == 1)	
	dAddr = dBlock;	
	if(g_SdDev.capacitymode[socketid] < 1)
#endif
	{
		dAddr = dBlock*512;
	}
	
	//set block num
	g_BlockCnt = 1;
	Vhost_WriteReg(VDEV_REG_SD_BLK_COUNT, g_BlockCnt);

	regVal = Vhost_ReadReg(VDEV_REG_SD_RST);
	regVal |= 0x08;
	Vhost_WriteReg(VDEV_REG_SD_RST, regVal);
		
	Result = Vsd_SendCommand(CMD17_READ_SINGLE_BLOCK, dAddr);//Send read command
	
	if (Result != SD_RES_CMD_FINISH)
	{
		//Error
		// reset state machine & buffer
		ASSERT(0xabc3);
		Vhost_WriteReg(VDEV_REG_SD_RST, 0x0c);
		Vhost_WriteReg(VDEV_REG_SD_INT_FLAG, 0xffffffff);
		return VSD_ERROR;
	}

	Vsd_GetResponse(CMD17_READ_SINGLE_BLOCK, &Response);
	
	Vsd_StartTrans(dSRAMAddr, SD_BLOCK_SIZE);	
	
	Result = Vsd_WaitTransCompleted(); //Wait buffer is empty!
	
	if(Result!=SD_RES_DAT_FINISH) 
		return VSD_ERROR;
	Result = Vsd_WaitControlerIdle();
	if(Result!=VSD_SUCCESS) 
		return VSD_ERROR;
	//dNum = Vhost_ReadReg(VDEV_REG_SD_DMA_PROCESS); 	
	//dmalen = Vhost_ReadReg(VDEV_REG_SD_MODE); 
	//dmalen &= 0xf0;
	//dmalen >>= 4;
	//dmalen = 1<<dmalen;

	//return dNum*4*dmalen;
	return VSD_SUCCESS;
}


/********************************************************************
 Description:
		Read Multi block from STO, until User call Stop 

 Parameters:
 		dBlock:		Block address to write to.
		wBlockCnt:	Number of blocks to read, 0 & 0xffffff: unknown lenth  
		
 Returns:
		Success or Error
		
Note:   Muti Write Length No more than 0xffff * SectorSize

********************************************************************/
SDSTATE Vsd_ReadMultiBlock(MMD_U32 dBlock, MMD_U16 wBlockCnt)
{
	SDSTATE bResult;
	MMD_U32 dAddr, regVal;
	SD_RESPONSE Response;
	MMD_U08 socketid;
	socketid = g_SdDev.current_socket;
	socketid &= 0x1;
#if (SD_HICAPACITY == 1)	
	dAddr = dBlock;	
	if(g_SdDev.capacitymode[socketid] < 1)
#endif
	{
		dAddr = dBlock*512;
	}
	
	//set block num
	if(wBlockCnt == 0)
		g_BlockCnt = 0xffff;
	else
		g_BlockCnt = wBlockCnt;
		
	Vhost_WriteReg(VDEV_REG_SD_BLK_COUNT, g_BlockCnt);
	
	regVal = Vhost_ReadReg(VDEV_REG_SD_RST);
	regVal |= 0x08;
	Vhost_WriteReg(VDEV_REG_SD_RST, regVal);
	
	bResult = Vsd_SendCommand(CMD18_READ_MULTIPLE_BLOCK, dAddr); //Send Read command	
		
	if (bResult != SD_RES_CMD_FINISH)
	{
		//Error
		// reset state machine & buffer
		ASSERT(0xabc2);
		Vhost_WriteReg(VDEV_REG_SD_RST, 0x0c);
		Vhost_WriteReg(VDEV_REG_SD_INT_FLAG, 0xffffffff);
		return VSD_ERROR;
	}

	Vsd_GetResponse(CMD18_READ_MULTIPLE_BLOCK, &Response);

	return VSD_SUCCESS;
}


/********************************************************************
 Description:
		Stop Muti Read

 Parameters:
 		dAddr:	buffer addr
		wLen:	buffer byte len, sector aligned.
		
		
 Returns:
	Success or Error
Note:
	Stop Muti Write need 2 sectors data. 
	Muti Write can't stop immediatelly.

********************************************************************/
SDSTATE Vsd_ReadMultiStop(MMD_U32 dAddr, MMD_U32 dLen)
{	
	SDSTATE Response;
	MMD_U32 ctrlstat, sdioconf, dNum, dCurBlk,dRepeat, dmalen=1;
	PSD_DEV pDev;
	MMD_U08 socketid ;
	
	socketid = g_SdDev.current_socket;
	socketid &= 0x1;
	if(g_SdDev.sd_power[socketid]  == SD_Power_Work)
	{
		pDev = &g_SdDev;
	}
	else
		return VSD_WORKMODE_ERR;
	dmalen = Vhost_ReadReg(VDEV_REG_SD_MODE); 
	dmalen &= 0xf0;
	dmalen >>= 4;
	dmalen = 1<<dmalen;
	if (g_BlockCnt == 0)
	{
		//Wait idle
	}
	else if(g_BlockCnt == 2)
	{
		if(dLen != 2* SD_BLOCK_SIZE)
			return VSD_ERROR;
		//WriteData 
		if(Vsd_ReadMultiData(dAddr, dLen)==0)
		{
			return VSD_ERROR;
		}
	
		dNum = Vhost_ReadReg(VDEV_REG_SD_DMA_PROCESS); 	
		dCurBlk = dNum*4*dmalen/SD_BLOCK_SIZE;
		g_BlockCnt -= dCurBlk;
		if (g_BlockCnt != 0)
		{
			ASSERT(g_BlockCnt);			
		}
		//Wait idle
	}
	else 
	{
		//Stop after 2 sector Multi Read
		do
		{	//Not full
			ctrlstat = Vhost_ReadReg(VDEV_REG_SD_PRESENT_STATE);
		}
		while ((ctrlstat & VDEV_SD_BUF_WRITE_STATE) == 1);
		//Fifo full
		//disable next block
		sdioconf = Vhost_ReadReg(VDEV_REG_SDIO_CONFIG);
		sdioconf |= SDIO_DATA_START_DISABLE;
		Vhost_WriteReg(VDEV_REG_SDIO_CONFIG, sdioconf);
		
		//StartDMA
		if( Vsd_ReadMultiData(dAddr, dLen)==0)

		{
			return VSD_ERROR;
		}
		Vsd_SendCommand(CMD12_STOP_TRANSMISSION, 0xffffffff);
		
		//enable next block
		sdioconf = Vhost_ReadReg(VDEV_REG_SDIO_CONFIG);
		sdioconf &= (~SDIO_DATA_START_DISABLE);
		Vhost_WriteReg(VDEV_REG_SDIO_CONFIG, sdioconf);
		
		
		dNum = Vhost_ReadReg(VDEV_REG_SD_DMA_PROCESS); 	
		dCurBlk = dNum*4*dmalen/SD_BLOCK_SIZE;
		g_BlockCnt -= dCurBlk;
	}
	
	//Wait Auto Cmd12 send out
	dRepeat = 0;
	while (dRepeat<SD_WRCMD_WAIT_COUNT)
	{
#if (SD_INTERRUPT ==0)
		Vsd_IntHandle();
		Vsd_ErrorHandle();
#endif
		if( g_RdAutoCmd12)
		{
			break;
		}
		dRepeat++;		
	}
#if (SD_SHOWTIME == 1)
	SDMSG(("Mul_R Auto12 %d\r\n",dRepeat));
#endif
	Response = Vsd_WaitControlerIdle();
	//Disable AUTO CMD12
	Vsd_SendAutoCmd(0x0, 0x0);
	
	if(Response!=VSD_SUCCESS) 
		return VSD_ERROR;
	pDev->sd_state[socketid] = SD_State_Idle;
	return Response;
}
#if 0
/********************************************************************
 Description:
		Waiting buffer is filled with data.

 Parameters:
		Void.
		
 Returns:
		Reponse id.		

********************************************************************/
MMD_U08 Vsd_WaitReadCompleted(void)
{
}

/********************************************************************
 Description:
		Waiting buffer is filled with data.

 Parameters:
		Void.
		
 Returns:
		Reponse id.		

********************************************************************/
MMD_U08 Vsd_WaitWriteCompleted(void)
{
}
#endif
/********************************************************************
 Description:
		Wait controler go to idle.

 Parameters:
		Void.
		
 Returns:
			

********************************************************************/
SDSTATE Vsd_WaitControlerIdle(void)
{
	MMD_U32 dRepeat, ctrlstat;
	PSD_DEV pDev;
	SD_STATE_MODE stat;
	MMD_U08 socketid ;
	
	socketid = g_SdDev.current_socket;
	socketid &= 0x1;
	if(g_SdDev.sd_power[socketid] == SD_Power_Work)
	{
		pDev = &g_SdDev;
	}
	else
		return VSD_WORKMODE_ERR;

	dRepeat = 0;
	stat = pDev->sd_state[socketid] ;
#if (SD_INTERRUPT == 1)
	 //enable_interrupts();   
	SD_EnableInt();   
#endif
	//Wait Controler Go to idle
	while (dRepeat<SD_IDLE_WAIT_COUNT)
	{
#if (STRESS_TEST == 1)
		memcpy((void *)0x20052200, (void *)0x20052200, 512);
#endif

#if (SD_INTERRUPT ==0)
		Vsd_IntHandle();
		Vsd_ErrorHandle();
#else
		// Int
#endif
		if (stat !=  pDev->sd_state[socketid]  )
			break;
		ctrlstat = Vhost_ReadReg(VDEV_REG_SD_PRESENT_STATE);

		if ((ctrlstat & VDEV_SD_CTRL_CURRENT_STATE) == VDEV_SD_CTRL_IDLE)
		{
#if (SD_SHOWTIME == 1)
			SDMSG(("Wait Idle %d\r\n",dRepeat));
#endif
			SD_DisableInt();   
			//for single end prefech bug
			// Vhost_WriteReg(VDEV_REG_SD_RST, 0x08);
			return VSD_SUCCESS;
		}
		
		dRepeat++;
	}
	
#if (SD_SHOWTIME == 1)
	SDMSG(("Wait Idle %d\r\n",dRepeat));
#endif
	
	SD_DisableInt(); 
	if(dRepeat>=SD_WRCMD_WAIT_COUNT)
	{
		return VSD_BACK2IDLE_TIMEOUT;
	}
	else
		return VSD_SUCCESS;
}

/********************************************************************
 Description:
		Waiting the DMA end

 Parameters:
		Void.
		
 Returns:
		Reponse .		

********************************************************************/
SDSTATE Vsd_WaitTransCompleted(void)
{	
	MMD_U32 dRepeat;
	PSD_DEV pDev;
	SD_STATE_MODE stat;
	MMD_U08 socketid ;
	
	socketid = g_SdDev.current_socket;
	socketid &= 0x1;
	if(g_SdDev.sd_power[socketid] == SD_Power_Work)
	{
		pDev = &g_SdDev;
	}
	else
		return VSD_WORKMODE_ERR;

	stat = pDev->sd_state[socketid];
	dRepeat = 0;
#if (SD_INTERRUPT == 1)
	// enable_interrupts();   
	SD_EnableInt();   
#endif
	//Wait Controler DMA finish
	while (dRepeat<SD_WRCMD_WAIT_COUNT)
	{

#if (STRESS_TEST == 1)
		memcpy((void *)0x20052200, (void *)0x20052200, 512);
#endif

#if (SD_INTERRUPT ==0)
		Vsd_IntHandle();
		Vsd_ErrorHandle();
#else
		// Int
#endif
		if (stat !=  pDev->sd_state[socketid] )
			break;
		if (pDev->sd_state[socketid] == SD_State_Data_Err)
		{
			break;
		}
		dRepeat++;
		
#ifdef __CACHE_ON__
#ifndef __NEW_SYS__
		*((volatile int *)(0x6004f000)) = (0);
#endif
#endif

	}
	
#if (SD_SHOWTIME == 1)
	SDMSG(("WaitTrans %d\r\n",dRepeat));
#endif
	SD_DisableInt();   
#ifdef __CACHE_ON__  
	SD_EnableOtherInt();
#endif
	if(dRepeat >= SD_WRCMD_WAIT_COUNT)
	{
		pDev->sd_state[socketid] = SD_State_MARB_Stop;
	
		//software stop dma
		Vhost_WriteReg(VDEV_REG_SD_DMA_CTRL, 0x02);
		//reset buffer & state machine
		// reset state machine & buffer
		ASSERT(0xabcd5);
		Vhost_WriteReg(VDEV_REG_SD_RST, 0x0c);
		Vhost_WriteReg(VDEV_REG_SD_INT_FLAG, 0xffffffff);
	
		return VSD_DATA_TIMEOUT;
	}
	if (pDev->sd_state[socketid] == SD_State_Data_Err)
	{
		pDev->sd_state[socketid] = SD_State_Idle;
		//software stop dma
		Vhost_WriteReg(VDEV_REG_SD_DMA_CTRL, 0x02);
		// reset state machine & buffer
		ASSERT(0xabcd6);
		Vhost_WriteReg(VDEV_REG_SD_RST, 0x0c);
		Vhost_WriteReg(VDEV_REG_SD_INT_FLAG, 0xffffffff);
		return VSD_DATA_CRC_ERR;
	}
	return SD_RES_DAT_FINISH;
	
}

/********************************************************************
 Description:
		Reset SD 

 Parameters:
		bBusWidth:	4 bits or 1bit
		bWorkMode:	1: fifo mode, 0: buffer mode
		dOCR:		OCR value
		
 Returns:
		0 is success, otherwise return VSD_ERROR;.		

********************************************************************/
unsigned char bCSD[16];
SDSTATE Vsd_Reset(MMD_U08 bBusWidth, MMD_U08 bWorkMode, MMD_U32 dOCR)
{
	SDSTATE bResult;
	MMD_U32 bRegVal, i, dSDOCR, dMMCOCR, reg, sram_addr;
	SD_RESPONSE Response;
	MMD_U08 socketid ;
	MMD_U32 Data[512/4];
	MMD_U08 *SDData = (MMD_U08 *)Data;

	g_SdDev.current_socket = 1;
	
	socketid = g_SdDev.current_socket;
	socketid &= 0x1;

	g_SdDev.Mod[socketid] = TYPE_MEMONLY;
	g_SdDev.MemType[socketid] = TYPE_UNKONW;
	g_SdDev.version[socketid] = 1;
	g_SdDev.speedmode[socketid] = 4;
	g_SdDev.capacitymode[socketid] = 0;
	//com all module are swreset
	if(dOCR & 0x2) //3.3
	{
		dMMCOCR=0x00ff8000;
		dSDOCR=0x00ff8000;
	}
	if (dOCR&0x1) // 1.8
	{
		dMMCOCR|=0x80;
	}

	i=Vhost_ReadReg(0x60010300);//software reset control reg
	Vhost_WriteReg(0x60010300  ,  i&0xffdfffff); //the module of STO is not reset

	//new pad
	//Vhost_WriteReg(0x6017000c,0x0);
	i = Vhost_ReadReg(0x6017000c);
	Vhost_WriteReg(0x6017000c,i|0x300);
	
	i = Vhost_ReadReg(0x6017030c);
	Vhost_WriteReg(0x6017030c,i&0x9fffffff);
	
	SD_DisableInt();   
	Vhost_WriteReg(VDEV_REG_SD_RST, 0x0F);
	Vhost_WriteReg(VDEV_REG_SD_INT_FLAG, 0xffffffff);

	SD_SelectSocket(socketid);

	//**************SD clock ****************************************
	//Vhost_WriteReg(VDEV_REG_SD_CLK_CTRL, 0x01); // 1/2 system clock
	//MP
	//Vhost_WriteReg(VDEV_REG_SD_CLK_CTRL, 0x02); // 1/3 system clock
	Vhost_WriteReg(VDEV_REG_SD_CLK_CTRL, 0x24); // run 2M clock to ensure MMC reset succeed
#ifdef __MPW__
	//Vhost_WriteReg(VDEV_REG_SD_CLK_CTRL, 0x01); // 1/2 system clock
	Vhost_WriteReg(VDEV_REG_SD_CLK_CTRL, 0x02); // 1/3 system clock
#endif
	//********************SD timeout***********
	Vhost_WriteReg(VDEV_REG_SD_RESPONSE_TIMEOUT, SD_RESPONSE_TIMOUT_CNT);
	Vhost_WriteReg(VDEV_REG_SD_RDDATA_TIMEOUT, SD_READDATA_TIMEOUT_CNT);
	//*************************

	SD_DisableInt(); 	
	
	Vhost_Delay(1);//Vhost_Delay(2);
	
	g_SdDev.sd_power[socketid]  = SD_Power_Work;
	g_SdDev.BusWidth[socketid]  = 4;
	

	bResult = Vsd_SendCommand(CMD0_GO_IDLE_STATE, 0xffffffff);
	if(bResult != SD_RES_CMD_FINISH) //Command error
		return VSD_ERROR;
#if (SD_HICAPACITY == 1)
	for(i = 0; i < SD_CMD8_REPEAT_COUNT; i++)
	{
		bResult = Vsd_SendCommand(CMD8_SEND_IF_COND, 0x138);
		if(bResult == SD_RES_CMD_FINISH) //Command error
		{
			Vsd_GetResponse(CMD8_SEND_IF_COND, &Response);	
			if((((Response.bRsp[0])&0xff) == 0x38) && 
			(((Response.bRsp[0])>>8)&0xff))
			{
				//SD2.0
				g_SdDev.version[socketid] = 2;
				Vhost_WriteReg(VDEV_REG_SD_CLK_CTRL, 0x01); // 36MHz,1/2 system clock // 1/n+1 sys clock 					
				Vhost_Delay(6);//Vhost_Delay(100);
				break;
			}
		}
	}
#endif		
	for(i = 0; i < SD_ACMD41_REPEAT_COUNT; i++)
	{
		bResult = Vsd_SendCommand(CMD55_APP_CMD, 0);
		if(bResult != SD_RES_CMD_FINISH) {//Command error
			//return VSD_ERROR;
			//maybe mmc
			break;
		}			
		if( g_SdDev.version[socketid] < 2)
		{
			bResult = Vsd_SendAppCommand(ACMD41_SD_SEND_OP_COND, dSDOCR);
		}
		else
		{//sd 2.0
			bResult = Vsd_SendAppCommand(ACMD41_SD_SEND_OP_COND, dSDOCR|0x40000000);
		}
		if(bResult != SD_RES_CMD_FINISH) //Command error
			break;//return VSD_ERROR;				

		Vsd_GetResponse(ACMD41_SD_SEND_OP_COND, &Response);
		
		if(Response.dOCR&0x80000000) //Initial success
		{
			g_SdDev.MemType[socketid] = TYPE_SD; //is SD			
			if (bBusWidth != 4)
				g_SdDev.BusWidth[socketid]  = 1;
			
			if (g_SdDev.version[socketid] == 1)
				Vhost_WriteReg(VDEV_REG_SD_CLK_CTRL, 0x03); // 1/4 bus clock // 1/n+1 bus clock
#if (SD_HICAPACITY == 1)					
			else if (g_SdDev.version[socketid]  == 2)
			{
				g_SdDev.capacitymode[socketid]  = ((Response.dOCR) >> 30) & 0x1;				
				SDMSG(("Hicapacity"));
			}
#endif
			else
			{
				//ASSERT("Unknown sd version\n");
				ASSERT(0xFFFF); // avoid warning
				return VSD_ERROR;
			}

			break;
		}
		else
			continue;
		
	}

#if (SD_SHOWTIME ==1)
	SDMSG(("ACMD41 %d times\n",i));
#endif
	if (g_SdDev.MemType[socketid]  == TYPE_UNKONW) //check MMC
	{
		if(bBusWidth >1)
			dMMCOCR|=0x40000000;
		bResult = Vsd_SendCommand(CMD0_GO_IDLE_STATE, 0xffffffff);
		if(bResult!= SD_RES_CMD_FINISH) //Command error
			return VSD_ERROR;;
			
		for(i = 0; i < SD_CMD1_REPEAT_COUNT; i++)
		{
			bResult = Vsd_SendCommand(CMD1_SD_SEND_OP_COND, dMMCOCR);
			Vsd_GetResponse(CMD1_SD_SEND_OP_COND, &Response);
			if(Response.dOCR & 0x80000000) //Initial success
			{
				g_SdDev.MemType[socketid] = TYPE_MMC; //is SD
				g_SdDev.BusWidth[socketid]  = bBusWidth;
				break;
			}
		}
			
		if(i == SD_CMD1_REPEAT_COUNT)//Initial failed
			return VSD_ERROR;;
	}		


 //CARD_CHECK_OK
	//Get OCR
	g_SdDev.dOCR[socketid] = Response.dOCR;
	
	//Get CID
	bResult = Vsd_SendCommand(CMD2_ALL_SEND_CID, 0);
	Vsd_GetResponse(CMD2_ALL_SEND_CID, &Response);
	for(i=0; i<16; i++)
	{
		g_SdDev.bCID[socketid][i] = Response.bCID[i];
	}
	
	if(g_SdDev.MemType[socketid] == TYPE_MMC)//MMC
	{
		//Get RCA
		bResult = Vsd_SendCommand(CMD3_SEND_RELATIVE_ADDR, 0x10000);
		if(bResult!=SD_RES_CMD_FINISH) //Command error
			return VSD_ERROR;;
		Vsd_GetResponse(CMD3_SEND_RELATIVE_ADDR, &Response);
		g_SdDev.wRCA[socketid] = 0x1;
		Vhost_WriteReg(VDEV_REG_SD_CLK_CTRL, 0x03); // 1/4 bus clock // 1/n+1 bus clock
	}
	else
	{
		//Get RCA
		bResult = Vsd_SendCommand(CMD3_SEND_RELATIVE_ADDR, 0);
		Vsd_GetResponse(CMD3_SEND_RELATIVE_ADDR, &Response);
		g_SdDev.wRCA[socketid] = Response.wRCA;
	}

	//Get CSD
	bResult = Vsd_SendCommand(CMD9_SEND_CSD, g_SdDev.wRCA[socketid]);
	Vsd_GetResponse(CMD9_SEND_CSD, &Response);
	for(i=0; i<16; i++)
	{
		g_SdDev.bCSD[socketid][i] = Response.bCSD[i];
	}
	
	bResult = Vsd_SendCommand(CMD10_SEND_CID, g_SdDev.wRCA[socketid]);
	Vsd_GetResponse(CMD10_SEND_CID, &Response);
	for(i=0; i<16; i++)
		g_SdDev.bCID[socketid][i] = Response.bCID[i];
	
	//Select card
	bResult = Vsd_SendCommand(CMD7_SELECT_CARD, g_SdDev.wRCA[socketid]);
	if(bResult != SD_RES_CMD_FINISH) //Command error
		return VSD_ERROR;

	if(g_SdDev.MemType[socketid] != TYPE_MMC)
	{
		if(g_SdDev.BusWidth[socketid] == 4)
		{
			bResult = Vsd_SendCommand(CMD55_APP_CMD, g_SdDev.wRCA[socketid]);  
			if(bResult != SD_RES_CMD_FINISH) //Command error
				return VSD_ERROR;
			bResult = Vsd_SendAppCommand(ACMD6_SET_BUS_WIDTH, 0x2); //'10' 4bit
			if(bResult != SD_RES_CMD_FINISH) //Command error
			{
				bResult = Vsd_SendCommand(CMD55_APP_CMD, g_SdDev.wRCA[socketid]);  
				if(bResult != SD_RES_CMD_FINISH) //Command error
					return VSD_ERROR;
			
				bResult = Vsd_SendAppCommand(ACMD6_SET_BUS_WIDTH, 0x0); //'00' 1bit
				if(bResult != SD_RES_CMD_FINISH) //Command error
					return VSD_ERROR;
			
			Vhost_WriteReg(VDEV_REG_SD_MODE, SD_1BIT|SD_BUFFER_MODE);
		}
		else 
			Vhost_WriteReg(VDEV_REG_SD_MODE, SD_4BITS|SD_BUFFER_MODE);
		}
		else
		{
			bResult = Vsd_SendCommand(CMD55_APP_CMD, g_SdDev.wRCA[socketid]);  
			if(bResult != SD_RES_CMD_FINISH) //Command error
				return VSD_ERROR;
			bResult = Vsd_SendAppCommand(ACMD6_SET_BUS_WIDTH, 0x0); //'00' 1bit
			if(bResult != SD_RES_CMD_FINISH) //Command error
				return VSD_ERROR;		
			Vhost_WriteReg(VDEV_REG_SD_MODE, SD_1BIT|SD_BUFFER_MODE);
		}
	}
	else
	{//MMC
		reg = g_SdDev.BusWidth[socketid];
		reg /= 4;
		bResult=Vsd_SwitchCSD((MMD_U08)0xb7, (MMD_U08)reg);
		if(reg)//4bit
		{
			if(bResult < VSD_SUCCESS)
				Vhost_WriteReg(VDEV_REG_SD_MODE, SD_1BIT|SD_BUFFER_MODE);
			else
				Vhost_WriteReg(VDEV_REG_SD_MODE, SD_4BITS|SD_BUFFER_MODE);			
		}
		else//1bit
		{
			if(bResult < VSD_SUCCESS)
				Vhost_WriteReg(VDEV_REG_SD_MODE, SD_4BITS|SD_BUFFER_MODE);
			else
				Vhost_WriteReg(VDEV_REG_SD_MODE, SD_1BIT|SD_BUFFER_MODE);			
		}

		do {
			g_SdDev.speedmode[socketid] = 0;
			bResult=Vsd_SendCommand(CMD8_MMC_SENDEXTCSD, 0xffffffff);
			if(bResult < VSD_SUCCESS)
				break;
			g_BlockCnt = 1;
			sram_addr = MMD_SYS_Malloc(512);
			if(sram_addr == NULL)
				return VSD_ERROR;
			
			bResult = Vsd_StartTrans(sram_addr, 512);
			if(bResult < VSD_SUCCESS)
				break;

			bResult = Vsd_WaitTransCompleted();
			if(bResult < VSD_SUCCESS)
				break;

			Vm_RdSram(sram_addr, (MMD_S08 *)SDData, 512);
			MMD_SYS_Free(sram_addr);

			if(SDData[194] & 0x2)
				g_SdDev.speedmode[socketid] = 1;
			//card type 196
			//sector 212:215
			bRegVal = *(MMD_U32 *)&SDData[212];
			
			if ((!bRegVal) || (bRegVal < 0x400000))//<2G
			{
				g_SdDev.capacitymode[socketid] = 0;
				//printf("normal capacity");
			}
			else
			{
				g_SdDev.capacity[socketid] = bRegVal;					
				g_SdDev.capacitymode[socketid] = 1;
				//printf("high capacity");
			}
		}while(0);
	}
	//Set SD block size to 512 bytes
	bResult = Vsd_SendCommand(CMD16_SET_BLOCKLEN, SD_BLOCK_SIZE);
	if(bResult != SD_RES_CMD_FINISH) //Command error
		return VSD_ERROR;	

	//Set STO block size to 512 bytes
	Vhost_WriteReg(VDEV_REG_SD_BLK_SIZE, (SD_BLOCK_SIZE-1)); 
	
	//Set STO block number
	Vhost_WriteReg(VDEV_REG_SD_BLK_COUNT, 0xff); //256block
	
	//Interrupt enable
	Vhost_WriteReg(VDEV_REG_SD_INT_EN, VDEV_SD_NORMAL_INT_EN|VDEV_SD_ERROR_INT_EN|VDEV_SD_CMD12_ERROR_EN); 
	//Interrupt disable
	Vhost_WriteReg(VDEV_REG_SD_INT_EN, 0x00000000); 
	
	if(bWorkMode) //fifo
	{
		bRegVal = Vhost_ReadReg(VDEV_REG_SD_MODE);
		bRegVal |= 0x01;
		Vhost_WriteReg(VDEV_REG_SD_MODE, bRegVal);
	}
	else //buffer
	{
		bRegVal = Vhost_ReadReg(VDEV_REG_SD_MODE);
		bRegVal &= 0xfffffffe;
	
		Vhost_WriteReg(VDEV_REG_SD_MODE, bRegVal);
		
		//****** for 0x80 can't write bug ********************
	/*	bRegValnew = Vhost_ReadReg(VDEV_REG_SD_MODE);
		
		while( bRegValnew != bRegVal)
		{
			bRegValnew = Vhost_ReadReg(VDEV_REG_SD_MODE);
			Vhost_WriteReg(VDEV_REG_SD_MODE, bRegVal);
		}
		*/
	}

	//g_SdioDev.sd_power = SD_Power_Idle;
	g_SdDev.sd_state[socketid] = SD_State_Idle;
	
	return VSD_SUCCESS;
}




/********************************************************************
 Description:
		Start DMA 

 Parameters:
		dAddr:	byte addr
		len:		byte len
		
 Returns:
		VSD_SUCCESS is success, otherwise return VSD_ERROR;
Note:
		Addr must be 4byte aligned,
		len must be 4bytes aligned in SD mem
		can be misaligned in SDIO
		
********************************************************************/
SDSTATE Vsd_StartTrans(MMD_U32  dAddr , MMD_U32 len)
{
	MMD_U32 addr_align, len_align,mode;
	PSD_DEV pDev;
	MMD_U08 socketid,dmalen=1 ;
	
	socketid=g_SdDev.current_socket;
	socketid&=0x1;
	
	addr_align = dAddr&0x3;
	len_align = (len+3) & 0xfffffffc;
	if(addr_align) //address misaligned 4bytes
	{
		SDERR(("dma start addr misaligned %x\n",dAddr));
		
		return VSD_ERROR;
	}	
	
	if(g_SdDev.sd_power[socketid] == SD_Power_Work)
	{
		pDev = &g_SdDev;
	}
	else
		return VSD_WORKMODE_ERR;

	//burst mode
#ifndef __MPW__
	mode = Vhost_ReadReg(VDEV_REG_SD_MODE);
	mode &= SD_BURST_MASK;// 1 align
#if 1 // for single end prefech bug  
	if((len_align & 0x3f) == 0) // 64 align
	{
		mode |= SD_BURST_16;
		dmalen = 16;
	}
	else if((len_align & 0x1f) == 0)//32 align	
#else	
	if((len_align & 0x1f) ==0)//32 align
#endif
	{
		mode |= SD_BURST_8;
		dmalen = 8;
	}
	else if((len_align & 0xf) ==0)//16 align
	{
		mode |= SD_BURST_4;
		dmalen = 4;
	}
	Vhost_WriteReg(VDEV_REG_SD_MODE,mode);
#endif
	if (pDev->sd_state[socketid] == SD_State_Cmd)
	{
		ASSERT(1);
	}
	else if( pDev->sd_state[socketid] == SD_State_Wait_RdData)
		pDev->sd_state[socketid] = SD_State_RdingData;
	else if (pDev->sd_state [socketid]== SD_State_Wait_WrData)
		pDev->sd_state [socketid]= SD_State_WringData;
							
	if(len < 4) 
	{
		//address misaligned 4bytes
		Vhost_WriteReg(VDEV_REG_SD_DMA_STARTADDR, (MMD_U32)dAddr);	
		Vhost_WriteReg(VDEV_REG_SD_DMA_ENDADDR, (MMD_U32)dAddr + 4);	
		Vhost_WriteReg(VDEV_REG_SD_DMA_LEN, 1);
	}
	else
	{		
		Vhost_WriteReg(VDEV_REG_SD_DMA_STARTADDR, (MMD_U32)dAddr);	
		Vhost_WriteReg(VDEV_REG_SD_DMA_ENDADDR, (MMD_U32)dAddr + len_align);	
		Vhost_WriteReg(VDEV_REG_SD_DMA_LEN, len/dmalen/4);		
	}
#ifdef __CACHE_ON__ 
	  SD_DisableOtherInt();
	  _cpu_dcache_clean_range(dAddr,len_align);
#endif
#ifdef __CACHE_ON__  
	 _cpu_dcache_invalidate_range(dAddr,len_align);
#endif

	Vhost_WriteReg (VDEV_REG_SD_DMA_THRESHOLD, 0x100);
	Vhost_WriteReg (VDEV_REG_SD_DMA_CTRL, 0x4); //START

	return VSD_SUCCESS;	
}

/********************************************************************
 Description:
		Reset SDIO

 Parameters:
		pData:	Pointer to the data.
		
 Returns:
		0 is success, otherwise return VSD_ERROR;.		

********************************************************************/
#if (SDIO_ENABLE == 1)
SDSTATE Vsdio_Reset( MMD_U08 bBusWidth, MMD_U08 bWorkMode ,MMD_U32 OCR)
{
	MMD_U08 bResult,  bCurWidth=0;
	SD_RESPONSE Response;
	MMD_U08 socketid ;
	
	socketid=g_SdDev.current_socket;
	socketid&=0x1;
	
	g_SdDev.Mod[socketid] 		= TYPE_MEMONLY;
	g_SdDev.MemType[socketid]  	= TYPE_UNKONW;
	g_SdDev.BusWidth[socketid]  =4;
	if (bBusWidth !=4)
		g_SdDev.BusWidth[socketid]  =1;
	
	
	Vhost_WriteReg(VDEV_REG_SD_RST, 0x0F);
	g_SdDev.sd_power[socketid]  = SD_Power_Work;
	Vhost_Delay(1);//Vhost_Delay(2);

#ifndef __MPW__
		//com all module are swreset
//	smem   0x60010100    0x20000800  32  
 	Vhost_WriteReg(0x60010100, 0x20000800);

	Vhost_WriteReg(0x60010100, 0x20000000);   
	//new pad
	Vhost_WriteReg(0x6017000c,0x0);
	Vhost_WriteReg(0x6017030c,0x0);
#endif

#if (CURSYSTEM == AASP||CURSYSTEM == ECOS)
// ************* IRQ **********************************
	AASP_DRV_INTERRUPT_CREATE(CYGNUM_HAL_INTERRUPT_STO,
	                             0, // Priority
	                             (aasp_ISR_t *)SD_ISR,
	                             &SD_handle,
	                             &SD_intr);
	AASP_DRV_INTERRUPT_ATTACH(SD_handle);
	
	//enable 1st level interrupt:CYGNUM_HAL_INTERRUPT_UART		
	AASP_DRV_INTERRUPT_ACKNOWLEDGE(CYGNUM_HAL_INTERRUPT_STO);
	AASP_DRV_INTERRUPT_UNMASK(CYGNUM_HAL_INTERRUPT_STO);
#endif

	SD_DisableInt();
	Vhost_WriteReg(VDEV_REG_SD_RST, 0x0F);
	Vhost_WriteReg(VDEV_REG_SD_INT_FLAG, 0xffffffff);
	
#ifdef __NEW_SYS__
	SD_SelectSocket(socketid);
#endif

	SD_DisableInt();
	//**************SD clock ****************************************
	//Vhost_WriteReg(VDEV_REG_SD_CLK_CTRL, 0x01); // 1/2 system clock
	Vhost_WriteReg(VDEV_REG_SD_CLK_CTRL, 0x00); // 1/1 system clock
	
#ifdef __MPW__
	Vhost_WriteReg(VDEV_REG_SD_CLK_CTRL, 0x03); // 1/4 system clock
#endif

	//********************SD timeout***********
	Vhost_WriteReg(VDEV_REG_SD_RESPONSE_TIMEOUT,SD_RESPONSE_TIMOUT_CNT);
	Vhost_WriteReg(VDEV_REG_SD_RDDATA_TIMEOUT,SD_READDATA_TIMEOUT_CNT);
	//*************************
	bResult = Vsd_SendCommand(CMD5_SET_OP_COND, 0);
	Vsd_GetResponse(CMD5_SET_OP_COND, &Response);
	
	if(Response.sdio_r4.IONum>=0) 
	{
		g_SdDev.Mod[socketid]  = TYPE_SDIOONLY;
		g_SdDev.MemType[socketid]  = TYPE_UNKONW;
		g_SdDev.SdioFunc [socketid] = Response.sdio_r4.IONum;
		if (Response.sdio_r4.MemPreset)
		{
			g_SdDev.Mod[socketid]  = TYPE_COMBO;
			g_SdDev.MemType[socketid] = TYPE_SD; 
		}

			
		g_SdDev.dOCR[socketid]  = Response.sdio_r4.OCR; // Store OCR			
	}
	else
	{
		g_SdDev.Mod[socketid]  = TYPE_MEMONLY;
		return VSD_ERROR;; // faulse
	}

	g_SdDev.dOCR[socketid]  &= OCR;
	if(!g_SdDev.dOCR[socketid])
	{
		return VSD_ERROR;; // OCR can't support
	}
	
	do{	
		bResult = Vsd_SendCommand(CMD5_SET_OP_COND, g_SdDev.dOCR[socketid]  );
		Vsd_GetResponse(CMD5_SET_OP_COND, &Response);
	}while(Response.sdio_r4.C != 1);

	//Get RCA
	bResult = Vsd_SendCommand(CMD3_SEND_RELATIVE_ADDR, 0);
	Vsd_GetResponse(CMD3_SEND_RELATIVE_ADDR, &Response);
	g_SdDev.wRCA[socketid]  = Response.wRCA;
	
	//Select card
	bResult = Vsd_SendCommand(CMD7_SELECT_CARD, g_SdDev.wRCA[socketid] );
	Vsd_GetResponse(CMD7_SELECT_CARD, &Response);
	if (Vsd_VerifyResponse(CMD7_SELECT_CARD, &Response) != VSD_SUCCESS)
		return VSD_ERROR;

	//Set Bus Width
	Vsdio_ReadOneByte(0, 0, &bCurWidth, 1);
	bCurWidth &= 0xfc;
	//cd enable
	bCurWidth &= 0x7f;
	if(g_SdDev.BusWidth[socketid]  == 4)
		{
			
			bCurWidth |=0x2;
			Vhost_WriteReg(VDEV_REG_SD_MODE, SD_4BITS|SD_BUFFER_MODE);
		}
	else
	{
		bCurWidth |=0x1;
		Vhost_WriteReg(VDEV_REG_SD_MODE, SD_1BIT|SD_BUFFER_MODE);
	}
	Vsdio_WriteOneByte(0, 0x7, &bCurWidth, 1);
	
	
	Vhost_WriteReg(VDEV_REG_SD_WCRC_TIMEOUT, 0x80);//for Atheros card
	
	g_MaxBlocksize = 0;
	Vsdio_ReadOneByte(0, 0x8, (MMD_U08 *)&g_MaxBlocksize, 1);
	if(g_MaxBlocksize&0x2)
	{
		//support BLK mode
		SDMSG(("blk capable\n"));
		g_MaxBlocksize=0x4;
	}
	else
	{
		//don't support blk mode
		g_MaxBlocksize=0;
	}
	
	
	g_SdDev.sd_state[socketid] 	=	SD_State_Idle;
	return VSD_SUCCESS;// success
}

/********************************************************************
 Description:
		Read some bytes from sdio

 Parameters:
		Funcid:  Sdio funcid
 		dAddr:    Address read from.
		len:		byte cnt ,now we support none 4 byte aligned bytes
		pData:	Pointer to the data.
		Opcode: 0 fix addr, 1 incrementing addr
 Returns:
		The number of byte real read to STO.		

********************************************************************/
MMD_U32 Vsdio_ReadBytes(MMD_U08 funcid, MMD_U32 dAddr, MMD_U16 len,MMD_U08 *pData,MMD_U08 Opcode)
{
	MMD_U08 bNum,  bResult;
	MMD_U32  arg;
	SD_RESPONSE Response;
	
	arg =0; //read	
	arg = (funcid & 0x7)<<28;
	arg |= (Opcode&0x1)<<26; //byte increasing or fix address
	arg |= (dAddr & 0x1ffff)<<9;
	arg |= len & 0x1ff;

	Vhost_WriteReg(VDEV_REG_SD_BLK_SIZE,len-1);
	//set block num
	Vhost_WriteReg(VDEV_REG_SD_BLK_COUNT, 0x01); 

	bResult = Vsd_SendCommand(CMD53_IORW_EXT, arg);
		
	if (bResult != SD_RES_CMD_FINISH)
	{
		//Error
		return VSD_ERROR;;
	}

	 Vsd_GetResponse(CMD53_IORW_EXT, &Response);
	
	
	
	 Vsd_StartTrans((MMD_U32)pData , len);
	
	bResult = Vsd_WaitTransCompleted(); //Wait buffer is empty!
	//bResult = Vsd_WaitReadCompleted();
	if(bResult!=SD_RES_DAT_FINISH) 
		return VSD_ERROR;;

	bNum = Vhost_ReadReg(VDEV_REG_SD_DMA_PROCESS); 	
	return bNum*4;
}

/********************************************************************
 Description:
		write some bytes to sdio

 Parameters:
		Funcid:  Sdio funcid
 		dAddr:    Address read from.
		len:		byte cnt byte cnt ,now we support none 4 byte aligned bytes
		pData:	Pointer to the data.
		Opcode: 0 fix addr, 1 incrementing addr
 Returns:
		The number of byte real read to STO.		

********************************************************************/
MMD_U32 Vsdio_WriteBytes(MMD_U08 funcid, MMD_U32 dAddr, MMD_U16 len,MMD_U08 *pData,MMD_U08 Opcode)
{
	MMD_U08 bNum,  bResult;
	MMD_U32  arg;
	SD_RESPONSE Response;
	
	arg =0x80000000; //Write	
	arg |= (funcid & 0x7)<<28;
	arg |= (Opcode&0x1)<<26; //byte increasing or fix address
	arg |= (dAddr & 0x1ffff)<<9;
	arg |= len & 0x1ff;
	
	Vhost_WriteReg(VDEV_REG_SD_BLK_SIZE,len-1);
	//set block num
	Vhost_WriteReg(VDEV_REG_SD_BLK_COUNT, 0x01); 
	
	bResult = Vsd_SendCommand(CMD53_IORW_EXT, arg);
		
	if (bResult != SD_RES_CMD_FINISH)
	{
		//Error
		return VSD_ERROR;;
	}

	 Vsd_GetResponse(CMD53_IORW_EXT, &Response);
	
	

	Vsd_StartTrans((MMD_U32)pData , len);

	bResult = Vsd_WaitTransCompleted(); //Wait buffer is empty!
	//bResult = Vsd_WaitReadCompleted();
	if(bResult!=SD_RES_DAT_FINISH) 
		return VSD_ERROR;;

	bNum = Vhost_ReadReg(VDEV_REG_SD_DMA_PROCESS); 	
	return bNum*4;
}
/*************************************
* Description:
* 		set block length of function 1-7

* Parameters:
* 		blklen: new block length
*
* Note:
*
*
**************************************/
void Vsdio_SetMaxBlkSize(MMD_U32 blklen)
{
	g_MaxBlocksize = blklen;
}
/********************************************************************
 Description:
		write some blocks to sdio

 Parameters:
		Funcid:  Sdio funcid
 		dAddr:    Address read from.
 		blocksize: block size
		blockcnt:	block cnt
		pData:	Pointer to the data.
		Opcode: 0 fix addr, 1 incrementing addr
		
 Returns:
		The number of byte real read to STO.		

********************************************************************/
MMD_U32 Vsdio_WriteBlocks(MMD_U08 funcid, MMD_U32 dAddr, MMD_U16 blocksize,MMD_U16 blockcnt,MMD_U08 *pData,MMD_U08 Opcode)
{
	MMD_U08 bNum,  bResult;
	MMD_U32  arg,totalSize=0;
	MMD_U16  i;
	SD_RESPONSE Response;
	
	arg =0x80000000; //Write	
	arg |= (funcid & 0x7)<<28;
	arg |= (0x1)<<27; //block mode
	arg |= (Opcode&0x1)<<26; //byte increasing or fix address
	arg |= (dAddr & 0x1ffff)<<9;
	arg |= blockcnt & 0x1ff;
	
	if(g_CurBlocksize != blocksize)
	{
		// set new func x block size
		if( blocksize > g_MaxBlocksize)
		return VSD_BLOCKSIZE_OVERFLOW;
		g_CurBlocksize =blocksize ;
		//Vsdio_WriteBytes(0, funcid*0x100+0x10, 2,(MMD_U08 *)&g_CurBlocksize,1);
		Vsdio_WriteOneByte(0, funcid*0x100+0x10,(MMD_U08 *)&g_CurBlocksize, 0);
		Vsdio_WriteOneByte(0, funcid*0x100+0x11,((MMD_U08 *)&g_CurBlocksize)+1, 0);
	}
	
	Vhost_WriteReg(VDEV_REG_SD_BLK_SIZE,g_CurBlocksize-1);
	//set block num
	Vhost_WriteReg(VDEV_REG_SD_BLK_COUNT, blockcnt); 
	
	bResult = Vsd_SendCommand(CMD53_IORW_EXT, arg);
		
	if (bResult != SD_RES_CMD_FINISH)
	{
		//Error
		return 0;;
	}

	 Vsd_GetResponse(CMD53_IORW_EXT, &Response);
	
	
 	for(i=0,totalSize=0; i< blockcnt;i++)
	{
		Vsd_StartTrans((MMD_U32)pData+totalSize , g_CurBlocksize);
		bResult = Vsd_WaitTransCompleted(); //Wait buffer is empty!
		//bResult = Vsd_WaitReadCompleted();
		if(bResult!=SD_RES_DAT_FINISH) 
			return VSD_ERROR;
		bNum = Vhost_ReadReg(VDEV_REG_SD_DMA_PROCESS); 	
		totalSize += bNum*4;
	}

	

	
	return totalSize;
}


/********************************************************************
 Description:
		read some blocks from sdio

 Parameters:
		Funcid:  Sdio funcid
 		dAddr:    Address read from.
 		blocksize: block size
		blockcnt:	block cnt
		pData:	Pointer to the data.
		Opcode: 0 fix addr, 1 incrementing addr
		
 Returns:
		The number of byte real read to STO.		

********************************************************************/
MMD_U32 Vsdio_ReadBlocks(MMD_U08 funcid, MMD_U32 dAddr, MMD_U16 blocksize,MMD_U16 blockcnt,MMD_U08 *pData,MMD_U08 Opcode)
{
	MMD_U08 bNum,  bResult;
	MMD_U32  arg,totalSize=0;
	MMD_U16  i;
	SD_RESPONSE Response;
	
	arg = 	0x0; //Read	
	arg |= (funcid & 0x7)<<28;
	arg |= (0x1)<<27; //block mode
	arg |= (Opcode&0x1)<<26; //byte increasing or fix address
	arg |= (dAddr & 0x1ffff)<<9;
	arg |= blockcnt & 0x1ff;
	
	if(g_CurBlocksize != blocksize)
	{
		// set new func x block size
		if( blocksize > g_MaxBlocksize)
		return VSD_BLOCKSIZE_OVERFLOW;
		g_CurBlocksize =blocksize ;
	//	Vsdio_WriteBytes(0, funcid*0x100+0x10, 2,(MMD_U08 *)&g_CurBlocksize,1);
		Vsdio_WriteOneByte(0, funcid*0x100+0x10,(MMD_U08 *)&g_CurBlocksize, 0);
		Vsdio_WriteOneByte(0, funcid*0x100+0x11,((MMD_U08 *)&g_CurBlocksize)+1, 0);
	}
	
	Vhost_WriteReg(VDEV_REG_SD_BLK_SIZE,g_CurBlocksize-1);
	//set block num
	Vhost_WriteReg(VDEV_REG_SD_BLK_COUNT, blockcnt); 
	
	bResult = Vsd_SendCommand(CMD53_IORW_EXT, arg);
		
	if (bResult != SD_RES_CMD_FINISH)
	{
		//Error
		return 0;;
	}

	 Vsd_GetResponse(CMD53_IORW_EXT, &Response);
	
	
 	for(i=0,totalSize=0; i< blockcnt;i++)
	{
		Vsd_StartTrans((MMD_U32)pData+totalSize , g_CurBlocksize);
		bResult = Vsd_WaitTransCompleted(); //Wait buffer is empty!
		//bResult = Vsd_WaitReadCompleted();
		if(bResult!=SD_RES_DAT_FINISH) 
			return VSD_ERROR;
		bNum = Vhost_ReadReg(VDEV_REG_SD_DMA_PROCESS); 	
		totalSize += bNum*4;
	}

	

	
	return totalSize;
}


/********************************************************************
 Description:
		Read one byte from sdio

 Parameters:
		Funcid:  Sdio funcid
 		dAddr:    Address read from.
		len:	byte cnt
		pData:	Pointer to the data.
		RAW:	1: Read after Write 0: value same as Write
		
 Returns:
		VSD_SUCCESS

********************************************************************/
SDSTATE Vsdio_ReadOneByte(MMD_U08 funcid, MMD_U32 dAddr, MMD_U08 *pData, MMD_U08 RAW)
{
	MMD_U08   bResult;
	MMD_U32  arg;
	SD_RESPONSE Response;
	
	arg =0; //read	
	arg |= (funcid & 0x7)<<28;
	arg |= RAW<<27;
	arg |= (dAddr & 0x1ffff)<<9;
	

	bResult = Vsd_SendCommand(CMD52_IORW_DIRECT, arg);
		
	if (bResult != SD_RES_CMD_FINISH)
	{
		//Error
		return VSD_ERROR;;
	}

	 Vsd_GetResponse(CMD52_IORW_DIRECT, &Response);
	if (Response.sdio_r5.ResponseFlag & 0xcb) //  Errors 11001011
		{
			*pData = 0;
			return VSD_ERROR;
		}
	*pData = Response.sdio_r5.Data;
	
	return VSD_SUCCESS;
}


/********************************************************************
 Description:
		write one bytes from sdio

 Parameters:
		Funcid:  Sdio funcid
 		dAddr:    Address read from.
		len:	byte cnt
		pData:	Pointer to the data.
		RAW: 	read after write flag
		
 Returns:
		VSD_SUCCESS		

********************************************************************/
SDSTATE Vsdio_WriteOneByte(MMD_U08 funcid, MMD_U32 dAddr, MMD_U08 *pData, MMD_U08 RAW)
{
	MMD_U08 bResult;
	MMD_U32 arg;
	SD_RESPONSE Response;
	
	arg =0x80000000; //write	
	arg |= (funcid & 0x7)<<28;
	arg |= RAW<<27;
	arg |= (dAddr & 0x1ffff)<<9;
	arg |= (*pData & 0xff);
	
	bResult = Vsd_SendCommand(CMD52_IORW_DIRECT, arg);
		
	if (bResult != SD_RES_CMD_FINISH)
	{
		//Error
		return VSD_ERROR;;
	}

	 Vsd_GetResponse(CMD52_IORW_DIRECT, &Response);
	
	if (Response.sdio_r5.ResponseFlag & 0xcb) //  Errors 11001011
		{
			*pData = 0;
			return VSD_ERROR;
		}
	if (RAW)
	{
		if (*pData != Response.sdio_r5.Data)
		{
		 *pData = Response.sdio_r5.Data	;
		 return VSD_RAW_DATA_ERR;
		}
		else
		 return VSD_SUCCESS;
	}
	else
		return VSD_SUCCESS;
}
#endif

#if (SD_HISPEED == 1)
/********************************************************************
 Description:
		tune new clk of SD card & controler

 Parameters:
		newclk:  new clock
 		
		
 Returns:
		VSD_SUCCESS		

********************************************************************/
SDSTATE Vsd_SpeedTune(MMD_U32 newclk)
{
	MMD_U32 curSpeed; //0 normal 1:high
	MMD_U32 newSpeed=0;//0 normal 1:high
	MMD_U32 divide;
	//MMD_U32 arg;
	//MMD_U08 bResult;
	//MMD_U32 dNum;
	//SD_RESPONSE Response;
	SDSTATE ret;
	MMD_U08 socketid ;
	
	socketid = g_SdDev.current_socket;
	socketid &= 0x1;
	if(g_SdDev.speedmode[socketid] < 4)
	{//hispeed
		curSpeed = 1;
	}
	else
	{//normal speed
		curSpeed = 0;
	}
	
	if(newclk > 24)
	{
		newSpeed=1;
	}

	if(newSpeed > curSpeed)
	{
		//switch function 1	
		ret = Vsd_SwitchFunc(1,1);
		if(ret < VSD_SUCCESS)
			return VSD_ERROR;
	}
	else if(newSpeed == curSpeed)
	{
		
	}
	else
	{
		//switch function 0
		ret = Vsd_SwitchFunc(1,0);
		if(ret < VSD_SUCCESS)
			return VSD_ERROR;
	}

	//change clk in idle
	divide = VSD_HCLK/newclk;
	Vhost_WriteReg(VDEV_REG_SD_CLK_CTRL, divide-1); // 1/4 system clock
	
	//********************SD timeout***********
	Vhost_WriteReg(VDEV_REG_SD_RESPONSE_TIMEOUT, SD_RESPONSE_TIMOUT_CNT/divide*4);
	Vhost_WriteReg(VDEV_REG_SD_RDDATA_TIMEOUT, SD_READDATA_TIMEOUT_CNT/divide*4);
	
	return VSD_SUCCESS;
}

/********************************************************************
 Description:
		change sd new function

 Parameters:
		bGrpNum:  group number
 		bfuncMode: function number
		
 Returns:
		VSD_SUCCESS		

********************************************************************/

SDSTATE Vsd_SwitchFunc(MMD_U08 bGrpNum, MMD_U08 bfuncMode)
{
	MMD_U32 arg;
	MMD_U08 bResult;
	//MMD_U32 dNum;
	MMD_U08 *pData;
	SD_RESPONSE Response;
	
	pData = (MMD_U08 *)&gSDData;
	arg = 0x00ffffff;
	arg &= (~((MMD_U32)0xff<<((bGrpNum-1)*4)));
	arg |= ((MMD_U32)bfuncMode<<((bGrpNum-1)*4));

	bResult=Vsd_SendCommand(CMD6_SWITCH_FUNC,arg);
	if (bResult != SD_RES_CMD_FINISH)
	{
		//Error
		return VSD_ERROR;;
	}

	Vsd_GetResponse(CMD6_SWITCH_FUNC, &Response);
	Vsd_StartTrans((MMD_U32)pData , 64);
	
	bResult = Vsd_WaitTransCompleted(); //Wait buffer is empty!
		
	if(bResult!=SD_RES_DAT_FINISH) 
		return VSD_ERROR;

	if((pData[0]|pData[1])==0)
	{
	//error
	return VSD_SWITCHFUNC_ERROR;
	}
	if(((MMD_U16)(pData[13+(bGrpNum-1)*2]) +(MMD_U16)(pData[14+(bGrpNum-1)*2] <<8))&& ((MMD_U16)1<<bfuncMode))
	{
		//support funcMode
	}
	else
	{	
		//not support
		return VSD_ERROR;
	}
	
	bResult = Vsd_WaitControlerIdle();
	if(bResult!=VSD_SUCCESS) 
		return VSD_ERROR;

	arg|=0x80000000;
	bResult=Vsd_SendCommand(CMD6_SWITCH_FUNC,arg);
	if (bResult != SD_RES_CMD_FINISH)
	{
		//Error
		//return VSD_ERROR;;
	}

	Vsd_GetResponse(CMD6_SWITCH_FUNC, &Response);
	Vsd_StartTrans((MMD_U32)pData , 64);
	
	bResult = Vsd_WaitTransCompleted(); //Wait buffer is empty!
		
	if(bResult!=SD_RES_DAT_FINISH) 
		return VSD_ERROR;

	if(((pData[0]|pData[1])==0))
	{
		//error
		return VSD_SWITCHFUNC_ERROR;
	}
	bResult = Vsd_WaitControlerIdle();
	if(bResult!=VSD_SUCCESS) 
		return VSD_ERROR;
	else
		return VSD_SUCCESS;
}
#endif

void Vsd_ErrorHandle()
{
	MMD_U32 IntStat, dErrNum;
	PSD_DEV pDev;
	MMD_U08 socketid ;
	
	socketid = g_SdDev.current_socket;
	socketid &= 0x1;
	if(g_SdDev.sd_power[socketid] == SD_Power_Work)
	{
		pDev = &g_SdDev;
	}
	else
		return ;//VSD_WORKMODE_ERR

	//dIntMask = Vhost_ReadReg(VDEV_REG_SD_INT_EN);
	//Disable Int
	//Vhost_WriteReg(VDEV_REG_SD_INT_EN,0);
	SD_DisableInt();
	
	
	IntStat = Vhost_ReadReg(VDEV_REG_SD_INT_FLAG);
	IntStat = IntStat & VDEV_SD_ERR_INT_FLAGS;

	if(!IntStat)
		return; //No Error
	
	//SD Error
	for(dErrNum=0x100; dErrNum <(0x1<<23) ; dErrNum<<=1)
	{
		if (dErrNum & IntStat)
		{
			switch(dErrNum)
			{
				case 0x100: //Wr busyTimeout
					ASSERT(dErrNum);
					if( pDev->sd_state[socketid] == SD_State_Cmd_WrWithData)
						pDev->sd_state[socketid] = SD_State_Data_Err;
					else
					{
						ASSERT(dErrNum);
					}
					break;
				case 0x200: //R1b busy Timeout
					ASSERT(dErrNum);
					if( pDev->sd_state[socketid] == SD_State_Cmd_RdWithData)
						pDev->sd_state[socketid] = SD_State_Data_Err;
					else
					{	
						ASSERT(dErrNum);
					}
					break;
				case 0x400: //Write CRC timeout
					ASSERT(dErrNum);
					if( pDev->sd_state[socketid] == SD_State_Cmd_WrWithData)
						pDev->sd_state[socketid] = SD_State_Data_Err;
					else
					{	ASSERT(dErrNum);
						pDev->sd_state[socketid] = SD_State_Data_Err;
					}
					break;
				case 0x800: //Read Data TimeOut
					ASSERT(dErrNum);
					if( pDev->sd_state[socketid] == SD_State_Cmd_RdWithData)
						pDev->sd_state[socketid] = SD_State_Data_Err;
					else
					{	
						ASSERT(dErrNum);
						pDev->sd_state[socketid] = SD_State_Data_Err;
					}
					break;
				case 0x1000: //response Timeout
					ASSERT(dErrNum);
					SDMSG(("CMD %d:\n",Vhost_ReadReg(VDEV_REG_SD_CMD)));					
					if (( pDev->sd_state[socketid] == SD_State_Cmd)||( pDev->sd_state[socketid] == SD_State_Cmd_RdWithData)
						|| (pDev->sd_state[socketid] == SD_State_Cmd_WrWithData))
						pDev->sd_state[socketid] = SD_State_Cmd_Err;
					else
					{	
						ASSERT(dErrNum);
					}
					break;
				case 0x2000: //Read Data Crc Err
					ASSERT(dErrNum);
					if( pDev->sd_state[socketid] == SD_State_Cmd_RdWithData)
						pDev->sd_state[socketid] = SD_State_Cmd_Err;
					else
					{
						pDev->sd_state[socketid] = SD_State_Data_Err;
						ASSERT(dErrNum);
					}
					break;
				case 0x8000: //Write Data Crc Err
						ASSERT(dErrNum);
					if( pDev->sd_state[socketid] == SD_State_Cmd_WrWithData)
						pDev->sd_state[socketid] = SD_State_Cmd_Err;
					else
					{
						ASSERT(dErrNum);
					}
					break;
				case 0x100000://Response Crc Err
						ASSERT(dErrNum);
					if ( pDev->sd_state[socketid] == SD_State_Cmd)
						pDev->sd_state[socketid] = SD_State_Cmd_Err;
					else if( pDev->sd_state[socketid] == SD_State_Cmd_RdWithData)
						pDev->sd_state[socketid] = SD_State_Cmd_Err;
					else if (pDev->sd_state[socketid] == SD_State_Cmd_WrWithData)
						pDev->sd_state[socketid] = SD_State_Cmd_Err;
					else
					{
						ASSERT(dErrNum);
					}
					break;
				case 0x200000://Cmd index Err
					ASSERT(dErrNum);
					if ( pDev->sd_state[socketid] == SD_State_Cmd)
						pDev->sd_state[socketid] = SD_State_Cmd_Err;
					else if( pDev->sd_state[socketid] == SD_State_Cmd_RdWithData)
						pDev->sd_state[socketid] = SD_State_Cmd_Err;
					else if (pDev->sd_state[socketid] == SD_State_Cmd_WrWithData)
						pDev->sd_state[socketid] = SD_State_Cmd_Err;
					else
					{
						ASSERT(dErrNum);
					}
					break;
				case 0x400000: //End bit Err
					ASSERT(dErrNum);
					if ( pDev->sd_state[socketid] == SD_State_Cmd)
						pDev->sd_state[socketid] = SD_State_Cmd_Err;
					else if( pDev->sd_state[socketid] == SD_State_RdingData)
						pDev->sd_state[socketid] = SD_State_Cmd_Err;
					else if (pDev->sd_state[socketid] == SD_State_WringData)
						pDev->sd_state [socketid]= SD_State_Cmd_Err;
					else
					{
						ASSERT(dErrNum);
					}
					break;					
			}
		}
	}
	//Clear Int Flag
	Vhost_WriteReg(VDEV_REG_SD_INT_FLAG,IntStat);
	//Enable Int
	//Vhost_WriteReg(VDEV_REG_SD_INT_EN,dIntMask);
#if (SD_INTERRUPT == 1)
	SD_EnableInt();
#endif
}

void Vsd_IntHandle()
{
	MMD_U32 IntStat, dIntNum;
	PSD_DEV pDev;
	MMD_U08 socketid;
	
	socketid = g_SdDev.current_socket;
	socketid &= 0x1;
	if(g_SdDev.sd_power[socketid] == SD_Power_Work)
	{
		pDev = &g_SdDev;
	}
	else
		return ;//VSD_WORKMODE_ERR

	//dIntMask = Vhost_ReadReg(VDEV_REG_SD_INT_EN);
	//Disable Int
	//Vhost_WriteReg(VDEV_REG_SD_INT_EN,0);
	SD_DisableInt();
	IntStat = Vhost_ReadReg(VDEV_REG_SD_INT_FLAG);
	IntStat = VDEV_SD_NORMAL_INT_FLAG & IntStat;
	
	for(dIntNum = 0x1; dIntNum <(0x1<<23) ; dIntNum<<=1)
	{
		if (dIntNum & IntStat)
		{
			switch(dIntNum)
			{
				case VDEV_SD_NORMAL_INT_FLAG_CMD_OK:	//cmd complete
					if (pDev->sd_state[socketid] == SD_State_Cmd)
						pDev->sd_state[socketid] = SD_State_Idle;
					else if( pDev->sd_state[socketid] == SD_State_Cmd_RdWithData)
						pDev->sd_state[socketid] = SD_State_Wait_RdData;
					else if (pDev->sd_state[socketid] == SD_State_Cmd_WrWithData)
						pDev->sd_state[socketid] = SD_State_Wait_WrData;
					else
					{	
						//auto cmd12 in muti rw
						//ASSERT(1);
					}
					break;
				case VDEV_SD_NORMAL_INT_FLAG_BLOCK_FINISH:	//block finish
					break;

				case VDEV_SD_NORMAL_INT_FLAG_DATA3:	//card event
				case VDEV_SD_NORMAL_INT_FLAG_CARDREMOVED: 	//card event1
#if (CARDDETECT == 1)
					if (pDev->sd_state[socketid] == SD_State_Cmd)
					{
						pDev->sd_state[socketid] = SD_State_CardStop;
						ASSERT(dIntNum);	
					}
#endif
					/* don't care pre data transfer
					else if( pDev->sd_state == SD_State_Cmd_RdWithData)
						{pDev->sd_state = SD_State_CardStop;
						 ASSERT(dIntNum);	
						}
					else if (pDev->sd_state == SD_State_Cmd_WrWithData)
						{pDev->sd_state = SD_State_CardStop;
						 ASSERT(dIntNum);	
						}
					*/
					//	else if( pDev->sd_state == SD_State_RdingData)
					//		pDev->sd_state = SD_State_CardStop;
					//	else if (pDev->sd_state == SD_State_WringData)
					//		pDev->sd_state = SD_State_CardStop;
					break;
				case VDEV_SD_NORMAL_INT_FLAG_DMA_FINISH:	//DMA END
					if ( pDev->sd_state[socketid] == SD_State_Cmd)
					{
						ASSERT(dIntNum);
					}						
					else if( pDev->sd_state[socketid] == SD_State_Cmd_RdWithData)
					{
						ASSERT(dIntNum);
					}						
					else if (pDev->sd_state[socketid] == SD_State_Cmd_WrWithData)
					{	
						ASSERT(dIntNum);
					}						
					else if( pDev->sd_state[socketid] == SD_State_RdingData)
						pDev->sd_state[socketid] = SD_State_Wait_RdData;
					else if (pDev->sd_state[socketid] == SD_State_WringData)
						pDev->sd_state[socketid] = SD_State_Wait_WrData;
					break;
				case VDEV_SD_NORMAL_INT_FLAG_THRESHOLD_OK:	//Threshold int
					break;
				case VDEV_SD_NORMAL_INT_FLAG_AUTOCMD_OK:
					/*
					if( pDev->sd_state[socketid] == SD_State_RdingData)
					{
						//pDev->sd_state[socketid] = SD_State_Wait_RdData;
						g_RdAutoCmd12 = 1;
					}
					else if ( pDev->sd_state[socketid] == SD_State_Wait_RdData)
					{
						g_RdAutoCmd12 = 1;
					}
					else if (pDev->sd_state[socketid] == SD_State_WringData)
						pDev->sd_state[socketid] = SD_State_Idle;
					else if (pDev->sd_state[socketid] == SD_State_Wait_WrData)
						pDev->sd_state[socketid] = SD_State_Idle;
					*/
					g_RdAutoCmd12 = 1;
					break;

				case VDEV_SD_NORMAL_INT_FLAG_SDIO_OK:
					//SDIO handler
					break;					
			}
		}
	}
	//Clear Int Flag
	Vhost_WriteReg(VDEV_REG_SD_INT_FLAG,IntStat);
	//Enable Int
#if (SD_INTERRUPT == 1)
	SD_EnableInt();
#endif
	//Vhost_WriteReg(VDEV_REG_SD_INT_EN,dIntMask);
}


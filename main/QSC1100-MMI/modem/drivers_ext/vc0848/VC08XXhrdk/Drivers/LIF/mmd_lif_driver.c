/*************************************************
  Copyright (C), 2005-2006, Vimicro  Co., Ltd.
  File name:        
    MMD_LIF_Driver.c 
  Description:   
    LapisII Interface Driver.
  Function List:  
  History:
      <author>   <time>     <version >      <desc>
      lizg      06/10/17        1.0.00      Create  
*************************************************/

/******************************************************************************
  INCLUDE FILE 
******************************************************************************/
#include "mmp_rdk.h"

/******************************************************************************
  MACRO DEFINITION
******************************************************************************/

//#define MMD_LIF_DEBUG_INFO
#define MMD_LIF_DEBUG_ERR
#define MMD_LIF_DEBUG_NOTE
#define MMD_LIF_DEBUG_WARNING

#ifdef MMD_LIF_DEBUG_INFO
    #define PRINTF(WriteVal)        MMD_PRINTF(WriteVal) //general print out
#else
    #define PRINTF(WriteVal)
#endif

#ifdef MMD_LIF_DEBUG_ERR
    #define PRINTFE(WriteVal)   MMD_PRINTFE(WriteVal) //error
#else
    #define PRINTFE(WriteVal)
#endif

#ifdef MMD_LIF_DEBUG_NOTE
    #define PRINTFN(WriteVal)   MMD_PRINTFN(WriteVal) //note
#else
    #define PRINTFN(WriteVal)
#endif

#ifdef MMD_LIF_DEBUG_WARNING
    #define PRINTFW(WriteVal)   MMD_PRINTFW(WriteVal) //warning
#else
    #define PRINTFW(WriteVal)
#endif

/******************************************************************************
  TYPE DEFINITION
******************************************************************************/

 const MMD_U32 gD2H_STATUS_MemAddr[] = {
						V5_REG_SIF_AE0_DATA_16_19,
						V5_REG_SIF_AEWIN_START_STOP,//V5_REG_SIF_AE0_DATA_8_11,//
						V5_REG_SIF_HD_START_STOP,
						V5_REG_SIF_HREF_START,
						V5_REG_SIF_VD_START_STOP,
						V5_REG_SIF_AE0_DATA_8_11,//V5_REG_SIF_VSYNC_START_STOP,
						V5_REG_SIF_AE0_DATA_4_7,
						0x60051078,	//interrupt vector TV_Addr
						0x60051070,	//interrupt vector GE_Addr
                                        };

 const MMD_U32 gH2D_CMD_MemAddr[BIU_H2D_MAX_LEN] = {
					     V5_REG_MARB_IPC_B2A0,
					     V5_REG_MARB_IPC_B2A1,//V5_REG_SIF_AE0_DATA_8_11,//
					     V5_REG_SIF_AE0_DATA_12_15
                                        };
						 
 const MMD_U32 gD2H_ACK_MemAddr[BIU_D2H_MAX_LEN] = {
					     V5_REG_MARB_IPC_A2B0, 
                                        V5_REG_MARB_IPC_A2B1,
                                        V5_REG_SIF_AE0_DATA_0_3,
                                        };

/*************************************************
  Function:       
  	MMD_BIU_Send_D2H_Ack
  Description:    
  	later move to baseband
  Input:
  	NULL
  Output:        
  	NULL
  Return:         
  	Error code 
  History:        
      <author>  	<time>     	<desc>
      lizg    		06/12/31     	create this function  
*************************************************/
MMD_U32 MMD_BIU_Send_H2D_Cmd(MMD_MSG* pMsg)
{
	// write command and parameter 
	HAL_WRITE_UINT32(gH2D_CMD_MemAddr[0], pMsg->CMD);
	HAL_WRITE_UINT32(gH2D_CMD_MemAddr[1], pMsg->OP1);
	HAL_WRITE_UINT32(gH2D_CMD_MemAddr[2], pMsg->OP2);
	// invoke soft interrupt to VC0848
	HAL_WRITE_UINT32(DAVINCI_SIB2A_ST, 1);
	return MMD_RET_NOERR;
}


/*************************************************
  Function:       
  	MMD_BIU_Send_D2H_Ack
  Description:    
  	
  Input:
  	NULL
  Output:        
  	NULL
  Return:         
  	Error code 
  History:        
      <author>  	<time>     	<desc>
      lizg    		06/12/31     	create this function  
*************************************************/
MMD_U32 MMD_BIU_Get_D2H_Ack(MMD_MSG* pMsg)
{
	//MMD_U32 CMD ;
	// write command and parameter 
	HAL_READ_UINT32(gD2H_ACK_MemAddr[0], pMsg->CMD);
	HAL_READ_UINT32(gD2H_ACK_MemAddr[1], pMsg->OP1);
	HAL_READ_UINT32(gD2H_ACK_MemAddr[2], pMsg->OP2);
	//CMD = pMsg->CMD & CMD_MASK_MIN;
	//if(CMD == MIN_PLY_AUDIO_REC_BUF_INIT)
	//printf("VIM_BIU_Send_D2H_Ack (MIN_PLY_AUDIO_REC_BUF_INIT) \n");
	return MMD_RET_NOERR;
}

MMD_U32 MMD_BIU_Set_Device_Status(MMD_U32 index, MMD_U32 status)
{
	HAL_WRITE_UINT32(gD2H_STATUS_MemAddr[index], status);
	return MMD_RET_NOERR;
}
MMD_U32 MMD_BIU_Get_Device_Status(MMD_U32 index, MMD_U32* pstatus)
{
	HAL_READ_UINT32(gD2H_STATUS_MemAddr[index], *pstatus);
	return MMD_RET_NOERR;
}


/*=============================================================================
[FILE NAME]
  MMD_LIF_Driver.h 
[DESCRIPTION]
  LIF Driver headder.
[NOTE]
  None 
-------------------------------------------------------------------------------
All rights reserved, Copyright (C) vimicro LIMITED 2005 
=============================================================================*/
#ifndef _MMD_LIF_DRIVER_H_
#define _MMD_LIF_DRIVER_H_


/******************************************************************************
  INCLUDE FILE 
******************************************************************************/
#include "mmp_rdk.h"

/******************************************************************************
  MACRO DEFINITION used in host and device side,   
******************************************************************************/
// max commands can store in the host to device buffer
#define BIU_H2D_MAX_LEN       3 
#define BIU_D2H_MAX_LEN       3 

typedef struct _CMD_FORMAT
{
    MMD_U32 cmd;
    MMD_U32 op1;
    MMD_U32 op2;
}CMD_FORMAT, *pCMD_FORMAT;

typedef struct _MMD_MSG
{
    MMD_U32 CMD;
    MMD_U32 OP1;
    MMD_U32 OP2;
}MMD_MSG, *pMMD_MSG;

/******************************************************************************
  API functions 
******************************************************************************/
MMD_U32 MMD_BIU_Send_H2D_Cmd(MMD_MSG* pMsg);
MMD_U32 MMD_BIU_Get_D2H_Ack(MMD_MSG* pMsg);
MMD_U32 MMD_BIU_Set_Device_Status(MMD_U32 index, MMD_U32 status);
MMD_U32 MMD_BIU_Get_Device_Status(MMD_U32 index, MMD_U32* pstatus);

/******************************************************************************
// variable declaration
******************************************************************************/

extern const MMD_U32 gH2D_CMD_MemAddr[BIU_H2D_MAX_LEN];
extern const MMD_U32 gD2H_ACK_MemAddr[BIU_D2H_MAX_LEN];

#define AUD_STA_DEC_ERR		(1<<16)
#define AUD_STA_NO_SRC_DATA		(1<<17)
#define AUD_STA_NO_PCM_DATA		(1<<18)

typedef enum _BIU_STATUS_INDEX
{
	IDX_ERROR_CODE=0,
	IDX_AUD_STATUS=1,	
	IDX_AUD_DMA_COUNT=2,	
	IDX_RV_SEEK_TIME = 3,
	IDX_VID_STATUS = 4,
	IDX_PT_STATUS = 5,
	IDX_MP3_LENGTH =6,
	IDX_MP3_STATUS =7,
	IDX_FILE_STATUS = 8,
	
	BIU_STATUS_INDEX_RESERVED=0x7FFFFFFF		
}BIU_STATUS_INDEX;
#endif /*_MMD_LIF_DRIVER_H_*/


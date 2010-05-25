#ifndef _SDCTRL_H_
#define _SDCTRL_H_

#if 0
#include "def.h"
#include "platform.h"
#endif

#include "mmp_rdk.h"

//******************************************************************
//#define __CACHE_ON__

#ifndef SD_INTERRUPT
#define SD_INTERRUPT 0 //0: NO int		1: Int
#endif

#define SD_DEBUG 0

#ifndef SDIO_ENABLE
#define SDIO_ENABLE  0 //0: disable SDIO 1: enable SDIO
#endif

#ifndef CARDDETECT
#define CARDDETECT 0  //1:card detect   0:not support
#endif

#ifndef RESPONSEDEBUG
#define RESPONSEBUG 1
#endif

#ifndef SD_HICAPACITY
#define SD_HICAPACITY 1
#endif

#ifndef SD_HISPEED
#define SD_HISPEED 1
#endif

#ifndef VSD_HCLK
#define VSD_HCLK 48
//#define VSD_HCLK 72 //MP
#endif

#define MAXSD_DEVICE 2

#if (SD_DEBUG == 1)
	//Debug Msg print
	#define SDMSG(str) MMD_PRINTF(str)
	#define SDERR(str)  MMD_PRINTF(str) 
#else
	#define SDMSG(str) 
	#define SDERR(str)
#endif

//************** sd card informations ******************
typedef struct _SDIO_CARD_INFORMATION
{
	MMD_U08 version,capacitymode,speedmode;
	MMD_U08 speed;
	MMD_U08 minRead,maxRead,minWrite,maxWrite;
	MMD_U08 ReadBlkLen, EraseBlkEn, Eraselen;
	MMD_U32 c_size,c_size_mult,capacity;
	MMD_U08 MID;
	MMD_U16 OID;
	MMD_U08 PNM[5];
	MMD_U08 PRV;
	MMD_U32 PSN;
	MMD_U16 MDT;
}SDIO_CARD_INFORMATION, *PSDIO_CARD_INFORMATION;
//***************************************************

// ************** Time out Define ***********************
#define STRESS_TEST 	0
#define SD_SHOWTIME		0

#define SD_SEND_CMD_REPEAT_COUNT			1
#ifdef __CACHE_ON__
	#define SD_ACMD41_REPEAT_COUNT			50000
#else
	#define SD_ACMD41_REPEAT_COUNT			5000
#endif
#define SD_CMD1_REPEAT_COUNT				50
#define SD_CMD8_REPEAT_COUNT				3
#define SD_WR_REPEAT_COUNT				10
#define SD_RESPONSE_TIMOUT_CNT 			0xff //0x46 for sandisk 0x70 for kingmax
#define SD_READDATA_TIMEOUT_CNT        0xf000000
#ifdef __CACHE_ON__
	#ifdef __ECOS__
	#define SD_WRCMD_WAIT_COUNT				0x12fff0
	#define SD_IDLE_WAIT_COUNT				0x12ff0
	#define SD_CMDOK_WAIT_COUNT				0x12ff0
	#else
	#define SD_WRCMD_WAIT_COUNT				0x12fff0
	#define SD_IDLE_WAIT_COUNT				0x12ff0
	#define SD_CMDOK_WAIT_COUNT				0xfff0
	#endif
#else
	#define SD_WRCMD_WAIT_COUNT				0xfff0
	#define SD_IDLE_WAIT_COUNT				0xff0
	#define SD_CMDOK_WAIT_COUNT				0xff0
#endif


//****************************************************

typedef enum _SDSTATE
{
	SD_RES_CMD_FINISH	=0x01,
	SD_RES_DAT_FINISH	=0x02,
	SD_RES_WR_FINISH	=0x03,
	SD_RES_RD_FINISH	=0x04,
	VSD_SUCCESS 		=0,
	VSD_ERROR 			=-1,
	VSD_RESPONSE_TIMEOUT = 		-2,
	VSD_DATA_TIMEOUT		 = -3,
	VSD_BUSY_TIMEOUT 		=-4,
	VSD_RESPONSE_CRC_ERR	 	=-5,
	VSD_DATA_CRC_ERR 		=-6,
	VSD_STATE_IDLE_TIMEOUT = 	-7,
	VSD_RAW_DATA_ERR 		= -8,
	VSD_WORKMODE_ERR		= -9,
	VSD_CMDOK_TIMEOUT = 		-10,
	VSD_BACK2IDLE_TIMEOUT =		-11,
	VSD_BLOCKSIZE_OVERFLOW =	-12,
	VSD_SWITCHFUNC_ERROR =-13
}SDSTATE;

#define SDIOMEMBASE							0x60150000
//-----------SD Command-------------//
#define CMD0_GO_IDLE_STATE					0
#define CMD1_SD_SEND_OP_COND				1
#define CMD2_ALL_SEND_CID					2
#define CMD3_SEND_RELATIVE_ADDR				3
#define CMD4_SET_DSR						4
#define CMD5_SET_OP_COND					5
#define CMD6_SWITCH_FUNC					6
#define CMD7_SELECT_CARD					7
#define CMD8_SEND_IF_COND					8
#define CMD9_SEND_CSD						9
#define CMD10_SEND_CID						10
#define CMD12_STOP_TRANSMISSION				12
#define CMD13_SEND_STATUS					13
#define CMD15_GO_INACTIVE_STATE				15
#define CMD16_SET_BLOCKLEN					16
#define CMD17_READ_SINGLE_BLOCK				17
#define CMD18_READ_MULTIPLE_BLOCK			18
#define CMD24_WRITE_BLOCK					24
#define CMD25_WRITE_MULTIPLE_BLOCK			25
#define CMD27_PROGRAM_CSD					27
#define CMD28_SET_WRITE_PROT				28
#define CMD29_CLR_WRITE_PROT				29
#define CMD30_SEND_WRITE_PROT				30
#define CMD32_ERASE_WR_BLK_START			32
#define CMD33_ERASE_WR_BLK_END				33
#define CMD38_ERASE							38
#define CMD42_LOCK_UNLOCK					42
#define CMD52_IORW_DIRECT					52
#define CMD53_IORW_EXT						53
#define CMD55_APP_CMD						55
#define CMD56_GEN_CMD						56
#define CMD6_MMC_SWITCH						0x86
#define CMD8_MMC_SENDEXTCSD					0x88

#define ACMD6_SET_BUS_WIDTH					6
#define ACMD13_SD_STATUS					13
#define ACMD22_SEND_NUM_WR_BLOCKS			22
#define ACMD23_SET_WR_BLK_ERASE_COUNT		23
#define ACMD41_SD_SEND_OP_COND				41
#define ACMD42_SET_CLR_CARD_DETECT			42
#define ACMD51_SEND_SCR						51



//-----------SD Register-------------//
#define VDEV_REG_SD_DEIVCE_SELECT			(SDIOMEMBASE+0x04)

#define VDEV_REG_SD_MODE					(SDIOMEMBASE+0x80)
//burst mode
#define SD_BURST_16 		0x40	//3'b100: 16 word
#define SD_BURST_8 			0x30	//3'b011: 8word
#define SD_BURST_4 			0x20	//3'b010: 4word
#define SD_BURST_1 			0x00	// 3'b000: 1word
#define SD_BURST_MASK 		0xffffff8f
//bus width
#define SD_4BITS 			0x4
#define SD_1BIT 			0x0
#define SD_FIFO_MODE 		0x1
#define SD_BUFFER_MODE 		0x0

#define VDEV_REG_SDIO_CONFIG 				(SDIOMEMBASE+0x54)
#define SDIO_DATA_START_DISABLE 	0x01
#define SDIO_WAIT_BUSY_EN 			0x80
#define SDIO_READ_WAIT_EN 			0x40

#define VDEV_REG_SD_WCRC_TIMEOUT			(SDIOMEMBASE+0xd4)
#define VDEV_REG_SD_RST 					(SDIOMEMBASE+0x84)
#define VDEV_REG_SD_BLK_SIZE 				(SDIOMEMBASE+0x90)
#define VDEV_REG_SD_BLK_COUNT 				(SDIOMEMBASE+0x94)
#define VDEV_REG_SD_CMDREG1 				(SDIOMEMBASE+0x88)
#define VDEV_REG_SD_CMD 					(SDIOMEMBASE+0x8c)
#define VDEV_REG_SD_CMD_ARGU 				(SDIOMEMBASE+0x08)
#define VDEV_REG_SD_RESPONSE 				(SDIOMEMBASE+0x0c)
#define VDEV_REG_SD_AUTOCMD 				(SDIOMEMBASE+0xc0)
#define VDEV_REG_SD_AUTOARG 				(SDIOMEMBASE+0xc4)
#define VDEV_REG_SD_RESPONSE_TIMEOUT 		(SDIOMEMBASE+0xc8)
#define VDEV_REG_SD_DMA_STARTADDR 			(SDIOMEMBASE+0x4c)
#define VDEV_REG_SD_DMA_ENDADDR 			(SDIOMEMBASE+0x50)
#define VDEV_REG_SD_DMA_LEN 				(SDIOMEMBASE+0x58)
#define VDEV_REG_SD_DMA_THRESHOLD 			(SDIOMEMBASE+0x5c)
#define VDEV_REG_SD_DMA_PROCESS 			(SDIOMEMBASE+0x60)
#define VDEV_REG_SD_DMA_CTRL 				(SDIOMEMBASE+0x98)

#define VDEV_REG_SD_STATE 					(SDIOMEMBASE+0x20)
#define VDEV_REG_SD_CLK_CTRL 				(SDIOMEMBASE+0x9c)

#define VDEV_REG_SD_INT_EN 					(SDIOMEMBASE+0x24)
#define VDEV_SD_NORMAL_INT_EN 		0xff  //7:0
#define VDEV_SD_ERROR_INT_EN 		0xff00 //15:8
#define VDEV_SD_CMD12_ERROR_EN 		0xff0000 //23:16

#define VDEV_REG_SD_INT_FLAG 				(SDIOMEMBASE+0x28)
#define VDEV_SD_NORMAL_INT_FLAG 	0x0081403b
 									//7:0	0011 1011
 									//15:8	0100 0000
 									//23:16	1000 0001
#define VDEV_SD_ERR_INT_FLAGS 		0x000070bf00
 									//31-24 	0
 									//23-16 0111 0000
 									//15-8   1011 1111

#define VDEV_SD_NORMAL_INT_FLAG_SDIO_OK 		0x800000
#define VDEV_SD_NORMAL_INT_FLAG_AUTOCMD_OK 		0x10000
#define VDEV_SD_NORMAL_INT_FLAG_THRESHOLD_OK 	0x4000
#define VDEV_SD_NORMAL_INT_FLAG_DMA_FINISH 		0x20
#define VDEV_SD_NORMAL_INT_FLAG_CARDREMOVED 	0x10
#define VDEV_SD_NORMAL_INT_FLAG_DATA3 			0x08
#define VDEV_SD_NORMAL_INT_FLAG_BLOCK_FINISH 	0x02
#define VDEV_SD_NORMAL_INT_FLAG_CMD_OK 			0x01

#define VDEV_REG_SD_PRESENT_STATE 			(SDIOMEMBASE+0x20)
//sd_ctrl_current_state definition
#define VDEV_SD_CTRL_CURRENT_STATE	0x3c0000
#define VDEV_SD_DATA_CURRENT_STATE	0xf000000
#define VDEV_SD_WDMA_CURRENT_STATE 	0x10000
#define VDEV_SD_BUF_READ_STATE		0x100
#define VDEV_SD_BUF_WRITE_STATE		0x80
#define VDEV_SD_RDMA_CURRENT_STATE	0x20000
	
#define VDEV_REG_SD_RDDATA_TIMEOUT 			(SDIOMEMBASE+0xd8)
#define VDEV_REG_SD_R1B_BUSY_TIMEOUT 		(SDIOMEMBASE+0x36)
#define VDEV_REG_SD_WRCRC_BUSY_TIMEOUT 		(SDIOMEMBASE+0x37)
#define VDEV_REG_SD_WRCRC_TIMEOUT 			(SDIOMEMBASE+0x38)
#define VDEV_REG_SD_RD_DATA_TIMEOUT 		(SDIOMEMBASE+0x39)

#define VDEV_REG_SD_CMD_RD_BLK 				(SDIOMEMBASE+0x3C)
#define VDEV_REG_SD_CMD_RD_MULTIBLK 		(SDIOMEMBASE+0x3D)
#define VDEV_REG_SD_CMD_WR_BLK 				(SDIOMEMBASE+0x3E)
#define VDEV_REG_SD_CMD_WR_MULTIBLK 		(SDIOMEMBASE+0x3F)

#define VDEV_REG_SD_RESPONSE_DMA 			(SDIOMEMBASE+0x40)

#define VDEV_REG_SD_CRC_ERROR_TH 			(SDIOMEMBASE+0x49)

#define VDEV_REG_SD_BUF_RD_PTR 				(SDIOMEMBASE+0x58)
#define VDEV_REG_SD_BUF_WR_PTR 				(SDIOMEMBASE+0x59)


typedef enum _SD_CTRL_STATE
{
	VDEV_SD_CTRL_IDLE		=		0x0,
 	VDEV_SD_CTRL_CMD		=		0x1,
	 VDEV_SD_CTRL_RESP		=		0x2,
	 VDEV_SD_CTRL_DATA		=		0x3,
	 VDEV_SD_CTRL_CMD12		=		0x4,
	 VDEV_SD_CTRL_RESP12	=		0x5,
	 VDEV_SD_CTRL_WAIT_BUSY=		0x6
}SD_CTRL_STATE;

typedef enum _SD_DATA_STATE
{
	VDEV_SD_DATA_IDEL				=		0x0,
 	VDEV_SD_DATA_WAIT_START			=		0x1,
	 VDEV_SD_DATA_CONTENT			=		0x3,
	 VDEV_SD_DATA_CRC				=		0x4,
	 VDEV_SD_DATA_END				=		0x5,
	 VDEV_SD_DATA_WAIT_WRCRC		=		0x6,
	 VDEV_SD_DATA_WR_CRC			=		0x7,
	 VDEV_SD_DATA_BUSY				=		0x8
}SD_DATA_STATE;
 
#define VDEV_SD_ERROR_INT1_FLAG		0xff00
#define VDEV_SD_ERROR_INT2_FLAG		0xff0000

//Response bits

#define SD_RESPONSE_NONE					0	//0bit
#define SD_RESPONSE_R1						0x2	//48bit
#define SD_RESPONSE_R1B						0x3 //48bit busy
#define SD_RESPONSE_R2						0x1	//136bit
#define SD_RESPONSE_R3						0x2	//48bit
#define SD_RESPONSE_R4						0x2	//48bit
#define SD_RESPONSE_R5						0x2 //48bit
#define SD_RESPONSE_R6						0x2	//48bit
#define SD_RESPONSE_R7						0x2	//48bit

#define SD_BLOCK_SIZE						512

//Stop mode manually
#define SD_HOST_SEND_CMD12					0x80
//Data Block
#define SD_CMD_WITH_DATA_BLK				0x20
//Direction
#define SD_DATA_READ						0x10
#define SD_CMD_IDX_CHECK					0x08
// CRC enable
#define SD_CMD_CRC_CHECK					0x04

/*
#define SD_CMD_FINISH						0x01
#define SD_BLK_FINISH						0x02
#define SD_DMA_FINISH						0x04
#define SD_ERROR_INT1						0x80
#define SD_ERROR_INT2						0x40
*/

#define SD_BUF_WR_EN						0x80
#define SD_WR_ACTIVE						0x20
#define SD_RD_ACTIVE						0x40
#define SD_BUF_RD_EN						0x01
#define SD_DAT0_IDLE						0x08

#define SD_BUF_RESET						0x08
#define SD_HOST_BUF_RD						0x10
#define SD_CTRL_RESET						0x02
#define SD_REG_RESET						0x01

#define SD_WRITE_CRC_ERROR					0x80

//----------Interface-----------------//
typedef struct _SDIO_R4
{
	MMD_U32 OCR:24; //OCR
	MMD_U32 StuffBits:3;
	MMD_U32 MemPreset:1;
	MMD_U32 IONum:3;
	MMD_U32 C:1;	
}SDIO_R4, *PSDIO_R4;

typedef struct _SDIO_R5
{
	MMD_U32 Data:8 ; 
	MMD_U32 ResponseFlag:8 ;
	MMD_U32 Stuff:16 ;	
}SDIO_R5, *PSDIO_R5;

typedef struct _MMCSWITCH
{
	
	MMD_U32 cmdset:3;
	MMD_U32 zero:5;
	MMD_U32 value:8;
	MMD_U32 index:8;
	MMD_U32 access:2;
	MMD_U32 zero1:6;
}MMCSWITCH,*PMMCSWITCH;

typedef union _MMCARG
{
	MMCSWITCH arg;
	MMD_U32 reg;
}MMCARG;

typedef struct _MMC_CARD_STAT
{
	MMD_U32 reserve0:5; //default 0
	MMD_U32 acmd:1; //0 disable
	MMD_U32 reserve2:1;
	MMD_U32 switch_err :1;//0 ok 1:fail
	MMD_U32 ready_for_data:1;	 // 1: ready
	MMD_U32 cur_stat :4;	// 0:idle 1:ready 2:ident
	MMD_U32 erase_reset:1; 
	MMD_U32 reserve1:1;
	MMD_U32 wp_erase_skip:1; //bit:15
	MMD_U32 reserve3:8;
	MMD_U32 cid_csd_overwrite:1; // 1:error
	MMD_U32 overrun:1; // 1:error
	MMD_U32 underrun:1; // 1:error
	MMD_U32 error:1; //1:error
	MMD_U32 cc_err:1;
	MMD_U32 card_ecc_err:1;
	MMD_U32 illega_command:1;
	MMD_U32 com_crc_err:1;
	MMD_U32 lock_unlock_fail:1;
	MMD_U32 lock_is_lock:1;
	MMD_U32 wp_violation:1;
	MMD_U32 erase_param:1;
	MMD_U32 erase_seq_err:1;
	MMD_U32 block_len_err:1;
	MMD_U32 addr_misalign:1;
	MMD_U32 addr_outrange:1;
}MMC_CARD_STAT, *PMMC_CARD_STAT;
typedef struct _SDIO_CARD_STAT
{
	MMD_U32 reserve0:9; //default 0
	MMD_U32 cur_stat:4; //I/O only default 0xf
	MMD_U32 reserve1 :6;//default 0
	MMD_U32 general_err:1;	
	MMD_U32 reserve2 :2;
	MMD_U32 illegal_cmd:1;
	MMD_U32 crc_err:1;
	MMD_U32 reserve3:7;
	MMD_U32 out_of_range_err:1;	
}SDIO_CARD_STAT, *PSDIO_CARD_STAT;

typedef union _SD_RESPONSE
{
	MMD_U32	dStatus; //R1,R1b
	MMD_U32	dOCR;
	MMD_U16	wRCA;
	MMD_U08	bCID[16];
	MMD_U08	bCSD[16];
	MMD_U32	bRsp[4];
	SDIO_R4 sdio_r4;
	SDIO_R5 sdio_r5;
	MMD_U32	dRsp;
	SDIO_CARD_STAT sdio_stat;
	MMC_CARD_STAT mmc_stat;
}SD_RESPONSE, *PSD_RESPONSE;

typedef enum _SDPOWERMODE
{
	SD_Power_Idle = 0,
	SD_Power_Work,
	SD_Power_Sleep
}SD_POWER_MODE;

typedef enum _SDSTATEMODE
{
	SD_State_Idle= 0,
	//Cmd sent out
	SD_State_Cmd,
	SD_State_Cmd_RdWithData,
	SD_State_Cmd_WrWithData,
	//wait data
	SD_State_Wait_RdData,
	SD_State_Wait_WrData,
	//data transfering
	SD_State_RdingData,
	SD_State_WringData,
	//Normal state DMA need restart
	//SD_State_Rd_Data,  
	//SD_State_Wr_Data,
	//Error State
	SD_State_CardStop,		//need reset card
	SD_State_MARB_Stop,		//need reset Marb
	SD_State_Data_Err,		//need Cmd12 & reset fifo
	SD_State_Cmd_Err		//need send cmd again	
}SD_STATE_MODE;

//SD control structure
typedef struct _SD_DEV
{	
	MMD_U16	wRCA[MAXSD_DEVICE] ;
	MMD_U32	dOCR[MAXSD_DEVICE] ;
	MMD_U08	bCID[MAXSD_DEVICE] [16];
	MMD_U08	bCSD[MAXSD_DEVICE] [16];

	SD_POWER_MODE sd_power[MAXSD_DEVICE];
	SD_STATE_MODE  sd_state[MAXSD_DEVICE];
	MMD_U08 Mod[MAXSD_DEVICE]; //0 : mem 1: sdio
	MMD_U08 MemType[MAXSD_DEVICE]; //0 unknow 1:SD 2: miniSD 3:microSD 4: MMC
	MMD_U08 SdioFunc[MAXSD_DEVICE]; //how much functions
	MMD_U08 BusWidth[MAXSD_DEVICE]; // 1: 1bit 4: 4bits
	MMD_U08 version[MAXSD_DEVICE];
	MMD_U08 speedmode[MAXSD_DEVICE]; //96/speedmode : 0=6M 8=12M 4=24M 2=48M 1=96M
	MMD_U08 capacitymode[MAXSD_DEVICE];//0:nomal 1:high
	MMD_U08 current_socket;	//0,1
	MMD_U32 capacity[MAXSD_DEVICE];
}SD_DEV, *PSD_DEV;

enum MEMTYPE
{
	TYPE_UNKONW = 0,
	TYPE_SD,
	TYPE_MINISD,
	TYPE_MICROSD,
	TYPE_MMC
};

enum SDCARDTYPE
{
	TYPE_MEMONLY = 0,
	TYPE_SDIOONLY,
	TYPE_COMBO
};

void SD_SelectSocket(MMD_U08 socketid);
SDSTATE Vsd_Reset(MMD_U08 bBusWidth, MMD_U08 bWorkMode, MMD_U32 dOCR);
SDSTATE Vsdio_Reset( MMD_U08 bBusWidth, MMD_U08 bWorkMode ,MMD_U32 OCR);
SDSTATE Vsd_SendCommand(MMD_U08 bCmdID, MMD_U32 dArgu);
SDSTATE Vsd_SendAppCommand(MMD_U08 bCmdID, MMD_U32 dArgu);
SDSTATE Vsd_SendAutoCmd(MMD_U08 bCmdID, MMD_U32 dArgu);
SDSTATE Vsd_WaitCmdCompleted(void);
void Vsd_GetResponse(MMD_U08 bRspID, PSD_RESPONSE pRsp);
SDSTATE Vsd_WriteOneBlock(MMD_U32 dBlock, MMD_U32 dSRAMAddr);
SDSTATE Vsd_ReadOneBlock(MMD_U32 dBlock, MMD_U32 dSRAMAddr);
SDSTATE Vsd_WriteMultiBlock(MMD_U32 blk, MMD_U16 wBlockCnt);
MMD_U32 Vsd_WriteMultiData(MMD_U32 dAddr, MMD_U32 dLen);
SDSTATE Vsd_WriteMutiStop(MMD_U32 dAddr, MMD_U32 dLen);
SDSTATE Vsd_ReadMultiBlock(MMD_U32 dBlock, MMD_U16 wBlockCnt);
#define Vsd_ReadMultiData 	Vsd_WriteMultiData
#define Vsd_RWMultiData		Vsd_ReadMultiData
SDSTATE Vsd_ReadMultiStop(MMD_U32 dAddr, MMD_U32 dLen);

SDSTATE Vsd_WaitTransCompleted(void);
SDSTATE Vsd_WaitControlerIdle(void);

MMD_U08 Vsd_WaitWriteCompleted(void);
MMD_U08 Vsd_WaitReadCompleted(void);
MMD_U32 Vsd_GetCapacity(void);
SDSTATE  Vsd_StartTrans(MMD_U32  dAddr , MMD_U32 len);

#if (SD_HISPEED == 1)
SDSTATE Vsd_SwitchFunc(MMD_U08 bGrpNum, MMD_U08 bfuncMode);
SDSTATE Vsd_SpeedTune(MMD_U32 newclk);
#endif

#if 0
MMD_U32 Vsd_WriteData(MMD_U32 dAddr, MMD_U08 *pData, MMD_U32 dLen);
MMD_U32 Vsd_ReadData(MMD_U32 dAddr, MMD_U08 *pData, MMD_U32 dLen);
#endif

SDSTATE Vsdio_ReadOneByte(MMD_U08 funcid, MMD_U32 dAddr, MMD_U08 *pData, MMD_U08 RAW);
SDSTATE Vsdio_WriteOneByte(MMD_U08 funcid, MMD_U32 dAddr, MMD_U08 *pData, MMD_U08 RAW);

MMD_U32 Vsdio_ReadBytes(MMD_U08 funcid, MMD_U32 dAddr, MMD_U16 len,MMD_U08 *pData,MMD_U08 Opcode);
MMD_U32 Vsdio_WriteBytes(MMD_U08 funcid, MMD_U32 dAddr, MMD_U16 len,MMD_U08 *pData,MMD_U08 Opcode);
MMD_U32 Vsdio_ReadBlocks(MMD_U08 funcid, MMD_U32 dAddr, MMD_U16 blocksize,MMD_U16 blockcnt,MMD_U08 *pData,MMD_U08 Opcode);
MMD_U32 Vsdio_WriteBlocks(MMD_U08 funcid, MMD_U32 dAddr, MMD_U16 blocksize,MMD_U16 blockcnt,MMD_U08 *pData,MMD_U08 Opcode);
void  Vsdio_SetMaxBlkSize(MMD_U32 blklen);
void Vsd_GetCardInfo(PSDIO_CARD_INFORMATION info);
//for sandisk 
MMD_U32 Vsd_RdMultiAtOnce(MMD_U32 dBlock, MMD_U32 dSRAMAddr, MMD_U32 dCnt);
MMD_U32 Vsd_WrMultiAtOnce(MMD_U32 dBlock, MMD_U32 dSRAMAddr, MMD_U32 dBlockCnt);
//for low level erase SD
MMD_U32 Vsd_GetMinErasableSize(void);
MMD_U32 Vsd_Erase(MMD_U32 dwStartSector, MMD_U32 dwEndSector);

void Vsd_IntHandle(void);
void Vsd_ErrorHandle(void);


#endif

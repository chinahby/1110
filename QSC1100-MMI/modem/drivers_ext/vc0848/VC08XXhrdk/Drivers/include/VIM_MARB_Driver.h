/*******************************************************************************
 *  All rights reserved, Copyright (C) vimicro LIMITED 2007                                                     
 * --------------------------------------------------------------------------------------------------------------
 * [File Name]          : VIM_MARB_Driver.h
 * [Description]        : 适用于578b.和marb相关的函数   
 *				
 * [Author]                 : angela	
 * [Date Of Creation]  : 2007-03-19 16:01
 * [Platform]               : 平台无关
 * [Note]                 : 
 *
 * Modification History : 
 * ------------------------------------------------------------------------------
 * Date        			Author     		Modifications
 * ------------------------------------------------------------------------------
 * 2007-03-19 		angela      		 Created
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
#ifndef _VIM_5XRDK_MARB_H_
#define _VIM_5XRDK_MARB_H_

#ifdef __cplusplus
extern "C" {
#endif

#define VIM_MARB_JBUF_MINSIZE	0X1000
#define VIM_MARB_JBUF_MAXSIZE 0x1e000//0X3FC00//0x5000//0x3e800//
#define VIM_MARB_JBUF_MAXBLOCK	10
#define   VIM_MARB_JBUF_GAPINTERVAL	10240	//kb
//#define LCDCA0SDRAM 1//LCDCTEST 1
#define LCDCSDRAMA0ADDR	0x40000000	//
#define LCDCSDRAMA1ADDR	0x40080000	//
#define LCDCSDRAMB0ADDR	0x40100000	//
#define LCDCSDRAMB1ADDR	0x40180000	//
//#define LBUFNEWMEM 0

 typedef enum _VIM_MARB_SUB_RESET
 {
#if (BOARD_TYPE==VC0820_FPGA)
	VIM_MARB_SUB_RESET_ARMI 	= BIT0, 
	//reset marb_lbuf module
	VIM_MARB_SUB_RESET_ARMD 	= BIT1, 
	//reset marb_lcdc module
	VIM_MARB_SUB_RESET_DMAC0	= BIT2, 
	//reset marb_tvdc module
	VIM_MARB_SUB_RESET_DMAC1	= BIT3, 
	//reset marb_vdec module
	VIM_MARB_SUB_RESET_BIU		= BIT4, 
	//reset marb_venc module
	VIM_MARB_SUB_RESET_LBUF 	= BIT5, 
	//reset marb_jpeg module
	VIM_MARB_SUB_RESET_LCDC 	= BIT6, 
	//reset marb_ipp module
	VIM_MARB_SUB_RESET_TVDC 	= BIT7, 
	//reset 1T-SRAM 0
	VIM_MARB_SUB_RESET_USB		= BIT8, 
	//reset 1T-SRAM 1 
	VIM_MARB_SUB_RESET_SD	   = BIT9, 
	//reset 1T-SRAM 2
	VIM_MARB_SUB_RESET_NFC	   = BIT10, 
	//reset 1T-SRAM 2
	VIM_MARB_SUB_RESET_VDEC    = BIT11, 
	
	VIM_MARB_SUB_RESET_VENC    = BIT12,

	VIM_MARB_SUB_RESET_JPEG    = BIT13,

	VIM_MARB_SUB_RESET_IPP	   = BIT14,

	VIM_MARB_SUB_RESET_GE	   = BIT15,

	VIM_MARB_SUB_RESET_1T0	   = BIT16,

	VIM_MARB_SUB_RESET_1T1	   = BIT17,

	VIM_MARB_SUB_RESET_1T2	   = BIT18,
	//set 0 to all bit
	VIM_MARB_NO_RESET  =  0, 
	//set ALL reset
	VIM_MARB_RESET_ALL=(BIT0|BIT1|BIT2|BIT3|BIT4|BIT5|BIT6|BIT7|BIT8|BIT9|BIT10|BIT11|BIT12|BIT13|BIT14|BIT15|BIT16|BIT17|BIT18),

	VIM_MARB_ERROR=0x7FFFFFFF
#else
		  //reset marb_biu module
	  VIM_MARB_SUB_RESET_BIU     = BIT0, 
   	  //reset marb_lbuf module
	  VIM_MARB_SUB_RESET_LBUF     = BIT1, 
   	  //reset marb_lcdc module
	  VIM_MARB_SUB_RESET_LCDC     = BIT2, 
  	  //reset marb_tvdc module
	  VIM_MARB_SUB_RESET_TVDC     = BIT3, 
 	  //reset marb_vdec module
	  VIM_MARB_SUB_RESET_VDEC     = BIT4, 
 	  //reset marb_venc module
	  VIM_MARB_SUB_RESET_VENC     = BIT5, 
	  //reset marb_jpeg module
	  VIM_MARB_SUB_RESET_JPEG     = BIT6, 
	  //reset marb_ipp module
	  VIM_MARB_SUB_RESET_IPP     = BIT7, 
	  //reset 1T-SRAM 0
	  VIM_MARB_SUB_RESET_1TSRAM0    = BIT8, 
	  //reset 1T-SRAM 1
	  VIM_MARB_SUB_RESET_1TSRAM1    = BIT9, 
	  //reset 1T-SRAM 2
	  VIM_MARB_SUB_RESET_1TSRAM2    = BIT10, 
	  //reset 1T-SRAM 2
	  VIM_MARB_SUB_RESET_1TSRAM3    = BIT11, 
	  //set 0 to all bit
	  VIM_MARB_NO_RESET  =  0, 
	  //set ALL reset
	  VIM_MARB_RESET_ALL=(BIT0|BIT1|BIT2|BIT3|BIT4|BIT5|BIT6|BIT7|BIT8|BIT9|BIT10|BIT11)
#endif
 }VIM_MARB_SUB_RESET;
 typedef enum _VIM_MARB_CHANNEL
 {
#if (BOARD_TYPE==VC0820_FPGA)
 //RES	ARMD	DMAC0	DMAC1	BIU		JPEG		USB 		VDEC	VENC	IPP		NFSD	LBUF RES		LCDC RES		TVDCRES 		GERES	TSIDERES.
 //0		1		2		3		4		5			6			7		8		9		10		11			12				13				14		15

	// marb_armd module
	VIM_MARB_PRI_ARMD     = 1, 
	// marb_biu module
	VIM_MARB_PRI_BIU     = 4, 
	// marb_lbuf module
	VIM_MARB_PRI_LBUF     = 11, 
	// marb_lcdc module
	VIM_MARB_PRI_LCDC     = 12, 
	// marb_tvdc module
	VIM_MARB_PRI_TVDC     = 13, 
	// marb_vdec module
	VIM_MARB_PRI_VDEC     = 7, 
	// marb_venc module
	VIM_MARB_PRI_VENC     = 8, 
	// marb_jpeg module
	VIM_MARB_PRI_JPEG     = 5, 
	// marb_ipp module
	VIM_MARB_PRI_IPP     = 9,
	// NO Use
	VIM_MARB_PRI_NULL     = 0XF,

	VIM_MARB_PRI_UNKOWN =0x7FFFFFFF

#else
 	// marb_biu module
	  VIM_MARB_PRI_BIU     = 0, 
   	  // marb_lbuf module
	  VIM_MARB_PRI_LBUF     = 1, 
   	  // marb_lcdc module
	  VIM_MARB_PRI_LCDC     = 2, 
  	  // marb_tvdc module
	  VIM_MARB_PRI_TVDC     = 3, 
 	  // marb_vdec module
	  VIM_MARB_PRI_VDEC     = 4, 
 	  // marb_venc module
	  VIM_MARB_PRI_VENC     = 5, 
	  // marb_jpeg module
	  VIM_MARB_PRI_JPEG     = 6, 
	  // marb_ipp module
	  VIM_MARB_PRI_IPP     = 7,
	  // NO Use
	  VIM_MARB_PRI_NULL     = 0XF

#endif
 }VIM_MARB_CHANNEL;

 
typedef enum _VIM_MARB_INT_TYPE
{
	MARB_INT_FIFO0_FULL		= 0,
	MARB_INT_FIFO0_EMPTY		= 1, //ignore
	MARB_INT_FIFO0_THRESH_UP			= 2,
	MARB_INT_FIFO0_THRESH_LOW			= 3,
	MARB_INT_FIFO0_LARGE		= 4,
	MARB_INT_FIFO0_FRAME_END	= 5, //ignore
	MARB_INT_FIFO0_DONE		= 6,
	MARB_INT_FIFO1_FULL			= 8,
	MARB_INT_FIFO1_EMPTY		= 9,
	MARB_INT_FIFO1_THRESH_UP		= 10, //ignore
	MARB_INT_FIFO1_THRESH_LOW		= 11,
	MARB_INT_FIFO1_LARGE		= 12,
	MARB_INT_FIFO1_FRAME_END	= 13, //ignore
	MARB_INT_FIFO1_DONE		= 14,
	MARB_INT_ADDERROR		= 15,
	MARB_INT_ALL			= VIM_MARB_INT_NUM,
	MARB_INT_UNKOWN =0x7FFFFFFF
}VIM_MARB_INT_TYPE;
#define MARB_SLV_INT_ALL 6

  typedef enum _VIM_MARB_FIFONUM
 {
	VIM_MARB_FIFO_0=0,
	VIM_MARB_FIFO_1=2,
	VIM_MARB_FIFO_UNKOWN =0x7FFFFFFF
 }VIM_MARB_FIFONUM;

typedef enum _VIM_MARB_FIFOMODE
{
	MARB_MODE_NORMAL =0,
	MARB_MODE_ARMD_READ=2,
	MARB_MODE_ARMD_WRITE=3,
	MARB_MODE_UNKOWN =0x7FFFFFFF
}VIM_MARB_FIFOMODE;
   
typedef enum _VIM_MARB_WORKMODE {
	// Preview modes
	VIM_MARB_PREVIEW_MODE=0, // Auto Bit Rate Control enable
	VIM_MARB_MP4_PREVIEW_MODE, // added by lizg

	// Capture modes
	VIM_MARB_CAPTURESTILL_MODE,
	VIM_MARB_CAPTURESTILLTHUMB_MODE,
	VIM_MARB_CAPTURESTILLWITHFRAME_MODE,
	VIM_MARB_CAPTUREMPG4_MODE,

	// Encode modes
	VIM_MARB_ENCODE_MODE,

	// Display modes
	VIM_MARB_DISPLAYSTILL_MODE,
	VIM_MARB_DISPLAYSTILLBYPASS_MODE,
	VIM_MARB_DISPLAYMPG4_MODE,

	// Decode modes
#if (BOARD_TYPE==VC0820_FPGA)
	VIM_MARB_DECODEONLY_MODE,
	VIM_MARB_DECODERGBBYPASS_MODE,
	VIM_MARB_DISPLAYDEBUG_MODE,
#endif
	VIM_MARB_DECODEYUV_MODE, // Without IPP
	VIM_MARB_DECODERGB_MODE,
	
	//Video conference modes
	VIM_MARB_VIDEOCONFERENCE_MODE,
	
	// Special modes
	VIM_MARB_DIRECTDISPLAY_MODE,
	VIM_MARB_BYPASS_MODE,
	VIM_MARB_NONE_MODE,

	VIM_MARB_UNKOWN_MODE=0x7FFFFFFF
}VIM_MARB_WORKMODE;
#define VIM_MARB_MODENUM	(VIM_MARB_BYPASS_MODE+1)

  typedef enum _VIM_MARB_1TSRAM_NUM
 {
	VIM_MARB_1TSRAM_0=0,
	VIM_MARB_1TSRAM_1=4,
	VIM_MARB_1TSRAM_2=8,
	VIM_MARB_1TSRAM_3=12,
	VIM_MARB_1TSRAM_ALL=13,
	VIM_MARB_1TSRAM_NUM_UNKOWN=0x7FFFFFFF
 }VIM_MARB_1TSRAM_NUM;
   typedef enum _VIM_MARB_1TSRAM_MODE
 {
	VIM_MARB_1TSRAM_POWERON=0x5,
	VIM_MARB_1TSRAM_STANDBY=0x3,
	VIM_MARB_1TSRAM_OFF=0,
	VIM_MARB_1TSRAM_MODE_UNKOWN=0x7FFFFFFF
 }VIM_MARB_1TSRAM_MODE;
   
typedef enum _VIM_MARB_JPGCAPFLAG
{
	VIM_MARB_FIFOFULL = 0x01,
	VIM_MARB_FIFOTHRESUP = 0X04,
	VIM_MARB_FIFOLARGE	= 0X10,
	VIM_MARB_FIFOFRMEND   = 0X20,
	VIM_MARB_FIFOFRMCNT	= 0X40	
}VIM_MARB_JPGCAPFLAG;


typedef struct tag_VIM_MARB_Map {
		UINT32	jbufnowblock_int;			// 2, 3
		UINT32	jbufstart, jbufsize;			// 2, 3
		UINT32	thumbstart, thumbsize;			//7,  8
		UINT32	lbuf1Ystart,lbuf1Ustart,lbuf1Vstart;				//5
		UINT32	lbuf0Ystart,lbuf0Ustart,lbuf0Vstart;				//5
		UINT32  	lbufend;				//6
		UINT32	layerA0start, layerA0end;		//13, 14
		UINT32	layerA1start, layerA1end;		//15, 16
		UINT32	layerB0start, layerB0end;		//0, 1
		UINT32	layerB1start, layerB1end;		//0, 1
#if VIM_USER_SUPPORT_SDRAM

		UINT32	layerSDstart, layerSD1end;		//0, 1
#endif
#if(BOARD_TYPE==VC0820_FPGA)
		UINT32	Total_start,Total_end;		
#endif
}VIM_MARB_Map, *PVIM_MARB_Map;

#if(BOARD_TYPE==VC0820_FPGA)

typedef struct tag_VIM_MARB_VFIFO {
		UINT32	address;
		UINT32	length;
		UINT32	read_cur;
		UINT32	write_cur;
}VIM_MARB_Fifo, *PVIM_MARB_Fifo;

#endif

typedef struct tag_VIM_MARB_RAM_INFO {
		UINT32	Start_addr;
		UINT32	End_addr;
		UINT32	Top_Cursor;
		UINT32	Bottom_Cursor;
}VIM_MARB_RAM_INFO, *PVIM_MARB_RAM_INFO;

#define MARB_SRAM				0
#define MARB_SDRAM				1
#define MARB_FROM_TOP			0
#define MARB_FROM_BOTTOM		1
void VIM_MARB_RamInfo_Init(UINT32 sram_addr, UINT32 sram_len, UINT32 sdram_addr, UINT32 sdram_len);
UINT32 VIM_MARB_RamInfo_Alloc_FromTop(UINT32 flag, UINT32 len, UINT32 addr_align,UINT32* palloc_addr);
UINT32 VIM_MARB_RamInfo_Alloc_FromBottom(UINT32 flag, UINT32 len, UINT32 addr_align,UINT32* palloc_addr);
UINT32 VIM_MARB_RamInfo_GetFreesize(UINT32 flag, UINT32* pSize);
void VIM_MARB_RamInfo_Reset(UINT32 flag, UINT32 direction);
UINT32 VIM_MARB_RamInfo_GetCursor(UINT32 flag, UINT32 direction, UINT32* pAddr);
void VIM_MARB_RamInfo_Log2Phy(UINT32 Log_addr, UINT32* pPhy_addr);
void VIM_MARB_RamInfo_Phy2Log(UINT32 Phy_addr, UINT32* pLog_addr);

//marb module
void VIM_MARB_SetFifoChannel(VIM_MARB_CHANNEL Wt,VIM_MARB_CHANNEL Rd,VIM_MARB_FIFONUM Fifo);
void VIM_MARB_SetFifoEn(VIM_MARB_FIFONUM Fifo,BOOL Enable);
void VIM_MARB_SetFifoDisable(VIM_MARB_FIFONUM Fifo);


UINT32 VIM_MARB_GetJbufRWSize(void);
UINT32 VIM_MARB_ReadJpegData(HUGE UINT8 *Start, UINT32 length);
void VIM_MARB_ReadThumbData(HUGE UINT8 *Start, UINT32 length);
VIM_RESULT VIM_MARB_SetMap(VIM_MARB_WORKMODE bWorkMode,VIM_DISP_BUFFERMODER Aformat,PVIM_MARB_Map Maping);
UINT8 VIM_MARB_GetDeStatus(void);
void VIM_MARB_WriteJpegData(HUGE UINT8 *Start,UINT32 length, UINT8 end);

void _ISR_MarbIntHandle(UINT16 wIntFlag);

void VIM_MARB_RegisterISR(VIM_MARB_INT_TYPE bySecondLevelInt, PVIM_Isr  Isr);
void VIM_MARB_SetIntEnable(VIM_MARB_INT_TYPE bySecondLevelInt,BOOL Eanble);

void VIM_MARB_Set1TSramMode(VIM_MARB_1TSRAM_NUM Num,VIM_MARB_1TSRAM_MODE Mode);
void VIM_MARB_ResetSubModule(VIM_MARB_SUB_RESET Module);
VIM_RESULT VIM_MARB_InitMarbRereshTime(void);
	void VIM_MARB_SetMode(VIM_MARB_WORKMODE bMode);
UINT32 VIM_MARB_GetJbufUpThreshold(void);
void VIM_MARB_SetJpegbufGapratio(UINT8 ratio,VIM_MARB_FIFONUM Fifo);

void VIM_MARB_SetFifoStop(VIM_MARB_FIFONUM Fifo);
void VIM_MARB_SetRecaptureEn(VIM_MARB_FIFONUM Fifo,BOOL Enable);
void VIM_MARB_SetCaptureNum(VIM_MARB_FIFONUM Fifo,UINT8 Num);
VIM_RESULT VIM_MARB_SetCaptureMaxLen(VIM_MARB_FIFONUM Fifo,UINT32 Length);
void VIM_MARB_WriteMp4Data(HUGE UINT8 *Start,UINT32 length,UINT8 end);
UINT32 VIM_MARB_GetFifoSize(VIM_MARB_FIFONUM Fifo);

UINT32 VIM_MARB_GetMp4RWSize(void);
VIM_RESULT VIM_MARB_SetLCDMap(PVIM_MARB_Map Maping);
#if(BOARD_TYPE==VC0820_FPGA)
void VIM_MARB_SetFifoMode(VIM_MARB_FIFONUM fifoNum, VIM_MARB_FIFOMODE mode);
void VIM_MARB_SetFifoAddLen(VIM_MARB_FIFONUM fifoNum, UINT32 address, UINT32 length);
void VIM_MARB_SetFifoVAddr(VIM_MARB_FIFONUM fifoNum, UINT32 vaddr);
void VIM_MARB_SetFifoCount(VIM_MARB_FIFONUM fifoNum,VIM_MARB_FIFOMODE mode, UINT32 count);


#endif

#ifdef __cplusplus
}
#endif

#endif /* _RDK_HIF_H_ */

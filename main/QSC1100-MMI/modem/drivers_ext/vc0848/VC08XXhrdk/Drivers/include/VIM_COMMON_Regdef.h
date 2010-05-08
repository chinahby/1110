
/*******************************************************************************
 *  All rights reserved, Copyright (C) vimicro LIMITED 2007                                                     
 * --------------------------------------------------------------------------------------------------------------
 * [File Name]          : VIM_COMMON_Debug.h
 * [Description]        : 公共文件，
 *                       	    适用于578b。寄存器定义。
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


#ifndef _VIM_V578B_COMMON_REGDEF_H_
#define _VIM_V578B_COMMON_REGDEF_H_

#ifdef __cplusplus
extern "C" {
#endif


/***********************************************************************************/
#define 	V5_REG_BIU_BASE						0x60000000
#define 	V5_REG_CPM_BASE						0x60001000
#define 	V5_REG_CLK_BASE						0x60010000
#define 	V5_REG_SYS_BASE						0x60010000
#define 	V5_REG_SDRC_BASE						0x60030000
#define 	V5_REG_MARB_BASE						0x60040000
#define 	V5_REG_INT_BASE						0x60050000
#define 	V5_REG_ISP_BASE						0x60060000
#define 	V5_REG_IPP_BASE						0x60070000
#define 	V5_REG_LCDC_BASE						0x60080000
#define 	V5_REG_LCDIF_BASE						0x60090000
#define 	V5_REG_SIF_BASE						0x600a0000
#define 	V5_REG_LBUF_BASE						0x600b0000
#define 	V5_REG_JPEG_BASE						0x600c0000
#define 	V5_REG_AUD_BASE						0x60110000 
#define 	V5_REG_PADC_BASE						0x60170000
#define 	V5_REG_MP4ENC_BASE					0x60180000
#define 	V5_REG_MP4DEC_BASE					0x60190000
#define 	V5_REG_TVDC_BASE						0x601a0000
#define 	V5_REG_TVADPCM_BASE					0x601a4000
#define 	V5_REG_TVENC_BASE						0x601b0000

#if 0 // moved to vim_config.h by lizg
#ifdef __NEW_SYS__ // 820 MP chip macro, by lizg
//#define 	V5_1TSRAM_BASE						0x40C10000
//#define 	V5_1TSRAM_TOTALSIZE					0xF0000
#define 	V5_1TSRAM_BASE						0x40310000
#define 	V5_1TSRAM_TOTALSIZE					0xF0000
#else
#define 	V5_1TSRAM_BASE						0x20000000
#define 	V5_1TSRAM_TOTALSIZE					0x80000
#endif
#endif
#define 	V5_SDRAM_BASE							0x40000000
/***********************************************************************************/


/***********************************************************************************/
/*! BIU registers */
#define 	V5_REG_BIU_COMMAND					(V5_REG_BIU_BASE+0x0000)
#define 	V5_REG_BIU_CONFIG						(V5_REG_BIU_BASE+0x1004)
#define 	V5_REG_BIU_STATUS						(V5_REG_BIU_BASE+0x0008)
#define 	V5_REG_BIU_BURST_LEN					(V5_REG_BIU_BASE+0x000c)
#define 	V5_REG_BIU_BASE_ADDR					(V5_REG_BIU_BASE+0x0010)
#define 	V5_REG_BIU_BREAK_OCCUR				(V5_REG_BIU_BASE+0x00b0)
#define	V5_REG_BIU_BREAK_FINISH				(V5_REG_BIU_BASE+0x00b4)
/***********************************************************************************/

/***********************************************************************************/
#ifndef __NEW_SYS__ // 820 MP chip macro, by lizg
/*! CLOCK registers */
#define	V5_REG_CLK_RST_CTRL					(V5_REG_CLK_BASE+0x0000)
#define	V5_REG_CLK_GATE_CTRL					(V5_REG_CLK_BASE+0x0004)
#define	V5_REG_CLK_SIF_PCLK_EDGE_SEL			(V5_REG_CLK_BASE+0x0008)
#define	V5_REG_CLK_UMONITOR_CTRL				(V5_REG_CLK_BASE+0x0000)
#endif

#ifdef __NEW_SYS__ // 820 MP chip macro, by lizg
#define   V5_REG_SW_RESET						(V5_REG_CLK_BASE+0x0300)
#define	V5_REG_CKD_BP_CTL						(V5_REG_CLK_BASE+0x0108)
#define	V5_REG_PLL_CTL			 				(V5_REG_CLK_BASE+0x0000)
#define	V5_REG_CKD_EN_CTL						(V5_REG_CLK_BASE+0x0104)
#define	V5_REG_CLK_SWITCH						(V5_REG_CLK_BASE+0x0100)
#define	V5_REG_CGT_MC_CTL						(V5_REG_CLK_BASE+0x0114)
#define	V5_REG_CKD_AT_CON					(V5_REG_CLK_BASE+0x0128)
#define	V5_REG_CKD_CODE_CON					(V5_REG_CLK_BASE+0x013c)
#define	V5_REG_CKD_SCLK_CON					(V5_REG_CLK_BASE+0x0138)
#define	V5_REG_RTC_CTL						(V5_REG_CLK_BASE+0x1414)
#if 0  //modify by wanglixin
#define	V5_REG_GPIO_A_CFG		 				(V5_REG_PADC_BASE+0x000c)
#define	V5_REG_GPIO_B_CFG		 				(V5_REG_PADC_BASE+0x010c)
#define	V5_REG_GPIO_C_CFG		 				(V5_REG_PADC_BASE+0x020c)
#define	V5_REG_GPIO_D_CFG		 				(V5_REG_PADC_BASE+0x030c)
#define	V5_REG_FS_LEDM             					(V5_REG_PADC_BASE+0x0800)
#define	V5_REG_FS_SD_GPIO		 				(V5_REG_PADC_BASE+0x0804)
#endif
#endif

/***********************************************************************************/

/***********************************************************************************/
/*! Audio registers */
#define V5_REG_AUD_I2S_RX_CFG      				(V5_REG_AUD_BASE+0x00)
#define V5_REG_AUD_I2S_TX_CFG      				(V5_REG_AUD_BASE+0x04)
#define V5_REG_AUD_SHARE_CFG      				(V5_REG_AUD_BASE+0x08)
#define V5_REG_AUD_FIFO_CFG      				(V5_REG_AUD_BASE+0x0c)
#define V5_REG_AUD_RFIFO      					(V5_REG_AUD_BASE+0x10)
#define V5_REG_AUD_PFIFO      					(V5_REG_AUD_BASE+0x14)
#define V5_REG_AUD_MEM_FORMAT      			(V5_REG_AUD_BASE+0x18)
#define V5_REG_AUD_WS_DIV      					(V5_REG_AUD_BASE+0x1c)
#define V5_REG_AUD_WS_SRC						(V5_REG_AUD_BASE+0x20)
#define V5_REG_AUD_APPL						(V5_REG_AUD_BASE+0x24)
#define V5_REG_AUD_POINTER					(V5_REG_AUD_BASE+0x28)
#define V5_REG_AUD_REC_SAMPS_CNT   			(V5_REG_AUD_BASE+0x2c)
#define V5_REG_AUD_PLY_SAMPS_CNT   			(V5_REG_AUD_BASE+0x30)
#define V5_REG_AUD_DELAY						(V5_REG_AUD_BASE+0x34)
#define V5_REG_AUD_TRANS_TIME					(V5_REG_AUD_BASE+0x38)
#define V5_REG_AUD_START						(V5_REG_AUD_BASE+0xa0)    
#define V5_REG_AUD_CODEC_SYS       				(V5_REG_AUD_BASE+0x60)  
#define V5_REG_AUD_CODEC_LATCH     				(V5_REG_AUD_BASE+0x64) 
#define V5_REG_AUD_CODEC_DIG_IF    			(V5_REG_AUD_BASE+0x68)    
#define V5_REG_AUD_CODEC_VOL_CTRL1 			(V5_REG_AUD_BASE+0x6C)  
#define V5_REG_AUD_CODEC_VOL_CTRL2 			(V5_REG_AUD_BASE+0x70)  
#define V5_REG_AUD_CODEC_VOL_CTRL3 			(V5_REG_AUD_BASE+0x74)
#define V5_REG_AUD_CODEC_MUX       				(V5_REG_AUD_BASE+0x78)
#define V5_REG_AUD_CODEC_ATEST     				(V5_REG_AUD_BASE+0x7c)
#define V5_REG_AUD_CODEC_DTEST     				(V5_REG_AUD_BASE+0x80)
#define V5_REG_AUD_TEST            					(V5_REG_AUD_BASE+0xe0)
#define V5_REG_AUD_ARM_R_DATA      				(V5_REG_AUD_BASE+0xe4)
#define V5_REG_AUD_ARM_W_DATA      			(V5_REG_AUD_BASE+0xe8)
/***********************************************************************************/

/***********************************************************************************/

/***********************************************************************************/
/***********************************************************************************/
/*! SDRC registers */
#define	V5_REG_SDRC_CFG						(V5_REG_SDRC_BASE+0x0000)
#define	V5_REG_SDRC_MODE						(V5_REG_SDRC_BASE+0x0004)
#define	V5_REG_SDRC_TIMING					(V5_REG_SDRC_BASE+0x0008)
#define	V5_REG_SDRC_REFRESH					(V5_REG_SDRC_BASE+0x000c)
#define	V5_REG_SDRC_STATUS					(V5_REG_SDRC_BASE+0x0010)
#define	V5_REG_SDRC_COMMAND					(V5_REG_SDRC_BASE+0x0014)
/***********************************************************************************/

/***********************************************************************************/
/*! MARB registers */
//deleted and add some by youhai_mp4enc 2007.8.30
#ifdef __NEW_SYS__ // 820 MP chip macro, by youhai_mp4enc

#define	 V5_REG_MARB_SRAM_ADDR			(V5_REG_MARB_BASE +0x0004)//Add by youhai_mp4enc 2007.8.30
#define	 V5_REG_MARB_MEMORY_ADDR			(V5_REG_MARB_BASE +0x0008)
#define	 V5_REG_MARB_PRIORITY_0				(V5_REG_MARB_BASE +0x000c)
#define	 V5_REG_MARB_PRIORITY_1				(V5_REG_MARB_BASE +0x0010)
#define	 V5_REG_MARB_1T_CTRL					(V5_REG_MARB_BASE +0x0014)
#define	 V5_REG_MARB_RESET					(V5_REG_MARB_BASE +0x0018)
#define	 V5_REG_MARB_ADDRERROR				(V5_REG_MARB_BASE +0x001c)
#define	 V5_REG_MARB_MARB_PRIORITY_SET		(V5_REG_MARB_BASE +0x0020)
#define	 V5_REG_MARB_PRIORITY_ADJUST			(V5_REG_MARB_BASE +0x0024)
#define	 V5_REG_MARB_PAD_DELAY		(V5_REG_MARB_BASE +0x0028)//ADD by youhai_mp4enc 2007.8.30
#define	 V5_REG_MARB_VFIFO_STOP				(V5_REG_MARB_BASE +0x0030)
#define	 V5_REG_MARB_VFIFO_ENABLE			(V5_REG_MARB_BASE +0x0034)
#define	 V5_REG_MARB_VFIFO_VADDR				(V5_REG_MARB_BASE +0x0038)
#define	 V5_REG_MARB_VFIFO_CHANNELS			(V5_REG_MARB_BASE +0x003c)
#define	 V5_REG_MARB_VFIFO_CTRL_01			(V5_REG_MARB_BASE +0x0040)
//#define	 V5_REG_MARB_VFIFO_CTRL_23			(V5_REG_MARB_BASE +0x0044)//deleted by youhai_mp4enc 2007.8.30
#define	 V5_REG_MARB_VFIFO_STARTADDR_0		(V5_REG_MARB_BASE +0x0050)
#define	 V5_REG_MARB_VFIFO_STARTADDR_1		(V5_REG_MARB_BASE +0x0054)
//#define	 V5_REG_MARB_VFIFO_STARTADDR_2		(V5_REG_MARB_BASE +0x0058)//deleted by youhai_mp4enc 2007.8.30
//#define	 V5_REG_MARB_VFIFO_STARTADDR_3		(V5_REG_MARB_BASE +0x005c)//deleted by youhai_mp4enc 2007.8.30
#define	 V5_REG_MARB_VFIFO_LENGTH_0			(V5_REG_MARB_BASE +0x0060)
#define	 V5_REG_MARB_VFIFO_LENGTH_1			(V5_REG_MARB_BASE +0x0064)
//#define	 V5_REG_MARB_VFIFO_LENGTH_2			(V5_REG_MARB_BASE +0x0068)//deleted by youhai_mp4enc 2007.8.30
//#define	 V5_REG_MARB_VFIFO_LENGTH_3			(V5_REG_MARB_BASE +0x006c)//deleted by youhai_mp4enc 2007.8.30
#define	 V5_REG_MARB_VFIFO_THRESHOLD_0		(V5_REG_MARB_BASE +0x0070)	
#define	 V5_REG_MARB_VFIFO_THRESHOLD_1		(V5_REG_MARB_BASE +0x0074)	
//#define	 V5_REG_MARB_VFIFO_THRESHOLD_2		(V5_REG_MARB_BASE +0x0078)	//deleted by youhai_mp4enc 2007.8.30
//#define	 V5_REG_MARB_VFIFO_THRESHOLD_3		(V5_REG_MARB_BASE +0x007c)	//deleted by youhai_mp4enc 2007.8.30

#define	 V5_REG_MARB_VFIFO_TOO_LARGE_0		(V5_REG_MARB_BASE +0x0080)
#define	 V5_REG_MARB_VFIFO_TOO_LARGE_1		(V5_REG_MARB_BASE +0x0084)
//#define	 V5_REG_MARB_VFIFO_TOO_LARGE_2		(V5_REG_MARB_BASE +0x0088)//deleted by youhai_mp4enc 2007.8.30
//#define	 V5_REG_MARB_VFIFO_TOO_LARGE_3		(V5_REG_MARB_BASE +0x008c)//deleted by youhai_mp4enc 2007.8.30
#define	 V5_REG_MARB_VFIFO_COUNT_0			(V5_REG_MARB_BASE +0x0090)
#define	 V5_REG_MARB_VFIFO_COUNT_1			(V5_REG_MARB_BASE +0x0094)
//#define	 V5_REG_MARB_VFIFO_COUNT_2			(V5_REG_MARB_BASE +0x0098)//deleted by youhai_mp4enc 2007.8.30
//#define	 V5_REG_MARB_VFIFO_COUNT_3			(V5_REG_MARB_BASE +0x009c)//deleted by youhai_mp4enc 2007.8.30
#define	 V5_REG_MARB_VFIFO_MODE				(V5_REG_MARB_BASE +0x00e0)
#define	 V5_REG_MARB_VFIFO_COUNT_ADD_0		(V5_REG_MARB_BASE +0x00f0)
#define	 V5_REG_MARB_VFIFO_COUNT_SUB_0 		(V5_REG_MARB_BASE +0x00f4)
#define	 V5_REG_MARB_VFIFO_COUNT_ADD_1		(V5_REG_MARB_BASE +0x00f8)
#define	 V5_REG_MARB_VFIFO_COUNT_SUB_1 		(V5_REG_MARB_BASE +0x00fc)
#define	 V5_REG_MARB_IPC_A2B0					(V5_REG_MARB_BASE +0x1000)
#define	 V5_REG_MARB_IPC_A2B1					(V5_REG_MARB_BASE +0x1004)
#define	 V5_REG_MARB_IPC_A2B2					(V5_REG_MARB_BASE +0x1008)
#define	 V5_REG_MARB_IPC_A2B3					(V5_REG_MARB_BASE +0x100c)
#define	 V5_REG_MARB_IPC_B2A0					(V5_REG_MARB_BASE +0x1100)
#define	 V5_REG_MARB_IPC_B2A1					(V5_REG_MARB_BASE +0x1104)

#else

#define V5_REG_MARB_RESET						(V5_REG_MARB_BASE+0x0000)
#define V5_REG_MARB_ADDRERROR				(V5_REG_MARB_BASE+0x0004)
#define V5_REG_MARB_MEMORY_ADDR				(V5_REG_MARB_BASE+0x0008)
#define V5_REG_MARB_PRIOPITY					(V5_REG_MARB_BASE+0x000c)
#define V5_REG_MARB_1T_CTRL					(V5_REG_MARB_BASE+0x0010)
#define V5_REG_MARB_CLK32_DIVIDER				(V5_REG_MARB_BASE+0x0014)
#define	V5_REG_MARB_VFIFO_STOP				(V5_REG_MARB_BASE+0x0028)
#define V5_REG_MARB_VFIFO_ENABLE				(V5_REG_MARB_BASE+0x002c)
#define V5_REG_MARB_VFIFO_CHANNELS			(V5_REG_MARB_BASE+0x0030)
#define V5_REG_MARB_VFIFO_CTRL				(V5_REG_MARB_BASE+0x0034)
#define V5_REG_MARB_VFIFO_TOO_LARGE0			(V5_REG_MARB_BASE+0x0038)
#define V5_REG_MARB_VFIFO_TOO_LARGE1			(V5_REG_MARB_BASE+0x003c)
#define V5_REG_MARB_VFIFO_STARTADDR_0		(V5_REG_MARB_BASE+0x0040)
#define V5_REG_MARB_VFIFO_LENGTH_0			(V5_REG_MARB_BASE+0x0044)
#define V5_REG_MARB_VFIFO_THRESHOLD_0		(V5_REG_MARB_BASE+0x0048)
#define V5_REG_MARB_VFIFO_COUNT_0			(V5_REG_MARB_BASE+0x004c)
#define V5_REG_MARB_VFIFO_STARTADDR_1		(V5_REG_MARB_BASE+0x0060)
#define V5_REG_MARB_VFIFO_LENGTH_1			(V5_REG_MARB_BASE+0x0064)
#define V5_REG_MARB_VFIFO_THRESHOLD_1		(V5_REG_MARB_BASE+0x0068)
#define V5_REG_MARB_VFIFO_COUNT_1			(V5_REG_MARB_BASE+0x006c)

#endif
/***********************************************************************************/

/***********************************************************************************/
/*! INT registers */
#ifdef __NEW_SYS__ // 820 MP chip macro, by youhai_mp4enc
#define V5_REG_INT_INTCRTL			(V5_REG_INT_BASE + 0x000000)
#define V5_REG_INT_VECADDR		(V5_REG_INT_BASE + 0x000004)
#define V5_REG_INT_RSDCNT			(V5_REG_INT_BASE + 0x000008)

#define V5_REG_INT_HIRQST			(V5_REG_INT_BASE + 0x1000)
#define V5_REG_INT_HFIQST			(V5_REG_INT_BASE + 0x1004)
#define V5_REG_INT_BIUIRQST		(V5_REG_INT_BASE + 0x1008)
#define V5_REG_INT_INTMASK			(V5_REG_INT_BASE + 0x100C)
#define V5_REG_INT_INTSLCT			(V5_REG_INT_BASE + 0x1014)
#define V5_REG_INT_HINTEN			(V5_REG_INT_BASE + 0x1018)
#define V5_REG_INT_BIUINTEN		(V5_REG_INT_BASE + 0x101C)
#define V5_REG_INT_1LvINTClr		(V5_REG_INT_BASE + 0x1020)

#define V5_REG_INT_IRQPrio0		(V5_REG_INT_BASE + 0x1200)
#define V5_REG_INT_IRQPrio1		(V5_REG_INT_BASE + 0x1204)
#define V5_REG_INT_IRQPrio2		(V5_REG_INT_BASE + 0x1208)
#define V5_REG_INT_IRQPrio3		(V5_REG_INT_BASE + 0x120C)

#define V5_REG_INT_TEST			(V5_REG_INT_BASE + 0x3000)

#define V5_REG_INT_MARB_Addr		(V5_REG_INT_BASE + 0x1024)
#define V5_REG_INT_UOTG_Addr		(V5_REG_INT_BASE + 0x1028)
#define V5_REG_INT_NFC_Addr		(V5_REG_INT_BASE + 0x102C)
#define V5_REG_INT_STO_Addr		(V5_REG_INT_BASE + 0x1030)
#define V5_REG_INT_DMAC_Addr		(V5_REG_INT_BASE + 0x1034)
#define V5_REG_INT_BIU_Addr		(V5_REG_INT_BASE + 0x1038)
#define V5_REG_INT_I2C_Addr		(V5_REG_INT_BASE + 0x103C)
#define V5_REG_INT_SPI_Addr		(V5_REG_INT_BASE + 0x1040)
#define V5_REG_INT_KEY_Addr		(V5_REG_INT_BASE + 0x1044)
#define V5_REG_INT_GPIO_Addr		(V5_REG_INT_BASE + 0x1048)
#define V5_REG_INT_UART_Addr		(V5_REG_INT_BASE + 0x104C)
#define V5_REG_INT_IPP_Addr		(V5_REG_INT_BASE + 0x1050)
#define V5_REG_INT_RTC_Addr		(V5_REG_INT_BASE + 0x1054)
#define V5_REG_INT_TIM_Addr		(V5_REG_INT_BASE + 0x1058)
#define V5_REG_INT_AUDIO_Addr		(V5_REG_INT_BASE + 0x105C)
#define V5_REG_INT_SIF_Addr		(V5_REG_INT_BASE + 0x1060)
#define V5_REG_INT_ISP_Addr		(V5_REG_INT_BASE + 0x1064)
#define V5_REG_INT_LBUF_Addr		(V5_REG_INT_BASE + 0x1068)
#define V5_REG_INT_JPEG_Addr		(V5_REG_INT_BASE + 0x106C)
#define V5_REG_INT_GE_Addr			(V5_REG_INT_BASE + 0x1070)
#define V5_REG_INT_LCD_Addr		(V5_REG_INT_BASE + 0x1074)
#define V5_REG_INT_TV_Addr			(V5_REG_INT_BASE + 0x1078)
#define V5_REG_INT_SIB2A_Addr		(V5_REG_INT_BASE + 0x1080)
#define V5_REG_INT_VENC_Addr		(V5_REG_INT_BASE + 0x1084)
#define V5_REG_INT_VDEC_Addr		(V5_REG_INT_BASE + 0x1088)
#define V5_REG_INT_TSAddr			(V5_REG_INT_BASE + 0x108C)
#define V5_REG_INT_HIFAddr			(V5_REG_INT_BASE + 0x1090)

#define V5_REG_INT_MARB_ST			(V5_REG_INT_BASE + 0x2000)
#define V5_REG_INT_MARB_EN			(V5_REG_INT_BASE + 0x2004)
#define V5_REG_INT_UOTG_ST			(V5_REG_INT_BASE + 0x2008)
#define V5_REG_INT_UOTG_EN			(V5_REG_INT_BASE + 0x200C)
#define V5_REG_INT_NFC_ST			(V5_REG_INT_BASE + 0x2010)
#define V5_REG_INT_NFC_EN			(V5_REG_INT_BASE + 0x2014)
#define V5_REG_INT_STO_ST			(V5_REG_INT_BASE + 0x2018)
#define V5_REG_INT_STO_EN			(V5_REG_INT_BASE + 0x201C)
#define V5_REG_INT_DMAC_ST			(V5_REG_INT_BASE + 0x2020)
#define V5_REG_INT_DMAC_EN			(V5_REG_INT_BASE + 0x2024)
#define V5_REG_INT_BIU_ST			(V5_REG_INT_BASE + 0x2028)
#define V5_REG_INT_BIU_EN			(V5_REG_INT_BASE + 0x202C)
#define V5_REG_INT_I2C_ST			(V5_REG_INT_BASE + 0x2030)
#define V5_REG_INT_I2C_EN			(V5_REG_INT_BASE + 0x2034)
#define V5_REG_INT_SPI_ST			(V5_REG_INT_BASE + 0x2038)
#define V5_REG_INT_SPI_EN			(V5_REG_INT_BASE + 0x203C)
#define V5_REG_INT_KEY_ST			(V5_REG_INT_BASE + 0x2040)
#define V5_REG_INT_KEY_EN			(V5_REG_INT_BASE + 0x2044)
#define V5_REG_INT_GPIO_ST			(V5_REG_INT_BASE + 0x2048)
#define V5_REG_INT_GPIO_EN			(V5_REG_INT_BASE + 0x204C)
#define V5_REG_INT_UART_ST			(V5_REG_INT_BASE + 0x2050)
#define V5_REG_INT_UART_EN			(V5_REG_INT_BASE + 0x2054)
#define V5_REG_INT_IPP_ST			(V5_REG_INT_BASE + 0x2058)
#define V5_REG_INT_IPP_EN			(V5_REG_INT_BASE + 0x205C)
#define V5_REG_INT_RTC_ST			(V5_REG_INT_BASE + 0x2060)
#define V5_REG_INT_RTC_EN			(V5_REG_INT_BASE + 0x2064)
#define V5_REG_INT_TIM1_ST			(V5_REG_INT_BASE + 0x2068)
#define V5_REG_INT_TIM1_EN			(V5_REG_INT_BASE + 0x206C)
#define V5_REG_INT_AUDIO_ST		(V5_REG_INT_BASE + 0x2070)
#define V5_REG_INT_AUDIO_EN		(V5_REG_INT_BASE + 0x2074)
#define V5_REG_INT_SIF_ST			(V5_REG_INT_BASE + 0x2078)
#define V5_REG_INT_SIF_EN			(V5_REG_INT_BASE + 0x207C)
#define V5_REG_INT_ISP_ST			(V5_REG_INT_BASE + 0x2080)
#define V5_REG_INT_ISP_EN			(V5_REG_INT_BASE + 0x2084)
#define V5_REG_INT_LBUF_ST			(V5_REG_INT_BASE + 0x2088)
#define V5_REG_INT_LBUF_EN			(V5_REG_INT_BASE + 0x208C)
#define V5_REG_INT_JPEG_ST			(V5_REG_INT_BASE + 0x2090)
#define V5_REG_INT_JPEG_EN			(V5_REG_INT_BASE + 0x2094)
#define V5_REG_INT_GE_ST			(V5_REG_INT_BASE + 0x2098)
#define V5_REG_INT_GE_EN			(V5_REG_INT_BASE + 0x209C)
#define V5_REG_INT_LCD_ST			(V5_REG_INT_BASE + 0x20A0)
#define V5_REG_INT_LCD_EN			(V5_REG_INT_BASE + 0x20A4)
#define V5_REG_INT_TV_ST			(V5_REG_INT_BASE + 0x20A8)
#define V5_REG_INT_TV_EN			(V5_REG_INT_BASE + 0x20AC)
#define V5_REG_INT_SIA2B_ST		(V5_REG_INT_BASE + 0x20B0)
#define V5_REG_INT_SIA2B_EN		(V5_REG_INT_BASE + 0x20B4)
#define V5_REG_INT_SIB2A_ST		(V5_REG_INT_BASE + 0x20B8)
#define V5_REG_INT_SIB2A_EN		(V5_REG_INT_BASE + 0x20BC)
#define V5_REG_INT_VENC_ST			(V5_REG_INT_BASE + 0x20C0)
#define V5_REG_INT_VENC_EN			(V5_REG_INT_BASE + 0x20C4)
#define V5_REG_INT_VDEC_ST			(V5_REG_INT_BASE + 0x20C8)
#define V5_REG_INT_VDEC_EN			(V5_REG_INT_BASE + 0x20CC)
#define V5_REG_INT_TS_ST			(V5_REG_INT_BASE + 0x20D0)
#define V5_REG_INT_TS_EN			(V5_REG_INT_BASE + 0x20D4)
#define V5_REG_INT_HIF_ST			(V5_REG_INT_BASE + 0x20D8)
#define V5_REG_INT_HIF_EN			(V5_REG_INT_BASE + 0x20DC)
#define V5_REG_INT_TIM2_ST			(V5_REG_INT_BASE + 0x20E0)
#define V5_REG_INT_TIM2_EN			(V5_REG_INT_BASE + 0x20E4)
#define V5_REG_INT_WDOG_ST			(V5_REG_INT_BASE + 0x20E8)
#define V5_REG_INT_WDOG_EN			(V5_REG_INT_BASE + 0x20EC)

#else
#define V5_REG_INT_UMCTRL						(V5_REG_INT_BASE+0x0000)
#define V5_REG_INT_ENABLE_FIRST				(V5_REG_INT_BASE+0x0004)
#define V5_REG_INT_CTRL							(V5_REG_INT_BASE+0x0008)
#define V5_REG_INT_SERV_FIRST					(V5_REG_INT_BASE+0x000c)
#define V5_REG_INT_FLAG_FIRST					(V5_REG_INT_BASE+0x0010)
        
#define V5_REG_INT_FLAG_SIF					(V5_REG_INT_BASE+0x0100)
#define V5_REG_INT_FLAG_ISP					(V5_REG_INT_BASE+0x0104)
#define V5_REG_INT_FLAG_LBUF					(V5_REG_INT_BASE+0x0108)
#define V5_REG_INT_FLAG_JPEG					(V5_REG_INT_BASE+0x010c)
#define V5_REG_INT_FLAG_IPP					(V5_REG_INT_BASE+0x0110)
#define V5_REG_INT_FLAG_MARB					(V5_REG_INT_BASE+0x0114)
#define V5_REG_INT_FLAG_LCDC					(V5_REG_INT_BASE+0x0118)
#define V5_REG_INT_FLAG_TVDC					(V5_REG_INT_BASE+0x011c)
#define V5_REG_INT_FLAG_VDEC					(V5_REG_INT_BASE+0x0120)
#define V5_REG_INT_FLAG_VENC					(V5_REG_INT_BASE+0x0124)
#define V5_REG_INT_FLAG_HWT					(V5_REG_INT_BASE+0x0128)
#define	V5_REG_INT_FLAG_GPIO					(V5_REG_INT_BASE+0x012c)
        
#define V5_REG_INT_ENABLE_SIF					(V5_REG_INT_BASE+0x0200)
#define V5_REG_INT_ENABLE_ISP					(V5_REG_INT_BASE+0x0204)
#define V5_REG_INT_ENABLE_LBUF				(V5_REG_INT_BASE+0x0208)
#define V5_REG_INT_ENABLE_JPEG				(V5_REG_INT_BASE+0x020c)
#define V5_REG_INT_ENABLE_IPP					(V5_REG_INT_BASE+0x0210)
#define V5_REG_INT_ENABLE_MARB				(V5_REG_INT_BASE+0x0214)
#define V5_REG_INT_ENABLE_LCDC				(V5_REG_INT_BASE+0x0218)
#define V5_REG_INT_ENABLE_TVDC				(V5_REG_INT_BASE+0x021c)
#define V5_REG_INT_ENABLE_VDEC				(V5_REG_INT_BASE+0x0220)
#define V5_REG_INT_ENABLE_VENC				(V5_REG_INT_BASE+0x0224)
#define	V5_REG_INT_ENABLE_GPIO				(V5_REG_INT_BASE+0x022c)
#endif
/***********************************************************************************/

/***********************************************************************************/
/*! IPP registers */
#define V5_REG_IPP_CTRL							(V5_REG_IPP_BASE+0x0000)
#define V5_REG_IPP_IMGSIZE						(V5_REG_IPP_BASE+0x0004)
#define V5_REG_IPP_WINSIZE						(V5_REG_IPP_BASE+0x0008)
#define V5_REG_IPP_WINST_POS					(V5_REG_IPP_BASE+0x000C)
#define V5_REG_IPP_DISPTGT_SIZE				(V5_REG_IPP_BASE+0x0010)
#define V5_REG_IPP_DISH_W						(V5_REG_IPP_BASE+0x0014)
#define V5_REG_IPP_DISH_S						(V5_REG_IPP_BASE+0x0018)
#define V5_REG_IPP_DISH_C						(V5_REG_IPP_BASE+0x001C)
#define V5_REG_IPP_DISV_H						(V5_REG_IPP_BASE+0x0020)
#define V5_REG_IPP_DISV_S						(V5_REG_IPP_BASE+0x0024)
#define V5_REG_IPP_DISV_C						(V5_REG_IPP_BASE+0x0028)
#define V5_REG_IPP_CAPTGT_SIZE					(V5_REG_IPP_BASE+0x002c)
#define V5_REG_IPP_CAPDH_W					(V5_REG_IPP_BASE+0x0030)
#define V5_REG_IPP_CAPDH_S						(V5_REG_IPP_BASE+0x0034)       
#define V5_REG_IPP_CAPDH_C						(V5_REG_IPP_BASE+0x0038)
#define V5_REG_IPP_CAPDV_H						(V5_REG_IPP_BASE+0x003c)
#define V5_REG_IPP_CAPDV_S						(V5_REG_IPP_BASE+0x0040)
#define V5_REG_IPP_CAPDV_C						(V5_REG_IPP_BASE+0x0044)
#define V5_REG_IPP_CAPUH_W					(V5_REG_IPP_BASE+0x0048)        
#define V5_REG_IPP_CAPUH_S						(V5_REG_IPP_BASE+0x004c)
#define V5_REG_IPP_CAPUV_H						(V5_REG_IPP_BASE+0x0050)
#define V5_REG_IPP_CAPUV_S						(V5_REG_IPP_BASE+0x0054)
#define V5_REG_IPP_THUMBTGT_SIZE				(V5_REG_IPP_BASE+0x0058)
#define V5_REG_IPP_THUMBH_W					(V5_REG_IPP_BASE+0x005c)
#define V5_REG_IPP_THUMBH_S					(V5_REG_IPP_BASE+0x0060)
#define V5_REG_IPP_THUMBH_C					(V5_REG_IPP_BASE+0x0064)    
#define V5_REG_IPP_THUMBV_H					(V5_REG_IPP_BASE+0x0068)
#define V5_REG_IPP_THUMBV_S					(V5_REG_IPP_BASE+0x006c)
#define V5_REG_IPP_THUMBV_C					(V5_REG_IPP_BASE+0x0070)
#define V5_REG_IPP_THUMBNAIL_START_ADDR0	(V5_REG_IPP_BASE+0x0074)
#define V5_REG_IPP_THUMBNAIL_END_ADDR0		(V5_REG_IPP_BASE+0x0078)
#define V5_REG_IPP_THUMBNAIL_START_ADDR1	(V5_REG_IPP_BASE+0x007c)
#define V5_REG_IPP_THUMBNAIL_END_ADDR1		(V5_REG_IPP_BASE+0x0080)
#define V5_REG_IPP_UV_OFFSET					(V5_REG_IPP_BASE+0x0084)
#define V5_REG_IPP_UV_THRESHOLD				(V5_REG_IPP_BASE+0x0088)        
#define V5_REG_IPP_DISFRMDP					(V5_REG_IPP_BASE+0x008c)
#define V5_REG_IPP_CAPFRMDP					(V5_REG_IPP_BASE+0x0090)
#define V5_REG_IPP_HISTOGRAM_CTRL				(V5_REG_IPP_BASE+0x0094)
#define V5_REG_IPP_X_HISTOGRAM				(V5_REG_IPP_BASE+0x0098)
#define V5_REG_IPP_Q_HISTOGRAM				(V5_REG_IPP_BASE+0x009c)
#define V5_REG_IPP_HISTOGRAM_SUM_00_1F		(V5_REG_IPP_BASE+0x00a0)
#define V5_REG_IPP_HISTOGRAM_SUM_20_3F		(V5_REG_IPP_BASE+0x00a4)
#define V5_REG_IPP_HISTOGRAM_SUM_40_5F		(V5_REG_IPP_BASE+0x00a8)
#define V5_REG_IPP_HISTOGRAM_SUM_60_7F		(V5_REG_IPP_BASE+0x00ac)        
#define V5_REG_IPP_HISTOGRAM_SUM_80_9F		(V5_REG_IPP_BASE+0x00b0)
#define V5_REG_IPP_HISTOGRAM_SUM_A0_BF		(V5_REG_IPP_BASE+0x00b4)
#define V5_REG_IPP_HISTOGRAM_SUM_C0_DF		(V5_REG_IPP_BASE+0x00b8)
#define V5_REG_IPP_HISTOGRAM_SUM_E0_FF		(V5_REG_IPP_BASE+0x00bc)
#define V5_REG_IPP_UMCTRL_SEL					(V5_REG_IPP_BASE+0x00c0)
#define V5_REG_IPP_WINDOW_UPDATE				(V5_REG_IPP_BASE+0x00c4)
/***********************************************************************************/

/***********************************************************************************/
#ifdef __NEW_SYS__ // 820 MP chip macro, by youhai_mp4enc
#else
/*! ISP registers */
#define V5_REG_ISP_CTRL							(V5_REG_ISP_BASE+0x0000)
#define V5_REG_ISP_WIN_WIDTH					(V5_REG_ISP_BASE+0x0004)
#define V5_REG_ISP_WIN_HEIGHT					(V5_REG_ISP_BASE+0x0008)
#define V5_REG_ISP_IMG_WIDTH					(V5_REG_ISP_BASE+0x000c)
#define V5_REG_ISP_IMG_HEIGHT					(V5_REG_ISP_BASE+0x0010)
#define V5_REG_ISP_FEM_M						(V5_REG_ISP_BASE+0x0014)
#define V5_REG_ISP_FEM_X						(V5_REG_ISP_BASE+0x0018)
#define V5_REG_ISP_FEM_MIN						(V5_REG_ISP_BASE+0x001c)
#define V5_REG_ISP_FEM_MAX					(V5_REG_ISP_BASE+0x0020)
#define V5_REG_ISP_LF_CENTERX					(V5_REG_ISP_BASE+0x0024)
#define V5_REG_ISP_LF_CENTERY					(V5_REG_ISP_BASE+0x0028)
#define V5_REG_ISP_LF_RECPROC					(V5_REG_ISP_BASE+0x002c)
#define V5_REG_ISP_AFLNCTRL					(V5_REG_ISP_BASE+0x0030)
#define V5_REG_ISP_AFTHD						(V5_REG_ISP_BASE+0x0034)
#define V5_REG_ISP_AFLNSTEP					(V5_REG_ISP_BASE+0x0038)
#define V5_REG_ISP_TAILBLANK					(V5_REG_ISP_BASE+0x003c)
#define V5_REG_ISP_RGBGAIN						(V5_REG_ISP_BASE+0x0040)
#define V5_REG_ISP_VS_RST_DIS					(V5_REG_ISP_BASE+0x0044)
#define V5_REG_ISP_GLBGAIN						(V5_REG_ISP_BASE+0x0048)
#define V5_REG_ISP_GLBGAINCTRL					(V5_REG_ISP_BASE+0x004c)
#define V5_REG_ISP_CB_CR_ADJ					(V5_REG_ISP_BASE+0x0050)
#define V5_REG_ISP_CB_CR_OFS					(V5_REG_ISP_BASE+0x0054)
#define V5_REG_ISP_COLMAT1                 	 		(V5_REG_ISP_BASE+0x0058)
#define V5_REG_ISP_COLMAT2                  			(V5_REG_ISP_BASE+0x005c)
#define V5_REG_ISP_COLMAT3                  			(V5_REG_ISP_BASE+0x0060)
#define V5_REG_ISP_COLOFSR                  			(V5_REG_ISP_BASE+0x0064)
#define V5_REG_ISP_COLOFSG                  			(V5_REG_ISP_BASE+0x0068)
#define V5_REG_ISP_COLOFSB                  			(V5_REG_ISP_BASE+0x006c)
#define V5_REG_ISP_YGMAP0_3                 			(V5_REG_ISP_BASE+0x0070)
#define V5_REG_ISP_YGMAP4_7                 			(V5_REG_ISP_BASE+0x0074)
#define V5_REG_ISP_YGMAP8                   			(V5_REG_ISP_BASE+0x0078)
#define V5_REG_ISP_YGMAS0_3                 			(V5_REG_ISP_BASE+0x007c)
#define V5_REG_ISP_YGMAS4_7                 			(V5_REG_ISP_BASE+0x0080)
#define V5_REG_ISP_YGMAS8                   			(V5_REG_ISP_BASE+0x0084)
#define V5_REG_ISP_RGMAS0_3                 			(V5_REG_ISP_BASE+0x0088)
#define V5_REG_ISP_RGMAS4_7                 			(V5_REG_ISP_BASE+0x008c)
#define V5_REG_ISP_RGMAS8_11                			(V5_REG_ISP_BASE+0x0090)
#define V5_REG_ISP_RGMAS12_15               			(V5_REG_ISP_BASE+0x0094)
#define V5_REG_ISP_RGMAS16                  			(V5_REG_ISP_BASE+0x0098)
#define V5_REG_ISP_GGMAS0_3                 			(V5_REG_ISP_BASE+0x009c)
#define V5_REG_ISP_GGMAS4_7                 			(V5_REG_ISP_BASE+0x00a0)
#define V5_REG_ISP_GGMAS8_11                			(V5_REG_ISP_BASE+0x00a4)
#define V5_REG_ISP_GGMAS12_15               		(V5_REG_ISP_BASE+0x00a8)
#define V5_REG_ISP_GGMAS16                  			(V5_REG_ISP_BASE+0x00ac)
#define V5_REG_ISP_BGMAS0_3                 			(V5_REG_ISP_BASE+0x00b0)
#define V5_REG_ISP_BGMAS4_7                 			(V5_REG_ISP_BASE+0x00b4)
#define V5_REG_ISP_BGMAS8_11                			(V5_REG_ISP_BASE+0x00b8)
#define V5_REG_ISP_BGMAS12_15               			(V5_REG_ISP_BASE+0x00bc)
#define V5_REG_ISP_BGMAS16                  			(V5_REG_ISP_BASE+0x00c0)
#define V5_REG_ISP_AUTOCTRL					(V5_REG_ISP_BASE+0x00d0)
#define V5_REG_ISP_CTRL1						(V5_REG_ISP_BASE+0x00d4)
#define V5_REG_ISP_UPDATECTRL					(V5_REG_ISP_BASE+0x00d8)
#define V5_REG_ISP_YBOT                     			(V5_REG_ISP_BASE+0x00dc)
#define V5_REG_ISP_YTOP						(V5_REG_ISP_BASE+0x00e0)
#define V5_REG_ISP_QTOP						(V5_REG_ISP_BASE+0x00e4)
#define V5_REG_ISP_ITOP                     				(V5_REG_ISP_BASE+0x00e8)
#define V5_REG_ISP_GLOW                     			(V5_REG_ISP_BASE+0x00ec)
#define V5_REG_ISP_GTOP                     			(V5_REG_ISP_BASE+0x00f0)
#define V5_REG_ISP_AWBCTRL                  			(V5_REG_ISP_BASE+0x00f4)
#define V5_REG_ISP_RMEAN                    			(V5_REG_ISP_BASE+0x00f8)
#define V5_REG_ISP_GMEAN                    			(V5_REG_ISP_BASE+0x00fc)
#define V5_REG_ISP_BMEAN                    			(V5_REG_ISP_BASE+0x0100)
#define V5_REG_ISP_YMEAN                    			(V5_REG_ISP_BASE+0x0104)
#define V5_REG_ISP_AUTOSTATUS              			(V5_REG_ISP_BASE+0x0108)
#define V5_REG_ISP_DPDTHD                   			(V5_REG_ISP_BASE+0x011c)
#define V5_REG_ISP_NT0_3                    			(V5_REG_ISP_BASE+0x0120)
#define V5_REG_ISP_NT4_7                    			(V5_REG_ISP_BASE+0x0124)
#define V5_REG_ISP_NT8_11                   			(V5_REG_ISP_BASE+0x0128)
#define V5_REG_ISP_NT12_15                  			(V5_REG_ISP_BASE+0x012c)
#define V5_REG_ISP_NT16                     			(V5_REG_ISP_BASE+0x0130)
#define V5_REG_ISP_DPDTHD_UV                			(V5_REG_ISP_BASE+0x0134)
#define V5_REG_ISP_NT0_3_UV                 			(V5_REG_ISP_BASE+0x0138)
#define V5_REG_ISP_NT4_7_UV                 			(V5_REG_ISP_BASE+0x013c)
#define V5_REG_ISP_NT8_11_UV                			(V5_REG_ISP_BASE+0x0140)
#define V5_REG_ISP_NT12_15_UV               			(V5_REG_ISP_BASE+0x0144)
#define V5_REG_ISP_NT16_UV                  			(V5_REG_ISP_BASE+0x0148)
#define V5_REG_ISP_BL_R_OFFSET              			(V5_REG_ISP_BASE+0x014c)
#define V5_REG_ISP_BL_B_OFFSET              			(V5_REG_ISP_BASE+0x0150)
#define V5_REG_ISP_BL_G1_OFFSET            		(V5_REG_ISP_BASE+0x0154)
#define V5_REG_ISP_BL_G2_OFFSET             		(V5_REG_ISP_BASE+0x0158)
#define V5_REG_ISP_EDGE_SUM                 			(V5_REG_ISP_BASE+0x015c)
#define V5_REG_ISP_AEAWB_WIN_HPOS_0_3       	(V5_REG_ISP_BASE+0x0160)
#define V5_REG_ISP_AEAWB_WIN_HPOS_4         	(V5_REG_ISP_BASE+0x0164)
#define V5_REG_ISP_AEAWB_WIN_VPOS_0_3       	(V5_REG_ISP_BASE+0x0168)
#define V5_REG_ISP_AEAWB_WIN_VPOS_4         	(V5_REG_ISP_BASE+0x016c)
#define V5_REG_ISP_HIS_CTRL                 			(V5_REG_ISP_BASE+0x0170)
#define V5_REG_ISP_HIS_MANUAL_GAIN          		(V5_REG_ISP_BASE+0x0174)
#define V5_REG_ISP_Y_MEAN_TGT               		(V5_REG_ISP_BASE+0x0178)
#define V5_REG_ISP_Y_MEAN_THD               		(V5_REG_ISP_BASE+0x017c)
#define V5_REG_ISP_INTERRUPT                			(V5_REG_ISP_BASE+0x0180)
#define V5_REG_ISP_AE_WT_0_3                			(V5_REG_ISP_BASE+0x0184)
#define V5_REG_ISP_AE_WT_4_7                			(V5_REG_ISP_BASE+0x0188)
#define V5_REG_ISP_WIN_YMEAN_1              		(V5_REG_ISP_BASE+0x018c)
#define V5_REG_ISP_WIN_YMEAN_2              		(V5_REG_ISP_BASE+0x0190)
#define V5_REG_ISP_WIN_YMEAN_3              		(V5_REG_ISP_BASE+0x0194)
#define V5_REG_ISP_WIN_YMEAN_4              		(V5_REG_ISP_BASE+0x0198)
#define V5_REG_ISP_HUE_EN                   			(V5_REG_ISP_BASE+0x019c)
#define V5_REG_ISP_HUE_COS                  			(V5_REG_ISP_BASE+0x01a0)
#define V5_REG_ISP_HUE_SIN                  			(V5_REG_ISP_BASE+0x01a4)
#define V5_REG_ISP_SATURATION               			(V5_REG_ISP_BASE+0x01a8)
#define V5_REG_ISP_HUE_YOFF                 			(V5_REG_ISP_BASE+0x01ac)
#define V5_REG_ISP_HUE_UOFF                 			(V5_REG_ISP_BASE+0x01b0)
#define V5_REG_ISP_HUE_VOFF                 			(V5_REG_ISP_BASE+0x01b4)
#define V5_REG_ISP_RESET						(V5_REG_ISP_BASE+0x01b8)
#endif
/***********************************************************************************/

/***********************************************************************************/

/*! LCDC registers */
#define V5_REG_LCDC_CTRL						(V5_REG_LCDC_BASE+0x0000)
#define V5_REG_LCDC_RESET						(V5_REG_LCDC_BASE+0x0004)
#define V5_REG_LCDC_UPDATE					(V5_REG_LCDC_BASE+0x0008)
#define V5_REG_LCDC_VBBA0						(V5_REG_LCDC_BASE+0x000c)
#define V5_REG_LCDC_VBBA1						(V5_REG_LCDC_BASE+0x0010)
#define V5_REG_LCDC_VBBA2						(V5_REG_LCDC_BASE+0x0014)
#define V5_REG_LCDC_VBBA3						(V5_REG_LCDC_BASE+0x0018)
#define V5_REG_LCDC_GBBA0						(V5_REG_LCDC_BASE+0x001c)
#define V5_REG_LCDC_GBBA1						(V5_REG_LCDC_BASE+0x0020)
#define V5_REG_LCDC_A0_LCD_POS				(V5_REG_LCDC_BASE+0x0024)
#define V5_REG_LCDC_A0_SIZE_H					(V5_REG_LCDC_BASE+0x0028)
#define V5_REG_LCDC_A0_SIZE_W					(V5_REG_LCDC_BASE+0x0028+2)

#define V5_REG_LCDC_A0_VFRAME_POS			(V5_REG_LCDC_BASE+0x002c)
#define V5_REG_LCDC_A1_LCD_POS				(V5_REG_LCDC_BASE+0x0034)
#define V5_REG_LCDC_A1_SIZE_H					(V5_REG_LCDC_BASE+0x0038)
#define V5_REG_LCDC_A1_SIZE_W					(V5_REG_LCDC_BASE+0x0038+2)

#define V5_REG_LCDC_B0_LCD_POS				(V5_REG_LCDC_BASE+0x003c)
#define V5_REG_LCDC_B0_SIZE					(V5_REG_LCDC_BASE+0x0040)
#define V5_REG_LCDC_B0_UPD_POS				(V5_REG_LCDC_BASE+0x0044)
#define V5_REG_LCDC_B0_UPD_SIZE				(V5_REG_LCDC_BASE+0x0048)
#define V5_REG_LCDC_B0_GBUF0_POS				(V5_REG_LCDC_BASE+0x004c)
#define V5_REG_LCDC_B1_LCD_POS				(V5_REG_LCDC_BASE+0x0050)
#define V5_REG_LCDC_B1_SIZE					(V5_REG_LCDC_BASE+0x0054)
#define V5_REG_LCDC_B1_UPD_POS				(V5_REG_LCDC_BASE+0x0058)
#define V5_REG_LCDC_B1_UPD_SIZE				(V5_REG_LCDC_BASE+0x005c)
#define V5_REG_LCDC_B1_GBUF1_POS				(V5_REG_LCDC_BASE+0x0060)
#define V5_REG_LCDC_GBUF_WIDTH				(V5_REG_LCDC_BASE+0x0064)
#define V5_REG_LCDC_BG_UPD_POS				(V5_REG_LCDC_BASE+0x0068)
#define V5_REG_LCDC_BG_UPD_SIZE				(V5_REG_LCDC_BASE+0x006c)
#define V5_REG_LCDC_BGD						(V5_REG_LCDC_BASE+0x0070)
#define V5_REG_LCDC_PALETTE					(V5_REG_LCDC_BASE+0x0074) 
#define V5_REG_LCDC_GLUT_R0123				(V5_REG_LCDC_BASE+0x0078)
#define V5_REG_LCDC_GLUT_R4567				(V5_REG_LCDC_BASE+0x007c)
#define V5_REG_LCDC_GLUT_R891011				(V5_REG_LCDC_BASE+0x0080)
#define V5_REG_LCDC_GLUT_R12131415			(V5_REG_LCDC_BASE+0x0084)
#define V5_REG_LCDC_GLUT_G0123				(V5_REG_LCDC_BASE+0x0088)
#define V5_REG_LCDC_GLUT_G4567				(V5_REG_LCDC_BASE+0x008c)
#define V5_REG_LCDC_GLUT_G891011				(V5_REG_LCDC_BASE+0x0090)
#define V5_REG_LCDC_GLUT_G12131415			(V5_REG_LCDC_BASE+0x0094)
#define V5_REG_LCDC_GLUT_B0123				(V5_REG_LCDC_BASE+0x0098)
#define V5_REG_LCDC_GLUT_B4567				(V5_REG_LCDC_BASE+0x009c)
#define V5_REG_LCDC_GLUT_B891011				(V5_REG_LCDC_BASE+0x00a0)
#define V5_REG_LCDC_GLUT_B12131415			(V5_REG_LCDC_BASE+0x00a4)
#define V5_REG_LCDC_GLUT_R16G16B16			(V5_REG_LCDC_BASE+0x00a8)
#define V5_REG_LCDC_DEDLY						(V5_REG_LCDC_BASE+0x00ac)
#define V5_REG_LCDC_OKC_B0						(V5_REG_LCDC_BASE+0x00b0)
#define V5_REG_LCDC_OKC_B1						(V5_REG_LCDC_BASE+0x00b4)
#define V5_REG_LCDC_BLENDRATIO_B				(V5_REG_LCDC_BASE+0x00b8)
/***********************************************************************************/

/***********************************************************************************/
/*! LCDIF registers */
#define V5_REG_LCDIF_CFG						(V5_REG_LCDIF_BASE+0x000)
#define V5_REG_LCDIF_LCD_RESET					(V5_REG_LCDIF_BASE+0x004)
#define V5_REG_LCDIF_DBI_TIM					(V5_REG_LCDIF_BASE+0x008)
#define V5_REG_LCDIF_DPI_HTIM0					(V5_REG_LCDIF_BASE+0x00c)
#define V5_REG_LCDIF_DPI_HTIM1					(V5_REG_LCDIF_BASE+0x010)
#define V5_REG_LCDIF_DPI_VTIM0					(V5_REG_LCDIF_BASE+0x014)
#define V5_REG_LCDIF_DPI_VTIM1					(V5_REG_LCDIF_BASE+0x018)
#define V5_REG_LCDIF_DAT_FMT					(V5_REG_LCDIF_BASE+0x01c)
#define V5_REG_LCDIF_EXRW_COMM_DATA			(V5_REG_LCDIF_BASE+0x020)
#define V5_REG_LCDIF_EXRW_EN					(V5_REG_LCDIF_BASE+0x024)
#define V5_REG_LCDIF_EXR_DATA					(V5_REG_LCDIF_BASE+0x028)
#define V5_REG_LCDIF_HEAD_CONFIG				(V5_REG_LCDIF_BASE+0x02c)
#define V5_REG_LCDIF_HEAD_RS_FLAG				(V5_REG_LCDIF_BASE+0x030)
#define V5_REG_LCDIF_HEAD_CS_FLAG				(V5_REG_LCDIF_BASE+0x034)
#define V5_REG_LCDIF_START_POS				(V5_REG_LCDIF_BASE+0x038)
#define V5_REG_LCDIF_END_POS					(V5_REG_LCDIF_BASE+0x03c)
#define V5_REG_LCDIF_BIAS						(V5_REG_LCDIF_BASE+0x040)
#define V5_REG_LCDIF_HEAD_PROCESS_FLAG		(V5_REG_LCDIF_BASE+0x044)
#define V5_REG_LCDIF_START_COMM_CODE			(V5_REG_LCDIF_BASE+0x048)
#define V5_REG_LCDIF_END_COMM_CODE			(V5_REG_LCDIF_BASE+0x04c)
#define V5_REG_LCDIF_REPEAT_START_POS		(V5_REG_LCDIF_BASE+0x050)
#define V5_REG_LCDIF_REPEAT_END_POS			(V5_REG_LCDIF_BASE+0x054)
#define V5_REG_LCDIF_HEAD_REPEAT_RS_FLAG	(V5_REG_LCDIF_BASE+0x058)
#define V5_REG_LCDIF_HEAD_REPEAT_CS_FLAG	(V5_REG_LCDIF_BASE+0x05c)
#define V5_REG_LCDIF_HEAD_DATA01				(V5_REG_LCDIF_BASE+0x060)
#define V5_REG_LCDIF_HEAD_DATA23				(V5_REG_LCDIF_BASE+0x064)
#define V5_REG_LCDIF_HEAD_DATA45				(V5_REG_LCDIF_BASE+0x068)
#define V5_REG_LCDIF_HEAD_DATA67				(V5_REG_LCDIF_BASE+0x06c)
#define V5_REG_LCDIF_HEAD_DATA89				(V5_REG_LCDIF_BASE+0x070)
#define V5_REG_LCDIF_HEAD_DATA1011			(V5_REG_LCDIF_BASE+0x074)
#define V5_REG_LCDIF_HEAD_DATA1213			(V5_REG_LCDIF_BASE+0x078)
#define V5_REG_LCDIF_HEAD_DATA1415			(V5_REG_LCDIF_BASE+0x07c)
#define V5_REG_LCDIF_HEAD_DATA1617			(V5_REG_LCDIF_BASE+0x080)
#define V5_REG_LCDIF_HEAD_DATA1819			(V5_REG_LCDIF_BASE+0x084)
#define V5_REG_LCDIF_HEAD_DATA2021			(V5_REG_LCDIF_BASE+0x088)
#define V5_REG_LCDIF_HEAD_DATA2223			(V5_REG_LCDIF_BASE+0x08c)
#define V5_REG_LCDIF_HEAD_DATA2425			(V5_REG_LCDIF_BASE+0x090)
#define V5_REG_LCDIF_HEAD_DATA2627			(V5_REG_LCDIF_BASE+0x094)
#define V5_REG_LCDIF_HEAD_DATA2829			(V5_REG_LCDIF_BASE+0x098)
#define V5_REG_LCDIF_HEAD_DATA3031			(V5_REG_LCDIF_BASE+0x09c)
#define V5_REG_LCDIF_SERIAL_SEND				(V5_REG_LCDIF_BASE+0x0a0)
#define V5_REG_LCDIF_SERIAL_COMM_DATA		(V5_REG_LCDIF_BASE+0x0ac)

/***********************************************************************************/

/***********************************************************************************/
/*! SIF registers */


#ifdef __NEW_SYS__ // 820 MP chip macro, by lizg
//Register SIF
#define	V5_REG_SIF_SNR_MODE						(V5_REG_SIF_BASE+0x000)
#define	V5_REG_SIF_SNROP_MODE					(V5_REG_SIF_BASE+0x004)
#define	V5_REG_SIF_YUV_FORMAT					(V5_REG_SIF_BASE+0x008)
#define	V5_REG_SIF_VH_CONTROL					(V5_REG_SIF_BASE+0x00C)
#define	V5_REG_SIF_SNRCLK_CNT					(V5_REG_SIF_BASE+0x010)
#define	V5_REG_SIF_PIXRATE_SEL				(V5_REG_SIF_BASE+0x011)
#define	V5_REG_SIF_SICLK_CNT					(V5_REG_SIF_BASE+0x014)
#define	V5_REG_SIF_I2C_CLKCNT					(V5_REG_SIF_BASE+0x018)
#define	V5_REG_SIF_HD_START_STOP				(V5_REG_SIF_BASE+0x01C)
#define	V5_REG_SIF_HSYNC_START					(V5_REG_SIF_BASE+0x020)
#define	V5_REG_SIF_HREF_START			(V5_REG_SIF_BASE+0x024)
#define	V5_REG_SIF_HREF_LENGTH				(V5_REG_SIF_BASE+0x026)
#define	V5_REG_SIF_VD_START_STOP				(V5_REG_SIF_BASE+0x028)
#define	V5_REG_SIF_VSYNC_START_STOP				(V5_REG_SIF_BASE+0x02C)
#define	V5_REG_SIF_VREF_START				(V5_REG_SIF_BASE+0x030)
#define	V5_REG_SIF_VREF_STOP						(V5_REG_SIF_BASE+0x032)
#define	V5_REG_SIF_HDFALL_START_STOP			(V5_REG_SIF_BASE+0x034)
#define	V5_REG_SIF_VDFALL_START_STOP			(V5_REG_SIF_BASE+0x038)
#define	V5_REG_SIF_COLMAX_ROWMAX				(V5_REG_SIF_BASE+0x03C)
#define	V5_REG_SIF_EXTIME						(V5_REG_SIF_BASE+0x040)
#define	V5_REG_SIF_OBREG						(V5_REG_SIF_BASE+0x044)
#define	V5_REG_SIF_IIC_CFG						(V5_REG_SIF_BASE+0x048)
#define	V5_REG_SIF_SNRWRDATA					(V5_REG_SIF_BASE+0x04C)
#define	V5_REG_SIF_SNRACSCTR					(V5_REG_SIF_BASE+0x050)
#define	V5_REG_SIF_SNRADDR						(V5_REG_SIF_BASE+0x054)
#define	V5_REG_SIF_BUS_STATE					(V5_REG_SIF_BASE+0x058)
#define	V5_REG_SIF_SNRRDDATA					(V5_REG_SIF_BASE+0x05C)
#define	V5_REG_SIF_AEWIN_START_STOP			(V5_REG_SIF_BASE+0x060)
#define	V5_REG_SIF_AEPIN_CTRL					(V5_REG_SIF_BASE+0x064)
#define	V5_REG_SIF_AEBUS_CTRL					(V5_REG_SIF_BASE+0x068)
#define	V5_REG_SIF_B_CNT						(V5_REG_SIF_BASE+0x06C)
#define	V5_REG_SIF_AE0_DATA_0_3					(V5_REG_SIF_BASE+0x070)
#define	V5_REG_SIF_AE0_DATA_4_7					(V5_REG_SIF_BASE+0x074)
#define	V5_REG_SIF_AE0_DATA_8_11				(V5_REG_SIF_BASE+0x078)
#define	V5_REG_SIF_AE0_DATA_12_15				(V5_REG_SIF_BASE+0x07C)
#define	V5_REG_SIF_AE0_DATA_16_19				(V5_REG_SIF_BASE+0x080)
#define	V5_REG_SIF_EDGE_CHOS					(V5_REG_SIF_BASE+0x084)
#define	V5_REG_SIF_COUNTER_RESET				(V5_REG_SIF_BASE+0x088)
#define	V5_REG_SIF_CAPTURE_DELAY				(V5_REG_SIF_BASE+0x08C)
#define	V5_REG_SIF_FL_PWM_REG					(V5_REG_SIF_BASE+0x090)
#define	V5_REG_SIF_FL_PWM_CNT_TGT				(V5_REG_SIF_BASE+0x094)
#define	V5_REG_SIF_FL_DUTY						(V5_REG_SIF_BASE+0x098)
#define	V5_REG_SIF_M10US_REG					(V5_REG_SIF_BASE+0x09C)
#define	V5_REG_SIF_DELAY_FLASH_TGT				(V5_REG_SIF_BASE+0x0A0)
#define	V5_REG_SIF_TRIGGER_TGT					(V5_REG_SIF_BASE+0x0A4)
#define	V5_REG_SIF_TRIGGER_STYLE_SEL			(V5_REG_SIF_BASE+0x0A8)
#define	V5_REG_SIF_START_TRIGGER				(V5_REG_SIF_BASE+0x0AC)
#define	V5_REG_SIF_TRIGGER_START_ROW_COL		(V5_REG_SIF_BASE+0x0B0)
#define	V5_REG_SIF_UMCTRL_SEL					(V5_REG_SIF_BASE+0x0B4)
#define	V5_REG_SIF_FIFO_STATUS					(V5_REG_SIF_BASE+0x0B8)
#define	V5_REG_SIF_VSYNC_DELAY					(V5_REG_SIF_BASE+0x0BC)
#define	V5_REG_SIF_FIFO_DLY						(V5_REG_SIF_BASE+0x0C0)
#define	V5_REG_SIF_HSYNC_PLUS					(V5_REG_SIF_BASE+0x0C4)
#define	V5_REG_SIF_SNR_ADDR_LH					(V5_REG_SIF_BASE+0x0C8)
#define	V5_REG_SIF_IIC_SADDR_BYTE				(V5_REG_SIF_BASE+0x0CC)
#define	V5_REG_SIF_IIC_DADDRTEN_FLAG			(V5_REG_SIF_BASE+0x0D0)

///ADD REGISTER BASSED ON VC0336
#define	V5_REG_SIF_WIN						(V5_REG_SIF_BASE+0x0D4)
#define	V5_REG_SIF_IMG						(V5_REG_SIF_BASE+0x0D8)
#define	V5_REG_SIF_RGB_TEST					(V5_REG_SIF_BASE+0x0DC)
#define	V5_REG_SIF_TEST_BAR_CTR				(V5_REG_SIF_BASE+0x0E0)
#define	V5_REG_SIF_SMIA_CTR					(V5_REG_SIF_BASE+0x0E4)
#define	V5_REG_SIF_SM_TRAN_CTL				(V5_REG_SIF_BASE+0x0E8)
#define	V5_REG_SIF_PATT_ERR_DATA			(V5_REG_SIF_BASE+0x0EC)
#define	V5_REG_SIF_PATT_ERR_DATA0_3			(V5_REG_SIF_BASE+0x0F0)
#define	V5_REG_SIF_PATT_ERR_DATA4_7			(V5_REG_SIF_BASE+0x0F4)
#define	V5_REG_SIF_PATT_ERR_DATA8_11		(V5_REG_SIF_BASE+0x0F8)
#define	V5_REG_SIF_PATT_ERR_DATA12_15		(V5_REG_SIF_BASE+0x0FC)
#define	V5_REG_SIF_PATT_ERR_DATA16_19		(V5_REG_SIF_BASE+0x100)
#define	V5_REG_SIF_INT						(V5_REG_SIF_BASE+0x104)
#define	V5_REG_SIF_FRAME_DROP				(V5_REG_SIF_BASE+0x108)
#define	V5_REG_SIF_PATT_ERR_FLAG			(V5_REG_SIF_BASE+0x10C)
#define	V5_REG_SIF_STREAM_STATUS			(V5_REG_SIF_BASE+0x110)

#else

#define	V5_REG_SIF_SNR_MODE					(V5_REG_SIF_BASE+0x000)
#define	V5_REG_SIF_SNROP_MODE				(V5_REG_SIF_BASE+0x004)
#define	V5_REG_SIF_YUV_FORMAT				(V5_REG_SIF_BASE+0x008)
#define	V5_REG_SIF_V_CONTROL				(V5_REG_SIF_BASE+0x00c)
#define	V5_REG_SIF_H_CONTROL				(V5_REG_SIF_BASE+0x010)
#define	V5_REG_SIF_SNRCLK_CNT				(V5_REG_SIF_BASE+0x014)
#define	V5_REG_SIF_PIXRATE_SEL				(V5_REG_SIF_BASE+0x018)
#define	V5_REG_SIF_SICLK_CNT				(V5_REG_SIF_BASE+0x01c)
#define	V5_REG_SIF_I2C_CLKCNT				(V5_REG_SIF_BASE+0x020)
#define	V5_REG_SIF_I2C_INTER_TRANS			(V5_REG_SIF_BASE+0x028)
#define	V5_REG_SIF_HD_START					(V5_REG_SIF_BASE+0x030)
#define	V5_REG_SIF_HD_STOP					(V5_REG_SIF_BASE+0x038)
#define	V5_REG_SIF_HSYNC_START				(V5_REG_SIF_BASE+0x040)
#define	V5_REG_SIF_HREF_START				(V5_REG_SIF_BASE+0x050)
#define	V5_REG_SIF_HREF_LENGTH				(V5_REG_SIF_BASE+0x058)
#define	V5_REG_SIF_VD_START					(V5_REG_SIF_BASE+0x060)
#define	V5_REG_SIF_VD_STOP					(V5_REG_SIF_BASE+0x068)
#define	V5_REG_SIF_VSYNC_START				(V5_REG_SIF_BASE+0x070)
#define	V5_REG_SIF_VSYNC_STOP				(V5_REG_SIF_BASE+0x078)
#define	V5_REG_SIF_VREF_START				(V5_REG_SIF_BASE+0x080)
#define	V5_REG_SIF_VREF_STOP				(V5_REG_SIF_BASE+0x088)
#define	V5_REG_SIF_HDFALL_START				(V5_REG_SIF_BASE+0x090)
#define	V5_REG_SIF_HDFALL_STOP				(V5_REG_SIF_BASE+0x098)
#define	V5_REG_SIF_VDFALL_START				(V5_REG_SIF_BASE+0x0a0)
#define	V5_REG_SIF_VDFALL_STOP				(V5_REG_SIF_BASE+0x0a8)
#define	V5_REG_SIF_COLMAX					(V5_REG_SIF_BASE+0x0b0)
#define	V5_REG_SIF_ROWMAX					(V5_REG_SIF_BASE+0x0b8)
#define	V5_REG_SIF_EXTIME					(V5_REG_SIF_BASE+0x0c0)
#define	V5_REG_SIF_OBREG					(V5_REG_SIF_BASE+0x0cc)
#define	V5_REG_SIF_IIC_BYTE					(V5_REG_SIF_BASE+0x0d0)
#define	V5_REG_SIF_IIC_DEVADDR				(V5_REG_SIF_BASE+0x0d4)
#define	V5_REG_SIF_SNRWRDATA				(V5_REG_SIF_BASE+0x0d8)
#define	V5_REG_SIF_SNRACSCTR				(V5_REG_SIF_BASE+0x0e4)
#define	V5_REG_SIF_SNRADDR					(V5_REG_SIF_BASE+0x0e8)
#define	V5_REG_SIF_SNRSTATUS				(V5_REG_SIF_BASE+0x0ec)
#define	V5_REG_SIF_SNRRDDATA				(V5_REG_SIF_BASE+0x0f0)
#define	V5_REG_SIF_BUS_STATE				(V5_REG_SIF_BASE+0x0fc)
#define	V5_REG_SIF_AEWIN_START				(V5_REG_SIF_BASE+0x100)
#define	V5_REG_SIF_AEWIN_STOP				(V5_REG_SIF_BASE+0x108)
#define	V5_REG_SIF_AEPIN_CTRL				(V5_REG_SIF_BASE+0x110)
#define	V5_REG_SIF_AEBUS_CTRL				(V5_REG_SIF_BASE+0x114)
#define	V5_REG_SIF_B_CNT					(V5_REG_SIF_BASE+0x118)
#define	V5_REG_SIF_AE0_DATA_0_3				(V5_REG_SIF_BASE+0x120)
#define	V5_REG_SIF_AE0_DATA_4_7				(V5_REG_SIF_BASE+0x130)
#define	V5_REG_SIF_AE0_DATA_8_11			(V5_REG_SIF_BASE+0x140)
#define	V5_REG_SIF_AE0_DATA_12_15			(V5_REG_SIF_BASE+0x150)
#define	V5_REG_SIF_AE0_DATA_16_19			(V5_REG_SIF_BASE+0x160)
#define	V5_REG_SIF_EDGE_CHOS				(V5_REG_SIF_BASE+0x170)
#define	V5_REG_SIF_COUNTER_RESET			(V5_REG_SIF_BASE+0x174)
#define	V5_REG_SIF_CAPTURE_DELAY			(V5_REG_SIF_BASE+0x178)
#define	V5_REG_SIF_FL_PWM_EN				(V5_REG_SIF_BASE+0x1c0)
#define	V5_REG_SIF_FL_POLARITY				(V5_REG_SIF_BASE+0x1c4)
#define	V5_REG_SIF_FL_PWM_CNT_TGT			(V5_REG_SIF_BASE+0x1c8)
#define	V5_REG_SIF_FL_PWM_CNT_DUTY			(V5_REG_SIF_BASE+0x1d0)
#define	V5_REG_SIF_FL_DUTY					(V5_REG_SIF_BASE+0x1d8)
#define	V5_REG_SIF_M10US_CNT_EN				(V5_REG_SIF_BASE+0x200)
#define	V5_REG_SIF_M10US_TGT				(V5_REG_SIF_BASE+0x204)
#define	V5_REG_SIF_DELAY_FLASH_TGT			(V5_REG_SIF_BASE+0x20c)
#define	V5_REG_SIF_TRIGGER_TGT				(V5_REG_SIF_BASE+0x218)
#define	V5_REG_SIF_TRIGGER_STYLE_SEL		(V5_REG_SIF_BASE+0x220)
#define	V5_REG_SIF_START_TRIGGER			(V5_REG_SIF_BASE+0x224)
#define	V5_REG_SIF_TRIGGER_START_ROW		(V5_REG_SIF_BASE+0x228)
#define	V5_REG_SIF_TRIGGER_START_COL		(V5_REG_SIF_BASE+0x230)
#define	V5_REG_SIF_UMCTRL_SEL				(V5_REG_SIF_BASE+0x238)
#define	V5_REG_SIF_FIFO_STATUS				(V5_REG_SIF_BASE+0x23c)
#define	V5_REG_SIF_VSYNC_DELAY				(V5_REG_SIF_BASE+0x240)
#define	V5_REG_SIF_FIFO_DLY					(V5_REG_SIF_BASE+0x248)
#define	V5_REG_SIF_HSYNC_PLUS				(V5_REG_SIF_BASE+0x24c)
#define	V5_REG_SIF_IIC_SADDR_BYTE			(V5_REG_SIF_BASE+0x280)
#define	V5_REG_SIF_SNRADDR_L				(V5_REG_SIF_BASE+0x288)
#define	V5_REG_SIF_DADDR_TEN				(V5_REG_SIF_BASE+0x290)
#define	V5_REG_SIF_IIC_DADDRTEN_FLAG		(V5_REG_SIF_BASE+0x298)
#define	V5_REG_SIF_INPUT_SEL				(V5_REG_SIF_BASE+0x2a0)

#endif

/***********************************************************************************/

/***********************************************************************************/
/*! LBUF registers */
#define V5_REG_LBUF_STATUS					(V5_REG_LBUF_BASE+0x000)
#define V5_REG_LBUF_MODE					(V5_REG_LBUF_BASE+0x004)
#define V5_REG_LBUF_CNT_RESET				(V5_REG_LBUF_BASE+0x008)
#define V5_REG_LBUF_IMAGESIZE				(V5_REG_LBUF_BASE+0x00c)
#define V5_REG_LBUF_HBLANK					(V5_REG_LBUF_BASE+0x010)
#define V5_REG_LBUF_VBLANK					(V5_REG_LBUF_BASE+0x014)
#define V5_REG_LBUF_Y_INI0_ENC				(V5_REG_LBUF_BASE+0x01C)
#define V5_REG_LBUF_CB_INI0_ENC				(V5_REG_LBUF_BASE+0x020)
#define V5_REG_LBUF_CR_INI0_ENC				(V5_REG_LBUF_BASE+0x024)
#define V5_REG_LBUF_Y_INI1_ENC				(V5_REG_LBUF_BASE+0x028)
#define V5_REG_LBUF_CB_INI1_ENC				(V5_REG_LBUF_BASE+0x02C)
#define V5_REG_LBUF_CR_INI1_ENC				(V5_REG_LBUF_BASE+0x030)
#define V5_REG_LBUF_Y_INI0_DEC				(V5_REG_LBUF_BASE+0x034)
#define V5_REG_LBUF_CB_INI0_DEC				(V5_REG_LBUF_BASE+0x038)
#define V5_REG_LBUF_CR_INI0_DEC				(V5_REG_LBUF_BASE+0x03C)
#define V5_REG_LBUF_Y_INI1_DEC				(V5_REG_LBUF_BASE+0x040)
#define V5_REG_LBUF_CB_INI1_DEC				(V5_REG_LBUF_BASE+0x044)
#define V5_REG_LBUF_CR_INI1_DEC				(V5_REG_LBUF_BASE+0x048)
#define V5_REG_LBUF_BURSTNUMBER0			(V5_REG_LBUF_BASE+0x04c)
#define V5_REG_LBUF_BURSTNUMBER12			(V5_REG_LBUF_BASE+0x050)
#define	V5_REG_LBUF_VSYNCDELAY				(V5_REG_LBUF_BASE+0x054)
#define	V5_REG_LBUF_MP4DEC_IMAGESIZE		(V5_REG_LBUF_BASE+0x058)
#define	V5_REG_LBUF_BUF1_LINENUM			(V5_REG_LBUF_BASE+0x05c)
/***********************************************************************************/

/***********************************************************************************/
/*! JPEG registers */
#define V5_REG_JPEG_MODE					(V5_REG_JPEG_BASE+0x0000)
#define V5_REG_JPEG_CTRL					(V5_REG_JPEG_BASE+0x0004)
#define V5_REG_JPEG_STATUS					(V5_REG_JPEG_BASE+0x0008)
#define V5_REG_JPEG_BRC                     (V5_REG_JPEG_BASE+0x000c)
#define V5_REG_JPEG_TCR						(V5_REG_JPEG_BASE+0x0010)
#define V5_REG_JPEG_QFACTOR					(V5_REG_JPEG_BASE+0x0014)
#define V5_REG_JPEG_HEADER_STEP             (V5_REG_JPEG_BASE+0x0018)
#define V5_REG_JPEG_RESERVED				(V5_REG_JPEG_BASE+0x001c)
#define V5_REG_JPEG_TARGET_WC				(V5_REG_JPEG_BASE+0x0020)
#define V5_REG_JPEG_VIDEO_WC				(V5_REG_JPEG_BASE+0x0024)
#define V5_REG_JPEG_HUFFMAN_WC				(V5_REG_JPEG_BASE+0x0028)
#define V5_REG_JPEG_IMAGEWIDTH				(V5_REG_JPEG_BASE+0x002c)
#define V5_REG_JPEG_IMAGEHEIGHT				(V5_REG_JPEG_BASE+0x0030)
#define V5_REG_JPEG_BRCPARAM				(V5_REG_JPEG_BASE+0x0034)
#define V5_REG_JPEG_FRAMECOUNT				(V5_REG_JPEG_BASE+0x0038)
#define V5_REG_JPEG_DCT_DATA				(V5_REG_JPEG_BASE+0x003C)
#define V5_REG_JPEG_QUANTATIONTABLE			(V5_REG_JPEG_BASE+0x0040)
#define V5_REG_JPEG_HUFFMANTABLE			(V5_REG_JPEG_BASE+0x0044)
#define V5_REG_JPEG_COMP_QUANT          	(V5_REG_JPEG_BASE+0x0048)
#define V5_REG_JPEG_COMP_01					(V5_REG_JPEG_BASE+0x004c)
#define V5_REG_JPEG_COMP_23					(V5_REG_JPEG_BASE+0x0050)
#define V5_REG_JPEG_BLOCKNUMBER				(V5_REG_JPEG_BASE+0x0054)
#define V5_REG_JPEG_IMGPARAM_0_3			(V5_REG_JPEG_BASE+0x0058)
#define V5_REG_JPEG_IMGPARAM_4_7			(V5_REG_JPEG_BASE+0x005c)
#define V5_REG_JPEG_IMGPARAM_8_11			(V5_REG_JPEG_BASE+0x0060)
#define V5_REG_JPEG_IMGPARAM_12_14			(V5_REG_JPEG_BASE+0x0064)
#define V5_REG_JPEG_HEAD_SEL				(V5_REG_JPEG_BASE+0x0068)
#define V5_REG_JPEG_JPEG_WC					(V5_REG_JPEG_BASE+0x006c)
#define V5_REG_JPEG_HEADER_LENGTH			(V5_REG_JPEG_BASE+0x0070)
#define V5_REG_JPEG_FEEDTHROUGH_HEAD		(V5_REG_JPEG_BASE+0x0074)
#define V5_REG_JPEG_PIXEL_COUNT				(V5_REG_JPEG_BASE+0x0078)
#define V5_REG_JPEG_BLOCKCOUNT				(V5_REG_JPEG_BASE+0x007c)
#define V5_REG_JPEG_DEC_BYPASS				(V5_REG_JPEG_BASE+0x0080)
#define V5_REG_JPEG_ENC_ERR_EN				(V5_REG_JPEG_BASE+0x0084)
#define V5_REG_JPEG_HBLANK					(V5_REG_JPEG_BASE+0x0088)
#define V5_REG_JPEG_ENH_THD					(V5_REG_JPEG_BASE+0x008c)
#define V5_REG_JPEG_ENH_SHF_NBIT			(V5_REG_JPEG_BASE+0x0090)
#define V5_REG_JPEG_QF_LEVEL0_3_COMP		(V5_REG_JPEG_BASE+0x0094)
#define V5_REG_JPEG_QF_LEVEL4_5_COMP		(V5_REG_JPEG_BASE+0x0098)
#define V5_REG_JPEG_RUDE_BRC				(V5_REG_JPEG_BASE+0x009c)
#define V5_REG_JPEG_RUDE_LEVEL				(V5_REG_JPEG_BASE+0x00a0)
#define V5_REG_JPEG_RUDE_THD_WC				(V5_REG_JPEG_BASE+0x00a4)
#define V5_REG_JPEG_TIMERINTERVAL			(V5_REG_JPEG_BASE+0x00a8)
#define V5_REG_JPEG_STILL_HEADLENGTH		(V5_REG_JPEG_BASE+0x00ac)
#define V5_REG_JPEG_QF_STILL				(V5_REG_JPEG_BASE+0x00b0)
#define V5_REG_JPEG_H_SIZE					(V5_REG_JPEG_BASE+0x00c0)
#define V5_REG_JPEG_MARB_STARTADDR			(V5_REG_JPEG_BASE+0x00c4)
/***********************************************************************************/

/***********************************************************************************/
/*! PAD registers */
#define	V5_REG_STRAP_PIN                          (V5_REG_PADC_BASE+0x700)
#ifdef __NEW_SYS__ // added by youhai
#define	V5_REG_GPIO_A_INTE                                   (V5_REG_PADC_BASE+0x00)
#define	V5_REG_GPIO_A_INT_NW                               (V5_REG_PADC_BASE+0x04)    
#define	V5_REG_GPIO_A_INT_WIDTH                             (V5_REG_PADC_BASE+0x08) 
#define	V5_REG_GPIO_A_CFG                    (V5_REG_PADC_BASE+0x0c)
#define	V5_REG_GPIO_A_I                            (V5_REG_PADC_BASE+0x10)
#define	V5_REG_GPIO_A_PE                        (V5_REG_PADC_BASE+0x14)
#define	V5_REG_GPIO_A_DIR                        (V5_REG_PADC_BASE+0x18)
#define	V5_REG_GPIO_A_ODE                       (V5_REG_PADC_BASE+0x1c)
#define	V5_REG_GPIO_A_PORT                     (V5_REG_PADC_BASE+0x20)
#define	V5_REG_GPIO_B_CFG                          (V5_REG_PADC_BASE+0x10c)
#define	V5_REG_GPIO_B_I                                 (V5_REG_PADC_BASE+0x110)
#define	V5_REG_GPIO_B_PE                              (V5_REG_PADC_BASE+0x114)
#define	V5_REG_GPIO_B_DIR                           (V5_REG_PADC_BASE+0x118)
#define	V5_REG_GPIO_B_ODE                        (V5_REG_PADC_BASE+0x11c)
#define	V5_REG_GPIO_B_PORT                       (V5_REG_PADC_BASE+0x120)
#define	V5_REG_GPIO_C_CFG                        (V5_REG_PADC_BASE+0x20c)
#define	V5_REG_GPIO_C_I                            (V5_REG_PADC_BASE+0x210)
#define	V5_REG_GPIO_C_PE                     (V5_REG_PADC_BASE+0x214)
#define	V5_REG_GPIO_C_DIR                     (V5_REG_PADC_BASE+0x218)
#define	V5_REG_GPIO_C_ODE                     (V5_REG_PADC_BASE+0x21c)
#define	V5_REG_GPIO_C_PORT                       (V5_REG_PADC_BASE+0x220)
#define	V5_REG_GPIO_D_CFG                     (V5_REG_PADC_BASE+0x30c)
#define	V5_REG_GPIO_D_I                           (V5_REG_PADC_BASE+0x310)
#define	V5_REG_GPIO_D_PE                         (V5_REG_PADC_BASE+0x314)
#define	V5_REG_GPIO_D_DIR                       (V5_REG_PADC_BASE+0x318)
#define	V5_REG_GPIO_D_ODE                       (V5_REG_PADC_BASE+0x31c)
#define	V5_REG_GPIO_D_PORT                       (V5_REG_PADC_BASE+0x320)
//Function Select
#define	V5_REG_FS_LEDM                      (V5_REG_PADC_BASE+0x800)
#define	V5_REG_FS_SD_GPIO                 (V5_REG_PADC_BASE+0x804)
#define	V5_REG_FS_I2C                       (V5_REG_PADC_BASE+0x808)
#define	V5_REG_FS_UART1                   (V5_REG_PADC_BASE+0x80c)
#define	V5_REG_FS_UART2                  (V5_REG_PADC_BASE+0x810)
#define	V5_REG_FS_UART3                 (V5_REG_PADC_BASE+0x814)
#define	V5_REG_FS_SPI1                    (V5_REG_PADC_BASE+0x818)
#define	V5_REG_FS_NF_DAT                 (V5_REG_PADC_BASE+0x81c)
#define	V5_REG_JTG_EN                          (V5_REG_PADC_BASE+0x820)
#define	V5_REG_USBHOST_DETECT_SEL   (V5_REG_PADC_BASE+0x824)
//Other configuration registers                
#define	V5_REG_STRAP_PIN                          (V5_REG_PADC_BASE+0x700)
#define	V5_REG_DRIVE_SEL                         (V5_REG_PADC_BASE+0x900)
#define	V5_REG_UM_OUT_OEN                      (V5_REG_PADC_BASE+0x904)
#define	V5_REG_UM_OUT_PE                       (V5_REG_PADC_BASE+0x908)
#define	V5_REG_MEM_SIG_OEN                      (V5_REG_PADC_BASE+0x90c)
#define	V5_REG_MEM_DQ_OEN_DISABLE         (V5_REG_PADC_BASE+0x910)
#define	V5_REG_MEM_PADS_PE                     (V5_REG_PADC_BASE+0x914)
#define	V5_REG_NF_DAT_PE                        (V5_REG_PADC_BASE+0x918)
#define	V5_REG_LCDDATA_FORMAT             (V5_REG_PADC_BASE+0x1000)
//#else
#define	V5_REG_PAD_GPIO_CFG					(V5_REG_PADC_BASE+0x0000)
#define	V5_REG_PAD_CTRL						(V5_REG_PADC_BASE+0x0004)
#define	V5_REG_PAD_CS_CTRL					(V5_REG_PADC_BASE+0x0008)
#define	V5_REG_PAD_UMONITOR_EN				(V5_REG_PADC_BASE+0x000c)
#define	V5_REG_PAD_SIFFLASH_CTRL				(V5_REG_PADC_BASE+0x0010)
#define	V5_REG_PAD_DEBUG_SELECT				(V5_REG_PADC_BASE+0x0014)
#define	V5_REG_PAD_GPIO_INPUT				(V5_REG_PADC_BASE+0x0018)
#define	V5_REG_PAD_MEM_PC					(V5_REG_PADC_BASE+0x001c)
#define	V5_REG_PAD_GPIO_MODE					(V5_REG_PADC_BASE+0x0030)
#define	V5_REG_PAD_GPIO_DIR					(V5_REG_PADC_BASE+0x0034)
#define	V5_REG_PAD_GPIO_P0					(V5_REG_PADC_BASE+0x0038)
#define	V5_REG_PAD_GPIO_REN_PC				(V5_REG_PADC_BASE+0x003c)
#endif


/***********************************************************************************/

/***********************************************************************************/
/*! MP4 ENC registers */
#define V5_REG_MP4ENC_RESET					(V5_REG_MP4ENC_BASE+0x0000)		/*!< MPEG4 Soft Reset Register */
#define V5_REG_MP4ENC_START					(V5_REG_MP4ENC_BASE+0x0004)		/*!< MPEG4 Start Register */
#define V5_REG_MP4ENC_SYSCTRL					(V5_REG_MP4ENC_BASE+0x0008)		/*!< MPEG4 System Control Register */
#define V5_REG_MP4ENC_STUFFCODE				(V5_REG_MP4ENC_BASE+0x000c)		/*!< Stuffing Code Register */
#define V5_REG_MP4ENC_WIDTH					(V5_REG_MP4ENC_BASE+0x0010)		/*!< Encoder Image Width Register */
#define V5_REG_MP4ENC_HEIGHT					(V5_REG_MP4ENC_BASE+0x0014)		/*!< Encoder Image Height Register */
#define V5_REG_MP4ENC_TIMERESO				(V5_REG_MP4ENC_BASE+0x0018)		/*!< VOL Time Increment Resolution */
#define V5_REG_MP4ENC_VOL_CTRL				(V5_REG_MP4ENC_BASE+0x001c)		/*!< VOL Control Register */
#define V5_REG_MP4ENC_VOP_CTRL1				(V5_REG_MP4ENC_BASE+0x0020)		/*!< VOP Control Register 1 */
#define V5_REG_MP4ENC_VOP_CTRL2				(V5_REG_MP4ENC_BASE+0x0024)		/*!< VOP Control Register 2 */
#define V5_REG_MP4ENC_TIME_INC				(V5_REG_MP4ENC_BASE+0x0028)		/*!< VOP Time Increment */
#define V5_REG_MP4ENC_FRAME_WORD_CNT		(V5_REG_MP4ENC_BASE+0x002c)		/*!< Bitstream Frame Word Count */
#define V5_REG_MP4ENC_RESYNC_CTRL			(V5_REG_MP4ENC_BASE+0x0030)		/*!< REsync marker Control Register */
#define V5_REG_MP4ENC_INT   					(V5_REG_MP4ENC_BASE+0x003c)		/*!< MPEG4 Interrupt Register */
#define V5_REG_MP4ENC_SRC_Y_ADDR				(V5_REG_MP4ENC_BASE+0x0040)		/*!< Source Image Y Component Initial Address */
#define	V5_REG_MP4ENC_SRC_CB_ADDR			(V5_REG_MP4ENC_BASE+0x0044)		/*!< Source Image Cb Component Initial Address */
#define V5_REG_MP4ENC_SRC_CR_ADDR			(V5_REG_MP4ENC_BASE+0x0048)		/*!< Source Image Cr Component Initial Address */
#define V5_REG_MP4ENC_REC_Y0_ADDR			(V5_REG_MP4ENC_BASE+0x004c)		/*!< Reconstruct Image Y0 Component Initial Address */
#define V5_REG_MP4ENC_REC_CB0_ADDR			(V5_REG_MP4ENC_BASE+0x0050)		/*!< Reconstruct Image Cb0 Component Initial Address */
#define	V5_REG_MP4ENC_REC_CR0_ADDR			(V5_REG_MP4ENC_BASE+0x0054)		/*!< Reconstruct Image Cr0 Component Initial Address */
#define V5_REG_MP4ENC_REC_Y1_ADDR			(V5_REG_MP4ENC_BASE+0x0058)		/*!< Reconstruct Image Y1 Component Initial Address */
#define V5_REG_MP4ENC_REC_CB1_ADDR			(V5_REG_MP4ENC_BASE+0x005c)		/*!< Reconstruct Image Cb1 Component Initial Address */
#define V5_REG_MP4ENC_REC_CR1_ADDR			(V5_REG_MP4ENC_BASE+0x0060)		/*!< Reconstruct Image Cr1 Component Initial Address */
#define	V5_REG_MP4ENC_OUTPUT_ADDR			(V5_REG_MP4ENC_BASE+0x0064)		/*!< Output Bitstream Initial Address */
#define	V5_REG_MP4ENC_WRAPPER_CTRL			(V5_REG_MP4ENC_BASE+0x0100)
#define	V5_REG_MP4ENC_STUCK_LIMIT			(V5_REG_MP4ENC_BASE+0x0104)
#define	V5_REG_MP4ENC_SRC_Y1_ADDR			(V5_REG_MP4ENC_BASE+0x0110)
#define	V5_REG_MP4ENC_SRC_CB1_ADDR			(V5_REG_MP4ENC_BASE+0x0114)
#define	V5_REG_MP4ENC_SRC_CR1_ADDR			(V5_REG_MP4ENC_BASE+0x0118)
//deleted by youhai_mp4enc 2007.8.30
#ifdef __NEW_SYS__
#else
#define	V5_REG_MP4ENC_IM_FRAMECTL			(V5_REG_MP4ENC_BASE+0x0120)
#define	V5_REG_MP4ENC_IM_FRAMETYPE			(V5_REG_MP4ENC_BASE+0x0124)
#define	V5_REG_MP4ENC_IM_FRAMECODE			(V5_REG_MP4ENC_BASE+0x0128)
#define	V5_REG_MP4ENC_IM_TIMECTRL			(V5_REG_MP4ENC_BASE+0x012c)
#define	V5_REG_MP4ENC_IM_TIMECNT			(V5_REG_MP4ENC_BASE+0x0130)
#define	V5_REG_MP4ENC_IM_BASEADDR			(V5_REG_MP4ENC_BASE+0x0134)
#endif
/***********************************************************************************/

/***********************************************************************************/
/*! MP4 DEC registers */
#define V5_REG_MP4DEC_RESET					(V5_REG_MP4DEC_BASE+0x0000)
#define V5_REG_MP4DEC_START					(V5_REG_MP4DEC_BASE+0x0004)
#define	V5_REG_MP4DEC_INT_MASK				(V5_REG_MP4DEC_BASE+0x0008)
#define V5_REG_MP4DEC_FUNC					(V5_REG_MP4DEC_BASE+0x000c)
#define V5_REG_MP4DEC_CHAN_NUM				(V5_REG_MP4DEC_BASE+0x0010)
#define V5_REG_MP4DEC_OUT0_Y_ADDR			(V5_REG_MP4DEC_BASE+0x0014)
#define V5_REG_MP4DEC_BITSTREAM_INI_ADD		(V5_REG_MP4DEC_BASE+0x0018) 
#define V5_REG_MP4DEC_BITSTREAM_SIZE			(V5_REG_MP4DEC_BASE+0x001c) 
#define V5_REG_MP4DEC_INT						(V5_REG_MP4DEC_BASE+0x0024) 
#define V5_REG_MP4DEC_RESO					(V5_REG_MP4DEC_BASE+0x0028) 
#define V5_REG_MP4DEC_TIME_INC_RESO			(V5_REG_MP4DEC_BASE+0x002c) 
#define V5_REG_MP4DEC_TIME_INF				(V5_REG_MP4DEC_BASE+0x0030) 
#define V5_REG_MP4DEC_TIME_CODE				(V5_REG_MP4DEC_BASE+0x0034) 
#define V5_REG_MP4DEC_MC_BASE0_ADDR     		(V5_REG_MP4DEC_BASE+0x0044) 
#define V5_REG_MP4DEC_MC_BASE1_ADDR     		(V5_REG_MP4DEC_BASE+0x0048) 
#define V5_REG_MP4DEC_MC_CB_OFFSET   			(V5_REG_MP4DEC_BASE+0x004c) 
#define V5_REG_MP4DEC_MC_CR_OFFSET     		(V5_REG_MP4DEC_BASE+0x0050) 
#define V5_REG_MP4DEC_OUT_CB_OFFSET     		(V5_REG_MP4DEC_BASE+0x0054) 
#define V5_REG_MP4DEC_OUT_CR_OFFSET    		(V5_REG_MP4DEC_BASE+0x0058) 
#define V5_REG_MP4DEC_PINGPANG_MBL 			(V5_REG_MP4DEC_BASE+0x0100) 
#define V5_REG_MP4DEC_OPT_PINGPONG_ADD		(V5_REG_MP4DEC_BASE+0x0108)
#define V5_REG_MP4DEC_ERROR_LIMIT				(V5_REG_MP4DEC_BASE+0x0104)
#define V5_REG_MP4DEC_OUT1_Y_ADDR			(V5_REG_MP4DEC_BASE+0x0108)

/***********************************************************************************/

/***********************************************************************************/
/*! TVDC registers */
#define V5_REG_TVDC_CTRL						(V5_REG_TVDC_BASE + 0x0000)
#define V5_REG_TVDC_OSD_XY					(V5_REG_TVDC_BASE + 0x0004)
#define V5_REG_TVDC_OSD_SIZE					(V5_REG_TVDC_BASE + 0x0008)
#define V5_REG_TVDC_OSD_BA					(V5_REG_TVDC_BASE + 0x000c)
#define V5_REG_TVDC_0SD_LENGTH				(V5_REG_TVDC_BASE + 0x0010)
#define V5_REG_TVDC_OSD_ALPHA					(V5_REG_TVDC_BASE + 0x0014)
#define V5_REG_TVDC_FRM0_BA					(V5_REG_TVDC_BASE + 0x0018)
#define V5_REG_TVDC_FRM1_BA					(V5_REG_TVDC_BASE + 0x001c)
#define V5_REG_TVDC_BG_COLOR					(V5_REG_TVDC_BASE + 0x0020)
#define V5_REG_TVDC_FRM_XY					(V5_REG_TVDC_BASE + 0x0024)
#define V5_REG_TVDC_FRM_SIZE					(V5_REG_TVDC_BASE + 0x0028)
#define V5_REG_TVDC_PALLETTE					(V5_REG_TVDC_BASE + 0x0800)
/***********************************************************************************/

/***********************************************************************************/
/*! TVADPCM registers */
#define V5_REG_ADPCM_CTRL						(V5_REG_TVADPCM_BASE + 0x0000)
#define V5_REG_ADPCM_IMG_WIDTH				(V5_REG_TVADPCM_BASE + 0x0004)
#define V5_REG_ADPCM_IMG_HEIGHT				(V5_REG_TVADPCM_BASE + 0x0008)
#define V5_REG_ADPCM_QSEGMENT14				(V5_REG_TVADPCM_BASE + 0x000c)
#define V5_REG_ADPCM_QSEGMENT15				(V5_REG_TVADPCM_BASE + 0x0010)
#define V5_REG_ADPCM_QSEGMENT16				(V5_REG_TVADPCM_BASE + 0x0014)
#define V5_REG_ADPCM_QSEGMENT24				(V5_REG_TVADPCM_BASE + 0x0018)
#define V5_REG_ADPCM_QSEGMENT25				(V5_REG_TVADPCM_BASE + 0x001c)
#define V5_REG_ADPCM_QSEGMENT26				(V5_REG_TVADPCM_BASE + 0x0020)
#define V5_REG_ADPCM_QVALUE14					(V5_REG_TVADPCM_BASE + 0x0024)
#define V5_REG_ADPCM_QVALUE15					(V5_REG_TVADPCM_BASE + 0x0028)
#define V5_REG_ADPCM_QVALUE16					(V5_REG_TVADPCM_BASE + 0x002c)
#define V5_REG_ADPCM_QVALUE17					(V5_REG_TVADPCM_BASE + 0x0030)
#define V5_REG_ADPCM_QVALUE24					(V5_REG_TVADPCM_BASE + 0x0034)
#define V5_REG_ADPCM_QVALUE25					(V5_REG_TVADPCM_BASE + 0x0038)
#define V5_REG_ADPCM_QVALUE26					(V5_REG_TVADPCM_BASE + 0x003c)
#define V5_REG_ADPCM_QVALUE27					(V5_REG_TVADPCM_BASE + 0x0040)
#define V5_REG_ADPCM_QUPDATE					(V5_REG_TVADPCM_BASE + 0x0044)
/***********************************************************************************/

/***********************************************************************************/
/*! TVENC registers */
#define	V5_REG_TVENC_TVCOLMAX_H				(V5_REG_TVENC_BASE + 0x0100)
#define	V5_REG_TVENC_TVCOLMAX_L				(V5_REG_TVENC_BASE + 0x0104)
#define	V5_REG_TVENC_TVROWMAX_H				(V5_REG_TVENC_BASE + 0x0108)
#define	V5_REG_TVENC_TVROWMAX_L				(V5_REG_TVENC_BASE + 0x010c)
#define	V5_REG_TVENC_VS_PULSE				(V5_REG_TVENC_BASE + 0x0110)
#define	V5_REG_TVENC_VE1_PULSE				(V5_REG_TVENC_BASE + 0x0114)
#define	V5_REG_TVENC_VE2_PULSE				(V5_REG_TVENC_BASE + 0x0118)
#define	V5_REG_TVENC_VB_PULSE				(V5_REG_TVENC_BASE + 0x011c)
#define	V5_REG_TVENC_VS_DURATION_H			(V5_REG_TVENC_BASE + 0x0120)
#define	V5_REG_TVENC_VS_DURATION_L			(V5_REG_TVENC_BASE + 0x0124)
#define	V5_REG_TVENC_VE_DURATION			(V5_REG_TVENC_BASE + 0x0128)
#define	V5_REG_TVENC_HS_PULSE				(V5_REG_TVENC_BASE + 0x012c)
#define	V5_REG_TVENC_BURST_START			(V5_REG_TVENC_BASE + 0x0130)
#define	V5_REG_TVENC_BURST_END				(V5_REG_TVENC_BASE + 0x0134)
#define	V5_REG_TVENC_DATA_START_H			(V5_REG_TVENC_BASE + 0x0138)
#define	V5_REG_TVENC_DATA_START_L			(V5_REG_TVENC_BASE + 0x013c)
#define	V5_REG_TVENC_FRONT_PORCH			(V5_REG_TVENC_BASE + 0x0140)
#define	V5_REG_TVENC_TV_CTRL					(V5_REG_TVENC_BASE + 0x0144)
#define	V5_REG_TVENC_YCOEF0_H				(V5_REG_TVENC_BASE + 0x0148)
#define	V5_REG_TVENC_YCOEF0_L				(V5_REG_TVENC_BASE + 0x014c)
#define	V5_REG_TVENC_YCOEF1_H				(V5_REG_TVENC_BASE + 0x0150)
#define	V5_REG_TVENC_YCOEF1_L				(V5_REG_TVENC_BASE + 0x0154)
#define	V5_REG_TVENC_YCOEF2_H				(V5_REG_TVENC_BASE + 0x0158)
#define	V5_REG_TVENC_YCOEF2_L				(V5_REG_TVENC_BASE + 0x015c)
#define	V5_REG_TVENC_YCOEF3_H				(V5_REG_TVENC_BASE + 0x0160)
#define	V5_REG_TVENC_YCOEF3_L				(V5_REG_TVENC_BASE + 0x0164)
#define	V5_REG_TVENC_YCOEF4_H				(V5_REG_TVENC_BASE + 0x0168)
#define	V5_REG_TVENC_YCOEF4_L				(V5_REG_TVENC_BASE + 0x016c)
#define	V5_REG_TVENC_YCOEF5_H				(V5_REG_TVENC_BASE + 0x0170)
#define	V5_REG_TVENC_YCOEF5_L				(V5_REG_TVENC_BASE + 0x0174)
#define	V5_REG_TVENC_UCOEF0_H				(V5_REG_TVENC_BASE + 0x0178)
#define	V5_REG_TVENC_UCOEF0_L				(V5_REG_TVENC_BASE + 0x017c)
#define	V5_REG_TVENC_UCOEF1_H				(V5_REG_TVENC_BASE + 0x0180)
#define	V5_REG_TVENC_UCOEF1_L				(V5_REG_TVENC_BASE + 0x0184)
#define	V5_REG_TVENC_UCOEF2_H				(V5_REG_TVENC_BASE + 0x0188)
#define	V5_REG_TVENC_UCOEF2_L				(V5_REG_TVENC_BASE + 0x018c)
#define	V5_REG_TVENC_UCOEF3_H				(V5_REG_TVENC_BASE + 0x0190)
#define	V5_REG_TVENC_UCOEF3_L				(V5_REG_TVENC_BASE + 0x0194)
#define	V5_REG_TVENC_VCOEF0_H				(V5_REG_TVENC_BASE + 0x0198)
#define	V5_REG_TVENC_VCOEF0_L				(V5_REG_TVENC_BASE + 0x019c)
#define	V5_REG_TVENC_VCOEF1_H				(V5_REG_TVENC_BASE + 0x01a0)
#define	V5_REG_TVENC_VCOEF1_L				(V5_REG_TVENC_BASE + 0x01a4)
#define	V5_REG_TVENC_VCOEF2_H				(V5_REG_TVENC_BASE + 0x01a8)
#define	V5_REG_TVENC_VCOEF2_L				(V5_REG_TVENC_BASE + 0x01ac)
#define	V5_REG_TVENC_VCOEF3_H				(V5_REG_TVENC_BASE + 0x01b0)
#define	V5_REG_TVENC_VCOEF3_L				(V5_REG_TVENC_BASE + 0x01b4)
#define	V5_REG_TVENC_SUBFREQ_3				(V5_REG_TVENC_BASE + 0x01b8)
#define	V5_REG_TVENC_SUBFREQ_2				(V5_REG_TVENC_BASE + 0x01bc)
#define	V5_REG_TVENC_SUBFREQ_1				(V5_REG_TVENC_BASE + 0x01c0)
#define	V5_REG_TVENC_SUBFREQ_0				(V5_REG_TVENC_BASE + 0x01c4)
#define	V5_REG_TVENC_SYNC_LEVEL				(V5_REG_TVENC_BASE + 0x01c8)
#define	V5_REG_TVENC_BLACK_LEVEL				(V5_REG_TVENC_BASE + 0x01cc)
#define	V5_REG_TVENC_BURST_LEVEL				(V5_REG_TVENC_BASE + 0x01d0)
#define	V5_REG_TVENC_CHROM_LEVEL			(V5_REG_TVENC_BASE + 0x01d4)
#define	V5_REG_TVENC_COLORBAR_STEP			(V5_REG_TVENC_BASE + 0x01d8)
/***********************************************************************************/


#ifdef __cplusplus
}
#endif

#endif	/*!< V578B_REGDEF_H_ */

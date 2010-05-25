/*******************************************************************************
 *  All rights reserved, Copyright (C) vimicro LIMITED 2007                                                     
 * --------------------------------------------------------------------------------------------------------------
 * [File Name]          : VIM_HIF_Driver.h
 * [Description]        : 适用于578b.
 *				    hif_driver.c需要的头文件
 *				    				
 * [Author]                 : angela	
 * [Date Of Creation]  : 2007-03-10 16:01
 * [Platform]               : not be limited by platform
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

#ifndef _VIM_5XRDK_HIF_H_
#define _VIM_5XRDK_HIF_H_

#ifdef __cplusplus
extern "C" {
#endif


#define VIM_WAITMAX	0X30000
#define VIM_READADD_LOCOSTO	0x601d

//---------------------------------------------------------------

// Set Bypass or Normal Type
typedef enum _VIM_HIF_BYPASSMODE
{
	VIM_HIF_NORMALTYPE = 0,
	VIM_HIF_BYPASSTYE = BIT2,
	VIM_HIF_ISOLATE_CORE=BIT1,
	VIM_HIF_RELATE_CORE=0,
	VIM_HIF_POWERDOWN_VR=BIT0,
	VIM_HIF_POWERUP_VR=0
}VIM_HIF_BYPASSMODE;

typedef enum _VIM_HIF_SRAMMODE
{
	VIM_HIF_SRAM_MODE = 0x0,
	VIM_HIF_FIFO_MODE = 0x2,
	// used for 820 ARM read/write mode, added by lizg
	VIM_HIF_FIFO_0_MODE = 0x3,
	VIM_HIF_FIFO_1_MODE = 0x4
}VIM_HIF_SRAMMODE;

typedef enum _VIM_HIF_DIRMODE
{
	VIM_PUTIN = 0x0,
	VIM_PUTOUT = 0x1
}VIM_HIF_DIRMODE;
// Set or Get Register
// Set Bypass or Normal Type
typedef enum _VIM_HIF_PLLMODE
{
	VIM_HIF_PLLPOWERON = 0x0,
	VIM_HIF_PLLPOWERDOWN = 0x1,
	VIM_HIF_PLLBYPASS=0X02
}VIM_HIF_PLLMODE;

typedef enum _VIM_HIF_LCDBYPASSDATA
{
	VIM_HIF_LCD_0_15 = 0x0,
	VIM_HIF_LCD_2_17= 0x1,
	VIM_HIF_LCD_1_8_10_17=0X02,
	VIM_HIF_LCD_0_7_8_15=0X03,
	VIM_HIF_LCD_1_16=0X04
	
}VIM_HIF_LCDBYPASSDATA;

#ifdef __NEW_SYS__ 	// VC0820 by lizg
//CpmModReset_CTRL
/*! vc0578b software reset control */
// To avoid warning, not use enum
#define VIM_HIF_RESET_RESERVED_0		 (1<<0)		/*!< reset reserved */
#define VIM_HIF_RESET_TIM		 (1<<1)		/*!< reset timer */
#define VIM_HIF_RESET_KEYP		 (1<<2)		/*!< reset keyboard */
#define VIM_HIF_RESET_UART		 (1<<3)		/*!< reset uart */
#define VIM_HIF_RESET_SPI		 (1<<4)		/*!< reset SPI */
#define VIM_HIF_RESET_PADC		 (1<<5)		/*!< reset pad */	
#define VIM_HIF_RESET_I2C			 (1<<6)		/*!< reset I2C */
#define VIM_HIF_RESET_USB			 (1<<7)		/*!< reset USB*/
#define VIM_HIF_RESET_LCD		 (1<<8)		/*!< reset LCD */
#define VIM_HIF_RESET_TV		 (1<<9)		/*!< reset TV */
#define VIM_HIF_RESET_I2S		 (1<<10)		/*!< reset I2S */
#define VIM_HIF_RESET_NFC		 (1<<11)		/*!< reset Nand */
#define VIM_HIF_RESET_VDEC		 (1<<12)		/*!< reset VDEC */
#define VIM_HIF_RESET_VENC		 (1<<13)		/*!< reset VENC */
#define VIM_HIF_RESET_GE		 (1<<14)		/*!< reset GE */
#define VIM_HIF_RESET_RESERVED_15		 (1<<15)		/*!< reset RESERVED */
#define VIM_HIF_RESET_JPEG		 (1<<16)		/*!< reset JPEG */
#define VIM_HIF_RESET_LBUF		 (1<<17)		/*!< reset LBUF */
#define VIM_HIF_RESET_IPP		 (1<<18)		/*!< reset IPP */
#define VIM_HIF_RESET_SIF		 (1<<19)		/*!< reset SIF */
#define VIM_HIF_RESET_RESERVED_20		 (1<<20)		/*!< reset RESERVED */
#define VIM_HIF_RESET_SDIO		 (1<<21)		/*!< reset SDIO */
#define VIM_HIF_RESET_SDRC		 (1<<22)		/*!< reset SDRC */
#define VIM_HIF_RESET_DMAC		 (1<<23)		/*!< reset DMAC */
#define VIM_HIF_RESET_RESERVED_24		 (1<<24)		/*!< reset RESERVED */
#define VIM_HIF_RESET_BIU		 (1<<25)		/*!< reset BIU */
#define VIM_HIF_RESET_IRQ		 (1<<26)		/*!< reset IRQ */
#define VIM_HIF_RESET_MARB		 (1<<27)		/*!< reset MARB */
#define VIM_HIF_RESET_AHB		 (1<<28)		/*!< reset AHB */
#define VIM_HIF_RESET_CPU		 (1<<29)		/*!< reset CPU */
#define VIM_HIF_RESET_GLOSW		 (1<<30)			/*!< global reset */
#define VIM_HIF_RESET_RTC		0x80000000//(1<<31)			/*!<  reset RTC*/

#define VIM_HIF_RESETMODULE	UINT32
#else	

//CpmModReset_CTRL
/*! vc0578b software reset control */
typedef enum _VIM_HIF_RESETMODULE			//wendy guo modified 3/27/2007
{

	VIM_HIF_RESET_LCDIF		= (1<<0),		/*!< reset lcdif */
	VIM_HIF_RESET_LCDC		= (1<<1),		/*!< reset lcdc */
	VIM_HIF_RESET_MARB		= (1<<2),		/*!< reset marb */
	VIM_HIF_RESET_JPEG		= (1<<4),		/*!< reset jpeg */
	VIM_HIF_RESET_LBUF		= (1<<5),		/*!< reset lbuf */
	VIM_HIF_RESET_IPP			= (1<<6),		/*!< reset ipp */
	VIM_HIF_RESET_SIF			= (1<<7),		/*!< reset sif */
	VIM_HIF_RESET_TVISP		= (1<<8),		/*!< reset tv & isp */
	VIM_HIF_RESET_VDEC		= (1<<9),		/*!< reset mp4 decoder */
	VIM_HIF_RESET_VENC		= (1<<10),		/*!< reset mp4 encoder */
	VIM_HIF_RESET_HRST		= (1<<11),		/*!< reset hreset */
	VIM_HIF_RESET_TVCLK		= (1<<12),		/*!< reset tv clock */
	VIM_HIF_RESET_LCDPCLK		= (1<<13),		/*!< reset lcd pclk */
	VIM_HIF_RESET_SDRC		= (1<<14),		/*!< reset sdrc */

	VIM_HIF_RESET_GLOBAL		= 0xffff			/*!< global reset */

}VIM_HIF_RESETMODULE;
#endif

/*! vc0578b clock gate control */
typedef enum _VIM_HIF_CLK_CTRL				//wendy guo modified 3/27/2007
{
	VIM_HIF_CLKGATE_SIF		= (1<<0),		/*!<  */
	VIM_HIF_CLKGATE_ISP		= (1<<1),		/*!<  */
	VIM_HIF_CLKGATE_IPP		= (1<<2),		/*!<  */
	VIM_HIF_CLKGATE_LBUF		= (1<<3),		/*!<  */
	VIM_HIF_CLKGATE_LCD		= (1<<4),		/*!<  */
	VIM_HIF_CLKGATE_VDEC		= (1<<5),		/*!<  */
	VIM_HIF_CLKGATE_VENC		= (1<<6),		/*!<  */
	VIM_HIF_CLKGATE_JPEG		= (1<<7),		/*!<  */
	VIM_HIF_CLKGATE_ISPCLK	= (1<<8),		/*!<  */
	VIM_HIF_CLKGATE_LBUFCLK	= (1<<9),		/*!<  */
	VIM_HIF_CLKGATE_MARB		= (1<<10),		/*!<  */
	VIM_HIF_CLKGATE_RAM1T0	= (1<<11),		/*!<  */
	VIM_HIF_CLKGATE_RAM1T1	= (1<<12),		/*!<  */
	VIM_HIF_CLKGATE_RAM1T2	= (1<<13),		/*!<  */
	VIM_HIF_CLKGATE_RAM1T3	= (1<<14),		/*!<  */
	VIM_HIF_CLKGATE_SDRC		= (1<<15),		/*!<  */
	VIM_HIF_CLKGATE_BIU		= (1<<16),		/*!<  */
	VIM_HIF_CLKGATE_PADC		= (1<<18),		/*!<  */
	VIM_HIF_CLKGATE_IRQ		= (1<<19),		/*!<  */
	VIM_HIF_CLKGATE_TV		= (1<<20),		/*!<  */
	VIM_HIF_CLKGATE_TVFCLK	= (1<<21),		/*!<  */
	
	VIM_HIF_CLKGATE_ALL		= 0x3FFFFF		/*!< clock gate all */
}VIM_HIF_CLK_CTRL;

typedef enum _VIM_HIF_EXTPIN_CTRL
{
		VIM_HIF_PIN_INT    			=BIT9,
		VIM_HIF_PIN_LCDRES 		=BIT8,
		VIM_HIF_PIN_CSRSTN     		=BIT7,
		VIM_HIF_PIN_CSPWD			=BIT6,
		VIM_HIF_PIN_CSENB			=BIT5,
		VIM_HIF_PIN_GPIO4          	=BIT4,
		VIM_HIF_PIN_GPIO3			=BIT3,
		VIM_HIF_PIN_GPIO2            	=BIT2,
		VIM_HIF_PIN_GPIO1            	=BIT1,
		VIM_HIF_PIN_GPIO0               	=BIT0,
		VIM_HIF_PIN_ALL           	=0xfff
}VIM_HIF_EXTPIN_CTRL;

typedef struct tag_TAcceContextInt
{	/*register and sram access context store function if they are interrupted in process*/
	UINT32              addr;
	UINT8               stat; 
} TAcceContext, *PTAcceContext;

typedef enum _VIM_HIF_GPIOMODE
{
	VIM_GPIO_PUTIN = 0x0,
	VIM_GPIO_PUTOUT = 0x1
}VIM_HIF_GPIOMODE;
///------------------------register---------------------
void	VIM_HIF_SetReg8(UINT32 adr, UINT8 val);
UINT8	VIM_HIF_GetReg8(UINT32 adr);
void	VIM_HIF_SetReg16(UINT32 adr,UINT16 val);
UINT16	VIM_HIF_GetReg16(UINT32 adr);
void	VIM_HIF_SetReg32(UINT32 adr,UINT32 val);
UINT32	VIM_HIF_GetReg32(UINT32 adr);

//----------------------------------------------------------------------------
// Sram
UINT32 VIM_HIF_ReadSram(UINT32 adr,  HUGE UINT8 *buf, UINT32 size,VIM_HIF_SRAMMODE Mode);
UINT32 VIM_HIF_WriteSram(UINT32 adr, const HUGE UINT8 *buf, UINT32 size,VIM_HIF_SRAMMODE Mode);
UINT32 VIM_HIF_WriteSramReverse(UINT32 adr,  const HUGE UINT8 *buf, UINT32 size);
UINT32 VIM_HIF_WriteSramOnWord(UINT32 adr, UINT16 dat, UINT32 size);
//------------------------cofig ---------------------------------------
void VIM_HIF_SetGpioValue(UINT8 Num,BOOL Value);
void VIM_HIF_SetGpioInfo(VIM_HIF_GPIOMODE Mode,UINT8 Num);

#if(BOARD_TYPE==VC0820_FPGA)
#define VIM_INT_NUM  32
#else
#define VIM_INT_NUM  12
#endif
/*!< interrupt type */

typedef enum _VIM_HIF_INT_TYPE
{
#if(BOARD_TYPE==VC0820_FPGA)
	VM_HIF_INT_SIF	= 15,		/*!< sif interrupt */
	VM_HIF_INT_ISP	= 16,		/*!< isp interrupt */
	VM_HIF_INT_LBUF	= 17,		/*!< lbuf interrupt */
	VM_HIF_INT_JPEG	= 18,		/*!< jpeg interrupt */
	VM_HIF_INT_IPP	= 11,		/*!< ipp interrupt */
	VM_HIF_INT_MARB	= 0,		/*!< marb interrupt */
	VM_HIF_INT_LCDC	= 20,		/*!< lcdc interrupt */
	VM_HIF_INT_TVDC	= 21,		/*!< tvdc interrupt */
	VM_HIF_INT_VDEC	= 25,		/*!< vdec interrupt */
	VM_HIF_INT_VENC	= 24,		/*!< venc interrupt */
	VM_HIF_INT_HWT	= 28,		/*!< Hardware Timer 2 interrupt */
	VM_HIF_INT_GPIO	= 9,		/*!< GPIO interrupt */
	//ALL
	INT_ALL		= VIM_INT_NUM
#else
	VM_HIF_INT_SIF	= 0,		/*!< sif interrupt */
	VM_HIF_INT_ISP	= 1,		/*!< isp interrupt */
	VM_HIF_INT_LBUF	= 2,		/*!< lbuf interrupt */
	VM_HIF_INT_JPEG	= 3,		/*!< jpeg interrupt */
	VM_HIF_INT_IPP	= 4,		/*!< ipp interrupt */
	VM_HIF_INT_MARB	= 5,		/*!< marb interrupt */
	VM_HIF_INT_LCDC	= 6,		/*!< lcdc interrupt */
	VM_HIF_INT_TVDC	= 7,		/*!< tvdc interrupt */
	VM_HIF_INT_VDEC	= 8,		/*!< vdec interrupt */
	VM_HIF_INT_VENC	= 9,		/*!< venc interrupt */
	VM_HIF_INT_HWT	= 10,		/*!< Hardware Timer interrupt */
	VM_HIF_INT_GPIO	= 11,		/*!< GPIO interrupt */
	//ALL
	INT_ALL		= VIM_INT_NUM

#endif
}VIM_HIF_INT_TYPE;

	
typedef void (*PVIM_Isr)(void);
#if (BOARD_TYPE==VC0820_FPGA)
// First level interrupt number
#define VIM_FIRST_LEVEL_INT_NUM 	32// angela changed it
#else
// First level interrupt number
#define VIM_FIRST_LEVEL_INT_NUM 	12// angela changed it
#endif
// Marb int
#define VIM_MARB_INT_NUM 			16 // angela changed it
// Marb int OLD 8个+addition marb interrupt 6个
// JPEG int
#define VIM_JPEG_JPEG_INT_NUM 		3 //maybe 16 in future
#if (BOARD_TYPE==VC0820_FPGA)//add by zhouj
//LBUF int
#define VIM_JPEG_LBUF_INT_NUM 		2 //maybe 16 in future
#else
//LBUF int
#define VIM_JPEG_LBUF_INT_NUM 		1 //maybe 16 in future
#endif
// SIF int
#define VIM_SIF_SIF_INT_NUM 		2 //maybe 16 in future
// ISP int
#define VIM_SIF_ISP_INT_NUM 		3 //maybe 16 in future
// DISP int
#define VIM_DISP_INT_NUM 		8 //maybe 16 in future
// mpeg4 encoder int
#define VIM_MPEG4ENC_INT_NUM 		1 //maybe 16 in future
// mpeg4 decoder int
#define VIM_MPEG4DEC_INT_NUM 		1 //maybe 16 in future
// mpeg4 timer int
#define VIM_TIMER_INT_NUM 		2 //maybe 16 in future

typedef struct tag_VIM_HIF_TISR{
	UINT16	byFirstLevelInt[VIM_FIRST_LEVEL_INT_NUM]; /* First level (module level) interrupt numbers */
	UINT16	bSecondLevelIntFlag[VIM_FIRST_LEVEL_INT_NUM]; /* The interrupt flag of first level interrupt */
	PVIM_Isr     marbisr[VIM_MARB_INT_NUM]; /* Marb module's ISRs */
#if (BOARD_TYPE==VC0820_FPGA)//add by zhouj
	PVIM_Isr	 lbufisr[VIM_JPEG_LBUF_INT_NUM];
	PVIM_Isr	 jpegisr[VIM_JPEG_JPEG_INT_NUM]; /* Jpeg module's ISRs */
	PVIM_Isr     Sifisr[VIM_SIF_SIF_INT_NUM]; /* sif  isp module's ISRs */
#else
	PVIM_Isr	 jpegisr[VIM_JPEG_JPEG_INT_NUM+VIM_JPEG_LBUF_INT_NUM]; /* Jpeg module's ISRs */
	PVIM_Isr     Sifisr[VIM_SIF_SIF_INT_NUM+VIM_SIF_ISP_INT_NUM]; /* sif  isp module's ISRs */
#endif	
	
	PVIM_Isr     Dispisr[VIM_DISP_INT_NUM]; /* disp module's ISRs */
	PVIM_Isr     Mpeg4isr[VIM_MPEG4ENC_INT_NUM+VIM_MPEG4DEC_INT_NUM+VIM_TIMER_INT_NUM]; /* mpeg4 encoder module's ISRs */	

	
} VIM_HIF_TISR, *PVIM_HIF_TISR;

void VIM_HIF_SetReg8(UINT32 adr, UINT8 val);
UINT8 VIM_HIF_GetReg8(UINT32 adr);
void VIM_HIF_SetReg16(UINT32 adr, UINT16 val);
UINT16 VIM_HIF_GetReg16(UINT32 adr);
void VIM_HIF_SetReg32(UINT32 adr, UINT32 val);
UINT32 VIM_HIF_GetReg32(UINT32 adr);

void VIM_HIF_SetSpecialReg(UINT8 val);  
UINT32 VIM_HIF_ReadSram(UINT32 adr,  HUGE UINT8 *buf, UINT32 size,VIM_HIF_SRAMMODE Mode);
UINT32 VIM_HIF_WriteSramReverse(UINT32 adr, const HUGE UINT8 *buf, UINT32 size);
UINT32 VIM_HIF_WriteSram(UINT32 adr, const HUGE UINT8 *buf, UINT32 size,VIM_HIF_SRAMMODE Mode);
UINT32 VIM_HIF_WriteSramOnWord(UINT32 adr, UINT16 dat, UINT32 size);

//void VIM_HIF_SetMulti16(void);

void VIM_HIF_SetDelay(PVIM_TIMING Timing);
void VIM_HIF_InitBypass(void);
UINT32 VIM_HIF_GetMclock(void);
UINT32 VIM_HIF_GetHclock(void);
//void VIM_HIF_SetBypassOrNormal(UINT32 byOrNormal);
//void VIM_HIF_SwitchPanelCs(UINT8 Panel);
VIM_RESULT VIM_HIF_SetLcdPclock(UINT32 clock);
VIM_RESULT VIM_HIF_InitPll(PVIM_CLK ClkInfo);
void VIM_HIF_SetPllStatus(VIM_HIF_PLLMODE Mode);
void VIM_HIF_SetLdoStatus(VIM_LDOMODE Val);
void VIM_HIF_SetExterPinMode(PVIM_EXTERNPINS_INFO PinsMode);
//void VIM_HIF_ResetSubModule(VIM_HIF_RESETMODULE wVal);   
VIM_RESULT VIM_HIF_InitInerruptLevel(PVIM_INTERRUPT Interrupt);
//void VIM_HIF_SetIntModuleEn(VIM_HIF_INT_TYPE byIntEnEnum,BOOL Enable);
UINT32 VIM_HIF_GetIntFlagSec(VIM_HIF_INT_TYPE byOffset);
//UINT16 VIM_HIF_GetIntEnableSec(VIM_HIF_INT_TYPE byOffset);
//void VIM_HIF_SetIntEnableSec(VIM_HIF_INT_TYPE byOffset, UINT16 uVal);
//void VIM_HIF_ClearIntModule(VIM_HIF_INT_TYPE byIntEnEnum);
//VIM_RESULT _ISR_HIF_IntHandle(void);

void VIM_HIF_SwitchPanelCs(UINT8 Panel);
void VIM_HIF_SetModClkOnOff(UINT32 wVal,BOOL OnOff);
void VIM_HIF_ResetSubModule(VIM_HIF_RESETMODULE wVal);
void VIM_HIF_SetBypassOrNormal(UINT32 byOrNormal);






void VIM_HIF_SetIntModuleEn(VIM_HIF_INT_TYPE byIntEnEnum,BOOL Enable);
UINT16 VIM_HIF_GetIntEnableSec(VIM_HIF_INT_TYPE byOffset);
void VIM_HIF_SetIntEnableSec(VIM_HIF_INT_TYPE byOffset, UINT16 uVal);
VIM_RESULT  _ISR_HIF_IntHandle(void);
 void VIM_HIF_ClearIntModule(VIM_HIF_INT_TYPE byIntEnEnum);
void VIM_HIF_SetLcdDataBus(VIM_HIF_LCDBYPASSDATA Mode);
void get_video_data(UINT8*des, UINT8 *scr, UINT32 size);


#if VIM_USER_SUPPORT_SDRAM
UINT32 VIM_HIF_WriteSdram(UINT32 adr, const HUGE UINT8 *buf, UINT32 size,VIM_HIF_SRAMMODE Mode);
UINT32 VIM_HIF_ReadSdram(UINT32 adr,  HUGE UINT8 *buf, UINT32 size,VIM_HIF_SRAMMODE Mode);

#endif

//---------------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif /* _RDK_HIF_H_ */

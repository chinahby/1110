#ifndef _VIM_CONFIG_H
#define _VIM_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif
#include "compile_macro.h"

/*******************************************************************************
	form here Define By Amanda Deng 2007/10/27 for project control
 *******************************************************************************/
#ifndef NU820_MOBILE
#define NU820_MOBILE				0x00	
#endif
#ifndef NU820_MOBILE_ARM9
#define NU820_MOBILE_ARM9			NU820_MOBILE+1	
#endif
#ifndef NU820_MOBILE_HOSTCTL
#define NU820_MOBILE_HOSTCTL 		NU820_MOBILE+2
#endif
#ifndef NU820_MOBILE_DEBUG
#define NU820_MOBILE_DEBUG		NU820_MOBILE+3	
#endif
#ifndef NU820_MOBILE_RELEASE
#define NU820_MOBILE_RELEASE		NU820_MOBILE+4	
#endif

#ifndef NU820_MP4
#define NU820_MP4					0x10	
#endif
#ifndef NU820_MP4_DEBUG
#define NU820_MP4_DEBUG			NU820_MP4+1
#endif
#ifndef NU820_MP4_RELEASE
#define NU820_MP4_RELEASE			NU820_MP4+2
#endif
#ifndef NU820_MP4_METEST
#define NU820_MP4_METEST			NU820_MP4+3
#endif

//DEMO A560
//#ifndef NU820_MP4_DEMOA560
//#define NU820_MP4_DEMOA560			NU820_MP4+4
//#endif


#ifndef PROJECT_NU820_VERSION
#define PROJECT_NU820_VERSION		NU820_MOBILE
#endif

#ifndef ID_FPAG 
#define ID_FPAG							0		
#endif
#ifndef ID_VC0824 
#define ID_VC0824						1		
#endif
#ifndef ID_VC0878 
#define ID_VC0878						2						
#endif
#ifndef ID_VC0848 
#define ID_VC0848						3						
#endif

/*******************************************************************************
	end Define By Amanda Deng 2007/10/27 for project control
 *******************************************************************************/
#if( PROJECT_NU820_VERSION==NU820_MOBILE_RELEASE)
#define PLY_CONTROL_BY_HOST	
#define AUDIO_CODEC_CONTROL_BY_HOST
#define USB_CONTROL_BY_HOST	1
#endif

#if( PROJECT_NU820_VERSION>=NU820_MP4)
#define PLY_CONTROL_BY_HOST	
#define USB_CONTROL_BY_HOST	1	
#endif

//------------- SDRAM size config start-----------------------
#define SIZE_1M				0x100000
#define SIZE_2M				0x200000
#define SIZE_3M				0x300000
#define SIZE_4M				0x400000
#define SIZE_8M				0x800000
#define SIZE_12M			0xC00000
#define SIZE_15M			0xF00000
#define SIZE_16M			0x1000000
#define SIZE_64M			0x4000000

#if(CHIP_ID==ID_FPAG)
#define SDRAM_SIZE				SIZE_16M		
// V578 rdk driver use memory
#define 	V5_1TSRAM_BASE						0x40C00000
#define 	V5_1TSRAM_TOTALSIZE					0x100000
#endif

#if(CHIP_ID==ID_VC0824)
#define SDRAM_SIZE				SIZE_16M		//default set
#define TASK_STACK_SIZE		0x20000

//#define SDRAM_SIZE				SIZE_64M		
#if(SDRAM_SIZE==SIZE_16M)		
// V578 rdk driver use memory
#define 	V5_1TSRAM_BASE						0x40A00000
#define 	V5_1TSRAM_TOTALSIZE					(0x600000-TTB_SIZE-BIU_COMMAND_SIZE)
#endif
#if(SDRAM_SIZE==SIZE_64M)		
#define 	V5_1TSRAM_BASE						0x41000000
#define 	V5_1TSRAM_TOTALSIZE					0x1000000
#endif
//-------------- Clock related begin----------------
#define XCLKIN_6M	0		// 0 : 12M , 1: 6M
//-------------- Clock related end----------------
#define SEND_PLAY_TIME			0

#define RM_MALLOC_START_ADDR				V5_1TSRAM_BASE
#define MALLOC_SIZE_THRESHOLD		2	// not use small malloc	// 0x1000		// large than 4K size use large malloc region
#define MALLOC_SMALL_RATION		0x1			// range 1~16, [1/256~256/256]

#endif

#if(CHIP_ID==ID_VC0878)
#define SDRAM_SIZE				SIZE_4M		
#define TASK_STACK_SIZE		0x20000
// V578 rdk driver use memory
#define 	V5_1TSRAM_BASE						0x0
#define 	V5_1TSRAM_TOTALSIZE					0x0
//-------------- Clock related begin----------------
#define XCLKIN_6M	1		// 0 : 12M , 1: 6M
//-------------- Clock related end----------------
#define SEND_PLAY_TIME			1			// Send play time to baseband in timer
#define RM_MALLOC_START_ADDR				V5_1TSRAM_BASE
#define MALLOC_SIZE_THRESHOLD		0x1000	// large than 4K size use large malloc region
#define MALLOC_SMALL_RATION		0x10	// range 1~16, [1/256~256/256]

#endif

#define SRAM_CACHEON			0			//0: cache off, 1: cache on 

#if(SRAM_CACHEON==0)
#define SRAM_CACHE_ATTR		0
#else
#define SRAM_CACHE_ATTR		3
#endif

#if(SDRAM_SIZE==SIZE_4M)
#define CACHE_ON_ADDR  			0x40000000
#define CACHE_ON_SIZE 			SIZE_3M
#define CACHE_OFF_ADDR		 	0x40300000
#define CACHE_OFF_SIZE 			SIZE_1M
#endif

#if(SDRAM_SIZE==SIZE_16M)
#define CACHE_ON_ADDR  			0x40000000
#define CACHE_ON_SIZE 			SIZE_15M
#define CACHE_OFF_ADDR		 	0x40000000+CACHE_ON_SIZE
#define CACHE_OFF_SIZE 			SDRAM_SIZE-CACHE_ON_SIZE
#endif

#if(SDRAM_SIZE==SIZE_64M)
#define CACHE_ON_ADDR  			0x40000000
#define CACHE_ON_SIZE 			SIZE_12M
#define CACHE_OFF_ADDR		 	0x40C00000
#define CACHE_OFF_SIZE 			SIZE_4M
#endif

#define NC_MALLOC_SUPPORT		0			// 1: support nc malloc	0: unsupport nc malloc

//------------- System buffer size config start-----------------------
#define  	TTB_SIZE				0x10000			//64K reserved for MMU TTB
#define  	BIU_COMMAND_SIZE		0x4000			//16K reserved for BIU command and parameter, at the top address of SRAM


//------------- nand begin -----------------------
/* nand driver中有操作系统中的信号量操作 */
/* 打开此宏即打开信号量操作，nand driver可重入；关闭此宏即关闭信号量操作，nand driver不可重入 */
#ifndef OS_NUCLEUS
#define OS_NUCLEUS
#endif

#ifdef OS_NUCLEUS

/* 关掉此宏，则关掉测试nand可重入的两个任务代码,by yangl 2007 10 26 */
#ifndef TEST_NAND_SEMAPHORE
//#define TEST_NAND_SEMAPHORE
#endif	/* TEST_NAND_SEMAPHORE */

/* 打开此宏表明nand和sd共用同一个信号量，因为在821中， nand和sd有io共用限制 add by yangl 2007 12 19*/
#ifndef MUTEX_NANDSD
//#define MUTEX_NANDSD
#endif	/* MUTEX_NANDSD */

#endif	/* OS_NUCLEUS */
//------------- nand end -----------------------

/*lhl add for touch panel version. 2008.2.20*/
#ifndef TOUCHPANEL_VERSION
//#define TOUCHPANEL_VERSION
#endif

//--------------Capture JPG begin--------------------------
#define CAP_VFIFO_LEN	0x1D800  //0x20000
#define CAP_VFIFO_LEN_FRAME	0x8000
//--------------Capture JPG end--------------------------

//--------------Display JPG begin--------------------------
#define DISP_VFIFO_LEN		0x19000//0x20000
#define DISP_VFIFO_ADD		0x20040000 // Must in SRAM for hardware bug
//--------------Display JPG end--------------------------

//-------------- Define memory map macor begin----------------
#define MP_MEMORY_MAP	0
#define MP_MARB_MAP	1
#define CHECK_MALLOC	0	//check malloc 
#define ACK_IN_HISR		0	// 1: Send ack in hisr, 0: Send ack in maintask
#define RAMINFO			1	// 1: use raminfo to alloc hardware driver memory
#define REMAP_PHY_ADDR 	0x40000000  // 0x20000000: SRAM		0x40000000: SDRAM
//#define VC0848SRAM 	1


//-------------- Define memory map macor end----------------



//-------------- Test code marcro begin----------------

#define	 DRV_DMA_TEST_ENA			0
#define	 DRV_INT_TEST_ENA			0
#define	 DRV_TIMER_TEST_ENA		0
#define	 DRV_578RDK_TEST_ENA		0
#define	 DRV_SENSOR_TEST_ENA		0
#define	 DRV_AUDIO_TEST_ENA		0
#define	 DRV_SPI_TEST_ENA		0
#define    DRV_FAT_TEST_ENA			0//add by sunlei,2008.03.05
#define    DRV_SD_TEST_ENA			0//add by sunlei,2008.03.05
#define	 DRV_USB_TEST_ENA			0//add by sunlei,2008.03.05
#define 	 DRV_GPIO_TEST_ENA		0//add by sunlei,2008.03.05
#define DRV_FTL_TEST_ENABLE 0	/* ftl test switch, 0, close; 1, open 2008 03 10 */
#define DRV_NF_TEST_ENABLE 0	/* nand test switch, 0, close; 1, open 2008 03 10 */
#define DRV_I2C_TEST_ENA		0

#define APP_MEM_TEST_ENA			0
#define APP_TASK_TEST_ENA			0
#define APP_CPULOAD_TEST_ENA			0
#define	 MEDIA_ENGINE_TEST_ENA		0	//amanda deng
#define	 MEDIA_PLAYER_TEST_ENA		0	//amanda deng

//-------------- Test code marcro end----------------
#ifdef __cplusplus
}
#endif

#endif






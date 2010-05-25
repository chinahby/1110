
#ifndef _MMD_USER_DEPEND_H_
#define _MMD_USER_DEPEND_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "mmd_data_type.h"
#include "mmd_common_typedef.h"
#include "me_object.h"
//#include "rtwthread.h"
/************************************************************************************ 
BIU数据总线的宽度
************************************************************************************/
#define MMD_BIU_DATABUS16BIT

/************************************************************************************ 
Normal 模式访问VC0820的CPU_RS的片选地址，0: index, 1: data, demo board use A9
************************************************************************************/
#define MMD_HOSTSIDE_INDEX_ADDR 		0x40000000
#define MMD_HOSTSIDE_VALUE_ADDR 		0x401E0004

//#define MMD_HOSTSIDE_INDEX_ADDR            0x8000000
//#define MMD_HOSTSIDE_VALUE_ADDR            0x8000200


/************************************************************************************ 
Bypass 模式访问LCD_RS的片选地址，0: index, 1: data, demo board use A9
************************************************************************************/
#define MMD_BYPASS_ADDR_RSLOW             0x8000000
#define MMD_BYPASS_ADDR_RSHIGH            0x8000200


/************************************************************************************ 
是否需要对VC0820的寄存器，内存进行读写测试
************************************************************************************/
#define MMD_BIU_SRAM_TEST

// Sensor自动匹配类型列表
#define MMD_DEF_SNR_MT9M112_YUV 					0
#define MMD_DEF_SNR_MT9M112_YUV_5XX				1

// Sif frequece define
#define MMD_SIF_CLK_FRE								36000	// 36M

/************************************************************************************ 
选择主屏使用的DriverIC的类型
************************************************************************************/
#define HX_PANEL 0
#define LPFD5420A 0
#define S6D0170_PANEL 0
#ifdef P_Q1 // T_Q1 Define in DMSS_FLAGS of makefile
#define LGDP4525_PANEL 1
#endif

#if HX_PANEL
#define MMD_MAINDRIVERIC_TYPE MMD_DEF_LCD_HX8347_18BIT
#elif LPFD5420A
#define MMD_MAINDRIVERIC_TYPE MMD_DEF_LCD_LPFD5420A
#elif S6D0170_PANEL
#define MMD_MAINDRIVERIC_TYPE MMD_DEF_LCD_S6D0170
#elif LGDP4525_PANEL
#define MMD_MAINDRIVERIC_TYPE MMD_DEF_LCD_LGDP4525_16BIT
#else
#define MMD_MAINDRIVERIC_TYPE  MMD_DEF_LCD_S6D0164_16BIT// MMD_DEF_LCD_ILI9320_16BIT_240x320 /* Main panel type */
#endif

/************************************************************************************ 
选择附屏使用的DriverIC的类型
************************************************************************************/
#define MMD_SUBDRIVERIC_TYPE    MMD_DEF_LCD_NOSUB /* Sub panel type */

/************************************************************************************ 
选择LCD Panel 模组的类型
************************************************************************************/
#if HX_PANEL
#define MMD_LCD_PANEL_TYPE LCD_HX8347_18BIT
#elif LPFD5420A
#define MMD_LCD_PANEL_TYPE LCD_LPFD5420A
#elif S6D0170_PANEL
#define MMD_LCD_PANEL_TYPE LCD_S6D0170
#elif LGDP4525_PANEL
#define MMD_LCD_PANEL_TYPE LCD_LGDP4525
#else
#define MMD_LCD_PANEL_TYPE	LCD_ILI9320_16BIT
#endif

/************************************************************************************ 
设置LCD Panel 模组的宽高
************************************************************************************/
#define MMD_LCD_WIDTH			176
#define MMD_LCD_HEIGHT			220

#define LCD_HEIGHT  220
#define LCD_WIDTH 176
// define preview window size and recording window size
#define MMD_FRAME_WIN_WIDTH         176
#define MMD_FRAME_WIN_HEIGHT        144  //modify by xinggang.xu 20090109

#define FILE_LEN_UTF8    128
#define FILE_LEN_UNICODE 256

/************************************************************************************ 
选择点屏使用的所在的模式(Bypass or Normal)
************************************************************************************/
#define MMD_LCD_POWER_ON_IN_BYPASS_MODE    1		// 1: Bypass, 0: Normal


/************************************************************************************ 
Default scene set
************************************************************************************/
#define SCENE_DEFAULT	SCENE_MEDIAPLAY
#define CODEC_DEFAULT	CODEC_PCM//CODEC_MID//
#define LCDCA0SDRAM 0
#define USERDISPLAYSIZE 0

/***********************************************************
Device to host interrupt trigger mode defination
***********************************************************/
#define TRI_MODE_POSITIVE_PULSE				1		
#define TRI_MODE_NEGATIVE_PULSE				2		
#define TRI_MODE_HIGH_LEVEL					3
#define TRI_MODE_LOW_LEVEL					4

#define TRI_MODE		TRI_MODE_POSITIVE_PULSE

/*for mute speaker*/
#define GPIO_A_CFG               0x6017000C
#define GPIO_A_DIR               0x60170018

/* define the GPIO control in QSC6020 for VC0848  */
#define VC_GPIO_CTRL(port, state) \
{ \
   BIO_TRISTATE(BIO_GPIO_##port##_REG, BIO_GPIO_##port##_M, BIO_OUTPUT_ENA_V); \
   BIO_OUT(BIO_GPIO_##port##_REG, BIO_GPIO_##port##_M, BIO_GPIO_##port##_##state##_V); \
  \
}

//#include "AEESTDLIB.h"
#define FARF_MSG  1
#define FARF(x, p)  if (1 == FARF_##x) DBGPRINTF p
   
    
/***********************************************************
Malloc , Free,  memcpy, memset defination
***********************************************************/
#ifdef SD_EM_DBG
extern void* SD_Malloc(unsigned int size);
extern void SD_Free(void* pBuf);
#endif

//#define MMD_RDK_HEAP_SIZE 0x200000//4096
#define MMD_RDK_HEAP_SIZE 0x100000//4096

#define MMD_MemCpy		memcpy
#define MMD_MemSet		memset
#define MMD_MemCmp	memcmp
#define MMD_Malloc		MMD_MM_Malloc
#define MMD_Free		MMD_MM_Free

typedef void (*TimerISR)(void);

typedef enum _MODULE_POWER
{
         POWER_AUDC_VDAC=0,
	  POWER_NF_CS,
	  POWER_SDRAM,
	  POWER_USB,
	  POWER_CORE
} MODULE_POWER;

typedef struct _REG_SAVE
{
        MMD_U32 addr;
	 MMD_U32 value;
}  REG_SAVE;
/*typedef struct _MEMORY_FILE_  MEMORY_FILE;
struct _MEMORY_FILE_{
	MMD_S08* 	FileAddress;
	MMD_S32 		FileReadCur;
	MMD_S32 		FileWriteCur;
	MMD_S32 		FileTotalLength;
};*/

void BrewTimerISR(void *pUser);
typedef enum _MMD_CODEC_MUX
{
	  CODEC_MUX_FM=0x01010101,
	  CODEC_MUX_MEDIA_PLAY=0x0101,
		 	
	  CODEC_MUX_RESERVED=0x7FFFFFFF
} MMD_CODEC_MUX;

typedef enum {
   VC_OFM_READ      = 0x0001,
   VC_OFM_READWRITE = 0x0002,
   VC_OFM_CREATE    = 0x0004,
   VC_OFM_APPEND    = 0x0008
} VC_EFS_OP;


/************************************************
  APIs
************************************************/
void MMD_USER_GPIO_Init(void);
/* Chip H/W control */
void MMD_USER_GPIO_PreSleep(void);
void MMD_USER_GPIO_PostWakeup(void);

extern void MMD_USER_SetChipMode(MMD_U08 Mode);
extern void MMD_USER_ResetChip(void);
extern void MMD_USER_WakeupInit(void);
extern void MMD_USER_Wakeup(void);
extern void MMD_USER_BiuTiming_Switch(MMD_U32 mode);
/* OS functions depending on user environment */
extern void MMD_USER_OS_GetResource(MMD_U32 ResourceID);
extern void MMD_USER_OS_RelResource(MMD_U32 ResourceID);

/* IRQ treatment functions depending on user environment */
extern void    MMD_USER_LockIRQFromChip  (void);
extern void    MMD_USER_UnlockIRQFromChip(void);

/* Delay function depending on user environment  */
extern void MMD_USER_WaitMSEC(MMD_U32 msec);
extern void MMD_USER_WaitMICRS(MMD_U32 msec);
/* Communication handshake depending on user environment  */
extern MMD_U32 MMD_USER_WaitResponse(MMD_U32 cmd, CMD_ACK_FORMAT *pAck);
extern void MMD_USER_SDRam_Init(void);

//extern void MMD_RV_WaitMSEC(MMD_U32 msec);
extern void MMD_USER_SensorPowerOn(void);
extern void MMD_USER_SensorPowerOff(void);

/* LCD initialization routine used to operate low level IO  */
void MMD_USER_ResetLCD_Bypass(void);
void MMD_USER_ResetLCD_Normal(void);
void MMD_USER_PowerOnLogo_Bypass(void);
void MMD_USER_PowerOnLogo_Normal(void);

void MMD_USER_Timer_Create(void(*pTimerISR)(void), MMD_U32 interval);
void MMD_USER_Timer_Release(void);
void MMD_USER_Timer_Start(void);
void MMD_USER_Timer_Stop(void);
void MMD_USER_Timer_Set(TimerISR pTimerISR, MMD_U32 interval);

/* Porting interface for FAT File System APIs */
void MMD_USER_GetDate(MMD_DATE *psDate);
void MMD_USER_GetTime(MMD_TIME *psTime);

void  MMD_ConvertString(MMD_S08* str,MMD_S08* strDesti);  


#ifdef RV_ENABLE
void MMD_USER_LCD_Rotation(MMD_U32 degree);
typedef void (*TaskFunc)(void* pMsg);

#define	MSG_SIZE			3
#define RV_TIMER_INTERVAL	 200		// unit ms
#define	RV_FRESH_TIMER_INTERVAL	200
#define	RV_FLUSH_TIMER_INTERVAL	400

void MMD_USER_Task_Create(TaskFunc pTaskFunc);
void MMD_USER_Task_Release(void);

void MMD_USER_FS_Init(void);
void MMD_USER_FS_Release(void);
void* MMD_USER_FS_File_Open(void* pFileName, void* mode);
MMD_U32 MMD_USER_FS_File_Get_Length(void* handle);
MMD_S32 MMD_USER_FS_File_Read(void* handle, MMD_S08* buf, MMD_S32 count);
MMD_S32 MMD_USER_FS_File_Write(void* handle, MMD_S08* buf, MMD_S32 count);
MMD_S32 MMD_USER_FS_File_Close(void* handle);
void MMD_USER_Msg_Send(void* para);
MMD_U32 MMD_USER_CMMB_GPIO_SELECT(MMD_U32 reset_pin, MMD_U32 intr_pin);
#endif

void MMD_USER_WaitMICRS(MMD_U32 msec);


int memory_fileread(void* file, char* buffer, int count);
int memory_filewrite(void* file, char* buffer, int count);
int memory_fileseek(void* file, int offset, int mode);
int memory_filetell(void* file);
int memory_fileeof(void* file);
int memory_fileclose(void* file);
int memory_filedelete(void *address);
void*memory_fileopen(void *address, void *size);

int TCL_RAM_fileread(void* file, char* buffer, int count);
int TCL_RAM_filewrite(void* file, char* buffer, int count);
int TCL_RAM_fileseek(void* file, int offset, int mode);
int TCL_RAM_filetell(void* file);
int TCL_RAM_fileeof(void* file);
int TCL_RAM_fileclose(void* file);
int TCL_RAM_filedelete(void *address);
void* TCL_RAM_fileopen(void *address, void *size);

extern fs_callbacks	 g_memory_baseband_file;
extern fs_callbacks	 g_TCL_RAM_file;
// nMode should be VC_OFM_READ, VC_OFM_CREATE or VC_OFM_APPEND
extern void memory_set_fileread_mode(int nMode);
void EnableUDisk(void);
void DisableUDisk(void);

void VC_Sleep_Allow(void);
void VC_Sleep_Forbid(void);
#ifdef __cplusplus
}
#endif

#endif  /* _MMD_USER_DEPEND_H_ */


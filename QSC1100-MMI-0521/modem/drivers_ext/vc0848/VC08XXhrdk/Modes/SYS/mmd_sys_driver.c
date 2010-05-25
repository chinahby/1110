/*******************************************************************************
 *  All rights reserved, Copyright (C) vimicro LIMITED 2007                                                     
 * --------------------------------------------------------------------------------------------------------------
 * [File Name]            : mmd_sys_driver.c
 * [Description]          : VC0820 system related API, including software version, etc.
 * [Author]               : zhangxf
 * [Date Of Creation]     : 2007-03-13
 * [Platform]             : Platform independent
 * [Note]                 : None
 *
 * Modification History : 
 * ------------------------------------------------------------------------------
 * Date        Author     Modifications
 * ------------------------------------------------------------------------------
 * 2007-03-13  zhangxf    Created
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
 
/*
 * INCLUDE FILE                                              
 */

#include "mmp_rdk.h"

#include "bio.h"
#include "msg.h"
#include "AEEStdlib.h"


#define MMD_SYS_DEBUG_INFO
#define MMD_SYS_DEBUG_ERR
#define MMD_SYS_DEBUG_NOTE
#define MMD_SYS_DEBUG_WARNING

#ifdef MMD_SYS_DEBUG_INFO
    #define PRINTF(WriteVal)        MMD_PRINTF(WriteVal) //general print out
#else
    #define PRINTF(WriteVal)
#endif

#ifdef MMD_SYS_DEBUG_ERR
    #define PRINTFE(WriteVal)   MMD_PRINTFE(WriteVal) //error
#else
    #define PRINTFE(WriteVal)
#endif

#ifdef MMD_SYS_DEBUG_NOTE
    #define PRINTFN(WriteVal)   MMD_PRINTFN(WriteVal) //note
#else
    #define PRINTFN(WriteVal)
#endif

#ifdef MMD_SYS_DEBUG_WARNING
    #define PRINTFW(WriteVal)   MMD_PRINTFW(WriteVal) //warning
#else
    #define PRINTFW(WriteVal)
#endif

SYSCallback g_SysCallback=NULL;

/*
 * GLOBAL VARIABLES DEFINITION                                          
 */
// system mode initial status, 0: uninit; 1: init
static MMD_U32 g_HostVersino=0x100;	//V1.00
// whole system work mode
MMD_U32 g_system_mode=0;
volatile MMD_U32 g_IPC_A2B0=0,g_IPC_A2B1=0;
extern MMD_U32 g_biu_width;
extern MMD_U32 setOverlay;
static SCENE_CODE_INFO gSceneCode[SCENE_NUM];
static CODE_MAP_INFO gCodecCode[CODEC_NUM];
#ifdef RV_ENABLE
static CODE_MAP_INFO gRVCodecCode[RV_CODEC_NUM];
#endif
static MMD_U32 g_SceneIdxInDevice=SCENE_NUM;
static MMD_U32 g_CodecIdxInDevice=CODEC_NUM;
volatile MMD_U32 last_pmu_mode=PMU_MODE_NUM;
volatile unsigned int allpanel=0;//,times=0;

volatile MMD_U32 g_Ack_MinCmd;
volatile MMD_U32 g_Ack_OP1;
volatile MMD_U32 g_Ack_OP2;
volatile MMD_U32 DeviceMallocTimes=0;
volatile MMD_U32 DeviceFreeTimes=0;
#ifdef AUTOFIND_INPRON
volatile MMD_U32 autoFindSensor=1;
#endif


volatile MMD_U32 core_mode = SCENE_MEDIAPLAY;
volatile MMD_U32 g_sys_clk=CLK_NUM;
/*************************************************
  Function:       
  	Mode_Sys_Lisr
  Description:    
 	sys mode command process call back function, called in LISR
  Input:
  	pMsg:	Host to device cmd msg
  Output:        
	NULL 	
  Return:         
  	Error code 
  History:        
      <author>  	<time>     	<desc>
      lizg    		06/12/31     	create this function  
*************************************************/
MMD_U32 MMD_Mode_Sys_Lisr(MMD_U32 cmd)
{
	MMD_MSG Msg;
	MMD_U08 min;

	// get command and parameter 
	MMD_BIU_Get_D2H_Ack(&Msg);
	MMD_ASSERT(Msg.CMD==cmd);
	
	min = Msg.CMD & CMD_MASK_MIN;
	
	g_Ack_MinCmd=min;
	g_Ack_OP1=Msg.OP1;
	g_Ack_OP2=Msg.OP2;

	switch(min)
	{
	default:
		break;
	}

	return MMD_RET_NOERR;
}

/*-----------------------------------------------------------------------------
[FUNCTION NAME]
  MMD_SYS_Init
[DESCRIPTION]
  This command inits the system. 
[ARGUMENT]
  None
[RETURN VALUE]
  MMD_RET_NOERR: success
  MMD_RET_ERR:    fail
-----------------------------------------------------------------------------*/
MMD_U32 MMD_VC08XX_Init(void)
{

	MMD_U32 ret;

	PRINTF(("[SYS] MMD_SYS_Init\r\n"));
	
#ifdef MMD_BIU_DATABUS16BIT
	MMD_BIU_Init(1);
#else
	MMD_BIU_Init(0);  
#endif

	MMD_USER_GPIO_Init();

   MMD_USER_WakeupInit();

#if (defined(CMMB_ENABLE)| defined(RV_ENABLE))
	//External SDRAM initialization
	MMD_SYS_Sdram_Init();
#endif
#ifdef MMD_BIU_SRAM_TEST
	//Test BIU, SRAM, SDRAM
	ret = MMD_SYS_TestRW();
	if(ret!=MMD_RET_NOERR)
	{
      MMD_SYS_PMU_SWITCH(PMU_MODE_BYPASS); // set vc0848 in deep sleep while in bypass mode
       VC_GPIO_CTRL(42, LO);
       PRINTFE(("Download code error, ret=0x%08X!\n",ret));
       return ret; 
	}       
#endif

	MMD_SYS_Set_TriggerMode();

 	MMD_SYS_Scene_Info_Init();

	MMD_MM_Initialize();
	
	ret = MMD_SYS_Mode_Switch(SCENE_DEFAULT);
	if(ret!=MMD_RET_NOERR)
	{
		//PRINTFE(("Download code error, ret=0x%08X!\n",ret));
      FARF(MSG,("Download code error, ret=0x%08X!\n",ret));
        //add by Laurence,xinggang.xu 20081222 start 
        MMD_SYS_PMU_SWITCH(PMU_MODE_BYPASS);
        VC_GPIO_CTRL(51, LO); // disable VDD_CORE V1.2 if init failed //
        VC_GPIO_CTRL(42, LO);
		return ret;	
	}		
	ret = MMD_SYS_Codec_Switch(CODEC_DEFAULT);
	if(ret!=MMD_RET_NOERR)
	{
		//PRINTFE(("Codec code switch error, ret=0x%08X!\n",ret));
      FARF(MSG,("Codec code switch error, ret=0x%08X!\n",ret));
        //add by Laurence,xinggang.xu 20081222 start 
        MMD_SYS_PMU_SWITCH(PMU_MODE_BYPASS);
        VC_GPIO_CTRL(51, LO); // disable VDD_CORE V1.2 if init failed //32
        VC_GPIO_CTRL(42, LO);
		return ret;	
	}		
	VIM_MAPI_InitIfo();
	MMD_MediaEngine_Init();

	MMD_SYS_PMU_SWITCH(PMU_MODE_BYPASS);
	MMD_USER_WaitMICRS(1000);
	Lcd_MPanel_SetList(&(MLcdParm.poweron),1);	
	// tuer off the power of sensor for power saving after VC0848 initialization.
	VC_GPIO_CTRL(42, LO);
	return MMD_RET_NOERR;

}
/*-----------------------------------------------------------------------------
[FUNCTION NAME]
  RDK_Mode_Get
[DESCRIPTION]
  Get current work mode.
[ARGUMENT]
  NULL
[RETURN VALUE]
  Work mode 
-----------------------------------------------------------------------------*/
MMD_U32 MMD_SYS_Mode_Get(void)
{
	return g_SceneIdxInDevice;
}
/*-----------------------------------------------------------------------------
[FUNCTION NAME]
  MMD_SYS_Mode_Switch
[DESCRIPTION]
[ARGUMENT]
  NULL
[RETURN VALUE]
  Work mode   Switch work mode(scene)

-----------------------------------------------------------------------------*/
MMD_U32 MMD_SYS_Mode_Switch(MMD_U32 mode)
{
	MMD_U32 ret;
    FARF(MSG,("MMD_SYS_Mode_Switch=%d",mode));
	ret = MMD_SYS_DownCore(mode, 1);
	return ret;
}

/*-----------------------------------------------------------------------------
[FUNCTION NAME]
  MMD_SYS_Codec_Switch
[DESCRIPTION]
  Switch codec 
[ARGUMENT]
  NULL
[RETURN VALUE]
  Work mode 
-----------------------------------------------------------------------------*/
MMD_U32 MMD_SYS_Codec_Switch(MMD_U32 codec)
{
	MMD_U32 ret,mode,len;
	MMD_U32 code_addr, code_len,zi_addr, zi_len,load_addr;
    FARF(MSG,("MMD_SYS_Codec_Switch=%d" ,codec));
	if(codec==CODEC_UNCHANGED)
		return MMD_RET_NOERR;

	mode = MMD_SYS_Mode_Get();
	if((mode!=SCENE_MEDIAPLAY)&(mode!=SCENE_RV_OVERLAY))
		return MMD_RET_SCENE_ERR;


	if(g_CodecIdxInDevice==codec)
		return MMD_RET_NOERR;

	setOverlay = 0;
	MMD_LCD_BLY_Release();
	if(mode == SCENE_MEDIAPLAY)	
	{
	zi_addr = gCodecCode[codec].ZI_Image_base;
	zi_len = gCodecCode[codec].ZI_Image_limit - gCodecCode[codec].ZI_Image_base;
	code_addr = gCodecCode[codec].RORW_Image_base|0x20000000;
	load_addr = gCodecCode[codec].RORW_Load_base&(~0x40000000);
	code_len = gCodecCode[codec].RORW_Image_limit - gCodecCode[codec].RORW_Image_base;
	code_len = (code_len +3)&(~3);

	// clear ZI region
	ret= MMD_SYS_Mem_Set(zi_addr, 0, (MMD_U16)zi_len);
	if(ret)
		return ret;
	// Download code
		Vm_WrSram(code_addr,EXEC_SAMR_START+load_addr, code_len);

	// restart memory pool
	ret=MMD_SYS_Mem_Pool_Create((zi_addr+zi_len)|0x20000000);
	if(ret)
		return ret;

	// codec function register
	ret=MMD_SYS_Func_Register(codec);
	if(ret)
		return ret;
		#if 0
		switch(codec)
		{
			case CODEC_SAMR:
				MMD_SYS_SetModeClk(CLK_CPU192M_BUS64M);
				break;
			case CODEC_MP4A:
				MMD_SYS_SetModeClk(CLK_CPU192M_BUS64M);
				break;
			case CODEC_MP3:
				MMD_SYS_SetModeClk(CLK_CPU96M_BUS48M);
				break;
			case CODEC_AAC:
				MMD_SYS_SetModeClk(CLK_CPU96M_BUS48M);
				break;
			case CODEC_AMR:
				MMD_SYS_SetModeClk(CLK_CPU192M_BUS64M);
				break;
			case CODEC_IMA:
				MMD_SYS_SetModeClk(CLK_CPU216M_BUS72M);
				break;
			case CODEC_PCM:
				MMD_SYS_SetModeClk(CLK_CPU216M_BUS72M);
				break;
			case CODEC_MID:
				MMD_SYS_SetModeClk(CLK_CPU216M_BUS72M);
				break;
			default:
				break;
		}
		#endif
	g_CodecIdxInDevice=codec;
	}
#ifdef RV_ENABLE
	if(mode == SCENE_RV_OVERLAY)	
	{
		zi_addr = gRVCodecCode[codec].ZI_Image_base;
		zi_len = gRVCodecCode[codec].ZI_Image_limit-gRVCodecCode[codec].ZI_Image_base;
		
		code_addr = gRVCodecCode[codec].RORW_Image_base;
		load_addr = gRVCodecCode[codec].RORW_Load_base;
		code_len = gRVCodecCode[codec].RORW_Image_limit - gRVCodecCode[codec].RORW_Image_base;

		// clear ZI region
		while(zi_len)
		{
			len = (zi_len>0xF000)?0xF000:zi_len;
			ret= MMD_SYS_Mem_Set(zi_addr, 0, (MMD_U16)len);
			if(ret)
				return ret;
			zi_addr += len;
			zi_len -= len;
		}	

		// Download code
		Vm_WrSram(code_addr,(MMD_S08*)load_addr, code_len);
		g_CodecIdxInDevice=codec;
		MMD_SYS_Icache_Clean();
		return MMD_RET_NOERR;
	}
#endif	
	return MMD_RET_NOERR;
}

/*-----------------------------------------------------------------------------
[FUNCTION NAME]
  RDK_Set_Reg
[DESCRIPTION]
  Set 820 chip register.
[ARGUMENT]
  addr: address
  data: data
[RETURN VALUE]
  NULL  
-----------------------------------------------------------------------------*/
void MMD_SYS_Reg_Set(MMD_U32 addr, MMD_U32 data)
{
	Vm_SetReg(addr, data);
}

/*-----------------------------------------------------------------------------
[FUNCTION NAME]
  RDK_Get_Reg
[DESCRIPTION]
  Get 820 chip register.
[ARGUMENT]
  addr: address
[RETURN VALUE]
  register value    
-----------------------------------------------------------------------------*/
MMD_U32 MMD_SYS_Reg_Get(MMD_U32 addr)
{
	return Vm_GetReg(addr);
}

/*-----------------------------------------------------------------------------
[FUNCTION NAME]
  RDK_Mem_Read
[DESCRIPTION]
  Read 820 side memory, sdram or sram
[ARGUMENT]
  addr: address
  pBuf: buffer pointer
  size: read length, make sure it less than buffer size
[RETURN VALUE]
  NULL  
-----------------------------------------------------------------------------*/
void MMD_SYS_Mem_Read(MMD_U32 addr, MMD_S08* pBuf, MMD_U32 size)
{
	Vm_RdSram(addr, pBuf, size);
}

/*-----------------------------------------------------------------------------
[FUNCTION NAME]
  RDK_Mem_Write
[DESCRIPTION]
  Write 820 side memory, sdram or sram
[ARGUMENT]
  addr: address
  pBuf: buffer pointer
  size: write length, make sure it less than buffer size
[RETURN VALUE]
  NULL  
-----------------------------------------------------------------------------*/
void MMD_SYS_Mem_Write(MMD_U32 addr, MMD_S08* pBuf, MMD_U32 size)
{
	Vm_WrSram(addr, pBuf, size);
}


/*-----------------------------------------------------------------------------
[FUNCTION NAME]
  MMD_SYS_Mem_Set
[DESCRIPTION]
  Set 820 side memory, just call memset in device side
[ARGUMENT]
  addr: address
  ch: set value
  size: set length, make sure it less than 64K
[RETURN VALUE]
  NULL  
-----------------------------------------------------------------------------*/
MMD_U32 MMD_SYS_Mem_Set(MMD_U32 addr, MMD_U08 ch, MMD_U16 size)
{
	MMD_MSG Msg;
	MMD_U32 timeout=1000;
	
	g_Ack_MinCmd = MIN_RESERVED;
	
	Msg.CMD=MAJ_SYS|MIN_SYS_MEM_SET;
	Msg.OP1=addr;
	Msg.OP2=ch|(size<<16);
	MMD_BIU_Send_H2D_Cmd(&Msg);
	
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
		{
			MMD_Check_Irq_Staus_And_Wait(1);
		}
		else
			break;
		timeout--;
	}
	if(timeout)
		return MMD_RET_NOERR;
	else
		return MMD_RET_TIMEOUT;
}


/*-----------------------------------------------------------------------------
[FUNCTION NAME]
  MMD_SYS_Mem_Pool_Restart
[DESCRIPTION]
  Set 820 side memory, just call memset in device side
[ARGUMENT]
  addr: address
  ch: set value
  size: set length, make sure it less than 64K
[RETURN VALUE]
  NULL  
-----------------------------------------------------------------------------*/
MMD_U32 MMD_SYS_Mem_Pool_Create(MMD_U32 start_addr)
{
	MMD_MSG Msg;
	MMD_U32 timeout=1000;
	
	g_Ack_MinCmd = MIN_RESERVED;
	
	Msg.CMD=MAJ_SYS|MIN_SYS_CREATE_MEM_POOL;
	Msg.OP1=start_addr;
	Msg.OP2=0;
	MMD_BIU_Send_H2D_Cmd(&Msg);
	
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
		{
			MMD_Check_Irq_Staus_And_Wait(1);
		}
		else
			break;
		timeout--;
	}
	if(timeout)
		return MMD_RET_NOERR;
	else
		return MMD_RET_TIMEOUT;
}



/*-----------------------------------------------------------------------------
[FUNCTION NAME]
  MMD_SYS_Icache_Clean
[DESCRIPTION]
  Clean 8XX side Icache 
[ARGUMENT]
  NULL
[RETURN VALUE]
  NULL  
-----------------------------------------------------------------------------*/
MMD_U32 MMD_SYS_Icache_Clean(void)
{
	MMD_MSG Msg;
	MMD_U32 timeout=1000;
	
	g_Ack_MinCmd = MIN_RESERVED;
	
	Msg.CMD=MAJ_SYS|MIN_SYS_ICACHE_CLEAN;
	Msg.OP1=0;
	Msg.OP2=0;
	MMD_BIU_Send_H2D_Cmd(&Msg);
	
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
		{
			MMD_Check_Irq_Staus_And_Wait(1);
		}
		else
			break;
		timeout--;
	}
	if(timeout)
		return MMD_RET_NOERR;
	else
		return MMD_RET_TIMEOUT;
}

/*-----------------------------------------------------------------------------
[FUNCTION NAME]
  MMD_SYS_Func_Register
[DESCRIPTION]
  Set 820 side memory, just call memset in device side
[ARGUMENT]
  addr: address
  ch: set value
  size: set length, make sure it less than 64K
[RETURN VALUE]
  NULL  
-----------------------------------------------------------------------------*/
MMD_U32 MMD_SYS_Func_Register(MMD_U32 codec_type)
{
	MMD_MSG Msg;
	MMD_U32 timeout=1000;
	
	g_Ack_MinCmd = MIN_RESERVED;
	
	Msg.CMD=MAJ_SYS|MIN_SYS_FUNC_REGISTER;
	Msg.OP1=codec_type;
	Msg.OP2=0;
	MMD_BIU_Send_H2D_Cmd(&Msg);
	
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
			{
			MMD_Check_Irq_Staus_And_Wait(1);
		}
		else
			break;
		timeout--;
	}
	if(timeout)
		return MMD_RET_NOERR;
	else
		return MMD_RET_TIMEOUT;
}

/*-----------------------------------------------------------------------------
[FUNCTION NAME]
  RDK_Sdram_Init
[DESCRIPTION]
  Config 820 side sdram, sdram or sram
[ARGUMENT]
  NULL  
[RETURN VALUE]
  NULL  
-----------------------------------------------------------------------------*/
void MMD_SYS_Sdram_Init(void)
{
	MMD_USER_SDRam_Init();
}
/*-----------------------------------------------------------------------------
[FUNCTION NAME]
  RDK_Reset_820
[DESCRIPTION]
  Reset 820 whole chip
[ARGUMENT]
  NULL  
[RETURN VALUE]
  NULL  
-----------------------------------------------------------------------------*/
void MMD_SYS_Reset_8XX(void)
{
	// reset only can be done in normal mode
	MMD_USER_SetChipMode(MMD_CHIP_MODE_NORMAL);
	MMD_USER_ResetChip();
	g_biu_width=0;
}

/*-----------------------------------------------------------------------------
[FUNCTION NAME]
  Scatter Reset 848 when system setup up.
  MMD_SYS_Reset_N_LOW and MMD_SYS_Reset_N_HIGH only Called before RAM init. 
  And we must call MMD_SYS_Reset_N_HOLD after RAM inited.
  
[DESCRIPTION]
  Reset 820 whole chip
[ARGUMENT]
  NULL  
[RETURN VALUE]
  NULL  
-----------------------------------------------------------------------------*/
void MMD_SYS_Reset_N_LOW(void)
{
    //Make GPIO1 (Reset_N) Low, GPIO17 (VDD2V9_EN) High, GPIO51 (VDD_CORE) High and GPIO52 (VC_BYPASS) Low
    HWIO_OUT(GPIO_OE_0, BIO_GPIO_1_M | BIO_GPIO_17_M);
    HWIO_OUT(GPIO_OUT_0, BIO_GPIO_1_LO_V | BIO_GPIO_17_HI_V);
    
    HWIO_OUT(GPIO_OE_2, BIO_GPIO_51_M |BIO_GPIO_52_M);    
    HWIO_OUT(GPIO_OUT_2, BIO_GPIO_52_LO_V);
}

void MMD_SYS_Reset_N_HIGH(void)
{
    //Make GPIO1 (Reset_N) High, Keep GPIO17 (VDD2V9_EN) High, GPIO51 (VDD_CORE) High and GPIO52 (VC_BYPASS) Low
    HWIO_OUT(GPIO_OE_0, BIO_GPIO_1_M | BIO_GPIO_17_M);
    HWIO_OUT(GPIO_OUT_0, BIO_GPIO_1_HI_V | BIO_GPIO_17_HI_V);
    
    HWIO_OUT(GPIO_OE_2, BIO_GPIO_51_M | BIO_GPIO_52_M);    
    HWIO_OUT(GPIO_OUT_2, BIO_GPIO_51_HI_V | BIO_GPIO_52_LO_V);
}

void MMD_SYS_Reset_N_HOLD(void)
{
    //Keep GPIO1 (Reset_N) High, GPIO17 (VDD2V9_EN) High and GPIO51 (VC_BYPASS) Low
    //and remember its status by call BIO functions.
    MMD_USER_SetChipMode(MMD_CHIP_MODE_NORMAL); 
    VC_GPIO_CTRL(51, HI);
    gpio_recover_pupd_state(1);
    
    g_biu_width = 0; 
}

#ifdef LOAD_CODE_FROM_VC0988				   
/***********************************************************************
  Function:       
  	MMD_SYS_DownCore
  Description:    
 	Download VC0848 code
  Input:
	MMD_U32 idx:				Scene index
	MMD_U32 load_flag:		download flag
							1: download code and run from Zero address
							0: just restart from Zero address
  Return:         
  	Error code 
  History:        
      <author>  	<time>     	<desc>
      lizg    		08/05/07     	create this function  
***********************************************************************/
MMD_U32 MMD_SYS_LoadFileCore(MMD_U32 idx,MMD_U32 addr)
{
	MMD_U32  code_len;
	MMD_S08 *pCodeAddr;
	void* filehandle;
	VFAT_FILE* fhand;
	MMD_S32 ret;
	MMD_U32 i, len;
	MMD_S08 dstBuf[256];
	
	MMD_U08 corefilename[128];
	memset(corefilename,0,128);
	MMD_USER_FS_Init();//988 fs init
	switch(idx)
	{
#if LOAD_CORE_FILE_USB
		case SCENE_USB:
			Gbk2UnicodeStr(corefilename,"V8XXCore_Usb.bin");
			break;
#endif
#if LOAD_CORE_FILE_REC_AMR
		case SCENE_REC_AMR:
			Gbk2UnicodeStr(corefilename,"V8XXCore_MediaRec.bin");
			break;
#endif
#if LOAD_CORE_FILE_MEDIAPLAY
		case SCENE_MEDIAPLAY:
			Gbk2UnicodeStr(corefilename,"SRAM_EXEC_ROM");
			break;
#endif
#if LOAD_CORE_FILE_CMMB
		case SCENE_CMMB:
			Gbk2UnicodeStr(corefilename,"V8XXCore_CMMB.bin");
			break;
#endif
#if LOAD_CORE_FILE_RVOVERLAY
		case SCENE_RV_OVERLAY:
			Gbk2UnicodeStr(corefilename,"V8XXCore_rv_overlay.bin");
			break;
#endif
		default:
			PRINTF(("Can't load core from sd!Load core from BB.\n"));
			return MMD_RET_SCENE_ERR;
	}
	filehandle = MMD_USER_FS_File_Open(corefilename, 0);
	if(filehandle == NULL)
	{
		PRINTF(("Core file open error!\n"));
		return MMD_RET_ERR;
	}
	fhand = (VFAT_FILE*)filehandle;
	pCodeAddr = MMD_Malloc(fhand->length);
	if(pCodeAddr == NULL)
	{
		PRINTF(("Core file buffer malloc error!\n"));
		return MMD_RET_MALLOC_ERR;
	}
	ret = MMD_USER_FS_File_Read(filehandle, pCodeAddr, fhand->length);
	if(ret <= 0)
	{
		PRINTF(("Core file read error!\n"));
		MMD_Free(pCodeAddr);
		return MMD_RET_ERR;
	}
	code_len = fhand->length;
	
	Vm_WrSram(addr, pCodeAddr, code_len);
	
	// read back check
		len = 256;
	Vm_RdSram(addr,dstBuf, len);
	for(i=0;i<len;i++)
	{
		if(dstBuf[i]!=pCodeAddr[i])
		{
			PRINTFE(("download SDRAM_EXEC_ROM error\r\n"));
			break;
		}   
	}
	if (i!=len)
	{
		MMD_Free(pCodeAddr);
		return MMD_RET_DOWNCODE_ERR;    
	}

	Vm_RdSram(addr+code_len-len,dstBuf, len);
	for(i=0;i<len;i++)
	{
		if(dstBuf[i]!=*(pCodeAddr+code_len-len+i))
		{
			PRINTFE(("download SDRAM_EXEC_ROM error, i=%d\r\n", i));
			break;
		}   
	}
	
	MMD_Free(pCodeAddr);
	if (i!=len)
			return MMD_RET_DOWNCODE_ERR;   
	return MMD_RET_NOERR;
}
#endif
/***********************************************************************
  Function:       
  	MMD_SYS_DownCore
  Description:    
 	Download VC0848 code
  Input:
	MMD_U32 idx:				Scene index
	MMD_U32 load_flag:		download flag
							1: download code and run from Zero address
							0: just restart from Zero address
  Return:         
  	Error code 
  History:        
      <author>  	<time>     	<desc>
      lizg    		08/05/07     	create this function  
***********************************************************************/
MMD_U32 MMD_SYS_LoadCore(MMD_U32 idx,MMD_U32 addr)
{
	MMD_U32  code_len;
	MMD_S08 *pCodeAddr;
	//void* filehandle;
	//VFAT_FILE* fhand;
	//MMD_S32 ret;
	MMD_U32 i, len;
	MMD_S08 dstBuf[256];
	
	code_len = gSceneCode[idx].CodeLen;
	pCodeAddr = gSceneCode[idx].StartAddr;
	PRINTF(("code_len=%d\n", code_len));

	Vm_WrSram(addr, pCodeAddr, code_len);


	// read back check
	len = 256;
	Vm_RdSram(addr,dstBuf, len);
	for(i=0;i<len;i++)
	{
		if(dstBuf[i]!=pCodeAddr[i])
		{
			PRINTFE(("download SDRAM_EXEC_ROM error\r\n"));
			break;
		}   
	}
	if (i!=len)
		return MMD_RET_DOWNCODE_ERR;    

	Vm_RdSram(addr+code_len-len,dstBuf, len);
	for(i=0;i<len;i++)
	{
		if(dstBuf[i]!=*(pCodeAddr+code_len-len+i))
		{
			PRINTFE(("download SDRAM_EXEC_ROM error, i=%d\r\n", i));
			break;
		}   
	}

	if (i!=len)
		return MMD_RET_DOWNCODE_ERR;   
	return MMD_RET_NOERR; 
}
/***********************************************************************
  Function:       
  	MMD_SYS_DownCore
  Description:    
 	Download VC0848 code
  Input:
	MMD_U32 idx:				Scene index
	MMD_U32 load_flag:		download flag
							1: download code and run from Zero address
							0: just restart from Zero address
  Return:         
  	Error code 
  History:        
      <author>  	<time>     	<desc>
      lizg    		08/05/07     	create this function  
***********************************************************************/
MMD_U32 MMD_SYS_DownCore(MMD_U32 idx, MMD_U32 load_flag)
{

	MMD_U32 addr=0, time_out=2000;
	// MMD_U32 i, len, code_len;
	MMD_S08 str[8];
	MMD_U32 HostVersion, CoreVersion;
	//void* filehandle;
	//VFAT_FILE* fhand;
	MMD_S32 ret;

	PRINTF(("Download nuclues core start\r\n"));
	if((g_SceneIdxInDevice==idx)&&(load_flag==1))
	{
	       PRINTF(("Scene code already in VC0848\r\n"));                 
	       return MMD_RET_NOERR;
	}
	if((g_SceneIdxInDevice!=idx)&&(load_flag==0))
	{
	       PRINTFE(("Scene code not in VC0848, can not restart\r\n"));              
	       return MMD_RET_RESTART_ERR;
	}

	// lock CPU software reset
	Vm_SetReg(0x60010300, 0x20000000);

	setOverlay = 0;
	MMD_LCD_BLY_Release();
	     
	if(load_flag)
	{
	       if((idx == SCENE_CMMB)|(idx == SCENE_RV_OVERLAY)|(idx == SCENE_RV20))
	       {
			Vm_SetReg(0x60020000, 0x0);
			Vm_SetReg(0x60020004, 0x3);        
			addr =  0x40000000;
			PRINTF(("\nDown CMMB or RV Core to SDRAM\n"));
	       }
	       else
	       {
			// sram remap   
			Vm_SetReg(0x60020000, 0x1);
			Vm_SetReg(0x60020004, 0x2);
			addr =  0x20000000;
			PRINTF(("\nDown Core to SRAM\n"));
	       }
	       MMD_USER_WaitMICRS(100); //zhangxiang mod 09/01/14 //1000
	       //MMD_USER_WaitMICRS(1000);
#ifdef LOAD_CODE_FROM_VC0988				   
	       if(MMD_SYS_LoadFileCore(idx,addr))//add by aihouyin for load file core
			MMD_SYS_LoadCore(idx,addr);//add by aihouyin for load core
#else
		MMD_SYS_LoadCore(idx,addr);//add by aihouyin for load core
#endif
	       // reset codec index
	       g_CodecIdxInDevice = CODEC_NUM;
	       PRINTFN(("Download successful\r\n"));
	}
         //Release CPU software reset 
         g_IPC_A2B0=0;
	  if((idx == SCENE_MEDIAPLAY)|(idx == SCENE_REC_AMR)|(idx == SCENE_REC_MP3))
		  MMD_USER_BiuTiming_Switch(1);	 
         Vm_SetReg(0x60010300,0);
         // wait 820 hardware initial complete
         PRINTFN(("Reset VC0820!\r\n"));
         do
         {
                   MMD_Check_Irq_Staus_And_Wait(100);
                   time_out--;
         }while((g_IPC_A2B0!=1)&&(time_out>0));

         if(g_IPC_A2B0!=1)
         {
                   PRINTFE(("VC0820 hardware initial error\r\n"));
                   return MMD_RET_HARD_INIT;
         }                 

         g_SceneIdxInDevice = idx;
         PRINTFN(("VC0820 hardware initial successful\r\n"));       
		 
#ifdef RV20_ENABLE
	if(idx == SCENE_RV20)
	{
		PRINTFN(("RV 2.0 Core in device\n"));    
		return MMD_RET_NOERR;
	}	
#endif
         // Get coreversion and show it through UART
         MMD_SYS_GetVersion(&HostVersion, &CoreVersion);
         MMD_MemSet(str, 0,sizeof(str));
         MMD_MemCpy(str, &gSceneCode[CoreVersion>>16].Scene_type, 4);
         PRINTFN(("CoreVersion=0x%08X, CoreType= <%s>\r\n", CoreVersion, str));    

         // Initialize file mode global variable, please add scene here if read2core or writefrcore is needed

         if (idx == SCENE_MEDIAPLAY || idx == SCENE_REC_AMR) {
                   ret = MMD_FILE_InitCore();
                   if(ret)
                            return ret;
         }

         if(g_SceneIdxInDevice==SCENE_MEDIAPLAY)
         {
                   ret = MMD_SYS_Codec_Info_Init();
                   if(ret)
                            return ret;
         }
         return MMD_RET_NOERR;

}

/*-----------------------------------------------------------------------------
[FUNCTION NAME]
  MMD_SYS_TestRW
[DESCRIPTION]
  820 register, sram, sdram R/W test
[ARGUMENT]
  NULL
[RETURN VALUE]
  Error code    
-----------------------------------------------------------------------------*/
MMD_U32 MMD_SYS_TestRW( void )
{
	MMD_U32 addr=0;
	MMD_U32 data=0,old_data;
	MMD_U32 i, len;
	MMD_S08 srcBuf[256],dstBuf[256];

	//(1) register  accesse test, GPIO_A_PE = 0x6017_0014
	// get chip ID
	addr = 0x60010604;
	data = Vm_GetReg(addr);
	PRINTF(("Chip version =0x%x\r\n",data));

	addr = 0x600A0070;
	old_data = 	Vm_GetReg(addr);
	Vm_SetReg(addr,0x12345678);

    //Sram_Read_Delay(200);
	data = Vm_GetReg(addr);
	if(data!=0x12345678)
	{
		//PRINTFE(("Access register inside 820 error,data=0x%x.\r\n",data));
		return MMD_RET_TEST_REG_ERR;    
	}   
	//else
	//	PRINTFN(("Access register inside 820 succeed.\r\n"));
	Vm_SetReg(addr,old_data);

	//(2) sram r/w test
	addr = 0x20000000;
	len = 256;
	for(i=0;i<len;i++)
		srcBuf[i]=i;
	Vm_WrSram(addr,srcBuf, len);
	Vm_RdSram(addr,dstBuf, len);
	for(i=0;i<len;i++)
	{
		if(srcBuf[i]!=dstBuf[i])
		{
			PRINTFE(("Access sram inside 820 error.\r\n"));
			return MMD_RET_TEST_SRAM_ERR;   
		}   
	}       
	if(i==len)  
	        PRINTFN(("Access sram inside 820 succeed.\r\n"));
#if 0
{
	MMD_U32 Src[256],Dst[256],data,count;

	count = 0;
	while(count<0x800000)
	{
		for(i=0;i<256;i++)
		{
			Src[i] = 0x40000000+count+i;
		}
		Vm_WrSram(0x40000000+count,(MMD_S08*)Src, 1024);
		count+=1024;
	}	
	count = 0;
	while(count<0x800000)
	{
		for(i=0;i<256;i++)
		{
			Src[i] = 0x40000000+count+i;
		}
		Vm_RdSram(0x40000000+count,(MMD_S08*)Dst, 1024);
		for(i=0;i<len;i++)
		{
			if(Src[i]!=Dst[i])
			{
				PRINTFE(("Access sdram inside 820 error.\r\n"));
				return MMD_RET_TEST_SRAM_ERR;   
			}   
		}       
		count+=1024;
	}	
}
#endif	
#if 0
	addr = 0x40000000;
	len = 256;
	for(i=0;i<len;i++)
		srcBuf[i]=i;
	Vm_WrSram(addr,srcBuf, len);
	Vm_RdSram(addr,dstBuf, len);
	for(i=0;i<len;i++)
	{
		if(srcBuf[i]!=dstBuf[i])
		{
			PRINTFE(("Access sdram inside 820 error.\r\n"));
			return MMD_RET_TEST_SRAM_ERR;   
		}   
	}       
	if(i==len)  
	        PRINTFN(("Access sdram inside 820 succeed.\r\n"));
#endif	
	return MMD_RET_NOERR;
}

/*-----------------------------------------------------------------------------
[FUNCTION NAME]
  MMD_SYS_Set_TriggerMode
[DESCRIPTION]
  Set device to host interrupt output mode
[ARGUMENT]
  NULL
[RETURN VALUE]
  Error code    
-----------------------------------------------------------------------------*/
void MMD_SYS_Set_TriggerMode( void )
{
	MMD_U32 regval;
	
	if(TRI_MODE==TRI_MODE_POSITIVE_PULSE)
	{
		regval = 0xf80f;
	}
	else if(TRI_MODE==TRI_MODE_NEGATIVE_PULSE)
	{
		regval = 0xfc0f;
	}
	else if(TRI_MODE==TRI_MODE_HIGH_LEVEL)
	{
		regval= 0xff0f;
	}
	else if(TRI_MODE==TRI_MODE_LOW_LEVEL)
	{
		regval= 0xf90f;
	}

	Vm_SetReg(0x60050000, regval);
	//regval = Vm_GetReg(0x60050000);
	//PRINTFN(("regval=0x%x\r\n",regval));
	
}

/***********************************************************************
  Function:       
  	MMD_SYS_Malloc
  Description:    
 	Alloc Vc0848 meory
  Input:
	MMD_U32 size:			memory size
  Return:         
  	Error code 
  History:        
      <author>  	<time>     	<desc>
      lizg    		08/05/07     	create this function  
***********************************************************************/
MMD_U32 MMD_SYS_Malloc(MMD_U32 size)
{
	MMD_MSG Msg;
	MMD_U32 timeout=1000;
	
	g_Ack_MinCmd = MIN_RESERVED;
	
	Msg.CMD=MAJ_SYS|MIN_SYS_MALLOC;
	Msg.OP1=size;
	Msg.OP2=0;
	MMD_BIU_Send_H2D_Cmd(&Msg);
	
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
		{
			MMD_Check_Irq_Staus_And_Wait(1);
		}
		else
			break;
		timeout--;
	}
	if(timeout)
	{
		//PRINTF(("The %dst MMD_SYS_Malloc(), addr is %x\n",++DeviceMallocTimes,g_Ack_OP1);
		return g_Ack_OP1;
	}
	else
		return NULL;
}

/***********************************************************************
  Function:       
  	MMD_SYS_Free
  Description:    
 	Release Vc0848 meory
  Input:
	MMD_U32 addr:			memory address
  Return:         
  	Error code 
  History:        
      <author>  	<time>     	<desc>
      lizg    		08/05/07     	create this function  
***********************************************************************/
MMD_U32 MMD_SYS_Free(MMD_U32 addr)
{
	MMD_MSG Msg;
	MMD_U32 timeout=1000;
	
	g_Ack_MinCmd = MIN_RESERVED;
	
	Msg.CMD=MAJ_SYS|MIN_SYS_FREE;
	Msg.OP1=addr;
	Msg.OP2=0;
	MMD_BIU_Send_H2D_Cmd(&Msg);
	
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
			{
			MMD_Check_Irq_Staus_And_Wait(1);
		}
		else
		{
			//PRINTF(("The %dst MMD_SYS_Free(), addr is %x\n",++DeviceFreeTimes,addr);
			break;
		}
			
		timeout--;
	}
	return MMD_RET_NOERR;
}


/***********************************************************************
  Function:       
  	MMD_SYS_MallocInfo
  Description:    
 	 show malloc information through VC0848 UART, debug function
  Input:
	NULL
  Return:         
  	Error code 
  History:        
      <author>  	<time>     	<desc>
      lizg    		08/05/07     	create this function  
***********************************************************************/
MMD_U32 MMD_SYS_MallocInfo(void)
{
	MMD_MSG Msg;
	
	Msg.CMD=MAJ_SYS|MIN_SYS_SHOW_MALLOC_INFO;
	Msg.OP1=0;
	Msg.OP2=0;
	MMD_BIU_Send_H2D_Cmd(&Msg);
	return MMD_RET_NOERR;
}

/***********************************************************************
  Function:       
  	MMD_SYS_MemPoolInfo
  Description:    
	get memory pool information 
  Input:
	MMD_U32 poll:		memory pool index, 
							0: large memory pool, 
							1: small memory pool
  Output:
	MMD_U32* pAddr:		memory pool start address
	MMD_U32* pSize:		memory pool total size
  Return:         
  	Error code 
  History:        
      <author>  	<time>     	<desc>
      lizg    		08/05/07     	create this function  
***********************************************************************/
MMD_U32 MMD_SYS_MemPoolInfo(MMD_U32 poll,MMD_U32* pAddr, MMD_U32* pSize)
{
	MMD_MSG Msg;
	MMD_U32 timeout=1000;

	g_Ack_MinCmd = MIN_RESERVED;
	
	Msg.CMD=MAJ_SYS|MIN_SYS_GET_MEM_POLL_INFO;
	Msg.OP1=poll;
	Msg.OP2=0;
	MMD_BIU_Send_H2D_Cmd(&Msg);

	while(timeout)
	{
		if(g_Ack_MinCmd!=MIN_SYS_GET_MEM_POLL_INFO)
			{
			MMD_Check_Irq_Staus_And_Wait(1);
		}
		else
			break;
		timeout--;
	}
	if(!timeout)
		return MMD_RET_TIMEOUT;
	
	*pAddr = g_Ack_OP1;
	*pSize = g_Ack_OP2;	
	return MMD_RET_NOERR;
}

/***********************************************************************
  Function:       
  	MMD_SYS_GetTimer2Value
  Description:    
	Get VC0848 timer2 count for caculate time
  Input:
  	NULL
  Return:         
  	Timer2 count
  History:        
      <author>  	<time>     	<desc>
      lizg    		08/05/07     	create this function  
***********************************************************************/
MMD_U32 MMD_SYS_GetTimer2Value(void)
{
	MMD_U32 temp;
	HAL_READ_UINT32(V8_REG_TIMER_TMR2DOUT, temp);	
	return temp;
}

/***********************************************************************
  Function:       
  	MMD_SYS_FillMem
  Description:    
	use 0xff to fill VC0848 memory, this function is used to fill video stream buffer
  Input:
  	MMD_U32 addr:			VC0848 memory fill address
  	MMD_U32 size:			VC0848 memory fill size
  Return:         
  	Error code 
  History:        
      <author>  	<time>     	<desc>
      lizg    		08/05/07     	create this function  
***********************************************************************/
MMD_U32 MMD_SYS_FillMem(MMD_U32 addr, MMD_U32 size)
{
	MMD_MSG Msg;
	MMD_U32 timeout=1000;
		
	g_Ack_MinCmd = MIN_RESERVED;
	
	Msg.CMD=MAJ_SYS|MIN_SYS_FILL_MEM;
	Msg.OP1=addr;
	Msg.OP2=size;
	MMD_BIU_Send_H2D_Cmd(&Msg);
	
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
		{
			MMD_Check_Irq_Staus_And_Wait(1);
		}
		else
			break;
		timeout--;
	}

	if(timeout)
		return MMD_RET_NOERR;
	else
		return MMD_RET_TIMEOUT;
}

/***********************************************************************
  Function:       
  	MMD_SYS_FillMemData
  Description:    
	use 0xff to fill VC0848 memory, this function is used to fill video stream buffer
  Input:
  	MMD_U32 addr:			VC0848 memory fill address
  	MMD_U32 size:			VC0848 memory fill size
  Return:         
  	Error code 
  History:        
      <author>  	<time>     	<desc>
      lizg    		08/05/07     	create this function  
***********************************************************************/
MMD_U32 MMD_SYS_FillMemData(MMD_U32 addr, MMD_U32 size,MMD_U32 data)
{
	MMD_MSG Msg;
	MMD_U32 timeout=1000;
	MMD_U32 OP2=0;
	
	g_Ack_MinCmd = MIN_RESERVED;
	OP2=((size&0xffffff)|(data<<24));
	
	Msg.CMD=MAJ_SYS|MIN_SYS_FILL_MEM_DATA;
	Msg.OP1=addr;
	//Msg.OP2=size;
	Msg.OP2=OP2;
	MMD_BIU_Send_H2D_Cmd(&Msg);
	
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
		{
			MMD_Check_Irq_Staus_And_Wait(1);
		}
		else
			break;
		timeout--;
	}

	if(timeout)
		return MMD_RET_NOERR;
	else
		return MMD_RET_TIMEOUT;
}


/***********************************************************************
  Function:       
  	MMD_SYS_SetMem
  Description:    
	use data to fill VC0848 memory
  Input:
  	MMD_U32 addr:			VC0848 memory fill address
  	MMD_U08 data:			VC0848 memory fill data
  Return:         
  	Error code 
  History:        
      <author>  	<time>     	<desc>
      dnn    		08/06/24     	create this function  
***********************************************************************/
MMD_U32 MMD_SYS_SetMem(MMD_U32 addr, MMD_U08 data)
{
	MMD_MSG Msg;
	MMD_U32 timeout=1000;
	
	g_Ack_MinCmd = MIN_RESERVED;
	
	Msg.CMD=MAJ_SYS|MIN_SYS_SET_MEM;
	Msg.OP1=addr;
	Msg.OP2=data;
	MMD_BIU_Send_H2D_Cmd(&Msg);
	
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
		{
			MMD_Check_Irq_Staus_And_Wait(1);
		}
		else
			break;
		timeout--;
	}

	if(timeout)
		return MMD_RET_NOERR;
	else
		return MMD_RET_TIMEOUT;
}

/***********************************************************************
  Function:       
  	MMD_SYS_Scene_Info_Init
  Description:    
	Initial scence code infomation, start address and length
  Input:
  	NULL
  Return:         
  	Error code 
  History:        
      <author>  	<time>     	<desc>
      lizg    		08/05/07     	create this function  
***********************************************************************/
MMD_U32 MMD_SYS_Scene_Info_Init(void)
{
#if CORE_RELEASE

	gSceneCode[SCENE_USB].Scene_type =(('u')|('d'<<8)|('s'<<16)|('k'<<24));
	gSceneCode[SCENE_USB].StartAddr = V8XXCORE_USB_START;
	gSceneCode[SCENE_USB].CodeLen = sizeof(V8XXCORE_USB_START);

	gSceneCode[SCENE_REC_AMR].Scene_type = (('s')|('a'<<8)|('m'<<16)|('r'<<24));
	gSceneCode[SCENE_REC_AMR].StartAddr = V8XXCORE_MEDIAREC_START;
	gSceneCode[SCENE_REC_AMR].CodeLen = sizeof(V8XXCORE_MEDIAREC_START);

	gSceneCode[SCENE_REC_MP3].Scene_type = (('m')|('p'<<8)|('3'<<16)|('.'<<24));
	gSceneCode[SCENE_REC_MP3].StartAddr = V8XXCORE_MP3REC_START;
	gSceneCode[SCENE_REC_MP3].CodeLen = sizeof(V8XXCORE_MP3REC_START);

	gSceneCode[SCENE_MEDIAPLAY].Scene_type =(('m')|('p'<<8)|('l'<<16)|('y'<<24));
	gSceneCode[SCENE_MEDIAPLAY].StartAddr = SRAM_EXEC_ROM_START;
	gSceneCode[SCENE_MEDIAPLAY].CodeLen = sizeof(SRAM_EXEC_ROM_START);

#ifdef CMMB_ENABLE
	gSceneCode[SCENE_CMMB].Scene_type =(('c')|('m'<<8)|('m'<<16)|('b'<<24));
	gSceneCode[SCENE_CMMB].StartAddr = V8XXCORE_CMMB_START;
	gSceneCode[SCENE_CMMB].CodeLen = sizeof(V8XXCORE_CMMB_START);
#endif

#ifdef RV_ENABLE
	// RV overlay code 
	gSceneCode[SCENE_RV_OVERLAY].Scene_type =(('r')|('m'<<8)|('o'<<16)|('v'<<24));
	gSceneCode[SCENE_RV_OVERLAY].StartAddr = VC08XX_CODE_RV_SDRAM_START;
	gSceneCode[SCENE_RV_OVERLAY].CodeLen = VC08XX_CODE_RV_SDRAM_END -VC08XX_CODE_RV_SDRAM_START;

	// RV overlay codec
	gRVCodecCode[RV_CODEC_RV9].RORW_Load_base = (MMD_U32)VC08XX_CODE_RV9_DEC_START;
	gRVCodecCode[RV_CODEC_RV9].RORW_Image_base = 0x20001600;
	gRVCodecCode[RV_CODEC_RV9].RORW_Image_limit = gRVCodecCode[RV_CODEC_RV9].RORW_Image_base+(VC08XX_CODE_RV9_DEC_END-VC08XX_CODE_RV9_DEC_START);
	gRVCodecCode[RV_CODEC_RV9].ZI_Image_base = gRVCodecCode[RV_CODEC_RV9].RORW_Image_limit;
	gRVCodecCode[RV_CODEC_RV9].ZI_Image_limit = 0x20050000;

	gRVCodecCode[RV_CODEC_RV8].RORW_Load_base = (MMD_U32)VC08XX_CODE_RV8_DEC_START;
	gRVCodecCode[RV_CODEC_RV8].RORW_Image_base = 0x20001600;
	gRVCodecCode[RV_CODEC_RV8].RORW_Image_limit = gRVCodecCode[RV_CODEC_RV8].RORW_Image_base+(VC08XX_CODE_RV9_DEC_END-VC08XX_CODE_RV9_DEC_START);
	gRVCodecCode[RV_CODEC_RV8].ZI_Image_base = gRVCodecCode[RV_CODEC_RV8].RORW_Image_limit;
	gRVCodecCode[RV_CODEC_RV8].ZI_Image_limit = 0x20050000;

	gRVCodecCode[RV_CODEC_RVG2].RORW_Load_base = (MMD_U32)VC08XX_CODE_RVG2_DEC_START;
	gRVCodecCode[RV_CODEC_RVG2].RORW_Image_base = 0x20001600;
	gRVCodecCode[RV_CODEC_RVG2].RORW_Image_limit = gRVCodecCode[RV_CODEC_RVG2].RORW_Image_base+(VC08XX_CODE_RV9_DEC_END-VC08XX_CODE_RV9_DEC_START);
	gRVCodecCode[RV_CODEC_RVG2].ZI_Image_base = gRVCodecCode[RV_CODEC_RVG2].RORW_Image_limit;
	gRVCodecCode[RV_CODEC_RVG2].ZI_Image_limit = 0x20050000;
#endif

#ifdef RV20_ENABLE
	gSceneCode[SCENE_RV20].Scene_type =(('r')|('v'<<8)|('2'<<16)|('0'<<24));
	gSceneCode[SCENE_RV20].StartAddr = RV20_EXEC_START;
	gSceneCode[SCENE_RV20].CodeLen = sizeof(RV20_EXEC_START);
#endif

#else

	gSceneCode[SCENE_USB].Scene_type =(('u')|('d'<<8)|('s'<<16)|('k'<<24));
	gSceneCode[SCENE_USB].StartAddr = V8XXCORE_USB_START;
	gSceneCode[SCENE_USB].CodeLen = V8XXCORE_USB_END -V8XXCORE_USB_START;

	gSceneCode[SCENE_REC_AMR].Scene_type = (('s')|('a'<<8)|('m'<<16)|('r'<<24));
	gSceneCode[SCENE_REC_AMR].StartAddr = V8XXCORE_MEDIAREC_START;
	gSceneCode[SCENE_REC_AMR].CodeLen = V8XXCORE_MEDIAREC_END -V8XXCORE_MEDIAREC_START;

	gSceneCode[SCENE_REC_MP3].Scene_type = (('m')|('p'<<8)|('3'<<16)|('.'<<24));
	gSceneCode[SCENE_REC_MP3].StartAddr = V8XXCORE_MP3REC_START;
	gSceneCode[SCENE_REC_MP3].CodeLen = V8XXCORE_MP3REC_END -V8XXCORE_MP3REC_START;
	
	gSceneCode[SCENE_MEDIAPLAY].Scene_type =(('m')|('p'<<8)|('l'<<16)|('y'<<24));
	gSceneCode[SCENE_MEDIAPLAY].StartAddr = SRAM_EXEC_ROM_START;
	gSceneCode[SCENE_MEDIAPLAY].CodeLen = SRAM_EXEC_ROM_END -SRAM_EXEC_ROM_START;

	gSceneCode[SCENE_CMMB].Scene_type =(('c')|('m'<<8)|('m'<<16)|('b'<<24));
	gSceneCode[SCENE_CMMB].StartAddr = V8XXCORE_CMMB_START;
	gSceneCode[SCENE_CMMB].CodeLen = V8XXCORE_CMMB_END -V8XXCORE_CMMB_START;

#ifdef RV_ENABLE
	// RV overlay code 
	gSceneCode[SCENE_RV_OVERLAY].Scene_type =(('r')|('m'<<8)|('o'<<16)|('v'<<24));
	gSceneCode[SCENE_RV_OVERLAY].StartAddr = RV_SDRAM_EXEC_ROM_START;
	gSceneCode[SCENE_RV_OVERLAY].CodeLen = RV_SDRAM_EXEC_ROM_END -RV_SDRAM_EXEC_ROM_START;

	// RV overlay codec
	gRVCodecCode[RV_CODEC_RV9].RORW_Load_base = (MMD_U32)RV9_EXEC_START;
	gRVCodecCode[RV_CODEC_RV9].RORW_Image_base = 0x20001600;
	gRVCodecCode[RV_CODEC_RV9].RORW_Image_limit = gRVCodecCode[RV_CODEC_RV9].RORW_Image_base+(RV9_EXEC_END-RV9_EXEC_START);
	gRVCodecCode[RV_CODEC_RV9].ZI_Image_base = gRVCodecCode[RV_CODEC_RV9].RORW_Image_limit;
	gRVCodecCode[RV_CODEC_RV9].ZI_Image_limit = 0x20050000;

	gRVCodecCode[RV_CODEC_RV8].RORW_Load_base = (MMD_U32)RV8_EXEC_START;
	gRVCodecCode[RV_CODEC_RV8].RORW_Image_base = 0x20001600;
	gRVCodecCode[RV_CODEC_RV8].RORW_Image_limit = gRVCodecCode[RV_CODEC_RV8].RORW_Image_base+(RV8_EXEC_END-RV8_EXEC_START);
	gRVCodecCode[RV_CODEC_RV8].ZI_Image_base = gRVCodecCode[RV_CODEC_RV8].RORW_Image_limit;
	gRVCodecCode[RV_CODEC_RV8].ZI_Image_limit = 0x20050000;

	gRVCodecCode[RV_CODEC_RVG2].RORW_Load_base = (MMD_U32)RVG2_EXEC_START;
	gRVCodecCode[RV_CODEC_RVG2].RORW_Image_base = 0x20001600;
	gRVCodecCode[RV_CODEC_RVG2].RORW_Image_limit = gRVCodecCode[RV_CODEC_RVG2].RORW_Image_base+(RVG2_EXEC_END-RVG2_EXEC_START);
	gRVCodecCode[RV_CODEC_RVG2].ZI_Image_base = gRVCodecCode[RV_CODEC_RVG2].RORW_Image_limit;
	gRVCodecCode[RV_CODEC_RVG2].ZI_Image_limit = 0x20050000;
#endif

#ifdef RV20_ENABLE
	gSceneCode[SCENE_RV20].Scene_type =(('r')|('v'<<8)|('2'<<16)|('0'<<24));
	gSceneCode[SCENE_RV20].StartAddr = RV20_EXEC_START;
	gSceneCode[SCENE_RV20].CodeLen = RV20_EXEC_END -RV20_EXEC_START;
#endif

#endif

	return MMD_RET_NOERR;
}


/***********************************************************************
  Function:       
  	MMD_SYS_Scene_Info_Init
  Description:    
	Initial scence code infomation, start address and length
  Input:
  	NULL
  Return:         
  	Error code 
  History:        
      <author>  	<time>     	<desc>
      lizg    		08/05/07     	create this function  
***********************************************************************/
MMD_U32 MMD_SYS_Codec_Info_Init(void)
{
	MMD_MSG Msg;
	MMD_U32 timeout=1000;
	
	g_Ack_MinCmd = MIN_RESERVED;
	
	Msg.CMD=MAJ_SYS|MIN_SYS_GET_CODE_MAP;
	Msg.OP1=0;
	Msg.OP2=0;
	MMD_BIU_Send_H2D_Cmd(&Msg);
	
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
			{
		//printf("MMD_SYS_Codec_Info_Init\n");
			MMD_Check_Irq_Staus_And_Wait(1);
		}
		else
			break;
		timeout--;
	}

	if(timeout==0)
		return MMD_RET_TIMEOUT;

	Vm_RdSram(g_Ack_OP1, (MMD_S08*)gCodecCode, sizeof(gCodecCode));
	// no need  free here for memory has not create

	return MMD_RET_NOERR;
}

/*-----------------------------------------------------------------------------
[FUNCTION NAME]
  MMD_SYS_GetVersion
[DESCRIPTION]
  Use this routine to return VC0848 software version
[ARGUMENT]
  pHostVersion: buffer used to save host software version.
  pCoreVersion: buffer used to save 820 software version.
  				scene idx: high 16 bit
  				scene version : low 16 bit
[RETURN VALUE]
  MMD_RET_NOERR        : Success
  MMD_RET_ERR          : Fail
-----------------------------------------------------------------------------*/
MMD_U32 MMD_SYS_GetVersion(MMD_U32* pHostVersion, MMD_U32* pCoreVersion)
{
	MMD_MSG Msg;
	MMD_U32 timeout=1000;
	
	g_Ack_MinCmd = MIN_RESERVED;
	
	Msg.CMD=MAJ_SYS|MIN_SYS_GETCOREVERSION;
	MMD_BIU_Send_H2D_Cmd(&Msg);
	*pHostVersion= g_HostVersino;
	*pCoreVersion = 0;
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
			{
			MMD_Check_Irq_Staus_And_Wait(1);
		}
		else
			break;
		timeout--;
	}

	if(timeout)
	{
		* pCoreVersion=(g_Ack_OP1<<16)|g_Ack_OP2;
		return MMD_RET_NOERR;
	}	
	else
		return MMD_RET_TIMEOUT;
}



/*-----------------------------------------------------------------------------
[FUNCTION NAME]
  MMD_SYS_Bypass_Set
[DESCRIPTION]
  Bypass mode set function
[ARGUMENT]
  flag:	
	MMD_CHIP_MODE_BYPASS: bypass on, VC0848 in bypass mode
	MMD_CHIP_MODE_NORMAL: bypass off, VC0848 in normal mode
[RETURN VALUE]
  MMD_RET_NOERR: success
  MMD_RET_ERR:    fail
-----------------------------------------------------------------------------*/
MMD_U32 MMD_SYS_Bypass_Set(MMD_U32 flag)
{
	MMD_USER_SetChipMode(flag);
	return MMD_RET_NOERR;
}

void MMD_Set_SysCallback(SYSCallback Callback)
{
	if(Callback)
		g_SysCallback = Callback;
}

MMD_U32 MMD_ERROR_Deal(MMD_U32 errorNO,MMD_U32 sysNO)
{
	MMD_MSG Msg;
	MMD_U32 timeout=1000,ret=0,coreNO=0,TestRm=0;
	printf("MMD_ERROR_Deal errorno(%d) \n",errorNO);
	g_Ack_MinCmd = MIN_RESERVED;
	switch(errorNO)
	{
		case CMD_FAILED:
		case COLSE_FILE_FAILED:
			Msg.CMD=sysNO|CHACK_CORE;
			Msg.OP1=0;
			MMD_BIU_Send_H2D_Cmd(&Msg);
			
			while(timeout)
			{
				if(g_Ack_MinCmd == MIN_RESERVED)
				{
					MMD_Check_Irq_Staus_And_Wait(1);
				}
				else
				{
					break;
				}
				timeout--;
			}
			if(timeout)
			{
				TestRm = MMD_SYS_Malloc(0x1000);
				if(!TestRm)
					timeout = 0;
			}
			if(!timeout)
			{
				switch(sysNO)
				{
					case MAJ_PLY:
						coreNO = SCENE_MEDIAPLAY;
						break;
					case MAJ_USB:
						coreNO = SCENE_USB;
						break;
					case MAJ_CMMB:
						coreNO = SCENE_CMMB;
						break;
					case MAJ_REC:
						coreNO = SCENE_REC_AMR;
						break;
					case MAJ_RV:
						coreNO = SCENE_RV_OVERLAY;
						break;
					case MAJ_FIL:
						coreNO = SCENE_MEDIAPLAY;
						break;
				}
				
				MMD_LCD_BLY_Cast();
				g_SceneIdxInDevice = 5;
				ret = MMD_SYS_DownCore(coreNO, 1);
				MMD_USER_WaitMICRS(1000);
				MMD_SYS_Codec_Switch(CODEC_DEFAULT);
				MMD_USER_WaitMICRS(1000);
				/*if(g_SysCallback)
				{
					(*g_SysCallback)(coreNO);
				}*/
			}
			break;
	}
	return ret;
}

/*-----------------------------------------------------------------------------
[FUNCTION NAME]
  MMD_SYS_SetCKD
[DESCRIPTION]
  ckd set function
[ARGUMENT]
  flag:	
	ckd
	enable
	bypass
[RETURN VALUE]
       no
-----------------------------------------------------------------------------*/
void MMD_SYS_SetCKDCtrl(MMD_CKD ckd,MMD_U08 enable,MMD_U08 bypass)
{
	MMD_U32 val;
	val=Vm_GetReg(V5_REG_SYS_CKDENCTRL);
	val &=(~(1<<ckd));
	val |=(enable<<ckd);
	Vm_SetReg(V5_REG_SYS_CKDENCTRL,val);
	val=Vm_GetReg(V5_REG_SYS_CKDBPCTRL);
	val &=(~(1<<ckd));
	val |=(bypass<<ckd);
	Vm_SetReg(V5_REG_SYS_CKDBPCTRL,val);	
}
/*-----------------------------------------------------------------------------
[FUNCTION NAME]
  MMD_SYS_SetModeClkGate
[DESCRIPTION]
  clk set function
[ARGUMENT]
  flag:	
	clk_gate
[RETURN VALUE]

-----------------------------------------------------------------------------*/
void MMD_SYS_SetClkGate(MMD_CLKGATE clkgate, MMD_U32 gate_flag)
{
	MMD_U32 val;
	if(clkgate<32)
	{
		val=Vm_GetReg(V5_REG_SYS_CGTMCTRL);
		val &=(~(1<<clkgate));
		val |=(gate_flag<<clkgate);
		Vm_SetReg(V5_REG_SYS_CGTMCTRL,val);
	}
	#if 1
	else
	{

		val=Vm_GetReg(V5_REG_SYS_SYSCLKCTRL);
		val &=(~(1<<(clkgate-32)));
		if((clkgate==CLKGATE_XCLK1)||(clkgate==CLKGATE_RTC))
		{
		   val |=((!gate_flag)<<(clkgate-32));
		}
		else
		{
		   val |=(gate_flag<<(clkgate-32));
		}
		Vm_SetReg(V5_REG_SYS_SYSCLKCTRL,val);
	}
	#endif
}

/*-----------------------------------------------------------------------------
[FUNCTION NAME]
  MMD_SYS_SetModeClkGate
[DESCRIPTION]
  clk set function
[ARGUMENT]
  flag:	
	clk_gate
[RETURN VALUE]

-----------------------------------------------------------------------------*/
void MMD_SYS_SetModeClkGate(CLKGATE_MODE mode)
{
	switch(mode)
	{
		case CLKGATE_MODE_NORMAL:
	        MMD_SYS_SetClkGate(CLKGATE_TIM,0);	
	        MMD_SYS_SetClkGate(CLKGATE_KPD,0);
	        MMD_SYS_SetClkGate(CLKGATE_UART,0);
	        MMD_SYS_SetClkGate(CLKGATE_SPI,0);
	        MMD_SYS_SetClkGate(CLKGATE_PAD,0);
	        MMD_SYS_SetClkGate(CLKGATE_I2C,0);
	        MMD_SYS_SetClkGate(CLKGATE_USB,0);
	        MMD_SYS_SetClkGate(CLKGATE_LCD,0);
	        MMD_SYS_SetClkGate(CLKGATE_TV,0);
	        MMD_SYS_SetClkGate(CLKGATE_I2S,0);
	        MMD_SYS_SetClkGate(CLKGATE_NFC,0);
	        MMD_SYS_SetClkGate(CLKGATE_VDEC,0);
	        MMD_SYS_SetClkGate(CLKGATE_VENC,0);
	        MMD_SYS_SetClkGate(CLKGATE_GE,0);
	        MMD_SYS_SetClkGate(CLKGATE_ISP,0);
	        MMD_SYS_SetClkGate(CLKGATE_JPEG,0);
	        MMD_SYS_SetClkGate(CLKGATE_LBUF,0);
	        MMD_SYS_SetClkGate(CLKGATE_IPP,0);
	        MMD_SYS_SetClkGate(CLKGATE_SIF,0);
	        MMD_SYS_SetClkGate(CLKGATE_SDIO,0);
	        MMD_SYS_SetClkGate(CLKGATE_SDRC,0);
	        MMD_SYS_SetClkGate(CLKGATE_DMAC,0);
	        MMD_SYS_SetClkGate(CLKGATE_LED,0);
	     #if 1
	        /* system clk gate */
	        MMD_SYS_SetClkGate(CLKGATE_BIU,0);
	        MMD_SYS_SetClkGate(CLKGATE_MARB,0);
	        MMD_SYS_SetClkGate(CLKGATE_AHB,0);
	        MMD_SYS_SetClkGate(CLKGATE_CPU,0);
	        MMD_SYS_SetClkGate(CLKGATE_RTC,0);
	        MMD_SYS_SetClkGate(CLKGATE_XCLK1,0);
	     #endif
			break;


		case CLKGATE_MODE_BYPASS:
			MMD_SYS_SetClkGate(CLKGATE_TIM,1);	
			MMD_SYS_SetClkGate(CLKGATE_KPD,1);
			MMD_SYS_SetClkGate(CLKGATE_UART,1);
			MMD_SYS_SetClkGate(CLKGATE_SPI,1);
			MMD_SYS_SetClkGate(CLKGATE_PAD,1);
			MMD_SYS_SetClkGate(CLKGATE_I2C,1);
			MMD_SYS_SetClkGate(CLKGATE_USB,1);
			MMD_SYS_SetClkGate(CLKGATE_LCD,1);
	        MMD_SYS_SetClkGate(CLKGATE_TV,1);
	        MMD_SYS_SetClkGate(CLKGATE_I2S,1);
	        MMD_SYS_SetClkGate(CLKGATE_NFC,1);
	        MMD_SYS_SetClkGate(CLKGATE_VDEC,1);
	        MMD_SYS_SetClkGate(CLKGATE_VENC,1);
	        MMD_SYS_SetClkGate(CLKGATE_GE,1);
	        MMD_SYS_SetClkGate(CLKGATE_ISP,1);
	        MMD_SYS_SetClkGate(CLKGATE_JPEG,1);
	        MMD_SYS_SetClkGate(CLKGATE_LBUF,1);
	        MMD_SYS_SetClkGate(CLKGATE_IPP,1);
	        MMD_SYS_SetClkGate(CLKGATE_SIF,1);
	        MMD_SYS_SetClkGate(CLKGATE_SDIO,1);
	        MMD_SYS_SetClkGate(CLKGATE_SDRC,1);
	        MMD_SYS_SetClkGate(CLKGATE_DMAC,1);
	        MMD_SYS_SetClkGate(CLKGATE_LED,1);
	     #if 1
	        /* system clk gate */
	        MMD_SYS_SetClkGate(CLKGATE_BIU,0);
	        MMD_SYS_SetClkGate(CLKGATE_MARB,0);
	        MMD_SYS_SetClkGate(CLKGATE_AHB,0);
	        MMD_SYS_SetClkGate(CLKGATE_CPU,0);
	        MMD_SYS_SetClkGate(CLKGATE_RTC,0);
			MMD_SYS_SetClkGate(CLKGATE_XCLK1,0);
#endif
			break;		
		case CLKGATE_MODE_CODECONLY:
#if 1	
			MMD_SYS_SetClkGate(CLKGATE_TIM,1);	
			MMD_SYS_SetClkGate(CLKGATE_KPD,1);
			MMD_SYS_SetClkGate(CLKGATE_UART,0);
			MMD_SYS_SetClkGate(CLKGATE_SPI,1);
			MMD_SYS_SetClkGate(CLKGATE_PAD,1);
			MMD_SYS_SetClkGate(CLKGATE_I2C,1);

			MMD_SYS_SetClkGate(CLKGATE_USB,1);
			MMD_SYS_SetClkGate(CLKGATE_LCD,0);
			MMD_SYS_SetClkGate(CLKGATE_TV,1);					
			MMD_SYS_SetClkGate(CLKGATE_I2S,0);
			MMD_SYS_SetClkGate(CLKGATE_NFC,1);
			MMD_SYS_SetClkGate(CLKGATE_VDEC,1);
			MMD_SYS_SetClkGate(CLKGATE_VENC,1);
			MMD_SYS_SetClkGate(CLKGATE_GE,1);
			MMD_SYS_SetClkGate(CLKGATE_ISP,1);
			MMD_SYS_SetClkGate(CLKGATE_JPEG,1);
			MMD_SYS_SetClkGate(CLKGATE_LBUF,1);
			MMD_SYS_SetClkGate(CLKGATE_IPP,1);
			MMD_SYS_SetClkGate(CLKGATE_SIF,1);
			MMD_SYS_SetClkGate(CLKGATE_SDIO,1);
			MMD_SYS_SetClkGate(CLKGATE_SDRC,1);
			MMD_SYS_SetClkGate(CLKGATE_DMAC,1);
			MMD_SYS_SetClkGate(CLKGATE_LED,1);
#endif
#if 1
			/* system clk gate */
			MMD_SYS_SetClkGate(CLKGATE_BIU,0);					
			MMD_SYS_SetClkGate(CLKGATE_MARB,0);
			MMD_SYS_SetClkGate(CLKGATE_AHB,0);
			MMD_SYS_SetClkGate(CLKGATE_CPU,0);
			MMD_SYS_SetClkGate(CLKGATE_RTC,0);
			MMD_SYS_SetClkGate(CLKGATE_XCLK1,0);

#endif
			break;	


		default:
		break;
	}
}
MMD_U32 MMD_SYS_SetModeClk(MMD_SYS_CLK sys_clk)
{
	#if 0
	if(sys_clk==g_sys_clk)
	{
		MMD_PRINTF(("The freq have been already set \n"));
		return 0;
	}
	
	else
	#endif
	{
		if(sys_clk==CLK_CPU12M_BUS12M)
		{
			MMD_USER_BiuTiming_Switch(1);
			MMD_SYS_SetClk(sys_clk); 
			g_sys_clk=sys_clk;
		}
		else
		{
			MMD_SYS_SetClk(sys_clk); 
			g_sys_clk=sys_clk;
			MMD_USER_BiuTiming_Switch(0);
		}
		return 0;
	}
	
	
}
/*-----------------------------------------------------------------------------
[FUNCTION NAME]
  MMD_SYS_SetClk
[DESCRIPTION]
  Set Clk
[ARGUMENT]
  flag:	
MMD_SYS_CLK
[RETURN VALUE]
  MMD_RET_NOERR: success
  MMD_RET_ERR:    fail
  MMD_RET_TIMEOUT: time out
-----------------------------------------------------------------------------*/
MMD_U32 MMD_SYS_SetClk(MMD_SYS_CLK sys_clk)
{
	MMD_SYS_FREQ Freq,sFreq;
	MMD_U32 addr;
	MMD_U16 buf[12]={0};
	{
		MMD_MSG Msg;
		MMD_U32 timeout=1000;
		addr=MMD_SYS_Malloc(24); /*sizeof(freq)*/
		Msg.CMD=MAJ_SYS|MIN_SYS_SET_CLK;
		Msg.OP1=sys_clk;
		Msg.OP2=addr;	
		MMD_BIU_Send_H2D_Cmd(&Msg);                               
		g_Ack_MinCmd = MIN_RESERVED;
		while(timeout)
		{
			if(g_Ack_MinCmd == MIN_RESERVED)
				MMD_Check_Irq_Staus_And_Wait(1);
			else
				break;
			timeout--;
		}

		if(timeout)
		{
			Vm_RdSram(addr,(MMD_S08 *)buf,24);

			sFreq.cpu=buf[0];
			sFreq.bus=buf[1];
			sFreq.vid=buf[2];
			sFreq.mpeg=buf[3];
			sFreq.peri=buf[4];
			sFreq.nand=buf[5];
			sFreq.tv=buf[6];
			sFreq.lcd=buf[7];
			sFreq.snr=buf[8];
			sFreq.phy=buf[9];
			sFreq.lvds=buf[10];
			
			MMD_PRINTF(("set cpu=%d,set bus=%d,set vid=%d,set mpeg=%d,set peri=%d,set nand=%d,set tv=%d,set lcd=%d,set snr=%d,set phy=%d,set lvds=%d\n",
				                  sFreq.cpu,sFreq.bus,sFreq.vid,sFreq.mpeg,sFreq.peri,sFreq.nand,sFreq.tv,sFreq.lcd,sFreq.snr,sFreq.phy,sFreq.lvds));
			
			MMD_SYS_GetClk(&Freq);					 

			if(   (sFreq.cpu==Freq.cpu)&&(sFreq.bus==Freq.bus)&&(sFreq.vid==Freq.vid)&&(sFreq.mpeg==Freq.mpeg)&&(sFreq.peri==Freq.peri)&&(sFreq.nand==Freq.nand)
				&&(sFreq.tv==Freq.tv)&&(sFreq.lcd==Freq.lcd)&&(sFreq.snr==Freq.snr)&&(sFreq.phy==Freq.phy)&&(sFreq.lvds==Freq.lvds)	)
			{
				MMD_PRINTF(("freq set succeed!\n"));
				MMD_SYS_Free(addr);
				return MMD_RET_NOERR;				   
			}
			else
			{
				MMD_PRINTF(("freq set no succeed!\n"));
				MMD_SYS_Free(addr);   
				return MMD_RET_ERR;			
			}
		}	
		else
		{

			MMD_PRINTF(("set freq time out!\n"));
			MMD_SYS_Free(addr);
			return MMD_RET_TIMEOUT;				   
		}  
	}
}
/*-----------------------------------------------------------------------------
[FUNCTION NAME]
  MMD_SYS_GetClk
[DESCRIPTION]
  Get clk
[ARGUMENT]
  flag:	
	PVIM_MODULE_FREQ_SET pFreq
[RETURN VALUE]
  
  MMD_RET_NOERR: success
  MMD_RET_TIMEOUT: time out
-----------------------------------------------------------------------------*/
MMD_U32 MMD_SYS_GetClk(MMD_SYS_FREQ *pFreq)
{

	MMD_MSG Msg;

	MMD_U32 timeout=1000;	
	MMD_U32 addr;
	MMD_U16 buf[12];
	addr=MMD_SYS_Malloc(24); /*sizeof(freq)*/
	Msg.CMD=MAJ_SYS|MIN_SYS_GET_CLK;
	Msg.OP1=addr;
	Msg.OP2=0;	
	MMD_BIU_Send_H2D_Cmd(&Msg);                               
	g_Ack_MinCmd = MIN_RESERVED;
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
			MMD_Check_Irq_Staus_And_Wait(1);
		else
			break;
		timeout--;
	}

	if(timeout)
	{
		Vm_RdSram(addr,(MMD_S08 *)buf,24);

		pFreq->cpu=buf[0];
		pFreq->bus=buf[1];
		pFreq->vid=buf[2];
		pFreq->mpeg=buf[3];
		pFreq->peri=buf[4];
		pFreq->nand=buf[5];
		pFreq->tv=buf[6];
		pFreq->lcd=buf[7];
		pFreq->snr=buf[8];
		pFreq->phy=buf[9];
		pFreq->lvds=buf[10];

		//MMD_PRINTF(("get cpu=%d,get bus=%d,get vid=%d,get mpeg=%d,get peri=%d,get nand=%d,get tv=%d,get lcd=%d,get snr=%d,get phy=%d,get lvds=%d\n",
		//			    pFreq->cpu,pFreq->bus,pFreq->vid,pFreq->mpeg,pFreq->peri,pFreq->nand,pFreq->tv,pFreq->lcd,pFreq->snr,pFreq->phy,pFreq->lvds));

		MMD_PRINTF(("freq get succeed!\n"));
		MMD_SYS_Free(addr);
		return MMD_RET_NOERR;				   
                    
	}	
	else
	{
		MMD_PRINTF(("get freq time out!\n"));
		MMD_SYS_Free(addr);
		return MMD_RET_TIMEOUT;				   
	}  
}
/*                                                                                              
  * PMU Set PS  mode .                                                                                          
  * Return:                                                                                                                                               
  *           MMD_SET_PSMODE_NOERR              : OK                                                                                  
  *           MMD_SET_PSMODE_INVALIDPARAM  : Parameter Error of Argument 
  */
MMD_U32 MMD_SYS_SetPSMode(MMD_PS_MODE mode)   /* (IN)  Power Save Mode  */
{

	MMD_MSG Msg;     
	MMD_U32 timeout=3000;	


		Msg.CMD=MAJ_SYS|MIN_SYS_PS_MODE;
		Msg.OP1=mode;
		Msg.OP2=0;	
		MMD_BIU_Send_H2D_Cmd(&Msg);                               
	g_Ack_MinCmd = MIN_RESERVED;
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
			MMD_USER_WaitMICRS(1);
		else
			break;
		timeout--;
	} 
	if(timeout)
	{
		PRINTF(("ps mode set Fail!\n"));
		return MMD_RET_ERR;  
	}
	else
	{
		PRINTF(("ps mode set Succeed!\n"));
		return MMD_RET_NOERR;	
	}
		
}
MMD_U32 MMD_SYS_Wakeup(MMD_PS_MODE mode)
{
	switch (mode)
	{
		case MMD_PS_MODE_NORMAL:
			break;

		 case MMD_PS_MODE_IDLE:
				MMD_USER_Wakeup();
				//Vm_SetReg(V5_REG_SYS_CLKSWITCH, 0x00000000);     // switch ARM clock to pll clock
				Vm_SetReg(V5_REG_INT_INTCRTL,Vm_GetReg(V5_REG_INT_INTCRTL)|0x01);
			break;

		case MMD_PS_MODE_LITESLEEP:
			MMD_USER_Wakeup();	
			g_biu_width=0;              
			MMD_BIU_Init(1);
			MMD_SYS_Set_TriggerMode();
			g_SceneIdxInDevice=SCENE_NUM;
			g_CodecIdxInDevice=CODEC_NUM;
			break;

		case MMD_PS_MODE_DEEPSLEEP:
			MMD_USER_Wakeup();	
			g_biu_width=0;              
			MMD_BIU_Init(1);
			MMD_SYS_Set_TriggerMode();
			g_SceneIdxInDevice=SCENE_NUM;
			g_CodecIdxInDevice=CODEC_NUM;
			break;

		default:
				break;
	}
	return 0;
}

/*                                                                                              
  * Set CODEC ONLY  mode .                                                                                          
  * Return:                                                                                                                                               
  *           MMD_SET_PSMODE_NOERR              : OK                                                                                  
  *           MMD_SET_PSMODE_INVALIDPARAM  : Parameter Error of Argument 
  */
MMD_U32 MMD_SYS_SetCodecOnly(void)
{
       MMD_SYS_PMU_SWITCH(PMU_MODE_NORMAL);
	MMD_SYS_Mode_Switch(SCENE_DEFAULT);
	MMD_SYS_Codec_Switch(CODEC_DEFAULT);   
	MMD_Media_Codec_Set_MuxMode(0x01010101);
	MMD_SYS_SetModeClk(CLK_CPU12M_BUS12M);
	//MMD_SYS_SetModeClkGate(CLKGATE_MODE_CODECONLY);

	/*** Should  turn off all othlers powers(NF,CS,USB,VDAC .etc) here to save power by customer self ***/
	
	//MMD_USER_Power_Control(POWER_NF_CS,0); 
	//MMD_USER_Power_Control(POWER_SDRAM,1); 
	//MMD_USER_Power_Control(POWER_USB,0);
	return 0;	
}



void MMD_SYS_PMU_SWITCH(MMD_PMU_MODE pmu_mode)
{
	 //modify 20081222 start
	 MMD_U32 ret;
    extern void clkrgm_gpio30_clk_control(boolean bEnable);
    FARF(MSG,("MMD_SYS_PMU_SWITCH=%d,%d",last_pmu_mode, pmu_mode));
	//:	last_pmu_mode initialise as MMD_CHIP_MODE_NORMAL
	if(pmu_mode==last_pmu_mode)
	{
		return ;
	}       
	switch(pmu_mode)
	{
		case PMU_MODE_NORMAL: 
            VC_Sleep_Forbid();
			VC_GPIO_CTRL(51, HI); // now GPIO_51 is used to control LDO V1.2V for VC0848 VDD_CORE, power on for VDD_CORE
			MMD_USER_WaitMICRS(50);  //zhangxiang mod 09/01/14
			//MMD_USER_WaitMICRS(2000);
			clkrgm_gpio30_clk_control(TRUE);
			MMD_USER_WaitMICRS(50);      //zhangxiang mod 09/01/14
			//MMD_USER_WaitMICRS(2000);
			MMD_USER_SetChipMode(MMD_CHIP_MODE_NORMAL);        
			if(last_pmu_mode==PMU_MODE_BYPASS)	//	If switch from bypass mode
			{  
				MMD_USER_Wakeup();	
				g_biu_width=0;              
				#ifdef MMD_BIU_DATABUS16BIT
					MMD_BIU_Init(1);
				#else
					MMD_BIU_Init(0);  
				#endif
				MMD_SYS_Set_TriggerMode();
				g_SceneIdxInDevice=SCENE_NUM;
				g_CodecIdxInDevice=CODEC_NUM;
			       
				MMD_USER_GPIO_PostWakeup();//:Below recover VC08XX regs after wake up
				#if (defined(CMMB_ENABLE)| defined(RV_ENABLE)|defined(RV20_ENABLE))
					MMD_SYS_Sdram_Init();
				#endif			
							
				ret = MMD_SYS_TestRW();	
				if(ret)
				{
					FARF(MSG,("Wake up error, try hardware reset\n"));
					MMD_TRACE(0);
					// wake up error, do hardware reset
					MMD_SYS_Reset_8XX();
					#ifdef MMD_BIU_DATABUS16BIT
						MMD_BIU_Init(1);
					#else
						MMD_BIU_Init(0);  
					#endif
					ret = MMD_SYS_TestRW();	
					if(ret)
					FARF(MSG,("Hardware reset error, stop here\n"));
					MMD_ASSERT(ret==0);
					LCDIF_SetLCDIFParm(MMD_HAPI_LCDPANEL_MAIN);
					Lcd_MPanel_SetList(&(MLcdParm.poweron),0);	
				}
				
				LCDIF_SetLCDIFParm(MMD_HAPI_LCDPANEL_MAIN);
				VIM_HAPI_SetLCDColordep(VIM_HAPI_COLORDEP_16BIT);		//wendy guo 4/5/2007 add
				//VIM_HAPI_SetLCDColordep(VIM_HAPI_COLORDEP_A16BIT);	
				MMD_Media_Codec_Initialize();
				MMD_LCD_Init();   
			}
			break;
		case PMU_MODE_BYPASS:	
			MMD_SYS_Mode_Switch(SCENE_DEFAULT);
		 	MMD_SYS_Codec_Switch(CODEC_DEFAULT);
			g_sys_clk=CLK_NUM;
			MMD_LCD_BLY_Release();
			MMD_SYS_SnrPowerOff();
			MMD_Media_Codec_Stop_Play_Mode();
			MMD_Media_Codec_Stop_Record_Mode();
			MMD_USER_GPIO_PreSleep();	 /*  Do GPIO re-configuration */

			MMD_SYS_SetPSMode(MMD_PS_MODE_DEEPSLEEP);
			//MMD_USER_WaitMICRS(4000);  20090303
			g_biu_width=0;              
#if 0   //zhangxiang del 09/03/24
			#ifdef MMD_BIU_DATABUS16BIT
				MMD_BIU_Init(1);
			#else
				MMD_BIU_Init(0);  
			#endif
			MMD_SYS_Set_TriggerMode();
			g_SceneIdxInDevice=SCENE_NUM;
			g_CodecIdxInDevice=CODEC_NUM;
#endif
			while(Vm_GetReg(0x60011208)==0x3000)
			{
			 	FARF(MSG,("the deepsleep is wakeup by unknow wakeup source\n"));
				MMD_SYS_Mode_Switch(SCENE_DEFAULT);
				MMD_SYS_Codec_Switch(CODEC_DEFAULT);	
				MMD_SYS_SetPSMode(MMD_PS_MODE_DEEPSLEEP);
				MMD_USER_WaitMICRS(4000);
				g_biu_width=0;              
				#ifdef MMD_BIU_DATABUS16BIT
					MMD_BIU_Init(1);
				#else
					MMD_BIU_Init(0);  
				#endif
				MMD_SYS_Set_TriggerMode();
				g_SceneIdxInDevice=SCENE_NUM;
				g_CodecIdxInDevice=CODEC_NUM;
			}
			MMD_USER_SetChipMode(MMD_CHIP_MODE_BYPASS);	  
			clkrgm_gpio30_clk_control(FALSE);
			MMD_USER_WaitMICRS(50); //zhangxiang mod 09/01/14
			//MMD_USER_WaitMICRS(3500);
			VC_GPIO_CTRL(51, LO);
			VC_Sleep_Allow();
			break;
		default:
			break;
	}
	last_pmu_mode=pmu_mode;	   
}

void MMD_SYS_SnrPowerOff(void)
{
	MMD_U32 sensormode,pwdnmode;
	sensormode=Vm_GetReg(V5_REG_SIF_SNR_MODE);
	pwdnmode=(sensormode&(1<<2));
	if(pwdnmode==0)
	{
		sensormode|=(1<<2);
	}
	else
	{
		sensormode&=0xfffffffb;
	}
	Vm_SetReg(V5_REG_SIF_SNR_MODE,sensormode);	
}

MMD_U32 MMD_SYS_SnrPowerOn(void)
{
	MMD_U32 result;
	result=VIM_MAPI_AutoFindSensor();
#if 1//(VIM_HIGH_API_DEBUG)&&(VIM_5XRDK_DEBUG)
	if(result)
       MSG_FATAL("have not find sensor", 0, 0, 0);
		 //FARF(MSG,("have not find sensor\n"));
	else
	{
       MSG_FATAL("have find sensor,%s", gVc0578b_Info.pSensorInfo->desc, 0, 0);
        	    //FARF(MSG,("have find sensor\n"));
		    //FARF(MSG,("%s\n",gVc0578b_Info.pSensorInfo->desc));
	}
		
#endif
	return result;
	
}
void MMD_SYS_SetPlayVolume(MMD_U32 channel,MMD_U32 volume)
{   
       MMD_U32 val=0x00005454;
	 val|=volume<<16;
	 val|=volume<<24;
        HAL_WRITE_UINT32(V5_REG_AUD_CODEC_VOL_CTRL2, val); 	/* play  volume set ,master and headset */   
	{
		   int i = 0, j = 0;
   
  	          for(i=0;i<10000;i++) j++;
   	          HAL_WRITE_UINT32(V5_REG_AUD_CODEC_LATCH, 0x1); 
     
   	          for(i=0;i<10000;i++) j++;
   
   	          HAL_WRITE_UINT32(V5_REG_AUD_CODEC_LATCH, 0x0);
   	          for(i=0;i<10000;i++) j++;
	} 
}
MMD_U32 MMD_GetChipMode(void)
{
    if(GetVCBypassMode() == TRUE) // in vc0848 bypass mode
    {
        return MMD_CHIP_MODE_BYPASS;
    }
    else // in vc0848 work mode
    {
        return MMD_CHIP_MODE_NORMAL;
    }
}


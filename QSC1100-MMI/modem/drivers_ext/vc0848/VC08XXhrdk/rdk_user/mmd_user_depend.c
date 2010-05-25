/*=============================================================================
All rights reserved, Copyright (C) Vimicro LIMITED 2005
-------------------------------------------------------------------------------
[FILE NAME]
    MMD_USER_Depend.c 
[DESCRIPTION]
    User porting related spec.
[FUNCTION]
[EXTERNAL VARIABLE]
    None 
[DEPENDENCE ON HARDWARE]
    User depend.
=============================================================================*/

/******************************************************************************
  INCLUDE FILE 
******************************************************************************/
/* RDK header */
#include "mmp_rdk.h"
/* User header */
#if 0
#include "44b0.h"
#include "nucleus.h"
#endif
//#include "vMaclib.h"
#include "memheap.h"

#include "gpio_60x0.h"
#include "bio.h"
#include "clk.h"
#include "rex.h"
#include "../apps/media/media_utils.h"	

#include "aeestdlib.h"
#include "aee.h"
#include "aee_oem.h"

#include "aeefile.h"
#include "vc0848.h"

#define FS_BREW_DIR     "fs:/"

#define VC0848_MEM_BUFFER_SIZE 38720//176*220
static mem_heap_type VC0848_heap;
static char VC0848_mem_buffer[ VC0848_MEM_BUFFER_SIZE ];

#define MMD_USER_DEBUG_INFO
#define MMD_USER_DEBUG_ERR
#define MMD_USER_DEBUG_NOTE
#define MMD_USER_DEBUG_WARNING

extern TIMER_STATE	emMMDTimerState;

#ifdef MMD_USER_DEBUG_INFO
    #define PRINTF(WriteVal)        MMD_PRINTF(WriteVal) //general print out
#else
    #define PRINTF(WriteVal)
#endif

#ifdef MMD_USER_DEBUG_ERR
    #define PRINTFE(WriteVal)   MMD_PRINTFE(WriteVal) //error
#else
    #define PRINTFE(WriteVal)
#endif

#ifdef MMD_USER_DEBUG_NOTE
    #define PRINTFN(WriteVal)   MMD_PRINTFN(WriteVal) //note
#else
    #define PRINTFN(WriteVal)
#endif

#ifdef MMD_USER_DEBUG_WARNING
    #define PRINTFW(WriteVal)   MMD_PRINTFW(WriteVal) //warning
#else
    #define PRINTFW(WriteVal)
#endif

LOCAL int nEFSOpenMode = 0;
LOCAL int memory_get_fileread_mode(void);
extern boolean UnicodeToUtf8(const uint16 * pSrc,int nLen, byte * pDst, int nSize);
extern TimerISR g_VC0848TimerISR;

extern  rex_timer_type vc_user_timer;
dword   g_TimerInterval = 0;
//static int nRunFirstTime = 0;

extern SetVCBypassMode(boolean bMode);

REG_SAVE  REG_JTAG_EN={V5_REG_JTG_EN,0};
REG_SAVE  REG_GPIO_A_CFG={V5_REG_GPIO_A_CFG,0};
REG_SAVE  REG_GPIO_B_CFG={V5_REG_GPIO_B_CFG,0};
REG_SAVE  REG_GPIO_C_CFG={V5_REG_GPIO_C_CFG,0};
REG_SAVE  REG_GPIO_D_CFG={V5_REG_GPIO_D_CFG,0};
REG_SAVE  REG_GPIO_A_DIR={V5_REG_GPIO_A_DIR,0};
REG_SAVE  REG_GPIO_B_DIR={V5_REG_GPIO_B_DIR,0};
REG_SAVE  REG_GPIO_C_DIR={V5_REG_GPIO_C_DIR,0};
REG_SAVE  REG_GPIO_D_DIR={V5_REG_GPIO_D_DIR,0};
REG_SAVE  REG_GPIO_A_PE={V5_REG_GPIO_A_PE,0};
REG_SAVE  REG_GPIO_B_PE={V5_REG_GPIO_B_PE,0};
REG_SAVE  REG_GPIO_C_PE={V5_REG_GPIO_C_PE,0};
REG_SAVE  REG_GPIO_D_PE={V5_REG_GPIO_D_PE,0};
REG_SAVE  REG_GPIO_A_I={V5_REG_GPIO_A_I,0};
REG_SAVE  REG_GPIO_B_I={V5_REG_GPIO_B_I,0};
REG_SAVE  REG_GPIO_C_I={V5_REG_GPIO_C_I,0};
REG_SAVE  REG_GPIO_D_I={V5_REG_GPIO_D_I,0};
REG_SAVE  REG_UM_OUT_OEN={V5_REG_UM_OUT_OEN,0};
REG_SAVE  REG_UM_OUT_PE={V5_REG_UM_OUT_PE,0};
REG_SAVE  REG_MEM_SIG_OEN={V5_REG_MEM_SIG_OEN,0};
REG_SAVE  REG_MEM_DQ_OEN_DISABLE={V5_REG_MEM_DQ_OEN_DISABLE,0};
REG_SAVE  REG_MEM_PADS_PE={V5_REG_MEM_PADS_PE,0};
REG_SAVE  REG_NF_DAT_PE={V5_REG_NF_DAT_PE,0};

void MMD_USER_BiuTiming_Switch(MMD_U32 mode)
{

	switch(mode)
	{
		case 0:	/*normal speed access biu*/    /*Tacs           Tcos          Tacc          Toch           Tcah        Tpac*/
			//WriteMemory( REG_BANKCON4,((0<< 13) + (0<< 11) + (4<< 8) + (1 << 6) + (0 << 4) + (0<< 2)) );

         HWIO_OUT(LCD_CFG0,0x00220202);
			MMD_PRINTF(("Change to normal speed access biu timing\n"));
			break;
		case 1:	/*low speed access biu */    /*Tacs           Tcos          Tacc          Toch           Tcah        Tpac*/ 
			//WriteMemory( REG_BANKCON4,((3 << 13) + (3 << 11) + (7 << 8) + (3 << 6) + (3<< 4) + (3<< 2)) );
			HWIO_OUT(LCD_CFG0,0x00110505);
			MMD_PRINTF(("Change to low speed access biu timing\n"));
			break;
		default:
			break;
	}
}
void MMD_USER_WakeupInit(void)
{
#if 0        
    MMD_S32 x = 0;

    x = ReadMemory(REG_PCOND);
    x &= 0x3fff;
    x |= 0x4000;
    WriteMemory(REG_PCOND, x);      //GPD7 set to output

    x = ReadMemory(REG_PUPD); // disable GPD7 pullup
    x |= 0x80;
    WriteMemory(REG_PUPD, x);     

    //Set GPD7 as low
    SetMemory(REG_PDATD, 0x7f, AND_OPCODE);
#endif
}

// output a pulse to wakeup VC0848 through A9<VC_WAKEUPBB> port.
void MMD_USER_Wakeup(void)
{
    // GPIO_45 is VC_WAKEUPBB control PIN for VC0848: set a HIGH pluse for VC0848 wake up func
    VC_GPIO_CTRL(45, HI);
    MMD_USER_WaitMICRS(200); //zhangxiang mod 09/01/14
    //MMD_USER_WaitMICRS(3000);
    // restore GPIO_45 in LOW state.
    VC_GPIO_CTRL(45, LO);
    MMD_USER_WaitMICRS(3000);
}

#if 0
void MMD_USER_Clk_Control(MMD_U32 flag)
{
	if(flag)
		SetMemory(REG_PDATE, (0x1<<0), OR_OPCODE);    //Set PE0 as high
	else
		SetMemory(REG_PDATE, ~(0x1<<0), AND_OPCODE);    //Set PE0 as low
}
// flag: 0: off,   1: on


void MMD_USER_Power_Control(MODULE_POWER module_power,MMD_U08 status)
{
       MMD_S32 x = 0;
	switch(module_power)
	{
	       case POWER_CORE:
			if(status)
				SetMemory(REG_PDATB, (0x1<<5), OR_OPCODE);    //Set GPB5 as high
			else
				SetMemory(REG_PDATB, ~(0x1<<5), AND_OPCODE);    //Set GPB5 as low
			break;
		case POWER_AUDC_VDAC:
#if 0
			x = ReadMemory(REG_PUPD);       /* disable GPD4 pullup */
			x |= 0x01<<4;
			WriteMemory(REG_PUPD, x);     
#endif
			if(status)
			{
				SetMemory(REG_PDATD, 0x01<<4, OR_OPCODE); /* Set GPD4 as high to enable power */
			}
			else
			{

				SetMemory(REG_PDATD, ~(0x01<<4), AND_OPCODE); /* Set GPD4 as low to disable power */	
			}
			x = ReadMemory(REG_PCOND);  
			x &= ~(0x01<<(4<<1+1));
			x |= 0x01<<(4<<1);
			WriteMemory(REG_PCOND, x);      /* GPD4 set to output */		 
			break;
		
		case POWER_NF_CS: 
#if 0
			x = ReadMemory(REG_PUPD);       /* disable GPD5 pullup */
			x |= 0x01<<5;
			WriteMemory(REG_PUPD, x);     
#endif
			if(status)
			{
				SetMemory(REG_PDATD, 0x01<<5, OR_OPCODE); /* Set GPD5 as high to enable power */
			}
			else
			{

				SetMemory(REG_PDATD, ~(0x01<<5), AND_OPCODE); /* Set GPD5 as low to disable power */	
			}
			x = ReadMemory(REG_PCOND);  
			x &= ~(0x01<<(5<<1+1));
			x |= 0x01<<(5<<1);
			WriteMemory(REG_PCOND, x);      /* GPD5 set to output */		 
			break;
		case POWER_SDRAM:
#if 0
			x = ReadMemory(REG_PUPD);       /* disable GPD6 pullup */
			x |= 0x01<<6;
			WriteMemory(REG_PUPD, x);     
#endif
			if(status)
			{

				SetMemory(REG_PDATD, 0x01<<6, OR_OPCODE); /* Set GPD6 as high to enable power */			 
			}
			else
			{
				SetMemory(REG_PDATD, ~(0x01<<6), AND_OPCODE); /* Set GPD6 as low to disable power */
			}
			x = ReadMemory(REG_PCOND);  
			x &= ~(0x01<<(6<<1+1));
			x |= 0x01<<(6<<1);
			WriteMemory(REG_PCOND, x);      /* GPD6 set to output */		 
			break;
		case POWER_USB:
#if 0
			x = ReadMemory(REG_PUPE);       /* disable GPE3 pullup */
			x |= 0x01<<3;
			WriteMemory(REG_PUPE, x);     
#endif
			if(status)
			{
				SetMemory(REG_PDATE, 0x01<<3, OR_OPCODE); /* Set GPE3 as high to enable power */
			}
			else
			{

				SetMemory(REG_PDATE, ~(0x01<<3), AND_OPCODE); /* Set GPE3 as low to disable power */	
			}
			x = ReadMemory(REG_PCONE);  
			x &= ~(0x01<<(3<<1+1));
			x |= 0x01<<(3<<1);
			WriteMemory(REG_PCONE, x);      /* GPE3 set to output */		 
			break;
               
		default:
			break;
	       
	}   

}

#endif

/* Mode: 
 MMD_CHIP_MODE_NORMAL :  normal mode. Set BYPASS PIN to low
 MMD_CHIP_MODE_BYPASS   bypass mode. Set BYPASS PIN to high
*/
void MMD_USER_SetChipMode(MMD_U08 Mode)
{
   if(MMD_CHIP_MODE_BYPASS == Mode)
   {
      SetVCBypassMode(TRUE); // set BypassMode flag as true for locdal lcd update 
      
      MMD_LCD_BLY_Release();
      
      // GPIO_52 is VC_BYPASS control PIN for VC0848: LOW: normal; HIGH: bypass
      VC_GPIO_CTRL(52, HI);
      
      // GPIO_46 is AUDIO_SPK_SEL control PIN for VC0848: LOW: select QC audio; HIGH: VC0848 speaker output
      //VC_GPIO_CTRL(46, LO);   // becase that default status of SPK_SEL is in VC0848.  
      
      // GPIO_44 is SPK_PA control PIN: LOW: turn off PA; HIGH: turn on PA
      //VC_GPIO_CTRL(44, LO);

      // GPIO_43 is USB_SEL control PIN for VC0848: LOW: disable UDisk func; HIGH: enable UDisk Func
      VC_GPIO_CTRL(43, LO);

      // GPIO_42 is Camera Power control PIN for VC0848: LOW: turn off camera power; HIGH: turn on camera power 
      VC_GPIO_CTRL(42, LO);
      
      // GPIO_17 is VDD2V9_EN control PIN for VC0848: LOW: turn off VDD2V9_EN; HIGH: turn on VDD2V9_EN
      // if turn on VDD_2.9V_EN, it will supply power for VC0848 VDD_CS, VDD_VF, VDD_MEM, but it should be in NORMAL Mode
      // else if in BYPASS Mode, turn off VDD_2.9V please.
      // in bypass mode, turn off VDD2V9_EN
      VC_GPIO_CTRL(17, LO);

      // turn off BT_BCM LDO
      //VC_GPIO_CTRL(32, LO);
   }
   else
   {

      SetVCBypassMode(FALSE); // set BypassMode flag as false for vc0848 lcd update 
      
      // GPIO_52 is VC_BYPASS control PIN for VC0848: LOW: normal; HIGH: bypass
      VC_GPIO_CTRL(52, LO);

      // GPIO_17 is VDD2V9_EN control PIN for VC0848: LOW: turn off VDD2V9_EN; HIGH: turn on VDD2V9_EN
      // if turn on VDD_2.9V_EN, it will supply power for VC0848 VDD_CS, VDD_VF, VDD_MEM, but it should be in NORMAL Mode
      // else if in BYPASS Mode, turn off VDD_2.9V please.
      // in normal mode, turn on VDD2V9_EN
      VC_GPIO_CTRL(17, HI);
   }
   	Lcd_Panel_SetRSFunc(Mode);
}

/*************************************************************************************************
	GPIO Config before Bypass Sleep
	This Function will be called in MMD_SYS_PMU_SWITCH(), in case switch to BYPASS
*************************************************************************************************/
#if 0
void MMD_USER_GPIO_PreSleep(void)
{
/*	
	Test code for GPIO ProSleep operation. 
	MAKE SURE implement  MMD_USER_GPIO_PostWakeup() with relative code if do any settings here
*/ 

	HAL_WRITE_UINT32(0x60170820,0); // JTAG DISABLE
	
	HAL_WRITE_UINT32(0x6017000C,0xFFFFFFFF);  // GPIO_A_CFG // 1: GPIO	 0: FUNC
	#if 0	//	Set all GPIOB to GPIO. Cannot flush LCD in bypass and normal
		HAL_WRITE_UINT32(0x6017010C,0xFFFFFFFF);	// GPIO_B_CFG // 1: GPIO	 0: FUNC
	#else	//	Set GPIOB lower 24 bits to functional. Flush LCD OK
	HAL_WRITE_UINT32(0x6017010C,0xFF000700);    // GPIO_B_CFG // 1: GPIO	 0: FUNC
	#endif

    HAL_WRITE_UINT32(0x6017020C,0xFFFFFFFF);    // GPIO_C_CFG   // 1: GPIO   0: FUNC
    HAL_WRITE_UINT32(0x6017030C,0xFFA00000);    // GPIO_D_CFG   //1: GPIO   0: FUNC

    HAL_WRITE_UINT32(0x60170018,0xFCDF0EFF);   //GPIO_A_DIR 0:INPUT,1:OUTPUT
    HAL_WRITE_UINT32(0x60170118,0xFFEFFF1C);   //GPIO_B_DIR 0:INPUT,1:OUTPUT  //OLD 0xFFEFF81C
    HAL_WRITE_UINT32(0x60170218,0xFFFFFFFF);   //GPIO_C_DIR 0:INPUT,1:OUTPUT
    HAL_WRITE_UINT32(0x60170318,0xD9000000);   //GPIO_D_DIR 0:INPUT,1:OUTPUT

    HAL_WRITE_UINT32(0x60170014,0x0);  //GPIO_A_PE   //1: PULL enable   0: PULL disable
    HAL_WRITE_UINT32(0x60170114,0x200);  //GPIO_A_PE     //1: PULL enable   0: PULL disable   //OLD: 0
    HAL_WRITE_UINT32(0x60170214,0x0);   //GPIO_A_PE   //1: PULL enable   0: PULL disable
    HAL_WRITE_UINT32(0x60170314,0x0);    //GPIO_A_PE   //1: PULL enable   0: PULL disable

    HAL_WRITE_UINT32(0x60170010,0x0);      //GPIO_A_I     // 1: OUTPUT H    0: OUTPU L
    HAL_WRITE_UINT32(0x60170110,0x200);   //GPIO_B_I     // 1: OUTPUT H    0: OUTPU L
    HAL_WRITE_UINT32(0x60170210,0x0);    //GPIO_C_I // 1: OUTPUT H    0: OUTPU L
    HAL_WRITE_UINT32(0x60170310,0x0);    //GPIO_D_I // 1: OUTPUT H    0: OUTPU L    

    //disable UM_OUT 
    HAL_WRITE_UINT32(0x60170904,0x0);   
    HAL_WRITE_UINT32(0x60170908,0x0);
    //disable Memory interface output
    HAL_WRITE_UINT32(0x6017090C,0xFFFFFFFF);   
    HAL_WRITE_UINT32(0x60170910,0x3);
    HAL_WRITE_UINT32(0x60170914,0x0);
    HAL_WRITE_UINT32(0x60170918,0x0);
}

#endif
/*************************************************************************************************
	GPIO Config before Bypass Sleep
	This Function will be called in MMD_SYS_PMU_SWITCH(), in case switch to BYPASS
*************************************************************************************************/
void MMD_USER_GPIO_PreSleep(void)
{

       /* save these registers to  resume them after wakeup */
       HAL_READ_UINT32(REG_JTAG_EN.addr,REG_JTAG_EN.value);
       
       HAL_READ_UINT32(REG_GPIO_A_CFG.addr,REG_GPIO_A_CFG.value);
	HAL_READ_UINT32(REG_GPIO_B_CFG.addr,REG_GPIO_B_CFG.value);   
	HAL_READ_UINT32(REG_GPIO_C_CFG.addr,REG_GPIO_C_CFG.value);
	HAL_READ_UINT32(REG_GPIO_D_CFG.addr,REG_GPIO_D_CFG.value); 
	
       HAL_READ_UINT32(REG_GPIO_A_DIR.addr,REG_GPIO_A_DIR.value);
	HAL_READ_UINT32(REG_GPIO_B_DIR.addr,REG_GPIO_B_DIR.value);   
	HAL_READ_UINT32(REG_GPIO_C_DIR.addr,REG_GPIO_C_DIR.value);
	HAL_READ_UINT32(REG_GPIO_D_DIR.addr,REG_GPIO_D_DIR.value); 
	
	HAL_READ_UINT32(REG_GPIO_A_PE.addr,REG_GPIO_A_PE.value);
	HAL_READ_UINT32(REG_GPIO_B_PE.addr,REG_GPIO_B_PE.value);   
	HAL_READ_UINT32(REG_GPIO_C_PE.addr,REG_GPIO_C_PE.value);
	HAL_READ_UINT32(REG_GPIO_D_PE.addr,REG_GPIO_D_PE.value); 
	
	HAL_READ_UINT32(REG_GPIO_A_I.addr,REG_GPIO_A_I.value);
	HAL_READ_UINT32(REG_GPIO_B_I.addr,REG_GPIO_B_I.value);   
	HAL_READ_UINT32(REG_GPIO_C_I.addr,REG_GPIO_C_I.value);
	HAL_READ_UINT32(REG_GPIO_D_I.addr,REG_GPIO_D_I.value); 
    
	HAL_READ_UINT32(REG_UM_OUT_OEN.addr,REG_UM_OUT_OEN.value);
	HAL_READ_UINT32(REG_UM_OUT_PE.addr,REG_UM_OUT_PE.value);   
	
	HAL_READ_UINT32(REG_MEM_SIG_OEN.addr,REG_MEM_SIG_OEN.value);
	HAL_READ_UINT32(REG_MEM_DQ_OEN_DISABLE.addr,REG_MEM_DQ_OEN_DISABLE.value); 	
	HAL_READ_UINT32(REG_MEM_PADS_PE.addr,REG_MEM_PADS_PE.value);
	HAL_READ_UINT32(REG_NF_DAT_PE.addr,REG_NF_DAT_PE.value); 
	
       /* set these registers to save power */
	HAL_WRITE_UINT32(REG_JTAG_EN.addr,0); 
	   
	HAL_WRITE_UINT32(REG_GPIO_A_CFG.addr,0xFFFFFFFF&(~(0x1<<21)));  /* 1: GPIO, 0:FUNC*/
	HAL_WRITE_UINT32(REG_GPIO_B_CFG.addr,0xFF000000);
	HAL_WRITE_UINT32(REG_GPIO_C_CFG.addr,0xFFFFFFFF);	
	HAL_WRITE_UINT32(REG_GPIO_D_CFG.addr,0xFFA00000);		
	
	HAL_WRITE_UINT32(REG_GPIO_A_DIR.addr,0xFCDF0cFF);   /* 1: output ,0: input*/
	HAL_WRITE_UINT32(REG_GPIO_B_DIR.addr,0xFFEFF81C);
	HAL_WRITE_UINT32(REG_GPIO_C_DIR.addr,0xFFFFFFFF);
	HAL_WRITE_UINT32(REG_GPIO_D_DIR.addr,0xD9000000);
	
	HAL_WRITE_UINT32(REG_GPIO_A_PE.addr,0x0);  /* 1: PULL enable,0: PULL disable */
	HAL_WRITE_UINT32(REG_GPIO_B_PE.addr,0x1<<19);  
	HAL_WRITE_UINT32(REG_GPIO_C_PE.addr,0x0);	
	HAL_WRITE_UINT32(REG_GPIO_D_PE.addr,0x0);	 
	
	HAL_WRITE_UINT32(REG_GPIO_A_I.addr,0x0);	  /* 1: OUTPUT H,0: OUTPU L   */
	HAL_WRITE_UINT32(REG_GPIO_B_I.addr,0x0);	  
	HAL_WRITE_UINT32(REG_GPIO_C_I.addr,0x0);	 
	HAL_WRITE_UINT32(REG_GPIO_D_I.addr,0x0);	 	
	
	HAL_WRITE_UINT32(REG_UM_OUT_OEN.addr,0x0);	
	HAL_WRITE_UINT32(REG_UM_OUT_PE.addr,0x0);
	
	HAL_WRITE_UINT32(REG_MEM_SIG_OEN.addr,0xFFFFFFFF);   
	HAL_WRITE_UINT32(REG_MEM_DQ_OEN_DISABLE.addr,0x3);
	HAL_WRITE_UINT32(REG_MEM_PADS_PE.addr,0x0);
	HAL_WRITE_UINT32(REG_NF_DAT_PE.addr,0x0);

}

/*************************************************************************************************
	GPIO Config Init 
*************************************************************************************************/
void MMD_USER_GPIO_Init(void)
{
	// Set GPIO B as LCD function 
	HAL_WRITE_UINT32(REG_GPIO_B_CFG.addr,0x0);
	// Set LCD_RST PIN up-pull 
	HAL_READ_UINT32(REG_GPIO_B_PE.addr,REG_GPIO_B_PE.value);
	HAL_WRITE_UINT32(REG_GPIO_B_PE.addr,REG_GPIO_B_PE.value|(0x01<<19));
	
	// Added other config here if needed
}
#if 0
void MMD_USER_GPIO_PostWakeup(void)
{
#if 1
    //HAL_READ_UINT32( 0x60170820, v60170820 );
    HAL_WRITE_UINT32(0x60170820,1); // JTAG DISABLE
       
    //HAL_READ_UINT32( 0x6017000C, v6017000C );
    HAL_WRITE_UINT32(0x6017000C,0xFFFFFFFF);  // GPIO_A_CFG // 1: GPIO   0: FUNC
#if 0   //  Set all GPIOB to GPIO. Cannot flush LCD in bypass and normal
    HAL_WRITE_UINT32(0x6017010C,0xFFFFFFFF);    // GPIO_B_CFG // 1: GPIO     0: FUNC
#else   //  Set GPIOB lower 24 bits to functional. Flush LCD OK
    HAL_WRITE_UINT32(0x6017010C,0);    // GPIO_B_CFG // 1: GPIO     0: FUNC
#endif

    HAL_WRITE_UINT32(0x6017020C,0xefff00);    // GPIO_C_CFG   // 1: GPIO   0: FUNC
    HAL_WRITE_UINT32(0x6017030C,0x70000000);    // GPIO_D_CFG   //1: GPIO   0: FUNC

    HAL_WRITE_UINT32(0x60170018,0x400000);   //GPIO_A_DIR 0:INPUT,1:OUTPUT
    HAL_WRITE_UINT32(0x60170118,0x30000);   //GPIO_B_DIR 0:INPUT,1:OUTPUT  //OLD 0xFFEFF81C
    HAL_WRITE_UINT32(0x60170218,0x0);   //GPIO_C_DIR 0:INPUT,1:OUTPUT
    HAL_WRITE_UINT32(0x60170318,0x0);   //GPIO_D_DIR 0:INPUT,1:OUTPUT

    HAL_WRITE_UINT32(0x60170014,0x4e);  //GPIO_A_PE   //1: PULL enable   0: PULL disable
    HAL_WRITE_UINT32(0x60170114,0x10ffff);  //GPIO_A_PE     //1: PULL enable   0: PULL disable   //OLD: 0
    HAL_WRITE_UINT32(0x60170214,0x0);   //GPIO_A_PE   //1: PULL enable   0: PULL disable
    HAL_WRITE_UINT32(0x60170314,0xfd0000);    //GPIO_A_PE   //1: PULL enable   0: PULL disable

    HAL_WRITE_UINT32(0x60170010,0x400000);      //GPIO_A_I     // 1: OUTPUT H    0: OUTPU L
    HAL_WRITE_UINT32(0x60170110,0x30000);   //GPIO_B_I     // 1: OUTPUT H    0: OUTPU L
    HAL_WRITE_UINT32(0x60170210,0x0);    //GPIO_C_I // 1: OUTPUT H    0: OUTPU L
    HAL_WRITE_UINT32(0x60170310,0x0);    //GPIO_D_I // 1: OUTPUT H    0: OUTPU L    

    //disable UM_OUT 
    HAL_WRITE_UINT32(0x60170904,0x0);   
    HAL_WRITE_UINT32(0x60170908,0x0);
    //disable Memory interface output
    HAL_WRITE_UINT32(0x6017090C,0x0);   
    HAL_WRITE_UINT32(0x60170910,0x0);
    HAL_WRITE_UINT32(0x60170914,0x0);
    HAL_WRITE_UINT32(0x60170918,0x0);
#else
	HAL_WRITE_UINT32(0x60170820,v60170820); // JTAG DISABLE
	
	HAL_WRITE_UINT32(0x6017000C,v6017000C);  // GPIO_A_CFG // 1: GPIO	 0: FUNC
	#if 0	//	Set all GPIOB to GPIO. Cannot flush LCD in bypass and normal
		HAL_WRITE_UINT32(0x6017010C,0xFFFFFFFF);	// GPIO_B_CFG // 1: GPIO	 0: FUNC
	#else	//	Set GPIOB lower 24 bits to functional. Flush LCD OK
		HAL_WRITE_UINT32(0x6017010C,v6017010C);    // GPIO_B_CFG // 1: GPIO	 0: FUNC
	#endif
    HAL_WRITE_UINT32(0x6017020C,v6017020C);	// GPIO_C_CFG   // 1: GPIO	 0: FUNC
	HAL_WRITE_UINT32(0x6017030C,v6017030C);	// GPIO_D_CFG	//1: GPIO	0: FUNC
	
	HAL_WRITE_UINT32(0x60170018,v60170018);   //GPIO_A_DIR 0:INPUT,1:OUTPUT
	HAL_WRITE_UINT32(0x60170118,v60170118);   //GPIO_B_DIR 0:INPUT,1:OUTPUT  //OLD 0xFFEFF81C
	HAL_WRITE_UINT32(0x60170218,v60170218);   //GPIO_C_DIR 0:INPUT,1:OUTPUT
	HAL_WRITE_UINT32(0x60170318,v60170318);   //GPIO_D_DIR 0:INPUT,1:OUTPUT
	
	HAL_WRITE_UINT32(0x60170014,v60170014);  //GPIO_A_PE	 //1: PULL enable	0: PULL disable
	HAL_WRITE_UINT32(0x60170114,v60170114);  //GPIO_A_PE	 //1: PULL enable	0: PULL disable   //OLD: 0
	HAL_WRITE_UINT32(0x60170214,v60170214);	//GPIO_A_PE   //1: PULL enable	 0: PULL disable
	HAL_WRITE_UINT32(0x60170314,v60170314);	 //GPIO_A_PE   //1: PULL enable   0: PULL disable
	
	HAL_WRITE_UINT32(0x60170010,v60170010);	   //GPIO_A_I	  // 1: OUTPUT H	0: OUTPU L
	HAL_WRITE_UINT32(0x60170110,v60170110);	  //GPIO_B_I	 // 1: OUTPUT H    0: OUTPU L
	HAL_WRITE_UINT32(0x60170210,v60170210);	 //GPIO_C_I	// 1: OUTPUT H	  0: OUTPU L
	HAL_WRITE_UINT32(0x60170310,v60170310);	 //GPIO_D_I	// 1: OUTPUT H	  0: OUTPU L	

	//disable UM_OUT 
	HAL_WRITE_UINT32(0x60170904,v60170904);	
	HAL_WRITE_UINT32(0x60170908,v60170908);
	//disable Memory interface output
	HAL_WRITE_UINT32(0x6017090C,v6017090C);   
	HAL_WRITE_UINT32(0x60170910,v60170910);
	HAL_WRITE_UINT32(0x60170914,v60170914);
	HAL_WRITE_UINT32(0x60170918,v60170918);
#endif
}

#endif

/*************************************************************************************************
	GPIO Config after wake up
	This Function will be called in MMD_SYS_PMU_SWITCH(), while wake up and switch to Normal from Sleep
*************************************************************************************************/
void MMD_USER_GPIO_PostWakeup(void)
{
	
       /* Resume these registers after goto normal */
       HAL_WRITE_UINT32(REG_JTAG_EN.addr,REG_JTAG_EN.value);
       HAL_WRITE_UINT32(REG_GPIO_A_CFG.addr,REG_GPIO_A_CFG.value);
	HAL_WRITE_UINT32(REG_GPIO_B_CFG.addr,REG_GPIO_B_CFG.value);   
	HAL_WRITE_UINT32(REG_GPIO_C_CFG.addr,REG_GPIO_C_CFG.value);
	HAL_WRITE_UINT32(REG_GPIO_D_CFG.addr,REG_GPIO_D_CFG.value); 
	
       HAL_WRITE_UINT32(REG_GPIO_A_DIR.addr,REG_GPIO_A_DIR.value);
	HAL_WRITE_UINT32(REG_GPIO_B_DIR.addr,REG_GPIO_B_DIR.value);   
	HAL_WRITE_UINT32(REG_GPIO_C_DIR.addr,REG_GPIO_C_DIR.value);
	HAL_WRITE_UINT32(REG_GPIO_D_DIR.addr,REG_GPIO_D_DIR.value); 
	
	HAL_WRITE_UINT32(REG_GPIO_A_PE.addr,REG_GPIO_A_PE.value);
	HAL_WRITE_UINT32(REG_GPIO_B_PE.addr,REG_GPIO_B_PE.value);   
	HAL_WRITE_UINT32(REG_GPIO_C_PE.addr,REG_GPIO_C_PE.value);
	HAL_WRITE_UINT32(REG_GPIO_D_PE.addr,REG_GPIO_D_PE.value); 
	
	HAL_WRITE_UINT32(REG_GPIO_A_I.addr,REG_GPIO_A_I.value);
	HAL_WRITE_UINT32(REG_GPIO_B_I.addr,REG_GPIO_B_I.value);   
	HAL_WRITE_UINT32(REG_GPIO_C_I.addr,REG_GPIO_C_I.value);
	HAL_WRITE_UINT32(REG_GPIO_D_I.addr,REG_GPIO_D_I.value); 
       
	HAL_WRITE_UINT32(REG_UM_OUT_OEN.addr,REG_UM_OUT_OEN.value);
	HAL_WRITE_UINT32(REG_UM_OUT_PE.addr,REG_UM_OUT_PE.value);   
	
	HAL_WRITE_UINT32(REG_MEM_SIG_OEN.addr,REG_MEM_SIG_OEN.value);
	HAL_WRITE_UINT32(REG_MEM_DQ_OEN_DISABLE.addr,REG_MEM_DQ_OEN_DISABLE.value); 	
	HAL_WRITE_UINT32(REG_MEM_PADS_PE.addr,REG_MEM_PADS_PE.value);
	HAL_WRITE_UINT32(REG_NF_DAT_PE.addr,REG_NF_DAT_PE.value); 
}
/*
    Low reset.
    RSTn must be asserted low a minimum 30 SYSCLK cycles after stable power supplies.
*/
void MMD_USER_ResetChip(void)
{
   gpio_invert_pupd_state(1);
   MMD_USER_WaitMICRS(3000);
   gpio_recover_pupd_state(1);
   MMD_USER_WaitMICRS(8000);
}

// External SDRAM Init
void MMD_USER_SDRam_Init(void)
{
	MMD_U32 tmp,time_out=10000;
#ifdef SDRAM_8M
	Vm_SetReg(0x60030000,0x14320050);
	Vm_SetReg(0x60030008,0x15185454);
	Vm_SetReg(0x60030014,0x08);
#else
	Vm_SetReg(0x60030000,0x14210010);
	Vm_SetReg(0x60030008,0x15185454);
	//Vm_SetReg(0x60030008,0x15187454);
	Vm_SetReg(0x60030014,0x08);
#endif
	tmp = Vm_GetReg(0x60030010);
	while((tmp & 0xFF00) != 0x0)
	{
		tmp = Vm_GetReg(0x60030010);
		time_out--;
		if(time_out==0)
		{
			PRINTFE(("SDRAM Init timeout"));
			break;
		}	
	}	
}

void MMD_USER_WaitMICRS(MMD_U32 d1msec)
{
    if(d1msec == 0) return;
    
    else if(d1msec <= 10) // unit for d1msec is 0.1ms = 100us, if delay time > 1ms then call rex_sleep
    {
       clk_busy_wait(d1msec*100);
    }
    else
    {
       rex_sleep(d1msec/10);
    }
}
/*-----------------------------------------------------------------------------------------------------------
[FUNCTION NAME]
  MMD_USER_WaitMSEC
[DESCRIPTION]
  This API MUST be defined by user. 
  This API wait some mili seconds.
[ARGUMENT]
  msec     (IN) : Time (msec)
[RETURN VALUE]
  None
-----------------------------------------------------------------------------------------------------------*/
void MMD_USER_WaitMSEC(MMD_U32 msec)
{
       rex_sleep(msec);
}

void MMD_USER_ResetLCD_Bypass(void)
{
	// demo board no baseband GPIO can reset LCD panel
}
void MMD_USER_ResetLCD_Normal(void)
{
#if 1
	/* Config GPIO_B, set as 16-bit LCD */
	Vm_SetReg( 0x6017010c, (Vm_GetReg(0x6017010c)|0x00030000) );
	Vm_SetReg( 0x60170118,  (Vm_GetReg(0x60170118)|0x00030000) );
	Vm_SetReg( 0x60170110,  (Vm_GetReg(0x60170110)|0x00030000) );

	/* Reset LCD by LCDIF*/
	Vm_SetReg( 0x60090004, 0 );
	//NU_Sleep( 100 );
    MMD_USER_WaitMSEC(100);
	Vm_SetReg( 0x60090004, 1 );
    MMD_USER_WaitMSEC(20);
	//NU_Sleep( 20 );
#endif
}

void MMD_USER_PowerOnLogo_Bypass(void)
{
	// power on logo, here just use blue screen instead
	pnl_draw_block(0,0,240,320,0x001f);
}

void MMD_USER_PowerOnLogo_Normal(void)
{
	// power on logo, here just use green screen instead
	pnl_draw_block(0,0,240,320,0x07E0);
}

#if 0
NU_TIMER g_OS_Depend_Timer;
#endif	

#if 0
void MMD_USER_Timer_ISR(UNSIGNED id)
{

	g_TimerISR();

}
#endif

void MMD_USER_Timer_Create(TimerISR pTimerISR, MMD_U32 interval)
{
}

void MMD_USER_Timer_Release(void)
{
	//STATUS	status;
    (void)rex_clr_timer(&vc_user_timer);
    VC_SetTimerState(VC_TIMER_RELEASE);

	g_VC0848TimerISR = NULL;
    g_TimerInterval = 0;
}

// This function call user OS dependent timer function, rdk will call this function to create timer
// so user can not modified this function
void MMD_USER_Timer_Set(TimerISR pTimerISR, MMD_U32 interval)
{
	// check timer ISR and inteval
	if(pTimerISR == g_VC0848TimerISR || interval == 0 || pTimerISR == NULL)
	{
	   return;
	}
	
	g_TimerInterval = interval;
	if(g_VC0848TimerISR == NULL)
	{
       g_VC0848TimerISR = pTimerISR;
	}
	else
	{
       rex_clr_timer(&vc_user_timer);
       VC_SetTimerState(VC_TIMER_STOP);
       g_VC0848TimerISR = pTimerISR;
	}
}

void MMD_USER_Timer_Start(void)
{
	//STATUS	status;
	
	if(g_VC0848TimerISR == NULL || g_TimerInterval == 0)
	{
	   return;
	}
    
    (void)rex_set_timer(&vc_user_timer, g_TimerInterval);
    VC_SetTimerState(VC_TIMER_RUN);
}

void MMD_USER_Timer_Stop(void)
{
	//STATUS	status;

    MSG_ERROR("Timer_Stop before rex_clr_timer for VC0848", 0, 0, 0);
    (void)rex_clr_timer(&vc_user_timer);
    VC_SetTimerState(VC_TIMER_STOP);
}

/*-----------------------------------------------------------------------------
[FUNCTION NAME]
	MMD_USER_GetDate
[DESCRIPTION]
	Retrieves the current date information. 
[PARAMETERS]
	psDate(OUT) 	: 	where to store current date information
[RETURN VALUE]
	None
[REMARKS]
	Need to be ported depending on user's system.
-----------------------------------------------------------------------------*/
void MMD_USER_GetDate(MMD_DATE *psDate)
{
    static JulianType julian;
    GETJULIANDATE(GETTIMESECONDS(), &julian);
	psDate->day = julian.wDay;
	psDate->month = julian.wMonth ;
	psDate->year = julian.wYear;
}

/*-----------------------------------------------------------------------------
[FUNCTION NAME]
	MMD_USER_GetTime
[DESCRIPTION]
	Retrieves the current time information. 
[PARAMETERS]
	psTime(OUT) 	: 	where to store current time information
[RETURN VALUE]
	None
[REMARKS]
	Need to be ported depending on user's system.
-----------------------------------------------------------------------------*/
void MMD_USER_GetTime(MMD_TIME *psTime)
{
    static JulianType julian;
    GETJULIANDATE(GETTIMESECONDS(), &julian);
	psTime->second = julian.wSecond;
	psTime->minute = julian.wMinute;
	psTime->hour = julian.wHour;
}

void EnableUDisk(void)
{
    USB_MASS_INFO usbInfo;
    if(MMD_GetChipMode() == PMU_MODE_BYPASS)
    {
       MMD_SYS_PMU_SWITCH(PMU_MODE_NORMAL);
       MMD_USER_WaitMICRS(4000);
    }
    // set usb_sel high to enable UDisk func for VC0848
    VC_GPIO_CTRL(43, HI);
    
    MMD_SYS_Mode_Switch(SCENE_USB);
    
    usbInfo.device =0;
    usbInfo.speed =0;
    MMD_USB_Enable(USB_MASS_STORAGE, NULL);
    MMD_USER_WaitMICRS(1000);
    
    MMD_USB_SetProperty(USB_MASS_STORAGE, &usbInfo);
    MMD_USER_WaitMICRS(1000);
    
}

void DisableUDisk(void)
{
    MMD_USB_Disable(USB_MASS_STORAGE);

    // disable the usb_sel and switch it to QC usb
    VC_GPIO_CTRL(43, LO);
    MMD_SYS_PMU_SWITCH(PMU_MODE_BYPASS);
    MMD_USER_WaitMICRS(2000);
}

#ifdef RV_ENABLE
//: Convert String
extern const unsigned short gbk2UnTab[21919][2];

static MMD_U16 MMD_Unicode2Gbk(MMD_U16 wUnicode)
{
	MMD_U32  i;
 
	for(i = 0; i < ((sizeof(gbk2UnTab)) >> 2); i++)
	{
		if(wUnicode == gbk2UnTab[i][1])
			return gbk2UnTab[i][0];
	}
 	return 0;
}


static void  MMD_UnicodeToGB(MMD_U16* srcbuf, MMD_U08* dstbuf, MMD_U32 datalen)
{
	MMD_U32 i;
	MMD_U16 ret_gb;
	for(i=0;i<datalen;i++)
	{
		ret_gb = MMD_Unicode2Gbk(srcbuf[i]);
		if(ret_gb != 0)
		{
			if(ret_gb > 0xff)
			{
				*dstbuf = ret_gb>>8;
				dstbuf ++;
				*dstbuf = ret_gb;
				dstbuf ++;
			}
			else
			{
				*dstbuf = ret_gb;
				dstbuf++;
			}
		}
	}
	*dstbuf = 0;
}

MMD_U32 GetUtf8ByteNumForWord(MMD_U08 firstCh)
{
	MMD_U08 temp = 0x80;
	MMD_S32 num = 0;
	
	while (temp & firstCh)
	{
		num++;
		temp = (temp >> 1);
	}
	
	return num;
}

//http://www.cnitblog.com/wujian-IT/archive/2008/03/04/37671.html
MMD_U32 MMD_UtfToUnicode(MMD_U08 *pUtfsrc, MMD_U16 *pUnidst)
{
	MMD_U32 unicode_len,byteCount;
	unicode_len = 0;
	//循环解析
	while (pUtfsrc[0])
	{   
        switch(GetUtf8ByteNumForWord(pUtfsrc[0]))
        {
		case 0:
			pUnidst[0] = pUtfsrc[0];
			byteCount = 1;
			break;
			
		case 2:
#if 0
			temp[j] = utf8[i];
			temp[j + 1] = utf8[i + 1];
#else
			//printf("byteCount = 2\n");
			//printf("%s\n", pUtfsrc);
#endif
			byteCount = 2;
			//while(1);
			break;
			
		case 3:
			//这里就开始进行UTF8->Unicode
			pUnidst[0] = (unsigned short)(((pUtfsrc[0] & 0x0F) << 12) | ((pUtfsrc[1] & 0x3F) << 6)|(pUtfsrc[2] & 0x3F));
			byteCount = 3;
			break;
			
		case 4:
			byteCount = 4;
			printf("byteCount = %d\n",byteCount);
			//while(1);
			break;
		case 5:
			byteCount = 5;
			printf("byteCount = %d\n",byteCount);
			//while(1);
			break;
		case 6:
			byteCount = 6;
			printf("byteCount = %d\n",byteCount);
			//while(1);
			break;
			
		default:
			printf("the len is more than 6\n");
			//while(1);
			break;    
        }
		pUnidst++;
		pUtfsrc +=byteCount;
		unicode_len ++;
	}
	pUnidst[0]=0;
	unicode_len++;
	return unicode_len;

}


void  MMD_ConvertString(MMD_S08* str, MMD_S08* strDesti)  
{   
	MMD_U16 tmp_unicode_buff[256];

#if 0
    int len=(int)strlen(str);   
    int unicodeLen=MultiByteToWideChar(sourceCodepage,0,str,len,NULL,0);   
    wchar_t *   pUnicode;   
    pUnicode=new wchar_t[unicodeLen+1];

    memset(pUnicode,0,(unicodeLen+1)*sizeof(wchar_t));
    MultiByteToWideChar(sourceCodepage,0,str,len,(LPWSTR)pUnicode,unicodeLen);
#else
	MMD_U32 unicodeLen = MMD_UtfToUnicode((MMD_U08*)str,tmp_unicode_buff);
#endif
#if 1
	MMD_UnicodeToGB(tmp_unicode_buff,(MMD_U08*)strDesti, unicodeLen);
#else
    BYTE *pTargetData;   
    int targetLen=WideCharToMultiByte(targetCodepage,0,(LPWSTR)pUnicode,unicodeLen,NULL,0,NULL,NULL);  

    pTargetData=new BYTE[targetLen+1];   
    memset(pTargetData,0,targetLen+1);
    WideCharToMultiByte(targetCodepage,0,(LPWSTR)pUnicode,unicodeLen,(char*)pTargetData,targetLen,NULL,NULL);
    sprintf(strDesti,"%s",pTargetData);

    delete pTargetData;   
    delete pUnicode;   
#endif
} 

///:~




/*-----------------------------------------------------------------------------
[FUNCTION NAME]
	MMD_USER_LCD_Rotation
[DESCRIPTION]
	LCD rotation function 
[PARAMETERS]
	rotation mode
[RETURN VALUE]
	None
[REMARKS]
	Need to be ported depending on user's system.
-----------------------------------------------------------------------------*/
void MMD_USER_LCD_Rotation(MMD_U32 degree)
{
	Lcd_Rotate(degree);
}


#define 		THREAD_PRIORITY_RV		6
#define 		THREAD_STACKSIZE_RV 		0x2000
#define 		QUEUE_SIZE_RV 				(MSG_SIZE*10)	// max 10 message

NU_QUEUE  	gmbox_rv;
NU_QUEUE  	gmbox_rv;
NU_TASK 	thread_rv;
TaskFunc 	g_TaskFunc=NULL;
MMD_U32 rv_thread_stack[THREAD_STACKSIZE_RV/4];
MMD_U32 rv_thread_queue[QUEUE_SIZE_RV];  // DWORD unit

void MMD_USER_Msg_Create(void)
{
	STATUS status;

	/* Create a queue to send msg*/
	MMD_MemSet(&gmbox_rv, 0, sizeof(gmbox_rv));
	status = NU_Create_Queue(&gmbox_rv, 		//NU_QUEUE *queue
							"rv file", 		//CHAR *name
							rv_thread_queue,				//VOID *start_address 
							QUEUE_SIZE_RV,	//UNSIGNED queue_size,
						       NU_FIXED_SIZE, 		//OPTION message_type
						       MSG_SIZE, 			//UNSIGNED message_size
						       NU_FIFO);			//OPTION suspend_type
	MMD_ASSERT(status==NU_SUCCESS);
}

/*******************************************************************************************
Description:
	rv decode thread message send function
Parameters:
	VIM_MSG* pMsg: message handle
Return:	
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.04.02		create first version
********************************************************************************************/
void MMD_USER_Msg_Send(void* para)
{
	STATUS status;
	MMD_MSG* pMsg=(MMD_MSG*)para;
	NU_TASK * task_ptr = NULL;
	NU_HISR * hisr_ptr = NULL;

	// This function is call form interrupt 
	hisr_ptr = NU_Current_HISR_Pointer();
	if(hisr_ptr != NU_NULL)
	{
		status = NU_Send_To_Queue(&gmbox_rv, 			//NU_QUEUE *queue
									 pMsg, 					//VOID *message
									 MSG_SIZE,				//UNSIGNED size 
									 NU_NO_SUSPEND);		//UNSIGNED suspend 
		MMD_ASSERT(status==NU_SUCCESS);
		return;
	}

	// This function is call from task
	task_ptr = NU_Current_Task_Pointer();
	
	if(task_ptr != NU_NULL)
	{
		status = NU_Send_To_Queue(&gmbox_rv, 			//NU_QUEUE *queue
									 pMsg, 					//VOID *message
									 MSG_SIZE, 				//UNSIGNED size
									 25); // 250ms timeout		//UNSIGNED suspend
		MMD_ASSERT(status==NU_SUCCESS);
		return;
	}
	return;
}

/*******************************************************************************************
Description:
	rv decode thread message send function
Parameters:
	VIM_MSG* pMsg: message handle
Return:	
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.04.02		create first version
********************************************************************************************/
MMD_U32 MMD_USER_Msg_Receive(MMD_MSG* pMsg)
{
	STATUS status;
	UNSIGNED actual_size;

	status = NU_Receive_From_Queue(&gmbox_rv, 	//NU_QUEUE *queue
								  pMsg, 				//VOID *message
								  MSG_SIZE,			//UNSIGNED size
								  &actual_size, 		//UNSIGNED *actual_size
								  NU_SUSPEND);		//UNSIGNED suspend
	MMD_ASSERT(status==NU_SUCCESS);

	return status;
}

void MMD_USER_Msg_Release(void)
{
	STATUS status;

	status = NU_Delete_Queue(&gmbox_rv);
	MMD_ASSERT(status==NU_SUCCESS);
}
void MMD_USER_Task_Func(UNSIGNED argc, VOID *argv)
{
	MMD_MSG msg;
	while(1)
	{
		MMD_USER_Msg_Receive(&msg);
		g_TaskFunc(&msg);	
	}	
}
void MMD_USER_Task_Create(TaskFunc pTaskFunc)
{
	STATUS status;
	
	if(g_TaskFunc!=NULL)
		return;
	
	// Create msg quene
	MMD_USER_Msg_Create();
	
	// Create task 
	MMD_MemSet(&thread_rv, 0, sizeof(thread_rv));
	g_TaskFunc = pTaskFunc;
	status = NU_Create_Task(&thread_rv,				//NU_TASK *task
						  "rv thread", 				//CHAR *name
						  MMD_USER_Task_Func, 			//VOID (*task_entry)(UNSIGNED, VOID *)
						  0, 							//UNSIGNED argc
						  NU_NULL,					//VOID *argv 
						  rv_thread_stack, 					//VOID *stack_address
						  THREAD_STACKSIZE_RV, 	//UNSIGNED stack_size
						  THREAD_PRIORITY_RV,	//OPTION priority 
						  0, 							//UNSIGNED time_slice
						  NU_PREEMPT, 				//OPTION preempt
						  NU_START);					//OPTION auto_start
						  
	MMD_ASSERT(status==NU_SUCCESS);

}
void MMD_USER_Task_Release(void)
{
	STATUS status;
	
	if(g_TaskFunc==NULL)
		return;
	// Stop and release task
	status = NU_Terminate_Task(&thread_rv);
	MMD_ASSERT(status==NU_SUCCESS);
	status = NU_Delete_Task(&thread_rv);
	MMD_ASSERT(status==NU_SUCCESS);
	g_TaskFunc = NULL;
	// Delete Msg quene
	MMD_USER_Msg_Release();
}

void MMD_USER_FS_Init(void)
{
	MMD_U32 ret, cap;
	
	V988host_ResetChip();//988 ic reset --modify by houyin
       PRINTF(("reset VC988 !\r\n"));

	ret = (MMD_U32)Vmac_GPIOConfig(1,0,0);
	PRINTF(("GPIOConfig return is 0x%x\r\n",ret));
	
	ret = (MMD_U32)Vmac_Standby2Normal();
	PRINTF(("Standby2Normal return is 0x%x\r\n",ret));
	
	ret = Vmac_ResetSD(&cap);
	PRINTF(("ResetSD return is 0x%x\r\n",ret));

	ret = (MMD_U32)Vmac_FMount();
	PRINTF(("mount return is 0x%x\r\n",ret));

	MMD_USER_WaitMICRS(1000*10);
}
void MMD_USER_FS_Release(void)
{
	// Added here if needed
}
volatile MMD_U32 g_pFileOffset;
void* MMD_USER_FS_File_Open(void* pFileName, void* mode)
{
	VFAT_FILE* pHandle;
	// only read mode is used 
	pHandle = Vmac_FOpen((MMD_U08*)pFileName, VFAT_FILE_RD);
	g_pFileOffset = 0;
	return pHandle;
}

MMD_U32 MMD_USER_FS_File_Get_Length(void* handle)
{
	VFAT_FILE* pHandle=(VFAT_FILE* )handle;

	return  pHandle->length;
}

MMD_S32 MMD_USER_FS_File_Read(void* handle, MMD_S08* buf, MMD_S32 count)
{
	VFAT_FILE* pHandle=(VFAT_FILE* )handle;
	MMD_S32 readsize;

	readsize = Vmac_FRead(pHandle, (MMD_U08*)buf,g_pFileOffset,count);
	g_pFileOffset+=readsize;
	return  readsize;
}

MMD_S32 MMD_USER_FS_File_Close(void* handle)
{
	VFAT_FILE* pHandle=(VFAT_FILE* )handle;
	g_pFileOffset = 0;
	return  Vmac_FClose(pHandle);
}

MMD_U32 MMD_USER_CMMB_GPIO_SELECT(MMD_U32 reset_pin, MMD_U32 intr_pin)
{
	//: GPIO A PORT only, config  Macro USER_CMMB_RESET_PIN and USER_CMMB_INTR_PIN in mmd_user_depend.h
	
	/* =======================
		Reset - GPIO A3
		interrupt GPIO A23
	 =======================*/
	 //: Do close JTAG(A3)
	 if(reset_pin == 3)
		Vm_SetReg(0x60170820 , 0x0);

	
	/* =======================
		Reset - GPIO A10
		interrupt GPIO A26
	 =======================*/	
	 //: Do nothing
	return MMD_CMMB_GPIO_SELECT(reset_pin, intr_pin);
}


#endif

void VC0848_mem_init( void )
{
   mem_init_heap( &VC0848_heap, VC0848_mem_buffer, sizeof( VC0848_mem_buffer ), NULL);
   return;
}

#ifdef SD_MEM_DBG
void *SD_Malloc( unsigned int num_bytes)
{
   return mmd_mem_malloc( &VC0848_heap, num_bytes );
}

void SD_Free( void *mem_ptr )
{
   mmd_mem_free( &VC0848_heap, mem_ptr );
   return;
} 
#endif  /* SD_MEM_DBG */

#define	MEM_FILE
#define VC0848_EFS2_SUPPORT

#ifdef	MEM_FILE
IFileMgr * pFileMgr = NULL;
IFile * pFileEFS = NULL; 
uint8 * pMemBuf = NULL;


LOCAL int memory_get_fileread_mode(void)
{
    return nEFSOpenMode;
}

void memory_set_fileread_mode(int nMode)
{
    nEFSOpenMode = nMode;
}

/* ---------------------------------------------------------
 file[]: file handle
 buffer[dst]: cope file content to this buffer
 count: number of content needed copied to buffer
----------------------------------------------------------*/
int memory_fileread(void* file, char* buffer, int count)
{
#ifdef VC0848_EFS2_SUPPORT   
	MEMORY_FILE* 	pFile =(MEMORY_FILE* )file;
	unsigned int 		cnt = 0;
	
	if(pFile->FileReadCur>=pFile->FileTotalLength)
		return 0;

	cnt = count>(pFile->FileTotalLength-pFile->FileReadCur)?(pFile->FileTotalLength-pFile->FileReadCur):count;
	mp_memcpy((char*)buffer, (const char*)pFile->FileAddress+pFile->FileReadCur,cnt);
	pFile->FileReadCur+=cnt;
	return cnt;
#endif  // VC0848_EFS2_SUPPORT
}


int memory_filewrite(void* file, char* buffer, int count)
{
#ifdef VC0848_EFS2_SUPPORT
    int nMode;
	MEMORY_FILE* pFile =(MEMORY_FILE*)file;
    MSG_ERROR("in memory_filewrite", 0, 0, 0);
    nMode = memory_get_fileread_mode();
    
	mp_memcpy((char*)pFile->FileAddress+pFile->FileWriteCur,(const char*)buffer, count);
	pFile->FileWriteCur+=count;
	pFile->FileTotalLength+=count;
    if(pFileEFS && nMode == VC_OFM_CREATE)
    {
        IFILE_Write(pFileEFS, (const void*)buffer, count);
    }
	return count;
#endif    
}

int memory_fileseek(void* file, int offset, int mode)
{
#ifdef VC0848_EFS2_SUPPORT
	MEMORY_FILE* pFile =(MEMORY_FILE*)file;
    int nSeekMode;

	switch(mode)
	{
	case SEEK_SET:
		pFile->FileReadCur = offset;
        nSeekMode = _SEEK_START;
		break;
	case SEEK_CUR:
		pFile->FileReadCur += offset;
        nSeekMode = _SEEK_CURRENT;
		break;
	case SEEK_END:
		pFile->FileReadCur = pFile->FileTotalLength+offset;
        nSeekMode = _SEEK_END;
		break;
	default:
		break;
	}
    
    if(pFileEFS && memory_get_fileread_mode() == VC_OFM_CREATE)
    {
        (void)IFILE_Seek(pFileEFS, nSeekMode, 0);
    }
    
	if(pFile->FileReadCur > pFile->FileTotalLength)
		return pFile->FileReadCur;
	else
		return 0;
#endif    
}

int memory_filetell(void* file)
{
#ifdef VC0848_EFS2_SUPPORT
    int nMode;
	MEMORY_FILE* pFile =(MEMORY_FILE*)file;
    MSG_ERROR("in memory_filetell", 0, 0, 0);
    nMode = memory_get_fileread_mode();
    
    if(pFileEFS && memory_get_fileread_mode() == VC_OFM_CREATE)
    {
        (void)IFILE_Seek((IFile *)file, _SEEK_CURRENT, 0);
    }
    
	return pFile->FileReadCur;
#endif    
}

int memory_fileeof(void* file)
{
#ifdef VC0848_EFS2_SUPPORT
	MEMORY_FILE* pFile =(MEMORY_FILE*)file;
    MSG_ERROR("in memory_fileeof", 0, 0, 0);
    
    if(pFileEFS && memory_get_fileread_mode() == VC_OFM_CREATE)
    {
        (void)IFILE_Seek((IFile *)file, _SEEK_END, 0);
    }
    
	if(pFile->FileReadCur >= pFile->FileTotalLength)
		return pFile->FileReadCur;
	else
		return 0;
#endif    
}


void * memory_fileopen(void *address, void *size)
{
#ifdef VC0848_EFS2_SUPPORT
	MEMORY_FILE* file=NULL;
    FileInfo info;
    char fileName[FILE_LEN_UTF8];
    int nMode;
	int *filelength = (int *)size;
    MSG_ERROR("in memory_fileopen", 0, 0, 0);
    memory_fileclose(NULL);
    memset(fileName, 0, FILE_LEN_UTF8);
    memcpy(fileName, FS_BREW_DIR, STRLEN(FS_BREW_DIR));
    UnicodeToUtf8((const uint16 *)address, WSTRLEN(address)*2, (byte *)(&fileName[STRLEN(FS_BREW_DIR)]), FILE_LEN_UTF8);
    
    if(pFileMgr  != NULL)
    {
        IFILEMGR_Release(pFileMgr);
        pFileMgr = NULL;
    }
    if (ISHELL_CreateInstance(AEE_GetShell(), AEECLSID_FILEMGR, (void**)&pFileMgr) != AEE_SUCCESS)
       return NULL;

    nMode = memory_get_fileread_mode();
    pFileEFS = IFILEMGR_OpenFile(pFileMgr, fileName, nMode);
        
    MSG_ERROR("pFile:0x%08x, &pFile:0x%08x", pFileEFS, &pFileEFS, 0);
    if(!pFileEFS)
    {
        MSG_ERROR("IFILEMGR_OpenFile failed in VC0848", 0, 0, 0);   
        memory_fileclose(NULL);
        return NULL;
    }
	file=(MEMORY_FILE*)mp_malloc(sizeof(MEMORY_FILE));
	if(!file)
	{
		MSG_ERROR("MP_ERR_MALLOC_MEMORY_ERROR", 0, 0, 0);
        memory_fileclose(NULL);
		return NULL;
	}

    if(nMode == VC_OFM_CREATE)
    {
        pMemBuf = (uint8 *)mp_malloc((unsigned int)(*(int *)size));
    	if(!pMemBuf)
    	{
    		MSG_ERROR("MP_ERR_MALLOC_MEMORY_ERROR", 0, 0, 0);
            memory_fileclose(file);
    		return NULL;
    	}
    }
    else if(nMode == VC_OFM_READ)
    {
        IFILE_GetInfo(pFileEFS, &info);
        pMemBuf = (uint8 *)mp_malloc(info.dwSize);
    	if(!pMemBuf || info.dwSize > 0x7D000) // 0x7D000 means 500KB 
    	{
    		MSG_ERROR("MP_ERR_MALLOC_MEMORY_ERROR", 0, 0, 0);
            memory_fileclose(file);
    		return NULL;
    	}
        IFILE_Read(pFileEFS, pMemBuf, info.dwSize);
        *filelength = info.dwSize;
    }
	file->FileAddress 		= (char *)pMemBuf; //(char*)address;
	file->FileTotalLength 	= (int)*filelength;
	file->FileReadCur 		= file->FileWriteCur = 0;
	return (void*)file;
#endif // VC0848_EFS2_SUPPORT    
}

int memory_fileclose(void* file)
{
#ifdef VC0848_EFS2_SUPPORT
    MSG_ERROR("in memory_fileclose", 0, 0, 0);
    if(!file) mp_free((void*)file);
    if(pFileEFS) (void)IFILE_Release((IFile *)pFileEFS);
    if(pFileMgr) (void)IFILEMGR_Release(pFileMgr);
    if(pMemBuf) mp_free((void *)pMemBuf);
    pFileMgr = NULL;
    pFileEFS = NULL;
    pMemBuf = NULL;
	return 0;
#endif    
}

int memory_filedelete(void *address)
{
    char fileName[FILE_LEN_UTF8];
    memset(fileName, 0, FILE_LEN_UTF8);
    memcpy(fileName, FS_BREW_DIR, STRLEN(FS_BREW_DIR));
    UnicodeToUtf8((const uint16 *)address, WSTRLEN(address)*2, (byte *)(&fileName[STRLEN(FS_BREW_DIR)]), FILE_LEN_UTF8);
    MSG_ERROR("in memory_filedelete", 0, 0, 0);

    if(pFileMgr  == NULL)
    {
        (void)ISHELL_CreateInstance(AEE_GetShell(), AEECLSID_FILEMGR, (void**)&pFileMgr);
    }
    if ( pFileMgr != NULL)
    {
        IFILEMGR_Remove(pFileMgr, fileName);
    }
	return 0;
}

//by dnn 2008/4/11
#if ME_VERSION2_FOURHANDLE
int memory_block_fileread(void* file, char* buffer, int count, unsigned int reg, unsigned int bit)
{
	MEMORY_FILE* 	pFile =(MEMORY_FILE* )file;
	unsigned int 		cnt = 0;
	unsigned int  realstart=0;
	unsigned int  offset=0;
	unsigned int  reallen=0;
	unsigned int  result=0;
	
	if(pFile->FileReadCur>=pFile->FileTotalLength)
		return 0;
	realstart = (unsigned int)(pFile->FileAddress+pFile->FileReadCur);
	offset = realstart%4;
	if(offset!=0) {
		if(offset>pFile->FileReadCur) {
			return 0;
		} else {
			realstart -= offset; // 4x
		}
	}
	reallen = count+offset;
	if(reallen%4!=0) {
		cnt = 4-(reallen%4);
	}else{
		cnt = 0;
	}
	if(count+cnt<=(pFile->FileTotalLength-pFile->FileReadCur)) {
		reallen += cnt;
		result = ME_MEMCPY_H2D((unsigned int)buffer, (unsigned char*)(realstart),reallen);
		if(result<=offset){
			return 0;
		} else if(result>offset && result<offset+count) {
			pFile->FileReadCur += result-offset;
			return result-offset;
		} else if(result>=offset+count) {
			pFile->FileReadCur +=count;
			return count;
		}
	} else {
		reallen = offset+pFile->FileTotalLength-pFile->FileReadCur;
		result = ME_MEMCPY_H2D((unsigned int)buffer, (unsigned char*)(realstart),reallen);
		if(result<=offset){
			return 0;
		} else if(result>offset && result<offset+count) {
			pFile->FileReadCur += result-offset;
			return result-offset;
		} else if(result>=offset+count) {
			pFile->FileReadCur +=count;
			return count;
		}
	}	
}

int memory_block_filewrite(void* file, char* buffer, int count, unsigned int reg, unsigned int bit)
{
	MEMORY_FILE* pFile =(MEMORY_FILE*)file;

//	mp_memcpy_d2h((char*)pFile->FileAddress+pFile->FileWriteCur,(const char*)buffer, count);
//	Vm_RdSram((unsigned int)buffer, (char*)(pFile->FileAddress+pFile->FileWriteCur), count);
	mp_memcpy_d2h(buffer, (char*)(pFile->FileAddress+pFile->FileWriteCur), count);
	pFile->FileWriteCur+=count;
	pFile->FileTotalLength+=count;
	return count;
}

int memory_get_sector_offset(void* file, unsigned int * sectorsize)
{	
	MEMORY_FILE* pFile =(MEMORY_FILE*)file;
	*sectorsize = 4;
	return pFile->FileReadCur%4;
}

void * memory_duplicate_handle(void *file){
	MEMORY_FILE* pFile2 = me_malloc(sizeof(MEMORY_FILE));
	if(pFile2==NULL){
		return NULL;
	}
	mp_memcpy((char *)pFile2,(char *)file,sizeof(MEMORY_FILE));
	return (void *)pFile2;
}
fs_callbacks	 g_memory_baseband_file	= {memory_filedelete, memory_fileopen, memory_fileclose,memory_fileread, memory_filewrite,memory_fileseek,memory_filetell,memory_fileeof,
										memory_block_fileread,memory_block_filewrite,memory_get_sector_offset,memory_duplicate_handle};

#endif

MMD_S32 TCL_RAM_fileread(void* file, MMD_S08* buffer, MMD_S32 count)
{
	MEMORY_FILE* 	pFile =(MEMORY_FILE* )file;
	MMD_U32		cnt = 0;
	 MSG_ERROR("in TCL_RAM_fileread", 0, 0, 0);
	if(pFile->FileReadCur>=pFile->FileTotalLength)
		return 0;

	cnt = count>(pFile->FileTotalLength-pFile->FileReadCur)?(pFile->FileTotalLength-pFile->FileReadCur):count;
	mp_memcpy((char*)buffer, (const char*)pFile->FileAddress+pFile->FileReadCur,cnt);
	pFile->FileReadCur+=cnt;
	return cnt;
}
MMD_S32 TCL_RAM_filewrite(void* file, MMD_S08* buffer, MMD_S32 count)
{
	MEMORY_FILE* pFile =(MEMORY_FILE*)file;

	mp_memcpy((char*)pFile->FileAddress+pFile->FileWriteCur,(const char*)buffer, count);
	pFile->FileWriteCur+=count;
	pFile->FileTotalLength+=count;
	return count;
}

MMD_S32 TCL_RAM_fileseek(void* file, MMD_S32 offset, MMD_S32 mode)
{
	MEMORY_FILE* pFile =(MEMORY_FILE*)file;
	MSG_ERROR("in TCL_RAM_fileseek", 0, 0, 0);
	switch(mode)
	{
	case SEEK_SET:
		pFile->FileReadCur = offset;
		break;
	case SEEK_CUR:
		pFile->FileReadCur += offset;
		break;
	case SEEK_END:
		pFile->FileReadCur = pFile->FileTotalLength+offset;
		break;
	default:
		break;
	}
	if(pFile->FileReadCur > pFile->FileTotalLength)
		return pFile->FileReadCur;
	else
		return 0;
}

MMD_S32 TCL_RAM_filetell(void* file)
{
	MEMORY_FILE* pFile =(MEMORY_FILE*)file;
	return pFile->FileReadCur;
}

MMD_S32 TCL_RAM_fileeof(void* file)
{
	MEMORY_FILE* pFile =(MEMORY_FILE*)file;
	if(pFile->FileReadCur >= pFile->FileTotalLength)
		return pFile->FileReadCur;
	else
		return 0;
}

void* TCL_RAM_fileopen(void *address, void *size)
{
	MEMORY_FILE* file=NULL;
	int *filelength = (int *)size;
	MSG_ERROR("in TCL_RAM_fileopen", 0, 0, 0);
	file=(MEMORY_FILE*)mp_malloc(sizeof(MEMORY_FILE));
	if(!file)
	{
		//ME_ERROR(("MP_ERR_MALLOC_MEMORY_ERROR"));
		return NULL;
	}
	file->FileAddress 		= (char*)address;
	file->FileTotalLength 	= (int)*filelength;
	file->FileReadCur 		= file->FileWriteCur = 0;
	return (void*)file;
}

MMD_S32 TCL_RAM_fileclose(void* file)
{
	MSG_ERROR("in TCL_RAM_fileclose", 0, 0, 0);
	mp_free((void*)file);
	return 0;
}

MMD_S32 TCL_RAM_filedelete(void *address)
{
	return 0;
}

#endif

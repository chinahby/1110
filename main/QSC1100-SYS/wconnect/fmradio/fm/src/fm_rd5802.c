/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include <string.h>

#include "custfmrds.h"          // include global fm rds switches

#include "target.h"
#include "comdef.h"
#include "clk.h"
#include "clkrgm_msm.h"

#ifdef FEATURE_FM

#include "fm_rd5802.h"       // include si4703 definitions

#if (defined(T_QSC1100) || defined(T_QSC1110))
#include "pm.h"                 // include pmic defintions if needed
#endif

/******************************************************************************
 ** File Name:      fm_drv.c                                                  *
 ** Author:         Wangliang                                             	  *
 ** DATE:           11/20/2009                                                *
 ** Description:                                               				  *
 ******************************************************************************

 ******************************************************************************
 **                        Edit History                                       *
 ** ------------------------------------------------------------------------- *
 ** DATE           NAME             DESCRIPTION                               *
 ** 11/20/2009     Wangliang         
 ******************************************************************************/

/**---------------------------------------------------------------------------*
 **                         Dependencies                                      *
 **---------------------------------------------------------------------------*/


/**---------------------------------------------------------------------------*
 **                         Debugging Flag                                    *
 **---------------------------------------------------------------------------*/

/**---------------------------------------------------------------------------*
 **                         Compiler Flag                                     *
 **---------------------------------------------------------------------------*/
/**---------------------------------------------------------------------------*
 **                         Const variables                                   *
 **---------------------------------------------------------------------------*/
 
 /**---------------------------------------------------------------------------*
 **                         Macro			                                  *
 **---------------------------------------------------------------------------*/


/***************************************************
Extern Definitions and Types
****************************************************/

/***************************************************
Local Definitions and Types
****************************************************/
/***************************************************
RDA5802 interfaces
****************************************************/
static void  	RDA5802_PowerOn(void);
static void  	RDA5802_PowerOffProc(void);
static void		RDA5802_ChipInit(void);
static boolean	RDA5802_Mute(boolean mute);
static uint32	RDA5802_FreqToChan(uint32 frequency);
static boolean	RDA5802_SetFreq(int16 curf);
static uint8 	RDA5802_ValidStop(int16 freq);
static uint8 	RDA5802_GetSigLvl(int16 curf );
static boolean	RDA5802_SetVolumeLevel(uint8 level);
static boolean  RDA5802_Intialization(void);

/* Used to send I2C command */
i2c_rw_cmd_type fm_i2c_command;

/***************************************************
Local variables
****************************************************/

static int16 CurrFrequency = -1;

//static uint8 fm_comm_buf[100];
/***************************************************
RDA5802
****************************************************/
uint8 RDA5802_initialization_reg[]={
	0xC0, 0x01, //02H: 
	0x00, 0x00,        
	0x04, 0x00,        
	0x88, 0xAF, //05H: 
	0x00, 0x00,        
	0x46, 0xC6,        
	0x50, 0x96,        
	0x00, 0x00,        
	0x40, 0x00, //0AH: 
	0x00, 0x8F,        
	0x00, 0x00,        
	0x00, 0x00,        
	0x00, 0x00,        
	0x00, 0x00,        
	0xF0, 0x05, //10H: 
	0x90, 0x00,        
	0xF4, 0x84,        
	0x70, 0x01,        
	0x40, 0xF0,        
	0x21, 0x80, //15H: 
	0x7A, 0xD0,        
	0x3E, 0x40,        
	0x55, 0xA9,        
	0xE8, 0x48,        
	0x50, 0x80, //1AH: 
	0x00, 0x00,        
	0x37, 0xB6,        
	0x40, 0x0C,        
	0x07, 0x9B,        
	0x4C, 0x1D,        
	0x81, 0x11, //20H: 
	0x45, 0x80,                    
};

static void HardwareCommInit(void)
{
	/* Configure I2C parameters */
	fm_i2c_command.bus_id     = I2C_BUS_HW_CTRL;
	fm_i2c_command.slave_addr = FM_RD5802_I2C_ID;
	/*lint -save -e655 */
	fm_i2c_command.options    = (i2c_options_type) (I2C_DFLT_ADDR_DEV | I2C_START_BEFORE_READ);
}

static uint8 OperationRDAFM_2w(FM_I2C_COMM_TYPE operation, uint8 *data, uint8 numBytes)
{
	uint8  	i = 0;
	uint8   ret = 0;
	
	if (data == NULL)
	{
		return FALSE;
	}

	fm_i2c_command.addr.reg = 0;
	fm_i2c_command.buf_ptr  = (byte *)(data);
	fm_i2c_command.len      = numBytes;

	for (i =0; i < 3; ++i)
	{
		if ( operation == FM_I2C_READ )
		{
			if (i2c_read(&fm_i2c_command) == I2C_SUCCESS)
			{
				ret = 1;
				break;
			}
		}
		else
		{
			if (i2c_write(&fm_i2c_command) == I2C_SUCCESS)
			{
				ret = 1;
				break;
			}
		}
	}

	if ( ret == 1 )
	{
		return TRUE;
	}

	ERR("OperationRDAFM_2w error numBytes =%d",numBytes,0,0);		
   
	return FALSE;

}

static void  RDA5802_PowerOn(void)
{	
	ERR("RDA5802_PowerOnReset!!!",0,0,0);
	pm_vote_clk_32k(ON, PM_32K_CLK_FM_APP);
}

static void  RDA5802_PowerOffProc(void)
{
	uint8 RDA5802_poweroff[] ={0x00,0x00};  

	ERR("RDA5802_PowerOffProc!!!",0,0,0);
	OperationRDAFM_2w(FM_I2C_WRITE, &(RDA5802_poweroff[0]), 2);
	pm_vote_clk_32k(OFF, PM_32K_CLK_FM_APP);
}

static void RDA5802_ChipInit(void)
{
	ERR("RDA5802_ChipInit!!!",0,0,0);
	RDA5802_PowerOn();
	clk_busy_wait(10*1000);	//Dealy 10ms
	HardwareCommInit();
	RDA5802_Intialization();
	//RDA5802_PowerOffProc();
}

static boolean RDA5802_Mute(boolean mute)
{
	return TRUE;
}

static uint32 RDA5802_FreqToChan(uint32 frequency) 
{
	uint8 	channelSpacing = 0;
	uint32 	bottomOfBand = 0;
	uint32 	channel = 0;

	if ((RDA5802_initialization_reg[3] & 0x0c) == 0x00)
	{
		bottomOfBand = 870;
	}
	else if ((RDA5802_initialization_reg[3] & 0x0c) == 0x04)
	{
		bottomOfBand = 760;
	}
	else if ((RDA5802_initialization_reg[3] & 0x0c) == 0x08)	
	{
		bottomOfBand = 760;	
	}
	if ((RDA5802_initialization_reg[3] & 0x03) == 0x00) 
	{
		channelSpacing = 1;
	}
	else if ((RDA5802_initialization_reg[3] & 0x03) == 0x01) 
	{
		channelSpacing = 2;
	}

	channel = (frequency - bottomOfBand) / channelSpacing;
	return (channel);
}

static  boolean RDA5802_SetFreq(int16 curFreq)
{   
	uint8 	RDA5802_channel_start_tune[] ={0xc0,0x01,0x00,0x10}; 	//87.0MHz
	uint8 	RDA5802_reg_data[4]={0};
	uint32 	curChan;

	CurrFrequency = curFreq;
	curChan=RDA5802_FreqToChan(curFreq);
	RDA5802_channel_start_tune[2]=curChan>>2;
	RDA5802_channel_start_tune[3]=(((curChan&0x0003)<<6)|0x10) | (RDA5802_initialization_reg[3]&0x0f);	//set tune bit
	
	OperationRDAFM_2w(FM_I2C_WRITE, &(RDA5802_channel_start_tune[0]), 4);
	clk_busy_wait(30*1000); //at least delay 20ms
	OperationRDAFM_2w(FM_I2C_READ, &(RDA5802_reg_data[0]), 4);
	clk_busy_wait(200*1000); //at least delay 20ms
	
	if ( (RDA5802_reg_data[2]&0x01) == 0x01 )
	{
		ERR("RDA5802_SetFreq = %d is a station!",curFreq,0,0);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

static int16 RDA5802_GetFreq(void)
{
	return CurrFrequency;
}

/************************************************************************************
function : judge current frequency whether is a True station,  for Auto seek, seek 
功能: 判断当前频点是否为一个电台, 自动搜台,半自动搜台请调用这个函数 
input : Frequency, eg: 97.1Mhz -> 971
output: 1->current Frequency is True station, 0-> current Frequency is fail station, 	  
*************************************************************************************/
 
static  uint8 RDA5802_ValidStop(int16 freq) 
{
	uint8 	RDA5802_reg_data[4]={0};	
	uint8 	RDA5802_channel_seek[] = {0xc0,0x01};
	uint8 	falseStation = 0;
	uint32  curChan;
	
	curChan=RDA5802_FreqToChan(freq);
	
	RDA5802_reg_data[0]=RDA5802_channel_seek[0];
	RDA5802_reg_data[1]=RDA5802_channel_seek[1];
	RDA5802_reg_data[2]=curChan>>2;
	RDA5802_reg_data[3]=(((curChan&0x0003)<<6)|0x10) | (RDA5802_initialization_reg[3]&0x0f);
	OperationRDAFM_2w(FM_I2C_WRITE,&(RDA5802_reg_data[0]), 4);
	
	clk_busy_wait(50*1000);  	//at least Delay 25ms

	OperationRDAFM_2w(FM_I2C_READ,&(RDA5802_reg_data[0]), 4);
	clk_busy_wait(100*1000);  	//at least Delay 25ms
	//check whether STC=1
	
	if((RDA5802_reg_data[0]&0x40)==0)
	{
		falseStation=1;
	}
	//check FM_TURE
	if((RDA5802_reg_data[2] &0x01)==0)
	{
		falseStation=1;
	}
	
	if (falseStation==1)
	{
		return 0;   
	}
	else 
	{
		return 1;
	}
}

static  uint8 RDA5802_GetSigLvl(int16 curf )  /*当满足rssi 的条件时，将信号记录，再选最强的9个频段*/
{    
	uint8 RDA5802_reg_data[4]={0};	

	ERR("RDA5802_GetSigLvl!!!",0,0,0);
	OperationRDAFM_2w(FM_I2C_READ,&(RDA5802_reg_data[0]), 4);
	return  (RDA5802_reg_data[2]>>1);  /*返回rssi*/
}

static  boolean RDA5802_SetVolumeLevel(uint8 level)   /*一般不调用，即不用芯片来调节音量。*/
{
	if ( level > 0x0f )
	{
		return FALSE;
	}
	
	RDA5802_initialization_reg[7]=( RDA5802_initialization_reg[7] & 0xf0 ) | level; 
	OperationRDAFM_2w(FM_I2C_WRITE, &(RDA5802_initialization_reg[0]), 8);
	return TRUE;
}

static  boolean  RDA5802_Intialization(void)
{
	uint8 error_ind = 0;
	uint8 RDA5802_REG[]={0x00,0x02};

	error_ind = OperationRDAFM_2w(FM_I2C_WRITE, (uint8 *)&RDA5802_REG[0], 2);
	clk_busy_wait(20*1000);			//Must Delay 10ms
	
	error_ind = OperationRDAFM_2w(FM_I2C_WRITE, (uint8 *)&RDA5802_initialization_reg[0], sizeof(RDA5802_initialization_reg));
	clk_busy_wait(50*1000);		//Must Delay 20ms	
	
    if (error_ind)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

static uint16 RDA5802_Seek(uint8 seekDirection) 
{
	uint8 	falseStation=0;
	uint32 	StartFrequency; 
	
	StartFrequency=CurrFrequency;	 /*stop flag*/

	do
	{
		falseStation=1;	    		

		if(seekDirection)
		{
			CurrFrequency++;  /*seek up*/
		}
		else
		{
			CurrFrequency--;  /*seek down*/
		}
		
		if(CurrFrequency>1080)
		{
			CurrFrequency=875; /* china band */
		}
		
		if(CurrFrequency<875)
		{
			CurrFrequency=1080;
		}
		
		if(RDA5802_ValidStop(CurrFrequency)==0) 
		{
			falseStation =0;      		
		}
	}
	while((falseStation==1)&&(StartFrequency!=CurrFrequency));	
		
	return CurrFrequency;
}

void RDA5802_Test(void)
{
	int16 i=0;
	boolean ret = FALSE;
	
	RDA5802_ChipInit();

	for(i=875; i<1080; i++ )
	{
		ret = RDA5802_SetFreq(i);

		ERR("FM: Get channel : %d ret = %d",i,ret,0);
	}
}

static boolean RDA5802_SetBand(uint32 band)
{
	return TRUE;
}

#endif //FEATURE_FM
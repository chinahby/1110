#include "fm_radio.h"
#include "Assert.h"
#include "OEMCFGI.h"
#include "snd.h"
#include "err.h"
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
#define FM_RD5802_I2C_ID	(0x20)
	
typedef enum
{
	FM_I2C_WRITE,
	FM_I2C_READ,
	FM_I2C_MAX
}FM_I2C_COMM_TYPE;


/***************************************************
Extern Definitions and Types
****************************************************/

/***************************************************
Local Definitions and Types
****************************************************/
static uint32	RDA5802_FreqToChan(uint32 frequency);
static uint8 	RDA5802_ValidStop(int16 freq);
static uint8 	RDA5802_GetSigLvl(int16 curf );
static uint8    fm_playing_mute = TRUE;
/***************************************************
RDA5802 interfaces
****************************************************/
static fm_status_type fm_work_status = FM_NOT_INIT;
static word fm_playing_channel = 971;

/* Used to send I2C command */
i2c_rw_cmd_type fm_i2c_command;

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

static uint8 OperationRDAFM_2w(FM_I2C_COMM_TYPE operation, uint8 *data, uint8 numBytes)
{
	uint8  	i = 0;
	uint8   ret = 0;
	
	if (data == NULL)
	{
		return FALSE;
	}

	/* Configure I2C parameters */
	fm_i2c_command.bus_id     = I2C_BUS_HW_CTRL;
	fm_i2c_command.slave_addr = FM_RD5802_I2C_ID;
	/*lint -save -e655 */
	fm_i2c_command.options    = (i2c_options_type) (I2C_DFLT_ADDR_DEV | I2C_START_BEFORE_READ);
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
		ERR("OperationRDAFM_2w OK numBytes =%d",numBytes,0,0);
		return TRUE;
	}

	ERR("OperationRDAFM_2w error numBytes =%d",numBytes,0,0);		
   
	return FALSE;

}

static  uint8 RDA5802_GetSigLvl(int16 curf )  /*当满足rssi 的条件时，将信号记录，再选最强的9个频段*/
{    
	uint8 RDA5802_reg_data[4]={0};	

	ERR("RDA5802_GetSigLvl!!!",0,0,0);
	OperationRDAFM_2w(FM_I2C_READ,&(RDA5802_reg_data[0]), 4);
	return  (RDA5802_reg_data[2]>>1);  /*返回rssi*/
}


boolean fm_radio_is_inited( void)
{
    return fm_work_status > FM_NOT_INIT;
}

boolean fm_radio_is_power_up( void)
{    
    return fm_work_status >= FM_POWER_UP;
}

boolean fm_radio_is_power_down( void)
{
    
    return fm_work_status == FM_POWER_DOWN;
}

boolean fm_radio_is_tuning( void)
{    
    return fm_work_status == FM_IN_PROGRESS;
}

word fm_radio_get_playing_channel( void)
{ 
    return fm_playing_channel;
}

void fm_radio_pre_init(void)
{
	ERR("fm_radio_pre_init!!!",0,0,0);
	return;
}

int fm_radio_init(void)
{
	uint8 error_ind = 0;
	uint8 RDA5802_REG[]={0x00,0x02};
	
	ERR("fm_radio_init!!!",0,0,0);
#if (defined(T_QSC1100) || defined(T_QSC1110))
	// qsc1100 platform
	// - enable platform power here if necessary
	pm_vote_clk_32k(ON, PM_32K_CLK_FM_APP);  // vote to enable 32KHz clock
	clk_busy_wait(20*1000);			//Must Delay 10ms
#else
	// other platforms
	// - enable platform here
#endif

	fm_playing_mute = TRUE;
	error_ind = OperationRDAFM_2w(FM_I2C_WRITE, (uint8 *)&RDA5802_REG[0], 2);
	clk_busy_wait(20*1000);			//Must Delay 10ms
	
	error_ind = OperationRDAFM_2w(FM_I2C_WRITE, (uint8 *)&RDA5802_initialization_reg[0], sizeof(RDA5802_initialization_reg));
	clk_busy_wait(50*1000);
	
	fm_work_status = FM_POWER_DOWN;
	return FM_RADIO_SUCCESSFUL;
}

int fm_radio_power_up(void)
{
	ERR("fm_radio_power_up start!!!",0,0,0);
	
	if(fm_work_status == FM_NOT_INIT)
    {
		return FM_RADIO_FAILED;
    }        
    else if(fm_work_status == FM_IN_PROGRESS)
    {
        return FM_RADIO_SUCCESSFUL;
    }  

	fm_work_status = FM_POWER_UP; 

	return FM_RADIO_SUCCESSFUL;
}

int fm_radio_power_down(void)
{
	uint8 RDA5802_poweroff[] ={0x00,0x00};  

	if(fm_work_status == FM_NOT_INIT)
    {
        return FM_RADIO_FAILED;
    }
    
	ERR("fm_radio_power_down!!!",0,0,0);
	OperationRDAFM_2w(FM_I2C_WRITE, &(RDA5802_poweroff[0]), 2);
	
#if (defined(T_QSC1100) || defined(T_QSC1110))
	// qsc1100 platform
	//pm_vote_clk_32k(OFF, PM_32K_CLK_FM_APP);  // vote to disable 32KHz clock
	// - disable platform power here if necessary
#else
	// other platforms
	// - disable platform power here
#endif

	fm_work_status = FM_POWER_DOWN;
	return FM_RADIO_SUCCESSFUL;
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


static uint16 RDA5802_Seek(uint8 seekDirection) 
{
	uint8 	falseStation=0;
	uint32 	StartFrequency; 
	
	StartFrequency=fm_playing_channel;	 /*stop flag*/

	do
	{
		falseStation=1;	    		

		if(seekDirection)
		{
			fm_playing_channel++;  /*seek up*/
		}
		else
		{
			fm_playing_channel--;  /*seek down*/
		}
		
		if(fm_playing_channel>1080)
		{
			fm_playing_channel=875; /* china band */
		}
		
		if(fm_playing_channel<875)
		{
			fm_playing_channel=1080;
		}
		
		if(RDA5802_ValidStop(fm_playing_channel)==0) 
		{
			falseStation =0;      		
		}
	}
	while((falseStation==1)&&(StartFrequency!=fm_playing_channel));	
		
	return fm_playing_channel;
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


int fm_tune_channel(word wChannel)
{
	uint8 	RDA5802_channel_start_tune[] ={0xc0,0x01,0x00,0x10}; 	//87.0MHz
	uint8 	RDA5802_reg_data[4]={0};
	uint32 	curChan;

	ERR("fm_tune_channel wChannel = %d!!!",wChannel,0,0);
	if(fm_work_status != FM_IDLE_STATUS)
    {
        return FM_RADIO_FAILED;
    }      

	if ( (wChannel < 875) || (wChannel > 1080))
	{
		return FM_RADIO_FAILED;
	}

	
    fm_work_status = FM_IN_PROGRESS;

    
	fm_playing_channel = wChannel;
	curChan=RDA5802_FreqToChan(wChannel);
	RDA5802_channel_start_tune[2]=curChan>>2;
	RDA5802_channel_start_tune[3]=(((curChan&0x0003)<<6)|0x10) | (RDA5802_initialization_reg[3]&0x0f);	//set tune bit
	
	OperationRDAFM_2w(FM_I2C_WRITE, &(RDA5802_channel_start_tune[0]), 4);
	clk_busy_wait(30*1000); //at least delay 20ms
	OperationRDAFM_2w(FM_I2C_READ, &(RDA5802_reg_data[0]), 4);
	clk_busy_wait(200*1000); //at least delay 20ms


	if ( (RDA5802_reg_data[2]&0x01) == 0x01 )
	{
		ERR("RDA5802_SetFreq = %d is a station!",wChannel,0,0);
		fm_work_status = FM_IDLE_STATUS;
    	fm_playing_channel = wChannel;
		return FM_RADIO_SUCCESSFUL;
	}
	else
	{
		ERR("RDA5802_SetFreq = %d is not a station!",wChannel,0,0);
		fm_work_status = FM_IDLE_STATUS;
    	fm_playing_channel = wChannel;
		return FM_RADIO_FAILED;
	}
}

//Synchronization Seek Function
int fm_seek_up(word* pChannel)
{
	if(fm_work_status != FM_IDLE_STATUS)
    {
        return FM_RADIO_FAILED;
    }    

    fm_work_status = FM_IN_PROGRESS;

	ERR("fm_seek_up!!!",0,0,0);
    
	fm_playing_channel = RDA5802_Seek(1);
	*pChannel = fm_playing_channel;

	fm_work_status = FM_IDLE_STATUS;
	
	return FM_RADIO_SUCCESSFUL;
}

int fm_seek_down(word* pChannel)
{
	if(fm_work_status != FM_IDLE_STATUS)
    {
        return FM_RADIO_FAILED;
    }      

    fm_work_status = FM_IN_PROGRESS;

	ERR("fm_seek_down!!!",0,0,0);
    
	fm_playing_channel = RDA5802_Seek(0);
	*pChannel = fm_playing_channel;

	fm_work_status = FM_IDLE_STATUS;
	return FM_RADIO_SUCCESSFUL;
}

//Asynchronization Seek Function
int fm_seek_start(boolean bIsUp, boolean bIsWrap)
{
	fm_work_status = FM_IDLE_STATUS;
	return FM_RADIO_SUCCESSFUL;
}

int fm_get_seek_status(boolean* pIsFinish, boolean* pIsBandLimit, word* pChannel)
{
	fm_work_status = FM_IDLE_STATUS;
	return FM_RADIO_SUCCESSFUL;
}

int fm_set_volume(word wVolume)
{
	uint8 level = (uint8)wVolume;
	if((wVolume == 0) && (fm_playing_mute == FALSE))
    {
        fm_mute(TRUE);
        return FM_RADIO_SUCCESSFUL;
    }
    else if((wVolume > 0) && (fm_playing_mute == TRUE))
    {
        fm_mute(FALSE);
    }
	if(fm_work_status != FM_IDLE_STATUS)
    {
        return FM_RADIO_FAILED;
    } 	

	if ( level > 0x0f )
	{
		return FM_RADIO_FAILED;
	}
	
	RDA5802_initialization_reg[7]=( RDA5802_initialization_reg[7] & 0xf0 ) | level; 
	OperationRDAFM_2w(FM_I2C_WRITE, &(RDA5802_initialization_reg[0]), 8);
	return FM_RADIO_SUCCESSFUL;
}

#include "uixsnd.h"
void fm_mute(boolean on)
{	
	if ( ( on == TRUE) && (fm_playing_mute == FALSE) )
	{
		fm_playing_mute = TRUE;
		//fm_set_volume(0);
		//snd_set_device(SND_DEVICE_HEADSET_FM, SND_MUTE_MUTED, SND_MUTE_MUTED, NULL, NULL);	//Del By zzg 2010_07_18
		
		//Add By zzg 2010_07_18
		snd_set_device(SND_DEVICE_HEADSET_FM, SND_MUTE_MUTED, SND_MUTE_MUTED, NULL, NULL);	
		snd_set_device(SND_DEVICE_STEREO_HEADSET, SND_MUTE_UNMUTED, SND_MUTE_UNMUTED, NULL, NULL);	
		//Add End
	}
	else if ( ( on == FALSE) && (fm_playing_mute == TRUE) )
	{
		fm_playing_mute = FALSE;
		//fm_set_volume(8);
		//snd_set_device(SND_DEVICE_HEADSET_FM, SND_MUTE_UNMUTED, SND_MUTE_UNMUTED, NULL, NULL);	//Del By zzg 2010_07_18

		//Add By zzg 2010_07_18
		snd_set_device(SND_DEVICE_STEREO_HEADSET, SND_MUTE_MUTED, SND_MUTE_MUTED, NULL, NULL);	
		snd_set_device(SND_DEVICE_HEADSET_FM, SND_MUTE_UNMUTED, SND_MUTE_UNMUTED, NULL, NULL);
		//Add End	
	}
	return;
}

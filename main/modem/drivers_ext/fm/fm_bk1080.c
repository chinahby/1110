#include "fm_radio.h"
#include "Assert.h"
#include "OEMCFGI.h"
#include "snd.h"
#include "err.h"
/******************************************************************************
 ** File Name:      fm_bk1080.c                                                  *
 ** Author:         Wangliang                                             	  *
 ** DATE:           2010/13/03                                                *
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
#define FM_BK1080_I2C_ID	(0x80)
	
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
static uint32	BK1080_FreqToChan(uint32 frequency);
static uint8 	BK1080_ValidStop(int16 freq);
static uint8 	BK1080_GetSigLvl(int16 curf );
static uint8    fm_playing_mute = TRUE;
/***************************************************
BK1080 interfaces
****************************************************/
static fm_status_type fm_work_status = FM_NOT_INIT;
static word fm_playing_channel = 971;

/* Used to send I2C command */
static i2c_rw_cmd_type fm_i2c_command;
static uint8 Operation_Beken_FM_2w(uint8 operation, uint8 *data,uint8 start_reg, uint8 numBytes);


static uint8 Operation_Beken_FM_2w(uint8 operation, uint8 *data,uint8 start_reg, uint8 numBytes)
{
	uint8  	i = 0;
	uint8   ret = 0;
	
	if (data == NULL)
	{
		return FALSE;
	}

	/* Configure I2C parameters */
	fm_i2c_command.bus_id     	= I2C_BUS_HW_CTRL;
	fm_i2c_command.slave_addr 	= FM_BK1080_I2C_ID;
	/*lint -save -e655 */
	fm_i2c_command.options    	= (i2c_options_type) (I2C_DFLT_ADDR_DEV | I2C_STOP_START_BEFORE_READ | I2C_DATA_REG_WRITE );

	if ( operation == FM_I2C_READ )
	{
		fm_i2c_command.addr.reg 	= (start_reg<<1)|0x01;
	}
	else
	{
		fm_i2c_command.addr.reg 	= start_reg<<1;
	}
	
	fm_i2c_command.buf_ptr  	= (byte *)(data);
	fm_i2c_command.len      	= numBytes;

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
		MSG_FATAL("OperationRDAFM_2w OK numBytes =%d",numBytes,0,0);
		return TRUE;
	}

	MSG_FATAL("OperationRDAFM_2w error numBytes =%d",numBytes,0,0);		
   
	return FALSE;
}

static uint8 OperationRDAFM_2w(FM_I2C_COMM_TYPE operation, uint8 *data, uint8 numBytes)
{
	//
}

static  uint8 BK1080_GetSigLvl(int16 curf )  /*当满足rssi 的条件时，将信号记录，再选最强的9个频段*/
{    
	uint8 BK1080_reg_data[4]={0};	

	MSG_FATAL("BK1080_GetSigLvl!!!",0,0,0);
	OperationRDAFM_2w(FM_I2C_READ,&(BK1080_reg_data[0]), 4);
	return  (BK1080_reg_data[2]>>1);  /*返回rssi*/
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
	MSG_FATAL("fm_radio_pre_init!!!",0,0,0);
	return;
}

int fm_radio_init(void)
{
	uint8 error_ind = 0;
	uint8 ReadData8[2]={0};
	
	MSG_FATAL("fm_radio_init!!!",0,0,0);
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
	Operation_Beken_FM_2w(FM_I2C_READ,ReadData8,1,2);//check device ID
	clk_busy_wait(20*1000);			//Must Delay 10ms

	MSG_FATAL("[FM]: ID = 0x%x%x",ReadData8[0],ReadData8[1],0);
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
	uint8 BK1080_poweroff[] ={0x00,0x00};  

	if(fm_work_status == FM_NOT_INIT)
    {
        return FM_RADIO_FAILED;
    }
    
	MSG_FATAL("fm_radio_power_down!!!",0,0,0);
	OperationRDAFM_2w(FM_I2C_WRITE, &(BK1080_poweroff[0]), 2);
	
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
 
static  uint8 BK1080_ValidStop(int16 freq) 
{
	uint8 	BK1080_reg_data[4]={0};	
	uint8 	BK1080_channel_seek[] = {0xc0,0x01};
	uint8 	falseStation = 0;
	uint32  curChan;
	
	curChan=BK1080_FreqToChan(freq);
	
	BK1080_reg_data[0]=BK1080_channel_seek[0];
	BK1080_reg_data[1]=BK1080_channel_seek[1];
	BK1080_reg_data[2]=curChan>>2;
	//BK1080_reg_data[3]=(((curChan&0x0003)<<6)|0x10) | (BK1080_initialization_reg[3]&0x0f);
	OperationRDAFM_2w(FM_I2C_WRITE,&(BK1080_reg_data[0]), 4);
	
	clk_busy_wait(50*1000);  	//at least Delay 25ms

	OperationRDAFM_2w(FM_I2C_READ,&(BK1080_reg_data[0]), 4);
	clk_busy_wait(100*1000);  	//at least Delay 25ms
	//check whether STC=1
	
	if((BK1080_reg_data[0]&0x40)==0)
	{
		falseStation=1;
	}
	//check FM_TURE
	if((BK1080_reg_data[2] &0x01)==0)
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


static uint16 BK1080_Seek(uint8 seekDirection) 
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
		
		if(BK1080_ValidStop(fm_playing_channel)==0) 
		{
			falseStation =0;      		
		}
	}
	while((falseStation==1)&&(StartFrequency!=fm_playing_channel));	
		
	return fm_playing_channel;
}


static uint32 BK1080_FreqToChan(uint32 frequency) 
{
	uint8 	channelSpacing = 0;
	uint32 	bottomOfBand = 0;
	uint32 	channel = 0;

	channel = (frequency - bottomOfBand) / channelSpacing;
	return (channel);
}


int fm_tune_channel(word wChannel)
{
	uint8 	BK1080_channel_start_tune[] ={0xc0,0x01,0x00,0x10}; 	//87.0MHz
	uint8 	BK1080_reg_data[4]={0};
	uint32 	curChan;

	MSG_FATAL("fm_tune_channel wChannel = %d!!!",wChannel,0,0);
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
	curChan=BK1080_FreqToChan(wChannel);
	BK1080_channel_start_tune[2]=curChan>>2;
	//BK1080_channel_start_tune[3]=(((curChan&0x0003)<<6)|0x10) | (BK1080_initialization_reg[3]&0x0f);	//set tune bit
	
	OperationRDAFM_2w(FM_I2C_WRITE, &(BK1080_channel_start_tune[0]), 4);
	clk_busy_wait(30*1000); //at least delay 20ms
	OperationRDAFM_2w(FM_I2C_READ, &(BK1080_reg_data[0]), 4);
	clk_busy_wait(200*1000); //at least delay 20ms


	if ( (BK1080_reg_data[2]&0x01) == 0x01 )
	{
		MSG_FATAL("BK1080_SetFreq = %d is a station!",wChannel,0,0);
		fm_work_status = FM_IDLE_STATUS;
    	fm_playing_channel = wChannel;
		return FM_RADIO_SUCCESSFUL;
	}
	else
	{
		MSG_FATAL("BK1080_SetFreq = %d is not a station!",wChannel,0,0);
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

	MSG_FATAL("fm_seek_up!!!",0,0,0);
    
	fm_playing_channel = BK1080_Seek(1);
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

	MSG_FATAL("fm_seek_down!!!",0,0,0);
    
	fm_playing_channel = BK1080_Seek(0);
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

int fm_set_volume(word wVolume,boolean speaker)
{
	uint8 level = (uint8)wVolume;
	if((wVolume == 0) && (fm_playing_mute == FALSE))
    {
        fm_mute(TRUE,speaker);
        return FM_RADIO_SUCCESSFUL;
    }
    else if((wVolume > 0) && (fm_playing_mute == TRUE))
    {
        fm_mute(FALSE,speaker);
    }
	if(fm_work_status != FM_IDLE_STATUS)
    {
        return FM_RADIO_FAILED;
    } 	

	if ( level > 0x0f )
	{
		return FM_RADIO_FAILED;
	}
	
	return FM_RADIO_SUCCESSFUL;
}

#include "uixsnd.h"
#include "Hs_mb6550.h"

void fm_mute(boolean on,boolean speaker)
{	
	if ( ( on == TRUE) && (fm_playing_mute == FALSE) )
	{
		fm_playing_mute = TRUE;
        
		if (HS_HEADSET_ON())
		{
            uisnd_set_device_auto(NULL,NULL);
		}
	}
	else if ( ( on == FALSE) && (fm_playing_mute == TRUE) )
	{
		fm_playing_mute = FALSE;
        snd_set_device(SND_DEVICE_STEREO_HEADSET, SND_MUTE_MUTED, SND_MUTE_MUTED, NULL, NULL);	
        if(speaker)
        {
        	snd_set_device(SND_DEVICE_SPEAKER_FM, SND_MUTE_UNMUTED, SND_MUTE_UNMUTED, NULL, NULL);
        }
        else
        {
			snd_set_device(SND_DEVICE_HEADSET_FM, SND_MUTE_UNMUTED, SND_MUTE_UNMUTED, NULL, NULL);
		}
	}
	return;
}


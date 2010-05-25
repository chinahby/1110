/*===========================================================================

        FM   Radio  Driver for BCM2048 Chipset

DESCRIPTION
  All the declarations and definitions necessary to support interaction
  with the FM Radio functions for the MSM60x0.

REFERENCES

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

===========================================================================*/
#include "fm_radio.h"
#include "Assert.h"
#include "OEMCFGI.h"

/* change the fm pa control to vc_task */
#ifdef VC0848_HEADSET_PA
#include "vc0848.h"
#endif

#ifdef EXT_HEADSET_PA
#define FM_AUDIO_SWITCH     33                 //Using GPIO33 as audio switch
#define FM_AUDIO_ON()       gpio_invert_pupd_state(FM_AUDIO_SWITCH); 
#define FM_AUDIO_OFF()      gpio_recover_pupd_state(FM_AUDIO_SWITCH); 
#endif 

#if defined FEATURE_FM_SPACE_50KHZ
#define BTA_FM_STEP 50
#else
#define BTA_FM_STEP 100
#endif
#define LOWEST_BAND_FREQ            87500
#define HIGHEST_BAND_FREQ           108000

#define FM_BCM2048_LOG   0

static fm_status_type fm_work_status = FM_NOT_INIT;
static word fm_playing_channel = 0;

/* all the control with vc0848 should be done in vc_task, not to call these func any more */
#ifndef VC0848_HEADSET_PA
extern void MMD_SYS_PMU_SWITCH(int NORMAL);
extern unsigned int MMD_SYS_Mode_Switch(unsigned int mode);
extern unsigned int MMD_SYS_Codec_Switch(unsigned int mode);
extern unsigned int MMD_Media_Codec_Set_Play_Volume(unsigned int nLeft ,unsigned int nRight);
extern unsigned int MMD_Media_Codec_Set_MuxMode(unsigned int mode);
extern unsigned int MMD_Media_Codec_Set_Mute(unsigned int mode,unsigned int muteFlag);//mode=0,record 1,play ; muteFlag:o,unmute, 1,mute
extern void MMD_SYS_SET_CLK(void);
#endif

#ifdef  BCM2048_I2C_MODE
#define CHANNEL2FREQ(wChannel)      ((wChannel * BTA_FM_STEP) + LOWEST_BAND_FREQ)
#define CHANNEL2BCM2048(wChannel)   (CHANNEL2FREQ(wChannel) - 64000)    // FREQ offset 64MHz 

#define BCM2048_2FREQ(freq)         (freq + 64000) 
#define BCM2048_2CHANNEL(freq)      ((BCM2048_2FREQ(freq) - LOWEST_BAND_FREQ) / BTA_FM_STEP)

#define FM_SDIO_SET_LOW()   \
    BIO_OUT(BIO_GPIO_18_REG, BIO_GPIO_18_M, BIO_GPIO_18_LO_V)    
#define FM_SDIO_SET_HIGH()   \
    BIO_OUT(BIO_GPIO_18_REG, BIO_GPIO_18_M, BIO_GPIO_18_HI_V)        
  
//--------------------------------------------------------------------------    
#define  KICK_WATCHDOG()   /*lint -e717 do ... while(0) */  \
                           do {                             \
   (void)outp(HWIO_WDOG_RESET_ADDR, HWIO_WDOG_RESET_RMSK);  \
                              }                             \
                           while (0)                        \
                           /*lint +e717 */
//--------------------------------------------------------------------------                           
    
//Si470x Register address    
#define DEVICE_ID_ADDR          0x00        //TBC
#define CHIPSET_ID_ADDR         0x01
#define POWER_CFG_ADDR          0x02

#define FM_I2C_SLAVE_ADDR       0x22

#define FM_MAX_LOOP_TIME        0x0FFF

static i2c_rw_cmd_type i2c_rw_cmd;


/*******************FUNCTION*******************/

void Delay_ms(int time) // 1ms, by lizg
{
	int i,j;
	if(time>=10)
		rex_sleep(time);
	else
	{
		for(i=0;i<time;i++)
			for(j=0;j<0x500;j++);
	}	
}	

//Reset write buffer value to chipset reset value;
LOCAL void fm_init_i2c_cmd_data()
{
    i2c_rw_cmd.bus_id = I2C_BUS_HW_CTRL;
    i2c_rw_cmd.slave_addr = FM_I2C_SLAVE_ADDR;
    i2c_rw_cmd.options = I2C_REG_DEV;
  //  i2c_rw_cmd.buf_ptr = i2c_data_buf;
    i2c_rw_cmd.addr.reg = 0;
    i2c_rw_cmd.len = 0;
    i2c_rw_cmd.last_index = 0;
}    

LOCAL i2c_status_type fm_read_i2c_cmd(byte reg, byte * pData)
{   
    i2c_status_type     result;
    byte                tmp_buf;
    
    i2c_rw_cmd.buf_ptr = &tmp_buf;
    i2c_rw_cmd.addr.reg = reg;
    i2c_rw_cmd.len = 1;
    i2c_rw_cmd.last_index = 0;    
    
    result = i2c_read(&i2c_rw_cmd);  
    *pData = tmp_buf;
    
    return result; 
}

//This functions over write the reg by data
LOCAL i2c_status_type fm_write_i2c_cmd(byte reg, byte data)
{

    i2c_rw_cmd.buf_ptr = &data;
    i2c_rw_cmd.addr.reg = reg;
    i2c_rw_cmd.len = 1;
    i2c_rw_cmd.last_index = 0;    
    
    return i2c_write(&i2c_rw_cmd);
}

//This is a shadow write functions
LOCAL i2c_status_type fm_shadow_write_i2c_cmd(byte reg, byte mask, byte data)
{   
    byte reg_data;
    int iResult;
    data &= mask; 
    iResult = fm_read_i2c_cmd(reg, &reg_data); 
    if(FM_RADIO_SUCCESSFUL != iResult)
    {
        return iResult;
    }
     reg_data = (reg_data & (~mask)) | (data); 
    
    i2c_rw_cmd.buf_ptr = &reg_data;
    i2c_rw_cmd.addr.reg = reg;
    i2c_rw_cmd.len = 1;
    i2c_rw_cmd.last_index = 0;    
    
    return i2c_write(&i2c_rw_cmd);
}    
int fm_radio_init()     //TBC
{           
    //config GPIO_18 back to SDA
    gpio_i2c_config();
    
    //init i2c cmd data struct
    fm_init_i2c_cmd_data();   
    
    //enable the internal 
//    iResult |= fm_write_i2c_cmd(0x00, 0x01);      //ENABLE FM function
 
    fm_work_status = FM_POWER_DOWN;   
    return 1;
}

void bcm2048_reg_read()
{
    int read_loop;
    int iResult= 0;
    byte* pdata;

    for(read_loop=0;read_loop<= 0x0b;read_loop++)
    {
        iResult |= fm_read_i2c_cmd(read_loop, pdata);  
        if(FM_RADIO_SUCCESSFUL == iResult)
        {
            MSG_ERROR("bcm2048_reg[%d] = %d", read_loop, *pdata, 0); 
        }     
        else
        {
            MSG_ERROR("bcm2048_reg[%d] reading error!", read_loop, 0, 0);   
            break;
        }
    }
}

int fm_radio_power_up()
{   
    int  iResult = 0;
 
    if(fm_work_status == FM_NOT_INIT)
    {
        return FM_RADIO_FAILED;
    }        
    else if(fm_work_status == FM_IN_PROGRESS)
    {
        return FM_RADIO_SUCCESSFUL;
    }    
    
    iResult |= fm_write_i2c_cmd(0x00, 0x01);      //ENABLE FM function
    Delay_ms(20);

    iResult |= fm_write_i2c_cmd(0x05, 0x1c);      //audio ctrl
    
#if defined FEATURE_FM_SPACE_50KHZ
    iResult |= fm_write_i2c_cmd(0xfd, 50);    //bw: Set Channel Space to 50 KHz
#else
    iResult |= fm_write_i2c_cmd(0xfd, 100);  //Set Channel Space to 100 KHz
#endif //#if defined FEATURE_FM_SPACE_50KHZ

    if(FM_RADIO_SUCCESSFUL == iResult)
    {
        fm_work_status = FM_POWER_UP;
#ifdef EXT_HEADSET_PA        
        FM_AUDIO_ON();
#endif
    }
    
    return iResult;         
}

int fm_radio_power_down()
{
    int  iResult = 0;
    if(fm_work_status == FM_NOT_INIT)
    {
        return FM_RADIO_FAILED;
    }           
    iResult |= fm_write_i2c_cmd(0x05, 0x03);      //mute    
    iResult |= fm_write_i2c_cmd(0x00, 0x00);      //disable power
    Delay_ms(20);
    if(FM_RADIO_SUCCESSFUL == iResult)
    {
        fm_work_status = FM_POWER_DOWN;   
#ifdef EXT_HEADSET_PA        
        FM_AUDIO_OFF();
#endif
    }   
    return iResult;    
}

int fm_tune_channel(word wChannel)
{
    int  iResult = 0;   
    int  iLoopCounter = 0;     
    byte status;    
    
    if(fm_work_status != FM_IDLE_STATUS)
    {
        return FM_RADIO_FAILED;
    }      
    fm_work_status = FM_IN_PROGRESS;
            
    //Begin tune and check tune status
    iLoopCounter = 0;
    fm_playing_channel = 0;
    //set freq
    iResult |= fm_write_i2c_cmd(0x0b, (byte)(CHANNEL2BCM2048(wChannel) >> 8)); 
    iResult |= fm_write_i2c_cmd(0x0a, (byte)CHANNEL2BCM2048(wChannel));  
    //tuning new
    iResult |= fm_write_i2c_cmd(0x09, 0x01); 
    if(FM_RADIO_SUCCESSFUL != iResult)
    {
        MSG_ERROR("FM seek stop failed_0", 0, 0, 0);
        return iResult;
    }
    
    do
    {
        iResult |= fm_read_i2c_cmd(0x12, &status);        
    
        KICK_WATCHDOG();
    
        if(FM_RADIO_SUCCESSFUL != iResult)
        {
            fm_work_status = FM_IDLE_STATUS;
            MSG_ERROR("FM seek stop failed_1", 0, 0, 0);
            
            return iResult;
        }
        if((status & 0x02) == 1)
        {
            fm_work_status = FM_IDLE_STATUS;
            MSG_ERROR("FM search tune fail", 0, 0, 0);
            
            return iResult;
        }
        iLoopCounter++;
    }
    while(iLoopCounter < FM_MAX_LOOP_TIME && ((status & 0x01) == 0));  
    
    if(FM_MAX_LOOP_TIME == iLoopCounter)
    {
        fm_work_status = FM_IDLE_STATUS;
        MSG_ERROR("FM seek stop failed_2", 0, 0, 0);
  
        return iResult;
    }
    
    iResult |= fm_write_i2c_cmd(0x09, 0x00);      //TBC   //tuning off
             
    fm_work_status = FM_IDLE_STATUS;
    fm_playing_channel = wChannel;
    return iResult;
}


LOCAL int fm_do_seek(word* pChannel)
{
    int  iResult = 0;   
    int  iLoopCounter = 0;     
    byte status;
    word pFreq;
    
    //Begin seek and check seek status
    iLoopCounter = 0;
    
    //auto seek
    iResult |= fm_write_i2c_cmd(0x09, 0x02);        
    if(FM_RADIO_SUCCESSFUL != iResult)
    {
        MSG_ERROR("FM seek stop failed_0", 0, 0, 0);
        return iResult;
    }
    
    do
    {
        iResult |= fm_read_i2c_cmd(0x12, &status);        
    
        KICK_WATCHDOG();
    
        if(FM_RADIO_SUCCESSFUL != iResult)
        {
            fm_work_status = FM_IDLE_STATUS;
            MSG_ERROR("FM seek stop failed_1", 0, 0, 0);
            
            return iResult;
        }
        if((status & 0x02) == 1)
        {
            fm_work_status = FM_IDLE_STATUS;
            MSG_ERROR("FM search tune fail", 0, 0, 0);
            
            return iResult;
        }
        iLoopCounter++;
    }
    while(iLoopCounter < FM_MAX_LOOP_TIME && ((status & 0x01) == 0));  
    
    if(FM_MAX_LOOP_TIME == iLoopCounter)
    {
        fm_work_status = FM_IDLE_STATUS;
        MSG_ERROR("FM seek stop failed_2", 0, 0, 0);
            
        return iResult;
    }  
    
    //OK, seek completed. Let's check if seek successful.
    iResult |= fm_read_i2c_cmd(0x0a, &status);   
    pFreq = (word)status & 0x00FF;        
    iResult |= fm_read_i2c_cmd(0x0b, &status);        
    pFreq |= ((word)status<<8) & 0xFF00;
    
    *pChannel = BCM2048_2CHANNEL(pFreq);     
    
    return iResult;
}

int fm_seek_up(word* pChannel)
{
    int  iResult = 0;  
    
    if(fm_work_status != FM_IDLE_STATUS)
    {
        return FM_RADIO_FAILED;
    }      
    fm_work_status = FM_IN_PROGRESS;
    
    iResult |= fm_shadow_write_i2c_cmd(0x07,0x80,0x80);
    iResult |= fm_do_seek(pChannel);
    
    fm_work_status = FM_IDLE_STATUS;
    
    return iResult;
}

int fm_seek_down(word* pChannel)
{
    int  iResult = 0;   
    
    if(fm_work_status != FM_IDLE_STATUS)
    {
        return FM_RADIO_FAILED;
    }      
    fm_work_status = FM_IN_PROGRESS;
      
    iResult |= fm_shadow_write_i2c_cmd(0x07,0x80,0x00);
    iResult |= fm_do_seek(pChannel);
    
    fm_work_status = FM_IDLE_STATUS;
    
    return iResult;
}

int fm_seek_start(boolean bIsUp, boolean bIsWrap)
{   
    int  iResult = 0;

    if(fm_work_status != FM_IDLE_STATUS)
    {
        return FM_RADIO_FAILED;
    }          
    fm_work_status = FM_IN_PROGRESS;
    
    if(bIsUp)
    {        
        iResult |= fm_write_i2c_cmd(0x07, 0xbc);   //Set RSSI seek threshold 60
    }
    else
    {
        iResult |= fm_write_i2c_cmd(0x07, 0x3c);   //Set RSSI seek threshold 60
    }
    
//set if seek stop when rich band limit
    if(bIsWrap)
    {     
    }
    else
    {    
    }
    
//auto seek
    iResult |= fm_write_i2c_cmd(0x09, 0x02);  

    fm_work_status = FM_IDLE_STATUS;
    return iResult;
    
}

int fm_get_seek_status(boolean* pIsFinish, boolean* pIsBandLimit, word* pChannel)
{    
    int iResult = 0;  
    byte status;
    word pFreq;
    
    if(fm_work_status != FM_IDLE_STATUS)
    {
        return FM_RADIO_FAILED;
    }      
    fm_work_status = FM_IN_PROGRESS;
    
    iResult |= fm_read_i2c_cmd(0x12, &status);        
    
    if((status & 0x01) == 0)
    {
        *pIsFinish = FALSE;
    }
    else
    {   
        *pIsFinish = TRUE;        
             
        iResult |= fm_read_i2c_cmd(0x0A, &status);
        if((status & 0x02) == 0)
        {
            *pIsBandLimit = FALSE;
        }
        else
        {
            *pIsBandLimit = TRUE;            
        }
        
        iResult |= fm_read_i2c_cmd(0x0a, &status);   
        pFreq = (word)status & 0x00FF;        
        iResult |= fm_read_i2c_cmd(0x0b, &status);        
        pFreq |= ((word)status<<8) & 0xFF00;
    
        *pChannel = BCM2048_2CHANNEL(pFreq);   
        
    }
    
    fm_work_status = FM_IDLE_STATUS;
    
    bcm2048_reg_read();
    
    return iResult;
}

int fm_set_volume(word wVolume)
{
    return 1;
}

#else  // BCM2048_I2C_MODE

#include "rex.h"
#include "bta_fm_api.h"
#include "Bta_rds_api.h"

#define FM_MAX_LOOP_TIME        30
#define BCM2048_CBACK_SLEEP_DURATION        100
#define FM_BCM2048_THRESHOLD    80

#define FM_BCM2048_SCAN_UP      0
#define FM_BCM2048_SCAN_DOWN    1

#define CHANNEL2FREQ(wChannel)      (((wChannel * BTA_FM_STEP) + LOWEST_BAND_FREQ)/10)
#define FREQ2CHANNEL(FREQ)          (((FREQ * 10) - LOWEST_BAND_FREQ) / BTA_FM_STEP)


enum
{
    BTA_FM_NONE_EVT_FLAG,
    BTA_FM_ENABLE_EVT_FLAG,          /* BTA FM is enabled */
    BTA_FM_DISABLE_EVT_FLAG,         /* BTA FM is disabled */
    BTA_FM_TUNE_EVT_FLAG,            /* FM tuning is completed or failed */
    BTA_FM_SEARCH_EVT_FLAG,          /* FM scanning, clear channel update event */
    BTA_FM_SEARCH_CMPL_EVT_FLAG,     /* FM scanning is completed event */
    BTA_FM_AF_JMP_EVT_FLAG,          /* AF jump event */
    BTA_FM_AUD_MODE_EVT_FLAG,        /* Set audio mode completed  */
    BTA_FM_AUD_DATA_EVT_FLAG,        /* audio quality live updating call back event */
    BTA_FM_AUD_PATH_EVT_FLAG,        /* set audio path completed */
    BTA_FM_RDS_UPD_EVT_FLAG,         /* read RDS data event */
    BTA_FM_RDS_MODE_EVT_FLAG,        /* set RDS mode event */
    BTA_FM_SET_DEEMPH_EVT_FLAG,      /* configure deempahsis parameter event */
    BTA_FM_MUTE_AUD_EVT_FLAG,         /* audio mute/unmute event */
    BTA_FM_SCAN_STEP_EVT_FLAG,       /* config scan step */
    BTA_FM_SET_REGION_EVT_FLAG,       /* set band region */
    BTA_FM_NFL_EVT_FLAG              /* noise floor estimation event */
};
typedef UINT8  tBTA_FM_EVT_FLAG;

static tBTA_FM_EVT_FLAG bcm2048_fm_evt_flag = BTA_FM_NONE_EVT_FLAG;
static tBTA_FM bcm2048_fm_info;
static int fm_bcm2048_scan_direct;
static boolean s_fm_mute = FALSE;
static word fm_wVolume;

/*******************************************************************************
**
** Function         btui_fm_cback
**
** Description      process the events from FM
**                  
**
** Returns          void
*******************************************************************************/
void bcm2048_fm_cback(tBTA_FM_EVT event, tBTA_FM *p_data)
{
#if FM_BCM2048_LOG
    MSG_ERROR("bcm2048_fm_cback event:%d", event, 0, 0);
#endif
    switch(event)
    {
        case BTA_FM_ENABLE_EVT:
            bcm2048_fm_evt_flag = BTA_FM_ENABLE_EVT_FLAG;            
            bcm2048_fm_info.status = p_data->status;
#if FM_BCM2048_LOG           
            MSG_ERROR("BTA_FM_ENABLE_EVT,status= %d", bcm2048_fm_info.status, 0, 0);  
#endif 
            break;

        case BTA_FM_DISABLE_EVT:
            bcm2048_fm_evt_flag = BTA_FM_DISABLE_EVT_FLAG;            
            bcm2048_fm_info.status = p_data->status;
#if FM_BCM2048_LOG         
            MSG_ERROR("BTA_FM_DISABLE_EVT,status= %d", bcm2048_fm_info.status, 0, 0);   
#endif 
            break;
        case BTA_FM_AF_JMP_EVT:
            bcm2048_fm_evt_flag = BTA_FM_AF_JMP_EVT_FLAG;
                
            break;
        case BTA_FM_TUNE_EVT:
            bcm2048_fm_evt_flag = BTA_FM_TUNE_EVT_FLAG;     
            bcm2048_fm_info.chnl_info.status      = p_data->chnl_info.status;   
            bcm2048_fm_info.chnl_info.freq        = p_data->chnl_info.freq;
            bcm2048_fm_info.chnl_info.rssi        = p_data->chnl_info.rssi;
#if FM_BCM2048_LOG
            MSG_ERROR("BTA_FM_TUNE_EVT_FLAG,status= %d,freq= %d,rssi= %d", bcm2048_fm_info.chnl_info.status, 
                bcm2048_fm_info.chnl_info.freq, bcm2048_fm_info.chnl_info.rssi);        
#endif
            break;
            
        case BTA_FM_SEARCH_EVT:
            bcm2048_fm_evt_flag = BTA_FM_SEARCH_EVT_FLAG;                
            bcm2048_fm_info.scan_data.freq        = p_data->scan_data.freq;
            bcm2048_fm_info.scan_data.rssi        = p_data->scan_data.rssi;
#if FM_BCM2048_LOG
            MSG_ERROR("BTA_FM_SEARCH_EVT_FLAG,status= null,freq= %d,rssi= %d",bcm2048_fm_info.scan_data.freq,
            bcm2048_fm_info.scan_data.rssi,0);        
#endif 
            break;

        case BTA_FM_SEARCH_CMPL_EVT:
            bcm2048_fm_evt_flag = BTA_FM_SEARCH_CMPL_EVT_FLAG;                
            bcm2048_fm_info.chnl_info.status      = p_data->chnl_info.status;   
            bcm2048_fm_info.chnl_info.freq        = p_data->chnl_info.freq;
            bcm2048_fm_info.chnl_info.rssi        = p_data->chnl_info.rssi;
#if FM_BCM2048_LOG
            MSG_ERROR("BTA_FM_SEARCH_CMPL_EVT_FLAG,status= %d,freq= %d,rssi= %d", bcm2048_fm_info.chnl_info.status, 
                bcm2048_fm_info.chnl_info.freq, bcm2048_fm_info.chnl_info.rssi);        
#endif
            break;

        case BTA_FM_AUD_MODE_EVT:     
            break;

        case BTA_FM_AUD_DATA_EVT:           
            break;
            
        case BTA_FM_AUD_PATH_EVT:            
            break;
            
        case BTA_FM_MUTE_AUD_EVT: 
            bcm2048_fm_evt_flag = BTA_FM_MUTE_AUD_EVT_FLAG;          
            bcm2048_fm_info.mute_stat.status = p_data->mute_stat.status;   
#if FM_BCM2048_LOG
            MSG_ERROR("BTA_FM_MUTE_AUD_EVT,status= %d, mute= %d",bcm2048_fm_info.mute_stat.status,
            p_data->mute_stat.is_mute,0);        
#endif             
            break;
            
        case BTA_FM_SCAN_STEP_EVT:
            break;
            
        case BTA_FM_SET_REGION_EVT:
            break;
      
        default:
            break;
    }
}

void fm_bcm2048_mute(boolean is)
{
    int  fm_loop = 0;

    BTA_FmMute(is);
#if FM_BCM2048_LOG
    MSG_ERROR("fm_bcm2048_mute = %d", is, 0, 0);
#endif 
    for(fm_loop=0; fm_loop<FM_MAX_LOOP_TIME; fm_loop++)
    {
        //SET TIMER 
        rex_sleep(BCM2048_CBACK_SLEEP_DURATION);
#if FM_BCM2048_LOG
        MSG_ERROR("fm_bcm2048_mute timeout!add_time = %d", fm_loop, 0, 0);
#endif  
        if(BTA_FM_MUTE_AUD_EVT_FLAG == bcm2048_fm_evt_flag)   //loop if don't get the event
        {
            bcm2048_fm_evt_flag = BTA_FM_NONE_EVT_FLAG;     //CLEAR EVENT FLAG
            if(BTA_FM_OK == bcm2048_fm_info.mute_stat.status)
            {
#if FM_BCM2048_LOG
                MSG_ERROR("fm_bcm2048_mute seting success!", 0, 0, 0);
#endif              
            }
            else
            {
#if FM_BCM2048_LOG
                MSG_ERROR("fm_bcm2048_mute seting fail!", 0, 0, 0);
#endif              
            }
            return;            
        }
    }    
}

int fm_radio_init()   
{           

    fm_work_status = FM_POWER_DOWN; 
    if(FALSE == BTA_DmIsDeviceUp())
    {
#if FM_BCM2048_LOG
        MSG_ERROR("fail:bcm2048 is not ready!", 0, 0, 0);
#endif          
    }
#if FM_BCM2048_LOG
    MSG_ERROR("fm_radio_init!", 0, 0, 0);
#endif       
    return 0;
}

#define FM_VOL_DBG  0 //just for debug.

int fm_radio_power_up()
{   
    int  fm_loop = 0;
   
    if(fm_work_status == FM_NOT_INIT)
    {
        return FM_RADIO_FAILED;
    }        
    else if(fm_work_status == FM_IN_PROGRESS)
    {
        return FM_RADIO_SUCCESSFUL;
    }  
      
    BTA_FmEnable(BTA_FM_REGION_EUR,bcm2048_fm_cback,0);
#if FM_BCM2048_LOG
    MSG_ERROR("BTA_FmEnable !", 0, 0, 0);
#endif 
    for(fm_loop=0; fm_loop<FM_MAX_LOOP_TIME; fm_loop++)
    {
        //SET TIMER 
        rex_sleep(BCM2048_CBACK_SLEEP_DURATION);
#if FM_BCM2048_LOG
        MSG_ERROR("FmEnable timeout!add_time = %d", fm_loop, 0, 0);
#endif  
        if(BTA_FM_ENABLE_EVT_FLAG == bcm2048_fm_evt_flag)   //loop if don't get the event
        {
            bcm2048_fm_evt_flag = BTA_FM_NONE_EVT_FLAG;     //CLEAR EVENT FLAG
            if(BTA_FM_OK == bcm2048_fm_info.status)
            {
                fm_work_status = FM_POWER_UP;    
                
#ifdef EXT_HEADSET_PA                
                FM_AUDIO_ON();
#endif

#ifdef VC0848_HEADSET_PA    
/*  vc_task is running, just sending OPEN command to vc_task is ok */
#if FM_VOL_DBG
                MMD_SYS_PMU_SWITCH(0);
                rex_sleep(200);
                MMD_SYS_Mode_Switch(2);
                MMD_SYS_Codec_Switch(6);
                MMD_LCD_BLY_Release();
                MMD_Media_Codec_Set_MuxMode(0x01010101);
                //MMD_SYS_SetClk(19); //CLK_CPU48M_BUS48M
               // MMD_SYS_SET_CLK();
#else         
               {
                  vc_union_type vc_data;
                  vc_data.dev_run.curr_dev = VC_DEV_FM_PA;
                  VC_DeviceControl(VC_ITM_DEV_OPEN_I, VC_FUNC_CONTROL, &vc_data);
                  if(VC_GetCurrentDevice() == VC_STAT_INUSE)
                  {
                     // if current device in mp3 playing mode, indicates user that device is busy
                     // ...
                  }
               }
#endif
#endif    

#if FM_BCM2048_LOG           
                MSG_ERROR("FmEnable successful!", 0, 0, 0);        
#endif   
                return 0;   //it's the only one normal return!
            }
            else
            {
                return 1; //error
            }
        }
    }
#if FM_BCM2048_LOG
    MSG_ERROR("FmEnable callback Error!", 0, 0, 0); 
#endif
    return 1; //error   
}

int fm_radio_power_down()
{
    int  fm_loop = 0;
    
    if(fm_work_status == FM_NOT_INIT)
    {
        return FM_RADIO_FAILED;
    }           
    BTA_FmDisable();
#if FM_BCM2048_LOG
    MSG_ERROR("BTA_FmDisable !", 0, 0, 0);
#endif  
    for(fm_loop=0; fm_loop<FM_MAX_LOOP_TIME; fm_loop++)
    {
        //SET TIMER 
         rex_sleep(BCM2048_CBACK_SLEEP_DURATION);
        
#if FM_BCM2048_LOG
         MSG_ERROR("FmDisable timerout!", 0, 0, 0); 
#endif
        if(BTA_FM_DISABLE_EVT_FLAG == bcm2048_fm_evt_flag)
        {
            bcm2048_fm_evt_flag = BTA_FM_NONE_EVT_FLAG;     //CLEAR EVENT FLAG
            if(BTA_FM_OK == bcm2048_fm_info.status)
            {
                fm_work_status = FM_POWER_DOWN;   
                
#ifdef EXT_HEADSET_PA                
                FM_AUDIO_OFF();
#endif

#ifdef VC0848_HEADSET_PA    
/* vc_task is running, just sending STOP command to vc_task is ok */
#if FM_VOL_DBG
                //MMD_SYS_SetClk(2); //CLK_CPU216M_BUS72M
                MMD_LCD_BLY_Release();
                MMD_SYS_PMU_SWITCH(1); // set vc0848 in deep sleep while in bypass mode
                //changing 848's mode is must!
                //TBD
#else
                {
                   vc_union_type vc_data;
                   vc_data.dev_run.curr_dev = VC_DEV_FM_PA;
                   VC_DeviceControl(VC_ITM_DEV_STOP_I, VC_FUNC_CONTROL, &vc_data);
                }
#endif // if 0
#endif    

#if FM_BCM2048_LOG
                MSG_ERROR("FmDisable callback successful!", 0, 0, 0); 
#endif
                return 0;   //sucess
            }
            else
            {
                return 1; //error
            }
        }
    }
#if FM_BCM2048_LOG
    MSG_ERROR("FmDisable callback Error!", 0, 0, 0); 
#endif 
    return 1; //error   
}

int fm_tune_channel(word wChannel)
{
    int  fm_loop = 0;     
    
    if(fm_work_status != FM_IDLE_STATUS)
    {
#if FM_BCM2048_LOG
        MSG_ERROR("fm_tune_channel status error!", 0, 0, 0);
#endif
        return FM_RADIO_FAILED;
    }      
    fm_work_status = FM_IN_PROGRESS;
            
    //Begin tune and check tune status
    fm_playing_channel = 0;
    //set freq
      BTA_FmTuneFreq(CHANNEL2FREQ(wChannel));
#if FM_BCM2048_LOG
      MSG_ERROR(" BTA_FmTuneFreq!", 0, 0, 0);
#endif 
    for(fm_loop=0; fm_loop<FM_MAX_LOOP_TIME; fm_loop++)
    {
        //SET TIMER 
        rex_sleep(BCM2048_CBACK_SLEEP_DURATION);
#if FM_BCM2048_LOG       
        MSG_ERROR("BTA_FmTuneFreq timeout!add_time = %d", fm_loop, 0, 0);
#endif 
        if(BTA_FM_TUNE_EVT_FLAG == bcm2048_fm_evt_flag)
        {
            bcm2048_fm_evt_flag = BTA_FM_NONE_EVT_FLAG;     //CLEAR EVENT FLAG
            if(BTA_FM_OK ==  bcm2048_fm_info.chnl_info.status)
            {
                fm_work_status = FM_IDLE_STATUS;   
                fm_playing_channel = FREQ2CHANNEL(bcm2048_fm_info.chnl_info.freq);
#if FM_BCM2048_LOG
                MSG_ERROR("fm_playing_channel=%d,wChannel=%d,they must be equal!", fm_playing_channel, wChannel, 0);  
#endif 
                return 0;   
            }
            else
            {
                fm_work_status = FM_IDLE_STATUS; 
#if FM_BCM2048_LOG
               MSG_ERROR("FM seek stop failed_1", 0, 0, 0);   
#endif 
                return 1; //error
            }
        }
    }
    fm_work_status = FM_IDLE_STATUS; 
#if FM_BCM2048_LOG
    MSG_ERROR("FM seek stop failed_2", 0, 0, 0);     
#endif
    return 1;
}

int fm_seek_start(boolean bIsUp, boolean bIsWrap)
{   
    if(fm_work_status != FM_IDLE_STATUS)
    {
#if FM_BCM2048_LOG
        MSG_ERROR("fm_seek_start status error!", 0, 0, 0);
#endif
        return FM_RADIO_FAILED;
    }          
    fm_work_status = FM_IN_PROGRESS;
    
    if(bIsUp)
    {        
        fm_bcm2048_scan_direct = FM_BCM2048_SCAN_UP;
        BTA_FmSearchFreq(BTA_FM_SCAN_UP, FM_BCM2048_THRESHOLD, NULL);  
#if FM_BCM2048_LOG
       MSG_ERROR("BTA_FmSearchFreq(BTA_FM_SCAN_UP, %d, NULL) !", FM_BCM2048_THRESHOLD, 0, 0);
#endif 

    }
    else
    {
        fm_bcm2048_scan_direct = FM_BCM2048_SCAN_DOWN;
        BTA_FmSearchFreq(BTA_FM_SCAN_DOWN, FM_BCM2048_THRESHOLD, NULL);  //Set RSSI and direction
#if FM_BCM2048_LOG
        MSG_ERROR("BTA_FmSearchFreq(BTA_FM_SCAN_DOWN, %d, NULL) !", FM_BCM2048_THRESHOLD, 0, 0);
#endif

    }
    
//set if seek stop when rich band limit. But this option isn't used in apps!
    if(bIsWrap)
    {     
    
#if FM_BCM2048_LOG
       MSG_ERROR("BTA_FmSearchFreq bIsWrap = %d!", bIsWrap, 0, 0);
#endif 
    }
    else
    {    
#if FM_BCM2048_LOG
       MSG_ERROR("BTA_FmSearchFreq bIsWrap = %d!", bIsWrap, 0, 0);   
#endif
    }
    
    fm_work_status = FM_IDLE_STATUS;
    return 0;
    
}

int fm_get_seek_status(boolean* pIsFinish, boolean* pIsBandLimit, word* pChannel)
{    
    int fm_loop = 0;  

    if(fm_work_status != FM_IDLE_STATUS)
    {
#if FM_BCM2048_LOG
        MSG_ERROR("fm_get_seek_status status error!", 0, 0, 0);
#endif
        return FM_RADIO_FAILED;
    }      
    fm_work_status = FM_IN_PROGRESS;
    
     for(fm_loop=0; fm_loop<FM_MAX_LOOP_TIME; fm_loop++)
    {
        //SET TIMER 
        rex_sleep(BCM2048_CBACK_SLEEP_DURATION);
#if FM_BCM2048_LOG
        MSG_ERROR(" BTA_FmSeek timerout add_timer = %d!", fm_loop, 0, 0);
#endif        
        if(BTA_FM_SEARCH_CMPL_EVT_FLAG == bcm2048_fm_evt_flag)
        {
            bcm2048_fm_evt_flag = BTA_FM_NONE_EVT_FLAG;     //CLEAR EVENT FLAG    
            fm_work_status = FM_IDLE_STATUS;   
            if(BTA_FM_OK ==  bcm2048_fm_info.chnl_info.status)
            {
                *pChannel = FREQ2CHANNEL(bcm2048_fm_info.chnl_info.freq);
#if FM_BCM2048_LOG
                MSG_ERROR(" fm_get_seek_status.freq = %d!", bcm2048_fm_info.chnl_info.freq, 0, 0);
#endif           
                if(FM_BCM2048_SCAN_UP == fm_bcm2048_scan_direct)
                {
                    if((HIGHEST_BAND_FREQ/10) <= bcm2048_fm_info.chnl_info.freq)  
                    {
                        *pIsBandLimit = 1;     
                    }
                    else
                    {
                        *pIsBandLimit = 0;          
                    }
                }
                else if(FM_BCM2048_SCAN_DOWN == fm_bcm2048_scan_direct)
                {
                    if((LOWEST_BAND_FREQ/10) >= bcm2048_fm_info.chnl_info.freq)  
                    {
                        *pIsBandLimit = 1;     
                    }
                    else
                    {
                        *pIsBandLimit = 0;          
                    }
                }
                *pIsFinish = 1;
                return 0;                 
            }
            *pIsFinish = 0;             
#if FM_BCM2048_LOG
            MSG_ERROR(" fm_get_seek_status is error!", 0, 0, 0);
#endif           
            return 1;    // don't modify this value of return!
        }
    }
    fm_work_status = FM_IDLE_STATUS; 
#if FM_BCM2048_LOG
    MSG_ERROR("FM seek stop failed_4", 0, 0, 0);  
#endif
    return 1; //error      
    
}

int fm_set_volume(word wVolume)
{
    word mmd_volume;  
    static int s_fm_bcm2048_mute = 1;
    
    ASSERT(wVolume <= MAX_FMRADIO_VOLUME);
    fm_wVolume = wVolume;
    if(TRUE == s_fm_mute)
    {
        return 0;
    }
#ifdef VC0848_HEADSET_PA  
    //mute if wVolume=0
    if (0 == wVolume)
    {
#if FM_VOL_DBG    
         MMD_Media_Codec_Set_Play_Volume(0,0);
        //MMD_Media_Codec_Set_Mute(0,1);  //VC0848 MUTE
#else
        /* use vc_task to control VC0848 with SET_FMPA_VOL command */
        vc_union_type vc_data;
        vc_data.parm.p1 = 0;
        vc_data.parm.p2 = 0;
        VC_DeviceControl(VC_ITM_SET_FMPA_VOL_I, VC_FUNC_SET_PARM, &vc_data);       
#endif        
        fm_bcm2048_mute(TRUE);               //BCM2048 MUTE
        s_fm_bcm2048_mute = 1;
#if FM_BCM2048_LOG
        MSG_ERROR("BCM2048 fm_set_volume mute", 0, 0, 0);
#endif        
    }
    else
    {
        /*  use vc_task to control VC0848 with SET_FMPA_VOL command */
        vc_union_type vc_data;

        mmd_volume = (wVolume * 50) / MAX_FMRADIO_VOLUME;  //Max volume level of VC0848 is 100,but 70 is enough.

#if FM_VOL_DBG 
        MMD_Media_Codec_Set_Play_Volume(mmd_volume,mmd_volume);
#else   
        vc_data.parm.p1 = mmd_volume;
        vc_data.parm.p2 = mmd_volume;        
        VC_DeviceControl(VC_ITM_SET_FMPA_VOL_I, VC_FUNC_SET_PARM, &vc_data);
#endif
        if(1 == s_fm_bcm2048_mute)
        {
           fm_bcm2048_mute(FALSE);               //BCM2048 UNMUTE          
           s_fm_bcm2048_mute = 0;
        }
#if FM_BCM2048_LOG
        MSG_ERROR("BCM2048 fm_set_volume,wVolume=%d,mmd_volume=%d", wVolume, mmd_volume, 0);
#endif        
    }
#endif //VC0848_HEADSET_PA    
    return 0;
}
void fm_mute(boolean on)
{
    if(on)
    {
#ifdef VC0848_HEADSET_PA    
        vc_union_type vc_data;
        vc_data.parm.p1 = 0;
        vc_data.parm.p2 = 0;
        VC_DeviceControl(VC_ITM_SET_FMPA_VOL_I, VC_FUNC_SET_PARM, &vc_data);       
#endif
        fm_bcm2048_mute(TRUE);
        s_fm_mute = TRUE;
    }
    else
    {
        s_fm_mute = FALSE;   
        fm_set_volume(fm_wVolume);         
        if(fm_wVolume)
        {
            fm_bcm2048_mute(FALSE);          
        }      
    }
}
#endif  // BCM2048_I2C_MODE

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
#if 0
tBTA_FM_STATUS bta_fm_co_init(tBTA_FM_RDS_B rds_mode)
{
}

tBTA_FM_STATUS bta_fm_co_rds_data(UINT8 * p_data, UINT16 len)
{
}

void bta_fm_co_close(void)
{
}
#endif

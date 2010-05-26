/*===========================================================================

        FM   Radio  Driver for Si470X Chipset

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

extern uint8 GPIO_STRENGTH[GPIO_NUM_GPIOS];
extern uint8 GPIO_PU_PD[GPIO_NUM_GPIOS];

#define FM_AUDIO_SWITCH  33                 //Using GPIO33 as audio switch
#define FM_SEN_PAD_NAME  GPIO_OUT_47
#define FM_RST_PAD_NAME  GPIO_OUT_48

/*
#define FM_AUDIO_ON()     \
       HWIO_OUT(GPIO2_PAGE, FM_AUDIO_SWITCH); \
       HWIO_OUT(GPIO2_CFG, ((uint32)GPIO_STRENGTH[FM_AUDIO_SWITCH] << 6) | \
                           ((uint32)GPIO_PU_PD[FM_AUDIO_SWITCH] ^ 0x0002));                           

#define FM_AUDIO_OFF()    \
       HWIO_OUT(GPIO2_PAGE, FM_AUDIO_SWITCH); \
       HWIO_OUT(GPIO2_CFG, ((uint32)GPIO_STRENGTH[FM_AUDIO_SWITCH] << 6) | \
                           ((uint32)GPIO_PU_PD[FM_AUDIO_SWITCH]));  
*/
#define FM_AUDIO_ON()       gpio_invert_pupd_state(FM_AUDIO_SWITCH); 
#define FM_AUDIO_OFF()      gpio_recover_pupd_state(FM_AUDIO_SWITCH); 

#define FM_SEN_SET_LOW()   \
    BIO_OUT(BIO_GPIO_47_REG, BIO_GPIO_47_M, BIO_GPIO_47_LO_V)    
#define FM_SEN_SET_HIGH()   \
    BIO_OUT(BIO_GPIO_47_REG, BIO_GPIO_47_M, BIO_GPIO_47_HI_V)
    
#define FM_RST_SET_LOW()   \
    BIO_OUT(BIO_GPIO_48_REG, BIO_GPIO_48_M, BIO_GPIO_48_LO_V)    
#define FM_RST_SET_HIGH()   \
    BIO_OUT(BIO_GPIO_48_REG, BIO_GPIO_48_M, BIO_GPIO_48_HI_V)

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
#define DEVICE_ID_ADDR          0x00
#define CHIPSET_ID_ADDR         0x01
#define POWER_CFG_ADDR          0x02

#define REG_SIZE                16
#define REG_READ_START_ADDR     0x0A
#define REG_WRITE_START_ADDR    0x02

#define REG_BUF_SIZE            REG_SIZE * 2

#ifdef FM_READ_ALL_REG
#define REG_READ_SIZE           REG_BUF_SIZE
#define REG_READ_BUF_OFFSET     (REG_SIZE - REG_READ_START_ADDR)*2
#else
//Read Operation from 0x0A to 0x0F then read 0x01, 0x02
#define REG_READ_SIZE           (REG_SIZE - REG_READ_START_ADDR + REG_WRITE_START_ADDR)*2
#endif

//Write Operation from 0x02 to 0x09
#define REG_WRITE_SIZE          REG_BUF_SIZE - REG_READ_SIZE//(REG_SIZE - REG_WRITE_START_ADDR)*2

#define FM_I2C_SLAVE_ADDR       0x20

#define FM_MAX_LOOP_TIME        0x0FFF

static byte i2c_data_buf[REG_BUF_SIZE]  = {0};

static fm_status_type fm_work_status = FM_NOT_INIT;

static i2c_rw_cmd_type i2c_rw_cmd;

static word fm_playing_channel = 0;

//Reset write buffer value to chipset reset value;
LOCAL void fm_init_i2c_cmd_data()
{
    int index = 0;
    
    while(index < REG_BUF_SIZE)
    {
        i2c_data_buf[index] = 0;   
        index++;
    }
    //Reg 0x07 upper byte is 0x01
    i2c_data_buf[0x07 * 2] = 0x01;
    
    i2c_rw_cmd.bus_id = I2C_BUS_HW_CTRL;
    i2c_rw_cmd.slave_addr = FM_I2C_SLAVE_ADDR;
    i2c_rw_cmd.options = 0;
    i2c_rw_cmd.buf_ptr = i2c_data_buf;
    //Needn't set i2c_rw_cmd.addr.reg = 0;
    i2c_rw_cmd.len = 0;
    i2c_rw_cmd.last_index = 0;
}    

LOCAL i2c_status_type fm_read_i2c_cmd(byte reg, word* pData)
{   
    i2c_status_type     result;
    byte                tmp_buf[REG_READ_SIZE]  = {0};
    byte                index, dest, source;
    
    //i2c_rw_cmd.buf_ptr = &i2c_data_buf[REG_READ_START_ADDR * 2];
    i2c_rw_cmd.buf_ptr = tmp_buf;
    
    i2c_rw_cmd.len = REG_READ_SIZE;
    i2c_rw_cmd.last_index = 0;    
    
    result = i2c_read(&i2c_rw_cmd);    
    
#ifdef FM_READ_ALL_REG
    //copy read data from temp buf to reg shadow memory
    index = 0;
    dest = 0;
    source = REG_READ_BUF_OFFSET;
    while(index < REG_READ_SIZE)
    {
        i2c_data_buf[dest] = tmp_buf[source];
        i2c_data_buf[dest + 1] = tmp_buf[source + 1];
        
        index += 2;
        dest += 2;
        source = (source + 2) & 0x1F;//(source + REG_READ_OFFSET) % REG_SIZE;
    }
#else    
    //copy read data from temp buf to reg shadow memory
    //Only copy the status reg to buf now
    index = 0;
    dest = REG_READ_START_ADDR * 2;
    source = 0;
    while(index < REG_READ_SIZE)
    {
        i2c_data_buf[dest] = tmp_buf[source];
        i2c_data_buf[dest + 1] = tmp_buf[source + 1];
        
        index += 2;
        dest = (dest + 2) & 0x1F;
        source += 2;
    }
#endif
    
    *pData = ((word)i2c_data_buf[reg *2]) << 8;   
    *pData += i2c_data_buf[reg * 2 + 1]; 
   
    return result;    
}

//This functions over write the reg by data
LOCAL i2c_status_type fm_write_i2c_cmd(byte reg, word data)
{
    i2c_data_buf[reg *2] = (byte)(data >> 8);   
    i2c_data_buf[reg * 2 + 1] = (byte) data; 
    
    i2c_rw_cmd.buf_ptr = &i2c_data_buf[REG_WRITE_START_ADDR * 2];
    
    i2c_rw_cmd.len = REG_WRITE_SIZE;
    i2c_rw_cmd.last_index = 0;    
    
    return i2c_write(&i2c_rw_cmd);
}

//This is a shadow write functions
LOCAL i2c_status_type fm_shadow_write_i2c_cmd(byte reg, word mask, word data)
{   
    data &= mask; 
    
    i2c_data_buf[reg *2] = (i2c_data_buf[reg *2] & (byte)(~(mask >> 8))) | (byte)(data >> 8);   
    i2c_data_buf[reg * 2 + 1] = (i2c_data_buf[reg * 2 + 1] & (byte)(~mask)) | (byte)(data); 
    
    i2c_rw_cmd.buf_ptr = &i2c_data_buf[REG_WRITE_START_ADDR * 2];
    
    i2c_rw_cmd.len = REG_WRITE_SIZE;
    i2c_rw_cmd.last_index = 0;    
    
    return i2c_write(&i2c_rw_cmd);
}

//This functions set the reg buffer by data
LOCAL void fm_set_reg_buf(byte reg, word data)
{
    i2c_data_buf[reg *2] = (byte)(data >> 8);   
    i2c_data_buf[reg * 2 + 1] = (byte) data; 
}

//This is a shadow set functions
LOCAL void fm_shadow_set_reg_buf(byte reg, word mask, word data)
{   
    data &= mask; 
    
    i2c_data_buf[reg *2] = (i2c_data_buf[reg *2] & (byte)(~(mask >> 8))) | (byte)(data >> 8);   
    i2c_data_buf[reg * 2 + 1] = (i2c_data_buf[reg * 2 + 1] & (byte)(~mask)) | (byte)(data); 
}

LOCAL i2c_status_type fm_write_reg_buf_to_chip()
{    
    i2c_rw_cmd.buf_ptr = &i2c_data_buf[REG_WRITE_START_ADDR * 2];
    
    i2c_rw_cmd.len = REG_WRITE_SIZE;
    i2c_rw_cmd.last_index = 0;    
    
    return i2c_write(&i2c_rw_cmd);
}

void fm_radio_pre_init()
{
    //make GPIO18 to normal GPIO pad, so we can use it to config Si470x's bus type.
    gpio_i2c_un_config();
    
    //make RST single low to config Si470x
    FM_RST_SET_LOW();
    
    //make SEN high and SDIO single low to config Si470x as 2 lin bus mode
    FM_SDIO_SET_LOW();
    FM_SEN_SET_HIGH();
    
    //Fine, make Si470x enable now
    FM_RST_SET_HIGH();
    
    //config GPIO_18 back to SDA
    gpio_i2c_config();
}
    
int fm_radio_init()
{   
    int iResult;
            
    //Make the GPIO_47 and GPIO_48 as normal output GPIO 
    gpio_tlmm_un_config(FM_SEN_PAD_NAME, GPIO_OUTPUT);
    gpio_tlmm_un_config(FM_RST_PAD_NAME, GPIO_OUTPUT);
    
    //make GPIO18 to normal GPIO pad, so we can use it to config Si470x's bus type.
    gpio_i2c_un_config();
    
    //NOTICE:
    //We must wait a short time here when using a certain hardware.
    //I think there may be some time to steady GPIO.
    
    //So we use fm_radio_pre_init() to do pre-init in hs_init to 
    //avoid this problem
    
    //make RST single low to config Si470x
    FM_RST_SET_LOW();
    
    //make SEN high and SDIO single low to config Si470x as 2 lin bus mode
    FM_SDIO_SET_LOW();
    FM_SEN_SET_HIGH();
    
    //Fine, make Si470x enable now
    FM_RST_SET_HIGH();
    
    //config GPIO_18 back to SDA
    gpio_i2c_config();
    
    //init i2c cmd data struct
    fm_init_i2c_cmd_data();   
    
    //enable the internal 
    iResult = fm_write_i2c_cmd(0x07, 0x8100);    
    
    //Maybe need to wait at least 500ms here.
    
    if(FM_RADIO_SUCCESSFUL == iResult)
    {
        fm_work_status = FM_POWER_DOWN;
    
    }
    else
    {
        fm_work_status = FM_NOT_INIT;      
    }
    
    return iResult;
    
}

int fm_radio_power_up()
{   
    int  iResult = 0;
 
/*        
    iResult |= fm_write_i2c_cmd(0x02, 0x4001);    
    
    iResult |= fm_write_i2c_cmd(0x07, 0xBC04);      //Maybe can remove.  
    
    //Do some config here;
    iResult |= fm_write_i2c_cmd(0x05, 0x000F);
    
    return iResult;     
*/
    if(fm_work_status == FM_NOT_INIT)
    {
        return FM_RADIO_FAILED;
    }        
    else if(fm_work_status == FM_IN_PROGRESS)
    {
        return FM_RADIO_SUCCESSFUL;
    }        
    
    fm_set_reg_buf(0x02, 0x4001);        
    fm_set_reg_buf(0x07, 0xBC04);      //Maybe can remove.  
    
    //Do some config here;
    //Force to use mono mode
    fm_shadow_set_reg_buf(0x02, 0x2000, 0x2000);
    //Set volum to Middle
    fm_shadow_set_reg_buf(0x05, 0x000F, 0x0008);
    
#if defined FEATURE_FM_SPACE_50KHZ
    //bw: Set Channel Space to 50 KHz
    fm_shadow_set_reg_buf(0x05, 0x0030, 0x0020);  
#else
    //Set Channel Space to 100 KHz
    fm_shadow_set_reg_buf(0x05, 0x0030, 0x0010);  
#endif //#if defined FEATURE_FM_SPACE_50KHZ

    //Set RSSI seek threshold
    fm_shadow_set_reg_buf(0x05, 0xFF00, 0x0A00);    
    //Set seek snr threshold
    fm_shadow_set_reg_buf(0x06, 0x00F0, 0x0005); 
    //Set seek cnt threshold
    fm_shadow_set_reg_buf(0x06, 0x000F, 0x0003);     
    //Set Volume Extention
    //fm_shadow_set_reg_buf(0x06, 0x0100, 0x0100);  
    
    iResult = fm_write_reg_buf_to_chip();
    
    if(FM_RADIO_SUCCESSFUL == iResult)
    {
        fm_work_status = FM_POWER_UP;    
        FM_AUDIO_ON();
    }
    
    return iResult;
}

int fm_radio_power_down()
{
    int  iResult = 0;
        
/*
    iResult |=  fm_write_i2c_cmd(0x02, 0x0041);       
    iResult |=  fm_write_i2c_cmd(0x07, 0xFC04);//0xC100);//0xFC04);   
    
    return iResult;.
*/        
    if(fm_work_status == FM_NOT_INIT)
    {
        return FM_RADIO_FAILED;
    }      
    
    fm_set_reg_buf(0x02, 0x0041);       
    fm_set_reg_buf(0x07, 0xFC04);//0xC100);//0xFC04);  
    
    iResult = fm_write_reg_buf_to_chip();
    
    if(FM_RADIO_SUCCESSFUL == iResult)
    {
        fm_work_status = FM_POWER_DOWN;    
        FM_AUDIO_OFF();
    }
    
    return iResult;
}
/*--------------------------------------------------------------

            Seek (Tune)状态寄存器操作顺序
            
    一般的想法是先清零寄存器，然后开始操作，并根据寄存器
    判断操作结果。
    
    实际上，因为Seek和Tune使用同一个寄存器的同一个Bit表示
    操作状态。当Tune使该Bit变为“完成”时，希望通过向Seek控
    制寄存器写0是该Bit变为“未完成”可能会失败!
    
    原因是：Reset后Seek控制寄存器本来就是“0”，所以向其写0
    不会引起任何操作!
    
    反之，先Seek，然后Tune也会出现类似问题。
    
    所以，我们在先操作（Seek或Tune），完成操作后马上将STC
    变为“完成”以待下一次操作。

----------------------------------------------------------------*/

int fm_tune_channel(word wChannel)
{
    int  iResult = 0;   
    int  iLoopCounter = 0;     
    word status;    
    
    if(fm_work_status != FM_IDLE_STATUS)
    {
        return FM_RADIO_FAILED;
    }      
    fm_work_status = FM_IN_PROGRESS;
            
    //Begin tune and check tune status
    iLoopCounter = 0;
    fm_playing_channel = 0;
    fm_shadow_set_reg_buf(0x03, 0x01FF, wChannel);
    iResult = fm_shadow_write_i2c_cmd(0x03, 0x8000, 0x8000);
    do
    {
        iResult |= fm_read_i2c_cmd(0x0A, &status);        
    
        KICK_WATCHDOG();
    
        if(FM_RADIO_SUCCESSFUL != iResult)
        {
            fm_work_status = FM_IDLE_STATUS;
            MSG_ERROR("FM seek stop failed", 0, 0, 0);
            
            return iResult;
        }
        iLoopCounter++;
    }
    while(iLoopCounter < FM_MAX_LOOP_TIME && ((status & 0x4000) == 0));        
    
    //Set tune bit to 0 to make STC low
    iResult = fm_shadow_write_i2c_cmd(0x03, 0x8000, 0x0000);    
    do
    {
        iResult |= fm_read_i2c_cmd(0x0A, &status);        
    
        KICK_WATCHDOG();
    
        if(FM_RADIO_SUCCESSFUL != iResult)
        {
            fm_work_status = FM_IDLE_STATUS;
            MSG_ERROR("FM tune stop failed", 0, 0, 0);
            
            return iResult;
        }
        iLoopCounter++;
    }
    while(iLoopCounter < FM_MAX_LOOP_TIME && ((status & 0x4000) != 0));    
            
    fm_work_status = FM_IDLE_STATUS;
    fm_playing_channel = wChannel;
    return iResult;
}

LOCAL int fm_do_seek(word* pChannel)
{
    int  iResult = 0;   
    int  iLoopCounter = 0;     
    word status;
    
    //Begin seek and check seek status
    iLoopCounter = 0;
    iResult = fm_shadow_write_i2c_cmd(0x02, 0x0100, 0x0100);
    do
    {
        iResult |= fm_read_i2c_cmd(0x0A, &status);        
    
        KICK_WATCHDOG();
    
        if(FM_RADIO_SUCCESSFUL != iResult)
        {
            MSG_ERROR("FM seek stop failed", 0, 0, 0);
            
            return iResult;
        }
        iLoopCounter++;
    }
    while(iLoopCounter < FM_MAX_LOOP_TIME && ((status & 0x4000) == 0));    
    
    //OK, seek completed. Let's check if seek successful.
    iResult = fm_read_i2c_cmd(0x0B, &status);
    *pChannel = status & 0x01FF;
    
    //Set seek bit to 0 to make STC and SF/BL low
    iResult = fm_shadow_write_i2c_cmd(0x02, 0x0100, 0x0000);    
    do
    {
        iResult |= fm_read_i2c_cmd(0x0A, &status);        
    
        KICK_WATCHDOG();
    
        if(FM_RADIO_SUCCESSFUL != iResult)
        {
            MSG_ERROR("FM seek stop failed", 0, 0, 0);
            
            return iResult;
        }
        iLoopCounter++;
    }
    while(iLoopCounter < FM_MAX_LOOP_TIME && ((status & 0x4000) != 0));    
    
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
    
    fm_shadow_set_reg_buf(0x02, 0x0200, 0x0200);    
    iResult = fm_do_seek(pChannel);
    
    fm_work_status = FM_IDLE_STATUS;
    
    return iResult;
}

int fm_seek_down(word* pChannel)
{
    int  iResult = 0;   
    
/*      
    //Set seek bit to 0 to make STC and SF/BL low
    iResult |= fm_shadow_write_i2c_cmd(0x02, 0x0300, 0x0000);
    //Begin seek down
    iResult |= fm_shadow_write_i2c_cmd(0x02, 0x0300, 0x0100);
    
    return iResult;
*/  
    if(fm_work_status != FM_IDLE_STATUS)
    {
        return FM_RADIO_FAILED;
    }      
    fm_work_status = FM_IN_PROGRESS;
      
    fm_shadow_set_reg_buf(0x02, 0x0200, 0x0000);    
    iResult = fm_do_seek(pChannel);
    
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
        fm_shadow_set_reg_buf(0x02, 0x0200, 0x0200);  
    }      
    else
    {    
        fm_shadow_set_reg_buf(0x02, 0x0200, 0x0000);          
    }
    
    //set if seek stop when rich band limit
    if(bIsWrap)
    {        
        fm_shadow_set_reg_buf(0x02, 0x0400, 0x0000);
    }
    else
    {        
        fm_shadow_set_reg_buf(0x02, 0x0400, 0x0400);
    }
    
    //Begin seek
    iResult = fm_shadow_write_i2c_cmd(0x02, 0x0100, 0x0100);   
    
    fm_work_status = FM_IDLE_STATUS;
    
    return iResult;
}

int fm_get_seek_status(boolean* pIsFinish, boolean* pIsBandLimit, word* pChannel)
{    
    int iResult = 0;
    int  iLoopCounter = 0;     
    word status;
    
    if(fm_work_status != FM_IDLE_STATUS)
    {
        return FM_RADIO_FAILED;
    }      
    fm_work_status = FM_IN_PROGRESS;
    
    iResult |= fm_read_i2c_cmd(0x0A, &status);
    
    if((status & 0x4000) == 0)
    {
        *pIsFinish = FALSE;
    }
    else
    {   
        *pIsFinish = TRUE;        
             
        iResult |= fm_read_i2c_cmd(0x0A, &status);
        if((status & 0x2000) == 0)
        {
            *pIsBandLimit = FALSE;
        }
        else
        {
            *pIsBandLimit = TRUE;            
        }
        
        iResult |= fm_read_i2c_cmd(0x0B, &status);
        *pChannel = status & 0x01FF;
        
        //Set seek bit to 0 to make STC and SF/BL low
        iResult = fm_shadow_write_i2c_cmd(0x02, 0x0100, 0x0000);    
        do
        {
            iResult |= fm_read_i2c_cmd(0x0A, &status);        
        
            KICK_WATCHDOG();
        
            if(FM_RADIO_SUCCESSFUL != iResult)
            {
                MSG_ERROR("FM seek stop failed", 0, 0, 0);
                
                return iResult;
            }
            iLoopCounter++;
        }
        while(iLoopCounter < FM_MAX_LOOP_TIME && ((status & 0x4000) != 0));  
    }
    
    fm_work_status = FM_IDLE_STATUS;
    
    return iResult;
}

int fm_set_volume(word wVolume)
{
    int iResult = 0;
    
    if(fm_work_status != FM_IDLE_STATUS)
    {
        return FM_RADIO_FAILED;
    }       
    
    if(wVolume != 0)
    {
        wVolume = wVolume * 2 -1;
    }
        
    iResult |= fm_shadow_write_i2c_cmd(0x05, 0x000F, wVolume);
    
    return iResult;
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

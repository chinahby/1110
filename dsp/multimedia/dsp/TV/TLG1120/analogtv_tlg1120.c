#include "tv_include.h"

#ifdef FEATURE_PMIC
#include "pm.h"
#endif

//#include "time_secure.h"

#define ANALOGTV_TLG1120_C_DEBUG
#ifdef ANALOGTV_TLG1120_C_DEBUG
#define ATV_DEBUG(fmt,x,y,z) MSG_FATAL(fmt,x,y,z)
#else
#define ATV_DEBUG(fmt,x,y,z) 
#endif

#if defined(FEATURE_ANALOG_TV)
#define TLG1120_TV
#endif

#define TLG1120_FM

extern i2c_rw_cmd_type tv_i2c_command;

void TLG1120_init_host_gpio(void)
{
	return;
}

void TLG1120_poweron_chip(void)
{    
	gpio_tlmm_config(ATV_POWER_PIN);
	gpio_out(ATV_POWER_PIN, GPIO_HIGH_VALUE);
    TLG_Delay(25);
	return;
}

void TLG1120_reset_chip(void)   
{
	gpio_tlmm_config(ATV_RESET_PIN);
    gpio_out(ATV_RESET_PIN, GPIO_LOW_VALUE);
	TLG_Delay(25);
#ifdef TLG_1120
	TLG_Delay(200);
#endif   
    gpio_out(ATV_RESET_PIN, GPIO_HIGH_VALUE);
    TLG_Delay(25);
}

static void TLG1120_poweroff_chip(void)
{
	/* here we power off */
	gpio_out(ATV_RESET_PIN, GPIO_LOW_VALUE);	
}

#ifdef TEST_TLG1120_I2C    
static void TLG1120_VerifyI2C(void)
{
    uint16 reg, write, read, reg_start, reg_end;

    // Test I2C communication
    tlg_i2c_addr = 0x2C;

#ifdef TLG_1120
    reg_start = 0xF0;
    reg_end = 0xFC;
#endif
    for (reg = reg_start, write = 1; reg <= reg_end; reg++, write <<= 1)
        TLG_WriteReg(tlg_i2c_addr, reg, write);

    for (reg = reg_start, write = 1; reg <= reg_end; reg++, write <<= 1)
    {
        TLG_ReadRegNS(tlg_i2c_addr, reg, &read);
        if (read != write)
        {
            TLG_PRINT_3("ATV ERROR: I2C Reg=%x Wr=%x Rd=%x\n", reg, write, read);
        }
    }
}
#endif


#ifdef TLG1120_TV
#define TLGI2C_DEFAULT_BASE_ADDR            (0x002c)
/**/
void TLG1120_tv_power_up(void)
{
    /*no need do anything*/
   TLG_PRINT_0("TLG1120_tlg1120_power_up invoke");

}

/**/
void TLG1120_tv_power_down(void)
{
/*电视的电源是另外接在LDO上，断电停止工作*/
    TLGAPP_PowerDown();
    TLG1120_reset_chip();    
    TLG1120_poweroff_chip();  

    TLG_PRINT_0("TLG1120_tlg1120_power_down invoke");
}



static int TLG1120_tv_save_program(int32 chn)
{
     if (gTvStorage.mIdxListCnt > TLG_MAX_PROGRAM_NUM)
     {        
         return EFAILED;
     }
 
     gTvStorage.mIdxListBuf[gTvStorage.mIdxListCnt++].ChnIdx = chn;
     
     return SUCCESS;
}

 

int TLG1120_tv_scan_channel(TLG_TV_STORAGE *tv_storage)    
{
    int ChnInx = 0, ret = 0;

#if 0    
    if (tv_storage == NULL)
    {
        return EFAILED;
    }
#endif    
    TLGAPP_StartChannelScan(p_tlg_cur_map, p_tlg_cur_map[1]HZ_S - p_tlg_cur_map[0]HZ_S);
    for(ChnInx=0;p_tlg_cur_map[ChnInx]CHN_S!=0;ChnInx++)
    {               
        ret = TLGAPP_SetChannel(p_tlg_cur_map[ChnInx]CHN_S);        
        if (ret == 0)
        {
            TLG_PRINT_1("freq:%d   no  signal\n", p_tlg_cur_map[ChnInx]CHN_S);
        }
        else
        {        
            if (TLG1120_tv_save_program(p_tlg_cur_map[ChnInx]CHN_S)==EFAILED)
            {
                break;
            }
            TLG_PRINT_1("freq:%d   have  signal\n", p_tlg_cur_map[ChnInx]CHN_S);           
        }              
    }
    TLGAPP_StopChannelScan();
    
    if (gTvStorage.mIdxListBuf[gTvStorage.mCurListIdx].ChnIdx != 0)
    {
         TLGAPP_SetChannel(gTvStorage.mIdxListBuf[gTvStorage.mCurListIdx].ChnIdx);  
         TLG_PRINT_1("set chn %d\n", gTvStorage.mIdxListBuf[gTvStorage.mCurListIdx].ChnIdx);
  //       MEMCPY(tv_storage,&gTvStorage,sizeof(TLG_TV_STORAGE));
         return SUCCESS;
    }
    else
    {
        TLG_PRINT_1("set chn %d\n", p_tlg_cur_map[24]CHN_S);   
        TLGAPP_SetChannel(p_tlg_cur_map[24]CHN_S);      
        return EFAILED;
    }   
}


/*俩种设置模式，ext为true时画面静止并且没有声音，但是时间久点。
只要使用在手动切换频点，ext为false主要应用于自动搜索*/
int TLG1120_tv_set_channel(uint16 chn)
{
    int ret = 0;
//    unsigned long time;
    
    /*应用程序设下来的chn是从1开始，来区分返回值0*/
//    time_secure_get_local_time_ms(&time);
//    TLG_PRINT_1("TLG1120_tv_set_channel Enter time=%u\n",time);
    if (chn<1)
    {
        return 0;
    }
    
    ret = TLGAPP_SetChannel(p_tlg_cur_map[chn-1]CHN_S);
//    time_secure_get_local_time_ms(&time);
//    TLG_PRINT_1("TLGAPP_SetChannel Leave time=%u\n",time);    
    return (ret!=0)?chn:0;   
}


int TLG1120_tv_set_fast_channel(uint16 chn)
{
    if (chn<1)
    {
        return 0;
    }

    return TLGAPP_FastSetChannel(p_tlg_cur_map[chn-1]CHN_S);
}



int TLG1120_tv_set_channel_ext(uint16 chn)
{
    if (chn<1)
    {
        return 0;
    }

    return TLGAPP_SetChannelExt(p_tlg_cur_map[chn-1]CHN_S);
}


int TLG1120_tv_set_display_size(uint16 w, uint16 h)
{
    if (TLGMMI_SetHSize(w)==TLG_ERR_SUCCESS&&TLGMMI_SetVSize(h)==TLG_ERR_SUCCESS)
    {
        return SUCCESS;
    }
    return EFAILED;
}


int TLG1120_tv_set_display_offset(uint16 xoff, uint16 yoff)
{
    
     if (TLGMMI_SetHOffset(xoff)==TLG_ERR_SUCCESS&&TLGMMI_SetVOffset(yoff)==TLG_ERR_SUCCESS)
    {
        return SUCCESS;
    }
    return EFAILED;
}


int TLG1120_tv_get_chn_count(void)
{
    int ChnInx = 0;
    
    while(p_tlg_cur_map[ChnInx++]CHN_S!=0) ;

    return ChnInx;
}
    
extern void camera_camif_error_clk_cb(int4 ms);
int TLG1120_tv_set_param(ATV_SET_PARAM_e type, int hparam, int lparam)
{
    int ret = 0;

    switch (type)
    {   
        case TLG_SET_REGION:
        {
            
            ret = 0;
            TLGMMI_SetRegion((TLG_REGION_CODE)hparam);
            TLGAPP_InitRegionChannelMap();
            ERR("gPreScanLines = %d, gTvScanLines=%d",gPreScanLines,gTvScanLines,0);
            if (gPreScanLines != gTvScanLines)
            {
                /*ret==1表示电视制式发生变化,应用需要reset sensor*/
                ret = 1;
            }
        }break;

        case TLG_SET_VOLUME:
        {
            if(hparam > TLG_MAX_AUD_GAIN_DAC||hparam < TLG_MIN_AUD_GAIN)
            {
                TLG_PRINT_0("TLG1120_tv_set_param TLG_SET_VOLUME param failed");
            }
             TLG_PRINT_1("TLG_SET_VOLUME %d",hparam);
            TLG_SetAudioGain(TLGI2C_DEFAULT_BASE_ADDR,TLG_DAC, hparam);
        }break;
        
        case TLG_SET_DEFINITION:
        {
            ret = TLGMMI_SetDefinition((uint8)hparam);
        }break;
        
        case TLG_SET_BRIGHT:
        {
            ret = TLGMMI_SetBrightness((uint8)hparam);
        }break;

        case TLG_SET_CONTRAST:
        {
            ret = TLGMMI_SetContrast((uint8)hparam);
        }break;

        case TLG_SET_SATURATION:
        {
            ret = TLGMMI_SetSaturation((uint8)hparam);
        }break;

        case TLG_SET_DISPLAY_SIZE:
        {
            ret = TLG1120_tv_set_display_size((uint16)hparam,(uint16)lparam);
        }break;

        case TLG_SET_DISPLAY_OFFSET:
        {
            ret = TLG1120_tv_set_display_offset((uint16)hparam,(uint16)lparam);
        }break;

        
        default:
            break;
        
    }
    
    return (ret == 0)?SUCCESS:EFAILED;
}

int TLG1120_tv_get_param(ATV_GET_PARAM_e type, void * hparam, void * lparam)
{
    switch(type)
    {
        case TLG_GET_CHN_COUNT:
        {
            
            *(int *)hparam = TLG1120_tv_get_chn_count();
            TLG_PRINT_1("TLG_GET_CHN_COUNT %d",*(int *)hparam);
        }break;
 
#ifndef TLG_TEST_SENSITIVE
        case TLG_GET_TFCODE:
        {
            uint16 value;
            
            TLG_GetTFCalCode(TLGI2C_DEFAULT_BASE_ADDR,&value);
            *(int *)hparam = value;
            TLG_PRINT_1("TLG_GET_TFCODE %d",*(int *)hparam);
        }break;

        case TLG_GET_TTLGAIN:
        {
            uint16 value;
            
            TLG_GetTotalGain(TLGI2C_DEFAULT_BASE_ADDR,&value);
            *(int *)hparam = value;
            TLG_PRINT_1("TLG_GET_TTLGAIN %d",*(int *)hparam);
        }break;

        case TLG_GET_CHROMLOCK:
        {
            *(int *)hparam = TLG_GetChromaLockStatus();
            TLG_PRINT_1("TLG_GET_CHROMLOCK %d",*(int *)hparam);
        }break;
        
#endif
        default:
            break;
    }

    return SUCCESS;
}



#if 0
boolean camsensor_tlg1120_init(camsensor_function_table_type *camsensor_function_table_ptr,
				                                camctrl_tbl_type              *camctrl_tbl_ptr)
#else
boolean camsensor_tlg1120_init(void)
#endif
{
    uint16 y, u, v;
	//unsigned long time;
    
 	//time_secure_get_local_time_ms(&time);
	//TLG_PRINT_1("TLG1120_tlg1120_init Enter time=%u\n",time);
    TLG_PRINT_0("TLG1120_tlg1120_init start");
    tv_i2c_command.bus_id     = I2C_BUS_HW_CTRL;
	tv_i2c_command.slave_addr = (TLGI2C_DEFAULT_BASE_ADDR<<1);
	tv_i2c_command.options    = (i2c_options_type) (I2C_DFLT_ADDR_DEV| I2C_START_BEFORE_READ); 
	
#if 1	//For debug, we set some variable, after mmi is finish we disable it
    TLGMMI_RestoreTvOption();
    TLGMMI_RestoreFmOption();
    TLGMMI_ClearTvStorage();
    TLGMMI_ClearFmStorage();
    gTvStorage.mCurChnIdx = 24;
#endif

    TLG1120_init_host_gpio();
    TLG1120_poweron_chip();
    TLG_Delay(25);
    TLG1120_reset_chip();
    
#ifdef TEST_TLG1120_I2C
    TLG1120_VerifyI2C();
#endif

    TLGAPP_Restart();
    if (TLGAPP_Init(tlg_i2c_addr)!=TLG_ERR_SUCCESS)
    {
        TLG1120_tv_power_down();
        return FALSE;
    }
        
    TLGAPP_InitRegionChannelMap();
    //gPreScanLines = gTvScanLines;
   //TLGAPP_SetChannel(p_tlg_cur_map[gTvStorage.mCurChnIdx] CHN_S);
   
#if 1    
    y = gTvOptions.mContrast;
    u = gTvOptions.mSaturation;
    v = ( (u * 172) >> 8 );
    TLG_SetYUVScale(tlg_i2c_addr, y, u, v);
#endif
        // Preset definition    
#ifdef TLG_1120
    TLG_SetMobileVidEnhancement(tlg_i2c_addr, TLG_MVE_MANUAL, gTvOptions.mDefinition);
    TLG_SquelchEvenFrames(tlg_i2c_addr, TLG_ON);
#endif

    //声音和图像的测试
	TLGAPP_TurnOnTestPattern();
	//TLGAPP_TurnOnTestToneMode();
	//TLG_SetAudioTestToneMode(tlg_i2c_addr, TLG_ON);
	//TLG_SetAudioTestToneL(tlg_i2c_addr, TLG_1KHZ);
	//TLG_SetAudioTestToneR(tlg_i2c_addr, TLG_1KHZ);


    //register function for sensor driver
    //TLG1120_tv_register(camsensor_function_table_ptr);
    //TLG1120_tv_setup_camctrl_tbl(camctrl_tbl_ptr);    
    //TLG1120_tv_scan_channel
    //TLG_SetAudioGain(TLGI2C_DEFAULT_BASE_ADDR,TLG_DAC, 1);
    
 	//time_secure_get_local_time_ms(&time);
 	//TLG_PRINT_1("TLG1120_tlg1120_init Leave time=%u\n",time);
	//TLG1120_tv_scan_channel(NULL);
    TLG_PRINT_0("TLG1120_tlg1120_init Leave");
    return TRUE;
}
#endif

/*************************************************************************/
/*                          下面是FM部分                                 */
/*************************************************************************/

#ifdef TLG1120_FM
//需要重定向的宏

#define TLG_FMDrv_Mute(a) 
#define FM_SEARCH_STEP	2 // 1		//2 for -200khz~+200khz compare window size; 1 for -100khz~+100khz compare window size  in 100khz step
//#define FM_SEARCH_STEP	4 // 2		//5 for -200khz~+200khz compare window size; 2 for -100khz~+100khz compare window size  in 50khz step
#define MIN_tlgFM_FREQUENCY 875	////	//min fm freq, 875 for 87.5Mhz in 100khz step; 7000 for 70Mhz in 50Khz step
#define MAX_tlgFM_FREQUENCY 1080	//max fm freq, 1080 for 108Mhz in 100khz step; 10800 for 108Mhz in 50Khz step
#define FM_FREQ_BOUND_PROC(var) if(*var < MIN_tlgFM_FREQUENCY){ *var = MIN_tlgFM_FREQUENCY;}else if(*var >MAX_tlgFM_FREQUENCY){*var = MAX_tlgFM_FREQUENCY;}

extern uint32 tlg_cur_fm_ssi;
kal_bool pre_valid;
uint32 ssi_fm[2*FM_SEARCH_STEP + 1];	//
kal_bool begin_valid_search = FALSE;

static uint16 g_uCurFreq = 0;

extern TLGDLL_API uint16 TLG_GetSSIThreshold(void);
extern TLGDLL_API int TLGAPP_FMSetFrequency(uint32 center_freq_hz);
extern TLGDLL_API void TLGAPP_FMStartScan(void);
extern TLGDLL_API int TLGAPP_FMSetParam(uint32 step_size, uint32 min_hz, uint32 max_hz);
extern TLGDLL_API void TLGAPP_FMStopScan(void);


static void TLG_FMDrv_SetFreq(short curf)	/* input value: 875 - 1080 ( 87.5 MHz - 108.0 MHz)*/
{
	TLG_FMDrv_Mute(1);
	if(!tlg_init_done)
	{
		TLGAPP_Init(TLGI2C_DEFAULT_BASE_ADDR);
	}
	
	TLGAPP_FMSetFrequency(curf*100000); 
	TLG_FMDrv_Mute(0);
}

static uint8 TLG_FMDrv_GetSigLvl(short curf)
{
	uint8	rssi;
	int a;
	
	TLG_FMDrv_SetFreq(curf);
	a =tlg_cur_fm_ssi;	
	if(a > 100)
	{
		a = 100;
	}
	rssi = a;
	return rssi;
}


static void TLG_FMDrv_EvaluateRSSIThreshold(void)
{
	pre_valid =TRUE;
	begin_valid_search = TRUE;
}


static uint8 TLG_FMDrv_ValidStop(short freq, signed char signalvl, bool is_step_up)
{
	int rssi;
	uint8 i,result=0;
	short nxt_frq;
		
	for(i = 1; i < 2*FM_SEARCH_STEP+1; i++ )
    {    
		ssi_fm[i-1] = ssi_fm[i];
    }

	if(begin_valid_search)
	{	
		rssi = TLG_FMDrv_GetSigLvl(freq);
		//ssi_fm[0] = ssi_fm[1] = ssi_fm[2] = tlg_cur_fm_ssi;
		for(i = 0; i < FM_SEARCH_STEP+1; i++ )
        {      
			ssi_fm[i] = tlg_cur_fm_ssi;
        }
		
		for(i = 1; i < FM_SEARCH_STEP; i++)
		{
			if(is_step_up)
			{
				nxt_frq = freq + i;
			}
			else
			{
				nxt_frq = freq - i;
			}
			FM_FREQ_BOUND_PROC(&nxt_frq);
			rssi = TLG_FMDrv_GetSigLvl(nxt_frq);
			ssi_fm[FM_SEARCH_STEP+i] = tlg_cur_fm_ssi;
		}		
		begin_valid_search = FALSE;
	}
	
	if(is_step_up)
	{
		nxt_frq = freq + FM_SEARCH_STEP;
	}
	else
	{
		nxt_frq = freq - FM_SEARCH_STEP;
	}	
	FM_FREQ_BOUND_PROC(&nxt_frq);
	rssi =	TLG_FMDrv_GetSigLvl(nxt_frq);
	ssi_fm[FM_SEARCH_STEP*2] = tlg_cur_fm_ssi;
	
	if(pre_valid)
	{
		pre_valid = FALSE;
		result = 0;
	}
	else 
	{
		for(i=0;i < FM_SEARCH_STEP; i++)
		{
			if((ssi_fm[FM_SEARCH_STEP] <= ssi_fm[i]) || (ssi_fm[FM_SEARCH_STEP] <= ssi_fm[FM_SEARCH_STEP+1+i]))//invalid
			{
			 break;		
			}
		}
		if(i == FM_SEARCH_STEP)
		{			
			result = 1;
			pre_valid = TRUE;
		}				
	}
	//rssi = FMDrv_GetSigLvl(freq);
	return result;
}


void FMDrv_StartScan(void)
{
	TLGAPP_FMStartScan();
	TLGAPP_FMSetParam(100000, MIN_tlgFM_FREQUENCY*100000, MAX_tlgFM_FREQUENCY*100000);	
}

void FMDrv_StopScan(void)
{
	TLGAPP_FMStopScan();
}


void TLG1120_fm_init(void)
{
    TLG_PRINT_0("TLG1120_fm_init Enter");
	#if 0
    camsensor_preview_resolution  = CAMSENSOR_QTR_SIZE;
    camsensor_i2c_command.bus_id     = I2C_BUS_HW_CTRL;
	camsensor_i2c_command.slave_addr = (TLGI2C_DEFAULT_BASE_ADDR<<1);
	camsensor_i2c_command.options    = (i2c_options_type) (I2C_DFLT_ADDR_DEV| I2C_START_BEFORE_READ); 
	#endif 

#if 1	//For debug, we set some variable, after mmi is finish we disable it
    TLGMMI_RestoreTvOption();
    TLGMMI_RestoreFmOption();
    TLGMMI_ClearTvStorage();
    TLGMMI_ClearFmStorage();
    gTvStorage.mCurChnIdx = 47;
#endif

    TLG1120_init_host_gpio();
    TLG1120_poweron_chip();
	//TLG1120_tlg1120_gpio_init_i2c();
    TLG_Delay(25);
    TLG1120_reset_chip();
    
#ifdef TEST_TLG1120_I2C
    TLG1120_VerifyI2C();
#endif

	TLGAPP_Restart();
	TLGAPP_Init(tlg_i2c_addr);
	TLGAPP_SetChannelMapFM();
	TLGAPP_SetChannel(p_tlg_cur_map[gFmStorage.mCurChnIdx] CHN_S);
    TLG_PRINT_0("TLG1120_fm_init leave");
    
}

boolean TLG1120_fm_mute(boolean mute)
{
   ATV_DEBUG("TLG1120_fm_mute",0,0,0);
   return FALSE;
}

void TLG1120_fm_power_on(void)
{
    ATV_DEBUG("TLG1120_fm_power_on",0,0,0);
    return;
}

void TLG1120_fm_power_off(void)
{
    ATV_DEBUG("TLG1120_fm_power_off",0,0,0);
    TLGAPP_PowerDown();
    TLG1120_reset_chip();
	//TLG1120_tlg1120_gpio_init_i2c();
    TLG1120_poweroff_chip();
}



boolean TLG1120_fm_set_channel(int16 freq)
{
    int i = 0, ret = SUCCESS;
    boolean result = FALSE;
    
    ATV_DEBUG("TLG1120_fm_set_channel freq=%d",freq,0,0);

    g_uCurFreq = freq;
    FMDrv_StartScan();
    ret = TLG_FMDrv_ValidStop(freq,0,TRUE);  
    FMDrv_StopScan();
    if (ret == 0)
    {
        ATV_DEBUG("TLG_FMDrv_ValidStop no sign ret=%d",ret,0,0);
        return FALSE;
    }
    else        
    {
        for (i=0;p_tlg_cur_map[i]!=0;i++)
        {
            if ((p_tlg_cur_map[i]HZ_S/100000)==freq)
            {               
                break;
            }
        }  
        TLGAPP_SetChannel(p_tlg_cur_map[i]CHN_S);
        ATV_DEBUG("TLG_FMDrv_ValidStop have sign ret=%d",ret,0,0);
        return TRUE;
    }
}


static uint8 TLG1120_fm_get_sigLv1(int16 curf)
{
    ATV_DEBUG("TLG1120_fm_get_sigLv1",0,0,0);
    return 0;
}

static boolean TLG1120_Set_Band(uint32 band)
{
    ATV_DEBUG("TLG1120_Set_Band",0,0,0);
    return TRUE;
}

static boolean TLG1120_fm_set_volume_level(uint8 level)
{
    ATV_DEBUG("TLG1120_fm_set_volume_level",0,0,0);

#ifdef TLG1120_TV
    TLG1120_tv_set_param(TLG_SET_VOLUME,(int)level,0);
#endif

    return TRUE;
}

static int16 TLG1120_Get_Freq(void)
{
    ATV_DEBUG("TLG1120_Get_Freq",0,0,0);    
    return 0;
}



static uint16 TLG1120_fm_scan_channel(uint8 direction)    
{
    int  ret = 0, firstchn = 0;
    uint16 startfreq = g_uCurFreq;
    uint16 getchn = 0;
    
    ATV_DEBUG("TLG1120_fm_scan_channel",0,0,0); 

    FMDrv_StartScan();
#if 1
    do
    {
        getchn = 0;
        
        if (direction)
        {
            g_uCurFreq++;
        }
        else
        {
            g_uCurFreq--;
        }

        if(g_uCurFreq>1080)
		{
			g_uCurFreq=875; /* china band */
		}
		
		if(g_uCurFreq<875)
		{
			g_uCurFreq=1080;
		}

        getchn = TLG_FMDrv_ValidStop(g_uCurFreq, 0, TRUE);
        if (getchn == 0)
        {
            getchn = 1;
        }             
    }while((getchn==0)&&(startfreq!=g_uCurFreq));
#else   
    for(ChnInx=0;p_tlg_cur_map[ChnInx]CHN_S!=0;ChnInx++)
    {
        ret = TLG_FMDrv_ValidStop(p_tlg_cur_map[ChnInx]HZ_S/100000, 0, TRUE);
        if (ret == 0)
        {
            if (found == FALSE)
            {
                firstchn = p_tlg_cur_map[ChnInx]CHN_S;
                found = TRUE;
            }
            ATV_DEBUG("freq:%d no signal\n", 0, 0, 0);
        }
        else
        {                    
            ATV_DEBUG("freq:%d have signal\n", p_tlg_cur_map[ChnInx]CHN_S, 0, 0);           
        }
    }
#endif  
  
    FMDrv_StopScan();    
    
    g_uCurFreq = firstchn;
    
    ret = TLGAPP_SetChannel(firstchn);    
    ATV_DEBUG("TLGAPP_SetChannel ret = %d",ret ,0,0);
    return SUCCESS;       
}
#if 0   //modi by yangdecai 
#include "FM_Device.h"
boolean TLGFM_Register(FM_Device *Fun_table)
{
	Fun_table->FM_ChipInit 			= TLG1120_fm_init;
	Fun_table->FM_Mute				= TLG1120_fm_mute;
	Fun_table->FM_PowerOffProc		= TLG1120_fm_power_off;
	Fun_table->FM_PowerOn			= TLG1120_fm_power_on;
	Fun_table->FM_SetFreq			= TLG1120_fm_set_channel;
	Fun_table->FM_GetSigLvl			= TLG1120_fm_get_sigLv1;
	Fun_table->FM_GetFreq			= TLG1120_Get_Freq;
	Fun_table->FM_SetBand			= TLG1120_Set_Band;
	Fun_table->FM_Seek				= TLG1120_fm_scan_channel;
	Fun_table->FM_SetVolumeLevel	= TLG1120_fm_set_volume_level;
	return TRUE;
}
#endif
#endif

#if 0
static boolean TLG1120_fm_set_channel(int16 freq)    
{
    int ChnInx = 0, ret = 0, firstchn = 0;
    boolean found = FALSE;
    uint16 startfreq = g_uCurFreq;
    uint16 getchn = 0;
    
    ATV_DEBUG("TLG1120_fm_scan_channel",0,0,0); 

    FMDrv_StartScan();
#if 1
    

    getchn = TLG_FMDrv_ValidStop(freq, 0, TRUE);
    
 #else   
    for(ChnInx=0;p_tlg_cur_map[ChnInx]CHN_S!=0;ChnInx++)
    {
        ret = TLG_FMDrv_ValidStop(p_tlg_cur_map[ChnInx]HZ_S/100000, 0, TRUE);
        if (ret == 0)
        {
            if (found == FALSE)
            {
                firstchn = p_tlg_cur_map[ChnInx]CHN_S;
                found = TRUE;
            }
            ATV_DEBUG("freq:%d no signal\n", 0, 0, 0);
        }
        else
        {                    
            ATV_DEBUG("freq:%d have signal\n", p_tlg_cur_map[ChnInx]CHN_S, 0, 0);           
        }
    }
#endif  
  
    FMDrv_StopScan();    
    
    g_uCurFreq = getchn;
    
    ret = TLGAPP_SetChannel(g_uCurFreq);    
    ATV_DEBUG("TLGAPP_SetChannel ret = %d",ret ,0,0);
    return TRUE;
       
}
#endif

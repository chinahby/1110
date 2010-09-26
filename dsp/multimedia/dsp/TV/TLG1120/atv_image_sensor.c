#ifndef WIN32

#if 0   //add by yangdecai 09-24
#include "kal_release.h"
#include "stack_config.h"
#include "isp_if.h"
#include "L1audio.h"
#endif
//#include "timerEvents.h"

#include "TlgInclude.h"

#include "Platform.h"



/* Global Valuable */
#ifdef WIN32
kal_bool isp_digital_zoom_flag=FALSE;
#else /* WIN32 */
extern kal_bool isp_digital_zoom_flag;
#endif
#if TV_FOR_MTK	
extern void cis_module_standby(kal_bool on);
#else
void cis_module_standby(kal_bool on)
{
}
#endif
TLG_TV_SCANLINES gTvScanLines = TLG_TV_625_LINES;

static void ATV_InitHostI2C()
{
	// Initial host I2C interface
	GPIO_ModeSetup(ATV_I2C_SDA, 0);
	GPIO_InitIO(1, ATV_I2C_SDA);
	GPIO_WriteIO(1, ATV_I2C_SDA);
	GPIO_ModeSetup(ATV_I2C_SCL, 0);
	GPIO_InitIO(1, ATV_I2C_SCL);
	GPIO_WriteIO(1, ATV_I2C_SCL);
}

static void ATV_DeInitHostI2C()
{
	// De-Init host I2C interface
	GPIO_ModeSetup(ATV_I2C_SDA, 0);
	GPIO_InitIO(1, ATV_I2C_SDA);
	GPIO_WriteIO(0, ATV_I2C_SDA);
	GPIO_ModeSetup(ATV_I2C_SCL, 0);
	GPIO_InitIO(1, ATV_I2C_SCL);
	GPIO_WriteIO(0, ATV_I2C_SCL);
}

static void ATV_ChipPowerOn()
{
	// Turn on 1.2V & 2.8V LDO
	GPIO_ModeSetup(ATV_POWER_PIN, 0);
	GPIO_InitIO(1, ATV_POWER_PIN);
	GPIO_WriteIO(1, ATV_POWER_PIN);
}

static void ATV_ChipPowerOff()
{
	// Turn off 1.2V & 2.8V LDO
	GPIO_ModeSetup(ATV_POWER_PIN, 0);
	GPIO_InitIO(1, ATV_POWER_PIN);
	GPIO_WriteIO(0, ATV_POWER_PIN);
	GPIO_WriteIO(0,ATV_RESET_PIN);//added by macro lei 2009/03/31
}

static void ATV_ChipReset()
{
	GPIO_ModeSetup(ATV_RESET_PIN, 0);
	GPIO_InitIO(1, ATV_RESET_PIN);
	GPIO_WriteIO(1, ATV_RESET_PIN);
	TLG_Delay(10);
	GPIO_WriteIO(0, ATV_RESET_PIN);
	TLG_Delay(100);//25 modified by macro lei 2009/07/08
	GPIO_WriteIO(1, ATV_RESET_PIN);
	TLG_Delay(50);//25 modified by macro lei 2009/07/08
}

static void ATV_VerifyI2C()
{
	uint16 reg, write, read,ErrCnt = 0;
	AIT_Message_P0("ATV_VerifyI2C Test+");

	// Test I2C communication
	for (reg = 0x7F, write = 1; reg <= 0x8E; reg++, write <<= 1)
		TLG_WriteReg(tlg_i2c_addr, reg, write);

	for (reg = 0x7F, write = 1; reg <= 0x8E; reg++, write <<= 1)
	{
		TLG_ReadReg(tlg_i2c_addr, reg, &read);
		if (read != write)
		{
			ErrCnt++;
			AIT_Message_P2("ATV_VerifyI2C Test Fail!!Reg[%d] = %d",reg,read);
		}
	}

		AIT_Message_P1("ATV_VerifyI2C Test- #Error = %d",ErrCnt);
	
}

void PowerOnFM(void)
{
	ATV_InitHostI2C();
	ATV_ChipPowerOn();
	ATV_ChipReset();
	ATV_VerifyI2C();
	TLGAPP_Restart();
	TLGAPP_Init(tlg_i2c_addr);
	//TLGAPP_SetChannelMapFM();
	//TLGAPP_SetChannel(p_tlg_cur_map[gFmStorage.mCurChnIdx] CHN_S);
	TLGAPP_FMSetParam(100000, 87500000, 108000000);
}

void PowerOffFM(void)
{
	TLGAPP_PowerDown();
	ATV_ChipReset();
	ATV_DeInitHostI2C();
	ATV_ChipPowerOff();
}

/*************************************************************************
* FUNCTION
* init_ATV
*
* DESCRIPTION
* This function initialize the ATV and ISP control register.
*
* PARAMETERS
* None
*
* RETURNS
* None
*
* GLOBALS AFFECTED
*
*************************************************************************/

kal_bool sensor_type_atv=KAL_FALSE;

kal_int8 init_ATV(void)
{
	AIT_Message_P0("init_ATV+");

	// Set polarities of PCLK, HSYNC and VSYNC
	ATV_ChipPowerOn();
	ATV_InitHostI2C();
	TLG_Delay(25);
	ATV_ChipReset();
	ATV_VerifyI2C();	
	//cis_module_standby(KAL_TRUE);	

	sensor_type_atv=KAL_TRUE;

	AIT_Message_P0("init_ATV-");

	return 1;
}

/*************************************************************************
* FUNCTION
* get_ATV_id
*
* DESCRIPTION
* None
*
* PARAMETERS
* None
*
* RETURNS
* None
*
* GLOBALS AFFECTED
*
*************************************************************************/
void get_ATV_id(kal_uint8 *sensor_write_id, kal_uint8 *sensor_read_id)
{
	*sensor_write_id=0x48;
	*sensor_read_id=0x49;
}

/*************************************************************************
* FUNCTION
* get_ATV_size
*
* DESCRIPTION
* None
*
* PARAMETERS
* None
*
* RETURNS
* None
*
* GLOBALS AFFECTED
*
*************************************************************************/
void get_ATV_size(kal_uint16 *sensor_width, kal_uint16 *sensor_height)
{
}

/*************************************************************************
* FUNCTION
* get_ATV_period
*
* DESCRIPTION
* None
*
* PARAMETERS
* None
*
* RETURNS
* None
*
* GLOBALS AFFECTED
*
*************************************************************************/
void get_ATV_period(kal_uint16 *pixel_number, kal_uint16 *line_number)
{
}

#if 0
void entry_loop_reset_tv(void)
{   
    custom_reset_video_interface(); 

    kal_set_timer(atv_reset_isp_timer_id, (kal_timer_func_ptr)entry_loop_reset_tv, NULL, 100, 0);
}

kal_uint16 atv_grab_start_x=0;
kal_uint16 atv_grab_start_y=8;
kal_uint16 atv_exposure_window_width=340;
kal_uint16 atv_exposure_window_height=280;
#endif


kal_uint16 ATV_Get_ScanLines(void)
{
	if(gTvScanLines==TLG_TV_525_LINES)
		return 525;
	else
		return 625;
}
#if TV_FOR_MTK
#include "timerEvents.h"
static void ATV_Timer_CallBack(void)
{

	kal_uint32 save_irq_mask;
	save_irq_mask=SaveAndSetIRQMask();
	isp_digital_zoom_flag=KAL_TRUE;
	ENABLE_CAMERA_FRAME_READY_INT;
	RestoreIRQMask(save_irq_mask);

	StopTimer(ATV_TIMER_RESET_DATA_PATH);

	StartTimer(ATV_TIMER_RESET_DATA_PATH, 200,ATV_Timer_CallBack);

}


void ATV_preview(image_sensor_exposure_window_struct *image_window, image_sensor_config_struct *sensor_config_data)
{

	SET_TG_OUTPUT_CLK_DIVIDER(1);
	SET_CMOS_RISING_EDGE(0);
	SET_CMOS_FALLING_EDGE(0);
	ENABLE_CAMERA_PIXEL_CLKIN_ENABLE;
	SET_TG_PIXEL_CLK_DIVIDER(1);
	SET_CMOS_DATA_LATCH(1);
	DISABLE_CAMERA_TG_PHASE_COUNTER;
	DISABLE_CAMERA_CLOCK_OUTPUT_TO_CMOS;
	
	#if 1	
	if (1)//gTvScanLines == TLG_TV_525_LINES)
	{
		image_window->grab_start_x = 0; 
	       image_window->grab_start_y = 8;
		image_window->exposure_window_width= 320;
		image_window->exposure_window_height= 220;
	}
	else /* TvScanLines == TLG_TV_625_LINES */
	{
		image_window->grab_start_x = 0;
       	image_window->grab_start_y = 8;
		image_window->exposure_window_width= 330;
		image_window->exposure_window_height= 260;
	}
	#else
	if (gTvScanLines == TLG_TV_525_LINES)
	{
		image_window->grab_start_x = 0; 
	       image_window->grab_start_y = 4;
		image_window->exposure_window_width= 320;
		image_window->exposure_window_height= 234;
	}
	else /* TvScanLines == TLG_TV_625_LINES */
	{
		image_window->grab_start_x = 4;
       	image_window->grab_start_y = 12;
		image_window->exposure_window_width= 352;
		image_window->exposure_window_height= 264;
	}
	#endif
	sensor_config_data->sensor_frame_rate = 50;//added by macro lei 2009/08/25
	ATV_Timer_CallBack();
	//kal_sleep_task(80);
#ifdef WIN32
	printf("ISP window: w=%d, h=%d\n",	image_window->exposure_window_width, image_window->exposure_window_height);
#else
#endif
}

void ATV_capture(image_sensor_exposure_window_struct *image_window, image_sensor_config_struct *sensor_config_data)
{
}
#endif
/*************************************************************************
* FUNCTION
* write_ATV_reg
*
* DESCRIPTION
* None
*
* PARAMETERS
* None
*
* RETURNS
* None
*
* GLOBALS AFFECTED
*
*************************************************************************/
void write_ATV_reg(kal_uint32 addr, kal_uint32 para)
{
}

/*************************************************************************
* FUNCTION
* read_cmos_sensor
*
* DESCRIPTION
* None
*
* PARAMETERS
* None
*
* RETURNS
* None
*
* GLOBALS AFFECTED
*
*************************************************************************/
kal_uint32 read_ATV_reg(kal_uint32 addr)
{
	return 0;
}

/*************************************************************************
* FUNCTION
* set_ATV_shutter
*
* DESCRIPTION
* None
*
* PARAMETERS
* None
*
* RETURNS
* None
*
* GLOBALS AFFECTED
*
*************************************************************************/
void set_ATV_shutter(kal_uint16 shutter)
{
}

/*************************************************************************
* FUNCTION
* set_ATV_gain
*
* DESCRIPTION
* None
*
* PARAMETERS
* None
*
* RETURNS
* None
*
* GLOBALS AFFECTED
*
*************************************************************************/
kal_uint16 set_ATV_gain(kal_uint16 gain)
{
	return 0;
}

/*************************************************************************
* FUNCTION
* ATV_night_mode
*
* DESCRIPTION
* None
*
* PARAMETERS
* None
*
* RETURNS
* None
*
* GLOBALS AFFECTED
*
*************************************************************************/
void ATV_night_mode(kal_bool enable)
{
}

/*************************************************************************
* FUNCTION
* set_ATV_flashlight
*
* DESCRIPTION
* None
*
* PARAMETERS
* None
*
* RETURNS
* None
*
* GLOBALS AFFECTED
*
*************************************************************************/
void set_ATV_flashlight(kal_bool enable)
{
}

/*************************************************************************
* FUNCTION
* yuv_ATV_setting
*
* DESCRIPTION
* None
*
* PARAMETERS
* None
*
* RETURNS
* None
*
* GLOBALS AFFECTED
*
*************************************************************************/
kal_uint32 yuv_ATV_setting(kal_uint32 cmd, kal_uint32 para)
{
	return KAL_TRUE;
}

/*************************************************************************
* FUNCTION
* power_off_ATV
*
* DESCRIPTION
* This function is to turn off ATV power.
*
* PARAMETERS
* None
*
* RETURNS
* None
*
* GLOBALS AFFECTED
*
*************************************************************************/

void power_off_ATV(void)
{
	if(sensor_type_atv == TRUE)
	{
#if TV_FOR_MTK
		//StopTimer(ATV_TIMER_CHANNEL_CHANGE);
		StopTimer(ATV_TIMER_RESET_DATA_PATH);

		// Turn off FM audio amplifier
#ifdef WIN32
#else
		AFE_TurnOffSpeaker(L1SP_FM_RADIO);
		AFE_SwitchExtAmplifier(KAL_FALSE);
#endif
#endif
		// Restore backlight according to system default
		TLGAPP_PowerDown();
		ATV_ChipReset();
		ATV_DeInitHostI2C();
		ATV_ChipPowerOff();
		cis_module_standby(KAL_FALSE);
		sensor_type_atv=KAL_FALSE;
	}
}

#if TV_FOR_MTK
static kal_bool atv_standby=KAL_FALSE;
void ATV_Standby(kal_bool on)
{
	if(on)
	{
		if(atv_standby)
			return;
		init_ATV();
		TLGAPP_Restart();
		TLGAPP_Init(tlg_i2c_addr);
		TLGAPP_PowerDown();
		#if 0
		ATV_InitHostI2C();
		ATV_ChipPowerOn();
		ATV_ChipReset();
		ATV_VerifyI2C();
		TLGAPP_PowerDown();
		#endif
	}
	else
	{
		if(!atv_standby)
			return;
		ATV_ChipReset();
		ATV_DeInitHostI2C();
		ATV_ChipPowerOff();
	}
	atv_standby=on;
}

/*************************************************************************
* FUNCTION
* image_sensor_func_ATV
*
* DESCRIPTION
* ATV functions struct.
*
* PARAMETERS
* None
*
* RETURNS
* None
*
* GLOBALS AFFECTED
*
*************************************************************************/

image_sensor_func_struct image_sensor_func_ATV =
{
	init_ATV,
	get_ATV_id,
	get_ATV_size,
	get_ATV_period,
	ATV_preview,
	ATV_capture,
	write_ATV_reg,
	read_ATV_reg,
	set_ATV_shutter,
	ATV_night_mode,
	power_off_ATV,
	set_ATV_gain,
	set_ATV_flashlight,
	yuv_ATV_setting
};

void atv_image_sensor_func_config(void)
{
    image_sensor_func = &image_sensor_func_ATV;
}

#endif
#endif


/*
 * FileName:       
 * Author:       
 * Description:    
 * Version:        
 * Function List:  
 *                 1.
 * History:        
 *     <author>   <time>    <version >   <desc>
 */

#include "drv_comm.h"
#include "IntrCtrl.h"
#include "reg_base.h"
#include "gpio_sw.h"
#include "isp_if.h"
#include "analog_tv.h"
#include "upll_ctrl.h"
#include "sccb.h"

#include "tlg1100i2c.h"
#include "tlg1100app.h"

#include "rda5888adp.h"

#include "afe.h"
#include "l1audio.h"
#include "TimerEvents.h"

#include "camera_if.h"

//for MMI/ISP (区分ANALOG TV 和Camera应用)
static unsigned char is_ATVAppRunningModule = 0;
kal_uint16 			channlNo,channlNoMAX;
kal_bool is_fullscr =KAL_FALSE;

//区分 FM/TV 
 unsigned char    	is_TLG1100_state 		= ATV_STATE;
//ExtAmplifier control
static kal_bool is_ExtAmplifier_Close = KAL_TRUE;
//not used;but need define it(for fm radio)
uint8 const FM_RADIO_INPUT_LEVEL_THRESHOLD = 5;

static unsigned char atv_power_state = ATV_POWER_OFF;
extern kal_uint16 current_tv_area;
extern kal_uint16 current_chn;

extern void image_sensor_func_config(void);

/*************************************************************************
* FUNCTION
*	atv_delayms
*
* DESCRIPTION
*	This function delay some times.
*
* PARAMETERS
*	None
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
extern void AFE_SetAnalogTvAudioPath(void);
#if 0
static void analogtv_delayms(kal_uint16 time)
{
	volatile kal_uint32 delay;
	while(time--)	
	for (delay =0;delay <6000;delay++) {} // 1ms
}
#else
#define analogtv_delayms(a)	TLG_Delay(a)
#endif
/*************************************************************************
* FUNCTION
*	atv_module_power_on
*
* DESCRIPTION
*	This function control power of  analog tv IC
*
* PARAMETERS
*	None
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
extern void cis_module_power_on(kal_bool on) ;
void analogtv_module_power_on(unsigned char on)
{
	kal_prompt_trace(0,"analogtv_module_power_on, power = %d", on);
	GPIO_ModeSetup(ATV_I2C_SDA_PIN,0);
	GPIO_ModeSetup(ATV_I2C_SCL_PIN,0);
	GPIO_ModeSetup(ATV_POWER_PIN,0);
	GPIO_InitIO(1, ATV_I2C_SDA_PIN);
	GPIO_InitIO(1, ATV_I2C_SCL_PIN);
	GPIO_InitIO(1, ATV_POWER_PIN);

	if(on)
	{
		GPIO_WriteIO(1, ATV_POWER_PIN);// ATV power on
		if (g_nRdamtvTimer == 0)
            GPTI_GetHandle(&g_nRdamtvTimer);		
	}
	else
	{
		GPTI_StopItem(g_nRdamtvTimer);
		tlg_init_done = 0;
		GPIO_WriteIO(0, ATV_POWER_PIN); // ATV power off
		cis_module_power_on(KAL_FALSE);
		GPIO_ModeSetup(14,0);
		GPIO_InitIO(1, 14);
		GPIO_WriteIO(0, 14);
	}
}

/*************************************************************************
* FUNCTION
*	atv_init_TLG1100
*
* DESCRIPTION
*	This function initialize the registers of analog tv IC and ISP control register.
*
* PARAMETERS
*	None
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
extern void ov7670_poweron_for_tv(kal_bool on_off);
extern void power_for_tv(kal_bool on_off);
extern void BYD_power_for_tv(kal_bool on_off);
kal_int8 Init_TLG1100(void)
{


    TLGAPP_Init(0x62);
	analogtv_delayms(50);

	atv_power_state = ATV_POWER_ON;
	return 1;
	
}	/* atv_init_TLG1100() */


/*************************************************************************
* FUNCTION
*	atv_power_off_TLG1100
*
* DESCRIPTION
*	This function is to turn off analog tv module power.
*
* PARAMETERS
*	None
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
static void power_off_TLG1100(void)
{	
	if(atv_power_state == ATV_POWER_OFF) return ;
	analogtv_module_power_on(KAL_FALSE);
	atv_power_state = ATV_POWER_OFF;
}	/* atv_power_off_TLG1100 */

/*************************************************************************
* FUNCTION
*	atv_preview_TLG1100
*
* DESCRIPTION
*	This function is to start to play analog tv module.
*
* PARAMETERS
*	None
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
static void preview_TLG1100(image_sensor_exposure_window_struct *image_window, image_sensor_config_struct *sensor_config_data)
{
  	switch(current_tv_area)
	{
		case TV_AREA_CHINA:
		case TV_AREA_CHINA_SHENZHEN:
		case TV_AREA_CHINA_HONGKONG:
		case TV_AREA_ARGENTINA:   
		case TV_AREA_WESTERNEUROP:
		case TV_AREA_TURKEY:
		//case TV_AREA_UAE:
		case TV_AREA_AFGHANISTA:
		case TV_AREA_SINGAPORE:
		case TV_AREA_THAI:
		case TV_AREA_CAMBODIA:
		case TV_AREA_INDONESIA:
		case TV_AREA_MALAYSIA:
		case TV_AREA_LAOS:
		case TV_AREA_INDIA:
		case TV_AREA_PAKISTAN:
		case TV_AREA_SPAIN:
		case TV_AREA_PORTUGAL:			
		case TV_AREA_VIETNAM:
		case TV_AREA_UK:
		case TV_AREA_SOUTH_AFRICA:	
			image_window->grab_start_x=4+40;//16; // 4
			image_window->grab_start_y=4; // 4
			image_window->exposure_window_width=352-40;//-12;
			image_window->exposure_window_height=280;
			break;
		case TV_AREA_CHINA_TAIWAN:	
		case TV_AREA_AMERICA:	
		case TV_AREA_CANADA:
		case TV_AREA_JAPAN:
		case TV_AREA_BRAZIL: 
		case TV_AREA_BURMA:
		case TV_AREA_MEXICO:
		case TV_AREA_PHILIPINES:
		case TV_AREA_KOREA:
		case TV_AREA_CHILE:
		case TV_AREA_VENEZUELA: 
			image_window->grab_start_x=16+40;
			image_window->grab_start_y=4;
			image_window->exposure_window_width=320;	
			image_window->exposure_window_height=236;
			break;
		default:
			image_window->grab_start_x=4;
			image_window->grab_start_y=4;
			image_window->exposure_window_width=352;
			image_window->exposure_window_height=280;
			break;
	}	

    kal_prompt_trace(0, "[rdamtv]preview_TLG1100 area=%d, s_x=%d, s_y=%d\n",
		current_tv_area, image_window->grab_start_x, image_window->grab_start_y);
		
}	/* atv_preview_TLG1100 */

void SetAtv_Fullscreen(kal_bool on)
{
	is_fullscr = on;
}

void ATV_Fullscreen(void)
{
     is_fullscr= is_fullscr? KAL_FALSE: KAL_TRUE;

}
/*************************************************************************
//these functions not used, set default
*************************************************************************/
static void get_TLG1100_id(kal_uint8 *sensor_write_id, kal_uint8 *sensor_read_id)
{
	return;
}
static void get_TLG1100_size(kal_uint16 *sensor_width, kal_uint16 *sensor_height)
{
	*sensor_width  = 352;
	*sensor_height = 232;
}
static void get_TLG1100_period(kal_uint16 *pixel_number, kal_uint16 *line_number)
{
	*pixel_number = 352;
	*line_number = 232;
}
static void TLG1100_capture(image_sensor_exposure_window_struct *image_window, image_sensor_config_struct *sensor_config_data)
{
	return;
}
static void write_TLG1100_reg(kal_uint32 addr, kal_uint32 para)
{
	return;
}
kal_uint32 read_TLG1100_reg(kal_uint32 addr)
{
	return 0x40;
}
static void set_TLG1100_shutter(kal_uint16 shutter)
{
	return;
}
static void TLG1100_night_mode(kal_bool enable)
{
	return;
}

static kal_uint16 set_TLG1100_gain(kal_uint16 gain)
{
	return 0x40;
}

static void set_TLG1100_flashlight(kal_bool enable)
{
	return;
}
static kal_uint32 TLG1100_yuv_setting(kal_uint32 cmd, kal_uint32 para)
{
	return KAL_TRUE;
}
/*************************************************************************
*************************************************************************/
/*************************************************************************
* FUNCTION
*	analog_tv_func_TLG1100
*
* DESCRIPTION
*	TLG1100  functions struct.
*
* PARAMETERS
*	none
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
static image_sensor_func_struct analogtv_func_TLG1100=
{
	Init_TLG1100,
	get_TLG1100_id,
	get_TLG1100_size,
	get_TLG1100_period,
	preview_TLG1100,
	TLG1100_capture,
	write_TLG1100_reg,
	read_TLG1100_reg,
	set_TLG1100_shutter,
	TLG1100_night_mode,	
	power_off_TLG1100,
	set_TLG1100_gain,
	set_TLG1100_flashlight,
	TLG1100_yuv_setting
};	/* analog_tv_func_TLG1100 */

static void analogtv_func_config(void)
{
    kal_prompt_trace(0, "[rdamtv]analogtv_func_config\n");
	image_sensor_func = &analogtv_func_TLG1100;
}

/*************************************************************************
* FUNCTION
*	atv_get_func_config
*
* DESCRIPTION
*	This function maps the analog tv module function API structure.
*
* PARAMETERS
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
void analogtv_sensor_func_config(void)
{
	if(ATV_APP_MODULE == analogtv_get_curmodule())
		analogtv_func_config(); 
	else
		image_sensor_func_config(); 
}	/* analogtv_sensor_func_config() */

/*************************************************************************/
/*************************************************************************
* FUNCTION
*	analogtv_get_currmodule
*
* DESCRIPTION
*	This function maps the analog tv module or camera module.
*
* PARAMETERS
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
unsigned char analogtv_get_curmodule(void)
{
	return is_ATVAppRunningModule;
}
/*************************************************************************
* FUNCTION
*	atv_set_current_running_module
*
* DESCRIPTION
*	This function maps the analog tv module or camera module.
*
* PARAMETERS
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
void analogtv_set_curmodule(unsigned char is_atv_app)
{
	is_ATVAppRunningModule = is_atv_app;
}
/*************************************************************************/
//search channel
void Autoscan_ATV_Channel(void)
{
	kal_bool is_auto=KAL_TRUE;
	kal_uint16 i,channl[100];
	
	if(is_auto)
		{
			for(i=0;i<80;i++)
				if(TLGAPP_SetChannel(i)==1)
				{
					channl[channlNo++]=i;
					analogtv_delayms(1000);
					//kal_prompt_trace(MOD_MED,"##INFO--i=%d,j=%d",i,channlNo);
					}
			}
	else
		{
				if(TLGAPP_SetChannel(i)==1)
					channl[channlNo++]=i;
			
		}
	channlNoMAX=channlNo;
	TLGAPP_SetChannel(channl[1]);
//	return;
	
}

/*************************************************************************/
unsigned char Get_TLG1100_State(void)
{
	return is_TLG1100_state;
}
void  Set_TLG1100_State(unsigned char state)
{
	is_TLG1100_state = state;
}

unsigned short analogtv_get_chn_count(void)
{extern int tlg_cur_chn_cnt;

	return tlg_cur_chn_cnt;
}


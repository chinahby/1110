
#include "cam_if_ait_api.h"
#include "AIT_Interface.h"
#include "a8_common.h"
#include "Rex.h"
#include "clk.h"
#include "clkregim.h"

#if defined(AIT701_DEMO_BOARD)
#include	"parts/r40008/lib_r40008.h"
const unsigned char DCAM_EINT_NO = 0;

#elif defined(__QSC_TARGET__)
#include "gpio_1100.h"
#endif

//add by yangdecai 09-25
#define	GPIO_ModeSetup(a, b)   
#define	GPIO_InitIO(a, b)      
#define	GPIO_WriteIO(a, b)


char  SysMsg[128];



//#define A8_MAIN_LCD_BUS_WIDTH	(8)

#define A8_SUB_LCD_WIDTH			(96)
#define A8_SUB_LCD_HEIGHT			(96)



unsigned char AIT_sleep_enable=1;



const unsigned short ait_sleep_checktime = AIT_SLEEP_CHECKTIME;
const kal_uint32 CS_AitBypass   = ((0x01<<30)|(0x01<<26)|(0x03<<8)|0x10);
const kal_uint32 CS_AitActiveWithoutPll = ((0x01<<30)|(0x01<<26)|(0x09<<8)|0x09);
const kal_uint32 CS_AitActiveWithPll   =((0x01<<30)|(0x01<<26)|(0x09<<8)|0x09);
unsigned short gA8MainLCDWidth = A8_MAIN_LCD_WIDTH;
unsigned short gA8MainLCDHeight = A8_MAIN_LCD_HEIGHT;

unsigned short gA8SubLCDWidth = A8_SUB_LCD_WIDTH;
unsigned short gA8SubLCDHeight = A8_SUB_LCD_HEIGHT;
const unsigned short maxNumPrevMode = AIT_PREV_MAX_NUMBER; 

unsigned short gLCDWidth = A8_MAIN_LCD_WIDTH;

// Panel Related
void AIT_ext_SetLCDWindow(unsigned short x,unsigned short y,unsigned short w,unsigned short h)
{
#if defined(__QSC_TARGET__)
//Customer define
    //extern void st7735_set_camera_area(uint32 start_row, uint32 start_col,uint32 end_row, uint32 end_col);

    //st7735_set_camera_area(x, y, w-1, h-1);
#else
//	LCD_INIT();   //modi by yangdecai 09-25
//	ScreenTest();
#endif	
}


void AIT_ext_SetLCDRotate(unsigned char bRotate)
{
#if 1
	return;//need to implement in the lcd.c for full screen rotate play function
#else

#if defined(__QSC_TARGET__)

#endif	
	
#endif	

}


const sLCD_ATTRIBUITE gsPreviewAttrib[]=
{

#if defined(__MMI_MAINLCD_240X320__)
	{
		AIT_CAM_PREV_FULL_MODE,
		CAM_ROTATE_NORMAL,
		{0,0,A8_MAIN_LCD_WIDTH,A8_MAIN_LCD_HEIGHT},
		{0,4,180,20},
		{200,35,40,210},
		{0,290,240,30}
	},
	{
		AIT_CAM_PREV_NOR_MODE,
		CAM_ROTATE_NORMAL,
		{0,30,240,196},	
		{0,4,180,20},
		{200,35,40,210},
		{0,290,240,30}
	
	},	
	{
		AIT_VDO_PREV_MODE,
		CAM_ROTATE_NORMAL,
		{0,30,240,196},	
		{0,0,A8_MAIN_LCD_WIDTH,30},
		{0,A8_MAIN_LCD_HEIGHT-94,A8_MAIN_LCD_WIDTH,94},
		{0,0,0,0}		
	},
	{
		AIT_JPG_PLAY_FULL_MODE,
		CAM_ROTATE_NORMAL,
		{0,0,A8_MAIN_LCD_WIDTH,A8_MAIN_LCD_HEIGHT},		
		{0,0,0,0},
		{0,0,0,0},
		{0,0,0,0}
	},
	{
		AIT_VDO_PLAY_NOR_MODE,
		CAM_ROTATE_NORMAL,
		{0,30,A8_MAIN_LCD_WIDTH,196},		
		{0,0,A8_MAIN_LCD_WIDTH,30},
		{0,A8_MAIN_LCD_HEIGHT-94,A8_MAIN_LCD_WIDTH,94},
		{0,0,0,0},

	},
	{
		AIT_ATV_PREV_NOR_MODE,
		CAM_ROTATE_NORMAL,
		{0,18,A8_MAIN_LCD_WIDTH,196},		
		{0,0,176,18},
		{0,0,176,58},
		{0,0,0,0}
	},
	{
		AIT_ATV_PREV_FULL_MODE,
		CAM_ROTATE_NORMAL,
		{0,0,320,240},
		{0,0,0,0},
		{0,0,0,0},
		{0,0,0,0}
	},
#if AIT_VIDEO_PHONE_SUPPORT	
	{
		AIT_VIDPHONE_PREV_MODE,
		CAM_ROTATE_NORMAL,
		{0,0,80,64},		
		{0,0,0,0},
		{0,0,0,0},
		{0,0,0,0}
	},
#endif

#elif defined(__MMI_MAINLCD_176X220__)
//Preview
	{
		AIT_CAM_PREV_FULL_MODE,
		CAM_ROTATE_NORMAL,//CAM_ROTATE_NORMAL,
		{0,0,A8_MAIN_LCD_WIDTH,A8_MAIN_LCD_HEIGHT},
		{0,4,150,16},
		{150,4,20,170},
		{4,196,170,20}
	},
	{
		AIT_CAM_PREV_NOR_MODE,
		CAM_ROTATE_NORMAL,
		{0,18,A8_MAIN_LCD_WIDTH,144},	
		{0,0,140,20},
		{150,3,20,170},
		{2,202,172,18}
	},	
	{
		AIT_VDO_PREV_MODE,
		CAM_ROTATE_NORMAL,
		{0,18,A8_MAIN_LCD_WIDTH,144},	
		{0,0,A8_MAIN_LCD_WIDTH,18},
		{150,18,20,100},
		{0,A8_MAIN_LCD_HEIGHT-56,A8_MAIN_LCD_WIDTH,56}		
	},

//Video & JPEG	
	{
		AIT_VDO_PLAY_NOR_MODE,
		CAM_ROTATE_NORMAL,
		{0,18,A8_MAIN_LCD_WIDTH,144},		
		{0,0,A8_MAIN_LCD_WIDTH,18},
		{0,0,0,0},
		{0,A8_MAIN_LCD_HEIGHT-56,A8_MAIN_LCD_WIDTH,56}		
	},
	{
		AIT_VDO_PLAY_FULL_MODE,
		CAM_ROTATE_90,
		{0,0,220,176},		
		{0,0,0,0},
		{0,0,0,0},
		{0,0,0,0}		
	},
	{
		AIT_JPG_PLAY_FULL_MODE,
		CAM_ROTATE_NORMAL,
		{0,0,A8_MAIN_LCD_WIDTH,A8_MAIN_LCD_HEIGHT},		
		{0,0,0,0},
		{0,0,0,0},
		{0,0,0,0}
	},
	{
		AIT_VDO_PLAY_YUVBUF_NOR_MODE,
		CAM_ROTATE_NORMAL,
		{0,18,A8_MAIN_LCD_WIDTH,144},	
		{0,0,A8_MAIN_LCD_WIDTH,18},
		{0,0,0,0},
		{0,A8_MAIN_LCD_HEIGHT-58,A8_MAIN_LCD_WIDTH,58}

	},	
	{
		 AIT_VDO_PLAY_YUVBUF_FULL_MODE,
		CAM_ROTATE_NORMAL,
		{0,0,A8_MAIN_LCD_HEIGHT,A8_MAIN_LCD_WIDTH},		
		{0,0,0,0},
		{0,0,0,0},
		{0,0,0,0}

	},		

	
//TV		
	{
		AIT_ATV_PREV_NOR_MODE,
		CAM_ROTATE_NORMAL,
		{0,18,176,144},		
		{0,0,176,18},
		{0,0,176,58},
		{0,0,0,0}
	},
	{
		AIT_ATV_PREV_FULL_MODE,
		CAM_ROTATE_90,
		{0,0,220,176},		
		{0,0,0,0},
		{0,0,0,0},
		{0,0,0,0}
	},
#if AIT_VIDEO_PHONE_SUPPORT	
	{
		AIT_VIDPHONE_PREV_MODE,
		CAM_ROTATE_NORMAL,
		{0,0,80,64},		
		{0,0,0,0},
		{0,0,0,0},
		{0,0,0,0}
	},
#endif


#elif defined(__MMI_MAINLCD_220X176__)
	{
		AIT_CAM_PREV_NOR_MODE,
		CAM_ROTATE_NORMAL,
		{0,0,176,144},
		{0,0,220,176}, 
		{0,0,0,0},
		{0,0,0,0}
	},
	{
		AIT_CAM_PREV_FULL_MODE,
		CAM_ROTATE_NORMAL,
		{0,0,220,176},
		{0,3,220,149/*30*/},
		{180,30,40,146},
		{0,152,180,24}
	},
	{
		AIT_VDO_PREV_MODE,
		CAM_ROTATE_NORMAL,
		{5,6,176,144},	
		{180,3,40,173},
		{0,150,179,26},
		{2,202,172,0}		
	},
		
	{
		AIT_ATV_PREV_NOR_MODE,
		CAM_ROTATE_NORMAL,
		{0,18,176,144},		
		{0,0,176,18},
		{0,0,176,58},
		{0,0,0,0}
	},
	{
		AIT_ATV_PREV_FULL_MODE,
		CAM_ROTATE_NORMAL,
		{0,0,220,176},		
		{0,0,0,0},
		{200,0,20,156},
		{0,156,220,20}
	},
	{
		AIT_JPG_PLAY_FULL_MODE,
		CAM_ROTATE_NORMAL,
		{0,0,A8_MAIN_LCD_WIDTH,A8_MAIN_LCD_HEIGHT},		
		{0,0,0,0},
		{0,0,0,0},
		{0,0,0,0}
	},
	
	{
		AIT_VDO_PLAY_NOR_MODE,
		CAM_ROTATE_NORMAL,
		{0,0,176,144},		
		{0,0,0,0},
		{181,0,39,176},
		{0,150,180,26}
	},
	{
		AIT_VDO_PLAY_FULL_MODE,
		CAM_ROTATE_NORMAL,
		{0,0,220,176},		
		{0,0,0,0},
		{0,0,0,0},
		{0,0,0,0}
	},
#if AIT_VIDEO_PHONE_SUPPORT	
	{
		AIT_VIDPHONE_PREV_MODE,
		CAM_ROTATE_NORMAL,
		{0,0,80,64},		
		{0,0,0,0},
		{0,0,0,0},
		{0,0,0,0}
	},
#endif
	
#elif defined(__MMI_MAINLCD_128X160__)
	{
		AIT_CAM_PREV_NOR_MODE,
		CAM_ROTATE_NORMAL,
		{0,0,128,96},
		{0,0,0,0},
		{0,0,0,0},
		{0,0,0,0}
	},
	{
		AIT_CAM_PREV_FULL_MODE,
		CAM_ROTATE_NORMAL,
		{0,0,A8_MAIN_LCD_WIDTH,A8_MAIN_LCD_HEIGHT},
		{0,0,A8_MAIN_LCD_WIDTH,25},
		{A8_MAIN_LCD_WIDTH-80,25,80,60},
		{0,A8_MAIN_LCD_HEIGHT-20,A8_MAIN_LCD_WIDTH,20}
	},
	{
		AIT_VDO_PREV_MODE,
		CAM_ROTATE_NORMAL,
		{0,10,A8_MAIN_LCD_WIDTH,96},	
		{0,0,A8_MAIN_LCD_WIDTH,10},
		{0,0,0,0},
		{0,A8_MAIN_LCD_HEIGHT-54,A8_MAIN_LCD_WIDTH,54}
	},
	{
		AIT_JPG_PLAY_FULL_MODE,
		CAM_ROTATE_NORMAL,
		{0,0,A8_MAIN_LCD_WIDTH,A8_MAIN_LCD_HEIGHT},		
		{0,0,0,0},
		{0,0,0,0},
		{0,0,0,0}
	},
	{
		AIT_VDO_PLAY_NOR_MODE,
		CAM_ROTATE_NORMAL,
		{0,10,A8_MAIN_LCD_WIDTH,96},		
		{0,0,A8_MAIN_LCD_WIDTH,10},
		{0,0,0,0},
		{0,A8_MAIN_LCD_HEIGHT-54,A8_MAIN_LCD_WIDTH,54}
	},
	{
		AIT_VDO_PLAY_FULL_MODE,
		CAM_ROTATE_90,
		{0,0,160,128},		
		{0,0,0,0},
		{0,0,0,0},
		{0,0,0,0}
	},	
	
	{
		AIT_VDO_PLAY_YUVBUF_NOR_MODE,
		CAM_ROTATE_NORMAL,
		{0,10,A8_MAIN_LCD_WIDTH,96},		
		{0,0,A8_MAIN_LCD_WIDTH,10},
		{0,0,0,0},
		{0,A8_MAIN_LCD_HEIGHT-54,A8_MAIN_LCD_WIDTH,54}

	},	
	{
		AIT_VDO_PLAY_YUVBUF_FULL_MODE,
		CAM_ROTATE_NORMAL,
		{0,10,A8_MAIN_LCD_WIDTH,A8_MAIN_LCD_HEIGHT},		
		{0,0,A8_MAIN_LCD_WIDTH,0},
		{0,0,0,0},
		{0,A8_MAIN_LCD_HEIGHT-54,A8_MAIN_LCD_WIDTH,0}

	},		
	{
		AIT_ATV_PREV_NOR_MODE,
		CAM_ROTATE_NORMAL,
		{0,32,A8_MAIN_LCD_WIDTH,96},		
		{0,0,0,0},
		{0,0,0,0},
		{0,0,A8_MAIN_LCD_WIDTH,A8_MAIN_LCD_HEIGHT}
	},
	{
		AIT_ATV_PREV_FULL_MODE,
		CAM_ROTATE_90,
		{0,0,A8_MAIN_LCD_HEIGHT,A8_MAIN_LCD_WIDTH},		
		{0,0,0,0},
		{0,0,0,0},
		{0,0,0,0}
	},
#if AIT_VIDEO_PHONE_SUPPORT	
	{
		AIT_VIDPHONE_PREV_MODE,
		CAM_ROTATE_NORMAL,
		{0,0,80,64},		
		{0,0,0,0},
		{0,0,0,0},
		{0,0,0,0}
	},
#endif

#endif		
//Don't Remove this item.	
	{AIT_PREV_MAX_NUMBER,	//ID for panel mode.
		0,						//Rotationg control
		{0,0,0,0},				//Preview range
		{0,0,0,0},				// Top OSD range	W or H is zero, OSD layer will be disable.
		{0,0,0,0},				// Center OSD range
		{0,0,0,0},				// Bottom OSD range
	},

};


// DSC Related
#ifndef WIN32
extern	t_sensor_manager	sensor_ov7670;
//extern	t_sensor_manager	sensor_ov7680;
extern	t_sensor_manager	sensor_ov7690;
//extern	t_sensor_manager	sensor_gc0307;
//extern	t_sensor_manager sensor_bf3503;
#endif

//TV
#ifdef AIT_ATV_SUPPORT
u_char g_ATV_Flag=0;
#if defined(ATV_CHIP_TLG1120)
extern	t_sensor_manager	sensor_tlg1120;
#elif defined(ATV_CHIP_RDA5888)
extern	t_sensor_manager	tv_rda5888;
#endif
#endif

t_sensor_manager* sensor_manager[] =
{
#ifndef WIN32

	&sensor_ov7670,
	//&sensor_ov7680,
	//&sensor_bf3503,
	//&sensor_ov7690,	
	//&sensor_gc0307,
#endif
	0
};


//Vin@20091210:Twin Sensor & TV
t_sensor_manager* ait_tv_manager[] =
{
#ifdef AIT_ATV_SUPPORT
#if defined(ATV_CHIP_TLG1120)
	&sensor_tlg1120,
#elif defined(ATV_CHIP_RDA5888)
	&tv_rda5888,
#endif
#endif
0
};

void AIT_ext_cam_cmd_mapping(void)
{
#if defined(__QSC_TARGET__)

#endif	
	return;
}
//OS Related Function
#if defined(__QSC_TARGET__)
const unsigned char DCAM_EINT_NO = 0;
static rex_crit_sect_type dsp_crit_sect = {0};
#define Delayms(t) clk_busy_wait(1000*t)

void AIT_ext_Create_Mutex(void)
{
	 /*if(dsp_lock.dsp_sem==NULL)
	 	dsp_lock.dsp_sem= kal_create_sem( "dsp_sema", 1 );
	 dsp_lock.lock_count = 0;
   	 dsp_lock.owner_id = NULL;*/
   	 rex_init_crit_sect(&dsp_crit_sect);
   	 
}

unsigned char AIT_ext_Take_Semaphore(unsigned short mode)
{

   	/*if( kal_query_systemInit() || INT_QueryExceptionStatus() ||kal_if_hisr())
        	return 1;
	{
		kal_status status;
		if(dsp_lock.dsp_sem == NULL)
		{
		      AIT_ext_Create_Mutex();
		}	
		status=kal_take_sem(dsp_lock.dsp_sem,mode);
		return status==KAL_SUCCESS ? 1 : 0;
	}*/
	rex_enter_crit_sect(&dsp_crit_sect);
    return 1;
}

void AIT_ext_Give_Semaphore(void)
{
	/*if( kal_query_systemInit() || INT_QueryExceptionStatus() ||kal_if_hisr())
	    	return;
	{
		if(dsp_lock.dsp_sem != NULL)
		         	kal_give_sem(dsp_lock.dsp_sem);
	}*/
	rex_leave_crit_sect(&dsp_crit_sect);
}

//AIT Related PIN control
void AIT_ext_ResetPinCtl(void)
{
	gpio_tlmm_config(AIT701_RESET);
    (void)gpio_out(AIT701_RESET, GPIO_HIGH_VALUE);
	Delayms(5);

	(void)gpio_out(AIT701_RESET, GPIO_LOW_VALUE);
	Delayms(20);

	(void)gpio_out(AIT701_RESET, GPIO_HIGH_VALUE);
	Delayms(5);
}

void AIT_ext_BypassPinCtl(unsigned char bEnable)
{
	if(bEnable>1)
	{
		//assert(0);
	}

	gpio_tlmm_config(AIT701_BYPASS);
    if(bEnable)
    {
        (void)gpio_out(AIT701_BYPASS, GPIO_HIGH_VALUE);
        //out_dword(HWIO_ADDR(LCD_CFG0), 0x04120105);
        //out_dword(HWIO_ADDR(LCD_CFG1), 0xc9000000);

    }
    else
    {
        (void)gpio_out(AIT701_BYPASS, GPIO_LOW_VALUE);
        //out_dword(HWIO_ADDR(LCD_CFG0), 0x068AFFFF);
        //out_dword(HWIO_ADDR(LCD_CFG1), 0xc9000000);

    }
}

void AIT_ext_ClockPinCtl(unsigned char bEnable)
{
	if(bEnable>2)
	{
		ASSERT(0)
	}

	if(bEnable==1)
	{
		Delayms(10);
	}
	else if(bEnable==0)
	{	

	}
}


void AIT_ext_Set_EMIMode(eEMI_MODE mode)
{

	switch(mode)
	{
		case EMIMODE_BYPASS:
		
			break;
		
		case EMIMODE_ACTIVE_PLL:
		
		
			break;
			
		case EMIMODE_ACTIVE_NO_PLL:
		
		
			break;
	
	}
}


void EINT_Set_Polarity(kal_uint8 eintno, kal_bool ACT_Polarity)
{
//Set interrupt polarity
}

void EINT_UnMask(kal_uint8 eintno)
{
//External interrupt enable

}

void EINT_Mask(kal_uint8 eintno)
{
//External interrupt disable
}


kal_bool ait_is_active_cam(void)
{


}


void sys_IF_ait_delay1us(u_int time)
{
	clk_busy_wait(time);
}


void AIT701_DP_PULLUP_ENABLE(void)
{
	A800_GetGPIO(AIT_GPIO_USB_DP_CTL,AIT_GPIO_PULL_FLOATING);
}


void AIT701_DP_PULLUP_DISABLE(void)	
{
	A800_ClearGPIO(AIT_GPIO_USB_DP_CTL);
}


void AIT701_DP_PULLUP_STANDBY(void)
{
	AIT701_DP_PULLUP_ENABLE();
}
void AIT701_DM_PULLUP_ENABLE(void)
{
	gpio_out(GPIO_OUTPUT_29, GPIO_HIGH_VALUE);
}
	
void AIT701_DM_PULLUP_DISABLE(void)
{
	gpio_out(GPIO_OUTPUT_29, GPIO_LOW_VALUE);
}


void AIT701_DM_PULLUP_STANDBY(void)
{
	AIT701_DM_PULLUP_DISABLE();
}


#elif defined(AIT701_DEMO_BOARD)
void AIT_ext_Create_Mutex(void){}
unsigned char AIT_ext_Take_Semaphore(unsigned short mode){return 0;}
void AIT_ext_Give_Semaphore(void){}
//AIT Related PIN control
void AIT_ext_ResetPinCtl(void)
{
	/* Reset AIT Chip, High Low High */
	at91_pio_open(&PIO_DESC, (1<<21),PIO_OUTPUT);
	at91_pio_write(&PIO_DESC, (1<<21),PIO_SET_OUT);
	sys_IF_ait_delay1ms(1);

	at91_pio_write(&PIO_DESC, (1<<21),PIO_CLEAR_OUT);
	sys_IF_ait_delay1ms(1);

	at91_pio_write(&PIO_DESC, (1<<21),PIO_SET_OUT);
	sys_IF_ait_delay1ms(1);
}
void AIT_ext_BypassPinCtl(unsigned char bEnable)
{
	at91_pio_open(&PIO_DESC, (1<<20),PIO_OUTPUT);
	if(bEnable)
		at91_pio_write(&PIO_DESC, (1<<20),PIO_SET_OUT);	// Bypass mode , Bypass pin is High
	else
		at91_pio_write(&PIO_DESC, (1<<20),PIO_CLEAR_OUT);	// Active mode , Bypass pin is low

}
void AIT_ext_ClockPinCtl(unsigned char bEnable){}
void AIT_ext_Set_EMIMode(eEMI_MODE mode){}
void lcd_busy_waiting(void){}
void EINT_Set_Polarity(kal_uint8 eintno, kal_bool ACT_Polarity){}
void EINT_UnMask(kal_uint8 eintno){}
void EINT_Mask(kal_uint8 eintno){}
kal_bool ait_is_active_cam(void){return 1;}
void sys_IF_ait_delay1us(u_int time)
{
	volatile u_int i;
	volatile u_int j;

	for ( j = 0; j < time; j++ )
	{
		for ( i = 0; i < /*4 **/ 7; i++ )
		 ;
	}
}
void AIT701_DP_PULLUP_ENABLE(void){return;}
void AIT701_DP_PULLUP_DISABLE(void)	{return;}
void AIT701_DP_PULLUP_STANDBY(void){return;}
void AIT701_DM_PULLUP_ENABLE(void){return;}
void AIT701_DM_PULLUP_DISABLE(void){return;}
void AIT701_DM_PULLUP_STANDBY(void){return;}
#else    //add by yangdecai 09-25   

#define Delayms(t) clk_busy_wait(1000*t)
static rex_crit_sect_type dsp_crit_sect = {0};
const unsigned char DCAM_EINT_NO = 0;

unsigned char AIT_ext_Take_Semaphore(unsigned short mode){return 0;}

void AIT_ext_Give_Semaphore(void)
{
	/*if( kal_query_systemInit() || INT_QueryExceptionStatus() ||kal_if_hisr())
	    	return;
	{
		if(dsp_lock.dsp_sem != NULL)
		         	kal_give_sem(dsp_lock.dsp_sem);
	}*/
	rex_leave_crit_sect(&dsp_crit_sect);
}

//AIT Related PIN control
void AIT_ext_ResetPinCtl(void)
{
	gpio_tlmm_config(AIT701_RESET);

	gpio_out(AIT701_RESET,GPIO_HIGH_VALUE);
	Delayms(5);
	gpio_out(AIT701_RESET,GPIO_LOW_VALUE);
	Delayms(10);
	gpio_out(AIT701_RESET,GPIO_HIGH_VALUE);
	Delayms(5);
}

void AIT_ext_BypassPinCtl(unsigned char bEnable)
{
	if(bEnable>1)
	{
		ASSERT(0)
	}

	gpio_out(AIT701_BYPASS,bEnable);
}





void sys_IF_ait_delay1us(u_int time)
{
	volatile u_int i;
	volatile u_int j;

	for ( j = 0; j < time; j++ )
	{
		for ( i = 0; i < /*4 **/ 16; i++ )
		 ;
	}
}
void EINT_UnMask(kal_uint8 eintno){}
void EINT_Mask(kal_uint8 eintno){}
kal_bool ait_is_active_cam(void){return 1;}
void AIT_ext_ClockPinCtl(unsigned char bEnable){}
void lcd_busy_waiting(void){}
void EINT_Set_Polarity(kal_uint8 eintno, kal_bool ACT_Polarity){}

void AIT701_DP_PULLUP_ENABLE(void){return;}
void AIT701_DP_PULLUP_DISABLE(void)	{return;}
void AIT701_DP_PULLUP_STANDBY(void){return;}
void AIT701_DM_PULLUP_ENABLE(void){return;}
void AIT701_DM_PULLUP_DISABLE(void){return;}
void AIT701_DM_PULLUP_STANDBY(void){return;}
void AIT_ext_Set_EMIMode(eEMI_MODE mode)
{

	switch(mode)
	{
		case EMIMODE_BYPASS:
		
			break;
		
		case EMIMODE_ACTIVE_PLL:
		
		
			break;
			
		case EMIMODE_ACTIVE_NO_PLL:
		
		
			break;
	
	}
}

#endif

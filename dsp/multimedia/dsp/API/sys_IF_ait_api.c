/**
 * @file sys_IF_ait_API.c
 * @brief Copyright (C) 2002-2008 Alpha Imaging Technology Corp.
 * @n AIT70x Series Host API.
 * @n The purpose of AIT70x Host API is for Host CPU to control and communicate AIT701
 *      chip via direct/indirect parallel interface.
 * @note N/A
 * @bug N/A
 */
#include "sys_IF_ait_api.h"
#include "A8_panel.h"
#include "a800_camera.h"
#include "a800_usb.h"
#include "cam_IF_ait_api.h"
//#include	"AIT701_FW.h"
#include "AIT_Interface.h"
#if defined(__AIT701_SUPPORT__)
#include "kal_release.h"      	/* Basic data type */
#include "kal_trace.h"
#include "emi_hw.h"
#include "stack_config.h"
#include"gpio_sw.h"
//#include "custom_drv_settings.h"
#endif

extern kal_uint32 LCD_GetCurrentTiming(void);

extern u_char AIT701_camera_fw[0x3000];

static AIT_STATUS AIT_Status = AIT_STATUS_DUMMY;
static  bool AITBypassMode = TRUE;
bool gAITActiveMode = FALSE;
#if 1	//added by ming @20090413 for protect clock en/dis
bool	gbUsbEn = FALSE;
bool	gbSdEn = FALSE;
bool	gbCamEn = FALSE;
extern	bool gbAitUsbEn,gbAitSdEn,gbAitCamEn;
#endif
#ifdef SENSOR_VIF
extern t_sensor_manager* gsSensorUsing;
extern t_sensor_manager* sensor_manager[];



//Vin@20091210:Twin Sensor & TV
#ifdef AIT_ATV_SUPPORT
extern t_sensor_manager* ait_tv_manager[];
#endif
extern sTVSensorPresent ait_tv_Sensor_Present;

#endif


/**
 @fn		void sys_IF_ait_delay1ms(u_int time)
 @brief	Delay function for AIT chip
 @param	time - The base for the delay function.
 @return	void
 @note	.
 @bug	N/A.
*/ 
void sys_IF_ait_delay1ms(u_int time)
{
	sys_IF_ait_delay1us(1000*time);
}

/**
 @fn		void sys_IF_ait_delay10ms(u_int time)
 @brief	Delay function for AIT chip
 @param	time - The base for the delay function.
 @return	void
 @note	.
 @bug	N/A.
*/ 
void sys_IF_ait_delay10ms(u_int time)
{
	sys_IF_ait_delay1ms(10*time);


}

/**
 @fn		void sys_IF_ait_set_input_clock(u_char on)
 @brief	Enable the clock for AIT chip
 @param	on - A8_ON: Turn on the clock for AITchip, A8_OFF: Turn off the clock for AIT chip.
 @return	void
 @note	.
 @bug	N/A.
*/ 
void sys_IF_ait_set_input_clock(u_char ait_mode, u_char on)
{
	if(A8_ON == on){
		if((FALSE == gbCamEn) && (FALSE==gbUsbEn) && (FALSE==gbSdEn) ){	
			switch(ait_mode)
		{
				case	A8_CAM:
				case	A8_SD:
				case	A8_USB:	
					break;
				}
		}
		switch(ait_mode)
		{
			case	A8_CAM:
				gbCamEn = TRUE;
				break;
			case	A8_SD:
				gbSdEn = TRUE;
				break;
			case	A8_USB:
				gbUsbEn = TRUE;
				break;
		}
	}else {
		switch(ait_mode)
		{
			case	A8_CAM:
				gbCamEn = FALSE;
				break;
			case	A8_SD:
				gbSdEn = FALSE;
				break;
			case	A8_USB:
				gbUsbEn = FALSE;
				break;
			case	A8_ALL:
				gbCamEn = FALSE;	
				gbSdEn = FALSE;		
				gbUsbEn = FALSE;				
				break;
		}	
		if((FALSE == gbCamEn) && (FALSE==gbUsbEn) && (FALSE==gbSdEn)){	
		}		
	}

}

/**
 @fn		void sys_IF_ait_reset_chip (void)
 @brief	Reset the AIT chip
 @note	.
 @bug	N/A.
*/ 
void sys_IF_ait_reset_chip (void)
{
	gbSdEn = FALSE;
	A800_SetPllFreq(A8_ALL, A8_OFF);

	EINT_UnMask(0);

	AIT_ext_ResetPinCtl();

#if defined(AIT701_DEMO_BOARD)
	/* Reset AIT chip and panel, High Low High */


	sys_IF_ait_set_bypass_mode(A8_ON);

	LCD_INIT();
	ScreenTest();
	
	sys_IF_ait_set_bypass_mode(A8_OFF);
#endif
}

/**
 @fn		void sys_IF_ait_set_bypass_mode (u_char on)
 @brief	Capture Command. (Not Used!)
 @param	on - AC8_ON: Bypass on the clock for AITchip, A8_OFF: Bypass off the clock for AIT chip.
 @return	s_short
 @note	.
 @bug	N/A.
*/ 
void sys_IF_ait_set_bypass_mode (u_char on)
{
	if (A8_ON == on)	{
		AIT_ext_Set_EMIMode(EMIMODE_BYPASS);
	}else{
		if((FALSE == gbAitCamEn) && (FALSE==gbAitSdEn) && (FALSE==gbAitUsbEn)) 
		{	
			AIT_ext_Set_EMIMode(EMIMODE_ACTIVE_NO_PLL);
				
		}else{
			AIT_ext_Set_EMIMode(EMIMODE_ACTIVE_PLL);
		}
	}

	AIT_ext_BypassPinCtl(on);

	if (A8_ON == on)	{
		AITBypassMode = TRUE;
	}else{
		AITBypassMode = FALSE;	
	}
}

/**
 @fn		void sys_IF_ait_sensor_power(u_char on)
 @brief	
 @note   
 @bug	N/A.
*/ 
void sys_IF_ait_sensor_power(u_char on)
{
	if (A8_ON == on)
	{
		A800_SetGPIO(AIT_GPIO_SENSOR_POWER_CTL);	
		sys_IF_ait_delay1ms(10);
	}
	else
	{
		A800_ClearGPIO(AIT_GPIO_SENSOR_POWER_CTL);		
	}
}

/**
 @fn		void sys_IF_ait_sd_power(u_char on)
 @brief	
 @note   
 @bug	N/A.
*/ 
void sys_IF_ait_sd_power(u_char on)
{
	if (A8_ON == on)
	{
		A800_ClearGPIO(AIT_GPIO_SD_POWER_CTL);	
		sys_IF_ait_delay1ms(10);
	}
	else
	{
		A800_SetGPIO(AIT_GPIO_SD_POWER_CTL);		
	}
}

/**
 @fn		void sys_IF_ait_usb_power(u_char on)
 @brief	
 @note   
 @bug	N/A.
*/ 
void sys_IF_ait_usb_power(u_char on)
{
	if (A8_ON == on)
	{	
		sys_IF_ait_delay1ms(10);
	}
	else
	{	
	}
}

/**
 @fn		void	sys_IF_ait_set_status(u_char CurStatus)
 @brief	Save the AIT current status
 @return	A8_ERROR_MSG
 @note   indicates the current status of the session CLOSED, READY, BUSY, ¡K
 @bug	N/A.
*/ 
void	sys_IF_ait_set_status(u_char CurStatus)
{
	AIT_Status = CurStatus;
}

/**
 @fn		A8_ERROR_MSG	sys_IF_ait_get_status(void)
 @brief	Checking the AIT status
 @return	A8_ERROR_MSG
 @note   indicates the current status of the session CLOSED, READY, BUSY, ¡K
 @bug	N/A.
*/ 
AIT_STATUS	sys_IF_ait_get_status(void)
{
	return AIT_Status;
}

/**
 @fn		u_char	sys_IF_ait_set_status(void)
 @brief	Save the AIT active status
 @return	u_char
 @note   indicates the current status of the session CLOSED, READY, BUSY, ¡K
 @bug	N/A.
*/ 
u_char	sys_IF_ait_get_bypass_status(void )
{
	return AITBypassMode;
}

u_char	sys_IF_ait_search_sensor(void)
{
	u_char retVal = 0;
	u_char i;
	u_short	sensor_id;
//Vin@20091210:Twin Sensor & TV
	ait_tv_Sensor_Present.MasterSensor = NULL;
	ait_tv_Sensor_Present.SlaveSensor = NULL;
	ait_tv_Sensor_Present.TV = NULL;

	MSG_FATAL("sys_IF_ait_search_sensor start!",0,0,0);
	for(i=0;i<10;i++)
	{
		if(sensor_manager[i])
		{
			gsSensorUsing = sensor_manager[i];
			retVal = A8L_I2CInit(gsSensorUsing->i2c_type,gsSensorUsing->i2c_id);

			if(retVal)
				return A8_SYSTEM_ERROR;
			
			gsSensorUsing->sensor_enable(1);
			gsSensorUsing->sensor_get_id(&sensor_id);
			gsSensorUsing->sensor_enable(0);

			MSG_FATAL("Found Sensor ID = %x",sensor_id,0,0);		


			if(sensor_id == gsSensorUsing->sensor_id)
			{
				if(!ait_tv_Sensor_Present.MasterSensor)
				{
					
					ait_tv_Sensor_Present.MasterSensor = sensor_manager[i];
					AIT_Message_P1("Found Sensor ID 2 = %x \r\n",ait_tv_Sensor_Present.MasterSensor->sensor_id);		


				}
				else if(!ait_tv_Sensor_Present.SlaveSensor)
					ait_tv_Sensor_Present.SlaveSensor = sensor_manager[i];
				else
					break;
			}
		}
		else
		{
			break;
		}			
	}
#ifdef AIT_ATV_SUPPORT
	if(ait_tv_manager[0])
		ait_tv_Sensor_Present.TV = ait_tv_manager[0];
#endif

	gsSensorUsing = NULL;
	//MSG_FATAL("sys_IF_ait_boot_init:Mastar sensor_id = %x\r\n",ait_tv_Sensor_Present.MasterSensor->sensor_id,0,0);
	//MSG_FATAL("sys_IF_ait_boot_init:Slave sensor_id = %x\r\n",ait_tv_Sensor_Present.SlaveSensor->sensor_id,0,0);
	//MSG_FATAL("sys_IF_ait_boot_init: TV_id = %x\r\n",ait_tv_Sensor_Present.TV->sensor_id,0,0);
	
	MSG_FATAL("sys_IF_ait_boot_init:sensor_id = 0x%x\r\n",sensor_id,0,0);
	return retVal;
}

/**
 @fn		A8_ERROR_MSG sys_IF_ait_boot_init(void)
 @brief	AIT booting sequence.
 @return	A8_ERROR_MSG
 @note	.
 @bug	N/A.
*/ 

A8_ERROR_MSG sys_IF_ait_boot_init(void)
{
	u_char retVal;
	uint16 firmware_version = 0;
	uint16 chipversion = 0;
	MSG_FATAL("sys_IF_ait_boot_init start!",0,0,0);
	sys_IF_ait_set_input_clock(A8_CAM, A8_ON);

	sys_IF_ait_reset_chip();

	sys_IF_ait_set_bypass_mode(A8_OFF);

	A800_SetPllFreq(A8_CAM, A8_ON);

#if 1	//For CS Timing test
{
	extern void AIT701_reg_mem_test(void);
	extern void sys_IF_ait_set_bypass_mode (u_char on);
#if 1
	int i;
	for (i = 0; i < 10; i++)
	{  

#if defined(__QSC_TARGET__)

        //unsigned long emisetting;
//Fine Tune CS timing   
        //out_dword(HWIO_ADDR(LCD_CFG0), emisetting);
        //out_dword(HWIO_ADDR(LCD_CFG1), 0xc9000000);
#endif        
    	SetA8RegB(0x6901, 0x46);   
	}
#endif
	AIT701_reg_mem_test();
   	sys_IF_ait_set_bypass_mode(A8_OFF);
}	
#endif

	/* Download AIT FW, and then set AIT into SW_Bypass mode, for power saving. */
	retVal = A800_DownloadFirmware((u_char *)AIT701_camera_fw, sizeof(AIT701_camera_fw));
	if (retVal != A8_NO_ERROR)
	{
		A800_PowerSavingOn();
		A800_SetPllFreq(A8_ALL, A8_OFF);
		
		sys_IF_ait_set_bypass_mode(A8_ON);
		sys_IF_ait_set_input_clock(A8_ALL, A8_OFF);
		
		sys_IF_ait_set_status( AIT_STATUS_DUMMY);
		AIT_Message_P0("A800_DownloadFirmware is failed !!!\n");
		
		return A8_DOWNLOAD_FW_ERROR;
	}

	firmware_version = A800_GetFirmwareMode();
	chipversion = A8L_GetA8FirmwareVersion();

	MSG_FATAL("firmware_version :0x%x,chipversion : 0x%x",firmware_version,chipversion,0);
	//Vin@20091210:Twin Sensor & TV
	sys_IF_ait_search_sensor();

	A800_PreInit_Sensor();

	
	retVal = A800_Camera_Bypass_Mode();

	AIT701_DP_PULLUP_STANDBY();
	AIT701_DM_PULLUP_STANDBY();

	A800_PowerSavingOn();
	A800_SetPllFreq(A8_ALL, A8_OFF);

	sys_IF_ait_set_bypass_mode(A8_ON);
	
	sys_IF_ait_set_input_clock(A8_ALL, A8_OFF);

	if (retVal != A8_NO_ERROR)
	{
		sys_IF_ait_set_status(AIT_STATUS_DUMMY);
		
		return A8_SYSTEM_ERROR;
	}
	else
	{
		sys_IF_ait_set_status(AIT_STATUS_CAM_CLOSED);

		return A8_NO_ERROR;
	}
}


A8_ERROR_MSG sys_IF_ait_enter_sleep(void)
{
	u_char retVal;

	sys_IF_ait_set_input_clock(A8_CAM, A8_ON);
#if 0

	if(0 /*(gAitUsbPcCamEnable*/)
	{
		A800_SensorPowerOn(A8_OFF);
		gAitUsbPcCamEnable = 0;
	}	
#endif		
	SetA8RegB(0x6902, (GetA8RegB(0x6902) & (~0x80)));
	
	retVal = A800_ExitUSBMode();
	sys_IF_ait_usb_power(A8_OFF);	
	AIT_Message_P1("A800_ExitUSBMode=0x%d\r\n", retVal);
	
	if(A800_IsResetStatus() || (sys_IF_ait_get_status() == AIT_STATUS_DUMMY))	{
		AIT_Message_P2("is_reset=0x%x,ait_status=0x%x\r\n", A800_IsResetStatus(), sys_IF_ait_get_status());
		SetA8RegB(0x6903, 0xFF);
		SetA8RegB(0x6904, 0x1C);
		SetA8RegW(0x690A, 0xFFFF);
		SetA8RegW(0x690A, 0x0000);
		SetA8RegB(0x6511, 0x04 );
		SetA8RegB(0x6511, 0x00);
		retVal = A8_SYSTEM_ERROR;
	}

	if (retVal == A8_NO_ERROR)	// AIT`s FW is not ready!!
	{
		retVal = A800_Camera_Bypass_Mode();
		AIT_Message_P1("cam_IF_ait_close_AIT, Bypass retVal= %d\n", retVal);
	}
	
	A800_PowerSavingOn();
	
	AIT701_DP_PULLUP_STANDBY();

	//Antonio@20100421, fix Hynix sensor power consumption issue, PDCLK must stop at low level.
	 SetA8RegB(0x7120,GetA8RegB(0x7120)&(~0x01));
	 sys_IF_ait_delay1ms(1);
	 SetA8RegB(0x6903,GetA8RegB(0x6903)|0x80);

	A800_SetPllFreq(A8_ALL, A8_OFF);

	sys_IF_ait_set_input_clock(A8_ALL, A8_OFF);

	sys_IF_ait_set_status(AIT_STATUS_DUMMY);
	return retVal;
}


void	sys_A800_LCDInit(void)
{
	extern const unsigned short gA8MainLCDWidth;
	extern const unsigned short gA8MainLCDHeight;
#ifndef A8_DISABLE_SUB_LCD_FUNCTION		
	extern const unsigned short gA8SubLCDWidth;
	extern const unsigned short gA8SubLCDHeight;
	extern sPanel_Ctrl_Attrib gSubPanelControlAttrib;
#endif	
	extern sPanel_Ctrl_Attrib gMainPanelControlAttrib;
	gMainPanelControlAttrib.PIXEL_CLOCK_DIV = 0x03;
	gMainPanelControlAttrib.ADDRESS_INDEX = 0x0021;
	gMainPanelControlAttrib.ADDRESS_DATA = 0x0000; //080401 0x0002
	gMainPanelControlAttrib.DATA_INDEX = 0x0022;
	gMainPanelControlAttrib.LCD_BusCtl.BusWidth = 8;
	gMainPanelControlAttrib.LCD_BusCtl.BusType = TYPE_80;
	gMainPanelControlAttrib.LCD_BusCtl.Burst = NON_BURST_MODE;

	gMainPanelControlAttrib.LCD_BusCtl.Width = gA8MainLCDWidth;
	gMainPanelControlAttrib.LCD_BusCtl.Height= gA8MainLCDHeight;

	gMainPanelControlAttrib.PixelStart.x =0;
	gMainPanelControlAttrib.PixelStart.y = 0;
	gMainPanelControlAttrib.PixelOffset.x = 1;
	gMainPanelControlAttrib.PixelOffset.y = gA8MainLCDWidth;


#ifndef A8_DISABLE_SUB_LCD_FUNCTION	
	gSubPanelControlAttrib.PIXEL_CLOCK_DIV = 0x04;
	gSubPanelControlAttrib.ADDRESS_INDEX = 0x0021;
	gSubPanelControlAttrib.ADDRESS_DATA = 0x0000; //080401 0x0002
	gSubPanelControlAttrib.DATA_INDEX = 0x0022;
	gSubPanelControlAttrib.LCD_BusCtl.BusWidth = 8;
	gSubPanelControlAttrib.LCD_BusCtl.BusType = TYPE_80;
	gSubPanelControlAttrib.LCD_BusCtl.Burst = NON_BURST_MODE;


	gSubPanelControlAttrib.LCD_BusCtl.Width =	gA8SubLCDWidth;
	gSubPanelControlAttrib.LCD_BusCtl.Height= gA8SubLCDHeight;

	gSubPanelControlAttrib.PixelStart.x =0;
	gSubPanelControlAttrib.PixelStart.y = 0;
	gSubPanelControlAttrib.PixelOffset.x = 1;
	gSubPanelControlAttrib.PixelOffset.y = gA8SubLCDWidth;
#endif
}

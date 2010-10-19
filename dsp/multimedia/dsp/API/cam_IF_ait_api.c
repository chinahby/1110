/**
 * @file cam_IF_ait_API.c
 * @brief Copyright (C) 2002-2008 Alpha Imaging Technology Corp.
 * @n AIT70x Series Host API.
 * @n The purpose of AIT70x Host API is for Host CPU to control and communicate AIT701
 *      chip via direct/indirect parallel interface.
 * @note N/A
 * @bug N/A
 */
#include	"sys_IF_ait_api.h"
#include	"cam_IF_ait_api.h"
//Vin@20091210:Twin Sensor & TV
#include  "AIT_Interface.h"
#include	"a8_sensor.h"
#include "A8_vif.h"
#include "a800_camera.h"

static bool	gAITIsPreview = FALSE;

static AIT_CAMERA_CONFIG		AIT_Camera_config	= {0xff};
static AIT_CAMERA_CONFIG		AIT_Current_Camera_config	= {0xff};
static AIT_PREVIEW_CONFIG		AIT_Preview_config	= {0xff};

static AIT_CAPTURE_CONFIG	AIT_Capture_config	= {0xff};

extern u_char AIT701_camera_fw[0x3000];

extern A800InfoDT  A800Data;
extern A8ScaleDT glVideoScale;
extern	u_char gAitCaptureStatus;
extern	u_char	gAitPreviewStatus;
extern	t_sensor_manager* gsSensorUsing;

u_short	gsJpegWidth,gsJpegHeight;

/* ****************************************************************************************** 
Description: 
	This Function should be called during handset power on sequence.
	Mostly, in handset power process, BB has to init some hw devices. (LCD, AIT, melody_IC....)
	BB also has to init AIT chip (download AIT`S FW) and then set AIT into power saving mode.
****************************************************************************************** */
/**
 @fn		A8_ERROR_MSG cam_IF_ait_open_AIT(void)
 @brief	AIT active sequence.
 @return	A8_ERROR_MSG
 @note    When BB wants to use AIT chip, BB has to setup AIT into "active mode".
	After AIT enter "active mode", next into preview or decode-JPEG mode process.
 @bug	N/A.
*/
A8_ERROR_MSG cam_IF_ait_open_AIT(void)
{
extern void	sys_A800_LCDInit(void);
	u_char retVal;	
	sys_IF_ait_set_input_clock(A8_CAM,A8_ON);	
	if(A800_IsResetStatus() || (sys_IF_ait_get_status() == AIT_STATUS_DUMMY))
	{
		SetA8RegB(0x6903, 0xFF);
		SetA8RegB(0x6904, 0x1C);
		SetA8RegW(0x690A, 0xFFFF);
		SetA8RegW(0x690A, 0x0000);
		SetA8RegB(0x6511, 0x04 );
		SetA8RegB(0x6511, 0x00);

		sys_IF_ait_set_status(AIT_STATUS_DUMMY);
	}
	
	AIT_Message_P1("cam_IF_ait_open_AIT, Status=%d! \n", sys_IF_ait_get_status());
	if((sys_IF_ait_get_status() == AIT_STATUS_DUMMY) || 
		(sys_IF_ait_get_status() >= AIT_STATUS_USB_CLOSED) || 
		(sys_IF_ait_get_status() == AIT_STATUS_CAM_CLOSED))
	{
		AIT_Message_P0("cam_IF_ait_open_AIT need to download FW \n\r");
		
		sys_IF_ait_reset_chip();

		A800_SetPllFreq(A8_CAM, A8_ON);
		AIT701_DP_PULLUP_STANDBY();	
		retVal = A800_DownloadFirmware((u_char *)AIT701_camera_fw, sizeof(AIT701_camera_fw));
		if(retVal != 0)
		{
			sys_IF_ait_reset_chip();

			A800_SetPllFreq(A8_CAM, A8_ON);
			AIT701_DP_PULLUP_STANDBY();	
			retVal = A800_DownloadFirmware((u_char *)AIT701_camera_fw, sizeof(AIT701_camera_fw));
			if (retVal != A8_NO_ERROR)	// AIT`s FW is not ready!!
			{
				sys_IF_ait_set_status(AIT_STATUS_DUMMY);
				AIT_Message_P0("A800_DownloadFirmware is failed !!!\n");
				return	A8_DOWNLOAD_FW_ERROR;
			}
		}
	}
	else
	{
		retVal = A800_SetPllFreq(A8_CAM, A8_ON);

		if (retVal != A8_NO_ERROR)
		{
			sys_IF_ait_set_status(AIT_STATUS_DUMMY);
			AIT_Message_P0("A800_SetPllFreq(A8_ON) is failed !!!\n");
			return	A8_SYSTEM_ERROR;
		}
	}

	//Active the AIT chip
	retVal = A800_Camera_Active_Mode();
	
	if (retVal != A8_NO_ERROR)
	{
		AIT_Message_P0("A800_Camera_Active_Mode is failed\n");
		sys_IF_ait_set_status(AIT_STATUS_DUMMY);
		return	A8_SYSTEM_ERROR;
	}

	//080430 for more stable HW block work
	SetA8RegB(0x690A, 0x1E); // reset jpeg, scaler, image, graphic engine
	sys_IF_ait_delay1ms(1);
	SetA8RegB(0x690A, 0x00);

	SetA8RegB(0x6511, GetA8RegB(0x6511) | 0x04); // reset PIP and jpeg line buffer
	sys_IF_ait_delay1ms(1);
	SetA8RegB(0x6511, GetA8RegB(0x6511) & (~0x04));
	sys_IF_ait_delay1ms(1);

	sys_A800_LCDInit();
	A800_InitDefine();
	A800_LCDInit();
	A800_GraphicInit();

	sys_IF_ait_set_status(AIT_STATUS_CAM_READY);
	
	return A8_NO_ERROR;
}

/**
 @fn		A8_ERROR_MSG cam_IF_ait_close_AIT(void)
 @brief	AIT power saving sequence.
 @return	A8_ERROR_MSG
 @note    	When BB wants to close AIT chip, BB has to setup AIT into "bypass mode".
	After AIT enter "bypass mode", BB can directly access LCD with AIT in power saving.
 @bug	N/A.
*/ 
A8_ERROR_MSG cam_IF_ait_close_AIT(void)
{
	u_char retVal = A8_NO_ERROR;

	sys_IF_ait_set_input_clock(A8_CAM, A8_ON);
	
	
	if(A800_IsResetStatus() || (sys_IF_ait_get_status() == AIT_STATUS_DUMMY))
	{

		retVal = A800_Camera_Bypass_Mode();

		SetA8RegB(0x6903, 0xFF);
		SetA8RegB(0x6904, 0x1C);
		SetA8RegW(0x690A, 0xFFFF);
		SetA8RegW(0x690A, 0x0000);
		SetA8RegB(0x6511, 0x04 );
		SetA8RegB(0x6511, 0x00);
		
		retVal = A8_SYSTEM_ERROR;
	}

	if (retVal != A8_NO_ERROR)	// AIT`s FW is not ready!!
	{
		A800_PowerSavingOn();
		A800_SetPllFreq(A8_ALL, A8_OFF);
	}
	else
	{
		retVal = A800_Camera_Bypass_Mode();
		A800_PowerSavingOn();
		A800_SetPllFreq(A8_ALL, A8_OFF);
		AIT_Message_P1("cam_IF_ait_close_AIT, Bypass retVal= %d\n", retVal);
	}
	
	//sys_IF_ait_set_bypass_mode(A8_ON);

	//sys_IF_ait_set_input_clock(A8_ALL, A8_OFF);

	if (retVal != A8_NO_ERROR)
	{
		sys_IF_ait_set_status(AIT_STATUS_DUMMY);
	}
	else
	{
		sys_IF_ait_set_status(AIT_STATUS_CAM_CLOSED);
	}

	
	sys_IF_ait_set_input_clock(A8_ALL, A8_OFF);	
	
	return	retVal;
}

/**
 @fn		A8_ERROR_MSG	cam_IF_ait_open_camera (void)
 @brief	AIT camera active
 @return	A8_ERROR_MSG
 @note 	Power On Sensor device for Preview process. Customer can base project`s sensor implemnet this function.
	Of course, after this step. BB can configure sensor`s registers. Else, sensor configure will be fail or timeout.
 @bug	N/A.
*/
A8_ERROR_MSG	cam_IF_ait_open_camera (void)
{
	extern u_char A800_PreInit_Sensor(void);
	extern u_char	A800_Init_Sensor(void);
	u_char retVal;
//	u_short pid_Val_MSB, pid_Val_LSB,sensor_id;

#ifdef AIT_ATV_SUPPORT
	extern u_char g_ATV_Flag;
#endif

#ifdef SENSOR_VIF
	if(!gsSensorUsing)
		return A8_SYSTEM_ERROR;
	AIT_Message_P1("sensor_id = 0x%x\r\n",gsSensorUsing -> sensor_id);
#ifdef AIT_ATV_SUPPORT
	if(g_ATV_Flag==0)
#endif		
	{
		A8L_I2CInit(gsSensorUsing->i2c_type,gsSensorUsing->i2c_id);
	}	
	A800_PreInit_Sensor();	
#endif	

	retVal = A800_Init_Sensor();
	if (retVal != A8_NO_ERROR)
	{
		sys_IF_ait_set_status(AIT_STATUS_DUMMY);
		AIT_Message_P1 ("cam_IF_ait_open_camera is failed ret= %d !!!\n",retVal);
		return A8_SYSTEM_ERROR;
	}

	A8L_CheckFrameStart();
	
/*
	AIT_Current_Camera_config.zoom_step = 0xff;
	AIT_Current_Camera_config.frame_rate = 0xff;
	AIT_Current_Camera_config.brightness = 0xff;
	AIT_Current_Camera_config.night_mode = 0xff;
	AIT_Current_Camera_config.AWB_mode = 0xff;
	AIT_Current_Camera_config.effect_mode = 0xff;
	AIT_Current_Camera_config.deband_mode = 0xff;
	AIT_Current_Camera_config.rotate_mode = 0xff;
*/
	sys_IF_ait_set_status(AIT_STATUS_CAM_READY);
	AIT_Message_P0 ("cam_IF_ait_open_camera, Sensor Init OK ! \n");

	return A8_NO_ERROR;
}

/**
 @fn		A8_ERROR_MSG	cam_IF_ait_close_camera (void)
 @brief	AIT camera power saving
 @return	A8_ERROR_MSG
 @note 		Power Off Sensor device for Quit-Preview process. Customer can base project`s sensor implemnet this function.
	Of course, after this step. BB can not configure sensor`s registers.
 @bug	N/A.
*/ 
A8_ERROR_MSG	cam_IF_ait_close_camera (void)
{
	extern u_char	A800_SensorPowerOn(u_char on);
	A800_SensorPowerOn(A8_OFF);

	sys_IF_ait_set_status(AIT_STATUS_CAM_READY);

	return A8_NO_ERROR;
}

sPresentSensorID gPresentSensorID = {0,0,0};
A8_ERROR_MSG	cam_IF_ait_camera_id_detect (unsigned camera_id, unsigned short *sensorID)
{
//	extern sTVSensorPresent ait_tv_Sensor_Present;
//	extern t_sensor_manager* sensor_manager;
	extern t_sensor_manager* sensor_manager[] ;
	extern t_sensor_manager* gsSensorUsing;
	extern t_sensor_manager* ait_tv_manager[];
	
	u_char retVal;
	int i;
	u_short detectedSensorID = 0;

	switch(camera_id)
	{
		default:
		case 0://Main Sensor ID
		if(gPresentSensorID.MainSensorID != 0)
		{

			*sensorID = gPresentSensorID.MainSensorID;
			return A8_NO_ERROR;
		}
		break;
		case 1: //Sub sensor ID
			if(gPresentSensorID.SubSensorID != 0)
			{

				*sensorID = gPresentSensorID.SubSensorID;
				return A8_NO_ERROR;
			}else if(gPresentSensorID.MainSensorID != 0)
			{
					*sensorID = gPresentSensorID.SubSensorID;
				return A8_NO_ERROR;
			}
			// esle redo real sensor detection
			break;

		case 2: //TV module ID
			if(ait_tv_manager[0])
			{
				gPresentSensorID.tvModuleID = ait_tv_manager[0]->sensor_id;

				*sensorID = gPresentSensorID.tvModuleID;
				return A8_NO_ERROR;
			}

			break;
	}
		

	sys_IF_ait_set_input_clock(A8_CAM,A8_ON);	
	if(A800_IsResetStatus() || (sys_IF_ait_get_status() == AIT_STATUS_DUMMY))
	{
		SetA8RegB(0x6903, 0xFF);
		SetA8RegB(0x6904, 0x1C);
		SetA8RegW(0x690A, 0xFFFF);
		SetA8RegW(0x690A, 0x0000);
		SetA8RegB(0x6511, 0x04 );
		SetA8RegB(0x6511, 0x00);

		sys_IF_ait_set_status(AIT_STATUS_DUMMY);
	}
	
	AIT_Message_P1("cam_IF_ait_open_AIT, Status=%d! \n", sys_IF_ait_get_status());
	if((sys_IF_ait_get_status() == AIT_STATUS_DUMMY) || 
		(sys_IF_ait_get_status() >= AIT_STATUS_USB_CLOSED) || 
		(sys_IF_ait_get_status() == AIT_STATUS_CAM_CLOSED))
	{
		AIT_Message_P0("cam_IF_ait_open_AIT need to download FW \n\r");
		
		sys_IF_ait_reset_chip();

		A800_SetPllFreq(A8_CAM, A8_ON);
		AIT701_DP_PULLUP_STANDBY();
		retVal = A800_DownloadFirmware((u_char *)AIT701_camera_fw, sizeof(AIT701_camera_fw));
		if(retVal != 0)
		{
			sys_IF_ait_reset_chip();

			A800_SetPllFreq(A8_CAM, A8_ON);
			AIT701_DP_PULLUP_STANDBY();
			retVal = A800_DownloadFirmware((u_char *)AIT701_camera_fw, sizeof(AIT701_camera_fw));
			if (retVal != A8_NO_ERROR)	// AIT`s FW is not ready!!
			{
				sys_IF_ait_set_status(AIT_STATUS_DUMMY);
				AIT_Message_P0("A800_DownloadFirmware is failed !!!\n");
				return	A8_DOWNLOAD_FW_ERROR;
			}
		}
	}
	else
	{
		retVal = A800_SetPllFreq(A8_CAM, A8_ON);

		if (retVal != A8_NO_ERROR)
		{
			sys_IF_ait_set_status(AIT_STATUS_DUMMY);
			AIT_Message_P0("A800_SetPllFreq(A8_ON) is failed !!!\n");
			return	A8_SYSTEM_ERROR;
		}
	}
	//Active the AIT chip
	retVal = A800_Camera_Active_Mode();
	
	if (retVal != A8_NO_ERROR)
	{
		AIT_Message_P0("A800_Camera_Active_Mode is failed\n");
		sys_IF_ait_set_status(AIT_STATUS_DUMMY);
		return	A8_SYSTEM_ERROR;
	}


	for(i=0;i<AIT_MAX_SENSOR_SUPPORT;i++)
	{
		if(sensor_manager[i])
		{
			gsSensorUsing = sensor_manager[i];
			retVal = A8L_I2CInit(gsSensorUsing->i2c_type,gsSensorUsing->i2c_id);

			if(retVal)
				return A8_SYSTEM_ERROR;
			
			gsSensorUsing->sensor_enable(1);
			gsSensorUsing->sensor_get_id(&detectedSensorID);
			gsSensorUsing->sensor_enable(0);

			AIT_Message_P1("Found Sensor ID = %x \r\n",detectedSensorID);		


			if(detectedSensorID == gsSensorUsing->sensor_id)
			{
				if(!gPresentSensorID.MainSensorID)
				{
					
					gPresentSensorID.MainSensorID = detectedSensorID;
					AIT_Message_P1("Found Sensor ID 2 = %x \r\n",gPresentSensorID.MainSensorID);		


				}
				else if(!gPresentSensorID.SubSensorID)
					gPresentSensorID.SubSensorID = detectedSensorID;
				else
					break;
			}
		}else{
			break;
		}			
	}


	gsSensorUsing = NULL;
	
	AIT_Message_P1("sys_IF_ait_boot_init:Mastar sensor_id = %x\r\n",gPresentSensorID.MainSensorID);
	AIT_Message_P1("sys_IF_ait_boot_init:Slave sensor_id = %x\r\n",gPresentSensorID.SubSensorID);
	AIT_Message_P1("sys_IF_ait_boot_init: TV_id = %x\r\n",gPresentSensorID.tvModuleID);
	
	
	switch(camera_id)
	{
		default:
		case 0://Main Sensor ID
		if(gPresentSensorID.MainSensorID != 0)
		{

			*sensorID = gPresentSensorID.MainSensorID;
			return A8_NO_ERROR;
		}
		break;
		case 1: //Sub sensor ID
			if(gPresentSensorID.SubSensorID != 0)
			{

				*sensorID = gPresentSensorID.SubSensorID;
				return A8_NO_ERROR;
			}else if(gPresentSensorID.MainSensorID != 0)
			{
					*sensorID = gPresentSensorID.SubSensorID;
				return A8_NO_ERROR;
			}
			// esle redo real sensor detection
			break;

		case 2: //TV module ID
			if(ait_tv_manager[0])
			{
				gPresentSensorID.tvModuleID = ait_tv_manager[0]->sensor_id;

				*sensorID = gPresentSensorID.tvModuleID;
				return A8_NO_ERROR;
			}

			break;
	}
	
	if((gPresentSensorID.MainSensorID==NULL)&&(gPresentSensorID.SubSensorID == NULL))
		return MMP_NO_SENSOR;
	else
		return MMP_SUCCESS;
}


A8_ERROR_MSG	cam_IF_ait_camera_id_serch (unsigned short sensorID)
{
//	extern sTVSensorPresent ait_tv_Sensor_Present;
//	extern t_sensor_manager* sensor_manager;
	extern t_sensor_manager* sensor_manager[] ;
	extern t_sensor_manager* gsSensorUsing;
//	extern t_sensor_manager* ait_tv_manager[];
	
	//u_char retVal;
	int i;

	for(i=0;i<AIT_MAX_SENSOR_SUPPORT;i++)
	{
		if(sensor_manager[i]->sensor_id==sensorID)
		{
			return MMP_SUCCESS;
		}
	}		

	return MMP_FAILED;
}
/**
 @fn		A8_ERROR_MSG	cam_IF_ait_camera_config (eCAM_CTRL_MODE mode, u_char para)
 @brief	Setting the camera effect, zoom, ... etc
 @param	mode - 	CAM_CTRL_ZOOM, CAM_CTRL_BRIGHTNESS, CAM_CTRL_NIGHT_MODE,
                          CAM_CTRL_WB, CAM_CTRL_EFFECT, CAM_CTRL_DEBAND_MODE, CAM_CTRL_ROTATE, CAM_CTRL_FRAMERATE
 @param	para - number of  0, 1, 2, 3,...
 @return	A8_ERROR_MSG
 @note   .
 @bug	N/A.
*/ 
A8_ERROR_MSG	cam_IF_ait_camera_config (CAM_CTRL_MODE mode, u_char para)
{
	u_char retVal = A8_NO_ERROR;

	AIT_Message_P2("### camera_config(%d,%d)\r\n", mode, para);
	
	switch(mode)
	{
	case CAM_CTRL_ZOOM:
		if (para <= 24)//&&para>=0)
			AIT_Camera_config.zoom_step = para;
		else
			AIT_Camera_config.zoom_step = 0;
		break;
		
	case CAM_CTRL_FRAMERATE:
		if ((para == 0) || (para == 15))
			AIT_Camera_config.frame_rate = para;
		break;
	case CAM_CTRL_BRIGHTNESS:
	case CAM_CTRL_EXPOSURE:
		if (para <= 10)
			AIT_Camera_config.brightness = para;
		break;
	case CAM_CTRL_NIGHT_MODE:
		if(1 == para)
			AIT_Camera_config.night_mode = 1;
		else
			AIT_Camera_config.night_mode = 0;			
		break;
	case CAM_CTRL_WB:
		if (para <= 5)
			AIT_Camera_config.AWB_mode = para;
		break;
	case CAM_CTRL_EFFECT:
		if (para <= 8)
			AIT_Camera_config.effect_mode = para;
		break;
	case CAM_CTRL_DEBAND_MODE:
		if ((para == 0) || (para == 1))
			AIT_Camera_config.deband_mode = para;
		break;
	case CAM_CTRL_ROTATE:
		if (para <= 3)
			AIT_Camera_config.rotate_mode = para;
		break;
	case CAM_CTRL_MATRIX:
//			retVal = A800_SensorMatrixMode();	
		break;
	case CAM_CTRL_CENTERWEIGHTED:
//			retVal = A800_SensorCenterWeightedMode();	
		break;
	case CAM_CTRL_SPOT:
//			retVal = A800_SensorSpotMode();	
		break;

	}

	if((TRUE == gAitPreviewStatus)||(CAM_CTRL_MAX == mode)){
		if(AIT_Current_Camera_config.zoom_step != AIT_Camera_config.zoom_step){
			retVal += A800_SetPreviewLinearZoom(AIT_Camera_config.zoom_step);
			AIT_Message_P3("### zoom_step %d => %d,ret = %d,\r\n", AIT_Current_Camera_config.zoom_step, AIT_Camera_config.zoom_step,retVal);

			AIT_Current_Camera_config.zoom_step = AIT_Camera_config.zoom_step;
		}
		if(AIT_Current_Camera_config.frame_rate != AIT_Camera_config.frame_rate){

			retVal += A800_SensorFixedFrameRate(AIT_Camera_config.frame_rate);
			AIT_Message_P3("### frame_rate %d => %d,ret = %d,\r\n", AIT_Current_Camera_config.frame_rate, AIT_Camera_config.frame_rate,retVal);
			
			AIT_Current_Camera_config.frame_rate = AIT_Camera_config.frame_rate;
		}	
		if(AIT_Current_Camera_config.brightness != AIT_Camera_config.brightness){
			retVal += A800_SensorBrightness(AIT_Camera_config.brightness);
			AIT_Message_P3("### brightness %d => %d,ret = %d,\r\n", AIT_Current_Camera_config.brightness, AIT_Camera_config.brightness,retVal);

			AIT_Current_Camera_config.brightness = AIT_Camera_config.brightness;
		}
		if(AIT_Current_Camera_config.night_mode != AIT_Camera_config.night_mode){
			retVal += A800_SensorNightMode(AIT_Camera_config.night_mode);
			AIT_Message_P3("### night_mode %d => %d,ret = %d,\r\n", AIT_Current_Camera_config.night_mode, AIT_Camera_config.night_mode,retVal);
			
			AIT_Current_Camera_config.night_mode = AIT_Camera_config.night_mode;
		}	
		if(AIT_Current_Camera_config.AWB_mode != AIT_Camera_config.AWB_mode){
			retVal += A800_SensorWBMode(AIT_Camera_config.AWB_mode);
			AIT_Message_P3("### AWB_mode %d => %d,ret = %d,\r\n", AIT_Current_Camera_config.AWB_mode, AIT_Camera_config.AWB_mode,retVal);

			AIT_Current_Camera_config.AWB_mode = AIT_Camera_config.AWB_mode;
		}
		if(AIT_Current_Camera_config.effect_mode != AIT_Camera_config.effect_mode){
			retVal += A800_SensorEffect(AIT_Camera_config.effect_mode);
			AIT_Message_P3("### effect_mode %d => %d,ret = %d,\r\n", AIT_Current_Camera_config.effect_mode, AIT_Camera_config.effect_mode,retVal);

			AIT_Current_Camera_config.effect_mode = AIT_Camera_config.effect_mode;
		}	
		if(AIT_Current_Camera_config.deband_mode != AIT_Camera_config.deband_mode){
			retVal += A800_SensorDebandMode(AIT_Camera_config.deband_mode);
			AIT_Message_P3("### deband_mode %d => %d,ret = %d,\r\n", AIT_Current_Camera_config.deband_mode, AIT_Camera_config.deband_mode,retVal);

			AIT_Current_Camera_config.deband_mode = AIT_Camera_config.deband_mode;
		}
		if(AIT_Current_Camera_config.rotate_mode != AIT_Camera_config.rotate_mode){
			retVal += A800_SensorRotation(AIT_Camera_config.rotate_mode);
			AIT_Current_Camera_config.rotate_mode = AIT_Camera_config.rotate_mode;
		}

	}
	if (retVal != A8_NO_ERROR)
	{
		sys_IF_ait_set_status(AIT_STATUS_CAM_READY);	// Only AIT Active. Sensor not ready.
		AIT_Message_P0("cam_IF_ait_camera_config is failed !!!\r\n");
		return A8_CAM_CONFIG_ERROR;
	}
	else
	{
		// Maybe AIT_Status is in preview or just finish capture, Keep original status.
		AIT_Message_P0("cam_IF_ait_camera_config is ok !!!\r\n");		
		return A8_NO_ERROR;
	}
}

/**
 @fn		A8_ERROR_MSG	cam_IF_ait_preview_config (sAIT_PREVIEW_CONFIG previewConfig)
 @brief	Pass the struct of the preview type. transparency Color, preview resolution ...
 @param	previewConfig
 @return	A8_ERROR_MSG
 @note   		Preview mode choose! Basically, we provide 128x96 and 128x160 preview modes on 128x160 LCD.
	Need discuss with Customer.
 @bug	N/A.
*/ 
A8_ERROR_MSG	cam_IF_ait_preview_config (AIT_PREVIEW_CONFIG previewConfig)
{
	u_char retVal;

#ifndef A8_DISABLE_SUB_LCD_FUNCTION	
	if (previewConfig.lcdMode == A8_SUB_LCD)
	{
		retVal = A800_SubLCDPreviewActive();
		sys_IF_ait_delay1ms(2);
		
		A800_UsingLCD2();
		A800_HostInitLCD_P2();
	}
	else
#endif		
	{
		retVal = A800_MainLCDPreviewActive();
		sys_IF_ait_delay1ms(2);
		
		A800_UsingLCD1();
		A800_HostInitLCD();
	}
	
	if (retVal != 0)
	{
		sys_IF_ait_set_status(AIT_STATUS_DUMMY);
		AIT_Message_P0("cam_IF_ait_preview_config:A800_Main/SubLCDPreviewActive is failed !!!\r\n");
		return A8_SYSTEM_ERROR;
	}

	// Base on preview config parameter.
	// 1. transparencyColor for OSD data.
	A800_Preview_MainWindow_Transparency(1, previewConfig.transparencyColor);
	A8L_SetWindowTransparency(A8_OVERLAY_WINDOW, 1, previewConfig.transparencyColor);
	A8L_SetIconTransparency(1, previewConfig.transparencyColor);
	AIT_Preview_config.transparencyColor = previewConfig.transparencyColor;
	// 2. previewMode, need to mapping into A800_SetPreviewMode()
	AIT_Preview_config.previewMode = previewConfig.previewMode;
	AIT_Message_P2("cam_IF_ait_preview_config, transcolor = %d, mode=%x\n", AIT_Preview_config.transparencyColor,AIT_Preview_config.previewMode);

	
	retVal = A800_SetPreviewMode(previewConfig.previewMode);
	

	if (retVal != A8_NO_ERROR)
	{
		sys_IF_ait_set_status(AIT_STATUS_DUMMY);
		AIT_Message_P0("A800_SetPreviewMode is failed !!!\r\n");
		return A8_SYSTEM_ERROR;
	}
	else
	{		
		return A8_NO_ERROR;
	}
}

/**
 @fn		A8_ERROR_MSG	cam_IF_ait_preview_control (bool bPreviewOn)
 @brief	Pass the struct of the preview type. transparency Color, preview resolution ...
 @param	bPreviewOn - 0: Start preview.  1: Stop preview.
 @return	A8_ERROR_MSG
 @note   	Preview enable or disable. BB must call this function after sensor enable!!
 @bug	N/A.
*/ 
A8_ERROR_MSG	cam_IF_ait_preview_control (bool bPreviewOn)
{
	extern const sPanel_Ctrl_Attrib gMainPanelControlAttrib;
	extern void A8L_CheckSensorFrame(u_char mode);
	u_char retVal;
	
	if (bPreviewOn)
	{

		u_char tmp7111;						
		if(CAM_LCD_REFRESH_BYPASS == A800Data.Select->LcdRefreshMode){		
			tmp7111 = GetA8RegB(0x7111);
			SetA8RegB(0x7111,0x0);
			A8L_CheckSensorFrame(VIF_FRAME_START);		
			SetA8RegB(0x690a,GetA8RegB(0x690a)|0x1);
			SetA8RegB(0x690a,GetA8RegB(0x690a)&0xfe);			
//			tmp5041 = GetA8RegB(0x5041);		
//			SetA8RegB(0x5041,tmp5041&0x2);	
			if (gMainPanelControlAttrib.LCD_BusCtl.BusWidth==8){		   
				SetA8RegB(0x5026, TYPE_80|
				POLARITY_ENABLE|
				PHASE_ENABLE|
				NON_BURST_MODE|
				//	BURST_MODE|
				DATA_BUS_8);
				SetA8RegB(0x5027, RGB56508BIT);				
	      		}
		}
		
		#if AIT_VIDEO_PHONE_SUPPORT
            if(AIT_Preview_config.previewMode == AIT_VIDPHONE_PREV_MODE){
                //A8L_SetTVPreviewMode(2);	
                retVal = A800_VideoPhoneStart();
            }
            else{
                //A8L_SetTVPreviewMode(0);
                retVal = A800_StartPreview();            
            }
		#else
                retVal = A800_StartPreview();            
		#endif		


		if(CAM_LCD_REFRESH_BYPASS == A800Data.Select->LcdRefreshMode){
			A800_PIPBypass(1);	
			SetA8RegB(0x7111,tmp7111);				
//			A8L_CheckSensorFrame(VIF_GRAB_END);	
//			SetA8RegB(0x5041,tmp5041);
		}


		if (retVal != A8_NO_ERROR)
		{
			sys_IF_ait_set_status(AIT_STATUS_DUMMY);
			AIT_Message_P1 ("Start Preview Fail !! AIT Status=%d\n", sys_IF_ait_get_status());
			gAitPreviewStatus = FALSE;			
		}
		else
		{
			gAITIsPreview = TRUE;
			sys_IF_ait_set_status(AIT_STATUS_CAM_PREVIEW);
			AIT_Message_P1 ("Start Preview OK !! AIT Status=%d\n", sys_IF_ait_get_status());
			gAitPreviewStatus = TRUE;			
		}
	}
	else
	{
#if AIT_VIDEO_PHONE_SUPPORT
            if(AIT_Preview_config.previewMode == AIT_VIDPHONE_PREV_MODE){
                retVal = A800_VideoPhoneStop();
            }
            else{
                retVal = A800_StopPreview();
            }
#else
		retVal = A800_StopPreview();
#endif
		A8L_SetIconActive(0, 0);

		if(CAM_LCD_REFRESH_BYPASS == A800Data.Select->LcdRefreshMode)
			A800_PIPBypass(0);


		if (retVal != A8_NO_ERROR)
		{
			AIT_Message_P1 ("Stop Preview Fail !! AIT Status=%d\n", sys_IF_ait_get_status());
		}

		SetA8RegB(0x690a,GetA8RegB(0x690a)|0x1);
		SetA8RegB(0x690a,GetA8RegB(0x690a)&0xfe);
		gAITIsPreview = FALSE;
		sys_IF_ait_set_status(AIT_STATUS_CAM_READY);
		gAitPreviewStatus = FALSE;			
	}
	
	return retVal;
}

/**
 @fn		A8_ERROR_MSG	cam_IF_ait_preview_type (char PipBypassOn, u_short Win, u_short Hin, u_short Wout, u_short Hout)
 @brief	Set preview mode, 0: LCD scaler 1: PIP bypass 2:both not
 @param	PipBypassOn: 0: LCD scaler 1: PIP bypass 2:both not
 @param	Win: LCD scaler input width
 @param	Hin: LCD scaler input height
 @param	Wout: LCD scaler output width
 @param	Hout: LCD scaler output height
 @return	A8_ERROR_MSG
 @note   	Preview enable or disable. BB must call this function after sensor enable!!
 @bug	N/A.
*/ 
A8_ERROR_MSG	cam_IF_ait_preview_LcdScaler (u_short Win, u_short Hin, u_short Wout, u_short Hout)
{
	
	A800_LCDScaler(Win, Hin, Wout, Hout);
	return A8_NO_ERROR;
}

/**
 @fn		A8_ERROR_MSG	cam_IF_ait_capture_config (AIT_CAPTURE_CONFIG captureConfig)
 @brief	Pass the struct of the image resolution, quility levle, sticker flag.
 @param	captureConfig
 @return	A8_ERROR_MSG
 @note   	Capture parameters setting for AIT. Capture zoom_step is from preview zoom_step.
 @bug	N/A.
*/ 
void cam_IF_ait_capture_config(ext_camera_para_struct*  Ext_Capture_config)
{
	AIT_Capture_config.jpeg_quality = Ext_Capture_config -> jpeg_compression_ratio;
	AIT_Capture_config.maxEncodedSize = Ext_Capture_config ->image_buffer_size;
	AIT_Capture_config.jpeg_withEXIF = FALSE;
	AIT_Capture_config.sticker_capture = Ext_Capture_config ->sticker_capture;

	AIT_Message_P3("capture 1=0x%x, 2=0x%x, 3=0x%x \r\n",AIT_Capture_config.jpeg_quality,AIT_Capture_config.maxEncodedSize,AIT_Capture_config.sticker_capture );
#if 0	
	if((Ext_Capture_config ->image_width == 640) && (Ext_Capture_config ->image_height == 480))
			AIT_Capture_config.image_resolution = 0;
	else if((Ext_Capture_config ->image_width == 320) && (Ext_Capture_config ->image_height == 240))
			AIT_Capture_config.image_resolution = 1;
	else if((Ext_Capture_config ->image_width == 128) && (Ext_Capture_config ->image_height == 160))
			AIT_Capture_config.image_resolution = 2;		
	else if((Ext_Capture_config ->image_width == 128) && (Ext_Capture_config ->image_height == 96))
			AIT_Capture_config.image_resolution = 3;		
	else if((Ext_Capture_config ->image_width == 160) && (Ext_Capture_config ->image_height == 120))
			AIT_Capture_config.image_resolution = 4;		
	else if((Ext_Capture_config ->image_width == 128) && (Ext_Capture_config ->image_height == 160))
			AIT_Capture_config.image_resolution = 5;		
	else if((Ext_Capture_config ->image_width == 128) && (Ext_Capture_config ->image_height == 96))
			AIT_Capture_config.image_resolution = 6;
	else if((Ext_Capture_config ->image_width == 80) && (Ext_Capture_config ->image_height == 60))
			AIT_Capture_config.image_resolution = 0xa;		
	else if((Ext_Capture_config ->image_width == 176) && (Ext_Capture_config ->image_height == 220))
			AIT_Capture_config.image_resolution = 0xb;		
	else 
			AIT_Capture_config.image_resolution = 0;
	A800_SetCaptureResolution(AIT_Capture_config.image_resolution);										
#else
	gsJpegWidth = Ext_Capture_config ->image_width;
	gsJpegHeight = Ext_Capture_config ->image_height;
	AIT_Message_P2("JpegReso=0x%x x 0x%x\r\n",gsJpegWidth,gsJpegHeight);
#endif
}
/**
 @fn		A8_ERROR_MSG	cam_IF_ait_capture (u_short *jpeg_buffer_ptr, u_int *JpegSize)
 @brief	Pass the memory buffer pointer for AIT save the JPEG file.
 @param	*jpeg_buffer_ptr  Pointer of the buffer in BB memory.
 @param	*JpegSize   The size limite for the capture JPEG file.
 @return	A8_ERROR_MSG
 @note   	Capture process. BB must make sure AIT in preview status.
 @bug	N/A.
*/ 
// (request an image capture, JPEG encode, get image )	
A8_ERROR_MSG	cam_IF_ait_capture (u_short *jpeg_buffer_ptr, u_int *JpegSize)
{
	u_char retVal;
	gAitCaptureStatus = FALSE;
	AIT_Message_P1("zoom_step=%d\n\r", AIT_Camera_config.zoom_step);
	AIT_Message_P1("image_resolution=%d\n\r", AIT_Capture_config.image_resolution);
	AIT_Message_P1("jpeg_quality=%d\n\r", AIT_Capture_config.jpeg_quality);
	AIT_Message_P1("maxEncodedSize=%d\n\r", AIT_Capture_config.maxEncodedSize);
	AIT_Message_P1("jpeg_withEXIF=%d\n\r", AIT_Capture_config.jpeg_withEXIF);
	AIT_Message_P1("sticker_capture=%d\n\r", AIT_Capture_config.sticker_capture);

	sys_IF_ait_set_status(AIT_STATUS_CAM_CAPTURE);

	*JpegSize = 0;

#if defined(EXIFSUPPORT)
	if (TRUE == AIT_Capture_config.jpeg_withEXIF)
	{
		// These code depends on Customer!
		u_char gMakeValue[13] = {"AIT Corp."};
		u_char gModelValue[9]  = {"AIT701"};
		u_char gDateTimeInfo[19] = {"2009.03.19 13:13:13"};

		SetEXIFMakeModelInfo(gMakeValue, gModelValue);
		SetEXIFDateTimeInfo(gDateTimeInfo);
		SetEXIFZoomInfo();
		SetEXIFImageResolutionInfo();
		SetEXIFExposureInfo();
		SetEXIFExposureBiasInfo();
	}
#endif

	A800_JPEGQuality(AIT_Capture_config.jpeg_quality);

	if (AIT_Capture_config.sticker_capture == TRUE)
	{
		SetGraphicsBackground (AIT_Preview_config.transparencyColor);
		retVal = A800_TakeJpegWithSticker(jpeg_buffer_ptr, JpegSize);
		if (retVal != A8_NO_ERROR)
		{
			sys_IF_ait_set_status(AIT_STATUS_DUMMY);
			AIT_Message_P0("A800_TakeStickerImage is failed !!!\n");
			return A8_CAM_CAPTURE_ERROR;
		}
	}
	else
	{

{	
		extern u_short	gsJpegWidth,gsJpegHeight;
		A800_TakeJpegSetting(gsJpegWidth,gsJpegHeight);

		retVal = A800_TakeJPEG(AIT_Capture_config.maxEncodedSize, AIT_Capture_config.maxEncodedSize/*300*1024*/, jpeg_buffer_ptr, JpegSize);
		if (retVal != A8_NO_ERROR) {
			sys_IF_ait_set_status(AIT_STATUS_DUMMY);
			AIT_Message_P0("A800_JPEGCapture is failed !!!\n");
			return A8_CAM_CAPTURE_ERROR;
		}		
}		
		

	}

	gAITIsPreview = FALSE;
	sys_IF_ait_set_status(AIT_STATUS_CAM_READY);
	
	// After capture process, AIT will be in stop_preview status.
	gAitCaptureStatus = TRUE;
	return A8_NO_ERROR;
	
}

// (request an image capture, JPEG encode, get image )
/**
 @fn		A8_ERROR_MSG	cam_IF_ait_update_OSD (u_short *OSDBufferPtr, u_short x, u_short y, u_short width, u_short height )
 @brief	Pass the memory buffer pointer for AIT save the JPEG file.
 @param	*OSDBufferPtr  Pointer of the OSD buffer in BB memory.
 @return	A8_ERROR_MSG
 @note  	This OSD data is for preview process. BB has to update OSD data into AIT with 128x160 RGB565 data.
	Thus, During AIT handles preview, AIT will combine "preview" and "OSD" data to show on LCD.
 @bug	N/A.
*/ 
extern u_short  gsCurViewMode;
A8_ERROR_MSG	cam_IF_ait_update_OSD (u_short *OSDBufferPtr, u_short x, u_short y, u_short width, u_short height )
{
	return A800_UpdateOSDBuffer(gsCurViewMode, OSDBufferPtr, x, y, width, height );
}


// (encodes one frame)
/**
 @fn		A8_ERROR_MSG	cam_IF_ait_JPEG_encode (u_short *rgbBufferPtr, u_short rgbW, u_short rgbH, u_short *JpegBufferPtr, u_int *JpegSize)
 @brief	Pass the memory buffer pointer for AIT save the JPEG file.
 @param	*rgbBufferPtr  Pointer of the RGB buffer in BB memory.
 @param rgbW: width of the RGB source
 @param rgbH:  hight of the RGB source
 @param *JpegBufferPtr: Pointer of the JPEG buffer in BB memory.
 @param *JpegSize: TBD
 @return	A8_ERROR_MSG
 @note  	TBD
 @bug	N/A.
*/ 
A8_ERROR_MSG	cam_IF_ait_JPEG_encode (u_short *rgbBufferPtr, u_short rgbW, u_short rbgH, u_short *JpegBufferPtr, u_int *JpegSize)
{
	return A8_NO_ERROR;
}

/**
 @fn		A8_ERROR_MSG	cam_IF_ait_playback_config (AIT_PREVIEW_CONFIG playbackConfig)
 @brief	Pass the play back mode. JPEG decode to RGB, video playback... etc
 @param	playbackConfig: The transparency Color, resoulation... etc.
 @return	A8_ERROR_MSG
 @note  	TBD
 @bug	N/A.
*/ 
A8_ERROR_MSG	cam_IF_ait_playback_config (AIT_PREVIEW_CONFIG playbackConfig)
{
	u_char retVal;
#ifndef A8_DISABLE_SUB_LCD_FUNCTION	
	if (playbackConfig.lcdMode == A8_SUB_LCD)
	{
		retVal = A800_SubLCDPreviewActive();
		sys_IF_ait_delay1ms(2);
		
		A800_UsingLCD2();
		A800_HostInitLCD_P2();
	}
	else
#endif
	{
		retVal = A800_MainLCDPreviewActive();
		sys_IF_ait_delay1ms(2);
		
		A800_UsingLCD1();
		A800_HostInitLCD();
	}
	
	if (retVal != 0)
	{
		sys_IF_ait_set_status(AIT_STATUS_DUMMY);
		AIT_Message_P0("cam_IF_ait_playback_config:A800_Main/SubLCDPreviewActive is failed !!!\r\n");
		return A8_SYSTEM_ERROR;
	}
	
	// Base on preview config parameter.
	// 1. transparencyColor for OSD data.
	A800_Preview_MainWindow_Transparency(1, playbackConfig.transparencyColor);
	AIT_Preview_config.transparencyColor = playbackConfig.transparencyColor;
	AIT_Message_P1("cam_IF_ait_playback_config, transcolor=0x%x\n", AIT_Preview_config.transparencyColor);
	A8L_SetWindowTransparency(A8_OVERLAY_WINDOW, 1, playbackConfig.transparencyColor);
    	A8L_SetIconTransparency(1, playbackConfig.transparencyColor);


	// 2. playbackMode, need to mapping into A800_SetPlaybackMode()
	AIT_Preview_config.previewMode = playbackConfig.previewMode;
	retVal = A800_SetPlaybackMode(playbackConfig.previewMode);
	
	AIT_Message_P1("cam_IF_ait_playback_config, playback mode=0x%x\n", AIT_Preview_config.previewMode);
	
	if (retVal != A8_NO_ERROR)
	{
		sys_IF_ait_set_status(AIT_STATUS_DUMMY);
		AIT_Message_P0("A800_SetPlaybackMode is failed !!!\r\n");
		return A8_SYSTEM_ERROR;
	}
	else
	{		
		return A8_NO_ERROR;
	}
}


/**
 @fn		A8_ERROR_MSG	cam_IF_ait_JPEG_decode(A8S_JPEG_INFO SrcJPEG, u_short* pRgbBuf, u_short OutW, u_short OutH, u_char OutFormat)
 @brief	Decode one frame.
 @param 
 @return	A8_ERROR_MSG
 @note  	TBD
 @bug	N/A.
*/ 
A8_ERROR_MSG cam_IF_ait_JPEG_decode(u_short * jpegbuf, u_int jpegsize, u_short panelwidth, u_short panelheight, u_char * dataptr)
{
	u_char retVal;
	s_short jpegFormat, jpegWidth, jpegHeight;
	u_short Quality;	

	sys_IF_ait_set_status(AIT_STATUS_CAM_PLAY);
	
   	SetA8RegB(0x6902, GetA8RegB(0x6902) | 0x40); //enable fast jpeg decode clock
   	
	retVal = A8L_GetJpegInfo( jpegbuf, jpegsize, &jpegWidth, &jpegHeight, &jpegFormat ,&Quality);
	if (retVal != 0) {  
		return 1;
	}

	AIT_Message_P3("cam_IF_ait_JPEG_decode js=%d, pw=%d, ph=%d \r\n", jpegsize, panelwidth, panelheight);
	jpegsize += 64;
	if((panelwidth*panelheight*2)<=(240*180*2))
	{
		retVal=A800_DecodeJpeg( jpegbuf, jpegsize, &panelwidth, &panelheight, (u_short *)dataptr, 1);		
	}
	else if((panelwidth == 640) && (panelheight == 480))
	{
		AIT_Message_P0("DecodeJpegToYuvViaFIFO\r\n");
		SetA8RegB(0x620A, 0x01);		
		SetA8RegB(0x4F1D, 0x00);	
		retVal = A8L_DecodeJpegToYuvViaFIFO(jpegbuf, jpegsize, (u_short *)dataptr ) ;
	}	
	else if((jpegWidth == panelwidth) && (jpegHeight == panelheight))
	{
		SetA8RegB(0x620A, 0x01);		/* JPEG repeat ratio */
		SetA8RegB(0x4F1D, 0x00);	
		retVal = A8L_DecodeJpegToRGBViaFIFO(jpegbuf, jpegsize, (u_short *)dataptr );
#if 0		
	}else if((jpegWidth == 2*panelwidth) && (jpegHeight == 2*panelheight)){
		SetA8RegB(0x620A, 0x41);		/* JPEG 1/2 down sample with repeat ratio */
		SetA8RegB(0x4F1D, 0x01);		/* Scaler V 1/2 downsample enable */
		retVal = A8L_DecodeJpegToRGBViaFIFO(jpegbuf, jpegsize, (u_short *)dataptr );
		SetA8RegB(0x620A, 0x01);		/* JPEG repeat ratio */
		SetA8RegB(0x4F1D, 0x00);			
#endif		
	}	
	else
	{
		retVal = A8_UNSUPPORT_ERROR;
	}
	AIT_Message_P1("cam_IF_ait_JPEG_decode 1,ret = %d\r\n",retVal);


	if (retVal != A8_NO_ERROR)
	{
		sys_IF_ait_set_status(AIT_STATUS_DUMMY);
		AIT_Message_P0("cam_IF_ait_JPEG_decode is failed !!!\n");
		return	A8_CAM_DECODE_ERROR;
	}
	else
	{
		sys_IF_ait_set_status(AIT_STATUS_CAM_READY);
		return A8_NO_ERROR;
	}
}


/**
 @fn		A8_ERROR_MSG	cam_IF_ait_vdo_record_start(void)
 @brief	Record the video.
 @return	A8_ERROR_MSG
 @note  	TBD
 @bug	N/A.
*/
A8_ERROR_MSG	cam_IF_ait_vdo_record_start(u_short quality)
{
	A800_SetMJPEGRecordStart();
	A800_SetQTable(quality);
	sys_IF_ait_set_status(AIT_STATUS_VDO_RECORD);
	
	return A8_NO_ERROR;
}

/**
 @fn		A8_ERROR_MSG cam_IF_ait_vdo_record_handler(u_char *jpegBuf, u_int *returnSize)
 @brief	Video record handle for get the video data from AIT to BB.
 @return	A8_ERROR_MSG
 @note  	.
 @bug	N/A.
*/
A8_ERROR_MSG cam_IF_ait_vdo_record_handler(u_char *frameBuf, u_int *frameSize)
{
	u_char retVal;
	
	retVal = A800_TransferMJPEGFrameToHost(frameBuf, frameSize);
	
    return retVal;
}

/**
 @fn		A8_ERROR_MSG cam_IF_ait_vdo_record_stop(void)
 @brief	Stop the Video record.
 @return	A8_ERROR_MSG
 @note  	.
 @bug	N/A.
*/
A8_ERROR_MSG cam_IF_ait_vdo_record_stop(void)
{	
	A800_SetMJPEGRecordStop();
	sys_IF_ait_set_status(AIT_STATUS_CAM_PREVIEW);	
	return A8_NO_ERROR;
}

/**
 @fn		A8_ERROR_MSG cam_IF_ait_vdo_playback_handler(u_short *frameBuf, u_int frameSize)
 @brief	
 @return	A8_ERROR_MSG
 @note  	.
 @bug	N/A.
*/
A8_ERROR_MSG cam_IF_ait_vdo_playback_handler(u_short *frameBuf, u_int frameSize)
{
	u_char retVal=0;
//	u_short jpeg_qulity;
//	s_short jpeg_width, jpeg_height, jpeg_format;
	
	u_short width=0;
	u_short height=0;
//Vin@20091219: Select Panel Size in same lib.

	A800Data.Select = A800_ChangeCameraSetting(gsCurViewMode);
	if(!A800Data.Select )
		return A8_SYSTEM_ERROR;
	width = A800Data.Select->BufWidth;
	height = A800Data.Select->BufHeight;
	
	retVal = A800_DecodeJpeg(frameBuf, frameSize, &width, &height, NULL, 1);
	if(retVal != 0)
	{
		AIT_Message_P1("A800_DecodeJpeg is failed %d\r\n",retVal);
		return A8_VDO_DECODE_ERROR;
	}

    return A8_NO_ERROR;
}

#if defined(AIT_3GP_SUPPORT)

A8_ERROR_MSG cam_IF_ait_vdo_playback_YUV420_buf_cfg(PREVIEW_MODE previewmode,YUV420BufInfo* YUV420Info)

{
	
		
	u_short tempWidth, tempHeight, rateV, rateH, Base;
	u_short	DispLcdX,DispLcdY;

	
	unsigned int Temp_Addr;
	unsigned short LCD_X_Shift;
	unsigned short LCD_Y_Shift;
	unsigned short LCD_N;
	unsigned short LCD_M;
	unsigned short LCD_O_W;
	unsigned short LCD_O_H;

	u_short InWidth = YUV420Info->u16VideoWidth;
	u_short InHeight = YUV420Info->u16VideoHeight;
	u_short OutWidth = A800Data.Select->DispWidth;
	u_short OutHeight = A800Data.Select->DispHeight;


	A800Data.Select->BufWidth = YUV420Info->u16VideoWidth;
	A800Data.Select->BufHeight = YUV420Info->u16VideoHeight; 


	if(YUV420Info->u16VideoWidth==224)
	{
		if(previewmode==AIT_VDO_PLAY_YUVBUF_NOR_MODE)
		{
				OutWidth = 174;
				OutHeight = 138;
		}
	}


//Buffer allocate
	Temp_Addr = A800Data.Select->BufAddr;
	SetA8RegW(0x5088, (u_short)Temp_Addr);
	SetA8RegW(0x508A, (u_short)(Temp_Addr>>16));
	SetA8RegW(0x508C, (u_short)Temp_Addr);
	SetA8RegW(0x508E, (u_short)(Temp_Addr>>16));

	Temp_Addr = A800Data.Select->BufAddr + 
				(A800Data.Select->BufWidth * A800Data.Select->BufHeight); 
	SetA8RegW(0x4800, (u_short)Temp_Addr);
	SetA8RegW(0x4802, (u_short)(Temp_Addr>>16));
	SetA8RegW(0x4808, (u_short)Temp_Addr);
	SetA8RegW(0x480A, (u_short)(Temp_Addr>>16));

	Temp_Addr = A800Data.Select->BufAddr + 
				(A800Data.Select->BufWidth * A800Data.Select->BufHeight) * 5 /4; 
	SetA8RegW(0x4804, (u_short)Temp_Addr);
	SetA8RegW(0x4806, (u_short)(Temp_Addr>>16));
	SetA8RegW(0x480c, (u_short)Temp_Addr);
	SetA8RegW(0x480e, (u_short)(Temp_Addr>>16));

 	
	DispLcdX = A800Data.Select -> DispLcdX;
	DispLcdY = A800Data.Select -> DispLcdY;
	tempWidth = OutWidth;
	tempHeight = OutHeight;
	
	if ((tempWidth <= InWidth) && (tempHeight <= InHeight)) // Scale down
	{
		Base = 32;
		rateH = tempWidth * Base / InWidth;
		rateV = tempHeight * Base / InHeight;

		if( ((s_short)InWidth * rateH / Base - tempWidth ) < 0 )
			rateH+=1;
		if( ((s_short)InHeight* rateV / Base - tempHeight) < 0 )
			rateV+=1;
		
		if (rateH < rateV)
			rateH = rateV;

		tempWidth = InWidth * rateH / Base;
		if (InWidth * rateH % Base != 0)
			tempWidth +=1;
		
		tempHeight = InHeight * rateV / Base;
		if (tempHeight * rateV % Base != 0)
			tempHeight +=1;
	}
	else		// Scale up
	{
		Base = 64;
		rateH = tempWidth * Base / (InWidth-1);
		rateV = tempHeight * Base / (InHeight-1);

		if( (((s_short)InWidth-1) * rateH / Base - tempWidth) < 0 )
			rateH+=1;
		if( (((s_short)InHeight-1) * rateV / Base - tempHeight) < 0 )
			rateV+=1;
		
		if (rateH < rateV)
			rateH = rateV;

		tempWidth = (InWidth-1) * rateH / Base;
		if ((InWidth-1) * rateH % Base != 0)
			tempWidth +=1;
		
		tempHeight = (InHeight-1) * rateV / Base;
		if ((tempHeight-1) * rateV % Base != 0)
			tempHeight +=1;
	}

	if(YUV420Info->u16VideoWidth==224)
	{
		if(previewmode==AIT_VDO_PLAY_YUVBUF_NOR_MODE)
		{
			tempWidth = 176;
			rateH = 66;
			Base = 84;
		}	
		else
			tempHeight = 224;
	}
	AIT_Message_P1("\r\nW = %d\r\n", YUV420Info->u16VideoWidth);
	AIT_Message_P1("Base = %d\r\n", Base);
	AIT_Message_P1("rateH = %d\r\n", rateH);
	AIT_Message_P1("tempWidth = %d\r\n", tempWidth);
	AIT_Message_P1("tempHeight = %d\r\n", tempHeight);
	
	SetA8RegW(0x4840, InWidth);
	SetA8RegW(0x4842, InHeight);
	SetA8RegW(0x484C, tempWidth);	//scaler out frame width
	SetA8RegW(0x484E, tempHeight);
	
	SetA8RegW(0x4850, rateH);

	SetA8RegW(0x4852, Base);
	if(YUV420Info->u16VideoWidth==224)
	{
		if(previewmode==AIT_VDO_PLAY_YUVBUF_NOR_MODE)
		{
			SetA8RegW(0x507E, 174);

		}
	}
	else
		SetA8RegW(0x507E, OutWidth);
	
	SetA8RegW(0x5080, OutHeight);
	
	SetA8RegW(0x5076, A800Data.Select->DispLcdX);
	SetA8RegW(0x5078, A800Data.Select->DispLcdY);	

	SetA8RegW(0x4810, InWidth);

	Temp_Addr = 0;

	SetA8RegW(0x5084, (u_short)Temp_Addr);
	SetA8RegW(0x5086, (u_short)(Temp_Addr>>16));
	Temp_Addr = 0;

	SetA8RegW(0x4814, (u_short)Temp_Addr);
	SetA8RegW(0x4816, (u_short)(Temp_Addr>>16));

	SetA8RegW(0x507A, 0x0001);
	SetA8RegW(0x507C, InWidth);

	SetA8RegW(0x4818, 1);
	SetA8RegW(0x481a, A800Data.Select->BufWidth/2);

	Temp_Addr = A800Data.Select->BufAddr + 
		(A800Data.Select->BufWidth * A800Data.Select->BufHeight) * 6 /4;
	SetA8RegW(0x4838, (u_short)Temp_Addr);
	SetA8RegW(0x483a, (u_short)(Temp_Addr>>16));
	AIT_Message_P1("### LCD WriteBuffer_1 = 0x%x\r\n", Temp_Addr);
	Temp_Addr = Temp_Addr + OutWidth*2*2;
	SetA8RegW(0x483c, (u_short)Temp_Addr);
	SetA8RegW(0x483e, (u_short)(Temp_Addr>>16));
	AIT_Message_P1("### LCD WriteBuffer_2 = 0x%x\r\n", Temp_Addr);


	SetA8RegW(0x4844, OutWidth*2);
	SetA8RegW(0x4846, 0x0000);

	SetA8RegB(0x4848, 0x0C);


	SetA8RegB(0x504A,  GetA8RegB(0x504A) | 0x40);	//scaler enable

	SetA8RegB(0x504A,  GetA8RegB(0x504A) & (~0x20));

	SetA8RegB(0x504B,  GetA8RegB(0x504B) & 0xF7);
		
	SetA8RegB(0x504B,  GetA8RegB(0x504B) | 0x10);
	SetA8RegB(0x504B,  GetA8RegB(0x504B) & 0xBF);	//420

	SetA8RegB( 0x5042, (GetA8RegB(0x5042) & 0xFE ));
	SetA8RegB( 0x5040, (GetA8RegB(0x5040) & 0xFD ));

	SetA8RegB(0x50F0, 0x00);

	AIT_Message_P2("Y Reg[%x] = %x\r\n",0x5088,GetA8RegW(0x5088)); 
	AIT_Message_P2("	Reg[%x] = %x\r\n",0x508A,GetA8RegW(0x508A));
	AIT_Message_P2("Y Reg[%x] = %x\r\n",0x508C,GetA8RegW(0x508C)); 
	AIT_Message_P2("	Reg[%x] = %x\r\n",0x508E,GetA8RegW(0x508E));

	AIT_Message_P2("U Reg[%x] = %x\r\n",0x4800,GetA8RegW(0x4800)); 
	AIT_Message_P2("	Reg[%x] = %x\r\n",0x4802,GetA8RegW(0x4802));
	AIT_Message_P2("U Reg[%x] = %x\r\n",0x4808,GetA8RegW(0x4808)); 
	AIT_Message_P2("	Reg[%x] = %x\r\n",0x480A,GetA8RegW(0x480A));


	AIT_Message_P2("U Reg[%x] = %x\r\n",0x4804,GetA8RegW(0x4804)); 
	AIT_Message_P2("	Reg[%x] = %x\r\n",0x4806,GetA8RegW(0x4806));
	AIT_Message_P2("U Reg[%x] = %x\r\n",0x480c,GetA8RegW(0x480c)); 
	AIT_Message_P2("	Reg[%x] = %x\r\n",0x480e,GetA8RegW(0x480e));

	
	AIT_Message_P2("Reg[%x] = %x\r\n",0x4840,GetA8RegB(0x4840)); 
	AIT_Message_P2("Reg[%x] = %x\r\n",0x4842,GetA8RegB(0x4842));

	AIT_Message_P2("Reg[%x] = %x\r\n",0x484C,GetA8RegB(0x484C)); 
	AIT_Message_P2("Reg[%x] = %x\r\n",0x484E,GetA8RegB(0x484E));

	AIT_Message_P2("Reg[%x] = %d\r\n",0x4850,GetA8RegB(0x4850)); 
	AIT_Message_P2("Reg[%x] = %d\r\n",0x4852,GetA8RegB(0x4852));	
	

	AIT_Message_P2("Reg[%x] = %x\r\n",0x504a,GetA8RegB(0x504a)); 
	AIT_Message_P2("Reg[%x] = %x\r\n",0x504b,GetA8RegB(0x504b));
}		


/**
 @fn		A8_ERROR_MSG cam_IF_ait_vdo_playback_YUV420_buf_cfg(void)
 @brief	
 @return	A8_ERROR_MSG
 @note  	.
 @bug	N/A.
*/
A8_ERROR_MSG cam_IF_ait_vdo_playback_YUV420_buf_update(YUV420BufInfo* ptrYUV420BufInfo)
{
	int i;
	YUV420BufInfo* p = ptrYUV420BufInfo;
	for(i=0;i<p->u16VideoHeight;++i)										
		CopyMemByteHostToA8(A800Data.Select->BufAddr+i*A800Data.Select->BufWidth,
								p->ptrYbuf+i*p->u16YBufWidth, 
								p->u16VideoWidth);

	for(i=0;i<p->u16VideoHeight/2;++i)				
		CopyMemByteHostToA8(A800Data.Select->BufAddr+A800Data.Select->BufWidth*A800Data.Select->BufHeight+i*A800Data.Select->BufWidth/2,
								p->ptrUbuf+i*p->u16UVBufWidth,
								p->u16VideoWidth/2);
	
	for(i=0;i<p->u16VideoHeight/2;++i)	
	
		CopyMemByteHostToA8(A800Data.Select->BufAddr+A800Data.Select->BufWidth*A800Data.Select->BufHeight*5/4+i*A800Data.Select->BufWidth/2,
								p->ptrVbuf+i*p->u16UVBufWidth,
								p->u16VideoWidth/2);

}

#endif /* AIT_3GP_SUPPORT */

/**
 @fn		A8_ERROR_MSG cam_IF_ait_vdo_playback_stop(void)
 @brief	Stop the Video playback.
 @return	A8_ERROR_MSG
 @note  	.
 @bug	N/A.
*/
A8_ERROR_MSG cam_IF_ait_vdo_playback_stop(void)
{
	u_char retVal = A8_NO_ERROR;

	return retVal;
}

/**
 @fn		A8_ERROR_MSG cam_IF_ait_preview_start(void)
 @brief	Preview start.
 @return	A8_ERROR_MSG
 @note  	.
 @bug	N/A.
*/
A8_ERROR_MSG cam_IF_ait_preview_start(PREVIEW_MODE preview_mode, ePREVIEW_SRC_MODE preview_src)//ext_camera_para_struct *ext_cam_para)
{
	u_char retVal=0;
	u_char isVideoMode = 0;
	AIT_PREVIEW_CONFIG previewConfig;

//Vin@20091210:Twin Sensor & TV
		switch(preview_src)
		{

			case AIT_TV_SRC:

				if(preview_mode == AIT_ATV_PREV_FULL_MODE)//preview_mode==1) //DSC mode
				{
					AIT_Message_P0("Full mode\r\n");	
					previewConfig.previewMode = AIT_ATV_PREV_FULL_MODE;
				}
				else //VIDEO mode
				{
					AIT_Message_P0("Landscap mode\r\n");
					previewConfig.previewMode = AIT_ATV_PREV_NOR_MODE;
				}
				break;


			default:
			case AIT_MASTER_SENSOR_SRC:
			case AIT_SLAVE_SENSOR_SRC:

				previewConfig.previewMode = preview_mode;
				switch(preview_mode)
				{
					case AIT_VDO_PREV_MODE:
					#if AIT_VIDEO_PHONE_SUPPORT
        			case AIT_VIDPHONE_PREV_MODE: //Andy@20100127. 
					#endif

					case AIT_VDO_REC_MODE:
						AIT_Message_P0("Video mode\r\n");			
						isVideoMode = 1;
						A8L_SensorFixedFrameRate(1);
						break;

				}
				break;
		}

	
	previewConfig.lcdMode = A8_MAIN_LCD;
	
	//Vin: Todo
    //    previewConfig.transparencyColor = OSD_TRANSPARENCY_COLOR ;
	// VIN: preview Screen is separate by some block

	SetA8RegB(0x500A,0x0);
	SetA8RegB(0x6908,GetA8RegB(0x6908)|0x54);
	SetA8RegB(0x6908,GetA8RegB(0x6908)&0xab);
	SetA8RegB(0x690a,GetA8RegB(0x690a)|0x15);
	SetA8RegB(0x690a,GetA8RegB(0x690a)&0xea);	
	
	retVal = cam_IF_ait_preview_config(previewConfig);
#if AIT_VIDEO_PHONE_SUPPORT	
	{
		unsigned short src_selection = 0;
		switch(preview_mode)
		{
			case AIT_CAM_PREV_NOR_MODE:
			case AIT_CAM_PREV_FULL_MODE:
			case AIT_JPG_PLAY_NOR_MODE:
			case  AIT_JPG_PLAY_FULL_MODE:		
			case AIT_VDO_PREV_MODE:
			case AIT_VDO_REC_MODE:
			case AIT_VDO_PLAY_NOR_MODE:
			case AIT_VDO_PLAY_FULL_MODE:
			case AIT_USB_WEBCAM_MODE:
			default:			
				src_selection = 0;
				break;			
			case AIT_ATV_PREV_NOR_MODE:
			case AIT_ATV_PREV_FULL_MODE:
				src_selection = 1;
				break;

			case AIT_VIDPHONE_PREV_MODE:
				src_selection = 2;

				break;

		}

    A8L_SetTVPreviewMode((u_short)src_selection,  (u_int)(A800Data.Select->BufAddr), (u_int)(A800Data.Select->BufAddr + A800Data.Select->BufWidth*A800Data.Select->BufHeight*2));
		
	}
#endif

	if(retVal==0){
		A800_ClearOSDBuffer(previewConfig.transparencyColor, previewConfig.previewMode);
	}

	if((preview_mode!=AIT_ATV_PREV_FULL_MODE)&&(preview_mode!=AIT_ATV_PREV_NOR_MODE))
	{
		if(!retVal)
	    {
	    	retVal = cam_IF_ait_camera_config (CAM_CTRL_MAX, 0);	
	        AIT_Message_P0("cam_IF_preview_config OK...\n");
	    }
    
	    else
	    {   
	        AIT_Message_P0("cam_IF_preview_config ERROR ...\n");

	    }
	}
	

	if(CAM_LCD_REFRESH_SCALE == A800Data.Select->LcdRefreshMode){
			cam_IF_ait_preview_LcdScaler(A800Data.Select->BufWidth,
										A800Data.Select->BufHeight,
										A800Data.Select->DispWidth,
										A800Data.Select->DispHeight);		
	}	
	
	SetA8RegB(0x7111,0x0);
			
	if(!retVal)
		retVal = cam_IF_ait_preview_control(A8_ON);	
#if AIT_VIDEO_PHONE_SUPPORT
	if(	AIT_VIDPHONE_PREV_MODE == preview_mode)
	{
		SetA8RegB(0x5040,GetA8RegB(0x5040)|0x04);
	}	
#endif	
		
	if(retVal)		
		return	MMP_FAILED;
	else

		return	MMP_SUCCESS;
}


//Vin@20091210:Twin Sensor & TV
/**
 @fn		A8_ERROR_MSG cam_IF_ait_VIF_Src_Selection(void)
 @brief	Select preview source from VIF and preview mode.
 @Para	if Preview_src = 0 or 1 (Sensor)
 @			preview_mode=0: Main sensor working
 @			preview_mode=1: Sub sensor working
 @		if preview_src = 2(TV)
 @			preview_mode=0 :Normal screen
 @			preview_mode=1 :Full screen and rotate
 @return	A8_ERROR_MSG
 @note  	.
 @bug	N/A.
*/
A8_ERROR_MSG cam_IF_ait_VIF_Src_Selection(ePREVIEW_SRC_MODE preview_src)
{
	extern sTVSensorPresent ait_tv_Sensor_Present;
	extern ePREVIEW_SRC_MODE	gsMasterSlaveSensor;
//	extern t_sensor_manager* sensor_manager[];
//	extern t_sensor_manager* ait_tv_manager[];

//	int i = 0;
	gsSensorUsing = NULL;
	AIT_Message_P1("cam_IF_ait_VIF_Src_Selection = %d", preview_src);
	
	switch(preview_src)
	{
		default:
		case AIT_MASTER_SENSOR_SRC:
			gsSensorUsing = ait_tv_Sensor_Present.MasterSensor;
			gsMasterSlaveSensor = AIT_MASTER_SENSOR_SRC;
			#if AIT_VIDEO_PHONE_SUPPORT
			A8L_SetTVPreviewMode(A8_OFF, 0x0000, 0x2800);
			#else
			A8L_SetTVPreviewMode(A8_ON);//			A8L_SetTVPreviewMode(A8_OFF);
			#endif
			break;

		case AIT_SLAVE_SENSOR_SRC:
			gsSensorUsing = ait_tv_Sensor_Present.SlaveSensor;	
			gsMasterSlaveSensor = AIT_SLAVE_SENSOR_SRC;
			#if AIT_VIDEO_PHONE_SUPPORT
			A8L_SetTVPreviewMode(A8_OFF, 0x0000, 0x2800);
			#else
			A8L_SetTVPreviewMode(A8_OFF);
			#endif
			break;


		case AIT_TV_SRC:
			gsSensorUsing = ait_tv_Sensor_Present.TV;
			#if AIT_VIDEO_PHONE_SUPPORT
			A8L_SetTVPreviewMode(A8_ON, 0x0000, 0x2800);
			#else
			A8L_SetTVPreviewMode(A8_ON);
			#endif
			break;


	}
	AIT_Message_P1("gsSensorUsing = %d", gsSensorUsing->sensor_id);

	if(gsSensorUsing)
		return A8_NO_ERROR;
	else
		return A8_SYSTEM_ERROR;
	

}

#if AIT_VIDEO_PHONE_SUPPORT
//u_int gait_video_phone_frame_ready=0;
#if 0   //Andy@20100223. 
u_char gvideo_phone_buf[80*64*2];

A8_ERROR_MSG cam_IF_ait_get_yuv420_data_From_AIT(u_short yuvWidth, u_short yuvHeight, u_char *yuv420_ptr)
{
    u_char retVal = A8_NO_ERROR;
    //u_int timeout=GET_YUV_DATA_TIMEOUT;
    if(0 != (GetA8RegB(0x654E) & 0x02)){
        retVal = A8_TIMEOUT_ERROR;
    }
    else{
        A800_VideoPhoneGetYUV420Data(yuvWidth, yuvHeight, yuv420_ptr);
    }
    A800_VideoPhoneStop();
    A800_VideoPhoneStart();

    //AIT_Message_P1("check preview stop timeout:%d\r\n", GET_YUV_DATA_TIMEOUT- timeout); 
    AIT_Message_P1("cam_IF_ait_get_yuv420_data_From_AIT, ret:%d", retVal);
    return retVal;
}
#endif

A8_ERROR_MSG cam_IF_ait_get_rgb565_data(u_short rgbWidth, u_short rgbHeight, u_char *rgb565_ptr)
{
    u_char retVal = A8_NO_ERROR;    

    if((0==rgb565_ptr) || (A800Data.Select->BufWidth != rgbWidth) || (A800Data.Select->BufHeight != rgbHeight)){
        AIT_Message_P0("cam_IF_ait_get_rgb565_data parameter error.\r\n");
        retVal=A8_INCORRECT_PARA_ERROR;
    }

    A800_VideoPhoneGetRGB565Data(rgbWidth, rgbHeight, rgb565_ptr);
    
    return retVal;
}

A8_ERROR_MSG cam_IF_ait_get_yuv420_data(u_short yuvWidth, u_short yuvHeight, u_char *yuv420_ptr)
{
    u_char retVal = A8_NO_ERROR;    
#if 0   //Andy@20100223.    
    u_int loop=0;
    u_char *psrc, *pdst;
#endif

    if((0==yuv420_ptr) || (A800Data.Select->BufWidth != yuvWidth) || (A800Data.Select->BufHeight != yuvHeight)){
        AIT_Message_P0("cam_IF_ait_get_yuv420_data parameter error.\r\n");
        retVal=A8_INCORRECT_PARA_ERROR;
    }

    A800_VideoPhoneGetYUV420Data(yuvWidth, yuvHeight, yuv420_ptr);
    
#if 0   //Andy@20100223.
    psrc = gvideo_phone_buf;
    pdst = yuv420_ptr;
#if 1//Andy@20100205.test
	memcpy(pdst, psrc, (yuvWidth*yuvHeight*3)>>1);
#else
    for(loop=0;loop<((yuvWidth*yuvHeight*3)>>1);loop++){
        *pdst++ = *psrc++;
    }
#endif
#endif

    return retVal;
}

A8_ERROR_MSG cam_IF_ait_check_snesor_frame(u_char mode)
{
    u_char retVal = A8_NO_ERROR;

//#define GET_YUV_DATA_TIMEOUT (60) //millisecond				
#if 0   //Andy@20100223. 
    if(AIT_Preview_config.previewMode == AIT_VIDPHONE_PREV_MODE){    
        cam_IF_ait_get_yuv420_data_From_AIT(A800Data.Select->BufWidth, A800Data.Select->BufHeight, (u_char *)gvideo_phone_buf);
    }
    else
#endif        
    {
        A8L_CheckSensorFrame(mode);
    }
    return retVal;
}
#endif




A8_ERROR_MSG cam_IF_ait_TV_SrcRes(ePREVIEW_TV_SRC TVSrcMode)
{
	extern sTVSensorPresent ait_tv_Sensor_Present;
	if(ait_tv_Sensor_Present.TV ==NULL)
		return A8_UNSUPPORT_ERROR;

	AIT_Message_P1("cam_IF_ait_TV_SrcRes = %d", TVSrcMode);
	switch(TVSrcMode)
	{
		default:
		case AIT_TV_SRC_PALD:
			ait_tv_Sensor_Present.TV->preview_mode->image_width = 360;
			ait_tv_Sensor_Present.TV->preview_mode->image_height = 288;
			break;
		case AIT_TV_SRC_NTSC:
			ait_tv_Sensor_Present.TV->preview_mode->image_width = 360;
			ait_tv_Sensor_Present.TV->preview_mode->image_height = 244;
			break;			
	}
	return  A8_NO_ERROR;	

}

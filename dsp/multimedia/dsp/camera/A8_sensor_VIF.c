/**
 * @file A8_sensor.c
 * @brief Copyright (C) 2002-2008 Alpha Imaging Technology Corp.
 * @n AIT701/703 Series Host API.
 * @n The purpose of AIT701 Host API is for Host CPU to control and communicate AIT701
 *      chip via direct/indirect parallel interface.
 * @note N/A
 * @bug N/A
 */

#include "A8_common.h"
#include "AIT700_ebibus.h"
#include "A8_vif.h"
#include "A8_dscapi.h"

#include "A8_sensor.h"
#include "AIT_Interface.h"
#include "A800_camera.h"
#define A8_SENSOR_WIDTH			(640)
#define A8_SENSOR_HEIGHT			(480)

SENSOR_TYPE sensor_type = SENSOR_TYPE_MAX_NUMBER;

t_sensor_manager* gsSensorUsing = NULL;
u_short	gAIT_camera_id = 0; //0: Main sensor  1: Sub sensor Please refer to CAM_MAIN_CAMERA and CAM_SUB_CAMERA
ePREVIEW_SRC_MODE	gsMasterSlaveSensor = AIT_MASTER_SENSOR_SRC; //0 is master sensor, 1 is slave sensor

//Vin@20091210:Twin Sensor & TV
sTVSensorPresent ait_tv_Sensor_Present = {NULL,NULL,NULL};
u_char	gbSensorInited = 0;

u_char	gA8I2Ctype = A8_I2C_1A1D;


void A8L_CheckSensorFrame(u_char mode)
{
	SetA8RegB(VIF_INT_ST_HOST,mode);
	while(TimeOutWhile(!((GetA8RegB(VIF_INT_ST_HOST)&mode)==mode)));
	if(CheckTimeOut()) {
		AIT_Message_P2("A8L_CheckSensorFrame failed, mode=0x%x, data=0x%x \r\n", mode,GetA8RegB(VIF_INT_ST_HOST));
	}	
}


u_char	A8L_CheckFrameStart(void)
{
	u_int nTimeout;
	u_char retVal;

	SetA8RegB(0x6041, (1<<2));
	nTimeout=400;
	do
	{
		--nTimeout;
		sys_IF_ait_delay1ms(1);
	}while( ( GetA8RegB(0x6041) & (1<<2) ) && nTimeout > 0 );

	if(nTimeout>0)
		retVal=0;
	else
		retVal=1;
	
	return retVal;
}

u_char	A8L_CheckExposureDone(void)
{
	u_int nTimeout;
	u_char retVal;

	SetA8RegB(0x6043, GetA8RegB(0x6043) | (1<<0));
	SetA8RegB(0x6042, GetA8RegB(0x6042) | (1<<0));

	nTimeout=400;
	do
	{
		--nTimeout;
		sys_IF_ait_delay1ms(1);
	}while( ( GetA8RegB(0x6042) & (1<<0) ) && nTimeout > 0 );

	if(nTimeout>0)
		retVal=0;
	else
		retVal=1;
	
	return retVal;
}

/**
 @fn		u_short A8L_SetSenReg(u_short Reg ,u_short Value)
 @brief	Write Sensor Register.
 @param	Reg - the Sensor Register.
 @param	Value - data write to sensor.
 @return	u_short - 0x00:Success, 0x01:timeout failure .
 @note	.
 @bug	N/A.
*/
u_short A8L_SetSenReg(u_short Reg, u_short Value)
{ 
	s_short	retVal;

	SetA8RegW(0x6546, Reg);
	SetA8RegW(0x6544, Value);

	retVal = SendA8Cmd(A8_HOST_CMD_SET_SENSOR_REG);
	if ( retVal != 0 )
	{
		AIT_Message_P0("A8L_SetSenReg cmd timer count \n");
		return A8_SYSTEM_ERROR;
	}

	if(A8L_CheckReadyForA8Command())
	{
		AIT_Message_P0("A8L_SetSenReg cmd not ready \n");	
		return A8_TIMEOUT_ERROR;
	}

	return A8_NO_ERROR;
}


/**
 @fn		u_short A8L_GetSenReg(u_short Reg ,u_short *Value)
 @brief	Read Sensor Register.
 @param	Reg - the Sensor Register.
 @param	Value - read back data from sensor.
 @return	u_short .
 @note	.
 @bug	N/A.
*/
u_short A8L_GetSenReg(u_short Reg, u_short *Value)
{ 
	s_short	retVal;

	SetA8RegW(0x6546, Reg);
	SetA8RegW(0x6544, 0xFF);

	retVal = SendA8Cmd(A8_HOST_CMD_GET_SENSOR_REG);
	if ( retVal != 0 )
	{
		AIT_Message_P0("Send cmd timer count \n");
		return A8_SYSTEM_ERROR;
	}

	if(A8L_CheckReadyForA8Command())
	{
		return A8_TIMEOUT_ERROR;
	}

	(*Value) =(GetA8RegW(0x6544));
	AIT_Message_P1("0x6544=0x%x\n",*Value);
	
	return A8_NO_ERROR;
}

u_char A8L_GetSenRegB(u_char Reg ,u_char *Value)
{
	s_short	retVal;

	SetA8RegB(0x6546, Reg);
	SetA8RegB(0x6544, 0xFF);

	retVal = SendA8Cmd(A8_HOST_CMD_GET_SENSOR_REG);
	if ( retVal != 0 )
	{
		AIT_Message_P0("Send cmd timer count \n");
		return A8_SYSTEM_ERROR;
	}

	if(A8L_CheckReadyForA8Command())
	{
		return A8_TIMEOUT_ERROR;
	}

	(*Value) =(GetA8RegB(0x6544));
	AIT_Message_P1("0x6544=0x%x\n",*Value);
	
	return A8_NO_ERROR;
}

/**
 @fn		u_short	A8L_I2CInit(u_char I2Ctype, u_char I2Cid)
 @brief	Set sensor I2C Write ID & I2C format to FW.
 @param	I2Ctype - (0)I2C_1A1D,(1)I2C_1A2D,(2)I2C_2A1D,(3)I2C_2A2D
 @param	I2Cid - I2C Write ID.
 @return	u_short
 @note	.
 @bug	N/A.
*/
u_short A8L_I2CInit(u_char I2Ctype, u_char I2Cid)
{
	gA8I2Ctype = I2Ctype;

	SetA8RegW(0x6540,(I2Ctype<<8) | I2Cid);
	return	SendA8Cmd(A8_HOST_CMD_I2C_INIT);
}

/**
 @fn		u_short	A8L_SensorClockOutput(u_char Enable)
 @brief	Enable output MCLK to sensor.
 @param	Enable - (0)Disable,(1)Enable
 @return	u_short
 @note	.
 @bug	N/A.
*/
u_short A8L_SensorClockOutput(u_char Enable)
{
	if(Enable)
	{
		SetA8RegB(0x6903,GetA8RegB(0x6903)&0x7F);
		sys_IF_ait_delay1ms(1);
		SetA8RegB(0x601D,GetA8RegB(0x601D)|0x80);
		//SetA8RegB(0x6020,0xA0);
	}
	else
	{
		//SetA8RegB(0x6020,0x00);
		SetA8RegB(0x601D,GetA8RegB(0x601D)&0x7F);
		sys_IF_ait_delay1ms(1);
		//SetA8RegB(0x6903,GetA8RegB(0x6903)|0x80);
	}
	sys_IF_ait_delay1ms(1);
	return A8_NO_ERROR;
}

void A8L_SetSensorIF(u_char gpio, u_char  enable)
{

	SetA8RegB(0x7150,GetA8RegB(0x7150) | (0x01 << gpio));

	if (enable)
		SetA8RegB(0x7151,GetA8RegB(0x7151) | (0x01 << gpio));
	else
		SetA8RegB(0x7151,GetA8RegB(0x7151) & (~(0x01 << gpio)));

	return;
}

u_char 	A8L_SensorRegTab(u_short *Init_Table,u_int TableLength)
{
	u_short	SCmd,SDat;
	u_char retVal = 0;
	u_int i;
 
	if(gA8I2Ctype)
	{
		for(i=0;i<(TableLength>>1);i+=2)
		{
			SCmd = *(Init_Table+i);
			SDat = *(Init_Table+i+1);
			if((SCmd == 0xffff) && (SDat == 0xffff))
				break;
			retVal = A8L_SetSenReg(SCmd,SDat);
			if(retVal)
			{
				AIT_Message_P0("!!!!!!!A8L_SetSenReg() A8_SYSTEM_ERROR 1");

				return A8_SYSTEM_ERROR;
			}
		}
	}
	else
	{
		for(i=0;i<TableLength>>1;i++)
		{
			SCmd = *(Init_Table+i);
			if(SCmd == 0xffff)
				break;
			retVal = A8L_SetSenReg(SCmd&0xff,SCmd>>8);
			if(retVal)
			{
				AIT_Message_P0("!!!!!!!A8L_SetSenReg() A8_SYSTEM_ERROR 2");

				return A8_SYSTEM_ERROR;
			}
		}
	}

	return A8_NO_ERROR;	
}

/**
 @fn		u_char A8L_PreInit_Sensor(void)
 @brief	PreInit Sensor
 @return	u_char -0: successfully, 1: failed. .
 @note	.
 @bug	N/A.
*/
u_char A8L_PreInit_Sensor(void)
{

	return A8_NO_ERROR;
}

/**
 @fn		u_char A8L_Init_Sensor()
 @brief	Init Sensor
 @return	u_char -0: successfully, 1: failed. .
 @note	.
 @bug	N/A.
*/
u_char A8L_Init_Sensor(void)
{
	u_short status;
	if(!gsSensorUsing)
		return A8_SYSTEM_ERROR;
	
	gsSensorUsing->sensor_enable(1);

	status = gsSensorUsing->sensor_setting(SENSOR_INIT);
	AIT_Message_P1("A8L_Init_Sensor1=%d\r\n",status);
	status = gsSensorUsing->sensor_setting(SENSOR_DSC_PREVIEW);
	AIT_Message_P1("A8L_Init_Sensor2=%d\r\n",status);	
	SetA8RegB(VIF_DATA_IN_EN,VIF_EN_DATA_IN);
	SetA8RegB(VIF_DATA_OUT_EN,VIF_EN_DATA_OUT);
	return A8_NO_ERROR;
}

u_char	A8L_SensorPowerOn(u_char on)
{
//Vin@20091210:Twin Sensor & TV
	extern sTVSensorPresent ait_tv_Sensor_Present;

	if(on==A8_OFF)
	{
		if(ait_tv_Sensor_Present.MasterSensor)
			ait_tv_Sensor_Present.MasterSensor->sensor_enable(0);
		if(ait_tv_Sensor_Present.SlaveSensor)
			ait_tv_Sensor_Present.SlaveSensor->sensor_enable(0);
		if(ait_tv_Sensor_Present.TV)
			ait_tv_Sensor_Present.TV->sensor_enable(0);
	}
	else
	{
		if(!gsSensorUsing)
			return A8_SYSTEM_ERROR;
		gsSensorUsing->sensor_enable(on);
	}

	return	A8_NO_ERROR;
}

/**
 @fn		u_char A8L_SensorEffect(u_char value)
 @brief	Setup Image Effect, these effects are all related to Sensor ISP.
 @param	value -The Effect ID.
 @return	u_char  .
 @note	These Effects depends on Sensor ISP characteristics.
 @bug	N/A.
*/
u_char A8L_SensorEffect(u_char value)
{
	if(!gsSensorUsing)
		return A8_SYSTEM_ERROR;
	return gsSensorUsing->sensor_effect(value);
}

/**
 @fn		u_char A8L_SensorBrightness(u_char value)
 @brief	Setup EV value.
 @param	value - the EV value.
 @return	u_char .
 @note	.
 @bug	N/A.
*/
u_char A8L_SensorBrightness( u_char value )
{
	if(!gsSensorUsing)
		return A8_SYSTEM_ERROR;
#ifdef EXIFSUPPORT
	sEXIFEVBias = value - 9;
#endif
	return gsSensorUsing->sensor_bright(value);
}

/**
 @fn		u_char 	A8L_SensorContrast(u_char value)
 @brief	Setup Contrast.
 @param	value - 
 @return	u_char
 @note	.
 @bug	N/A.
*/
u_char 	A8L_SensorContrast(u_char value)
{
	return A8_NO_ERROR;
}

/**
 @fn		u_char A8L_SensorWBMode(u_char mode)
 @brief	Setup White Balance mode.
 @param	mode - the White Balance mode.
 @return	u_char .
 @note	.
 @bug	N/A.
*/
u_char A8L_SensorWBMode(u_char mode)
{
	if(!gsSensorUsing)
		return A8_SYSTEM_ERROR;
	return gsSensorUsing->sensor_white_balance(mode);
}

/**
 @fn		u_char A8L_SensorNightMode(u_char mode)
 @brief	Enable/Disable Sensor Night mode.
 @param	mode - 0:Disable, 1:Active
 @return	u_char .
 @note	.
 @bug	N/A.
*/
u_char A8L_SensorNightMode(u_char mode)
{
	if(!gsSensorUsing)
		return A8_SYSTEM_ERROR;
	return gsSensorUsing->sensor_night_mode(mode); 	
}


/**
 @fn		u_char A8L_SensorDebandMode(u_char value)
 @brief	Set sensor De-band mode.
 @param	value : 0:No debanding, 1:60Hz deband, 2:50Hz deband
 @return	u_char
 @note	.
 @bug	N/A.
*/
u_char A8L_SensorDebandMode(u_char value)
{
	if(!gsSensorUsing)
		return A8_SYSTEM_ERROR;
	return gsSensorUsing->sensor_deband(value);
}

/**
 @fn		u_char 	A8L_SensorRotation(u_char value)
 @brief	Setup Sensor Direction.
 @param	value -Sensor Direction, 0:normal, 1:mirror, 2:vertical, 3:rotate
 @return	u_char
 @note	.
 @bug	N/A.
*/
u_char 	A8L_SensorRotation(u_char value)
{ 
	if(!gsSensorUsing)
		return A8_SYSTEM_ERROR;
	return gsSensorUsing->sensor_rotate(value);
}

/**
 @fn		u_char A8L_SensorFixedFrameRate( u_char frameRate )
 @brief	Set Sensor frame rate for Video operation.
 @param	frameRate - the Sensor frame rate.
 @return	u_char - 0x00:Success, 0x01:timeout failure .
 @note	The sensor frame rate for Video operation mostly depends on Host performance.
 @bug	N/A.
*/
u_char A8L_SensorFixedFrameRate( u_char frameRate )
{
	if(!gsSensorUsing)
		return A8_SYSTEM_ERROR;
	return gsSensorUsing->sensor_frame_rate(frameRate);
}
u_char	A8L_BeforeCapture(u_char Reso, u_char Zoom)
{
	if(!gsSensorUsing)
		return A8_SYSTEM_ERROR;
	return gsSensorUsing->sensor_setting(SENSOR_NORMAL_CAP_MODE);
}
u_char	A8L_AfterCapture(u_char Reso, u_char Zoom)
{
	if(!gsSensorUsing)
		return A8_SYSTEM_ERROR;
	return gsSensorUsing->sensor_setting(SENSOR_NORMAL_CAP_MODE);
}

/**
 @fn		u_char 	A8L_SensorMatrixMode(void)
 @brief	MATRIX Mode.
 @param	value - 
 @return	u_char
 @note	.
 @bug	N/A.
*/
u_char 	A8L_SensorMatrixMode(void)
{
	return A8_NO_ERROR;
}

/**
 @fn		u_char 	A8L_SensorCenterWeightedMode(void)
 @brief	CENTERWEIGHTED Mode.
 @param	value - 
 @return	u_char
 @note	.
 @bug	N/A.
*/
u_char 	A8L_SensorCenterWeightedMode(void)
{
	return A8_NO_ERROR;
}

/**
 @fn		u_char 	A8L_SensorSpotMode(void)
 @brief	SPOT Mode.
 @param	value - 
 @return	u_char
 @note	.
 @bug	N/A.
*/
u_char 	A8L_SensorSpotMode(void)
{
	return A8_NO_ERROR;
}
void A8L_SetSensorPowerPin(u_char level)
{
	extern u_short A800_SetGPIO(u_short gpio);
	extern u_short A800_ClearGPIO(u_short gpio);
#ifndef AIT701_DEMO_BOARD
	if(level){//Turn on Sensor power		
        	A800_SetGPIO(AIT_GPIO_SENSOR_POWER_CTL);		
	}else{
		A800_ClearGPIO(AIT_GPIO_SENSOR_POWER_CTL);
	}
#endif	
	return;	
}

void A8L_SetSensorEnablePin(u_char level)
{
	

#ifdef __TWO_SENSOR__
	extern kal_uint8 g_which_sensor_in_use;
	if(level == A8_OFF) //Work
	{
		if(g_which_sensor_in_use == BACK_SENSOR)
		{
			sys_ait_sub_sensor_pdn(A8_ON);
			A8L_SetSensorIF(A8_SENSOR_SEN_PIN, A8_OFF);
		}
		else
		{
			sys_ait_sub_sensor_pdn(A8_OFF);
			A8L_SetSensorIF(A8_SENSOR_SEN_PIN, A8_ON);
		}
	}
	else //pdn
	{
		sys_ait_sub_sensor_pdn(A8_ON);
		A8L_SetSensorIF(A8_SENSOR_SEN_PIN, A8_ON);
	}
#else
	A8L_SetSensorIF(A8_SENSOR_SEN_PIN, level);
#endif
	return ;	
}

void A8L_SetSensorResetPin(u_char level)
{
	A8L_SetSensorIF(A8_SENSOR_RST_PIN, level);
	return ;	
}

void A8L_VIF_Init(t_sensor_mode_attr* sensor_mode)
{
	SetA8RegB(0x6007,GetA8RegB(0x6007)|0x40);
	SetA8RegB(0x690a,0x20);
	SetA8RegB(0x690a,0x00);
	SetA8RegB(VIF_OPR_UPDATE_CTL,VIF_UPDATE_OPR_EN);
	SetA8RegB(VIF_INT_EN_HOST,0);
	SetA8RegB(VIF_INT_MODE,0x01);
	if(0)//sensor_mode->clk_div==A8_PHASE_COUNTER_NONE)
	{
		SetA8RegB(VIF_SEN_CLK_DIV,3);
		SetA8RegB(VIF_SEN_CLK_CTL,0);
	}
	else
	{
		SetA8RegB(VIF_SEN_CLK_DIV,sensor_mode->clk_div);
		SetA8RegB(VIF_SEN_CLK_CTL,SEN_CLK_OUTPUT);
	}
	SetA8RegB(VIF_DATA_IN_EN,0);
	SetA8RegB(VIF_DATA_OUT_EN,VIF_EN_DATA_OUT);	
	SetA8RegB(VIF_OPR_UPDATE_CTL,VIF_UPDATE_OPR_EN|VIF_UPDATE_OPR_FRAME_SYNC);	
	return ;	
}

u_short	A8L_SetSensorAttr(t_sensor_mode_attr* sensor_mode)
{
	SetA8RegW(VIF_GRAB_START_X,sensor_mode->image_start_x);
	SetA8RegW(VIF_GRAB_END_X,sensor_mode->image_start_x
								+sensor_mode->image_width -1);
	SetA8RegW(VIF_GRAB_START_Y,sensor_mode->image_start_y);
	SetA8RegW(VIF_GRAB_END_Y,sensor_mode->image_start_y
								+sensor_mode->image_height -1);
	if( (gsSensorUsing->sensor_id!=NULL)&&(gsSensorUsing->sensor_id==ait_tv_Sensor_Present.TV->sensor_id))
	{
		SetA8RegW(VIF_LINE_INT_NUM,sensor_mode->image_height);								
		SetA8RegB(VIF_SEN_GEN_CTL,(sensor_mode->latch_edge<<7)|(sensor_mode->hv_polarity));
		SetA8RegB(VIF_SEN_YUV_CTL,SEN_YUV_EN|(sensor_mode->yuv_format <<4));
#if defined(AIT_ATV_RDA5888)&&(AIT_ATV_RDA5888==1)
		SetA8RegB(VIF_PXL_CLK_DELAY,0x03); // tune for RDA5888
#endif
	}
	else
		SetA8RegW(VIF_LINE_INT_NUM,sensor_mode->image_height);// - 0x20);	 					

	//								
	SetA8RegB(VIF_SEN_GEN_CTL,(sensor_mode->latch_edge<<7)|(sensor_mode->hv_polarity));
	SetA8RegB(VIF_SEN_YUV_CTL,SEN_YUV_EN|(sensor_mode->yuv_format <<4));


//	SetA8RegW(0x6048,0x8080);
//	SetA8RegW(0x604a,0x8080);
	return A8_NO_ERROR;
}


/**
 @fn		u_char 	A8L_SendSensorSettingTable(u_short* tbl)
 @brief	SPOT Mode.
 @param	value - 
 @return	u_char
 @note	.
 @bug	N/A.
*/

u_char	A8L_SendSensorSettingTable(u_short* tbl,u_short table_length)
{
	u_short	Saddr,Sdata;
	u_short	i;//table_length,i;
	if(!gsSensorUsing)
		return A8_SYSTEM_ERROR;	
//	table_length = sizeof(tbl);
	table_length >>= 1;
	for(i=0;i<table_length;){
		Saddr = *tbl++;
		i++;
		if(A8_I2C_1A1D == gsSensorUsing->i2c_type){
			Sdata = Saddr >> 8;
		}else{
			Sdata = *tbl++;
			i++;
		}
		if(A8L_SetSenReg(Saddr,Sdata))
			return	A8_SYSTEM_ERROR;
	}
	return	A8_NO_ERROR;
}

/**
 * @file A8_sensor.h
 * @brief Copyright (C) 2002-2008 Alpha Imaging Technology Corp.
 * @n AIT701/703 Series Host API.
 * @n The purpose of AIT701 Host API is for Host CPU to control and communicate AIT701
 *      chip via direct/indirect parallel interface.
 * @note N/A
 * @bug N/A
 */
 
#ifndef	AIT_SENSOR_H
#define	AIT_SENSOR_H

#define	I2C_1A1D	(0)
#define	I2C_1A2D	(1)
#define	I2C_2A1D	(2)
#define	I2C_2A2D	(3)

#define	A8_I2C_1A1D	(0)
#define	A8_I2C_1A2D	(1)
#define	A8_I2C_2A1D	(2)
#define	A8_I2C_2A2D	(3)

#define A8_SENSOR_HOR_POS   0x00
#define A8_SENSOR_HOR_NEG   0x02
#define A8_SENSOR_VER_POS   0x00
#define A8_SENSOR_VER_NEG   0x01

#define A8_SENSOR_POS_EDGE  0x00
#define A8_SENSOR_NEG_EDGE  0x01

#define A8_SENSOR_YCBYCR    0x00
#define A8_SENSOR_YCRYCB    0x01
#define A8_SENSOR_CBYCRY    0x02
#define A8_SENSOR_CRYCBY    0x03

#define A8_PHASE_COUNTER_NONE			0xff
#define A8_PHASE_COUNTER_NUM_16         0x0f
#define A8_PHASE_COUNTER_NUM_8          0x07
#define A8_PHASE_COUNTER_NUM_4          0x03
#define A8_PHASE_COUNTER_NUM_3          0x02
#define A8_PHASE_COUNTER_NUM_2          0x01
#define A8_PHASE_COUNTER_NUM_1          0x00
//=============================//
//  VIF_INT_ST_HOST : 0x01     //
//=============================//
#define	VIF_LINE_INT				0x08
#define	VIF_FRAME_START				0x04
#define	VIF_FRAME_END				0x02
#define	VIF_GRAB_END				0x01

#define	A8_SENSOR_SEN_PIN		0
#define	A8_SENSOR_SCK_PIN		1
#define	A8_SENSOR_SDA_PIN	2
#define	A8_SENSOR_RST_PIN		3

#define	SENSOR_PIX_PHASE_CNT_B	(0x6000)
#define	SENSOR_CLK_CTL_B				(0x6005)
#define	SENSOR_LATCH_PHASE_B		(0x6006)
#define	TG_GRAB_H_START_W			(0x6008)
#define	TG_GRAB_H_END_W				(0x600A)
#define	TG_GRAB_V_START_W			(0x600C)
#define	TG_GRAB_V_END_W				(0x600E)	

#define	SENSOR_MODE_CTL_B			(0x6010)
#define	SENSOR_TIMING_CTL_B			(0x601D)
#define	SENSOR_CLK_DIV_B				(0x601E)
#define	TG_VIEW_FINDER_CTL_W		(0x6020)
#define	TG_PRE_EXPO_LINE_W			(0x6038)
#define	TG_YUV_PATH_CTL_B				(0x603A)
#define	TG_CPU_INT_EN_B					(0x6042)
#define	TG_CPU_INT_ST_B					(0x6043)	

//=========================//
//  SENSOR_MODE_CTL_B : 0x10  //
//=========================//
#define  SENSOR_VSYNC_POLARITY_POSITIVE		0x00
#define  SENSOR_VSYNC_POLARITY_NEGATIVE	0x80
#define  SENSOR_HSYNC_POLARITY_POSITIVE		0x00
#define  SENSOR_HSYNC_POLARITY_NEGATIVE	0x40

//=========================//
//  TG_ADCLK_CTL_B : 0x1D  //
//=========================//
#define  SENSOR_MCLK_ENABLE	0x80
#define  SENSOR_MCLK_POLARITY	0x40
#define  PCLK_FALLING_LATCH		0x00
#define  PCLK_RISING_LATCH		0x02
#define  LATCH_DATA_BY_PCLK_ENABLE  0x01

//==============================//
//  TG_YUV_PATH_CTL_B : 0x3A //
//==============================//
#define  TG_HREF_POLARITY						0x80
#define  YUV_OUTPUT_Y_CB_Y_CR			0x00
#define  YUV_OUTPUT_Y_CR_Y_CB			0x10
#define  YUV_OUTPUT_CB_Y_CR_Y			0x20
#define  YUV_OUTPUT_CR_Y_CB_Y			0x30
#define  TG_YUV_PATH_ENABLE				0x01
/*
#ifdef AUTO_DETECT_SENSOR
u_char  (*fvSensorRotate)(u_char Dir);
u_char  (*fvSensorEffect)(u_char Effect);
u_char  (*fvSensorWhiteBalance)(u_char WhiteBalance);
u_char  (*fvSensorBright)(u_char Bright);
u_char  (*fvSensorNightMode)(u_char Enable);
u_char  (*fvSensorDeband)(u_char Enable);
u_char  (*fvBeforeCapture)(u_char Reso, u_char Zoom);
u_char  (*fvAfterCapture)(u_char Reso, u_char Zoom);
u_char  (*fvSensorPowerOn)(u_char on);
u_char  (*fvSensorVideo)(u_char bEnable);
#else
extern u_char  (*fvSensorRotate)(u_char Dir);
extern u_char  (*fvSensorEffect)(u_char Effect);
extern u_char  (*fvSensorWhiteBalance)(u_char WhiteBalance);
extern u_char  (*fvSensorBright)(u_char Bright);
extern u_char  (*fvSensorNightMode)(u_char Enable);
extern u_char  (*fvSensorDeband)(u_char Enable);
extern u_char  (*fvBeforeCapture)(u_char Reso, u_char Zoom);
extern u_char  (*fvAfterCapture)(u_char Reso, u_char Zoom);
extern u_char  (*fvSensorPowerOn)(u_char on);
#endif
*/
//Vin@20091210:Twin Sensor & TV
typedef struct
{
	t_sensor_manager* MasterSensor;
	t_sensor_manager* SlaveSensor;
	t_sensor_manager* TV;
//	u_short	(*MasterSensorPowerEnable)(u_short enable);

//	u_short	(*SlaveSensorPowerEnable)(u_short enable);
//	u_short	(*TVPowerEnable)(u_short enable);
}sTVSensorPresent;

typedef enum
{
	SENSOR_TYPE_OV7670 		= 0,
	SENSOR_TYPE_OV7680,
	SENSOR_TYPE_OV7690,
	SENSOR_TYPE_MC501CB,
	SENSOR_TYPE_LSI_S5KA3DFX,
	SENSOR_TYPE_SIV100B,
	SENSOR_TYPE_GC0307,
	SENSOR_TYPE_MAX_NUMBER
}SENSOR_TYPE;

extern u_char 	A8L_SensorRegTab(u_short *Init_Table,u_int TableLength);
extern u_short A8L_I2CInit(u_char I2Ctype, u_char I2Cid);
extern u_short A8L_SensorClockOutput(u_char Enable);

extern u_short A8L_SetSenReg(u_short Reg ,u_short Value);
extern u_short A8L_GetSenReg(u_short Reg ,u_short *Value);
extern u_char A8L_GetSenRegB(u_char Reg ,u_char *Value);

extern u_char A8L_Init_Sensor( void );
extern u_char A8L_SensorEffect(u_char value);
extern u_char A8L_SensorBrightness( u_char value );
extern u_char A8L_SensorContrast(u_char value);
extern u_char A8L_SensorWBMode(u_char mode);
extern u_char A8L_SensorNightMode(u_char mode);
extern u_char A8L_SensorDebandMode(u_char value);
extern u_char A8L_SensorRotation(u_char value);
extern u_char A8L_SensorFixedFrameRate( u_char frameRate );
extern void A8L_SetSensorPowerPin(u_char level);
extern void A8L_SetSensorEnablePin(u_char level);
extern void A8L_SetSensorResetPin(u_char level);
extern void A8L_SetSensorIF(u_char gpio, u_char  enable);
extern u_char A8L_CheckFrameStart(void);
extern u_short A8L_CheckSensorFrame(u_char mode);
extern u_char A8L_CheckExposureDone(void);
extern u_char A8L_SensorPowerOn(u_char on);
extern u_char A8L_PreInit_Sensor(void);
extern u_char A8L_BeforeCapture(u_char Reso, u_char Zoom);
extern u_char A8L_AfterCapture(u_char Reso, u_char Zoom);
extern u_char A8L_SensorMatrixMode(void);
extern u_char A8L_SensorCenterWeightedMode(void);
extern u_char A8L_SensorSpotMode(void);

#if defined(SENSOR_OV7670)
u_char	A8L_Sensor_Rotate_OV7670(u_char	Dir);
u_char	A8L_Sensor_Effect_OV7670(u_char Effect);
u_char	A8L_Sensor_WhiteBalance_OV7670(u_char WhiteBalance);
u_char	A8L_Sensor_Bright_OV7670(u_char Bright);
u_char	A8L_Sensor_NightMode_OV7670(u_char Enable);
u_char	A8L_Sensor_Deband_OV7670(u_char Bandtype);
u_char	A8L_BeforeCapture_OV7670(u_char Reso, u_char Zoom);
u_char	A8L_AfterCapture_OV7670(u_char Reso, u_char Zoom);
u_char	A8L_SensorPowerOn_OV7670(u_char on);
#endif

#if defined(SENSOR_OV7690)
u_char	A8L_Sensor_Rotate_OV7690(u_char	Dir);
u_char	A8L_Sensor_Effect_OV7690(u_char Effect);
u_char	A8L_Sensor_WhiteBalance_OV7690(u_char WhiteBalance);
u_char	A8L_Sensor_Bright_OV7690(u_char Bright);
u_char	A8L_Sensor_NightMode_OV7690(u_char Enable);
u_char	A8L_Sensor_Deband_OV7690(u_char Bandtype);
u_char	A8L_BeforeCapture_OV7690(u_char Reso, u_char Zoom);
u_char	A8L_AfterCapture_OV7690(u_char Reso, u_char Zoom);
u_char	A8L_SensorPowerOn_OV7690(u_char on);
u_char A8L_Sensor_Video_OV7690(void);
#endif

#if defined(SENSOR_MC501CB)
u_char	A8L_Sensor_Rotate_MC501CB(u_char	Dir);
u_char	A8L_Sensor_Effect_MC501CB(u_char Effect);
u_char	A8L_Sensor_WhiteBalance_MC501CB(u_char WhiteBalance);
u_char	A8L_Sensor_Bright_MC501CB(u_char Bright);
u_char	A8L_Sensor_NightMode_MC501CB(u_char Enable);
u_char	A8L_Sensor_Deband_MC501CB(u_char Bandtype);
u_char	A8L_BeforeCapture_MC501CB(u_char Reso, u_char Zoom);
u_char	A8L_AfterCapture_MC501CB(u_char Reso, u_char Zoom);
u_char	A8L_SensorPowerOn_MC501CB(u_char on);
#endif
#if defined(SENSOR_LSI_S5KA3DFX)
u_char	A8L_Sensor_Rotate_S5KA3DFX(u_char	Dir);
u_char	A8L_Sensor_Effect_S5KA3DFX(u_char Effect);
u_char	A8L_Sensor_WhiteBalance_S5KA3DFX(u_char WhiteBalance);
u_char	A8L_Sensor_Bright_S5KA3DFX(u_char Bright);
u_char	A8L_Sensor_NightMode_S5KA3DFX(u_char Enable);
u_char	A8L_Sensor_Deband_S5KA3DFX(u_char Bandtype);
u_char	A8L_BeforeCapture_S5KA3DFX(u_char Reso, u_char Zoom);
u_char	A8L_AfterCapture_S5KA3DFX(u_char Reso, u_char Zoom);
u_char	A8L_SensorPowerOn_S5KA3DFX(u_char on);
#endif
#if defined(SENSOR_SIV100B)
u_char	A8L_PreInit_Sensor_SIV100b(void);
u_char	A8L_Sensor_Rotate_SIV100b(u_char	Dir);
u_char	A8L_Sensor_Effect_SIV100b(u_char Effect);
u_char	A8L_Sensor_WhiteBalance_SIV100b(u_char WhiteBalance);
u_char	A8L_Sensor_Bright_SIV100b(u_char Bright);
u_char	A8L_Sensor_NightMode_SIV100b(u_char Enable);
u_char	A8L_Sensor_Deband_SIV100b(u_char Bandtype);
u_char	A8L_BeforeCapture_SIV100b(u_char Reso, u_char Zoom);
u_char	A8L_AfterCapture_SIV100b(u_char Reso, u_char Zoom);
u_char	A8L_SensorPowerOn_SIV100b(u_char on);
#endif
#if defined(SENSOR_GC0307)
extern u_char	A8L_PreInit_Sensor_GC0307(void);
extern u_char	A8L_Sensor_Rotate_GC0307(u_char	Dir);
extern u_char	A8L_Sensor_Effect_GC0307(u_char Effect);
extern u_char	A8L_Sensor_WhiteBalance_GC0307(u_char WhiteBalance);
extern u_char	A8L_Sensor_Bright_GC0307(u_char Bright);
extern u_char	A8L_Sensor_NightMode_GC0307(u_char Enable);
extern u_char	A8L_Sensor_Deband_GC0307(u_char Bandtype);
extern u_char	A8L_BeforeCapture_GC0307(u_char Reso, u_char Zoom);
extern u_char	A8L_AfterCapture_GC0307(u_char Reso, u_char Zoom);
extern u_char	A8L_SensorPowerOn_GC0307(u_char on);
extern u_char	A8L_Sensor_FixedFrameRate_GC0307(u_char bEnable);
#endif

#endif

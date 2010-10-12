/**
 * @file A8_common.h
 * @brief Copyright (C) 2002-2008 Alpha Imaging Technology Corp.
 * @n AIT701/703 Series Host API.
 * @n The purpose of AIT701 Host API is for Host CPU to control and communicate AIT701
 *      chip via direct/indirect parallel interface.
 * @note N/A
 * @bug N/A
 */
#if !defined (AIT_COMMON_H)
#define AIT_COMMON_H

#ifdef WIN32
#define MTK_PLATFORM
#define __align(x)
#endif

#include <stdio.h>
#include "assert.h"
typedef unsigned char		bool;
typedef unsigned char		u_char;
typedef unsigned short		u_short;
typedef unsigned int			u_int;

typedef int                 		s_int;
typedef short               	s_short;
typedef char                	s_char;

typedef unsigned char kal_uint8;
typedef unsigned short kal_uint16;
typedef unsigned int kal_uint32;
typedef char kal_int8;


typedef enum 
{
  KAL_FALSE,
  KAL_TRUE
} kal_bool;


#ifndef TRUE
#define TRUE  1
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef NULL
#define NULL	0
#endif

//===========================
// Chip type
//===========================
#define AIT701U


//===========================
// Baseband platform
//===========================
#ifndef __QSC_TARGET__
#define __QSC_TARGET__
#endif

#if defined(__MTK_TARGET__)
#include "emi_hw.h"
#include "math.h"
#include "Reg_base.h"
#include "kal_release.h"
#include "stack_types.h"
#include "syscomp_config.h"
#include "custom_config.h"
#include "drv_comm.h"
#include "reg_base.h" 
#include "emi_hw.h"
#include "msdc_def.h"
#include "sd_def.h"
#include "fat_fs.h"
#include "ext_camera.h"
#include "med_api.h"
#include "gpio_sw.h"

#define MTK_PLATFORM
#define EXTCLK_26M
#elif defined(__QSC_TARGET__)
#include "msmhwioreg.h"
#include "clk.h"
#include "assert.h"

#define EXTCLK_19M2
#define EBI_BUS_8BIT_MODE
#else
#define EXTCLK_26M
//#define EBI_BUS_8BIT_MODE
//#define _FW_FROM_PC_
#endif


//===========================
// Sensor I/F
//===========================
#define	SENSOR_VIF

//===========================
// Host I/F
//===========================
#define DEMO_BOARD_VER (1) 




typedef enum
{
	SENSOR_INIT = 0,
	SENSOR_DSC_PREVIEW,
	SENSOR_VIDEO_PREVIEW,
	SENSOR_NORMAL_CAP_MODE,
	SENSOR_FULL_RESOL_MODE,
	SENSOR_SCALE_UP_MODE
}mmp_sensor_mode;

typedef enum
{
	A8_SENSOR_NIGHT_MODE_OFF = 0,
	A8_SENSOR_NIGHT_MODE_ON
}mmp_sensor_night_mode;

typedef enum
{
	A8_SENSOR_DEBAND_50 = 0,
	A8_SENSOR_DEBAND_60
}mmp_sensor_deband;

typedef enum
{
	A8_SENSOR_ROTATE_NORMAL = 0,
	A8_SENSOR_ROTATE_MIRROR,
	A8_SENSOR_ROTATE_FILP,
	A8_SENSOR_ROTATE_MIRROR_FLIP
}mmp_sensor_rotate;

typedef struct _t_sensor_mode_attr
{
	u_short	hv_polarity;
	u_short	latch_phase;
	u_short	yuv_format;
	u_short	latch_edge;
	u_short	image_start_x;
	u_short image_start_y;
	u_short	image_width;
	u_short	image_height;
	u_short	wait_count;
	u_short	clk_div;	
} t_sensor_mode_attr ;
typedef struct _t_sensor_manager
{
    u_short sensor_id;
	u_short	i2c_id;
	u_short	i2c_type;
	t_sensor_mode_attr	*preview_mode;
	t_sensor_mode_attr	*full_mode;
	u_short	(*sensor_setting)(mmp_sensor_mode sensor_mode);
	void	(*sensor_enable)(u_short enable);
	u_short	(*sensor_get_id)(u_short *sensorID);
	u_short	(*sensor_effect)(u_short effect);
	u_short	(*sensor_bright)(u_short bright);
	u_short	(*sensor_white_balance)(u_short WB);
	u_short	(*sensor_night_mode)(mmp_sensor_night_mode enable);
	u_short	(*sensor_deband)(mmp_sensor_deband deband);
	u_short	(*sensor_rotate)(mmp_sensor_rotate rotate);
	u_short	(*sensor_frame_rate)(u_short frame_rate);
} t_sensor_manager ;


typedef struct 
{
    u_short sensor_id;
	u_short	i2c_id;
	u_short	i2c_type;
	t_sensor_mode_attr	*preview_mode;
	t_sensor_mode_attr	*full_mode;
	u_short	(*sensor_setting)(mmp_sensor_mode sensor_mode);
	void	(*sensor_enable)(u_short enable);
	u_short	(*sensor_get_id)(u_short *sensorID);
	u_short	(*sensor_effect)(u_short effect);
	u_short	(*sensor_bright)(u_short bright);
	u_short	(*sensor_white_balance)(u_short WB);
	u_short	(*sensor_night_mode)(mmp_sensor_night_mode enable);
	u_short	(*sensor_deband)(mmp_sensor_deband deband);
	u_short	(*sensor_rotate)(mmp_sensor_rotate rotate);
	u_short	(*sensor_frame_rate)(u_short frame_rate);
} t_ait_tv_manager ;


typedef struct 
{
	t_sensor_manager* pAIT_VIF_Src_MasterSensor;
	t_sensor_manager* pAIT_VIF_Src_SlaveSensor;
	t_ait_tv_manager* pAIT_VIF_Src_TV;

}t_AIT_VIF_Src;
//===========================
// Debug Message
//===========================
#if defined(__MTK_TARGET__)
#include "kal_release.h"

#if 0
#define AIT_Message_P0(s) kal_prompt_trace(MOD_NIL, s)
#define AIT_Message_P1(s, a1) kal_prompt_trace(MOD_NIL, s, a1)
#define AIT_Message_P2(s, a1, a2) kal_prompt_trace(MOD_NIL, s, a1, a2)
#define AIT_Message_P3(s, a1, a2, a3) kal_prompt_trace(MOD_NIL, s, a1, a2, a3)
#define AIT_Message_P4(s, a1, a2, a3, a4) kal_prompt_trace(MOD_NIL, s, a1, a2, a3, a4)
#define AIT_Message_P5(s, a1, a2, a3, a4, a5) kal_prompt_trace(MOD_NIL, s, a1, a2, a3, a4, a5)
#define AIT_Message_P6(s, a1, a2, a3, a4, a5, a6) kal_prompt_trace(MOD_NIL, s, a1, a2, a3, a4, a5, a6)
#define AIT_Message_P7(s, a1, a2, a3, a4, a5, a6, a7) kal_prompt_trace(MOD_NIL, s, a1, a2, a3, a4, a5, a6, a7)
#define AIT_Message_P8(s, a1, a2, a3, a4, a5, a6, a7, a8) kal_prompt_trace(MOD_NIL, s, a1, a2, a3, a4, a5, a6, a7, a8)
#define AIT_Message_Error(s, a1) kal_prompt_trace(MOD_NIL, s, a1)

#else
extern char  SysMsg[128];
#define    SYS_DBG(_X_) { sprintf _X_; PutUARTBytes(0, SysMsg, strlen(SysMsg));}
#define AIT_Message_P0(s) SYS_DBG((SysMsg,s))
#define AIT_Message_P1(s, a1) SYS_DBG((SysMsg,s,a1))
#define AIT_Message_P2(s, a1, a2) SYS_DBG((SysMsg,s,a1,a2))
#define AIT_Message_P3(s, a1, a2, a3) SYS_DBG((SysMsg,s,a1,a2,a3))
#define AIT_Message_P4(s, a1, a2, a3, a4) SYS_DBG((SysMsg,s,a1,a2,a3,a4))
#define AIT_Message_P5(s, a1, a2, a3, a4, a5) SYS_DBG((SysMsg,s,a1,a2,a3,a4,a5))
#define AIT_Message_P6(s, a1, a2, a3, a4, a5, a6) SYS_DBG((SysMsg,s,a1,a2,a3,a4,a5,a6))
#define AIT_Message_P7(s, a1, a2, a3, a4, a5, a6, a7) SYS_DBG((SysMsg,s,a1,a2,a3,a4,a5,a6,a7))
#define AIT_Message_P8(s, a1, a2, a3, a4, a5, a6, a7, a8) SYS_DBG((SysMsg,s,a1,a2,a3,a4,a5,a6,a7,a8))
#define AIT_Message_Error(s, a1) SYS_DBG((SysMsg,s,a1))
#endif

#elif defined(__QSC_TARGET__)
#define AIT_Message_P0(s) DBGPRINTF("%s",s)
#define AIT_Message_P1(s, a1) DBGPRINTF("%s",s, a1)
#define AIT_Message_P2(s, a1, a2) DBGPRINTF("%s",s, a1, a2)
#define AIT_Message_P3(s, a1, a2, a3) DBGPRINTF("%s",s, a1, a2, a3)
#define AIT_Message_P4(s, a1, a2, a3, a4) DBGPRINTF("%s",s, a1, a2, a3, a4)
#define AIT_Message_P5(s, a1, a2, a3, a4, a5) DBGPRINTF("%s",s, a1, a2, a3, a4, a5)
#define AIT_Message_P6(s, a1, a2, a3, a4, a5, a6) DBGPRINTF("%s",s, a1, a2, a3, a4, a5, a6)
#define AIT_Message_P7(s, a1, a2, a3, a4, a5, a6, a7) DBGPRINTF("%s",s, a1, a2, a3, a4, a5, a6, a7)
#define AIT_Message_P8(s, a1, a2, a3, a4, a5, a6, a7, a8) DBGPRINTF("%s",s, a1, a2, a3, a4, a5, a6, a7, a8)
#define AIT_Message_Error(s, a1) DBGPRINTF("%s",s, a1)
#else
#define AIT_Message_P0(s)  printf(s)
#define AIT_Message_P1(s, a1) printf(s, a1)
#define AIT_Message_P2(s, a1, a2) printf(s, a1, a2)
#define AIT_Message_P3(s, a1, a2, a3) printf(s, a1, a2, a3)
#define AIT_Message_P4(s, a1, a2, a3, a4) printf(s, a1, a2, a3, a4)
#define AIT_Message_P5(s, a1, a2, a3, a4, a5) printf(s, a1, a2, a3, a4, a5)
#define AIT_Message_P6(s, a1, a2, a3, a4, a5, a6) printf(s, a1, a2, a3, a4, a5, a6)
#define AIT_Message_P7(s, a1, a2, a3, a4, a5, a6, a7) printf(s, a1, a2, a3, a4, a5, a6, a7)
#define AIT_Message_P8(s, a1, a2, a3, a4, a5, a6, a7, a8) printf(s, a1, a2, a3, a4, a5, a6, a7, a8)
#define AIT_Message_Error(s, a1)  printf( s, a1)
#endif

//===========================
// Video function definition
//===========================
#define A8_AVI_VIDEO_SUPPORT_ENABLE
//#define AIT_VIDEO_PHONE_SUPPORT (1)

//===========================
// Other common definitions
//===========================
#define A8_SWITCH_PREVIEW_TIME_OUT				(500)
#define A8_CMD_READY_TIME_OUT						(10000)
#define A8_SENSOR_FRAME_START_TIME_OUT	(400)
#define A8_SENSOR_EXP_DONE_TIME_OUT			(400)
#define A8_HOST_CMD_RG_TIME_OUT					(1000)
#define A8_LCD_UPDATE_TIME_OUT						(500)
#define A8_JPEG_GET_PIC_TIME_OUT					(10000)
#define A8_JPEG_ENCODEDONE_TIME_OUT			(0x40000)
#define A8_JPEG_DECODE_TIME_OUT						(500)

#define	A8_MAIN_WINDOW			0x00
#define	A8_PIP_WINDOW			0x01
#define	A8_OVERLAY_WINDOW	0x02
#define	A8_GAME_WINDOW		0x03

#define	A8_RT_NORMAL		0x00
#define	A8_RT_RIGHT_90	0x01
#define	A8_RT_RIGHT_180	0x02
#define	A8_RT_RIGHT_270	0x03
#define	A8_RT_NO_MIRROR	0x00
#define	A8_RT_MIRROR		0x04

#define	A8_DEPTH_8B			0x00
#define	A8_DEPTH_16B		0x01
#define	A8_DEPTH_24B		0x02
#define	A8_YUV_422				0x03
#define	A8_DEPTH_8I			0x04
#define	A8_DEPTH_4I			0x05

#define A8_TG_INT_PREEXPOSUREDONE (1<<3)
#define A8_TG_INT_FRAMESTART 			(1<<2)
#define A8_TG_INT_SNAPDONE 				(1<<2)
#define A8_TG_INT_EXPOSUREDONE 		(1<<0)

#define A8_ACTIVE_LOW	0
#define A8_ACTIVE_HIGH	1

#define A8_OFF	FALSE
#define A8_ON		TRUE

#define	A8_ALL			0
#define	A8_CAM			1
#define	A8_SD			2
#define	A8_USB			3

#define A8_MAIN_LCD		0
#define A8_SUB_LCD		1

typedef enum
{
	AIT_GPIO_PULL_DONT_CARE, 
	AIT_GPIO_PULL_UP,
	AIT_GPIO_PULL_DOWN,
	AIT_GPIO_PULL_FLOATING
}eAIT_GPIO_MODE;

typedef	struct	A800CameraParam
{
	u_short SrcWidth;
	u_short SrcHeight;
	u_short JpgWidth;
	u_short JpgHeight;
	u_char  ScaleN;
	u_char  ScaleM;
	u_short startx;
	u_short endx;
	u_short starty;
	u_short endy;
	u_int     Length;
	u_char	Mode;
#if 1 //defined(DECODE_EXTEND)
	unsigned short	SrcX;
	unsigned short	SrcY;
	unsigned char    bFrontGrab;
#endif
}A8S_CameraParam;


typedef struct
{
	u_short x;
	u_short y;
	u_short w;
	u_short h;
} RECT_AIT;

typedef struct
{
	u_short x;
	u_short y;
}POINT_AIT;

// Full: Full Screen
typedef enum{
	AIT_CAM_PREV_NOR_MODE = 0x000,
	AIT_CAM_PREV_FULL_MODE,
	AIT_CAM_STICKER_MODE,
	
	AIT_JPG_PLAY_NOR_MODE = 0x100,
	AIT_JPG_PLAY_FULL_MODE,		
	
	AIT_VDO_PREV_MODE = 0x200,
	AIT_VDO_REC_MODE,
	AIT_VDO_PLAY_NOR_MODE,
	AIT_VDO_PLAY_FULL_MODE,
	AIT_VDO_PLAY_YUVBUF_NOR_MODE,
	AIT_VDO_PLAY_YUVBUF_FULL_MODE,
		
	
	AIT_ATV_PREV_NOR_MODE = 0x300,
	AIT_ATV_PREV_FULL_MODE,	
	AIT_ATV_REC_NOR_MODE,
	AIT_ATV_REC_FULL_MODE,	

	AIT_USB_WEBCAM_MODE = 0x400,
//#if AIT_VIDEO_PHONE_SUPPORT
	AIT_VIDPHONE_PREV_MODE,
//#endif
	
	AIT_PREV_MAX_NUMBER
}PREVIEW_MODE;


typedef enum{
	A8_USB_PCCAM	= 0x00,
	A8_USB_MSDC		= 0x01,
	A8_USB_VCOM		= 0x04,

	A8_USB_MAX_NUMBER
}A8_USB_MODE;

typedef enum{
	A8_NO_ERROR	= 0x0000,
	MMP_SUCCESS = A8_NO_ERROR,
	MMP_FAILED,
	MMP_INVALID_STATE,
	MMP_NO_SENSOR,
	MMP_INVALID_SIZE,
		
	A8_SYSTEM_ERROR,
	A8_TIMEOUT_ERROR,
	A8_DOWNLOAD_FW_ERROR,
	A8_OUT_OF_RANGE_ERROR,
	A8_INCORRECT_PARA_ERROR,	//5

	A8_UNSUPPORT_ERROR,
	A8_CAM_PREV_START_ERROR = 0x1000,
	A8_CAM_PREV_STOP_ERROR,
	A8_CAM_CONFIG_ERROR,
	A8_CAM_CAPTURE_ERROR,	//10
	
	A8_CAM_DECODE_ERROR,
	A8_CAM_ENCODE_ERROR,
	A8_CAM_FIFO_EMPTY_ERROR,
	A8_CAM_FIFO_OVERFLOW_ERROR,

	A8_VDO_RECORD_ERROR = 0x2000,		//15
	A8_VDO_DECODE_ERROR,

	A8_USB_ERROR = 0x3000,
	A8_USB_DLL_ERROR,
	
	A8_SD_ERROR	=	0x4000,
	A8_SD_SIZE_ERROR,
	A8_SD_ADDR_ERROR
}A8_ERROR_MSG;

extern void sys_IF_ait_delay1ms(u_int time);
extern void sys_IF_ait_delay1us(u_int time);
#endif

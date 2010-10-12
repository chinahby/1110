#ifndef	_PLATFORM_H_
#define	_PLATFORM_H_
#include "AIT_Interface.h"
/*****************************************************************************/

#define	PIXEL_STEP				4

#define	H_OFFSET_525			(8/PIXEL_STEP)
#define	H_REDUCE_525			(24/PIXEL_STEP)
#define	V_OFFSET_525			(0/PIXEL_STEP)
#define	V_REDUCE_525			(0/PIXEL_STEP)

#define	H_OFFSET_625			(0/PIXEL_STEP)
#define	H_REDUCE_625			(0/PIXEL_STEP)
#define	V_OFFSET_625			(0/PIXEL_STEP)
#define	V_REDUCE_625			(0/PIXEL_STEP)

#define	H_OFFSET_RANGE		16
#define	H_REDUCE_RANGE		16
#define	V_OFFSET_RANGE		16
#define	V_REDUCE_RANGE		16

#define	WYSIWYG				1	// What You See Is What You Get
//#define ATV_POWER_USE_VMC //added by jiajin 2009-10-20 tv 2.8v use vmc

#define CHANNEL_SWITCH_EFFECT	1

#define USE_SYNC_CONTROL		0
#define USE_MUTE_CONTROL		1	/*modified by macro lei 2008/08/19*/
#define USE_PA_CONTROL			1	/*modified by macro lei 2008/08/19*/

#define FADE_IN_STEP			5
#define SILENCE_STEP			1
#define STEP_DURATION			100
/*****************************************************************************/

#ifdef WIN32  //modi by yangdecai  09-24

typedef	unsigned char		kal_uint8;
typedef	signed char			kal_int8;
typedef	char				kal_char;
typedef	unsigned short		kal_wchar;

typedef	unsigned short int	kal_uint16;
typedef	signed short int	kal_int16;

typedef	unsigned int		kal_uint32;
typedef	signed int			kal_int32;

typedef	unsigned long long	kal_uint64;
typedef	signed long	long	kal_int64;


typedef	struct
{
	kal_uint16 grab_start_x;				/* The first grabed	column data	of the image sensor	in pixel clock count */
	kal_uint16 grab_start_y;				/* The first grabed	row	data of	the	image sensor in	pixel clock	count */
	kal_uint16 exposure_window_width;		/* Exposure	window width of	image sensor */
	kal_uint16 exposure_window_height;		/* Exposure	window height of image sensor */
	kal_uint16 image_target_width;			/* image captured width	*/
	kal_uint16 image_target_height;			/* image captuerd height */
	kal_uint16 exposure_pixel;				/* exposure	window width of	image sensor + dummy pixel */
	kal_uint16 exposure_line;				/* exposure	window width of	image sensor + dummy line */
	kal_uint8  digital_zoom_factor;			/* digital zoom	factor */
} image_sensor_exposure_window_struct;

typedef	struct
{
	kal_bool	enable_shutter_tansfer;		/* capture only	*/
	kal_uint16	capture_shutter;			/* capture only	*/
	kal_uint8	image_mirror;
	kal_uint8	frame_rate;
	kal_bool	meta_mode;					/* capture only, 0:	normal mode, 1:	meta mode */
	kal_bool	enable_flashlight_tansfer;	/* flash light capture only	*/
	kal_uint16	flashlight_duty;			/* flash light capture only	*/
	kal_uint16	flashlight_offset;			/* flash light capture only	*/
} image_sensor_config_struct;

typedef	struct
{
   kal_int8	(* sensor_init)(void);
   void	(* get_sensor_id)(kal_uint8	*sensor_write_id, kal_uint8	*sensor_read_id);
   void	(* get_sensor_size)(kal_uint16 *sensor_width, kal_uint16 *sensor_height);
   void	(* get_sensor_period) (kal_uint16 *pixel_number, kal_uint16	*line_number);
   void	(* sensor_preview_setting)(image_sensor_exposure_window_struct *image_window, image_sensor_config_struct *sensor_config_data);
   void	(* sensor_capture_setting)(image_sensor_exposure_window_struct *image_window, image_sensor_config_struct *sensor_config_data);
   void	(* write_sensor_reg)(kal_uint32	cmd, kal_uint32	para);
   kal_uint32 (* read_sensor_reg)(kal_uint32 cmd);
   void	(* set_sensor_eshutter)(kal_uint16 eshutter);
   void	(* set_camera_night_mode) (kal_bool	enable);
   void	(* sensor_power_off) (void);
   kal_uint16 (* set_sensor_gain) (kal_uint16 gain);
   void	(* set_flashlight) (kal_bool enable);
   kal_uint32 (* yuv_ATV_setting)(kal_uint32 cmd, kal_uint32 para);
} image_sensor_func_struct;


char GPIO_ReadIO(char port);

#define	SET_CMOS_CLOCK_POLARITY_LOW
#define	SET_VSYNC_POLARITY_LOW
#define	SET_VSYNC_POLARITY_HIGH
#define	SET_HSYNC_POLARITY_LOW
#define	SET_HSYNC_POLARITY_HIGH
#define	ENABLE_CAMERA_INDATA_FORMAT
#define	SET_CAMERA_INPUT_TYPE(a)
#define	SET_CAMERA_INPUT_ORDER(a)
#define	ENABLE_CAMERA_TG_CLK_48M
#define	set_isp_driving_current(a)
#define	ISP_DRIVING_2MA

#define	SET_TG_OUTPUT_CLK_DIVIDER
#define	SET_CMOS_RISING_EDGE(a)
#define	SET_CMOS_FALLING_EDGE(a)
#define	ENABLE_CAMERA_PIXEL_CLKIN_ENABLE
#define	SET_TG_PIXEL_CLK_DIVIDER(a)
#define	SET_CMOS_DATA_LATCH(a)
#define	DISABLE_CAMERA_TG_PHASE_COUNTER
#define	DISABLE_CAMERA_CLOCK_OUTPUT_TO_CMOS

#define ENABLE_CAMERA_IDLE_INT
#define DISABLE_VIEW_FINDER_MODE
#define ENABLE_VIEW_FINDER_MODE
void isp_ibw2_cb(void);

typedef void (*FuncPtr)(void);
#define	ATV_TIMER_RESET_DATA_PATH	1
#define	ATV_TIMER_CHANNEL_CHANGE	2
#define ENABLE_CAMERA_FRAME_READY_INT
extern kal_bool isp_digital_zoom_flag;
kal_uint32 SaveAndSetIRQMask(void);
void RestoreIRQMask(kal_uint32 a);
void StartTimer(kal_uint16 timerid, kal_uint32 delay, FuncPtr funcPtr);
void StopTimer(kal_uint16 timerid);

#endif /* WIN32 */
//modi by yangdecai 09-25
#define	GPIO_ModeSetup(a, b)   
#define	GPIO_InitIO(a, b)      
#define	GPIO_WriteIO(a, b)     


typedef	enum {
	TLG_TV_525_LINES,
	TLG_TV_625_LINES
} TLG_TV_SCANLINES;

typedef	enum {
	TLG_SCRMD_WINDOW,
	TLG_SCRMD_FULLSCREEN
} TLG_SCREEN_MODE;

typedef enum {
	TLG_TUNEMD_AUTO,
	TLG_TUNEMD_MANUAL
} TLG_TUNE_MODE;

typedef struct {
	p_tlg_channel_t(chnmap);
	tlg_mode_t mode;
	tlg_vidstd_t vidstd;
	tlg_bw_t bw;
	TLG_TV_SCANLINES scanlines;
	uint16 chrlma_order, cbcr_order;
} TLG_REGION_SETTING;

typedef struct {
	uint8 mBrightness;				// 亮度
	uint8 mContrast;					// 对比度
	uint8 mSaturation;				// 色饱和度
	uint8 mDefinition;				// 清晰度
	uint8 mVolume;					// 音量

	uint8 mHOffset,					// 水平偏移调节
	 	 mHReduce,					// 水平大小调节
		 mVOffset,					// 垂直偏移调节
		 mVReduce;					// 垂直大小调节
} TLG_TV_OPTIONS;

typedef struct {
	uint8 mVolume;					// 音量
} TLG_FM_OPTIONS;

extern int tlg_cur_chn_cnt;
extern TLG_TV_SCANLINES gTvScanLines;
extern TLG_TV_OPTIONS gTvOptions;
//extern TLG_TV_STORAGE gTvStorage;
extern TLG_FM_OPTIONS gFmOptions;
//extern TLG_FM_STORAGE gFmStorage;

void PowerOnFM(void);
void PowerOffFM(void);

int TLGAPP_SetChannelExt(int iChn);
void TLG_InitRegionChannelMap(uint8 region);

void TLGMMI_NextTvChannel(void);
void TLGMMI_PrevTvChannel(void);
void TLGMMI_NextFmChannel(void);
void TLGMMI_PrevFmChannel(void);

int TLGMMI_GetBrightness(uint8 *pBrightness);
int TLGMMI_SetBrightness(uint8 iBrightness);
int TLGMMI_GetContrast(uint8 *pContrast);
int TLGMMI_SetContrast(uint8 iContrast);
int TLGMMI_GetSaturation(uint8 *pSaturation);
int TLGMMI_SetSaturation(uint8 iSaturation);
int TLGMMI_GetDefinition(uint8 *pDefinition);
int TLGMMI_SetDefinition(uint8 iDefinition);
int TLGMMI_GetHSize(uint8 *pHsize);
int TLGMMI_GetVSize(uint8 *pVsize);
int TLGMMI_SetHSize(uint8 iHsize);
int TLGMMI_SetVSize(uint8 iVsize);
int TLGMMI_GetHOffset(uint8 *pHoffset);
int TLGMMI_GetVOffset(uint8 *pVoffset);
int TLGMMI_SetHOffset(uint8 iHoffset);
int TLGMMI_SetVOffset(uint8 iVoffset);
void TLGMMI_RestoreTvOption();
void TLGMMI_RestoreFmOption();
void TLGMMI_ClearTvStorage();
void TLGMMI_ClearFmStorage();

#endif /* _PLATFORM_H_ */


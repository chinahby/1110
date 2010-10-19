
#ifndef __CAM_MODULE_H_
#define __CAM_MODULE_H_
#include	"A8_common.h"

#include	"ait_interface.h"
 
typedef struct 
{ 
//        ext_camera_burst_struct        *burst_capture_ptr; 
        u_short        image_width; 
        u_short        image_height; 
        u_short  buffer_width; 
        u_short  buffer_height; 
        u_short  preview_offset_x; 
        u_short  preview_offset_y; 
        u_short        preview_width; 
        u_short        preview_height; 
        u_int        total_file_size; 
        u_int  src_key_color; 
        u_char        captured_picture; 
        u_char   snapshot_number; 
        u_char        white_balance_mode1; 
        u_char        ev_value; 
        u_char        gamma_table; 
        u_char        zoom_factor; 
        u_char         image_effect1; 
        u_char         post_effect; 
        u_char         sticker_capture; 
        u_char        lcd_id; 
        u_char        frame_rate; 
        u_char        jpeg_compression_ratio; 
        u_char         preview_rotate; 
        u_char         banding_freq; 
        u_char        *jpeg_buffer_ptr; 
        u_short        *image_buffer_ptr; 
        u_int  image_buffer_size; 
        u_char        continue_capture; 
        u_char    video_rec_mode; 
        u_short preview_src;	//ePREVIEW_SRC_MODE 
} ext_camera_para_struct; 


typedef struct
{

	void (* cam_module_power_on)(void);
	void (* cam_module_cmd_mapping)(void);
	unsigned char (* cam_module_reset_status)(void);
	unsigned char (* cam_module_preview_ready_check)(void);
	unsigned int (* cam_module_capture_ready_check)(void);
	void (* cam_module_power_off)(void);
	void (* cam_module_preview)(ext_camera_para_struct *ext_cam_para);
	void (* exit_cam_module_preview)(void);
	void (* cam_module_capture)(ext_camera_para_struct *ext_cam_para);
	unsigned int (* cam_module_jpeg_encode) (ext_camera_para_struct *ext_cam_para, unsigned char back_to_preview);
	void (* cam_module_set_para)(unsigned char cmd, unsigned char para);
	void (* cam_module_write_para)(unsigned short cmd, unsigned short para);
	unsigned short (* cam_module_read_para)(unsigned short cmd);
	unsigned char (* cam_module_frame_rate)(void);

} ait_cam_func;


typedef struct
{
	void (*ait_cam_mjpg_rec_start)(void);
	void (*ait_cam_mjpg_rec_pause)(void);
	void (*ait_cam_mjpg_rec_resume)(void);
	void (*ait_cam_mjpg_rec_stop)(void);
	unsigned char* (*ait_cam_mjpg_rec_encode)(unsigned char* pFileBuff, unsigned int* pJpegfilesize);
	int (*ait_cam_mjpg_rec_interrupt)(unsigned char* pJpegFile);
} ait_video_rec_func;


typedef struct
{

	unsigned char (*ait_video_play_play)(u_short *frameBuf, u_int frameSize, u_short image_width, u_short image_height, u_char update_to_lcd);
	void (*ait_video_play_pause)(void);
	void (*ait_video_play_resume)(void);
	void (*ait_video_play_stop)(void);

} ait_video_play_func;

typedef struct
{
	void (*ait_usb_msdc_start)(void);
	void (*ait_usb_msdc_stop)(void);
	void (*ait_usb_web_cam_active)(void);
	void (*ait_usb_dp_pull_up)(void);
	void (*ait_usb_dp_pull_down)(void);
	void (*ait_usb_dm_pull_up)(void);
	void (*ait_usb_dm_pull_down)(void);
} ait_usb_func;


typedef struct
{
	unsigned char* (*ait_sd_init)(void);
	unsigned short (*ait_sd_read)( unsigned int startsect,unsigned int offset,unsigned char *buf,unsigned int read_size );
	unsigned short (*ait_sd_write)( unsigned int startsect,unsigned int offset,unsigned char *buf,unsigned int write_size );
	u_int (*ait_sd_get_size)(void);
} ait_sd_func;


/*===========================================================================

				MACRO DEFINITIONS

===========================================================================*/

/*===========================================================================

				TYPE DEFINITIONS

===========================================================================*/
/*typedef enum
{
	MMP_SUCCESS = 0,
	MMP_FAILED,
	MMP_INVALID_STATE,
	MMP_INVALID_FORMAT,
	MMP_INVALID_PARAM,
	MMP_NO_SENSOR,	
	MMP_NO_MEMORY,
	MMP_INVALID_SIZE,
	MMP_NOT_SUPPORTED,
	MMP_RET_CODE_MAX
}mmp_ret_code_type1;
*/

typedef enum
{
	MMPFUNC = 0,	
	MMPFUNC_CAM_CMD,	
	MMPFUNC_CAM_INIT,
	MMPFUNC_CAM_CMD_MAPPING,
	MMPFUNC_CAM_PWR_ON,
	MMPFUNC_CAM_PWR_OFF,
	MMPFUNC_CAM_WRITE_PARAM,
	MMPFUNC_CAM_READ_PRARM,
	MMPFUNC_CAM_FRAME_RATE,
	MMPFUNC_CAM_SET_PARAM,
	MMPFUNC_CAM_UPDATEOSD,
	MMPFUNC_CAM_CHK_SEN_FRAME,	
	MMPFUNC_CAM_RESET_STATUS,
	MMPFUNC_CAM_DETECT_SENSOR_ID,
	MMPFUNC_CAM_SEARCH_SENSOR_ID,
	
	MMPFUNC_CAM_PREVIEW_START = 0x0100,
	MMPFUNC_CAM_PREVIEW_STOP,
	MMPFUNC_CAM_PREVIEW_RDY_CHK,
	MMPFUNC_CAM_CAPTURE,
	MMPFUNC_CAM_CAPTURE_RDY_CHK,

	MMPFUNC_CAM_JPEG_ENCODE,
	MMPFUNC_CAM_JPEG_DECODE,

	
	MMPFUNC_CAM_VIDEO_RECORD_START = 0x0200,
	MMPFUNC_CAM_VIDEO_RECORD_STOP,
	MMPFUNC_CAM_VIDEO_PAUSE,
	MMPFUNC_CAM_VIDEO_RESUME,
	MMPFUNC_CAM_GET_VIDEO_DATA,


				

	MMPFUNC_CAM_VIDEO_PLAY_START = 0x0300,
	MMPFUNC_CAM_VIDEO_PLAY_STOP,
	MMPFUNC_CAM_VIDEO_PLAY_PAUSE,
	MMPFUNC_CAM_VIDEO_PLAY_RESUME,
//#if defined(AIT_3GP_SUPPORT)
	MMPFUNC_CAM_VIDEO_PLAY_NORMAL_SCREEN,
	MMPFUNC_CAM_VIDEO_PLAY_FULL_SCREEN,
	MMPFUNC_CAM_VIDEO_PLAY_YUV_BUF,
//#endif	
	MMPFUNC_CAM_SET_VIDEO_FRAME_RATE,
	MMPFUNC_CAM_GET_VIDEO_FRAME_RATE,	
//#if AIT_VIDEO_PHONE_SUPPORT
	MMPFUNC_CAM_VIDEO_PHONE_GET_RAW_DATA,//AIT_VIDEO_PHONE_SUPPORT
//#endif

	MMPFUNC_SD_CMD = 0x0400,
	MMPFUNC_SD_INIT,
	MMPFUNC_SD_OPEN,
	MMPFUCN_SD_CLOSE,
	MMPFUNC_SD_READ,
	MMPFUNC_SD_WRITE,			//40
	MMPFUNC_SD_GET_CSD,
	MMPFUNC_SD_IO_CTL,

	MMPFUNC_USB_CMD = 0x0500,
	MMPFUCN_USB_DP_PULL_UP,
	MMPFUCN_USB_DP_PULL_DOWN,
	MMPFUCN_USB_DM_PULL_UP,
	MMPFUCN_USB_DM_PULL_DOWN,
	MMPFUNC_USB_START,
	MMPFUNC_USB_STOP,
	MMPFUNC_USB_MSDC,
	MMPFUCN_USB_PCCAM,

	MMPFUNC_SYS_CMD = 0x0600,
	MMPFUNC_SYS_ENTERSLEEP,
	MMPFUNC_SYS_EXITSLEEP,
	MMPFUNC_SYS_SETGPIO,
	
	MMPFUNC_MAX					//50
}mmp_func_type;


typedef enum 	
{
	DSC_CMD_NULL = 0,
	DSC_CMD_SET_ZOOM,
	DSC_CMD_SET_EV_VALUE,
	DSC_CMD_SET_EFFECT,
	DSC_CMD_SET_WB,
	DSC_CMD_SET_AC_HERTZ
}DSCCmdMapping_e;

typedef enum
{
	DSC_NORMAL = 0,
	DSC_GRAYSCALE,
	DSC_SEPIA,
	DSC_SEPIA_GREEN,
	DSC_SEPIA_BLUE,
	DSC_COLOR_INV,
	DSC_GRAY_INV,
	DSC_BLACKBOARD,
	DSC_WHITEBOARD,
	DSC_COPPER_CARVING,
	DSC_EMBOSSMENT,
	DSC_BLUE_CARVING,
	DSC_CONTRAST,
	DSC_JEAN,
	DSC_SKETCH,
	DSC_OIL,
	DSC_EFF_TYPE_COUNT
} DSCEffTyp_e;

typedef enum
{
	DSC_WB_AUTO = 0,
	DSC_WB_CLOUD,	
	DSC_WB_DAYLIGHT,
	DSC_WB_INCANDESCENCE,
	DSC_WB_FLUORESCENT,
	DSC_WB_TUNGSTEN,
	DSC_WB_MANUAL,
	DSC_WB_TYPE_COUNT
} DSCWBTyp_e;

typedef enum
{
	DSC_ICON_WIN0 = 0,
	DSC_ICON_WIN1,
	DSC_ICON_WIN_COUNT,
	DSC_ICON_WIN_CLEAR = DSC_ICON_WIN_COUNT
} DSCIconWin_e;

typedef enum
{
	DSC_COLOR_ARGB1555 = 0,
	DSC_COLOR_RGB565
} DSCColorTyp_e;

typedef enum
{
	DSC_BYPASS_LCD0 = 0,
	DSC_BYPASS_LCD1,
	DSC_BYPASS_LCD2,
	DSC_BYPASS_LCD3,
	DSC_BYPASS_LCD4,
	DSC_BYPASS_LCD5,
	DSC_BYPASS_LCD6,
	DSC_BYPASS_LCD7
} DSCBypassLcd_e;

typedef enum
{
	DSC_GPO_LOW = 0,
	DSC_GPO_HIGH
} DSCGpoType_e;

typedef enum
{
	DSC_MODE_50HZ = 0,
	DSC_MODE_60HZ
} DSCPreviewHz_e;

typedef enum  {
	DSC_EV_VALUE_NEG_4 = 0,
	DSC_EV_VALUE_NEG_3,
	DSC_EV_VALUE_NEG_2,
	DSC_EV_VALUE_NEG_1,
	DSC_EV_VALUE_ZERO,
	DSC_EV_VALUE_POS_1,
	DSC_EV_VALUE_POS_2,
	DSC_EV_VALUE_POS_3,
	DSC_EV_VALUE_POS_4
} DSCExposureValue_e;

typedef enum
{
	DSC_CAP_SIZE_PREVIEW = 0,
	DSC_CAP_SIZE_QQVGA,
	DSC_CAP_SIZE_QVGA,
	DSC_CAP_SIZE_VGA,
	DSC_CAP_SIZE_XVGA,
	DSC_CAP_SIZE_QCIF,
	DSC_CAP_SIZE_CIF,
	DSC_CAP_SIZE_CUSTOM_SPECIFY_0
} DSCCapImgSize_e;

typedef enum
{
	DSC_PREVIEW_FAR = 0,
	DSC_PREVIEW_MIDDLE,
	DSC_PREVIEW_NEAR
} DSCPreviewRange_e;

typedef enum
{
	DSC_PREVIEW_FRAME_NONE = 0,
	DSC_PREVIEW_ONE,
	DSC_PREVIEW_TWO,
	DSC_PREVIEW_THREE
} DSCPreviewFrame_e;

typedef enum
{
	DSC_JPEG_QUALITY_FINE ,//= 0x10,
	DSC_JPEG_QUALITY_HIGH ,//= 0x18,
	DSC_JPEG_QUALITY_NORMAL,// = 0x20,
	DSC_JPEG_QUALITY_LOW
} DSCJpegQuality_e;

#endif

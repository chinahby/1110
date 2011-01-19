#ifndef AIT_CAM_API_H
#define AIT_CAM_API_H

#include "A8_common.h"
#include "AIT700_EBIbus.h"
#include "A8_sensor.h"
#include "A8_dscapi.h"
#include "A800_camera.h"
#include "sys_IF_ait_api.h"
#include "cam_module.h"
#include "ait_interface.h"
#include "OEMCFGI.h"

typedef enum
{
	CAPTURE_RESO_VGA			=0x00,
	CAPTURE_RESO_QVGA,
	CAPTURE_RESO_176x220,	
	CAPTURE_RESO_176x144,		
	CAPTURE_RESO_128x160,	
	CAPTURE_RESO_128x96,	
	CAPTURE_RESO_QQVGA		
} CAPTURE_RESO;

typedef enum
{
	CAPTURE_QUALITY_SUPER_FINE	= 0x00,
	CAPTURE_QUALITY_FINE					= 0x01,
	CAPTURE_QUALITY_NORMAL			= 0x02,
	CAPTURE_QUALITY_ECONOMY			= 0x03
} CAPTURE_QUALITY;

typedef enum
{
    CAM_CTRL_NONE = 0,
    CAM_CTRL_ZOOM,
    CAM_CTRL_CONTRAST,
    CAM_CTRL_BRIGHTNESS,
    CAM_CTRL_HUE,
    CAM_CTRL_GAMMA,
    CAM_CTRL_WB,
    CAM_CTRL_EXPOSURE,
    CAM_CTRL_EFFECT,
    CAM_CTRL_DEBAND_MODE,
    CAM_CTRL_SATURATION,		//10
    CAM_CTRL_NIGHT_MODE,
    CAM_CTRL_SHARPNESS,
    CAM_CTRL_EV_VALUE,
    CAM_CTRL_FLASH,
    CAM_CTRL_FLASH_MODE,
    CAM_CTRL_AE_METERING,		// 15
    CAM_CTRL_AF_KEY,
    CAM_CTRL_AF_METERING,
    CAM_CTRL_AF_MODE,
    CAM_CTRL_MANUAL_FOCUS_DIR,
    CAM_CTRL_SHUTTER_PRIORITY,		//20
    CAM_CTRL_APERTURE_PRIORITY,
    CAM_CTRL_ISO,
    CAM_CTRL_DSC_MODE,
    CAM_CTRL_YUV_AF_STATUS,
    CAM_CTRL_AF_OPERATION,
    CAM_CTRL_AF_RANGE,
    CAM_CTRL_FRAMERATE,
    CAM_CTRL_ROTATE,
    CAM_CTRL_MATRIX,
    CAM_CTRL_CENTERWEIGHTED,		//30
    CAM_CTRL_SPOT,
    CAM_CTRL_MAX
} CAM_CTRL_MODE;

typedef enum
{
	CAM_CAP_JPEG_SIZE = 0x00,
	CAM_CAP_QUALITY,
	CAM_CAP_MAX_ENCODED_SIZE,
	CAM_CAP_WITH_EXIF,
	CAM_CAP_WITH_STICKER
} CAM_CAP_MODE;

typedef enum 
{ 
	CAM_EDIT_IMAGE_NORMAL = 0x00, 
	CAM_EDIT_IMAGE_GRAY, 
	CAM_EDIT_IMAGE_NEGATIVE,
	CAM_EDIT_IMAGE_SEPIA, 
	CAM_EDIT_IMAGE_SKETCH,
	CAM_EDIT_IMAGE_POSTERIZE, 
	CAM_EDIT_IMAGE_SOLARIZE, 
	CAM_EDIT_IMAGE_BLUR, 
	CAM_EDIT_IMAGE_SHARPNESS, 
	CAM_EDIT_IMAGE_BRIGHTNESS, 
	CAM_EDIT_IMAGE_CONTRAST, 
	CAM_EDIT_IMAGE_COLOR
}CAM_EDIT_IMAGE_TYPE;

typedef enum
{
	CAM_ROTATE_NORMAL = A8_RT_NORMAL,
	CAM_ROTATE_90 = A8_RT_RIGHT_90,
	CAM_ROTATE_180 = A8_RT_RIGHT_180,
	CAM_ROTATE_270 = A8_RT_RIGHT_270,
	CAM_ROTATE_H_MIRROR,
	CAM_ROTATE_V_MIRROR
} CAM_PREVIEW_ROTATE;

typedef enum
{
	CAM_LCD_REFRESH_NORMAL = 0x00,
	CAM_LCD_REFRESH_SCALE,
	CAM_LCD_REFRESH_BYPASS
}CAM_LCD_REFRESH_MODE;
//Structure Define


typedef struct
{
	u_char lcdMode;
	PREVIEW_MODE previewMode;
	u_int	transparencyColor;	/*transparency color*/
}AIT_PREVIEW_CONFIG;

typedef struct
{

	CAPTURE_RESO		image_resolution;		/*capture resolution*/
	CAPTURE_QUALITY	jpeg_quality;			/*jpeg quality*/
	u_int				maxEncodedSize;
	bool				jpeg_withEXIF;
	bool				sticker_capture;			/*Whethre capture image with sticker */
} AIT_CAPTURE_CONFIG;

typedef struct
{	
	// Mapping to CAM_CTRL_MODE!!
	u_char	zoom_step;
	u_char	frame_rate;
	u_char	brightness;
	u_char	night_mode;
	u_char	AWB_mode;
	u_char	effect_mode;
	u_char	deband_mode;
	u_char	rotate_mode;
}AIT_CAMERA_CONFIG;

typedef struct
{
	A8S_IMAGE SrcImg;
	u_char *tmpBuf;
	s_short brightness;	//For this A800_Adjust_Brightness function use
	s_short contrast;		//For this A800_Adjust_Contrast function use
	s_short cr, mg, yb;	//For this A800_Adjust_CR_MG_YB function use
}CAM_EDIT_IMAGE_PARA;

typedef struct{
	PREVIEW_MODE mode;
	CAM_PREVIEW_ROTATE PanelRotate;

	RECT_AIT PreviewRange;
	
	RECT_AIT OSD_TOP;
	
	RECT_AIT OSD_CENTER;

	RECT_AIT OSD_BOTTOM;

}sLCD_ATTRIBUITE;


typedef struct{
	u_short MainSensorID;
	u_short SubSensorID;
	u_short tvModuleID;
}sPresentSensorID;

#if defined(AIT_3GP_SUPPORT)

typedef struct 
{
	unsigned char* ptrYbuf;
	unsigned char* ptrUbuf;
	unsigned char* ptrVbuf;

	unsigned short u16VideoWidth;    //Availagle Video width & Height
	unsigned short u16VideoHeight;
	 
	unsigned short u16YBufWidth;        // Memory size of video = w + span!A 0<=span<=64!A span  16x, x=integer
	unsigned short u16UVBufWidth;

	unsigned short * pOsdBufTitle;	//RBG565 OSD Buffer for title
	unsigned short * pOsdBufStatus;	//RBG565 OSD Buffer for status & progress bar.

	// OSD update range, original arix from PANEL start(x,y)
	RECT_AIT osd_Title;

	RECT_AIT osd_Status;

 
}YUV420BufInfo;
#endif /* AIT_3GP_SUPPORT */

extern A8_ERROR_MSG cam_IF_ait_open_AIT(void);
extern A8_ERROR_MSG	cam_IF_ait_close_AIT(void);
extern A8_ERROR_MSG	cam_IF_ait_status (void);

extern A8_ERROR_MSG	cam_IF_ait_open_camera (void);
extern A8_ERROR_MSG	cam_IF_ait_close_camera (void);

extern A8_ERROR_MSG	cam_IF_ait_camera_config (CAM_CTRL_MODE mode, u_char para);
#if 0
extern A8_ERROR_MSG	cam_IF_ait_capture_config (CAM_CAP_MODE mode, u_int para);
#else
extern void cam_IF_ait_capture_config(ext_camera_para_struct*  Ext_Capture_config);
#endif
extern A8_ERROR_MSG	cam_IF_ait_preview_config (AIT_PREVIEW_CONFIG previewConfig);
extern A8_ERROR_MSG	cam_IF_ait_preview_LcdScaler (u_short Win, u_short Hin, u_short Wout, u_short Hout);

extern A8_ERROR_MSG	cam_IF_ait_preview_control (bool bPreviewOn);
extern A8_ERROR_MSG	cam_IF_ait_capture (u_short *jpeg_buffer_ptr, u_int *JpegSize);
extern A8_ERROR_MSG	cam_IF_ait_update_OSD (u_short *OSDBufferPtr, u_short x, u_short y, u_short width, u_short height );

extern A8_ERROR_MSG	cam_IF_ait_JPEG_encode (u_short *rgbBufferPtr,u_short rgbW,u_short rbgH,u_short *JpegBufferPtr,u_int *JpegSize);
extern A8_ERROR_MSG	cam_IF_ait_playback_config (AIT_PREVIEW_CONFIG playbackConfig);
#if 0
extern A8_ERROR_MSG	cam_IF_ait_JPEG_decode(A8S_JPEG_INFO SrcJPEG, u_short *pRgbBuf, u_short OutW, u_short OutH, u_char OutFormat);
#else
extern A8_ERROR_MSG cam_IF_ait_JPEG_decode(u_short * jpegbuf, u_int jpegsize, u_short panelwidth, u_short panelheight, u_char * dataptr);
#endif
extern A8_ERROR_MSG	cam_IF_ait_vdo_record_start(u_short quality);
extern A8_ERROR_MSG cam_IF_ait_vdo_record_handler(u_char *frameBuf, u_int *frameSize);
extern A8_ERROR_MSG cam_IF_ait_vdo_record_stop(void);
extern A8_ERROR_MSG cam_IF_ait_vdo_playback_handler(u_short *videoBuf, u_int frameSize);
extern A8_ERROR_MSG cam_IF_ait_vdo_playback_stop(void);
extern A8_ERROR_MSG cam_IF_ait_preview_start(PREVIEW_MODE preview_mode, ePREVIEW_SRC_MODE preview_src);
extern A8_ERROR_MSG cam_IF_ait_VIF_Src_Selection(ePREVIEW_SRC_MODE preview_src);
#if AIT_VIDEO_PHONE_SUPPORT
extern A8_ERROR_MSG cam_IF_ait_get_rgb565_data(u_short rgbWidth, u_short rgbHeight, u_char *rgb565_ptr);
extern A8_ERROR_MSG cam_IF_ait_get_yuv420_data(u_short yuvWidth, u_short yuvHeight, u_char *yuv420_ptr);
extern A8_ERROR_MSG cam_IF_ait_check_snesor_frame(u_char mode);
#endif
extern A8_ERROR_MSG	cam_IF_ait_camera_id_detect (unsigned camera_id, unsigned short *sensorID);
extern A8_ERROR_MSG	cam_IF_ait_camera_id_serch (unsigned short sensorID);
#endif

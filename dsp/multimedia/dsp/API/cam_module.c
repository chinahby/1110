#include "A8_common.h"
#if defined(__QSC_TARGET__)
#include "clk.h"
clk_cb_type ait_hw_clk_cb;
#endif

#ifndef _FW_FROM_PC_
#include "AIT701_FW.h"
#endif

#include "cam_IF_ait_api.h"
#include "sys_IF_ait_api.h"
#include "usb_IF_ait_api.h"
#include "sd_IF_ait_api.h"
#include "a8_sensor.h"


#include "AIT_Interface.h"
 
typedef A8_ERROR_MSG mmp_ret_code_type;

u_char *JpegBuf;
u_short gAitVidPlaywMode = AIT_VDO_PLAY_NOR_MODE;


static u_char AIT701_sleep_flag = 1;
u_char gAitPreviewStatus = 1;
u_char gAitCaptureStatus = 1;
u_int 	gAitJpegSize = 0;
u_char gVideoQuality = 0;
bool		gAitEnterSleep = 0;
unsigned char   dcam_state = 0;
extern bool gbUsbEn;
extern bool gbSdEn;
extern bool gbCamEn;
extern A800InfoDT  A800Data;
static u_char  AIT_HW_TimerHandler;
static u_char AIT_sleep_mode_handler;
unsigned char ait_sleep_flg=0;
static AIT_PREVIEW_CONFIG playbackConfig = {A8_MAIN_LCD,AIT_CAM_PREV_NOR_MODE,0x1f};
extern unsigned short ait_sleep_checktime;
extern const unsigned char DCAM_EINT_NO;
static void AIT701_set_gpio(u_short pin, u_char level);
static void AIT701_cam_preview(ext_camera_para_struct *ext_cam_para);

extern unsigned char g_ATV_Flag;

#if AIT_VIDEO_PHONE_SUPPORT
unsigned char bVideoChat;
#endif

#if defined(AIT_3GP_SUPPORT)
#if AIT_YUV_BUF_VDOPLY_TEST ==1

#include "raw220x176.h"
YUV420BufInfo YUV420frameBufTest = {0};

#endif

static YUV420BufInfo* pAIT_YUV420frameBufInfo = NULL;
static char bIsVidPlayFullScreen = 0;
#endif /* AIT_3GP_SUPPORT */



static void AIT701_ResetLCDWindow(void)
{
	int timeout;
	extern unsigned short gA8MainLCDWidth;
	extern unsigned short gA8MainLCDHeight;
	
	sys_IF_ait_set_bypass_mode(A8_ON);			
	AIT_ext_SetLCDWindow(0,0,gA8MainLCDWidth,gA8MainLCDHeight);
	sys_IF_ait_set_bypass_mode(A8_OFF);


	
	return;
}






static void mmpfunc_sleepevent_handler(void *parameter)
{
	MSG_FATAL("mmpfunc_sleepevent_handler!",0,0,0);

#if defined(__QSC_TARGET__)
//Stop timer Handler
    clk_dereg( &ait_hw_clk_cb );
#endif
	gAitEnterSleep = TRUE;

}


static void mmpfunc_sleep_enable()
{
//	if(ait_sleep_enable==1)
	{
		if((gbCamEn == FALSE) &&(gbUsbEn == FALSE)&&AIT_STATUS_VDO_PLAY!= sys_IF_ait_get_status())
		{		
#if defined(__QSC_TARGET__)
    clk_reg( &ait_hw_clk_cb, mmpfunc_sleepevent_handler, AIT_SLEEP_CHECKTIME, 0, FALSE );
#endif
		}			
	}


}

static void mmpfunc_sleep_disable()
{
	gAitEnterSleep = FALSE;
#if defined(__QSC_TARGET__)
//Stop timer Handler
    clk_dereg( &ait_hw_clk_cb );
#endif

	if(ait_sleep_flg==1)
	{
		AIT_ext_ClockPinCtl(1);
		sys_IF_ait_delay1ms(10);	
		ait_sleep_flg=0;	
		AIT_Message_P0("Exit AIT Sleep \r\n");
	}
}

u_char JpegdecodeDummyPixel;
u_short gLCDX0;

static mmp_ret_code_type mmpfunc_cam_process(mmp_func_type func, void* client_data, u_int p1, u_int p2, u_int p3, u_int p4, void* pdata)
{
	mmp_ret_code_type status = MMP_INVALID_STATE;
	if(AIT_STATUS_USB_READY <= sys_IF_ait_get_status())
	{
		AIT_Message_Error("!!!mmpfunc_cam_process: SD is Busy, AIT Status = %d\r\n", sys_IF_ait_get_status());
		return MMP_INVALID_STATE;	
	}

	switch(func)
	{
		case	MMPFUNC_CAM_INIT:
				status = sys_IF_ait_boot_init();
				break;
				
		case	MMPFUNC_CAM_CMD_MAPPING:
				break;
				
		case	MMPFUNC_CAM_PWR_ON:	
				status = cam_IF_ait_open_AIT();
				break;
				
		case 	MMPFUNC_CAM_DETECT_SENSOR_ID:

				status = cam_IF_ait_camera_id_detect(p1,pdata);
				break;
		case 	MMPFUNC_CAM_SEARCH_SENSOR_ID:				
				status = cam_IF_ait_camera_id_serch(p1);
				break;
				
		case	MMPFUNC_CAM_RESET_STATUS:
		case	MMPFUNC_CAM_PREVIEW_START:	
				{
					ePREVIEW_SRC_MODE preview_src = (ePREVIEW_SRC_MODE)p2;

					MSG_FATAL("preview_src = %d",preview_src,0,0);
					status = MMP_SUCCESS;
					if(AIT_STATUS_CAM_READY != sys_IF_ait_get_status())
						status = cam_IF_ait_open_AIT();
					
					if(!status)
					{		
						AIT701_ResetLCDWindow();
						status = cam_IF_ait_VIF_Src_Selection((ePREVIEW_SRC_MODE)preview_src);
						if(status)					
						{
							AIT_Message_Error("Sensor Selection Error",status);
						}


						status = cam_IF_ait_open_camera();
		
						if(!status)					
						{
							status = cam_IF_ait_preview_start(p1,p2);//(ext_camera_para_struct *) client_data);
							MSG_FATAL("cam_IF_ait_preview_start status : %d",status,0,0);
						}
					}
				}
				break;
				
		case	MMPFUNC_CAM_PREVIEW_STOP:
				status = cam_IF_ait_preview_control(A8_OFF);
				break;
				
		case	MMPFUNC_CAM_PREVIEW_RDY_CHK:
				*(u_char *)pdata = gAitPreviewStatus;
				status = MMP_SUCCESS;
				break;
				
		case	MMPFUNC_CAM_CAPTURE:
				if(AIT_STATUS_CAM_PREVIEW == sys_IF_ait_get_status())
				{
					cam_IF_ait_capture_config((ext_camera_para_struct*)  client_data);
					status = cam_IF_ait_capture((u_short*)(((ext_camera_para_struct *)client_data)->jpeg_buffer_ptr),&gAitJpegSize);
					*(u_int*)pdata = gAitJpegSize;	
					AIT_Message_P1("cam_IF_ait_capture size = %d\r\n",gAitJpegSize);
				}else{
					status = MMP_INVALID_STATE;
				}
				break;
 
		case	MMPFUNC_CAM_JPEG_DECODE:
			{
				u_short 	Quality;
				u_int temp=0;
				short 	jpegWidth,jpegHeight,jpegFormat;
				if(AIT_STATUS_USB_READY != sys_IF_ait_get_status())
				{
					status = cam_IF_ait_open_AIT();
					if(status)
						return status;
					playbackConfig.lcdMode = A8_MAIN_LCD;

					{ 
						playbackConfig.previewMode = AIT_JPG_PLAY_FULL_MODE;
					}

					A8L_GetJpegInfo( (u_short*)client_data, p1, &jpegWidth, &jpegHeight, &jpegFormat ,&Quality);
					AIT_Message_P2("JPEG Reso: W:%d, H: %d",jpegWidth,jpegHeight);

					if((jpegWidth==1) || (jpegHeight==1)) //not support w=1 or h=1
					{
						status = 0x0C;
					}
					else
					{
						status = cam_IF_ait_preview_config(playbackConfig);	
						if(!status)
							AIT_Message_P1("cam_IF_ait_preview_config, status = %d\r\n",status);
						
						{
							extern unsigned short gLCDWidth;
							gLCDWidth = p4;
							temp = ((p2+3)/4)*4;
							JpegdecodeDummyPixel = temp - p2;
							p2 =temp;
						}
						status = cam_IF_ait_JPEG_decode((u_short *) client_data, p1, p2, p3, (u_char *) pdata);
						if(!status)
							AIT_Message_P1("cam_IF_ait_JPEG_decode, status = %d\r\n",status);
					}
					AIT_Message_P3("MMPFUNC_CAM_JPEG_DECODE, Size=%d, W=%d, H=%d\r\n",p1,p2,p3);
					cam_IF_ait_close_AIT();					
				}
				else
				{
					status = MMP_INVALID_STATE;
				}
			}
				break;
				
		case	MMPFUNC_CAM_CAPTURE_RDY_CHK:
				if(!gAitCaptureStatus)
					gAitJpegSize = 0;
				else
					gAitCaptureStatus = 0;
				*(u_int *)pdata = gAitJpegSize;
				AIT_Message_P1("MMPFUNC_CAM_CAPTURE_RDY_CHK size = %d\r\n",*(u_int *)pdata);				
				status = MMP_SUCCESS;
				break;
				
		case	MMPFUNC_CAM_VIDEO_RECORD_START:
				gVideoQuality = 2;
				if(AIT_STATUS_CAM_PREVIEW == sys_IF_ait_get_status())
				{
					status = cam_IF_ait_vdo_record_start(gVideoQuality);
					SetA8RegB(0x690a,GetA8RegB(0x690a)|0x08);	//sw reset encode block
					SetA8RegB(0x690a,GetA8RegB(0x690a)&0xf7);
					SetA8RegB(0x6511,GetA8RegB(0x6511)|0x04);	//reset line buffer
					SetA8RegB(0x6511,GetA8RegB(0x6511)&0xfb);
					SetA8RegB(0x6240,0x1);
					SetA8RegB(0x6242,0x1);	
					
					#if (AIT_VIDREC_INT_POLARITY == AIT_MTK_2_8V_FLASH_LOW )
					SetA8RegB(0x6800,0x3);						
					#else
						SetA8RegB(0x6800,0x1);
					#endif
					
					SetA8RegB(0x6810,GetA8RegB(0x6810)|0x04);
					SetA8RegB(0x690c,GetA8RegB(0x690c)|0x02);
					SetA8RegB(0x6204,0x28);						
				}else{
					status = MMP_INVALID_STATE;
				}	
				break;
				
		case	MMPFUNC_CAM_VIDEO_RECORD_STOP:
				status = cam_IF_ait_vdo_record_stop();
				SetA8RegB(0x690c,GetA8RegB(0x690c)&(~0x02));
				SetA8RegB(0x6810,GetA8RegB(0x6810)&(~0x04));
				SetA8RegB(0x6800,0);								
				SetA8RegB(0x6240,0x0);						
				SetA8RegB(0x6242,0x1);						
				break;
#if AIT_VIDEO_PHONE_SUPPORT
			case	MMPFUNC_CAM_VIDEO_PHONE_GET_RAW_DATA:
                        {		
                            A8_ERROR_MSG retVal=A8_NO_ERROR;
                            u_int *preturn_size;
                            AIT_Message_P0("[!!]#1 MMPFUNC_CAM_VIDEO_PHONE_GET_RAW_DATA");                                
                            preturn_size = (u_int *)pdata;                            
                            if (A8_YUV_422 == A800Data.Select->BufFormat){//Andy@20100224. YUV420 format
                                status = cam_IF_ait_get_yuv420_data(p1, p2, (u_char *)client_data);
                                if(A8_NO_ERROR == status){
                                    *preturn_size = (p1*p2* 3) >>1;
                                }
                                else{//Error
                                    *preturn_size = 0;                                    
                                }
                            }
                            else{//Andy@20100224. RGB565 format
                                status = cam_IF_ait_get_rgb565_data(p1, p2, (u_char *)client_data);
                                if(A8_NO_ERROR == status){
                                    *preturn_size = (p1*p2*2);
                                }
                                else{//Error
                                    *preturn_size = 0;                                    
                                }
                            }
                            AIT_Message_P1("[!!]#2 MMPFUNC_CAM_VIDEO_PHONE_GET_RAW_DATA. returen size=%d", *preturn_size);
                        }
                        break;
#endif			

		case	MMPFUNC_CAM_GET_VIDEO_DATA:
			{
				u_int jpegSize;
				u_short* jpegBuf;
				SetA8RegB(0x6242,0x01);	
				status = A8L_GetJpegPictureForVideo((u_short*)client_data, 300*1024, (u_int*)pdata);
				jpegSize = (GetA8RegW( 0x6250 ) + ((s_int)GetA8RegW(0x6252)<<16) + 1);				

				jpegSize = *((u_int*)pdata);
				if(jpegBuf[0]!=0xFFD8||jpegBuf[jpegSize-1]!=0xFFD9)
					AIT_Message_P2("JPEG Header: %x, %d",jpegBuf[0],jpegBuf[jpegSize-1]);							

				if(A8_CAM_FIFO_OVERFLOW_ERROR == status)
				{
					if(gVideoQuality < 4)
					{
						gVideoQuality++;
						A800_SetQTable(gVideoQuality);						
					}						
					AIT_Message_P1("JPEG Q=%d\r\n",gVideoQuality);	
					SetA8RegB(0x6511,GetA8RegB(0x6511)|0x04);	//reset line buffer
					SetA8RegB(0x6511,GetA8RegB(0x6511)&0xfb);					
				}	
				if(0==status){
					SetA8RegB(0x690a,GetA8RegB(0x690a)|0x08);	//sw reset encode block
					SetA8RegB(0x690a,GetA8RegB(0x690a)&0xf7);
					SetA8RegB(0x6222,0x80);				
					SetA8RegW(0x6242,0xffff);
					SetA8RegW(0x6246,0xffff);
					SetA8RegB(0x6204,0x28); 		
					gAitCaptureStatus = 1;
					if(!(u_int*)pdata)
						status = MMP_INVALID_SIZE;
					AIT_Message_P1("Video Encode Frame Size = %x,",*(u_int*)pdata);	
				}else{
                    			gAitJpegSize = 0;
					gAitCaptureStatus = 1;
					status = MMP_FAILED;
					AIT_Message_P0("NoJPEG\r\n");							
				}
			}
			break;		
				
		case	MMPFUNC_CAM_JPEG_ENCODE:
			break;

		case	MMPFUNC_CAM_SET_PARAM:
			status = cam_IF_ait_camera_config (p1, p2);
			break;
					
		case	MMPFUNC_CAM_PWR_OFF:		
			status = cam_IF_ait_close_camera();
			if(!status)
				status = cam_IF_ait_close_AIT();
			sys_IF_ait_set_status(AIT_STATUS_CAM_CLOSED);
			break;
				
		case MMPFUNC_CAM_UPDATEOSD:
			status = cam_IF_ait_update_OSD((u_short *) client_data, p1, p2, p3, p4);
		
//Ming
		        if(AIT_STATUS_CAM_PREVIEW==sys_IF_ait_get_status()){
		        	u_char reg7111=GetA8RegB(0x7111);
		        	if(!reg7111){
		        		SetA8RegB(0x7111,0x01);
		        	}
		        }
		        
			if(AIT_STATUS_VDO_PLAY==sys_IF_ait_get_status())
			{
				status = A800_UpdateLCD();		        	
			}
		   	break;
			
		case MMPFUNC_CAM_VIDEO_PLAY_START:		
			{
				u_short 	Quality;
				extern u_short	gsCurViewMode;
				short 	jpegWidth,jpegHeight,jpegFormat;

				if((AIT_STATUS_VDO_PLAY != sys_IF_ait_get_status()) || (gAitVidPlaywMode!=gsCurViewMode)){//;AIT_VideoPlayInit == 0){		

					AIT701_ResetLCDWindow();
					status = cam_IF_ait_open_AIT();
					if(status)
						return status;
					playbackConfig.lcdMode = A8_MAIN_LCD;
					if(gAitVidPlaywMode == AIT_VDO_PLAY_FULL_MODE)
						playbackConfig.previewMode = AIT_VDO_PLAY_FULL_MODE;		
					else	
						playbackConfig.previewMode = AIT_VDO_PLAY_NOR_MODE;					
					A8L_GetJpegInfo( (u_short*)client_data, p1, &jpegWidth, &jpegHeight, &jpegFormat ,&Quality);
					status = cam_IF_ait_preview_config(playbackConfig);  
			
					if(CAM_LCD_REFRESH_SCALE == A800Data.Select->LcdRefreshMode){
						cam_IF_ait_preview_LcdScaler(A800Data.Select->BufWidth,A800Data.Select->BufHeight, A800Data.Select->DispWidth, A800Data.Select->DispHeight);							
					}

					if(status != 0)  {
	       				AIT_Message_P1("cam_IF_ait_playback_config is failed = %d\r\n", status);
		                     	return status;
					}
				}
								
				status = cam_IF_ait_vdo_playback_handler((u_short*)client_data, p1);
				if(status != 0) {
					AIT_Message_P1("cam_IF_ait_vdo_playback_handler is failed = %d\r\n", status);
					return status;
				}
				sys_IF_ait_set_status(AIT_STATUS_VDO_PLAY);
				if (p4) {
					status = A800_UpdateLCD();
					if(status != 0) {
						AIT_Message_P1("A800_UpdateLCD is failed = %d\r\n", status);
						return status;
					}
				}	
			}       
			break;
			

		case MMPFUNC_CAM_VIDEO_PLAY_PAUSE:
		case MMPFUNC_CAM_VIDEO_PLAY_RESUME:
			sys_IF_ait_set_status(AIT_STATUS_VDO_PLAY);
			break;		
			
		case MMPFUNC_CAM_VIDEO_PLAY_STOP:
			status = cam_IF_ait_close_AIT();
			if(status != 0){
				AIT_Message_P1("cam_IF_ait_close_AIT is failed = %d\r\n", status);
			}

			AIT_ext_SetLCDRotate(0);
			AIT701_ResetLCDWindow();	
			
			break;

#if defined(AIT_3GP_SUPPORT)
		case MMPFUNC_CAM_VIDEO_PLAY_NORMAL_SCREEN:
//				if(AIT_STATUS_VDO_PLAY != sys_IF_ait_get_status())
				{	
					A8L_CheckLcdBusy();	
					AIT_ext_SetLCDRotate(0);
					AIT701_ResetLCDWindow();
					status = cam_IF_ait_open_AIT();
					if(status)
					{	
	       				AIT_Message_Error("cam_IF_ait_open_AIT is failed = %x\r\n", status);
					
						return status;
					}					
					playbackConfig.lcdMode = A8_MAIN_LCD;
					playbackConfig.previewMode = AIT_VDO_PLAY_YUVBUF_NOR_MODE;					

					status = cam_IF_ait_preview_config(playbackConfig);  
			

					if(status != 0)  {
	       				AIT_Message_P1("cam_IF_ait_playback_config is failed = %d\r\n", status);
		                     	return status;
					}
					
					cam_IF_ait_vdo_playback_YUV420_buf_cfg(AIT_VDO_PLAY_YUVBUF_NOR_MODE,(YUV420BufInfo*)client_data);
					

					
					sys_IF_ait_set_status(AIT_STATUS_VDO_PLAY);
					
				}

			break;

		case MMPFUNC_CAM_VIDEO_PLAY_FULL_SCREEN:
				{	
					A8L_CheckLcdBusy();
					AIT_ext_SetLCDRotate(1);
					AIT701_ResetLCDWindow();
					status = cam_IF_ait_open_AIT();
					if(status)
					{	
	       				AIT_Message_Error("cam_IF_ait_open_AIT is failed = %d\r\n", status);
					
						return status;
					}					
					playbackConfig.lcdMode = A8_MAIN_LCD;
					playbackConfig.previewMode = AIT_VDO_PLAY_YUVBUF_FULL_MODE;					

					status = cam_IF_ait_preview_config(playbackConfig);  
			

					if(status != 0)  {
	       				AIT_Message_P1("cam_IF_ait_playback_config is failed = %d\r\n", status);
		                     	return status;
					}
					
					cam_IF_ait_vdo_playback_YUV420_buf_cfg(AIT_VDO_PLAY_YUVBUF_FULL_MODE,(YUV420BufInfo*)client_data);
					sys_IF_ait_set_status(AIT_STATUS_VDO_PLAY);
					
				}

			break;

			
		case MMPFUNC_CAM_VIDEO_PLAY_YUV_BUF:
			{
				u_short 	Quality;
				short 	jpegWidth,jpegHeight,jpegFormat;



				if(AIT_STATUS_VDO_PLAY != sys_IF_ait_get_status())
				{	
					AIT_ext_SetLCDRotate(0);
					AIT701_ResetLCDWindow();

					status = cam_IF_ait_open_AIT();
					if(status)
					{	
	       				AIT_Message_Error("cam_IF_ait_open_AIT is failed = %d\r\n", status);
					
						return status;
					}					
					playbackConfig.lcdMode = A8_MAIN_LCD;
					playbackConfig.previewMode = AIT_VDO_PLAY_YUVBUF_NOR_MODE;					

					status = cam_IF_ait_preview_config(playbackConfig);  
			

					if(status != 0)  {
	       				AIT_Message_P1("cam_IF_ait_playback_config is failed = %d\r\n", status);
		                     	return status;
					}
					
					cam_IF_ait_vdo_playback_YUV420_buf_cfg(AIT_VDO_PLAY_YUVBUF_NOR_MODE,(YUV420BufInfo*)client_data);
				}

				cam_IF_ait_vdo_playback_YUV420_buf_update((YUV420BufInfo*)client_data);


				sys_IF_ait_set_status(AIT_STATUS_VDO_PLAY);

				status = A800_UpdateLCD();
				if(status != 0) {
					AIT_Message_P1("A800_UpdateLCD is failed = %d\r\n", status);
					return status;
				}
			}       



				break;
#endif /* AIT_3GP_SUPPORT */
			
		 case MMPFUNC_CAM_CHK_SEN_FRAME:		
				A8L_CheckSensorFrame(p1);
				status = MMP_SUCCESS;
		 	break;
		 
	}
	return	status;
}


static mmp_ret_code_type mmpfunc_sd_process(mmp_func_type func, void* client_data, u_int p1, u_int p2, u_int p3, u_int p4, void* pdata)
{
	mmp_ret_code_type status = MMP_SUCCESS;

	if(AIT_STATUS_USB_READY <= sys_IF_ait_get_status())
	{
		AIT_Message_Error("!!!mmpfunc_sd_process: SD is Busy, AIT Status = %d\r\n", sys_IF_ait_get_status());
		return MMP_INVALID_STATE;
	}

	
	switch(func)
	{
		case	MMPFUNC_SD_INIT:
			status = sd_IF_ait_init_sd();
			break;
			
		case	MMPFUNC_SD_OPEN:
                    if(FALSE == gbSdEn)
        			status = sd_IF_ait_init_sd();
                    if(MMP_SUCCESS == status){
			status = sd_IF_ait_open_sd();
                    }else{
                        AIT_Message_P0("MMPFUNC_SD_OPEN  failed\r\n");
                    }
			break;
			
		case	MMPFUCN_SD_CLOSE:
			status = sd_IF_ait_close_sd();
			break;
			
			case	MMPFUNC_SD_READ:				
                    if(FALSE == gbSdEn)
        			status = sd_IF_ait_init_sd();
			if(MMP_SUCCESS == status){
				if(sd_IF_ait_open_sd()==MMP_SUCCESS){
					status = sd_IF_ait_read_sectors(p1, p2, (u_char*)client_data, p3);
				}
			}else{
                        AIT_Message_P0("MMPFUNC_SD_READ  failed\r\n");
			}	
			break;
			
		case	MMPFUNC_SD_WRITE:
                    if(FALSE == gbSdEn)
        			status = sd_IF_ait_init_sd();
                    if(MMP_SUCCESS == status){
			if(sd_IF_ait_open_sd()==MMP_SUCCESS)
			{
				status = sd_IF_ait_write_sectors(p1, p2, (u_char*)client_data, p3);
			}
                    }else{
				AIT_Message_P0("MMPFUNC_SD_WRITE  failed\r\n");
                    }
			break;
			
		case	MMPFUNC_SD_GET_CSD:
			{
				u_char* pCSD=sd_IF_ait_get_CSD();
				AIT_Message_P1("Enter CSD1 Value= %d !\r\n",*pCSD);		
				*(u_int*)pdata=(u_int)pCSD;
				status = MMP_SUCCESS;
			}
			break;
			
		case	MMPFUNC_SD_IO_CTL:
			break;
			
	}
	if(status)	
		AIT_Message_P2("SD Func:%d Status:%d\r\n",func,status);
	
	return	status;	
}
static mmp_ret_code_type mmpfunc_usb_process(mmp_func_type func, void* client_data, u_int p1, u_int p2, u_int p3, u_int p4, void* pdata)
{
	mmp_ret_code_type status = MMP_INVALID_STATE;

	if(AIT_STATUS_USB_READY <= sys_IF_ait_get_status()&&sys_IF_ait_get_status()>AIT_STATUS_CAM_READY)
	{
		AIT_Message_Error("!!!mmpfunc_sd_process: SD is Busy, AIT Status = %d\r\n", sys_IF_ait_get_status());
//Vin: After verify, open the following.		
//		return MMP_INVALID_STATE;	
	}

	
	switch(func)
	{
		case MMPFUCN_USB_DP_PULL_UP:
			sys_IF_ait_set_input_clock(A8_CAM, A8_ON);
			AIT701_DP_PULLUP_ENABLE();
			status = MMP_SUCCESS;
			break;
		case MMPFUCN_USB_DP_PULL_DOWN:
			sys_IF_ait_set_input_clock(A8_CAM, A8_ON);
			AIT701_DP_PULLUP_DISABLE();
			status = MMP_SUCCESS;
			break;
		case	MMPFUCN_USB_DM_PULL_UP:
			sys_IF_ait_set_input_clock(A8_CAM, A8_ON);
			AIT701_DM_PULLUP_ENABLE();
			status = MMP_SUCCESS;
			break;
		case	MMPFUCN_USB_DM_PULL_DOWN:
			sys_IF_ait_set_input_clock(A8_CAM, A8_ON);
			AIT701_DM_PULLUP_DISABLE();
			status = MMP_SUCCESS;
			break;
		case	MMPFUNC_USB_START:
		case	MMPFUNC_USB_STOP:
			if(AIT_STATUS_USB_READY == sys_IF_ait_get_status()){
				status = usb_IF_ait_close_usb();
			}
			break;

		case	MMPFUNC_USB_MSDC:
			status = usb_IF_ait_open_usb(A8_USB_MSDC);
			AIT701_DP_PULLUP_DISABLE();
			sys_IF_ait_delay1ms(300);
			AIT701_DP_PULLUP_ENABLE();
			break;
			
		case	MMPFUCN_USB_PCCAM:	
			status = usb_IF_ait_open_usb(A8_USB_PCCAM);
			AIT701_DP_PULLUP_DISABLE();
			sys_IF_ait_delay1ms(300);
			AIT701_DP_PULLUP_ENABLE();
			break;
	}
	return	status;	
}


static mmp_ret_code_type mmpfunc_sys_process(mmp_func_type func, void* client_data, u_int p1, u_int p2, u_int p3, u_int p4, void* pdata)
{
	mmp_ret_code_type status = MMP_SUCCESS;
	A8_ERROR_MSG retVal = MMP_SUCCESS;
	switch(func)
	{
		case MMPFUNC_SYS_ENTERSLEEP:
		{

			sys_IF_ait_enter_sleep();					

				ait_sleep_flg=1;
				AIT_ext_ClockPinCtl(2);
#if defined(__QSC_TARGET__)
	//Stop timer Handler
    clk_dereg( &ait_hw_clk_cb );
#endif

			AIT701_sleep_flag = 1;			
		}
			break;
			
		case MMPFUNC_SYS_EXITSLEEP:

                    retVal = cam_IF_ait_open_AIT();
                    if(retVal)  {
                        AIT_Message_P1("cam_IF_ait_open_AIT is failed = %d\r\n", retVal);
				status = MMP_FAILED;
                    }
					
			if(MMP_SUCCESS == retVal)
	                    AIT701_sleep_flag = 1;
			break;
			
		case MMPFUNC_SYS_SETGPIO:
			AIT701_set_gpio(p1, p2);
			break;
		default:
			break;
	}
	return	status;
}
	
static mmp_ret_code_type mmpfunc_process(mmp_func_type func, void* client_data, u_int p1, u_int p2, u_int p3, u_int p4, void* pdata)
{
	mmp_ret_code_type status = MMP_INVALID_STATE;

	AIT_ext_Take_Semaphore(0);
	//lcd_busy_waiting();
	//mmpfunc_sleep_disable();

	switch(func)
	{
		case MMPFUNC_CAM_PREVIEW_RDY_CHK:
		case MMPFUNC_SD_READ:
		case MMPFUNC_SD_WRITE:
		case MMPFUNC_CAM_GET_VIDEO_DATA:
			break;

		default:
			AIT_Message_P5("+Func=%x, p1=%d, p2=%d, p3=%d, p4=%d\r\n",func,p1,p2,p3,p4);
			break;
	}
	sys_IF_ait_set_bypass_mode(A8_OFF);		
	if(func<MMPFUNC_SD_CMD)
		status = mmpfunc_cam_process(func, client_data, p1, p2, p3, p4, pdata);
	else if(func<MMPFUNC_USB_CMD)
		status = mmpfunc_sd_process(func, client_data, p1, p2, p3, p4, pdata);
	else if(func<MMPFUNC_SYS_CMD)
		status = mmpfunc_usb_process(func, client_data, p1, p2, p3, p4, pdata);
	else if(func<MMPFUNC_MAX)
		status = mmpfunc_sys_process(func, client_data, p1, p2, p3, p4, pdata);

	switch(sys_IF_ait_get_status())
	{
		case AIT_STATUS_CAM_PREVIEW:
#if AIT_VIDEO_PHONE_SUPPORT
		//vid phne only need to get frame from AIT, MMI will handle refresh LCD
		//so, need to set bypass on
		if(bVideoChat)
		{
			sys_IF_ait_set_bypass_mode(A8_ON);		
			break;
		}	
#endif
		case AIT_STATUS_VDO_RECORD:
		case AIT_STATUS_VDO_PLAY:
		case AIT_STATUS_VDO_PLAY_PAUSE:
			break;
		default:
		sys_IF_ait_set_bypass_mode(A8_ON);		
			break;
	}

	switch(func)
	{
		case MMPFUNC_CAM_PREVIEW_RDY_CHK:
		case MMPFUNC_SD_READ:
		case MMPFUNC_SD_WRITE:
		case MMPFUNC_CAM_GET_VIDEO_DATA:
			break;

		default:

		AIT_Message_P2("\r\n-Func=%x, status=%d\r\n",func,status);
			break;
	}
	if(status)
		MSG_FATAL("!!AIT Func Fail =%x ,P1=%d,P2=%d!!!!!",0,0,0);
		
	//mmpfunc_sleep_enable();
	AIT_ext_Give_Semaphore();
	return status;
}

void AIT701_reg_mem_test(void)
{	
   	sys_IF_ait_set_bypass_mode(A8_OFF);
	
	A800_TestRegisterAccess();
   	A800_TestMemoryAccess();
	
	sys_IF_ait_set_bypass_mode(A8_ON);
	return;
}


void AIT701_system_init(void)
{
	u_char retVal;
	
	retVal = 	mmpfunc_process(MMPFUNC_CAM_INIT, 0, 0, 0, 0, 0, 0);

	if(retVal != 0)
	{
		AIT_Message_P1("sys_IF_ait_boot_init is failed = %d\r\n", retVal);
		return;
	}
	
	AIT_Message_P0("-AIT701_system_init\r\n");
	return;
}

static void AIT701_cam_power_on(void)
{
	mmpfunc_process(MMPFUNC_CAM_PWR_ON, 0, 0, 0, 0, 0, 0);
	return;
}

static void AIT701_cam_power_off(void)
{
	mmpfunc_process(MMPFUNC_CAM_PWR_OFF, 0, 0, 0, 0, 0, 0);
	return;
}

#if defined(AIT_READ_SENSOR_ID_FROM_NVRAM)&&AIT_READ_SENSOR_ID_FROM_NVRAM==1
static unsigned short AIT701_cam_detect_sensor_id(unsigned char camera_id)
{
	mmp_ret_code_type ret;
	unsigned short sensorID;
	if(camera_id>1)
		return 0xffff;

	ret =  mmpfunc_process(MMPFUNC_CAM_DETECT_SENSOR_ID, 0,camera_id,  0, 0, 0, &sensorID);
	AIT_Message_P1("AIT701_cam_detect_sensor_id: %x\r\n",sensorID);

	if(ret)
		return 0xffff;
	else
		return sensorID;
}

static unsigned short AIT701_cam_search_sensor_id(unsigned short sensorID)
{
	mmp_ret_code_type  ret = 0;

	ret = mmpfunc_process(MMPFUNC_CAM_SEARCH_SENSOR_ID, 0, sensorID, 0, 0, 0, 0);
	AIT_Message_P1("AIT701_cam_search_sensor_id: %x\r\n",sensorID);
	return ret;
}
#endif

static void AIT701_cam_cmd_mapping()
{
	AIT_ext_cam_cmd_mapping();
}

void cam_test(void)
{
	ext_camera_para_struct ext_cam_para = {0};

	ext_cam_para.preview_src = AIT_TV_SRC;
	AIT701_cam_preview(&ext_cam_para);
}

static void AIT701_cam_preview(ext_camera_para_struct *ext_cam_para)
{
	extern u_char g_ATV_Flag;
	ASSERT(ext_cam_para!=NULL);
#ifdef AIT_ATV_SUPPORT
	if(g_ATV_Flag)
	{
		ext_cam_para->preview_src = (unsigned char)	AIT_TV_SRC;
		#ifdef __MMI_MAINLCD_128X160__
			mmpfunc_process(MMPFUNC_CAM_PREVIEW_START,ext_cam_para,AIT_ATV_PREV_NOR_MODE,AIT_TV_SRC,0,0,0);
		#else
			mmpfunc_process(MMPFUNC_CAM_PREVIEW_START,ext_cam_para,AIT_ATV_PREV_NOR_MODE,AIT_TV_SRC,0,0,0);
		#endif
	}
	else
#endif	
	{
		if(ait_is_active_cam()==1){
#ifdef __SXMOBI_VC_SUPPORT__		
		#if AIT_VIDEO_PHONE_SUPPORT
            if(1 == bVideoChat)
            {
                mmpfunc_process(MMPFUNC_CAM_PREVIEW_START, ext_cam_para, AIT_VIDPHONE_PREV_MODE, 0, 0, 0, 0);
            }
            else
		#endif
#endif		
			mmpfunc_process(MMPFUNC_CAM_PREVIEW_START, ext_cam_para,AIT_VDO_PREV_MODE , 0, 0, 0, 0);
		}
		else
		{
			mmpfunc_process(MMPFUNC_CAM_PREVIEW_START, ext_cam_para, AIT_CAM_PREV_FULL_MODE , 0, 0, 0, 0);
		}
	}
	return;
}

static void AIT701_cam_exit_preview(void)
{
	mmpfunc_process(MMPFUNC_CAM_PREVIEW_STOP, 0, 0, 0, 0, 0, 0);
	return;
}

u_char AIT701_cam_pause_lcd_refresh(void)
{
	AIT_Message_P0("+AIT701_cam_pause_lcd_refresh\r\n");
	
	AIT_Message_P0("-AIT701_cam_pause_lcd_refresh\r\n");
	return 0;
}

u_char AIT701_cam_resume_lcd_refresh(void)
{
	AIT_Message_P0("+AIT701_cam_resume_lcd_refresh\r\n");

	AIT_Message_P0("-AIT701_cam_resume_lcd_refresh\r\n");
	return 0;
}

static void AIT701_cam_capture(ext_camera_para_struct *ext_cam_para)
{
	ASSERT(ext_cam_para!=NULL);

	mmpfunc_process(MMPFUNC_CAM_CAPTURE, ext_cam_para, 0, 0, 0, 0, 0);
	return;
}

static unsigned int AIT701_cam_capture_ready_check(void)
{
	unsigned int CaptureRady = 0;
	mmpfunc_process(MMPFUNC_CAM_CAPTURE_RDY_CHK, 0, 0, 0, 0, 0, &CaptureRady);
	AIT_Message_P1("MMPFUNC_CAM_CAPTURE_RDY_CHK CaptureRady = %d\r\n",CaptureRady);				
	return	CaptureRady;
}




static unsigned char AIT701_cam_reset_status(void)
{
	return 0;
}

static unsigned char AIT701_cam_preview_ready_check(void)
{
	unsigned int PrevewRady = 0;
	mmpfunc_process(MMPFUNC_CAM_PREVIEW_RDY_CHK, 0, 0, 0, 0, 0, &PrevewRady);
	return	PrevewRady;
}

static unsigned int AIT701_cam_jpeg_encode(ext_camera_para_struct *ext_cam_para, unsigned char back_to_preview)
{	
	AIT_Message_P0("+AIT701_cam_jpeg_encode\r\n");
	ASSERT(ext_cam_para!=NULL);

	return mmpfunc_process(MMPFUNC_CAM_GET_VIDEO_DATA, ext_cam_para, 0, 0, 0, 0, 0);
}

static void AIT701_cam_write_para(unsigned short cmd, unsigned short para)
{
	AIT_Message_P0("+AIT701_cam_write_para\r\n");

	AIT_Message_P0("-AIT701_cam_write_para\r\n");
	
	return;
}

static unsigned short AIT701_cam_read_para(unsigned short cmd)
{
	AIT_Message_P0("+AIT701_cam_read_para\r\n");

	AIT_Message_P0("-AIT701_cam_read_para\r\n");
	
	return 0;
}

static void AIT701_cam_set_para(unsigned char cmd, unsigned char para)
{
	mmpfunc_process(MMPFUNC_CAM_SET_PARAM, 0, cmd, para, 0, 0, 0);
}

static unsigned char AIT701_cam_frame_rate(void)
{
	AIT_Message_P0("+AIT701_cam_frame_rate\r\n");

	AIT_Message_P0("-AIT701_cam_frame_rate\r\n");
	
	return 15;
}


unsigned char* pFileBuff;
unsigned int jpegfilesize=0;
static void AIT701_Camera_MJPEG_Record_Start(void)
{

	mmpfunc_process(MMPFUNC_CAM_VIDEO_RECORD_START, 0, 0, 0, 0, 0, 0);
	dcam_state=0;

	EINT_UnMask(DCAM_EINT_NO);
}

static void AIT701_Camera_MJPEG_Record_Pause(void)
{
	mmpfunc_process(MMPFUNC_CAM_VIDEO_RECORD_STOP, 0, 0, 0, 0, 0, 0);
	EINT_Mask(DCAM_EINT_NO);
	dcam_state=0;
}

static void AIT701_Camera_MJPEG_Record_Resume(void)
{
	mmpfunc_process(MMPFUNC_CAM_VIDEO_RECORD_START, 0, 0, 0, 0, 0, 0);
	EINT_UnMask(DCAM_EINT_NO);
	dcam_state=0;
}

static void AIT701_Camera_MJPEG_Record_Stop(void)
{
	mmpfunc_process(MMPFUNC_CAM_VIDEO_RECORD_STOP, 0, 0, 0, 0, 0, 0);
	EINT_Mask(DCAM_EINT_NO);
	dcam_state=0;
}


unsigned char* AIT701_Camera_MJPEG_Encode(unsigned char* pFileBuff, unsigned int* pJpegfilesize)
{
	unsigned char* ptmpFileBuff = pFileBuff;

	u_int jpegfilesize=0;
	ASSERT(pFileBuff!=NULL);
	ASSERT(pJpegfilesize!=NULL);
	AIT_Message_P0("+AIT_Camera_MJPEG_Encode\r\n");
	
	mmpfunc_process(MMPFUNC_CAM_GET_VIDEO_DATA, ptmpFileBuff, 0, 0, 0, 0, &jpegfilesize);

	*pJpegfilesize=jpegfilesize;
	return pFileBuff;
}


int AIT701_Camera_MJPEG_Interrupt(unsigned char* pJpegFile)
{
	ASSERT(pJpegFile!=NULL);

	#if 1
	dcam_state=1-dcam_state;
	EINT_Set_Polarity(DCAM_EINT_NO,dcam_state);
	if(dcam_state==1)
	{
	}
	
	#else
	pFileBuff=pJpegFile;
	mmpfunc_process(MMPFUNC_CAM_GET_VIDEO_DATA, pFileBuff, 0, 0, 0, 0, &jpegfilesize);
	#endif

	return dcam_state;
}


unsigned char AIT701_JpegDecode(unsigned short * jpegBufPtr, unsigned int jpegsize, unsigned int panelwidth, unsigned int panelheight, unsigned char* RGBBufPtr,unsigned int RBGBufStartXFill )
{
	ASSERT(jpegBufPtr!=NULL);
	return mmpfunc_process(MMPFUNC_CAM_JPEG_DECODE,jpegBufPtr, jpegsize, panelwidth, panelheight, RBGBufStartXFill,RGBBufPtr );
}


#if defined(AIT_3GP_SUPPORT)
u_char AIT701_vdoply_ScreenModeSwitch(unsigned char bFullScreen)
{
	mmp_ret_code_type fRet = MMP_SUCCESS;;
	ASSERT(pAIT_YUV420frameBufInfo!=NULL);

	if(bIsVidPlayFullScreen!=bFullScreen||sys_IF_ait_get_status()!=AIT_STATUS_VDO_PLAY)
	{
		if(bFullScreen)
			fRet = mmpfunc_process(MMPFUNC_CAM_VIDEO_PLAY_FULL_SCREEN, pAIT_YUV420frameBufInfo, 0, 0,0, 0, 0);
		else
			fRet = mmpfunc_process(MMPFUNC_CAM_VIDEO_PLAY_NORMAL_SCREEN, pAIT_YUV420frameBufInfo,0,0,0, 0, 0);
	}
	bIsVidPlayFullScreen = bFullScreen;

}


u_char AIT701_vdoply_YUVBuf_playback(YUV420BufInfo* pYUV420BufInfo, u_int rotate,u_char bShowTitleOSD,u_char update_osd)
{
	mmp_ret_code_type fRet = MMP_SUCCESS;

	ASSERT(pYUV420BufInfo!=NULL);
	ASSERT(pYUV420BufInfo->ptrYbuf!=NULL);
	ASSERT(pYUV420BufInfo->ptrUbuf!=NULL);
	ASSERT(pYUV420BufInfo->ptrVbuf!=NULL);
	pAIT_YUV420frameBufInfo = pYUV420BufInfo;


	fRet = mmpfunc_process(MMPFUNC_CAM_VIDEO_PLAY_YUV_BUF, pYUV420BufInfo, 0, rotate, bShowTitleOSD, 0, 0);
	

	return fRet;

}
unsigned short testn1 = 0;

#endif
static u_char AIT701_avi_playback(u_short *frameBuf, u_int frameSize, u_short image_width, u_short image_height, u_char update_to_lcd)
{
	extern unsigned short gA8MainLCDWidth ;
	extern unsigned short gA8MainLCDHeight;

	mmp_ret_code_type fRet = MMP_SUCCESS;
	ASSERT(frameBuf!=NULL);

#if (AIT_YUV_BUF_VDOPLY_TEST ==1 && defined(AIT_3GP_SUPPORT))
{
	unsigned char bRotate = 0;
	pAIT_YUV420frameBufInfo = &YUV420frameBufTest;
	if(YUV420frameBufTest.ptrYbuf==0)
	{
		YUV420frameBufTest.u16VideoWidth = 224;	//Availagle Video width & Height
		testn1++;
		YUV420frameBufTest.u16VideoHeight = 176;
		 
		YUV420frameBufTest.u16YBufWidth = 220;
		YUV420frameBufTest.u16UVBufWidth = 110;

		YUV420frameBufTest.ptrYbuf = (unsigned char*)YUV420RAW;
		YUV420frameBufTest.ptrUbuf = (unsigned char*)(YUV420RAW+YUV420frameBufTest.u16YBufWidth*YUV420frameBufTest.u16VideoHeight);
		YUV420frameBufTest.ptrVbuf = (unsigned char*)(YUV420RAW+YUV420frameBufTest.u16YBufWidth*YUV420frameBufTest.u16VideoHeight+YUV420frameBufTest.u16UVBufWidth*YUV420frameBufTest.u16VideoHeight/2);

		YUV420frameBufTest.pOsdBufTitle = (kal_uint16 *)GetVdoPly_Osd_Buffer();
		YUV420frameBufTest.osd_Title.x = 0; 
		YUV420frameBufTest.osd_Title.y= 0; 
		YUV420frameBufTest.osd_Title.w = 176; 
		YUV420frameBufTest.osd_Title.h = 18;

		YUV420frameBufTest.pOsdBufStatus = (kal_uint16 *)GetVdoPly_Osd_Buffer()+176*(220-58)*2;
		YUV420frameBufTest.osd_Status.x = 0; 
		YUV420frameBufTest.osd_Status.y = 220-58; 
		YUV420frameBufTest.osd_Status.w = 176; 
		YUV420frameBufTest.osd_Status.h = 58;

		
	 
	}
	
	fRet= AIT701_vdoply_YUVBuf_playback(&YUV420frameBufTest, 0,1,0);
	
}	
#else


	AIT_Message_P0("+AIT701_avi_playback\r\n");
	if((image_width==gA8MainLCDWidth && image_height==gA8MainLCDHeight) || (image_height==gA8MainLCDWidth && image_width==gA8MainLCDHeight))
		gAitVidPlaywMode = AIT_VDO_PLAY_FULL_MODE; 
	else
		gAitVidPlaywMode = AIT_VDO_PLAY_NOR_MODE; 
	fRet = mmpfunc_process(MMPFUNC_CAM_VIDEO_PLAY_START, frameBuf, frameSize, image_width, image_height, update_to_lcd, 0);
	if(MMP_SUCCESS == fRet){
	}	
#endif	
	return  fRet;
}

static void AIT701_avi_playback_pause(void)
{
	mmpfunc_process(MMPFUNC_CAM_VIDEO_PLAY_PAUSE, 0, 0, 0, 0, 0, 0);
	return;
}

static void AIT701_avi_playback_resume(void)
{
	mmpfunc_process(MMPFUNC_CAM_VIDEO_PLAY_RESUME, 0, 0, 0, 0, 0, 0);
	return;	
}

static void AIT701_avi_playback_stop(void)
{
    mmpfunc_process(MMPFUNC_CAM_VIDEO_PLAY_STOP, 0, 0, 0, 0, 0, 0);
	return;
}




void AIT701_enter_sleep(void)
{
	AIT_Message_P0("+AIT701_enter_sleep\r\n");

	AIT_ext_SetLCDRotate(0);	

	mmpfunc_process(MMPFUNC_SYS_ENTERSLEEP, 0, 0, 0, 0, 0, 0);
	
	return;
}

void AIT701_exit_sleep(void)
{
	AIT_Message_P0("+AIT701_exit_sleep\r\n");
	if(AIT701_sleep_flag)
	{
		mmpfunc_process(MMPFUNC_SYS_EXITSLEEP, 0, 0, 0, 0, 0, 0);
	}
	return;
}


static void AIT701_set_gpio(u_short pin, u_char level)
{
	if (level==1) {
		A800_SetGPIO(pin);
	} else {
		A800_ClearGPIO(pin);
	}
	return;
}


unsigned short AIT_SD_InitializeFlag=0;
static unsigned char* AIT701_SD_Initialize(void)
{
	unsigned char* pCSD=NULL;
	unsigned short ret = MMP_SUCCESS;
	static unsigned isinit=0;

	AIT_Message_P0("Enter AIT_SD_Initialize!\r\n");

	if(0 == isinit){
		mmpfunc_process(MMPFUNC_CAM_INIT, 0, 0, 0, 0, 0, 0);
		isinit = 1;
	}

	ret = mmpfunc_process(MMPFUNC_SD_INIT, 0, 0, 0, 0, 0, 0);

	if(!ret){
		
		ret = mmpfunc_process(MMPFUNC_SD_OPEN, 0, 0, 0, 0, 0, 0);
		if(!ret){
			
			ret = mmpfunc_process(MMPFUNC_SD_GET_CSD, 0, 0, 0, 0, 0, &pCSD);
		}
		mmpfunc_process(MMPFUCN_SD_CLOSE, 0, 0, 0, 0, 0, 0);
	}
		

	return pCSD;
}

static unsigned short AIT701_SD_ReadSector( unsigned int startsect,unsigned int offset,unsigned char *buf,unsigned int read_size )
{
	unsigned short retVal=0xFF;
	ASSERT(buf!=NULL);

	retVal = (u_char)mmpfunc_process(MMPFUNC_SD_READ, buf, startsect, offset, read_size, 0, 0);
	return retVal;
}


static unsigned short AIT701_SD_WriteSector( unsigned int startsect,unsigned int offset,unsigned char *buf,unsigned int write_size )
{
	unsigned short retVal=0xFF;
	ASSERT(buf!=NULL);

	retVal = (u_char)mmpfunc_process(MMPFUNC_SD_WRITE, buf, startsect, offset, write_size, 0, 0);
	return  retVal;
}

u_char* AIT701_sd_get_CSD()
{
	u_int	pCSD_Addr;
	mmpfunc_process(MMPFUNC_SD_GET_CSD, 0, 0, 0, 0, 0, &pCSD_Addr);
	AIT_Message_P1("CSD=0x%x", pCSD_Addr);
	return (u_char*)pCSD_Addr;
}

u_int  AIT701_sd_get_Size()
{
	u_int	sd_size ;
	sd_size = sd_IF_ait_ioctl() * 512 ;
	AIT_Message_P1("T Card Capability =0x%x  Byte\r\n", sd_size);
	return sd_size;
}

u_char	AIT701_cam_check_frame(u_char mode)
{
	return mmpfunc_process(MMPFUNC_CAM_CHK_SEN_FRAME,0,mode,0,0,0,0);
}
static void AIT701_usb_DP_pull_up(void)
{
	mmpfunc_process(MMPFUCN_USB_DP_PULL_UP,0, 0, 0, 0, 0, 0);
	return;
}

static void AIT701_usb_DP_pull_down(void)
{
    mmpfunc_process(MMPFUCN_USB_DP_PULL_DOWN,0, 0, 0, 0, 0, 0);
	return;
}

static void AIT701_usb_DM_pull_up(void)
{
	mmpfunc_process(MMPFUCN_USB_DM_PULL_UP,0, 0, 0, 0, 0, 0);
	return;
}

static void AIT701_usb_DM_pull_down(void)
{
    mmpfunc_process(MMPFUCN_USB_DM_PULL_DOWN,0, 0, 0, 0, 0, 0);
	return;
}


static void AIT701_usb_msdc_start(void)
{
	mmpfunc_process(MMPFUNC_USB_MSDC,0, 0, 0, 0, 0, 0);
	return;
}

static void AIT701_usb_msdc_stop(void)
{
    mmpfunc_process(MMPFUNC_USB_STOP,0, 0, 0, 0, 0, 0);
	return;
}

static void AIT701_USB_WebCAMActive(void)
{
    mmpfunc_process(MMPFUCN_USB_PCCAM,0, 0, 0, 0, 0, 0);
	return;
}


void AIT701_cam_update_osd(u_short *osdBuffer, u_short start_x, u_short start_y, u_short width, u_short height)
{
	ASSERT(osdBuffer!=NULL);
	if(1==sys_IF_ait_get_status())
	{
		AIT_Message_P0("!!Warning!LCD is in Bypass mode");				
		return;
	}
#ifndef WIN32
	mmpfunc_process(MMPFUNC_CAM_UPDATEOSD, osdBuffer, start_x, start_y, width, height,0);
#endif

	return;
}

#if AIT_VIDEO_PHONE_SUPPORT



void AIT701_Camera_Video_Phone_Timer_Hander(void)
{
    
}

unsigned int AIT701_Camera_Video_Phone_Get_Raw_Data(unsigned short Width, unsigned short Height, unsigned char *pYUVBuff)
{
	unsigned char *ptmpYuvBuff = pYUVBuff;
	u_int return_size=0;

	mmpfunc_process(MMPFUNC_CAM_VIDEO_PHONE_GET_RAW_DATA, ptmpYuvBuff, Width, Height, 0, 0, &return_size);
	return return_size;
}

u_char AIT701_cam_VidPhone_On() 
{ 
        u_char status=0;
        #if (AIT_VIDPHONE_TEST_ENABLE==0)
        status = mmpfunc_process(MMPFUNC_CAM_PWR_ON, 0, 0, 0, 0, 0, 0); 
        #endif
        return status; 
} 
u_char AIT701_cam_VidPhone_Preview_Start(u_int w, u_int h, u_char max_fps) 
{ 
        u_char status=0; 
        #if (AIT_VIDPHONE_TEST_ENABLE==0)
        status =  mmpfunc_process(MMPFUNC_CAM_PREVIEW_START, 0, AIT_VIDPHONE_PREV_MODE, 0, 0, 0, 0); 
		#endif
        return status; 
} 

extern char VidPhoneTestDataFlag=0;
u_char AIT701_cam_VidPhone_Get_Frame(u_char* des_buff, u_char* type, u_int *size) 
{ 
        u_char status=0; 
        u_int return_size=0;
        u_short Width=80, Height=64;
        unsigned char *ptmpYuvBuff = des_buff;
        *type = 1; // 0:JPEG, 1:RGB565, 2:YUV420 
		*size=80*64;
		
        #if AIT_VIDPHONE_TEST_ENABLE
		{
			int i=0;
			u_short *tembuf=  (u_short *)des_buff;
			for(i=0;i<(80*64);i++)
			{
				if(VidPhoneTestDataFlag)
					*(tembuf+i) = 0xF800; //red
				else
					*(tembuf+i) = 0x001F; //blue	
			}
			VidPhoneTestDataFlag++;

			if(VidPhoneTestDataFlag>1)
				VidPhoneTestDataFlag = 0;
		}
        #else
		//AIT_Message_P0("+AIT701_Camera_Video_Phone_Get_Raw_Data\r\n");	
		mmpfunc_process(MMPFUNC_CAM_VIDEO_PHONE_GET_RAW_DATA, ptmpYuvBuff, Width, Height, 0, 0, &return_size);
		#endif
        return status; 
} 
u_char AIT701_cam_VidPhone_Off() 
{ 
        u_char status=0; 
        #if (AIT_VIDPHONE_TEST_ENABLE==0)
        status = mmpfunc_process(MMPFUNC_CAM_PREVIEW_STOP, 0, 0, 0, 0, 0, 0); 
        //if(!status) 
        //        status =  mmpfunc_process(MMPFUNC_CAM_PREVIEW_START, 0, AIT_VIDPHONE_PREV_MODE, 0, 0, 0, 0); 
        #endif
        return status; 
} 

#endif

ait_cam_func AIT701_cam_module_func =
{	
	 AIT701_cam_power_on
	,AIT701_cam_cmd_mapping
	,AIT701_cam_reset_status
	,AIT701_cam_preview_ready_check
	,AIT701_cam_capture_ready_check
	,AIT701_cam_power_off
	,AIT701_cam_preview
	,AIT701_cam_exit_preview
	,AIT701_cam_capture
	,AIT701_cam_jpeg_encode
	,AIT701_cam_set_para
	,AIT701_cam_write_para
	,AIT701_cam_read_para
	,AIT701_cam_frame_rate
};

ait_video_rec_func AIT701_Video_Record_Func = 
{
	AIT701_Camera_MJPEG_Record_Start,
	AIT701_Camera_MJPEG_Record_Pause,
	AIT701_Camera_MJPEG_Record_Resume,
	AIT701_Camera_MJPEG_Record_Stop,
	AIT701_Camera_MJPEG_Encode,
	AIT701_Camera_MJPEG_Interrupt
}; 


ait_video_play_func AIT701_Video_Play_Func =
{
	AIT701_avi_playback,
	AIT701_avi_playback_pause,
	AIT701_avi_playback_resume,
	AIT701_avi_playback_stop
};


ait_sd_func AIT701_SD_Func =
{
	AIT701_SD_Initialize,
	AIT701_SD_ReadSector,
	AIT701_SD_WriteSector,
	AIT701_sd_get_Size
};

ait_usb_func AIT701_USB_Func =
{
	AIT701_usb_msdc_start,
	AIT701_usb_msdc_stop,
	AIT701_USB_WebCAMActive,
	AIT701_usb_DP_pull_up,
	AIT701_usb_DP_pull_down,
	AIT701_usb_DM_pull_up,
	AIT701_usb_DM_pull_down
} ;


#if defined (MTK_PLATFORM)
/*===========================================================================

FUNCTION cam_module_func_config

DESCRIPTION
    This function maps the external camera module function API structure.
	This is for the MB2168 porting to the MTK platform.

PARAMETERS
  None

DEPENDENCIES
  Target external camera module feature should be defined.

RETURN VALUE
  None

SIDE EFFECTS
  External camera module function is hooked.

===========================================================================*/

ait_cam_module_func ait_cam_func;
void image_sensor_func_config(void)
{
}
void cam_module_func_config(void)
{
	ait_cam_func.cam_module_cmd_mapping=AIT701_cam_cmd_mapping;
	ait_cam_func.cam_module_power_on=AIT701_cam_power_on;
	ait_cam_func.cam_module_power_off=AIT701_cam_power_off;
	ait_cam_func.cam_module_reset_status=AIT701_cam_reset_status;
	ait_cam_func.cam_module_set_para=AIT701_cam_set_para;
	ait_cam_func.cam_module_read_para=AIT701_cam_read_para;
	ait_cam_func.cam_module_write_para=AIT701_cam_write_para;
	ait_cam_func.cam_module_preview=AIT701_cam_preview;
	ait_cam_func.exit_cam_module_preview=AIT701_cam_exit_preview;
	ait_cam_func.cam_module_preview_ready_check=AIT701_cam_preview_ready_check;
	ait_cam_func.cam_module_capture=AIT701_cam_capture;
	ait_cam_func.cam_module_capture_ready_check=AIT701_cam_capture_ready_check;
	ait_cam_func.cam_module_jpeg_encode=AIT701_cam_jpeg_encode;
	ait_cam_func.cam_module_frame_rate=AIT701_cam_frame_rate;
#if defined(AIT_READ_SENSOR_ID_FROM_NVRAM)
	ait_cam_func.cam_module_detect_sensor_id = AIT701_cam_detect_sensor_id;
	ait_cam_func.cam_module_search_sensor_id = AIT701_cam_search_sensor_id;
#endif
	ext_cam_func=&ait_cam_func;
	//ext_cam_func=&AIT701_cam_module_func;
	return;
}
#endif

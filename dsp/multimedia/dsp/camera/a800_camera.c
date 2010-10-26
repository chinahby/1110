/**
 * @file a800camera.c
 * @brief Copyright (C) 2002-2008 Alpha Imaging Technology Corp.
 * @n AIT701/703 Series Host API.
 * @n The purpose of AIT701 Host API is for Host CPU to control and communicate AIT701
 *      chip via direct/indirect parallel interface.
 * @note N/A
 * @bug N/A
 */
#include "float.h"
#include "a800_camera.h"
#include "cam_IF_ait_api.h"
#include "a8_common.h"
#include "a8_vif.h"
#include "AIT_Interface.h"


#define Progress

#define A8_STILL_CAPTURE_ZOOM_SUPPORT

#define A8_SENSOR_WIDTH			(640)
#define A8_SENSOR_HEIGHT			(480)

extern void	A800_InitLCDLookUpTable(void);
//-------------------------------------------------------------------------//
// The variables are defined in A8_panel.c for main/sub LCD dimension
//Vin@20091219: Select Panel Size in same lib.
extern unsigned short gA8MainLCDWidth;
extern unsigned short gA8MainLCDHeight;
extern unsigned short gA8SubLCDWidth;
extern unsigned short gA8SubLCDHeight;
//-------------------------------------------------------------------------//
extern	u_char gCurrentCaptureResolution;
u_short gAITInitialFailed = 1;
u_short gAITJpegQuality;
u_short	mOSD_DEPTH = A8_DEPTH_16B;

A800InfoDT  A800Data;

u_short  gsCurViewMode =0xff;	//Will be initialized in A800_InitDefine

u_short gA8CurrentCaptureResolution = 0;       // It is used for getting the capture resolution at preview time

extern StructBUF	gsBuffer[MAX_BUFFER];


u_short APIRun = 0;

u_short EntryAPI(void)
{
	if (APIRun==0)
	{
		APIRun=1;
	}
	return APIRun;
}

void ExitAPI(void)
{
	APIRun=0;
}

#define EntryAITAPI    if (EntryAPI()==0)  return 0xFF  /* if return 0xFF => means other AIT API is running! */
#define ExitAITAPI    ExitAPI()
/*---------------------------------------------------------- */
u_short gA8PrevZoomFactor = 0;  //
/*---------------------------------------------------------- */

u_short VideoPlayScreenMode;
/*---------------------------------------------------------- */


u_int*  offsetaddr;
u_short* frametime; 
u_char* Y_color;
u_char* Cr_color;
u_char* Cb_color;
u_char *LastImageBuf;

u_short ZoomRate_Org=0;


extern int A8L_SetVideoResolution(u_short width, u_short height, u_short colorFormat, u_short fileFormat);
extern int A8L_SetVideoCaptureMemConfig(u_int encodeAddrStart, u_int encodeAddrEnd, u_int lineBufAddr);
extern int A8L_StartMJPEGCapture(void);
extern int A8L_TransferMJPEGFrameToHost(u_char *frameBuf, u_int *frameSize);


/**
 @fn		u_char A800_InitDefine(void)
 @brief	Init all the Preview and Playback environment struct setting.
 @return	u_char
 @note	The Preview and Playback window setting all depends on Customer.
 @bug	N/A.
*/

u_char A800_InitDefine(void)
{
	extern unsigned short gA8MainLCDWidth;
	extern unsigned short gA8MainLCDHeight;
	extern unsigned short gA8SubLCDWidth;
	extern unsigned short gA8SubLCDHeight;
	extern const sPanel_Ctrl_Attrib gMainPanelControlAttrib;
#ifndef A8_DISABLE_SUB_LCD_FUNCTION
	extern const sPanel_Ctrl_Attrib gMainPanelControlAttrib;
#endif
	A800Data.LCDCount = 1;     /* Main LCD only */
	A800Data.ActiveLCDNo = 0;  /* Main LCD active */
	
	A800Data.MainLCDInfo.Width = gA8MainLCDWidth;
	A800Data.MainLCDInfo.Height = gA8MainLCDHeight;
	A800Data.MainLCDInfo.BusWidth = gMainPanelControlAttrib.LCD_BusCtl.BusWidth;
	A800Data.MainLCDInfo.BusType = gMainPanelControlAttrib.LCD_BusCtl.BusType;
	A800Data.MainLCDInfo.Burst = gMainPanelControlAttrib.LCD_BusCtl.Burst ;

#ifndef A8_DISABLE_SUB_LCD_FUNCTION
	A800Data.SubLCDInfo.Width = gA8SubLCDWidth;
	A800Data.SubLCDInfo.Height = gA8SubLCDHeight;

	A800Data.SubLCDInfo.BusWidth = gMainPanelControlAttrib.LCD_BusCtl.BusWidth;
	A800Data.SubLCDInfo.BusType = gMainPanelControlAttrib.LCD_BusCtl.BusType;
	A800Data.SubLCDInfo.Burst =  gMainPanelControlAttrib.LCD_BusCtl.Burst;
#endif

	A800Data.SensorDes.MaxWidth = 640;
	A800Data.SensorDes.MaxHeight = 480;
	A800Data.SensorDes.PrevWidth = 640;
	A800Data.SensorDes.PrevHeight = 480;
	A800Data.SensorDes.IspUsed = 1;

	A800Data.Select = &(A800Data.AITPreviewOSDModeSetting);

	A800Data.Select->WinState =  A8_MAIN_WIN_ON | A8_PIP_WIN_ON | A8_OVERLAY_WIN_ON | A8_ICON_WIN_ON;

	A800Data.Select->WinPriority = A8_WIN_PRIORITY_NORM_ALL;
	A800Data.Select->TransColor = 0x1F;


	A800_ChangeCameraSetting(AIT_VDO_PLAY_NOR_MODE);
	
	return A8_NO_ERROR;
}


WinInfoDT *A800_ChangeCameraSetting(PREVIEW_MODE mode)
{
	extern 	sLCD_ATTRIBUITE gsPreviewAttrib[];
	extern t_sensor_manager* gsSensorUsing;
	extern const unsigned short maxNumPrevMode;

	WinInfoDT *Select = &(A800Data.AITPreviewOSDModeSetting);
	unsigned char PreviewModeIndex = 0;
	unsigned short OSDBufFormat = A8_DEPTH_16B;
	unsigned char bufNumColorBytes = 2;
	unsigned int temUpScalew=0,temUpScaleh=0;
	if(mode==gsCurViewMode)
		return &(A800Data.AITPreviewOSDModeSetting);
	else
		gsCurViewMode = mode;
	
	for(PreviewModeIndex=0;gsPreviewAttrib[PreviewModeIndex].mode!=maxNumPrevMode;++PreviewModeIndex)
	{
		AIT_Message_P1("gsPreviewAttrib[PreviewModeIndex].mode = %x\r\n",gsPreviewAttrib[PreviewModeIndex].mode);

		if(gsPreviewAttrib[PreviewModeIndex].mode==mode)
		{
			AIT_Message_P1("OSD mode found = %x\r\n",gsPreviewAttrib[PreviewModeIndex].mode);
			break;
		}
	}
	
	if(gsPreviewAttrib[PreviewModeIndex].mode==maxNumPrevMode)
		return NULL;


	switch(mode)
	{
		default:
		{
			if(!gsSensorUsing)
			{
				AIT_Message_P0("gsSensorUsing = NULL\r\n");
				return NULL;
			}
			AIT_Message_P1("gsSensorUsing ID= %x\r\n",gsSensorUsing->sensor_id);

			A800Data.SensorDes.MaxWidth = gsSensorUsing->preview_mode->image_width;
			A800Data.SensorDes.MaxHeight = gsSensorUsing->preview_mode->image_height;
			A800Data.SensorDes.PrevWidth = gsSensorUsing->preview_mode->image_width;
			A800Data.SensorDes.PrevHeight = gsSensorUsing->preview_mode->image_height;
		
			}
			break;

			case AIT_VDO_PLAY_NOR_MODE:
			case AIT_VDO_PLAY_FULL_MODE:
			case AIT_JPG_PLAY_NOR_MODE:
			case AIT_JPG_PLAY_FULL_MODE:
			case AIT_VDO_PLAY_YUVBUF_NOR_MODE:
			case AIT_VDO_PLAY_YUVBUF_FULL_MODE:
			{
				A800Data.SensorDes.MaxWidth = 640;
				A800Data.SensorDes.MaxHeight = 480;
				A800Data.SensorDes.PrevWidth = 640;
				A800Data.SensorDes.PrevHeight = 480;
			}
			break;
		}	
		AIT_Message_P2("%d,%d",A800Data.LCDCount,A800Data.ActiveLCDNo);

		Select->Rotate = gsPreviewAttrib[PreviewModeIndex].PanelRotate; 
		switch(gsPreviewAttrib[PreviewModeIndex].PanelRotate)
		{
			default:
			case A8_RT_NORMAL:
			case A8_RT_RIGHT_180:			
				Select->DispWidth = gsPreviewAttrib[PreviewModeIndex].PreviewRange.w;
				Select->DispHeight = gsPreviewAttrib[PreviewModeIndex].PreviewRange.h;

				break;
			case A8_RT_RIGHT_270:
			case A8_RT_RIGHT_90:
				Select->DispWidth = gsPreviewAttrib[PreviewModeIndex].PreviewRange.h;
				Select->DispHeight = gsPreviewAttrib[PreviewModeIndex].PreviewRange.w;
				
				break;
		}
	
		Select->BufOriginX = 0;
		Select->BufOriginY = 0;
		Select->DispLcdX = gsPreviewAttrib[PreviewModeIndex].PreviewRange.x;
		Select->DispLcdY = gsPreviewAttrib[PreviewModeIndex].PreviewRange.y;

		AIT_Message_P2("Display W= %d,H = %d",Select->DispWidth,Select->DispHeight);



		Select->BufWidth = gsPreviewAttrib[PreviewModeIndex].PreviewRange.w;
		Select->BufHeight = gsPreviewAttrib[PreviewModeIndex].PreviewRange.h;		
#if defined(__MMI_MAINLCD_240X320__) || defined(__MMI_MAINLCD_320X240__)
	
		switch(mode)
		{
			case AIT_VDO_PREV_MODE:
				AIT_Message_P0(" Refresh Mode :CAM_LCD_REFRESH_SCALE\r\n");		
				Select->BufFormat = A8_YUV_422;
				Select->LcdRefreshMode = CAM_LCD_REFRESH_SCALE;
				Select->BufWidth = 176;
				Select->BufHeight = 144;
				OSDBufFormat = A8_DEPTH_8B;	
				break;
				
			case AIT_VDO_PLAY_NOR_MODE:
				AIT_Message_P0(" Refresh Mode :CAM_LCD_REFRESH_SCALE\r\n");
				
				Select->BufFormat = A8_DEPTH_16B;
				Select->LcdRefreshMode = CAM_LCD_REFRESH_SCALE;
				Select->BufWidth = 176;
				Select->BufHeight = 144;
				OSDBufFormat = A8_DEPTH_8B;			
				
				break;


			case AIT_VDO_PLAY_YUVBUF_NOR_MODE:
				Select->BufFormat = A8_YUV_422;
				Select->LcdRefreshMode = CAM_LCD_REFRESH_NORMAL;
				OSDBufFormat = A8_DEPTH_16B;
				break;
				
			case AIT_VDO_PLAY_YUVBUF_FULL_MODE:
				Select->BufFormat = A8_YUV_422;
				Select->LcdRefreshMode = CAM_LCD_REFRESH_NORMAL;
				OSDBufFormat = A8_DEPTH_16B;
				break;
				
			case AIT_JPG_PLAY_NOR_MODE:
			case AIT_JPG_PLAY_FULL_MODE:
				AIT_Message_P0(" Refresh Mode :CAM_LCD_REFRESH_NORMAL\r\n");
				Select->BufFormat = A8_YUV_422;
				Select->LcdRefreshMode = CAM_LCD_REFRESH_BYPASS;
				OSDBufFormat = A8_DEPTH_16B;	
				break;

			case AIT_ATV_PREV_NOR_MODE:
			case AIT_ATV_PREV_FULL_MODE:
				AIT_Message_P0(" Refresh Mode :CAM_LCD_REFRESH_BYPASS\r\n");
#if 0
				Select->BufFormat = A8_YUV_422;
				Select->LcdRefreshMode = CAM_LCD_REFRESH_SCALE;

				Select->BufWidth = 176;
				Select->BufHeight = 144;

				OSDBufFormat = A8_DEPTH_8B;	
#else
				Select->BufFormat = A8_YUV_422;
				Select->LcdRefreshMode = CAM_LCD_REFRESH_BYPASS;
				OSDBufFormat = A8_DEPTH_8B;	
#endif
				break;;

			case AIT_ATV_REC_NOR_MODE:
			case AIT_ATV_REC_FULL_MODE:
				AIT_Message_P0(" Refresh Mode :CAM_LCD_REFRESH_BYPASS\r\n");
				Select->BufFormat = A8_DEPTH_16B;
				Select->LcdRefreshMode = CAM_LCD_REFRESH_SCALE;

				Select->BufWidth = 176;
				Select->BufHeight = 144;

				OSDBufFormat = A8_DEPTH_8B;	
				break;

		
			default:
			case AIT_CAM_PREV_NOR_MODE:
			case AIT_CAM_PREV_FULL_MODE:

#if 0			
				AIT_Message_P0(" Refresh Mode :CAM_LCD_REFRESH_NORMAL\r\n");
				Select->BufFormat = A8_DEPTH_16B;
				Select->LcdRefreshMode = CAM_LCD_REFRESH_NORMAL;					
#else	//Sometimes it cause preview fail.
				AIT_Message_P0(" Refresh Mode :CAM_LCD_REFRESH_BYPASS\r\n");
				
				Select->BufFormat = A8_YUV_422;
				Select->LcdRefreshMode = CAM_LCD_REFRESH_BYPASS;	
#endif
				OSDBufFormat = A8_DEPTH_16B;
								
				break;
		}
#endif
#if defined(__MMI_MAINLCD_176X220__)
		switch(mode)
		{
			case AIT_VDO_PREV_MODE:
				AIT_Message_P0(" AIT_VDO_PREV_MODE"); 	
				Select->BufFormat = A8_YUV_422;
				Select->LcdRefreshMode = CAM_LCD_REFRESH_SCALE;
				Select->BufWidth = 176;
				Select->BufHeight = 144;
				OSDBufFormat = A8_DEPTH_16B; 
				break;

			case AIT_VDO_PLAY_FULL_MODE:	
				AIT_Message_P0(" AIT_VDO_PLAY_FULL_MODE");
				Select->BufFormat = A8_DEPTH_16B;
				Select->LcdRefreshMode = CAM_LCD_REFRESH_NORMAL;
				Select->BufWidth = 220;
				Select->BufHeight = 176;
				OSDBufFormat = A8_DEPTH_16B;

				break;
			case AIT_VDO_PLAY_NOR_MODE:
				AIT_Message_P0(" Refresh Mode :CAM_LCD_REFRESH_SCALE");
				Select->BufFormat = A8_DEPTH_16B;
				Select->LcdRefreshMode = CAM_LCD_REFRESH_NORMAL;
				Select->BufWidth = 176;
				Select->BufHeight = 144;
				OSDBufFormat = A8_DEPTH_16B;
				
				break;
			case AIT_JPG_PLAY_NOR_MODE:
			case AIT_JPG_PLAY_FULL_MODE:
				AIT_Message_P0(" Refresh Mode :CAM_LCD_REFRESH_NORMAL");
				Select->BufFormat = A8_YUV_422;
				Select->LcdRefreshMode = CAM_LCD_REFRESH_BYPASS;
				OSDBufFormat = A8_DEPTH_16B;	
				break;
			case AIT_VDO_PLAY_YUVBUF_NOR_MODE:
			case AIT_VDO_PLAY_YUVBUF_FULL_MODE:
				AIT_Message_P0(" Refresh Mode :AIT_VDO_PLAY_YUVBUF_FULL_MODE,CAM_LCD_REFRESH_NORMAL");		
				Select->BufFormat = A8_YUV_422;
				Select->LcdRefreshMode = CAM_LCD_REFRESH_SCALE;
				Select->BufWidth = 224;//176;
				Select->BufHeight = 176;//144;
				
				OSDBufFormat = A8_DEPTH_16B;
				break;		
			case AIT_ATV_PREV_NOR_MODE:
			case AIT_ATV_PREV_FULL_MODE:
				Select->BufFormat = A8_YUV_422;
				if(Select->Rotate==A8_RT_RIGHT_90)
					Select->LcdRefreshMode = CAM_LCD_REFRESH_NORMAL;
				else
					Select->LcdRefreshMode = CAM_LCD_REFRESH_BYPASS;
				
				OSDBufFormat = A8_DEPTH_16B;

				
				break;
				
			default:


			case AIT_CAM_PREV_NOR_MODE:
			case AIT_CAM_PREV_FULL_MODE:
				AIT_Message_P0(" Refresh Mode :CAM_LCD_REFRESH_BYPASS");
				
				Select->BufFormat = A8_DEPTH_16B;
				Select->LcdRefreshMode = CAM_LCD_REFRESH_NORMAL;//CAM_LCD_REFRESH_BYPASS;					
				OSDBufFormat = A8_DEPTH_16B;
								
				break;
		}

#endif
#if defined(__MMI_MAINLCD_220X176__)
		switch(mode)
		{
			case AIT_VDO_PREV_MODE:
				AIT_Message_P0(" Refresh Mode :CAM_LCD_REFRESH_SCALE"); 	
				Select->BufFormat = A8_YUV_422;
				Select->LcdRefreshMode = CAM_LCD_REFRESH_BYPASS;
				Select->BufWidth = 176;
				Select->BufHeight = 144;
				OSDBufFormat = A8_DEPTH_16B; 
				break;

			case AIT_VDO_PLAY_FULL_MODE:
				AIT_Message_P0(" Refresh Mode :CAM_LCD_REFRESH_SCALE");
				Select->BufFormat = A8_DEPTH_16B;
				Select->LcdRefreshMode = CAM_LCD_REFRESH_SCALE;
				Select->BufWidth = 176;
				Select->BufHeight = 144;
				OSDBufFormat = A8_DEPTH_16B;
				break;
			case AIT_VDO_PLAY_NOR_MODE:
				AIT_Message_P0(" Refresh Mode :CAM_LCD_REFRESH_SCALE");
				Select->BufFormat = A8_DEPTH_16B;
				Select->LcdRefreshMode = CAM_LCD_REFRESH_NORMAL;
				Select->BufWidth = 176;
				Select->BufHeight = 144;
				OSDBufFormat = A8_DEPTH_16B;
				
				break;
			case AIT_JPG_PLAY_NOR_MODE:
			case AIT_JPG_PLAY_FULL_MODE:
				AIT_Message_P0(" Refresh Mode :CAM_LCD_REFRESH_NORMAL");
				Select->BufFormat = A8_YUV_422;
				Select->LcdRefreshMode = CAM_LCD_REFRESH_BYPASS;
				OSDBufFormat = A8_DEPTH_16B;	
				break;
			case AIT_ATV_PREV_NOR_MODE:
			case AIT_ATV_PREV_FULL_MODE:
				Select->BufFormat = A8_YUV_422;
				Select->LcdRefreshMode = CAM_LCD_REFRESH_BYPASS;
				OSDBufFormat = A8_DEPTH_16B;

				
				break;
				
			default:


			case AIT_CAM_PREV_NOR_MODE:
			case AIT_CAM_PREV_FULL_MODE:
				AIT_Message_P0(" Refresh Mode :CAM_LCD_REFRESH_BYPASS");
				
				Select->BufFormat = A8_YUV_422;
				Select->LcdRefreshMode = CAM_LCD_REFRESH_BYPASS;					
				OSDBufFormat = A8_DEPTH_16B;
								
				break;
		}

#endif
#if defined(__MMI_MAINLCD_128X160__)
		switch(mode)
		{
			case AIT_VDO_PREV_MODE:
				AIT_Message_P0(" Refresh Mode :CAM_LCD_REFRESH_SCALE"); 	
				Select->BufFormat = A8_YUV_422;
				Select->LcdRefreshMode = CAM_LCD_REFRESH_SCALE;//CAM_LCD_REFRESH_BYPASS;
				Select->BufWidth = 176;
				Select->BufHeight = 144;
					OSDBufFormat = A8_DEPTH_8B;	
				break;

			case AIT_VDO_PLAY_FULL_MODE:
				AIT_Message_P0(" AIT_VDO_PLAY_FULL_MODE");
				Select->BufFormat = A8_DEPTH_16B;
				Select->LcdRefreshMode = CAM_LCD_REFRESH_NORMAL;
				Select->BufWidth = 160;
				Select->BufHeight = 128;
				OSDBufFormat = A8_DEPTH_16B;
				break;
			case AIT_VDO_PLAY_NOR_MODE:
				AIT_Message_P0(" AIT_VDO_PLAY_NOR_MODE");
				
				Select->BufFormat = A8_DEPTH_16B;
				Select->LcdRefreshMode = CAM_LCD_REFRESH_NORMAL;
				Select->BufWidth = 128;
				Select->BufHeight = 96;
				OSDBufFormat = A8_DEPTH_16B;
				
				break;
			case AIT_JPG_PLAY_NOR_MODE:
			case AIT_JPG_PLAY_FULL_MODE:
				AIT_Message_P0(" Refresh Mode :CAM_LCD_REFRESH_NORMAL");
				Select->BufFormat = A8_YUV_422;
				Select->LcdRefreshMode = CAM_LCD_REFRESH_BYPASS;
				OSDBufFormat = A8_DEPTH_16B;	
				break;
			case AIT_ATV_PREV_NOR_MODE:
			case AIT_ATV_PREV_FULL_MODE:
				Select->BufFormat = A8_YUV_422;
				if(Select->Rotate==A8_RT_RIGHT_90)
					Select->LcdRefreshMode = CAM_LCD_REFRESH_NORMAL;
				else
					Select->LcdRefreshMode = CAM_LCD_REFRESH_BYPASS;

				OSDBufFormat = A8_DEPTH_16B;

				
				break;
				
			default:


			case AIT_CAM_PREV_NOR_MODE:
			case AIT_CAM_PREV_FULL_MODE:
				AIT_Message_P0(" Refresh Mode :CAM_LCD_REFRESH_BYPASS");
				
				Select->BufFormat = A8_YUV_422;
				Select->LcdRefreshMode = CAM_LCD_REFRESH_BYPASS;					
				OSDBufFormat = A8_DEPTH_16B;
								
				break;
		}

#endif

	{

		Select->DownScale = 80;

		temUpScalew = Select->BufWidth*Select->DownScale /A800Data.SensorDes.MaxWidth;
		temUpScaleh= Select->BufHeight*Select->DownScale /A800Data.SensorDes.MaxHeight;
		
		if(temUpScalew>=temUpScaleh)
			Select->UpScale = temUpScalew;
		else
			Select->UpScale = temUpScaleh;

		Select->UpScale += 1;
		
		AIT_Message_P2(" M = %d,N = %d\r\n",Select->DownScale,Select->UpScale);
	}
	switch(gA8MainLCDWidth)
		
	{
		default:
			break;
		case 128:
		case 160:
		case 176:
		case 220:			
				OSDBufFormat = A8_DEPTH_16B;
				break;
	}

	if(OSDBufFormat == A8_DEPTH_8B)
		bufNumColorBytes = 1;
	else
		bufNumColorBytes = 2;

	Select->BufAddr = 0;	
			
	switch(Select->LcdRefreshMode )	
	{
		default:
		case CAM_LCD_REFRESH_BYPASS:
			Select->MainOSD.OSDBufAddr = 0x0000;
			break;

		case CAM_LCD_REFRESH_SCALE:

			if(mode==AIT_VDO_PLAY_YUVBUF_NOR_MODE||mode ==AIT_VDO_PLAY_YUVBUF_FULL_MODE)
			{
	
				Select->BufAddr = 0;	

				Select->MainOSD.OSDBufAddr = Select->BufAddr+224*Select->BufHeight*3/2+224*8;
			}
			else
			{

						
				Select->BufAddr = Select->DispWidth*8;	

				Select->MainOSD.OSDBufAddr = Select->BufAddr
											+ Select->BufWidth*Select->BufHeight*2;
			}
			break;

		case CAM_LCD_REFRESH_NORMAL:
			Select->MainOSD.OSDBufAddr = Select->BufAddr
										+ Select->BufWidth*Select->BufHeight*2;
			break;
	}

	Select->ScaleRatio = 1;

	Select->ZoomStep = 1;

	//OSD Range Setting
							
	AIT_Message_P1("Main OSD Buf Addr =%x\r\n",Select->MainOSD.OSDBufAddr);
	if(((gsPreviewAttrib[PreviewModeIndex].OSD_TOP.x+gsPreviewAttrib[PreviewModeIndex].OSD_TOP.w)>gA8MainLCDWidth)||
		((gsPreviewAttrib[PreviewModeIndex].OSD_TOP.y+gsPreviewAttrib[PreviewModeIndex].OSD_TOP.h)>gA8MainLCDHeight))
	{
		AIT_Message_Error("!!!Main OSD Range Error!!\r\n",0);
	}

	
   	Select->MainOSD.OSDBufWidth = gsPreviewAttrib[PreviewModeIndex].OSD_TOP.w;
	Select->MainOSD.OSDBufHeight = gsPreviewAttrib[PreviewModeIndex].OSD_TOP.h;
	Select->MainOSD.OSDDispLcdX = gsPreviewAttrib[PreviewModeIndex].OSD_TOP.x;
	Select->MainOSD.OSDDispLcdY = gsPreviewAttrib[PreviewModeIndex].OSD_TOP.y;

	Select->MainOSD.OSDBufFormat = OSDBufFormat;
	Select->MainOSD.OSDEnable = Select->MainOSD.OSDBufWidth&&Select->MainOSD.OSDBufHeight ;

	Select->OverlayOSD.OSDBufAddr = Select->MainOSD.OSDBufAddr+
       								Select->MainOSD.OSDBufWidth*
       								Select->MainOSD.OSDBufHeight*bufNumColorBytes;
	AIT_Message_P1(" Overlay Buf Addr =%x\r\n",Select->OverlayOSD.OSDBufAddr );
	if(((gsPreviewAttrib[PreviewModeIndex].OSD_CENTER.x+gsPreviewAttrib[PreviewModeIndex].OSD_CENTER.w)>gA8MainLCDWidth)||
		((gsPreviewAttrib[PreviewModeIndex].OSD_CENTER.y+gsPreviewAttrib[PreviewModeIndex].OSD_CENTER.h)>gA8MainLCDHeight))
	{
		AIT_Message_Error("!!!Overlay OSD Range Error!!\r\n",0);
	}

	
	Select->OverlayOSD.OSDBufWidth =gsPreviewAttrib[PreviewModeIndex].OSD_CENTER.w;
	Select->OverlayOSD.OSDBufHeight =gsPreviewAttrib[PreviewModeIndex].OSD_CENTER.h;
	Select->OverlayOSD.OSDDispLcdX = gsPreviewAttrib[PreviewModeIndex].OSD_CENTER.x;
	Select->OverlayOSD.OSDDispLcdY = gsPreviewAttrib[PreviewModeIndex].OSD_CENTER.y;
	
	Select->OverlayOSD.OSDBufFormat = OSDBufFormat;
	Select->OverlayOSD.OSDEnable = Select->OverlayOSD.OSDBufWidth&&Select->OverlayOSD.OSDBufHeight;



	Select->IconOSD.OSDBufAddr = Select->OverlayOSD.OSDBufAddr +
									Select->OverlayOSD.OSDBufWidth*
									Select->OverlayOSD.OSDBufHeight*bufNumColorBytes;
	AIT_Message_P1(" Icon  Buf Addr =%x\r\n",Select->IconOSD.OSDBufAddr );
	if(((gsPreviewAttrib[PreviewModeIndex].OSD_BOTTOM.x+gsPreviewAttrib[PreviewModeIndex].OSD_BOTTOM.w)>gA8MainLCDWidth)||
		((gsPreviewAttrib[PreviewModeIndex].OSD_BOTTOM.y+gsPreviewAttrib[PreviewModeIndex].OSD_BOTTOM.h)>gA8MainLCDHeight))
	{
		AIT_Message_Error("!!!Icon OSD Range Error!!\r\n",0);
	}



	Select->IconOSD.OSDBufWidth = gsPreviewAttrib[PreviewModeIndex].OSD_BOTTOM.w;
	Select->IconOSD.OSDBufHeight = gsPreviewAttrib[PreviewModeIndex].OSD_BOTTOM.h;
	Select->IconOSD.OSDDispLcdX = gsPreviewAttrib[PreviewModeIndex].OSD_BOTTOM.x;
	Select->IconOSD.OSDDispLcdY = gsPreviewAttrib[PreviewModeIndex].OSD_BOTTOM.y;
	
	Select->IconOSD.OSDBufFormat = OSDBufFormat;

	Select->IconOSD.OSDEnable = Select->IconOSD.OSDBufWidth&&Select->IconOSD.OSDBufHeight;


	AIT_Message_P1("End of OSD Addr =%x\r\n",Select->IconOSD.OSDBufAddr +Select->IconOSD.OSDBufWidth*Select->IconOSD.OSDBufHeight*2);

	Select->IconCount = 0;
	Select->IconPtr = NULL;

	if(Select->IconOSD.OSDBufAddr>96*1024)
	{
		AIT_Message_Error("!!!Buffer usage exceed 96K\r\n",0);
	}
	return Select;
}

/**
 @fn		u_char A800_JPEGQuality(u_char level)
 @brief	Setup the JPEG picture Quality.
 @param	level - Quality level : spuerfine, fine, normal, economy.
 @return	u_char
 @note	.
 @bug	N/A.
*/
u_char A800_JPEGQuality(u_char level)
{
EntryAITAPI;
	if (level < 8)
	{
		A800_SetQTable(level);			
		gAITJpegQuality = level;
		AIT_Message_P1("A800_JPEGQuality = 0x%x\r\n", level);
		return A8L_SetJpegQuality(level);
	}
	

ExitAITAPI;

   	return A8_NO_ERROR;
}

/**
 @fn		u_short A800_GetFwVersion(void)
 @brief	This function is for get AIT Firmware version.
 @param	A8FwVer.
 @return	u_short - 0:success, 1:Fail
 @note	.
 @bug	N/A.
*/

u_short A800_GetFwVersion(u_short *A8FwVer)
{
  u_int timeout=0; 
  while(((GetA8DSCStatus() & A8_DSC_READY_FOR_CMD) == 0) && timeout < A8_CMD_READY_TIME_OUT>>4)
  {
    timeout++;
  }
  if ( timeout >= A8_CMD_READY_TIME_OUT>>4)
  {
    return A8_TIMEOUT_ERROR;
  }
  SendA8Cmd( A8_HOST_CMD_GET_FW_VER ); 
  while(((GetA8DSCStatus() & A8_DSC_READY_FOR_CMD) == 0) && timeout < A8_CMD_READY_TIME_OUT>>4)
  {
    timeout++;
  }
  if ( timeout >= A8_CMD_READY_TIME_OUT>>4)
  {
    return A8_TIMEOUT_ERROR;
  }
  *A8FwVer = GetA8RegW(0x654C);
 
  return A8_NO_ERROR;
}

/**
 @fn		u_char A800_PowerSavingOn(void)
 @brief	Setup AIT chip ENTER PowerSaving mode.
 @return	u_char
 @note	.
 @bug	N/A.
*/
u_char A800_PowerSavingOn(void)
{
	u_int i;
   SetA8RegB(0x69C4, GetA8RegB(0x69c4)&0xFD);      // PNINT to High
   SetA8RegB(0x69C7, GetA8RegB(0x69c7)&0xFD);      // PHLCD_BY to High


//	1. pull sd io to floation
	for(i=0; i<6; i++)
		SetA8RegB(0x69D9+i,(GetA8RegB(0x69D9+i)&(~0x6)));
//	2. off sd power
	//control GPIO 6, currently do not control SD power, this pin may be used in other app.
	//SetA8RegB(0x69DF,0x12);
	//SetA8RegB(0x6600,GetA8RegB(0x6600)|0x40);
	//SetA8RegB(0x6610,GetA8RegB(0x6610)&(~0x40));
//	3. config SD io as GPIO
	SetA8RegB(0x692C,GetA8RegB(0x692C)&(~0xC));	
//	4. set GPIO to low
	SetA8RegB(0x6610,GetA8RegB(0x6610)&(~0x3F));
//	5. set GPIO to output
	SetA8RegB(0x6600,GetA8RegB(0x6600)|(0x3F));
	sys_IF_ait_delay1ms(10);	//for discharge sd 
//	6. set GPIO to input
	SetA8RegB(0x6600,GetA8RegB(0x6600)&(~0x3F));
#if 0	//added by ming @20090421
//	7. pull sd io to up
	for(i=0; i<6; i++)
		SetA8RegB(0x69D9+i,(GetA8RegB(0x69D9+i)|0x4));
#endif	

//	1. pull SDA,SCK to down
	SetA8RegB(0x69D2,0x12);
	SetA8RegB(0x69D3,0x12);
//	2. pull SEN to floating //down
	SetA8RegB(0x69D1,0x10);
//	3. pull SRST to floating //high
	SetA8RegB(0x69CC,0x10);
//	4. set to input
	SetA8RegB(0x6050,0x0);
//	5. off sensor power
	//control GPIO 7, currently do not control SD power, this pin may be used in other app.
	//SetA8RegB(0x6600, GetA8RegB(0x6600)|0x80);
	//SetA8RegB(0x6610, GetA8RegB(0x6610)&(~0x80));
   //   AIT_Message_P2("XX, A800_PowerSavingOn, 0x6026:0x%x, 0x6050:0x%x \n", GetA8RegW(0x6026), GetA8RegW(0x6050));
   //   AIT_Message_P2("XX, A800_PowerSavingOn, 0x69C4:0x%x, 0x69C7:0x%x \n", GetA8RegB(0x69c4), GetA8RegB(0x69c7));
#if 0 //Reserve: if sensor power is off, sensor I/F should pull low
//sensor data pin pull down
 SetA8RegB(0x69a0, (GetA8RegB(0x69a0)&(~0x04)) |(0x02));
 //Vsync pull down
 SetA8RegB(0x69CE, (GetA8RegB(0x69CE)&(~0x04)) |(0x02));
 //Hsync pull down
 SetA8RegB(0x69CD, (GetA8RegB(0x69CD)&(~0x04)) |(0x02));
#endif
   return A8_NO_ERROR;
}


/**
 @fn		u_char 	A800_PowerSavingOff(void)
 @brief	Setup AIT chip EXIT PowerSaving mode.
 @return	u_char
 @note	.
 @bug	N/A.
*/
u_char 	A800_PowerSavingOff(void)
{
	return A8_NO_ERROR;
}

/**
 @fn		u_char A800_LCDInit()
 @brief	Init LCD panel and AIT800 LCD control
 @return	u_char
 @note	.
 @bug	N/A.
*/
u_char A800_LCDInit()
{
EntryAITAPI;
	A800_HostInitLCDCommon();
	//A800_HostInitLCD();     // Replaced by the above function
ExitAITAPI;
	return A8_NO_ERROR;
}

/**
 @fn		u_short A800_IsResetStatus()
 @brief	If AIT Chip in Reset Status.
 @return	u_short
 @note	.
 @bug	N/A.
*/
u_short A800_IsResetStatus(void)
{
	if ( (GetA8RegW(0x6900)>>8) != 0x40 || (GetA8RegW(0x6902)>>8) == 0xFF )
	{
            return A8_SYSTEM_ERROR;
	}
	else
	{
            return A8_NO_ERROR;
	}
}


//**************************************************************************
//**************************************************************************

/**
 @fn		u_char 	A800_Camera_Bypass_Mode(void)
 @brief	Set AIT chip into SW Bypass mode, prepare for HW Bypass mode.
 @return	u_char
 @note	.
 @bug	N/A.
*/
#if 1	//added by ming @20090413 for protect clock en/dis
bool	gbAitUsbEn = FALSE;
bool	gbAitSdEn = FALSE;
bool	gbAitCamEn = FALSE;
#endif

u_char A800_SetPllFreq(u_char ait_mode, u_char on)
{

	u_short	retVal = A8_NO_ERROR;
	
	if(A8_ON == on){
		if((FALSE == gbAitCamEn) && (FALSE==gbAitSdEn) && (FALSE==gbAitUsbEn)) {
			switch(ait_mode)
			{
				case	A8_CAM:
				case	A8_SD:
				case	A8_USB:	
#if defined(EXTCLK_26M)					
					retVal = A8L_SetPllFreq(2);
#elif defined(EXTCLK_19M2)					
					retVal = A8L_SetPllFreq(3);
#endif					
					AIT_ext_Set_EMIMode(EMIMODE_ACTIVE_PLL);
					break;
			}
		}
		switch(ait_mode)
		{
			case	A8_CAM:
				gbAitCamEn = TRUE;
				break;
			case	A8_SD:
				gbAitSdEn = TRUE;
				break;
			case	A8_USB:
				gbAitUsbEn = TRUE;
				break;			
		}
	}else{
		switch(ait_mode)
		{
			case	A8_CAM:
				gbAitCamEn = FALSE;
				break;
			case	A8_SD:
				gbAitSdEn = FALSE;
				break;
			case	A8_USB:
				gbAitUsbEn = FALSE;
				break;	
			case	A8_ALL:
				gbAitCamEn = FALSE;
				gbAitSdEn = FALSE;	
				gbAitUsbEn = FALSE;				
				break;
		}
		if((FALSE == gbAitCamEn) && (FALSE==gbAitSdEn) && (FALSE==gbAitUsbEn)) {
			AIT_ext_Set_EMIMode(EMIMODE_ACTIVE_NO_PLL);
			retVal = A8L_SetPllFreq(0);
		}
	}
	return retVal;
}

/**
 @fn		u_char 	A800_Camera_Bypass_Mode(void)
 @brief	Set AIT chip into SW Bypass mode, prepare for HW Bypass mode.
 @return	u_char
 @note	.
 @bug	N/A.
*/
u_char 	A800_Camera_Bypass_Mode(void)
{
	s_short retVal;

	A8L_SetWindowActive(A8_PIP_WINDOW, 0);
	retVal = A8L_CheckReadyForA8ByPass();
#if 0	//removed by ming @20091006 need to check
	SetA8RegB(0x6960, (GetA8RegB(0x6960) & 0x8F)); // 20080614, // 2T SRAM buffer memory disable!
#endif
	if (retVal )
		return A8_SYSTEM_ERROR;
	else
		return A8_NO_ERROR;
}

/**
 @fn		u_char 	A800_Camera_Active_Mode(void) 
 @brief	After AIT HW Active mode, Set AIT chip into SW active mode(with PLL enable).
 @return	u_char
 @note	Make sure AIT Bypass pin is at low status.
 @bug	N/A.
*/
u_char 	A800_Camera_Active_Mode(void)
{
	s_short retVal;

	SetA8RegB(0x6960, (GetA8RegB(0x6960) | 0x70)); // 20080614, buffer memory enable!

	retVal = A8L_CheckReadyForA8Resume();
	if ( retVal  )
	{
		return A8_TIMEOUT_ERROR;
	} 

	return A8_NO_ERROR;
}

/**
 @fn		u_char 	A800_OpenPreviewWindow(void)
 @brief	Open Preview Window (PIP window is for Preview).
 @return	u_char
 @note	.
 @bug	N/A.
*/
u_char 	A800_OpenPreviewWindow(void)
{
EntryAITAPI;
	A8L_SetWindowActive(A8_PIP_WINDOW, 1);
ExitAITAPI;
	return A8_NO_ERROR;
}


/**
 @fn		u_char 	A800_ClosePreviewWindow(void)
 @brief	Close Preview Window (PIP window is for Preview).
 @return	u_char
 @note	.
 @bug	N/A.
*/
u_char 	A800_ClosePreviewWindow(void)
{
EntryAITAPI;
	A8L_SetWindowActive(A8_PIP_WINDOW, 0);
ExitAITAPI;
	return A8_NO_ERROR;
}


/**
 @fn		u_char 	A800_OpenMainWindow(void)
 @brief	Open Main Window.
 @return	u_char
 @note	.
 @bug	N/A.
*/
u_char 	A800_OpenMainWindow(void)
{
EntryAITAPI;
	A8L_SetWindowActive(A8_MAIN_WINDOW, 1);
ExitAITAPI;
	return A8_NO_ERROR;
}


/**
 @fn		u_char 	A800_CloseMainWindow(void)
 @brief	Open Main Window.
 @return	u_char
 @note	.
 @bug	N/A.
*/
u_char 	A800_CloseMainWindow(void)
{
EntryAITAPI;
	A8L_SetWindowActive(A8_MAIN_WINDOW, 0);
ExitAITAPI;
	return A8_NO_ERROR;
}


/**
 @fn		u_short 	A800_SetCaptureResolution(u_short Reso)
 @brief	Setting capture resolution information for preview (zoom rate setting).
 @return	void
 @note	This function is used for getting the information of capture resolution at preview time.
 @bug	N/A.
*/
u_short A800_SetCaptureResolution(u_short Reso)
{
    gA8CurrentCaptureResolution = Reso;

	return A8_NO_ERROR;
}

/**
 @fn		u_short 	A800_GetCaptureResolution()
 @brief	Getting capture resolution information for preview (zoom rate setting).
 @return	void
 @note	This function is used for getting the information of capture resolution at preview time.
 @note   Notice that this function should be called after A800_GetCaptureResolution() called.
 @bug	N/A.
*/
u_short A800_GetCaptureResolution()
{
//	AIT_Message_P1("### Capture Resolution=%d\n", gA8CurrentCaptureResolution);
    return gA8CurrentCaptureResolution;
}


/**
 @fn		u_short A800_SetSECCameraMode()
 @brief	Select Preview mode to SEC Video Mode.
 @return	u_short.
 @note	.
 @bug	N/A.
*/
u_short A800_SetSECCameraMode(u_short	VideoMode)
{
//	s_int timeout = 0;
	
	SendA8Cmd( A8_HOST_CMD_SELECT_SEC_CAM_MODE + (VideoMode << 8) );
	
	if(A8L_CheckReadyForA8Command())
	{
		return A8_TIMEOUT_ERROR;
	}
	
	return A8_NO_ERROR;
}

/**
 @fn		u_short A800_SetSECViewResolution()
 @brief	Select Preview Resolution to SEC Video Mode.
 @return	u_short.
 @note	.
 @bug	N/A.
*/
u_short A800_SetSECViewResolution(u_short	ViewResol)
{
	//s_int timeout = 0;
	
	SendA8Cmd( A8_HOST_CMD_SELECT_SEC_VIEW_MODE + (ViewResol << 8) );
	
	if(A8L_CheckReadyForA8Command())
	{
		return A8_TIMEOUT_ERROR;
	}
	
	return A8_NO_ERROR;
}


//=============================================================//
u_short	A800_SetPreviewMode(u_short mode)
{
	extern void A800_InitLCDLookUpTable(void);
	u_short RetVal;
	
	A800Data.Select = A800_ChangeCameraSetting(mode);

	if(!A800Data.Select)
	{
		AIT_Message_P2("A800_SetPreviewMode Error!",__FILE__,__LINE__);
		return A8_SYSTEM_ERROR;
	}
	
	A8L_SetWindowActive(A8_MAIN_WINDOW,0);
	A8L_SetWindowActive(A8_PIP_WINDOW,0);
	A8L_SetWindowActive(A8_OVERLAY_WINDOW, 0);
	A8L_SetWindowActive(A8_GAME_WINDOW, 0); 	
	A8L_ConfigPreviewZoomParam( A800Data.Select->ScaleRatio, 1 );
	
	#if 1
		AIT_Message_P3("scaleRatio =%d, sensor W= %d, sensor H=%d\n",A800Data.Select->ScaleRatio,A800Data.SensorDes.PrevWidth,A800Data.SensorDes.PrevHeight);


	AIT_Message_P5("Preview W=%d, H=%d, N=%d, M=%d, Zoom=%d\n",A800Data.Select->BufWidth, A800Data.Select->BufHeight,
	                A800Data.Select->UpScale, A800Data.Select->DownScale, gA8PrevZoomFactor);
	#endif

	A8L_SetCdspPreviewResolution(A800Data.SensorDes.PrevWidth,
                                              A800Data.SensorDes.PrevHeight,
                                              A800Data.Select->BufWidth,
                                              A800Data.Select->BufHeight,
                                              A800Data.Select->UpScale,
                                              A800Data.Select->DownScale,
                                              gA8PrevZoomFactor);

	//A8L_SetZoomRate((gsCurViewMode<<6)+gA8PrevZoomFactor);
	A8L_SetZoomRate(gA8PrevZoomFactor); // 0425 for SQCIF multishot zoom issue

	A8L_SetBufferAttribute(0, A800Data.Select->BufWidth, A800Data.Select->BufHeight, 
							 A800Data.Select->BufWidth<<1, A800Data.Select->BufFormat, A800Data.Select->BufAddr);
	A8L_BindBufferToWindow(0, A8_PIP_WINDOW);

	A8L_SetWindowToDisplay( A8_PIP_WINDOW, A800Data.Select->BufOriginX, A800Data.Select->BufOriginY,
							A800Data.Select->DispWidth, A800Data.Select->DispHeight,
							A800Data.Select->DispLcdX, A800Data.Select->DispLcdY, 
							A800Data.Select->Rotate );

	if ( A800Data.Select->BufFormat == A8_YUV_422 )
		A8L_SetPreviewPath(1);
	else
		A8L_SetPreviewPath(0);
#if 1	//added by ming @20090918 for 8bit init 
	A800_InitLCDLookUpTable();
#endif	
	if(A800Data.Select->MainOSD.OSDEnable)
	{
		A8L_SetBufferAttribute(9, A800Data.Select->MainOSD.OSDBufWidth, A800Data.Select->MainOSD.OSDBufHeight,
								A800Data.Select->MainOSD.OSDBufWidth<<1, A800Data.Select->MainOSD.OSDBufFormat, A800Data.Select->MainOSD.OSDBufAddr );
		A8L_BindBufferToWindow(9, A8_MAIN_WINDOW);
		A8L_SetWindowToDisplay( A8_MAIN_WINDOW, 0, 0, A800Data.Select->MainOSD.OSDBufWidth, A800Data.Select->MainOSD.OSDBufHeight, 
							A800Data.Select->MainOSD.OSDDispLcdX, A800Data.Select->MainOSD.OSDDispLcdY, A8_RT_NORMAL );
		AIT_Message_P4("MainOSD X=%d, Y=%d, W=%d, H=%d",A800Data.Select->MainOSD.OSDDispLcdX,A800Data.Select->MainOSD.OSDDispLcdY,
		A800Data.Select->MainOSD.OSDBufWidth, A800Data.Select->MainOSD.OSDBufHeight);
	}

	if (A800Data.Select->OverlayOSD.OSDEnable)
	{
		A8L_SetBufferAttribute(1, A800Data.Select->OverlayOSD.OSDBufWidth, A800Data.Select->OverlayOSD.OSDBufHeight, 
						 	A800Data.Select->OverlayOSD.OSDBufWidth<<1, A800Data.Select->OverlayOSD.OSDBufFormat, A800Data.Select->OverlayOSD.OSDBufAddr);
		A8L_BindBufferToWindow(1, A8_OVERLAY_WINDOW);
		A8L_SetWindowToDisplay( A8_OVERLAY_WINDOW, 0, 0, A800Data.Select->OverlayOSD.OSDBufWidth, A800Data.Select->OverlayOSD.OSDBufHeight,
							A800Data.Select->OverlayOSD.OSDDispLcdX, A800Data.Select->OverlayOSD.OSDDispLcdY, A8_RT_NORMAL );
		AIT_Message_P4("OverlayOSD X=%d, Y=%d, W=%d, H=%d",A800Data.Select->OverlayOSD.OSDDispLcdX,A800Data.Select->OverlayOSD.OSDDispLcdY,
		A800Data.Select->OverlayOSD.OSDBufWidth, A800Data.Select->OverlayOSD.OSDBufHeight);
	}

	if (A800Data.Select->IconOSD.OSDEnable)
	{
		A8L_SetIconAttribute(A800Data.Select->IconOSD.OSDBufAddr, A800Data.Select->IconOSD.OSDBufWidth, A800Data.Select->IconOSD.OSDBufHeight, 
							 	A800Data.Select->IconOSD.OSDDispLcdX, A800Data.Select->IconOSD.OSDDispLcdY, A800Data.Select->IconOSD.OSDBufFormat, 0x03);
        //A8L_SetIconTransparency(1, 0x001F);
		A8L_SetIconActive(1, 1);
		AIT_Message_P4("IconOSD X=%d, Y=%d, W=%d, H=%d",A800Data.Select->IconOSD.OSDDispLcdX,A800Data.Select->IconOSD.OSDDispLcdY,
		A800Data.Select->IconOSD.OSDBufWidth, A800Data.Select->IconOSD.OSDBufHeight);
	}

	#if 0
	else if (gsCurViewMode == AITCAM_PREV_128x96_MODE) // 0507 for 701A, frame update /* Stricker Mode, with 128x96 preview resolution */
	{
	    A8L_SetBufferAttribute(1, A800Data.Select->BufWidth, A800Data.Select->BufHeight, 
	    					 A800Data.Select->BufWidth<<1, A800Data.Select->BufFormat, A800Data.Select->BufAddr+0x6000);
	    A8L_BindBufferToWindow(1, A8_OVERLAY_WINDOW);

	    A8L_SetWindowToDisplay( A8_OVERLAY_WINDOW, A800Data.Select->BufOriginX, A800Data.Select->BufOriginY,
	    					A800Data.Select->DispWidth, A800Data.Select->DispHeight,
	    					A800Data.Select->DispLcdX, A800Data.Select->DispLcdY, 
	    					A800Data.Select->Rotate );

	    ClearA8Mem(A800Data.Select->BufAddr+0x6000, 0xF81F, 0x6000);
	}
	#endif
	
	switch(A800Data.Select->WinPriority)
	{
		case A8_WIN_PRIORITY_NORM:
			A8L_SetWindowPriority(A8_OVERLAY_WINDOW, A8_MAIN_WINDOW, A8_PIP_WINDOW, A8_GAME_WINDOW );
			break;
		case A8_WIN_PRIORITY_NORM_ALL:
			A8L_SetWindowPriority(A8_GAME_WINDOW, A8_OVERLAY_WINDOW, A8_MAIN_WINDOW, A8_PIP_WINDOW );
			break;
		case A8_WIN_PRIORITY_REVE:
			A8L_SetWindowPriority(A8_OVERLAY_WINDOW, A8_PIP_WINDOW, A8_MAIN_WINDOW, A8_GAME_WINDOW );
			break;
		case A8_WIN_PRIORITY_NORM_OVBACK:
			//A8L_SetWindowPriority(A8_MAIN_WINDOW, A8_PIP_WINDOW, A8_OVERLAY_WINDOW, A8_GAME_WINDOW );
		A8L_SetWindowPriority(A8_MAIN_WINDOW, A8_OVERLAY_WINDOW, A8_PIP_WINDOW, A8_GAME_WINDOW ); // 0507 for 701A
			break;
		case A8_WIN_PRIORITY_REVE_OVBACK:
			A8L_SetWindowPriority(A8_PIP_WINDOW, A8_MAIN_WINDOW, A8_OVERLAY_WINDOW, A8_GAME_WINDOW );
			break;
		default:
			break;
	}

	
	if (A800Data.Select->MainOSD.OSDEnable) {
	    A8L_SetWindowActive(A8_MAIN_WINDOW, 1);
	}
	if (A800Data.Select->OverlayOSD.OSDEnable) {
	    A8L_SetWindowActive(A8_OVERLAY_WINDOW, 1);
	}
	if (A800Data.Select->IconOSD.OSDEnable) {
	    A8L_SetWindowActive(A8_GAME_WINDOW, 1);
	}

	A8L_SetWindowActive(A8_PIP_WINDOW, A800Data.Select->WinState & A8_PIP_WIN_ON );

	
	RetVal = A8L_SetCamMode(A8_STATE_STILL_PREVIEW);
	if(RetVal)
		return A8_SYSTEM_ERROR;
	else
		return A8_NO_ERROR;
}



//=============================================================//
u_short A800_ResumeToPreviewMode(u_short mode)
{
//Vin@20091219: Select Panel Size in same lib.

	A800Data.Select = A800_ChangeCameraSetting(mode);
	if(!A800Data.Select )
		return A8_SYSTEM_ERROR;
	
	A8L_SetCdspPreviewResolution(640, 480, 
									A800Data.Select->BufWidth, 
									A800Data.Select->BufHeight,
									A800Data.Select->UpScale,
									A800Data.Select->DownScale,
									gA8PrevZoomFactor);
	A8L_StartLCDPreview();

	return A8_NO_ERROR;
}

//=============================================================//
u_short A800_SetPlaybackMode(u_short mode)
{
EntryAITAPI;
//Vin@20091219: Select Panel Size in same lib.

	A800Data.Select = A800_ChangeCameraSetting(mode);
	if(!A800Data.Select )
		return A8_SYSTEM_ERROR;
	
	A8L_SetWindowActive(A8_MAIN_WINDOW,0);
	A8L_SetWindowActive(A8_PIP_WINDOW,0);
	A8L_SetWindowActive(A8_OVERLAY_WINDOW, 0);
	A8L_SetWindowActive(A8_GAME_WINDOW, 0); 

	A8L_SetBufferAttribute(0, A800Data.Select->BufWidth, A800Data.Select->BufHeight,  A800Data.Select->BufWidth<<1,
												A800Data.Select->BufFormat, A800Data.Select->BufAddr);
	A8L_BindBufferToWindow(0, A8_PIP_WINDOW);
	A8L_SetWindowToDisplay(A8_PIP_WINDOW, A800Data.Select->BufOriginX, A800Data.Select->BufOriginY,
	        										A800Data.Select->BufWidth, A800Data.Select->BufHeight, A800Data.Select->DispLcdX, A800Data.Select->DispLcdY,
													A800Data.Select->Rotate);

	A8L_SetBufferAttribute(9, A800Data.Select->MainOSD.OSDBufWidth, A800Data.Select->MainOSD.OSDBufHeight,
												A800Data.Select->MainOSD.OSDBufWidth<<1, mOSD_DEPTH, A800Data.Select->MainOSD.OSDBufAddr);
	A8L_BindBufferToWindow(9, A8_MAIN_WINDOW);
	A8L_SetWindowToDisplay( A8_MAIN_WINDOW, 0, 0,  A800Data.Select->MainOSD.OSDBufWidth, A800Data.Select->MainOSD.OSDBufHeight, 
													A800Data.Select->MainOSD.OSDDispLcdX,  A800Data.Select->MainOSD.OSDDispLcdY,  A800Data.Select->Rotate );

	if (A800Data.Select->OverlayOSD.OSDEnable)
	{
		A8L_SetBufferAttribute(1, A800Data.Select->OverlayOSD.OSDBufWidth, A800Data.Select->OverlayOSD.OSDBufHeight, 
	    					 	A800Data.Select->OverlayOSD.OSDBufWidth<<1, A800Data.Select->OverlayOSD.OSDBufFormat, A800Data.Select->OverlayOSD.OSDBufAddr);
	    A8L_BindBufferToWindow(1, A8_OVERLAY_WINDOW);
	    A8L_SetWindowToDisplay( A8_OVERLAY_WINDOW, 0, 0, A800Data.Select->OverlayOSD.OSDBufWidth, A800Data.Select->OverlayOSD.OSDBufHeight,
	    						A800Data.Select->OverlayOSD.OSDDispLcdX, A800Data.Select->OverlayOSD.OSDDispLcdY, A8_RT_NORMAL );
	}

	if (A800Data.Select->IconOSD.OSDEnable)
	{
		A8L_SetIconAttribute(A800Data.Select->IconOSD.OSDBufAddr, A800Data.Select->IconOSD.OSDBufWidth, A800Data.Select->IconOSD.OSDBufHeight, 
	    					 	A800Data.Select->IconOSD.OSDDispLcdX, A800Data.Select->IconOSD.OSDDispLcdY, A800Data.Select->IconOSD.OSDBufFormat, 0x03);
		A8L_SetIconTransparency(1, 0x00);
		A8L_SetIconActive(1, 1);
	}

	switch(A800Data.Select->WinPriority)
	{
		case A8_WIN_PRIORITY_NORM:
			A8L_SetWindowPriority(A8_OVERLAY_WINDOW, A8_MAIN_WINDOW, A8_PIP_WINDOW, A8_GAME_WINDOW );
			break;
		case A8_WIN_PRIORITY_NORM_ALL:
			A8L_SetWindowPriority(A8_GAME_WINDOW, A8_OVERLAY_WINDOW, A8_MAIN_WINDOW, A8_PIP_WINDOW );
			break;
		case A8_WIN_PRIORITY_REVE:
			A8L_SetWindowPriority(A8_OVERLAY_WINDOW, A8_PIP_WINDOW, A8_MAIN_WINDOW, A8_GAME_WINDOW );
			break;
		case A8_WIN_PRIORITY_NORM_OVBACK:
			A8L_SetWindowPriority(A8_MAIN_WINDOW, A8_PIP_WINDOW, A8_OVERLAY_WINDOW, A8_GAME_WINDOW );
			break;
		case A8_WIN_PRIORITY_REVE_OVBACK:
			A8L_SetWindowPriority(A8_PIP_WINDOW, A8_MAIN_WINDOW, A8_OVERLAY_WINDOW, A8_GAME_WINDOW );
			break;
		default:
			break;
	}

	A8L_SetWindowActive(A8_PIP_WINDOW, A800Data.Select->WinState & A8_PIP_WIN_ON );
	A8L_SetWindowActive(A8_MAIN_WINDOW, A800Data.Select->WinState & A8_MAIN_WIN_ON);
	A8L_SetWindowActive(A8_OVERLAY_WINDOW, A800Data.Select->WinState & A8_OVERLAY_WIN_ON);
	A8L_SetWindowActive(A8_GAME_WINDOW, A800Data.Select->WinState & A8_ICON_WIN_ON);

ExitAITAPI;
	return A8_NO_ERROR;
}

/**
 @fn		u_char A800_SetPreviewParam(u_short mode, u_short Width, u_short Height, u_short OffsetX, u_short OffsetY, u_short ZoomRate, u_short SensorPrevRes)
 @brief	Set up Preview parameter for different preview mode.
 @param	mode - Normal/Full/~~ Screen Preview Mode.
 @param	Width
 @param	Height
 @param	OffsetX
 @param	OffsetY
 @param	ZoomRate
 @param	SensorPrevRes
 @return	u_short
 @note	.
 @bug	N/A.
*/
u_char A800_SetPreviewParam(u_short mode, u_short Width, u_short Height, u_short OffsetX, u_short OffsetY, u_short ZoomRate, u_short SensorPrevRes)
{
	A800Data.Select = A800_ChangeCameraSetting(mode);
	if(!A800Data.Select )
		return A8_SYSTEM_ERROR;

	A8L_SetWindowActive(A8_PIP_WINDOW,0);      // this code, just for avoid BB abnormal compiler fatal error!!!

	
    return A8_NO_ERROR;

}


/**
 @fn		u_short A800_SetPreviewZoom(u_short mode, u_short ZoomRate)
 @brief	Adjust the preview zoom rate for selected preview mode. ( the preview view angle will be changed )
 @param	mode - Normal/Full/~~ Screen Preview Mode.
 @param	ZoomRate -Digital Zoom Rate.
 @return	u_short
 @note	This function should only be called in preview mode..
 @bug	N/A.
*/
u_short A800_SetPreviewZoom(u_short mode, u_short ZoomRate)	 
{
	u_short Width=0, Height=0 , Temp , W_Multiple;
	u_short DownScale=0, UpScale;
	u_int timeout;

	gA8PrevZoomFactor = ZoomRate;
	A800Data.Select = A800_ChangeCameraSetting(mode);
	if(!A800Data.Select )
		return A8_SYSTEM_ERROR;


	Width = A800Data.Select->BufWidth;
	Height = A800Data.Select->BufWidth;
	Temp = A800Data.SensorDes.PrevWidth / Width;
	W_Multiple = A800Data.SensorDes.PrevHeight / Height;
	if(Temp > W_Multiple)
		Temp = W_Multiple;

	UpScale = 16 / Temp;
	DownScale =  (16 / UpScale) * UpScale;

	A800Data.Select->UpScale = UpScale;
	A800Data.Select->DownScale = DownScale;
	  
	if ( ZoomRate < 3 )
		SetA8RegB(0x617B, 0x5C );	
	else
		SetA8RegB(0x617B, 0x00 );

	SetA8RegB(0x5013, 0x01);
	SetA8RegW(0x6510, 0x00);
	if ( GetA8RegB(5008) & 0x01 )
	{
		timeout = 0;
		while(!(GetA8RegB(0x5013) & 0x01) && timeout < 0x100000)
		timeout++;
	}
	A8L_ConfigPreviewZoomParam( 1, 1 );
	A8L_SetCdspPreviewResolution( A800Data.SensorDes.PrevWidth/*640*/, A800Data.SensorDes.PrevWidth/*480*/, Width, Height, UpScale, DownScale, ZoomRate );

	SetA8RegW(0x6510, 0x04);
	SendA8Cmd((ZoomRate << 8) + A8_HOST_CMD_SET_ZOOM_RATE);

	return A8_NO_ERROR;
}


/**
 @fn		u_short 	A800_MainLCDPreviewActive()
 @brief	Preview in Main LCD.
 @return	u_short, Sucess = 0, TimeOut = 1
 @note	.
 @bug	N/A.
*/
u_short 	A800_MainLCDPreviewActive(void)
{
EntryAITAPI;
	return A8L_MainLCDPreviewActive();
ExitAITAPI;
}


/**
 @fn		u_short 	A800_SubLCDPreviewActive()
 @brief	Preview in Sub LCD.
 @return	u_short, Sucess = 0, TimeOut = 1
 @note	.
 @bug	N/A.
*/
u_short 	A800_SubLCDPreviewActive(void)
{
EntryAITAPI;
	return A8L_SubLCDPreviewActive();
ExitAITAPI;
}


/**
 @fn		u_char 	A800_StartPreview()
 @brief	Start Preview.
 @return	u_char, Sucess = 0, TimeOut = 1
 @note	.
 @bug	N/A.
*/
u_char 	A800_StartPreview(void)
{
EntryAITAPI;
	return A8L_StartLCDPreview();
ExitAITAPI;
	//return A8_NO_ERROR;
}


/**
 @fn		u_char 	A800_StopPreview()
 @brief	Stop Preview.
 @return	u_char, Sucess = 0, TimeOut = 1
 @note	.
 @bug	N/A.
*/
u_char 	A800_StopPreview(void)
{
EntryAITAPI;
	return A8L_StopLCDPreview();
ExitAITAPI;
	//return A8_NO_ERROR;
}

/**
 @fn		u_char 	A800_JPEGCapture(u_char resolution, u_int maxFileSize, u_short *pdata, u_int *filesize)
 @brief	JPEG Capture function.
 @param	resolution - 0:1.3M, 1:VGA, 2:QVGA, 3:QQVGA (4:CIF, 5:QCIF, 6:QQCIF)
 @param	pdata - the buffer pointer for storing the jpeg picture (in host ).
 @param	filesize - a pointer to a s_int variable to store the jpeg size.
 @return	u_char
 @note	.
 @bug	N/A.
*/
A8_ERROR_MSG 	A800_JPEGCapture(u_char resolution, u_int maxFileSize, u_short *pdata, u_int *filesize)
{
	u_char retVal;
	
	retVal = A8L_TakeJpegPicture(resolution,  gA8PrevZoomFactor, 0);
	if (retVal != A8_NO_ERROR)
	{
		return A8_CAM_ENCODE_ERROR;
	}

	if(resolution == 9)
		retVal = A8L_GetJpegPictureWithLimit_RAW((u_short *)pdata, maxFileSize, 9, filesize);
	else
		retVal = A8L_GetJpegPictureWithLimit((u_short *)pdata, maxFileSize, filesize);
	
	if (retVal != A8_NO_ERROR) // 0626 re-capture 
	{
		A800_JPEGQuality(gAITJpegQuality + 1);

		SetA8RegB(0x6511, 0x08);
		SetA8RegB(0x6020, 0xA0);
		retVal = A8L_TakeJpegPicture(resolution, gA8PrevZoomFactor, 1);
		if (retVal != A8_NO_ERROR)
		{
			return A8_CAM_ENCODE_ERROR;
		}

		if(resolution == 9)
			retVal = A8L_GetJpegPictureWithLimit_RAW((u_short *)pdata, maxFileSize, 9, filesize);
		else
			retVal = A8L_GetJpegPictureWithLimit((u_short *)pdata, maxFileSize, filesize);
		
		if (retVal != A8_NO_ERROR)
		{
			return A8_CAM_ENCODE_ERROR;
		}
	}

	return A8_NO_ERROR;
}

/**
 @fn		s_short A800_TakeStickerImage(u_short resolution, u_short *pdata, u_int *filesize)	 
 @brief	JPEG Capture function with StickerImage.
 @param	resolution - Depends on Customer.
 @param	pdata - the buffer pointer for storing the jpeg picture (in host ).
 @param	filesize - a pointer to a s_int variable to store the jpeg size.
 @return	u_char-0 ... Success, 1, 2, 3, 4 ... Failure
 @note	.
 @bug	N/A.
*/
s_short A800_TakeStickerImage(u_short resolution, u_short *pdata, u_int *filesize)	 
{
	WINDOW_INFO winInfo;
	s_int StickSize;
	u_int RGBImageAddr, FrameImgAddr;
	u_short ImageWidth, ImageHeight ;
	u_short retVal, trans_color;
       u_short i, j, temp_pixel;

	s_int Status;

	A8L_GetWindowAttribute( A8_MAIN_WINDOW, &winInfo );
	StickSize = 128 * 96 * 2;	// Update for TechnoBar
    
	if ( winInfo.colordepth != A8_DEPTH_16B )
		return A8_INCORRECT_PARA_ERROR;

	if ( (resolution & 0x07) == 0x05 )
	{
		ImageWidth = 128;
		ImageHeight = 96;
		//ImageWidth = A800Data.Select->BufWidth;
        	//ImageHeight = A800Data.Select->BufHeight;
	}
	else if ( (resolution & 0x07) == 0x06 )
	{
		ImageWidth = 128;
		ImageHeight = 96;
	}
	else
		return A8_OUT_OF_RANGE_ERROR;	// 2;


	retVal = A8L_StopLCDPreview();
	if ( retVal != 0 )
	{
	   AIT_Message_Error("\n[AIT800] A800_TakeStickerImage-1 Time out =%d\n",retVal); 
	   return A8_CAM_PREV_STOP_ERROR;
	}
#if 1 // 0619 for Frame update in low fps
	SetA8RegB(0x500A, 0x02);
	sys_IF_ait_delay1ms(30);
#endif
	A8L_SetWindowActive(A8_PIP_WINDOW, 0);
    
	// Using memory copy process for Frame data.
	FrameImgAddr = 0x6000;
	RGBImageAddr = 0x0000;

    trans_color = GetA8RegW(MAIN_WIN_TP_COLOR_B_RG);
    for (i=0; i<ImageHeight ; i++)
    {
	for (j=0; j<ImageWidth ; j++)
       {
		temp_pixel = GetA8MemW(FrameImgAddr);
            //if (temp_pixel == 0xF81F)
            if (temp_pixel == trans_color)
            {
            	  FrameImgAddr = FrameImgAddr + 2;
                RGBImageAddr = RGBImageAddr + 2;  
            }
            else
            {
                SetA8MemW(RGBImageAddr, temp_pixel);
            	  FrameImgAddr = FrameImgAddr + 2;
                RGBImageAddr = RGBImageAddr + 2;  
            }
       }
    }

	sys_IF_ait_delay1ms(1);

	if ( (resolution & 0x07) == 0x05 )
	{
		/* EncodeRGBToJpegByFW( 0 ); */
		A8L_EncodeRGBToJpegByFW( 5 );
		Status = A8L_GetJpegPictureBufWithLimit( pdata, 0xA000, 40*1024, filesize );
       if ( Status != 0 )
		{
			/* AIT_Message_P0(("[AIT800] Get Jpeg Picture-1 Timeout\n")); */
			return A8_CAM_ENCODE_ERROR;	// 4;
		}
	}
	else
	{
            A8L_EncodeRGBToJpegByFW( 6 );
            Status = A8L_GetJpegPictureBufWithLimit( pdata, 0xA000, 40*1024, filesize );
            if ( Status != 0 )
            {
                /* AIT_Message_P0(("[AIT800] Get Jpeg Picture-2 Timeout\n")); */
                return A8_CAM_ENCODE_ERROR;	// 4;
            }
    	}

	*filesize = A8L_GetJpegPictureSize();
#ifdef EXIFSUPPORT // 080430 match file size for exif case
	*filesize += 0x314;
#endif
	return A8_NO_ERROR;
}


/**
 @fn		s_short A800_TakeStickerImageEx(u_short resolution, u_short *sticker, u_short *pdata, u_int *filesize)
 @brief	JPEG Capture function with StickerImage.
 @param	resolution - Depends on Customer.
 @param	sticker - the buffer pointer of sticker image (in host ).
 @param	pdata - the buffer pointer for storing the jpeg picture (in host ).
 @param	filesize - a pointer to a s_int variable to store the jpeg size.
 @return	u_char-0 ... Success, 1, 2, 3, 4 ... Failure
 @note	.
 @bug	N/A.
*/
s_short A800_TakeStickerImageEx(u_short resolution, u_short *sticker, u_short *pdata, u_int *filesize)	 
{
	WINDOW_INFO winInfo;
	s_int StickSize;
	u_int RGBImageAddr;
	u_short ImageWidth, ImageHeight ;
	u_short retVal;

	s_int timeout = 0;
	s_int Status;

	A8L_GetWindowAttribute( A8_MAIN_WINDOW, &winInfo );
	StickSize = winInfo.width * winInfo.height * 2;
	if ( winInfo.colordepth != A8_DEPTH_16B )
		return A8_INCORRECT_PARA_ERROR;

	if ( (resolution & 0x07) == 0x05 )
	{
		ImageWidth = 128;
		ImageHeight = 160;
	}
	else if ( (resolution & 0x07) == 0x06 )
	{
		ImageWidth = 64;
		ImageHeight = 72;
	}
	else
		return A8_OUT_OF_RANGE_ERROR;

	/* AIT_Message_P2(("winInfo.width=%d, winInfo.height=%d\n",winInfo.width,winInfo.height)); */
	if ( winInfo.width != ImageWidth )
		return A8_INCORRECT_PARA_ERROR;

	RGBImageAddr = 0x0000;    /* 0xA000; */
	A8L_StopLCDPreview();
	A8L_SetBufferAttribute(7, ImageWidth, ImageHeight, ImageWidth*2, A8_DEPTH_16B, RGBImageAddr);  
	A8L_SetBufferAttribute(9, ImageWidth, ImageHeight, ImageWidth*2, A8_DEPTH_16B, 0xA000);

	while(((GetA8DSCStatus() & A8_DSC_READY_FOR_CMD) == 0) && timeout < A8_CMD_READY_TIME_OUT )
		timeout++;

	if ( timeout >= A8_CMD_READY_TIME_OUT )
	{
		/* AIT_Message_P0(("\n[AIT800] A800_TakeStickerImageEX-1 Time out \n")); */
	}
	/* SetGraphicsBackground( TransparentColor ); */
	SetGraphicsTransparent( 1 );
	/* retVal = CopyImageBuftoBuf( ImageWidth, ImageHeight, 9, 0, 0, 7, 0, 0, GRH_ROP_SRCCOPY ); */
	retVal = A8L_CopyImageFIFOtoBuf( sticker, ImageWidth, ImageHeight, 1, 7, 0, 0, GRH_ROP_SRCCOPY );

	sys_IF_ait_delay1ms(5);
	if ( ImageHeight == 160 )
	{
		A8L_EncodeRGBToJpegByFW( 0 );
		Status = A8L_GetJpegPictureBufWithLimit( pdata, 0xA000, 0x10000, filesize);
		/* Status = GetJpegPictureBufWithLimit( pdata, 0xC000); */
		if ( Status != 0 )
		{
			/* AIT_Message_P0(("[AIT800] EX Get Jpeg Picture-1 Timeout\n")); */
			return 4;
		}
	}
	else
	{
		A8L_EncodeRGBToJpegByFW( 1 );
		Status = A8L_GetJpegPictureBufWithLimit( pdata, 0x8000, 0x10000, filesize );
		if ( Status != 0 )
		{
			/* AIT_Message_P0(("[AIT800] EX Get Jpeg Picture-2 Timeout\n")); */
			return 4;
		}
	}

	*filesize = A8L_GetJpegPictureSize();

	return A8_NO_ERROR;
}


#if 0
//==========================================================================
// Function 	: A800_TakeContinueStickerImage()
// Purpose     : 
// Comments    :
// Parameters	:  
//		resolution : 0x05 ... 128x143, 0x06 ... 64x72
//    pdata : the address of the image buffer ( in host )
//		filesize : the image size
// Return      : 0 ... Success, 1, 2, 3, 4 ... Failure
//==========================================================================
s_short A800_TakeContinueStickerImage(u_short resolution, u_short *pdata, u_int *filesize)	 
{
	WINDOW_INFO winInfo;
	s_int StickSize;
	u_int RGBImageAddr;
	u_short ImageWidth, ImageHeight ;
	u_short retVal;

	s_int timeout = 0;
	s_int Status;

	A8L_GetWindowAttribute( A8_MAIN_WINDOW, &winInfo );
	StickSize = winInfo.width * winInfo.height * 2;
	if ( winInfo.colordepth != A8_DEPTH_16B )
		return A8_INCORRECT_PARA_ERROR;

	if ( (resolution & 0x07) == 0x05 )
	{
		ImageWidth = 128;
		ImageHeight = 160;
	}
	else if ( (resolution & 0x07) == 0x06 )
	{
		ImageWidth = 64;
		ImageHeight = 72;
	}
	else
		return A8_OUT_OF_RANGE_ERROR;

	//AIT_Message_P2(("winInfo.width=%d, winInfo.height=%d\n",winInfo.width,winInfo.height));
	if ( winInfo.width != ImageWidth )
		return A8_INCORRECT_PARA_ERROR;

	RGBImageAddr = 0x0000;		//0xA000;
	A8L_StopLCDPreview();

	A8L_SetBufferAttribute(7, ImageWidth, ImageHeight, ImageWidth*2, A8_DEPTH_16B, RGBImageAddr);	
#ifdef AIT700_API_SETTING
	A8L_SetBufferAttribute(9, ImageWidth, ImageHeight, ImageWidth*2, A8_DEPTH_16B, 0x16000);
#else
	A8L_SetBufferAttribute(9, ImageWidth, ImageHeight, ImageWidth*2, A8_DEPTH_16B, 0x20000);
#endif

	while(((GetA8DSCStatus() & A8_DSC_READY_FOR_CMD) == 0) && timeout < A8_CMD_READY_TIME_OUT )
	timeout++;

	if ( timeout >= A8_CMD_READY_TIME_OUT )
	{
	//AIT_Message_P0(("\n[AIT800] con A800_TakeStickerImage-1 Time out \n"));
	}
	//SetGraphicsBackground( TransparentColor );
	SetGraphicsTransparent( 1 );
	retVal = A8L_CopyImageBuftoBuf( ImageWidth, ImageHeight, 9, 0, 0, 7, 0, 0, GRH_ROP_SRCCOPY );

	if ( ImageHeight == 160 )
	{
		A8L_EncodeRGBToJpegByFW( 0 );
		Status = A8L_GetJpegPictureBufWithLimit( pdata, 0xA000, 0xC000, filesize );
		//Status = GetJpegPictureBufWithLimit( pdata, 0xC000 /*0x10000*/ );
		if ( Status != 0 )
		{
			//AIT_Message_P0(("[AIT800]con Get Jpeg Picture-1 Timeout\n"));
			return 4;
		}
	}
	else
	{
		A8L_EncodeRGBToJpegByFW( 1 );
		Status = A8L_GetJpegPictureBufWithLimit( pdata, 0xA000, 0xC000, filesize );
		//Status = GetJpegPictureBufWithLimit( pdata, 0xC000 /*0x10000*/ );
		if ( Status != 0 )
		{
			//AIT_Message_P0(("[AIT800]con Get Jpeg Picture-2 Timeout\n"));
			return 4;
		}
	}

	*filesize = A8L_GetJpegPictureSize();

	A8L_StartLCDPreview();
	return A8_NO_ERROR;
}
#endif

/**
 @fn		u_char 	A800_GraphicInit(void)
 @brief	Graphic and Main Window Init.
 @return	u_char
 @note	.
 @bug	N/A.
*/
u_char 	A800_GraphicInit(void)
{
	u_short rst;

EntryAITAPI;

	/* A800_SetPlaybackMode(); */
	/* A8L_SetWindowBackground(0x0000); */

	SetGraphicsForeground(0xFFFF);
	SetGraphicsBackground(0x0);

	rst=A8L_SetWindowBackground(0x0); /* background 0xBBGGRR */

	rst=A8L_SetBufferAttribute(9, 
							A800Data.MainLCDInfo.Width,///A800Data.CAM_128x160Prev.MainOSD.OSDBufWidth,
							A800Data.MainLCDInfo.Height,///A800Data.CAM_128x160Prev.MainOSD.OSDBufHeight,
							A800Data.MainLCDInfo.Width<<1,///A800Data.CAM_128x160Prev.MainOSD.OSDBufWidth<<1,
							mOSD_DEPTH, //A8_DEPTH_16B, //ming test
							0xA000);///A800Data.CAM_128x160Prev.MainOSD.OSDBufAddr);//0xA000);

	rst=A8L_BindBufferToWindow(9, A8_MAIN_WINDOW);
	///SetGraphicsForeground(0x07E0); 
	SetGraphicsForeground(0x0000); 
	
	A8L_DrawRectangletoBuf(A800Data.MainLCDInfo.Width,///A800Data.CAM_128x160Prev.MainOSD.OSDBufWidth,
							A800Data.MainLCDInfo.Height,///A800Data.CAM_128x160Prev.MainOSD.OSDBufHeight, 
							9, 
							0,   
							0, 
							SOLID_STYLE,
							GRH_ROP_SRCCOPY);

	rst=A8L_SetWindowToDisplay(A8_MAIN_WINDOW, 
							0,
							0,
							A800Data.MainLCDInfo.Width,///A800Data.MainLCDInfo.Width,
							A800Data.MainLCDInfo.Height,///A800Data.MainLCDInfo.Height,
							0,///A800Data.CAM_128x160Prev.OSDDispLcdX,
							0,///A800Data.CAM_128x160Prev.OSDDispLcdY, 
							A8_RT_NORMAL);
	
	A8L_SetWindowTransparency(A8_MAIN_WINDOW, 0, 0x00); 
	A8L_SetWindowActive(A8_MAIN_WINDOW, 0);
    

ExitAITAPI;
	return A8_NO_ERROR;
}


/**
 @fn		u_char A800_DrawFrameBGR(A8S_IMAGE * SourceImage,u_short X,u_short Y)
 @brief	Update Host Frame Data into AIT Main Window.
 @param	SourceImage -the buffer pointer of the Frame Data (in host ).
 @param	X -Start X Position of Main Window.
 @param	Y -Start Y Position of Main Window.
 @return	u_char
 @note	.
 @bug	N/A.
*/
u_char A800_DrawFrameBGR(A8S_IMAGE * SourceImage,u_short X,u_short Y)
{
	s_int i,j;
	u_int  SrcAddr,DestAddr;
	u_short stride,Data,Data2;

	stride=A800Data.MainLCDInfo.Width<<1;
	SrcAddr=SourceImage->ImageAddr;
	DestAddr=0x20000+((Y*A800Data.MainLCDInfo.Width+X)<<1);

	for(i=0;i<SourceImage->Height;i++)
	{
		for(j=0;j<SourceImage->Width;j++)
		{
			Data=*(u_short *)SrcAddr;
			Data2=(Data<<8)&0xFF00;
			Data2|=(Data>>8)&0x00FF;

			SrcAddr+=2;
			SetA8MemW(DestAddr+(j<<1), Data2);
		}
		DestAddr+=stride;
	}

	A8L_SetWindowActive(A8_MAIN_WINDOW, 1);
	return A8_NO_ERROR;

}

/**
 @fn		u_char A800_DrawFrame(A8S_IMAGE * SourceImage,u_short X,u_short Y)
 @brief	Update Host Frame Data into AIT Main Window. (During Preview!)
 @param	SourceImage -the buffer pointer of the Frame Data (in host ).
 @param	X -Start X Position of Main Window.
 @param	Y -Start Y Position of Main Window.
 @return	u_char
 @note	(During Preview!).
 @bug	N/A.
*/
u_char A800_DrawFrame(A8S_IMAGE * SourceImage,u_short X,u_short Y)
{
	s_int i,j;
	u_int  SrcAddr,DestAddr;
	u_short stride,Data;
	u_short Pstatus;

EntryAITAPI;
	Pstatus=A8L_CheckLCDPreview();
	if (Pstatus)
		A8L_PauseLCDRefresh();

	if (Pstatus)
		A8L_ResumeLCDRefresh();

	stride=A800Data.MainLCDInfo.Width<<1;
	SrcAddr=SourceImage->ImageAddr;
	DestAddr=0x20000+((Y*A800Data.MainLCDInfo.Width+X)<<1);

	for(i=0;i<SourceImage->Height;i++)
	{
		for(j=0;j<SourceImage->Width;j++)
		{
			Data=*(u_short *)SrcAddr;
			SrcAddr+=2; 
			SetA8MemW(DestAddr+(j<<1), Data);
		}
		DestAddr+=stride;
	}
#ifdef SEMIHOSTING
	/*AIT_Message_P2("DrawFrame : Width : 0x%X Height : 0x%X ",SourceImage->Width,SourceImage->Height);*/
#endif
	A8L_SetWindowActive(A8_MAIN_WINDOW, 1);
	if (Pstatus)
		A8L_ResumeLCDRefresh();

ExitAITAPI;
	return A8_NO_ERROR;
  
}


/**
 @fn		u_char A800_UpdateLCD(void)
 @brief	Set AIT LCD controller update LCD.
 @return	u_char
 @note	.
 @bug	N/A.
*/
u_char A800_UpdateLCD(void)
{
#if 0
	u_char retVal = 0;
EntryAITAPI;

	if ( A8L_UpdateLCD() )
		retVal = 1;

ExitAITAPI;
	return retVal;
#else
	return A8L_UpdateLCD();
#endif
}

/**
 @fn		u_char 	A800_TestRegisterAccess(void)
 @brief	Test function AIT chip`s register.
 @return	u_char
 @note	.
 @bug	N/A.
*/
u_char A800_TestRegisterAccess(void)
{
	u_short i,Val;
	u_short ErrorCount;

	for (i=0; i<16; i++ )
		SetA8RegB(i+0x6540, i );
	ErrorCount = 0;

	for (i=0; i<16; i++ )
	{
		Val =(u_short) GetA8RegB(i+0x6540);
		if ( Val != i )
		{ 
			ErrorCount++;
		 	MSG_FATAL( "Reg[%04x]= %02x\n",(i+0x6540),Val,0) ;
		}
	}
	if ( ErrorCount > 0 )
	{
		MSG_FATAL( "AIT RegB R/W error ! Count %d\n", ErrorCount,0,0) ;
	}
	else
	{
		MSG_FATAL( "AIT RegB R/W ok ! Count %d\n",ErrorCount,0,0) ;
	}  
      
	for (i=0; i<16; i+=2 )
		SetA8RegW(i+0x6540, i<<7 );
	ErrorCount = 0;

	for (i=0; i<16; i+=2 )
	{
		Val = GetA8RegW(i+0x6540);
		if ( Val != (i<<7) )
		{
			ErrorCount++;
			MSG_FATAL( "RegW[%04x]= %04x\n", (i+0x6540), Val,0) ;
		}
	}
	if ( ErrorCount > 0 )
	{
		MSG_FATAL( "AIT RegW R/W error ! Count %d\n",ErrorCount,0,0) ;
	}
	else
	{
		MSG_FATAL( "AIT RegW R/W ok ! Count %d\n", ErrorCount,0,0) ;    
	}   
	return A8_NO_ERROR;
}

/**
 @fn		u_char 	A800_TestMemoryAccess(void)
 @brief	Test function AIT chip`s memory.
 @return	u_char
 @note	.
 @bug	N/A.
*/
u_char 	A800_TestMemoryAccess(void)
{
   u_int Val, i;
   u_int ErrorCount,nPattern;
   u_char cPattern,cVal;
   u_int nMemsize;

   nMemsize=96*1024;

   
   //=====================================================
   //Byte test, pattern 0x00
   cPattern=0x55;
   for (i=0; i<nMemsize; i++ )
      SetA8MemB(i, cPattern );

   ErrorCount = 0;


   for (i=0; i<nMemsize; i++ )
   {
		cVal = GetA8MemB(i);
		if ( cVal != cPattern )
		{
			AIT_Message_P2("MemB [%d]=%d \n",i,Val);
			ErrorCount++;
		}
   }
   if ( ErrorCount > 0 )
   {
      MSG_FATAL("AIT MemB 1 R/W error ! Count %d, pattern:0x%x \n", ErrorCount, cPattern ,0) ;
   }
   else
   { 
      MSG_FATAL("AIT MemB 1 R/W ok ! Count %d, pattern:0x%x \n", ErrorCount, cPattern ,0) ;   
	}
   //=====================================================

   //=====================================================
   //Byte test, pattern 0xff
   cPattern=0xaa;
   for (i=0; i<nMemsize; i++ )
      SetA8MemB(i, cPattern );

   ErrorCount = 0;


   for (i=0; i<nMemsize; i++ )
   {
      cVal = GetA8MemB(i);
      if ( cVal != cPattern )
      {
		AIT_Message_P2("MemB [%d]=%d \n",i,Val);
		ErrorCount++;
   	}
   }
   if ( ErrorCount > 0 ){
      MSG_FATAL( "AIT MemB 2 R/W error ! Count %d, pattern:0x%x \n", ErrorCount, cPattern,0) ;
   }else{
      MSG_FATAL( "AIT MemB 2 R/W ok ! Count %d, pattern:0x%x \n", ErrorCount, cPattern,0) ;   
   }   
   //=====================================================

   //=====================================================
   //Byte test, pattern 0xff
   cPattern=0x55;
   for (i=0; i<nMemsize; i++ )
      SetA8MemB(i, cPattern+i );

   ErrorCount = 0;


   for (i=0; i<nMemsize; i++ )
   {
      cVal = GetA8MemB(i);
      if ( cVal != (u_char)(cPattern+i) )
      {
		AIT_Message_P2("MemB [%d]=%d \n",i,Val);
		ErrorCount++;
   	}
   }
   if ( ErrorCount > 0 ){
      MSG_FATAL( "AIT MemB 3 R/W error ! Count %d, pattern:0x%x \n", ErrorCount, cPattern,0) ;
   }else{
      MSG_FATAL( "AIT MemB 3 R/W ok ! Count %d, pattern:0x%x \n", ErrorCount, cPattern,0) ;   
   }   
   //=====================================================

   //=====================================================
   //Byte test, pattern 0xff
   cPattern=0xaa;
   for (i=0; i<nMemsize; i++ )
      SetA8MemB(i, cPattern+i );

   ErrorCount = 0;

  
   for (i=0; i<nMemsize; i++ )
   {
      cVal = GetA8MemB(i);
      if ( cVal != (u_char)(cPattern+i) )
      {
		AIT_Message_P2("MemB [%d]=%d \n",i,Val);
		ErrorCount++;
   	}
   }
   if ( ErrorCount > 0 ){
      MSG_FATAL( "AIT MemB 4 R/W error ! Count %d, pattern:0x%x \n", ErrorCount, cPattern,0) ;
   }else{ 
      MSG_FATAL( "AIT MemB 4 R/W ok ! Count %d, pattern:0x%x \n", ErrorCount, cPattern,0) ;   
   }   
   //=====================================================


   //=====================================================
   //Word test, pattern 0x0000
   nPattern=0x55aa;
   for (i=0; i<nMemsize/2; i+=2 )
      SetA8MemW(i, nPattern );
   ErrorCount = 0;

 
   for (i=0; i<nMemsize/2; i+=2 )
   {
      Val = GetA8MemW(i);
      if ( Val != nPattern)
      {
	  AIT_Message_P2("MemW [%d]=%d \n",i,Val);
	  ErrorCount++;
   	}
   }
   if ( ErrorCount > 0 ){
      MSG_FATAL( "AIT MemW  1 R/W error ! Count %d, pattern:0x%x\n",ErrorCount, nPattern,0) ;
   }else{
      MSG_FATAL( "AIT MemW  1 R/W ok ! Count %d, pattern:0x%x\n",ErrorCount, nPattern,0) ;
   }   
   //=====================================================

   //=====================================================
   //Word test, pattern 0xffff
   nPattern=0xaa55;
   for (i=0; i<nMemsize/2; i+=2 )
      SetA8MemW(i, nPattern );
   ErrorCount = 0;

 
   for (i=0; i<nMemsize/2; i+=2 )
   {
      Val = GetA8MemW(i);
      if ( Val != nPattern )
      {
	  AIT_Message_P2("MemW [%d]=%d \n",i,Val);
	  ErrorCount++;
   	}
   }
   if ( ErrorCount > 0 ){
      MSG_FATAL( "AIT MemW 2 R/W error ! Count %d, pattern:0x%x\n",ErrorCount, nPattern,0) ;
   }else{
      MSG_FATAL( "AIT MemW 2 R/W ok ! Count %d, pattern:0x%x\n",ErrorCount, nPattern,0) ;
   }   
   //=====================================================   

   //=====================================================
   //Word test, pattern 0x5555
   nPattern=0x5555;
   for (i=0; i<nMemsize/2; i+=2 )
      SetA8MemW(i, nPattern + i);
   ErrorCount = 0;

 
   for (i=0; i<nMemsize/2; i+=2 )
   {
      Val = GetA8MemW(i);
      if ( Val != (u_short)(nPattern + i))
      {
	  AIT_Message_P2("MemW [%d]=%d \n",i,Val);
	  ErrorCount++;
   	}
   }
   if ( ErrorCount > 0 ){
      MSG_FATAL( "AIT MemW 3 R/W error ! Count %d, pattern:0x%x\n",ErrorCount, nPattern,0) ;
   }else{
      MSG_FATAL( "AIT MemW 3 R/W ok ! Count %d, pattern:0x%x\n",ErrorCount, nPattern,0) ;
   }   
   //=====================================================

   //=====================================================
   //Word test, pattern 0xaaaa
   nPattern=0xaaaa;
   for (i=0; i<nMemsize/2; i+=2 )
      SetA8MemW(i, nPattern+i);
   ErrorCount = 0;

 
   for (i=0; i<nMemsize/2; i+=2 )
   {
      Val = GetA8MemW(i);
      if ( Val != (u_short)(nPattern+i) )
      {
	  AIT_Message_P2("MemW [%d]=%d \n",i,Val);
	  ErrorCount++;
   	}
   }
   if ( ErrorCount > 0 ){
      MSG_FATAL("AIT MemW 4 R/W error ! Count %d, pattern:0x%x\n",ErrorCount, nPattern,0) ;
   }else{
      MSG_FATAL("AIT MemW 4 R/W ok ! Count %d, pattern:0x%x\n",ErrorCount, nPattern,0) ;
   }   
   //=====================================================   
	return A8_NO_ERROR;
}

/**
 @fn		u_short 	A800_GetFirmwareMode(void)
 @brief	Get AIT FW`s Version.
 @return	u_short
 @note	.
 @bug	N/A.
*/
u_short 	A800_GetFirmwareMode(void)
{
	return A8L_GetA8FirmwareVersion();
}


/**
 @fn		u_char 	A800_DownloadFirmware(u_char *ptr, u_int fwSize)
 @brief	Download AIT chip`s FW.
 @param	ptr -the buffer of the AIT FW Data (in host ).
 @return	u_char
 @note	.
 @bug	N/A.
*/
u_char 	A800_DownloadFirmware(u_char *ptr, u_int fwSize)
{
	s_int i;
	u_char retVal;   
	s_int timeout = 0;  

EntryAITAPI;
	for (i = 0; i < 10; i++)
	{  
		SetA8RegB(0x6901, 0x46);   
	}

	retVal = A8L_DownloadA8Firmware(ptr, fwSize);
	while(!(GetA8RegB(0x654e) & 0x10) && timeout < 0x1000)
    {
        sys_IF_ait_delay1ms(1);
		timeout++;
	}

	MSG_FATAL("A800_DownloadFirmware timeout = %d",timeout,0,0);
ExitAITAPI;
	return retVal;
}

/**
 @fn		u_char A800_Preview_MainWindow_Transparency( u_short enable, u_int keycolor )
 @brief	Enable/Disable Main Window Transparency.
 @param	enable - Enable/Disable.
 @param	keycolor - Transparency Color.
 @return	u_char
 @note	.
 @bug	N/A.
*/
u_char A800_Preview_MainWindow_Transparency( u_short enable, u_int keycolor )
{
EntryAITAPI;
  /* TempColor = ((keycolor & 0x0FF) << 8) + (keycolor >> 8); */
  A8L_SetWindowTransparency(A8_MAIN_WINDOW, enable, keycolor); /* Black */
  SetGraphicsBackground( keycolor );
ExitAITAPI;

  return A8_NO_ERROR;
}


/**
 @fn		u_char A800_Preview_MainWindow_SemiTransparency (u_int chromakey, u_short transparencyRate)
 @brief	Semi Transparency of Main Window.
 @param	chromakey -Transparency Colore.
 @param	transparencyRate -Transparency Rate .
 @return	u_char
 @note	.
 @bug	N/A.
*/
u_char A800_Preview_MainWindow_SemiTransparency (u_int chromakey, u_short transparencyRate)
{
EntryAITAPI;
  A8L_SetWindowSemiTransparency(A8_MAIN_WINDOW, 1, chromakey, FUNX_AVERAGE, transparencyRate);
  A8L_SetWindowSemiTransparency(A8_PIP_WINDOW, 1, chromakey, FUNX_AVERAGE, 0x10-transparencyRate);
  A8L_SetWindowPriority(A8_OVERLAY_WINDOW, A8_MAIN_WINDOW, A8_PIP_WINDOW, 0x00);
ExitAITAPI;

  return A8_NO_ERROR;
}


/**
 @fn		s_short A800_UpdateOSDBuffer( u_short mode, u_short *MemPtr, u_short x, u_short y, u_short w, u_short h )
 @brief	Update data into OSD buffer of different Preview mode.
 @param	mode -Normal/Full/~~ preview mode.
 @param	MemPtr -Data pointer in host.
 @param	x -start x of update position in OSD.
 @param	y -start y of update position in OSD.
 @param	w -width of update area in OSD.
 @param	h -height of update area in OSD.
 @return	s_short
 @note	For Preview mode.
 @bug	N/A.
*/
s_short A800_UpdateOSDBuffer( u_short mode, u_short *MemPtr, u_short x, u_short y, u_short w, u_short h )
{
	u_short Width, Height;
	s_short i;
	u_int addr, base;
	u_short *TempMemPtr=MemPtr;
	extern void Trans565to332ToA8(u_int destAddr, u_short *srcAddr, u_int length);


	A800Data.Select = A800_ChangeCameraSetting(mode);
	if(!A800Data.Select )
		return A8_SYSTEM_ERROR;

	if(x!=0 ||y!=0||w!=A800Data.MainLCDInfo.Width||h!=A800Data.MainLCDInfo.Height)
	{
		OSDInfoDT* OSDInfoPtr = NULL;


		if(A800Data.Select->MainOSD.OSDEnable&&
			x>=A800Data.Select->MainOSD.OSDDispLcdX && 
			y >= A800Data.Select->MainOSD.OSDDispLcdY&&
			x+w <= (A800Data.Select->MainOSD.OSDDispLcdX+A800Data.Select->MainOSD.OSDBufWidth)&&
			y+h <= (A800Data.Select->MainOSD.OSDDispLcdY+A800Data.Select->MainOSD.OSDBufHeight))
		{

			OSDInfoPtr = &A800Data.Select->MainOSD;
			AIT_Message_P4("MainOSD: (x,y,w,h) = (%d,%d,%d,%d,)",x,y,w,h);

		}
		else if(A800Data.Select->OverlayOSD.OSDEnable&&
			x>=A800Data.Select->OverlayOSD.OSDDispLcdX && 
			y >= A800Data.Select->OverlayOSD.OSDDispLcdY&&
			x+w <= (A800Data.Select->OverlayOSD.OSDDispLcdX+A800Data.Select->OverlayOSD.OSDBufWidth)&&
			y+h <= (A800Data.Select->OverlayOSD.OSDDispLcdY+A800Data.Select->OverlayOSD.OSDBufHeight))
		{

			OSDInfoPtr = &A800Data.Select->OverlayOSD;
			AIT_Message_P4("OverlayOSD: (x,y,w,h) = (%d,%d,%d,%d,)",x,y,w,h);
	
		}

		else if(A800Data.Select->IconOSD.OSDEnable&&
			x>=A800Data.Select->IconOSD.OSDDispLcdX && 
			y >= A800Data.Select->IconOSD.OSDDispLcdY&&
			x+w <= (A800Data.Select->IconOSD.OSDDispLcdX+A800Data.Select->IconOSD.OSDBufWidth)&&
			y+h <= (A800Data.Select->IconOSD.OSDDispLcdY+A800Data.Select->IconOSD.OSDBufHeight))
		{
			AIT_Message_P4("IconOSD: (x,y,w,h) = (%d,%d,%d,%d,)",x,y,w,h);

			OSDInfoPtr = &A800Data.Select->IconOSD;
		}

		if(OSDInfoPtr)
		{

			Width = OSDInfoPtr->OSDBufWidth;
			Height = OSDInfoPtr->OSDBufHeight;
			base = OSDInfoPtr->OSDBufAddr;

			TempMemPtr = MemPtr;
#if defined(EBI_BUS_8BIT_MODE)
			addr=base+(Width*y+x)*2;
#else /* EBI_BUS_8BIT_MODE */		
			TempMemPtr += A800Data.MainLCDInfo.Width*y+
										OSDInfoPtr->OSDDispLcdX;
#endif /* EBI_BUS_8BIT_MODE */
			if (base >= 0x18000)
				return A8_OUT_OF_RANGE_ERROR;
			if(A8_DEPTH_8B == OSDInfoPtr->OSDBufFormat){
				SetA8RegB(0x5040,GetA8RegB(0x5040)&0xe0);
				base+=OSDInfoPtr->OSDBufWidth*(y-OSDInfoPtr->OSDDispLcdY);//+(x-OSDInfoPtr->OSDDispLcdX);
				while(h--){
					Trans565to332ToA8(base, (u_short *)TempMemPtr, w);
					base += OSDInfoPtr->OSDBufWidth;
					TempMemPtr += A800Data.MainLCDInfo.Width;
				}
			}else{
				SetA8RegB(0x5040,GetA8RegB(0x5040)|0x10);			
				for ( i=0; i<h; i++ ){
#if defined(EBI_BUS_8BIT_MODE)
					CopyMemWordHostToA8( addr, (u_short *) TempMemPtr, w*2 );
					addr += (Width*2);
					TempMemPtr += w;//A800Data.MainLCDInfo.Width;

#else /* EBI_BUS_8BIT_MODE */	 			
					CopyMemWordHostToA8( base, (u_short *) TempMemPtr, Width*2 );
					base += (Width*2);
					TempMemPtr += A800Data.MainLCDInfo.Width;
#endif /* EBI_BUS_8BIT_MODE */				
				}
			}



		}
		return A8_NO_ERROR;
	}

	if (1 /*(mode <= AITCAM_PREV_176x144_MODE) || 
		(mode == AITVDO_PREV_176x144_MODE)  ||
		(mode == AITVDO_PLAY_176x144_MODE)*/)
	{
		if (A800Data.Select->MainOSD.OSDEnable)
		{
			Width = A800Data.Select->MainOSD.OSDBufWidth;
			Height = A800Data.Select->MainOSD.OSDBufHeight;
			base = A800Data.Select->MainOSD.OSDBufAddr;

			TempMemPtr = MemPtr;
			TempMemPtr += A800Data.MainLCDInfo.Width*A800Data.Select->MainOSD.OSDDispLcdY+
										A800Data.Select->MainOSD.OSDDispLcdX;

			if (base >= 0x18000)
				return A8_OUT_OF_RANGE_ERROR;
			if(A8_DEPTH_8B == A800Data.Select->MainOSD.OSDBufFormat){
				SetA8RegB(0x5040,GetA8RegB(0x5040)&0xe0);			
				while(Height--){
					Trans565to332ToA8(base, (u_short *)TempMemPtr, Width);
					base += Width;
					TempMemPtr += A800Data.MainLCDInfo.Width;
				}
			}else{
				SetA8RegB(0x5040,GetA8RegB(0x5040)|0x10);			
				for ( i=0; i<Height; i++ ){
					CopyMemWordHostToA8( base, (u_short *) TempMemPtr, Width*2 );
					base += (Width*2);
					TempMemPtr += A800Data.MainLCDInfo.Width;
				}
			}
		}
			
		if (A800Data.Select->OverlayOSD.OSDEnable)
		{
			Width = A800Data.Select->OverlayOSD.OSDBufWidth;
			Height = A800Data.Select->OverlayOSD.OSDBufHeight;
			base = A800Data.Select->OverlayOSD.OSDBufAddr;

			TempMemPtr = MemPtr;
			TempMemPtr += A800Data.MainLCDInfo.Width*A800Data.Select->OverlayOSD.OSDDispLcdY+
										A800Data.Select->OverlayOSD.OSDDispLcdX;



			if (base >= 0x18000)
				return A8_OUT_OF_RANGE_ERROR;
			
			if(A8_DEPTH_8B == A800Data.Select->OverlayOSD.OSDBufFormat)
			{
				SetA8RegB(0x5040,GetA8RegB(0x5040)&0xb0);			
				while(Height--)
				{
					Trans565to332ToA8(base, (u_short *)TempMemPtr, Width);
					base += Width;
					TempMemPtr += A800Data.MainLCDInfo.Width;
				}
			}
			else
			{
				SetA8RegB(0x5040,GetA8RegB(0x5040)|0x40);				
				for ( i=0; i<Height; i++ )
				{
					CopyMemWordHostToA8( base, (u_short *) TempMemPtr, Width*2 );
					base += (Width*2);
					TempMemPtr += A800Data.MainLCDInfo.Width;
				}
			}
		}

		if (A800Data.Select->IconOSD.OSDEnable)
		{
			Width = A800Data.Select->IconOSD.OSDBufWidth;
			Height = A800Data.Select->IconOSD.OSDBufHeight;
			base = A800Data.Select->IconOSD.OSDBufAddr;

			TempMemPtr = MemPtr;
			TempMemPtr += A800Data.MainLCDInfo.Width*A800Data.Select->IconOSD.OSDDispLcdY+
										A800Data.Select->IconOSD.OSDDispLcdX;
			if (base >= 0x18000)
				return A8_OUT_OF_RANGE_ERROR;

			if(A8_DEPTH_8B == A800Data.Select->IconOSD.OSDBufFormat){
					SetA8RegB(0x5040,GetA8RegB(0x5040)&0x70);			
				while(Height--){
					Trans565to332ToA8(base, (u_short *)TempMemPtr, Width);
					base += Width;
					TempMemPtr += A800Data.MainLCDInfo.Width;
				}
			}else{
				SetA8RegB(0x5040,GetA8RegB(0x5040)|0x80);				
				for ( i=0; i<Height; i++ )
				{
					CopyMemWordHostToA8( base, (u_short *) TempMemPtr, Width*2 );
					base += (Width*2);
					TempMemPtr += A800Data.MainLCDInfo.Width;
				}
			}
		}
	}
	else
	{
		Width = A800Data.Select->MainOSD.OSDBufWidth;
		Height = A800Data.Select->MainOSD.OSDBufHeight;

		base = A800Data.Select->MainOSD.OSDBufAddr;
		addr = base + (y * Width + x) * 2;

		if (addr >= 0x18000)
			return A8_OUT_OF_RANGE_ERROR;

		for ( i=0; i<h; i++ )
		{
			CopyMemWordHostToA8( addr, (u_short *) TempMemPtr, w*2 );
			addr += (Width*2);
			TempMemPtr += w;
		}
	}
	AIT_Message_P0("A800_UpdateOSDBuffer-");
	
	return A8_NO_ERROR;
}


/**
 @fn		u_char A800_ClearOSDBuffer(u_short chromaKey, u_short mode)
 @brief	Full same color into OSD buffer of different Preview/Playback mode.
 @param	chromaKey -Color.
 @param	mode -Preview or Playback mode.
 @return	u_char
 @note	.
 @bug	N/A.
*/
u_char A800_ClearOSDBuffer(u_short chromaKey, u_short mode)
{
	
	A800Data.Select = A800_ChangeCameraSetting(mode);
	if(!A800Data.Select )
		return A8_SYSTEM_ERROR;


	if(A800Data.Select->MainOSD.OSDEnable)
	{
		ClearA8Mem(A800Data.Select->MainOSD.OSDBufAddr, chromaKey, 
			A800Data.Select->MainOSD.OSDBufWidth*A800Data.Select->MainOSD.OSDBufHeight*2);
	}

	if(A800Data.Select->OverlayOSD.OSDEnable)
	{
		ClearA8Mem(A800Data.Select->OverlayOSD.OSDBufAddr, chromaKey, 
			A800Data.Select->OverlayOSD.OSDBufWidth*A800Data.Select->OverlayOSD.OSDBufHeight*2);

	}

	if(A800Data.Select->IconOSD.OSDEnable)
	{
		ClearA8Mem(A800Data.Select->IconOSD.OSDBufAddr, chromaKey, 
			A800Data.Select->IconOSD.OSDBufWidth*A800Data.Select->IconOSD.OSDBufHeight*2);

	}

	return A8_NO_ERROR;
}


/**
 @fn		s_short A800_UpdateFrameBuffer( u_short mode, u_short *MemPtr, u_short x, u_short y, u_short w, u_short h )
 @brief	Update data into playback buffer of different Playback mode.
 @param	mode -Normal/Full/~~ playback mode.
 @param	MemPtr -Data pointer in host.
 @param	x -start x of update position in playback buffer.
 @param	y -start y of update position in playback buffer.
 @param	w -width of update area in playback buffer.
 @param	h -height of update area in playback buffer.
 @return	s_short
 @note	For Playback mode.
 @bug	N/A.
*/
s_short A800_UpdateFrameBuffer( u_short mode, u_short *MemPtr, u_short x, u_short y, u_short w, u_short h )
{
	u_short Width, Height;
	s_short i;
	u_int addr, base;
	u_short *TempMemPtr=MemPtr; 

	A800Data.Select = A800_ChangeCameraSetting(mode);

	Width = A800Data.Select->BufWidth;
	Height = A800Data.Select->BufHeight;

	base = A800Data.Select->BufAddr;
	addr = base + (y * Width + x) * 2;

	if (addr >= 0x18000)
		return A8_OUT_OF_RANGE_ERROR;

	for ( i=0; i<h; i++ )
	{
		CopyMemWordHostToA8( addr, (u_short *) TempMemPtr, w*2 );
		addr += (Width*2);
		TempMemPtr += w;
	}

	return A8_NO_ERROR;
}


/**
 @fn		s_short A800_UpdateMainWindowMemory( u_short *MemPtr, u_short x, u_short y, u_short w, u_short h )
 @brief	Update data into main window buffer.
 @param	MemPtr -Data pointer in host.
 @param	x -start x of update position in main window buffer.
 @param	y -start y of update position in main window buffer.
 @param	w -width of update area in main window buffer.
 @param	h -height of update area in main window buffer.
 @return	s_short
 @note	Main Window Buffer.
 @bug	N/A.
*/
s_short A800_UpdateMainWindowMemory( u_short *MemPtr, u_short x, u_short y, u_short w, u_short h )
{
	u_short Width, Height;
	s_short i;
	u_int addr, base;
	u_short *TempMemPtr=MemPtr; 

    	Width = A800Data.Select->MainOSD.OSDBufWidth;
	Height = A800Data.Select->MainOSD.OSDBufHeight;

	base = A800Data.Select->MainOSD.OSDBufAddr;
	addr = base + (y * Width + x) * 2;

	if (addr >= 0x18000)
	{
		AIT_Message_P2("Access invalid memory address!!%s,%d", __FILE__,__LINE__);
		return A8_OUT_OF_RANGE_ERROR;
	}
	for ( i=0; i<h; i++ )
	{
		CopyMemWordHostToA8( addr, (u_short *) TempMemPtr, w*2 );
		addr += (Width*2);
		TempMemPtr += w;
	}

	return A8_NO_ERROR;
}


/**
 @fn		u_char A800_ClearMainWindowMemory(u_short chromaKey)
 @brief	Full same color into main window buffer.
 @param	chromaKey -Color.
 @return	u_char
 @note	Main Window Buffer.
 @bug	N/A.
*/
u_char A800_ClearMainWindowMemory(u_short chromaKey)
{
	u_int  DestAddr;
	DestAddr= A800Data.Select->MainOSD.OSDBufAddr;
EntryAITAPI;
	ClearA8Mem(DestAddr, chromaKey, 0xA000);
ExitAITAPI;
	return A8_NO_ERROR;
}

/**
 @fn		u_char A800_ClearWindowMemory(void)
 @brief	Full black color into (PIP) window buffer.
 @return	u_char
 @note	PIP Window Buffer.
 @bug	N/A.
*/
u_char A800_ClearWindowMemory(void)
{
	u_int  DestAddr;
	DestAddr=0x0000;              
EntryAITAPI;
	ClearA8Mem(DestAddr, 0, 0xA000);
ExitAITAPI;
	return A8_NO_ERROR;
}

/**
 @fn		u_short A800_DecodeJpeg( u_short *jpgData, u_int jpgLength, u_short *outW, u_short *outH, u_short *rgbBuf, u_char autoScale )
 @brief	This function is used for decoding a jpeg and output the rgb image in Host.
 @param	jpgData -JPEG Data pointer in host.
 @param	jpgLength -JPEG Data length.
 @param	outW -the required width of decoded RGB image.
 @param	outH -the required height of decoded RGB image.
 @param	rgbBuf -the buffer pointer for storing the decoded RGB image.
 @param	autoScale -Auto Scale Enable/Disable.
 @return	u_short
 @note	.
 @bug	N/A.
*/
u_short A800_DecodeJpeg( u_short *jpgData, u_int jpgLength, u_short *outW, u_short *outH, u_short *rgbBuf, u_char autoScale )
{
	s_short jpegFormat, jpegWidth, jpegHeight;
	u_short tempWidth, tempHeight, rateV, rateH, Base, i, LineOffset,Quality;
	s_short x0, y0, x1, y1, retVal;
	u_int BufAddr;
	A8S_CameraParam A8S_Cameraparam;

	MSG_FATAL("A800_DecodeJpeg",0,0,0);
	retVal = A8L_GetJpegInfo( jpgData, jpgLength, &jpegWidth, &jpegHeight, &jpegFormat ,&Quality);
	if (retVal != 0)
	{  
		AIT_Message_P2( "[A800_DecodeJpeg] ERROR jpgW:%d, jpgH:%d", jpegWidth, jpegHeight ) ;
		return 1;
	}
	AIT_Message_P3("[A800_DecodeJpeg] jpgW:%d, jpgH:%d, format=%d", jpegWidth, jpegHeight, jpegFormat );
	if ( autoScale )
	{
		tempWidth = *outW & 0xFFFC;
		tempHeight = *outH;
		if ((tempWidth <= jpegWidth) && (tempHeight <= jpegHeight)) // Scale down
		{
			Base = 60;	// this value need to sync with Preview.
			rateH = tempWidth * Base / jpegWidth;			
			rateV = tempHeight * Base / jpegHeight;

			if( (jpegWidth * rateH / Base - tempWidth ) < 0 )
				rateH+=1;
			if( (jpegHeight* rateV / Base - tempHeight) < 0 )
				rateV+=1;
			if (rateH < rateV)
				rateH = rateV;
	
			x0 = (jpegWidth * rateH / Base - tempWidth)/2;
			if ((x0 %2) ==0)
			  x0+=1;
			x1 = x0 + tempWidth - 1;
			y0 = (jpegHeight * rateH / Base - tempHeight)/2 + 1;
			y1 = y0 + tempHeight - 1;
		}
		else		// Scale up
		{
			Base = 20;	

			if(jpegWidth==1 || jpegHeight==1 )
				return 0xffff;
			
			rateH = tempWidth * Base / (jpegWidth-1);
			rateV = tempHeight * Base / (jpegHeight-1);

			if( ((jpegWidth-1) * rateH / Base - tempWidth) < 0 )
				rateH+=1;
			if( ((jpegHeight-1) * rateV / Base - tempHeight) < 0 )
				rateV+=1;
			if (rateH < rateV)
				rateH = rateV;

			if (rateH > 100)
			{
				rateH = 100 ;
				tempWidth = (jpegWidth-1) * rateH / Base;
				tempHeight = (jpegHeight-1) * rateV / Base ;
			}
	
			x0 = ((jpegWidth-1) * rateH / Base - tempWidth)/2;
			if ((x0 %2) ==0)
			  x0+=1;
			x1 = x0 + tempWidth - 1;
			y0 = ((jpegHeight-1) * rateH / Base - tempHeight)/2 + 1;
			y1 = y0 + tempHeight - 1;
		}
	
	}
	else		// No Scale Up/Down !!
	{
		tempWidth = *outW;
		tempHeight = *outH;
		
		if (tempWidth > jpegWidth)
			tempWidth = jpegWidth;
		
		if (tempHeight > jpegHeight)
			tempHeight = jpegHeight;
		
		rateH = 64;
		Base = 64;
		x0 = (jpegWidth - tempWidth)/2;
		if ((x0 %2) ==0)
		  x0+=1;
		x1 = x0 + tempWidth - 1;
		y0 = (jpegHeight - tempHeight)/2 + 1;
		y1 = y0 + tempHeight - 1;
	}
	
	*outW = tempWidth;
	*outH = tempHeight;
		
	AIT_Message_P2("[A800_DecodeJpeg] tgtW:%d, tgtH:%d \n", tempWidth, tempHeight );
	AIT_Message_P2("[A800_DecodeJpeg] M(Base):%d, N(rateH):%d \n", Base, rateH );

	if (tempHeight%2 ==1)
		y1 +=1;
	//AIT_Message_P4("[A800_DecodeJpeg] x0:%d, y0:%d, x1:%d, y1:%d \n", x0, y0, x1, y1 );

	A8S_Cameraparam.JpgWidth = tempWidth;
	A8S_Cameraparam.JpgHeight = tempHeight;
	A8S_Cameraparam.ScaleN = rateH;
	A8S_Cameraparam.ScaleM = Base;
	A8S_Cameraparam.startx = x0;
	A8S_Cameraparam.starty = y0;
	A8S_Cameraparam.endx = x1;
	A8S_Cameraparam.endy = y1;

	BufAddr = A800Data.Select->BufAddr;//0L;

   	if(CAM_LCD_REFRESH_SCALE == A800Data.Select->LcdRefreshMode)	
		retVal = A8L_DecodeJpegToYuv( jpgData, jpgLength, BufAddr, A8S_Cameraparam);	
	else
		retVal = A8L_DecodeJpegPicture( jpgData, jpgLength, BufAddr, A8S_Cameraparam);
	
	if ( retVal >= A8_JPEG_DECODE_SUCCESS )
	{
		return 2;
	}

	if (rgbBuf != NULL)
	{
		//CopyMemWordA8ToHost(rgbBuf, 0L, tempWidth*tempHeight*2);	
		if(tempWidth % 4)
			LineOffset = ((tempWidth + (4- tempWidth %4)) << 1);
		else
			LineOffset = (tempWidth << 1);

		if ( (tempWidth% 4) != 1 ) 
		{
			if ((tempWidth% 4) == 0)
			{	
				// 20070621, update. Memory copy is more stable!!
				//added by ming @20091105 for test 
				extern unsigned short gA8MainLCDWidth;
				u_int h;
				u_short *tmprgbBuf;	
				extern unsigned char JpegdecodeDummyPixel;
				for(h=0;h<tempHeight;h++)
				{	
						tmprgbBuf = rgbBuf;
						CopyMemWordA8ToHost(tmprgbBuf, 0L+tempWidth*2*h, (tempWidth-JpegdecodeDummyPixel)*2);    						
						rgbBuf += gA8MainLCDWidth;
				}
			}
			else
			{
				A8L_SetBufferAttribute(1, tempWidth, tempHeight, LineOffset, A8_DEPTH_16B, 0L);
				A8L_CopyImageBuftoFIFO(rgbBuf, tempWidth, tempHeight, 1, 0, 0);
			}
		}
		else
		{
			//BufAddr = DecodeDataBuf;
			for ( i=0; i<tempHeight; i++ ) 
			{
				CopyMemWordA8ToHost(rgbBuf, BufAddr, tempWidth <<1);	/* length in byte*/
				rgbBuf += tempWidth;
				BufAddr += (tempWidth << 1);
				if (i & 0x01)
					BufAddr += 4;
			}
		}
	}
	return A8_NO_ERROR;
}

/**
 @fn		
 @brief	This function is used for decoding a jpeg and output the rgb image in Host.
 @param	jpgData -JPEG Data pointer in host.
 @param	jpgLength -JPEG Data length.
 @param	outW -the required width of decoded RGB image.
 @param	outH -the required height of decoded RGB image.
 @param	rgbBuf -the buffer pointer for storing the decoded RGB image.
 @param	autoScale -Auto Scale Enable/Disable.
 @return	u_short
 @note	.
 @bug	N/A.
*/
u_short A800_DecodeJpegClip( u_short *jpgData, u_int jpgLength, 
							u_short srcStartx, u_short srcStarty, u_short srcEndx, u_short srcEndy,
							u_short* outW,u_short* outH,u_short *rgbBuf)
{
	s_short jpegFormat, jpegWidth, jpegHeight;
	s_short srcWidth, srcHeight;
	u_short tempWidth, tempHeight, rateV, rateH, Base, i, LineOffset,Quality;
	s_short x0, y0, x1, y1, retVal;
	u_int BufAddr;
	A8S_CameraParam A8S_Cameraparam;

	srcWidth = srcEndx-srcStartx;
	srcHeight = srcEndy-srcStarty;
	



	
	retVal = A8L_GetJpegInfo( jpgData, jpgLength, &jpegWidth, &jpegHeight, &jpegFormat ,&Quality);
	if (retVal != 0)
	{  
		//AIT_Message_P2( "[A800_DecodeJpeg] Error jpgW=%d, jpgH=%d ",jpegWidth,jpegHeight);
		return 1;
	}


	if(srcWidth>jpegWidth||srcHeight>jpegHeight)
		return 1;
	
	if ( 1 )
	{
		tempWidth = *outW & 0xFFFC;
		tempHeight = *outH;
		if ((tempWidth <= srcWidth) && (tempHeight <= srcHeight)) // Scale down
		{
			Base = 60;	// this value need to sync with Preview.
			rateH = tempWidth * Base / jpegWidth;			
			rateV = tempHeight * Base / jpegHeight;

			if( (jpegWidth * rateH / Base - tempWidth ) < 0 )
				rateH+=1;
			if( (jpegHeight* rateV / Base - tempHeight) < 0 )
				rateV+=1;
			if (rateH < rateV)
				rateH = rateV;
	
			x0 = (jpegWidth * rateH / Base - tempWidth)/2;
			if ((x0 %2) ==0)
			  x0+=1;
			x1 = x0 + tempWidth - 1;
			y0 = (jpegHeight * rateH / Base - tempHeight)/2 + 1;
			y1 = y0 + tempHeight - 1;
		}
		else		// Scale up
		{
			Base = 20;	
			rateH = tempWidth * Base / (jpegWidth-1);
			rateV = tempHeight * Base / (jpegHeight-1);

			if( ((jpegWidth-1) * rateH / Base - tempWidth) < 0 )
				rateH+=1;
			if( ((jpegHeight-1) * rateV / Base - tempHeight) < 0 )
				rateV+=1;
			if (rateH < rateV)
				rateH = rateV;

			if (rateH > 100)
			{
				rateH = 100 ;
				tempWidth = (jpegWidth-1) * rateH / Base;
				tempHeight = (jpegHeight-1) * rateV / Base ;
			}
	
			x0 = ((jpegWidth-1) * rateH / Base - tempWidth)/2;
			if ((x0 %2) ==0)
			  x0+=1;
			x1 = x0 + tempWidth - 1;
			y0 = ((jpegHeight-1) * rateH / Base - tempHeight)/2 + 1;
			y1 = y0 + tempHeight - 1;
		}
	
	}
	else		// No Scale Up/Down !!
	{
		tempWidth = *outW;
		tempHeight = *outH;
		
		if (tempWidth > jpegWidth)
			tempWidth = jpegWidth;
		
		if (tempHeight > jpegHeight)
			tempHeight = jpegHeight;
		
		rateH = 64;
		Base = 64;
		x0 = (jpegWidth - tempWidth)/2;
		if ((x0 %2) ==0)
		  x0+=1;
		x1 = x0 + tempWidth - 1;
		y0 = (jpegHeight - tempHeight)/2 + 1;
		y1 = y0 + tempHeight - 1;
	}
	
	*outW = tempWidth;
	*outH = tempHeight;
		
//	AIT_Message_P2("[A800_DecodeJpeg] tgtW:%d, tgtH:%d \n", tempWidth, tempHeight );
//	AIT_Message_P2("[A800_DecodeJpeg] M(Base):%d, N(rateH):%d \n", Base, rateH );

	if (tempHeight%2 ==1)
		y1 +=1;
	//AIT_Message_P4("[A800_DecodeJpeg] x0:%d, y0:%d, x1:%d, y1:%d \n", x0, y0, x1, y1 );

	A8S_Cameraparam.JpgWidth = tempWidth;
	A8S_Cameraparam.JpgHeight = tempHeight;
	A8S_Cameraparam.ScaleN = rateH;
	A8S_Cameraparam.ScaleM = Base;
	A8S_Cameraparam.startx = x0;
	A8S_Cameraparam.starty = y0;
	A8S_Cameraparam.endx = x1;
	A8S_Cameraparam.endy = y1;

	BufAddr = A800Data.Select->BufAddr;//0L;

   	if(CAM_LCD_REFRESH_SCALE == A800Data.Select->LcdRefreshMode)	
		retVal = A8L_DecodeJpegToYuv( jpgData, jpgLength, BufAddr, A8S_Cameraparam);	
	else
		retVal = A8L_DecodeJpegPicture( jpgData, jpgLength, BufAddr, A8S_Cameraparam);
	
	if ( retVal >= A8_JPEG_DECODE_SUCCESS )
	{
		//AIT_Message_P0( "[A800_DecodeJpeg] Decode success " ) ;
		return 2;
	}

	if (rgbBuf != NULL)
	{
		//CopyMemWordA8ToHost(rgbBuf, 0L, tempWidth*tempHeight*2);	
		if(tempWidth % 4)
			LineOffset = ((tempWidth + (4- tempWidth %4)) << 1);
		else
			LineOffset = (tempWidth << 1);

		if ( (tempWidth% 4) != 1 ) 
		{
			if ((tempWidth% 4) == 0)
			{	// 20070621, update. Memory copy is more stable!!
				CopyMemWordA8ToHost(rgbBuf, 0L, tempWidth*tempHeight*2);    
			}
			else
			{
				A8L_SetBufferAttribute(1, tempWidth, tempHeight, LineOffset, A8_DEPTH_16B, 0L);
				A8L_CopyImageBuftoFIFO(rgbBuf, tempWidth, tempHeight, 1, 0, 0);
			}
		}
		else
		{
			//BufAddr = DecodeDataBuf;
			for ( i=0; i<tempHeight; i++ ) 
			{
				CopyMemWordA8ToHost(rgbBuf, BufAddr, tempWidth <<1);	/* length in byte*/
				rgbBuf += tempWidth;
				BufAddr += (tempWidth << 1);
				if (i & 0x01)
					BufAddr += 4;
			}

		}
	}
	return A8_NO_ERROR;
}

//New AIT701 SW
u_char A800_DecodeJpegViaFIFO_SWSample(A8S_JPEG_INFO SrcJPEG, u_short* pRgbBuf, u_short OutW, u_short OutH, u_char OutFormat) 
{

    u_char RetVal = 0 ; //RET_SUCCESS

    u_short Status, Temp,Quality;
    u_int JpegByteCount, RawByteCount;
    u_int i, LengthForSend, LengthForRecv;
    s_short Width, Height, Format;
    u_int RawLength, RawWidth ;
    s_short RetErr;

    u_int nSrcLoopX,nSrcLoopY,nDstLoopX,nDstLoopY,nNextX,nNextY; 
    u_int nTimeout=0x50000,nTimeoutCount=0;
    u_int nInputTimeout=10000,nInputTimeoutCount=0;
    u_int nOutputTimeout=10000,nOutputTimeoutCount=0;
    u_int nInputMaxCount=0,nOutputMaxCount=0;
    u_short nJpegInputDone=0,nRawOutputDone=0;

    volatile u_short *pDummyData =NULL; /* prevent Chuls 080513 initial value */
    u_short nDummyData = 0; 
    u_short nDummyloop = 0;
    u_short nSkipNumX,nSkipNumY,nRemainderX,nRemainderY,nTmpRemainderX;
    u_short nSkipIncX,nSkipIncY,v_SrcSx_W,v_SrcSy_H,nSkipDecX,nSkipDecY;
    u_short nOutputFormat=1;    //RGB565
    u_short* p_JpegBuffer = SrcJPEG.dataBuf;
    u_short* pImgBuf=pRgbBuf;

	nOutputFormat = OutFormat;

    RetErr = A8L_GetJpegInfo( (u_short *)p_JpegBuffer, (s_int)SrcJPEG.fileSize, &Width, &Height, &Format,&Quality );
    if(RetErr != 0)
        return A8_JPEG_FORMAT_NOT_BASELINE;
    //==================================================================
    //Check input parameter
	if( (SrcJPEG.width==0) || ((SrcJPEG.sx+SrcJPEG.width)>(u_short)(Width)) || 
		(SrcJPEG.height==0) || ((SrcJPEG.sy+SrcJPEG.height)>(u_short)(Height)) ||
		(SrcJPEG.width<OutW) || (SrcJPEG.height<OutH)  ||
		(OutW==0) || (OutH==0))
	{
		RetVal=A8_OUT_OF_RANGE_ERROR;
        
       	//AIT_Message_P2( "Error wrong parameter,%s,%d",__FILE__,__LINE__);
		return RetVal;
	}

    SetA8RegB(0x6B24, GetA8RegB(0x6B24) | (1<<1));
    SetA8RegW(0x6020, 0x0000);      // Disable TG viewfinder mode and viewfinder data output    
    SetA8RegB( 0x690A, 0x08 );  
    SetA8RegB( 0x690A, 0x00 );  

    Temp = GetA8RegW( 0x6510 );
    SetA8RegW( 0x6510, Temp | 0x0400 ); // Reset PIP and jpeg line buffer
    SetA8RegW( 0x6510, Temp );

    SetA8RegW( 0x6210, A8_JPEG_FIFO_DECODE_ROW_BUF_ADDR & 0xFFFF );
    SetA8RegW( 0x6212, A8_JPEG_FIFO_DECODE_ROW_BUF_ADDR >> 16 );
    SetA8RegW( 0x6214, A8_JPEG_FIFO_DECODE_COMP_BUF_START_ADDR & 0xFFFF );
    SetA8RegW( 0x6216, A8_JPEG_FIFO_DECODE_COMP_BUF_START_ADDR >> 16 );
    SetA8RegW( 0x6218, A8_JPEG_FIFO_DECODE_COMP_BUF_END_ADDR & 0xFFFF );
    SetA8RegW( 0x621A, A8_JPEG_FIFO_DECODE_COMP_BUF_END_ADDR >> 16 );

    SetA8RegW( 0x6224, A8_JPEG_FIFO_DECODE_FIFO_BUF_SIZE & 0xFFFF );
    SetA8RegB( 0x6226, A8_JPEG_FIFO_DECODE_FIFO_BUF_SIZE >> 16 );

    SetA8RegW(0x6222, 0x0000);
    SetA8RegW(0x6510, 0x0004);

    if(1==nOutputFormat ) // output RGB565       
    {
        SetA8RegW(0x6B0E, 0x0112);  // Enable Graphic FIFO ( RGB565 )
    }
    else // output Yuv422
    { 
        SetA8RegW(0x6B0E, 0x0102);  // Enable Graphic FIFO ( Yuv )
    }
    
    RawLength = Width * Height * 2;
    RawWidth = Width;

    nSkipNumX=SrcJPEG.width/OutW;
    nSkipNumY=SrcJPEG.height/OutH;

    nRemainderX=SrcJPEG.width%OutW;
    nRemainderY=SrcJPEG.height%OutH;

    if(0!=nRemainderX)
    {
        nSkipIncX=OutW/nRemainderX;   
    }
    else
    {
        nSkipIncX=0;
    }
    
    if(0!=nRemainderY)
    {
        nSkipIncY=OutH/nRemainderY;
    }
    else
    {
        nSkipIncY=0;
    }

	if(1==nSkipIncX)
	{
		nSkipDecX=OutW-nRemainderX;
		nSkipDecX=OutW/nSkipDecX;
		nRemainderX=OutW-nRemainderX;
	}
	else
	{
		nSkipDecX=0;
	}
	
	if(1==nSkipIncY)
	{
		nSkipDecY=OutH-nRemainderY;
		nSkipDecY=OutH/nSkipDecY;
		nRemainderY=OutH-nRemainderY;
	}
	else
	{
		nSkipDecY=0;
	}
	
	nSrcLoopX=0;
	nSrcLoopY=0;
	nDstLoopX=0;
	nDstLoopY=0;

	nNextX=SrcJPEG.sx;
	nNextY=SrcJPEG.sy; 

	v_SrcSx_W=SrcJPEG.sx+SrcJPEG.width;
	v_SrcSy_H=SrcJPEG.sy+SrcJPEG.height;

	JpegByteCount = 0;
	RawByteCount = 0;

	SetA8RegB(0x6902, GetA8RegB(0x6902)|0x40); //enable fast jpeg decode clock
	sys_IF_ait_delay1ms(1);

	// if(Width>=800)
	//SetA8RegB(0x620A, GetA8RegB(0x620A)|0x80); //decode double buffer
	SetA8RegB(0x620A, 0x01); //Disable decode double buffer  & clear the JPEG helf size

	//SetA8RegW(0x6B2E, 0xF000);    //0x2C0); //JPEG delay counter
	SetA8RegW(0x6222, 0x0080);      // Enable JPEG FIFO Path
	SetA8RegB(0x6204, 0x0006);      // Enable JPEG Decoder

    do
    {
        //==================================================================
        //Input JPEG data via JPEG FIFO
        if(0==nJpegInputDone)
        {
            LengthForSend = (SrcJPEG.fileSize) - JpegByteCount;
            if ( LengthForSend >32 )
            {
                LengthForSend = 16;
            }
            else
            {
                LengthForSend = (LengthForSend+1) >> 1;
            }
            Status = GetA8RegB(0x6222);
            if ( Status == 0xA0 )
            {
                A8IndCmdP = HII_SET_ADDR_L;
                A8IndDataPW(0) = 0x6220;
                A8IndCmdP = HII_RW_REG_W;
                for (i=0; i<LengthForSend; i++ )
                {
                    A8IndDataPW(i) = *(p_JpegBuffer++);
                }
                JpegByteCount += (LengthForSend<<1);

                nInputTimeoutCount=0;
            }
            else
            {      
                ++nInputTimeoutCount;
                if(nInputTimeoutCount>nInputMaxCount)
                {                    
					nInputMaxCount=nInputTimeoutCount;
                }
            }       
        }
        //==================================================================

        //==================================================================
        //Output decoded raw data via Graphic FIFO
        if(0==nRawOutputDone)
        {
            LengthForRecv = GetA8RegB(0x6B27) & 0x1F;
            if(0==LengthForRecv)
            {
                ++nOutputTimeoutCount;
                if(nOutputTimeoutCount>nOutputMaxCount)
                {
                    nOutputMaxCount=nOutputTimeoutCount;
                }
            }
            while ( LengthForRecv > 0 )
            {
                A8IndCmdP = HII_SET_ADDR_L;
                A8IndDataPW(0) = 0x6B28;
                A8IndCmdP = HII_RW_REG_W;
			
                if((nSrcLoopY>=SrcJPEG.sy) && (nSrcLoopY<=(v_SrcSy_H)) && (nSrcLoopY==nNextY) && (nDstLoopY<OutH) )    //Get this line
                {
                    nNextX=SrcJPEG.sx;
                    nDstLoopX=0;
                    nTmpRemainderX=nRemainderX;
                    for (nSrcLoopX=0; nSrcLoopX < RawWidth; nSrcLoopX++ )
                    {
                        if((nSrcLoopX>=SrcJPEG.sx) && (nSrcLoopX<=v_SrcSx_W) && (nSrcLoopX==nNextX) && (nDstLoopX<OutW))  //Get this point
                        {
                            *pImgBuf++ = A8IndDataPW(i);
                            //*pImgBuf++ = GetA8RegW(0x6B28);
                            ++nDstLoopX;
							if(0!=nSkipDecX)
							{
								if((nTmpRemainderX>0) && (0==(nDstLoopX%nSkipDecX)) )
								{															
									nNextX+=nSkipNumX;
									--nTmpRemainderX;						
								}
								else
								{														
									nNextX+=(nSkipNumX+1);
								}
							}							
                            else if((nTmpRemainderX>0) && (0==(nDstLoopX%nSkipIncX)) )
                            {
                                nNextX+=(nSkipNumX+1);
                                --nTmpRemainderX;
                            }
                            else
                            {
                                nNextX+=nSkipNumX;
                            }

                        }
                        else    //Skip this point
                        {
                        pDummyData=&nDummyData;
                        (*pDummyData) = A8IndDataPW(i);
//                         (*pDummyData) = GetA8RegW(0x6b28);
                        }   
                    }

                    ++nDstLoopY;
					if(0!=nSkipDecY)
					{
						if((nRemainderY>0) && (0==(nDstLoopY%nSkipDecY)) )
						{
							nNextY+=nSkipNumY;			
							--nRemainderY;																							
						}
						else
						{
							nNextY+=(nSkipNumY+1);
						}
					}					
                    else if((nRemainderY>0) && (0==(nDstLoopY%nSkipIncY)) )
                    {
                        nNextY+=(nSkipNumY+1);
                        --nRemainderY;
                    }
                    else
                    {
                        nNextY+=nSkipNumY;
                    }

                }
                else    //Skip this line
                {
                    //20081110 Fixed non 8x8 source
                    for(nDummyloop = 0; nDummyloop < (8 - (SrcJPEG.width%8)); nDummyloop++)
                    {
                        //20081110
                        (*pDummyData) = A8IndDataPW(i); 
                        (*pDummyData) = A8IndDataPW(i);
                    }
                
                    for (nSrcLoopX=0; nSrcLoopX < RawWidth; nSrcLoopX++ )
                    {
                        pDummyData=&nDummyData;
                        (*pDummyData) = A8IndDataPW(i);                 
 //                       (*pDummyData) = GetA8RegW(0x6b28);
                    }
                }

                ++nSrcLoopY;
                RawByteCount += (RawWidth<<1);
                LengthForRecv = GetA8RegB(0x6B27) & 0x1F;
                nOutputTimeoutCount=0;
            }
        }
        //==================================================================        
        Status = GetA8RegW(0x6204);

        if(JpegByteCount >= (SrcJPEG.fileSize))
        {
            SetA8RegB(0x6223, 0x02);
            nJpegInputDone=1;
        }
        if(RawByteCount >= RawLength )
        {
            nRawOutputDone=1;
        }
        ++nTimeoutCount;
    }
	while ( ((0==nJpegInputDone) || (0==nRawOutputDone)) && 
    				(( Status & 0x02 ) != 0) && (nTimeoutCount<nTimeout) &&
    				(nInputTimeoutCount<nInputTimeout) && (nOutputTimeoutCount<nOutputTimeout) );


    MSG_FATAL("Max Input TimeOut nTimeoutCount=%d",nInputMaxCount,0,0);
    MSG_FATAL("Max Output TimeOut nTimeoutCount=%d",nOutputMaxCount,0,0);

    if(nInputTimeoutCount>=nInputTimeout)
    {
        RetVal=1;	// RET_FAIL
        MSG_FATAL("Error Input TimeOut nTimeoutCount=%d nTimeout=%d", nInputTimeoutCount, nInputTimeout,0);
    }
	
    if(nOutputTimeoutCount>=nOutputTimeout)
    {
        RetVal=1;	// RET_FAIL
        MSG_FATAL("Error Output TimeOut nTimeoutCount=%d nTimeout=%d", nOutputTimeoutCount, nOutputTimeout,0);
    }

    if(nTimeoutCount>=nTimeout)
    {
        RetVal=1;	// RET_FAIL
        MSG_FATAL("Error TimeOut nTimeoutCount=%d nTimeout=%d", nTimeoutCount, nTimeout,0);
    }
    
    MSG_FATAL("nDummyData =%d",nDummyData,0,0);
    
    SetA8RegW(0x6222, 0x0000);

	return A8_NO_ERROR;
}

/**
 @fn		u_char A800_SetPreviewLinearZoom(u_short Level)
 @brief	Setup Preview liner Zoom Step.
 @param	Level -Preview Zoom Step.
 @note	.
 @bug	N/A.
*/
u_char A800_SetPreviewLinearZoom(u_short Level)
{
	u_short	Reso;
//Vin@20091219: Select Panel Size in same lib.

	A800Data.Select = A800_ChangeCameraSetting(gsCurViewMode);
	if(!A800Data.Select )
		return A8_SYSTEM_ERROR;

	//This will sent the BB zoom step into the F.W. for capeture.
	// Technobar project is 25 step
	Reso = A800_GetCaptureResolution();

	gA8PrevZoomFactor = A800Data.Select -> ZoomStep * Level;
	AIT_Message_P1("### Zoom Step for N = %d\n", gA8PrevZoomFactor);

	A8L_ConfigPreviewZoomParam(A800Data.Select->ScaleRatio,1);
	A8L_SetCdspPreviewResolution(A800Data.SensorDes.PrevWidth, A800Data.SensorDes.PrevHeight,
																A800Data.Select->BufWidth, A800Data.Select->BufHeight,
																A800Data.Select->UpScale, A800Data.Select->DownScale,
																gA8PrevZoomFactor);

    A8L_SetZoomRate(gA8PrevZoomFactor ); //0422 for SQCIF zoom level send
    
    return A8_NO_ERROR;
/* AIT 080423 */
}

#if 0
u_short gA8AlphaRate[8] = { 8, 6, 5, 4, 3, 2, 1, 0 };


s_short A800_MergeImageWithOSD( u_short *FrameImg, u_short *OSDImg, u_short FrameKeyColor, u_int BufAddr, s_int Length )
{
   s_short OsdARate, FrameARate;
   register u_short TempO, TempF;
   u_short TempR, TempG, TempB;
   s_int i;
   
	A8IndCmdP = HII_SET_ADDR_H;
	A8IndDataPW(0) = (u_short)(BufAddr>>16);
	A8IndCmdP = HII_SET_ADDR_L;
	A8IndDataPW(0) = (u_short)(BufAddr & 0xFFFF);
	A8IndCmdP = HII_RW_MEM_W_INC;
   
   for ( i=0; i<Length; i++ )
   {

      if ( (*OSDImg & 0xE000) == 0xE000 )
      {
         TempF = *FrameImg;
         TempF = ((TempF & 0x001F)>>1) + ((TempF & 0x07c0)>>2) + ((TempF & 0xF800)>>3) + 0xE000;
      }
      else
      {
         if ( *FrameImg == FrameKeyColor )
         {
            TempF = *OSDImg;
         }
         else
         {
            OsdARate = gA8AlphaRate[ *OSDImg >> 13 ];
            FrameARate = 8 - OsdARate;
            TempO = *OSDImg;
            TempF = *FrameImg;
            TempR = (((TempO & 0x001F)>>1) * OsdARate + ((TempF & 0x001F)>>1) * FrameARate) >> 3;
            if ( TempR > 0x1F )
               TempR = 0x1F;
            TempG = (((TempO & 0x07c0)>>2) * OsdARate + ((TempF & 0x07c0)>>2) * FrameARate) >> 3;
            TempG = TempG & 0xFFF0;
            if ( TempG > 0x01F0 )
               TempG = 0x01F0;
            TempB = (((TempO & 0xF800)>>3) * OsdARate + ((TempF & 0xF800)>>3) * FrameARate) >> 3;
            TempB = TempB & 0xFE00;
            if ( TempB > 0x1E00 )
               TempB = 0x1E00;
            TempF = 0xE000 + TempR + TempG + TempB;
         }
      }

      A8IndDataPW(0) = TempF;
      OSDImg++;
      FrameImg++;
   }
   return A8_NO_ERROR;
}

#endif

/*=================================================================================*/
/******************************** These functions are prepared for BB video Record/Playback. *************************/
/*=================================================================================*/

#if 0

/**
 @fn		u_short A800_ScreenGrabCapture(u_short *mem_ptr, u_short startX, u_short startY, u_short width, u_short height)
 @brief	Grab Preview RAW data from PIP window and transfer to Host.
 @param	mem_ptr -memory pointer in host.
 @param	startX -Start X of Grab range.
 @param	startY -Start Y of Grab range.
 @param	width -Width of Grab range.
 @param	height -Height of Grab range.
 @return	u_short
 @note	Get Preview RAW data, thus Host can encode the RAW to video data.
 @bug	N/A.
*/
u_short A800_ScreenGrabCapture(u_short *mem_ptr, u_short startX, u_short startY, u_short width, u_short height)
{
  u_int source, sourceTemp;
  u_short tmpYCbCr[2];
  u_short *RGBData = mem_ptr;
  WINDOW_INFO win_infoPIP;
  s_int i, j;
  
  A8L_GetWindowAttribute(A8_PIP_WINDOW, &win_infoPIP);
  source=win_infoPIP.baseaddr + (startY * (win_infoPIP.width * 2)) + (startX * 2);
  sourceTemp = source;
  for (j = 0 ; j < height; j++)
  {
    for (i=0;i<(width >> 1);i++)
    {
      tmpYCbCr[0]=GetA8MemW(sourceTemp);
      sourceTemp+=2;
      tmpYCbCr[1]=GetA8MemW(sourceTemp);
      sourceTemp+=2;

      YCbCr2RGB(RGBData, tmpYCbCr);
      RGBData += 2;
    }
    sourceTemp = source +(j * (win_infoPIP.width * 2));
  }
  return A8_NO_ERROR;
}
#endif

/* begin of AITVES */
#if 0
/* end of AITVES */

/**
 @fn		s_short A800_EnableFrameEndHostInterrupt( s_short Polarity )
 @brief	(Enable)AIT detect Sensor FrameEnd and trigger an interrupt to Host.
 @param	Polarity -the Polarity of Host Interrupt.
 @return	s_short
 @note	AIT trigger an interrupt to HOST.
 @bug	N/A.
*/
s_short A800_EnableFrameEndHostInterrupt( s_short Polarity )
{
   SetA8RegB( 0x6041, 0x0F);
   SetA8RegB( 0x6040, GetA8RegB( 0x6040 ) | 0x01 );   // Enable Frame End Host INT
   SetA8RegB( 0x6810, GetA8RegB( 0x6810 ) | 0x01 );   // Enable Host TG INT
   SetA8RegB( 0x690C, GetA8RegB( 0x690C ) | 0x02 );   // Enable host interrup pin
   
   if ( Polarity )
   {
      SetA8RegB( 0x6800, (GetA8RegB( 0x6800 ) & 0xFD) | 0x01 );   // Enable Host INT
   }
   else
   {
      SetA8RegB( 0x6800, GetA8RegB( 0x6800 ) | 0x03 );   // Enable Host INT
   }
   return A8_NO_ERROR;
}


/**
 @fn		s_short A800_DisableFrameEndHostInterrupt()
 @brief	Disable AIT trigger an interrupt to Host.
 @return	s_short
 @note	AIT will not trigger an interrupt to HOST.
 @bug	N/A.
*/
s_short A800_DisableFrameEndHostInterrupt(void)
{
   SetA8RegB( 0x6041, 0x0F);
//   SetA8RegB( 0x690C, GetA8RegB( 0x690C ) & 0xFD );   // Disable host interrup pin
   SetA8RegB( 0x6040, GetA8RegB( 0x6040 ) & 0xFE );   // Disable Frame End Host INT
   SetA8RegB( 0x6810, GetA8RegB( 0x6810 ) & 0xFE );   // Disable Host TG INT
   SetA8RegB( 0x6800, GetA8RegB( 0x6800 ) & 0xFE );   // Disable Host INT
   return A8_NO_ERROR;
}


/**
 @fn		s_short A800_ClearFrameEndInterruptStatus()
 @brief	Clear the FrameEnd status of AIT for next trigger (interrupt) to Host.
 @return	s_short
 @note	Clear the FrameEnd status of AIT chip.
 @bug	N/A.
*/
s_short A800_ClearFrameEndInterruptStatus(void)
{
   SetA8RegB( 0x6041, 0x01 );   // Clear Frame End Host INT
    return A8_NO_ERROR;
}


/**
 @fn		s_short A800_CheckIfFrameEndInterrupt()
 @brief	Check if AIT already triggered a FrameEnd Interrupt to Host.
 @return	s_short
 @note	Check the FrameEnd Interrupt status of AIT chip.
 @bug	N/A.
*/
s_short A800_CheckIfFrameEndInterrupt(void)
{
   if ( GetA8RegB( 0x6814 ) & 0x01 )
      {
      if ( GetA8RegB( 0x6041 ) & 0x01 )
         return 1;
      else
         return A8_NO_ERROR;
      }
   else
      return A8_NO_ERROR;
}


#else
/**
 @fn		s_short A800_EnableTGToHostInterrupt(u_char SignalType, u_short Polarity )
 @brief	(Enable)AIT detect TG status and trigger an interrupt to Host.
 @param	cSignalType - Signal type of interrupt.
 @param	cPolarity -the Polarity of Host Interrupt.
 @return	u_short
 @note	AIT trigger an interrupt to HOST.
 @bug	N/A.
*/
u_short A800_EnableTGToHostInterrupt(u_char cSignalType, u_short cPolarity )
{
	SetA8RegB( 0x6041, cSignalType);	//Clear interrupt staus
	SetA8RegB( 0x6040, GetA8RegB( 0x6040 ) | cSignalType );   // Enable Frame End Host INT
	SetA8RegB( 0x6810, GetA8RegB( 0x6810 ) | 0x01 );   // Enable Host TG INT
	SetA8RegB( 0x690C, GetA8RegB( 0x690C ) | 0x02 );   // Enable host interrup pin

	if ( cPolarity )
	{
			SetA8RegB( 0x6800, (GetA8RegB( 0x6800 ) & 0xFD) | 0x01 );   // Enable Host INT => Interrupt polarity is Active High
	}
	else
	{
		SetA8RegB( 0x6800, GetA8RegB( 0x6800 ) | 0x03 );   // Enable Host INT
	}
	return A8_NO_ERROR;
}


/**
 @fn		u_short A800_DisableTGToHostInterrupt(u_char cSignalType)
 @brief	Disable AIT trigger an interrupt to Host.
 @param	cSignalType - Signal type of interrupt.
 @return	u_short
 @note	AIT will not trigger an interrupt to HOST.
 @bug	N/A.
*/
u_short A800_DisableTGToHostInterrupt(u_char cSignalType)
{
	SetA8RegB( 0x6041, cSignalType);
	SetA8RegB( 0x6040, GetA8RegB( 0x6040 ) & ~(cSignalType) );   // Disable TG Host INT
	/*if( 0x0 == GetA8RegB(0x6040) )	//All TG INT is diable
	{
		//   SetA8RegB( 0x690C, GetA8RegB( 0x690C ) & 0xFD );   // Disable host interrup pin
		SetA8RegB( 0x6810, GetA8RegB( 0x6810 ) & 0xFE );   // Disable Host TG INT
		SetA8RegB( 0x6800, GetA8RegB( 0x6800 ) & 0xFE );   // Disable Host INT
	}*/
	return A8_NO_ERROR;   
}


/**
 @fn		u_short A800_ClearTGToHostInterruptStatus(u_char cSignalType)
 @brief	Clear the TG status of AIT for next trigger (interrupt) to Host.
 @return	u_short
 @note	Clear the FrameEnd status of AIT chip.
 @bug	N/A.
*/
u_short A800_ClearTGToHostInterruptStatus(u_char cSignalType)
{
	SetA8RegB( 0x6041, cSignalType );   // Clear TG INT status
	return A8_NO_ERROR;  
}


/**
 @fn		u_short A800_CheckTGToHostInterruptStatus(u_char cSignalType)
 @brief	Check if AIT already triggered a TG Interrupt to Host.
 @return	u_short
 @note	Check the TG Interrupt status of AIT chip.
 @bug	N/A.
*/
u_short A800_CheckTGToHostInterruptStatus(u_char cSignalType)
{
	if ( GetA8RegB( 0x6814 ) & 0x01 )
	{
		if ( GetA8RegB( 0x6041 ) & cSignalType )	// Check TG INT status
			return 1;
		else
			return A8_NO_ERROR;
	}
	return A8_NO_ERROR;
}
#endif

s_short A800_EnableCPUToHostInterrupt( s_short Polarity )
{
   SetA8RegB( 0x6810, GetA8RegB( 0x6810 ) | 0x80 );   // Enable cpu to host interrupt, active high
   SetA8RegB( 0x690C, GetA8RegB( 0x690C ) | 0x02 );   // Enable host interrup pin, Set to Low
   
   if ( Polarity )  // if Polarity is "1" then polarity active low and default high.
   {
      SetA8RegB( 0x6800, (GetA8RegB( 0x6800 ) & 0xFD) | 0x01 );   // Enable Host INT, Active High
   }
   else
   {
      SetA8RegB( 0x6800, GetA8RegB( 0x6800 ) | 0x03 );   // Enable Host INT, Active Low
   }

   return A8_NO_ERROR;
}

s_short A800_DisableCPUToHostInterrupt(void)
{
   SetA8RegB( 0x6800, GetA8RegB(0x6800) | 0x80 );   // Clear CPU 2 Host INT
   SetA8RegB( 0x6810, GetA8RegB( 0x6810 ) & 0x7F );   // Disable CPU2Host INT
   SetA8RegB( 0x6800, GetA8RegB( 0x6800 ) & 0xFE );   // Disable Host INT

   return A8_NO_ERROR;
}

s_short A800_ClearCPUToHostInterruptStatus(void)
{
   SetA8RegB( 0x6800, GetA8RegB(0x6800) | 0x80 );   // Clear CPU 2 Host INT

   return A8_NO_ERROR;
}

/**
 @fn		s_short A800_CopyPreviewWindowBufferToHost( u_short *FrameBuf )
 @brief	Copy whole AIT Preview Window data to Host.
 @param	FrameBuf -the buffer pointer of Host.
 @return	s_short
 @note	.
 @bug	N/A.
*/
s_short A800_CopyPreviewWindowBufferToHost( u_short *FrameBuf )
{
  WINDOW_INFO PIPInfo;
  u_int Length;
  
  A8L_GetWindowAttribute(A8_PIP_WINDOW, &PIPInfo);
  Length = PIPInfo.width * PIPInfo.height*2;
  CopyMemWordA8ToHost( FrameBuf, PIPInfo.baseaddr, Length );
  return A8_NO_ERROR;
}

/**
 @fn		s_short A800_CopyHostBufferToWindowBuffer( u_short *HostFrameBuf )
 @brief	Copy Host Data to AIT Preview Window.
 @param	HostFrameBuf -the buffer pointer of Host.
 @return	s_short
 @note	.
 @bug	N/A.
*/
s_short A800_CopyHostBufferToWindowBuffer( u_short *HostFrameBuf )
{
  WINDOW_INFO PIPInfo;
  u_int Length;
  
  A8L_GetWindowAttribute(A8_PIP_WINDOW, &PIPInfo);
  Length = PIPInfo.width * PIPInfo.height*2;
  CopyMemWordHostToA8( PIPInfo.baseaddr, HostFrameBuf, Length );
  return A8_NO_ERROR;
}


/*=============================================================*/

u_short A800_SetGPIOMode(u_short gpio, eAIT_GPIO_MODE PullUpDown)
{

	//u_short GPIO_MODE_Reg = 0x69D9;


	switch(gpio)
	{
		case 6:
			 SetA8RegB(0x692A, GetA8RegB(0x692A)&(~0x20) ); //disable uart mode, set to gpio mode, gpio 6 control sub-sensor enable

			break;
		case 18:
			SetA8RegB(0x690C, GetA8RegB(0x690C)&(~0x04) );
			SetA8RegB(0x6920,GetA8RegB(0x6920)|0x10);	// GPIO Mode
			break;

		case 11:
			break;

	}

	if(AIT_GPIO_PULL_UP == PullUpDown){
		if(gpio<7){
			SetA8RegB(0x69D9+gpio,(GetA8RegB(0x69D9+gpio)&(~0x6))|0x4);
		}else if(gpio<14){
			SetA8RegB(0x69B8+gpio-7,(GetA8RegB(0x69B8+gpio-7)&(~0x6))|0x4);		
		}else if(gpio<16){
			SetA8RegB(0x69D6+gpio-14,(GetA8RegB(0x69D6+gpio-14)&(~0x6))|0x4);				
		}else if(gpio<19){
			SetA8RegB(0x69C7+gpio-16,(GetA8RegB(0x69C7+gpio-16)&(~0x6))|0x4);				
		}else if(gpio<21){
			SetA8RegB(0x69CF+gpio-19,(GetA8RegB(0x69CF+gpio-19)&(~0x6))|0x4);				
		}else if(gpio==21){
			SetA8RegB(0x69D4,(GetA8RegB(0x69D4)&(~0x6))|0x4);				
		}else if(gpio==22){
			SetA8RegB(0x69CC,(GetA8RegB(0x69CC)&(~0x6))|0x4);				
		}
	}else if(AIT_GPIO_PULL_DOWN == PullUpDown){
		if(gpio<7){
			SetA8RegB(0x69D9+gpio,(GetA8RegB(0x69D9)&(~0x6))|0x2);	
		}else if(gpio<14){
			SetA8RegB(0x69B8+gpio-7,(GetA8RegB(0x69B8+gpio-7)&(~0x6))|0x2);		
		}else if(gpio<16){
			SetA8RegB(0x69D6+gpio-14,(GetA8RegB(0x69D6+gpio-14)&(~0x6))|0x2);				
		}else if(gpio<19){
			SetA8RegB(0x69C7+gpio-16,(GetA8RegB(0x69C7+gpio-16)&(~0x6))|0x2);				
		}else if(gpio<21){
			SetA8RegB(0x69CF+gpio-19,(GetA8RegB(0x69CF+gpio-19)&(~0x6))|0x2);				
		}else if(gpio==21){
			SetA8RegB(0x69D4,(GetA8RegB(0x69D4)&(~0x6))|0x2);				
		}else if(gpio==22){
			SetA8RegB(0x69CC,(GetA8RegB(0x69CC)&(~0x6))|0x2);				
		}	
	}else if(AIT_GPIO_PULL_FLOATING == PullUpDown){
		if(gpio<7){
			SetA8RegB(0x69D9+gpio,(GetA8RegB(0x69D9)&(~0x6)));	
		}else if(gpio<14){
			SetA8RegB(0x69B8+gpio-7,(GetA8RegB(0x69B8+gpio-7)&(~0x6)));		
		}else if(gpio<16){
			SetA8RegB(0x69D6+gpio-14,(GetA8RegB(0x69D6+gpio-14)&(~0x6)));				
		}else if(gpio<19){
			SetA8RegB(0x69C7+gpio-16,(GetA8RegB(0x69C7+gpio-16)&(~0x6)));				
		}else if(gpio<21){
			SetA8RegB(0x69CF+gpio-19,(GetA8RegB(0x69CF+gpio-19)&(~0x6)));				
		}else if(gpio==21){
			SetA8RegB(0x69D4,(GetA8RegB(0x69D4)&(~0x6)));				
		}else if(gpio==22){
			SetA8RegB(0x69CC,(GetA8RegB(0x69CC)&(~0x6)));				
		}	
	}
		
	if(gpio<16){
		SetA8RegW(0x6600, GetA8RegW(0x6600)&(~(1<<gpio)));
		if((GetA8RegW(0x6610)>>gpio) & 1)
			return 1;
		else
			return 0;		
	}else if(gpio<23){
		gpio -= 0x10;
		SetA8RegB(0x6602, GetA8RegB(0x6602)&(~(1<<gpio)));	
		if ((GetA8RegB(0x6612)>>gpio)&1)
			return 1;
		else
			return 0;		
	}else{
		return	A8_OUT_OF_RANGE_ERROR;
	}
}

u_short A800_GetGPIO(u_short gpio, u_short PullUpDown)
{
	if(AIT_GPIO_PULL_UP == PullUpDown){
		if(gpio<7){
			SetA8RegB(0x69D9+gpio,(GetA8RegB(0x69D9+gpio)&(~0x6))|0x4);
		}else if(gpio<14){
			SetA8RegB(0x69B8+gpio-7,(GetA8RegB(0x69B8+gpio-7)&(~0x6))|0x4);		
		}else if(gpio<16){
			SetA8RegB(0x69D6+gpio-14,(GetA8RegB(0x69D6+gpio-14)&(~0x6))|0x4);				
		}else if(gpio<19){
			SetA8RegB(0x69C7+gpio-16,(GetA8RegB(0x69C7+gpio-16)&(~0x6))|0x4);				
		}else if(gpio<21){
			SetA8RegB(0x69CF+gpio-19,(GetA8RegB(0x69CF+gpio-19)&(~0x6))|0x4);				
		}else if(gpio==21){
			SetA8RegB(0x69D4,(GetA8RegB(0x69D4)&(~0x6))|0x4);				
		}else if(gpio==22){
			SetA8RegB(0x69CC,(GetA8RegB(0x69CC)&(~0x6))|0x4);				
		}
	}else if(AIT_GPIO_PULL_DOWN == PullUpDown){
		if(gpio<7){
			SetA8RegB(0x69D9+gpio,(GetA8RegB(0x69D9)&(~0x6))|0x2);	
		}else if(gpio<14){
			SetA8RegB(0x69B8+gpio-7,(GetA8RegB(0x69B8+gpio-7)&(~0x6))|0x2);		
		}else if(gpio<16){
			SetA8RegB(0x69D6+gpio-14,(GetA8RegB(0x69D6+gpio-14)&(~0x6))|0x2);				
		}else if(gpio<19){
			SetA8RegB(0x69C7+gpio-16,(GetA8RegB(0x69C7+gpio-16)&(~0x6))|0x2);				
		}else if(gpio<21){
			SetA8RegB(0x69CF+gpio-19,(GetA8RegB(0x69CF+gpio-19)&(~0x6))|0x2);				
		}else if(gpio==21){
			SetA8RegB(0x69D4,(GetA8RegB(0x69D4)&(~0x6))|0x2);				
		}else if(gpio==22){
			SetA8RegB(0x69CC,(GetA8RegB(0x69CC)&(~0x6))|0x2);				
		}	
	}else if(AIT_GPIO_PULL_FLOATING == PullUpDown){
		if(gpio<7){
			SetA8RegB(0x69D9+gpio,(GetA8RegB(0x69D9)&(~0x6)));	
		}else if(gpio<14){
			SetA8RegB(0x69B8+gpio-7,(GetA8RegB(0x69B8+gpio-7)&(~0x6)));		
		}else if(gpio<16){
			SetA8RegB(0x69D6+gpio-14,(GetA8RegB(0x69D6+gpio-14)&(~0x6)));				
		}else if(gpio<19){
			SetA8RegB(0x69C7+gpio-16,(GetA8RegB(0x69C7+gpio-16)&(~0x6)));				
		}else if(gpio<21){
			SetA8RegB(0x69CF+gpio-19,(GetA8RegB(0x69CF+gpio-19)&(~0x6)));				
		}else if(gpio==21){
			SetA8RegB(0x69D4,(GetA8RegB(0x69D4)&(~0x6)));				
		}else if(gpio==22){
			SetA8RegB(0x69CC,(GetA8RegB(0x69CC)&(~0x6)));				
		}	
	}
		
	if(gpio<16){
		SetA8RegW(0x6600, GetA8RegW(0x6600)&(~(1<<gpio)));
		if((GetA8RegW(0x6610)>>gpio) & 1)
			return 1;
		else
			return 0;		
	}else if(gpio<23){
		gpio -= 0x10;
		SetA8RegB(0x6602, GetA8RegB(0x6602)&(~(1<<gpio)));	
		if ((GetA8RegB(0x6612)>>gpio)&1)
			return 1;
		else
			return 0;		
	}else{
		return	A8_OUT_OF_RANGE_ERROR;
	}
}
/**
 @fn		u_short A800_SetGPIO(u_short gpio)
 @brief	This function is for Set AIT GPIO as Out direction and High.
 @param	gpio -the GPIO pin of AIT.
 @return	u_short - 0:success, 1:Fail
 @note	Set GPIO to High.
 @bug	N/A.
*/
u_short A800_SetGPIO(u_short gpio)
{
	u_short GPIO_MODE_Reg = 0x69D9;

//Set Multifunction PIN mode
	switch(gpio)
	{
		case 6:
			 SetA8RegB(0x692A, GetA8RegB(0x692A)&(~0x20) ); //disable uart mode, set to gpio mode, gpio 6 control sub-sensor enable
			break;
		case 18:
				SetA8RegB(0x690C, GetA8RegB(0x690C)&(~0x04) );
				SetA8RegB(0x6920,GetA8RegB(0x6920)|0x10);
			break;
	}
				
	//Set GPIO to floating mode
	if(gpio<7){
		GPIO_MODE_Reg = 0x69D9+gpio;
	}else if(gpio<14){
		GPIO_MODE_Reg = 0x69B8+gpio-7;
	}else if(gpio<16){
		GPIO_MODE_Reg = 0x69D6+gpio-14;
	}else if(gpio<19){
		GPIO_MODE_Reg = 0x69C7+gpio-16;
	}else if(gpio<21){
		GPIO_MODE_Reg = 0x69CF+gpio-19;
	}else if(gpio==21){
		GPIO_MODE_Reg = 0x69D4;
	}else if(gpio==22){
		GPIO_MODE_Reg = 0x69CC;
	}

    //SetA8RegB(GPIO_MODE_Reg,(GetA8RegB(GPIO_MODE_Reg)|0x06));
    SetA8RegB(GPIO_MODE_Reg,(GetA8RegB(GPIO_MODE_Reg)&(~0x6)));	//AIT_GPIO_PULL_FLOATING


// Set GPIO output High
	if(gpio<16){
		SetA8RegW(0x6600, GetA8RegW(0x6600)|(1<<gpio));
		SetA8RegW(0x6610, GetA8RegW(0x6610)|(1<<gpio));		
		if ((GetA8RegW(0x6610)>>gpio)&1)
			return A8_NO_ERROR;
		else
			return 1;		
	}else if(gpio<23){
		gpio -= 0x10;
		SetA8RegB(0x6602, GetA8RegB(0x6602)|(1<<gpio));
		SetA8RegB(0x6612, GetA8RegB(0x6612)|(1<<gpio));		
		if ((GetA8RegB(0x6612)>>gpio)&1)
			return A8_NO_ERROR;
		else
			return 1;		
	}else{
		return	A8_OUT_OF_RANGE_ERROR;
	}
}


/**
 @fn		u_short A800_SetGPIO(u_short gpio)
 @brief	This function is for Set AIT GPIO as Out direction and Low.
 @param	gpio -the GPIO pin of AIT.
 @return	u_short - 0:success, 1:Fail
 @note	Set GPIO to Low.
 @bug	N/A.
*/
u_short A800_ClearGPIO(u_short gpio)
{
	u_short GPIO_MODE_Reg = 0x69D9;

//Set Multifunction PIN mode
	switch(gpio)
	{
		case 6:
			 SetA8RegB(0x692A, GetA8RegB(0x692A)&(~0x20) ); //disable uart mode, set to gpio mode, gpio 6 control sub-sensor enable
			break;
		case 18:
				SetA8RegB(0x690C, GetA8RegB(0x690C)&(~0x04) );
		SetA8RegB(0x6920,GetA8RegB(0x6920)|0x10);
			break;
			}
	
	//Set GPIO to floating mode
	if(gpio<7){
		GPIO_MODE_Reg = 0x69D9+gpio;
	}else if(gpio<14){
		GPIO_MODE_Reg = 0x69B8+gpio-7;
	}else if(gpio<16){
		GPIO_MODE_Reg = 0x69D6+gpio-14;
	}else if(gpio<19){
		GPIO_MODE_Reg = 0x69C7+gpio-16;
	}else if(gpio<21){
		GPIO_MODE_Reg = 0x69CF+gpio-19;
	}else if(gpio==21){
		GPIO_MODE_Reg = 0x69D4;
	}else if(gpio==22){
		GPIO_MODE_Reg = 0x69CC;
	}

    //SetA8RegB(GPIO_MODE_Reg,(GetA8RegB(GPIO_MODE_Reg)|0x06));
    SetA8RegB(GPIO_MODE_Reg,(GetA8RegB(GPIO_MODE_Reg)&(~0x6)));	//AIT_GPIO_PULL_FLOATING
	
	if (gpio<16)
	{
		SetA8RegW(0x6600, GetA8RegW(0x6600)|(1<<gpio));
		SetA8RegW(0x6610, GetA8RegW(0x6610)&(~(1<<gpio)));
		if ((GetA8RegW(0x6610)>>gpio)&1)
			return 1;
		else
			return A8_NO_ERROR;		
	}
	else if(gpio<23)
	{
		gpio -= 0x10;
		SetA8RegB(0x6602, GetA8RegB(0x6602)|(1<<gpio));
		SetA8RegB(0x6612, GetA8RegB(0x6612)&(~(1<<gpio)));	
		if ((GetA8RegB(0x6612)>>gpio)&1)
			return 1;
		else
			return A8_NO_ERROR;		
	}else{
		return	A8_OUT_OF_RANGE_ERROR;
	}
}

void A800_LCDScaler(u_short InWidth, u_short InHeight, u_short OutWidth, u_short OutHeight)
{
	u_short tempWidth, tempHeight, rateV, rateH, Base;
	u_int lineBuffer1 = 0;//glsdDmaAddr - (OutWidth*2)*2;
	u_int lineBuffer0 = OutWidth*2*2;//lineBuffer1 - (OutWidth*2)*2;
	u_short	DispLcdX,DispLcdY;

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
		Base = 64;//32;
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

	AIT_Message_P1("Base = %d\r\n", Base);
	AIT_Message_P1("rateH = %d\r\n", rateH);
	AIT_Message_P1("tempWidth = %d\r\n", tempWidth);
	AIT_Message_P1("tempHeight = %d\r\n", tempHeight);
	
	// LCD Scaler In Width
	SetA8RegW(0x4840, InWidth);
	// LCD Scaler In Height
	SetA8RegW(0x4842, InHeight);
	// LCD Scaler Out Width
	SetA8RegW(0x484C, tempWidth);
	// LCD Scaler Out Height
	SetA8RegW(0x484E, tempHeight);
	
	// LCD Scaler N ratio
	SetA8RegW(0x4850, rateH);
	// LCD Scaler M ratio
	SetA8RegW(0x4852, Base);

	// YUV422, buffer row offset for Grab function, count by "Byte".
	SetA8RegW(0x4810, InWidth << 1);

	// YUV422, buffer Start offset, count by "Byte".
	SetA8RegW(0x5084, 0x0000);//0x0004);
	SetA8RegW(0x5086, 0x0000);

	// YUV422, pixel_offset and row_offset for Grab 
	SetA8RegW(0x507A, 0x0002);	// Pixel offset, 2 byte
	SetA8RegW(0x507C, InWidth << 1);	// Row offset, 160x2 = 320 byte
	
	// PIP display Width and Height
	SetA8RegW(0x507E, OutWidth);
	SetA8RegW(0x5080, OutHeight);
	
	// PIP on LCD position (x,y)
	SetA8RegW(0x5076, DispLcdX);
	SetA8RegW(0x5078, DispLcdY);

	// Flow_Controller, Scaler output to PIP buffer(Addr).
	// PingPong Buf_0 and Buf_1 for Y,U,V (base on PIP window)
	SetA8RegW(0x6508, A800Data.Select -> BufAddr);
	SetA8RegW(0x650A, A800Data.Select -> BufAddr >> 16);

	SetA8RegW(0x650C, A800Data.Select -> BufAddr);
	SetA8RegW(0x650E, A800Data.Select -> BufAddr >> 16);

	// PIP window for YUV422 buffer.
	// PIP_O and PIP_1 buffer for YUV422
	SetA8RegW(0x5088, A800Data.Select -> BufAddr);
	SetA8RegW(0x508A, A800Data.Select -> BufAddr>>16);

	SetA8RegW(0x508C, A800Data.Select -> BufAddr);
	SetA8RegW(0x508E, A800Data.Select -> BufAddr>>16);

	// LCD Scaler WR Frame Buffer_0.
	// ###### The size is related to LCD ScalerOut Width ######
	// From FrameRam 95KB,0x17880 position.  
	SetA8RegW(0x4838, lineBuffer0 & 0xFFFF);
	SetA8RegW(0x483A, lineBuffer0 >> 16);

	// LCD Scaler WR Frame Buffer_1.
	SetA8RegW(0x483C, lineBuffer1 & 0xFFFF);
	SetA8RegW(0x483E, lineBuffer1 >> 16);

	// LCD Scaler WR Frame Buffer length, count by "pixel" !!
	SetA8RegW(0x4844, OutWidth);
	SetA8RegW(0x4846, 0x0000);

	// LCD Scaler WR Frame FIFO Threshold
	SetA8RegB(0x4848, 0x0C);
	
	// LCD Scaler Enable, Reg_504A[6]+Reg_504B[3]
	SetA8RegB(0x504A,  GetA8RegB(0x504A) | 0x40);
	SetA8RegB(0x504B,  GetA8RegB(0x504B) & 0xF7);

	// LCD Scaler Grab function enable. Reg_504B[4]
	SetA8RegB(0x504B,  GetA8RegB(0x504B) | 0x10);
	// LCD Scaler YUV422 format. Reg_504B[6]
	SetA8RegB(0x504B,  GetA8RegB(0x504B) | 0x40);

	// YUV422(H263) mode off Reg_5042[0], PIP bpp is 16bit mode
	// YUV422(H263) mode off
	SetA8RegB(0x5042,  GetA8RegB(0x5042) & 0xFE);

	// PIP is 16bit mode, Reg_5040[1] 16bit
	SetA8RegB(0x5040, GetA8RegB(0x5040) & 0xFD);
	
	// 20080613, LCD delay line
	SetA8RegB(0x50F0, 0x00);
}

#if 0
//----------------------------------------------------------------------------
/**
 @fn		u_short TranslateYUV420_to_YUV422(u_short width, u_short height, u_char *pY_color, u_char *pCb_color, u_char *pCr_color, u_char *yuvBuf)
 @brief	Translate YUV420 Data to YUV422 format.
 @param	width -width of image.
 @param	height -height of image.
 @param	pY_color -the buffer pointer of Y in Host.
 @param	pCb_color -the buffer pointer of Cb in Host.
 @param	pCr_color -the buffer pointer of Cr in Host.
 @param	yuvBuf -the buffer pointer of YUV in Host.
 @return	u_short
 @note	.
 @bug	N/A.
*/
u_short	TranslateYUV420_to_YUV422(u_short width, u_short height, u_char *pY_color, u_char *pCb_color, u_char *pCr_color, u_char *yuvBuf)
{
	u_char	*charptr,*yptr, *cbptr, *crptr;
	u_short *shortptr;
	u_int	i, j;
	
	shortptr = (u_short*)yuvBuf;
	
	for(j=0;j<height;j++)
	{
		for(i=0;i<width;i+=2)
		{
			*shortptr++  = ((u_short)(*pY_color++) << 8) | (u_short)(*pCb_color++);
			*shortptr++  = ((u_short)(*pY_color++) << 8) | (u_short)(*pCr_color++);
		}

		if(!(j&0x1))	// if at even rows , Cb and Cr return to last row again.
		{
			pCb_color -= width/2;
			pCr_color -= width/2;
		}
	}
	return A8_NO_ERROR;
}


/**
 @fn		u_short TranslateYUV422_to_YUV420(u_short width, u_short height, u_char *pY_color, u_char *pCb_color, u_char *pCr_color, u_char *yuvBuf)
 @brief	Translate YUV422 Data to YUV420 format.
 @param	width -width of image.
 @param	height -height of image.
 @param	pY_color -the buffer pointer of Y in Host.
 @param	pCb_color -the buffer pointer of Cb in Host.
 @param	pCr_color -the buffer pointer of Cr in Host.
 @param	yuvBuf -the buffer pointer of YUV in Host.
 @return	u_short
 @note	.
 @bug	N/A.
*/
u_short	TranslateYUV422_to_YUV420(u_short width, u_short height, u_char *pY_color, u_char *pCb_color, u_char *pCr_color, u_char *yuvBuf)
{
	u_char	*charptr,*yptr, *cbptr, *crptr, temp;
	u_short *shortptr, temp_short;
	u_int	i, j;
	
	shortptr = (u_short*)yuvBuf;
	yptr = pY_color;
	cbptr = pCb_color;
	crptr = pCr_color;	
	
	for(j=0;j<height;j++)
	{
		for(i=0;i<width;i+=2)
		{
			temp_short = (*shortptr);
			*yptr++  = (u_char)(temp_short >> 8);
			if(!(j&0x1))	// if at even rows
			{
				*cbptr++ = (u_char)temp_short;
			}
			*shortptr++;
			
			temp_short = (*shortptr);
			*yptr++  = (u_char)(temp_short >> 8);
			if(!(j&0x1))	// if at even rows
			{
				*crptr++ = (u_char)temp_short;
			}
			*shortptr++;
		}
	}
	return A8_NO_ERROR;
}


/**
 @fn		u_short	YUV422_to_YUV420(u_short width, u_short height, u_char *yuv422Buf, u_char* yuv420Buf)
 @brief	Translate YUV422 Data to YUV420 format.
 @param	width -width of image.
 @param	height -height of image.
 @param	yuv422Buf -the buffer pointer of YUV422 in Host.
 @param	yuv420Buf -the buffer pointer of YUV420 in Host.
 @return	u_short
 @note	.
 @bug	N/A.
*/
u_short	YUV422_to_YUV420(u_short width, u_short height, u_char *yuv422Buf, u_char* yuv420Buf)
{
	u_char	temp;
	u_short *shortptr_422, *shortptr_420, temp_short;
	u_int	i, j;

	shortptr_422 = (u_short*)yuv422Buf;
	shortptr_420 = (u_short*)yuv420Buf;
	
	for(j=0;j<height;j++)
	{
		for(i=0;i<width;i+=2)
		{
			if(!(j&0x1))	// if at even rows
			{
				temp_short = (*shortptr_422);
				*shortptr_420  = temp_short;
				shortptr_422++;
				shortptr_420++;
				
				temp_short = (*shortptr_422);
				*shortptr_420  = temp_short;
				shortptr_422++;
				shortptr_420++;
			}
			else
			{
				temp_short = (*shortptr_422);
				temp  = (temp_short >> 8);
				shortptr_422++;
				
				temp_short = (*shortptr_422);
				temp_short = (temp_short&0xFF00) + temp;
				*shortptr_420  = temp_short;
				shortptr_422++;
				
				shortptr_420++;
			}	
			
		}
	}
	return A8_NO_ERROR;
}

#endif
#if 0	//removed by ming @20090407
u_char A800_GetColor(A8S_IMAGE *Image, u_short x, u_short y, u_short channel)
{
	s_short temp=0; /* prevent Chuls 080513 initial value */
	u_char value;
	
	if (x >= (Image->Width)) 
		temp = 0;
	if (y >= Image->Height) 
		temp = 0;

	switch (channel) {
		case AITC_Y_CH:
#if defined (AITD_YCbYCr_)
				temp = *((u_short *)Image->ImageAddr+((y*(Image->Width)+x) << 1));
#elif defined (AITD_YCrYCb_)
				temp = *((u_short *)Image->ImageAddr+((y*(Image->Width)+x) << 1));
#elif defined (AITD_CbYCrY_)
				temp = *((u_short *)Image->ImageAddr+((y*(Image->Width)+x) << 1)+1);
#elif defined (AITD_CrYCbY_)
				temp = *((u_short *)Image->ImageAddr+((y*(Image->Width)+x) << 1)+1);
#endif
			break;
		case AITC_Cb_CH:
#if defined (AITD_YCbYCr_)
				if ( (x & 0x01) == 0 )
					temp = *((u_short *)Image->ImageAddr+((y*(Image->Width)+x) << 1)+1);
#elif defined (AITD_YCrYCb_)
				if ( (x & 0x01) == 1 )
					temp = *((u_short *)Image->ImageAddr+((y*(Image->Width)+x) << 1)+1);
#elif defined (AITD_CbYCrY_)
				if ( (x & 0x01) == 0 )
					temp = *((u_short *)Image->ImageAddr+((y*(Image->Width)+x) << 1));
#elif defined (AITD_CrYCbY_)
				if ( (x & 0x01) == 1 )
					temp = *((u_short *)Image->ImageAddr+((y*(Image->Width)+x) << 1));
#endif
			break;
		case AITC_Cr_CH:
#if defined (AITD_YCbYCr_)
				if ( (x & 0x01) == 1 )
					temp = *((u_short *)Image->ImageAddr+((y*(Image->Width)+x) << 1)+1);
#elif defined (AITD_YCrYCb_)
				if ( (x & 0x01) == 0 )
					temp = *((u_short *)Image->ImageAddr+((y*(Image->Width)+x) << 1)+1);
#elif defined (AITD_CbYCrY_)
				if ( (x & 0x01) == 1 )
					temp = *((u_short *)Image->ImageAddr+((y*(Image->Width)+x) << 1));
#elif defined (AITD_CrYCbY_)
				if ( (x & 0x01) == 0 )
					temp = *((u_short *)Image->ImageAddr+((y*(Image->Width)+x) << 1));
#endif
			break;
	}

	if (temp > 255) 
		value = 255;
	if (temp < 0) 
		value = 0;

	value = (u_char)temp;
	
	return value;
}


s_short A800_SetColor(A8S_IMAGE *Image, u_short x, u_short y, u_short channel, s_short value)
{
	if (value >= 255) 
		value = 255;
	if (value <= 0) 
		value = 0;
	
	if (x >= (Image->Width)) 
		return A8_OUT_OF_RANGE_ERROR;
	if (y > Image->Height)
		return A8_OUT_OF_RANGE_ERROR;	
	
	switch (channel) {
		case AITC_Y_CH:
#if defined (AITD_YCbYCr_)
				*((u_short *)Image->ImageAddr+((y*(Image->Width)+x) << 1)) = (u_char) value;
#elif defined (AITD_YCrYCb_)
				*((u_short *)Image->ImageAddr+((y*(Image->Width)+x) << 1)) = (u_char) value;
#elif defined (AITD_CbYCrY_)
				*((u_short *)Image->ImageAddr+((y*(Image->Width)+x) << 1)+1) = (u_char) value;
#elif defined (AITD_CrYCbY_)
				*((u_short *)Image->ImageAddr+((y*(Image->Width)+x) << 1)+1) = (u_char) value;
#endif
			break;
		case AITC_Cb_CH:
#if defined (AITD_YCbYCr_)
				if ( (x & 0x01) == 0 )
					*((u_short *)Image->ImageAddr+((y*(Image->Width)+x) << 1)+1) = (u_char) value;
#elif defined (AITD_YCrYCb_)
				if ( (x & 0x01) == 1 )
					*((u_short *)Image->ImageAddr+((y*(Image->Width)+x) << 1)+1) = (u_char) value;
#elif defined (AITD_CbYCrY_)
				if ( (x & 0x01) == 0 )
					*((u_short *)Image->ImageAddr+((y*(Image->Width)+x) << 1)) = (u_char) value;
#elif defined (AITD_CrYCbY_)
				if ( (x & 0x01) == 1 )
					*((u_short *)Image->ImageAddr+((y*(Image->Width)+x) << 1)) = (u_char) value;
#endif
			break;
		case AITC_Cr_CH:
#if defined (AITD_YCbYCr_)
				if ( (x & 0x01) == 1 )
					*((u_short *)Image->ImageAddr+((y*(Image->Width)+x) << 1)+1) = (u_char) value;
#elif defined (AITD_YCrYCb_)
				if ( (x & 0x01) == 0 )
					*((u_short *)Image->ImageAddr+((y*(Image->Width)+x) << 1)+1) = (u_char) value;
#elif defined (AITD_CbYCrY_)
				if ( (x & 0x01) == 1 )
					*((u_short *)Image->ImageAddr+((y*(Image->Width)+x) << 1)) = (u_char) value;
#elif defined (AITD_CrYCbY_)
				if ( (x & 0x01) == 0 )
					*((u_short *)Image->ImageAddr+((y*(Image->Width)+x) << 1)) = (u_char) value;
#endif
			break;
	}

	return A8_NO_ERROR;
}


s_short A800_SetFrameColor(A8S_IMAGE *Image, u_short channel, s_short value)
{
	s_int FrameSize;
	s_int offset=0, inc=0; /* prevent Chuls 080513 initial value */

	FrameSize = Image->Width* Image->Height<< 1;

	if (value >= 255) 
		value = 255;
	if (value <= 0) 
		value = 0;
	
	switch (channel) {
		case AITC_Y_CH:
#if defined (AITD_YCbYCr_)
			offset = 0;
#elif defined (AITD_YCrYCb_)
			offset = 0;
#elif defined (AITD_CbYCrY_)
			offset = 1;
#elif defined (AITD_CrYCbY_)
			offset = 1;
#endif
			inc = 2;
			break;
		case AITC_Cb_CH:
#if defined (AITD_YCbYCr_)
			offset = 1;
#elif defined (AITD_YCrYCb_)
			offset = 3;
#elif defined (AITD_CbYCrY_)
			offset = 0;
#elif defined (AITD_CrYCbY_)
			offset = 2;
#endif
			inc = 4;
			break;
		case AITC_Cr_CH:
#if defined (AITD_YCbYCr_)
			offset = 3;
#elif defined (AITD_YCrYCb_)
			offset = 1;
#elif defined (AITD_CbYCrY_)
			offset = 2;
#elif defined (AITD_CrYCbY_)
			offset = 0;
#endif
			inc = 4;
			break;
	}
	while ( offset < FrameSize )
	{
		*((u_short *)Image->ImageAddr + offset) = (u_char) value;
		offset += inc;
	}

	return A8_NO_ERROR;
}


s_short A800_Adjust_CR(A8S_IMAGE *Image, s_short cr)
{
	cr = cr + 128;
	A800_SetFrameColor(Image, AITC_Cr_CH, cr);

	return A8_NO_ERROR;
}


s_short A800_Adjust_MG(A8S_IMAGE *Image, s_short mg)
{
	mg = mg + 128;
	A800_SetFrameColor(Image, AITC_Cb_CH, mg);
	A800_SetFrameColor(Image, AITC_Cr_CH, mg);

	return A8_NO_ERROR;
}


s_short A800_Adjust_YB(A8S_IMAGE *Image, s_short yb)
{
	yb = yb + 128;
	A800_SetFrameColor(Image, AITC_Cb_CH, 255 - yb);
	A800_SetFrameColor(Image, AITC_Cr_CH, yb);
	
	return A8_NO_ERROR;
}


s_short A800_Adjust_CR_MG_YB(A8S_IMAGE *Image, s_short cr, s_short mg, s_short yb) 
{
	s_short OutCr, OutCb;
	OutCr = cr + mg - yb + 128;

	if (OutCr > 255)
		OutCr = 255;
	if (OutCr < 0)
		OutCr = 0;

//	if (yb> 0)
//		OutCb = (128+ yb + mg);
//	else
//		OutCb = mg + 128;

 	if (yb< 0)
		OutCb = (128+ yb*2 + mg);
	else
		OutCb = (128+ yb + mg); 

	if (OutCb > 255)
		OutCb = 255;
	if (OutCb < 0)
		OutCb = 0;

	if(OutCr != 128)
		A800_SetFrameColor(Image, AITC_Cr_CH, OutCr);
	if(OutCb != 128)
		A800_SetFrameColor(Image, AITC_Cb_CH, OutCb);
    
    return A8_NO_ERROR;
}

s_short A800_Adjust_Brightness(A8S_IMAGE *Image, s_short bright)
{
	s_int FrameSize;
	s_int offset;
	s_short b;

#if defined (AITD_YCbYCr_)
		offset = 0;
#elif defined (AITD_YCrYCb_)
		offset = 0;
#elif defined (AITD_CbYCrY_)
		offset = 1;
#elif defined (AITD_CrYCbY_)
		offset = 1;
#endif

	FrameSize = Image->Width* Image->Height<< 1;
	while ( offset < FrameSize )
	{
		b = *((u_short *)Image->ImageAddr+ offset) + bright;
		
             if (b>255)
                b=255;
             if (b<0)
                b=0;
		
		*((u_short *)Image->ImageAddr+ offset) = (u_char)b; //(u_char) (b>255 ? 255 : b);
		offset+=2;
	}
	return A8_NO_ERROR;
}


s_short A800_Adjust_Contrast(A8S_IMAGE *Image, s_short contrast)
{
	s_int FrameSize;
	s_int offset;
	s_short b, min, max, r;

#if defined (AITD_YCbYCr_)
		offset = 0;
#elif defined (AITD_YCrYCb_)
		offset = 0;
#elif defined (AITD_CbYCrY_)
		offset = 1;
#elif defined (AITD_CrYCbY_)
		offset = 1;
#endif

	FrameSize = Image->Width* Image->Height<< 1;
	max = 0; 
	min = 255;
	while ( offset < FrameSize )
	{
		b = *((u_short *)Image->ImageAddr+ offset);
		if ( max < b )
			max = b;
		if ( min > b )
			min = b;
		offset+=2;
	}

	r = (max + contrast);
	r = (r << 8) / max;

	if (contrast > min)
		contrast = min;

#if defined (AITD_YCbYCr_)
		offset = 0;
#elif defined (AITD_YCrYCb_)
		offset = 0;
#elif defined (AITD_CbYCrY_)
		offset = 1;
#elif defined (AITD_CrYCbY_)
		offset = 1;
#endif

	while ( offset < FrameSize )
	{
		b = *((u_short *)Image->ImageAddr+ offset);

		if (contrast < 0) {
			b = (b * r) >> 8;
			//b = b - contrast;
		}
		else {
			b = b - contrast;
			b = (b * r) >> 8;
		}

		*((u_short *)Image->ImageAddr+ offset) = (u_char) (b>255 ? 255 : b);
		offset+=2;
	}
	return A8_NO_ERROR;
}

s_short A800_Set_Effect_Black_White(A8S_IMAGE *Image )
{
	A800_SetFrameColor(Image, AITC_Cb_CH, 128);
	A800_SetFrameColor(Image, AITC_Cr_CH, 128);
	return A8_NO_ERROR;
}

s_short A800_Set_Effect_Moonlight(A8S_IMAGE *Image )
{
	A800_SetFrameColor(Image, AITC_Cb_CH, 142);
	A800_SetFrameColor(Image, AITC_Cr_CH, 92);
	return A8_NO_ERROR;
}

s_short A800_Set_Effect_Antique(A8S_IMAGE *Image )
{
	A800_SetFrameColor(Image, AITC_Cb_CH, 128-24);
	A800_SetFrameColor(Image, AITC_Cr_CH, 128+12);
	return A8_NO_ERROR;
}

s_short A800_Set_Effect_Sepia(A8S_IMAGE *Image )
{
	A800_SetFrameColor(Image, AITC_Cb_CH, 128-16);
	A800_SetFrameColor(Image, AITC_Cr_CH, 128+16);
	return A8_NO_ERROR;
}

#if 1
s_short A800_Set_Effect_Sketch(A8S_IMAGE *Image, u_char *OpBuffer )
{
	s_int FrameSize;
	s_int Coffset, Yoffset,  YoffsetInit, YoffsetCorret;
	s_int TwoRowLength;
	s_short temp, HCount;
	u_short ShiftPixel;

#if defined (AITD_YCbYCr_)
		Yoffset = 0;
		Coffset = 1;
#elif defined (AITD_YCrYCb_)
		Yoffset = 0;
		Coffset = 1;
#elif defined (AITD_CbYCrY_)
		Yoffset = 1;
		Coffset = 0;
#elif defined (AITD_CrYCbY_)
		Yoffset = 1;
		Coffset = 0;
#endif

	YoffsetInit = Yoffset;
	FrameSize = Image->Width* Image->Height<< 1;
	TwoRowLength = Image->Width<< 2;

	while ( Coffset < FrameSize )
	{
		*((u_short *)Image->ImageAddr+ Coffset) = 128;
		*((u_short *)Image->ImageAddr+ Coffset + 2) = 128;
		Coffset+=4;
	}

	HCount = 0;
	ShiftPixel = 2;
	while ( Yoffset < (FrameSize) )	
	{
		YoffsetCorret = Yoffset - YoffsetInit;
		if  (YoffsetCorret >= ShiftPixel && YoffsetCorret <= (TwoRowLength-ShiftPixel)) 
		{
			temp = *((u_short *)Image->ImageAddr+ Yoffset+ShiftPixel) - *((u_short *)Image->ImageAddr+ Yoffset-ShiftPixel) ; //top
		} 
		else if ((YoffsetCorret - ((YoffsetCorret/TwoRowLength)*TwoRowLength ))< ShiftPixel) 
		{
			temp =  *((u_short *)Image->ImageAddr + Yoffset+TwoRowLength) - *((u_short *)Image->ImageAddr + Yoffset-TwoRowLength); //left
		} 
		else if ((YoffsetCorret - ((YoffsetCorret/TwoRowLength)*TwoRowLength)) > (TwoRowLength-ShiftPixel)) 
		{
			temp = *((u_short *)Image->ImageAddr + Yoffset+TwoRowLength) - *((u_short *)Image->ImageAddr + Yoffset-TwoRowLength); //right
		} 
		else if (YoffsetCorret >= (FrameSize-TwoRowLength + ShiftPixel) && YoffsetCorret <= (FrameSize - ShiftPixel) )
		{
			temp = *((u_short *)Image->ImageAddr + Yoffset+ShiftPixel) - *((u_short *)Image->ImageAddr + Yoffset-ShiftPixel); //bottom
		} 
		else 
	{
			temp = *((u_short *)Image->ImageAddr + Yoffset+ShiftPixel) - *((u_short *)Image->ImageAddr + Yoffset-ShiftPixel) + 
				     *((u_short *)Image->ImageAddr + Yoffset+TwoRowLength) - *((u_short *)Image->ImageAddr + Yoffset-TwoRowLength); //normal
		}
		
		if ( temp > 0 )
			temp = -temp;
		temp += 255;
		if ( temp > 255 )
			temp = 255;
		if ( temp <0 )
			temp = 0;
		
		*(OpBuffer + Yoffset) = temp;
		HCount++;
			Yoffset += 2;
	}


#if defined (AITD_YCbYCr_)
		Yoffset = 0;
#elif defined (AITD_YCrYCb_)
		Yoffset = 0;
#elif defined (AITD_CbYCrY_)
		Yoffset = 1;
#elif defined (AITD_CrYCbY_)
		Yoffset = 1;
#endif


	HCount = 0;
	while ( Yoffset < (FrameSize) )
	{
		*((u_short *)Image->ImageAddr+ Yoffset) = *(OpBuffer + Yoffset);
			Yoffset += 2;
	}

	return A8_NO_ERROR;
}
#endif

s_short A800_Set_Effect_Negative(A8S_IMAGE *Image )
{
	s_int FrameSize;
	s_int offset=0;

	FrameSize = Image->Width* Image->Height<< 1;
	while ( offset < FrameSize )
	{
		*((u_short *)Image->ImageAddr+ offset) = (u_char)(255 - *((u_short *)Image->ImageAddr+ offset));
		offset++;
	}
	return A8_NO_ERROR;
}

s_short A800_Set_Effect_Fog(A8S_IMAGE *Image )
{
	s_int FrameSize;
	s_int offset;

#if defined (AITD_YCbYCr_)
		offset = 0;
#elif defined (AITD_YCrYCb_)
		offset = 0;
#elif defined (AITD_CbYCrY_)
		offset = 1;
#elif defined (AITD_CrYCbY_)
		offset = 1;
#endif

	FrameSize = Image->Width* Image->Height<< 1;
	while ( offset < FrameSize )
	{
		*((u_short *)Image->ImageAddr+ offset) = (u_char)(128 + (*((u_short *)Image->ImageAddr+ offset)>>1));
		offset+=2;
	}
	return A8_NO_ERROR;
}

s_short A800_Set_Effect_Blur(A8S_IMAGE *Image, u_char *OpBuffer)
{
      s_int FrameSize;
      s_int Yoffset;
      s_int TwoRowLength;
      s_int temp, HCount;

#if defined (AITD_YCbYCr_)
            Yoffset = 0;
#elif defined (AITD_YCrYCb_)
            Yoffset = 0;
#elif defined (AITD_CbYCrY_)
            Yoffset = 1;
#elif defined (AITD_CrYCbY_)
            Yoffset = 1;
#endif

      FrameSize = Image->Width* Image->Height<< 1;
      TwoRowLength = Image->Width<< 2;

      Yoffset += (TwoRowLength + 4);
      HCount = 0;
      while ( Yoffset < (FrameSize-TwoRowLength) )
      {
            temp = (*((u_short *)Image->ImageAddr+ Yoffset+4)<<1) + (*((u_short *)Image->ImageAddr+ Yoffset-4)<<1)
                  + (*((u_short *)Image->ImageAddr+ Yoffset+TwoRowLength)<<1) + (*((u_short *)Image->ImageAddr + Yoffset-TwoRowLength)<<1)
                  + (*((u_short *)Image->ImageAddr + Yoffset+4 + TwoRowLength )<<1) + (*((u_short *)Image->ImageAddr + Yoffset-4 + TwoRowLength )<<1)
                  + (*((u_short *)Image->ImageAddr + Yoffset+4 - TwoRowLength )<<1) + (*((u_short *)Image->ImageAddr + Yoffset-4 - TwoRowLength )<<1)
                  + (*((u_short *)Image->ImageAddr + Yoffset));
            temp = temp / 17;

            *(OpBuffer + Yoffset) = temp;
            HCount++;
            if (HCount > (Image->Width- 4))
            {
                  Yoffset += 8;
                  HCount = 0;
            }
            else
                  Yoffset += 2;
      }

#if defined (AITD_YCbYCr_)
            Yoffset = 0;
#elif defined (AITD_YCrYCb_)
            Yoffset = 0;
#elif defined (AITD_CbYCrY_)
            Yoffset = 1;
#elif defined (AITD_CrYCbY_)
            Yoffset = 1;
#endif
      Yoffset += (TwoRowLength + 4);
      HCount = 0;
      while ( Yoffset < (FrameSize-TwoRowLength) )
      {
            *((u_short *)Image->ImageAddr + Yoffset) = *(OpBuffer + Yoffset);
            HCount++;
            if (HCount > (Image->Width- 4))
            {
                  Yoffset += 8;
                  HCount = 0;
            }
            else
                  Yoffset += 2;
      }

      return A8_NO_ERROR;
}

#pragma O1

s_short A800_Set_Effect_Sharpness(A8S_IMAGE *Image, u_char *OpBuffer)
{
      s_int FrameSize;
      s_int Yoffset;
      s_int TwoRowLength;
      s_int temp, HCount;
//      s_int temp1, temp2, temp3, temp4, temp5;

#if defined (AITD_YCbYCr_)
            Yoffset = 0;
#elif defined (AITD_YCrYCb_)
            Yoffset = 0;
#elif defined (AITD_CbYCrY_)
            Yoffset = 1;
#elif defined (AITD_CrYCbY_)
            Yoffset = 1;
#endif


      FrameSize = Image->Width* Image->Height<< 1;
      TwoRowLength = Image->Width<< 2;

      Yoffset += (TwoRowLength + 4);
      HCount = 0;

      while ( Yoffset < (FrameSize-TwoRowLength) )
      {        
        
            temp = (*((u_short *)Image->ImageAddr+ Yoffset) * 8) - (*((u_short *)Image->ImageAddr + Yoffset+4)) - (*((u_short *)Image->ImageAddr + Yoffset-4))
                  	- (*((u_short *)Image->ImageAddr + Yoffset+TwoRowLength)) - (*((u_short *)Image->ImageAddr+ Yoffset-TwoRowLength));

            temp = temp >> 2;
            if (temp < 0)
                  temp = 0;
            if (temp > 255)
                  temp = 255;

            *(OpBuffer + Yoffset) = temp;
            HCount++;
            if (HCount > (Image->Width- 4))
            {
                  Yoffset += 8;
                  HCount = 0;
            }
            else
                  Yoffset += 2;
      }

#if defined (AITD_YCbYCr_)
            Yoffset = 0;
#elif defined (AITD_YCrYCb_)
            Yoffset = 0;
#elif defined (AITD_CbYCrY_)
            Yoffset = 1;
#elif defined (AITD_CrYCbY_)
            Yoffset = 1;
#endif
      Yoffset += (TwoRowLength + 4);
      HCount = 0;
      while ( Yoffset < (FrameSize-TwoRowLength) )
      {
            *((u_short *)Image->ImageAddr+ Yoffset) = *(OpBuffer + Yoffset);
            HCount++;
            if (HCount > (Image->Width- 4))
            {
                  Yoffset += 8;
                  HCount = 0;
            }
            else
                  Yoffset += 2;
      }

      return A8_NO_ERROR;
}

s_short A800_Set_Effect_Solarize(A8S_IMAGE *Image) 
{
    // /\ gamma
	s_int FrameSize;
	s_int RGBOffset;
	s_short R, G, B;
	u_short RGBTemp;

	 FrameSize = Image->Width* Image->Height; 
	 RGBOffset = 0;

	while(RGBOffset<FrameSize)
	{
	        R = (*((u_short *)Image->ImageAddr+RGBOffset) & 0xF800)>>8;
	        G = (*((u_short *)Image->ImageAddr+RGBOffset) & 0x07E0)>>3; 
	        B = (*((u_short *)Image->ImageAddr+RGBOffset) & 0x001F)<<3;

	        if (R >= 128)
	            R = 512 - ((255 * R)>>7) ;
	        else
	            R = (R<<1);

	        if (G >= 128)
	            G = 512 -( (255 * G)>>7) ;
	        else
	            G = ( G<<1);

	        if (B >= 128)
	            B =512 - ((255 * B)>>7) ;
	        else
	            B = (B<<1);

		if (R < 0)
			R = 0;
		if (R > 255)
			R = 255;

		if (G < 0)
			G = 0;
		if (G > 255)
			G = 255;

		if (B < 0)
			B = 0;
		if (B > 255)
			B = 255;

		RGBTemp = ((R&0x00F8)<<8) + ((G&0x00FC)<<3) + ((B&0x00F8)>>3);

		*((u_short *)Image->ImageAddr+RGBOffset) = RGBTemp;

		RGBOffset++;
	}
    return A8_NO_ERROR;
}

s_short A800_Set_Effect_Posterize(A8S_IMAGE *Image) 
{
    // 4 step gamma, 0, 85, 170, 255
    	s_int FrameSize;
	s_int RGBOffset;
	s_short R, G, B;
	u_short RGBTemp;

	FrameSize = Image->Width* Image->Height; 
	 RGBOffset = 0;
		
	while(RGBOffset<FrameSize)
	{	
		R =(((*((u_short *)Image->ImageAddr+RGBOffset) & 0xF800)>>8)>>6)*85 ;
		G =(((*((u_short *)Image->ImageAddr+RGBOffset) & 0x07E0)>>3)>>6)*85  ;
		B = (*((u_short *)Image->ImageAddr+RGBOffset) & 0x001F)<<3 ;

		if (B < 65)
			B = 0;
		else if (B < 150)
			B = 85*1;
		else if (B < 205)
			B = 85*2;
		else B = 85*3;
		RGBTemp = ((R&0x00F8)<<8) + ((G&0x00FC)<<3) + ((B&0x00F8)>>3);
		*((u_short *)Image->ImageAddr+RGBOffset) = RGBTemp;

		RGBOffset++;
 	}                                                           
	                                                               
    return A8_NO_ERROR;
}

#define ByteClip(x)  ((x<0)? 0:(x<255)?x:255 )
 
u_short YCbCr2RGB(u_short *RGB,u_short *YUV)
{
  s_int Y1,Cb,Cr,Y2;
  s_int R1,G1,B1,R2,G2,B2,tmpRGB;
  Cr=YUV[1] & 0xFF;
  Y2=(YUV[1] >>8) & 0xFF;
  Cb=YUV[0] & 0xFF;
  Y1=(YUV[0] >>8) & 0xFF;

  R1 = ByteClip(Y1+( (91881* (Cr-128))>>16));

  G1 = ByteClip(Y1 -( (22553* (Cb-128))>>16 )-( (46802*(Cr-128))>>16));

  B1 = ByteClip(Y1 + ((116130 * (Cb-128)) >>16));

  R2 = ByteClip(Y2+( (91881* (Cr-128))>>16));

  G2 = ByteClip(Y2 - ((22553* (Cb-128))>>16) -( (46802*(Cr-128))>>16));

  B2 = ByteClip(Y2 +( (116130 * (Cb-128)) >>16));
  tmpRGB=(R1 & 0xF8)<<8;
  tmpRGB+=(G1 &0xFC) <<3;
  tmpRGB+=(B1 &0xF8) >>3;
  RGB[0]=tmpRGB;

  tmpRGB=(R2 & 0xF8)<<8;
  tmpRGB+=(G2 &0xFC) <<3;
  tmpRGB+=(B2 &0xF8) >>3;
  RGB[1]=tmpRGB;
  return A8_NO_ERROR;
}

// YUV and RGB are in different buffers. Update to other buffer.
u_short A800_YCbCr2RGB(u_short *yuv_ptr, u_short *rgb_ptr, u_short width, u_short height)
{
	u_short tmpYCbCr[2];
	u_short *RGBData = rgb_ptr;
	u_short *YCCData = yuv_ptr;
	s_int i, j;

	for (j = 0 ; j < height; j++)
	{
		YCCData = yuv_ptr +(j * width);
		for (i=0;i<(width >> 1);i++)
		{
			tmpYCbCr[0]=*YCCData;
			YCCData+=1;
			tmpYCbCr[1]=*YCCData;
			YCCData+=1;

			YCbCr2RGB(RGBData, tmpYCbCr);
			RGBData += 2;
		}
	}
  	return A8_NO_ERROR;
}

u_short A800_TransformColorSpace(A8S_IMAGE SrcImage, A8S_IMAGE *DstImage)
{
    u_short  i;
    u_short	N, M;
    u_short	rateH, rateV;

    u_short	LineOffset, ColorDepth;
    u_int BufAddr;
    
    if ((DstImage->Width <= SrcImage.Width) && (DstImage->Height <= SrcImage.Height)) // Scale down
    {
        M = 60; // this value need to sync with Preview.
        rateH = DstImage->Width * M / SrcImage.Width;         
        rateV = DstImage->Height * M / SrcImage.Height;
    
        if( (SrcImage.Width * rateH / M - DstImage->Width ) < 0 )
            rateH+=1;
        if( (SrcImage.Height* rateV / M - DstImage->Height) < 0 )
            rateV+=1;
        if (rateH < rateV)
            rateH = rateV;
    }
    else        // Scale up
    {
        M = 20; 
        rateH = DstImage->Width * M / (SrcImage.Width-1);
        rateV = DstImage->Height * M / (SrcImage.Height-1);
    
        if( ((SrcImage.Width -1) * rateH / M - DstImage->Width) < 0 )
            rateH+=1;
        if( ((SrcImage.Height -1) * rateV / M - DstImage->Height) < 0 )
            rateV+=1;
        if (rateH < rateV)
            rateH = rateV;
    }
    
    N = rateH;
    
    AIT_Message_P1("N = [%d]\r\n", N );
    AIT_Message_P1("M = [%d]\r\n", M);

    //YUV422 to RGB565 or RGB565 to YUV422
    if (DstImage->Width*DstImage->Height*2 <= 96*1024)//((SrcImage.Width != DstImage->Width) || (SrcImage.Height != DstImage->Height))
    {
		AIT_Message_P0("scaling\r\n");
      
      	A8L_ConvertImageFIFOToBuf((u_short *)SrcImage.ImageAddr, N, M, SrcImage.Width, SrcImage.Height, SrcImage.ColorDepth, 
								DstImage->Width, DstImage->Height, DstImage->ColorDepth, 0x0000);

		BufAddr = 0L;
		
		if(DstImage->Width % 4)
			LineOffset = ((DstImage->Width + (4- DstImage->Width %4)) << 1);
		else
			LineOffset = (DstImage->Width<< 1);

		if ( (DstImage->Width% 4) != 1 ) 
		{
			if ((DstImage->Width% 4) == 0)
			{	// 20070621, update. Memory copy is more stable!!
				CopyMemWordA8ToHost((u_short *)DstImage->ImageAddr, 0L, DstImage->Width*DstImage->Height*2);    
			}
			else
			{
				ColorDepth = ((DstImage->ColorDepth == 2)? A8_YUV_422 : A8_DEPTH_16B);
				
				A8L_SetBufferAttribute1(6, DstImage->Width, DstImage->Height, LineOffset, ColorDepth, 0L);
				A8L_CopyImageBuftoFIFO((u_short *)DstImage->ImageAddr, DstImage->Width, DstImage->Height, 6, 0, 0);
			}
		}
		else
		{
			//BufAddr = DecodeDataBuf;
			for ( i=0; i<DstImage->Height; i++ ) 
			{
				CopyMemWordA8ToHost((u_short *)DstImage->ImageAddr, BufAddr, DstImage->Width <<1);	/* length in byte*/
				DstImage->ImageAddr += DstImage->Width;
				BufAddr += (DstImage->Width << 1);
				if (i & 0x01)
					BufAddr += 4;
			}
		}
    }
    else if((SrcImage.ColorDepth == 2) && (DstImage->ColorDepth == 1))
    {
        AIT_Message_P0("no scaling\r\n");

        A800_YCbCr2RGB((u_short *)SrcImage.ImageAddr, (u_short *)DstImage->ImageAddr, SrcImage.Width, SrcImage.Height);
    }
    
}
#endif
/**
 @fn		u_short A800_SensorClockOutput(u_char enable, u_char freqDiv)
 @brief	
 @return	u_short
 @note	.
 @bug	N/A.
*/
u_short A800_SensorClockOutput(u_char enable, u_char freqDiv)
{
	if(enable)
	{
		if(freqDiv>0)
		{
			SetA8RegB(0x6000, GetA8RegB(0x6000) |((freqDiv-1) & 0x3F));
			SetA8RegB(0x6010, GetA8RegB(0x6010) |(1<<1));
			SetA8RegB(0x601E, GetA8RegB(0x601E) |((freqDiv-1) & 0x1F));
		}
		
		SetA8RegB(0x6903, GetA8RegB(0x6903) & ~(1<<7));	//TG clock Enable
		SetA8RegB(0x601D, GetA8RegB(0x601D) | (1<<7));	//Pixel Clock Enable
		SetA8RegB(0x6902, GetA8RegB(0x6902) | (1<<7));	//for host bypass draw LCD
	}
	else
	{
		SetA8RegB(0x601D, GetA8RegB(0x601D) & ~(1<<7));	// Pixel Clock Disable
		sys_IF_ait_delay1ms(1);
		SetA8RegB(0x6903, GetA8RegB(0x6903) |(1<<7));	// TG clock Disable
		SetA8RegB(0x6902, GetA8RegB(0x6902) & ~(1<<7));	// for host bypass draw LCD
	}
	
	return A8_NO_ERROR;
}

/**
 @fn		void A800_SetSensorIF(u_char gpio, u_char  enable)
 @brief	
 @return	void
 @note	.
 @bug	N/A.
*/
void A800_SetSensorIF(u_char gpio, u_char  enable)
{
	SetA8RegB(0x6050,GetA8RegB(0x6050) | (0x01 << gpio));

	if (enable)
		SetA8RegB(0x6051,GetA8RegB(0x6051) | (0x01 << gpio));
	else
		SetA8RegB(0x6051,GetA8RegB(0x6051) & (~(0x01 << gpio)));
}

/**
 @fn		u_short 	A800_CheckFrameStart(void)
 @brief	
 @return	u_short
 @note	.
 @bug	N/A.
*/
u_short A800_CheckFrameStart(void)
{
	return A8L_CheckFrameStart();
}

/**
 @fn		u_short 	A800_CheckExposureDone(void)
 @brief	
 @return	u_short
 @note	.
 @bug	N/A.
*/
u_short A800_CheckExposureDone(void)
{
	return A8L_CheckExposureDone();
}

/**
 @fn		u_short A800_SetSenReg(u_short Reg ,u_short Value)
 @brief	Write Sensor Register.
 @param	Reg - the Sensor Register.
 @param	Value - data write to sensor.
 @return	u_short - 0x00:Success, 0x01:timeout failure .
 @note	.
 @bug	N/A.
*/
u_short A800_SetSenReg(u_short Reg, u_short Value)
{
	return A8L_SetSenReg(Reg ,Value);
}

/**
 @fn		u_short A800_GetSenReg(u_short Reg ,u_short *Value)
 @brief	Read Sensor Register.
 @param	Reg - the Sensor Register.
 @param	Value - read back data from sensor.
 @return	u_short .
 @note	.
 @bug	N/A.
*/
u_short A800_GetSenReg(u_short Reg, u_short *Value)
{
	return A8L_GetSenReg(Reg ,Value);
}

/**
 @fn		u_char 	A800_SensorPowerOn(void)
 @brief	Sensor power on
 @return	u_char
 @note	.
 @bug	N/A.
*/
u_char	A800_SensorPowerOn(u_char on)
{
	return A8L_SensorPowerOn(on);
}

/**
 @fn		u_char 	A800_PreInit_Sensor(void)
 @brief	Sensor PreInit
 @return	u_char
 @note	.
 @bug	N/A.
*/
u_char A800_PreInit_Sensor(void)
{
	u_char retVal;
	
	retVal = A8L_PreInit_Sensor();
	A8L_SensorPowerOn(A8_OFF);

	return retVal;
}

/**
 @fn		u_char 	A800_Init_Sensor(void)
 @brief	Sensor Init
 @return	u_char
 @note	.
 @bug	N/A.
*/
u_char	A800_Init_Sensor(void)
{
	return A8L_Init_Sensor();
}

/**
 @fn		u_short A800_SensorEffect(u_char value)
 @brief	Setup Sensor Effect.
 @param	value - 0:normal, 1~x (specia, BW, oil,~~~) Depends on Sensor.
 @return	u_char
 @note	.
 @bug	N/A.
*/
u_short A800_SensorEffect(u_char value)
{
	u_short Pstatus;
	s_short retVal;
EntryAITAPI;
	Pstatus=A8L_CheckLCDPreview();
	if (Pstatus)
            A8L_PauseLCDPreview();
	retVal = A8L_SensorEffect(value);
  // AIT_Message_P1( "AIT effect value = %x \n", value );
	if (Pstatus)
		A8L_ResumeLCDPreview();
	if (retVal != 0 )
	   return retVal;
ExitAITAPI;
	return A8_NO_ERROR;
}

/**
 @fn		u_char A800_SensorBrightness(u_char Value)
 @brief	Tune the image exposure targe.
 @param	Value : 1~12
 @return	u_char
 @note	+ 0.25EV step.
 @bug	N/A.
*/
u_char 	A800_SensorBrightness(u_char Value)
{
  u_char retVal = 0;

  EntryAITAPI;
  retVal = A8L_SensorBrightness(Value);
  ExitAITAPI;
  return retVal;
}

/**
 @fn		u_char 	A800_SensorContrast(u_char value)
 @brief	Setup Contrast.
 @param	value - 
 @return	u_char
 @note	.
 @bug	N/A.
*/
u_char 	A800_SensorContrast(u_char value)
{
	return A8_NO_ERROR;
}

/**
 @fn		u_char 	A800_SensorWBMode(u_char value)
 @brief	Auto White Balance.
 @param	value - 
 @return	u_char
 @note	.
 @bug	N/A.
*/
u_char 	A800_SensorWBMode(u_char value)
{
  u_char retVal = 0;

  EntryAITAPI;
  retVal = A8L_SensorWBMode( value );
  ExitAITAPI;
  return retVal; 
}

/**
 @fn		u_char 	A800_SensorNightMode(u_char value)
 @brief	Enable/Disable Sensor Night.
 @param	value - 0 : Disable , 1 : Enable
 @return	u_char
 @note	.
 @bug	N/A.
*/
u_char 	A800_SensorNightMode(u_char value)
{
  // AIT_Message_P1 ("AIT Set Night Mode value = %x\n", value );
	return A8L_SensorNightMode(value);
}

/**
 @fn		u_char A800_SensorDebandMode(u_char value)
 @brief	Setup Deband mode of 50/60 Hz.
 @param	value - Deband Frequency.
 @return	u_char
 @note	For Deband function.
 @bug	N/A.
*/
u_char 	A800_SensorDebandMode(u_char value)
{
  u_char retVal = 0;

  EntryAITAPI;
  retVal = A8L_SensorDebandMode(value);
  ExitAITAPI;
  return retVal;
}

/**
 @fn		u_char 	A800_SensorRotation(u_char dir)
 @brief	Setup Sensor Direction.
 @param	dir -Sensor Direction, 0:normal, 1:mirror, 2:vertical, 3:rotate
 @return	u_char
 @note	.
 @bug	N/A.
*/
u_char 	A800_SensorRotation(u_char dir)
{
	u_char RetVal;
	
	RetVal = A8L_SensorRotation(dir);

	return RetVal;
}

/**
 @fn		s_short A800_SensorFixedFrameRate( s_short FrameRate )
 @brief	Set Sensor frame rate for Video operation.
 @param	FrameRate - the Sensor frame rate.
 @return	s_short - 0x00:Success, 0x01:timeout failure .
 @note	The sensor frame rate for Video operation mostly depends on Host performance.
 @bug	N/A.
*/
u_char A800_SensorFixedFrameRate( u_char set )
{
	return A8L_SensorFixedFrameRate(set);//camcorder
}

u_char	A800_BeforeCapture(u_char Reso, u_char Zoom)
{
	return A8L_BeforeCapture(Reso, Zoom);
}

u_char	A800_AfterCapture(u_char Reso, u_char Zoom)
{
	return A8L_AfterCapture(Reso, Zoom);
}

/**
 @fn		u_char 	A800_SensorMatrixMode(void)
 @brief	MATRIX Mode.
 @param	value - 
 @return	u_char
 @note	.
 @bug	N/A.
*/
u_char 	A800_SensorMatrixMode(void)
{
  u_char retVal = 0;

  EntryAITAPI;
  retVal = A8L_SensorMatrixMode();
  ExitAITAPI;
  return retVal; 
}

/**
 @fn		u_char 	A800_SensorCenterWeightedMode(void)
 @brief	CENTERWEIGHTED Mode.
 @param	value - 
 @return	u_char
 @note	.
 @bug	N/A.
*/
u_char 	A800_SensorCenterWeightedMode(void)
{
  u_char retVal = 0;

  EntryAITAPI;
  retVal = A8L_SensorCenterWeightedMode();
  ExitAITAPI;
  return retVal; 
}

/**
 @fn		u_char 	A800_SensorSpotMode(void)
 @brief	SPOT Mode.
 @param	value - 
 @return	u_char
 @note	.
 @bug	N/A.
*/
u_char 	A800_SensorSpotMode(void)
{
  u_char retVal = 0;

  EntryAITAPI;
  retVal = A8L_SensorSpotMode();
  ExitAITAPI;
  return retVal; 
}

u_short	A800_SetMJPEGRecordStart(void)
{
	A8L_SetJpegQuality(0);
	
	SetA8RegB(0x6206, 0x10);

	A8L_SetVideoResolution( 176, 144, FORMAT_YUV_422, FILE_FORMAT_AVI );

	A8L_SetVideoCaptureMemConfig(A8_AVI_ENCODE_COMP_BUF_START_ADDR, 
									A8_AVI_ENCODE_COMP_BUF_END_ADDR,
									A8_AVI_ENCODE_ROW_BUF_ADDR);
	

	A8L_StartMJPEGCapture();
	
	return A8_NO_ERROR;
}

u_short	A800_SetMJPEGRecordStop(void)
{	
	SendA8Cmd(A8_HOST_CMD_STOP_MJPEG_REC);
	while(!(GetA8DSCStatus() & A8_DSC_VIDEO_STOP_ST));
	
	return A8_NO_ERROR;
}

u_char A800_TransferMJPEGFrameToHost(u_char *frameBuf, u_int *frameSize)
{
	return A8L_TransferMJPEGFrameToHost(frameBuf, frameSize);
}
#ifdef SENSOR_VIF
extern void A8L_CheckSensorFrame(u_char mode);
#else
extern u_char A8L_CheckSensorFrame(u_char mode);
#endif
void A800_PIPBypass(u_char bEnable)
{
	if(bEnable)
	{
		
		SetA8RegB(0x6510,GetA8RegB(0x6510) & (~0x04));
		SetA8RegB(0x5040,GetA8RegB(0x5040) & (~0x2));
			
		SetA8RegB(0x504A,  GetA8RegB(0x504A)  & (~0x40));
		SetA8RegB(0x504B,GetA8RegB(0x504B) | 0x01);
//		SetA8RegW(0x5078, 0x0);
//		SetA8RegW(0x507a, 0x0);
//		SetA8RegW(0x507c, 0x0);
//		SetA8RegW(0x507e, 0x0);
//		SetA8RegW(0x5080, 0x0);
	}
	else
	{
		SetA8RegB(0x504B,GetA8RegB(0x504B) & (~0x01));			
	}
}

void A800_SetQTable(u_char Index)
{
	u_char	*QTablePtr,*TmpPtr;	
	u_int 	i;
	SetA8RegB(0x6206, 0x10);
	switch(Index)
	{
		case	0x1:
			QTablePtr = (u_char*)AIT_QTableFine;
			break;
		case	0x2:		
			QTablePtr = (u_char*)AIT_QTableNormal;
			break;
		case	0x3:
			QTablePtr = (u_char*)AIT_QTableEconomy;
			break;		
		case	0x4:
			QTablePtr = (u_char*)AIT_QTableWorst;
			break;								
		default:
			QTablePtr = (u_char*)AIT_QTableSuperFine;
			break;		
	}
	
	for(i=0x4300;i<0x4340;i++)
		SetA8RegB(i,*QTablePtr++);
	TmpPtr = QTablePtr;
	for(i=0x4340;i<0x4380;i++)	
		SetA8RegB(i,*QTablePtr++);
	for(i=0x4380;i<0x43c0;i++)
		SetA8RegB(i,*TmpPtr++);		
}


u_short	A800_TakeJpegWithSticker(u_short *pdata, u_int *filesize)
{

	u_int RGBImageAddr, FrameImgAddr;
	u_short ImageWidth, ImageHeight ;
	u_short retVal, trans_color;
	u_short i, j, temp_pixel;
	u_int	MaxSize,JpegCompBuf,JpegLineBuf;   
	s_int Status;


	ImageWidth = A800Data.Select->DispWidth;
	ImageHeight = A800Data.Select->DispHeight;
	RGBImageAddr = A800Data.Select->BufAddr;
	MaxSize = ImageWidth * ImageHeight * 2;
	FrameImgAddr = A800Data.Select->MainOSD.OSDBufAddr;
	if ( A800Data.Select->MainOSD.OSDBufFormat != A8_DEPTH_16B )
		return A8_INCORRECT_PARA_ERROR;

	retVal = A8L_StopLCDPreview();

	if ( retVal != 0 )
	{
	   return A8_CAM_PREV_STOP_ERROR;
	}
// 0619 for Frame update in low fps
	SetA8RegB(0x500A, 0x02);
	sys_IF_ait_delay1ms(30);

	A8L_SetWindowActive(A8_PIP_WINDOW, 0);
#if 1    

    trans_color = GetA8RegW(MAIN_WIN_TP_COLOR_B_RG);
    for (i=0; i<ImageHeight ; i++)
    {
	for (j=0; j<ImageWidth ; j++)
       {
		temp_pixel = GetA8MemW(FrameImgAddr);
            //if (temp_pixel == 0xF81F)
            if (temp_pixel == trans_color)
            {
            	  FrameImgAddr = FrameImgAddr + 2;
                RGBImageAddr = RGBImageAddr + 2;  
            }
            else
            {
                SetA8MemW(RGBImageAddr, temp_pixel);
            	  FrameImgAddr = FrameImgAddr + 2;
                RGBImageAddr = RGBImageAddr + 2;  
            }
       }
    }    
#else
	RGBImageAddr = A800Data.Select->BufAddr;    /* 0x0000; */
	A8L_SetBufferAttribute(7, ImageWidth, ImageHeight, ImageWidth*2, A8_DEPTH_16B, RGBImageAddr);  
	sys_IF_ait_delay1ms(1);
	/* SetGraphicsBackground( TransparentColor ); */
	SetGraphicsTransparent( 1 );
	retVal = A8L_CopyImageBuftoBuf( ImageWidth, ImageHeight, 9, 0, 33, 7, 0, 0, GRH_ROP_SRCCOPY );
	/* This code, we need to make sure Src_Main and Des_PIP Buffer. */
#endif    

	sys_IF_ait_delay1ms(1);
	

#ifdef EXIFSUPPORT 
	gCurrentCaptureResolution = 5; 
#endif
	SetA8RegB( 0x6242, 0x03 );		// Clear JPEG frame encode done status
	SetA8RegW( 0x6222, 0x0000 );	// Clear JPEG FIFO
	RGBImageAddr = A800Data.Select->BufAddr;
	FrameImgAddr = A800Data.Select->MainOSD.OSDBufAddr;	
	JpegLineBuf = FrameImgAddr;
	JpegCompBuf = JpegLineBuf + ImageWidth * 32;
	
		
	SetA8RegB(0x690A, 0x0a);
//	sys_IF_ait_delay1ms(1);	
	SetA8RegB(0x690A, 0x00);
	
	SetA8RegW(0x4f30, 0x00);
	SetA8RegB(0x4f12, 0x01);
	SetA8RegW(0x4f14, 0x0101);
	SetA8RegB(0x4f16, 0x01);
	SetA8RegW(0x4f00, 0x01);
	SetA8RegW(0x4f02, ImageWidth);
	SetA8RegW(0x4f04, 0x01);
	SetA8RegW(0x4f06, ImageHeight);
	SetA8RegW(0x4f08, 0x01);
	SetA8RegW(0x4f0A, ImageWidth);
	SetA8RegW(0x4f0C, 0x01);
	SetA8RegW(0x4f0e, ImageHeight);
	SetA8RegW(0x6200, ImageWidth);
	SetA8RegW(0x6202, ImageHeight);
	SetA8RegW(0x6210, JpegLineBuf);
	SetA8RegW(0x6212, JpegLineBuf>>16);
	SetA8RegW(0x6214, JpegCompBuf);
	SetA8RegW(0x6216, JpegCompBuf>>16);
	SetA8RegW(0x6218, 0x7fff);
	SetA8RegW(0x621A, 0x0001);
	SetA8RegB(0x6510, 0x00);
	SetA8RegB(0x6511, 0x04);
	SetA8RegB(0x6511, 0x00);
//	SetA8RegB(0x6222, 0x80);
#if 0
	SetA8RegB(0x6204, 0x28);
#endif	
	SetA8RegW(0x6b50, RGBImageAddr);
	SetA8RegW(0x6b52, RGBImageAddr>>16);
	SetA8RegW(0x6b54, ImageWidth*2);
	SetA8RegW(0x6b56, 0x0000);
	SetA8RegW(0x6b06, ImageWidth);
	SetA8RegW(0x6b0c, ImageHeight);
	SetA8RegW(0x6b0e, 0x0100);

	SetA8RegB(0x6204, 0x28);

	SetA8RegB(0x6b24, 0x04);
	while((GetA8RegB(0x6204)&0x8)==0);
	while(GetA8RegB(0x6204)&0x8);	
//	SetA8RegB(0x6222, 0x00);		


	Status = A8L_GetJpegPictureBufWithLimit( pdata, JpegCompBuf, MaxSize, filesize );
	if ( Status != 0 )	{
			return A8_CAM_ENCODE_ERROR;	// 4;
	}


	*filesize = A8L_GetJpegPictureSize();
#ifdef EXIFSUPPORT // 080430 match file size for exif case
	*filesize += 0x314;
#endif

	return A8_NO_ERROR;

	
}

#if AIT_VIDEO_PHONE_SUPPORT
u_char A800_VideoPhoneStart(void)
{
    u_char retVal = A8_NO_ERROR;
    
    /* AIT do NOT output to LCD */
    SetA8RegB(DISP_GENERAL_CONTROL1_RG, GetA8RegB(DISP_GENERAL_CONTROL1_RG) & 0xFB);// Disable LCD ping-pong buffer
    SetA8RegB(0x500A, GetA8RegB(0x500A) & 0xFC);// Disable LCD Auto & Frame Mode;
    SetA8RegB(0x6510, GetA8RegB(0x6510) | 0x04);// Scaler output to PIP buffer.
    SetA8RegB(0x7110/*VIF_DATA_IN_EN*/, GetA8RegB(0x7110/*VIF_DATA_IN_EN*/) | 0x01);
    SetA8RegB(0x7111/*VIF_DATA_OUT_EN*/, GetA8RegB(0x7111/*VIF_DATA_OUT_EN*/) | 0x01);

    SetA8RegB(0x7103, GetA8RegB(0x7103) | 0x0F);//Clear cup int status
    SetA8RegB(0x7102, GetA8RegB(0x7102) | 0x02);//Enable cpu frame end int

    SetA8RegB(0x654E, GetA8RegB(0x654E) | 0x02); //preview start
    return retVal;
}

u_char A800_VideoPhoneStop(void)
{
    u_char retVal = A8_NO_ERROR;

    SetA8RegB(0x7103, GetA8RegB(0x7103) | 0x0F);//Clear cup int status
    SetA8RegB(0x7102, GetA8RegB(0x7102) & (~0x02));//Disable cpu frame end int
    
    SetA8RegB(0x6510, GetA8RegB(0x6510) & (~0x04));// Scaler output to PIP buffer.
    SetA8RegB(0x7110/*VIF_DATA_IN_EN*/, GetA8RegB(0x7110/*VIF_DATA_IN_EN*/) & (~0x01));
    SetA8RegB(0x7111/*VIF_DATA_OUT_EN*/, GetA8RegB(0x7111/*VIF_DATA_OUT_EN*/) & (~0x01));

    SetA8RegB(0x654E, GetA8RegB(0x654E) & (~0x02)); //preview stop
    return retVal;
}

u_char A800_VideoPhoneGetRGB565Data(u_short rgbWidth, u_short rgbHeight, u_char *rgb565_ptr)//Andy@20100224.
{
    u_char retVal = A8_NO_ERROR;

    u_int srcAddr;
#if 1 //Andy@20100223. 
    u_char status=0;
#endif

    if((0==rgb565_ptr) || (A800Data.Select->BufWidth != rgbWidth) || (A800Data.Select->BufHeight != rgbHeight)){
        AIT_Message_P0("A800_VideoPhoneGetRGB565Data parameter error.\r\n");
        retVal=A8_INCORRECT_PARA_ERROR;
    }
    
#if 1 //Andy@20100223.  Use ping-pong buffer in FW.
    status = GetA8RegB(0x654B);
    if(status & 0x01){ //PIP buffer 1 is working. PIP buffer 0 is ready
        srcAddr = A800Data.Select->BufAddr;
    }
    else{//PIP buffer 0 is working. PIP buffer 1 is ready
        srcAddr = A800Data.Select->BufAddr + A800Data.Select->BufWidth*A800Data.Select->BufHeight*2;
    }    
#endif

    CopyMemByteA8ToHost(rgb565_ptr, srcAddr, rgbWidth*rgbHeight*2);

    return retVal;
}

u_char A800_VideoPhoneGetYUV420Data(u_short yuvWidth, u_short yuvHeight, u_char *yuv420_ptr)
{
    u_char retVal = A8_NO_ERROR;

    u_int loop_x, loop_y, yuv_arraysize;
    u_int srcAddr;
    u_short data;
    u_char *y_ptr, *cb_ptr, *cr_ptr;
#if 1 //Andy@20100223. 
    u_char status=0;
#endif

    if((0==yuv420_ptr) || (A800Data.Select->BufWidth != yuvWidth) || (A800Data.Select->BufHeight != yuvHeight)){
        AIT_Message_P0("A800_VideoPhoneGetYUV420Data parameter error.\r\n");
        retVal=A8_INCORRECT_PARA_ERROR;
    }

    yuv_arraysize = (yuvWidth * yuvHeight);
    y_ptr = yuv420_ptr;
    cb_ptr = y_ptr + yuv_arraysize;
    cr_ptr = cb_ptr + (yuv_arraysize>>2);

#if 1 //Andy@20100223.  Use ping-pong buffer in FW.
    status = GetA8RegB(0x654B);
    if(status & 0x01){ //PIP buffer 1 is working. PIP buffer 0 is ready
        srcAddr = A800Data.Select->BufAddr;
    }
    else{//PIP buffer 0 is working. PIP buffer 1 is ready
        srcAddr = A800Data.Select->BufAddr + A800Data.Select->BufWidth*A800Data.Select->BufHeight*2;
    }    
#endif

    A8IndCmdP = HII_SET_ADDR_H;
    A8IndDataPW(0) = (u_short)(srcAddr>>16);
    A8IndCmdP = HII_SET_ADDR_L;
    A8IndDataPW(0) = (u_short)(srcAddr & 0xFFFF);
    A8IndCmdP = HII_RW_MEM_W_INC;
    
    for(loop_y=0;loop_y<yuvHeight;loop_y++){
        if(!(loop_y&0x1)){// if at even rows
            for(loop_x=0;loop_x<yuvWidth;loop_x += 2){
                data = A8IndDataPW(0);
                *y_ptr++  = (u_char)(data >> 8);
                *cb_ptr++ = (u_char)data;

                data = A8IndDataPW(0);
                *y_ptr++  = (u_char)(data >> 8);
                *cr_ptr++ = (u_char)data;
            }
        }
        else{//at odd rows
            for(loop_x=0;loop_x<yuvWidth;loop_x += 2){
                data = A8IndDataPW(0);
                *y_ptr++  = (u_char)(data >> 8);
                data = A8IndDataPW(0);
                *y_ptr++  = (u_char)(data >> 8);
            }        
        }
    }

    return retVal;
}
#endif


#ifdef SENSOR_VIF
void A800_FillJpegScaleUp(u_short JpegWidth, u_short JpegHeight, u_short InputWidth, u_short InputHeight, u_short	ScaleM, u_short ScaleN)
{
	u_int TempX0,TempX1;
	u_int TempY0,TempY1;  

//	u_short	TempCurZoom = SetA8RegB(0x4f15);
    TempX0 = (JpegWidth * ScaleN / ScaleM)+2;
    TempY0 = (JpegHeight * ScaleN / ScaleM)+2; 
	if((TempX0 > InputWidth) || (TempY0 > InputHeight)) {
		TempX0 = 126/ScaleM;
		ScaleM = TempX0 * ScaleM +1;
		ScaleN = TempX0 * ScaleN;
    	TempX0 = (JpegWidth * ScaleN / ScaleM)+2;
    	TempY0 = (JpegHeight * ScaleN / ScaleM)+2; 		
	}
    TempX1 = (InputWidth - TempX0) / 2 + 1;
    TempY1 = (InputHeight - TempY0) / 2 + 1;
    
    SetA8RegW(0x4f00, TempX1);
    SetA8RegW(0x4f02, TempX0 + TempX1);
    SetA8RegW(0x4f04, TempY1);
    SetA8RegW(0x4f06, TempY0 + TempY1);

    TempX1 = (((TempX0 -1 ) * ScaleM)+(ScaleN-1)) / ScaleN;
    TempY1 = (((TempY0 -1 ) * ScaleM)+(ScaleN-1)) / ScaleN;
    TempX0 = (TempX1 - JpegWidth) / 2 + 1;
    TempY0 = (TempY1 - JpegHeight) / 2 + 1; 
   	    
    SetA8RegW(0x4f08, TempX0);
    SetA8RegW(0x4f0a, TempX0 + JpegWidth -1);
    SetA8RegW(0x4f0c, TempY0);
    SetA8RegW(0x4f0e, TempY0 + JpegHeight -1);       
    SetA8RegB(0x4f15, ScaleM);
    SetA8RegB(0x4f16, ScaleN); 		    
}
void A800_FillJpegScaleDown(u_short JpegWidth, u_short JpegHeight, u_short InputWidth, u_short InputHeight, u_short ScaleM, u_short ScaleN)
{
	u_int Temp0,Temp1;
	
        Temp0 = InputWidth * ScaleN / ScaleM;
        Temp1 = InputHeight * ScaleN / ScaleM; 
        Temp0 = (Temp0 - JpegWidth) / 2 + 1;
        if(!(Temp0&1))  Temp0++;
        Temp1 = (Temp1 - JpegHeight) / 2 + 1;
        if(!(Temp1&1))  Temp1++; 
	    SetA8RegW(0x4f00, 1);
	    SetA8RegW(0x4f02, InputWidth);
	    SetA8RegW(0x4f04, 1);
	    SetA8RegW(0x4f06, InputHeight);        
	    SetA8RegW(0x4f08, Temp0);
	    SetA8RegW(0x4f0a, Temp0 + JpegWidth -1);
	    SetA8RegW(0x4f0c, Temp1);
	    SetA8RegW(0x4f0e, Temp1 + JpegHeight -1);       
	    SetA8RegB(0x4f15, ScaleN);
	    SetA8RegB(0x4f16, ScaleM);        
}

#define	MaxWidth	640
void A800_TakeJpegSetting(u_int JpegWidth, u_int JpegHeight)
{	
	extern	t_sensor_manager* gsSensorUsing;

	extern	u_short gA8CurrentZoomRate;
	u_char	CurZoomStep,ZoomBase;
	u_int	tmpCurZoomStep, Temp0, MaxN;
	u_int	timeout = 0;
	if(!gsSensorUsing)
		return;

	ZoomBase = A800Data.Select->UpScale;
	CurZoomStep =gA8CurrentZoomRate+ZoomBase;// GetA8RegB(0x4f15);
	SetA8RegB(0x6510,GetA8RegB(0x6510)&0xfb);	
	while( (GetA8RegB(0x5008)&0x1) && (timeout < 0x10000))
	{
		timeout++;
	};
	SetA8RegB(0x500A, GetA8RegB(0x500a)&0xfe);
	if((gsSensorUsing->preview_mode->image_width*JpegHeight)>(gsSensorUsing->preview_mode->image_height*JpegWidth)){
		Temp0 = ((120<<8)*JpegHeight) / (gsSensorUsing->preview_mode->image_height*ZoomBase);
		MaxN = Temp0 * gsSensorUsing->preview_mode->image_height * ZoomBase / JpegHeight;
	}
	else
	{
		Temp0 = ((120<<8)*JpegWidth) / (gsSensorUsing->preview_mode->image_width*ZoomBase);
		MaxN = Temp0 * gsSensorUsing->preview_mode->image_width * ZoomBase / JpegWidth;
	}
	tmpCurZoomStep = Temp0 * CurZoomStep;

	if(tmpCurZoomStep > MaxN)
	{
		gsSensorUsing->sensor_setting(SENSOR_FULL_RESOL_MODE);
		if((gsSensorUsing->full_mode->image_width*JpegHeight) > (gsSensorUsing->full_mode->image_height*JpegWidth)) {
			Temp0 = ((120<<8)*JpegHeight) / (gsSensorUsing->full_mode->image_height*ZoomBase);
			MaxN = Temp0 * gsSensorUsing->full_mode->image_height * ZoomBase / JpegHeight;
		}
		else
		{
			Temp0 = ((120<<8)*JpegWidth) / (gsSensorUsing->full_mode->image_width*ZoomBase);
			MaxN = Temp0 * gsSensorUsing->full_mode->image_width * ZoomBase / JpegWidth;
		}
		tmpCurZoomStep = Temp0 * CurZoomStep;
		if(MaxWidth < JpegWidth)
		{
			tmpCurZoomStep >>= 8;
			MaxN >>= 8;			
			if((((gsSensorUsing->full_mode->image_width*tmpCurZoomStep/MaxN))<JpegWidth)||(((gsSensorUsing->full_mode->image_height*tmpCurZoomStep/MaxN))<JpegHeight))
				tmpCurZoomStep++;		
			A800_FillJpegScaleDown(JpegWidth, JpegHeight, gsSensorUsing->full_mode->image_width, gsSensorUsing->full_mode->image_height, MaxN, tmpCurZoomStep);
		}
		else
		{
			if(tmpCurZoomStep > MaxN)
			{
				Temp0 = 120 * MaxN / tmpCurZoomStep;
				MaxN = Temp0 * tmpCurZoomStep / MaxN;
				tmpCurZoomStep = Temp0;
				gsSensorUsing->sensor_setting(SENSOR_SCALE_UP_MODE);
				if((((gsSensorUsing->full_mode->image_width*tmpCurZoomStep/MaxN))<JpegWidth)||(((gsSensorUsing->full_mode->image_height*tmpCurZoomStep/MaxN))<JpegHeight))
					tmpCurZoomStep++;		
				A800_FillJpegScaleUp(JpegWidth, JpegHeight, gsSensorUsing->full_mode->image_width, gsSensorUsing->full_mode->image_height, MaxN, tmpCurZoomStep);
			}
			else
			{
				tmpCurZoomStep >>= 8;
				MaxN >>= 8;			
				if((((gsSensorUsing->full_mode->image_width*tmpCurZoomStep/MaxN))<JpegWidth)||(((gsSensorUsing->full_mode->image_height*tmpCurZoomStep/MaxN))<JpegHeight))
					tmpCurZoomStep++;	
				
				A800_FillJpegScaleDown(JpegWidth, JpegHeight, gsSensorUsing->full_mode->image_width, gsSensorUsing->full_mode->image_height, MaxN, tmpCurZoomStep);
			}
		}		
	}
	else
	{
		gsSensorUsing->sensor_setting(SENSOR_NORMAL_CAP_MODE);
		tmpCurZoomStep >>= 8;
		MaxN >>= 8;			
		if((((gsSensorUsing->preview_mode->image_width*tmpCurZoomStep)/MaxN)<JpegWidth)||(((gsSensorUsing->preview_mode->image_height*tmpCurZoomStep)/MaxN)<JpegHeight))
			tmpCurZoomStep++;

		A800_FillJpegScaleDown(JpegWidth, JpegHeight, gsSensorUsing->preview_mode->image_width, gsSensorUsing->preview_mode->image_height, MaxN, tmpCurZoomStep);
	} 
		
	Temp0 = ((JpegWidth + 15) >>4)<<4;
	
	SetA8RegW(0x6200,Temp0);
	SetA8RegW(0x6202,JpegHeight);

	SetA8RegW(0x4f0a, GetA8RegW(0x4f0a)+(Temp0-JpegWidth));	
	AIT_Message_P2("x0=0x%x,x1=0x%x",GetA8RegW(0x4f08),GetA8RegW(0x4f0a));
	AIT_Message_P2("y0=0x%x,y1=0x%x\r\n",GetA8RegW(0x4f0c),GetA8RegW(0x4f0e));
	AIT_Message_P2("N=0x%x,M=0x%x\r\n",GetA8RegB(0x4f15),GetA8RegB(0x4f16));		

}

void A800_TakeJpegPicture( s_short PictSize, s_short ZoomRate, u_char Re_Flag )
{
   SetA8RegB( 0x6242, 0x03 );		// Clear JPEG frame encode done status
   SetA8RegW( 0x6222, 0x0000 );	// Clear JPEG FIFO

#ifdef EXIFSUPPORT
	gCurrentCaptureResolution = PictSize;
#endif

	SetA8RegB(0x690a,GetA8RegB(0x690a)|0x08);	//sw reset encode block
	SetA8RegB(0x690a,GetA8RegB(0x690a)&0xf7);
	SetA8RegB(0x6511,GetA8RegB(0x6511)|0x04);	//reset line buffer
	SetA8RegB(0x6511,GetA8RegB(0x6511)&0xfb);
	
	SetA8RegW(0x6210,0x3000);	//line buffer: 			0x13000	20K
	SetA8RegW(0x6212,0x0001);
	SetA8RegW(0x6214,0x0000);	//compress buffer start:	0x0000
	SetA8RegW(0x6216,0x0000);
	SetA8RegW(0x6218,0x2fff);	//compress buffer end:	0x12fff	76K
	SetA8RegW(0x621a,0x0001);

	SetA8RegB(0x6511,0x08);
	SetA8RegB(0x6222,0x80);
	SetA8RegB(0x6204,0x28);
	

//	   SendA8Cmd( (Re_Flag<<12) +(PictSize << 8) + A8_HOST_CMD_TAKE_JPEG );
	return;
}

A8_ERROR_MSG 	A800_TakeJPEG(u_char resolution, u_int maxFileSize, u_short *pdata, u_int *filesize)
{
	u_char retVal;

	A800_TakeJpegPicture(resolution, gA8PrevZoomFactor, 1);
	retVal = A8L_GetJpegPictureWithLimit((u_short *)pdata, maxFileSize, filesize);
	
	if (retVal != A8_NO_ERROR) // 0626 re-capture 
	{
		A800_JPEGQuality(gAITJpegQuality + 1);

		SetA8RegB(0x6511, 0x08);
		SetA8RegB(0x6020, 0xA0);
		A800_TakeJpegPicture(resolution, gA8PrevZoomFactor, 1);

		retVal = A8L_GetJpegPictureWithLimit((u_short *)pdata, maxFileSize, filesize);
		
		if (retVal != A8_NO_ERROR)
		{
			A800_JPEGQuality(4);
			SetA8RegB(0x6511, 0x08);
			SetA8RegB(0x6020, 0xA0);
			A800_TakeJpegPicture(resolution, gA8PrevZoomFactor, 1);
			retVal = A8L_GetJpegPictureWithLimit((u_short *)pdata, maxFileSize, filesize);
		
		if (retVal != A8_NO_ERROR)
		{
			return A8_CAM_ENCODE_ERROR;
			}	
		}
	}
	return A8_NO_ERROR;
}

void A800_LCD_Refresh_Pause(void)
{
	//s_int timeout;
	//u_short val;
	//u_short color = 0xff;
	/*  SetA8RegB(0x500a, 0x00);
	do
	{
		val=GetA8RegB(0x5008);
		sys_IF_ait_delay1us(1);
		timeout++;
	}while((val & 0x01) &&(timeout < A8_CMD_READY_TIME_OUT));


	SetA8RegW(0x5006, 0x02);//Index  ?index for writing to GRAM
	SetA8RegB(0x500a, 0x04);
	sys_IF_ait_delay1us(5);

	SetA8RegW(0x5006, 0x02);//Index  ?index for writing to GRAM
	SetA8RegB(0x500a, 0x04);
	sys_IF_ait_delay1us(5);


	for(timeout=0;timeout<176*220*2;++timeout)
	{
		SetA8RegW(0x5004, color);//Index  ? Reg[0x28] = 0x34
		SetA8RegB(0x500a, 0x08);
		sys_IF_ait_delay1us(5);
	}
	*/


	SetA8RegW(0x5006, 0x0007);//Index  ? Reg[0x28] = 0x34
	SetA8RegB(0x500a, 0x04);
	sys_IF_ait_delay1us(15);


	SetA8RegW(0x5004, 0x0003);//CMD  ? Reg[0x28] = 0x34
	SetA8RegB(0x500a, 0x08);
	sys_IF_ait_delay1us(1);


	//SetA8RegW(0x5006, 0x0202);//Index  ?index for writing to GRAM
	//SetA8RegB(0x500a, 0x04);
	//sys_IF_ait_delay1us(1);

}

void A800_LCD_Refresh_Resume(void)
{
	//s_int timeout;
	//u_short val;
	A8L_CheckSensorFrame(VIF_FRAME_END); 
	A8L_CheckLcdBusy();		
	SetA8RegW(0x5006, 0x0007);//Index  ? Reg[0x28] = 0x34
	SetA8RegB(0x500a, 0x04);
	sys_IF_ait_delay1us(1);

	SetA8RegW(0x5004, 0x0003);//CMD  ? Reg[0x28] = 0x34
	SetA8RegB(0x500a, 0x08);
	sys_IF_ait_delay1us(1);
	//A8L_CheckLcdBusy();	
	SetA8RegW(0x5006, 0x0007);//Index  ? Reg[0x28] = 0x34
	SetA8RegB(0x500a, 0x04);
	sys_IF_ait_delay1us(1);

	SetA8RegW(0x5004, 0x0001);//CMD  ? Reg[0x28] = 0x34
	SetA8RegB(0x500a, 0x04);
	sys_IF_ait_delay1us(1);
	SetA8RegW(0x5006, 0x0202);//Index  ?index for writing to GRAM
	SetA8RegB(0x500a, 0x04);
	sys_IF_ait_delay1us(1);
	A8L_CheckSensorFrame(VIF_FRAME_END);
	A8L_CheckLcdBusy();		
	SetA8RegB(0x500a, 0x01);
	A8L_CheckSensorFrame(VIF_FRAME_END);
	A8L_CheckLcdBusy();		
	SetA8RegB(0x500a, 0x00);	
}

#endif

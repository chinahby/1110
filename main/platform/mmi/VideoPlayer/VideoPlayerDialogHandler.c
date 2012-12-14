/*=================================================================================================================
文件：
      VideoPlayerDialogHandler.c
      版权所有(c) 2009 Anylook Tech. CO.,LTD.

 文件说明：
 作者:  
 创建日期：2008-10-06
 当前版本：Draft
=================================================================================================================*/

/*=================================================================================================================
修改历史记录：
时       间      修 改 人    问题单号  修改内容、位置及原因
-----------      ----------  --------  -----------------------------------------
=================================================================================================================*/


/*=================================================================================================================
                          本文件包含的外部文件
=================================================================================================================*/
#include "VideoPlayer_priv.h"
#ifndef WIN32
//#include "mmd_ply_driver.h"
#else
#include "oemhelperfunctype.h"
#endif//WIN32
#ifdef FEATURE_PROJECT_FLEXI203
	#include "appscommon_images160x128_Flexi203.brh"
#else
	#include "appscommonimages.brh"
#endif
//#include "appscommon_color.brh"
#include "appscommon.brh"

extern boolean b_is_GetFrame;

boolean        FullScreen = FALSE;
/*=================================================================================================================
                             宏定义和常数
=================================================================================================================*/
// 关闭对话框
#define CLOSE_DIALOG(DlgRet)  {    \
                                     pMe->m_eDlgRet = DlgRet; \
                                     (void) ISHELL_EndDialog(pMe->m_pShell);  \
                                 }


/*=================================================================================================================
                             类型定义
=================================================================================================================*/


/*=================================================================================================================
                              函数声明
=================================================================================================================*/

/*=================================================================================================================
                对话框IDD_VIDEOPLAYER事件处理函数
=================================================================================================================*/

static boolean VPDVideoPlayer_HandleEvent(CVideoPlayer *pMe,AEEEvent eCode, uint16 wParam,uint32 dwParam); 

/*=================================================================================================================
                   对话框IDD_OPTION事件处理函数
=================================================================================================================*/

//static boolean VPDOption_HandleEvent(CVideoPlayer *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam); 

/*=================================================================================================================
                   对话框IDD_MSGBOX事件处理函数
=================================================================================================================*/

static boolean VPDMSGBOX_HandleEvent(CVideoPlayer *pMe,AEEEvent eCode,uint16 wParam,uint32 dwParam);

/*=================================================================================================================
                             视频播放相关处理函
=================================================================================================================*/

//对话框IDD_VIDEOPLAYER key事件处理函数
static boolean VPDVideoPlayer_HandleKeyEvent(CVideoPlayer *pMe,AEEEvent eCode,uint16 wParam,uint32 dwParam);

//对话框IDD_OPTION key事件处理函数
//static boolean VPDOption_HandleKeyEvent(CVideoPlayer *pMe,AEEEvent eCode,uint16 wParam,uint32 dwParam);

//对话框IDD_MSGBOX key事件处理函数
//static boolean VPDMSGBOX_HandleKeyEvent(CVideoPlayer *pMe,AEEEvent eCode,uint16 wParam,uint32 dwParam);

//播放视频 
void VideoPlayer_PlayVideo(CVideoPlayer *pMe);

//画视频播放器相关图片
static void VideoPlayer_DrawImage(CVideoPlayer *pMe, char *resFile, int16 resId, int x, int y);

//播放下一个文件
static void VideoPlayer_PlayNext(CVideoPlayer *pMe, boolean bDirection); 

//防止快速按健
static void VideoPlayer_keybusy(CVideoPlayer *pMe);

//重画player屏的上半部分的文件名
static void VideoPlayer_RefreshPlayerFileName(CVideoPlayer *pMe);
static void VideoPlayer_ShowName(CVideoPlayer *pMe);

//重画player屏的下半部分的按钮部分
static void VideoPlayer_UpdateButton(CVideoPlayer *pMe);
static void VideoPlayer_UpdateFRButton(CVideoPlayer *pMe);

//刷新播放时间
static void VideoPlayer_RefreshPlayingTick(CVideoPlayer *pMe);

//刷新音量bar
static void VideoPlayer_RefreshVolBar(CVideoPlayer *pMe);

//刷新进度条
static void VideoPlayer_RefreshScheduleBar(CVideoPlayer *pMe);

// 删除当前文件
//static boolean VideoPlayer_DeleteVideo(CVideoPlayer *pMe);

// 接受CMX底层回调
static void VideoPlayer_VideoNotify(void * pUser, AEEMediaCmdNotify * pCmdNotify);

// 定时函数
static void DialogTimeoutCallback(void *pUser);

// 获取指定视频的名字
static int  VideoPlayer_GetFileID(CVideoPlayer *pMe);
static boolean VideoPlayer_ChangeScrState(CVideoPlayer* pMe,boolean isLockScr);

// 画标题
static void VideoPlayer_WriteTitleRes(CVideoPlayer *pMe,char* pResPath,int uResId);
static void VideoPlayer_WriteTitle(CVideoPlayer *pMe,AECHAR* pText);

// 帮助
static void VideoPlayer_Help(CVideoPlayer* pMe);
static boolean VideoPlayer_HelpInit(CVideoPlayer* pMe);
static void VideoPlayer_HelpTerminate(CVideoPlayer *pMe);
static boolean VideoPlayer_HelpHandleEvent(CVideoPlayer *pMe,AEEEvent eCode,uint16 wParam,uint32 dwParam);
static void VideoPlayer_DrawHelp(CVideoPlayer *pMe);
static uint16 VideoPlayer_SearchKey(uint16 wParam);





/*=================================================================================================================
                              全局数据
=================================================================================================================*/

/*=================================================================================================================
                             本地（静态）数据
=================================================================================================================*/


/*=================================================================================================================
                                函数定义
=================================================================================================================*/

/*=================================================================================================================
函数：VideoPlayer_ShowDialog

说明：
       函数由状态处理函数用来创建相应对话框资源ID的对话框。

参数：
       pMe [in]：指向VideoPlayer Applet对象结构的指针。该结构包含小程序的特定信息。
       dlgResId [in]：对话框资源ID，函数根据该ID创建对话框。

返回值：
        无

备注：:

=================================================================================================================*/
int VideoPlayer_ShowDialog(CVideoPlayer *pMe , uint16 dlgResId)
{
    int nRet;
    
       // At most one dialog open at once
       if (ISHELL_GetActiveDialog(pMe->m_pShell) != NULL)
       {
           // Looks like there is one dialog already opened.
           // Flag an error an return without doing anything.
   #if defined(AEE_STATIC)
           ERR("Trying to create a dialog without closing the previous one",0,0,0);
   #endif
           return  EFAILED;
       } 
       
       nRet = ISHELL_CreateDialog(pMe->m_pShell,AEE_APPSVIDEOPLAYER_RES_FILE,dlgResId,NULL);    
    
   #if defined(AEE_STATIC)
           if (nRet != SUCCESS)
           {
               ERR("Failed to create the dialog in the videoplayer applet",0,0,0);
           }
   #endif
           return  nRet;

}

/*=================================================================================================================
函数:
    VideoPlayer_ShowMsgBox

说明:
    函数由状态处理函数用来唤起消息对话框，显示消息提示用户。

参数:
    pMe [in]: 指向 VIDEOPLAYER Applet 对象结构的指针。该结构包含小程序的特定信息。
    wTextResId [in]: 消息文本资源 ID。

返回值:
    none

备注:

=================================================================================================================*/
int  VideoPlayer_ShowMsgBox(CVideoPlayer *pMe, uint16 wTextResId)
{
    pMe->m_nMsgResID = wTextResId;
     
    // 创建消息对话框.
    return  VideoPlayer_ShowDialog(pMe, IDD_MSGBOX);
}

/*=================================================================================================================
函数：VideoPlayer_RouteDialogEvent

说明：
       函数将BREW事件路由给当前活动对话框的事件处理函数。

参数：
       pMe [in]：指向VideoPlayer Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       dwParam [in]：与事件eCode关联的数据。。

返回值：
        TRUE：传入事件被处理。
        FALSE：传入事件没被处理。

备注：:

=================================================================================================================*/
boolean VideoPlayer_RouteDialogEvent( CVideoPlayer *pMe,AEEEvent eCode,uint16 wParam,uint32 dwParam)
{
    #if defined(AEE_STATIC)
        if (NULL == pMe)
        {
            return FALSE;
        }
    #endif
    
    if (NULL == pMe->m_pActiveDlg)//当前活动对话框IDialog接口指针
    {
        return FALSE;
    }
    switch (pMe->m_pActiveDlgID)
    {     
        case IDD_VIDEOPLAYER:
            return VPDVideoPlayer_HandleEvent(pMe,eCode,wParam,dwParam);

        case IDD_MSGBOX:
            return VPDMSGBOX_HandleEvent(pMe,eCode,wParam,dwParam);
#if 0
        case IDD_OPTION:
            return VPDOption_HandleEvent(pMe,eCode,wParam,dwParam);
#endif  
        default:
            return FALSE;
     }
}




/*=================================================================================================================
函数：VPDVideoPlayer_HandleEvent

说明：
       VideoPlayer Applet 主界面对话框IDD_VIDEOPLAYER事件处理函数。

参数：
       pMe [in]：指向VideoPlayer Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件参数
       dwParam [in]：与wParam关联的数据。。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件没被处理。

备注：
=================================================================================================================*/
static  boolean VPDVideoPlayer_HandleEvent(CVideoPlayer *pMe,AEEEvent eCode,uint16 wParam,uint32 dwParam)
{   
    #if defined(AEE_STATIC)
        ASSERT(pMe != NULL);
    #endif
	MSG_FATAL("VPDVideoPlayer_HandleEvent eCode = 0x%x,wParam=%d,dwParam=%d",eCode,wParam,dwParam);
#ifdef VIDEOPLAYER_HELP
	if(pMe->m_pHelp != NULL)
	{
	   boolean nReturn = FALSE;
	   nReturn = VideoPlayer_HelpHandleEvent(pMe,eCode,wParam,dwParam);

	   if(nReturn) return TRUE;
	}
#endif

    switch (eCode)
    {
        case EVT_DIALOG_INIT:        
            return TRUE;
        
        case EVT_DIALOG_START:
            (void) ISHELL_PostEvent(pMe->m_pShell,AEECLSID_VIDEOPLAYER,EVT_USER_REDRAW,0,0);                                   
            return TRUE;
        
        case EVT_USER_REDRAW:
			{
		    AEERect rc = {0,0,SCR_W,SCR_H};   
			IDisplay_FillRect(pMe->m_pDisplay,&rc,0x0);
#if defined(FEATURE_DISP_128X160)
            if(!pMe->IsPlay)
            {
                VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE,IDI_PLAYERPICTURE_PLAY, 0, 0);                 
            }
            else
            {					
                VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE,IDI_PLAYERPICTURE_PAUSE, 0, 0);                
            }         
#endif		
			VideoPlayer_WriteTitleRes(pMe,AEE_APPSVIDEOPLAYER_RES_FILE,IDS_TITLE);
			#ifndef FEATURE_DISP_128X160
			VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE,IDI_LOGO, VIDEOPLAYER_LOGO_X, VIDEOPLAYER_LOGO_Y);
			#endif
            if(pMe->m_FileToPlay != NULL) // 如果不加此判断条件，则在pMe->m_FileToPlay为NULL时，屏幕上会显示乱码
            {
                VideoPlayer_RefreshPlayerFileName(pMe); //刷新文件名
            }
			#if !defined (FEATURE_DISP_240X320)&& !defined(FEATURE_DISP_220X176) && !defined(FEATURE_DISP_176X220)
            VideoPlayer_RefreshVolBar(pMe);
			#endif
            if(!pMe->IsPause && !pMe->IsPlay)                 
            {
                if(!pMe->UserStop)
                {
 #ifdef FEATURE_VERSION_C337
 					DRAW_BOTTOMBAR(BTBAR_GALLERY_HELP_BACK);
 #else
                    DRAW_BOTTOMBAR(BTBAR_GALLERY_BACK);
 #endif
                }
                else
                {
                    if(pMe->m_IsPlaynext)
                    {
                        #if defined(FEATURE_DISP_176X220)
                         if(pMe->IsPause) 
                         {
                              VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE,IDI_PLAY, VIDEOPLAYER_PLAY_X,VIDEOPLAYER_PLAY_Y); //将中间图标刷新为三角形的小图标
                         }           
                        #endif	
                        DRAW_BOTTOMBAR(BTBAR_GALLERY_PLAY_BACK);
                    }
                    else
                    {
                        DRAW_BOTTOMBAR(BTBAR_PLAY_BACK);
                    }
                }
            }
            if(pMe->IsPause)
            {
                DRAW_BOTTOMBAR(BTBAR_PLAY_STOP);
            }
            if(pMe->IsPlay)
            {
                DRAW_BOTTOMBAR(BTBAR_FULLSCREEN_PAUSE_STOP);
            } 
		//	VideoPlayer_RefreshPlayingTick(pMe);
            IDISPLAY_Update(pMe->m_pDisplay); 
            if(pMe->Is848Busy)
            {
                CLOSE_DIALOG(DLGRET_MSGBOX);
            }          
            if(pMe->IsGallery)
            {
                if(pMe->m_InitFailed != SUCCESS )
                {                    
                    CLOSE_DIALOG(DLGRET_MSGBOX);
                }
                else if(pMe->IsPlay)
                { 
                    VideoPlayer_PlayVideo(pMe);
                }
            } 
            return TRUE;
        	}
  
        case EVT_DIALOG_END:            
            return TRUE;
        
        case EVT_KEY: 
            if(pMe->IsGallery)
            {
                return  VPDVideoPlayer_HandleKeyEvent(pMe,eCode,wParam,dwParam); 
            }
            else 
            {
                switch(wParam)
                {
                    case AVK_SELECT:
                        #if 0
                        //启动Mdeida Gallery applet                   
                        if (!ISHELL_CanStartApplet(pMe->m_pShell, AEECLSID_MEDIAGALLERY)) 
                        {
                            //当前不能启动指定 Applet
                            DBGPRINTF("ERR!!!!!");
                            return FALSE;
                        }
                        if (SUCCESS != ISHELL_StartApplet(pMe->m_pShell, AEECLSID_MEDIAGALLERY)) 
                        {
                            DBGPRINTF("RIGHT~~~~~~");
                            return FALSE;
                        }
                        #endif
                        CMediaGallery_FileExplorer(GALLERY_VIDEO_ADDLIST,NULL);
                        return TRUE;
#ifdef VIDEOPLAYER_HELP
					case AVK_INFO:
						{
							VideoPlayer_Help(pMe);
							return TRUE;
						}
#endif
                    case AVK_CLR:
                        CLOSE_DIALOG(DLGRET_CANCELED);
                        break;
                        
                    default:
                        return TRUE;
                }
                     
            }

        case EVT_APPISREADY: 
            pMe->m_bAppIsReady  = TRUE;
            return TRUE;
           
        case EVT_DISPLAYDIALOGTIMEOUT:
            CLOSE_DIALOG(DLGRET_CANCELED);
            return TRUE;
           
        case EVT_COMMAND:
            return TRUE;
        case EVT_NO_CLOSEBACKLIGHT:
            if(pMe->IsPlay)
            {
                return TRUE;
            }
            break;
#ifdef FEATURE_LCD_TOUCH_ENABLE//WLH ADD FOR LCD TOUCH
		case EVT_PEN_UP:
			{
				int i;
				AEEDeviceInfo devinfo;
				int nBarH ;
				AEERect rc;
				int16 wXPos = (int16)AEE_GET_X((const char *)dwParam);
				int16 wYPos = (int16)AEE_GET_Y((const char *)dwParam);
				
				nBarH = GetBottomBarHeight(pMe->m_pDisplay);
				MEMSET(&devinfo, 0, sizeof(devinfo));
				ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
				SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, devinfo.cyScreen);
				if ((wXPos>0)&&(wXPos<devinfo.cxScreen/3)&&(wYPos>rc.y)&&(wYPos<devinfo.cyScreen))
				{
					boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_VIDEOPLAYER,EVT_USER,AVK_SELECT,1);
					return rt;
				}
				else if ((wXPos>devinfo.cxScreen/3)&&(wXPos<(devinfo.cxScreen/3)*2)&&(wYPos>rc.y)&&(wYPos<devinfo.cyScreen))
				{
					boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_VIDEOPLAYER,EVT_USER,AVK_INFO,0);
					 return rt;
				}
				else if ((wXPos>wXPos<(devinfo.cxScreen/3)*2)&&(wXPos<devinfo.cxScreen)&&(wYPos>rc.y)&&(wYPos<devinfo.cyScreen))
				{
					boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_VIDEOPLAYER,EVT_USER,AVK_CLR,0);
					return rt;
				}
			}
			return TRUE;
#endif//FEATURE_LCD_TOUCH_ENABLE       
        default:
               break;
    }//switch (eCode)
        return FALSE;
}

/*=================================================================================================================

函数：VPDOption_HandleEvent

说明：
       VideoPlayer Applet 主界面对话框IDD_OPTION事件处理函数。

参数：
       pMe [in]：指向VideoPlayer Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件参数
       dwParam [in]：与wParam关联的数据。。

返回值：
        TRUE：传入事件被处理。
        FALSE：传入事件没被处理。

备注：:

================================================================================================================*/
#if 0
static boolean VPDOption_HandleEvent(CVideoPlayer *pMe,AEEEvent eCode,uint16 wParam,uint32 dwParam)
{   
    
    IMenuCtl* pMenuOption = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,
                                                          IDC_OPTION_MENU);
    if( pMenuOption == NULL)
    {
        return FALSE;
    }

    switch (eCode)
    {     
        case EVT_DIALOG_INIT:  
            if(pMe->IsPlay)
            {
                IMENUCTL_AddItem(pMenuOption,AEE_APPSCOMMONRES_LANGFILE,IDS_PAUSE, IDS_PAUSE,0,0);
                IMENUCTL_AddItem(pMenuOption,AEE_APPSCOMMONRES_LANGFILE,IDS_FULLSCREEN,IDS_FULLSCREEN,0,0);
            }
            if(pMe->IsPause)
            {
                IMENUCTL_AddItem(pMenuOption,AEE_APPSCOMMONRES_LANGFILE,IDS_PLAY,IDS_PLAY,0,0); 
            }
            IMENUCTL_AddItem(pMenuOption,AEE_APPSVIDEOPLAYER_RES_FILE,IDS_DELETE, IDS_DELETE,0,0);
            
            return TRUE;
         
        case EVT_DIALOG_START:        
            IMENUCTL_SetPopMenuRect(pMenuOption);
            IMENUCTL_SetBottomBarType(pMenuOption, BTBAR_SELECT_BACK);
            (void) ISHELL_PostEvent(pMe->m_pShell,AEECLSID_VIDEOPLAYER,EVT_USER_REDRAW,0,0);
            return TRUE;

        case EVT_USER_REDRAW:
            (void)IMENUCTL_Redraw( pMenuOption);       
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:            
            return VPDOption_HandleKeyEvent(pMe,eCode,wParam,dwParam); 

        case EVT_DISPLAYDIALOGTIMEOUT:
            CLOSE_DIALOG(DLGRET_CANCELED);
            return TRUE;

        //自定义控件事件 ,菜单按下时发生
        case EVT_COMMAND:        
            pMe->m_nItemID=wParam;
            switch(wParam)
            {      
                case IDS_PLAY:
                    if(SUCCESS==pMe->m_InitFailed)
                    {    
                        VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE, IDI_PAUSE, 82, 179); //"||" 
                        //刷新屏幕，包括时间和进度条
                        //VideoPlayer_RefreshPlayingTick(pMe);//刷新时间
                        //VideoPlayer_RefreshScheduleBar(pMe);//刷新进度条                                       
                        CLOSE_DIALOG(DLGRET_CANCELED);
                        if(pMe->IsPause)
                        {
                            ISHELL_SetTimer(pMe->m_pShell,150, (PFNNOTIFY)(IMEDIA_Resume),pMe->m_pMedia); 
                        }
                        else
                        {
                            ISHELL_SetTimer(pMe->m_pShell,150, (PFNNOTIFY)VideoPlayer_PlayVideo,pMe);                            
                        }
                        pMe->IsPlay  = TRUE;
                        pMe->IsPause=FALSE;
                        pMe->UserStop=FALSE; 
 
                    } 
                    else CLOSE_DIALOG(DLGRET_MSGBOX);
                    break; 
                    
                case IDS_PAUSE:
                    VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE, IDI_PLAY, 82,179); //"|>"
                    CLOSE_DIALOG(DLGRET_CANCELED);
                    
                    //if(!pMe->PauseLock)//pMe->bTotalTime == pMe->bCurrentTime || pMe->bCurrentTime == 0时,pMe->PauseLock=TRUE，
                                       //所以该判断条件暂不能用
                    //{                        
                        ISHELL_SetTimer(pMe->m_pShell,150, (PFNNOTIFY)(IMEDIA_Pause),pMe->m_pMedia);
                        pMe->IsPause = TRUE;
                        pMe->IsPlay = FALSE;
                        pMe->UserStop = FALSE;                            
                        
                    //} 
                    break;
                  
                case IDS_DELETE:                    
                    if(pMe->IsPlay)
                    {   
                        CLOSE_DIALOG(DLGRET_MSGBOX);
                        
                        ISHELL_SetTimer(pMe->m_pShell,1000, (PFNNOTIFY)(IMEDIA_Pause),pMe->m_pMedia);                                              
                        pMe->IsPause = TRUE;
                        pMe->IsPlay = FALSE;                        
                    }
                    //CLOSE_DIALOG(DLGRET_MSGBOX);
                    break;
                    
                case IDS_FULLSCREEN:
                    CLOSE_DIALOG(DLGRET_CANCELED);                    
                    VideoPlayer_PlayMod(pMe, 0);
                    break;
                     
                default:
                    break;
            }
            return TRUE;
      
        default:
            break;
    }
        return FALSE;
}
#endif
/*=================================================================================================================
函数：VPDMSGBOX_HandleEvent

说明：
       VideoPlayer Applet 主界面对话框IDD_DELETE事件处理函数。

参数：
       pMe [in]：指向VideoPlayer Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件参数
       dwParam [in]：与wParam关联的数据。。

返回值：
        TRUE：传入事件被处理。
        FALSE：传入事件没被处理。

备注：:

=================================================================================================================*/

static boolean  VPDMSGBOX_HandleEvent(CVideoPlayer *pMe,AEEEvent eCode,uint16 wParam,uint32 dwParam)
{   
    static  IStatic * pStatic = NULL;
    AECHAR  wstrText[128];
    int     set_timer=0;
    
    if (NULL == pMe)
    {
        return FALSE;
    }

    switch(eCode)
    {
        case EVT_DIALOG_INIT:
            if (NULL == pStatic)
            {
                AEERect rect = {0};
                if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,AEECLSID_STATIC,(void **)&pStatic))
                {
                    return FALSE;
                }
                ISTATIC_SetRect(pStatic, &rect);
            }
            return TRUE;
        

        case EVT_DIALOG_START: 
            ISHELL_PostEvent(pMe->m_pShell, AEECLSID_VIDEOPLAYER,EVT_USER_REDRAW,  0, 0);
            return TRUE;        

        case EVT_USER_REDRAW:            
            {
                PromptMsg_Param_type PromptMsg={0}; 
    
                if(pMe->m_nMsgResID==IDS_PLAYFAILED || pMe->m_nMsgResID==IDS_PLAYFORMATFAILD)
                {
                    DBGPRINTF("message");
                    PromptMsg.ePMsgType = MESSAGE_ERR;
                    set_timer=3000;//SETTIME_MIDDLE; // 1000
                    (void)ISHELL_LoadResString(pMe->m_pShell, AEE_APPSVIDEOPLAYER_RES_FILE, pMe->m_nMsgResID, wstrText, sizeof(wstrText));
                    PromptMsg.pwszMsg = wstrText;
                    DrawPromptMessage(pMe->m_pDisplay, pStatic, &PromptMsg);
                    IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE);
                    (void)ISHELL_CancelTimer(pMe->m_pShell, NULL, pMe);
                     //启动发送关闭对话框事件的定时器               
                    ISHELL_SetTimer(pMe->m_pShell,set_timer,DialogTimeoutCallback,pMe);
                }
                else 
                {  
                    PromptMsg.ePMsgType =MESSAGE_CONFIRM;
                    PromptMsg.eBBarType = BTBAR_OK_BACK;
                    (void)ISHELL_LoadResString(pMe->m_pShell, AEE_APPSCOMMONRES_LANGFILE, pMe->m_nMsgResID, wstrText, sizeof(wstrText));                    
                    PromptMsg.pwszMsg = wstrText;
                    DrawPromptMessage(pMe->m_pDisplay, pStatic, &PromptMsg);
                    IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE);
                    (void)ISHELL_CancelTimer(pMe->m_pShell, NULL, pMe);
                } 
            }        
            return TRUE; 

        case EVT_DIALOG_END:        
            ISHELL_CancelTimer(pMe->m_pShell, DialogTimeoutCallback,pMe);
            ISTATIC_Release(pStatic);
            pStatic = NULL;
            //FREE(wstrText);
            return TRUE;
        
        case EVT_KEY:           
            if(pMe->m_nMsgResID != IDS_PLAYFAILED || pMe->m_nMsgResID!=IDS_PLAYFORMATFAILD)
            {
                switch(wParam)
                {
                    case AVK_SELECT:
                        pMe->Is848Busy = FALSE; 
                        if(app_media_scheduler()==APP_MEDIA_IMPACT_BY_FM)
                        {                        
                            ISHELL_SendEvent(pMe->m_pShell, AEECLSID_APP_FMRADIO, EVT_CLOSEAPP, 0, 0);
                        }
                        if(app_media_scheduler()==APP_MEDIA_IMPACT_BY_MP3)
                        {                           
                            ISHELL_SendEvent(pMe->m_pShell, AEECLSID_APP_MUSICPLAYER, EVT_CLOSEAPP, 0, 0);
                        } 
                        //初始化Media指针并注册回调
                        //VideoPlayer_InitVideo(pMe); 
                        CLOSE_DIALOG(DLGRET_VIDEOPLAYER);
                        break;
                        
                    case AVK_CLR:
                        CLOSE_DIALOG(DLGRET_CANCELED);
                        break;
                        
                    default:
                        break;
                }
            }          
            //return VPDMSGBOX_HandleKeyEvent(pMe,eCode,wParam,dwParam);            

        case EVT_DISPLAYDIALOGTIMEOUT:            
            if(pMe->m_nMsgResID==IDS_PLAYFAILED|| pMe->m_nMsgResID==IDS_PLAYFORMATFAILD)
            {
                CLOSE_DIALOG(DLGRET_CANCELED);
            }
#if 0
            if(pMe->m_nMsgResID==IDS_DEL || pMe->m_nMsgResID==IDS_DONE)
            {                
                CLOSE_DIALOG(DLGRET_VIDEOPLAYER);
            }  
#endif 
            return TRUE; 

        default:
            break;
    }
    return FALSE;
}
/*=================================================================================================================  
  处理视频播放的按钮事件
=================================================================================================================*/
#ifndef WIN32
extern /*MMD_PLYFILE*/uint32 pFileHandle;
#endif//WIN32
static boolean VPDVideoPlayer_HandleKeyEvent(CVideoPlayer *pMe,AEEEvent eCode,uint16 wParam ,uint32 dwParam)
{   
    if(pMe->IsFullScreen)
    {
		return VideoPlayer_PlayMod(pMe,wParam);
    }
    switch(wParam)
    {   
        //播放或暂停	
		case AVK_INFO:
            // add by pyuangui 20121203
			#ifdef FEATURE_VERSION_W317A
			if(pMe->m_keybusy)
			{
                 return TRUE;
			}
			#else
            (void)ISHELL_CancelTimer(pMe->m_pShell, NULL, pMe);
			#endif
			//add end
            if(pMe->m_InitFailed != SUCCESS)
            {
                CLOSE_DIALOG(DLGRET_MSGBOX);
            }
            else // 初始化成功
            {
                if(pMe->bTotalTime == 0)
                {
                  CLOSE_DIALOG(DLGRET_MSGBOX);
                  return FALSE;
                }
                if(!pMe->IsFullScreen)// 正常模式
                {                   
                    if(!pMe->IsPlay) // 非播放状态
                    {    
#ifdef FEATURE_DISP_128X160
                        VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE, IDI_PAUSE, VIDEOPLAYER_PLAY_X, VIDEOPLAYER_PLAY_Y); //"||"
#endif
						pMe->m_rtype = TYPE_PLAYER;

                        DRAW_BOTTOMBAR(BTBAR_FULLSCREEN_PAUSE_STOP);

                        IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE);

                        if(pMe->IsPause)// 当前状态为暂停时
                        {                             
                            IMEDIA_Resume((IMedia*)pMe->m_pMedia);
							#if defined (FEATURE_DISP_240X320)||defined(FEATURE_DISP_220X176) ||defined(FEATURE_DISP_176X220)
                            VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE, IDI_SELECT_PAUSE, VIDEOPLAYER_PLAY_X, VIDEOPLAYER_PLAY_Y);
                            ISHELL_SetTimer(pMe->m_pShell,50,(PFNNOTIFY)(IMEDIA_Resume),pMe->m_pMedia);
                            (void) ISHELL_SetTimer(pMe->m_pShell, 50, (PFNNOTIFY)VideoPlayer_UpdateButton, pMe);
							#endif
                            SetDeviceState(DEVICE_TYPE_MP4,DEVICE_MP4_STATE_ON);                              
                        }
                        else  
                        {
                            VideoPlayer_RefreshPlayerFileName(pMe); //刷新文件名
                            VideoPlayer_PlayVideo(pMe);
                            //ISHELL_SetTimer(pMe->m_pShell,50,(PFNNOTIFY)VideoPlayer_PlayVideo,pMe);
                        }                        
                        
                        pMe->IsPlay=TRUE;
                        pMe->IsPause=FALSE;
                        pMe->UserStop=FALSE; 
                        pMe->m_bAppIsReady=TRUE;
                        videoplayer_play_flag = TRUE;
                    }
                    else// 当前状态为播放时
                    {  
#ifdef FEATURE_DISP_128X160                  
                        VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE, IDI_PLAY, VIDEOPLAYER_PLAY_X,VIDEOPLAYER_PLAY_Y); //"|>"
#endif
						pMe->m_rtype = TYPE_PLAYER;
                        DRAW_BOTTOMBAR(BTBAR_PLAY_STOP); 
                        //IMEDIA_Pause(pMe->m_pMedia); // 不设时间的话，底条刷不出来
                        VideoPlayer_RefreshPlayerFileName(pMe); //刷新文件名      
						#if defined (FEATURE_DISP_240X320)||defined(FEATURE_DISP_220X176)||defined(FEATURE_DISP_176X220) 
                        VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE, IDI_SELECT_PLAY, VIDEOPLAYER_PLAY_X,VIDEOPLAYER_PLAY_Y);
                        (void) ISHELL_SetTimer(pMe->m_pShell, 50, (PFNNOTIFY)VideoPlayer_UpdateButton, pMe);
						#endif
                        IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE);
                        ISHELL_SetTimer(pMe->m_pShell,50,(PFNNOTIFY)(IMEDIA_Pause),pMe->m_pMedia); 
                        SetDeviceState(DEVICE_TYPE_MP4,DEVICE_MP4_STATE_OFF);  
                        pMe->IsPlay=FALSE;
                        pMe->IsPause=TRUE;
                        pMe->UserStop=FALSE; 
                        pMe->m_bAppIsReady=TRUE;
                    }               
                }
                else // 全屏模式
                {                  
                    SetDeviceState(DEVICE_TYPE_MP4,DEVICE_MP4_STATE_ON);
                    if(!pMe->IsPlay) // 非播放状态
                    { 
                        if(pMe->IsPause)// 当前状态为暂停时
                        {
                            IMEDIA_Resume((IMedia*)pMe->m_pMedia);
                            //ISHELL_SetTimer(pMe->m_pShell,50,(PFNNOTIFY)(IMEDIA_Resume),pMe->m_pMedia);
                            
                        }
                        else// 当前状态为停止时
                        {                            
                            VideoPlayer_PlayVideo(pMe);
                            //ISHELL_SetTimer(pMe->m_pShell,50,(PFNNOTIFY)VideoPlayer_PlayVideo,pMe); 
                        }
                        pMe->IsPlay=TRUE;;
                        pMe->IsPause=FALSE;
                        pMe->UserStop=FALSE;
                        pMe->m_bAppIsReady=TRUE;
                        videoplayer_play_flag = TRUE;
                    }
                    else // 当前状态为播放时
                    {   
                        IMEDIA_Pause((IMedia*)pMe->m_pMedia);
                        //ISHELL_SetTimer(pMe->m_pShell,50,(PFNNOTIFY)(IMEDIA_Pause),pMe->m_pMedia); 
                        
                        pMe->IsPlay=FALSE;
                        pMe->IsPause=TRUE;
                        pMe->UserStop=FALSE;
                        pMe->m_bAppIsReady=TRUE;
                    }
                }                
            }
            return TRUE; 
       
        case AVK_CLR:            
            (void)ISHELL_CancelTimer(pMe->m_pShell, NULL, pMe);
            /*在暂停或播放状态按stop键后，oem层都会传上来一个MM_STATUS_DONE回调事件，
               此时的UI已在回调函数内重绘，此处不做处理*/
            if(!pMe->IsFullScreen)//非全屏状态
            {
                
                if(pMe->IsPause || pMe->IsPlay)
                { 
#if 0                
                    VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE,IDI_PLAY, VIDEOPLAYER_PLAY_X,VIDEOPLAYER_PLAY_Y); //将中间图标刷新为三角形的小图标      
#endif
                    AEERect rc = {0,0,SCR_W,SCR_H}; 
                    IDisplay_FillRect(pMe->m_pDisplay,&rc,0x0);
                    #if defined FEATURE_DISP_240X320
                    VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE,IDI_PLAY, VIDEOPLAYER_PLAY_X,VIDEOPLAYER_PLAY_Y); //将中间图标刷新为三角形的小图标
                 // VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE, IDI_BEFORE, VIDEOPLAYER_PREVIOUS_X, VIDEOPLAYER_PREVIOUS_Y); //画按键按下去的小图标
                 // VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE, IDI_NEXT, VIDEOPLAYER_NEXT_X, VIDEOPLAYER_NEXT_Y); //画按键按下去的小图标
        		    VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE, IDI_ADD, VIDEOPLAYER_VOLUME_ADD_X,VIDEOPLAYER_VOLUME_ADD_Y);   
		            VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE, IDI_LOW, VIDEOPLAYER_VOLUME_LOW_X,VIDEOPLAYER_VOLUME_LOW_Y);
                    #elif defined(FEATURE_DISP_220X176)
                    VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE,IDI_PLAY, VIDEOPLAYER_PLAY_X,VIDEOPLAYER_PLAY_Y); //将中间图标刷新为三角形的小图标
                    #elif defined(FEATURE_DISP_176X220)
                    VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE,IDI_PLAY, VIDEOPLAYER_PLAY_X,VIDEOPLAYER_PLAY_Y); //将中间图标刷新为三角形的小图标
                    #else
                    #endif
                    VideoPlayer_WriteTitleRes(pMe,AEE_APPSVIDEOPLAYER_RES_FILE,IDS_TITLE);
					#ifndef FEATURE_DISP_128X160
                    VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE,IDI_LOGO, VIDEOPLAYER_LOGO_X, VIDEOPLAYER_LOGO_Y);
					#endif

					pMe->m_rtype = TYPE_PLAYER;
                    if(pMe->m_IsPlaynext)
                    {
                        DRAW_BOTTOMBAR(BTBAR_GALLERY_PLAY_BACK);
                    }
                    else
                    {
                        DRAW_BOTTOMBAR(BTBAR_PLAY_BACK);
                    }                
                    pMe->bCurrentTime=0; //将当前播放时间设为0，总时间不变 
                    pMe->bOldTime = 0;
                    VideoPlayer_RefreshPlayingTick(pMe);//刷新时间
                    VideoPlayer_RefreshScheduleBar(pMe);//进度条置在初始位置，不刷新文件名和其他设置
                    VideoPlayer_RefreshVolBar(pMe);// 刷新音量
                    IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE);
                    pMe->UserStop=TRUE;
                    pMe->IsPlay=FALSE;
                    pMe->IsPause=FALSE;
                    pMe->m_bAppIsReady=FALSE;
                   
                    if(pMe->m_pMedia)
					{
						IMEDIA_Stop((IMedia*)pMe->m_pMedia);
					}			
					//VideoPlayer_ChangeScrState(pMe,FALSE);
                    SetDeviceState(DEVICE_TYPE_MP4,DEVICE_MP4_STATE_OFF);
                    //ISHELL_SetTimer(pMe->m_pShell,50,(PFNNOTIFY)(IMEDIA_Stop),pMe->m_pMedia);                                    
                }
                else
                {
                    VideoPlayer_ReleaseVideo(pMe);
                    CLOSE_DIALOG(DLGRET_CANCELED);
                } 
            }            
            else //全屏状态下
            {
                {   
				// YY TODO:
                    //MMD_LCDRotate(0);
                    AEERect rc = {0,0,SCR_W,SCR_H};
                    pMe->IsFullScreen = FALSE;
		            FullScreen = FALSE;
							
					IDisplay_FillRect(pMe->m_pDisplay,&rc,0x0);
					VideoPlayer_WriteTitleRes(pMe,AEE_APPSVIDEOPLAYER_RES_FILE,IDS_TITLE);
					#ifndef FEATURE_DISP_128X160
					VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE,IDI_LOGO, VIDEOPLAYER_LOGO_X, VIDEOPLAYER_LOGO_Y);
					#endif

#ifdef FEATURE_DISP_128X160                   
                    VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE,IDI_PLAYERPICTURE_PLAY, 0, 0);//背景图
#endif
                    if( pMe->IsPlay)
                    { 
#ifdef FEATURE_DISP_128X160                    
                        VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE, IDI_PAUSE, VIDEOPLAYER_PLAY_X, VIDEOPLAYER_PLAY_Y); // "||"   
#endif
						pMe->m_rtype = TYPE_PLAYER;
                        DRAW_BOTTOMBAR(BTBAR_FULLSCREEN_PAUSE_STOP);
                        
                    }
                    else if(pMe->IsPause)
                    {

#ifndef WIN32
						// YY TODO:
                        //MMD_Player_File_Pause(pFileHandle);     //add by wangjian 2009.2.5
#endif//WIN32
                        SetDeviceState(DEVICE_TYPE_MP4,DEVICE_MP4_STATE_OFF);
#if 0
                        VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE, IDI_PLAY, VIDEOPLAYER_PLAY_X, VIDEOPLAYER_PLAY_Y); //"|>"
#endif
						pMe->m_rtype = TYPE_PLAYER;
                        DRAW_BOTTOMBAR(BTBAR_PLAY_STOP);
                        
                    }
                    else
                    {
#if 0                   
                        VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE, IDI_PLAY, VIDEOPLAYER_PLAY_X, VIDEOPLAYER_PLAY_Y); //"|>"
#endif
						pMe->m_rtype = TYPE_PLAYER;
                        if(pMe->m_IsPlaynext)
                        {
                            DRAW_BOTTOMBAR(BTBAR_GALLERY_PLAY_BACK);
                        }
                        else
                        {
                            DRAW_BOTTOMBAR(BTBAR_PLAY_BACK);
                        }
                    }
                    
                    if(pMe->m_FileToPlay!=NULL )
                    {                       
                        VideoPlayer_RefreshPlayerFileName(pMe); //刷新文件名                        
                    }                    
                
                    VideoPlayer_RefreshPlayingTick(pMe);//刷新时间tick
                    VideoPlayer_RefreshVolBar(pMe);//刷新音量
                    VideoPlayer_RefreshScheduleBar(pMe);//刷新进度条            
                    IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE);                                   
                }
            }            
            return TRUE;
			                           
        //前进
#ifdef FEATURE_VERSION_C337    
		case AVK_RIGHT:
#else
        case AVK_POUND:
#endif			
            if(pMe->IsPlay)
            {             
                if((pMe->bTotalTime - pMe->bCurrentTime) < 5)
                {
                    IMEDIA_FastForward((IMedia*)pMe->m_pMedia,pMe->bTotalTime - pMe->bCurrentTime);
                    pMe->bCurrentTime=pMe->bTotalTime;
                }
                else 
                {
                    IMEDIA_FastForward((IMedia*)pMe->m_pMedia,5000); 
                    pMe->bCurrentTime=pMe->bCurrentTime+5;
                }
                if(!pMe->IsFullScreen)
                {
                    
#ifdef FEATURE_DISP_240X320
                    VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE,IDI_FORWARD_SELECT, VIDEOPLAYER_FORWARD_X,VIDEOPLAYER_FORWARD_Y); //画按键按下去的小图标
#endif
					pMe->m_rtype = TYPE_KUAIJIN;
                    IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE);
                    VideoPlayer_RefreshPlayingTick(pMe);//刷新时间
                    VideoPlayer_RefreshScheduleBar(pMe);//刷新播放进度
                    (void) ISHELL_SetTimer(pMe->m_pShell, 100, (PFNNOTIFY)VideoPlayer_UpdateButton, pMe);            
                }
            }
            return TRUE;
                
        //后退
#ifdef FEATURE_VERSION_C337    
		case AVK_LEFT:
#else       
        case AVK_STAR:	
#endif			
            if(pMe->IsPlay)
            {               
                if( pMe->bCurrentTime < 5)
                {                  
                    IMEDIA_Rewind((IMedia*)pMe->m_pMedia,pMe->bCurrentTime);
                    pMe->bCurrentTime=0;
                }
                else
                {
                    IMEDIA_Rewind((IMedia*)pMe->m_pMedia,5000);   
                    pMe->bCurrentTime=pMe->bCurrentTime-5;
                }
                if(!pMe->IsFullScreen)
                {
#ifdef FEATURE_DISP_240X320
                    VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE, IDI_REWIND_SELECT, VIDEOPLAYER_REWIND_X, VIDEOPLAYER_REWIND_Y); //画按键按下去的小图标   
#endif
                    VideoPlayer_RefreshPlayingTick(pMe);
                    VideoPlayer_RefreshScheduleBar(pMe);//刷新播放进度条
					pMe->m_rtype = TYPE_KUAITUI;
                    IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE);
                    (void) ISHELL_SetTimer(pMe->m_pShell, 100, (PFNNOTIFY)VideoPlayer_UpdateButton, pMe);            
                }
            }
            return TRUE;     		
               
        //播放上一首   
#ifdef FEATURE_VERSION_C337
		case AVK_UP:
#else
        case AVK_LEFT:
#endif			
            if(! pMe->m_IsPlaynext)
            {
                return TRUE;
            }
            if(!pMe->m_bAppIsReady)
            {
                 return TRUE;  
            }
			#ifdef FEATURE_VERSION_W317A
			{
    		   pMe->m_keybusy = TRUE;			
               ISHELL_SetTimer(pMe->m_pShell,3000,(PFNNOTIFY) VideoPlayer_keybusy,pMe);
			}
			#endif
           /* ISHELL_SetTimer(pMe->m_pShell,APPISREADY_TIMER,VideoPlayer_APPIsReadyTimer,pMe);
            if((!pMe->m_bAppIsReady)||(pMe->UserStop))
            {
                return TRUE;
            }*/
            if(!pMe->IsFullScreen)
            {
#if defined (FEATURE_DISP_240X320)||defined(FEATURE_DISP_220X176)|| defined(FEATURE_DISP_176X220)
                VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE, IDI_BEFORE_SELECT, VIDEOPLAYER_PREVIOUS_X, VIDEOPLAYER_PREVIOUS_Y); //画按键按下去的小图标
#endif
                pMe->bOldTime=0;
                pMe->m_rtype = TYPE_PREVIOUS;
                IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE);
              //(void) ISHELL_SetTimer(pMe->m_pShell, 100, (PFNNOTIFY)VideoPlayer_UpdateButton, pMe);//100ms后画按钮弹起的图
              //  pMe->UserStop = FALSE;
                VideoPlayer_PlayNext(pMe, FALSE); 
            }
            else if(!pMe->IsPause)
            {
                VideoPlayer_PlayNext(pMe, FALSE); 
            }
           // pMe->m_bAppIsReady=FALSE;
            return TRUE;
       
        //播放下一首   
#ifdef FEATURE_VERSION_C337
		case AVK_DOWN:
#else        
        case AVK_RIGHT:  
#endif			
            MSG_FATAL("pMe->m_IsPlaynext=%d",pMe->m_IsPlaynext,0,0);
            if(! pMe->m_IsPlaynext)
            {
                return TRUE;
            }
            if(!pMe->m_bAppIsReady)
            {
                 return TRUE;  
            }
			#ifdef FEATURE_VERSION_W317A
			{
    		   pMe->m_keybusy = TRUE;			
               ISHELL_SetTimer(pMe->m_pShell,3000,(PFNNOTIFY) VideoPlayer_keybusy,pMe);
			}
			#endif
            /*ISHELL_SetTimer(pMe->m_pShell,APPISREADY_TIMER,VideoPlayer_APPIsReadyTimer,pMe);
            MSG_FATAL("pMe->m_bAppIsReady=%d--pMe->UserStop=%d",pMe->m_bAppIsReady,pMe->UserStop,0);
            if((!pMe->m_bAppIsReady) ||(pMe->UserStop))
            {
                return TRUE;
            }*/
            if(!pMe->IsFullScreen)
            { 
#if defined (FEATURE_DISP_240X320)||defined(FEATURE_DISP_220X176)|| defined(FEATURE_DISP_176X220)
                VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE,IDI_NEXT_SELECT, VIDEOPLAYER_NEXT_X, VIDEOPLAYER_NEXT_Y);//画按键按下去的小图标  
#endif
                pMe->bOldTime=0;
				pMe->m_rtype = TYPE_NEXT;
                IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE);
               //(void) ISHELL_SetTimer(pMe->m_pShell, 100, (PFNNOTIFY)VideoPlayer_UpdateButton, pMe);//100ms后画按钮弹起的图
               // pMe->UserStop = FALSE;
                VideoPlayer_PlayNext(pMe, TRUE);
            }            
            else if(!pMe->IsPause)//全屏播放状态
            {
                VideoPlayer_PlayNext(pMe, TRUE); 
            }
            //pMe->m_bAppIsReady=FALSE;
            return TRUE;          
          
        //增大音量   
#ifdef FEATURE_VERSION_C337
		case AVK_POUND:
#else
        case AVK_UP: 
#endif			
#ifdef FEATURE_ALL_KEY_PAD                     
        case AVK_O:
#endif                
            if(pMe->m_bVolumeLevel < VOLUME_FIVE)
            {
                pMe->m_bVolumeLevel++;
                if(pMe->m_bVolumeLevel == VOLUME_FIVE)
                {
                    pMe->totalvolume = 100;//最大音量值
                }
                else
                {
                    pMe->totalvolume = pMe->m_bVolumeLevel * VP_VOLUME_STEP;
                }
                    
                if(pMe->m_pMedia)
                {
                    (void)IMEDIA_SetVolume((IMedia*)pMe->m_pMedia,pMe->totalvolume); 
                } 
                if(!pMe->IsFullScreen)
                {
#ifdef FEATURE_DISP_240X320
                    VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE,IDI_ADD_SELECT, VIDEOPLAYER_VOLUME_ADD_X,VIDEOPLAYER_VOLUME_ADD_Y);// 画按键按下去的小图标
#endif
                    VideoPlayer_RefreshVolBar(pMe); // 重新刷新音量bar
					pMe->m_rtype = TYPE_ADDVOLUME;
                    IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE);                    
                    //(void) ISHELL_SetTimer(pMe->m_pShell, 100, (PFNNOTIFY)VideoPlayer_UpdateButton, pMe);// 50ms后画按钮弹起的图                
                }
            }             
            return TRUE;          
          
        //减小音量  
#ifdef FEATURE_VERSION_C337
		case AVK_STAR:
#else
        case AVK_DOWN: 
#endif			
#ifdef FEATURE_ALL_KEY_PAD                     
        case AVK_I:
#endif                
            if(pMe->m_bVolumeLevel > VOLUME_OFF)
            {
                pMe->m_bVolumeLevel--;
                pMe->totalvolume = pMe->m_bVolumeLevel * VP_VOLUME_STEP;
                if(pMe->m_pMedia)                                                                                                                                                                   
                {
                    (void)IMEDIA_SetVolume((IMedia*)pMe->m_pMedia,pMe->totalvolume);
                } 
                if(!pMe->IsFullScreen)
                {
#ifdef FEATURE_DISP_240X320
                    VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE,IDI_LOW_SELECT, VIDEOPLAYER_VOLUME_LOW_X, VIDEOPLAYER_VOLUME_LOW_Y);// 画按键按下去的小图标
#endif
                    VideoPlayer_RefreshVolBar(pMe);// 重新刷新音量bar 
					pMe->m_rtype = TYPE_DECVOLUME;
                    IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE);
                    //(void) ISHELL_SetTimer(pMe->m_pShell, 100, (PFNNOTIFY)VideoPlayer_UpdateButton, pMe);// 50ms后画按钮弹起的图   
                }
            }            
            return TRUE;
       
#if 0
        //全屏播放
        case AVK_0:
            return VideoPlayer_PlayMod(pMe);            
            break; 
            
        case AVK_SELECT:
            if(pMe->IsFullScreen)
            {                
                return TRUE;
            }
            else
            {
                CLOSE_DIALOG(DLGRET_OPTION);
                return TRUE;
            }               
#endif
        
        case AVK_GSENSOR_STAND:
        case AVK_GSENSOR_UPEND: 
        case AVK_GSENSOR_LEFT:
        case AVK_GSENSOR_RIGHT: 

#ifdef FEATURE_VERSION_C337
		case AVK_0:
#else
		case AVK_SELECT:
#endif        
            if(pMe->IsPlay)
            {
            	MSG_FATAL("------->Fullscreen1",0,0,0);
                FullScreen = TRUE;
                return VideoPlayer_PlayMod(pMe,wParam);
            }
            if(pMe->UserStop && pMe->m_IsPlaynext)
            {
                CMediaGallery_FileExplorer(GALLERY_VIDEO_ADDLIST,NULL);
                return TRUE;
            }
		
#ifdef FEATURE_LCD_TOUCH_ENABLE//WLH ADD FOR LCD TOUCH
		case EVT_PEN_UP:
			{
				int i;
				AEEDeviceInfo devinfo;
				int nBarH ;
				AEERect rc;
				int16 wXPos = (int16)AEE_GET_X((const char *)dwParam);
				int16 wYPos = (int16)AEE_GET_Y((const char *)dwParam);
				
				nBarH = GetBottomBarHeight(pMe->m_pDisplay);
				MEMSET(&devinfo, 0, sizeof(devinfo));
				ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
				SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, devinfo.cyScreen);
				if ((wXPos>0)&&(wXPos<devinfo.cxScreen/3)&&(wYPos>rc.y)&&(wYPos<devinfo.cyScreen))
				{
					boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_VIDEOPLAYER,EVT_USER,AVK_SELECT,1);
					return rt;
				}
				else if ((wXPos>devinfo.cxScreen/3)&&(wXPos<(devinfo.cxScreen/3)*2)&&(wYPos>rc.y)&&(wYPos<devinfo.cyScreen))
				{
					boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_VIDEOPLAYER,EVT_USER,AVK_INFO,0);
					 return rt;
				}
				else if ((wXPos>wXPos<(devinfo.cxScreen/3)*2)&&(wXPos<devinfo.cxScreen)&&(wYPos>rc.y)&&(wYPos<devinfo.cyScreen))
				{
					boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_VIDEOPLAYER,EVT_USER,AVK_CLR,0);
					return rt;
				}
			}
			return TRUE;
#endif//FEATURE_LCD_TOUCH_ENABLE         
        default:
            break;
    }
    return TRUE;
}
/*=================================================================================================================
  处理OPTION的按钮事件
=================================================================================================================*/
#if 0
static boolean VPDOption_HandleKeyEvent(CVideoPlayer *pMe,AEEEvent eCode,uint16 wParam,uint32 dwParam)
{
    switch(wParam)
    {       
        case AVK_UP:
        case AVK_DOWN:
            {                    
                if(pMe->m_nItemID == IDS_PLAY || IDS_PAUSE)     
                {
                    pMe->m_nItemID = IDS_FULLSCREEN;
                }
                if(pMe->m_nItemID== IDS_FULLSCREEN) 
                {
                    pMe->m_nItemID = IDS_DELETE || IDS_GALLERY;
                }
                if(pMe->m_nItemID== IDS_GALLERY) 
                {
                    pMe->m_nItemID = IDS_PLAY || IDS_PAUSE;
                }
                else if(pMe->m_nItemID== IDS_DELETE) 
                {
                    pMe->m_nItemID = IDS_GALLERY;  
                }
            } 
            return TRUE;
  
        case AVK_INFO:
        case AVK_SELECT:
            CLOSE_DIALOG(DLGRET_MSGBOX);
            return TRUE;

        case AVK_CLR:
            CLOSE_DIALOG(DLGRET_CANCELED);
            return TRUE;   

        default:
            break;         
    }
    return TRUE;
}
#endif
/*=================================================================================================================
      处理MSGBOX的按钮事件
=================================================================================================================*/
#if 0
static boolean VPDMSGBOX_HandleKeyEvent(CVideoPlayer *pMe,AEEEvent eCode,uint16 wParam,uint32 dwParam)
{ 
    if(pMe->m_nMsgResID==IDS_DONE)
    {
        CLOSE_DIALOG(DLGRET_VIDEOPLAYER);
    }
    switch(wParam)
    {
        case AVK_SELECT:
            (void)ISHELL_CancelTimer(pMe->m_pShell, DialogTimeoutCallback,pMe);            
            /*
            if(pMe->m_nMsgResID==IDS_PLAYFAILED)
            {
                CLOSE_DIALOG(DLGRET_CANCELED);
            }
            */
            if(pMe->m_nMsgResID==IDS_DEL)
            {    
                //删除正在播放的视频文件
                //VideoPlayer_DeleteVideo(pMe);
                //如果删除成功，则显示Done对话框
                CLOSE_DIALOG(DLGRET_MSGBOX);//Done弹框
            }
            return TRUE;
                    
        case AVK_CLR:  
            (void)ISHELL_CancelTimer(pMe->m_pShell, DialogTimeoutCallback,pMe);
            if(pMe->m_nMsgResID==IDS_DEL || pMe->m_nMsgResID==IDS_DONE)
            {                
                CLOSE_DIALOG(DLGRET_VIDEOPLAYER);
            }
            return TRUE;
                   
        default:
            break;
    }

    return TRUE;
}   
#endif
/*=================================================================================================================
  在指定位置画图
=================================================================================================================*/
static void VideoPlayer_DrawImage(CVideoPlayer *pMe, char *resFile, int16 resId, int x, int y)
{
    IImage *image = ISHELL_LoadResImage( pMe->m_pShell, resFile, resId);
    if( image != NULL)
    {
        IIMAGE_Draw(image, x, y);
        IIMAGE_Release(image);
    }
    //IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE);
}
/*=================================================================================================================
   重画player下半屏的按钮部分
=================================================================================================================*/                
static  void VideoPlayer_UpdateButton(CVideoPlayer *pMe)
{
#ifdef FEATURE_DISP_240X320
    if(!pMe->IsPlay)
    {
        VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE, IDI_PLAY, VIDEOPLAYER_PLAY_X,VIDEOPLAYER_PLAY_Y);//重画三角形按键盘,弹起的大图   
    }
    else VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE, IDI_PAUSE, VIDEOPLAYER_PLAY_X,VIDEOPLAYER_PLAY_Y);//重画"||"按键盘

	if(pMe->m_rtype == TYPE_ADDVOLUME)
	{
		VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE, IDI_ADD, VIDEOPLAYER_VOLUME_ADD_X,VIDEOPLAYER_VOLUME_ADD_Y);//重画三角形按键盘,弹起的大图   
	}
	else if(pMe->m_rtype == TYPE_DECVOLUME)
	{
		VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE, IDI_LOW, VIDEOPLAYER_VOLUME_LOW_X,VIDEOPLAYER_VOLUME_LOW_Y);//重画三角形按键盘,弹起的大图   
	}
	else if(pMe->m_rtype == TYPE_PREVIOUS)
	{
		VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE, IDI_BEFORE, VIDEOPLAYER_PREVIOUS_X,VIDEOPLAYER_PREVIOUS_Y);//重画三角形按键盘,弹起的大图   
	}
	else if(pMe->m_rtype == TYPE_NEXT)
	{
		VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE, IDI_NEXT, VIDEOPLAYER_NEXT_X,VIDEOPLAYER_NEXT_Y);//重画三角形按键盘,弹起的大图   
	}
	else if(pMe->m_rtype == TYPE_KUAIJIN)
	{
		VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE, IDI_FORWARD, VIDEOPLAYER_FORWARD_X,VIDEOPLAYER_FORWARD_Y);//重画三角形按键盘,弹起的大图   
	}
	else if(pMe->m_rtype == TYPE_KUAITUI)
	{
		VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE, IDI_REWIND, VIDEOPLAYER_REWIND_X,VIDEOPLAYER_REWIND_Y);//重画三角形按键盘,弹起的大图   
	}
    IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE);
#elif defined(FEATURE_DISP_220X176)
    if(!pMe->IsPlay)
    {
        VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE, IDI_PLAY, VIDEOPLAYER_PLAY_X,VIDEOPLAYER_PLAY_Y);//重画三角形按键盘,弹起的大图   
    }
    else VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE, IDI_PAUSE, VIDEOPLAYER_PLAY_X,VIDEOPLAYER_PLAY_Y);//重画"||"按键盘
    if(pMe->m_rtype == TYPE_PREVIOUS)
	{
		VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE, IDI_BEFORE, VIDEOPLAYER_PREVIOUS_X,VIDEOPLAYER_PREVIOUS_Y);//重画三角形按键盘,弹起的大图   
	}
	else if(pMe->m_rtype == TYPE_NEXT)
	{
		VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE, IDI_NEXT, VIDEOPLAYER_NEXT_X,VIDEOPLAYER_NEXT_Y);//重画三角形按键盘,弹起的大图   
	}
#elif defined(FEATURE_DISP_176X220)
    if(!pMe->IsPlay)
    {
        VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE, IDI_PLAY, VIDEOPLAYER_PLAY_X,VIDEOPLAYER_PLAY_Y);//重画三角形按键盘,弹起的大图   
    }
    else VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE, IDI_PAUSE, VIDEOPLAYER_PLAY_X,VIDEOPLAYER_PLAY_Y);//重画"||"按键盘
    if(pMe->m_rtype == TYPE_PREVIOUS)
	{
		VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE, IDI_BEFORE, VIDEOPLAYER_PREVIOUS_X,VIDEOPLAYER_PREVIOUS_Y);//重画三角形按键盘,弹起的大图   
	}
	else if(pMe->m_rtype == TYPE_NEXT)
	{
		VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE, IDI_NEXT, VIDEOPLAYER_NEXT_X,VIDEOPLAYER_NEXT_Y);//重画三角形按键盘,弹起的大图   
	}    
#else
#endif
}
static void VideoPlayer_UpdateFRButton(CVideoPlayer *pMe)
{
#if 0
    VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE, IDI_FR_PART, VIDEOPLAYER_FR_PART_X,VIDEOPLAYER_FR_PART_Y);
    IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE);
#endif
}
/*=================================================================================================================
初始化video
=================================================================================================================*/
void VidePlayer_SoundRestore(void)
{    
    IConfig             *pConfig;
    int nRet = EFAILED;
    byte		return_ringer_level[PROFILENUMBER];
	byte	    return_beep_level[PROFILENUMBER];    
	byte        CurProfile;      //当前情景模式	
	byte		set_ringer_level;
	byte		set_beep_level;
	byte        CallVolume;
#if 0    
        // Create the IConfig Serivce object.
    nRet = ISHELL_CreateInstance(AEE_GetShell(),
                                 AEECLSID_CONFIG,
                                (void **) &pConfig);
    if (nRet != SUCCESS||pConfig==NULL) 
    {
        return ;
    }
    
    
    ICONFIG_GetItem(pConfig, CFGI_PROFILE_CUR_NUMBER,&CurProfile, sizeof(byte));
    MSG_FATAL("VidePlayer_SoundRestore CurProfile=%d",CurProfile,0,0);
    //wangliang modify!  2010-09-25
	(void) ICONFIG_GetItem(pConfig,
	                    CFGI_PROFILE_RINGER_VOL,
	                    return_ringer_level,
	                    sizeof(return_ringer_level));

	(void) ICONFIG_GetItem(pConfig,
	                    CFGI_PROFILE_BEEP_VOL,
	                    return_beep_level,
	                    sizeof(return_beep_level));

	(void) ICONFIG_GetItem(pConfig,CFGI_EAR_VOL,&CallVolume,sizeof(byte));
    MSG_FATAL("VidePlayer_SoundRestore CallVolume=%d",CallVolume,0,0);                                            
	set_ringer_level            =   return_ringer_level[CurProfile];
	set_beep_level              =   return_beep_level[CurProfile];
           
   (void)ICONFIG_SetItem(pConfig,CFGI_BEEP_VOL,set_beep_level,sizeof(byte));
   (void)ICONFIG_SetItem(pConfig,CFGI_RINGER_VOL,set_ringer_level,sizeof(byte));
   (void)ICONFIG_SetItem(pConfig,CFGI_EAR_VOL,CallVolume,sizeof(byte));

    if (pConfig!=NULL)
        ICONFIG_Release(pConfig);   
#endif    
}

void  VideoPlayer_InitVideo(CVideoPlayer  *pMe)                  
{    
    //填写m_md stucture
    uint32 uiClsId = 0;
    pMe->m_md.clsData = MMD_FILE_NAME;
    pMe->m_md.pData = (void*)pMe->m_FileToPlay;
    pMe->m_md.dwSize = 0;
	pMe->m_pMediaUtil = 0;
	pMe->m_pHelp = NULL;
    //snd_set_device(snd_device_type device,snd_mute_control_type ear_mute,snd_mute_control_type mic_mute,snd_cb_func_ptr_type callback_ptr,const void * client_data)
	//pMe->m_InitFailed = AEEMediaUtil_CreateMedia(pMe->m_pShell, &pMe->m_md, &pMe->m_pMedia);
	pMe->m_InitFailed = AEEMediaUtil_CreateMedia(pMe->m_pShell, &pMe->m_md,&(IMedia*)pMe->m_pMedia);
    DBGPRINTF("pMe->m_InitFailed=%d",pMe->m_InitFailed);
    //(void)IMEDIA_SetAudioDevice((IMedia *)pMe->m_pMedia, HS_HEADSET_ON()?AEE_SOUND_DEVICE_STEREO_HEADSET:AEE_SOUND_DEVICE_SPEAKER);

    if(pMe->m_InitFailed == SUCCESS)
    {             
        (void)IMEDIA_RegisterNotify((IMedia*)pMe->m_pMedia, VideoPlayer_VideoNotify, pMe);//注册底层回调 
        pMe->bCurrentTime = 0;
        pMe->bTotalTime = 0;
		uiClsId = IMEDIA_GetTotalTime((IMedia*)pMe->m_pMedia); 
		DBGPRINTF("(void)IMEDIA_GetTotalTime(pMe->m_pMedia); %d",uiClsId);        
        (void)IMEDIA_SetAudioDevice((IMedia *)pMe->m_pMedia, HS_HEADSET_ON()?AEE_SOUND_DEVICE_STEREO_HEADSET:AEE_SOUND_DEVICE_SPEAKER);
        (void)IMEDIA_SetVolume((IMedia*)pMe->m_pMedia, 80/*pMe->totalvolume*/); //设置当前音量大小
       // VideoPlayer_ChangeScrState(pMe,TRUE);
    }    
}
/*=================================================================================================================
  播放视频
=================================================================================================================*/      
void VideoPlayer_PlayVideo(CVideoPlayer *pMe)
{     
    if(pMe->m_pMedia)
    {  
    	MSG_FATAL("VideoPlayer_PlayVideo YY Said : Play!!! ",0,0,0);
    	VideoPlayer_ChangeScrState(pMe,TRUE);
        pMe->m_bAppIsReady = TRUE;
		b_is_GetFrame = FALSE;
        pMe->m_PlayFailed = IMEDIA_Play((IMedia*)pMe->m_pMedia);//播放          
        #if defined(FEATURE_DISP_240X320)
        VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE,IDI_PAUSE, VIDEOPLAYER_PLAY_X,VIDEOPLAYER_PLAY_Y);
        VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE,IDI_BEFORE, VIDEOPLAYER_PREVIOUS_X,VIDEOPLAYER_PREVIOUS_Y);
        VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE,IDI_NEXT, VIDEOPLAYER_NEXT_X,VIDEOPLAYER_NEXT_Y);
        VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE,IDI_REWIND, VIDEOPLAYER_REWIND_X,VIDEOPLAYER_REWIND_Y);
        VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE,IDI_FORWARD, VIDEOPLAYER_FORWARD_X,VIDEOPLAYER_FORWARD_Y);
        VideoPlayer_RefreshVolBar(pMe);
		VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE, IDI_ADD, VIDEOPLAYER_VOLUME_ADD_X,VIDEOPLAYER_VOLUME_ADD_Y);   
		VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE, IDI_LOW, VIDEOPLAYER_VOLUME_LOW_X,VIDEOPLAYER_VOLUME_LOW_Y);
        #elif defined(FEATURE_DISP_220X176)
        VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE,IDI_PAUSE, VIDEOPLAYER_PLAY_X,VIDEOPLAYER_PLAY_Y);
        VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE,IDI_BEFORE, VIDEOPLAYER_PREVIOUS_X,VIDEOPLAYER_PREVIOUS_Y);
        VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE,IDI_NEXT, VIDEOPLAYER_NEXT_X,VIDEOPLAYER_NEXT_Y);
		VideoPlayer_RefreshVolBar(pMe);
		#elif defined(FEATURE_DISP_176X220)
        VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE,IDI_PAUSE, VIDEOPLAYER_PLAY_X,VIDEOPLAYER_PLAY_Y);
        VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE,IDI_BEFORE, VIDEOPLAYER_PREVIOUS_X,VIDEOPLAYER_PREVIOUS_Y);
        VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE,IDI_NEXT, VIDEOPLAYER_NEXT_X,VIDEOPLAYER_NEXT_Y);
        VideoPlayer_RefreshVolBar(pMe); 
        #endif
        SetDeviceState(DEVICE_TYPE_MP4,DEVICE_MP4_STATE_ON);
		MSG_FATAL("b_is_GetFrame===========%d",b_is_GetFrame,0,0);
    }      
} 
                    
/*=================================================================================================================
   播放下一个文件, 
   bDirection为TRUE, 下一个文件
   bDirection为FALSE, 前一个文件
   pMe->m_MenuSelected
=================================================================================================================*/
static  void VideoPlayer_PlayNext(CVideoPlayer *pMe, boolean bDirection)
{  
    uint16    videoID = 0;
    MGFileInfo  pInfo;
    
    videoID=VideoPlayer_GetFileID(pMe);
    //video放置在指定文件夹外
    MSG_FATAL("videoID=%d,pMe->m_RecordCount=%d",videoID,pMe->m_RecordCount,0);
    MSG_FATAL("pMe->IsPause=%d,pMe->UserStop=%d",pMe->IsPause,pMe->UserStop,0);
    if(videoID >= pMe->m_RecordCount)
    {  
        return ;
    }
    else
    {
        switch(pMe->m_RecordCount)
        {
            //文件数为0，什么都不做
            case 0:
                return;
        
            //文件数为1，重新播放此文件(直接播放)   
            case 1:    
                pMe->bCurrentTime = 0; 
                if(! pMe->UserStop)
                {
                    (void)IMEDIA_Stop((IMedia*)pMe->m_pMedia); 
                }
                VideoPlayer_ReleaseVideo(pMe);
                videoplayer_play_flag = TRUE;
                #if 0
                if( ! pMe->IsFullScreen && pMe->IsPlay)
                {
#if 0
                    VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE,IDI_PAUSE, VIDEOPLAYER_PLAY_X,VIDEOPLAYER_PLAY_Y);            
#endif
                    // YY TODO:
					//DRAW_BOTTOMBAR(BTBAR_FULLSCREEN_PAUSE_STOP); 
                    ISHELL_SetTimer(pMe->m_pShell,50,(PFNNOTIFY)(IMEDIA_Resume),pMe->m_pMedia);
                    VideoPlayer_RefreshPlayingTick(pMe);//刷新时间
                    VideoPlayer_RefreshScheduleBar(pMe);//进度条置在初始位置
                    IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE);
                }
                #endif
                VideoPlayer_InitVideo(pMe);
                // 如果当前视频为播放状态,则下一首视频也直接置为播放状态;不是,则手动播放                
               
                VideoPlayer_PlayVideo(pMe); 
                if(pMe->IsPause)
                {
                    IMedia_Pause((IMedia*)pMe->m_pMedia);                    
                }
                if(pMe->UserStop)
                {
                    IMedia_Stop((IMedia*)pMe->m_pMedia);                   
                }
                if(!pMe->IsFullScreen)
                {
                    if(pMe->m_PlayFailed == SUCCESS)
                    {
                        VideoPlayer_RefreshPlayingTick(pMe);// 刷新播放时间             
                        VideoPlayer_RefreshScheduleBar(pMe);// 刷新进度条 
                    }
                    VideoPlayer_RefreshPlayerFileName(pMe);// 刷新文件名 
                    IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE);    
                }
                break;
        
            //文件数大于1，播放下一首   
            default:  
                if(! pMe->UserStop)
                {
                    (void)IMEDIA_Stop((IMedia*)pMe->m_pMedia); 
                }
                //释放本m_pMedia指针
                VideoPlayer_ReleaseVideo(pMe);
                FREEIF(pMe->m_FileToPlay);
                if(bDirection)//下一首
                {   
                    //当前播放的文件为记录中的最后一个,则播放记录中的第一个文件     
                    if(videoID +1 == pMe->m_RecordCount)
                    {                  
                        videoID=0;
                    }                    
                    else  videoID+=1;                    
                }                
                else//上一首
                {
                    //当前播放的文件为记录中的第一个,则播放记录中的最后一个文件     
                    if(videoID == 0)
                    {     
                        videoID=pMe->m_RecordCount-1;
                    }                    
                    else  videoID-=1;
                } 
                CMediaGallery_GetFileInfoRecord(pMe->m_pFileDB,videoID, &pInfo);
                
                pMe->m_FileToPlay= STRDUP(pInfo.szName);
                if(pMe->m_pFileDB!=NULL)
                {
                    IDATABASE_Release(pMe->m_pFileDB);
                    pMe->m_pFileDB = NULL;
                }
                VideoPlayer_InitVideo(pMe);
                // 如果当前视频为播放状态,则下一首视频也直接置为播放状态;不是,则手动播放                
               
                VideoPlayer_PlayVideo(pMe); 
                if(pMe->IsPause)
                {
                    IMedia_Pause((IMedia*)pMe->m_pMedia);                    
                }
                if(pMe->UserStop)
                {
                    IMedia_Stop((IMedia*)pMe->m_pMedia);                   
                }
                if(pMe->IsFullScreen)
                {
					//MMD_LCDRotate(0);
                    //如果下一首播放成功，则转为全屏，不成功则在正常模式弹出播放失败的弹框
                    if(pMe->m_PlayFailed == SUCCESS)
                    {
                        //MMD_LCDRotate(1);                        
                    }
                    else
                    {
                        VideoPlayer_RefreshPlayerFileName(pMe);// 刷新文件名 
                        IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE);    
                    }
                    /*全屏暂停时不playnext
                    if(pMe->IsPause)
                    {
						// YY TODO:
                        MMD_Player_File_Pause(pFileHandle); 
                    }
                    */
                }  
                
                if(!pMe->IsFullScreen)
                {
                    if(pMe->m_PlayFailed == SUCCESS)
                    {
                        VideoPlayer_RefreshPlayingTick(pMe);// 刷新播放时间             
                        VideoPlayer_RefreshScheduleBar(pMe);// 刷新进度条 
                    }
                    VideoPlayer_RefreshPlayerFileName(pMe);// 刷新文件名 
                    IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE);    
                }
            
                break;    
        }      
    }
    //非用户手动停止
    //pMe->UserStop = FALSE;
}
void VideoPlayer_WriteTitle(CVideoPlayer *pMe,AECHAR* pText)
{
	AEERect   rc_name;
	VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE,IDI_NAME_PART, 0, 0); 
	#if defined(FEATURE_DISP_240X320)
	SETAEERECT(&rc_name, VIDEOPLAYER_NAMEPART_X+60,VIDEOPLAYER_NAMEPART_Y, VIDEOPLAYER_NAMEPART_W-120, VIDEOPLAYER_NAMEPART_H-8);
	//写title
	DrawTextWithProfile(pMe->m_pShell, 
					pMe->m_pDisplay, 
					RGB_WHITE, //文本轮廓的RGBVAL颜色值,RGB_BLACK
					AEE_FONT_BOLD,
					pText, 
					-1, 
					0, 
					0, 
					&rc_name, 
					IDF_ALIGN_CENTER|IDF_ALIGN_MIDDLE|IDF_TEXT_TRANSPARENT);	
    #elif defined(FEATURE_DISP_220X176)
    //VideoPlayer_RefreshVolBar(pMe);
	//SETAEERECT(&rc_name, VIDEOPLAYER_NAMEPART_X+55,VIDEOPLAYER_NAMEPART_Y, VIDEOPLAYER_NAMEPART_W-110, VIDEOPLAYER_NAMEPART_H);
	
	SETAEERECT(&rc_name, VIDEOPLAYER_NAMEPART_X+5,VIDEOPLAYER_NAMEPART_Y, VIDEOPLAYER_NAMEPART_W-10, VIDEOPLAYER_NAMEPART_H);
	
	//写title
	DrawTextWithProfile(pMe->m_pShell, 
					pMe->m_pDisplay, 
					RGB_WHITE, //文本轮廓的RGBVAL颜色值,RGB_BLACK
					AEE_FONT_BOLD,
					pText, 
					-1, 
					0, 
					0, 
					&rc_name, 
					IDF_ALIGN_CENTER|IDF_ALIGN_MIDDLE|IDF_TEXT_TRANSPARENT);
    #elif defined(FEATURE_DISP_176X220)
    //VideoPlayer_RefreshVolBar(pMe);
	SETAEERECT(&rc_name, VIDEOPLAYER_NAMEPART_X+44,VIDEOPLAYER_NAMEPART_Y, VIDEOPLAYER_NAMEPART_W-88, VIDEOPLAYER_NAMEPART_H);
    //写title
	DrawTextWithProfile(pMe->m_pShell, 
					pMe->m_pDisplay, 
					RGB_WHITE, //文本轮廓的RGBVAL颜色值,RGB_BLACK
					AEE_FONT_BOLD,
					pText, 
					-1, 
					0, 
					0, 
					&rc_name, 
					IDF_ALIGN_CENTER|IDF_ALIGN_MIDDLE|IDF_TEXT_TRANSPARENT);
	#else 
	SETAEERECT(&rc_name, VIDEOPLAYER_NAMEPART_X + 20,VIDEOPLAYER_NAMEPART_Y, VIDEOPLAYER_NAMEPART_W - 40, VIDEOPLAYER_NAMEPART_H);
	//写title
	DrawTextWithProfile(pMe->m_pShell, 
					pMe->m_pDisplay, 
					RGB_WHITE, //文本轮廓的RGBVAL颜色值,RGB_BLACK
					AEE_FONT_BOLD,
					pText, 
					-1, 
					22, 
					22, 
					&rc_name, 
					IDF_ALIGN_CENTER|IDF_ALIGN_MIDDLE|IDF_TEXT_TRANSPARENT);
    #endif	
}
void VideoPlayer_WriteTitleRes(CVideoPlayer *pMe,char* pResPath,int uResId)
{
  AECHAR pText[256] = {0};
  ISHELL_LoadResString(pMe->m_pShell,pResPath,uResId,pText,sizeof(pText));
  VideoPlayer_WriteTitle(pMe,pText);
}

//防止快速按健
static void VideoPlayer_keybusy(CVideoPlayer *pMe) 
{ 
    pMe->m_keybusy = FALSE;
  
}


/*=================================================================================================================
   刷player屏的上半部分的文件名
=================================================================================================================*/                                                                        
static  void VideoPlayer_RefreshPlayerFileName(CVideoPlayer *pMe)
{   
    char      title_copy[MAX_STR_LEN];  
    AECHAR    player_title[MAX_STR_LEN];
    char      *filename=NULL; 
    int       len;

    if(pMe->IsFullScreen) return;
        
    pMe->m_np=0;
    MEMSET(title_copy, 0, sizeof(title_copy));
    MEMSET(player_title, 0, sizeof(player_title));
    MEMSET(pMe->m_playname, 0, sizeof(pMe->m_playname));
    //截取文件路径名中的文件名以显示在播放器上 
    filename= STRRCHR(pMe->m_FileToPlay, (int)DIRECTORY_CHAR);
    if(filename!=NULL)
    {
        filename=filename+1;
        STRCPY(title_copy,filename);
    } 
    else STRCPY(title_copy,pMe->m_FileToPlay);    
   
    //UTF8转AECHAR
    (void)UTF8TOWSTR((const byte *)title_copy,(int)STRLEN(title_copy),player_title,sizeof(player_title)); 
    (void)UTF8TOWSTR((const byte *)title_copy,(int)STRLEN(title_copy),pMe->m_playname,sizeof(pMe->m_playname));

    len=IDISPLAY_MeasureText(pMe->m_pDisplay,AEE_FONT_NORMAL,player_title); // 绘制字符串所需要的像素    
    if(len > MAX_NAME_LEN )
    {        
        //滚动显示名字
        VideoPlayer_ShowName(pMe);  
    }
    else 
    {        
       VideoPlayer_WriteTitle(pMe,player_title);

    }
}
//滚动显示文件名
static void VideoPlayer_ShowName(CVideoPlayer *pMe) 
{ 
    int      str;
    AECHAR   *pp;   
    int      settime;

	if(pMe->IsFullScreen) return;
    
    pp=&pMe->m_playname[pMe->m_np]; 
    pMe->m_np += 1;
    str=IDISPLAY_MeasureText(pMe->m_pDisplay,AEE_FONT_NORMAL,pp);
	
	VideoPlayer_WriteTitle(pMe,pp);

    IDISPLAY_UpdateEx(pMe->m_pDisplay,TRUE); 
    if(str <= MAX_NAME_LEN)
    {
        pMe->m_np=0;
    }
    if(pMe->m_np==1)
    {
        settime=SETTIME_LONG; // 3000 
    }
    else settime=SETTIME_SHORT; // 300
    ISHELL_SetTimer(pMe->m_pShell,settime,(PFNNOTIFY) VideoPlayer_ShowName,pMe);
  
}
/*=================================================================================================================
   刷新播放时间
=================================================================================================================*/   
static void VideoPlayer_RefreshPlayingTick(CVideoPlayer *pMe)
{
    AEERect rc_tick;    
    char    tick_time[MAX_STR_LEN];
    AECHAR  Wtick_time[MAX_STR_LEN]; 
    
    //在临近结束和开始的时候不允许暂停，否则可能出错
    if(pMe->bTotalTime == pMe->bCurrentTime || pMe->bCurrentTime == 0)
    {
        pMe->PauseLock = TRUE;
    }
    else
    {
        pMe->PauseLock = FALSE; 
    }      
	
    if (pMe->TickUpdateImg[IDI_TIME_PART_PRELOAD]!=NULL)
    {
        //画时间显示区域
#if defined(FEATURE_DISP_128X160)
#elif defined(FEATURE_DISP_220X176)
#elif defined(FEATURE_DISP_240X320)
#elif defined(FEATURE_DISP_176X220)
#else 
    	IIMAGE_Draw(pMe->TickUpdateImg[IDI_TIME_PART_PRELOAD], VIDEOPLAYER_TIME_X, VIDEOPLAYER_TIME_Y); 
    	IIMAGE_Draw(pMe->TickUpdateImg[IDI_TIME_PART_PRELOAD], VIDEOPLAYER_TIME_X+85, VIDEOPLAYER_TIME_Y); 
 #endif
    }
	
	SETAEERECT(&rc_tick, VIDEOPLAYER_TIME_X, VIDEOPLAYER_TIME_Y, VIDEOPLAYER_TIME_W, VIDEOPLAYER_TIME_H);
    IDISPLAY_FillRect(pMe->m_pDisplay,&rc_tick,0x0);
    
    MEMSET(tick_time,0,MAX_STR_LEN);
	
#if defined (FEATURE_DISP_240X320)||defined (FEATURE_DISP_220X176)||defined(FEATURE_DISP_176X220)
    SPRINTF(tick_time,"%02d:%02d/", pMe->bCurrentTime/60,pMe->bCurrentTime%60);
#else
    SPRINTF(tick_time,"%02d:%02d", pMe->bCurrentTime/60,pMe->bCurrentTime%60);
#endif
    STRTOWSTR(tick_time, Wtick_time, sizeof(Wtick_time));

#ifndef FEATURE_DISP_128X160
#if defined (FEATURE_DISP_240X320)||defined (FEATURE_DISP_220X176)||defined(FEATURE_DISP_176X220)
	DrawTextWithProfile(pMe->m_pShell, 
                    pMe->m_pDisplay, 
                    RGB_WHITE, 
                    AEE_FONT_BOLD,
                    Wtick_time, 
                    -1, 
                    0, 
                    0, 
                    &rc_tick, 
                    IDF_ALIGN_CENTER|IDF_ALIGN_MIDDLE|IDF_TEXT_TRANSPARENT);
#else
	DrawTextWithProfile(pMe->m_pShell, 
                        pMe->m_pDisplay, 
                        RGB_WHITE, 
                        AEE_FONT_BOLD,
                        Wtick_time, 
                        -1, 
                        21, 
                        57, 
                        &rc_tick, 
                        IDF_ALIGN_CENTER|IDF_ALIGN_MIDDLE|IDF_TEXT_TRANSPARENT);
#endif
#else
#endif

#if defined(FEATURE_DISP_128X160)
	SETAEERECT(&rc_tick, VIDEOPLAYER_TIME_X, VIDEOPLAYER_TIME_Y, VIDEOPLAYER_TIME_W, VIDEOPLAYER_TIME_H);
	IDISPLAY_FillRect(pMe->m_pDisplay,&rc_tick,0x0);
#elif defined(FEATURE_DISP_220X176)
	SETAEERECT(&rc_tick, SCR_W-VIDEOPLAYER_TIME_W, VIDEOPLAYER_TIME_Y, VIDEOPLAYER_TIME_W, VIDEOPLAYER_TIME_H);
	IDISPLAY_FillRect(pMe->m_pDisplay,&rc_tick,0x0);
#elif defined(FEATURE_DISP_176X220)
	SETAEERECT(&rc_tick, SCR_W-VIDEOPLAYER_TIME_W, VIDEOPLAYER_TIME_Y, VIDEOPLAYER_TIME_W, VIDEOPLAYER_TIME_H);
    IDISPLAY_FillRect(pMe->m_pDisplay,&rc_tick,0x0);
#elif defined(FEATURE_DISP_240X320)
	SETAEERECT(&rc_tick, 120, VIDEOPLAYER_TIME_Y, VIDEOPLAYER_TIME_W, VIDEOPLAYER_TIME_H);
    IDISPLAY_FillRect(pMe->m_pDisplay,&rc_tick,0x0);
#else
	SETAEERECT(&rc_tick, VIDEOPLAYER_TIME_X+85, VIDEOPLAYER_TIME_Y, VIDEOPLAYER_TIME_W, VIDEOPLAYER_TIME_H);
#endif
	MEMSET(tick_time, 0, MAX_STR_LEN);
	MEMSET(Wtick_time, 0, sizeof(Wtick_time));
	SPRINTF(tick_time, "%02d:%02d", pMe->bTotalTime/60,pMe->bTotalTime%60);
	STRTOWSTR(tick_time, Wtick_time, sizeof(Wtick_time));
	
#if defined (FEATURE_DISP_240X320)||defined (FEATURE_DISP_220X176)||defined(FEATURE_DISP_176X220)
	DrawTextWithProfile(pMe->m_pShell, 
                    pMe->m_pDisplay, 
                    RGB_WHITE, 
                    AEE_FONT_BOLD,
                    Wtick_time, 
                    -1, 
                    0, 
                    0, 
                    &rc_tick, 
                    IDF_ALIGN_CENTER|IDF_ALIGN_MIDDLE|IDF_TEXT_TRANSPARENT);
#else
	DrawTextWithProfile(pMe->m_pShell, 
                        pMe->m_pDisplay, 
                        RGB_WHITE, 
                        AEE_FONT_BOLD,
                        Wtick_time, 
                        -1, 
                        21, 
                        57, 
                        &rc_tick, 
                        IDF_ALIGN_CENTER|IDF_ALIGN_MIDDLE|IDF_TEXT_TRANSPARENT);
#endif
}    

/*=================================================================================================================
  刷新进度条
=================================================================================================================*/
static void VideoPlayer_RefreshScheduleBar(CVideoPlayer *pMe)
{

    AEERect  rc;
    AEERect  Rc;
    AEERect  OldClip;
    AEERect  Clip;
    int16    ma;
	AEEImageInfo pi;
    IImage *iGlider = NULL,*iSchedule=NULL;

	//IImage *image = ISHELL_LoadResImage(pMe->m_pShell, VIDEOPLAYER_IMAGES_RES_FILE, IDI_SCHEDULE_EMPTY);
//	IIMAGE_GetInfo(image, &pi);
    //取小图标图片
    //image =ISHELL_LoadResImage(pMe->m_pShell, VIDEOPLAYER_IMAGES_RES_FILE, IDI_GLIDER); 

    if (pMe->TickUpdateImg[IDI_SCHEDULE_EMPTY_PRELOAD]!=NULL)
    {
        iSchedule = pMe->TickUpdateImg[IDI_SCHEDULE_EMPTY_PRELOAD];        
        IIMAGE_Draw(iSchedule, VIDEOPLAYER_SCHEDULE_X, VIDEOPLAYER_SCHEDULE_Y);
        IIMAGE_GetInfo(iSchedule, &pi);
#if defined(FEATURE_DISP_128X160)
    	//VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE, IDI_SCHEDULE_EMPTY, VIDEOPLAYER_SCHEDULE_X, VIDEOPLAYER_SCHEDULE_Y);     	
        SETAEERECT(&rc,VIDEOPLAYER_SCHEDULE_X,VIDEOPLAYER_SCHEDULE_Y,9,9);//滑块起始位置 
        SETAEERECT(&Rc,pi.cx-9,VIDEOPLAYER_SCHEDULE_Y,9,9);//滑块最终位置   
 #elif defined(FEATURE_DISP_220X176)
    	//画进度条
        //VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE, IDI_SCHEDULE_EMPTY, VIDEOPLAYER_SCHEDULE_X, VIDEOPLAYER_SCHEDULE_Y);         
        SETAEERECT(&rc,VIDEOPLAYER_SCHEDULE_X,VIDEOPLAYER_SCHEDULE_Y,5,5);//滑块起始位置 
        SETAEERECT(&Rc,pi.cx-5,VIDEOPLAYER_SCHEDULE_Y,5,5);//滑块最终位置        
 #else
        //画进度条
        //VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE, IDI_SCHEDULE_EMPTY, VIDEOPLAYER_SCHEDULE_X, VIDEOPLAYER_SCHEDULE_Y); 
        SETAEERECT(&rc,VIDEOPLAYER_SCHEDULE_X,VIDEOPLAYER_SCHEDULE_Y,9,9);//滑块起始位置 
		#ifdef FEATURE_DISP_240X320
		SETAEERECT(&Rc,pi.cx+VIDEOPLAYER_SCHEDULE_X-9,VIDEOPLAYER_SCHEDULE_Y,9,9);//滑块最终位置  
		#else
        SETAEERECT(&Rc,pi.cx-9,VIDEOPLAYER_SCHEDULE_Y,9,9);//滑块最终位置   
		#endif 
 #endif
    }
	
    ma=Rc.x-rc.x;//滑块可以移动的长度，26个像素  
    if(pMe->bCurrentTime == 0)
    {
        pMe->bCurrentTime=pMe->bOldTime;
    }
    else
    {
        pMe->bOldTime=pMe->bCurrentTime;
    }

        
    if(pMe->bTotalTime == 0)
    {
        Clip.x=rc.x;        
    }   
    else 
    {
        Clip.x=rc.x + pMe->bCurrentTime * ma / pMe->bTotalTime;
    }
    
    if(pMe->bCurrentTime > pMe->bTotalTime)
    {
        Clip.x=Rc.x;        
    }

    //VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE, IDI_GLIDER, Clip.x, 174);
    
    IDISPLAY_GetClipRect(pMe->m_pDisplay, &OldClip);//restore clip rect
    #if defined( FEATURE_DISP_128X160)
    SETAEERECT(&Clip,Clip.x,VIDEOPLAYER_SCHEDULE_Y,9,9);//滑块移动时的位置  
    #elif defined(FEATURE_DISP_220X176)
	SETAEERECT(&Clip,Clip.x,VIDEOPLAYER_SCHEDULE_Y,5,5);//滑块移动时的位置  
	#else
	SETAEERECT(&Clip,Clip.x,VIDEOPLAYER_SCHEDULE_Y,9,9);//滑块移动时的位置  
	#endif
    IDISPLAY_SetClipRect(pMe->m_pDisplay, &Clip);
    if (pMe->TickUpdateImg[IDI_GLIDER_PRELOAD]!=NULL)
    {
        iGlider = pMe->TickUpdateImg[IDI_GLIDER_PRELOAD];
  #if defined( FEATURE_DISP_128X160)
    	IIMAGE_Draw(iGlider,Clip.x,VIDEOPLAYER_SCHEDULE_Y);  
 #elif defined(FEATURE_DISP_220X176)
    	IIMAGE_Draw(iGlider,Clip.x,VIDEOPLAYER_SCHEDULE_Y+1);
 #else
        IIMAGE_Draw(iGlider,Clip.x,VIDEOPLAYER_SCHEDULE_Y);    
 #endif
    }

    //IMAGE_Release(image);
    IDISPLAY_SetClipRect(pMe->m_pDisplay, &OldClip);//recover clip rect

}
/*=================================================================================================================
  刷新音量bar
 ================================================================================================================*/
static void VideoPlayer_RefreshVolBar(CVideoPlayer *pMe)
{
    uint16 ResID;   
    switch ((int)pMe->m_bVolumeLevel)
    {
        case 0:
            ResID = IDI_VOLUME2_0;//图片资源ID 
            break;
            
        case 1:
            ResID = IDI_VOLUME2_1;
            break;
            
        case 2:
            ResID = IDI_VOLUME2_2;
            break;
            
        case 3:
            ResID = IDI_VOLUME2_3;
            break;
            
        case 4:
            ResID = IDI_VOLUME2_4;
            break;
            
        case 5:
            ResID = IDI_VOLUME2_5;
            break;
        
        default :
            ResID = IDI_VOLUME2_0;            
    }
    VideoPlayer_DrawImage( pMe, VIDEOPLAYER_IMAGES_RES_FILE,ResID, VIDEOPLAYER_VOLUME_X, VIDEOPLAYER_VOLUME_Y);  
}

/*=================================================================================================================
   视频播放的模式:部分 or 全屏
=================================================================================================================*/
boolean VideoPlayer_PlayMod(CVideoPlayer *pMe, uint16 wParam)
{  
  
    if(pMe->IsPlay)
    {
    	MSG_FATAL("pMe->IsFullScreen = %d",pMe->IsFullScreen,0,0);
        if(!pMe->IsFullScreen)
        {
        	MSG_FATAL("---------->FullScreen",0,0,0);
            if((AVK_GSENSOR_STAND != wParam) && (AVK_GSENSOR_UPEND != wParam))
            {    
                //ISHELL_CancelTimer(pMe->m_pShell,NULL,pMe); 
                pMe->IsFullScreen = TRUE;
				//IMEDIA_SetMediaParm((IMedia*)pMe->m_pMedia,MM_PARM_RECT,(int32)&pMe->m_rc,NULL);
				VideoPlayer_ChangeScrState(pMe,TRUE);
				// YY TODO:
                //MMD_LCDRotate(1);
            }
        }
        else
        {   
            if((AVK_GSENSOR_LEFT != wParam) && (AVK_GSENSOR_RIGHT != wParam))
            {   
                pMe->IsFullScreen = FALSE;
                FullScreen = FALSE;
				VideoPlayer_ChangeScrState(pMe,TRUE);
                #if !defined (FEATURE_DISP_240X320) && !defined(FEATURE_DISP_220X176)&& !defined(FEATURE_DISP_176X220)
                VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE,IDI_PLAYERPICTURE_PAUSE, 0, 0);//背景图
                #endif
			    DRAW_BOTTOMBAR(BTBAR_FULLSCREEN_PAUSE_STOP);               
                VideoPlayer_RefreshPlayerFileName(pMe); //刷新文件名               
                VideoPlayer_RefreshPlayingTick(pMe);
                VideoPlayer_RefreshVolBar(pMe);//刷新音量
                VideoPlayer_RefreshScheduleBar(pMe);//刷新进度条
                IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE); 
                
				// YY TODO:
                //MMD_LCDRotate(0);
            }
        return TRUE;
        }
    }    
   
    return TRUE;
}

/*=================================================================================================================
  画帮助的文字
=================================================================================================================*/
void Draw_Parser_Text(CVideoPlayer* pMe,const AECHAR* pText,uint16* height)
{
	AECHAR* pPosCur = (AECHAR*)pText;
	uint16 textw = 0;
	uint16 texth = 1;
	uint16 lh = 1;
	int    pos = 1,spacePos = 0;
    AECHAR entext[2] = {'a','\0'};
	uint16 charw = IDisplay_MeasureText(pMe->m_pDisplay, AEE_FONT_NORMAL,entext);
    uint16 wcharw = (charw<<1);
	#ifdef FEATURE_VERSION_X3
	const uint16 charh = 22;
	#else
    const uint16 charh = 18;
	#endif
	AEERect rc = {0,0,SCR_W,SCR_H};
	#if defined(FEATURE_VERSION_X3) 
	rc.x  = pMe->m_rc.x + 2;
	rc.dx = pMe->m_rc.dx - 2;
    #elif defined(FEATURE_VERSION_1110W516)
    rc.x  = pMe->m_rc.x;
	rc.dx = pMe->m_rc.dx;
	#else
	rc.x  = pMe->m_rc.x + 5;
	rc.dx = pMe->m_rc.dx - 5;
	#endif
    rc.y = VIDEOPLAYER_NAMEPART_H;
    rc.dy = pMe->m_rc.dy - VIDEOPLAYER_NAMEPART_H -  GetBottomBarHeight(pMe->m_pDisplay) - 5;
    #if defined(FEATURE_VERSION_1110W516)
    rc.dy = pMe->m_rc.dy - VIDEOPLAYER_NAMEPART_H -  GetBottomBarHeight(pMe->m_pDisplay);
    #endif
	#ifdef FEATURE_VERSION_X3
	wcharw = 25;
	#endif
    MSG_FATAL("****pji****charw = %d; wcharw=%d",charw,wcharw,0);
	if(pText == NULL) return ;

	IDISPLAY_FillRect(pMe->m_pDisplay,&rc,0x0);

	while(*pPosCur != '\0')
	{
		//if(*pPosCur < 'A'&&*pPosCur > 'z')
		if(*pPosCur == ' ')
		{
		    spacePos = 0;
			if(pos == 1)
			{
			  pPosCur++;
			  continue;
			}
		}
		
		if(*pPosCur == '\\')
		{
			switch(*(pPosCur + 1))
			{
			case 'n':
				{
					DrawTextWithProfile(pMe->m_pShell, 
						pMe->m_pDisplay, 
						RGB_WHITE, //文本轮廓的RGBVAL颜色值,RGB_BLACK
						AEE_FONT_NORMAL,
						pPosCur - pos + 1,
						pos - 1, 
						rc.x, 
						VIDEOPLAYER_NAMEPART_H + texth - pMe->m_pHelp->m_Posy, 
						&rc, 
						IDF_TEXT_TRANSPARENT);
					texth += (lh + charh);
					textw = 0;
					pos = 0;
					pPosCur++;
					break;
				}
			default:
				{
					break;
				}
			}
		}

		if(textw + charw > VIDEO_TEXT_W)
		{
		   if(GetLngCode() != LNG_SCHINESE)
		   	{
	  			DrawTextWithProfile(pMe->m_pShell, 
	  				pMe->m_pDisplay, 
	  				RGB_WHITE, //文本轮廓的RGBVAL颜色值,RGB_BLACK
	  				AEE_FONT_NORMAL,
	  				pPosCur - pos + 1,
	  				pos - spacePos,
	  				rc.x, 
	  				VIDEOPLAYER_NAMEPART_H + texth - pMe->m_pHelp->m_Posy, 
	  				&rc, 
	  				IDF_TEXT_TRANSPARENT);
	  			pPosCur -= spacePos;
		   	}else
		   	{
		   	    DrawTextWithProfile(pMe->m_pShell, 
	  				pMe->m_pDisplay, 
	  				RGB_WHITE, //文本轮廓的RGBVAL颜色值,RGB_BLACK
	  				AEE_FONT_NORMAL,
	  				pPosCur - pos + 1,
	  				pos,
	  				rc.x, 
	  				VIDEOPLAYER_NAMEPART_H + texth - pMe->m_pHelp->m_Posy, 
	  				&rc, 
	  				IDF_TEXT_TRANSPARENT);
		   	}
			
			texth += (lh + charh);
			textw = 0;
			pos = 0;
			

		}

        if(*pPosCur < 0xFF)
        {
		  textw += charw;
        }
		else
		{
		  textw += wcharw;
		}
		
		pPosCur++;
		pos++;
		spacePos++;
	}
	DrawTextWithProfile(pMe->m_pShell, 
		pMe->m_pDisplay, 
		RGB_WHITE, //文本轮廓的RGBVAL颜色值,RGB_BLACK
		AEE_FONT_NORMAL,
		pPosCur - pos + 1,
		pos - 1,
		rc.x, 
		VIDEOPLAYER_NAMEPART_H + texth - pMe->m_pHelp->m_Posy, 
		&rc, 
		IDF_TEXT_TRANSPARENT);
	*height = texth + (lh + charh);
}


/*=================================================================================================================
  构造帮助对象
=================================================================================================================*/
static void VideoPlayer_Help(CVideoPlayer* pMe)
{
  if(pMe->m_pHelp != NULL)
  	return ;
  
  pMe->m_pHelp = (CHelpPtr)MALLOC(sizeof(CHelp));
  if(pMe->m_pHelp == NULL)
  	 return ;
  
  pMe->m_pHelp->pText = NULL;
  pMe->m_pHelp->pTitle = NULL;	
  pMe->m_pHelp->m_Posy= 0;
  pMe->m_pHelp->m_Height = 0;
  pMe->m_pHelp->m_Key = 0;
  
  
  if(!VideoPlayer_HelpInit(pMe))
  	{
  	  SAFE_DELETE(pMe->m_pHelp);
  	}
}

static boolean VideoPlayer_HelpInit(CVideoPlayer* pMe)
{
	CHelpPtr pHelp;
	int      nReturn;

    pHelp = pMe->m_pHelp;
	
	if(!pHelp->pTitle)
	{
	    pHelp->pTitle = (AECHAR*)MALLOC(VIDEOPLAYER_HELP_TITLE);

		if(pHelp->pTitle == NULL)
			return FALSE;
		
		nReturn = ISHELL_LoadResString(pMe->m_pShell,
			AEE_APPSVIDEOPLAYER_RES_FILE,
			IDS_HELPTITLE,
			pHelp->pTitle,
			VIDEOPLAYER_HELP_TITLE);
		
		if(!nReturn)
		  SAFE_DELETE(pHelp->pTitle); 
	}
	if(!pHelp->pText)
	{
	    pHelp->pText = (AECHAR*)MALLOC(VIDEOPLAYER_HELP_TEXT);

		if(pHelp->pText == NULL)
			return FALSE;
		
		nReturn = ISHELL_LoadResString(pMe->m_pShell,
			AEE_APPSVIDEOPLAYER_RES_FILE,
			IDS_HELP,
			pHelp->pText,
			VIDEOPLAYER_HELP_TEXT);
		
		if(!nReturn)
		{
			SAFE_DELETE(pHelp->pTitle);
			return FALSE;
		}
	}
	VideoPlayer_DrawHelp(pMe);
	
	return TRUE;

}
/*=================================================================================================================
  按键转换
=================================================================================================================*/
uint16 VideoPlayer_SearchKey(uint16 wParam)
{
	uint16 tempKey;
	switch(wParam)
		{
		case AVK_CLR:
			{
				tempKey = KEY_CLR;
				break;
			}
		case AVK_UP:
			{
				tempKey = KEY_UP;
				break;
			}
		case AVK_DOWN:
			{
				tempKey = KEY_DOWN;
			}
		default:
			{
				;
			}
		
		}
	return tempKey;
}
/*=================================================================================================================
  画帮助
=================================================================================================================*/
void VideoPlayer_DrawHelp(CVideoPlayer *pMe)
{
	CHelpPtr pHelp = pMe->m_pHelp;

	if(pHelp->m_Key & KEY_UP)
		{
				if((pHelp->m_Posy - VIDEO_SCROLL_SPEED) >= 0)
					pHelp->m_Posy -= VIDEO_SCROLL_SPEED;
		}
	else if(pHelp->m_Key & KEY_DOWN)
		{
				if(pHelp->m_Posy + VIDEO_SCROLL_SPEED< pHelp->m_Height)
				{
				  pHelp->m_Posy += VIDEO_SCROLL_SPEED;
				}	
		}
	
	Draw_Parser_Text(pMe,pHelp->pText,&pHelp->m_Height);
	VideoPlayer_WriteTitle(pMe,pHelp->pTitle);
	DRAW_BOTTOMBAR(BTBAR_BACK); 
	IDISPLAY_Update(pMe->m_pDisplay);
	ISHELL_SetTimer(pMe->m_pShell,50,(PFNNOTIFY)VideoPlayer_DrawHelp,pMe);

}
/*=================================================================================================================
  消息处理函数
=================================================================================================================*/
static boolean VideoPlayer_HelpHandleEvent(CVideoPlayer *pMe,AEEEvent eCode,uint16 wParam,uint32 dwParam)
{
  CHelpPtr pHelp = pMe->m_pHelp;
  uint16 key = 0;
  
  switch(eCode)
  	{
  	    //case EVT_KEY_HELD:
  	    case EVT_KEY_RELEASE:
			{
				key = VideoPlayer_SearchKey(wParam);
				pHelp->m_Key &= (~key);
				return TRUE;
  	    	}
		case EVT_KEY:
			{
				if(wParam == AVK_CLR)
				{
		          ISHELL_CancelTimer(pMe->m_pShell,(PFNNOTIFY)VideoPlayer_DrawHelp,pMe);
				  VideoPlayer_HelpTerminate(pMe);
				  SAFE_DELETE(pMe->m_pHelp);
				  ISHELL_PostEvent(pMe->m_pShell, AEECLSID_VIDEOPLAYER,EVT_USER_REDRAW,  0, 0);
				  return TRUE;
				}
			}
    	case EVT_KEY_PRESS:
			{
				key = VideoPlayer_SearchKey(wParam);
				pHelp->m_Key |= key;
				
			    return TRUE;

				break;
    		}
		case EVT_USER_REDRAW:
			{
				VideoPlayer_DrawHelp(pMe);
				return TRUE;
			}
		default:
			{
				return FALSE;
			}
  	}
  return TRUE;
}

static void VideoPlayer_HelpTerminate(CVideoPlayer *pMe)
{
  SAFE_DELETE(pMe->m_pHelp->pText);
  SAFE_DELETE(pMe->m_pHelp->pTitle);
}

#if 0
/*=================================================================================================================
   删除当前播放的文件
==================================================================================================================*/
static boolean VideoPlayer_DeleteVideo(CVideoPlayer *pMe)
{
    uint32  NumOfFile;//总文件数

    NumOfFile=1;//从GALLERY里获取

    if(!pMe->UserStop)
    {
        pMe->UserStop=TRUE;        
        //IMEDIA_Stop(pMe->m_pMedia);
    }
    VideoPlayer_ReleaseVideo(pMe);
    //if(成功删除文件)
    //{
    //    pMe->m_MenuSelected++;
          NumOfFile= NumOfFile-1; 
    //}
    switch(NumOfFile)
    {
        case 0:
            return FALSE;         
            
        default:
            //因为底层使用的是事件传递机制，需要给底层stop并释放Video 的时间，
            //否则会出错，此处等1秒钟后初始化下一首
            (void) ISHELL_SetTimer(pMe->m_pShell,1000,(PFNNOTIFY) VideoPlayer_InitVideo,pMe);
            break;
            
    }
    return TRUE;
}
#endif

/*=================================================================================================================
   接受CMX底层回调
=================================================================================================================*/ 
static void VideoPlayer_VideoNotify(void * pUser, AEEMediaCmdNotify * pCmdNotify)
{
    CVideoPlayer *   pMe = (CVideoPlayer *)pUser; 

	MSG_FATAL("VideoPlayer_VideoNotify pCmdNotify->nStatus=%d,pCmdNotify->nCmd=%d",pCmdNotify->nStatus,pCmdNotify->nCmd,0);
    switch (pCmdNotify->nStatus)
    {   
        case MM_STATUS_SEEK_FAIL:     
        case MM_STATUS_PAUSE_FAIL:
        case MM_STATUS_RESUME_FAIL:
        default: 
            break;
    }
    
    //播放视频
    if (pCmdNotify->nCmd == MM_CMD_PLAY)
    {
        switch (pCmdNotify->nStatus)
        {   
            // playback done
            case MM_STATUS_DONE: 
                IMEDIA_Stop((IMedia*)pMe->m_pMedia);
                SetDeviceState(DEVICE_TYPE_MP4,DEVICE_MP4_STATE_OFF); 
                pMe->IsPlay=FALSE;
                pMe->IsPause=FALSE;
                pMe->UserStop=TRUE; 
                pMe->m_bAppIsReady=FALSE;
                videoplayer_play_flag = FALSE;
        
                pMe->bCurrentTime=0;
                pMe->IsFullScreen=FALSE;                   
                VideoPlayer_RefreshPlayingTick(pMe);
                VideoPlayer_RefreshPlayerFileName(pMe);
                VideoPlayer_RefreshScheduleBar(pMe);//刷新进度条
                VideoPlayer_RefreshVolBar(pMe);// 刷新音量
#if defined (FEATURE_DISP_240X320)||defined(FEATURE_DISP_220X176)||defined(FEATURE_DISP_176X220)
                VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE, IDI_PLAY, VIDEOPLAYER_PLAY_X,VIDEOPLAYER_PLAY_Y); //"|>"
#endif

				pMe->m_rtype = TYPE_PLAYER;
                
                if(pMe->m_IsPlaynext)
                {
                    DRAW_BOTTOMBAR(BTBAR_GALLERY_PLAY_BACK);
                }
                else
                {
                    DRAW_BOTTOMBAR(BTBAR_PLAY_BACK);
                }
                
                IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE);                                    
                break;
            case MM_MP4_STATUS_OPEN_COMPLETE:
                //(void)IMEDIA_SetAudioDevice((IMedia *)pMe->m_pMedia, AEE_SOUND_DEVICE_SPEAKER);                
				IMEDIA_SetVolume((IMedia *)pMe->m_pMedia, 50);
                break;
                
            // playback aborted,异常中断
            case MM_STATUS_ABORT:
                CLOSE_DIALOG(DLGRET_MSGBOX); // 提示播放失败后退出  
                break;  
            
            case MM_STATUS_PAUSE:
                //取暂停时间,以便挂起时恢复到当前位置
                pMe->m_bPauseTime= (uint32)pCmdNotify->pCmdData;        
                break;

            case MM_STATUS_RESUME:
                //取恢复时的时间,以便挂起时恢复到当前位置
                //pMe->m_bResumeTime= (uint32)pCmdNotify->pCmdData;
                               
            //播放的时候每秒会发上来一次
            case MM_STATUS_TICK_UPDATE: 
                MSG_FATAL("b_is_GetFrame========%d",b_is_GetFrame,0,0);
				//(void)IMEDIA_SetAudioDevice((IMedia *)pMe->m_pMedia, AEE_SOUND_DEVICE_SPEAKER);                
				//(void)IMEDIA_SetVolume((IMedia *)pMe->m_pMedia, pMe->totalvolume);
                pMe->bCurrentTime=((uint32)pCmdNotify->pCmdData) / 1000;  
				if(!b_is_GetFrame && pMe->bCurrentTime>0)
				{
					if(pMe->IsGallery)
            		{
                		VPDVideoPlayer_HandleKeyEvent(pMe,EVT_KEY,AVK_CLR,0); 
            		}
					CLOSE_DIALOG(DLGRET_FAILD)
					break;
				}
				MSG_FATAL("pMe->bCurrentTime==%d",pMe->bCurrentTime,0,0);
                if(!pMe->IsFullScreen)
                {
                    VideoPlayer_RefreshScheduleBar(pMe);//刷新进度条
                    VideoPlayer_RefreshPlayingTick(pMe);  
                    IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE);
                }                        
                break; 
                        
            default:
                break;        
        }
    } 
    
    //return from IMEDIA_GetTotalTime
    if(pCmdNotify->nCmd == MM_CMD_GETTOTALTIME) 
    {
        pMe->bTotalTime = ((uint32)pCmdNotify->pCmdData) / 1000;
    }         
}
/*=================================================================================================================
函数:
    DialogTimeoutCallback

说明:
    传递给BREW层的回调函数，以便设定的定时器超时发出超时事件通知对话框。

参数:
    pUser [in]: 必须是指向 videoplayer Applet对象结构的指针。

返回值:
    none

备注:

=================================================================================================================*/
static void DialogTimeoutCallback(void *pUser)
{
    CVideoPlayer *pMe = (CVideoPlayer*)pUser;  

    if (NULL == pMe)
    {
        return;
    }
    
    if (ISHELL_ActiveApplet(pMe->m_pShell) != AEECLSID_VIDEOPLAYER)
    {
        return;
    }
    (void) ISHELL_PostEvent(pMe->m_pShell,AEECLSID_VIDEOPLAYER,EVT_DISPLAYDIALOGTIMEOUT,0,0);
                         
}
#if 1
// 获取指定视频的名字
static int VideoPlayer_GetFileID(CVideoPlayer *pMe)
{   
    MGFileInfo   pInfo;
    uint16    fileID=0;//ufileID;
    
    if(pMe->m_pFileDB!=NULL)
    {
        IDATABASE_Release(pMe->m_pFileDB);
        pMe->m_pFileDB = NULL;
     }
 
    //打开video数据库
    CMediaGallery_OpenFileInfoDatabase(pMe->m_pShell,MG_VIDEOFILE_DB_NAME,TRUE,&pMe->m_pFileDB);
    if(pMe->m_pFileDB !=NULL)
    {
        // 获取数据库中记录的个数
        pMe->m_RecordCount=IDATABASE_GetRecordCount(pMe->m_pFileDB);  
        DBGPRINTF("pMe->m_RecordCount=%s",pMe->m_RecordCount);
    }
    #if 1
    for(fileID=0;fileID < pMe->m_RecordCount; fileID++)
    {
        CMediaGallery_GetFileInfoRecord(pMe->m_pFileDB,fileID, &pInfo);  
        if(STRCMP(pInfo.szName,pMe->m_FileToPlay)== 0)
        {
            break;
        }
    }  
    #endif
	MSG_FATAL("------>fileID = %d" ,fileID,0,0);
    return fileID;

    #if 0
    if(SUCCESS != IFILEMGR_EnumInit(pMe->pIFileMgr,const char * pszDir,FALSE))
    {
        return EFAILED;
    }  
    for(;;)
    {           
        ret= IFILEMGR_EnumNext(pMe->pIFileMgr,&pInfo);
        
        if(ret!=SUCCESS)
        {
            break;
        }
        //将文件的名字放在FileName中,以便在播放上下首时可以取得文件名进行初始化
       
    }   
    #endif

}

#endif



boolean VideoPlayer_ChangeScrState(CVideoPlayer* pMe,boolean isLockScr)
{
	AEERect rc;
	int result = SUCCESS;
	boolean ret_val = FALSE;
	MEMSET(&rc,NULL,sizeof(rc));

	if(isLockScr)
	{
		if(pMe->IsFullScreen)
		{
			rc.x = pMe->m_rc.x;
			rc.dx = pMe->m_rc.dx;
			rc.y =  pMe->m_rc.y;
			rc.dy = pMe->m_rc.dy;

            if(rc.dx < rc.dy)
            {
                 AEEMediaMPEG4Spec *pMP4Spec = NULL;
                 int32 nSpecSize = 0;

                 IMedia_GetMediaParm((IMedia*)pMe->m_pMedia,MM_MP4_PARM_MEDIA_SPEC,(int32 *)&pMP4Spec,(int32 *)&nSpecSize);
                 MSG_FATAL("VideoPlayer_ChangeScrState-1--dx=%d---dy=%d",pMP4Spec->dwWidth,pMP4Spec->dwHeight,0);
                 if(pMP4Spec->dwWidth > pMP4Spec->dwHeight)
                 {
                    result = IMEDIA_SetMediaParm((IMedia*)pMe->m_pMedia, MM_MP4_PARM_ROTATION, MM_MPEG4_90_CW_ROTATION, 0);
				 }
                result += IMEDIA_SetMediaParm((IMedia*)pMe->m_pMedia, MM_MP4_PARM_ASCALING, rc.dx, rc.dy);

            }
            else
            {
                result = IMEDIA_SetMediaParm((IMedia*)pMe->m_pMedia, MM_MP4_PARM_ASCALING, rc.dx, rc.dy);
            }
			//
			
			if(result == SUCCESS || (result  == MM_PENDING))
			{
				result = IMEDIA_SetMediaParm((IMedia*)pMe->m_pMedia,MM_PARM_RECT,(int32)&rc,NULL);								
			}
		}
		else
		{
            //int yDelta;
            //int dy;
            result = IMEDIA_SetMediaParm((IMedia*)pMe->m_pMedia, MM_MP4_PARM_ROTATION, MM_MPEG4_NO_ROTATION, 0);
            result+= IMEDIA_SetMediaParm((IMedia*)pMe->m_pMedia, MM_MP4_PARM_SCALING, MM_MPEG4_NO_SCALING, 0);
			
			rc.x    = pMe->m_rc.x;
			rc.dx   = pMe->m_rc.dx;
			rc.y    = VIDEOPLAYER_NAMEPART_H;
			#if defined(FEATURE_DISP_128X160)
			rc.dy   = pMe->m_rc.dy - VIDEOPLAYER_NAMEPART_H - 26-  GetBottomBarHeight(pMe->m_pDisplay);
			#elif defined(FEATURE_DISP_220X176)
			rc.dy   = pMe->m_rc.dy - VIDEOPLAYER_NAMEPART_H - 27 -  GetBottomBarHeight(pMe->m_pDisplay);
            IMEDIA_SetMediaParm((IMedia*)pMe->m_pMedia, MM_MP4_PARM_SCALING, MM_MPEG4_0P75X_SHRINK, 0);
			#elif defined(FEATURE_DISP_176X220)
			rc.dy   = pMe->m_rc.dy - VIDEOPLAYER_NAMEPART_H -  GetBottomBarHeight(pMe->m_pDisplay)-VIDEOPLAYER_PLAY_H-10;
//          IMEDIA_SetMediaParm((IMedia*)pMe->m_pMedia, MM_MP4_PARM_SCALING, MM_MPEG4_0P75X_SHRINK, 0);
            #elif defined(FEATURE_DISP_240X320)
            rc.dy   = pMe->m_rc.dy - VIDEOPLAYER_TIME_W -  GetBottomBarHeight(pMe->m_pDisplay)-VIDEOPLAYER_PLAY_H-10;
			#else
			rc.dy   = pMe->m_rc.dy - VIDEOPLAYER_NAMEPART_H -  GetBottomBarHeight(pMe->m_pDisplay);
			#endif
            //dy      = (rc.dx*3)/4;
            //yDelta  = rc.dy - dy;

            //if(yDelta > 0)
            //{
            //    rc.y += (yDelta/2);
            //    rc.dy = dy;
            //    result += IMEDIA_SetMediaParm((IMedia*)pMe->m_pMedia, MM_MP4_PARM_ASCALING, rc.dx, rc.dy);
            //}
			
			if(result == SUCCESS || (result  == MM_PENDING))
			{
				result = IMEDIA_SetMediaParm((IMedia*)pMe->m_pMedia,MM_PARM_RECT,(int32)&rc,NULL);						
			}
		}
	}
	
	return result;
}

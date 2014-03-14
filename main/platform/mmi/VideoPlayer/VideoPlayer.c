/*=================================================================================================================
// 文件：VideoPlayer.c
//        
//	  版权所有(c) 2004 Anylook
//	
//
// 文件说明：
//	  
//        
// 作者：Gemsea
// 创建日期：2008/10/14
=================================================================================================================*/

/*=================================================================================================================
修改历史记录：
时       间      修 改 人     修改内容、位置及原因
-----------      ----------     ------------------------------------------------
2008/10/14                    初始版本
=================================================================================================================*/


/*=================================================================================================================
                         本文件包含的外部文件
=================================================================================================================*/

#include "VideoPlayer_priv.h"      
#include "AEE_OEMEvent.h"
#include "AEEHeap.h"
/*=================================================================================================================
              定义（宏定义及数据类型提前声明）和常数
=================================================================================================================*/

/*=================================================================================================================
                                类型定义
=================================================================================================================*/

/*================================================================================================================
                                  函数声明                                            
=================================================================================================================*/

int VideoPlayerMod_Load(IShell *ps, void * pHelpers, IModule ** pMod);

static int  VideoPlayer_CreateInstance(AEECLSID ClsId,IShell *pIShell,IModule *po,void **ppObj);

static int VideoPlayer_New(IShell * pIShell,IModule * po,void ** ppObj);

static uint32  VideoPlayer_AddRef(IVideoPlayer *pi);

static uint32  VideoPlayer_Release (IVideoPlayer *pi);

static boolean VideoPlayer_HandleEvent(IVideoPlayer *pi,AEEEvent eCode,uint16 wParam,uint32  dwParam);

int VideoPlayer_Play(IVideoPlayer *pi,char *VideoName);

static int VideoPlayer_InitAppData(CVideoPlayer *pMe);

static void VideoPlayer_FreeAppData(CVideoPlayer *pMe);

static void VideoPlayer_RunFSM(CVideoPlayer *pMe);

//初始化音量等级
static void VideoPlayer_InitVolumeLevel(CVideoPlayer *pMe);
// YY TODO:
//extern int Rendering_UpdateEx(void);//wlh 20090409 add
/*=================================================================================================================
                                 全局数据
=================================================================================================================*/


/*=================================================================================================================
                                 本地（静态）数据
=================================================================================================================*/
// 只允许一个VideoPlayer实例。每次创建VideoPlayer Applet时，
// 返回同一结构指针给BREW层。
static CVideoPlayer gVideoPlayer;

//static const VTBL(IVideoPlayer) gVideoPlayerMethods = CONTAPP_METHODS;


static const VTBL(IVideoPlayer) gVideoPlayerMethods =
{
    VideoPlayer_AddRef,
    VideoPlayer_Release,
    VideoPlayer_HandleEvent,    
    VideoPlayer_Play   
};

boolean videoplayer_play_flag = FALSE;

/*----------------------模块相关函数定义---------------------*/
/*=================================================================================================================
函数:
       VideoPlayerMod_Load

说明:
       模块装载函数。

参数:
       pIShell [in]：IShell接口指针。
       ph ：没有使用
       ppMod [out]：

返回值:
       int

备注:

=================================================================================================================*/
#if defined(AEE_STATIC)
int VideoPlayerMod_Load(IShell *ps, void * pHelpers, IModule ** pMod)
{
    return(AEEStaticMod_New((int16)(sizeof(AEEMod)),
                             ps,
                             pHelpers,
                             pMod,
                             (PFNMODCREATEINST)VideoPlayer_CreateInstance,
                             (PFNFREEMODDATA)NULL));
}
#endif

/*=================================================================================================================
函数:
       VideoPlayerMod_CreateInstance

说明:
        通过创建(初始化)VideoPlayer对象来创建模块实例。

参数:
       po [in]：IModule接口对象指针。
       pIShell [in]：IShell接口对象指针。
       ClsId [in]：要创建的实例的类ID。
       ppObj [out]:：用于保存对象实例的指针的指针变量。

返回值:
       EFAILED;操作失败。
       SUCCESS;操作成功。

备注:
       不需用户去调用。

=================================================================================================================*/
#if defined(AEE_STATIC)
static int VideoPlayer_CreateInstance(AEECLSID  ClsId,IShell *pIShell,IModule  *po,void  **ppObj)
#else
int AEEClsCreateInstance(AEECLSID ClsId,IShell  * pIShell, IModule * po,void  ** ppObj)
#endif
{
    *ppObj = NULL;

    if(ClsId != AEECLSID_VIDEOPLAYER)
    {
        return EFAILED;
    }

    if(VideoPlayer_New(pIShell, po, ppObj) != SUCCESS)
    {
        return EFAILED;
    }
    return SUCCESS;
}
/*----------------------Applet相关函数定义---------------------*/
/*=================================================================================================================
函数:
       VideoPlayer_New

说明:
        初始化VideoPlayer Applet虚拟表和内部数据变量，同时其引用计数加1。

参数:
       ps [in]：IShell接口对象指针。
       pIModule [in]：pIModule接口对象指针。
       ppObj [out]:：指向IClockApps对象指针的指针。

返回值:
       EFAILED：失败。
       SUCCESS：成功。

备注:
       不需用户去调用。
       关于Applet结构对象可使用动态分配内存的方式创建，本处采用的是使用静态
       变量的方式。建议对结构比较大的Applet使用静态变量的方式。对于动态分配
       内存的方式需要注意对所分配空间的释放。

=================================================================================================================*/
static int VideoPlayer_New(IShell *ps, IModule *pIModule, void **ppObj)
{
    CVideoPlayer *pMe;
    int retVal = SUCCESS;
   
    if(!ppObj || !ps || !pIModule)
    {      
        return EFAILED;
    }
#if defined(AEE_STATIC)
    pMe = &gVideoPlayer;
#else
    pMe = (CVideoPlayer *)MALLOC(sizeof(CVideoPlayer) + sizeof(VTBL(IVideoPlayer)));
#endif
    if (NULL == pMe)
    {
        return EFAILED;
    }
    if (0 == pMe->m_nRefs)
    {
    #if defined(AEE_STATIC)
        // Must initialize the object
        MEMSET(pMe,  0, sizeof(CVideoPlayer));
        INIT_VTBL(pMe, IVideoPlayer, gVideoPlayerMethods);
    #else
        //NOTE: In the dynamic applet, the extend classes will re-malloc the memery.
        //      So it run in simulator for test only. If you want run in simulator 
        //      correcttly Please modify the following code。       
        VTBL(IVideoPlayer) *pMod = (VTBL(IVideoPlayer) *)((byte *)pMe + sizeof(CVideoPlayer));
        MEMCPY((byte *)pMod, (byte *)&gVideoPlayerMethods, sizeof(VTBL(IVideoPlayer)));
        INIT_VTBL( pMe,IVideoPlayer,*pMod);
    #endif

        pMe->m_nRefs     = 0;
        pMe->m_pShell    = ps;
        pMe->m_pModule   = pIModule;
        (void) ISHELL_AddRef(pMe->m_pShell);
        (void) IMODULE_AddRef(pMe->m_pModule);

        retVal = VideoPlayer_InitAppData(pMe);
        if( SUCCESS != retVal )
        {
            VideoPlayer_FreeAppData(pMe);
            return retVal;
        }
    }
    
    ++pMe->m_nRefs;
    *ppObj = (IVideoPlayer*)pMe;

    return retVal;
}

/*=================================================================================================================
函数:
       VideoPlayer_AddRef

说明:
       VideoPlayer Applet 引用计数加1，并返回当前的引用数。

参数:
       pi [in]：IVideoPlayer接口对象指针。

返回值:
       当前对VideoPlayer Applet的引用数

备注:

=================================================================================================================*/
static uint32  VideoPlayer_AddRef(IVideoPlayer *pi)
{
    CVideoPlayer *pMe = (CVideoPlayer*)pi;

    ASSERT(pMe != NULL);
    ASSERT(pMe->m_nRefs > 0);
    //ERR("m_nRefs+ IS = %d",pMe->m_nRefs,0,0);

    return (++pMe->m_nRefs);
}

/*=================================================================================================================
函数:
       VideoPlayer_Release

说明:
       释放对VideoPlayer Applet的引用，引用计数减1，并返回当前的引用数。
        如果当前的引用数为零，还将释放Applet及相关资源。

参数:
       pi [in]：IVideoPlayer接口对象指针。

返回值:
       当前对VideoPlayer Applet的引用数

备注:

=================================================================================================================*/
static uint32  VideoPlayer_Release (IVideoPlayer *pi)
{
    register CVideoPlayer *pMe = (CVideoPlayer*)pi;

    MSG_FATAL("VideoPlayer_Release",0,0,0);
    ASSERT(pMe != NULL);

    if(pMe->m_nRefs == 0)
    {
        return 0;
    }

    if(--pMe->m_nRefs)
    {
        return pMe->m_nRefs;
    }

    // 释放Applet相关资源
    VideoPlayer_FreeAppData(pMe);

    (void) ISHELL_Release(pMe->m_pShell);
    (void) IMODULE_Release(pMe->m_pModule);

    // 注意：pMe是静态分配空间，因此不需要释放。FREE()
    #if !defined(AEE_STATIC)
    (void)FREE(pMe); 
    #endif
    return 0;
}

/*=================================================================================================================
函数:
       VideoPlayer_InitAppData

说明:
       初始化VideoPlayer Applet结构数据缺省值。

参数:
       pMe [in]：指向VideoPlayer Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
       SUCCESS：初始化成功。
       EFAILED：初始化失败。

备注:

=================================================================================================================*/
static int VideoPlayer_InitAppData(CVideoPlayer *pMe)
{
    AEEDeviceInfo  di;
    
#if defined(AEE_STATIC)
    ASSERT(pMe != NULL);
#endif
    ISHELL_GetDeviceInfo(pMe->m_pShell, &di);
   
    pMe->m_rc.x         = 0;
    pMe->m_rc.y         = 0;
    pMe->m_rc.dx        = (int16) di.cxScreen;
    pMe->m_rc.dy        = (int16) di.cyScreen;
    
    pMe->m_ePreState    = STATE_NONE;
    pMe->m_eCurState    = STATE_INIT;
    pMe->m_pActiveDlg   = NULL;
    pMe->m_pActiveDlgID = IDD_VIDEOPLAYER;
    pMe->m_eDlgRet      = DLGRET_CREATE;
    pMe->m_bNotOverwriteDlgRet = FALSE;
    pMe->m_bAppIsReady  = FALSE;
    pMe->m_pImage       = NULL;
    pMe->m_pMedia       = NULL;
    pMe->m_pMenuCtl     = NULL;
    pMe->m_FileToPlay   = NULL;
    pMe->m_bSuspending  = FALSE;
    pMe->totalvolume    = 0;

    pMe->UserStop       = FALSE; 
    pMe->PauseLock      = FALSE; 
    pMe->bCurrentTime   = 0;
    pMe->bTotalTime     = 0;
    pMe->bOldTime       = 0;
    pMe->m_IsPlaynext   = FALSE;
    pMe->IsPause        = FALSE;
    pMe->IsFullScreen   = FALSE;
    pMe->IsPlay         = FALSE; 
    pMe->IsGallery      = FALSE; 
    pMe->Is848Busy      = FALSE;
	pMe->m_InitFailed   = TRUE;

#if defined (FEATURE_VERSION_C337) || defined(FEATURE_VERSION_IC241A_MMX)|| defined (FEATURE_VERSION_KK5)
	pMe->keystart_time = 0;	
    pMe->keyend_time = 0;
#endif
    pMe->TickUpdateImg[IDI_SCHEDULE_EMPTY_PRELOAD] = ISHELL_LoadResImage(pMe->m_pShell, VIDEOPLAYER_IMAGES_RES_FILE, IDI_SCHEDULE_EMPTY);
        
    pMe->TickUpdateImg[IDI_GLIDER_PRELOAD] = ISHELL_LoadResImage(pMe->m_pShell, VIDEOPLAYER_IMAGES_RES_FILE, IDI_GLIDER);
    #if !defined(FEATURE_DISP_240X320) && !defined(FEATURE_DISP_220X176)
    pMe->TickUpdateImg[IDI_TIME_PART_PRELOAD] = ISHELL_LoadResImage(pMe->m_pShell, VIDEOPLAYER_IMAGES_RES_FILE, IDI_TIME_PART);
    #endif

    
    //创建需要的接口
    if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,AEECLSID_CONFIG,(void **)&pMe->m_pConfig))
    {
        VideoPlayer_FreeAppData(pMe);                                        
        return EFAILED;                             
    }
    
    /* 因为848处理能力问题，如果在进入多媒体模块后不禁止按键音，则会导致较长时间的声音停顿, 
       给用户体验带来影响.这里增加进入模块时禁止按键音，退出时恢复设置*/       
    (void)ICONFIG_GetItem(pMe->m_pConfig,CFGI_BEEP_VOL, &pMe->m_KeySound,sizeof(byte));    
    pMe->m_CKSound= pMe->m_KeySound;
    if(pMe->m_KeySound != 0)
    {  
        pMe->m_KeySound=0;
        (void)ICONFIG_SetItem(pMe->m_pConfig,CFGI_BEEP_VOL,&pMe->m_KeySound ,sizeof(byte));              
    }
    //设置初始音量
    VideoPlayer_InitVolumeLevel(pMe);
    return SUCCESS;
}

/*=================================================================================================================
函数:
       VideoPlayer_FreeAppData

说明:
       释放VideoPlayer Applet使用的相关资源。

参数:
       pMe [in]：指向VideoPlayer Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
       无。

备注:

=================================================================================================================*/


static void VideoPlayer_FreeAppData(CVideoPlayer *pMe)
{
    MSG_FATAL("VideoPlayer_FreeAppData",0,0,0);
    if (NULL == pMe)
    {
        return ;
    }
    
    if (pMe->TickUpdateImg[IDI_SCHEDULE_EMPTY_PRELOAD]!=NULL)
    {
        IIMAGE_Release(pMe->TickUpdateImg[IDI_SCHEDULE_EMPTY_PRELOAD]);
    }

    
    if (pMe->TickUpdateImg[IDI_GLIDER_PRELOAD]!=NULL)
    {
        
        IIMAGE_Release(pMe->TickUpdateImg[IDI_GLIDER_PRELOAD]);
    }
    #if !defined (FEATURE_DISP_240X320) && !defined(FEATURE_DISP_220X176)
    if (pMe->TickUpdateImg[IDI_TIME_PART_PRELOAD]!=NULL)
    {
        
        IIMAGE_Release(pMe->TickUpdateImg[IDI_TIME_PART_PRELOAD]);
    }
    #endif
    //恢复按键音
    (void)ICONFIG_SetItem(pMe->m_pConfig,CFGI_BEEP_VOL,&pMe->m_CKSound,sizeof(byte));    

    
    if (pMe->m_pConfig)
    {
        ICONFIG_Release(pMe->m_pConfig);
        pMe->m_pConfig = NULL;
    }
#ifdef VIDEOPLAYER_HELP
	//帮助
	if(pMe->m_pHelp != NULL)
	{
	    SAFE_DELETE(pMe->m_pHelp->pText);
        SAFE_DELETE(pMe->m_pHelp->pTitle);
	}
	SAFE_DELETE(pMe->m_pHelp);
#endif
    FREEIF(pMe->m_FileToPlay);
    pMe->m_FileToPlay=NULL;      
    if(NULL != pMe->m_pImage)
    {       
        IIMAGE_Release(pMe->m_pImage);
        pMe->m_pImage = NULL;
    }
    RELEASEIF(pMe->m_pFileDB);
    
   // VidePlayer_SoundRestore();
}

/*=================================================================================================================
函数:
       VideoPlayer_RunFSM

说明:
       VideoPlayer Applet有限状态机引擎。

参数:
       pMe [in]：指向VideoPlayer Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
       无。

备注:

=================================================================================================================*/
static void VideoPlayer_RunFSM(CVideoPlayer *pMe)
{
    NextFSMAction nextFSMAction = NFSMACTION_WAIT;

    for ( ; ; )
    {
     
        nextFSMAction = VideoPlayer_ProcessState(pMe);

        if (pMe->m_bNotOverwriteDlgRet)
        {
            pMe->m_bNotOverwriteDlgRet = FALSE;
        }
        else
        {
            pMe->m_eDlgRet = DLGRET_CREATE;
        }

        if (nextFSMAction == NFSMACTION_WAIT)
        {
            break;
        }
    }
}

/*=================================================================================================================
函数:
       VideoPlayer_HandleEvent

说明:
       VideoPlayer  Applet事件处理主函数。所有属于本Applet的事件，
       都将经由该函数处理。

参数:
       pi [in]：指向IVideoPlayer接口对象的指针。
       eCode：BREW事件代码。
       wParam：与事件相关联的数据。
       dwParam [in]：与事件相关联的数据。

返回值:
       TRUE：传入事件被处理。
       FALSE：传入事件没被处理。

备注:

=================================================================================================================*/
#ifdef FEATRUE_SUPPORT_G_SENSOR
#include "g_sensor.h"
#endif

static boolean VideoPlayer_HandleEvent(IVideoPlayer *pi, AEEEvent  eCode, uint16  wParam,uint32 dwParam)
{
    CVideoPlayer *pMe = (CVideoPlayer*)pi;
    AEEAppStart  *as;

    MSG_FATAL("VideoPlayer_HandleEvent:eCode=%x,wParam=%x",eCode,wParam,0);
    switch (eCode)
    {
        case EVT_APP_START:
		// YY TODO:
			//Rendering_UpdateEx();//wlh add for 3D test
            ASSERT(dwParam != 0);
            as = (AEEAppStart*)dwParam;
//            if (NULL != pMe->m_pDisplay)
//            {
//                (void) IDISPLAY_Release(pMe->m_pDisplay);
//                pMe->m_pDisplay = NULL;
//            }
            pMe->m_pDisplay = as->pDisplay;
//            (void) IDISPLAY_AddRef(pMe->m_pDisplay);       
            pMe->m_rc = as->rc; 
            pMe->m_bSuspending = FALSE;
            pMe->m_bActive = TRUE; 
            pMe->m_bAppIsReady  = FALSE;
            SetDeviceState(DEVICE_TYPE_MP4,DEVICE_MP4_STATE_OFF);
            if(app_media_scheduler() != APP_MEDIA_ALLOW)
            {
                pMe->Is848Busy = TRUE;
            }
            else
            {
                if(pMe->m_pMedia == NULL)
                {
#ifndef WIN32
                    if(as->pszArgs[0]==STARTARGS_GALLERY)
                    {
#endif//WIN32
                        pMe->IsGallery=TRUE;
                        pMe->IsPlay  = TRUE;
                        videoplayer_play_flag = TRUE;
#ifndef WIN32          
                        FREEIF(pMe->m_FileToPlay);
                        pMe->m_FileToPlay = (char *)MALLOC(MAX_STR_LEN*sizeof(char));
                        (void)STRCPY(pMe->m_FileToPlay,&as->pszArgs[1]); 
                        //初始化Media指针并注册回调
                        VideoPlayer_InitVideo(pMe); 
            	    }
#else
	//					VideoPlayer_InitVideo(pMe); 
#endif//WIN32
                }
            }
            {
                dword shake;
                OEM_GetConfig(CFGI_GSENSOR,&shake,sizeof(shake));
                if(shake & OEMNV_SHAKE_VIDEO_MASK)
                {
#ifdef FEATRUE_SUPPORT_G_SENSOR
                    mmi_g_sensor_process(G_SENSOR_ENABLE | G_SENSOR_VIDEO_CIRC);
#endif
                }
            }
            // 开始VideoPlayer状态机           
            VideoPlayer_RunFSM(pMe);            
            return TRUE;         


        case EVT_APP_STOP: 
        {            
            if(pMe->m_pMedia)
			{
				IMEDIA_Stop((IMedia*)pMe->m_pMedia);
			}
			
            VideoPlayer_ReleaseVideo(pMe); 
            (void)ISHELL_CancelTimer(pMe->m_pShell, NULL, pMe);
            SetDeviceState(DEVICE_TYPE_MP4,DEVICE_MP4_STATE_OFF);
             //恢复按键音
            (void)ICONFIG_SetItem(pMe->m_pConfig,CFGI_BEEP_VOL,&pMe->m_CKSound,sizeof(byte));
            
            pMe->m_bAppIsReady  = FALSE;
             videoplayer_play_flag = FALSE;
            //if (pMe->m_pDisplay != NULL)
            //{
            //    (void) IDISPLAY_Release(pMe->m_pDisplay);
            //    pMe->m_pDisplay = NULL;
            //}
            pMe->m_bSuspending = TRUE; 
            {
                dword shake;
                OEM_GetConfig(CFGI_GSENSOR,&shake,sizeof(shake));             
                if(shake & OEMNV_SHAKE_VIDEO_MASK)
                {                    
#ifdef FEATRUE_SUPPORT_G_SENSOR
                    mmi_g_sensor_process(G_SENSOR_VIDEO_CIRC_DISABLE);
#endif
                }
            }
        }
        return TRUE;

        case EVT_APP_SUSPEND:  
            {
                AEERect rc = {0,0,SCR_W,SCR_H};
                SetDeviceState(DEVICE_TYPE_MP4,DEVICE_MP4_STATE_OFF);
                (void)ISHELL_CancelTimer(pMe->m_pShell, NULL, pMe);
                (void)ICONFIG_SetItem(pMe->m_pConfig,CFGI_BEEP_VOL,&pMe->m_CKSound,sizeof(byte));
                pMe->m_bSuspending = TRUE;                               
                
                
                //中断相关操作(自动关机,低电量关机,来电)            
                if(pMe->m_pMedia != NULL)// && !pMe->UserStop)
                {
                    if(pMe->IsFullScreen) // (来电时)全屏界面则回到正常模式
                    {
                        VideoPlayer_PlayMod(pMe, wParam);
                    }
                    //if(pMe->IsPlay)
                    //{
                    //   IMEDIA_Pause((IMedia*)pMe->m_pMedia);
                    //}
                    IMedia_Stop((IMedia*)pMe->m_pMedia);
                }
                
                IDisplay_EraseRect(pMe->m_pDisplay,&rc);
                //RELEASEIF(pMe->m_pDisplay);
                //中断相关操作(自动关机,低电量关机,来电)
            }
            return TRUE;

        case EVT_APP_RESUME:  
            SetDeviceState(DEVICE_TYPE_MP4,DEVICE_MP4_STATE_OFF);           
            (void)ICONFIG_SetItem(pMe->m_pConfig,CFGI_BEEP_VOL,&pMe->m_KeySound,sizeof(byte));
            ASSERT(dwParam != 0);
            as = (AEEAppStart*)dwParam;
            pMe->m_bSuspending = FALSE;          
            //if (pMe->m_pDisplay != NULL)
            //{
            //    (void) IDISPLAY_Release(pMe->m_pDisplay);
            //    pMe->m_pDisplay = NULL;
            //}
            pMe->m_pDisplay = as->pDisplay;
            //(void) IDISPLAY_AddRef(pMe->m_pDisplay);
            pMe->m_rc = as->rc; 
 #if 0        
            if(pMe->m_eCurState != STATE_PLAYER)
            {
                pMe->m_eCurState=STATE_PLAYER;
            } 
#endif      
            //中断相关操作(自动关机,低电量关机,来电)
            if(pMe->IsPause || pMe->IsPlay)
            {
                IMEDIA_Play((IMedia*)pMe->m_pMedia);
                IMEDIA_FastForward((IMedia*)pMe->m_pMedia,pMe->m_bPauseTime);
                IMEDIA_Pause((IMedia*)pMe->m_pMedia);
            }
            //中断相关操作(自动关机,低电量关机,来电)
            if(pMe->m_pMedia == NULL)// 防止中断恢复时会跑到这里
            {
                if(as->pszArgs[0]==STARTARGS_GALLERY)
                {           
                    pMe->IsGallery=TRUE;
                    pMe->IsPlay  = TRUE;
                    videoplayer_play_flag = TRUE;
                    FREEIF(pMe->m_FileToPlay);
                    pMe->m_FileToPlay=(char *)MALLOC(MAX_STR_LEN*sizeof(char));
                    (void)STRCPY(pMe->m_FileToPlay,&as->pszArgs[1]); 
                    //初始化Media指针并注册回调
                    VideoPlayer_InitVideo(pMe);
                }
                else if(pMe->m_FileToPlay != NULL)
                {       
                    pMe->IsGallery=TRUE;
                    pMe->IsPlay  = TRUE;                  
                    pMe->m_IsPlaynext =TRUE;
                    
                    //初始化Media指针并注册回调
                    VideoPlayer_InitVideo(pMe);                
                }
            }
 
            VideoPlayer_RunFSM(pMe);
            return TRUE;       

        case EVT_DIALOG_INIT:
            pMe->m_pActiveDlg = (IDialog*)dwParam;
            pMe->m_pActiveDlgID = wParam;
            pMe->m_bAppIsReady  = FALSE;
            return VideoPlayer_RouteDialogEvent(pMe,eCode,wParam,dwParam);

        case EVT_DIALOG_START:
			// YY TODO:
			//Rendering_UpdateEx();//wlh add for 3D test
            return VideoPlayer_RouteDialogEvent(pMe,eCode,wParam,dwParam);

        case EVT_KEY:
        case EVT_KEY_PRESS:
        case  EVT_KEY_RELEASE:
        case EVT_COMMAND:            
        case EVT_USER_REDRAW:            
            return VideoPlayer_RouteDialogEvent(pMe,eCode,wParam,dwParam); 
            
        case EVT_PLAYNEXT: 
            if(&(char*)dwParam !=NULL)
            {
                /*从videoplayer里进入gallery选择视频播放，播放停止后，再进入gallery，
                  选择视频后接收到playnext事件，则释放前一个pMe->m_FileToPlay和pMe->m_pMedia*/
                FREEIF(pMe->m_FileToPlay);
                pMe->m_FileToPlay=NULL;
                VideoPlayer_ReleaseVideo(pMe);
                pMe->m_FileToPlay=STRDUP((char*)dwParam); 
            }            
            return TRUE;
            
        case EVT_AUTO_POWERDOWN:           
            IMEDIA_Pause((IMedia*)pMe->m_pMedia);
            pMe->IsPlay = FALSE;
            pMe->IsPause = TRUE;
            videoplayer_play_flag = FALSE;
            return  TRUE;

        case EVT_DIALOG_END:   
            DBGPRINTF("EVT_DIALOG_END");
            if (wParam == 0)
            {                
                return TRUE;
            }
            pMe->m_bAppIsReady  = FALSE;
            (void) VideoPlayer_RouteDialogEvent(pMe,eCode,wParam,dwParam);
            pMe->m_pActiveDlg = NULL;

            // Applet被挂起，不跑状态机，等待Applet返回。（注意：EVT_APP_SUSPEND
            // 事件在EVT_DIALOG_END事件前发出。)
            if (pMe->m_bSuspending == FALSE)
            {
                // 开始VideoPlayer状态机
                VideoPlayer_RunFSM(pMe);
            }
            return TRUE;
        case EVT_APP_EXIT:
            ISHELL_CloseApplet(pMe->m_pShell, TRUE);
            return TRUE;
#ifdef FEATURE_LCD_TOUCH_ENABLE//WLH ADD FOR LCD TOUCH
	  case EVT_PEN_UP:
		  {
			  int i;
			  AEEDeviceInfo devinfo;
			  int nBarH ;
			  AEERect rc;
			  int16 wXPos = (int16)AEE_GET_X((const char *)dwParam);
			  int16 wYPos = (int16)AEE_GET_Y((const char *)dwParam);



			  //播放
			  SETAEERECT(&rc,VIDEOPLAYER_PLAY_X,VIDEOPLAYER_PLAY_Y,VIDEOPLAYER_PLAY_W, VIDEOPLAYER_PLAY_H);
			  if(VIDEOPLAYER_PT_IN_RECT(wXPos,wYPos,rc))
			  {
				  return  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_VIDEOPLAYER,EVT_USER,AVK_INFO,0);
			  }
			  //快进
			  SETAEERECT(&rc,VIDEOPLAYER_FORWARD_X,VIDEOPLAYER_FORWARD_Y,VIDEOPLAYER_FORWARD_W, VIDEOPLAYER_FORWARD_H);
			  if(VIDEOPLAYER_PT_IN_RECT(wXPos,wYPos,rc))
			  {
				  return  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_VIDEOPLAYER,EVT_USER,AVK_POUND,0);
			  }
			  //后退
			  SETAEERECT(&rc,VIDEOPLAYER_REWIND_X,VIDEOPLAYER_REWIND_Y,VIDEOPLAYER_REWIND_W, VIDEOPLAYER_REWIND_H);
			  if(VIDEOPLAYER_PT_IN_RECT(wXPos,wYPos,rc))
			  {
				  return  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_VIDEOPLAYER,EVT_USER,AVK_STAR,0);
			  }
			  //前一首
			  SETAEERECT(&rc,VIDEOPLAYER_PREVIOUS_X,VIDEOPLAYER_PREVIOUS_Y,VIDEOPLAYER_PREVIOUS_W, VIDEOPLAYER_PREVIOUS_H);
			  if(VIDEOPLAYER_PT_IN_RECT(wXPos,wYPos,rc))
			  {
				  return  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_VIDEOPLAYER,EVT_USER,AVK_LEFT,0);
			  }
			  //下一首
			  SETAEERECT(&rc,VIDEOPLAYER_NEXT_X,VIDEOPLAYER_NEXT_Y,VIDEOPLAYER_NEXT_W, VIDEOPLAYER_NEXT_H);
			  if(VIDEOPLAYER_PT_IN_RECT(wXPos,wYPos,rc))
			  {
				  return  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_VIDEOPLAYER,EVT_USER,AVK_RIGHT,0);
			  }
              #ifdef FEATURE_DISP_240X320
  			  //增加音量
			  SETAEERECT(&rc,VIDEOPLAYER_VOLUME_ADD_X-15,VIDEOPLAYER_VOLUME_ADD_Y,VIDEOPLAYER_VOLUME_ADD_W+15, VIDEOPLAYER_VOLUME_ADD_H);
			  if(VIDEOPLAYER_PT_IN_RECT(wXPos,wYPos,rc))
			  {
				  return  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_VIDEOPLAYER,EVT_USER,AVK_UP,0);
			  }
			  //降低音量
			  SETAEERECT(&rc,VIDEOPLAYER_VOLUME_LOW_X,VIDEOPLAYER_VOLUME_LOW_Y,VIDEOPLAYER_VOLUME_LOW_W+15, VIDEOPLAYER_VOLUME_LOW_H);
			  if(VIDEOPLAYER_PT_IN_RECT(wXPos,wYPos,rc))
			  {
				  return  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_VIDEOPLAYER,EVT_USER,AVK_DOWN,0);
			  }
              #else
			  //增加音量
			  SETAEERECT(&rc,VIDEOPLAYER_VOLUME_ADD_X,VIDEOPLAYER_VOLUME_ADD_Y,VIDEOPLAYER_VOLUME_ADD_W, VIDEOPLAYER_VOLUME_ADD_H);
			  if(VIDEOPLAYER_PT_IN_RECT(wXPos,wYPos,rc))
			  {
				  return  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_VIDEOPLAYER,EVT_USER,AVK_UP,0);
			  }
			  //降低音量
			  SETAEERECT(&rc,VIDEOPLAYER_VOLUME_LOW_X,VIDEOPLAYER_VOLUME_LOW_Y,VIDEOPLAYER_VOLUME_LOW_W, VIDEOPLAYER_VOLUME_LOW_H);
			  if(VIDEOPLAYER_PT_IN_RECT(wXPos,wYPos,rc))
			  {
				  return  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_VIDEOPLAYER,EVT_USER,AVK_DOWN,0);
			  }
              #endif
			  //全屏
			  //SETAEERECT(&rc,VIDEOPLAYER_FULLSCREEN_X,VIDEOPLAYER_FULLSCREEN_Y,VIDEOPLAYER_FULLSCREEN_W, VIDEOPLAYER_FULLSCREEN_H);
			  //if(VIDEOPLAYER_PT_IN_RECT(wXPos,wYPos,rc))
			  //{
			//	  return  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_VIDEOPLAYER,EVT_USER,AVK_INFO,0);
			  //}
			  //底部操作栏
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
	  case EVT_USER:
		  {
				eCode = EVT_KEY;
		  }
		  return VideoPlayer_RouteDialogEvent(pMe,eCode,wParam,dwParam); 
#endif//FEATURE_LCD_TOUCH_ENABLE            
        default:            
            // 将接收到的事件路由至当前活动的对话框事件处理函数。
            return VideoPlayer_RouteDialogEvent(pMe,eCode,wParam,dwParam);
    }
}
/*=================================================================================================================
函数：VideoPlayer_ReleaseVideo

说明：
       释放Media指针

参数：
       pMe [in]：指向VideooPlayer Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值：
        无

备注：:

=================================================================================================================*/
void VideoPlayer_ReleaseVideo(CVideoPlayer *pMe)
{
    if(pMe->m_pMedia)
    {//restore video music
        (void)IMEDIA_SetAudioDevice((IMedia *)pMe->m_pMedia, HS_HEADSET_ON()?AEE_SOUND_DEVICE_STEREO_HEADSET:AEE_SOUND_DEVICE_HANDSET);        
        MSLEEP(100);
        //VidePlayer_SoundRestore();
        IMEDIA_SetVolume((IMedia *)pMe->m_pMedia, 60);        
        MSLEEP(100);
        (void)IMEDIA_RegisterNotify((IMedia*)pMe->m_pMedia, NULL, pMe);
        IMEDIA_Release((IMedia*)pMe->m_pMedia);
        pMe->m_pMedia = NULL;
    }
} 
/*=================================================================================================================
函数: VideoPlayer_Play

说明:
      提供给GALLERY调用的外部接口,播放选择的视频     
      
参数:
返回值:
备注:
=================================================================================================================*/
int VideoPlayer_Play(IVideoPlayer *pi,char *VideoName)
{
    CVideoPlayer *pMe=(CVideoPlayer *)pi;
    char     *args = NULL;
    int       nRet;
    
    if ( pMe == NULL ) 
    {
        return EFAILED;
    }
    args=(char *)MALLOC((1+MAX_STR_LEN)*sizeof(char));
    args[0]=STARTARGS_GALLERY;
    STRCPY(&args[1],VideoName);
	
    nRet = ISHELL_StartAppletArgs(pMe->m_pShell,AEECLSID_VIDEOPLAYER,args);   
    if (args) 
    {
        FREE(args);
    }   
    
    return nRet;
}

/*=================================================================================================================
   初始化音量等级，音量与当前系统音量一致
=================================================================================================================*/ 
static void VideoPlayer_InitVolumeLevel(CVideoPlayer *pMe)
{
    pMe->m_bVolumeLevel = VOLUME_THREE;
    pMe->totalvolume = pMe->m_bVolumeLevel*VP_VOLUME_STEP;
} 

/*=================================================================================================================
函数: 
       VideoPlayer_APPIsReadyTimer
       
说明: 
       定时器回调函数。主要用于控制快速按键问题的处理。
       
参数: 
       pMe [in]：void *类型指针
       
返回值:
       无。
       
备注:
       
=================================================================================================================*/
void VideoPlayer_APPIsReadyTimer(void *pme)
{
    CVideoPlayer *pMe = (CVideoPlayer *)pme;

    if (NULL == pMe)
    {
        return;
    }

    (void) ISHELL_PostEvent( pMe->m_pShell,
                            AEECLSID_VIDEOPLAYER,
                            EVT_APPISREADY,
                            0,
                            0);
}

boolean is_videoplayer_play(void)
{
    return videoplayer_play_flag;
}
/*
int GetScreenH()
{
  return ((CVideoPlayer *)AEE_GetAppInstance())->m_rc.dy;
}
int GetScreenW()
{
  return ((CVideoPlayer *)AEE_GetAppInstance())->m_rc.dx;
}
*/


/*==============================================================================
// 文件：
//        MusicPlayer.c
//        版权所有(c) 2009 Anylook Tech. CO.,LTD.
//        
// 文件说明：
//        
==============================================================================*/

/*==============================================================================
修改历史记录：
时       间      修 改 人     修改内容、位置及原因
-----------      ----------     -----------------------------------------------
==============================================================================*/


/*==============================================================================
                                 本文件包含的外部文件
==============================================================================*/
#include "MusicPlayer_priv.h"
#include "AEE_OEMEvent.h"
#include "MediaGallery.h"
#include "OEMHeap.h"
/*==============================================================================
                                 宏定义和常数
==============================================================================*/
#ifndef SYSFREEIF
#define SYSFREEIF(p) if (p) { sys_free((void*)p); (p) = 0; }
#endif
/*==============================================================================
                                 类型定义
==============================================================================*/

/*==============================================================================
                                 函数声明
==============================================================================*/
/*----------------------Applet相关函数声明---------------------*/
int MusicPlayer_Load(IShell *ps, void * pHelpers, IModule ** pMod);
#if defined(AEE_STATIC)
static int MusicPlayer_CreateInstance(AEECLSID    ClsId,
                                       IShell     *pIShell,
                                       IModule    *po,
                                       void      **ppObj);
#endif

static int MusicPlayer_New( IShell *ps,
                             IModule *pIModule,
                             void **ppObj);  

static uint32  IMusicPlayer_AddRef(IMusicPlayer *pi);

static uint32  IMusicPlayer_Release(IMusicPlayer *pi);

static boolean IMusicPlayer_HandleEvent(IMusicPlayer *pi,
                                         AEEEvent       eCode,
                                         uint16         wParam,
                                         uint32         dwParam
                                        );
/*供Gallery进行简单播放的接口*/
static int IMusicPlayer_SimplePlayer(IMusicPlayer *pi,char *musicName);
/*供计步器调用播放的接口*/
static int IMusicPlayer_PedometerCallPlayer(IMusicPlayer *pi);
static int CMusicPlayer_InitAppData(CMusicPlayer *pMe);

static void CMusicPlayer_FreeAppData(CMusicPlayer *pMe);

static void CMusicPlayer_RunFSM(CMusicPlayer *pMe);
//主要用于控制快速按键出现的问题
static void CMusicPlayer_AppIsReadyCB(void * pUser);

//中断处理
static void MP3_InterruptHandle(CMusicPlayer *pMe);

/*中断恢复函数*/
static void MP3_ResumeHandle(CMusicPlayer *pMe);

/*关闭键盘音*/
static void MP3_CloseKeyBeepVol(CMusicPlayer *pMe);

/*恢复键盘音*/
static void MP3_ResumeKeyBeepVol(CMusicPlayer *pMe);
static void Music_Shake_Open(void);

static void Music_Shake_Close(void);
static void MP3_DecodeStartArgs(CMusicPlayer *pMe, char *args);
static void MP3_SetStartCurState(CMusicPlayer *pMe);
static void MP3_SetStartStatus(CMusicPlayer *pMe);

static void MP3_Build_DefaultPlaylist(CMusicPlayer *pMe);

/*==============================================================================
                                 全局数据
==============================================================================*/
static Mp3Player_Status g_eMp3PlayerStatus = MP3STATUS_NONE;
#ifdef FEATURE_SUPPORT_BT_APP
static boolean g_bPlayMP3ByBt = FALSE;
#endif
static int16 g_nInterruptRef = 0;
/* 以下代码可为换播放器皮肤等做扩展*/
static AEERect MP3SKIN1_RECT[] = {
               {0,0,},               // 主界面大图坐标
               
			   //{15,10,149,22},       // 音乐名称坐标
#if defined (FEATURE_DISP_128X160)
				{4,10,120,22},       // 音乐名称坐标
#elif defined (FEATURE_DISP_220X176)
				{5,10,210,30},       // 音乐名称坐标  
#elif defined (FEATURE_DISP_320X240)
				{5,10,310,30},       // 音乐名称坐标  				
#else
				{5,10,149,22},       // 音乐名称坐标  
#endif			   
			   {4,134,46,6},        // 进度条坐标
               {9,168,50,18},        // 时间显示坐标
               {126,142,48,18},      // 索引显示坐标
               {82,136,},            // 增加音量坐标
               {82,174,},            // 减少音量坐标
               {79,150,},            // 播放暂停坐标
               {65,153,},            // 上一首坐标
               {101,153,},           // 下一首坐标
               {131,168,},           // 音量显示坐标
               {40,140,},            // 快进坐标
               {3,140,},             // 后退坐标
               {0,0},                   // 快速查看正在播放的列表的图标坐标
               {60,129,52,50}        // 绘制上一首下一首播放暂停等的剪切矩形的坐标
};

/*==============================================================================
                                 本地（静态）数据
==============================================================================*/
#define MUSICPLAYER_METHODS     \
        {                           \
             IMusicPlayer_AddRef,     \
             IMusicPlayer_Release,     \
             IMusicPlayer_HandleEvent,     \
             IMusicPlayer_SimplePlayer,     \
             IMusicPlayer_PedometerCallPlayer   \
        }

#if defined(AEE_STATIC)
// 只允许一个MusicPlayer实例。每次创建MusicPlayer Applet时，
// 返回同一结构指针给BREW层。
static CMusicPlayer gMusicPlayer;

static const VTBL(IMusicPlayer) gMusicPlayerMethods = MUSICPLAYER_METHODS;
#endif

/*==============================================================================
                                 函数定义
==============================================================================*/
/*----------------------Applet相关函数定义---------------------*/
#if defined(AEE_STATIC)
int MusicPlayer_Load(IShell *ps, void * pHelpers, IModule ** pMod)
{
    return(AEEStaticMod_New( (int16)(sizeof(AEEMod)),
                             ps,
                             pHelpers,
                             pMod,
                             (PFNMODCREATEINST)MusicPlayer_CreateInstance,
                             (PFNFREEMODDATA)NULL));
}

#endif

/*==============================================================================
函数: 
       MusicPlayer_CreateInstance
// DESCRIPTION
//      This function is invoked while the app is being loaded. All Modules must provide this 
//  function. Ensure to retain the same name and parameters for this function.
//
// PARAMETERS
//  ClsId: [in]: Specifies the ClassID of the applet which is being loaded
//  pIShell: [in]: Contains pointer to the IShell interface. 
//  pIModule: [in]: Contains pointer to the IModule interface to the current module to which
//  this app belongs
//  ppObj: [out]: On return, *ppObj must point to a valid AEEApplet structure. Allocation
//  of memory for this structure and initializing the base data members is done by AEEApplet_New().
//
// Return Value:
//      
==============================================================================*/
#if defined(AEE_STATIC)
static int MusicPlayer_CreateInstance( AEECLSID    ClsId, 
                                        IShell      *pIShell,
                                        IModule     *po, 
                                        void        **ppObj)
#else
int AEEClsCreateInstance( AEECLSID      ClsId, 
                          IShell        * pIShell, 
                          IModule       * po, 
                          void          ** ppObj)
#endif
{
    if (ClsId == AEECLSID_APP_MUSICPLAYER)
    {
        return MusicPlayer_New(pIShell, po, ppObj);
    }
    else
    {
        //ERR("CLS ID IS NOT",0,0,0);
        return EUNSUPPORTED;
    }
}
/*==============================================================================
函数：MusicPlayer_New

说明：
       初始化MusicPlayer Applet虚拟表和内部数据变量，同时MusicPlayer Applet
       引用计数加1。

参数：
       ps [in]：IShell接口对象指针。
       pIModule [in/out]：pIModule接口对象指针。
       ppObj [out]:：指向IMusicPlayer对象指针的指针。

返回值：
       EFAILED：失败。
       SUCCESS：成功。
       
备注：:
       不需用户去调用。
       关于Applet结构对象可使用动态分配内存的方式创建，本处采用的是使用静态
       变量的方式。建议对结构比较大的Applet使用静态变量的方式。对于动态分配
       内存的方式需要注意对所分配空间的释放。

==============================================================================*/
static int MusicPlayer_New(IShell *ps, IModule *pIModule, void **ppObj)
{
    CMusicPlayer *pMe;
    int retVal = SUCCESS;
    if(!ppObj || !ps || !pIModule)
    {
        //ERR("STH IS NULL",0,0,0);
        return EFAILED;
    }
#if defined(AEE_STATIC)
    pMe = &gMusicPlayer;
#else
    pMe = (CMusicPlayer *)MALLOC(sizeof(CMusicPlayer) + sizeof(VTBL(IMusicPlayer)));
#endif
    if (NULL == pMe)
    {
        return EFAILED;
    }
    if (0 == pMe->m_nRefs)
    {
#if defined(AEE_STATIC)
        // Must initialize the object
        MEMSET(pMe,  0, sizeof(CMusicPlayer));
        INIT_VTBL(pMe, IMusicPlayer, gMusicPlayerMethods);
#else
//NOTE: In the dynamic applet, the extend classes will re-malloc the memery.
//      So it run in simulator for test only. If you want run in simulator correcttly
//      Please modify the following code.
        VTBL(IMusicPlayer) gMusicPlayerMethods = MUSICPLAYER_METHODS;
        VTBL(IMusicPlayer) *pMod = (VTBL(IMusicPlayer) *)((byte *)pMe + sizeof(CMusicPlayer));
        MEMCPY((byte *)pMod, (byte *)&gMusicPlayerMethods, sizeof(VTBL(IMusicPlayer)));
        INIT_VTBL( pMe,IMusicPlayer*pMod);
#endif

        pMe->m_nRefs     = 0;
        pMe->m_pShell    = ps;
        pMe->m_pModule   = pIModule;
        (void) ISHELL_AddRef(pMe->m_pShell);
        (void) IMODULE_AddRef(pMe->m_pModule);

        retVal = CMusicPlayer_InitAppData(pMe);
        if( SUCCESS != retVal )
        {
            CMusicPlayer_FreeAppData(pMe);
            return retVal;
        }
    }
    
    ++pMe->m_nRefs;
    *ppObj = (IMusicPlayer*)pMe;

    return retVal;
}

/*==============================================================================
函数：IMusicPlayer_AddRef

说明：
       MusicPlayer Applet 引用计数加1，并返回当前的引用数。

参数：
       pi [in/out]：IMusicPlayer接口对象指针。

返回值：
       当前对MusicPlayer Applet的引用数

备注：:

==============================================================================*/
static uint32 IMusicPlayer_AddRef(IMusicPlayer *pi)
{
    CMusicPlayer *pMe = (CMusicPlayer*)pi;
#if defined(AEE_STATIC)
    ASSERT(pMe != NULL);
    ASSERT(pMe->m_nRefs > 0);
#endif

    return (++pMe->m_nRefs);
}

/*==============================================================================
函数：IMusicPlayer_Release

说明：
       释放对SounMenu Applet的引用，引用计数减1，并返回当前的引用数。
       如果当前的引用数为零，还将释放Applet及相关资源。

参数：
       pi [in/out]：IMusicPlayer接口对象指针。

返回值：
       当前对MusicPlayer Applet的引用数

备注：:

==============================================================================*/
static uint32  IMusicPlayer_Release (IMusicPlayer *pi)
{
    CMusicPlayer *pMe = (CMusicPlayer*)pi;

#if defined(AEE_STATIC)
    ASSERT(pMe != NULL);
#endif
    if (pMe->m_nRefs == 0)
    {
        return 0;
    }

    if (--pMe->m_nRefs)
    {
        return pMe->m_nRefs;
    }
    // 释放Applet相关资源
    CMusicPlayer_FreeAppData(pMe);
    (void) ISHELL_Release(pMe->m_pShell);
    (void) IMODULE_Release(pMe->m_pModule);

    // 注意：pMe是静态分配空间，因此不需要释放。FREE()
#if !defined(AEE_STATIC)
    (void)FREE(pMe);
#endif
    return 0;
}

/*==============================================================================
函数：CMusicPlayer_InitAppData

说明：
       初始化CMusicPlayer Applet结构数据缺省值。

参数：
       pMe [in]：指向MusicPlayer Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       EFAILED：初始化失败。
       SUCCESS：初始化成功。

备注：:

==============================================================================*/
static int CMusicPlayer_InitAppData(CMusicPlayer *pMe)
{
   AEEDeviceInfo  di;
   int i;
  #if defined(AEE_STATIC)
    ASSERT(pMe != NULL);
  #endif
    ISHELL_GetDeviceInfo(pMe->m_pShell, &di);
    pMe->m_rc.x       = 0;
    pMe->m_rc.y       = 0;
    pMe->m_rc.dx      = (int16) di.cxScreen;
    pMe->m_rc.dy      = (int16) di.cyScreen;
    pMe->m_ePreState = STATE_NONE;
    pMe->m_eCurState = STATE_INIT;
    pMe->m_pActiveDlg = NULL;
    //pMe->m_pActiveDlgID = IDD_WINDOWS;
    pMe->m_eStartMethod=STARTMETHOD_NORMAL;
    pMe->m_eDlgRet = DLGRET_CREATE;
    pMe->m_ePreMp3Status = MP3STATUS_NONE;
    pMe->m_bNotOverwriteDlgRet = FALSE;
    pMe->m_pImage = NULL;
    pMe->m_pMedia = NULL;
    pMe->m_bSuspending = FALSE;
    pMe->m_nCurPlaylistID=0;
    pMe->m_nPlaylistNum=0;
    pMe->m_nCurMusiclistID=0;
    pMe->m_nPlaylistMusicNum=0;
    pMe->m_nPlayinglistMusicNum=0;
    pMe->m_nCtlID = IDC_MUSICPLAY_MODE;
    pMe->m_bPlaying = FALSE;
    pMe->m_bPaused = FALSE;
    pMe->m_bUserStopped = FALSE;
    pMe->m_bUserPressNext = FALSE;
    pMe->m_bAppReady = FALSE;
    pMe->m_bRenameOpt = FALSE;
	pMe->m_headsetSwitch = FALSE;
    pMe->m_nAutoScrollIdx = 0;
    pMe->m_nCurrentTime = 0;
    pMe->m_nTotalTime = 0;
    pMe->m_nSimPlayCurTime = 0;
    pMe->m_keyBeepVol = 0;
    pMe->m_bInterrupt = FALSE;
    pMe->m_pMp3FileToPlay = NULL;
    pMe->m_bStartApp = FALSE;
    pMe->m_bSimPlayFailed = FALSE;
    g_nInterruptRef = 0;
    pMe->m_nRandNum = 0;
#ifdef FEATURE_VERSION_C337		
	pMe->keystart_time = 0;	
    pMe->keyend_time = 0;
#endif
#ifdef FEATURE_SUPPORT_BT_APP
#ifndef WIN32
    pMe->m_nBTID = BCMAPP_AG_BAD_APPID;
#endif
    pMe->m_bUse848 = TRUE;
    pMe->m_bUseBT = FALSE;
    pMe->m_isshift = FALSE;	
    SetIsPlayMP3ByBt(FALSE);
    pMe->m_bInterruptByBT = FALSE;
#endif //FEATURE_SUPPORT_BT_APP

#ifdef FEATURE_SUPPORT_BT_AUDIO
	pMe->m_bBtHSConnected = FALSE;	  //Add By zzg 2011_10_27
#endif

  if(SUCCESS != ISHELL_GetPrefs(pMe->m_pShell,
                               AEECLSID_APP_MUSICPLAYER,
                               MUSICPLAYERCFG_VERSION,
                               &pMe->m_MusicPlayerCfg,
                               sizeof(CMusicPlayerCfg)))
  {
    pMe->m_MusicPlayerCfg.eMusicPlayMode =PLAYMODE_ORDER;
#ifdef FEATURE_VERSION_C11
    pMe->m_MusicPlayerCfg.eMusicVolume=VOLUME_FIVE;
#elif defined (FEATURE_VERSION_C316)||defined(FEATURE_VERSION_W021_CT100)
    pMe->m_MusicPlayerCfg.eMusicVolume=VOLUME_FIVE;
#else
    pMe->m_MusicPlayerCfg.eMusicVolume=VOLUME_THREE;
#endif
    pMe->m_MusicPlayerCfg.eMusicSortBy=SORT_NONE;
#ifdef FEATURE_MP3PLAYER_SWITCHSKIN
    pMe->m_MusicPlayerCfg.eMp3Skin = PLAYERSKIN1;
#endif
    pMe->m_MusicPlayerCfg.lastPlayMusicID=0;
    pMe->m_MusicPlayerCfg.lastPlayPlaylist[0]='\0';
  }
#ifdef FEATURE_MP3PLAYER_SWITCHSKIN
  pMe->m_pMP3FaceRect = GetPlayerSkinRect(pMe->m_MusicPlayerCfg.eMp3Skin);
#else
  pMe->m_pMP3FaceRect = MP3SKIN1_RECT;
#endif
  pMe->m_nPlayMode = pMe->m_MusicPlayerCfg.eMusicPlayMode;
  pMe->m_nVolume = pMe->m_MusicPlayerCfg.eMusicVolume;
  pMe->m_nMusicSort = pMe->m_MusicPlayerCfg.eMusicSortBy;
  pMe->m_nCurrentVolume=pMe->m_MusicPlayerCfg.eMusicVolume*AEE_MAX_VOLUME/5;
  if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,AEECLSID_ANNUNCIATOR,(void **)&pMe->m_pIAnn))
    {
        return EFAILED;
    }
  IANNUNCIATOR_SetHasTitleText(pMe->m_pIAnn, FALSE);
   if ( SUCCESS != ISHELL_CreateInstance( pMe->m_pShell,
                                          AEECLSID_FILEMGR,
                                          (void **)&pMe->m_pFileMgr))
    {
        return EFAILED;
    }
   
   if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                               AEECLSID_CONFIG,
                                               (void **)&pMe->m_pConfig))
   {
        return EFAILED;
   }
     // Create IDisplay instance
    if ( SUCCESS != ISHELL_CreateInstance( pMe->m_pShell,
                                           AEECLSID_DISPLAY,
                                           (void **)&pMe->m_pDisplay))
    {
        return EFAILED;
    }
    if( ISHELL_CreateInstance( pMe->m_pShell, AEECLSID_BACKLIGHT, (void **)&pMe->m_pBackLight)!=AEE_SUCCESS)
    {
        return EFAILED;
    }
   for(i=0;i<MUSIC_MAX_FILESNUM;i++)
   {
     MEMSET(pMe->m_Musiclist[i].pMusicName,0,MP3NAME_MAX_LEN*sizeof(char));
     MEMSET(pMe->m_PlayingMusiclist[i].pMusicName,0,MP3NAME_MAX_LEN*sizeof(char));
   }
   MP3_Build_DefaultPlaylist(pMe);
   if(pMe->m_MusicPlayerCfg.lastPlayPlaylist[0]!='\0')
   {
     (void) CMusicPlayer_ReadMusiclist(pMe,pMe->m_MusicPlayerCfg.lastPlayPlaylist,TRUE);
   }
   else
   {
    for(i=0;i<MUSIC_MAX_FILESNUM;i++)
    {
     MEMSET(pMe->m_PlayingMusiclist[i].pMusicName,0,MP3NAME_MAX_LEN*sizeof(char));
    }
   }
   MP3_ResetRandonIdentifier(pMe);
   pMe->m_pMp3FileToPlay = (AECHAR *)sys_malloc(MP3NAME_MAX_LEN * sizeof(AECHAR));
   return SUCCESS;
  
}

/*==============================================================================
函数：CMusicPlayer_FreeAppData

说明：
       释放MusicPlayer Applet相关资源。

参数：
       pMe [in]：指向MusicPlayer Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       无。

备注：:

==============================================================================*/
static void CMusicPlayer_FreeAppData(CMusicPlayer *pMe)
{ 
 #if defined(AEE_STATIC)
    ASSERT(pMe != NULL);
  #endif

    (void)ISHELL_CancelTimer(pMe->m_pShell, NULL, pMe);
  	if(pMe->m_pIAnn)
    {
        IANNUNCIATOR_SetHasTitleText(pMe->m_pIAnn, TRUE);
        IANNUNCIATOR_Release(pMe->m_pIAnn);
        pMe->m_pIAnn = NULL;
    }
	
    if (pMe->m_pDisplay != NULL)
    {
        (void) IDISPLAY_Release(pMe->m_pDisplay);
        pMe->m_pDisplay = NULL;
    }
    
    if (pMe->m_pFileMgr != NULL)
    {
        (void) IFILEMGR_Release(pMe->m_pFileMgr);
        pMe->m_pFileMgr = NULL;
    } 
    if (pMe->m_pConfig)
    {
        ICONFIG_Release(pMe->m_pConfig);
        pMe->m_pConfig = NULL;
    }
     if(pMe->m_pBackLight)
    {
        IBACKLIGHT_Release(pMe->m_pBackLight);
        pMe->m_pBackLight = NULL;
    }
    ISHELL_RegisterNotify( pMe->m_pShell,
                           AEECLSID_APP_MUSICPLAYER,
                           AEECLSID_CM_NOTIFIER,
                           0
                          );
    FREEIF(pMe->m_pMsgBox_Msg);
    FREEIF(pMe->m_pInputListName);
    FREEIF(pMe->m_pBuffer);
    SYSFREEIF(pMe->m_pMp3FileToPlay);
    FREEIF(pMe->m_pSimplePlayMusicName);
   if(pMe->m_pImage)
   {
      IIMAGE_Release(pMe->m_pImage);
      pMe->m_pImage = NULL;
   }
    if(pMe->m_paniImage)
    {
        IIMAGE_Release(pMe->m_paniImage);
        pMe->m_paniImage = NULL;
    }
    if(pMe->m_pMode)
    {
        IMENUCTL_Release(pMe->m_pMode);
        pMe->m_pMode = NULL;
    }
    if(pMe->m_pVolume)
    {
        IMENUCTL_Release(pMe->m_pVolume);
        pMe->m_pVolume = NULL;
    }
    if(pMe->m_pSort)
    {
        IMENUCTL_Release(pMe->m_pSort);
        pMe->m_pSort = NULL;
    }
#ifdef FEATURE_SUPPORT_BT_APP
#ifndef WIN32
    if(pMe->m_bUseBT)
    {
        bcmapp_ag_audio_close(pMe->m_nBTID);
        pMe->m_nBTID = BCMAPP_AG_BAD_APPID;
    }
#endif
    //g_bPlayMP3ByBt = FALSE;
    SetIsPlayMP3ByBt(FALSE);
    pMe->m_bUseBT = FALSE;
#endif
    CMusicPlayer_ReleaseMedia(pMe);
} // CMusicPlayer_FreeAppData

/*==============================================================================
函数：CMusicPlayer_RunFSM

说明：
       MusicPlayer Applet有限状态机引擎。

参数：
       pMe [in]：指向MusicPlayer Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       无。

备注：:

==============================================================================*/
static void CMusicPlayer_RunFSM(CMusicPlayer *pMe)
{ 
    NextFSMAction nextFSMAction = NFSMACTION_WAIT;
   
#if defined(AEE_STATIC)
    ASSERT(pMe != NULL);
#endif
    for ( ; ; )
    {
        nextFSMAction = CMusicPlayer_ProcessState(pMe);

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
} // CMP_RunFSM
extern AEECLSID            AEE_Active(void);
/*==============================================================================
函数：IMusicPlayer_HandleEvent

说明：
       MusicPlayer Applet事件处理主函数。所有属于本Applet的事件，都将经由该函数处
       理。

参数：
       pi [in]：指向IMusicPlayer接口t对象的指针。
       eCode：BREW事件代码。
       wParam：事件参数
       dwParam [in]：与wParam关联的数据。。

返回值：
        TRUE：传入事件被处理。
        FALSE：传入事件没被处理。

备注：:
        该函数供BREW调用，用户不得调用。
        
==============================================================================*/
static boolean IMusicPlayer_HandleEvent( IMusicPlayer *pi,
                                         AEEEvent    eCode,
                                         uint16      wParam,
                                         uint32  dwParam)
{
    CMusicPlayer *pMe = (CMusicPlayer*)pi;
    AEEAppStart *as;
    MSG_FATAL("IMusicPlayer_HandleEvent,eCode = %x",eCode,0,0);
	
	MSG_FATAL("***zzg IMusicPlayer_HandleEvent,eCode = %x,dwParam=%d***",eCode,dwParam,0);
	
    switch (eCode)
    {
        case EVT_APP_START_BACKGROUND:
        case EVT_APP_START:
        #if defined(AEE_STATIC)
            ASSERT(dwParam != 0);
        #endif
            as = (AEEAppStart*)dwParam;
            pMe->m_rc = as->rc;
            pMe->m_bSuspending = FALSE;
            pMe->m_bAppReady = FALSE;
            pMe->m_bActive = TRUE;
            MP3_CloseKeyBeepVol(pMe);  //close keybeepvol
            Music_Shake_Open();
            MP3_DecodeStartArgs(pMe,(char *)as->pszArgs);
            MP3_SetStartCurState(pMe);
           if(GetMp3PlayerStatus() == MP3STATUS_NONE && APP_MEDIA_ALLOW == app_media_scheduler()
                    && pMe->m_eStartMethod != STARTMETHOD_SIMPLEPLAYER)
            {
                pMe->m_bStartApp = TRUE;
                (void) ISHELL_SetTimer(pMe->m_pShell,200,(PFNNOTIFY)MP3_InitMusicCB,pMe);
            }
            MP3_SetStartStatus(pMe);
            if(ISHELL_RegisterNotify( pMe->m_pShell, 
                              AEECLSID_APP_MUSICPLAYER,
                              AEECLSID_CM_NOTIFIER,
                              NMASK_CM_VOICE_CALL | NMASK_CM_OTHER_CALL | NMASK_CM_TEST_CALL) != SUCCESS)
            {
               CMusicPlayer_FreeAppData(pMe);
               return FALSE;
            }
            
            // 开始MusicPlayer状态机
            CMusicPlayer_RunFSM(pMe);
            return TRUE;

        case EVT_APP_STOP:
			MSG_FATAL("***zzg MusicPlayer EVT_APP_STOP***", 0, 0, 0);
#ifdef FEATURE_SOUND_BO
           {
               nv_item_type    SimChoice;
               SimChoice.sim_select =2;
               (void)OEMNV_Put(NV_SIM_SELECT_I,&SimChoice);
           }
#endif
           if(GetMp3PlayerStatus()!=MP3STATUS_RUNONBACKGROUND)
           {
                pMe->m_bActive = FALSE;
                SetMp3PlayerStatus(pMe,MP3STATUS_NONE);
                if(pMe->m_eStartMethod != STARTMETHOD_PEDOMETERCALLPLAYER)
                {
                 (void) ISHELL_SetPrefs(pMe->m_pShell,
                                       AEECLSID_APP_MUSICPLAYER,
                                       MUSICPLAYERCFG_VERSION,
                                       &pMe->m_MusicPlayerCfg,
                                       sizeof(CMusicPlayerCfg));
                }
               MP3_ResumeKeyBeepVol(pMe);  //resume keybeepvol
               Music_Shake_Close();
           }
           else
           {
              ISHELL_CancelTimer(pMe->m_pShell,(PFNNOTIFY)MP3_MusicNameAutoScroll,pMe);
			  
             (void)ISHELL_PostEvent( pMe->m_pShell,
                                        AEECLSID_CORE_APP,
                                        EVT_DRAWMUSICNAME,
                                        0,
                                        (uint32)pMe->m_pMp3FileToPlay);
			
			 
             *((boolean*)dwParam) = FALSE;
           }
            pMe->m_bSuspending = TRUE;
            //*((boolean*)dwParam) = !g_bMp3PlayBackground;
            return TRUE;

        case EVT_APP_SUSPEND:
            pMe->m_bSuspending = TRUE;		

	 
#ifdef FEATURE_SUPPORT_BT_AUDIO
			MSG_FATAL("***zzg MP3 m_bBtHSConnected=%x***", pMe->m_bBtHSConnected, 0, 0);

			//Modify By zzg 2011_10_27
			if (FALSE == pMe->m_bBtHSConnected)
			{
				MP3_InterruptHandle(pMe);	
			}
			else
			{
				pMe->m_bBtHSConnected = FALSE;	//Add By zzg 2011_10_27
			}
			//Modify end	
#else			
			MP3_InterruptHandle(pMe);	
#endif		
          
            MSG_FATAL("pMe->m_nCurrentTime = %d",pMe->m_nCurrentTime,0,0);
            return TRUE;

        case EVT_APP_RESUME:
        #if defined(AEE_STATIC)
            ASSERT(dwParam != 0);
        #endif
            as = (AEEAppStart*)dwParam;
            pMe->m_bSuspending = FALSE;
            pMe->m_rc = as->rc;
            if(pMe->m_eCurState == STATE_PLAYLIST_OPTS || pMe->m_eCurState == STATE_VIEW_OPTS
                || pMe->m_eCurState == STATE_MAINOPTSMENU )
            {
                pMe->m_eCurState = pMe->m_ePreState;
            }

#ifdef FEATURE_SUPPORT_BT_AUDIO            
            //Modify By zzg 2011_10_27
			if (FALSE == pMe->m_bBtHSConnected)
			{
				MP3_ResumeHandle(pMe);	
			}
			//Modify end
#else			
			MP3_ResumeHandle(pMe);
#endif			

            MSG_FATAL("pMe->m_nCurrentTime = %d,pMe->m_eCurState = %d",pMe->m_nCurrentTime,pMe->m_eCurState,0);
            CMusicPlayer_RunFSM(pMe);
            return TRUE;
        case EVT_DIALOG_START:
			if(OEM_IME_DIALOG == wParam)
			{
				return TRUE;
			}
			
            (void) CMusicPlayer_RouteDialogEvent(pMe,eCode,wParam,dwParam);

            return TRUE;  
        case EVT_DIALOG_INIT:
            if( wParam == OEM_IME_DIALOG)
			{
				return TRUE;
			}
            // 更新对话框控制参数信息！！！
            pMe->m_pActiveDlg = (IDialog*)dwParam;
            pMe->m_pActiveDlgID = wParam;
            pMe->m_bAppReady = FALSE;

            //Add By zzg 2010_08_09
            if (pMe->m_pIAnn != NULL)
            {
                IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn, FALSE);
                IANNUNCIATOR_SetHasTitleText(pMe->m_pIAnn, TRUE);
                IANNUNCIATOR_SetFieldTextEx(pMe->m_pIAnn, NULL,FALSE);
        	}
            //Add End
            
            return CMusicPlayer_RouteDialogEvent(pMe,eCode,wParam,dwParam);

        case EVT_DIALOG_END:
				
            if (wParam == 0)
            {
                return TRUE;
            }
            if(OEM_IME_DIALOG == wParam)
			{
				return ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_MUSICPLAYER,EVT_USER_REDRAW,0,0);
			}
            (void) CMusicPlayer_RouteDialogEvent(pMe,eCode,wParam,dwParam);
            pMe->m_pActiveDlg = NULL;
            // Applet被挂起，不跑状态机，等待Applet返回。（注意：EVT_APP_SUSPEND
            // 事件在EVT_DIALOG_END事件前发出。
            if (pMe->m_bSuspending == FALSE)
            {
                // 开始MusicPlayer状态机
                CMusicPlayer_RunFSM(pMe);
            }
            return TRUE;
        case EVT_USER_REDRAW:
           (void) CMusicPlayer_RouteDialogEvent(pMe,eCode,wParam,dwParam);
            (void)ISHELL_SetTimer( pMe->m_pShell,
                                   MP3APPISREADY_TIME,
                                   CMusicPlayer_AppIsReadyCB,
                                   pMe);
            return TRUE;
        case EVT_ALARM:
            { 
                if(wParam)    
                {  
                   MSG_FATAL("EVT_ALARM wParam == TRUE",0,0,0);
                   MP3_InterruptHandle(pMe);

				   //Add End
				   (void)ISHELL_PostEvent( pMe->m_pShell,
	                                        AEECLSID_BLUETOOTH_APP,
	                                        EVT_USER,
	                                        EVT_MUSICPLAYER_ALARM_SUSPEND_BT,
	                                        0);
				   //Add End
				   
                }
                else
                {    
                   MSG_FATAL("EVT_ALARM wParam == FALSE",0,0,0);
                   MP3_ResumeHandle(pMe);

					//Add By zzg 2011_11_07
				   (void)ISHELL_PostEvent( pMe->m_pShell,
	                                        AEECLSID_BLUETOOTH_APP,
	                                        EVT_USER,
	                                        EVT_MUSICPLAYER_ALARM_RESUME_BT,
	                                        0);
					//Add End
                }
     
            }
            return TRUE;

        case EVT_NOTIFY:
            {
                if(GetMp3PlayerStatus()== MP3STATUS_RUNONBACKGROUND)
                {
                    AEENotify* notify = (AEENotify*)dwParam;
                    notify->st = NSTAT_IGNORED;
                    if( notify->cls == AEECLSID_CM_NOTIFIER)
                    {
                        if( ( notify->dwMask & NMASK_CM_VOICE_CALL)||( notify->dwMask & NMASK_CM_TEST_CALL))
                        {
                            AEECMNotifyInfo* eventInfo = (AEECMNotifyInfo *)(notify->pData);
                           if( eventInfo->event == AEECM_EVENT_CALL_ORIG ||
                                eventInfo->event == AEECM_EVENT_CALL_INCOM )
                            {
                               MP3_InterruptHandle(pMe);
                            }
                           else if(eventInfo->event == AEECM_EVENT_CALL_END)
                           {
                            // 由于接受AEECM_EVENT_CALL_END事件就马上恢复时机过早现在改为通过传递EVT_ALARM来恢复故暂时注释掉该语句
                             //MP3_ResumeHandle(pMe);
                           }
                            notify->st = NSTAT_PROCESSED;
                        }
                    }
                }
                return TRUE;
            }
        case EVT_GSENSOR_SHAKE:
            switch(wParam)
            {
                case AVK_GSENSOR_FORWARD:
                    CMusicPlayer_PlayNext(pMe,FALSE );//播放上一首
                    return TRUE;

                case AVK_GSENSOR_BACKWARD:
                    pMe->m_bUserPressNext = TRUE;
                    CMusicPlayer_PlayNext(pMe,TRUE);//播放下一首
                    pMe->m_bUserPressNext = FALSE;
                    return TRUE;
            }
            break;
		case EVT_HEADSET:
			//recorder_set_media_device_auto( &pMe->m_Media);
			if(pMe->m_pMedia != NULL)
			{
			(void)IMEDIA_SetVolume(pMe->m_pMedia,pMe->m_nCurrentVolume); 
			return TRUE;
			}
			break;
        case EVT_KEY_PRESS:
        case EVT_KEY:
        case EVT_KEY_RELEASE:
        case EVT_COMMAND:
			#if !defined(FEATURE_VERSION_K212)
            if(!pMe->m_bAppReady)
            {
                return TRUE;
            }
			#endif
            return CMusicPlayer_RouteDialogEvent(pMe,eCode,wParam,dwParam);
        case EVT_CLOSEAPP:
            SetMp3PlayerStatus(pMe,MP3STATUS_NONE);
            if(AEECLSID_CORE_APP == AEE_Active())
            {
                SYSFREEIF(pMe->m_pMp3FileToPlay);
                (void)ISHELL_PostEvent( pMe->m_pShell,
                                        AEECLSID_CORE_APP,
                                        EVT_DRAWMUSICNAME,
                                        0,
                                        (uint32)pMe->m_pMp3FileToPlay);
            }
            ISHELL_CloseApplet(pMe->m_pShell, FALSE);
            return TRUE;

        case EVT_DRAWMUSICNAME:
            (void)ISHELL_PostEvent( pMe->m_pShell,
                                    AEECLSID_CORE_APP,
                                    EVT_DRAWMUSICNAME,
                                    0,
                                    (uint32)pMe->m_pMp3FileToPlay);
            return TRUE;
            
        case EVT_OPENSHAKE:
            Music_Shake_Open();
            return TRUE;
        case EVT_POPMSGDIALOG:
            pMe->m_nMsgResID = wParam;//IDS_PLAYFAILED;
            if(pMe->m_eCurState == STATE_PLAYLIST_OPTS || pMe->m_eCurState == STATE_VIEW_OPTS
                || pMe->m_eCurState == STATE_MAINOPTSMENU )
            {
                pMe->m_eCurState = pMe->m_ePreState;
            }
            pMe->m_eMsgRetState = pMe->m_eCurState;
            pMe->m_eMsgType = MESSAGE_WARNNING;
            //pMe->m_eMsgRetState = STATE_PLAYMUSIC_WINDOWS;
            MOVE_TO_STATE(STATE_MSG);
            CLOSE_DIALOG(DLGRET_CREATE);
            return TRUE;

#ifdef FEATURE_SUPPORT_BT_APP
        case AVK_BT_HEADSET_CONNECT:
             if(pMe->m_bInterruptByBT)
             {
                return TRUE;
             }
#ifndef WIN32
             pMe->m_nBTID = bcmapp_is_ag_connected();
             if(BCMAPP_AG_BAD_APPID != pMe->m_nBTID)
             {
                (void)bcmapp_ag_audio_open(pMe->m_nBTID, 
                                            (PFN_AG_NOTIFY)MP3_PlayMusicByBTCallBack,
                                            pMe, 
                                            pMe->m_MusicPlayerCfg.eMusicVolume,
                                            pMe->m_bUse848); 
                pMe->m_bUseBT = TRUE;
                //g_bPlayMP3ByBt = TRUE;
                SetIsPlayMP3ByBt(TRUE);
             }
#endif
             return TRUE;
             
        case AVK_BT_HEADSET_DISCONNECT:
            if(pMe->m_bInterruptByBT)
            {
                return TRUE;
            }
            if(pMe->m_bUseBT)
            {
#ifndef WIN32
                bcmapp_ag_audio_close(pMe->m_nBTID);
                pMe->m_nBTID = BCMAPP_AG_BAD_APPID;
#endif
                pMe->m_bUseBT = FALSE;
                SetIsPlayMP3ByBt(FALSE);
            }
            return TRUE;
            
       case EVT_APP_EXIT:
           if(!pMe->m_bPlaying && !pMe->m_bPaused)
           {
             SetMp3PlayerStatus(pMe,MP3STATUS_NONE);
             ISHELL_CloseApplet(pMe->m_pShell, TRUE);
           }
           return TRUE;
#endif //FEATURE_SUPPORT_BT_APP

//Add By zzg 2011_10_29
#ifdef FEATURE_SUPPORT_BT_AUDIO
        case AVK_BT_HEADSET_CONNECT:   
			MSG_FATAL("***zzg IMusicPlayer_HandlEvt AVK_BT_HEADSET_CONNECT***", 0, 0, 0);
             return TRUE;
             
        case AVK_BT_HEADSET_DISCONNECT: 
			MSG_FATAL("***zzg IMusicPlayer_HandlEvt AVK_BT_HEADSET_DISCONNECT***", 0, 0, 0);
            return TRUE;
            
       case EVT_APP_EXIT:         
	   		MSG_FATAL("***zzg IMusicPlayer_HandlEvt EVT_APP_EXIT***", 0, 0, 0);
           return TRUE;
#endif 
//Add End


#ifdef FEATURE_LCD_TOUCH_ENABLE//WLH ADD FOR LCD TOUCH
		case EVT_USER:
			{
				if (wParam == AVK_SELECT)
				{
					if (dwParam == 0)
					{
						eCode = EVT_KEY;
					}
					else if(dwParam == KEY_PRESS_DWPARM)
					{
						eCode = EVT_KEY_PRESS;
						dwParam = 0;
					}
					else
					{
						eCode = EVT_COMMAND;
						wParam = dwParam;
					}
				}
				else if (wParam == AVK_INFO)
				{
					eCode = EVT_KEY;
				}
				else if (wParam == AVK_CLR)
				{
					eCode = EVT_KEY;
				}
				else if ((wParam == AVK_LEFT)||(wParam == AVK_RIGHT)||(wParam == AVK_UP)||(wParam == AVK_DOWN))
				{
					eCode = EVT_KEY;
				}
                
			}
			return CMusicPlayer_RouteDialogEvent(pMe,eCode,wParam,dwParam);
#else          
#ifdef FEATURE_VERSION_EC99
        case EVT_USER:
            {
                if(GetMp3PlayerStatus() == MP3STATUS_RUNONBACKGROUND)
                {
                    if (wParam == AVK_FFWD)     //pre
                    {
                        CMusicPlayer_PlayNext(pMe,FALSE );//播放上一首         
                    }

                    else if (wParam == AVK_RWD)      //next
                    {
                       CMusicPlayer_PlayNext(pMe,TRUE);//播放下一首
                    }

                    return TRUE;
                }
                else
                {
                    return CMusicPlayer_RouteDialogEvent(pMe,eCode,wParam,dwParam);
                }
                
            }
#endif        
#endif//FEATURE_LCD_TOUCH_ENABLE

        default:
            // 将接收到的事件路由至当前活动的对话框事件处理函数。
            return CMusicPlayer_RouteDialogEvent(pMe,eCode,wParam,dwParam);
    }
} // CMusicPlayer_HandleEvent
/*==============================================================================
函数： IMusicPlayer_SimplePlayer

说明：
      用来读取指定播放列表里面的所有音乐名称

参数：
      pi:IMusicPlayer *类型指针
      musicName:包含路径和后缀的文件名,由调用者传入
      
返回值：
    返回值同ISHELL_StartAppletArgs的返回值

备注：:
        无
        
==============================================================================*/

static int IMusicPlayer_SimplePlayer(IMusicPlayer *pi,char *musicName)
{
    CMusicPlayer *pMe=(CMusicPlayer *)pi;
    char     *args = NULL;
    int       nRet;
    
    if ( pMe == NULL ) 
    {
        return EFAILED;
    }
    args=(char *)MALLOC((1+MP3NAME_MAX_LEN)*sizeof(char));
    args[0]=STARTARGS_SIMPLEPLAYER;
    STRCPY(&args[1],musicName);
    nRet = ISHELL_StartAppletArgs(  pMe->m_pShell,
                                    AEECLSID_APP_MUSICPLAYER, 
                                    args);
    if (args) 
    {
        FREE(args);
    }
    
    return nRet;
}
/*==============================================================================
函数： IMusicPlayer_PedometerCallPlayer

说明：
      用来读取指定播放列表里面的所有音乐名称

参数：
      pi:IMusicPlayer *类型指针
      musicName:包含路径和后缀的文件名,由调用者传入
      
返回值：
    返回值同ISHELL_StartAppletArgs的返回值

备注：:
        无
        
==============================================================================*/
static int IMusicPlayer_PedometerCallPlayer(IMusicPlayer *pi)
{
    CMusicPlayer *pMe=(CMusicPlayer *)pi;
    char     *args = NULL;
    int       nRet;
    
    if ( pMe == NULL ) 
    {
        return EFAILED;
    }
    args=(char *)MALLOC(2*sizeof(char));
    args[0]=STARTARGS_PEDOMETERCALLPLAYER;
    args[1]='\0';
    nRet = ISHELL_StartBackgroundApplet( pMe->m_pShell,
                                        AEECLSID_APP_MUSICPLAYER, 
                                        args);
    if (args) 
    {
        FREE(args);
    }
    
    return nRet;
}

/*==============================================================================
函数： CMusicPlayer_ReadMusiclist

说明：
      用来读取指定播放列表里面的所有音乐名称

参数：
      pMe:CMusicPlayer *类型指针
      curfileName:不包含路径和后缀的文件名
      isPlaying:TRUE:把从文件中读取的数据放到pMe->m_PlayingMusiclist中
                FALSE:把从文件中读取的数据存放到pMe->m_Musiclist中

返回值：
    SUCCESS:读取成功
    EFAILED:读取失败

备注：:
        无
        
==============================================================================*/
int CMusicPlayer_ReadMusiclist(CMusicPlayer *pMe , char *curFileName,boolean isPlaying)
{
    IFile  *pFile;
    char fileName[MP3_MAX_FILE_NAME];
    if(pMe==NULL||curFileName==NULL)
    {
     return EFAILED;
    }
    (void)STRCPY(fileName,MUSICPLAYLIST_DIR);
    (void)STRCAT(fileName,"/");
    (void)STRCAT(fileName,curFileName);
    (void)STRCAT(fileName,".txt");
    pFile = IFILEMGR_OpenFile(pMe->m_pFileMgr, fileName, _OFM_READ);
    if(pFile == NULL)
    {
        ERR("FILE IS NULL",0,0,0);
        // 尚未创建，使用默认值
        return EFAILED;
    } //else 打开文件继续执行

    // 从文件开始读取数据
    if(SUCCESS != IFILE_Seek(pFile, _SEEK_START, 0))
    {
        (void)IFILE_Release(pFile);
        pFile = NULL;
        return EFAILED;
    }
    if(TRUE == isPlaying)
    {
      if(0==IFILE_Read(pFile, pMe->m_PlayingMusiclist, sizeof(pMe->m_PlayingMusiclist)))
      {
        pMe->m_nPlayinglistMusicNum = 0;
        (void)IFILE_Release(pFile);
        pFile = NULL;
        return EFAILED;
      }
      else
      {
        CMusicPlayer_PlayingMusiclistSortBy(pMe);
      }
    }
    else
    {
      if(IFILE_Read(pFile, pMe->m_Musiclist, sizeof(pMe->m_Musiclist))==0)
      {
        pMe->m_nPlaylistMusicNum = 0;
        (void)IFILE_Release(pFile);
        pFile = NULL;
        return EFAILED;
      }
      else
      {
        CMusicPlayer_MusiclistSortBy(pMe);
      }
    }
    (void)IFILE_Release(pFile);
    pFile = NULL;
    return SUCCESS;
}
/*==============================================================================
函数： CMusicPlayer_WriteMusiclist

说明：
       用来写入指定播放列表里面的所有音乐名称

参数：
      pMe:CMusicPlayer *类型指针
      curfileName:不包含路径和后缀的文件名

返回值：
    SUCCESS:读取成功
    EFAILED:读取失败

备注：:
        无
        
==============================================================================*/
int CMusicPlayer_WriteMusiclist(CMusicPlayer *pMe , char *curFileName)
{
    IFile  *pFile;
    char fileName[MP3_MAX_FILE_NAME];
    if(pMe==NULL||fileName==NULL)
    {
    return EFAILED;
    }
    (void)STRCPY(fileName,MUSICPLAYLIST_DIR);
    (void)STRCAT(fileName,"/");
    (void)STRCAT(fileName,curFileName);
    (void)STRCAT(fileName,".txt");

    pFile = IFILEMGR_OpenFile(pMe->m_pFileMgr, fileName, _OFM_READWRITE);
    if(pFile == NULL)
    {
        ERR("FILE IS NULL",0,0,0);
        // 尚未创建，使用默认值
        return EFAILED;
    } //else 打开文件继续执行

    // 从文件开头写入数据
    if(SUCCESS != IFILE_Seek(pFile, _SEEK_START, 0))
    {
        (void)IFILE_Release(pFile);
        pFile = NULL;
        return EFAILED;
    }
    (void) IFILE_Write(pFile, pMe->m_Musiclist, sizeof(pMe->m_Musiclist));
    (void)IFILE_Release(pFile);
    pFile = NULL;
    return SUCCESS;
}
/*==============================================================================
函数:
       CMusicPlayer_APPIsReadyTimer

说明:
       定时器回调函数。主要用于控制快速按键问题的处理。

参数:
       pme [in]：void *类型指针，暗指CMusicPlayer结构指针。

返回值:
       无。

备注:

==============================================================================*/
static void CMusicPlayer_AppIsReadyCB(void * pUser)
{
    CMusicPlayer *pMe = (CMusicPlayer *)pUser;

    if (NULL == pMe)
    {
        return;
    }
    
    pMe->m_bAppReady = TRUE;
}

/*中断处理函数*/
static void MP3_InterruptHandle(CMusicPlayer *pMe)
{
    if(!pMe)
    {
        return;
    }
    g_nInterruptRef ++;
    if(g_nInterruptRef > 1)
    {
        return;
    }
#ifdef FEATURE_SUPPORT_BT_APP
    if(pMe->m_bUseBT)
    {
#ifndef WIN32
        bcmapp_ag_audio_close(pMe->m_nBTID);
        pMe->m_nBTID = BCMAPP_AG_BAD_APPID;
#endif
        pMe->m_bInterruptByBT = TRUE;
        SetIsPlayMP3ByBt(FALSE);
    }
#endif
    if(pMe->m_eStartMethod == STARTMETHOD_SIMPLEPLAYER)
    {
        //ISHELL_CancelTimer(pMe->m_pShell,(PFNNOTIFY)MP3_MusicNameAutoScroll,pMe);
        (void)ISHELL_CancelTimer(pMe->m_pShell, NULL, pMe);
        CMusicPlayer_ReleaseMedia(pMe);
        pMe->m_bPaused = FALSE;
        pMe->m_bPlaying = FALSE;
    }
    else
    {
        //ISHELL_CancelTimer(pMe->m_pShell,(PFNNOTIFY)MP3_MusicNameAutoScroll,pMe);
        (void)ISHELL_CancelTimer(pMe->m_pShell, NULL, pMe);
        CMusicPlayer_ReleaseMedia(pMe);
    }
}

/*中断恢复函数*/
static void MP3_ResumeHandle(CMusicPlayer *pMe)
{
    if(!pMe)
    {
        return;
    }
    g_nInterruptRef --;
    if(g_nInterruptRef > 0)
    {
        return;
    }
    else
    {
        g_nInterruptRef = 0;
    }
    pMe->m_bInterrupt = TRUE;
    MP3_CloseKeyBeepVol(pMe);  //close keybeepvol again 防止是由于进入设置按键音引起的
    Music_Shake_Open(); // open again if be closed by other app
    if(pMe->m_eStartMethod == STARTMETHOD_SIMPLEPLAYER)
    {
      return;   
    }
#ifdef FEATURE_SUPPORT_BT_APP
    if(pMe->m_bUseBT)
    {
        pMe->m_bInterruptByBT = FALSE;
        SetIsPlayMP3ByBt(TRUE);
#ifndef WIN32
        pMe->m_nBTID = bcmapp_is_ag_connected();
        if(BCMAPP_AG_BAD_APPID == pMe->m_nBTID)
        {
           (void)ISHELL_PostEvent(pMe->m_pShell,
                                  AEECLSID_APP_MUSICPLAYER,
                                  EVT_POPMSGDIALOG,
                                  IDS_MSG_UNCONNECT_BT,
                                  0);
        }
        else
        {
           (void)bcmapp_ag_audio_open(pMe->m_nBTID, 
                                (PFN_AG_NOTIFY)MP3_PlayMusicByBTCallBack,
                                pMe, 
                                pMe->m_MusicPlayerCfg.eMusicVolume,
                                pMe->m_bUse848);
        }
#endif
    }
#endif
    CMusicPlayer_ReleaseMedia(pMe);
 
    ISHELL_SetTimer(pMe->m_pShell,300,(PFNNOTIFY)CMusicPlayer_InitMusic,pMe);

    if(pMe->m_bPlaying || pMe->m_bPaused)
    {
      ISHELL_SetTimer(pMe->m_pShell,800,(PFNNOTIFY) CMusicPlayer_PlayMusic,pMe);
      ISHELL_SetTimer(pMe->m_pShell,2000, (PFNNOTIFY)CMusicPlayer_SeekMusic,pMe);      
    }
    if(pMe->m_bPaused)
     {
        ISHELL_SetTimer(pMe->m_pShell,2100, (PFNNOTIFY)CMusicPlayer_PauseMusic,pMe);
     }   
}
/*关闭键盘音*/
static void MP3_CloseKeyBeepVol(CMusicPlayer *pMe)
{
    byte mute = OEMSOUND_MUTE_VOL;
    byte curBeepVol = 0;
    ICONFIG_GetItem( pMe->m_pConfig, CFGI_BEEP_VOL, &curBeepVol, sizeof(byte));

	
    if(curBeepVol !=0)
    {
        ICONFIG_GetItem( pMe->m_pConfig, CFGI_BEEP_VOL, &pMe->m_keyBeepVol, sizeof(byte));
        ICONFIG_SetItem( pMe->m_pConfig, CFGI_BEEP_VOL, &mute, sizeof(byte));
    }
}

/*恢复键盘音*/
static void MP3_ResumeKeyBeepVol(CMusicPlayer * pMe)
{
    ICONFIG_SetItem( pMe->m_pConfig, CFGI_BEEP_VOL, &pMe->m_keyBeepVol, sizeof(byte));
}

extern boolean is_alarm_time_reach();
#ifdef FEATURE_SPORTS_APP
extern boolean is_g_sportsapp_pedometer_bground_flag();
#endif

void MP3_InitMusicCB(CMusicPlayer *pMe)
{
    if(pMe == NULL)
    {
        return;
    }
    Music_Shake_Open(); // open again ,because FM able close it
    MP3_CloseKeyBeepVol(pMe); // close again ,because FM able resume KeyBeeVol
#ifdef FEATURE_SUPPORT_BT_APP
    if(pMe->m_bStartApp)
    {
        char *            pf = NULL;

#ifndef WIN32
        pMe->m_nBTID = bcmapp_is_ag_connected();
        if(BCMAPP_AG_BAD_APPID != pMe->m_nBTID)
        {
            pf = STRRCHR(pMe->m_PlayingMusiclist[pMe->m_MusicPlayerCfg.lastPlayMusicID].pMusicName, '.');
            pf++;
            if(0 == STRICMP(pf,MP3_TYPE))
            {
               pMe->m_bUse848 = TRUE;
            }
            else
            {
                pMe->m_bUse848 = FALSE;
            }
            (void)bcmapp_ag_audio_open(pMe->m_nBTID, 
                                        (PFN_AG_NOTIFY)MP3_PlayMusicByBTCallBack,
                                        pMe, 
                                        pMe->m_MusicPlayerCfg.eMusicVolume,
                                        pMe->m_bUse848); 
            pMe->m_bUseBT = TRUE;
            SetIsPlayMP3ByBt(TRUE);
        }
#endif
    }
#endif
   //判断是否是第一次使用播放器
   if(FALSE==CMusicPlayer_InitMusic(pMe))
   {
     if(pMe->m_bStartApp)
     {
        CMusicPlayer_ReleaseMedia(pMe);
        (void)STRCPY(pMe->m_MusicPlayerCfg.lastPlayPlaylist,DEFAULT_PLAYLIST);
        CMusicPlayer_ReadMusiclist(pMe,
                                   pMe->m_MusicPlayerCfg.lastPlayPlaylist,
                                   TRUE);
        pMe->m_MusicPlayerCfg.lastPlayMusicID=0;
  
        (void)CMusicPlayer_InitMusic(pMe);
     }
     else if((GetMp3PlayerStatus() == MP3STATUS_RUNONFOREGROUND && pMe->m_eCurState == STATE_PLAYMUSIC_WINDOWS)
           || (pMe->m_eStartMethod == STARTMETHOD_PEDOMETERCALLPLAYER && pMe->m_bStartApp)) 
     {
      // if(!pMe->m_bStartApp)
       //{
            pMe->m_bPlaying = FALSE;
            pMe->m_bPaused = FALSE;
            (void)ISHELL_PostEvent(pMe->m_pShell, AEECLSID_APP_MUSICPLAYER, EVT_POPMSGDIALOG, IDS_MSG_ERR_FORMAT, 0);
            //初试化默认音乐的操作
            CMusicPlayer_ReleaseMedia(pMe);
            pMe->m_MusicPlayerCfg.lastPlayPlaylist[0]='\0';
#ifdef FEATURE_VERSION_EC99
            (void)STRCPY(pMe->m_PlayingMusiclist[0].pMusicName,"1fs:/hsmm/music/Aishiniwo.mp3");
#else
            (void)STRCPY(pMe->m_PlayingMusiclist[0].pMusicName,"1fs:/hsmm/music/Away.mp3");
#endif
            pMe->m_MusicPlayerCfg.lastPlayMusicID=0;
            pMe->m_nPlayinglistMusicNum = 1;
        (void)CMusicPlayer_InitMusic(pMe);
        if(pMe->m_eStartMethod != STARTMETHOD_PEDOMETERCALLPLAYER)
        {
           ISHELL_CancelTimer(pMe->m_pShell,(PFNNOTIFY) CMusicPlayer_PlayMusic,pMe);
        }
    }
    else if(GetMp3PlayerStatus() == MP3STATUS_RUNONBACKGROUND && pMe->m_MusicPlayerCfg.eMusicPlayMode == PLAYMODE_ORDER
            && pMe->m_MusicPlayerCfg.lastPlayMusicID == pMe->m_nPlayinglistMusicNum - 1)
    {
        (void)ISHELL_PostEvent(pMe->m_pShell, AEECLSID_APP_MUSICPLAYER, EVT_CLOSEAPP, 0, 0);
    }
    else
    {
        (void)ISHELL_PostEvent(pMe->m_pShell, AEECLSID_APP_MUSICPLAYER, EVT_GSENSOR_SHAKE, AVK_GSENSOR_BACKWARD, 0);
    }
   }
   pMe->m_bStartApp = FALSE;
}
/*decode app startargs*/
static void MP3_DecodeStartArgs(CMusicPlayer *pMe, char *args)
{
    if(NULL == pMe || NULL == args)
    {
        return;
    }
    switch(args[0])
    {
        case STARTARGS_SIMPLEPLAYER:
            pMe->m_eStartMethod=STARTMETHOD_SIMPLEPLAYER;
            pMe->m_pSimplePlayMusicName=(char *)MALLOC(MP3NAME_MAX_LEN*sizeof(char));
            (void)STRCPY(pMe->m_pSimplePlayMusicName,&args[1]);
            break;

       case STARTARGS_PEDOMETERCALLPLAYER:
           pMe->m_eStartMethod = STARTMETHOD_PEDOMETERCALLPLAYER;
           break;

       default:
          break;
    }
}
/*set app start curState*/
static void MP3_SetStartCurState(CMusicPlayer *pMe)
{
    if(NULL == pMe)
    {
        return;
    }
    if(GetMp3PlayerStatus()==MP3STATUS_RUNONBACKGROUND)
    {   
        pMe->m_eStartMethod = STARTMETHOD_NORMAL;
        pMe->m_eCurState = STATE_PLAYMUSIC_WINDOWS;
    }
#ifndef WIN32
    else if(APP_MEDIA_IMPACT_BY_FM == app_media_scheduler())
    {
        pMe->m_eCurState = STATE_MSGFMBGPLAYPROMPT;
    }
#endif//WIN32
    else
    {
        pMe->m_eCurState = STATE_INIT;
    }
    
}
/*set app start status*/
static void MP3_SetStartStatus(CMusicPlayer *pMe)
{
    if(NULL == pMe)
    {
        return;
    }
    if(STARTMETHOD_PEDOMETERCALLPLAYER == pMe->m_eStartMethod)
    {
        SetMp3PlayerStatus(pMe,MP3STATUS_RUNONBACKGROUND);
    }
    else
    {
      SetMp3PlayerStatus(pMe,MP3STATUS_RUNONFOREGROUND);
    }
}

static void Music_Shake_Open(void)
{
#ifdef FEATRUE_SUPPORT_G_SENSOR
    dword shake;
    //g_bMp3PlayerRun = TRUE;
    if((MMI_GSENSOR_SHAKE_OPEN == mmi_g_sensor_state) 
        ||(MMI_GSENSOR_SHAKE_OPEN_IN_IDLE == mmi_g_sensor_state))   //return if sensor has been open.
    {
        return;
    }
    if(is_g_sportsapp_pedometer_bground_flag() == FALSE)
    {
        OEM_GetConfig(CFGI_GSENSOR,&shake,sizeof(shake));
        if(shake & OEMNV_SHAKE_MUSIC_MASK)
        {
#ifdef FEATRUE_SUPPORT_G_SENSOR
#ifndef WIN32//wlh 临时
            mmi_g_sensor_process(G_SENSOR_ENABLE | G_SENSOR_SHAKE);
#endif//WIN32
#endif
            mmi_g_sensor_state = MMI_GSENSOR_SHAKE_OPEN;
        }    
    }
#endif
}

static void Music_Shake_Close(void)
{
    dword shake;
#ifdef FEATRUE_SUPPORT_G_SENSOR
     //g_bMp3PlayerRun = FALSE;
    if((MMI_GSENSOR_SHAKE_CLOSE == mmi_g_sensor_state) 
        ||(MMI_GSENSOR_SHAKE_CLOSE_IN_IDLE == mmi_g_sensor_state))  //return if sensor has been closed.
    {
        return;
    }
    if(is_g_sportsapp_pedometer_bground_flag() == FALSE)
    {
        OEM_GetConfig(CFGI_GSENSOR,&shake,sizeof(shake));
        if((shake & OEMNV_SHAKE_SNOOZE_ALARM_MASK) && (TRUE == is_alarm_time_reach()))
        {
            return;
        }
        //if(shake & OEMNV_SHAKE_MUSIC_MASK) 
        {
#ifdef FEATRUE_SUPPORT_G_SENSOR
#ifndef WIN32//wlh 临时
            mmi_g_sensor_process(G_SENSOR_SHAKE_DISABLE);
#endif//WIN32
#endif
            mmi_g_sensor_state = MMI_GSENSOR_SHAKE_CLOSE;
        }            
    }
    #endif
}

Mp3Player_Status GetMp3PlayerStatus(void)
{
    return g_eMp3PlayerStatus;
}

boolean IsMp3PlayerStatusNone(void)
{
    return (g_eMp3PlayerStatus == MP3STATUS_NONE);
}

boolean IsMp3PlayerStatusOnBG(void)
{
    return (g_eMp3PlayerStatus == MP3STATUS_RUNONBACKGROUND);
}


void SetMp3PlayerStatus(CMusicPlayer *pMe,Mp3Player_Status eSt)
{
    pMe->m_ePreMp3Status = g_eMp3PlayerStatus;
    g_eMp3PlayerStatus = eSt;
}

void MP3_ResetRandonIdentifier(CMusicPlayer *pMe)
{
    MEMSET(pMe->m_aIdentifier,0,MUSIC_MAX_FILESNUM * sizeof(boolean));
    pMe->m_aIdentifier[pMe->m_MusicPlayerCfg.lastPlayMusicID] = 1;
    pMe->m_nRandNum = 1;   
}

AEERect *GetPlayerSkinRect(CPlayerSkin ePlayerSkin)
{
    switch(ePlayerSkin)
    {
        case PLAYERSKIN1:
            return MP3SKIN1_RECT;
    }
	return NULL;
}

#ifdef FEATURE_SUPPORT_BT_APP
boolean GetIsPlayMP3ByBt(void)
{
    return g_bPlayMP3ByBt;
}
void SetIsPlayMP3ByBt(boolean bPlayMp3ByBt)
{
    g_bPlayMP3ByBt = bPlayMp3ByBt;
}
#endif

//extern boolean CMediaGallery_GetTflashStatus(void);
static void MP3_Build_DefaultPlaylist(CMusicPlayer *pMe)
{
    FileName szFileName;
    IFile *pFile;
    AEEFileInfoEx  fi;
    char destFileName[PLAYLISTNAME_MAX_LEN];	
	
	//Add By zzg 2010_08_17
	uint16 count = 0;		//遍历T卡文件夹的数量
	char  Folder[50][MAX_STR_LEN];		//最多允许50个T卡文件夹
	//Add End
    
    ASSERT(pMe != NULL); 
    if(pMe == NULL||pMe->m_pFileMgr==NULL)
    {
     return;
    }
    // 如果该目录不存在创建目录
    if(SUCCESS != IFILEMGR_Test(pMe->m_pFileMgr, MUSICPLAYLIST_DIR))
    {
     (void)IFILEMGR_MkDir(pMe->m_pFileMgr, MUSICPLAYLIST_DIR);
    }
    (void)STRCPY(destFileName,MUSICPLAYLIST_DIR);
    (void)STRCAT(destFileName,"/");
    (void)STRCAT(destFileName,DEFAULT_PLAYLIST);
    (void)STRCAT(destFileName,".txt");
    //(void)STRCPY(destFileName,DEFAULT_PLAYLIST);
    // 检查该文件是否存在，不存在则创建它
    if(SUCCESS != IFILEMGR_Test(pMe->m_pFileMgr, destFileName))
    {
        pFile=IFILEMGR_OpenFile(pMe->m_pFileMgr, destFileName, _OFM_CREATE);
        if(NULL != pFile)
        {
          IFILE_Release(pFile);
          pFile = NULL;
        }
    }

    // 开始添加音乐
   // 填充fi结构体
   MEMSET(&fi, 0, sizeof(fi));
   fi.nStructSize = sizeof(fi);
   fi.pszFile = szFileName.m_szFileName;
   fi.nMaxFile = sizeof(szFileName.m_szFileName);   

	//以前是有T卡时默认加载的就只有T卡里的音乐文件，没有T卡时就只加载PHONE里的音乐文件
	//现在是PHONE + T卡(如果有)

   (void)IFILEMGR_EnumInit(pMe->m_pFileMgr, MP3_PHONE_MUSIC_DIR, FALSE);

   pMe->m_nPlaylistMusicNum = 0;

   while (IFILEMGR_EnumNextEx(pMe->m_pFileMgr, &fi))
   {
	   char *			 psz = NULL;
	   char *			 pf = NULL;
	   
	   if(pMe->m_nPlaylistMusicNum >= MUSIC_MAX_FILESNUM)
	   {
		 break;
	   }
	   psz = STRRCHR(fi.pszFile, '.');
	   if (NULL == psz)
	   {
		 continue; 
	   }
	   psz++;
	   
	   pf = STRRCHR(fi.pszFile, '/');
	   if (NULL == pf)
	   {
		 continue;
	   }  
	   pf++;
	  
	   if ((*psz) && (*pf) && (0 == STRICMP(psz,MP3_TYPE) || 0 == STRICMP(psz,MIDI_TYPE)) 
		   && pMe->m_nPlaylistMusicNum < MUSIC_MAX_FILESNUM)
	   {					 
		 pMe->m_Musiclist[pMe->m_nPlaylistMusicNum].pMusicName[0]=(pMe->m_nPlaylistMusicNum+48);
		 STRLCPY(pMe->m_Musiclist[pMe->m_nPlaylistMusicNum].pMusicName+1,
				 fi.pszFile,MP3_MAX_FILE_NAME * sizeof(char));
		 pMe->m_nPlaylistMusicNum++;
		  continue; 
	   }
	   else
	   {					
		 continue;
	   }
   }

   //Add By zzg 2010_08_17
    if(CMediaGallery_GetTflashStatus())		//有T 卡时
    {
#if defined(FEATURE_VERSION_C337)    
       (void)IFILEMGR_EnumInit(pMe->m_pFileMgr, MG_MASSCARDMUSIC_PATH, FALSE); //T卡根目录的文件
#else
       (void)IFILEMGR_EnumInit(pMe->m_pFileMgr, AEEFS_CARD0_DIR, FALSE); //T卡根目录的文件
#endif
	     while (IFILEMGR_EnumNextEx(pMe->m_pFileMgr, &fi))
	     {
	          char *            psz = NULL;
	          char *            pf = NULL;
	          
	          if(pMe->m_nPlaylistMusicNum >= MUSIC_MAX_FILESNUM)
	          {
	            break;
	          }
	          psz = STRRCHR(fi.pszFile, '.');
	          if (NULL == psz)
	          {
	            continue; 
	          }
	          psz++;
	          
	          pf = STRRCHR(fi.pszFile, '/');
	          if (NULL == pf)
	          {
	            continue;
	          }  
	          pf++;
	          // 如果是musicplayer 支持的文件则加进来(有支持更多需要在此添加)
	          if ((*psz) && (*pf) && (0 == STRICMP(psz,MP3_TYPE) || 0 == STRICMP(psz,MIDI_TYPE)) 
	              && pMe->m_nPlaylistMusicNum < MUSIC_MAX_FILESNUM)
	          {
	            //为了保存添加播放列表中歌曲的顺序，这里用第一位来存储该歌曲的添加顺序，这里用'0'开始的50个字符表示
	            // PS:选择的该段字符最好不能含有'/''.'这两个字符
	            pMe->m_Musiclist[pMe->m_nPlaylistMusicNum].pMusicName[0]=(pMe->m_nPlaylistMusicNum+48);
	            STRLCPY(pMe->m_Musiclist[pMe->m_nPlaylistMusicNum].pMusicName+1,
	                    fi.pszFile,MP3_MAX_FILE_NAME * sizeof(char));
	            pMe->m_nPlaylistMusicNum++;
	             continue; 
	          }
	          else
	          {
	            // 如果我们不支持的文件则继续枚举下一个文件
	            continue;
	          }
	     }
    }
   //Add End
   
    //枚举playlist文件夹 
    /*
    if(CMediaGallery_GetTflashStatus())		//有T 卡时
    {
       (void)IFILEMGR_EnumInit(pMe->m_pFileMgr, MP3_SD_MUSIC_DIR, FALSE);  

	     while (IFILEMGR_EnumNextEx(pMe->m_pFileMgr, &fi))
	     {
	          char *            psz = NULL;
	          char *            pf = NULL;
	          
	          if(pMe->m_nPlaylistMusicNum >= MUSIC_MAX_FILESNUM)
	          {
	            break;
	          }
	          psz = STRRCHR(fi.pszFile, '.');
	          if (NULL == psz)
	          {
	            continue; 
	          }
	          psz++;
	          
	          pf = STRRCHR(fi.pszFile, '/');
	          if (NULL == pf)
	          {
	            continue;
	          }  
	          pf++;
	          // 如果是musicplayer 支持的文件则加进来(有支持更多需要在此添加)
	          if ((*psz) && (*pf) && (0 == STRICMP(psz,MP3_TYPE) || 0 == STRICMP(psz,MIDI_TYPE)) 
	              && pMe->m_nPlaylistMusicNum < MUSIC_MAX_FILESNUM)
	          {
	            //为了保存添加播放列表中歌曲的顺序，这里用第一位来存储该歌曲的添加顺序，这里用'0'开始的50个字符表示
	            // PS:选择的该段字符最好不能含有'/''.'这两个字符
	            pMe->m_Musiclist[pMe->m_nPlaylistMusicNum].pMusicName[0]=(pMe->m_nPlaylistMusicNum+48);
	            STRLCPY(pMe->m_Musiclist[pMe->m_nPlaylistMusicNum].pMusicName+1,
	                    fi.pszFile,MP3_MAX_FILE_NAME * sizeof(char));
	            pMe->m_nPlaylistMusicNum++;
	             continue; 
	          }
	          else
	          {
	            // 如果我们不支持的文件则继续枚举下一个文件
	            continue;
	          }
	     }
    }
    */

	 //枚举playlist文件夹     
    if (CMediaGallery_GetTflashStatus())		//有T 卡时
    {
#if defined(FEATURE_VERSION_C337)    
       (void)IFILEMGR_EnumInit(pMe->m_pFileMgr, MG_MASSCARDMUSIC_PATH, FALSE); //T卡根目录的文件
#else
       (void)IFILEMGR_EnumInit(pMe->m_pFileMgr, AEEFS_CARD0_DIR, TRUE); //T卡根目录的文件
#endif    
		 count = 0;
	     while ((IFILEMGR_EnumNextEx(pMe->m_pFileMgr, &fi)) &&(count<MUSIC_MAX_FILESNUM))
	     {
	          if (pMe->m_nPlaylistMusicNum >= MUSIC_MAX_FILESNUM)
	          {
	            break;
	          }

			  if (NULL != fi.pszFile)
			  {
				STRLCPY(Folder[count], fi.pszFile, MP3_MAX_FILE_NAME * sizeof(char));
				
				count ++;
	          	continue; 
	          }
	     }

		 
		 //依次遍历T卡里的所有文件夹
		 {
			int16 temp = 0;
			for (temp = 0; temp < count; temp ++)
			{	
			     STRLCPY(fi.pszFile, Folder[temp], MP3_MAX_FILE_NAME * sizeof(char));				
				
				(void)IFILEMGR_EnumInit(pMe->m_pFileMgr, fi.pszFile, FALSE); 
						  
				while (IFILEMGR_EnumNextEx(pMe->m_pFileMgr, &fi))
				{
				  char *            psz = NULL;
				  char *            pf = NULL;

				  if (pMe->m_nPlaylistMusicNum >= MUSIC_MAX_FILESNUM)
				  {
				    break;
				  }
				  
				  psz = STRRCHR(fi.pszFile, '.');
				  
				  if (NULL == psz)
				  {	
					  continue; 
				  }
				  
				  psz++;
				  
				  pf = STRRCHR(fi.pszFile, '/');
				  
				  if (NULL == pf)
				  {				  
				    continue;
				  }  
				  
				  pf++;
				  
				  // 如果是musicplayer 支持的文件则加进来(有支持更多需要在此添加)
				  if ((*psz) && (*pf) && (0 == STRICMP(psz,MP3_TYPE) || 0 == STRICMP(psz,MIDI_TYPE)) 
				      && pMe->m_nPlaylistMusicNum < MUSIC_MAX_FILESNUM)
				  {				  	
				    //为了保存添加播放列表中歌曲的顺序，这里用第一位来存储该歌曲的添加顺序，这里用'0'开始的50个字符表示
				    // PS:选择的该段字符最好不能含有'/''.'这两个字符
				    pMe->m_Musiclist[pMe->m_nPlaylistMusicNum].pMusicName[0]=(pMe->m_nPlaylistMusicNum+48);
				    STRLCPY(pMe->m_Musiclist[pMe->m_nPlaylistMusicNum].pMusicName+1,
				            fi.pszFile,MP3_MAX_FILE_NAME * sizeof(char));
				    pMe->m_nPlaylistMusicNum++;
				     continue; 
				  }
				  else
				  {				  	
				    // 如果我们不支持的文件则继续枚举下一个文件
				    continue;
				  }
				}
			}	
		 }
		 
	     
    }
     CMusicPlayer_WriteMusiclist(pMe,DEFAULT_PLAYLIST);
     (void)CMediaGallery_GetTflashStatus();
}

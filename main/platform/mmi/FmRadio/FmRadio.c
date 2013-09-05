/*==============================================================================
// 文件：
//        fmRadio.c
//        2007-11-11 Gemsea草拟版本(Draft Version)
//        版权所有(c) 2007 Anylook.,LTD.
//
// 文件说明：
//
// 作者：2007-11-11
// 创建日期：Gemsea
// 当前版本：Draft
==============================================================================*/

/*==============================================================================
修改历史记录：
时       间      修 改 人     修改内容、位置及原因
-----------      ----------     -----------------------------------------------
2007-11-11              初始版本
==============================================================================*/


/*==============================================================================
                                 本文件包含的外部文件
==============================================================================*/
#include "FmRadio_priv.h"
#ifdef FEATURE_APP_MUSICPLAYER
#include "MusicPlayer.h"
#endif 
#include "Appscommon.h"
#include "hs.h"
#include "snd.h"
/*==============================================================================
                                 宏定义和常数
==============================================================================*/
#define EVT_UI_EXIT                 1150

/*==============================================================================
                                 类型定义
==============================================================================*/

/*==============================================================================
                                 函数声明
==============================================================================*/
//static void FmRadio_APPIsReadyTimer(void *pme);

/*----------------------模块相关函数声明---------------------*/
 
int  FmRadioMod_Load(IShell   *pIShell,
    void     *ph,
    IModule **ppMod
);

int  FmRadioMod_New(int16  nSize,
    IShell   *pIShell,
    void     *ph,
    IModule  **ppMod,
    PFNMODCREATEINST pfnMC,
    PFNFREEMODDATA pfnMF
);

static int  FmRadioMod_CreateInstance(IModule *po,
    IShell *pIShell,
    AEECLSID ClsId,
    void **ppObj
);

static uint32 FmRadioMod_AddRef(IModule *po);

static uint32 FmRadioMod_Release(IModule *po);

static void FmRadioMod_FreeResources(IModule  *po,
    IHeap  *ph,
    IFileMgr *pfm
);

/*----------------------Applet相关函数声明---------------------*/
static int FmRadio_New(IShell *ps,
    IModule *pIModule,
    IFmRadio **ppObj
);

static uint32  FmRadio_AddRef(IFmRadio *pi);

static uint32  FmRadio_Release (IFmRadio *pi);

static boolean FmRadio_HandleEvent(IFmRadio *pi,
    AEEEvent eCode,
    uint16  wParam,
    uint32 dwParam
);

static int FmRadio_InitAppData(CFmRadio *pMe);
static void FmRadio_FreeAppData(CFmRadio *pMe);
static void FmRadio_RunFSM(CFmRadio *pMe);
static void FmRadio_InitFmRadioResource(CFmRadio *pMe);
static void FmRadio_PowerDown( CFmRadio *pMe);
static void FmRadio_PowerUp(void *pme);
static void FmRadio_AppIsReadyCB(void *pUser);

/*==============================================================================
                                 全局数据
==============================================================================*/
// 重要提示：
//        g_dwAEEStdLibEntry全局变量仅用于SDK ！
//        BREW SDK用户不能改写和直接使用本变量。它用作AEEStdLib的一个入口点，不
//        要移动下列三行代码。
#ifdef AEE_SIMULATOR
   uint32 g_dwAEEStdLibEntry;
#endif

#if FEATURE_FMRADIO_CHANNEL_LIST_SUPPORT
void FmRadio_ClearChanList( CFmRadio* pMe);
void FmRadio_ChanList_EnumInit( CFmRadio* pMe);
void FmRadio_ChanList_EnumInit_WithLoop( CFmRadio* pMe);
boolean FmRadio_ChanList_EnumNext( CFmRadio* pMe);
boolean FmRadio_ChanList_EnumPrevious_WithLoop( CFmRadio* pMe);
boolean FmRadio_ChanList_EnumNext_WithLoop( CFmRadio* pMe);
sChanInfo* FmRadio_ChanList_GetCurrent( CFmRadio* pMe);
sChanInfo* FmRadio_ChanList_GetCurrent_WithLoop( CFmRadio* pMe);
sChanInfo* FmRadio_ChanList_GetByIndex( CFmRadio* pMe, int index);
boolean FmRadio_ChanList_DeleteByIndex( CFmRadio* pMe, int index);
boolean FmRadio_IsChannelValid( uint16 channel);

word FmRadio_GetPlayingChannel(CFmRadio* pMe);      //Add By zzg 2013_09_02
#endif
void Fm_Shake_Open(void);
/*==============================================================================
                                 本地（静态）数据
==============================================================================*/
static FmRadioMod gFmRadioMod;

static const VTBL(IModule) gModFuncs =
{
    FmRadioMod_AddRef,
    FmRadioMod_Release,
    FmRadioMod_CreateInstance,
    FmRadioMod_FreeResources
};

// 只允许一个FmRadio实例。每次创建FmRadio Applet时，
// 返回同一结构指针给BREW层。
static CFmRadio gFmRadio;

static const VTBL(IFmRadio) gFmRadioMethods =
{
    FmRadio_AddRef,
    FmRadio_Release,
    FmRadio_HandleEvent
};

static boolean g_m_fm_is_on = FALSE;
/*----------------------模块相关函数定义---------------------*/

/*==============================================================================
函数:
       FmRadioMod_Load

说明:
       模块装载函数。

参数:
       pIShell [in]：IShell接口指针。
       ph ：没有使用
       ppMod [out]：

返回值:
       int

备注:

==============================================================================*/
#ifndef BREW_STATIC_APP
__declspec(dllexport) int AEEMod_Load(IShell   *pIShell,
                                                        void     *ph,
                                                        IModule **ppMod)
#else
int FmRadioMod_Load(IShell   *pIShell,
                                                   void     *ph,
                                                   IModule **ppMod)
#endif
{
    // 对于动态Applet，验证AEE STD库版本，以确保库与建立本模块时使用的库版本匹配
    // 。在AEE_SIMULATOR下不必进行本检查。
#if !defined(BREW_STATIC_APP) && !defined(AEE_SIMULATOR)
    if (GET_HELPER_VER() != AEESTDLIB_VER)
    {
        return EVERSIONNOTSUPPORT;
    }
#endif

    return FmRadioMod_New(sizeof(FmRadioMod),
                          pIShell,
                          ph,
                          ppMod,
                          NULL,
                          NULL);
}

/*==============================================================================
函数:
       FmRadioMod_New

说明:
       用来初始化FmRadio模块虚拟表（也增加引用计数）的助手函数。

参数:
       nSize:        没有使用
       pIShell [In]: IShell接口指针
       ph ：没有使用
       ppMod [out]：
       pfnMC:        没有使用
       pfnMF:        没有使用

返回值:
       int

备注:

==============================================================================*/
int  FmRadioMod_New(int16  nSize,
                    IShell   *pIShell,
                    void     *ph,
                    IModule  **ppMod,
                    PFNMODCREATEINST pfnMC,
                    PFNFREEMODDATA pfnMF
                   )
{

    if (NULL == ppMod)
    {
        return EFAILED;
    }

    *ppMod = NULL;

    // 重要提示：:
    //        g_dwAEEStdLibEntry全局变量仅用于SDK ！
    //        BREW SDK用户不能改写和直接使用本变量。它用作AEEStdLib的一个入口点
    //        。
#ifdef AEE_SIMULATOR
    g_dwAEEStdLibEntry = (uint32)ph;
    if (!pIShell || !ph)
    {
        return EFAILED;
    }
#else
    if (!pIShell)
    {
        return EFAILED;
    }
#endif

    MEMSET(&gFmRadioMod,0,sizeof(FmRadioMod));
    INIT_VTBL(&gFmRadioMod, IModule, gModFuncs);

    gFmRadioMod.m_nRefs = 1;
    *ppMod = (IModule *)&gFmRadioMod;

    return AEE_SUCCESS;
}

/*==============================================================================
函数:
       FmRadioMod_AddRef

说明:
       FmRadio 模块引用计数加1，并返回当前的引用数。

参数:
       po [in]：IModule接口对象指针。

返回值:
       当前对FmRadio 模块的引用数

备注:

==============================================================================*/
static uint32 FmRadioMod_AddRef(IModule *po)
{
    return(++((FmRadioMod *)po)->m_nRefs);
}

/*==============================================================================
函数:
       FmRadioMod_Release

说明:
       FmRadio 模块引用计数减1，并返回当前的引用数。

参数:
       po [in]：IModule接口对象指针。

返回值:
       当前对FmRadio 模块的引用数

备注:

==============================================================================*/
static uint32 FmRadioMod_Release(IModule *po)
{
    if (((FmRadioMod *) po)->m_nRefs == 0)
    {
        return 0;
    }

    return(--((FmRadioMod *)po)->m_nRefs);
}

/*==============================================================================
函数:
       FmRadioMod_CreateInstance

说明:
        通过创建(初始化)FmRadio对象来创建模块实例。

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

==============================================================================*/
static int  FmRadioMod_CreateInstance(IModule   *po,
                                      IShell    *pIShell,
                                      AEECLSID  ClsId,
                                      void      **ppObj
                                     )
{
    *ppObj = NULL;

    if (ClsId != AEECLSID_APP_FMRADIO)
    {
        return EFAILED;
    }

    if (FmRadio_New(pIShell, po, (IFmRadio**)ppObj) != SUCCESS)
    {
        return EFAILED;
    }
    return SUCCESS;
}

/*==============================================================================
函数:
       FmRadioMod_FreeResources

说明:
        释放FmRadio模块资源的函数。

参数:
       po ：IModule。
       ph ：IHeap接口指针。
       pfm :IFileMgr接口指针。

返回值:
       无

备注:
       不需用户去调用。

==============================================================================*/
static void FmRadioMod_FreeResources(IModule  *po,
    IHeap  *ph,
    IFileMgr *pfm
)
{

}

/*----------------------Applet相关函数定义---------------------*/
/*==============================================================================
函数:
       FmRadio_New

说明:
        初始化FmRadio Applet虚拟表和内部数据变量，同时其引用计数加1。

参数:
       ps [in]：IShell接口对象指针。
       pIModule [in]：pIModule接口对象指针。
       ppObj [out]:：指向IFmRadio对象指针的指针。

返回值:
       EFAILED：失败。
       SUCCESS：成功。

备注:
       不需用户去调用。
       关于Applet结构对象可使用动态分配内存的方式创建，本处采用的是使用静态
       变量的方式。建议对结构比较大的Applet使用静态变量的方式。对于动态分配
       内存的方式需要注意对所分配空间的释放。

==============================================================================*/
static int FmRadio_New(IShell *ps,
                       IModule *pIModule,
                       IFmRadio **ppObj
                      )
{
    int retVal = SUCCESS;

    if (0 == gFmRadio.m_nRefs)
    {
        // Must initialize the object
        MEMSET(&gFmRadio,  0, sizeof(CFmRadio));
        INIT_VTBL(&gFmRadio, IFmRadio, gFmRadioMethods);

        gFmRadio.m_nRefs     = 0;
        gFmRadio.m_pShell    = ps;
        gFmRadio.m_pModule   = pIModule;

        (void) ISHELL_AddRef(ps);
        (void) IMODULE_AddRef(pIModule);

        retVal = FmRadio_InitAppData(&gFmRadio);
    }

    ++gFmRadio.m_nRefs;
    *ppObj = (IFmRadio*)&gFmRadio;
    return retVal;
}

/*==============================================================================
函数:
       FmRadio_AddRef

说明:
       FmRadio Applet 引用计数加1，并返回当前的引用数。

参数:
       pi [in]：IFmRadio接口对象指针。

返回值:
       当前对FmRadio Applet的引用数

备注:

==============================================================================*/
static uint32  FmRadio_AddRef(IFmRadio *pi)
{
    register CFmRadio *pMe = (CFmRadio*)pi;

    return (++pMe->m_nRefs);
}

/*==============================================================================
函数:
       FmRadio_Release

说明:
       释放对FmRadio Applet的引用，引用计数减1，并返回当前的引用数。
        如果当前的引用数为零，还将释放Applet及相关资源。

参数:
       pi [in]：IFmRadio接口对象指针。

返回值:
       当前对FmRadio Applet的引用数

备注:

==============================================================================*/
static uint32  FmRadio_Release (IFmRadio *pi)
{
    register CFmRadio *pMe = (CFmRadio*)pi;

    if (pMe->m_nRefs == 0)
    {
        return 0;
    }

    if (--pMe->m_nRefs)
    {
        return pMe->m_nRefs;
    }

    // 释放Applet相关资源
    FmRadio_FreeAppData(pMe);

    (void) ISHELL_Release(pMe->m_pShell);
    (void) IMODULE_Release(pMe->m_pModule);

    // 注意：pMe是静态分配空间，因此不需要释放。FREE()
    return 0;
}

static int FmRadio_InitAppData(CFmRadio *pMe)
{
	if (NULL  == pMe)
    {
        return EFAILED;
    }

    if( (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_CONFIG, (void **)&pMe->m_pConfig)) ||
        (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_MENUCTL, (void **)&pMe->m_pMenu))  ||
        (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_STATIC, (void **)&pMe->pStatic))   ||
        (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_TEXTCTL, (void **)&pMe->pText))    ||
        (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_ANNUNCIATOR, (void **)&pMe->m_pIAnn))
#if !defined( AEE_SIMULATOR)
        ||
        #ifdef FEATURE_USES_LOWMEM
        ISHELL_RegisterNotify( pMe->m_pShell, AEECLSID_APP_FMRADIO, AEECLSID_PHONENOTIFIER, AEET_NMASK_VOICE_CALL | AEET_NMASK_OTHER_CALL | AEET_NMASK_TEST_CALL) != SUCCESS
        #else
        ISHELL_RegisterNotify( pMe->m_pShell, AEECLSID_APP_FMRADIO, AEECLSID_CM_NOTIFIER, NMASK_CM_VOICE_CALL | NMASK_CM_OTHER_CALL | NMASK_CM_TEST_CALL) != SUCCESS
        #endif
#endif
    )
    {
        FmRadio_FreeAppData( pMe);
        return EFAILED;
    }
    IANNUNCIATOR_SetHasTitleText(pMe->m_pIAnn, FALSE);
    pMe->m_ePreState            = FMRADIOST_NONE;
    pMe->m_eCurState            = FMRADIOST_INIT;
    pMe->m_pActiveDlg           = NULL;
    pMe->m_eDlgRet              = DLGRET_CREATE;
    pMe->m_bNotOverwriteDlgRet  = FALSE;
    pMe->startFromBackground    = FALSE;
    pMe->fmVolumeStop = TRUE;
    pMe->fmSpeaker=FALSE;
    pMe->m_pVolumeImage = NULL;

    pMe->bMsgBoxExist = FALSE;  //Add By zzg 2013_09_03
    pMe->bCurrect = FALSE;      //Add By zzg 2013_09_03
    
    //初始化必要的数据
    FmRadio_InitFmRadioResource( pMe);
    return SUCCESS;
}

/*==============================================================================
函数:
       FmRadio_InitFmRadioResource

说明:
       FmRadio  Applet事件处理主函数。所有属于本Applet的事件，
       都将经由该函数处理。

参数:

返回值:

备注:

==============================================================================*/
static void FmRadio_InitFmRadioResource(CFmRadio *pMe)
{

	byte byMax = 0;

#if FEATURE_FMRADIO_CHANNEL_LIST_SUPPORT
	FmRadio_ClearChanList( pMe);
#endif
	//初始化FM Radio音量
	(void) ICONFIG_GetItem(pMe->m_pConfig,
						   CFGI_FMRADIO_VOLUME,
						   &pMe->byVolumeLevel,
						   sizeof(byte));
	if( pMe->byVolumeLevel > MAX_FMRADIO_VOLUME || pMe->byVolumeLevel == 0)
	{
		pMe->byVolumeLevel = 3;//(MAX_FMRADIO_VOLUME + 1) / 2;
	}

#if FEATURE_FMRADIO_CHANNEL_LIST_SUPPORT
	//初始化各频道的信息
	(void) ICONFIG_GetItem(pMe->m_pConfig,
						   CFGI_FMRADIO_CHAN_INFO,
						   (void*) pMe->chanInfoList,
						   sizeof( pMe->chanInfoList));

	//初始化频道总数
	(void) ICONFIG_GetItem(pMe->m_pConfig,
						   CFGI_FMRADIO_CHAN_TOTAL,
						   &byMax,
						   sizeof(byte));

	//在下面将Node添加到List的过程中byChannelMax会自动增加。
	if( byMax > MAX_FMRADIO_STORED_CHANNEL)
	{
		byMax = MAX_FMRADIO_STORED_CHANNEL;
	}

	pMe->byChannelMax                     = byMax;
	pMe->channelListMenuSelectedItemId    = -1;
	//pMe->opMode         = FM_RADIO_OPMODE_PLAY;
	//pMe->refuseReason   = FM_RADIO_REFUSE_REASON_NOT_REFUSE;
	FmRadio_ChanList_EnumInit( pMe);
	FmRadio_ChanList_EnumInit_WithLoop( pMe);

	if( ISHELL_GetPrefs( pMe->m_pShell, AEECLSID_APP_FMRADIO, 1, &pMe->cfg, sizeof(pMe->cfg)) == SUCCESS)
	{

		while( FmRadio_ChanList_EnumNext( pMe))
		{
			if( FmRadio_ChanList_GetCurrent( pMe)->wChannel == pMe->cfg.channel)
			{
				goto __FmRadio_InitFmRadioResource_end__;
			}
		}

		if( pMe->cfg.tuningMode < 0 || pMe->cfg.tuningMode > 2)
		{
			pMe->cfg.tuningMode = FM_RADIO_TUNNING_MODE_LIST;
		}
	}
	else
	{

		if( FmRadio_ChanList_EnumNext( pMe))
		{
			pMe->cfg.channel = FmRadio_ChanList_GetCurrent( pMe)->wChannel;
		}
		else
		{
#ifdef FEATURE_VERSION_C337	
			pMe->cfg.channel = 36;
#else
			pMe->cfg.channel = 0;
#endif
		}

		pMe->cfg.tuningMode = FM_RADIO_TUNNING_MODE_LIST;
	}
#else
#ifdef FEATURE_VERSION_C337	
	pMe->cfg.channel = 36;
#else
	pMe->cfg.channel = 96;
#endif
	pMe->cfg.tuningMode = FM_RADIO_TUNNING_MODE_MANUAL;
#endif


__FmRadio_InitFmRadioResource_end__:
	pMe->opMode         = FM_RADIO_OPMODE_PLAY;
	pMe->refuseReason   = FM_RADIO_REFUSE_REASON_NOT_REFUSE;
}

/*==============================================================================
函数:
       FmRadio_FreeAppData

说明:
       释放FmRadio Applet使用的相关资源。

参数:
       pMe [in]：指向FmRadio Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
       无。

备注:

==============================================================================*/
static void FmRadio_FreeAppData(CFmRadio *pMe)
{
    if (NULL == pMe)
    {
        return ;
    }

    ISHELL_RegisterNotify( pMe->m_pShell,
                           AEECLSID_APP_FMRADIO,
                           AEECLSID_CM_NOTIFIER,
                           0
                      );

    if (pMe->m_pConfig)
    {
        ICONFIG_Release(pMe->m_pConfig);
        pMe->m_pConfig = NULL;
    }

    if(pMe->m_pMenu)
    {
        IMENUCTL_Release(pMe->m_pMenu);
        pMe->m_pMenu = NULL;
    }
    if(pMe->pStatic)
    {
        ISTATIC_Release(pMe->pStatic);
        pMe->pStatic = NULL;
    }
    if(pMe->pText)
    {
        ITEXTCTL_Release(pMe->pText);
        pMe->pText = NULL;
    }
    if(pMe->m_pIAnn)
    {
        IANNUNCIATOR_SetHasTitleText(pMe->m_pIAnn, TRUE);
        IANNUNCIATOR_Release( pMe->m_pIAnn);
        pMe->m_pIAnn = NULL;
    }
    if (pMe->m_pVolumeImage)
    {
       IIMAGE_Release(pMe->m_pVolumeImage);
       pMe->m_pVolumeImage = NULL;
    }    
}

/*==============================================================================
函数:
       FmRadio_RunFSM

说明:
       FmRadio Applet有限状态机引擎。

参数:
       pMe [in]：指向FmRadio Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
       无。

备注:

==============================================================================*/
static void FmRadio_RunFSM(CFmRadio *pMe)
{
    NextFSMAction nextFSMAction = NFSMACTION_WAIT;

    for ( ; ; )
    {
        nextFSMAction = FmRadio_ProcessState(pMe);

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

/*==============================================================================
函数:
       FmRadio_HandleEvent

说明:
       FmRadio  Applet事件处理主函数。所有属于本Applet的事件，
       都将经由该函数处理。

参数:
       pi [in]：指向IFmRadio接口t对象的指针。
       eCode：BREW事件代码。
       wParam：与事件相关联的数据。
       dwParam [in]：与事件相关联的数据。

返回值:
       TRUE：传入事件被处理。
       FALSE：传入事件没被处理。

备注:

==============================================================================*/
static void FmRadio_CheckRefuse( CFmRadio* pMe)
{
#if !FEATURE_TEST_VERSION_WITHOUT_HEADSET_PRESENCE_VERIFY
    boolean             headsetPresent   = TRUE;
#endif

#if !FEATURE_TEST_VERSION_WITHOUT_HEADSET_PRESENCE_VERIFY
    //ICONFIG_GetItem(pMe->m_pConfig, CFGI_HEADSET_PRESENT, &headsetPresent, sizeof(boolean));
    headsetPresent = HS_HEADSET_ON();
#endif
#if 0 //当有来电时，会自动挂起，不需要做其他处理
    db_get( DB_IN_USE, &dbItemValue);

    pMe->refuseReason = FM_RADIO_REFUSE_REASON_NOT_REFUSE;
    if( dbItemValue.in_use)
    {
        pMe->refuseReason = FM_RADIO_REFUSE_REASON_VOICE_CALL_CONNECTED;
    }
#endif    
#if !FEATURE_TEST_VERSION_WITHOUT_HEADSET_PRESENCE_VERIFY
    if( !headsetPresent)
    {
        pMe->refuseReason = FM_RADIO_REFUSE_REASON_NO_HEADSET;
    }
#endif
    // bw:<< 20080808 : close app when pull out the headset
#if !FEATURE_TEST_VERSION_WITHOUT_HEADSET_PRESENCE_VERIFY
    if( headsetPresent)
    {
        ISHELL_CancelTimer( pMe->m_pShell, closeApp, (void*)pMe);
    }
    else
    {
        ISHELL_SetTimer( pMe->m_pShell, 2000, closeApp, (void*)pMe);
    }
#endif
    //bw:>>
}

static boolean FmRadio_HandleEvent(IFmRadio *pi,
                                   AEEEvent eCode,
                                   uint16  wParam,
                                   uint32 dwParam
                                  )
{
    CFmRadio *pMe = (CFmRadio*)pi;
    AEEAppStart *as;

    switch (eCode)
    {
        case EVT_APP_START:
        case EVT_APP_RESUME:
        {   
            pMe->startFromBackground = FALSE;
            as = (AEEAppStart*)dwParam;
            pMe->m_pDisplay = as->pDisplay;
            pMe->m_rc = as->rc;
            {
                AEEDeviceInfo di; 
                ISHELL_GetDeviceInfo(pMe->m_pShell,&di);
                pMe->m_rc.dx = di.cxScreen;
                pMe->m_rc.dy = di.cyScreen;
            }
            if(pMe->m_pIAnn != NULL)
            {
                IANNUNCIATOR_SetHasTitleText(pMe->m_pIAnn, FALSE);            
                IANNUNCIATOR_Redraw(pMe->m_pIAnn);
            }

            pMe->m_bSuspending  = FALSE;

            //pMe->fmVolumeStop  = TRUE;
            
            if(APP_MEDIA_IMPACT_BY_MP3 == app_media_scheduler())
            {
                pMe->opMode = FM_RADIO_OPMODE_USER_MSG;
                pMe->m_eCurState = FMRADIOST_USER_MSG;
#if FEATURE_FMRADIO_SUPPORT_BACKGROUND                
                pMe->runOnBackground = TRUE;
#endif
                pMe->m_bAppIsReady = TRUE;
                FmRadio_RunFSM( pMe);
                return TRUE;
            }
			
            pMe->m_eCurState = FMRADIOST_INIT;  
            pMe->m_bAppIsReady  = FALSE;

			//Add By zzg 2010_07_18
#if FEATURE_FMRADIO_SUPPORT_BACKGROUND 			
			if (!pMe->runOnBackground )
#endif      
            #if defined (FEATURE_VERSION_SKY) || defined (FEATURE_VERSION_EC99) || defined (FEATURE_VERSION_K212_20D)
			if (HS_HEADSET_ON())
			{
               pMe->fmSpeaker=FALSE;
            }
            else
			{
               pMe->fmSpeaker=TRUE;
            }
            #endif
#if !defined( AEE_SIMULATOR)
            FmRadio_PowerUp( pMe);
            #ifndef FEATURE_VERSION_SKY
            #ifndef FEATURE_VERSION_EC99
			#ifndef FEATURE_VERSION_K212_20D
			if (HS_HEADSET_ON())
            #endif
			#endif
            #endif    
			{
#ifdef FEATURE_ANALOG_TV
                WarT_Fm_Mute(FALSE);
#else             
                fm_mute(FALSE,pMe->fmSpeaker);

                if ((eCode == EVT_APP_RESUME) && (!pMe->fmVolumeStop))
                {
                    fm_set_volume(0, pMe->fmSpeaker); 
                    pMe->fmVolumeStop=FALSE;
                }	
#endif
			}	
			//Add End
			
#endif//#if !defined( AEE_SIMULATOR)
#if FEATURE_FMRADIO_SUPPORT_BACKGROUND 
            pMe->runOnBackground = FALSE;
#endif            
            if(NULL != as->pszArgs)
            {
                if( 'e' == as->pszArgs[0] )
                {
#if FEATURE_FMRADIO_SUPPORT_BACKGROUND                 
                    pMe->runOnBackground = TRUE;
#endif
                    ISHELL_CloseApplet(pMe->m_pShell, FALSE);
                    return TRUE;
                }
            }
            FmRadio_RunFSM( pMe);
        }
        return TRUE;
        case EVT_APP_STOP:			
            if( pMe->startFromBackground) 
            {
            	ISHELL_CancelTimer( pMe->m_pShell, 0, pMe);
            	return TRUE;
            }
			
#if !defined( AEE_SIMULATOR)
#if FEATURE_FMRADIO_SUPPORT_BACKGROUND 
            if( !pMe->runOnBackground)
#endif                
            {
            	//Add By zzg 2010_07_18
				//if (HS_HEADSET_ON())	//Del by zzg 2010_08_24
				{
#ifdef FEATURE_ANALOG_TV
                    WarT_Fm_Mute(TRUE);
#else
					fm_mute(TRUE,pMe->fmSpeaker);
#endif
				}
				
                FmRadio_PowerDown( pMe);								
            #if FEATURE_FMRADIO_CHANNEL_LIST_SUPPORT
				FmRadio_SaveChannelList( pMe);
			#endif
				ISHELL_SetPrefs( pMe->m_pShell, AEECLSID_APP_FMRADIO, 1, &pMe->cfg, sizeof(pMe->cfg));
            }
#endif//#if !defined( AEE_SIMULATOR)
            pMe->m_bSuspending = TRUE;
#if FEATURE_FMRADIO_SUPPORT_BACKGROUND 
            *((boolean*)dwParam) = !pMe->runOnBackground;
#endif
            g_m_fm_is_on = FALSE;
            (void)ISHELL_CancelTimer( pMe->m_pShell,FmRadio_AppIsReadyCB,pMe);
            return TRUE;

        case EVT_APP_SUSPEND:			
#if !defined( AEE_SIMULATOR)
            FmRadio_PowerDown( pMe);			
#endif//#if !defined( AEE_SIMULATOR)
#if FEATURE_FMRADIO_SUPPORT_BACKGROUND 
			//Add By zzg 2010_07_18
			if( !pMe->runOnBackground)
#endif                
			{
                #ifndef FEATURE_VERSION_SKY
                #ifndef FEATURE_VERSION_EC99
				#ifndef FEATURE_VERSION_K212_20D
				if (HS_HEADSET_ON())
                #endif
				#endif
                #endif    
				{
#ifdef FEATURE_ANALOG_TV
                    WarT_Fm_Mute(TRUE);
#else
					fm_mute(TRUE,pMe->fmSpeaker);
#endif
				}			
			}
			//Add End
			
            pMe->m_bSuspending = TRUE;
            (void)ISHELL_CancelTimer( pMe->m_pShell,FmRadio_AppIsReadyCB,pMe);
            return TRUE;

        case EVT_DIALOG_INIT:
            if( wParam == OEM_IME_DIALOG)
			{
				return TRUE;
			}
            pMe->m_pActiveDlg = (IDialog*)dwParam;
            pMe->m_pActiveDlgID = wParam;
             return FmRadio_RouteDialogEvent(pMe,eCode,wParam,dwParam);
         case EVT_DIALOG_START:
			if(OEM_IME_DIALOG == wParam)
			{
				return TRUE;
			}

            (void)ISHELL_SetTimer( pMe->m_pShell,                                       
                                       APPISREADY_TIMER,
                                       FmRadio_AppIsReadyCB,
                                       pMe);
            
            (void) FmRadio_RouteDialogEvent(pMe,eCode,wParam,dwParam);

            return TRUE;
        case EVT_USER_REDRAW:			
            if( eCode == EVT_USER_REDRAW)
            {
                /*
                (void)ISHELL_SetTimer( pMe->m_pShell,                                       
                                       APPISREADY_TIMER,
                                       FmRadio_AppIsReadyCB,
                                       pMe);
                */
            		//pMe->m_bAppIsReady = TRUE;                         
            }
            (void) FmRadio_RouteDialogEvent(pMe,eCode,wParam,dwParam);
            return TRUE;

        case EVT_KEY:
        case EVT_KEY_PRESS:
            if( pMe->m_bAppIsReady &&
                ( ( (wParam == AVK_CLR || wParam == AVK_SOFT2 || wParam == AVK_SELECT || wParam == AVK_SOFT1) &&
                    pMe->refuseReason != FM_RADIO_REFUSE_REASON_NOT_REFUSE
                  ) || wParam == AVK_END
                )
            )
            {
                //pMe->runOnBackground = (wParam == AVK_END);
#if FEATURE_FMRADIO_SUPPORT_BACKGROUND 
				//Add By zzg 2010_07_30
				if ((pMe->refuseReason != FM_RADIO_REFUSE_REASON_NO_HEADSET) 
					&& (pMe->refuseReason != FM_RADIO_REFUSE_REASON_HEADSET_PLUG_OUT))
				{
					pMe->runOnBackground = (wParam == AVK_END);
				}
				else
				{
					pMe->runOnBackground = FALSE;
				}
				//Add End
#endif				
				
                pMe->opMode = FM_RADIO_OPMODE_PLAY;
                ISHELL_CloseApplet( pMe->m_pShell, wParam == AVK_END ? TRUE : FALSE);
                return TRUE;
            }
        case EVT_KEY_RELEASE:
        case EVT_COMMAND:
            if (!pMe->m_bAppIsReady || pMe->refuseReason != FM_RADIO_REFUSE_REASON_NOT_REFUSE || pMe->newSmsIncoming)
            {
                return TRUE;
            }
            return FmRadio_RouteDialogEvent(pMe,eCode,wParam,dwParam);

        case EVT_DIALOG_END:			
            if (wParam == 0)
            {
                return TRUE;
            }
            if(OEM_IME_DIALOG == wParam)
			{
				return ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_FMRADIO,EVT_USER_REDRAW,0,0);
			}            
            (void) FmRadio_RouteDialogEvent(pMe,eCode,wParam,dwParam);
            pMe->m_pActiveDlg = NULL;

            // Applet被挂起，不跑状态机，等待Applet返回。（注意：EVT_APP_SUSPEND
            // 事件在EVT_DIALOG_END事件前发出。
            if( !pMe->m_bSuspending)
            {
                // 开始FmRadio状态机
                FmRadio_RunFSM(pMe);
            }
            return TRUE;

        case EVT_NOTIFY:
        {			
#if FEATURE_FMRADIO_SUPPORT_BACKGROUND             
#if !defined( AEE_SIMULATOR)
            if( pMe->runOnBackground)
            {

                AEENotify* notify = (AEENotify*)dwParam;
                notify->st = NSTAT_IGNORED;
                if( notify->cls == AEECLSID_CM_NOTIFIER)
                {

                    if( ( notify->dwMask & NMASK_CM_VOICE_CALL))
                    {
                        AEECMNotifyInfo* eventInfo = (AEECMNotifyInfo *)(notify->pData);


                        if( eventInfo->event == AEECM_EVENT_CALL_ORIG    ||
                            eventInfo->event == AEECM_EVENT_CALL_INCOM   ||
                            eventInfo->event == AEECM_EVENT_CALL_END
                        )
                        {
                            if( eventInfo->event == AEECM_EVENT_CALL_END)
                            {
                                FmRadio_PowerUp( pMe);
                            }
                            else
                            {
                                FmRadio_PowerDown( pMe);
                            }

                            notify->st = NSTAT_PROCESSED;
                        }
                    }
                }
            }
#endif
#endif
            return TRUE;
        }

        case EVT_CLOSEAPP:
        {			
            pMe->startFromBackground = FALSE;
#if FEATURE_FMRADIO_SUPPORT_BACKGROUND             
            pMe->runOnBackground = FALSE;
#endif
            ISHELL_CloseApplet( pMe->m_pShell, FALSE);           
            return TRUE;
        }
        
        case EVT_HEADSET:
        {
            #if defined (FEATURE_VERSION_SKY) || defined (FEATURE_VERSION_EC99) || defined (FEATURE_VERSION_K212_20D)
            if (HS_HEADSET_ON())
			{
               pMe->fmSpeaker=FALSE;
            }
            else
			{
               pMe->fmSpeaker=TRUE;
            }
            pMe->byVolumeLevel=0;
            fm_set_volume( pMe->byVolumeLevel,pMe->fmSpeaker);
            (void) ICONFIG_GetItem(pMe->m_pConfig,
     						   CFGI_FMRADIO_VOLUME,
     						   &pMe->byVolumeLevel,
     						   sizeof(byte));
			#if defined(FEATURE_VERSION_C337)
			pMe->byVolumeLevel = (pMe->byVolumeLevel*3)/5;
			#endif
            fm_set_volume( pMe->byVolumeLevel,pMe->fmSpeaker);  
            #endif   
        	if ( wParam )
        	{
                //Nothing TODO
        	}
        	else
        	{
	            if( pMe->startFromBackground)
	            {
	                ISHELL_CloseApplet( pMe->m_pShell, FALSE);
	            }
	            else
	            {
	                FmRadio_PowerUp( pMe);
					if( pMe->refuseReason == FM_RADIO_REFUSE_REASON_NOT_REFUSE)
					{
						extern void resume( CFmRadio* pMe);
						resume( pMe);
					}
					else
					{
						if( IMENUCTL_IsActive( pMe->m_pMenu))
						{
							IMENUCTL_SetActive( pMe->m_pMenu, FALSE);
						}
						if( ITEXTCTL_IsActive( pMe->pText))
						{
							ITEXTCTL_SetActive( pMe->pText, FALSE);
						}

#if defined( FEATURE_FMRADIO_PROMPT_HEADSET_PLUG_OUT)
						if( pMe->refuseReason == FM_RADIO_REFUSE_REASON_NO_HEADSET)
						{
							pMe->refuseReason = FM_RADIO_REFUSE_REASON_HEADSET_PLUG_OUT;							
						}
#endif
					}
					ISHELL_PostEvent( pMe->m_pShell,
	                             AEECLSID_APP_FMRADIO,
	                             EVT_USER_REDRAW,
	                             0,
	                             0
	                         );
	            }
            }
            
        }
        return TRUE;

        case EVT_ALARM:
        {
#if FEATURE_FMRADIO_SUPPORT_BACKGROUND
            if( pMe->startFromBackground)
            {
                ISHELL_CloseApplet( pMe->m_pShell, FALSE);
            }
            else
            {
                pMe->alarmNow = wParam;
                if( wParam)
                {
                    FmRadio_PowerDown( pMe);
                }
                else
                {
                    FmRadio_PowerUp( pMe);
                }
            }
#endif            
        }
        return TRUE;

        case EVT_FLIP:
        {
#if FEATURE_FMRADIO_SUPPORT_BACKGROUND             
            if( !wParam)
            {
                pMe->runOnBackground = TRUE;
            }
#endif            
        }
        return TRUE;
        case EVT_OPENSHAKE:
            Fm_Shake_Open();
            return TRUE;
            
        case EVT_USER:
        {
            switch(wParam) 
            {
                case EVT_UI_EXIT:            
                    /* AEE is about to exit, set the exit flag to TRUE */
                    pMe->startFromBackground = FALSE;
#if FEATURE_FMRADIO_SUPPORT_BACKGROUND                     
                    pMe->runOnBackground = FALSE;
#endif
                    ISHELL_CloseApplet( pMe->m_pShell, FALSE);
                    return TRUE;
                default:
				{
					MSG_FATAL("FmRadio_RouteDialogEvent..............",0,0,0);
					if (wParam == AVK_CLR)
					{
						eCode = EVT_KEY;
					}
					else if ((wParam == AVK_SELECT)||(wParam == AVK_INFO))
					{
						if (dwParam == 0)
						{
							
							eCode = EVT_KEY;
						}
						else
						{
							eCode = EVT_COMMAND;
						}
					}
					return FmRadio_RouteDialogEvent(pMe,eCode,wParam,dwParam);
                }
                break;
            }
        }
        default:
            // 将接收到的事件路由至当前活动的对话框事件处理函数。
            return FmRadio_RouteDialogEvent(pMe,eCode,wParam,dwParam);
    }
}

static boolean fm_set_key_beep = 0; // 1:set, 0:free
/*closed key beep if it is open.*/
static void FM_CloseKeyBeepVol(CFmRadio *pMe)
{
    byte mute = OEMSOUND_MUTE_VOL;

    ICONFIG_GetItem( pMe->m_pConfig, CFGI_BEEP_VOL, &pMe->keyBeepVol, sizeof(byte));
    ICONFIG_SetItem( pMe->m_pConfig, CFGI_BEEP_VOL, &mute, sizeof(byte));
    fm_set_key_beep = 1;
}

/*resume key beep.*/
static void Fm_ResumeKeyBeepVol(CFmRadio * pMe)
{
    if(1 == fm_set_key_beep)    //free keybeep setting.
    {
        ICONFIG_SetItem( pMe->m_pConfig, CFGI_BEEP_VOL, &pMe->keyBeepVol, sizeof(byte));
        fm_set_key_beep = 0;
    
    }
}

#ifdef FEATRUE_SUPPORT_G_SENSOR
#include "g_sensor.h"
#endif
#ifdef FEATURE_SPORTS_APP
extern boolean is_g_sportsapp_pedometer_bground_flag(void);
#endif
extern boolean is_alarm_time_reach(void);

void Fm_Shake_Open(void)
{
#ifdef FEATRUE_SUPPORT_G_SENSOR
    dword shake;
    if((MMI_GSENSOR_SHAKE_OPEN == mmi_g_sensor_state) 
        ||(MMI_GSENSOR_SHAKE_OPEN_IN_IDLE == mmi_g_sensor_state))   //return if sensor has been open.
    {
        return;
    }

    if(is_g_sportsapp_pedometer_bground_flag() == FALSE)
    {
        OEM_GetConfig(CFGI_GSENSOR,&shake,sizeof(shake));
        if(shake & OEMNV_SHAKE_FM_RADIO_MASK)
        {
#ifdef FEATRUE_SUPPORT_G_SENSOR
            mmi_g_sensor_process(G_SENSOR_ENABLE | G_SENSOR_SHAKE);
#endif
            mmi_g_sensor_state = MMI_GSENSOR_SHAKE_OPEN;
        }    
    }
#endif 
}

void Fm_Shake_Close(void)
{
#ifdef FEATRUE_SUPPORT_G_SENSOR
    dword shake;

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
       // if(shake & OEMNV_SHAKE_FM_RADIO_MASK)
        {
#ifdef FEATRUE_SUPPORT_G_SENSOR
            mmi_g_sensor_process(G_SENSOR_SHAKE_DISABLE);
#endif
            mmi_g_sensor_state = MMI_GSENSOR_SHAKE_CLOSE;
        }            
    }
#endif
}

static void FmRadio_PowerDown( CFmRadio *pMe)
{
 #if FEATURE_FMRADIO_SUPPORT_BACKGROUND     
    ICONFIG_SetItem( pMe->m_pConfig, CFGI_FM_BACKGROUND, &b_FMBackground, sizeof(b_FMBackground));
#endif 
    IANNUNCIATOR_SetField( pMe->m_pIAnn, ANNUN_FIELD_FMRADIO, ANNUN_STATE_FMRADIO_OFF/*ANNUN_STATE_OFF*/);
    g_m_fm_is_on = FALSE;
#ifndef WIN32

    //Del By zzg 2010_08_11
    /*
	if (HS_HEADSET_ON())
	{
		fm_mute(TRUE);
	}
	*/
	//Del End
#ifdef FEATURE_ANALOG_TV
    WarT_Fm_PowerDown();
#else
    fm_radio_power_down();
#endif
#endif//WIN32
    Fm_Shake_Close(); 
    Fm_ResumeKeyBeepVol(pMe);

}

static void FmRadio_PowerUp(void *pme)
{

    CFmRadio *pMe = (CFmRadio *)pme;

    if( pMe->alarmNow ||
        (GETTIMESECONDS() - pMe->wmsEventTimestamp) < pMe->wmsEventTimer
    )
    {
        return;
    }

    pMe->newSmsIncoming = FALSE;
    #ifndef FEATURE_VERSION_SKY
    #ifndef FEATURE_VERSION_EC99  
	#ifndef FEATURE_VERSION_K212_20D
    FmRadio_CheckRefuse( pMe);
    #endif
	#endif
	#endif
    if( pMe->refuseReason == FM_RADIO_REFUSE_REASON_VOICE_CALL_CONNECTED)
    {
#ifndef WIN32
#ifdef FEATURE_ANALOG_TV
        if( WarT_Fm_Get_status()==FM_POWER_UP)
#else
        if( fm_radio_is_power_up())
#endif
        {
            FmRadio_PowerDown( pMe);
        }
#endif//WIN32
        return;
    }

#if !defined( AEE_SIMULATOR)
#ifdef FEATURE_ANALOG_TV
    if( WarT_Fm_Get_status()==FM_NOT_INIT)
    {
        WarT_Fm_Init();
        ISHELL_SetTimer( pMe->m_pShell, 1000, FmRadio_PowerUp, pMe);
        return;
    }
#else
    if( !fm_radio_is_inited())
    {
        fm_radio_init();
        ISHELL_SetTimer( pMe->m_pShell, 1000, FmRadio_PowerUp, pMe);
        return;
    }
#endif
    g_m_fm_is_on = TRUE;
#ifdef FEATURE_ANALOG_TV
    if( WarT_Fm_Get_status()!=FM_POWER_UP)
#else
    if( !fm_radio_is_power_up())
#endif
    {
#if FEATURE_FMRADIO_SUPPORT_BACKGROUND     
        boolean b_FMBackground = TRUE;

        ICONFIG_SetItem(pMe->m_pConfig, CFGI_FM_BACKGROUND, &b_FMBackground, sizeof(b_FMBackground));
#endif
        IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_FMRADIO, ANNUN_STATE_FMRADIO_ON/*ANNUN_STATE_ON*/);
#ifdef FEATURE_ANALOG_TV
        WarT_Fm_PowerUp();
#else
        fm_radio_power_up();
#endif
        FM_CloseKeyBeepVol(pMe);
        Fm_Shake_Open();
#ifdef FEATURE_ANALOG_TV
        if( WarT_Fm_Set_Channel((LOWEST_BAND_FREQ + CHANNEL_SPACE * pMe->cfg.channel)/100) == WART_FM_SUCCESS)
#else
        if( fm_tune_channel((LOWEST_BAND_FREQ + CHANNEL_SPACE * pMe->cfg.channel)/100) == FM_RADIO_SUCCESSFUL)
#endif
        {
            pMe->ledLightType = FM_RADIO_LED_LIGHT_PLAYING;
        }
        else
        {
            pMe->ledLightType = FM_RADIO_LED_LIGHT_IDLE;
        }
        #ifndef FEATURE_VERSION_SKY
        #ifndef FEATURE_VERSION_EC99
		#ifndef FEATURE_VERSION_K212_20D
        if (HS_HEADSET_ON())
        #endif
		#endif
        #endif    
        {
#ifdef FEATURE_ANALOG_TV
            WarT_Fm_Set_Volume( pMe->byVolumeLevel);
#else
            fm_set_volume( pMe->byVolumeLevel,pMe->fmSpeaker);
#endif
        }
    }
#endif//#if !defined( AEE_SIMULATOR)
}
boolean get_fm_play_state(void)
{
    //Note:if g_m_fm_is_on = 0 ,FM not use or in background;
    return g_m_fm_is_on;
}

#if FEATURE_FMRADIO_CHANNEL_LIST_SUPPORT
/*==============================================================================
函数:
       FmRadio_AddChannel
说明:
       将指定的CHANNEL加入Channel List, 并排列在指定
       Node的后面.

参数:
       ppNode [IO]: 指向sChanListNode结构指针的指针.
       pChInfo [In]: 指向sChanInfo结构的指针.

返回值:
       TRUE: 成功
       FALSE: 失败

备注:

==============================================================================*/
boolean FmRadio_AddChanListNode( sChanInfo* pChInfo)
{
	if( pChInfo != NULL &&
	    gFmRadio.byChannelMax < MAX_FMRADIO_STORED_CHANNEL &&
	    FmRadio_IsChannelValid( pChInfo->wChannel)
	)
	
	{           
		gFmRadio.chanInfoList[gFmRadio.byChannelMax] = *pChInfo;
		gFmRadio.byChannelMax ++;

        FmRadio_SequenceChanList();
        
		return TRUE;
	}

    FmRadio_SequenceChanList();

    return FALSE;
}


//Add By zzg 2013_08_26
boolean FmRadio_SequenceChanList(void)
{
    int i, max;
    sChanInfo  tmp;

    if (gFmRadio.byChannelMax < 2)
    {
        return FALSE;
    }
    
    for (max = gFmRadio.byChannelMax-1; max>0; max--)
    {
        for (i=0; i<max; i++)
        {       
            if (gFmRadio.chanInfoList[i].wChannel > gFmRadio.chanInfoList[i+1].wChannel)
            {
                tmp = gFmRadio.chanInfoList[i];
                gFmRadio.chanInfoList[i] = gFmRadio.chanInfoList[i+1];
                gFmRadio.chanInfoList[i+1] = tmp;
            }               
        }
    }
    
    return TRUE;
}
//Add End

int FmRadio_GetChannelTotal(void)
{
    return gFmRadio.byChannelMax;
}
/*==============================================================================
函数:
       FmRadio_DeleteChanListNode
说明:
       将指定的CHANNEL从Channel List删除

参数:
       pNode [In]: 指向sChanListNode结构的指针.

返回值:
       TRUE: 成功
       FALSE: 失败

备注:

==============================================================================*/
boolean FmRadio_ChanList_DeleteByIndex( CFmRadio* pMe, int index)
{

	if( pMe->byChannelMax > 0 && index >= 0 && index < pMe->byChannelMax)
	{

		int i = 0;
		int j = pMe->byChannelMax - 1;
		for( i = index; i < j; i ++)
		{
			pMe->chanInfoList[i] = pMe->chanInfoList[i + 1];
		}
		pMe->chanInfoList[pMe->byChannelMax - 1].wChannel = 0xffff;
		pMe->byChannelMax --;

		return TRUE;
	}

	return FALSE;
}


sChanInfo* FmRadio_ChanList_GetByIndex( CFmRadio* pMe, int index)
{

	if( index >= 0 && index < pMe->byChannelMax)
	{
		return &pMe->chanInfoList[index];
	}
	else
	{
		return NULL;
	}
}


void FmRadio_SaveChannelList( CFmRadio* pMe)
{

	byte byMax = pMe->byChannelMax;

	if( pMe->byChannelMax > MAX_FMRADIO_STORED_CHANNEL)
	{
		byMax = MAX_FMRADIO_STORED_CHANNEL;
	}
	#if defined(FEATURE_VERSION_HITZ181)||defined(FEATURE_VERSION_MTM)
    if(pMe->byChannelMax <=0)   //add by yangdecai 
    {
    	byMax = MAX_FMRADIO_STORED_CHANNEL/2;
    	WSTRCPY(pMe->chanInfoList[0].szName ,L"88");
        pMe->chanInfoList[0].wChannel = 5;
        WSTRCPY(pMe->chanInfoList[1].szName ,L"88.5");
        pMe->chanInfoList[1].wChannel = 10;       
        WSTRCPY(pMe->chanInfoList[2].szName ,L"89.5");
        pMe->chanInfoList[2].wChannel = 20;  
        WSTRCPY(pMe->chanInfoList[3].szName ,L"91");
        pMe->chanInfoList[3].wChannel = 35; 
        WSTRCPY(pMe->chanInfoList[4].szName ,L"91.5");
        pMe->chanInfoList[4].wChannel = 40;  
        WSTRCPY(pMe->chanInfoList[5].szName ,L"93");
        pMe->chanInfoList[5].wChannel = 55;   
        WSTRCPY(pMe->chanInfoList[6].szName ,L"94");
        pMe->chanInfoList[6].wChannel = 65;    
        WSTRCPY(pMe->chanInfoList[7].szName ,L"94.5");
        pMe->chanInfoList[7].wChannel = 70;   
        WSTRCPY(pMe->chanInfoList[8].szName ,L"95");
        pMe->chanInfoList[8].wChannel = 75;  
        WSTRCPY(pMe->chanInfoList[9].szName ,L"95.5");
        pMe->chanInfoList[9].wChannel = 80;
        WSTRCPY(pMe->chanInfoList[10].szName ,L"96");
        pMe->chanInfoList[10].wChannel = 85;  
        WSTRCPY(pMe->chanInfoList[11].szName ,L"97.5");
        pMe->chanInfoList[11].wChannel = 100;
        WSTRCPY(pMe->chanInfoList[12].szName ,L"98.5");
        pMe->chanInfoList[12].wChannel = 110;   
        WSTRCPY(pMe->chanInfoList[13].szName ,L"100");
        pMe->chanInfoList[13].wChannel = 125;   
        WSTRCPY(pMe->chanInfoList[14].szName ,L"102.5");
        pMe->chanInfoList[14].wChannel = 150;
        WSTRCPY(pMe->chanInfoList[15].szName ,L"103");
        pMe->chanInfoList[15].wChannel = 155;      
        WSTRCPY(pMe->chanInfoList[16].szName ,L"103.5");
        pMe->chanInfoList[16].wChannel = 160;
        WSTRCPY(pMe->chanInfoList[17].szName ,L"105.5");
        pMe->chanInfoList[17].wChannel = 180;
        WSTRCPY(pMe->chanInfoList[18].szName ,L"106.5");
        pMe->chanInfoList[18].wChannel = 190;
        WSTRCPY(pMe->chanInfoList[19].szName ,L"107.5");
        pMe->chanInfoList[19].wChannel = 200;
    }
	#elif defined(FEATURE_VERSION_C316)
	if(pMe->byChannelMax <=0)   //add by yangdecai 
    {

		byMax = 9;
    	WSTRCPY(pMe->chanInfoList[0].szName ,L"91.1");
        pMe->chanInfoList[0].wChannel = 36;
        WSTRCPY(pMe->chanInfoList[1].szName ,L"92.7");
        pMe->chanInfoList[1].wChannel = 52;       
        WSTRCPY(pMe->chanInfoList[2].szName ,L"93.5");
        pMe->chanInfoList[2].wChannel = 60;  
        WSTRCPY(pMe->chanInfoList[3].szName ,L"94.3");
        pMe->chanInfoList[3].wChannel = 68; 
        WSTRCPY(pMe->chanInfoList[4].szName ,L"95");
        pMe->chanInfoList[4].wChannel = 75;  
        WSTRCPY(pMe->chanInfoList[5].szName ,L"98.6");
        pMe->chanInfoList[5].wChannel = 111;   
        WSTRCPY(pMe->chanInfoList[6].szName ,L"102.4");
        pMe->chanInfoList[6].wChannel = 149;    
        WSTRCPY(pMe->chanInfoList[7].szName ,L"104");
        pMe->chanInfoList[7].wChannel = 165;   
        WSTRCPY(pMe->chanInfoList[8].szName ,L"106.4");
        pMe->chanInfoList[8].wChannel = 189;  
	}
    #endif
    
	(void) ICONFIG_SetItem(pMe->m_pConfig,
						   CFGI_FMRADIO_CHAN_TOTAL,
						   &byMax,
						   sizeof(byte));
	(void) ICONFIG_SetItem(pMe->m_pConfig,
						   CFGI_FMRADIO_CHAN_INFO,
						   (void*) pMe->chanInfoList,
						   sizeof(pMe->chanInfoList));
}

void FmRadio_ClearChanList( CFmRadio* pMe)
{
	int i = 0;
	sChanInfo info = { {0}, 0xffff};

	for( i = 0; i < MAX_FMRADIO_STORED_CHANNEL; i ++)
	{
		pMe->chanInfoList[i] = info;
	}
	pMe->byChannelMax = 0;
}


boolean FmRadio_IsChannelValid( uint16 channel)
{

	return channel <= FM_MAX_CHANNEL;
}

void FmRadio_ChanList_EnumInit( CFmRadio* pMe)
{

	pMe->enumCursor = -1;
}

void FmRadio_ChanList_EnumInit_WithLoop( CFmRadio* pMe)
{

	pMe->enumCursorWithLoop = -1;
}

boolean FmRadio_ChanList_EnumNext( CFmRadio* pMe)
{

	if( pMe->byChannelMax > 0)
	{
		pMe->enumCursor ++;
		if( pMe->enumCursor >= pMe->byChannelMax)
		{
			return FALSE;
		}
		return FmRadio_IsChannelValid( pMe->chanInfoList[pMe->enumCursor].wChannel);
	}
	else
	{
		return FALSE;
	}
}

boolean FmRadio_ChanList_EnumPrevious_WithLoop( CFmRadio* pMe)
{

	if( pMe->byChannelMax > 0)
	{
		pMe->enumCursorWithLoop --;
		if( pMe->enumCursorWithLoop < 0)
		{
			pMe->enumCursorWithLoop = pMe->byChannelMax - 1;
		}
		return FmRadio_IsChannelValid( pMe->chanInfoList[pMe->enumCursorWithLoop].wChannel);
	}
	else
	{
		return FALSE;
	}
}

boolean FmRadio_ChanList_EnumNext_WithLoop( CFmRadio* pMe)
{

	if( pMe->byChannelMax > 0)
	{
		pMe->enumCursorWithLoop ++;
		if( pMe->enumCursorWithLoop >= pMe->byChannelMax)
		{
			pMe->enumCursorWithLoop = 0;
		}
		return FmRadio_IsChannelValid( pMe->chanInfoList[pMe->enumCursorWithLoop].wChannel);
	}
	else
	{
		return FALSE;
	}
}

sChanInfo* FmRadio_ChanList_GetCurrent( CFmRadio* pMe)
{

	return FmRadio_ChanList_GetByIndex( pMe, pMe->enumCursor);
}

//Add By zzg 2013_09_02
word FmRadio_GetPlayingChannel(CFmRadio* pMe)
{
    word channel1=0, channel2=0;
    channel1 =  fm_radio_get_playing_channel();
    channel2 = (channel1*100 - LOWEST_BAND_FREQ)/CHANNEL_SPACE;  

    MSG_FATAL("***zzg FmRadio_GetPlayingChannel channel1=%d, channel2=%d***", channel1, channel1, 0);
    
    return channel2;
}
//Add End

sChanInfo* FmRadio_ChanList_GetCurrent_WithLoop( CFmRadio* pMe)
{

	return FmRadio_ChanList_GetByIndex( pMe, pMe->enumCursorWithLoop);
}


boolean FmRadio_FindChanListNode(int channel)
{

	int i = 0;

	for( i = 0; i < MAX_FMRADIO_STORED_CHANNEL; i++)
	{
		if( channel == gFmRadio.chanInfoList[i].wChannel)
		{
			return TRUE;
		}
	}


    return FALSE;

}
#endif
/*==============================================================================
函数:
       closeApp

说明:
       关闭App
参数:
       pMe [in]：

返回值:

备注:

==============================================================================*/
// bw:<< 20080808 : close app when pull out the headset
void closeApp( void *pme) //bw:
{
    CFmRadio *pMe = pme;   
#if FEATURE_FMRADIO_SUPPORT_BACKGROUND     
    pMe->runOnBackground = FALSE;
#endif
    ISHELL_CloseApplet( pMe->m_pShell, FALSE);
}
// bw:>> 20080808 : close app when pull out the headset

/*==============================================================================
函数:
       FmRadio_APPIsReadyTimer

说明:
       定时器回调函数。主要用于控制快速按键问题的处理。

参数:
       pme [in]：void *类型指针，暗指CFmRadio结构指针。

返回值:
       无。

备注:

==============================================================================*/
static void FmRadio_AppIsReadyCB(void *pUser)
{
    CFmRadio *pMe = (CFmRadio *)pUser;

    if (NULL == pMe)
    {
        return;
    }
    
    pMe->m_bAppIsReady = TRUE;
}// FmRadio_AppIsReadyCB

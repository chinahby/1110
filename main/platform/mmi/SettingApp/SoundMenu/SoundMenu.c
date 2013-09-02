/*==============================================================================
// 文件：
//        SoundMenu.c
//        版权所有(c) 2009 Anylook Tech. CO.,LTD.
//
// 文件说明：
//
// 作者：Gemsea
// 创建日期：2007-11-01
// 当前版本：Draft
==============================================================================*/

/*==============================================================================
修改历史记录：
时       间      修 改 人    问题单号  修改内容、位置及原因
-----------      ----------  --------  -----------------------------------------
2007-11-01                  初始版本
==============================================================================*/


/*==============================================================================
                                 本文件包含的外部文件
==============================================================================*/
#include "SoundMenu_priv.h"
#include "OEMHeap.h"
/*==============================================================================
                                 宏定义和常数
==============================================================================*/

/*==============================================================================
                                 类型定义
==============================================================================*/

/*==============================================================================
                                 函数声明
==============================================================================*/
static void SoundMenu_FreeRingerList(CSoundMenu *pMe);

static void SoundMenu_APPIsReadyTimer(void *pme);

/*----------------------模块相关函数声明---------------------*/

/*static*/ int  SoundMenuMod_Load(IShell   *pIShell,
    void     *ph,
    IModule **ppMod
);

int  SoundMenuMod_New(int16  nSize,
    IShell   *pIShell,
    void     *ph,
    IModule  **ppMod,
    PFNMODCREATEINST pfnMC,
    PFNFREEMODDATA pfnMF
);

static int  SoundMenuMod_CreateInstance(IModule *po,
    IShell *pIShell,
    AEECLSID ClsId,
    void **ppObj
);

static uint32 SoundMenuMod_AddRef(IModule *po);

static uint32 SoundMenuMod_Release(IModule *po);

static void SoundMenuMod_FreeResources(IModule  *po,
    IHeap  *ph,
    IFileMgr *pfm
);

/*----------------------Applet相关函数声明---------------------*/
static int SoundMenu_New(IShell *ps,
    IModule *pIModule,
    ISoundMenu **ppObj
);

static uint32  SoundMenu_AddRef(ISoundMenu *pi);

static uint32  SoundMenu_Release (ISoundMenu *pi);

static boolean SoundMenu_HandleEvent(ISoundMenu *pi,
    AEEEvent eCode,
    uint16  wParam,
    uint32 dwParam
);

static int SoundMenu_InitAppData(CSoundMenu *pMe);

static void SoundMenu_FreeAppData(CSoundMenu *pMe);

static void SoundMenu_RunFSM(CSoundMenu *pMe);

void SoundMenu_UpdateRingerListMenu(CSoundMenu *pMe, IMenuCtl *pMenu);

static void  SoundMenu_SoundList (ISoundMenu *pi);

static void  SoundMenu_Autoanswer(ISoundMenu *pi);

static void  SoundMenu_Ringer_List (ISoundMenu *pi,void*  ringer_id);

/*==============================================================================
                                 全局数据
==============================================================================*/
// 重要提示：
//        g_dwAEEStdLibEntry全局变量仅用于SDK ！
//        BREW SDK用户不能改写和直接使用本变量。它用作AEEStdLib的一个入口点，不
//        要移动下列三行代码。
#ifdef AEE_SIMULATOR
   uint32 g_dwAEEStdLibEntry;
#ifndef WIN32
   #error This applet was never intended to be used with the AEE Simulator.
#endif//WIN32
#endif

/*==============================================================================
                                 本地（静态）数据
==============================================================================*/
static SoundMenuMod gSoundMenuMod;

static const VTBL(IModule) gModFuncs =
{
    SoundMenuMod_AddRef,
    SoundMenuMod_Release,
    SoundMenuMod_CreateInstance,
    SoundMenuMod_FreeResources
};

// 只允许一个SoundMenu实例。每次创建SoundMenu Applet时，
// 返回同一结构指针给BREW层。
static CSoundMenu gSoundMenu;

static const VTBL(ISoundMenu) gSoundMenuMethods =
{
    SoundMenu_AddRef,
    SoundMenu_Release,
    SoundMenu_HandleEvent,
    SoundMenu_SoundList,
    SoundMenu_Autoanswer,
    SoundMenu_Ringer_List
};

#if defined(BREW_STATIC_APP)
static const AEEAppInfo gaiSoundMenu =
{
    AEECLSID_APP_SOUNDMENU,
    AEE_APPSSOUNDMENU_RES_FILE,
    IDS_SOUNDMENU_TITLE,
    0,
    0,
    0,
    0,
    AFLAG_HIDDEN
};
#endif

/*----------------------模块相关函数定义---------------------*/

/*==============================================================================
函数:
       SoundMenuMod_Load

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
#ifndef WIN32
#ifdef AEE_SIMULATOR
__declspec(dllexport) int AEEMod_Load(IShell   *pIShell,
                                                        void     *ph,
                                                        IModule **ppMod)
#else
/*static*/ int SoundMenuMod_Load(IShell   *pIShell,
                                                   void     *ph,
                                                   IModule **ppMod)
#endif
#else
/*static*/ int SoundMenuMod_Load(IShell   *pIShell,
                                                   void     *ph,
                                                   IModule **ppMod)
#endif//WIn32
{
    // 对于动态Applet，验证AEE STD库版本，以确保库与建立本模块时使用的库版本匹配
    // 。在AEE_SIMULATOR下不必进行本检查。
#if !defined(BREW_STATIC_APP) && !defined(AEE_SIMULATOR)
    if (GET_HELPER_VER() != AEESTDLIB_VER)
    {
        return EVERSIONNOTSUPPORT;
    }
#endif

    return SoundMenuMod_New(sizeof(SoundMenuMod),
                    pIShell,
                    ph,
                    ppMod,
                    NULL,
                    NULL);
}

/*==============================================================================
函数:
       SoundMenuMod_New

说明:
       用来初始化SoundMenu模块虚拟表（也增加引用计数）的助手函数。

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
int  SoundMenuMod_New(int16  nSize,
    IShell   *pIShell,
    void     *ph,
    IModule  **ppMod,
    PFNMODCREATEINST pfnMC,
    PFNFREEMODDATA pfnMF
)
{
    PARAM_NOT_REF(nSize)
    PARAM_NOT_REF(pfnMC)
    PARAM_NOT_REF(pfnMF)
#ifndef AEE_SIMULATOR
    PARAM_NOT_REF(ph)
#endif

    if (NULL == ppMod)
    {
        return EFAILED;
    }

    *ppMod = NULL;

    // 重要提示：:
    //        g_dwAEEStdLibEntry全局变量仅用于SDK ！
    //        BREW SDK用户不能改写和直接使用本变量。它用作AEEStdLib的一个入口点
    //        。
#ifndef WIN32
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
#else
    if (!pIShell)
    {
        return EFAILED;
    }
#endif//WIN32
    MEMSET(&gSoundMenuMod,0,sizeof(SoundMenuMod));

    INIT_VTBL(&gSoundMenuMod,IModule,gModFuncs);
    gSoundMenuMod.m_nRefs = 1;
    *ppMod = (IModule *)&gSoundMenuMod;
    return AEE_SUCCESS;
}

/*==============================================================================
函数:
       SoundMenuMod_AddRef

说明:
       SoundMenu 模块引用计数加1，并返回当前的引用数。

参数:
       po [in]：IModule接口对象指针。

返回值:
       当前对SoundMenu 模块的引用数

备注:

==============================================================================*/
static uint32 SoundMenuMod_AddRef(IModule *po)
{
    return(++((SoundMenuMod *)po)->m_nRefs);
}

/*==============================================================================
函数:
       SoundMenuMod_Release

说明:
       SoundMenu 模块引用计数减1，并返回当前的引用数。

参数:
       po [in]：IModule接口对象指针。

返回值:
       当前对SoundMenu 模块的引用数

备注:

==============================================================================*/
static uint32 SoundMenuMod_Release(IModule *po)
{
    if (((SoundMenuMod *) po)->m_nRefs == 0)
    {
        return 0;
    }

    return(--((SoundMenuMod *)po)->m_nRefs);
}

/*==============================================================================
函数:
       SoundMenuMod_CreateInstance

说明:
        通过创建(初始化)SoundMenu对象来创建模块实例。

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
static int  SoundMenuMod_CreateInstance(IModule *po,
    IShell *pIShell,
    AEECLSID ClsId,
    void **ppObj
)
{
    *ppObj = NULL;

    if (ClsId != AEECLSID_APP_SOUNDMENU)
    {
        return EFAILED;
    }

    if (SoundMenu_New(pIShell, po, (ISoundMenu**)ppObj) != SUCCESS)
    {
        return EFAILED;
    }
    return SUCCESS;
}

/*==============================================================================
函数:
       SoundMenuMod_FreeResources

说明:
        释放SoundMenu模块资源的函数。

参数:
       po ：IModule。
       ph ：IHeap接口指针。
       pfm :IFileMgr接口指针。

返回值:
       无

备注:
       不需用户去调用。

==============================================================================*/
static void SoundMenuMod_FreeResources(IModule  *po,
    IHeap  *ph,
    IFileMgr *pfm
)
{

}

/*----------------------Applet相关函数定义---------------------*/
/*==============================================================================
函数:
       SoundMenu_New

说明:
        初始化SoundMenu Applet虚拟表和内部数据变量，同时其引用计数加1。

参数:
       ps [in]：IShell接口对象指针。
       pIModule [in]：pIModule接口对象指针。
       ppObj [out]:：指向ISoundMenu对象指针的指针。

返回值:
       EFAILED：失败。
       SUCCESS：成功。

备注:
       不需用户去调用。
       关于Applet结构对象可使用动态分配内存的方式创建，本处采用的是使用静态
       变量的方式。建议对结构比较大的Applet使用静态变量的方式。对于动态分配
       内存的方式需要注意对所分配空间的释放。

==============================================================================*/
static int SoundMenu_New(IShell *ps,
    IModule *pIModule,
    ISoundMenu **ppObj
)
{
    int retVal = SUCCESS;

    if (0 == gSoundMenu.m_nRefs)
    {
        // Must initialize the object
        MEMSET(&gSoundMenu,  0, sizeof(CSoundMenu));
        INIT_VTBL(&gSoundMenu, ISoundMenu, gSoundMenuMethods);

        gSoundMenu.m_nRefs     = 0;
        gSoundMenu.m_pShell    = ps;
        gSoundMenu.m_pModule   = pIModule;

        (void) ISHELL_AddRef(ps);
        (void) IMODULE_AddRef(pIModule);

        retVal = SoundMenu_InitAppData(&gSoundMenu);
    }

    ++gSoundMenu.m_nRefs;
    *ppObj = (ISoundMenu*)&gSoundMenu;
    return retVal;
}

/*==============================================================================
函数:
       SoundMenu_AddRef

说明:
       SoundMenu Applet 引用计数加1，并返回当前的引用数。

参数:
       pi [in]：ISoundMenu接口对象指针。

返回值:
       当前对SoundMenu Applet的引用数

备注:

==============================================================================*/
static uint32  SoundMenu_AddRef(ISoundMenu *pi)
{
    register CSoundMenu *pMe = (CSoundMenu*)pi;

    ASSERT(pMe != NULL);
    ASSERT(pMe->m_nRefs > 0);

    return (++pMe->m_nRefs);
}

/*==============================================================================
函数:
       SoundMenu_Release

说明:
       释放对SoundMenu Applet的引用，引用计数减1，并返回当前的引用数。
        如果当前的引用数为零，还将释放Applet及相关资源。

参数:
       pi [in]：ISoundMenu接口对象指针。

返回值:
       当前对SoundMenu Applet的引用数

备注:

==============================================================================*/
static uint32  SoundMenu_Release (ISoundMenu *pi)
{
    register CSoundMenu *pMe = (CSoundMenu*)pi;

    ASSERT(pMe != NULL);

    if (pMe->m_nRefs == 0)
    {
        return 0;
    }

    if (--pMe->m_nRefs)
    {
        return pMe->m_nRefs;
    }

    // 释放Applet相关资源
    SoundMenu_FreeAppData(pMe);

    (void) ISHELL_Release(pMe->m_pShell);
    (void) IMODULE_Release(pMe->m_pModule);

    // 注意：pMe是静态分配空间，因此不需要释放。FREE()
    return 0;
}

/*==============================================================================
SoundMenu_SoundList

说明：
      设置菜单进入声音菜单列出的当前激活的情景模式。

参数：
       pi [in/out]：ISoundMenu 接口对象指针。

返回值：
       当前对Setting Applet的引用数

备注：:

==============================================================================*/
static void  SoundMenu_SoundList (ISoundMenu *pi)
{
   register CSoundMenu *pMe = (CSoundMenu*)pi;
   char     *args;

   args = MALLOC(4);
   *args = STARTARG_SOUNDMENU;

   pMe->m_eCurState = SOUNDMENUST_SOUNDMENU;
   (void)ISHELL_StartAppletArgs(pMe->m_pShell,
                                AEECLSID_APP_SOUNDMENU,
                                args);
   //(void) ISHELL_PostEvent(pMe->m_pShell,
   //                        AEECLSID_APP_SOUNDMENU,
   //                        EVT_USER_CLOSEAPP,
   //                        0,
   //                        0);
   if(args)
   {
      FREEIF(args);
   }
}

/*==============================================================================
SoundMenu_Autoanswer

说明：
      设置菜单进入声音菜单列出的当前激活的情景模式。

参数：
       pi [in/out]：ISoundMenu 接口对象指针。

返回值：
       当前对Setting Applet的引用数

备注：:

==============================================================================*/
static void  SoundMenu_Autoanswer (ISoundMenu *pi)
{
#if 0
   register CSoundMenu *pMe = (CSoundMenu*)pi;
   char     *args;

   args = MALLOC(4);
   *args = STARTARG_AUTOANSWERSUB;

   pMe->m_eCurState = SOUNDMENUST_AUTOANSWERSUB;
   (void)ISHELL_StartAppletArgs(pMe->m_pShell,
                                AEECLSID_APP_SOUNDMENU,
                                args);
   //(void) ISHELL_PostEvent(pMe->m_pShell,
   //                        AEECLSID_APP_SOUNDMENU,
   //                        EVT_USER_CLOSEAPP,
   //                        0,
   //                        0);
   if(args)
   {
      FREE(args);
   }
#endif
}



/*==============================================================================
函数:
       SoundMenu_InitAppData

说明:
       初始化SoundMenu Applet结构数据缺省值。

参数:
       pMe [in]：指向SoundMenu Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
       SUCCESS：初始化成功。
       EFAILED：初始化失败。

备注:

==============================================================================*/
static int SoundMenu_InitAppData(CSoundMenu *pMe)
{
    AEEDeviceInfo  di;

    if (NULL  == pMe)
    {
        return EFAILED;
    }
    ISHELL_GetDeviceInfo(pMe->m_pShell, &di);
    pMe->m_rc.x       = 0;
    pMe->m_rc.y       = 0;
    pMe->m_rc.dx      = (int16) di.cxScreen;
    pMe->m_rc.dy      = (int16) di.cyScreen;

    pMe->m_bAppIsReady = FALSE;
    pMe->m_ePreState = SOUNDMENUST_NONE;
    pMe->m_eCurState = SOUNDMENUST_INIT;
    pMe->m_pActiveDlg = NULL;
    pMe->m_eDlgRet = DLGRET_CREATE;
    pMe->m_bNotOverwriteDlgRet = FALSE;

    pMe->m_currDlgId = 0;
    pMe->m_nSubDlgId = 0;
    pMe->m_sSubDlgId = 0;
    pMe->m_fSubDlgId = 0;

    pMe->m_lastRingerPlayed = NUM_OF_MAXRINGER;
#if defined(FEATURE_WMS_APP) && !defined(FEATURE_WMSAPP_ONLYSUPPORTVMAIL)
    pMe->m_lastSmsRingPlayed = 78;      //78代表短信铃声id,在sndid.h中定义
#endif

    pMe->m_ProfileType = SET_NORMALMODE;
    pMe->m_VolType = SET_RINGER_VOL;
    //pMe->m_OtherVolType = SET_FLIP;
    pMe->m_RingerType = SET_RINGER;
    pMe->m_HintType = SET_CALLHINT ;
    pMe->m_ScenemodesubType = SET_ACTIVATE;
    pMe->m_bIsSuspend = FALSE;
    pMe->m_CurProfile = OEMNV_PROFILE_NORMALMODE;
    pMe->m_bNaturalStart = NORMAL_ENTRY;
	//MODI BY YANGDECAI 09-27
	pMe->m_slecet_id     = 0;
    if (ISHELL_CreateInstance(pMe->m_pShell,
                                 AEECLSID_ANNUNCIATOR,
                                 (void **) &pMe->m_pIAnn))
    {
        SoundMenu_FreeAppData(pMe);
        return FALSE;
    }
	MSG_FATAL("IANNUNCIATOR_SetFieldIsActiveEx::::soundmenu111:::",0,0,0);
	//IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE);
    if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                            AEECLSID_CONFIG,
                                            (void **)&pMe->m_pConfig))
    {
        SoundMenu_FreeAppData(pMe);
        return EFAILED;
    }

    if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                            AEECLSID_RINGERMGR,
                                            (void **)&pMe->m_pRingerMgr))
    {
        SoundMenu_FreeAppData(pMe);
        return EFAILED;
    }

    if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                            AEECLSID_SOUND,
                                            (void **)(&pMe->m_pISound)))
    {
        SoundMenu_FreeAppData(pMe);
        return EFAILED;
    }

    if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                            AEECLSID_ALERT,
                                            (void **)(&pMe->m_pAlert)))
    {
        SoundMenu_FreeAppData(pMe);
        return EFAILED;
    }

#ifdef FLIPSND_ZY
    if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                            AEECLSID_SOUNDPLAYER,
                                            (void **)(&pMe->m_SndPlayer)))
    {
        SoundMenu_FreeAppData(pMe);
        return EFAILED;
    }
#endif /* FLIPSND_ZY */

    if (ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_VECTOR/*AEECLSID_IVECTOR*/,
            (void**)&pMe->m_pRingerList) != SUCCESS )
    {
        SoundMenu_FreeAppData(pMe);
        return EFAILED;
    }
    else
    {
        (void) IVector_EnsureCapacity(pMe->m_pRingerList, 100);
        pMe->m_ringer_be = FALSE ;
        //SoundMenu_InitRingerList(pMe);
    }

    //pMe->m_bBTHeadSetConnected = FALSE;      //Add By zzg 2013_01_09
    
    //if(ISHELL_RegisterNotify(pMe->m_pShell, AEECLSID_APP_SOUNDMENU,
    //    AEECLSID_ALERT_NOTIFIER, NMASK_ALERT_ONOFF | NMASK_ALERT_MUTED) != SUCCESS)
    //{
    //    ASSERT_NOT_REACHABLE;
    //}
    return SUCCESS;
}

/*==============================================================================
函数:
       SoundMenu_FreeAppData

说明:
       释放SoundMenu Applet使用的相关资源。

参数:
       pMe [in]：指向SoundMenu Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
       无。

备注:

==============================================================================*/
static void SoundMenu_FreeAppData(CSoundMenu *pMe)
{
    if (NULL == pMe)
    {
        return ;
    }

    if (pMe->m_pConfig)
    {
        ICONFIG_Release(pMe->m_pConfig);
        pMe->m_pConfig = NULL;
    }

    if (pMe->m_pRingerMgr)
    {
        IRINGERMGR_Release(pMe->m_pRingerMgr);
        pMe->m_pRingerMgr = NULL;
    }

    if (pMe->m_pAlert != NULL)
    {
        IALERT_StopRingerAlert(pMe->m_pAlert);
        IALERT_Release(pMe->m_pAlert);
        pMe->m_pAlert = NULL;
    }

    if (pMe->m_pISound != NULL)
    {
        ISOUND_Release(pMe->m_pISound);
        pMe->m_pISound = NULL;
    }

#ifdef FLIPSND_ZY
    if (pMe->m_SndPlayer != NULL)
    {
        ISOUNDPLAYER_Stop(pMe->m_SndPlayer);
        ISOUNDPLAYER_Release(pMe->m_SndPlayer);
        pMe->m_SndPlayer = NULL;
    }
#endif /* FLIPSND_ZY */

    //释放链表接口和所有相关数据
    if (NULL != pMe->m_pRingerList)
    {
        SoundMenu_FreeRingerList(pMe);
        (void) IVector_Release(pMe->m_pRingerList);
        pMe->m_pRingerList=NULL;
    }

    if (pMe->m_pIAnn)
    {
        //IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,TRUE);//恢复信号栏原始状态
        IANNUNCIATOR_Release(pMe->m_pIAnn);
        pMe->m_pIAnn= NULL;
    }
}

/*==============================================================================
函数:
       SoundMenu_RunFSM

说明:
       SoundMenu Applet有限状态机引擎。

参数:
       pMe [in]：指向SoundMenu Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
       无。

备注:

==============================================================================*/
static void SoundMenu_RunFSM(CSoundMenu *pMe)
{
    NextFSMAction nextFSMAction = NFSMACTION_WAIT;

    for ( ; ; )
    {
        nextFSMAction = SoundMenu_ProcessState(pMe);

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
       SoundMenu_HandleEvent

说明:
       SoundMenu  Applet事件处理主函数。所有属于本Applet的事件，
       都将经由该函数处理。

参数:
       pi [in]：指向ISoundMenu接口t对象的指针。
       eCode：BREW事件代码。
       wParam：与事件相关联的数据。
       dwParam [in]：与事件相关联的数据。

返回值:
       TRUE：传入事件被处理。
       FALSE：传入事件没被处理。

备注:

==============================================================================*/
static boolean SoundMenu_HandleEvent(ISoundMenu *pi,
    AEEEvent eCode,
    uint16  wParam,
    uint32 dwParam
)
{
    CSoundMenu *pMe = (CSoundMenu*)pi;
    AEEAppStart *as;
    //SOUND_ERR("%x,%x,%x,SoundMenu_HandleEvent",eCode,wParam,dwParam);
    switch (eCode)
    {
        case EVT_APP_START:
            if(pMe->m_pIAnn != NULL)
            {
                IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE);
            }
            pMe->m_bAppIsReady = FALSE;
            ASSERT(dwParam != 0);
            as = (AEEAppStart*)dwParam;
            if (NULL != pMe->m_pDisplay)
            {
                (void) IDISPLAY_Release(pMe->m_pDisplay);
                pMe->m_pDisplay = NULL;
            }
            pMe->m_pDisplay = as->pDisplay;
            (void) IDISPLAY_AddRef(pMe->m_pDisplay);
            pMe->m_rc = as->rc;
            {
                AEEDeviceInfo di; 
                ISHELL_GetDeviceInfo(pMe->m_pShell,&di);
                pMe->m_rc.dy = di.cyScreen;
            }
            //end added
            pMe->m_bSuspending = FALSE;
             (void) ICONFIG_GetItem(pMe->m_pConfig,
                              CFGI_PROFILE_CUR_NUMBER,
                              &pMe->m_active,
                              sizeof(pMe->m_active));
             pMe->m_CurProfile = pMe->m_active;
            // 开始SOUNDMENUST_SOUNDMENU状态机
#ifndef WIN32
            if(STRNCMP(as->pszArgs,"R",1) == 0)
            {
                char temp[16] = {0};
                STRLCPY(temp,as->pszArgs +1,sizeof(temp));
                pMe->m_enter_address = (AECHAR*)ATOI(temp);
                SOUND_ERR("%S %x SoundMenu_Ringer_List2",pMe->m_enter_address,&pMe->m_enter_address,0);
                pMe->m_bNaturalStart = ENTER_RINGER_LIST;
                pMe->m_RingerType =  SET_ANOTHER;
                {
                    if((pMe->m_enter_address[0] == (AECHAR)'\0'))
                    {
                        pMe->m_lastRingerPlayed = DEFAULT_MENU;
                    }
                    else
                    {
                        IRingerMgr      *RingerMgr = NULL;
                        int ringerId = AEE_RINGER_ID_NONE;
                        char        filename[MAX_FILE_NAME]={0};
                        
                        if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                                            AEECLSID_RINGERMGR,
                                                            (void **)&RingerMgr))
                        {
                            return FALSE;
                        }
                        
                        WSTRTOSTR(pMe->m_enter_address, filename, MAX_FILE_NAME);
                        
                        // 为了支持PEK 测试，电话本的RINGTONE 字段只能保存字符串类型，这里根据路径名查找是否有MID存在
                        ringerId = IRINGERMGR_GetRingerID(RingerMgr, filename);
                        if(AEE_RINGER_ID_NONE == ringerId)
                        {
                            // 是mp3
                            pMe->m_lastRingerPlayed = DOWNLOAD_MENU;
                        }
                        else
                        {
                            // mid
                            pMe->m_lastRingerPlayed = ringerId;
                        }

                    	if ( RingerMgr )
                    	{
							IRINGERMGR_Release(RingerMgr);
							RingerMgr = NULL;
                    	}
                    }

                }
                
                SOUND_ERR("tagnwq soundhandle first:%d",pMe->m_lastRingerPlayed,0,0);
                //pMe->m_lastRingerPlayed = (uint16) (pMe->m_enter_addr)->midID;
                MOVE_TO_STATE(SOUNDMENUST_RINGER);
            }

            if(*as->pszArgs == STARTARG_SOUNDMENU)
            {
                pMe->m_bNaturalStart = SOUNDLIST_ENTRY;
                MOVE_TO_STATE(SOUNDMENUST_SOUNDMENU);
            }
#endif//WIN32
            //开始SOUNDMENUST_AUTOANSWERSUB状态机
            //if(*as->pszArgs == STARTARG_AUTOANSWERSUB)
            //{
            //   pMe->m_bNaturalStart = AUTOANSWER_ENTRY;
            //   MOVE_TO_STATE(SOUNDMENUST_AUTOANSWERSUB);
            //}

            SoundMenu_RunFSM(pMe);
            return TRUE;

        case EVT_APP_STOP:
            pMe->m_bNaturalStart = NORMAL_ENTRY;
            if (pMe->m_pDisplay != NULL)
            {
                (void) IDISPLAY_Release(pMe->m_pDisplay);
                pMe->m_pDisplay = NULL;
            }
            pMe->m_bSuspending = TRUE;
            ISHELL_CancelTimer(pMe->m_pShell, NULL, pMe);
            ISHELL_RegisterNotify(pMe->m_pShell, AEECLSID_APP_SOUNDMENU,AEECLSID_ALERT_NOTIFIER, 0);
            return TRUE;

        case EVT_APP_SUSPEND:
            pMe->m_bSuspending = TRUE;
            if(pMe->m_pActiveDlgID == IDD_RINGER_MENU)
            {
                //解决挂起后返回时的焦点问题
                pMe->m_bIsSuspend = TRUE;
            }
            ISHELL_CancelTimer(pMe->m_pShell, NULL, pMe);
            return TRUE;

        case EVT_APP_RESUME:
            ASSERT(dwParam != 0);
            as = (AEEAppStart*)dwParam;
            pMe->m_bSuspending = FALSE;

            if (pMe->m_pDisplay != NULL)
            {
                (void) IDISPLAY_Release(pMe->m_pDisplay);
                pMe->m_pDisplay = NULL;
            }
            pMe->m_pDisplay = as->pDisplay;
            (void) IDISPLAY_AddRef(pMe->m_pDisplay);
            pMe->m_rc = as->rc;
            pMe->m_rc.y = pMe->m_rc.y - SOUND_MENU_SOFK_HIGHT;
            SoundMenu_FreeRingerList(pMe);
            pMe->m_eMakeListMode = MAKELIST_INIT;
            SoundMenu_InitRingerList(pMe);
            SoundMenu_RunFSM(pMe);
            return TRUE;

        case EVT_NOTIFY:
        {
            AEENotify   *Notify_msg = (AEENotify *) dwParam;
            if(Notify_msg->cls == AEECLSID_ALERT_NOTIFIER)
            {
                switch(wParam)
                {
                    case NMASK_ALERT_ONOFF:
                        return TRUE;

                    case NMASK_ALERT_MUTED:
                        return TRUE;

                    default:
                        return FALSE;
                }
            }

        }

        case EVT_DIALOG_INIT:
            pMe->m_bAppIsReady = FALSE;
            pMe->m_pActiveDlg = (IDialog*)dwParam;
            pMe->m_pActiveDlgID = wParam;

            return SoundMenu_RouteDialogEvent(pMe,eCode,wParam,dwParam);

        case EVT_DIALOG_START:
            return SoundMenu_RouteDialogEvent(pMe,eCode,wParam,dwParam);

        case EVT_USER_REDRAW:
            (void) SoundMenu_RouteDialogEvent(pMe,eCode,wParam,dwParam);
            (void)ISHELL_SetTimer ( pMe->m_pShell,
                            APPISREADY_TIMER,
                            SoundMenu_APPIsReadyTimer,
                            pMe);
            return TRUE;

        case EVT_APPISREADY:
            pMe->m_bAppIsReady = TRUE;
            return TRUE;

        case EVT_KEY_PRESS:
        case EVT_KEY_RELEASE:
        case EVT_KEY:
        case EVT_COMMAND:
			#if !defined(FEATURE_VERSION_K212)
            if (!pMe->m_bAppIsReady)
            {
                return TRUE;
            }
			#endif
            return SoundMenu_RouteDialogEvent(pMe,eCode,wParam,dwParam);

        case EVT_DIALOG_END:
            if (wParam == 0)
            {
                return TRUE;
            }

            pMe->m_bAppIsReady = FALSE;
            (void) SoundMenu_RouteDialogEvent(pMe,eCode,wParam,dwParam);
            pMe->m_pActiveDlg = NULL;

            // Applet被挂起，不跑状态机，等待Applet返回。（注意：EVT_APP_SUSPEND
            // 事件在EVT_DIALOG_END事件前发出。
            if (pMe->m_bSuspending == FALSE)
            {
                // 开始SoundMenu状态机
                SoundMenu_RunFSM(pMe);
            }
            return TRUE;
#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
		case EVT_PEN_UP:
		//case EVT_PEN_DOWN:
			{
				return SoundMenu_RouteDialogEvent(pMe,eCode,wParam,dwParam);
			}

		case EVT_USER:
			{
				if((wParam == AVK_SELECT) || (wParam == AVK_INFO))
				{
					if(dwParam != 0)
					{
						eCode = EVT_COMMAND;
						wParam = dwParam;
						dwParam = 0;
					}
					else
					{
						eCode = EVT_KEY;
					}
				}
				else if(wParam == AVK_CLR)
				{
					eCode = EVT_KEY;
				}
				return SoundMenu_RouteDialogEvent(pMe,eCode,wParam,dwParam);
			}
#endif//FEATURE_LCD_TOUCH_ENABLE
        default:
            // 将接收到的事件路由至当前活动的对话框事件处理函数。
            return SoundMenu_RouteDialogEvent(pMe,eCode,wParam,dwParam);
    }
}

/*==============================================================================
函数:
       SoundMenu_APPIsReadyTimer

说明:
       定时器回调函数。主要用于控制快速按键问题的处理。

参数:
       pme [in]：void *类型指针，暗指CSoundMenu结构指针。

返回值:
       无。

备注:

==============================================================================*/
static void SoundMenu_APPIsReadyTimer(void *pme)
{
    CSoundMenu *pMe = (CSoundMenu *)pme;

    if (NULL == pMe)
    {
        return;
    }

    (void) ISHELL_PostEvent( pMe->m_pShell,
                            AEECLSID_APP_SOUNDMENU,
                            EVT_APPISREADY,
                            0,
                            0);
}

/*==============================================================================
函数：
       SoundMenu_InitRingerList

说明：
       函数用于初始铃声链表。

参数：
       pMe [in]：指向SoundMenu Applet对象结构的指针。
                           该结构包含小程序的特定信息。

返回值：
       无。

备注：:

==============================================================================*/
void SoundMenu_InitRingerList(CSoundMenu *pMe)
{
    RingerMenuItemInfo   *pItemInfo;
    AEERingerInfo  ri;

    SOUND_ERR("Init ringer list begin!",0,0,0);
    if (pMe == NULL)
    {
        return;
    }
    if(pMe->m_ringer_be)
    {
        return;
    }

    IVector_RemoveAllElements(pMe->m_pRingerList);
#if !defined( FEATURE_RECORDER)
    pMe->m_ringer_be = TRUE ;
#endif

    if ( IRINGERMGR_EnumRingerInit(pMe->m_pRingerMgr) != SUCCESS)
    {
        return;
    }
    while (IRINGERMGR_EnumNextRinger(pMe->m_pRingerMgr, &ri))
    {
        if (ri.id > NUM_OF_MAXRINGER)
        {
            break;  // only support NUM_OF_MAXRINGER-1 ringers
        }
        else
        {
            pItemInfo = NULL;
            pItemInfo = (RingerMenuItemInfo *)sys_malloc(sizeof(RingerMenuItemInfo));
            if (NULL == pItemInfo)
            {
                return;
            }
            else
            {
#if defined( FEATURE_RECORDER)
			   if( STRBEGINS( RECORDER_MEMO_RINGTONE_PATH_NAME_PREFIX, ri.szFile))
			   {
			   	
               if( ( pMe->m_RingerType == SET_RINGER    ||
                     pMe->m_RingerType == SET_ALARMRING ||
                     pMe->m_RingerType == SET_SMSTONE   ||
                     pMe->m_RingerType == SET_ANOTHER
                   )                   
               )
               {
                   if( ( pMe->m_RingerType == SET_RINGER &&
                         STRBEGINS( RECORDER_MEMO_CALL_RINGTONE_PATH_NAME + 4, ri.szFile)
                       )
                       ||
                       ( pMe->m_RingerType == SET_ALARMRING &&
                         STRBEGINS( RECORDER_MEMO_ALARM_RINGTONE_PATH_NAME + 4, ri.szFile)
                       )
                       ||
                       ( pMe->m_RingerType == SET_SMSTONE &&
                         STRBEGINS( RECORDER_MEMO_SMS_RINGTONE_PATH_NAME + 4, ri.szFile)
                       )
                       ||
                       ( pMe->m_RingerType == SET_ANOTHER &&
                         STRBEGINS( RECORDER_MEMO_SMS_RINGTONE_PATH_NAME + 4, ri.szFile)
                       )
                   )
                   {
                       AECHAR memo[64] = {0};

					   #ifdef FEATURE_VERSION_C337
					   if( ISHELL_LoadResString( pMe->m_pShell,
                                             APP_RECORDER_RES_FILE,
                                             IDS_RECORDER_MEMO_RINGER_NAME_C337,
                                             memo,
                                             sizeof( memo)
                                         ) > 0
                       )
                       #elif defined FEATURE_VERSION_W317A
					   if( ISHELL_LoadResString( pMe->m_pShell,
                                             APP_RECORDER_RES_FILE,
                                             IDS_RECORDER,
                                             memo,
                                             sizeof( memo)
                                         ) > 0
                       )
					   #else
					   if( ISHELL_LoadResString( pMe->m_pShell,
                                             APP_RECORDER_RES_FILE,
                                             IDS_RECORDER_MEMO_RINGER_NAME,
                                             memo,
                                             sizeof( memo)
                                         ) > 0
                       )
					   #endif                       
                       {
                           (void)WSTRCPY( ri.szName, memo);
                       }
                       else
                       {
                       		#ifdef FEATURE_VERSION_C337
							(void)WSTRCPY( ri.szName, L"Record");
							#else
                           (void)WSTRCPY( ri.szName, L"Voice memo");
							#endif
                       }

                   }
                   else
                   {
					   FREEIF( pItemInfo);
                       continue;
                   }
               }
			   else
			   {
					FREEIF( pItemInfo);
                    continue;
			   }
			   }
#endif

                pItemInfo->id = ri.id;
                (void)WSTRCPY(pItemInfo->szName,  ri.szName);
                if ( IVector_AddElement(pMe->m_pRingerList, pItemInfo) != SUCCESS)
                {
                    FREEIF(pItemInfo);
                    return;
                }
            }
        }
    }

    SOUND_ERR("Init ringer list end!",0,0,0);
}

/*==============================================================================
函数：
       SoundMenu_FreeRingerList

说明：
       函数用于释放动态为铃声链表项分配的内存。

参数：
       pMe [in]：指向SoundMenu Applet对象结构的指针。
                           该结构包含小程序的特定信息。

返回值：
       无。

备注：:

==============================================================================*/
static void SoundMenu_FreeRingerList(CSoundMenu *pMe)
{
    RingerMenuItemInfo   *pItemInfo = NULL;
    uint32       i, dwCount;

    SOUND_ERR("Free ringer list begin!",0,0,0);
    if (pMe == NULL)
    {
        return;
    }

    if (pMe->m_pRingerList == NULL)
    {
        return;
    }

    dwCount = IVector_Size(pMe->m_pRingerList);

    // free the data first
    for (i = 0; i < dwCount; i++)
    {
        pItemInfo = (RingerMenuItemInfo *)IVector_ElementAt(pMe->m_pRingerList, i);
        if (NULL != pItemInfo)
        {
            sys_free(pItemInfo);
            pItemInfo = NULL;
        }
    }

    IVector_RemoveAllElements(pMe->m_pRingerList);
    pMe->m_ringer_be = FALSE ;
    SOUND_ERR("Free ringer list end!",0,0,0);
}

/*==============================================================================
函数：
       SoundMenu_UpdateRingerListMenu

说明：
       更新铃声列表菜单。

参数：
       pMe [in]：指向SoundMenu Applet对象结构的指针。
                           该结构包含小程序的特定信息。
       pMenu [in/out]：IMenuCtl接口指针。

返回值：
        无

备注：

==============================================================================*/
void SoundMenu_UpdateRingerListMenu(CSoundMenu *pMe, IMenuCtl *pMenu)
{
    RingerMenuItemInfo  *pListItem = NULL;
    int16     wItemCount;
    int16    wBeginPos, wEndPos;
    int16    wTotalPages;
    CtlAddItem       mai;
    uint32 dwIndex;

    if ((NULL == pMe) || (NULL == pMenu))
    {
        return;
    }

    if (pMe->m_pRingerList == NULL)
    {
        return;
    }
    // 取链表项数
    wItemCount = IVector_Size(pMe->m_pRingerList);

    // 考虑"下载铃声"菜单项
    wItemCount++;

    SOUND_ERR("wItemCount = %d",wItemCount,0,0);

    // 计算需要的总页数
    wTotalPages = wItemCount / MAXITEMS_ONEPAGE;
    if ((wItemCount % MAXITEMS_ONEPAGE) > 1)
    {
        wTotalPages++;
    }
    SOUND_ERR("wTotalPages = %d",wTotalPages,0,0);

    // 铃声总数不大于一页显示的菜单项数时，不考虑下两种要求产生菜单列表的模式
    if (((pMe->m_eMakeListMode == MAKELIST_NEXTPAGE) ||
         (pMe->m_eMakeListMode == MAKELIST_PREPAGE)) &&
        (wTotalPages <= 1))
    {
        int nCount;
        uint16 wItemID;

        nCount = IMENUCTL_GetItemCount(pMenu);

        if (nCount > 1)
        {
            if (pMe->m_eMakeListMode == MAKELIST_PREPAGE)
            {
                if (IMENUCTL_GetItemID(pMenu, 0) !=
                    IMENUCTL_GetSel(pMenu))
                {   // 考虑按左方向键
                    return;
                }
                wItemID = IMENUCTL_GetItemID(pMenu, nCount-1);
            }
            else
            {
                if (IMENUCTL_GetItemID(pMenu, nCount-1) !=
                    IMENUCTL_GetSel(pMenu))
                {   // 考虑按右方向键
                    return;
                }
                wItemID = IMENUCTL_GetItemID(pMenu, 0);
            }

            // 将当前选中项置为末尾，体现移动菜单项的连贯性
            IMENUCTL_SetSel(pMenu, wItemID);

            pMe->m_lastRingerPlayed = wItemID;
            (void)IMENUCTL_Redraw(pMenu);
        }
        return;
    }
    SOUND_ERR("pMe->m_eMakeListMode = %d",pMe->m_eMakeListMode,0,0);
    // 确定建立菜单列表时，对应消息列表的起点位置
    switch (pMe->m_eMakeListMode)
    {
        case MAKELIST_INIT:
            // 确定设定铃声所在的页
            {
                uint32   dwCurIndex = IVector_Size(pMe->m_pRingerList);

                wBeginPos = 0;
                wEndPos = MAXITEMS_ONEPAGE - 1;
                pMe->m_wCurPageNum = 0;

                for (dwIndex = 0; dwIndex < dwCurIndex; dwIndex++)
                {
                    pListItem = (RingerMenuItemInfo *)IVector_ElementAt(pMe->m_pRingerList, dwIndex);

                    if (NULL == pListItem)
                    {
                        break;
                    }

                    if (pListItem->id == pMe->m_RingID[pMe->m_CurProfile])
                    {
                        dwCurIndex = dwIndex;

                        pMe->m_wCurPageNum = dwCurIndex / MAXITEMS_ONEPAGE;

                        wBeginPos = pMe->m_wCurPageNum * MAXITEMS_ONEPAGE;
                        wEndPos = wBeginPos + MAXITEMS_ONEPAGE - 1;
                        break;
                    }
                }
            }
            break;

        case MAKELIST_NEXTPAGE:
            if ((pMe->m_wCurPageNum + 1) >= wTotalPages)
            {   // 已到列表末尾时要求换到下一页, 从链表头部开始
                wBeginPos = 0;
                wEndPos = MAXITEMS_ONEPAGE - 1;
                pMe->m_wCurPageNum = 0;
            }
            else
            {
                pMe->m_wCurPageNum++;
                wBeginPos = MAXITEMS_ONEPAGE * pMe->m_wCurPageNum;
                wEndPos = wBeginPos + MAXITEMS_ONEPAGE - 1;
            }
            break;

        case MAKELIST_PREPAGE:
            if (pMe->m_wCurPageNum == 0)
            {
                // 跳至最后一页显示
                pMe->m_wCurPageNum = wTotalPages - 1;
                wBeginPos = MAXITEMS_ONEPAGE * pMe->m_wCurPageNum;
                wEndPos = wBeginPos + MAXITEMS_ONEPAGE - 1;

            }
            else
            {
                pMe->m_wCurPageNum--;
                wBeginPos = MAXITEMS_ONEPAGE * pMe->m_wCurPageNum;
                wEndPos = wBeginPos + MAXITEMS_ONEPAGE - 1;

            }
            break;

        case MAKELIST_RESUMECURPAGE:
            // 因删除操作导致 m_wCurPageNum 取值不再适用
            if (pMe->m_wCurPageNum >= wTotalPages)
            {
                // 显示最后一页
                pMe->m_wCurPageNum = wTotalPages - 1;
                wBeginPos = MAXITEMS_ONEPAGE * pMe->m_wCurPageNum;
                wEndPos = wBeginPos + MAXITEMS_ONEPAGE - 1;

            }
            else
            {
                wBeginPos = MAXITEMS_ONEPAGE * pMe->m_wCurPageNum;
                wEndPos = wBeginPos + MAXITEMS_ONEPAGE - 1;
            }
            break;

        default:
            return;
    }

    // 先清除旧有消息列表
    (void)IMENUCTL_DeleteAll(pMenu);

    if(pMe->m_RingerType == SET_ANOTHER)
    {
        IMENUCTL_AddItem(pMenu, AEE_APPSSOUNDMENU_RES_FILE, IDS_DEFAULT, DEFAULT_MENU, NULL, 0); 
        IMENUCTL_AddItem(pMenu, AEE_APPSSOUNDMENU_RES_FILE, IDS_GALLERY_PROFILE, DOWNLOAD_MENU, NULL, 0);
    }

    if(pMe->m_RingerType == SET_ALARMRING || pMe->m_RingerType == SET_RINGER)
    {
        IMENUCTL_AddItem(pMenu, AEE_APPSSOUNDMENU_RES_FILE, IDS_GALLERY_PROFILE, DOWNLOAD_MENU, NULL, 0);
    }

    // 建立菜单项列表
    MEMSET(&mai, 0, sizeof(mai));
    //mai.pszResImage = AEEEXT_RES_FILE;
    mai.pszResText = NULL;
    mai.pImage = NULL;
    mai.wFont = AEE_FONT_NORMAL;
    mai.dwData = 0;
    for (dwIndex = wBeginPos; dwIndex <= wEndPos; dwIndex++)
    {
        if ((dwIndex + 1) >= wItemCount)
        {   // "下载铃声"菜单项

            if (pListItem->id == pMe->m_RingID[pMe->m_CurProfile])
            {
                //mai.wImage = IDB_SELECT_ICON;
                DBGPRINTF("pMe->m_lastRingerPlayed:%d",pListItem->id);
                pMe->m_lastRingerPlayed = pListItem->id;
            }
            break;
        }
        else
        {
            pListItem = (RingerMenuItemInfo *)IVector_ElementAt(pMe->m_pRingerList, dwIndex);

            if (NULL != pListItem)
            {
                  //mai.pText = pListItem->szName;
                mai.wItemID = pListItem->id;
                mai.pText = pListItem->szName;
                // 添加带图标菜单项到菜单
                (void)IMENUCTL_AddItemEx(pMenu, &mai);
                //ERR("dwIndex = %d",dwIndex , 0, 0);
            }
        }
    }

    if ((pMe->m_eMakeListMode == MAKELIST_NEXTPAGE) ||
        (pMe->m_eMakeListMode == MAKELIST_PREPAGE))
    {
        if (pMe->m_eMakeListMode == MAKELIST_PREPAGE)
        {
            // 将当前选中项置为末尾，体现移动菜单项的连贯性
            pMe->m_lastRingerPlayed = mai.wItemID;
        }

        if (pMe->m_eMakeListMode == MAKELIST_NEXTPAGE)
        {
            uint16 wItemID;
            wItemID = IMENUCTL_GetItemID(pMenu, 0);
            pMe->m_lastRingerPlayed = wItemID;
        }
    }
    pMe->m_fSubDlgId = pMe->m_lastRingerPlayed;
    DBGPRINTF("IMENUCTL_SetSel:%d",pMe->m_lastRingerPlayed);
    IMENUCTL_SetSel(pMenu, pMe->m_lastRingerPlayed);
    (void)IMENUCTL_Redraw(pMenu);
    SOUND_ERR("UpdateRingerListMenu end!",0,0,0);
}

static void  SoundMenu_Ringer_List (ISoundMenu *pi,void*  ringer_id)
{
   register CSoundMenu *pMe = (CSoundMenu*)pi;
   char     args[16] = {0};
   SPRINTF(args, "R %u", ringer_id);
   ISHELL_StartAppletArgs(pMe->m_pShell,
                                AEECLSID_APP_SOUNDMENU,
                                args);
}

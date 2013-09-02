/*==============================================================================
// 文件：
//        SettingMenu.c
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
==============================================================================*/


/*==============================================================================
                                 本文件包含的外部文件
==============================================================================*/
#include "SettingMenu_priv.h"

/*==============================================================================
                                 宏定义和常数
==============================================================================*/

/*==============================================================================
                                 类型定义
==============================================================================*/

/*==============================================================================
                                 函数声明
==============================================================================*/
static void SettingMenu_APPIsReadyTimer(void *pme);

/*----------------------模块相关函数声明---------------------*/

/*static*/ int  SettingMenuMod_Load(IShell   *pIShell,
    void     *ph,
    IModule **ppMod
);

int  SettingMenuMod_New(int16  nSize,
    IShell   *pIShell,
    void     *ph,
    IModule  **ppMod,
    PFNMODCREATEINST pfnMC,
    PFNFREEMODDATA pfnMF
);

static int  SettingMenuMod_CreateInstance(IModule *po,
    IShell *pIShell,
    AEECLSID ClsId,
    void **ppObj
);

static uint32 SettingMenuMod_AddRef(IModule *po);

static uint32 SettingMenuMod_Release(IModule *po);

static void SettingMenuMod_FreeResources(IModule  *po,
    IHeap  *ph,
    IFileMgr *pfm
);

/*----------------------Applet相关函数声明---------------------*/
static int SettingMenu_New(IShell *ps,
    IModule *pIModule,
    ISettingMenu **ppObj
);

static uint32  SettingMenu_AddRef(ISettingMenu *pi);

static uint32  SettingMenu_Release (ISettingMenu *pi);

static boolean SettingMenu_HandleEvent(ISettingMenu *pi,
    AEEEvent eCode,
    uint16  wParam,
    uint32 dwParam
);

static int SettingMenu_InitAppData(CSettingMenu *pMe);

static void SettingMenu_FreeAppData(CSettingMenu *pMe);

static void SettingMenu_RunFSM(CSettingMenu *pMe);
#ifdef FEATRUE_SET_IP_NUMBER
static void SettingMenu_Get_Ip_Number(CSettingMenu *pMe);
static void SettingMenu_Free_And_Save_Ip_Number(CSettingMenu *pMe);
#endif /*FEATRUE_SET_IP_NUMBER*/

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
#endif
#endif

/*==============================================================================
                                 本地（静态）数据
==============================================================================*/
static SettingMenuMod gSettingMenuMod;

static const VTBL(IModule) gModFuncs =
{
    SettingMenuMod_AddRef,
    SettingMenuMod_Release,
    SettingMenuMod_CreateInstance,
    SettingMenuMod_FreeResources
};

// 只允许一个SettingMenu实例。每次创建SettingMenu Applet时，
// 返回同一结构指针给BREW层。
static CSettingMenu gSettingMenu;

static const VTBL(ISettingMenu) gSettingMenuMethods =
{
    SettingMenu_AddRef,
    SettingMenu_Release,
    SettingMenu_HandleEvent
};

/*----------------------模块相关函数定义---------------------*/

/*==============================================================================
函数:
       SettingMenuMod_Load

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
/*static*/ int SettingMenuMod_Load(IShell   *pIShell,
                                                   void     *ph,
                                                   IModule **ppMod)
#endif
#else
/*static*/ int SettingMenuMod_Load(IShell   *pIShell,
                                                   void     *ph,
                                                   IModule **ppMod)
#endif//WIN32
{
    // 对于动态Applet，验证AEE STD库版本，以确保库与建立本模块时使用的库版本匹配
    // 。在AEE_SIMULATOR下不必进行本检查。
#if !defined(BREW_STATIC_APP) && !defined(AEE_SIMULATOR)
    if (GET_HELPER_VER() != AEESTDLIB_VER)
    {
        return EVERSIONNOTSUPPORT;
    }
#endif

    return SettingMenuMod_New(sizeof(SettingMenuMod),
                    pIShell,
                    ph,
                    ppMod,
                    NULL,
                    NULL);
}

/*==============================================================================
函数:
       SettingMenuMod_New

说明:
       用来初始化SettingMenu模块虚拟表（也增加引用计数）的助手函数。

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
int  SettingMenuMod_New(int16  nSize,
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

    MEMSET(&gSettingMenuMod,0,sizeof(SettingMenuMod));

    INIT_VTBL(&gSettingMenuMod,IModule,gModFuncs);	
	
    gSettingMenuMod.m_nRefs = 1;
    *ppMod = (IModule *)&gSettingMenuMod;
    return AEE_SUCCESS;
}

/*==============================================================================
函数:
       SettingMenuMod_AddRef

说明:
       SettingMenu 模块引用计数加1，并返回当前的引用数。

参数:
       po [in]：IModule接口对象指针。

返回值:
       当前对SettingMenu 模块的引用数

备注:

==============================================================================*/
static uint32 SettingMenuMod_AddRef(IModule *po)
{	
    return(++((SettingMenuMod *)po)->m_nRefs);
}

/*==============================================================================
函数:
       SettingMenuMod_Release

说明:
       SettingMenu 模块引用计数减1，并返回当前的引用数。

参数:
       po [in]：IModule接口对象指针。

返回值:
       当前对SettingMenu 模块的引用数

备注:

==============================================================================*/
static uint32 SettingMenuMod_Release(IModule *po)
{
    if (((SettingMenuMod *) po)->m_nRefs == 0)
    {
        return 0;
    }

    return(--((SettingMenuMod *)po)->m_nRefs);
}

/*==============================================================================
函数:
       SettingMenuMod_CreateInstance

说明:
        通过创建(初始化)SettingMenu对象来创建模块实例。

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
static int  SettingMenuMod_CreateInstance(IModule *po,
    IShell *pIShell,
    AEECLSID ClsId,
    void **ppObj
)
{
    *ppObj = NULL;

    if (ClsId != AEECLSID_APP_SETTINGMENU)
    {
        return EFAILED;
    }

    if (SettingMenu_New(pIShell, po, (ISettingMenu**)ppObj) != SUCCESS)
    {
        return EFAILED;
    }
    return SUCCESS;
}

/*==============================================================================
函数:
       SettingMenuMod_FreeResources

说明:
        释放SettingMenu模块资源的函数。

参数:
       po ：IModule。
       ph ：IHeap接口指针。
       pfm :IFileMgr接口指针。

返回值:
       无

备注:
       不需用户去调用。

==============================================================================*/
static void SettingMenuMod_FreeResources(IModule  *po,
    IHeap  *ph,
    IFileMgr *pfm
)
{
    PARAM_NOT_REF(po)
    PARAM_NOT_REF(ph)
    PARAM_NOT_REF(pfm)
}

/*----------------------Applet相关函数定义---------------------*/
/*==============================================================================
函数:
       SettingMenu_New

说明:
        初始化SettingMenu Applet虚拟表和内部数据变量，同时其引用计数加1。

参数:
       ps [in]：IShell接口对象指针。
       pIModule [in]：pIModule接口对象指针。
       ppObj [out]:：指向ISettingMenu对象指针的指针。

返回值:
       EFAILED：失败。
       SUCCESS：成功。

备注:
       不需用户去调用。
       关于Applet结构对象可使用动态分配内存的方式创建，本处采用的是使用静态
       变量的方式。建议对结构比较大的Applet使用静态变量的方式。对于动态分配
       内存的方式需要注意对所分配空间的释放。

==============================================================================*/
static int SettingMenu_New(IShell *ps,
    IModule *pIModule,
    ISettingMenu **ppObj
)
{
    int retVal = SUCCESS;
    if (0 == gSettingMenu.m_nRefs)
    {
        // Must initialize the object
        MEMSET(&gSettingMenu,  0, sizeof(CSettingMenu));
        INIT_VTBL(&gSettingMenu, ISettingMenu, gSettingMenuMethods);

        gSettingMenu.m_nRefs     = 0;
        gSettingMenu.m_pShell    = ps;
        gSettingMenu.m_pModule   = pIModule;

        (void) ISHELL_AddRef(ps);
        (void) IMODULE_AddRef(pIModule);

        retVal = SettingMenu_InitAppData(&gSettingMenu);
    }

    ++gSettingMenu.m_nRefs;
    *ppObj = (ISettingMenu*)&gSettingMenu;
    return retVal;
}

/*==============================================================================
函数:
       SettingMenu_AddRef

说明:
       SettingMenu Applet 引用计数加1，并返回当前的引用数。

参数:
       pi [in]：ISettingMenu接口对象指针。

返回值:
       当前对SettingMenu Applet的引用数

备注:

==============================================================================*/
static uint32  SettingMenu_AddRef(ISettingMenu *pi)
{
    register CSettingMenu *pMe = (CSettingMenu*)pi;	

    ASSERT(pMe != NULL);
    ASSERT(pMe->m_nRefs > 0);

    return (++pMe->m_nRefs);
}

/*==============================================================================
函数:
       SettingMenu_Release

说明:
       释放对SettingMenu Applet的引用，引用计数减1，并返回当前的引用数。
        如果当前的引用数为零，还将释放Applet及相关资源。

参数:
       pi [in]：ISettingMenu接口对象指针。

返回值:
       当前对SettingMenu Applet的引用数

备注:

==============================================================================*/
static uint32  SettingMenu_Release (ISettingMenu *pi)
{
    register CSettingMenu *pMe = (CSettingMenu*)pi;

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
    SettingMenu_FreeAppData(pMe);

    (void) ISHELL_Release(pMe->m_pShell);
    (void) IMODULE_Release(pMe->m_pModule);

    // 注意：pMe是静态分配空间，因此不需要释放。FREE()
    return 0;
}

/*==============================================================================
函数:
       SettingMenu_InitAppData

说明:
       初始化SettingMenu Applet结构数据缺省值。

参数:
       pMe [in]：指向SettingMenu Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
       SUCCESS：初始化成功。
       EFAILED：初始化失败。

备注:

==============================================================================*/
static int SettingMenu_InitAppData(CSettingMenu *pMe)
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
    pMe->m_ePreState = SETTINGMENUST_NONE;
    pMe->m_eCurState = SETTINGMENUST_INIT;
    pMe->m_pActiveDlg = NULL;
    pMe->m_eDlgRet = DLGRET_CREATE;
    pMe->m_bNotOverwriteDlgRet = FALSE;

    pMe->m_currDlgId = 0;
    pMe->m_nSubDlgId = 0;
    pMe->m_input_mode = 0;
    pMe->m_ip_sel_num = 0;
    pMe->m_msg_id = 0;
    pMe->m_shaketype_sel = SET_SHAKE_NONE;
    pMe->m_strPhonePWD = NULL;

    if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                            AEECLSID_CONFIG,
                                            (void **)&pMe->m_pConfig))
    {
        SettingMenu_FreeAppData(pMe);
        return EFAILED;
    }
    if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                            AEECLSID_ANNUNCIATOR,
                                            (void **)&pMe->m_pAnn))
    {
        SettingMenu_FreeAppData(pMe);
        return EFAILED;
    }
	MSG_FATAL("IANNUNCIATOR_SetFieldIsActiveEx::::settingmunu111:::",0,0,0);
	//IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pAnn,FALSE);
#ifndef WIN32    
    // 创建 IRUIM 接口
    if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                 AEECLSID_RUIM,
                                 (void **) &pMe->m_pIRUIM))
    {
        SettingMenu_FreeAppData(pMe);
        return EFAILED;
    }
#endif//WIN32
	
    return SUCCESS;
}

/*==============================================================================
函数:
       SettingMenu_FreeAppData

说明:
       释放SettingMenu Applet使用的相关资源。

参数:
       pMe [in]：指向SettingMenu Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
       无。

备注:

==============================================================================*/
static void SettingMenu_FreeAppData(CSettingMenu *pMe)
{
    if (NULL == pMe)
    {
        return ;
    }

    FREEIF(pMe->m_strPhonePWD);
#ifdef FEATRUE_SET_IP_NUMBER
    {
        SettingMenu_Free_And_Save_Ip_Number(pMe);
    }
#endif/*FEATRUE_SET_IP_NUMBER*/

    if (pMe->m_pConfig)
    {
        ICONFIG_Release(pMe->m_pConfig);
        pMe->m_pConfig = NULL;
    }
    if (pMe->m_pAnn != NULL)
    {
       IANNUNCIATOR_Release(pMe->m_pAnn);
        pMe->m_pAnn = NULL;
    }
    // 释放 IRUIM 接口
    if (pMe->m_pIRUIM != NULL)
    {
        IRUIM_Release(pMe->m_pIRUIM);
        pMe->m_pIRUIM = NULL;
    }
	
}

/*==============================================================================
函数:
       SettingMenu_RunFSM

说明:
       SettingMenu Applet有限状态机引擎。

参数:
       pMe [in]：指向SettingMenu Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
       无。

备注:

==============================================================================*/
static void SettingMenu_RunFSM(CSettingMenu *pMe)
{
    NextFSMAction nextFSMAction = NFSMACTION_WAIT;

    for ( ; ; )
    {
        nextFSMAction = SettingMenu_ProcessState(pMe);

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
       SettingMenu_HandleEvent

说明:
       SettingMenu  Applet事件处理主函数。所有属于本Applet的事件，
       都将经由该函数处理。

参数:
       pi [in]：指向ISettingMenu接口t对象的指针。
       eCode：BREW事件代码。
       wParam：与事件相关联的数据。
       dwParam [in]：与事件相关联的数据。

返回值:
       TRUE：传入事件被处理。
       FALSE：传入事件没被处理。

备注:

==============================================================================*/
static boolean SettingMenu_HandleEvent(ISettingMenu *pi,
    AEEEvent eCode,
    uint16  wParam,
    uint32 dwParam
)
{
    CSettingMenu *pMe = (CSettingMenu*)pi;
    AEEAppStart *as;
    AEEDeviceInfo di; 

    ISHELL_GetDeviceInfo(pMe->m_pShell,&di);    
    //SETTING_ERR("%x, %x ,%x,SettingMenu_HandleEvent",eCode,wParam,dwParam);
	
    switch (eCode)
    {
        case EVT_APP_START:			
            pMe->m_bAppIsReady = FALSE;
            ASSERT(dwParam != 0);
            as = (AEEAppStart*)dwParam;
            if (NULL != pMe->m_pDisplay)
            {
                (void) IDISPLAY_Release(pMe->m_pDisplay);
                pMe->m_pDisplay = NULL;
            }
#ifdef FEATRUE_SET_IP_NUMBER            
            SettingMenu_Get_Ip_Number(pMe);
#endif
            pMe->m_pDisplay = as->pDisplay;
            (void) IDISPLAY_AddRef(pMe->m_pDisplay);
            pMe->m_rc = as->rc;
            pMe->m_rc.dy = di.cyScreen;
            pMe->m_rc.y = pMe->m_rc.y - SETTING_MENU_SOFK_HIGHT;
            
            pMe->m_bSuspending = FALSE;
            if(pMe->m_pAnn != NULL)
            {
                IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pAnn,FALSE);
            }
            // 开始SettingMenu状态机
            SettingMenu_RunFSM(pMe);
            return TRUE;

        case EVT_APP_STOP:
            if (pMe->m_pDisplay != NULL)
            {
                (void) IDISPLAY_Release(pMe->m_pDisplay);
                pMe->m_pDisplay = NULL;
            }
            pMe->m_bSuspending = TRUE;

            return TRUE;

        case EVT_APP_SUSPEND:
            pMe->m_bSuspending = TRUE;

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
            pMe->m_rc.y = pMe->m_rc.y - SETTING_MENU_SOFK_HIGHT;
            pMe->m_rc.dy = di.cyScreen;
            ERR("pMe->m_rc.y:%d,pMe->m_rc.dy:%d",pMe->m_rc.y,pMe->m_rc.dy,0);
            SettingMenu_RunFSM(pMe);
            return TRUE;

        case EVT_DIALOG_INIT:			
			if(OEM_IME_DIALOG == wParam)
			{
				return TRUE;
			}
            pMe->m_bAppIsReady = FALSE;
            pMe->m_pActiveDlg = (IDialog*)dwParam;
            pMe->m_pActiveDlgID = wParam;

            return SettingMenu_RouteDialogEvent(pMe,eCode,wParam,dwParam);

        case EVT_DIALOG_START:
            if(OEM_IME_DIALOG == wParam)
			{
				return TRUE;
			}
            return SettingMenu_RouteDialogEvent(pMe,eCode,wParam,dwParam);

        case EVT_USER_REDRAW:
            (void) SettingMenu_RouteDialogEvent(pMe,eCode,wParam,dwParam);
            (void)ISHELL_SetTimer ( pMe->m_pShell,
                            APPISREADY_TIMER,
                            SettingMenu_APPIsReadyTimer,
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
            return SettingMenu_RouteDialogEvent(pMe,eCode,wParam,dwParam);

        case EVT_DIALOG_END:
            if (wParam == 0)
            {
                return TRUE;
            }
            if(OEM_IME_DIALOG == wParam)
			{
				return ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_SETTINGMENU,EVT_USER_REDRAW,0,0);
			}
            pMe->m_bAppIsReady = FALSE;
            (void) SettingMenu_RouteDialogEvent(pMe,eCode,wParam,dwParam);
            pMe->m_pActiveDlg = NULL;

            // Applet被挂起，不跑状态机，等待Applet返回。（注意：EVT_APP_SUSPEND
            // 事件在EVT_DIALOG_END事件前发出。
            if (pMe->m_bSuspending == FALSE)
            {
                // 开始SettingMenu状态机
                SettingMenu_RunFSM(pMe);
            }
            return TRUE;
#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
		case EVT_PEN_UP:
		//case EVT_PEN_DOWN:
			{
				return SettingMenu_RouteDialogEvent(pMe,eCode,wParam,dwParam);
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
				else if((wParam == AVK_CLR) || (wParam == AVK_LEFT) ||(wParam == AVK_RIGHT))
				{
					eCode = EVT_KEY;
				}
				return SettingMenu_RouteDialogEvent(pMe,eCode,wParam,dwParam);
			}
#endif//FEATURE_LCD_TOUCH_ENABLE
        default:
            // 将接收到的事件路由至当前活动的对话框事件处理函数。
            return SettingMenu_RouteDialogEvent(pMe,eCode,wParam,dwParam);
    }
}

/*==============================================================================
函数:
       SettingMenu_APPIsReadyTimer

说明:
       定时器回调函数。主要用于控制快速按键问题的处理。

参数:
       pme [in]：void *类型指针，暗指CSettingMenu结构指针。

返回值:
       无。

备注:

==============================================================================*/
static void SettingMenu_APPIsReadyTimer(void *pme)
{
    CSettingMenu *pMe = (CSettingMenu *)pme;

    if (NULL == pMe)
    {
        return;
    }

    (void) ISHELL_PostEvent( pMe->m_pShell,
                            AEECLSID_APP_SETTINGMENU,
                            EVT_APPISREADY,
                            0,
                            0);
}
#ifdef FEATRUE_SET_IP_NUMBER
static void SettingMenu_Get_Ip_Number(CSettingMenu *pMe)
{
    IP_Number_Cfg  ip_cfg;
    MEMSET((void*)&ip_cfg,0 , sizeof(IP_Number_Cfg));

    ICONFIG_GetItem(pMe->m_pConfig,CFGI_IP_NUMBER,(void *)&ip_cfg,sizeof(IP_Number_Cfg));
    pMe->ip_number_tab.count = 0;
    pMe->ip_number_tab.head = NULL;
    pMe->ip_number_tab.tial = NULL;
    pMe->ip_number_tab.ip_state = ip_cfg.bStateOn;
    MEMSET(pMe->ip_number_tab.default_ip, 0 ,sizeof(pMe->ip_number_tab.default_ip));
    if(ip_cfg.ip_count>0)
    {
        char       c_buf[OEM_IP_NUMBER_MAX +1];
        char * point = NULL;
        char * head = NULL;
        point = STRCHR(ip_cfg.Number, ':');
        head = ip_cfg.Number;
        while(point)
        {
            number_table *num_tab = NULL;
            MEMSET((void *)c_buf, 0 ,sizeof(c_buf));
            STRNCPY(c_buf,head, point - head);
            head = point +1;
            point = STRCHR(head, ':');

            num_tab = (number_table *)MALLOC(sizeof(number_table));
            if(num_tab)
            {
                MEMSET(num_tab,0 ,sizeof(number_table));
                num_tab->next = NULL;
                STRTOWSTR(c_buf, num_tab->number, (OEM_IP_NUMBER_MAX+1)*sizeof(AECHAR));
                pMe->ip_number_tab.count ++;
            }
            if(pMe->ip_number_tab.head == NULL) //the first node
            {
                pMe->ip_number_tab.head = num_tab;
            }
            if(pMe->ip_number_tab.tial)
            {
                pMe->ip_number_tab.tial->next = num_tab;
            }
            pMe->ip_number_tab.tial = num_tab;
        }
        if(ip_cfg.default_number[0] != 0)
        {
            STRTOWSTR(ip_cfg.default_number,pMe->ip_number_tab.default_ip,sizeof(pMe->ip_number_tab.default_ip));
        }
        SettingMenu_Print_IP_Number(pMe);
    }
}

static void SettingMenu_Free_And_Save_Ip_Number(CSettingMenu *pMe)
{
    IP_Number_Cfg  ip_cfg;
    char       c_buf[OEM_IP_NUMBER_MAX +2];

    //if(pMe->ip_number_tab.head == NULL)
    //{
    //    return;
    //}

    MEMSET((void*)&ip_cfg,0 , sizeof(IP_Number_Cfg));
    if(pMe->ip_number_tab.head)
    {
        if(pMe->ip_number_tab.default_ip[0] != 0)
        {
            WSTRTOSTR(pMe->ip_number_tab.default_ip ,ip_cfg.default_number,sizeof(ip_cfg.default_number));
        }
        else
        {
            WSTRTOSTR(pMe->ip_number_tab.head->number,ip_cfg.default_number,sizeof(ip_cfg.default_number));
        }
    }
    SettingMenu_Print_IP_Number(pMe);
    while(pMe->ip_number_tab.head)
    {
        number_table *temp = NULL;
        temp = pMe->ip_number_tab.head;
        MEMSET((void *)c_buf, 0 ,sizeof(c_buf));
        WSTRTOSTR(temp->number,c_buf,OEM_IP_NUMBER_MAX+2);

        STRLCAT(c_buf,":",OEM_IP_NUMBER_MAX+2);
        STRLCAT(ip_cfg.Number,c_buf,OEM_IP_NUMBER_COUNT * (OEM_IP_NUMBER_MAX+1));
        ip_cfg.ip_count ++;
        pMe->ip_number_tab.head = temp->next;
        FREEIF(temp);
    }
    pMe->ip_number_tab.head = NULL;
    pMe->ip_number_tab.tial =NULL;
    pMe->ip_number_tab.count = 0;
    ip_cfg.bStateOn = (pMe->ip_number_tab.ip_state == 0 ? FALSE:TRUE);
    ICONFIG_SetItem(pMe->m_pConfig,CFGI_IP_NUMBER,(void *)&ip_cfg,sizeof(IP_Number_Cfg));
}

void SettingMenu_Print_IP_Number(CSettingMenu *pMe)
{
#if 0
    number_table *temp = pMe->ip_number_tab.head;
    if(pMe->ip_number_tab.default_ip[0] != 0)
    {
    }
    
    while(temp)
    {
        temp = temp->next;
    }
#endif
}
#endif /*FEATRUE_SET_IP_NUMBER*/

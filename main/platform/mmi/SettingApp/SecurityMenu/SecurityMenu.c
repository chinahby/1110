/*==============================================================================
// 文件：
//        SecurityMenur.c
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
-----------      ----------  --------  -----------------------------------------------
==============================================================================*/


/*==============================================================================
                                 本文件包含的外部文件
==============================================================================*/
#include "SecurityMenu_priv.h"

/*==============================================================================
                                 宏定义和常数
==============================================================================*/

/*==============================================================================
                                 类型定义
==============================================================================*/

/*==============================================================================
                                 函数声明
==============================================================================*/
static void SecurityMenu_APPIsReadyTimer(void *pme);

/*----------------------模块相关函数声明---------------------*/
/*static*/ int  SecurityMenuMod_Load(IShell   *pIShell,
                                        void     *ph,
                                        IModule  **ppMod);

int  SecurityMenuMod_New(int16              nSize,
                                        IShell             *pIShell,
                                        void               *ph,
                                        IModule            **ppMod,
                                        PFNMODCREATEINST   pfnMC,
                                        PFNFREEMODDATA     pfnMF);

static int  SecurityMenuMod_CreateInstance(IModule   *po,
                                        IShell    *pIShell,
                                        AEECLSID  ClsId,
                                        void      **ppObj);

static uint32 SecurityMenuMod_AddRef(IModule *po);

static uint32 SecurityMenuMod_Release(IModule *po);

static void SecurityMenuMod_FreeResources(IModule     *po,
                                        IHeap       *ph,
                                        IFileMgr    *pfm);

/*----------------------Applet相关函数声明---------------------*/
static int SecurityMenu_New(IShell         *ps,
                                        IModule        *pIModule,
                                        ISecurityMenu  **ppObj);

static uint32  SecurityMenu_AddRef(ISecurityMenu *pi);

static uint32  SecurityMenu_Release (ISecurityMenu *pi);

static boolean SecurityMenu_HandleEvent(ISecurityMenu *pi,
                                        AEEEvent      eCode,
                                        uint16        wParam,
                                        uint32        dwParam);

static int SecurityMenu_InitAppData(CSecurityMenu *pMe);

static void SecurityMenu_FreeAppData(CSecurityMenu *pMe);

static void SecurityMenu_RunFSM(CSecurityMenu *pMe);

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
static SecurityMenuMod gSecurityMenuMod;

static const VTBL(IModule) gModFuncs =
{
    SecurityMenuMod_AddRef,
    SecurityMenuMod_Release,
    SecurityMenuMod_CreateInstance,
    SecurityMenuMod_FreeResources
};

// 只允许一个SecurityMenu实例。每次创建SecurityMenu Applet时，
// 返回同一结构指针给BREW层。
static CSecurityMenu gSecurityMenu;

static const VTBL(ISecurityMenu) gSecurityMenuMethods =
{
    SecurityMenu_AddRef,
    SecurityMenu_Release,
    SecurityMenu_HandleEvent
};

/*----------------------模块相关函数定义---------------------*/

/*==============================================================================
函数:
       SecurityMenuMod_Load

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
__declspec(dllexport) int AEEMod_Load(IShell    *pIShell,
                                        void      *ph,
                                        IModule   **ppMod)
#else
/*static*/ int SecurityMenuMod_Load(IShell    *pIShell,
                                        void      *ph,
                                        IModule   **ppMod)
#endif
#else
/*static*/ int SecurityMenuMod_Load(IShell    *pIShell,
                                        void      *ph,
                                        IModule   **ppMod)
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

    return SecurityMenuMod_New(sizeof(SecurityMenuMod),
                                        pIShell,
                                        ph,
                                        ppMod,
                                        NULL,
                                        NULL);
}

/*==============================================================================
函数:
       SecurityMenuMod_New

说明:
       用来初始化SecurityMenu模块虚拟表（也增加引用计数）的助手函数。

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
int  SecurityMenuMod_New(int16             nSize,
                                        IShell            *pIShell,
                                        void              *ph,
                                        IModule           **ppMod,
                                        PFNMODCREATEINST  pfnMC,
                                        PFNFREEMODDATA    pfnMF)
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
    MEMSET(&gSecurityMenuMod,0,sizeof(SecurityMenuMod));

    INIT_VTBL(&gSecurityMenuMod,IModule,gModFuncs);
    gSecurityMenuMod.m_nRefs = 1;
    *ppMod = (IModule *)&gSecurityMenuMod;
    return AEE_SUCCESS;
}

/*==============================================================================
函数:
       SecurityMenuMod_AddRef

说明:
       SecurityMenu 模块引用计数加1，并返回当前的引用数。

参数:
       po [in]：IModule接口对象指针。

返回值:
       当前对SecurityMenu 模块的引用数

备注:

==============================================================================*/
static uint32 SecurityMenuMod_AddRef(IModule *po)
{
    return(++((SecurityMenuMod *)po)->m_nRefs);
}

/*==============================================================================
函数:
       SecurityMenuMod_Release

说明:
       SecurityMenu 模块引用计数减1，并返回当前的引用数。

参数:
       po [in]：IModule接口对象指针。

返回值:
       当前对SecurityMenu 模块的引用数

备注:

==============================================================================*/
static uint32 SecurityMenuMod_Release(IModule *po)
{
    if (((SecurityMenuMod *) po)->m_nRefs == 0)
    {
        return 0;
    }

    return(--((SecurityMenuMod *)po)->m_nRefs);
}

/*==============================================================================
函数:
       SecurityMenuMod_CreateInstance

说明:
        通过创建(初始化)SecurityMenu对象来创建模块实例。

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
static int  SecurityMenuMod_CreateInstance(IModule    *po,
                                        IShell     *pIShell,
                                        AEECLSID   ClsId,
                                        void       **ppObj)
{
    *ppObj = NULL;

    if (ClsId != AEECLSID_APP_SECURITYMENU)
    {
        return EFAILED;
    }

    if (SecurityMenu_New(pIShell, po, (ISecurityMenu**)ppObj) != SUCCESS)
    {
        return EFAILED;
    }
    return SUCCESS;
}

/*==============================================================================
函数:
       SecurityMenuMod_FreeResources

说明:
        释放SecurityMenu模块资源的函数。

参数:
       po ：IModule。
       ph ：IHeap接口指针。
       pfm :IFileMgr接口指针。

返回值:
       无

备注:
       不需用户去调用。

==============================================================================*/
static void SecurityMenuMod_FreeResources(IModule  *po,
                                        IHeap    *ph,
                                        IFileMgr *pfm)
{

}

/*----------------------Applet相关函数定义---------------------*/
/*==============================================================================
函数:
       SecurityMenu_New

说明:
        初始化SecurityMenu Applet虚拟表和内部数据变量，同时其引用计数加1。

参数:
       ps [in]：IShell接口对象指针。
       pIModule [in]：pIModule接口对象指针。
       ppObj [out]:：指向ISecurityMenu对象指针的指针。

返回值:
       EFAILED：失败。
       SUCCESS：成功。

备注:
       不需用户去调用。
       关于Applet结构对象可使用动态分配内存的方式创建，本处采用的是使用静态
       变量的方式。建议对结构比较大的Applet使用静态变量的方式。对于动态分配
       内存的方式需要注意对所分配空间的释放。

==============================================================================*/
static int SecurityMenu_New(IShell         *ps,
                                        IModule        *pIModule,
                                        ISecurityMenu  **ppObj)
{
    int retVal = SUCCESS;

    if (0 == gSecurityMenu.m_nRefs)
    {
        // Must initialize the object
        MEMSET(&gSecurityMenu,  0, sizeof(CSecurityMenu));
        INIT_VTBL(&gSecurityMenu, ISecurityMenu, gSecurityMenuMethods);

        gSecurityMenu.m_nRefs     = 0;
        gSecurityMenu.m_pShell    = ps;
        gSecurityMenu.m_pModule   = pIModule;

        (void) ISHELL_AddRef(ps);
        (void) IMODULE_AddRef(pIModule);

        retVal = SecurityMenu_InitAppData(&gSecurityMenu);
    }

    ++gSecurityMenu.m_nRefs;
    *ppObj = (ISecurityMenu*)&gSecurityMenu;
    return retVal;
}

/*==============================================================================
函数:
       SecurityMenu_AddRef

说明:
       SecurityMenu Applet 引用计数加1，并返回当前的引用数。

参数:
       pi [in]：ISecurityMenu接口对象指针。

返回值:
       当前对SecurityMenu Applet的引用数

备注:

==============================================================================*/
static uint32  SecurityMenu_AddRef(ISecurityMenu *pi)
{
    register CSecurityMenu *pMe = (CSecurityMenu*)pi;

    ASSERT(pMe != NULL);
    ASSERT(pMe->m_nRefs > 0);

    return (++pMe->m_nRefs);
}

/*==============================================================================
函数:
       SecurityMenu_Release

说明:
       释放对SecurityMenu Applet的引用，引用计数减1，并返回当前的引用数。
        如果当前的引用数为零，还将释放Applet及相关资源。

参数:
       pi [in]：ISecurityMenu接口对象指针。

返回值:
       当前对SecurityMenu Applet的引用数

备注:

==============================================================================*/
static uint32  SecurityMenu_Release (ISecurityMenu *pi)
{
    register CSecurityMenu *pMe = (CSecurityMenu*)pi;

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
    SecurityMenu_FreeAppData(pMe);

    (void) ISHELL_Release(pMe->m_pShell);
    (void) IMODULE_Release(pMe->m_pModule);

    // 注意：pMe是静态分配空间，因此不需要释放。FREE()
    return 0;
}

/*==============================================================================
函数:
       SecurityMenu_InitAppData

说明:
       初始化SecurityMenu Applet结构数据缺省值。

参数:
       pMe [in]：指向SecurityMenu Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
       SUCCESS：初始化成功。
       EFAILED：初始化失败。

备注:

==============================================================================*/
static int SecurityMenu_InitAppData(CSecurityMenu *pMe)
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
    pMe->m_ePreState = SECURITYMENU_NONE;
    pMe->m_eCurState = SECURITYMENU_INIT;
    pMe->m_pActiveDlg = NULL;
    pMe->m_eDlgRet = DLGRET_CREATE;
    pMe->m_bNotOverwriteDlgRet = FALSE;
    pMe->m_PromtImage = NULL;
    pMe->m_lock_sel = SEC_SEL_NONE;
    //pMe->passwordInputState = 0;
    pMe->m_bIsConfirmPassword = FALSE;
    pMe->m_eRUIMSCode = ENTERPUK_STEP0;
    pMe->m_strPhonePWD = NULL;
	pMe->m_currDlgId = 0;
	pMe->m_nSubDlgId = 0;
	pMe->m_strPhoneNUM = NULL;
    if (ISHELL_CreateInstance(pMe->m_pShell,
                                 AEECLSID_ANNUNCIATOR,
                                 (void **) &pMe->m_pIAnn))
    {
        SecurityMenu_FreeAppData(pMe);
        return FALSE;
    }
	MSG_FATAL("IANNUNCIATOR_SetFieldIsActiveEx::::securitymenu111:::",0,0,0);
	//IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE);
    if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                        AEECLSID_CONFIG,
                                        (void **)&pMe->m_pConfig))
    {
        SecurityMenu_FreeAppData(pMe);
        return EFAILED;
    }

    if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                        AEECLSID_STATIC,
                                        (void **)&pMe->m_pIStatic))
    {
        SecurityMenu_FreeAppData(pMe);
        return EFAILED;
    }
#ifndef WIN32
    // 创建 IRUIM 接口
    if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                 AEECLSID_RUIM,
                                 (void **) &pMe->m_pIRUIM))
    {
        SecurityMenu_FreeAppData(pMe);
        return EFAILED;
    }
#endif//WIN32 
    //pMe->m_pPhoneLockPassword = NULL;
    
    pMe->nOldPSWLength = 0;
    pMe->nNewPSWLength = 0;
    
    return SUCCESS;
}

/*==============================================================================
函数:
       SecurityMenu_FreeAppData

说明:
       释放SecurityMenu Applet使用的相关资源。

参数:
       pMe [in]：指向SecurityMenu Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
       无。

备注:

==============================================================================*/
static void SecurityMenu_FreeAppData(CSecurityMenu *pMe)
{
    if (NULL == pMe)
    {
        return ;
    }

    FREEIF(pMe->m_strPhonePWD);
	FREEIF(pMe->m_strPhoneNUM);

    if (pMe->m_pConfig)
    {
        ICONFIG_Release(pMe->m_pConfig);
        pMe->m_pConfig = NULL;
    }

    if (pMe->m_pIStatic)
    {
        ISTATIC_Release(pMe->m_pIStatic);
    }

    if(pMe->m_PromtImage)
    {
        IIMAGE_Release(pMe->m_PromtImage);
        pMe->m_PromtImage = NULL;
    }

     // 释放 IRUIM 接口
    if (pMe->m_pIRUIM != NULL)
    {
        IRUIM_Release(pMe->m_pIRUIM);
        pMe->m_pIRUIM = NULL;
    }

    if (pMe->m_pIAnn)
    {
        IANNUNCIATOR_Release(pMe->m_pIAnn);
        pMe->m_pIAnn= NULL;
    }
    if(pMe->m_strPhonePWD)
    {
      FREEIF(pMe->m_strPhonePWD);
    }
	if(pMe->m_strPhoneNUM)
	{
		FREEIF(pMe->m_strPhoneNUM);
	}
    /*if(pMe->m_oldPassword)
    {
        ISTATIC_Release(pMe->m_oldPassword);
        pMe->m_oldPassword = NULL;
    }

    if(pMe->m_newPassword)
    {
        ISTATIC_Release(pMe->m_newPassword);
        pMe->m_newPassword = NULL;
    }

    if(pMe->m_confirmPassword)
    {
        ISTATIC_Release(pMe->m_confirmPassword);
        pMe->m_confirmPassword = NULL;
    }
*/
    
    //FREEIF( pMe->m_pPhoneLockPassword);
}

/*==============================================================================
函数:
       SecurityMenu_RunFSM

说明:
       SecurityMenu Applet有限状态机引擎。

参数:
       pMe [in]：指向SecurityMenu Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
       无。

备注:

==============================================================================*/
static void SecurityMenu_RunFSM(CSecurityMenu *pMe)
{
    NextFSMAction nextFSMAction = NFSMACTION_WAIT;

    for ( ; ; )
    {
        nextFSMAction = SecurityMenu_ProcessState(pMe);

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
       SecurityMenu_HandleEvent

说明:
       SecurityMenu  Applet事件处理主函数。所有属于本Applet的事件，
       都将经由该函数处理。

参数:
       pi [in]：指向ISecurityMenu接口t对象的指针。
       eCode：BREW事件代码。
       wParam：与事件相关联的数据。
       dwParam [in]：与事件相关联的数据。

返回值:
       TRUE：传入事件被处理。
       FALSE：传入事件没被处理。

备注:

==============================================================================*/
static boolean SecurityMenu_HandleEvent(ISecurityMenu  *pi,
                                        AEEEvent        eCode,
                                        uint16          wParam,
                                        uint32          dwParam)
{
    CSecurityMenu *pMe = (CSecurityMenu*)pi;
    AEEAppStart *as;
    extern boolean start_security_setting_by_user;
    //nv_item_type nvi;
    //SEC_ERR("%x,%x,%x,SecurityMenu_HandleEvent",eCode,wParam,dwParam);    
    AEEDeviceInfo di;
    MSG_FATAL("SecurityMenu_HandleEvent------eCode=%d----wParam=%d",eCode,wParam,0);
    ISHELL_GetDeviceInfo(pMe->m_pShell,&di);
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
#ifdef FEATURE_LCD_TOUCH_ENABLE            
            if ((as != NULL) && (as->pszArgs != NULL))	
			{
                if (STRNCMP(as->pszArgs, "InputPW", 7) == 0)
				{
                    pMe->m_pActiveTSIMInputID=0;
					MOVE_TO_STATE(SECURITYMENU_TSIMPASSWORDINPUT);				
				}
			}
#endif       
#ifdef FEATURE_VERSION_C316
			if ((as != NULL) && (as->pszArgs != NULL))	
			{
                if (STRNCMP(as->pszArgs, "Mobiletracker", 13) == 0)
				{
                    //pMe->m_pActiveTSIMInputID=0;
					MOVE_TO_STATE(MOBILE_TRACKER);				
				}
			}
#endif
            pMe->m_pDisplay = as->pDisplay;
            (void) IDISPLAY_AddRef(pMe->m_pDisplay);
            pMe->m_rc = as->rc;
            pMe->m_rc.y = pMe->m_rc.y - SECRUITY_MENU_SOFK_HIGHT;
            pMe->m_rc.dy = di.cyScreen;
            pMe->m_bSuspending = FALSE;
            if(pMe->m_pIAnn != NULL)
            {
                IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE);
            }
            // 开始SecurityMenu状态机
            SecurityMenu_RunFSM(pMe);
            start_security_setting_by_user = FALSE;
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
            pMe->m_rc.y = pMe->m_rc.y - SECRUITY_MENU_SOFK_HIGHT;
            pMe->m_rc.dy = di.cyScreen;
			if( start_security_setting_by_user)
			{
				pMe->m_ePreState = SECURITYMENU_NONE;
				pMe->m_eCurState = SECURITYMENU_INIT;
				pMe->m_currDlgId = 0;
				pMe->m_nSubDlgId = 0;
				start_security_setting_by_user = FALSE;
			}
            SecurityMenu_RunFSM(pMe);
            return TRUE;

        case EVT_DIALOG_INIT:
            if( wParam == OEM_IME_DIALOG)
			{
				return TRUE;
			}
            pMe->m_bAppIsReady = FALSE;
            pMe->m_pActiveDlg = (IDialog*)dwParam;
            pMe->m_pActiveDlgID = wParam;

            return SecurityMenu_RouteDialogEvent(pMe,eCode,wParam,dwParam);

        case EVT_DIALOG_START:
            if(OEM_IME_DIALOG == wParam)
			{
				return TRUE;
			}
             (void)ISHELL_SetTimer ( pMe->m_pShell,
                                                APPISREADY_TIMER,
                                                SecurityMenu_APPIsReadyTimer,
                                                pMe);
             
            return SecurityMenu_RouteDialogEvent(pMe,eCode,wParam,dwParam);

        case EVT_USER_REDRAW:
            (void) SecurityMenu_RouteDialogEvent(pMe,eCode,wParam,dwParam);

            /*
            (void)ISHELL_SetTimer ( pMe->m_pShell,
                                                APPISREADY_TIMER,
                                                SecurityMenu_APPIsReadyTimer,
                                                pMe);
            */
                
            return TRUE;

        case EVT_APPISREADY:
            pMe->m_bAppIsReady = TRUE;
            return TRUE;

        case EVT_KEY_PRESS:
        case EVT_KEY_RELEASE:
        case EVT_KEY:
        case EVT_COMMAND:
			#if !defined(FEATURE_VERSION_K212) && !defined(FEATURE_VERSION_EC99)&&!defined(FEATURE_QVGA_INHERIT_K212)&&!defined(FEATURE_VERSION_K212_HUALU)
            if (!pMe->m_bAppIsReady)
            {
                return TRUE;
            }
			#endif
            return SecurityMenu_RouteDialogEvent(pMe,eCode,wParam,dwParam);

        case EVT_DIALOG_END:
            if (wParam == 0)
            {
                return TRUE;
            }
            if(OEM_IME_DIALOG == wParam)
			{
				return ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER_REDRAW,0,0);
			}
            pMe->m_bAppIsReady = FALSE;
            (void) SecurityMenu_RouteDialogEvent(pMe,eCode,wParam,dwParam);
            pMe->m_pActiveDlg = NULL;

            // Applet被挂起，不跑状态机，等待Applet返回。（注意：EVT_APP_SUSPEND
            // 事件在EVT_DIALOG_END事件前发出。
            if (pMe->m_bSuspending == FALSE)
            {
                // 开始SecurityMenu状态机
                SecurityMenu_RunFSM(pMe);
            }
            return TRUE;

#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
		case EVT_PEN_UP:
		//case EVT_PEN_DOWN:
			{
				MSG_FATAL("EVT_PEN_UP......................",0,0,0);
				return SecurityMenu_RouteDialogEvent(pMe,eCode,wParam,dwParam);
			}
			break;

		case EVT_USER:
			{
					MSG_FATAL("EVT_USER......................",0,0,0);
				if((wParam == AVK_SELECT) || (wParam == AVK_INFO))
				{
					if(dwParam == DWPARMA_OTHER)
					{
						eCode = EVT_KEY_RELEASE;
						dwParam = 0;
					}
					else if(dwParam != 0)
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
					MSG_FATAL("AVK_CLR......................",0,0,0);
					eCode = EVT_KEY;
				}
				return SecurityMenu_RouteDialogEvent(pMe,eCode,wParam,dwParam);
			}
#endif//FEATURE_LCD_TOUCH_ENABLE
        default:
            // 将接收到的事件路由至当前活动的对话框事件处理函数。
            return SecurityMenu_RouteDialogEvent(pMe,eCode,wParam,dwParam);
    }
}

/*==============================================================================
函数:
       SecurityMenu_APPIsReadyTimer

说明:
       定时器回调函数。主要用于控制快速按键问题的处理。

参数:
       pme [in]：void *类型指针，暗指CSecurityMenu结构指针。

返回值:
       无。

备注:

==============================================================================*/
static void SecurityMenu_APPIsReadyTimer(void *pme)
{
    CSecurityMenu *pMe = (CSecurityMenu *)pme;

    if (NULL == pMe)
    {
        return;
    }

    (void) ISHELL_PostEvent(pMe->m_pShell,
                                        AEECLSID_APP_SECURITYMENU,
                                        EVT_APPISREADY,
                                        0,
                                        0);
}

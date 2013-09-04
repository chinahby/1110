/*==============================================================================
// 文件：
//        DisplayMenu.c
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
#include "DisplayMenu_priv.h"

/*==============================================================================
                                 宏定义和常数
==============================================================================*/

/*==============================================================================
                                 类型定义
==============================================================================*/

/*==============================================================================
                                 函数声明
==============================================================================*/
static void DisplayMenu_APPIsReadyTimer(void *pme);

/*----------------------模块相关函数声明---------------------*/

/*static*/ int  DisplayMenuMod_Load(IShell   *pIShell,
    void     *ph,
    IModule **ppMod
);

int  DisplayMenuMod_New(int16  nSize,
    IShell   *pIShell,
    void     *ph,
    IModule  **ppMod,
    PFNMODCREATEINST pfnMC,
    PFNFREEMODDATA pfnMF
);

static int  DisplayMenuMod_CreateInstance(IModule *po,
    IShell *pIShell,
    AEECLSID ClsId,
    void **ppObj
);

static uint32 DisplayMenuMod_AddRef(IModule *po);

static uint32 DisplayMenuMod_Release(IModule *po);

static void DisplayMenuMod_FreeResources(IModule  *po,
    IHeap  *ph,
    IFileMgr *pfm
);

/*----------------------Applet相关函数声明---------------------*/
static int DisplayMenu_New(IShell *ps,
    IModule *pIModule,
    IDisplayMenu **ppObj
);

static uint32  DisplayMenu_AddRef(IDisplayMenu *pi);

static uint32  DisplayMenu_Release (IDisplayMenu *pi);

static boolean DisplayMenu_HandleEvent(IDisplayMenu *pi,
    AEEEvent eCode,
    uint16  wParam,
    uint32 dwParam
);

static void DisplayMenu_ChangePictrue(IDisplayMenu * pMe, boolean direction);
static int DisplayMenu_InitAppData(CDisplayMenu *pMe);

static void DisplayMenu_FreeAppData(CDisplayMenu *pMe);

static void DisplayMenu_RunFSM(CDisplayMenu *pMe);

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
static DisplayMenuMod gDisplayMenuMod;

static const VTBL(IModule) gModFuncs =
{
    DisplayMenuMod_AddRef,
    DisplayMenuMod_Release,
    DisplayMenuMod_CreateInstance,
    DisplayMenuMod_FreeResources
};

// 只允许一个DisplayMenu实例。每次创建DisplayMenu Applet时，
// 返回同一结构指针给BREW层。
static CDisplayMenu gDisplayMenu;

static const VTBL(IDisplayMenu) gDisplayMenuMethods =
{
    DisplayMenu_AddRef,
    DisplayMenu_Release,
    DisplayMenu_HandleEvent,
    DisplayMenu_ChangePictrue
};

/*----------------------模块相关函数定义---------------------*/

/*==============================================================================
函数:
       DisplayMenuMod_Load

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
/*static*/ int DisplayMenuMod_Load(IShell   *pIShell,
                                                   void     *ph,
                                                   IModule **ppMod)
#endif
#else
/*static*/ int DisplayMenuMod_Load(IShell   *pIShell,
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

    return DisplayMenuMod_New(sizeof(DisplayMenuMod),
                    pIShell,
                    ph,
                    ppMod,
                    NULL,
                    NULL);
}

/*==============================================================================
函数:
       DisplayMenuMod_New

说明:
       用来初始化DisplayMenu模块虚拟表（也增加引用计数）的助手函数。

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
int  DisplayMenuMod_New(int16  nSize,
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
    if (!pIShell)
    {
        return EFAILED;
    }

    MEMSET(&gDisplayMenuMod,0,sizeof(DisplayMenuMod));

    INIT_VTBL(&gDisplayMenuMod,IModule,gModFuncs);
    gDisplayMenuMod.m_nRefs = 1;
    *ppMod = (IModule *)&gDisplayMenuMod;
    return AEE_SUCCESS;
}

/*==============================================================================
函数:
       DisplayMenuMod_AddRef

说明:
       DisplayMenu 模块引用计数加1，并返回当前的引用数。

参数:
       po [in]：IModule接口对象指针。

返回值:
       当前对DisplayMenu 模块的引用数

备注:

==============================================================================*/
static uint32 DisplayMenuMod_AddRef(IModule *po)
{
    return(++((DisplayMenuMod *)po)->m_nRefs);
}

/*==============================================================================
函数:
       DisplayMenuMod_Release

说明:
       DisplayMenu 模块引用计数减1，并返回当前的引用数。

参数:
       po [in]：IModule接口对象指针。

返回值:
       当前对DisplayMenu 模块的引用数

备注:

==============================================================================*/
static uint32 DisplayMenuMod_Release(IModule *po)
{
    if (((DisplayMenuMod *) po)->m_nRefs == 0)
    {
        return 0;
    }

    return(--((DisplayMenuMod *)po)->m_nRefs);
}

/*==============================================================================
函数:
       DisplayMenuMod_CreateInstance

说明:
        通过创建(初始化)DisplayMenu对象来创建模块实例。

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
static int  DisplayMenuMod_CreateInstance(IModule *po,
    IShell *pIShell,
    AEECLSID ClsId,
    void **ppObj
)
{
    *ppObj = NULL;

    if (ClsId != AEECLSID_APP_DISPLAYMENU)
    {
        return EFAILED;
    }

    if (DisplayMenu_New(pIShell, po, (IDisplayMenu**)ppObj) != SUCCESS)
    {
        return EFAILED;
    }
    return SUCCESS;
}

/*==============================================================================
函数:
       DisplayMenuMod_FreeResources

说明:
        释放DisplayMenu模块资源的函数。

参数:
       po ：IModule。
       ph ：IHeap接口指针。
       pfm :IFileMgr接口指针。

返回值:
       无

备注:
       不需用户去调用。

==============================================================================*/
static void DisplayMenuMod_FreeResources(IModule  *po,
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
       DisplayMenu_New

说明:
        初始化DisplayMenu Applet虚拟表和内部数据变量，同时其引用计数加1。

参数:
       ps [in]：IShell接口对象指针。
       pIModule [in]：pIModule接口对象指针。
       ppObj [out]:：指向IDisplayMenu对象指针的指针。

返回值:
       EFAILED：失败。
       SUCCESS：成功。

备注:
       不需用户去调用。
       关于Applet结构对象可使用动态分配内存的方式创建，本处采用的是使用静态
       变量的方式。建议对结构比较大的Applet使用静态变量的方式。对于动态分配
       内存的方式需要注意对所分配空间的释放。

==============================================================================*/
static int DisplayMenu_New(IShell *ps,
    IModule *pIModule,
    IDisplayMenu **ppObj
)
{
    int retVal = SUCCESS;

    if (0 == gDisplayMenu.m_nRefs)
    {
        // Must initialize the object
        MEMSET(&gDisplayMenu,  0, sizeof(CDisplayMenu));
        INIT_VTBL(&gDisplayMenu, IDisplayMenu, gDisplayMenuMethods);

        gDisplayMenu.m_nRefs     = 0;
        gDisplayMenu.m_pShell    = ps;
        gDisplayMenu.m_pModule   = pIModule;

        (void) ISHELL_AddRef(ps);
        (void) IMODULE_AddRef(pIModule);

        retVal = DisplayMenu_InitAppData(&gDisplayMenu);
    }

    ++gDisplayMenu.m_nRefs;
    *ppObj = (IDisplayMenu*)&gDisplayMenu;
    return retVal;
}

/*==============================================================================
函数:
       DisplayMenu_AddRef

说明:
       DisplayMenu Applet 引用计数加1，并返回当前的引用数。

参数:
       pi [in]：IDisplayMenu接口对象指针。

返回值:
       当前对DisplayMenu Applet的引用数

备注:

==============================================================================*/
static uint32  DisplayMenu_AddRef(IDisplayMenu *pi)
{
    register CDisplayMenu *pMe = (CDisplayMenu*)pi;

    ASSERT(pMe != NULL);
    ASSERT(pMe->m_nRefs > 0);

    return (++pMe->m_nRefs);
}

/*==============================================================================
函数:
       DisplayMenu_Release

说明:
       释放对DisplayMenu Applet的引用，引用计数减1，并返回当前的引用数。
        如果当前的引用数为零，还将释放Applet及相关资源。

参数:
       pi [in]：IDisplayMenu接口对象指针。

返回值:
       当前对DisplayMenu Applet的引用数

备注:

==============================================================================*/
static uint32  DisplayMenu_Release (IDisplayMenu *pi)
{
    register CDisplayMenu *pMe = (CDisplayMenu*)pi;

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
    DisplayMenu_FreeAppData(pMe);

    (void) ISHELL_Release(pMe->m_pShell);
    (void) IMODULE_Release(pMe->m_pModule);

    // 注意：pMe是静态分配空间，因此不需要释放。FREE()
    return 0;
}

/*==============================================================================
函数:
       DisplayMenu_InitAppData

说明:
       初始化DisplayMenu Applet结构数据缺省值。

参数:
       pMe [in]：指向DisplayMenu Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
       SUCCESS：初始化成功。
       EFAILED：初始化失败。

备注:

==============================================================================*/
static int DisplayMenu_InitAppData(CDisplayMenu *pMe)
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
    pMe->m_ePreState = DISPLAYMENUST_NONE;
    pMe->m_eCurState = DISPLAYMENUST_INIT;
    pMe->m_pActiveDlg = NULL;
    pMe->m_eDlgRet = DLGRET_CREATE;
    pMe->m_bNotOverwriteDlgRet = FALSE;

    pMe->m_currDlgId = 0;
    pMe->m_nSubDlgId = 0;
    pMe->m_nExSubDlgId = 0;

    pMe->m_pWallPaper = NULL;
    pMe->m_pAniPaper = NULL;
    //pMe->m_pTitleImg = NULL;

    pMe->m_DelImageBack = FALSE;    //默认取图片的NV值
    pMe->m_bOkToDo = TRUE;
    pMe->m_msg_id = 0;
    if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                            AEECLSID_CONFIG,
                                            (void **)&pMe->m_pConfig))
    {
        DisplayMenu_FreeAppData(pMe);
        return EFAILED;
    }

    if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                            AEECLSID_BACKLIGHT,
                                            (void **)&pMe->m_pIBacklight))
    {
        DisplayMenu_FreeAppData(pMe);
        return EFAILED;
    }
#if defined(FEATURE_BACKLIGHT_KEYPAD)
    if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                            AEECLSID_BACKLIGHT_KEYPAD,
                                            (void **)&pMe->m_pKeypadBacklight))
    {
        DisplayMenu_FreeAppData(pMe);
        return EFAILED;
    }
#endif
    if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,AEECLSID_ANNUNCIATOR,(void **)&pMe->m_pIAnn))
    {
        DisplayMenu_FreeAppData(pMe);
        return EFAILED;
    }
    
    if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                            AEECLSID_FILEMGR,
                                            (void **)&pMe->m_pIFileMgr))
    {
        DisplayMenu_FreeAppData(pMe);
        return EFAILED;
    }
    return SUCCESS;
}

/*==============================================================================
函数:
       DisplayMenu_FreeAppData

说明:
       释放DisplayMenu Applet使用的相关资源。

参数:
       pMe [in]：指向DisplayMenu Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
       无。

备注:

==============================================================================*/
static void DisplayMenu_FreeAppData(CDisplayMenu *pMe)
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

    if(pMe->m_pIBacklight)
    {
        IBACKLIGHT_Release(pMe->m_pIBacklight);
        pMe->m_pIBacklight = NULL;
    }
#if defined(FEATURE_BACKLIGHT_KEYPAD)
    if(pMe->m_pKeypadBacklight)
    {
        IBACKLIGHT_Release(pMe->m_pKeypadBacklight);
        pMe->m_pKeypadBacklight = NULL;
    }
#endif
    if(pMe->m_pIAnn)
    {
        IANNUNCIATOR_Release(pMe->m_pIAnn);
        pMe->m_pIAnn = NULL;
    }
    
    if(pMe->m_pIFileMgr)
    {
        IFILEMGR_Release(pMe->m_pIFileMgr);
        pMe->m_pIFileMgr = NULL;
    }

    if (pMe->m_pWallPaper != NULL)
    {
       IIMAGE_Release(pMe->m_pWallPaper);
       pMe->m_pWallPaper = NULL;
    }

    if (pMe->m_pAniPaper != NULL)
    {
       IIMAGE_Release(pMe->m_pAniPaper);
       pMe->m_pAniPaper = NULL;
    }

    //if (pMe->m_pTitleImg != NULL)
    //{
    //   IIMAGE_Release(pMe->m_pTitleImg);
    //   pMe->m_pTitleImg = NULL;
    //}
}

/*==============================================================================
函数:
       DisplayMenu_RunFSM

说明:
       DisplayMenu Applet有限状态机引擎。

参数:
       pMe [in]：指向DisplayMenu Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
       无。

备注:

==============================================================================*/
static void DisplayMenu_RunFSM(CDisplayMenu *pMe)
{
    NextFSMAction nextFSMAction = NFSMACTION_WAIT;

    for ( ; ; )
    {
        nextFSMAction = DisplayMenu_ProcessState(pMe);

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
       DisplayMenu_HandleEvent

说明:
       DisplayMenu  Applet事件处理主函数。所有属于本Applet的事件，
       都将经由该函数处理。

参数:
       pi [in]：指向IDisplayMenu接口t对象的指针。
       eCode：BREW事件代码。
       wParam：与事件相关联的数据。
       dwParam [in]：与事件相关联的数据。

返回值:
       TRUE：传入事件被处理。
       FALSE：传入事件没被处理。

备注:

==============================================================================*/
static boolean DisplayMenu_HandleEvent(IDisplayMenu *pi,
    AEEEvent eCode,
    uint16  wParam,
    uint32 dwParam
)
{
    CDisplayMenu *pMe = (CDisplayMenu*)pi;
    AEEAppStart *as;
    //DISP_ERR("%x, %x ,%x,DisplayMenu_HandleEvent",eCode,wParam,dwParam);
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

            pMe->m_pDisplay = as->pDisplay;
            (void) IDISPLAY_AddRef(pMe->m_pDisplay);
            pMe->m_rc = as->rc;
            pMe->m_rc.y = pMe->m_rc.y - DISPLAY_MENU_SOFK_HIGHT;
            pMe->m_bSuspending = FALSE;

            // 开始DisplayMenu状态机
            DisplayMenu_RunFSM(pMe);
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
            pMe->m_rc.y = pMe->m_rc.y - DISPLAY_MENU_SOFK_HIGHT;
            //挂起返回时浏览的图片为最后一次看到的那张
            pMe->m_DelImageBack = TRUE;

            DisplayMenu_RunFSM(pMe);
            return TRUE;

        case EVT_DIALOG_INIT:
            pMe->m_bAppIsReady = FALSE;
            pMe->m_pActiveDlg = (IDialog*)dwParam;
            pMe->m_pActiveDlgID = wParam;

            return DisplayMenu_RouteDialogEvent(pMe,eCode,wParam,dwParam);

        case EVT_DIALOG_START:
             (void)ISHELL_SetTimer ( pMe->m_pShell,
                            APPISREADY_TIMER,
                            DisplayMenu_APPIsReadyTimer,
                            pMe);
             
            return DisplayMenu_RouteDialogEvent(pMe,eCode,wParam,dwParam);

        case EVT_USER_REDRAW:
            (void) DisplayMenu_RouteDialogEvent(pMe,eCode,wParam,dwParam);

            /*
            (void)ISHELL_SetTimer ( pMe->m_pShell,
                            APPISREADY_TIMER,
                            DisplayMenu_APPIsReadyTimer,
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
			#if !defined(FEATURE_VERSION_K212)
            if (!pMe->m_bAppIsReady)
            {
                return TRUE;
            }
			#endif
            return DisplayMenu_RouteDialogEvent(pMe,eCode,wParam,dwParam);

        case EVT_DIALOG_END:
            if (wParam == 0)
            {
                return TRUE;
            }

            pMe->m_bAppIsReady = FALSE;
            (void) DisplayMenu_RouteDialogEvent(pMe,eCode,wParam,dwParam);
            pMe->m_pActiveDlg = NULL;

            // Applet被挂起，不跑状态机，等待Applet返回。（注意：EVT_APP_SUSPEND
            // 事件在EVT_DIALOG_END事件前发出。
            if (pMe->m_bSuspending == FALSE)
            {
                // 开始DisplayMenu状态机
                DisplayMenu_RunFSM(pMe);
            }
            return TRUE;
#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
		case EVT_PEN_UP:
		//case EVT_PEN_DOWN:
			{
				return DisplayMenu_RouteDialogEvent(pMe,eCode,wParam,dwParam);
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
				else if(wParam == AVK_DOWN)//wlh 换桌面图片
				{
					eCode = EVT_KEY;
				}
				return DisplayMenu_RouteDialogEvent(pMe,eCode,wParam,dwParam);
			}
#endif//FEATURE_LCD_TOUCH_ENABLE
        default:
            // 将接收到的事件路由至当前活动的对话框事件处理函数。
            return DisplayMenu_RouteDialogEvent(pMe,eCode,wParam,dwParam);
    }
}

/*==============================================================================
函数:
       DisplayMenu_APPIsReadyTimer

说明:
       定时器回调函数。主要用于控制快速按键问题的处理。

参数:
       pme [in]：void *类型指针，暗指CDisplayMenu结构指针。

返回值:
       无。

备注:

==============================================================================*/
static void DisplayMenu_APPIsReadyTimer(void *pme)
{
    CDisplayMenu *pMe = (CDisplayMenu *)pme;

    if (NULL == pMe)
    {
        return;
    }

    (void) ISHELL_PostEvent( pMe->m_pShell,
                            AEECLSID_APP_DISPLAYMENU,
                            EVT_APPISREADY,
                            0,
                            0);
}

/****************************************************************/
static double_list *p_wallpaper = NULL; 

static double_list * Display_Creat_Double_list(void)
{
    double_list * temp = NULL;
    DISP_ERR("Display_Creat_Double_list",0,0,0);
    temp = (double_list*) MALLOC(sizeof(double_list));
    if(temp)
    {
        temp->phead = NULL;
        temp->ptail    = NULL;
        temp->pcur   = NULL;
        temp->count =0;
    }
    return temp;
}

static void Display_Free_Double_list(double_list * plist )
{
    image_name_table *list_head = NULL;
    image_name_table *temp = NULL;

    //DISP_ERR("DisplayMenu_Free_Double_list",0,0,0);

    if((plist == NULL) ||(plist->phead == NULL))
    {
        return;
    }
    DISP_ERR("%x Display_Free_Double_list start",plist->phead,0,0);
    list_head = plist->phead ;

    while(list_head)
    {
        temp =list_head->next;
        FREE(list_head);
        list_head = temp;
        temp = NULL;
    }

    list_head = NULL;

}

static void Display_Add_Node_To_Double_list_Tail(double_list * plist ,char * image_name)
{
    image_name_table *temp = NULL;

    DISP_ERR("DisplayMenu_Add_Node_To_Double_list_Tail",0,0,0);

    if((plist == NULL) ||(image_name == NULL))
    {
        return;
    }

    temp = (image_name_table*)MALLOC(sizeof(image_name_table));

    if(temp)
    {
        if(plist->count == 0)/*空的双向链表*/
        {
            temp->next = NULL;
            temp->pref = NULL;
            plist->phead = temp;
            plist->ptail   = temp;
        }
        else
        {
            temp->next = NULL;
            temp->pref = plist->ptail;
            plist->ptail->next = temp;
            plist->ptail  = temp;
        }

        STRCPY(temp->imange_name , image_name);
        plist->count ++ ;
    }
    else
    {
        DISP_ERR("MALLOC FAIL,Cann't add",0,0,0);
    }
}

static image_name_table * Display_Search_Name_In_Double_list(double_list * plist ,char * image_name , uint16 * count)
{
    image_name_table *b_find = NULL;
    image_name_table *temp   = NULL;
    uint16 i = 1;

    //DISP_ERR("DisplayMenu_Search_Name_In_Double_list",0,0,0);

    if(plist == NULL || image_name == NULL)
    {
        return NULL;
    }

    temp =  plist->phead;

    while(temp != NULL)
    {
        if(0 == STRCMP (image_name,temp->imange_name))
        {
            b_find = temp;
            break;
        }
        temp = temp->next;
        i ++ ;
    }

    if(b_find)
    {
        DISP_ERR("ok,find it %d",i,0,0);
    }
    else
    {
        i = 0;
        DISP_ERR("fail,not find",0,0,0);
    }

    *count = i;

    return b_find;
}

static image_name_table * Display_Search_Next_In_Double_list(double_list * plist ,image_name_table * p_cur,uint16 * img_id)
{
    if((p_cur ==plist->phead)&& (p_cur ==plist->ptail)) /*Only one*/
    {
        * img_id = 1;
        return p_cur;
    }
    else if ((p_cur !=plist->phead)&& (p_cur ==plist->ptail))  /*the tail */
    {
        * img_id = 1;
        return plist->phead;
    }
    else
    {
        (* img_id) ++;
        return p_cur->next;
    }

}

static image_name_table * Display_Search_Pref_In_Double_list(double_list * plist ,image_name_table * p_cur,uint16 * img_id)
{
    if((p_cur ==plist->phead)&& (p_cur ==plist->ptail)) /*Only one*/
    {
        * img_id = 1;
        return p_cur;
    }
    else if ((p_cur ==plist->phead)&& (p_cur !=plist->ptail))  /*the head */
    {
        * img_id = plist->count;
        return plist->ptail;
    }
    else
    {
        (* img_id) --;
        return p_cur->pref;
    }

}
/*==============================================================================
函数：
       Display_EnumFile
说明：
       分类枚举出所有的墙纸或动画

参数：
       pMe [in]：指向DisplayMenu Applet对象结构的指针。该结构包含小程序的特定信息。
       PICTURE_TYPE [in]：PIC类型。

返回值：
       void

备注：

==============================================================================*/
static void Display_EnumFile(CDisplayMenu *pMe)
{
    FileInfo  Photopos;
    int         Start_bit = 0; /*判断是墙纸、开机动画还是关机动画*/
    char      PhotoName[AEE_MAX_FILE_NAME/*FILESPECLEN*/];

    p_wallpaper = Display_Creat_Double_list();
    
    if(IFILEMGR_EnumInit(pMe->m_pIFileMgr, "fs:/image/wallpaper/", FALSE)  == SUCCESS)
    {
        while(IFILEMGR_EnumNext(pMe->m_pIFileMgr, &Photopos))
        {                	    
		    //判断是桌面墙纸、开机动画还是关机动画，分别以w、k、g字母开头
		    if(Photopos.attrib & AEE_FA_CONST)
		    {
		    	MEMSET(PhotoName,0,sizeof(PhotoName));                 
		    	STRCPY(PhotoName,Photopos.szName);   /*将列举出的文件名存入PhotoName中*/
		        //DISP_ERR("Photopos.szName: %d,Photopos.attrib: %d",Photopos.szName ,Photopos.attrib,0);
		        if(p_wallpaper)
		        {
		            Display_Add_Node_To_Double_list_Tail(p_wallpaper,PhotoName);
		        }
		    }
         }
    }
}


static void DisplayMenu_ChangePictrue(IDisplayMenu * pme, boolean direction)
{
    CDisplayMenu *pMe = (CDisplayMenu *)pme;
    char wallnv[AEE_MAX_FILE_NAME/*FILESPECLEN*/];
    
    pMe->m_count = 0;

    if (NULL == pMe)
    {
        return;
    }
    
    Display_EnumFile(pMe);
        
    (void) ICONFIG_GetItem(pMe->m_pConfig,
                            CFGI_WALLPAPER,
                            wallnv,
                            sizeof(wallnv));
    
    pMe->m_CurPaper = Display_Search_Name_In_Double_list(p_wallpaper,wallnv,&pMe->m_count);

    if(pMe->m_CurPaper != NULL)
    {
    if(direction)
    {
        pMe->m_CurPaper = Display_Search_Pref_In_Double_list(p_wallpaper,pMe->m_CurPaper,&pMe->m_count);
    }
    else
    {
        pMe->m_CurPaper = Display_Search_Next_In_Double_list(p_wallpaper,pMe->m_CurPaper,&pMe->m_count);
    }
    (void) ICONFIG_SetItem(pMe->m_pConfig,
                            CFGI_WALLPAPER,
                            pMe->m_CurPaper->imange_name,
                            sizeof(pMe->m_CurPaper->imange_name));
    }
    else
    {
        // 初始化墙纸
        MEMSET(wallnv, 0x00, sizeof(wallnv));

        (void) ICONFIG_SetItem(pMe->m_pConfig,
                                CFGI_WALLPAPER,
                                OEMNV_WALLPAPER,
                                sizeof(OEMNV_WALLPAPER));

    }

    Display_Free_Double_list(p_wallpaper);
    if(p_wallpaper)
    {
        FREE(p_wallpaper);
        p_wallpaper = NULL;
    }
    
    if(pMe->m_CurPaper != NULL)
    {
        pMe->m_CurPaper = NULL;
    }
}


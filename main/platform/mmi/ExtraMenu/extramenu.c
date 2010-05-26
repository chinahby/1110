
/*==============================================================================
// 文件：
//        ExtraMenu.c
//        版权所有(c) 2004 Anylook
//
// 文件说明：
//
// 作者：Gemsea
// 创建日期：2007-11-14
// 当前版本：Draft
==============================================================================*/

/*==============================================================================
修改历史记录：
时       间      修 改 人     修改内容、位置及原因
-----------      ----------     -----------------------------------------------
07-11-14                 初始版本
==============================================================================*/


/*==============================================================================
                                 本文件包含的外部文件
==============================================================================*/
#ifndef AEE_SIMULATOR
#include "customer.h"   //Handset Feature Definitions
#else
#include "BREWSimFeatures.h"   //Simulator Feature Definitions
#endif

#if !defined(BREW_STATIC_APP)
// Use the .bid file if compiling as dynamic applet
   #include "extramenu.bid"
#else
   #include "OEMClassIds.h"  // Applet Ids
   #ifndef AEECLSID_EXTRAMENU
      #error AEECLSID_EXTRAMENU must be defined
   #endif
#endif

#include "extramenu.brh"
#include "AEEShell.h"
#include "AEEModTable.h"
#include "AEEStdLib.h"
#include "AEEModGen.h"
#include "AEEAppGen.h"
#include "OEMClassIDs.h"
#include "ExtraMenu.h"
#include "OEMFeatures.h"
#include "AppComFunc.h"
#include "Appscommon.h"

/*==============================================================================
                                 宏定义和常数
==============================================================================*/
// 关闭对话框宏
#define CLOSE_DIALOG(DlgRet)  {    \
                                     pMe->m_eDlgRet = DlgRet; \
                                     (void) ISHELL_EndDialog(pMe->m_pShell);  \
                                 }

// 为防止用户快速按键而添加事件EVT_APPISREADY，事件由定时器
// ExtraMenu_APPIsReadyTimer发出
#define APPISREADY_TIMER       100                     
#define EVT_APPISREADY   (EVT_USER+1)                  

// 用户重新更新界面事件

// 状态移动宏：先将前一状态用当前状态更新，再将当前状态设为nextState
#define MOVE_TO_STATE(nextState) {            \
            ExtraMenuState tpState;            \
            tpState = nextState;          \
            pMe->m_ePreState = pMe->m_eCurState; \
            pMe->m_eCurState = tpState;        \
        }


#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
#define EXTRAMENU_PT_IN_RECT(a,b,rct)      (boolean)( ((a) >= (rct).x && (a) <= ((rct).x + (rct).dx)) && ((b) >= (rct).y && (b) <= ((rct).y + (rct).dy)) )
#endif//FEATURE_LCD_TOUCH_ENABLE
/*==============================================================================
                                 类型定义
==============================================================================*/
/*----------------------对话框相关数据类型声明---------------------*/
// 对话框关闭时返回值列表
typedef enum DLGRetValue
{
   // 初始值，表明需要创建对话框
   DLGRET_CREATE,

   // 对话框关闭时可返回的通用值
   DLGRET_OK,
   DLGRET_CANCELED
} DLGRet_Value_e_Type;

/*----------------------状态机相关数据类型声明---------------------*/
// ExtraMenu Applet 状态机状态：
typedef enum _ExtraMenuState
{
   EXTRAMENUST_NONE,
   EXTRAMENUST_INIT,
   EXTRAMENUST_MAIN,
   EXTRAMENUST_EXIT,

} ExtraMenuState;

// 状态处理函数返回给状态处理主函数的值类型
typedef enum NextFSMAction
{
   NFSMACTION_WAIT,
   NFSMACTION_CONTINUE
} NextFSMAction;

// 保存模块信息和模块引用计数的结构
typedef struct ExtraMenuMod
{
    DECLARE_VTBL(IModule)
    uint32   m_nRefs;
} ExtraMenuMod;

// ExtraMenu Applet对象结构体：
typedef struct _CExtraMenu
{
    DECLARE_VTBL(IExtraMenu)
    uint32          m_nRefs;          // 对象引用计数器
    IModule        *m_pModule;
    IDisplay        *m_pDisplay;
    IShell            *m_pShell;

    AEERect       m_rc;
    // 当前活动对话框IDialog接口指针
    IDialog             *m_pActiveDlg;
    // 当前活动对话框ID
    uint16 m_pActiveDlgID;
    // 活动对话框关闭时的返回结果
    DLGRet_Value_e_Type   m_eDlgRet;
    // 是否不改写对话框返回结果，一般情况需要改写(FALSE)。
    boolean m_bNotOverwriteDlgRet;
    // Applet 前一状态
    ExtraMenuState m_ePreState;
    // Applet 当前状态
    ExtraMenuState m_eCurState;
    // Applet是否处于挂起状态
    boolean m_bSuspending;
    // Applet是否准备好可以处理按键和命令事件。改变量主要用于快速按键引起的问题
    boolean         m_bAppIsReady;

    uint16          m_currDlgId;       //保存当前所选菜单位置
} CExtraMenu;


/*==============================================================================
                                 函数声明
==============================================================================*/
static void ExtraMenu_APPIsReadyTimer(void *pme);

// 对话框 IDD_EXTRAS_MENU 事件处理函数
static boolean  HandleMainDialogEvent(CExtraMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

// 状态 EXTRAMENUST_NONE 处理函数
static NextFSMAction StateNoOperationHandler(CExtraMenu *pMe);

// 状态 EXTRAMENUST_INIT 处理函数
static NextFSMAction StateInitHandler(CExtraMenu *pMe);

// 状态 EXTRAMENUST_MAIN 处理函数
static NextFSMAction StateMainHandler(CExtraMenu *pMe);

// 状态 EXTRAMENUST_EXIT 处理函数
static NextFSMAction StateExitHandler(CExtraMenu *pMe);


/*----------------------对话框相关函数声明---------------------*/
/*==============================================================================
函数:
       ExtraMenu_ShowDialog
说明:
       函数由状态处理函数用来创建相应对话框资源ID的对话框。

参数:
       pMe [In]: 指向ExtraMenu Applet对象结构的指针,该结构包含小程序的特定信息.
       dlgResId [in]：对话框资源ID，函数根据该ID创建对话框。

返回值:
       无

备注:


==============================================================================*/
void ExtraMenu_ShowDialog(CExtraMenu *pMe,uint16  dlgResId);

/*==============================================================================
函数:
       ExtraMenu_RouteDialogEvent
说明:
       函数将BREW事件路由给当前活动对话框的事件处理函数。

参数:
       pMe [In]: 指向ExtraMenu Applet对象结构的指针,该结构包含小程序的特定信息.
       eCode [in]：事件代码。
       wParam [in]：与事件eCode关联的数据。
       dwParam [in]：与事件eCode关联的数据。

返回值:
       TRUE：传入事件被处理。
       FALSE：传入事件没被处理。

备注:

==============================================================================*/
boolean ExtraMenu_RouteDialogEvent(CExtraMenu *pMe,
    AEEEvent    eCode,
    uint16  wParam,
    uint32 dwParam
);

/*----------------------状态机相关函数声明---------------------*/
/*==============================================================================
函数:
       ExtraMenu_ProcessState
说明:
       ExtraMenu Applet状态处理主函数。函数根据pMe->m_currState将调用路由至
       相应的处理函数。

参数:
       pMe [In]: 指向ExtraMenu Applet对象结构的指针,该结构包含小程序的特定信息.

返回值:
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注:

==============================================================================*/
NextFSMAction ExtraMenu_ProcessState(CExtraMenu *pMe);

/*----------------------模块相关函数声明---------------------*/

int  ExtraMenuMod_Load(IShell   *pIShell,
    void     *ph,
    IModule **ppMod
);

int  ExtraMenuMod_New(int16  nSize,
    IShell   *pIShell,
    void     *ph,
    IModule  **ppMod,
    PFNMODCREATEINST pfnMC,
    PFNFREEMODDATA pfnMF
);

static int  ExtraMenuMod_CreateInstance(IModule *po,
    IShell *pIShell,
    AEECLSID ClsId,
    void **ppObj
);

static uint32 ExtraMenuMod_AddRef(IModule *po);

static uint32 ExtraMenuMod_Release(IModule *po);

static void ExtraMenuMod_FreeResources(IModule  *po,
    IHeap  *ph,
    IFileMgr *pfm
);

/*----------------------Applet相关函数声明---------------------*/
static int ExtraMenu_New(IShell *ps,
    IModule *pIModule,
    IExtraMenu **ppObj
);

static uint32  ExtraMenu_AddRef(IExtraMenu *pi);

static uint32  ExtraMenu_Release (IExtraMenu *pi);

static boolean ExtraMenu_HandleEvent(IExtraMenu *pi,
    AEEEvent eCode,
    uint16  wParam,
    uint32 dwParam
);

static int ExtraMenu_InitAppData(CExtraMenu *pMe);

static void ExtraMenu_FreeAppData(CExtraMenu *pMe);

static void ExtraMenu_RunFSM(CExtraMenu *pMe);

/*==============================================================================
                                 全局数据
==============================================================================*/
// 重要提示：
//        g_dwAEEStdLibEntry全局变量仅用于SDK ！
//        BREW SDK用户不能改写和直接使用本变量。它用作AEEStdLib的一个入口点，不
//        要移动下列三行代码。
#ifdef AEE_SIMULATOR
   uint32 g_dwAEEStdLibEntry;
#ifndef AEE_STATIC //warning for dynamic building only
   #error This applet was never intended to be used with the AEE Simulator.
#endif
#endif

/*==============================================================================
                                 本地（静态）数据
==============================================================================*/
static ExtraMenuMod gExtraMenuMod;

static const VTBL(IModule) gModFuncs =
{
    ExtraMenuMod_AddRef,
    ExtraMenuMod_Release,
    ExtraMenuMod_CreateInstance,
    ExtraMenuMod_FreeResources
};

// 只允许一个ExtraMenu实例。每次创建ExtraMenu Applet时，
// 返回同一结构指针给BREW层。
static CExtraMenu gExtraMenu;

static const VTBL(IExtraMenu) gExtraMenuMethods =
{
    ExtraMenu_AddRef,
    ExtraMenu_Release,
    ExtraMenu_HandleEvent
};

/*----------------------模块相关函数定义---------------------*/

/*==============================================================================
函数:
       ExtraMenuMod_Load

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
#if defined(AEE_SIMULATOR)&&!defined( AEE_STATIC) //for dynamic building only
__declspec(dllexport) int AEEMod_Load(IShell   *pIShell,
                                                        void     *ph,
                                                        IModule **ppMod)
#else
int ExtraMenuMod_Load(IShell   *pIShell,
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

    return ExtraMenuMod_New(sizeof(ExtraMenuMod),
                    pIShell,
                    ph,
                    ppMod,
                    NULL,
                    NULL);
}

/*==============================================================================
函数:
       ExtraMenuMod_New

说明:
       用来初始化ExtraMenu模块虚拟表（也增加引用计数）的助手函数。

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
int  ExtraMenuMod_New(int16  nSize,
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

    MEMSET(&gExtraMenuMod,0,sizeof(ExtraMenuMod));

    INIT_VTBL(&gExtraMenuMod,IModule,gModFuncs);
    gExtraMenuMod.m_nRefs = 1;
    *ppMod = (IModule *)&gExtraMenuMod;
    return AEE_SUCCESS;
}

/*==============================================================================
函数:
       ExtraMenuMod_AddRef

说明:
       ExtraMenu 模块引用计数加1，并返回当前的引用数。

参数:
       po [in]：IModule接口对象指针。

返回值:
       当前对ExtraMenu 模块的引用数

备注:

==============================================================================*/
static uint32 ExtraMenuMod_AddRef(IModule *po)
{
    return(++((ExtraMenuMod *)po)->m_nRefs);
}

/*==============================================================================
函数:
       ExtraMenuMod_Release

说明:
       ExtraMenu 模块引用计数减1，并返回当前的引用数。

参数:
       po [in]：IModule接口对象指针。

返回值:
       当前对ExtraMenu 模块的引用数

备注:

==============================================================================*/
static uint32 ExtraMenuMod_Release(IModule *po)
{
    if (((ExtraMenuMod *) po)->m_nRefs == 0)
    {
        return 0;
    }

    return(--((ExtraMenuMod *)po)->m_nRefs);
}

/*==============================================================================
函数:
       ExtraMenuMod_CreateInstance

说明:
        通过创建(初始化)ExtraMenu对象来创建模块实例。

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
static int  ExtraMenuMod_CreateInstance(IModule *po,
    IShell *pIShell,
    AEECLSID ClsId,
    void **ppObj
)
{
    *ppObj = NULL;

    if (ClsId != AEECLSID_EXTRAMENU)
    {
        return EFAILED;
    }

    if (ExtraMenu_New(pIShell, po, (IExtraMenu**)ppObj) != SUCCESS)
    {
        return EFAILED;
    }
    return SUCCESS;
}

/*==============================================================================
函数:
       ExtraMenuMod_FreeResources

说明:
        释放ExtraMenu模块资源的函数。

参数:
       po ：IModule。
       ph ：IHeap接口指针。
       pfm :IFileMgr接口指针。

返回值:
       无

备注:
       不需用户去调用。

==============================================================================*/
static void ExtraMenuMod_FreeResources(IModule  *po,
    IHeap  *ph,
    IFileMgr *pfm
)
{

}

/*----------------------Applet相关函数定义---------------------*/
/*==============================================================================
函数:
       ExtraMenu_New

说明:
        初始化ExtraMenu Applet虚拟表和内部数据变量，同时其引用计数加1。

参数:
       ps [in]：IShell接口对象指针。
       pIModule [in]：pIModule接口对象指针。
       ppObj [out]:：指向IExtraMenu对象指针的指针。

返回值:
       EFAILED：失败。
       SUCCESS：成功。

备注:
       不需用户去调用。
       关于Applet结构对象可使用动态分配内存的方式创建，本处采用的是使用静态
       变量的方式。建议对结构比较大的Applet使用静态变量的方式。对于动态分配
       内存的方式需要注意对所分配空间的释放。

==============================================================================*/
static int ExtraMenu_New(IShell *ps,
    IModule *pIModule,
    IExtraMenu **ppObj
)
{
    int retVal = SUCCESS;

    if (0 == gExtraMenu.m_nRefs)
    {
        // Must initialize the object
        MEMSET(&gExtraMenu,  0, sizeof(CExtraMenu));
        INIT_VTBL(&gExtraMenu, IExtraMenu, gExtraMenuMethods);

        gExtraMenu.m_nRefs     = 0;
        gExtraMenu.m_pShell    = ps;
        gExtraMenu.m_pModule   = pIModule;

        (void) ISHELL_AddRef(ps);
        (void) IMODULE_AddRef(pIModule);

        retVal = ExtraMenu_InitAppData(&gExtraMenu);
    }

    ++gExtraMenu.m_nRefs;
    *ppObj = (IExtraMenu*)&gExtraMenu;
    return retVal;
}

/*==============================================================================
函数:
       ExtraMenu_AddRef

说明:
       ExtraMenu Applet 引用计数加1，并返回当前的引用数。

参数:
       pi [in]：IExtraMenu接口对象指针。

返回值:
       当前对ExtraMenu Applet的引用数

备注:

==============================================================================*/
static uint32  ExtraMenu_AddRef(IExtraMenu *pi)
{
    register CExtraMenu *pMe = (CExtraMenu*)pi;

    return (++pMe->m_nRefs);
}

/*==============================================================================
函数:
       ExtraMenu_Release

说明:
       释放对ExtraMenu Applet的引用，引用计数减1，并返回当前的引用数。
        如果当前的引用数为零，还将释放Applet及相关资源。

参数:
       pi [in]：IExtraMenu接口对象指针。

返回值:
       当前对ExtraMenu Applet的引用数

备注:

==============================================================================*/
static uint32  ExtraMenu_Release (IExtraMenu *pi)
{
    register CExtraMenu *pMe = (CExtraMenu*)pi;

    if (pMe->m_nRefs == 0)
    {
        return 0;
    }

    if (--pMe->m_nRefs)
    {
        return pMe->m_nRefs;
    }

    // 释放Applet相关资源
    ExtraMenu_FreeAppData(pMe);

    (void) ISHELL_Release(pMe->m_pShell);
    (void) IMODULE_Release(pMe->m_pModule);

    // 注意：pMe是静态分配空间，因此不需要释放。FREE()
    return 0;
}

/*==============================================================================
函数:
       ExtraMenu_InitAppData

说明:
       初始化ExtraMenu Applet结构数据缺省值。

参数:
       pMe [in]：指向ExtraMenu Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
       SUCCESS：初始化成功。
       EFAILED：初始化失败。

备注:

==============================================================================*/
static int ExtraMenu_InitAppData(CExtraMenu *pMe)
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
    pMe->m_ePreState = EXTRAMENUST_NONE;
    pMe->m_eCurState = EXTRAMENUST_INIT;
    pMe->m_pActiveDlg = NULL;
    pMe->m_eDlgRet = DLGRET_CREATE;
    pMe->m_bNotOverwriteDlgRet = FALSE;

    pMe->m_currDlgId = 0;

    return SUCCESS;
}

/*==============================================================================
函数:
       ExtraMenu_FreeAppData

说明:
       释放ExtraMenu Applet使用的相关资源。

参数:
       pMe [in]：指向ExtraMenu Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
       无。

备注:

==============================================================================*/
static void ExtraMenu_FreeAppData(CExtraMenu *pMe)
{
    if (NULL == pMe)
    {
        return ;
    }

}

/*==============================================================================
函数:
       ExtraMenu_RunFSM

说明:
       ExtraMenu Applet有限状态机引擎。

参数:
       pMe [in]：指向ExtraMenu Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
       无。

备注:

==============================================================================*/
static void ExtraMenu_RunFSM(CExtraMenu *pMe)
{
    NextFSMAction nextFSMAction = NFSMACTION_WAIT;

    for ( ; ; )
    {
        nextFSMAction = ExtraMenu_ProcessState(pMe);

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
       ExtraMenu_HandleEvent

说明:
       ExtraMenu  Applet事件处理主函数。所有属于本Applet的事件，
       都将经由该函数处理。

参数:
       pi [in]：指向IExtraMenu接口t对象的指针。
       eCode：BREW事件代码。
       wParam：与事件相关联的数据。
       dwParam [in]：与事件相关联的数据。

返回值:
       TRUE：传入事件被处理。
       FALSE：传入事件没被处理。

备注:

==============================================================================*/
static boolean ExtraMenu_HandleEvent(IExtraMenu *pi,
    AEEEvent eCode,
    uint16  wParam,
    uint32 dwParam
)
{
    CExtraMenu *pMe = (CExtraMenu*)pi;
    AEEAppStart *as;

    switch (eCode)
    {
        case EVT_APP_START:
            pMe->m_bAppIsReady = FALSE;
            
            as = (AEEAppStart*)dwParam;
            if (NULL != pMe->m_pDisplay)
            {
                (void) IDISPLAY_Release(pMe->m_pDisplay);
                pMe->m_pDisplay = NULL;
            }
            pMe->m_pDisplay = as->pDisplay;
            (void) IDISPLAY_AddRef(pMe->m_pDisplay);
            pMe->m_rc = as->rc;

            pMe->m_bSuspending = FALSE;

            // 开始ExtraMenu状态机
            ExtraMenu_RunFSM(pMe);
            return TRUE;

        case EVT_APP_STOP:
            if (pMe->m_pDisplay != NULL)
            {
                (void) IDISPLAY_Release(pMe->m_pDisplay);
                pMe->m_pDisplay = NULL;
            }
            (void)ISHELL_CancelTimer(pMe->m_pShell, NULL, pMe);
            pMe->m_bSuspending = TRUE;

            return TRUE;

        case EVT_APP_SUSPEND:
            pMe->m_bSuspending = TRUE;
            (void)ISHELL_CancelTimer(pMe->m_pShell, NULL, pMe);
            return TRUE;

        case EVT_APP_RESUME:
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

            ExtraMenu_RunFSM(pMe);
            return TRUE;

        case EVT_DIALOG_INIT:
            pMe->m_bAppIsReady = FALSE;
            pMe->m_pActiveDlg = (IDialog*)dwParam;
            pMe->m_pActiveDlgID = wParam;

            return ExtraMenu_RouteDialogEvent(pMe,eCode,wParam,dwParam);

        case EVT_DIALOG_START:
            return ExtraMenu_RouteDialogEvent(pMe,eCode,wParam,dwParam);

        case EVT_USER_REDRAW:
            (void) ExtraMenu_RouteDialogEvent(pMe,eCode,wParam,dwParam);
            (void)ISHELL_SetTimer ( pMe->m_pShell,
                            APPISREADY_TIMER,
                            ExtraMenu_APPIsReadyTimer,
                            pMe);
            return TRUE;

        case EVT_APPISREADY:
            (void) ExtraMenu_RouteDialogEvent(pMe,eCode,wParam,dwParam);
            pMe->m_bAppIsReady = TRUE;
            return TRUE;

        case EVT_KEY_PRESS:
            if (!pMe->m_bAppIsReady)
            {
                return TRUE;
            }
            return ExtraMenu_RouteDialogEvent(pMe,eCode,wParam,dwParam);

        case EVT_KEY_RELEASE:
            if (!pMe->m_bAppIsReady)
            {
                return TRUE;
            }
            return ExtraMenu_RouteDialogEvent(pMe,eCode,wParam,dwParam);

        case EVT_KEY_HELD:
            if (!pMe->m_bAppIsReady)
            {
                return TRUE;
            }
            return ExtraMenu_RouteDialogEvent(pMe,eCode,wParam,dwParam);

        case EVT_KEY:
            if (!pMe->m_bAppIsReady)
            {
                return TRUE;
            }
            return ExtraMenu_RouteDialogEvent(pMe,eCode,wParam,dwParam);

        case EVT_COMMAND:
            if (!pMe->m_bAppIsReady)
            {
                return TRUE;
            }
            return ExtraMenu_RouteDialogEvent(pMe,eCode,wParam,dwParam);

        case EVT_DIALOG_END:
            if (wParam == 0)
            {
                return TRUE;
            }
            pMe->m_bAppIsReady = FALSE;
            (void) ExtraMenu_RouteDialogEvent(pMe,eCode,wParam,dwParam);
            pMe->m_pActiveDlg = NULL;

            // Applet被挂起，不跑状态机，等待Applet返回。（注意：EVT_APP_SUSPEND
            // 事件在EVT_DIALOG_END事件前发出。
            if (pMe->m_bSuspending == FALSE)
            {
                // 开始ExtraMenu状态机
                ExtraMenu_RunFSM(pMe);
            }
            return TRUE;
#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
		case EVT_PEN_UP:
		//case EVT_PEN_DOWN:
			{
				return ExtraMenu_RouteDialogEvent(pMe,eCode,wParam,dwParam);
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
			}
#endif//FEATURE_LCD_TOUCH_ENABLE
        default:
            // 将接收到的事件路由至当前活动的对话框事件处理函数。
            return ExtraMenu_RouteDialogEvent(pMe,eCode,wParam,dwParam);
    }
}

/*==============================================================================
函数:
       ExtraMenu_APPIsReadyTimer

说明:
       定时器回调函数。主要用于控制快速按键问题的处理。

参数:
       pme [in]：void *类型指针，暗指CExtraMenu结构指针。

返回值:
       无。

备注:

==============================================================================*/
static void ExtraMenu_APPIsReadyTimer(void *pme)
{
    CExtraMenu *pMe = (CExtraMenu *)pme;

    if (NULL == pMe)
    {
        return;
    }

    (void) ISHELL_PostEvent( pMe->m_pShell,
                            AEECLSID_EXTRAMENU,
                            EVT_APPISREADY,
                            0,
                            0);
}


/*==============================================================================
函数:
       ExtraMenu_ShowDialog
说明:
       函数由状态处理函数用来创建相应对话框资源ID的对话框。

参数:
       pMe [In]: 指向ExtraMenu Applet对象结构的指针,该结构包含小程序的特定信息.
       dlgResId [in]：对话框资源ID，函数根据该ID创建对话框。

返回值:
       无

备注:


==============================================================================*/
void ExtraMenu_ShowDialog(CExtraMenu *pMe,uint16  dlgResId)
{
    int nRet;

    // At most one dialog open at once
    if (ISHELL_GetActiveDialog(pMe->m_pShell) != NULL)
    {
        // Looks like there is one dialog already opened.
        // Flag an error an return without doing anything.
        return;
    }

    nRet = ISHELL_CreateDialog(pMe->m_pShell,AEE_EXTRAMENU_RES_FILE,dlgResId,NULL);
}

/*==============================================================================
函数:
       ExtraMenu_RouteDialogEvent
说明:
       函数将BREW事件路由给当前活动对话框的事件处理函数。

参数:
       pMe [In]: 指向ExtraMenu Applet对象结构的指针,该结构包含小程序的特定信息.
       eCode [in]：事件代码。
       wParam [in]：与事件eCode关联的数据。
       dwParam [in]：与事件eCode关联的数据。

返回值:
       TRUE：传入事件被处理。
       FALSE：传入事件没被处理。

备注:

==============================================================================*/
boolean ExtraMenu_RouteDialogEvent(CExtraMenu *pMe,
    AEEEvent    eCode,
    uint16  wParam,
    uint32 dwParam
)
{
    if (NULL == pMe)
    {
        return FALSE;
    }

    if (NULL == pMe->m_pActiveDlg)
    {
        return FALSE;
    }

    switch (pMe->m_pActiveDlgID)
    {
        case IDD_EXTRA_DIALOG:
            return HandleMainDialogEvent(pMe,eCode,wParam,dwParam);

        default:
            return FALSE;
    }
}




/*==============================================================================
函数:
       ExtraMenu_ProcessState
说明:
       ExtraMenu Applet状态处理主函数。函数根据pMe->m_currState将调用路由至
       相应的处理函数。

参数:
       pMe [In]: 指向ExtraMenu Applet对象结构的指针,该结构包含小程序的特定信息.

返回值:
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注:

==============================================================================*/
NextFSMAction ExtraMenu_ProcessState(CExtraMenu *pMe)
{
    NextFSMAction retVal = NFSMACTION_WAIT;

    if (NULL == pMe)
    {
        return retVal;
    }

    // 根据当前状态调用相应的状态处理函数
    switch (pMe->m_eCurState)
    {
        case EXTRAMENUST_NONE:
            retVal = StateNoOperationHandler(pMe);
            break;

        case EXTRAMENUST_INIT:
            retVal = StateInitHandler(pMe);
            break;

        case EXTRAMENUST_MAIN:
            retVal = StateMainHandler(pMe);
            break;

        case EXTRAMENUST_EXIT:
            retVal = StateExitHandler(pMe);
            break;


        default:
            break;
    }
    return retVal;
}

/*==============================================================================
函数：
       StateNoOperationHandler
说明：
       EXTRAMENUST_NONE 状态处理函数

参数：
       pMe [in]：指向ExtraMenu Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/
static NextFSMAction StateNoOperationHandler(CExtraMenu *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    MOVE_TO_STATE(EXTRAMENUST_EXIT)
    return NFSMACTION_CONTINUE;
} // StateNoOperationHandler


/*==============================================================================
函数：
       StateInitHandler
说明：
       EXTRAMENUST_INIT 状态处理函数

参数：
       pMe [in]：指向ExtraMenu Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/
static NextFSMAction StateInitHandler(CExtraMenu *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    MOVE_TO_STATE(EXTRAMENUST_MAIN)
    return NFSMACTION_CONTINUE;
} // StateInitHandler


/*==============================================================================
函数：
       StateMainHandler
说明：
       EXTRAMENUST_MAIN 状态处理函数

参数：
       pMe [in]：指向ExtraMenu Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/
static NextFSMAction StateMainHandler(CExtraMenu *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            ExtraMenu_ShowDialog(pMe, IDD_EXTRA_DIALOG);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
            MOVE_TO_STATE(EXTRAMENUST_EXIT)
            return NFSMACTION_CONTINUE;

        default:
            break;
    }

    return NFSMACTION_WAIT;
} // StateMainHandler


/*==============================================================================
函数：
       StateExitHandler
说明：
       EXTRAMENUST_EXIT 状态处理函数

参数：
       pMe [in]：指向ExtraMenu Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/
static NextFSMAction StateExitHandler(CExtraMenu *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    (void) ISHELL_CloseApplet(pMe->m_pShell, FALSE);

    return NFSMACTION_WAIT;
} // StateExitHandler


/*==============================================================================
函数：
       HandleMainDialogEvent
说明：
       IDD_EXTRAS_MENU对话框事件处理函数

参数：
       pMe [in]：指向ExtraMenu Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/
static boolean  HandleMainDialogEvent(CExtraMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    static boolean isSelectByNumberKey = FALSE;
    PARAM_NOT_REF(dwParam)

    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,
                                                    IDC_EXTRA_DIALOG);
    if (pMenu == NULL)
    {
        return FALSE;
    }

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
#ifdef FEATURE_SUPPORT_BT_APP
            IMENUCTL_AddItem(pMenu, AEE_EXTRAMENU_RES_FILE, IDS_BLUETOOTH_TITLE,  IDS_BLUETOOTH_TITLE, NULL, 0);
#endif
            IMENUCTL_AddItem(pMenu, AEE_EXTRAMENU_RES_FILE, IDS_ALARMCLOCK_TITLE,  IDS_ALARMCLOCK_TITLE, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_EXTRAMENU_RES_FILE, IDS_CALENDAR_TITLE, IDS_CALENDAR_TITLE, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_EXTRAMENU_RES_FILE, IDS_CALCULATOR_TITLE,  IDS_CALCULATOR_TITLE, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_EXTRAMENU_RES_FILE, IDS_STOPWATCH_TITLE,  IDS_STOPWATCH_TITLE, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_EXTRAMENU_RES_FILE, IDS_WORLDTIME_TITLE,  IDS_WORLDTIME_TITLE, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_EXTRAMENU_RES_FILE, IDS_CONVERTER_TITLE, IDS_CONVERTER_TITLE, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_EXTRAMENU_RES_FILE, IDS_TIMEER_TITLE, IDS_TIMEER_TITLE, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_EXTRAMENU_RES_FILE, IDS_RECORDER_TITLE,  IDS_RECORDER_TITLE, NULL, 0);

#ifdef FEATURE_MORSE
            IMENUCTL_AddItem(pMenu, AEE_EXTRAMENU_RES_FILE, IDS_MORSE_TITLE,  IDS_MORSE_TITLE, NULL, 0);
#endif
#if defined( FEATURE_JEWISH_CALENDAR)
            IMENUCTL_AddItem(pMenu, AEE_EXTRAMENU_RES_FILE, IDS_JEWISH_CALENDAR_TITLE, IDS_JEWISH_CALENDAR_TITLE, NULL, 0);
#endif
            return TRUE;

        case EVT_DIALOG_START:
            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);
            IMENUCTL_SetOemProperties( pMenu, OEMMP_USE_MENU_STYLE);
            IMENUCTL_SetBottomBarType( pMenu, BTBAR_SELECT_BACK);

            (void) ISHELL_PostEvent( pMe->m_pShell,
                                     AEECLSID_EXTRAMENU,
                                     EVT_USER_REDRAW,
                                     0,
                                     0);
            return TRUE;

        case EVT_USER_REDRAW:
            IMENUCTL_SetSel(pMenu, pMe->m_currDlgId);
            // 统一更新界面
            IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            (void)IMENUCTL_Redraw(pMenu);
            return TRUE;

        case EVT_DIALOG_END:
            if(pMe->m_bSuspending)
            {
                if(!isSelectByNumberKey)
                {
                    pMe->m_currDlgId = IMENUCTL_GetSel(pMenu);
                }
                else
                {
                    isSelectByNumberKey = FALSE;
                }
            }
            else
            {
                pMe->m_currDlgId = 0;
            }
            //end added
            return TRUE;

        case EVT_KEY:
            if(wParam == AVK_CLR)
               CLOSE_DIALOG(DLGRET_CANCELED)
            return TRUE;

        case EVT_COMMAND:
            //pMe->m_currDlgId = IMENUCTL_GetSel(pMenu);
            pMe->m_currDlgId = wParam;
            if(wParam != IMENUCTL_GetSel(pMenu))
            {
                isSelectByNumberKey = TRUE;
            }
            switch (wParam)
            {
#ifdef FEATURE_SUPPORT_BT_APP
                case IDS_BLUETOOTH_TITLE:
                    ISHELL_StartApplet(pMe->m_pShell, AEECLSID_BTUIAPP);
                    break;
#endif
                case IDS_ALARMCLOCK_TITLE:
                    ISHELL_StartApplet(pMe->m_pShell, AEECLSID_ALARMCLOCK);
                    break;

                case IDS_CALENDAR_TITLE:
                    ISHELL_StartApplet(pMe->m_pShell,AEECLSID_SCHEDULEAPP);
                    break;

                case IDS_CALCULATOR_TITLE:
                    ISHELL_StartApplet(pMe->m_pShell, AEECLSID_CALCAPP);
                    break;

                case IDS_STOPWATCH_TITLE:
                    ISHELL_StartApplet(pMe->m_pShell, AEECLSID_STOPWATCH);
                    break;

                case IDS_WORLDTIME_TITLE:
                    ISHELL_StartApplet(pMe->m_pShell, AEECLSID_WORLDTIME);
                    break;
                    
                case IDS_CONVERTER_TITLE:
                    ISHELL_StartApplet(pMe->m_pShell, AEECLSID_CONVERTER);
                    break;
                    
                case IDS_TIMEER_TITLE:
                    ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APPTIMER);
                    break;
                    

#ifdef FEATURE_MORSE
                case IDS_MORSE_TITLE:
                    ISHELL_StartApplet(pMe->m_pShell, AEECLSID_MORSE);
                    break;
#endif

#if defined( FEATURE_JEWISH_CALENDAR)
                case IDS_JEWISH_CALENDAR_TITLE:
                    ISHELL_StartApplet( pMe->m_pShell, AEECLSID_JEWISH_CALENDAR);
                    break;
#endif

#if defined( FEATURE_RECORDER)
                case IDS_RECORDER_TITLE:
                    ISHELL_StartApplet( pMe->m_pShell, AEECLSID_RECORDER);
                    break;
#endif

                default:
                    break;
            }
            return TRUE;
#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
		case EVT_PEN_UP:
			{
				AEEDeviceInfo devinfo;
				int nBarH ;
				AEERect rc;
				int16 wXPos = (int16)AEE_GET_X(dwParam);
				int16 wYPos = (int16)AEE_GET_Y(dwParam);

				nBarH = GetBottomBarHeight(pMe->m_pDisplay);
        
				MEMSET(&devinfo, 0, sizeof(devinfo));
				ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
				SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);

				if(EXTRAMENU_PT_IN_RECT(wXPos,wYPos,rc))
				{
					if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
					{
						boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_EXTRAMENU,EVT_USER,AVK_SELECT,0);
						return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)   && wXPos < rc.x + (rc.dx/3)*2 )//左
					{
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_EXTRAMENU,EVT_USER,AVK_INFO,0);
						 return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
					{						
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_EXTRAMENU,EVT_USER,AVK_CLR,0);
						 return rt;
					}
				}
			}
			break;
#endif //FEATURE_LCD_TOUCH_ENABLE
        default:
            break;
    }
    return FALSE;
} // HandleMainDialogEvent


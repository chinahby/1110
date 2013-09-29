
/*==============================================================================
// 文件：
//        NumberManager.c
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
08-12-13                 初始版本
==============================================================================*/


/*==============================================================================
                                 本文件包含的外部文件
==============================================================================*/
#ifndef AEE_SIMULATOR
#include "customer.h"   //Handset Feature Definitions
#else
#include "BREWSimFeatures.h"   //Simulator Feature Definitions
#endif
#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
#include "OEMFeatures.h"
#endif
#if !defined(BREW_STATIC_APP)
// Use the .bid file if compiling as dynamic applet
   #include "NumberManager.bid"
#else
   #include "OEMClassIds.h"  // Applet Ids
   #ifndef AEECLSID_NUMBERMANAGER
      #error AEECLSID_NUMBERMANAGER must be defined
   #endif
#endif

#include "numbermanager.brh"
#include "AEEShell.h"
#include "AEEModTable.h"
#include "AEEStdLib.h"
#include "AEEModGen.h"
#include "AEEAppGen.h"
#include "OEMClassIDs.h"
#include "NumberManager.h"
#include "OEMFeatures.h"
#include "AppComFunc.h"
#include "Appscommon.h"
#include "OEMText.h"
#include "CallApp.h"
#include "Appscommon_color.brh"
/*==============================================================================
                                 宏定义和常数
==============================================================================*/
// 关闭对话框宏
#define CLOSE_DIALOG(DlgRet)  {    \
                                     pMe->m_eDlgRet = DlgRet; \
                                     (void) ISHELL_EndDialog(pMe->m_pShell);  \
                                 }

// 为防止用户快速按键而添加事件EVT_APPISREADY，事件由定时器
// NumberManager_APPIsReadyTimer发出
#define APPISREADY_TIMER       100
#define EVT_APPISREADY      (EVT_USER + 30)

// 状态移动宏：先将前一状态用当前状态更新，再将当前状态设为nextState
#define MOVE_TO_STATE(nextState) {            \
            NumberManagerState tpState;            \
            tpState = nextState;          \
            pMe->m_ePreState = pMe->m_eCurState; \
            pMe->m_eCurState = tpState;        \
        }

// 画界面底部提示条宏定义
#define NUMBERMANAGER_DRAW_BOTTOMBAR(x)                           \
{                                                   \
    BottomBar_Param_type BarParam;                  \
    MEMSET(&BarParam, 0, sizeof(BarParam));         \
    BarParam.eBBarType = x;                         \
    DrawBottomBar(pMe->m_pDisplay, &BarParam);      \
}

#define NUMBERMANAGER_CFG_VERSION 1
#define MAX_INPUT_NUM 32
#define MAX_INPUT_URL 128

#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch 
#define NUMBERMANAGER_PT_IN_RECT(a,b,rct)      (boolean)( ((a) >= (rct).x && (a) <= ((rct).x + (rct).dx)) && ((b) >= (rct).y && (b) <= ((rct).y + (rct).dy)) )
#endif
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
   DLGRET_CANCELED,
   DLGRET_TEXT_INPUT
} DLGRet_Value_e_Type;

/*----------------------状态机相关数据类型声明---------------------*/
// NumberManager Applet 状态机状态：
typedef enum _NumberManagerState
{
   NUMBERMANAGERST_NONE,
   NUMBERMANAGERST_INIT,
   NUMBERMANAGERST_MAIN,
   NUMBERMANAGERST_DISPLAY_NUMBER,
   NUMBERMANAGERST_TEXT_INPUT,
   NUMBERMANAGERST_EXIT,       
} NumberManagerState;

// 状态处理函数返回给状态处理主函数的值类型
typedef enum NextFSMAction
{
   NFSMACTION_WAIT,
   NFSMACTION_CONTINUE
} NextFSMAction;

// 保存模块信息和模块引用计数的结构
typedef struct NumberManagerMod
{
    DECLARE_VTBL(IModule)
    uint32   m_nRefs;
} NumberManagerMod;

//闹钟首选项的数据结构
typedef struct _NumberManagerCfg
{
	AECHAR telexchange_num[MAX_INPUT_NUM];        //总机号码
} NumberManagerCfg;

// NumberManager Applet对象结构体：
typedef struct _CNumberManager
{
    DECLARE_VTBL(INumberManager)
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
    NumberManagerState m_ePreState;
    // Applet 当前状态
    NumberManagerState m_eCurState;
    // Applet 返回状态
    NumberManagerState m_eMsgRetState;    
    // Applet是否处于挂起状态
    boolean m_bSuspending;
    // Applet是否准备好可以处理按键和命令事件。改变量主要用于快速按键引起的问题
    boolean m_bAppIsReady;

    uint16 m_currDlgId;       //保存当前所选菜单位置
    uint16 m_MsgId;  
    uint16 m_nFldInputID;
    AECHAR m_number[MAX_INPUT_NUM];
    NumberManagerCfg m_NumberManagerCfg;
    char m_URL[MAX_INPUT_URL];    
} CNumberManager;


/*==============================================================================
                                 函数声明
==============================================================================*/
static void NumberManager_APPIsReadyTimer(void *pme);

// 对话框 IDD_NUMBERMANAGER_DIALOG 事件处理函数
static boolean  HandleMainDialogEvent(CNumberManager *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

// 对话框 IDD_DISPLAY_NUMBER_DIALOG 事件处理函数
static boolean  HandleDisplayNumberDialogEvent(CNumberManager *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

// 对话框 IDD_TEXT_INPUT_DIALOG 事件处理函数
static boolean  HandleTextInputDialogEvent(CNumberManager *pMe,
   AEEEvent   eCode,
   uint16     wParam,
   uint32     dwParam
);

// 状态 NUMBERMANAGERST_NONE 处理函数
static NextFSMAction StateNoOperationHandler(CNumberManager *pMe);

// 状态 NUMBERMANAGERST_INIT 处理函数
static NextFSMAction StateInitHandler(CNumberManager *pMe);

// 状态 NUMBERMANAGERST_MAIN 处理函数
static NextFSMAction StateMainHandler(CNumberManager *pMe);

// 状态 NUMBERMANAGERST_DISPLAY_NUMBER 处理函数
static NextFSMAction StateDisplayNumberHandler(CNumberManager *pMe);

// 状态 NUMBERMANAGERST_TEXT_INPUT 处理函数
static NextFSMAction StateTextInputHandler(CNumberManager *pMe);

// 状态 NUMBERMANAGERST_EXIT 处理函数
static NextFSMAction StateExitHandler(CNumberManager *pMe);

//拨号处理函数
static boolean Numbermanager_SendCall(CNumberManager *pMe);

/*----------------------对话框相关函数声明---------------------*/
/*==============================================================================
函数:
       NumberManager_ShowDialog
说明:
       函数由状态处理函数用来创建相应对话框资源ID的对话框。

参数:
       pMe [In]: 指向NumberManager Applet对象结构的指针,该结构包含小程序的特定信息.
       dlgResId [in]：对话框资源ID，函数根据该ID创建对话框。

返回值:
       无

备注:


==============================================================================*/
void NumberManager_ShowDialog(CNumberManager *pMe,uint16  dlgResId);

/*==============================================================================
函数:
       NumberManager_RouteDialogEvent
说明:
       函数将BREW事件路由给当前活动对话框的事件处理函数。

参数:
       pMe [In]: 指向NumberManager Applet对象结构的指针,该结构包含小程序的特定信息.
       eCode [in]：事件代码。
       wParam [in]：与事件eCode关联的数据。
       dwParam [in]：与事件eCode关联的数据。

返回值:
       TRUE：传入事件被处理。
       FALSE：传入事件没被处理。

备注:

==============================================================================*/
boolean NumberManager_RouteDialogEvent(CNumberManager *pMe,
    AEEEvent    eCode,
    uint16  wParam,
    uint32 dwParam
);

/*----------------------状态机相关函数声明---------------------*/
/*==============================================================================
函数:
       NumberManager_ProcessState
说明:
       NumberManager Applet状态处理主函数。函数根据pMe->m_currState将调用路由至
       相应的处理函数。

参数:
       pMe [In]: 指向NumberManager Applet对象结构的指针,该结构包含小程序的特定信息.

返回值:
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注:

==============================================================================*/
NextFSMAction NumberManager_ProcessState(CNumberManager *pMe);

/*----------------------模块相关函数声明---------------------*/
#if defined(BREW_STATIC_APP)
PFNMODENTRY NumberManager_GetModInfo(IShell   *ps,
    AEECLSID  **ppClasses,
    AEEAppInfo  **pApps,
    uint16  *pnApps,
    uint16  *pwMinPriv
);
#endif

int  NumberManagerMod_Load(IShell   *pIShell,
    void     *ph,
    IModule **ppMod
);

int  NumberManagerMod_New(int16  nSize,
    IShell   *pIShell,
    void     *ph,
    IModule  **ppMod,
    PFNMODCREATEINST pfnMC,
    PFNFREEMODDATA pfnMF
);

static int  NumberManagerMod_CreateInstance(IModule *po,
    IShell *pIShell,
    AEECLSID ClsId,
    void **ppObj
);

static uint32 NumberManagerMod_AddRef(IModule *po);

static uint32 NumberManagerMod_Release(IModule *po);

static void NumberManagerMod_FreeResources(IModule  *po,
    IHeap  *ph,
    IFileMgr *pfm
);

/*----------------------Applet相关函数声明---------------------*/
static int NumberManager_New(IShell *ps,
    IModule *pIModule,
    INumberManager **ppObj
);

static uint32  NumberManager_AddRef(INumberManager *pi);

static uint32  NumberManager_Release (INumberManager *pi);

static boolean NumberManager_HandleEvent(INumberManager *pi,
    AEEEvent eCode,
    uint16  wParam,
    uint32 dwParam
);

static int NumberManager_InitAppData(CNumberManager *pMe);

static void NumberManager_FreeAppData(CNumberManager *pMe);

static void NumberManager_RunFSM(CNumberManager *pMe);

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
static NumberManagerMod gNumberManagerMod;

static const VTBL(IModule) gModFuncs =
{
    NumberManagerMod_AddRef,
    NumberManagerMod_Release,
    NumberManagerMod_CreateInstance,
    NumberManagerMod_FreeResources
};

// 只允许一个NumberManager实例。每次创建NumberManager Applet时，
// 返回同一结构指针给BREW层。
static CNumberManager gNumberManager;

static const VTBL(INumberManager) gNumberManagerMethods =
{
    NumberManager_AddRef,
    NumberManager_Release,
    NumberManager_HandleEvent
};

/*----------------------模块相关函数定义---------------------*/

/*==============================================================================
函数:
       NumberManagerMod_Load

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
int NumberManagerMod_Load(IShell   *pIShell,
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

    return NumberManagerMod_New(sizeof(NumberManagerMod),
                    pIShell,
                    ph,
                    ppMod,
                    NULL,
                    NULL);
}

/*==============================================================================
函数:
       NumberManagerMod_New

说明:
       用来初始化NumberManager模块虚拟表（也增加引用计数）的助手函数。

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
int  NumberManagerMod_New(int16  nSize,
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

    MEMSET(&gNumberManagerMod,0,sizeof(NumberManagerMod));

    INIT_VTBL(&gNumberManagerMod,IModule,gModFuncs);
    gNumberManagerMod.m_nRefs = 1;
    *ppMod = (IModule *)&gNumberManagerMod;
    return AEE_SUCCESS;
}

/*==============================================================================
函数:
       NumberManagerMod_AddRef

说明:
       NumberManager 模块引用计数加1，并返回当前的引用数。

参数:
       po [in]：IModule接口对象指针。

返回值:
       当前对NumberManager 模块的引用数

备注:

==============================================================================*/
static uint32 NumberManagerMod_AddRef(IModule *po)
{
    return(++((NumberManagerMod *)po)->m_nRefs);
}

/*==============================================================================
函数:
       NumberManagerMod_Release

说明:
       NumberManager 模块引用计数减1，并返回当前的引用数。

参数:
       po [in]：IModule接口对象指针。

返回值:
       当前对NumberManager 模块的引用数

备注:

==============================================================================*/
static uint32 NumberManagerMod_Release(IModule *po)
{
    if (((NumberManagerMod *) po)->m_nRefs == 0)
    {
        return 0;
    }

    return(--((NumberManagerMod *)po)->m_nRefs);
}

/*==============================================================================
函数:
       NumberManagerMod_CreateInstance

说明:
        通过创建(初始化)NumberManager对象来创建模块实例。

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
static int  NumberManagerMod_CreateInstance(IModule *po,
    IShell *pIShell,
    AEECLSID ClsId,
    void **ppObj
)
{
    *ppObj = NULL;

    if (ClsId != AEECLSID_NUMBERMANAGER)
    {
        return EFAILED;
    }

    if (NumberManager_New(pIShell, po, (INumberManager**)ppObj) != SUCCESS)
    {
        return EFAILED;
    }
    return SUCCESS;
}

/*==============================================================================
函数:
       NumberManagerMod_FreeResources

说明:
        释放NumberManager模块资源的函数。

参数:
       po ：IModule。
       ph ：IHeap接口指针。
       pfm :IFileMgr接口指针。

返回值:
       无

备注:
       不需用户去调用。

==============================================================================*/
static void NumberManagerMod_FreeResources(IModule  *po,
    IHeap  *ph,
    IFileMgr *pfm
)
{

}

/*----------------------Applet相关函数定义---------------------*/
/*==============================================================================
函数:
       NumberManager_New

说明:
        初始化NumberManager Applet虚拟表和内部数据变量，同时其引用计数加1。

参数:
       ps [in]：IShell接口对象指针。
       pIModule [in]：pIModule接口对象指针。
       ppObj [out]:：指向INumberManager对象指针的指针。

返回值:
       EFAILED：失败。
       SUCCESS：成功。

备注:
       不需用户去调用。
       关于Applet结构对象可使用动态分配内存的方式创建，本处采用的是使用静态
       变量的方式。建议对结构比较大的Applet使用静态变量的方式。对于动态分配
       内存的方式需要注意对所分配空间的释放。

==============================================================================*/
static int NumberManager_New(IShell *ps,
    IModule *pIModule,
    INumberManager **ppObj
)
{
    int retVal = SUCCESS;

    if (0 == gNumberManager.m_nRefs)
    {
        // Must initialize the object
        MEMSET(&gNumberManager,  0, sizeof(CNumberManager));
        INIT_VTBL(&gNumberManager, INumberManager, gNumberManagerMethods);

        gNumberManager.m_nRefs     = 0;
        gNumberManager.m_pShell    = ps;
        gNumberManager.m_pModule   = pIModule;

        (void) ISHELL_AddRef(ps);
        (void) IMODULE_AddRef(pIModule);

        retVal = NumberManager_InitAppData(&gNumberManager);
    }

    ++gNumberManager.m_nRefs;
    *ppObj = (INumberManager*)&gNumberManager;
    return retVal;
}

/*==============================================================================
函数:
       NumberManager_AddRef

说明:
       NumberManager Applet 引用计数加1，并返回当前的引用数。

参数:
       pi [in]：INumberManager接口对象指针。

返回值:
       当前对NumberManager Applet的引用数

备注:

==============================================================================*/
static uint32  NumberManager_AddRef(INumberManager *pi)
{
    register CNumberManager *pMe = (CNumberManager*)pi;

    //ASSERT(pMe != NULL);
    //ASSERT(pMe->m_nRefs > 0);

    return (++pMe->m_nRefs);
}

/*==============================================================================
函数:
       NumberManager_Release

说明:
       释放对NumberManager Applet的引用，引用计数减1，并返回当前的引用数。
        如果当前的引用数为零，还将释放Applet及相关资源。

参数:
       pi [in]：INumberManager接口对象指针。

返回值:
       当前对NumberManager Applet的引用数

备注:

==============================================================================*/
static uint32  NumberManager_Release (INumberManager *pi)
{
    register CNumberManager *pMe = (CNumberManager*)pi;

    //ASSERT(pMe != NULL);

    if (pMe->m_nRefs == 0)
    {
        return 0;
    }

    if (--pMe->m_nRefs)
    {
        return pMe->m_nRefs;
    }

    // 释放Applet相关资源
    NumberManager_FreeAppData(pMe);

    (void) ISHELL_Release(pMe->m_pShell);
    (void) IMODULE_Release(pMe->m_pModule);

    // 注意：pMe是静态分配空间，因此不需要释放。FREE()
    return 0;
}

/*==============================================================================
函数:
       NumberManager_InitAppData

说明:
       初始化NumberManager Applet结构数据缺省值。

参数:
       pMe [in]：指向NumberManager Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
       SUCCESS：初始化成功。
       EFAILED：初始化失败。

备注:

==============================================================================*/
static int NumberManager_InitAppData(CNumberManager *pMe)
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
    pMe->m_ePreState = NUMBERMANAGERST_NONE;
    pMe->m_eCurState = NUMBERMANAGERST_INIT;
    pMe->m_pActiveDlg = NULL;
    pMe->m_eDlgRet = DLGRET_CREATE;
    pMe->m_bNotOverwriteDlgRet = FALSE;

    pMe->m_currDlgId = 0;
    
    pMe->m_MsgId = 0;
    pMe->m_nFldInputID = 0;  

    return SUCCESS;
}

/*==============================================================================
函数:
       NumberManager_FreeAppData

说明:
       释放NumberManager Applet使用的相关资源。

参数:
       pMe [in]：指向NumberManager Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
       无。

备注:

==============================================================================*/
static void NumberManager_FreeAppData(CNumberManager *pMe)
{
    if (NULL == pMe)
    {
        return ;
    }

}

/*==============================================================================
函数:
       NumberManager_RunFSM

说明:
       NumberManager Applet有限状态机引擎。

参数:
       pMe [in]：指向NumberManager Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
       无。

备注:

==============================================================================*/
static void NumberManager_RunFSM(CNumberManager *pMe)
{
    NextFSMAction nextFSMAction = NFSMACTION_WAIT;

    for ( ; ; )
    {
        nextFSMAction = NumberManager_ProcessState(pMe);

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
       NumberManager_HandleEvent

说明:
       NumberManager  Applet事件处理主函数。所有属于本Applet的事件，
       都将经由该函数处理。

参数:
       pi [in]：指向INumberManager接口t对象的指针。
       eCode：BREW事件代码。
       wParam：与事件相关联的数据。
       dwParam [in]：与事件相关联的数据。

返回值:
       TRUE：传入事件被处理。
       FALSE：传入事件没被处理。

备注:

==============================================================================*/
static boolean NumberManager_HandleEvent(INumberManager *pi,
    AEEEvent eCode,
    uint16  wParam,
    uint32 dwParam
)
{
    CNumberManager *pMe = (CNumberManager*)pi;
    AEEAppStart *as;

    switch (eCode)
    {
        case EVT_APP_START:
            pMe->m_bAppIsReady = FALSE;
            //ASSERT(dwParam != 0);
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

            pMe->m_bSuspending = FALSE;

            // 开始NumberManager状态机
            NumberManager_RunFSM(pMe);
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
            //ASSERT(dwParam != 0);
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
            {
                AEEDeviceInfo di; 
                ISHELL_GetDeviceInfo(pMe->m_pShell,&di);
                pMe->m_rc.dy = di.cyScreen;
            }

            NumberManager_RunFSM(pMe);
            return TRUE;

        case EVT_DIALOG_INIT:
            pMe->m_bAppIsReady = FALSE;
            pMe->m_pActiveDlg = (IDialog*)dwParam;
            pMe->m_pActiveDlgID = wParam;

            return NumberManager_RouteDialogEvent(pMe,eCode,wParam,dwParam);

        case EVT_DIALOG_START:
            (void)ISHELL_SetTimer ( pMe->m_pShell,
                            APPISREADY_TIMER,
                            NumberManager_APPIsReadyTimer,
                            pMe);
            
            return NumberManager_RouteDialogEvent(pMe,eCode,wParam,dwParam);

        case EVT_USER_REDRAW:
            (void) NumberManager_RouteDialogEvent(pMe,eCode,wParam,dwParam);
            
            /*
            (void)ISHELL_SetTimer ( pMe->m_pShell,
                            APPISREADY_TIMER,
                            NumberManager_APPIsReadyTimer,
                            pMe);
            */
            return TRUE;

        case EVT_APPISREADY:
            (void) NumberManager_RouteDialogEvent(pMe,eCode,wParam,dwParam);
            pMe->m_bAppIsReady = TRUE;
            return TRUE;

        case EVT_KEY_PRESS:
			#if !defined(FEATURE_VERSION_K212) && !defined(FEATURE_VERSION_EC99)&&!defined(FEATURE_QVGA_INHERIT_K212)
            if (!pMe->m_bAppIsReady)
            {
                return TRUE;
            }
			#endif
            return NumberManager_RouteDialogEvent(pMe,eCode,wParam,dwParam);

        case EVT_KEY_RELEASE:
			#if !defined(FEATURE_VERSION_K212) && !defined(FEATURE_VERSION_EC99)&&!defined(FEATURE_QVGA_INHERIT_K212)
            if (!pMe->m_bAppIsReady)
            {
                return TRUE;
            }
			#endif
            return NumberManager_RouteDialogEvent(pMe,eCode,wParam,dwParam);

        case EVT_KEY_HELD:
			#if !defined(FEATURE_VERSION_K212) && !defined(FEATURE_VERSION_EC99)&&!defined(FEATURE_QVGA_INHERIT_K212)
            if (!pMe->m_bAppIsReady)
            {
                return TRUE;
            }
			#endif
            return NumberManager_RouteDialogEvent(pMe,eCode,wParam,dwParam);

        case EVT_KEY:
			#if !defined(FEATURE_VERSION_K212) && !defined(FEATURE_VERSION_EC99)&&!defined(FEATURE_QVGA_INHERIT_K212)
            if (!pMe->m_bAppIsReady)
            {
                return TRUE;
            }
			#endif
            return NumberManager_RouteDialogEvent(pMe,eCode,wParam,dwParam);

        case EVT_COMMAND:
			#if !defined(FEATURE_VERSION_K212) && !defined(FEATURE_VERSION_EC99)&&!defined(FEATURE_QVGA_INHERIT_K212)
            if (!pMe->m_bAppIsReady)
            {
                return TRUE;
            }
			#endif
            return NumberManager_RouteDialogEvent(pMe,eCode,wParam,dwParam);

        case EVT_DIALOG_END:
            if (wParam == 0)
            {
                return TRUE;
            }
            pMe->m_bAppIsReady = FALSE;
            (void) NumberManager_RouteDialogEvent(pMe,eCode,wParam,dwParam);
            pMe->m_pActiveDlg = NULL;

            // Applet被挂起，不跑状态机，等待Applet返回。（注意：EVT_APP_SUSPEND
            // 事件在EVT_DIALOG_END事件前发出。
            if (pMe->m_bSuspending == FALSE)
            {
                // 开始NumberManager状态机
                NumberManager_RunFSM(pMe);
            }
            return TRUE;
#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
		case EVT_PEN_UP:
		//case EVT_PEN_DOWN:
			{
				return NumberManager_RouteDialogEvent(pMe,eCode,wParam,dwParam);
			}

		case EVT_USER:
			{
				if((wParam == AVK_SELECT))
				{
					eCode = EVT_KEY;
					//wParam = dwParam;
					//dwParam = 0;
				}
				else if((wParam == AVK_CLR)  || (wParam == AVK_INFO))
				{
					eCode = EVT_KEY;
				}
				return NumberManager_RouteDialogEvent(pMe,eCode,wParam,dwParam);
			}
#endif 
        default:
            // 将接收到的事件路由至当前活动的对话框事件处理函数。
            return NumberManager_RouteDialogEvent(pMe,eCode,wParam,dwParam);
    }
}

/*==============================================================================
函数:
       NumberManager_APPIsReadyTimer

说明:
       定时器回调函数。主要用于控制快速按键问题的处理。

参数:
       pme [in]：void *类型指针，暗指CNumberManager结构指针。

返回值:
       无。

备注:

==============================================================================*/
static void NumberManager_APPIsReadyTimer(void *pme)
{
    CNumberManager *pMe = (CNumberManager *)pme;

    if (NULL == pMe)
    {
        return;
    }

    (void) ISHELL_PostEvent( pMe->m_pShell,
                            AEECLSID_NUMBERMANAGER,
                            EVT_APPISREADY,
                            0,
                            0);
}


/*==============================================================================
函数:
       NumberManager_ShowDialog
说明:
       函数由状态处理函数用来创建相应对话框资源ID的对话框。

参数:
       pMe [In]: 指向NumberManager Applet对象结构的指针,该结构包含小程序的特定信息.
       dlgResId [in]：对话框资源ID，函数根据该ID创建对话框。

返回值:
       无

备注:


==============================================================================*/
void NumberManager_ShowDialog(CNumberManager *pMe,uint16  dlgResId)
{
    int nRet;

    // At most one dialog open at once
    if (ISHELL_GetActiveDialog(pMe->m_pShell) != NULL)
    {
        // Looks like there is one dialog already opened.
        // Flag an error an return without doing anything.
        return;
    }

    nRet = ISHELL_CreateDialog(pMe->m_pShell,AEE_NUMBERMANAGER_RES_FILE,dlgResId,NULL);
}

/*==============================================================================
函数:
       NumberManager_RouteDialogEvent
说明:
       函数将BREW事件路由给当前活动对话框的事件处理函数。

参数:
       pMe [In]: 指向NumberManager Applet对象结构的指针,该结构包含小程序的特定信息.
       eCode [in]：事件代码。
       wParam [in]：与事件eCode关联的数据。
       dwParam [in]：与事件eCode关联的数据。

返回值:
       TRUE：传入事件被处理。
       FALSE：传入事件没被处理。

备注:

==============================================================================*/
boolean NumberManager_RouteDialogEvent(CNumberManager *pMe,
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
        case IDD_NUMBERMANAGER_DIALOG:
            return HandleMainDialogEvent(pMe,eCode,wParam,dwParam);

        case IDD_DISPLAY_NUMBER_DIALOG:
            return HandleDisplayNumberDialogEvent(pMe,eCode,wParam,dwParam);
            
        case IDD_TEXT_INPUT_DIALOG:
            return HandleTextInputDialogEvent(pMe,eCode,wParam,dwParam);
                        
        default:
            return FALSE;
    }
}




/*==============================================================================
函数:
       NumberManager_ProcessState
说明:
       NumberManager Applet状态处理主函数。函数根据pMe->m_currState将调用路由至
       相应的处理函数。

参数:
       pMe [In]: 指向NumberManager Applet对象结构的指针,该结构包含小程序的特定信息.

返回值:
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注:

==============================================================================*/
NextFSMAction NumberManager_ProcessState(CNumberManager *pMe)
{
    NextFSMAction retVal = NFSMACTION_WAIT;

    if (NULL == pMe)
    {
        return retVal;
    }

    // 根据当前状态调用相应的状态处理函数
    switch (pMe->m_eCurState)
    {
        case NUMBERMANAGERST_NONE:
            retVal = StateNoOperationHandler(pMe);
            break;

        case NUMBERMANAGERST_INIT:
            retVal = StateInitHandler(pMe);
            break;

        case NUMBERMANAGERST_MAIN:
            retVal = StateMainHandler(pMe);
            break;
        
        case NUMBERMANAGERST_DISPLAY_NUMBER:
            retVal = StateDisplayNumberHandler(pMe);
            break;

        case NUMBERMANAGERST_TEXT_INPUT:
            retVal = StateTextInputHandler(pMe);
            break;
                                
        case NUMBERMANAGERST_EXIT:
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
       NUMBERMANAGERST_NONE 状态处理函数

参数：
       pMe [in]：指向NumberManager Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/
static NextFSMAction StateNoOperationHandler(CNumberManager *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    MOVE_TO_STATE(NUMBERMANAGERST_EXIT)
    return NFSMACTION_CONTINUE;
} // StateNoOperationHandler


/*==============================================================================
函数：
       StateInitHandler
说明：
       NUMBERMANAGERST_INIT 状态处理函数

参数：
       pMe [in]：指向NumberManager Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/
static NextFSMAction StateInitHandler(CNumberManager *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    MOVE_TO_STATE(NUMBERMANAGERST_MAIN)
    return NFSMACTION_CONTINUE;
} // StateInitHandler


/*==============================================================================
函数：
       StateMainHandler
说明：
       NUMBERMANAGERST_MAIN 状态处理函数

参数：
       pMe [in]：指向NumberManager Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/
static NextFSMAction StateMainHandler(CNumberManager *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            NumberManager_ShowDialog(pMe, IDD_NUMBERMANAGER_DIALOG);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
            MOVE_TO_STATE(NUMBERMANAGERST_EXIT)
            return NFSMACTION_CONTINUE;
            
        case DLGRET_OK:
            MOVE_TO_STATE(NUMBERMANAGERST_DISPLAY_NUMBER)
            return NFSMACTION_CONTINUE;    
            
        case DLGRET_TEXT_INPUT:
            pMe->m_eMsgRetState = NUMBERMANAGERST_MAIN;
            MOVE_TO_STATE(NUMBERMANAGERST_TEXT_INPUT)
            return NFSMACTION_CONTINUE;                        

        default:
            //ASSERT_NOT_REACHABLE;
            break;
    }

    return NFSMACTION_WAIT;
} // StateMainHandler

/*==============================================================================
函数：
       StateDisplayNumberHandler
说明：
       NUMBERMANAGERST_DISPLAY_NUMBER 状态处理函数

参数：
       pMe [in]：指向NumberManager Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/
static NextFSMAction StateDisplayNumberHandler(CNumberManager *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            NumberManager_ShowDialog(pMe, IDD_DISPLAY_NUMBER_DIALOG);
            return NFSMACTION_WAIT;
            
        case DLGRET_CANCELED:
            MOVE_TO_STATE(NUMBERMANAGERST_MAIN)
            return NFSMACTION_CONTINUE;
            
        case DLGRET_TEXT_INPUT:
            pMe->m_eMsgRetState = NUMBERMANAGERST_DISPLAY_NUMBER;
            MOVE_TO_STATE(NUMBERMANAGERST_TEXT_INPUT)
            return NFSMACTION_CONTINUE;   
                      
        default:
            //ASSERT_NOT_REACHABLE;
            break;
    }

    return NFSMACTION_WAIT;
} // StateMainHandler

/*==============================================================================
函数：
       StateTextInputHandler
说明：
       NUMBERMANAGERST_TEXT_INPUT 状态处理函数

参数：
       pMe [in]：指向NumberManager Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/
static NextFSMAction StateTextInputHandler(CNumberManager *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            NumberManager_ShowDialog(pMe, IDD_TEXT_INPUT_DIALOG);
            return NFSMACTION_WAIT;

        case DLGRET_OK:
            if(0 != WSTRLEN(pMe->m_NumberManagerCfg.telexchange_num)) 
            {  
                MOVE_TO_STATE(NUMBERMANAGERST_DISPLAY_NUMBER)
            }
            else
            {
                MOVE_TO_STATE(NUMBERMANAGERST_MAIN)
            }
            return NFSMACTION_CONTINUE;

        case DLGRET_CANCELED:        	
            MOVE_TO_STATE(pMe->m_eMsgRetState)
            return NFSMACTION_CONTINUE;
            
        default:
            //ASSERT_NOT_REACHABLE;
            break;
    }

    return NFSMACTION_WAIT;
} // StateTextInputHandler

/*==============================================================================
函数：
       StateExitHandler
说明：
       NUMBERMANAGERST_EXIT 状态处理函数

参数：
       pMe [in]：指向NumberManager Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/
static NextFSMAction StateExitHandler(CNumberManager *pMe)
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
       IDD_NUMBERMANAGER_DIALOG对话框事件处理函数

参数：
       pMe [in]：指向NumberManager Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/
static boolean  HandleMainDialogEvent(CNumberManager *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    PARAM_NOT_REF(dwParam)

    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,
                                                    IDC_NUMBERMANAGER_DIALOG);
    if (pMenu == NULL)
    {
        return FALSE;
    }

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
#ifdef FEATURE_SUPPORT_WAP_APP            
            IMENUCTL_AddItem(pMenu, AEE_NUMBERMANAGER_RES_FILE, IDS_LIFE_SEARCHING_TITLE,  IDS_LIFE_SEARCHING_TITLE, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_NUMBERMANAGER_RES_FILE, IDS_MAP_SERVICE_TITLE,  IDS_MAP_SERVICE_TITLE, NULL, 0);            
#endif //FEATURE_SUPPORT_WAP_APP
            IMENUCTL_AddItem(pMenu, AEE_NUMBERMANAGER_RES_FILE, IDS_BOOK_TICKET_TITLE,  IDS_BOOK_TICKET_TITLE, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_NUMBERMANAGER_RES_FILE, IDS_BOOK_HOTEL_TITLE, IDS_BOOK_HOTEL_TITLE, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_NUMBERMANAGER_RES_FILE, IDS_BOOK_RESTAURANT_TITLE, IDS_BOOK_RESTAURANT_TITLE, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_NUMBERMANAGER_RES_FILE, IDS_COMMUNICATE_ASSISTANT_TITLE,  IDS_COMMUNICATE_ASSISTANT_TITLE, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_NUMBERMANAGER_RES_FILE, IDS_TELEPHONE_EXCHANGE_TITLE, IDS_TELEPHONE_EXCHANGE_TITLE, NULL, 0);         
#ifdef FEATURE_SUPPORT_WAP_APP
            IMENUCTL_AddItem(pMenu, AEE_NUMBERMANAGER_RES_FILE, IDS_MEMBER_SPACE_TITLE,  IDS_MEMBER_SPACE_TITLE, NULL, 0);
#endif //FEATURE_SUPPORT_WAP_APP
            IMENUCTL_AddItem(pMenu, AEE_NUMBERMANAGER_RES_FILE, IDS_CALL_NUMBER_MANAGER_TITLE, IDS_CALL_NUMBER_MANAGER_TITLE, NULL, 0);
            return TRUE;

        case EVT_DIALOG_START:
            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);
            IMENUCTL_SetOemProperties( pMenu , OEMMP_USE_MENU_STYLE);
            IMENUCTL_SetBottomBarType( pMenu, BTBAR_SELECT_BACK);
            (void) ISHELL_PostEvent( pMe->m_pShell,
                                     AEECLSID_NUMBERMANAGER,
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
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED);
                    return TRUE;                                                                           
                                        
                default:
                    break;
            }    
            return TRUE;

        case EVT_COMMAND:
            //pMe->m_currDlgId = IMENUCTL_GetSel(pMenu);
            pMe->m_currDlgId = wParam;
            switch (wParam)
            {
                case IDS_LIFE_SEARCHING_TITLE:                    
                    STRCPY( pMe->m_URL, "http://wap.118114.cn/life");
                    pMe->m_MsgId = IDS_LIFE_SEARCHING_TITLE;
                    CLOSE_DIALOG(DLGRET_OK);
                    return TRUE;  

                case IDS_MAP_SERVICE_TITLE:
                    STRCPY( pMe->m_URL, "http://wap.118114.cn/around");                   
                    pMe->m_MsgId = IDS_MAP_SERVICE_TITLE;
                    CLOSE_DIALOG(DLGRET_OK);
                    return TRUE;  
                    
                case IDS_BOOK_TICKET_TITLE:
                    (void) STRTOWSTR("11843490001",pMe->m_number,sizeof(pMe->m_number));
                    pMe->m_MsgId = IDS_BOOK_TICKET_TITLE;
                    CLOSE_DIALOG(DLGRET_OK);
                    return TRUE;  

                case IDS_BOOK_HOTEL_TITLE:
                    (void) STRTOWSTR("11843490002",pMe->m_number,sizeof(pMe->m_number));
                    pMe->m_MsgId = IDS_BOOK_HOTEL_TITLE;                
                    CLOSE_DIALOG(DLGRET_OK);
                    return TRUE;  

                case IDS_BOOK_RESTAURANT_TITLE:
                    (void) STRTOWSTR("11843490003",pMe->m_number,sizeof(pMe->m_number));
                    pMe->m_MsgId = IDS_BOOK_RESTAURANT_TITLE;                
                    CLOSE_DIALOG(DLGRET_OK);
                    return TRUE;  

                case IDS_COMMUNICATE_ASSISTANT_TITLE:
                    (void) STRTOWSTR("11843490004",pMe->m_number,sizeof(pMe->m_number));
                    pMe->m_MsgId = IDS_COMMUNICATE_ASSISTANT_TITLE;                
                    CLOSE_DIALOG(DLGRET_OK);
                    return TRUE;  

                case IDS_TELEPHONE_EXCHANGE_TITLE:
                    pMe->m_MsgId = IDS_TELEPHONE_EXCHANGE_TITLE;                  
                    (void)ISHELL_GetPrefs(pMe->m_pShell, 
                                AEECLSID_NUMBERMANAGER, 
                                NUMBERMANAGER_CFG_VERSION,
                                &pMe->m_NumberManagerCfg, 
                                sizeof(NumberManagerCfg)); 
                    if(0 == WSTRLEN(pMe->m_NumberManagerCfg.telexchange_num))
                    {        
                        pMe->m_nFldInputID = IDS_TELEPHONE_EXCHANGE_INPUT;
                        CLOSE_DIALOG(DLGRET_TEXT_INPUT);
                    } 
                    else
                    {   
                        WSTRCPY(pMe->m_number, pMe->m_NumberManagerCfg.telexchange_num);  
                        CLOSE_DIALOG(DLGRET_OK);
                    }
                    return TRUE;  

                 case IDS_MEMBER_SPACE_TITLE:
                    STRCPY( pMe->m_URL, "http://wap.118114.cn/member");    
                    pMe->m_MsgId = IDS_MEMBER_SPACE_TITLE;
                    CLOSE_DIALOG(DLGRET_OK);
                    return TRUE;  
                    
                case IDS_CALL_NUMBER_MANAGER_TITLE:
                    (void) STRTOWSTR("118114",pMe->m_number,sizeof(pMe->m_number));
                    pMe->m_MsgId = IDS_CALL_NUMBER_MANAGER_TITLE;                
                    CLOSE_DIALOG(DLGRET_OK);
                    return TRUE;                     

                default:
                    //ASSERT_NOT_REACHABLE;
                    break;
            }
            return TRUE;

        default:
            break;
    }
    return FALSE;
} // HandleMainDialogEvent

/*==============================================================================
函数：
       HandleDisplayNumberDialogEvent
说明：
       IDD_DISPLAY_NUMBER_DIALOG对话框事件处理函数

参数：
       pMe [in]：指向NumberManager Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/
static boolean  HandleDisplayNumberDialogEvent(CNumberManager *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    PARAM_NOT_REF(dwParam)

    IStatic * pStatic = (IStatic*)IDIALOG_GetControl(pMe->m_pActiveDlg,
                                                     IDC_DISPLAY_NUMBER_DIALOG);

    if (NULL == pStatic)
    {
        return FALSE;
    }

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            return TRUE;

        case EVT_DIALOG_START:
           {
                AEERect   rc; 
                AECHAR    szBuf[256];  //静态文本显示的内容
                int n = 0;           

                //Draw static                           
                rc = pMe->m_rc;
                rc.y = GetTitleBarHeight(pMe->m_pDisplay);
                rc.dy = rc.dy - GetBottomBarHeight(pMe->m_pDisplay)-GetTitleBarHeight(pMe->m_pDisplay);
                ISTATIC_SetRect(pStatic, &rc);                        
                // 设置静态文本控件属性
                ISTATIC_SetProperties(pStatic, ST_CENTERTEXT|ST_MIDDLETEXT|ST_GRAPHIC_BG);
                ISTATIC_SetBackGround(pStatic, AEE_APPSCOMMONRES_IMAGESFILE, IDI_MENU_BACKGROUND); //added by chengxiao 2009.04.08

                //Draw text                              
                (void)MEMSET( szBuf,(AECHAR) 0, sizeof(szBuf));  

                switch (pMe->m_MsgId)
                {
                    case IDS_TELEPHONE_EXCHANGE_TITLE:
                        (void) ISHELL_LoadResString(pMe->m_pShell,
                                     AEE_NUMBERMANAGER_RES_FILE,
                                     IDS_TELEPHONE_EXCHANGE_NUMBER,
                                     szBuf,
                                     sizeof(szBuf)); 
                        (void)ISHELL_GetPrefs(pMe->m_pShell, 
                                    AEECLSID_NUMBERMANAGER, 
                                    NUMBERMANAGER_CFG_VERSION,
                                    &pMe->m_NumberManagerCfg, 
                                    sizeof(NumberManagerCfg)); 
                        
                        //Draw text                             
                        n = WSTRLEN(szBuf);
                        szBuf[n++] = (AECHAR) ':';             
                        if(0 != WSTRLEN(pMe->m_NumberManagerCfg.telexchange_num))
                        {
                            WSTRCPY(pMe->m_number, pMe->m_NumberManagerCfg.telexchange_num); 
                            WSTRCPY(szBuf + n, pMe->m_number);          
                        } 
                        break;

                    case IDS_LIFE_SEARCHING_TITLE:
                    case IDS_MAP_SERVICE_TITLE:
                    case IDS_MEMBER_SPACE_TITLE:
                        (void) ISHELL_LoadResString(pMe->m_pShell,
                                     AEE_NUMBERMANAGER_RES_FILE,
                                     IDS_CONNECT_TO,
                                     szBuf,
                                     sizeof(szBuf));                         
                        //Draw text                             
                        n = WSTRLEN(szBuf);
                        szBuf[n++] = (AECHAR) ' ';                          
                        if(0 != STRLEN(pMe->m_URL))
                        {
                            STRTOWSTR(pMe->m_URL, szBuf + n, sizeof(szBuf));      
                        }                         
                        break;

                    default:
                        WSTRCPY(szBuf, pMe->m_number);                         
                        break;                    
                }
               
                // 设置静态文本控件文本
                (void)ISTATIC_SetText(pStatic, 
                            NULL, 
                            szBuf, 
                            AEE_FONT_NORMAL, 
                            AEE_FONT_NORMAL);          
          
                (void) ISHELL_PostEvent( pMe->m_pShell,
                                         AEECLSID_NUMBERMANAGER,
                                         EVT_USER_REDRAW,
                                         0,
                                         0);
           }
            return TRUE;

        case EVT_USER_REDRAW:
            {
                TitleBar_Param_type  titleParam; 
                AEERect   titlerect; 
                AECHAR    titleBuf[128];  

                //Draw title           
                (void) ISHELL_LoadResString(pMe->m_pShell,
                                AEE_NUMBERMANAGER_RES_FILE,
                                pMe->m_MsgId,
                                titleBuf,
                                sizeof(titleBuf));                                                    
                SETAEERECT(&titlerect, 0, 0, pMe->m_rc.dx, GetTitleBarHeight(pMe->m_pDisplay));
                MEMSET(&titleParam,0,sizeof(TitleBar_Param_type));
                titleParam.prc = &titlerect;
                titleParam.pwszTitle = titleBuf;
                titleParam.dwAlignFlags = IDF_TEXT_TRANSPARENT | IDF_ALIGN_CENTER;        
                DrawTitleBar(pMe->m_pDisplay, &titleParam);
                
                // Draw prompt bar here                
                switch (pMe->m_MsgId)
                {
                    case IDS_TELEPHONE_EXCHANGE_TITLE:
                        NUMBERMANAGER_DRAW_BOTTOMBAR(BTBAR_CALL_MODIFY_BACK);
                        break;

                    case IDS_LIFE_SEARCHING_TITLE:
                    case IDS_MAP_SERVICE_TITLE:
                    case IDS_MEMBER_SPACE_TITLE:
                        NUMBERMANAGER_DRAW_BOTTOMBAR(BTBAR_OK_BACK);                 
                        break;

                    default:
                        NUMBERMANAGER_DRAW_BOTTOMBAR(BTBAR_CALL_BACK);                                   
                        break;                    
                }

                IDISPLAY_Update(pMe->m_pDisplay);   
            } 
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED);
                    return TRUE;  
                                    
                case AVK_SELECT:
                    switch (pMe->m_MsgId)
                    {                       
                        case IDS_LIFE_SEARCHING_TITLE:
                        case IDS_MAP_SERVICE_TITLE:
                        case IDS_MEMBER_SPACE_TITLE:
                            ISHELL_PostURL(pMe->m_pShell,pMe->m_URL);                 
                            return TRUE; 

                        default:
                            return Numbermanager_SendCall(pMe);         
                    }
                    return TRUE; 
                                                                                            
                case AVK_INFO:
                    if(IDS_TELEPHONE_EXCHANGE_TITLE == pMe->m_MsgId)
                    {
                        pMe->m_nFldInputID = IDS_TELEPHONE_EXCHANGE_MODIFY;
                        CLOSE_DIALOG(DLGRET_TEXT_INPUT);
                        return TRUE; 
                    }
                    break;  
                                                         
                default:
                    break;
            }    
            return TRUE;

        case EVT_COMMAND:
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

				if(NUMBERMANAGER_PT_IN_RECT(wXPos,wYPos,rc))
				{
					if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
					{
						boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_NUMBERMANAGER,EVT_USER,AVK_SELECT,0);
						return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)   && wXPos < rc.x + (rc.dx/3)*2 )//左
					{
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_NUMBERMANAGER,EVT_USER,AVK_INFO,0);
						 return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
					{						
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_NUMBERMANAGER,EVT_USER,AVK_CLR,0);
						 return rt;
					}
				}

			}
			break;
#endif
        default:
            break;
    }
    return FALSE;
} // HandleDisplayNumberDialogEvent

/*==============================================================================
函数：
       IDD_TEXT_INPUT_Handler
说明：
       IDD_TEXT_INPUT_DIALOG对话框事件处理函数
       
参数：
       pMe [in]：指向MainMenu Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean  HandleTextInputDialogEvent(CNumberManager *pMe,
                                   AEEEvent   eCode,
                                   uint16     wParam,
                                   uint32     dwParam)
{
    PARAM_NOT_REF(dwParam)	
    ITextCtl *pTextCtl = (ITextCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,
                                                      IDC_TEXT_INPUT_DIALOG);
    if (NULL == pTextCtl)
    {
        return FALSE;
    }
    
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            return TRUE;
            
        case EVT_DIALOG_START: 
           {     
            AEERect rc;
            
            rc = pMe->m_rc;
            rc.dy -= GetBottomBarHeight(pMe->m_pDisplay);
            ITEXTCTL_SetRect(pTextCtl, &rc);            
            ITEXTCTL_SetProperties( pTextCtl,TP_FRAME | TP_FIXSETRECT |TP_EDITNUMBER_PTSTRING|TP_FOCUS_NOSEL|TP_GRAPHIC_BG);
            (void)ITEXTCTL_SetInputMode(pTextCtl,AEE_TM_NUMBERS);
            (void)ITEXTCTL_SetTitle( pTextCtl,
                                    AEE_NUMBERMANAGER_RES_FILE,
                                    pMe->m_nFldInputID,
                                     NULL);
            ITEXTCTL_SetMaxSize( pTextCtl,MAX_INPUT_NUM);
            
            (void)ISHELL_GetPrefs(pMe->m_pShell, 
                        AEECLSID_NUMBERMANAGER, 
                        NUMBERMANAGER_CFG_VERSION,
                        &pMe->m_NumberManagerCfg, 
                        sizeof(NumberManagerCfg));                                      
            if(0 != WSTRLEN(pMe->m_NumberManagerCfg.telexchange_num))
            {
                (void)ITEXTCTL_SetText(pTextCtl, pMe->m_NumberManagerCfg.telexchange_num, -1);
            }

            // For redraw the dialog
            (void)ISHELL_PostEvent( pMe->m_pShell,
                                    AEECLSID_NUMBERMANAGER,
                                    EVT_USER_REDRAW,
                                    0,
                                    0);
           }            
            return TRUE;
            
        case EVT_KEY_RELEASE:
        case EVT_USER_REDRAW:
            {
                AECHAR *pwstrText = ITEXTCTL_GetTextPtr(pTextCtl);
                int nLen=0;
                
                if (NULL != pwstrText)
                {
                    nLen = WSTRLEN(pwstrText);
                }
                
                if (nLen>0)
                {
                    // Option     Delete
                    NUMBERMANAGER_DRAW_BOTTOMBAR(BTBAR_OK_DELETE)
                }
                else
                {
                    // Option     Back
                    NUMBERMANAGER_DRAW_BOTTOMBAR(BTBAR_OK_BACK)
                }
            }        
            IDISPLAY_Update(pMe->m_pDisplay);             
            return TRUE;
                        
        case EVT_DIALOG_END:
            return TRUE;
            
        case EVT_DISPLAYDIALOGTIMEOUT:
            return TRUE;
            
        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED);
                    return TRUE;  
                    
                case AVK_SELECT:   
                    if(IDS_TELEPHONE_EXCHANGE_TITLE == pMe->m_MsgId)
                    {                        
                        (void) ITEXTCTL_GetText(pTextCtl,
                                        pMe->m_NumberManagerCfg.telexchange_num,
                                        MAX_INPUT_NUM);
                        (void)ISHELL_SetPrefs(pMe->m_pShell, 
                                    AEECLSID_NUMBERMANAGER, 
                                    NUMBERMANAGER_CFG_VERSION,
                                    &pMe->m_NumberManagerCfg, 
                                    sizeof(NumberManagerCfg));                                      
                         CLOSE_DIALOG(DLGRET_OK)                                  
                    }                                                                        
                    return TRUE;  
                                                            
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

				if(NUMBERMANAGER_PT_IN_RECT(wXPos,wYPos,rc))
				{
					if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
					{
						boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_NUMBERMANAGER,EVT_USER,AVK_SELECT,0);
						return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)   && wXPos < rc.x + (rc.dx/3)*2 )//左
					{
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_NUMBERMANAGER,EVT_USER,AVK_INFO,0);
						 return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
					{						
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_NUMBERMANAGER,EVT_USER,AVK_CLR,0);
						 return rt;
					}
				}

			}
			break;
#endif        
        default:
            break;
    }
    
    return FALSE;
} // HandleTextInputDialogEvent


/*==================================================================================
FUNCTION Numbermanager_SendCall
Description 发起一个呼叫
PARAMETERS:
          *pMe: CNumberManager object pointer
RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:
===================================================================================*/
static boolean Numbermanager_SendCall(CNumberManager *pMe)
{
    PARAM_NOT_REF(pMe)
    boolean  bRet = FALSE;        
    ICallApp *pCallApp = NULL;
    
    if(SUCCESS == ISHELL_CreateInstance(pMe->m_pShell, 
                                          AEECLSID_DIALER,
                                           (void**)&pCallApp))
    { 
        // 改用带参数启动 CallApp Applet
        if (SUCCESS == ICallApp_CallNumber(pCallApp,pMe->m_number))
        {
            bRet = TRUE;         
        }
    }
    if(pCallApp)
    {
        (void)ICallApp_Release(pCallApp);
        pCallApp = NULL;
    }
    return bRet;        
} //Numbermanager_SendCall
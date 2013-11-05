/*==============================================================================
// 文件：
//        IQuickTest.c
//        版权所有(c) 2007 Anylook
//        
// 文件说明：
//        
// 作者：Gemsea
// 创建日期：2007-11-13
// 当前版本：Draft
==============================================================================*/

/*==============================================================================
修改历史记录：
时       间      修 改 人     修改内容、位置及原因
-----------      ----------     -----------------------------------------------
07-11-13                初始版本
==============================================================================*/


/*==============================================================================
                                 本文件包含的外部文件
==============================================================================*/
#include "iquicktest_priv.h"

/*==============================================================================
                                 宏定义和常数
==============================================================================*/

/*==============================================================================
                                 类型定义
==============================================================================*/

/*==============================================================================
                                 函数声明
==============================================================================*/

static void QuickTest_APPIsReadyTimer(void *pme);

/*----------------------模块相关函数声明---------------------*/
#if defined(BREW_STATIC_APP)
                                    
int QuickTest_Load(IShell   *pIShell, 
                             void     *ph, 
                             IModule **ppMod);

static int QuickTest_CreateInstance( AEECLSID ClsId,
                                     IShell  *pIShell,
                                     IModule *po,
                                     void   **ppObj);
#endif

/*----------------------Applet相关函数声明---------------------*/
static int     CQuickTest_New(IShell       *ps,
                              IModule      *pIModule,
                              IQuickTest **ppObj);
static uint32  CQuickTest_AddRef(IQuickTest *pi);
static uint32  CQuickTest_Release(IQuickTest *pi);
static boolean CQuickTest_HandleEvent(IQuickTest *pi,
                                      AEEEvent    eCode,
                                      uint16      wParam,
                                      uint32      dwParam);
static int     CQuickTest_InitData(CQuickTest *pMe);
static void    CQuickTest_FreeAppData(CQuickTest *pMe);
static void    CQuickTest_RunFSM(CQuickTest *pMe);
/*==============================================================================
                                 全局数据
==============================================================================*/

/*==============================================================================
                                 本地（静态）数据
==============================================================================*/

// 只允许一个CQuickTest实例。每次创建CQuickTest Applet时，
// 返回同一结构指针给BREW层。
//static CQuickTest gQuickTest;

static const VTBL(IQuickTest) gIQuickTestMethods =
{
   CQuickTest_AddRef,
   CQuickTest_Release,
   CQuickTest_HandleEvent
};

/*----------------------模块相关函数定义---------------------*/

#if defined(BREW_STATIC_APP)

/*==============================================================================
函数: 
       QuickTest_Load
       
说明: 
       模块装载函数。
       
参数: 
       pIShell [in]：IShell接口指针。
       ph ：没有使用
       ppMod [out]：
       
返回值:
       int
       
备注:
       
===========================================================================*/
int QuickTest_Load(IShell   *pIShell,
                             void     *ph,
                             IModule **ppMod)
{
   return AEEStaticMod_New(sizeof(AEEMod),
                           pIShell,
                           ph,
                           ppMod,
                           (PFNMODCREATEINST)QuickTest_CreateInstance,
                           (PFNFREEMODDATA)NULL);
}
#endif
/*==============================================================================
函数: 
       QuickTest_CreateInstance
       
说明: 
        通过创建(初始化)QuickTest对象来创建模块实例。
       
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
#if defined(AEE_STATIC)
static int QuickTest_CreateInstance( AEECLSID ClsId,
                                     IShell  *pIShell,
                                     IModule *po,
                                     void   **ppObj)
#else
int AEEClsCreateInstance( AEECLSID   ClsId,
                          IShell    *pIShell,
                          IModule   *po,
                          void     **ppObj)
#endif
{
   *ppObj = NULL;

   if (ClsId != AEECLSID_QUICKTEST) {
      return EFAILED;
   }
   if (CQuickTest_New(pIShell, po, (IQuickTest**)ppObj) != SUCCESS) {
      return EFAILED;
   }
   return SUCCESS;
}
/*----------------------Applet相关函数定义---------------------*/
/*==============================================================================
函数: 
       CQuickTest_New
       
说明: 
        CQuickTest Applet虚拟表和内部数据变量，同时其引用计数加1。
       
参数: 
       ps [in]：IShell接口对象指针。
       pIModule [in]：pIModule接口对象指针。
       ppObj [out]:：指向IQuickTest对象指针的指针。
       
返回值:
       EFAILED：失败。
       SUCCESS：成功。
       
备注:
       不需用户去调用。
       关于Applet结构对象可使用动态分配内存的方式创建，本处采用的是使用静态
       变量的方式。建议对结构比较大的Applet使用静态变量的方式。对于动态分配
       内存的方式需要注意对所分配空间的释放。
       
==============================================================================*/
static int CQuickTest_New(IShell       *ps,
                          IModule      *pIModule,
                          IQuickTest **ppObj)
{
   CQuickTest *cq;
   int        Retval = SUCCESS;

   cq = MALLOC(sizeof(CQuickTest));
   if (NULL == cq) {
      return ENOMEMORY;
   }

   MEMSET(cq,
          0,
          sizeof(CQuickTest));

   INIT_VTBL(cq,
             IQuickTest,
             gIQuickTestMethods);

   cq->m_nRefs     = 1;
   cq->m_pShell    = ps;
   cq->m_pModule   = pIModule;

   ISHELL_AddRef(ps);
   IMODULE_AddRef(pIModule);

   Retval = CQuickTest_InitData(cq);

   *ppObj = (IQuickTest*)cq;
   return Retval;
}

/*==============================================================================
函数: 
       CQuickTest_AddRef
       
说明: 
       CQuickTest Applet 引用计数加1，并返回当前的引用数。
       
参数: 
       p [in]：IQuickTest接口对象指针。
       
返回值:
       当前对QuickTest Applet的引用数
       
备注:
       
==============================================================================*/
static uint32 CQuickTest_AddRef(IQuickTest *p)
{
   register CQuickTest *pMe = (CQuickTest*)p;

   return ++pMe->m_nRefs;
}

/*==============================================================================
函数: 
       CQuickTest_Release
       
说明: 
       释放对QuickTest Applet的引用，引用计数减1，并返回当前的引用数。
        如果当前的引用数为零，还将释放Applet及相关资源。
       
参数: 
       p [in]：IQuickTest接口对象指针。
       
返回值:
       当前对QuickTest Applet的引用数
       
备注:
       
==============================================================================*/
static uint32 CQuickTest_Release(IQuickTest *p)
{
   register CQuickTest *pMe = (CQuickTest*)p;

   if (--pMe->m_nRefs) {
      return pMe->m_nRefs;
   }

   CQuickTest_FreeAppData(pMe);

   ISHELL_Release(pMe->m_pShell);
   IMODULE_Release(pMe->m_pModule);

   FREE(pMe);

   return 0;
}

/*==============================================================================
函数: 
       CQuickTest_InitData
       
说明: 
       初始化CQuickTest Applet结构数据缺省值。
       
参数: 
       pMe [in]：指向CQuickTest Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值:
       SUCCESS：初始化成功。
       EFAILED：初始化失败。
       
备注:
       
==============================================================================*/
static int CQuickTest_InitData(CQuickTest *pMe)
{
   AEEDeviceInfo  di;
   
   if(pMe == NULL)
   {
      return EFAILED;
   }
   
   ISHELL_GetDeviceInfo(pMe->m_pShell, &di);

   pMe->m_rc.x       = 0;
   pMe->m_rc.y       = 0;
   pMe->m_rc.dx      = (int16) di.cxScreen;
   pMe->m_rc.dy      = (int16) di.cyScreen;

   pMe->m_bAppIsReady = FALSE;
   pMe->m_ePreState = QUICKTESTST_NONE;
   pMe->m_eCurState = QUICKTESTST_INIT;
   pMe->m_pActiveDlg = NULL;
   pMe->m_pActiveDlgID = IDD_MAINMENU;
   pMe->m_eDlgRet = DLGRET_CREATE;
   pMe->m_bNotOverwriteDlgRet = FALSE;
   pMe->m_mainMenuSel = 0;
#ifdef FEATURE_BREW_CAMERA    
   pMe->m_isFormCamera = FALSE;
#endif
   pMe->m_quicktestmic = FALSE;
   pMe->m_testkeycount = 0;
   if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                            AEECLSID_RINGERMGR,
                                            (void **)&pMe->m_pRingerMgr)) 
   {
       CQuickTest_FreeAppData(pMe);
       return EFAILED;
   } 

   if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                            AEECLSID_SOUND,
                                            (void **)&pMe->m_pISound))
   {
       CQuickTest_FreeAppData(pMe);
       return EFAILED;
   }

   if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                            AEECLSID_ALERT,
                                            (void **)&pMe->m_pIAlert))
   {
       CQuickTest_FreeAppData(pMe);
       return EFAILED;
   }

   if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                            AEECLSID_CONFIG,
                                            (void **)&pMe->m_pConfig))
   {
       CQuickTest_FreeAppData(pMe);
       return EFAILED;
   }

   if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                            AEECLSID_BACKLIGHT,
                                            (void **)&pMe->m_pIBacklight))
   {
       CQuickTest_FreeAppData(pMe);
       return EFAILED;
   }   
   
    if (ISHELL_CreateInstance(pMe->m_pShell,
                                 AEECLSID_ANNUNCIATOR,
                                 (void **) &pMe->m_pIAnn))
    {
        CQuickTest_FreeAppData(pMe);
        return FALSE;
    }

   if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                            AEECLSID_RUIM,
                                            (void **)&pMe->m_pIRUIM))
   {
       CQuickTest_FreeAppData(pMe);
       return EFAILED;
   }
    
   (void) ICONFIG_GetItem(pMe->m_pConfig,
                           CFGI_BACKLIGHT_LEVEL,
                           &pMe->m_nlightlevel,
                           sizeof(pMe->m_nlightlevel));
   pMe->colorMask    = 0;

   return SUCCESS;
}

/*==============================================================================
函数: 
       CQuickTest_FreeAppData
       
说明: 
       释放CQuickTest Applet使用的相关资源。
       
参数: 
       pMe [in]：指向CQuickTest Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值:
       无。
       
备注:
       
==============================================================================*/
static void CQuickTest_FreeAppData(CQuickTest *pMe)
{
    if (NULL == pMe)
    {
        return ;
    }

    if (pMe->m_pISound) 
    {
        ISOUND_Release(pMe->m_pISound);
        pMe->m_pISound = NULL;
    }
    
    if (pMe->m_pRingerMgr)
    {
        IRINGERMGR_Release(pMe->m_pRingerMgr);
        pMe->m_pRingerMgr = NULL;
    }
 
    if (pMe->m_pIAlert)
    {
        IALERT_Release(pMe->m_pIAlert);
        pMe->m_pIAlert = NULL;
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

    
    if (pMe->m_pIAnn) 
    {
        IANNUNCIATOR_Release(pMe->m_pIAnn);
        pMe->m_pIAnn= NULL;
    }

    if (pMe->m_pIRUIM != NULL)
    {
        IRUIM_Release(pMe->m_pIRUIM);
        pMe->m_pIRUIM = NULL;
    }
}

/*==============================================================================
函数: 
       CQuickTest_RunFSM
       
说明: 
       CQuickTest Applet有限状态机引擎。
       
参数: 
       pMe [in]：指向CQuickTest Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值:
       无。
       
备注:
       
==============================================================================*/
static void CQuickTest_RunFSM(CQuickTest *pMe)
{
    NextFSMAction nextFSMAction = NFSMACTION_WAIT;

    for ( ; ; )
    {
        nextFSMAction = QuickTest_ProcessState(pMe);

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
       CQuickTest_HandleEvent
       
说明: 
       CQuickTest  Applet事件处理主函数。所有属于本Applet的事件，
       都将经由该函数处理。
       
参数: 
       pi [in]：指向IQuickTest接口t对象的指针。
       eCode：BREW事件代码。
       wParam：与事件相关联的数据。
       dwParam [in]：与事件相关联的数据。
       
返回值:
       TRUE：传入事件被处理。
       FALSE：传入事件没被处理。
       
备注:
       
==============================================================================*/
static boolean CQuickTest_HandleEvent(IQuickTest *pi,
    AEEEvent eCode,
    uint16  wParam,
    uint32 dwParam
)
{
    CQuickTest *pMe = (CQuickTest*)pi;
    AEEAppStart *as;

    //MSG_FATAL("***zzg CQuickTest_HandleEvent eCode=%x***", eCode, 0, 0);
    
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
			if(STRCMP(as->pszArgs,"*#39#") == 0)
            {
            	 pMe->m_quicktestmic  = TRUE;
                 MOVE_TO_STATE(QUICKTESTST_CALLTEST)
            }
            pMe->m_lineheight = IDISPLAY_GetFontMetrics(pMe->m_pDisplay, AEE_FONT_BOLD, NULL, NULL);

            pMe->m_bSuspending = FALSE;

            // 开始CQuickTest状态机
            CQuickTest_RunFSM(pMe);
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
#ifdef FEATURE_BREW_CAMERA             
            if(pMe->m_isFormCamera)
            {
                MOVE_TO_STATE(QUICKTESTST_SDTEST)
                pMe->m_isFormCamera = FALSE;
            }
#endif            
            CQuickTest_RunFSM(pMe);
            return TRUE;

        case EVT_DIALOG_INIT:
            pMe->m_bAppIsReady = FALSE;
            QuickTest_UpdateActiveDialogInfo(pMe, wParam, dwParam);

            return QuickTest_RouteDialogEvent(pMe,eCode,wParam,dwParam);

        case EVT_DIALOG_START:
             (void)ISHELL_SetTimer ( pMe->m_pShell,
                            APPISREADY_TIMER,
                            QuickTest_APPIsReadyTimer,
                            pMe);
             
            return QuickTest_RouteDialogEvent(pMe,eCode,wParam,dwParam);

        case EVT_USER_REDRAW:
            MSG_FATAL("CQuickTest_HandleEvent EVT_USER_REDRAW",0,0,0);
            (void) QuickTest_RouteDialogEvent(pMe,eCode,wParam,dwParam);
            /*
            (void)ISHELL_SetTimer ( pMe->m_pShell,
                            APPISREADY_TIMER,
                            QuickTest_APPIsReadyTimer,
                            pMe);
            */
            return TRUE;

        case EVT_APPISREADY:
            (void) QuickTest_RouteDialogEvent(pMe,eCode,wParam,dwParam);
            pMe->m_bAppIsReady = TRUE;
            return TRUE;

        case EVT_KEY_PRESS:
			#if !defined(FEATURE_VERSION_K212) && !defined(FEATURE_VERSION_EC99) && !defined(FEATURE_QVGA_INHERIT_K212)
            if (!pMe->m_bAppIsReady)
            {
                return TRUE;
            }
			#endif
            MSG_FATAL("CQuickTest_HandleEvent EVT_KEY_PRESS",0,0,0);
            return QuickTest_RouteDialogEvent(pMe,eCode,wParam,dwParam);

        case EVT_KEY_RELEASE:
			#if !defined(FEATURE_VERSION_K212) && !defined(FEATURE_VERSION_EC99)&&!defined(FEATURE_QVGA_INHERIT_K212)
            if (!pMe->m_bAppIsReady)
            {
                return TRUE;
            }
			#endif
            MSG_FATAL("CQuickTest_HandleEvent EVT_KEY_RELEASE",0,0,0);
            return QuickTest_RouteDialogEvent(pMe,eCode,wParam,dwParam);

        case EVT_KEY:
			#if !defined(FEATURE_VERSION_K212) && !defined(FEATURE_VERSION_EC99)&&!defined(FEATURE_QVGA_INHERIT_K212)
            if (!pMe->m_bAppIsReady)
            {
            	MSG_FATAL("CQuickTest_HandleEvent EVT_KEY",0,0,0);
                return TRUE;
            }
			#endif
            MSG_FATAL("CQuickTest_HandleEvent EVT_KEY",0,0,0);
            return QuickTest_RouteDialogEvent(pMe,eCode,wParam,dwParam);

        case EVT_COMMAND:
			#if !defined(FEATURE_VERSION_K212) && !defined(FEATURE_VERSION_EC99)&&!defined(FEATURE_QVGA_INHERIT_K212)
            if (!pMe->m_bAppIsReady)
            {
                return TRUE;
            }
			#endif
            MSG_FATAL("CQuickTest_HandleEvent EVT_COMMAND",0,0,0);
            return QuickTest_RouteDialogEvent(pMe,eCode,wParam,dwParam);

        case EVT_DIALOG_END:
			
            if (wParam == 0)
            {
                return TRUE;
            }
            pMe->m_bAppIsReady = FALSE;
            MSG_FATAL("CQuickTest_HandleEvent EVT_DIALOG_END",0,0,0);
            (void) QuickTest_RouteDialogEvent(pMe,eCode,wParam,dwParam);
            pMe->m_pActiveDlg = NULL;

            // Applet被挂起，不跑状态机，等待Applet返回。（注意：EVT_APP_SUSPEND
            // 事件在EVT_DIALOG_END事件前发出。
            if (pMe->m_bSuspending == FALSE)
            {
                // 开始CQuickTest状态机
                CQuickTest_RunFSM(pMe);
            }
            return TRUE;

        default:
            // 将接收到的事件路由至当前活动的对话框事件处理函数。
            return QuickTest_RouteDialogEvent(pMe,eCode,wParam,dwParam);
    }
}

/*==============================================================================
函数: 
       QuickTest_APPIsReadyTimer
       
说明: 
       定时器回调函数。主要用于控制快速按键问题的处理。
       
参数: 
       pme [in]：void *类型指针，暗指CQuickTest结构指针。
       
返回值:
       无。
       
备注:
       
==============================================================================*/
static void QuickTest_APPIsReadyTimer(void *pme)
{
    CQuickTest *pMe = (CQuickTest *)pme;

    if (NULL == pMe)
    {
        return;
    }

    (void) ISHELL_PostEvent( pMe->m_pShell,
                            AEECLSID_QUICKTEST,
                            EVT_APPISREADY,
                            0,
                            0);
}

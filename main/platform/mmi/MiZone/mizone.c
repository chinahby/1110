/*==============================================================================
// 文件：
//        IMiZone.c
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
#include "Mizone_priv.h"

/*==============================================================================
                                 宏定义和常数
==============================================================================*/

/*==============================================================================
                                 类型定义
==============================================================================*/

/*==============================================================================
                                 函数声明
==============================================================================*/

static void MiZone_APPIsReadyTimer(void *pme);
/*----------------------模块相关函数声明---------------------*/
#if defined(BREW_STATIC_APP)
                                    
int MiZone_Load(IShell   *pIShell, 
                             void     *ph, 
                             IModule **ppMod);

static int MiZone_CreateInstance( AEECLSID ClsId,
                                     IShell  *pIShell,
                                     IModule *po,
                                     void   **ppObj);
#endif

/*----------------------Applet相关函数声明---------------------*/

/*----------------------Applet相关函数声明---------------------*/
static int     CMiZone_New(IShell       *ps,
                              IModule      *pIModule,
                              IMiZone **ppObj);
static uint32  CMiZone_AddRef(IMiZone *pi);
static uint32  CMiZone_Release(IMiZone *pi);
static boolean CMiZone_HandleEvent(IMiZone *pi,
                                      AEEEvent    eCode,
                                      uint16      wParam,
                                      uint32      dwParam);
static int     CMiZone_InitData(CMiZone *pMe);
static void    CMiZone_FreeAppData(CMiZone *pMe);
static void    CMiZone_RunFSM(CMiZone *pMe);
/*==============================================================================
                                 全局数据
==============================================================================*/

/*==============================================================================
                                 本地（静态）数据
==============================================================================*/

// 只允许一个CMiZone实例。每次创建CMiZone Applet时，
// 返回同一结构指针给BREW层。
//static CMiZone gMiZone;

static const VTBL(IMiZone) gIMiZoneMethods =
{
   CMiZone_AddRef,
   CMiZone_Release,
   CMiZone_HandleEvent
};

/*----------------------模块相关函数定义---------------------*/

#if defined(BREW_STATIC_APP)

/*==============================================================================
函数: 
       MiZone_Load
       
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
int MiZone_Load(IShell   *pIShell,
                             void     *ph,
                             IModule **ppMod)
{
   return AEEStaticMod_New(sizeof(AEEMod),
                           pIShell,
                           ph,
                           ppMod,
                           (PFNMODCREATEINST)MiZone_CreateInstance,
                           (PFNFREEMODDATA)NULL);
}
#endif
/*==============================================================================
函数: 
       MiZone_CreateInstance
       
说明: 
        通过创建(初始化)MiZone对象来创建模块实例。
       
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
static int MiZone_CreateInstance( AEECLSID ClsId,
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

   if (ClsId != AEECLSID_MiZone) {
      return EFAILED;
   }
   if (CMiZone_New(pIShell, po, (IMiZone**)ppObj) != SUCCESS) {
      return EFAILED;
   }
   return SUCCESS;
}
/*----------------------Applet相关函数定义---------------------*/
/*==============================================================================
函数: 
       CMiZone_New
       
说明: 
        CMiZone Applet虚拟表和内部数据变量，同时其引用计数加1。
       
参数: 
       ps [in]：IShell接口对象指针。
       pIModule [in]：pIModule接口对象指针。
       ppObj [out]:：指向IMiZone对象指针的指针。
       
返回值:
       EFAILED：失败。
       SUCCESS：成功。
       
备注:
       不需用户去调用。
       关于Applet结构对象可使用动态分配内存的方式创建，本处采用的是使用静态
       变量的方式。建议对结构比较大的Applet使用静态变量的方式。对于动态分配
       内存的方式需要注意对所分配空间的释放。
       
==============================================================================*/
static int CMiZone_New(IShell       *ps,
                          IModule      *pIModule,
                          IMiZone **ppObj)
{
   CMiZone *cq;
   int        Retval = SUCCESS;
   MSG_FATAL("***CMiZone_New****",0,0,0);
   cq = MALLOC(sizeof(CMiZone));
   if (NULL == cq) {
      return ENOMEMORY;
   }

   MEMSET(cq,
          0,
          sizeof(CMiZone));

   INIT_VTBL(cq,
             IMiZone,
             gIMiZoneMethods);

   cq->m_nRefs     = 1;
   cq->m_pShell    = ps;
   cq->m_pModule   = pIModule;

   ISHELL_AddRef(ps);
   IMODULE_AddRef(pIModule);

   Retval = CMiZone_InitData(cq);

   *ppObj = (IMiZone*)cq;
   return Retval;
}

/*==============================================================================
函数: 
       CMiZone_AddRef
       
说明: 
       CMiZone Applet 引用计数加1，并返回当前的引用数。
       
参数: 
       p [in]：IMiZone接口对象指针。
       
返回值:
       当前对MiZone Applet的引用数
       
备注:
       
==============================================================================*/
static uint32 CMiZone_AddRef(IMiZone *p)
{
   register CMiZone *pMe = (CMiZone*)p;

   return ++pMe->m_nRefs;
}

/*==============================================================================
函数: 
       CMiZone_Release
       
说明: 
       释放对MiZone Applet的引用，引用计数减1，并返回当前的引用数。
        如果当前的引用数为零，还将释放Applet及相关资源。
       
参数: 
       p [in]：IMiZone接口对象指针。
       
返回值:
       当前对MiZone Applet的引用数
       
备注:
       
==============================================================================*/
static uint32 CMiZone_Release(IMiZone *p)
{
   register CMiZone *pMe = (CMiZone*)p;

   if (--pMe->m_nRefs) {
      return pMe->m_nRefs;
   }

   CMiZone_FreeAppData(pMe);

   ISHELL_Release(pMe->m_pShell);
   IMODULE_Release(pMe->m_pModule);

   FREE(pMe);

   return 0;
}

/*==============================================================================
函数: 
       CMiZone_InitData
       
说明: 
       初始化CMiZone Applet结构数据缺省值。
       
参数: 
       pMe [in]：指向CMiZone Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值:
       SUCCESS：初始化成功。
       EFAILED：初始化失败。
       
备注:
       
==============================================================================*/
static int CMiZone_InitData(CMiZone *pMe)
{
   AEEDeviceInfo  di;
   
   if(pMe == NULL)
   {
      return EFAILED;
   }
   MSG_FATAL("***CMiZone_InitData****1",0,0,0);
   ISHELL_GetDeviceInfo(pMe->m_pShell, &di);

   pMe->m_rc.x       = 0;
   pMe->m_rc.y       = 0;
   pMe->m_rc.dx      = (int16) di.cxScreen;
   pMe->m_rc.dy      = (int16) di.cyScreen;

   pMe->m_bAppIsReady = FALSE;
   pMe->m_ePreState = MIZONE_NONE;
   pMe->m_eCurState = MIZONE_MAIN;
   pMe->m_pActiveDlg = NULL;
   pMe->m_pActiveDlgID = IDD_MAINMENU;
   pMe->m_eDlgRet = DLGRET_CREATE;
   pMe->m_bNotOverwriteDlgRet = FALSE;
   pMe->m_mainMenuSel = 0;
   
    if(AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                 AEECLSID_ANNUNCIATOR,
                                (void **) &pMe->m_pIAnn))
    {
       MSG_FATAL("***CMiZone_InitData****2",0,0,0);
       CMiZone_FreeAppData(pMe);
       return FALSE;
    }
    if(ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_STATIC,
                                (void **)&pMe->m_pStatic) != SUCCESS)
    {
        DBGPRINTF("The creation of static interface has failed!" );
        return FALSE;
    }  
   if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,AEECLSID_CONFIG,
                                            (void **)&pMe->m_pConfig))
    {
       return FALSE;
    }
   if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,AEECLSID_RUIM,
                                            (void **)&pMe->m_pIRUIM))
    {
       return FALSE;
    }
     MSG_FATAL("***CMiZone_InitData****pMe->m_eCurState=%d",pMe->m_eCurState,0,0);
   return SUCCESS;
}

/*==============================================================================
函数: 
       CMiZone_FreeAppData
       
说明: 
       释放CMiZone Applet使用的相关资源。
       
参数: 
       pMe [in]：指向CMiZone Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值:
       无。
       
备注:
       
==============================================================================*/
static void CMiZone_FreeAppData(CMiZone *pMe)
{
    if (NULL == pMe)
    {
        return ;
    }
    MSG_FATAL("***CMiZone_FreeAppData****",0,0,0);

    if (pMe->m_pIAnn) 
    {
        IANNUNCIATOR_Release(pMe->m_pIAnn);
        pMe->m_pIAnn= NULL;
    }
    if(pMe->m_pStatic != NULL)
    {
        ISTATIC_Release(pMe->m_pStatic);
        pMe->m_pStatic = NULL; 
    }
    if (pMe->m_pConfig)
    {
        ICONFIG_Release(pMe->m_pConfig);
        pMe->m_pConfig = NULL;
    }
    if (pMe->m_pIRUIM != NULL)
    {
        IRUIM_Release(pMe->m_pIRUIM);
        pMe->m_pIRUIM = NULL;
    }
}

/*==============================================================================
函数: 
       CMiZone_RunFSM
       
说明: 
       CMiZone Applet有限状态机引擎。
       
参数: 
       pMe [in]：指向CMiZone Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值:
       无。
       
备注:
       
==============================================================================*/
static void CMiZone_RunFSM(CMiZone *pMe)
{
    NextFSMAction nextFSMAction = NFSMACTION_WAIT;

    for ( ; ; )
    {
        nextFSMAction = MiZone_ProcessState(pMe);

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
       CMiZone_HandleEvent
       
说明: 
       CMiZone  Applet事件处理主函数。所有属于本Applet的事件，
       都将经由该函数处理。
       
参数: 
       pi [in]：指向IMiZone接口t对象的指针。
       eCode：BREW事件代码。
       wParam：与事件相关联的数据。
       dwParam [in]：与事件相关联的数据。
       
返回值:
       TRUE：传入事件被处理。
       FALSE：传入事件没被处理。
       
备注:
       
==============================================================================*/
static boolean CMiZone_HandleEvent(IMiZone *pi,
    AEEEvent eCode,
    uint16  wParam,
    uint32 dwParam
)
{
    CMiZone *pMe = (CMiZone*)pi;
    AEEAppStart *as;
    MSG_FATAL("***CMiZone_HandleEvent****eCode=%d",eCode,0,0);
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
            pMe->m_lineheight = IDISPLAY_GetFontMetrics(pMe->m_pDisplay, AEE_FONT_BOLD, NULL, NULL);

            pMe->m_bSuspending = FALSE;

            // 开始CMiZone状态机
            CMiZone_RunFSM(pMe);
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
            CMiZone_RunFSM(pMe);
            return TRUE;

        case EVT_DIALOG_INIT:
            pMe->m_bAppIsReady = FALSE;
            MiZone_UpdateActiveDialogInfo(pMe, wParam, dwParam);

            return MiZone_RouteDialogEvent(pMe,eCode,wParam,dwParam);

        case EVT_DIALOG_START:
            (void)ISHELL_SetTimer ( pMe->m_pShell,
                            APPISREADY_TIMER,
                            MiZone_APPIsReadyTimer,
                            pMe);
            return MiZone_RouteDialogEvent(pMe,eCode,wParam,dwParam);

        case EVT_USER_REDRAW:
            MSG_FATAL("CMiZone_HandleEvent EVT_USER_REDRAW",0,0,0);
            (void) MiZone_RouteDialogEvent(pMe,eCode,wParam,dwParam);
            /*
            (void)ISHELL_SetTimer ( pMe->m_pShell,
                            APPISREADY_TIMER,
                            MiZone_APPIsReadyTimer,
                            pMe);
            */
            return TRUE;

        case EVT_APPISREADY:
            (void) MiZone_RouteDialogEvent(pMe,eCode,wParam,dwParam);
            pMe->m_bAppIsReady = TRUE;
            return TRUE;

        case EVT_KEY_PRESS:
			#if !defined(FEATURE_VERSION_K212) && !defined(FEATURE_VERSION_EC99)&&!defined(FEATURE_QVGA_INHERIT_K212)&& !defined(FEATURE_LOW_MEM_BIGFONT)&&!defined(FEATURE_VERSION_IN50A)
            if (!pMe->m_bAppIsReady)
            {
                return TRUE;
            }
			#endif
            MSG_FATAL("CMiZone_HandleEvent EVT_KEY_PRESS",0,0,0);
            return MiZone_RouteDialogEvent(pMe,eCode,wParam,dwParam);

        case EVT_KEY_RELEASE:
			#if !defined(FEATURE_VERSION_K212) && !defined(FEATURE_VERSION_EC99)&&!defined(FEATURE_QVGA_INHERIT_K212)&& !defined(FEATURE_LOW_MEM_BIGFONT)&&!defined(FEATURE_VERSION_IN50A)
            if (!pMe->m_bAppIsReady)
            {
                return TRUE;
            }
			#endif
            MSG_FATAL("CMiZone_HandleEvent EVT_KEY_RELEASE",0,0,0);
            return MiZone_RouteDialogEvent(pMe,eCode,wParam,dwParam);

        case EVT_KEY:
			#if !defined(FEATURE_VERSION_K212) && !defined(FEATURE_VERSION_EC99)&&!defined(FEATURE_QVGA_INHERIT_K212)&& !defined(FEATURE_LOW_MEM_BIGFONT)&&!defined(FEATURE_VERSION_IN50A)
            if (!pMe->m_bAppIsReady)
            {
            	MSG_FATAL("CMiZone_HandleEvent EVT_KEY",0,0,0);
                return TRUE;
            }
			#endif
            MSG_FATAL("CMiZone_HandleEvent EVT_KEY",0,0,0);
            return MiZone_RouteDialogEvent(pMe,eCode,wParam,dwParam);

        case EVT_COMMAND:
			#if !defined(FEATURE_VERSION_K212) && !defined(FEATURE_VERSION_EC99)&&!defined(FEATURE_QVGA_INHERIT_K212)&& !defined(FEATURE_LOW_MEM_BIGFONT)&&!defined(FEATURE_VERSION_IN50A)
            if (!pMe->m_bAppIsReady)
            {
                return TRUE;
            }
			#endif
            MSG_FATAL("CMiZone_HandleEvent EVT_COMMAND",0,0,0);
            return MiZone_RouteDialogEvent(pMe,eCode,wParam,dwParam);

        case EVT_DIALOG_END:
			
            if (wParam == 0)
            {
                return TRUE;
            }
            pMe->m_bAppIsReady = FALSE;
            MSG_FATAL("CMiZone_HandleEvent EVT_DIALOG_END",0,0,0);
            (void) MiZone_RouteDialogEvent(pMe,eCode,wParam,dwParam);
            pMe->m_pActiveDlg = NULL;

            // Applet被挂起，不跑状态机，等待Applet返回。（注意：EVT_APP_SUSPEND
            // 事件在EVT_DIALOG_END事件前发出。
            if (pMe->m_bSuspending == FALSE)
            {
                // 开始CMiZone状态机
                CMiZone_RunFSM(pMe);
            }
            return TRUE;

        default:
            // 将接收到的事件路由至当前活动的对话框事件处理函数。
            return MiZone_RouteDialogEvent(pMe,eCode,wParam,dwParam);
    }
}

/*==============================================================================
函数: 
       MiZone_APPIsReadyTimer
       
说明: 
       定时器回调函数。主要用于控制快速按键问题的处理。
       
参数: 
       pme [in]：void *类型指针，暗指CMiZone结构指针。
       
返回值:
       无。
       
备注:
       
==============================================================================*/
static void MiZone_APPIsReadyTimer(void *pme)
{
    CMiZone *pMe = (CMiZone *)pme;

    if (NULL == pMe)
    {
        return;
    }

    (void) ISHELL_PostEvent( pMe->m_pShell,
                            AEECLSID_MiZone,
                            EVT_APPISREADY,
                            0,
                            0);
}

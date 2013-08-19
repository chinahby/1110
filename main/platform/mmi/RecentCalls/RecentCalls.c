/*==============================================================================
// 文件：
//        recentcalls.c
//        2007-11-18 陈喜玲草拟版本(Draft Version)
//        版权所有(c) 2007 Anylook
//        
// 文件说明：
//        
// 作者：2007-11-18
// 创建日期：陈喜玲
// 当前版本：Draft
==============================================================================*/

/*==============================================================================
修改历史记录：
时       间      修 改 人       修改内容、位置及原因
-----------      ----------     -----------------------------------------------
07-11-18         陈喜玲         初始版本
==============================================================================*/


/*==============================================================================
                                 本文件包含的外部文件
==============================================================================*/
#include "recentcalls_priv.h"

/*==============================================================================
                                 宏定义和常数
==============================================================================*/

/*==============================================================================
                                 类型定义
==============================================================================*/

/*==============================================================================
                                 函数声明
==============================================================================*/
static void recentcalls_APPIsReadyTimer(void *pme);

/*----------------------模块相关函数声明---------------------*/

int  RecentCalls_Load(IShell   *pIShell,
    void     *ph,
    IModule **ppMod
);

int  recentcallsMod_New(int16  nSize,
    IShell   *pIShell,
    void     *ph,
    IModule  **ppMod,
    PFNMODCREATEINST pfnMC,
    PFNFREEMODDATA pfnMF
);

static int  recentcallsMod_CreateInstance(IModule *po,
    IShell *pIShell,
    AEECLSID ClsId,
    void **ppObj
);

static uint32 recentcallsMod_AddRef(IModule *po);

static uint32 recentcallsMod_Release(IModule *po);

static void recentcallsMod_FreeResources(IModule  *po,
    IHeap  *ph,
    IFileMgr *pfm
);

/*----------------------Applet相关函数声明---------------------*/
static int recentcalls_New(IShell *ps,
    IModule *pIModule,
    IRecentCalls **ppObj
);

static uint32  recentcalls_AddRef(IRecentCalls *pi);

static uint32  recentcalls_Release (IRecentCalls *pi);

static boolean recentcalls_HandleEvent(IRecentCalls *pi,
    AEEEvent eCode,
    uint16  wParam,
    uint32 dwParam
);

static void    recentcalls_SendList (IRecentCalls *pi);

static void    recentcalls_CallList (IRecentCalls *pi, PFNSELECTEDRECENTCALLSCB cb, SelectRecordType type);

static void    recentcalls_MissedList (IRecentCalls *pi);

static int recentcalls_InitAppData(CRecentCalls *pMe);

static void recentcalls_FreeAppData(CRecentCalls *pMe);

static void recentcalls_RunFSM(CRecentCalls *pMe);

/*==============================================================================
                                 全局数据
==============================================================================*/
// 重要提示：
//        g_dwAEEStdLibEntry全局变量仅用于SDK ！
//        BREW SDK用户不能改写和直接使用本变量。它用作AEEStdLib的一个入口点，不
//        要移动下列三行代码。
#ifdef AEE_SIMULATOR
   uint32 g_dwAEEStdLibEntry;
   #error This applet was never intended to be used with the AEE Simulator.
#endif

/*==============================================================================
                                 本地（静态）数据
==============================================================================*/
static recentcallsMod grecentcallsMod;

static const VTBL(IModule) gModFuncs =
{
    recentcallsMod_AddRef,
    recentcallsMod_Release,
    recentcallsMod_CreateInstance,
    recentcallsMod_FreeResources
};

// 只允许一个recentcalls实例。每次创建recentcalls Applet时，
// 返回同一结构指针给BREW层。
static CRecentCalls grecentcalls;

static const VTBL(IRecentCalls) grecentcallsMethods =
{
    recentcalls_AddRef,
    recentcalls_Release,
    recentcalls_HandleEvent,
    recentcalls_SendList,
    recentcalls_MissedList,
    recentcalls_CallList
};

/*----------------------模块相关函数定义---------------------*/

/*==============================================================================
函数: 
       RecentCalls_Load
       
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
#ifdef AEE_SIMULATOR
__declspec(dllexport) int AEEMod_Load(IShell   *pIShell,
                                                        void     *ph,
                                                        IModule **ppMod)
#else
int RecentCalls_Load(IShell   *pIShell,
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

    return recentcallsMod_New(sizeof(recentcallsMod),
                    pIShell,
                    ph,
                    ppMod,
                    NULL,
                    NULL);
}

/*==============================================================================
函数: 
       recentcallsMod_New
       
说明: 
       用来初始化recentcalls模块虚拟表（也增加引用计数）的助手函数。
       
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
int  recentcallsMod_New(int16  nSize,
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

    MEMSET(&grecentcallsMod,0,sizeof(recentcallsMod));

    INIT_VTBL(&grecentcallsMod,IModule,gModFuncs);
    grecentcallsMod.m_nRefs = 1;
    *ppMod = (IModule *)&grecentcallsMod;
    return AEE_SUCCESS;
}

/*==============================================================================
函数: 
       recentcallsMod_AddRef
       
说明: 
       recentcalls 模块引用计数加1，并返回当前的引用数。
       
参数: 
       po [in]：IModule接口对象指针。
       
返回值:
       当前对recentcalls 模块的引用数
       
备注:
       
==============================================================================*/
static uint32 recentcallsMod_AddRef(IModule *po)
{
    return(++((recentcallsMod *)po)->m_nRefs);
}

/*==============================================================================
函数: 
       recentcallsMod_Release
       
说明: 
       recentcalls 模块引用计数减1，并返回当前的引用数。
       
参数: 
       po [in]：IModule接口对象指针。
       
返回值:
       当前对recentcalls 模块的引用数
       
备注:
       
==============================================================================*/
static uint32 recentcallsMod_Release(IModule *po)
{
    if (((recentcallsMod *) po)->m_nRefs == 0)
    {
        return 0;
    }

    return(--((recentcallsMod *)po)->m_nRefs);
}

/*==============================================================================
函数: 
       recentcallsMod_CreateInstance
       
说明: 
        通过创建(初始化)recentcalls对象来创建模块实例。
       
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
static int  recentcallsMod_CreateInstance(IModule *po,
    IShell *pIShell,
    AEECLSID ClsId,
    void **ppObj
)
{
    *ppObj = NULL;

    if (ClsId != AEECLSID_APP_RECENTCALL)
    {
        return EFAILED;
    }

    if (recentcalls_New(pIShell, po, (IRecentCalls**)ppObj) != SUCCESS)
    {
        return EFAILED;
    }
    return SUCCESS;
}

/*==============================================================================
函数: 
       recentcallsMod_FreeResources
       
说明: 
        释放recentcalls模块资源的函数。
       
参数: 
       po ：IModule。
       ph ：IHeap接口指针。
       pfm :IFileMgr接口指针。
       
返回值:
       无
       
备注:
       不需用户去调用。
       
==============================================================================*/
static void recentcallsMod_FreeResources(IModule  *po,
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
       recentcalls_New
       
说明: 
        初始化recentcalls Applet虚拟表和内部数据变量，同时其引用计数加1。
       
参数: 
       ps [in]：IShell接口对象指针。
       pIModule [in]：pIModule接口对象指针。
       ppObj [out]:：指向IRecentCalls对象指针的指针。
       
返回值:
       EFAILED：失败。
       SUCCESS：成功。
       
备注:
       不需用户去调用。
       关于Applet结构对象可使用动态分配内存的方式创建，本处采用的是使用静态
       变量的方式。建议对结构比较大的Applet使用静态变量的方式。对于动态分配
       内存的方式需要注意对所分配空间的释放。
       
==============================================================================*/
static int recentcalls_New(IShell *ps,
    IModule *pIModule,
    IRecentCalls **ppObj
)
{
    int retVal = SUCCESS;

    if (0 == grecentcalls.m_nRefs)
    {
        // Must initialize the object
        MEMSET(&grecentcalls,  0, sizeof(CRecentCalls));
        INIT_VTBL(&grecentcalls, IRecentCalls, grecentcallsMethods);

        grecentcalls.m_nRefs     = 0;
        grecentcalls.m_pShell    = ps;
        grecentcalls.m_pModule   = pIModule;

        (void) ISHELL_AddRef(ps);
        (void) IMODULE_AddRef(pIModule);

        retVal = recentcalls_InitAppData(&grecentcalls);
    }

    ++grecentcalls.m_nRefs;
    *ppObj = (IRecentCalls*)&grecentcalls;
    return retVal;
}

/*==============================================================================
函数: 
       recentcalls_AddRef
       
说明: 
       recentcalls Applet 引用计数加1，并返回当前的引用数。
       
参数: 
       pi [in]：IRecentCalls接口对象指针。
       
返回值:
       当前对recentcalls Applet的引用数
       
备注:
       
==============================================================================*/
static uint32  recentcalls_AddRef(IRecentCalls *pi)
{
    register CRecentCalls *pMe = (CRecentCalls*)pi;

    return (++pMe->m_nRefs);
}

/*==============================================================================
函数: 
       recentcalls_Release
       
说明: 
       释放对recentcalls Applet的引用，引用计数减1，并返回当前的引用数。
        如果当前的引用数为零，还将释放Applet及相关资源。
       
参数: 
       pi [in]：IRecentCalls接口对象指针。
       
返回值:
       当前对recentcalls Applet的引用数
       
备注:
       
==============================================================================*/
static uint32  recentcalls_Release (IRecentCalls *pi)
{
    register CRecentCalls *pMe = (CRecentCalls*)pi;

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
    recentcalls_FreeAppData(pMe);

    (void) ISHELL_Release(pMe->m_pShell);
    (void) IMODULE_Release(pMe->m_pModule);

    // 注意：pMe是静态分配空间，因此不需要释放。FREE()
    return 0;
}

/*==============================================================================
recentcalls_SendList

说明：
      待机界面按SEND键的时候,列出的最近的呼出记录

参数：
       pi [in/out]：IRecentCalls接口对象指针。

返回值：
       当前对SounMenu Applet的引用数

备注：:

==============================================================================*/
static void  recentcalls_SendList (IRecentCalls *pi)
{
   register CRecentCalls *pMe = (CRecentCalls*)pi;
   char     *args;
   
   args = MALLOC(4);
   *args = STARTARG_ALLCALL;

   pMe->m_callsCategory = AEECALLHISTORY_CALL_TYPE_ALL;//CALLHISTORY_OUTGOING_CATEGORY;
   //pMe->m_eStartMethod = STARTMETHOD_NORMAL;

   (void)ISHELL_StartAppletArgs(pMe->m_pShell,
                          AEECLSID_APP_RECENTCALL,
                          args);
   if(args)
   {
      FREE(args);
   }
}

/*==============================================================================
recentcalls_RecentCallList


说明：
      待机界面按SEND键的时候,列出的最近的呼出记录

参数：
       pi [in/out]：IRecentCalls接口对象指针。

返回值：
       当前对SounMenu Applet的引用数

备注：:

==============================================================================*/
static void  recentcalls_CallList (IRecentCalls *pi, PFNSELECTEDRECENTCALLSCB cb, SelectRecordType type)
{
   CRecentCalls *pMe = (CRecentCalls*)pi;
   char     *args;
   char      buf[20];
   
   switch(type)
   {
       case SELECT_ONE_NUMBER:
           SPRINTF(buf, "%c%d", STARTARG_ONECALL, (uint32) cb);
           break;
           
       case SELECT_MULTI_NUMBER:
       default :
           SPRINTF(buf, "%c%d", STARTARG_ALLCALL, (uint32) cb);
           break;

   }
   
   args = STRDUP(buf);
    
   (void)ISHELL_StartAppletArgs(pMe->m_pShell,
                          AEECLSID_APP_RECENTCALL,
                          args);
   if(args)
   {
      FREE(args);
   }
}

/*==============================================================================
recentcalls_MissedList

说明：
      在错过来电时候,按SOFTKEY进入错过来电列表

参数：
       pi [in/out]：IRecentCalls接口对象指针。

返回值：
       当前对SounMenu Applet的引用数

备注：:

==============================================================================*/
static void  recentcalls_MissedList (IRecentCalls *pi)
{
   register CRecentCalls *pMe = (CRecentCalls*)pi;
   char     *args;
   
   args = MALLOC(4);
   *args = STARTARG_MISSEDCALL;
   pMe->m_callsCategory = AEECALLHISTORY_CALL_TYPE_MISSED;//CALLHISTORY_MISSED_CATEGORY;
   pMe->m_eCurState = STATE_VERIFY_PASSWORD;
   pMe->record_selected = 0;
   (void)ISHELL_StartAppletArgs(pMe->m_pShell,
                          AEECLSID_APP_RECENTCALL,
                          args);
   (void) ISHELL_PostEvent(pMe->m_pShell,
                           AEECLSID_CALL,
                           EVT_USER_CLOSEAPP,
                           0,
                           0);      
   if(args)
   {
      FREE(args);
   }
}

/*==============================================================================
函数: 
       recentcalls_InitAppData
       
说明: 
       初始化recentcalls Applet结构数据缺省值。
       
参数: 
       pMe [in]：指向recentcalls Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值:
       SUCCESS：初始化成功。
       EFAILED：初始化失败。
       
备注:
       
==============================================================================*/
static int recentcalls_InitAppData(CRecentCalls *pMe)
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
    pMe->m_ePreState = STATE_REXIT;
    pMe->m_eCurState = STATE_VERIFY_PASSWORD;
    pMe->m_pDialog = NULL;
    pMe->m_eDlgRet = DLGRET_CREATE;
    pMe->m_bNotOverwriteDlgRet = FALSE;
    pMe->key_enable = TRUE; 
    pMe->record_selected = 0;
    pMe->m_currDlgId = 0;
    if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                             AEECLSID_CONFIG,
                                             (void **)&pMe->m_pConfig))
    
    {
        return EFAILED;
    }
#if 0
    if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                             AEECLSID_CALLLIST,
                                             (void **)&pMe->m_pCallList))
    
    {
        return EFAILED;
    }
#endif
    if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                             AEECLSID_CALLHISTORY,
                                             (void **)&pMe->m_pCallHistory))
    
    {
        recentcalls_FreeAppData( pMe); // 2008.12.2
        return EFAILED;
    }
    if( ISHELL_CreateInstance( pMe->m_pShell,
                             AEECLSID_TEXTCTL,
                             (void **)&pMe->m_pPhoneLockPasswordTextCtl
                         ) != AEE_SUCCESS
    )
    {
        recentcalls_FreeAppData( pMe); // 2008.03.14
        return EFAILED;
    }
    if( ISHELL_CreateInstance( pMe->m_pShell,
                             AEECLSID_STATIC,
                             (void **)&pMe->m_pStatic
                         ) != SUCCESS
    )
    {
        recentcalls_FreeAppData( pMe);
        return EFAILED;
    }    
    pMe->m_pPhoneLockPassword = NULL;

    if (ISHELL_CreateInstance(pMe->m_pShell,
                                 AEECLSID_ANNUNCIATOR,
                                 (void **) &pMe->m_pIAnn))
    {
        return EFAILED;
    }
	MSG_FATAL("IANNUNCIATOR_SetFieldIsActiveEx::::recentcall111:::",0,0,0);
	//IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE);
#ifdef FEATURE_LED_CONTROL
    if (ISHELL_CreateInstance(pMe->m_pShell,
                                            AEECLSID_BACKLIGHT,
                                            (void **)&pMe->m_pBacklight)!=AEE_SUCCESS)
    {
        return EFAILED;
    }
#endif    
    pMe->m_selectedItem[0] = 0;
    pMe->m_selectedItem[1] = 0; 

    return SUCCESS;
}

/*==============================================================================
函数: 
       recentcalls_FreeAppData
       
说明: 
       释放recentcalls Applet使用的相关资源。
       
参数: 
       pMe [in]：指向recentcalls Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值:
       无。
       
备注:
       
==============================================================================*/
static void recentcalls_FreeAppData(CRecentCalls *pMe)
{
    int j;
    
    if (NULL == pMe)
    {
        return ;
    }
    
    for(j=0; j<AEECALLHISTORY_MAX_CALLS; j++)
    {
        FREEIF(pMe->list_record[j].name);
        FREEIF(pMe->list_record[j].number);
    }
    
    if(pMe->Image){ 
       IIMAGE_Release(pMe->Image); 
       pMe->Image = NULL;
    }
    
    if(pMe->m_pConfig)
    {
       ICONFIG_Release(pMe->m_pConfig);
       pMe->m_pConfig = NULL;
    } 
    #if 0
    if(pMe->m_pCallList)
    {
       ICALLLIST_Release(pMe->m_pCallList);
       pMe->m_pCallList = NULL;
    } 
    #endif
    if(pMe->m_pCallHistory)
    {
       ICALLHISTORY_Release(pMe->m_pCallHistory);
       pMe->m_pCallHistory = NULL;
    }
    if( pMe->m_pPhoneLockPasswordTextCtl)
    {
        ITEXTCTL_Release( pMe->m_pPhoneLockPasswordTextCtl);
        pMe->m_pPhoneLockPasswordTextCtl = NULL;
    }
    if( pMe->m_pStatic != NULL)
    {
        ISTATIC_Release(pMe->m_pStatic);
        pMe->m_pStatic = NULL;        
    }
    FREEIF( pMe->m_pPhoneLockPassword); // 2008.03.18

    if (pMe->m_pIAnn)
    {
        IANNUNCIATOR_Release(pMe->m_pIAnn);
        pMe->m_pIAnn= NULL;
    }
#ifdef FEATURE_LED_CONTROL
    if(pMe->m_pBacklight)
    {
        IBACKLIGHT_Release(pMe->m_pBacklight);
        pMe->m_pBacklight=NULL;
    }
#endif

    RecentCalls_FreeSelectRecordListNode(pMe);

}

/*==============================================================================
函数: 
       recentcalls_RunFSM
       
说明: 
       recentcalls Applet有限状态机引擎。
       
参数: 
       pMe [in]：指向recentcalls Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值:
       无。
       
备注:
       
==============================================================================*/
static void recentcalls_RunFSM(CRecentCalls *pMe)
{
    NextFSMAction nextFSMAction = NFSMACTION_WAIT;

    for ( ; ; )
    {
        nextFSMAction = recentcalls_ProcessState(pMe);

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
       recentcalls_HandleEvent
       
说明: 
       recentcalls  Applet事件处理主函数。所有属于本Applet的事件，
       都将经由该函数处理。
       
参数: 
       pi [in]：指向IRecentCalls接口t对象的指针。
       eCode：BREW事件代码。
       wParam：与事件相关联的数据。
       dwParam [in]：与事件相关联的数据。
       
返回值:
       TRUE：传入事件被处理。
       FALSE：传入事件没被处理。
       
备注:
       
==============================================================================*/
static boolean recentcalls_HandleEvent(IRecentCalls *pi,
    AEEEvent eCode,
    uint16  wParam,
    uint32 dwParam
)
{
    CRecentCalls *pMe = (CRecentCalls*)pi;
    AEEAppStart* as = 0;
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

            pMe->m_bSuspending = FALSE;
            #if 0
            if (TRUE != ICALLLIST_InitEnd(pMe->m_pCallList))
            {
                return TRUE;   
            }
            #endif
            if( as->pszArgs != NULL && STRLEN( as->pszArgs) > 0)
            {
                pMe->m_charAppStartArgs = as->pszArgs[0];
                pMe->m_pSelFldCB =(PFNSELECTEDRECENTCALLSCB)ATOI(as->pszArgs + 1);
            }
            else
            {
                pMe->m_charAppStartArgs = 0;
            }

            // 开始RecentCalls状态机
            if(*as->pszArgs == STARTARG_OUTGCALL)
            {
               pMe->m_callsCategory = AEECALLHISTORY_CALL_TYPE_TO;//CALLHISTORY_OUTGOING_CATEGORY;
               //MOVE_TO_STATE(STATE_RECORD_LIST);
            }
            else if(*as->pszArgs == STARTARG_MISSEDCALL)
            {         
               pMe->m_callsCategory = AEECALLHISTORY_CALL_TYPE_MISSED;//CALLHISTORY_MISSED_CATEGORY;
               //MOVE_TO_STATE(STATE_RECORD_LIST);
            }
            // 开始recentcalls状态机
            IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE);
            recentcalls_RunFSM(pMe);
            return TRUE;

        case EVT_APP_STOP:
            if (pMe->m_pDisplay != NULL)
            {
                (void) IDISPLAY_Release(pMe->m_pDisplay);
                pMe->m_pDisplay = NULL;
            }
            pMe->m_bSuspending = TRUE;
            (void)ISHELL_CancelTimer(pMe->m_pShell, NULL, pMe);
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
            pMe->key_enable = TRUE; 
            if(pMe->m_eCurState == STATE_RECORDDEAL)
            {
                pMe->m_eCurState = STATE_RECORD_LIST;
            }
            recentcalls_RunFSM(pMe);
            return TRUE;

        case EVT_DIALOG_INIT:
            pMe->m_bAppIsReady = FALSE;
            pMe->m_pDialog = (IDialog*)dwParam;
            pMe->m_pActiveDlgID = wParam;

            return recentcalls_RouteDialogEvent(pMe,eCode,wParam,dwParam);

        case EVT_DIALOG_START:
            return recentcalls_RouteDialogEvent(pMe,eCode,wParam,dwParam);

        case EVT_USER_REDRAW:
            (void)recentcalls_RouteDialogEvent(pMe,eCode,wParam,dwParam);
            (void)ISHELL_SetTimer ( pMe->m_pShell,
                                    APPISREADY_TIMER,
                                    recentcalls_APPIsReadyTimer,
                                    pMe);
            return TRUE;
        
        case EVT_APPISREADY:
            (void) recentcalls_RouteDialogEvent(pMe,eCode,wParam,dwParam);
            pMe->m_bAppIsReady = TRUE;
            return TRUE;
  
        case EVT_KEY_PRESS:
        case EVT_KEY_RELEASE:
        case EVT_KEY:
        case EVT_COMMAND:
            if (!pMe->m_bAppIsReady)
            {
                return TRUE;
            }
            return recentcalls_RouteDialogEvent(pMe,eCode,wParam,dwParam);

        case EVT_DIALOG_END:
            if (wParam == 0)
            {
                return TRUE;
            }
            pMe->m_bAppIsReady = FALSE; 
            (void) recentcalls_RouteDialogEvent(pMe,eCode,wParam,dwParam);
            pMe->m_pDialog = NULL;

            // Applet被挂起，不跑状态机，等待Applet返回。（注意：EVT_APP_SUSPEND
            // 事件在EVT_DIALOG_END事件前发出。
            if (pMe->m_bSuspending == FALSE)
            {
                // 开始recentcalls状态机
                recentcalls_RunFSM(pMe);
            }
            return TRUE;
#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
					case EVT_PEN_UP:
					case EVT_PEN_DOWN:
						{
							return recentcalls_RouteDialogEvent(pMe,eCode,wParam,dwParam);
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
							return recentcalls_RouteDialogEvent(pMe,eCode,wParam,dwParam);
						}
#endif

        default:
            // 将接收到的事件路由至当前活动的对话框事件处理函数。
            return recentcalls_RouteDialogEvent(pMe,eCode,wParam,dwParam);
    }
}

/*==============================================================================
函数：recentcalls_APPIsReadyTimer

说明：
       定时器回调函数。主要用于控制快速按键问题的处理。

参数：
       pme [in]：void *类型指针

返回值：
        无。

备注：:
        建议每一个对话框处理函数在界面准备妥当时都向本Applet发送
        EVT_USER_REDRAW事件，在该事件处理完毕设置定时器，便
        于统一处理。
        
==============================================================================*/
static void recentcalls_APPIsReadyTimer(void *pme)
{
    CRecentCalls *pMe = (CRecentCalls *)pme;

    if (NULL == pMe)
    {
        return;
    }

    (void) ISHELL_PostEvent( pMe->m_pShell,
                             AEECLSID_APP_RECENTCALL,
                             EVT_APPISREADY,
                             0,
                             0);
}


/*==============================================================================
函数：RecentCalls_FreeSelectRecordListNode

说明：
       释放SelectRecordList

参数：

返回值：

备注：
       
==============================================================================*/
void RecentCalls_FreeSelectRecordListNode(CRecentCalls *pMe)
{
    extern sSelectRecordListNode* pSelectRecordListRoot;
    sSelectRecordListNode *pNode = pSelectRecordListRoot;
    
    while (pNode != NULL)
    {
        FREEIF(pNode->SelectRecord.number);
        FREEIF(pNode->SelectRecord.name);
        
        pSelectRecordListRoot = pNode->pNextNode;
        
        if(NULL != pNode)
        {
            FREE(pNode);
        }
        
        pNode = pSelectRecordListRoot;
    }
}
/*==================================================================================
FUNCTION 
        RecentCalls_GetRemainWMSNum
        
Description 
        获取短信发送列表剩余号码条数
        
PARAMETERS:
        *pMe: CRecentCalls object pointer
        
RETURN VALUE:
        uint16: 剩余号码条数
   
COMMENTS:
   
SIDE EFFECTS:

===================================================================================*/
uint16 RecentCalls_GetRemainWMSNum(CRecentCalls *pMe)
{
	int     nRemainNum = 0;
	
#ifdef FEATURE_WMS_APP
    IWmsApp *pWmsApp = NULL;    
    
    if (SUCCESS == ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_WMSAPP, (void**)&pWmsApp))
    {
        (void)IWmsApp_GetLeftSendListNodeNum(pWmsApp, &nRemainNum);
        IWmsApp_Release(pWmsApp);
    }
#endif	

	
    return (uint16)nRemainNum;
}


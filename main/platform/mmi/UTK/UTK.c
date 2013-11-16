/*==============================================================================
// 文件：
//        UTK.c
//        2007-11-13 陈喜玲草拟版本(Draft Version)
//        版权所有(c) 2004 Anylook
//        
// 文件说明：
//        
// 作者：2007-11-13
// 创建日期：陈喜玲
// 当前版本：Draft
==============================================================================*/

/*==============================================================================
修改历史记录：
时       间      修 改 人     修改内容、位置及原因
-----------      ----------     -----------------------------------------------
07-11-13         陈喜玲         初始版本
==============================================================================*/


/*==============================================================================
                                 本文件包含的外部文件
==============================================================================*/
#include "UTK_priv.h"
//#include "AEEPrompt.h"
#include "UTKUtils.h"
#include "OEMHeap.h"

/*==============================================================================
                                 宏定义和常数
==============================================================================*/
#define EVT_DISPLAYOUTAPPMSG (EVT_USER+20)

/*==============================================================================
                                 类型定义
==============================================================================*/

/*==============================================================================
                                 函数声明
==============================================================================*/
/*----------------------模块相关函数声明---------------------*/
/*----------------------Applet相关函数声明---------------------*/
int UTK_Load(IShell *ps, void * pHelpers, IModule ** pMod);
#if defined(AEE_STATIC)
static int UTK_CreateInstance(AEECLSID    ClsId,
                              IShell     *pIShell,
                              IModule    *po,
                              void      **ppObj);
#endif

static int UTK_New(IShell *ps, IModule *pIModule, void **ppObj);

static uint32  UTK_AddRef(IUTK *pi);

static uint32  UTK_Release (IUTK *pi);

static boolean UTK_HandleEvent(IUTK *pi,
                               AEEEvent eCode,
                               uint16  wParam,
                               uint32 dwParam);
static int UTK_InitAppData(CUTK *pMe);

static void UTK_FreeAppData(CUTK *pMe);

static void UTK_RunFSM(CUTK *pMe);

static void UTK_MAKE_UTK_CALL(CUTK *pMe);

/*==============================================================================
                                 本地（静态）数据
==============================================================================*/
#if defined(AEE_STATIC)  
#define UTK_METHODS           \
        {                     \
            UTK_AddRef,       \
            UTK_Release,      \
            UTK_HandleEvent   \
        }
        
// 只允许一个UTK实例。每次创建UTK Applet时，
// 返回同一结构指针给BREW层。
static CUTK gUTK;

static const VTBL(IUTK) gUTKMethods = UTK_METHODS;
#endif

/*----------------------模块相关函数定义---------------------*/


/*----------------------Applet相关函数定义---------------------*/
//===========================================================================
//
// DESCRIPTION
//		Standard function called by BREW to initialize the static module.
//
// Return Value:
//	   0 - Success, non-0 on failure
//	
//===========================================================================
#if defined(AEE_STATIC)
int UTK_Load(IShell *ps, void * pHelpers, IModule ** pMod)
{
   return(AEEStaticMod_New((int16)(sizeof(AEEMod)),ps,pHelpers,pMod,(PFNMODCREATEINST)UTK_CreateInstance,(PFNFREEMODDATA)NULL));
}
#endif


//===========================================================================
// 
// DESCRIPTION
//		This function is invoked while the app is being loaded. All Modules must provide this 
// 	function. Ensure to retain the same name and parameters for this function.
//
// PARAMETERS
// 	ClsId: [in]: Specifies the ClassID of the applet which is being loaded
// 	pIShell: [in]: Contains pointer to the IShell interface. 
// 	pIModule: [in]: Contains pointer to the IModule interface to the current module to which
// 	this app belongs
// 	ppObj: [out]: On return, *ppObj must point to a valid AEEApplet structure. Allocation
// 	of memory for this structure and initializing the base data members is done by AEEApplet_New().
//
// Return Value:
//		
//===========================================================================
#if defined(AEE_STATIC)
static int UTK_CreateInstance(AEECLSID    ClsId,
                              IShell     *pIShell,
                              IModule    *po,
                              void      **ppObj)
#else
int AEEClsCreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * po,void ** ppObj)
#endif
{
    if (ClsId == AEECLSID_APP_UTK)
    {
        return UTK_New(pIShell, po, ppObj);
    }
    else
    {
        return EUNSUPPORTED;
    }
}
/*==============================================================================
函数: 
       UTK_New
       
说明: 
        初始化UTK Applet虚拟表和内部数据变量，同时其引用计数加1。
       
参数: 
       ps [in]：IShell接口对象指针。
       pIModule [in]：pIModule接口对象指针。
       ppObj [out]:：指向IUTK对象指针的指针。
       
返回值:
       EFAILED：失败。
       SUCCESS：成功。
       
备注:
       不需用户去调用。
       关于Applet结构对象可使用动态分配内存的方式创建，本处采用的是使用静态
       变量的方式。建议对结构比较大的Applet使用静态变量的方式。对于动态分配
       内存的方式需要注意对所分配空间的释放。
       
==============================================================================*/
#include "err.h"
static int UTK_New(IShell *ps, IModule *pIModule, void **ppObj)
{
    CUTK     *pMe;
    int retVal = SUCCESS;
    
    if (!ps || !pIModule || !ppObj)
        return EFAILED;

#if defined(AEE_STATIC)
    pMe = &gUTK;    
#else
    pMe = (CUTK *)MALLOC(sizeof(CUTK) + sizeof(VTBL(IUTK)));
    if (!pMe)        
        return EFAILED;
#endif  
    ERR("miaoxiaoming:UTK_New start here",0,0,0);
    if (0 == pMe->m_nRefs)
    {
#if defined(AEE_STATIC)        
        // Must initialize the object
        MEMSET(pMe,  0, sizeof(CUTK));
        INIT_VTBL(pMe, IUTK, gUTKMethods);
#else
//NOTE: In the dynamic applet, the extend classes will re-malloc the memery.
//      So it run in simulator for test only. If you want run in simulator correcttly
//      Please modify the following code.
        VTBL(IUTK) gUTKMethods;
        VTBL(IUTK) *pMod;
        gUTKMethods.AddRef      =  UTK_AddRef;
        gUTKMethods.Release     =  UTK_Release;
        gUTKMethods.HandleEvent =  UTK_HandleEvent;
        
        pMod = (VTBL(IUTK) *)((byte *)pMe + sizeof(CUTK));
        MEMCPY((byte *)pMod, (byte *)&gUTKMethods, sizeof(VTBL(IUTK)));
        INIT_VTBL(pMe, IUTK, *pMod);
#endif  
        pMe->m_nRefs     = 0;
        pMe->m_pShell    = ps;
        pMe->m_pModule   = pIModule;

        (void) ISHELL_AddRef(ps);
        (void) IMODULE_AddRef(pIModule);

        retVal = UTK_InitAppData(pMe);
        
        if ( SUCCESS != retVal )
        {
        	UTK_FreeAppData(pMe);
        	return retVal;
        }
    }

    ++pMe->m_nRefs;
    *ppObj = (IUTK*)pMe;
    return retVal;
}

/*==============================================================================
函数: 
       UTK_AddRef
       
说明: 
       UTK Applet 引用计数加1，并返回当前的引用数。
       
参数: 
       pi [in]：IUTK接口对象指针。
       
返回值:
       当前对UTK Applet的引用数
       
备注:
       
==============================================================================*/
static uint32  UTK_AddRef(IUTK *pi)
{
    register CUTK *pMe = (CUTK*)pi;

    ASSERT(pMe != NULL);
    ASSERT(pMe->m_nRefs > 0);

    return (++pMe->m_nRefs);
}

/*==============================================================================
函数: 
       UTK_Release
       
说明: 
       释放对UTK Applet的引用，引用计数减1，并返回当前的引用数。
        如果当前的引用数为零，还将释放Applet及相关资源。
       
参数: 
       pi [in]：IUTK接口对象指针。
       
返回值:
       当前对UTK Applet的引用数
       
备注:
       
==============================================================================*/
static uint32  UTK_Release (IUTK *pi)
{
    register CUTK *pMe = (CUTK*)pi;

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
    UTK_FreeAppData(pMe);

    (void) ISHELL_Release(pMe->m_pShell);
    (void) IMODULE_Release(pMe->m_pModule);

    // 注意：pMe是静态分配空间，因此不需要释放。FREE()
    return 0;
}

/*==============================================================================
函数: 
       UTK_InitAppData
       
说明: 
       初始化UTK Applet结构数据缺省值。
       
参数: 
       pMe [in]：指向UTK Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值:
       SUCCESS：初始化成功。
       EFAILED：初始化失败。
       
备注:
       
==============================================================================*/
static int UTK_InitAppData(CUTK *pMe)
{
    AEEDeviceInfo  di;
    DBGPRINTF("UTK_InitAppData");
    if (NULL  == pMe)
    {
        return EFAILED;
    }
    ISHELL_GetDeviceInfo(pMe->m_pShell, &di);
    pMe->m_rc.x       = 0;
    pMe->m_rc.y       = 0;
    pMe->m_rc.dx      = (int16) di.cxScreen;
    pMe->m_rc.dy      = (int16) di.cyScreen;
    pMe->m_ePreState = UTKST_NONE;
    pMe->m_eCurState = UTKST_INIT;
    pMe->m_pActiveDlg = NULL;
    pMe->m_eDlgRet = DLGRET_CREATE;
    pMe->m_bNotOverwriteDlgRet = FALSE;
    pMe->cmd_type = (uim_tk_proactive_cmd_enum_type)UIM_TK_SETUP_MENU;
    pMe->m_wSelectStore        = MENU_SELECT_NULL;    
    
    //因为挂起可以在任何时候发生，所以默认需要挂起备份 
    pMe->InputeBackupNeeded = TRUE;
    MEMSET(pMe->bInputeBackup, 0, (MAX_STR_ACHARS+2) * sizeof(AECHAR));
    
    pMe->m_wszInputTitle = NULL;
    pMe->m_wszInputText = NULL;
    pMe->m_TextMaxLen = 0;
    pMe->m_TextMinLen = 0;  
    pMe->m_PlaytoneMsgID = 0;   
    pMe->m_PlaytoneToneID = 0;     
    
    // 创建需要的接口
    if (ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_ALERT,
            (void **)&pMe->m_pAlert) != SUCCESS)
    {
        ERR("Failed to create AEECLSID_ALERT",0,0,0);
        return EFAILED;
    }
    
    if (ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_CONFIG,
            (void **)&pMe->m_pConfig) != SUCCESS)
    {
        ERR("Failed to create AEECLSID_CONFIG",0,0,0);
        return EFAILED;
    }
#ifdef FEATURE_ICM
    if (ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_CALLMANAGER,
            (void **)&pMe->m_pCM) != SUCCESS)
#else
    
    if (ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_TELEPHONE ,
            (void **)&pMe->m_pITelephone) != SUCCESS)
#endif
    {
        ERR("Failed to create AEECLSID_CALLMANAGER",0,0,0);
        return EFAILED;
    }
     
    if (ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_WMS,
            (void **)&pMe->m_pwms) != SUCCESS)
    {
        ERR("Failed to create AEECLSID_PHONE",0,0,0);
        return EFAILED;
    } 
    
    CALLBACK_Init(&pMe->m_callback, 
                  (PFNNOTIFY) UTK_wmscommandreport,
                  (void*)pMe);
    (void)IWMS_Init(pMe->m_pwms, WMS_CLIENT_TYPE_CAT, &pMe->m_clientId); 
    (void)IWMS_RegisterMsgCb(pMe->m_pwms, pMe->m_clientId, UTK_MsgCb);
    (void)IWMS_Activate(pMe->m_pwms,pMe->m_clientId);
#ifdef FEATURE_FUNCS_THEME   
    {
        // 获取主题参数
        Theme_Param_type    Theme_Param;
        Appscom_GetThemeParameters(&Theme_Param);
        pMe->m_themeColor = Theme_Param.themeColor;        
    }
#endif /* FEATURE_FUNCS_THEME */    
	if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,AEECLSID_ANNUNCIATOR,(void **)&pMe->m_pIAnn))
    {
        ERR("Failed to create AEECLSID_ANNUNCIATOR",0,0,0);
        return EFAILED;
    }
    DBGPRINTF("UTK_InitAppData");
    return SUCCESS;
}

/*==============================================================================
函数: 
       UTK_FreeAppData
       
说明: 
       释放UTK Applet使用的相关资源。
       
参数: 
       pMe [in]：指向UTK Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值:
       无。
       
备注:
       
==============================================================================*/
static void UTK_FreeAppData(CUTK *pMe)
{
    if (NULL == pMe)
    {
        return ;
    }
    
    if (NULL != pMe->m_wszInputTitle)
    {
        FREE(pMe->m_wszInputTitle);
        pMe->m_wszInputTitle = NULL;
    }
    
    if (NULL != pMe->m_pAlert)
    {
        IALERT_Release(pMe->m_pAlert);
        pMe->m_pAlert = NULL;
    }
    
    if (NULL != pMe->m_pConfig)
    {
        ICONFIG_Release(pMe->m_pConfig);
        pMe->m_pConfig = NULL;
    }
#ifdef FEATURE_ICM
    if (NULL != pMe->m_pCM)
    {
        ICM_Release(pMe->m_pCM);
        pMe->m_pCM = NULL;
    }
#else
    if (NULL != pMe->m_pITelephone)
    {
        ITELEPHONE_Release(pMe->m_pITelephone);
        pMe->m_pITelephone = NULL;
    }
#endif
    if (NULL != pMe->m_wszInputText)
    {
        FREE(pMe->m_wszInputText);
        pMe->m_wszInputText = NULL;
    }   

    if (NULL != pMe->m_pwms)
    {
        IWMS_Deactivate(pMe->m_pwms,pMe->m_clientId);
        IWMS_Release(pMe->m_pwms);
        pMe->m_pwms = NULL;
    }
	if(pMe->m_pIAnn)
    {
        IANNUNCIATOR_Release(pMe->m_pIAnn);
        pMe->m_pIAnn = NULL;
    }
    return;
}

/*==============================================================================
函数: 
       UTK_RunFSM
       
说明: 
       UTK Applet有限状态机引擎。
       
参数: 
       pMe [in]：指向UTK Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值:
       无。
       
备注:
       
==============================================================================*/
static void UTK_RunFSM(CUTK *pMe)
{
    NextFSMAction nextFSMAction = NFSMACTION_WAIT;

    for ( ; ; )
    {
        nextFSMAction = UTK_ProcessState(pMe);

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
       UTK_HandleEvent
       
说明: 
       UTK  Applet事件处理主函数。所有属于本Applet的事件，
       都将经由该函数处理。
       
参数: 
       pi [in]：指向IUTK接口t对象的指针。
       eCode：BREW事件代码。
       wParam：与事件相关联的数据。
       dwParam [in]：与事件相关联的数据。
       
返回值:
       TRUE：传入事件被处理。
       FALSE：传入事件没被处理。
       
备注:
       
==============================================================================*/
static boolean UTK_HandleEvent(IUTK *pi,
    AEEEvent eCode,
    uint16  wParam,
    uint32 dwParam
)
{
    CUTK *pMe = (CUTK*)pi;
    AEEAppStart *as;
    UTKState  eTempState = UTKST_NONE;
    uim_tk_proactive_cmd_enum_type   old_cmd_type;
    
    // 开机时初始化UIM UTK 功能时会收到 UIM_TK_SETUP_MENU ，此时忽略该事件
    //static boolean bIsInitUTK = TRUE;
    //ERR("UTK CODE=%x,wParam = %x",eCode,wParam,0);
    DBGPRINTF("UTK_HandleEvent 0x%x 0x%x 0x%x Start",eCode,wParam,dwParam);
    switch (eCode)
    {
        case EVT_APP_START:
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
            
            if(pMe->m_pIAnn != NULL)
            {
                IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE); 
            }
            
            pMe->m_bSuspending = FALSE; 
            // 开始UTK状态机
            UTK_RunFSM(pMe);        
            return TRUE;

        case EVT_APP_STOP:
            if (pMe->m_pDisplay != NULL)
            {
                // 恢复背景色值
                (void)IDISPLAY_SetColor(pMe->m_pDisplay, 
                    CLR_USER_BACKGROUND, pMe->m_dwOldBkColor);

                (void) IDISPLAY_Release(pMe->m_pDisplay);
                pMe->m_pDisplay = NULL;
            }             
            OEMSOUND_Sound_Stop(SND_PRIO_MED);
            IALERT_StopRingerAlert(pMe->m_pAlert);
            IALERT_StopSMSAlert(pMe->m_pAlert); 
            IALERT_StopMp3Alert(pMe->m_pAlert);
            pMe->m_bSuspending = TRUE;
            return TRUE;

        case EVT_APP_SUSPEND:
            //挂起时不关闭UTK
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
            {
                AEEDeviceInfo di; 
                
                ISHELL_GetDeviceInfo(pMe->m_pShell,&di);
                pMe->m_rc.dy = di.cyScreen;
            }
            MSG_FATAL("start EVT_APP_RESUME-UTK_RunFSM",0,0,0);   
            UTK_RunFSM(pMe);
            MSG_FATAL("end EVT_APP_RESUME-UTK_RunFSM",0,0,0);             
            return TRUE;

        case EVT_DIALOG_INIT:
            if (wParam == OEM_IME_DIALOG)
            {
                return TRUE;
            }
            pMe->m_bAppIsReady = FALSE;
            pMe->m_dwOldBkColor = IDISPLAY_SetColor(pMe->m_pDisplay ,
                    CLR_USER_BACKGROUND, RGB_WHITE);
            pMe->m_pActiveDlg = (IDialog*)dwParam;
            pMe->m_pActiveDlgID = wParam;
            MSG_FATAL("EVT_DIALOG_INIT 0x%x %d",pMe->m_pActiveDlg, pMe->m_pActiveDlgID, 0);
            return UTK_RouteDialogEvent(pMe,eCode,wParam,dwParam);

        case EVT_DIALOG_START:
            if (wParam == OEM_IME_DIALOG)
            {
                return TRUE;
            }
            (void) ISHELL_SetTimer(pMe->m_pShell,
                     APPISREADY_TIMER,
                     UTK_APPIsReadyTimer,
                     pMe);
            return UTK_RouteDialogEvent(pMe,eCode,wParam,dwParam);

        case EVT_USER_REDRAW:
            (void) UTK_RouteDialogEvent(pMe,eCode,wParam,dwParam);
            /*
            (void) ISHELL_SetTimer(pMe->m_pShell,
                     APPISREADY_TIMER,
                     UTK_APPIsReadyTimer,
                     pMe);
            */
            return TRUE;

        case EVT_APPISREADY:
            pMe->m_bAppIsReady = TRUE;
            return TRUE;
            
        case EVT_SEND_STATUS:
            return UTK_RouteDialogEvent(pMe,eCode,wParam,dwParam);

        case EVT_KEY_PRESS:
        case EVT_KEY_RELEASE:
        case EVT_KEY:
        case EVT_COMMAND: 
			#if !defined(FEATURE_VERSION_K212)&&!defined(FEATURE_QVGA_INHERIT_K212)&& !defined(FEATURE_VERSION_K212_HUALU)
            if (pMe->m_bAppIsReady == FALSE)
            {
                return TRUE;
            }
			#endif
            return UTK_RouteDialogEvent(pMe,eCode,wParam,dwParam);

        case EVT_DIALOG_END:
            if ((wParam == OEM_IME_DIALOG) &&
                (pMe->m_pActiveDlg != (IDialog *)dwParam))
            {
                (void) ISHELL_PostEvent(pMe->m_pShell,
                                        AEECLSID_APP_UTK,
                                        EVT_USER_REDRAW,
                                        0,
                                        0);
                return TRUE;
            }
            if (wParam == 0)
            {
                return TRUE;
            }
            (void) UTK_RouteDialogEvent(pMe,eCode,wParam,dwParam);
            MSG_FATAL("EVT_DIALOG_END 0x%x %d",pMe->m_pActiveDlg, pMe->m_pActiveDlgID, 0);
            pMe->m_pActiveDlg = NULL; 
            pMe->m_bAppIsReady = FALSE;

            // Applet被挂起，不跑状态机，等待Applet返回。（注意：EVT_APP_SUSPEND
            // 事件在EVT_DIALOG_END事件前发出。
            if (pMe->m_bSuspending == FALSE)
            {
                // 开始UTK状态机
                UTK_RunFSM(pMe);
            }
            return TRUE;
#ifdef FEATURE_LCD_TOUCH_ENABLE
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
                else if (wParam == AVK_POUND)
                {
                    eCode = EVT_KEY_PRESS;
                }
                return UTK_RouteDialogEvent(pMe,eCode,wParam,dwParam);
            }
#endif

        case EVT_BACKUTK:
            (void)ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APP_UTK);
            return TRUE;

        case EVT_RUIM_PROACTIVE:
            old_cmd_type = pMe->cmd_type;
            pMe->cmd_type = (uim_tk_proactive_cmd_enum_type)wParam;
            DBGPRINTF("EVT_RUIM_PROACTIVE cmd %d",pMe->cmd_type);
            switch (pMe->cmd_type)
            {
                case UIM_TK_REFRESH:
                    // Refresh 命令是卡上内容发生了变化，要求终端与之同步。对于终端没有缓存卡上
                    // 内容的部分可不予处理，因每次读取时都从卡上读；对于终端缓存了的卡上部分内
                    // 容，则终端在收到此命令后，应重新从卡上读取保持与卡同步。
                    {
                        RefreshCmdType *pCmd=NULL;
                        byte *pdata;
                        uim_tk_general_result_type eResult = UIM_TK_CMD_PERFORMED_SUCCESSFULLY;
                        uint16   wMask = 0;
                        
                        pdata = UTK_GetCmddata(pMe->cmd_type);
                        pCmd = (RefreshCmdType *)MALLOC(sizeof(RefreshCmdType));
                        if ((NULL == pdata) || (NULL == pCmd))
                        {
                            UTK_GiveResponse(pMe, pMe->cmd_type, FALSE, UIM_TK_COMMAND_BEYOND_TERMINALS_CAPABILITIES);
                            return TRUE;
                        }
                        
                        DecodeRefreshData(pdata, pCmd);
                        
                        switch(pCmd->cmd_qualifier)
                        {
                            // 初始化及全文件变更
                            case UIM_TK_NAA_INITIALIZATION_AND_FULL_FILE_CHANGE_NOTIFICATION:
                                
                            // 初始化及文件变更
                            case UIM_TK_NAA_INITIALISATION_AND_FILE_CHANGE_NOTIFICATION:
                                
                            // SIM卡初始化    
                            case UIM_TK_NAA_INITIALISATION:
                                wMask = REFRESHMASK_UIMALL;
                                break;
                                
                            // 文件变更
                            case UIM_TK_FILE_CHANGE_NOTIFICATION:
                                // 分析变更了的文件项，目前只处理短信和卡电话本
                                if ((pCmd->fileList.numFiles>0) &&
                                    (pCmd->fileList.numBytes>=4))
                                {
                                    int i,j,k,pos=2;
                                    boolean bValid=FALSE;
                                    
                                    for (i=0; i<pCmd->fileList.numFiles; i++)
                                    {
                                        if (i==pCmd->fileList.numFiles-1)
                                        {// 最后一项
                                            j=pCmd->fileList.numBytes-2;
                                            k=pCmd->fileList.numBytes-1;
                                            
                                            if (pCmd->fileList.fileData[j]==0x6F &&
                                                pCmd->fileList.fileData[k]==0x3C)
                                            {// 短信
                                                wMask |= REFRESHMASK_UIMSMS;
                                            }
                                            else if (pCmd->fileList.fileData[j]==0x6F &&
                                                pCmd->fileList.fileData[k]==0x3A)
                                            {// 卡电话本
                                                wMask |= REFRESHMASK_UIMADD;
                                            }
                                        }
                                        else
                                        {
                                            bValid=FALSE;
                                            for(;pos<pCmd->fileList.numFiles;)
                                            {
                                                if (pCmd->fileList.fileData[pos]==0x3F &&
                                                    pCmd->fileList.fileData[pos+1]==0x00)
                                                {
                                                    bValid=TRUE;
                                                    break;
                                                }
                                                pos++;
                                            }
                                            if (bValid)
                                            {
                                                j=pos-2;
                                                k=pos-1;
                                                
                                                // 跳过文件项起始标记，便于分析下一文件项
                                                pos+=2;
                                                
                                                if (pCmd->fileList.fileData[j]==0x6F &&
                                                    pCmd->fileList.fileData[k]==0x3C)
                                                {// 短信
                                                    wMask |= REFRESHMASK_UIMSMS;
                                                }
                                                else if (pCmd->fileList.fileData[j]==0x6F &&
                                                    pCmd->fileList.fileData[k]==0x3A)
                                                {// 卡电话本
                                                    wMask |= REFRESHMASK_UIMADD;
                                                }
                                            }
                                            else
                                            {
                                                break;
                                            }
                                        }
                                    }
                                }
                                break;
                                
                            // SIM卡复位    
                            case UIM_TK_NAA_RESET:
                                wMask = REFRESHMASK_UIMRESET;
                                break;
                                
                            default:
                                eResult = UIM_TK_COMMAND_BEYOND_TERMINALS_CAPABILITIES;
                                break;
                                
                        }
                        
                        FREEIF(pCmd);
                        UTK_GiveResponse(pMe, pMe->cmd_type, FALSE, eResult);
                        if (wMask != 0)
                        {// 更新工作放在 Ilde 去进行
                            (void) ISHELL_PostEvent( pMe->m_pShell,
                                                     AEECLSID_CORE_APP,
                                                     EVT_UTKREFRESH,
                                                     wMask,
                                                     0);
                            
                            // 必须关闭全部 Applet, 以便于释放对卡电话本接口                         
                            (void)ISHELL_CloseApplet(pMe->m_pShell, TRUE);
                        }
                    }
                    return TRUE;

                case UIM_TK_PLAY_TONE:                
                    eTempState = UTKST_PLAYTONE;                     
                    break;
                    
                case UIM_TK_SETUP_CALL:
                    {
                        Setup_Call  *pstCall;
                        byte        *pdata;
#ifndef FEATURE_ICM
						AEETCalls po;
#endif

                        pstCall = UTK_Get_Setup_Call_Buf();
                        pdata = UTK_GetCmddata(pMe->cmd_type);
                        
                        if (NULL == pstCall || NULL == pdata)
                        {
                            UTK_GiveResponse(pMe, pMe->cmd_type, FALSE, UIM_TK_CMD_PERFORMED_SUCCESSFULLY);
                            break;
                        }
                        
                        MEMSET(pstCall, 0, sizeof(Setup_Call));
                        (void)UTK_DecodeSetupCallCmdData(pdata, pstCall);
                        DBGPRINTF("UIM_TK_SETUP_CALL %S",pstCall->wszDialNum);
#ifdef FEATURE_ICM
                        if ((AEECM_IS_VOICECALL_PRESENT(pMe->m_pCM))  &&
#else
						if(SUCCESS != ITELEPHONE_GetCalls(pMe->m_pITelephone, &po,sizeof(AEETCalls)))
						{
							break;
						}
                        if ((po.dwCount>0)&&
#endif
                            ((pstCall->cmd_describe.command_restricttag == 0x00) ||
                             (pstCall->cmd_describe.command_restricttag == 0x01)) &&
                            (WSTRLEN(pstCall->wszAlpha) == 0))
                        {
                            pMe->m_eMsgRetState = UTKST_MAIN;
                            pMe->m_wMsgID = IDS_CALLINFO;     
                            pMe->m_MsgTimeout = TIMEOUT_MS_MSGBOX;                           	
                            eTempState = UTKST_MSG;
                        }
                        else if(WSTRLEN(pstCall->wszDialNum)>0)
                        {
                            UTK_MAKE_UTK_CALL(pMe);
                        }
                        UTK_GiveResponse(pMe, pMe->cmd_type, FALSE, UIM_TK_CMD_PERFORMED_SUCCESSFULLY);
                    }
                    break;
                
                case UIM_TK_DISPLAY_TEXT:
                {
                    Display_text *ptext = (Display_text *)MALLOC(sizeof(Display_text));
                    eTempState = UTKST_NONE;//UTKST_DISPLAY;
                    if (NULL != ptext)
                    {
                        byte *utk_ptr = UTK_GetCmddata(pMe->cmd_type);;
                        
                        DecodeDisplayTextData(utk_ptr, ptext); 
                        DBGPRINTF("UIM_TK_DISPLAY_TEXT %d",ptext->text.length);
                        if(ptext->text.length == 0)
                        {
                            UTK_GiveResponse(pMe, pMe->cmd_type, FALSE, UIM_TK_COMMAND_DATA_NOT_UNDERSTOOD_BY_TERMINAL);
                        }
                        else
                        {
                            if (ptext->cmd_describe.command_restricttag & 0x01)
                            {
                                // 高优先级，什么时候都显示
                                eTempState = UTKST_DISPLAY;
                            }
                            else
                            {
                                // 正常优先级，判断是否在IDLE和UTK界面
                                if (ISHELL_ActiveApplet(pMe->m_pShell) == AEECLSID_APP_UTK
                                    ||ISHELL_ActiveApplet(pMe->m_pShell) == AEECLSID_CORE_APP)
                                {
                                    eTempState = UTKST_DISPLAY;
                                }
                                else
                                {
                                    UTK_GiveResponseEx(pMe, pMe->cmd_type, FALSE, UIM_TK_TERMINAL_CURRENTLY_UNABLE_TO_PROCESS_COMMAND,UIM_TK_SCREEN_IS_BUSY);
                                }
                            }
                        }
                        
                        // 释放临时分配空间
                        FREEIF(ptext);
                    }
                    break;
                }
                case UIM_TK_GET_INPUT:
                    eTempState = UTKST_INPUT;                                 
                    //如果是连续的两个文本框输入，不备份文本框，防止第二个文本框把第一个文本框中的内容贴出来
                    if(pMe->m_pActiveDlgID == IDD_INPUT_DIALOG)
                    {
                       pMe->InputeBackupNeeded = FALSE; 
                    }                     
                    break;
                
                case UIM_TK_SELECT_ITEM:
                    eTempState = UTKST_LIST;
                    break;
                    
                case UIM_TK_END_PROACTIVE_SESSION:
                    if (ISHELL_ActiveApplet(pMe->m_pShell) == AEECLSID_APP_UTK)
                    {
                        if(pMe->m_ePreState == UTKST_INIT || pMe->m_ePreState == UTKST_NONE)
                        {
                            (void)ISHELL_CloseApplet(pMe->m_pShell, FALSE);
                            break;
                        }
                    }
                    //else fall through
                case UIM_TK_SETUP_MENU:                  
                    pMe->cmd_type = UIM_TK_SETUP_MENU;
                    if (ISHELL_ActiveApplet(pMe->m_pShell) != AEECLSID_APP_UTK)
                    {
                        // 只需更新当前状态，不必启动 Applet
                        pMe->m_eCurState = UTKST_MAIN;
                        return TRUE;
                    }
                    else
                    {
                        eTempState = UTKST_MAIN;
                    }
                    break;
                
                case UIM_TK_CDMA_SEND_SHORT_MSG:
                {
#ifdef FEATURE_OEMOMH
                    AECHAR   wszInfo[64]={0};
                    wms_client_message_s_type * pCltMsg = NULL;

                    if(pMe->m_bSendingSMSBG)
                    {
                        UTK_GiveResponse(pMe, pMe->cmd_type, FALSE, UIM_TK_TERMINAL_CURRENTLY_UNABLE_TO_PROCESS_COMMAND);
                        return TRUE;
                    }
                    
                    UTK_ProcessSendCDMASMSCmd(pMe, &pCltMsg, wszInfo, 64);
                    if(WSTRLEN(wszInfo) == 0)
                    {
                        int nRet;
                        MSG_FATAL("UTK: IWMS_MsgSend %d", pMe->m_clientId, 0, 0);
                        nRet = IWMS_MsgSend(pMe->m_pwms, 
                                            pMe->m_clientId, 
                                            &pMe->m_callback,
                                            (void*)pMe,
                                            WMS_SEND_MODE_CLIENT_MESSAGE,
                                            pCltMsg);
                                            
                        if (nRet != SUCCESS)
                        {
                            UTK_GiveResponse(pMe, pMe->cmd_type, FALSE, UIM_TK_NETWORK_CURRENTLY_UNABLE_TO_PROCESS_COMMAND);
                        }
                        else
                        {
                            UTK_AddRef(pi);
                            pMe->m_bSendingSMSBG = TRUE;
                        }
                    }
                    else
#else
                    if (ISHELL_ActiveApplet(pMe->m_pShell) != AEECLSID_APP_UTK)
                    {
                        UTK_GiveResponse(pMe, pMe->cmd_type, FALSE, UIM_TK_NETWORK_CURRENTLY_UNABLE_TO_PROCESS_COMMAND);
                        return TRUE;
                    }
                    else
#endif
                    eTempState = UTKST_SENDMSG;
                    break;
                }
                
                case UIM_TK_PROVIDE_LOCAL_INFO: 
                    UTK_GiveResponse(pMe, pMe->cmd_type, TRUE, UIM_TK_CMD_PERFORMED_SUCCESSFULLY);  
                    break;
                    
                default:
                    break;
            }

            if (eTempState == UTKST_NONE)
            {
                return TRUE;
            }
            
            // 更新当前状态
            MOVE_TO_STATE(eTempState);

            DBGPRINTF("EVT_RUIM_PROACTIVE %d",pMe->m_eCurState);
            // 若当前活动Applet不是本Applet，则忽略该事件，直接返回TRUE
            if (ISHELL_ActiveApplet(pMe->m_pShell) != AEECLSID_APP_UTK)
            {
                if((old_cmd_type == UIM_TK_SETUP_CALL) && (pMe->m_bSuspending == TRUE))
                {               
                    return TRUE;
                }
                else
                {
                    (void)ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APP_UTK);
                }
                return TRUE;
            }
            else
            {
                // Applet 已经启动运行。状态已经更新，仅仅关闭对话框。在
                // 执行状态机引擎 时会跑至新的状态。
                CLOSE_DIALOG(DLGRET_CREATE);
            }
            return TRUE;
            
        case EVT_WMS_CMD_STATUS:
#ifdef FEATURE_OEMOMH
            if(pMe->m_bSendingSMSBG)
            {
                wmsutk_cmd_status_type  *pStatus = (wmsutk_cmd_status_type *)dwParam;
                
                if (NULL == pStatus)
                {
                    return TRUE;
                }
                
                MSG_FATAL("UTK: EVT_WMS_CMD_STATUS %d %d", pStatus->cmd, pStatus->cmd_err, 0);
                switch (pStatus->cmd)
                {
                    case WMS_CMD_MSG_SEND:
                        if (WMS_CMD_ERR_NONE != pStatus->cmd_err)
                        {// 发送消息失败
                            UTK_GiveResponse(pMe, pMe->cmd_type, FALSE, UIM_TK_NETWORK_CURRENTLY_UNABLE_TO_PROCESS_COMMAND);
                            pMe->m_bSendingSMSBG = FALSE;
                            UTK_Release(pi);
                        }
                        break;
                    
                    default:
                        break;
                }
            }
            else
#endif
            (void)UTK_RouteDialogEvent(pMe,eCode,wParam,dwParam);
            FREEIF(dwParam);

            return TRUE;
            
        case EVT_WMS_MSG_SUBMIT_REPORT:
        case EVT_WMS_MSG_SEND:
            if (((wms_msg_event_info_s_type *)dwParam)->submit_report_info.client_id == WMS_CLIENT_TYPE_CAT)
            {
                UTK_ProcessStatus(pMe, &((wms_msg_event_info_s_type *)dwParam)->submit_report_info);
            }
            sys_free((wms_msg_event_info_s_type*)dwParam);
            return TRUE;
            
        default:
            // 将接收到的事件路由至当前活动的对话框事件处理函数。
            return UTK_RouteDialogEvent(pMe,eCode,wParam,dwParam);
    }
}

/*==============================================================================
函数: 
       ByteToUnicode
       
说明: 
       本函数将字节数据转换为 Unicode 数据。
       
参数: 
       pUnicodeBuf [in]：IMenuCtl 接口指针。
       pBytBuf [out]： 菜单标题
       cmd_type： uim_tk_proactive_cmd_enum_type
       
返回值:
       无。
       
备注:
       
==============================================================================*/
void ByteToUnicode(AECHAR *pUnicodeBuf, 
    byte *pBytBuf, 
    byte bytLen, 
    int mode
)
{
    byte i, high, low;
    boolean bUincode;

    bUincode = FALSE;
    if ((mode && pBytBuf[0] == UNICODE_FLAG) ||
        (!mode && pBytBuf[0] == ALPHA_UNICODE))
    {
        bUincode = TRUE;
    }
    
    if (!mode || pBytBuf[0] == UNICODE_FLAG)
    {
        pBytBuf++;
        bytLen--;
    }

    for (i=0; i<bytLen; )
    {
        if (bUincode)
        {
            high = pBytBuf[i++];
        }
        else
        {
            high = 0;
        }

        if (i < bytLen)
        {
            low = pBytBuf[i++];
        }
        else
        {
            break;
        }

        *pUnicodeBuf++ = (high<<8) | low; //lint !e539
    }
    *pUnicodeBuf = 0;
}

/*==============================================================================
函数：
       UTK_MsgSendStatusCallBack
说明：
       传给BREW的回调函数。由BREW调用用来通知Applet短消息发送状态。

参数：
       pUser {in]：void类型指针(此处使用CUTK 结构指针)
       status [in]：消息发送返回的结果码。
       
返回值：
       无。

备注：:
       以比BREW更高的优先级在CM环境运行。用以反馈正确发送消息结果给用户。

==============================================================================*/
void UTK_MsgSendStatusCallBack(void * pUser, int status)
{
    CUTK *pMe = (CUTK *) pUser;
    
    if (pMe == NULL)
    {
        return;
    }
    
    if (ISHELL_ActiveApplet(pMe->m_pShell) != AEECLSID_APP_UTK)
    {
        return;
    }
    
    (void) ISHELL_PostEvent(pMe->m_pShell,
                AEECLSID_APP_UTK,
                EVT_SEND_STATUS,
                0,
                (uint32)status);
}

/*==============================================================================
函数：
       UTK_MessageAlert

说明：
       用来播放发短消息成功提示音。

参数：
       pMe [in]：指向UTK Applet对象结构的指针。该结构包含小程序的特定信息。
       bPlay [in]: 播放指示，TRUE: 播放； 否则，停止播放。
       
返回值：
       无。

备注：:
       
==============================================================================*/
void UTK_MessageAlert(CUTK * pMe, boolean  bPlay)
{
    byte bSmsAlertType = 0;
    uint16 ringerID = 78;
    
    if ( NULL == pMe)
    {
        return;
    }
    
    if (!bPlay)
    {
        IALERT_StopSMSAlert(pMe->m_pAlert);
        IALERT_StopMp3Alert(pMe->m_pAlert);
        return;
    }
    (void) ICONFIG_GetItem(pMe->m_pConfig,
                          CFGI_SMS_RINGER,
                          &bSmsAlertType,
                          sizeof(bSmsAlertType));

    (void) ICONFIG_GetItem( pMe->m_pConfig,
                           CFGI_SMS_RINGER_ID,
                           &ringerID,
                           sizeof(ringerID));

    if (bSmsAlertType)
    {
#ifdef FEATURE_ICM
        if (AEECM_IS_VOICECALL_CONNECTED(pMe->m_pCM))
#else
		AEETCalls po;
		
		if(SUCCESS != ITELEPHONE_GetCalls(pMe->m_pITelephone, &po,sizeof(AEETCalls)))
		{
			return;
		}
		
        if (po.dwCount>0)
#endif
        {
            (void)ISHELL_Beep(pMe->m_pShell,BEEP_MSG,TRUE);
            return;
        }    
        IALERT_StartSMSAlert(pMe->m_pAlert, ringerID);      
    }
}

/*==============================================================================
函数: 
       UTK_APPIsReadyTimer
       
说明: 
       定时器回调函数。主要用于控制用户操作速度。
       
参数: 
       pme [in]：void *类型指针，暗指CUTK结构指针。
       
返回值:
       无。
       
备注:
       
==============================================================================*/
void UTK_APPIsReadyTimer(void *pme)
{
    CUTK *pMe = (CUTK *)pme;

    if (NULL == pMe)
    {
        return;
    }

    (void) ISHELL_PostEvent( pMe->m_pShell,
                            AEECLSID_APP_UTK,
                            EVT_APPISREADY,
                            0,
                            0);
}

/*===========================================================================
FUNCTION HexToDialStr

DESCRIPTION
  translate the hex code to Dial String,Create by Azlun.Xie 20060604
    
DEPENDENCIES
  None

RETURN VALUE
  finish digit count

SIDE EFFECTS
  None
===========================================================================*/
int HexToDialStr(byte *src,char *dst,int num_dig)
{
 char low_digit=0x00;
 char hig_digit=0x00;
 int finish_count=0;

 if(!src||!dst||num_dig<=0)
 {
    return 0;
    ERR("HexToDialStr:!src||!dst||num_dig<=0:(%X,%X,%d)",src,dst,num_dig);
 }

 do
 {
    low_digit= (char) ( *src     &0x0F)+'0';
    hig_digit= (char)(((*src)>>4)&0x0F)+'0';
    ERR("HexToDialStr:Src:0x%02X: Str:%c%c",*src,low_digit,hig_digit);

    //step forward
    src++;

    if('0'<=low_digit&&low_digit<='9')
    {
        *(dst++) =low_digit;
        finish_count++ ;
    }
    else if('0'+0x0A==low_digit)
    {
        *(dst++) ='*';
        finish_count++ ;
    }
    else if('0'+0x0B==low_digit)
    {
        *(dst++) ='#';
        finish_count++ ;
    }
    else if('0'+0x0C==low_digit)
    {
        *(dst++) ='+';
        finish_count++ ;
    }
    else
    {
        ERR("HexToDialStr Src End with 0x%2X",low_digit,0,0);
        break;
    } 
    if('0'<=hig_digit&&hig_digit<='9')
    {
        *(dst++) =hig_digit;
        finish_count++ ;
    }
    else if('0'+0x0A==hig_digit)
    {
        *(dst++) ='*';
        finish_count++ ;
    }
    else if('0'+0x0B==hig_digit)
    {
        *(dst++) ='#';
        finish_count++ ;
    }
    else if('0'+0x0C==hig_digit)
    {
        *(dst++) ='+';
        finish_count++ ;
    }
    else
    {
        ERR("HexToDialStr Src End with 0x%2X",hig_digit,0,0);
        break;
    } 
 }while(finish_count<num_dig);
 
 *dst='\0';//end the dialstring
 
 return finish_count;
}

static void UTK_MAKE_UTK_CALL(CUTK *pMe)
{
    PARAM_NOT_REF(pMe) 
    ICallApp *pCallApp = NULL;
    
    if(SUCCESS == ISHELL_CreateInstance(pMe->m_pShell, 
                                          AEECLSID_DIALER,
                                           (void**)&pCallApp))
    { 
        ICallApp_Make_UTK_Call_F(pCallApp);
    }
    if(pCallApp)
    {
        (void)ICallApp_Release(pCallApp);
        pCallApp = NULL;
    }
}

/*==============================================================================
函数: 
    UTK_wmscommandreport
       
说明: 
    wms 用此函数向命令发出者汇报命令处理结果。
       
参数: 
    cmd [in]: wms 命令类型。
    user_data[in]: 发布命令时传入的用户数据指针。
    cmd_err [in]: 命令状态或错误类型。
    
返回值:
    NULL 或 符合条件的节点指针。
       
备注:

==============================================================================*/
void UTK_wmscommandreport(wms_cmd_id_e_type   cmd,
                          void                *user_data,
                          wms_cmd_err_e_type  cmd_err)
{
    CUTK * pMe = (CUTK *) (user_data);
  
    if (pMe != NULL)
    {
        wmsutk_cmd_status_type *cmd_status = MALLOC(sizeof(wmsutk_cmd_status_type));
        
        if (cmd_status == NULL)
        {
            return;
        }
        cmd_status->cmd = cmd;
        cmd_status->cmd_err = cmd_err;
        
        (void)ISHELL_PostEventEx(pMe->m_pShell, 
                                 EVTFLG_ASYNC,
                                 AEECLSID_APP_UTK,
                                 EVT_WMS_CMD_STATUS, 
                                 0, 
                                 (uint32)cmd_status);
    }
}

/*==============================================================================
函数: 
    UTK_MsgCb
       
说明: 
    本函数为所有 MSG 相关事件的回调函数。
       
参数: 
    event [In]: CFG 相关事件 。
    pInfo [In]: wms_msg_event_info_s_type 结构指针，存有事件相关信息。
    
       
返回值:
    none
       
备注:
    Posts the Event to itself

==============================================================================*/
void UTK_MsgCb(wms_msg_event_e_type       event,
               wms_msg_event_info_s_type  *pInfo,
               boolean                    *shared)
{
    wms_msg_event_info_s_type   *pInfobuf=NULL;
    IShell                      *pShell = AEE_GetShell();
    uint8                       btRet; 
    AEEEvent                    evt=0;
    
    if (pShell == NULL)
    {
        MSG_ERROR("WMSAPP: pShell = NULL",0,0,0);
        return;
    }
    
    pInfobuf = sys_malloc(sizeof(wms_msg_event_info_s_type));
    if (pInfobuf == NULL)
    {
        MSG_ERROR("WMSAPP: msg_event_buf = NULL",0,0,0);
        return;
    }
    
    (void)MEMCPY(pInfobuf, pInfo, sizeof(wms_msg_event_info_s_type));
    MSG_ERROR("UTK_MsgCb: %d", event, 0, 0);
    switch (event)
    {
        case WMS_MSG_EVENT_SEND:
            if ((pInfobuf->submit_report_info.client_id == WMS_CLIENT_TYPE_CAT) &&
                (pInfobuf->submit_report_info.report_status != WMS_RPT_OK))
            {
                evt = EVT_WMS_MSG_SEND;
            }
            break;
            
        case WMS_MSG_EVENT_SUBMIT_REPORT:
            pInfobuf->submit_report_info.alpha_id.data = (uint8 *)MALLOC(WMS_ALPHA_ID_MAX);
            pInfobuf->submit_report_info.alpha_id.len = pInfo->submit_report_info.alpha_id.len;
            
            if ((pInfo->submit_report_info.alpha_id.data != NULL) &&
                (pInfo->submit_report_info.alpha_id.len > 0))
            {
                (void)MEMCPY(pInfobuf->submit_report_info.alpha_id.data,
                            pInfo->submit_report_info.alpha_id.data,
                            pInfo->submit_report_info.alpha_id.len);
            }
            
            evt = EVT_WMS_MSG_SUBMIT_REPORT;
            break;
            
        default:
            MSG_ERROR("Invalid msg event %d", event, 0,0);
            break;
    } // switch
    
    if (evt != 0)
    {
        btRet = ISHELL_PostEventEx(pShell,
                                   EVTFLG_ASYNC, 
                                   AEECLSID_APP_UTK, 
                                   evt,
                                   0, 
                                   (uint32) pInfobuf);
                        
        if (btRet == FALSE)
        {
            sys_free(pInfobuf);
            MSG_HIGH("Event  %x could not be posted!", evt,0,0);
        }
    }
    else
    {
        sys_free(pInfobuf);
    }
} // UTK_MsgCb() 

/*==============================================================================
函数:
    UTK_ProcessStatus

说明:
    函数处理来自 wms 的报告信息。

参数:
    pMe [in]: 指向 UTK Applet对象结构的指针。该结构包含小程序的特定信息。
    pRptInfo [in]: wms_submit_report_info_s_type 结构指针。

返回值:
    none

备注:

==============================================================================*/
void UTK_ProcessStatus(CUTK *pMe, wms_submit_report_info_s_type *pRptInfo)
{
    pMe->m_SendStatus = pRptInfo->report_status;
    pMe->m_SendtlStatus = pRptInfo->cause_info.tl_status;
    
    switch (pRptInfo->report_status)
    {
        case WMS_RPT_OK:
        case WMS_RPT_LL_ERROR:
        
        case WMS_RPT_OUT_OF_RESOURCES:
        case WMS_RPT_SMR_NO_RESOURCES:

        case WMS_RPT_NETWORK_NOT_READY:

        case WMS_RPT_PHONE_NOT_READY:

        case WMS_RPT_NO_ACK:

        case WMS_RPT_CDMA_TL_ERROR:
            break;

        case WMS_RPT_ACCESS_TOO_LARGE:
            return;

        case WMS_RPT_DC_TOO_LARGE:

        case WMS_RPT_NOT_ALLOWED_IN_AMPS:
            
        case WMS_RPT_CANNOT_SEND_BROADCAST:
            
        case WMS_RPT_RP_ERROR:
            
        case WMS_RPT_CP_ERROR:
            
        case WMS_RPT_SMR_TR1M_TIMEOUT:
            
        case WMS_RPT_SMR_TR2M_TIMEOUT:
            
        // Any other error
        default:
            break;
    }
    
    ERR("pRptInfo->report_status = %d", pRptInfo->report_status, 0, 0);
#ifdef FEATURE_OEMOMH
    if(pMe->m_bSendingSMSBG)
    {
        pMe->m_bSendingSMSBG = FALSE;
        if (pMe->m_SendStatus == WMS_RPT_OK)
        {
            UTK_GiveResponse(pMe, pMe->cmd_type, FALSE, UIM_TK_CMD_PERFORMED_SUCCESSFULLY);
        }
        else
        {
            UTK_GiveResponse(pMe, pMe->cmd_type, FALSE, UIM_TK_NETWORK_CURRENTLY_UNABLE_TO_PROCESS_COMMAND);
        }
        UTK_Release((IUTK *)pMe);
        return;
    }
#endif
    if ((pMe->m_pActiveDlgID == IDD_SENDMSG_DIALOG) &&
        (ISHELL_ActiveApplet(pMe->m_pShell) == AEECLSID_APP_UTK))
    {
        (void)ISHELL_PostEventEx(pMe->m_pShell,
                                 EVTFLG_ASYNC, 
                                 AEECLSID_APP_UTK, 
                                 EVT_SEND_STATUS,
                                 0, 
                                 0);
    }
}


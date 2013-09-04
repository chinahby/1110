/*==============================================================================
// 文件：
//        ClockApps.c
//        2007-11-12 Gemsea草拟版本(Draft Version)
//        版权所有(c) 2007 Anylook CO.,LTD.
//
// 文件说明：
//
// 作者：2007-11-12
// 创建日期：Gemsea
// 当前版本：Draft
==============================================================================*/

/*==============================================================================
修改历史记录：
when         who            what, where, why
----------   ---            ---------------------------------------------------
=============================================================================*/


/*==============================================================================
                                 本文件包含的外部文件
==============================================================================*/
#include "ClockApps_priv.h"
#include "oemnvint.h"
#include "OEMRTC.h"
#ifdef FEATURE_ICM
#include "AEECM.h"
#else
#include "AEETelephone.h"
#include "AEETelDef.h"
#endif

/*==============================================================================
                                 类型定义
==============================================================================*/

/*==============================================================================
                                 函数声明
==============================================================================*/

#if defined(AEE_STATIC)
int ClockAppsMod_Load(IShell *ps, void * pHelpers, IModule ** pMod);
static int  ClockApps_CreateInstance(AEECLSID    ClsId,
                                    IShell     *pIShell,
                                    IModule    *po,
                                    void      **ppObj);
#endif // AEE_STATIC

static int ClockApps_New(IShell * pIShell,IModule * po,void ** ppObj);

static uint32  ClockApps_AddRef(IClockApps *pi);

static uint32  ClockApps_Release (IClockApps *pi);

static boolean ClockApps_HandleEvent(IClockApps *pi,
    AEEEvent eCode,
    uint16  wParam,
    uint32 dwParam
);

static int ClockApps_InitAppData(CClockApps *pMe);

static void ClockApps_FreeAppData(CClockApps *pMe);

static void ClockApps_RunFSM(CClockApps *pMe);

static void ClockApps_Delay(CClockApps *pMe);
//定时器时间到，打开闹铃/振动
static boolean ClockApps_Expired(CClockApps *pMe, AlarmType eAlarmType, boolean justRegister);
static void ClockApps_APPIsReadyTimer(void *pme);
/*==============================================================================
                                 全局数据
==============================================================================*/


/*==============================================================================
                                 本地（静态）数据
==============================================================================*/


// 只允许一个ClockApps实例。每次创建ClockApps Applet时，
// 返回同一结构指针给BREW层。
static CClockApps gClockApps;

static const VTBL(IClockApps) gClockAppsMethods =
{
    ClockApps_AddRef,
    ClockApps_Release,
    ClockApps_HandleEvent
};

/*==============================================================================
函数:
       ClockAppsMod_Load

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
#if defined(AEE_STATIC)
int ClockAppsMod_Load(IShell *ps, void * pHelpers, IModule ** pMod)
{

return(AEEStaticMod_New((int16)(sizeof(AEEMod)),ps,pHelpers,pMod,(PFNMODCREATEINST)ClockApps_CreateInstance,(PFNFREEMODDATA)NULL));
}
#endif

/*==============================================================================
函数:
       ClockAppsMod_CreateInstance

说明:
        通过创建(初始化)ClockApps对象来创建模块实例。

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
static int  ClockApps_CreateInstance(AEECLSID    ClsId,
    IShell *pIShell,
                                    IModule    *po,
                                    void      **ppObj)
{
    *ppObj = NULL;

    if (ClsId != AEECLSID_ALARMCLOCK)
    {
        return EFAILED;
    }

    if (ClockApps_New(pIShell, po, ppObj) != SUCCESS)
    {
        return EFAILED;
    }
    return SUCCESS;
}
#endif // AEE_STATIC
/*----------------------Applet相关函数定义---------------------*/
/*==============================================================================
函数:
       ClockApps_New

说明:
        初始化ClockApps Applet虚拟表和内部数据变量，同时其引用计数加1。

参数:
       ps [in]：IShell接口对象指针。
       pIModule [in]：pIModule接口对象指针。
       ppObj [out]:：指向IClockApps对象指针的指针。

返回值:
       EFAILED：失败。
       SUCCESS：成功。

备注:
       不需用户去调用。
       关于Applet结构对象可使用动态分配内存的方式创建，本处采用的是使用静态
       变量的方式。建议对结构比较大的Applet使用静态变量的方式。对于动态分配
       内存的方式需要注意对所分配空间的释放。

==============================================================================*/
static int ClockApps_New(IShell * pIShell,IModule * po,void ** ppObj)
{
    CClockApps *pMe;
    int retVal = SUCCESS;

    pMe = &gClockApps;

    if (0 == pMe->m_nRefs)
    {
        // Must initialize the object
        MEMSET(pMe,  0, sizeof(CClockApps));
        INIT_VTBL(pMe, IClockApps, gClockAppsMethods);

        pMe->m_nRefs     = 0;
        pMe->m_pShell    = pIShell;
        pMe->m_pModule   = po;

        (void) ISHELL_AddRef(pMe->m_pShell);
        (void) IMODULE_AddRef(pMe->m_pModule);

        retVal = ClockApps_InitAppData(pMe);
    }

    ++pMe->m_nRefs;
    *ppObj = (IClockApps*)pMe;
    return retVal;
}

/*==============================================================================
函数:
       ClockApps_AddRef

说明:
       ClockApps Applet 引用计数加1，并返回当前的引用数。

参数:
       pi [in]：IClockApps接口对象指针。

返回值:
       当前对ClockApps Applet的引用数

备注:

==============================================================================*/
static uint32  ClockApps_AddRef(IClockApps *pi)
{
    register CClockApps *pMe = (CClockApps*)pi;

    return (++pMe->m_nRefs);
}

/*==============================================================================
函数:
       ClockApps_Release

说明:
       释放对ClockApps Applet的引用，引用计数减1，并返回当前的引用数。
        如果当前的引用数为零，还将释放Applet及相关资源。

参数:
       pi [in]：IClockApps接口对象指针。

返回值:
       当前对ClockApps Applet的引用数

备注:

==============================================================================*/
static uint32  ClockApps_Release (IClockApps *pi)
{
    register CClockApps *pMe = (CClockApps*)pi;

//    ASSERT(pMe != NULL);

    if (pMe->m_nRefs == 0)
    {
        return 0;
    }

    if (--pMe->m_nRefs)
    {
        return pMe->m_nRefs;
    }

    // 释放Applet相关资源
    ClockApps_FreeAppData(pMe);

    (void) ISHELL_Release(pMe->m_pShell);
    (void) IMODULE_Release(pMe->m_pModule);

    // 注意：pMe是静态分配空间，因此不需要释放。FREE()
    return 0;
}

/*==============================================================================
函数:
       ClockApps_InitAppData

说明:
       初始化ClockApps Applet结构数据缺省值。

参数:
       pMe [in]：指向ClockApps Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
       SUCCESS：初始化成功。
       EFAILED：初始化失败。

备注:

==============================================================================*/
static int ClockApps_InitAppData(CClockApps *pMe)
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
    pMe->m_ePreState = ALARMST_NONE;
    pMe->m_eCurState = ALARMST_INIT;
    pMe->m_pActiveDlg = NULL;
    pMe->m_eDlgRet = DLGRET_CREATE;
    pMe->m_bNotOverwriteDlgRet = FALSE;
    pMe->m_nCtlID = IDC_CLOCK_ST_LIST;   //控件ID
    pMe->m_nTimeCtlCount = 0;    //0代表焦点在小时上,1代表在分钟上
#ifdef FEATURE_SUB_LCD
    pMe->m_pWallSubImage = NULL;
#endif //FEATURE_SUB_LCD

    pMe->m_currDlgId = 0;

    pMe->m_nNumKeyCount = 0;    //时间控件按键次数
    pMe->m_dwDispTime = 0;
    pMe->m_dwHour = 0;
    pMe->m_dwMin = 0;

	pMe->m_bAlarmOff = TRUE;
    pMe->m_bAlarmSuspend = FALSE;

    if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                            AEECLSID_CONFIG,
                                            (void **)&pMe->m_pConfig))
    {
        ClockApps_FreeAppData(pMe);
        return EFAILED;
    }

    if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                            AEECLSID_BACKLIGHT,
                                            (void **)&pMe->m_pBacklight))
    {
        ClockApps_FreeAppData(pMe);
        return EFAILED;
    }

#if !defined( AEE_SIMULATOR)
    if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                            AEECLSID_ANNUNCIATOR,
                                            (void **)&pMe->m_pIAnn))
    {
        ClockApps_FreeAppData(pMe);
        return EFAILED;
    }
	IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE);
#endif

#if defined( FEATURE_CARRIER_SUDAN_SUDATEL) && !defined( AEE_SIMULATOR)
    ICONFIG_GetItem(pMe->m_pConfig, CFGI_LANGUAGE_SELECTION, &pMe->language, sizeof(pMe->language));
#endif

#if !defined( AEE_SIMULATOR)
    if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                            AEECLSID_ALERT,
                                            (void **)&pMe->m_pAlert))
    {
        ClockApps_FreeAppData(pMe);
        return EFAILED;
    }
#endif

#ifdef FEATURE_SUB_LCD
    // 创建IDisplaySub接口
    if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                            AEECLSID_DISPLAYSUB,
                                            (void **)&pMe->m_pDisplaySub))
    {
        ClockApps_FreeAppData(pMe);
        return EFAILED;
    }
#endif
    if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                            AEECLSID_SOUND,
                                            (void **)&pMe->m_pSound))
    {
        ClockApps_FreeAppData(pMe);
        return EFAILED;
    }
    //闹钟开或关LIST 控件
    if(AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                         AEECLSID_LISTCTL,
                         (void **)&pMe->m_pState))
    {
        ClockApps_FreeAppData(pMe);
        return EFAILED;
    }
    
    //时间控件
    if(AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                         AEECLSID_COUNTDOWNCTL,
                         (void **)&pMe->m_pTime))
    {
        ClockApps_FreeAppData(pMe);
        return EFAILED;
    }
    
    //循环方式LIST 控件
    if(AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                         AEECLSID_LISTCTL,
                         (void **)&pMe->m_pRepMode))
    {
        ClockApps_FreeAppData(pMe);
        return EFAILED;
    }
    
    if(AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                         AEECLSID_LISTCTL,
                         (void **)&pMe->m_pSnooze))
    {
        ClockApps_FreeAppData(pMe);
        return EFAILED;
    }
//end added

    return SUCCESS;
}

/*==============================================================================
函数:
       ClockApps_FreeAppData

说明:
       释放ClockApps Applet使用的相关资源。

参数:
       pMe [in]：指向ClockApps Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
       无。

备注:

==============================================================================*/
static void ClockApps_FreeAppData(CClockApps *pMe)
{
    if (NULL == pMe)
    {
        return ;
    }

    ISHELL_CancelTimer(pMe->m_pShell, NULL, pMe);

    if (pMe->m_pConfig)
    {
        ICONFIG_Release(pMe->m_pConfig);
        pMe->m_pConfig = NULL;
    }

    if (pMe->m_pBacklight)
    {
        IBACKLIGHT_Release(pMe->m_pBacklight);
        pMe->m_pBacklight = NULL;
    }

	if (pMe->m_pIAnn)
    {
        IANNUNCIATOR_Release(pMe->m_pIAnn);
		pMe->m_pIAnn = NULL;
    }

#if !defined( AEE_SIMULATOR)
    if (pMe->m_pAlert)
    {
        IALERT_Release(pMe->m_pAlert);
        pMe->m_pAlert = NULL;
    }
#endif

#ifdef FEATURE_SUB_LCD
    if (pMe->m_pDisplaySub != NULL)
    {
        IDISPLAY_Release(pMe->m_pDisplaySub);
        pMe->m_pDisplaySub = NULL;
    }
#endif

    if (pMe->m_pSound != NULL)
    {
        ISound_Release(pMe->m_pSound);
        pMe->m_pSound = NULL;
    }
    if(pMe->m_pState != NULL)
    {
        (void)IMENUCTL_Release(pMe->m_pState);
        pMe->m_pState = NULL;
    }
    if(pMe->m_pTime != NULL)
    {
        (void)ITIMECTL_Release(pMe->m_pTime);
        pMe->m_pTime = NULL;
    }
    if(pMe->m_pRepMode != NULL)
    {
        (void)IMENUCTL_Release(pMe->m_pRepMode);
        pMe->m_pRepMode = NULL;
    }
    if(pMe->m_pSnooze != NULL)
    {
        (void)IMENUCTL_Release(pMe->m_pSnooze);
        pMe->m_pSnooze = NULL;
    }
//end added
}

/*==============================================================================
函数:
       ClockApps_RunFSM

说明:
       ClockApps Applet有限状态机引擎。

参数:
       pMe [in]：指向ClockApps Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
       无。

备注:

==============================================================================*/
static void ClockApps_RunFSM(CClockApps *pMe)
{
    NextFSMAction nextFSMAction = NFSMACTION_WAIT;

    for ( ; ; )
    {
        nextFSMAction = ClockApps_ProcessState(pMe);

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
       ClockApps_HandleEvent

说明:
       ClockApps  Applet事件处理主函数。所有属于本Applet的事件，
       都将经由该函数处理。

参数:
       pi [in]：指向IClockApps接口t对象的指针。
       eCode：BREW事件代码。
       wParam：与事件相关联的数据。
       dwParam [in]：与事件相关联的数据。

返回值:
       TRUE：传入事件被处理。
       FALSE：传入事件没被处理。

备注:

==============================================================================*/
#define EVT_REREGISTER_ALARMS   EVT_USER + 20
static void reregisterAlarms( CClockApps *pMe)
{ 

	int i = 0;

	for(i = 0; i < NUM_OF_ALARMCLOCK; i++)
	{
		// 执行闹钟
		ClockApps_Expired(pMe, i, TRUE);
	}
}
static boolean ClockApps_HandleEvent(IClockApps *pi,
    AEEEvent eCode,
    uint16  wParam,
    uint32 dwParam
)
{
    CClockApps *pMe = (CClockApps*)pi;
    AEEAppStart *as;


    switch (eCode)
    {
        case EVT_APP_START:
            pMe->m_bAppIsReady = FALSE;
//            ASSERT(dwParam != 0);
            as = (AEEAppStart*)dwParam;
//            if (NULL != pMe->m_pDisplay)
//            {
//                (void) IDISPLAY_Release(pMe->m_pDisplay);
//                pMe->m_pDisplay = NULL;
//            }
            pMe->m_pDisplay = as->pDisplay;
//            (void) IDISPLAY_AddRef(pMe->m_pDisplay);
            pMe->m_rc = as->rc;

            {
                AEEDeviceInfo di; 
                ISHELL_GetDeviceInfo(pMe->m_pShell,&di);
                pMe->m_rc.dy = di.cyScreen;
            }

            pMe->m_bSuspending = FALSE;
            // 开始ClockApps状态机
            ClockApps_RunFSM(pMe);
            return TRUE;
        // 定时器时间到

        case EVT_ALARM:
            {
#if 0
            uint16 i;
#endif


#if 0
            if (!ClockApps_CanAlert(pMe))
            {
                // 因通话或其它原因，暂不能启动，将闹钟推迟2分钟
                pMe->m_eCurAlarmType = (AlarmType)wParam;
                //ClockApps_Delay(pMe);
            }
            else if( pMe->m_bAlarmOff)
#endif//这里如果在通话中也要执行闹钟，只是响闹的方式不同，所以在这里不加限制
            if(pMe->m_bAlarmOff)
            {
                pMe->m_bAlarmOff = FALSE;

#if 1
                ClockApps_Expired(pMe, wParam, FALSE);
#else
                    // 为了避免在设置有重复的闹钟和多个闹钟重起后有些闹钟不
                    // 能起闹的情况下,每次都全部判断一遍,若有重复的闹钟则都
                    // 执行,而看到的是闹钟编号大的那个.
                    for(i = 0; i < NUM_OF_ALARMCLOCK; i++)
                    {
                        // 执行闹钟
                        ClockApps_Expired(pMe, i);
                    }
#endif

                ISHELL_PostEvent( pMe->m_pShell, AEECLSID_ALARMCLOCK, EVT_REREGISTER_ALARMS, 0, 0);

            }
        }
        return TRUE;
        case EVT_ALARMOFF:
            if(pMe->m_eCurState == ALARMST_REACH)
            {
                pMe->m_bAlarmSuspend = TRUE;
            }
            return TRUE;
        case EVT_REREGISTER_ALARMS:
        {
#if defined( FEATURE_POWERDOWN_ALARM)
            registerPowerdownAlarmclock();
#endif
            reregisterAlarms( pMe);
        }
            return TRUE;

        case EVT_APP_STOP:
//            if (pMe->m_pDisplay != NULL)
//            {
//                (void) IDISPLAY_Release(pMe->m_pDisplay);
//                pMe->m_pDisplay = NULL;
//            }
            pMe->m_bSuspending = TRUE;

            return TRUE;

        case EVT_APP_SUSPEND:
            pMe->m_bSuspending = TRUE;

            return TRUE;

        case EVT_APP_RESUME:
//            ASSERT(dwParam != 0);
            as = (AEEAppStart*)dwParam;
            pMe->m_bSuspending = FALSE;

//            if (pMe->m_pDisplay != NULL)
//            {
//                (void) IDISPLAY_Release(pMe->m_pDisplay);
//                pMe->m_pDisplay = NULL;
//            }
            pMe->m_pDisplay = as->pDisplay;
//            (void) IDISPLAY_AddRef(pMe->m_pDisplay);
            pMe->m_rc = as->rc;
            {
                AEEDeviceInfo di; 
                ISHELL_GetDeviceInfo(pMe->m_pShell,&di);
                pMe->m_rc.dy = di.cyScreen;
            }


            if( pMe->m_eCurState == ALARMST_OPTION)
            {
                pMe->m_eCurState = ALARMST_MAIN;
            }
            if(pMe->m_eCurState == ALARMST_REACH)
            {
                if(pMe->m_bAlarmSuspend)
                {
                    CClockApps_Snooze(pMe);
                    pMe->m_eCurState = ALARMST_EXIT;   
                    pMe->m_bAlarmSuspend = FALSE;
                }
            }

            ClockApps_RunFSM(pMe);
            return TRUE;

        case EVT_DIALOG_INIT:
            if (wParam == OEM_IME_DIALOG)
            {
                return TRUE;
            }
            pMe->m_bAppIsReady = FALSE;
            pMe->m_pActiveDlg = (IDialog*)dwParam;
            pMe->m_pActiveDlgID = wParam;

            return ClockApps_RouteDialogEvent(pMe,eCode,wParam,dwParam);

        case EVT_DIALOG_START:
            if (wParam == OEM_IME_DIALOG)
            {
                return TRUE;
            }

            (void)ISHELL_SetTimer ( pMe->m_pShell,
                            APPISREADY_TIMER,
                            ClockApps_APPIsReadyTimer,
                            pMe);
            
            return ClockApps_RouteDialogEvent(pMe,eCode,wParam,dwParam);

        case EVT_USER_REDRAW:
            (void) ClockApps_RouteDialogEvent(pMe,eCode,wParam,dwParam);

            /*
            (void)ISHELL_SetTimer ( pMe->m_pShell,
                            APPISREADY_TIMER,
                            ClockApps_APPIsReadyTimer,
                            pMe);
            */
                
            return TRUE;

        case EVT_APPISREADY:
            (void) ClockApps_RouteDialogEvent(pMe,eCode,wParam,dwParam);
            pMe->m_bAppIsReady = TRUE;
            return TRUE;

        case EVT_KEY_PRESS:
#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
		case EVT_PEN_UP:
		//case EVT_PEN_DOWN: 
#endif
			#if !defined(FEATURE_VERSION_K212)
            if (!pMe->m_bAppIsReady)
            {
                return TRUE;
            }
			#endif
            return ClockApps_RouteDialogEvent(pMe,eCode,wParam,dwParam);

        case EVT_KEY_RELEASE:
			#if !defined(FEATURE_VERSION_K212)
            if (!pMe->m_bAppIsReady)
            {
                return TRUE;
            }
			#endif
            return ClockApps_RouteDialogEvent(pMe,eCode,wParam,dwParam);

        case EVT_KEY_HELD:
			#if !defined(FEATURE_VERSION_K212)
            if (!pMe->m_bAppIsReady)
            {
                return TRUE;
            }
			#endif
            return ClockApps_RouteDialogEvent(pMe,eCode,wParam,dwParam);

        case EVT_GSENSOR_SHAKE:
        case EVT_KEY:
			#if !defined(FEATURE_VERSION_K212)
            if (!pMe->m_bAppIsReady)
            {
                return TRUE;
            }
			#endif
            return ClockApps_RouteDialogEvent(pMe,eCode,wParam,dwParam);

/*        case EVT_PEN_DOWN_1:
        case EVT_PEN_MOVE:
        case EVT_PEN_DOWN:
//            ERR("HANDLE PEN EVENTS", 0, 0, 0);
            if (!pMe->m_bAppIsReady)
            {
//                ERR("APP IS NOT READY", 0, 0, 0);
                return TRUE;
            }
            return ClockApps_RouteDialogEvent(pMe,eCode,wParam,dwParam);
*/
        case EVT_COMMAND:
			#if !defined(FEATURE_VERSION_K212)
            if (!pMe->m_bAppIsReady)
            {
                return TRUE;
            }
			#endif
            return ClockApps_RouteDialogEvent(pMe,eCode,wParam,dwParam);

        case EVT_DIALOG_END:
            if (wParam == OEM_IME_DIALOG)
            {
               (void)ISHELL_PostEvent(pMe->m_pShell,
                                      AEECLSID_ALARMCLOCK,
                                      EVT_USER_REDRAW,
                                      0,
                                      0);
                return TRUE;
            }

            //pMe->m_AlarmReach = FALSE;

            if (wParam == 0)
            {
                return TRUE;
            }
            pMe->m_bAppIsReady = FALSE;
            (void) ClockApps_RouteDialogEvent(pMe,eCode,wParam,dwParam);
            pMe->m_pActiveDlg = NULL;

            // Applet被挂起，不跑状态机，等待Applet返回。（注意：EVT_APP_SUSPEND
            // 事件在EVT_DIALOG_END事件前发出。
            if (pMe->m_bSuspending == FALSE)
            {
                // 开始ClockApps状态机
                ClockApps_RunFSM(pMe);
            }
            return TRUE;
#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
		case EVT_USER:
		{
			if (wParam == AVK_CLR)
			{
				eCode = EVT_KEY;
			}
			else if ((wParam == AVK_SELECT)||(wParam == AVK_INFO))
			{
				if (dwParam == 0)
				{
					eCode = EVT_KEY_PRESS;
				}
				else
				{
					eCode = EVT_COMMAND;
				}
			}
			return ClockApps_RouteDialogEvent(pMe,eCode,wParam,dwParam);
		}
		return TRUE;
			
#endif

        default:
            // 将接收到的事件路由至当前活动的对话框事件处理函数。
            return ClockApps_RouteDialogEvent(pMe,eCode,wParam,dwParam);
    }
}

/*==============================================================================
函数:
       ClockApps_APPIsReadyTimer

说明:
       定时器回调函数。主要用于控制快速按键问题的处理。

参数:
       pme [in]：void *类型指针，暗指CClockApps结构指针。

返回值:
       无。

备注:

==============================================================================*/
static void ClockApps_APPIsReadyTimer(void *pme)
{
    CClockApps *pMe = (CClockApps *)pme;

    if (NULL == pMe)
    {
        return;
    }

    (void) ISHELL_PostEvent( pMe->m_pShell,
                            AEECLSID_ALARMCLOCK,
                            EVT_APPISREADY,
                            0,
                            0);
}



/*==============================================================================
函数：
       AlarmClock_Expired
说明：
       函数用来唤起闹钟主界面，并设定下次响闹时间。函数还会对当前时间和设定
       时间作比较，以确定是否延迟响闹，将误差控制在30秒内。

参数：
       pMe [in]：指向 AlarmClock Applet对象结构的指针。该结构包含小程序的特定信息。
       eAlarmType [in]：闹钟类型。

返回值：
       无。

备注：

==============================================================================*/
static boolean ClockApps_Expired(CClockApps *pMe, AlarmType eAlarmType, boolean justRegister)
{
    uint32 dwNow;
    uint32 dwAlarmTime;
    uint32 dwTepTime;
    uint32 dwSnooze;
    boolean  wWeekAlarmMask = FALSE;
    uint16 wValue;
    boolean bSetTimer = FALSE;

    // EVT_ALARM事件有时会提前设定时间数分钟到来，如是这种情况，bDelay变量
    // 应为TRUE，表示需推迟响闹。
    boolean bDelay = FALSE;
    JulianType  jDate;
#ifdef FEATURE_UIALARM
    IAlarm *pAlarm = NULL;
#endif
#if defined( FEATURE_ONCE_ALARM)
    boolean continueAlarm    = 0;
#endif

#ifdef FEATURE_VERSION_C337
	continueAlarm    = TRUE;
#endif
    if (pMe == NULL)
    {
        return FALSE;
    }

#ifdef FEATURE_UIALARM
    if (SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                        AEECLSID_UIALARM,
                                        (void**)&pAlarm))
    {
        return FALSE;
    }
#endif
    //获取闹钟首选项的数据结构
    (void) ISHELL_GetPrefs(pMe->m_pShell,
                           AEECLSID_ALARMCLOCK,
                           CLOCK_CFG_VERSION,
                           &pMe->m_ClockCfg,
                           sizeof(ClockAppCfg));

    //是否开启了闹钟
    if (pMe->m_ClockCfg.bStateOn[eAlarmType])
    {
        bSetTimer = TRUE;       //闹钟已经设置了开启
    }

    if (!bSetTimer) // 取消定时器
    {
#ifdef FEATURE_UIALARM
        (void)IAlarm_CancelAlarm(pAlarm,
                AEECLSID_ALARMCLOCK,
                (uint16)eAlarmType);
        IAlarm_Release(pAlarm);
#else
        (void) ISHELL_CancelAlarm(pMe->m_pShell,
                AEECLSID_ALARMCLOCK,
                (uint16)eAlarmType);
#endif
		return FALSE;
    }

    //计算实际定时时间,并启动AEEAlarm进行定时操作的实际管理
    // 取当前时间
    dwNow = GETTIMESECONDS();
    GETJULIANDATE(dwNow, &jDate);

    wValue = pMe->m_ClockCfg.RepMode[eAlarmType];  //重复方式

    switch (eAlarmType)
    {
        case WEEK_ALARM_1:
        case WEEK_ALARM_2:
        case WEEK_ALARM_3:
        case WEEK_ALARM_4:
        case WEEK_ALARM_5:
        case WEEK_ALARM_6:
        //case WEEK_ALARM_7:
        //case WEEK_ALARM_8:
            // 判断当天星期闹钟是否设置
            switch (wValue)
            {
                case WEEK_ALARM_REP1:   // 每天
                    wWeekAlarmMask = TRUE;
                    break;

#if defined( FEATURE_CARRIER_ISRAEL_PELEPHONE)
                case WEEK_ALARM_REP2: // 周日~~周四
                    if( jDate.wWeekDay < 4 || jDate.wWeekDay == 6)
                    {
                        wWeekAlarmMask = TRUE;
                    }
                    break;

                case WEEK_ALARM_REP3: // 周日~~周五
                    if( jDate.wWeekDay != 5)
                    {
                        wWeekAlarmMask = TRUE;
                    }
                    break;

                case WEEK_ALARM_REP4: // 周日
                    if(jDate.wWeekDay == 6)
                    {
                        wWeekAlarmMask = TRUE;
                    }
                    break;

                case WEEK_ALARM_REP5: // 周1
                    if(jDate.wWeekDay == 0)
                    {
                        wWeekAlarmMask = TRUE;
                    }
                    break;

                case WEEK_ALARM_REP6: // 周2
                    if(jDate.wWeekDay == 1)
                    {
                        wWeekAlarmMask = TRUE;
                    }
                    break;

                case WEEK_ALARM_REP7: // 周3
                    if(jDate.wWeekDay == 2)
                    {
                        wWeekAlarmMask = TRUE;
                    }
                    break;

                case WEEK_ALARM_REP8: // 周4
                    if(jDate.wWeekDay == 3)
                    {
                        wWeekAlarmMask = TRUE;
                    }
                    break;

                case WEEK_ALARM_REP9: // 周5
                    if(jDate.wWeekDay == 4)
                    {
                        wWeekAlarmMask = TRUE;
                    }
                    break;

                case WEEK_ALARM_REP10: // 周6
                    if(jDate.wWeekDay == 5)
                    {
                        wWeekAlarmMask = TRUE;
                    }
                    break;
#else
                case WEEK_ALARM_REP2: // 周一~~周五
                    if(jDate.wWeekDay > 4)//周六和周日
                    {
                        wWeekAlarmMask = FALSE;
                    }
                    else
                    {
                        wWeekAlarmMask = TRUE;
                    }
                    break;

                case WEEK_ALARM_REP3: // 周一~~周六
                    if(jDate.wWeekDay > 5)//周日
                    {
                        wWeekAlarmMask = FALSE;
                    }
                    else
                    {
                        wWeekAlarmMask = TRUE;
                    }
                    break;

                case WEEK_ALARM_REP4: // 周一
                    if(jDate.wWeekDay == 0)
                    {
                        wWeekAlarmMask = TRUE;
                    }
                    break;

                case WEEK_ALARM_REP5: // 周二
                    if(jDate.wWeekDay == 1)
                    {
                        wWeekAlarmMask = TRUE;
                    }
                    break;

                case WEEK_ALARM_REP6: // 周三
                    if(jDate.wWeekDay == 2)
                    {
                        wWeekAlarmMask = TRUE;
                    }
                    break;

                case WEEK_ALARM_REP7: // 周四
                    if(jDate.wWeekDay == 3)
                    {
                        wWeekAlarmMask = TRUE;
                    }
                    break;

                case WEEK_ALARM_REP8: // 周五
                    if(jDate.wWeekDay == 4)
                    {
                        wWeekAlarmMask = TRUE;
                    }
                    break;

                case WEEK_ALARM_REP9: // 周六
                    if(jDate.wWeekDay == 5)
                    {
                        wWeekAlarmMask = TRUE;
                    }
                    break;

                case WEEK_ALARM_REP10: // 周日
                    if(jDate.wWeekDay == 6)
                    {
                        wWeekAlarmMask = TRUE;
                    }
                    break;
#endif
                    
#if defined( FEATURE_ONCE_ALARM)
                case WEEK_ALARM_REP11:   // 一次
                    wWeekAlarmMask = TRUE;
                    break;
#endif

                default:
#ifdef FEATURE_UIALARM
                    IAlarm_Release(pAlarm);
#endif
                    return FALSE;
            }

            //闹钟状态是启动,且当天闹钟已设置,则闹钟有效
            if (pMe->m_ClockCfg.bStateOn[eAlarmType] && wWeekAlarmMask)
            {
                pMe->m_bWeekAlarmEnabled = TRUE;
            }
            else
            {
                pMe->m_bWeekAlarmEnabled = FALSE;
            }
            //闹钟时间
            dwAlarmTime = pMe->m_ClockCfg.dwWATime[eAlarmType];
            break;

        default:
#ifdef FEATURE_UIALARM
            IAlarm_Release(pAlarm);
#endif
			return FALSE;
    }

    // 将时间转换为秒，计算设定响闹时间值dwAlarmTime
#if defined( FEATURE_ONCE_ALARM)
	dwAlarmTime = dwAlarmTime;
#else
	dwAlarmTime = dwAlarmTime / 1000;
#endif
    jDate.wHour = 0;
    jDate.wMinute = 0;
    jDate.wSecond = 0;
#if defined( FEATURE_ONCE_ALARM)
	if( pMe->m_ClockCfg.RepMode[eAlarmType] != WEEK_ALARM_REP11)
	{
		dwAlarmTime += JULIANTOSECONDS(&jDate);
	}
#else
	dwAlarmTime += JULIANTOSECONDS(&jDate);
#endif
    dwSnooze = (pMe->m_ClockCfg.Snooze[eAlarmType] + MAX_ALARM_MSECS/1000) * 10;
    MSG_FATAL("dwSnooze = %d",dwSnooze,0,0);
    MSG_FATAL("dwNow = %d;dwAlarmTime = %d",dwNow,dwAlarmTime,0);
    if (dwAlarmTime > dwNow)
    {
        // 当天闹钟时间未到
        dwTepTime = dwAlarmTime - dwNow;

        // 为降低误差，将误差控制在10
        // 秒以内，若dwTepTime小于10，则立即响闹；否则至少延迟1分钟。
        // 因此闹钟在设定时间前后10内启动均为正常。
        if (dwTepTime >= 10)
        {
            bDelay = TRUE;

#if defined( FEATURE_ONCE_ALARM)
			continueAlarm = TRUE;
#endif
        }
        else
        {
            // 时间与设定时间误差低于1分钟，予以向闹，但下轮时间需增加1天,
            // 避免刚提示过，马上又来EVT_ALARM事件
            dwTepTime = 24 * 60 * 60;
        }
    }
    else if((dwNow - dwAlarmTime) <= dwSnooze)   //允许有10秒的误差
    {
        // 当天闹钟时间已过，设定为次日
        dwTepTime = dwAlarmTime + 24 * 60 * 60 - dwNow;
    }
    else   //小于
    {
        // 当天闹钟时间已过，设定为次日
        dwTepTime = dwAlarmTime + 24 * 60 * 60 - dwNow;
        bDelay = TRUE;
    }

#if defined( FEATURE_ONCE_ALARM)
    if( pMe->m_ClockCfg.RepMode[eAlarmType] == WEEK_ALARM_REP11 && !continueAlarm)
    {
        pMe->m_ClockCfg.bStateOn[eAlarmType] = 0;
        (void) ISHELL_SetPrefs(pMe->m_pShell,
                           AEECLSID_ALARMCLOCK,
                           CLOCK_CFG_VERSION,
                           &pMe->m_ClockCfg,
                           sizeof(ClockAppCfg));
    }
#endif

    // 将秒化为分钟
    //    dwTepTime /= 60;
    // Now the parameter have been changed to seconds instead of min

    if (dwTepTime == 0)
    {
        dwTepTime = 5;
    }

#if defined( FEATURE_ONCE_ALARM)
    if( pMe->m_ClockCfg.RepMode[eAlarmType] != WEEK_ALARM_REP11 ||
        ( pMe->m_ClockCfg.RepMode[eAlarmType] == WEEK_ALARM_REP11 && continueAlarm)
    )
    {
#ifdef FEATURE_UIALARM
        (void) IAlarm_SetAlarm(pAlarm,
                                AEECLSID_ALARMCLOCK,
                                (uint16)eAlarmType,
                                dwTepTime);
        IAlarm_Release(pAlarm);
#else
        (void) ISHELL_SetAlarm(pMe->m_pShell,
                               AEECLSID_ALARMCLOCK,
                                (uint16)eAlarmType,
                                dwTepTime);
#endif
    }
#else
#ifdef FEATURE_UIALARM
    (void) IAlarm_SetAlarm(pAlarm,
                            AEECLSID_ALARMCLOCK,
                            (uint16)eAlarmType,
                            dwTepTime);
    IAlarm_Release(pAlarm);
#else
    (void) ISHELL_SetAlarm(pMe->m_pShell,
                           AEECLSID_ALARMCLOCK,
                            (uint16)eAlarmType,
                            dwTepTime);
#endif
#endif

    if (bDelay)
    {
        return FALSE;
    }

    // 星期闹钟
    if (!pMe->m_bWeekAlarmEnabled || justRegister)
    {
        return FALSE;
    }
    
    if (!ClockApps_CanAlert(pMe))// 这里如果在通话中就只震动5秒，而不进入提示界面
    {
        ISOUND_Vibrate(pMe->m_pSound, 2000);
        if(AEECLSID_DIALER == ISHELL_ActiveApplet(pMe->m_pShell))
        {
            ISHELL_PostEvent(pMe->m_pShell, AEECLSID_DIALER, EVT_ALARM, 0, 0);
        }
        return TRUE;
    }
    else
    {
        // REACH对话框要显示的闹钟
        pMe->m_eCurAlarmType = (AlarmType)eAlarmType;

        MOVE_TO_STATE(ALARMST_REACH)    //转到闹钟时间到状态
        // 当前 Applet 尚未处于活动状态，启动 Applet
        if (ISHELL_ActiveApplet(pMe->m_pShell) != AEECLSID_ALARMCLOCK)
        {
            (void) ISHELL_StartApplet(pMe->m_pShell, AEECLSID_ALARMCLOCK);
        }
        else
        {
            if(ISHELL_GetActiveDialog(pMe->m_pShell)
            != pMe->m_pActiveDlg)
            {
                (void)ISHELL_EndDialog(pMe->m_pShell);
            }

            CLOSE_DIALOG(DLGRET_CREATE);

        }
    }
    return TRUE;
}


/*==============================================================================
函数：
       ClockApps_CanAlert
说明：
       函数用来确定是否可以唤起闹钟主界面，目前仅对通话情况做出判断，以后有其它
       要求可在此添加。

参数：
       pMe [in]：指向 AlarmClock Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       TRUE: 可以提示
       FALSE: 不可以提示

备注：

==============================================================================*/
boolean ClockApps_CanAlert(CClockApps *pMe)
{
#if 0    
    db_items_value_type dbItemValue = {0};
    db_get( DB_IN_USE, &dbItemValue);
    if( dbItemValue.in_use)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
#endif   // #if 0   db.in_use也包括了wap连接时的状态，用下面的判断方法实现
#ifdef FEATURE_ICM
    ICM *pICM = NULL;
    uint16 num = 0;
 
    if(AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell, 
                                            AEECLSID_CM, 
                                            (void **)&pICM))
    {
        return FALSE;
    }
    if(pICM)
    {
        num = ICM_GetActiveCallIDs(pICM, 
                               (AEECM_CALL_TYPE_VOICE | AEECM_CALL_TYPE_EMERGENCY), 
                               (AEECM_CALL_STATE_ORIG | AEECM_CALL_STATE_INCOM | 
                                AEECM_CALL_STATE_CONV | AEECM_CALL_STATE_ONHOLD | 
                                AEECM_CALL_STATE_DORMANT),
                               NULL, 
                               0);
        
        ICM_Release(pICM);
        pICM = NULL;
    }
#else
    ITelephone *pITelephone = NULL;
    uint16 num = 0;
 
    if(AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell, 
                                            AEECLSID_TELEPHONE, 
                                            (void **)&pITelephone))
    {
        return FALSE;
    }
    if(pITelephone)
    {
    	
		AEETCalls po;
		
		if(SUCCESS != ITELEPHONE_GetCalls(pITelephone, &po,sizeof(AEETCalls)))
		{
			return FALSE;
		}
        num = po.dwCount;
        
        ITELEPHONE_Release(pITelephone);
        pITelephone = NULL;
    }
#endif
    if(num > 0)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}



/*==============================================================================
函数：
       ClockApps_Delay
说明：
       函数用来将闹钟推迟2分钟

参数：
       pMe [in]：指向 AlarmClock Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       无

备注：

==============================================================================*/
static void ClockApps_Delay(CClockApps *pMe)
{
#ifdef FEATURE_UIALARM
    IAlarm *pAlarm = NULL;
#endif

    if (NULL == pMe)
    {
        return;
    }

#ifdef FEATURE_UIALARM
    if (SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
        AEECLSID_UIALARM,
        (void**)&pAlarm))
    {
        return;
    }
#endif

    //更新显示的闹钟时间向后退移2分钟
    //...

#ifdef FEATURE_UIALARM
    (void) IAlarm_SetAlarm(pAlarm,
                            AEECLSID_ALARMCLOCK,
                            (uint16)pMe->m_eCurAlarmType,
                            (120 - 1));//避免delay时设置的1秒误差
    //获取闹钟首选项的数据结构
    (void) ISHELL_GetPrefs(pMe->m_pShell,
                           AEECLSID_ALARMCLOCK,
                           CLOCK_CFG_VERSION,
                           &pMe->m_ClockCfg,
                           sizeof(ClockAppCfg));
    //保存重新设置的时间
    pMe->m_ClockCfg.dwWATime[pMe->m_eCurAlarmType] += 2*MSSEC_PER_MIN;

    //存储闹钟首选项的数据结构
    (void) ISHELL_SetPrefs(pMe->m_pShell,
                           AEECLSID_ALARMCLOCK,
                           CLOCK_CFG_VERSION,
                           &pMe->m_ClockCfg,
                           sizeof(ClockAppCfg));
    IAlarm_Release(pAlarm);
#else
    (void) ISHELL_SetAlarm(pMe->m_pShell,
                            AEECLSID_ALARMCLOCK,
                            (uint16)pMe->m_eCurAlarmType,
                            120);
#endif
}

// when restore factory settings, the ClockAppCfg SHALL be zero-cleared
void ClockApps_ClearAll(void)
{
    ClockAppCfg cfg;

    MEMSET( &cfg, 0, sizeof( cfg));
    ISHELL_SetPrefs( AEE_GetShell(),
               AEECLSID_ALARMCLOCK,
               CLOCK_CFG_VERSION,
               &cfg,
               sizeof(ClockAppCfg)
           );
}


#if defined( FEATURE_POWERDOWN_ALARM)
static int registerPowerdownAlarmclockIf( uint32 now)
{
    ClockAppCfg clockCfg        = {0};
    uint32      today           = 0;
    uint32      last            = 0xffffffff;
    uint32      time            = 0;
    boolean     wWeekAlarmMask  = 0;
    boolean     registered      = 0;
    JulianType  jdate           = {0};
    int         result          = 0;
    int         i               = 0;
#ifdef FEATURE_APP_WORLDTIME
    extern boolean Calendar_FormatDateTime( uint32 seconds, AECHAR* resultString, int resultStringLength);
#endif /*FEATURE_APP_WORLDTIME*/

    GETJULIANDATE( now, &jdate);
    jdate.wHour = jdate.wMinute = jdate.wSecond = 0;
    today = JULIANTOSECONDS( &jdate);
    if( now == 0 || today == 0)
    {
        return -1;
    }
    result = ISHELL_GetPrefs( AEE_GetShell(),
                    AEECLSID_ALARMCLOCK,
                    CLOCK_CFG_VERSION,
                    &clockCfg,
                    sizeof(ClockAppCfg)
               );

    if( result !=SUCCESS)
    {
        i = NUM_OF_ALARMCLOCK;
    }

    {
        PowerDown_Alarm_Cfg alarmCfg = {0};
        OEM_SetConfig(CFGI_POWERDOWN_ALARM, (void*)&alarmCfg, sizeof(alarmCfg));
    }

    for( i = 0; i < NUM_OF_ALARMCLOCK ; i++)
    {

        if( !clockCfg.bStateOn[i])
        {
            continue;
        }

        switch ( clockCfg.RepMode[i])
        {
            case WEEK_ALARM_REP1:   // 每天
                wWeekAlarmMask = TRUE;
                break;

#if defined( FEATURE_CARRIER_ISRAEL_PELEPHONE)
				case WEEK_ALARM_REP2: // 周日~~周四
					if( jdate.wWeekDay < 4 || jdate.wWeekDay == 6)
					{
						wWeekAlarmMask = TRUE;
					}
					break;

				case WEEK_ALARM_REP3: // 周日~~周五
					if( jdate.wWeekDay != 5)
					{
						wWeekAlarmMask = TRUE;
					}
					break;

				case WEEK_ALARM_REP4: // 周日
					if(jdate.wWeekDay == 6)
					{
						wWeekAlarmMask = TRUE;
					}
					break;

				case WEEK_ALARM_REP5: // 周1
					if(jdate.wWeekDay == 0)
					{
						wWeekAlarmMask = TRUE;
					}
					break;

				case WEEK_ALARM_REP6: // 周2
					if(jdate.wWeekDay == 1)
					{
						wWeekAlarmMask = TRUE;
					}
					break;

				case WEEK_ALARM_REP7: // 周3
					if(jdate.wWeekDay == 2)
					{
						wWeekAlarmMask = TRUE;
					}
					break;

				case WEEK_ALARM_REP8: // 周4
					if(jdate.wWeekDay == 3)
					{
						wWeekAlarmMask = TRUE;
					}
					break;

				case WEEK_ALARM_REP9: // 周5
					if(jdate.wWeekDay == 4)
					{
						wWeekAlarmMask = TRUE;
					}
					break;

				case WEEK_ALARM_REP10: // 周6
					if(jdate.wWeekDay == 5)
					{
						wWeekAlarmMask = TRUE;
					}
					break;
#else
				case WEEK_ALARM_REP2: // 周一~~周五
                if(jdate.wWeekDay > 4)//周六和周日
                {
                    wWeekAlarmMask = FALSE;
                }
                else
                {
                    wWeekAlarmMask = TRUE;
                }
                break;

            case WEEK_ALARM_REP3: // 周一~~周六
                if(jdate.wWeekDay > 5)//周日
                {
                    wWeekAlarmMask = FALSE;
                }
                else
                {
                    wWeekAlarmMask = TRUE;
                }
                break;

            case WEEK_ALARM_REP4: // 周一
                if(jdate.wWeekDay == 0)
                {
                    wWeekAlarmMask = TRUE;
                }
                break;

            case WEEK_ALARM_REP5: // 周二
                if(jdate.wWeekDay == 1)
                {
                    wWeekAlarmMask = TRUE;
                }
                break;

            case WEEK_ALARM_REP6: // 周三
                if(jdate.wWeekDay == 2)
                {
                    wWeekAlarmMask = TRUE;
                }
                break;

            case WEEK_ALARM_REP7: // 周四
                if(jdate.wWeekDay == 3)
                {
                    wWeekAlarmMask = TRUE;
                }
                break;

            case WEEK_ALARM_REP8: // 周五
                if(jdate.wWeekDay == 4)
                {
                    wWeekAlarmMask = TRUE;
                }
                break;

            case WEEK_ALARM_REP9: // 周六
                if(jdate.wWeekDay == 5)
                {
                    wWeekAlarmMask = TRUE;
                }
                break;

            case WEEK_ALARM_REP10: // 周日
                if(jdate.wWeekDay == 6)
                {
                    wWeekAlarmMask = TRUE;
                }
                break;
#endif
                
            case WEEK_ALARM_REP11: // 一次
                wWeekAlarmMask = TRUE;
                break;  

            default:
                wWeekAlarmMask = FALSE;
                break;
        }

        if( !wWeekAlarmMask)
        {
            continue;
        }

#if defined( FEATURE_ONCE_ALARM)
		if( clockCfg.RepMode[i] == WEEK_ALARM_REP11)
		{
			time = clockCfg.dwWATime[i];
		}
		else
		{
			time = clockCfg.dwWATime[i] + today;
		}
#else
		time = clockCfg.dwWATime[i] / 1000 + today;
#endif
        if( time > now && last > time)
        {
            PowerDown_Alarm_Cfg alarmCfg    = {0};
            AECHAR              text[64]    = {0};

            last                = time;
            alarmCfg.bStateOn   = TRUE;
            alarmCfg.alarm_id   = i;
            alarmCfg.dwWATime   = last;
            OEM_SetConfig( CFGI_POWERDOWN_ALARM, (void*)&alarmCfg, sizeof(alarmCfg));
#ifdef FEATURE_APP_WORLDTIME
            Calendar_FormatDateTime( time, text, sizeof( text));
#endif /*FEATURE_APP_WORLDTIME*/

            registered ++;
        }
    }
#ifndef WIN32
#if !defined( AEE_SIMULATOR)
    if( registered)
    {
        OEMRTC_Process_PowerDown_Alarm();
    }
#endif
#endif//WIN32
    return registered;
}

void registerPowerdownAlarmclock( void)
{
    uint32      now             = 0;
    uint32      tommorrow       = 0;
    JulianType  jdate           = {0};
    int         result          = 0;

#ifndef WIN32
    if( !OEMRTC_Get_Current_Time( &now))
    {
        return;
    }
#endif//WIN32
	result = registerPowerdownAlarmclockIf( now);
    if( result == 0)
    {
        GETJULIANDATE( now, &jdate);
        jdate.wHour = jdate.wMinute = jdate.wSecond = 0;
        tommorrow = JULIANTOSECONDS( &jdate) + 24*60*60;

        result = registerPowerdownAlarmclockIf( tommorrow);
    }
}

void snoozePowerdownAlarmclock( void)
{
    ClockAppCfg         clockCfg    = {0};
    PowerDown_Alarm_Cfg alarmCfg    = {0};
    int                 result      = 0;

    AECHAR text[64] = {0};
#ifdef FEATURE_APP_WORLDTIME    
    extern boolean Calendar_FormatDateTime( uint32 seconds, AECHAR* resultString, int resultStringLength);
#endif /*FEATURE_APP_WORLDTIME*/

    result = ISHELL_GetPrefs( AEE_GetShell(),
                    AEECLSID_ALARMCLOCK,
                    CLOCK_CFG_VERSION,
                    &clockCfg,
                    sizeof(ClockAppCfg)
               );

    if( result == SUCCESS)
    {
        uint32      oldtime = 0;
        uint32      newtime = 0;
        uint32      now     = 0;
        uint32      today   = 0;
        uint32      snooze  = 0;
        JulianType  jdate   = {0};
#ifndef WIN32
        if( !OEMRTC_Get_Current_Time( &now))
        {
            return;
        }
#endif//WIN32        
        snooze = clockCfg.Snooze[alarmCfg.alarm_id];
        if(0 == snooze)
        {
            return;
        }
        
        GETJULIANDATE( now, &jdate);
        jdate.wHour = jdate.wMinute = jdate.wSecond = 0;
        today = JULIANTOSECONDS( &jdate);

        OEM_GetConfig( CFGI_POWERDOWN_ALARM, (void*)&alarmCfg, sizeof(alarmCfg));

        oldtime = clockCfg.dwWATime[alarmCfg.alarm_id];
#if defined( FEATURE_ONCE_ALARM)
		if( clockCfg.RepMode[alarmCfg.alarm_id] == WEEK_ALARM_REP11)
		{
			newtime = oldtime;
		}
		else
		{
			newtime = oldtime + today;
		}
#else
		newtime = oldtime / 1000 + today;
#endif
#ifdef FEATURE_APP_WORLDTIME
        Calendar_FormatDateTime( newtime, text, sizeof( text));
#endif /*FEATURE_APP_WORLDTIME*/

        while( newtime < now)
        {
            newtime += snooze;
        }
#ifdef FEATURE_APP_WORLDTIME        
        Calendar_FormatDateTime( newtime, text, sizeof( text));
#endif /*FEATURE_APP_WORLDTIME*/

#if defined( FEATURE_ONCE_ALARM)
		if( clockCfg.RepMode[alarmCfg.alarm_id] == WEEK_ALARM_REP11)
		{
			clockCfg.dwWATime[alarmCfg.alarm_id] = newtime;
		}
		else
		{
			clockCfg.dwWATime[alarmCfg.alarm_id] = newtime - today;
		}
#else
		clockCfg.dwWATime[alarmCfg.alarm_id] = (newtime - today) * 1000;
#endif
        ISHELL_SetPrefs( AEE_GetShell(), AEECLSID_ALARMCLOCK, CLOCK_CFG_VERSION, &clockCfg, sizeof(ClockAppCfg));

        registerPowerdownAlarmclock();

        clockCfg.dwWATime[alarmCfg.alarm_id] = oldtime;
        ISHELL_SetPrefs( AEE_GetShell(), AEECLSID_ALARMCLOCK, CLOCK_CFG_VERSION, &clockCfg, sizeof(ClockAppCfg));
    }
    else
    {
    }
}

void registerAgainPowerdownAlarmclock( void)
{
    PowerDown_Alarm_Cfg     alarmCfg    = {0};
    uint32                  now         = 0;
#ifndef WIN32
    if( !OEMRTC_Get_Current_Time( &now))
    {
        return;
    }
#endif//WIN32
    OEM_GetConfig( CFGI_POWERDOWN_ALARM, (void*)&alarmCfg, sizeof(alarmCfg));
    if( alarmCfg.dwWATime >= now)
    {
#ifndef WIN32
        OEMRTC_Process_PowerDown_Alarm();
#endif//WIN32
    }
    else
    {
        registerPowerdownAlarmclock();
    }
}
#endif

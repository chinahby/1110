/*==============================================================================
// 文件：
//        ScheduleApp.c
//        2007-11-21 Gemsea草拟版本(Draft Version)
//        版权所有(c) 2007 Anylook
//
// 文件说明：
//
// 作者：2007-11-21
// 创建日期：
// 当前版本：Draft
==============================================================================*/

/*==============================================================================
修改历史记录：
时    间      修   改   人     修改内容、位置及原因
--------      -----------     -----------------------------------------------
==============================================================================*/


/*==============================================================================
                                 本文件包含的外部文件
==============================================================================*/
#include "ScheduleApp_priv.h"
#include "AEECM.h"

#ifdef FEATURE_SUPPORT_VC0848
#include "OEMSVC.h"
#include "Vc0848.h"
#endif

/*==============================================================================
                                 宏定义和常数
==============================================================================*/

/*==============================================================================
                                 类型定义
==============================================================================*/

/*==============================================================================
                                 函数声明
==============================================================================*/
/*----------------------Applet相关函数声明---------------------*/
#if defined(AEE_STATIC)
static int      ScheduleApp_CreateInstance( AEECLSID    ClsId,
                                            IShell     *pIShell,
                                            IModule    *po,
                                            void      **ppObj);
#endif

static int      ScheduleApp_New(IShell * pIShell,IModule * po,void ** ppObj);

static uint32   IScheduleApp_AddRef(IScheduleApp *pi);

static uint32   IScheduleApp_Release(IScheduleApp *pi);

static boolean  IScheduleApp_HandleEvent( IScheduleApp   *pi,
                                          AEEEvent       eCode,
                                          uint16         wParam,
                                          uint32         dwParam);

static void     IScheduleApp_SportsPlan(IScheduleApp *pi);

static int  IScheduleApp_DeleteAllEntry(IScheduleApp *pi);

static int      CScheduleApp_InitAppData(CScheduleApp *pme);

static void     CScheduleApp_FreeAppData(CScheduleApp *pme);

static void     CScheduleApp_RunFSM(CScheduleApp *pme);

static void     Schedule_APPIsReadyTimer(void *pme);

static boolean  Schedule_CanAlert(CScheduleApp *pme);


/*==============================================================================
                                 全局数据
==============================================================================*/


/*==============================================================================
                                 本地（静态）数据
==============================================================================*/

#define SCHEDULEAPP_METHODS            \
        {                              \
             IScheduleApp_AddRef,      \
             IScheduleApp_Release,     \
             IScheduleApp_HandleEvent,  \
             IScheduleApp_SportsPlan,    \
             IScheduleApp_DeleteAllEntry  \
        }

static CScheduleApp gScheduleApp;
static const VTBL(IScheduleApp) gScheduleAppMethods = SCHEDULEAPP_METHODS;

/*==============================================================================
                                 函数定义
==============================================================================*/
/*----------------------Applet相关函数定义---------------------*/
/*==============================================================================
函数:
       ScheduleApp_Load(IShell *ps, void * pHelpers, IModule ** pMod)

说明:
        由BREW调用以初始静态模块的标准函数。

参数:
       ps [in]：IShell接口对象指针。
       pHelpers [in]：
       pIModule [in]：pIModule接口对象指针。

返回值:
       非0：失败。
       0：成功。

备注:

==============================================================================*/
int ScheduleApp_Load(IShell *ps, void * pHelpers, IModule ** pMod)
{
    //debug( ";****************ScheduleApp_Load*******************");
    return(AEEStaticMod_New( (int16)(sizeof(AEEMod)),
                             ps ,
                             pHelpers,
                             pMod,
                             (PFNMODCREATEINST)ScheduleApp_CreateInstance,
                             (PFNFREEMODDATA)NULL));
} // ScheduleApp_Load
/*==============================================================================
函数:
       ScheduleApp_CreateInstance

说明:
       This function is invoked while the app is being loaded. All Modules must
       provide this function. Ensure to retain the same name and parameters for
       this function

参数:
       ClsId:    [in]: Specifies the ClassID of the applet which is being loaded
       pIShell:  [in]: Contains pointer to the IShell interface.
       pIModule: [in]: Contains pointer to the IModule interface to the current
                       module to whichthis app belongs
       ppObj:   [out]: On return, *ppObj must point to a valid AEEApplet
                       structure. Allocation of memory for this structure and
                       initializing the base data members is done by
                       AEEApplet_New().

返回值:

备注:

==============================================================================*/
static int ScheduleApp_CreateInstance( AEECLSID    ClsId,
                                       IShell     *pIShell,
                                       IModule    *po,
                                       void      **ppObj)
{
    if (ClsId == AEECLSID_SCHEDULEAPP)
    {
        return ScheduleApp_New(pIShell, po, ppObj);
    }
    else
    {
        //debug("****************FAILUE TO CREATE SCHEDULEAPP",0,0,0);
        return EUNSUPPORTED;
    }
} // ScheduleApp_CreateInstance

/*==============================================================================
函数:
       ScheduleApp_New

说明:
        创建对象实例。

参数:
       ps [in]：IShell接口对象指针。
       po [in]：pIModule接口对象指针。
       ppObj [out]:指向IScheduleApp对象指针的指针。

返回值:
       SUCCESS：成功。

备注:

==============================================================================*/
static int ScheduleApp_New(IShell * pIShell, IModule * po, void ** ppObj)
{
    CScheduleApp *pme = NULL;
    int retVal = SUCCESS;

    if ((NULL == ppObj) || (NULL == pIShell) || (NULL == po))
    {
        return EFAILED;
    }

#if defined(AEE_STATIC)
    pme = &gScheduleApp;
#else

    pme = (CScheduleApp *)MALLOC(sizeof(CScheduleApp) + sizeof(VTBL(IScheduleApp)));
    if (NULL == pme)
    {
        return EFAILED;
    }
#endif
    if (0 == pme->m_nRefs)
    {
#if defined(AEE_STATIC)
        // Must initialize the object
        MEMSET(pme,  0, sizeof(CScheduleApp));
        INIT_VTBL(pme, IScheduleApp, gScheduleAppMethods);
#else
        //NOTE: In the dynamic applet, the extend classes will re-malloc the
        //      memery everytime in startapplet. So it run in simulator for
        //      test only. If you want run in simulator correcttly please
        //      modify the following code.
        VTBL(IScheduleApp) *pMod = (VTBL(IScheduleApp)*)((byte *)pme +sizeof(CScheduleApp));
        pMod->AddRef      = IScheduleApp_AddRef;
        pMod->Release     = IScheduleApp_Release;
        pMod->HandleEvent = IScheduleApp_HandleEvent;
        INIT_VTBL(pme, IScheduleApp, *pMod);
#endif

        pme->m_nRefs    = 0;
        pme->m_pShell   = pIShell;
        pme->m_pModule  = po;

        (void) ISHELL_AddRef(pme->m_pShell);

        (void) IMODULE_AddRef(pme->m_pModule);

        retVal = CScheduleApp_InitAppData(pme);

        if( SUCCESS != retVal )
        {
            CScheduleApp_FreeAppData(pme);
            return retVal;
        }
    }

    ++pme->m_nRefs;
    *ppObj = (IScheduleApp*)pme;
    return retVal;
} // ScheduleApp_Load

/*==============================================================================
函数:
       IScheduleApp_AddRef

说明:
       ScheduleApp Applet 引用计数加1，并返回当前的引用数。

参数:
       pi [in]：IScheduleApp接口对象指针。

返回值:
       当前对ScheduleApp Applet的引用数

备注:

==============================================================================*/
static uint32  IScheduleApp_AddRef(IScheduleApp *pi)
{
    CScheduleApp *pme = (CScheduleApp*)pi;
    //debug( ";************IScheduleApp_AddRef, %d**********", pme->m_nRefs + 1);
    return (++pme->m_nRefs);
} // IScheduleApp_AddRef

/*==============================================================================
函数:
       IScheduleApp_Release

说明:
       释放对ScheduleApp Applet的引用，引用计数减1，并返回当前的引用数。
        如果当前的引用数为零，还将释放Applet及相关资源。

参数:
       pi [in]：IScheduleApp接口对象指针。

返回值:
       当前对ScheduleApp Applet的引用数

备注:

==============================================================================*/
static uint32  IScheduleApp_Release (IScheduleApp *pi)
{
    CScheduleApp *pme = (CScheduleApp*)pi;
    if (pme->m_nRefs == 0)
    {
        //debug( ";********IScheduleApp_Release, m_nRefs = 0, return");
        return 0;
    }

    //当m_nRefs为1时，(--pme->m_nRefs)为0
    if (--pme->m_nRefs)
    {
        //debug( ";********IScheduleApp_Release, m_nRefs = %d, return", pme->m_nRefs);
        return pme->m_nRefs;
    }

    //debug( ";********IScheduleApp_Release, m_nRefs = %d, to free data", pme->m_nRefs);

    // 释放Applet相关资源
    CScheduleApp_FreeAppData(pme);

    //debug( ";********IScheduleApp_Release, after CScheduleApp_FreeAppData");
    (void) ISHELL_Release(pme->m_pShell);
    (void) IMODULE_Release(pme->m_pModule);

    // 注意：如果pMe是静态分配空间，则不需要释放。FREE()
#if !defined(AEE_STATIC)
    FREE_VTBL(pme, IScheduleApp);
    FREE(pme);
#endif

    return 0;
} // IScheduleApp_Release

/*==============================================================================
函数:
       CScheduleApp_InitAppData

说明:
       初始化ScheduleApp Applet结构数据缺省值。

参数:
       pme [in]：指向ScheduleApp Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
       SUCCESS：初始化成功。
       EFAILED：初始化失败。

备注:

==============================================================================*/
static int CScheduleApp_InitAppData(CScheduleApp *pme)
{

    pme->m_ePreState           = STATE_NONE;
    pme->m_eCurState           = STATE_INIT;
    pme->m_pActiveDlg          = NULL;
    pme->m_eDlgRet             = DLGRET_CREATE;
    pme->m_bNotOverwriteDlgRet = FALSE;

	pme->m_bInactive         = FALSE;
    pme->m_strPhonePWD = NULL;

    pme->m_bshowEventDetail = FALSE;
    pme->m_bRepaint = FALSE; //modify for phone die in display loading menu when incoming call
    pme->m_bDeferred = TRUE; //modify for phone die in display loading menu when incoming call

    //pme->m_pMenu = NULL;
    // Create IDisplay instance
    if (SUCCESS != ISHELL_CreateInstance( pme->m_pShell,
                                          AEECLSID_DISPLAY,
                                          (void **)&pme->m_pDisplay))
    {
        return EFAILED;
    }
	if (AEE_SUCCESS != ISHELL_CreateInstance(pme->m_pShell,AEECLSID_ANNUNCIATOR,(void **)&pme->m_pIAnn))
    {
        return EFAILED;
    }
    IANNUNCIATOR_SetFieldIsActiveEx(pme->m_pIAnn,FALSE);
#if FEATURE_DRAW_LUNAR_CALENDAR
    if(Calendar_InitAppData(&pme->m_calendar, pme->m_pShell)!=TRUE)
    {
        return EFAILED;
    }
#endif

    if(CalMgr_InitAppData(&pme->m_CalMgr, pme->m_pShell, pme->m_pDisplay)!=TRUE)
    {

        return EFAILED;
    }

    if (AEE_SUCCESS != ISHELL_CreateInstance(pme->m_pShell,
                                 AEECLSID_SOUND,
                                 (void **) &pme->m_pSound))
    {
        return EFAILED;
    }

    //debug( ";****************CScheduleApp_InitAppData*******************");
    return SUCCESS;
} // CScheduleApp_InitAppData

/*==============================================================================
函数:
       CScheduleApp_FreeAppData

说明:
       释放ScheduleApp Applet使用的相关资源。

参数:
       pme [in]：指向ScheduleApp Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
       无。

备注:

==============================================================================*/
static void CScheduleApp_FreeAppData(CScheduleApp *pme)
{

//    if (pme->m_pDisplay != NULL)
//    {
//        (void) IDISPLAY_Release(pme->m_pDisplay);
//        pme->m_pDisplay = NULL;
//    }
    FREEIF(pme->m_strPhonePWD);
#if FEATURE_DRAW_LUNAR_CALENDAR
    Calendar_FreeAppData(&pme->m_calendar);
#endif
    if (pme->m_pDisplay != NULL)
    {
        IDISPLAY_Release(pme->m_pDisplay);
        pme->m_pDisplay = NULL;
    }

    if(pme->m_pSound != NULL)
    {
        (void) ISOUND_Release(pme->m_pSound);
        pme->m_pSound = NULL;
    }
	if(pme->m_pIAnn)
    {
        IANNUNCIATOR_Release(pme->m_pIAnn);
        pme->m_pIAnn = NULL;
    }

    //debug( ";********CScheduleApp_FreeAppData, to call CalMgr_FreeAppData");
    CalMgr_FreeAppData(&pme->m_CalMgr);
} // CScheduleApp_FreeAppData

/*==============================================================================
函数:
       CScheduleApp_RunFSM

说明:
       ScheduleApp Applet有限状态机引擎。

参数:
       pme [in]：指向ScheduleApp Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
       无。

备注:

==============================================================================*/
static void CScheduleApp_RunFSM(CScheduleApp *pme)
{
    NextFSMAction nextFSMAction = NFSMACTION_WAIT;
    for ( ; ; )
    {
        nextFSMAction = CScheduleApp_ProcessState(pme);

        if (pme->m_bNotOverwriteDlgRet)
        {
            pme->m_bNotOverwriteDlgRet = FALSE;
        }
        else
        {
            pme->m_eDlgRet = DLGRET_CREATE;
        }

        if (nextFSMAction == NFSMACTION_WAIT)
        {
            break;
        }
    }
} // CScheduleApp_RunFSM

/*==============================================================================
函数:
       IScheduleApp_HandleEvent

说明:
       ScheduleApp  Applet事件处理主函数。所有属于本Applet的事件，
       都将经由该函数处理。

参数:
       pi [in]：指向IScheduleApp接口t对象的指针。
       eCode：BREW事件代码。
       wParam：与事件相关联的数据。
       dwParam [in]：与事件相关联的数据。

返回值:
       TRUE：传入事件被处理。
       FALSE：传入事件没被处理。

备注:

==============================================================================*/
#ifdef FEATURE_APP_RECORDER
        extern boolean recorder_GetRunningFlag(void);
#endif

static boolean  IScheduleApp_HandleEvent( IScheduleApp   *pi,
                                          AEEEvent       eCode,
                                          uint16         wParam,
                                          uint32         dwParam)
{
    CScheduleApp *pme = (CScheduleApp*)pi;
    AEEAppStart *as;
    uint32   lDateTime;
    extern uint32 getNowCompositeTime( void);
	
    switch (eCode)
    {
        case EVT_APP_START:
			if(OEM_IME_DIALOG == wParam)
			{
				return TRUE;
			}
            pme->m_bAppIsReady = FALSE;
            as = (AEEAppStart*)dwParam;
            pme->m_rc = as->rc;
            {
                AEEDeviceInfo di;
                ISHELL_GetDeviceInfo(pme->m_pShell, &di);
                pme->m_rc.dy = di.cyScreen;
            }
            pme->m_bInactive = FALSE;
            pme->m_bSuspended   = FALSE;

            if( as->pszArgs != NULL && STRLEN( as->pszArgs) > 0)
            {
                pme->m_AppStartArgs = as->pszArgs[0];
                pme->m_sports = TRUE;
            }
            else
            {
                pme->m_AppStartArgs = 0;
                pme->m_sports = FALSE;
            }
            // Do not allow the app to start if there is no valid timestamp...

            if (ISHELL_GetSeconds(pme->m_pShell) < ONE_YEAR_SECS)
            {
                
                AEEAppStart * pas = (AEEAppStart *)dwParam;
                pas->error = EINVALIDTIME;
                return FALSE;
            }

            lDateTime = getNowCompositeTime();

            pme->m_CalMgr.m_lCurrentDay = GETDATE(lDateTime);

            pme->m_CalMgr.m_lToday = pme->m_CalMgr.m_lCurrentDay;
            if(pme->m_pIAnn != NULL)
            {
                IANNUNCIATOR_SetFieldIsActiveEx(pme->m_pIAnn,FALSE);
            }
            if(pme->m_CalMgr.m_wPendingAlarmPermID != EMPTY_ALARM_ID)
            {
                pme->m_eCurState = STATE_SHOWALERT;
                pme->m_eDlgRet = DLGRET_CREATE;
                (void)CScheduleApp_ProcessState(pme);
            }
            else
            {
                // 开始ScheduleApp状态机
                CScheduleApp_RunFSM(pme);
            }
            pme->m_bStartAppletCommandQueued = FALSE;
            return TRUE;

        case EVT_APP_STOP:
            //debug( ";***************EVT_APP_STOP");
            pme->m_bInactive = TRUE;
            ISHELL_CancelTimer( pme->m_pShell, 0, pme);
            return TRUE;

        case EVT_APP_SUSPEND:
            pme->m_bInactive = TRUE;
            pme->m_bSuspended   = TRUE;
            ISHELL_CancelTimer( pme->m_pShell, 0, pme);
            //closeDatabaseIf( &pme->m_CalMgr);

//          if (pme->m_CalMgr.m_alert != NULL)
//          {
//              IALERT_Release(pme->m_CalMgr.m_alert);
//              pme->m_CalMgr.m_alert = NULL;
//          }
            return TRUE;

        case EVT_APP_RESUME:
            as = (AEEAppStart*)dwParam;
            pme->m_rc = as->rc;
            {
                AEEDeviceInfo di;
                ISHELL_GetDeviceInfo(pme->m_pShell, &di);
                pme->m_rc.dy = di.cyScreen;
            }

            if (IDD_SHOWALERT == pme->m_pActiveDlgID)
            {
                pme->m_CalMgr.m_wPendingAlarmPermID = pme->ALERTID;
            }

            switch( pme->m_eCurState)
            {
                case STATE_OPTION:
                {
                    FSMState moveTo[] = { STATE_VIEWMONTH, STATE_VIEWDAY};
                    MOVE_TO_STATE( moveTo[pme->m_subStateOption - SUBSTATE_OPTION_VIEWMONTH])
                }
                break;
            }

            pme->m_bSuspended   = FALSE;
            if(pme->m_bRepaint)
            {
                pme->m_bRepaint = FALSE;
                repaint(pme, pme->m_bDeferred);
            }
            CScheduleApp_RunFSM(pme);
            return TRUE;

        case EVT_DIALOG_INIT:
            if (wParam == OEM_IME_DIALOG)
            {
                return TRUE;
            }

            //debug( ";*************EVT_DIALOG_INIT");
            pme->m_bAppIsReady = FALSE;
            // 更新对话框控制参数信息！！！
            pme->m_pActiveDlg = (IDialog*)dwParam;
            pme->m_pActiveDlgID = wParam;
            return CScheduleApp_RouteDialogEvent(pme,eCode,wParam,dwParam);

        case EVT_DIALOG_START:
            if (wParam == OEM_IME_DIALOG)
            {
                return TRUE;
            }
            
            (void)ISHELL_SetTimer (pme->m_pShell,
                                   APPISREADY_TIMER,
                                   Schedule_APPIsReadyTimer,
                                   pme);
            
            //debug( ";*************EVT_DIALOG_START");
            
            return CScheduleApp_RouteDialogEvent(pme,eCode,wParam,dwParam);

        case EVT_DIALOG_END:
            if (wParam == OEM_IME_DIALOG)
            {
                // For redraw the dialog
                (void)ISHELL_PostEvent(pme->m_pShell,
                                       AEECLSID_SCHEDULEAPP,
                                       EVT_USER_REDRAW,
                                       0,
                                       0);
                return TRUE;
            }

            if (wParam == 0)
            {
                return TRUE;
            }

            pme->m_bAppIsReady = FALSE;

            (void) CScheduleApp_RouteDialogEvent(pme,eCode,wParam,dwParam);

            //debug( ";***************EVT_DIALOG_END, pme->m_bSuspending = %d", pme->m_bSuspending);
            // Applet被挂起，不跑状态机，等待Applet返回。（注意：EVT_APP_SUSPEND
            // 事件在EVT_DIALOG_END事件前发出。
            if (pme->m_bInactive == FALSE)
            {
                if(pme->m_CalMgr.m_wPendingAlarmPermID != EMPTY_ALARM_ID)
                {
                    pme->m_eCurState = STATE_SHOWALERT;
                    pme->m_eDlgRet = DLGRET_CREATE;
                    (void)CScheduleApp_ProcessState(pme);
                }
                else
                {
                    // 开始ScheduleApp状态机
                    CScheduleApp_RunFSM(pme);
                }
            }
            return TRUE;

        case EVT_ALARM:
            pme->ALERTID = wParam;
#ifdef FEATURE_SUPPORT_VC0848           
            switch(VC_GetCurrentDevice())
            {
                case VC_DEV_CAMERA:
                    ISHELL_SendEvent(pme->m_pShell, AEECLSID_APP_CAMERA, EVT_ALARM, 0, 0);
                    break;
     
                default:
                    break;                      
            }
#endif
            if(Schedule_CanAlert(pme))
            {
                Cal_HandleAlarm(pme, wParam);
            }
            else
            {
                ISOUND_Vibrate(pme->m_pSound, 2000);

                if(AEECLSID_DIALER == ISHELL_ActiveApplet(pme->m_pShell))
                {
                    ISHELL_PostEvent(pme->m_pShell, AEECLSID_DIALER, EVT_ALARM, 0, 0);
                }

#ifdef FEATURE_APP_RECORDER
                if(TRUE == recorder_GetRunningFlag())
                {
                     ISHELL_SendEvent(pme->m_pShell, AEECLSID_RECORDER, EVT_ALARM, 0, 0);
                }
                
#endif /*FEATURE_APP_RECORDER*/
            }
            return TRUE;

        case EVT_USER_REDRAW:
            if(pme->m_bSuspended && pme->m_bRepaint)
            {
                return TRUE;
            }
            (void) CScheduleApp_RouteDialogEvent(pme,eCode,wParam,dwParam);
            /*
            (void)ISHELL_SetTimer (pme->m_pShell,
                                   APPISREADY_TIMER,
                                   Schedule_APPIsReadyTimer,
                                   pme);
            */
            return TRUE;

        case EVT_APPISREADY:
            (void) CScheduleApp_RouteDialogEvent(pme,eCode,wParam,dwParam);
            pme->m_bAppIsReady = TRUE;
            pme->m_bInactive = FALSE;
            pme->m_bSuspended   = FALSE;
            return TRUE;

        case EVT_KEY:
        case EVT_KEY_PRESS:
            //debug( ";---------------EVT_KEY");
            #if !defined(FEATURE_VERSION_K212) && !defined(FEATURE_VERSION_EC99)&&!defined(FEATURE_QVGA_INHERIT_K212)&&!defined(FEATURE_VERSION_K212_HUALU)&&!defined(FEATURE_VERSION_IN50A)
            if (!pme->m_bAppIsReady)
            {
                return TRUE;
            }
			#endif
            if( eCode == EVT_KEY_PRESS && wParam == AVK_END)
            {
                pme->m_closeByPowerKey = TRUE;
                ISHELL_CloseApplet( pme->m_pShell, TRUE);
                return TRUE;
            }
            return CScheduleApp_RouteDialogEvent(pme,eCode,wParam,dwParam);


        case EVT_COMMAND:
			#if !defined(FEATURE_VERSION_K212) && !defined(FEATURE_VERSION_EC99)&&!defined(FEATURE_QVGA_INHERIT_K212)&&!defined(FEATURE_VERSION_K212_HUALU)&&!defined(FEATURE_VERSION_IN50A)
            if (!pme->m_bAppIsReady)
            {
                return TRUE;
            }
			#endif
            return CScheduleApp_RouteDialogEvent(pme,eCode,wParam,dwParam);

#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
		case EVT_PEN_UP:
		//case EVT_PEN_DOWN: 
			return CScheduleApp_RouteDialogEvent(pme,eCode,wParam,dwParam);
		case EVT_USER:
		{
			if (wParam == AVK_SELECT)
			{
				if (dwParam== 0)
				{
					eCode = EVT_KEY;
				}
				else if (dwParam==NEVT_KEY_PRESSS)
				{
					eCode = EVT_KEY_PRESS;
				}
				else
				{
					eCode = EVT_COMMAND;
				}
			}
			else if (wParam == AVK_INFO)
			{
				eCode = EVT_KEY;
			}
			else if (wParam == AVK_CLR)
			{
				if (dwParam == NEVT_KEY_PRESSS)
				{
					eCode = EVT_KEY_PRESS;
				}
				else
				{
					eCode = EVT_KEY;
				}
			}
			else if (wParam == AVK_LEFT)
			{
				eCode = EVT_KEY_PRESS;
			}
			else if (wParam == AVK_RIGHT)
			{
				eCode = EVT_KEY_PRESS;
			}
			return CScheduleApp_RouteDialogEvent(pme,eCode,wParam,dwParam);
		}
		return TRUE;
#endif

        default:
            // 将接收到的事件路由至当前活动的对话框事件处理函数。
            return CScheduleApp_RouteDialogEvent(pme,eCode,wParam,dwParam);
    }
} // IScheduleApp_HandleEvent



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
static void Schedule_APPIsReadyTimer(void *pi)
{
    CScheduleApp *pme = (CScheduleApp*)pi;

    if (NULL == pme)
    {
        return;
    }

    (void) ISHELL_PostEvent( pme->m_pShell,
                            AEECLSID_SCHEDULEAPP,
                            EVT_APPISREADY,
                            0,
                            0);
}

/*==============================================================================
函数 :

    IScheduleApp_SportsPlan

说明：
      从sports模块进入日程表添加 soprts plans

参数：
       pi [in/out]：CScheduleApp接口对象指针。

返回值：


备注：:

==============================================================================*/
static void  IScheduleApp_SportsPlan (IScheduleApp *pi)
{
    CScheduleApp *pme = (CScheduleApp*)pi;
    char     args[20] = {0};
    //char      buf[20];
    
    if (NULL == pme)
    {
        return;
    } 
    
    SPRINTF(args, "%c", 
        STARTARG_SPORT);
    
    //args = STRDUP(buf);  
    
    (void)ISHELL_StartAppletArgs(pme->m_pShell,
                           AEECLSID_SCHEDULEAPP,
                           args);
    //FREEIF(args);

}

/*==============================================================================
函数：
       Schedule_CanAlert
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
static boolean Schedule_CanAlert(CScheduleApp *pme)
{    
    ICM *pICM = NULL;
    uint16 num = 0;
    if(AEE_SUCCESS != ISHELL_CreateInstance(pme->m_pShell, 
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
#ifdef FEATURE_SUPPORT_VC0848
    if(DEVICE_CAMERA_STATE_ON == GetDeviceState(DEVICE_TYPE_CAMERA))
    {       
        return FALSE;
    }

    if(DEVICE_MP4_STATE_ON == GetDeviceState(DEVICE_TYPE_MP4))
    {       
        return FALSE;
    }
#endif
#ifdef FEATURE_APP_RECORDER
    if(TRUE == recorder_GetRunningFlag())
    {
            return FALSE;
    }
    
#endif /*FEATURE_APP_RECORDER*/

    
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
       IScheduleApp_DeleteAllEntry
说明：
       删除所有的日程表db文件

参数：
       pi [in/out]：CScheduleApp接口对象指针。

返回值：
        NONE
备注：

==============================================================================*/
static int  IScheduleApp_DeleteAllEntry(IScheduleApp *pi)
{
    CScheduleApp *pme = (CScheduleApp*)pi;
    if (NULL == pme)
    {
        return EFAILED;
    } 
    
    deleteAllEventsFromDB(&(pme->m_CalMgr));
    return SUCCESS;
}

void repaint(CScheduleApp *pme, boolean deferred)  
{
    if(deferred)
    {
        postEvent( EVT_USER_REDRAW);
    }
    else
    {
        sendEvent( EVT_USER_REDRAW);
    }
    pme->m_bDeferred = deferred;
    pme->m_bRepaint = TRUE;
}


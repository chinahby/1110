#ifndef _SPORT_APP_C_
#define _SPORT_APP_C_
/*
********************************************************************************

模块名       : <SportApplication>
文件名       : <SportSApp.c>
相关文件     : 
文件实现功能 : Sports Application
作者         : 
版本         : 1.0
--------------------------------------------------------------------------------
备注         : 
--------------------------------------------------------------------------------
修改记录 : 

******************************************************************************/
#include "SportSApp_priv.h"



/*----------------------模块相关函数声明---------------------*/
int SportsMod_Load(IShell *pIShell,void *ph,IModule **ppMod);
int SportsMod_New(int16  nSize,IShell *pIShell,void *ph,IModule  **ppMod,PFNMODCREATEINST pfnMC,
    				PFNFREEMODDATA pfnMF);
static int SportsMod_CreateInstance(IModule *po,IShell *pIShell,AEECLSID ClsId,void **ppObj);
static uint32 SportsMod_AddRef(IModule *po);
static uint32 SportsMod_Release(IModule *po);
static void SportsMod_FreeResources(IModule  *po,IHeap  *ph,IFileMgr *pfm);

/*----------------------Applet相关函数声明---------------------*/
static int SportSApp_New(IShell *ps,IModule *pIModule,ISportsMenu **ppObj);
static uint32 SportSApp_AddRef(ISportsMenu *pi);
static uint32 SportSApp_Release (ISportsMenu *pi);
static boolean SportSApp_HandleEvent(ISportsMenu *pi,AEEEvent eCode,uint16  wParam,uint32 dwParam);

static void SportSApp_APPIsReadyTimer(void *pme);
static int SportSApp_InitAppData(CSportsMenu *pMe);
static void SportSApp_FreeAppData(CSportsMenu *pMe);
static void SportSApp_RunFSM(CSportsMenu *pMe);

#ifdef AEE_SIMULATOR
   uint32 g_dwAEEStdLibEntry;
#ifndef WIN32
   #error This applet was never intended to be used with the AEE Simulator.
#endif//WIN32
#endif

extern int Rendering_UpdateEx(void);//wlh 20090409 add
/*-------------------------------------------------------------------
            Static variable Definitions
-------------------------------------------------------------------*/
static SportsMenuMod gSportsMenuMod;

static const VTBL(IModule) gModFuncs =
{
    SportsMod_AddRef,
    SportsMod_Release,
    SportsMod_CreateInstance,
    SportsMod_FreeResources
};

// 只允许一个SettingMenu实例。每次创建SettingMenu Applet时，
// 返回同一结构指针给BREW层。
static CSportsMenu gSportsMenu;

static const VTBL(ISportsMenu) gSportsMenuMethods =
{
    SportSApp_AddRef,
    SportSApp_Release,
    SportSApp_HandleEvent
};

/* =========================================================================
                     FUNCTION DEFINITIONS
   ========================================================================= */

/*----------------------模块相关函数定义---------------------*/

/*==============================================================================
函数:
       SportsMod_Load

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
#ifndef AEE_SIMULATOR
int SportsMod_Load(IShell *pIShell,void *ph,IModule **ppMod)
#else
__declspec(dllexport) int AEEMod_Load(IShell *pIShell,void *ph,IModule **ppMod)

#endif
#else
int SportsMod_Load(IShell *pIShell,void *ph,IModule **ppMod)
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
    return SportsMod_New(sizeof(SportsMenuMod),pIShell,ph,ppMod,NULL,NULL);
}

/*==============================================================================
函数:
       SportsMod_New

说明:
       用来初始化SettingMenu模块虚拟表（也增加引用计数）的助手函数。

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
int  SportsMod_New(int16 nSize,IShell *pIShell,void *ph,IModule **ppMod,
 					PFNMODCREATEINST pfnMC,PFNFREEMODDATA pfnMF)
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
    MEMSET(&gSportsMenuMod,0,sizeof(SportsMenuMod));
    INIT_VTBL(&gSportsMenuMod,IModule,gModFuncs);
    gSportsMenuMod.m_nRefs = 1;
    *ppMod = (IModule *)&gSportsMenuMod;
    return AEE_SUCCESS;
}

/*==============================================================================
函数:
       SportsMod_AddRef

说明:
       Sports 模块引用计数加1，并返回当前的引用数。

参数:
       po [in]：IModule接口对象指针。

返回值:
       当前对Sports 模块的引用数

备注:

==============================================================================*/
static uint32 SportsMod_AddRef(IModule *po)
{
    return(++((SportsMenuMod *)po)->m_nRefs);
}

/*==============================================================================
函数:
       SportsMod_Release

说明:
       Sports 模块引用计数减1，并返回当前的引用数。

参数:
       po [in]：IModule接口对象指针。

返回值:
       当前对Sports 模块的引用数

备注:

==============================================================================*/
static uint32 SportsMod_Release(IModule *po)
{
    if (((SportsMenuMod *) po)->m_nRefs == 0)
    {
        return 0;
    }

    return(--((SportsMenuMod *)po)->m_nRefs);
}

/*==============================================================================
函数:
       SportsMod_CreateInstance

说明:
        通过创建(初始化)Sports对象来创建模块实例。

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
static int  SportsMod_CreateInstance(IModule *po,
    IShell *pIShell,
    AEECLSID ClsId,
    void **ppObj
)
{
    *ppObj = NULL;

    if (ClsId != AEECLSID_SPORTSAPP)
    {
        return EFAILED;
    }

    if (SportSApp_New(pIShell, po, (ISportsMenu**)ppObj) != SUCCESS)
    {
        return EFAILED;
    }
    return SUCCESS;
}

/*==============================================================================
函数:
       SportsMod_FreeResources

说明:
        释放Sports模块资源的函数。

参数:
       po ：IModule。
       ph ：IHeap接口指针。
       pfm :IFileMgr接口指针。

返回值:
       无

备注:
       不需用户去调用。

==============================================================================*/
static void SportsMod_FreeResources(IModule * po,IHeap * ph,IFileMgr * pfm)
{

    PARAM_NOT_REF(po)
    PARAM_NOT_REF(ph)
    PARAM_NOT_REF(pfm)
}

/*----------------------Applet相关函数定义---------------------*/
/*==============================================================================
函数:
       SportSApp_New

说明:
        初始化SportSApp Applet虚拟表和内部数据变量，同时其引用计数加1。

参数:
       ps [in]：IShell接口对象指针。
       pIModule [in]：pIModule接口对象指针。
       ppObj [out]:：指向ISportsMenu对象指针的指针。

返回值:
       EFAILED：失败。
       SUCCESS：成功。

备注:
       不需用户去调用。
       关于Applet结构对象可使用动态分配内存的方式创建，本处采用的是使用静态
       变量的方式。建议对结构比较大的Applet使用静态变量的方式。对于动态分配
       内存的方式需要注意对所分配空间的释放。

==============================================================================*/
static int SportSApp_New(IShell *ps,IModule *pIModule,ISportsMenu **ppObj)
{
    int retVal = SUCCESS;
    if (0 == gSportsMenu.m_nRefs)
    {
        // Must initialize the object
        MEMSET(&gSportsMenu,  0, sizeof(CSportsMenu));
        INIT_VTBL(&gSportsMenu, ISportsMenu, gSportsMenuMethods);

        gSportsMenu.m_nRefs     = 0;
        gSportsMenu.m_pShell    = ps;
        gSportsMenu.m_pModule   = pIModule;

        (void) ISHELL_AddRef(ps);
        (void) IMODULE_AddRef(pIModule);

        retVal = SportSApp_InitAppData(&gSportsMenu);
    }

    ++gSportsMenu.m_nRefs;
    *ppObj = (ISportsMenu*)&gSportsMenu;
    return retVal;
}

/*==============================================================================
函数:
       SportSApp_AddRef

说明:
       Sports Applet 引用计数加1，并返回当前的引用数。

参数:
       pi [in]：SportsApp接口对象指针。

返回值:
       当前对SportsApp Applet的引用数

备注:

==============================================================================*/
static uint32  SportSApp_AddRef(ISportsMenu *pi)
{
    register CSportsMenu *pMe = (CSportsMenu*)pi;

    ASSERT(pMe != NULL);
    ASSERT(pMe->m_nRefs > 0);

    return (++pMe->m_nRefs);
}

/*==============================================================================
函数:
       SportSApp_Release

说明:
       释放对SportsApp Applet的引用，引用计数减1，并返回当前的引用数。
        如果当前的引用数为零，还将释放Applet及相关资源。

参数:
       pi [in]：ISportsMenu接口对象指针。

返回值:
       当前对ISportsApp Applet的引用数

备注:

==============================================================================*/
static uint32  SportSApp_Release (ISportsMenu *pi)
{
    register CSportsMenu *pMe = (CSportsMenu*)pi;

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
    SportSApp_FreeAppData(pMe);

    (void) ISHELL_Release(pMe->m_pShell);
    (void) IMODULE_Release(pMe->m_pModule);

    // 注意：pMe是静态分配空间，因此不需要释放。FREE()
    return 0;
}

/*==============================================================================
函数:
       SportSApp_InitAppData

说明:
       初始化SportsMenu Applet结构数据缺省值。

参数:
       pMe [in]：指向CSportsMenu Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
       SUCCESS：初始化成功。
       EFAILED：初始化失败。

备注:

==============================================================================*/
static int SportSApp_InitAppData(CSportsMenu *pMe)
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
	pMe->m_ePreState = SPORT_STATE_NONE;
	pMe->m_eCurState = SPORT_STATE_INIT;
	pMe->m_pActiveDlg = NULL;
	pMe->m_eDlgRet = DLGRET_CREATE;
	pMe->m_bNotOverwriteDlgRet = FALSE;
	pMe->m_currDlgId = 0;
	pMe->m_nSubDlgId = 0;
	/* background*/
	pMe->runOnBackgrounds =FALSE;
	pMe->GobackIdleFlag = FALSE;
	/*background end */
	/*Init Pedometer Data About Pedometer and UnitData */
	InitPedoMeter(pMe);
	
    if(AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,AEECLSID_LISTCTL,
	(void **)&pMe->m_pSportsAppMenu.m_pSAppMenu))
	{
		SportSApp_FreeAppData(pMe);
	    return FALSE;
	}

	if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_STOPWATCHCTL, 
		(void **) &pMe->m_pSportsAppMenu.m_pSAppTime))
	{
	    SportSApp_FreeAppData(pMe);
	    return FALSE;
	}

	if(AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_CONFIG,
		(void **) &pMe->m_pConfig))
	{
		SportSApp_FreeAppData(pMe);
		return FALSE;
	}

	if ( SUCCESS != ISHELL_CreateInstance( pMe->m_pShell,
	                                   AEECLSID_FILEMGR,
	                                   (void **)&pMe->m_pSportsAppMenu.m_pFileMgr))
	{
	    return EFAILED;
	}

       /*small title icon create resource*/
        if (ISHELL_CreateInstance(pMe->m_pShell,
                                     AEECLSID_ANNUNCIATOR,
                                     (void **) &pMe->m_pIAnn))
        {
            return FALSE;
        }    
    return SUCCESS;
}

/*==============================================================================
函数:
       SportSApp_FreeAppData

说明:
       释放SettingMenu Applet使用的相关资源。

参数:
       pMe [in]：指向SettingMenu Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
       无。

备注:

==============================================================================*/
static void SportSApp_FreeAppData(CSportsMenu *pMe)
{
    if (NULL == pMe)
    {
        return;
    }

    if (pMe->m_pSportsAppMenu.m_pSAppMenu != NULL)
	{
       IMENUCTL_Release(pMe->m_pSportsAppMenu.m_pSAppMenu);
       pMe->m_pSportsAppMenu.m_pSAppMenu = NULL;
    }

    if (pMe->m_pSportsAppMenu.m_pSAppTime != NULL) 
	{
       ITIMECTL_Release(pMe->m_pSportsAppMenu.m_pSAppTime);
       pMe->m_pSportsAppMenu.m_pSAppTime = NULL;
    }

    if (pMe->m_pConfig!= NULL) 
    	{
    		ICONFIG_Release(pMe->m_pConfig);
		pMe->m_pConfig = NULL;
    	}

	/*Release FileMgr handle*/
    if (pMe->m_pSportsAppMenu.m_pFileMgr != NULL)
    {
        (void) IFILEMGR_Release(pMe->m_pSportsAppMenu.m_pFileMgr);
        pMe->m_pSportsAppMenu.m_pFileMgr = NULL;
    }
    /*small title icon freee resource*/
    if (pMe->m_pIAnn)
    {
        IANNUNCIATOR_Release(pMe->m_pIAnn);
        pMe->m_pIAnn= NULL;
    }
	ISHELL_CancelTimer(pMe->m_pShell,NULL,pMe);
}

/*==============================================================================
函数:
       SportSApp_RunFSM

说明:
       SportsMenu Applet有限状态机引擎。

参数:
       pMe [in]：指向SettingMenu Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
       无。

备注:

==============================================================================*/
static void SportSApp_RunFSM(CSportsMenu *pMe)
{
    NextFSMAction nextFSMAction = NFSMACTION_WAIT;

    for ( ; ; )
    {
        nextFSMAction = SportSApp_ProcessState(pMe);

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
       SportSApp_HandleEvent

说明:
       SportsApp  Applet事件处理主函数。所有属于本Applet的事件，
       都将经由该函数处理。

参数:
       pi [in]：指向ISportMenu接口t对象的指针。
       eCode：BREW事件代码。
       wParam：与事件相关联的数据。
       dwParam [in]：与事件相关联的数据。

返回值:
       TRUE：传入事件被处理。
       FALSE：传入事件没被处理。

备注:

==============================================================================*/
extern boolean g_sportsapp_pedometer_bground_flag;
#ifdef FEATURE_SPORTS_APP
extern boolean g_gobacktosportflag;
#endif

static boolean SportSApp_HandleEvent(ISportsMenu *pi,
    AEEEvent eCode,
    uint16  wParam,
    uint32 dwParam
)
{
    CSportsMenu *pMe = (CSportsMenu*)pi;
    AEEAppStart *as;
	
    SPORTS_ERR("%x, %x ,%x,SettingMenu_HandleEvent",eCode,wParam,dwParam);
	
    switch (eCode)
    {
        case EVT_APP_START:
			Rendering_UpdateEx();//wlh add for 3D test
            /*event application start*/
            as = (AEEAppStart*)dwParam;

            #ifdef FEATURE_SPORTS_APP
            if(as->pszArgs != NULL)
            {
                uint16 bytNewData;                
                DBGPRINTF("as->pszArgs != NULL ");
                
                (void)ICONFIG_GetItem(pMe->m_pConfig,
                CFGI_SPORT_FLAG,
                &bytNewData, sizeof(bytNewData));

                if(bytNewData >= 3)
                {
                    pMe->m_pSportsAppMenu.m_nSelProFile = 0;
                }
                else
                {
                    pMe->m_pSportsAppMenu.m_nSelProFile = bytNewData;
                }
                /*Get Sport Data From File System*/
                SportSApp_LoadSportData(pMe);
                
                pMe->FromScheduleFlag = TRUE;
                pMe->m_eCurState = SPORT_STATE_PETOMETER;
                pMe->m_eDlgRet = DLGRET_CREATE;
                //return TRUE;
            }
            #endif
            
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
            pMe->m_rc.y = pMe->m_rc.y - SPORTS_MENU_SOFK_HIGHT;
            pMe->m_bSuspending = FALSE;

            // 开始SportSApp状态机
            SportSApp_RunFSM(pMe);
            return TRUE;

        case EVT_APP_STOP:
		if(!pMe->runOnBackgrounds)
		{
            if (pMe->m_pDisplay != NULL)
            {
                (void) IDISPLAY_Release(pMe->m_pDisplay);
                pMe->m_pDisplay = NULL;
            }
		}
            pMe->m_bSuspending = TRUE;
		*((boolean*)dwParam) = !pMe->runOnBackgrounds;
            return TRUE;

        case EVT_APP_SUSPEND:
            pMe->m_bSuspending = TRUE;

            return TRUE;

        case EVT_APP_RESUME:
            ASSERT(dwParam != 0);
            as = (AEEAppStart*)dwParam;

#ifdef FEATURE_SPORTS_APP

            if(g_gobacktosportflag == TRUE)
            {
                uint16 bytNewData;
                InitPedoMeter(pMe);
                
                (void)ICONFIG_GetItem(pMe->m_pConfig,
                CFGI_SPORT_FLAG,
                &bytNewData, sizeof(bytNewData));

                if(bytNewData >= 3)
                {
                    pMe->m_pSportsAppMenu.m_nSelProFile = 0;
                }
                else
                {
                    pMe->m_pSportsAppMenu.m_nSelProFile = bytNewData;
                }
                /*Get Sport Data From File System*/
                SportSApp_LoadSportData(pMe);		
                
                pMe->FromScheduleFlag = TRUE;
                pMe->m_eCurState = SPORT_STATE_PETOMETER;
                pMe->m_eDlgRet = DLGRET_CREATE;
                g_gobacktosportflag = FALSE;
                //CLOSE_DIALOG(DLGRET_PEDOMETER);
                
                }
            #endif
            pMe->m_bSuspending = FALSE;

            if (pMe->m_pDisplay != NULL)
            {
                (void) IDISPLAY_Release(pMe->m_pDisplay);
                pMe->m_pDisplay = NULL;
            }
            pMe->m_pDisplay = as->pDisplay;
            (void) IDISPLAY_AddRef(pMe->m_pDisplay);
            pMe->m_rc = as->rc;
            pMe->m_rc.y = pMe->m_rc.y - SPORTS_MENU_SOFK_HIGHT;
            SportSApp_RunFSM(pMe);
            return TRUE;

        case EVT_DIALOG_INIT:
			if(OEM_IME_DIALOG == wParam)
			{
				return TRUE;
			}
            pMe->m_bAppIsReady = FALSE;
            pMe->m_pActiveDlg = (IDialog*)dwParam;
            pMe->m_pActiveDlgID = wParam;

            return SportSApp_RouteDialogEvent(pMe,eCode,wParam,dwParam);

        case EVT_DIALOG_START:
			if(OEM_IME_DIALOG == wParam)
			{
				return TRUE;
			}
			Rendering_UpdateEx();//wlh add for 3D test
            return SportSApp_RouteDialogEvent(pMe,eCode,wParam,dwParam);
        case EVT_CLOSEAPP:
        {
            #ifdef FEATRUE_SUPPORT_G_SENSOR
            mmi_g_sensor_process(G_SENSOR_PEDOMETER_DISABLE);
            #endif
            Pedometer_Reset(pMe);
            pMe->m_pSportsAppMenu.m_nEntryRecordState = REC_LAST;
            pMe->runOnBackgrounds = FALSE;
            g_sportsapp_pedometer_bground_flag =  FALSE;  
            IANNUNCIATOR_SetField(pMe->m_pIAnn,ANNUN_FIELD_SPORTS, ANNUN_STATE_SPORTS_OFF);
            /*Switch Fm or Music stop*/
            //SportSapp_SwitchFmAndMusic(pMe); 
            /*entry From Schedule False flag*/
            pMe->FromScheduleFlag = FALSE;    

            ISHELL_CloseApplet( pMe->m_pShell, FALSE);
            return TRUE;
        }
        case EVT_USER_REDRAW:
            (void) SportSApp_RouteDialogEvent(pMe,eCode,wParam,dwParam);
            (void)ISHELL_SetTimer ( pMe->m_pShell,
                            APPISREADY_TIMER,
                            SportSApp_APPIsReadyTimer,
                            pMe);
            return TRUE;

        case EVT_APPISREADY:
            pMe->m_bAppIsReady = TRUE;
            return TRUE;
#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
		case EVT_PEN_UP:
		//case EVT_PEN_DOWN:
			{
				return SportSApp_RouteDialogEvent(pMe,eCode,wParam,dwParam);
			}
#endif
        case EVT_KEY_PRESS:
        case EVT_KEY_RELEASE:
        case EVT_KEY:
        case EVT_COMMAND:
            if (!pMe->m_bAppIsReady)
            {
                return TRUE;
            }
            return SportSApp_RouteDialogEvent(pMe,eCode,wParam,dwParam);

        case EVT_DIALOG_END:
			if(OEM_IME_DIALOG == wParam)
			{
				return ISHELL_PostEvent(pMe->m_pShell,AEECLSID_SPORTSAPP,EVT_USER_REDRAW,0,0);
			}
            if (wParam == 0)
            {
                return TRUE;
            }
            pMe->m_bAppIsReady = FALSE;
            (void) SportSApp_RouteDialogEvent(pMe,eCode,wParam,dwParam);
            pMe->m_pActiveDlg = NULL;

            if (pMe->m_bSuspending == FALSE)
            {
                SportSApp_RunFSM(pMe);
            }
            return TRUE;
#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
		case EVT_USER:
			{
				if (wParam == AVK_SELECT)
				{
					if (dwParam>0)
					{
						eCode = EVT_COMMAND;
						wParam = dwParam;
					}
					else
					{
						eCode = EVT_KEY;
					}
					
				}
				else if (wParam == AVK_INFO)
				{
					eCode = EVT_KEY;
				}
				else if (wParam ==  AVK_CLR)
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
				else if ((wParam == AVK_LEFT)||(wParam == AVK_RIGHT))
				{
					eCode = EVT_KEY_PRESS;
				}
				return  SportSApp_RouteDialogEvent(pMe,eCode,wParam,dwParam);

			}
			return TRUE;
#endif

        default:
            return SportSApp_RouteDialogEvent(pMe,eCode,wParam,dwParam);
    }
}

/*==============================================================================
函数:
       SportSApp_APPIsReadyTimer

说明:
       定时器回调函数。主要用于控制快速按键问题的处理。

参数:
       pme [in]：void *类型指针，暗指SportMenu结构指针。

返回值:
       无。

备注:

==============================================================================*/
static void SportSApp_APPIsReadyTimer(void *pme)
{
    CSportsMenu *pMe = (CSportsMenu *)pme;

    if (NULL == pMe)
    {
        return;
    }

    (void) ISHELL_PostEvent( pMe->m_pShell,AEECLSID_SPORTSAPP,EVT_APPISREADY,0,0);
}

#endif





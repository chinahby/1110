/** ----------------------------------------------------------------------------
 *Application.c
 *
 * Copyright(c) 2007 Anylook
 *
 *
 * History:
 *
 * when         who            what and why
 * -----------  -----------    -----------------------------
 * 2008.12.29            create  3 X 4 large icon style menu  &  list menu
 *
 * -----------------------------------------------------------------------------
 */


#include "customer.h"
#include "OEMFeatures.h"
#include "OEMClassIds.h"
#ifndef AEECLSID_MAIN_MENU
    #error AEECLSID_MAIN_MENU must be defined
#endif
#ifdef FEATURE_SUPPORT_WAP_APP
//#include "WapBrowser.bid"
#endif

#include "AEEShell.h"
#include "AEE_OEM.h"
#include "AEEModGen.h"
#include "AEEModTable.h"
#include "AEEMenu.h"
#include "AEEStdLib.h"
#include "AEEAnnunciator.h"
#include "application.h"
#include "application_priv.h" 

#include "ContApp.h"

#include "Appscommon.h"
#include "appscommon.brh"
#include "appscommonimages.brh"

#if !defined( AEE_SIMULATOR)
#include "AEERUIM.h" 
#include "OEMCFGI.h"
#endif //#if !defined( AEE_SIMULATOR)
#include "err.h"
#define PARAM_NOT_REF(x)

/*==============================================================================

                              本地函数声明

==============================================================================*/
static ApplicationMod gApplicationMod;
static const VTBL( IModule) gModFuncs =
{
    ApplicationMod_AddRef,
    ApplicationMod_Release,
    ApplicationMod_CreateInstance,
    ApplicationMod_FreeResources
};

static uint32  Application_AddRef( IApplication *pi);

static uint32  Application_Release ( IApplication *pi);

static boolean Application_HandleEvent( IApplication *pi,
    AEEEvent eCode,
    uint16  wParam,
    uint32 dwParam
);


static Application gApplication={0};

static const VTBL( IApplication) gApplicationMethods =
{
    Application_AddRef,
    Application_Release,
    Application_HandleEvent
};

int ApplicationMod_Load( IShell *pIShell, void *ph, IModule **ppMod);

int ApplicationMod_New( int16 nSize,
    IShell              *pIShell,
    void                *ph,
    IModule             **ppMod,
    PFNMODCREATEINST    pfnMC,
    PFNFREEMODDATA      pfnMF
);

static int Application_New( IShell *ps, IModule *pIModule, IApplication **ppObj);

static int CApplication_InitAppData(Application *pMe);

static void CApplication_FreeAppData(Application *pMe);

static void Application_RunFSM(Application *pMe);

static int StartApplet(Application *pMe, int i);

boolean Application_RouteDialogEvt(Application *pMe,
    AEEEvent    eCode,
    uint16  wParam,
    uint32 dwParam
);

NextFSMAction Application_ProcessState(Application *pMe);

void Application_ShowDialog(Application  *pMe,  uint16 dlgResId);

// APPLICATIONST_MAIN 状态处理函数
static NextFSMAction APPLICATIONST_MAIN_Handler(Application *pMe);
static NextFSMAction APPLICATIONST_FLASHLIGHT_Handler(Application *pMe);


//APPLICATIONST_EXIT  状态处理函数
static NextFSMAction APPLICATIONST_EXIT_Handler(Application *pMe);
static boolean  Application_ListMenuHandler(Application *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam);
static boolean  Application_FlashlightMenuHandler(Application *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam);



/*==============================================================================
                              
                              本地（静态）数据
                              
==============================================================================*/



//wlh 20090410 add  end 移动效果
/*=============================================================================
FUNCTION:  ApplicationMod_Load

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int ApplicationMod_Load( IShell *pIShell, void *ph, IModule **ppMod)
{
    return ApplicationMod_New( sizeof( ApplicationMod),
                    pIShell,
                    ph,
                    ppMod,
                    NULL,
                    NULL);
}

/*=============================================================================
FUNCTION:  ApplicationMod_New

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int  ApplicationMod_New( int16 nSize,
    IShell   *pIShell,
    void     *ph,
    IModule  **ppMod,
    PFNMODCREATEINST pfnMC,
    PFNFREEMODDATA pfnMF
)
{
#if !defined( AEE_SIMULATOR)
    PARAM_NOT_REF( nSize)
    PARAM_NOT_REF( pfnMC)
    PARAM_NOT_REF( pfnMF)
    PARAM_NOT_REF( ph)
#endif

    if( NULL == ppMod)
    {
        return EFAILED;
    }

    MEMSET( &gApplicationMod, 0, sizeof( ApplicationMod));

    INIT_VTBL( &gApplicationMod, IModule, gModFuncs);
    gApplicationMod.referenceCounter = 1;
    *ppMod = ( IModule *)&gApplicationMod;
    return AEE_SUCCESS;
}

/*=============================================================================
FUNCTION:  ApplicationMod_AddRef

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32 ApplicationMod_AddRef( IModule *po)
{
    return( ++( ( ApplicationMod *)po)->referenceCounter);
}

/*=============================================================================
FUNCTION:  MainMenuMod_Release

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32 ApplicationMod_Release( IModule *po)
{
    if ( ( ( ApplicationMod *) po)->referenceCounter == 0)
    {
        return 0;
    }

    return( --( ( ApplicationMod *)po)->referenceCounter);
}

/*=============================================================================
FUNCTION:  ApplicationMod_CreateInstance

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int  ApplicationMod_CreateInstance( IModule *po,
    IShell *pIShell,
    AEECLSID ClsId,
    void **ppObj
)
{
    *ppObj = NULL;

    if( ClsId != AEECLSID_APPLICATION || Application_New( pIShell, po, ( IApplication**)ppObj) != SUCCESS)
    {
        return EFAILED;
    }

    return SUCCESS;
}

/*=============================================================================
FUNCTION:  ApplicationMod_FreeResources

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void ApplicationMod_FreeResources( IModule *po, IHeap *ph, IFileMgr *pfm)
{
}

/*=============================================================================
FUNCTION:  Application_New

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int Application_New( IShell *ps, IModule *pIModule, IApplication **ppObj)
{
    int retVal = SUCCESS;
    if( 0 == gApplication.referenceCounter)
    {

        if( pIModule == NULL || ps == NULL)
        {
            return EFAILED;
        }

        MEMSET(&gApplication, 0, sizeof(gApplication));
        INIT_VTBL(&gApplication, IApplication, gApplicationMethods);

       
        gApplication.m_pShell     = ps;
        gApplication.m_pModule    = pIModule;
        retVal = CApplication_InitAppData(&gApplication);
        if(retVal != SUCCESS)
        {
            CApplication_FreeAppData((Application*)&gApplication);
            return retVal;
        }
        (void) ISHELL_AddRef( ps);
        (void) IMODULE_AddRef( pIModule);
        gApplication.referenceCounter  = 0;

        
    }

    ++ gApplication.referenceCounter;
    *ppObj = ( IApplication*)&gApplication;
    
    return retVal;
}

/*==============================================================================
函数:
    CApplicationApp_InitAppData

说明:
    初始化 Application Applet 结构数据缺省值。

参数:
    pMe [in]: 指向 Application Applet 对象结构的指针。该结构包含小程序的特定信息。

返回值:
    EFAILED: 初始化失败。
    SUCCESS: 初始化成功。

备注:

==============================================================================*/
static int CApplication_InitAppData(Application *pMe)
{
    if (NULL == pMe)
    {
        return EFAILED;
    }

    pMe->m_MainSel  = 0;
    
	
    if (ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_DISPLAY, 
            (void **) &pMe->m_pDisplay) != SUCCESS)
    {
        return EFAILED;
    }

	if (ISHELL_CreateInstance(pMe->m_pShell,
									 AEECLSID_ANNUNCIATOR,
									 (void **) &pMe->m_pIAnn))
	{
		CApplication_FreeAppData(pMe);
		return FALSE;
	}
	 if( ISHELL_CreateInstance( pMe->m_pShell, AEECLSID_BACKLIGHT, (void **)&pMe->m_pBacklight)!=AEE_SUCCESS)
    {
    	CApplication_FreeAppData(pMe);
        return FALSE;
    }
#ifdef FEATURE_OEMOMH
	if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                            AEECLSID_RUIM,
                                            (void **)&pMe->m_pIRUIM))
	{
		return EFAILED;
	}     
#endif    
    return SUCCESS;
}

/*==============================================================================
函数:
    CApplication_FreeAppData

说明:
    释放 Application Applet 相关资源。

参数:
    pMe [in]: 指向Application Applet 对象结构的指针。该结构包含小程序的特定信息。

返回值:
    none

备注:

==============================================================================*/
static void CApplication_FreeAppData(Application *pMe)
{
    if (NULL == pMe)
    {
        return;
    }

    pMe->m_eAppStatus = APPLICATION_STOP; 
    
    if (pMe->m_pDisplay != NULL)
    {
        (void) IDISPLAY_Release(pMe->m_pDisplay);
        pMe->m_pDisplay = NULL;
    }
	if (pMe->m_pIAnn)
    {
        IANNUNCIATOR_Release(pMe->m_pIAnn);
        pMe->m_pIAnn= NULL;
    }
    if(pMe->m_pBacklight)
    {
        IBACKLIGHT_Release(pMe->m_pBacklight);
        pMe->m_pBacklight=NULL;
    }
#ifdef FEATURE_OEMOMH    
	if (pMe->m_pIRUIM != NULL)
	{
	    IRUIM_Release(pMe->m_pIRUIM);
	    pMe->m_pIRUIM = NULL;
	}
#endif
}

/*=============================================================================
FUNCTION:  Application_AddRef

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32  Application_AddRef( IApplication *pi)
{
    return ( ++gApplication.referenceCounter);
}

/*=============================================================================
FUNCTION:  Application_Release

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32  Application_Release( IApplication *pi)
{
    register Application *pMe = (Application*)pi;

    if (pMe->referenceCounter == 0)
    {
        return 0;
    }
    
    if( --pMe->referenceCounter)
    {
        return pMe->referenceCounter;
    }


    CApplication_FreeAppData(pMe);

    (void) ISHELL_Release(pMe->m_pShell);
    (void) IMODULE_Release(pMe->m_pModule);

    return 0;
}

/*==============================================================================
函数:
    Application_RunFSM

说明:
    运行有限状态机引擎。

参数:
    pMe [in]: 指向Application Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值:
    无。

备注:

==============================================================================*/
static void Application_RunFSM(Application *pMe)
{
    NextFSMAction nextFSMAction = NFSMACTION_WAIT;
    for ( ; ; )
    {
        nextFSMAction = Application_ProcessState(pMe);

        // 这里不需要复杂的状态机，因此为简化状态机设计这里仅用一个布尔变量来
        // 指示dlgResult是否应该被改写。在某些特殊场合（见状态机设计），这种作
        // 法，避免了不必要的复杂性，简化了设计。
        if (pMe->m_bDoNotOverwriteDlgResult)
        {
            // We allow only one iteration without touching this variable. So,
            // let's reset it back to indicate that we are going to overwrite the
            // result for all subsequent iterations.
            pMe->m_bDoNotOverwriteDlgResult = FALSE;
        }
        else
        {
            // Overwrite the dlgResult to its default value. The next dialog that
            // we are going to display will set and return the dlgResult based on
            // the users action.
            pMe->m_eDlgReturn = DLGRET_CREATE;
        }

        // If the state machine processing engine indicates that it created a
        // dialog and requesting us to exit this loop, then we quit.
        if (nextFSMAction == NFSMACTION_WAIT)
        {
            break;
        }
    }
}

/*=============================================================================
FUNCTION:  MainMenu_HandleEvent

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean Application_HandleEvent( IApplication *pi,
    AEEEvent eCode,
    uint16  wParam,
    uint32 dwParam
)
{
    Application *pMe = (Application*)pi;
    AEEAppStart* as = NULL;
    AEEDeviceInfo di; 

    ISHELL_GetDeviceInfo(pMe->m_pShell,&di); 
    switch ( eCode)
    {
        case EVT_APP_START:
			
            // 此事件dwParam为指针，不应为0
            if (dwParam == 0) 
            {
                return FALSE;
            }            
            as = (AEEAppStart*)dwParam;
   
            pMe->m_rc = as->rc;
            pMe->m_currState  = APPLICATIONST_MAIN;
            pMe->m_eDlgReturn = DLGRET_CREATE;
            pMe->m_eAppStatus = APPLICATION_RUNNING;
            if(pMe->m_pIAnn != NULL)
            {	    			
                IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE);				
				MSG_FATAL("***zzg Application EVT_APP_START EnableAnnunciatorBar TRUE",0,0,0);				   
            }
            {
                AECHAR WTitle[20] = {0};
#ifdef FEATURE_VERSION_FLEXI203P				
    			(void)ISHELL_LoadResString(pMe->m_pShell,
                                        APPLICATION_RES_FILE_LANG,                                
                                        IDS_TOOLS_LIST,
                                        WTitle,
                                        sizeof(WTitle));
#else
				(void)ISHELL_LoadResString(pMe->m_pShell,
										APPLICATION_RES_FILE_LANG,								  
										IDS_APPLICATION_LIST,
										WTitle,
										sizeof(WTitle));

#endif
                if(pMe->m_pIAnn != NULL)
                {
    			    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
                }
            }
            Application_RunFSM(pMe);
            return TRUE;

        case EVT_APP_STOP:
            {
                if (pMe->m_pDisplay != NULL)
                {
                    (void) IDISPLAY_Release(pMe->m_pDisplay);
                    pMe->m_pDisplay = NULL;
                }
                pMe->m_eAppStatus = APPLICATION_STOP;
                
                return TRUE;
            }

        case EVT_APP_SUSPEND:
            pMe->m_eAppStatus = APPLICATION_SUSPEND;
            return TRUE;
        

        case EVT_APP_RESUME:
            {
                AEEAppStart* as = ( AEEAppStart*)dwParam;
                pMe->m_rc    = as->rc;
                pMe->m_rc.dy = di.cyScreen;
                pMe->m_eAppStatus = APPLICATION_RUNNING;
                Application_RunFSM(pMe); 
                return TRUE;
            }

        case EVT_DIALOG_INIT:
            
            pMe->m_pActiveIDlg = (IDialog*)dwParam;
            pMe->m_pActivedlgID = wParam;
            
            return Application_RouteDialogEvt(pMe,eCode,wParam,dwParam);

        case EVT_DIALOG_START:

            return Application_RouteDialogEvt(pMe,eCode,wParam,dwParam);

        case EVT_USER_REDRAW:
            return Application_RouteDialogEvt(pMe,eCode,wParam,dwParam);

        case EVT_DIALOG_END:
		  
            (void) Application_RouteDialogEvt(pMe,eCode,wParam,dwParam);
            pMe->m_pActiveIDlg = NULL;
            pMe->m_pActivedlgID = 0;
            if(pMe->m_eAppStatus == APPLICATION_RUNNING)
            {
                Application_RunFSM(pMe);
            }
            return TRUE;
        
        case EVT_KEY:
#if MIN_BREW_VERSION(3,0)
            // do not want to handle au
            if (((dwParam & KB_AUTOREPEAT) != 0) &&
                (wParam != AVK_UP) &&
                (wParam != AVK_DOWN) &&
                (wParam != AVK_LEFT) &&
                (wParam != AVK_RIGHT))
            {
                return TRUE;
            }
#endif
            return Application_RouteDialogEvt(pMe,eCode,wParam,dwParam);   
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
					return Application_RouteDialogEvt(pMe,eCode,wParam,dwParam);
				}
#endif

        case EVT_PEN_UP:
        case EVT_KEY_PRESS:
        case EVT_KEY_RELEASE:
        case EVT_COMMAND:
            return Application_RouteDialogEvt(pMe,eCode,wParam,dwParam); 
        default:
            return Application_RouteDialogEvt(pMe,eCode,wParam,dwParam);
    }
}

/*==============================================================================
函数:
   Application_ProcessState

说明:
    根据 pMe->m_currState, 调用相应的状态处理函数。

参数:
    pMe [in]: 指向Application Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
NextFSMAction Application_ProcessState(Application *pMe)
{
    NextFSMAction retVal = NFSMACTION_WAIT;
    
    if (NULL == pMe)
    {
        return retVal;
    }
    
    switch(pMe->m_currState)
    {
        case APPLICATIONST_MAIN:
            return APPLICATIONST_MAIN_Handler(pMe);
        case APPLICATIONST_FLASHLIGHT:
        	return APPLICATIONST_FLASHLIGHT_Handler(pMe);
        case APPLICATIONST_EXIT:
            return APPLICATIONST_EXIT_Handler(pMe);            

        default:
            break;

    }
    return retVal;
}

/*==============================================================================
函数:
    APPLICATIONST_MAIN_Handler

说明:
    APPLICATIONST_MAIN 状态处理函数。

参数:
    pMe [in]: 指向APPLICATION Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction APPLICATIONST_MAIN_Handler(Application *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    switch (pMe->m_eDlgReturn)
    {
        // 进入主界面
        case DLGRET_CREATE:
            {
                  Application_ShowDialog(pMe, IDD_LIST_APPLICATION);

            }
            return NFSMACTION_WAIT;
        case DLGRET_FLASHLITHT:
        	{
        		MOVE_TO_STATE(APPLICATIONST_FLASHLIGHT)
            	return NFSMACTION_CONTINUE;
        	}
        default:
            MOVE_TO_STATE(APPLICATIONST_EXIT)
            return NFSMACTION_CONTINUE;
    }
}


static NextFSMAction APPLICATIONST_FLASHLIGHT_Handler(Application *pMe)
{
	if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    switch (pMe->m_eDlgReturn)
    {
        // 进入主界面
        case DLGRET_CREATE:
            {
            	  MSG_FATAL("APPLICATIONST_FLASHLIGHT_Handler DLGRET_CREATE",0,0,0);
                  Application_ShowDialog(pMe, IDD_FLASHLIGHT_SETTING);

            }
            return NFSMACTION_WAIT;
         case DLGRET_CANCELED:         
            MOVE_TO_STATE(APPLICATIONST_MAIN)
            return NFSMACTION_CONTINUE;
        default:
            MOVE_TO_STATE(APPLICATIONST_EXIT)
            return NFSMACTION_CONTINUE;
    }
}


/*==============================================================================
函数:
    APPLICATIONST_EXIT_Handler

说明:
    APPLICATIONST_EXIT 状态处理函数。

参数:
    pMe [in]: 指向APPLICATION  Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction APPLICATIONST_EXIT_Handler(Application *pMe)
{
    (void) ISHELL_CloseApplet(pMe->m_pShell, FALSE);
    return NFSMACTION_WAIT;
} 


/*==============================================================================
函数:
       Application_ShowDialog

说明:
    函数由状态处理函数用来唤起相应对话框资源ID的对话框。

参数:
    pMe [in]: 指向 Application Applet 对象结构的指针。该结构包含小程序的特定信息。
    dlgResId [in]: 对话框资源ID，函数根据该ID创建对话框。

返回值:
    none

备注:

==============================================================================*/
void Application_ShowDialog(Application  *pMe,  uint16 dlgResId)
{
    int nRet;

    // 每次最多打开一个对话框
    if (ISHELL_GetActiveDialog(pMe->m_pShell) != NULL)
    {
        // 如果当前已经有对话框被打开，直接返回
        MSG_FATAL("Trying to create dialog %d without closing previous one",dlgResId,0,0);
        return;
    }
    if (NULL != pMe->m_pDisplay)
    {
        AEEDeviceInfo di={0,};
       (void)IDISPLAY_SetPrefs(pMe->m_pDisplay, "a:1", STRLEN("a:1"));
        ISHELL_GetDeviceInfo(pMe->m_pShell, &di);
        pMe->m_rc.dx = di.cxScreen;
        pMe->m_rc.dy = di.cyScreen;
        IDISPLAY_SetClipRect(pMe->m_pDisplay, &pMe->m_rc);
    }
    nRet = ISHELL_CreateDialog(pMe->m_pShell,APPLICATION_RES_FILE_LANG,dlgResId,NULL);
    if (nRet != SUCCESS)
    {
        MSG_FATAL("Failed to create the dialog %d in the MAINMENU applet.",dlgResId,0,0);
    }
}


/*==============================================================================
函数:
      Application_RouteDialogEvt
说明:
       函数将BREW事件路由给当前活动对话框的事件处理函数。

参数:
       pMe [In]: 指向SettingMenu Applet对象结构的指针,该结构包含小程序的特定信息.
       eCode [in]：事件代码。
       wParam [in]：与事件eCode关联的数据。
       dwParam [in]：与事件eCode关联的数据。

返回值:
       TRUE：传入事件被处理。
       FALSE：传入事件没被处理。

备注:

==============================================================================*/
boolean Application_RouteDialogEvt(Application *pMe,
    AEEEvent    eCode,
    uint16  wParam,
    uint32 dwParam
)

{
    if (NULL == pMe)
    {
        return FALSE;
    }

    if (NULL == pMe->m_pActiveIDlg)
    {
        return FALSE;
    }    
    switch( pMe->m_pActivedlgID)
    {

        case IDD_LIST_APPLICATION:
            return Application_ListMenuHandler(pMe, eCode, wParam, dwParam);
        case IDD_FLASHLIGHT_SETTING:
        	return Application_FlashlightMenuHandler(pMe, eCode, wParam,dwParam);
        default:
            return FALSE;
    }
}



/*=============================================================================
FUNCTION:  Application_ListMenuHandler

DESCRIPTION:   列表菜单

PARAMETERS:

=============================================================================*/

static boolean Application_ListMenuHandler(Application *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
    PARAM_NOT_REF(dwParam)
    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveIDlg,IDC_APPLICATION_LIST);
    AECHAR WTitle[40] = {0};
    if (pMenu == NULL)
    {
        return FALSE;
    }
    if(pMe->m_pIAnn != NULL)
    {
	    IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE);
    }
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
			#if 0
            IMENUCTL_SetTitle(pMenu, APPLICATION_RES_FILE_LANG, IDS_APPLICATION_LIST, NULL);
			#else
#if defined	(FEATURE_VERSION_FLEXI203)||defined(FEATURE_VERSION_IVIO203) ||defined(FEATURE_VERSION_C500BE)||defined	(FEATURE_VERSION_FLEXI203P)||defined (FEATURE_VERSION_VG68)
			(void)ISHELL_LoadResString(pMe->m_pShell,
                                    APPLICATION_RES_FILE_LANG,                                
                                    IDS_TOOLS_LIST,
                                    WTitle,
                                    sizeof(WTitle));
#else
			(void)ISHELL_LoadResString(pMe->m_pShell,
                                    APPLICATION_RES_FILE_LANG,                                
                                    IDS_APPLICATION_LIST,
                                    WTitle,
                                    sizeof(WTitle));
#endif
            if(pMe->m_pIAnn != NULL)
            {
			    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
            }
			#endif
			
#if defined	(FEATURE_VERSION_FLEXI203) ||defined(FEATURE_VERSION_IVIO203) 
			IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_ALARM, IDS_APPLICATION_ALARM, NULL, 0);
            IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_CALCULATOR, IDS_APPLICATION_CALCULATOR, NULL, 0);
            IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_CONVERTER, IDS_APPLICATION_CONVERTER, NULL, 0);
            IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_STOPWATCH, IDS_APPLICATION_STOPWATCH, NULL, 0);
            IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_TIMER, IDS_APPLICATION_TIMER, NULL, 0);
            IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_WORLDTIME, IDS_APPLICATION_WORLDTIME, NULL, 0);
#elif defined (FEATURE_VERSION_FLEXI203P)		
            IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_TIMER, IDS_APPLICATION_TIMER, NULL, 0);
			IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_ALARM, IDS_APPLICATION_ALARM, NULL, 0);
			IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_CALCULATOR, IDS_APPLICATION_CALCULATOR, NULL, 0);
			IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_CONVERTER, IDS_APPLICATION_CONVERTER, NULL, 0);
			IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_STOPWATCH, IDS_APPLICATION_STOPWATCH, NULL, 0);
			IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_TIMER, IDS_APPLICATION_TIMER, NULL, 0);
			IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_WORLDTIME, IDS_APPLICATION_WORLDTIME, NULL, 0);
#elif defined (FEATURE_VERSION_C500BE)		
			IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_ALARM, IDS_APPLICATION_ALARM, NULL, 0);
			//IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_CALCULATOR, IDS_APPLICATION_CALCULATOR, NULL, 0);
			IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_CONVERTER, IDS_APPLICATION_CONVERTER, NULL, 0);
			IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_STOPWATCH, IDS_APPLICATION_STOPWATCH, NULL, 0);
			IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_TIMER, IDS_APPLICATION_TIMER, NULL, 0);
			IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_WORLDTIME, IDS_APPLICATION_WORLDTIME, NULL, 0);
#elif defined (FEATURE_VERSION_SMART)
			IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_TIMER, IDS_APPLICATION_TIMER, NULL, 0);
            IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_STOPWATCH, IDS_APPLICATION_STOPWATCH, NULL, 0);
            IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_ALARM, IDS_APPLICATION_ALARM, NULL, 0);
            IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_CONVERTER, IDS_APPLICATION_CONVERTER, NULL, 0);
            IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_WORLDTIME, IDS_APPLICATION_WORLDTIME, NULL, 0);
			IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_RECORDER, IDS_APPLICATION_RECORDER, NULL, 0);
			IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_CALCULATOR, IDS_APPLICATION_CALCULATOR, NULL, 0);
#elif defined (FEATURE_VERSION_HITZ181)
			IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_TIMER, IDS_APPLICATION_TIMER, NULL, 0);
            IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_STOPWATCH, IDS_APPLICATION_STOPWATCH, NULL, 0);
            IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_ALARM, IDS_APPLICATION_ALARM, NULL, 0);
            IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_CONVERTER, IDS_APPLICATION_CONVERTER, NULL, 0);
            IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_WORLDTIME, IDS_APPLICATION_WORLDTIME, NULL, 0);
			IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_CALCULATOR, IDS_APPLICATION_CALCULATOR, NULL, 0);	
			IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_FLASHLIGHT, IDS_APPLICATION_FLASHLIGHT, NULL, 0);
#elif defined (FEATURE_VERSION_M8)
            IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_TIMER, IDS_APPLICATION_TIMER, NULL, 0);
            IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_STOPWATCH, IDS_APPLICATION_STOPWATCH, NULL, 0);
            IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_ALARM, IDS_APPLICATION_ALARM, NULL, 0);
            IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_CONVERTER, IDS_APPLICATION_CONVERTER, NULL, 0);
            IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_WORLDTIME, IDS_APPLICATION_WORLDTIME, NULL, 0);
			IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_RECORDER, IDS_APPLICATION_RECORDER, NULL, 0);
			IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_SCHEDULER, IDS_APPLICATION_SCHEDULER, NULL, 0);	
			IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_CALCULATOR, IDS_APPLICATION_CALCULATOR, NULL, 0);
#elif defined (FEATURE_VERSION_M8021)	
			IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_TIMER, IDS_APPLICATION_TIMER, NULL, 0);
            IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_STOPWATCH, IDS_APPLICATION_STOPWATCH, NULL, 0);
            IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_ALARM, IDS_APPLICATION_ALARM, NULL, 0);
            IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_CONVERTER, IDS_APPLICATION_CONVERTER, NULL, 0);
            IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_WORLDTIME, IDS_APPLICATION_WORLDTIME, NULL, 0);
			IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_RECORDER, IDS_APPLICATION_RECORDER, NULL, 0);
			IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_SCHEDULER, IDS_APPLICATION_SCHEDULER, NULL, 0);	
			IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_CALCULATOR, IDS_APPLICATION_CALCULATOR, NULL, 0);
#elif defined (FEATURE_VERSION_M8P)
#ifdef FEATURE_APP_MANAGER
			IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_BAM, IDS_APPLICATION_BAM, NULL, 0);
#endif
			IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_CAMERA, IDS_APPLICATION_CAMERA, NULL, 0);
			IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_TIMER, IDS_APPLICATION_TIMER, NULL, 0);
			IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_STOPWATCH, IDS_APPLICATION_STOPWATCH, NULL, 0);
			IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_ALARM, IDS_APPLICATION_ALARM, NULL, 0);
			IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_CONVERTER, IDS_APPLICATION_CONVERTER, NULL, 0);
			IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_WORLDTIME, IDS_APPLICATION_WORLDTIME, NULL, 0);
			IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_RECORDER, IDS_APPLICATION_RECORDER, NULL, 0);
			IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_SCHEDULER, IDS_APPLICATION_SCHEDULER, NULL, 0);	
			IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_CALCULATOR, IDS_APPLICATION_CALCULATOR, NULL, 0);
#elif defined (FEATURE_DISP_128X160)
#ifdef FEATURE_APP_MANAGER
			IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_BAM, IDS_APPLICATION_BAM, NULL, 0);
#endif
#ifndef FEATURE_VERSION_C306
            IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_TIMER, IDS_APPLICATION_TIMER, NULL, 0);
#endif
            IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_STOPWATCH, IDS_APPLICATION_STOPWATCH, NULL, 0);
            IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_ALARM, IDS_APPLICATION_ALARM, NULL, 0);
            IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_CONVERTER, IDS_APPLICATION_CONVERTER, NULL, 0);
            IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_WORLDTIME, IDS_APPLICATION_WORLDTIME, NULL, 0);
            #ifndef FEATURE_VERSION_C01
            IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_FLASHLIGHT, IDS_APPLICATION_FLASHLIGHT, NULL, 0);
            #else
            IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_SCHEDULER, IDS_APPLICATION_SCHEDULER, NULL, 0);	
			IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_CALCULATOR, IDS_APPLICATION_CALCULATOR, NULL, 0);
            #endif
			
#else
#ifndef FEATURE_VERSION_W208S
#ifdef FEATURE_APP_MANAGER
            IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_BAM, IDS_APPLICATION_BAM, NULL, 0);
#endif
#endif
            IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_TIMER, IDS_APPLICATION_TIMER, NULL, 0);
            IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_STOPWATCH, IDS_APPLICATION_STOPWATCH, NULL, 0);
            IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_ALARM, IDS_APPLICATION_ALARM, NULL, 0);
            IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_CONVERTER, IDS_APPLICATION_CONVERTER, NULL, 0);
            IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_WORLDTIME, IDS_APPLICATION_WORLDTIME, NULL, 0);
#ifdef FEAUTRE_VERSION_N450
			IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_FLASHLIGHT, IDS_APPLICATION_FLASHLIGHT, NULL, 0);
#endif
#if defined(FEATURE_VERSION_VG68)||defined(FEATURE_VERSION_1110W516) || defined(FEATURE_VERSION_W208S)
            IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_CALCULATOR, IDS_APPLICATION_CALCULATOR, NULL, 0);
#endif
#if defined (FEATURE_DISP_128X128)
            IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_SCHEDULER, IDS_APPLICATION_SCHEDULER, NULL, 0);
            IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_GAME, IDS_APPLICATION_GAME, NULL, 0);
            IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_CALCULATOR, IDS_APPLICATION_CALCULATOR, NULL, 0);
#endif
#endif
#ifdef FEATURE_SUPPORT_WAP_APP
#ifndef FEATURE_VERSION_W208S
			IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_BROWSER, IDS_APPLICATION_BROWSER, NULL, 0); 
#else
			IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_SCHEDULER, IDS_APPLICATION_SCHEDULER, NULL, 0); 
#endif
#endif
#ifdef	FEATURE_APP_BLUETOOTH  //add by yangdecai
            IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG,IDS_APPLICATION_BLUETOOTH, IDS_APPLICATION_BLUETOOTH, NULL, 0); 
#endif
            return TRUE;
            
        case EVT_DIALOG_START:
            {  
                int i;
                uint16 wID;
                AECHAR pwsz[67] = {0};
                AECHAR pstr[64] = {0};
                AECHAR wsFmt[5] = {0};
#ifdef FEATURE_OEMOMH
                AECHAR  AppLabel[33];
                MEMSET(AppLabel, 0x00, sizeof(AppLabel));
#endif

                (void)STRTOWSTR("%d. ",wsFmt,sizeof(wsFmt));
                for (i=0;i<IMENUCTL_GetItemCount(pMenu);i++)
                {
                    wID = IMENUCTL_GetItemID(pMenu, i);
                    WSPRINTF(pwsz,sizeof(pwsz),wsFmt,i+1);
#ifdef FEATURE_OEMOMH
                    if(wID == IDS_APPLICATION_BAM && pMe->m_pIRUIM && SUCCESS == IRUIM_Get_AppLabels_Code(pMe->m_pIRUIM,RUIM_APPLABEL_BREW,pstr))
                    {
                        //STRTOWSTR(AppLabel, pstr, sizeof(pstr));
                        DBGPRINTF("Buf =%S, length=%d", pstr, WSTRLEN(pstr));
                    }
                    else
#endif
                    {
                    ISHELL_LoadResString( pMe->m_pShell,
                          APPLICATION_RES_FILE_LANG,
                          wID,
                          pstr,
                          sizeof(pstr));
                    }
                    WSTRLCAT(pwsz,pstr,sizeof(pwsz));
                    IMENUCTL_SetItemText(pMenu, wID, NULL, NULL, pwsz);
                }
                
                IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL);
                IMENUCTL_SetOemProperties( pMenu, OEMMP_USE_MENU_STYLE);
                IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
                IMENUCTL_SetSel(pMenu, pMe->m_MainSel);
                (void) ISHELL_PostEvent(pMe->m_pShell, AEECLSID_MAIN_MENU, EVT_USER_REDRAW,0,0);
            }
            return TRUE;
            
        case EVT_USER_REDRAW:
            //(void)IMENUCTL_Redraw(pMenu);
            return TRUE;
            
        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
				case AVK_1:
                case AVK_2:
                case AVK_3:
                case AVK_4:
                case AVK_5:
                case AVK_6:
				case AVK_7:	
				case AVK_8:		
				case AVK_9:
                    StartApplet(pMe, IMENUCTL_GetItemID(pMenu, (wParam - AVK_1)));
                    return TRUE;
                    
                case AVK_0:
                    StartApplet(pMe, IMENUCTL_GetItemID(pMenu, 9));
                    return TRUE;
                    
                case AVK_STAR:
                    StartApplet(pMe, IMENUCTL_GetItemID(pMenu, 10));
                    return TRUE;
                    
                case AVK_POUND:
                    StartApplet(pMe, IMENUCTL_GetItemID(pMenu, 11));
                    return TRUE;
                    
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;
                    
                default:
                    break;
          
            }
            return TRUE;
            
        case EVT_COMMAND:
            pMe->m_MainSel = wParam;
			StartApplet(pMe, wParam);
            return TRUE;
            
        default:
            break;
    }             
    return FALSE;
}


static boolean  Application_FlashlightMenuHandler(Application *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
	PARAM_NOT_REF(dwParam)
    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveIDlg,IDC_FLASHLIGHT_SET);
    AECHAR WTitle[40] = {0};
    if (pMenu == NULL)
    {
        return FALSE;
    }
    if(pMe->m_pIAnn != NULL)
    {
	    IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE);
    }
     switch (eCode)
    {
        case EVT_DIALOG_INIT:

			(void)ISHELL_LoadResString(pMe->m_pShell,
                                    APPLICATION_RES_FILE_LANG,                                
                                    IDS_APPLICATION_FLASHLIGHT,
                                    WTitle,
                                    sizeof(WTitle));

            if(pMe->m_pIAnn != NULL)
            {
			    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
            }
            IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG, IDS_DIAG_AND_DATA, IDS_DIAG_AND_DATA, NULL, 0);
            IMENUCTL_AddItem(pMenu, APPLICATION_RES_FILE_LANG, IDS_MASS_STORAGE, IDS_MASS_STORAGE, NULL, 0);
            return TRUE;
        case EVT_DIALOG_START:
            {
            	uint16 wItemID;
                boolean Is_on = FALSE;
                IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_TEXT_ALIGN_LEFT_ICON_ALIGN_RIGHT);
                IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);

                IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
                OEM_GetConfig(CFGI_FLSHLITHG_STATUS,&Is_on, sizeof(Is_on));
                if(Is_on)
                {
                	wItemID = IDS_DIAG_AND_DATA;
                }
                else
                {
                	wItemID = IDS_MASS_STORAGE;
                }

                InitMenuIcons(pMenu);
                SetMenuIcon(pMenu, wItemID, TRUE);
                IMENUCTL_SetSel(pMenu, wItemID);
                (void) ISHELL_PostEvent( pMe->m_pShell,
                                         AEECLSID_MAIN_MENU,
                                         EVT_USER_REDRAW,
                                         0,
                                         0);
            }
            return TRUE;

        case EVT_USER_REDRAW:
            //(void)IMENUCTL_Redraw(pMenu);
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;

                  default:
                    break;
            }
            return TRUE;
#ifdef FEATURE_LCD_TOUCH_ENABLE//andrew add for LCD touch
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

				if(TOUCH_PT_IN_RECT(wXPos,wYPos,rc))
				{
					if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
					{
						boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APPLICATION,EVT_KEY,AVK_SELECT,0);
						return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)   && wXPos < rc.x + (rc.dx/3)*2 )//左
					{
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APPLICATION,EVT_KEY,AVK_INFO,0);
						 return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
					{						
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APPLICATION,EVT_KEY,AVK_CLR,0);
						 return rt;
					}
				}

			}
			break;
#endif 
        case EVT_COMMAND:
            {
                boolean bytNewData = 0;

                switch (wParam)
                {
                    case  IDS_DIAG_AND_DATA:
                       bytNewData = TRUE;
                       IBACKLIGHT_TurnOnTorch(pMe->m_pBacklight);
                       break;

                    case IDS_MASS_STORAGE:
                       bytNewData = FALSE;
                       IBACKLIGHT_TurnOffTorch(pMe->m_pBacklight);
                       break;

                    default:
                       break;

                }
                OEM_SetConfig(CFGI_FLSHLITHG_STATUS,&bytNewData, sizeof(bytNewData));
                InitMenuIcons(pMenu);
                SetMenuIcon(pMenu, wParam, TRUE);
                (void) ISHELL_PostEvent( pMe->m_pShell,
                                         AEECLSID_MAIN_MENU,
                                         EVT_USER_REDRAW,
                                         0,
                                         0);
            }
        default:
        	break;
	}
}


/*=============================================================================
FUNCTION:  StartApplet

DESCRIPTION:  启动applet

PARAMETERS:  如果APPLET 有变动，只需改动次函数

=============================================================================*/
static int StartApplet(Application *pMe, int i)
{
    int Result = EUNSUPPORTED;
	
    switch(i){
    case IDS_APPLICATION_BAM:
#ifdef FEATURE_OEMOMH
        if(ISHELL_SendEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_OMH_PROMPT,0,0))
        {
            break;
        }
#endif
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APPMANAGER);
        if(Result == SUCCESS && pMe->m_pIAnn != NULL)
        {
            IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,NULL);
        }
        break;
        
    case IDS_APPLICATION_TIMER:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APPTIMER);		
        break;
        
    case IDS_APPLICATION_STOPWATCH:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_STOPWATCH);
        break;
        
    case IDS_APPLICATION_ALARM:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_ALARMCLOCK);
        break;
        
    case IDS_APPLICATION_CONVERTER:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_CONVERTER);
        break;
        
    case IDS_APPLICATION_WORLDTIME:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_WORLDTIME);
        break;
        
    case IDS_APPLICATION_RECORDER:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_RECORDER);
        break;
        
    case IDS_APPLICATION_SCHEDULER:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_SCHEDULEAPP);
        break;
        
    case IDS_APPLICATION_GAME:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_GAME);
        break;
        
    case IDS_APPLICATION_CALCULATOR:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_CALCAPP);
        break;
        
    case IDS_APPLICATION_CAMERA:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APP_CAMERA);
        break;
        
    case IDS_APPLICATION_FMRADIO:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APP_FMRADIO);
        break;
#ifdef FEATURE_SUPPORT_WAP_APP
    case IDS_APPLICATION_BROWSER:
    {
    	char	buf[12];
		
		OEM_SetBROWSER_ADSAccount();	//Add By zzg 2011_07_08
		
    	MSG_FATAL("AEECLSID_BRW_APP...........START",0,0,0);
    	start_info.appid_fx = APP_ORIGINATOR_BROWSER;
        start_info.subtype_keycode = APP_BROWSER_START_MAINMENU;
        start_info.par_keychar = 0;
        //start_info.fInfo.url_info.title = "163";
        //start_info.fInfo.url_info.url = "http://www.163.com";
        SPRINTF(buf, "%p", &start_info);
		Result = ISHELL_StartAppletArgs(pMe->m_pShell,AEECLSID_NF3,buf);
        //Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_BRW_APP);
        MSG_FATAL("AEECLSID_BRW_APP...........Result=%d",Result,0,0);		
		
        break;
    }
#endif
#ifdef	FEATURE_APP_BLUETOOTH  //add by yangdecai
    case IDS_APPLICATION_BLUETOOTH:		
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_BLUETOOTH_APP);		
        break;
#endif
#if defined(FEATURE_VERSION_HITZ181) || defined(FEATURE_DISP_128X160) || defined(FEAUTRE_VERSION_N450)
	case IDS_APPLICATION_FLASHLIGHT:		
        CLOSE_DIALOG(DLGRET_FLASHLITHT)
        break;
#endif
    default:
        break;
    }
    return Result;
}


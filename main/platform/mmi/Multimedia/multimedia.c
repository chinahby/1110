/** ----------------------------------------------------------------------------
 *Multimed.c
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
#ifndef AEECLSID_MULTIMEDIA_LIST
    #error AEECLSID_MULTIMEDIA_LIST must be defined
#endif

#include "AEEShell.h"
#include "AEE_OEM.h"
#include "AEEModGen.h"
#include "AEEModTable.h"
#include "AEEMenu.h"
#include "AEEStdLib.h"
#include "AEEAnnunciator.h"
#include "multimedia.h"
#include "multimedia_priv.h" 

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
static MultimediaMod gMultimedMod;
static const VTBL( IModule) gModFuncs =
{
    MultimedMod_AddRef,
    MultimedMod_Release,
    MultimedMod_CreateInstance,
    MultimedMod_FreeResources
};

static uint32  Multimed_AddRef( IMultimed *pi);

static uint32  Multimed_Release ( IMultimed *pi);

static boolean Multimed_HandleEvent( IMultimed *pi,
    AEEEvent eCode,
    uint16  wParam,
    uint32 dwParam
);




static Multimed gMultimed={0};

static const VTBL( IMultimed) gMultimedMethods =
{
    Multimed_AddRef,
    Multimed_Release,
    Multimed_HandleEvent,
};

int MultimedMod_Load( IShell *pIShell, void *ph, IModule **ppMod);

int MultimedMod_New( int16 nSize,
    IShell              *pIShell,
    void                *ph,
    IModule             **ppMod,
    PFNMODCREATEINST    pfnMC,
    PFNFREEMODDATA      pfnMF
);

static int Multimed_New( IShell *ps, IModule *pIModule, IMultimed **ppObj);

static int CMultimed_InitAppData(Multimed *pMe);

static void CMultimed_FreeAppData(Multimed *pMe);

static void Multimed_RunFSM(Multimed *pMe);

static int StartApplet(Multimed *pMe, int i);

boolean Multimed_RouteDialogEvt(Multimed *pMe,
    AEEEvent    eCode,
    uint16  wParam,
    uint32 dwParam
);

NextFSMAction Multimed_ProcessState(Multimed *pMe);

void Multimed_ShowDialog(Multimed  *pMe,  uint16 dlgResId);

// MAINST_MAIN 状态处理函数
static NextFSMAction MAINST_MULTIMED_Handler(Multimed *pMe);


//MAINST_EXIT  状态处理函数
static NextFSMAction MULTIMEDST_EXIT_Handler(Multimed *pMe);
static boolean Multimed_ListMenuHandler(Multimed *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam);



/*==============================================================================
                              
                              本地（静态）数据
                              
==============================================================================*/




/*=============================================================================
FUNCTION:  MultimedMod_Load

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int MultimedMod_Load( IShell *pIShell, void *ph, IModule **ppMod)
{
    return MultimedMod_New( sizeof(MultimediaMod),
                    pIShell,
                    ph,
                    ppMod,
                    NULL,
                    NULL);
}

/*=============================================================================
FUNCTION:  MultimedMod_New

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int  MultimedMod_New( int16 nSize,
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

    MEMSET( &gMultimedMod, 0, sizeof(MultimediaMod));

    INIT_VTBL( &gMultimedMod, IModule, gModFuncs);
    gMultimedMod.referenceCounter = 1;
    *ppMod = ( IModule *)&gMultimedMod;
    return AEE_SUCCESS;
}

/*=============================================================================
FUNCTION:  MultimedMod_AddRef

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32 MultimedMod_AddRef( IModule *po)
{
    return( ++( ( MultimediaMod *)po)->referenceCounter);
}

/*=============================================================================
FUNCTION:  MultimedMod_Release

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32 MultimedMod_Release( IModule *po)
{
    if ( ( ( MultimediaMod *) po)->referenceCounter == 0)
    {
        
        return 0;
    }
    return( --( ( MultimediaMod *)po)->referenceCounter);
}

/*=============================================================================
FUNCTION:  MultimedMod_CreateInstance

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int  MultimedMod_CreateInstance( IModule *po,
    IShell *pIShell,
    AEECLSID ClsId,
    void **ppObj
)
{
    MSG_FATAL("MultimedMod_CreateInstance Start",0,0,0);
    *ppObj = NULL;

    if( ClsId != AEECLSID_MULTIMEDIA_LIST || Multimed_New( pIShell, po, ( IMultimed**)ppObj) != SUCCESS)
    {
        return EFAILED;
    }
    MSG_FATAL("MultimedMod_CreateInstance End",0,0,0);
    return SUCCESS;
}

/*=============================================================================
FUNCTION:  MultimedMod_FreeResources

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void MultimedMod_FreeResources( IModule *po, IHeap *ph, IFileMgr *pfm)
{
}

/*=============================================================================
FUNCTION:  Multimed_New

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int Multimed_New( IShell *ps, IModule *pIModule, IMultimed **ppObj)
{
    int retVal = SUCCESS;
    MSG_FATAL("Multimed_New Start",0,0,0);
    if( 0 == gMultimed.referenceCounter)
    {

        if( pIModule == NULL || ps == NULL)
        {
            return EFAILED;
        }

        MEMSET(&gMultimed, 0, sizeof(gMultimed));
        INIT_VTBL(&gMultimed, IMultimed, gMultimedMethods);

       
        gMultimed.m_pShell     = ps;
        gMultimed.m_pModule    = pIModule;
        retVal = CMultimed_InitAppData(&gMultimed);
        MSG_FATAL("Multimed_New retVal=%d",retVal,0,0);
        if(retVal != SUCCESS)
        {
            CMultimed_FreeAppData((Multimed*)&gMultimed);
            return retVal;
        }
        (void) ISHELL_AddRef( ps);
        (void) IMODULE_AddRef( pIModule);
        gMultimed.referenceCounter  = 0;

        
    }

    ++ gMultimed.referenceCounter;
    *ppObj = ( IMultimed*)&gMultimed;
    MSG_FATAL("Multimed_New retVal=%d",retVal,0,0);
    return retVal;
}

/*==============================================================================
函数:
    CMultimedApp_InitAppData

说明:
    初始化 Multimed Applet 结构数据缺省值。

参数:
    pMe [in]: 指向 Multimed Applet 对象结构的指针。该结构包含小程序的特定信息。

返回值:
    EFAILED: 初始化失败。
    SUCCESS: 初始化成功。

备注:

==============================================================================*/
static int CMultimed_InitAppData(Multimed *pMe)
{
    if (NULL == pMe)
    {
        return EFAILED;
    }

    pMe->m_MainSel  = 0;
    
    if (ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_DISPLAY, 
            (void **) &pMe->m_pDisplay) != SUCCESS)
    {
        MSG_FATAL("CMultimed_InitAppData EFAILED",0,0,0);
        return EFAILED;
    }
    
	if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,AEECLSID_ANNUNCIATOR,(void **)&pMe->m_pIAnn))
    {
        MSG_FATAL("CMultimed_InitAppData EFAILED",0,0,0);
        return EFAILED;
    }
    return SUCCESS;
}

/*==============================================================================
函数:
    CMultimedFreeAppData

说明:
    释放 Multimed Applet 相关资源。

参数:
    pMe [in]: Multimed Applet 对象结构的指针。该结构包含小程序的特定信息。

返回值:
    none

备注:

==============================================================================*/
static void CMultimed_FreeAppData(Multimed *pMe)
{
    if (NULL == pMe)
    {
        return;
    }
    pMe->m_eAppStatus = MULTIMEDIA_STOP; 
    
    if (pMe->m_pDisplay != NULL)
    {
        (void) IDISPLAY_Release(pMe->m_pDisplay);
        pMe->m_pDisplay = NULL;
    }
	if(pMe->m_pIAnn)
    {
        IANNUNCIATOR_Release(pMe->m_pIAnn);
        pMe->m_pIAnn = NULL;
    }

}

/*=============================================================================
FUNCTION:  Multimed_AddRef

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32  Multimed_AddRef( IMultimed *pi)
{
    return ( ++gMultimed.referenceCounter);
}

/*=============================================================================
FUNCTION:  Multimed_Release

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32  Multimed_Release( IMultimed *pi)
{
    register Multimed *pMe = (Multimed*)pi;
   
    if (pMe->referenceCounter == 0)
    {
        return 0;
    }
    
    if( --pMe->referenceCounter)
    {
        return pMe->referenceCounter;
    }


    CMultimed_FreeAppData(pMe);

    (void) ISHELL_Release(pMe->m_pShell);
    (void) IMODULE_Release(pMe->m_pModule);
   
    return 0;
}

/*==============================================================================
函数:
    Game_RunFSM

说明:
    运行有限状态机引擎。

参数:
    pMe [in]: 指向Application Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值:
    无。

备注:

==============================================================================*/
static void Multimed_RunFSM(Multimed *pMe)
{
    NextFSMAction nextFSMAction = NFSMACTION_WAIT;
    for ( ; ; )
    {
        nextFSMAction = Multimed_ProcessState(pMe);

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
/*==============================================================================
函数:
      Multimed_RouteDialogEvt
说明:
       函数将BREW事件路由给当前活动对话框的事件处理函数。

参数:
       pMe [In]: 指向Multimed Applet对象结构的指针,该结构包含小程序的特定信息.
       eCode [in]：事件代码。
       wParam [in]：与事件eCode关联的数据。
       dwParam [in]：与事件eCode关联的数据。

返回值:
       TRUE：传入事件被处理。
       FALSE：传入事件没被处理。

备注:

==============================================================================*/
boolean Multimed_RouteDialogEvt(Multimed *pMe,
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

        case IDD_LIST_MULTIMEDIA_DIALOGS:
            return Multimed_ListMenuHandler(pMe, eCode, wParam, dwParam);

        default:
            return FALSE;
    }
}


/*=============================================================================
FUNCTION:  Multimed_HandleEvent

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean Multimed_HandleEvent( IMultimed *pi,
    AEEEvent eCode,
    uint16  wParam,
    uint32 dwParam
)
{
    Multimed *pMe = (Multimed*)pi;
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
            pMe->m_currState  = MULTIMEDIAST_MAIN;
            pMe->m_eDlgReturn = DLGRET_CREATE;
            pMe->m_eAppStatus = MULTIMEDIA_RUNNING;
            if(pMe->m_pIAnn != NULL)
            {
                IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE); 
            }
            Multimed_RunFSM(pMe);
            return TRUE;

        case EVT_APP_STOP:
            {
                if (pMe->m_pDisplay != NULL)
                {
                    (void) IDISPLAY_Release(pMe->m_pDisplay);
                    pMe->m_pDisplay = NULL;
                }
                pMe->m_eAppStatus = MULTIMEDIA_STOP;
                
                return TRUE;
            }

        case EVT_APP_SUSPEND:
          
            pMe->m_eAppStatus = MULTIMEDIA_SUSPEND;
            
            return TRUE;
        

        case EVT_APP_RESUME:
            {
                AEEAppStart* as = ( AEEAppStart*)dwParam;
            
                pMe->m_rc    = as->rc;
                pMe->m_rc.dy = di.cyScreen;
                pMe->m_eAppStatus = MULTIMEDIA_RUNNING;
                Multimed_RunFSM(pMe); 
                return TRUE;
            }

        case EVT_DIALOG_INIT:
           
            pMe->m_pActiveIDlg = (IDialog*)dwParam;
            pMe->m_pActivedlgID = wParam;
            
            return Multimed_RouteDialogEvt(pMe, eCode, wParam, dwParam);

        case EVT_DIALOG_START:
            
            return Multimed_RouteDialogEvt(pMe, eCode, wParam, dwParam);

        case EVT_USER_REDRAW:
            return Multimed_RouteDialogEvt(pMe, eCode, wParam, dwParam);

        case EVT_DIALOG_END:
		 
            (void)Multimed_RouteDialogEvt(pMe, eCode, wParam, dwParam);
            pMe->m_pActiveIDlg = NULL;
            pMe->m_pActivedlgID = 0;
            if(pMe->m_eAppStatus == MULTIMEDIA_RUNNING)
            {
                Multimed_RunFSM(pMe);
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
				return Multimed_RouteDialogEvt(pMe,eCode,wParam,dwParam);
			}
#endif

			
        case EVT_PEN_UP:
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
            return Multimed_RouteDialogEvt(pMe, eCode, wParam, dwParam);

        case EVT_KEY_PRESS:
        case EVT_KEY_RELEASE:
        case EVT_COMMAND:
            return Multimed_RouteDialogEvt(pMe, eCode, wParam, dwParam); 
        default:
            return Multimed_RouteDialogEvt(pMe, eCode, wParam, dwParam);
    }
}

/*==============================================================================
函数:
   Game_ProcessState

说明:
    根据 pMe->m_currState, 调用相应的状态处理函数。

参数:
    pMe [in]: 指向Game Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
NextFSMAction Multimed_ProcessState(Multimed *pMe)
{
    NextFSMAction retVal = NFSMACTION_WAIT;
    
    if (NULL == pMe)
    {
        return retVal;
    }
    
    switch(pMe->m_currState)
    {
        case MULTIMEDIAST_MAIN:
            return MAINST_MULTIMED_Handler(pMe);

        case MULTIMEDIAST_EXIT:
            return MULTIMEDST_EXIT_Handler(pMe);            

        default:
            break;

    }
    return retVal;
}

/*==============================================================================
函数:
    GAMEST_MAIN_Handler

说明:
   GAMEST_MAIN 状态处理函数。

参数:
    pMe [in]: 指向GAME Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction MAINST_MULTIMED_Handler(Multimed *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    switch (pMe->m_eDlgReturn)
    {
        // 进入主界面
        case DLGRET_CREATE:
            Multimed_ShowDialog(pMe, IDD_LIST_MULTIMEDIA_DIALOGS);
            return NFSMACTION_WAIT;
            
        default:
            MOVE_TO_STATE(MULTIMEDIAST_EXIT)
            return NFSMACTION_CONTINUE;
    }
}



/*==============================================================================
函数:
    GAMEST_EXIT_Handler

说明:
    GAMEST_EXIT 状态处理函数。

参数:
    pMe [in]: 指向GAME Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction MULTIMEDST_EXIT_Handler(Multimed *pMe)
{
    (void) ISHELL_CloseApplet(pMe->m_pShell, FALSE);
    return NFSMACTION_WAIT;
} 
/*
       Multimed_ShowDialog

说明:
    函数由状态处理函数用来唤起相应对话框资源ID的对话框。

参数:
    pMe [in]: 指向 MAINMENU Applet 对象结构的指针。该结构包含小程序的特定信息。
    dlgResId [in]: 对话框资源ID，函数根据该ID创建对话框。

返回值:
    none

备注:

==============================================================================*/
void Multimed_ShowDialog(Multimed  *pMe,  uint16 dlgResId)
{
    int nRet;
    // 每次最多打开一个对话框
    if (ISHELL_GetActiveDialog(pMe->m_pShell) != NULL)
    {
        // 如果当前已经有对话框被打开，直接返回
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
    
    nRet = ISHELL_CreateDialog(pMe->m_pShell,MULTIMEDIA_RES_FILE_LANG,dlgResId,NULL);
    if (nRet != SUCCESS)
    {
    }
    
}


/*=============================================================================
FUNCTION:  Multimed_ListMenuHandler

DESCRIPTION:   列表菜单

PARAMETERS:

=============================================================================*/

static boolean Multimed_ListMenuHandler(Multimed *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
    PARAM_NOT_REF(dwParam)
    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveIDlg,IDC_MULTIMEDIA_LIST);
        
    if (pMenu == NULL)
    {
        return FALSE;
    }

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
			#if 0
            IMENUCTL_SetTitle(pMenu, MULTIMEDIA_RES_FILE_LANG, IDS_MULTIMEDIA_TITLE, NULL);
			#else
		    {
		  		AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        MULTIMEDIA_RES_FILE_LANG,                                
                        IDS_MULTIMEDIA_TITLE,
                        WTitle,
                        sizeof(WTitle));
                if(pMe->m_pIAnn != NULL)
                {
				    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
                }
		    }
			#endif
            
            //IMENUCTL_SetTitle(pMenu, MULTIMEDIA_RES_FILE_LANG, IDS_MENU_LIST, NULL);      
#if defined (FEATURE_VERSION_W208S) || defined (FEATURE_VERSION_C337)||defined(FEATURE_VERSION_C316)
			IMENUCTL_AddItem(pMenu, MULTIMEDIA_RES_FILE_LANG,IDS_MULTIMEDIA_MUSICPLAYER, IDS_MULTIMEDIA_MUSICPLAYER, NULL, 0);
#endif

#ifdef FEATURE_BREW_CAMERA
#ifndef FEATURE_VERSION_W208S
            IMENUCTL_AddItem(pMenu, MULTIMEDIA_RES_FILE_LANG,IDS_MULTIMEDIA_CAMERA, IDS_MULTIMEDIA_CAMERA, NULL, 0);
#endif
#endif
#ifndef FEATURE_VERSION_C180
#ifndef FEATURE_VERSION_W208S
            IMENUCTL_AddItem(pMenu, MULTIMEDIA_RES_FILE_LANG,IDS_MULTIMEDIA_FMRADIO, IDS_MULTIMEDIA_FMRADIO, NULL, 0);
#endif
#endif
#if defined	(FEATURE_VERSION_FLEXI203) || defined (FEATURE_VERSION_IVIO203) ||defined (FEATURE_VERSION_FLEXI203P)|| defined (FEATURE_VERSION_C500BE)||defined (FEATURE_VERSION_VG68)
            IMENUCTL_AddItem(pMenu, MULTIMEDIA_RES_FILE_LANG,IDS_MULTIMEDIA_MUSICPLAYER, IDS_MULTIMEDIA_MUSICPLAYER, NULL, 0);
#endif

#ifdef FEATURE_VERSION_C337
			IMENUCTL_AddItem(pMenu, MULTIMEDIA_RES_FILE_LANG,IDS_MULTIMEDIA_SOUND_RECORDER, IDS_MULTIMEDIA_SOUND_RECORDER, NULL, 0);
#elif defined(FEATURE_VERSION_W317A)
            IMENUCTL_AddItem(pMenu, MULTIMEDIA_RES_FILE_LANG,IDS_MULTIMEDIA_AUDIORECORDER, IDS_MULTIMEDIA_AUDIORECORDER, NULL, 0);
#else
            IMENUCTL_AddItem(pMenu, MULTIMEDIA_RES_FILE_LANG,IDS_MULTIMEDIA_RECORDER, IDS_MULTIMEDIA_RECORDER, NULL, 0);
#endif

#ifdef FEATURE_APP_MPEG4
#if !defined(FEATURE_PEKTEST)
		IMENUCTL_AddItem(pMenu, MULTIMEDIA_RES_FILE_LANG,IDS_MULTIMEDIA_VIDEOPLAYER, IDS_MULTIMEDIA_VIDEOPLAYER, NULL, 0);
#endif
#endif
#ifdef FEATURE_ANALOG_TV
            IMENUCTL_AddItem(pMenu, MULTIMEDIA_RES_FILE_LANG,IDS_MULTIMEDIA_TV, IDS_MULTIMEDIA_TV, NULL, 0);
#endif
#if defined (FEATURE_VERSION_VG68) ||defined(FEATURE_VERSION_C01)
            IMENUCTL_AddItem(pMenu, MULTIMEDIA_RES_FILE_LANG,IDS_MULTIMEDIA_MEDIAGALLERY,IDS_MULTIMEDIA_MEDIAGALLERY, NULL, 0);
#endif
#if defined(FEATURE_VERSION_W516) || defined(FEATURE_VERSION_W208S) || defined(FEATURE_VERSION_W027)||defined(FEATURE_VERSION_C11)
#if !defined(FEATURE_PEKTEST)
            IMENUCTL_AddItem(pMenu, MULTIMEDIA_RES_FILE_LANG,IDS_MULTIMEDIA_VIDEO_CAMERA, IDS_MULTIMEDIA_VIDEO_CAMERA, NULL, 0);
#endif
#endif
            return TRUE;
            
        case EVT_DIALOG_START:
            {  
                int i;

#ifndef FEATURE_VERSION_C337
                for (i=1;i<=IMENUCTL_GetItemCount(pMenu);i++)
                {
                    AECHAR pwsz[67] = {0};
                    AECHAR pstr[64] = {0};
                    AECHAR wsFmt[5] = {0};
    
                    (void)STRTOWSTR("%d. ",wsFmt,sizeof(wsFmt));
                    WSPRINTF(pwsz,sizeof(pwsz),wsFmt,i);
                    
                    ISHELL_LoadResString( pMe->m_pShell,
                                          MULTIMEDIA_RES_FILE_LANG,
                                          IMENUCTL_GetItemID(pMenu, i - 1),
                                          pstr,
                                          sizeof(pstr));
                    

                    WSTRLCAT(pwsz,pstr,sizeof(pwsz));
                    IMENUCTL_SetItemText(pMenu, IMENUCTL_GetItemID(pMenu, i - 1), NULL, NULL, pwsz);
                }
                
                IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL);
#else
				IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY|MP_ACTIVE_NO_REDRAW);
#endif
				
                IMENUCTL_SetOemProperties( pMenu, OEMMP_USE_MENU_STYLE);
                IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
                IMENUCTL_SetSel(pMenu, pMe->m_MainSel);
                (void) ISHELL_PostEvent(pMe->m_pShell, AEECLSID_MAIN_MENU, EVT_USER_REDRAW,0,0);
            }
            return TRUE;
            
        case EVT_USER_REDRAW:
            (void)IMENUCTL_Redraw(pMenu);
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
						boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_MULTIMEDIA_LIST,EVT_KEY,AVK_SELECT,0);
						return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)   && wXPos < rc.x + (rc.dx/3)*2 )//左
					{
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_MULTIMEDIA_LIST,EVT_KEY,AVK_INFO,0);
						 return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
					{						
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_MULTIMEDIA_LIST,EVT_KEY,AVK_CLR,0);
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
}




/*=============================================================================
FUNCTION:  StartApplet

DESCRIPTION:  启动applet

PARAMETERS:  如果APPLET 有变动，只需改动次函数

=============================================================================*/
static int StartApplet(Multimed *pMe, int i)
{
    int Result = EUNSUPPORTED;
    switch(i)
    {
#if defined (FEATURE_VERSION_W208S)||defined (FEATURE_VERSION_W516) || defined(FEATURE_VERSION_W027)||defined(FEATURE_VERSION_C11)
		case IDS_MULTIMEDIA_VIDEO_CAMERA:
			Result = ISHELL_StartAppletArgs(pMe->m_pShell, AEECLSID_APP_CAMERA, "record");
            break;
#endif
        case IDS_MULTIMEDIA_CAMERA:
            Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APP_CAMERA);
            break;
        #ifndef FEATURE_VERSION_C180
        case IDS_MULTIMEDIA_FMRADIO:
            Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APP_FMRADIO);
            break;
        #endif
        case IDS_MULTIMEDIA_MUSICPLAYER:
            Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APP_MUSICPLAYER);
            break;
		#ifdef FEATURE_VERSION_C337
		case IDS_MULTIMEDIA_SOUND_RECORDER:
		#endif
        #ifdef FEATURE_VERSION_W317A
        case IDS_MULTIMEDIA_AUDIORECORDER:
        #endif
        case IDS_MULTIMEDIA_RECORDER:
            Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_RECORDER);
            break;
#ifdef FEATURE_APP_MPEG4
		case IDS_MULTIMEDIA_VIDEOPLAYER:
			Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_VIDEOPLAYER);
            break;
#endif
#ifdef FEATURE_ANALOG_TV
        case IDS_MULTIMEDIA_TV:
            Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_TVAPP);
            break;
#endif
#if defined(FEATURE_VERSION_VG68)||defined(FEATURE_VERSION_C01)
        case IDS_MULTIMEDIA_MEDIAGALLERY:
            Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_MEDIAGALLERY);
            break;
#endif
		default:
			break;
        
    }
    return Result;
}


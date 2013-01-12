/** ----------------------------------------------------------------------------
 *MobileTracker.c
 *.h
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

#include "AppComFunc.h"
#include "AEEShell.h"
#include "AEE_OEM.h"
#include "AEEModGen.h"
#include "AEEModTable.h"
#include "AEEMenu.h"
#include "AEEStdLib.h"
#include "AEEAnnunciator.h"
#include "MobileTracker.h"
#include "MobileTracker_priv.h" 

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
static MobileTrackerMod gMobileTrackerMod;
static const VTBL( IModule) gModFuncs =
{
    MobileTrackerMod_AddRef,
    MobileTrackerMod_Release,
    MobileTrackerMod_CreateInstance,
    MobileTrackerMod_FreeResources
};

static uint32  MobileTracker_AddRef( IMobileTracker *pi);

static uint32  MobileTracker_Release ( IMobileTracker *pi);

static boolean MobileTracker_HandleEvent( IMobileTracker *pi,
    AEEEvent eCode,
    uint16  wParam,
    uint32 dwParam
);




static MobileTracker gMobileTracker={0};

static const VTBL( IMobileTracker) gMobileTrackerMethods =
{
    MobileTracker_AddRef,
    MobileTracker_Release,
    MobileTracker_HandleEvent,
};

int MobileTrackerMod_Load( IShell *pIShell, void *ph, IModule **ppMod);

int MobileTrackerMod_New( int16 nSize,
    IShell              *pIShell,
    void                *ph,
    IModule             **ppMod,
    PFNMODCREATEINST    pfnMC,
    PFNFREEMODDATA      pfnMF
);

static int MobileTracker_New( IShell *ps, IModule *pIModule, IMobileTracker **ppObj);

static int CMobileTracker_InitAppData(MobileTracker *pMe);

static void CMobileTracker_FreeAppData(MobileTracker *pMe);

static void MobileTracker_RunFSM(MobileTracker *pMe);


boolean MobileTracker_RouteDialogEvt(MobileTracker *pMe,
    AEEEvent    eCode,
    uint16  wParam,
    uint32 dwParam
);

NextFSMAction MobileTracker_ProcessState(MobileTracker *pMe);

void MobileTracker_ShowDialog(MobileTracker  *pMe,  uint16 dlgResId);

// MAINST_MAIN 状态处理函数
static NextFSMAction MAINST_MOBILETRACKER_Handler(MobileTracker *pMe);
static NextFSMAction MANINPWD_INPUTPWD_Handler(MobileTracker *pMe);

static NextFSMAction STATE_ENABLE_Handler(MobileTracker *pMe);
static NextFSMAction STATE_D_NUMBER_Handler(MobileTracker *pMe);
static NextFSMAction STATE_VIEW_IMSI_Handler(MobileTracker *pMe);
static NextFSMAction STATE_OWER_Handler(MobileTracker *pMe);
static NextFSMAction STATE_CHAGNEPASSWOWRD_Handler(MobileTracker *pMe);
static NextFSMAction STATE_CONTENT_Handler(MobileTracker *pMe);


//MAINST_EXIT  状态处理函数
static NextFSMAction MOBILETRACKER_EXIT_Handler(MobileTracker *pMe);
static boolean MobileTracker_ListMenuHandler(MobileTracker *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam);
static boolean MobileTracker_PassWordHandler(MobileTracker *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam);
static void MobileTracker_ShowMsgBoxDlg(MobileTracker *pMe, uint16 nDlgResId);
static __inline void MTAppUtil_DrawSoftkey(MobileTracker* pMe,
                                       BottomBar_e_Type eSoftkeyType);
static boolean MobileTracker_PopMsgHandler(MobileTracker *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam);
static boolean MobileTracker_EanbleHandler(MobileTracker *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam);
static boolean MobileTracker_ViewimsiHandler(MobileTracker *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam);	
static boolean MobileTracker_DnumberHandler(MobileTracker *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam);
static boolean MobileTracker_OwernameHandler(MobileTracker *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam);
static boolean MobileTracker_ChangPassWordHandler(MobileTracker *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam);
static boolean MobileTracker_ContentHandler(MobileTracker *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam);
static void MobileTracker_MsgBoxTimeout(void* pUser);
//static NextFSMAction MANINPWD_INPUTPWDINVAD_Handler(Multimed *pMe);
void MobileTracker_ShowPopDialog(MobileTracker  *pMe,  uint16 dlgResId);


/*==============================================================================
                              
                              本地（静态）数据
                              
==============================================================================*/




/*=============================================================================
FUNCTION:  MobileTrackerMod_Load

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int MobileTrackerMod_Load( IShell *pIShell, void *ph, IModule **ppMod)
{
    return MobileTrackerMod_New( sizeof(MobileTrackerMod),
                    pIShell,
                    ph,
                    ppMod,
                    NULL,
                    NULL);
}

/*=============================================================================
FUNCTION:  MobileTrackerMod_New

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int  MobileTrackerMod_New( int16 nSize,
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

    MEMSET( &gMobileTrackerMod, 0, sizeof(MobileTrackerMod));

    INIT_VTBL( &gMobileTrackerMod, IModule, gModFuncs);
    gMobileTrackerMod.referenceCounter = 1;
    *ppMod = ( IModule *)&gMobileTrackerMod;
    return AEE_SUCCESS;
}

/*=============================================================================
FUNCTION:  MobileTrackerMod_AddRef

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32 MobileTrackerMod_AddRef( IModule *po)
{
    return( ++( ( MobileTrackerMod *)po)->referenceCounter);
}

/*=============================================================================
FUNCTION:  MobileTrackerMod_Release

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32 MobileTrackerMod_Release( IModule *po)
{
    if ( ( ( MobileTrackerMod *) po)->referenceCounter == 0)
    {
        
        return 0;
    }
    return( --( ( MobileTrackerMod *)po)->referenceCounter);
}

/*=============================================================================
FUNCTION:  MobileTrackerMod_CreateInstance

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int  MobileTrackerMod_CreateInstance( IModule *po,
    IShell *pIShell,
    AEECLSID ClsId,
    void **ppObj
)
{
    MSG_FATAL("MobileTrackerMod_CreateInstance Start",0,0,0);
    *ppObj = NULL;

    if( ClsId != AEECLSID_MOBILETRACKER || MobileTracker_New( pIShell, po, ( IMobileTracker**)ppObj) != SUCCESS)
    {
        return EFAILED;
    }
    MSG_FATAL("MobileTrackerMod_CreateInstance End",0,0,0);
    return SUCCESS;
}

/*=============================================================================
FUNCTION:  MobileTrackerMod_FreeResources

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void MobileTrackerMod_FreeResources( IModule *po, IHeap *ph, IFileMgr *pfm)
{
}

/*=============================================================================
FUNCTION:  MobileTracker_New

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int MobileTracker_New( IShell *ps, IModule *pIModule, IMobileTracker **ppObj)
{
    int retVal = SUCCESS;
    MSG_FATAL("MobileTracker_New Start",0,0,0);
    if( 0 == gMobileTracker.referenceCounter)
    {

        if( pIModule == NULL || ps == NULL)
        {
            return EFAILED;
        }

        MEMSET(&gMobileTracker, 0, sizeof(gMobileTracker));
        INIT_VTBL(&gMobileTracker, IMobileTracker, gMobileTrackerMethods);

       
        gMobileTracker.m_pShell     = ps;
        gMobileTracker.m_pModule    = pIModule;
        retVal = CMobileTracker_InitAppData(&gMobileTracker);
        MSG_FATAL("MobileTracker_New retVal=%d",retVal,0,0);
        if(retVal != SUCCESS)
        {
            CMobileTracker_FreeAppData((MobileTracker*)&gMobileTracker);
            return retVal;
        }
        (void) ISHELL_AddRef( ps);
        (void) IMODULE_AddRef( pIModule);
        gMobileTracker.referenceCounter  = 0;

        
    }

    ++ gMobileTracker.referenceCounter;
    *ppObj = ( IMobileTracker*)&gMobileTracker;
    MSG_FATAL("MobileTracker_New retVal=%d",retVal,0,0);
    return retVal;
}

/*==============================================================================
函数:
    CMobileTracker_InitAppData

说明:
    初始化 MobileTracker Applet 结构数据缺省值。

参数:
    pMe [in]: 指向 MobileTracker Applet 对象结构的指针。该结构包含小程序的特定信息。

返回值:
    EFAILED: 初始化失败。
    SUCCESS: 初始化成功。

备注:

==============================================================================*/
static int CMobileTracker_InitAppData(MobileTracker *pMe)
{
	
    if (NULL == pMe)
    {
        return EFAILED;
    }
	
   	pMe->m_strPhonePWD = NULL;
	pMe->b_pwdWright = FALSE;
	
    pMe->m_MainSel  = 0;
    
    if (ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_DISPLAY, 
            (void **) &pMe->m_pDisplay) != SUCCESS)
    {
        MSG_FATAL("CMobileTracker_InitAppData EFAILED",0,0,0);
        return EFAILED;
    }
    
	if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,AEECLSID_ANNUNCIATOR,(void **)&pMe->m_pIAnn))
    {
        MSG_FATAL("CMobileTracker_InitAppData EFAILED",0,0,0);
        return EFAILED;
    }
    return SUCCESS;
}

/*==============================================================================
函数:
    CMobileTracker_FreeAppData

说明:
    释放 MobileTracker Applet 相关资源。

参数:
    pMe [in]: MobileTracker Applet 对象结构的指针。该结构包含小程序的特定信息。

返回值:
    none

备注:

==============================================================================*/
static void CMobileTracker_FreeAppData(MobileTracker *pMe)
{
    if (NULL == pMe)
    {
        return;
    }

	pMe->b_pwdWright = FALSE;
	pMe->m_strPhonePWD = NULL;
    pMe->m_eAppStatus = MOBILETRACKER_STOP; 
    
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
FUNCTION:  MobileTracker_AddRef

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32  MobileTracker_AddRef( IMobileTracker *pi)
{
    return ( ++gMobileTracker.referenceCounter);
}

/*=============================================================================
FUNCTION:  MobileTracker_Release

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32  MobileTracker_Release( IMobileTracker *pi)
{
    register MobileTracker *pMe = (MobileTracker*)pi;
   
    if (pMe->referenceCounter == 0)
    {
        return 0;
    }
    
    if( --pMe->referenceCounter)
    {
        return pMe->referenceCounter;
    }


    CMobileTracker_FreeAppData(pMe);

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
static void MobileTracker_RunFSM(MobileTracker *pMe)
{
    NextFSMAction nextFSMAction = NFSMACTION_WAIT;
    for ( ; ; )
    {
        nextFSMAction = MobileTracker_ProcessState(pMe);

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
      MobileTracker_RouteDialogEvt
说明:
       函数将BREW事件路由给当前活动对话框的事件处理函数。

参数:
       pMe [In]: 指向MobileTracker Applet对象结构的指针,该结构包含小程序的特定信息.
       eCode [in]：事件代码。
       wParam [in]：与事件eCode关联的数据。
       dwParam [in]：与事件eCode关联的数据。

返回值:
       TRUE：传入事件被处理。
       FALSE：传入事件没被处理。

备注:

==============================================================================*/
boolean MobileTracker_RouteDialogEvt(MobileTracker *pMe,
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

        case IDD_LIST_MOBILETRACKER_DIALOGS:
            return MobileTracker_ListMenuHandler(pMe, eCode, wParam, dwParam);
		case IDD_PWD:
			return MobileTracker_PassWordHandler(pMe, eCode, wParam, dwParam);
		case IDD_PWD_INVAD:
			return MobileTracker_PopMsgHandler(pMe, eCode, wParam, dwParam);  
		case IDD_DIS_EANABLE:
			return MobileTracker_EanbleHandler(pMe, eCode, wParam, dwParam);  
		case IDD_VIEW_IMSI:
			return MobileTracker_ViewimsiHandler(pMe, eCode, wParam, dwParam);  
		case IDD_D_NUMBER:
			return MobileTracker_DnumberHandler(pMe, eCode, wParam, dwParam);  
		case IDD_OWER_NAME:
			return MobileTracker_OwernameHandler(pMe, eCode, wParam, dwParam);  
		case IDD_CHANGEPASS_WORD:
			return MobileTracker_ChangPassWordHandler(pMe, eCode, wParam, dwParam);  
		case IDD_CONTENT:
			return MobileTracker_ContentHandler(pMe, eCode, wParam, dwParam);  
		default:
            return FALSE;
    }
}


/*=============================================================================
FUNCTION:  MobileTracker_HandleEvent

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean MobileTracker_HandleEvent( IMobileTracker *pi,
    AEEEvent eCode,
    uint16  wParam,
    uint32 dwParam
)
{
    MobileTracker *pMe = (MobileTracker*)pi;
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
            pMe->m_currState  = MOBILETRACKERIAST_MAIN;
            pMe->m_eDlgReturn = DLGRET_CREATE;
            pMe->m_eAppStatus = MOBILETRACKER_RUNNING;
            if(pMe->m_pIAnn != NULL)
            {
                IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE); 
            }
            MobileTracker_RunFSM(pMe);
            return TRUE;

        case EVT_APP_STOP:
            {
                if (pMe->m_pDisplay != NULL)
                {
                    (void) IDISPLAY_Release(pMe->m_pDisplay);
                    pMe->m_pDisplay = NULL;
                }
                pMe->m_eAppStatus = MOBILETRACKER_STOP;
                
                return TRUE;
            }

        case EVT_APP_SUSPEND:
          
            pMe->m_eAppStatus = MOBILETRACKER_SUSPEND;
            
            return TRUE;
        

        case EVT_APP_RESUME:
            {
                AEEAppStart* as = ( AEEAppStart*)dwParam;
            
                pMe->m_rc    = as->rc;
                pMe->m_rc.dy = di.cyScreen;
                pMe->m_eAppStatus = MOBILETRACKER_RUNNING;
                MobileTracker_RunFSM(pMe); 
                return TRUE;
            }

        case EVT_DIALOG_INIT:
           
            pMe->m_pActiveIDlg = (IDialog*)dwParam;
            pMe->m_pActivedlgID = wParam;
            
            return MobileTracker_RouteDialogEvt(pMe, eCode, wParam, dwParam);

        case EVT_DIALOG_START:
            
            return MobileTracker_RouteDialogEvt(pMe, eCode, wParam, dwParam);

        case EVT_USER_REDRAW:
            return MobileTracker_RouteDialogEvt(pMe, eCode, wParam, dwParam);

        case EVT_DIALOG_END:
		 
            (void)MobileTracker_RouteDialogEvt(pMe, eCode, wParam, dwParam);
            pMe->m_pActiveIDlg = NULL;
            pMe->m_pActivedlgID = 0;
            if(pMe->m_eAppStatus == MOBILETRACKER_RUNNING)
            {
                MobileTracker_RunFSM(pMe);
            }
            return TRUE;
			
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
            return MobileTracker_RouteDialogEvt(pMe, eCode, wParam, dwParam);

        case EVT_KEY_PRESS:
        case EVT_KEY_RELEASE:
        case EVT_COMMAND:
            return MobileTracker_RouteDialogEvt(pMe, eCode, wParam, dwParam); 
        default:
            return MobileTracker_RouteDialogEvt(pMe, eCode, wParam, dwParam);
    }
}

/*==============================================================================
函数:
   MobileTracker_ProcessState

说明:
    根据 pMe->m_currState, 调用相应的状态处理函数。

参数:
    pMe [in]: 指向Game Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
NextFSMAction MobileTracker_ProcessState(MobileTracker *pMe)
{
    NextFSMAction retVal = NFSMACTION_WAIT;
    
    if (NULL == pMe)
    {
        return retVal;
    }
    MSG_FATAL("MobileTracker_ProcessState=%d",pMe->m_currState,0,0);
    switch(pMe->m_currState)
    {
        case MOBILETRACKERIAST_MAIN:
            return MAINST_MOBILETRACKER_Handler(pMe);
        case MULTIMEDIAST_EXIT:
            return MOBILETRACKER_EXIT_Handler(pMe);    
		case STATE_PWD:
			return MANINPWD_INPUTPWD_Handler(pMe);
		case STATE_ENABLE:
			return STATE_ENABLE_Handler(pMe);
        case STATE_D_NUMBER:
			return STATE_D_NUMBER_Handler(pMe);
        case STATE_VIEW_IMSI:
			return STATE_VIEW_IMSI_Handler(pMe);
        case STATE_OWER:
			return STATE_OWER_Handler(pMe);
        case STATE_CHAGNEPASSWOWRD:
			return STATE_CHAGNEPASSWOWRD_Handler(pMe);
		case STATE_CONTENT:
			return STATE_CONTENT_Handler(pMe);
        default:
            break;

    }
    return retVal;
}

/*==============================================================================
函数:
    MAINST_MOBILETRACKER_Handler

说明:
   GAMEST_MAIN 状态处理函数。

参数:
    pMe [in]: 指向GAME Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction MAINST_MOBILETRACKER_Handler(MobileTracker *pMe)
{
    //boolean locksel = TRUE;
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
	MSG_FATAL("MAINST_MOBILETRACKER_Handler....",0,0,0);
	//OEM_GetConfig(CFGI_MOBILETRACKER_LOCK_CHECK, &locksel, sizeof( locksel));
	if((!pMe->b_pwdWright))
	{
		MOVE_TO_STATE(STATE_PWD);
	}
	else
	{
		MSG_FATAL("MAINST_MOBILETRACKER_Handler.pMe->m_eDlgReturn=%d",pMe->m_eDlgReturn,0,0);
    	switch (pMe->m_eDlgReturn)
    	{		
        	// 进入主界面
        	case DLGRET_CREATE:
            	MobileTracker_ShowDialog(pMe, IDD_LIST_MOBILETRACKER_DIALOGS);
            	return NFSMACTION_WAIT;
            case DLGRET_ENABLE:
				MOVE_TO_STATE(STATE_ENABLE)
            	return NFSMACTION_CONTINUE;
			case DLGRET_D_NUMBER:
				MOVE_TO_STATE(STATE_D_NUMBER)
            	return NFSMACTION_CONTINUE;
			case DLGRET_VIEW_IMSI:
				MOVE_TO_STATE(STATE_VIEW_IMSI)
            	return NFSMACTION_CONTINUE;
			case DLGRET_OWER:
				MOVE_TO_STATE(STATE_OWER)
            	return NFSMACTION_CONTINUE;
			case DLGRET_CHAGNEPASSWOWRD:
				MOVE_TO_STATE(STATE_CHAGNEPASSWOWRD)
            	return NFSMACTION_CONTINUE;
			case DLGRET_CONTENT:
				MOVE_TO_STATE(STATE_CONTENT)
            	return NFSMACTION_CONTINUE;
        	default:
            	MOVE_TO_STATE(MULTIMEDIAST_EXIT)
            	return NFSMACTION_CONTINUE;
    	}
	}
}


static NextFSMAction MANINPWD_INPUTPWD_Handler(MobileTracker *pMe)
{
	MSG_FATAL("MANINPWD_INPUTPWD_Handler....==%d",pMe->m_eDlgReturn,0,0);
	switch(pMe->m_eDlgReturn)
    {
        case DLGRET_CREATE:
            MobileTracker_ShowDialog(pMe, IDD_PWD);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
			{
				boolean m_MobileTracker = TRUE; 
				OEM_SetConfig(CFGI_MOBILE_TRACKER_BACK,&m_MobileTracker,sizeof(boolean));
            	MOVE_TO_STATE(MULTIMEDIAST_EXIT);
            	return NFSMACTION_CONTINUE;
        	}

        case MGDLGRET_PASS:            
			MOVE_TO_STATE(MOBILETRACKERIAST_MAIN);
			return NFSMACTION_CONTINUE;

        case MGDLGRET_FAILD:    
			MSG_FATAL("MGDLGRET_FAILD.............",0,0,0);
			pMe->m_Rturnpass = FALSE;
			MobileTracker_ShowPopDialog(pMe, IDD_PWD_INVAD);
            return NFSMACTION_WAIT;
		
        case MGDLGRET_MSGBOX_OK:
            MOVE_TO_STATE(STATE_PWD);
            return NFSMACTION_CONTINUE;        

        default:
            MOVE_TO_STATE(MULTIMEDIAST_EXIT);
            return NFSMACTION_CONTINUE;
    }
	return NFSMACTION_CONTINUE;
}

static NextFSMAction STATE_ENABLE_Handler(MobileTracker *pMe)
{
	if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
	switch(pMe->m_eDlgReturn)
    {
        case DLGRET_CREATE:
            MobileTracker_ShowDialog(pMe, IDD_DIS_EANABLE);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
            MOVE_TO_STATE(MOBILETRACKERIAST_MAIN);
            return NFSMACTION_CONTINUE;
	}
}
static NextFSMAction STATE_D_NUMBER_Handler(MobileTracker *pMe)
{
	if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
	switch(pMe->m_eDlgReturn)
    {
        case DLGRET_CREATE:
            MobileTracker_ShowDialog(pMe, IDD_D_NUMBER);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
            MOVE_TO_STATE(MOBILETRACKERIAST_MAIN);
            return NFSMACTION_CONTINUE;
	}
}
static NextFSMAction STATE_VIEW_IMSI_Handler(MobileTracker *pMe)
{
	if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
	switch(pMe->m_eDlgReturn)
    {
        case DLGRET_CREATE:
            MobileTracker_ShowDialog(pMe, IDD_VIEW_IMSI);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
            MOVE_TO_STATE(MOBILETRACKERIAST_MAIN);
            return NFSMACTION_CONTINUE;
	}
}
static NextFSMAction STATE_OWER_Handler(MobileTracker *pMe)
{
	if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
	switch(pMe->m_eDlgReturn)
    {
        case DLGRET_CREATE:
            MobileTracker_ShowDialog(pMe, IDD_OWER_NAME);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
            MOVE_TO_STATE(MOBILETRACKERIAST_MAIN);
            return NFSMACTION_CONTINUE;
	}
}

static NextFSMAction STATE_CONTENT_Handler(MobileTracker *pMe)
{
	if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
	MSG_FATAL("STATE_CONTENT_Handler.............",0,0,0);
	switch(pMe->m_eDlgReturn)
    {
        case DLGRET_CREATE:
            MobileTracker_ShowDialog(pMe, IDD_CONTENT);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
            MOVE_TO_STATE(MOBILETRACKERIAST_MAIN);
            return NFSMACTION_CONTINUE;
	}
}

static NextFSMAction STATE_CHAGNEPASSWOWRD_Handler(MobileTracker *pMe)
{
	if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
	switch(pMe->m_eDlgReturn)
    {
        case DLGRET_CREATE:
            MobileTracker_ShowDialog(pMe, IDD_CHANGEPASS_WORD);
            return NFSMACTION_WAIT;
		case MGDLGRET_FAILD:    
			MSG_FATAL("MGDLGRET_FAILD.............",0,0,0);
			pMe->m_Rturnpass = TRUE;
			MobileTracker_ShowPopDialog(pMe, IDD_PWD_INVAD);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
            MOVE_TO_STATE(MOBILETRACKERIAST_MAIN);
            return NFSMACTION_CONTINUE;
	}
}


static void MobileTracker_ShowMsgBoxDlg(MobileTracker *pMe, uint16 nDlgResId)
{
	int nRet;

   if(!pMe)
   {
      return ;
   }
   MobileTracker_ShowDialog(pMe, nDlgResId);

}

/*==============================================================================
函数:
    MOBILETRACKER_EXIT_Handler

说明:
    GAMEST_EXIT 状态处理函数。

参数:
    pMe [in]: 指向GAME Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction MOBILETRACKER_EXIT_Handler(MobileTracker *pMe)
{
    (void) ISHELL_CloseApplet(pMe->m_pShell, FALSE);
    return NFSMACTION_WAIT;
} 
/*
       MobileTracker_ShowDialog

说明:
    函数由状态处理函数用来唤起相应对话框资源ID的对话框。

参数:
    pMe [in]: 指向 MAINMENU Applet 对象结构的指针。该结构包含小程序的特定信息。
    dlgResId [in]: 对话框资源ID，函数根据该ID创建对话框。

返回值:
    none

备注:

==============================================================================*/
void MobileTracker_ShowDialog(MobileTracker  *pMe,  uint16 dlgResId)
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
	
    nRet = ISHELL_CreateDialog(pMe->m_pShell,MOBILETRACKER_RES_FILE_LANG,dlgResId,NULL);
    if (nRet != SUCCESS)
    {
    }
    
}

void MobileTracker_ShowPopDialog(MobileTracker  *pMe,  uint16 dlgResId)
{
   int nRet;
   if(!pMe)
   {
      return ;
   }
   /*At most one dialog open at once*/
   if(NULL != ISHELL_GetActiveDialog(pMe->m_pShell))
   {
      /*Looks like there is one dialog already opened.
       * Flag an error  return without doing anything.
       */
      return ;
   }
   nRet = ISHELL_CreateDialog(pMe->m_pShell,
                              MOBILETRACKER_RES_FILE_LANG,
                              dlgResId,
                              NULL);
   if(nRet != SUCCESS)
   {
   }
}

/*=============================================================================
FUNCTION:  MobileTracker_ListMenuHandler

DESCRIPTION:   列表菜单

PARAMETERS:

=============================================================================*/

static boolean MobileTracker_ListMenuHandler(MobileTracker *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
    PARAM_NOT_REF(dwParam)
    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveIDlg,IDC_MOBILETRACKER_LIST);
    MSG_FATAL("MobileTracker_ListMenuHandler==%d,,==%x",pMe->m_MainSel,eCode,0);
    if (pMenu == NULL)
    {
        return FALSE;
    }

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
		    {
		  		AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        MOBILETRACKER_RES_FILE_LANG,                                
                        IDS_MOBILETRACKER_TITLE,
                        WTitle,
                        sizeof(WTitle));
                if(pMe->m_pIAnn != NULL)
                {
				    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
                }
				IMENUCTL_AddItem(pMenu, MOBILETRACKER_RES_FILE_LANG,IDS_MOBILETRAXKER_ONOFF, IDS_MOBILETRAXKER_ONOFF, NULL, 0);
				IMENUCTL_AddItem(pMenu, MOBILETRACKER_RES_FILE_LANG,IDS_MOBILETRACKER_DESIGNATENUMBER, IDS_MOBILETRACKER_DESIGNATENUMBER, NULL, 0);
				IMENUCTL_AddItem(pMenu, MOBILETRACKER_RES_FILE_LANG,IDS_MOBILETRACKER_VIEW_IMSI, IDS_MOBILETRACKER_VIEW_IMSI, NULL, 0);
				//IMENUCTL_AddItem(pMenu, MOBILETRACKER_RES_FILE_LANG,IDS_MOBILETRACKER_OWNERNAME, IDS_MOBILETRACKER_OWNERNAME, NULL, 0);
				IMENUCTL_AddItem(pMenu, MOBILETRACKER_RES_FILE_LANG,IDS_CONTENT, IDS_CONTENT, NULL, 0);
				IMENUCTL_AddItem(pMenu, MOBILETRACKER_RES_FILE_LANG,IDS_MOBILETRAXKER_PWDCHG, IDS_MOBILETRAXKER_PWDCHG, NULL, 0);
		    }
            return TRUE;
            
        case EVT_DIALOG_START:
            {  
                int i;
                for (i=1;i<=IMENUCTL_GetItemCount(pMenu);i++)
                {
                    AECHAR pwsz[67] = {0};
                    AECHAR pstr[64] = {0};
                    AECHAR wsFmt[5] = {0};
    
                    (void)STRTOWSTR("%d. ",wsFmt,sizeof(wsFmt));
                    WSPRINTF(pwsz,sizeof(pwsz),wsFmt,i);
                    
                    ISHELL_LoadResString( pMe->m_pShell,
                                          MOBILETRACKER_RES_FILE_LANG,
                                          IMENUCTL_GetItemID(pMenu, i - 1),
                                          pstr,
                                          sizeof(pstr));
                    

                    WSTRLCAT(pwsz,pstr,sizeof(pwsz));
                    IMENUCTL_SetItemText(pMenu, IMENUCTL_GetItemID(pMenu, i - 1), NULL, NULL, pwsz);
                }
                
                IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL);

				
                IMENUCTL_SetOemProperties( pMenu, OEMMP_USE_MENU_STYLE);
                IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
                IMENUCTL_SetSel(pMenu, pMe->m_MainSel);
                (void) ISHELL_PostEvent(pMe->m_pShell, AEECLSID_MOBILETRACKER, EVT_USER_REDRAW,0,0);
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
                case AVK_0:
                case AVK_STAR:
                case AVK_POUND:
                    return TRUE;
                    
                case AVK_CLR:
					{
						boolean m_MobileTracker = TRUE; 
						OEM_SetConfig(CFGI_MOBILE_TRACKER_BACK,&m_MobileTracker,sizeof(boolean));
                    	CLOSE_DIALOG(DLGRET_CANCELED)
                    	return TRUE;
                	}
                    
                default:
                    break;
          
            }
            return TRUE;
            
        case EVT_COMMAND:
			pMe->m_MainSel = wParam;
			switch(wParam)
			{
				case IDS_MOBILETRAXKER_ONOFF:
					CLOSE_DIALOG(DLGRET_ENABLE)
					break;
				case IDS_MOBILETRACKER_DESIGNATENUMBER:
					CLOSE_DIALOG(DLGRET_D_NUMBER)
					break;
				case IDS_MOBILETRACKER_VIEW_IMSI:
					CLOSE_DIALOG(DLGRET_VIEW_IMSI)
					break;
				case IDS_MOBILETRACKER_OWNERNAME:
					CLOSE_DIALOG(DLGRET_OWER)
					break;
				case IDS_MOBILETRAXKER_PWDCHG:
					CLOSE_DIALOG(DLGRET_CHAGNEPASSWOWRD)
					break;
				case IDS_CONTENT:
					CLOSE_DIALOG(DLGRET_CONTENT)
					break;
			}
			
            return TRUE;
        default:
            break;
    }             
    return FALSE;
}

static void MobileTracker_MsgBoxTimeout(void* pUser)
{
   MobileTracker* pMe = (MobileTracker*)pUser;
   MSG_FATAL("MobileTracker_MsgBoxTimeout Start",0,0,0);
   if(NULL == pMe )
   {
      return;
   }


   ISHELL_PostEvent(pMe->m_pShell,
                       AEECLSID_MOBILETRACKER,
                       EVT_DISPLAYDIALOGTIMEOUT,
                       0,
                       0);

}//MediaGalleryApp_MsgBoxTimeout

static boolean MobileTracker_EanbleHandler(MobileTracker *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
   PARAM_NOT_REF(dwParam)
   IMenuCtl  *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveIDlg,
                                                    IDC_EANABLE);
   boolean moblile_lock = FALSE;  
   if(!pMe)
   {
      return FALSE;
   }
      switch(eCode)
   {
	   case EVT_DIALOG_INIT:
	      {
	         AECHAR WTitle[40] = {0};
			 (void)ISHELL_LoadResString(pMe->m_pShell,
                        MOBILETRACKER_RES_FILE_LANG,                                
                        IDS_MOBILETRAXKER_ONOFF,
                        WTitle,
                        sizeof(WTitle));
             if(pMe->m_pIAnn != NULL)
             {
			    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
             }

          
            IMENUCTL_AddItem(pMenu, MOBILETRACKER_RES_FILE_LANG, IDS_ENABLE_ON, IDS_ENABLE_ON, NULL, 0);           
            IMENUCTL_AddItem(pMenu, MOBILETRACKER_RES_FILE_LANG, IDS_EANBLE_OFF, IDS_EANBLE_OFF, NULL, 0);
	         return TRUE;
	      }
	   case EVT_DIALOG_START:
	      {
		  	 uint16 nSelItem = 0;
		  	 OEM_GetConfig(CFGI_MOBILETRACKER_LOCK_CHECK,
                                       &moblile_lock,
                                       sizeof(moblile_lock));
			 if(moblile_lock)
			 {
			 	nSelItem = IDS_ENABLE_ON;
			 }
			 else
			 {
			 	nSelItem = IDS_EANBLE_OFF;
			 }

			 MSG_FATAL("EVT_DIALOG_STARTmoblile_lock====%d",moblile_lock,0,0);
			 InitMenuIcons(pMenu);
             SetMenuIcon(pMenu, nSelItem, TRUE);
             IMENUCTL_SetSel(pMenu, nSelItem);

			 IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_TEXT_ALIGN_LEFT_ICON_ALIGN_RIGHT);
             IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);
             IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);


	         ISHELL_PostEvent(pMe->m_pShell, AEECLSID_MOBILETRACKER,
	                          EVT_USER_REDRAW, 0, 0);
	         return TRUE;
	      }
	   case EVT_USER_REDRAW:
	      {
		  	return TRUE;
	   	  }
	   case EVT_DIALOG_END:
		 {
		  	return TRUE;
	   	 }
	   case EVT_COMMAND:
	      {
		  		uint16 num = 0;
				AEEMobileInfo     mi;
				uint16 wImsiData[OEMNV_LOCKIMSI_MAXLEN] = {0};
		  		GetMobileInfo(&mi);
		  		switch(wParam)
		  		{
                	case IDS_ENABLE_ON:
						MSG_FATAL("IDS_ENABLE_ON...........",0,0,0);
   						moblile_lock = TRUE;
                    	break;

                	case IDS_EANBLE_OFF:    
						MSG_FATAL("IDS_ENABLE_OFF...........",0,0,0);
						moblile_lock = FALSE;
                	    break;

		  		}
				MSG_FATAL("EVT_COMMANDmoblile_lock====%d",moblile_lock,0,0);
				
				(void) STRTOWSTR(mi.szMobileID, wImsiData, sizeof(wImsiData));
				OEM_SetConfig(CFGI_MOBILE_TRACKER_IMSI,wImsiData,sizeof(wImsiData));
				
				OEM_SetConfig(CFGI_MOBILETRACKER_LOCK_CHECK,
                                       &moblile_lock,
                                       sizeof(moblile_lock));
				InitMenuIcons(pMenu);
             	SetMenuIcon(pMenu, wParam, TRUE);
				IMENUCTL_SetSel(pMenu, wParam);
				CLOSE_DIALOG(DLGRET_CANCELED)
			    return TRUE;
	      }

	   case EVT_KEY:
	      {
		  	switch(wParam)
            {
            	case AVK_INFO:
				case AVK_SELECT:
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;

                default:
                    break;
            }
            return TRUE;
	   	  }
	   default:
	      break;
    }
}
static boolean MobileTracker_ViewimsiHandler(MobileTracker *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
	
   PARAM_NOT_REF(dwParam)
   static IStatic * pStatic = NULL;
   if(!pMe)
   {
      return FALSE;
   }
   
   if (NULL == pStatic)
   {
		   AEERect rc = {0};
   
		   if (ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_STATIC,
				   (void**)&pStatic) == SUCCESS)
		   {
			   ISTATIC_SetRect(pStatic, &rc);
		   }
		   else
		   {
			   return FALSE;
		   }
   }
   
   switch(eCode)
   {
	   case EVT_DIALOG_INIT:
	      {
	         AECHAR WTitle[40] = {0};
			 (void)ISHELL_LoadResString(pMe->m_pShell,
                        MOBILETRACKER_RES_FILE_LANG,                                
                        IDS_MOBILETRACKER_VIEW_IMSI,
                        WTitle,
                        sizeof(WTitle));
             if(pMe->m_pIAnn != NULL)
             {
			    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
             }
	         return TRUE;
	      }
	   case EVT_DIALOG_START:
	      {
	         ISHELL_PostEvent(pMe->m_pShell, AEECLSID_MOBILETRACKER,
	                          EVT_USER_REDRAW, 0, 0);
	         return TRUE;
	      }
	   case EVT_USER_REDRAW:
	      {
		  		AECHAR  wstrText[240]={0};
				PromptMsg_Param_type  Msg_Param={0};
				BottomBar_Param_type bottomParam;
				uint64 meid = 0;
				uint32 H32,L32;
				AECHAR	fmt_str[20];
				AEEMobileInfo     mi;
				GetMobileInfo(&mi);
				STRTOWSTR(mi.szMobileID,wstrText,sizeof(wstrText));
				IDISPLAY_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, RGB_WHITE);
				ISTATIC_SetProperties(pStatic, ST_CENTERTEXT|ST_MIDDLETEXT); 
				ISTATIC_SetFontColor(pStatic, RGB_BLACK);		//Add By zzg 2011_12_07
        		(void)ISTATIC_SetText(pStatic, 
                              NULL, 
                              wstrText, 
                              AEE_FONT_NORMAL, 
                              AEE_FONT_NORMAL);
        		(void)ISTATIC_Redraw(pStatic);
   				MEMSET(&bottomParam,0,sizeof(BottomBar_Param_type));
            	bottomParam.eBBarType = BTBAR_OK; //BTBAR_SELECT_BACK;            
				DrawBottomBar(pMe->m_pDisplay, &bottomParam);  
	   	  }
	     case EVT_DIALOG_END:
	      {
		  		ISTATIC_Release(pStatic);
            	pStatic = NULL;
			    return TRUE;
	      }

	   case EVT_KEY:
	      {
		  		switch(wParam)
            	{
            		case AVK_INFO:
					case AVK_SELECT:
                	case AVK_CLR:
                    	CLOSE_DIALOG(DLGRET_CANCELED)
                    	return TRUE;

                	default:
                    	break;
            	}
				 return TRUE;
	   	  }
	   default:
	      break;
    }
}	
static boolean MobileTracker_DnumberHandler(MobileTracker *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
   PARAM_NOT_REF(dwParam)

   AEERect      p_Phnumber1Menu      = {0};
   AEERect      p_Phnumber2Menu      = {0};
   AEERect      p_Phnumber3Menu      = {0};
   IControl*    pControls[3]         = {0};

   AECHAR         m_NumberStr1[20] = {0};
   AECHAR         m_NumberStr2[20] = {0};
   AECHAR         m_NumberStr3[20] = {0};
   
   
   if(!pMe)
   {
      return FALSE;
   }
      switch(eCode)
   {
	   case EVT_DIALOG_INIT:
	      {
	         AECHAR WTitle[40] = {0};
			 AECHAR WNumber1[20] = {0};
			 AECHAR WNumber2[20] = {0};
			 AECHAR WNumber3[20] = {0};
			 int i = 0;
			 (void)ISHELL_LoadResString(pMe->m_pShell,
                        MOBILETRACKER_RES_FILE_LANG,                                
                        IDS_MOBILETRACKER_DESIGNATENUMBER,
                        WTitle,
                        sizeof(WTitle));
             if(pMe->m_pIAnn != NULL)
             {
			    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
             }

			 pMe->m_Number_one = (ITextCtl*)IDIALOG_GetControl(pMe->m_pActiveIDlg,
                                                    IDC_TEXT_NUMBERONE);
			 pMe->m_Number_tow = (ITextCtl*)IDIALOG_GetControl(pMe->m_pActiveIDlg,
                                                    IDC_TEXT_NUMBERTOW);
			 pMe->m_Number_three = (ITextCtl*)IDIALOG_GetControl(pMe->m_pActiveIDlg,
                                                    IDC_TEXT_NUMBERTHREE);

			ITEXTCTL_SetActive(pMe->m_Number_one, FALSE);
			ITEXTCTL_SetActive(pMe->m_Number_tow, FALSE);
			ITEXTCTL_SetActive(pMe->m_Number_three, FALSE);
			ITEXTCTL_SetActive(pMe->m_Number_one,TRUE);
		
			 pMe->m_nitems = 1;


			SETAEERECT(&p_Phnumber1Menu,2,20,120,20);

			SETAEERECT(&p_Phnumber2Menu,2,60,120,20);

			SETAEERECT(&p_Phnumber3Menu,2,100,120,20);

			ITEXTCTL_SetRect(pMe->m_Number_one, &p_Phnumber1Menu);
			ITEXTCTL_SetRect(pMe->m_Number_tow, &p_Phnumber2Menu);
			ITEXTCTL_SetRect(pMe->m_Number_three, &p_Phnumber3Menu);

			ITEXTCTL_SetInputMode(pMe->m_Number_one, AEE_TM_NUMBERS);
			ITEXTCTL_SetInputMode(pMe->m_Number_tow, AEE_TM_NUMBERS);
			ITEXTCTL_SetInputMode(pMe->m_Number_three, AEE_TM_NUMBERS);

			
  			ITEXTCTL_SetProperties(pMe->m_Number_one, TP_FRAME|TP_FIXSETRECT|TP_FOCUS_NOSEL|TP_MB_PROPERTY);
			ITEXTCTL_SetProperties(pMe->m_Number_tow, TP_FRAME|TP_FIXSETRECT|TP_MB_PROPERTY);
			ITEXTCTL_SetProperties(pMe->m_Number_three, TP_FRAME|TP_FIXSETRECT|TP_MB_PROPERTY);
			
			OEM_GetConfig(CFGI_MOBILE_TRACKER_PHONENUMB,WNumber1,sizeof(uint16)*20);
			OEM_GetConfig(CFGI_MOBILE_TRACKER_PHONENUMBTWO,WNumber2,sizeof(uint16)*20);
			OEM_GetConfig(CFGI_MOBILE_TRACKER_PHONENUMBTHREE,WNumber3,sizeof(uint16)*20);

			ITEXTCTL_SetText(pMe->m_Number_one,WNumber1,sizeof(uint16)*20);
			ITEXTCTL_SetText(pMe->m_Number_tow,WNumber2,sizeof(uint16)*20);
			ITEXTCTL_SetText(pMe->m_Number_three,WNumber3,sizeof(uint16)*20);
			
			ITEXTCTL_SetMaxSize(pMe->m_Number_one, 20);
			ITEXTCTL_SetMaxSize(pMe->m_Number_tow, 20);
			ITEXTCTL_SetMaxSize(pMe->m_Number_three, 20);

	         return TRUE;
	      }
	   case EVT_DIALOG_START:
	      {
	        ISHELL_PostEvent(pMe->m_pShell, AEECLSID_MOBILETRACKER,
	                          EVT_USER_REDRAW, 0, 0);
	         return TRUE;
	      }
	   case EVT_USER_REDRAW:
	      {
		     AECHAR WNumber1[20] = {0};
			 AECHAR WNumber2[20] = {0};
			 AECHAR WNumber3[20] = {0};
			 int i = 0;
			 TitleBar_Param_type     TBarParam = {0};
             BottomBar_Param_type  BBarParam ={0};
			 BBarParam.eBBarType = BTBAR_OK_DELETE;
           	 //
            (void) ISHELL_LoadResString(pMe->m_pShell,
                                        MOBILETRACKER_RES_FILE_LANG,
                                        IDS_NUMBER_1,
                                        WNumber1,
                                        sizeof(WNumber1));

			 (void) ISHELL_LoadResString(pMe->m_pShell,
                                        MOBILETRACKER_RES_FILE_LANG,
                                        IDS_NUMBER_2,
                                        WNumber2,
                                        sizeof(WNumber2));

			 (void) ISHELL_LoadResString(pMe->m_pShell,
                                        MOBILETRACKER_RES_FILE_LANG,
                                        IDS_NUMBER_3,
                                        WNumber3,
                                        sizeof(WNumber3));
			 
		  	Appscommon_ResetBackgroundEx(pMe->m_pDisplay, &pMe->m_rc, TRUE);

			 
			IDISPLAY_SetColor( pMe->m_pDisplay, CLR_USER_TEXT, RGB_WHITE);

		    IDISPLAY_DrawText(pMe->m_pDisplay, AEE_FONT_NORMAL, WNumber1,-1, 0, 0, 0, IDF_TEXT_TRANSPARENT);
			IDISPLAY_DrawText(pMe->m_pDisplay, AEE_FONT_NORMAL, WNumber2,-1, 0, 40, 0, IDF_TEXT_TRANSPARENT);
			IDISPLAY_DrawText(pMe->m_pDisplay, AEE_FONT_NORMAL, WNumber3,-1, 0, 80, 0, IDF_TEXT_TRANSPARENT);

		    IDISPLAY_SetColor( pMe->m_pDisplay, CLR_USER_TEXT, RGB_BLACK);
			ITEXTCTL_SetActive(pMe->m_Number_one,TRUE);
			ITEXTCTL_Redraw(pMe->m_Number_one);
			ITEXTCTL_Redraw(pMe->m_Number_tow);
			ITEXTCTL_Redraw(pMe->m_Number_three);
			
		    DrawBottomBar(pMe->m_pDisplay, &BBarParam);
		    return TRUE;
	   	  }
	     case EVT_DIALOG_END:
	      {
		  		 pMe->m_nitems = 1;
			    return TRUE;
	      }

	   case EVT_KEY:
	      {
		  	switch(wParam)
            {
            	//MSG_FATAL("EVT_KEY..................",0,0,0);
            	case AVK_UP:
					{
						if(pMe->m_nitems == 1)
						{
							//MSG_FATAL("m_Number_one..............",0,0,0);
							ITEXTCTL_SetActive(pMe->m_Number_three,TRUE);
							ITEXTCTL_SetActive(pMe->m_Number_one,FALSE);
							ITEXTCTL_SetActive(pMe->m_Number_tow,FALSE);
							pMe->m_nitems = 3;
							return TRUE;
						}
						else if(pMe->m_nitems == 2)
						{
							//MSG_FATAL("m_Number_tow..............",0,0,0);
							ITEXTCTL_SetActive(pMe->m_Number_one,TRUE);
							ITEXTCTL_SetActive(pMe->m_Number_tow,FALSE);
							ITEXTCTL_SetActive(pMe->m_Number_three,FALSE);
							pMe->m_nitems = 1;
							return TRUE;
							
						}
						else if(pMe->m_nitems == 3)
						{
							//MSG_FATAL("m_Number_three..............",0,0,0);
							ITEXTCTL_SetActive(pMe->m_Number_tow,TRUE);
							ITEXTCTL_SetActive(pMe->m_Number_three,FALSE);
							ITEXTCTL_SetActive(pMe->m_Number_one,FALSE);
							pMe->m_nitems = 2;
							return TRUE;
						}
					}
					
					break;
				case AVK_DOWN:
					{
						if(pMe->m_nitems == 1)
						{
							//MSG_FATAL("m_Number_one..............",0,0,0);
							ITEXTCTL_SetActive(pMe->m_Number_tow,TRUE);
							ITEXTCTL_SetActive(pMe->m_Number_one,FALSE);
							ITEXTCTL_SetActive(pMe->m_Number_three,FALSE);
							pMe->m_nitems = 2;
							return TRUE;
						}
						else if(pMe->m_nitems == 2)
						{
							//MSG_FATAL("m_Number_tow..............",0,0,0);
							ITEXTCTL_SetActive(pMe->m_Number_three,TRUE);
							ITEXTCTL_SetActive(pMe->m_Number_tow,FALSE);
							ITEXTCTL_SetActive(pMe->m_Number_one,FALSE);
							pMe->m_nitems = 3;
							return TRUE;
						}
						else if(pMe->m_nitems == 3)
						{
							//MSG_FATAL("m_Number_three..............",0,0,0);
							ITEXTCTL_SetActive(pMe->m_Number_one,TRUE);
							ITEXTCTL_SetActive(pMe->m_Number_three,FALSE);
							ITEXTCTL_SetActive(pMe->m_Number_tow,FALSE);
							pMe->m_nitems = 1;
							return TRUE;
						}
					}
					break;

				case AVK_INFO:
				case AVK_SELECT:
					{
						AECHAR *pNumber1Text  = ITEXTCTL_GetTextPtr(pMe->m_Number_one);
						AECHAR *pNumber2Text  = ITEXTCTL_GetTextPtr(pMe->m_Number_tow);
						AECHAR *pNumber3Text  = ITEXTCTL_GetTextPtr(pMe->m_Number_three);
						
						WSTRCPY(m_NumberStr1,pNumber1Text);
						WSTRCPY(m_NumberStr2,pNumber2Text);
						WSTRCPY(m_NumberStr3,pNumber3Text);
					
						OEM_SetConfig(CFGI_MOBILE_TRACKER_PHONENUMB,pNumber1Text,sizeof(uint16)*20);
						OEM_SetConfig(CFGI_MOBILE_TRACKER_PHONENUMBTWO,pNumber2Text,sizeof(uint16)*20);
						OEM_SetConfig(CFGI_MOBILE_TRACKER_PHONENUMBTHREE,pNumber3Text,sizeof(uint16)*20);
                            
						CLOSE_DIALOG(DLGRET_CANCELED)
						return TRUE;
					}
					break;
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;

                default:
                    break;
            }
			return TRUE;
	   	  }
	   default:
	      break;
    }
}
static boolean MobileTracker_OwernameHandler(MobileTracker *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
   PARAM_NOT_REF(dwParam)
   if(!pMe)
   {
      return FALSE;
   }
      switch(eCode)
   {
	   case EVT_DIALOG_INIT:
	      {
	          AECHAR WTitle[40] = {0};
			 (void)ISHELL_LoadResString(pMe->m_pShell,
                        MOBILETRACKER_RES_FILE_LANG,                                
                        IDS_MOBILETRACKER_OWNERNAME,
                        WTitle,
                        sizeof(WTitle));
             if(pMe->m_pIAnn != NULL)
             {
			    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
             }
	         return TRUE;
	      }
	   case EVT_DIALOG_START:
	      {
	         ISHELL_PostEvent(pMe->m_pShell, AEECLSID_MOBILETRACKER,
	                          EVT_USER_REDRAW, 0, 0);
	         return TRUE;
	      }
	   case EVT_USER_REDRAW:
	      {
		  		
	   	  }
	     case EVT_DIALOG_END:
	      {
			    return TRUE;
	      }

	   case EVT_KEY:
	      {
	   	  }
	   default:
	      break;
    }
}

static boolean MobileTracker_ContentHandler(MobileTracker *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
   PARAM_NOT_REF(dwParam)
   if(!pMe)
   {
      return FALSE;
   }
   pMe->m_Content = (ITextCtl*)IDIALOG_GetControl(pMe->m_pActiveIDlg,IDC_TEXT_CONTEXT);
   if (pMe->m_Content == NULL)
   {
      return FALSE;
   }
 
   switch(eCode)
   {
   	   
	   case EVT_DIALOG_INIT:
	      {
		  	  AEERect m_ContentRect = {0};
			  AECHAR WTTitle[2] = {0};
	          AECHAR WTitle[40] = {0};
			  AECHAR m_wstrContect[120] = {0};
			 (void)ISHELL_LoadResString(pMe->m_pShell,
                        MOBILETRACKER_RES_FILE_LANG,                                
                        IDS_CONTENT,
                        WTitle,
                        sizeof(WTitle));
             if(pMe->m_pIAnn != NULL)
             {
			    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
             }
			 Appscommon_ResetBackgroundEx(pMe->m_pDisplay, &pMe->m_rc, TRUE);
			 SETAEERECT(&m_ContentRect,0,0,pMe->m_rc.dx, pMe->m_rc.dy - GetBottomBarHeight(pMe->m_pDisplay));
			 ITEXTCTL_SetRect(pMe->m_Content, &m_ContentRect);
			 (void)ITEXTCTL_SetTitle( pMe->m_Content, NULL,0,WTTitle);
  			 ITEXTCTL_SetInputMode(pMe->m_Content, AEE_TM_LETTERS);
  			 ITEXTCTL_SetProperties(pMe->m_Content, TP_MULTILINE|TP_FRAME|TP_FOCUS_NOSEL|TP_FIXSETRECT|TP_STARKEY_SWITCH|TP_DISPLAY_COUNT|TP_NOUPDATE|TP_GRAPHIC_BG|TP_NO_HI);
             ITEXTCTL_SetMaxSize(pMe->m_Content, 120);
			 OEM_GetConfig(CFGI_MOBILE_TRACKER_CONTECT,&m_wstrContect,sizeof(uint16)*120);
			 ITEXTCTL_SetText(pMe->m_Content,m_wstrContect,sizeof(uint16)*120);
	         return TRUE;
	      }
	   case EVT_DIALOG_START:
	      {
		  	 //CFGI_MOBILE_TRACKER_CONTECT
	         ISHELL_PostEvent(pMe->m_pShell, AEECLSID_MOBILETRACKER,EVT_USER_REDRAW, 0, 0);
	         return TRUE;
	      }
	   case EVT_USER_REDRAW:
	      {
		  		 TitleBar_Param_type     TBarParam = {0};
	   	       	 BottomBar_Param_type  BBarParam ={0};
				 
	   		     BBarParam.eBBarType = BTBAR_OK_DELETE;
	   			 DrawBottomBar(pMe->m_pDisplay, &BBarParam);
				 ITEXTCTL_Redraw(pMe->m_Content);
	   	       	 // 更新显示
	   	       	 IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE); 
				 return TRUE;
	   	  }
	     case EVT_DIALOG_END:
	      {
			    return TRUE;
	      }

	   case EVT_KEY:
	      {
		  	switch (wParam)
            {    
                MSG_FATAL("key.....................",0,0,0);
            	case AVK_INFO:
				case AVK_SELECT:
				{
					int len = 0;
					uint16 wPWD=0;
					AECHAR m_wstrContect[120] = {0};
					AECHAR *pwsText  = ITEXTCTL_GetTextPtr(pMe->m_Content);
					
					WSTRCPY(m_wstrContect,pwsText);
					OEM_SetConfig(CFGI_MOBILE_TRACKER_CONTECT,&m_wstrContect,sizeof(uint16)*120);
					CLOSE_DIALOG(DLGRET_CANCELED)
					return TRUE;
					
				}
				break;
				case AVK_CLR:
					CLOSE_DIALOG(DLGRET_CANCELED)
				    return TRUE;
					break;
				default:
					if(ITEXTCTL_HandleEvent(pMe->m_Content, eCode, wParam, dwParam))
 			        {
 			            return TRUE;
 			        }	
					break;
		  	}
			return TRUE;
	   	  }
	   default:
	      break;
    }
   
}

static boolean MobileTracker_ChangPassWordHandler(MobileTracker *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
   PARAM_NOT_REF(dwParam)
   AEERect      p_PassWordMenu       = {0};
   AEERect      p_ConfirmPassWord    = {0};
   AECHAR      wszTile[24];     //时间
   AECHAR      wszPassWord[20];
   if(!pMe)
   {
      return FALSE;
   }
   
   pMe->m_ppwdword = (ITextCtl*)IDIALOG_GetControl(pMe->m_pActiveIDlg,
                                                    IDC_CHANGEPASSWORD);
   pMe->m_cpwdword = (ITextCtl*)IDIALOG_GetControl(pMe->m_pActiveIDlg,
                                                    IDC_CONFIRM_PASSWORD);

   if (pMe->m_ppwdword == NULL)
   {
      return FALSE;
   }
   switch(eCode)
   {
	   case EVT_DIALOG_INIT:
	      {
	         AECHAR WTitle[40] = {0};
			 (void)ISHELL_LoadResString(pMe->m_pShell,
                        MOBILETRACKER_RES_FILE_LANG,                                
                        IDS_MOBILETRAXKER_PWDCHG,
                        WTitle,
                        sizeof(WTitle));
             if(pMe->m_pIAnn != NULL)
             {
			    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
             }

			 SETAEERECT(&p_PassWordMenu,2,40,124,20);
			 SETAEERECT(&p_ConfirmPassWord,2,80,124,20);
			 //ITEXTCTL_SetRect(pMe->m_ppwdword, &p_PassWordMenu);
			 //ITEXTCTL_SetRect(pMe->m_cpwdword, &p_ConfirmPassWord);
			 ICONTROL_SetRect((IControl*)pMe->m_ppwdword, &p_PassWordMenu);
			 ICONTROL_SetRect((IControl*)pMe->m_cpwdword, &p_ConfirmPassWord);
  			 ITEXTCTL_SetInputMode(pMe->m_ppwdword, AEE_TM_NUMBERS);
			 ITEXTCTL_SetInputMode(pMe->m_cpwdword, AEE_TM_NUMBERS);
  			 ITEXTCTL_SetProperties(pMe->m_ppwdword, TP_FRAME|TP_FIXSETRECT|TP_FOCUS_NOSEL|TP_MB_PROPERTY);
			 ITEXTCTL_SetProperties(pMe->m_cpwdword, TP_FRAME|TP_FIXSETRECT|TP_MB_PROPERTY);
             ITEXTCTL_SetMaxSize(pMe->m_ppwdword, 5);
		     ITEXTCTL_SetMaxSize(pMe->m_cpwdword, 5);
	         return TRUE;
	      }
	   case EVT_DIALOG_START:
	      {
	         ISHELL_PostEvent(pMe->m_pShell, AEECLSID_MOBILETRACKER,
	                          EVT_USER_REDRAW, 0, 0);
	         return TRUE;
	      }
	   case EVT_USER_REDRAW:
	      {
		  		 AECHAR      wcpTile[24];     //时间
		  		 TitleBar_Param_type     TBarParam = {0};
            	 BottomBar_Param_type  BBarParam ={0};
				 BBarParam.eBBarType = BTBAR_OK_BACK;
           		 //
            	(void) ISHELL_LoadResString(pMe->m_pShell,
                                        MOBILETRACKER_RES_FILE_LANG,
                                        IDS_PWD_TITLE,
                                        wszTile,
                                        sizeof(wszTile));
				(void) ISHELL_LoadResString(pMe->m_pShell,
                                        MOBILETRACKER_RES_FILE_LANG,
                                        IDS_CONFIRM_PASSWORD,
                                        wcpTile,
                                        sizeof(wcpTile));
				 WSTRCAT(wszTile,L":");
				 Appscommon_ResetBackgroundEx(pMe->m_pDisplay, &pMe->m_rc, TRUE);
				 IDISPLAY_SetColor( pMe->m_pDisplay, CLR_USER_TEXT, RGB_WHITE);
				 IDISPLAY_DrawText(pMe->m_pDisplay, AEE_FONT_NORMAL, wszTile,-1, 0, 20, 0, IDF_TEXT_TRANSPARENT);
				 IDISPLAY_DrawText(pMe->m_pDisplay, AEE_FONT_NORMAL, wcpTile,-1, 0, 60, 0, IDF_TEXT_TRANSPARENT);
				 IDISPLAY_SetColor( pMe->m_pDisplay, CLR_USER_TEXT, RGB_BLACK);
				 DrawBottomBar(pMe->m_pDisplay, &BBarParam);
				 ITEXTCTL_SetActive(pMe->m_ppwdword,TRUE);
				 ITEXTCTL_SetActive(pMe->m_cpwdword,FALSE);
            	 // 更新显示
            	 IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);  
				 return TRUE;
	   	  }
	     case EVT_DIALOG_END:
	      {
			    return TRUE;
	      }

	   case EVT_KEY:
	      {
		  	
		  	switch (wParam)
            {    
                MSG_FATAL("key.....................",0,0,0);
            	case AVK_INFO:
				case AVK_SELECT:
				{
					int len = 0;
					uint16 wPWD=0;
					char m_strPhonePWD[24] = {0};
					AECHAR *pwsText  = ITEXTCTL_GetTextPtr(pMe->m_ppwdword);
					AECHAR *PcpText  = ITEXTCTL_GetTextPtr(pMe->m_cpwdword);
					MSG_FATAL("avk_selectpws==%d,,pcp===%d",WSTRLEN(pwsText),WSTRLEN(PcpText),0);
					len =WSTRLEN(pwsText);
					if(WSTRCMP(pwsText,PcpText) != 0 || len<5)
					{
						CLOSE_DIALOG(MGDLGRET_FAILD)
						return TRUE;
					}

					WSTRTOSTR(pwsText,m_strPhonePWD,sizeof(m_strPhonePWD));
					len = STRLEN(m_strPhonePWD);
					DBGPRINTF("pMe->m_strPhoneNUM %s",m_strPhonePWD);
                    wPWD = EncodePWDToUint16(m_strPhonePWD);
					OEM_SetConfig(CFGI_MOBILETRACKER_PASSWORD,&wPWD,sizeof(uint16));
					CLOSE_DIALOG(DLGRET_CANCELED)
					return TRUE;
					
				}
				break;
				case AVK_UP:
				case AVK_DOWN:
					{
						if(ITEXTCTL_IsActive(pMe->m_ppwdword))
						{
							ITEXTCTL_SetActive(pMe->m_ppwdword,FALSE);
							ITEXTCTL_SetActive(pMe->m_cpwdword,TRUE);
						}
						else
						{
							ITEXTCTL_SetActive(pMe->m_ppwdword,TRUE);
							ITEXTCTL_SetActive(pMe->m_cpwdword,FALSE);
						}
					}
					break;
				case AVK_CLR:
					CLOSE_DIALOG(DLGRET_CANCELED)
				    return TRUE;
					break;
				default:
					if(ITEXTCTL_HandleEvent(pMe->m_ppwdword, eCode, wParam, dwParam))
 			        {
 			            return TRUE;
 			        }	
					break;
		  	}
			
			return TRUE;
	   	  }
	   default:
	      break;
    }
}


static boolean MobileTracker_PopMsgHandler(MobileTracker *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
   PARAM_NOT_REF(dwParam)
   uint16   nMsgBoxId;   
   if(!pMe)
   {
      return FALSE;
   }
   
   MSG_FATAL("***zzg MobileTracker_PopMsgHandler eCode=%x***", eCode, 0, 0);

   switch(eCode)
   {
   	  case EVT_DIALOG_INIT:
      {
         
         return TRUE;
      }
   case EVT_DIALOG_START:
      {
         ISHELL_PostEvent(pMe->m_pShell, AEECLSID_MOBILETRACKER,
                          EVT_USER_REDRAW, 0, 0);
         return TRUE;
      }
   case EVT_USER_REDRAW:
      {
	  	 IStatic* pIStatic = NULL;
         AECHAR szText[256];
         PromptMsg_Param_type MsgParam={0};
		 Appscommon_ResetBackgroundEx(pMe->m_pDisplay, &pMe->m_rc, TRUE);
		 MSG_FATAL("***zzg MobileTracker_PopMsgHandler eCode=%x,%d,%d***", pMe->m_rc.x, pMe->m_rc.y, pMe->m_rc.dy);
         if(SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                             AEECLSID_STATIC,
                                             (void **)&pIStatic))
         {
            return FALSE;
         }

         ISHELL_LoadResString(pMe->m_pShell,
                              MOBILETRACKER_RES_FILE_LANG,
                              IDS_INVALID,
                              szText,
                              sizeof(szText));
         MsgParam.ePMsgType = MESSAGE_INFORMATION;
         MsgParam.pwszMsg = szText;
         MsgParam.eBBarType = BTBAR_NONE;
         
        DrawPromptMessage(pMe->m_pDisplay, pIStatic, &MsgParam);
       
        ISHELL_SetTimer(pMe->m_pShell,
                            PROMPTMSG_TIMER,
                            MobileTracker_MsgBoxTimeout,
                            pMe);
         return TRUE;
      }

   case EVT_DIALOG_END:
      {
		    return TRUE;
      }

   case EVT_KEY:
      {
		 switch(wParam)
         {
         case AVK_CLR:	
		 	{
               ISHELL_CancelTimer(pMe->m_pShell,
                                  MobileTracker_MsgBoxTimeout, pMe);
			   if(pMe->m_Rturnpass)
			   {
			   		CLOSE_DIALOG(DLGRET_CHAGNEPASSWOWRD);
			   }
			   else
			   {
               		CLOSE_DIALOG(DLGRET_CANCELED);
			   }
         	}
            break;

         case AVK_SELECT:
		 	{
               if(pMe->m_Rturnpass)
			   {
			   		CLOSE_DIALOG(DLGRET_CHAGNEPASSWOWRD);
			   }
			   else
			   {
               		CLOSE_DIALOG(DLGRET_CANCELED);
			   }
         	}
            break;

         default:
            break;
         }

         return TRUE;
         break;
      }


   case EVT_DISPLAYDIALOGTIMEOUT:
      {
		 CLOSE_DIALOG(DLGRET_CANCELED)
         return TRUE;
      }

   default:
      break;
   }

   return FALSE;
}


static boolean MobileTracker_PassWordHandler(MobileTracker *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
	PARAM_NOT_REF(dwParam)
    //static char   *m_strPhonePWD = NULL;
    AECHAR      wstrDisplay[OEMNV_LOCKCODE_MAXLEN+2] = {0};
    int             nLen = 0;
    char        strDisplay[OEMNV_LOCKCODE_MAXLEN+2] = {0};
    MSG_FATAL("MobileTracker_PassWordHandler....",0,0,0);
    if (NULL == pMe)
    {
        return FALSE;
    }
    
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            if(NULL == pMe->m_strPhonePWD)
            {
                pMe->m_strPhonePWD = (char *)MALLOC((OEMNV_LOCKCODE_MAXLEN + 1)* sizeof(char));
            }
            return TRUE;
            
        case EVT_DIALOG_START:  
            (void) ISHELL_PostEvent(pMe->m_pShell,
                                    AEECLSID_MOBILETRACKER,
                                    EVT_USER_REDRAW,
                                    NULL,
                                    NULL);

            return TRUE;
            
        case EVT_USER_REDRAW:
            // 绘制相关信息
            {
                AECHAR  text[32] = {0};
                RGBVAL nOldFontColor;
                TitleBar_Param_type  TitleBar_Param = {0};
                
                Appscommon_ResetBackgroundEx(pMe->m_pDisplay, &pMe->m_rc, TRUE);
                //IDISPLAY_FillRect  (pMe->m_pDisplay,&pMe->m_rc,RGB_BLACK);

				(void)ISHELL_LoadResString(pMe->m_pShell, 
	                                        MOBILETRACKER_RES_FILE_LANG,
	                                        IDS_MOBILETRACKER_TITLE, 
	                                        text,
	                                        sizeof(text));    
				 
				
                // 画标题条
                TitleBar_Param.pwszTitle = text;
                TitleBar_Param.dwAlignFlags = IDF_ALIGN_MIDDLE | IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE;
                #if 0
                DrawTitleBar(pMe->m_pDisplay, &TitleBar_Param);
				#else
				IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,text);
				#endif

               (void)ISHELL_LoadResString(pMe->m_pShell, 
                                                MOBILETRACKER_RES_FILE_LANG,
                                                IDS_PWD_TITLE, 
                                                text,
                                                sizeof(text));
                nOldFontColor = IDISPLAY_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, RGB_WHITE);
                
                IDISPLAY_DrawText(pMe->m_pDisplay, 
                                    AEE_FONT_BOLD, 
                                    text,
                                    -1, 
                                    5, 
                                    MENUITEM_HEIGHT*1/2, 
                                    NULL, 
                                    IDF_TEXT_TRANSPARENT);
                   
                nLen = (pMe->m_strPhonePWD == NULL)?(0):(STRLEN(pMe->m_strPhonePWD));
                MEMSET(strDisplay, '*', nLen);
                strDisplay[nLen] = '|';
                strDisplay[nLen + 1] = '\0';
                (void) STRTOWSTR(strDisplay, wstrDisplay, sizeof(wstrDisplay));
                IDISPLAY_DrawText(pMe->m_pDisplay, 
                                AEE_FONT_BOLD, 
                                wstrDisplay,
                                -1, 
                                10, 
                                MENUITEM_HEIGHT*3/2,
                                NULL, 
                                IDF_TEXT_TRANSPARENT);
                (void)IDISPLAY_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, nOldFontColor);
        
                // 绘制底条提示
                if (nLen > 3)
                {// 确定-----删除
                	#ifndef FEATURE_ALL_KEY_PAD
					MTAppUtil_DrawSoftkey(pMe, BTBAR_OK_DELETE);                    
                    #else
					MTAppUtil_DrawSoftkey(pMe, BTBAR_OK_BACK);                       
                    #endif
                }
                else if(nLen > 0)
                {
                	#ifndef FEATURE_ALL_KEY_PAD
                    MTAppUtil_DrawSoftkey(pMe,BTBAR_DELETE);
                    #else
                    MTAppUtil_DrawSoftkey(pMe,BTBAR_BACK);
                    #endif
                }
                else
                {// 确定-----取消
                    MTAppUtil_DrawSoftkey(pMe,BTBAR_CANCEL);
                }

                // 更新显示
                IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE); 
        
                return TRUE;
            }
            
        case EVT_DIALOG_END:
			if(pMe->m_strPhonePWD)
			{
				 FREEIF(pMe->m_strPhonePWD);
			}			
			return TRUE;

        case EVT_KEY:
            {
                char  chEnter = 0;
                int   nLen = 0;
                boolean bRedraw = FALSE;
                
                switch (wParam)
                {
                    case AVK_0:
                    case AVK_1:
                    case AVK_2:
                    case AVK_3:
                    case AVK_4:
                    case AVK_5:
                    case AVK_6:
                    case AVK_7:
                    case AVK_8:
                    case AVK_9:
                        chEnter = '0' + (wParam - AVK_0);
                        break;

                    case AVK_STAR:
                        chEnter = '*';
                        break;
 
                    case AVK_POUND:
                        chEnter = '#';
                        break;
                    //Add By zzg 2012_02_27					
					case AVK_DEL:	 
					{
						chEnter = 0;
						break;
					}
					//Add End	
                    case AVK_CLR:
                        chEnter = 0;       
                        if (pMe->m_strPhonePWD == NULL || STRLEN(pMe->m_strPhonePWD) == 0)
	                    {
	                         CLOSE_DIALOG(DLGRET_CANCELED)
	                         return TRUE;
	                    }
                        break;
                        
                    case AVK_SELECT:
                    case AVK_INFO:
                        if (pMe->m_strPhonePWD == NULL || STRLEN(pMe->m_strPhonePWD) < 4)
                        {
                            return TRUE;
                        }
                        else
                        //end added
                        {
                            uint16 wPWD=0;

                            OEM_GetConfig(CFGI_MOBILETRACKER_PASSWORD, &wPWD, sizeof(wPWD));
                        
                            if (wPWD == EncodePWDToUint16(pMe->m_strPhonePWD))
                            {// 密码符合
                            	pMe->b_pwdWright = TRUE;
                                CLOSE_DIALOG(MGDLGRET_PASS)
                            }
                            else
                            {// 密码错误
                                CLOSE_DIALOG(MGDLGRET_FAILD)
                            }
                        }
                        return TRUE;
                        
                    default:
                        return TRUE;
                }
                nLen = (pMe->m_strPhonePWD == NULL)?(0):(STRLEN(pMe->m_strPhonePWD));
                if (chEnter == 0)
                {// 删除字符
                    if (nLen > 0)
                    {
                        bRedraw = TRUE;
                        pMe->m_strPhonePWD[nLen-1] = chEnter;
                    }
                }
                else if (nLen < OEMNV_LOCKCODE_MAXLEN)
                {
                    pMe->m_strPhonePWD[nLen] = chEnter;
                    nLen++;
                    pMe->m_strPhonePWD[nLen] = 0;
                    bRedraw = TRUE;
                }
                
                if (bRedraw)
                {
                    (void) ISHELL_PostEvent(pMe->m_pShell,
                                            AEECLSID_MOBILETRACKER,
                                            EVT_USER_REDRAW,
                                            NULL,
                                            NULL);
                }
            }
            return TRUE;
            
        default:
            break;
    }
    
    return FALSE;
}

static __inline void MTAppUtil_DrawSoftkey(MobileTracker* pMe,
                                       BottomBar_e_Type eSoftkeyType)
{
   if(pMe)
   {
      BottomBar_Param_type BarParam={0};
      BarParam.eBBarType = eSoftkeyType;
      DrawBottomBar(pMe->m_pDisplay, &BarParam);
      IDISPLAY_Update(pMe->m_pDisplay);
   }
}//MGAppUtil_DrawSoftkey



#ifndef _CUSTSERVICE_APP_C_
#define _CUSTSERVICE_APP_C_

#include "Custservice_priv.h"
//#include "MorseAppProt.h"

int CustServiceMod_Load(IShell *pIShell,void *ph,IModule **ppMod);
int CustServiceMod_New(int16  nSize,IShell *pIShell,void *ph,IModule  **ppMod,PFNMODCREATEINST pfnMC,
    				PFNFREEMODDATA pfnMF);
static int CustServiceMod_CreateInstance(IModule *po,IShell *pIShell,AEECLSID ClsId,void **ppObj);
static uint32 CustServiceMod_AddRef(IModule *po);
static uint32 CustServiceMod_Release(IModule *po);
static void CustServiceMod_FreeResources(IModule  *po,IHeap  *ph,IFileMgr *pfm);
static int CustService_New(IShell *ps,IModule *pIModule,ICustServiceMenu **ppObj);
static uint32 CustService_AddRef(ICustServiceMenu *pi);
static uint32 CustService_Release (ICustServiceMenu *pi);
static boolean CustService_HandleEvent(ICustServiceMenu *pi,AEEEvent eCode,uint16  wParam,uint32 dwParam);

static void CustService_APPIsReadyTimer(void *pme);
static int CustService_InitAppData(CCustServiceMenu *pMe);
static void CustService_FreeAppData(CCustServiceMenu *pMe);
static void CustService_RunFSM(CCustServiceMenu *pMe);

/*-------------------------------------------------------------------
            Static variable Definitions
-------------------------------------------------------------------*/
static CustServiceMod gCustServiceMod;

static const VTBL(IModule) gModFuncs =
{
	CustServiceMod_AddRef,
	CustServiceMod_Release,
	CustServiceMod_CreateInstance,
	CustServiceMod_FreeResources
};


static CCustServiceMenu gCustServiceMenu;
extern boolean gCustServiceBgRun;
static const VTBL(ICustServiceMenu) gCustServiceMethods =
{
	CustService_AddRef,
	CustService_Release,
	CustService_HandleEvent
};

/* =========================================================================
                     FUNCTION DEFINITIONS
   ========================================================================= */

/*==============================================================================
       SportsMod_Load
==============================================================================*/
int CustServiceMod_Load(IShell *pIShell,void *ph,IModule **ppMod)
{
	return CustServiceMod_New(sizeof(CustServiceMod),pIShell,ph,ppMod,NULL,NULL);
}


int  CustServiceMod_New(int16 nSize,IShell *pIShell,void *ph,IModule **ppMod,
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
    
	if (!pIShell)
	{
	    return EFAILED;
	}
	
	MEMSET(&gCustServiceMod,0,sizeof(CustServiceMod));
	INIT_VTBL(&gCustServiceMod,IModule,gModFuncs);
	gCustServiceMod.m_nRefs = 1;
	*ppMod = (IModule *)&gCustServiceMod;
	return AEE_SUCCESS;
}

/*==============================================================================
o‘那y:
       SportsMod_AddRef

?米?¯:
       Sports ?㏒?谷辰y車???那y?車1㏒?2⊿﹞米??米㊣?～米?辰y車?那y?㏒

2?那y:
       po [in]㏒oIModule?車?迆???車?????㏒

﹞米???米:
       米㊣?～??Sports ?㏒?谷米?辰y車?那y

㊣?℅⊿:

==============================================================================*/
static uint32 CustServiceMod_AddRef(IModule *po)
{
	return(++((CustServiceMod *)po)->m_nRefs);
}

/*==============================================================================
o‘那y:
       SportsMod_Release

?米?¯:
       Sports ?㏒?谷辰y車???那y??1㏒?2⊿﹞米??米㊣?～米?辰y車?那y?㏒

2?那y:
       po [in]㏒oIModule?車?迆???車?????㏒

﹞米???米:
       米㊣?～??Sports ?㏒?谷米?辰y車?那y

㊣?℅⊿:

==============================================================================*/
static uint32 CustServiceMod_Release(IModule *po)
{
	if (((CustServiceMod *) po)->m_nRefs == 0)
	{
	    return 0;
	}

	return(--((CustServiceMod *)po)->m_nRefs);
}

/*==============================================================================
o‘那y:
       SportsMod_CreateInstance

?米?¯:
        赤“1y∩∩?“(3?那??‘)Sports???車角∩∩∩?“?㏒?谷那米角y?㏒

2?那y:
       po [in]㏒oIModule?車?迆???車?????㏒
       pIShell [in]㏒oIShell?車?迆???車?????㏒
       ClsId [in]㏒o辰a∩∩?“米?那米角y米?角角ID?㏒
       ppObj [out]:㏒o車?車迆㊣㏒∩????車那米角y米?????米?????㊣?芍??㏒

﹞米???米:
       EFAILED;2迄℅¯那∫～邦?㏒
       SUCCESS;2迄℅¯3谷1|?㏒

㊣?℅⊿:
       2?D豕車??∫豕ㄓ米¯車??㏒

==============================================================================*/
static int  CustServiceMod_CreateInstance(IModule *po,
    IShell *pIShell,
    AEECLSID ClsId,
    void **ppObj
)
{
	*ppObj = NULL;

	if (ClsId != AEECLSID_CUSTSERVICE)
	{
	    return EFAILED;
	}

	if (CustService_New(pIShell, po,(ICustServiceMenu**)ppObj) != SUCCESS)
	{
	    return EFAILED;
	}
	
	return SUCCESS;
}

/*==============================================================================
o‘那y:
       SportsMod_FreeResources

?米?¯:
        那赤﹞?Sports?㏒?谷℅那?∩米?o‘那y?㏒

2?那y:
       po ㏒oIModule?㏒
       ph ㏒oIHeap?車?迆?????㏒
       pfm :IFileMgr?車?迆?????㏒

﹞米???米:
       ?T

㊣?℅⊿:
       2?D豕車??∫豕ㄓ米¯車??㏒

==============================================================================*/
static void CustServiceMod_FreeResources(IModule * po,IHeap * ph,IFileMgr * pfm)
{

    PARAM_NOT_REF(po)
    PARAM_NOT_REF(ph)
    PARAM_NOT_REF(pfm)
}

static int CustService_New(IShell *ps,IModule *pIModule,ICustServiceMenu **ppObj)
{
	int retVal = SUCCESS;
	if (0 == gCustServiceMenu.m_nRefs)
	{
		// Must initialize the object
		MEMSET(&gCustServiceMenu,  0, sizeof(CCustServiceMenu));
		INIT_VTBL(&gCustServiceMenu, ICustServiceMenu, gCustServiceMethods);

		gCustServiceMenu.m_nRefs     = 0;
		gCustServiceMenu.m_pShell    = ps;
		gCustServiceMenu.m_pModule   = pIModule;

		(void) ISHELL_AddRef(ps);
		(void) IMODULE_AddRef(pIModule);
		retVal = CustService_InitAppData(&gCustServiceMenu);
	}

	++gCustServiceMenu.m_nRefs;
	*ppObj = (ICustServiceMenu*)&gCustServiceMenu;
	
	return retVal;
}

static uint32  CustService_AddRef(ICustServiceMenu *pi)
{
    register CCustServiceMenu *pMe = (CCustServiceMenu*)pi;

    //ASSERT(pMe != NULL);
    //ASSERT(pMe->m_nRefs > 0);

    return (++pMe->m_nRefs);
}

static uint32  CustService_Release (ICustServiceMenu *pi)
{
	register CCustServiceMenu *pMe = (CCustServiceMenu*)pi;

	//ASSERT(pMe != NULL);

	if (pMe->m_nRefs == 0)
	{
	    return 0;
	}

	if (--pMe->m_nRefs)
	{
	    return pMe->m_nRefs;
	}

	CustService_FreeAppData(pMe);

	(void) ISHELL_Release(pMe->m_pShell);
	(void) IMODULE_Release(pMe->m_pModule);

	return 0;
}

/*==============================================================================
o‘那y:
       SportSApp_InitAppData

?米?¯:
       3?那??‘SportsMenu Applet?芍11那y?Y豕㊣那??米?㏒

2?那y:
       pMe [in]㏒o???辰CSportsMenu Applet???車?芍11米??????㏒???芍11～邦o?D?3足D辰米?足??“D??⊿?㏒

﹞米???米:
       SUCCESS㏒o3?那??‘3谷1|?㏒
       EFAILED㏒o3?那??‘那∫～邦?㏒

㊣?℅⊿:

==============================================================================*/
static int CustService_InitAppData(CCustServiceMenu *pMe)
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
	pMe->m_ePreState = CUSTSERVICE_STATE_NONE;
	pMe->m_eCurState = CUSTSERVICE_STATE_INIT;
	pMe->m_pActiveDlg = NULL;
	pMe->m_eDlgRet = DLGRET_CREATE;
	pMe->m_bNotOverwriteDlgRet = FALSE;
	pMe->m_currDlgId = 0;
	pMe->m_nSubDlgId = 0;
	/*Init Morse App Data*/
	//CustService_CodeInitApp();
    return SUCCESS;
}

/*==============================================================================
o‘那y:
       SportSApp_FreeAppData

?米?¯:
       那赤﹞?SettingMenu Applet那1車?米??角1?℅那?∩?㏒

2?那y:
       pMe [in]㏒o???辰SettingMenu Applet???車?芍11米??????㏒???芍11～邦o?D?3足D辰米?足??“D??⊿?㏒

﹞米???米:
       ?T?㏒

㊣?℅⊿:

==============================================================================*/
static void CustService_FreeAppData(CCustServiceMenu *pMe)
{
	if (NULL == pMe)
	{
	    return;
	}
	
	ISHELL_CancelTimer(pMe->m_pShell,NULL,pMe);
}

/*==============================================================================
o‘那y:
       SportSApp_RunFSM

?米?¯:
       SportsMenu Applet車D?T℅∩足??迆辰y???㏒

2?那y:
       pMe [in]㏒o???辰SettingMenu Applet???車?芍11米??????㏒???芍11～邦o?D?3足D辰米?足??“D??⊿?㏒

﹞米???米:
       ?T?㏒

㊣?℅⊿:

==============================================================================*/
static void CustService_RunFSM(CCustServiceMenu *pMe)
{
    NextFSMAction nextFSMAction = NFSMACTION_WAIT;

    for ( ; ; )
    {
        nextFSMAction = CustService_ProcessState(pMe);

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
o‘那y:
       SportSApp_HandleEvent

?米?¯:
       SportsApp  Applet那??t∩|角赤?¯o‘那y?㏒?迄車D那?車迆㊣?Applet米?那??t㏒?
       ?????-車谷??o‘那y∩|角赤?㏒

2?那y:
       pi [in]㏒o???辰ISportMenu?車?迆t???車米??????㏒
       eCode㏒oBREW那??t∩迆???㏒
       wParam㏒o車?那??t?角1?芍a米?那y?Y?㏒
       dwParam [in]㏒o車?那??t?角1?芍a米?那y?Y?㏒

﹞米???米:
       TRUE㏒o∩?豕?那??t㊣?∩|角赤?㏒
       FALSE㏒o∩?豕?那??t??㊣?∩|角赤?㏒

㊣?℅⊿:

==============================================================================*/
static boolean CustService_HandleEvent(ICustServiceMenu *pi,
    AEEEvent eCode,
    uint16  wParam,
    uint32 dwParam
)
{
    CCustServiceMenu *pMe = (CCustServiceMenu*)pi;
    AEEAppStart *as;

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
            pMe->m_rc.y = pMe->m_rc.y - CUSTSERVICE_MENU_SOFK_HIGHT;
            pMe->m_bSuspending = FALSE;

            CustService_RunFSM(pMe);
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
            pMe->m_rc.y = pMe->m_rc.y - CUSTSERVICE_MENU_SOFK_HIGHT;
            CustService_RunFSM(pMe);
            return TRUE;

        case EVT_DIALOG_INIT:
            pMe->m_bAppIsReady = FALSE;
            pMe->m_pActiveDlg = (IDialog*)dwParam;
            pMe->m_pActiveDlgID = wParam;
            return CustService_RouteDialogEvent(pMe,eCode,wParam,dwParam);

        case EVT_DIALOG_START:
            return CustService_RouteDialogEvent(pMe,eCode,wParam,dwParam);

        case EVT_USER_REDRAW:
            (void) CustService_RouteDialogEvent(pMe,eCode,wParam,dwParam);
            (void)ISHELL_SetTimer ( pMe->m_pShell,
                            APPISREADY_TIMER,
                            CustService_APPIsReadyTimer,
                            pMe);
            return TRUE;

        case EVT_APPISREADY:
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
            return CustService_RouteDialogEvent(pMe,eCode,wParam,dwParam);

        case EVT_DIALOG_END:
            if (wParam == 0)
            {
                return TRUE;
            }
            pMe->m_bAppIsReady = FALSE;
            (void) CustService_RouteDialogEvent(pMe,eCode,wParam,dwParam);
            pMe->m_pActiveDlg = NULL;

            if (pMe->m_bSuspending == FALSE)
            {
                CustService_RunFSM(pMe);
            }
            return TRUE;

        default:
            return CustService_RouteDialogEvent(pMe,eCode,wParam,dwParam);
    }
}

/*==============================================================================
o‘那y:
       SportSApp_APPIsReadyTimer

?米?¯:
       ?“那㊣?¯??米¯o‘那y?㏒?¯辰a車?車迆?????足?迄～∩?邦?那足a米?∩|角赤?㏒

2?那y:
       pme [in]㏒ovoid *角角D赤????㏒?～米??SportMenu?芍11?????㏒

﹞米???米:
       ?T?㏒

㊣?℅⊿:

==============================================================================*/
static void CustService_APPIsReadyTimer(void *pme)
{
	CCustServiceMenu *pMe = (CCustServiceMenu *)pme;

	if (NULL == pMe)
	{
	    return;
	}

	(void) ISHELL_PostEvent( pMe->m_pShell,AEECLSID_CUSTSERVICE,EVT_APPISREADY,0,0);
}

#endif

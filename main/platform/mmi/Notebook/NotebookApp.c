#ifndef _NOTEBOOK_APP_C_
#define _NOTEBOOK_APP_C_

//#include "Esurfing_priv.h"
#include "NotebookAppProt.h"

int NotebookMod_Load(IShell *pIShell,void *ph,IModule **ppMod);
int NotebookMod_New(int16  nSize,IShell *pIShell,void *ph,IModule  **ppMod,PFNMODCREATEINST pfnMC,
    				PFNFREEMODDATA pfnMF);
static int NotebookMod_CreateInstance(IModule *po,IShell *pIShell,AEECLSID ClsId,void **ppObj);
static uint32 NotebookMod_AddRef(IModule *po);
static uint32 NotebookMod_Release(IModule *po);
static void NotebookMod_FreeResources(IModule  *po,IHeap  *ph,IFileMgr *pfm);


/*----------------------Application Notebook ---------------------*/
static int Notebook_New(IShell *ps,IModule *pIModule,INotebookMenu **ppObj);
static uint32 Notebook_AddRef(INotebookMenu *pi);
static uint32 Notebook_Release (INotebookMenu *pi);
static boolean Notebook_HandleEvent(INotebookMenu *pi,AEEEvent eCode,uint16  wParam,uint32 dwParam);

static void Notebook_APPIsReadyTimer(void *pme);
static int Notebook_InitAppData(CNotebookMenu *pMe);
static void Notebook_FreeAppData(CNotebookMenu *pMe);
static void Notebook_RunFSM(CNotebookMenu *pMe);

/*-------------------------------------------------------------------
            Static variable Definitions
-------------------------------------------------------------------*/
static NotebookMod gNotebookMod;

static const VTBL(IModule) gModFuncs =
{
	NotebookMod_AddRef,
	NotebookMod_Release,
	NotebookMod_CreateInstance,
	NotebookMod_FreeResources
};


static CNotebookMenu gNotebookMenu;
extern boolean gNotebookBgRun;
static const VTBL(INotebookMenu) gNotebookMethods =
{
	Notebook_AddRef,
	Notebook_Release,
	Notebook_HandleEvent
};

/* =========================================================================
                     FUNCTION DEFINITIONS
   ========================================================================= */
int NotebookMod_Load(IShell *pIShell,void *ph,IModule **ppMod)
{
	return NotebookMod_New(sizeof(NotebookMod),pIShell,ph,ppMod,NULL,NULL);
}

int  NotebookMod_New(int16 nSize,IShell *pIShell,void *ph,IModule **ppMod,
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
	
	MEMSET(&gNotebookMod,0,sizeof(NotebookMod));
	INIT_VTBL(&gNotebookMod,IModule,gModFuncs);
	gNotebookMod.m_nRefs = 1;
	*ppMod = (IModule *)&gNotebookMod;
	return AEE_SUCCESS;
}

static uint32 NotebookMod_AddRef(IModule *po)
{
	return(++((NotebookMod *)po)->m_nRefs);
}

static uint32 NotebookMod_Release(IModule *po)
{
	if (((NotebookMod *) po)->m_nRefs == 0)
	{
	    return 0;
	}

	return(--((NotebookMod *)po)->m_nRefs);
}

static int  NotebookMod_CreateInstance(IModule *po,
    IShell *pIShell,
    AEECLSID ClsId,
    void **ppObj
)
{
	*ppObj = NULL;

	if (ClsId != AEECLSID_NOTEBOOK)
	{
	    return EFAILED;
	}

	if (Notebook_New(pIShell, po,(INotebookMenu**)ppObj) != SUCCESS)
	{
	    return EFAILED;
	}
	
	return SUCCESS;
}

static void NotebookMod_FreeResources(IModule * po,IHeap * ph,IFileMgr * pfm)
{

    PARAM_NOT_REF(po)
    PARAM_NOT_REF(ph)
    PARAM_NOT_REF(pfm)
}

static int Notebook_New(IShell *ps,IModule *pIModule,INotebookMenu **ppObj)
{
	int retVal = SUCCESS;
	if (0 == gNotebookMenu.m_nRefs)
	{
		// Must initialize the object
		MEMSET(&gNotebookMenu,  0, sizeof(CNotebookMenu));
		INIT_VTBL(&gNotebookMenu, INotebookMenu, gNotebookMethods);

		gNotebookMenu.m_nRefs     = 0;
		gNotebookMenu.m_pShell    = ps;
		gNotebookMenu.m_pModule   = pIModule;

		(void) ISHELL_AddRef(ps);
		(void) IMODULE_AddRef(pIModule);
		retVal = Notebook_InitAppData(&gNotebookMenu);
	}

	++gNotebookMenu.m_nRefs;
	*ppObj = (INotebookMenu*)&gNotebookMenu;
	
	return retVal;
}

static uint32  Notebook_AddRef(INotebookMenu *pi)
{
    register CNotebookMenu *pMe = (CNotebookMenu*)pi;

    //ASSERT(pMe != NULL);
    //ASSERT(pMe->m_nRefs > 0);

    return (++pMe->m_nRefs);
}

static uint32  Notebook_Release (INotebookMenu *pi)
{
	register CNotebookMenu *pMe = (CNotebookMenu*)pi;

	//ASSERT(pMe != NULL);

	if (pMe->m_nRefs == 0)
	{
	    return 0;
	}

	if (--pMe->m_nRefs)
	{
	    return pMe->m_nRefs;
	}

	Notebook_FreeAppData(pMe);

	(void) ISHELL_Release(pMe->m_pShell);
	(void) IMODULE_Release(pMe->m_pModule);

	return 0;
}

static int Notebook_InitAppData(CNotebookMenu *pMe)
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
	pMe->m_ePreState = NOTEBOOK_STATE_NONE;
	pMe->m_eCurState = NOTEBOOK_STATE_INIT;
	pMe->m_pActiveDlg = NULL;
	pMe->m_eDlgRet = DLGRET_CREATE;
	pMe->m_bNotOverwriteDlgRet = FALSE;
	pMe->m_currDlgId = 0;
	pMe->m_nSubDlgId = 0;
    pMe->m_pszSaveDir = NOTEBOOK_MAIN_DIR;
    pMe->m_pszFileExtension = NOTEBOOK_FILE_EXT;
    // Create IVector instance
    
	if ( SUCCESS != ISHELL_CreateInstance( pMe->m_pShell,
	                                   AEECLSID_FILEMGR,
	                                   (void **)&pMe->m_pFileMgr))
	{
	    return EFAILED;
	}

    return SUCCESS;
}

static void Notebook_FreeAppData(CNotebookMenu *pMe)
{
	if (NULL == pMe)
	{
	    return;
	}

    /*release notebook*/

    if (pMe->m_pFileMgr != NULL)
    {
        (void) IFILEMGR_Release(pMe->m_pFileMgr);
        pMe->m_pFileMgr = NULL;
    }    
	ISHELL_CancelTimer(pMe->m_pShell,NULL,pMe);
}

static void Notebook_RunFSM(CNotebookMenu *pMe)
{
    NextFSMAction nextFSMAction = NFSMACTION_WAIT;

    for ( ; ; )
    {
        nextFSMAction = Notebook_ProcessState(pMe);

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

static boolean Notebook_HandleEvent(INotebookMenu *pi,
    AEEEvent eCode,
    uint16  wParam,
    uint32 dwParam
)
{
    CNotebookMenu *pMe = (CNotebookMenu*)pi;
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
            pMe->m_rc.y = pMe->m_rc.y - NOTEBOOK_MENU_SOFK_HIGHT;
            pMe->m_bSuspending = FALSE;

            Notebook_RunFSM(pMe);
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
            pMe->m_rc.y = pMe->m_rc.y - NOTEBOOK_MENU_SOFK_HIGHT;
            Notebook_RunFSM(pMe);
            return TRUE;

        case EVT_DIALOG_INIT:
            pMe->m_bAppIsReady = FALSE;
            pMe->m_pActiveDlg = (IDialog*)dwParam;
            pMe->m_pActiveDlgID = wParam;
            return Notebook_RouteDialogEvent(pMe,eCode,wParam,dwParam);

        case EVT_DIALOG_START:
            (void)ISHELL_SetTimer ( pMe->m_pShell,
                            APPISREADY_TIMER,
                            Notebook_APPIsReadyTimer,
                            pMe);
            return Notebook_RouteDialogEvent(pMe,eCode,wParam,dwParam);

        case EVT_USER_REDRAW:
            (void) Notebook_RouteDialogEvent(pMe,eCode,wParam,dwParam);
            /*
            (void)ISHELL_SetTimer ( pMe->m_pShell,
                            APPISREADY_TIMER,
                            Notebook_APPIsReadyTimer,
                            pMe);
            */
            return TRUE;

        case EVT_APPISREADY:
            pMe->m_bAppIsReady = TRUE;
            return TRUE;

        case EVT_KEY_PRESS:
        case EVT_KEY_RELEASE:
        case EVT_KEY:
        case EVT_COMMAND:
			#if !defined(FEATURE_VERSION_K212) && !defined(FEATURE_VERSION_EC99)&&!defined(FEATURE_QVGA_INHERIT_K212)&&!defined(FEATURE_VERSION_IN50A)
            if (!pMe->m_bAppIsReady)
            {
                return TRUE;
            }
			#endif
            return Notebook_RouteDialogEvent(pMe,eCode,wParam,dwParam);

        case EVT_DIALOG_END:
            if (wParam == 0)
            {
                return TRUE;
            }
            pMe->m_bAppIsReady = FALSE;
            (void) Notebook_RouteDialogEvent(pMe,eCode,wParam,dwParam);
            pMe->m_pActiveDlg = NULL;

            if(pMe->m_bSuspending == FALSE)
            {
                Notebook_RunFSM(pMe);
            }
            return TRUE;

        default:
            return Notebook_RouteDialogEvent(pMe,eCode,wParam,dwParam);
    }
}

static void Notebook_APPIsReadyTimer(void *pme)
{
	CNotebookMenu *pMe = (CNotebookMenu *)pme;

	if (NULL == pMe)
	{
	    return;
	}

	(void) ISHELL_PostEvent( pMe->m_pShell,AEECLSID_NOTEBOOK,EVT_APPISREADY,0,0);
}

#endif


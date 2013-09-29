/*==============================================================================
//           CAMERA  A P P L E T  M O D U L E
// 文件:
//        TVApp.c
//        版权所有(c) 2009 Anylook Tech. CO.,LTD.
//        
// 文件说明：
//        拍照摄像模块主文件
//                 
//
==============================================================================*/

/*==============================================================================
修改历史记录：
时       间       修 改 人     修改内容、位置及原因
-----------      ----------    -----------------------------------------------

==============================================================================*/


/*==============================================================================

  本文件包含的外部文件
  
==============================================================================*/
#include "TVApp_priv.h"      // 主Applet头文件

/*==============================================================================

  宏定义和常数
  
==============================================================================*/


/*==============================================================================

  相关数据类型定义
  
==============================================================================*/


/*==============================================================================

  本地函数声明
  
==============================================================================*/
//模块相关函数声明

int CTVAppMod_Load(IShell *pIShell, void *ph, IModule **ppMod);

int CTVAppMod_New(int16                nSize, 
                      IShell               *pIShell,
                      void                 *ph,   
                      IModule              **ppMod,
                      PFNMODCREATEINST     pfnMC, 
                      PFNFREEMODDATA       pfnMF);

static uint32 CTVAppMod_AddRef(IModule *po);

static uint32 CTVAppMod_Release(IModule *po);

static int CTVAppMod_CreateInstance(IModule  *po, 
                                        IShell   *pIShell,
                                        AEECLSID ClsId, 
                                        void     **ppObj);

static void CTVAppMod_FreeResources(IModule *po, IHeap *ph, IFileMgr *pfm);

// Applet相关函数声明
// Applet创建和释放函数
static int TVApp_New(IShell *ps, IModule *pIModule, ITVApp **ppObj);

static uint32  TVApp_AddRef(ITVApp *pi);

static uint32  TVApp_Release(ITVApp *pi);

// Applet事件处理函数
static boolean TVApp_HandleEvent(ITVApp   *pi, 
                                     AEEEvent     eCode,
                                     uint16       wParam, 
                                     uint32       dwParam);

// 内部Applet方法声明
static int TVApp_InitAppData(CTVApp *pMe);

static void TVApp_FreeAppData(CTVApp *pMe);

// Applet主状态机处理方法
static void TVApp_RunFSM(CTVApp *pMe);

// 各种外来通知事件的处理函数
//static boolean TVApp_HandleNotifyEvent(CTVApp *pMe, AEENotify *pN);

static void TVApp_APPIsReadyTimer(void *pme);



/*==============================================================================
全局数据
==============================================================================*/
// 重要提示::
//        g_dwAEEStdLibEntry全局变量仅用于SDK !
//        BREW SDK用户不能改写和直接使用本变量。它用作AEEStdLib的一个入口点，不
//        要移动下列三行代码。
#ifdef AEE_SIMULATOR
uint32 g_dwAEEStdLibEntry;
//   #error The TV applet was never intended to be used with the AEE Simulator.
#endif

/*==============================================================================

  本地（静态）数据
  
==============================================================================*/
static CTVAppMod gTVAppMod;

static const VTBL(IModule) gModFuncs =
{
    CTVAppMod_AddRef,    
    CTVAppMod_Release,
    CTVAppMod_CreateInstance,
    CTVAppMod_FreeResources
};

// 只允许一个 TVApp 实例。每次创建 TV Applet 时，返回同一结构指针给 BREW 层。
static CTVApp gTVApp;//={0};

static const VTBL(ITVApp) gTVAppMethods =
{
    TVApp_AddRef,
    TVApp_Release,
    TVApp_HandleEvent
};

/*==============================================================================
函数:
       CTVAppMod_Load

说明:
       模块装载函数。

参数:
       pIShell [in]:IShell接口指针。
       ph :没有使用
       ppMod [out]:
 
返回值:
       int
   
备注:
  
==============================================================================*/
#ifndef AEE_STATIC
	__declspec(dllexport) int AEEMod_Load(IShell   *pIShell,
										  void	   *ph,
										  IModule  **ppMod)
#else

    int CTVAppMod_Load(IShell   *pIShell,
                           void     *ph,
                           IModule  **ppMod)
#endif
{
    // 对于动态Applet，验证AEE STD库版本，以确保库与建立本模块时使用的库版本匹配
    // 。在AEE_SIMULATOR下不必进行本检查。
#if !defined(BREW_STATIC_APP) && !defined(AEE_SIMULATOR)
    if(GET_HELPER_VER() != AEESTDLIB_VER)
    {
        return EVERSIONNOTSUPPORT;
    }
#endif
MSG_FATAL("CTVAppMod_Load================================",0,0,0);
    return CTVAppMod_New(sizeof(CTVAppMod), pIShell, ph, ppMod, NULL, NULL);
}

/*==============================================================================
函数:
       CTVAppMod_New

说明:
       用来初始化TVApp模块虚拟表（也增加引用计数）的助手函数。
  
参数:
       nSize: 没有使用
       pIShell [in]: IShell 接口指针。
       ph : 没有使用
       ppMod [out]:
       pfnMC: 没有使用
       pfnMF: 没有使用
 
返回值:
       int
   
备注:
  
==============================================================================*/
int CTVAppMod_New(int16             nSize,
                      IShell            *pIShell,
                      void              *ph,
                      IModule           **ppMod,            
                      PFNMODCREATEINST  pfnMC,
                      PFNFREEMODDATA    pfnMF)
{
    PARAM_NOT_REF(nSize)
    PARAM_NOT_REF(pfnMC)
    PARAM_NOT_REF(pfnMF)
#ifndef AEE_SIMULATOR
    PARAM_NOT_REF(ph)
#endif
    if(!ppMod)
    {
        return EFAILED;
    }
  
    *ppMod = NULL;
  
// 重要提示:
//        g_dwAEEStdLibEntry全局变量仅用于SDK !
//        BREW SDK用户不能改写和直接使用本变量。它用作AEEStdLib的一个入口点
//        。
#ifdef AEE_SIMULATOR
    g_dwAEEStdLibEntry = (uint32)ph;
    if(!pIShell || !ph)
    {
        return EFAILED;
    }
#else
    if(!pIShell)
    {
        return EFAILED;
    }
#endif
  
    MEMSET(&gTVAppMod,0,sizeof(CTVAppMod));
  
    INIT_VTBL(&gTVAppMod,IModule,gModFuncs);
  
    gTVAppMod.m_nRefs = 1;
  
    *ppMod = (IModule *)&gTVAppMod;
  
    return AEE_SUCCESS;
}

/*==============================================================================
函数:
       CTVAppMod_AddRef

说明:
       TVApp模块引用计数加1，并返回当前的引用数。
  
参数:
       po [in/out]:IModule接口对象指针。
 
返回值:
       当前对TVApp模块的引用数
   
备注:
  
==============================================================================*/
static uint32 CTVAppMod_AddRef(IModule *po)
{
    return(++((CTVAppMod *)po)->m_nRefs);
}

/*==============================================================================
函数:
       CTVAppMod_Release

说明:
       TVApp模块引用计数减1，并返回当前的引用数。

参数:
       po [in/out]:IModule接口对象指针。
 
返回值:
       当前对TVApp模块的引用数
   
备注:
  
==============================================================================*/
static uint32 CTVAppMod_Release(IModule *po)
{
    if (((CTVAppMod *) po)->m_nRefs == 0)
    {
        return 0;
    }
 
    return(--((CTVAppMod *)po)->m_nRefs);
}

/*==============================================================================
函数:
       CTVAppMod_CreateInstance

说明:
       通过创建(初始化)TVApp对象来创建模块实例。
  
参数:
       po [in]:IModule接口对象指针。
       pIShell [in]:IShell接口对象指针。
       ClsId [in]:要创建的实例的类ID。
       ppObj [out]::用于保存对象实例的指针的指针变量。
 
返回值:
       none
   
备注:
       不需用户去调用。
  
==============================================================================*/
static int CTVAppMod_CreateInstance(IModule   *po,
                                        IShell    *pIShell,
                                        AEECLSID  ClsId,
                                        void      **ppObj)
{
    *ppObj = NULL;
 
    if(ClsId != AEECLSID_TVAPP)
    {
        return EFAILED;
    }
 
    if(TVApp_New(pIShell, po, (ITVApp**)ppObj) != SUCCESS)
    {
        return EFAILED;
    }
 
    return SUCCESS;
}

/*==============================================================================
函数:
       CTVAppMod_FreeResources

说明:
       释放TVApp模块资源的函数。

参数:
       po :没有使用。
       IHeap :没有使用。
       pfm ::没有使用。
 
返回值:
       none
   
备注:
       不需用户去调用。
  
==============================================================================*/
static void CTVAppMod_FreeResources(IModule *po, IHeap  *ph, IFileMgr *pfm)
{
    MSG_FATAL("po address = %x", po, 0, 0);
    MSG_FATAL("ph address = %x", ph, 0, 0);
    MSG_FATAL("pfm address = %x", pfm, 0, 0);
    PARAM_NOT_REF(po)
    PARAM_NOT_REF(ph)
    PARAM_NOT_REF(pfm)
}

/*==============================================================================
函数:
       TVApp_New

说明:
       初始化 TVApp Applet 虚拟表和内部数据变量，同时 TVApp Applet 引用计数加1。
  
参数:
       ps [in]: IShell接口对象指针。
       pIModule [in/out]: pIModule接口对象指针。
       ppObj [out]: 指向 ITVApp 对象指针的指针。
 
返回值:
       EFAILED: 失败。
       SUCCESS: 成功。
   
备注:
       不需用户去调用。
       关于Applet结构对象可使用动态分配内存的方式创建，本处采用的是使用静态
       变量的方式。建议对结构比较大的Applet使用静态变量的方式。对于动态分配
       内存的方式需要注意对所分配空间的释放。
  
==============================================================================*/
static int TVApp_New(IShell *ps, IModule *pIModule, ITVApp **ppObj)
{
    int retVal = SUCCESS;
   // ITvUtil* pITvUtil = NULL;
	//CTVApp* pThis = NULL;
	
    if(0 == gTVApp.m_nRefs)
    {
        MEMSET(&gTVApp, 0, sizeof(gTVApp));
        INIT_VTBL(&gTVApp, ITVApp, gTVAppMethods);
  
        gTVApp.m_nRefs = 0;
        gTVApp.m_pShell = ps;
        gTVApp.m_pModule = pIModule;
        gTVApp.pIMMITv=NULL;
        (void)ISHELL_AddRef(ps);
        (void)IMODULE_AddRef(pIModule);
  
        retVal = TVApp_InitAppData(&gTVApp);
		//pThis = CTvUtil_Constructor(ps);
    }
 
    ++gTVApp.m_nRefs;
 
    *ppObj = (ITVApp*)&gTVApp;
 MSG_FATAL("TVApp_New-----------------%d",retVal,0,0);
    return retVal;
}

/*==============================================================================
函数:
       TVApp_AddRef

说明:
       TVApp Applet 引用计数加1，并返回当前的引用数。
  
参数:
       pi [in/out]: ITVApp 接口对象指针。
 
返回值:
       当前对 TVApp Applet 的引用数
   
备注:
  
==============================================================================*/
static uint32 TVApp_AddRef(ITVApp *pi)
{
    register CTVApp *pMe = (CTVApp*)pi;
 
    ASSERT(pMe != NULL);
    ASSERT(pMe->m_nRefs > 0);
 
    return (++pMe->m_nRefs);
}

/*==============================================================================
函数:
       TVApp_Release

说明:
       释放对 TVApp Applet 的引用，引用计数减1，并返回当前的引用数。
       如果当前的引用数为零，还将释放 Applet 及相关资源。
  
参数:
       pi [in/out]: ITVApp 接口对象指针。
 
返回值:
       当前对 TV Applet 的引用数
   
备注:
  
==============================================================================*/
static uint32 TVApp_Release(ITVApp *p)
{
    register CTVApp *pMe = (CTVApp*)p;
 
    if(pMe->m_nRefs == 0)
    {
        return 0;
    }
 
    if(--pMe->m_nRefs)
    {
        return pMe->m_nRefs;
    }
 
    TVApp_FreeAppData(pMe);
    MSG_FATAL("m_pShell address = %x", pMe->m_pShell, 0, 0);
    MSG_FATAL("m_pModule address = %x", pMe->m_pModule, 0, 0);
    (void)ISHELL_Release(pMe->m_pShell);
    (void)IMODULE_Release(pMe->m_pModule);
    #if 0
	if(pMe->pTvSetting)
	{
        MSG_FATAL("pTvSetting address = %x", pMe->pTvSetting, 0, 0);
		FREE(pMe->pTvSetting);
		pMe->pTvSetting = NULL;
	}
    #endif
    // 注意:pMe是静态分配空间，因此不需要释放。FREE()
    return 0;
}

/*==============================================================================
函数:
       TVApp_InitAppData

说明:
       初始化 TVApp Applet 结构数据缺省值。
  
参数:
       pMe [in]: 指向 TVApp Applet 对象结构的指针。该结构包含小程序的特定信息。
 
返回值:
       EFAILED: 初始化失败。
       SUCCESS: 初始化成功。
   
备注:
  
==============================================================================*/
static int TVApp_InitAppData(CTVApp *pMe)
{
    AEEDeviceInfo  di;
	//int Ret = 0;
    uint32 m_tv = 1;   
    if(NULL == pMe)
    {
        return EFAILED;
    }
    
    (void)OEM_SetConfig( CFGI_TV_OR_CAMERA,
	                          &m_tv,
	                          sizeof(uint32));
    // initial applet data
    ISHELL_GetDeviceInfo(pMe->m_pShell, &di);
    pMe->m_rc.x       = 0;
    pMe->m_rc.y       = 0;
    pMe->m_rc.dx      = (int16) di.cxScreen;
    pMe->m_rc.dy      = (int16) di.cyScreen;
 
    pMe->m_cxWidth    = di.cxScreen; 
    pMe->m_cyHeight   = di.cyScreen;
    
    pMe->m_bAppIsReady = FALSE;
    pMe->m_pActiveDlg = NULL;
    pMe->m_eDlgRet = DLGRET_CREATE;
    pMe->m_bNotOverwriteDlgRet = FALSE;
    pMe->m_bIsPreview = FALSE;
    pMe->m_curChnIdx=0;
    pMe->m_bAUTOSCAN=FALSE;
    pMe->m_bAUTOSTOP=FALSE;
    
    pMe->m_bCapturePic = FALSE;
    pMe->m_ePreState = STATE_NULL;
    pMe->m_nMainMenuItemSel = IDS_ITEM_TV;
    //pMe->m_nSelfTimeItemSel = IDS_SELFTIME_OFF;
    pMe->m_nTVCFG = TVSETChannel;
    pMe->m_sCurrentFileName[0] = '\0';
    pMe->m_sCaptureFileName[0] = '\0';
    pMe->m_bCanCapture = TRUE;
    pMe->m_dwMemTotal = 0;
    pMe->m_dwMemFree = 0;
    pMe->m_barW=0;
    pMe->m_barMAXW=1;
    

    //Video计时器
    pMe->m_dwDispTime = 0;
    pMe->m_nNumDays = 0;
    pMe->m_nTicks = 0;
    pMe->m_bRePreview = FALSE;
    pMe->m_nTVState = TV_START;

    pMe->m_pMedia = NULL;
    pMe->m_isFormQuicktest = FALSE;
    pMe->m_sensor_model = -1;
	
	pMe->pTvSetting =(CFG_TvSetting*)MALLOC(sizeof(CFG_TvSetting));
	MEMSET(pMe->pTvSetting,0,sizeof(CFG_TvSetting));
    MEMSET(&pMe->m_CallBack, 0, sizeof(AEECallback));
    
MSG_FATAL("TVApp_InitAppData-----------------START",0,0,0);
   
    if(AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                             AEECLSID_CONFIG,
                                             (void **)&pMe->m_pConfig))
    {
        TVApp_FreeAppData(pMe);
        return EFAILED;
    }
 
    if(AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                            AEECLSID_DISPLAY,
                                            (void **)&pMe->m_pDisplay))
    {
        TVApp_FreeAppData(pMe);
		MSG_FATAL("AEECLSID_DISPLAY-----------------%d",EFAILED,0,0);
        return EFAILED;
    } 
    
    if(AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell, 
                                            AEECLSID_FILEMGR, 
                                            (void **)&pMe->m_pFileMgr))
    {
        TVApp_FreeAppData(pMe);
		MSG_FATAL("AEECLSID_FILEMGR-----------------%d",EFAILED,0,0);
        return EFAILED;
    }
    
    if(AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell, 
                                              AEECLSID_GRAPHICS, 
                                              (void **)&pMe->pGraphics))
      {
          TVApp_FreeAppData(pMe);
          MSG_FATAL("AEECLSID_GRAPHICS-----------------%d",EFAILED,0,0);
          return EFAILED;
      }

    if(AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell, 
                                            AEECLSID_ANNUNCIATOR, 
                                            (void **)&pMe->m_pIAnn))
    {
        TVApp_FreeAppData(pMe);
		MSG_FATAL("AEECLSID_ANNUNCIATOR-----------------%d",EFAILED,0,0);
        return EFAILED;
    }
    
    if(AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_BACKLIGHT, (void**)&pMe->m_pBacklight))
    {
        TVApp_FreeAppData(pMe);
		MSG_FATAL("AEECLSID_BACKLIGHT-----------------%d",EFAILED,0,0);
        return EFAILED;
    }
    
    (void)ICONFIG_GetItem(pMe->m_pConfig,
                          CFGI_BACK_LIGHT,
                         &pMe->m_nBacklightVal,
                          sizeof(pMe->m_nBacklightVal));

#if 0
      {
      int num = 6;
      	int i = 0;
      	int nSize = 0;	 
          MSG_FATAL("CTvUtil_BookmarkOperator_RemoveAll Start",0,0,0); 
              (void)ICONFIG_GetItem(pMe->m_pConfig,
                               CFGI_TV_SETCHANNL,
                               (void*)pMe->pTvSetting,
                               sizeof(CFG_TvSetting));
      	for(; i < num; i++)
      	{
      		nSize = WSTRLEN(pMe->pTvSetting->Bookmark[i].name); 
      		MEMSET (pMe->pTvSetting->Bookmark[i].name, '\0', sizeof(AECHAR) * nSize); 
      
      		nSize = WSTRLEN(pMe->pTvSetting->Bookmark[i].channel); 
      		MEMSET (pMe->pTvSetting->Bookmark[i].channel, '\0', sizeof(AECHAR) * nSize); 
      	}
      	pMe->pTvSetting->Bookmarktotal = 0;
      	pMe->pTvSetting->ChannelCountAble = 0;
           (void)ICONFIG_SetItem(pMe->m_pConfig,
                            CFGI_TV_SETCHANNL,
                            (void*)pMe->pTvSetting,
                            sizeof(CFG_TvSetting));
                 
        }
#endif 
	MSG_FATAL("TVApp_InitAppData-----------------end--%d",SUCCESS,0,0);
    return SUCCESS;

	
} 



/*==============================================================================
函数:
       TVApp_FreeAppData

说明:
       释放 TVApp Applet 相关资源。
  
参数:
       pMe [in]: 指向TVApp Applet 对象结构的指针。该结构包含小程序的特定信息。
 
返回值:
       none
   
备注:
  
==============================================================================*/
static void TVApp_FreeAppData(CTVApp *pMe)
{
    if(NULL == pMe)
    {
        return;
    }
    
    if(pMe->m_pConfig)
    {
        MSG_FATAL("m_pConfig address = %x", pMe->m_pConfig, 0, 0);
        ICONFIG_Release(pMe->m_pConfig);
        pMe->m_pConfig = NULL;
    }
     
    if(pMe->m_pDisplay)
    {
        MSG_FATAL("m_pDisplay address = %x", pMe->m_pDisplay, 0, 0);
        IDISPLAY_Release(pMe->m_pDisplay);
        pMe->m_pDisplay = NULL;
    }

    if(pMe->pGraphics)
      {
          MSG_FATAL("pGraphics address = %x", pMe->pGraphics, 0, 0);
          IGRAPHICS_Release(pMe->pGraphics);
          pMe->pGraphics = NULL;
      }


    if(pMe->m_pFileMgr)
    {
        MSG_FATAL("m_pFileMgr address = %x", pMe->m_pFileMgr, 0, 0);
        IFILEMGR_Release(pMe->m_pFileMgr);
        pMe->m_pFileMgr = NULL;            
    }

    if(pMe->m_pMedia)
    {
        MSG_FATAL("m_pMedia address = %x", pMe->m_pMedia, 0, 0);
        IMEDIA_Release(pMe->m_pMedia);
        pMe->m_pMedia = NULL;
    }

    if(pMe->m_pIAnn)
    {
        MSG_FATAL("m_pIAnn address = %x", pMe->m_pIAnn, 0, 0);
        IANNUNCIATOR_Release(pMe->m_pIAnn);
        pMe->m_pIAnn = NULL;
    }
    
    if(pMe->m_pBacklight)
    {
        MSG_FATAL("m_pBacklight address = %x", pMe->m_pBacklight, 0, 0);
        IBACKLIGHT_Release(pMe->m_pBacklight);
        pMe->m_pBacklight = NULL;
    }
    if(pMe->pIMMITv)
    {
        MSG_FATAL("pIMMITv address = %x", pMe->pIMMITv, 0, 0);
        IMMITv_Release(pMe->pIMMITv);
        pMe->pIMMITv=NULL;

    }
    if(pMe->pTvSetting )
    {
      MSG_FATAL("pTvSetting address = %x", pMe->pTvSetting, 0, 0);
      FREE(pMe->pTvSetting);
      pMe->pTvSetting = NULL;
    }
// if SD Dev is existed, stop SD dev while exitting from TV APP to avoid high current<43mA> consume in idle state
    // Note: if SD card detected after Creating ITV interface, it needn't to stop sd card in this way, cause's SD won't be pushed in stack for low dev priority
    if(pMe->m_bMemoryCardExist)
    {
    #ifdef FEATURE_SUPPORT_VC0848
        vc_union_type vc_data;
        vc_data.dev_run.curr_dev = VC_DEV_SD;
        VC_DeviceControl(VC_ITM_DEV_STOP_I, VC_FUNC_CONTROL, &vc_data);
    #endif
    }
}

/*==============================================================================
函数:
       TVApp_RunFSM
    
说明:
       TVApp Applet有限状态机引擎。
   
参数:
       pMe [in]：指向TVApp Applet对象结构的指针。该结构包含小程序的特定信息。
     
返回值:
       无。
    
备注:
      
==============================================================================*/
static void TVApp_RunFSM(CTVApp *pMe)
{
    NextFSMAction nextFSMAction = NFSMACTION_WAIT;
     
    for( ; ; )
    {
        nextFSMAction = TVApp_ProcessState(pMe);
      
        if(pMe->m_bNotOverwriteDlgRet)
        {
            pMe->m_bNotOverwriteDlgRet = FALSE;
        }
        else
        {
            pMe->m_eDlgRet = DLGRET_CREATE;
        }
      
        if(nextFSMAction == NFSMACTION_WAIT)
        {
            break;
        }
    }
}
    
/*==============================================================================
函数:
       TVApp_HandleEvent
    
说明:
       TVApp Applet事件处理主函数。所有属于本Applet的事件，都将经由该函数处理。
   
参数:
       pi [in]:指向ITVApp接口t对象的指针。
       eCode:BREW事件代码。
       wParam:事件参数
       dwParam [in]:与wParam关联的数据。。
     
返回值:
       TRUE:传入事件被处理。
       FALSE:传入事件没被处理。
    
备注:
       该函数供BREW调用，用户不得调用。
      
==============================================================================*/
static boolean TVApp_HandleEvent(ITVApp  *pi,
                                     AEEEvent     eCode,
                                     uint16       wParam,
                                     uint32       dwParam)
{
    CTVApp *pMe = (CTVApp*)pi;   
    AEEAppStart *as;    
    int  nAscent,nDescent;

    MSG_FATAL("TVApp_HandleEvent-----------------------0x%x",eCode,0,0);
    switch(eCode)
    {
        case EVT_APP_START:
			MSG_FATAL("TVApp_HandleEvent--------EVT_APP_START---------------",0,0,0);
            pMe->m_bAppIsReady = FALSE;
            ASSERT(dwParam != 0);
            as = (AEEAppStart*)dwParam;
            if(NULL != as->pDisplay)
            {
                if(NULL != pMe->m_pDisplay)
                {
                    MSG_FATAL("m_pDisplay address = %x", pMe->m_pDisplay, 0, 0);
                    (void)IDISPLAY_Release(pMe->m_pDisplay);
                    pMe->m_pDisplay = NULL;
                }
          
                pMe->m_pDisplay = as->pDisplay;
                (void)IDISPLAY_AddRef(pMe->m_pDisplay);
            }
            if(as->pszArgs != NULL)
            {
                if(STRCMP(as->pszArgs, "Formquicktest") == 0)
                {
                    pMe->m_isFormQuicktest = TRUE;
                }
            }
            pMe->m_rc = as->rc;
            pMe->m_bSuspending = FALSE;
            // 获得系统的字体大小
            (void)IDISPLAY_GetFontMetrics(pMe->m_pDisplay, AEE_FONT_NORMAL, &nAscent, &nDescent);
            pMe->m_nItemH  = nAscent + nDescent;

            {
                int nDevice;
                nDevice = app_media_scheduler();

                switch(nDevice)
                {
                    case APP_MEDIA_ALLOW:
                        pMe->m_eCurState = STATE_TVWAITING;  //STATE_CMAINMENU                             
                        break;

                    case APP_MEDIA_IMPACT_BY_MP3:                    
                    {          
                    	//Del By zzg 2010_09_01
                    	/*
                    	pMe->m_eCurState = STATE_CPOPMSG;
                        pMe->m_nMsgTimeout = 0; 
                        pMe->m_wMsgID = IDS_CONFIRM_OFF_MP;
                        */
                        //Del End
                        
                        //Add By zzg 2010_09_01
                        pMe->m_eCurState = STATE_CMAINMENU;    
                        break;
                        //Add End                        
                        
                    }

                    case APP_MEDIA_IMPACT_BY_FM:
                        pMe->m_eCurState = STATE_CPOPMSG;
                        pMe->m_nMsgTimeout = 0; 
                        pMe->m_wMsgID = IDS_CONFIRM_OFF_FM;
                        break;

                    default:
                        break;                   
                }
            }

            SetDeviceState(DEVICE_TYPE_CAMERA, DEVICE_CAMERA_STATE_ON);
      
            // 开始SettingMenu状态机
            TVApp_RunFSM(pMe);
            return TRUE;
      
        case EVT_APP_STOP:
            MSG_FATAL("TVAPP-EVT_APP_STOP",0,0,0);
            if(pMe->m_pDisplay != NULL)
            {
                MSG_FATAL("m_pDisplay address = %x", pMe->m_pDisplay, 0, 0);
                (void)IDISPLAY_Release(pMe->m_pDisplay);
                pMe->m_pDisplay = NULL;
            }
            pMe->m_bSuspending = TRUE; 
            SetDeviceState(DEVICE_TYPE_CAMERA, DEVICE_CAMERA_STATE_OFF);
            if(pMe->m_bIsPreview == TRUE && pMe->pIMMITv)
            {
                //ICAMERAEX_RegisterNotify(pMe->m_pTV,NULL, NULL);
              //  ICAMERA_Stop(pMe->pIMMITv);
            }
            (void)ICONFIG_SetItem(pMe->m_pConfig,
                                  CFGI_BACK_LIGHT,
                                  &pMe->m_nBacklightVal,
                                  sizeof(pMe->m_nBacklightVal));
            IBACKLIGHT_Enable(pMe->m_pBacklight);
            return TRUE;
      
        case EVT_APP_SUSPEND:
            MSG_FATAL("TVAPP-EVT_APP_SUSPEND",0,0,0);
            ISHELL_CancelTimer(pMe->m_pShell, NULL, pMe);// 取消所有定时器
            
            pMe->m_bSuspending = TRUE;
            if(pMe->m_bIsPreview == TRUE && pMe->pIMMITv)
            {
               // ICAMERAEX_RegisterNotify(pMe->m_pTV,NULL, NULL);
              //  ICAMERA_Stop(pMe->pIMMITv);
            }
            
            if(pMe->m_nTVState == TV_START && pMe->pIMMITv)
            {
                MSG_FATAL("m_pTV address = %x", pMe->pIMMITv, 0, 0);
                IMMITv_Release(pMe->pIMMITv);
                pMe->pIMMITv = NULL;
            }
            pMe->m_bIsPreview = FALSE;
            return TRUE;
            
        case EVT_ALARM:
            MSG_FATAL("TVAPP-EVT_ALARM",0,0,0);
            if(pMe->m_nTVState == TV_START && pMe->pIMMITv)
            {
                MSG_FATAL("m_pTV address = %x", pMe->pIMMITv, 0, 0);
                IMMITv_Release(pMe->pIMMITv);
                pMe->pIMMITv = NULL;
            }
            pMe->m_bIsPreview = FALSE;
            return TRUE;
            
        case EVT_APP_RESUME:
            MSG_FATAL("TVAPP-EVT_APP_RESUME",0,0,0);
           // TVApp_InitTVCheck(pMe); 
            as = (AEEAppStart*)dwParam;
            pMe->m_bSuspending = FALSE;
      
            if(pMe->m_pDisplay != NULL)
            {
                MSG_FATAL("m_pDisplay address = %x", pMe->m_pDisplay, 0, 0);
                (void)IDISPLAY_Release(pMe->m_pDisplay);
                pMe->m_pDisplay = NULL;
            }
            pMe->m_pDisplay = as->pDisplay;
            (void)IDISPLAY_AddRef(pMe->m_pDisplay);
			
			pMe->m_nLeftTime = 0;
			
            pMe->m_rc = as->rc;
            {
                byte nBackLight;
                nBackLight = OEMNV_BL_ALWAYS_ON;
                (void)ICONFIG_SetItem(pMe->m_pConfig,
                                      CFGI_BACK_LIGHT,
                                      &nBackLight,
                                      sizeof(nBackLight));
                IBACKLIGHT_Enable(pMe->m_pBacklight);     
            }
            TVApp_RunFSM(pMe);
            return TRUE;
      
        case EVT_DIALOG_INIT:
            MSG_FATAL("TVAPP-EVT_DIALOG_INIT",0,0,0);
            pMe->m_bAppIsReady = FALSE;
            pMe->m_pActiveDlg = (IDialog*)dwParam;
            pMe->m_pActiveDlgID = wParam;   
            return TVApp_RouteDialogEvent(pMe, eCode, wParam, dwParam);
      
        case EVT_DIALOG_START:
            MSG_FATAL("TVAPP-EVT_DIALOG_START",0,0,0);
            (void)ISHELL_SetTimer(pMe->m_pShell,
                                  APPISREADY_TIMER,
                                  TVApp_APPIsReadyTimer,
                                  pMe);
            return TVApp_RouteDialogEvent(pMe, eCode, wParam, dwParam);
            
        case EVT_USER_REDRAW:
            MSG_FATAL("TVAPP-EVT_USER_REDRAW",0,0,0);
            (void)TVApp_RouteDialogEvent(pMe, eCode, wParam, dwParam);
            return TRUE;
            
        case EVT_CAMERA_NOTIFY:
            MSG_FATAL("TVAPP-EVT_CAMERA_NOTIFY",0,0,0);
            TVApp_AppEventNotify(pMe, (int16)wParam, (int16)dwParam);
            return TRUE;
            
        case EVT_APPISREADY:
            pMe->m_bAppIsReady = TRUE;  //7001
            MSG_FATAL("TVAPP-EVT_APPISREADY",0,0,0);
            return TRUE;
      
        case EVT_KEY_PRESS:
        case EVT_KEY_RELEASE:
        case EVT_KEY:
        case EVT_COMMAND:     
			#if !defined(FEATURE_VERSION_K212) && !defined(FEATURE_VERSION_EC99)&&!defined(FEATURE_QVGA_INHERIT_K212)
            if(!pMe->m_bAppIsReady)
            {
                return TRUE;
            }
			#endif
			MSG_FATAL("case EVT_COMMAND:  ----------------------",0,0,0);
            return TVApp_RouteDialogEvent(pMe, eCode, wParam, dwParam);
      
        case EVT_DIALOG_END:
             MSG_FATAL("TVAPP-EVT_DIALOG_END",0,0,0);
            if(wParam == 0)
            {
                return TRUE;
            }
            pMe->m_bAppIsReady = FALSE;
            (void)TVApp_RouteDialogEvent(pMe, eCode, wParam, dwParam);
            pMe->m_pActiveDlg = NULL; 
            
            //Applet被挂起，不跑状态机，等待Applet返回。（注意：EVT_APP_SUSPEND
            //事件在EVT_DIALOG_END事件前发出。
            if(pMe->m_bSuspending == FALSE)
            {
                //开始TVApp状态机
                TVApp_RunFSM(pMe);
            }
            return TRUE;
#ifdef FEATURE_LCD_TOUCH_ENABLE//WLH ADD FOR LCD TOUCH

			  case EVT_USER:
                  MSG_FATAL("TVAPP-EVT_USER",0,0,0);
				  if(wParam == AVK_CLR)
				  {
					  eCode = EVT_KEY;
				  }
				  else if((wParam == AVK_SELECT) || (wParam == AVK_INFO))
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
				  return TVApp_RouteDialogEvent(pMe, eCode, wParam, dwParam);
			  break;
#endif      
        default:
            //将接收到的事件路由至当前活动的对话框事件处理函数。
            return TVApp_RouteDialogEvent(pMe, eCode, wParam, dwParam);
    }

    return FALSE;
} //TVApp_HandleEvent()

/*==============================================================================
函数:
       TVApp_APPIsReadyTimer
  
说明:
       定时器回调函数。主要用于控制快速按键问题的处理。
 
参数:
       pme [in]：void *类型指针，暗指CTVApp结构指针。
   
返回值:
       无。
  
备注:
    
==============================================================================*/
static void TVApp_APPIsReadyTimer(void *pme)
{
    CTVApp *pMe = (CTVApp *)pme;
    
    if(NULL == pMe)
    {
        return;
    }
    
    (void)ISHELL_PostEvent(pMe->m_pShell,
                           AEECLSID_TVAPP,
                           EVT_APPISREADY,
                           0,
                           0);
}

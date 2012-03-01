/*==============================================================================
//           CAMERA  A P P L E T  M O D U L E
// 文件:
//        CameraApp.c
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
#include "CameraApp_priv.h"      // 主Applet头文件

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

int CCameraAppMod_Load(IShell *pIShell, void *ph, IModule **ppMod);

int CCameraAppMod_New(int16                nSize, 
                      IShell               *pIShell,
                      void                 *ph,   
                      IModule              **ppMod,
                      PFNMODCREATEINST     pfnMC, 
                      PFNFREEMODDATA       pfnMF);

static uint32 CCameraAppMod_AddRef(IModule *po);

static uint32 CCameraAppMod_Release(IModule *po);

static int CCameraAppMod_CreateInstance(IModule  *po, 
                                        IShell   *pIShell,
                                        AEECLSID ClsId, 
                                        void     **ppObj);

static void CCameraAppMod_FreeResources(IModule *po, IHeap *ph, IFileMgr *pfm);

// Applet相关函数声明
// Applet创建和释放函数
static int CameraApp_New(IShell *ps, IModule *pIModule, ICameraApp **ppObj);

static uint32  CameraApp_AddRef(ICameraApp *pi);

static uint32  CameraApp_Release(ICameraApp *pi);

// Applet事件处理函数
static boolean CameraApp_HandleEvent(ICameraApp   *pi, 
                                     AEEEvent     eCode,
                                     uint16       wParam, 
                                     uint32       dwParam);

// 内部Applet方法声明
static int CameraApp_InitAppData(CCameraApp *pMe);

static void CameraApp_FreeAppData(CCameraApp *pMe);

// Applet主状态机处理方法
static void CameraApp_RunFSM(CCameraApp *pMe);

// 各种外来通知事件的处理函数
//static boolean CameraApp_HandleNotifyEvent(CCameraApp *pMe, AEENotify *pN);

static void CameraApp_APPIsReadyTimer(void *pme);

/*==============================================================================
全局数据
==============================================================================*/
// 重要提示::
//        g_dwAEEStdLibEntry全局变量仅用于SDK !
//        BREW SDK用户不能改写和直接使用本变量。它用作AEEStdLib的一个入口点，不
//        要移动下列三行代码。
#ifdef AEE_SIMULATOR
uint32 g_dwAEEStdLibEntry;
//   #error The Camera applet was never intended to be used with the AEE Simulator.
#endif

/*==============================================================================

  本地（静态）数据
  
==============================================================================*/
static CCameraAppMod gCameraAppMod;

static const VTBL(IModule) gModFuncs =
{
    CCameraAppMod_AddRef,    
    CCameraAppMod_Release,
    CCameraAppMod_CreateInstance,
    CCameraAppMod_FreeResources
};

// 只允许一个 CameraApp 实例。每次创建 Camera Applet 时，返回同一结构指针给 BREW 层。
static CCameraApp gCameraApp;//={0};

static const VTBL(ICameraApp) gCameraAppMethods =
{
    CameraApp_AddRef,
    CameraApp_Release,
    CameraApp_HandleEvent
};

/*==============================================================================
函数:
       CCameraAppMod_Load

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
                                      void     *ph,
                                      IModule  **ppMod)
#else
    int CCameraAppMod_Load(IShell   *pIShell,
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
    return CCameraAppMod_New(sizeof(CCameraAppMod), pIShell, ph, ppMod, NULL, NULL);
}

/*==============================================================================
函数:
       CCameraAppMod_New

说明:
       用来初始化CameraApp模块虚拟表（也增加引用计数）的助手函数。
  
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
int CCameraAppMod_New(int16             nSize,
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
  
    MEMSET(&gCameraAppMod,0,sizeof(CCameraAppMod));
  
    INIT_VTBL(&gCameraAppMod,IModule,gModFuncs);
  
    gCameraAppMod.m_nRefs = 1;
  
    *ppMod = (IModule *)&gCameraAppMod;
  
    return AEE_SUCCESS;
}

/*==============================================================================
函数:
       CCameraAppMod_AddRef

说明:
       CameraApp模块引用计数加1，并返回当前的引用数。
  
参数:
       po [in/out]:IModule接口对象指针。
 
返回值:
       当前对CameraApp模块的引用数
   
备注:
  
==============================================================================*/
static uint32 CCameraAppMod_AddRef(IModule *po)
{
    return(++((CCameraAppMod *)po)->m_nRefs);
}

/*==============================================================================
函数:
       CCameraAppMod_Release

说明:
       CameraApp模块引用计数减1，并返回当前的引用数。

参数:
       po [in/out]:IModule接口对象指针。
 
返回值:
       当前对CameraApp模块的引用数
   
备注:
  
==============================================================================*/
static uint32 CCameraAppMod_Release(IModule *po)
{
    if (((CCameraAppMod *) po)->m_nRefs == 0)
    {
        return 0;
    }
 
    return(--((CCameraAppMod *)po)->m_nRefs);
}

/*==============================================================================
函数:
       CCameraAppMod_CreateInstance

说明:
       通过创建(初始化)CameraApp对象来创建模块实例。
  
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
static int CCameraAppMod_CreateInstance(IModule   *po,
                                        IShell    *pIShell,
                                        AEECLSID  ClsId,
                                        void      **ppObj)
{
    *ppObj = NULL;
 
    if(ClsId != AEECLSID_APP_CAMERA)
    {
        return EFAILED;
    }
 
    if(CameraApp_New(pIShell, po, (ICameraApp**)ppObj) != SUCCESS)
    {
        return EFAILED;
    }
 
    return SUCCESS;
}

/*==============================================================================
函数:
       CCameraAppMod_FreeResources

说明:
       释放CameraApp模块资源的函数。

参数:
       po :没有使用。
       IHeap :没有使用。
       pfm ::没有使用。
 
返回值:
       none
   
备注:
       不需用户去调用。
  
==============================================================================*/
static void CCameraAppMod_FreeResources(IModule *po, IHeap  *ph, IFileMgr *pfm)
{
    PARAM_NOT_REF(po)
    PARAM_NOT_REF(ph)
    PARAM_NOT_REF(pfm)
}

/*==============================================================================
函数:
       CameraApp_New

说明:
       初始化 CameraApp Applet 虚拟表和内部数据变量，同时 CameraApp Applet 引用计数加1。
  
参数:
       ps [in]: IShell接口对象指针。
       pIModule [in/out]: pIModule接口对象指针。
       ppObj [out]: 指向 ICameraApp 对象指针的指针。
 
返回值:
       EFAILED: 失败。
       SUCCESS: 成功。
   
备注:
       不需用户去调用。
       关于Applet结构对象可使用动态分配内存的方式创建，本处采用的是使用静态
       变量的方式。建议对结构比较大的Applet使用静态变量的方式。对于动态分配
       内存的方式需要注意对所分配空间的释放。
  
==============================================================================*/
static int CameraApp_New(IShell *ps, IModule *pIModule, ICameraApp **ppObj)
{
    int retVal = SUCCESS;
 
    if(0 == gCameraApp.m_nRefs)
    {
        MEMSET(&gCameraApp, 0, sizeof(gCameraApp));
        INIT_VTBL(&gCameraApp, ICameraApp, gCameraAppMethods);
  
        gCameraApp.m_nRefs = 0;
        gCameraApp.m_pShell = ps;
        gCameraApp.m_pModule = pIModule;
  
        (void)ISHELL_AddRef(ps);
        (void)IMODULE_AddRef(pIModule);
  
        retVal = CameraApp_InitAppData(&gCameraApp);
    }
 
    ++gCameraApp.m_nRefs;
 
    *ppObj = (ICameraApp*)&gCameraApp;
 
    return retVal;
}

/*==============================================================================
函数:
       CameraApp_AddRef

说明:
       CameraApp Applet 引用计数加1，并返回当前的引用数。
  
参数:
       pi [in/out]: ICameraApp 接口对象指针。
 
返回值:
       当前对 CameraApp Applet 的引用数
   
备注:
  
==============================================================================*/
static uint32 CameraApp_AddRef(ICameraApp *pi)
{
    register CCameraApp *pMe = (CCameraApp*)pi;
 
    ASSERT(pMe != NULL);
    ASSERT(pMe->m_nRefs > 0);
 
    return (++pMe->m_nRefs);
}

/*==============================================================================
函数:
       CameraApp_Release

说明:
       释放对 CameraApp Applet 的引用，引用计数减1，并返回当前的引用数。
       如果当前的引用数为零，还将释放 Applet 及相关资源。
  
参数:
       pi [in/out]: ICameraApp 接口对象指针。
 
返回值:
       当前对 Camera Applet 的引用数
   
备注:
  
==============================================================================*/
static uint32 CameraApp_Release(ICameraApp *p)
{
    register CCameraApp *pMe = (CCameraApp*)p;
 
    if(pMe->m_nRefs == 0)
    {
        return 0;
    }
 
    if(--pMe->m_nRefs)
    {
        return pMe->m_nRefs;
    }
 
    CameraApp_FreeAppData(pMe);
 
    (void)ISHELL_Release(pMe->m_pShell);
    (void)IMODULE_Release(pMe->m_pModule);
 
    // 注意:pMe是静态分配空间，因此不需要释放。FREE()
    return 0;
}

/*==============================================================================
函数:
       CameraApp_InitAppData

说明:
       初始化 CameraApp Applet 结构数据缺省值。
  
参数:
       pMe [in]: 指向 CameraApp Applet 对象结构的指针。该结构包含小程序的特定信息。
 
返回值:
       EFAILED: 初始化失败。
       SUCCESS: 初始化成功。
   
备注:
  
==============================================================================*/
static int CameraApp_InitAppData(CCameraApp *pMe)
{
    AEEDeviceInfo  di;
#ifdef FEATURE_ANALOG_TV
    uint32 m_tv = 0;
#endif
    if(NULL == pMe)
    {
        return EFAILED;
    }
#ifdef FEATURE_ANALOG_TV
    (void)OEM_SetConfig( CFGI_TV_OR_CAMERA,
	                          &m_tv,
	                          sizeof(uint32));
#endif
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
    pMe->m_isPicCamera = FALSE;
    
    pMe->m_bCapturePic = FALSE;
    pMe->m_ePreState = STATE_NULL;
    pMe->m_nMainMenuItemSel = IDS_ITEM_CAMERA;
    pMe->m_nSelfTimeItemSel = IDS_SELFTIME_OFF;
    pMe->m_nCameraCFG = CAMERACFGFIRST;
    pMe->m_sCurrentFileName[0] = '\0';
    pMe->m_sCaptureFileName[0] = '\0';
    pMe->m_bCanCapture = TRUE;
    pMe->m_dwMemTotal = 0;
    pMe->m_dwMemFree = 0;
    

    //Video计时器
    pMe->m_dwDispTime = 0;
    pMe->m_nNumDays = 0;
    pMe->m_nTicks = 0;
    pMe->m_bRePreview = FALSE;
    pMe->m_nCameraState = CAM_START;

    pMe->m_pCamera = NULL;  
    pMe->m_pMedia = NULL;
    pMe->m_isFormQuicktest = FALSE;
#ifdef FEATURE_USES_MMS  
    pMe->m_isFormMMS = FALSE;
#endif

#ifdef CAMERA_RECORD_DEBUG
    pMe->m_isStartFromFacebook = TRUE;
    pMe->m_isRecordMode = TRUE;
#else
	pMe->m_isStartFromFacebook = FALSE;
    pMe->m_isRecordMode = FALSE;
#endif
    pMe->m_sensor_model = -1;
    MEMSET(&pMe->m_CallBack, 0, sizeof(AEECallback));

    if(AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                             AEECLSID_CONFIG,
                                             (void **)&pMe->m_pConfig))
    {
        CameraApp_FreeAppData(pMe);
        return EFAILED;
    }

#ifndef FEATURE_DSP
    if(AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                            AEECLSID_DISPLAY,
                                            (void **)&pMe->m_pDisplay))
    {
        CameraApp_FreeAppData(pMe);
        return EFAILED;
    }
#endif
    
    if(AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell, 
                                            AEECLSID_FILEMGR, 
                                            (void **)&pMe->m_pFileMgr))
    {
        CameraApp_FreeAppData(pMe);
        return EFAILED;
    }

    if(AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell, 
                                            AEECLSID_ANNUNCIATOR, 
                                            (void **)&pMe->m_pIAnn))
    {
        CameraApp_FreeAppData(pMe);
        return EFAILED;
    }
    return SUCCESS;
}

/*==============================================================================
函数:
       CameraApp_FreeAppData

说明:
       释放 CameraApp Applet 相关资源。
  
参数:
       pMe [in]: 指向CameraApp Applet 对象结构的指针。该结构包含小程序的特定信息。
 
返回值:
       none
   
备注:
  
==============================================================================*/
static void CameraApp_FreeAppData(CCameraApp *pMe)
{
    if(NULL == pMe)
    {
        return;
    }
    
    if(pMe->m_pConfig)
    {
        ICONFIG_Release(pMe->m_pConfig);
        pMe->m_pConfig = NULL;
    }
     
    if(pMe->m_pDisplay)
    {
        IDISPLAY_Release(pMe->m_pDisplay);
        pMe->m_pDisplay = NULL;
    }

    if(pMe->m_pCamera)
    {
        ICAMERA_Release(pMe->m_pCamera);
        pMe->m_pCamera = NULL;
    }

    if(pMe->m_pFileMgr)
    {
        IFILEMGR_Release(pMe->m_pFileMgr);
        pMe->m_pFileMgr = NULL;            
    }

    if(pMe->m_pMedia)
    {
        IMEDIA_Release(pMe->m_pMedia);
        pMe->m_pMedia = NULL;
    }

    if(pMe->m_pIAnn)
    {
        IANNUNCIATOR_Release(pMe->m_pIAnn);
        pMe->m_pIAnn = NULL;
    }
// if SD Dev is existed, stop SD dev while exitting from Camera APP to avoid high current<43mA> consume in idle state
    // Note: if SD card detected after Creating ICamera interface, it needn't to stop sd card in this way, cause's SD won't be pushed in stack for low dev priority
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
       CameraApp_RunFSM
    
说明:
       CameraApp Applet有限状态机引擎。
   
参数:
       pMe [in]：指向CameraApp Applet对象结构的指针。该结构包含小程序的特定信息。
     
返回值:
       无。
    
备注:
      
==============================================================================*/
static void CameraApp_RunFSM(CCameraApp *pMe)
{
    NextFSMAction nextFSMAction = NFSMACTION_WAIT;
     
    for( ; ; )
    {
        nextFSMAction = CameraApp_ProcessState(pMe);
      
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
       CameraApp_HandleEvent
    
说明:
       CameraApp Applet事件处理主函数。所有属于本Applet的事件，都将经由该函数处理。
   
参数:
       pi [in]:指向ICameraApp接口t对象的指针。
       eCode:BREW事件代码。
       wParam:事件参数
       dwParam [in]:与wParam关联的数据。。
     
返回值:
       TRUE:传入事件被处理。
       FALSE:传入事件没被处理。
    
备注:
       该函数供BREW调用，用户不得调用。
      
==============================================================================*/
static boolean CameraApp_HandleEvent(ICameraApp  *pi,
                                     AEEEvent     eCode,
                                     uint16       wParam,
                                     uint32       dwParam)
{
    CCameraApp *pMe = (CCameraApp*)pi;   
    AEEAppStart *as;    
    int  nAscent,nDescent;


    switch(eCode)
    {
        case EVT_APP_START:
            pMe->m_bAppIsReady = FALSE;
            ASSERT(dwParam != 0);
            as = (AEEAppStart*)dwParam;
            if(NULL != as->pDisplay)
            {
                if(NULL != pMe->m_pDisplay)
                {
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
                else if( STRCMP(as->pszArgs, "facebook") == 0 )
                {
					pMe->m_isStartFromFacebook = TRUE;
                }
                else if( STRCMP(as->pszArgs, "record") == 0 )
                {
					pMe->m_isRecordMode = TRUE;
                }
#ifdef FEATURE_USES_MMS                  
                else if( STRCMP(as->pszArgs, "MMS") == 0 )
                {
					pMe->m_isFormMMS = TRUE;
                }           
#endif                
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
						if ((pMe->m_isRecordMode == TRUE)
#ifdef FEATURE_USES_MMS  
                            || (pMe->m_isFormMMS)
#endif                            
                            )
						{
							pMe->m_eCurState = STATE_CPOPMSG;
						}
						else
						{
                        	pMe->m_eCurState = STATE_CMAINMENU;                               
						}
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
                        //pMe->m_eCurState = STATE_CMAINMENU;   
                        
						if ((pMe->m_isRecordMode == TRUE)
#ifdef FEATURE_USES_MMS  
                            || (pMe->m_isFormMMS)
#endif                            
                            )
						{
							pMe->m_eCurState = STATE_CPOPMSG;
						}
						else
						{
                        	pMe->m_eCurState = STATE_CMAINMENU;                               
						}
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
            CameraApp_RunFSM(pMe);
            return TRUE;
      
        case EVT_APP_STOP:
            if(pMe->m_pDisplay != NULL)
            {
                (void)IDISPLAY_Release(pMe->m_pDisplay);
                pMe->m_pDisplay = NULL;
            }
            pMe->m_bSuspending = TRUE; 
            SetDeviceState(DEVICE_TYPE_CAMERA, DEVICE_CAMERA_STATE_OFF);
            if(pMe->m_bIsPreview == TRUE && pMe->m_pCamera)
            {
#ifndef FEATURE_DSP
                ICAMERA_RegisterNotify(pMe->m_pCamera,NULL, NULL);
#endif
                ICAMERA_Stop(pMe->m_pCamera);
            }
            return TRUE;
      
        case EVT_APP_SUSPEND:
            ISHELL_CancelTimer(pMe->m_pShell, NULL, pMe);// 取消所有定时器
            
            pMe->m_bSuspending = TRUE;
            if(pMe->m_bIsPreview == TRUE && pMe->m_pCamera)
            {
#ifndef FEATURE_DSP
                ICAMERA_RegisterNotify(pMe->m_pCamera,NULL, NULL);
#endif
                ICAMERA_Stop(pMe->m_pCamera);
            }
            
            if(pMe->m_nCameraState == CAM_START && pMe->m_pCamera)
            {
                ICAMERA_Release(pMe->m_pCamera);
                pMe->m_pCamera = NULL;
            }
            pMe->m_bIsPreview = FALSE;
            return TRUE;
            
        case EVT_ALARM:
            if(pMe->m_nCameraState == CAM_START && pMe->m_pCamera)
            {
                ICAMERA_Release(pMe->m_pCamera);
                pMe->m_pCamera = NULL;
            }
            pMe->m_bIsPreview = FALSE;
            return TRUE;
            
        case EVT_APP_RESUME:
            CameraApp_InitCameraCheck(pMe); 
            as = (AEEAppStart*)dwParam;
            pMe->m_bSuspending = FALSE;
      
            if(pMe->m_pDisplay != NULL)
            {
                (void)IDISPLAY_Release(pMe->m_pDisplay);
                pMe->m_pDisplay = NULL;
            }
            pMe->m_pDisplay = as->pDisplay;
            (void)IDISPLAY_AddRef(pMe->m_pDisplay);
			
			pMe->m_nLeftTime = 0;
			
            pMe->m_rc = as->rc;
            CameraApp_RunFSM(pMe);
            return TRUE;
      
        case EVT_DIALOG_INIT:
            pMe->m_bAppIsReady = FALSE;
            pMe->m_pActiveDlg = (IDialog*)dwParam;
            pMe->m_pActiveDlgID = wParam;   
            return CameraApp_RouteDialogEvent(pMe, eCode, wParam, dwParam);
      
        case EVT_DIALOG_START:
            (void)ISHELL_SetTimer(pMe->m_pShell,
                                  APPISREADY_TIMER,
                                  CameraApp_APPIsReadyTimer,
                                  pMe);
            return CameraApp_RouteDialogEvent(pMe, eCode, wParam, dwParam);
            
        case EVT_USER_REDRAW:
            (void)CameraApp_RouteDialogEvent(pMe, eCode, wParam, dwParam);
            return TRUE;
            
        case EVT_CAMERA_NOTIFY:
            CameraApp_AppEventNotify(pMe, (int16)wParam, (int16)dwParam);
            return TRUE;
            
        case EVT_APPISREADY:
            pMe->m_bAppIsReady = TRUE;
            return TRUE;
      
        case EVT_KEY_PRESS:
        case EVT_KEY_RELEASE:
        case EVT_KEY:
        case EVT_COMMAND:    
#ifdef FEATURE_LCD_TOUCH_ENABLE
		case EVT_PEN_UP:
#endif          
            if(!pMe->m_bAppIsReady)
            {
                return TRUE;
            }

            return CameraApp_RouteDialogEvent(pMe, eCode, wParam, dwParam);
      
        case EVT_DIALOG_END:
            if(wParam == 0)
            {
                return TRUE;
            }
            pMe->m_bAppIsReady = FALSE;
            (void)CameraApp_RouteDialogEvent(pMe, eCode, wParam, dwParam);
            pMe->m_pActiveDlg = NULL; 
            
            //Applet被挂起，不跑状态机，等待Applet返回。（注意：EVT_APP_SUSPEND
            //事件在EVT_DIALOG_END事件前发出。
            if(pMe->m_bSuspending == FALSE)
            {
                //开始CameraApp状态机
                CameraApp_RunFSM(pMe);
            }
            return TRUE;
#ifdef FEATURE_LCD_TOUCH_ENABLE//WLH ADD FOR LCD TOUCH

			  case EVT_USER:
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
				  return CameraApp_RouteDialogEvent(pMe, eCode, wParam, dwParam);
			  break;
#endif      
        default:
            //将接收到的事件路由至当前活动的对话框事件处理函数。
            return CameraApp_RouteDialogEvent(pMe, eCode, wParam, dwParam);
    }

    return FALSE;
} //CameraApp_HandleEvent()

/*==============================================================================
函数:
       CameraApp_APPIsReadyTimer
  
说明:
       定时器回调函数。主要用于控制快速按键问题的处理。
 
参数:
       pme [in]：void *类型指针，暗指CCameraApp结构指针。
   
返回值:
       无。
  
备注:
    
==============================================================================*/
static void CameraApp_APPIsReadyTimer(void *pme)
{
    CCameraApp *pMe = (CCameraApp *)pme;
    
    if(NULL == pMe)
    {
        return;
    }
    
    (void)ISHELL_PostEvent(pMe->m_pShell,
                           AEECLSID_APP_CAMERA,
                           EVT_APPISREADY,
                           0,
                           0);
}



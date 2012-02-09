/*==============================================================================
// 文件：
//        Converter.c
//        2008-10-28 Gemsea草拟版本(Draft Version)
//        版权所有(c) 2008 Anylook
//        
// 文件说明：
//        
// 作者：Gemsea
// 创建日期：2008-10-28
// 当前版本：Draft
==============================================================================*/

/*==============================================================================
修改历史记录：
时       间      修 改 人       修改内容、位置及原因
-----------      ----------     -----------------------------------------------
08-10-28                   Initial version
==============================================================================*/


/*==============================================================================
                                 本文件包含的外部文件
==============================================================================*/
#include "Converter_priv.h"

/*==============================================================================
                                 类型定义
==============================================================================*/

/*==============================================================================
                                 函数声明
==============================================================================*/
/*----------------------模块相关函数声明---------------------*/

int  ConverterMod_Load(IShell *ps, void *pHelpers, IModule** ppMod);

int  ConverterMod_New(int16  nSize, IShell *ps, void *pHelpers, IModule **ppMod, PFNMODCREATEINST pfnMC, PFNFREEMODDATA pfnMF);

static int ConverterMod_CreateInstance(IModule *po, IShell *pIShell, AEECLSID ClsId,  void ** ppObj);

static uint32 ConverterMod_AddRef(IModule *po);

static uint32 ConverterMod_Release(IModule *po);

static void ConverterMod_FreeResources(IModule  *po, IHeap  *ph, IFileMgr *pfm);

/*----------------------Applet相关函数声明---------------------*/
static int  Converter_New(IShell* pIShell, IModule* po, IConverter** ppObj);

static uint32  Converter_AddRef(IConverter *pi);

static uint32  Converter_Release (IConverter *pi);

static boolean Converter_HandleEvent(IConverter *pi, AEEEvent eCode, uint16  wParam, uint32 dwParam);

static int Converter_InitAppData(CConverter *pMe);

static void Converter_FreeAppData(CConverter *pMe);

static void Converter_RunFSM(CConverter *pMe);

static void Converter_APPIsReadyTimer(void *pme);

// 初始化单位换算各菜单位置
static void Converter_SetMenuPosition(CConverter *pMe);

/*==============================================================================
                                 全局数据
==============================================================================*/
    // 重要提示：
    //        g_dwAEEStdLibEntry全局变量仅用于SDK ！
    //        BREW SDK用户不能改写和直接使用本变量。它用作AEEStdLib的一个入口点，不
    //        要移动下列三行代码。
#ifdef AEE_SIMULATOR
       uint32 g_dwAEEStdLibEntry;
#endif


/*==============================================================================
                                 本地（静态）数据
==============================================================================*/
static ConverterMod gConverterMod;
    
static const VTBL(IModule) gModFuncs =
{
    ConverterMod_AddRef,
    ConverterMod_Release,
    ConverterMod_CreateInstance,
    ConverterMod_FreeResources
};


// 只允许一个Converter实例。每次创建Converter Applet时，
// 返回同一结构指针给BREW层。
static CConverter gConverter;

static const VTBL(IConverter) gConverterMethods =
{
    Converter_AddRef,
    Converter_Release,
    Converter_HandleEvent
};

/*==============================================================================
函数:
       ConverterMod_Load

说明:
       模块装载函数。

参数:
       ps [in]：IShell接口指针。
       pHelpers ：没有使用
       ppMod [out]：

返回值:
       int

备注:

==============================================================================*/
#ifndef BREW_STATIC_APP
__declspec(dllexport) int AEEMod_Load(IShell   *ps,
                                                        void     *pHelpers,
                                                        IModule **ppMod)
#else
int  ConverterMod_Load(IShell *ps, void *pHelpers, IModule** ppMod)
#endif
{
#if !defined(BREW_STATIC_APP) && !defined(AEE_SIMULATOR)
    if (GET_HELPER_VER() != AEESTDLIB_VER)
    {
        return EVERSIONNOTSUPPORT;
    }
#endif
   return ConverterMod_New((sizeof(ConverterMod)), ps, pHelpers, ppMod, NULL, NULL);
}

/*==============================================================================
函数: 
       ConverterMod_New
       
说明: 
       用来初始化Converter模块虚拟表（也增加引用计数）的助手函数。
       
参数: 
       nSize:        没有使用
       ps [In]: IShell接口指针
       pHelpers ：没有使用
       ppMod [out]：
       pfnMC:        没有使用
       pfnMF:        没有使用
       
返回值:
       int
       
备注:
       
==============================================================================*/
int ConverterMod_New(int16  nSize, IShell *ps, void *pHelpers, IModule **ppMod, PFNMODCREATEINST pfnMC, PFNFREEMODDATA pfnMF)
{
    if (NULL == ppMod)
    {
        return EFAILED;
    }

    *ppMod = NULL;

#ifdef AEE_SIMULATOR
        g_dwAEEStdLibEntry = (uint32)pHelpers;
        if (!ps || !pHelpers)
        {
            return EFAILED;
        }
#else

    if (!ps)
    {
        return EFAILED;
    }
#endif

    MEMSET(&gConverterMod,0,sizeof(ConverterMod) + sizeof(coefficient));

    INIT_VTBL(&gConverterMod,IModule,gModFuncs);
    gConverterMod.m_nRefs = 1;
    *ppMod = (IModule *)&gConverterMod;
    MSG_FATAL("*ppMod address=%x",*ppMod,0,0);
    return AEE_SUCCESS;
}

/*==============================================================================
函数: 
       ConverterMod_AddRef
       
说明: 
       Converter 模块引用计数加1，并返回当前的引用数。
       
参数: 
       po [in]：IModule接口对象指针。
       
返回值:
       当前对Converter 模块的引用数
       
备注:
       
==============================================================================*/
static uint32 ConverterMod_AddRef(IModule *po)
{
    MSG_FATAL("*po address=%x",po,0,0);
    return(++((ConverterMod *)po)->m_nRefs);
}

/*==============================================================================
函数: 
       ConverterMod_Release
       
说明: 
       Converter 模块引用计数减1，并返回当前的引用数。
       
参数: 
       po [in]：IModule接口对象指针。
       
返回值:
       当前对Converter 模块的引用数
       
备注:
       
==============================================================================*/
static uint32 ConverterMod_Release(IModule *po)
{
    MSG_FATAL("po address=%x",po,0,0);
    if (((ConverterMod *) po)->m_nRefs == 0)
    {
        return 0;
    }

    return(--((ConverterMod *)po)->m_nRefs);
}

/*==============================================================================
函数:
       ConverterMod_CreateInstance

说明:
        通过创建(初始化)Converter对象来创建模块实例。

参数:
       pIShell [in]：IShell接口对象指针。
       po [in]：IModule接口对象指针。
       ClsId [in]：要创建的实例的类ID
       ppObj [out]:：用于保存对象实例的指针的指针变量。

返回值:
       EFAILED;操作失败。
       SUCCESS;操作成功。

备注:
       不需用户去调用。

==============================================================================*/
static int ConverterMod_CreateInstance(IModule *po, IShell *pIShell, AEECLSID ClsId, void ** ppObj)
{
    *ppObj = NULL;

    if (ClsId != AEECLSID_CONVERTER)
    {
        return EFAILED;
    }
    
    if(Converter_New(pIShell, po, (IConverter**)ppObj) != SUCCESS)
    {
        return EFAILED;
    }
    MSG_FATAL("ppObj address=%x",ppObj,0,0);
    return SUCCESS;
}

/*==============================================================================
函数: 
       ConverterMod_FreeResources
       
说明: 
        释放Converter模块资源的函数。
       
参数: 
       po ：IModule。
       ph ：IHeap接口指针。
       pfm :IFileMgr接口指针。
       
返回值:
       无
       
备注:
       不需用户去调用。
       
==============================================================================*/
static void ConverterMod_FreeResources(IModule  *po, IHeap  *ph, IFileMgr *pfm)
{
    PARAM_NOT_REF(po)
    PARAM_NOT_REF(ph)
    PARAM_NOT_REF(pfm)
}

/*==============================================================================
函数:
       Converter_New

说明:
       用来初始化Converter模块虚拟表（也增加引用计数）的助手函数。

参数:
       pIShell [In]: IShell接口指针
       po [in]：IModule接口对象指针。
       ppObj [out]:：用于保存对象实例的指针的指针变量。

返回值:
       int

备注:

==============================================================================*/
static int  Converter_New(IShell* pIShell, IModule* po, IConverter** ppObj)
{
    int retVal = SUCCESS;

    if (0 == gConverter.m_nRefs)
    {
        // Must initialize the object
        MEMSET(&gConverter,  0, sizeof(CConverter) + sizeof(coefficient));
        INIT_VTBL(&gConverter, IConverter, gConverterMethods);

        gConverter.m_nRefs    = 0;
        gConverter.m_pShell    = pIShell;
        gConverter.m_pModule = po;

        (void) ISHELL_AddRef(pIShell);
        (void) IMODULE_AddRef(po);
        MSG_FATAL("pIShell address=%x",pIShell,0,0);
        retVal = Converter_InitAppData(&gConverter);
    }

    ++gConverter.m_nRefs;
    *ppObj = (IConverter*)&gConverter;
    return retVal; 
}

/*==============================================================================
函数:
       Converter_AddRef

说明:
       Converter Applet 引用计数加1，并返回当前的引用数。

参数:
       pi [in]：IConverter接口对象指针。

返回值:
       当前对Converter Applet 的引用数

备注:

==============================================================================*/
static uint32  Converter_AddRef(IConverter *pi)
{
    register CConverter *pMe = (CConverter*)pi;

    return (++pMe->m_nRefs);
}

/*==============================================================================
函数:
       Converter_Release

说明:
       释放对Coverter Applet的引用，引用计数减1，并返回当前的引用数。
        如果当前的引用数为零，还将释放Applet及相关资源。

参数:
       pi [in]：ICoverter接口对象指针。

返回值:
       当前对Coverter Applet的引用数

备注:

==============================================================================*/
static uint32  Converter_Release (IConverter *pi)
{
    register CConverter* pMe = (CConverter*)pi;
    MSG_FATAL("pMe address=%x",pMe,0,0);
    if (pMe->m_nRefs == 0)
    {
        return 0;
    }

    if(--pMe->m_nRefs)
    {
        return pMe->m_nRefs;
    }

    Converter_FreeAppData(pMe);
    (void) ISHELL_Release(pMe->m_pShell);
    (void) IMODULE_Release(pMe->m_pModule);
    
   return 0;
}

/*==============================================================================
函数:
       Converter_HandleEvent

说明:
       Converter  Applet事件处理主函数。所有属于本Applet的事件，
       都将经由该函数处理。

参数:
       pi [in]：指向IConverter接口t对象的指针。
       eCode：BREW事件代码。
       wParam：与事件相关联的数据。
       dwParam [in]：与事件相关联的数据。

返回值:
       TRUE：传入事件被处理。
       FALSE：传入事件没被处理。

备注:

==============================================================================*/
static boolean Converter_HandleEvent(IConverter *pi, AEEEvent eCode, uint16  wParam, uint32 dwParam)
{
    CConverter *pMe = (CConverter*)pi;
    AEEAppStart* as = 0;
    switch (eCode)
    {
        case EVT_APP_START:
        {
            pMe->m_bAppIsReady = FALSE;
            as = (AEEAppStart*) dwParam;
            if (NULL != pMe->m_pDisplay)
            {
                (void) IDISPLAY_Release(pMe->m_pDisplay);
                pMe->m_pDisplay = NULL;
            }
            pMe->m_pDisplay = as->pDisplay;
            MSG_FATAL("m_pDisplay address=%x",pMe->m_pDisplay,0,0);
            (void) IDISPLAY_AddRef(pMe->m_pDisplay);
            pMe->m_rc = as->rc;
            {
                AEEDeviceInfo di; 
                ISHELL_GetDeviceInfo(pMe->m_pShell,&di);
                pMe->m_rc.dy = di.cyScreen;
            }
			if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,AEECLSID_ANNUNCIATOR,(void **)&pMe->m_pIAnn))
		    {

		        return EFAILED;
		    }
            MSG_FATAL("m_pIAnn address=%x",pMe->m_pIAnn,0,0);
			IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE);
            pMe->m_bSuspending = FALSE;
            Converter_RunFSM(pMe);
            return TRUE;
        }

        case EVT_APP_STOP:
        {
            pMe->m_bSuspending = TRUE;
            (void)ISHELL_CancelTimer(pMe->m_pShell, NULL, pMe);
            return TRUE;
        }

        case EVT_APP_SUSPEND:
        {
            pMe->m_bSuspending = TRUE;
            (void)ISHELL_CancelTimer(pMe->m_pShell, NULL, pMe);
            return TRUE;
        }

        case EVT_APP_RESUME:
        {
            as = (AEEAppStart*) dwParam;
            pMe->m_bSuspending = FALSE;
            if (pMe->m_pDisplay != NULL)
            {
                (void) IDISPLAY_Release(pMe->m_pDisplay);
                pMe->m_pDisplay = NULL;
            }
            pMe->m_pDisplay = as->pDisplay;
            MSG_FATAL("m_pDisplay address=%x",pMe->m_pDisplay,0,0);
            (void) IDISPLAY_AddRef(pMe->m_pDisplay);
            pMe->m_rc = as->rc;
            {
                AEEDeviceInfo di; 
                ISHELL_GetDeviceInfo(pMe->m_pShell,&di);
                pMe->m_rc.dy = di.cyScreen;
            }

            Converter_RunFSM(pMe);
            return TRUE;
        }

        case EVT_DIALOG_INIT:
            if( wParam == OEM_IME_DIALOG)
			{
				return TRUE;
			}
            pMe->m_bAppIsReady = FALSE;
            pMe->m_pDialog = (IDialog*)dwParam;
            pMe->m_pActiveDlgID = wParam;

            return Converter_RouteDialogEvent(pMe,eCode,wParam,dwParam);

        case EVT_DIALOG_START:
            if(OEM_IME_DIALOG == wParam)
			{
				return TRUE;
			}
            return Converter_RouteDialogEvent(pMe,eCode,wParam,dwParam);

        case EVT_USER_REDRAW:
            (void)Converter_RouteDialogEvent(pMe,eCode,wParam,dwParam);
            (void)ISHELL_SetTimer ( pMe->m_pShell,
                            APPISREADY_TIMER,
                            Converter_APPIsReadyTimer,
                            pMe);
            return TRUE;
            
        case EVT_APPISREADY:
            (void) Converter_RouteDialogEvent(pMe,eCode,wParam,dwParam);
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
            return Converter_RouteDialogEvent(pMe,eCode,wParam,dwParam);
			
        case EVT_USER:
			{
				MSG_FATAL("wParam=============%d",wParam,0,0);
				if((wParam == AVK_SELECT)||(wParam == AVK_INFO))
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
				else if((wParam == AVK_CLR)||(wParam == AVK_LEFT)||(wParam == AVK_RIGHT))
				{
					eCode = EVT_KEY;
				}
				return Converter_RouteDialogEvent(pMe,eCode,wParam,dwParam);
			}
			break;
			
        case EVT_DIALOG_END:
            if (wParam == 0)
            {
                return TRUE;
            }
            if(OEM_IME_DIALOG == wParam)
			{
				return ISHELL_PostEvent(pMe->m_pShell,AEECLSID_CONVERTER,EVT_USER_REDRAW,0,0);
			}
            pMe->m_bAppIsReady = FALSE;
            (void) Converter_RouteDialogEvent(pMe,eCode,wParam,dwParam);
            pMe->m_pDialog = NULL;

            if (pMe->m_bSuspending == FALSE)
            {
                Converter_RunFSM(pMe);
            }
            return TRUE;

        default:
            return Converter_RouteDialogEvent(pMe,eCode,wParam,dwParam);
    }
   return FALSE;
}

/*==============================================================================
函数:
       Converter_InitAppData

说明:
       初始化Converter Applet结构数据缺省值。

参数:
       pMe [in]：指向Converter Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
       SUCCESS：初始化成功。
       EFAILED：初始化失败。

备注:

==============================================================================*/
static int Converter_InitAppData(CConverter *pMe)
{
    AEEDeviceInfo di;

    if (NULL  == pMe)
    {
        return EFAILED;
    }
    ISHELL_GetDeviceInfo(pMe->m_pShell, &di);
    pMe->m_rc.x             = 0;
    pMe->m_rc.y             = 0;
    pMe->m_rc.dx            = (int16) di.cxScreen;
    pMe->m_rc.dy            = (int16) di.cyScreen;
    pMe->m_bAppIsReady = FALSE;
    pMe->m_ePreState     = CONVERTER_NONE;
    pMe->m_eCurState     = CONVERTER_INIT; 
    pMe->m_pDialog         = NULL;
    pMe->m_eDlgRet        = DLGRET_CREATE;
    pMe->m_bNotOverwriteDlgRet = FALSE;
    pMe->m_nCtlID          = IDC_NUMBER1;
    pMe->m_selectedItem = 0;
    pMe->dyMenu            = MENUITEM_HEIGHT;
    pMe->basiccoefficient = 0;
    pMe->m_converterMode = CONVERTER_MODE_NONE;
    pMe->b_overflow = FALSE;
    #ifdef FEATURE_LCD_TOUCH_ENABLE
    pMe->PENUPbRedraw = FALSE;
    pMe->PENUPbCalc = FALSE;
    #endif
    (void)MEMSET(&pMe->m_CurrencyCFG, 0, sizeof(ConverterCurrencyCfg));
    
    if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                             AEECLSID_CONFIG,
                                             (void **)&pMe->m_pConfig))
    
    {
        Converter_FreeAppData( pMe);
        return EFAILED;
    }
    MSG_FATAL("m_pConfig address=%x",pMe->m_pConfig,0,0);
    if( ISHELL_CreateInstance( pMe->m_pShell,
                             AEECLSID_STATIC,
                             (void **)&pMe->m_pStatic
                         ) != SUCCESS
    )
    {
        Converter_FreeAppData( pMe);
        return EFAILED;
    }    
    MSG_FATAL("m_pStatic address=%x",pMe->m_pStatic,0,0);
    if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                 AEECLSID_LISTCTL,
                                 (void **) &pMe->pUnitMenu1))
    {
        Converter_FreeAppData( pMe);
        return EFAILED;
    }
    MSG_FATAL("pUnitMenu1 address=%x",pMe->pUnitMenu1,0,0);
    if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                 AEECLSID_LISTCTL,
                                 (void **) &pMe->pUnitMenu2))
    {
        Converter_FreeAppData( pMe);
        return EFAILED;
    }
    MSG_FATAL("pUnitMenu2 address=%x",pMe->pUnitMenu2,0,0);
    if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                 AEECLSID_TEXTCTL,
                                 (void **) &pMe->pNumber1))
    {
        Converter_FreeAppData( pMe);
        return EFAILED;
    }
    MSG_FATAL("pNumber1 address=%x",pMe->pNumber1,0,0);
    if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                 AEECLSID_TEXTCTL,
                                 (void **) &pMe->pNumber2))
    {
        Converter_FreeAppData( pMe);
        return EFAILED;
    }
    MSG_FATAL("pNumber2 address=%x",pMe->pNumber2,0,0);
    if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell, 
                                AEECLSID_LISTCTL, 
                                (void **) &pMe->title))
    {
        Converter_FreeAppData( pMe);
        return EFAILED;
    }
    MSG_FATAL("title address=%x",pMe->title,0,0);
    if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell, 
                                AEECLSID_MENUCTL, 
                                (void **) &pMe->currency))
    {
        Converter_FreeAppData( pMe);
        return EFAILED;
    }
    MSG_FATAL("currency address=%x",pMe->currency,0,0);
    if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell, 
                                AEECLSID_TEXTCTL, 
                                (void **) &pMe->coeff1))
    {
        Converter_FreeAppData( pMe);
        return EFAILED;
    }
    MSG_FATAL("coeff1 address=%x",pMe->coeff1,0,0);
    if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell, 
                                AEECLSID_TEXTCTL, 
                                (void **) &pMe->coeff2)) 
    {
        Converter_FreeAppData( pMe);
        return EFAILED;
    }
    MSG_FATAL("coeff2 address=%x",pMe->coeff2,0,0);
    #if defined(FEATURE_VERSION_S1000T)|| defined(FEATURE_VERSION_W515V3)|| defined(FEATURE_VERSION_W516)
    #else
    if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell, 
                                AEECLSID_TEXTCTL, 
                                (void **) &pMe->coeff3))
    {
        Converter_FreeAppData( pMe);
        return EFAILED;
    }
    MSG_FATAL("coeff3 address=%x",pMe->coeff3,0,0);
    #endif
    Converter_SetMenuPosition(pMe);

    return SUCCESS;
}

/*==============================================================================
函数:
       Converter_FreeAppData

说明:
       释放Converter Applet使用的相关资源。

参数:
       pMe [in]：指向Converter Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
       无。

备注:

==============================================================================*/
static void Converter_FreeAppData(CConverter *pMe)
{
    if (NULL == pMe)
    {
        return ;
    }
    MSG_FATAL("pNumber1 address=%x",pMe->pNumber1,0,0);
    if(pMe->pNumber1)
    {
        ITEXTCTL_Release(pMe->pNumber1);
        pMe->pNumber1 = NULL;
    }
    MSG_FATAL("pNumber2 address=%x",pMe->pNumber2,0,0);
    if(pMe->pNumber2)
    {
        ITEXTCTL_Release(pMe->pNumber2);
        pMe->pNumber2 = NULL;
    }
    MSG_FATAL("pUnitMenu1 address=%x",pMe->pUnitMenu1,0,0);
    if(pMe->pUnitMenu1)
    {
        IMENUCTL_Release(pMe->pUnitMenu1);
        pMe->pUnitMenu1 = NULL;
    }
    MSG_FATAL("pUnitMenu2 address=%x",pMe->pUnitMenu2,0,0);
    if(pMe->pUnitMenu2)
    {
        IMENUCTL_Release(pMe->pUnitMenu2);
        pMe->pUnitMenu2 = NULL;
    }
    MSG_FATAL("title address=%x",pMe->title,0,0);
    if(pMe->title)
    {
        IMENUCTL_Release(pMe->title);
        pMe->title = NULL;
    }
    MSG_FATAL("currency address=%x",pMe->currency,0,0);
    if(pMe->currency)
    {
        IMENUCTL_Release(pMe->currency);
        pMe->currency = NULL;
    }
    MSG_FATAL("coeff1 address=%x",pMe->coeff1,0,0);
    if(pMe->coeff1)
    {
        ITEXTCTL_Release(pMe->coeff1);
        pMe->coeff1 = NULL;
    }
    MSG_FATAL("coeff2 address=%x",pMe->coeff2,0,0);
    if(pMe->coeff2)
    {
        ITEXTCTL_Release(pMe->coeff2);
        pMe->coeff2 = NULL;
    }
    #if defined(FEATURE_VERSION_S1000T)|| defined(FEATURE_VERSION_W515V3)|| defined(FEATURE_VERSION_W516)
    #else
    MSG_FATAL("coeff3 address=%x",pMe->coeff3,0,0);
    if(pMe->coeff3)
    {
        ITEXTCTL_Release(pMe->coeff3);
        pMe->coeff3 = NULL;
    }
    #endif
    MSG_FATAL("coefficient address=%x",coefficient,0,0);
    if(coefficient)
    {
       // FREE(coefficient);数组不需要手动释放，否则会引起重复删除
    }
    MSG_FATAL("m_pConfig address=%x",pMe->m_pConfig,0,0);
    if(pMe->m_pConfig)
    {
        ICONFIG_Release(pMe->m_pConfig);
        pMe->m_pConfig = NULL;
    } 
    MSG_FATAL("m_pStatic address=%x",pMe->m_pStatic,0,0);
    if( pMe->m_pStatic != NULL)
    {
        ISTATIC_Release(pMe->m_pStatic);
        pMe->m_pStatic = NULL;        
    }
    MSG_FATAL("m_pIAnn address=%x",pMe->m_pIAnn,0,0);
	if(pMe->m_pIAnn)
    {
        IANNUNCIATOR_Release(pMe->m_pIAnn);
        pMe->m_pIAnn = NULL;
    }
    if (pMe->m_pDisplay != NULL)
    {
        (void) IDISPLAY_Release(pMe->m_pDisplay);
        pMe->m_pDisplay = NULL;
    }    
}

/*==============================================================================
函数:
       Converter_RunFSM

说明:
       Converte Applet有限状态机引擎。

参数:
       pMe [in]：指向Converte Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
       无。

备注:

==============================================================================*/
static void Converter_RunFSM(CConverter *pMe)
{
    NextFSMAction nextFSMAction = NFSMACTION_WAIT;

    for ( ; ; )
    {
        nextFSMAction = Converter_ProcessState(pMe);

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
       Converter_APPIsReadyTimer

说明:
       定时器回调函数。主要用于控制快速按键问题的处理。

参数:
       pme [in]：void *类型指针，暗指Converter结构指针。

返回值:
       无。

备注:

==============================================================================*/
static void Converter_APPIsReadyTimer(void *pme)
{
    CConverter *pMe = (CConverter *)pme;

    if (NULL == pMe)
    {
        return;
    }

    (void) ISHELL_PostEvent( pMe->m_pShell,
                            AEECLSID_CONVERTER,
                            EVT_APPISREADY,
                            0,
                            0);
}

/*==============================================================================
函数：
       Converter_SetMenuPosition
说明：
       初始化单位换算各菜单位置。
       
参数：
       pMe [in]：指向Converter Applet对象结构的指针。
       
返回值：
       NONE
       
备注：
       
==============================================================================*/
static void Converter_SetMenuPosition(CConverter *pMe)
{
    AEERect rect = {0};
    
    SETAEERECT(&rect, 
                        pMe->m_rc.x + ARROW_WIDTH + FRAME_SIZE, 
                        //TITLEBAR_HEIGHT + FRAME_SIZE, 
                        pMe->m_rc.y + TITLEBAR_HEIGHT/2 + FRAME_SIZE, 	//Add By zzg 2010_07_08
                        pMe->m_rc.dx - (2*(ARROW_WIDTH + FRAME_SIZE)), 
                        pMe->dyMenu - 2*FRAME_SIZE);
    IMENUCTL_SetRect(pMe->pUnitMenu1, &rect);

    rect.x -= ARROW_WIDTH;
    rect.dx += 2*ARROW_WIDTH;
    rect.y += pMe->dyMenu;		
	rect.dy += 5*FRAME_SIZE;					//Add By zzg 2010_07_07
    ITEXTCTL_SetRect(pMe->pNumber1, &rect);
	
    rect.x += ARROW_WIDTH;
    rect.dx -= 2*ARROW_WIDTH;
    //rect.y += pMe->dyMenu;		
    rect.y += (rect.dy + 5*FRAME_SIZE);			//Add By zzg 2010_07_07
	rect.dy -= 5*FRAME_SIZE; 					//Add By zzg 2010_07_07
    IMENUCTL_SetRect(pMe->pUnitMenu2, &rect);
    
    rect.x -= ARROW_WIDTH;
    rect.dx += 2*ARROW_WIDTH;
    rect.y += pMe->dyMenu;		
	rect.dy += 5*FRAME_SIZE;					//Add By zzg 2010_07_07
    ITEXTCTL_SetRect(pMe->pNumber2, &rect);
	rect.dy -= 5*FRAME_SIZE;					//Add By zzg 2010_07_07
	    
    rect.x += ARROW_WIDTH;
    rect.y = pMe->m_rc.y + TITLEBAR_HEIGHT/2 + FRAME_SIZE;
    rect.dx = pMe->m_rc.dx - (2*ARROW_WIDTH) - 2*FRAME_SIZE;		
    IMENUCTL_SetRect(pMe->title, &rect);
        
    rect.x   = pMe->m_rc.x;
    //rect.y += pMe->dyMenu;    
    rect.y += pMe->dyMenu - FRAME_SIZE;   		//Add By zzg 2010_07_08
    rect.dx = pMe->m_rc.dx;
    rect.dy = pMe->m_rc.dy - TITLEBAR_HEIGHT/2 - BOTTOMBAR_HEIGHT - pMe->dyMenu - STATUSBAR_HEIGHT;
    IMENUCTL_SetRect(pMe->currency, &rect);

    rect.x += FRAME_SIZE;
    rect.y = pMe->m_rc.y + TITLEBAR_HEIGHT/2 + 2*pMe->dyMenu + FRAME_SIZE;
    rect.dx = pMe->m_rc.dx - SCROLLBAR_WIDTH - 2*FRAME_SIZE;
    rect.dy = pMe->dyMenu - 2*FRAME_SIZE;	
    rect.y -= 3*FRAME_SIZE;						//Add By zzg 2010_07_08
    ITEXTCTL_SetRect(pMe->coeff1, &rect);
	rect.y += 3*FRAME_SIZE;						//Add By zzg 2010_07_08
	
    rect.y += (2*pMe->dyMenu);    
    ITEXTCTL_SetRect(pMe->coeff2, &rect);
    
	#if defined(FEATURE_VERSION_S1000T)|| defined(FEATURE_VERSION_W515V3)|| defined(FEATURE_VERSION_W516)
    #else
    rect.y += (2*pMe->dyMenu);	
    rect.y += (2*pMe->dyMenu - FRAME_SIZE);		//Add By zzg 2010_07_08
    ITEXTCTL_SetRect(pMe->coeff3, &rect);
    #endif
	

	ITEXTCTL_SetProperties(pMe->pNumber1, TP_FRAME|TP_FOCUS_NOSEL|TP_FIXSETRECT);
    ITEXTCTL_SetProperties(pMe->pNumber2, TP_FRAME|TP_FOCUS_NOSEL|TP_FIXSETRECT);	
    ITEXTCTL_SetProperties(pMe->coeff1, TP_FRAME|TP_FOCUS_NOSEL);
    ITEXTCTL_SetProperties(pMe->coeff2, TP_FRAME|TP_FOCUS_NOSEL);
    #if defined(FEATURE_VERSION_S1000T)|| defined(FEATURE_VERSION_W515V3)|| defined(FEATURE_VERSION_W516)
    #else
    ITEXTCTL_SetProperties(pMe->coeff3, TP_FRAME|TP_FOCUS_NOSEL);
    #endif
    ITEXTCTL_SetCursorPos(pMe->pNumber1, TC_CURSOREND);
    ITEXTCTL_SetCursorPos(pMe->pNumber2, TC_CURSOREND);
    ITEXTCTL_SetCursorPos(pMe->coeff1, TC_CURSOREND);
    ITEXTCTL_SetCursorPos(pMe->coeff2, TC_CURSOREND);
    #if defined(FEATURE_VERSION_S1000T)|| defined(FEATURE_VERSION_W515V3)|| defined(FEATURE_VERSION_W516)
    #else
    ITEXTCTL_SetCursorPos(pMe->coeff3, TC_CURSOREND);
    #endif
    IMENUCTL_SetProperties(pMe->pUnitMenu1, MP_NO_REDRAW);
    IMENUCTL_SetProperties(pMe->pUnitMenu2, MP_NO_REDRAW);
    IMENUCTL_SetProperties(pMe->currency, MP_NO_REDRAW);
    IMENUCTL_SetProperties(pMe->title, MP_NO_REDRAW);
    IMENUCTL_SetOemProperties(pMe->title, OEMMP_IDF_ALIGN_CENTER);
    IMENUCTL_SetOemProperties(pMe->pUnitMenu1, OEMMP_IDF_ALIGN_CENTER);
    IMENUCTL_SetOemProperties(pMe->pUnitMenu2, OEMMP_IDF_ALIGN_CENTER);
    IMENUCTL_SetOemProperties(pMe->currency, OEMMP_USE_MENU_STYLE);
}


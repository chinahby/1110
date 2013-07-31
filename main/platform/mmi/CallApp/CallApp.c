/*==============================================================================
// 文件：
//        CallApp.c
//        2007-11-01 Gemsea草拟版本(Draft Version)
//        版权所有(c) 2007-2009 Anylook Tech. CO.,LTD.
//
// 文件说明：
//
// 作者：Gemsea
// 创建日期：2007-11-01
// 当前版本：Draft
==============================================================================*/

/*==============================================================================
修改历史记录：
时       间      修 改 人    问题单号  修改内容、位置及原因
-----------      ----------  --------  -----------------------------------------------
==============================================================================*/


/*==============================================================================
                                 本文件包含的外部文件
==============================================================================*/
#include "CallApp_priv.h"
#ifndef WIN32
#include "Snd.h"
#include "Nv_items.h"
#endif
#include "OEMHeap.h"

#ifdef FEATURE_KEYGUARD
#include "OEMKeyguard.h"
#endif
#include "AEE_OEMEvent.h"
#ifdef FEATURE_SUPPORT_BT_APP
#include "Bcmapp_ag.h"
#endif
#ifdef FEATURE_APP_MUSICPLAYER
#include "MusicPlayer.h"
#endif 
#include "AEETelephoneTypes.h"

/*==============================================================================
                                 宏定义和常数
==============================================================================*/

/*==============================================================================
                                 类型定义
==============================================================================*/

/*==============================================================================
                                 函数声明
==============================================================================*/

/*----------------------模块相关函数声明---------------------*/

/*static*/ int  CallAppMod_Load(IShell   *pIShell,
                                        void     *ph,
                                        IModule  **ppMod);

int  CallAppMod_New(int16              nSize,
                                        IShell             *pIShell,
                                        void               *ph,
                                        IModule            **ppMod,
                                        PFNMODCREATEINST   pfnMC,
                                        PFNFREEMODDATA     pfnMF);

static int  CallAppMod_CreateInstance(IModule   *po,
                                        IShell    *pIShell,
                                        AEECLSID  ClsId,
                                        void      **ppObj);

static uint32 CallAppMod_AddRef(IModule *po);

static uint32 CallAppMod_Release(IModule *po);

static void CallAppMod_FreeResources(IModule   *po,
                                        IHeap     *ph,
                                        IFileMgr  *pfm);

/*----------------------Applet相关函数声明---------------------*/
static int CallApp_New(IShell        *ps,
                                        IModule       *pIModule,
                                        ICallApp    **ppObj);

static uint32  CallApp_AddRef(ICallApp *pi);

static uint32  CallApp_Release (ICallApp *pi);

static boolean CallApp_HandleEvent(ICallApp *pi,
                                        AEEEvent    eCode,
                                        uint16      wParam,
                                        uint32      dwParam);

static int CallApp_CallNumber(ICallApp *p, AECHAR *number);

#ifdef FEATURE_EDITABLE_RECORD
static int CallApp_EditRecNumber(ICallApp *p, AECHAR *number, uint16 call_type);
#endif
static int CallApp_InitAppData(CCallApp *pMe);

static void CallApp_FreeAppData(CCallApp *pMe);

static void CallApp_RunFSM(CCallApp *pMe);

static boolean CallApp_Notify(void *pUser,
                                        AEEEvent eCode,
                                        uint16 wParam,
                                        uint32 dwParam);

#ifndef FEATURE_ICM
#define AEECMNotifyInfo             AEETNotifyInfo
#define AEECMCallEventData          AEETCallEventData
#define AEECM_MAX_DIGITS_LENGTH     AEET_MAX_DIGITS_LENGTH
#define AEECM_MAX_ALPHA_TAG_LENGTH  AEET_MAX_ALPHA_TAG_LENGTH
#endif

//static void  CallApp_ProcessBattNotify(CCallApp  *pMe,AEENotify   *msg);
//static int CallApp_SetStartCallType(ICallApp       *p,
//                                        start_call_type type,
//                                        AECHAR          *num,
//                                        callPIType      pi,
//                                        AECHAR          *name);
//static void CallApp_Handle_SS_StateChange(CCallApp*pMe, AEECMNotifyInfo  *p_Ss_Info);

//static void CallApp_Handle_Phone_StateChange(CCallApp *pMe,AEECMNotifyInfo  *p_Phone_Info);

static void CallApp_Handle_Call_StateChange(CCallApp *pMe,AEECMNotifyInfo  *p_Call_Info);

static void CallApp_ProcessCallStateDATA(CCallApp *pMe,
                                        AEECMNotifyInfo  *pCallInfo,
                                        CallAppState *newState);

static void CallApp_ProcessCallStateOTAPA(CCallApp *pMe,
                                        AEECMNotifyInfo  *pCallInfo,
                                        CallAppState *newState);

static void CallApp_ProcessCallStateTest(CCallApp *pMe,
                                        AEECMNotifyInfo  *pCallInfo,
                                        CallAppState *newState);

static void CallApp_ProcessCallStateVoice(CCallApp *pMe,
                                        AEECMNotifyInfo  *pCallInfo,
                                        CallAppState *newState);

static void CallApp_ProcessCallStateVoiceAnswer(CCallApp *pMe,
                                        AEECMCallEventData    *call_table,
                                        CallAppState *newState);

static void CallApp_ProcessCallStateVoiceCallerID(CCallApp *pMe,
                                        AEECMCallEventData    *call_table,
                                        CallAppState *newState);

static void CallApp_ProcessCallStateVoiceConnect(CCallApp *pMe,
                                        AEECMCallEventData    *call_table,
                                        CallAppState *newState);

static void CallApp_ProcessCallStateVoiceAccept(CCallApp *pMe,
                                        AEECMCallEventData    *call_table,
                                        CallAppState *newState);

static void CallApp_ProcessCallStateVoiceDisplay(CCallApp *pMe,
                                        AEECMNotifyInfo  *pCallInfo,
                                        CallAppState *newState);

static void CallApp_ProcessCallStateVoiceEnd(CCallApp *pMe,
                                        AEECMCallEventData    *call_table,
                                        CallAppState *newState);

static void CallApp_ProcessCallStateVoice_Incoming(CCallApp *pMe,
                                        AEECMCallEventData    *call_table,
                                        CallAppState *newState);

static void CallApp_ProcessCallStateVoiceOrig(CCallApp *pMe,
                                        AEECMCallEventData    *call_table,
                                        CallAppState *newState);

static void CallApp_ProcessCallStateVoiceRedirNum(CCallApp *pMe,
                                        AEECMCallEventData    *call_table,
                                        CallAppState *newState);

static void CallApp_ProcessCallStateVoiceSignal(CCallApp *pMe,
                                        AEECMCallEventData    *call_table,
                                        CallAppState *newState);

static boolean CallApp_PlaySignal(CCallApp *pMe,
                                        AECHAR* pszPhoneNumber,
#ifdef FEATURE_ICM
                                        AEECMSignal *pSignal);
#else
                                        AEETSignal *pSignal);
#endif

static void CallApp_Add_OneCall_To_History(CCallApp       *pMe,
                                        AECHAR           *number,
                                        callPIType        pi,
                                        AECHAR           *name,
                                        uint16 call_type,
                                        uint32 call_length);

static int CallApp_Make_IP_Call_F(ICallApp *p, AECHAR *number);

static int CallApp_VoiceCallDial_F(ICallApp *p, AECHAR *number);

static void CallApp_CB_MakeVoiceCallAfterCloseAll(void *pUser);

#ifdef FEATURE_UTK2
static int CallApp_Make_UTK_Call_F(ICallApp *p);
#endif //FEATURE_UTK2

#ifdef FEATURE_LED_CONTROL
#if 0
static void CallApp_DisableLedSig( CCallApp *pMe);
#endif
#endif

static void CallApp_Settime_To_EndCall(CCallApp *pMe);
static void CallApp_Conver_Buf(AECHAR *buf);
static int CallApp_BtCallNumber(ICallApp *p, AECHAR *number);
//修改Config项中的通话时长
static void CallApp_AddToCFGCallTimer(CCallApp *pMe, uint16 call_type, uint32 durationS);
static int CallApp_Start(CCallApp *pMe);
static void CallApp_Stop(CCallApp *pMe);

//static void CallApp_Process_Batty_Msg(CCallApp  *pMe, uint16  msg_id);

/*==============================================================================
                                 全局数据
==============================================================================*/
// 重要提示：
//        g_dwAEEStdLibEntry全局变量仅用于SDK ！
//        BREW SDK用户不能改写和直接使用本变量。它用作AEEStdLib的一个入口点，不
//        要移动下列三行代码。
#ifdef AEE_SIMULATOR
   uint32 g_dwAEEStdLibEntry;
   #ifndef WIN32
   #error This applet was never intended to be used with the AEE Simulator.
	#endif
#endif
#ifdef FEATURE_UTK2
// 在通话中进入 UTK ，进行呼叫时无法传入参数，故使用全局变量来控制
// 进行UTK呼叫时设置此值，处理后要及时清除掉(直接赋值 NULL, 不要释放，因其指向全局静态变量)
call_start_info_type *pUTKCALLinfo=NULL;
#endif //FEATURE_UTK2
/*==============================================================================
                                 本地（静态）数据
==============================================================================*/
static CallAppMod gCallAppMod;

static const VTBL(IModule) gModFuncs =
{
    CallAppMod_AddRef,
    CallAppMod_Release,
    CallAppMod_CreateInstance,
    CallAppMod_FreeResources
};

// 只允许一个CallApp实例。每次创建CallApp Applet时，
// 返回同一结构指针给BREW层。
static CCallApp gCallApp;

static const VTBL(ICallApp) gCallAppMethods =
{
    CallApp_AddRef,
    CallApp_Release,
    CallApp_HandleEvent,
    CallApp_CallNumber,    
    CallApp_BtCallNumber,
    //CallApp_IsRestictCallNumber,
    //CallApp_SetStartCallType,
    CallApp_Make_IP_Call_F,
    //CallApp_Make_Call_F,
    CallApp_VoiceCallDial_F
#ifdef FEATURE_UTK2    
    ,CallApp_Make_UTK_Call_F 
#endif //FEATURE_UTK2    
#ifdef FEATURE_EDITABLE_RECORD
    ,CallApp_EditRecNumber
#endif
};

#ifdef FEATURE_OEMOMH
#include "Gsdi.h"
#define CDG_TEMP_TEST
#endif
static call_start_info_type call_start_info_call;
/*----------------------模块相关函数定义---------------------*/

/*==============================================================================
函数:
       CallAppMod_Load

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
/*static*/ int CallAppMod_Load(IShell   *pIShell,
                                                   void     *ph,
                                                   IModule **ppMod)
{
    // 对于动态Applet，验证AEE STD库版本，以确保库与建立本模块时使用的库版本匹配
    // 。在AEE_SIMULATOR下不必进行本检查。
#if !defined(BREW_STATIC_APP) && !defined(AEE_SIMULATOR)
    if (GET_HELPER_VER() != AEESTDLIB_VER)
    {
        return EVERSIONNOTSUPPORT;
    }
#endif

    return CallAppMod_New(sizeof(CallAppMod), pIShell, ph,ppMod, NULL,NULL);
}

/*==============================================================================
函数:
       CallAppMod_New

说明:
       用来初始化CallApp模块虚拟表（也增加引用计数）的助手函数。

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
int  CallAppMod_New(int16             nSize,
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

    MEMSET(&gCallAppMod,0,sizeof(CallAppMod));

    INIT_VTBL(&gCallAppMod,IModule,gModFuncs);
    gCallAppMod.m_nRefs = 1;
    *ppMod = (IModule *)&gCallAppMod;
    return AEE_SUCCESS;
}

/*==============================================================================
函数:
       CallAppMod_AddRef

说明:
       CallApp 模块引用计数加1，并返回当前的引用数。

参数:
       po [in]：IModule接口对象指针。

返回值:
       当前对CallApp 模块的引用数

备注:

==============================================================================*/
static uint32 CallAppMod_AddRef(IModule *po)
{
    return(++((CallAppMod *)po)->m_nRefs);
}

/*==============================================================================
函数:
       CallAppMod_Release

说明:
       CallApp 模块引用计数减1，并返回当前的引用数。

参数:
       po [in]：IModule接口对象指针。

返回值:
       当前对CallApp 模块的引用数

备注:

==============================================================================*/
static uint32 CallAppMod_Release(IModule *po)
{
    if (((CallAppMod *) po)->m_nRefs == 0)
    {
        return 0;
    }

    return(--((CallAppMod *)po)->m_nRefs);
}

/*==============================================================================
函数:
       CallAppMod_CreateInstance

说明:
        通过创建(初始化)CallApp对象来创建模块实例。

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
static int  CallAppMod_CreateInstance(IModule   *po,
                                        IShell    *pIShell,
                                        AEECLSID  ClsId,
                                        void      **ppObj)
{
    *ppObj = NULL;

    if (ClsId != AEECLSID_DIALER/*AEECLSID_CALL*/)
    {
        return EFAILED;
    }

    if (CallApp_New(pIShell, po, (ICallApp**)ppObj) != SUCCESS)
    {
        return EFAILED;
    }

    return SUCCESS;
}

/*==============================================================================
函数:
       CallAppMod_FreeResources

说明:
        释放CallApp模块资源的函数。

参数:
       po ：IModule。
       ph ：IHeap接口指针。
       pfm :IFileMgr接口指针。

返回值:
       无

备注:
       不需用户去调用。

==============================================================================*/
static void CallAppMod_FreeResources(IModule  *po,
                                       IHeap  *ph,
                                       IFileMgr *pfm)
{
    PARAM_NOT_REF (pfm)
    PARAM_NOT_REF (po)
    PARAM_NOT_REF (ph)
}

/*----------------------Applet相关函数定义---------------------*/
/*==============================================================================
函数:
       CallApp_New

说明:
        初始化CallApp Applet虚拟表和内部数据变量，同时其引用计数加1。

参数:
       ps [in]：IShell接口对象指针。
       pIModule [in]：pIModule接口对象指针。
       ppObj [out]:：指向ICallApp对象指针的指针。

返回值:
       EFAILED：失败。
       SUCCESS：成功。

备注:
       不需用户去调用。
       关于Applet结构对象可使用动态分配内存的方式创建，本处采用的是使用静态
       变量的方式。建议对结构比较大的Applet使用静态变量的方式。对于动态分配
       内存的方式需要注意对所分配空间的释放。

==============================================================================*/
static int CallApp_New(IShell     *ps,
                         IModule    *pIModule,
                         ICallApp **ppObj)
{
    int retVal = SUCCESS;

    if (0 == gCallApp.m_nRefs)
    {
        // Must initialize the object
        MEMSET(&gCallApp,  0, sizeof(CCallApp));
        INIT_VTBL(&gCallApp, ICallApp, gCallAppMethods);

        gCallApp.m_nRefs     = 1;
        gCallApp.m_pShell    = ps;
        gCallApp.m_pModule   = pIModule;

        (void) ISHELL_AddRef(ps);
        (void) IMODULE_AddRef(pIModule);
        retVal = CallApp_InitAppData(&gCallApp);
    }

    ++gCallApp.m_nRefs;
    *ppObj = (ICallApp*)&gCallApp;
    return retVal;
}

/*==============================================================================
函数:
       CallApp_AddRef

说明:
       CallApp Applet 引用计数加1，并返回当前的引用数。

参数:
       pi [in]：ICallApp接口对象指针。

返回值:
       当前对CallApp Applet的引用数

备注:

==============================================================================*/
static uint32  CallApp_AddRef(ICallApp *pi)
{
    register CCallApp *pMe = (CCallApp*)pi;

    ASSERT(pMe != NULL);
    ASSERT(pMe->m_nRefs > 0);

    return (++pMe->m_nRefs);
}

/*==============================================================================
函数:
       CallApp_Release

说明:
       释放对CallApp Applet的引用，引用计数减1，并返回当前的引用数。
        如果当前的引用数为零，还将释放Applet及相关资源。

参数:
       pi [in]：ICallApp接口对象指针。

返回值:
       当前对CallApp Applet的引用数

备注:

==============================================================================*/
static uint32  CallApp_Release (ICallApp *pi)
{
    register CCallApp *pMe = (CCallApp*)pi;
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
    CallApp_FreeAppData(pMe);

    (void) ISHELL_Release(pMe->m_pShell);
    (void) IMODULE_Release(pMe->m_pModule);

    // 注意：pMe是静态分配空间，因此不需要释放。FREE()
    return 0;
}

/*==============================================================================
函数:
       CallApp_InitAppData

说明:
       初始化CallApp Applet结构数据缺省值。

参数:
       pMe [in]：指向CallApp Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
       SUCCESS：初始化成功。
       EFAILED：初始化失败。

备注:

==============================================================================*/
static int CallApp_InitAppData(CCallApp *pMe)
{
    AEEDeviceInfo  di;
    //uint32 dwMask = 0;
    CALL_FUN_START("CallApp_InitAppData",0,0,0);

	 #if 0//def FEATURE_VERSION_K212
    MSG_FATAL("disp_drv_on......................",0,0,0);
	gpio_out(GPIO_OUTPUT_10,(GPIO_ValueType)GPIO_LOW_VALUE);
    clk_busy_wait(30*1000);
    gpio_tlmm_config(GPIO_OUTPUT_10);
    gpio_out(GPIO_OUTPUT_10,(GPIO_ValueType)GPIO_HIGH_VALUE);
    clk_busy_wait(1);
    gpio_out(GPIO_OUTPUT_10,(GPIO_ValueType)GPIO_LOW_VALUE);
    clk_busy_wait(5);
    gpio_out(GPIO_OUTPUT_10,(GPIO_ValueType)GPIO_HIGH_VALUE);
    clk_busy_wait(1);
    gpio_out(GPIO_OUTPUT_10,(GPIO_ValueType)GPIO_LOW_VALUE);
    clk_busy_wait(1);
    gpio_out(GPIO_OUTPUT_10,(GPIO_ValueType)GPIO_HIGH_VALUE);
    clk_busy_wait(1);
    gpio_out(GPIO_OUTPUT_10,(GPIO_ValueType)GPIO_LOW_VALUE);
    clk_busy_wait(1);
    gpio_out(GPIO_OUTPUT_10,(GPIO_ValueType)GPIO_HIGH_VALUE);
	#endif

    if (NULL  == pMe)
    {
        return EFAILED;
    }
#if 0
    if (ISHELL_CreateInstance(pMe->m_pShell,
                                            AEECLSID_STATIC,
                                            (void **)&(pMe->m_IStatic)) != SUCCESS)
    {
        return EFAILED;
    }
#endif
    // Create IConfig interface
    if (ISHELL_CreateInstance(pMe->m_pShell,
                                            AEECLSID_CONFIG,
                                            (void **)&pMe->m_pConfig)!= AEE_SUCCESS)
    {
        return EFAILED;
    }
#ifdef FEATURE_OEMOMH
    if(ISHELL_CreateInstance(pMe->m_pShell,
                                            AEECLSID_RUIM,
                                            (void **) &pMe->m_pIRUIM) != SUCCESS)
    {
        return EFAILED;
    }
#endif
//Add by pyuangui 2013-01-10
#ifdef FEATURE_VERSION_C316
   if(AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell, 
										   AEECLSID_FILEMGR, 
										   (void **)&pMe->m_pFileMgr))
   {
	   return EFAILED;
   }
#endif
//Add end
#ifdef FEATURE_ICM
    if(ISHELL_CreateInstance(pMe->m_pShell,
                                            AEECLSID_CM,
                                            (void **) &pMe->m_pICM) != SUCCESS)
    {
        return EFAILED;
    }
#else
    if(ISHELL_CreateInstance(pMe->m_pShell,
                                            AEECLSID_TELEPHONE,
                                            (void **) &pMe->m_pITelephone) != SUCCESS)
    {
        return EFAILED;
    }

	if(ISHELL_CreateInstance(pMe->m_pShell,
                                 AEECLSID_CALLMGR,
                                 (void **) &pMe->m_pICallMgr)!=SUCCESS)
    {
        return EFAILED;
    }
	
    if( ISHELL_CreateInstance(pMe->m_pShell,
                                 AEECLSID_PHONECTL,
                                 (void **) &pMe->m_pIPhoneCtl)!=SUCCESS)
    {
        return EFAILED;
    }
#endif
    // Create Sound instance
    if (ISHELL_CreateInstance(pMe->m_pShell,
                                            AEECLSID_SOUND,
                                            (void **)&pMe->m_pSound)!=AEE_SUCCESS)
    {
        return EFAILED;
    }

#ifdef KEYSND_ZY
    if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                            AEECLSID_SOUNDPLAYER,
                                            (void **)(&pMe->m_SndPlayer)))
    {
        return EFAILED;
    }
#endif /* KEYSND_ZY */
#ifdef FEATURE_SUB_LCD
    if (SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                            AEECLSID_DISP2,
                                            (void**)&pMe->m_pDisplaySub))
    {
        return EFAILED;
    }
#endif


    // Create IAlert instance
    if (ISHELL_CreateInstance(pMe->m_pShell,
                                            AEECLSID_ALERT,
                                            (void **)&pMe->m_pAlert)!=AEE_SUCCESS)
    {
        return EFAILED;
    }
    
    // Create the instance of backlight
    if (ISHELL_CreateInstance(pMe->m_pShell,
                                            AEECLSID_BACKLIGHT,
                                            (void **)&pMe->m_pBacklight)!=AEE_SUCCESS)
    {
        return EFAILED;
    }

    if (ISHELL_CreateInstance(pMe->m_pShell,
                                 AEECLSID_ANNUNCIATOR,
                                 (void **) &pMe->m_pIAnn))
    {
        return EFAILED;
    }
    if ( SUCCESS != ISHELL_CreateInstance ( pMe->m_pShell,
                                            AEECLSID_MENUCTL,
                                            (void **)&pMe->m_pMenu) )
    {
        return EFAILED;
    }
#ifdef FEATURE_ICM
    // Register for phone, call, and serving system notifications
    if (AEE_SUCCESS != ISHELL_RegisterNotify(pMe->m_pShell,
                                             AEECLSID_DIALER,
                                             AEECLSID_CM_NOTIFIER,
                                             NMASK_CM_VOICE_CALL
                                             | NMASK_CM_OTHER_CALL
                                             | NMASK_CM_TEST_CALL
                                             | NMASK_CM_DATA_CALL
                                             ))
    {
        return EFAILED;
    }
#else
    if (AEE_SUCCESS != ISHELL_RegisterNotify(pMe->m_pShell,
                                             AEECLSID_DIALER,
                                             AEECLSID_PHONENOTIFIER,
                                             AEET_NMASK_VOICE_CALL
                                             | AEET_NMASK_OTHER_CALL
                                             | AEET_NMASK_TEST_CALL
                                             | AEET_NMASK_DATA_CALL
                                             ))
    {
        return EFAILED;
    }
#endif
    if(ISHELL_RegisterNotify(pMe->m_pShell, AEECLSID_DIALER,
        AEECLSID_ALERT_NOTIFIER, NMASK_ALERT_ONOFF | NMASK_ALERT_MUTED) != SUCCESS)
    {
        return EFAILED;
    }

#if 0// defined( FEATURE_CALL_RECORDER)
	if( !recorder_init_media( &pMe->m_Media))
	{
		return EFAILED;
	}
	{
		char*	resFile[] = { AEE_APPSCOMMONRES_IMAGESFILE,
							APP_RECORDER_IMAGES_RES_FILE,
							APP_RECORDER_IMAGES_RES_FILE
						};
		uint16 	resId[]   = { IDI_DIALOG,
							IDI_RECORDER_CURSOR,
							IDI_RECORDER_CURSOR1
						};
		int 	i 		= 0;
		int		number	= 3;

		for( i = 0; i < number; i ++)
		{
			if( !( pMe->m_pImage[i] = ISHELL_LoadResImage( pMe->m_pShell, resFile[i], resId[i])))
			{
				return EFAILED;
			}
		}
	}
#endif


    //get the device information
    ISHELL_GetDeviceInfo(pMe->m_pShell, &di);
    pMe->m_rc.x       = 0;
    pMe->m_rc.y       = 0;
    pMe->m_rc.dx      = (int16) di.cxScreen;
    pMe->m_rc.dy      = (int16) di.cyScreen;
	pMe->m_penup      = FALSE;
    pMe->m_ePreState = STATE_NULL;
    pMe->m_eCurState = STATE_INIT;
    pMe->m_pActiveDlg = NULL;
    pMe->m_eDlgRet = DLGRET_CREATE;
    pMe->m_bNotOverwriteDlgRet = FALSE;
    //pMe->idle_info.timeState = TIMESTATE_INVALID;//Not use
    pMe->idle_info.inLPM = FALSE;  // CALLCHECK

    pMe->m_userCanceled = FALSE;
    pMe->m_nToneCount = 0;
    pMe->m_bShowPopMenu = FALSE;
    pMe->m_toneData = NULL;
	//pMe->m_short_hit = FALSE;
    pMe->Ispwpass=FALSE;
#ifdef FEATURE_EDITABLE_NUMBER
    pMe->m_nCursorPos = 0;
#endif
#ifdef FEATURE_EDITABLE_RECORD
    pMe->m_bEditRecNumber = FALSE;
#endif
    CallApp_Free_All_Call_Table(pMe);//ok
    //pMe->m_RedirNumStr[0] = 0;
    //pMe->m_DispInfo[0] = 0;
    //pMe->m_ExtendedDispInfo[0] = 0;
    //pMe->m_ExtendedDispInfo2[0] = 0;
    //pMe->m_CalledPartyNum[0] = 0;
    //CallApp_Reset_CDG_Info(pMe);
    MEMSET((void*)&pMe->m_cdg_dsp_info , 0 ,sizeof(cdg_dsp_info));
    pMe->m_nStartCallType = START_MAX;

    //pMe->m_msg_text = NULL;
    //pMe->m_IStatic = NULL;
#ifdef FEATURE_ICM
    pMe->m_lastCallState = AEECM_CALL_STATE_IDLE;
#else
    pMe->m_lastCallState = AEET_CALL_STATE_IDLE;
#endif
    pMe->IsStartCall = TRUE;
    pMe->m_bCloseAllApplet = FALSE;
    pMe->IsRestrictNumber = FALSE;
    pMe->m_bmissCallWaiting = FALSE;

    pMe->m_pBigNumFont = NULL;
    pMe->m_pNormalNumFont = NULL;
#ifdef FEATURE_UTK2
    pMe->m_bEndcurcallForUTKCall = FALSE;
#endif
    pMe->m_CallsTable = NULL;
    pMe->m_CallsTable_Count =0;
    pMe->m_bReceiveCallerID     = FALSE;
#ifndef FEATURE_USES_LOWMEM
    pMe->m_pConvImage = NULL;
#endif
    pMe->m_btime_out = 0;
    pMe->m_return_value = RETURN_ZERO;
    pMe->m_bincoming_rsk = IDS_MUTE;
    //pMe->m_b_show_cdg = FALSE;
    pMe->m_cdg_row = 0;
    pMe->m_anykey_answer = OEM_AUTO_ANSWER_MODE;
    pMe->m_b_incoming = FALSE;
#if defined(FEATURE_VERSION_C337)||defined(FEATURE_VERSION_K202)   	
    pMe->m_isIncoming 	= FALSE;
#endif
    pMe->m_b_auto_redial = FALSE;
    pMe->m_auto_redial_count = 0;
    pMe->m_b_incall = FALSE;
    pMe->m_msg_text_id = 0;
	pMe->m_prompt_id = 0;			//Add By zzg 2012_11_01
    pMe->m_b_from_numedit = TRUE;
    pMe->m_b_draw_dot = TRUE;
    pMe->m_curpros = 0;
    pMe->b_multenter = FALSE;
#ifdef FEATRUE_AUTO_POWER
    pMe->m_b_powerdown = FALSE;
#endif

    pMe->m_b_is_PI_ALLOWED = PI_ALLOWED;
    pMe->m_b_miss_notify = TRUE;
#ifdef FEATURE_KEYGUARD
    pMe->m_b_set_keyguard = FALSE;
#endif
    //pMe->m_b_from_another = FALSE;
#ifdef FEATRUE_SET_IP_NUMBER
{//when make ip call from recent call option, this is useful
    IP_Number_Cfg  ip_cfg;
    MEMSET((void*)&ip_cfg,0 , sizeof(IP_Number_Cfg));
    pMe->m_b_have_set_ip = FALSE;
    ICONFIG_GetItem(pMe->m_pConfig,CFGI_IP_NUMBER,(void *)&ip_cfg,sizeof(IP_Number_Cfg));
    if(ip_cfg.ip_count>0)
    {
        pMe->m_b_have_set_ip = TRUE;
    }
}
#endif /*FEATRUE_SET_IP_NUMBER*/

#ifdef FEATURE_SUPPORT_BT_APP
    pMe->m_bt_audio = SEND_NONE;
    pMe->m_b_add_btag_menu = FALSE;
#endif

	//Add By zzg 2011_10_27
#ifdef FEATURE_SUPPORT_BT_AUDIO
	pMe->m_bBtAvailable = FALSE;
#endif
	//Add End
#ifdef FEATURE_LCD_TOUCH_ENABLE
	pMe->m_bup= TRUE;
    pMe->m_i  = -1;
#endif

    (void) ISHELL_LoadResString(pMe->m_pShell,
                                            AEE_APPSCALLAPP_RES_FILE,
                                            IDS_NO_NUMBER,
                                            pMe->wszPrivateString_tw,
                                            sizeof(pMe->wszPrivateString_tw));

    pMe->m_pwstrDialStringkeep = NULL;

#ifdef WIN32//wlh for vkey number
	if( ISHELL_CreateInstance( pMe->m_pShell, AEECLSID_VKEY_CONTROL, (void **)&pMe->m_pIVkeyCtl)!=AEE_SUCCESS)
	{
		return EFAILD;
	}
#endif
	pMe->m_bShift = FALSE;   //add by yangdecai 2010-07-27
	
    return SUCCESS;

}

/*==============================================================================
函数:
       CallApp_FreeAppData

说明:
       释放CallApp Applet使用的相关资源。

参数:
       pMe [in]：指向CallApp Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
       无。

备注:

==============================================================================*/
static void CallApp_FreeAppData(CCallApp *pMe)
{
    if (NULL == pMe)
    {
        return ;
    }
#if 0//def FEATURE_VERSION_K212
     gpio_tlmm_config(GPIO_OUTPUT_10);
	clk_busy_wait(30*1000);
	gpio_out(GPIO_OUTPUT_10,(GPIO_ValueType)GPIO_LOW_VALUE);
	#endif

   FREEIF(pMe->m_strPhonePWD);
  

   // 释放 IConfig 接口
    if (pMe->m_pConfig != NULL)
    {
        ICONFIG_Release(pMe->m_pConfig);
        pMe->m_pConfig = NULL;
    }
#ifdef FEATURE_OEMOMH
    // 释放 IRUIM 接口
    if (pMe->m_pIRUIM != NULL)
    {
        IRUIM_Release(pMe->m_pIRUIM);
        pMe->m_pIRUIM = NULL;
    }
#endif
//Add by pyuangui 2013-01-10
#ifdef FEATURE_VERSION_C316
    if(pMe->m_pFileMgr)
    {
        IFILEMGR_Release(pMe->m_pFileMgr);
        pMe->m_pFileMgr = NULL;            
    }
#endif
//Add end
#ifdef FEATURE_ICM
    if(pMe->m_pICM != NULL)
    {
        ICM_Release(pMe->m_pICM);
        pMe->m_pICM = NULL;
    }
#else
    if(pMe->m_pITelephone != NULL)
    {
        ITELEPHONE_Release(pMe->m_pITelephone);
        pMe->m_pITelephone = NULL;
    }

	if(pMe->m_pICallMgr != NULL)
    {
        ICALLMGR_Release(pMe->m_pICallMgr);
        pMe->m_pICallMgr = NULL;
    }

	if(pMe->m_pIPhoneCtl != NULL)
    {
        IPHONECTL_Release(pMe->m_pIPhoneCtl);
        pMe->m_pIPhoneCtl = NULL;
    }
#endif
    if (pMe->m_pSound != NULL)
    {
        ISOUND_Release(pMe->m_pSound);
        pMe->m_pSound = NULL;
    }

    if (pMe->m_pAlert != NULL)
    {
        IALERT_Release(pMe->m_pAlert);
        pMe->m_pAlert = NULL;
    }
#ifdef KEYSND_ZY
    if (pMe->m_SndPlayer != NULL)
    {
        ISOUNDPLAYER_Stop(pMe->m_SndPlayer);
        ISOUNDPLAYER_Release(pMe->m_SndPlayer);
        pMe->m_SndPlayer = NULL;
    }
#endif /* KEYSND_ZY */

    if(pMe->m_pBacklight)
    {
        IBACKLIGHT_Release(pMe->m_pBacklight);
        pMe->m_pBacklight=NULL;
    }

    if (pMe->m_pIAnn)
    {
        IANNUNCIATOR_Release(pMe->m_pIAnn);
        pMe->m_pIAnn= NULL;
    }
    
    
    if ( pMe->m_pMenu )
    {
        IMENUCTL_Release ( pMe->m_pMenu );
        pMe->m_pMenu = NULL;
    }
    CallApp_Free_All_Call_Table(pMe);//ok
#ifdef FEATURE_ICM
    ISHELL_RegisterNotify(pMe->m_pShell,AEECLSID_DIALER,  AEECLSID_CM_NOTIFIER,0);
#else
    ISHELL_RegisterNotify(pMe->m_pShell,AEECLSID_DIALER,  AEECLSID_PHONENOTIFIER,0);
#endif
    ISHELL_RegisterNotify(pMe->m_pShell,AEECLSID_DIALER,  AEECLSID_ALERT_NOTIFIER,0);

#if defined( FEATURE_CALL_RECORDER)
	recorder_release_media_if( &pMe->m_Media);
	recorder_release_filemgr_if( &pMe->m_Media);
	{
		int i 		= 0;
		int number 	= ARRAY_SIZE( pMe->m_pImage);
		for( i = 0; i < number; i ++)
		{
			if( pMe->m_pImage[i])
			{
				IIMAGE_Release( pMe->m_pImage[i]);
				pMe->m_pImage[i] = NULL;
			}
		}
	}
#endif
#ifdef FEATURE_VERSION_K212
	snd_set_device(SND_DEVICE_STEREO_HEADSET, SND_MUTE_MUTED, SND_MUTE_MUTED, NULL, NULL);	
	snd_set_device(SND_DEVICE_HEADSET_FM, SND_MUTE_UNMUTED, SND_MUTE_UNMUTED, NULL, NULL);
#endif
}

/*==============================================================================
函数:
       CallApp_RunFSM

说明:
       CallApp Applet有限状态机引擎。

参数:
       pMe [in]：指向CallApp Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
       无。

备注:

==============================================================================*/
static void CallApp_RunFSM(CCallApp *pMe)
{
    NextFSMAction nextFSMAction = NFSMACTION_WAIT;

    for ( ; ; )
    {
        nextFSMAction = CallApp_ProcessState(pMe);
        //CALL_ERR("CallApp_RunFSM %d,%d",pMe->m_bNotOverwriteDlgRet,nextFSMAction,0);
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
       CallApp_HandleEvent

说明:
       CallApp  Applet事件处理主函数。所有属于本Applet的事件，
       都将经由该函数处理。

参数:
       pi [in]：指向ICallApp接口t对象的指针。
       eCode：BREW事件代码。
       wParam：与事件相关联的数据。
       dwParam [in]：与事件相关联的数据。

返回值:
       TRUE：传入事件被处理。
       FALSE：传入事件没被处理。

备注:

==============================================================================*/
static boolean CallApp_HandleEvent(ICallApp *pi,
                                     AEEEvent    eCode,
                                     uint16      wParam,
                                     uint32      dwParam)
{
    register CCallApp *pMe = (CCallApp*)pi;
    AEEAppStart *as;
    CALL_ERR("CallApp_HandleEvent:%x,%x,%x",eCode,wParam,dwParam);
    switch (eCode)
    {
        case EVT_APP_START:
            pMe->m_userCanceled = FALSE;
            pMe->m_eCurState = STATE_INIT;
            ASSERT(dwParam != 0);
            as = (AEEAppStart*)dwParam;

            if (NULL != pMe->m_pDisplay)
            {
                (void) IDISPLAY_Release(pMe->m_pDisplay);
                pMe->m_pDisplay = NULL;
            }

            pMe->m_pDisplay = as->pDisplay;

            ASSERT(pMe->m_pDisplay != NULL);

            (void) IDISPLAY_AddRef(pMe->m_pDisplay);
            //pMe->m_rc = as->rc;
            CallApp_Start ( pMe );

            // Save text line height for normal and large fonts
            pMe->m_LineHeight = IDISPLAY_GetFontMetrics(pMe->m_pDisplay,
                    AEE_FONT_NORMAL, NULL, NULL);

            //pMe->m_LargeLineHeight = IDISPLAY_GetFontMetrics(pMe->m_pDisplay,
            //        AEE_FONT_LARGE, NULL, NULL);
            pMe->m_bRefreshVol = REFRESHVOL_OFF;      // for display conversation text

            pMe->m_bSuspending = FALSE;
            pMe->m_running = TRUE;
#ifdef FEATURE_APP_PAUSE_TIMER
            pMe->m_PauseTimer = 0;
#endif
            pMe->AppStart = (AEEAppStart *)dwParam;
#ifdef  FEATURE_PERU_VERSION
            pMe->in_convert = FALSE; //to save another incoming call in conversation;
#endif
            pMe->m_CallMuted = TRUE;
#ifdef FEATRUE_SET_IP_NUMBER
            pMe->m_b_ip_call[0] = 0;
#endif
            IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, NULL);

			//Add By zzg 2011_10_25
			{
				uisnd_notify_data_s_type sndInfo;
				uisnd_get_device(&sndInfo);
				MSG_FATAL("***zzg CallApp_Handle EVT_APP_START - dev=%d sMute=%d mMute=%d***", 
			  			sndInfo.out_device, sndInfo.speaker_mute, sndInfo.microphone_mute);
			}
			//Add End

			//Add By zzg 2011_10_27
			MSG_FATAL("***zzg CallApp EVT_APP_START***", 0, 0, 0);
#ifdef FEATURE_SUPPORT_BT_AUDIO			
			(void) ISHELL_PostEvent(pMe->m_pShell,
									AEECLSID_BLUETOOTH_APP,
									EVT_USER,
									EVT_CALLAPP_CHECK_BT_STATUS,	
									0);			
#endif
			//Add End
			
            CallApp_SetupCallAudio(pMe);
            // 开始CallApp状态机
            
            CallApp_RunFSM(pMe);
            return TRUE;

        case EVT_ALARM:
            {
#ifdef FEATURE_ICM
                ICM *pICM = NULL;
                uint16 num = 0;
             
                if(AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell, 
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
#else
                ITelephone*pITelephone = NULL;
                uint16 num = 0;             

                if(AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell, 
                                                        AEECLSID_TELEPHONE, 
                                                        (void **)&pITelephone))
                {
                    return FALSE;
                }
                if(pITelephone)
                {
                
					AEETCalls po;
					if(SUCCESS != ITELEPHONE_GetCalls(pITelephone,                                            
	                                           &po, 
	                                           sizeof(AEETCalls)))
					{
						return FALSE;
					}
	                    
                    ITELEPHONE_Release(pITelephone);
                    pITelephone = NULL;
					num = po.dwCount;
                }
#endif
                if(num > 0)
                {
                    IANNUNCIATOR_SetField(pMe->m_pIAnn, ANNUN_FIELD_ALARM, ANNUN_STATE_ALARM_ON | ANNUN_STATE_BLINK);
                    IANNUNCIATOR_SetUnblinkTimer(pMe->m_pIAnn, ANNUN_FIELD_ALARM, ANNUN_STATE_ALARM_OFF, 5000);
                }
            }
            return TRUE;
        case EVT_APP_STOP:
            {
                boolean *pb = (boolean *)dwParam;
                //CALL_ERR("EVT_APP_STOP", 0, 0, 0);

                if(pb) // brew magic var
                {
                    // Set the app to background app
                    *pb = FALSE;
                }
            }
#ifdef FEATURE_LED_CONTROL
            //CallApp_DisableLedSig( pMe);
#endif
            if (pMe->m_pDisplay != NULL)
            {
                (void) IDISPLAY_Release(pMe->m_pDisplay);
                pMe->m_pDisplay = NULL;
            }
#ifdef FEATRUE_SET_IP_NUMBER
            pMe->m_b_ip_call[0] = 0;
#endif
            CALL_ERR("pMe->m_bSuspending %d %d",pMe->m_bSuspending,pMe->m_b_auto_redial,0);
            /*If in call state,we enter another aplet,and press end key ,dailer will
            end the call ,in this time,not free call_table because call_table will be used in call_end dialog*/
            if (pMe->m_bSuspending == FALSE)
            {
                CallApp_Free_All_Call_Table(pMe);//ok
            }
            pMe->m_bSuspending = TRUE;
            pMe->m_running   = FALSE;
            pMe->m_userCanceled = TRUE;
            //pMe->m_b_from_another = FALSE;
            // Ensure the Ringer and Missed Call Alert are turned off.
            pMe->m_MissedCallCount = 0;
            
#ifdef FEATURE_SUPPORT_BT_APP
            pMe->m_bt_audio = SEND_NONE;
            pMe->m_b_add_btag_menu = FALSE;
#endif

#ifdef FEATURE_SUPPORT_BT_AUDIO
			pMe->m_bBtAvailable = FALSE;	
#endif

            //IALERT_StopAlerting(pMe->m_pAlert);
            //IALERT_StopRingerAlert(pMe->m_pAlert);
            //ICM_EndAllCalls(pMe->m_pICM);
            //CallAppNotifyMP3PlayerAlertEvent(pMe,FALSE);
            (void) ISHELL_CancelTimer(pMe->m_pShell, NULL, pMe);
            //in call state,if user press end key in another app,the phone will back to coreapp
            if (pMe->m_bSuspending)
            {
                ISHELL_SetTimer(pMe->m_pShell, 400 ,(PFNNOTIFY)CallApp_Settime_To_EndCall,(void *)pMe);
            }

            if(pMe->m_b_auto_redial)
            {
                nv_auto_redial_type  b_auto_redial;
                ICONFIG_GetItem(pMe->m_pConfig, CFGI_AUTO_REDIAL, (void*)&b_auto_redial, sizeof(nv_auto_redial_type));
                if(b_auto_redial.enable && b_auto_redial.rings != 0)
                {
                    WSTRLCPY(pMe->m_DialString,pMe->m_call_info.call_number,AEECM_MAX_DIGITS_LENGTH);
                    ISHELL_SetTimer(pMe->m_pShell, b_auto_redial.rings *1000,(PFNNOTIFY)CallApp_MakeCall,(void *)pMe);
                }
            }

            if(pMe->m_pActiveDlg)
            {
                (void) ISHELL_EndDialog(pMe->m_pShell);
                pMe->m_pActiveDlg = NULL;
                pMe->m_pActiveDlgID = 0;
            }

            CallApp_Stop( pMe );
            //pMe->m_RedirNumStr[0] = 0;
            //pMe->m_DispInfo[0] = 0;
            //pMe->m_ExtendedDispInfo[0] = 0;
            //pMe->m_ExtendedDispInfo2[0] = 0;
            //pMe->m_CalledPartyNum[0] = 0;
            //CallApp_Reset_CDG_Info(pMe);
            MEMSET((void*)&pMe->m_cdg_dsp_info , 0 ,sizeof(cdg_dsp_info));
            pMe->m_eCurState = STATE_NULL;
            pMe->m_ePreState = STATE_NULL;
            pMe->m_nStartCallType = START_MAX;
            pMe->m_bHandFree = FALSE;
#ifndef FEATURE_USES_LOWMEM
            if(pMe->m_pConvImage)
            {
                IIMAGE_Release(pMe->m_pConvImage);
                pMe->m_pConvImage = NULL;
            }
#endif
            pMe->m_btime_out = 0;
            pMe->m_return_value = RETURN_ZERO;
            pMe->m_bincoming_rsk = IDS_MUTE;
            //pMe->m_b_show_cdg = FALSE;
            pMe->m_cdg_row = 0;
            pMe->m_msg_text_id = 0;
			pMe->m_prompt_id = 0;		//Add By zzg 2012_11_01
            CallApp_SetupCallAudio(pMe);
#ifdef FEATRUE_AUTO_POWER
            if(pMe->m_b_powerdown)
            {
                pMe->m_b_powerdown = FALSE;
                ISHELL_PostEvent(pMe->m_pShell,AEECLSID_CORE_APP,EVT_AUTO_POWERDOWN,1,1);
            }
#endif
#ifdef FEATURE_KEYGUARD
            if(pMe->m_b_set_keyguard)
            {
                OEMKeyguard_SetState(TRUE);
                pMe->m_b_set_keyguard = FALSE;
            }
#endif
            FREEIF(pMe->m_pwstrDialStringkeep);
            return TRUE;

        case EVT_APP_SUSPEND:
            pMe->m_bSuspending = TRUE;
            pMe->m_b_auto_redial = FALSE;
            pMe->m_auto_redial_count = 0;
            ISHELL_CancelTimer(pMe->m_pShell,
                                                        (PFNNOTIFY)CallApp_MakeCall, pMe);
            return TRUE;
        case EVT_FLIP:// wParam = TRUE if open, FALSE if closed.
#ifdef FEATURE_ICM
            if((AEECM_IS_VOICECALL_CONNECTED(pMe->m_pICM))&& pMe->m_bSuspending)
            {
                CallApp_Process_EVT_FLIP_Event(pMe,wParam);
            }
            else
            {
                CallApp_RouteDialogEvent(pMe,eCode,wParam,dwParam);
            }
#else
        	{   
				AEETCalls po;
				if(SUCCESS != ITELEPHONE_GetCalls(pMe->m_pITelephone,                                            
                                            &po, 
                                            sizeof(AEETCalls)))
				{
					return FALSE;
				}
	            if((po.dwCount>0)&& pMe->m_bSuspending)
	            {
	                CallApp_Process_EVT_FLIP_Event(pMe,wParam);
	            }
	            else
	            {
	                CallApp_RouteDialogEvent(pMe,eCode,wParam,dwParam);
	            }
        	}
#endif
            return TRUE;
            
        case EVT_APP_RESUME:
            ASSERT(dwParam != 0);
            as = (AEEAppStart*)dwParam;
            pMe->m_bSuspending = FALSE;

            if (pMe->m_pDisplay != NULL)
            {
                (void) IDISPLAY_Release(pMe->m_pDisplay);
                pMe->m_pDisplay = NULL;
            }
            pMe->m_pDisplay = as->pDisplay;

            ASSERT(pMe->m_pDisplay != NULL);
            
            IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, NULL);
            (void) IDISPLAY_AddRef(pMe->m_pDisplay);
            //pMe->m_rc = as->rc;
            CallApp_SetupCallAudio(pMe);
#ifdef FEATURE_UTK2
            {
                if (pUTKCALLinfo != NULL)
                {
                    if (pUTKCALLinfo->catgory==CALL_INFO_UTK)
                    {
                        CallAppState  eTsatate = STATE_CALLING_FROM_ANOTHERAPP;
                        Setup_Call  *pstCall;

                        pstCall = UTK_Get_Setup_Call_Buf();
                        (void)WSTRLCPY(pMe->m_DialString, pstCall->wszDialNum,MAX_SIZE_DIAL_STR);
                        if (WSTRLEN(pstCall->wszAlpha)>0)
                        {
                            eTsatate = STATE_CALLCONFIRM;
                        }
                        else
                        {
#ifdef FEATURE_ICM
                            //if (pMe->m_lastCallState == AEECM_CALL_STATE_CONV)
                            if(AEECM_IS_VOICECALL_CONNECTED(pMe->m_pICM))
#else
							AEETCalls po;
							if(SUCCESS != ITELEPHONE_GetCalls(pMe->m_pITelephone,                                            
															&po, 
															sizeof(AEETCalls)))
							{
								return FALSE;
							}
                            if(po.dwCount > 0)
#endif
                            {
                                switch(pstCall->cmd_describe.command_restricttag)
                                {
                                    // 当前处于通话状态，不能进行呼叫
                                    case 0x00:
                                    case 0x01:
                                        eTsatate = STATE_CONVERSATION;
                                        break;

                                    // 当前处于通话状态，挂断当前通话再呼出
                                    case 0x04:
                                    case 0x05:
                                        pMe->m_bEndcurcallForUTKCall = TRUE;
#ifdef FEATURE_ICM
                                        ICM_EndAllCalls(pMe->m_pICM);
#else
                                        ICALLMGR_EndAllCalls(pMe->m_pICallMgr);
#endif
                                        eTsatate = STATE_ENDCALL;
                                        break;

                                    // 尝试进入3方通话
                                    default:
                                        eTsatate = STATE_CALLING_FROM_ANOTHERAPP;
                                        break;
                                }
                            }
                        }

                        MOVE_TO_STATE(eTsatate)
                    }

                    // 及时清掉此值，不需释放
                    pUTKCALLinfo = NULL;
                }
            }
#endif //FEATURE_UTK2

            CallApp_RunFSM(pMe);
            return TRUE;

        case EVT_NOTIFY:
            //CALL_ERR("EVT_NOTIFY: %d", wParam, 0, 0);
            return CallApp_Notify(pMe, eCode, wParam, dwParam);
            
        case EVT_HEADSET:
            ISHELL_PostEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_USER_REDRAW,0,0);
            CallApp_SetupCallAudio(pMe);
            return TRUE;
            
        case EVT_APP_NO_SLEEP://保证不会进入睡眠模式
            return TRUE;

        //case EVT_USER_CLOSEAPP:
        //    (void)ISHELL_CloseApplet(pMe->m_pShell, FALSE);
        //    return TRUE;

        case EVT_DIALOG_INIT:
            pMe->m_pActiveDlg = (IDialog*)dwParam;
            pMe->m_pActiveDlgID = wParam;
            return CallApp_RouteDialogEvent(pMe,eCode,wParam,dwParam);

        case EVT_DIALOG_END:
            if (wParam == 0)
            {
                return TRUE;
            }
            (void) CallApp_RouteDialogEvent(pMe,eCode,wParam,dwParam);
            pMe->m_pActiveDlg = NULL;

            // Applet被挂起，不跑状态机，等待Applet返回。（注意：EVT_APP_SUSPEND
            // 事件在EVT_DIALOG_END事件前发出。
            if (pMe->m_bSuspending == FALSE)
            {
                // 开始CallApp状态机
                CallApp_RunFSM(pMe);
            }
#ifdef FEATURE_VERSION_K212
			{
				nv_item_type	SimChoice;
				SimChoice.sim_select =2;
				(void)OEMNV_Put(NV_SIM_SELECT_I,&SimChoice);
			}
#endif

            return TRUE;
#ifdef FEATRUE_AUTO_POWER
        case EVT_AUTO_POWERDOWN:
        {
#ifdef FEATURE_ICM
            if((pMe->m_pActiveDlg != NULL)|| AEECM_IS_VOICECALL_CONNECTED(pMe->m_pICM))
#else
			AEETCalls po;
			if(SUCCESS != ITELEPHONE_GetCalls(pMe->m_pITelephone,                                            
											&po, 
											sizeof(AEETCalls)))
			{
				return FALSE;
			}
            if((pMe->m_pActiveDlg != NULL)||(po.dwCount>0))
#endif
            {
                pMe->m_b_powerdown = TRUE;
            }
            else//in this ,it seem is data call,powerdown phone???
            {
                pMe->m_b_powerdown = FALSE;
                ISHELL_PostEvent(pMe->m_pShell,AEECLSID_CORE_APP,EVT_AUTO_POWERDOWN,1,1);
            }
            return TRUE;
        }
#endif

        case EVT_BATT_POWERDOWN:
            if(pMe->m_b_incall)
            {
#ifdef FEATURE_ICM
                ICM_EndCall(pMe->m_pICM, pMe->m_CallsTable->call_id);
#else
                ICall *pCall = NULL;
            
            	ICALLMGR_GetCall(pMe->m_pICallMgr,pMe->m_CallsTable->call_id,&pCall);
				if (pCall != NULL)
				{
					ICALL_End(pCall);
					ICALL_Release(pCall);
				}
#endif
            }
            return TRUE;

#ifdef FEATURE_SUPPORT_BT_APP
        case EVT_ORIGINATE_CALL:
        {
            AVKType key = AVK_SEND;/*0xe02f*/
#ifdef FEATURE_ICM
            pMe->m_b_incall = AEECM_IS_VOICECALL_CONNECTED(pMe->m_pICM);
#else
			AEETCalls po;
			if(SUCCESS != ITELEPHONE_GetCalls(pMe->m_pITelephone,                                            
												&po, 
												sizeof(AEETCalls)))
			{
				return FALSE;
			}
			pMe->m_b_incall = (po.dwCount>0)?TRUE:FALSE;
#endif
            MSG_FATAL("EVT_ORIGINATE_CALL %d %d %x",pMe->m_b_incall,pMe->m_Is3Way,wParam);
            if(wParam == 0)
            {
                if(pMe->m_b_incall && !pMe->m_Is3Way)
                {
                    key = AVK_ENDCALL;
                }
                /*Numedit used send key release to make a call*/
                if(NULL != pMe->m_pActiveDlg && IDD_NUMEDIT == pMe->m_pActiveDlgID)
                {
                    return CallApp_RouteDialogEvent(pMe,EVT_KEY_RELEASE,key,dwParam);
                }
            }
            else
            {
                /*in incoming state,when used bt headset to answer the call,the voice need send to bt ag*/
                if(NULL != pMe->m_pActiveDlg && IDD_INCOMINGCALL == pMe->m_pActiveDlgID)
                {
                    pMe->m_bt_audio = SEND_BT_CALL;
                }
                if(TRUE == pMe->m_b_incall)/*in call state,if ag disconnect, we need to switch device to handfree,and if ag reconnect,need switch to ag*/
                {
                    if(wParam == AVK_BT_HEADSET_DISCONNECT)
                    {
                        pMe->m_b_add_btag_menu = FALSE;
                        if(pMe->m_bt_audio ==  SEND_BT_AUDIO)/*only set audio if we use bluetooth headset*/
                        {
                            pMe->m_bt_audio = SEND_NONE;
                            pMe->m_bHandFree = TRUE;
                            CallApp_SetupCallAudio(pMe);
                        }
                        return TRUE;
                    }
                    else if(wParam == AVK_BT_HEADSET_CONNECT)
                    {
                        pMe->m_b_add_btag_menu = TRUE;//??
                        if(pMe->m_bt_audio ==  SEND_NONE)/*only set audio if we use bluetooth headset*/
                        {
                            pMe->m_bt_audio = SEND_BT_AUDIO;
                            pMe->m_bHandFree = FALSE;
                            CallApp_SetupCallAudio(pMe);
                        }
                        return TRUE;
                    }
                }
                key = (AVKType)wParam;
            }
            return CallApp_RouteDialogEvent(pMe,EVT_KEY,key,dwParam);
        }
#endif
		//Add By zzg 2011_10_27
		case EVT_USER:
		{
			MSG_FATAL("***zzg CallApp_HandleEvt EVT_USER wParam=%x***", wParam, 0, 0);
			#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
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
					return CallApp_RouteDialogEvent(pMe,eCode,wParam,dwParam);
				}
				else if(wParam == AVK_CLR)
				{
					eCode = EVT_KEY;
					return CallApp_RouteDialogEvent(pMe,eCode,wParam,dwParam);
				}
				else if((wParam >= AVK_0) && (wParam <= AVK_POUND))
				{
					eCode = EVT_KEY;
					return CallApp_RouteDialogEvent(pMe,eCode,wParam,dwParam);
				}				
				
			}
			#endif
			
#ifdef FEATURE_SUPPORT_BT_AUDIO
			if ((wParam == EVT_BT_AG_AUDIO_CONNECTED) || (wParam == EVT_BT_A2DP_AUDIO_CONNECTED))			
			{              
				pMe->m_bBtAvailable = TRUE;		
				//CallApp_SetupCallAudio(pMe);

				//Add By zzg 2012_12_05
				CallApp_SwitchCallAudio(pMe, TRUE);	
					
				pMe->m_bHandFree = FALSE;
				CallApp_SetupCallAudio(pMe);
				//Add End
			}
			else if ((wParam == EVT_BT_AG_AUDIO_DISCONNECTED) || (wParam == EVT_BT_A2DP_AUDIO_DISCONNECTED))
			{				
				pMe->m_bBtAvailable = FALSE;	
				CallApp_SwitchCallAudio(pMe, FALSE);	
				pMe->m_bHandFree = FALSE;
				CallApp_SetupCallAudio(pMe);				
			}
#endif
			break;
			
		}
		//Add End


#ifdef FEATURE_OEMOMH
        case EVT_OMH_PROMPT:
            if(!gsdi_uim_omh_cap.omh_enabled && IRUIM_IsCardConnected(pMe->m_pIRUIM))
            {
                MSG_FATAL("EVT_OMH_PROMPT %d 0x%x",pMe->m_running,ISHELL_ActiveApplet(pMe->m_pShell),0);
                if (!pMe->m_running)
                {
#if 0
                    ISHELL_PostEvent(pMe->m_pShell,ISHELL_ActiveApplet(pMe->m_pShell),EVT_OMH_PROMPT,0,0);
#ifdef FEATURE_ICM
                    ICM_EndAllCalls(pMe->m_pICM);
#else
                    ICALLMGR_EndAllCalls(pMe->m_pICallMgr);
#endif
#endif
#ifndef FEATURE_NOOMHPROMPT
                    pMe->m_nStartCallType = START_NONOMH;
                    pMe->m_clsOMHApplet   = dwParam;
                    ISHELL_StartApplet(pMe->m_pShell, AEECLSID_DIALER/*AEECLSID_CALL*/);
#endif
                    return TRUE;
                }
            }
            break;
#endif
        default:
            // 将接收到的事件路由至当前活动的对话框事件处理函数。
            return CallApp_RouteDialogEvent(pMe,eCode,wParam,dwParam);
    }
    return FALSE;
}


/*==============================================================================
函数:
       CallApp_SetStartCallType

说明:
     It seem not use.

参数:


返回值:
       无。

备注:

==============================================================================*/
//static int CallApp_SetStartCallType(ICallApp *p, start_call_type type, AECHAR *num, callPIType pi, AECHAR *name)
//{
//    CCallApp *pMe = (CCallApp *)p;

//    CALL_FUN_START("CallApp_SetStartCallType",0,0,0);

//    pMe->m_nStartCallType = type;
//    CallApp_Add_Number_To_Call_Table(pMe,num,0,AEECALLHISTORY_CALL_TYPE_FROM,pi,FALSE);
//    return SUCCESS;
//}

/*==============================================================================
函数:
       CallApp_CallNumber

说明:
     Start CallApp  to dialer the number from another Applet.

参数:


返回值:
       无。

备注:

==============================================================================*/
static int CallApp_CallNumber(ICallApp *p, AECHAR *number)
{
    register CCallApp *pMe = (CCallApp *)p;
    //static call_start_info_type  call_start_info_call;

    CALL_FUN_START("CallApp_CallNumber",0,0,0);

    if (NULL == pMe || NULL == number)
    {
        return EFAILED;
    }

    if (0 == *number)
    {
        return EFAILED;
    }

    if (ISHELL_ActiveApplet(pMe->m_pShell) == AEECLSID_DIALER/*AEECLSID_CALL*/)
    {
        // Make directly speeddial call
        WSTRLCPY(pMe->m_DialString, number ,MAX_SIZE_DIAL_STR);
        if(CALL_SUCESS == CallApp_MakeCall(pMe))
        {
            return SUCCESS;
        }
        else
        {
            return EFAILED;
        }
    }

    call_start_info_call.b_closeAll = CALL_INFO_NO;
    call_start_info_call.catgory = CALL_INFO_SEND;
    WSTRTOSTR(number,call_start_info_call.dial_str, sizeof(call_start_info_call.dial_str));

    STRTOWSTR(call_start_info_call.dial_str, pMe->m_DialString, sizeof(pMe->m_DialString));
    MOVE_TO_STATE(STATE_CALLING_FROM_ANOTHERAPP)
    if (ISHELL_StartAppletArgs(pMe->m_pShell,
           AEECLSID_DIALER,
           (const char *)&call_start_info_call) != SUCCESS)
    {
        CALL_ERR("^&^& Start AEECLSID_DIALER fail", 0,0,0);
        return EFAILED;
    }

    return SUCCESS;
}

#ifdef FEATURE_EDITABLE_RECORD
static int CallApp_EditRecNumber(ICallApp *p, AECHAR *number, uint16 call_type)
{
    register CCallApp *pMe = (CCallApp *)p;
    if (NULL == pMe || NULL == number || WSTRLEN(number) == 0)
    {
        return EFAILED;
    }

    call_start_info_call.b_closeAll = CALL_INFO_NO;
    call_start_info_call.catgory = CALL_INFO_TO + (call_type - AEECALLHISTORY_CALL_TYPE_TO);
    WSTRTOSTR(number,call_start_info_call.dial_str, sizeof(call_start_info_call.dial_str));

    STRTOWSTR(call_start_info_call.dial_str, pMe->m_DialString, sizeof(pMe->m_DialString));
    MOVE_TO_STATE(STATE_EDIT_REC_NUMBER)
    if (ISHELL_StartAppletArgs(pMe->m_pShell,
           AEECLSID_DIALER,
           (const char *)&call_start_info_call) != SUCCESS)
    {
        return EFAILED;
    }

    return SUCCESS;
}
#endif
/*==============================================================================
函数:
       CallApp_IsRestictCallNumber

说明:
     It seem not use.

参数:


返回值:
       无。

备注:

==============================================================================*/
//static boolean CallApp_IsRestictCallNumber(ICallApp *p, AECHAR *pNumber,boolean IsOutgoing)
//{
//    register CCallApp   *pMe = (CCallApp *)p;

//    CALL_FUN_START("CallApp_IsRestictCallNumber",0,0,0);

//    if( pMe == NULL )
//    {
//        return FALSE;
//    }

//    return CallApp_IsRestictCallNumber_Ex(pMe, pNumber, IsOutgoing);
//}

/*=============================================================================
FUNCTION: CallApp_Notify

DESCRIPTION: Handles the EVT_NOTIFY event

PARAMETERS:
   *pMe: CCallApp object pointer
   *msg:

RETURN VALUE:
    boolean

COMMENTS:


SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CallApp_Notify(void *pUser, AEEEvent eCode,
                              uint16 wParam, uint32 dwParam)
{
    dword clsid;
    CCallApp *pMe = (CCallApp*) pUser;
    AEENotify   *Notify_msg = (AEENotify *) dwParam;

    ASSERT(pMe != NULL);
    ASSERT(Notify_msg != NULL);

    CALL_FUN_START("CallApp_Notify %x",Notify_msg->cls,0,0);

#ifdef Temp_remove
    //Call Idle applets service
    if (ISHELL_ActiveApplet(pMe->m_pShell) != AEECLSID_DIALER/*AEECLSID_CALL*/)
    {
        IIDLE_GetState(pMe->m_pIdle, &pMe->idle_info);
        if(!IsRunAsUIMVersion(void))
        {
            pMe->idle_info.uimLocked = FALSE;
        }
    }
#endif/*Temp_remove*/
    //CALL_ERR("Notify_msg->cls %x",Notify_msg->cls,0,0);
    switch (Notify_msg->cls)
    {
        case AEECLSID_ALERT_NOTIFIER:
            switch(wParam)
            {
                case NMASK_ALERT_ONOFF:
                    return TRUE;

                case NMASK_ALERT_MUTED:
                    return TRUE;

                default:
                    return FALSE;
            }
#ifdef FEATURE_ICM
        case AEECLSID_CM_NOTIFIER:
#else
        case AEECLSID_PHONENOTIFIER:
#endif
        {
            AEECMNotifyInfo *EventInfo = (AEECMNotifyInfo *)(Notify_msg->pData);
            //CALL_ERR("[DlgManager]dwMask = %x byCMCallID=%d ", Notify_msg->dwMask,
            //                        EventInfo->event_data.call.call_id, 0);

            switch (Notify_msg->dwMask)
            {
#ifdef FEATURE_ICM
                case NMASK_CM_VOICE_CALL:
                case NMASK_CM_TEST_CALL:
                case NMASK_CM_OTHER_CALL:
#ifdef FEATURE_OEMOMH
                case NMASK_CM_DATA_CALL:
#endif
#else
                case AEET_NMASK_VOICE_CALL:
                case AEET_NMASK_TEST_CALL:
                case AEET_NMASK_OTHER_CALL:
#ifdef FEATURE_OEMOMH
                case AEET_NMASK_DATA_CALL:
#endif
#endif
                    //CALL_ERR("NMASK_CM_VOICE_CALL %x",EventInfo->event,0,0);
                    CallApp_Handle_Call_StateChange(pMe, EventInfo);
                    return TRUE;
                    
                //case NMASK_CM_PHONE:
                //    CALL_ERR("NMASK_CM_PHONE %x",EventInfo->event,0,0);
                //    if (pMe->idle_info.inLPM)
                //    {
                //        return FALSE;
                //    }
                //
                //    CallApp_Handle_Phone_StateChange(pMe, EventInfo);
                //    break;

                //case NMASK_CM_SS:
                //    CALL_ERR("NMASK_CM_SS %x",EventInfo->event,0,0);
                //    CallApp_Handle_SS_StateChange(pMe, EventInfo);
                //    break;

                default:
                    CALL_ERR("Unhandled notification (0x%X).",Notify_msg->dwMask,0,0);
                    return FALSE;
            }

        }
        case AEECLSID_SHELL:
            CALL_ERR("AEECLSID_SHELL %x",Notify_msg->dwMask,0,0);
            if (Notify_msg->dwMask == NMASK_SHELL_START_STATUS)
            {
                (void)ISHELL_RegisterNotify(pMe->m_pShell, 
                                            AEECLSID_DIALER,
                                            AEECLSID_SHELL,
                                            0);
                ICallApp_AddRef((ICallApp*)pMe);
            }
            return TRUE;
            
        default:
            CALL_ERR("Unhandled notification class (0x%X).",Notify_msg->cls,0,0);
            return FALSE;
    }

    return TRUE;
} /* CallApp_Notify() */
#if 0
/*=============================================================================
FUNCTION: CallApp_ProcessBattNotify

DESCRIPTION:

PARAMETERS:
   *pMe: CCallApp object pointer
   *msg:

RETURN VALUE:
    boolean

COMMENTS:


SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void  CallApp_ProcessBattNotify(CCallApp  *pMe, AEENotify   *msg)
{
    CALL_FUN_START("CallApp_ProcessBattNotify %d %d", msg->dwMask, msg->pData, 0);

    switch (msg->dwMask)
    {
        case NMASK_BATTNOTIFIER_BATTSTATUS_CHANGE:
        {
            AEEBattStatus * pBattStatus;
            pBattStatus = (AEEBattStatus * )msg->pData;
            switch(*pBattStatus)
            {
                case AEEBATTSTATUS_POWERDOWN:  // Phone must be powered down
                    CallApp_Change_Call_Table_All_Call_End_Time(pMe);
                    CallApp_Add_All_Call_History(pMe);
                    CallApp_Free_All_Call_Table(pMe);
                    //start powering down the phone
                    (void)ISHELL_CloseApplet(pMe->m_pShell, TRUE);  // return to idle to power down
                    break;

                 case AEEBATTSTATUS_LOW:        // Battery is low
                    pMe->m_bNotOverwriteDlgRet = FALSE;
                    CallApp_Process_Batty_Msg(pMe, IDS_LOW_BATTERY_MSG);
                    //CallApp_Init_Msgbox_String(pMe, IDS_LOW_BATTERY_MSG, NULL);
                    //CallApp_ShowDialog(pMe, IDD_MSGBOX);
                    break;

                default:
                    break;
            }
            break;
        }

        case NMASK_BATTNOTIFIER_CHARGERSTATUS_CHANGE:
        {
            AEEChargerStatus *pChargerStatus;
            pChargerStatus = msg->pData;
            switch(*pChargerStatus)
            {
                case AEECHG_STATUS_FULLY_CHARGE:
                {
                    CALL_ERR("AEECHG_STATUS_FULLY_CHARGE",0,0,0);
                    pMe->m_bNotOverwriteDlgRet = FALSE;
                    CallApp_Process_Batty_Msg(pMe, IDS_FULLY_BATTERY_MSG);
                    break;
                }

                default:
                    break;
            }
            break;
        }
            
#if 0
        case NMASK_BATTNOTIFIER_BATTLEVEL_CHANGE:
        {
            //if were in a call, log it in recent calls before powering down the  phone
            AEEBattLevel *battlevel = (AEEBattLevel*)msg->pData;
            if(battlevel->level == 0)
            {
                CallApp_Change_Call_Table_All_Call_End_Time(pMe);
                CallApp_Add_All_Call_History(pMe);
                CallApp_Free_All_Call_Table(pMe);
                //start powering down the phone
                (void)ISHELL_CloseApplet(pMe->m_pShell, TRUE);  // return to idle to power down
            }
            break;
        }
#endif
        default:
            CALL_ERR("Unknown IBatt notification %x", msg->dwMask, 0, 0);
            break;
    }
}
//#if 0
/*=============================================================================
FUNCTION: CallApp_HandlePhoneStateChange

DESCRIPTION: Phone State notification event handler.

PARAMETERS:
   *pMe: CCallApp object pointer
   *pPhoneState: current phone state

RETURN VALUE:
    None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:
   cm_ph_info_s_type
   CallApp_Notify()
=============================================================================*/
static void CallApp_Handle_Phone_StateChange(CCallApp *pMe,
                                             AEECMNotifyInfo  *p_Phone_Info)
{

    CallAppState newState = STATE_NULL;
    AEECMPhEventData *p_Ph_Info= &(p_Phone_Info->event_data.ph);

    CALL_FUN_START("CallApp_Handle_Phone_StateChange",0,0,0);

    if (NULL == p_Ph_Info)
    {
#ifdef FEATURE_ICM
        ICM_GetPhoneInfo(pMe->m_pICM, p_Ph_Info, sizeof(AEECMPhInfo));
        if (NULL == p_Ph_Info)
        {
            return;
        }

        p_Phone_Info->event= AEECM_EVENT_NONE;
#else
        ICM_GetPhoneInfo(pMe->m_pITelephone, p_Ph_Info, sizeof(AEECMPhInfo));
        if (NULL == p_Ph_Info)
        {
            return;
        }

        p_Phone_Info->event= AEET_EVENT_NONE;
#endif
    }

    if (pMe->idle_info.inLPM)
    {
        // Ignore while in LPM
        return;
    }

    switch (p_Phone_Info->event)
    {
#ifdef FEATURE_ICM
        /*Operating mode was changed*/
        case AEECM_EVENT_PH_OPRT_MODE:
#else
        case AEET_EVENT_PH_OPRT_MODE:
#endif
        {
            CALL_MSG_HIGH("AEECM_EVENT_PH_OPRT_MODE %d",p_Ph_Info->oprt_mode,0,0);
            switch (p_Ph_Info->oprt_mode)
            {
#ifdef FEATURE_ICM
                case AEECM_OPRT_MODE_PWROFF:/* Phone is powering off                            */
                case AEECM_OPRT_MODE_FTM:/* Phone is in factory test mode                    */
                case AEECM_OPRT_MODE_OFFLINE: /* Phone is offline                                 */
                case AEECM_OPRT_MODE_OFFLINE_AMPS:/* Phone is offline analog                          */
                case AEECM_OPRT_MODE_OFFLINE_CDMA:/* Phone is offline cdma                            */
                case AEECM_OPRT_MODE_ONLINE: /* Phone is online                                  */
                case AEECM_OPRT_MODE_LPM:/* Phone is in LPM - Low Power Mode                 */
                case AEECM_OPRT_MODE_RESET:/* Phone is resetting - i.e. power-cycling          */
                case AEECM_OPRT_MODE_NET_TEST_GW:
#else
                case AEET_OPRT_MODE_PWROFF:/* Phone is powering off                            */
                case AEET_OPRT_MODE_FTM:/* Phone is in factory test mode                    */
                case AEET_OPRT_MODE_OFFLINE: /* Phone is offline                                 */
                case AEET_OPRT_MODE_OFFLINE_AMPS:/* Phone is offline analog                          */
                case AEET_OPRT_MODE_OFFLINE_CDMA:/* Phone is offline cdma                            */
                case AEET_OPRT_MODE_ONLINE: /* Phone is online                                  */
                case AEET_OPRT_MODE_LPM:/* Phone is in LPM - Low Power Mode                 */
                case AEET_OPRT_MODE_RESET:/* Phone is resetting - i.e. power-cycling          */
                case AEET_OPRT_MODE_NET_TEST_GW:
#endif
                /* Phone is conducting network test for GSM/WCDMA.  */
                /* This mode can NOT be set by the clients. It can  */
                /* only be set by the lower layers of the stack.    */
                    break;

                default:
                    break;
            }
        }
        break;

#ifdef FEATURE_ICM
        case AEECM_EVENT_PH_SYSTEM_PREF:        /* System preference changed */
#else
        case AEET_EVENT_PH_SYSTEM_PREF:        /* System preference changed */
#endif
            CALL_MSG_HIGH("AEECM_EVENT_PH_SYSTEM_PREF",0,0,0);
            break;
#ifdef FEATURE_ICM
        case AEECM_EVENT_PH_ANSWER_VOICE:       /* Answer voice as data was changed. */
#else
        case AEET_EVENT_PH_ANSWER_VOICE:       /* Answer voice as data was changed. */
#endif
            CALL_MSG_HIGH("AEECM_EVENT_PH_ANSWER_VOICE %d",p_Ph_Info->answer_voice,0,0);
            switch (p_Ph_Info->answer_voice)
            {
                // TBD
                default:
                    break;
            }
#ifdef FEATURE_ICM
        case AEECM_EVENT_PH_NAM_SEL:        /* NAM selection was changed */
#else
        case AEET_EVENT_PH_NAM_SEL:        /* NAM selection was changed */
#endif
            CALL_MSG_HIGH("AEECM_EVENT_PH_NAM_SEL %d",p_Ph_Info->nam_sel,0,0);
            switch (p_Ph_Info->nam_sel)
            {
                // TBD
                default:
                    break;
            }
#ifdef FEATURE_ICM
        case AEECM_EVENT_PH_CURR_NAM:       /* Current NAM was changed. */
#else
        case AEET_EVENT_PH_CURR_NAM:       /* Current NAM was changed. */
#endif
            CALL_MSG_HIGH("AEECM_EVENT_PH_CURR_NAM %d",p_Ph_Info->curr_nam,0,0);
            switch (p_Ph_Info->curr_nam)
            {
                // TBD
                default:
                    break;
            }
#ifdef FEATURE_ICM
        case AEECM_EVENT_PH_IN_USE_STATE:       /* In use state was changed */
#else
        case AEET_EVENT_PH_IN_USE_STATE:       /* In use state was changed */
#endif
            CALL_MSG_HIGH("AEECM_EVENT_PH_IN_USE_STATE %d",p_Ph_Info->is_in_use,0,0);
            break;
#ifdef FEATURE_ICM
        case AEECM_EVENT_PH_CDMA_LOCK_MODE:     /* CDMA lock mode was changed. */
#else
        case AEET_EVENT_PH_CDMA_LOCK_MODE:     /* CDMA lock mode was changed. */
#endif
            CALL_MSG_HIGH("AEECM_EVENT_PH_CDMA_LOCK_MODE %d",p_Ph_Info->cdma_lock_mode,0,0);
            switch (p_Ph_Info->cdma_lock_mode)
            {
                // TBD
                default:
                    break;
            }
#ifdef FEATURE_ICM
        case AEECM_EVENT_PH_MAINTREQ:       /* CDMA maintenance required command. */
#else
        case AEET_EVENT_PH_MAINTREQ:       /* CDMA maintenance required command. */
#endif
            // TBD
            CALL_MSG_HIGH("AEECM_EVENT_PH_MAINTREQ",0,0,0);
            break;
#ifdef FEATURE_ICM
        case AEECM_EVENT_PH_STANDBY_SLEEP:      /* Entering powerdown sleep mode */
#else
        case AEET_EVENT_PH_STANDBY_SLEEP:      /* Entering powerdown sleep mode */
#endif
            CALL_MSG_HIGH("AEECM_EVENT_PH_STANDBY_SLEEP",0,0,0);
            break;
#ifdef FEATURE_ICM
        case AEECM_EVENT_PH_STANDBY_WAKE:       /* Exiting powerdown sleep mode */
#else
        case AEET_EVENT_PH_STANDBY_WAKE:       /* Exiting powerdown sleep mode */
#endif
            CALL_MSG_HIGH("AEECM_EVENT_PH_STANDBY_WAKE %d",pMe->m_lastCallState,0,0);
            // Only force a change back to Idle, if no other call event has
            // already made the change out of Power Save for us.
#ifdef FEATURE_ICM
            if (pMe->m_lastCallState == AEECM_CALL_STATE_IDLE)
#else
            if (pMe->m_lastCallState == AEET_CALL_STATE_IDLE)
#endif
            {
                newState = STATE_EXIT;
            }
            break;
#ifdef FEATURE_ICM
        case AEECM_EVENT_PH_INFO_AVAIL:     /* Phone information is now available */
#else
        case AEET_EVENT_PH_INFO_AVAIL:     /* Phone information is now available */
#endif
            CALL_MSG_HIGH("AEECM_EVENT_PH_INFO_AVAIL",0,0,0);
            break;
#ifdef FEATURE_ICM
        case AEECM_EVENT_PH_SUBSCRIPTION:       /* Subscription info changed */
        case AEECM_EVENT_PH_FUNDS_LOW:          /* Funds running low. */
#else
        case AEET_EVENT_PH_SUBSCRIPTION:       /* Subscription info changed */
        case AEET_EVENT_PH_FUNDS_LOW:          /* Funds running low. */
#endif
            CALL_MSG_HIGH("AEECM_EVENT_PH_FUNDS_LOW",0,0,0);
            break;
#ifdef FEATURE_ICM
        case AEECM_EVENT_PH_NVRUIM_CONFIG:     /* RTRE configuration changed. */
#else
        case AEET_EVENT_PH_NVRUIM_CONFIG:     /* RTRE configuration changed. */
#endif
            CALL_MSG_HIGH("AEECM_EVENT_PH_NVRUIM_CONFIG",p_Ph_Info->rtre_config,0,0);
            break;
#ifdef FEATURE_ICM
        case AEECM_EVENT_PH_ACM_RESET:          /* Accumulated Call Meter was reset */
        case AEECM_EVENT_PH_ACMMAX_SET:         /* Accumulated Call Meter was set */
#else
        case AEET_EVENT_PH_ACM_RESET:          /* Accumulated Call Meter was reset */
        case AEET_EVENT_PH_ACMMAX_SET:         /* Accumulated Call Meter was set */
#endif
            CALL_MSG_HIGH("AEECM_EVENT_PH_ACMMAX_SET",0,0,0);
            break;

        default:
            CALL_MSG_HIGH("Invalid event =%x",p_Phone_Info.event,0,0);
            break;
    }
    if (newState != STATE_NULL)
    {
        // 将当前状态移至新的状态
        MOVE_TO_STATE(newState)
        if (!pMe->m_bSuspending)
        {
            // 若本 Applet 处于挂起状态，Applet返回时将自动进入新的状态；
            // 否则需关闭当前对话框再跑状态机
            CLOSE_DIALOG(DLGRET_CREATE)
        }
    }
    return;

}/* CallApp_HandlePhoneStateChange() */

#endif
/*=============================================================================
FUNCTION: CallApp_HandleCallStateChange

DESCRIPTION: Call state notification event handler.

PARAMETERS:
   *pMe: CCallApp object pointer
   *pCallInfo: Current phone call state

RETURN VALUE:
    None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:
   cm_call_info_s_type
   CallApp_Notify()
=============================================================================*/
static void CallApp_Handle_Call_StateChange(CCallApp *pMe,
                                             AEECMNotifyInfo  *p_Call_Info)
{
    CallAppState newState = STATE_NULL;
#ifdef FEATURE_ICM
    AEECMCallEventData *p_Call_Eventdata_Info =NULL;
    AEECMCallInfo          *p_cm_call_info = NULL;
#else
    AEETCallEventData *p_Call_Eventdata_Info =NULL;
    AEETCallInfo          *p_cm_call_info = NULL;
#endif
    CALL_FUN_START("CallApp_Handle_Call_StateChange",0,0,0);

    if (NULL == p_Call_Info)
    {
        // NULL call state, don't care
        return;
    }

    p_Call_Eventdata_Info= &(p_Call_Info->event_data.call);
    if (NULL == p_Call_Eventdata_Info)
    {
        // NULL call state, don't care
        return;
    }

    p_cm_call_info = &(p_Call_Eventdata_Info->call_info);

    // Should never change call state while in LPM
    // 解决在充电、输入手机密码界面下来电重启问题
    //ASSERT(FALSE == pMe->m_inLPM);

    CALL_ERR( "CALL TYPE: %d, STATE: %d, EVENT: %x",
    p_cm_call_info->call_type, p_cm_call_info->call_state, p_Call_Info->event);

    switch (p_cm_call_info->call_type)
    {
#ifdef FEATURE_ICM
        case AEECM_CALL_TYPE_OTAPA:
#else
        case AEET_CALL_TYPE_OTAPA:
#endif
            //CALL_ERR("AEECM_CALL_TYPE_OTAPA",0,0,0);
            CallApp_ProcessCallStateOTAPA(pMe, p_Call_Info, &newState);
            break;
#ifdef FEATURE_ICM
        case AEECM_CALL_TYPE_SMS:
        case AEECM_CALL_TYPE_PD:
#else
        case AEET_CALL_TYPE_SMS:
        case AEET_CALL_TYPE_PD:
#endif
            // The Idle Applet is not registered for these events and will
            // be handled by other Applets, if at all.
            CALL_ERR("AEECM_CALL_TYPE_SMS??",0,0,0);
            return;
            //ASSERT_NOT_REACHABLE;
            //break;
#if 1
#ifdef FEATURE_ICM
        case AEECM_CALL_TYPE_CS_DATA:
        case AEECM_CALL_TYPE_PS_DATA:
#else
        case AEET_CALL_TYPE_CS_DATA:
        case AEET_CALL_TYPE_PS_DATA:
#endif
            //CALL_ERR("AEECM_CALL_TYPE_CS_DATA",0,0,0);
            CallApp_ProcessCallStateDATA(pMe, p_Call_Info, &newState);
            break;
#endif
#ifdef FEATURE_ICM
        case AEECM_CALL_TYPE_TEST:
#else
        case AEET_CALL_TYPE_TEST:
#endif
            //CALL_ERR("AEECM_CALL_TYPE_TEST",0,0,0);
            CallApp_ProcessCallStateTest(pMe, p_Call_Info, &newState);
            /*lint -fallthrough*/
#ifdef FEATURE_ICM
        case AEECM_CALL_TYPE_NON_STD_OTASP:
        case AEECM_CALL_TYPE_STD_OTASP:
#else
        case AEET_CALL_TYPE_NON_STD_OTASP:
        case AEET_CALL_TYPE_STD_OTASP:
#endif
        // OTASP (by definition, MO) calls are initiated by the user, are
        // thusly NOT transparent, and should be handled like voice calls.

        // TO DO: We may want to handle OTASP calls a little differently
        //       from normal voice calls.  Maybe display a 'OTASP call'
        //       dialog or something.

        // FALL THROUGH
#ifdef FEATURE_ICM
        case AEECM_CALL_TYPE_VOICE:
        case AEECM_CALL_TYPE_EMERGENCY:
        case AEECM_CALL_TYPE_NONE:
#else
        case AEET_CALL_TYPE_VOICE:
        case AEET_CALL_TYPE_EMERGENCY:
        case AEET_CALL_TYPE_NONE:
#endif
            // 解决在充电、输入手机密码界面下来电重启问题

            //CALL_ERR("AEECM_CALL_TYPE_VOICE",0,0,0);
            CallApp_ProcessCallStateVoice(pMe, p_Call_Info, &newState);
            break;

        default:
        {
#ifdef FEATURE_ICM
            AEECMCallEventData    *call_table = NULL;	
#else
            AEETCallEventData    *call_table = NULL;	
#endif
            call_table = &(p_Call_Info->event_data.call); 
            //CALL_ERR("3333333333333",0,0,0);
            //if((p_Call_Info->event == AEECM_EVENT_CALL_SIGNAL) && (AEECM_IS_VOICECALL_CONNECTED(pMe->m_pICM)))
#ifdef FEATURE_ICM
            if((p_Call_Info->event == AEECM_EVENT_CALL_SIGNAL))
#else
            if((p_Call_Info->event == AEET_EVENT_CALL_SIGNAL))
#endif
            {
                //  call_table->signal.signal_type=AEECM_SIGNAL_CDMA_IS54B;
                // call_table->signal.signal.cdma_tone=AEECM_CDMA_TONE_BUSY;
                CallApp_ProcessCallStateVoiceSignal(pMe, call_table, &newState);
                // CALL_ERR("DDDDDDDDDDDDDDDFFFFFFFFFFFFFFFF",0,0,0);
            }
#ifdef FEATURE_ICM
            else if (p_Call_Info->event ==  AEECM_EVENT_CALL_DISPLAY)//cdg 9.71
#else
            else if (p_Call_Info->event ==  AEET_EVENT_CALL_DISPLAY)//cdg 9.71
#endif
            {
                CallApp_ProcessCallStateVoiceDisplay(pMe, p_Call_Info, &newState);
            }
            else
            {
                newState = STATE_EXIT;
            }
            break;
        }        
    }

    CALL_ERR("state:%x run = %d,suspend = %d",newState,pMe->m_running,pMe->m_bSuspending);
    if (newState != STATE_NULL)
    {
        if (pMe->m_running)
        {
            //CALL_ERR("CallApp APP  RUNNING, ", 0,0,0);
            if(pMe->m_CallsTable == NULL)
            {
                CALL_ERR("Warning:m_CallsTable == NULL", 0,0,0);
                return;
            }

            //CALL_ERR(" %d pMe->m_bSuspending", pMe->m_bSuspending,0,0);
            if (pMe->m_bSuspending)
            {
                if(pMe->m_CallsTable->b_restict
                                    &&(newState == STATE_INCOMINGCALL ||newState == STATE_EXIT))
                {
                    CALL_ERR("We can enter here?",0,0,0);
                    if(newState == STATE_EXIT)
                    {
                        //CALL_ERR("The CallApp will be close,back to IDLE",0,0,0);
                        CallApp_Free_All_Call_Table(pMe);
                    }

                    return;
                }
                MOVE_TO_STATE(newState)
                (void)ISHELL_StartApplet(pMe->m_pShell, AEECLSID_DIALER/*AEECLSID_CALL*/);
            }
            else /*!pMe->m_bSuspending*/
            {
                CALL_ERR(" new %d cur %d RUNNING, NOT SUSPENDED", newState,pMe->m_eCurState,0);
                if(newState == STATE_CALLING && pMe->m_eCurState == STATE_CALLING)
                {
                    (void) ISHELL_PostEvent(pMe->m_pShell, AEECLSID_DIALER, EVT_USER_REDRAW, 0,0);
                    return;
                }

                if (newState != pMe->m_eCurState)
                {
                    if(pMe->m_CallsTable->b_restict  &&(newState == STATE_INCOMINGCALL))
                    {
                        return;
                    }
                    MOVE_TO_STATE(newState)
                    if(pMe->m_pActiveDlg != NULL)
                    {
                        CLOSE_DIALOG(DLGRET_CREATE)
                    }
                    else
                    {
                        pMe->m_eDlgRet = DLGRET_CREATE;
                    }
                }
                else
                {
                    if(newState == STATE_INCOMINGCALL && pMe->m_eCurState == STATE_INCOMINGCALL)
                    {
                        if(pMe->m_CallsTable->b_restict)
                        {
                            return;
                        }
                        MOVE_TO_STATE(newState)
                        if(pMe->m_pActiveDlg != NULL)
                        {
                            CLOSE_DIALOG(DLGRET_CREATE)
                        }
                        else
                        {
                            pMe->m_eDlgRet = DLGRET_CREATE;
                        }
                        return;
                    }
                    (void) ISHELL_PostEvent(pMe->m_pShell, AEECLSID_DIALER,EVT_USER_REDRAW, 0,0);
                    CALL_ERR("Change to same state: %d", newState,0,0);
                }
            } /*pMe->m_bSuspending*/
        }
        else
        {
            //CALL_ERR("CallApp APP NOT RUNNING, ", 0,0,0);
            // Don't bother starting the  applet if it will just exit immediately
            if (STATE_EXIT != newState
            	&& STATE_INCOMINGCALL != pMe->m_eCurState
            	&& pMe->m_nStartCallType != START_INCOMING_CALL
            	&& pMe->m_nStartCallType != START_INCOMING_ANSWERED)
            {
                int ret;
                CALL_ERR("StartIncomingCall: %d,%d,%d",  newState,pMe->m_eCurState,pMe->m_nStartCallType);

                if (newState == STATE_ENDCALL)
                {
                    pMe->m_nStartCallType = START_END_CALL;
                }
                else if(newState == STATE_CALLING)/*add for dial form QXDM*/
                {
                    pMe->m_nStartCallType = START_WITH_ARGS;
                }
                else if (newState == STATE_INCOMINGCALL)
                {
                    pMe->m_nStartCallType = START_INCOMING_CALL;
                }
#ifdef FEATURE_OEMOMH
                else if(newState == STATE_NONOMH)
                {
                    pMe->m_nStartCallType = START_NONOMH;
                }
#endif
                //in some case ,if we press end key in incoming dialog,dialer app been stop,in AEECM_EVENT_CALL_END,we may set new state to STATE_MISSEDCALL,in this case ,we return
                else
                {
                    return;
                }
                //合上翻盖后直接回到IDLE，不再启动CALL
                if((pMe->IsStartCall == FALSE)&&(pMe->m_nStartCallType == START_END_CALL))
                {
                    pMe->IsStartCall = TRUE;
                    pMe->m_nStartCallType = START_MAX;
                    CALL_ERR("The phone cover close",0,0,0);
                    CallApp_Free_All_Call_Table(pMe);
                    (void)ISHELL_CloseApplet(pMe->m_pShell, TRUE);
                    return;
                }

                //MOVE_TO_STATE(newState)
                ret = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_DIALER/*AEECLSID_CALL*/);
                // EALREADY == already starting the applet.  The caller Id
                // event general comes in immediately after an incoming call
                // event and before the applet has had a chance to start itself
                // up.
                if ((SUCCESS != ret) && (EALREADY != ret))
                {
                    MOVE_TO_STATE(STATE_EXIT)
                }
            }
        }
        if(newState == STATE_EXIT)
        {
            CALL_ERR("The CallApp will be close,back to IDLE",0,0,0);
            CallApp_Free_All_Call_Table(pMe);
        }
    }

    else /*newState == STATE_NULL*/
    {
        if (pMe->m_nStartCallType == START_INCOMING_ANSWERED)  // from NIGps
        {
            int ret;

            CALL_ERR("@#@#NIGps answered incoming CALL->CONV",0,0,0);
            MOVE_TO_STATE(STATE_CONVERSATION)
            ret = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_DIALER/*AEECLSID_CALL*/);
            // EALREADY == already starting the applet.  The caller Id
            // event general comes in immediately after an incoming call
            // event and before the applet has had a chance to start itself
            // up.
            if ((SUCCESS != ret) && (EALREADY != ret))
            {
                MOVE_TO_STATE(STATE_EXIT);
            }
        }
        //CALL_ERR("newState IS STATE_NULL %d", pMe->m_b_from_another,0,0);
        //if(pMe->m_b_from_another)
        //{
        //    pMe->m_b_from_another = FALSE;
        //    MOVE_TO_STATE(STATE_EXIT)
        //}
    }
}

static void CallApp_ProcessCallStateOTAPA(CCallApp*pMe,AEECMNotifyInfo   *pCallInfo,
	                                        CallAppState             *newState)
{
    // OTAPA (by definition, MT) calls are transparent to the UI, so we
    // just tell IPHONE to answer the call, and otherwise pretend that
    // nothing happened. This way, the UI will still allow the user to
    // place an MO call, while an OTAPA call is in progress.

    CALL_FUN_START("%d ProcessCallStateOTAPA",pCallInfo->event,0,0);

    switch (pCallInfo->event)
    {
#ifdef FEATURE_ICM
        case AEECM_EVENT_CALL_INCOM:
            //CALL_ERR("AEECM_EVENT_CALL_INCOM",0,0,0);
            ICM_AnswerCall(pMe->m_pICM, pCallInfo->event_data.call.call_id);
            break;

        case AEECM_EVENT_CALL_END:
#else
        case AEET_EVENT_CALL_INCOM:
		{
			ICall 				 *pCall= NULL;
            //CALL_ERR("AEET_EVENT_CALL_INCOM",0,0,0);
            ICALLMGR_GetCall(pMe->m_pICallMgr,pCallInfo->event_data.call.cd,&pCall);
            if (pCall != NULL)
        	{
        		ICALL_Answer(pCall);
				ICALL_Release(pCall);
        	}
        }
            //ICM_AnswerCall(pMe->m_pITelephone, pCallInfo->event_data.call.cd);
            break;

        case AEET_EVENT_CALL_END:
#endif
            //CALL_ERR("AEECM_EVENT_CALL_END",0,0,0);
            // If the OTAPA call was ended because the user tried to
            // call a number, make the call now
            if (pMe->m_makeCallAfterOTAPA)
            {
                pMe->m_makeCallAfterOTAPA = FALSE;
                if (CALL_SUCESS != CallApp_MakeCall(pMe))
                {
#ifdef FEATURE_FDN
                    *newState = STATE_MO_NOT_ALLOWED;
#else
                    *newState = STATE_EXIT;
#endif /* FEATURE_FDN */
                }
            }
            break;

        default:
            break;
    }
}

static void CallApp_ProcessCallStateDATA(CCallApp                 *pMe,
	                                       AEECMNotifyInfo   *pCallInfo,
	                                       CallAppState             *newState)
{
    CALL_ERR("%d ProcessCallStateDATA",pCallInfo->event,0,0);
#ifdef Temp_remove
    // Ignore data calls unless OEM_GetShowCallDialogs is TRUE
    if (!OEM_GetShowCallDialogs())
#endif/*Temp_remove*/
    {
        switch (pCallInfo->event)
        {
#ifdef FEATURE_ICM
            case AEECM_EVENT_CALL_END:
#else
            case AEET_EVENT_CALL_END:
#endif
                // If the OTAPA call was ended because the user tried to
                // call a number, make the call now
                if (pMe->m_makeCallAfterOTAPA)
                {
                    pMe->m_makeCallAfterOTAPA = FALSE;
                    if (CALL_SUCESS != CallApp_MakeCall(pMe))
                    {
#ifdef FEATURE_FDN
                        *newState = STATE_MO_NOT_ALLOWED;
#else
                        *newState = STATE_EXIT;
#endif /* FEATURE_FDN */

                    }
                }
                break;
#ifdef FEATURE_OEMOMH
#ifdef FEATURE_ICM
           case AEECM_EVENT_CALL_ORIG:
#else
           case AEET_EVENT_CALL_ORIG:
#endif
#if defined(FEATURE_VERSION_W317A)||defined(FEATURE_VERSION_C337) ||defined(FEATURE_VERSION_C316)
#else
               ISHELL_SendEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_OMH_PROMPT,0,0);
#endif
#endif
            default:
                break;
        }
    }
}

static void CallApp_ProcessCallStateTest(CCallApp                 *pMe,
	                                       AEECMNotifyInfo   *pCallInfo,
	                                       CallAppState             *newState)
{
    // TO DO: Need to handle these calls just like voice calls
    //       FOR TESTING PURPOSES ONLY until their
    //       respective applets take over. We don't have to answer
    //       these calls in IIdle (the test tools answer the
    //       calls via AT commands), so all of the calls to
    //       IPHONE_AnswerCall() are still hardcoded to
    //       CM_CALL_TYPE_VOICE.

    // Ensure the caller id information from the
    // previous voice call is removed.
    CALL_ERR("%d ProcessCallStateTEST",pCallInfo->event,0,0);
    switch (pCallInfo->event)
    {
#ifdef FEATURE_ICM
        case AEECM_EVENT_CALL_CONNECT: /* Originated/incoming call was connected */
#else
        case AEET_EVENT_CALL_CONNECT: /* Originated/incoming call was connected */
#endif
            break;
#ifdef FEATURE_ICM
        case AEECM_EVENT_CALL_ORIG:    /* phone originated a call */
#else
        case AEET_EVENT_CALL_ORIG:    /* phone originated a call */
#endif
            //CALL_ERR("AEECM_EVENT_CALL_CONNECT",0,0,0);
            if(pMe->m_CallsTable)
            {
                pMe->m_CallsTable->call_name[0] = 0;
                pMe->m_CallsTable->call_number[0] = 0;
                pMe->m_CallsTable->type = PI_ALLOWED;
            }
            break;
#ifdef FEATURE_ICM
        case AEECM_EVENT_CALL_END:
#else
        case AEET_EVENT_CALL_END:
#endif
            //CALL_ERR("AEECM_EVENT_CALL_END",0,0,0);
            // If the OTAPA call was ended because the user tried to
            // call a number, make the call now
            if (pMe->m_makeCallAfterOTAPA)
            {
                pMe->m_makeCallAfterOTAPA = FALSE;
                if (CALL_SUCESS != CallApp_MakeCall(pMe))
                {
#ifdef FEATURE_FDN
                    *newState = STATE_MO_NOT_ALLOWED;
#else
                    *newState = STATE_EXIT;
#endif /* FEATURE_FDN */

                }
            }

            break;

        default:
           break;
    }
}

static void CallApp_ProcessCallStateVoiceOrig(CCallApp               *pMe,
	                                            AEECMCallEventData         *call_table,
	                                            CallAppState             *newState)/* phone originated a call */
{
    boolean b_energency = FALSE;
    boolean b_restict   = FALSE;
    Dialer_call_table * call_tb = NULL ;
#ifdef FEATURE_ICM
    call_tb = CallApp_Search_Number_In_Call_Table(pMe,call_table->call_info.other_party_no);
    if(NULL == call_tb)
    {
        b_energency = CallApp_IsEmergency_Number(call_table->call_info.other_party_no);
        if(!b_energency)
        {
            b_restict  = CallApp_IsRestictCallNumber_Ex(pMe, call_table->call_info.other_party_no, TRUE);
        }
    }
#else
	AECHAR  other_party_no[AEET_MAX_DIGITS_LENGTH]={0}; 	

	STRTOWSTR(call_table->call_info.other_party_no,other_party_no,AEET_MAX_DIGITS_LENGTH*sizeof(AECHAR));
    call_tb = CallApp_Search_Number_In_Call_Table(pMe,other_party_no);
    if(NULL == call_tb)
    {
        b_energency = CallApp_IsEmergency_Number(other_party_no);
        if(!b_energency)
        {
            b_restict  = CallApp_IsRestictCallNumber_Ex(pMe, other_party_no, TRUE);
        }
    }
#endif
    if(pMe->m_pIAnn)
    {
        IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_CALL/*ANNUN_FIELD_CALLFORWARD*/, ANNUN_STATE_CALL_INUSE_ON/*ANNUN_STATE_ON*/);
    }
    ISHELL_CancelTimer(pMe->m_pShell,
                                                        CallApp_HandleEndCallBlinkTimer, pMe);

    *newState = STATE_CALLING;
    //pMe->m_b_from_another = FALSE;
    //CallApp_Reset_CDG_Info(pMe);
    MEMSET((void*)&pMe->m_cdg_dsp_info , 0 ,sizeof(cdg_dsp_info));
    //if the call be dialed in another applet.we need add the number to calls_table 2007/09/14
    if(NULL == call_tb)
    {
#ifdef FEATURE_ICM
        CallApp_Add_Number_To_Call_Table(pMe,call_table->call_info.other_party_no,call_table->call_id,
                                                                            AEECALLHISTORY_CALL_TYPE_TO/*CALLHISTORY_OUTGOING*/,call_table->number.pi,
                                                                            FALSE,b_energency,b_restict);
#else
        CallApp_Add_Number_To_Call_Table(pMe,other_party_no,call_table->cd,
                                                                            AEECALLHISTORY_CALL_TYPE_TO/*CALLHISTORY_OUTGOING*/,call_table->number.pi,
                                                                            FALSE,b_energency,b_restict);
#endif
    }
    //if the call be dialed in another applet.we need add the number to calls_table 2007/09/14
#ifdef FEATURE_ICM
    CallApp_Change_Call_Table_Call_Start_Time(pMe,call_table->call_info.other_party_no);
#else
    CallApp_Change_Call_Table_Call_Start_Time(pMe,other_party_no);
#endif
    //CallApp_Change_Call_Table_Call_ID(pMe,call_table->call_info.other_party_no,call_table->call_id);
}

static void CallApp_ProcessCallStateVoiceAnswer(CCallApp               *pMe,
	                                              AEECMCallEventData         *call_table,
	                                              CallAppState             *newState)/* Incoming call was answered */
{
#ifndef FEATURE_ICM
	AECHAR  other_party_no[AEET_MAX_DIGITS_LENGTH]={0}; 	

	STRTOWSTR(call_table->call_info.other_party_no,other_party_no,AEET_MAX_DIGITS_LENGTH*sizeof(AECHAR));
#endif
    PARAM_NOT_REF (call_table)
    PARAM_NOT_REF (newState)

    IALERT_StopAlerting(pMe->m_pAlert);
#ifdef FEATURE_ICM
    CallApp_Change_Call_Table_Call_Start_Time(pMe,call_table->call_info.other_party_no);
    CallApp_Change_Call_Table_Call_History_State(pMe,call_table->call_info.other_party_no,
                                                                    AEECALLHISTORY_CALL_TYPE_FROM/*CALLHISTORY_INCOMING*/);
#else
    CallApp_Change_Call_Table_Call_Start_Time(pMe,other_party_no);
    CallApp_Change_Call_Table_Call_History_State(pMe,other_party_no,
                                                                    AEECALLHISTORY_CALL_TYPE_FROM/*CALLHISTORY_INCOMING*/);
#endif
}

static void CallApp_ProcessCallStateVoiceEnd(CCallApp               *pMe,
	                                           AEECMCallEventData         *call_table,
	                                           CallAppState             *newState)/* Originated/incoming call was ended */
{
#ifndef FEATURE_ICM
	AECHAR  other_party_no[AEET_MAX_DIGITS_LENGTH]={0}; 	

	STRTOWSTR(call_table->call_info.other_party_no,other_party_no,AEET_MAX_DIGITS_LENGTH*sizeof(AECHAR));
#endif
#ifdef  FEATURE_PERU_VERSION
    pMe->in_convert = FALSE; //to save another incoming call in conversation;
#endif
    IALERT_StopAlerting(pMe->m_pAlert);

    if(pMe->IsRestrictNumber)
    {
         pMe->IsRestrictNumber = FALSE;
         pMe->m_b_miss_notify = FALSE;
    }
    MEMSET(pMe->m_RedirNumStr,0,sizeof(pMe->m_RedirNumStr));
#ifdef FEATRUE_SET_IP_NUMBER
    pMe->m_b_ip_call[0] = 0;
#endif
    IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_CALL/*ANNUN_FIELD_CALLFORWARD*/, ANNUN_STATE_CALL_INUSE_OFF/*ANNUN_STATE_OFF*/);
#ifdef FEATURE_CARRIER_VENEZUELA_MOVILNET
    IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_CALL/*ANNUN_FIELD_MUTE*/, ANNUN_STATE_CALL_MUTE_OFF/*ANNUN_STATE_OFF*/);
#endif //FEATURE_CARRIER_VENEZUELA_MOVILNET
    ISHELL_CancelTimer(pMe->m_pShell, CallApp_NextTonePlay, pMe);
    FREEIF(pMe->m_toneData);
    // If the OTAPA call was ended because the user tried to
    // call a number, make the call now
    if (pMe->m_makeCallAfterOTAPA)//OTAPA Call,current we do not process
    {
        pMe->m_makeCallAfterOTAPA = FALSE;
        if (CALL_SUCESS != CallApp_MakeCall(pMe))
        {
#ifdef FEATURE_FDN
            *newState = STATE_MO_NOT_ALLOWED;
#else
            *newState = STATE_EXIT;
#endif /* FEATURE_FDN */

        }
    }
    if(pMe->m_bIsPrivacy )
    {
        pMe->m_bIsPrivacy = FALSE;
    }

    pMe->m_callEndStatus = call_table->end_status;
    pMe->m_callEndInOrig = FALSE;
	
#ifdef FEATURE_SUPPORT_BT_APP
    pMe->m_bt_audio = SEND_NONE;
    pMe->m_b_add_btag_menu = FALSE;
#endif

    /*CallWaiting 第三方先挂机,记录第三方为未接电话 */
    if (pMe->m_bmissCallWaiting)
    {
        pMe->m_bmissCallWaiting = FALSE;
        pMe->m_MissedCallCount++;
    }
    CALL_ERR("pMe->m_lastCallState =%d Canceled = %d callEndStatus = %d",pMe->m_lastCallState,pMe->m_userCanceled,pMe->m_callEndStatus);
    switch (pMe->m_lastCallState)
    {
#ifdef FEATURE_ICM
        case AEECM_CALL_STATE_CONV:
#else
        case AEET_CALL_STATE_CONV:
#endif
            *newState = STATE_ENDCALL;
            pMe->m_LastMinuteAlert = 0;
            pMe->m_CallVolume = 0;
            pMe->m_CallMuted = FALSE;
            CallApp_SetupCallAudio(pMe);
            CallApp_Change_Call_Table_All_Call_End_Time(pMe);//set all call end time for call histore
            break;
#ifdef FEATURE_ICM
        case AEECM_CALL_STATE_INCOM:
        case AEECM_CALL_STATE_IDLE:
#else
        case AEET_CALL_STATE_INCOM:
        case AEET_CALL_STATE_IDLE:
#endif
            if (pMe->m_userCanceled)
            {
                pMe->m_userCanceled = FALSE;
                *newState = STATE_EXIT;
                CallAppNotifyMP3PlayerAlertEvent(pMe,FALSE);
#ifdef FEATURE_ICM
                CallApp_Change_Call_Table_Call_History_State(pMe,
                                                        call_table->call_info.other_party_no,AEECALLHISTORY_CALL_TYPE_FROM);//CALLHISTORY_INCOMING);
                CallApp_Change_Call_Table_All_Call_End_Time(pMe);
                CallApp_Reset_Call_Table_Call_End_Time(pMe,call_table->call_info.other_party_no);
#else
                CallApp_Change_Call_Table_Call_History_State(pMe,
                                                        other_party_no,AEECALLHISTORY_CALL_TYPE_FROM);//CALLHISTORY_INCOMING);
                CallApp_Change_Call_Table_All_Call_End_Time(pMe);
                CallApp_Reset_Call_Table_Call_End_Time(pMe,other_party_no);
#endif
            }
            else
            {
                pMe->m_MissedCallCount++;
                *newState = STATE_MISSEDCALL;
            }
            break;
#ifdef FEATURE_ICM
        case AEECM_CALL_STATE_ORIG:
#else
        case AEET_CALL_STATE_ORIG:
#endif
            pMe->m_callEndInOrig = TRUE;
            if (pMe->m_userCanceled)
            {
                pMe->m_userCanceled = FALSE;
                *newState = STATE_EXIT;
                CallAppNotifyMP3PlayerAlertEvent(pMe,FALSE);
            }

            else
            {
                *newState = STATE_ENDCALL;
                // Call failed.  Add a call history entry with a zero
                // duration so the user can redial it later.
            }
#ifdef FEATURE_ICM
            CallApp_Reset_Call_Table_Call_End_Time(pMe,call_table->call_info.other_party_no);
#else
            CallApp_Reset_Call_Table_Call_End_Time(pMe,other_party_no);
#endif
            break;

        default:
            *newState = STATE_EXIT;
             break;
    }
    //pMe->m_b_call_barrings=FALSE;
    CallApp_Add_All_Call_History(pMe);
    //CFGI_AUTO_REDIAL
    CALL_ERR("CFGI_AUTO_REDIAL %d %d %d",pMe->m_auto_redial_count,call_table->call_info.direction,pMe->m_callEndStatus);
    if(pMe->m_auto_redial_count<MAXIMUM_TIMES_OF_REDIAL
#ifdef FEATURE_ICM
            && call_table->call_info.direction == AEECM_CALL_DIRECTION_MO)
#else
            && call_table->call_info.direction == AEET_CALL_DIRECTION_MO)
#endif
    {
        switch (pMe->m_callEndStatus)
        {
#ifdef FEATURE_ICM
            case AEECM_CALL_END_CD_GEN_OR_BUSY:
            case AEECM_CALL_END_NO_CDMA_SRV:
            case AEECM_CALL_END_NO_SRV:
#else
            case AEET_CALL_END_CD_GEN_OR_BUSY:
            case AEET_CALL_END_NO_CDMA_SRV:
            case AEET_CALL_END_NO_SRV:
#endif
            {
                if(pMe->m_CallsTable->b_emerg == FALSE)//Emerg call cann't allow to redail
                {
                    nv_auto_redial_type  b_auto_redial;
                    ICONFIG_GetItem(pMe->m_pConfig, CFGI_AUTO_REDIAL, (void*)&b_auto_redial, sizeof(nv_auto_redial_type));
                    CALL_ERR("CFGI_AUTO_REDIAL enable= %d rings= %d",b_auto_redial.enable,b_auto_redial.rings,0);
                    if(b_auto_redial.enable && b_auto_redial.rings != 0)
                    {
                        WSTRLCPY(pMe->m_DialString,pMe->m_call_info.call_number,AEECM_MAX_DIGITS_LENGTH);
                        ISHELL_SetTimer(pMe->m_pShell, b_auto_redial.rings *1000,(PFNNOTIFY)CallApp_MakeCall,(void *)pMe);
                        pMe->m_auto_redial_count ++;
                        pMe->m_b_auto_redial = TRUE;
                    }
                }
                break;
            }

            default:
                break;
        }
    }
    else
    {
        pMe->m_auto_redial_count = 0;
        pMe->m_b_auto_redial = FALSE;
    }
    switch (pMe->m_callEndStatus)
    {
#ifdef FEATURE_ICM
        case AEECM_CALL_END_INTERCEPT:/*cdg6.4*/
            IALERT_StartAlerting(pMe->m_pAlert, AEEALERT_CALLTYPE_VOICE, call_table->call_info.other_party_no, AEECM_CDMA_TONE_INTERCEPT);
            break;

        case AEECM_CALL_END_REORDER:/*cdg2.6*/
            IALERT_StartAlerting(pMe->m_pAlert, AEEALERT_CALLTYPE_VOICE, call_table->call_info.other_party_no, AEECM_CDMA_TONE_REORDER);
            break;
            
        case AEECM_CALL_END_ALERT_STOP:
            IALERT_StopAlerting(pMe->m_pAlert);
            break;
#else
        case AEET_CALL_END_INTERCEPT:/*cdg6.4*/
            IALERT_StartAlerting(pMe->m_pAlert, AEEALERT_CALLTYPE_VOICE, other_party_no, AEET_CDMA_TONE_INTERCEPT);
            break;

        case AEET_CALL_END_REORDER:/*cdg2.6*/
            IALERT_StartAlerting(pMe->m_pAlert, AEEALERT_CALLTYPE_VOICE, other_party_no, AEET_CDMA_TONE_REORDER);
            break;

        case AEET_CALL_END_ALERT_STOP:
            IALERT_StopAlerting(pMe->m_pAlert);
            break;
#endif
        //test only
        //case 1:/*cdg2.6*/
        //    IALERT_StartAlerting(pMe->m_pAlert, AEEALERT_CALLTYPE_VOICE, call_table->call_info.other_party_no, AEECM_CDMA_TONE_ABBR_INTERCEPT);
        //    break;

        //case 2:/*cdg2.6*/
        //    IALERT_StartAlerting(pMe->m_pAlert, AEEALERT_CALLTYPE_VOICE, call_table->call_info.other_party_no, AEECM_CDMA_TONE_ABBR_REORDER);
        //    break;

        default:
            break;
    }

    if(*newState == STATE_EXIT)
    {
        CALL_ERR("The Call App will be close,back to IDLE",0,0,0);
    }
}

static void CallApp_ProcessCallStateVoice_Incoming(CCallApp      *pMe,
	                                             AEECMCallEventData    *call_table,
	                                             CallAppState        *newState)/* phone received an incoming call */
{
    byte data = 0;
    boolean b_energency = FALSE;
    boolean b_restict   = FALSE;
#ifdef FEATURE_ICM
    if(pMe->m_pICM == NULL)
    {
        return;
    }
#else
	AEETCalls po;
	AECHAR  other_party_no[AEET_MAX_DIGITS_LENGTH]={0}; 	

	STRTOWSTR(call_table->call_info.other_party_no,other_party_no,AEET_MAX_DIGITS_LENGTH*sizeof(AECHAR));
	
    if(pMe->m_pITelephone == NULL)
    {
        return;
    }
#endif
#ifdef FEATRUE_SET_IP_NUMBER
    pMe->m_b_ip_call[0] = 0;
#endif
    
#ifdef FEATURE_ICM
    pMe->m_b_incall = AEECM_IS_VOICECALL_CONNECTED(pMe->m_pICM);
    if(CallApp_IsEmergencyMode(pMe->m_pICM)
                                    && pMe->m_b_incall)
    {
        ICM_EndCall(pMe->m_pICM, call_table->call_id);
        return;
    }

    b_energency = CallApp_IsEmergency_Number(call_table->call_info.other_party_no);
    if(!b_energency)
    {
        b_restict  = CallApp_IsRestictCallNumber_Ex(pMe, call_table->call_info.other_party_no, FALSE);
    }
#else
	if(SUCCESS != ITELEPHONE_GetCalls(pMe->m_pITelephone,											 
										&po, 
										sizeof(AEETCalls)))
	{
		return ;
	}
	pMe->m_b_incall = (po.dwCount>0)?TRUE:FALSE;
    if(CallApp_IsEmergencyMode(pMe->m_pITelephone)
                                    && pMe->m_b_incall)
    {
    	ICall *pCall= NULL;
		
     	ICALLMGR_GetCall(pMe->m_pICallMgr,call_table->cd,&pCall);
        if (pCall != NULL)
    	{
    		ICALL_Answer(pCall);
			ICALL_Release(pCall);
    	}
       // ICM_EndCall(pMe->m_pITelephone, call_table->cd);
        return;
    }

    b_energency = CallApp_IsEmergency_Number(other_party_no);
    if(!b_energency)
    {
        b_restict  = CallApp_IsRestictCallNumber_Ex(pMe, other_party_no, FALSE);
    }
#endif
    if (pMe->idle_info.uimLocked)
    {
#ifdef FEATURE_ICM
        // Ignore incoming calls while PIN locked
        ICM_EndCall(pMe->m_pICM, call_table->call_id);
#else
    	ICall *pCall= NULL;
		
     	ICALLMGR_GetCall(pMe->m_pICallMgr,call_table->cd,&pCall);
        if (pCall != NULL)
    	{
    		ICALL_Answer(pCall);
			ICALL_Release(pCall);
    	}
#endif
        return;
    }
    CALL_ERR("number.pi =%d %d %d",call_table->number.pi,pMe->m_b_incall,b_restict);
    (void) ICONFIG_GetItem(pMe->m_pConfig, CFGI_RESTRICT_INCOMING, &data, sizeof(byte));
    //不知道为什么要(data != OEMNV_RESTRICT_INCOMING_OUTCONTACT)这个条件
    if (b_restict  &&(data != OEMNV_RESTRICT_INCOMING_OUTCONTACT))
    {
        pMe->IsRestrictNumber = TRUE;
#ifdef FEATURE_ICM
        if(pMe->m_pICM != NULL)
        {
            ICM_EndCall(pMe->m_pICM, call_table->call_id);
            return;
        }
#else
        if(pMe->m_pITelephone != NULL)
        {
        	ICall *pCall= NULL;
			
	     	ICALLMGR_GetCall(pMe->m_pICallMgr,call_table->cd,&pCall);
	        if (pCall != NULL)
	    	{
	    		ICALL_End(pCall);
				ICALL_Release(pCall);
	    	}
           // ICM_EndCall(pMe->m_pITelephone, call_table->cd);
            return;
        }
#endif
    }
    //test ok,we show incoming dialog
    CALL_ERR("test ok,we show incoming dialog",0,0,0);
    //if(pMe->m_b_incall)
    //{
    //    CallApp_Reset_CDG_Info(pMe);
    //}
    MEMSET((void*)&pMe->m_cdg_dsp_info , 0 ,sizeof(cdg_dsp_info));
    ISHELL_CancelTimer(pMe->m_pShell,
                                                        CallApp_HandleEndCallBlinkTimer, pMe);
    ISHELL_CancelTimer(pMe->m_pShell,
                                                        (PFNNOTIFY)CallApp_MakeCall, pMe);
    pMe->m_b_auto_redial = FALSE;
    pMe->m_auto_redial_count = 0;
    ICONFIG_GetItem(pMe->m_pConfig, CFGI_ANYKEY_ANSWER, &pMe->m_anykey_answer, sizeof(byte));
    pMe->m_b_incoming = TRUE;
#if defined(FEATURE_VERSION_C337)||defined(FEATURE_VERSION_K202)   	
    pMe->m_isIncoming = TRUE;
#endif
#ifdef FEATURE_ICM
    CallApp_Add_Number_To_Call_Table(pMe,call_table->call_info.other_party_no,call_table->call_id,
#else
    CallApp_Add_Number_To_Call_Table(pMe,other_party_no,call_table->cd,
#endif
                                                                            AEECALLHISTORY_CALL_TYPE_MISSED/*CALLHISTORY_MISSED*/,call_table->number.pi,TRUE,b_energency,b_restict );
#if 0
    // start the ringer
    if(pMe->m_b_incall )
    {
        // play call waiting ring
        (void) IALERT_StartAlerting(pMe->m_pAlert, AEEALERT_CALLTYPE_VOICE,
                                                call_table->call_info.other_party_no,
                                                AEEALERT_ALERT_CALLWAITING);
    }
    else
    {
        // play normal ring
        (void) IALERT_StartAlerting(pMe->m_pAlert, AEEALERT_CALLTYPE_VOICE,
                                                call_table->call_info.other_party_no,
                                                AEEALERT_ALERT_NORMAL);
    }
#endif
    //不知道为什么要(data != OEMNV_RESTRICT_INCOMING_OUTCONTACT)这个条件
    if(data != OEMNV_RESTRICT_INCOMING_OUTCONTACT)
    {
        *newState = STATE_INCOMINGCALL;
    }
}

static void CallApp_ProcessCallStateVoiceCallerID(CCallApp          *pMe,
                                                        AEECMCallEventData    *call_table,
                                                        CallAppState        *newState)/* Caller ID info was received from BS */
{
    boolean b_energency = FALSE;
    boolean b_restict   = FALSE;
#ifdef FEATURE_ICM
    b_energency = CallApp_IsEmergency_Number(call_table->call_info.other_party_no);
    if(!b_energency)
    {
        b_restict  = CallApp_IsRestictCallNumber_Ex(pMe, call_table->call_info.other_party_no, FALSE);
    }
#else
	AECHAR  other_party_no[AEET_MAX_DIGITS_LENGTH]={0}; 	

	STRTOWSTR(call_table->call_info.other_party_no,other_party_no,AEET_MAX_DIGITS_LENGTH*sizeof(AECHAR));
	
    b_energency = CallApp_IsEmergency_Number(other_party_no);
    if(!b_energency)
    {
        b_restict  = CallApp_IsRestictCallNumber_Ex(pMe, other_party_no, FALSE);
    }
#endif
#ifdef  FEATURE_PERU_VERSION
    //to save another incoming call in conversation;
    if(pMe->in_convert)
    {
        if(pMe->m_CallsTable)
        {
            CallApp_Add_OneCall_To_History(pMe,
                                     pMe->m_CallsTable->call_number,
                                     pMe->m_CallsTable->type,
                                     pMe->m_CallsTable->call_name,
                                     AEECALLHISTORY_CALL_TYPE_FROM/*CALLHISTORY_MISSED*/,0);
        }
    }
#endif
    if (b_restict)//Restict Number
    {
#ifdef FEATURE_ICM
        pMe->m_userCanceled = TRUE;
        pMe->IsRestrictNumber = TRUE;
        //this code is used to decide the state of the phone, last state is idle, mean
        //it's first call.
        //if in conversation then the phone is in conversation,endcall will end all the link
        if (AEECM_CALL_STATE_CONV == pMe->m_lastCallState)
        {
            return;
        }

        ICM_EndCall(pMe->m_pICM, call_table->call_id);
#else
    	ICall *pCall= NULL;

        pMe->m_userCanceled = TRUE;
        pMe->IsRestrictNumber = TRUE;
        //this code is used to decide the state of the phone, last state is idle, mean
        //it's first call.
        //if in conversation then the phone is in conversation,endcall will end all the link
        if (AEET_CALL_STATE_CONV == pMe->m_lastCallState)
        {
            return;
        }
		
     	ICALLMGR_GetCall(pMe->m_pICallMgr,call_table->cd,&pCall);
        if (pCall != NULL)
    	{
    		ICALL_End(pCall);
			ICALL_Release(pCall);
    	}

#endif
        *newState = STATE_EXIT;
        MOVE_TO_STATE(STATE_EXIT)
    }
    else//not a Restict Number
    {
        if(pMe->m_pIAnn)
        {
            IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_CALL/*ANNUN_FIELD_CALLFORWARD*/, ANNUN_STATE_CALL_INUSE_ON/*ANNUN_STATE_ON*/);
        }
        
        if(pMe->m_b_incoming)
        {
#ifdef FEATURE_ICM
            CallApp_Modify_Number_To_Call_Table(pMe,call_table->call_info.other_party_no,call_table->call_id,
#else
            CallApp_Modify_Number_To_Call_Table(pMe,other_party_no,call_table->cd,
#endif
                                                                            AEECALLHISTORY_CALL_TYPE_MISSED/*CALLHISTORY_MISSED*/,call_table->number.pi);
            pMe->m_b_incoming = FALSE;
        }
        else
        {
#ifdef FEATURE_ICM
            CallApp_Add_Number_To_Call_Table(pMe,call_table->call_info.other_party_no,call_table->call_id,
#else
            CallApp_Add_Number_To_Call_Table(pMe,other_party_no,call_table->cd,
#endif
                                                                            AEECALLHISTORY_CALL_TYPE_MISSED/*CALLHISTORY_MISSED*/,call_table->number.pi,FALSE,b_energency,b_restict);
        }
        //CallApp_Change_Call_Table_Call_ID(pMe,call_table->call_info.other_party_no,call_table->call_id);
        switch (call_table->call_info.call_state)
        {
#ifdef FEATURE_ICM
            case AEECM_CALL_STATE_INCOM: // caller id of the incoming call
#else
            case AEET_CALL_STATE_INCOM: // caller id of the incoming call
#endif
                // We are probably already displaying the incoming call dialog,
                // but this will force it to refresh the call id information
                *newState = STATE_INCOMINGCALL;
                break;
#ifdef FEATURE_ICM
            case AEECM_CALL_STATE_CONV://waiting call
#else
            case AEET_CALL_STATE_CONV://waiting call
#endif
               // If a CallerID event occurs while in the conversation state,
               // assume it's an In-Band call
               //pMe->m_answerHold = FALSE;
               *newState = STATE_INCOMINGCALL;
                break;

            default:
                break;
        }
    }

}

static void CallApp_ProcessCallStateVoiceRedirNum(CCallApp                 *pMe,
                                                        AEECMCallEventData    *call_table,
                                                        CallAppState             *newState)
{
    CALL_ERR("%d %d %d CallApp_ProcessCallStateVoiceRedirNum",call_table->call_info.redirect_party_number.pi,pMe->m_lastCallState,call_table->number.pi);
    //pMe->m_b_show_cdg = TRUE;
    switch(pMe->m_cdg_dsp_info.pi/*call_table->call_info.redirect_party_number.pi*/)
    {
#ifdef FEATURE_ICM
        case AEECM_PI_RES: /* Presentation restricted */
#else
        case AEET_PI_RES: /* Presentation restricted */
#endif
            //for CDG test, CNAP with Forwarding
            pMe->m_RedirectingNumberPI = PI_RESTRICTED;
            (void) ISHELL_LoadResString(pMe->m_pShell,
                                                    AEE_APPSCALLAPP_RES_FILE,
                                                    IDS_NUMBER_RESTRICTED,
                                                    pMe->m_RedirNumStr,
                                                    sizeof(pMe->m_RedirNumStr));
            break;
#ifdef FEATURE_ICM
        case AEECM_PI_NOT_AVAIL: /* Number not available */
#else
        case AEET_PI_NOT_AVAIL: /* Number not available */
#endif
            //for CDG test, CNAP with Forwarding
            pMe->m_RedirectingNumberPI = PI_UNAVAILABLE;
            (void) ISHELL_LoadResString(pMe->m_pShell,
                                                    AEE_APPSCALLAPP_RES_FILE,
                                                    IDS_NUMBER_UNAVAILABLE,
                                                    pMe->m_RedirNumStr,
                                                    sizeof(pMe->m_RedirNumStr));
            break;
#ifdef FEATURE_ICM
        case AEECM_PI_ALLOW: /* Presentation allowed */
#else
        case AEET_PI_ALLOW: /* Presentation allowed */
#endif
        {
#ifndef FEATURE_ICM
			AECHAR redirect_party_number_buf[AEET_MAX_DIGITS_LENGTH]={0};

			STRTOWSTR(call_table->call_info.redirect_party_number.buf,redirect_party_number_buf,AEET_MAX_DIGITS_LENGTH*sizeof(AECHAR));
#endif
            //for CDG test, CNAP with Forwarding
            pMe->m_RedirectingNumberPI = PI_ALLOWED;
            MEMSET(pMe->m_RedirNumStr,0,sizeof(pMe->m_RedirNumStr));
#ifdef FEATURE_ICM
            CALL_PRINTF("VoiceRedirNum %S -%S",call_table->call_info.redirect_party_number.buf,call_table->call_info.forwarded_from);
            WSTRLCPY(pMe->m_RedirNumStr,call_table->call_info.redirect_party_number.buf,AEECM_MAX_DIGITS_LENGTH);
#else
            CALL_PRINTF("VoiceRedirNum %S -%S",redirect_party_number_buf,call_table->call_info.forwarded_from);
            WSTRLCPY(pMe->m_RedirNumStr,redirect_party_number_buf,AEET_MAX_DIGITS_LENGTH);
#endif
            CallApp_Conver_Buf(pMe->m_RedirNumStr);
            break;
        }

        default:
            ASSERT_NOT_REACHABLE;
    }
    pMe->m_cdg_dsp_info.m_b_show_cdg = TRUE;
    //for CDG test, CNAP with Forwarding
#ifdef FEATURE_ICM
    if((pMe->m_lastCallState == AEECM_CALL_STATE_ORIG)
        ||(pMe->m_lastCallState == AEECM_CALL_STATE_INCOM)
        ||(pMe->m_lastCallState == AEECM_CALL_STATE_CONV)
        )
#else
    if((pMe->m_lastCallState == AEET_CALL_STATE_ORIG)
        ||(pMe->m_lastCallState == AEET_CALL_STATE_INCOM)
        ||(pMe->m_lastCallState == AEET_CALL_STATE_CONV)
        )
#endif
    {
        //if(call_table->call_info.is_last_cdma_info_rec)
        if (ISHELL_GetActiveDialog(pMe->m_pShell) != NULL)
        {
            (void) ISHELL_PostEvent(pMe->m_pShell,
                                              AEECLSID_DIALER/*AEECLSID_CALL*/, EVT_USER_REDRAW, 0, 0);
        }
    }
}

static void CallApp_ProcessCallStateVoiceSignal(CCallApp                 *pMe,
                                                            AEECMCallEventData    *call_table,
                                                            CallAppState             *newState)
{
#ifndef FEATURE_ICM
	AECHAR  other_party_no[AEET_MAX_DIGITS_LENGTH]={0}; 	

	STRTOWSTR(call_table->call_info.other_party_no,other_party_no,AEET_MAX_DIGITS_LENGTH*sizeof(AECHAR));
#endif
    if (!call_table->signal.is_signal_info_avail)
    {
        CALL_ERR("Received AEECM_EVENT_CALL_SIGNAL with no signal info.",
                0, 0, 0);
        return;
    }

    //增加来电响铃的判断，在呼叫等待的条件下响铃
    //当不是需要响铃的时候,就设置状态为FALSE,
    //取消在这个地方响铃,统一到来电界面去
    pMe->incoming_id = FALSE;
#ifdef FEATURE_ICM
    if((call_table->signal.signal_type == AEECM_SIGNAL_CDMA_IS54B)
        &&(call_table->signal.signal.cdma_is54b== AEECM_CDMA_IS54B_L  )
        &&(call_table->signal.signal_pitch == AEECM_PITCH_MED)
        &&(call_table->call_info.call_type          != AEECM_CALL_TYPE_TEST)
        &&(call_table->call_info.call_state         == AEECM_CALL_STATE_INCOM)
      )
#else
    if((call_table->signal.signal_type == AEET_SIGNAL_CDMA_IS54B)
        &&(call_table->signal.signal.cdma_is54b== AEET_CDMA_IS54B_L  )
        &&(call_table->signal.signal_pitch == AEET_PITCH_MED)
        &&(call_table->call_info.call_type          != AEET_CALL_TYPE_TEST)
        &&(call_table->call_info.call_state         == AEET_CALL_STATE_INCOM)
      )
#endif
    {
         pMe->incoming_id = TRUE;
    }
#ifdef FEATURE_CDG2_TEST
    {
        int len = 0 ;
        len = WSTRLEN(call_table->call_info.alpha);
        if(len > 0)//cdg 2.5
        {
#ifdef FEATURE_ICM
            AECHAR  alpha[AEECM_MAX_ALPHA_TAG_LENGTH] = {0};
#else
            AECHAR  alpha[AEET_MAX_ALPHA_TAG_LENGTH] = {0};
#endif
            MEMCPY(alpha, call_table->call_info.alpha, len*sizeof(AECHAR));
            CallApp_Conver_Buf(alpha);
            if(WSTRLEN(alpha) >0 )
            {
                MEMCPY(pMe->m_cdg_dsp_info.alpha, alpha, len*sizeof(AECHAR));
                pMe->m_cdg_dsp_info.m_b_show_cdg = TRUE;
#ifdef FEATURE_ICM
                pMe->m_cdg_dsp_info.event = AEECM_EVENT_CALL_SIGNAL;
                pMe->m_cdg_dsp_info.disp_type = AEECM_DISP_NORMAL;
                pMe->m_cdg_dsp_info.b_last_rec = call_table->call_info.is_last_cdma_info_rec;
#else
                pMe->m_cdg_dsp_info.event = AEET_EVENT_CALL_SIGNAL;
                pMe->m_cdg_dsp_info.disp_type = AEET_DISP_NORMAL;
           //     pMe->m_cdg_dsp_info.b_last_rec = call_table->call_info.is_last_cdma_info_rec;  remove by aming
#endif
                pMe->m_cdg_dsp_info.pi = PI_ALLOWED;
                ISHELL_PostEvent(pMe->m_pShell, AEECLSID_DIALER, EVT_USER_REDRAW,  0,0);
            }
        }
    }
#endif
    if(call_table->signal.is_signal_info_avail == TRUE)
    {
#ifdef FEATURE_ICM
        (void) CallApp_PlaySignal(pMe, call_table->call_info.other_party_no,
#else
        (void) CallApp_PlaySignal(pMe, other_party_no,
#endif
                                     &(call_table->signal));
    }
}

static void CallApp_ProcessCallStateVoiceConnect(CCallApp                 *pMe,
	                                               AEECMCallEventData    *call_table,
	                                               CallAppState             *newState)
{
#ifndef FEATURE_ICM
	AECHAR  other_party_no[AEET_MAX_DIGITS_LENGTH]={0}; 	

	STRTOWSTR(call_table->call_info.other_party_no,other_party_no,AEET_MAX_DIGITS_LENGTH*sizeof(AECHAR));
#endif
    *newState = STATE_CONVERSATION;
    // 如果不置零，会造成在正常通话中也出现pMe->m_Is3Way==TRUE的情况。
    pMe->m_Is3Way = FALSE;
    IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_CALL/*ANNUN_FIELD_CALLFORWARD*/, ANNUN_STATE_CALL_INUSE_ON/*ANNUN_STATE_ON*/);
    pMe->m_bIsPrivacy |= call_table->call_info.is_privacy;
	
	//Add By zzg 2012_10_30
	#ifndef FEATURE_VERSION_W317A
    IANNUNCIATOR_SetField(pMe->m_pIAnn, ANNUN_FIELD_LOCKSTATUS/*ANNUN_FIELD_VOICE_PRIVACY*/,
                            (pMe->m_bIsPrivacy? ANNUN_STATE_VOICE_PRIV_ON/*ANNUN_STATE_ON*/ : ANNUN_STATE_VOICE_PRIV_OFF/*ANNUN_STATE_OFF*/));
	#endif
	//Add End
	
#ifdef FEATURE_ICM
    CallApp_Change_Call_Table_Call_Start_Time(pMe,call_table->call_info.other_party_no);
#else
    CallApp_Change_Call_Table_Call_Start_Time(pMe,other_party_no);
#endif

    pMe->m_LastMinuteAlert = 0;
    // Get current NV volume level
    (void) ICONFIG_GetItem(pMe->m_pConfig,CFGI_EAR_VOL,
                                            &pMe->m_CallVolume,sizeof(pMe->m_CallVolume));
    pMe->m_CallMuted = FALSE;
	
#ifdef FEATURE_SUPPORT_BT_APP
    if(pMe->m_bt_audio == SEND_BT_CALL)
    {
        pMe->m_bt_audio = SEND_BT_AUDIO;
    }
#endif

    MSG_FATAL("***zzg CallApp_ProcessCallStateVoiceConnect***", 0, 0, 0);

    CallApp_SetupCallAudio(pMe);
    MEMSET((void*)&pMe->m_cdg_dsp_info , 0 ,sizeof(cdg_dsp_info));
}

static void CallApp_ProcessCallStateVoiceAccept(CCallApp                 *pMe,
	                                              AEECMCallEventData    *call_table,
	                                              CallAppState             *newState)
{
#ifndef FEATURE_ICM
	AECHAR  other_party_no[AEET_MAX_DIGITS_LENGTH]={0}; 	

	STRTOWSTR(call_table->call_info.other_party_no,other_party_no,AEET_MAX_DIGITS_LENGTH*sizeof(AECHAR));
#endif
    *newState = STATE_CONVERSATION;
    CALL_ERR("OK, originated call accepted!", 0, 0, 0);

    //Reset call time when receive CM_CALL_EVENT_ORG_ACCEPT.
#ifdef FEATURE_ICM
    CallApp_Change_Call_Table_Call_Start_Time(pMe,call_table->call_info.other_party_no);
#else
    CallApp_Change_Call_Table_Call_Start_Time(pMe,other_party_no);
#endif
}

//add for CDG test, CNAP with Forwarding
/*
** Dispalying the string in Display Record or Extended Display Record
** or Called Party Number Record from Forward Channel message.
*/
static void CallApp_ProcessCallStateVoiceDisplay(CCallApp *pMe,
	AEECMNotifyInfo  *pCallInfo, CallAppState *newState)
{
#ifdef FEATURE_ICM
    AECHAR   tmpBuf[AEECM_MAX_ALPHA_TAG_LENGTH + 1] = {'\0'};
    uint32 len = 0;
    AEECLSID id = 0;
    CALL_ERR(" %x lastCallState= %d  info_rec= %d CallApp_ProcessCallStateVoiceDisplay",pCallInfo->event,pMe->m_lastCallState,pCallInfo->event_data.call.call_info.is_last_cdma_info_rec);
#else
    AECHAR   tmpBuf[AEET_MAX_ALPHA_TAG_LENGTH + 1] = {'\0'};
    uint32 len = 0;
    AEECLSID id = 0;
	AECHAR  other_party_no[AEET_MAX_DIGITS_LENGTH]={0}; 	

	STRTOWSTR(pCallInfo->event_data.call.call_info.other_party_no,other_party_no,AEET_MAX_DIGITS_LENGTH*sizeof(AECHAR));
	
    //CALL_ERR(" %x lastCallState= %d  info_rec= %d CallApp_ProcessCallStateVoiceDisplay",pCallInfo->event,pMe->m_lastCallState,pCallInfo->event_data.call.call_info.is_last_cdma_info_rec);
    //pMe->m_b_show_cdg = TRUE;
#endif
    //pMe->m_b_show_cdg = TRUE;
    CALL_ERR("call_info.alpha %x %x %x",pCallInfo->event_data.call.call_info.alpha[0],pCallInfo->event_data.call.call_info.alpha[1],pCallInfo->event_data.call.call_info.alpha[2]);
    CALL_PRINTF("call_info.alpha %S",pCallInfo->event_data.call.call_info.alpha);
    //CALL_PRINTF("party_subaddr %S",pCallInfo->event_data.call.call_info.other_party_subaddr);
    //CALL_PRINTF("other_party_no %S",pCallInfo->event_data.call.call_info.other_party_no);
    //CALL_PRINTF("call.number.buf %S",pCallInfo->event_data.call.number.buf);
    // Get the string from Display or Extended Display record
#ifdef FEATURE_ICM
    if(pCallInfo->event == AEECM_EVENT_CALL_DISPLAY)//||
    //(pCallInfo->event == AEECM_EVENT_INTERNAL_EXT_DISP))
#else
    if(pCallInfo->event == AEET_EVENT_CALL_DISPLAY)//||
    //(pCallInfo->event == AEET_EVENT_INTERNAL_EXT_DISP))
#endif
    {
        CALL_ERR("%d pCallInfo->event_data.call.disp_type",pCallInfo->event_data.call.disp_type,0,0);
        /*OEMCM.c 5409     AEECM_EVENT_CALL_DISPLAY
        notify_info->event_data.call.disp_type = AEECM_DISP_NORMAL;*/
        len = WSTRLEN(pCallInfo->event_data.call.call_info.alpha);
        if(len > 0)
        {
#ifdef FEATURE_ICM
            if(len > AEECM_MAX_ALPHA_TAG_LENGTH)
            {
                len = AEECM_MAX_ALPHA_TAG_LENGTH;
            }
#else
            if(len > AEET_MAX_ALPHA_TAG_LENGTH)
            {
                len = AEET_MAX_ALPHA_TAG_LENGTH;
            }
#endif
            MEMCPY(tmpBuf, pCallInfo->event_data.call.call_info.alpha, len*sizeof(AECHAR));
        }
    }
#ifdef FEATURE_ICM
    else if(pCallInfo->event == AEECM_EVENT_CALL_CALLED_PARTY)
#else
    else if(pCallInfo->event == AEET_EVENT_CALL_CALLED_PARTY)
#endif
    // Get the string from Called Party Number record    
    {
        //当前还不能确定传上来的信息在哪个里面
        //From cmxcall.c 8611,call party number be saved in num->buf
        //From oemcm.c 5358,call party number be saved in call_info.other_party_no
#ifdef FEATURE_ICM
        len = WSTRLEN(pCallInfo->event_data.call.call_info.other_party_no);
        if(len > 0)
        {
            if(len > AEECM_MAX_SUBADDRESS_DIGITS_LENGTH)
            {
                len = AEECM_MAX_SUBADDRESS_DIGITS_LENGTH;
            }
            MEMCPY(tmpBuf, pCallInfo->event_data.call.call_info.other_party_no, len*sizeof(AECHAR));
        }
#else
        len = WSTRLEN(other_party_no);
        if(len > 0)
        {
            if(len > AEET_MAX_SUBADDRESS_DIGITS_LENGTH)
            {
                len = AEET_MAX_SUBADDRESS_DIGITS_LENGTH;
            }
            MEMCPY(tmpBuf, other_party_no, len*sizeof(AECHAR));
        }
#endif
    }
    CALL_PRINTF("pMe->m_RedirNumStr %S",pMe->m_RedirNumStr);
#ifdef FEATURE_ICM
    if(WSTRLEN(pMe->m_RedirNumStr)>0 && pMe->m_cdg_dsp_info.pi == AEECM_PI_ALLOW) //in cdg 9.6.3 ,we need show ABC- 01628-681690
    {
        AECHAR mm[AEECM_MAX_ALPHA_TAG_LENGTH] = {0};
        CallApp_Conver_Buf(tmpBuf);
        WSTRLCPY(mm, tmpBuf, AEECM_MAX_ALPHA_TAG_LENGTH); 
        WSTRLCAT(mm,L"-",AEECM_MAX_ALPHA_TAG_LENGTH);
        WSTRLCAT(mm, pMe->m_RedirNumStr, AEECM_MAX_ALPHA_TAG_LENGTH);
        WSTRLCPY(pMe->m_RedirNumStr,mm,AEECM_MAX_ALPHA_TAG_LENGTH);
    }
    else
    {
        WSTRLCPY(pMe->m_cdg_dsp_info.alpha, tmpBuf, AEECM_MAX_ALPHA_TAG_LENGTH);
    }
#else
    if(WSTRLEN(pMe->m_RedirNumStr)>0 && pMe->m_cdg_dsp_info.pi == AEET_PI_ALLOW) //in cdg 9.6.3 ,we need show ABC- 01628-681690
    {
        AECHAR mm[AEET_MAX_ALPHA_TAG_LENGTH] = {0};
        CallApp_Conver_Buf(tmpBuf);
        WSTRLCPY(mm, tmpBuf, AEET_MAX_ALPHA_TAG_LENGTH); 
        WSTRLCAT(mm,L"-",AEET_MAX_ALPHA_TAG_LENGTH);
        WSTRLCAT(mm, pMe->m_RedirNumStr, AEET_MAX_ALPHA_TAG_LENGTH);
        WSTRLCPY(pMe->m_RedirNumStr,mm,AEET_MAX_ALPHA_TAG_LENGTH);
    }
    else
    {
        WSTRLCPY(pMe->m_cdg_dsp_info.alpha, tmpBuf, AEET_MAX_ALPHA_TAG_LENGTH);
    }
#endif
    CALL_PRINTF("pMe->m_cdg_dsp_info.alpha %S",pMe->m_cdg_dsp_info.alpha);
    if(WSTRLEN(pMe->m_cdg_dsp_info.alpha) == 0)
    {
        return;
    }
    else //the current we receive 0x8d 0x0e Default CNA #1
    {
        CallApp_Conver_Buf(pMe->m_cdg_dsp_info.alpha);
    }
    pMe->m_cdg_dsp_info.m_b_show_cdg = TRUE;
    pMe->m_cdg_dsp_info.event = pCallInfo->event;
    pMe->m_cdg_dsp_info.disp_type = pCallInfo->event_data.call.disp_type;
#ifdef FEATURE_ICM
    pMe->m_cdg_dsp_info.b_last_rec = pCallInfo->event_data.call.call_info.is_last_cdma_info_rec;
#else
    //pMe->m_cdg_dsp_info.b_last_rec = pCallInfo->event_data.call.call_info.is_last_cdma_info_rec;  remove by aming
#endif
    pMe->m_cdg_dsp_info.pi = pMe->m_b_is_PI_ALLOWED/*pCallInfo->event_data.call.number.pi*/;
    id = AEE_Active();
    if(id == AEECLSID_CORE_APP)
    {
        ISHELL_SendEvent(pMe->m_pShell, AEECLSID_CORE_APP,EVT_NEWMSGIN,  0,(uint32)(pMe->m_cdg_dsp_info.alpha));
        return;
    }
    else if(id == AEECLSID_DIALER)
    {
        IDialog  *pIActiveDialog = ISHELL_GetActiveDialog(pMe->m_pShell);
        if (pIActiveDialog != NULL)
        {
            uint16 dilog_id = 0;
            dilog_id = IDIALOG_GetID(pIActiveDialog);
            if(dilog_id == IDD_CALLING || dilog_id == IDD_CONVERSATION || dilog_id == IDD_INCOMINGCALL)
            {
                ISHELL_PostEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_USER_REDRAW,0,0);
            }
        }
    }
    else
    {
    }
}

static void CallApp_ProcessCallStateVoice(CCallApp *pMe,
        AEECMNotifyInfo  *pCallInfo, CallAppState *newState)
{
    // NOTE:
    // When a 'Feature Notification Message' containing a Display record is
    // sent to the mobile while in the Idle state, CM will indicate the call
    // type as CM_CALL_TYPE_NONE (even though there is a cautionary
    // 'FOR INTERNAL USE OF CM ONLY!' comment in cm.h where CM_CALL_TYPE_NONE
    // is declared...).
    //
    // In conclusion, we only care about CM display events while in the
    // idle state so ignore everything else.
    //
#ifdef FEATURE_ICM
    AEECMCallEventData    *call_table = NULL;
#else
    AEETCallEventData    *call_table = NULL;
#endif
    CALL_FUN_START("CallApp_ProcessCallStateVoice",0,0,0);
    if(pCallInfo == NULL)
    {
        return;
    }

    call_table = &(pCallInfo->event_data.call);

    if(call_table == NULL)
    {
        return;
    }
#ifdef FEATURE_ICM
    if ( (AEECM_CALL_TYPE_NONE == call_table->call_info.call_type) &&
           //(pCallInfo->event != AEECM_EVENT_INTERNAL_EXT_DISP) &&     //add for CR330: CDG2 9.7.1
           (pCallInfo->event != AEECM_EVENT_CALL_DISPLAY)&&
           (pCallInfo->event != AEECM_EVENT_CALL_SIGNAL)) //add for CDG2 9.1.2.1
    {
        CALL_ERR("%d AEECM_CALL_TYPE_NONE",pCallInfo->event, 0, 0);
        return;
    }
#else
    if ( (AEET_CALL_TYPE_NONE == call_table->call_info.call_type) &&
           //(pCallInfo->event != AEET_EVENT_INTERNAL_EXT_DISP) &&     //add for CR330: CDG2 9.7.1
           (pCallInfo->event != AEET_EVENT_CALL_DISPLAY)&&
           (pCallInfo->event != AEET_EVENT_CALL_SIGNAL)) //add for CDG2 9.1.2.1
    {
        CALL_ERR("%d AEET_CALL_TYPE_NONE",pCallInfo->event, 0, 0);
        return;
    }
#endif
    switch (pCallInfo->event)
    {
#ifdef FEATURE_ICM
        case AEECM_EVENT_CALL_ORIG:       /* phone originated a call */
#else
        case AEET_EVENT_CALL_ORIG:       /* phone originated a call */
#endif
            CALL_ERR("AEECM_EVENT_CALL_ORIG=+=",0,0,0);
            CallApp_ProcessCallStateVoiceOrig(pMe, call_table, newState);
#ifdef CDG_TEMP_TEST
            CallApp_ProcessCallStateVoiceDisplay(pMe,pCallInfo,newState);
#endif
            break;
#ifdef FEATURE_ICM
        case AEECM_EVENT_CALL_ANSWER:     /* Incoming call was answered */
#else
        case AEET_EVENT_CALL_ANSWER:     /* Incoming call was answered */
#endif
            CALL_ERR("%d AEECM_EVENT_CALL_ANSWER=+=",call_table->number.pi,0,0);
            CallApp_ProcessCallStateVoiceAnswer(pMe, call_table, newState);
#ifdef FEATURE_LED_CONTROL
            //CallApp_DisableLedSig( pMe);
#endif
            break;
#ifdef FEATURE_ICM
        case AEECM_EVENT_CALL_END:        /* Originated/incoming call was ended */
#else
        case AEET_EVENT_CALL_END:        /* Originated/incoming call was ended */
#endif
            CALL_ERR("AEECM_EVENT_CALL_END=+=",0,0,0);
            pMe->m_bReceiveCallerID     = FALSE;
            CallApp_ProcessCallStateVoiceEnd(pMe, call_table, newState);
#ifdef FEATURE_LED_CONTROL
            //CallApp_DisableLedSig( pMe);
#endif
            break;
#ifdef FEATURE_ICM
        case AEECM_EVENT_CALL_INCOM:      /* phone received an incoming call */
#else
        case AEET_EVENT_CALL_INCOM:      /* phone received an incoming call */
#endif
            CALL_ERR("%d AEECM_EVENT_CALL_INCOM=+=",call_table->number.pi,0,0);
            pMe->incoming_id=TRUE;  // 初始需要播放铃声
            CallApp_ProcessCallStateVoice_Incoming(pMe, call_table, newState);
            break;
#ifdef FEATURE_ICM
        case AEECM_EVENT_CALL_CALLER_ID:  /* Caller ID info was received from BS */
#else
        case AEET_EVENT_CALL_CALLER_ID:  /* Caller ID info was received from BS */
#endif
            CALL_ERR("%d AEECM_EVENT_CALL_CALLER_ID=+=",call_table->number.pi,0,0);
            pMe->m_bReceiveCallerID = TRUE;
            CallApp_ProcessCallStateVoiceCallerID(pMe, call_table, newState);
#ifdef CDG_TEMP_TEST
            CallApp_ProcessCallStateVoiceDisplay(pMe,pCallInfo,newState);
#endif
            break;
#ifdef FEATURE_ICM
        case AEECM_EVENT_CALL_FORWARDED://we not use the pMe->m_RedirNumStr
#else
        case AEET_EVENT_CALL_FORWARDED://we not use the pMe->m_RedirNumStr
#endif
        //for CDG test, CNAP with Forwarding
#ifdef FEATURE_ICM
#ifdef T_MSM6000
        case AEECM_EVENT_CALL_EXIT_TC:
#endif
#ifdef FEATURE_REDIRECTING_NUMBER_INFO_REC
        case AEECM_EVENT_CALL_REDIR_NUM:// Redirection number info was received
#endif
#else
#ifdef T_MSM6000
        case AEET_EVENT_CALL_EXIT_TC:
#endif
#ifdef FEATURE_REDIRECTING_NUMBER_INFO_REC
        case AEET_EVENT_CALL_REDIR_NUM:// Redirection number info was received
#endif
#endif
            CALL_ERR("%d %d AEECM_EVENT_CALL_REDIR_NUM=+=",call_table->number.pi,call_table->call_info.redirect_party_number.pi,0);
            CallApp_ProcessCallStateVoiceRedirNum(pMe, call_table, newState);
#ifdef FEATURE_LED_CONTROL
            //CallApp_DisableLedSig( pMe);
#endif
            break;
#ifdef FEATURE_ICM
        case AEECM_EVENT_CALL_SIGNAL:       /* signal info was received from BS */
#else
        case AEET_EVENT_CALL_SIGNAL:       /* signal info was received from BS */
#endif
            CALL_ERR("%d %d AEECM_EVENT_CALL_SIGNAL=+=",pMe->m_bReceiveCallerID,call_table->number.pi,0);
            if (pMe->m_bReceiveCallerID)
            {
                pMe->m_bReceiveCallerID = FALSE;
                return ;
            }
            CallApp_ProcessCallStateVoiceSignal(pMe, call_table, newState);
            break;
#ifdef FEATURE_ICM
        case AEECM_EVENT_CALL_CONNECT: /* Originated/incoming call was connected */
#else
        case AEET_EVENT_CALL_CONNECT: /* Originated/incoming call was connected */
#endif
            CALL_ERR("%d AEECM_EVENT_CALL_CONNECT=+=",call_table->number.pi,0,0);
            pMe->m_bReceiveCallerID     = FALSE;
            
            CallApp_ProcessCallStateVoiceConnect(pMe, call_table, newState);
#ifdef CDG_TEMP_TEST
            CallApp_ProcessCallStateVoiceDisplay(pMe,pCallInfo,newState);
#endif
            break;

#ifdef FEATURE_ICM
        case AEECM_EVENT_CALL_LINE_CTRL: /* Originated was accepted */
            CALL_ERR("%d AEECM_EVENT_CALL_LINE_CTRL=+=",call_table->number.pi,0,0);
            CallApp_ProcessCallStateVoiceAccept(pMe, call_table, newState);
      
            break;
#endif
#ifdef FEATURE_ICM
        case AEECM_EVENT_CALL_OPS:      /* phone sent Flash/Flash with Info to BS */
#else
        case AEET_EVENT_CALL_OPS:      /* phone sent Flash/Flash with Info to BS */
#endif
            CALL_ERR("AEECM_EVENT_CALL_OPS=+=",0,0,0);
#ifdef FEATURE_ICM
            if (AEECM_CALL_STATE_CONV == call_table->call_info.call_state)
#else
            if (AEET_CALL_STATE_CONV == call_table->call_info.call_state)
#endif
            {
                *newState = STATE_CONVERSATION;
            }
            break;
#ifdef FEATURE_ICM
        case AEECM_EVENT_CALL_PRIVACY:  /* Privacy mode changed while in a call */
        case AEECM_EVENT_CALL_SRV_OPT: /* Service option changed */
#else
        case AEET_EVENT_CALL_PRIVACY:  /* Privacy mode changed while in a call */
        case AEET_EVENT_CALL_SRV_OPT: /* Service option changed */
#endif
            CALL_ERR("%d AEECM_EVENT_CALL_PRIVACY=+=",call_table->call_info.is_privacy,0,0);
            // Update the privacy annunciator
            pMe->m_bIsPrivacy |= call_table->call_info.is_privacy;

			//Add By zzg 2012_10_30
			#ifndef FEATURE_VERSION_W317A
            IANNUNCIATOR_SetField(pMe->m_pIAnn, ANNUN_FIELD_LOCKSTATUS/*ANNUN_FIELD_VOICE_PRIVACY*/,
                                                    (pMe->m_bIsPrivacy ? ANNUN_STATE_VOICE_PRIV_ON/*ANNUN_STATE_ON*/ : ANNUN_STATE_VOICE_PRIV_OFF/*ANNUN_STATE_OFF*/));
			#endif
			//Add End
			
			ISHELL_PostEvent( pMe->m_pShell,AEECLSID_DIALER,EVT_USER_REDRAW, 0,0);
            break;

           /* Events added forcase CM 2.0 */
#ifdef FEATURE_ICM
        case AEECM_EVENT_CALL_ABRV_ALERT:   /* CDMA/AMPS abbreviated alert */
#else
        case AEET_EVENT_CALL_ABRV_ALERT:   /* CDMA/AMPS abbreviated alert */
#endif
            CALL_ERR("AEECM_EVENT_CALL_ABRV_ALERT=+=",0,0,0);
            //OEM_Snd_Sound_Id_Start(SND_DEVICE_CURRENT, SND_METHOD_VOICE,
            //  SND_ABRV_ALERT, SND_PRIO_MED, SND_APATH_LOCAL, NULL);
#ifndef WIN32
            snd_sound_id_start(SND_DEVICE_CURRENT, SND_METHOD_VOICE,
                SND_ABRV_ALERT, SND_PRIO_MED, SND_APATH_LOCAL, NULL,NULL);
#endif
            break;
#ifdef FEATURE_ICM
        case AEECM_EVENT_CALL_ABRV_REORDER: /* AMPS abbreviated reorder */
#else
        case AEET_EVENT_CALL_ABRV_REORDER: /* AMPS abbreviated reorder */
#endif
            CALL_ERR("AEECM_EVENT_CALL_ABRV_REORDER=+=",0,0,0);
            //OEM_Snd_Sound_Id_Start(SND_DEVICE_CURRENT, SND_METHOD_VOICE,
            //    SND_ABRV_REORDER, SND_PRIO_MED, SND_APATH_LOCAL, NULL);
#ifndef WIN32
            snd_sound_id_start(SND_DEVICE_CURRENT, SND_METHOD_VOICE,
                SND_ABRV_REORDER, SND_PRIO_MED, SND_APATH_LOCAL, NULL,NULL);
#endif
            break;
#ifdef FEATURE_ICM
        case AEECM_EVENT_CALL_ABRV_INTERCEPT: /* AMPS abbreviated intercept */
#else
        case AEET_EVENT_CALL_ABRV_INTERCEPT: /* AMPS abbreviated intercept */
#endif
            CALL_ERR("AEECM_EVENT_CALL_ABRV_INTERCEPT=+=",0,0,0);
            //OEM_Snd_Sound_Id_Start(SND_DEVICE_CURRENT, SND_METHOD_VOICE,
            //     SND_ABRV_INTERCEPT, SND_PRIO_MED, SND_APATH_LOCAL, NULL);
#ifndef WIN32
            snd_sound_id_start(SND_DEVICE_CURRENT, SND_METHOD_VOICE,
                 SND_ABRV_INTERCEPT, SND_PRIO_MED, SND_APATH_LOCAL, NULL,NULL);
#endif
            break;

        //add for CDG test, CNAP with Forwarding
#ifdef FEATURE_ICM
        case AEECM_EVENT_CALL_DISPLAY:      /* display info was received from BS */
#else
        case AEET_EVENT_CALL_DISPLAY:      /* display info was received from BS */
#endif
            //#ifdef FEATURE_IS95B_EXT_DISP
            //        case AEECM_EVENT_INTERNAL_EXT_DISP:  /* extended display was received from BS */
            //#endif /* FEATURE_IS95B_EXT_DISP */
            /*From OEMCM.c 5456,we cann't receive AEECM_EVENT_INTERNAL_EXT_DISP
                when OEMCM receive AEECM_EVENT_INTERNAL_EXT_DISP,it send AEECM_EVENT_CALL_DISPLAY
            */
            CALL_ERR("AEECM_EVENT_CALL_DISPLAY=+=",0,0,0);
            //CallApp_ProcessCallStateVoiceDisplay(pMe, pCallInfo, newState);
            //break;
#ifdef FEATURE_ICM
        case AEECM_EVENT_CALL_CALLED_PARTY:   /* called party info was received from BS */
#else
        case AEET_EVENT_CALL_CALLED_PARTY:   /* called party info was received from BS */
#endif
            CALL_ERR("%d AEECM_EVENT_CALL_CALLED_PARTY=+=", call_table->number.pi, 0, 0);
            CallApp_ProcessCallStateVoiceDisplay(pMe, pCallInfo, newState);
            break;
#ifdef FEATURE_ICM
        case AEECM_EVENT_CALL_CONNECTED_NUM:  /* connected number info was received from BS */
#else
        case AEET_EVENT_CALL_CONNECTED_NUM:  /* connected number info was received from BS */
#endif
            CALL_ERR("%d && %d %d AEECM_EVENT_CALL_CONNECTED_NUM=+=",
                                            call_table->number.number_type, pMe->m_lastCallState, call_table->number.pi);
#if 1
#ifdef FEATURE_ICM
            if(call_table->number.number_type == AEECM_TON_INTERNATIONAL)
            {
                /*in this time,CM will inset a "+" in call_info->other_party_no OEMCM.c 5903*/
                Dialer_call_table * temp = NULL;
                AECHAR new_number[AEECM_MAX_DIGITS_LENGTH] = {0};

                WSTRLCPY(new_number,call_table->call_info.other_party_no + 1,AEECM_MAX_DIGITS_LENGTH);
                CALL_PRINTF("new_number = %S %S",new_number,call_table->call_info.other_party_no);
                //CALL_PRINT("other_party_no = ",call_table->call_info.other_party_no,0);
                temp = CallApp_Search_Number_In_Call_Table(pMe,new_number);
                if (temp)
                {
                    temp->call_number[0] = 0;
                    WSTRLCPY(temp->call_number,call_table->call_info.other_party_no ,AEECM_MAX_DIGITS_LENGTH);
                }

                if(pMe->m_lastCallState == AEECM_CALL_STATE_CONV)/*||
                     (pMe->m_lastCallState == AEECM_CALL_STATE_INCOM) ||
                        (pMe->m_lastCallState == AEECM_CALL_STATE_ORIG))*/
                {
                    (void) ISHELL_PostEvent(pMe->m_pShell,
                                          AEECLSID_DIALER/*AEECLSID_CALL*/, EVT_USER_REDRAW, 0, 0);
                }
            }
#else
            if(call_table->number.number_type == AEET_TON_INTERNATIONAL)
            {
                /*in this time,CM will inset a "+" in call_info->other_party_no OEMCM.c 5903*/
                Dialer_call_table * temp = NULL;
                AECHAR new_number[AEET_MAX_DIGITS_LENGTH] = {0};
				AECHAR	other_party_no[AEET_MAX_DIGITS_LENGTH]={0};  
				
				STRTOWSTR(call_table->call_info.other_party_no,other_party_no,AEET_MAX_DIGITS_LENGTH*sizeof(AECHAR));

                WSTRLCPY(new_number,other_party_no + 1,AEET_MAX_DIGITS_LENGTH);
                CALL_PRINTF("new_number = %S %S",new_number,other_party_no);
                //CALL_PRINT("other_party_no = ",call_table->call_info.other_party_no,0);
                temp = CallApp_Search_Number_In_Call_Table(pMe,new_number);
                if (temp)
                {
                    temp->call_number[0] = 0;
                    WSTRLCPY(temp->call_number,other_party_no ,AEET_MAX_DIGITS_LENGTH);
                }

                if(pMe->m_lastCallState == AEET_CALL_STATE_CONV)/*||
                     (pMe->m_lastCallState == AEET_CALL_STATE_INCOM) ||
                        (pMe->m_lastCallState == AEET_CALL_STATE_ORIG))*/
                {
                    (void) ISHELL_PostEvent(pMe->m_pShell,
                                          AEECLSID_DIALER/*AEECLSID_CALL*/, EVT_USER_REDRAW, 0, 0);
                }
            }
#endif
#endif
            break;
#ifdef FEATURE_ICM
        case AEECM_EVENT_CALL_ERROR:
#else
        case AEET_EVENT_CALL_ERROR:
#endif
            CALL_ERR("%x AEECM_EVENT_CALL_ERROR=+=", pCallInfo->event_data.call.call_error, 0, 0);
            *newState = STATE_EXIT;
            break;

        default:
            CALL_ERR("CM call event %x not handled", pCallInfo->event, 0, 0);
            break;
    }

#ifdef FEATURE_SUPPORT_BT_APP
    bt_ui_process_cmcall_notify(pMe,(uint32)pCallInfo->event/*(void*)&pCallInfo->event_data.call*/,TRUE);/*temp add to here*/
#endif

    pMe->m_lastCallState = call_table->call_info.call_state;
    CALL_ERR("pMe->m_lastCallState = %x", pMe->m_lastCallState, 0, 0);
}

/*=============================================================================
FUNCTION: CallApp_PlaySignal

DESCRIPTION:  Plays the signal info from network using IALERT

PARAMETERS:
   *pMe         : [IN]  CCallApp object pointer
    bySignalType: [IN]  Signal type from network
    bySignal    : [IN]  Signal from network
    byAlertPitch: [IN]  Alert pitch from network

RETURN VALUE:
    boolean     : returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CallApp_PlaySignal(CCallApp *pMe, AECHAR* pszPhoneNumber,
#ifdef FEATURE_ICM
                                                AEECMSignal *pSignal)
#else
                                                AEETSignal *pSignal)
#endif
{
    AECHAR         *pszPhNumber = NULL;
    AEEALERTType    AlertType;

    //PRINT_FUNCTION_NAME();
    //CALL_FUN_START("CallApp_PlaySignal", 0, 0, 0);

    if (pMe == NULL || pSignal == NULL)
    {
        CALL_ERR("Null pointer", 0,0,0);
        return FALSE;
    }

    if(WSTRLEN(pszPhoneNumber) > 0)
    {
        // give IALERT null if there is no phone number
        pszPhNumber = pszPhoneNumber;
    }

    CALL_ERR("%d %d CallApp_PlaySignal", pSignal->signal_type, pSignal->signal.cdma_tone, 0);
    switch(pSignal->signal_type)
    {
#ifdef FEATURE_ICM
        case AEECM_SIGNAL_CDMA_TONE:
#else
        case AEET_SIGNAL_CDMA_TONE:
#endif
            switch(pSignal->signal.cdma_tone)
            {
#ifdef FEATURE_ICM
                case AEECM_CDMA_TONE_DIAL:
#else
                case AEET_CDMA_TONE_DIAL:
#endif
                    AlertType = AEEALERT_ALERT_DIALTONE;
                    break;
#ifdef FEATURE_ICM
                case AEECM_CDMA_TONE_RING:
#else
                case AEET_CDMA_TONE_RING:
#endif
                    AlertType = AEEALERT_ALERT_RINGBACK;
                    break;
#ifdef FEATURE_ICM
                case AEECM_CDMA_TONE_BUSY:
#else
#endif
                    AlertType = AEEALERT_ALERT_BUSY;
                    break;
#ifdef FEATURE_ICM
                case AEECM_CDMA_TONE_ANSWER:
#else
                case AEET_CDMA_TONE_ANSWER:
#endif
                    AlertType = AEEALERT_ALERT_ANSWER;
                    break;
#ifdef FEATURE_ICM
                case AEECM_CDMA_TONE_INTERCEPT:
#else
                case AEET_CDMA_TONE_INTERCEPT:
#endif
                    AlertType = AEEALERT_ALERT_INTERCEPT;
                    break;
#ifdef FEATURE_ICM
                case AEECM_CDMA_TONE_ABBR_INTERCEPT:
#else
                case AEET_CDMA_TONE_ABBR_INTERCEPT:
#endif
                    AlertType = AEEALERT_ALERT_ABBR_INTERCEPT;
                    break;
#ifdef FEATURE_ICM
                case AEECM_CDMA_TONE_REORDER:
#else
                case AEET_CDMA_TONE_REORDER:
#endif
                    AlertType = AEEALERT_ALERT_REORDER;
                    break;
#ifdef FEATURE_ICM
                case AEECM_CDMA_TONE_ABBR_REORDER:
#else

#endif
                    AlertType = AEEALERT_ALERT_ABBR_REORDER;
                    break;
#ifdef FEATURE_ICM
                case AEECM_CDMA_TONE_CONFIRM:
#else
                case AEET_CDMA_TONE_CONFIRM:
#endif
                    AlertType = AEEALERT_ALERT_CONFIRM;
                    break;
#ifdef FEATURE_ICM
                case AEECM_CDMA_TONE_CALLWAIT:
#else
                case AEET_CDMA_TONE_CALLWAIT:
#endif
                    AlertType = AEEALERT_ALERT_CALLWAITING;
                    break;
#ifdef FEATURE_ICM
                case AEECM_CDMA_TONE_PIP:
#else
                case AEET_CDMA_TONE_PIP:
#endif
                    AlertType = AEEALERT_ALERT_PIP;
                    break;
#ifdef FEATURE_ICM
                case AEECM_CDMA_TONE_OFF:
#else
                case AEET_CDMA_TONE_OFF:
#endif
                default:
                    (void) IALERT_StopAlerting(pMe->m_pAlert);
                    return TRUE;
            }
            break;
#ifdef FEATURE_ICM
        case AEECM_SIGNAL_CDMA_ISDN:
#else
        case AEET_SIGNAL_CDMA_ISDN:
#endif
            switch(pSignal->signal.cdma_isdn)
            {
#ifdef FEATURE_ICM
                case AEECM_CDMA_ISDN_INTERGROUP:
#else
                case AEET_CDMA_ISDN_INTERGROUP:
#endif
                    AlertType = AEEALERT_ALERT_ISDN_INTERGROUP;
                    break;
#ifdef FEATURE_ICM
                case AEECM_CDMA_ISDN_SPECIAL:
#else
                case AEET_CDMA_ISDN_SPECIAL:
#endif
                    AlertType = AEEALERT_ALERT_ISDN_SPECIAL;
                    break;
#ifdef FEATURE_ICM
                case AEECM_CDMA_ISDN_PING:
#else
                case AEET_CDMA_ISDN_PING:
#endif
                    AlertType = AEEALERT_ALERT_ISDN_PING;
                    break;
                    return TRUE;
#ifdef FEATURE_ICM
                case AEECM_CDMA_ISDN_OFF:
#else
                case AEET_CDMA_ISDN_OFF:
#endif
                    (void) IALERT_StopAlerting(pMe->m_pAlert);
                    return TRUE;
#ifdef FEATURE_ICM
                case AEECM_CDMA_ISDN_NORMAL:
#else
                case AEET_CDMA_ISDN_NORMAL:
#endif
                default:
                    // play the normal ringer since this is default
                    AlertType = AEEALERT_ALERT_NORMAL;
                    //AlertType = AEEALERT_ALERT_ISDN_NORMAL;
                    break;
            }
            break;
#ifdef FEATURE_ICM
        case AEECM_SIGNAL_CDMA_IS54B:
#else
        case AEET_SIGNAL_CDMA_IS54B:
#endif
        {
            switch(pSignal->signal.cdma_is54b)
            {
#ifdef FEATURE_ICM
                case AEECM_CDMA_IS54B_L:
#else
                case AEET_CDMA_IS54B_L:
#endif
                    switch(pSignal->signal_pitch)
                    {
#ifdef FEATURE_ICM
                        case AEECM_PITCH_MED:
#else
                        case AEET_PITCH_MED:
#endif
                            // play the normal ringer since this is default
                            AlertType = AEEALERT_ALERT_NORMAL;
                            break;
#ifdef FEATURE_ICM
                        case AEECM_PITCH_HIGH:
#else
                        case AEET_PITCH_HIGH:
#endif
                            AlertType = AEEALERT_ALERT_IS54B_LONG_H;
                            break;
#ifdef FEATURE_ICM
                        case AEECM_PITCH_LOW:
#else
                        case AEET_PITCH_LOW:
#endif
                            AlertType = AEEALERT_ALERT_IS54B_LONG_L;
                            break;

                        default:
                            CALL_ERR("Invalid Alert Pitch: %d", pSignal->signal_pitch,0,0);
                            return FALSE;
                    }
                    break;
#ifdef FEATURE_ICM
                case AEECM_CDMA_IS54B_SS:
#else
                case AEET_CDMA_IS54B_SS:
#endif
                    switch(pSignal->signal_pitch)
                    {
#ifdef FEATURE_ICM
                        case AEECM_PITCH_MED:
#else
                        case AEET_PITCH_MED:
#endif
                            AlertType = AEEALERT_ALERT_IS54B_SS_M;
                            break;
#ifdef FEATURE_ICM
                        case AEECM_PITCH_HIGH:
#else
                        case AEET_PITCH_HIGH:
#endif
                            AlertType = AEEALERT_ALERT_IS54B_SS_H;
                            break;
#ifdef FEATURE_ICM
                        case AEECM_PITCH_LOW:
#else
                        case AEET_PITCH_LOW:
#endif
                            AlertType = AEEALERT_ALERT_IS54B_SS_L;
                            break;

                        default:
                            CALL_ERR("Invalid Alert Pitch: %d", pSignal->signal_pitch,0,0);
                            return FALSE;
                    }
                    break;
#ifdef FEATURE_ICM
                case AEECM_CDMA_IS54B_SSL:
#else
                case AEET_CDMA_IS54B_SSL:
#endif
                    switch(pSignal->signal_pitch)
                    {
#ifdef FEATURE_ICM
                        case AEECM_PITCH_MED:
#else
                        case AEET_PITCH_MED:
#endif
                            AlertType = AEEALERT_ALERT_IS54B_SSL_M;
                            break;

#ifdef FEATURE_ICM
                        case AEECM_PITCH_HIGH:
#else
                        case AEET_PITCH_HIGH:
#endif
                            AlertType = AEEALERT_ALERT_IS54B_SSL_H;
                            break;

#ifdef FEATURE_ICM
                        case AEECM_PITCH_LOW:
#else
                        case AEET_PITCH_LOW:
#endif
                            AlertType = AEEALERT_ALERT_IS54B_SSL_L;
                            break;

                        default:
                            CALL_ERR("Invalid Alert Pitch: %d", pSignal->signal_pitch,0,0);
                            return FALSE;
                    }
                    break;
#ifdef FEATURE_ICM
                case AEECM_CDMA_IS54B_SS2:
#else
                case AEET_CDMA_IS54B_SS2:
#endif
                    switch(pSignal->signal_pitch)
                    {
#ifdef FEATURE_ICM
                        case AEECM_PITCH_MED:
#else
                        case AEET_PITCH_MED:
#endif
                            AlertType = AEEALERT_ALERT_IS54B_SS2_M;
                            break;

#ifdef FEATURE_ICM
                        case AEECM_PITCH_HIGH:
#else
                        case AEET_PITCH_HIGH:
#endif
                            AlertType = AEEALERT_ALERT_IS54B_SS2_H;
                            break;

#ifdef FEATURE_ICM
                        case AEECM_PITCH_LOW:
#else
                        case AEET_PITCH_LOW:
#endif
                            AlertType = AEEALERT_ALERT_IS54B_SS2_L;
                            break;

                        default:
                            CALL_ERR("Invalid Alert Pitch: %d", pSignal->signal_pitch,0,0);
                            return FALSE;
                    }
                    break;
#ifdef FEATURE_ICM
                case AEECM_CDMA_IS54B_SLS:
#else
                case AEET_CDMA_IS54B_SLS:
#endif
                    switch(pSignal->signal_pitch)
                    {
#ifdef FEATURE_ICM
                        case AEECM_PITCH_MED:
#else
                        case AEET_PITCH_MED:
#endif
                            AlertType = AEEALERT_ALERT_IS54B_SLS_M;
                            break;

#ifdef FEATURE_ICM
                        case AEECM_PITCH_HIGH:
#else
                        case AEET_PITCH_HIGH:
#endif
                            AlertType = AEEALERT_ALERT_IS54B_SLS_H;
                            break;

#ifdef FEATURE_ICM
                        case AEECM_PITCH_LOW:
#else
                        case AEET_PITCH_LOW:
#endif
                            AlertType = AEEALERT_ALERT_IS54B_SLS_L;
                            break;

                        default:
                            CALL_ERR("Invalid Alert Pitch: %d", pSignal->signal_pitch,0,0);
                            return FALSE;
                    }
                    break;
#ifdef FEATURE_ICM
                case AEECM_CDMA_IS54B_SSSS:
#else
                case AEET_CDMA_IS54B_SSSS:
#endif
                    switch(pSignal->signal_pitch)
                    {
#ifdef FEATURE_ICM
                        case AEECM_PITCH_MED:
#else
                        case AEET_PITCH_MED:
#endif
                            AlertType = AEEALERT_ALERT_IS54B_SSSS_M;
                            break;

#ifdef FEATURE_ICM
                        case AEECM_PITCH_HIGH:
#else
                        case AEET_PITCH_HIGH:
#endif
                            AlertType = AEEALERT_ALERT_IS54B_SSSS_H;
                            break;

#ifdef FEATURE_ICM
                        case AEECM_PITCH_LOW:
#else
                        case AEET_PITCH_LOW:
#endif
                            AlertType = AEEALERT_ALERT_IS54B_SSSS_L;
                            break;

                        default:
                            CALL_ERR("Invalid Alert Pitch: %d", pSignal->signal_pitch,0,0);
                            return FALSE;
                    }
                    break;
#ifdef FEATURE_ICM
                case AEECM_CDMA_IS54B_PBX_L:
#else
                case AEET_CDMA_IS54B_PBX_L:
#endif
                    switch(pSignal->signal_pitch)
                    {
#ifdef FEATURE_ICM
                        case AEECM_PITCH_MED:
#else
                        case AEET_PITCH_MED:
#endif
                            AlertType = AEEALERT_ALERT_IS54B_PBX_LONG_M;
                            break;

#ifdef FEATURE_ICM
                        case AEECM_PITCH_HIGH:
#else
                        case AEET_PITCH_HIGH:
#endif
                            AlertType = AEEALERT_ALERT_IS54B_PBX_LONG_H;
                            break;

#ifdef FEATURE_ICM
                        case AEECM_PITCH_LOW:
#else
                        case AEET_PITCH_LOW:
#endif
                            AlertType = AEEALERT_ALERT_IS54B_PBX_LONG_L;
                            break;

                        default:
                            CALL_ERR("Invalid Alert Pitch: %d", pSignal->signal_pitch,0,0);
                            return FALSE;
                    }
                    break;
#ifdef FEATURE_ICM
                case AEECM_CDMA_IS54B_PBX_SS:
#else
                case AEET_CDMA_IS54B_PBX_SS:
#endif
                    switch(pSignal->signal_pitch)
                    {
#ifdef FEATURE_ICM
                        case AEECM_PITCH_MED:
#else
                        case AEET_PITCH_MED:
#endif
                            AlertType = AEEALERT_ALERT_IS54B_PBX_SS_M;
                            break;

#ifdef FEATURE_ICM
                        case AEECM_PITCH_HIGH:
#else
                        case AEET_PITCH_HIGH:
#endif
                            AlertType = AEEALERT_ALERT_IS54B_PBX_SS_H;
                            break;

#ifdef FEATURE_ICM
                        case AEECM_PITCH_LOW:
#else
                        case AEET_PITCH_LOW:
#endif
                            AlertType = AEEALERT_ALERT_IS54B_PBX_SS_L;
                            break;

                        default:
                            CALL_ERR("Invalid Alert Pitch: %d", pSignal->signal_pitch,0,0);
                            return FALSE;
                    }
                    break;
#ifdef FEATURE_ICM
                case AEECM_CDMA_IS54B_PBX_SSL:
#else
                case AEET_CDMA_IS54B_PBX_SSL:
#endif
                    switch(pSignal->signal_pitch)
                    {
#ifdef FEATURE_ICM
                        case AEECM_PITCH_MED:
#else
                        case AEET_PITCH_MED:
#endif
                            AlertType = AEEALERT_ALERT_IS54B_PBX_SSL_M;
                            break;

#ifdef FEATURE_ICM
                        case AEECM_PITCH_HIGH:
#else
                        case AEET_PITCH_HIGH:
#endif
                            AlertType = AEEALERT_ALERT_IS54B_PBX_SSL_H;
                            break;

#ifdef FEATURE_ICM
                        case AEECM_PITCH_LOW:
#else
                        case AEET_PITCH_LOW:
#endif
                            AlertType = AEEALERT_ALERT_IS54B_PBX_SSL_L;
                            break;

                        default:
                            CALL_ERR("Invalid Alert Pitch: %d", pSignal->signal_pitch,0,0);
                            return FALSE;
                    }
                    break;
#ifdef FEATURE_ICM
                case AEECM_CDMA_IS54B_PBX_SLS:
#else
                case AEET_CDMA_IS54B_PBX_SLS:
#endif
                    switch(pSignal->signal_pitch)
                    {
#ifdef FEATURE_ICM
                        case AEECM_PITCH_MED:
#else
                        case AEET_PITCH_MED:
#endif
                            AlertType = AEEALERT_ALERT_IS54B_PBX_SLS_M;
                            break;

#ifdef FEATURE_ICM
                        case AEECM_PITCH_HIGH:
#else
                        case AEET_PITCH_HIGH:
#endif
                            AlertType = AEEALERT_ALERT_IS54B_PBX_SLS_H;
                            break;

#ifdef FEATURE_ICM
                        case AEECM_PITCH_LOW:
#else
                        case AEET_PITCH_LOW:
#endif
                            AlertType = AEEALERT_ALERT_IS54B_PBX_SLS_L;
                            break;

                        default:
                            CALL_ERR("Invalid Alert Pitch: %d", pSignal->signal_pitch,0,0);
                            return FALSE;
                    }
                    break;
#ifdef FEATURE_ICM
                case AEECM_CDMA_IS54B_PBX_SSSS:
#else
                case AEET_CDMA_IS54B_PBX_SSSS:
#endif
                    switch(pSignal->signal_pitch)
                    {
#ifdef FEATURE_ICM
                        case AEECM_PITCH_MED:
#else
                        case AEET_PITCH_MED:
#endif
                            AlertType = AEEALERT_ALERT_IS54B_PBX_SSSS_M;
                            break;

#ifdef FEATURE_ICM
                        case AEECM_PITCH_HIGH:
#else
                        case AEET_PITCH_HIGH:
#endif
                            AlertType = AEEALERT_ALERT_IS54B_PBX_SSSS_H;
                            break;

#ifdef FEATURE_ICM
                        case AEECM_PITCH_LOW:
#else
                        case AEET_PITCH_LOW:
#endif
                            AlertType = AEEALERT_ALERT_IS54B_PBX_SSSS_L;
                            break;

                        default:
                            CALL_ERR("Invalid Alert Pitch: %d", pSignal->signal_pitch,0,0);
                            return FALSE;
                    }
                    break;
#ifdef FEATURE_ICM
                case AEECM_CDMA_IS54B_PPPP:
#else
                case AEET_CDMA_IS54B_PPPP:
#endif
                    switch(pSignal->signal_pitch)
                    {
#ifdef FEATURE_ICM
                        case AEECM_PITCH_MED:
#else
                        case AEET_PITCH_MED:
#endif
                            AlertType = AEEALERT_ALERT_IS53A_PPPP_M;
                            break;

#ifdef FEATURE_ICM
                        case AEECM_PITCH_HIGH:
#else
                        case AEET_PITCH_HIGH:
#endif
                            AlertType = AEEALERT_ALERT_IS53A_PPPP_H;
                            break;

#ifdef FEATURE_ICM
                        case AEECM_PITCH_LOW:
#else
                        case AEET_PITCH_LOW:
#endif
                            AlertType = AEEALERT_ALERT_IS53A_PPPP_L;
                            break;

                        default:
                            CALL_ERR("Invalid Alert Pitch: %d", pSignal->signal_pitch,0,0);
                            return FALSE;
                    }
                    break;
#ifdef FEATURE_ICM
                case AEECM_CDMA_IS54B_OFF:
#else
                case AEET_CDMA_IS54B_OFF:
#endif
                    (void) IALERT_StopAlerting(pMe->m_pAlert);
                    return TRUE;

                default:
                    CALL_ERR("Invalid Signal for IS54b: %d", pSignal->signal.cdma_is54b,0,0);
                    return FALSE;
            }
        }
        break;
#ifdef FEATURE_ICM
        case AEECM_SIGNAL_GW_TONE:
#else
        case AEET_SIGNAL_GW_TONE:
#endif
            switch(pSignal->signal.gw_tone)
            {
#ifdef FEATURE_ICM
                case AEECM_GW_DIAL_TONE_ON:
#else
                case AEET_GW_DIAL_TONE_ON:
#endif
                    AlertType = AEEALERT_ALERT_DIALTONE;
                    break;
#ifdef FEATURE_ICM
                case AEECM_GW_RING_BACK_TONE_ON:
#else
                case AEET_GW_RING_BACK_TONE_ON:
#endif
                    AlertType = AEEALERT_ALERT_RINGBACK;
                    break;
#ifdef FEATURE_ICM
                case AEECM_GW_BUSY_TONE_ON:
#else
                case AEET_GW_BUSY_TONE_ON:
#endif
                    AlertType = AEEALERT_ALERT_BUSY;
                    break;
#ifdef FEATURE_ICM
                case AEECM_GW_ANSWER_TONE_ON:
#else
                case AEET_GW_ANSWER_TONE_ON:
#endif
                    AlertType = AEEALERT_ALERT_ANSWER;
                    break;
#ifdef FEATURE_ICM
                case AEECM_GW_INTERCEPT_TONE_ON:
#else
                case AEET_GW_INTERCEPT_TONE_ON:
#endif
                    AlertType = AEEALERT_ALERT_INTERCEPT;
                    break;
#ifdef FEATURE_ICM
                case AEECM_GW_CONFIRM_TONE_ON:
#else
                case AEET_GW_CONFIRM_TONE_ON:
#endif
                    AlertType = AEEALERT_ALERT_CONFIRM;
                    break;
#ifdef FEATURE_ICM
                case AEECM_GW_CALL_WAITING_TONE_ON:
#else
                case AEET_GW_CALL_WAITING_TONE_ON:
#endif
                    AlertType = AEEALERT_ALERT_CALLWAITING;
                    break;
#ifdef FEATURE_ICM
                case AEECM_GW_TONES_OFF:
                case AEECM_GW_NW_CONGESTION_TONE_ON:
                case AEECM_GW_OFF_HOOK_WARNING_TONE_ON:
                case AEECM_GW_ALERTING_TONE_OFF:
#else
                case AEET_GW_TONES_OFF:
                case AEET_GW_NW_CONGESTION_TONE_ON:
                case AEET_GW_OFF_HOOK_WARNING_TONE_ON:
                case AEET_GW_ALERTING_TONE_OFF:
#endif
                default:
                    (void) IALERT_StopAlerting(pMe->m_pAlert);
                    return TRUE;
            }
            break;

        default:
            CALL_ERR("Invalid Signal Type: %d", pSignal->signal_type,0,0);
            return FALSE;
    }

    CALL_ERR(" CallApp_PlaySignal AlertType = %d", AlertType, 0, 0);
    if ( AlertType == AEEALERT_ALERT_NORMAL )
    {
        //IALERT_SetAlertType(pMe->m_pAlert, AEEALERT_ALERT_NORMAL);
        return TRUE;
    }
    else
        return (CheckAEEReturnStatus(IALERT_StartAlerting(pMe->m_pAlert, AEEALERT_CALLTYPE_VOICE, pszPhNumber, AlertType)));
}

/*=============================================================================
FUNCTION:  Play next tone

DESCRIPTION:

PARAMETERS:


RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void CallApp_NextTonePlay(void *pUser)
{
    CCallApp *pMe = (CCallApp *)pUser;
    static AEESoundInfo    si =
    {
        AEE_SOUND_DEVICE_CURRENT,
        AEE_SOUND_METHOD_BEEP,
        AEE_SOUND_APATH_LOCAL,
        AEE_SOUND_MUTECTL_UNMUTED,
        AEE_SOUND_MUTECTL_UNMUTED,
    };

    CALL_FUN_START("CallApp_NextTonePlay", 0, 0, 0);

    pMe->m_nToneCurrIndex ++;

    if (pMe->m_nToneCurrIndex >= pMe->m_nToneCount)
    {
        OEMSound_StopTone(FALSE, NULL);
        pMe->m_nToneCount = 0;
        pMe->m_nToneCurrIndex = 0;
    }

    else
    {
        OEMSound_PlayTone(&si, &(pMe->m_toneData[pMe->m_nToneCurrIndex]), NULL);
        (void)ISHELL_SetTimer(pMe->m_pShell,
                                                pMe->m_toneData[pMe->m_nToneCurrIndex].wDuration,
                                                CallApp_NextTonePlay, pMe);
    }
}

/*=============================================================================
FUNCTION: CallApp_IsRestictCallNumber_Ex

DESCRIPTION: 判断是否为呼叫受限制的号码

PARAMETERS:
  *pMe : CCallApp object pointer
   pNumber: phone number
   IsOutgoing  : TRUE is Outgoing

RETURN VALUE:
    None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:
   AEEOEM_CallHistoryAddEntry(pMe->m_pOEM_TSGBridge,)

=============================================================================*/
boolean CallApp_IsRestictCallNumber_Ex(CCallApp   *pMe,
                                       AECHAR       *pNumber,
                                       boolean       IsOutgoing)
{
    byte        data;
    AECHAR      Name[32];
    Name[0] = 0;

    CALL_FUN_START("CallApp_IsRestictCallNumber_Ex",0,0,0);
    //pMe->m_b_call_barrings=FALSE;

    if(pMe == NULL)
    {
        return FALSE;
    }

    if(pNumber == NULL)
    {
        return FALSE;
    }

    //if(CallApp_IsEmergency_Number(pNumber))
    //{
    //    return FALSE;
    //}

    if(IsOutgoing)
    {
        ICONFIG_GetItem(pMe->m_pConfig, CFGI_RESTRICT_OUTGOING, &data, sizeof(byte));

        switch (data)
        {
      	    case OEMNV_RESTRICT_OUTGOING_ALL:
                return TRUE;

            case OEMNV_RESTRICT_OUTGOING_OUTCONTACT:
                (void)CallApp_NumberLookup(pMe,
                                             pNumber,
                                             Name,
                                             sizeof(Name)/sizeof(AECHAR),
                                             NULL,
                                             NULL);
                if (Name[0] == 0)
                {
                    return TRUE;
                }

                else
                {
                    return FALSE;
                }

            case OEMNV_RESTRICT_OUTGOING_INTERNATIONAL:
                if(WSTRLEN(pNumber)>=2)
                {
                    if(((AECHAR)'0' == pNumber[0]  && (AECHAR)'0' == pNumber[1])||
                                                       ((AECHAR)'+' == pNumber[0]))
                    {
                        return TRUE;
                    }

                    else
                    {
                        return FALSE;
                    }
                }

                else
                {
                    return FALSE;
                }

            case OEMNV_RESTRICT_OUTGOING_INTERNAL:
                if(WSTRLEN(pNumber)>=2)
                {
                    if((AECHAR)'0' == pNumber[0] )
                    {
                        return TRUE;
                    }

                    else
                    {
                        return FALSE;
                    }
                }

                else
                {
                    return FALSE;
                }

            case OEMNV_RESTRICT_OUTGOING_OFF:
            default:
                return FALSE;
        }
    }
    else
    {
        (void) ICONFIG_GetItem(pMe->m_pConfig, CFGI_RESTRICT_INCOMING, &data, sizeof(byte));
        if (data == OEMNV_RESTRICT_INCOMING_ALL)
        {
            //pMe->m_b_call_barrings=TRUE;
            return TRUE;
        }

        if(data == OEMNV_RESTRICT_INCOMING_OUTCONTACT)
        {
            (void)CallApp_NumberLookup(pMe,
                                         pNumber,
                                         Name,
                                         sizeof(Name)/sizeof(AECHAR),
                                         NULL,
                                         NULL);
            if (WSTRLEN(Name) == 0)
            {
                //pMe->m_b_call_barrings=TRUE;
         	    return TRUE;
            }

            else
            {
         	    return FALSE;
            }
        }
    }
    return FALSE;
}
//AEECMCallState CallApp_Get_Current_CallState(AEECMCallEventData *call_table)
//{
//    return call_table->call_info.call_state;
//}
/*=============================================================================
FUNCTION: CallApp_IsEmergencyMode

DESCRIPTION: Check if the  phone  is emergency mode

PARAMETERS:
    pICM            :  [IN] ICM Instance

RETURN VALUE:
    boolean         : return TRUE if phone  is in emergency mode

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
#ifdef FEATURE_ICM
boolean CallApp_IsEmergencyMode(ICM* pICM)
{
    AEECMPhInfo phoneInfo;

    //PRINT_FUNCTION_NAME();
    CALL_FUN_START("CallApp_IsEmergencyMode",0,0,0);

    if (!pICM)
    {
        return FALSE;
    }

    ICM_GetPhoneInfo(pICM, &phoneInfo, sizeof(phoneInfo));
    if (phoneInfo.mode_pref == AEECM_MODE_PREF_EMERGENCY)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
#else
boolean CallApp_IsEmergencyMode(ITelephone * pTelephone)
{
    AEETPhInfo phoneInfo;

    //PRINT_FUNCTION_NAME();
    CALL_FUN_START("CallApp_IsEmergencyMode",0,0,0);

    if (!pTelephone)
    {
        return FALSE;
    }

    ITELEPHONE_GetPhoneInfo(pTelephone, &phoneInfo, sizeof(phoneInfo));
    if (phoneInfo.mode_pref == AEET_MODE_PREF_EMERGENCY)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
#endif

/*===========================================================================

FUNCTION CallApp_IsEmergency_Number

DESCRIPTION
  Query if the specified number is considered an emergency number

DEPENDENCIES


RETURN VALUE
  TRUE - if the number is en emergency number
  FALSE - otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean CallApp_IsEmergency_Number(AECHAR *pNumber)
{
    int       len = 0;
    char    num_buf[33];

    //CALL_FUN_START("CallApp_IsEmergency_Number",0,0,0);

    if (NULL == pNumber)
    {
        return FALSE;
    }

    if(0 == WSTRLEN(pNumber))
    {
        return FALSE;
    }

    WSTRTOSTR(pNumber,  num_buf,  sizeof(num_buf));
    num_buf[sizeof(num_buf)-1] = '\0';
    len = STRLEN(num_buf);
    CALL_PRINTF("%s %d CallApp_IsEmergency_Number",num_buf,len);
    return OEM_IsEmergency_Number(num_buf,len);

    //len = STRLEN((char*)num_buf);

    //return CallApp_IsEmergency_Number_Ex(num_buf, (byte) len);
}

#if 0
/*===========================================================================
FUNCTION   CallApp_IsEmergency_Number_Ex

DESCRIPTION
  Determine if a call is an emergency Number

DEPENDENCIES
  None

RETURNS
  is it an emergency Number?

SIDE EFFECTS
  None
===========================================================================*/
static boolean CallApp_IsEmergency_Number_Ex(const byte *num_buf,
                                       byte        num_len)
{
    uint32 i;

    CALL_FUN_START("CallApp_IsEmergency_Number_Ex",0,0,0);

    if (num_len == 0)
    {
        return FALSE;
    }

    for (i = 0; i < ARR_SIZE(emergencyNumTbl); i++)
    {
        EmergencyNumber *eList = emergencyNumTbl[i];

        if (eList)
        {
            int q;

            for (q = 0; eList[q].num_len > 0; q++)
            {
                if (num_len == eList[q].num_len)
                {
                    if (0 == STRNCMP((char*) num_buf,  eList[q].num_buf, num_len))
                    {
                        return TRUE;
                    }
                }
            }
        }
    }

    return FALSE;
}
#endif
/*===========================================================================
FUNCTION   CallApp_Compare_Number

DESCRIPTION

DEPENDENCIES
  None

RETURNS

SIDE EFFECTS
  None
===========================================================================*/
int CallApp_Compare_Number(AECHAR * Number1, AECHAR * Number2)
{
    AECHAR *pause = NULL;
    AECHAR *timer = NULL;
    int result = 0;
    int len =0;

    pause = WSTRCHR(Number2, DIALER_PAUSE_AECHAR);//search the first P
#ifdef FEATURE_APP_PAUSE_TIMER
    timer =  WSTRCHR(Number2, DIALER_TIMER_AECHAR);//search the first T
#endif
    CALL_ERR("pause / timer = %d, %d",pause,timer,0);

    if (NULL == timer && NULL == pause)
    {
        result = WSTRCMP(Number1,Number2);
    }
    else
    {
        // 若同时出现'P'和'T',谁在前就先处理谁
        if ((NULL != timer) && (NULL != pause))
        {
            if (timer>pause)
            {   // 'P'在前
                timer=NULL;
                len = pause - Number2;
            }
            else
            {
                pause=NULL;
                len = timer - Number2;
            }
        }

        if (NULL != timer)
        {
            len = timer - Number2;
        }
        else if (NULL != pause)
        {
            len = pause - Number2;
        }

        CALL_ERR("Compare_Number len = %d",len,0,0);

        result = WSTRNCMP(Number1,Number2,len);
    }
    return result;
}

Dialer_call_table * CallApp_Search_Number_In_Call_Table(CCallApp *pMe,AECHAR *number)
{
    Dialer_call_table *b_find = NULL;
    Dialer_call_table *temp =  pMe->m_CallsTable;

    if(pMe == NULL || number == NULL)
    {
        return FALSE;
    }

    if(temp == NULL)
    {
        return NULL;
    }

    while(temp != NULL)
    {
        //CALL_ERR("Search_Number len = %d",len,0,0);
        //if inset the P/T in call_table,we can not find the match number,
        if(0 == CallApp_Compare_Number(number,temp->call_number))
        {
            b_find = temp;
            break;
        }
        temp = temp->next;
    }
    if(b_find)
    {
        CALL_ERR("ok,find it",0,0,0);
    }
    else
    {
        CALL_ERR("fail,not find",0,0,0);
    }
    return b_find;
}

boolean CallApp_Add_Number_To_Call_Table(CCallApp *pMe,AECHAR *number,
                                            uint8 call_id, uint16 call_type/*uint16 call_status*/,callPIType type,boolean b_force_add,
                                            boolean is_energency, boolean is_restict)
{
    AECHAR    m_call_name[MAX_SIZE_NAME_TEXT] = {0};
    AECHAR    m_call_number[AEECM_MAX_DIGITS_LENGTH] = {0};
    AECHAR    ringer[MAX_FILE_NAME] = {0};
    boolean   allow_add = TRUE;
    int       len       = 0;
    CALL_ERR(" %d ,%d ,%d CallApp_Add_Number_To_Call_Table",call_id,call_type/*call_status*/,type);

    if(pMe == NULL || number == NULL)
    {
        return FALSE;
    }

    //CALL_PRINT("Add Number To Table",number,0);
    len = WSTRLEN(number);
    if(len == 0)
    {
        CALL_ERR("No number, Not allow add to call table",0,0,0);
        allow_add = FALSE;
    }
    else
    {
        CALL_PRINTF("Add %S To Table",number);
        WSTRLCPY(m_call_number,number,AEECM_MAX_DIGITS_LENGTH);
        (void)CallApp_NumberLookup(pMe, m_call_number, m_call_name,MAX_SIZE_NAME_TEXT, ringer,  NULL);
    }

    if(b_force_add)
    {
        allow_add = TRUE;
        if(len == 0)
        {
            (void) ISHELL_LoadResString(pMe->m_pShell,
                                        AEE_APPSCALLAPP_RES_FILE,
                                        IDS_NO_NUMBER,
                                        m_call_number/*m_call_name*/,
                                        AEECM_MAX_DIGITS_LENGTH/*MAX_SIZE_NAME_TEXT*/);
        }
        else
        {
            WSTRLCPY(m_call_number,number,AEECM_MAX_DIGITS_LENGTH);
        }
    }
    
    switch (type)
    {
        case PI_RESTRICTED: // restricted
            (void) ISHELL_LoadResString(pMe->m_pShell,
                                        AEE_APPSCALLAPP_RES_FILE,
                                        IDS_NUMBER_RESTRICTED,
                                        m_call_number/*m_call_name*/,
                                        AEECM_MAX_DIGITS_LENGTH/*MAX_SIZE_NAME_TEXT*/);
            pMe->m_b_is_PI_ALLOWED = PI_RESTRICTED;
            allow_add = TRUE;
            break;

        case PI_UNAVAILABLE: // unavailable
            (void) ISHELL_LoadResString(pMe->m_pShell,
                                        AEE_APPSCALLAPP_RES_FILE,
                                        IDS_NUMBER_UNAVAILABLE,
                                        m_call_number/*m_call_name*/,
                                        AEECM_MAX_DIGITS_LENGTH/*MAX_SIZE_NAME_TEXT*/);
            allow_add = TRUE;
            pMe->m_b_is_PI_ALLOWED = PI_UNAVAILABLE;
            break;

        default:
            pMe->m_b_is_PI_ALLOWED =PI_ALLOWED;
            break;
    }    

    if(!allow_add)
    {
        CALL_ERR("Not allow add to call table",0,0,0);
        return FALSE;
    }
#if 1
    MEMSET((void*)&pMe->m_call_info,0,sizeof(Dialer_call_table));
    WSTRLCPY(pMe->m_call_info.call_number, m_call_number, AEECM_MAX_DIGITS_LENGTH);
    pMe->m_call_info.in_phonebook = FALSE;

    if(m_call_name[0] != 0)
    {
        WSTRLCPY(pMe->m_call_info.call_name, m_call_name, MAX_SIZE_NAME_TEXT);
        pMe->m_call_info.in_phonebook = TRUE;
    }

    pMe->m_call_info.call_id     = call_id;
    pMe->m_call_info.start_time  = ISHELL_GetUpTimeMS(pMe->m_pShell);//get current time
    pMe->m_call_info.call_type = call_type;
    //pMe->m_call_info.call_status = call_status;
    pMe->m_call_info.type        = type;
    WSTRCPY(pMe->m_call_info.ringer, ringer);
    pMe->m_call_info.next        = NULL;
    pMe->m_call_info.b_emerg     = is_energency;
    pMe->m_call_info.b_restict   = is_restict;
#endif
    CALL_ERR("pMe->m_CallsTable_Count = %d",pMe->m_CallsTable_Count,0,0);
    if(pMe->m_CallsTable_Count == 0)//the fisrt call
    {
        Dialer_call_table *temp = NULL;
        temp = (Dialer_call_table*) CALLAPP_MALLOC(sizeof(Dialer_call_table));

        if(temp)
        {
            MEMSET(temp,0,sizeof(Dialer_call_table));
            WSTRLCPY(temp->call_number, m_call_number, AEECM_MAX_DIGITS_LENGTH);
            temp->in_phonebook = FALSE;

            if(m_call_name[0] != 0)
            {
                WSTRLCPY(temp->call_name, m_call_name, MAX_SIZE_NAME_TEXT);
                temp->in_phonebook = TRUE;
            }

            temp->call_id      = call_id;
            temp->start_time   = ISHELL_GetUpTimeMS(pMe->m_pShell);//get current time
            temp->call_type = call_type;
            //temp->call_status  = call_status;
            temp->type         = type;
            WSTRCPY(temp->ringer, ringer);
            temp->next         = NULL;
            temp->b_emerg      = is_energency;
            temp->b_restict    = is_restict;
            pMe->m_CallsTable  = temp;
            pMe->m_CallsTable_Count ++ ;
            return TRUE;
        }
        else
        {
            CALL_ERR("MALLOC Dialer_call_table fail",0,0,0);
            return FALSE;
        }

    }
    else//Muti call
    {
        if(pMe->m_CallsTable_Count == DIALERAPP_MAX_NUM_CALLS)
        {
            CALL_ERR("have %d call table,not allow add it",pMe->m_CallsTable_Count,0,0);
            return FALSE;
        }
        else
        {
            Dialer_call_table *head = NULL;
            head = CallApp_Search_Number_In_Call_Table(pMe,m_call_number);
            if(NULL == head)
            {
                Dialer_call_table *temp = NULL;
                temp = (Dialer_call_table*) CALLAPP_MALLOC(sizeof(Dialer_call_table));
                if(temp)
                {
                    MEMSET(temp,0,sizeof(Dialer_call_table));
                    WSTRLCPY(temp->call_number, m_call_number, AEECM_MAX_DIGITS_LENGTH);
                    temp->in_phonebook = FALSE;

                    if(m_call_name[0] != 0)
                    {
                        WSTRLCPY(temp->call_name, m_call_name, MAX_SIZE_NAME_TEXT);
                        temp->in_phonebook = TRUE;
                    }

                    temp->call_id      = call_id;
                    temp->start_time   = ISHELL_GetUpTimeMS(pMe->m_pShell);//get current time
                    temp->call_type = call_type;
                    //temp->call_status  = call_status;
                    temp->type         = type;
                    WSTRCPY(temp->ringer, ringer);
                    temp->b_emerg      = is_energency;
                    temp->b_restict    = is_restict;
                    temp->next         = pMe->m_CallsTable;
                    pMe->m_CallsTable  = temp;
                    pMe->m_CallsTable_Count ++ ;
                    return TRUE;
                }
                else
                {
                    head->call_id = call_id;
                    //CALL_ERR("MALLOC Dialer_call_table fail",0,0,0);
                    return TRUE;
                }
            }
            else
            {
                //CALL_ERR("we have find this number in call table,not allow add it",0,0,0);
                return FALSE;
            }
        }
    }
}

boolean CallApp_Modify_Number_To_Call_Table(CCallApp *pMe,AECHAR *number,
                                            uint8 call_id,uint16 call_type/*uint16 call_status*/,callPIType type)
{
    AECHAR    m_call_name[MAX_SIZE_NAME_TEXT] = {0};
    AECHAR    m_call_number[MAX_SIZE_DIAL_STR] = {0};
    AECHAR    ringer[MAX_FILE_NAME] = {0};
    CALL_FUN_START("%d type= %d CallApp_Modify_Number_To_Call_Table",pMe->m_CallsTable_Count,type,0);

    if(WSTRLEN(number) == 0)
    {
        CALL_ERR("No number, Not allow Modify call table",0,0,0);
        return FALSE;
    }
    else
    {
        if(pMe->m_b_is_PI_ALLOWED == PI_ALLOWED)
        {
            (void)CallApp_NumberLookup(pMe, number, m_call_name,MAX_SIZE_NAME_TEXT, ringer,  NULL);
        }
    }
    
    switch (type)
    {
        case PI_RESTRICTED: // restricted
            (void) ISHELL_LoadResString(pMe->m_pShell,
                                        AEE_APPSCALLAPP_RES_FILE,
                                        IDS_NUMBER_RESTRICTED,
                                        m_call_number,
                                        MAX_SIZE_DIAL_STR);
            pMe->m_b_is_PI_ALLOWED = PI_RESTRICTED;
            break;

        case PI_UNAVAILABLE: // unavailable
            (void) ISHELL_LoadResString(pMe->m_pShell,
                                        AEE_APPSCALLAPP_RES_FILE,
                                        IDS_NUMBER_UNAVAILABLE,
                                        m_call_number,
                                        MAX_SIZE_DIAL_STR);
            pMe->m_b_is_PI_ALLOWED = PI_UNAVAILABLE;
            break;

        default:
            pMe->m_b_is_PI_ALLOWED = PI_ALLOWED;
            break;
    }    

#if 1
    MEMSET((void*)&pMe->m_call_info,0,sizeof(Dialer_call_table));
    pMe->m_call_info.in_phonebook = FALSE;
    if(pMe->m_b_is_PI_ALLOWED == PI_ALLOWED)
    {
        WSTRLCPY(pMe->m_call_info.call_number, number, AEECM_MAX_DIGITS_LENGTH);
        if(m_call_name[0] != 0)
        {
            WSTRLCPY(pMe->m_call_info.call_name, m_call_name, MAX_SIZE_NAME_TEXT);
            pMe->m_call_info.in_phonebook = TRUE;
        }
    }
    else
    {
        WSTRLCPY(pMe->m_call_info.call_number, m_call_number, AEECM_MAX_DIGITS_LENGTH);
    }

    pMe->m_call_info.call_id     = call_id;
    pMe->m_call_info.start_time  = ISHELL_GetUpTimeMS(pMe->m_pShell);//get current time
    //pMe->m_call_info.call_status = call_status;
    pMe->m_call_info.call_type = call_type;
    pMe->m_call_info.type        = type;
    //pMe->m_call_info.ringer= ringer_id;
    WSTRCPY(pMe->m_call_info.ringer, ringer);
    pMe->m_call_info.next        = NULL;
#endif
    if(pMe->m_CallsTable_Count == 1)
    {
        if(pMe->m_CallsTable)
        {
            MEMSET((void*)pMe->m_CallsTable,0,sizeof(Dialer_call_table));
            pMe->m_CallsTable->in_phonebook = FALSE;
            if(pMe->m_b_is_PI_ALLOWED == PI_ALLOWED)
            {
                WSTRLCPY(pMe->m_CallsTable->call_number, number, AEECM_MAX_DIGITS_LENGTH);
                if(m_call_name[0] != 0)
                {
                    WSTRLCPY(pMe->m_CallsTable->call_name, m_call_name, MAX_SIZE_NAME_TEXT);
                    pMe->m_CallsTable->in_phonebook = TRUE;
                }
            }
            else
            {
                WSTRLCPY(pMe->m_CallsTable->call_number, m_call_number, AEECM_MAX_DIGITS_LENGTH);
            }

            pMe->m_CallsTable->call_id     = call_id;
            pMe->m_CallsTable->start_time  = ISHELL_GetUpTimeMS(pMe->m_pShell);//get current time
            pMe->m_CallsTable->call_type = call_type;
            pMe->m_CallsTable->type        = type;
            WSTRCPY(pMe->m_CallsTable->ringer, ringer);
            pMe->m_CallsTable->next        = NULL;
            return TRUE;
        }
        return FALSE;
    }
    return FALSE;
}
boolean CallApp_Get_CallName_In_Call_Table(CCallApp *pMe,AECHAR *name)
{
    Dialer_call_table *temp = NULL;

    CALL_FUN_START("CallApp_Get_CallName_In_Call_Table",0,0,0);

    temp = pMe->m_CallsTable;
    if(temp == NULL)
    {
        return FALSE;
    }

    if(temp->in_phonebook)
    {
        WSTRLCPY(name, temp->call_name ,MAX_SIZE_NAME_TEXT);
    }

    //else
    //    WSTRCPY(name, temp->call_number);//we need show name and number.
    return TRUE;
}

Dialer_call_table* CallApp_Get_First_Entry_In_Call_Table(CCallApp *pMe)
{
    Dialer_call_table *temp = NULL;

    if(pMe == NULL)
    {
        return NULL;
    }
    //CALL_FUN_START("CallApp_Get_First_Entry_In_Call_Table",0,0,0);

    temp = pMe->m_CallsTable;

    if(temp == NULL)
    {
        return NULL;
    }

    while(temp->next)
    {
        temp = temp->next;
    }

    return temp;

}

void CallApp_Free_All_Call_Table(CCallApp *pMe)
{
    Dialer_call_table * temp = NULL;

    CALL_FUN_START("CallApp_Free_All_Call_Table",0,0,0);

    if(pMe == NULL)
    {
      return;
    }
    if(pMe->m_CallsTable == NULL)
    {
        return;
    }
    while(pMe->m_CallsTable)
    {
        if(pMe->m_CallsTable->next)
        {
            temp = pMe->m_CallsTable->next;
        }

        CALLAPP_FREE(pMe->m_CallsTable);
        pMe->m_CallsTable = temp;
        temp = NULL;
    }
    pMe->m_CallsTable = NULL;
    pMe->m_CallsTable_Count =0;
    MEMSET((void*)&pMe->m_cont_cache , 0 ,sizeof(Dialer_cont_cache));
}

void CallApp_Change_Call_Table_Call_Start_Time(CCallApp *pMe,AECHAR *number)
{
    Dialer_call_table *temp = NULL;

    CALL_FUN_START("CallApp_Change_Call_Table_Call_Start_Time",0,0,0);

    if(pMe == NULL || number == NULL)
    {
        return;
    }

    temp = CallApp_Search_Number_In_Call_Table(pMe,number);

    if(temp)
    {
        temp->start_time = ISHELL_GetUpTimeMS(pMe->m_pShell);//get current time
    }
    else
    {
        CALL_ERR("we not find this number in call table,not chane call start time",0,0,0);
    }
}

void CallApp_Change_Call_Table_Call_End_Time(CCallApp *pMe,AECHAR *number)
{
    Dialer_call_table *temp = NULL;

    CALL_FUN_START("CallApp_Change_Call_Table_Call_End_Time",0,0,0);

    if(pMe == NULL || number == NULL)
    {
        return;
    }

    temp = CallApp_Search_Number_In_Call_Table(pMe,number);

    if(temp)
    {
        temp->end_time = ISHELL_GetUpTimeMS(pMe->m_pShell);//get current time
    }
    else
    {
        CALL_ERR("we not find this number in call table,not chane call end time",0,0,0);
    }
}

void CallApp_Reset_Call_Table_Call_End_Time(CCallApp *pMe,AECHAR *number)
{
    Dialer_call_table *temp = NULL;

    CALL_FUN_START("CallApp_Reset_Call_Table_Call_End_Time",0,0,0);

    if(pMe == NULL || number == NULL)
    {
        return;
    }

    temp = CallApp_Search_Number_In_Call_Table(pMe,number);

    if(temp)
    {
        temp->end_time = 0;//get current time
        temp->start_time = 0;
    }
    else
    {
        CALL_ERR("we not find this number in call table,not chane call end time",0,0,0);
    }
}

void CallApp_Change_Call_Table_All_Call_End_Time(CCallApp *pMe)
{
    Dialer_call_table *temp = NULL;

    CALL_FUN_START("CallApp_Change_Call_Table_All_Call_End_Time",0,0,0);

    if(pMe == NULL ||pMe->m_CallsTable == NULL)
    {
        return;
    }

    temp = pMe->m_CallsTable;

    while(temp)
    {
        temp->end_time = ISHELL_GetUpTimeMS(pMe->m_pShell);
        temp = temp->next;
    }
}

void CallApp_Change_Call_Table_Call_ID(CCallApp *pMe,AECHAR *number,uint8 call_id)
{
    Dialer_call_table *temp = NULL;

    CALL_FUN_START("CallApp_Change_Call_Table_Call_ID",0,0,0);

    if(pMe == NULL || number == NULL)
    {
        return;
    }

    CALL_PRINTF("Change Call ID %S",number);
    temp = CallApp_Search_Number_In_Call_Table(pMe,number);

    if(temp)
    {
        temp->call_id = call_id;
    }
    else
    {
        CALL_ERR("we not find this number in call table,not chane call_id",0,0,0);
    }
}

void CallApp_Change_Call_Table_Call_History_State(CCallApp *pMe,AECHAR *number,
                                                                                                        uint16 type/*uint16 status*/)
{
    Dialer_call_table *temp = NULL;

    CALL_FUN_START("CallApp_Change_Call_Table_Call_History_State",0,0,0);

    if(pMe == NULL || number == NULL)
    {
        return;
    }
    temp = CallApp_Search_Number_In_Call_Table(pMe,number);

    if(temp)
    {
        //temp->call_status = status;
        temp->call_type = type;
    }
    else
    {
        CALL_ERR("we not find this number in call table,not chane call_type",0,0,0);
    }
}

#if 0
void CallApp_Init_Msgbox_String(CCallApp *pMe,uint16 str_id,AECHAR * pach_string)
{
    CALL_FUN_START("CallApp_Init_Msgbox_String %d",str_id,0,0);

    if(pMe->m_msg_text != NULL)
    {
        CALLAPP_FREE(pMe->m_msg_text);
        pMe->m_msg_text = NULL;
    }
    pMe->m_msg_text_id = str_id;
    pMe->m_msg_text= (AECHAR*)CALLAPP_MALLOC(sizeof(AECHAR)*MSGBOX_MAXTEXTLEN);

    if(pMe->m_msg_text == NULL)
    {
        //CALL_ERR("MALLOC memory fail",0,0,0);
        return;
    }

    MEMSET(pMe->m_msg_text, 0, MSGBOX_MAXTEXTLEN);

    (void) ISHELL_LoadResString(pMe->m_pShell,
                                            AEE_APPSCALLAPP_RES_FILE,
                                            str_id,
                                            pMe->m_msg_text,
                                            sizeof(AECHAR)*MSGBOX_MAXTEXTLEN);
    CALL_PRINTF("pMe->m_msg_text2 = %S ",pMe->m_msg_text);
    if(pach_string)
    {
        AECHAR     szText[MSGBOX_MAXTEXTLEN] = {0};

        CALL_PRINTF("pach_string = %S",pach_string);

        WSTRLCPY(szText, pMe->m_msg_text, MSGBOX_MAXTEXTLEN);
        MEMSET(pMe->m_msg_text, 0, MSGBOX_MAXTEXTLEN);

        WSPRINTF(pMe->m_msg_text, sizeof(AECHAR)*MSGBOX_MAXTEXTLEN, szText, pach_string);
    }
}
#endif
void CallApp_ShowMsgBox(CCallApp *pMe, uint16 str_id)
{
    pMe->m_msg_text_id = str_id;
   
    // 创建消息对话框.
    CallApp_ShowDialog(pMe, IDD_MSGBOX);
}

void CallApp_Add_All_Call_History(CCallApp *pMe)
{
    Dialer_call_table *temp = NULL;
    uint32 call_length = 0;

    CALL_FUN_START("CallApp_Add_All_Call_History",0,0,0);

    if(pMe == NULL||pMe->m_CallsTable == NULL)
    {
        return;
    }

    temp = pMe->m_CallsTable;

    while(temp)
    {
        if(temp->call_type == AEECALLHISTORY_CALL_TYPE_MISSED)//(temp->call_status ==CALLHISTORY_MISSED)
        {
            call_length = 0;
        }
        else
        {
            if(temp->end_time < temp->start_time)
            {
                temp->end_time = ISHELL_GetUpTimeMS(pMe->m_pShell);
            }

            if(temp->end_time < temp->start_time)
            {
                call_length = 0;
            }
            else
            {
                call_length = temp->end_time - temp->start_time;
            }

        }
        CallApp_Add_OneCall_To_History(pMe,temp->call_number,temp->type,
                                                            temp->call_name,temp->call_type/*temp->call_status*/,call_length);
        temp = temp->next;
    }
}

/*=============================================================================
FUNCTION: CallApp_Add_OneCall_To_History

DESCRIPTION:

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void CallApp_Add_OneCall_To_History(CCallApp       *pMe,
                                             AECHAR           *number,
                                             callPIType       pi,
                                             AECHAR           *name,
                                             uint16           call_type,
                                             uint32           call_length)
{
    ICallHistory    *pCallHistory = NULL;
    AEECallHistoryEntry *pEntry = NULL;
    AEECallHistoryEntry *pNewEntry = NULL;
    char                *pInNumber = NULL;
    int                 nSize=0, nLen=0;
    int                 i,nFields,nRet = 0;
    uint32            nDurationS = call_length/1000;
    boolean             bHasNumber = FALSE;
    boolean             bHasName = FALSE;
    uint16              nCount = 1;
    if (ISHELL_CreateInstance(pMe->m_pShell,
                              AEECLSID_CALLHISTORY,
                              (void **) &pCallHistory))
    {
        return;
    }

    if ((NULL != number) && (WSTRLEN(number)>0))
    {
/*    
#if defined(FEATURE_VERSION_C337) 
        {
        	if((WSTRLEN(number)>=4) && WSTRNCMP(number,L"+91",3)==0)
        	{
        		AECHAR   wstrNumberbuf[AEECALLHISTORY_MAXDIGITS] = {0};
        		WSTRLCAT(wstrNumberbuf,&number[3],AEECALLHISTORY_MAXDIGITS);
        		WSTRLCPY(number,wstrNumberbuf,AEECALLHISTORY_MAXDIGITS);
        	}
			else	if((WSTRLEN(number)>=4) && WSTRNCMP(number,L"091",3)==0) 	
        	{
        		AECHAR   wstrNumberbuf[AEECALLHISTORY_MAXDIGITS] = {0};
        		WSTRLCAT(wstrNumberbuf,&number[3],AEECALLHISTORY_MAXDIGITS);
        		WSTRLCPY(number,wstrNumberbuf,AEECALLHISTORY_MAXDIGITS);
        	}				
			else	if((WSTRLEN(number)>=5) && WSTRNCMP(number,L"0091",4)==0) 	
        	{
        		AECHAR   wstrNumberbuf[AEECALLHISTORY_MAXDIGITS] = {0};
        		WSTRLCAT(wstrNumberbuf,&number[4],AEECALLHISTORY_MAXDIGITS);
        		WSTRLCPY(number,wstrNumberbuf,AEECALLHISTORY_MAXDIGITS);
        	}								
        }												
#endif
*/
		
        nSize = (WSTRLEN(number)+1)*sizeof(char);
        pInNumber = (char *)MALLOC(nSize);
        if (NULL == pInNumber)
        {
            goto ADDENTRY_EXIT;
        }
        (void)WSTRTOSTR(number, pInNumber, nSize);
        bHasNumber = TRUE;
        
        // 检查号码的有效性
        for (i=STRLEN(pInNumber)-1; i>=0; i--)
        {
            if ((pInNumber[i] < '0' || pInNumber[i] > '9') &&
                (pInNumber[i] != '*') &&
                (pInNumber[i] != '#') &&
                (pInNumber[i] != '+'))
            {
                bHasNumber = FALSE;
                break;
            }
        }
        
        if (bHasNumber == FALSE)
        {
            FREEIF(pInNumber);
        }
    }
    
    // 一条记录目前我们使用的字段：号码|人名|PI|时间|通话时长|呼叫类型|同类型同号码呼叫计数
    // 其中，号码、人名是否出现可选, 因此一条记录的最小字段数为 7-2=5
    nFields = 5;
    if ((NULL != name) && (WSTRLEN(name)>0))
    {
        bHasName = TRUE;
        nFields++;
    }
    if (bHasNumber)
    {
        nFields++;
    }
    
    // 开始构建 AEECallHistoryEntry 结构
    pNewEntry = (AEECallHistoryEntry *)MALLOC(sizeof(AEECallHistoryEntry));
    if (NULL == pNewEntry)
    {
        goto ADDENTRY_EXIT;
    }
	
    pNewEntry->wNumFields = nFields;
    pNewEntry->pFields = (AEECallHistoryField *)MALLOC(sizeof(AEECallHistoryField)*nFields);
    if (NULL == pNewEntry->pFields)
    {
        goto ADDENTRY_EXIT;
    }
    
    i=0;
    // 人名
    if (bHasName)
    {
        pNewEntry->pFields[i].wID = AEECALLHISTORY_FIELD_NAME;
        pNewEntry->pFields[i].wDataLen = WSTRLEN(name) * sizeof(AECHAR);
        pNewEntry->pFields[i].pData = (void *)MALLOC(pNewEntry->pFields[i].wDataLen);
        if (NULL == pNewEntry->pFields[i].pData)
        {
            goto ADDENTRY_EXIT;
        }
        MEMCPY(pNewEntry->pFields[i].pData, name, pNewEntry->pFields[i].wDataLen);
        i++;
    }
    
    // 号码
    if (bHasNumber)
    {
        pNewEntry->pFields[i].wID = AEECALLHISTORY_FIELD_NUMBER;
        pNewEntry->pFields[i].wDataLen = STRLEN(pInNumber) * sizeof(char);
        pNewEntry->pFields[i].pData = (void *)MALLOC(pNewEntry->pFields[i].wDataLen);
        if (NULL == pNewEntry->pFields[i].pData)
        {
            goto ADDENTRY_EXIT;
        }
        MEMCPY(pNewEntry->pFields[i].pData, pInNumber, pNewEntry->pFields[i].wDataLen);
        i++;
    }
    
    // PI
    pNewEntry->pFields[i].wID = AEECALLHISTORY_FIELD_PI;
    pNewEntry->pFields[i].wDataLen = sizeof(AEETPI);
    pNewEntry->pFields[i].pData = (void *)MALLOC(pNewEntry->pFields[i].wDataLen);
    if (NULL == pNewEntry->pFields[i].pData)
    {
        goto ADDENTRY_EXIT;
    }
    else
    {
        *((AEETPI *)pNewEntry->pFields[i].pData) = pi;
        i++;
    }
    
    // 通话开始时间
    pNewEntry->pFields[i].wID = AEECALLHISTORY_FIELD_DATE_TIME;
    pNewEntry->pFields[i].wDataLen = sizeof(uint32);
    pNewEntry->pFields[i].pData = (void *)MALLOC(pNewEntry->pFields[i].wDataLen);
    if (NULL == pNewEntry->pFields[i].pData)
    {
        goto ADDENTRY_EXIT;
    }
    else
    {
        *((uint32 *)pNewEntry->pFields[i].pData) = GETTIMESECONDS()- nDurationS;
        i++;
    }
    
    // 通话持续时间
    pNewEntry->pFields[i].wID = AEECALLHISTORY_FIELD_CALL_DURATION;
    pNewEntry->pFields[i].wDataLen = sizeof(uint32);
    pNewEntry->pFields[i].pData = (void *)MALLOC(pNewEntry->pFields[i].wDataLen);
    if (NULL == pNewEntry->pFields[i].pData)
    {
        goto ADDENTRY_EXIT;
    }
    else
    {
        *((uint32 *)pNewEntry->pFields[i].pData) = nDurationS;
        i++;
    }
    
    // 通话记录类型
    pNewEntry->pFields[i].wID = AEECALLHISTORY_FIELD_CALL_TYPE;
    pNewEntry->pFields[i].wDataLen = sizeof(uint16);
    pNewEntry->pFields[i].pData = (void *)MALLOC(pNewEntry->pFields[i].wDataLen);
    if (NULL == pNewEntry->pFields[i].pData)
    {
        goto ADDENTRY_EXIT;
    }
    else
    {
    	MSG_FATAL("2....nDurationS:::::::::::::%d",nDurationS,0,0);
        *((uint16 *)pNewEntry->pFields[i].pData) = call_type;
        i++;
    }
    
    // 同类型、同号码呼叫计数
    pNewEntry->pFields[i].wID = AEECALLHISTORY_FIELD_COUNTER;
    pNewEntry->pFields[i].wDataLen = sizeof(uint16);
    pNewEntry->pFields[i].pData = (void *)MALLOC(pNewEntry->pFields[i].wDataLen);
    if (NULL == pNewEntry->pFields[i].pData)
    {
        goto ADDENTRY_EXIT;
    }
    else
    {
        *((uint16 *)pNewEntry->pFields[i].pData) = 1;
        i++;
    }
	
    if ((pi == PI_ALLOWED) && (NULL != pInNumber))
    {   // 查找是否存在同号码记录
        boolean     bRecordExisted = FALSE;
        
        if (ICALLHISTORY_EnumInitByCallType(pCallHistory/*pMe->m_pCallHistory*/, call_type) == SUCCESS)
        {
            pEntry = (AEECallHistoryEntry *)ICALLHISTORY_EnumNext(pCallHistory, &nRet);
        }
		
        nLen = STRLEN(pInNumber);		
		
        while (NULL != pEntry)
        {        	
            for (i=0; i<pEntry->wNumFields; i++)
            {
                if (pEntry->pFields[i].wID == AEECALLHISTORY_FIELD_NUMBER)
                {                	
                    if ((pEntry->pFields[i].pData != NULL) &&
                        (nLen == pEntry->pFields[i].wDataLen))
                    {                    	
						
                        // 判断已有记录号码与待添加号码是否匹配
                        if (STRNCMP(pInNumber, (char *)pEntry->pFields[i].pData, nLen) == 0)
                        {// 存在号码相同、呼叫类型相同的记录，对此我们只需更新该记录                        	
                            bRecordExisted = TRUE;
                            break;
                        }
                    }
                }
            }
            
            if (bRecordExisted)
            {
                break;
            }
            pEntry = (AEECallHistoryEntry *)ICALLHISTORY_EnumNext(pCallHistory, &nRet);
        }
    }
    
    if (NULL != pEntry)
    {// 更新记录
        for (i=0; i<pEntry->wNumFields; i++)
        {
            if ((pEntry->pFields[i].wID == AEECALLHISTORY_FIELD_COUNTER) &&
                (pEntry->pFields[i].pData != NULL))
            {
                nCount = *((uint16 *)pEntry->pFields[i].pData);
            }
        }
        *((uint16 *)pNewEntry->pFields[nFields-1].pData) = nCount+1;     

        // 删除记录
        (void)ICALLHISTORY_ClearEntry(pCallHistory);
    }
    // 添加新记录
    (void)ICALLHISTORY_AddEntry(pCallHistory, pNewEntry);
    
    //添加通话时长到Config项
    if (nDurationS > 0)
    {
    	MSG_FATAL("3....nDurationS:::::::::::::%d",nDurationS,0,0);
        CallApp_AddToCFGCallTimer(pMe, call_type, nDurationS);
    }

ADDENTRY_EXIT:
    FREEIF(pInNumber);
    if (NULL != pNewEntry)
    {// 注意此结构的释放顺序
        if (NULL != pNewEntry->pFields)
        {
            for (i=0; i<pNewEntry->wNumFields; i++)
            {
                FREEIF(pNewEntry->pFields[i].pData);
            }
            
            FREEIF(pNewEntry->pFields);
        }
        FREEIF(pNewEntry);
    }
    if (pCallHistory != NULL)
    {
        ICALLHISTORY_Release(pCallHistory);
        pCallHistory = NULL;
    }
}

void CallApp_DrawTextWithProfile(IShell* pShell,
                                        IDisplay * pDisplay,
                                        RGBVAL ProfileColor,
                                        AEEFont Font,
                                        const AECHAR * pcText,
                                        int nChars,
                                        int x,
                                        int y,
                                        const AEERect * prcBackground,
                                        uint32 dwFlags
                                        )
{
    DrawTextWithProfile( pShell,
                                        pDisplay,
                                        ProfileColor,
                                        Font,
                                        pcText,
                                        nChars,
                                        x,
                                        y,
                                        prcBackground,
                                        dwFlags
                                        );
}

static int CallApp_Start(CCallApp *pMe)
{
    AEEFontInfo myInfo = {0,0};
    
    (void)ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_FONTSYSBIGNUMBER, (void **)&pMe->m_pBigNumFont);
    (void)ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_FONTSYSNORMAL, (void **)&pMe->m_pNormalNumFont);
    
    if(pMe->m_pBigNumFont)
    {
        IFONT_GetInfo(pMe->m_pBigNumFont, &myInfo, sizeof(myInfo));
        pMe->m_large_Num_Height = myInfo.nAscent+myInfo.nDescent;
    }
    
    if(pMe->m_pNormalNumFont)
    {
        IFONT_GetInfo(pMe->m_pNormalNumFont, &myInfo, sizeof(myInfo));
        pMe->m_Normal_Num_Height = myInfo.nAscent+myInfo.nDescent;
    }
    return SUCCESS;
}

static void CallApp_Stop(CCallApp *pMe)
{
    if( pMe->m_pBigNumFont )
    {
        IFONT_Release(pMe->m_pBigNumFont);
        pMe->m_pBigNumFont = NULL;
    }
    
    if( pMe->m_pNormalNumFont )
    {
        IFONT_Release(pMe->m_pNormalNumFont);
        pMe->m_pNormalNumFont = NULL;
    }
}

#ifdef DIALER_MEM_CHECK
void CallApp_show_Mem(CCallApp *pMe)
{
    IHeap    *heap = NULL;
    uint32     memsize= 0;
    AEEDeviceInfo  di;
    CALL_FUN_START("CallApp_show_Mem",0,0,0);
    ISHELL_GetDeviceInfo(pMe->m_pShell, &di);
    (void)ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_HEAP, (void **)&heap);

    if (NULL == heap)
    {
        CALL_ERR("MP: Error: Failed to create IHeap instance",0,0,0);
        return;
    }
    memsize = IHEAP_GetMemStats(heap);
    CALL_ERR("IHEAP_GetMemStats %d total = %d",memsize,di.dwRAM,0);
    IHEAP_Release(heap);
    heap = NULL;
}
#endif
void * CallApp_Malloc(uint32 size, int line)
{
    void *p;
    if (!size)
    {
        return NULL;
    }
    if((p = OEM_Malloc(size)) == NULL)
    {
        CALL_ERR("Failed to allocate memory %d in line %d, total size %d", size, line, 0);
        return NULL;
    }
    CALL_ERR("CallApp_Malloc",0,0,0);
    (void) MEMSET(p, 0, size);

    return p;
}
void CallApp_Free(void *p)
{
    if (p)
    {
        /*lint -e534 */
        OEM_Free(p);
        CALL_ERR("CallApp_Free",0,0,0);
        /*lint +e534 */
    }
}
#if 0
#ifdef DIALER_DUBEG
void CallApp_Printf(const char * file_name,AECHAR * string1,AECHAR * string2)
{
    int len1 = 0;
    int len2 = 0;
    char * m_local1 = NULL;
    char * m_local2 = NULL;
    CALL_FUN_START("CallApp_Printf",0,0,0);
    if(file_name == NULL)
    {
        ERR("FILE NAME IS NULL",0,0,0);
        return;
    }

    if(!((string1  == NULL)||(string2  == NULL)))
    {
        ERR("STRING IS NULL",0,0,0);
        return;
    }

    if(string1)
    {
        len1 = WSTRLEN(string1);
        len1 = len1 +1;
        m_local1 = (char *)MALLOC(len1);
        if(m_local1)
        {
            WSTRTOSTR(string1,m_local1,len1);
        }
        else
        {
            return;
        }
    }
    if(string2)
    {
        len2 = WSTRLEN(string2);
        len2 = len2 +1;
        m_local2 = (char *)MALLOC(len2);
        if(m_local2)
        {
            WSTRTOSTR(string2,m_local2,len2);
        }
        else
        {
            if(m_local1)
            {
                FREE(m_local1);
            }
            return;
        }
    }

    if(m_local1)
    {
        //MSG_SPRINTF_1(MSG_SSID_DFLT,MSG_LEGACY_MED, "DIALER %s", m_local1);
        MSG_SPRINTF_2(MSG_SSID_DFLT,MSG_LEGACY_HIGH, "%s DIALER %s para1",m_local1,file_name);
        FREE(m_local1);
        m_local1 = NULL;
    }

    if(m_local2)
    {
        //MSG_SPRINTF_1(MSG_SSID_DFLT,MSG_LEGACY_MED, "DIALER %s", m_local2);
        MSG_SPRINTF_2(MSG_SSID_DFLT,MSG_LEGACY_HIGH, "%s DIALER %s para2",m_local2,file_name);
        FREE(m_local2);
        m_local2 = NULL;
    }
}
#endif
#endif

static int CallApp_Make_IP_Call_F(ICallApp *p, AECHAR *number)
{
#ifdef FEATRUE_SET_IP_NUMBER
    register CCallApp *pMe = (CCallApp *)p;
    //static call_start_info_type  call_start_info_call;

    CALL_FUN_START("CallApp_Make_IP_Call_F",0,0,0);

    if (NULL == pMe || NULL == number)
    {
        return EFAILED;
    }

    if (0 == *number)
    {
        return EFAILED;
    }

    if (ISHELL_ActiveApplet(pMe->m_pShell) == AEECLSID_DIALER)
    {
        // Make directly speeddial call
        WSTRLCPY(pMe->m_DialString, number , MAX_SIZE_DIAL_STR);
        MOVE_TO_STATE(STATE_SEL_IP_NUMBER)
        return SUCCESS;
    }

    call_start_info_call.b_closeAll = CALL_INFO_NO;
    call_start_info_call.catgory = CALL_INFO_IP_CALL;
    WSTRTOSTR(number,call_start_info_call.dial_str, sizeof(call_start_info_call.dial_str));

    if (ISHELL_StartAppletArgs(pMe->m_pShell,AEECLSID_DIALER,(const char *)&call_start_info_call) != SUCCESS)
    {
        CALL_ERR("^&^& Start AEECLSID_DIALER fail", 0,0,0);
        return EFAILED;
    }

    return SUCCESS;
#else
    return EFAILED;
#endif
}

static int CallApp_VoiceCallDial_F(ICallApp *p, AECHAR *number)
{
    static AECHAR number_to_dial[MAX_SIZE_DIAL_STR];
    CCallApp *pMe = (CCallApp *)p;
    if(pMe == NULL)
    {
        return EFAILED;
    }

    if((pMe->m_pShell == NULL)||(number == NULL))
    {
        return EFAILED;
    }
    call_start_info_call.b_closeAll = CALL_INFO_NO;
    call_start_info_call.catgory = CALL_INFO_DIAL;
    WSTRTOSTR(number, call_start_info_call.dial_str, sizeof(call_start_info_call.dial_str));

    if (ISHELL_StartAppletArgs(pMe->m_pShell, AEECLSID_DIALER, (const char *)&call_start_info_call) != SUCCESS)
    {
        CALL_ERR("^&^& Start AEECLSID_CALL fail, try 500 ms later", 0,0,0);
        WSTRLCPY(number_to_dial, number ,MAX_SIZE_DIAL_STR);
        ISHELL_SetTimer(pMe->m_pShell, 500, CallApp_CB_MakeVoiceCallAfterCloseAll, number_to_dial);
        return EFAILED;
    }

    return SUCCESS;
    }


static void CallApp_CB_MakeVoiceCallAfterCloseAll(void *pUser)
{
    AECHAR *number = (AECHAR *)pUser;
    IShell *pShell = AEE_GetShell();
    if(pShell == NULL)
    {
        return;
    }

    call_start_info_call.b_closeAll = CALL_INFO_NO;
    call_start_info_call.catgory = CALL_INFO_SEND;

    WSTRTOSTR(number, call_start_info_call.dial_str, sizeof(call_start_info_call.dial_str));

    CALL_ERR("CB_MakeVoiceCallAfterCloseAll", 0,0,0);
    // Start origination
    if (ISHELL_StartAppletArgs(pShell, AEECLSID_DIALER, (const char *)&call_start_info_call) != SUCCESS)
    {
        CALL_ERR("^&^& Start AEECLSID_CALL fail", 0,0,0);
    }
}

#ifdef FEATURE_UTK2
static int CallApp_Make_UTK_Call_F(ICallApp *p)
{
    CCallApp *pMe = (CCallApp *)p;
    if(pMe == NULL)
    {
        return EFAILED;
    }

    if(pMe->m_pShell == NULL)
    {
        return EFAILED;
    }
    call_start_info_call.b_closeAll = CALL_INFO_NO;
    call_start_info_call.catgory = CALL_INFO_UTK;

    pUTKCALLinfo = &call_start_info_call;    
    if (ISHELL_StartAppletArgs(pMe->m_pShell, AEECLSID_DIALER, (const char *)&call_start_info_call) != SUCCESS)
    {
        CALL_ERR("^&^& Start AEECLSID_CALL fail, try 500 ms later", 0,0,0);
        pUTKCALLinfo = NULL;
        return EFAILED;
    }
    return SUCCESS;    
}
#endif //FEATURE_UTK2

#ifdef FEATURE_LED_CONTROL
#if 0
static void CallApp_DisableLedSig( CCallApp *pMe)
{

    IBACKLIGHT_SigLedDisable(pMe->m_pBacklight);    //cancel the previous LED control

    if( pMe->m_MissedCallCount > 0)
    {
        IBACKLIGHT_SigLedEnable(pMe->m_pBacklight,SIG_LED_MISSED_CALL);
    }
}
#endif
#endif

static void CallApp_Settime_To_EndCall(CCallApp *pMe)
{
#ifdef FEATURE_ICM
    ICM_EndAllCalls(pMe->m_pICM);
#else
    ICALLMGR_EndAllCalls(pMe->m_pICallMgr);
#endif
}

//static void CallApp_Process_Batty_Msg(CCallApp  *pMe, uint16  msg_id)
//{
//    if(pMe->m_pActiveDlgID == IDD_NUMEDIT ||
//        pMe->m_pActiveDlgID == IDD_CONVERSATION ||
//        pMe->m_pActiveDlgID == IDD_MISSED
//        )
//    {
//        CallApp_Init_Msgbox_String(pMe, msg_id, NULL);
//        CLOSE_DIALOG(DLGRET_MSGBOX)
//    }
//}

static void CallApp_Conver_Buf(AECHAR *buf)
{
    int len = 0;
    char  temp1[AEECM_MAX_ALPHA_TAG_LENGTH] = {0};
    char  temp2[AEECM_MAX_ALPHA_TAG_LENGTH] = {0};
    int i = 0;
    int j = 0;
    if(buf == NULL)
    {
        return;
    }

    len = WSTRLEN(buf);

    if(len > AEECM_MAX_ALPHA_TAG_LENGTH)
    {
        len = AEECM_MAX_ALPHA_TAG_LENGTH;
    }

    WSTRTOSTR(buf, temp1,sizeof(temp1));
    for(i = 0; i <STRLEN(temp1); i++)
    {
        if(temp1[i]>0x1f && temp1[i]<0x7a)
        {
            temp2[j] = temp1[i];
            j ++;
        }
        temp2[j] = '\0';
    }
    STRTOWSTR(temp2, buf, AEECM_MAX_ALPHA_TAG_LENGTH);                        

}
#ifdef FEATURE_SUPPORT_BT_APP
boolean bt_ui_process_vol_change(byte vol)
{
    int app_id;
    tBTUI_AG_CB *bt_ui_ag_cb = bcmapp_ag_get_ag_cb();
    tBTA_AG_RES_DATA    data;
    CALL_ERR("bt_ui_process_vol_change %d %d %d",bt_ui_ag_cb->app[BTUI_AG_ID_1].is_open,bt_ui_ag_cb->app[BTUI_AG_ID_2].is_open,vol);
    if(vol <1 || vol >15)
    {
        return FALSE;
    }

    for (app_id = 0; app_id < BTUI_AG_NUM_APP; app_id++)
    {
        if(bt_ui_ag_cb->app[app_id].is_open)
        {
            data.num = bcmapp_ag_get_vol(vol);
            data.ok_flag = BTA_AG_OK_DONE;
            BTA_AgResult(bt_ui_ag_cb->app[app_id].handle, BTA_AG_SPK_RES, &data);
            BTA_AgResult(bt_ui_ag_cb->app[app_id].handle, BTA_AG_MIC_RES, &data);
        }
    }

    if(app_id < BTUI_AG_NUM_APP)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
boolean bt_ui_process_cmcall_notify(CCallApp *pMe,uint32 event/*,void  *pCallInfo*/,boolean cm_event)
{
    tBTA_AG_RES_DATA data;
    //AEECMCallEventData  *CallInfo = (AEECMCallEventData  *)pCallInfo;
    int xx;
    tBTUI_AG_CB *bt_ui_ag_cb = bcmapp_ag_get_ag_cb();
    static boolean b_incoming = TRUE;
    CALL_ERR("bt_ui_process_cmcall_notify %x %d %d",event,bt_ui_ag_cb->app[BTUI_AG_ID_1].is_open,bt_ui_ag_cb->app[BTUI_AG_ID_2].is_open);
    for (xx = 0; xx < BTUI_AG_NUM_APP; xx++)
    {
        if(bt_ui_ag_cb->app[xx].is_open)
        {
            break;
        }
        return FALSE;/*if no ag open ,we don't process cm event*/
    }

    switch(event)
    {
#ifdef FEATURE_ICM
        case AEECM_EVENT_CALL_ORIG:       /* phone originated a call */
#else
        case AEET_EVENT_CALL_ORIG:       /* phone originated a call */
#endif
            bt_ui_ag_cb->call_state = BTUI_AG_CALL_OUT;
            bt_ui_ag_cb->ind[BTUI_AG_IND_CALL]   = BTUI_AG_CALL_IND_ACTIVE;
            bt_ui_ag_cb->ind[BTUI_AG_IND_SETUP] =  BTUI_AG_CALLSETUP_ALERTING;
            data.audio_handle = bt_ui_ag_cb->call_handle;
            b_incoming = FALSE;
            pMe->m_bt_audio = SEND_BT_CALL;
            if(!cm_event)
            {
                bta_ag_setpath(FALSE);
                BTA_AgResult(BTA_AG_HANDLE_ALL, BTA_AG_OUT_CALL_ORIG_RES, &data);
                BTA_AgResult(BTA_AG_HANDLE_ALL, BTA_AG_OUT_CALL_ALERT_RES, &data);
            }
            break;
#ifdef FEATURE_ICM
        case AEECM_EVENT_CALL_ANSWER:     /* Incoming call was answered */
#else
        case AEET_EVENT_CALL_ANSWER:     /* Incoming call was answered */
#endif
            //if (bt_ui_ag_cb->call_handle != BTA_AG_HANDLE_NONE)
            //{
            //    data.audio_handle = bt_ui_ag_cb->call_handle;
            //    BTA_AgResult(BTA_AG_HANDLE_ALL, BTA_AG_IN_CALL_CONN_RES, &data);
            //}
            break;
#ifdef FEATURE_ICM
        case AEECM_EVENT_CALL_END:        /* Originated/incoming call was ended */
#else
        case AEET_EVENT_CALL_END:        /* Originated/incoming call was ended */
#endif
            bt_ui_ag_cb->call_state = BTUI_AG_CALL_NONE;
            bt_ui_ag_cb->ind[BTUI_AG_IND_SETUP] =BTUI_AG_CALLSETUP_NONE;
            bt_ui_ag_cb->ind[BTUI_AG_IND_CALL] = BTUI_AG_CALL_IND_INACTIVE;
            bt_ui_ag_cb->ind[BTUI_AG_IND_HELD] = '0';
            BTA_AgResult(BTA_AG_HANDLE_ALL, BTA_AG_END_CALL_RES, NULL);
            break;
#ifdef FEATURE_ICM
        case AEECM_EVENT_CALL_INCOM:      /* phone received an incoming call */
#else
        case AEET_EVENT_CALL_INCOM:      /* phone received an incoming call */
#endif
            ERR("AEECM_EVENT_CALL_INCOM",0,0,0);
            b_incoming = TRUE;
            pMe->m_bt_audio = SEND_BT_CALL;
            bt_ui_ag_cb->call_state = BTUI_AG_CALL_INC;
            //bt_ui_ag_cb->ind[BTUI_AG_IND_CALL]   = BTUI_AG_CALL_IND_ACTIVE;
            bt_ui_ag_cb->ind[BTUI_AG_IND_SETUP] = BTUI_AG_CALLSETUP_INCOMING;
            break;
#ifdef FEATURE_ICM
        case AEECM_EVENT_CALL_CALLER_ID:  /* Caller ID info was received from BS */
#else
        case AEET_EVENT_CALL_CALLER_ID:  /* Caller ID info was received from BS */
#endif
            bt_ui_ag_cb->call_state = BTUI_AG_CALL_INC;
            //bt_ui_ag_cb->ind[BTUI_AG_IND_CALL]   = BTUI_AG_CALL_IND_ACTIVE;
            bt_ui_ag_cb->ind[BTUI_AG_IND_SETUP] = BTUI_AG_CALLSETUP_INCOMING;
#ifdef FEATURE_ICM
            pMe->m_b_incall = AEECM_IS_VOICECALL_CONNECTED(pMe->m_pICM);
#else
            pMe->m_b_incall = AEET_IS_VOICECALL_CONNECTED(pMe->m_pITelephone);
#endif
            MSG_FATAL("AEECM_EVENT_CALL_CALLER_ID %d",pMe->m_b_incall,0,0);
            b_incoming = TRUE;
            data.str[0]='\"';

            WSTRTOSTR(pMe->m_CallsTable->call_number/* CallInfo->call_info.other_party_no*/,&data.str[1],BTA_AG_AT_MAX_LEN-6);
            data.str[BTA_AG_AT_MAX_LEN-5]='\0';	  
            STRCAT(data.str,"\",129");
            if(!cm_event)
            {
                bta_ag_setpath(FALSE);
                if(pMe->m_b_incall)
                {
                    BTA_AgResult(BTA_AG_HANDLE_ALL, BTA_AG_CALL_WAIT_RES, &data);
                }
                else
                {
                    BTA_AgResult(BTA_AG_HANDLE_ALL, BTA_AG_IN_CALL_RES, &data);
                }
            }
            break;
#ifdef FEATURE_ICM
        case AEECM_EVENT_CALL_SIGNAL:       /* signal info was received from BS */
#else
        case AEET_EVENT_CALL_SIGNAL:       /* signal info was received from BS */
#endif
            //data.audio_handle = bt_ui_ag_cb->call_handle;
            //data.state = TRUE;
            //BTA_AgResult(bt_ui_ag_cb->call_handle, BTA_AG_INBAND_RING_RES, &data);
            break;
#ifdef FEATURE_ICM
        case AEECM_EVENT_CALL_CONNECT: /* Originated/incoming call was connected */
#else
        case AEET_EVENT_CALL_CONNECT: /* Originated/incoming call was connected */
#endif
            ERR("AEECM_EVENT_CALL_CONNECT %d",b_incoming,0,0);
            pMe->m_b_add_btag_menu = TRUE;
            bt_ui_ag_cb->call_state = BTUI_AG_CALL_ACTIVE;
            bt_ui_ag_cb->ind[BTUI_AG_IND_SETUP] = BTUI_AG_CALLSETUP_NONE;
            bt_ui_ag_cb->ind[BTUI_AG_IND_CALL] = BTUI_AG_CALL_IND_ACTIVE;
            data.audio_handle = bt_ui_ag_cb->call_handle;
            if(b_incoming)
            {
                BTA_AgResult(BTA_AG_HANDLE_ALL, BTA_AG_IN_CALL_CONN_RES, &data);
            }
            else
            {
                BTA_AgResult(BTA_AG_HANDLE_ALL, BTA_AG_OUT_CALL_CONN_RES, &data);
            }
            break;
#ifdef FEATURE_ICM
        case AEECM_EVENT_CALL_ERROR:
#else
        case AEET_EVENT_CALL_ERROR:
#endif
            bt_ui_ag_cb->call_state = BTUI_AG_CALL_NONE;
            bt_ui_ag_cb->ind[BTUI_AG_IND_SETUP] =BTUI_AG_CALLSETUP_NONE;
            bt_ui_ag_cb->ind[BTUI_AG_IND_CALL] = BTUI_AG_CALL_IND_INACTIVE;
            data.audio_handle = bt_ui_ag_cb->call_handle;
            BTA_AgResult(BTA_AG_HANDLE_ALL, BTA_AG_CALL_CANCEL_RES, &data);
            break;

        default:
            break;
    }
    return TRUE;
}

void bt_ui_process_incall_org_state(void)
{
    tBTA_AG_RES_DATA data;
    tBTUI_AG_CB *bt_ui_ag_cb = bcmapp_ag_get_ag_cb();
    ERR("bt_ui_process_incall_org_state",0,0,0);
    bt_ui_ag_cb->ind[BTUI_AG_IND_SETUP] = BTUI_AG_CALLSETUP_OUTGOING;	// BTA_AG_CALLSETUP_ALERTING
    bt_ui_ag_cb->ind[BTUI_AG_IND_HELD] = '1';
    data.ind.id = BTA_AG_IND_CALLHELD;
    data.ind.value = bt_ui_ag_cb->ind[BTUI_AG_IND_HELD] - '0';
    BTA_AgResult(BTA_AG_HANDLE_ALL, BTA_AG_IND_RES, &data);/*+CIEV: (callheld=1)*/

    //data.ind.id = BTA_AG_IND_CALLSETUP;
    //data.ind.value = bt_ui_ag_cb->ind[BTUI_AG_IND_SETUP] - '0';
    //BTA_AgResult(BTA_AG_HANDLE_ALL, BTA_AG_IND_RES, &data);

    /* send originating and alerting */
    data.audio_handle = bt_ui_ag_cb->call_handle;
    BTA_AgResult(BTA_AG_HANDLE_ALL, BTA_AG_OUT_CALL_ORIG_RES, &data);/*+CIEV: (callheld=1)*/
    BTA_AgResult(BTA_AG_HANDLE_ALL, BTA_AG_OUT_CALL_ALERT_RES, &data);/*+CIEV: (callheld=1)*/
    BTA_AgResult(BTA_AG_HANDLE_ALL, BTA_AG_OUT_CALL_CONN_RES, &data);/*+CIEV: (callheld=1)*/
}
#endif
static int CallApp_BtCallNumber(ICallApp *p, AECHAR *number)
{
#ifdef FEATURE_SUPPORT_BT_APP
    register CCallApp *pMe = (CCallApp *)p;
    //static call_start_info_type  call_start_info_call;

    CALL_FUN_START("CallApp_BtCallNumber",0,0,0);

    if (NULL == pMe || NULL == number)
    {
        return EFAILED;
    }

    if (0 == *number)
    {
        return EFAILED;
    }

    if (ISHELL_ActiveApplet(pMe->m_pShell) == AEECLSID_DIALER/*AEECLSID_CALL*/)
    {
        // Make directly speeddial call
        WSTRLCPY(pMe->m_DialString, number ,MAX_SIZE_DIAL_STR);
        if(CALL_SUCESS == CallApp_MakeCall(pMe))
        {
            pMe->m_bt_audio = SEND_BT_CALL;
            return SUCCESS;
        }
        else
        {
            return EFAILED;
        }
    }

    call_start_info_call.b_closeAll = CALL_INFO_NO;
    call_start_info_call.catgory = CALL_INFO_SEND_TO_BT;
    WSTRTOSTR(number,call_start_info_call.dial_str, sizeof(call_start_info_call.dial_str));

    STRTOWSTR(call_start_info_call.dial_str, pMe->m_DialString, sizeof(pMe->m_DialString));
    pMe->m_bt_audio = SEND_BT_CALL;
    MOVE_TO_STATE(STATE_CALLING_FROM_ANOTHERAPP)
    if (ISHELL_StartAppletArgs(pMe->m_pShell,
           AEECLSID_DIALER,
           (const char *)&call_start_info_call) != SUCCESS)
    {
        CALL_ERR("^&^& Start AEECLSID_DIALER fail", 0,0,0);
        return EFAILED;
    }

    return SUCCESS;
#else
    return EFAILED;
#endif
}

/*=============================================================================
FUNCTION: CallApp_AddToCFGCallTimer

DESCRIPTION: 修改Config项中的通话时长

PARAMETERS:
  *pMe : CCallApp object pointer
   call_type: phone type
   durationS  : Call duration in s

RETURN VALUE:
    None

COMMENTS:

SIDE EFFECTS:

=============================================================================*/
static void CallApp_AddToCFGCallTimer(CCallApp *pMe, uint16 call_type, uint32 durationS)
{
      uint32        timerS;
      AEEConfigItem item;

      timerS = 0;
      (void) OEM_GetConfig(CFGI_ALL_CALL_TIMER, &timerS, sizeof(timerS));
      timerS += durationS;
      (void) OEM_SetConfig(CFGI_ALL_CALL_TIMER, &timerS, sizeof(timerS));

      if (call_type == AEECALLHISTORY_CALL_TYPE_FROM)
      {
         item = CFGI_RECENT_MT_CALL_TIMER;
      }
      else if (call_type == AEECALLHISTORY_CALL_TYPE_TO)
      {
         item = CFGI_RECENT_MO_CALL_TIMER;
      }
      else
      {
         return;
      }

      timerS = 0;
      (void) OEM_GetConfig(item, &timerS, sizeof(timerS));
      timerS += durationS;
      (void) OEM_SetConfig(item, &timerS, sizeof(timerS));
}
void CallAppNotifyMP3PlayerAlertEvent(CCallApp *pMe, boolean toStartAlert)
{
 #ifdef FEATURE_APP_MUSICPLAYER
    if(GetMp3PlayerStatus() == MP3STATUS_RUNONBACKGROUND)
    {
        ISHELL_PostEvent(pMe->m_pShell,
                         AEECLSID_APP_MUSICPLAYER,
                         EVT_ALARM,
                         toStartAlert,
                         0);
    }
 #endif
}

static call_start_info_type call_start_info_call;

boolean MakeVoiceCall(IShell *pShell, boolean bCloseAll, AECHAR *number)
{
      if((pShell == NULL)||(number == NULL))
        return FALSE;

      call_start_info_call.b_closeAll = (bCloseAll) ? CALL_INFO_YES : CALL_INFO_NO;
      call_start_info_call.catgory = CALL_INFO_SEND;

      WSTRTOSTR(number, call_start_info_call.dial_str, sizeof(call_start_info_call.dial_str));
      if (ISHELL_StartAppletArgs(pShell, AEECLSID_DIALER/*AEECLSID_CALL*/, (const char *)&call_start_info_call) != SUCCESS) 
      {
            MSG_ERROR("^&^& Start AEECLSID_CALL fail", 0,0,0);
            return FALSE;
      }

      return TRUE;
}

typedef struct{
    AEECMEvent              event;
    cm_mm_call_info_s_type  myInfo;
    int                     nWaitTimeMS;
}CallTestDataType;

extern void OEMCM_TestCallEventNotify(AEECMEvent event, cm_mm_call_info_s_type *call_info);
static CallTestDataType *g_pTestStep = NULL;
static int               g_nMaxSteps = 0;
static int               g_nCurrStep = 0;

void CallApp_CallTestCB(void *pUser)
{
    if(g_pTestStep && g_nCurrStep<g_nMaxSteps)
    {
        OEMCM_TestCallEventNotify(g_pTestStep[g_nCurrStep].event, &(g_pTestStep[g_nCurrStep].myInfo));
        MSG_FATAL("CallApp_CallTestCB %d %d",g_nCurrStep,g_pTestStep[g_nCurrStep].nWaitTimeMS,0);
        AEE_SetSysTimer(g_pTestStep[g_nCurrStep].nWaitTimeMS,CallApp_CallTestCB,NULL);
        g_nCurrStep++;
    }
    else
    {
        if(g_pTestStep)
        {
            sys_free(g_pTestStep);
        }
        g_pTestStep = NULL;
        g_nMaxSteps = 0;
        g_nCurrStep = 0;
    }
}

void CallApp_StartCallTest(void)
{
    CallTestDataType   *pCurr;
    
    AEE_CancelTimer(CallApp_CallTestCB,NULL);
    if(g_pTestStep)
    {
        sys_free(g_pTestStep);
    }
    g_nMaxSteps = 9;
    g_nCurrStep = 0;
    g_pTestStep = sys_malloc(g_nMaxSteps*sizeof(CallTestDataType));
    pCurr = g_pTestStep;
    
    // Init Step 1
    pCurr->nWaitTimeMS  = 1000;
    pCurr->event = AEECM_EVENT_CALL_INCOM;
    pCurr->myInfo.call_id = 1;
    STRCPY((char *)pCurr->myInfo.num.buf,"00911234567890");
    pCurr->myInfo.num.len=STRLEN("00911234567890");
   // STRCPY((char *)pCurr->myInfo.num.buf, "111111");
   /* MEMSET(pCurr->myInfo.num.buf,0xFF,sizeof(pCurr->myInfo.num.buf));
    pCurr->myInfo.num.len=6;
	pCurr->myInfo.num.buf[0]	  = 0x91;
	pCurr->myInfo.num.buf[1]	  = 0x13;
	pCurr->myInfo.num.buf[2]	  = 0x56;
	pCurr->myInfo.num.buf[3]	  = 0x07;
	pCurr->myInfo.num.buf[4]	  = 0x23;
	pCurr->myInfo.num.buf[5]	  = 0x30;
    */
    
    pCurr->myInfo.num.number_type = CM_NUM_TYPE_INTERNATIONAL;
    pCurr->myInfo.num.number_plan = CM_NPI_UNKNOWN;
    pCurr->myInfo.num.pi          = 0;
    pCurr->myInfo.num.si          = 1;
    pCurr->myInfo.call_state      = CM_CALL_STATE_INCOM;
    pCurr->myInfo.call_type       = CM_CALL_TYPE_VOICE;
    pCurr->myInfo.sys_mode        = SYS_SYS_MODE_CDMA;
    pCurr->myInfo.direction       = CM_CALL_DIRECTION_MT;
    pCurr->myInfo.signal.is_signal_info_avail   = TRUE;
    pCurr->myInfo.signal.signal_type            = AEECM_SIGNAL_CDMA_IS54B;
    pCurr->myInfo.signal.signal       = AEECM_CDMA_TONE_RING;
    pCurr->myInfo.end_status                    = CM_CALL_END_ALERT_STOP;
    pCurr++;
   #if 0 
    // Init Step 2
    pCurr->nWaitTimeMS  = 1000;
    pCurr->event = AEECM_EVENT_CALL_SIGNAL;
    STRCPY((char *)pCurr->myInfo.num.buf,"00911234567890");
    pCurr->myInfo.num.len=STRLEN("00911234567890");



    pCurr->myInfo.num.number_type = CM_NUM_TYPE_INTERNATIONAL;
    pCurr->myInfo.num.number_plan = CM_NPI_UNKNOWN;
    pCurr->myInfo.num.pi          = 0;
    pCurr->myInfo.num.si          = 1;
    pCurr->myInfo.call_state      = CM_CALL_STATE_INCOM;
    pCurr->myInfo.call_type       = CM_CALL_TYPE_VOICE;
    pCurr->myInfo.sys_mode        = SYS_SYS_MODE_CDMA;
    pCurr->myInfo.direction       = CM_CALL_DIRECTION_MT;
    pCurr->myInfo.signal.is_signal_info_avail   = TRUE;
    pCurr->myInfo.signal.signal_type            = AEECM_SIGNAL_CDMA_IS54B;
    pCurr->myInfo.signal.signal       = AEECM_CDMA_TONE_RING;
    pCurr->myInfo.end_status                    = CM_CALL_END_ALERT_STOP;
    pCurr++;

    // Init Step 3
    pCurr->nWaitTimeMS  = 10000;
    pCurr->event = AEECM_EVENT_CALL_CALLER_ID;
    pCurr->myInfo.call_id = 1;
    STRCPY((char *)pCurr->myInfo.num.buf,"911234567890");
    pCurr->myInfo.num.len=STRLEN("911234567890");



    pCurr->myInfo.num.number_type = CM_NUM_TYPE_INTERNATIONAL;
    pCurr->myInfo.num.number_plan = CM_NPI_UNKNOWN;
    pCurr->myInfo.num.pi          = 0;
    pCurr->myInfo.num.si          = 1;
    pCurr->myInfo.call_state      = CM_CALL_STATE_INCOM;
    pCurr->myInfo.call_type       = CM_CALL_TYPE_VOICE;
    pCurr->myInfo.sys_mode        = SYS_SYS_MODE_CDMA;
    pCurr->myInfo.direction       = CM_CALL_DIRECTION_MT;
    pCurr->myInfo.signal.is_signal_info_avail   = TRUE;
    pCurr->myInfo.signal.signal_type            = AEECM_SIGNAL_CDMA_IS54B;
    pCurr->myInfo.signal.signal       = AEECM_CDMA_TONE_RING;
    pCurr->myInfo.end_status                    = CM_CALL_END_ALERT_STOP;
    pCurr++;

    // Init Step 4
    pCurr->nWaitTimeMS  = 1000;
    pCurr->event = AEECM_EVENT_CALL_ANSWER;
    pCurr->myInfo.call_id = 1;
    STRCPY((char *)pCurr->myInfo.num.buf,"911234567890");
    pCurr->myInfo.num.len=STRLEN("911234567890");



    pCurr->myInfo.num.number_type = CM_NUM_TYPE_INTERNATIONAL;
    pCurr->myInfo.num.number_plan = CM_NPI_UNKNOWN;
    pCurr->myInfo.num.pi          = 0;
    pCurr->myInfo.num.si          = 1;
    pCurr->myInfo.call_state      = CM_CALL_STATE_INCOM;
    pCurr->myInfo.call_type       = CM_CALL_TYPE_VOICE;
    pCurr->myInfo.sys_mode        = SYS_SYS_MODE_CDMA;
    pCurr->myInfo.direction       = CM_CALL_DIRECTION_MT;
    pCurr->myInfo.signal.is_signal_info_avail   = TRUE;
    pCurr->myInfo.signal.signal_type            = AEECM_SIGNAL_CDMA_IS54B;
    pCurr->myInfo.signal.signal       = AEECM_CDMA_TONE_RING;
    pCurr->myInfo.end_status                    = CM_CALL_END_ALERT_STOP;
    pCurr++;

    // Init Step 5
    pCurr->nWaitTimeMS  = 1000;
    pCurr->event = AEECM_EVENT_CALL_CONNECT;
    pCurr->myInfo.call_id = 1;
    STRCPY((char *)pCurr->myInfo.num.buf,"911234567890");
    pCurr->myInfo.num.len=STRLEN("911234567890");



    pCurr->myInfo.num.number_type = CM_NUM_TYPE_INTERNATIONAL;
    pCurr->myInfo.num.number_plan = CM_NPI_UNKNOWN;
    pCurr->myInfo.num.pi          = 0;
    pCurr->myInfo.num.si          = 1;
    pCurr->myInfo.call_state      = CM_CALL_STATE_INCOM;
    pCurr->myInfo.call_type       = CM_CALL_TYPE_VOICE;
    pCurr->myInfo.sys_mode        = SYS_SYS_MODE_CDMA;
    pCurr->myInfo.direction       = CM_CALL_DIRECTION_MT;
    pCurr->myInfo.signal.is_signal_info_avail   = TRUE;
    pCurr->myInfo.signal.signal_type            = AEECM_SIGNAL_CDMA_IS54B;
    pCurr->myInfo.signal.signal       = AEECM_CDMA_TONE_RING;
    pCurr->myInfo.end_status                    = CM_CALL_END_ALERT_STOP;
    pCurr++;

    // Init Step 6
    pCurr->nWaitTimeMS  = 1000;
    pCurr->event = AEECM_EVENT_CALL_INCOM;
    pCurr->myInfo.call_id = 1;
    STRCPY((char *)pCurr->myInfo.num.buf,"911234567890");
    pCurr->myInfo.num.len=STRLEN("911234567890");



    pCurr->myInfo.num.number_type = CM_NUM_TYPE_INTERNATIONAL;
    pCurr->myInfo.num.number_plan = CM_NPI_UNKNOWN;
    pCurr->myInfo.num.pi          = 0;
    pCurr->myInfo.num.si          = 1;
    pCurr->myInfo.call_state      = CM_CALL_STATE_INCOM;
    pCurr->myInfo.call_type       = CM_CALL_TYPE_VOICE;
    pCurr->myInfo.sys_mode        = SYS_SYS_MODE_CDMA;
    pCurr->myInfo.direction       = CM_CALL_DIRECTION_MT;
    pCurr->myInfo.signal.is_signal_info_avail   = TRUE;
    pCurr->myInfo.signal.signal_type            = AEECM_SIGNAL_CDMA_IS54B;
    pCurr->myInfo.signal.signal       = AEECM_CDMA_TONE_INTERCEPT;
    pCurr->myInfo.end_status                    = CM_CALL_END_ALERT_STOP;
    pCurr++;

    // Init Step 7
    pCurr->nWaitTimeMS  = 1000;
    pCurr->event = AEECM_EVENT_CALL_SIGNAL;
    pCurr->myInfo.call_id = 1;
    STRCPY((char *)pCurr->myInfo.num.buf,"911234567890");
    pCurr->myInfo.num.len=STRLEN("911234567890");


    pCurr->myInfo.num.number_type = CM_NUM_TYPE_INTERNATIONAL;
    pCurr->myInfo.num.number_plan = CM_NPI_UNKNOWN;
    pCurr->myInfo.num.pi          = 0;
    pCurr->myInfo.num.si          = 1;
    pCurr->myInfo.call_state      = CM_CALL_STATE_INCOM;
    pCurr->myInfo.call_type       = CM_CALL_TYPE_VOICE;
    pCurr->myInfo.sys_mode        = SYS_SYS_MODE_CDMA;
    pCurr->myInfo.direction       = CM_CALL_DIRECTION_MT;
    pCurr->myInfo.signal.is_signal_info_avail   = TRUE;
    pCurr->myInfo.signal.signal_type            = AEECM_SIGNAL_CDMA_IS54B;
    pCurr->myInfo.signal.signal       = AEECM_CDMA_TONE_INTERCEPT;
    pCurr->myInfo.end_status                    = CM_CALL_END_ALERT_STOP;
    pCurr++;

    // Init Step 8
    pCurr->nWaitTimeMS  = 10000;
    pCurr->event = AEECM_EVENT_CALL_CALLER_ID;
    pCurr->myInfo.call_id = 1;
    STRCPY((char *)pCurr->myInfo.num.buf,"911234567890");
    pCurr->myInfo.num.len=STRLEN("911234567890");



    pCurr->myInfo.num.number_type = CM_NUM_TYPE_INTERNATIONAL;
    pCurr->myInfo.num.number_plan = CM_NPI_UNKNOWN;
    pCurr->myInfo.num.pi          = 0;
    pCurr->myInfo.num.si          = 1;
    pCurr->myInfo.call_state      = CM_CALL_STATE_INCOM;
    pCurr->myInfo.call_type       = CM_CALL_TYPE_VOICE;
    pCurr->myInfo.sys_mode        = SYS_SYS_MODE_CDMA;
    pCurr->myInfo.direction       = CM_CALL_DIRECTION_MT;
    pCurr->myInfo.signal.is_signal_info_avail   = TRUE;
    pCurr->myInfo.signal.signal_type            = AEECM_SIGNAL_CDMA_IS54B;
    pCurr->myInfo.signal.signal       = AEECM_CDMA_TONE_INTERCEPT;
    pCurr->myInfo.end_status                    = CM_CALL_END_ALERT_STOP;
    pCurr++;
    
    // Init Step 9
    pCurr->nWaitTimeMS  = 1000;
    pCurr->event = AEECM_EVENT_CALL_END;
    pCurr->myInfo.call_id = 1;
    STRCPY((char *)pCurr->myInfo.num.buf,"911234567890");
    pCurr->myInfo.num.len=STRLEN("911234567890");

    pCurr->myInfo.num.number_type = CM_NUM_TYPE_INTERNATIONAL;
    pCurr->myInfo.num.number_plan = CM_NPI_UNKNOWN;
    pCurr->myInfo.num.pi          = 0;
    pCurr->myInfo.num.si          = 1;
    pCurr->myInfo.call_state      = CM_CALL_STATE_INCOM;
    pCurr->myInfo.call_type       = CM_CALL_TYPE_VOICE;
    pCurr->myInfo.sys_mode        = SYS_SYS_MODE_CDMA;
    pCurr->myInfo.direction       = CM_CALL_DIRECTION_MT;
    pCurr->myInfo.signal.is_signal_info_avail   = TRUE;
    pCurr->myInfo.signal.signal_type            = AEECM_SIGNAL_CDMA_IS54B;
    pCurr->myInfo.signal.signal       = AEECM_CDMA_TONE_INTERCEPT;
    pCurr->myInfo.end_status                    = CM_CALL_END_ALERT_STOP;
    #endif
    AEE_SetSysTimer(0,CallApp_CallTestCB,NULL);
}


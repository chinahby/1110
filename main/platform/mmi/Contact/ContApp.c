/*==============================================================================
// 文件：
//        ContApp.c
//        2004-06-09 jyh草拟版本(Draft Version)
//        版权所有(c) 2004 Anylook
//        
// 文件说明：
//        
// 作者：2004-06-09
// 创建日期：jyh
// 当前版本：Draft
==============================================================================*/

/*==============================================================================
修改历史记录：
时    间      修   改   人     修改内容、位置及原因
--------      -----------     -----------------------------------------------
04-06-09      jyh             初始版本
==============================================================================*/


/*==============================================================================
                                 本文件包含的外部文件
==============================================================================*/
#include "ContApp_priv.h"
#include "ContAppCommon.h"
#include "ContApp_vCard.h"
#ifndef WIN32
#include "db.h"
#endif
/*==============================================================================
                                 宏定义和常数
==============================================================================*/
/*lint -e506 -e774 -e737*/
/*==============================================================================
                                 类型定义
==============================================================================*/

/*==============================================================================
                                 函数声明
==============================================================================*/
/*----------------------Applet相关函数声明---------------------*/
#if defined(AEE_STATIC)
extern int ContApp_Load(IShell *ps, void * pHelpers, IModule ** pMod);
static int ContApp_CreateInstance( AEECLSID    ClsId,
                                        IShell     *pIShell,
                                        IModule    *po,
                                        void      **ppObj);
#endif

static int ContApp_New(IShell * pIShell,IModule * po,void ** ppObj);

static uint32 IContApp_AddRef(IContApp *pi);

static uint32 IContApp_Release(IContApp *pi);

static boolean IContApp_HandleEvent( IContApp *pi,
                                      AEEEvent   eCode,
                                      uint16     wParam,
                                      uint32     dwParam);

static int CContApp_InitAppData(CContApp *pMe);

static void CContApp_FreeAppData(CContApp *pMe);

static void CContApp_RunFSM(CContApp *pMe);

static int IContApp_SaveNumber( IContApp *pi, 
                                     AECHAR *number,
                                     AECHAR *E_Mail,    
                                     AECHAR *URL,                                     
                                     PhoneSaveType saveType);

static int      IContApp_FindNumber(IContApp *pi, AECHAR *searchStr);

static int      IContApp_SelectField( IContApp *pi, 
                                      PFNSELECTEDCONTACTCB fnCB, 
                                      SelectFldType FldType);
static int      IContApp_NumberLookup( IContApp   *pi,
                                       AECHAR *pNumber,
                                       ContInfo *pContInfo);
static int      IContApp_SpeedDialLookup( IContApp   *pi,
                                          AECHAR     *pSpeedDial,
                                          AECHAR     *pNumber,
                                          uint16      wNumLen);
static int      IContApp_SpeedDialCall(IContApp *pi, AECHAR *pSpeedDial);

static int      IContApp_MainMenu(IContApp *pi);
                                        
static int      IContApp_DeleteAll(IContApp *pi);

#ifdef FEATURE_SUPPORT_VCARD
static int      IContApp_ReadVcfFile(IContApp *pi, char* fileName, int * cnt);

static int      IContApp_CreateVcfFile(IContApp *pi, int contId, char* fileName);

static int      IContApp_CreateVcfFileEx(IContApp *pi, AEEAddrField *pFields, int nFldCnt, char* fileName);

static int      IContApp_CheckCapacity(IContApp *pi, boolean *isFull);

#endif

// ContApp helper functions
////////////
static int      CContApp_Start(CContApp *pMe);
static void     CContApp_AppIsReadyCB(void *pUser);
static int      CContApp_ReadCFG( IFileMgr *pFileMgr, ContAppCFGCache *pCFGCache);
static int      CContApp_WriteCFG( IFileMgr *pFileMgr,  ContAppCFGCache *pCFGCache);
static void     CContApp_CFGCacheInit(CContApp *pMe, ContAppCFGCache *pCFGCache);
static char     *CContApp_BuildStartArgs( AppletStartMethod method,
                                          void             *saveType,
                                          void             *arg);
static void     CContApp_DecodeStartArgs(CContApp *pMe, char *args);
static boolean  CContApp_WStrStrEx( AECHAR *pHaystack, AECHAR *pNeedle);
static int      CContApp_OneDialCall(CContApp *pMe, AECHAR *pOneDial);
static int      CContApp_GetOneDial( CContApp   *pMe,
                                     AECHAR     *pSpeedDial,
                                     AECHAR     *pNumber,
                                     uint16      wNumLen);
static int      CContApp_GetSmartDial( CContApp   *pMe,
                                       AECHAR     *pSpeedDial,
                                       AECHAR     *pNumber,
                                       uint16      wNumLen);
static int      CContApp_GetNumber( IAddrRec   *pAddrRec,
                                    AECHAR     *pNumber,
                                    uint16      wNumLen);
                               

/*快速查找中焦点自动切换处理函数，Gemsea @2008.08.08*/
void CContApp_SmartMenuSetFocus(void *pUser);


/*==============================================================================
                                 全局数据
==============================================================================*/
sSelectFieldListNode* pSelectFieldListRoot = NULL;
sSelectFieldListNode* pCurSelectFieldNode = NULL;


/*==============================================================================
                                 本地（静态）数据
==============================================================================*/
#if defined(AEE_STATIC)
/*

#define CONTAPP_METHODS                 \
        {                               \
             IContApp_AddRef,           \
             IContApp_Release,          \
             IContApp_HandleEvent,      \
             IContApp_SaveNumber,       \
             IContApp_FindNumber,       \
             IContApp_SelectField,      \
             IContApp_NumberLookup,     \
             IContApp_SpeedDialLookup,  \
             IContApp_SpeedDialCall,    \
             IContApp_MainMenu,         \
             IContApp_DeleteAll         \
#ifdef FEATURE_SUPPORT_BT_APP           \
             ,IContApp_ReadVcfFile      \
#endif                                  \
        }
*/
// 只允许一个ContApp实例。每次创建ContApp Applet时，
// 返回同一结构指针给BREW层。
static CContApp gContApp;

static const VTBL(IContApp) gContAppMethods = /*CONTAPP_METHODS;*/
            {                               
             IContApp_AddRef,           
             IContApp_Release,          
             IContApp_HandleEvent,      
             IContApp_SaveNumber,       
             IContApp_FindNumber,       
             IContApp_SelectField,      
             IContApp_NumberLookup,     
             IContApp_SpeedDialLookup,  
             IContApp_SpeedDialCall,    
             IContApp_MainMenu,         
             IContApp_DeleteAll         
#ifdef FEATURE_SUPPORT_VCARD           
             ,IContApp_ReadVcfFile
             ,IContApp_CreateVcfFile
             ,IContApp_CreateVcfFileEx
             ,IContApp_CheckCapacity
#endif                                  
        };

#endif

/*==============================================================================
                                 函数定义
==============================================================================*/
/*----------------------Applet相关函数定义---------------------*/
#if defined(AEE_STATIC)

/*==============================================================================
函数: 
       ContApp_Load(IShell *ps, void * pHelpers, IModule ** pMod)

说明:  
        由BREW调用以初始静态模块的标准函数。

参数: 
       ps [in]：IShell接口对象指针。
       pHelpers [in]：
       pIModule [in]：pIModule接口对象指针。

返回值:
       非0：失败。
       0：成功。
       
备注:

==============================================================================*/
int ContApp_Load(IShell *ps, void * pHelpers, IModule ** pMod)
{
    return(AEEStaticMod_New( (int16)(sizeof(AEEMod)),
                             ps ,
                             pHelpers,
                             pMod, 
                             (PFNMODCREATEINST)ContApp_CreateInstance,
                             (PFNFREEMODDATA)NULL));
} // ContApp_Load
#endif

/*==============================================================================
函数: 
       ContApp_CreateInstance

说明:  
       This function is invoked while the app is being loaded. All Modules must 
       provide this function. Ensure to retain the same name and parameters for 
       this function

参数: 
       ClsId:    [in]: Specifies the ClassID of the applet which is being loaded
       pIShell:  [in]: Contains pointer to the IShell interface. 
       pIModule: [in]: Contains pointer to the IModule interface to the current 
                       module to whichthis app belongs
       ppObj:   [out]: On return, *ppObj must point to a valid AEEApplet 
                       structure. Allocation of memory for this structure and 
                       initializing the base data members is done by 
                       AEEApplet_New().

返回值:

备注:

==============================================================================*/
#if defined(AEE_STATIC)
static int ContApp_CreateInstance( AEECLSID    ClsId,
                                   IShell     *pIShell,
                                   IModule    *po,
                                   void      **ppObj)
#else
int AEEClsCreateInstance( AEECLSID   ClsId,
                          IShell    *pIShell,
                          IModule   *po,
                          void     **ppObj)
#endif
{
    if (ClsId == AEECLSID_APP_CONTACT)
    {
        return ContApp_New(pIShell, po, ppObj);
    }
    else
    {
        return EUNSUPPORTED;
    }
} // ContApp_CreateInstance

/*==============================================================================
函数: 
       ContApp_New
       
说明: 
        创建对象实例。
       
参数: 
       ps [in]：IShell接口对象指针。
       po [in]：pIModule接口对象指针。
       ppObj [out]:指向IContApp对象指针的指针。
       
返回值:
       SUCCESS：成功。
       
备注:
       
==============================================================================*/
static int ContApp_New(IShell * pIShell, IModule * po, void ** ppObj)
{
    CContApp *pMe = NULL;
    int retVal = SUCCESS;
    
    if ((NULL == ppObj) || (NULL == pIShell) || (NULL == po))
    {
        return EFAILED;
    }

#if defined(AEE_STATIC)
    pMe = &gContApp;
#else
    pMe = (CContApp *)MALLOC(sizeof(CContApp) + sizeof(VTBL(IContApp)));
    if (NULL == pMe)
    {
        return EFAILED;
    }
#endif
    if (0 == pMe->m_nRefs)
    {
#if defined(AEE_STATIC)
        // Must initialize the object
        MEMSET(pMe,  0, sizeof(CContApp));
        INIT_VTBL(pMe, IContApp, gContAppMethods);
#else
        //NOTE: In the dynamic applet, the extend classes will re-malloc the 
        //      memery everytime in startapplet. So it run in simulator for
        //      test only. If you want run in simulator correcttly please 
        //      modify the following code.
        VTBL(IContApp) *pMod = (VTBL(IContApp)*)((byte *)pMe +sizeof(CContApp));
        pMod->AddRef      = IContApp_AddRef;
        pMod->Release     = IContApp_Release;
        pMod->HandleEvent = IContApp_HandleEvent;
        INIT_VTBL(pMe, IContApp, *pMod);
#endif
        
        pMe->m_nRefs    = 0;
        pMe->m_pShell   = pIShell;
        pMe->m_pModule  = po;
        pMe->m_isdel    = FALSE;
    
        (void) ISHELL_AddRef(pMe->m_pShell);
        (void) IMODULE_AddRef(pMe->m_pModule);
    
        retVal = CContApp_InitAppData(pMe);
        if( SUCCESS != retVal )
        {
            CContApp_FreeAppData(pMe);
            return retVal;
        }
    }
    
    ++pMe->m_nRefs;
    *ppObj = (IContApp*)pMe;
    return retVal;
} // ContApp_Load

/*==============================================================================
函数: 
       IContApp_AddRef
       
说明: 
       ContApp Applet 引用计数加1，并返回当前的引用数。
       
参数: 
       pi [in]：IContApp接口对象指针。
       
返回值:
       当前对ContApp Applet的引用数
       
备注:
       
==============================================================================*/
static uint32  IContApp_AddRef(IContApp *pi)
{
    CContApp *pMe = (CContApp*)pi;
    
#if defined(AEE_STATIC)
    ASSERT(pMe != NULL);
    ASSERT(pMe->m_nRefs > 0);
#endif
    
    return (++pMe->m_nRefs);
} // IContApp_AddRef

/*==============================================================================
函数: 
       IContApp_Release
       
说明: 
       释放对ContApp Applet的引用，引用计数减1，并返回当前的引用数。
        如果当前的引用数为零，还将释放Applet及相关资源。
       
参数: 
       pi [in]：IContApp接口对象指针。
       
返回值:
       当前对ContApp Applet的引用数
       
备注:
       
==============================================================================*/
static uint32  IContApp_Release (IContApp *pi)
{
    CContApp *pMe = (CContApp*)pi;

#if defined(AEE_STATIC)
    ASSERT(pMe != NULL);
#endif
    
    if (pMe->m_nRefs == 0)
    {
        return 0;
    }
    
    if (--pMe->m_nRefs)
    {
        return pMe->m_nRefs;
    }

    CContApp_FreeSelectFieldListNode();
    
    // 释放Applet相关资源
    CContApp_FreeAppData(pMe);
    
    (void) ISHELL_Release(pMe->m_pShell);
    (void) IMODULE_Release(pMe->m_pModule);
    
    // 注意：如果pMe是静态分配空间，则不需要释放。FREE()
#if !defined(AEE_STATIC)
    FREE_VTBL(pMe, IContApp);
    FREE(pMe);
#endif
    return 0;
} // IContApp_Release

/*==============================================================================
函数: 
       CContApp_InitAppData
       
说明: 
       初始化ContApp Applet结构数据缺省值。
       
参数: 
       pMe [in]：指向ContApp Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值:
       SUCCESS：初始化成功。
       EFAILED：初始化失败。
       
备注:
       
==============================================================================*/
static int CContApp_InitAppData(CContApp *pMe)
{
#if defined(AEE_STATIC)
    ASSERT(pMe != NULL);
#endif

    // Create IDisplay instance
    if ( SUCCESS != ISHELL_CreateInstance( pMe->m_pShell,
                                           AEECLSID_DISPLAY,
                                           (void **)&pMe->m_pDisplay))
    {
        return EFAILED;
    }
    else
    {
        // 获得正常字体的行高
        pMe->m_nLineHeight = IDISPLAY_GetFontMetrics( pMe->m_pDisplay,
                                                      AEE_FONT_NORMAL,
                                                      NULL,
                                                      NULL );
    }
    
    // Create IVector instance
    if ( SUCCESS != ISHELL_CreateInstance( pMe->m_pShell,
                                           AEECLSID_VECTOR,
                                           (void **)&pMe->m_pAddList))
    {
        return EFAILED;
    }

    // Create IVector instance
    if ( SUCCESS != ISHELL_CreateInstance( pMe->m_pShell,
                                           AEECLSID_VECTOR,
                                           (void **)&pMe->m_pFldIv))
    {
        return EFAILED;
    }
    
    if ( SUCCESS != ISHELL_CreateInstance( pMe->m_pShell,
                                           AEECLSID_FILEMGR,
                                           (void **)&pMe->m_pFileMgr))
    {
        return EFAILED;
    }
    
    // Create IAddrbokk instance for phone
    if ( SUCCESS != ISHELL_CreateInstance( pMe->m_pShell,
                                           AEECLSID_ADDRBOOK,
                                           (void **)&pMe->m_pAddrPhone))
    {
        return EFAILED;
    }

#ifdef FEATURE_RUIM_PHONEBOOK
    pMe->m_bADNRUIMSupport = OEM_IsCDMASVCSupport(UIM_CDMA_SVC_ADN);
    if( IsRunAsUIMVersion() )
    {    // Create IAddrbokk instance for RUIM
        if ( SUCCESS != ISHELL_CreateInstance( pMe->m_pShell,
                                               AEECLSID_ADDRBOOK_RUIM,
                                               (void **)&pMe->m_pAddrRUIM))
        {
            return EFAILED;
        }
    }
#endif//FEATURE_RUIM_PHONEBOOK

    if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell, 
                                            AEECLSID_CONFIG, 
                                            (void **)&pMe->m_pConfig))
    {
        return EFAILED;
    }

    if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                        AEECLSID_RINGERMGR,
                                        (void **)&pMe->m_pRingerMgr))
    {
        return EFAILED;
    }
	if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,AEECLSID_ANNUNCIATOR,(void **)&pMe->m_pIAnn))
    {
        return EFAILED;
    }
	if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                            AEECLSID_RUIM,
                                            (void **)&pMe->m_pIRUIM))
	{
		return EFAILED;
	}

#if defined(FEATURE_VERSION_C316)
//#ifdef FEATURE_CHECK_SAME_NAME             
                    {
                        boolean sameRecord = FALSE;
                        IAddrBook          *addr;      // IADDRBOOK interface for RUIM

                        AECHAR  name[11] = {'I', 'N', 'T', 'E', 'X', ' ', 'C', 'A', 'R', 'E', '\0'};
#ifdef FEATURE_VERSION_K232_Y105A
                        AECHAR Mobile[12] = {'1', '8', '6', '0', '1', '0', '8', '5', '5', '5', '5', '\0'};
#else
						  
						AECHAR Mobile[12] = {'1', '8', '0', '0', '1', '0', '3', '8', '8', '2', '2', '\0'};
#endif                          
                        addr = pMe->m_pAddrPhone;
						  pMe->m_pAddNewMobile = WSTRDUP(Mobile);	
						  pMe->m_pAddNewName = WSTRDUP(name);	 	
                        if(SUCCESS == IADDRBOOK_CheckSameRecord(addr, name, &sameRecord) && sameRecord == TRUE)
                        {
                            ;
                        }
						  else
						  {
						      int result = 0;
						      result=CContApp_CreateCont( pMe, FALSE)	;
							  MSG_FATAL("CContApp_CreateCont result=%d", result,0,0);
						  }
                    }
//#endif
#endif											
    return SUCCESS;
} // CContApp_InitAppData

/*==============================================================================
函数: 
       CContApp_FreeAppData
       
说明: 
       释放ContApp Applet使用的相关资源。
       
参数: 
       pMe [in]：指向ContApp Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值:
       无。
       
备注:
       
==============================================================================*/
static void CContApp_FreeAppData(CContApp *pMe)
{
#if defined(AEE_STATIC)
    ASSERT(pMe != NULL);
#endif
    /*取消快速查找中焦点自动切换处理函数*/
     (void) ISHELL_CancelTimer( pMe->m_pShell,
                                                                    CContApp_SmartMenuSetFocus,
                                                                    (void *)pMe);

    FREEIF(pMe->m_pMsgBox_Msg);
    FREEIF(pMe->m_pSrchData);
    FREEIF(pMe->m_pFldInputBuf);
    FREEIF(pMe->m_pAddNewName);
    FREEIF(pMe->m_pAddNewMobile);
    FREEIF(pMe->m_pAddNewHome);
    FREEIF(pMe->m_pAddNewOffice);
    FREEIF(pMe->m_pAddNewFax);
    FREEIF(pMe->m_pAddNewEMAIL);
    FREEIF(pMe->m_pAddNewAddress);
    FREEIF(pMe->m_pAddNewURL);
    FREEIF(pMe->m_pAddNewRemark);
    FREEIF(pMe->m_pFindData);
    FREEIF(pMe->m_pSmartFindBuf);
    FREEIF(pMe->m_pFindNum);
    FREEIF(pMe->m_pOneDial);
    FREEIF(pMe->m_szAlpha);
    FREEIF(pMe->m_sSearchStr);
    FREEIF(pMe->m_strPhonePWD);
    FREEIF(pMe->m_nRingToneID);
    
    if (pMe->m_pCurrRec != NULL) 
    {
        IADDRREC_Release(pMe->m_pCurrRec);
        pMe->m_pCurrRec = NULL;
    }
    
    if (pMe->m_pDisplay != NULL)
    {
        (void) IDISPLAY_Release(pMe->m_pDisplay);
        pMe->m_pDisplay = NULL;
    }
    
    if (pMe->m_pAddList != NULL)
    {
        (void) IVector_Release(pMe->m_pAddList);
        pMe->m_pAddList = NULL;
    }

    if (pMe->m_pFldIv != NULL)
    {
        CContApp_FreeAddrFlds(pMe);
        (void) IVector_Release(pMe->m_pFldIv);
        pMe->m_pFldIv = NULL;
    }
    
    if (pMe->m_pFileMgr != NULL)
    {
        (void) IFILEMGR_Release(pMe->m_pFileMgr);
        pMe->m_pFileMgr = NULL;
    }
    
    if (pMe->m_pAddrPhone != NULL)
    {
        (void) IADDRBOOK_Release(pMe->m_pAddrPhone);
        pMe->m_pAddrPhone = NULL;
    }
    
#ifdef FEATURE_RUIM_PHONEBOOK
    if( IsRunAsUIMVersion() )
    {
        if (pMe->m_pAddrRUIM != NULL)
        {
            (void) IADDRBOOK_Release(pMe->m_pAddrRUIM);
            pMe->m_pAddrRUIM = NULL;
        }
    }
#endif// FEATURE_RUIM_PHONEBOOK

    if (pMe->m_pConfig)
    {
        ICONFIG_Release(pMe->m_pConfig);
        pMe->m_pConfig = NULL;
    }

    if (pMe->m_pRingerMgr)
    {
        IRINGERMGR_Release(pMe->m_pRingerMgr);
        pMe->m_pRingerMgr = NULL;
    }

    if (NULL != pMe->m_eBitmap)
    {
        IBITMAP_Release(pMe->m_eBitmap);
        pMe->m_eBitmap = NULL;
    }
	if(pMe->m_pIAnn)
    {
        IANNUNCIATOR_Release(pMe->m_pIAnn);
        pMe->m_pIAnn = NULL;
    }
	if (pMe->m_pIRUIM != NULL)
	{
	    IRUIM_Release(pMe->m_pIRUIM);
	    pMe->m_pIRUIM = NULL;
	}
	
} // CContApp_FreeAppData

/*==============================================================================
函数: 
       CContApp_RunFSM
       
说明: 
       ContApp Applet有限状态机引擎。
       
参数: 
       pMe [in]：指向ContApp Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值:
       无。
       
备注:
       
==============================================================================*/
static void CContApp_RunFSM(CContApp *pMe)
{
    NextFSMAction nextFSMAction = NFSMACTION_WAIT;

#if defined(AEE_STATIC)
    ASSERT(pMe != NULL);
#endif

    for ( ; ; )
    {
        nextFSMAction = CContApp_ProcessState(pMe);
        
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
} // CContApp_RunFSM

/*==============================================================================
函数: 
       IContApp_HandleEvent
       
说明: 
       ContApp  Applet事件处理主函数。所有属于本Applet的事件，
       都将经由该函数处理。
       
参数: 
       pi [in]：指向IContApp接口t对象的指针。
       eCode：BREW事件代码。
       wParam：与事件相关联的数据。
       dwParam [in]：与事件相关联的数据。
       
返回值:
       TRUE：传入事件被处理。
       FALSE：传入事件没被处理。
       
备注:
       
==============================================================================*/
static boolean  IContApp_HandleEvent( IContApp   *pi,
                                      AEEEvent   eCode,
                                      uint16     wParam,
                                      uint32     dwParam)
{
    CContApp *pMe = (CContApp*)pi;
    AEEAppStart *as;
    AEEDeviceInfo di;
    MSG_FATAL("IContApp_HandleEvent Start",0,0,0);
    ISHELL_GetDeviceInfo(pMe->m_pShell,&di);    
    
    MSG_FATAL("Handle evt 0x%x, w %x, dw %x",eCode, wParam, dwParam );
    
    switch (eCode)
    {
        case EVT_APP_START:
            MSG_FATAL("IContApp_HandleEvent EVT_APP_START",0,0,0);
#if defined(AEE_STATIC)
            ASSERT(dwParam != 0);
#endif
            as = (AEEAppStart*)dwParam;
            pMe->m_rc = as->rc;
            pMe->m_rc.dy = di.cyScreen; //在当前状态下显示信号栏，要重新取dy的大小            
            
            pMe->m_bSuspending = FALSE;
            
            pMe->m_bActive = TRUE;
			
			pMe->m_isCalling = FALSE;
			
            pMe->m_bUnLock = FALSE;
			#if defined(FEATURE_VERSION_K212)||defined(FEATURE_LOW_MEM_BIGFONT)
			 if(as->pszArgs != NULL)
            {
                if(STRCMP(as->pszArgs, "Calling") == 0)
                {
                    pMe->m_isCalling = TRUE;
                }
			}
			#endif
            (void)CContApp_Start(pMe);
            ERR("CCONTAPP_START END",0,0,0);
            // Decode the arguments
            CContApp_DecodeStartArgs(pMe, (char *) as->pszArgs);
            
            if ((pMe->m_eStartMethod == STARTMETHOD_FINDNUMBER) ||
                (pMe->m_eStartMethod == STARTMETHOD_SMARTCALL))
            {
                pMe->m_bUnLock = TRUE;
            }
            if(pMe->m_pIAnn != NULL)
            {
                IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE);
            }
#if defined(FEATURE_INIT_RUIM_SMSandADD_BYUIMTASK)
#if defined( FEATURE_RUIM_PHONEBOOK)
            if (!PhoneBookCache_IsIninited())
            {
                if ((pMe->m_eStartMethod == STARTMETHOD_FINDNUMBER) || 
                    (pMe->m_eStartMethod == STARTMETHOD_SAVEFIELD) ||
                    (pMe->m_eStartMethod == STARTMETHOD_SELECTFIELD) ||
                    (pMe->m_eStartMethod == STARTMETHOD_SMARTCALL))
                {
                    pMe->m_eCurState = STATE_EXIT;
                }

                if ((pMe->m_eStartMethod == STARTMETHOD_NORMAL) ||
                    (pMe->m_eStartMethod == STARTMETHOD_MAINMENU))
                {
                    pMe->m_eDlgRet = DLGRET_CREATE;
                    pMe->m_eCurState = STATE_INIT;
                }
            }
#endif /* FEATURE_RUIM_PHONEBOOK */
#endif /* FEATURE_INIT_RUIM_SMSandADD_BYUIMTASK*/            
            
            // 开始ContApp状态机
            MSG_FATAL("IContApp_HandleEvent Start CContApp_RunFSM",0,0,0);
            CContApp_RunFSM(pMe);
            return TRUE;
			break;
#if defined(FEATURE_SUPPORT_BT_APP) && defined(FEATURE_SUPPORT_VCARD) 
//#if defined(FEATURE_SUPPORT_VCARD) 	//Modify by zzg 2011_10_25

        case EVT_NEWMSGIN:/*test only*/
        {
            ERR("EVT_NEWMSGIN",dwParam,0,0);
            if(dwParam)
            {
                char name[128];
                int cnt=0;
                STRCPY(name,(char*)dwParam);
                ContApp_ReadVcfFile(pMe, name, &cnt);
            }
            return TRUE;
			break;
        }
#endif
        case EVT_FLIP:
            MSG_FATAL("IContApp_HandleEvent EVT_FLIP",0,0,0);
            if( !wParam)
            {
                if(pMe->m_eCurState != STATE_COPYING)
                {
                     (void) ISHELL_CloseApplet(pMe->m_pShell,FALSE);
                     return TRUE;
                }
                else
                {
                     return CContApp_RouteDialogEvent(pMe,eCode,wParam,dwParam);                
                }
            }
            return TRUE;
            break;
        case EVT_APP_STOP:
            MSG_FATAL("IContApp_HandleEvent EVT_APP_STOP",0,0,0);
            pMe->m_bSuspending = TRUE;
            pMe->m_bActive = FALSE;

			//Add By zzg 2012_11_01
			pMe->m_bSpeedDialParam = FALSE;					
			pMe->m_nSpeedDialNumber = 0;
			//Add End
			
            // 转为后台模式
//            *((boolean *)dwParam) = FALSE;
		 
            (void)ISHELL_CancelTimer(pMe->m_pShell, CContApp_AppIsReadyCB, pMe);
            return TRUE;
            break;
        case EVT_APP_SUSPEND:
            MSG_FATAL("IContApp_HandleEvent EVT_APP_SUSPEND",0,0,0);
            pMe->m_bSuspending = TRUE;
            (void)ISHELL_CancelTimer(pMe->m_pShell, CContApp_AppIsReadyCB, pMe);
            return TRUE;
            break;
        case EVT_APP_RESUME:
            MSG_FATAL("IContApp_HandleEvent EVT_APP_RESUME",0,0,0);
#if defined(AEE_STATIC)
            ASSERT(dwParam != 0);
#endif
            as = (AEEAppStart*)dwParam;
            pMe->m_bSuspending = FALSE;
            
            pMe->m_rc = as->rc;
            pMe->m_rc.dy = di.cyScreen; //在当前状态下显示信号栏，要重新取dy的大小            

            if(!(LISTFROMSEARCH == pMe->m_eListFrom
                || LISTFROMGROUP == pMe->m_eListFrom
                || STATE_COPY == pMe->m_eCurState
                || STATE_DELETE == pMe->m_eCurState
                || STATE_SELECT_RECORD == pMe->m_eCurState))
            {
                CContApp_Phonebook_Load(pMe);
            }

            if(CContApp_IsPopState(pMe))
            {
                pMe->m_eCurState = pMe->m_ePreState;
            }
            
            CContApp_RunFSM(pMe);
            return TRUE;
			break;
        case EVT_DIALOG_START:
			if(OEM_IME_DIALOG == wParam)
			{
				return TRUE;
			}
            (void) CContApp_RouteDialogEvent(pMe,eCode,wParam,dwParam);

            return TRUE;  
			break;
        case EVT_DIALOG_INIT:
            if( wParam == OEM_IME_DIALOG)
			{
				return TRUE;
			}
            MSG_FATAL("IContApp_HandleEvent EVT_DIALOG_INIT",0,0,0);
            // 更新对话框控制参数信息！！！
            pMe->m_pActiveDlg = (IDialog*)dwParam;
            pMe->m_pActiveDlgID = wParam;
            pMe->m_bAppReady = FALSE;
            return CContApp_RouteDialogEvent(pMe,eCode,wParam,dwParam);
            break;
        case EVT_DIALOG_END:
            MSG_FATAL("IContApp_HandleEvent EVT_DIALOG_END",0,0,0);
            
			/*if( wParam == OEM_IME_DIALOG)
			{
				repaint( TRUE);
				return TRUE;
			} */
			
            if(OEM_IME_DIALOG == wParam)
			{
				return ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER_REDRAW,0,0);
			}
                        
            if (wParam == 0)
            {
                return TRUE;
            }
            
            (void) CContApp_RouteDialogEvent(pMe,eCode,wParam,dwParam);
            pMe->m_pActiveDlg = NULL;
            // Applet被挂起，不跑状态机，等待Applet返回。（注意：EVT_APP_SUSPEND
            // 事件在EVT_DIALOG_END事件前发出。
            if (pMe->m_bSuspending == FALSE)
            {
                // 开始ContApp状态机
                CContApp_RunFSM(pMe);
            }

            return TRUE;
        	break;
        case EVT_USER_REDRAW:
            (void) CContApp_RouteDialogEvent(pMe,eCode,wParam,dwParam);
            (void)ISHELL_SetTimer( pMe->m_pShell,
                                   CONTAPP_APPREADY_MS,
                                   CContApp_AppIsReadyCB,
                                   pMe);
            return TRUE;
            break;
        case EVT_KEY_PRESS:
        case EVT_KEY:
        case EVT_KEY_RELEASE:
        case EVT_COMMAND:
			#if !defined(FEATURE_VERSION_K212) && !defined(FEATURE_VERSION_EC99)&&!defined(FEATURE_LOW_MEM_BIGFONT)
            if(!pMe->m_bAppReady)
            {
              // 在编辑或者添加状态，因为一进去text控件处于激活状态，text控件可以接收到底层dialog
              // 分发的key事件，但是因为这里的timer的存在，使得上层的dialog在timer到达之前不能接收到key事件。
              //如果一进入EDIT状态就快速按键，text控件会接收到key事件并在相应位置把字符显示出来，如果程序员的目的
              //不是想显示字符而是触发另外的dialog，这样就会造成冲突。
              if(pMe->m_eCurState != STATE_EDIT && pMe->m_eCurState != STATE_ADDNEW)
              {
                return TRUE;
              }
            }
			#endif
            return CContApp_RouteDialogEvent(pMe,eCode,wParam,dwParam);
			break;
#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
		case EVT_PEN_UP:
		//case EVT_PEN_DOWN:
			{
				MSG_FATAL("EVT_PEN_UP...................",0,0,0);
				return CContApp_RouteDialogEvent(pMe,eCode,wParam,dwParam);
			}
		break;

		case EVT_USER:
			{
				MSG_FATAL("EVT_USER dwParam========%d",dwParam,0,0);
				if((wParam == AVK_SELECT) || (wParam == AVK_INFO))
				{
					if(dwParam == 0)
					{
						MSG_FATAL("EVT_USER dwParam====0K",0,0,0);
						eCode = EVT_KEY;
						dwParam = 0;
					}
					else if(dwParam == EVT_KEY_PRESS)
					{
                        eCode = EVT_KEY_PRESS;
						dwParam = 0;
					}
					else if(dwParam == EVT_KEY_RELEASE)
					{
						eCode = EVT_KEY_RELEASE;
						dwParam = 0;
					}
					else
					{
						MSG_FATAL("EVT_COMMAND.....................",0,0,0);
						eCode = EVT_COMMAND;
						wParam = dwParam;
						dwParam = 0;
					}
				}
				else if(wParam == AVK_CLR)
				{
					eCode = EVT_KEY;
				}
				return CContApp_RouteDialogEvent(pMe,eCode,wParam,dwParam);
			}
#else
		case EVT_USER:
		{
			if (wParam == PARAM_SPEED_DIAL) 
			{
				pMe->m_bSpeedDialParam = TRUE;
				pMe->m_nSpeedDialNumber = (uint16)dwParam;

				MSG_FATAL("***zzg PARAM_SPEED_DIAL m_nSpeedDialNumber=%x", pMe->m_nSpeedDialNumber, 0, 0);
				
				return CContApp_RouteDialogEvent(pMe,eCode,wParam,dwParam);				
			}
			break;
		}
#endif            
        default:
            // 将接收到的事件路由至当前活动的对话框事件处理函数。
            return CContApp_RouteDialogEvent(pMe,eCode,wParam,dwParam);
    }
} // IContApp_HandleEvent

///////////////////////////
// ContApp helper functions
////////////

/*==============================================================================
函数: 
       CContApp_Start
       
说明: 
       初始化ContApp Applet结构数据缺省值。
       
参数: 
       pMe [in]：指向ContApp Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值:
       SUCCESS：初始化成功。
       EFAILED：初始化失败。
       
备注:
       
==============================================================================*/
static int CContApp_Start(CContApp *pMe)
{
#if defined(AEE_STATIC)
    ASSERT(pMe != NULL);
#endif
    
    pMe->m_eStartMethod        = STARTMETHOD_NORMAL;
    pMe->m_ePreState           = STATE_NONE;
    pMe->m_eCurState           = STATE_LOAD;
    pMe->m_pActiveDlg          = NULL;
    pMe->m_eDlgRet             = DLGRET_CREATE;
    pMe->m_bNotOverwriteDlgRet = FALSE;
    
    pMe->m_bSuspending         = FALSE;
    
    pMe->m_wSelectCont         = CONTAPP_RECID_NULL;
    pMe->m_wSearchCont        = CONTAPP_RECID_NULL;
    pMe->m_wSelectSmart        = CONTAPP_RECID_NULL;
    pMe->m_wSelectStore        = MENU_SELECT_NULL;
    pMe->m_bAppReady           = FALSE;
    pMe->m_strPhonePWD        = NULL;

//guoys modified for PR 4.2.1 begin
#ifdef FEATURE_RUIM_PHONEBOOK
    if (IsRunAsUIMVersion() && pMe->m_bADNRUIMSupport)
    {
        (void)CContApp_GetConfig( pMe, CONTCFG_SAVETYPE, &pMe->m_nSaveType, sizeof(byte));
    }
    else
    {
        pMe->m_nSaveType    = CONTCFG_SAVETYPE_PHONE;
    }
#else            
    {
        pMe->m_nSaveType    = CONTCFG_SAVETYPE_PHONE;
    }
#endif// FEATURE_RUIM_PHONEBOOK
//guoys modified for PR 4.2.1 end

    pMe->m_pAddNewName = NULL;
    pMe->m_pAddNewMobile =NULL;
    pMe->m_pAddNewHome = NULL;
    pMe->m_pAddNewOffice = NULL;
    pMe->m_pAddNewFax = NULL;
    pMe->m_pAddNewEMAIL = NULL;
    pMe->m_pAddNewAddress = NULL;
    pMe->m_pAddNewURL = NULL;
    pMe->m_pAddNewRemark = NULL;
    pMe->m_nRingToneID = NULL;
    pMe->m_nGroupCat = 0;    
    pMe->m_wSelectGroup =0; 
    pMe->m_nSmartStateType = SMART_STATE_IDD_LIST;
#ifdef FEATURE_LANG_CHINESE
    pMe->m_nInputModeTable[0] = AEE_TM_LETTERS;
    pMe->m_nInputModeTable[1] = AEE_TM_NUMBERS;
    pMe->m_nCurrentInputMode = 0;
#endif
    pMe->m_boptaleadyView = FALSE;
	pMe->m_bNumberInvalid = FALSE;		//Add By zzg 2011-12_15
	pMe->m_bNameLengthLonger = FALSE;		//Add By zzg 2012_10_31
	pMe->m_bSpeedDialParam = FALSE;		//Add By zzg 2012_10_31
	pMe->m_nSpeedDialNumber = 0;		//Add By zzg 2012_11_01
    CContApp_CFGCacheInit(pMe, &pMe->m_sCFGCache);
	
    // Read the config file
    (void)CContApp_ReadCFG(pMe->m_pFileMgr, &pMe->m_sCFGCache);
    

    return SUCCESS;
} // CContApp_Start


/*==============================================================================
函数:
       SoundMenu_APPIsReadyTimer

说明:
       定时器回调函数。主要用于控制快速按键问题的处理。

参数:
       pme [in]：void *类型指针，暗指CSoundMenu结构指针。

返回值:
       无。

备注:

==============================================================================*/
static void CContApp_AppIsReadyCB(void *pUser)
{
    CContApp *pMe = (CContApp *)pUser;
    if (NULL == pMe)
    {
        return;
    }
    
    pMe->m_bAppReady = TRUE;
}// CContApp_AppIsReadyCB

/*==============================================================================
函数: 
       CContApp_ReadCFG
       
说明: 
       将配置文件中的内容用读到Cache中
       
参数: 
       pFileMgr [in]：指向IFileMgr对象结构的指针。
       pCFGCache [out]:存储数据的Cache
       
返回值:
       SUCCESS : 操作成功
       EFAILED : 操作失败
       EBADPARM: 参数错误
       
备注:
       
==============================================================================*/
static int CContApp_ReadCFG(IFileMgr *pFileMgr, ContAppCFGCache *pCFGCache)
{
    IFile  *pFile;
    uint16  version;
    
    if(pCFGCache == NULL || pFileMgr == NULL)
    {
        return EBADPARM;
    }
    
    ERR("READCFG",0,0,0);
    // 打开配置文件
    pFile = IFILEMGR_OpenFile(pFileMgr, FILE_CONTAPP_CFG, _OFM_READ);
    if(pFile == NULL)
    {
        ERR("FILE IS NULL",0,0,0);
        // 尚未创建，使用默认值
        return SUCCESS;
    } //else 打开文件继续执行
    
    // 从文件开始读取数据
    if(SUCCESS != IFILE_Seek(pFile, _SEEK_START, 0))
    {
        (void)IFILE_Release(pFile);
        return EFAILED;
    }
    
    // 读取版本号
    if(sizeof(version) != IFILE_Read(pFile, &version, sizeof(version)))
    {
        (void)IFILE_Release(pFile);
        return EFAILED;
    }
    
    // 校验版本号
    if(version != CFG_VERSION)
    {
        // 版本号不相等，忽略此文件，使用默认值
        (void)IFILE_Release(pFile);
        return SUCCESS;
    }
    
    // 读取配置信息
    if(sizeof(ContAppCFGCache) != IFILE_Read( pFile, 
                                              pCFGCache, 
                                              sizeof(ContAppCFGCache)))
    {
        (void)IFILE_Release(pFile);
        return EFAILED;
    }
    
    (void)IFILE_Release(pFile);
    return SUCCESS;
} // CContApp_ReadCFG

/*==============================================================================
函数: 
       CContApp_WriteCFG
       
说明: 
       将Cache中的内容写到配置文件中
       
参数: 
       pFileMgr [in]：指向IFileMgr对象结构的指针。
       pCFGCache [out]:存储数据的Cache
       
返回值:
       SUCCESS : 操作成功
       EFAILED : 操作失败
       EBADPARM: 参数错误
       
备注:
       
==============================================================================*/
static int CContApp_WriteCFG(IFileMgr *pFileMgr, ContAppCFGCache *pCFGCache)
{
    IFile  *pFile;
    uint16  version = CFG_VERSION;
    
    if(pCFGCache == NULL || pFileMgr == NULL)
    {
        return EBADPARM;
    }
    
    // 打开配置文件
    pFile = IFILEMGR_OpenFile(pFileMgr, FILE_CONTAPP_CFG, _OFM_READWRITE);
    if(pFile == NULL)
    {
        // 创建配置文件
        pFile = IFILEMGR_OpenFile(pFileMgr, FILE_CONTAPP_CFG, _OFM_CREATE);
        ERR("CREATE cfg FILE",0,0,0);
        if(pFile == NULL)
        {
            ERR("CREATE cfg FILE FAIL",0,0,0);
            // 创建配置文件失败
            return EFAILED;
        }//else 打开文件继续执行
    }//else 打开文件继续执行
    
    // 校验版本号
    if(SUCCESS != IFILE_Seek(pFile, _SEEK_START, 0))
    {
        (void)IFILE_Release(pFile);
        return EFAILED;
    }
    
    // 写版本号
    if(sizeof(version) != IFILE_Write(pFile, &version, sizeof(version)))
    {
        (void)IFILE_Release(pFile);
        return EFAILED;
    }
    
    // 写配置信息
    if(sizeof(ContAppCFGCache) != IFILE_Write( pFile,
                                               pCFGCache,
                                               sizeof(ContAppCFGCache)))
    {
        (void)IFILE_Release(pFile);
        return EFAILED;
    }
    
    (void)IFILE_Release(pFile);
    return SUCCESS;
} // CContApp_WriteCFG

/*==============================================================================
函数: 
       CContApp_CFGCacheInit
       
说明: 
       初始化配置项Cache
       
参数: 
       pCFGCache [int]:存储数据的Cache
       
返回值:
       
备注:
       
==============================================================================*/
static void CContApp_CFGCacheInit(CContApp *pMe, ContAppCFGCache *pCFGCache)
{
    ASSERT(pCFGCache != NULL);
    
    pCFGCache->wPassWord[0] = DEFAULT_PASSWORD;
    pCFGCache->bLocked      = FALSE;
    pCFGCache->nViewType    = CONTCFG_VIEWTYPE_ALL;
#ifdef FEATURE_RUIM_PHONEBOOK
    if (IsRunAsUIMVersion() && pMe->m_bADNRUIMSupport)
    {
        pCFGCache->nSaveType    = CONTCFG_SAVETYPE_SELECT;
    }
    else
#endif
    {
        pCFGCache->nSaveType    =     CONTCFG_SAVETYPE_PHONE;
    }
    pCFGCache->bSmartDial   = TRUE;
    pCFGCache->wOneDial1[0] = ONEDIAL_NULL;
    pCFGCache->wOneDial2[0] = ONEDIAL_NULL;
    pCFGCache->wOneDial3[0] = ONEDIAL_NULL;
    pCFGCache->wOneDial4[0] = ONEDIAL_NULL;
    pCFGCache->wOneDial5[0] = ONEDIAL_NULL;
    pCFGCache->wOneDial6[0] = ONEDIAL_NULL;
    pCFGCache->wOneDial7[0] = ONEDIAL_NULL;
    pCFGCache->wOneDial8[0] = ONEDIAL_NULL;
    pCFGCache->wOneDial9[0] = ONEDIAL_NULL;
    pCFGCache->wGroup1[0] = ONEDIAL_NULL;
    pCFGCache->wGroup2[0] = ONEDIAL_NULL;
    pCFGCache->wGroup3[0] = ONEDIAL_NULL;
    pCFGCache->wGroup4[0] = ONEDIAL_NULL;
    pCFGCache->wGroup5[0] = ONEDIAL_NULL;
    pCFGCache->wGroup6[0] = ONEDIAL_NULL; 
    pCFGCache->wGroup7[0] = ONEDIAL_NULL; 
#ifdef FEATURE_LANG_CHINESE
    pCFGCache->inputMode = 0;
#endif
} // CContApp_CFGCacheInit

/*==============================================================================
函数: 
       CContApp_GetConfig
       
说明: 
       获取Config项目的值
       
参数: 
       pMe       [in] :指向ContApp Applet对象结构的指针。
       eCFG      [in] :获取的配置项
       data      [out]:数据存储区
       dataLen   [in] :数据存储区的大小,in bytes
       
返回值:
       SUCCESS : 操作成功
       EFAILED : 操作失败
       EBADPARM: 参数错误
       
备注:
       
==============================================================================*/
int CContApp_GetConfig( CContApp        *pMe, 
                        ContAppCFG       eCFG, 
                        void            *data,
                        int              dataLen)
{
    ContAppCFGCache *pCFGCache;
#if defined(AEE_STATIC)
    ASSERT(pMe != NULL);
#endif
    
    pCFGCache = &pMe->m_sCFGCache;
    if(pCFGCache == NULL)
    {
        return EBADPARM;
    }
    
    // 查找配置项
    switch(eCFG)
    {
        case CONTCFG_LOCKED:
            // 校验数据长度
            if(dataLen != sizeof(pCFGCache->bLocked))
            {
                break;
            }
            
            *(boolean *)data = pCFGCache->bLocked;
            return SUCCESS;
            
        case CONTCFG_VIEWTYPE:
            // 校验数据长度
            if(dataLen != sizeof(pCFGCache->nViewType))
            {
                break;
            }
            
            *(byte *)data = pCFGCache->nViewType;
            return SUCCESS;
             
        case CONTCFG_SAVETYPE:
            // 校验数据长度
            if(dataLen != sizeof(pCFGCache->nSaveType))
            {
                break;
            }
            
            *(byte *)data = pCFGCache->nSaveType;
            return SUCCESS;
            
        case CONTCFG_PASSWORD:
            // 校验数据长度
            if(dataLen < WSTRSIZE(pCFGCache->wPassWord))
            {
                break;
            }
            
            WSTRCPY((AECHAR *)data, pCFGCache->wPassWord);
            return SUCCESS;
            
        case CONTCFG_SMARTDIAL:
            // 校验数据长度
            if(dataLen != sizeof(pCFGCache->bSmartDial))
            {
                break;
            }
            
            *(boolean *)data = pCFGCache->bSmartDial;
            return SUCCESS;
            
        case CONTCFG_ONEDIAL1:
            // 校验数据长度
            if(dataLen < WSTRSIZE(pCFGCache->wOneDial1))
            {
                break;
            }

            WSTRCPY((AECHAR *)data, pCFGCache->wOneDial1);
            return SUCCESS;
            
        case CONTCFG_ONEDIAL2:
            // 校验数据长度
            if(dataLen < WSTRSIZE(pCFGCache->wOneDial2))
            {
                break;
            }
            
            WSTRCPY((AECHAR *)data, pCFGCache->wOneDial2);
            return SUCCESS;
            
        case CONTCFG_ONEDIAL3:
            // 校验数据长度
            if(dataLen < WSTRSIZE(pCFGCache->wOneDial3))
            {
                break;
            }
            
            WSTRCPY((AECHAR *)data, pCFGCache->wOneDial3);
            return SUCCESS;
            
        case CONTCFG_ONEDIAL4:
            // 校验数据长度
            if(dataLen < WSTRSIZE(pCFGCache->wOneDial4))
            {
                break;
            }
            
            WSTRCPY((AECHAR *)data, pCFGCache->wOneDial4);
            return SUCCESS;
            
        case CONTCFG_ONEDIAL5:
            // 校验数据长度
            if(dataLen < WSTRSIZE(pCFGCache->wOneDial5))
            {
                break;
            }
            
            WSTRCPY((AECHAR *)data, pCFGCache->wOneDial5);
            return SUCCESS;
            
        case CONTCFG_ONEDIAL6:
            // 校验数据长度
            if(dataLen < WSTRSIZE(pCFGCache->wOneDial6))
            {
                break;
            }
            
            WSTRCPY((AECHAR *)data, pCFGCache->wOneDial6);
            return SUCCESS;
            
        case CONTCFG_ONEDIAL7:
            // 校验数据长度
            if(dataLen < WSTRSIZE(pCFGCache->wOneDial7))
            {
                break;
            }
            
            WSTRCPY((AECHAR *)data, pCFGCache->wOneDial7);
            return SUCCESS;
            
        case CONTCFG_ONEDIAL8:
            // 校验数据长度
            if(dataLen < WSTRSIZE(pCFGCache->wOneDial8))
            {
                break;
            }
            
            WSTRCPY((AECHAR *)data, pCFGCache->wOneDial8);
            return SUCCESS;
            
        case CONTCFG_ONEDIAL9:
            // 校验数据长度
            if(dataLen < WSTRSIZE(pCFGCache->wOneDial9))
            {
                break;
            }
            
            WSTRCPY((AECHAR *)data, pCFGCache->wOneDial9);
            return SUCCESS;

        case CONTCFG_GROUP1:
            // 校验数据长度
            if(dataLen < WSTRSIZE(pCFGCache->wGroup1))
            {
                break;
            }
            
            WSTRCPY((AECHAR *)data, pCFGCache->wGroup1);
            return SUCCESS;
            
        case CONTCFG_GROUP2:
            // 校验数据长度
            if(dataLen < WSTRSIZE(pCFGCache->wGroup2))
            {
                break;
            }
            
            WSTRCPY((AECHAR *)data, pCFGCache->wGroup2);
            return SUCCESS;
            
        case CONTCFG_GROUP3:
            // 校验数据长度
            if(dataLen < WSTRSIZE(pCFGCache->wGroup3))
            {
                break;
            }
            
            WSTRCPY((AECHAR *)data, pCFGCache->wGroup3);
            return SUCCESS;
        
        case CONTCFG_GROUP4:
            // 校验数据长度
            if(dataLen < WSTRSIZE(pCFGCache->wGroup4))
            {
                break;
            }
            
            WSTRCPY((AECHAR *)data, pCFGCache->wGroup4);
            return SUCCESS;
            
        case CONTCFG_GROUP5:
            // 校验数据长度
            if(dataLen < WSTRSIZE(pCFGCache->wGroup5))
            {
                break;
            }
            
            WSTRCPY((AECHAR *)data, pCFGCache->wGroup5);
            return SUCCESS;
            
        case CONTCFG_GROUP6:
            // 校验数据长度
            if(dataLen < WSTRSIZE(pCFGCache->wGroup6))
            {
                break;
            }
            
            WSTRCPY((AECHAR *)data, pCFGCache->wGroup6);
            return SUCCESS;
        case CONTCFG_GROUP7:
            // 校验数据长度
            if(dataLen < WSTRSIZE(pCFGCache->wGroup7))
            {
                break;
            }
            
            WSTRCPY((AECHAR *)data, pCFGCache->wGroup7);
            return SUCCESS;
#ifdef FEATURE_LANG_CHINESE
        case CONTTCFG_QUICKSEARCH_INPUT_MODE:
            // 校验数据长度
            if(dataLen != sizeof(pCFGCache->inputMode))
            {
                break;
            }
            
            *(byte *)data = pCFGCache->inputMode;
            return SUCCESS;
#endif
        default:
            break;
    }
    return EFAILED;
} // CContApp_GetConfig

/*==============================================================================
函数: 
       CContApp_SetConfig
       
说明: 
       设置Config项目的值
       
参数: 
       pMe       [in] :指向ContApp Applet对象结构的指针。
       eCFG      [in] :获取的配置项
       data      [out]:数据存储区
       dataLen   [in] :数据存储区的大小,in bytes
       
返回值:
       SUCCESS : 操作成功
       EFAILED : 操作失败
       EBADPARM: 参数错误
       
备注:
       
==============================================================================*/
int CContApp_SetConfig(CContApp        *pMe, 
                       ContAppCFG       eCFG, 
                       void            *data,
                       int              dataLen)
{
    ContAppCFGCache *pCFGCache;
#if defined(AEE_STATIC)
    ASSERT(pMe != NULL);
#endif
    
    pCFGCache = &pMe->m_sCFGCache;
    if(pCFGCache == NULL)
    {
        ERR("pCFGCache == NULL",0,0,0);
        return EBADPARM;
    }

	MSG_FATAL("***zzg CContApp_SetConfig eCFG=%x", eCFG, 0, 0);

    ERR("eCFG = %d",eCFG,0,0);
    // 查找配置项
    switch(eCFG)
    {
        case CONTCFG_LOCKED:
            // 校验数据长度
            if(dataLen != sizeof(pCFGCache->bLocked))
            {
                return EFAILED;
            }
            
            pCFGCache->bLocked = *(boolean *)data;
            break;
            
        case CONTCFG_VIEWTYPE:
            // 校验数据长度
            if(dataLen != sizeof(pCFGCache->nViewType))
            {
                return EFAILED;
            }
            
            pCFGCache->nViewType = *(byte *)data;
            break;
             
        case CONTCFG_SAVETYPE:
            // 校验数据长度
            if(dataLen != sizeof(pCFGCache->nSaveType))
            {
                return EFAILED;
            }
            
            pCFGCache->nSaveType = *(byte *)data;
            break;
            
        case CONTCFG_PASSWORD:
            // 校验数据长度
            if(WSTRLEN((AECHAR *)data) > MAX_LEN_PASSWORD)
            {
                return EFAILED;
            }
            
            WSTRCPY(pCFGCache->wPassWord, (AECHAR *)data);
            break;
            
        case CONTCFG_SMARTDIAL:
            // 校验数据长度
            if(dataLen != sizeof(pCFGCache->bSmartDial))
            {
                return EFAILED;
            }
            
            pCFGCache->bSmartDial = *(boolean *)data;
            break;
            
        case CONTCFG_ONEDIAL1:
            // 校验数据长度
            ERR("dataLen=%d",WSTRLEN((AECHAR *)data),0,0);
            if(WSTRLEN((AECHAR *)data) > MAX_INPUT_NUM)
            {
                return EFAILED;
            }
            
            WSTRCPY(pCFGCache->wOneDial1, (AECHAR *)data);
            break;
            
        case CONTCFG_ONEDIAL2:
            // 校验数据长度
            if(WSTRLEN((AECHAR *)data) > MAX_INPUT_NUM)
            {
                return EFAILED;
            }
            
            WSTRCPY(pCFGCache->wOneDial2, (AECHAR *)data);
            break;
            
        case CONTCFG_ONEDIAL3:
            // 校验数据长度
            if(WSTRLEN((AECHAR *)data) > MAX_INPUT_NUM)
            {
                return EFAILED;
            }
            
            WSTRCPY(pCFGCache->wOneDial3, (AECHAR *)data);
            break;
            
        case CONTCFG_ONEDIAL4:
            // 校验数据长度
            if(WSTRLEN((AECHAR *)data) > MAX_INPUT_NUM)
            {
                return EFAILED;
            }
            
            WSTRCPY(pCFGCache->wOneDial4, (AECHAR *)data);
            break;
            
        case CONTCFG_ONEDIAL5:
            // 校验数据长度
            if(WSTRLEN((AECHAR *)data) > MAX_INPUT_NUM)
            {
                return EFAILED;
            }
            
            WSTRCPY(pCFGCache->wOneDial5, (AECHAR *)data);
            break;
            
        case CONTCFG_ONEDIAL6:
            // 校验数据长度
            if(WSTRLEN((AECHAR *)data) > MAX_INPUT_NUM)
            {
                return EFAILED;
            }
            
            WSTRCPY(pCFGCache->wOneDial6, (AECHAR *)data);
            break;
        
        case CONTCFG_ONEDIAL7:
            // 校验数据长度
            if(WSTRLEN((AECHAR *)data) > MAX_INPUT_NUM)
            {
                return EFAILED;
            }
            
            WSTRCPY(pCFGCache->wOneDial7, (AECHAR *)data);
            break;
            
        case CONTCFG_ONEDIAL8:
            // 校验数据长度
            if(WSTRLEN((AECHAR *)data) > MAX_INPUT_NUM)
            {
                return EFAILED;
            }
            
            WSTRCPY(pCFGCache->wOneDial8, (AECHAR *)data);
            break;
            
        case CONTCFG_ONEDIAL9:
            // 校验数据长度
            if(WSTRLEN((AECHAR *)data) > MAX_INPUT_NUM)
            {
                return EFAILED;
            }
            
            WSTRCPY(pCFGCache->wOneDial9, (AECHAR *)data);
            break;

        case CONTCFG_GROUP1:
            // 校验数据长度
            if(WSTRLEN((AECHAR *)data) > MAX_INPUT_NAME_EN)
            {
                return EFAILED;
            }
            
            WSTRCPY(pCFGCache->wGroup1, (AECHAR *)data);
            break;
            
        case CONTCFG_GROUP2:
            // 校验数据长度
            if(WSTRLEN((AECHAR *)data) > MAX_INPUT_NAME_EN)
            {
                return EFAILED;
            }
            
            WSTRCPY(pCFGCache->wGroup2, (AECHAR *)data);
            break;
            
        case CONTCFG_GROUP3:
            // 校验数据长度
            if(WSTRLEN((AECHAR *)data) > MAX_INPUT_NAME_EN)
            {
                return EFAILED;
            }
            
            WSTRCPY(pCFGCache->wGroup3, (AECHAR *)data);
            break;
        
        case CONTCFG_GROUP4:
            // 校验数据长度
            if(WSTRLEN((AECHAR *)data) > MAX_INPUT_NAME_EN)
            {
                return EFAILED;
            }
            
            WSTRCPY(pCFGCache->wGroup4, (AECHAR *)data);
            break;
            
        case CONTCFG_GROUP5:
            // 校验数据长度
            if(WSTRLEN((AECHAR *)data) > MAX_INPUT_NAME_EN)
            {
                return EFAILED;
            }
            
            WSTRCPY(pCFGCache->wGroup5, (AECHAR *)data);
            break;
            
        case CONTCFG_GROUP6:
            // 校验数据长度
            if(WSTRLEN((AECHAR *)data) > MAX_INPUT_NAME_EN)
            {
                return EFAILED;
            }
            
            WSTRCPY(pCFGCache->wGroup6, (AECHAR *)data);
            break;
       case CONTCFG_GROUP7:
           // 校验数据长度
           if(WSTRLEN((AECHAR *)data) > MAX_INPUT_NAME_EN)
           {
               return EFAILED;
           }
           
           WSTRCPY(pCFGCache->wGroup7, (AECHAR *)data);
           break;
           
#ifdef FEATURE_LANG_CHINESE
       case CONTTCFG_QUICKSEARCH_INPUT_MODE:
       // 校验数据长度
       if(dataLen != sizeof(pCFGCache->inputMode))
       {
           return EFAILED;
       }
       
       pCFGCache->inputMode = *(byte *)data;
       break;


#endif
        default:
            return EFAILED;
    }
    
    // Write the config data
    return CContApp_WriteCFG(pMe->m_pFileMgr, pCFGCache);
} // CContApp_SetConfig

/*=============================================================================
FUNCTION: CContApp_BuildStartArgs

DESCRIPTION: Builds a startup argument string

PARAMETERS:
   method: desired 'startup method'.  
   saveType: Phone number save Type(Only useful in case method is
             STARTMETHOD_SAVEFIELD All other methods should pass this as null
   *arg: optional startup method specific argument

RETURN VALUE:
    char *:  Encoded startup argument string that may be used with
             ISHELL_StartAppletArgs().  

             If non-NULL, this string must be deallocated by the caller

COMMENTS:

SIDE EFFECTS:

SEE ALSO:
   CContactApp_SaveNumber()
   CContactApp_FindNumber()
   CContactApp_SelectField()

=============================================================================*/
static char *CContApp_BuildStartArgs( AppletStartMethod method,
                                      void             *saveType,
                                      void             *arg)
{
    char *str = NULL;
    switch (method) 
    {
        case STARTMETHOD_NORMAL:
            break;
        
        case STARTMETHOD_SAVEFIELD:
            if ((arg != NULL)&& (saveType!=NULL)) 
            {
                uint32 len;
                
                len = (uint32) WSTRLEN((AECHAR*) arg);
                
                // Note:UTF8 
                // encoding can occupy anything from 0.5x to 4x more space
                // than Unicode encoding of the same string.   We take the
                // paranoid (and wasteful!) approach and always assume it will
                // take 4x the space
                //In the first char we will store the method and the second char
                // we will store the save type. Hence we have 2 in the begining
                str = MALLOC(1 + len * 4);
                
                // Save the start type at first character
                *str = STARTARGPREFIX_SAVENUMBER;
                
                if (str) 
                {
                    //store the saveType in the second character
                    switch(*(PhoneSaveType *)saveType)
                    {
                        case ADD_FIELD:
                            str[1] = STARTARGPREFIX_ADD_FLD;
                            break;
                        
                        case SAVE_NUMBER:
                            str[1] = STARTARGPREFIX_SAVE_NUMBER;
                            break;
                            
                        case SAVE_EMAIL:
                            str[1] = STARTARGPREFIX_SAVE_EMAIL;
                            break;
                            
                        case SAVE_URL:
                        default:
                            str[1] = STARTARGPREFIX_SAVE_URL;
                            break;
                    }
                    
                    (void)WSTRTOUTF8((AECHAR *) arg, 
                                     (int)      len, 
                                     (byte *)  &str[2], 
                                     (int)     (len*4));
                }
            }
            break;
            
        case STARTMETHOD_FINDNUMBER:
            if (arg != NULL) 
            {
                uint32 len;
                
                len = (uint32) WSTRLEN((AECHAR*) arg);
                
                // Note: 
                // UTF8 encoding can occupy anything from 0.5x to 4x more space
                // than Unicode encoding of the same string.   We take the
                // paranoid (and wasteful!) approach and always assume it will
                // take 4x the space
                str = MALLOC(1 + len * 4);
                
                if (str) 
                {
                    *str = STARTARGPREFIX_FINDNUMBER;
                    
                    (void) WSTRTOUTF8((AECHAR *) arg,
                                      (int)      len, 
                                      (byte *)  &str[1], 
                                      (int)     (len*4));
                }
            }
            break;
            
        case STARTMETHOD_SELECTFIELD:
            if (arg != NULL) 
            {
                SelectFldType selectFld;
                char buf[20];
                
                if ( saveType == NULL )
                {
                    selectFld = SINGLE_SELECT_NUMBER;
                }
                else
                {
                    selectFld = *((SelectFldType*)saveType);
                }

                if(*((SelectFldType*)saveType) == MULTI_SELECT_GROUPLIST)
                {
                    SPRINTF(buf, "%c%c%d", 
                        STARTARGPREFIX_GROUPLIST, selectFld, (uint32) arg);
                }
                else if(*((SelectFldType*)saveType) == SINGLE_SELECT_GROUPLIST)
                {
                    SPRINTF(buf, "%c%c%d", 
                        STARTARGPREFIX_GROUPLIST, selectFld, (uint32) arg);
                }
                else
                {
                    SPRINTF(buf, "%c%c%d", 
                        STARTARGPREFIX_SELECTFIELD, selectFld, (uint32) arg);
                }
                
                str = STRDUP(buf);
            }
            break;
            
        case STARTMETHOD_SMARTCALL:
            if (arg != NULL) 
            {
                uint32 len;
                
                len = (uint32) WSTRLEN((AECHAR*) arg);
                
                // Note: 
                // UTF8 encoding can occupy anything from 0.5x to 4x more space
                // than Unicode encoding of the same string.   We take the
                // paranoid (and wasteful!) approach and always assume it will
                // take 4x the space
                str = MALLOC(1 + len * 4);
                
                if (str) 
                {
                    *str = STARTARGPREFIX_SMARTCALL;
                    
                    (void) WSTRTOUTF8((AECHAR *) arg,
                                      (int)      len, 
                                      (byte *)  &str[1], 
                                      (int)     (len*4));
                }
            }
            break;

         case STARTMETHOD_MAINMENU:
                str = MALLOC(1);
                *str = STARTARGPREFIX_MAINMENU;
      	     break;
            
        default:
            ASSERT_NOT_REACHABLE;
            break;
    }
    
    return str;
}

/*=============================================================================
FUNCTION: CContApp_DecodeStartArgs

DESCRIPTION:  Decodes a startup argument string passed into EVT_APP_START

PARAMETERS:
   *pMe:  CContactApp object pointer
   *args: startup argument string

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:
   Sets the CContApp::m_eStartMethod field and possibly other fields
   depending on the contents of the startup argument string.

SEE ALSO:
=============================================================================*/
static void CContApp_DecodeStartArgs(CContApp *pMe, char *args)
{
    uint32  len;
    AECHAR *wStr;
    
    if ( pMe == NULL ) 
    {
        return;
    }
    
    if (NULL == args) 
    {
        return;
    }
    
    len = STRLEN(args);
    if (len < 1) 
    {
        return;
    }
    
    // Default to 'normal start method'
    pMe->m_eStartMethod        = STARTMETHOD_NORMAL;
    pMe->m_ePreState           = STATE_NONE;
    pMe->m_eCurState           = STATE_LOAD;
    pMe->m_pActiveDlg          = NULL;
    pMe->m_eDlgRet             = DLGRET_CREATE;
    pMe->m_bNotOverwriteDlgRet = FALSE;
    
    pMe->m_wSelectCont         = CONTAPP_RECID_NULL;
    pMe->m_wSelectSmart        = CONTAPP_RECID_NULL;
    
    switch (*args) 
    {
        case STARTARGPREFIX_SAVENUMBER:
            if (len < 2) 
            {
                // invalid argument length
                break;
            }
            
            len = STRLEN(&args[2]) + 1;
            
            // Note:
            // UTF8 encoding can occupy anything from 0.5x to 4x more space
            // than Unicode encoding of the same string.   We take the
            // paranoid (and wasteful!) approach and always assume that the
            // Unicode string will take 2x the length of the UTF8 string
            wStr = MALLOC(len * sizeof(AECHAR));
            
            if (NULL == wStr) 
            {
                break;
            }
            
            (void) UTF8TOWSTR((byte *) &args[2],
                              (int) len,  
                              wStr, 
                              (int)(len*sizeof(AECHAR)));
            
            
            //decode the first character where the method is stored
            //pMe->m_eStartMethod = STARTMETHOD_SAVEFIELD;  // move to switch follow
           
            //decode the second character where the PhoneSaveType is stored
            switch(args[1])
            {
                case STARTARGPREFIX_ADD_FLD:
                    pMe->m_eFldSaveType = ADD_FIELD;
                    FREEIF(pMe->m_pFldInputBuf);
                    pMe->m_pFldInputBuf =  wStr;
                    pMe->m_eStartMethod = STARTMETHOD_ADDFIELD;
                    break;
                
                case STARTARGPREFIX_SAVE_NUMBER:
                    pMe->m_eFldSaveType = SAVE_NUMBER;	
                    FREEIF(pMe->m_pAddNewMobile);
                    pMe->m_pAddNewMobile =  wStr;	
                    pMe->m_nAddNewFldID = AEE_ADDRFIELD_PHONE_GENERIC;
                    pMe->m_eStartMethod = STARTMETHOD_SAVEFIELD;
                    break;
                    
                case STARTARGPREFIX_SAVE_EMAIL:                    
                    pMe->m_eFldSaveType = SAVE_EMAIL;
                    //FREEIF(pMe->m_pAddNewEMAIL);
                    //pMe->m_pAddNewEMAIL =  wStr;
                    FREEIF(pMe->m_pFldInputBuf);
                    pMe->m_pFldInputBuf =  wStr;
                    pMe->m_nAddNewFldID = AEE_ADDRFIELD_EMAIL;
                    pMe->m_eStartMethod = STARTMETHOD_ADDEMAIL;
                    break;
                    
                case STARTARGPREFIX_SAVE_URL:
                default:
                    pMe->m_eFldSaveType = SAVE_URL;
                    //FREEIF(pMe->m_pAddNewURL);
                    //pMe->m_pAddNewURL =  wStr;
                    FREEIF(pMe->m_pFldInputBuf);
                    pMe->m_pFldInputBuf =  wStr;
                    pMe->m_nAddNewFldID = AEE_ADDRFIELD_URL;
                    pMe->m_eStartMethod = STARTMETHOD_ADDURL;
                    break;
            }

            break;
            
        //If the start method is find number
        case STARTARGPREFIX_FINDNUMBER:
            if (len < 2) 
            {
                // invalid argument length
                break;
            }
            
            len = STRLEN(&args[1]) + 1;
            
            // Note: UTF8 encoding can occupy anything from 0.5x to 4x more space
            //       than Unicode encoding of the same string.   We take the
            //       paranoid (and wasteful!) approach and always assume that the
            //       Unicode string will take 2x the length of the UTF8 string
            wStr = MALLOC(len * sizeof(AECHAR));
            
            if (NULL == wStr) 
            {
                break;
            }
            
            (void) UTF8TOWSTR((byte *)&args[1],
                              (int)len, 
                              wStr, 
                              (int)(len*sizeof(AECHAR)));
            
            pMe->m_eStartMethod = STARTMETHOD_FINDNUMBER;
            FREEIF(pMe->m_pFindNum);
            pMe->m_pFindNum     = wStr;
            break;

        case STARTARGPREFIX_GROUPLIST:
            pMe->m_eStartMethod = STARTMETHOD_GROUPLIST;
            pMe->m_wSelFldType  =(SelectFldType)args[1];
            pMe->m_pSelFldCB    =(PFNSELECTEDCONTACTCB)ATOI(args + 2);
            break;
            
        case STARTARGPREFIX_SELECTFIELD:
            pMe->m_eStartMethod = STARTMETHOD_SELECTFIELD;
            pMe->m_wSelFldType  =(SelectFldType)args[1];
            pMe->m_pSelFldCB    =(PFNSELECTEDCONTACTCB)ATOI(args + 2);
            break;
            
        //If the start method is Smart call
        case STARTARGPREFIX_SMARTCALL:
            if (len < 2) 
            {
                // invalid argument length
                break;
            }
            
            len = STRLEN(&args[1]) + 1;
            
            // Note: UTF8 encoding can occupy anything from 0.5x to 4x more space
            //       than Unicode encoding of the same string.   We take the
            //       paranoid (and wasteful!) approach and always assume that the
            //       Unicode string will take 2x the length of the UTF8 string
            wStr = MALLOC(len * sizeof(AECHAR));
            
            if (NULL == wStr) 
            {
                break;
            }
            
            (void) UTF8TOWSTR((byte *)&args[1],
                              (int)len, 
                              wStr, 
                              (int)(len*sizeof(AECHAR)));
            
            pMe->m_eStartMethod = STARTMETHOD_SMARTCALL;
            FREEIF(pMe->m_pFindNum);
            pMe->m_pFindNum     = wStr;
            break;

        //If the start method is Mainmenu
        case STARTARGPREFIX_MAINMENU:
            pMe->m_eStartMethod = STARTMETHOD_MAINMENU;
            break;			
            
        default:
            break;
    }
}

/*==============================================================================

函数: IContApp_SaveNumber
       
说明: 
    外部调用保存记录接口
       
参数: 
    pi      [in]:IContApp接口对象指针。
    number  [in]:
    method  [in]:
    saveType[in]:    
       
返回值:
    SUCCESS:
       
备注:
       
==============================================================================*/
static int IContApp_SaveNumber( IContApp            *pi, 
                                AECHAR              *Number,
                                AECHAR              *E_Mail,     
                                AECHAR              *URL,                                
                                PhoneSaveType  saveType)
{
    CContApp *pMe = (CContApp*)pi;
    char     *args = NULL;
    int       nRet;
	    
    if ( pMe == NULL ) 
    {
        return EFAILED;
    }

    if ((NULL == Number) && (NULL == E_Mail) &&(NULL == URL))
    {
        return EFAILED;
    }

    if (NULL != Number)
    {
        args = CContApp_BuildStartArgs( STARTMETHOD_SAVEFIELD, &saveType, Number);		
    }
    else if (NULL != E_Mail)
    {
        args = CContApp_BuildStartArgs( STARTMETHOD_SAVEFIELD, &saveType, E_Mail);
    }
    else   if (NULL != URL)
    {
        args = CContApp_BuildStartArgs( STARTMETHOD_SAVEFIELD, &saveType, URL);
    }
    
    pMe->m_eStartMethod = STARTMETHOD_SAVEFIELD;
    
    if (TRUE == pMe->m_bActive) 
    {
        // Decode the arguments if applet have run
        CContApp_DecodeStartArgs(pMe, args);
    }

	
    
    nRet = ISHELL_StartAppletArgs(  pMe->m_pShell,
                                    AEECLSID_APP_CONTACT, 
                                    args);
    
    if (args) 
    {
        FREEIF(args);
    }
    
    return nRet;
} // IContApp_SaveNumber

/*==============================================================================

函数: IContApp_FindNumber
       
说明: 
    外部调用查找记录接口
       
参数: 
    pi       [in]:IContApp接口对象指针。
    searchStr[in]:
    
返回值:
    SUCCESS:
       
备注:
       
==============================================================================*/
static int IContApp_FindNumber(IContApp *pi, AECHAR *searchStr)
{
    CContApp *pMe = (CContApp*)pi;
    char        *args;
    int          nRet;

    if ( pMe == NULL ) 
    {
        return EFAILED;
    }

    if (NULL == searchStr || WSTRLEN(searchStr) <= 0) 
    {
        return EFAILED;
    }
    
    args = CContApp_BuildStartArgs( STARTMETHOD_FINDNUMBER,
                                    NULL,
                                    searchStr);
    
    if (TRUE == pMe->m_bActive) 
    {
        // Decode the arguments if applet have run
        CContApp_DecodeStartArgs(pMe, args);
    }
    
    nRet = ISHELL_StartAppletArgs( pMe->m_pShell,
                                   AEECLSID_APP_CONTACT,
                                   args);
    
    if (args) 
    {
        FREEIF(args);
    }
    
    return nRet;
} // IContApp_FindNumber

/*==============================================================================

函数: IContApp_MainMenu
       
说明: 
    外部调用查找记录接口
       
参数: 
    pi       [in]:IContApp接口对象指针。
    searchStr[in]:
    
返回值:
    SUCCESS:
       
备注:
       
==============================================================================*/
static int IContApp_MainMenu(IContApp *pi)
{
    CContApp *pMe = (CContApp*)pi;
    char        *args;
    int          nRet;

    if ( pMe == NULL ) 
    {
        return EFAILED;
    }
   
    args = CContApp_BuildStartArgs( STARTMETHOD_MAINMENU,
                                    NULL,
                                    NULL);
    
    if (TRUE == pMe->m_bActive) 
    {
        // Decode the arguments if applet have run
        CContApp_DecodeStartArgs(pMe, args);
    }
    
    nRet = ISHELL_StartAppletArgs( pMe->m_pShell,
                                   AEECLSID_APP_CONTACT,
                                   args);
    
    if (args) 
    {
        FREEIF(args);
    }
    
    return nRet;
} // IContApp_FindNumber

/*==============================================================================

函数: IContApp_SelectField
       
说明: 
    外部调用选择字段记录接口
       
参数: 
    pi     [in]:IContApp接口对象指针。
    fnCB   [in]:
    FldType[in]:
    
返回值:
    SUCCESS:
       
备注:
       
==============================================================================*/
static int IContApp_SelectField( IContApp            *pi, 
                                 PFNSELECTEDCONTACTCB fnCB, 
                                 SelectFldType        FldType)
{
    CContApp *pMe = (CContApp*)pi;
    char        *args;
    int          nRet;

    if ( pMe == NULL ) 
    {
        return EFAILED;
    }
    
    args = CContApp_BuildStartArgs( STARTMETHOD_SELECTFIELD,
                                   &FldType,
                                   (void *) (uint32) fnCB);
    
    if (TRUE == pMe->m_bActive) 
    {
        // Decode the arguments if applet have run
        CContApp_DecodeStartArgs(pMe, args);
    }
    
    nRet = ISHELL_StartAppletArgs( pMe->m_pShell,
                                   AEECLSID_APP_CONTACT,
                                   args);
            
    if (args) 
    {
        FREEIF(args);
    }
    
    return nRet;
} // IContApp_SelectField

/*==============================================================================

函数: IContApp_NumberLookup
       
说明: 
    查找对应号码的姓名等信息
       
参数: 
    pi       [in]    : IContApp接口对象指针。
    pNumber  [in]    : Number
    pContInfo[in/out]: 查询到的信息输出
返回值:
    SUCCESS: 如果存在该记录
       
备注:
       
==============================================================================*/
static int IContApp_NumberLookup( IContApp   *pi,
                                  AECHAR     *pNumber,
                                  ContInfo   *pContInfo)
{
    CContApp *pMe = (CContApp*)pi;
    IAddrBook  *pAddr = NULL;
    AEEAddCacheInfo info = {0};
    AEEAddCacheInfo *pUseinfo = NULL;
#ifdef FEATURE_RUIM_PHONEBOOK
    AEEAddCacheInfo uiminfo = {0};
    boolean    bNeedSearchUIM = FALSE;
#endif
    
    int nRet;
    
    if ((pNumber == NULL) || (NULL == pContInfo))
    {
        return EFAILED;
    }
    #ifndef FEATURE_OEMOMH
    pMe->m_bADNRUIMSupport = TRUE;
    #endif
    if (WSTRLEN(pNumber) <= 1)
    {
        // 小于3位的号码不查找
        return EFAILED;
    }

    // Create IAddrbokk instance for phone
    if (SUCCESS != ISHELL_CreateInstance((AEE_GetShell()),
                                         AEECLSID_ADDRBOOK,
                                         (void **)&pAddr))
    {
        return EFAILED;
    }
    
    nRet = IADDRBOOK_GetCacheinfoByNumber(pAddr, pNumber, &info, ContApp_NumberMatch);

    if(SUCCESS == nRet)
    {
        CContApp_GetRingtone(pAddr, pContInfo->ringName, info.wRecID);
    }
    
    (void) IADDRBOOK_Release(pAddr);
    pAddr = NULL;
    
    if (nRet != SUCCESS)
    {
#ifdef FEATURE_RUIM_PHONEBOOK
        if (IsRunAsUIMVersion() && pMe->m_bADNRUIMSupport)
        {
            bNeedSearchUIM = TRUE;
        }
#endif
    }
#ifdef FEATURE_RUIM_PHONEBOOK
    else if ((info.szNumber != NULL) &&
             (ContApp_NumberMatch(info.szNumber, pNumber, NULL) != NUMBERMATCH_EQUAL) &&
             IsRunAsUIMVersion() && pMe->m_bADNRUIMSupport)
    {
        pUseinfo = &info;
        bNeedSearchUIM = TRUE;
    }
#endif    
    else
    {
        pUseinfo = &info;
    }
    
#ifdef FEATURE_RUIM_PHONEBOOK
    if (bNeedSearchUIM)
    {
        if (SUCCESS != ISHELL_CreateInstance((AEE_GetShell()),
                                             AEECLSID_ADDRBOOK_RUIM,
                                             (void **)&pAddr))
        {
            FREEIF(info.szName);
            FREEIF(info.szNumber);
            return EFAILED;
        }
        
        nRet = IADDRBOOK_GetCacheinfoByNumber(pAddr, pNumber, &uiminfo, ContApp_NumberMatch);
        (void) IADDRBOOK_Release(pAddr);
        pAddr = NULL;
        
        if (nRet == SUCCESS)
        {
            int nLen_uim = 0;
            int nLen_Ph = 0;
            Numbermatch_e_Type eRet_uim;
            Numbermatch_e_Type eRet_ph;
            
            eRet_uim = ContApp_NumberMatch(uiminfo.szNumber, pNumber, &nLen_uim);
            if (pUseinfo != NULL)
            {
                eRet_ph = ContApp_NumberMatch(pUseinfo->szNumber, pNumber, &nLen_Ph);
            }
            
            switch (eRet_uim)
            {
                // 匹配度最高
                case NUMBERMATCH_EQUAL:
                    pUseinfo = &uiminfo;
                    break;
                    
                case NUMBERMATCH_WSTR1_ISTAIL_OF_WSTR2:
                    if (NULL == pUseinfo)
                    {
                        pUseinfo = &uiminfo;
                    }
                    else
                    {
                        switch (eRet_ph)
                        {
                            case NUMBERMATCH_WSTR1_ISTAIL_OF_WSTR2:
                                if (nLen_uim>nLen_Ph)
                                {// UIM 上的号码更匹配
                                    pUseinfo= &uiminfo;
                                }
                                break;
                                
                            case NUMBERMATCH_WSTR2_ISTAIL_OF_WSTR1:
                            default:
                                break;
                        }
                    }
                    break;

                case NUMBERMATCH_WSTR2_ISTAIL_OF_WSTR1:
                    if (NULL == pUseinfo)
                    {
                        pUseinfo = &uiminfo;
                    }
                    break;
                
                default:
                    break;
            }
        }
    }
#endif
    
    nRet = EFAILED;
    if (NULL != pUseinfo)
    {
        if(IS_RUIM_REC(pUseinfo->wRecID))
        {
            pContInfo->ringName[0] = (AECHAR)'\0';
        }
        
        if (NULL != pUseinfo->szName)
        {
            if (pContInfo->wNameLen > WSTRLEN(pUseinfo->szName))
            {
                (void)WSTRCPY(pContInfo->pName, pUseinfo->szName);
            }
        }
        nRet = SUCCESS;
    }
    FREEIF(info.szName);
    FREEIF(info.szNumber);
#ifdef FEATURE_RUIM_PHONEBOOK
    FREEIF(uiminfo.szName);
    FREEIF(uiminfo.szNumber);
#endif    
    
   return nRet;
} // IContApp_NumberLookup

/*==============================================================================

函数: CContApp_CmpNumber
       
说明: 
    精确匹配号码
       
参数: 
    pAddrRec: 
    pNum    : 
    
返回值:
    boolean
       
备注:
       
==============================================================================*/
boolean CContApp_CmpNumber(IAddrRec *pAddrRec, AECHAR *pNumber)
{
    int           i;
    int           nFldsNum;
    AEEAddrField *pFld;
    
    //if(NULL == pAddrRec || pNum == NULL)
    //{
    //    return FALSE;
    //}
    
    nFldsNum = IADDRREC_GetFieldCount(pAddrRec);
    for(i=0; i<nFldsNum; i++)
    {
        pFld = IADDRREC_GetField( pAddrRec, i );
        if(NULL == pFld)
        {
            continue;
        }
        
        if(IS_NUM_FIELD(pFld->fID))
        {
            if(0 == WSTRCMP(pFld->pBuffer, pNumber))
            {
                return TRUE;
            }
        }
    }
    
    return FALSE;
}// CContApp_CmpNumber


/*==============================================================================

函数: CContApp_CmpRegionNumber
       
说明: 
    匹配号码的区号
       
参数: 
    pAddrRec: 
    pNum    : 
    
返回值:
    boolean
       
备注:
       
==============================================================================*/
boolean CContApp_CmpRegionNumber(IAddrRec *pAddrRec, AECHAR *pNumber)
{
    int           i;
    int           nFldsNum;
    AEEAddrField *pFld;
    AECHAR       *pStr;
    
    //if(NULL == pAddrRec || pNum == NULL)
    //{
    //    return FALSE;
    //}
    
    nFldsNum = IADDRREC_GetFieldCount(pAddrRec);
    for(i=0; i<nFldsNum; i++)
    {
        pFld = IADDRREC_GetField( pAddrRec, i );
        if(NULL == pFld)
        {
            continue;
        }
        
        if(IS_NUM_FIELD(pFld->fID))
        {
            pStr = (AECHAR *)pFld->pBuffer;
            if(CContApp_WStrStrEx(pStr, pNumber))
            {
#ifdef FEATURE_PERU_VERSION
//correctly inquire Peruvian cellular phone number in phone book
            	if(WSTRLEN(pNumber) >= 7)

            	{
            	     return TRUE;
                }
#else            
                if(pStr[0]=='0' || pStr[0]=='+')
                {
                    return TRUE;
                }
                else if(WSTRLEN(pNumber) >= 10)
                {
                    return TRUE;
                }
#endif //FEATURE_PERU_VERSION                                
                break;
            }
            else if(CContApp_WStrStrEx(pNumber, pStr))
            {
                if(pNumber[0]=='0' || pNumber[0]=='+')
                {
                    return TRUE;
                }
                else if(WSTRLEN(pStr) >= 10)
                {
                    return TRUE;
                }
                
                break;
            }
        }
    }
    
    return FALSE;
}// CContApp_CmpRegionNumber

/*=============================================================================
FUNCTION: CContApp_WStrStrEx

DESCRIPTION:
   Widestring strstr() function with case sensitive compare flag.

PARAMETERS:
   *pHaystack: string in which pNeedle is found
   *pNeedle:  string which is searched in pHaystack

RETURN VALUE:
boolean:
   Returns true if pNeedle is found in pHaystack. 

COMMENTS:

SIDE EFFECTS:

=============================================================================*/
static boolean CContApp_WStrStrEx( AECHAR *pHaystack, AECHAR *pNeedle)
{
   AECHAR *pTempNdl, *pTempHay;
   AECHAR char1[2],char2[2];

   if((pHaystack == NULL)||(pNeedle == NULL))
   {
       return FALSE;
   }

   char1[1] = char2[1] = (AECHAR)0;

   for (;;)
   {
      pTempNdl = pNeedle;
      pTempHay = pHaystack;


      if ((AECHAR)0 == *pHaystack)
      {
         return FALSE;
      }
      
      for(;;)
      {
         char1[0] = *pTempHay;
         char2[0] = *pTempNdl;
         
         // Compare the characters
         if ((char1[0] == char2[0] )&&(char1[0]!=(AECHAR)0))
         {
            pTempNdl++;
            pTempHay++;
         }
         else
         {
            break;
         }
      }
      
      if ((AECHAR)0 == *pTempNdl) {
         return TRUE;
      }
      
      pHaystack++;
   }
}// CContApp_WStrStrEx


/*==============================================================================

函数: CContApp_BuildContInfo
       
说明: 
    构建 Contact info 结构
       
参数: 
    pAddrRec  : 
    pContInfo : 
    
返回值:
    None
       
备注:
       
==============================================================================*/
void CContApp_BuildContInfo(IAddrRec *pAddrRec, ContInfo *pContInfo)
{
    int           i;
    int           nFldsNum;
    AEEAddrField *pFld;
    
    if(NULL == pAddrRec || pContInfo == NULL)
    {
        ERR("NULL", 0, 0, 0);
        return;
    }
    
    nFldsNum = IADDRREC_GetFieldCount(pAddrRec);
    for(i=0; i<nFldsNum; i++)
    {
        pFld = IADDRREC_GetField( pAddrRec, i );
        if(NULL == pFld)
        {
            continue;
        }
        
        switch(pFld->fID)
        {
            case AEE_ADDRFIELD_NAME:
                if(pContInfo->pName)
                {
                    WSTRNCOPYN(pContInfo->pName,
                               (int)pContInfo->wNameLen,
                               (AECHAR *)pFld->pBuffer,
                               -1);
                }
                break;
           //guoys modified for add ringtone     
            case AEE_ADDRFIELD_RINGTONE:
                if(pFld->pBuffer != NULL)
                {
                    WSTRCPY(pContInfo->ringName, (AECHAR*)pFld->pBuffer);
                }
                break;
            
            case AEE_ADDRFIELD_PHOTO:
            {
                char szName[AEE_MAX_FILE_NAME] = {0};
                int  len;
                
                len = WSTRLEN(pFld->pBuffer);
                
                if(len == 0)
                {
                    break;
                }
                
                // Generate the filename
                STRCPY(pContInfo->szPhoto, MP_CONTPIC_DIR);
                STRCAT(pContInfo->szPhoto, DIRECTORY_STR);
                (void)WSTRTOUTF8( (AECHAR *) pFld->pBuffer,
                                  (int)      len, 
                                  (byte *)   szName, 
                                  (int)      sizeof(szName));;
                STRCAT(pContInfo->szPhoto, szName);
                break;
            }
            
            default:
                break;
        }
    }
}// CContApp_BuildContInfo

/*==============================================================================

函数: IContApp_SpeedDialLookup
       
说明: 
    查找对应的快速拨号信息
       
参数: 
    pMe       [in]: IContApp接口对象指针。
    pSpeedDial[in]: Speed dial text
    pNumber   [in]: Number
    wNumLen   [in]: 查询到的信息输出buffer长度（in AECHAR）
    
返回值:
    SUCCESS: 如果存在该记录
       
备注:
       
==============================================================================*/
static int IContApp_SpeedDialLookup( IContApp   *pi,
                                     AECHAR     *pSpeedDial,
                                     AECHAR     *pNumber,
                                     uint16      wNumLen)
{
    CContApp *pMe = (CContApp*)pi;
    
    if ( pMe == NULL || pSpeedDial == NULL) 
    {
        return EFAILED;
    }
    
    // Nothing to look up
    if(WSTRLEN(pSpeedDial) == 0)
    {
        return EFAILED;
    }
    
    // OneDial 单键拨号
    if(WSTRLEN(pSpeedDial) == 1)
    {
        return CContApp_GetOneDial(pMe, pSpeedDial, pNumber, wNumLen);
    }
    else if(WSTRLEN(pSpeedDial) <= MAX_LEN_SPEEDDIAL)
    {
        if(0 != CContApp_GetSmartDial(pMe, pSpeedDial, pNumber, wNumLen))
        {
            return SUCCESS;
        }
    }// else it is not speed dial then return EFAILED
    
    return EFAILED;
} // IContApp_SpeedDialLookup

/*==============================================================================

函数: IContApp_SpeedDialCall
       
说明: 
    呼叫对应的快速拨号信息
       
参数: 
    pMe       [in]: IContApp接口对象指针。
    pSpeedDial[in]: Speed dial text
    
返回值:
    SUCCESS: 如果存在该记录
       
备注:
       
==============================================================================*/
static int IContApp_SpeedDialCall(IContApp *pi, AECHAR *pSpeedDial)
{
    CContApp *pMe = (CContApp*)pi;
    AECHAR    pNumber[MAX_INPUT_NUM+1];
    int       nCnt;
    char     *args;
    int       nRet;
    
    if ( pMe == NULL || pSpeedDial == NULL) 
    {
        return EFAILED;
    }

    // Nothing to look up
    if(WSTRLEN(pSpeedDial) == 0)
    {
        return EFAILED;
    }

    // One dial call not smart call
    if(WSTRLEN(pSpeedDial) == 1)
    {
        return CContApp_OneDialCall(pMe, pSpeedDial);
    }
    
    // Check the smart call
    nCnt = CContApp_GetSmartDial(pMe, pSpeedDial, pNumber, MAX_INPUT_NUM);
    if(0 == nCnt)
    {
        return EFAILED;
    }
    else if(1 == nCnt)
    {
#if 1
        WSTRLCPY(pSpeedDial,pNumber,MAX_INPUT_NUM);
#else
        // Call this number
        // Create the Call instance
        if ( SUCCESS != ISHELL_CreateInstance( (AEE_GetShell()),
                                               AEECLSID_DIALER,
                                               (void **)&pMe->m_pCallApp))
        {
            return EFAILED;
        }
        
        // Make a voice call
        ICallApp_CallNumber(pMe->m_pCallApp, pNumber);
        
        if (pMe->m_pCallApp != NULL) 
        {
            ICallApp_Release(pMe->m_pCallApp);
            pMe->m_pCallApp = NULL;
        }
#endif
        return SUCCESS;
    }// else start the contacts applet
    
    // Smart call
    args = CContApp_BuildStartArgs( STARTMETHOD_SMARTCALL,
                                    NULL,
                                    pSpeedDial);
    
    if (TRUE == pMe->m_bActive) 
    {
        // Decode the arguments if applet have run
        CContApp_DecodeStartArgs(pMe, args);
    }
    
    nRet = ISHELL_StartAppletArgs( AEE_GetShell(),
                                   AEECLSID_APP_CONTACT,
                                   args);
    
    if (args) 
    {
        FREEIF(args);
    }
    
    return nRet;
} // IContApp_SpeedDialCall

/*==============================================================================

函数: CContApp_OneDialCall
       
说明: 
    呼叫对应的快速拨号信息
       
参数: 
    pMe     [in]: CContApp接口对象指针。
    pOneDial[in]: Speed dial text
    
返回值:
    SUCCESS: 如果存在该记录
       
备注:
       
==============================================================================*/
static int CContApp_OneDialCall(CContApp *pMe, AECHAR *pOneDial)
{
    AECHAR pNumber[MAX_INPUT_NUM+1];
    
    if ( pMe == NULL || pOneDial == NULL) 
    {
        return EFAILED;
    }
    
    FARF(ADDR, ("Contact application Make one dial Call"));
    
    // Get the onedial number
    if(SUCCESS != CContApp_GetOneDial(pMe, pOneDial, pNumber, MAX_INPUT_NUM))
    {
        return EFAILED;
    }
#if 1
    WSTRLCPY(pOneDial,pNumber,MAX_INPUT_NUM);
#else
    // Create the Call instance
    if ( SUCCESS != ISHELL_CreateInstance( (AEE_GetShell()),
                                           AEECLSID_DIALER,
                                           (void **)&pMe->m_pCallApp))
    {
        return EFAILED;
    }
    
    // Make a voice call
    ICallApp_CallNumber(pMe->m_pCallApp, pNumber);
    
    if (pMe->m_pCallApp != NULL) 
    {
        ICallApp_Release(pMe->m_pCallApp);
        pMe->m_pCallApp = NULL;
    }
#endif
    return SUCCESS;
} // CContApp_OneDialCall


/*==============================================================================

函数: CContApp_GetOneDial
       
说明: 
    获取单键拨号的信息
       
参数: 
    pMe       [in]: IContApp接口对象指针。
    pSpeedDial[in]: Speed dial text
    pNumber   [in]: Number
    wNumLen   [in]: 查询到的信息输出buffer长度（in AECHAR）
    
返回值:
    SUCCESS: 如果存在该记录
       
备注:
       
==============================================================================*/
static int CContApp_GetOneDial( CContApp   *pMe,
                                AECHAR     *pSpeedDial,
                                AECHAR     *pNumber,
                                uint16      wNumLen)
{
    uint16        wIndex;
    
    wIndex = (uint16)WSTRTOFLOAT(pSpeedDial);
        
    // No index 0 in one dial
    if(wIndex == 0)
    {
        return EFAILED;
    }
    
    // Get the config data
    if(pMe->m_pFileMgr)
    {
        CContApp_CFGCacheInit(pMe, &pMe->m_sCFGCache);
        // Read the config file
        (void)CContApp_ReadCFG(pMe->m_pFileMgr, &pMe->m_sCFGCache);
        // Get config
        wIndex = wIndex-2;
        (void)CContApp_GetConfig( pMe, 
                                 (ContAppCFG)(CONTCFG_ONEDIAL2 + wIndex),	//CONTCFG_ONEDIAL1
                                 pMe->m_pOneDialBuf,
                                  sizeof(pMe->m_pOneDialBuf));
        
        // No onedial data
        if(pMe->m_pOneDialBuf[0] == ONEDIAL_NULL)
        {
            MSG_FATAL("Failed to get ONEDIAL %d", wIndex,0,0);
            return EFAILED;
        }

        if(pNumber)
        {
            WSTRNCOPYN( pNumber, (int)wNumLen, pMe->m_pOneDialBuf, -1);
        } 
        else
        {
            return EFAILED;
        }
    }
    
    return SUCCESS;
}// CContApp_GetOneDial

/*==============================================================================

函数: CContApp_GetSmartDial
       
说明: 
    获取智能拨号的信息
       
参数: 
    pMe       [in]: IContApp接口对象指针。
    pSpeedDial[in]: Speed dial text
    pNumber   [in]: Number
    wNumLen   [in]: 查询到的信息输出buffer长度（in AECHAR）
    
返回值:
    int : 匹配的记录数
       
备注:
       
==============================================================================*/
static int CContApp_GetSmartDial( CContApp   *pMe,
                                  AECHAR     *pSpeedDial,
                                  AECHAR     *pNumber,
                                  uint16      wNumLen)
{
    IAddrBook  *pAddrPhone;     // IADDRBOOK interface for phone
#ifdef FEATURE_RUIM_PHONEBOOK
    IAddrBook  *pAddrRUIM;      // IADDRBOOK interface for RUIM
    AEEAddrField *pFld;
#endif// FEATURE_RUIM_PHONEBOOK
    IAddrRec     *pAddrRec;
    void         *pCache;
    AECHAR       *pName;
    uint16        wRecID;
    boolean       bSmartDial;
    AECHAR        alpha[MAX_INPUT_NAME_EN +1];
    int           nRet;
    int           nCnt   = 0;
    boolean       bFound = FALSE;
    
    // Get the config data
    if(pMe->m_pFileMgr)
    {
        CContApp_CFGCacheInit(pMe, &pMe->m_sCFGCache);
        // Read the config file
        (void)CContApp_ReadCFG(pMe->m_pFileMgr, &pMe->m_sCFGCache);
    }
    else
    {
        return(0);
    }
    (void)CContApp_GetConfig( pMe, 
                              CONTCFG_SMARTDIAL,
                             &bSmartDial,
                              sizeof(bSmartDial));
    if(FALSE == bSmartDial)
    {
        return(0);
    }
    
#ifdef FEATURE_RUIM_PHONEBOOK
    if( IsRunAsUIMVersion() && pMe->m_bADNRUIMSupport)
    {
        // Create IAddrbokk instance for Ruim
        if ( SUCCESS != ISHELL_CreateInstance( (AEE_GetShell()),
                                               AEECLSID_ADDRBOOK_RUIM,
                                               (void **)&pAddrRUIM))
        {
            return (0);
        }
        
        // 从RUIM 卡Cache载入记录
        nRet = IADDRBOOK_EnumCacheInit( pAddrRUIM,
                                        AEE_ADDR_CAT_NONE,
                                        AEE_ADDRFIELD_NONE,
                                        NULL,
                                        0);
        if(SUCCESS != nRet)
        {
            FARF(ADDR, ("Failed to Enum init %d", nRet));
            (void) IADDRBOOK_Release(pAddrRUIM);
            return(0);
        }
        
        for(;;)
        {
            // Search the cache
            if(SUCCESS != IADDRBOOK_EnumNextCache(pAddrRUIM, &pCache))
            {
                break;
            }
            
            // Find the record
            wRecID = IADDRBOOK_ExtractCache( pAddrRUIM, 
                                             pCache, 
                                            &pName, 
                                             NULL);
            // Convert to alpha
            WStr2Alpha(alpha, MAX_INPUT_NAME_EN, pName);
            
            if(0 != WSTRCMP(alpha, pSpeedDial))
            {
                continue;
            }
            
            // Increase the count
            nCnt++;
            
            // Whether or not have find the record
            if(bFound)
            {
                continue;
            }
            bFound = TRUE;
            
            pAddrRec = IADDRBOOK_GetRecByID(pAddrRUIM, wRecID);
            if(pAddrRec)
            {
                // Get number field
                pFld = IADDRREC_GetField(pAddrRec, IDX_NUM_RUIM_FLD);
                if(pFld == NULL)
                {
                    IADDRREC_Release(pAddrRec);
                    return(0);
                }
                
                // Copy number string
                if(pNumber)
                {
                    WSTRNCOPYN( pNumber,
                               (int)wNumLen,
                               (AECHAR *)pFld->pBuffer,
                                -1);
                }
                
                IADDRREC_Release(pAddrRec);
                pAddrRec = NULL;
            }
            else
            {
                nCnt--;
                bFound = FALSE;
            }
        }
        
        if(pAddrRUIM)
        {
            (void) IADDRBOOK_Release(pAddrRUIM);
        }
    }
#endif// FEATURE_RUIM_PHONEBOOK

    // Create IAddrbokk instance for phone
    if ( SUCCESS != ISHELL_CreateInstance( (AEE_GetShell()),
                                           AEECLSID_ADDRBOOK,
                                           (void **)&pAddrPhone))
    {
        return (0);
    }
    
    // 从电话本Cache载入记录
    nRet = IADDRBOOK_EnumCacheInit( pAddrPhone,
                                    AEE_ADDR_CAT_NONE,
                                    AEE_ADDRFIELD_NONE,
                                    NULL,
                                    0);
    if(SUCCESS != nRet)
    {
        FARF(ADDR, ("Failed to Enum init %d", nRet));
        (void) IADDRBOOK_Release(pAddrPhone);
        return(nCnt);
    }
    
    for(;;)
    {
        // Search the cache
        if(SUCCESS != IADDRBOOK_EnumNextCache(pAddrPhone, &pCache))
        {
            break;
        }
        
        // Find the record
        wRecID = IADDRBOOK_ExtractCache( pAddrPhone, 
                                         pCache, 
                                        &pName, 
                                         NULL);
        // Convert to alpha
        WStr2Alpha(alpha, MAX_INPUT_NAME_EN, pName);
        
        if(0 != WSTRCMP(alpha, pSpeedDial))
        {
            continue;
        }
        
        // Increase the count
        nCnt++;
        
        // Whether or not have find the record
        if(bFound)
        {
            continue;
        }
        bFound = TRUE;
        
        pAddrRec = IADDRBOOK_GetRecByID(pAddrPhone, wRecID);
        if(pAddrRec)
        {
            nRet = CContApp_GetNumber(pAddrRec, pNumber, wNumLen);
            IADDRREC_Release(pAddrRec);
            pAddrRec = NULL;
            if(nRet != SUCCESS)
            {
                nCnt--;
                bFound = FALSE;
            }
        }
        else
        {
            nCnt--;
            bFound = FALSE;
        }
    }
    
    if(pAddrPhone)
    {
        (void) IADDRBOOK_Release(pAddrPhone);
    }

    return nCnt;
}// CContApp_GetSmartDial

/*==============================================================================

函数: CContApp_GetNumber
       
说明: 
    获取记录中的Number字段
       
参数: 
    pAddrRec  : 
    pNumber   [in]: Number
    wNumLen   [in]: 查询到的信息输出buffer长度（in AECHAR）
    
返回值:
    None
       
备注:
       
==============================================================================*/
static int CContApp_GetNumber( IAddrRec   *pAddrRec,
                               AECHAR     *pNumber,
                               uint16      wNumLen)
{
    int           i;
    int           nFldsNum;
    AEEAddrField *pFld;
    
    nFldsNum = IADDRREC_GetFieldCount(pAddrRec);
    for(i=0; i<nFldsNum; i++)
    {
        pFld = IADDRREC_GetField( pAddrRec, i );
        if(NULL == pFld)
        {
            continue;
        }
        
        // Is it number field
        if(IS_NUM_FIELD(pFld->fID))
        {
            // Copy number string
            if(pNumber)
            {
                WSTRNCOPYN( pNumber,
                           (int)wNumLen,
                           (AECHAR *)pFld->pBuffer,
                            -1);
            }
            
            return SUCCESS;
        }
    }
    
    return EFAILED;
}// CContApp_GetNumber


static int IContApp_DeleteAll(IContApp *pi)
{
    CContApp *pMe = (CContApp*)pi;
    
    IADDRBOOK_RemoveAllRecs(pMe->m_pAddrPhone);
/*
#ifdef FEATURE_RUIM_PHONEBOOK
   if(IsRunAsUIMVersion())
   {
       IADDRBOOK_RemoveAllRecs(pMe->m_pAddrRUIM);
   }
#endif

    IVector_RemoveAllElements(pMe->m_pAddList);
*/                 
    return SUCCESS;
}

#ifdef FEATURE_SUPPORT_VCARD
static int  IContApp_ReadVcfFile(IContApp *pi, char* fileName, int * cnt)
{
    CContApp *pMe = (CContApp*)pi;
    
    if(ContApp_ReadVcfFile(pMe, fileName, cnt))
    {
        return SUCCESS;
    }

    return EFAILED;
}

static int  IContApp_CreateVcfFile(IContApp *pi, int contId, char* fileName)
{
    CContApp *pMe = (CContApp*)pi;
    
    if(Vcard_CreateVcfFile(pMe, contId, fileName))
    {
        return SUCCESS;
    }

    return EFAILED;
}


static int  IContApp_CreateVcfFileEx(IContApp *pi, AEEAddrField *pFields, int nFldCnt, char* fileName)
{
    CContApp *pMe = (CContApp*)pi;
    
    if(Vcard_CreateVcfFileEx(pMe, pFields, nFldCnt, fileName))
    {
        return SUCCESS;
    }

    return EFAILED;
}

static int   IContApp_CheckCapacity(IContApp *pi, boolean *isFull)
{
    CContApp *pMe = (CContApp*)pi;

    if(IADDRBOOK_GetNumRecs(pMe->m_pAddrPhone) >= IADDRBOOK_GetCapacity(pMe->m_pAddrPhone))
    {
        *isFull = TRUE;
    }
    else
    {
        *isFull = FALSE;
    }

    return SUCCESS;
    
}

#endif

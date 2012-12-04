/*==============================================================================
//           U S E R   I N T E R F A C E   M A I N  M O D U L E
// 文件:
//        CoreApp.c
//        版权所有(c) 2009 Anylook Tech. CO.,LTD.
//        
// 文件说明:
//        本应用是 UI 的起始点，在开机时由 BREW 自动启动。         
//
==============================================================================*/

/*==============================================================================
修改历史记录:
时       间       修 改 人     修改内容、位置及原因
-----------      ----------    -----------------------------------------------

==============================================================================*/



/*==============================================================================
                             
                             本文件包含的外部文件
                             
==============================================================================*/
#include "OEMFeatures.h"

#include "BREWVersion.h"
#include "AEE.h"
#include "AEEShell.h"
#include "AEEStdlib.h"
#include "AEEModTable.h"
#include "AEEAppGen.h"
#include "AEEAnnunciator.h"

#include "CoreApp_priv.h"        /* CoreApp external data and function prototypes */

#include "AEEClipboard.h"
#include "appscommonimages.brh"

#ifdef FEATURE_LCD_TOUCH_ENABLE
#include "touchpad.h"
#endif

#include "Appscommon.h"//wlh add
#include "ui.h"
#if defined(FEATURE_WMS_APP)
#include "WMSApp.h"
#endif
#include "OEMRTC.h"
#include "AEEDownload.h"
#include "OEMDeviceNotifier.h"
#include "gsdi.h"
#include "Msg.h"

/*==============================================================================
                                 
                                 宏定义和常数
                                 
==============================================================================*/


/*==============================================================================
                                 
                                内部数据类型定义
                                 
==============================================================================*/
#ifdef AEE_STATIC

CCoreApp *g_pCoreApp = NULL; 

#endif


boolean	bIsPPPAuthEnabled = FALSE;	//Add By zzg 2012_03_07
/*==============================================================================

                                 函数声明
                                 
==============================================================================*/

#ifdef FEATURE_SUPPORT_WAP_APP
#if 0
boolean PushMod_StartPush(IShell *ps);
#endif
#endif

static void CoreApp_RunFSM(CCoreApp *pMe);
static boolean CoreApp_HandleNotify(CCoreApp * pMe, AEENotify *pNotify);
static boolean CoreApp_HandleCMNotify(CCoreApp * pMe, AEENotify *pNotify);
static boolean CoreApp_HandleBattNotify(CCoreApp * pMe, AEENotify *pNotify);
static void CoreApp_PoweronStartApps(CCoreApp *pMe);
static int DBToLevel (int nDBVal);
static void CCharger_EnableICONCB(void *pUser);
static void CCharger_BlinkLowBattIcon(void *pUser);
#ifdef FEATRUE_AUTO_POWER
static void CoreApp_Process_AutoPower_Event(void *pUser);
#endif

static void CoreApp_Process_Batty_Msg(CCoreApp   *pMe, uint16  msg_id);
static void CoreApp_Process_BattyLow_Msg(CCoreApp   *pMe, uint16  msg_id);

static void CoreApp_Process_Batty_Msg_CB(void *pp);
#ifdef FEATURE_ICM
static void CoreApp_Process_SS_info(CCoreApp * pMe ,AEECMSSEventData *ss);
#else
static void CoreApp_Process_SS_info(CCoreApp * pMe ,AEETSSEventData *ss);
#endif
static void CoreApp_Process_Charger_Msg(CCoreApp   *pMe);
static uint32 CoreApp_ConvertBattLvToAnnunState(int nBattLevel);
//end added
static void CoreNotifyMP3PlayerAlertEvent(CCoreApp *pMe, boolean toStartAlert);
static void CoreNotifyMP3PlayerAlertEventCB(CCoreApp *pMe);

static void CoreAppReadNVKeyBeepValue(CCoreApp *pMe);
#ifdef CUST_EDITION
 void CoreApp_InitBattStatus(CCoreApp * pMe);
#endif
//static void CoreAppLoadTimepImage(CCoreApp *pMe);   //add by ydc
boolean CoreApp_ProcessOffLine(CCoreApp *pMe);
void SendRTREConfig (CCoreApp *pMe);
static boolean CoreApp_ProcessFTMMode(CCoreApp *pMe);
static void CoreApp_Process_Headset_Msg(CCoreApp *pMe, uint16 msgId);
static boolean CoreApp_GetCardStatus(CCoreApp *pMe,uint8 slot);
#ifdef USES_CONSTEFS
static void CoreApp_RebuildEFS(CCoreApp *pMe);
#endif
static void StereoHeadsetOn(CCoreApp * pMe);
static void HeadsetOff(CCoreApp *pMe);
static void CoreApp_SceneMode(CCoreApp *pMe);
//add by yangdecai  2010-09-10
static void CoreApp_ResetProfileTimer(void *pUser);
static boolean CoreApp_TestCard(CCoreApp *pMe);
static void    CoreApp_ResetRing(CCoreApp *pMe);
//add by yangdecai end
extern int CCoreApp_ChangePIN(CCoreApp *pMe, uint8 byPinID, AECHAR *pOldPIN, AECHAR *pNewPIN);
extern int CCoreApp_UnblockPIN(CCoreApp *pMe, uint8 byPinID, AECHAR *pPUK, AECHAR *pPIN);
extern int CCoreApp_DisplayADN(CCoreApp *pMe, uint16 wRecID);
static boolean Coreapp_CanAlert(CCoreApp *pme);

/*==============================================================================

                                 函数定义
                                 
                                 
==============================================================================*/

#ifdef FEATURE_LCD_TOUCH_ENABLE

static void CoreApp_InitdataTouch(CCoreApp *pMe)
{
	pMe->m_pImageTouchIcon[0] = ISHELL_LoadImage(pMe->a.m_pIShell,IDLE_TOUCH_1);
	pMe->m_pImageTouchIcon[1] = ISHELL_LoadImage(pMe->a.m_pIShell,IDLE_TOUCH_2);
	pMe->m_pImageTouchIcon[2] = ISHELL_LoadImage(pMe->a.m_pIShell,IDLE_TOUCH_3);
	pMe->m_pImageTouchIcon[3] = ISHELL_LoadImage(pMe->a.m_pIShell,IDLE_TOUCH_4);

	pMe->m_pImageTimeIcon[0] = ISHELL_LoadImage(pMe->a.m_pIShell,IDLE_TIME_0);
	pMe->m_pImageTimeIcon[1] = ISHELL_LoadImage(pMe->a.m_pIShell,IDLE_TIME_1);
	pMe->m_pImageTimeIcon[2] = ISHELL_LoadImage(pMe->a.m_pIShell,IDLE_TIME_2);
	pMe->m_pImageTimeIcon[3] = ISHELL_LoadImage(pMe->a.m_pIShell,IDLE_TIME_3);
	pMe->m_pImageTimeIcon[4] = ISHELL_LoadImage(pMe->a.m_pIShell,IDLE_TIME_4);
	pMe->m_pImageTimeIcon[5] = ISHELL_LoadImage(pMe->a.m_pIShell,IDLE_TIME_5);
	pMe->m_pImageTimeIcon[6] = ISHELL_LoadImage(pMe->a.m_pIShell,IDLE_TIME_6);
	pMe->m_pImageTimeIcon[7] = ISHELL_LoadImage(pMe->a.m_pIShell,IDLE_TIME_7);
	pMe->m_pImageTimeIcon[8] = ISHELL_LoadImage(pMe->a.m_pIShell,IDLE_TIME_8);
	pMe->m_pImageTimeIcon[9] = ISHELL_LoadImage(pMe->a.m_pIShell,IDLE_TIME_9);
	pMe->m_pImageTimeIcon[10] = ISHELL_LoadImage(pMe->a.m_pIShell,IDLE_TIME_DAY);
	pMe->m_pImageTimeIcon[11] = ISHELL_LoadImage(pMe->a.m_pIShell,IDLE_TIME_IDLE_TIME_BG);
	pMe->m_pImageTimeIcon[12] = ISHELL_LoadImage(pMe->a.m_pIShell,IDLE_TIME_NIGHT);
}
#endif

#ifdef FEATURE_USES_BLACKBERRY
static void CoreApp_InitdataBlackBerry(CCoreApp *pMe)
{
    MSG_FATAL("CoreApp_InitdataBlackBerry..................",0,0,0);
	pMe->m_pImageIcon[0]    = ISHELL_LoadImage(pMe->a.m_pIShell,IDLE_BLACKBERRY_1);
	pMe->m_pImageSelIcon[0] = ISHELL_LoadImage(pMe->a.m_pIShell,IDLE_BLACKBERRY_SEL_1);
	pMe->m_pImageIcon[1]    = ISHELL_LoadImage(pMe->a.m_pIShell,IDLE_BLACKBERRY_2);
	pMe->m_pImageSelIcon[1] = ISHELL_LoadImage(pMe->a.m_pIShell,IDLE_BLACKBERRY_SEL_2);
	pMe->m_pImageIcon[2]    = ISHELL_LoadImage(pMe->a.m_pIShell,IDLE_BLACKBERRY_3);
	pMe->m_pImageSelIcon[2] = ISHELL_LoadImage(pMe->a.m_pIShell,IDLE_BLACKBERRY_SEL_3);
	pMe->m_pImageIcon[3]    = ISHELL_LoadImage(pMe->a.m_pIShell,IDLE_BLACKBERRY_4);
	pMe->m_pImageSelIcon[3] = ISHELL_LoadImage(pMe->a.m_pIShell,IDLE_BLACKBERRY_SEL_4);
	pMe->m_pImageIcon[4]    = ISHELL_LoadImage(pMe->a.m_pIShell,IDLE_BLACKBERRY_5);
	pMe->m_pImageSelIcon[4] = ISHELL_LoadImage(pMe->a.m_pIShell,IDLE_BLACKBERRY_SEL_5);
	pMe->m_pImageIcon[5]    = ISHELL_LoadImage(pMe->a.m_pIShell,IDLE_BLACKBERRY_6);
	pMe->m_pImageSelIcon[5] = ISHELL_LoadImage(pMe->a.m_pIShell,IDLE_BLACKBERRY_SEL_6);
}
#endif

/*==============================================================================
函数: 
    CoreApp_IsIdle
       
说明: 
    判断是否处于IDLE  状态。
           
参数: 
    void    
    
返回值:
    TRUE: 处于IDLE  状态。
    FALSE:
       
备注:

==============================================================================*/
boolean CoreApp_IsIdle(void)
{
    if (g_pCoreApp)        
    {
    	return (!g_pCoreApp->m_bSuspended && IDD_IDLE == g_pCoreApp->m_wActiveDlgID);
    }

    return FALSE;
}

static AECHAR g_UIMPINSTR1[UIM_MAX_CHV_DIGITS + 1];
static AECHAR g_UIMPINSTR2[UIM_MAX_CHV_DIGITS + 1];

int CoreApp_DisplayADN(IShell *pShell, uint16 wRecID)
{
    if(!pShell || !OEM_IsCDMASVCSupport(UIM_CDMA_SVC_ADN))
    {
        return EBADPARM;
    }
    
    DBGPRINTF("CoreApp_DisplayADN %d",wRecID);
    return ISHELL_PostEventEx(pShell, EVTFLG_ASYNC, AEECLSID_CORE_APP, EVT_DISPLAYADN, wRecID, 0);
}

int CoreApp_ChangePIN(IShell *pShell, uint8 byPinID, AECHAR *pOldPIN, AECHAR *pNewPIN)
{
    if(!pShell || !pOldPIN || !pNewPIN)
    {
        return EBADPARM;
    }
    
    WSTRCPY(g_UIMPINSTR1, pOldPIN);
    WSTRCPY(g_UIMPINSTR2, pNewPIN);
    DBGPRINTF("CoreApp_ChangePIN %S %S",pOldPIN,pNewPIN);
    return ISHELL_PostEventEx(pShell, EVTFLG_ASYNC, AEECLSID_CORE_APP, EVT_CHANGEPIN, byPinID, 0);
}

int CoreApp_UnblockPIN(IShell *pShell, uint8 byPinID, AECHAR *pPUK, AECHAR *pPIN)
{
    if(!pShell || !pPUK || !pPIN)
    {
        return EBADPARM;
    }
    
    WSTRCPY(g_UIMPINSTR1, pPUK);
    WSTRCPY(g_UIMPINSTR2, pPIN);
    DBGPRINTF("CoreApp_UnblockPIN %S %S",pPUK,pPIN);
    return ISHELL_PostEventEx(pShell, EVTFLG_ASYNC, AEECLSID_CORE_APP, EVT_UNBLOCKPIN, byPinID, 0);
}

/*==============================================================================
函数:
    CoreApp_FreeAppData
       
说明: 
    函数用于释放应用的数据成员。
       
参数: 
    po [in]: IApplet 接口指针。
    
返回值:
    none
       
备注:

==============================================================================*/
void CoreApp_FreeAppData(IApplet* po)
{
    CCoreApp *pMe = (CCoreApp*)po;
	int i=0;
	int j = 0;
	int k = 0;
    FREEIF(pMe->m_pADNName);
    FREEIF(pMe->m_pADNNumber);
    if (pMe->m_pIAnn) 
    { 
        IANNUNCIATOR_Release(pMe->m_pIAnn);
        pMe->m_pIAnn= NULL;
    }
	
#ifdef FEATURE_UIALARM    
    // 释放 IAlarm 接口
    if (pMe->m_pAlarm != NULL)
    {
        IAlarm_Release(pMe->m_pAlarm);
        pMe->m_pAlarm = NULL;
    }
#endif 

    if (pMe->m_pAlert != NULL)
    {
        IALERT_Release(pMe->m_pAlert);
        pMe->m_pAlert = NULL;
    }
    // 释放 IBatt 接口
    if (pMe->m_pBatt != NULL)
    {
        IBATTERY_Release(pMe->m_pBatt);
        pMe->m_pBatt = NULL;
    }
    
    // 释放 IRUIM 接口
    if (pMe->m_pIRUIM != NULL)
    {
        IRUIM_Release(pMe->m_pIRUIM);
        pMe->m_pIRUIM = NULL;
    }
    
    /* ICard */
    if (pMe->m_pICard) 
    {
        ICARD_Release(pMe->m_pICard);
        pMe->m_pICard = NULL;
    }
#ifdef FEATURE_ICM
    // 释放 ICM 接口
    if (pMe->m_pCM != NULL)
    {
        (void)ISHELL_RegisterNotify(pMe->a.m_pIShell, 
                                    AEECLSID_CORE_APP,
                                    AEECLSID_CM_NOTIFIER, 
                                    0);
        ICM_Release(pMe->m_pCM);
        pMe->m_pCM = NULL;
    }
#else
    if (pMe->m_pITelephone != NULL)
    {
        (void)ISHELL_RegisterNotify(pMe->a.m_pIShell, 
                                    AEECLSID_CORE_APP,
                                    AEECLSID_PHONENOTIFIER, 
                                    0);
        ITELEPHONE_Release(pMe->m_pITelephone);
        pMe->m_pITelephone = NULL;
    }

	 if (pMe->m_pICallMgr != NULL)
    {
        ICALLMGR_Release(pMe->m_pICallMgr);
        pMe->m_pICallMgr = NULL;
    }
		
	if (pMe->m_pIPhoneCtl != NULL)
    {
        IPHONECTL_Release(pMe->m_pIPhoneCtl);
        pMe->m_pIPhoneCtl = NULL;
    }
#endif
	#if defined(FEATURE_VERSION_W317A)||defined(FEATURE_VERSION_C337)
	if(pMe->m_pSmsTrackTime != NULL)
	   {
		   (void)IMENUCTL_Release(pMe->m_pSmsTrackTime);
		   pMe->m_pSmsTrackTime = NULL;
	   }

    #endif
    if (pMe->m_pConfig) 
    {
        (void)ICONFIG_Release(pMe->m_pConfig);
        pMe->m_pConfig = NULL;
    }

    if(pMe->m_pBacklight)
    {
        IBACKLIGHT_Release(pMe->m_pBacklight);
        pMe->m_pBacklight=NULL;
    }

    if (pMe->m_pDisplay != NULL)
    {
        IDISPLAY_Release(pMe->m_pDisplay);
        pMe->m_pDisplay = NULL;
    }
	
#ifndef WIN32
    OEMRTC_Free_All_Node();
#endif//WIN32

    ISHELL_RegisterNotify(pMe->a.m_pIShell,AEECLSID_CORE_APP,  AEECLSID_ALERT_NOTIFIER,0);

    FREEIF(pMe->m_cdg_msgptr);
    if(pMe->m_pAddrPhone)
    {
        IADDRBOOK_Release(pMe->m_pAddrPhone);
        pMe->m_pAddrPhone = NULL;
    }
#ifdef FEATURE_RUIM_PHONEBOOK
    if(pMe->m_pAddrRUIM)
    {
        IADDRBOOK_Release(pMe->m_pAddrRUIM);
        pMe->m_pAddrRUIM = NULL;
    }
#endif
    g_pCoreApp = NULL;

#ifdef FEATURE_GREYBIT
	{
	    extern void GreyBitBrewFont_Done(void);
	    GreyBitBrewFont_Done();
	}
#endif

#ifdef FEATURE_RANDOM_MENU_REND
	// Destroy Rend
	{
	    extern void DisplayRend_Done(void);
	    DisplayRend_Done();
	}
#endif
#ifdef	FEATURE_USES_BLACKBERRY
	 for(i=0;i<IDLE_BLACKBERRY_ITEMMAX;i++)
        {
            if(pMe->m_pImageIcon[i] != NULL)
            {
                (void)IIMAGE_Release(pMe->m_pImageIcon[i]);
                pMe->m_pImageIcon[i] = NULL;
            }
            if(pMe->m_pImageSelIcon[i] != NULL)
            {
                (void)IIMAGE_Release(pMe->m_pImageSelIcon[i]);
                pMe->m_pImageSelIcon[i] = NULL;
            }
        }
#endif
#ifdef FEATURE_LCD_TOUCH_ENABLE
#ifdef FEATURE_DISP_240X320
	for(j=0;j<IDLE_TOUCH_ITEMMAX;j++)
	{
		if(pMe->m_pImageTouchIcon[j]!=NULL)
		{
			(void)IIMAGE_Release(pMe->m_pImageTouchIcon[j]);
            pMe->m_pImageTouchIcon[j] = NULL;
		}
		
	}
	for(k=0;k<IDLE_TIME_COUNT;k++)
	{
		if(pMe->m_pImageTimeIcon[k]!=NULL)
		{
			(void)IIMAGE_Release(pMe->m_pImageTimeIcon[k]);
            pMe->m_pImageTimeIcon[k] = NULL;
		}
	}
#else
#endif
#endif
#if defined(FEATURE_VERSION_W317A)||defined(FEATURE_VERSION_C337)
#ifdef FEATURE_UIALARM
	IAlarm_CancelAlarm(pMe->m_pIAlarm,
                       		AEECLSID_CORE_APP,
                       		PERMID);
    if (pMe->m_pIAlarm)
    {
        IAlarm_Release(pMe->m_pIAlarm);
    }
#endif
#endif

} /* End CoreApp_FreeAppData */



/*==============================================================================
函数: 
    CoreApp_InitAppData
       
说明: 
    函数用于初始化应用的数据成员。
       
参数: 
    po [in]: IApplet 接口指针。
    
返回值:
    none
       
备注:

==============================================================================*/
boolean CoreApp_InitAppData(IApplet* po)
{
    CCoreApp *pMe = (CCoreApp*)po;
    boolean b_FMBackground = FALSE;
    int i;
    if (NULL == pMe)
    {
        return FALSE;
    }
	
#ifdef FEATURE_GREYBIT
	{
	    extern void GreyBitBrewFont_Init(void);
	    GreyBitBrewFont_Init();
	}
#endif

#ifdef USES_CONSTEFS
    CoreApp_RebuildEFS(pMe);
#endif

#ifdef FEATURE_MMGSDI
    pMe->m_nCardStatus = AEECARD_NOT_READY;
#else
    pMe->m_nCardStatus = AEECARD_NO_CARD;
#endif

    pMe->m_bSuspended = FALSE;
    pMe->m_bChargFull = FALSE;
    pMe->m_bBatteryActive=FALSE;
#if defined(FEATURE_VERSION_C11) || defined(FEATURE_VERSION_SKY)    
    pMe->m_keyinfoheld=FALSE;
#endif    
    if (SUCCESS != ISHELL_CreateInstance(pMe->a.m_pIShell,
                                         AEECLSID_CARD,
                                         (void **) &pMe->m_pICard))
    {
        return FALSE;
    }
    
    
    // 在这里等ICARD的PIN状态初始化完成，以保证剩余初始化流程能够按顺序完成
    while (1)
    {
        AEECardPinStatus sPinStatus;
        
        CoreApp_GetCardStatus(pMe, AEECARD_SLOT1);
        if (ICARD_GetPinStatus(pMe->m_pICard, AEECARD_PIN1, &sPinStatus) != SUCCESS)
        {
            MSLEEP(100);
        }
        else
        {
            if(sPinStatus.lock_state == AEECARD_PIN_NOT_INIT || sPinStatus.lock_state == AEECARD_PIN_UNKNOWN)
            {
                if(AEECARD_NO_CARD != pMe->m_nCardStatus)
                {
                    MSLEEP(100);
                }
                else
                {
                    break;
                }
            }
            else
            {
                break;
            }
        }
    }
    
    if (TRUE != CoreApp_InitExtInterface(pMe))
    {
        return FALSE;
    }

    MediaGallery_StopUDisk(pMe);   // add by pengyuangui 防止U盘模式异常退出 T卡无法再使用
    IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,TRUE);
    CoreTask_SetPwrDnComplete(FALSE);
    
    if( ISHELL_CreateInstance( pMe->a.m_pIShell, AEECLSID_BACKLIGHT, (void **)&pMe->m_pBacklight)!=AEE_SUCCESS)
    {
        return FALSE;
    }

	#if defined(FEATURE_VERSION_W317A)||defined(FEATURE_VERSION_C337)
	  //闹钟开或关LIST 控件
    if(AEE_SUCCESS != ISHELL_CreateInstance(pMe->a.m_pIShell,
                         AEECLSID_LISTCTL,
                         (void **)&pMe->m_pSmsTrackTime))
    {
        CoreApp_FreeAppData((IApplet*)pMe);
        return EFAILED;
    }
	#endif
    pMe->bunlockuim =FALSE;
    pMe->m_nMsgID = 0;
    
    pMe->m_eCurState = COREST_INIT;
    pMe->m_eDlgRet = DLGRET_CREATE;
    
    pMe->m_eUIMErrCode = UIMERR_UNKNOW;
    pMe->m_bAcquiredTime = FALSE;
    pMe->m_bExit = FALSE;
    pMe->m_isShift  = FALSE;   //add by yangdecai  2010-07-27
    pMe->m_battery_msgup = FALSE;//add by yangdecai 2010-07-27
#ifdef FEATURE_QQ_APP    
    pMe->m_qqstate=QQSTATE_QUIT; //add by pengyuangui 2011-10-14
#endif    
#if defined(FEATURE_WMS_APP)
    pMe->m_bsmstipscheck = FALSE;
#endif
#ifdef FEATRUE_AUTO_POWER
    pMe->m_b_autopower_off = FALSE;
    pMe->m_b_needclose_core = FALSE;
#endif
    MEMSET(pMe->m_strPhonePWD, 0, PINCODE_LENGTH + 1);
#if defined(FEATURE_VERSION_W317A)||defined(FEATURE_VERSION_C337)
    MEMSET(pMe->m_strPhoneNUM, 0, PHONENUMBER);
#endif

#ifdef FEATURE_LCD_TOUCH_ENABLE
    MEMSET(pMe->m_strPhonePWDtemp, 0, PINCODE_LENGTH + 1);
#endif
    MEMSET(pMe->m_strLockuimPWD, 0, sizeof(pMe->m_strLockuimPWD));
    MEMSET(pMe->m_strPIN1, 0, PINCODE_LENGTH + 1);
    MEMSET(pMe->m_strPIN2, 0, PINCODE_LENGTH + 1);
    MEMSET(pMe->m_strPUK, 0, PUKCODE_LENGTH + 1);   
    MEMSET(pMe->m_wPIN, 0, sizeof(pMe->m_wPIN));
    MEMSET(pMe->m_wPUK, 0, sizeof(pMe->m_wPUK));
    
    pMe->m_SYS_MODE_NO_SRV = TRUE;
    MEMSET(pMe->svc_p_name, 0, UIM_CDMA_HOME_SERVICE_SIZE + 1); 
#ifndef FEATURE_USES_LOWMEM
    pMe->m_pStartupAniImg = NULL;
#endif
    pMe->m_battery_time = 0;
    pMe->m_battery_state = TRUE;
#ifdef FEATURE_KEYGUARD
    pMe->m_b_set_lock = FALSE ;
    pMe->m_iskeypadtime = FALSE;
#endif

#ifdef FEATURE_VERSION_C337
    pMe->m_bLocked = FALSE;	
#endif
    
    pMe->m_bConfigSent  = FALSE;
    pMe->m_cdg_msgptr = NULL;
    pMe->m_bActive = TRUE;
    pMe->m_bVerifying = FALSE;
    //if phone power down abnormal, we need set CFGI_FM_BACKGROUND  false to avoid show FM in idle
    ICONFIG_SetItem(pMe->m_pConfig, CFGI_FM_BACKGROUND, &b_FMBackground, sizeof(b_FMBackground));
	
#ifdef FEATURE_PLANEMODE
    pMe->bPlaneModeOn = FALSE;
#endif

#ifdef FEATURE_TORCH_SUPPORT
    pMe->TorchOn = FALSE;
#endif
#ifdef FEATURE_USES_BLACKBERRY
	pMe->m_CurMainItems = 0;    //当前主菜单ITEMS
	pMe->m_pImageSelIcon[1] = NULL;
	pMe->m_pImageSelIcon[2] = NULL;
	pMe->m_pImageSelIcon[3] = NULL;
	pMe->m_pImageSelIcon[4] = NULL;
	pMe->m_pImageSelIcon[5] = NULL;
	pMe->m_pImageIcon[1] = NULL;
	pMe->m_pImageIcon[2] = NULL;
	pMe->m_pImageIcon[3] = NULL;
	pMe->m_pImageIcon[4] = NULL;
	pMe->m_pImageIcon[5] = NULL;
#endif
#ifdef FEATURE_LCD_TOUCH_ENABLE
#ifdef FEATURE_DISP_240X320
	pMe->m_pImageTouchIcon[0] = NULL;
	pMe->m_pImageTouchIcon[1] = NULL;
	pMe->m_pImageTouchIcon[2] = NULL;
	pMe->m_pImageTouchIcon[3] = NULL;
	for(i=0;i<IDLE_TIME_COUNT;i++)
	{
		pMe->m_pImageTimeIcon[i] = NULL;
	}

#else
#endif
#endif
	pMe->m_IsSametime = FALSE;
    pMe->m_pADNName = NULL;
    pMe->m_pADNNumber = NULL;
#if defined (FEATURE_VERSION_FLEXI203P)
{
	extern	 void OEM_SetBAM_ADSAccount(void);

	 OEM_SetBAM_ADSAccount();
}
#endif
    OEM_SetBROWSER_ADSAccount();
	(void)ICONFIG_GetItem(pMe->m_pConfig, CFGI_PROFILE_CUR_NUMBER,&pMe->m_CurProfile, sizeof(pMe->m_CurProfile));//CFGI_ALERT_TYPE
	(void)ICONFIG_GetItem(pMe->m_pConfig,CFGI_PROFILE_CUR_NUMBER,&pMe->m_active,sizeof(pMe->m_active));
    CoreAppReadNVKeyBeepValue(pMe);
#ifdef FEATURE_USES_BLACKBERRY
     CoreApp_InitdataBlackBerry(pMe);
#endif
#ifdef FEATURE_LCD_TOUCH_ENABLE
    CoreApp_InitdataTouch(pMe);
#endif

#if defined(FEATURE_VERSION_W317A)||defined(FEATURE_VERSION_C337)
#if defined( FEATURE_UIALARM)
		if (ISHELL_CreateInstance(pMe->a.m_pIShell,
								  AEECLSID_UIALARM,
								  (void**)&pMe->m_pIAlarm))
		{
			MSG_FATAL("ALARM CREAT FAILE",0,0,0);
			return FALSE;
		}
#endif
#endif
    g_pCoreApp = pMe;
    return TRUE;
} /* End CoreApp_InitAppData */



static boolean Coreapp_CanAlert(CCoreApp *pme)
{    
    ICM *pICM = NULL;
    uint16 num = 0;
    if(AEE_SUCCESS != ISHELL_CreateInstance(pme->a.m_pIShell, 
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
    if(num > 0)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

/*==============================================================================
函数:
    CoreApp_HandleEvent

说明:
    本函数是 Core Applet 的事件处理函数。属于本 Applet 的全部事件在本函数处理。任
    何 Applet 都必须提供一个事件处理函数。

参数:
    pi [in]: IApplet 接口指针，这里指向 Core Applet 结构对象的第一个成员的地址。
    eCode [in]: 待处理事件
    wParam [in]: 事件参数。
    dwParam [in]: 事件数据。


返回值:
    TRUE: 事件得到处理
    FALSE:事件没被处理

备注:

==============================================================================*/
static boolean CoreApp_HandleEvent(IApplet * pi,
                                   AEEEvent eCode,
                                   uint16 wParam,
                                   uint32 dwParam)
{
    CCoreApp * pMe = (CCoreApp *)pi;

	MSG_FATAL("***zzg CoreApp_HandleEvent eCode=%x, wParam=%x, dwParam=%x***", eCode, wParam, dwParam);
	
    switch (eCode)
    {
        case EVT_APP_START:
            if(!CoreApp_RegisterNotify(pMe))
            {
                return FALSE;
            }
            else
            {
                AEEAppStart *as;

                as = (AEEAppStart*)dwParam;
                //if (NULL != pMe->m_pDisplay)
                //{
                //    (void) IDISPLAY_Release(pMe->m_pDisplay);
                //    pMe->m_pDisplay = NULL;
                //}
                //pMe->m_pDisplay = as->pDisplay;
                //(void) IDISPLAY_AddRef(pMe->m_pDisplay);
                pMe->m_rc = as->rc;

                // 获取大字体高度
                pMe->m_nLargeFontHeight = IDISPLAY_GetFontMetrics(pMe->m_pDisplay,
                                                                  AEE_FONT_LARGE,
                                                                  NULL,
                                                                  NULL);

                // 获取AEE_FONT_NORMAL字体高度
                pMe->m_nNormalFontHeight = IDISPLAY_GetFontMetrics(pMe->m_pDisplay,
                                                                  AEE_FONT_NORMAL,
                                                                  NULL,
                                                                  NULL);

                pMe->m_bSuspended = FALSE;
                pMe->m_bActive = TRUE;
            }
        
            if(pMe->m_pIAnn != NULL)
            {
                IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,TRUE);
            }

            //Add By zzg 2012_03_07
            {
            	byte bEnabled=OEMNV_PPP_AUTH_DISABLED;
	            ICONFIG_GetItem(pMe->m_pConfig, CFGI_PPP_AUTH, &bEnabled, sizeof(byte));

	            DBGPRINTF("***zzg CoreApp CFGI_PPP_AUTH bEnabled=%x***", bEnabled);
	            
	            if (bEnabled == OEMNV_PPP_AUTH_ENABLED)
	            {
	                bIsPPPAuthEnabled = TRUE;
	            }
	            else
	            {
	                bIsPPPAuthEnabled = FALSE;
	            }
			}
            //Add End
            
            // 开始 Core Applet 状态机, 当前状态已初始为 COREST_INIT
            CoreApp_RunFSM(pMe);

            CoreApp_PoweronStartApps(pMe);
            //CoreApp_InitBattStatus(pMe);

			{
				extern void EnableUIKeys (boolean flag);
            	EnableUIKeys(TRUE);
			}
            (void)ISHELL_SetTimer(pMe->a.m_pIShell, 
                                  RESETPROFILE_TIME,
                                  CoreApp_ResetProfileTimer, 
                                  pMe);
#ifdef FEATURE_WMS_APP			
            MSG_FATAL("EVT_APP_RESUME %d %d %d",gbWmsVMailNtf,gbWmsSMSNtf,gbWmsLastNtfIsSMS);
            if(gbWmsVMailNtf || gbWmsSMSNtf)
            {
                // 通知 CoreApp 需要进行短信提示
                #if defined(FEATURE_VERSION_S1000T)|| defined(FEATURE_VERSION_W515V3)
			    if(ISHELL_ActiveApplet(pMe->a.m_pIShell) == AEECLSID_CORE_APP)
				#endif
				{
	    			(void)ISHELL_PostEvent(pMe->a.m_pIShell,
	                         AEECLSID_CORE_APP, 
	                         EVT_WMS_MSG_RECEIVED_MESSAGE,
	                         0, 
	                         0);
                }
            }
#endif			
            return TRUE;
        case EVT_APP_STOP:
			{
            //if (pMe->m_pDisplay != NULL)
            //{
            //    (void) IDISPLAY_Release(pMe->m_pDisplay);
            //    pMe->m_pDisplay = NULL;
            //}
            JulianType  julian;
        	GetJulianDate(GETTIMESECONDS(), &julian);
            pm_rtc_rw_cmd (PM_RTC_SET_CMD, (pm_rtc_julian_type*)(&julian));
            pMe->m_bSuspended = TRUE;
            pMe->m_bActive = TRUE;

            if (pMe->m_bExit == FALSE)
            {
                // 转为后台模式, 只有在关机时才能退出 Core Applet
                *((boolean *)dwParam) = FALSE;
            }
            return TRUE;
    	  }

        case EVT_APP_SUSPEND:
        {
            AEESuspendInfo * pinfo = (AEESuspendInfo *)dwParam;

            if (NULL != pinfo)
            {
                // 要求关闭对话框, 程序恢复时跑状态机会重新创建对话框的
                pinfo->bCloseDialogs = TRUE;
            }

            pMe->m_bSuspended = TRUE;
            pMe->m_bActive = FALSE;
            pMe->m_pAppStart = FALSE;
            AEE_CancelTimer(CoreApp_Process_Batty_Msg_CB, (void*)pMe);
            return TRUE;
        }

        case EVT_HEADSET:
        {
            if((boolean)wParam)
            {
                CoreApp_Process_Headset_Msg(pMe, IDS_HEADSET_ON);
            }
            else
            {
                CoreApp_Process_Headset_Msg(pMe, IDS_HEADSET_OFF);
            }
            return TRUE;
        }

        case EVT_APP_RESUME:
        {
            AEEAppStart *as;

            as = (AEEAppStart*)dwParam;
            //if (NULL != pMe->m_pDisplay)
            //{
            //    (void) IDISPLAY_Release(pMe->m_pDisplay);
            //    pMe->m_pDisplay = NULL;
            //}
            //pMe->m_pDisplay = as->pDisplay;
            //(void) IDISPLAY_AddRef(pMe->m_pDisplay);
            pMe->m_rc = as->rc;
#ifdef FEATURE_ICM
            pMe->m_bemergencymode = CoreApp_IsEmergencyMode(pMe->m_pCM);
            // 关闭全部通话。以前出现过访问数据业务后电流过大的问题
            ICM_EndAllCalls(pMe->m_pCM);
#else
            pMe->m_bemergencymode = CoreApp_IsEmergencyMode(pMe->m_pITelephone);
            // 关闭全部通话。以前出现过访问数据业务后电流过大的问题
            ICALLMGR_EndAllCalls(pMe->m_pICallMgr);
#endif
            pMe->m_bSuspended = FALSE;
            pMe->m_bActive = TRUE;

            // 跑状态机
            CoreApp_RunFSM(pMe);

#ifdef FEATURE_WMS_APP				
            MSG_FATAL("EVT_APP_RESUME %d %d %d",gbWmsVMailNtf,gbWmsSMSNtf,gbWmsLastNtfIsSMS);
            if(gbWmsVMailNtf || gbWmsSMSNtf)
            {
                // 通知 CoreApp 需要进行短信提示
                #if defined(FEATURE_VERSION_S1000T) || defined(FEATURE_VERSION_W515V3)
			    if(ISHELL_ActiveApplet(pMe->a.m_pIShell) == AEECLSID_CORE_APP)
				#endif
				{
    				(void)ISHELL_PostEvent(pMe->a.m_pIShell,
                         AEECLSID_CORE_APP, 
                         EVT_WMS_MSG_RECEIVED_MESSAGE,
                         0, 
                         0);
                }
            }
#endif			
            return TRUE;
        }

        case EVT_DIALOG_INIT:
            pMe->m_pActiveDlg = (IDialog*)dwParam;
            pMe->m_wActiveDlgID = wParam;
			#ifdef FEATURE_USE_TIME_VU
			/* The PMIC has been initialized, get time-of-day from the PMIC's RTC */
			time_set_from_pmic();
			#endif /* FEATURE_USE_TIME_VU */
            CoreApp_SetDialogHandler(pMe);
            return CoreApp_RouteDialogEvent(pMe, eCode, wParam, dwParam);

        case EVT_DIALOG_START:
            return CoreApp_RouteDialogEvent(pMe,eCode,wParam,dwParam);

        case EVT_USER_REDRAW:
            (void) CoreApp_RouteDialogEvent(pMe,eCode,wParam,dwParam);
            return TRUE;
            
        case EVT_DISPLAYADN:
            (void)CCoreApp_DisplayADN(pMe, wParam);
            return TRUE;
            
        case EVT_CHANGEPIN:
            (void)CCoreApp_ChangePIN(pMe, wParam, g_UIMPINSTR1, g_UIMPINSTR2);
            return TRUE;
            
        case EVT_UNBLOCKPIN:
            (void)CCoreApp_UnblockPIN(pMe, wParam, g_UIMPINSTR1, g_UIMPINSTR2);
            return TRUE;
            
        case EVT_CARD_STATUS:
        case EVT_DISPLAYDIALOGTIMEOUT:
            (void) CoreApp_RouteDialogEvent(pMe,eCode,wParam,dwParam);
            return TRUE;

        case EVT_DIALOG_END:
            if (wParam == 0)
            {
                return TRUE;
            }
            (void) CoreApp_RouteDialogEvent(pMe,eCode,wParam,dwParam);
            pMe->m_pActiveDlg = NULL;
            pMe->m_wActiveDlgID = 0;
            pMe->m_pDialogHandler = NULL;

            // Applet被挂起，不跑状态机，等待Applet返回。注意:EVT_APP_SUSPEND
            // 事件在EVT_DIALOG_END事件前发出。
            if (pMe->m_bSuspended == FALSE)
            {
                // 跑状态机
                CoreApp_RunFSM(pMe);
            }
            return TRUE;

        case EVT_AUTO_POWERDOWN:
            //CORE_ERR("EVT_AUTO_POWERDOWN w= %d dw= %d",wParam,dwParam,0);
#ifdef FEATRUE_AUTO_POWER
            if(wParam == 1)
            {
                ISHELL_SetTimer(pMe->a.m_pIShell, 3000, CoreApp_Process_AutoPower_Event, (void *)pMe);
            }
            else
            {
                CoreApp_Process_AutoPower_Event(pMe);
            }
#endif
            return TRUE; 

        case EVT_NEWMSGIN:
            //CORE_ERR("EVT_NEWMSGIN w= %d dw= %d",wParam,dwParam);
            if(dwParam != 0)
            {
                if(pMe->m_cdg_msgptr == NULL)
                {
#ifdef FEATURE_ICM
                    pMe->m_cdg_msgptr = (AECHAR * )MALLOC(AEECM_MAX_ALPHA_TAG_LENGTH*sizeof(AECHAR));
#else
                    pMe->m_cdg_msgptr = (AECHAR * )MALLOC(AEET_MAX_ALPHA_TAG_LENGTH*sizeof(AECHAR));
#endif
                }
                if(pMe->m_cdg_msgptr != NULL)
                {
                    IDialog  * pDialog = NULL;
                    pDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
                    if(pDialog != NULL)
                    {
#ifdef FEATURE_ICM
                        WSTRLCPY(pMe->m_cdg_msgptr,(AECHAR * )dwParam,AEECM_MAX_ALPHA_TAG_LENGTH);
#else
                        WSTRLCPY(pMe->m_cdg_msgptr,(AECHAR * )dwParam,AEET_MAX_ALPHA_TAG_LENGTH);
#endif
                        pMe->m_nMsgID = 0xFFFF;
                        if(IDIALOG_GetID(pDialog) == IDD_IDLE)
                        {
                            CLOSE_DIALOG(DLGRET_MSG);
                        }
                        else if(IDIALOG_GetID(pDialog) == IDD_MSGBOX)
                        {
                            ISHELL_PostEvent(pMe->a.m_pIShell, AEECLSID_CORE_APP,EVT_USER_REDRAW,  0, 0);
                        }
                        else
                        {
                            FREEIF(pMe->m_cdg_msgptr);
                        }
                    }
                    else
                    {
                        FREEIF(pMe->m_cdg_msgptr);
                    }
                }
                return TRUE;
            }
            return FALSE;
        case EVT_KEY_HELD:
			
            switch (wParam)
            {
                case AVK_POWER:
                case AVK_END:
                
#ifdef FEATURE_KEYGUARD
                    if(OEMKeyguard_IsEnabled())
                    {
                        OEMKeyguard_SetState(TRUE);  //(FALSE);                      
                        return TRUE;
                    }
#endif
                    if ((pMe->m_bSuspended) || (NULL == pMe->m_pActiveDlg))
                    {
                        return FALSE;
                    }
                    
                    if(pMe->m_eCurState == COREST_LPM)
                    {
                        return CoreApp_RouteDialogEvent(pMe,eCode,wParam,dwParam);
                    }
                    
                    pMe->m_ePowerDownType = POWERDOWN_NORMAL;
                    if (pMe->m_eCurState != COREST_POWEROFF
                      &&pMe->m_eCurState != COREST_STARTUPANI
                      &&pMe->m_eCurState != COREST_INIT
                      &&pMe->m_eCurState != COREST_POWERONSYSINIT
                      &&pMe->m_eCurState != COREST_POWERONAPPSDATAINIT)
                    {
                        pMe->m_wStartupAniTime = 0;
                        #ifndef FEATURE_OEMOMH
                        #ifdef FEATURE_ICM
                		ICM_SetOperatingMode(pMe->m_pCM, AEECM_OPRT_MODE_OFFLINE);
                		#endif
                        #endif
                        DBGPRINTF("AVK_END to COREST_POWEROFF");
                        MOVE_TO_STATE(COREST_POWEROFF)
                        CLOSE_DIALOG(DLGRET_CREATE)
                    }

                    return TRUE;
#if defined(FEATURE_VERSION_HITZ181)||defined(FEATURE_VERSION_MTM)||defined(FEATURE_VERSION_S1000T)|| defined(FEATURE_VERSION_W515V3)
#else
 
                case AVK_RWD:
                {
					MSG_FATAL("***zzg CoreApp_Handle EVT_HELD AVK_RWD***", 0, 0, 0);
					
                    // 长按切换情景模式
                    ICONFIG_GetItem(pMe->m_pConfig, CFGI_PROFILE_CUR_NUMBER,&pMe->m_CurProfile, sizeof(pMe->m_CurProfile));//CFGI_ALERT_TYPE
                    
                    if(pMe->m_CurProfile != OEMNV_PROFILE_QUIETMODE)
                    {
                        pMe->m_CurProfile = OEMNV_PROFILE_QUIETMODE;
                    }
                    else
                    {
                        pMe->m_CurProfile = OEMNV_PROFILE_NORMALMODE;
                    }

                    CoreApp_SceneMode(pMe);
                    //ICONFIG_SetItem(pMe->m_pConfig, CFGI_PROFILE_CUR_NUMBER,&alertType, sizeof(alertType));
                    CoreApp_UpdateAnnunciator(pMe);
                    return TRUE;
                }
 #endif
                    
       #ifdef FEATURE_TORCH_SUPPORT

            #if defined(FEATURE_TORCH_KEY_SPACE)	
			
                case AVK_SPACE:
                {
                    if ( pMe->TorchOn == FALSE )
                    {
                        pMe->TorchOn = TRUE;
                        if (pMe->m_pBacklight)
                        {
                            IBACKLIGHT_TurnOnTorch(pMe->m_pBacklight);
                        }
                    }
                    else
                    {
                        pMe->TorchOn = FALSE;
                        if (pMe->m_pBacklight)
                        {                        	
                            IBACKLIGHT_TurnOffTorch(pMe->m_pBacklight);
                        }
                    }                                                   
                    }             

                return TRUE;                
            #endif

			#if defined(FEATURE_TORCH_KEY_LCTRL)	
			
                case AVK_LCTRL:
                {
                    if ( pMe->TorchOn == FALSE )
                    {
                        pMe->TorchOn = TRUE;
                        if (pMe->m_pBacklight)
                        {
                            IBACKLIGHT_TurnOnTorch(pMe->m_pBacklight);
                        }
                    }
                    else
                    {
                        pMe->TorchOn = FALSE;
                        if (pMe->m_pBacklight)
                        {                        	
                            IBACKLIGHT_TurnOffTorch(pMe->m_pBacklight);
                        }
                    }                                                   
                }             

                return TRUE;                
            #endif
            
            #if defined(FEATURE_TORCH_KEY_SYMBOL)	
			
                case AVK_SYMBOL:
                {
                    if ( pMe->TorchOn == FALSE )
                    {
                        pMe->TorchOn = TRUE;
                        if (pMe->m_pBacklight)
                        {
                            IBACKLIGHT_TurnOnTorch(pMe->m_pBacklight);
                        }
                    }
                    else
                    {
                        pMe->TorchOn = FALSE;
                        if (pMe->m_pBacklight)
                        {                        	
                            IBACKLIGHT_TurnOffTorch(pMe->m_pBacklight);
                        }
                    }                                                   
                }             

                return TRUE;                
            #endif
            
            #if defined(FEATURE_TORCH_KEY_CAMERA)	
            #ifndef FEATURE_VERSION_C180
				case AVK_INFO:
            #else
                case AVK_CAMERA:
            #endif
                {
                    if ( pMe->TorchOn == FALSE )
                    {
                        pMe->TorchOn = TRUE;
                        if (pMe->m_pBacklight)
                        {
                            IBACKLIGHT_TurnOnTorch(pMe->m_pBacklight);
                        }
                    }
                    else
                    {
                        pMe->TorchOn = FALSE;
                        if (pMe->m_pBacklight)
                        {                        	
                            IBACKLIGHT_TurnOffTorch(pMe->m_pBacklight);
                        }
                    }
                }

                return TRUE;
            #endif

            
                break;
        #endif
            default:
                break;
            }
            #if defined(FEATURE_VERSION_C11)|| defined(FEATURE_VERSION_SKY)
             if(wParam == AVK_INFO)
             {
               return CoreApp_RouteDialogEvent(pMe,eCode,wParam,dwParam);
             }
            #endif
            #if defined(FEATURE_VERSION_HITZ181)||defined(FEATURE_VERSION_MTM)||defined(FEATURE_VERSION_S1000T)|| defined(FEATURE_VERSION_W515V3)||defined(FEATURE_LCD_TOUCH_ENABLE)
			
			return CoreApp_RouteDialogEvent(pMe,eCode,wParam,dwParam);
            #endif
            break;
            
        case EVT_GSENSOR_SHAKE:
        case EVT_KEY:
#if MIN_BREW_VERSION(3,0)
            // do not want to handle au
            if ((dwParam & KB_AUTOREPEAT) != 0)
            {
                if ((wParam & AVK_CLR)!=0)
                {
                    return TRUE;
                }
                else
                {
                    return FALSE;
                }
            }
#endif
			
            return CoreApp_RouteDialogEvent(pMe,eCode,wParam,dwParam);

#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
		case EVT_PEN_UP:
		case EVT_PEN_MOVE:
		case EVT_PEN_DOWN:
#endif
        case EVT_KEY_PRESS:
        case EVT_KEY_RELEASE:
        case EVT_COMMAND:
        {
				
            return CoreApp_RouteDialogEvent(pMe,eCode,wParam,dwParam);
        }
            
        case EVT_HEADSET_CONNECT:
            StereoHeadsetOn(pMe);
            return TRUE;
            
        case EVT_HEADSET_DISCONNECT:
            HeadsetOff(pMe);
            return TRUE;
            
        case EVT_NOTIFY:
            if(((AEENotify *)dwParam)->cls == AEECLSID_ALERT_NOTIFIER)
            {
                //CORE_ERR("AEECLSID_ALERT_NOTIFIER %x",wParam,0,0);
                switch(wParam)
                {
                    case NMASK_ALERT_ONOFF:
                        return TRUE;

                    case NMASK_ALERT_MUTED:
                        return TRUE;

                    default:
                        return FALSE;
                }
            }
            return CoreApp_HandleNotify(pMe, (AEENotify *)dwParam);
            
        case EVT_UPDATEIDLE:
           
            if ((ISHELL_ActiveApplet(pMe->a.m_pIShell) != AEECLSID_CORE_APP) ||
                (pMe->m_wActiveDlgID != IDD_IDLE))
            {
                return TRUE; 
            }
            
            {   // 在此完成闹钟的开机初始化。初始化过程只需执行一次
                static boolean bNotInitedAlarm = TRUE;
                
                if (bNotInitedAlarm)
                {
#ifdef FEATURE_UIALARM
                    // 直道取得有效时间才开始启动警报器
                    CoreApp_EnableShellAlarms(pMe, TRUE);
#endif
                    bNotInitedAlarm = FALSE;

                }
            }      
            return CoreApp_RouteDialogEvent(pMe,eCode,wParam,dwParam);
            
#if defined(FEATURE_WMS_APP)
        case EVT_WMS_MSG_RECEIVED_MESSAGE:
            pMe->m_bsmstipscheck = TRUE;
            
            if (ISHELL_ActiveApplet(pMe->a.m_pIShell) == AEECLSID_CORE_APP)
            {
                uint16  nNewsVmail=0, nNewsSMS=0;
                
                // 获取消息数
                wms_cacheinfolist_getcounts(WMS_MB_VOICEMAIL, &nNewsVmail, NULL, NULL);
                wms_cacheinfolist_getcounts(WMS_MB_INBOX, &nNewsSMS, NULL, NULL);
                
                if (pMe->m_wActiveDlgID == IDD_IDLE)
                {
                    if (nNewsVmail>0 || nNewsSMS>0 || gbWmsVMailNtf)
                    {
                        CLOSE_DIALOG(DLGRET_SMSTIPS)
                    }
                }
                else if (pMe->m_wActiveDlgID == IDD_WMSTIPS)
                {// 重新创建对话框,更新显示
                    if (nNewsVmail>0 || nNewsSMS>0 || gbWmsVMailNtf)
                    {
                        CLOSE_DIALOG(DLGRET_CREATE)
                    }
                    else
                    {// 语音通知短信为0应清除提示
                        pMe->m_bsmstipscheck = FALSE;
                        CLOSE_DIALOG(DLGRET_SMSTIPS_OK)
                    }
                }
            }
            
            return TRUE;
#endif            
#ifdef FEATURE_POWERUP_REGISTER_CHINAUNICOM
        case EVT_SENDREGINFOR:
            // 发送注册短信
            if (CoreApp_SendReginfo(pMe) != SUCCESS)
            {
                // 设置发送注册短信的定时器函数
                (void)ISHELL_SetTimer(pMe->a.m_pIShell, 
                                      SENDREGINFOR_TIME,
                                      CoreApp_SendReginfoTimer, 
                                      pMe);
            }
            
            //CORE_ERR("EVT_SENDREGINFOR",0,0,0);
            return TRUE;
            
        // 发送注册短信状态报告事件
        case EVT_SENDSTATUS:
            //CORE_ERR("EVT_SENDSTATUS",0,0,0);
            if (dwParam != SUCCESS)
            {// 发送注册短信失败
                // 设置发送注册短信的定时器函数
                (void)ISHELL_SetTimer(pMe->a.m_pIShell, 
                                      SENDREGINFOR_TIME,
                                      CoreApp_SendReginfoTimer, 
                                      pMe);
            }
            return TRUE;
            
        //收到注册确认短信
        case EVT_UPDATEREGSTATUS:
            //CORE_ERR("----Reg--%d-",wParam,0,0);
            if (wParam != 0)
            {
                IdleAPP_Config_Type cfg;
                
                (void)ISHELL_GetPrefs(pMe->a.m_pIShell, 
                            AEECLSID_CORE_APP, 
                            IDLE_CFG_VERSION,
                            &cfg, 
                            sizeof(cfg));
                            
                cfg.m_RegItem.m_bRegistered = TRUE;
                (void)ISHELL_SetPrefs(pMe->a.m_pIShell, 
                        AEECLSID_CORE_APP, 
                        IDLE_CFG_VERSION,
                        &cfg, 
                        sizeof(cfg));
            }
            return TRUE;
#endif

#if defined(FEATURE_VERSION_W317A)||defined(FEATURE_VERSION_C337)
		case EVT_MOBILE_TRACKER:
			{
				if(CoreApp_MobileTracker(pMe) != SUCCESS)
				{
					(void)ISHELL_SetTimer(pMe->a.m_pIShell, 
                                  MOBILETRACKERREGINFOR_TIME,
                                  CoreApp_MobileTrackerTimer, 
                                  pMe);
				}
			}
			return TRUE;
		case EVT_SALES_TRACKER:
			{
				if (IRUIM_IsCardConnected(pMe->m_pIRUIM)) 
				{
					MSG_FATAL("EVT_SALES_TRACKER......................",0,0,0);
					if(CoreApp_SMSTracker(pMe) != SUCCESS)
					{
						(void)ISHELL_SetTimer(pMe->a.m_pIShell, 
                                  SMS_TRACKER_SMSTIME,
                                  CoreApp_SmsTrackerTimer, 
                                  pMe);
					}
					else
					{
						boolean m_bsendsalessms = TRUE;
						(void) ICONFIG_SetItem(pMe->m_pConfig,	
									   CFGI_SMS_TRACKER_SEND_B,
									   &m_bsendsalessms, 
									   sizeof(m_bsendsalessms));
						#if defined(FEATURE_VERSION_W317A)
						CLOSE_DIALOG(DLGRET_SALES_SUCESS)
						#endif
					}
				}
				else
				{
					MSG_FATAL("NO UIM CARD !!!!!!!!!!!!1",0,0,0);
				}
					
			}
			return TRUE;
			
		//Add By zzg 2012_11_16	
		case EVT_WMS_DRAFT_SAVED:
		{
			MSG_FATAL("EVT_WMS_DRAFT_SAVED",0,0,0);
			CLOSE_DIALOG(DLGRET_WMS_DRAFT_SAVED)
			return TRUE;
		}
		//Add End
		
		case EVT_ALARM:
			{
				if(Coreapp_CanAlert(pMe))
				{
					CoreApp_HandleAlarm(pMe, wParam);
				}
				MSG_FATAL("EVT_ALARM...................................",0,0,0);
			}
		return TRUE;
#endif

#ifdef FEATURE_SEAMLESS_SMS
        case EVT_SEND_SEAMLESSSMS:
            // 发送注册短信
            if (CoreApp_SendSeamlessSMS(pMe) != SUCCESS)
            {
                // 设置发送注册短信的定时器函数
                (void)ISHELL_SetTimer(pMe->a.m_pIShell, 
                                      SENDSEAMLESSSMS_TIME,
                                      CoreApp_SendSeamlessSMSTimer, 
                                      pMe);
            }
            
            //CORE_ERR("EVT_SEND_SEAMLESSSMS",0,0,0);
            return TRUE;
#endif

#ifdef FEATURE_UTK2
        case EVT_UTKREFRESH:           
            pMe->wRefreshMask = wParam;
            // 先改变当前状态
            MOVE_TO_STATE(COREST_UTKREFRESH)
            
            // 再以 DLGRET_CREATE 关闭当前对话框
            CLOSE_DIALOG(DLGRET_CREATE)         
            return TRUE;
#endif

#if defined(FEATURE_VERSION_W317A)||defined(FEATURE_VERSION_C337)
		case EVT_SMS_TRACKER:
			// 先改变当前状态
            MOVE_TO_STATE(COREST_SALES_EDIT)
            
            // 再以 DLGRET_CREATE 关闭当前对话框
            CLOSE_DIALOG(DLGRET_CREATE)  
			return TRUE;
#endif
        case EVT_RTC:
        {
            if( pMe->m_eCurState == COREST_LPM)
            {

#if defined( FEATURE_POWERDOWN_ALARM)
                ICONFIG_GetItem(pMe->m_pConfig, CFGI_ALARM_FLAG,(void*)&pMe->powerupByAlarm, sizeof(boolean));
#endif              
                CLOSE_DIALOG( DLGRET_RTC)
            }
        }
        return TRUE;
        case EVT_BUSY:
            if(pMe->m_wActiveDlgID == IDD_STARTUPANI)
            {
                return TRUE;//CoreApp_RouteDialogEvent(pMe,eCode,wParam,dwParam);
            }
            return FALSE;

        case EVT_DRAWMUSICNAME:
        {
#ifndef WIN32//wlh 暂时屏蔽
#ifdef FEATURE_APP_MUSICPLAYER
            pMe->m_pMusicName = (AECHAR *)dwParam;  // yeuzhen.li add @090118
#endif            
#endif//WIN32
            if ((ISHELL_ActiveApplet(pMe->a.m_pIShell) != AEECLSID_CORE_APP) ||
                (pMe->m_wActiveDlgID != IDD_IDLE))
            {
                return TRUE; 
            }

            //change EVT_DRAWMUSICNAME to EVT_UPDATEIDLE
            CoreApp_RouteDialogEvent(pMe,(AEEEvent)EVT_USER_REDRAW,wParam,dwParam);
            return TRUE;
        }

#ifdef FEATURE_PLANEMODE
        case EVT_PLANEMODEON: /* settings 那边的设置改变后，让idle界面立马显示，欺骗用户 */
            pMe->m_SYS_MODE_NO_SRV = TRUE;
            break;
#endif
        case EVT_USER:
			MSG_FATAL("***zzg CoreApp EVT_USER wParam=%x, dwParam=%x***",wParam,dwParam,0);
#ifdef FEATURE_LCD_TOUCH_ENABLE
			{				
				if(wParam == AVK_SELECT)
				{
					if(dwParam!=0)
					{
						eCode = EVT_COMMAND;
					}
					else
					{
						eCode = EVT_KEY;	
					}
					return CoreApp_RouteDialogEvent(pMe,eCode,wParam,dwParam);
				}
				else if(wParam == AVK_CLR)
				{
					eCode = EVT_KEY;
					return CoreApp_RouteDialogEvent(pMe,eCode,wParam,dwParam);
				}	
                else if (wParam==100)
                {     
                      char * password=(char *)dwParam;
                      (void) STRCPY( pMe->m_strPhonePWDtemp,password);
                      MSG_FATAL("pMe->m_strPhonePWD--=%x---=%x----=%x",pMe->m_strPhonePWD[0],pMe->m_strPhonePWD[1],STRLEN(pMe->m_strPhonePWD));
                      (void) ISHELL_PostEvent(pMe->a.m_pIShell,
                                            AEECLSID_CORE_APP,
                                            EVT_USER_REDRAW,
                                            0,
                                            0);                     
                }
			}
#endif
            
            switch(wParam) 
			{

			//Add By zzg 2011_12_12
#ifdef FEATURE_VERSION_W208S
			case 1:
			{
				MSG_FATAL("***zzg CoreApp EVT_USER FEATURE_VERSION_W208S***",0,0,0);
				return CoreApp_RouteDialogEvent(pMe,eCode,wParam,dwParam);				
			}
#endif
			//Add End
            case EVT_UI_EXIT:
                /* AEE is about to exit, set the exit flag to TRUE */
                pMe->m_bExit = TRUE;
                return TRUE;
                
            case EVT_MC_PWR_DOWN:
                DBGPRINTF("EVT_MC_PWR_DOWN");
                CoreApp_Poweroff_Phone(pMe);
                break;
            case EVT_MODE_CHANGE:
            	{
            		MSG_FATAL("EVT_MODE_CHANGE........................",0,0,0);
            		pMe->m_nMsgID = IDS_USERPROFILE_MODE;
          			CLOSE_DIALOG(DLGRET_BATT_INFO)
            	}
            	break;
#ifdef FEATURE_QQ_APP                 
            case EVT_QQ_ANNU_UPDATE:
                 IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_FMRADIO/*ANNUN_FIELD_QQ*/, ANNUN_STATE_QQ_OFFLINE);
                 IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_FMRADIO/*ANNUN_FIELD_QQ*/, ANNUN_STATE_QQ_MSG_OFF);
                 IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_FMRADIO/*ANNUN_FIELD_QQ*/, ANNUN_STATE_QQ_LEAVE_OFF); 
                 IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_FMRADIO/*ANNUN_FIELD_QQ*/, ANNUN_STATE_QQ_HIDING_OFF);

                 switch(dwParam)
                        {
                        case 0:
                             pMe->m_qqstate=QQSTATE_ONLINE;
                             IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_FMRADIO/*ANNUN_FIELD_QQ*/, ANNUN_STATE_QQ_ONLINE);
                             break;
                         case 1:
                             pMe->m_qqstate=QQSTATE_AWAYLINE;
                             IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_FMRADIO/*ANNUN_FIELD_QQ*/, ANNUN_STATE_QQ_LEAVE_ON); 
                             break;
                         case 2:
                             pMe->m_qqstate=QQSTATE_OFFLINE;
                             break;
                         case 3:
                             pMe->m_qqstate=QQSTATE_INVISIBLE;
                             IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_FMRADIO/*ANNUN_FIELD_QQ*/, ANNUN_STATE_QQ_HIDING_ON); 
                             break;
                         case 4:
                              pMe->m_qqstate=QQSTATE_HAVEMSG;
                              IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_FMRADIO/*ANNUN_FIELD_QQ*/, ANNUN_STATE_QQ_MSG_ON); 
                             break;
                         case 5:
                             pMe->m_qqstate=QQSTATE_QUIT;
                             break;
                        }
                break;
#endif                
            default:
            #if defined (FEATURE_VERSION_N021) || defined(FEATURE_VERSION_W0216A)
            	if(wParam==1)
            	{
            		OEM_GetConfig(CFGI_FLSHLITHG_STATUS,&pMe->TorchOn, sizeof(pMe->TorchOn));
					if ( pMe->TorchOn == FALSE )
					{
						pMe->TorchOn = TRUE;
						if (pMe->m_pBacklight)
						{
							IBACKLIGHT_TurnOnTorch(pMe->m_pBacklight);
						}
					}
					else
					{
						pMe->TorchOn = FALSE;
						if (pMe->m_pBacklight)
						{							
							IBACKLIGHT_TurnOffTorch(pMe->m_pBacklight);
						}
					}
					OEM_SetConfig(CFGI_FLSHLITHG_STATUS,&pMe->TorchOn, sizeof(pMe->TorchOn));
            	}
            	#endif
                break;
            }
            break;
        case EVT_SET_OPERATING_MODE:
#ifdef FEATURE_ICM
            ICM_SetOperatingMode(pMe->m_pCM, (AEECMOprtMode)wParam);
#else
            IPHONECTL_SetOperatingMode(pMe->m_pIPhoneCtl, (AEETOprtMode)wParam);
#endif
            break;
            
        default:
            break;
    }
    
    return(FALSE);
}  // End CoreApp_HandleEvent

/*==============================================================================
函数: 
    CoreApp_HandleNotify
       
说明: 
    函数用于将 Core Apple 收到的通知事件根据其来源分发给相应函数处理。
       
参数: 
    pMe [In]: 指向 Core Applet对象结构的指针,该结构包含小程序的特定信息。
    pNotify [in]: 指向 AEENotify 结构体变量的指针。
    
    
返回值:
    TRUE: 事件得到处理
    FALSE:事件没被处理
       
备注:

==============================================================================*/
static boolean CoreApp_HandleNotify(CCoreApp * pMe, AEENotify *pNotify)
{
    if (NULL == pMe || NULL == pNotify)
    {
        return FALSE;
    }
    
    switch (pNotify->cls)
    {
#ifdef FEATURE_ICM
        case AEECLSID_CM_NOTIFIER:
#else
        case AEECLSID_PHONENOTIFIER:
#endif
            return CoreApp_HandleCMNotify(pMe, pNotify);

        case AEECLSID_BATTERYNOTIFIER:
            return CoreApp_HandleBattNotify(pMe, pNotify);          
     
        default:
            break;
    }
    
    return FALSE;
}
#ifdef FEATURE_ICM
static void CoreApp_Process_SS_info(CCoreApp * pMe ,AEECMSSEventData *ss)
#else
static void CoreApp_Process_SS_info(CCoreApp * pMe ,AEETSSEventData *ss)
#endif
{
    switch (ss->ss_info.srv_status)
    {
#ifdef FEATURE_ICM
        case AEECM_SRV_STATUS_SRV:
        case AEECM_SRV_STATUS_LIMITED:
#else
        case AEET_SRV_STATUS_SRV:
        case AEET_SRV_STATUS_LIMITED:
#endif
        {
            if(pMe->m_pIAnn == NULL)
            {
                return ;
            }

            switch (ss->ss_info.roam_status)
            {
#ifdef FEATURE_ICM
                case AEECM_ROAM_STATUS_OFF:
#else
                case AEET_ROAM_STATUS_OFF:
#endif
                    IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_WAP/*ANNUN_FIELD_ROAM*/, ANNUN_STATE_ROAM_OFF/*ANNUN_STATE_OFF*/);
                    break;
#ifdef FEATURE_ICM
                case AEECM_ROAM_STATUS_ON:
#else
                case AEET_ROAM_STATUS_ON:
#endif
                    IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_WAP/*ANNUN_FIELD_ROAM*/, ANNUN_STATE_ROAM_ON/*ANNUN_STATE_ON*/);
                    break;
#ifdef FEATURE_ICM
                case AEECM_ROAM_STATUS_BLINK:
#else
                case AEET_ROAM_STATUS_BLINK:
#endif
                    //IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_ROAM, ANNUN_STATE_BLINK);
                    break;
                    
                default:
                    IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_WAP/*ANNUN_FIELD_ROAM*/, ANNUN_STATE_ROAM_OFF/*ANNUN_STATE_OFF*/);
                    break;
            }
            break;
        }
#ifdef FEATURE_ICM
        case AEECM_SRV_STATUS_NO_SRV:
        case AEECM_SRV_STATUS_PWR_SAVE:
#else
        case AEET_SRV_STATUS_NO_SRV:
        case AEET_SRV_STATUS_PWR_SAVE:
#endif
            if(pMe->m_pIAnn == NULL)
            {
                return ;
            }
            IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_RSSI,ANNUN_STATE_RSSI_NO_SERV/*ANNUN_STATE_RSSI_0*/);
            IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_WAP/*ANNUN_FIELD_ROAM*/, ANNUN_STATE_ROAM_OFF/*ANNUN_STATE_OFF*/);
            break;

        default:
            break;
    }
}
/*==============================================================================
函数: 
    CoreApp_HandleCMNotify
       
说明: 
    函数处理来自 AEECLSID_CM_NOTIFIER 的通知事件。
       
参数: 
    pMe [In]: 指向 Core Applet对象结构的指针,该结构包含小程序的特定信息。
    pNotify [in]: 指向 AEENotify 结构体变量的指针。
    
    
返回值:
    TRUE: 事件得到处理
    FALSE:事件没被处理
       
备注:

==============================================================================*/
static boolean CoreApp_HandleCMNotify(CCoreApp * pMe, AEENotify *pNotify)
{
#ifdef FEATURE_ICM
    AEECMSysMode SysMode;
    boolean  bUpdate = FALSE;
    AEECMNotifyInfo *pEvtInfo;
#else
    AEETSysMode SysMode;
    boolean  bUpdate = FALSE;
    AEETNotifyInfo *pEvtInfo;
#endif

    if (NULL == pNotify)
    {
        return FALSE;
    }
    
    pEvtInfo = pNotify->pData;
    
    SysMode = pEvtInfo->event_data.ss.ss_info.sys_mode;
    
    switch (pNotify->dwMask)
    {
#ifdef FEATURE_ICM
    case NMASK_CM_DATA_CALL:
#else
    case AEET_NMASK_DATA_CALL:
#endif
            switch (pEvtInfo->event)
            {
#ifdef FEATURE_ICM
                case AEECM_EVENT_CALL_CONNECT:
#else
                case AEET_EVENT_CALL_CONNECT:
#endif
                    if(pMe->m_pIAnn != NULL)
                    {
                        IANNUNCIATOR_SetField(pMe->m_pIAnn, ANNUN_FIELD_WAP, ANNUN_STATE_1X_ON);
                    }
                    break;
#if 0
                case AEECM_EVENT_CALL_ENTER_DORMANT:
                    IANNUNCIATOR_SetField(pMe->m_pIAnn, ANNUN_FIELD_WAP, ANNUN_STATE_1X_OFF);                   
                    break;
                case AEECM_EVENT_CALL_EXIT_DORMANT:
                    IANNUNCIATOR_SetField(pMe->m_pIAnn, ANNUN_FIELD_WAP, ANNUN_STATE_1X_ON);                   
                    break;
#endif
#ifdef FEATURE_ICM
                case AEECM_EVENT_CALL_END: 
#else
                case AEET_EVENT_CALL_END: 
#endif
                    if(pMe->m_pIAnn != NULL)
                    {
                        IANNUNCIATOR_SetField(pMe->m_pIAnn, ANNUN_FIELD_WAP, ANNUN_STATE_1X_OFF);   
                    }
                    break;
            }
            break;
#ifdef FEATURE_ICM
       case NMASK_CM_SS:
#else
       case AEET_NMASK_SS:
#endif
            switch (pEvtInfo->event)
            {
#ifdef FEATURE_ICM
                case AEECM_EVENT_SS_SRV_CHANGED:
                case AEECM_EVENT_SS_RSSI:
#else
                case AEET_EVENT_SS_SRV_CHANGED:
                case AEET_EVENT_SS_RSSI:
#endif
#ifdef FEATURE_ICM
                    if (SysMode == AEECM_SYS_MODE_NO_SRV)
#else
                    if (SysMode == AEET_SYS_MODE_NO_SRV)
#endif
                    {
                        pMe->m_SYS_MODE_NO_SRV = TRUE;
                        if(pMe->m_pIAnn != NULL)
                        {
                            IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_RSSI, ANNUN_STATE_RSSI_NO_SERV);
                        }
#ifdef FEATURE_LED_CONTROL
                        IBACKLIGHT_SigLedDisable(pMe->m_pBacklight);
#endif
                        bUpdate = TRUE;
                    }
#ifdef FEATURE_ICM
                    else if (AEECM_SYS_MODE_NONE != SysMode)
#else
                    else if (AEET_SYS_MODE_NONE != SysMode)
#endif
                    {
                        if (pMe->m_bAcquiredTime == FALSE)
                        {
                            pMe->m_bAcquiredTime = TRUE;
                        }
                        if(pMe->m_SYS_MODE_NO_SRV == TRUE)
                        {
                            bUpdate = TRUE;                        
                            pMe->m_SYS_MODE_NO_SRV = FALSE;
                        }
                        
                        {
                            if(pMe->m_pIAnn != NULL)
                            {
                                IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_RSSI, DBToLevel(pEvtInfo->event_data.ss.ss_info.rssi));
                            }
                        }
                        //IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_RSSI, DBToLevel(pEvtInfo->event_data.ss.ss_info.rssi));
#ifdef FEATURE_LED_CONTROL
#if defined( FEATURE_HAS_NETWORK_LED_CONTROL)
                        if( DBToLevel(pEvtInfo->event_data.ss.ss_info.rssi) == ANNUN_STATE_RSSI_0)
                        {
                            IBACKLIGHT_SigLedDisable(pMe->m_pBacklight);
                        }
                        else
                        {
                            IBACKLIGHT_SigLedDisable(pMe->m_pBacklight);    //cancel the previous LED control
                            IBACKLIGHT_SigLedEnable(pMe->m_pBacklight,SIG_LED_NETWORK);
                        }
#endif
#endif
                    }

                    CoreApp_Process_SS_info(pMe,&pEvtInfo->event_data.ss);
                    break;
#ifdef FEATURE_ICM
                case AEECM_EVENT_SS_HDR_RSSI:
                    break;
#endif
                default:
                    break;
            }
            break;
#ifdef FEATURE_ICM
        case NMASK_CM_PHONE:
#else
        case AEET_NMASK_PHONE:
#endif
            switch(pEvtInfo->event)
            {
                // System preference changed
#ifdef FEATURE_ICM
                case AEECM_EVENT_PH_SYSTEM_PREF:
#else
                case AEET_EVENT_PH_SYSTEM_PREF:
#endif
                    return TRUE;

                // PLMN: Subscription info available
#ifdef FEATURE_ICM
                case AEECM_EVENT_PH_SUBSCRIPTION:
#else
                case AEET_EVENT_PH_SUBSCRIPTION:
#endif
                    return TRUE;

                // CDMA lock mode was changed
#ifdef FEATURE_ICM
                case AEECM_EVENT_PH_CDMA_LOCK_MODE:
                case AEECM_EVENT_PH_FUNDS_LOW:
#else
                case AEET_EVENT_PH_CDMA_LOCK_MODE:
                case AEET_EVENT_PH_FUNDS_LOW:
#endif
                    return TRUE;

                // NAM selection was changed
#ifdef FEATURE_UIM_RUN_TIME_ENABLE
#ifdef FEATURE_ICM
                case AEECM_EVENT_PH_NVRUIM_CONFIG:
#else
                case AEET_EVENT_PH_NVRUIM_CONFIG:
#endif
#endif
#ifdef FEATURE_ICM
                case AEECM_EVENT_PH_NAM_SEL:
#else
                case AEET_EVENT_PH_NAM_SEL:
#endif
                    // Reset the date, operator name
#ifdef FEATURE_OEMOMH
                    if(OEM_IsCDMASVCSupport(UIM_CDMA_SVC_BREW))
                    {
                        extern void OEMOMH_InitBREWDownload(void);
                        OEMOMH_InitBREWDownload();
                    }
                    else
#endif
                    /* Inform BREW sub changed */
                    {
                        IDownload* pDownload = NULL;
                        IConfig*   pConfig   = NULL;
                        
                        DBGPRINTF("CM EVENT %x", pEvtInfo->event);
                        
                        /* Create the objects which contain the information. */
                        ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_DOWNLOAD,
                                              (void**)&pDownload);
                        ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_CONFIG,
                                              (void**)&pConfig);

                        if(pConfig && pDownload)
                        {
                           int nSIDLen;
                           char* pSID = NULL;
                           char* pSIDToSet = NULL;
                           int nResult = AEE_SUCCESS;
                           AEEDownloadInfo DownloadInfo;

                           nResult = ICONFIG_GetItem(pConfig, CFGI_DOWNLOAD,
                                                     (void*)&DownloadInfo,
                                                     sizeof(DownloadInfo));
                           if(nResult == AEE_SUCCESS)
                           {
                              if(DownloadInfo.wFlags & DIF_MIN_FOR_SID)
                              {
                                 AEEMobileInfo MobileInfo;
                                 nResult = ICONFIG_GetItem(pConfig, CFGI_MOBILEINFO,
                                                           &MobileInfo, sizeof(MobileInfo));
                                 if(nResult == AEE_SUCCESS)
                                 {
                                    pSIDToSet = STRDUP(MobileInfo.szMobileID);
                                 }
                                 else
                                 {
                                   DBGPRINTF("Could not get MOBILEINFO %d", nResult);
                                 }
                              }
                              else
                              {
                                 nResult = ICONFIG_GetItem(pConfig, CFGI_SUBSCRIBERID_LEN,
                                                           &nSIDLen, sizeof(nSIDLen));
                                 if(nResult == AEE_SUCCESS)
                                 {
                                    pSID = (char*)MALLOC(nSIDLen);
                                    if(pSID)
                                    {
                                      nResult = ICONFIG_GetItem(pConfig, CFGI_SUBSCRIBERID,
                                                                pSID, nSIDLen);
                                      if(nResult == AEE_SUCCESS)
                                      {
                                         pSIDToSet = STRDUP(pSID);
                                      }
                                      else
                                      {
                                        DBGPRINTF("Could not get SUBSCRIBER ID %d", nResult);
                                      }
                                    }
                                    else
                                    {
                                      DBGPRINTF("Could not get memory for SID %d", nSIDLen);
                                    }
                                 }
                                 else
                                 {
                                   DBGPRINTF("Could not get SUBSCRIBERID_LEN %d", nResult);
                                 }
                              }
                           }
                           else
                           {
                             DBGPRINTF("Could not get CFGI_DOWNLOAD");
                           }

                           if(pSIDToSet)
                           {
                             int i;
                             nSIDLen = STRLEN(pSIDToSet);
                             DBGPRINTF("Setting Subscriber ID len %d %s", nSIDLen,pSIDToSet);
                             IDOWNLOAD_SetSubscriberID(pDownload, pSIDToSet, nSIDLen+1);
                           }
                           FREEIF(pSIDToSet);
                           FREEIF(pSID);
                        }
                        else
                        {
                          DBGPRINTF("Could not create interfaces %x %x", pDownload, pConfig);
                        }

                        if(pDownload)
                           IDOWNLOAD_Release(pDownload);
                        if(pConfig)
                           ICONFIG_Release(pConfig);
                    }
                    return TRUE;

                // Current NAM was changed
#ifdef FEATURE_ICM
                case AEECM_EVENT_PH_CURR_NAM:
#else
                case AEET_EVENT_PH_CURR_NAM:
#endif
                    (void) AEE_IssueSystemCallback(AEE_SCB_DEVICE_INFO_CHANGED);
                    return TRUE;

                // Answer voice as data was changed.
#ifdef FEATURE_ICM
                case AEECM_EVENT_PH_ANSWER_VOICE:
#else
                case AEET_EVENT_PH_ANSWER_VOICE:
#endif
                    return TRUE;

                // Entering powerdown sleep mode
#ifdef FEATURE_ICM
                case AEECM_EVENT_PH_STANDBY_SLEEP:
#else
                case AEET_EVENT_PH_STANDBY_SLEEP:
#endif
                    return TRUE;

                // Existing powerdown sleep mode
#ifdef FEATURE_ICM
                case AEECM_EVENT_PH_STANDBY_WAKE:
#else
                case AEET_EVENT_PH_STANDBY_WAKE:
#endif
                    return TRUE;

                // DMA maintenance required command
#ifdef FEATURE_ICM
                case AEECM_EVENT_PH_MAINTREQ:
#else
                case AEET_EVENT_PH_MAINTREQ:
#endif
                    return TRUE;

                // In use state was changed
#ifdef FEATURE_ICM
                case AEECM_EVENT_PH_IN_USE_STATE:
#else
                case AEET_EVENT_PH_IN_USE_STATE:
#endif
                    return TRUE;

                // Phone information is now available
#ifdef FEATURE_ICM
                case AEECM_EVENT_PH_INFO_AVAIL:
#else
                case AEET_EVENT_PH_INFO_AVAIL:
#endif
                    if (!pMe->m_bProvisioned) {
                       InitAfterPhInfo(pMe, pEvtInfo->event_data.ph.oprt_mode);
                    }
                    return TRUE;

                // Operating mode was changed
#ifdef FEATURE_ICM
                case AEECM_EVENT_PH_OPRT_MODE:
#else
                case AEET_EVENT_PH_OPRT_MODE:
#endif
                    /* continue processing of the event, e.g., update banner etc
                       this is done regardless of whether we are in the Test Mode
                       dialog or not */
                    switch (pEvtInfo->event_data.ph.oprt_mode)
                    {
#ifdef FEATURE_ICM
                    case AEECM_OPRT_MODE_FTM:
#else
                    case AEET_OPRT_MODE_FTM:
#endif
                      return CoreApp_ProcessFTMMode(pMe);
#ifdef FEATURE_ICM
                    case AEECM_OPRT_MODE_OFFLINE:
#else
                    case AEET_OPRT_MODE_OFFLINE:
#endif
                      return CoreApp_ProcessOffLine(pMe);
#ifdef FEATURE_ICM
                    case AEECM_OPRT_MODE_ONLINE:
#else
                    case AEET_OPRT_MODE_ONLINE:
#endif
                      CoreApp_ProcessSubscriptionStatus(pMe);
                      return TRUE;
#ifdef FEATURE_ICM
                    case AEECM_OPRT_MODE_LPM:
#else
                    case AEET_OPRT_MODE_LPM:
#endif 
                      return TRUE;
                      
                    default:
                      /* unhandled OPRT events */
                      return FALSE;
                    }
                    //if(AEECM_OPRT_MODE_ONLINE == pEvtInfo->event_data.ph.oprt_mode)
                    //{
                    //    CoreApp_load_uim_esn(pMe);
                    //}
                    break;

                default:
                    break;
            }
            break;
            
        default:
            break;
    }
    
    if (bUpdate)
    {
        ISHELL_PostEvent(pMe->a.m_pIShell, 
                         AEECLSID_CORE_APP,
                         EVT_UPDATEIDLE,
                         0,0L);
    }
    return TRUE;
}

#ifdef CUST_EDITION
 void CoreApp_InitBattStatus(CCoreApp * pMe)
{
    AEENotify pNotify;

    MEMSET(&pNotify,0x00,sizeof(AEENotify));
    if(pMe->m_pBatt != NULL)
    {        
        pNotify.dwMask = NMASK_BATTERY_CHARGERSTATUS_CHANGE;
        CoreApp_HandleBattNotify(pMe,&pNotify);
        pNotify.dwMask = NMASK_BATTERY_STATUS_CHANGE;
        CoreApp_HandleBattNotify(pMe,&pNotify); 
    }       
}
#endif

/*=============================================================================
FUNCTION: CoreApp_GetCardStatus

DESCRIPTION:
Obtains Card Status for a given slot

SIDE EFFECTS:
  None.

=============================================================================*/
static boolean CoreApp_GetCardStatus(CCoreApp *pMe,uint8 slot)
{
    /* Get the card status from card */
    if (AEECARD_SLOT1 == slot) 
    {
        ICARD_GetCardStatus(pMe->m_pICard, (uint8 *) &(pMe->m_nCardStatus));
    }
    else 
    {
        return FALSE;
    }
    
    return TRUE;
}

/*==============================================================================
函数: 
    CoreApp_HandleBattNotify
       
说明: 
    函数处理来自 AEECLSID_CM_NOTIFIER 的通知事件。
       
参数: 
    pMe [In]: 指向 Core Applet对象结构的指针,该结构包含小程序的特定信息。
    pNotify [in]: 指向 AEENotify 结构体变量的指针。
    
    
返回值:
    TRUE: 事件得到处理
    FALSE:事件没被处理
       
备注:

==============================================================================*/
static boolean CoreApp_HandleBattNotify(CCoreApp * pMe, AEENotify *pNotify)
{
    //AEEBattLevel *pBattLevel;
    AEEBatteryChargerStatus nChargerStatus;
    AEEBatteryStatus nBattStatus;   
    
    if ((NULL == pMe) || (pNotify == NULL))
    {
        return FALSE;
    }
        
    switch (pNotify->dwMask) 
    {
        
        // 外部电源接入或拔除
        case NMASK_BATTERY_EXTPWR_CHANGE:
            pMe->m_bExtPwrState = IBATTERY_GetExternalPower(pMe->m_pBatt);

#ifdef FEATURE_APP_MEDIAGALLERY
            MediaGallery_SetUSBCableConnect(pMe->m_bExtPwrState);
#endif
            if(pMe->m_bExtPwrState == FALSE && pMe->m_wActiveDlgID == IDD_LPM)
            {
                // in this time ,we need power down the phone
                ISHELL_CancelTimer(pMe->a.m_pIShell, CoreApp_Draw_Charger_image, pMe);
                //CLOSE_DIALOG(DLGRET_OK)
                CoreApp_Process_Batty_Msg(pMe, IDS_POWEROFF_1);
                DBGPRINTF("NMASK_BATTERY_EXTPWR_CHANGE");
                AEE_SetSysTimer(POWER_DOWN_TIME , CoreApp_Poweroff_Phone, pMe);
                return TRUE;
            }
            
            CoreApp_Process_Charger_Msg(pMe);
            pMe->m_battery_time= 0;
            pMe->m_battery_state = TRUE ;
            AEE_CancelTimer( CoreApp_Process_Batty_Msg_CB, (void*)pMe);
            IBACKLIGHT_Enable(pMe->m_pBacklight);
            pMe->m_bChargFull = FALSE;
            break;
        
        // 充电状态改变
        case NMASK_BATTERY_CHARGERSTATUS_CHANGE:  
            nChargerStatus = IBATTERY_GetChargerStatus(pMe->m_pBatt);
      //    (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY)CoreApp_Process_BattyLow_Msg, (void *) pMe);
            if(pMe->m_wActiveDlgID == IDD_LPM)
            {
                ISHELL_PostEvent(pMe->a.m_pIShell, 
                                 AEECLSID_CORE_APP,
                                 EVT_USER_REDRAW,
                                 0,0L);
            }
            switch(nChargerStatus)
            {
                case AEEBATTERY_CHARGERSTATUS_FULLY_CHARGE:
                {
                    pMe->m_bChargFull = TRUE;
                    pMe->m_bExtPwrState = TRUE;
                    CoreApp_Process_Batty_Msg(pMe, IDS_FULLY_CHARGED);
                    (void) ISHELL_CancelTimer(pMe->a.m_pIShell, CCharger_EnableICONCB, (void *) pMe);
                    if(pMe->m_pIAnn != NULL)
                    {
                        IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_BATT, ANNUN_STATE_BATT_FULL);
                    }
#ifdef FEATURE_APP_MEDIAGALLERY
                    MediaGallery_SetUSBCableConnect(pMe->m_bExtPwrState);
#endif
#if defined(FEATURE_VERSION_W515V3) || defined(FEATURE_VERSION_S1000T)|| defined(FEATURE_VERSION_W027)
                  if(!pMe->m_bBatteryActive && pMe->m_eCurState == COREST_STANDBY)
                  {
                     pMe->m_bBatteryActive=TRUE;
                     pMe->m_nMsgID = IDS_BATTERY_FULLY_CHARGE;
                     CLOSE_DIALOG(DLGRET_BATT_INFO)
                  }
#endif
                    break;
                }

                case AEEBATTERY_CHARGERSTATUS_CHARGING:
                {
                    pMe->m_bExtPwrState = TRUE;
#ifdef FEATURE_APP_MEDIAGALLERY
                    MediaGallery_SetUSBCableConnect(pMe->m_bExtPwrState);
#endif              
                    if(pMe->m_bChargFull)
                    {
                        break;
                    }
					pMe->m_bBatteryActive=FALSE;
                    (void) ISHELL_SetTimer(pMe->a.m_pIShell,1000, CCharger_EnableICONCB, (void *) pMe);
                    break;
                }

                default:
                    break;
            }
            break;
            
        case NMASK_BATTERY_STATUS_CHANGE:
            pMe->m_bExtPwrState = IBATTERY_GetExternalPower(pMe->m_pBatt);
            if (FALSE == pMe->m_bExtPwrState)
            {
                nBattStatus = IBATTERY_GetStatus(pMe->m_pBatt);
                switch(nBattStatus)
                {
                    case AEEBATTERY_STATUS_POWERDOWN:  // Phone must be powered down
                    {
                        ISHELL_SendEvent(pMe->a.m_pIShell,AEECLSID_DIALER, EVT_BATT_POWERDOWN, 0, 0);
                        CoreApp_Process_Batty_Msg(pMe, IDS_POWEROFF_1);
                        DBGPRINTF("AEEBATTERY_STATUS_POWERDOWN");
                        AEE_SetSysTimer(POWER_DOWN_TIME , CoreApp_Poweroff_Phone, pMe);
                        break;
                    }

                    case AEEBATTERY_STATUS_LOW:        // Battery is low
                    {
                        (void) ISHELL_CancelTimer(pMe->a.m_pIShell,CCharger_BlinkLowBattIcon, (void *) pMe);                        
                        if(pMe->m_pIAnn != NULL)
                        {
                            IANNUNCIATOR_SetField(pMe->m_pIAnn, ANNUN_FIELD_BATT, ANNUN_STATE_BATT_LOW | ANNUN_STATE_BLINK);
                        }
                        (void) ISHELL_SetTimer(pMe->a.m_pIShell, 10000, CCharger_BlinkLowBattIcon, (void *) pMe);
                        CoreApp_Process_BattyLow_Msg(pMe, IDS_LOWBATTMSG_TEXT);
                        break;
                    }

                    case AEEBATTERY_STATUS_NORMAL:      // Battery is normal
                    default:
                        break;
                }
            }
            break;

        case NMASK_BATTERY_LEVEL_CHANGE:
        {
            uint32 nBattState = CoreApp_ConvertBattLvToAnnunState(CoreApp_GetBatteryLevel(pMe));
            //IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_BATT, ANNUN_STATE_OFF);
            IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_BATT, nBattState);
            break;
        }
        
        default:
            ERR("Unknown IBatt notification", 0, 0, 0);
            break;
    }
    
    return FALSE;
}
/*==============================================================================
函数: 
    CoreApp_CreateInstance
       
说明: 
    函数用于载入应用程序。
       
参数: 
    ClsId [in]: 将创建实例的应用对象的 ID
    pIShell [in]: IShell 接口指针。
    pMod [in]: 将创建实例应用所属的模块接口指针。
    ppObj [in/out]: 用于返回创建的应用对象。需指向有效的 AEEApplet 结构。
    
    
返回值:
    SUCCESS:成功，应用会被载入
    EFAILED:失败，应用不会载入
       
备注:
    当应用载入时，本函数被调用。所有模块必须提供本函数。确?:筒问恢隆Ｕ?
    里，模块必须校验 ClsId ，然后调用文件 AEEAppGen.c 提供的函数 AEEApplet_New()
    。调用 AEEApplet_New() 成功后，函数可以做应用特定初始化。
==============================================================================*/
int CoreApp_CreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * pMod,void ** ppObj)
{
    *ppObj = NULL;
    
    if (AEEApplet_New(sizeof(CCoreApp),                    /* Size of our private class */
                      ClsId,                               /* Our class ID */
                      pIShell,                             /* Shell interface */
                      pMod,                                /* Module instance */
                      (IApplet**)ppObj,                    /* Return object */
                      (AEEHANDLER)CoreApp_HandleEvent,     /* Our event handler */
                      (PFNFREEAPPDATA)CoreApp_FreeAppData))/* Cleanup function */
    {
        if (CoreApp_InitAppData((IApplet*)*ppObj)) 
        {
            return(AEE_SUCCESS);
        }
        
        // 初始化不成功，释放为此 Applet 分配的内存
        IAPPLET_Release((IApplet*)*ppObj);
    }
    
    return (EFAILED);
} /* End CoreApp_CreateInstance / AEEClsCreateInstance */

#ifdef AEE_STATIC
/*==============================================================================
函数: 
    CoreApp_Load
       
说明: 
    函数用于载入应用程序。
       
参数: 
    ps [in]: IShell 接口指针。
    pHelpers [in]: 
    pMod [in/out]: 用于返回创建的模块地址。
    
返回值:
    载入应用的函数指针
       
备注:

==============================================================================*/
int CoreApp_Load(IShell *ps, void * pHelpers, IModule ** pMod)
{
    int nRet;
    
    nRet = AEEStaticMod_New((int16)(sizeof(AEEMod)),
                            ps, 
                            pHelpers,
                            pMod,
                            CoreApp_CreateInstance,
                            NULL);
    return(nRet);
} /* End CoreApp_Load */

#endif  /* AEE_STATIC */

/*==============================================================================
函数: 
    CoreApp_RunFSM
       
说明: 
    Core Applet有限状态机引擎。
       
参数: 
    pMe [in]:指向 Core Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值:
    none
       
备注:
       
==============================================================================*/
static void CoreApp_RunFSM(CCoreApp *pMe)
{
    NextFSMAction nextFSMAction = NFSMACTION_WAIT;

    for ( ; ; )
    {
        nextFSMAction = CoreApp_ProcessState(pMe);

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
} // CoreApp_RunFSM

/*==============================================================================
函数:
    CoreApp_RegisterNotify

说明:
    用于将话机置于在线操作模式。

参数:
    pMe [in]:指向 Core Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    none

备注:
       
==============================================================================*/
boolean CoreApp_RegisterNotify(CCoreApp *pMe)
{
    int nRet;
    uint32 dwMask;
#ifdef FEATURE_ICM
    // register with ICM
    dwMask = NMASK_CM_PHONE|NMASK_CM_SS|NMASK_CM_DATA_CALL;
    //dwMask = NMASK_CM_PHONE|NMASK_CM_SS;
    nRet = ISHELL_RegisterNotify(pMe->a.m_pIShell, 
                                 AEECLSID_CORE_APP, 
                                 AEECLSID_CM_NOTIFIER, 
                                 dwMask);
#else
    // register with ICM
    dwMask = AEET_NMASK_PHONE|AEET_NMASK_SS|AEET_NMASK_DATA_CALL;
    //dwMask = NMASK_CM_PHONE|NMASK_CM_SS;
    nRet = ISHELL_RegisterNotify(pMe->a.m_pIShell, 
                                 AEECLSID_CORE_APP, 
                                 AEECLSID_PHONENOTIFIER, 
                                 dwMask);
#endif
    if (nRet != SUCCESS) 
    {
        return FALSE;
    }
    
    // 注册 IBatt 通知事件：仅注册外部电源和充电状态通知事件，其余
    // 通知事件一旦话机启动并运行将被注册
    dwMask = NMASK_BATTERY_STATUS_CHANGE 
            |NMASK_BATTERY_LEVEL_CHANGE
            |NMASK_BATTERY_CHARGERSTATUS_CHANGE
            |NMASK_BATTERY_EXTPWR_CHANGE;
                                           
    nRet = ISHELL_RegisterNotify(pMe->a.m_pIShell,
                        AEECLSID_CORE_APP, 
                        AEECLSID_BATTERYNOTIFIER,  
                        dwMask);
    if (nRet != SUCCESS) 
    {
        return FALSE;
    }     
    
    if(ISHELL_RegisterNotify(pMe->a.m_pIShell, AEECLSID_CORE_APP,
        AEECLSID_ALERT_NOTIFIER, NMASK_ALERT_ONOFF | NMASK_ALERT_MUTED) != SUCCESS)
    {
        return FALSE;
    }

    return TRUE;    
   
}

/*==============================================================================
函数:
    CoreApp_InitExtInface

说明:
    用于将话机置于在线操作模式。

参数:
    pMe [in]:指向 Core Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    none

备注     :  WHEN                    WHO                WHAT                     WHY
            2008-02-20              fj                 V1.6                     Instance AEECLSID_ALERT for power on ring       
==============================================================================*/
boolean CoreApp_InitExtInterface(CCoreApp *pMe)
{
    int nRet;
#ifdef FEATURE_ICM
    AEECMPhInfo phInfo;
    
    pMe->m_bProvisioned = FALSE;
    // 创建 ICM 接口
    nRet = ISHELL_CreateInstance(pMe->a.m_pIShell,
                                 AEECLSID_CALLMANAGER,
                                 (void **) &pMe->m_pCM);
    if (nRet != SUCCESS) 
    {
        return FALSE;
    }
    if (pMe->m_pCM == NULL) 
    {
        return FALSE;
    }
    
    ICM_SetRSSIDeltaThreshold(pMe->m_pCM, 5);
    /* If phone info is available, do not wait for PH_INFO_AVAIL event for
       * starting provisioning */
    if (!pMe->m_bProvisioned && (SUCCESS == ICM_GetPhoneInfo(pMe->m_pCM, &phInfo, sizeof(AEECMPhInfo))))
#else
    AEETPhInfo phInfo;
    
    pMe->m_bProvisioned = FALSE;
    // 创建 ICM 接口
    nRet = ISHELL_CreateInstance(pMe->a.m_pIShell,
                                 AEECLSID_TELEPHONE,
                                 (void **) &pMe->m_pITelephone);
    if (nRet != SUCCESS) 
    {
        return FALSE;
    }
    if (pMe->m_pITelephone == NULL) 
    {
        return FALSE;
    }	

	nRet = ISHELL_CreateInstance(pMe->a.m_pIShell,
                                 AEECLSID_CALLMGR,
                                 (void **) &pMe->m_pICallMgr);
	if (nRet != SUCCESS) 
    {
        return FALSE;
    }
    if (pMe->m_pICallMgr == NULL) 
    {
        return FALSE;
    }
	
    nRet = ISHELL_CreateInstance(pMe->a.m_pIShell,
                                 AEECLSID_PHONECTL,
                                 (void **) &pMe->m_pIPhoneCtl);
	if (nRet != SUCCESS) 
    {
        return FALSE;
    }
    if (pMe->m_pIPhoneCtl == NULL) 
    {
        return FALSE;
    }
	
    IPHONECTL_SetRSSIDeltaThreshold(pMe->m_pIPhoneCtl, 5);
    /* If phone info is available, do not wait for PH_INFO_AVAIL event for
       * starting provisioning */
    if (!pMe->m_bProvisioned && (SUCCESS == ITELEPHONE_GetPhoneInfo(pMe->m_pITelephone, &phInfo, sizeof(AEETPhInfo))))
#endif
    {
        InitAfterPhInfo(pMe, phInfo.oprt_mode);
    }

    // 创建 IBatt 接口
    nRet = ISHELL_CreateInstance(pMe->a.m_pIShell,
                                 AEECLSID_BATTERY,
                                 (void **) &pMe->m_pBatt);
    if (nRet != SUCCESS) 
    {
        return FALSE;
    }
    
    nRet = ISHELL_CreateInstance(pMe->a.m_pIShell,
                                 AEECLSID_ALERT,
                                 (void **)&pMe->m_pAlert);
    if (nRet != SUCCESS) 
    {
        return FALSE;
    }
    
    if (pMe->m_pAlert == NULL) 
    {
        return FALSE;
    }
    
    nRet = ISHELL_CreateInstance(pMe->a.m_pIShell,
                                 AEECLSID_ANNUNCIATOR,
                                 (void **) &pMe->m_pIAnn);
    if (nRet != SUCCESS) 
    {
        return FALSE;
    }
    if (pMe->m_pIAnn == NULL) 
    {
        return FALSE;
    }
    else
    {
        uint32 nBattState = CoreApp_ConvertBattLvToAnnunState(CoreApp_GetBatteryLevel(pMe));
        IANNUNCIATOR_EnableAnnunciatorBar(pMe->m_pIAnn,AEECLSID_DISPLAY1,FALSE);
        IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_BATT, nBattState);
    }
    
#ifdef FEATURE_UIALARM
    // 创建 IAlarm 接口
    nRet = ISHELL_CreateInstance(pMe->a.m_pIShell,
                                 AEECLSID_UIALARM,
                                 (void **)&pMe->m_pAlarm);
    if (nRet != SUCCESS) 
    {
        return FALSE;
    }
    if (pMe->m_pAlarm == NULL) 
    {
        return FALSE;
    }
#endif 
    
#ifndef WIN32
    // 创建 IRUIM 接口
    nRet = ISHELL_CreateInstance(pMe->a.m_pIShell,
                                 AEECLSID_RUIM,
                                 (void **) &pMe->m_pIRUIM);
    if (nRet != SUCCESS) 
    {
        return FALSE;
    }
    if (pMe->m_pIRUIM == NULL) 
    {
        return FALSE;
    }
#endif//WIN32
    
    // Create the IConfig Serivce object.
    nRet = ISHELL_CreateInstance(pMe->a.m_pIShell,
                                 AEECLSID_CONFIG,
                                (void **) &pMe->m_pConfig);
    if (nRet != SUCCESS) 
    {
        return FALSE;
    }
    if (pMe->m_pConfig == NULL) 
    {
        return FALSE;
    }
    
    if (ISHELL_CreateInstance(pMe->a.m_pIShell,
                                            AEECLSID_DISPLAY,
                                            (void **)&pMe->m_pDisplay)!=AEE_SUCCESS)
    {
        CoreApp_FreeAppData( (IApplet*)pMe );
        return FALSE;
    }
    
    IDisplay_SetPrefs(pMe->m_pDisplay, "a:0", 3);
    IDisplay_SetClipRect(pMe->m_pDisplay, NULL);
    pMe->m_bemergencymode = FALSE;
    return TRUE;

}

/*==============================================================================
函数:
    CoreApp_PoweronStartApps

说明:
    开机时，部分 Applet 需要后台启动一次，以便注册相关应用。

参数:
    pMe [in]:指向 Core Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    none

备注:
       
==============================================================================*/
static void CoreApp_PoweronStartApps(CCoreApp *pMe)
{
    static boolean bRun = FALSE;
    
    if (bRun)
    {
        return;
    }
    
#ifdef FEATURE_SUPPORT_WAP_APP
#if 0
    PushMod_StartPush(pMe->a.m_pIShell);
#endif
#endif

#ifdef FEATURE_KEYGUARD
#ifdef FEATURE_ICM
    OEMKeyguard_Init(pMe->a.m_pIShell,pMe->m_pCM,pMe->m_pAlert,pMe->m_pIAnn);
#else
    OEMKeyguard_Init(pMe->a.m_pIShell,pMe->m_pITelephone,pMe->m_pAlert,pMe->m_pIAnn);
#endif
#endif
#ifdef FEATURE_LCD_TOUCH_ENABLE
{
    pen_cal_type pen_adjust_data;
    
    (void)OEM_GetConfig(CFGI_PEN_CAL, (void*)&pen_adjust_data, sizeof(pen_cal_type));
	MSG_FATAL("pen_adjust_data.kmvx===%d,pen_adjust_data.kmvy=%d",pen_adjust_data.kmvx,pen_adjust_data.kmvy,0);
	MSG_FATAL("pen_adjust_data.kmv0===%d,pen_adjust_data.kmv0=%d",pen_adjust_data.mvx0,pen_adjust_data.mvy0,0);
    if(pen_adjust_data.kmvx == -1 && pen_adjust_data.kmvy == -1
        && pen_adjust_data.mvx0 == -1 && pen_adjust_data.mvy0 == -1 )
    {
        ISHELL_StartApplet(pMe->a.m_pIShell, AEECLSID_ADJUSTPENAPP);
    }
}
#endif
    bRun = TRUE;
}


/*===========================================================================
FUNCTION DBToLevel

DESCRIPTION
  Converts RSSI absolute DB value to an annunciator level

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static int DBToLevel (int nDBVal)
{
    MSG_FATAL("DBToLevel",0,0,0);
	#ifdef FEATURE_VERSION_VG68
    if (nDBVal < 92)
        return ANNUN_STATE_RSSI_6;
    else if(nDBVal<95)
        return ANNUN_STATE_RSSI_5;
    else if (nDBVal < 97)
        return ANNUN_STATE_RSSI_4;
    else if (nDBVal < 102)
        return ANNUN_STATE_RSSI_3;
    else if (nDBVal < 106)
        return ANNUN_STATE_RSSI_2;
	#else
	if (nDBVal < 92)
        return ANNUN_STATE_RSSI_4;
    else if (nDBVal < 97)
        return ANNUN_STATE_RSSI_3;
    else if (nDBVal < 102)
        return ANNUN_STATE_RSSI_2;
	#endif
    else if (nDBVal < 108)
        return ANNUN_STATE_RSSI_1;
    else
        return ANNUN_STATE_RSSI_0;
}

/*=============================================================================
FUNCTION: CCharger_EnableICONCB

DESCRIPTION:

PARAMETERS:
   *pUser:

RETURN VALUE:
    None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void CCharger_EnableICONCB(void *pUser)
{
    CCoreApp    *pMe = (CCoreApp *)pUser;
    static int BattLevel = 0;
    
    (void) ISHELL_CancelTimer(pMe->a.m_pIShell, CCharger_EnableICONCB, (void *) pMe);

    if (TRUE == pMe->m_bExtPwrState)
    {
        uint32 nBattState = 0;
        
        if (BattLevel++ >= CHARGE_FULL_STATE)
        {
            BattLevel = 0;
        }
        
        nBattState = CoreApp_ConvertBattLvToAnnunState(BattLevel);
        IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_BATT, nBattState);
        (void) ISHELL_SetTimer(pMe->a.m_pIShell, 1000, CCharger_EnableICONCB, (void *) pMe);
    }
    else
    {   //及时更新停止充电时的电池level显示
        uint32 nBattState = CoreApp_ConvertBattLvToAnnunState(CoreApp_GetBatteryLevel(pMe));
        IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_BATT, nBattState);
    }
}

/*=============================================================================
FUNCTION: CCharger_BlinkLowBattIcon

DESCRIPTION:

PARAMETERS:
   *pUser:

RETURN VALUE:
    None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void CCharger_BlinkLowBattIcon(void *pUser)
{
    CCoreApp    *pMe = (CCoreApp *)pUser;
    //static int BattLevel = 0;

    IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_BATT, ANNUN_STATE_BATT_LOW);
}
#ifdef FEATURE_ICM
/*=============================================================================
FUNCTION: CoreApp_IsEmergencyMode

DESCRIPTION: Check if the  phone  is emergency mode

PARAMETERS:
    pICM            :  [IN] ICM Instance

RETURN VALUE:
    boolean         : return TRUE if phone  is in emergency mode

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CoreApp_IsEmergencyMode(ICM* pICM)
{
    AEECMPhInfo phoneInfo;
    MSG_FATAL("CoreApp_IsEmergencyMode Start",0,0,0);
    
    //PRINT_FUNCTION_NAME();

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
    MSG_FATAL("CoreApp_IsEmergencyMode End",0,0,0); 
}
#else
/*=============================================================================
FUNCTION: CoreApp_IsEmergencyMode

DESCRIPTION: Check if the  phone  is emergency mode

PARAMETERS:
    m_pITelephone            :  [IN] m_pITelephone Instance

RETURN VALUE:
    boolean         : return TRUE if phone  is in emergency mode

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CoreApp_IsEmergencyMode(ITelephone* pITelephone)
{
    AEETPhInfo phoneInfo;
    MSG_FATAL("CoreApp_IsEmergencyMode Start",0,0,0);
    
    //PRINT_FUNCTION_NAME();

    if (!pITelephone)
    {
        return FALSE;
    }

    ITELEPHONE_GetPhoneInfo(pITelephone, &phoneInfo, sizeof(phoneInfo));
    if (phoneInfo.mode_pref == AEET_MODE_PREF_EMERGENCY)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
    MSG_FATAL("CoreApp_IsEmergencyMode End",0,0,0); 
}
#endif
#ifdef FEATRUE_AUTO_POWER
static void CoreApp_Process_AutoPower_Event(void *pUser)
{
    CCoreApp    *pMe = (CCoreApp *)pUser;
    db_items_value_type db;
    db_get(DB_IN_USE, &db);
    if(db.in_use)
    //if(AEECM_IS_VOICECALL_CONNECTED(pMe->m_pCM))
    {
        ISHELL_PostEvent(pMe->a.m_pIShell,AEECLSID_DIALER,EVT_AUTO_POWERDOWN,0,0);
        return;
    }
    pMe->m_b_autopower_off = TRUE;
    if (ISHELL_ActiveApplet(pMe->a.m_pIShell) != AEECLSID_CORE_APP)
    {
        pMe->m_b_needclose_core = TRUE;
        ISHELL_StartApplet(pMe->a.m_pIShell, AEECLSID_CORE_APP);        
    }
    else
    {
        if(IDIALOG_GetID(ISHELL_GetActiveDialog(pMe->a.m_pIShell)) == IDD_IDLE)
        {
            CLOSE_DIALOG(DLGRET_MSG);
        }
    }
}
#endif

#ifdef FEATURE_UIALARM
/*==============================================================================
函数：
       CoreApp_EnableShellAlarms

说明：
       函数用于挂起或恢复通过 IAlarm 接口设置的警报器。

参数：
       pMe [In]: 指向CoreApp Applet对象结构的指针,该结构包含小程序的特定信息.
       bEnabled [In]: 布尔变量。
       
返回值：
       无。

备注：:
       当 bEnabled 为TRUE 时恢复警报器；否则，挂起警报器。
       
==============================================================================*/
void CoreApp_EnableShellAlarms(CCoreApp   *pMe, boolean  bEnabled)
{
    if (NULL == pMe)
    {
        return;
    }
    
    if (bEnabled) 
    {
        IAlarm_ResumeAlarms(pMe->m_pAlarm);
    } 
    else 
    {
        IAlarm_SuspendAlarms(pMe->m_pAlarm);
    }
}
#endif
static void CoreApp_Process_Batty_Msg(CCoreApp   *pMe, uint16  msg_id)
{
    static uint16 prv_msg_id = 0;
    if(prv_msg_id != msg_id)
    {
        AEE_CancelTimer( CoreApp_Process_Batty_Msg_CB, (void*)pMe);
        pMe->m_battery_time = 0;
        pMe->m_battery_state = TRUE;
    }

    if(pMe->m_battery_time < MAX_BATTERY_POP_COUNT && pMe->m_battery_state)
    {
        CoreNotifyMP3PlayerAlertEvent(pMe, TRUE);
        if(msg_id == IDS_LOWBATTMSG_TEXT)
            IALERT_StartAlerting(pMe->m_pAlert, NULL, NULL, AEEALERT_ALERT_LOW_BATTERY);
        else
            IALERT_StartAlerting(pMe->m_pAlert, NULL, NULL, AEEALERT_ALERT_ROAMING);
        ISHELL_SetTimer(pMe->a.m_pIShell, 1500,(PFNNOTIFY)CoreNotifyMP3PlayerAlertEventCB, pMe);
        if((pMe->m_wActiveDlgID == IDD_PWDIMSIMCC ||
            pMe->m_wActiveDlgID == IDD_PWDINPUT ||
            pMe->m_wActiveDlgID == IDD_UIMSECCODE ||
            pMe->m_wActiveDlgID == IDD_EMERGENCYNUMLIST ||
            pMe->m_wActiveDlgID == IDD_IDLE
            ) && (pMe->m_bSuspended == FALSE))
        {
            pMe->m_nMsgID = msg_id;
            MSG_FATAL("CoreApp_Process_Batty_Msg 1",0,0,0); 
            CLOSE_DIALOG(DLGRET_BATT_INFO)
        }
        pMe->m_battery_time ++;
        pMe->m_battery_state = FALSE ;
        AEE_SetSysTimer(600000/*600000 10 minute*/, CoreApp_Process_Batty_Msg_CB, (void*)pMe);
    }
    prv_msg_id = msg_id;
}
static void CoreApp_Process_BattyLow_Msg(CCoreApp   *pMe, uint16  msg_id)
{
#ifdef FEATURE_VERSION_C337
    if(pMe->m_battery_state)
#endif
    {
       
       AEE_CancelTimer(CoreApp_Process_Batty_Msg_CB, (void*)pMe);
       if(msg_id == IDS_LOWBATTMSG_TEXT)
       {
           IALERT_StartAlerting(pMe->m_pAlert, NULL, NULL, AEEALERT_ALERT_LOW_BATTERY);
       }
       else
       {
           IALERT_StartAlerting(pMe->m_pAlert, NULL, NULL, AEEALERT_ALERT_ROAMING);
       }
       if((pMe->m_wActiveDlgID == IDD_PWDIMSIMCC ||
                  pMe->m_wActiveDlgID == IDD_PWDINPUT ||
                  pMe->m_wActiveDlgID == IDD_UIMSECCODE ||
                  pMe->m_wActiveDlgID == IDD_EMERGENCYNUMLIST ||
                  pMe->m_wActiveDlgID == IDD_IDLE
                  ) && (pMe->m_bSuspended == FALSE))
       {
           pMe->m_nMsgID = msg_id;
           MSG_FATAL("CoreApp_Process_Batty_Msg 1",0,0,0); 
           pMe->m_battery_state = FALSE ;
           #ifdef FEATURE_VERSION_C337
           AEE_SetSysTimer(10000, CoreApp_Process_Batty_Msg_CB, (void*)pMe);
           #endif
           CLOSE_DIALOG(DLGRET_BATT_INFO)
         //ISHELL_MessageBox(pMe->a.m_pIShell,AEE_COREAPPRES_LANGFILE,NULL,IDS_LOWBATTMSG_TEXT);
           
       }
    }
}

static void CoreApp_Process_Batty_Msg_CB(void *pp)
{
    CCoreApp   *pMe = (CCoreApp *)pp;
    pMe->m_battery_state = TRUE;
}

static void CoreApp_Process_Charger_Msg(CCoreApp   *pMe)
{
    //CORE_ERR("Charger_Msg %d %d",pMe->m_wActiveDlgID,pMe->m_bExtPwrState);
    if((   pMe->m_wActiveDlgID == IDD_PWDIMSIMCC ||
            pMe->m_wActiveDlgID == IDD_PWDINPUT ||
            pMe->m_wActiveDlgID == IDD_UIMSECCODE ||
            pMe->m_wActiveDlgID == IDD_EMERGENCYNUMLIST ||
            pMe->m_wActiveDlgID == IDD_IDLE
            ) && (pMe->m_bSuspended == FALSE))
    {
        MSG_FATAL("CoreApp_Process_Charger_Msg Start",0,0,0); 
        if(pMe->m_bExtPwrState)
        {
            #if (defined(FEATURE_VERSION_C337) || defined(FEATURE_VERSION_W317A))
            pMe->m_nMsgID = IDS_CHARGER_CONNECTED;
            #else
            pMe->m_nMsgID = IDS_CHARGER_ON;
            #endif
         CLOSE_DIALOG(DLGRET_BATT_INFO)

        }
       else
       {
          #if (defined(FEATURE_VERSION_C337) || defined(FEATURE_VERSION_W317A))
          pMe->m_nMsgID = IDS_CHARGER_REMOVED;
          #else
          pMe->m_nMsgID = IDS_CHARGER_OFF;
          #endif
          CLOSE_DIALOG(DLGRET_BATT_INFO)

       }
    }
}

static void CoreApp_Process_Headset_Msg(CCoreApp *pMe, uint16 msgId)
{
    if((   pMe->m_wActiveDlgID == IDD_PWDIMSIMCC ||
            pMe->m_wActiveDlgID == IDD_PWDINPUT ||
            pMe->m_wActiveDlgID == IDD_UIMSECCODE ||
            pMe->m_wActiveDlgID == IDD_EMERGENCYNUMLIST ||
            pMe->m_wActiveDlgID == IDD_IDLE
            ) && (pMe->m_bSuspended == FALSE))
    {
          pMe->m_nMsgID = msgId;
          CLOSE_DIALOG(DLGRET_BATT_INFO)
    }
}
#ifdef FEATURE_POWERUP_REGISTER_CHINAUNICOM
/*==============================================================================
函数：
    CoreApp_SendReginfoTimer

说明：
    用于发送 EVT_SENDREGINFOR 给 AEECLSID_CORE_APP 通知程序发送手机注册事件。

参数：
    pme [in]：指向CCoreApp Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
    无。

备注：
    若 Applet 被挂起，将不做任何操作直接返回。
       
==============================================================================*/
void CoreApp_SendReginfoTimer(void *pme)
{
   CCoreApp *pMe = (CCoreApp *)pme;
   
   if (NULL == pMe)
   {
      return;
   }
   
   // 发送EVT_DISPLAYDIALOGTIMEOUT事件
  (void) ISHELL_PostEvent(pMe->a.m_pIShell,
                          AEECLSID_CORE_APP,
                          EVT_MOBILE_TRACKER,
                          0,
                          0);
}

/*==============================================================================
函数：
    CoreApp_SendReginfoCallBack

说明：
    传给 OEMTAPI 的回调函数。由 OEMTAPI 调用用来通知Applet短消息发送状态。

参数：
    pUser {in]：void类型指针(实际是指向 CCoreApp 结构的指针)
    status [in]：消息发送返回的结果码。
       
返回值：
    无。

备注:
       

==============================================================================*/
void CoreApp_SendReginfoCallBack(void * pUser, int status)
{
    CCoreApp *pMe = (CCoreApp *) pUser;
    
    if (pMe == NULL)
    {
        return;
    }
    
    (void) ISHELL_PostEvent(pMe->a.m_pIShell,
                AEECLSID_CORE_APP,
                EVT_SENDSTATUS,
                0,
                (uint32)status);
}

/*==============================================================================
函数：
    CoreApp_SendReginfo

说明：
    函数用来发送注册信息。

参数：
    pMe [in]：指向CCoreApp Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值：
    int 

备注:

==============================================================================*/
int CoreApp_SendReginfo(CCoreApp   *pMe)
{
    IdleAPP_Config_Type cfg;
    int  result = SUCCESS;
    AEEMobileInfo     mi;
    IWmsApp *pIWmsApp = NULL;
    AECHAR  wstrType[2] = {(AECHAR)POWERUP_REGISTER_CHINAUNICOM, 0};
    //CORE_ERR("START CoreApp_SendReginfo",0,0,0);
    
    if (pMe == NULL)
    {
        return EFAILED;
    }
    
    // 获取手机电子串号
    OEM_GetConfig (CFGI_MOBILEINFO, &mi,  sizeof(AEEMobileInfo));
    if (mi.dwESN == 0)
    {
        return EFAILED;
    }

    (void)ISHELL_GetPrefs(pMe->a.m_pIShell, 
                AEECLSID_CORE_APP, 
                IDLE_CFG_VERSION,
                &cfg, 
                sizeof(cfg));
                            
    if (cfg.nInitIndictor != IDLE_CFG_VERSION)
    {   // 机器首次开机使用
        cfg.nInitIndictor = IDLE_CFG_VERSION;
        (void)STRCPY(cfg.m_RegItem.szMobileIMSI, mi.szMobileID);
        cfg.m_RegItem.m_bRegistered = FALSE;
        
        // 初始化配置
        (void)ISHELL_SetPrefs(pMe->a.m_pIShell, 
                AEECLSID_CORE_APP, 
                IDLE_CFG_VERSION,
                &cfg, 
                sizeof(cfg));
    }
    else
    {
        if ((STRCMP(cfg.m_RegItem.szMobileIMSI, mi.szMobileID)==0) &&
            cfg.m_RegItem.m_bRegistered)
        {// 已注册,不需再注册
            return SUCCESS;
        }
    }
            
    result = ISHELL_CreateInstance(pMe->a.m_pIShell,
                                 AEECLSID_WMSAPP,
                                 (void **) &pIWmsApp);
    if ((result == SUCCESS) && (NULL != pIWmsApp))
    {
        result = IWmsApp_SendSpecMessage(pIWmsApp, wstrType);
        IWmsApp_Release(pIWmsApp);
    }
    
    //CORE_ERR("END CoreApp_SendReginfo",0,0,0);
    return result;
}   
#endif


#if defined(FEATURE_VERSION_W317A)||defined(FEATURE_VERSION_C337)
/*==============================================================================
函数：
    CoreApp_MobileTracker

说明：
    函数用来发送注册信息。

参数：
    pMe [in]：指向CCoreApp Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值：
    int 

备注:

==============================================================================*/
int CoreApp_MobileTracker(CCoreApp *pme)
{
    IdleAPP_Config_MobileTracker_Type cfg;
    int  result = SUCCESS;
    AEEMobileInfo     mi;
    IWmsApp *pIWmsApp = NULL;
    AECHAR  wstr[120] = {0};
	uint16  wstrNumber[20] = {0};
	uint16  wstrImsi[16] = {0};
	char strImsi[16] = {0};
	uint16 len = 0;
	AECHAR  wstrType[2] = {(AECHAR)MOBILE_TRACKER_MSG, 0};
	
    MSG_FATAL("START CoreApp_MobileTracker",0,0,0);
    
    if (pme == NULL)
    {
        return EFAILED;
    }
    
    // 获取手机电子串号
    OEM_GetConfig (CFGI_MOBILEINFO, &mi,  sizeof(AEEMobileInfo));
	MSG_FATAL("mi.szMobileID!!!!!!!!!!!!=%0x",mi.szMobileID,0,0);
    if (mi.szMobileID == 0)
    {
    	MSG_FATAL("NOT UIM card!!!!!!!!!!!!",0,0,0);
        return EFAILED;
    }
    /*
    (void)ISHELL_GetPrefs(pme->a.m_pIShell, 
                AEECLSID_CORE_APP, 
                IDLE_CFG_MOBILETRANKER_VERSION,
                &cfg, 
                sizeof(cfg));*/
	(void) ICONFIG_GetItem(pme->m_pConfig,  
                           CFGI_MOBILE_TRACKER_IMSI,
                           wstrImsi, 
                           sizeof(wstrImsi));
	(void) WSTRTOSTR(wstrImsi, strImsi, sizeof(strImsi));

	MSG_FATAL("cfg.m_RegItem.szMobileIMSI!!!!!!!!!!!!=%0x",strImsi,0,0);
                            
    
    if ((STRCMP(strImsi, mi.szMobileID)==0))
    {// 已注册,不需再注册
    	MSG_FATAL("END CoreApp_MobileTracker=sucess=",0,0,0);
        return SUCCESS;
    }


	(void) ICONFIG_GetItem(pme->m_pConfig,  
                           CFGI_MOBILE_TRACKER_PHONENUMB,
                           wstrNumber, 
                           sizeof(wstrNumber));
	 len = WSTRLEN(wstrNumber);
	MSG_FATAL("wstrNumber len!!!!!!!!!!!!==%d",len,0,0);
	if(WSTRLEN(wstrNumber)<5)
	{
		return SUCCESS;
	}
            
    result = ISHELL_CreateInstance(pme->a.m_pIShell,
                                 AEECLSID_WMSAPP,
                                 (void **) &pIWmsApp);
    if ((result == SUCCESS) && (NULL != pIWmsApp))
    {
        //result = IWmsApp_SendTextMessageExt(pIWmsApp,wstrNumber,wstr);
        //IWmsApp_Release(pIWmsApp);
        result = IWmsApp_SendSpecMessage(pIWmsApp, wstrType);
        IWmsApp_Release(pIWmsApp);
    }
    
    MSG_FATAL("END CoreApp_MobileTracker==%d",result,0,0);
    return result;
   
   
}

void CoreApp_MobileTrackerTimer(void *pme)
{
	CCoreApp *pMe = (CCoreApp *)pme;
   
  	 if (NULL == pMe)
   	{
      	return;
   	}
   
   	// 发送EVT_DISPLAYDIALOGTIMEOUT事件
  	(void) ISHELL_PostEvent(pMe->a.m_pIShell,
                          AEECLSID_CORE_APP,
                          EVT_MOBILE_TRACKER,
                          0,
                          0);
}

void CoreApp_SalesTrackerTimer(void *pme)
{
	CCoreApp *pMe = (CCoreApp *)pme;
    uint16 m_alarm_time = 0;
  	 if (NULL == pMe)
   	{
      	return;
   	}
	(void)OEM_GetConfig(CFGI_SMS_TRACKER_TIME,
                           &m_alarm_time, 
                           sizeof(uint16));
	if(m_alarm_time<0)
	{
		m_alarm_time = SMS_TRACKER_TIME;
	}
	else
	{
		m_alarm_time=m_alarm_time*60;
	}
	
    MSG_FATAL("CoreApp_SalesTrackerTimer...m_alarm_time===%d",m_alarm_time,0,0);
   	IAlarm_SetAlarm( pMe->m_pIAlarm,
                        AEECLSID_CORE_APP,
                        PERMID,
                        m_alarm_time
                    	);
}

/*==============================================================================
函数：
    CoreApp_SMSTracker

说明：
    函数用来发送注册信息。

参数：
    pMe [in]：指向CCoreApp Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值：
    int 

备注:

==============================================================================*/
int CoreApp_SMSTracker(CCoreApp *pme)
{
	int  result = SUCCESS;
	IWmsApp *pIWmsApp = NULL;
	AECHAR  wstrType[2] = {(AECHAR)SMS_TRACKER_MSG, 0};
	
	result = ISHELL_CreateInstance(pme->a.m_pIShell,
                                 AEECLSID_WMSAPP,
                                 (void **) &pIWmsApp);
    if ((result == SUCCESS) && (NULL != pIWmsApp))
    {
        //result = IWmsApp_SendTextMessageExt(pIWmsApp,wstrNumber,wstr);
        //IWmsApp_Release(pIWmsApp);
        result = IWmsApp_SendSpecMessage(pIWmsApp, wstrType);
        IWmsApp_Release(pIWmsApp);
    }
    
    MSG_FATAL("END CoreApp_MobileTracker==%d",result,0,0);
    return result;
   

}



void CoreApp_SmsTrackerTimer(void *pme)
{
	CCoreApp *pMe = (CCoreApp *)pme;
   
  	 if (NULL == pMe)
   	{
      	return;
   	}
   
   	// 发送EVT_DISPLAYDIALOGTIMEOUT事件
  	(void) ISHELL_PostEvent(pMe->a.m_pIShell,
                          AEECLSID_CORE_APP,
                          EVT_SALES_TRACKER,
                          0,
                          0);
}




#endif



#ifdef FEATURE_SEAMLESS_SMS
void CoreApp_SendSeamlessSMSTimer(void *pme)
{
   CCoreApp *pMe = (CCoreApp *)pme;
   
   if (NULL == pMe)
   {
      return;
   }
   
   // 发送EVT_DISPLAYDIALOGTIMEOUT事件
  (void) ISHELL_PostEvent(pMe->a.m_pIShell,
                          AEECLSID_CORE_APP,
                          EVT_SEND_SEAMLESSSMS,
                          0,
                          0);
}

int CoreApp_SendSeamlessSMS(CCoreApp   *pMe)
{
    int  result = SUCCESS;
    AEEMobileInfo     mi;
    IWmsApp *pIWmsApp = NULL;
    AECHAR  wstrType[2] = {(AECHAR)POWERUP_REGISTER_SEAMLESSSMS, 0};
    //CORE_ERR("START CoreApp_SendSeamlessSMS",0,0,0);
    
    if (pMe == NULL)
    {
        return EFAILED;
    }
    
    // 获取手机电子串号
    OEM_GetConfig (CFGI_MOBILEINFO, &mi,  sizeof(AEEMobileInfo));
    if (mi.dwESN == 0)
    {
        return EFAILED;
    }
    
    result = ISHELL_CreateInstance(pMe->a.m_pIShell,
                                 AEECLSID_WMSAPP,
                                 (void **) &pIWmsApp);
    if ((result == SUCCESS) && (NULL != pIWmsApp))
    {
        result = IWmsApp_SendSpecMessage(pIWmsApp, wstrType);
        IWmsApp_Release(pIWmsApp);
    }
    
    //CORE_ERR("END CoreApp_SendSeamlessSMS",0,0,0);
    return result;
}   

#endif

int CoreApp_GetBatteryLevel(CCoreApp *pMe)
{
    uint32  pdwData;
    uint16  uBattLevel ;
    uint16  uBattScale;
    uint8   ATBattLevel;
    uint8   uLevel;
  
    if ( IBATTERY_GetBatteryLevel( pMe->m_pBatt, &(pdwData) ) != SUCCESS )
    {
        return 0;
    }
    
    uBattLevel = GETBATTERYLEVEL(pdwData);
    uBattScale = GETBATTERYSCALE(pdwData);
    
    if ( uBattScale == 0 )
    {
        return 0;
    }
    
    if ( uBattLevel == 0 )
    {
        ATBattLevel = 0;
    }
    else
    {
        ATBattLevel = (uBattLevel * 100) / uBattScale;
    }
    ERR("miaoxiaoming: ATBattLevel =%d",ATBattLevel ,0,0);
    /* convert to 0-4 levels */
    if ( ATBattLevel <= 40 )
    {
        uLevel = 0;
    }
    else if ( ATBattLevel <= 50 )
    {
        uLevel = 1;
    }
    else if ( ATBattLevel <= 65 )
    {
        uLevel = 2;
    }
    else if ( ATBattLevel <= 80 )
    {
        uLevel = 3;
    }
    else
    {
        uLevel = 4;
    }
    
    return uLevel;
}

/*==============================================================================
函数：
       CoreApp_ConvertBattLvToAnnunState

说明：
       将电池电量等级转换为状态栏显示参数。

参数：
       nBattLevel [In]: 
       
返回值：
       uint32

备注：:
       当 bEnabled 为TRUE 时恢复警报器；否则，挂起警报器。
       
==============================================================================*/
static uint32 CoreApp_ConvertBattLvToAnnunState(int nBattLevel)
{
    uint32 nState = 0;
    
    if(nBattLevel == 0)
    {
        nState = ANNUN_STATE_BATT_LOW;
    }
    else if(nBattLevel == 1)
    {
        nState = ANNUN_STATE_BATT_1;
    }
    else if(nBattLevel == 2)
    {
        nState = ANNUN_STATE_BATT_2;
    }
    else if(nBattLevel == 3)
    {
        nState = ANNUN_STATE_BATT_3;
    }
    else
    {
        nState = ANNUN_STATE_BATT_FULL;
    }

    return nState;
}
//end added
static void CoreNotifyMP3PlayerAlertEvent(CCoreApp *pMe, boolean toStartAlert)
{
 #ifdef FEATURE_APP_MUSICPLAYER
    if(GetMp3PlayerStatus() != MP3STATUS_NONE)
    {
        ISHELL_SendEvent(pMe->a.m_pIShell,
                         AEECLSID_APP_MUSICPLAYER,
                         EVT_ALARM,
                         toStartAlert,
                         0);
    }
 #endif
}
static void CoreNotifyMP3PlayerAlertEventCB(CCoreApp *pMe)
{
    CoreNotifyMP3PlayerAlertEvent(pMe,FALSE);
}

static void CoreAppReadNVKeyBeepValue(CCoreApp *pMe)
{
    byte p_beep_level[PROFILENUMBER]; 
    byte cur_active = 0;
    byte cur_btkeyVol = 0;
    byte btKeyVol = 0;

   (void) ICONFIG_GetItem(pMe->m_pConfig,
                    CFGI_PROFILE_CUR_NUMBER,
                    &cur_active,
                    sizeof(cur_active));
   
    (void) ICONFIG_GetItem(pMe->m_pConfig,
                           CFGI_PROFILE_BEEP_VOL, 
                           &p_beep_level,
                           sizeof(p_beep_level));
    
    (void) ICONFIG_GetItem(pMe->m_pConfig,
                           CFGI_BEEP_VOL, 
                           &btKeyVol,
                           sizeof(byte));
    
    
    cur_btkeyVol = p_beep_level[cur_active];
        
    if(btKeyVol != cur_btkeyVol)
    {
         btKeyVol = cur_btkeyVol; //changed NV data  
         ICONFIG_SetItem(pMe->m_pConfig,CFGI_BEEP_VOL, &btKeyVol,sizeof(byte));
    }

}

#ifdef FEATURE_UIM_RUN_TIME_ENABLE
extern nv_stat_enum_type ui_get_nv(
  nv_items_enum_type item,        /* which item */
  nv_item_type *data_ptr          /* pointer to space for item */
);
extern nv_stat_enum_type ui_put_nv(
  nv_items_enum_type item,        /* which item */
  nv_item_type *data_ptr          /* pointer to data for item */
);

/*===========================================================================
FUNCTION GetRTREConfig

DESCRIPTION
  Get RTRE Config.

DEPENDENCIES
  SK UI

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetRTREConfig(CCoreApp *pMe)
{
  nv_item_type nvi;
  nv_stat_enum_type result;

  result = ui_get_nv (NV_RTRE_CONFIG_I, &nvi);

  if(result != NV_DONE_S) {  /* write to NV */
#ifdef FEATURE_RTRE_DEFAULT_IS_NV
    nvi.rtre_config = NV_RTRE_CONFIG_NV_ONLY;
#else
    nvi.rtre_config = NV_RTRE_CONFIG_RUIM_ONLY;
#endif
    (void) ui_put_nv (NV_RTRE_CONFIG_I, &nvi);
  }
  return (uint64)(nvi.rtre_config);
}

/*===========================================================================
FUNCTION SetRTREConfig

DESCRIPTION
  Set RTRE Config.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean SetRTREConfig(CCoreApp *pMe, uint64 nNewSetting)
{
  int retVal;
  
  if (pMe == NULL) {
    //CORE_ERR("pMe is Null", 0, 0, 0);
    return FALSE;
  }

  switch (nNewSetting)
  {
#ifdef FEATURE_UIM_RUIM_W_GSM_ACCESS
    case NV_RTRE_CONFIG_SIM_ACCESS:
#ifdef FEATURE_ICM
      retVal = ICM_SetRTREConfig(pMe->m_pCM, AEECM_RTRE_CONFIG_SIM_ACCESS);
#else
      retVal = IPHONECTL_SetRTREConfig(pMe->m_pIPhoneCtl, AEECM_RTRE_CONFIG_SIM_ACCESS);
#endif
      break;
#endif
    case NV_RTRE_CONFIG_RUIM_ONLY:
#ifdef FEATURE_ICM
      retVal = ICM_SetRTREConfig(pMe->m_pCM, AEECM_RTRE_CONFIG_RUIM_ONLY);
#else
      retVal = IPHONECTL_SetRTREConfig(pMe->m_pIPhoneCtl, AEET_RTRE_CONFIG_RUIM_ONLY);
#endif
      break;
      
    case NV_RTRE_CONFIG_NV_ONLY:
#ifdef FEATURE_ICM
      retVal = ICM_SetRTREConfig(pMe->m_pCM, AEECM_RTRE_CONFIG_NV_ONLY);
#else
      retVal = IPHONECTL_SetRTREConfig(pMe->m_pIPhoneCtl, AEET_RTRE_CONFIG_NV_ONLY);
#endif
      break;
      
    case NV_RTRE_CONFIG_RUIM_OR_DROP_BACK:
    default:
#ifdef FEATURE_ICM
      retVal = ICM_SetRTREConfig(pMe->m_pCM, AEECM_RTRE_CONFIG_RUIM_OR_DROP_BACK);
#else
      retVal = IPHONECTL_SetRTREConfig(pMe->m_pIPhoneCtl, AEET_RTRE_CONFIG_RUIM_OR_DROP_BACK);
#endif
      break;
  }
  return (retVal == AEE_SUCCESS) ? TRUE : FALSE;
}

/*=============================================================================
FUNCTION: SendRTREConfig

DESCRIPTION:
  Read config from NV and send it to CM.
  This function is only executed once at powerup.

SIDE EFFECTS:
  None.

=============================================================================*/
void SendRTREConfig (CCoreApp *pMe)
{
  uint32 val;

  if ((pMe->m_bConfigSent == FALSE)
#ifdef FEATURE_MMGSDI_DUAL_SLOT
      &&
      (IsBothSlotReady(pMe))
#endif /*FEATURE_MMGSDI_DUAL_SLOT */
     )
  {
    val = GetRTREConfig(pMe);
    pMe->m_bConfigSent = SetRTREConfig(pMe, val);
  }
}
#endif /* F_UIM_RUN_TIME_ENABLE */

/*===========================================================================
FUNCTION InitAfterPhInfo

DESCRIPTION
  Executes startup sequence after phone information is available from ICM.
  If phone info is available at Core App startup, no need to wait for
  AEECM_EVENT_PH_INFO_AVAIL event.

DEPENDENCIES
  ICM interface should have been created previously.

SIDE EFFECTS
  None
===========================================================================*/
#ifdef FEATURE_ICM
void InitAfterPhInfo(CCoreApp *pMe, AEECMOprtMode mode)
{
  if (pMe == NULL)
  {
    //CORE_ERR("Null pMe",0,0,0);
    return;
  }
#ifdef FEATURE_UIM_RUN_TIME_ENABLE
  /* Send RTRE config changed to CM - response from CM will */
  /* generate subscription changed event */
  SendRTREConfig(pMe);
#endif

  //Change modes if we're provisioned properly.
  if (mode != AEECM_OPRT_MODE_FTM)
  {
    if (ui_check_provisioning() && mode != AEECM_OPRT_MODE_OFFLINE &&
        mode != AEECM_OPRT_MODE_OFFLINE_CDMA &&
        mode != AEECM_OPRT_MODE_OFFLINE_AMPS) {
      //ICM_SetOperatingMode(pMe->m_pCM, AEECM_OPRT_MODE_ONLINE);
      ISHELL_PostEventEx (pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CORE_APP,
                          EVT_SET_OPERATING_MODE,AEECM_OPRT_MODE_ONLINE,0L);
    }
    else
    {
      ICM_SetOperatingMode(pMe->m_pCM, AEECM_OPRT_MODE_OFFLINE);
    }
  }
  else
  {
    ICM_SetOperatingMode(pMe->m_pCM, AEECM_OPRT_MODE_OFFLINE);
  }
  pMe->m_bProvisioned = TRUE;
}
#else
void InitAfterPhInfo(CCoreApp *pMe, AEETOprtMode mode)
{
  if (pMe == NULL)
  {
    //CORE_ERR("Null pMe",0,0,0);
    return;
  }
#ifdef FEATURE_UIM_RUN_TIME_ENABLE
  /* Send RTRE config changed to CM - response from CM will */
  /* generate subscription changed event */
  SendRTREConfig(pMe);
#endif

  //Change modes if we're provisioned properly.
  if (mode != AEET_OPRT_MODE_FTM)
  {  
    extern boolean ui_check_provisioning(void);
    if (ui_check_provisioning() && mode != AEET_OPRT_MODE_OFFLINE &&
        mode != AEET_OPRT_MODE_OFFLINE_CDMA &&
        mode != AEET_OPRT_MODE_OFFLINE_AMPS) {
      //ICM_SetOperatingMode(pMe->m_pCM, AEECM_OPRT_MODE_ONLINE);
      ISHELL_PostEventEx (pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CORE_APP,
                          EVT_SET_OPERATING_MODE,AEET_OPRT_MODE_ONLINE,0L);
    }
    else
    {
      IPHONECTL_SetOperatingMode(pMe->m_pIPhoneCtl, AEET_OPRT_MODE_OFFLINE);
    }
  }
  else
  {
    IPHONECTL_SetOperatingMode(pMe->m_pIPhoneCtl, AEET_OPRT_MODE_OFFLINE);
  }
  pMe->m_bProvisioned = TRUE;
}
#endif
/*=============================================================================
FUNCTION: ProcessSubscriptionStatus

DESCRIPTION:
  Look at various parameters to see the subscription status and send it to CM

SIDE EFFECTS:
  None.

=============================================================================*/
void CoreApp_ProcessSubscriptionStatus (CCoreApp *pMe)
{
    boolean bSubAvail = TRUE;
    
    /* the technology is in slot 1 */
    /* is the pin perm disabled? or blocked */
    if (pMe->m_eUIMErrCode == UIMERR_NONE || pMe->m_eUIMErrCode == UIMERR_NOUIM)
    {
        bSubAvail = TRUE;
    }
    else
    {
        bSubAvail = FALSE;
    } /* pin is not permanently disabled */
#ifdef FEATURE_ICM
    //CORE_ERR("CoreApp_ProcessSubscriptionStatus", 0, 0, 0);
    ICM_SetSubscriptionStatus(pMe->m_pCM, AEECM_SYS_MODE_CDMA, bSubAvail);
#else
    IPHONECTL_SetSubscriptionStatus(pMe->m_pIPhoneCtl, AEET_SYS_MODE_CDMA, bSubAvail);
#endif
}

/*=============================================================================
FUNCTION: CoreApp_ProcessOffLine

DESCRIPTION:
  Display the corresponding message for offline mode

SIDE EFFECTS:
  None.

=============================================================================*/
boolean CoreApp_ProcessOffLine(CCoreApp *pMe)
{
  if (pMe == NULL)
  {
     //CORE_ERR("Null pMe Ptr", 0, 0, 0);
     return FALSE;
  }
  //CORE_ERR("CoreApp_ProcessOffLine", 0, 0, 0);
  return TRUE;

}

/*=============================================================================
FUNCTION: CoreApp_ProcessFTMMode

DESCRIPTION:
  Display the corresponding message for FTM mode

SIDE EFFECTS:
  None.

=============================================================================*/
static boolean CoreApp_ProcessFTMMode(CCoreApp *pMe)
{
#if defined (FEATURE_APP_FLDDBG)
    // Launch the debug screen of the field debug app.
    if((ISHELL_StartAppletArgs(pMe->a.m_pIShell, AEECLSID_FIELDDEBUGAPP, "FTM")) != AEE_SUCCESS) {
        //CORE_ERR("Could not start FldDbg App", 0, 0, 0);
    }
#endif
    //CORE_ERR("CoreApp_ProcessFTMMode", 0, 0, 0);
    return TRUE;
}

#ifdef USES_CONSTEFS
#define COREAPP_REBUILDEFS_BUFSIZE  1024
#define COREAPP_REBUILDEFS_VER      USES_CONSTEFS "/" PROJECTNAME ".ver"

static void CoreApp_RebuildDirFiles(IFileMgr *pFileMgr, const char * pszDir, byte *pBuff)
{
    // 枚举当前的文件
    if(SUCCESS == IFILEMGR_EnumInit(pFileMgr,pszDir,FALSE))
    {
        AEEFileInfo myInfo;
        char        szName[AEE_MAX_FILE_NAME+1];
        IFile      *pFileSrc;
        IFile      *pFileDst;
        int         nCount;
        
        while(TRUE == IFILEMGR_EnumNext(pFileMgr, &myInfo))
        {
            STRCPY(szName,AEEFS_ROOT_DIR);
            STRCAT(szName,&(myInfo.szName[sizeof(USES_CONSTEFS)]));
            //if(SUCCESS != IFILEMGR_Test(pFileMgr, szName))
            {
                pFileSrc = IFILEMGR_OpenFile(pFileMgr,myInfo.szName,_OFM_READ);
                pFileDst = IFILEMGR_OpenFile(pFileMgr,szName,_OFM_CREATE);
                if(NULL != pFileSrc && NULL != pFileDst)
                {
                    while(1)
                    {
                        nCount = IFILE_Read(pFileSrc,pBuff,COREAPP_REBUILDEFS_BUFSIZE);
                        if(nCount> 0)
                        {
                            IFILE_Write(pFileDst,pBuff,nCount);
                        }
                        else
                        {
                            break;
                        }
                    }
                }
                RELEASEIF(pFileSrc);
                RELEASEIF(pFileDst);
            }
        }
    }
}

static void CoreApp_RebuildDir(CCoreApp *pMe, const char * pszDir, byte *pBuff)
{
    int       nRet;
    IFileMgr *pFileMgr  = NULL;

    nRet = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR, (void **) &pFileMgr);
    if (nRet != SUCCESS) 
    {
        goto EXIT_FUN;
    }
    
    CoreApp_RebuildDirFiles(pFileMgr, pszDir, pBuff);
    
    // 枚举当前的文件夹
    if(SUCCESS == IFILEMGR_EnumInit(pFileMgr,pszDir,TRUE))
    {
        AEEFileInfo myInfo;
        
        while(TRUE == IFILEMGR_EnumNext(pFileMgr, &myInfo))
        {
            CoreApp_RebuildDir(pMe, myInfo.szName, pBuff);
        }
    }
    
EXIT_FUN:
    RELEASEIF(pFileMgr);
}

static void CoreApp_RebuildEFS(CCoreApp *pMe)
{
    int       nRet;
    IFileMgr *pFileMgr  = NULL;
    IFile    *pFile     = NULL;
    byte     *pBuff     = NULL;
    
    nRet = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR, (void **) &pFileMgr);
    if (nRet != SUCCESS) 
    {
        goto EXIT_FUN;
    }
    
    // 已经存在对应版本号的文件，则无需重建EFS
    if(SUCCESS == IFILEMGR_Test(pFileMgr,COREAPP_REBUILDEFS_VER))
    {
        goto EXIT_FUN;
    }
    
    pBuff = (byte *)MALLOC(COREAPP_REBUILDEFS_BUFSIZE);
    if(!pBuff)
    {
        goto EXIT_FUN;
    }
    
    // 递归枚举 USES_CONSTEFS 文件夹下面的文件,并将其复制到 USES_CONSTEFS 替换为 AEEFS_ROOT_DIR 的文件夹下面
    CoreApp_RebuildDir(pMe, USES_CONSTEFS, pBuff);
    
    // EFS重建完成，创建 COREAPP_REBUILDEFS_VER 文件
    pFile = IFILEMGR_OpenFile(pFileMgr, COREAPP_REBUILDEFS_VER,_OFM_CREATE);
EXIT_FUN:
    FREEIF(pBuff);
    RELEASEIF(pFile);
    RELEASEIF(pFileMgr);
}
#endif

/*===========================================================================
FUNCTION StereoHeadsetOn

DESCRIPTION
  Upon receiving stereo headset on event, this function updates the
  corresponding info.

DEPENDENCIES


SIDE EFFECTS
  None
===========================================================================*/
static void StereoHeadsetOn(CCoreApp * pMe)
{
	boolean stereoHeadSetOn = TRUE;
	int     nRetVal;
	AEEDeviceNotify devnotify;

	byte			return_ringer_level[PROFILENUMBER];
	byte			return_beep_level[PROFILENUMBER];
	byte			set_ringer_level;
	byte			set_beep_level;
	byte			m_CallVolume;
	
	if ((pMe == NULL) || (pMe->m_pConfig == NULL))
	{
		MSG_ERROR("NULL pointer, pMe=0x%x", pMe, 0, 0);
		return;
	}
	if(pMe->m_pIAnn)
	{
		IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_FMRADIO/*ANNUN_FIELD_HEADSET*/, ANNUN_STATE_HEADSET_ON/*ANNUN_STATE_ON*/);
	}

	ICONFIG_SetItem(pMe->m_pConfig, CFGI_HEADSET_PRESENT, &stereoHeadSetOn, 1);

	/*Also set the headset in ICONFIG because OEMCall will use it if WCDMA is defined*/
	nRetVal = ICONFIG_SetItem(pMe->m_pConfig, CFGI_STEREO_HEADSET, &stereoHeadSetOn, 1);

	if (nRetVal != SUCCESS)
	{
		MSG_HIGH("Failed to set config item, %d", nRetVal, 0, 0);
	}

    /* Change the audio path */
    uisnd_set_device_status(SND_DEVICE_STEREO_HEADSET, UISND_DEV_ENABLED);
    uisnd_set_device_auto(NULL,NULL);
    
    //wangliang modify!  2010-09-25
	(void) ICONFIG_GetItem(pMe->m_pConfig,
	                    CFGI_PROFILE_RINGER_VOL,
	                    return_ringer_level,
	                    sizeof(return_ringer_level));


	(void) ICONFIG_GetItem(pMe->m_pConfig,
	                    CFGI_PROFILE_BEEP_VOL,
	                    return_beep_level,
	                    sizeof(return_beep_level));

	(void) ICONFIG_GetItem(pMe->m_pConfig,CFGI_EAR_VOL,&m_CallVolume,sizeof(byte));
	
	set_ringer_level            =   return_ringer_level[pMe->m_CurProfile];
	set_beep_level              =   return_beep_level[pMe->m_CurProfile];

	snd_set_volume( SND_DEVICE_STEREO_HEADSET, SND_METHOD_KEY_BEEP,set_beep_level, NULL, NULL );
	snd_set_volume( SND_DEVICE_STEREO_HEADSET, SND_METHOD_VOICE,m_CallVolume, NULL, NULL );		

	snd_set_volume( SND_DEVICE_STEREO_HEADSET, SND_METHOD_MESSAGE,set_ringer_level, NULL, NULL );	
	snd_set_volume( SND_DEVICE_STEREO_HEADSET, SND_METHOD_RING,set_ringer_level, NULL, NULL );	
	
#ifdef FEATURE_MULTIMEDIA
#ifndef FEATURE_USES_LOWMEM
	if(!IsMp3PlayerStatusNone())  //add by miaoxiaoming
	{
		ISHELL_PostEvent(pMe->a.m_pIShell, AEECLSID_APP_MUSICPLAYER,EVT_HEADSET_CONNECT, 0, 0);
	}
#endif
#endif   



	devnotify.wParam = TRUE;
	AEE_SEND_HEADSET_EVT(&devnotify);
} /* End HeadsetOn */


/*===========================================================================
FUNCTION HeadsetOff

DESCRIPTION
  Upon receiving headset off event, this function updates the
  corresponding info.

DEPENDENCIES


SIDE EFFECTS
  None
===========================================================================*/
static void HeadsetOff(CCoreApp *pMe)
{
	boolean headSetOn = FALSE;
	int     nRetVal;
	AEEDeviceNotify devnotify;
	byte		return_ringer_level[PROFILENUMBER];
	byte	    return_beep_level[PROFILENUMBER];
	byte		set_ringer_level;
	byte		set_beep_level;
	byte        m_CallVolume;
	
	if ((pMe == NULL) || (pMe->m_pConfig == NULL))
	{
		MSG_ERROR("NULL pointer, pMe=0x%x", pMe, 0, 0);
		return;
	}

	if(pMe->m_pIAnn)
	{
		IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_FMRADIO/*ANNUN_FIELD_HEADSET*/, ANNUN_STATE_HEADSET_OFF/*ANNUN_STATE_ON*/);
	}

	ICONFIG_SetItem(pMe->m_pConfig, CFGI_HEADSET_PRESENT, &headSetOn, 1);

	devnotify.wParam = FALSE;
	AEE_SEND_HEADSET_EVT(&devnotify);
    
	/* Change the audio path */
	//uisnd_set_device_status(SND_DEVICE_STEREO_HEADSET, UISND_DEV_UNATTACHED);
	//uisnd_set_device_status(SND_DEVICE_HEADSET, UISND_DEV_UNATTACHED);

	/*Also set the headset in ICONFIG because OEMCall will use it if WCDMA is defined*/
	nRetVal = ICONFIG_SetItem(pMe->m_pConfig, CFGI_HEADSET, &headSetOn, 1);
	if (nRetVal != SUCCESS)
	{
		MSG_HIGH("Failed to set config item, %d", nRetVal, 0, 0);
	}

	nRetVal = ICONFIG_SetItem(pMe->m_pConfig, CFGI_STEREO_HEADSET, &headSetOn, 1);
	if (nRetVal != SUCCESS)
	{
		MSG_HIGH("Failed to set config item, %d", nRetVal, 0, 0);
	}
	
    uisnd_set_device_status(SND_DEVICE_STEREO_HEADSET, UISND_DEV_UNATTACHED);
    uisnd_set_device_status(SND_DEVICE_HEADSET, UISND_DEV_UNATTACHED);	
#ifdef FEATURE_FM_OEM
    uisnd_set_device_status(SND_DEVICE_HEADSET_FM, UISND_DEV_UNATTACHED);
    uisnd_set_device_status(SND_DEVICE_SPEAKER_FM, UISND_DEV_UNATTACHED);

#endif
    uisnd_set_device_auto(NULL,NULL);

//wangliang modify!  2010-09-25
	(void) ICONFIG_GetItem(pMe->m_pConfig,
	                    CFGI_PROFILE_RINGER_VOL,
	                    return_ringer_level,
	                    sizeof(return_ringer_level));

	(void) ICONFIG_GetItem(pMe->m_pConfig,
	                    CFGI_PROFILE_BEEP_VOL,
	                    return_beep_level,
	                    sizeof(return_beep_level));

	(void) ICONFIG_GetItem(pMe->m_pConfig,CFGI_EAR_VOL,&m_CallVolume,sizeof(byte));
                                                
	set_ringer_level            =   return_ringer_level[pMe->m_CurProfile];
	set_beep_level              =   return_beep_level[pMe->m_CurProfile];

	snd_set_volume( SND_DEVICE_HANDSET, SND_METHOD_KEY_BEEP,set_beep_level, NULL, NULL );
	snd_set_volume( SND_DEVICE_HANDSET, SND_METHOD_VOICE,m_CallVolume, NULL, NULL );		

	snd_set_volume( SND_DEVICE_HANDSET, SND_METHOD_MESSAGE,set_ringer_level, NULL, NULL );	
	snd_set_volume( SND_DEVICE_HANDSET, SND_METHOD_RING,set_ringer_level, NULL, NULL );	
	
#ifdef FEATURE_MULTIMEDIA
	//if(!IsMp3PlayerStatusNone())  //add by miaoxiaoming
	//{
	//	ISHELL_PostEvent(pMe->a.m_pIShell, AEECLSID_APP_MUSICPLAYER,EVT_HEADSET_CONNECT, 0, 0);
	//}
#endif

	devnotify.wParam = FALSE;
	AEE_SEND_HEADSET_EVT(&devnotify);
} /*End HeadsetOff */


static void CoreApp_SceneMode(CCoreApp *pMe)
{
	byte	return_alert_type[PROFILENUMBER],
		    return_missed_call_alert[PROFILENUMBER],
		    //return_headset_autoanswer[PROFILENUMBER],
		    return_keysnd_type[PROFILENUMBER],
		    /*return_keytone_length[PROFILENUMBER],*/
		    return_ringer_level[PROFILENUMBER],
		    return_handset_ear_level[PROFILENUMBER],
		    return_beep_level[PROFILENUMBER], //;
		    return_power_onoff[PROFILENUMBER];
	uint16 	return_start_music[PROFILENUMBER];
	uint16 	return_shutdown_music[PROFILENUMBER];

	ringID  return_call_ringer[PROFILENUMBER],
	    	return_alarm_ringer[PROFILENUMBER];

	byte	set_alert_type,
			set_missed_call_alert,
			//set_headset_autoanswer,
			set_keysnd_type,
			/*set_keytone_length,*/
			set_ringer_level,
			set_handset_ear_level,
			set_beep_level, //;
			set_power_onoff;

	ringID  set_call_ringer;
	ringID  set_alarm_ringer; 
	uint16  set_start_music,
	    	set_shutdown_music;
#if defined(FEATURE_WMS_APP) && !defined(FEATURE_WMSAPP_ONLYSUPPORTVMAIL)
	byte    set_bt_sms_ringer, return_bt_sms_ringer[PROFILENUMBER];
	ringID  set_sms_ringer_id, return_sms_ringer_id[PROFILENUMBER];
#endif

    (void) ICONFIG_GetItem(pMe->m_pConfig,
                           CFGI_PROFILE_ALERT_TYPE,
                           return_alert_type,
                           sizeof(return_alert_type));

#if defined(FEATURE_WMS_APP) && !defined(FEATURE_WMSAPP_ONLYSUPPORTVMAIL)
    (void) ICONFIG_GetItem(pMe->m_pConfig,
                           CFGI_PROFILE_SMS_RINGER,
                           return_bt_sms_ringer,
                           sizeof(return_bt_sms_ringer));
#endif

	(void) ICONFIG_GetItem(pMe->m_pConfig,
	                   CFGI_PROFILE_CALL_RINGER,
	                   (void*)return_call_ringer,
	                   sizeof(return_call_ringer));

	(void) ICONFIG_GetItem(pMe->m_pConfig,
	                    CFGI_PROFILE_ALARM_RINGER,
	                    (void*)return_alarm_ringer,
	                    sizeof(return_alarm_ringer));

#if defined(FEATURE_WMS_APP) && !defined(FEATURE_WMSAPP_ONLYSUPPORTVMAIL)
	(void) ICONFIG_GetItem(pMe->m_pConfig,
	                    CFGI_PROFILE_SMS_RINGER_ID,
	                    (void*)return_sms_ringer_id,
	                    sizeof(return_sms_ringer_id));
#endif

	(void) ICONFIG_GetItem(pMe->m_pConfig,
	                    CFGI_PROFILE_MISSED_CALL_ALERT,
	                    return_missed_call_alert,
	                    sizeof(return_missed_call_alert));

	(void) ICONFIG_GetItem(pMe->m_pConfig,
	                    CFGI_PROFILE_STARTUP_MUSIC,
	                    return_start_music,
	                    sizeof(return_start_music));

	(void) ICONFIG_GetItem(pMe->m_pConfig,
	                    CFGI_PROFILE_SHUTDOWN_MUSIC,
	                    return_shutdown_music,
	                    sizeof(return_shutdown_music));

	(void) ICONFIG_GetItem(pMe->m_pConfig,
	                    CFGI_PROFILE_POWERONOFF_ALERT,
	                    return_power_onoff,
	                    sizeof(return_power_onoff));

	(void) ICONFIG_GetItem(pMe->m_pConfig,
	                    CFGI_PROFILE_KEYSND_TYPE,
	                    return_keysnd_type,
	                    sizeof(return_keysnd_type));

	(void) ICONFIG_GetItem(pMe->m_pConfig,
	                    CFGI_PROFILE_RINGER_VOL,
	                    return_ringer_level,
	                    sizeof(return_ringer_level));

	(void) ICONFIG_GetItem(pMe->m_pConfig,
	                    CFGI_PROFILE_EAR_VOL,
	                    return_handset_ear_level,
	                    sizeof(return_handset_ear_level));

	(void) ICONFIG_GetItem(pMe->m_pConfig,
	                    CFGI_PROFILE_BEEP_VOL,
	                    return_beep_level,
	                    sizeof(return_beep_level));

	set_alert_type              =   return_alert_type[pMe->m_CurProfile];
	set_call_ringer             =   return_call_ringer[pMe->m_CurProfile];
	set_alarm_ringer            =   return_alarm_ringer[pMe->m_CurProfile];
#if defined(FEATURE_WMS_APP) && !defined(FEATURE_WMSAPP_ONLYSUPPORTVMAIL)
	set_bt_sms_ringer           =   return_bt_sms_ringer[pMe->m_CurProfile];
	set_sms_ringer_id           =   return_sms_ringer_id[pMe->m_CurProfile];
#endif          
	set_missed_call_alert       =   return_missed_call_alert[pMe->m_CurProfile];
	set_power_onoff             =   return_power_onoff[pMe->m_CurProfile];
	set_start_music             =   return_start_music[pMe->m_CurProfile];
	set_shutdown_music          =   return_shutdown_music[pMe->m_CurProfile];
	//set_headset_autoanswer      =   return_headset_autoanswer[pMe->m_CurProfile];
	set_keysnd_type             =   return_keysnd_type[pMe->m_CurProfile];
	//set_keytone_length          =   return_keytone_length[pMe->m_CurProfile];
	set_ringer_level            =   return_ringer_level[pMe->m_CurProfile];
	set_handset_ear_level       =   return_handset_ear_level[pMe->m_CurProfile];
	set_beep_level              =   return_beep_level[pMe->m_CurProfile];


	(void) ICONFIG_SetItem(pMe->m_pConfig,
	                  CFGI_PROFILE_CUR_NUMBER,
	                  &pMe->m_CurProfile,
	                  sizeof(byte));

	(void)ICONFIG_SetItem(pMe->m_pConfig,
	                  CFGI_ALERT_TYPE,
	                  &set_alert_type,
	                  sizeof(set_alert_type));
	                  
#if defined(FEATURE_WMS_APP) && !defined(FEATURE_WMSAPP_ONLYSUPPORTVMAIL)
	(void)ICONFIG_SetItem(pMe->m_pConfig,
	                  CFGI_SMS_RINGER,
	                  &set_bt_sms_ringer,
	                  sizeof(set_bt_sms_ringer));
#endif
	{
		uint16 alarm_id = 0;
		alarm_id = set_alarm_ringer.midID;

		(void)ICONFIG_SetItem(pMe->m_pConfig,
		                      CFGI_ALARM_RINGER,
		                      &alarm_id,
		                      sizeof(alarm_id));
	}

#if defined(FEATURE_WMS_APP) && !defined(FEATURE_WMSAPP_ONLYSUPPORTVMAIL)
	{
		uint16 sms_id = 0;
		sms_id = set_sms_ringer_id.midID;

		(void)ICONFIG_SetItem(pMe->m_pConfig,
		                      CFGI_SMS_RINGER_ID,
		                      &sms_id,
		                      sizeof(sms_id));
	}
#endif

	(void)ICONFIG_SetItem(pMe->m_pConfig,
	                  CFGI_MISSED_CALL_ALERT,
	                  &set_missed_call_alert,
	                  sizeof(set_missed_call_alert));

	(void) ICONFIG_SetItem(pMe->m_pConfig,
	                   CFGI_PROFILE_POWERONOFF_ALERT,
	                   &set_power_onoff,
	                   sizeof(set_power_onoff));

	(void) ICONFIG_SetItem(pMe->m_pConfig,
	                   CFGI_PROFILE_POWERONOFF_ALERT,
	                   &set_start_music,
	                   sizeof(set_start_music));

	(void) ICONFIG_SetItem(pMe->m_pConfig,
	                   CFGI_PROFILE_POWERONOFF_ALERT,
	                   &set_shutdown_music,
	                   sizeof(set_shutdown_music));

	(void)ICONFIG_SetItem(pMe->m_pConfig,
	                  CFGI_KEYBEEP_SOUND,
	                  &set_keysnd_type,
	                  sizeof(set_keysnd_type));


	(void)ICONFIG_SetItem(pMe->m_pConfig,
	                  CFGI_RINGER_VOL,
	                  &set_ringer_level,
	                  sizeof(set_ringer_level));

	(void)ICONFIG_SetItem(pMe->m_pConfig,
	                  CFGI_BEEP_VOL,
	                  &set_beep_level,
	                  sizeof(set_beep_level));

	(void)ICONFIG_GetItem(pMe->m_pConfig,
	                  CFGI_PROFILE_CUR_NUMBER,
	                  &pMe->m_active,
	                  sizeof(pMe->m_active));

		
	switch(pMe->m_active)
	{
		case OEMNV_PROFILE_NORMALMODE:          //正常模式
		  IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_RINGTONE, ANNUN_STATE_RINGTONE_ALERT);
		  break;

		case OEMNV_PROFILE_QUIETMODE:           //安静模式
		  IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_RINGTONE, ANNUN_STATE_RINGTONE_SILENT);
		  break;

		case OEMNV_PROFILE_MEETING:             //会议模式
		  IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_RINGTONE, ANNUN_STATE_RINGTONE_VIBRATOR);
		  break;

		case OEMNV_PROFILE_NOISEMODE:           //户外模式
		  IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_RINGTONE, ANNUN_STATE_RINGTONE_VIBRING);
		  break;

		case OEMNV_PROFILE_CARMODE:             //车载模式
		  IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_RINGTONE, ANNUN_STATE_RINGTONE_VIBRING);
		  break;

		default:
		  break;
	}
      
#ifdef FEATURE_APP_MUSICPLAYER
	if(GetMp3PlayerStatus() == MP3STATUS_RUNONBACKGROUND)
    {
        ISHELL_SendEvent(pMe->a.m_pIShell,
                         AEECLSID_APP_MUSICPLAYER,
                         EVT_ALARM,
                         FALSE,
                         TRUE);
    }
#endif
}

// 开机检查一次即可
static void CoreApp_ResetProfileTimer(void *pUser)
{
    CCoreApp *pMe = (CCoreApp *)pUser;
	boolean Is_on =FALSE;
	
    OEM_GetConfig(CFGI_FLSHLITHG_STATUS,&Is_on, sizeof(Is_on));
    if(!CoreApp_TestCard(pMe))
	{
		CoreApp_ResetRing(pMe);
	}
}

//add by yangdecai  2010-09-10
static boolean CoreApp_TestCard(CCoreApp *pMe)
{
	boolean Result = FALSE;
	IFileMgr          *m_pFileMgr;
	if( SUCCESS != ISHELL_CreateInstance(pMe->a.m_pIShell,
            AEECLSID_FILEMGR,
            (void**)&m_pFileMgr))
	{
		return EFAILED;
	}
	
	if(SUCCESS == IFILEMGR_Test(m_pFileMgr, AEEFS_CARD0_DIR))
	{
		Result = TRUE;
	}
	else
	{
		Result = FALSE;
	}
	RELEASEIF(m_pFileMgr);
	return Result;
}
static void    CoreApp_ResetRing(CCoreApp *pMe)
{
	ringID nNewSmsConfigRinger[PROFILENUMBER];
	ringID nNewCallConfigRinger[PROFILENUMBER];
	ringID nNewAlarmConfigRinger[PROFILENUMBER];
	boolean Relsut = FALSE;
	
	ICONFIG_GetItem(pMe->m_pConfig,CFGI_PROFILE_CALL_RINGER,(void*)nNewCallConfigRinger,sizeof(nNewCallConfigRinger));
	ICONFIG_GetItem(pMe->m_pConfig,CFGI_PROFILE_SMS_RINGER_ID,(void*)nNewSmsConfigRinger,sizeof(nNewSmsConfigRinger));
	ICONFIG_GetItem(pMe->m_pConfig,CFGI_PROFILE_ALARM_RINGER,(void*)nNewAlarmConfigRinger,sizeof(nNewAlarmConfigRinger));

	//sms

	if(nNewSmsConfigRinger[OEMNV_PROFILE_NORMALMODE].ringType == OEMNV_MP3_RINGER)
	{
        if(STRNICMP(AEEFS_CARD0_DIR,nNewSmsConfigRinger[OEMNV_PROFILE_NORMALMODE].szMusicname,STRLEN(AEEFS_CARD0_DIR)) == 0)
        {
    		nNewSmsConfigRinger[OEMNV_PROFILE_NORMALMODE].ringType = OEMNV_MID_RINGER;
    		nNewSmsConfigRinger[OEMNV_PROFILE_NORMALMODE].midID =OEMNV_SMS_RINGER_ID;
    		Relsut = ICONFIG_SetItem(pMe->m_pConfig, CFGI_PROFILE_SMS_RINGER_ID,(void*)nNewSmsConfigRinger,sizeof(nNewSmsConfigRinger));
        }
	}
	if(nNewSmsConfigRinger[OEMNV_PROFILE_QUIETMODE].ringType == OEMNV_MP3_RINGER)
   	{
        if(STRNICMP(AEEFS_CARD0_DIR,nNewSmsConfigRinger[OEMNV_PROFILE_QUIETMODE].szMusicname,STRLEN(AEEFS_CARD0_DIR)) == 0)
        {
       		nNewSmsConfigRinger[OEMNV_PROFILE_QUIETMODE].ringType = OEMNV_MID_RINGER;
    		nNewSmsConfigRinger[OEMNV_PROFILE_QUIETMODE].midID =OEMNV_SMS_RINGER_ID;
    		ICONFIG_SetItem(pMe->m_pConfig, CFGI_PROFILE_SMS_RINGER_ID,(void*)nNewSmsConfigRinger,sizeof(nNewSmsConfigRinger));
        }
   	}
	   
    if(nNewSmsConfigRinger[OEMNV_PROFILE_MEETING].ringType == OEMNV_MP3_RINGER)
   	{
        if(STRNICMP(AEEFS_CARD0_DIR,nNewSmsConfigRinger[OEMNV_PROFILE_MEETING].szMusicname,STRLEN(AEEFS_CARD0_DIR)) == 0)
        {
       		nNewSmsConfigRinger[OEMNV_PROFILE_MEETING].ringType = OEMNV_MID_RINGER;
    		nNewSmsConfigRinger[OEMNV_PROFILE_MEETING].midID =OEMNV_SMS_RINGER_ID;
    		ICONFIG_SetItem(pMe->m_pConfig, CFGI_PROFILE_SMS_RINGER_ID,(void*)nNewSmsConfigRinger,sizeof(nNewSmsConfigRinger));
        }
   	}
	if(  nNewSmsConfigRinger[OEMNV_PROFILE_NOISEMODE].ringType == OEMNV_MP3_RINGER)
	{
        if(STRNICMP(AEEFS_CARD0_DIR,nNewSmsConfigRinger[OEMNV_PROFILE_NOISEMODE].szMusicname,STRLEN(AEEFS_CARD0_DIR)) == 0)
        {
    		nNewSmsConfigRinger[OEMNV_PROFILE_NOISEMODE].ringType = OEMNV_MID_RINGER;
    		nNewSmsConfigRinger[OEMNV_PROFILE_NOISEMODE].midID =OEMNV_SMS_RINGER_ID;
    		ICONFIG_SetItem(pMe->m_pConfig, CFGI_PROFILE_SMS_RINGER_ID,(void*)nNewSmsConfigRinger,sizeof(nNewSmsConfigRinger));
        }
	}
	
	//CALL
	if(nNewCallConfigRinger[OEMNV_PROFILE_NORMALMODE].ringType == OEMNV_MP3_RINGER)
	{
        if(STRNICMP(AEEFS_CARD0_DIR,nNewCallConfigRinger[OEMNV_PROFILE_NORMALMODE].szMusicname,STRLEN(AEEFS_CARD0_DIR)) == 0)
        {
    		nNewCallConfigRinger[OEMNV_PROFILE_NORMALMODE].ringType = OEMNV_MID_RINGER;
    		nNewCallConfigRinger[OEMNV_PROFILE_NORMALMODE].midID =OEMNV_DEFAULTRINGER;
    		ICONFIG_SetItem(pMe->m_pConfig, CFGI_PROFILE_CALL_RINGER,(void*)nNewCallConfigRinger,sizeof(nNewCallConfigRinger));
        }
	}
	if(nNewCallConfigRinger[OEMNV_PROFILE_QUIETMODE].ringType == OEMNV_MP3_RINGER)
   	{
        if(STRNICMP(AEEFS_CARD0_DIR,nNewCallConfigRinger[OEMNV_PROFILE_QUIETMODE].szMusicname,STRLEN(AEEFS_CARD0_DIR)) == 0)
        {
       		nNewCallConfigRinger[OEMNV_PROFILE_QUIETMODE].ringType = OEMNV_MID_RINGER;
    		nNewCallConfigRinger[OEMNV_PROFILE_QUIETMODE].midID =OEMNV_DEFAULTRINGER;
    		ICONFIG_SetItem(pMe->m_pConfig, CFGI_PROFILE_CALL_RINGER,(void*)nNewCallConfigRinger,sizeof(nNewCallConfigRinger));
        }
   	}
	   
    if(nNewCallConfigRinger[OEMNV_PROFILE_MEETING].ringType == OEMNV_MP3_RINGER)
   	{
        if(STRNICMP(AEEFS_CARD0_DIR,nNewCallConfigRinger[OEMNV_PROFILE_MEETING].szMusicname,STRLEN(AEEFS_CARD0_DIR)) == 0)
        {
       		nNewCallConfigRinger[OEMNV_PROFILE_MEETING].ringType = OEMNV_MID_RINGER;
    		nNewCallConfigRinger[OEMNV_PROFILE_MEETING].midID =OEMNV_DEFAULTRINGER;
    		ICONFIG_SetItem(pMe->m_pConfig, CFGI_PROFILE_CALL_RINGER,(void*)nNewCallConfigRinger,sizeof(nNewCallConfigRinger));
        }
   	}
	if(  nNewCallConfigRinger[OEMNV_PROFILE_NOISEMODE].ringType == OEMNV_MP3_RINGER)
	{
        if(STRNICMP(AEEFS_CARD0_DIR,nNewCallConfigRinger[OEMNV_PROFILE_NOISEMODE].szMusicname,STRLEN(AEEFS_CARD0_DIR)) == 0)
        {
    		nNewCallConfigRinger[OEMNV_PROFILE_NOISEMODE].ringType = OEMNV_MID_RINGER;
    		nNewCallConfigRinger[OEMNV_PROFILE_NOISEMODE].midID =OEMNV_DEFAULTRINGER;
    		ICONFIG_SetItem(pMe->m_pConfig, CFGI_PROFILE_CALL_RINGER,(void*)nNewCallConfigRinger,sizeof(nNewCallConfigRinger));
        }
	}


	//ALASRM
	if(nNewAlarmConfigRinger[OEMNV_PROFILE_NORMALMODE].ringType == OEMNV_MP3_RINGER)
	{
	        if(STRNICMP(AEEFS_CARD0_DIR,nNewAlarmConfigRinger[OEMNV_PROFILE_NORMALMODE].szMusicname,STRLEN(AEEFS_CARD0_DIR)) == 0)
	        {
	    		nNewAlarmConfigRinger[OEMNV_PROFILE_NORMALMODE].ringType = OEMNV_MID_RINGER;
	    		nNewAlarmConfigRinger[OEMNV_PROFILE_NORMALMODE].midID = OEMNV_ALARM_RINGER;
	    		ICONFIG_SetItem(pMe->m_pConfig, CFGI_PROFILE_ALARM_RINGER,(void*)nNewAlarmConfigRinger,sizeof(nNewAlarmConfigRinger));
	        }
	}
	if(nNewAlarmConfigRinger[OEMNV_PROFILE_QUIETMODE].ringType == OEMNV_MP3_RINGER)
   	{
	        if(STRNICMP(AEEFS_CARD0_DIR,nNewAlarmConfigRinger[OEMNV_PROFILE_QUIETMODE].szMusicname,STRLEN(AEEFS_CARD0_DIR)) == 0)
	        {
	       		nNewAlarmConfigRinger[OEMNV_PROFILE_QUIETMODE].ringType = OEMNV_MID_RINGER;
	    		nNewAlarmConfigRinger[OEMNV_PROFILE_QUIETMODE].midID = OEMNV_ALARM_RINGER;
	    		ICONFIG_SetItem(pMe->m_pConfig, CFGI_PROFILE_ALARM_RINGER,(void*)nNewAlarmConfigRinger,sizeof(nNewAlarmConfigRinger));
	        }
   	}
	   
    if(nNewAlarmConfigRinger[OEMNV_PROFILE_MEETING].ringType == OEMNV_MP3_RINGER)
   	{
        if(STRNICMP(AEEFS_CARD0_DIR,nNewAlarmConfigRinger[OEMNV_PROFILE_MEETING].szMusicname,STRLEN(AEEFS_CARD0_DIR)) == 0)
        {
       		nNewAlarmConfigRinger[OEMNV_PROFILE_MEETING].ringType = OEMNV_MID_RINGER;
    		nNewAlarmConfigRinger[OEMNV_PROFILE_MEETING].midID = OEMNV_ALARM_RINGER;
    		ICONFIG_SetItem(pMe->m_pConfig, CFGI_PROFILE_ALARM_RINGER,(void*)nNewAlarmConfigRinger,sizeof(nNewAlarmConfigRinger));
        }
   	}
	if(  nNewAlarmConfigRinger[OEMNV_PROFILE_NOISEMODE].ringType == OEMNV_MP3_RINGER)
	{
        if(STRNICMP(AEEFS_CARD0_DIR,nNewAlarmConfigRinger[OEMNV_PROFILE_NOISEMODE].szMusicname,STRLEN(AEEFS_CARD0_DIR)) == 0)
        {
    		nNewAlarmConfigRinger[OEMNV_PROFILE_NOISEMODE].ringType = OEMNV_MID_RINGER;
    		nNewAlarmConfigRinger[OEMNV_PROFILE_NOISEMODE].midID = OEMNV_ALARM_RINGER;
    		ICONFIG_SetItem(pMe->m_pConfig, CFGI_PROFILE_ALARM_RINGER,(void*)nNewAlarmConfigRinger,sizeof(nNewAlarmConfigRinger));
        }
	}
}
//add by yangdecai end


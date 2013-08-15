/*==============================================================================
// 文件：
//        AppTimer.c
//        2007-10-31 草拟版本(Draft Version)
//        版权所有(c) 2008 Anylook
//
// 文件说明：
//
// 创建日期：2008-07-17
// 当前版本：Draft
==============================================================================*/


/*==============================================================================
修改历史记录：
when         who            what, where, why
----------   ---            ---------------------------------------------------
2008-07-15	 	modify draft code.
2008-07-8   raojin.wu   
=============================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE

===========================================================================*/
#ifndef AEE_SIMULATOR
#include "customer.h"         //Handset Feature Definitions
#else
#include "BREWSimFeatures.h"  // Simulator Feature Definitions
#endif

#include "AEEAlert.h"
#include "AEEShell.h"
#include "AEEAppGen.h"
#include "AEEStdLib.h"
#include "AEEHeap.h"
#include "AEEMenu.h"
#include "AEETime.h"
#include "AEEModTable.h"
#include "AEEdisp.h"
#include "AppComFunc.h"
#include "OEMClassIDs.h"
#include "Appscommon.h"
#include "OEMCFGI.h"
#include "Oemnvint.h"
#ifdef FEATURE_ICM
#include "AEECM.h"
#else
#include "AEETelephone.h"
#include "AEETelDef.h"
#endif
#ifdef FEATURE_APP_MUSICPLAYER
#include "MusicPlayer.h"
#endif 
#include "AEEAnnunciator.h"

#include "apptimerres.brh"
#include "appscommonimages.brh"

#if defined( AEE_SIMULATOR)
    #define  AEE_APPTIMER_RES_FILE "fs:/mod/apptimer/en/apptimerres.bar"
#else
    #define  AEE_APPTIMER_RES_FILE (AEE_RES_LANGDIR APPTIMERRES_RES_FILE)
#endif

#define MSSEC_PER_HOUR         (60*60*1000)  //一小时内的毫秒数
#define MSSEC_PER_MIN           (60*1000)     //一分钟内的毫秒数

/*===========================================================================

                    MACRO DEFINITIONS

===========================================================================*/
//#define  AEE_APPTIMER_RES_FILE (AEE_RES_LANGDIR APPTIMER_RES_FILE)
#define RUNNING_UPDATE_MSECS  100



#if defined(FEATURE_DISP_220X176)
#define TIMER_SCREEN_HEIGHT    38
#define TIMER_SCREEN_WIDTH     62
#define TIMER_IMAGE_WIDTH      138
#define TIMER_SCREEN_YOFFSET   36
#define TIMER_SCREEN_XOFFSET   36
#define TIMER_HEIGHT_N         2

#elif defined(FEATURE_DISP_320X240)
#define TIMER_SCREEN_HEIGHT   38
#define TIMER_SCREEN_WIDTH    84
#define TIMER_IMAGE_WIDTH     200
#define TIMER_SCREEN_YOFFSET  48
#define TIMER_SCREEN_XOFFSET  60
#define TIMER_HEIGHT_N         2

#elif defined(FEATURE_DISP_128X160)
#define TIMER_SCREEN_HEIGHT    38
#define TIMER_SCREEN_WIDTH     60
#define TIMER_IMAGE_WIDTH      100
#define TIMER_SCREEN_XOFFSET   20
#define TIMER_SCREEN_YOFFSET   30
#define TIMER_HEIGHT_N         3

#elif defined(FEATURE_DISP_176X220)
#define TIMER_SCREEN_HEIGHT    38
#define TIMER_SCREEN_WIDTH     62
#define TIMER_IMAGE_WIDTH      138
#define TIMER_SCREEN_XOFFSET   38
#define TIMER_SCREEN_YOFFSET   36
#define TIMER_HEIGHT_N         3
#elif defined(FEATURE_DISP_240X320)
#define TIMER_SCREEN_HEIGHT    38
#define TIMER_SCREEN_WIDTH     62
#define TIMER_IMAGE_WIDTH      180
#define TIMER_SCREEN_XOFFSET   62
#define TIMER_SCREEN_YOFFSET   56
#define TIMER_HEIGHT_N         3
#else
#define TIMER_SCREEN_HEIGHT    38
#define TIMER_SCREEN_WIDTH     60
#define TIMER_IMAGE_WIDTH      100
#define TIMER_SCREEN_XOFFSET   20
#define TIMER_SCREEN_YOFFSET   30
#define TIMER_HEIGHT_N         2


#endif
/*===========================================================================

                    TYPE DECLARATIONs

===========================================================================*/

typedef enum {
   W_IDLE = 0,     //初始状态
   W_STOPPED,      //停止
   W_RUNNING,      //计时中
   W_CONFIRM,      //到时状态
} AppTimerState;


typedef struct _CAppTimer CAppTimer;
struct _CAppTimer {
    AEEApplet          a;
    //IMenuCtl           *m_pmenu;
    AppTimerState   timer_state; //判断当前的状态
    boolean              m_suspending;//判断是否是后台运行或是否挂起
    ITimeCtl           *m_pTime;  //倒计数控件
    
    //IStatic             * pStatic;
    //IStatic             * pStaticText;

    IConfig            *m_pConfig;         //IConfig接口指针
    
    IAnnunciator        *m_pIAnn;
    IALERT            * m_pAlert;           //IALERT指针
    boolean           m_bAlert;         /*标识铃声状态*/
    ISound            * m_pSound;//振动接口指针
    boolean           m_bSound;  /*标识处于振动状态*/
    
    uint16              m_nTimeCtlCount;    //0代表焦点在小时上,1代表在分钟上,2代表秒钟上
    uint16              m_nNumKeyCount;     /*记录在设置时间时的按键次数.0:小时十位; 1:小时个位;   
                                                                2:分钟十位  3:分钟个位;   4:秒钟十位  5:秒钟个位*/

    uint32             m_nTicks;//启动时刻的手机时间
    uint32             AppTimerTime;//记录itime控件初始设置值
    int32               m_dwDispTime;       //屏幕上显示的时间，按秒减小
    
    int32               m_dwHour;           //记录用数字键设置的小时
    int32               m_dwMin;            //记录用数字键设置的分钟
    int32               m_dwSec;            //记录用数字键设置的秒钟

    int32               cxScreen;           //屏幕宽度
    int32               cyScreen;           //屏幕高度
    int32               titleBarHeight;   //标题栏高度
    boolean             m_Normal;
};

/*===========================================================================

                       FUNCTION DECLARATIONS

===========================================================================*/
int  AppTimerMod_CreateInstance(AEECLSID ClsId,
                                    IShell *pIShell,
                                    IModule *po,
                                    void **ppObj);
int  AppTimerMod_Load(IShell *ps,
                          void *pHelpers,
                          IModule **pMod);

static boolean AppTimer_HandleEvent(CAppTimer * pme, AEEEvent eCode, uint16 wParam, uint32 dwParam);
static void      AppTimer_Free(CAppTimer *pme);

static void      AppTimer_Stop(CAppTimer *pme);
static void      AppTimer_Running(CAppTimer *pme);

static int         AppTimer_Gettime(CAppTimer *pme,uint32 nTicks);
static void      AppTimer_Redraw(CAppTimer *pme);
static boolean CAppTimer_HandleKeyEvent(CAppTimer *pMe, uint16 wParam);
static boolean InitAppTimer(CAppTimer *pme);
static int32 AppTimerPromptMessage(CAppTimer *pme);
static void AppTimer_RingStop(CAppTimer *pme);
static void AppTimer_Vibrate(CAppTimer *pme);
static void AppTimer_StopVibrate(CAppTimer *pme);
static void AppTimer_Confirm(CAppTimer *pme);
//static int AppTimer_StaticTextInit(CAppTimer *pme);
static void TimerNotifyMP3PlayerAlertEvent(CAppTimer *pMe, boolean toStartAlert);
static boolean AppTimer_CanAlert(CAppTimer *pMe);
static void AppTimer_ClearAnnField(CAppTimer *pme);

/*=============================================================================
FUNCTION: AppTimerMod_Load

DESCRIPTION:oem层入口函数

PARAMETERS:
   *ps:
   *pHelpers:
   **pMod:

RETURN VALUE:
   int:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int AppTimerMod_Load(IShell *ps,
                         void *pHelpers,
                         IModule **pMod)
{
   return AEEStaticMod_New(sizeof(AEEMod),
                           ps,
                           pHelpers,
                           pMod,
                           (PFNMODCREATEINST) AppTimerMod_CreateInstance,
                           (PFNFREEMODDATA) NULL);
}

/*=============================================================================
FUNCTION: AppTimerMod_CreateInstance

DESCRIPTION:

PARAMETERS:
   ClsId:
   *pIShell:
   *po:
   **ppObj:

RETURN VALUE:
   int:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
#if defined(BREW_STATIC_APP)
int AppTimerMod_CreateInstance(AEECLSID ClsId,
                                   IShell *pIShell,
                                   IModule *po,
                                   void **ppObj)
#else
int AEEClsCreateInstance(AEECLSID ClsId,
                         IShell *pIShell,
                         IModule *po,
                         void **ppObj)
#endif
{
    CAppTimer     *pme;

    // Allocate the applet class.

    if (!AEEApplet_New(sizeof(CAppTimer),
                       ClsId,
                       pIShell,
                       po,
                       (IApplet **) ppObj,
                       (AEEHANDLER) AppTimer_HandleEvent,
                       (PFNFREEAPPDATA) AppTimer_Free)) {
       return ENOMEMORY;
    }

    pme = (CAppTimer *) *ppObj;
    
    InitAppTimer(pme);

    return AEE_SUCCESS;
}


/*=============================================================================
FUNCTION:  InitAppTimer

DESCRIPTION: Returns module information

PARAMETERS:
   *pme:

RETURN VALUE:
   PFNMODENTRY:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean InitAppTimer(CAppTimer *pme)
{
       AEERect rect;
       AEEDeviceInfo deviceInfo;
	
    //pme->m_pmenu = NULL;
    pme->m_pTime = NULL;
    //pme->pStatic = NULL;
    //pme->pStaticText = NULL;
    pme->m_pSound = NULL;
	
#ifndef AEE_SIMULATOR
	pme->m_pAlert = NULL;
#endif

    if (AEE_SUCCESS != ISHELL_CreateInstance(pme->a.m_pIShell, AEECLSID_CONFIG, (void **)&pme->m_pConfig))
    {
	 goto FAILED;
    }
    
    /*if (AEE_SUCCESS != ISHELL_CreateInstance(pme->a.m_pIShell, AEECLSID_MENUCTL, (void **)&pme->m_pmenu))
    {
        goto FAILED;
    }*/

    if (AEE_SUCCESS != ISHELL_CreateInstance(pme->a.m_pIShell, AEECLSID_COUNTDOWNCTL, (void **)&pme->m_pTime))
    {
        goto FAILED;
    }

    /*if( AEE_SUCCESS != ISHELL_CreateInstance( pme->a.m_pIShell, AEECLSID_STATIC, (void **)&pme->pStatic))
    {
        goto FAILED;
    }*/
    
    /*if( AEE_SUCCESS != ISHELL_CreateInstance( pme->a.m_pIShell, AEECLSID_STATIC, (void **)&pme->pStaticText))
    {
        goto FAILED;
    }*/
    
    if (AEE_SUCCESS != ISHELL_CreateInstance(pme->a.m_pIShell, AEECLSID_SOUND,(void **)&pme->m_pSound)) 
    {
        goto FAILED;
    }

#if !defined( AEE_SIMULATOR)
    if (AEE_SUCCESS != ISHELL_CreateInstance(pme->a.m_pIShell, AEECLSID_ALERT,(void **)&pme->m_pAlert)) 
    {
        goto FAILED;
    }
#endif //end of  AEE_SIMULATOR

    if (AEE_SUCCESS != ISHELL_CreateInstance(pme->a.m_pIShell,
                                            AEECLSID_ANNUNCIATOR,
                                            (void **)&pme->m_pIAnn))
    {
        goto FAILED;
    }
	MSG_FATAL("IANNUNCIATOR_SetFieldIsActiveEx::::timeapp111:::",0,0,0);
    //IANNUNCIATOR_SetFieldIsActiveEx(pme->m_pIAnn,FALSE);
    pme->timer_state = W_IDLE;
    pme->m_nTicks = 0;
    pme->m_suspending = FALSE;
    pme->m_dwDispTime = 0;
    pme->m_nNumKeyCount = 0;
    
    ISHELL_GetDeviceInfo(pme->a.m_pIShell, &deviceInfo);
    pme->cxScreen = deviceInfo.cxScreen;
    pme->cyScreen = deviceInfo.cyScreen;
    pme->titleBarHeight = GetTitleBarHeight(pme->a.m_pIDisplay);

    //if(pme->m_pmenu)
    //{
        /*绘制软键栏*/
        /*SETAEERECT( &rect, 0, pme->cyScreen - BOTTOMBAR_HEIGHT,  pme->cxScreen, BOTTOMBAR_HEIGHT);
        IMENUCTL_SetRect(pme->m_pmenu, &rect);
        
        IMENUCTL_SetProperties( pme->m_pmenu, MP_BIND_ITEM_TO_NUMBER_KEY);
        IMENUCTL_SetActive( pme->m_pmenu, TRUE);
    }*/
        /*TIME控件栏*/
    if(NULL != pme->m_pTime)
    {
        //uint32 dwProps;
#if 0        
        SETAEERECT( &rect, 
                            (pme->cxScreen - TIMER_IMAGE_WIDTH)/2 + TIMER_SCREEN_XOFFSET+2,
                            pme->titleBarHeight + MENUITEM_HEIGHT*2 +  TIMER_SCREEN_YOFFSET - 16,
                            TIMER_SCREEN_WIDTH,
                            0);
#else
#if defined(FEATURE_VERSION_K212)||defined(FEATURE_VERSION_K212_12832)
        SETAEERECT( &rect, 
                            (pme->cxScreen - TIMER_IMAGE_WIDTH)/2 + TIMER_SCREEN_XOFFSET+2,
                            MENUITEM_HEIGHT*TIMER_HEIGHT_N +  TIMER_SCREEN_YOFFSET - 32,
                            TIMER_SCREEN_WIDTH,
                            0);
#else
		SETAEERECT( &rect, 
                            (pme->cxScreen - TIMER_IMAGE_WIDTH)/2 + TIMER_SCREEN_XOFFSET+2,
                            MENUITEM_HEIGHT*TIMER_HEIGHT_N +  TIMER_SCREEN_YOFFSET - 16,
                            TIMER_SCREEN_WIDTH,
                            0);
#endif
#endif

        //SETAEERECT( &rect, 0, (pme->cyScreen - pme->titleBarHeight) / 2 , pme->cxScreen, pme->titleBarHeight*2);
        ITIMECTL_SetRect(pme->m_pTime, &rect);
        ITIMECTL_SetFont(pme->m_pTime, AEE_FONT_BOLD, AEE_FONT_BOLD);
        //ITIMECTL_SetFont(pme->m_pTime, AEE_FONT_LARGE, AEE_FONT_LARGE);
        //dwProps = ITIMECTL_GetProperties(pme->m_pTime);
        ITIMECTL_SetProperties(pme->m_pTime, TP_AUTOREDRAW);
        (void) ITIMECTL_SetTimeEx(pme->m_pTime, pme->m_dwDispTime, TRUE);
        
        ITIMECTL_SetActive( pme->m_pTime, TRUE);
    }

    return TRUE;
    
FAILED:
    AppTimer_Free(pme);
    return FALSE;
}
#if 0
/*=============================================================================
FUNCTION: AppTimer_StaticTextInit

DESCRIPTION:创建静态时间格式提示文本
   Called when applet init or resumed.

PARAMETERS:
   *pme:

RETURN VALUE:
0:OK;-1:FAIL

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int AppTimer_StaticTextInit(CAppTimer *pme)
{
        //draw static text information.
        AEERect  rect  = {0};
        AECHAR wszTitle[16];

        if(NULL ==  pme->pStaticText)
        {
            if( AEE_SUCCESS != ISHELL_CreateInstance( pme->a.m_pIShell, AEECLSID_STATIC, (void **)&pme->pStaticText))
            {
                  return -1;
            }
        }
        /*高度为titlebarhight，与time控件之间隔一个2高度的分割线*/
        SETAEERECT( &rect, 0, 2*pme->titleBarHeight, pme->cxScreen, MENUITEM_HEIGHT );
        ISTATIC_SetRect(pme->pStaticText, &rect);
        ISTATIC_SetProperties(pme->pStaticText, ST_CENTERTEXT|ST_MIDDLETEXT|ST_TRANSPARENT | ST_TRANSPARENTBACK);
        ISHELL_LoadResString(pme->a.m_pIShell, AEE_APPTIMER_RES_FILE, IDS_APPTIMER_INFO, wszTitle, sizeof(wszTitle));
        ISTATIC_SetText(pme->pStaticText, NULL, wszTitle, AEE_FONT_NORMAL, AEE_FONT_NORMAL);
        ISTATIC_SizeToFit(pme->pStaticText, &rect);
        
        ISTATIC_SetActive(pme->pStaticText, TRUE);
        return 0;
}
#endif
/*=============================================================================
FUNCTION: AppTimer_Free

DESCRIPTION:
   Called by AEEAppGen when the applet's memory is being released.

PARAMETERS:
   *pme:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void AppTimer_Free(CAppTimer *pme)
{
    if (pme->m_pConfig)
    {
        ICONFIG_Release(pme->m_pConfig);
        pme->m_pConfig = NULL;
    }

    /*if (pme->m_pmenu != NULL) {
       IMENUCTL_Release(pme->m_pmenu);
       pme->m_pmenu = NULL;
    }*/

    if (pme->m_pTime != NULL) {
       ITIMECTL_Release(pme->m_pTime);
       pme->m_pTime = NULL;
    }

    /*if( pme->pStatic != NULL)
    {
        ISTATIC_Release( pme->pStatic);
        pme->pStatic = NULL;
    }*/
    
    /*if( pme->pStaticText != NULL)
    {
        ISTATIC_Release( pme->pStaticText);
        pme->pStaticText = NULL;
    }*/
    
    if(pme->m_pSound != NULL)
    {
        ISOUND_Release(pme->m_pSound);
        pme->m_pSound = NULL;
    }

    if(pme->m_pIAnn != NULL)
    {
        IANNUNCIATOR_Release(pme->m_pIAnn);
        pme->m_pIAnn = NULL;
    } 
    
#ifndef AEE_SIMULATOR
    if(pme->m_pAlert!= NULL)
    {
        IALERT_Release(pme->m_pAlert);
        pme->m_pAlert = NULL;
    }
#endif
    
    ISHELL_CancelTimer(pme->a.m_pIShell,NULL,pme);
}
/*=============================================================================
FUNCTION: AppTimerPromptMessage

DESCRIPTION:
   Called by AEEAppGen when the applet's itime expire

PARAMETERS:
   *pme:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int32 AppTimerPromptMessage(CAppTimer *pme)
{
        //PromptMsg_Param_type    parm;
        AECHAR                 wszTitle[16];
        byte     curProfile = 0;
        byte     byte_return[PROFILENUMBER];
        ringID   ring_id[PROFILENUMBER];//闹钟铃声ID
        IImage* prompt;
        AEERect rc;

#if 0
        if(NULL == pme->pStatic)
        {
            if( AEE_SUCCESS != ISHELL_CreateInstance( pme->a.m_pIShell, AEECLSID_STATIC, (void **)&pme->pStatic))
            {
                AppTimer_Free(pme);
                return FALSE;
            }
        }  

        MEMSET( &parm, 0, sizeof( parm));
        parm.ePMsgType  = MESSAGE_INFORMATIVE;
        parm.eBBarType  = BTBAR_PROMPT_OK;
#endif
        ISHELL_LoadResString(pme->a.m_pIShell, AEE_APPTIMER_RES_FILE, IDS_APPTIMER_PROMPT, wszTitle, sizeof(wszTitle));
        //parm.pwszTitle = wszTitle;
        prompt = ISHELL_LoadResImage(pme->a.m_pIShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_TIMER);

//        parm.pwszMsg = wszTitle;
        
        //ISHELL_CreateDialog( pme->a.m_pIShell,AEE_APPTIMER_RES_FILE,IDD_DIALOG_6001,NULL);
        
        //parm.nMsgResID = IDS_APPTIMER_PROMPT;
        //MEMCPY(parm.strMsgResFile,AEE_APPTIMER_RES_FILE,sizeof(parm.strMsgResFile));

        pme->m_dwHour = 0;
        pme->m_dwMin = 0;
        pme->m_dwSec = 0;

        ICONFIG_GetItem(pme->m_pConfig, CFGI_PROFILE_CUR_NUMBER, &curProfile, sizeof(curProfile));

        ICONFIG_GetItem(pme->m_pConfig, CFGI_PROFILE_SMS_RINGER, byte_return, sizeof(byte_return));
        if((OEMNV_ALERTTYPE_VIB == byte_return[curProfile]) || (OEMNV_ALERTTYPE_VIBRINGER == byte_return[curProfile]))
        {
             AppTimer_Vibrate(pme);
             pme->m_bSound = TRUE;
             ISHELL_SetTimer(pme->a.m_pIShell, 60000, (PFNNOTIFY)AppTimer_StopVibrate, pme);
        }

#ifndef AEE_SIMULATOR
        pme->m_bAlert = TRUE;
        //取闹钟设定的铃声
        ICONFIG_GetItem(pme->m_pConfig,CFGI_PROFILE_ALARM_RINGER, (void*)ring_id,sizeof(ring_id));
        /*如果用IALERT_StartRingerAlert()会导致normal以外的模式不一致,
        比如没设振动时会有先振动后闹铃的现象*/
        TimerNotifyMP3PlayerAlertEvent(pme, TRUE);
        if(ring_id[curProfile].ringType == OEMNV_MID_RINGER)
        {
            IALERT_StartRingerAlert(pme->m_pAlert, ring_id[curProfile].midID);
        }
        else
        {
            if( IALERT_StartMp3Alert(pme->m_pAlert, ring_id[curProfile].szMusicname, ALERT_NORMAL_SND) != SUCCESS )
            {
                IALERT_StartRingerAlert(pme->m_pAlert, ring_id[curProfile].midID);
            }
        }
        ISHELL_SetTimer(pme->a.m_pIShell, 60000, (PFNNOTIFY)(AppTimer_RingStop), pme);
#endif

//        DrawPromptMessage( pme->a.m_pIDisplay, pme->pStatic, &parm);
#if 0
        SETAEERECT(&rc, 0, TITLEBAR_HEIGHT, pme->cxScreen, pme->cyScreen - TITLEBAR_HEIGHT - BOTTOMBAR_HEIGHT);
#else
        SETAEERECT(&rc, 0, 0, pme->cxScreen, pme->cyScreen - TITLEBAR_HEIGHT - BOTTOMBAR_HEIGHT);
#endif
        Appscommon_ResetBackgroundEx(pme->a.m_pIDisplay, &rc, TRUE);
        if( prompt != NULL)
        {
            AEEImageInfo ImageInfo;
            
            IIMAGE_GetInfo(prompt, &ImageInfo);
            //IIMAGE_Draw(prompt, 0, TITLEBAR_HEIGHT);
            IIMAGE_Draw(prompt, (pme->cxScreen - ImageInfo.cx)/2, TITLEBAR_HEIGHT + (pme->cyScreen - BOTTOMBAR_HEIGHT - ImageInfo.cy)/2);
            IIMAGE_Release(prompt);
            prompt = NULL;
        }
        //draw string
        if( wszTitle != NULL)
        {
            AEEDeviceInfo di;
            RGBVAL nOldFontColor;
            ISHELL_GetDeviceInfo(pme->a.m_pIShell, &di);
#if 0            
            SETAEERECT(&rc, 0, TITLEBAR_HEIGHT, di.cxScreen, TITLEBAR_HEIGHT);
#else
            SETAEERECT(&rc, 0, 0, di.cxScreen, TITLEBAR_HEIGHT);
#endif
            nOldFontColor = IDISPLAY_SetColor(pme->a.m_pIDisplay, CLR_USER_TEXT, RGB_WHITE);
            IDISPLAY_DrawText(pme->a.m_pIDisplay, 
                                AEE_FONT_NORMAL, 
                                wszTitle, 
                                -1, 
                                rc.x, 
                                rc.y, 
                                &rc, 
                                IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE |IDF_TEXT_TRANSPARENT);
            (void)IDISPLAY_SetColor(pme->a.m_pIDisplay, CLR_USER_TEXT, nOldFontColor);
        }
        pme->timer_state = W_CONFIRM;
        {
            BottomBar_Param_type  BBarParam ={0};
            
            BBarParam.eBBarType = BTBAR_BACK;
            DrawBottomBar(pme->a.m_pIDisplay, &BBarParam);
        }

        return SUCCESS;
}
/*==============================================================================
函数：
       AppTimer_Vibrate

说明：
       此函数使手机处在振动状态。

参数：
       pMe [in]：指向CAppTimer Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       无。

备注：:
==============================================================================*/
static void AppTimer_Vibrate(CAppTimer *pme)
{
    static boolean vib = FALSE;

    // Alternate between vibrating and not
    vib = !vib;
    if( vib )
    {
        #ifndef FEATURE_VERSION_W317A
        ISOUND_Vibrate(pme->m_pSound, 1500);
		#endif
        (void)ISHELL_SetTimer(pme->a.m_pIShell, 1500, (PFNNOTIFY)(AppTimer_Vibrate), pme);
    }
    else
    {
        ISOUND_StopVibrate(pme->m_pSound);
        (void)ISHELL_SetTimer(pme->a.m_pIShell, 1000, (PFNNOTIFY)(AppTimer_Vibrate), pme);
    }
}
/*==============================================================================
函数：
       AppTimer_StopVibrate

说明：
       此函数使手机取消振动状态。

参数：
       pme ：指向CAppTimer Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       无。

备注：:
==============================================================================*/
static void AppTimer_StopVibrate(CAppTimer *pme)
{
    ISHELL_CancelTimer(pme->a.m_pIShell, (PFNNOTIFY)AppTimer_Vibrate, pme);
}

/*==============================================================================
函数：
       AppTimer_ClearAnnField

说明：
       此函数使手机取消振动状态。

参数：
       pme ：指向CAppTimer Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       无。

备注：:
==============================================================================*/
static void AppTimer_ClearAnnField(CAppTimer *pme)
{
    IANNUNCIATOR_SetField(pme->m_pIAnn, ANNUN_FIELD_ALARM, ANNUN_STATE_TIMER_OFF);
}

/*==============================================================================
函数：
       AppTimer_RingStop

说明：
       此函数使手机停止响铃状态。

参数：
       pme ：指向CAppTimer Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       无。

备注：:
==============================================================================*/
static void AppTimer_RingStop(CAppTimer *pme)
{
#ifndef AEE_SIMULATOR
    if(pme->m_bAlert == TRUE)
    {
        pme->m_bAlert = FALSE;
        IALERT_StopRingerAlert(pme->m_pAlert);
        IALERT_StopMp3Alert(pme->m_pAlert);
        TimerNotifyMP3PlayerAlertEvent(pme, FALSE);
    }
    pme->timer_state = W_IDLE;
    if(pme->m_Normal)
    {
        ITIMECTL_SetActive(pme->m_pTime, TRUE);
        AppTimer_Redraw(pme);
    }
    else
    {
        ISHELL_CloseApplet(pme->a.m_pIShell, FALSE);
    }
#endif
}
/*=============================================================================
FUNCTION:  AppTimer_HandleEvent

DESCRIPTION:apptimer event handle

PARAMETERS:
   *pme:

RETURN VALUE:
   boolean:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean AppTimer_HandleEvent(CAppTimer *pme, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
	if (eCode == EVT_USER)
	{
		if (wParam == AVK_CLR)
		{
			eCode = EVT_KEY;
		}
		else if (wParam == AVK_SELECT)
		{
			eCode = EVT_KEY;
		}
		else if (wParam == AVK_INFO)
		{
			eCode = EVT_KEY;
		}
	}
#endif
    switch (eCode)
    {
        case EVT_APP_START:
        if((pme->m_suspending == TRUE) && (pme->timer_state == W_RUNNING) && (0 == ITIMECTL_GetTime(pme->m_pTime)))
        {
            //pme->timer_state = W_STOPPED;
            pme->m_suspending = FALSE;
            
            AppTimer_Redraw( pme);
            if(SUCCESS == AppTimerPromptMessage(pme))
            {
                return TRUE;
            }
            return FALSE;
        }
        if(pme->m_pIAnn != NULL)
         {
            IANNUNCIATOR_SetFieldIsActiveEx(pme->m_pIAnn,FALSE);
        }
        pme->m_suspending = FALSE;
        AppTimer_Redraw(pme);
        return TRUE;

        case EVT_APP_SUSPEND:
            pme->m_suspending = TRUE;
            if (pme->timer_state != W_RUNNING)
            {
                ISHELL_CancelTimer(pme->a.m_pIShell, NULL, pme);
            }

            if(pme->timer_state == W_CONFIRM)
            {
                IALERT_StopMp3Alert(pme->m_pAlert);
            }
            /*if(NULL != pme->pStatic)
            {
                 ISTATIC_Release(pme->pStatic);
                 pme->pStatic = NULL;
            }*/
            /*if(NULL != pme->pStaticText)
            {
                 ISTATIC_Release(pme->pStaticText);
                 pme->pStaticText = NULL;
            }*/
            
            return TRUE;

        case EVT_APP_RESUME:
            pme->m_suspending = FALSE;

            /*if( AEE_SUCCESS != ISHELL_CreateInstance( pme->a.m_pIShell, AEECLSID_STATIC, (void **)&pme->pStatic))
            {
                return FALSE;
            }*/
            /*if(0 != AppTimer_StaticTextInit(pme))
            {
                return FALSE;
            }*/
            
            switch (pme->timer_state)
            {
                case W_RUNNING:
                    AppTimer_Running( pme);
                    break;

                case W_STOPPED:
                    AppTimer_Stop( pme);
                    break;

                case W_CONFIRM:
                    AppTimer_Confirm(pme);
                    break;
            }
            AppTimer_Redraw( pme);
            return TRUE;


        case EVT_APP_STOP:
            if(0 != ITIMECTL_GetTime(pme->m_pTime) && pme->timer_state != W_CONFIRM)
            {
                *((boolean *)dwParam) = FALSE;
                pme->m_suspending = TRUE;
                return TRUE;
            }

            pme->m_dwHour = 0;
            pme->m_dwMin = 0;
            pme->m_dwSec = 0;

            pme->m_bSound = FALSE;
            //pme->m_bAlert = FALSE;
            ISHELL_CancelTimer(pme->a.m_pIShell, (PFNNOTIFY)AppTimer_Vibrate, pme);
            ISHELL_CancelTimer(pme->a.m_pIShell, (PFNNOTIFY)AppTimer_StopVibrate, pme);
            if(pme->m_bAlert)
            {
                pme->m_bAlert =FALSE;
                IALERT_StopRingerAlert(pme->m_pAlert);
                IALERT_StopMp3Alert(pme->m_pAlert);
                TimerNotifyMP3PlayerAlertEvent(pme, FALSE);
            }
            pme->timer_state = W_IDLE;
            IANNUNCIATOR_SetField(pme->m_pIAnn, ANNUN_FIELD_ALARM, ANNUN_STATE_TIMER_OFF);
            return TRUE;

        case EVT_APP_NO_SLEEP:
        case EVT_DIALOG_INIT:
        case EVT_DIALOG_START:
        case EVT_DIALOG_END:
        case EVT_USER_REDRAW:
            return TRUE;

        case EVT_KEY:
            if(pme->m_bSound == TRUE)
            {
                pme->m_bSound = FALSE;
                ISHELL_CancelTimer(pme->a.m_pIShell, (PFNNOTIFY)AppTimer_Vibrate, pme);
                ISHELL_CancelTimer(pme->a.m_pIShell, (PFNNOTIFY)AppTimer_StopVibrate, pme);
            }
#ifndef AEE_SIMULATOR
            if(pme->m_bAlert == TRUE)
            {
                pme->m_bAlert = FALSE;
                IALERT_StopRingerAlert(pme->m_pAlert);
                IALERT_StopMp3Alert(pme->m_pAlert);
                TimerNotifyMP3PlayerAlertEvent(pme, FALSE);
            }
#endif  
#ifdef FEATURE_LCD_TOUCH_ENABLE
          if(wParam==AVK_SYMBOL)
            {
              return FALSE;
            }
#endif          
            switch(wParam)
            {

                case AVK_CLR: 
                {
                    if (pme->timer_state == W_RUNNING || pme->timer_state == W_STOPPED)
                    {
                        ISHELL_CloseApplet(pme->a.m_pIShell, FALSE);
                    }
                    else
                    {
                        //pme->m_dwDispTime = 0;
                        //(void) ITIMECTL_SetTimeEx(pme->m_pTime, pme->m_dwDispTime, TRUE);
                        pme->timer_state = W_IDLE;
                        ISHELL_CancelTimer(pme->a.m_pIShell, NULL, pme);
                        ISHELL_CloseApplet(pme->a.m_pIShell, FALSE);
                        IANNUNCIATOR_SetField(pme->m_pIAnn, ANNUN_FIELD_ALARM, ANNUN_STATE_TIMER_OFF);
                    }

                    return TRUE;
                }
                case AVK_SELECT:
                    {
                        switch( pme->timer_state)
                        {
                            case W_IDLE:
                                {
                                      if(100 < ITIMECTL_GetTime(pme->m_pTime))
                                      {
                                          pme->timer_state    = W_RUNNING;
                                          pme->AppTimerTime = ITIMECTL_GetTime(pme->m_pTime);
                                          pme->m_nTicks       = ISHELL_GetTimeMS( pme->a.m_pIShell);
                                          ITIMECTL_SetActive(pme->m_pTime, FALSE);
                                          AppTimer_Running( pme);
                                          IANNUNCIATOR_SetField(pme->m_pIAnn, ANNUN_FIELD_ALARM, ANNUN_STATE_TIMER_ON);
                                      }
                                      else
                                      {
                                        ;//prompt a dialog to set the time
                                      }
                                }
                                break;
                            
                            case W_RUNNING:
                                pme->m_dwDispTime = 0;
                                pme->m_dwHour = 0;
                                pme->m_dwMin = 0;
                                pme->m_dwSec = 0;
                                (void) ITIMECTL_SetTimeEx(pme->m_pTime, pme->m_dwDispTime, TRUE);
                                pme->timer_state = W_IDLE;
                                ISHELL_CancelTimer(pme->a.m_pIShell, NULL, pme);
                                pme->m_nTimeCtlCount = 0;
                                pme->m_nNumKeyCount = 0;
                                ITIMECTL_SetActive(pme->m_pTime, TRUE);
                                IANNUNCIATOR_SetField(pme->m_pIAnn, ANNUN_FIELD_ALARM, ANNUN_STATE_TIMER_OFF); 
                                break;

                            case W_CONFIRM:
                                return TRUE;

                            default:
                                break;
                        }
                        AppTimer_Redraw( pme);
                        return TRUE;
                    }

                case AVK_UP:
                case AVK_DOWN:
                    //ITIMECTL_HandleEvent(pme->m_pTime,EVT_KEY,wParam,dwParam);
                    //break;
                    return TRUE;

                default:
                    if(pme->timer_state == W_IDLE)
                    {
                        CAppTimer_HandleKeyEvent(pme,wParam);
                    }
                    break;
             }
            return TRUE;
#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
		case EVT_PEN_UP:
			{
				int i=0;
				AEEDeviceInfo devinfo;
				int nBarH ;
				AEERect rc;
				int16 wXPos = (int16)AEE_GET_X((const char *)dwParam);
				int16 wYPos = (int16)AEE_GET_Y((const char *)dwParam);
				int xy[][4] = {
					{79,107,99,132},      // hour
					{107,107,130,132},      // minute
					{138,107,158,132}     // second
					
				};
				MSG_FATAL("wXPos========%d,wYPos==========%d",wXPos,wYPos,0);
				nBarH = GetBottomBarHeight(pme->a.m_pIDisplay);
        
				MEMSET(&devinfo, 0, sizeof(devinfo));
				ISHELL_GetDeviceInfo(pme->a.m_pIShell, &devinfo);
				SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);

				if(TOUCH_PT_IN_RECT(wXPos,wYPos,rc))
				{
					if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
					{
						boolean rt =  ISHELL_PostEvent(pme->a.m_pIShell,AEECLSID_APPTIMER,EVT_USER,AVK_SELECT,0);
						return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)   && wXPos < rc.x + (rc.dx/3)*2 )//中
					{
						 boolean rt = ISHELL_PostEvent(pme->a.m_pIShell,AEECLSID_APPTIMER,EVT_USER,AVK_INFO,0);
						 return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//右
					{						
						 boolean rt = ISHELL_PostEvent(pme->a.m_pIShell,AEECLSID_APPTIMER,EVT_USER,AVK_CLR,0);
						 return rt;
					}
				}
				
				for( i = 0; i < 3; i ++)
				{
					if( wXPos >= xy[i][0] &&
						wXPos <= xy[i][2] &&
						wYPos >= xy[i][1] &&
						wYPos <= xy[i][3]
					)
					{
						if (i == 0)
						{
							if (pme->timer_state == W_IDLE)
							{
								pme->m_nTimeCtlCount = 0;
								pme->m_nNumKeyCount = 0;
								ITIMECTL_SetEditField(pme->m_pTime,ITF_HOUR); 
								(void)ITIMECTL_Redraw(pme->m_pTime);
								return TRUE;
							}

						}
						else if (i == 1)
						{
							if (pme->timer_state == W_IDLE)
							{
								pme->m_nTimeCtlCount = 1;
								pme->m_nNumKeyCount = 2;
								ITIMECTL_SetEditField(pme->m_pTime,ITF_MIN); 
								(void)ITIMECTL_Redraw(pme->m_pTime);
								return TRUE;
							}
						}
						else if (i == 2)
						{
							if (pme->timer_state == W_IDLE)
							{
								pme->m_nTimeCtlCount = 2;
								pme->m_nNumKeyCount = 4;
								ITIMECTL_SetEditField(pme->m_pTime,ITF_SEC); 
								(void)ITIMECTL_Redraw(pme->m_pTime);
								return TRUE;
							}
						}
						
					}
				}
			}
			return TRUE;
#endif              
        default:
            break;
        }

    return (FALSE);
}

/*=============================================================================
FUNCTION: AppTimer_ReDraw

DESCRIPTION:画出定时器界面

PARAMETERS:
   *pme:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void AppTimer_Redraw(CAppTimer *pme)
{
    TitleBar_Param_type  TBarParam = {0};
    BottomBar_Param_type  BBarParam ={0};
    AECHAR                   wszTitle[16] = {0};
    AEERect                  rect            = {0};
    RGBVAL                   nOldFontColor;
    IImage                    *pBgImage = NULL;
    BottomBar_e_Type    btbType[] = {BTBAR_START_BACK, 
                                     BTBAR_CANCEL_RESUME_NULL, 
                                     BTBAR_STOP_BACK, 
                                     BTBAR_STOP_BACK};
    
    //draw background
#if 0    
    SETAEERECT(&rect, 0, pme->titleBarHeight, pme->cxScreen, pme->cyScreen - BOTTOMBAR_HEIGHT);
#else
    SETAEERECT(&rect, 0, 0, pme->cxScreen, pme->cyScreen - BOTTOMBAR_HEIGHT);
#endif
    Appscommon_ResetBackgroundEx(pme->a.m_pIDisplay, &rect, TRUE);

    //Draw title bar
    ISHELL_LoadResString(pme->a.m_pIShell, AEE_APPTIMER_RES_FILE, IDS_APPTIMER_TITLE, wszTitle, sizeof(wszTitle));
    TBarParam.pwszTitle = wszTitle;
	#if 0
    TBarParam.dwAlignFlags = IDF_TEXT_TRANSPARENT | IDF_ALIGN_CENTER;
    DrawTitleBar(pme->a.m_pIDisplay, &TBarParam);
    #else
	IANNUNCIATOR_SetFieldText(pme->m_pIAnn,wszTitle);
	#endif
    //draw static text info
    MEMSET(wszTitle, 0, sizeof(wszTitle));
	#if 0
    SETAEERECT( &rect, 0, pme->titleBarHeight + MENUITEM_HEIGHT/2, pme->cxScreen, MENUITEM_HEIGHT);
	#else
	SETAEERECT( &rect, 0, MENUITEM_HEIGHT/2, pme->cxScreen, MENUITEM_HEIGHT);
	#endif
    ISHELL_LoadResString(pme->a.m_pIShell, AEE_APPTIMER_RES_FILE, IDS_APPTIMER_INFO, wszTitle, sizeof(wszTitle));
    nOldFontColor = IDISPLAY_SetColor(pme->a.m_pIDisplay, CLR_USER_TEXT, RGB_WHITE);
    IDISPLAY_DrawText(pme->a.m_pIDisplay, 
                        AEE_FONT_NORMAL, 
                        wszTitle, 
                        -1, 
                        0, 
                        0, 
                        &rect, 
                        IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE |IDF_TEXT_TRANSPARENT);
    (void)IDISPLAY_SetColor(pme->a.m_pIDisplay, CLR_USER_TEXT, nOldFontColor);

    //draw clock image
    pBgImage = ISHELL_LoadResImage(pme->a.m_pIShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_TIMER_CLOCK);
    if(pBgImage != NULL)
    {
    	#if 0
        IImage_Draw(pBgImage, (rect.dx - TIMER_IMAGE_WIDTH)/2, pme->titleBarHeight + MENUITEM_HEIGHT*2);
		#else
		IImage_Draw(pBgImage, (rect.dx - TIMER_IMAGE_WIDTH)/2,  MENUITEM_HEIGHT*TIMER_HEIGHT_N);
		#endif
        IImage_Release(pBgImage);
        pBgImage = NULL;
    }

    //draw line
    BBarParam.eBBarType = btbType[pme->timer_state];
    if(pme->timer_state == W_IDLE || pme->timer_state == W_CONFIRM)
    {
        uint32 dwseconds = ITIMECTL_GetTime(pme->m_pTime);
        if(dwseconds == 0)
        {
            BBarParam.eBBarType = BTBAR_BACK;
        }
    }
    DrawBottomBar(pme->a.m_pIDisplay, &BBarParam);

    ITIMECTL_Redraw( pme->m_pTime);
    //IMENUCTL_Redraw( pme->m_pmenu);
    //ISTATIC_Redraw(pme->pStaticText);
    IDISPLAY_Update(pme->a.m_pIDisplay);
}

/*=============================================================================
FUNCTION: AppTimer_Gettime

DESCRIPTION:画出当前要刷新的时间

PARAMETERS:
   *pme:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int AppTimer_Gettime(CAppTimer *pme,uint32 nTicks)
{
    pme->m_dwDispTime = pme->AppTimerTime - (nTicks - pme->m_nTicks);

    ITIMECTL_SetProperties(pme->m_pTime,0);

    if(pme->m_suspending != TRUE)
    {
        (void) ITIMECTL_SetTimeEx(pme->m_pTime, pme->m_dwDispTime,TRUE);
    }

    return ((100 >= pme->m_dwDispTime)?-1:0);
}
/*=============================================================================
FUNCTION: AppTimer_Running

DESCRIPTION: 定时器开始倒数计时

PARAMETERS:
   *pme:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void AppTimer_Running(CAppTimer *pme)
{
        uint32   nMS;
        int ret = 0;

        //返回当前时间（毫秒）。
        //返回的值取决于 BREW 移动设备从基站获取的设备当前时间值
        nMS = ISHELL_GetTimeMS(pme->a.m_pIShell);

        ret = AppTimer_Gettime(pme,nMS); //画出当前的时间
        if(-1 != ret)
        {
                // 算法尽量靠近下100ms的时间
                nMS = (RUNNING_UPDATE_MSECS - (ISHELL_GetTimeMS(pme->a.m_pIShell) - nMS));

                (void) ISHELL_SetTimer(pme->a.m_pIShell,(int32) nMS,(PFNNOTIFY)AppTimer_Running, pme);
        }
        else if(TRUE == pme->m_suspending) //后台计数到时
        {
                pme->m_dwDispTime = 0;
                (void) ITIMECTL_SetTimeEx(pme->m_pTime, pme->m_dwDispTime, TRUE);
                (void) ISHELL_CancelTimer(pme->a.m_pIShell,(PFNNOTIFY)AppTimer_Running, pme);
                
                if(AppTimer_CanAlert(pme))
                {
                    ret = ISHELL_StartApplet(pme->a.m_pIShell, AEECLSID_APPTIMER);
                    pme->m_Normal = FALSE;
                }
                else
                {
                    #ifndef FEATURE_VERSION_W317A
                    ISOUND_Vibrate(pme->m_pSound, 2000);
					#endif
                    pme->timer_state = W_IDLE;
                    pme->m_nTimeCtlCount = 0;
                    pme->m_nNumKeyCount = 0;
                    ITIMECTL_SetActive(pme->m_pTime, TRUE);

                    IANNUNCIATOR_SetField(pme->m_pIAnn, ANNUN_FIELD_ALARM, ANNUN_STATE_TIMER_ON|ANNUN_STATE_BLINK);
                    IANNUNCIATOR_SetUnblinkTimer(pme->m_pIAnn, ANNUN_FIELD_ALARM, ANNUN_STATE_TIMER_OFF, 10000);
                    ISHELL_SetTimer(pme->a.m_pIShell, 10000, (PFNNOTIFY)AppTimer_ClearAnnField, pme);// 这里的TIMER 要和状态栏那边的闪烁时间一致 
                    
                }
        }
        else //正常计数到时
        {
            pme->m_dwDispTime = 0;
            (void) ITIMECTL_SetTimeEx(pme->m_pTime, pme->m_dwDispTime, TRUE);        
            (void) ISHELL_CancelTimer(pme->a.m_pIShell,(PFNNOTIFY)AppTimer_Running, pme);
            pme->timer_state = W_CONFIRM;
            pme->m_Normal = TRUE;
            AppTimerPromptMessage(pme);
        }
}

/*=============================================================================
FUNCTION: AppTimer_Stop

DESCRIPTION: 定时器停止

PARAMETERS:
   *pme:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void AppTimer_Stop(CAppTimer *pme)
{
    (void) ISHELL_CancelTimer(pme->a.m_pIShell, (PFNNOTIFY)AppTimer_Running, pme);
    pme->m_dwDispTime = pme->AppTimerTime - (ISHELL_GetTimeMS(pme->a.m_pIShell) - pme->m_nTicks);
    ITIMECTL_SetProperties(pme->m_pTime,0);
    (void) ITIMECTL_SetTimeEx(pme->m_pTime, pme->m_dwDispTime, TRUE);

    pme->timer_state = W_STOPPED;
    AppTimer_Redraw( pme);
}
/*=============================================================================
FUNCTION: AppTimer_Stop

DESCRIPTION: 定时器到时处理函数

PARAMETERS:
   *pme:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void AppTimer_Confirm(CAppTimer *pme)
{
    pme->timer_state = W_IDLE;
    pme->m_nTimeCtlCount = 0;
    pme->m_nNumKeyCount = 0;
    ITIMECTL_SetActive(pme->m_pTime, TRUE);
    AppTimer_Redraw( pme);
    return;
}
/*==============================================================================
函数：
       CAppTimer_HandleKeyEvent

说明：
       时间控件下按数字键设置时间

参数：
       pMe [in]：指向Applet对象结构的指针。该结构包含小程序的特定信息。
       wParam [in]: 按键事件.


返回值：
       boolean。

备注：:

==============================================================================*/
static boolean CAppTimer_HandleKeyEvent(CAppTimer *pMe, uint16 wParam)
{
    uint16 nKeyMask = 0;

    if (pMe == NULL)
    {
        return FALSE;
    }
    
    switch(wParam)
    {
       case AVK_LEFT:
            if(pMe->m_nTimeCtlCount == 0)
            {
                pMe->m_nTimeCtlCount = 2;
                pMe->m_nNumKeyCount = 4;
                ITIMECTL_SetEditField(pMe->m_pTime,ITF_SEC); 
                (void)ITIMECTL_Redraw(pMe->m_pTime);
            }
            else if(pMe->m_nTimeCtlCount == 1)
            {
                pMe->m_nTimeCtlCount = 0;
                pMe->m_nNumKeyCount = 0;
                ITIMECTL_SetEditField(pMe->m_pTime,ITF_HOUR); 
                (void)ITIMECTL_Redraw(pMe->m_pTime);
            }
            else if(pMe->m_nTimeCtlCount == 2)
            {
                pMe->m_nTimeCtlCount = 1;
                pMe->m_nNumKeyCount = 2;
                ITIMECTL_SetEditField(pMe->m_pTime,ITF_MIN); 
                (void)ITIMECTL_Redraw(pMe->m_pTime);
            }
            return TRUE;
            
        case AVK_RIGHT:
            //在时间控件激活的情况下,忽略按上下键所设置的时间
            if(pMe->m_nTimeCtlCount == 0)
            {
                pMe->m_nTimeCtlCount = 1;
                pMe->m_nNumKeyCount = 2;
                ITIMECTL_SetEditField(pMe->m_pTime,ITF_MIN); 
                (void)ITIMECTL_Redraw(pMe->m_pTime);
            }
            else if(pMe->m_nTimeCtlCount == 1)
            {
                pMe->m_nTimeCtlCount = 2;
                pMe->m_nNumKeyCount = 4;
                ITIMECTL_SetEditField(pMe->m_pTime,ITF_SEC); 
                (void)ITIMECTL_Redraw(pMe->m_pTime);
            }
            else if(pMe->m_nTimeCtlCount == 2)
            {
                pMe->m_nTimeCtlCount = 0;
                pMe->m_nNumKeyCount = 0;
                ITIMECTL_SetEditField(pMe->m_pTime,ITF_HOUR); 
                (void)ITIMECTL_Redraw(pMe->m_pTime);
            }
            return TRUE;

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
            nKeyMask = wParam - AVK_0;
            break;

        default:
            break;
    }
    
    if(pMe->m_nNumKeyCount == 0)    //小时的十位
    {
        //小时的只能设置23小时
        if(nKeyMask == 0 || nKeyMask == 1 || nKeyMask == 2)
        {
            pMe->m_dwHour = 10* nKeyMask * MSSEC_PER_HOUR;
            pMe->m_dwDispTime = pMe->m_dwHour + pMe->m_dwMin + pMe->m_dwSec;

            (void)ITIMECTL_SetTime(pMe->m_pTime, pMe->m_dwDispTime);
             pMe->m_nTimeCtlCount = 0;
        }
        else
        {
            return TRUE;
        }
    }
    else if(pMe->m_nNumKeyCount == 1)   //小时的个位
    {
        if(nKeyMask == 0 || nKeyMask == 1 || nKeyMask == 2 || nKeyMask == 3||nKeyMask == 4||nKeyMask == 5
			||nKeyMask == 6||nKeyMask == 7||nKeyMask == 8||nKeyMask == 9)
        {
        	MSG_FATAL("pMe->m_dwHour===%d",pMe->m_dwHour,0,0);
        	if(nKeyMask == 4||nKeyMask == 5||nKeyMask == 6||nKeyMask == 7||nKeyMask == 8||nKeyMask == 9)
            {
            	if((pMe->m_dwHour)>19*MSSEC_PER_HOUR)
            	{
            		return TRUE;
            	}
            }
            pMe->m_dwHour += nKeyMask * MSSEC_PER_HOUR;
            pMe->m_dwDispTime = pMe->m_dwHour + pMe->m_dwMin  + pMe->m_dwSec;

            (void)ITIMECTL_SetTime(pMe->m_pTime, pMe->m_dwDispTime);

            //编辑区域自动设置为分钟
            ITIMECTL_SetEditField(pMe->m_pTime, ITF_MIN);
            (void)ITIMECTL_Redraw(pMe->m_pTime);
            pMe->m_nTimeCtlCount = 1;
        }
        else 
        {
            return TRUE;
        }
    }
    else if(pMe->m_nNumKeyCount == 2)   //分钟的十位
    {
        //分钟的十位不可以设置6789,否则设置的时间将超界
        if(nKeyMask != 6 && nKeyMask != 7
           && nKeyMask != 8 && nKeyMask != 9)
        {
            pMe->m_dwMin = 10 * nKeyMask * MSSEC_PER_MIN;
            pMe->m_dwDispTime = pMe->m_dwHour + pMe->m_dwMin + pMe->m_dwSec ;

            (void)ITIMECTL_SetTime(pMe->m_pTime, pMe->m_dwDispTime);
             pMe->m_nTimeCtlCount = 1;
        }
        else
        {
            return TRUE;
        }
    }
    else if(pMe->m_nNumKeyCount == 3)  //分钟的个位
    {
        pMe->m_dwMin += nKeyMask * MSSEC_PER_MIN;
        pMe->m_dwDispTime = pMe->m_dwHour + pMe->m_dwMin + pMe->m_dwSec;

        (void)ITIMECTL_SetTime(pMe->m_pTime, pMe->m_dwDispTime);

        //编辑区域自动设置为秒钟
        ITIMECTL_SetEditField(pMe->m_pTime, ITF_SEC);
        (void)ITIMECTL_Redraw(pMe->m_pTime);
        pMe->m_nTimeCtlCount = 2;
    }
    else if(pMe->m_nNumKeyCount == 4) //秒钟十位
    {
        if(nKeyMask != 6 && nKeyMask != 7
        && nKeyMask != 8 && nKeyMask != 9)
        {
            pMe->m_dwSec = 10 * nKeyMask * 1000;
            pMe->m_dwDispTime = pMe->m_dwHour + pMe->m_dwMin + pMe->m_dwSec ;

            (void)ITIMECTL_SetTime(pMe->m_pTime, pMe->m_dwDispTime);
            pMe->m_nTimeCtlCount = 2;
        }
        else
        {
            return TRUE;
        }
    }
    else if(pMe->m_nNumKeyCount == 5) //秒钟个位
    {
        pMe->m_dwSec += nKeyMask * 1000;
        pMe->m_dwDispTime = pMe->m_dwHour + pMe->m_dwMin + pMe->m_dwSec;

        (void)ITIMECTL_SetTime(pMe->m_pTime, pMe->m_dwDispTime);
        
        //自动设置控件焦点
        pMe->m_nNumKeyCount = 0;    //重置m_nNumKeyCount
        pMe->m_nTimeCtlCount = 0;  
        //编辑区域自动设置为时钟
        ITIMECTL_SetEditField(pMe->m_pTime, ITF_HOUR);
        (void)ITIMECTL_Redraw(pMe->m_pTime);
        (void)AppTimer_Redraw(pMe);
        return TRUE;    //重置了m_nNumKeyCount,直接返回
    }

    //时间控件按键次数增加
    pMe->m_nNumKeyCount++;
    //pMe->m_nTimeCtlCount++;
    (void)ITIMECTL_Redraw(pMe->m_pTime);
    (void)AppTimer_Redraw(pMe);
    return TRUE;
}
static void TimerNotifyMP3PlayerAlertEvent(CAppTimer *pMe, boolean toStartAlert)
{
 #ifdef FEATURE_APP_MUSICPLAYER
    if(GetMp3PlayerStatus() == MP3STATUS_RUNONBACKGROUND)
    {
        ISHELL_SendEvent(pMe->a.m_pIShell,
                         AEECLSID_APP_MUSICPLAYER,
                         EVT_ALARM,
                         toStartAlert,
                         0);
    }
 #endif
}

/*==============================================================================
函数：
       AppTimer_CanAlert
说明：
       函数用来确定是否可以唤起闹钟主界面，目前仅对通话情况做出判断，以后有其它
       要求可在此添加。

参数：
       pMe [in]：指向 AlarmClock Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       TRUE: 可以提示
       FALSE: 不可以提示

备注：

==============================================================================*/
#ifdef FEATURE_APP_RECORDER
extern boolean recorder_GetRunningFlag(void);
#endif
boolean AppTimer_CanAlert(CAppTimer *pMe)
{
#ifdef FEATURE_ICM
    ICM *pICM = NULL;
    uint16 num = 0;
    
    if(AEE_SUCCESS != ISHELL_CreateInstance(pMe->a.m_pIShell, 
                                            AEECLSID_CM, 
                                            (void **)&pICM))
    {
        return EFAILED;
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
   ITelephone *pITelephone = NULL;
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
	   
	   if(SUCCESS != ITELEPHONE_GetCalls(pITelephone, &po,sizeof(AEETCalls)))
	   {
		   return FALSE;
	   }
	   num = po.dwCount;
	   
	   ITELEPHONE_Release(pITelephone);
	   pITelephone = NULL;
   }
#endif
#ifdef FEATURE_APP_RECORDER
if(TRUE == recorder_GetRunningFlag())
{
        return FALSE;
}

#endif
    
    if(num > 0)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}



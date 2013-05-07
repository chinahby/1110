/*==============================================================================
// 文件：
//        CallDialogHandler.c
//        2007-11-01 Gemsea草拟版本(Draft Version)
//        版权所有(c) 2009 Anylook Tech. CO.,LTD.
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
#include "recentcalls.h"
#include "wmsapp.h"
#include "AEE_OEMEvent.h"
#ifdef FEATURE_SUPPORT_BT_APP
#include "Bcmapp_ag.h"
#endif

//Add By zzg 2011_12_09
#ifdef FEATURE_APP_MUSICPLAYER
#include "MusicPlayer.h"
#endif 
//Add End

#include "UTKUtils.h"

//Add By zzg 2010_08_03  for Frenduo Send sms
#ifndef AEE_SIMULATOR
#if defined(FEATURE_WMS_APP) && !defined(FEATURE_WMSAPP_ONLYSUPPORTVMAIL)
#include "WMSApp.h"
#endif
#endif
//Add End


//#define FEATURE_TEST_ANNUN_ICONS 
#ifdef FEATURE_TEST_ANNUN_ICONS
    //#include "oemannunciator.brh" 
    #include "appscommonimages.brh" 
#endif
#define EVT_MODE_CHANGE             1299
#include "AEERUIM.h"
/*==============================================================================
                                 宏定义和常数
==============================================================================*/

extern boolean	bIsPowerUp; //Add By zzg 2012_03_07

#if FEATURE_DIALER_ANIMAION_SUPPORT
// 来电动画图片文件定义
#define CALLAPP_CALLIN_ANI      "fs:/image/notice/callin.png"  

// 去电动画图片文件定义
#define CALLAPP_CALLOUT_ANI    "fs:/image/notice/callout.png"  

#endif
// 通话动画图片帧数
#define CALLAPP_ANI_FRAMES      (7)


//通话音量图片文件定义
#define REFUI_VOLUME_IMAGE         "fs:/image/notice/volume.png" 

// 通话音量图片帧数
#define REFUI_VOLUME_LEVELS        (6) 

// 定义界面背景颜色
#define CALLAPP_BG_COLOR          (RGB_BLACK) //(MAKE_RGB(0x66, 0x66, 0x66))

// 定义界面文本颜色           //added by chenxgiao 2009.01.20
#define CALLAPP_TEXT_COLOR       (RGB_WHITE)
/*==============================================================================
                                 类型定义
==============================================================================*/
static const DTMFToneDuration dtmfDuration[2] =
{
   // Normal duration
   { CAI_DTMF_ON_150, CAI_DTMF_OFF_100 },

   // Long duration
   { CAI_DTMF_ON_350, CAI_DTMF_OFF_200 }
};

#ifdef LCD_COLOR

static const RGBVAL gColorFont[11] =
{
    MAKE_RGB(0, 255, 0),// 青
    MAKE_RGB(3, 99, 239),// 浅兰
    MAKE_RGB(11, 11, 255),// 兰
    MAKE_RGB(153, 51, 255), // 紫
    MAKE_RGB(255, 25, 195),// 殷红
    MAKE_RGB(251, 40, 9), // 红
    MAKE_RGB(250, 79, 9),//棕色
    MAKE_RGB(240, 230, 0),// 黄
    MAKE_RGB(133, 180, 1),//草绿
    MAKE_RGB(45, 185, 45),// 绿
    MAKE_RGB(1, 255, 11) // 青
};
#endif //#ifdef LCD_COLOR

/*==============================================================================
                                 函数声明
==============================================================================*/
// 对话框 IDD_DIALER 事件处理函数
static boolean  CallApp_Dialer_NumEdit_DlgHandler(CCallApp *pMe,
                                        AEEEvent   eCode,
                                        uint16     wParam,
                                        uint32     dwParam);

// 对话框 IDD_CALLING 事件处理函数
static boolean  CallApp_Dialer_Calling_DlgHandler(CCallApp *pMe,
                                        AEEEvent   eCode,
                                        uint16     wParam,
                                        uint32     dwParam);

// 对话框 IDD_CONVERSATION 事件处理函数
static boolean  CallApp_Dialer_Connect_DlgHandler(CCallApp *pMe,
                                        AEEEvent   eCode,
                                        uint16     wParam,
                                        uint32     dwParam);

// 对话框 IDD_ENDCALL 事件处理函数
static boolean  CallApp_Dialer_Callend_DlgHandler(CCallApp *pMe,
                                        AEEEvent   eCode,
                                        uint16     wParam,
                                        uint32     dwParam);

//Add By zzg 2012_10_31
// 对话框 IDD_PROMPT事件处理函数
static boolean  CallApp_ProMpt_DlgHandler(CCallApp *pMe,
                                        AEEEvent   eCode,
                                        uint16     wParam,
                                        uint32     dwParam);
//Add End

// 对话框 IDD_MSGBOX 事件处理函数
static boolean  CallApp_MsgBox_DlgHandler(CCallApp *pMe,
                                        AEEEvent   eCode,
                                        uint16     wParam,
                                        uint32     dwParam);

// 对话框 IDD_INCOMINGCALL 事件处理函数
static boolean  CallApp_IncomingCall_DlgHandler(CCallApp *pMe,
                                        AEEEvent   eCode,
                                        uint16     wParam,
                                        uint32     dwParam);

static void CallApp_IncomingCall_Dlg_Init(CCallApp *pMe);

static boolean CallApp_pwd_dialog_handler(CCallApp *pMe,
                                            AEEEvent    eCode,
                                            uint16      wParam,
                                            uint32      dwParam);



// 对话框 IDD_MISSED 事件处理函数
static boolean  CallApp_Missedcall_DlgHandler(CCallApp *pMe,
                                        AEEEvent   eCode,
                                        uint16     wParam,
                                        uint32     dwParam);

#ifdef FEATURE_UTK2
// 对话框 IDD_USERCONFIRM 事件处理函数
static boolean  IDD_USERCONFIRM_Handler(CCallApp *pMe,
                                        AEEEvent   eCode,
                                        uint16     wParam,
                                        uint32     dwParam);
#endif

//快速拨号函数
static void CallApp_MakeSpeedDialCall(CCallApp  *pMe);

static void CallApp_MakeVoiceMailCall(CCallApp  *pMe);

// show  IDD_DIALER bar
static void CallApp_Draw_NumEdit_SoftKey(CCallApp *pMe);

static void CallApp_Build_NumEdit_Option_Menu(CCallApp *pMe,IMenuCtl   *pMenuCtl );

static void CallApp_Build_Connect_Option_Menu(CCallApp *pMe);

#ifdef FEATURE_LARGE_DIALING_DIGITS
//display the dialing digits
static void CallApp_DrawDialerString(CCallApp *pMe,AECHAR const *dialStr);
#endif
//Converts  AVKType to the ASCII equivalent
static char CallApp_AVKType2ASCII(AVKType key);
static void CallApp_keypadtimer(void *pUser);
static boolean CallApp_FindMemoryCardExist(CCallApp *pMe);  //Add by pyuangui 2013-01-10
static AECHAR CallApp_AVKSTAR_2ASCII(CCallApp *pMe);

//Determines the current DTMF tone length
static const DTMFToneDuration *CallApp_GetDTMFDuration(CCallApp *pMe);
//Invokes the Contacts applet to save a phone number
static boolean CallApp_SaveNumber(CCallApp *pMe, PhoneSaveType saveType);
//dialog expired callback function
static void CallApp_HandleDialogTimer(void *pUser);

//Answers an in-band call
static void CallApp_AnswerInbandCall(CCallApp *pMe);

// setup the IDD_CONVERSATION softkeys
static void CallApp_Draw_Connect_Softkey(CCallApp *pMe);

//Process the screen display when conversatin
static void CallApp_Draw_Connect_Time(void *pUser);

//Play a list of DTMF tones to the user
static void CallApp_PlayToneList(CCallApp *pMe,char *digits, byte  len);

//set the phone number that is displayed when conversation
static void CallApp_Draw_Connect_Number_and_Name(CCallApp *pMe);

//Timer callback to flash the call timer.
//static void CallApp_HandleEndCallBlinkTimer(void *pUser);

//进入未接电话列表
static boolean CallApp_MissedList(CCallApp *pMe);

//来电时限制按键速度
//static void CallApp_IncomingCall_IsReady_Timer(void *pme);

// Speed dial call function
static int CallApp_SpeedDialCall(CCallApp *pMe, AECHAR *pSpeedDial);
//显示听筒声音图片函数
static void CallApp_RefreshVolBar(CCallApp *pMe);

#ifdef FEATURE_APP_PAUSE_TIMER
static void  CallApp_SetTimerControl(void *pUser);
#endif //FEATURE_APP_PAUSE_TIMER


//Add By zzg 2010_08_03
#ifdef FEATURE_FRENDUO
static void  CallApp_SetFrenduoTimer(void *pUser);
static boolean  CallApp_SendFrenduoSMS(void);
#endif 
//Add End

static void  CallApp_SetPauseControl(void *pUser);

static void CallApp_ConvReleasePKey(CCallApp *pMe);

static void CallApp_HandleStopMissedAlertTimer(void *pUser);

static void CallApp_ShortcutQuiet(CCallApp *pMe);
static void CallApp_Dialer_Show_Animation(void *pUser);

static void CallApp_Flash_Call(CCallApp *pMe);

static void CallApp_StartContDTMF(CCallApp *pMe,AECHAR *c_dtmf);

static void CallApp_BurstDTMF(CCallApp *pMe,AECHAR *dtmf,int len);
#ifdef FEATURE_ICM
static void CallApp_Stop_ContDTMF(ICM *m_pICM);
#else
static void CallApp_Stop_ContDTMF(CCallApp *pMe);
#endif

static boolean CallApp_NotifyDisp(CCallApp *pMe);

static boolean CallApp_Search_HasExceptionalChar(AECHAR* szCallNumber);

static void CallApp_Set_Db_In_Idle(boolean val);

//static void CallApp_Draw_Softkey(CCallApp *pMe,uint16 lsk,uint16 rsk);

static boolean CallApp_Process_HeldKey_Event(CCallApp *pMe,
                                           AEEEvent    eCode,
                                           uint16      wParam,
                                           uint32      dwParam);

#if defined(FEATURE_VERSION_W027V3) || defined(FEATURE_VERSION_W317A)||defined(FEATURE_VERSION_M74)

static void CallApp_TorchTipTimeOut(CCallApp *pMe);
#endif

static void CallApp_Process_Spec_Key_Event(CCallApp *pMe,uint16 wp);

//static void CallApp_Process_Spec_Key_Event_CB(CCallApp *pMe);

static boolean CallApp_DrawText_Ex(CCallApp *pMe, AEEFont fnt,
                  AECHAR *wStr, AEERect  *rect,  uint32 mask);

//static void CallApp_NotifyDisp_CB(CCallApp *pMe);

static boolean CallApp_Process_Send_Key_Release_Event(CCallApp *pMe);
static void CallApp_HandleDialogTimer_Redraw(void *pUser);
#ifdef FEATRUE_SET_IP_NUMBER
static boolean CallApp_Show_Ip_Number_DlgHandler(CCallApp *pMe,
                                           AEEEvent    eCode,
                                           uint16      wParam,
                                           uint32      dwParam);
static boolean CallApp_Process_Ip_Call_Key_Press(CCallApp *pMe,IMenuCtl *pMenu);
#endif /*FEATRUE_SET_IP_NUMBER*/

static void CallApp_Play_Incoming_Tone(CCallApp *pMe);
#ifdef FEATURE_IMAGE_DIALING_DIGITS
static void CallApp_Load_Numer_Img(CCallApp *pMe);
static void CallApp_Release_Numer_Img(CCallApp *pMe);
static void CallApp_Draw_Numer_Img(CCallApp   *pMe,  AECHAR const *dialStr);
#endif//#ifdef FEATURE_IMAGE_DIALING_DIGITS
static void CallApp_Display_Number(CCallApp *pMe);
static void CallApp_Send_DTMF(CCallApp *pMe , AECHAR *w_str );
static void notifyFMRadioAlertEvent( CCallApp *pMe, boolean toStartAlert);


#if defined(FEATURE_WMS_APP) && !defined(FEATURE_WMSAPP_ONLYSUPPORTVMAIL)
static boolean CallApp_SendSMS(CCallApp *pMe);
#endif

#if defined( FEATURE_CALL_RECORDER)
static void CallApp_Dialer_Connect_Turn_On_Recorder( CCallApp* pme);
static void CallApp_Dialer_Connect_Turn_Off_Recorder( CCallApp* pme);
static void CallApp_Dialer_Connect_Record_Reverse( CCallApp* pme);
static void CallApp_Dialer_Connect_Record_Forward( CCallApp* pme);
static void recorder_show_media_spec( CCallApp* ref, RGBVAL color);
#endif

#ifdef FEATURE_EDITABLE_NUMBER
// 计算绘制光标的矩形框
static void CallApp_Calc_Cursor_Rect(CCallApp* pMe, AEERect *pRect);
// 绘制光标
static void CallApp_Draw_Cursor(CCallApp* pMe, AEERect *pRect);
// 设置光标闪烁
static void CallApp_Set_Cursor_Blink(void* pUser);
#endif
static void CallApp_ProcessUIMMMIStr(CCallApp* pMe, AECHAR *pStr);


//Add By zzg 2011_11_09
#ifdef FEATURE_SUPPORT_BT_AUDIO
static boolean CallApp_CheckBtHeadSetInUsing(CCallApp* pMe);
#endif
//Add End



#ifdef FEATURE_LCD_TOUCH_ENABLE
static void CallApp_Draw_BackGround(CCallApp* pMe, AEERect * rect);
static void callApp_draw_penup(CCallApp* pMe,int16 x,int16 y);
static void callApp_draw_pendown(CCallApp* pMe,int16 x,int16 y);
static void callApp_draw_penmove(CCallApp* pMe,int16 x,int16 y);
#endif

/*==============================================================================
                                 全局数据
==============================================================================*/


/*==============================================================================
                                 本地（静态）数据
==============================================================================*/
    
#ifdef FEATURE_EDITABLE_NUMBER
static boolean bDrawCursor = TRUE;
#endif


/*==============================================================================
                                 函数定义
==============================================================================*/
#ifdef FEATURE_MORSE
#include "morse.brh"
#define  AEE_MORSE_RES_FILE AEE_RES_LANGDIR MORSE_RES_FILE

//#include "morse_priv.h"
    extern boolean gMorseSendingFlag;
    /*==============================================================================
    函数:
           Morse_DialogTimeout
    
    说明:
           定时器回调函数。主要用于自动关闭消息对话框。
    
    参数:
           pme [in]：void *类型指针，暗指CCallApp结构指针。
    
    返回值:
           无。
    
    备注:
    
    ==============================================================================*/
    static void Morse_DialogTimeout(void *pme)
    {
        CCallApp *pMe = (CCallApp *)pme;
    
        if (NULL == pMe)
        {
            return;
        }
        (void) ISHELL_PostEvent(pMe->m_pShell,
                                AEECLSID_DIALER,
                                EVT_DISPLAYDIALOGTIMEOUT,
                                0,
                                0);
       //CLOSE_DIALOG(DLGRET_NUMEDIT);
    }
	
    static boolean Morse_PopPrompt(CCallApp *pMe,uint16 id)
    {

            static IStatic * pStatic = NULL;
            PromptMsg_Param_type  Msg_Param={0};
            AECHAR  wstrText[MSGBOX_MAXTEXTLEN] = {(AECHAR)'\0'};
           if (NULL == pStatic)
           {
                AEERect rect = {0};
                if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                                         AEECLSID_STATIC,
                                                         (void **)&pStatic))
                
                {
                    return FALSE;
                    ERR("ISHELL_CreateInstance,AEECLSID_STATIC 2",0,0,0);
                }        
                ISTATIC_SetRect(pStatic, &rect);  
           }
          
            // 从资源文件取消息内容
      (void)ISHELL_LoadResString(pMe->m_pShell,
                                AEE_APPSCALLAPP_RES_FILE,                                
                                id,
                                wstrText,
                                sizeof(wstrText));
                            
            Msg_Param.ePMsgType = MESSAGE_WARNNING;
            Msg_Param.pwszMsg = wstrText;
            Msg_Param.eBBarType = BTBAR_NONE;
            DrawPromptMessage(pMe->m_pDisplay, pStatic, &Msg_Param);
            IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE); 
            return TRUE;
    }
#endif

/*==============================================================================
函数:
       CallApp_ShowDialog
说明:
       函数由状态处理函数用来创建相应对话框资源ID的对话框。

参数:
       pMe [In]: 指向CallApp Applet对象结构的指针,该结构包含小程序的特定信息.
       dlgResId [in]：对话框资源ID，函数根据该ID创建对话框。

返回值:
       无

备注:


==============================================================================*/
void CallApp_ShowDialog(CCallApp *pMe,uint16  dlgResId)
{
    int nRet;
#ifdef DIALER_MEM_CHECK
    CallApp_show_Mem(pMe);
#endif
    // At most one dialog open at once
    if (ISHELL_GetActiveDialog(pMe->m_pShell) != NULL)
    {
        // Looks like there is one dialog already opened.
        // Flag an error an return without doing anything.
        CALL_ERR("Trying to create a dialog without closing the previous one",0,0,0);
        return;
    }
    
    nRet = ISHELL_CreateDialog(pMe->m_pShell,AEE_APPSCALLAPP_RES_FILE,dlgResId,NULL);

    if (nRet != SUCCESS)
    {
        CALL_ERR("Failed to create the dialog in the CallApp applet",0,0,0);
    }
}

/*===============================================================================
函数:
       CallApp_RouteDialogEvent
说明:
       函数将BREW事件路由给当前活动对话框的事件处理函数。

参数:
       pMe [In]: 指向CallApp Applet对象结构的指针,该结构包含小程序的特定信息.
       eCode [in]：事件代码。
       wParam [in]：与事件eCode关联的数据。
       dwParam [in]：与事件eCode关联的数据。

返回值:
       TRUE：传入事件被处理。
       FALSE：传入事件没被处理。

备注:

==============================================================================*/
boolean CallApp_RouteDialogEvent(CCallApp *pMe,
                                   AEEEvent    eCode,
                                   uint16      wParam,
                                   uint32      dwParam)
{
    if (NULL == pMe)
    {
        return FALSE;
    }

    //CALL_ERR("CallApp_RouteDialogEvent",0,0,0);
    if (NULL == pMe->m_pActiveDlg)
    {
        return FALSE;
    }


    //CALL_ERR("The active dialog is  %d dialog",pMe->m_pActiveDlgID,0,0);
    switch (pMe->m_pActiveDlgID)
    {
        case IDD_NUMEDIT:
            return CallApp_Dialer_NumEdit_DlgHandler(pMe,eCode,wParam,dwParam);

        case IDD_CALLING:
            return CallApp_Dialer_Calling_DlgHandler(pMe,eCode,wParam,dwParam);

        case IDD_CONVERSATION:
            return CallApp_Dialer_Connect_DlgHandler(pMe,eCode,wParam,dwParam);

        case IDD_ENDCALL:
            return CallApp_Dialer_Callend_DlgHandler(pMe,eCode,wParam,dwParam);

		//Add By zzg 2012_10_31
		case IDD_PROMPT:
			return CallApp_ProMpt_DlgHandler(pMe,eCode,wParam,dwParam);
		//Add End
        case IDD_MSGBOX:
            return CallApp_MsgBox_DlgHandler(pMe,eCode,wParam,dwParam);

        case IDD_INCOMINGCALL:
            return CallApp_IncomingCall_DlgHandler(pMe,eCode,wParam,dwParam);

        case IDD_PWD:
            return CallApp_pwd_dialog_handler(pMe,eCode,wParam,dwParam);
            
        case IDD_MISSED:
            return CallApp_Missedcall_DlgHandler(pMe,eCode,wParam,dwParam);

#ifdef FEATURE_UTK2
        case IDD_USERCONFIRM:
            return IDD_USERCONFIRM_Handler(pMe,eCode,wParam,dwParam);
#endif
#ifdef FEATRUE_SET_IP_NUMBER
        case IDD_IP_NUMBER:
            return CallApp_Show_Ip_Number_DlgHandler(pMe,eCode,wParam,dwParam);
#endif /*FEATRUE_SET_IP_NUMBER*/
        default:
            return FALSE;
    }
}
/*==============================================================================
函数：
       CallAppDialerDlgHandler
说明：
       IDD_DIALER 对话框事件处理函数

参数：
       pMe [in]：指向CallApp Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/
static boolean  CallApp_Dialer_NumEdit_DlgHandler(CCallApp *pMe,
                                          AEEEvent    eCode,
                                          uint16      wParam,
                                          uint32      dwParam)//This is dialerapp numedit dialog
{
    boolean vol_add =FALSE;

    MSG_FATAL("eCode= %x,w=%x,dw=%x CallApp_Dialer_NumEdit_DlgHandler ",eCode,wParam,dwParam);

    if ( pMe->m_bShowPopMenu && IMENUCTL_IsActive(pMe->m_pMenu) )
    {
        if ( IMENUCTL_HandleEvent(pMe->m_pMenu, eCode, wParam, dwParam) )
        {
            return TRUE;
        }
    }

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
		{
#ifndef FEATURE_ICM
			AEETCalls po;			
			if(SUCCESS != ITELEPHONE_GetCalls(pMe->m_pITelephone, &po,sizeof(AEETCalls)))
			{
				return FALSE;
			}
#endif
            pMe->m_btime_out     = 0;
            pMe->m_return_value  = RETURN_ZERO;
            pMe->m_bShowPopMenu  = FALSE;
            pMe->m_bprocess_held = FALSE;
            //ICONFIG_GetItem(pMe->m_pConfig,  CFGI_KEYTONE_LENGTH,&dtmflen,sizeof(keyToneLength));
            ISHELL_CancelTimer(pMe->m_pShell, (PFNNOTIFY)CallApp_MakeCall, pMe);
            pMe->m_b_auto_redial = FALSE;
            pMe->m_auto_redial_count = 0;
#ifdef FEATURE_ICM
            pMe->m_b_incall      = AEECM_IS_VOICECALL_CONNECTED(pMe->m_pICM);
#else
            pMe->m_b_incall = po.dwCount>0?TRUE:FALSE;
#endif
#ifdef FEATURE_EDITABLE_NUMBER
            pMe->m_nCursorPos = 0;
#endif
            MSG_FATAL("CallApp_Dialer_NumEdit_DlgHandler %d",pMe->m_b_incall,0,0);
            if(!pMe->m_b_incall)
            {
#ifdef FEATRUE_SET_IP_NUMBER
                IP_Number_Cfg  ip_cfg;
                MEMSET((void*)&ip_cfg,0 , sizeof(IP_Number_Cfg));
                pMe->m_b_have_set_ip = FALSE;
                ICONFIG_GetItem(pMe->m_pConfig,CFGI_IP_NUMBER,(void *)&ip_cfg,sizeof(IP_Number_Cfg));
                if(ip_cfg.ip_count>0)
                {
                    pMe->m_b_have_set_ip = TRUE;
                }
#endif /*FEATRUE_SET_IP_NUMBER*/

/*#ifdef FEATURE_IMAGE_DIALING_DIGITS
                CallApp_Load_Numer_Img(pMe);
#endif*//*FEATURE_IMAGE_DIALING_DIGITS*/
            }
            
#ifdef FEATURE_IMAGE_DIALING_DIGITS
            CallApp_Load_Numer_Img(pMe);
#endif/*FEATURE_IMAGE_DIALING_DIGITS*/
            return TRUE;
        }
        case EVT_DIALOG_START:
            if(pMe->m_pIAnn != NULL)
            {
                //拨号盘不能显示titlebar
                IANNUNCIATOR_SetHasTitleText(pMe->m_pIAnn, FALSE);
                IANNUNCIATOR_Redraw(pMe->m_pIAnn);
            }
			MSG_FATAL("EVT_DIALOG_START.........................",0,0,0);
			#ifndef FEATURE_LCD_TOUCH_ENABLE
            if (WSTRLEN(pMe->m_DialString) > 0) //copy from the start info
            #endif
            {
                pMe->m_bShowPopMenu = FALSE; // flag of pop menu
                if((0 == WSTRCMP(pMe->m_DialString, L"0") )||(0 == WSTRCMP(pMe->m_DialString, L"#") )||(0 == WSTRCMP(pMe->m_DialString, L"*") ))
                {                	
                    pMe->m_btime_out = MAX_COUNT_TO_CHANGE;
                    //ISHELL_SetTimer(pMe->m_pShell,
                    //                                        500,
                    //                                        (PFNNOTIFY)CallApp_Process_Spec_Key_Event_CB,
                    //                                        (void *)pMe);
                }
                (void) ISHELL_PostEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_USER_REDRAW,0,0);

            }

#if defined( FEATURE_CALL_RECORDER)
        	pMe->m_bSettingVolume = FALSE;
#endif
            return TRUE;

        case EVT_FLIP:
            //CALL_ERR("Connect EVT_FLIP %d", wParam, 0, 0);
            if(pMe->m_b_incall)
            {
                CallApp_Process_EVT_FLIP_Event(pMe,wParam);
            }
            return TRUE;

        case EVT_USER_REDRAW:          
#if defined( FEATURE_CALL_RECORDER)
        	if( pMe->m_bShowPopMenu)
        	{
        		return TRUE;
        	}
#endif

#ifdef FEATURE_LCD_TOUCH_ENABLE
			CallApp_Draw_BackGround(pMe,&pMe->m_rc);
#else
            Appscommon_ResetBackgroundEx(pMe->m_pDisplay, &pMe->m_rc, TRUE);
#endif
			CallApp_Draw_NumEdit_SoftKey(pMe);
            CallApp_Display_Number(pMe);

#ifdef WIN32//wlh for virtualkey
			IVKEYCTL_SetKeyPad(pMe->m_pIVkeyCtl, &pMe->m_rc, AEE_VKEY_DIALNUMBER);
			IVKEYCTL_SetActive(pMe->m_pIVkeyCtl, TRUE);			
#endif
			IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
#if defined( FEATURE_CALL_RECORDER)
        //modi by pyuangui 20121231  为防止在输入界面重叠 不画录音提示和时间
        #if defined(FEATURE_VERSION_W317A) || defined(FEATURE_VERSION_C316)  
        #else
		if( pMe->m_bRecorderOn && !pMe->m_bShowPopMenu)
        {
        	recorder_show_media_spec( pMe, RGB_WHITE);
        }
        #endif
		
        if( pMe->m_bSettingVolume && pMe->m_b_incall)
        {
			CallApp_RefreshVolBar( pMe);
        }
#endif

            if(pMe->m_b_incall)
            {
                IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_CALL/*ANNUN_FIELD_CALLFORWARD*/, ANNUN_STATE_CALL_INUSE_ON/*ANNUN_STATE_ON*/);
            }
            else
            {
                pMe->m_bHandFree  = FALSE;//IDLE default handfree is false
                pMe->m_CallMuted  = FALSE;
                IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_CALL/*ANNUN_FIELD_CALLFORWARD*/, ANNUN_STATE_CALL_INUSE_OFF/*ANNUN_STATE_OFF*/);
            }
#ifdef FEATURE_EDITABLE_NUMBER
#if defined( FEATURE_CALL_RECORDER)
            if(!(pMe->m_bSettingVolume && pMe->m_b_incall))
#endif
            CallApp_Set_Cursor_Blink (pMe);
#endif

            IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            return TRUE;

        case EVT_DIALOG_END:
			MSG_FATAL("EVT_DIALOG_END.....123....",0,0,0);
            if(pMe->m_pIAnn != NULL)
            {
                //退出拨号盘时，恢复titlebar
                IANNUNCIATOR_SetHasTitleText(pMe->m_pIAnn, TRUE);
            }
            // TBD - dial string format should be typedef'd
#ifdef KEYSND_ZY
            ISOUNDPLAYER_Stop(pMe->m_SndPlayer);
#endif /* KEYSND_ZY */

            // 防止有来电时，POP menu还是active
            MSG_FATAL("EVT_DIALOG_END.....222....",0,0,0);
            pMe->m_bShowPopMenu = FALSE;
            IMENUCTL_SetActive ( pMe->m_pMenu, FALSE );
            ISHELL_CancelTimer(pMe->m_pShell,
                                                    (PFNNOTIFY)CallApp_HandleDialogTimer_Redraw,
                                                    pMe);
#ifdef FEATURE_IMAGE_DIALING_DIGITS
            //if(!pMe->m_b_incall)
            {
                CallApp_Release_Numer_Img(pMe);
            }
#endif//#ifdef FEATURE_IMAGE_DIALING_DIGITS
            if (!pMe->m_b_incall)
            {// 此时可能有来电，但未接通，需进一步判断是否有来电以确认是否备份用户拨号
                db_items_value_type  db_item;
                
                db_get(DB_IN_USE, &db_item);
                if ((db_item.in_use) && (WSTRLEN(pMe->m_DialString)>0))
                {
                    FREEIF(pMe->m_pwstrDialStringkeep);
                    pMe->m_pwstrDialStringkeep = WSTRDUP(pMe->m_DialString);
                }
            }
#ifdef FEATURE_EDITABLE_NUMBER
            ISHELL_CancelTimer(pMe->m_pShell, CallApp_Set_Cursor_Blink, pMe);
#endif
#ifdef FEATURE_EDITABLE_RECORD
            pMe->m_bEditRecNumber = FALSE;
#endif
			MSG_FATAL("EVT_DIALOG_END......3333...",0,0,0);
            return TRUE;

        case EVT_COMMAND:
            MSG_FATAL("EVT_COMMANDpMe->m_bShowPopMenu==%d",pMe->m_bShowPopMenu,0,0);
            if (TRUE == pMe->m_bShowPopMenu) //(pMe->m_b_incall )
            {
                pMe->m_bShowPopMenu = FALSE;
                IMENUCTL_SetActive ( pMe->m_pMenu, FALSE );
                (void)ISHELL_SendEvent( pMe->m_pShell, AEECLSID_DIALER, EVT_USER_REDRAW,0, 0 );
                switch (wParam)
                {
                    case IDS_HANDFREE:
                    case IDS_NONHANDFREE:
                        pMe->m_bHandFree = !pMe->m_bHandFree;
                        CallApp_SetupCallAudio(pMe);
                        return TRUE;
#ifdef FEATURE_SUPPORT_BT_APP
                    case IDS_USE_BT_AG:
                    case IDS_USE_PHONE:
                        if(wParam == IDS_USE_PHONE)
                        {
                            pMe->m_bt_audio = SEND_NONE;
                        }
                        else
                        {
                            pMe->m_bt_audio = SEND_BT_AUDIO;
                        }
                        pMe->m_bHandFree = FALSE;
                        CallApp_SetupCallAudio(pMe);
                        break;
#endif

//Add By zzg 2011_10_28
#ifdef FEATURE_SUPPORT_BT_AUDIO
					case IDS_USE_BT_AG:
					case IDS_USE_PHONE:
					{
						if (wParam == IDS_USE_PHONE)
						{
							MSG_FATAL("***zzg CallApp_Dialer_Connect IDS_USE_PHONE***", 0, 0, 0);

							CallApp_SwitchCallAudio(pMe, FALSE);							
	                    }
	                    else
	                    {
	                    	MSG_FATAL("***zzg CallApp_Dialer_Connect IDS_USE_BT_AG***", 0, 0, 0);

							CallApp_SwitchCallAudio(pMe, TRUE);							
	                    }
						pMe->m_bHandFree = FALSE;
						CallApp_SetupCallAudio(pMe);
						break;
					}
#endif
//Add End

                     case IDS_MUTE:
                     case IDS_UNMUTE:
//Add by pyuangui 2013-01-17					
#ifdef FEATURE_VERSION_C337
                     if(wParam == IDS_MUTE) 
					 	IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_CALL/*ANNUN_FIELD_MUTE*/, ANNUN_STATE_CALL_MUTE_ON/*ANNUN_STATE_ON*/);
					 else
					 	IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_CALL/*ANNUN_FIELD_MUTE*/, ANNUN_STATE_CALL_MUTE_OFF/*ANNUN_STATE_OFF*/);
#endif
//Add End    					 	
                         pMe->m_CallMuted = !pMe->m_CallMuted;
                         CallApp_SetupCallAudio(pMe);
                         CallApp_Draw_Connect_Softkey(pMe);
                         return TRUE;

                     case IDS_SK_MENU:
                        CallApp_LaunchApplet(pMe,  AEECLSID_MAIN_MENU);
                        return TRUE;
                    
                     case IDS_SAVE_NEW:
                        if(WSTRLEN(pMe->m_DialString) > (32))
                        {
                            CLOSE_DIALOG(DLGRET_NUM_TOO_LONG);
                            return TRUE;
                        }
                        else
                        {
                            return CallApp_SaveNumber(pMe, SAVE_NUMBER);
                        }

                      case IDS_SAVE_TO_CONT:
                      	if(WSTRLEN(pMe->m_DialString) > (20))
                        {
                            CLOSE_DIALOG(DLGRET_NUM_TOO_LONG);
                            return TRUE;
                        }
                        else
                        {
                            return CallApp_SaveNumber(pMe, ADD_FIELD);
                        }
						CLOSE_DIALOG(DLGRET_OK)
                        return TRUE;
                        
                      case IDS_SEND_MESSAGE:
#if defined(FEATURE_WMS_APP) && !defined(FEATURE_WMSAPP_ONLYSUPPORTVMAIL)					  	
                        return CallApp_SendSMS(pMe);
#endif
							break;
                        
                      case IDS_CALL:
                        return CallApp_Process_Send_Key_Release_Event(pMe);
#ifdef FEATURE_SUPPORT_WAP_APP
#ifdef FEATURE_CARRIER_CHINA_TELCOM
                      case IDS_LINK:
                        {
                            if(WSTRLEN(pMe->m_DialString) > 11)
                            {
                                //show error message
                                CLOSE_DIALOG(DLGRET_INVALID_LINK_NUM);
                            }
                            else
                            {
                                //link WAP browser
                                char strURL[128] = {"http://digital.tele-info.com.cn/digiaddr/DigitalAddressProc.jsp?DA="};
                                char nTempNumber[16] = {0};
                                int   nLength = STRLEN(strURL);
                                
                                (void)WSTRTOSTR(pMe->m_DialString, nTempNumber, sizeof(nTempNumber));
                                STRCPY(strURL + nLength, nTempNumber);
                                ISHELL_PostURL(pMe->m_pShell, strURL);
                            }
                            return TRUE;
                        }
#endif
#endif

#if defined( FEATURE_CALL_RECORDER)
                case IDS_OPTION_RECORDER_ON:
                case IDS_OPTION_RECORDER_OFF:
                {
                	pMe->m_bRecorderOn = wParam == IDS_OPTION_RECORDER_ON;

                	if( pMe->m_bRecorderOn)
                	{
						CallApp_Dialer_Connect_Turn_On_Recorder( pMe);
                	}
                	else
                	{
                		CallApp_Dialer_Connect_Turn_Off_Recorder( pMe);
                	}
                }
                break;
#endif

                    
                    default:
                        break;
                }
            }
            return FALSE;

        case EVT_KEY_HELD:
            CallApp_Process_HeldKey_Event(pMe,eCode,wParam,dwParam);
#ifdef FEATURE_MORSE
            if(((AVKType)wParam == AVK_5)&&(gMorseSendingFlag == TRUE))
            {
                (void) ISHELL_SetTimer(pMe->m_pShell,
                                1000,
                                Morse_DialogTimeout,
                                pMe); 
            }
#endif
            return TRUE;
#ifdef FEATURE_LCD_TOUCH_ENABLE
		case EVT_PEN_DOWN:
		{
			
			int16 wXPos   = (int16)AEE_GET_X(dwParam);
			int16 wYPos   = (int16)AEE_GET_Y(dwParam);
			if(pMe->m_bShowPopMenu)
			{
				return TRUE;
			}
			callApp_draw_pendown(pMe,wXPos,wYPos);
			return TRUE;
		}
		break;
		case EVT_PEN_MOVE:
		{
			int16 wXPos   = (int16)AEE_GET_X(dwParam);
			int16 wYPos   = (int16)AEE_GET_Y(dwParam);
			callApp_draw_penmove(pMe,wXPos,wYPos);
			if(pMe->m_bShowPopMenu)
			{
				return TRUE;
			}
			return TRUE;
		}
		break;
		case EVT_PEN_UP:
		{
			
			int16 wXPos = (int16)AEE_GET_X(dwParam);
			int16 wYPos = (int16)AEE_GET_Y(dwParam);
			if(pMe->m_bShowPopMenu)
			{
				return TRUE;
			}
			pMe->m_penup = TRUE;
			
			callApp_draw_penup(pMe,wXPos,wYPos);
			return TRUE;
		}
		break;
		case EVT_USER:
			switch(wParam)
			
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
				case AVK_STAR:
				case AVK_POUND:
				case AVK_CLR:
				case AVK_SELECT:
				case AVK_INFO:
				{
					eCode = EVT_KEY;
					return CallApp_Dialer_NumEdit_DlgHandler(pMe,eCode,wParam,dwParam);
				}
		    }
			MSG_FATAL("EVT_USER..................",0,0,0);
			
			break;
#endif
        
#ifdef FEATURE_MORSE
        case EVT_DISPLAYDIALOGTIMEOUT:
            ISHELL_CancelTimer(pMe->m_pShell,Morse_DialogTimeout,(void*)pMe);
            CLOSE_DIALOG(DLGRET_OK);
            return TRUE;
#endif
        case EVT_KEY_RELEASE:
#if defined(FEATURE_VERSION_C306)||defined(FEATURE_VERSION_W0216A) || defined(FEAUTRE_VERSION_N450)|| defined(FEATURE_VERSION_N021)|| defined(FEATURE_VERSION_C01)|| defined(FEATURE_VERSION_W516) || defined(FEATURE_VERSION_W208S) || defined(FEATURE_VERSION_W027)||defined(FEATURE_VERSION_S600S)
            {
				nv_item_type	SimChoice;
				OEMNV_Get(NV_SIM_SELECT_I,&SimChoice);
				if(SimChoice.sim_select==AVK_SEND_TWO)
				{
					if(AVK_CAMERA == (AVKType)wParam && !pMe->m_bprocess_held)
            		{
                		return CallApp_Process_Send_Key_Release_Event(pMe);
            		}
				}
			}
#endif
#if defined(FEATURE_VERSION_C01) 
			{
				nv_item_type	SimChoice;
				OEMNV_Get(NV_SIM_SELECT_I,&SimChoice);
				if(SimChoice.sim_select==AVK_SEND_TWO && AVK_SEND == (AVKType)wParam )
				{
					return TRUE;
				}
			}
#endif
            if(AVK_SEND == (AVKType)wParam && !pMe->m_bprocess_held)
            {
            	MSG_FATAL("AVK_SEND....................................",0,0,0);
                return CallApp_Process_Send_Key_Release_Event(pMe);
            }

            if (pMe->m_b_incall) //DTMF
            {
                //keyToneLength len;

                //(void) ICONFIG_GetItem(pMe->m_pConfig,
                //                                        CFGI_KEYTONE_LENGTH,
                //                                        &len,
                //                                        sizeof(keyToneLength));

                if (OEMNV_KEYTONE_LONG == pMe->m_dtmf_length)
                {
                    switch ((AVKType)wParam)
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
                        case AVK_STAR:
                        case AVK_POUND:
#ifdef FEATURE_ICM
                            CallApp_Stop_ContDTMF(pMe->m_pICM);
#else
                            CallApp_Stop_ContDTMF(pMe);
#endif
                            return TRUE;
                        default:
                            break;
                    }
                }
            }
            else/*STATE_IDLE*/
            {
                switch ((AVKType)wParam)
                {
                    //case AVK_0:
                        //TBD
                        //break;

                    case AVK_POUND:
#ifdef FEATURE_RESCUE_INVALID_RUIM
                        if (WSTRCMP(pMe->m_DialString, L"953047#") == 0 && pMe->idle_info.uimLocked)
                        {
                            boolean bData = 0;
                            AECHAR temp[40];
                            RGBVAL oldColor = 0;
                            
                            (void) ICONFIG_GetItem ( pMe->m_pConfig,
                                                                        CFGI_LOCK_RUIM,
                                                                        &bData,
                                                                        sizeof(bData));
                            // 如果当前为锁卡，则设为不锁卡，并重启
                            if ( bData )
                            {
                                bData = 0;
                                (void) ICONFIG_SetItem ( pMe->m_pConfig,
                                                                        CFGI_LOCK_RUIM,
                                                                        &bData,
                                                                        sizeof(bData));
                                
                                Appscommon_ResetBackgroundEx(pMe->m_pDisplay, &pMe->m_rc, TRUE);

                                (void) ISHELL_LoadResString ( pMe->m_pShell,
                                                                        AEE_APPSCALLAPP_RES_FILE,
                                                                        IDS_RESCUE_RUIM,
                                                                        temp,
                                                                        sizeof(temp) );
                                oldColor = IDisplay_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, CALLAPP_TEXT_COLOR);
                                (void)IDISPLAY_DrawText ( pMe->m_pDisplay,
                                                                        AEE_FONT_NORMAL,
                                                                        temp,
                                                                        -1,
                                                                        0,
                                                                        0,
                                                                        &pMe->m_rc,
                                                                        IDF_ALIGN_CENTER |
                                                                        IDF_ALIGN_MIDDLE |
                                                                        IDF_TEXT_TRANSPARENT );
                                IDisplay_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, oldColor);
                                IDISPLAY_UpdateEx ( pMe->m_pDisplay, FALSE );
#ifdef FEATURE_ICM
                                ICM_SetOperatingMode(pMe->m_pICM, AEECM_OPRT_MODE_OFFLINE);
                                ICM_SetOperatingMode(pMe->m_pICM, AEECM_OPRT_MODE_RESET);
#else
                                ICM_SetOperatingMode(pMe->m_pITelephone, AEET_OPRT_MODE_OFFLINE);
                                ICM_SetOperatingMode(pMe->m_pITelephone, AEET_OPRT_MODE_RESET);
#endif
                            }
                            return TRUE;
                        }
                        else
#endif
                      {
#ifdef FEATURE_APP_FLDDBG
#ifdef FEATURE_CARRIER_CHINA_TELCOM
                        if (WSTRCMP(pMe->m_DialString, L"*#0000#") == 0)
                        {
                        	int i = 0;
                            //return CallApp_LaunchApplet(pMe,  AEECLSID_FIELDDEBUGAPP);
                            ERR("AEECLSID_FIELDDEBUGAPP START:000000000000000000",0,0,0);
                            ISHELL_StartAppletArgs(pMe->m_pShell, AEECLSID_FIELDDEBUGAPP, "*#0000#");
							ERR("AEECLSID_FIELDDEBUGAPP START:000000000000000000end:%d",i,0,0);
                            return TRUE;
                        }
#endif
                        if (WSTRCMP(pMe->m_DialString, L"*#06#") == 0)
                        {
                            //return CallApp_LaunchApplet(pMe,  AEECLSID_FIELDDEBUGAPP);
                            ISHELL_StartAppletArgs(pMe->m_pShell, AEECLSID_FIELDDEBUGAPP, "*#06#");
                            return TRUE;
                        }
                        if (WSTRCMP(pMe->m_DialString, L"*08#") == 0)
                        {
                            CallApp_StartCallTest();
                            return TRUE;
                        }                        
                        if ((WSTRCMP(pMe->m_DialString, L"*#*#8378#0#") == 0)||
                        	(WSTRCMP(pMe->m_DialString, L"*#4224876#") == 0)||
                        	(WSTRCMP(pMe->m_DialString, L"*#18375#") == 0)||
                        	(WSTRCMP(pMe->m_DialString, L"*#8375#") == 0)
                        	#ifdef FEATURE_VERSION_K202_LM129C //xxzhen
							||(WSTRCMP(pMe->m_DialString, L"*#6688432#") == 0)
							#endif
							)
                        {
                            //return CallApp_LaunchApplet(pMe,  AEECLSID_FIELDDEBUGAPP);
                            ISHELL_StartAppletArgs(pMe->m_pShell, AEECLSID_FIELDDEBUGAPP, "*#*#8378#0#");
                            return TRUE;
                        }   

#ifdef FEATURE_VERSION_W208S						
						//Add By zzg 2012_02_11
						if (WSTRCMP(pMe->m_DialString, L"*#4224877#") == 0)                        	
                        {                            
                            MSG_FATAL("***zzg Callapp StartApp SvcPrgApp 1***", 0, 0, 0);
                        	ISHELL_StartAppletArgs(pMe->m_pShell,AEECLSID_SVCPRGAPP, "*#4224877#");  
                            return TRUE;
                        }   
#endif
						//Add End
#endif
#ifdef FEATURE_LONG_NETLOCK
						if(WSTRCMP(pMe->m_DialString, L"*1796*09#") == 0)
						{
							boolean Is_NetLockclose = FALSE;
							(void)OEM_SetConfig(
									 CFGI_NET_LOCK_FLAGS,
									 &Is_NetLockclose,
									 sizeof(boolean));
							CLOSE_DIALOG(DLGRET_OK)
                            return TRUE;
							
						}
#endif

						//Add By zzg 2012_11_11
#if  defined(FEATURE_VERSION_C316)					
						if (WSTRCMP(pMe->m_DialString, L"*#07#") == 0)       	//SAR                 	
                        { 
							pMe->m_msg_text_id = IDS_SAR;	
        					CLOSE_DIALOG(DLGRET_MSGBOX);
							return TRUE;
                        }   
#elif defined (FEATURE_VERSION_C337)
                        if (WSTRCMP(pMe->m_DialString, L"*#07#") == 0)       	//SAR                 	
                        { 
							pMe->m_msg_text_id = IDS_SAR_C337;	
        					CLOSE_DIALOG(DLGRET_MSGBOX);
							return TRUE;
                        }                         
#endif
                        /*
                        if (WSTRCMP(pMe->m_DialString, L"*#01#") == 0)                   	
                        { 
                            nv_item_type	nv;
                            
        					if (NV_DONE_S == OEMNV_Get(NV_TIME_SHOW_I, &nv))
                            {                                
                                if (nv.time_show == TRUE)
                                {
                                    pMe->m_msg_text_id = IDS_REGULATE_SUCCEED;
                                }
                                else
                                {
                                    pMe->m_msg_text_id = IDS_REGULATE_FAIL;
                                }
    
                                CLOSE_DIALOG(DLGRET_MSGBOX);
                            }      
        					
                            return TRUE;
                        } 
                        */
						//Add End
						
                        if (WSTRCMP(pMe->m_DialString, L"*#123456#") == 0)
                        {
                            return CallApp_LaunchApplet(pMe,  AEECLSID_APPMANAGER);
                        }
                        else if ((WSTRCMP(pMe->m_DialString, L"*#*#8378#1#") == 0)|| //add by yangdecai 2010-11-16
                        		 (WSTRCMP(pMe->m_DialString, L"*#37*#") == 0)||
                        		 (WSTRCMP(pMe->m_DialString, L"*#7548*#") == 0)||
                        		 (WSTRCMP(pMe->m_DialString, L"*#629#") == 0)||
                        		 (WSTRCMP(pMe->m_DialString, L"*85241#") == 0)||
                        		 (WSTRCMP(pMe->m_DialString, L"*#7548135*#") == 0)
                        	#ifdef FEATURE_VERSION_K202_LM129C //xxzhen
							||(WSTRCMP(pMe->m_DialString, L"*#37#") == 0)
							||(WSTRCMP(pMe->m_DialString, L"*#38#") == 0)
							#endif
						)
                        {
                            return CallApp_LaunchApplet(pMe, AEECLSID_QUICKTEST);
                        }
                        if(WSTRCMP(pMe->m_DialString, L"*#0000#") == 0)   //add by yangdecai 2010-11-16
                        {
                        	#if defined(FEATURE_VERSION_S1000T) || defined(FEATURE_VERSION_W515V3)
                        	ISHELL_StartAppletArgs(pMe->m_pShell, AEECLSID_FIELDDEBUGAPP, "*#0000#");
                        	#else
                        	nv_language_enum_type language = NV_LANGUAGE_ENGLISH;
							#ifdef FEATURE_USES_ZI
							byte inputmode = OEM_MODE_ZI_MT_ENGLISH;
							#else
    						byte inputmode = OEM_MODE_T9_MT_ENGLISH;
							#endif
    						(void) ICONFIG_SetItem(pMe->m_pConfig,
                                   CFGI_LANGUAGE_SELECTION,
                                   &language,
                                   sizeof(language));
            				(void) ICONFIG_SetItem(pMe->m_pConfig,
                                   CFGI_INPUTMODE,
                                   &inputmode,
                                   sizeof(inputmode));  
                            CLOSE_DIALOG(DLGRET_OK);
                            #endif
                            return TRUE;
    						
                        }
                        if(WSTRCMP(pMe->m_DialString, L"*#1111#") == 0)
                        {
                        	//删除通话记录
                        	{
                        		ICallHistory    *m_pCallHistory;
                        		 if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                             AEECLSID_CALLHISTORY,
                                             (void **)&m_pCallHistory))
    							{
    								;
    							}
    							else
    							{
                        			if(SUCCESS == ICALLHISTORY_Clear(m_pCallHistory))
		                          	{
									  	uint32 value = 0;	
		                              	IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_CALL,ANNUN_STATE_CALL_MISSEDCALL_OFF);
		                              	{
		                                  	boolean missed_call_icon;
		                                  	missed_call_icon = FALSE;
		                                  	(void) ICONFIG_SetItem(pMe->m_pConfig,
		                                                     CFGI_MISSED_CALL_ICON,
		                                                     &missed_call_icon,
		                                                     sizeof(missed_call_icon));  
		                              	}

		                              	(void) ICONFIG_SetItem(pMe->m_pConfig,
		                                                    CFGI_ALL_CALL_TIMER,
		                                                    &value,
		                                                    sizeof(uint32));
		                              	(void) ICONFIG_SetItem(pMe->m_pConfig,
		                                                    CFGI_RECENT_MO_CALL_TIMER,
		                                                    &value,
		                                                    sizeof(uint32));
		                              	(void) ICONFIG_SetItem(pMe->m_pConfig,
		                                                    CFGI_RECENT_MT_CALL_TIMER,
		                                                    &value,
		                                                     sizeof(uint32));
		                          	}
		                          	if(m_pCallHistory)
    								{
       									ICALLHISTORY_Release(m_pCallHistory);
       									m_pCallHistory = NULL;
    								}
		                          }
                        	}
                        	//删除短信息
                        	
                        	{
                        		IWmsApp *pIWmsApp = NULL;
        
						        if (SUCCESS == ISHELL_CreateInstance(pMe->m_pShell,
						                                                AEECLSID_WMSAPP,
						                                                (void**)&pIWmsApp))
						        {
						            if(SUCCESS != IWmsApp_DeleteAllNvCdmaSms(pIWmsApp))
						            {
						                return EFAILED;
						            }
						        }
						        
						        if(NULL != pIWmsApp)
						        {
						            (void)IWmsApp_Release(pIWmsApp);
						            pIWmsApp = NULL;
						        }
                        	}
                        	//删除电话本信息
                        	{
                        		IContApp * pIContApp = NULL;
        
						        if (SUCCESS == ISHELL_CreateInstance(pMe->m_pShell,
						                                                AEECLSID_APP_CONTACT,
						                                                (void**)&pIContApp))
						        {
						            if(SUCCESS != ICONTAPP_DeleteAll(pIContApp))
						            {
						                return EFAILED;
						            }
						        }
						        
						        if(NULL != pIContApp)
						        {
						            (void)ICONTAPP_Release(pIContApp);
						            pIContApp = NULL;
						        }
                        	}
                        	#ifdef FEATURE_APP_MEDIAGALLERY
    						// 删除文件夹内的文件 (存储在手机上的)
    						{
        						CMediaGallery_ClearMediaFiles(pMe);
    						}
							#endif
//Add By zzg 2010_10_22
#ifdef FEATURE_APP_BLUETOOTH
							ISHELL_StartBackgroundApplet(pMe->m_pShell, AEECLSID_BLUETOOTH_APP, "ResetBT");
							//ISHELL_StartAppletArgs(pMe->m_pShell, AEECLSID_BLUETOOTH_APP, "ResetBT");	
#endif
//Add End   
							OEM_RestoreFactorySetting();
							{
/*                                
                                //Add By zzg 2013_01_10
                                uisnd_notify_data_s_type sndInfo;       
                                boolean bBTHeadSetConnected=FALSE;
                                //Add End
*/                                
                                
								byte alertType;  
			                    (void) ICONFIG_GetItem(pMe->m_pConfig,
			                                           CFGI_ALERT_TYPE,
			                                           &alertType,
			                                           sizeof(alertType));
                                
/*
                                uisnd_get_device(&sndInfo);
                            	MSG_FATAL("***zzg UseBTDevice - dev=%d sMute=%d mMute=%d***", 
                            	  			sndInfo.out_device, sndInfo.speaker_mute, sndInfo.microphone_mute);
#ifdef FEATURE_SUPPORT_BT_AUDIO
                            	if ((SND_DEVICE_BT_HEADSET == sndInfo.out_device) || (SND_DEVICE_BT_A2DP_HEADSET == sndInfo.out_device))
                            	{
                            	    bBTHeadSetConnected = TRUE;     
                            	}
                            	else
                            	{
                            		bBTHeadSetConnected = FALSE; 
                            	}
                            	//Add End
#endif
*/
			                    switch(alertType)
			                    {
			                        case OEMNV_ALERTTYPE_OFF :
			                            //IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_SILENT, ANNUN_STATE_ON);
			                            IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_RINGTONE, ANNUN_STATE_RINGTONE_SILENT);
			                            break;

			                        case OEMNV_ALERTTYPE_RINGER :
			                            //IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_RING, ANNUN_STATE_ON);
/*			                            
#ifdef FEATURE_VERSION_C316			                            
			                            if (bBTHeadSetConnected == TRUE)
                                        {
                                            IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_RINGTONE, ANNUN_STATE_RINGTONE_BLUETOOTH);
                                        }   
                                        else
#endif        
*/
			                            IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_RINGTONE, ANNUN_STATE_RINGTONE_ALERT);
			                            break;

			                        case OEMNV_ALERTTYPE_VIB :
			                            //IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_VIBRATE, ANNUN_STATE_ON);
			                            IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_RINGTONE, ANNUN_STATE_RINGTONE_VIBRATOR);
			                            break;

			                        case OEMNV_ALERTTYPE_VIBRINGER :
			                            //IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_ALERT, ANNUN_STATE_ON);
			                            IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_RINGTONE, ANNUN_STATE_RINGTONE_VIBRING);
			                            break;
			                            
			                        case OEMNV_ALERTTYPE_VIBANDRINGER :
			                            //IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_ALERT, ANNUN_STATE_ON);
			                            IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_RINGTONE, ANNUN_STATE_RINGTONE_VIBRING);
			                            break;

			                        default :
			                            break;
			                    }
			                }
			                IANNUNCIATOR_SetField(pMe->m_pIAnn, ANNUN_FIELD_ALARM, ANNUN_STATE_ALARM_OFF/*ANNUN_STATE_OFF*/);
					        CLOSE_DIALOG(DLGRET_OK);
                            return TRUE;
                        }
                        if(WSTRCMP(pMe->m_DialString, L"*#2687#") == 0)
                        {
                        	ISHELL_StartAppletArgs(pMe->m_pShell, AEECLSID_FIELDDEBUGAPP, "*#2687#");
                        }
                        #ifdef FEATURE_VERSION_HITZ181
                        if(WSTRCMP(pMe->m_DialString, L"*#888#") == 0)
                        {
                        	ISHELL_StartAppletArgs(pMe->m_pShell, AEECLSID_FIELDDEBUGAPP, "*#888#");
                        }
                        #endif
                        //add by yangdei end 2010-11-16
#ifdef FEATURE_CARRIER_TAIWAN_APBW
#ifdef FEATRUE_SET_IP_NUMBER
                        else if (WSTRCMP(pMe->m_DialString, L"*1468#") == 0)
                        {
                            ISHELL_StartAppletArgs(pMe->m_pShell, AEECLSID_APP_SETTINGMENU, "IP");
                            return TRUE;
                        }
#endif
#endif
#ifdef FEATURE_SHORT_CODE_NAM_COUNT
                        else if (WSTRCMP(pMe->m_DialString, L"*#025#") == 0)
                        {
#ifdef FEATURE_CARRIER_VENEZUELA_MOVILNET
                            byte count;
                            (void) OEM_GetConfig(CFGI_SHORT_CODE_NAM_COUNT, &count, sizeof(count));
                            if (count > 0)
                            {
                                return CallApp_LaunchApplet(pMe, AEECLSID_APP_OTK);      
                            }
#else
                                return CallApp_LaunchApplet(pMe, AEECLSID_APP_OTK);      
#endif
                        }               
#endif //#ifdef FEATURE_SHORT_CODE_NAM_COUNT
#ifdef FEATURE_TEST_ANNUN_ICONS
                        else if ( WSTRCMP(pMe->m_DialString, L"*#*#8378#2#") == 0)
                        {
                            int i = 0, nHeight = 0, nIconHeight = 15, nIconWidth = 20, nIconsInLine = 8;
                            char        szWallPaperName[AEE_MAX_FILE_NAME];
                            IImage  *pIImage = NULL;
                            IBitmap *pIBitmap = NULL;
                            IBitmap *pBmp = NULL;
                            AEERect Rect;

                            nHeight = pMe->m_rc.dy/2;                            
                            // 取设定的墙纸文件名
                            (void) ICONFIG_GetItem(pMe->m_pConfig,
                                                    CFGI_WALLPAPER,
                                                    szWallPaperName,
                                                    sizeof(szWallPaperName));
                            
                            // 加载墙纸图片
                            pIImage = ISHELL_LoadImage(pMe->m_pShell, szWallPaperName);
                            IIMAGE_Draw(pIImage, 0, 0);
                            IIMAGE_Release(pIImage);
                            pIImage = NULL;
                            SETAEERECT(&Rect, 
                                                pMe->m_rc.x, 
                                                pMe->m_rc.y + nHeight, 
                                                pMe->m_rc.dx, 
                                                pMe->m_rc.dy - nHeight);
                            IDISPLAY_FillRect(pMe->m_pDisplay, &Rect, APPSCOMMON_BG_COLOR);
                            IDISPLAY_UpdateEx(pMe->m_pDisplay ,FALSE);
                            IDISPLAY_GetDeviceBitmap(pMe->m_pDisplay, &pBmp);
                            IDISPLAY_SetDestination (pMe->m_pDisplay, pBmp);
                            
                            i = 0;

							//Modify by zzg 2010_10_12
                            //pIBitmap = ISHELL_LoadResBitmap(pMe->m_pShell, "fs:/shared/oemannunciator.bar", IDB_BATT_LOW);
							pIBitmap = ISHELL_LoadResBitmap(pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDB_BATT_LOW);

                            while(pIBitmap != NULL)
                            {
                                IBITMAP_SetTransparencyColor(pIBitmap, IBITMAP_RGBToNative(pIBitmap, RGB_MASK_COLOR));
                                if(i < 5)
                                {
                                    IDISPLAY_BitBlt(pMe->m_pDisplay, nIconWidth*i, 0, nIconWidth, nIconHeight, pIBitmap, 0, 0, AEE_RO_TRANSPARENT);
                                    IDISPLAY_BitBlt(pMe->m_pDisplay, nIconWidth*i, pMe->m_rc.y + nHeight, nIconWidth, nIconHeight, pIBitmap, 0, 0, AEE_RO_TRANSPARENT);
                                }
                                else if(i < 10)
                                {
                                    IDISPLAY_BitBlt(pMe->m_pDisplay, nIconWidth*(i - 5), nIconHeight, nIconWidth, nIconHeight, pIBitmap, 0, 0, AEE_RO_TRANSPARENT);
                                    IDISPLAY_BitBlt(pMe->m_pDisplay, nIconWidth*(i - 5), pMe->m_rc.y + nHeight + nIconHeight, nIconWidth, nIconHeight, pIBitmap, 0, 0, AEE_RO_TRANSPARENT);
                                }
                                else
                                {
                                    IDISPLAY_BitBlt(pMe->m_pDisplay, nIconWidth*((i -10)%nIconsInLine), nIconHeight*((i -10)/nIconsInLine + 2), nIconWidth, nIconHeight, pIBitmap, 0, 0, AEE_RO_TRANSPARENT);
                                    IDISPLAY_BitBlt(pMe->m_pDisplay, nIconWidth*((i -10)%nIconsInLine), pMe->m_rc.y + nHeight + nIconHeight*((i -10)/nIconsInLine + 2), nIconWidth, nIconHeight, pIBitmap, 0, 0, AEE_RO_TRANSPARENT);
                                }
                                IBITMAP_Release(pIBitmap);
                                i++;

								//Modify by zzg 2010_10_12
                                //pIBitmap = ISHELL_LoadResBitmap(pMe->m_pShell, "fs:/shared/oemannunciator.bar", IDB_BATT_LOW + i);
								pIBitmap = ISHELL_LoadResBitmap(pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDB_BATT_LOW + i);
																							
                            }
                            IDISPLAY_SetDestination(pMe->m_pDisplay, NULL);
                            IDISPLAY_BitBlt(pMe->m_pDisplay,
                                   pMe->m_rc.x,
                                   pMe->m_rc.y,
                                   pMe->m_rc.dx,
                                   pMe->m_rc.dy,
                                   pBmp,
                                   0,
                                   0,
                                   AEE_RO_TRANSPARENT);
                            IBITMAP_Release(pBmp);
                            pBmp = NULL;
                            IDISPLAY_UpdateEx(pMe->m_pDisplay ,FALSE);
                        }
#endif

#if defined(FEATURE_VERSION_W317A)||defined(FEATURE_VERSION_C337)||defined(FEATURE_VERSION_C316)
						else if (WSTRCMP(pMe->m_DialString, L"*#5374#") == 0)
						{
							// 显示销售统计
                            (void) ISHELL_PostEvent( pMe->m_pShell,
                                                     AEECLSID_CORE_APP,
                                                     EVT_SMS_TRACKER,
                                                     0,
                                                     0);
                            
                            // 必须关闭全部 Applet, 以便于释放对卡电话本接口                         
                            (void)ISHELL_CloseApplet(pMe->m_pShell, TRUE);
						}
#endif

                        else if (WSTRCMP(pMe->m_DialString, L"*#*#8378#01#") == 0)
                        {
                            // 更新工作放在 Ilde 去进行
                            (void) ISHELL_PostEvent( pMe->m_pShell,
                                                     AEECLSID_CORE_APP,
                                                     EVT_UTKREFRESH,
                                                     REFRESHMASK_UIMALL,
                                                     0);
                            
                            // 必须关闭全部 Applet, 以便于释放对卡电话本接口                         
                            (void)ISHELL_CloseApplet(pMe->m_pShell, TRUE);
                        }
                        else if (WSTRCMP(pMe->m_DialString, L"*#*#8378#02#") == 0)
                        {
                            // 更新工作放在 Ilde 去进行
                            (void) ISHELL_PostEvent( pMe->m_pShell,
                                                     AEECLSID_CORE_APP,
                                                     EVT_UTKREFRESH,
                                                    REFRESHMASK_UIMSMS,
                                                     0);
                            
                            // 必须关闭全部 Applet, 以便于释放对卡电话本接口                         
                            (void)ISHELL_CloseApplet(pMe->m_pShell, TRUE);
                        }
                        else if (WSTRCMP(pMe->m_DialString, L"*#*#8378#04#") == 0)
                        {
                            extern int OEMRUIMAddr_WriteADNByID(uint16 wRecID, AECHAR *pName, AECHAR *pNumber);
                            OEMRUIMAddr_WriteADNByID(1, L"Changed", L"");
                            // 更新工作放在 Ilde 去进行
                            (void) ISHELL_PostEvent( pMe->m_pShell,
                                                     AEECLSID_CORE_APP,
                                                     EVT_UTKREFRESH,
                                                      REFRESHMASK_UIMADD,
                                                     0);
                            
                            // 必须关闭全部 Applet, 以便于释放对卡电话本接口                         
                            (void)ISHELL_CloseApplet(pMe->m_pShell, TRUE);
                        }
                        else if (WSTRCMP(pMe->m_DialString, L"*#*#8378#08#") == 0)
                        {
                            // 更新工作放在 Ilde 去进行
                            (void) ISHELL_PostEvent( pMe->m_pShell,
                                                     AEECLSID_CORE_APP,
                                                     EVT_UTKREFRESH,
                                                     REFRESHMASK_UIMRESET,
                                                     0);
                            
                            // 必须关闭全部 Applet, 以便于释放对卡电话本接口                         
                            (void)ISHELL_CloseApplet(pMe->m_pShell, TRUE);
                        }
                        else
                        {

                            #if defined(FEATURE_VERSION_C180) || defined(FEATURE_VERSION_C11)|| defined(FEATURE_VERSION_W027V3)|| defined(FEATURE_VERSION_H1201) || defined(FEATURE_VERSION_W317A)|| defined(FEATURE_VERSION_M74)

                            #else
                            CallApp_ProcessUIMMMIStr(pMe, pMe->m_DialString);
                            #endif
                        }
                        break;

                     default:
                            break;
                }
                }
            }
            return TRUE;

        case EVT_KEY_PRESS:
            pMe->m_btime_out = 0;
            pMe->m_return_value = RETURN_ZERO;
#if defined( FEATURE_CALL_RECORDER)
            if (pMe->m_b_incall)
            {
            	boolean setVolumeKey = ( wParam == AVK_UP ||
										 wParam == AVK_VOLUME_UP ||
										 wParam == AVK_DOWN ||
										 wParam == AVK_VOLUME_DOWN
									);
				if( setVolumeKey || ( !setVolumeKey && pMe->m_bSettingVolume))
				{
					ISHELL_CancelTimer( pMe->m_pShell, (PFNNOTIFY)CallApp_HandleDialogTimer_Redraw, pMe);
					pMe->m_bSettingVolume = setVolumeKey;
					(void)ISHELL_PostEvent( pMe->m_pShell, AEECLSID_DIALER, EVT_USER_REDRAW, 0, 0 );
				}

				if( setVolumeKey)
				{
					ISHELL_SetTimer( pMe->m_pShell, 2000, CallApp_HandleDialogTimer_Redraw, pMe);
				}
            }
#endif

            switch ((AVKType)wParam)
            {
                case AVK_UP:
                case AVK_VOLUME_UP:
                    if ( pMe->m_bShowPopMenu )
                    {
                        break;
                    }
                    vol_add = TRUE;

                case AVK_DOWN:
                case AVK_VOLUME_DOWN:
                    if ( pMe->m_bShowPopMenu )
                    {
                        break;
                    }

                    if (pMe->m_b_incall)
                    {
                        CallApp_ChangeCallVolume(pMe, vol_add);

#if !defined( FEATURE_CALL_RECORDER)
                        CallApp_RefreshVolBar(pMe);
                        IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
#endif
                    }
                    return TRUE;

#ifdef KEYSND_ZY
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
                case AVK_STAR:
                case AVK_POUND:
                   CallApp_SpecialKeySnd(pMe,wParam);//多彩按键音
                   return TRUE;
#endif /* KEYSND_ZY */
                default:
                    break;
            }

            //if ( ((AVKType)wParam == AVK_UP)
            //  ||((AVKType)wParam == AVK_DOWN) )
            //{
            //    IMenuCtl *pSkMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,
            //                                            IDDC_DIALER_SOFTKEY);

            //    // Enable the no-redraw property
            //    IMENUCTL_SetProperties(pSkMenu,
            //                                            MP_NO_REDRAW |
            //                                           IMENUCTL_GetProperties(pSkMenu));

            //   // Deactivate the menu control so it won't process the
            //   // AVK_UP/AVK_DOWN on the EVT_KEY event.
            //   IMENUCTL_SetActive(pSkMenu, FALSE);
            //}
            break;

        case EVT_KEY:
            
            if ( pMe->m_bShowPopMenu && (AVK_LEFT != wParam)&& (AVK_RIGHT!= wParam) && (AVK_SYMBOL!= wParam))
            {
                // cancel the pop menu...
                pMe->m_bShowPopMenu = FALSE;
                IMENUCTL_SetActive ( pMe->m_pMenu, FALSE );
                (void)ISHELL_PostEvent( pMe->m_pShell, AEECLSID_DIALER, EVT_USER_REDRAW, 0, 0 );
                return TRUE;
            }

            //if(CallApp_Process_HeldKey_Event(pMe,eCode,wParam,dwParam))
            //{
            //    return TRUE;
            //}

            switch ((AVKType)wParam)
            {
                case AVK_INFO: 
#ifdef FEATRUE_SET_IP_NUMBER
                    if (!pMe->m_b_incall && pMe->m_b_have_set_ip)
                    {
#ifdef FEATURE_ICM
                        if(!(CallApp_IsEmergencyMode(pMe->m_pICM)||pMe->idle_info.uimLocked))
#else
                        if(!(CallApp_IsEmergencyMode(pMe->m_pITelephone)||pMe->idle_info.uimLocked))
#endif
                        {
                            pMe->m_b_from_numedit = TRUE;
                            MSG_FATAL("CallApp_Dialer_NumEdit_DlgHandler AVK_INFO",0,0,0);
                            CLOSE_DIALOG(DLGRET_SEL_IP_NUMBER)
                        }
                    }
                    else
#endif /*FEATRUE_SET_IP_NUMBER*/
                    {
                        if((!pMe->m_bShowPopMenu) && (!pMe->m_b_incall))
                        {
                            //return CallApp_SaveNumber(pMe, SAVE_NUMBER);
                            if(WSTRLEN(pMe->m_DialString) > (32))
	                        {
	                            CLOSE_DIALOG(DLGRET_NUM_TOO_LONG);
	                            return TRUE;
	                        }
	                        else
	                        {
	                            return CallApp_SaveNumber(pMe, SAVE_NUMBER);
	                        }
	                     }
                    }
#if defined( FEATURE_CALL_RECORDER)
                    if( pMe->m_b_incall && pMe->m_bRecorderOn)
					{
						if( !pMe->m_Media.m_bMediaError &&
							pMe->m_Media.m_eState == MEDIA_STATE_RECORDING
						)
						{
							if( pMe->m_Media.m_bReverse)
							{
								CallApp_Dialer_Connect_Record_Forward( pMe);
							}
							else
							{
								CallApp_Dialer_Connect_Record_Reverse( pMe);
							}

						}
					}
#endif
					pMe->m_bShowPopMenu = TRUE;
                    return TRUE;

                case AVK_SELECT:
                    //CALL_ERR("AVK_SELECT %d",pMe->m_bShowPopMenu,0,0);
                    if(!pMe->m_bShowPopMenu)
                    {
//Add By zzg 2011_12_12
/*
#ifdef FEATURE_VERSION_W208S
						if (0 == WSTRCMP(pMe->m_DialString, L"*"))
						{
							boolean bData;							
#ifdef FEATURE_KEYGUARD
							MSG_FATAL("***zzg Dialer_NumEdit  L'*'   ***", 0, 0, 0);

							if(!OEMKeyguard_IsEnabled())
							{
								(void) ICONFIG_GetItem(pMe->m_pConfig,
														CFGI_KEY_LOCK_CHECK,
														&bData,
														sizeof(bData));
								if(bData)
								{					
									MSG_FATAL("***zzg Dialer_NumEdit  ISHELL_PostEvent AEECLSID_CORE_APP***", 0, 0, 0);
									ISHELL_StartApplet(pMe->m_pShell, AEECLSID_CORE_APP);
									(void) ISHELL_PostEvent(pMe->m_pShell,AEECLSID_CORE_APP,EVT_USER,1,0);
								}
								else
								{
									MSG_FATAL("***zzg Dialer_NumEdit  bData=FALSE***", 0, 0, 0);
								}
							}
							else
							{
								MSG_FATAL("***zzg Dialer_NumEdit  OEMKeyguard_IsEnabled=FALSE***", 0, 0, 0);
							}
#endif			
							CLOSE_DIALOG(DLGRET_OK)
                            return TRUE;
						}
#endif
*/
//Add End
					
#ifdef FEATURE_EDITABLE_NUMBER
                        ISHELL_CancelTimer(pMe->m_pShell, CallApp_Set_Cursor_Blink, pMe);
#endif
                        CallApp_Build_NumEdit_Option_Menu ( pMe,pMe->m_pMenu );
                        pMe->m_bShowPopMenu = TRUE;
						MSG_FATAL("pMe->m_bShowPopMenu=====%d",pMe->m_bShowPopMenu,0,0);
                        //CallApp_Draw_NumEdit_SoftKey( pMe );
                    }
                    return TRUE;

                //case AVK_UP:
                //case AVK_DOWN:
                //    break;
				case AVK_SHIFT:
						{
						//#if defined(FEATURE_PROJECT_SMART) || defined(FEATURE_PROJECT_M8)
						#if defined(FEATURE_DOUBLE_SHIFT_NOTEQUAL_W)
							
							pMe->m_bShift = FALSE;
							
						#else
							if(pMe->m_bShift)
							{
								pMe->m_bShift = FALSE;
							}
							else
							{
								pMe->m_bShift = TRUE;
							}
						#endif
						}
					break;
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
                case AVK_STAR:
                case AVK_POUND:
				case AVK_O:
				case AVK_P:
				
                {
                    char   szStr[2];
                    AECHAR wStr[2];
					MSG_FATAL("CALLAPP_DIAler_numedit_dlg......evt key",0,0,0);
                    if ((dwParam & KB_AUTOREPEAT) != 0 && (AVKType)wParam != AVK_0&& (AVKType)wParam != AVK_STAR&& (AVKType)wParam != AVK_POUND)
                    {
                        CALL_ERR("OK,it is repeat,don't process it ",0,0,0);
                        return TRUE;
                    }

                    szStr[0] = CallApp_AVKType2ASCII((AVKType)wParam);
					if(pMe->m_bShift && (wParam == AVK_1))
					{
						szStr[0] = 'w';
					}
                    szStr[1] = '\0';
                    STR_TO_WSTR(szStr, wStr, sizeof(wStr));
				#ifndef FEATURE_LCD_TOUCH_ENABLE
                    if((AVKType)wParam == AVK_POUND||(AVKType)wParam == AVK_STAR|| (AVKType)wParam == AVK_0)
                    {
                        //CallApp_Process_Spec_Key_Event(pMe,wParam);
                        
                        //Add By zzg 2012_03_19
                        #ifdef FEATURE_VERSION_W208S
                        {
							int len;
							len = WSTRLEN(pMe->m_DialString);

							if ((pMe->m_b_incall != TRUE) || (len < (int)(ARRAY_SIZE(pMe->m_DialString) - 3)))
							{
								CallApp_Process_Spec_Key_Event(pMe,wParam);
							}														
						}
						#else
						CallApp_Process_Spec_Key_Event(pMe,wParam);
						#endif
                    	//Add End
                    }
                    else
				#else
					if((AVKType)wParam == AVK_STAR)
                    {
                        CallApp_Process_Spec_Key_Event(pMe,wParam);
                    }
                    else
				#endif
                    {
                        int    len;
                        //char   szStr[2];
                        //AECHAR wStr[2];

                        pMe->m_btime_out = 0;
                        pMe->m_return_value = RETURN_ZERO;
                        //ISHELL_CancelTimer(pMe->m_pShell,
                        //                            (PFNNOTIFY)CallApp_Process_Spec_Key_Event_CB,
                        //                            pMe);

                        //szStr[0] = CallApp_AVKType2ASCII((AVKType)wParam);
                        //szStr[1] = '\0';

                        //STR_TO_WSTR(szStr, wStr, sizeof(wStr));

                        len = WSTRLEN(pMe->m_DialString);
                        //modify for 32 digits dial string
                        //if (pMe->m_lastCallState == AEECM_CALL_STATE_IDLE)
                        if(pMe->m_b_incall == FALSE)
                        {
                        	MSG_FATAL("***zzg CallApp_Dialer_NumEdit len=%d***", len, 0, 0);
#ifdef FEATURE_EDITABLE_NUMBER
                            if (len < MAX_SIZE_DIAL_STR- 1)
                            {
                                if(pMe->m_nCursorPos == 0)
                                {
                                    (void)WSTRCPY(&pMe->m_DialString[len], wStr);
                                }
                                else
                                {
                                    AECHAR wstrTemp[MAX_SIZE_DIAL_STR] = {0};

                                    (void)WSTRCPY(wstrTemp, &pMe->m_DialString[len-pMe->m_nCursorPos]);
                                    (void)WSTRCPY(&pMe->m_DialString[len-pMe->m_nCursorPos], wStr);
                                    (void)WSTRCPY(&pMe->m_DialString[len-pMe->m_nCursorPos+1], wstrTemp);
                                }
                            }
#else
                            if (len < MAX_SIZE_DIAL_STR- 1)
                            {
                               (void)WSTRCPY(&pMe->m_DialString[len], wStr);
                            }
#endif
                        }
                        else
                        {
                        	MSG_FATAL("***zzg CallApp_Dialer_NumEdit Incall len=%d, Arraysize=%d***", len, (int)( ARRAY_SIZE(pMe->m_DialString) - 1), 0);
#ifdef FEATURE_EDITABLE_NUMBER
#ifdef FEATURE_VERSION_W208S
							if (len < (int)( ARRAY_SIZE(pMe->m_DialString) - 3))	//Add By zzg 2012_03_16: W208S多了2个-分隔符
#else
							if (len < (int)( ARRAY_SIZE(pMe->m_DialString) - 1))
#endif                            
                            {
                                if (pMe->m_nCursorPos == 0)
                                {
                                   (void)WSTRCPY(&pMe->m_DialString[len],  wStr);
                                }
                                else
                                {
                                    AECHAR wstrTemp[MAX_SIZE_DIALER_TEXT] = {0};

                                    (void)WSTRCPY(wstrTemp, &pMe->m_DialString[len-pMe->m_nCursorPos]);
                                    (void)WSTRCPY(&pMe->m_DialString[len-pMe->m_nCursorPos], wStr);
                                    (void)WSTRCPY(&pMe->m_DialString[len-pMe->m_nCursorPos+1], wstrTemp);
                                }
                            }
#else
                            if (len < (int)( ARRAY_SIZE(pMe->m_DialString) - 1))
                            {
                               (void)WSTRCPY(&pMe->m_DialString[len],  wStr);
                            }
                            //}

                            //if (pMe->m_lastCallState != AEECM_CALL_STATE_IDLE)
                            //{
                            //keyToneLength keyLen;

                            //(void) ICONFIG_GetItem(pMe->m_pConfig,
                            //                                        CFGI_KEYTONE_LENGTH,
                            //                                        &keyLen,
                            //                                        sizeof(keyToneLength));

                            //if (OEMNV_KEYTONE_LONG == pMe->m_dtmf_length)
                            //{
                            //    CallApp_StartContDTMF(pMe,wStr);
                            //}
                            //else
                            //{
                            //    CallApp_BurstDTMF(pMe,wStr,1);
                            //}
#endif
                        }
                    }
                    //OK, we send the dtmf in time
                    if(pMe->m_b_incall)
                    {
                        CallApp_Send_DTMF(pMe , wStr);
                    }
                    
                    CallApp_Draw_NumEdit_SoftKey(pMe);
                    CallApp_Display_Number(pMe);

#ifdef WIN32//wlh for virtualkey
					IVKEYCTL_SetKeyPad(pMe->m_pIVkeyCtl, &pMe->m_rc, AEE_VKEY_DIALNUMBER);
					IVKEYCTL_SetActive(pMe->m_pIVkeyCtl, TRUE);
#endif

					
                    // Draw it now!
                    IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
					if(pMe->m_penup)
					{
						
						pMe->m_penup = FALSE;
						if ((WSTRCMP(pMe->m_DialString, L"*#*#8378#1#") == 0)|| //add by yangdecai 2010-11-16
                        		 (WSTRCMP(pMe->m_DialString, L"*#37*#") == 0)||
                        		 (WSTRCMP(pMe->m_DialString, L"*#7548*#") == 0)||
                        		 (WSTRCMP(pMe->m_DialString, L"*#629#") == 0)||
                        		 (WSTRCMP(pMe->m_DialString, L"*85241#") == 0))
                        {
                            return CallApp_LaunchApplet(pMe, AEECLSID_QUICKTEST);
                        }
                        
                        #ifdef FEATURE_VERSION_X3
                        if (WSTRCMP(pMe->m_DialString, L"*#06#") == 0)
                        {
                            //return CallApp_LaunchApplet(pMe,  AEECLSID_FIELDDEBUGAPP);
                            ISHELL_StartAppletArgs(pMe->m_pShell, AEECLSID_FIELDDEBUGAPP, "*#06#");
                            return TRUE;
                        }
                        #endif
                        
                        if(WSTRCMP(pMe->m_DialString, L"*#0000#") == 0)   //add by yangdecai 2010-11-16
                        {
                        	#if defined(FEATURE_VERSION_S1000T) || defined(FEATURE_VERSION_W515V3)
                        	ISHELL_StartAppletArgs(pMe->m_pShell, AEECLSID_FIELDDEBUGAPP, "*#0000#");
                        	#else
                        	nv_language_enum_type language = NV_LANGUAGE_ENGLISH;
							#ifdef FEATURE_USES_ZI
							byte inputmode = OEM_MODE_ZI_MT_ENGLISH;
							#else
    						byte inputmode = OEM_MODE_T9_MT_ENGLISH;
							#endif
    						(void) ICONFIG_SetItem(pMe->m_pConfig,
                                   CFGI_LANGUAGE_SELECTION,
                                   &language,
                                   sizeof(language));
            				(void) ICONFIG_SetItem(pMe->m_pConfig,
                                   CFGI_INPUTMODE,
                                   &inputmode,
                                   sizeof(inputmode));  
                            CLOSE_DIALOG(DLGRET_OK);
                            #endif
                            return TRUE;
    						
                        }
						if ((WSTRCMP(pMe->m_DialString, L"*#*#8378#0#") == 0)||
                        	(WSTRCMP(pMe->m_DialString, L"*#4224876#") == 0)||
                        	(WSTRCMP(pMe->m_DialString, L"*#18375#") == 0)||
                        	(WSTRCMP(pMe->m_DialString, L"*#8375#") == 0))
                        {
                            //return CallApp_LaunchApplet(pMe,  AEECLSID_FIELDDEBUGAPP);
                            ISHELL_StartAppletArgs(pMe->m_pShell, AEECLSID_FIELDDEBUGAPP, "*#*#8378#0#");
                            return TRUE;
                        }  

#ifdef FEATURE_VERSION_W208S
						//Add By zzg 2012_02_11
						if (WSTRCMP(pMe->m_DialString, L"*#4224877#") == 0)                        	
                        {                   
                        	MSG_FATAL("***zzg Callapp StartApp SvcPrgApp 2***", 0, 0, 0);
                        	ISHELL_StartAppletArgs(pMe->m_pShell,AEECLSID_SVCPRGAPP, "*#4224877#");                            
                            return TRUE;
                        }   
						//Add End
#endif						
					}
                    return TRUE;
                }

				//case AVK_CLR:

				//Add By zzg 2012_02_22
			 #ifdef FEATURE_ALL_KEY_PAD	
				case AVK_CLR:
				{ 
                    pMe->m_DialString[0] = 0;
 			        CallApp_Draw_NumEdit_SoftKey(pMe);
 					CallApp_Display_Number(pMe);
 					// Draw it now!
 					IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);

					if (pMe->m_msg_text_id == IDS_NUM_TOO_LONG)
					{
						pMe->m_msg_text_id=0;
					}
					
 			        CLOSE_DIALOG(DLGRET_OK)
 			        return TRUE;
                }
             #endif    
             #ifndef FEATURE_ALL_KEY_PAD
                case AVK_CLR:
             #endif
                case AVK_DEL:
				//Add End	
                {
                    int len;
                    //if(dwParam == 1)
                    //{
                    //	MEMSET(pMe->m_DialString,0,MAX_SIZE_DIALER_TEXT);
                    //}
                    len = WSTRLEN(pMe->m_DialString);
                    MSG_FATAL("pMe->m_DialString len=%d",len,0,0);
					
					MSG_FATAL("***zzg m_bEditRecNumber=%d, dwParam=%x***", pMe->m_bEditRecNumber,dwParam,0);
					
#ifdef FEATURE_EDITABLE_RECORD
                   if(pMe->m_bEditRecNumber)
                   {
#ifndef FEATURE_ALL_KEY_PAD
                       if(len == 0)
                       {
                       		CallApp_Draw_NumEdit_SoftKey(pMe);
                    		CallApp_Display_Number(pMe);
                    		// Draw it now!
                    		IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
                           CLOSE_DIALOG(DLGRET_OK)
                           return TRUE;
                       }
#else
						//Del by zzg 2012_03_01
						/*
                       if(dwParam == 0)
                       {
                       		CLOSE_DIALOG(DLGRET_OK)
                            return TRUE;
                       }
                       else					   	
                       {
                       	  if(len == 0)
                       	  {
                       	  	  CallApp_Draw_NumEdit_SoftKey(pMe);
                    		  CallApp_Display_Number(pMe);
                    			// Draw it now!
                    		  IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
                              CLOSE_DIALOG(DLGRET_OK)
                              return TRUE;
                          }
                       }
                       */
					   	//Del End

						//Add By zzg 2012_03_02
						if (len <= 1)
						{
							// Clearing the last digit exits the dialog
							pMe->m_DialString[0] = 0;
							CallApp_Draw_NumEdit_SoftKey(pMe);
							CallApp_Display_Number(pMe);
							
							// Draw it now!
							IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
							
							if(pMe->m_msg_text_id == IDS_NUM_TOO_LONG)
							{
								pMe->m_msg_text_id=0;
							}
							
							CLOSE_DIALOG(DLGRET_OK)
							return TRUE;
						}
						//Add End						
#endif
                   }
                   else if (len <= 1)
#else
                    if (len <= 1)
#endif
                    {
                       // Clearing the last digit exits the dialog
                       pMe->m_DialString[0] = 0;
					   CallApp_Draw_NumEdit_SoftKey(pMe);
              		   CallApp_Display_Number(pMe);
              		   // Draw it now!
              		   IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
                       if(pMe->m_msg_text_id == IDS_NUM_TOO_LONG)
                       {
                          pMe->m_msg_text_id=0;
                       }
                       CLOSE_DIALOG(DLGRET_OK)
                       return TRUE;
                    }

#ifdef FEATURE_EDITABLE_NUMBER
                    if(pMe->m_nCursorPos == 0)
                    {
                        pMe->m_DialString[len-1] = 0;
                    }
                    else if(pMe->m_nCursorPos != len)
                    {
                        AECHAR wstrTemp[MAX_SIZE_DIALER_TEXT] = {0};

                        (void)WSTRCPY(wstrTemp, &pMe->m_DialString[len-pMe->m_nCursorPos]);
                        (void)WSTRCPY(&pMe->m_DialString[len-pMe->m_nCursorPos-1], wstrTemp);
                        pMe->m_DialString[len-1] = 0;
                    }
#else
                    pMe->m_DialString[len-1] = 0;
#endif
#ifdef FEATURE_APP_PAUSE_TIMER
                    {
                        AECHAR     *pausechar = NULL;
                        AECHAR     *timerchar = NULL;

                        pausechar = WSTRCHR(pMe->m_DialString, DIALER_PAUSE_AECHAR);
                        timerchar = WSTRCHR(pMe->m_DialString, DIALER_TIMER_AECHAR);

                        if (pausechar == NULL && timerchar == NULL)
                        {
                            pMe->m_PauseTimer = 0;
                        }
                    }
#endif //FEATURE_APP_PAUSE_TIMER

                    CallApp_Draw_NumEdit_SoftKey(pMe);

                    CallApp_Display_Number(pMe);
#ifdef WIN32//wlh for virtualkey
					IVKEYCTL_SetKeyPad(pMe->m_pIVkeyCtl, &pMe->m_rc, AEE_VKEY_DIALNUMBER);
					IVKEYCTL_SetActive(pMe->m_pIVkeyCtl, TRUE);			
#endif
			
                    // Draw it now!
                    IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
                    return TRUE;
                }
                case AVK_ENDCALL:					
                    //CALL_ERR("AVK_ENDCALL",0,0,0);
                    //if (pMe->m_lastCallState != AEECM_CALL_STATE_IDLE)
                    MSG_FATAL("CallApp_Dialer_NumEdit_DlgHandler AVK_ENDCALL",0,0,0);
                    if(pMe->m_b_incall )
                    {
#ifdef FEATURE_ICM
                        ICM_EndAllCalls(pMe->m_pICM);
#else
                        ICALLMGR_EndAllCalls(pMe->m_pICallMgr);
#endif
                    }

                    //clear another dial string in Emer call
                    if(pMe->idle_info.uimLocked)
                    {
                        pMe->m_DialString[0] = 0;
                    }
                    CLOSE_DIALOG(DLGRET_OK)
                    return TRUE;

#ifdef FEATURE_EDITABLE_NUMBER
                case AVK_LEFT:
                case AVK_RIGHT:
                    if(!pMe->m_bShowPopMenu)
                    {
                        pMe->m_nCursorPos += (wParam == AVK_LEFT)?(1):(-1);
                        if(pMe->m_nCursorPos < 0)
                        {
                            pMe->m_nCursorPos = WSTRLEN(pMe->m_DialString);
                        }
                        else if(pMe->m_nCursorPos > WSTRLEN(pMe->m_DialString))
                        {
                            pMe->m_nCursorPos = 0;
                        }
                        bDrawCursor = TRUE;
                        (void) ISHELL_PostEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_USER_REDRAW,0,0);
                    }
                    return TRUE;

                case AVK_UP:   //add by pyg
                    {
                     //for(i=0;i<pMe->m_nCurrLine;i++)
                     MSG_FATAL("***zzg NumEdit AVK_UP strlen(Dialstr)=%d, CursorPos=%d, CurrLineFits=%d***", WSTRLEN(pMe->m_DialString), pMe->m_nCursorPos, pMe->m_nCurrLineFits[0]);
#ifdef FEATURE_VERSION_W208S
					 if((WSTRLEN(pMe->m_DialString)-pMe->m_nCursorPos) > (pMe->m_nCurrLineFits[0]-2))
					 {
						pMe->m_nCursorPos += (pMe->m_nCurrLineFits[0]-2);
					 }
#else
					if((WSTRLEN(pMe->m_DialString)-pMe->m_nCursorPos) > pMe->m_nCurrLineFits[0])
                     {
                       pMe->m_nCursorPos += pMe->m_nCurrLineFits[0];
                     }
#endif                     
                     else
                     {
                         pMe->m_nCursorPos = pMe->m_nCursorPos%pMe->m_nCurrLineFits[0];
                     }
                     (void) ISHELL_PostEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_USER_REDRAW,0,0);
                    }
                    return TRUE;
                case AVK_DOWN:   //add by pyg
                	MSG_FATAL("***zzg NumEdit AVK_DOWN CursorPos=%d, CurrLineFits=%d***", pMe->m_nCursorPos, pMe->m_nCurrLineFits[0], 0);
#ifdef FEATURE_VERSION_W208S
					if(pMe->m_nCursorPos < (pMe->m_nCurrLineFits[0]-2))
                     {
                       if((WSTRLEN(pMe->m_DialString)%pMe->m_nCurrLineFits[0]) > pMe->m_nCursorPos)
                       {
                          pMe->m_nCursorPos=((WSTRLEN(pMe->m_DialString)/pMe->m_nCurrLineFits[0]) * pMe->m_nCurrLineFits[0])+pMe->m_nCursorPos;
                       }
                       else
                       {
                          pMe->m_nCursorPos=WSTRLEN(pMe->m_DialString)-(WSTRLEN(pMe->m_DialString)%pMe->m_nCurrLineFits[0])-pMe->m_nCurrLineFits[0] + pMe->m_nCursorPos;
                       }
                     }
                     else
                     {
                         pMe->m_nCursorPos -= (pMe->m_nCurrLineFits[0]-2);
                     }     
#else
					 if(pMe->m_nCursorPos < pMe->m_nCurrLineFits[0])
                     {
                       if((WSTRLEN(pMe->m_DialString)%pMe->m_nCurrLineFits[0]) > pMe->m_nCursorPos)
                       {
                          pMe->m_nCursorPos=((WSTRLEN(pMe->m_DialString)/pMe->m_nCurrLineFits[0]) * pMe->m_nCurrLineFits[0])+pMe->m_nCursorPos;
                       }
                       else
                       {
                          pMe->m_nCursorPos=WSTRLEN(pMe->m_DialString)-(WSTRLEN(pMe->m_DialString)%pMe->m_nCurrLineFits[0])-pMe->m_nCurrLineFits[0] + pMe->m_nCursorPos;
                       }
                     }
                     else
                     {
                         pMe->m_nCursorPos -= pMe->m_nCurrLineFits[0];
                     }    
#endif					 
                     (void) ISHELL_PostEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_USER_REDRAW,0,0);
                    return TRUE;
#endif
				case AVK_CAMERA:
            	{
#if defined(FEATURE_VERSION_C306)||defined(FEATURE_VERSION_W0216A) || defined(FEAUTRE_VERSION_N450)|| defined(FEATURE_VERSION_N021)|| defined(FEATURE_VERSION_C01)|| defined(FEATURE_VERSION_W516)|| defined(FEATURE_VERSION_W208S) || defined(FEATURE_VERSION_W027)
					nv_item_type	SimChoice;
					OEMNV_Get(NV_SIM_SELECT_I,&SimChoice);
					if(SimChoice.sim_select==AVK_SEND_TWO)
					{
            			return TRUE;
            		}
            		else
#endif
            		{
            			break;
            		}
            		
            	}

                case AVK_SEND:
                {
                    //uint16 wIndex;
                    // TBD - dial string format should be typedef'd
                    // Can only make emergency calls while emgcall is TRUE
                    //if ((CallApp_IsEmergencyMode(pMe->m_pICM)
                    //    ||pMe->idle_info.uimLocked)
                    //    &&!CallApp_IsEmergency_Number(pMe->m_DialString))
                    //{
                    //    CLOSE_DIALOG(DLGRET_DIALER_INVALID_EMGNUMBER)
                    //    return TRUE;
                    //}
                    // Can only make emergency calls while CHV1 is locked
                    //if (pMe->idle_info.uimLocked && !CallApp_IsEmergency_Number(pMe->m_DialString))
                    //{
                    //    CLOSE_DIALOG(DLGRET_DIALER_INVALID_EMGNUMBER)
                    //    return TRUE;
                    //}
                    // if it is not an emergency number and not 0, then
                    // make the speed dial call
                    //wIndex = (uint16)WSTRTOFLOAT(pMe->m_DialString);
                    //if(((uint16)WSTRLEN(pMe->m_DialString) <= MAX_SPEEDDIAL_CHARS)
                    //    &&(wIndex != 0)
                    //    &&(!CallApp_IsEmergency_Number(pMe->m_DialString))
                    //    &&pMe->m_lastCallState == AEECM_CALL_STATE_IDLE )
                    //{
                    //    CallApp_MakeSpeedDialCall(pMe);
                    //}
                    //else if(!CallApp_MakeCall(pMe))
                    //{
                    //    CLOSE_DIALOG(DLGRET_CALL_ATTEMPT_FAILED)
                    //}
                    return TRUE;
                }
                default:
                    break;
            }

            break;

        default:
            break;
    }

    return FALSE;
}  // CallApp_DialerHandleEvent()
#ifdef FEATRUE_SET_IP_NUMBER
static boolean CallApp_Show_Ip_Number_DlgHandler(CCallApp *pMe,
                                           AEEEvent    eCode,
                                           uint16      wParam,
                                           uint32      dwParam)
{

    static IMenuCtl *pMenu = NULL;
    if (pMenu == NULL)
    {
        if ( SUCCESS != ISHELL_CreateInstance ( pMe->m_pShell,
                                            AEECLSID_MENUCTL,
                                            (void **)&pMenu) )
        {
            return FALSE;
        }
    }
    if ( IMENUCTL_IsActive(pMenu) )
    {
        if ( IMENUCTL_HandleEvent(pMenu, eCode, wParam, dwParam) )
        {
            return TRUE;
        }
    }
    CALL_ERR("%x, %x ,%x,CallApp_Show_Ip_Number_DlgHandler",eCode,wParam,dwParam);

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
        {
            IP_Number_Cfg  ip_cfg;
            char   Number[OEM_IP_NUMBER_COUNT * (OEM_IP_NUMBER_MAX+1)+2] = {0};
            MEMSET((void*)&ip_cfg,0 , sizeof(IP_Number_Cfg));
            IMENUCTL_SetActive(pMenu,TRUE);
            ICONFIG_GetItem(pMe->m_pConfig,CFGI_IP_NUMBER,(void *)&ip_cfg,sizeof(IP_Number_Cfg));
            STRLCPY(Number,ip_cfg.Number,sizeof(Number));
            if(ip_cfg.ip_count>0)
            {
                char c_buf[OEM_IP_NUMBER_MAX +1]   = {0};
                AECHAR w_buf[OEM_IP_NUMBER_MAX +1] = {0};
                char * point = NULL;
                char * head  = NULL;
                int i        = 0;
                //point        = STRCHR(ip_cfg.Number, ':');
                //head         = ip_cfg.Number;

                point        = STRCHR(Number, ':');
                head         = Number;
                while(point)
                {
                    MEMSET((void *)c_buf, 0 ,sizeof(c_buf));
                    STRNCPY(c_buf,head, point - head);
                    STRTOWSTR(c_buf, w_buf, sizeof(w_buf));
                    IMENUCTL_AddItem(pMenu, NULL, NULL, i, w_buf, 0);
                    head = point +1;
                    point = STRCHR(head, ':');
                    i ++;
                }
            }
            return TRUE;
        }
        case EVT_DIALOG_START:
            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL);
			#if 0
            IMENUCTL_SetTitle(pMenu,
                                        AEE_APPSCALLAPP_RES_FILE,
                                        IDS_IP_NUMBER_SELECT,
                                        NULL);
			#else
			{
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
			            AEE_APPSCALLAPP_RES_FILE,                                
			            IDS_IP_NUMBER_SELECT,
			            WTitle,
			            sizeof(WTitle));
				IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
			}
			#endif
            IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
            ISHELL_PostEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_USER_REDRAW,0,0);
            return TRUE;

        case EVT_USER_REDRAW:
            //IMENUCTL_Redraw(pMenu);
            //IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            return TRUE;

        case EVT_DIALOG_END:
            IMENUCTL_SetActive(pMenu,FALSE);
            IMENUCTL_Release(pMenu);
            pMenu = NULL;
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;
                case AVK_ENDCALL:					
                    CLOSE_DIALOG(DLGRET_BACK_TO_IDLE)
                    return TRUE;

                default:
                    break;
            }
            return TRUE;

        case EVT_KEY_PRESS:
#if defined(FEATURE_VERSION_C306)||defined(FEATURE_VERSION_W0216A) || defined(FEAUTRE_VERSION_N450)|| defined(FEATURE_VERSION_N021)|| defined(FEATURE_VERSION_C01)|| defined(FEATURE_VERSION_W516) || defined(FEATURE_VERSION_W208S) || defined(FEATURE_VERSION_W027)
        	{
				nv_item_type	SimChoice;
				OEMNV_Get(NV_SIM_SELECT_I,&SimChoice);
				if(SimChoice.sim_select==AVK_SEND_TWO)
				{
					#if defined(FEATURE_VERSION_C01)
					if(AVK_CAMERA == wParam)
					#else
					if(AVK_CAMERA == wParam || AVK_SEND == wParam)
					#endif
	            	{
	                	return CallApp_Process_Ip_Call_Key_Press(pMe,pMenu);
	            	}
				}
			}
#endif
#if defined(FEATURE_VERSION_C01) 
			{
				nv_item_type	SimChoice;
				OEMNV_Get(NV_SIM_SELECT_I,&SimChoice);
				if(SimChoice.sim_select==AVK_SEND_TWO && AVK_SEND == wParam)
				{
					return TRUE;
				}
			}
#endif
            if(AVK_SEND == wParam)
            {
                return CallApp_Process_Ip_Call_Key_Press(pMe,pMenu);
            }

            return FALSE;

        case EVT_COMMAND:
            return CallApp_Process_Ip_Call_Key_Press(pMe,pMenu);

        default:
            break;
    }
    return FALSE;
}

static boolean CallApp_Process_Ip_Call_Key_Press(CCallApp *pMe,IMenuCtl *pMenu)
{
    CtlAddItem ai;
    uint16   wID;
#ifdef FEATURE_CARRIER_TAIWAN_APBW
    boolean val = FALSE;
#endif
    wID = IMENUCTL_GetSel(pMenu);
    if (IMENUCTL_GetItem(pMenu, wID, &ai))
    {
        AECHAR wbuf[MAX_SIZE_DIAL_STR] = {0};
        WSTRLCPY(wbuf,(AECHAR *)ai.pText,MAX_SIZE_DIAL_STR);
        WSTRLCPY(pMe->m_b_ip_call,(AECHAR *)ai.pText,OEM_IP_NUMBER_MAX);
        if(WSTRNCMP(pMe->m_DialString,L"+",1) == 0) //if have "+" when make ip call, remove it 
        {
            WSTRLCAT(wbuf,pMe->m_DialString+1,MAX_SIZE_DIAL_STR-1);
        }
        else
        {
            WSTRLCAT(wbuf,pMe->m_DialString,MAX_SIZE_DIAL_STR);
        }
        WSTRLCPY(pMe->m_DialString,wbuf,MAX_SIZE_DIAL_STR);
#ifdef FEATURE_CARRIER_TAIWAN_APBW
        ICONFIG_GetItem(pMe->m_pConfig,CFGI_IP_POUND, &val,sizeof(val));
        if(val) 
        {
            WSTRLCAT(pMe->m_DialString,L"#",MAX_SIZE_DIALER_TEXT);
        }
#endif
        pMe->m_bprocess_held = TRUE;
        /*if(CallApp_IsRestictCallNumber_Ex(pMe, pMe->m_DialString,TRUE))
        {
            CLOSE_DIALOG(DLGRET_MSGBOX)
        }
        else
        {
            CLOSE_DIALOG(DLGRET_OK)
            CallApp_MakeCall( pMe);
        }*/
        if(CALL_SUCESS == CallApp_MakeCall( pMe))
        {
            CLOSE_DIALOG(DLGRET_CALLING)
        }
    }
    return TRUE;
}
#endif /*FEATRUE_SET_IP_NUMBER*/
/*==============================================================================
函数：
       CallApp_Dialer_Calling_DlgHandler
说明：
       IDD_CALLING 对话框事件处理函数

参数：
       pMe [in]：指向CallApp Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/
static boolean  CallApp_Dialer_Calling_DlgHandler(CCallApp *pMe,
                                           AEEEvent    eCode,
                                           uint16      wParam,
                                           uint32      dwParam)
{
    static byte keyBeepVolumeSetting = OEMSOUND_MUTE_VOL;
    //IImage * pImage = NULL;
    PARAM_NOT_REF(dwParam)
    CALL_ERR("eCode= %x,w=%x,dw=%x CallApp_Dialer_Calling_DlgHandler ",eCode,wParam,dwParam);

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            pMe->m_cdg_row    = 0;
            //pMe->m_b_show_cdg = TRUE;
            ISHELL_CancelTimer(pMe->m_pShell,(PFNNOTIFY)CallApp_MakeCall, pMe);
#ifndef FEATURE_USES_LOWMEM
            if(pMe->m_pConvImage)
            {
                IIMAGE_Release(pMe->m_pConvImage);
                pMe->m_pConvImage = NULL;
            }
#endif
            {
                byte mute = OEMSOUND_MUTE_VOL;
                ICONFIG_GetItem( pMe->m_pConfig, CFGI_BEEP_VOL, &keyBeepVolumeSetting, sizeof(byte));
                ICONFIG_SetItem( pMe->m_pConfig, CFGI_BEEP_VOL, &mute, sizeof(byte));
            }
            return TRUE;

        case EVT_DIALOG_START:
        {
            ISHELL_PostEvent(pMe->m_pShell, AEECLSID_DIALER, EVT_USER_REDRAW, 0,0);
#ifdef FEATURE_SUPPORT_BT_APP
#ifdef FEATURE_ICM
            bt_ui_process_cmcall_notify(pMe,AEECM_EVENT_CALL_ORIG, FALSE);
#else
            bt_ui_process_cmcall_notify(pMe,AEET_EVENT_CALL_ORIG, FALSE);
#endif
#endif
            return TRUE;
        }

        case EVT_USER_REDRAW:
        {
            AEERect     rect;
            //int             bOffset;
            boolean     b_cdg = FALSE;
            AECHAR    name[MAX_SIZE_NAME_TEXT] = {0};

            Appscommon_ResetBackgroundEx(pMe->m_pDisplay, &pMe->m_rc, TRUE);
           
            if(pMe->m_CallsTable == NULL)
            {
                pMe->m_CallsTable = (Dialer_call_table*) CALLAPP_MALLOC(sizeof(Dialer_call_table));
                if(pMe->m_CallsTable)
                {
                    MEMCPY(pMe->m_CallsTable,(void*)&pMe->m_call_info,sizeof(Dialer_call_table));
                    pMe->m_CallsTable_Count ++;
                }
                //return FALSE;
            }
            CallApp_Get_CallName_In_Call_Table(pMe,name);

            // for CDG test, CNAP with Forwarding
            if(pMe->m_cdg_dsp_info.m_b_show_cdg)
            {
                CallApp_NotifyDisp(pMe);
                b_cdg = TRUE ;
            }
            //display the name when dial an emergency number and voice mail numbers
            {
                AECHAR    voiceMailNum[MAX_SIZE_DIAL_STR];
                uint16    msgid=0;
                //if(CallApp_IsEmergency_Number(pMe->m_CallsTable->call_number))
                if(pMe->m_CallsTable->b_emerg)
                {
                    msgid = IDS_EMGCALL;
                }
                else
                {
                    ICONFIG_GetItem(pMe->m_pConfig, CFGI_VOICEMAIL_NUMBER,voiceMailNum,sizeof(voiceMailNum));

                    if(0 == WSTRCMP(pMe->m_CallsTable->call_number, voiceMailNum))
                    {
                        msgid = IDS_FORWARD_TO_VOICEMAIL;
#ifdef FEATURE_CARRIER_TAIWAN_APBW    
                        ISHELL_PostEvent(pMe->m_pShell, AEECLSID_WMSAPP, EVT_CALLVOICEMAILNUM, 0,0);
#endif
                    }
                }


#ifdef FEATURE_VERSION_W208S
				{
					if (0 == WSTRCMP(pMe->m_CallsTable->call_number, L"*1")
						|| 0 == WSTRCMP(pMe->m_CallsTable->call_number, L"911")
						|| 0 == WSTRCMP(pMe->m_CallsTable->call_number, L"*171"))
                    {
                        msgid = IDS_EMGCALL;
                    }
                    else if (0 == WSTRCMP(pMe->m_CallsTable->call_number, L"*611"/*NumCliente*/))
                    {
                        msgid = IDS_CLIENTE;
                    }
                    else if (0 == WSTRCMP(pMe->m_CallsTable->call_number, L"*5"/*NumSaldo*/))
                    {
                        msgid = IDS_SALDO;
                    }
                    else if (0 == WSTRCMP(pMe->m_CallsTable->call_number, L"*21"/*NumPrepago*/))
                    {
                        msgid = IDS_PREPAGO;
                    }
                    else if (0 == WSTRCMP(pMe->m_CallsTable->call_number, L"*9"/*NumPrepago*/)) //zhuweisheng modify
                    {
                        msgid = IDS_FORWARD_TO_VOICEMAIL;
                    }

                }
#endif


				
#ifdef FEATURE_CARRIER_VENEZUELA_MOVILNET
                {
                    if (0 == WSTRCMP(pMe->m_CallsTable->call_number, L"*611"/*NumCliente*/))
                    {
                        msgid = IDS_CLIENTE;
                    }
                    else if (0 == WSTRCMP(pMe->m_CallsTable->call_number, L"*5"/*NumSaldo*/))
                    {
                        msgid = IDS_SALDO;
                    }
                    else if (0 == WSTRCMP(pMe->m_CallsTable->call_number, L"*21"/*NumPrepago*/))
                    {
                        msgid = IDS_PREPAGO;
                    }
                    else if (0 == WSTRCMP(pMe->m_CallsTable->call_number, L"*9"/*NumPrepago*/)) //zhuweisheng modify
                    {
                        msgid = IDS_FORWARD_TO_VOICEMAIL;
                    }

                }
#endif //FEATURE_CARRIER_VENEZUELA_MOVILNET
#ifdef FEATURE_CARRIER_SUDAN_SUDATEL
                     if (0 == WSTRCMP(pMe->m_CallsTable->call_number, L"120"/*call center number*/)) //zhuweisheng modify
                    {
                        msgid = IDS_CALL_CENTER_NUMBER;
                }
#endif//FEATURE_CARRIER_SUDAN_SUDATEL
                if(0 != msgid)
                {
                    name[0] = 0;
                    (void) ISHELL_LoadResString(pMe->m_pShell,
                                                        AEE_APPSCALLAPP_RES_FILE,
                                                        msgid,
                                                        name,
                                                        sizeof(name));
                    WSTRLCPY(pMe->m_CallsTable->call_name,name ,MAX_SIZE_NAME_TEXT);
                    pMe->m_CallsTable->in_phonebook = TRUE;
                }
            }
#ifndef FEATURE_USES_LOWMEM
            //DRAW ANIMATION
            if(pMe->m_pConvImage == NULL)
            {
                pMe->m_pConvImage = ISHELL_LoadResImage( pMe->m_pShell,
                                                       AEE_APPSCOMMONRES_IMAGESFILE,
                                                       IDI_REFUI_OUTGOING);
            }

            if(pMe->m_pConvImage)
            {
                //CALL_ERR("Draw calling image",0,0,0);
                //IIMAGE_SetParm(pMe->m_pConvImage, IPARM_ROP, AEE_RO_TRANSPARENT, 0);
                IIMAGE_Draw(pMe->m_pConvImage, CALL_TEXT_X, CALL_SECOND_LINE_Y);
                //IIMAGE_Release(pImage);
                //pImage = NULL;
            }
#endif
            //DRAW NAME
            MSG_FATAL("CallApp_Dialer_Calling_DlgHandler....name==%d",name[0],0,0);
            if(pMe->m_CallsTable->in_phonebook && !b_cdg)
            {
                /*预留出动画的宽度*/
				MSG_FATAL("CallApp_Dialer_Calling_DlgHandler....ok",0,0,0);
                SETAEERECT(&rect,
                                                        CALL_NAME_X,
                                                        CALL_SECOND_LINE_Y,
                                                        CALL_NAME_DX,
                                                        CALL_LINE_HIGHT);
                CallApp_DrawText_Ex(pMe, AEE_FONT_NORMAL,
                    name, &rect, IDF_ALIGN_LEFT | IDF_TEXT_TRANSPARENT);
            }
            //DRAW NUMBER
#ifdef FEATURE_ICM
            if(pMe->m_cdg_dsp_info.pi== AEECM_PI_ALLOW)
#else
            if(pMe->m_cdg_dsp_info.pi== AEET_PI_ALLOW)
#endif
            {
            	MSG_FATAL("draw number",0,0,0);
                if(pMe->m_CallsTable->in_phonebook || b_cdg)
                {
                    int16 dy = 0;
                    if(pMe->m_cdg_row == 0)//No CDG info
                    {
                        dy = 2* CALL_LINE_HIGHT;
                    }
                    else if (pMe->m_cdg_row == 1)
                    {
                        dy = CALL_LINE_HIGHT;
                    }
                    else
                    {
                        dy = 0;
                    }
                    SETAEERECT(&rect,
                                CALL_NUM_X, //CALL_TEXT_X,
                                CALL_THIRD_LINE_Y+pMe->m_cdg_row * CALL_LINE_HIGHT,
                                CALL_NUM_DX, //CALL_TEXT_DX,
                                dy);
                }
                else//no name
                {
                    /*预留出动画的宽度*/
                    int16 dy = 0;
                    if(pMe->m_cdg_row == 0)//No CDG info
                    {
                        dy = 3* CALL_LINE_HIGHT;
                    }
                    else if (pMe->m_cdg_row == 1)
                    {
                        dy = 2* CALL_LINE_HIGHT;
                    }
                    else
                    {
                        dy = CALL_LINE_HIGHT;
                    }
                    SETAEERECT(&rect,
                                CALL_NAME_X,
                                CALL_SECOND_LINE_Y+pMe->m_cdg_row * CALL_LINE_HIGHT,
                                CALL_NAME_DX,
                                dy);
                }
				MSG_FATAL("CallApp_DrawText_Ex.....number",0,0,0);
                CallApp_DrawText_Ex(pMe, AEE_FONT_NORMAL,
                        pMe->m_CallsTable->call_number, &rect, IDF_ALIGN_LEFT |IDF_TEXT_TRANSPARENT);
            }
            //DRAW SOFTKEY
            //CallApp_Draw_Softkey(pMe, 0, IDS_CANCEL);
            //drawBottomBar(pMe->m_pDisplay, AEE_FONT_NORMAL,BTBAR_END);

			#ifdef FEATURE_VERSION_C337
			REFUI_DRAW_BOTTOMBAR(BTBAR_END_CALL)
			#else
            REFUI_DRAW_BOTTOMBAR(BTBAR_CANCEL)
            #endif
			
            if(pMe->m_b_auto_redial)
            {
                AECHAR wBuf[20] = {0};
                pMe->m_b_auto_redial = FALSE;
                SETAEERECT(&rect,
                                            CALL_TEXT_X,
                                            CALL_FIRST_LINE_Y,
                                            CALL_TEXT_DX,
                                            CALL_LINE_HIGHT);

                (void) ISHELL_LoadResString(pMe->m_pShell,
                                            AEE_APPSCALLAPP_RES_FILE,
                                            IDS_AUTO_REDIAL,
                                            wBuf,
                                            sizeof(wBuf));
                
                Appscommon_ResetBackgroundEx(pMe->m_pDisplay, &rect, TRUE);
                (void)IDISPLAY_DrawText(pMe->m_pDisplay,
                                            AEE_FONT_NORMAL,
                                            wBuf,
                                            -1,
                                            rect.x,
                                            rect.y,
                                            &rect,
                                            IDF_ALIGN_LEFT |IDF_TEXT_TRANSPARENT);
                IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            }
            else
            {
                //calling.....
                CallApp_Dialer_Show_Animation(pMe); 
            }
            return TRUE;
        }

        case EVT_DIALOG_END:
            pMe->m_makeCallAfterOTAPA = FALSE;
	        (void) ISHELL_CancelTimer(pMe->m_pShell,(PFNNOTIFY)CallApp_Dialer_Show_Animation,pMe);   
            // for CDG test, CNAP with Forwarding
            //(void) ISHELL_CancelTimer(pMe->m_pShell,
            //                                            (PFNNOTIFY)CallApp_DrawTextScroll,
            //                                            pMe);
            //(void) ISHELL_CancelTimer(pMe->m_pShell,
            //                                            (PFNNOTIFY)CallApp_NotifyDisp_CB,
            //                                            pMe);
#ifndef FEATURE_USES_LOWMEM
            if(pMe->m_pConvImage)
            {
                IIMAGE_Release(pMe->m_pConvImage);
                pMe->m_pConvImage = NULL;
            }
#endif
            ICONFIG_SetItem( pMe->m_pConfig, CFGI_BEEP_VOL, &keyBeepVolumeSetting, sizeof(byte));
            return TRUE;

        case EVT_FLIP:// wParam = TRUE if open, FALSE if closed.
            //CALL_ERR("Calling EVT_FLIP %d", wParam, 0, 0);
            if(CallApp_Process_EVT_FLIP_Event(pMe,wParam))
            {
                pMe->m_userCanceled = TRUE;
            }
            return TRUE;

        case EVT_KEY:
            switch ((AVKType)wParam)
            {
                case AVK_CLR:
                case AVK_ENDCALL:					
                    CALL_ERR("AVK_ENDCALL %d", pMe->m_lastCallState,0,0);
#ifdef FEATURE_ICM
                    ICM_EndAllCalls(pMe->m_pICM);
#else
                    ICALLMGR_EndAllCalls(pMe->m_pICallMgr);
#endif
                    pMe->m_userCanceled = TRUE;
                    switch (pMe->m_lastCallState)
                    {
#ifdef FEATURE_ICM
                        case AEECM_CALL_STATE_ORIG:
#else
                        case AEET_CALL_STATE_ORIG:
#endif
                            // Note:
                            // We are allowing the IPHONE notifier to move us to the
                            // next state...
                            break;

                        default:
                            //CALL_ERR("Unexpected phone state (%d)", pMe->m_lastCallState, 0, 0);
                            /*lint -fallthrough*/
#ifdef FEATURE_ICM
                        case AEECM_CALL_STATE_IDLE:
#else
                        case AEET_CALL_STATE_IDLE:
#endif
                            // The Calling dialog will sometimes be displayed
                            // before the phone actually enters the origination state.
                            // If this is the case then we may need to manually
                            // move back to the Idle dialog because there will be no
                            // IPHONE notification to do so.
                            CLOSE_DIALOG(DLGRET_OK)
                            break;
                    }
					
                    return TRUE;

                default:
                    break;
            }
            break;
#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
		case EVT_PEN_UP:
			{
				AEEDeviceInfo devinfo;
				int nBarH ;
				AEERect rc;
				int16 wXPos = (int16)AEE_GET_X(dwParam);
				int16 wYPos = (int16)AEE_GET_Y(dwParam);
			
				nBarH = GetBottomBarHeight(pMe->m_pDisplay);
					
				MEMSET(&devinfo, 0, sizeof(devinfo));
				ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
				SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);
			
				if(CALLAPP_PT_IN_RECT(wXPos,wYPos,rc))
				{
					if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
					{
						boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_USER,AVK_SELECT,0);
						return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)	&& wXPos < rc.x + (rc.dx/3)*2 )//左
					{
						boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_USER,AVK_INFO,0);
						return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
					{						
						boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_USER,AVK_CLR,0);
						return rt;
					}
				}
			}
			break;
#endif 

        default:
            break;
    }

    return FALSE;
} // CallAppCallingDlgHandler

/*==============================================================================
函数：
       CallApp_Dialer_Show_Animation
说明：
       绘制通话时的显示动画和变化字符。

参数：
       pUser [in]：void指针

返回值：

备注：

==============================================================================*/
static void CallApp_Dialer_Show_Animation(void *pUser)
{
    AEERect   rect;
    AECHAR    wBuf[40] ={0};
    static byte calling_coute = 0;
    CCallApp *pMe = (CCallApp *)pUser;
#if FEATURE_DIALER_ANIMAION_SUPPORT	
    AEEImageInfo ImageSize;
    int            nCurrentFrame = 0;
#endif


    (void) ISHELL_CancelTimer(pMe->m_pShell, CallApp_Dialer_Show_Animation,pMe);
   
    if(calling_coute <= CALLAPP_ANI_FRAMES)
    {
        calling_coute++;
    }
    else
    {
        calling_coute = 0;
    }
     
    if(pMe->m_b_draw_dot) //此判断用来决定是否绘制字符串后面的点
    {
        RGBVAL     oldColor = 0;
        //calling.....
        SETAEERECT(&rect,CALL_TEXT_X,CALL_FIRST_LINE_Y,CALL_TEXT_DX,CALL_LINE_HIGHT);

        (void) ISHELL_LoadResString(pMe->m_pShell,
                                                AEE_APPSCALLAPP_RES_FILE,
                                                IDS_CALLING_DIALOG_TEXT,
                                                wBuf,
                                                sizeof(wBuf));
        //WSTRLCPY(wBuf, L"calling", 20*sizeof(AECHAR));

        /*if(calling_coute == 4)
        {
            calling_coute =0;
        }*/

        if((calling_coute/2)%4 == 1)
        {
            WSTRCAT(wBuf, L".");
        }
        if((calling_coute/2)%4 == 2)
        {
            WSTRCAT(wBuf, L"..");
        }
        if((calling_coute/2)%4 == 3)
        {
            WSTRCAT(wBuf, L"...");
        }
        
        Appscommon_ResetBackgroundEx(pMe->m_pDisplay, &rect, TRUE);

        oldColor = IDisplay_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, CALLAPP_TEXT_COLOR);
        (void)IDISPLAY_DrawText(pMe->m_pDisplay,
                                                AEE_FONT_NORMAL,
                                                wBuf,
                                                -1,
                                                rect.x,
                                                rect.y,
                                                &rect,
                                                IDF_ALIGN_LEFT|IDF_TEXT_TRANSPARENT);
        IDisplay_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, oldColor);
    }

    (void) ISHELL_SetTimer(pMe->m_pShell,
	                        400,
	                        (PFNNOTIFY)CallApp_Dialer_Show_Animation,
	                        (void *)pMe);


    IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
}
/*==============================================================================
函数：
       CallApp_Dialer_Connect_DlgHandler
说明：
       IDD_CONVERSATION 对话框事件处理函数

参数：
       pMe [in]：指向CallApp Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/
#if defined( FEATURE_CALL_RECORDER)
static void CallApp_Dialer_Connect_Record( CCallApp* pme);

static void CallApp_Dialer_Connect_Record_Forward( CCallApp* pme)
{

	recorder_set_media_path_to_forward( &pme->m_Media);
	CallApp_Dialer_Connect_Record( pme);
}

static void CallApp_Dialer_Connect_Record_Reverse( CCallApp* pme)
{

	recorder_set_media_path_to_reverse( &pme->m_Media);
	CallApp_Dialer_Connect_Record( pme);
}

static void CallApp_Dialer_Connect_Turn_On_Recorder( CCallApp* pme)
{
	CallApp_Dialer_Connect_Record_Forward( pme);
}

static void CallApp_Dialer_Connect_Turn_Off_Recorder( CCallApp* pme)
{

	ISHELL_CancelTimer( pme->m_pShell, (PFNNOTIFY)CallApp_Dialer_Connect_Turn_Off_Recorder, pme);
	pme->m_bRecorderOn = FALSE;
	recorder_stop_if( &pme->m_Media);
	repaint( TRUE);
}

static void recorder_media_event_handler( void* pUser, MediaEventNotify* pEventNotify)
{

	CCallApp* pme = (CCallApp*)pUser;

	switch( pEventNotify->eEvent)
	{
		case MEDIA_EVENT_STARTED:
			CallApp_SetupCallAudio( pme);
			break;

		case MEDIA_EVENT_TICK_UPDATE:
		case MEDIA_EVENT_TOTAL_TIME:
			break;

		case MEDIA_EVENT_TIME_LIMIT_REACHED:
		case MEDIA_EVENT_ERROR:
		case MEDIA_EVENT_SPACE:
			ISHELL_SetTimer( pme->m_pShell, 5000, (PFNNOTIFY)CallApp_Dialer_Connect_Turn_Off_Recorder, pme);
			break;
	}
}

static void CallApp_Dialer_Connect_Record( CCallApp* pme)
{
    //Add by pyuangui 2013-01-09
    #ifdef FEATURE_VERSION_C316
    char FilePath[AEE_MAX_FILE_NAME] = {0};
    if(CallApp_FindMemoryCardExist(pme))
    {
		 char strBuf[AEE_MAX_FILE_NAME] = {0};
		 STRCPY(FilePath, MG_MASSCARDCALLRECOED_PATH);
		 if(WSTRLEN(pme->m_CallsTable->call_name) > 0)
		 {
	         WSTRTOSTR(pme->m_CallsTable->call_name,strBuf,sizeof(strBuf));
	         STRCAT(FilePath, strBuf);
			 STRCAT(FilePath, DIRECTORY_STR);
		 }
		 else
		 {
             WSTRTOSTR(pme->m_CallsTable->call_number,strBuf,sizeof(strBuf));
	         STRCAT(FilePath, strBuf);
			 STRCAT(FilePath, DIRECTORY_STR);
		 }
         MSG_FATAL("****pyg****CallRecord---FilePath=%s",FilePath,0,0);
          
         if(SUCCESS != IFILEMGR_Test(pme->m_pFileMgr, FilePath))
         {
             MSG_FATAL("****pyg****CallRecord---FilePath=%s",FilePath,0,0);
             IFILEMGR_MkDir(pme->m_pFileMgr, FilePath);
         }
    }
	#endif
	//Add End
	if( recorder_get_memo_number( &pme->m_Media) < RECORDER_FILE_NUMBER_LIMIT)
	{
		pme->m_bRecorderOn 		= TRUE;
		pme->m_Media.m_eState 	= MEDIA_STATE_INIT;
//		repaint( FALSE);

		recorder_stop_if( &pme->m_Media);
		recorder_create_media_if( &pme->m_Media);
#if 0 //handfree时候录音声音变大
		recorder_set_media_volume( &pme->m_Media, AEE_MAX_VOLUME);
#endif
		recorder_set_media_event_notify( &pme->m_Media, recorder_media_event_handler, pme);
        //Add by pyuangui 2013-01-10
        #ifdef FEATURE_VERSION_C316
		recorder_process_media_operation_result( &pme->m_Media, recorder_call_record(  &pme->m_Media,FilePath), MEDIA_STATE_READY);
		#else
		recorder_process_media_operation_result( &pme->m_Media, recorder_record(  &pme->m_Media), MEDIA_STATE_READY);
		#endif
		//Add End
	}
	else
	{
		pme->m_Media.m_bMediaError 			= TRUE;
		pme->m_Media.m_nLastOperationError 	= 10;
	}

	if( pme->m_Media.m_bMediaError)
	{
		ISHELL_SetTimer( pme->m_pShell, 5000, (PFNNOTIFY)CallApp_Dialer_Connect_Turn_Off_Recorder, pme);
	}
}

static void drawText( IDisplay* pDisplay,
		AECHAR   *text,
		AEERect  *rect,
		RGBVAL   theTextColor,
		AEEFont  font,
		uint32   dwAlignFlags
)
{

	RGBVAL  oldColor       = 0;
	int     textLength     = ( text == NULL) ? 0 : WSTRLEN( text);
	int     textOffset     = 0;
	int     fontHeight     = 0;
	int     y              = rect->y;
	int     lineNumber     = 0;
	int     maxLineNumber  = 0;
	AEERect oldClip        = {0};
	AEERect rectDrawing    = *rect;

	if( pDisplay == NULL || rect == NULL || textLength == 0)
	{
		return;
	}

	fontHeight = IDISPLAY_GetFontMetrics( pDisplay, font, 0, 0);
	oldColor   = IDISPLAY_SetColor( pDisplay, CLR_USER_TEXT, theTextColor);
	IDISPLAY_GetClipRect( pDisplay, &oldClip);
	IDISPLAY_SetClipRect( pDisplay, rect);

	maxLineNumber = rectDrawing.dy / fontHeight;

	do
	{

		int     fits = 0;
		boolean newLineFound = FALSE;

		if( IDISPLAY_MeasureTextEx( pDisplay,
							  font,
							  text + textOffset,
							  textLength - textOffset,
							  rectDrawing.dx,
							  &fits
					   ) == 0
		)
		{
			break;
		}


		{
			AECHAR* result = WSTRCHR( text + textOffset, '\\');
			int     length = result - text - textOffset;
			if( result != NULL && (length + textOffset < textLength) && result[1] == 'n')
			{
				newLineFound = TRUE;
				fits = min( fits, length);
			}
			else
			{
				newLineFound = FALSE;
			}
		}

		textOffset += fits;
		lineNumber ++;

		if( newLineFound)
		{
			textOffset += 2;
		}
	}
	while( textOffset < textLength);

	y = max( rectDrawing.y - ( maxLineNumber - 1) * fontHeight / 2,
			rectDrawing.y - ( lineNumber - 1) * fontHeight / 2);
	rectDrawing.y = y;
	textOffset = 0;

	do
	{

		int     fits = 0;
		AECHAR  line[64];
		boolean newLineFound = FALSE;

		if( IDISPLAY_MeasureTextEx( pDisplay,
							  font,
							  text + textOffset,
							  textLength - textOffset,
							  rectDrawing.dx,
							  &fits
					   ) == 0
		)
		{
			break;
		}

		{
			AECHAR* result = WSTRCHR( text + textOffset, '\\');
			int     length = result - text - textOffset;
			if( result != NULL && (length + textOffset < textLength) && result[1] == 'n')
			{
				newLineFound = TRUE;
				fits = min( fits, length);
			}
			else
			{
				newLineFound = FALSE;
			}
		}
		WSTRNCOPYN( line, 64, text + textOffset, fits);
		IDISPLAY_DrawText( pDisplay,
					font,
					line,
					-1,
					rectDrawing.x,
					y,
					&rectDrawing,
					dwAlignFlags | IDF_TEXT_TRANSPARENT
				);

		textOffset      += fits;
		y               += fontHeight;
		rectDrawing.y   += fontHeight;

		if( newLineFound)
		{
			textOffset += 2;
		}
	}
	while( textOffset < textLength);

	IDISPLAY_SetColor( pDisplay, CLR_USER_TEXT, oldColor);
	IDISPLAY_SetClipRect( pDisplay, &oldClip);
}

static void drawText1( CCallApp* pme, AECHAR* text, AEERect* pclip, uint32 dwAlignFlags, RGBVAL color)
{

	drawText( pme->m_pDisplay, text, pclip, color, AEE_FONT_NORMAL, dwAlignFlags);
}

static void drawText3( CCallApp* pme, AEERect* pclip, char* resFile, uint16 resId, RGBVAL color)
{

	AECHAR text[256]   = {0};
	ISHELL_LoadResString( pme->m_pShell,
	                      resFile,
						  resId,
						  text,
						  sizeof( text)
					  );

	drawText( pme->m_pDisplay, text, pclip, color, AEE_FONT_NORMAL, IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);
}

static void drawText2( CCallApp* pme, AEERect* pclip, uint16 resId, RGBVAL color)
{
	drawText3( pme, pclip, APP_RECORDER_RES_FILE, resId, color);
}
static void recorder_show_media_spec( CCallApp* ref, RGBVAL color)
{

	Media* pme = &ref->m_Media;

	uint16 resIds[]  = { IDS_MM_STATUS_INIT,
				IDS_MM_STATUS_READY,
				IDS_MM_STATUS_PAUSE,
				IDS_MM_STATUS_DONE,
				IDS_MM_STATUS_RECORDING,
				IDS_MM_STATUS_PLAYING,
				IDS_MM_STATUS_DATA_IO_DELAY
			};
	static 			index	= 0;
	int				height  = 40;
	AEERect			rect    = {0};

	// animation
//	IIMAGE_GetInfo( image, &info);
//	x = ( ref->m_rc.dx - info.cx) / 2;
//	y = ref->m_rc.dy - GetBottomBarHeight( ref->m_pDisplay) - height + ( height - info.cy) / 2;
//	IIMAGE_Draw( image, x, y);

	SETAEERECT( &rect,
				0,
				ref->m_rc.dy - GetBottomBarHeight( ref->m_pDisplay)*2 - height - 2,
				ref->m_rc.dx,
				height
			);
//	drawBgMask( ref, &rect);
	if( pme->m_bMediaError)
	{
		extern uint16 recorder_get_res_id_from_error_code( Media* pme);
		uint16 resId = recorder_get_res_id_from_error_code( pme);
		drawText2( ref, &rect, resId, color);
	}
	else
	{

		index = ( index + 1) % 4;
		if( pme->m_nElapsedTime > 0)
		{
			extern void recorder_get_time_string( Media* pme, AECHAR time[]);
			AECHAR  time[32] = {0};

			pme->m_eTimeDisplayType = MEDIA_TIME_DISPLAY_TYPE_ELAPSED;
			recorder_get_time_string( pme, time);
			drawText1( ref, time, &rect, IDF_ALIGN_CENTER | IDF_ALIGN_BOTTOM, color);
		}

		if( ( pme->m_eState >= MEDIA_STATE_INIT && pme->m_eState <= MEDIA_STATE_PAUSED) ||
			pme->m_eState == MEDIA_STATE_BUFFERING
		)
		{
			drawText2( ref, &rect, resIds[pme->m_eState], color);
		}
//		else if( pme->m_eState == MEDIA_STATE_RECORDING)
//		{
//			uint16 resId = pme->m_bReverse ? IDS_RECORD_SELF : IDS_RECORD_THE_OTHER_SIZE;
//			drawText3( ref, &rect, resId, IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);
//		}
	}

	{
		uint16 resId = ( pme->m_bReverse ? IDS_RECORD_THE_OTHER_SIZE : IDS_RECORD_SELF);

		SETAEERECT( &rect,
				0,
				ref->m_rc.dy - GetBottomBarHeight( ref->m_pDisplay)*2,
				ref->m_rc.dx,
				GetBottomBarHeight( ref->m_pDisplay)
			);
		
		#ifndef FEATURE_VERSION_C316    //modi by pyuangui 2013-01-09
		drawText3( ref, &rect, AEE_APPSCALLAPP_RES_FILE, resId, color);
		#endif
	}

	IDISPLAY_Update( ref->m_pDisplay);
}


static void CallApp_build_recorder_option_menu( CCallApp *pMe, IMenuCtl* pMenu)
{

	if( recorder_get_memo_number( &pMe->m_Media) < RECORDER_FILE_NUMBER_LIMIT
	)
	{
		uint16 resId = pMe->m_bRecorderOn ? IDS_OPTION_RECORDER_OFF : IDS_OPTION_RECORDER_ON;
		(void) IMENUCTL_AddItem( pMenu, AEE_APPSCALLAPP_RES_FILE, resId, resId, 0, 0);
	}
}
#endif


static boolean  CallApp_Dialer_Connect_DlgHandler(CCallApp *pMe,
                                        AEEEvent    eCode,
                                        uint16      wParam,
                                        uint32      dwParam)
{
    PARAM_NOT_REF(dwParam)
    CALL_ERR("eCode= %x,w=%x,dw=%x CallApp_Dialer_Connect_DlgHandler ",eCode,wParam,dwParam);

    if ( pMe->m_bShowPopMenu && IMENUCTL_IsActive(pMe->m_pMenu) )
    {
        if ( IMENUCTL_HandleEvent(pMe->m_pMenu, eCode, wParam, dwParam) )
        {
            return TRUE;
        }
    }
    if(pMe->m_bShowPopMenu &&(eCode == EVT_KEY || eCode == EVT_KEY_PRESS || eCode == EVT_KEY_RELEASE)&&(wParam != AVK_SELECT && (AVKType)wParam != AVK_CLR && (AVKType)wParam != AVK_UP && (AVKType)wParam != AVK_DOWN && (AVKType)wParam != AVK_SEND && (AVKType)wParam != AVK_END))
	{
		return TRUE;
	}
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
#if defined( FEATURE_RECORDER)
        	CallApp_SetupCallAudio(pMe);
#endif
			
            pMe->m_cdg_row = 0;
            //pMe->m_b_show_cdg = TRUE;
            pMe->m_dtmf_length = OEMNV_KEYTONE_NORMAL;
            ICONFIG_GetItem(pMe->m_pConfig,CFGI_KEYTONE_LENGTH,&pMe->m_dtmf_length,sizeof(keyToneLength));
#ifndef FEATURE_USES_LOWMEM
            if(pMe->m_pConvImage)
            {
                IIMAGE_Release(pMe->m_pConvImage);
                pMe->m_pConvImage = NULL;
            }
#endif
            if(pMe->m_pActiveDlg)
            {
                IDIALOG_SetProperties(pMe->m_pActiveDlg,DLG_NOT_SET_FOCUS_AUTO);
            }
            return TRUE;

        case EVT_DIALOG_START:
        {
//Add By zzg 2010_08_03		  
#ifdef FEATURE_FRENDUO
			Dialer_call_table *str = NULL;
			str = CallApp_Get_First_Entry_In_Call_Table(pMe);

			if (((WSTRLEN(str->call_number) == 3) && (WSTRNCMP(str->call_number, L"*55", 3) == 0))
				||((WSTRLEN(str->call_number) == 4) && (WSTRNCMP(str->call_number, L"*550", 4) == 0)))			

			//if((WSTRNCMP(str->call_number, L"*55", 3) == 0) || (WSTRNCMP(str->call_number, L"*550", 4) == 0))
			{ 
			  (void)ISHELL_SetTimer(pMe->m_pShell, CALL_TIMER_FRENDUO,  CallApp_SetFrenduoTimer, pMe);
			}		 
#endif 
//Add End
		  
#ifdef FEATURE_APP_PAUSE_TIMER
            if (pMe->m_TimerString[0] != 0)
            {
                (void)ISHELL_SetTimer(pMe->m_pShell, CALL_TIMER_SET + 1000,
                                                        CallApp_SetTimerControl, pMe);
            }
#endif //FEATURE_APP_PAUSE_TIMER
            if(pMe->m_pBacklight)
            {
                IBACKLIGHT_Enable(pMe->m_pBacklight);
            }
            pMe->m_bShowPopMenu = FALSE;

            IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_CALL/*ANNUN_FIELD_CALLFORWARD*/, ANNUN_STATE_CALL_INUSE_ON/*ANNUN_STATE_ON*/);
            pMe->m_bRefreshVol = REFRESHVOL_OFF; //don't display Vol
            // put db item DB_IN_IDLE true
            //CallApp_Set_Db_In_Idle(TRUE);

            (void) ISHELL_PostEvent(pMe->m_pShell,AEECLSID_DIALER/*AEECLSID_CALL*/,EVT_USER_REDRAW,0, 0);

#if defined( FEATURE_CALL_RECORDER)
        	pMe->m_bSettingVolume = FALSE;
#endif

            return TRUE;
        }

        case EVT_USER_REDRAW:
        {
#if defined( FEATURE_CALL_RECORDER)
        	if( pMe->m_bShowPopMenu)
        	{
        		return TRUE;
        	}
#endif
            if( pMe->m_bShowPopMenu)
        	{
        		return TRUE;
        	}
            Appscommon_ResetBackgroundEx(pMe->m_pDisplay, &pMe->m_rc, TRUE);

            //for CDG test, CNAP with Forwarding
            
            CallApp_Draw_Connect_Softkey(pMe);
            CallApp_Draw_Connect_Number_and_Name(pMe);			
            CallApp_Draw_Connect_Time(pMe);

#ifdef FEATURE_CARRIER_VENEZUELA_MOVILNET
            if (pMe->m_CallMuted ==TRUE)
                IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_CALL/*ANNUN_FIELD_MUTE*/, ANNUN_STATE_CALL_MUTE_ON/*ANNUN_STATE_ON*/);
            else
                IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_CALL/*ANNUN_FIELD_MUTE*/, ANNUN_STATE_CALL_MUTE_OFF/*ANNUN_STATE_OFF*/);
#endif //FEATURE_CARRIER_VENEZUELA_MOVILNET

#if defined( FEATURE_CALL_RECORDER)
        if( pMe->m_bRecorderOn && !pMe->m_bShowPopMenu)
        {
        	recorder_show_media_spec( pMe, RGB_WHITE);
        }

        if( pMe->m_bSettingVolume)
        {
			CallApp_RefreshVolBar( pMe);
        }
#endif

//Add by pyuangui 20121231
#ifdef  FEATURE_CALL_RECORDER
      if(pMe->m_bRecorderOn == FALSE)
       {
	#if defined(FEATURE_VERSION_C316) || defined(FEATURE_VERSION_W317A)
        boolean m_autocallrecord;
        (void) ICONFIG_GetItem(pMe->m_pConfig,
                                         CFGI_AUTOCALLRECORD,
                                         &m_autocallrecord,
                                         sizeof(m_autocallrecord));
                                         
         if (m_autocallrecord) 
	#endif
         {
           pMe->m_bRecorderOn = TRUE;            
		   CallApp_Dialer_Connect_Turn_On_Recorder( pMe);
         }
 
       }
#endif
//Add End
            return TRUE;
        }

        case EVT_DIALOG_END:
          //  (void)ISHELL_CancelTimer(pMe->m_pShell, CallApp_SetPauseControl, pMe);   
          
          //Add By zzg 2010_08_03		
#ifdef FEATURE_FRENDUO
			{
			    Dialer_call_table* p_temp = NULL;
                p_temp = CallApp_Get_First_Entry_In_Call_Table(pMe);

				if (((WSTRLEN(p_temp->call_number) == 3) && (WSTRNCMP(p_temp->call_number, L"*55", 3) == 0))
					||((WSTRLEN(p_temp->call_number) == 4) && (WSTRNCMP(p_temp->call_number, L"*550", 4) == 0)))
				//if((WSTRNCMP(p_temp->call_number, L"*55",3) == 0) || (WSTRNCMP(p_temp->call_number, L"*550",4) == 0))
				{					
				  	(void) ISHELL_CancelTimer(pMe->m_pShell, CallApp_SetFrenduoTimer, pMe); 
                    
                    CallApp_SendFrenduoSMS();
				}
			}		   
#endif 
        //Add End
	  
#ifdef FEATURE_APP_PAUSE_TIMER
            (void) ISHELL_CancelTimer(pMe->m_pShell, CallApp_SetTimerControl, pMe);
#endif //FEATURE_APP_PAUSE_TIMER
            (void) ISHELL_CancelTimer(pMe->m_pShell, CallApp_Draw_Connect_Time, pMe);

            //add for CDG test, CNAP with Forwarding
            //(void) ISHELL_CancelTimer(pMe->m_pShell,
            //                                        (PFNNOTIFY)CallApp_DrawTextScroll,
            //                                        pMe);
            //(void) ISHELL_CancelTimer(pMe->m_pShell,
            //                                        (PFNNOTIFY)CallApp_NotifyDisp_CB,
            //                                        pMe);
            ISHELL_CancelTimer(pMe->m_pShell,
                                                    (PFNNOTIFY)CallApp_HandleDialogTimer_Redraw,
                                                    pMe);

            //CallApp_Set_Db_In_Idle(FALSE);

            pMe->m_bShowPopMenu = FALSE;
            IMENUCTL_SetActive ( pMe->m_pMenu, FALSE );
#ifndef FEATURE_USES_LOWMEM
            if(pMe->m_pConvImage)
            {
                IIMAGE_Release(pMe->m_pConvImage);
                pMe->m_pConvImage = NULL;
            }
#endif
            return TRUE;

        case EVT_FLIP:// wParam = TRUE if open, FALSE if closed.
            //CALL_ERR("Connect EVT_FLIP %d", wParam, 0, 0);
            CallApp_Process_EVT_FLIP_Event(pMe,wParam);
            return TRUE;

        case EVT_KEY_PRESS:
        {
            boolean vol_add = FALSE;
            if ( pMe->m_bShowPopMenu )
            {
                break;
            }

#if defined( FEATURE_CALL_RECORDER)
            {
            	boolean setVolumeKey = ( wParam == AVK_UP ||
										 wParam == AVK_VOLUME_UP ||
										 wParam == AVK_DOWN ||
										 wParam == AVK_VOLUME_DOWN
									);
				if( setVolumeKey || ( !setVolumeKey && pMe->m_bSettingVolume))
				{
					ISHELL_CancelTimer( pMe->m_pShell, (PFNNOTIFY)CallApp_HandleDialogTimer_Redraw, pMe);
					pMe->m_bSettingVolume = setVolumeKey;
					(void)ISHELL_PostEvent( pMe->m_pShell, AEECLSID_DIALER, EVT_USER_REDRAW, 0, 0 );
				}

				if( setVolumeKey)
				{
					ISHELL_SetTimer( pMe->m_pShell, 2000, CallApp_HandleDialogTimer_Redraw, pMe);
				}
            }
#endif


            // The UP/DOWN keys need to be processed on the EVT_KEY_PRESS
            // event because the softkey will hijack the EVT_KEY event.
            switch ((AVKType)wParam)
            {
                case AVK_UP:
                case AVK_VOLUME_UP:
#ifdef FEATURE_ALL_KEY_PAD                     
                case AVK_O:
#endif                    
                    vol_add = TRUE;

                case AVK_DOWN:
                case AVK_VOLUME_DOWN:
#ifdef FEATURE_ALL_KEY_PAD                     
                case AVK_I:
#endif                         
                    CallApp_ChangeCallVolume(pMe, vol_add);

#if !defined( FEATURE_CALL_RECORDER)
					CallApp_RefreshVolBar(pMe);
					IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
#endif
                    return TRUE;

				
				case AVK_CAMERA:
#if defined(FEATURE_VERSION_C306)||defined(FEATURE_VERSION_W0216A)|| defined(FEAUTRE_VERSION_N450)|| defined(FEATURE_VERSION_N021)|| defined(FEATURE_VERSION_C01)|| defined(FEATURE_VERSION_W516) || defined(FEATURE_VERSION_W208S) || defined(FEATURE_VERSION_W027)
				{
					nv_item_type	SimChoice;
					OEMNV_Get(NV_SIM_SELECT_I,&SimChoice);
					if(SimChoice.sim_select==AVK_SEND_TWO)
					{
	                	return TRUE;  //make the cm 2312 disappear
	                }
                }
#endif
                break;
                case AVK_SEND: 
                    return TRUE;  //make the cm 2312 disappear
                #if defined(FEATURE_VERSION_S1000T)
   				//case AVK_RWD:  //add by yangdecai 2012-06-09
   				#else
                case AVK_RWD:
                #endif
                    CallApp_ChangeCallVolume_AVK_RWD(pMe);
                    
#if !defined( FEATURE_CALL_RECORDER)
                    CallApp_RefreshVolBar(pMe);
                    IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
#endif

                    return TRUE;
                default:
                    break;
            }

            break;
        }

        case EVT_KEY_HELD:
            switch ((AVKType)wParam){
            #ifdef FEATURE_VERSION_C337
            case AVK_STAR:
                CallApp_Process_HeldKey_Event(pMe,eCode,wParam,dwParam);
                return TRUE;
            #endif    
            #if defined(FEATURE_VERSION_S1000T)
   			//case AVK_RWD:  //add by yangdecai 2012-06-09
   			#else
            case AVK_RWD:
            #endif
                ISHELL_PostEvent( pMe->m_pShell, AEECLSID_DIALER,EVT_USER_REDRAW,0,0 );
 
                if (HS_HEADSET_ON())
                {
                    pMe->m_userCanceled = TRUE;
 #ifdef FEATURE_ICM
                    ICM_EndAllCalls(pMe->m_pICM);
 #else
                    ICALLMGR_EndAllCalls(pMe->m_pICallMgr);
 #endif
                }
                else
                {
                    pMe->m_bHandFree = !pMe->m_bHandFree;
                    CallApp_SetupCallAudio(pMe);
                }
                return TRUE;  //make the dialog can't closed by avk_clr.
                
            case AVK_SEND://CDG 3-way call need send fwi 
            {
                CallApp_Flash_Call(pMe);
                return TRUE;
            }
            default:
                break;
            }      
            break;

        case EVT_KEY:
            if ( pMe->m_bShowPopMenu && (AVK_LEFT != wParam) && (AVK_RIGHT!= wParam))
            {
                // cancel the pop menu...
                pMe->m_bShowPopMenu = FALSE;
                IMENUCTL_SetActive ( pMe->m_pMenu, FALSE );
                (void)ISHELL_PostEvent( pMe->m_pShell, AEECLSID_DIALER/*AEECLSID_CALL*/,EVT_USER_REDRAW,0,0 );
                return TRUE;
            }

            switch ((AVKType)wParam)
            {
                case AVK_CLR:
                    //ICM_EndAllCalls(pMe->m_pICM);
                    
                    ISHELL_PostEvent( pMe->m_pShell, AEECLSID_DIALER,EVT_USER_REDRAW,0,0 );

                    if (HS_HEADSET_ON())
					{
					    pMe->m_userCanceled = TRUE;
#ifdef FEATURE_ICM
                        ICM_EndAllCalls(pMe->m_pICM);
#else
                        ICALLMGR_EndAllCalls(pMe->m_pICallMgr);
#endif
					}
                    else
                    {
                        //modi by yangdecai
                        pMe->m_bHandFree = !pMe->m_bHandFree;
                        CallApp_SetupCallAudio(pMe);
                    }
                    return TRUE;  //make the dialog can't closed by avk_clr.

                case AVK_ENDCALL:				
					
                    //CALL_ERR("AVK_ENDCALL", 0,0,0);
                    // End call
                    //ASSERT(AEECM_CALL_STATE_CONV == pMe->m_lastCallState);		
                    
                    pMe->m_userCanceled = TRUE;
#ifdef FEATURE_ICM
                    ICM_EndAllCalls(pMe->m_pICM);
#else
                    ICALLMGR_EndAllCalls(pMe->m_pICallMgr);
#endif

					
                    // Note:
                    // We are allowing the IPHONE notifier to move us to the
                    // next state...
                    return TRUE;
#if defined(FEATURE_VERSION_C306)||defined(FEATURE_VERSION_W0216A)|| defined(FEAUTRE_VERSION_N450)|| defined(FEATURE_VERSION_N021)|| defined(FEATURE_VERSION_C01)|| defined(FEATURE_VERSION_W516)||defined(FEATURE_VERSION_W208S) || defined(FEATURE_VERSION_W027)
                case AVK_CAMERA:
                {
					nv_item_type	SimChoice;
					OEMNV_Get(NV_SIM_SELECT_I,&SimChoice);
					if(SimChoice.sim_select==AVK_SEND_TWO)
					{
						// Make sure aren't waiting for a hard pause to be released...
                    
	                    if (pMe->m_PauseString[0] != 0)
	                    {
	                      CallApp_SetPauseControl(pMe);
	                        return TRUE;
	                    }

	                    // Don't send if we are PIN locked
	                    // (ie. in an emergency call)
	                    if (pMe->idle_info.uimLocked)
	                    {
	                        return TRUE;
	                    }

#ifdef FEATURE_IS2000_SCC_CODES
	                    // Remove the 'Release Hold' softkey if necessary
	                    if (pMe->m_bAnswerHold)
	                    {
	                        pMe->m_bAnswerHold = FALSE;
	                        CallApp_Draw_Connect_Softkey(pMe);
	                    }
#endif /* FEATURE_IS2000_SCC_CODES */

	                    //modify for Three-Way Calling : Hold Key not supported by MS
	                    CallApp_Flash_Call(pMe);
	                    return TRUE;
					}
                    break;
                }
#endif

                case AVK_SEND:
                    // Make sure aren't waiting for a hard pause to be released...
                    #if defined(FEATURE_VERSION_C01) 
					{
						nv_item_type	SimChoice;
						OEMNV_Get(NV_SIM_SELECT_I,&SimChoice);
						if(SimChoice.sim_select==AVK_SEND_TWO)
						{
							 return TRUE;
						}
					}
					#endif
                    if (pMe->m_PauseString[0] != 0)
                    {
                        CallApp_SetPauseControl(pMe);
                        return TRUE;
                    }

                    // Don't send if we are PIN locked
                    // (ie. in an emergency call)
                    if (pMe->idle_info.uimLocked)
                    {
                        return TRUE;
                    }

#ifdef FEATURE_IS2000_SCC_CODES
                    // Remove the 'Release Hold' softkey if necessary
                    if (pMe->m_bAnswerHold)
                    {
                        pMe->m_bAnswerHold = FALSE;
                        CallApp_Draw_Connect_Softkey(pMe);
                    }
#endif /* FEATURE_IS2000_SCC_CODES */

                    //modify for Three-Way Calling : Hold Key not supported by MS
                    CallApp_Flash_Call(pMe);
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
                case AVK_STAR:
                case AVK_POUND:
                {
                    // Pass the digit on to the dialer dialog
                    char          digit[2];
                    AECHAR   w_str[2] = {0};
                    MSG_FATAL("AVK_1-AVK_9", 0, 0, 0);
                    //keyToneLength keyLen;
                    if(pMe->m_bShowPopMenu)
                	{
                		return TRUE;
                	}
                    // Don't start the incall dialer if we are PIN locked
                    // (ie. in an emergency call)
                    if (pMe->idle_info.uimLocked)
                    {
                        return TRUE;
                    }

                    // Make sure aren't waiting for a hard pause to be released...
                    if (pMe->m_PauseString[0] != 0)
                    {
                        return TRUE;
                    }

                    digit[0] = CallApp_AVKType2ASCII((AVKType) wParam);
                    digit[1] = '\0';
                    pMe->m_DialString[0] = 0;//init m_DialString
                    STR_TO_WSTR(digit,pMe->m_DialString, sizeof(pMe->m_DialString));//I see.the fisrt key to numedit
                    STRTOWSTR(digit,w_str,sizeof(w_str));

                    // Play/Start the DTMF tone
                    //ICONFIG_GetItem(pMe->m_pConfig,CFGI_KEYTONE_LENGTH,&pMe->m_dtmf_length,sizeof(keyToneLength));

                    //if (OEMNV_KEYTONE_LONG == pMe->m_dtmf_length)
                    //{
                    // The tone will be stopped in CIlde_DialerHandleEvent()
                    // function on the EVT_KEY_RELEASE event...
                    //    CallApp_StartContDTMF(pMe,w_str);
                    //}
                    //else
                    //{
                    //    CallApp_BurstDTMF(pMe,w_str,1);
                    //}
                    CallApp_Send_DTMF(pMe , w_str);
                    return TRUE;
                }

                case AVK_SELECT:
                {
                    if ( !pMe->m_bShowPopMenu )
                    {
                        pMe->m_bShowPopMenu = TRUE;
                        CallApp_Build_Connect_Option_Menu ( pMe );
                        CallApp_Draw_Connect_Softkey(pMe);
                        //IDISPLAY_Update ( pMe->m_pDisplay );
                    }
                }
                return TRUE;

#if defined( FEATURE_CALL_RECORDER)
                case AVK_INFO:
					if( pMe->m_bRecorderOn)
					{
						if( !pMe->m_Media.m_bMediaError &&
							pMe->m_Media.m_eState == MEDIA_STATE_RECORDING
						)
						{
							if( pMe->m_Media.m_bReverse)
							{
								CallApp_Dialer_Connect_Record_Forward( pMe);
							}
							else
							{
								CallApp_Dialer_Connect_Record_Reverse( pMe);
							}

						}
					}
					return TRUE;
#endif

                default:
                    break;
            }
            break;
        case EVT_KEY_RELEASE:
            switch(wParam){
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
            case AVK_STAR:
            case AVK_POUND:
            {
                MSG_FATAL("AVK_1-AVK_9 %d %d", pMe->idle_info.uimLocked, pMe->m_PauseString[0], 0);
                // Don't start the incall dialer if we are PIN locked
                // (ie. in an emergency call)
                if (pMe->idle_info.uimLocked)
                {
                    return TRUE;
                }
                if(pMe->m_bShowPopMenu)
            	{
            		return TRUE;
            	}
                // Make sure aren't waiting for a hard pause to be released...
                if (pMe->m_PauseString[0] != 0)
                {
                    return TRUE;
                }
                
                if (OEMNV_KEYTONE_LONG == pMe->m_dtmf_length)
                {
#ifdef FEATURE_ICM
                    CallApp_Stop_ContDTMF(pMe->m_pICM);
#else
                    CallApp_Stop_ContDTMF(pMe);
#endif
                }
                CLOSE_DIALOG(DLGRET_CONV_DIAL)
                return TRUE;
            }
            default:
                break;
            }
            break;
            
        case EVT_COMMAND:
            pMe->m_bShowPopMenu = FALSE;
            IMENUCTL_SetActive ( pMe->m_pMenu, FALSE );
            switch (wParam)
            {
                case IDS_RELEASE:
                    CallApp_ConvReleasePKey(pMe);
                    //CallApp_Draw_Connect_Number_and_Name(pMe);
                    //CallApp_Draw_Connect_Softkey(pMe);
                    break;

                /*jinqing add,2009/01/08*/
                //case IDS_HANDFREE:
                //case IDS_NONHANDFREE:
                //    pMe->m_bHandFree = !pMe->m_bHandFree;
                //    CallApp_SetupCallAudio(pMe);
                //    break;
                /*jinqing add,2009/01/08*/
                case IDS_RECORD:
                    //Recording proccess adds here
                    break;
#ifdef FEATURE_SUPPORT_BT_APP
                case IDS_USE_BT_AG:
                case IDS_USE_PHONE:
                    if(wParam == IDS_USE_PHONE)
                    {
                        pMe->m_bt_audio = SEND_NONE;
                    }
                    else
                    {
                        pMe->m_bt_audio = SEND_BT_AUDIO;
                    }

                    pMe->m_bHandFree = FALSE;
                    CallApp_SetupCallAudio(pMe);
                    break;
#endif

				//Add By zzg 2011_10_28
#ifdef FEATURE_SUPPORT_BT_AUDIO
				case IDS_USE_BT_AG:
				case IDS_USE_PHONE:
				{						
					if (wParam == IDS_USE_PHONE)
					{						
						MSG_FATAL("***zzg CallApp_Dialer_Connect IDS_USE_PHONE***", 0, 0, 0);
						CallApp_SwitchCallAudio(pMe, FALSE);						
                    }
                    else
                    {
                    	MSG_FATAL("***zzg CallApp_Dialer_Connect IDS_USE_BT_AG***", 0, 0, 0);

						//CallApp_SwitchCallAudio(pMe, TRUE);	
						
						//Add By zzg 2012_12_05

#ifdef FEATURE_SUPPORT_BT_AUDIO			
						(void) ISHELL_PostEvent(pMe->m_pShell,
												AEECLSID_BLUETOOTH_APP,
												EVT_USER,
												EVT_CALLAPP_CHECK_BT_STATUS,	
												EVT_CALLAPP_USE_BT_HEADSET);			
#endif						
						//Add End											
                    }
					pMe->m_bHandFree = FALSE;
					CallApp_SetupCallAudio(pMe);
					break;
				}
#endif
				//Add End
				case IDS_MUTE_OFF:
				case IDS_MUTE_ON:
                case IDS_MUTE:
                case IDS_UNMUTE:
//Add by pyuangui 2013-01-17					
#ifdef FEATURE_VERSION_C337
                     if(wParam == IDS_MUTE) 
					 	IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_CALL/*ANNUN_FIELD_MUTE*/, ANNUN_STATE_CALL_MUTE_ON/*ANNUN_STATE_ON*/);
					 else
					 	IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_CALL/*ANNUN_FIELD_MUTE*/, ANNUN_STATE_CALL_MUTE_OFF/*ANNUN_STATE_OFF*/);
#endif
//Add End                    
                    pMe->m_CallMuted = !pMe->m_CallMuted;
                    CallApp_SetupCallAudio(pMe);
                    //CallApp_Draw_Connect_Softkey(pMe);
                    break;

#ifdef FEATURE_IS2000_SCC_CODES
                case IDS_RELEASEHOLD:
                    ASSERT(pMe->m_bAnswerHold);
                    CallApp_Flash_Call(pMe);
                    pMe->m_bAnswerHold = FALSE;
                    //CallApp_Draw_Connect_Softkey(pMe);
                    break;
#endif /* FEATURE_IS2000_SCC_CODES */

                case IDS_SK_MENU:
                    CallApp_LaunchApplet(pMe,  AEECLSID_MAIN_MENU);
                    return TRUE;

#if defined( FEATURE_CALL_RECORDER)
                case IDS_OPTION_RECORDER_ON:
                case IDS_OPTION_RECORDER_OFF:
                {
                	pMe->m_bRecorderOn = wParam == IDS_OPTION_RECORDER_ON;

                	if( pMe->m_bRecorderOn)
                	{
						CallApp_Dialer_Connect_Turn_On_Recorder( pMe);
                	}
                	else
                	{
                		CallApp_Dialer_Connect_Turn_Off_Recorder( pMe);
                	}
                }
                break;
#endif

                case IDS_CONTACT: 
				case IDS_PHONE_BOOKS:
                    CallApp_LaunchApplet(pMe,  AEECLSID_APP_CONTACT);
                    return TRUE;

                case IDS_SMS:
                    CallApp_LaunchApplet(pMe,  AEECLSID_WMSAPP);
                    return TRUE;
                default:
                    break;
            }
            (void)ISHELL_SendEvent( pMe->m_pShell, AEECLSID_DIALER,EVT_USER_REDRAW,0,0 );
            return TRUE;

            default:
                break;
        }
    return FALSE;
} // CallAppConvDlgHandler


/*==============================================================================
函数：
       CallAppEndcallDlgHandler
说明：
       IDD_ENDCALL对话框事件处理函数

参数：
       pMe [in]：指向CallApp Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/
static boolean  CallApp_Dialer_Callend_DlgHandler(CCallApp *pMe,
                                           AEEEvent    eCode,
                                           uint16      wParam,
                                           uint32      dwParam)
{
    //IImage * pImage = NULL;
    PARAM_NOT_REF(dwParam)
    PARAM_NOT_REF(wParam)
    CALL_ERR("eCode= %x,w=%x,dw=%x CallApp_Dialer_Callend_DlgHandler ",eCode,wParam,dwParam);

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
        {
            Dialer_call_table* p_temp = NULL;
            p_temp = CallApp_Get_First_Entry_In_Call_Table(pMe);

            if(p_temp)
            {
                MEMCPY(&pMe->m_call_info,p_temp,sizeof(Dialer_call_table));
            }
            if(pMe->m_pBacklight)
            {
                IBACKLIGHT_Enable(pMe->m_pBacklight);
            }

            CallApp_Free_All_Call_Table(pMe);
#ifndef FEATURE_USES_LOWMEM
            if(pMe->m_pConvImage)
            {
                IIMAGE_Release(pMe->m_pConvImage);
                pMe->m_pConvImage = NULL;
            }
#endif
            pMe->m_b_callend_time = 0;

#if defined( FEATURE_CALL_RECORDER)
            CallApp_Dialer_Connect_Turn_Off_Recorder( pMe);
#endif

#if defined(FEATURE_VERSION_C337)   	
            pMe->m_isIncoming = FALSE;
#endif
            return TRUE;
        }

        case EVT_DIALOG_START:
            //pMe->IncomIsready = FALSE;

			//Add By zzg 2012_10_30
			#ifndef FEATURE_VERSION_W317A
            IANNUNCIATOR_SetField ( pMe->m_pIAnn, ANNUN_FIELD_LOCKSTATUS/*ANNUN_FIELD_VOICE_PRIVACY*/, ANNUN_STATE_VOICE_PRIV_OFF/*ANNUN_STATE_OFF*/); 
			#endif
			//Add End
						
			IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_CALL/*ANNUN_FIELD_CALLFORWARD*/, ANNUN_STATE_CALL_INUSE_OFF/*ANNUN_STATE_OFF*/);

			//Add By pyuangui 2013-01-17
			#ifdef FEATURE_VERSION_C337
			IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_CALL/*ANNUN_FIELD_MUTE*/, ANNUN_STATE_CALL_MUTE_OFF/*ANNUN_STATE_OFF*/);
            #endif
			//Add End
			
			//CallApp_Set_Db_In_Idle(TRUE);
            (void) ISHELL_PostEvent(pMe->m_pShell,  AEECLSID_DIALER, EVT_USER_REDRAW, 0, 0);
            return TRUE;

        case EVT_USER_REDRAW:
        {
            ITimeCtl *pTimerCtl = NULL;
            AEERect   rect;
            //int       bOffset;
            boolean   bShowTime = TRUE;
            Dialer_call_table* p_temp = NULL;
            AEEALERTType m_alertType = AEEALERT_ALERT_NONE;
            pTimerCtl = (ITimeCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDDC_ENDCALL_TIMER);
            
            Appscommon_ResetBackgroundEx(pMe->m_pDisplay, &pMe->m_rc, TRUE);

            // Position banner on first line
            if(pMe->m_pBacklight)
            {
                IBACKLIGHT_Enable(pMe->m_pBacklight);
            }
            SETAEERECT(&rect,
                                            CALL_TEXT_X,
                                            CALL_FIRST_LINE_Y,
                                            CALL_TEXT_DX,
                                            CALL_LINE_HIGHT);

            // Call was terminated abormally, display either 'Call Failed'
            // or 'Call Lost' depending on the if the phone was in
            // the conversation state or not.
#ifdef FEATURE_ICM
            if ( (pMe->m_callEndStatus != AEECM_CALL_END_CLIENT_END)
            	&&(pMe->m_callEndStatus != AEECM_CALL_END_REL_NORMAL) )
#else
            if ( (pMe->m_callEndStatus != AEET_CALL_END_CLIENT_END)
            	&&(pMe->m_callEndStatus != AEET_CALL_END_REL_NORMAL) )
#endif
            {

                // Display the normal 'Call Ended' if the call was terminated
                // due to an incoming call (see IS-95B 7.6.44).  However we
                // still play a Call Fade alert in the earpiece to notify the
                // user.   This will hopefully cause the user to pull the phone
                // away from their head to:
                // 1) See the new incoming call (as they will need to press send
                //    to answer it).
                // 2) Avoid having the ringer go off right in their ear.
                //
#ifdef FEATURE_ICM
                if (pMe->m_callEndStatus != AEECM_CALL_END_INCOM_CALL)
#else
                if (pMe->m_callEndStatus != AEET_CALL_END_INCOM_CALL)
#endif
                {
                    AECHAR wBuf[MAX_SIZE_BANNER_TEXT+1];

                    (void) ISHELL_LoadResString(pMe->m_pShell,
                                                        AEE_APPSCALLAPP_RES_FILE,
                                                        pMe->m_callEndInOrig ?
                                                        IDS_CALLFAILED : IDS_CALLLOST,
                                                        wBuf,
                                                        sizeof(wBuf));
                    
                    IDisplay_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, CALLAPP_TEXT_COLOR);
                    (void)IDISPLAY_DrawText(pMe->m_pDisplay,
                                                        AEE_FONT_NORMAL,
                                                        wBuf,
                                                        -1,
                                                        CALL_TEXT_X,
                                                        CALL_FIRST_LINE_Y,
                                                        &rect,
                                                        IDF_TEXT_TRANSPARENT);
                    IDisplay_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, RGB_BLACK);
                }

                if (pMe->m_callEndInOrig)
                {
                    // Don't display the call timer if no call was established
                    bShowTime = FALSE;
                }

                // Alert the user
                switch (pMe->m_callEndStatus)
                {
#ifdef FEATURE_ICM
                    case AEECM_CALL_END_FADE:
#else
                    case AEET_CALL_END_FADE:
#endif
                        m_alertType = AEEALERT_ALERT_FADE_TONE;
                        break;
#ifdef FEATURE_ICM
                    case AEECM_CALL_END_ACC_FAIL:
                    case AEECM_CALL_END_REORDER:
#else
                    case AEET_CALL_END_ACC_FAIL:
                    case AEET_CALL_END_REORDER:
#endif
                        m_alertType = AEEALERT_ALERT_ABBR_REORDER;
                        break;
#ifdef FEATURE_ICM
                    case AEECM_CALL_END_INTERCEPT:
#else
                    case AEET_CALL_END_INTERCEPT:
#endif
                        m_alertType = AEEALERT_ALERT_INTERCEPT;
                        break;
#ifdef FEATURE_ICM
                    case AEECM_CALL_END_UIM_NOT_PRESENT:
#else
                    case AEET_CALL_END_UIM_NOT_PRESENT:
#endif
                        // If the call failed due because the R-UIM card
                        // was removed (or failed), move to the Insert Card
                        // screen...
#ifdef Temp_remove
                        if(IsRunAsUIMVersion())
                        {
                            (void) ISHELL_PostEvent(pMe->m_pShell, AEECLSID_CORE_APP,OEMEVT_RUIM_DOOR_OPEN,0,0);
                        }
#endif/*Temp_remove*/
                        break;

                    default:
                        // Play a generic beep to entice the user to look at the
                        // display as something bad has happened.
                        m_alertType = AEEALERT_ALERT_REORDER;
                        break;
                }
                //CallAppNotifyMP3PlayerAlertEvent(pMe,TRUE);
                IALERT_StartAlerting(pMe->m_pAlert, AEEALERT_CALLTYPE_VOICE, NULL, m_alertType);
            }
            else//normal call end
            {
#ifdef FEATURE_CARRIER_TELEFONICA_VENEZUELA
                AECHAR wBuf_new[25];
#else
                AECHAR wBuf_new[MAX_SIZE_BANNER_TEXT+1];
#endif /* FEATURE_CARRIER_TELEFONICA_VENEZUELA */
                //DRAW CALL_END in first line
                (void) ISHELL_LoadResString(pMe->m_pShell,
                                                        AEE_APPSCALLAPP_RES_FILE,
                                                        IDS_ENDCALL_DIALOG_TEXT,/*call end*/
                                                        wBuf_new,
                                                        sizeof(wBuf_new));
                
                IDisplay_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, CALLAPP_TEXT_COLOR);
                (void)IDISPLAY_DrawText(pMe->m_pDisplay,
                                                        AEE_FONT_NORMAL,
                                                        wBuf_new,
                                                        -1,
                                                        CALL_TEXT_X,
                                                        CALL_FIRST_LINE_Y,
                                                        &rect,
                                                        IDF_TEXT_TRANSPARENT|IDF_ALIGN_CENTER);
                IDisplay_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, RGB_BLACK);
            }

            //  Position CALL TIMER  in thrid line
            SETAEERECT(&rect,
                                                        CALL_TEXT_X,		
                                                        CALL_SECOND_LINE_Y,
                                                        CALL_TEXT_DX,
                                                        CALL_LINE_HIGHT);
            
            //always hide time control, use CallApp_HandleEndCallBlinkTimer() to show time
            //if (!bShowTime)
            {
                // Hide the time control by positioning it off the
                // bottom of the screen
                rect.y = 5000;
            }

            ITIMECTL_SetRect(pTimerCtl, &rect);
            ITIMECTL_SetProperties(pTimerCtl, TP_NO_MSECONDS);
#ifndef FEATURE_USES_LOWMEM
            if(pMe->m_pConvImage == NULL)
            {
                pMe->m_pConvImage = ISHELL_LoadResImage( pMe->m_pShell,
                                                    AEE_APPSCOMMONRES_IMAGESFILE,
                                                    IDI_REFUI_ENDCALL);
            }

            if(pMe->m_pConvImage)
            {
                //IIMAGE_SetParm(pMe->m_pConvImage, IPARM_ROP, AEE_RO_TRANSPARENT, 0);
                IIMAGE_Draw(pMe->m_pConvImage, CALL_TEXT_X, CALL_THIRD_LINE_Y);
                //IIMAGE_Release(pImage);
                //pImage = NULL;
                //CALL_ERR("Draw call end  image",0,0,0);
            }
#endif
            if(pMe->m_Is3Way)
            {
                //  Position NAME
                // Display on secend line
                AECHAR mui_call[MAX_SIZE_NAME_TEXT] ={0};
#if defined(FEATURE_VERSION_C337)  				
                (void) ISHELL_LoadResString(pMe->m_pShell,
                                                        AEE_APPSCALLAPP_RES_FILE,
							    IDS_MULTICALL,                                                       
                                                        mui_call,
                                                        sizeof(mui_call));
#else
                (void) ISHELL_LoadResString(pMe->m_pShell,
                                                        AEE_APPSCALLAPP_RES_FILE,
                                                        IDS_MUTI_CALL,                                                        
                                                        mui_call,
                                                        sizeof(mui_call));
#endif

                SETAEERECT(&rect,
                                                        CALL_NAME_X,
                                                        CALL_THIRD_LINE_Y,
                                                        CALL_NAME_DX,
                                                        CALL_LINE_HIGHT);
                IDisplay_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, CALLAPP_TEXT_COLOR);
#if defined(FEATURE_VERSION_C01) ||defined(FEATURE_VERSION_C11) ||defined(FEATURE_VERSION_C180)||defined(FEATURE_VERSION_W027V3)|| defined(FEATURE_VERSION_H1201)
             SETAEERECT(&rect,
                                                        CALL_NAME_X-20,
                                                        CALL_THIRD_LINE_Y+30,
                                                        CALL_NAME_DX+20,
                                                        CALL_LINE_HIGHT);
             (void)IDISPLAY_DrawText(pMe->m_pDisplay,
                                                        AEE_FONT_NORMAL,
                                                        mui_call,
                                                        -1,
                                                        rect.x,
                                                        rect.y,
                                                        &rect,
                                                        IDF_ALIGN_LEFT|IDF_TEXT_TRANSPARENT);
            #else
#ifndef FEATURE_VERSION_C316								
             (void)IDISPLAY_DrawText(pMe->m_pDisplay,
                                                        AEE_FONT_NORMAL,
                                                        mui_call,
                                                        -1,
                                                        CALL_NAME_X,
                                                        CALL_THIRD_LINE_Y,
                                                        &rect,
                                                        IDF_ALIGN_LEFT|IDF_TEXT_TRANSPARENT);
#endif
#endif
            
                IDisplay_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, CALLAPP_TEXT_COLOR);
            }
            else
            {
                //Dialer_call_table * temp = NULL;
                p_temp = &pMe->m_call_info;
                //temp = CallApp_Get_First_Entry_In_Call_Table(pMe);
                if(p_temp == NULL)
                {
                    CALL_ERR("Warning:temp == NULL",0,0,0);
                    //(void)ISHELL_CloseApplet(pMe->m_pShell, FALSE);
                    CLOSE_DIALOG(DLGRET_OK)
                    return TRUE;
                }
                if(p_temp->in_phonebook)
                {
                    //  Position NAME
                    // Display on secend line
                    SETAEERECT(&rect,
                                                        CALL_NAME_X,
                                                        CALL_THIRD_LINE_Y,
                                                        CALL_NAME_DX,
                                                        CALL_LINE_HIGHT);

                    CallApp_DrawText_Ex(pMe, AEE_FONT_NORMAL, p_temp->call_name, &rect, IDF_TEXT_TRANSPARENT);

                }
                //  Position NUMBER
                // Position on last line, to the right
                //bOffset = IDISPLAY_MeasureText(pMe->m_pDisplay, AEE_FONT_LARGE,
                //                                        temp->call_number);

                //if (bOffset > pMe->m_rc.dx)
                //{
                //    bOffset = pMe->m_rc.dx;
                //}
                if(p_temp->in_phonebook)
                {
                    SETAEERECT(&rect,
                                                        CALL_NUM_X, //CALL_TEXT_X, 
                                                        CALL_FOURTH_LINE_Y,
                                                        CALL_NUM_DX, //CALL_TEXT_DX,
                                                        CALL_LINE_HIGHT);
                }
                else
                {
                    SETAEERECT(&rect,
                                                        CALL_NAME_X,
                                                        CALL_THIRD_LINE_Y,
                                                        CALL_NAME_DX,
                                                        2 *CALL_LINE_HIGHT);
                }

                if(WSTRNCMP(p_temp->call_number, L"PRIVATE"/*pMe->wszPrivateString*/,7) == 0)
                {
                  (void) ISHELL_LoadResString(pMe->m_pShell,
                                            AEE_APPSCALLAPP_RES_FILE,
                                            IDS_NO_NUMBER,
                                            pMe->wszPrivateString_tw,
                                            sizeof(pMe->wszPrivateString_tw));
                  /////////////////////////////////////////////////////////////////////////
                    IDisplay_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, CALLAPP_TEXT_COLOR);
                    (void)IDISPLAY_DrawText(pMe->m_pDisplay,
                                                        AEE_FONT_NORMAL,
                                                        pMe->wszPrivateString_tw,
                                                        -1,
                                                        rect.x,
                                                        rect.y,
                                                        &rect,
                                                        IDF_TEXT_TRANSPARENT);
                    IDisplay_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, RGB_BLACK);
                }
                else
                {
                    CallApp_DrawText_Ex(pMe, AEE_FONT_NORMAL,
                            p_temp->call_number, &rect, IDF_TEXT_TRANSPARENT);
                }
            }
            //  Set CALL TIMER
            //p_temp = CallApp_Get_First_Entry_In_Call_Table(pMe);
            p_temp = &pMe->m_call_info;
            if(p_temp == NULL)
            {
                return FALSE;
            }

            (void) ITIMECTL_SetTime(pTimerCtl, (int32)(p_temp->end_time - p_temp->start_time));
            //(void)ISHELL_SetTimer(pMe->m_pShell,
            //                                            INCOMINGCALLISREADY_TIMER,
            //                                            CallApp_IncomingCall_IsReady_Timer,
            //                                            pMe);
            // Start overall timer
            //(void) ISHELL_SetTimer(pMe->m_pShell,
            //                                            TIMEOUT_MS_ENDCALLTIMER,
            //                                            CallApp_HandleDialogTimer,
            //                                            pMe);

            if (bShowTime)
            {
                // Start blink timer
                CallApp_HandleEndCallBlinkTimer(pMe);
            }
            else
            {
                // Start overall timer
                ISHELL_SetTimer(pMe->m_pShell,
                                                        TIMEOUT_MS_ENDCALLTIMER,
                                                        CallApp_HandleDialogTimer,
                                                        pMe);
            }
            //DRAW SOFTKEY
            //drawBottomBar(pMe->m_pDisplay,AEE_FONT_NORMAL,BTBAR_CLOSE);
            REFUI_DRAW_BOTTOMBAR(BTBAR_CLOSE)
            //CallApp_Draw_Softkey(pMe, 0, IDS_CANCEL);
            ITIMECTL_SetActive(pTimerCtl, FALSE);
            IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            return TRUE;

        }

        case EVT_DIALOG_END:
            //CallApp_Set_Db_In_Idle(FALSE);
            
            IALERT_StopAlerting(pMe->m_pAlert);
            CallAppNotifyMP3PlayerAlertEvent(pMe,FALSE);
            //IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE);
            //CallApp_Free_All_Call_Table(pMe);//Free all call table
            // Cancel overall timer
            (void) ISHELL_CancelTimer(pMe->m_pShell,
                                                        CallApp_HandleDialogTimer, pMe);

            // Cancel blink timer
            (void) ISHELL_CancelTimer(pMe->m_pShell,
                                                        CallApp_HandleEndCallBlinkTimer, pMe);
#ifndef FEATURE_USES_LOWMEM
            if(pMe->m_pConvImage)
            {
                IIMAGE_Release(pMe->m_pConvImage);
                pMe->m_pConvImage = NULL;
            }
#endif
#ifdef FEATURE_ICM
            pMe->m_lastCallState = AEECM_CALL_STATE_IDLE;
#else
            pMe->m_lastCallState = AEET_CALL_STATE_IDLE;
#endif
            return TRUE;
        //case EVT_INCOMISREADY:
        //    pMe->IncomIsready = TRUE;
        //    return TRUE;

        case EVT_KEY:
            //if(pMe->IncomIsready == FALSE)
            //{
            //    return TRUE;
            //}
            
        {
#if defined(FEATURE_VERSION_M8) || defined(FEATURE_VERSION_M8P) || defined(FEATURE_VERSION_M8021)
            //M8 For Frenduo
            Dialer_call_table* callname_str = NULL;
            callname_str = &pMe->m_call_info;			

			if (((WSTRLEN(callname_str->call_number) == 3) && (WSTRNCMP(callname_str->call_number, L"*55", 3) == 0))
					||((WSTRLEN(callname_str->call_number) == 4) && (WSTRNCMP(callname_str->call_number, L"*550", 4) == 0)))					
			//if((WSTRNCMP(callname_str->call_number, L"*55", 3) == 0) || (WSTRNCMP(callname_str->call_number, L"*550", 4) == 0))
			{ 			   
			  //CLOSE_DIALOG(DLGRET_OK)       
			}	
            else 
            {               
                CLOSE_DIALOG(DLGRET_OK)
            }
#else           
            CLOSE_DIALOG(DLGRET_OK)
#endif
            return TRUE;
        }
        case EVT_DISPLAYDIALOGTIMEOUT:
			
            CLOSE_DIALOG(DLGRET_OK)
            return TRUE;
#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
		case EVT_PEN_UP:
			{
				AEEDeviceInfo devinfo;
				int nBarH ;
				AEERect rc;
				int16 wXPos = (int16)AEE_GET_X(dwParam);
				int16 wYPos = (int16)AEE_GET_Y(dwParam);
						
				nBarH = GetBottomBarHeight(pMe->m_pDisplay);
								
				MEMSET(&devinfo, 0, sizeof(devinfo));
				ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
				SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);
						
				if(CALLAPP_PT_IN_RECT(wXPos,wYPos,rc))
				{
					if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
					{
						boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_USER,AVK_SELECT,0);
						return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)	&& wXPos < rc.x + (rc.dx/3)*2 )//左
					{
						boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_USER,AVK_INFO,0);
						return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
					{						
						boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_USER,AVK_CLR,0);
						return rt;
					}
				}
			}
			break;
#endif

        default:
            break;
    }

    return FALSE;
} // CallAppEndcallDlgHandler
/*==============================================================================
函数：
       CallAppMsgBoxDlgHandler
说明：
       IDD_MSGBOX 对话框事件处理函数

参数：
       pMe [in]：指向CallApp Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/
static boolean  CallApp_MsgBox_DlgHandler(CCallApp  *pMe,
                                          AEEEvent     eCode,
                                          uint16       wParam,
                                          uint32       dwParam)
{
    static IStatic * m_pIStatic = NULL;
    
    if (NULL == pMe)
    {
        return FALSE;
    }

    if(pMe->m_pActiveDlg == NULL)
    {
        return FALSE;
    }
    CALL_ERR("eCode= %x,w=%x,dw=%x CallApp_MsgBox_DlgHandler ",eCode,wParam,dwParam);

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
        {
            if (NULL == m_pIStatic)
            {
                AEERect rect = {0};
                
                if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_STATIC,(void **)&m_pIStatic))        
                {
                    CALL_ERR("ISHELL_CreateInstance,AEECLSID_STATIC 2",0,0,0);
                    return FALSE;            
                }        
                ISTATIC_SetRect(m_pIStatic, &rect);  
            }
        }
            return TRUE;

        case EVT_DIALOG_START:
            if(pMe->m_msg_text_id != IDS_INVALIDEMGNUM && pMe->m_msg_text_id != IDS_NOOMH_CARD && pMe->m_msg_text_id != IDS_SAR && pMe->m_msg_text_id != IDS_SAR_C337
                || (pMe->m_msg_text_id == IDS_REGULATE_SUCCEED)|| (pMe->m_msg_text_id == IDS_REGULATE_FAIL))
            {
                ISHELL_SetTimer(pMe->m_pShell, TIMEOUT_MS_INVALIDEMGNUMDIALOG_TIMER,
                                       CallApp_HandleDialogTimer, pMe);
            }
            ISHELL_PostEvent(pMe->m_pShell, AEECLSID_DIALER, EVT_USER_REDRAW, 0,0);
            return TRUE;

        case EVT_USER_REDRAW:
        {
            PromptMsg_Param_type m_PromptMsg={0};
            AECHAR  wstrText[MSGBOX_MAXTEXTLEN];

			MSG_FATAL("***zzg CallApp_MSG_Handle m_msg_text_id=%d***", pMe->m_msg_text_id, 0, 0);
            
            switch(pMe->m_msg_text_id)
            {
                case IDS_VOICEMAIL_EMPTY:
                case IDS_RESTRICT_OUTGOING:
                case IDS_SPEED_DIAL_EMPTY:
                case IDS_INVALIDEMGNUM:
                case IDS_INVALID_IP_NUMBER:
                case IDS_MSG_EXCEPTIONAL_CHAR:
                case IDS_INVALID:
#ifdef FEATURE_SUPPORT_WAP_APP
#ifdef FEATURE_CARRIER_CHINA_TELCOM
                case IDS_INVALID_LINK_NUM:
#endif
#endif
                    m_PromptMsg.ePMsgType = MESSAGE_WARNNING; 
                    break;

                case IDS_EMGNUMBER_CALL_ONLY:
                    m_PromptMsg.ePMsgType = MESSAGE_INFORMATION;
                    break;

                case IDS_REGULATE_FAIL:
                    m_PromptMsg.ePMsgType = MESSAGE_WARNNING;
                    break;
                    
				case IDS_REGULATE_SUCCEED:
                    m_PromptMsg.ePMsgType = MESSAGE_INFORMATION;
                    break;
                    
                default:
                    m_PromptMsg.ePMsgType = MESSAGE_NONE;
                    break;
            }
            
            (void) ISHELL_LoadResString(pMe->m_pShell,
                                            AEE_APPSCALLAPP_RES_FILE,
                                            pMe->m_msg_text_id,
                                            wstrText,
                                            sizeof(wstrText));
            
            m_PromptMsg.pwszMsg = wstrText;
            if(pMe->m_msg_text_id == IDS_SPEED_DIAL_EMPTY)
            {
                AECHAR   wsDispMsg[MSGBOX_MAXTEXTLEN] = {0};
                
                WSPRINTF(wsDispMsg, sizeof(wsDispMsg), wstrText, pMe->m_DialString);
                m_PromptMsg.pwszMsg = wsDispMsg;
            }
            
            if(pMe->m_msg_text_id == IDS_INVALIDEMGNUM)
            {
                m_PromptMsg.eBBarType = BTBAR_OK_BACK;
            }
            else
            {
                m_PromptMsg.eBBarType = BTBAR_BACK;
            }
            DrawPromptMessage(pMe->m_pDisplay,m_pIStatic,&m_PromptMsg);
            IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE);
        }
            return TRUE;

        case EVT_DIALOG_END:
            if(pMe->m_msg_text_id != IDS_INVALIDEMGNUM)
            {
                ISHELL_CancelTimer(pMe->m_pShell,
                                  CallApp_HandleDialogTimer, pMe);
            }

            ISTATIC_Release(m_pIStatic); 
            m_pIStatic = NULL;
            return TRUE;

        case EVT_KEY_PRESS:
            if(pMe->m_msg_text_id == IDS_INVALIDEMGNUM)
            {
                switch (wParam)
                {
                    case AVK_SELECT:
#ifdef FEATURE_ICM
                        //exit the emergency mode
                        ICM_SetSystemPreference(pMe->m_pICM,
                                            AEECM_MODE_PREF_PERSISTENT, AEECM_PREF_TERM_PERMANENT, 0,
                                            AEECM_GW_ACQ_ORDER_PREF_NO_CHANGE, AEECM_BAND_PREF_NO_CHANGE,
                                            AEECM_ROAM_PREF_NO_CHANGE, AEECM_HYBR_PREF_NO_CHANGE,
                                            AEECM_SRV_DOMAIN_PREF_NO_CHANGE, AEECM_NETWORK_SEL_MODE_PREF_NO_CHANGE,
                                            NULL, NULL, NULL);
#else
                    {
                        AEETSystemPreference tSysPref={
														AEET_MODE_PREF_PERSISTENT, 
														AEET_PREF_TERM_PERMANENT,
														0,
								                        AEET_GW_ACQ_ORDER_PREF_NO_CHANGE,
								                        AEET_BAND_PREF_NO_CHANGE,
								                        AEET_ROAM_PREF_NO_CHANGE, 
								                        AEET_HYBR_PREF_NO_CHANGE,
								                        AEET_SRV_DOMAIN_PREF_NO_CHANGE, 
								                        AEET_NETWORK_SEL_MODE_PREF_NO_CHANGE,
							                            {0xFF,0xFF,0xFF}};  /*if we don,t use plmn set it to 0xff,0xff,0xff*/
		                //exit the emergency mode
		                IPHONECTL_SetSystemPreference(pMe->m_pIPhoneCtl,NULL,&tSysPref,sizeof(AEETSystemPreference),NULL);
                    }
#endif
                        break;

                    case AVK_ENDCALL:
                    case AVK_CLR:						
                        //back to emergency mode
                        break;

                    default:
                        //not process the another event key
                        return TRUE;

                }
            }
            else if((pMe->m_msg_text_id == IDS_NOOMH_CARD) || (pMe->m_msg_text_id == IDS_SAR) || (pMe->m_msg_text_id == IDS_SAR_C337)
                || (pMe->m_msg_text_id == IDS_REGULATE_SUCCEED)|| (pMe->m_msg_text_id == IDS_REGULATE_FAIL))
            {
                switch (wParam)
                {
                    case AVK_SELECT:
                    case AVK_ENDCALL:
                    case AVK_CLR:
                    case AVK_INFO:
                        pMe->m_bCloseAllApplet = FALSE;
                        CLOSE_DIALOG(DLGRET_OK)
                        return TRUE;
                        
                    default:
                        //not process the another event key
                        return TRUE;

                }
            }
            else if(pMe->m_msg_text_id == IDS_INVALID)
            {
                switch (wParam)
                {
                    case AVK_CLR:
                        pMe->m_bCloseAllApplet = FALSE;
                        CLOSE_DIALOG(DLGRET_OK)
                        return TRUE;
                        
                    default:
                        //not process the another event key
                        return TRUE;

                }
            }            
        case EVT_DISPLAYDIALOGTIMEOUT:
            pMe->m_bCloseAllApplet = FALSE;
            CLOSE_DIALOG(DLGRET_OK)
            return TRUE;

        default:
            break;
    }

    return FALSE;
}  // CallApp_MsgBox_DlgHandler()

//Add By zzg 2012_10_31
static boolean  CallApp_ProMpt_DlgHandler(CCallApp  *pMe,
                                          AEEEvent     eCode,
                                          uint16       wParam,
                                          uint32       dwParam)
{	
	static IStatic * m_pIStatic = NULL;
		
	if (NULL == pMe)
	{
		return FALSE;
	}

	if(pMe->m_pActiveDlg == NULL)
	{
		return FALSE;
	}
	
	CALL_ERR("eCode= %x,w=%x,dw=%x CallApp_ProMpt_DlgHandler ",eCode,wParam,dwParam);

	switch (eCode)
	{
		case EVT_DIALOG_INIT:
		{
			if (NULL == m_pIStatic)
			{
				AEERect rect = {0};
				
				if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_STATIC,(void **)&m_pIStatic))		  
				{
					CALL_ERR("ISHELL_CreateInstance,AEECLSID_STATIC 2",0,0,0);
					return FALSE;			 
				}		 
				ISTATIC_SetRect(m_pIStatic, &rect);  
			}
		}
			return TRUE;

		case EVT_DIALOG_START:			
			ISHELL_PostEvent(pMe->m_pShell, AEECLSID_DIALER, EVT_USER_REDRAW, 0,0);
			return TRUE;

		case EVT_USER_REDRAW:
		{
			PromptMsg_Param_type m_PromptMsg={0};
			AECHAR	wstrText[MSGBOX_MAXTEXTLEN];

			MSG_FATAL("***zzg CallApp_ProMpt_DlgHandler m_prompt_id=%d***", pMe->m_prompt_id, 0, 0);
			
			switch(pMe->m_prompt_id)
			{
				case IDS_SPEED_DIAL_QUERY:
					m_PromptMsg.ePMsgType = MESSAGE_CONFIRM;	
					break;
				
				default:
					m_PromptMsg.ePMsgType = MESSAGE_NONE;
					break;
			}
			
			(void) ISHELL_LoadResString(pMe->m_pShell,
											AEE_APPSCALLAPP_RES_FILE,
											pMe->m_prompt_id,
											wstrText,
											sizeof(wstrText));
			
			m_PromptMsg.pwszMsg = wstrText;
			if(pMe->m_prompt_id == IDS_SPEED_DIAL_QUERY)
			{
				AECHAR	 wsDispMsg[MSGBOX_MAXTEXTLEN] = {0};				
				WSPRINTF(wsDispMsg, sizeof(wsDispMsg), wstrText, pMe->m_DialString);
				m_PromptMsg.pwszMsg = wsDispMsg;
				m_PromptMsg.eBBarType = BTBAR_YES_NO;				
			}						
			else
			{
				m_PromptMsg.eBBarType = BTBAR_BACK;
			}
			
			DrawPromptMessage(pMe->m_pDisplay,m_pIStatic,&m_PromptMsg);
			IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE);
		}
			return TRUE;

		case EVT_DIALOG_END:	
			ISTATIC_Release(m_pIStatic); 
			m_pIStatic = NULL;
			return TRUE;

		case EVT_KEY_PRESS:			
			if(pMe->m_prompt_id == IDS_SPEED_DIAL_QUERY)
			{
				switch (wParam)
				{
					case AVK_CLR:
						pMe->m_bCloseAllApplet = FALSE;
						CLOSE_DIALOG(DLGRET_OK)
						return TRUE;
					case AVK_INFO:
					case AVK_SELECT:	
						ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APP_CONTACT);
						{
							boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER, PARAM_SPEED_DIAL, (uint16)ATOI((char *)pMe->m_DialString));
							MSG_FATAL("***zzg ISHELL_PostEvent AEECLSID_APP_CONTACT rt=%x", rt, 0, 0);
						}
						return TRUE;
					default:
						//not process the another event key
						return TRUE;

				}
			}	
		default:
			break;
	}
    return FALSE;
} 
//Add End



/*==============================================================================
函数：
       CallAppIncomingCallDlgHandler
说明：
       IDD_INCOMINGCALL对话框事件处理函数

参数：
       pMe [in]：指向CallApp Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/
static boolean  CallApp_IncomingCall_DlgHandler(CCallApp *pMe,
                                                AEEEvent    eCode,
                                                uint16      wParam,
                                                uint32      dwParam)
{

    static byte keyBeepVolumeSetting = 0;
    boolean bValue = FALSE;
    (void) ICONFIG_GetItem(pMe->m_pConfig,
                           CFGI_PHONE_PASSWORD_CHECK,
                           &bValue,
                           sizeof(bValue));

    //IImage  *pImage = NULL;
    PARAM_NOT_REF(dwParam)
    MSG_FATAL("eCode= %x,w=%x,dw=%x CallApp_IncomingCall_DlgHandler ",eCode,wParam,dwParam);

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            //pMe->IncomIsready = FALSE;
            pMe->m_cdg_row = 0;
            pMe->m_bincoming_rsk = IDS_MUTE;
            //pMe->m_CallMuted = TRUE;
            pMe->m_b_press_1=FALSE;
            //pMe->m_b_show_cdg = TRUE;
            pMe->m_b_miss_notify = TRUE;
            MSG_FATAL("CallApp_IncomingCall_DlgHandler EVT_DIALOG_INIT %d",pMe->m_b_incall,0,0);
            if(pMe->m_b_incall)
            {
                // play call waiting ring
                //IALERT_StartAlerting(pMe->m_pAlert, AEEALERT_CALLTYPE_VOICE,
                //                                    pMe->m_CallsTable->call_number, AEEALERT_ALERT_CALLWAITING);
            }
            else
            {
                           
                pMe->m_CallMuted = TRUE;
                Appscomm_is_incoming_state(1);
                CallApp_Play_Incoming_Tone(pMe);
                
#ifdef FEATURE_LED_CONTROL
                IBACKLIGHT_SigLedDisable(pMe->m_pBacklight);    //cancel the previous LED control
                IBACKLIGHT_SigLedEnable( pMe->m_pBacklight, SIG_LED_INCOMING_CALL);
#endif
            }
#ifndef FEATURE_USES_LOWMEM
            if(pMe->m_pConvImage)
            {
                IIMAGE_Release(pMe->m_pConvImage);
                pMe->m_pConvImage = NULL;
            }
#endif
            {
                byte mute = OEMSOUND_MUTE_VOL;
                ICONFIG_GetItem( pMe->m_pConfig, CFGI_BEEP_VOL, &keyBeepVolumeSetting, sizeof(byte));
                ICONFIG_SetItem( pMe->m_pConfig, CFGI_BEEP_VOL, &mute, sizeof(byte));
            }
            return TRUE;

        case EVT_DIALOG_START:
            //CallApp_Set_Db_In_Idle(TRUE);
            {
            	//add by yangdecai 2010-08-04
            	AECHAR StrBuf[20] = {0};
				boolean RingtonesPlaymode =FALSE;
               // boolean headsetPresent =FALSE;
				(void) ISHELL_LoadResString(pMe->m_pShell,
                                            AEE_APPSCALLAPP_RES_FILE,
                                            IDS_INCOMINGCALL_TEXT,//incoming call
                                            StrBuf,
                                            sizeof(StrBuf));
              //  if (SUCCESS != ICONFIG_GetItem(pMe->m_pConfig,CFGI_HEADSET_PRESENT,&headsetPresent,sizeof(headsetPresent)))
              //  {
              //      headsetPresent = FALSE;
              //  }
              
                /*
                OEM_GetConfig(CFGI_FM_PLAY_MODE,&RingtonesPlaymode, sizeof(RingtonesPlaymode));
			    
                if(RingtonesPlaymode ) //&& HS_HEADSET_ON()
                {
                  snd_set_device(SND_DEVICE_HANDSET, SND_MUTE_UNMUTED, SND_MUTE_UNMUTED, NULL, NULL);
                }
				*/

	            CallApp_IncomingCall_Dlg_Init(pMe);
#ifndef FEATURE_VERSION_C337								
				IANNUNCIATOR_SetHasTitleText(pMe->m_pIAnn, TRUE);
				IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,StrBuf);
				IANNUNCIATOR_SetHasTitleText(pMe->m_pIAnn, FALSE);
#endif
			    if(pMe->m_pBacklight)
	            {
	                IBACKLIGHT_Enable(pMe->m_pBacklight);
	            }


	            (void) ISHELL_PostEvent(pMe->m_pShell, AEECLSID_DIALER, EVT_USER_REDRAW,  0,  0);
#ifdef FEATURE_SUPPORT_BT_APP
#ifdef FEATURE_ICM
	            bt_ui_process_cmcall_notify(pMe,AEECM_EVENT_CALL_CALLER_ID, FALSE);
#else
	            bt_ui_process_cmcall_notify(pMe,AEET_EVENT_CALL_CALLER_ID, FALSE);
#endif
#endif
	            }
            return TRUE;
            
		case EVT_NO_CLOSEBACKLIGHT:
			//wParam 1:背光半亮, 0: 背光灭	
#if defined (FEATURE_VERSION_C337) || defined (FEATURE_VERSION_W317A)
			return TRUE;
#else
			if(wParam == 0)			
			{
			    return TRUE;
			}
#endif
            break;
            
        case EVT_USER_REDRAW:
        {
            //IMenuCtl *pSKMenu = NULL;
            AECHAR    wBuf[MAX_SIZE_BANNER_TEXT+1];
            AEERect   rect;//, rc;
            boolean b_cdg = FALSE;
            
            Appscommon_ResetBackgroundEx(pMe->m_pDisplay, &pMe->m_rc, TRUE);

            //  BANNER
            SETAEERECT(&rect,
                        CALL_TEXT_X,
                        CALL_FIRST_LINE_Y,
                        CALL_TEXT_DX,
                        CALL_LINE_HIGHT);

            if(pMe->m_b_incall == FALSE)//the first incoming call
            {
                (void) ISHELL_LoadResString(pMe->m_pShell,
                                            AEE_APPSCALLAPP_RES_FILE,
                                            IDS_INCOMINGCALL_TEXT,//incoming call
                                            wBuf,
                                            sizeof(wBuf));
            }
            else
            {
                //WSTRCPY(wBuf, L"Waiting call");
                (void) ISHELL_LoadResString(pMe->m_pShell,
                                            AEE_APPSCALLAPP_RES_FILE,
                                            IDS_WAITING_CALL,
                                            wBuf,
                                            sizeof(wBuf));
            }
            
            IDisplay_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, CALLAPP_TEXT_COLOR);
            (void)IDISPLAY_DrawText(pMe->m_pDisplay,
                                                    AEE_FONT_NORMAL,
                                                    wBuf,
                                                    -1,
                                                    rect.x,
                                                    rect.y,
                                                    &rect,
                                                    IDF_ALIGN_CENTER|IDF_TEXT_TRANSPARENT);
            IDisplay_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, RGB_BLACK);
            SETAEERECT(&rect,
                                                    CALL_SCREEN_X,
                                                    CALL_SECOND_LINE_Y,
                                                    CALL_SCREEN_DX,
                                                    CALL_THIRD_LINE_Y);
            //IDISPLAY_EraseRect(pMe->m_pDisplay, &rect);
            //add for CDG test, CNAP with Forwarding
            if(pMe->m_cdg_dsp_info.m_b_show_cdg)
            {
                CallApp_NotifyDisp(pMe);
                b_cdg = TRUE ;
            }

            //  NUMBER
            // Display immediately above the softkey
            //the incoming call and waiting call all in pMe->m_CallsTable
#ifdef FEATURE_ICM
            if(pMe->m_cdg_dsp_info.pi== AEECM_PI_ALLOW)
#else
            if(pMe->m_cdg_dsp_info.pi== AEET_PI_ALLOW)
#endif
            {
                if(pMe->m_CallsTable == NULL)
                {
                    CALL_ERR("incoming pMe->m_CallsTable == NULL",0,0,0);
                    pMe->m_CallsTable = (Dialer_call_table*) CALLAPP_MALLOC(sizeof(Dialer_call_table));
                    if(pMe->m_CallsTable)
                    {
                        MEMCPY(pMe->m_CallsTable,(void*)&pMe->m_call_info,sizeof(Dialer_call_table));
                        pMe->m_CallsTable_Count ++;
                    }
                    //return FALSE;
                }

                if(pMe->m_CallsTable->in_phonebook ||b_cdg)
                {
                    int16 dy = 0;
                    if(pMe->m_cdg_row == 0)//No CDG info
                    {
                        dy = 2* CALL_LINE_HIGHT;
                    }
                    else if (pMe->m_cdg_row == 1)
                    {
                        dy = CALL_LINE_HIGHT;
                    }
                    else
                    {
                        dy = 0;
                    }
                    SETAEERECT(&rect,
                               CALL_NUM_X, //CALL_TEXT_X,
                               CALL_THIRD_LINE_Y+pMe->m_cdg_row * CALL_LINE_HIGHT,
                               CALL_NUM_DX, //CALL_TEXT_DX,
                               dy);
                }
                else//no name
                {
                    /*预留出动画的宽度*/
                    int16 dy = 0;
                    if(pMe->m_cdg_row == 0)//No CDG info
                    {
                        dy = 3* CALL_LINE_HIGHT;
                    }
                    else if (pMe->m_cdg_row == 1)
                    {
                        dy = 2* CALL_LINE_HIGHT;
                    }
                    else
                    {
                        dy = CALL_LINE_HIGHT;
                    }
                    SETAEERECT(&rect,
                                                        CALL_NAME_X,
                                                        CALL_SECOND_LINE_Y+pMe->m_cdg_row * CALL_LINE_HIGHT,
                                                        CALL_NAME_DX,
                                                        dy);
                }

                if(WSTRNCMP(pMe->m_CallsTable->call_number, L"PRIVATE",7) == 0)
                {
                    (void) ISHELL_LoadResString(pMe->m_pShell,
                                                AEE_APPSCALLAPP_RES_FILE,
                                                IDS_NO_NUMBER,
                                                pMe->wszPrivateString_tw,
                                                sizeof(pMe->wszPrivateString_tw));
                    
                    IDisplay_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, CALLAPP_TEXT_COLOR);
                    (void)IDISPLAY_DrawText(pMe->m_pDisplay,
                                                        AEE_FONT_NORMAL,
                                                        pMe->wszPrivateString_tw,
                                                        -1,
                                                        rect.x,
                                                        rect.y,
                                                        &rect,
                                                        IDF_TEXT_TRANSPARENT);
                    IDisplay_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, RGB_BLACK);
                }
                else
                {
                	#if defined(FEATURE_VERSION_MTM)
                	MSG_FATAL("FEATURE_VERSION_MTM.........INCOMEING",0,0,0);
                	if(WSTRNCMP(pMe->m_CallsTable->call_number,L"+",1)==0)
                	{
                		AECHAR   call_Numberbuf[AEECM_MAX_DIGITS_LENGTH] = {0};
                        WSTRLCPY(call_Numberbuf,&pMe->m_CallsTable->call_number[1],AEECM_MAX_DIGITS_LENGTH);
                        CallApp_DrawText_Ex(pMe, AEE_FONT_NORMAL,
                                call_Numberbuf, &rect, IDF_TEXT_TRANSPARENT);
                	}
                	else
                	{
                		CallApp_DrawText_Ex(pMe, AEE_FONT_NORMAL,
                                pMe->m_CallsTable->call_number, &rect, IDF_TEXT_TRANSPARENT);
                    }
                	#else
                    {
                       #if defined (FEATURE_VERSION_W515V3)||defined(FEATURE_VERSION_S1000T)
                        AECHAR Temp_Number[AEECM_MAX_DIGITS_LENGTH] = {0};
                        if(WSTRNICMP(pMe->m_CallsTable->call_number,L"+00",3)==0)
                        {
                            Temp_Number[0]=L'+';
                            WSTRLCPY(Temp_Number+1,pMe->m_CallsTable->call_number+3,AEECM_MAX_DIGITS_LENGTH);
                            //WSTRCAT(Temp_Number, L"+");
                        }
                        else
                        {
                            WSTRLCPY(Temp_Number,pMe->m_CallsTable->call_number,AEECM_MAX_DIGITS_LENGTH);
                        }
                        // MSG_FATAL("pMe->m_CallsTable->call_number---=%d",pMe->m_CallsTable->call_number,0,0);
                       CallApp_DrawText_Ex(pMe, AEE_FONT_NORMAL,Temp_Number, &rect, IDF_TEXT_TRANSPARENT);
                      #else
                       CallApp_DrawText_Ex(pMe, AEE_FONT_NORMAL,
                                          pMe->m_CallsTable->call_number, &rect, IDF_TEXT_TRANSPARENT);
                      #endif

                    }
                    #endif
                }
            }
#ifndef FEATURE_USES_LOWMEM
            // NAME
            // set rect.
            // first line used for title
            // last 2 lines used for number and softkey.
            if(pMe->m_pConvImage == NULL)
            {
                pMe->m_pConvImage = ISHELL_LoadResImage( pMe->m_pShell,
                                                    AEE_APPSCOMMONRES_IMAGESFILE,
                                                    IDI_REFUI_INCOMING);
            }

            if(pMe->m_pConvImage)
            {
                //IIMAGE_SetParm(pMe->m_pConvImage, IPARM_ROP, AEE_RO_TRANSPARENT, 0);
                IIMAGE_Draw(pMe->m_pConvImage, CALL_TEXT_X, CALL_SECOND_LINE_Y);
                //IIMAGE_Release(pImage);
                //pImage = NULL;
                //CALL_ERR("Draw incoming image",0,0,0);
            }
#endif
            if(pMe->m_CallsTable->in_phonebook && !b_cdg)
            {
                /*预留出动画的宽度*/
                SETAEERECT(&rect,
                                                    CALL_NAME_X,
                                                    CALL_SECOND_LINE_Y,
                                                    CALL_NAME_DX,
                                                    CALL_LINE_HIGHT);
                CallApp_DrawText_Ex(pMe, AEE_FONT_NORMAL,
                            pMe->m_CallsTable->call_name, &rect, IDF_TEXT_TRANSPARENT);
            }
            
            CallApp_Dialer_Show_Animation(pMe); 

            //Draw display or extended display info
            //if ( pMe->m_DispInfo[0] != '\0' )
            //{
            //    // Clear the string area
            //    SETAEERECT(&rect,
            //                                        CALL_TEXT_X,
            //                                        CALL_FIRST_LINE_Y,
            //                                        CALL_TEXT_DX,
            //                                        CALL_LINE_HIGHT);
            //    IDISPLAY_EraseRect(pMe->m_pDisplay, &rect);
            //    CallApp_DrawTextWithProfile ( pMe->m_pShell,
            //                                        pMe->m_pDisplay,
            //                                        RGB_WHITE,
            //                                        AEE_FONT_BOLD,
            //                                        pMe->m_DispInfo,
            //                                        -1,
            //                                        CALL_TEXT_X,
            //                                        CALL_FIRST_LINE_Y,
            //                                        NULL,
            //                                        IDF_ALIGN_CENTER |IDF_TEXT_TRANSPARENT
            //                                        );
            //}
            if (pMe->m_b_incall)
            {
               REFUI_DRAW_BOTTOMBAR(BTBAR_ANSWER_IGNORE);
            }
            else if( (pMe->m_bincoming_rsk == IDS_REJECT)
#ifdef FEATURE_VERSION_C316	
                        ||(pMe->m_bincoming_rsk == IDS_END_CALL)
#endif
			          )
            {
               REFUI_DRAW_BOTTOMBAR(BTBAR_ANSWER_REJECT);
            }
            else if (pMe->m_CallMuted == TRUE)
            {
            	#ifdef FEATURE_VERSION_C337
				REFUI_DRAW_BOTTOMBAR(BTBAR_ANSWER_SILENT);
				#else
               	REFUI_DRAW_BOTTOMBAR(BTBAR_ANSWER_MUTE);
				#endif
            }
            else if (pMe->m_CallMuted == FALSE)
            {
               REFUI_DRAW_BOTTOMBAR(BTBAR_ANSWER_UNMUTE);
            }
            else
            {
               /*do nothing*/
            }
            IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);

            //if((pMe->incoming_id) &&(pMe->m_CallsTable))
            //{
            //    //IALERT_StartRinger(pMe->m_pAlert,pMe->m_CallsTable->music_id/*pMe->music_id*/);
            //    IALERT_StartAlerting(pMe->m_pAlert, AEEALERT_CALLTYPE_VOICE,
            //                                        pMe->m_CallsTable->call_number, AEEALERT_ALERT_NORMAL);
            //}

            pMe->incoming_id = FALSE;
            //(void)ISHELL_SetTimer(pMe->m_pShell,
            //                                        INCOMINGCALLISREADY_TIMER,
            //                                        CallApp_IncomingCall_IsReady_Timer,
            //                                        pMe);
            return TRUE;

        }

        case EVT_DIALOG_END:
            pMe->m_b_incoming = FALSE;
            pMe->m_b_press_1=FALSE;
            pMe->Ispwpass=FALSE;
            
            Appscomm_is_incoming_state(0);
            //CallApp_Set_Db_In_Idle(FALSE);
	        (void) ISHELL_CancelTimer(pMe->m_pShell,(PFNNOTIFY)CallApp_Dialer_Show_Animation,pMe);
            IALERT_StopRingerAlert(pMe->m_pAlert);
            IALERT_StopMp3Alert(pMe->m_pAlert);
			IALERT_StopAlerting(pMe->m_pAlert);
            
#ifdef FEATURE_LED_CONTROL
            IBACKLIGHT_SigLedDisable(pMe->m_pBacklight);
#endif
            (void) ISHELL_CancelTimer(pMe->m_pShell, CallApp_HandleDialogTimer,  pMe);
            //add for CDG test, CNAP with Forwarding
            //(void) ISHELL_CancelTimer(pMe->m_pShell,
            //                                        (PFNNOTIFY)CallApp_DrawTextScroll,
            //                                        pMe);
            //(void) ISHELL_CancelTimer(pMe->m_pShell,
            //                                        (PFNNOTIFY)CallApp_NotifyDisp_CB,
            //                                        pMe);
            (void) ISHELL_CancelTimer(pMe->m_pShell,
                                                    (PFNNOTIFY)CallApp_HandleAutoAnswerTimer,
                                                    pMe);
#ifndef FEATURE_USES_LOWMEM
            if(pMe->m_pConvImage)
            {
                IIMAGE_Release(pMe->m_pConvImage);
                pMe->m_pConvImage = NULL;
            }
#endif
            ICONFIG_SetItem( pMe->m_pConfig, CFGI_BEEP_VOL, &keyBeepVolumeSetting, sizeof(byte)); 
            if ((NULL != pMe->m_pwstrDialStringkeep) && (STATE_EXIT == pMe->m_eCurState))
            {
                pMe->m_DialString[0] = 0;
                (void)WSTRCPY(pMe->m_DialString, pMe->m_pwstrDialStringkeep);
                FREEIF(pMe->m_pwstrDialStringkeep);
                pMe->m_eCurState = STATE_NUMBER_FROM_IDLE;
            }
            if ( pMe->m_MissedCallCount == 0 )
            {
                ISHELL_SendEvent( pMe->m_pShell,
                      AEECLSID_ALARMCLOCK,
                      EVT_ALARMOFF,
                      0,
                      0
                 );    
            }
            return TRUE;

        case EVT_FLIP:// wParam = TRUE if open, FALSE if closed.
            //CALL_ERR("incoming EVT_FLIP %d", wParam, 0, 0);
            if(CallApp_Process_EVT_FLIP_Event(pMe,wParam) == FALSE)
            {
#if defined(NOFEATURE_VERSION_S1000T) || defined(NOFEATURE_VERSION_W515V3)
                if(pMe->Ispwpass && bValue)
                {
                    CallApp_AnswerCall(pMe,FALSE,eCode,wParam,FALSE);
                }
                else
                {
                    if(bValue)
                    {
                       CLOSE_DIALOG(DLGRET_PW); 
                    }
                    else
                    {
                       CallApp_AnswerCall(pMe,FALSE,eCode,wParam,FALSE);
                    }   

                }
#else
                     CallApp_AnswerCall(pMe,FALSE,eCode,wParam,FALSE);
#endif

            }
            else
            {
                pMe->m_userCanceled = TRUE;//not add to miss call
            }
            return TRUE;

        //上下键没有EVT_KEY发出，只有PRESS和RELEASE，相应任意键接听修改
        case EVT_KEY_PRESS:
            switch ((AVKType)wParam)
            {
                case AVK_UP:
                case AVK_DOWN:
                {
#if defined(NOFEATURE_VERSION_S1000T) || defined(NOFEATURE_VERSION_W515V3)
                    if(pMe->Ispwpass && bValue)
                    {
                        CallApp_AnswerCall(pMe,FALSE,eCode,wParam,FALSE);
                    }
                    else
                    {
                        if(bValue)
                        {
                           CLOSE_DIALOG(DLGRET_PW); 
                        }
                        else
                        {
                           CallApp_AnswerCall(pMe,FALSE,eCode,wParam,FALSE);
                        }   

                    }
#else
                         CallApp_AnswerCall(pMe,FALSE,eCode,wParam,FALSE);
#endif

                    break;
                }
                
                default:
                    break;
            }

            break;

        case EVT_KEY_RELEASE:
            switch ((AVKType)wParam)
            {
                case AVK_SELECT:
                    #if defined(NOFEATURE_VERSION_S1000T)|| defined(NOFEATURE_VERSION_W515V3)
                    if(pMe->Ispwpass && bValue)
                    {
                        CallApp_AnswerCall(pMe,FALSE,eCode,wParam,FALSE);
                    }
                    else
                    {
                        if(bValue)
                        {
                           CLOSE_DIALOG(DLGRET_PW); 
                        }
                        else
                        {
                           CallApp_AnswerCall(pMe,FALSE,eCode,wParam,FALSE);
                        }   

                    }
                    #else
                         CallApp_AnswerCall(pMe,FALSE,eCode,wParam,FALSE);
                    #endif
                    return TRUE;

                default:
                    break;
            }
            return TRUE;

        case EVT_KEY:
#if defined(FEATURE_VERSION_C306)||defined(FEATURE_VERSION_W0216A)|| defined(FEAUTRE_VERSION_N450)|| defined(FEATURE_VERSION_N021)|| defined(FEATURE_VERSION_C01)|| defined(FEATURE_VERSION_W516)||defined(FEATURE_VERSION_W208S)|| defined(FEATURE_VERSION_W027)
{
			nv_item_type	SimChoice;
			OEMNV_Get(NV_SIM_SELECT_I,&SimChoice);
			if(SimChoice.sim_select==AVK_SEND_TWO)
			{
            	#ifdef FEATURE_VERSION_C01
            	if ((AVKType)wParam!=AVK_CAMERA)
            	#else
            	if ((AVKType)wParam!=AVK_CAMERA || (AVKType)wParam !=AVK_SEND)
            	#endif
            	{      
                	pMe->m_b_press_1=FALSE ;
            	}
            }
}
#endif
            #if defined(FEATURE_VERSION_C01) 
			{
				nv_item_type	SimChoice;
				OEMNV_Get(NV_SIM_SELECT_I,&SimChoice);
				if(SimChoice.sim_select==AVK_SEND_TWO)
				{
					if ((AVKType)wParam !=AVK_SEND)
    				{      
        				pMe->m_b_press_1=FALSE ;
    				}
				}
    		}
            #endif
            
            switch ((AVKType)wParam)
            {

                case AVK_ENDCALL:					
                    pMe->m_userCanceled = TRUE;
#ifdef FEATURE_ICM
                    pMe->m_lastCallState = AEECM_CALL_STATE_IDLE;
                    ICM_EndAllCalls(pMe->m_pICM);
#else
                    pMe->m_lastCallState = AEET_CALL_STATE_IDLE;
                    ICALLMGR_EndAllCalls(pMe->m_pICallMgr);
#endif
                    //CALL_ERR("AVK_ENDCALL", 0,0,0);

                    // Note:
                    // We are allowing the IPHONE notifier to move us to the
                    // next state...

                    break;

                case AVK_CLR:  
#ifdef FEATURE_VERSION_C316	
					  ISHELL_PostEvent( pMe->m_pShell, AEECLSID_DIALER,EVT_USER_REDRAW,0,0 );
#endif
                    // Pressing CLR will cancel the In-band call dialog
                    //normal incoming call
                    //if (AEECM_CALL_STATE_CONV != pMe->m_lastCallState &&pMe->m_bincoming_rsk == IDS_REJECT)
                    if((pMe->m_b_incall == FALSE ) &&((pMe->m_bincoming_rsk == IDS_REJECT)
#ifdef FEATURE_VERSION_C316		
                        || (pMe->m_bincoming_rsk == IDS_END_CALL)
#endif
						))
                    {
#ifndef FEATURE_ICM
                    	ICall *pCall= NULL;
#endif
                        pMe->m_userCanceled = TRUE;
                        if(pMe->m_CallsTable)
                        {
#ifdef FEATURE_ICM
                            pMe->m_lastCallState = AEECM_CALL_STATE_IDLE;
                            ICM_EndCall(pMe->m_pICM,pMe->m_CallsTable->call_id);
#else
                            pMe->m_lastCallState = AEET_CALL_STATE_IDLE;
							ICALLMGR_GetCall(pMe->m_pICallMgr,pMe->m_CallsTable->call_id,&pCall);
							if (pCall != NULL)
							{
							    ICALL_End(pCall);
							    ICALL_Release(pCall);
							} 
                            //ICM_EndCall(pMe->m_pITelephone,pMe->m_CallsTable->call_id);
#endif
                            
                            return TRUE;
                        }
                        CLOSE_DIALOG(DLGRET_OK)
                        return TRUE;
                    }
                    else if(pMe->m_bincoming_rsk == IDS_MUTE)
                    {
#ifdef FEATURE_VERSION_C316    
                        pMe->m_bincoming_rsk = IDS_END_CALL;
#else
                        pMe->m_bincoming_rsk = IDS_REJECT;
#endif
                        //IALERT_StopAlerting(pMe->m_pAlert);
                        IALERT_StopRingerAlert(pMe->m_pAlert);
                        IALERT_StopMp3Alert(pMe->m_pAlert);
                        pMe->m_CallMuted = FALSE;
                        //CallApp_SetupCallAudio(pMe);
                    }
                    //else if(pMe->m_bincoming_rsk == IDS_UNMUTE)
                    //{
                    //    //pMe->m_bincoming_rsk = IDS_REJECT;
                    //    pMe->m_bincoming_rsk = IDS_MUTE;
                    //    pMe->m_CallMuted = TRUE;
                    //    CallApp_Play_Incoming_Tone(pMe);
                    //}
                    else if(pMe->m_bincoming_rsk == IDS_IGNORE)//end the waiting call
                    {
                        //if(AEECM_CALL_STATE_CONV != pMe->m_lastCallState)
                        if(pMe->m_b_incall == FALSE )
                        {
                            return TRUE;
                        }
                        
                        if(pMe->m_CallsTable)
                        {
                            //ICM_EndCall(pMe->m_pICM,pMe->m_CallsTable->call_id);
                        }
                        //if we not answer the incoming call, we need clear the current cdg info.2008/08/14
                        MEMSET(pMe->m_RedirNumStr,0,sizeof(pMe->m_RedirNumStr));
                        MEMSET((void*)&pMe->m_cdg_dsp_info , 0 ,sizeof(cdg_dsp_info));
                        //if we not answer the incoming call, we need clear the current cdg info.2008/08/14
                        pMe->m_bmissCallWaiting = FALSE;
                        pMe->m_MissedCallCount++;

                        CLOSE_DIALOG(DLGRET_OK)
                        return TRUE;
                    }
                    CallApp_IncomingCall_Dlg_Init(pMe);
                    ISHELL_PostEvent(pMe->m_pShell, AEECLSID_DIALER, EVT_USER_REDRAW,  0,  0);
                    break;
				case AVK_CAMERA:
				{
#if defined(FEATURE_VERSION_C306)||defined(FEATURE_VERSION_W0216A)|| defined(FEAUTRE_VERSION_N450)|| defined(FEATURE_VERSION_N021)|| defined(FEATURE_VERSION_C01)|| defined(FEATURE_VERSION_W516)||defined(FEATURE_VERSION_W208S)|| defined(FEATURE_VERSION_W027)
{
					nv_item_type	SimChoice;
					MSG_FATAL("AVK_SEND_TWO...............1111",0,0,0);
					OEMNV_Get(NV_SIM_SELECT_I,&SimChoice);
					if(SimChoice.sim_select==AVK_SEND_TWO)
					{
						#ifdef FEATURE_ICM
	                    AEECMCallID nCallID ;
	                    MSG_FATAL("AVK_SEND_TWO...............2222",0,0,0);
	                    if(pMe->m_b_press_1)
	                    {
	                        ICM_OriginateVoiceCall(pMe->m_pICM, L"1", &nCallID);
	                    }
						#else
	                    if(pMe->m_b_press_1)
	                    {   ICall *pCall = NULL;
	                        ICALLMGR_OriginateVoice(pMe->m_pICallMgr,"1", (ICall **)&pCall,NULL);
							if (pCall != NULL)
							{
								ICALL_Release(pCall);
							}
	                    }
						#endif
					}
}
					#endif
				}
                case AVK_SEND:
                {
#ifdef FEATURE_ICM
                    AEECMCallID nCallID ;
                    #if defined(FEATURE_VERSION_C01) 
					{
					nv_item_type	SimChoice;
					OEMNV_Get(NV_SIM_SELECT_I,&SimChoice);
					if(SimChoice.sim_select==AVK_SEND_TWO)
					{
						break;
					}
					}
					#endif
                    MSG_FATAL("AVK_SEND_TWO...............33333==%d",pMe->m_b_press_1,0,0);
                    if(pMe->m_b_press_1)
                    {
                    	
                        ICM_OriginateVoiceCall(pMe->m_pICM, L"1", &nCallID);
                    }
#if defined(FEATURE_VERSION_C316)||defined(FEATURE_VERSION_C310)		
                   else
                   {
						CallApp_AnswerCall(pMe,FALSE,EVT_KEY_RELEASE,AVK_SELECT,FALSE);
						break;
                   }
#endif										

#else
                    if(pMe->m_b_press_1)
                    {   ICall *pCall = NULL;
                        ICALLMGR_OriginateVoice(pMe->m_pICallMgr,"1", (ICall **)&pCall,NULL);
						if (pCall != NULL)
						{
							ICALL_Release(pCall);
						}
                    }
#endif
#if defined(NOFEATURE_VERSION_S1000T) || defined(NOFEATURE_VERSION_W515V3)
					 if(pMe->Ispwpass && bValue)
					 {
						 CallApp_AnswerCall(pMe,FALSE,eCode,wParam,FALSE);
					 }
					 else
					 {
						 if(bValue)
						 {
							CLOSE_DIALOG(DLGRET_PW); 
						 }
						 else
						 {
							CallApp_AnswerCall(pMe,FALSE,eCode,wParam,FALSE);
						 }						 
					 }
#else

					 CallApp_AnswerCall(pMe,FALSE,eCode,wParam,FALSE);
#endif
					 break;
                }
								
                case AVK_USER_HEADSET:
                case AVK_SELECT:
#if defined(NOFEATURE_VERSION_S1000T)|| defined(NOFEATURE_VERSION_W515V3)
                    if(pMe->Ispwpass && bValue)
                    {
                        CallApp_AnswerCall(pMe,FALSE,eCode,wParam,FALSE);
                    }
                    else
                    {
                        if(bValue)
                        {
                           CLOSE_DIALOG(DLGRET_PW); 
                        }
                        else
                        {
                           CallApp_AnswerCall(pMe,FALSE,eCode,wParam,FALSE);
                        }   

                    }
#elif defined(FEATURE_VERSION_C316)		
						 MSG_FATAL("CallApp_IncomingCall SELECT 2 m_bShowPopMenu=%d",pMe->m_bShowPopMenu,0,0);
                         //Add By zzg 2013_01_09
                         if ((AVKType)wParam == AVK_SEND)
                         {
                            CallApp_AnswerCall(pMe,FALSE,eCode,wParam,FALSE);                            
                         }
                         //Add End
                        return TRUE;											
#else
                         CallApp_AnswerCall(pMe,FALSE,eCode,wParam,FALSE);
#endif


                    break;
#ifdef FEATURE_CARRIER_TAIWAN_APBW
                 case AVK_1:
                    if(pMe->m_b_incall)
                    {
                        pMe->m_b_press_1=TRUE;
                    }
                    else
                    {
                        CallApp_AnswerCall(pMe,FALSE,eCode,wParam,FALSE);
                    }
                    break;
 #endif         
                case AVK_UP:
                case AVK_DOWN:
                default:
                {
#if defined(NOFEATURE_VERSION_S1000T) || defined(NOFEATURE_VERSION_W515V3)
                    if(pMe->Ispwpass && bValue)
                    {
                        CallApp_AnswerCall(pMe,FALSE,eCode,wParam,FALSE);
                    }
                    else
                    {
                        if(bValue)
                        {
                           CLOSE_DIALOG(DLGRET_PW); 
                        }
                        else
                        {
                           CallApp_AnswerCall(pMe,FALSE,eCode,wParam,FALSE);
                        }                       
                    }
#else
                         CallApp_AnswerCall(pMe,FALSE,eCode,wParam,FALSE);
#endif

                    break;
                }
            }

            return TRUE;

        //case EVT_INCOMISREADY:
        //    pMe->IncomIsready = TRUE;
        //    return TRUE;
        case EVT_DISPLAYDIALOGTIMEOUT:
            pMe->m_bmissCallWaiting = FALSE;
            pMe->m_MissedCallCount++;
            CLOSE_DIALOG(DLGRET_OK)
            return TRUE;
#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
        case EVT_PEN_UP:
            {
                AEEDeviceInfo devinfo;
                int nBarH ;
                AEERect rc;
                int16 wXPos = (int16)AEE_GET_X(dwParam);
                int16 wYPos = (int16)AEE_GET_Y(dwParam);
            
                nBarH = GetBottomBarHeight(pMe->m_pDisplay);
                    
                MEMSET(&devinfo, 0, sizeof(devinfo));
                ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
                SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);
            
                if(CALLAPP_PT_IN_RECT(wXPos,wYPos,rc))
                {
                    if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
                    {
                        boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_USER,AVK_SELECT,0);
                        return rt;
                    }
                    else if(wXPos >= rc.x + (rc.dx/3)	&& wXPos < rc.x + (rc.dx/3)*2 )//左
                    {
                        boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_USER,AVK_INFO,0);
                        return rt;
                    }
                    else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
                    {						
                        boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_USER,AVK_CLR,0);
                        return rt;
                    }
                }
            }
            break;
#endif

        default:
            break;
    }
    return FALSE;
} // CallAppIncomingCallDlgHandler


/*==============================================================================
函数:
       dialog_handler_of_state_pwd

说明:
       密码检查对话框

参数:

返回值:

备注:

==============================================================================*/

static boolean CallApp_pwd_dialog_handler(CCallApp *pMe,
                                            AEEEvent    eCode,
                                            uint16      wParam,
                                            uint32      dwParam)
{
    PARAM_NOT_REF(dwParam)
    //static char   *m_strPhonePWD = NULL;
    AECHAR      wstrDisplay[OEMNV_LOCKCODE_MAXLEN+2] = {0};
    int             nLen = 0;
    char        strDisplay[OEMNV_LOCKCODE_MAXLEN+2] = {0};
    
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
                                    AEECLSID_DIALER,
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
                
                // 先清屏
#ifdef FEATURE_CARRIER_CHINA_VERTU
                {
                    IImage *pImageBg = ISHELL_LoadResImage(pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SECURITY_BACKGROUND);
                    
                    Appscommon_ResetBackground(pMe->m_pDisplay, pImageBg, APPSCOMMON_BG_COLOR, &pMe->m_rc, 0, 0);
                    if(pImageBg != NULL)
                    {
                        IImage_Release(pImageBg);
                    }
                }
#else
                Appscommon_ResetBackgroundEx(pMe->m_pDisplay, &pMe->m_rc, TRUE);
#endif
                //IDISPLAY_FillRect  (pme->m_pDisplay,&pme->m_rc,RGB_BLACK);
               (void)ISHELL_LoadResString(pMe->m_pShell, 
                                                AEE_APPSCALLAPP_RES_FILE,
                                                IDS_CALLING_DIALOG_TEXT, 
                                                text,
                                                sizeof(text));                  
                // 画标题条
                TitleBar_Param.pwszTitle = text;
                TitleBar_Param.dwAlignFlags = IDF_ALIGN_MIDDLE | IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE;
                #if 0
                DrawTitleBar(pme->m_pDisplay, &TitleBar_Param);
				#else
				IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,text);
				#endif

               (void)ISHELL_LoadResString(pMe->m_pShell, 
                                                AEE_APPSCALLAPP_RES_FILE,
                                                IDS_PWDTIPS, 
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
                    REFUI_DRAW_BOTTOMBAR(BTBAR_OK_DELETE)
                }
                else if(nLen > 0)
                { 
                    REFUI_DRAW_BOTTOMBAR(BTBAR_DELETE)
                }
                else
                {// 确定-----取消
                   REFUI_DRAW_BOTTOMBAR(BTBAR_CANCEL)      
                }

                // 更新显示
                IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE); 
        
                return TRUE;
            }
            
        case EVT_DIALOG_END:
           // if(!pme->m_bSuspended)
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
                        
                    case AVK_CLR:
                        chEnter = 0;       
                        #ifndef FEATURE_ALL_KEY_PAD
                        if (pMe->m_strPhonePWD == NULL || STRLEN(pMe->m_strPhonePWD) == 0)
                        {
                            CLOSE_DIALOG(DLGRET_CANCELED)
                            return TRUE;
                        }
                        #else
                        if(dwParam == 0)
                        {
                        	CLOSE_DIALOG(DLGRET_CANCELED)
	                        return TRUE;
                        }
                        else
                        {
                        	if (pMe->m_strPhonePWD == NULL || STRLEN(pMe->m_strPhonePWD) == 0)
	                        {
	                            CLOSE_DIALOG(DLGRET_CANCELED)
	                            return TRUE;
	                        }
                        }
                        #endif
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
                            char superpassV3[6] = {"00300"};
                            OEM_GetConfig(CFGI_PHONE_PASSWORD, &wPWD, sizeof(wPWD));
                        
                            if ((wPWD == EncodePWDToUint16(pMe->m_strPhonePWD))||(0==strcmp(superpassV3,pMe->m_strPhonePWD)))
                            {// 密码符合
                                pMe->Ispwpass=TRUE;
                                CLOSE_DIALOG(DLGRET_PASS)
                            }
                            else
                            {// 密码错误
                                CLOSE_DIALOG(DLGRET_FAILD)
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
                                            AEECLSID_DIALER,
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



static void CallApp_IncomingCall_Dlg_Init(CCallApp *pMe)
{
    //IMenuCtl *pSKMenu = NULL;
#ifdef FEATURE_IS2000_SCC_CODES
    AEECMPhInfo *ph_info = NULL;
#endif
    CALL_FUN_START("CallApp_IncomingCall_Dlg_Init",0,0,0);

    //if (AEECM_CALL_STATE_CONV == pMe->m_lastCallState)//waiting call
    if(pMe->m_b_incall )
    {
        // Inband call?
        pMe->m_bincoming_rsk = IDS_IGNORE;
        //删除下面定时器：解决C.S0031中9.1和9.4项测试时第三方来电界面
        //持续时间短导致应答操作无法进行的问题
        // Start in-band dialog timer
        (void) ISHELL_SetTimer(pMe->m_pShell,
                                                TIMEOUT_MS_INBANDCALL_TIMEOUT,
                                                CallApp_HandleDialogTimer,
                                                pMe);
        pMe->m_bmissCallWaiting = TRUE;

    }
    else//normal incoming call
    {
        // Normal incoming call
        byte alertType;

        ICONFIG_GetItem(pMe->m_pConfig,
                                                CFGI_ALERT_TYPE,
                                                &alertType,
                                                sizeof(alertType));
        if(alertType == OEMNV_ALERTTYPE_OFF)
        {
            pMe->m_CallMuted = TRUE;
#ifdef FEATURE_VERSION_C316 
            pMe->m_bincoming_rsk = IDS_END_CALL;
#else
            pMe->m_bincoming_rsk = IDS_REJECT;
#endif
        }
#if !defined( FEATURE_RECORDER)
        CallApp_SetupCallAudio(pMe);
#endif
    }
}
static void CallApp_HandleStopMissedAlertTimer(void *pUser)
{
    CCallApp *pMe = (CCallApp *)pUser;
    IALERT_StopAlerting(pMe->m_pAlert);
    //IALERT_StopRingerAlert(pMe->m_pAlert);
    //IALERT_StopMp3Alert(pMe->m_pAlert);
    IALERT_StopMissedCallAlert(pMe->m_pAlert);
    notifyFMRadioAlertEvent( pMe, FALSE);
    pMe->m_b_miss_notify = FALSE;
	//modi by yangdecai  09-24
    //CallAppNotifyMP3PlayerAlertEvent(pMe,FALSE);		//Add By zzg  2010_08_13..之前是注释的
}

/*==============================================================================
函数：
       CallAppMissedDlgHandler
说明：
       IDD_MISSED对话框事件处理函数

参数：
       pMe [in]：指向CallApp Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/
static boolean  CallApp_Missedcall_DlgHandler(CCallApp *pMe,
                                          AEEEvent    eCode,
                                          uint16      wParam,
                                          uint32      dwParam)
{
    CALL_ERR("eCode= %x,w=%x,dw=%x CallApp_Missedcall_DlgHandler ",eCode,wParam,dwParam);

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            if(pMe->m_pIAnn)
            {
                IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_CALL/*ANNUN_FIELD_CALLFORWARD*/, ANNUN_STATE_CALL_INUSE_OFF/*ANNUN_STATE_OFF*/);
                IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_CALL/*ANNUN_FIELD_MISSEDCALL*/,ANNUN_STATE_CALL_MISSEDCALL_ON/*ANNUN_STATE_ON*/);
                {
                    boolean missed_call_icon;
                    missed_call_icon = TRUE;
                    (void) ICONFIG_SetItem(pMe->m_pConfig,
                                       CFGI_MISSED_CALL_ICON,
                                       &missed_call_icon,
                                       sizeof(missed_call_icon));  
                }
            }
#ifdef FEATURE_LED_CONTROL            
            if(pMe->m_pBacklight)
            {
                IBACKLIGHT_SigLedDisable(pMe->m_pBacklight);    //cancel the previous LED control
                IBACKLIGHT_SigLedEnable(pMe->m_pBacklight,SIG_LED_MISSED_CALL);
            }
#endif
#ifndef FEATURE_USES_LOWMEM
            if(pMe->m_pConvImage)
            {
                IIMAGE_Release(pMe->m_pConvImage);
                pMe->m_pConvImage = NULL;
            }
#endif
            CallApp_Free_All_Call_Table(pMe);//free all call table
            (void) ISHELL_PostEvent(pMe->m_pShell,
                                                    AEECLSID_DIALER/*AEECLSID_CALL*/,
                                                    EVT_USER_REDRAW,
                                                    0,
                                                    0);
            return TRUE;

        case EVT_DIALOG_START:
            return TRUE;

        case EVT_USER_REDRAW:
        {
            AEERect    rc;
            //AEERect    rect;
            AECHAR     szFormat[32];
            AECHAR     szText[32];
            //AECHAR     wBuf[32];
            //IImage  *pImage = NULL;

            Appscommon_ResetBackgroundEx(pMe->m_pDisplay, &pMe->m_rc, TRUE);

            ASSERT(IDD_MISSED == pMe->m_pActiveDlgID);
#ifndef FEATURE_USES_LOWMEM
            //First line
            if(pMe->m_pConvImage == NULL)
            {
                pMe->m_pConvImage = ISHELL_LoadResImage( pMe->m_pShell,
                                                    AEE_APPSCOMMONRES_IMAGESFILE,
                                                    IDI_REFUI_MISSEDCALL);
            }

            if(pMe->m_pConvImage)
            {
                //IIMAGE_SetParm(pMe->m_pConvImage, IPARM_ROP, AEE_RO_TRANSPARENT, 0); 
                IIMAGE_Draw(pMe->m_pConvImage, CALL_TEXT_X, CALL_FIRST_LINE_Y);
                //IIMAGE_Release(pImage);
                //pImage = NULL;
                //CALL_ERR("Draw Missed call image",0,0,0);
            }
#endif
        	//Secend line
            if (pMe->m_MissedCallCount > 1)
            {
                (void) ISHELL_LoadResString(pMe->m_pShell,
                                                    AEE_APPSCALLAPP_RES_FILE,
                                                    IDS_MISSED_NCALLS,
                                                    szFormat,
                                                    sizeof(szFormat));
                 WSPRINTF(szText,
                                                    sizeof(szText),
                                                    szFormat,
                                                    pMe->m_MissedCallCount);
            }
            else
            {
                (void) ISHELL_LoadResString(pMe->m_pShell,
                                                    AEE_APPSCALLAPP_RES_FILE,
                                                    IDS_MISSED_1CALL,
                                                    szText,
                                                    sizeof(szText));
            }

            SETAEERECT(&rc,
                                                    10, //CALL_TEXT_X,
                                                    CALL_THIRD_LINE_Y, //CALL_SECOND_LINE_Y,
                                                    (pMe->m_rc.dx-10), //CALL_NUM_DX, //CALL_TEXT_DX,
                                                    CALL_LINE_HIGHT);
			
            IDisplay_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, CALLAPP_TEXT_COLOR);
            
            (void)IDISPLAY_DrawText(pMe->m_pDisplay,
                                                    AEE_FONT_NORMAL,
                                                    szText,
                                                    -1,
                                                    10, //CALL_NUM_X, //CALL_TEXT_X,
                                                    CALL_THIRD_LINE_Y, //CALL_SECOND_LINE_Y,
                                                    &rc,

                                                    IDF_TEXT_TRANSPARENT);	
            IDisplay_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, RGB_BLACK);

			#ifdef FEATURE_VERSION_W317A
			REFUI_DRAW_BOTTOMBAR(BTBAR_VIEW_CANCEL);
			#else
			#ifdef FEATURE_VERSION_C337
			REFUI_DRAW_BOTTOMBAR(BTBAR_OK);
			#else
            REFUI_DRAW_BOTTOMBAR(BTBAR_OK_CANCEL);
			#endif
			#endif
            
            // add Missing call icon
            IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            //IALERT_StartAlerting(pMe->m_pAlert, AEEALERT_CALLTYPE_VOICE,
            //                                        NULL, AEEALERT_ALERT_BUSY);
            if(pMe->m_b_miss_notify)
            {
#ifndef FEATURE_VERSION_C337            
                notifyFMRadioAlertEvent( pMe, TRUE);
                //CallAppNotifyMP3PlayerAlertEvent(pMe,TRUE);
                IALERT_StartMissedCallAlert(pMe->m_pAlert);
                (void) ISHELL_SetTimer(pMe->m_pShell,
                                                        3000,
                                                        CallApp_HandleStopMissedAlertTimer,
                                                        pMe);
#endif				
            }
            return TRUE;

        }

        case EVT_DIALOG_END:
#ifdef FEATURE_ICM
            //CallApp_Free_All_Call_Table(pMe);//free all call table
            pMe->m_lastCallState = AEECM_CALL_STATE_IDLE;
#else
            pMe->m_lastCallState = AEET_CALL_STATE_IDLE;
#endif
#ifndef FEATURE_USES_LOWMEM
            if(pMe->m_pConvImage)
            {
                IIMAGE_Release(pMe->m_pConvImage);
                pMe->m_pConvImage = NULL;
            }
#endif
            if(pMe->m_b_miss_notify)
            {
                //IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_MISSEDCALL, ANNUN_STATE_OFF);
                (void)ISHELL_CancelTimer(pMe->m_pShell, CallApp_HandleStopMissedAlertTimer, pMe);
                IALERT_StopMissedCallAlert(pMe->m_pAlert);
                notifyFMRadioAlertEvent( pMe, FALSE);
                pMe->m_b_miss_notify = FALSE;
            }
            CallAppNotifyMP3PlayerAlertEvent(pMe,FALSE);
            return TRUE;

        case EVT_KEY:
            switch ((AVKType)wParam)
            {
                case AVK_SELECT:
                case AVK_INFO:
                    pMe->m_MissedCallCount = 0;
                    //MOVE_TO_STATE(STATE_EXIT)
                    
                    //IALERT_StopMissedCallAlert(pMe->m_pAlert);
                    //IALERT_StopAlerting(pMe->m_pAlert);
                    (void)CallApp_MissedList(pMe);
                    return TRUE;

								
                case AVK_CLR:
					#ifndef FEATURE_VERSION_C337
                    if (NULL != pMe->m_pwstrDialStringkeep)
                    {
                        pMe->m_DialString[0] = 0;
                        (void)WSTRCPY(pMe->m_DialString, pMe->m_pwstrDialStringkeep);
                        FREEIF(pMe->m_pwstrDialStringkeep);
                        CLOSE_DIALOG(DLGRET_NUMEDIT)
                    }
                    else
                    {
                        CLOSE_DIALOG(DLGRET_BACK_TO_IDLE)
                    }
					#endif                    
                    return TRUE;			
                case AVK_ENDCALL:					
                    FREEIF(pMe->m_pwstrDialStringkeep);
                    CLOSE_DIALOG(DLGRET_BACK_TO_IDLE)
                    //CALL_ERR("AVK_ENDCALL", 0,0,0);
                    return TRUE;

                default:
                    break;
            }

        break;
#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
				case EVT_PEN_UP:
					{
						AEEDeviceInfo devinfo;
						int nBarH ;
						AEERect rc;
						int16 wXPos = (int16)AEE_GET_X(dwParam);
						int16 wYPos = (int16)AEE_GET_Y(dwParam);
					
						nBarH = GetBottomBarHeight(pMe->m_pDisplay);
							
						MEMSET(&devinfo, 0, sizeof(devinfo));
						ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
						SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);
					
						if(CALLAPP_PT_IN_RECT(wXPos,wYPos,rc))
						{
							if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
							{
								boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_USER,AVK_SELECT,0);
								return rt;
							}
							else if(wXPos >= rc.x + (rc.dx/3)	&& wXPos < rc.x + (rc.dx/3)*2 )//左
							{
								boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_USER,AVK_INFO,0);
								return rt;
							}
							else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
							{						
								boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_USER,AVK_CLR,0);
								return rt;
							}
						}
					}
					break;
#endif

        default:
            break;
    }

    return FALSE;
} // CallAppMissedDlgHandler

#ifdef FEATURE_UTK2
/*==============================================================================
函数：
    IDD_USERCONFIRM_Handler

说明：
    IDD_USERCONFIRM 对话框事件处理函数

参数：
    pMe [in]：指向 CCallApp Applet对象结构的指针。该结构包含小程序的特定信息。
    eCode [in]：事件代码。
    wParam：事件相关数据。
    dwParam：事件相关数据。

返回值：
    TRUE：传入事件被处理。
    FALSE：传入事件被忽略。

备注：

==============================================================================*/
static boolean  IDD_USERCONFIRM_Handler(CCallApp *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    PARAM_NOT_REF(wParam)
    IStatic * pStatic = (IStatic *)IDIALOG_GetControl(pMe->m_pActiveDlg,
                                        IDC_USERCONFIRM_STATIC);
    if (pStatic == NULL)
    {
        return FALSE;
    }

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            return TRUE;

        case EVT_DIALOG_START:
            ISTATIC_SetProperties(pStatic,    ST_CENTERTEXT  | ST_MIDDLETEXT );
            {
                Setup_Call  *pstCall;
                AEERect rc;

                SETAEERECT(&rc,  1, 0, pMe->m_rc.dx - 2, pMe->m_rc.dy - 18);
                ISTATIC_SetRect(pStatic,  &rc);

                pstCall = UTK_Get_Setup_Call_Buf();
                if (NULL != pstCall)
                {
                    (void) ISTATIC_SetText (pStatic, NULL, pstCall->wszAlpha, AEE_FONT_NORMAL, AEE_FONT_NORMAL);
                }
            }
            (void) ISHELL_PostEvent( pMe->m_pShell,
                                                    AEECLSID_DIALER/*AEECLSID_CALL*/,
                                                    EVT_USER_REDRAW,
                                                    0,
                                                    0);
            return TRUE;

        case EVT_USER_REDRAW:
            // Draw prompt bar here
            //DrawBottomBar(pMe->m_pShell,  pMe->m_pDisplay, &pMe->m_rc, BTBAR_OK_BACK);
            REFUI_DRAW_BOTTOMBAR(BTBAR_YES_NO)
            //(void)ISTATIC_Redraw(pStatic);
            return TRUE;

        case EVT_DIALOG_END:
            // 注意: 出现这种情况是因 Applet 被挂起关闭对话框。不论是因发起呼叫
            //       还是因其它原因导致此情况均不需要再回到本状态
            if (pMe->m_eDlgRet == DLGRET_CREATE)
            {
                pMe->m_eDlgRet = DLGRET_CANCELED;
            }
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                    // 用户取消呼叫
                    CLOSE_DIALOG(DLGRET_CANCELED);
                    return TRUE;

                case AVK_SELECT:
                    // 用户确认呼叫
                    CLOSE_DIALOG(DLGRET_OK);
                    return TRUE;

                default:
                    break;
            }

            return TRUE;
#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
					case EVT_PEN_UP:
						{
							AEEDeviceInfo devinfo;
							int nBarH ;
							AEERect rc;
							int16 wXPos = (int16)AEE_GET_X(dwParam);
							int16 wYPos = (int16)AEE_GET_Y(dwParam);
						
							nBarH = GetBottomBarHeight(pMe->m_pDisplay);
								
							MEMSET(&devinfo, 0, sizeof(devinfo));
							ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
							SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);
						
							if(CALLAPP_PT_IN_RECT(wXPos,wYPos,rc))
							{
								if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
								{
									boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_USER,AVK_SELECT,0);
									return rt;
								}
								else if(wXPos >= rc.x + (rc.dx/3)	&& wXPos < rc.x + (rc.dx/3)*2 )//左
								{
									boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_USER,AVK_INFO,0);
									return rt;
								}
								else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
								{						
									boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_USER,AVK_CLR,0);
									return rt;
								}
							}
						}
						break;
#endif

        default:
            break;
    }

    return FALSE;
} // IDD_USERCONFIRM_Handler
#endif

/*=============================================================================
FUNCTION: CallApp_SetupCallAudio

DESCRIPTION:
   Sets the current sound path based on the current values of:
      pMe->m_CallVolume
      pMe->m_CallMuted

PARAMETERS:
   *pMe: CCallApp object pointer

RETURN VALUE:
    None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void CallApp_SetupCallAudio(CCallApp *pMe)
{
    AEESoundInfo                soundStuff;
    boolean                     headsetPresent =FALSE;
#ifndef FEATURE_ICM
	AEETCalls po;
#endif

	//Add By zzg 2011_10_25
	uisnd_notify_data_s_type sndInfo;
	uisnd_get_device(&sndInfo);
	MSG_FATAL("***zzg CallApp_SetupCallAudio - dev=%d sMute=%d mMute=%d***", 
	  			sndInfo.out_device, sndInfo.speaker_mute, sndInfo.microphone_mute);	
	//Add End

    if (SUCCESS != ICONFIG_GetItem(pMe->m_pConfig,CFGI_HEADSET_PRESENT,&headsetPresent,sizeof(headsetPresent)))
    {
        headsetPresent = FALSE;
    }

	MSG_FATAL("***zzg CallApp_SetupCallAudio headsetPresent=%x, m_bHandFree=%x***", headsetPresent, pMe->m_bHandFree, 0);	
	
#ifdef FEATURE_SUPPORT_BT_AUDIO
	MSG_FATAL("***zzg CallApp_SetupCallAudio headsetPresent=%x, m_bHandFree=%x, m_bBtAvailable=%x***", 
				headsetPresent, pMe->m_bHandFree, pMe->m_bBtAvailable);	
#endif
#ifdef FEATURE_SUPPORT_BT_APP
    CALL_FUN_START("CallApp_SetupCallAudio %d %d %d", pMe->m_bHandFree, headsetPresent, pMe->m_bt_audio );
#else
    CALL_FUN_START("CallApp_SetupCallAudio %d %d", pMe->m_bHandFree, pMe->m_CallMuted, 0 );
#endif

	//Add By zzg 2010_09_16 : headset > anyother
	if (headsetPresent)
    {
      soundStuff.eDevice = AEE_SOUND_DEVICE_STEREO_HEADSET; //AEE_SOUND_DEVICE_HEADSET;   
    }
	//Add End
    else if(pMe->m_bHandFree)
    {
#ifdef FEATURE_SPEAKER_PHONE
        soundStuff.eDevice = AEE_SOUND_DEVICE_SPEAKER;
#else
        soundStuff.eDevice = AEE_SOUND_DEVICE_HFK;
#endif
    }
#ifdef FEATURE_SUPPORT_BT_APP
    else if(pMe->m_bt_audio ==  SEND_BT_AUDIO)
    {
        soundStuff.eDevice = AEE_SOUND_DEVICE_BT_HEADSET;
        bt_ui_process_vol_change(pMe->m_CallVolume);
    }
#endif


//Add By zzg 2011_10_28
#ifdef FEATURE_SUPPORT_BT_AUDIO
	//else if(TRUE == pMe->m_bBtAvailable)
	else if (sndInfo.out_device == SND_DEVICE_BT_HEADSET) 
	{				
		soundStuff.eDevice = AEE_SOUND_DEVICE_BT_HEADSET;		
		//bt_ui_process_vol_change(pMe->m_CallVolume);
	}	
	else if (sndInfo.out_device == SND_DEVICE_BT_A2DP_HEADSET)	
	{				
		soundStuff.eDevice = AEE_SOUND_DEVICE_BT_STEREO_HEADSET;		
		//bt_ui_process_vol_change(pMe->m_CallVolume);
	}	
#endif
//Add End



	/*
    else if (headsetPresent)
    {
         soundStuff.eDevice = AEE_SOUND_DEVICE_STEREO_HEADSET; //AEE_SOUND_DEVICE_HEADSET;            
    }
    */
    else
    {
        soundStuff.eDevice = AEE_SOUND_DEVICE_HANDSET;
    }

#ifdef FEATURE_SUPPORT_BT_APP
    if(soundStuff.eDevice != AEE_SOUND_DEVICE_BT_HEADSET)
    {
        pMe->m_bt_audio = SEND_NONE;
    }
#endif

	MSG_FATAL("***zzg CallApp_SetupCallAudio eDevice=%d***", soundStuff.eDevice, 0, 0);

    soundStuff.eMethod = AEE_SOUND_METHOD_VOICE;
    soundStuff.eAPath = AEE_SOUND_APATH_BOTH;
    
#ifdef FEATURE_ICM
    if(AEECM_IS_VOICECALL_CONNECTED(pMe->m_pICM) )
#else
	if(SUCCESS != ITELEPHONE_GetCalls(pMe->m_pITelephone, &po,sizeof(AEETCalls)))
	{
	`	MSG_FATAL("***zzg CallApp_SetupCallAudio SUCCESS != ITELEPHONE_GetCalls***", 0, 0, 0);
		return ;
	}

    if(po.dwCount>0)
#endif
    {
        soundStuff.eEarMuteCtl = AEE_SOUND_MUTECTL_UNMUTED;
        if (pMe->m_CallMuted)
        {
            soundStuff.eMicMuteCtl = AEE_SOUND_MUTECTL_MUTED;
        }

        else
        {
            soundStuff.eMicMuteCtl = AEE_SOUND_MUTECTL_UNMUTED;
        }
    }

    else
    {
        soundStuff.eEarMuteCtl = AEE_SOUND_MUTECTL_MUTED;
        soundStuff.eMicMuteCtl = AEE_SOUND_MUTECTL_MUTED;
    }

	MSG_FATAL("***zzg CallApp_SetupCallAudio 2 eDevice=%d***", soundStuff.eDevice, 0, 0);

    (void) ISOUND_Set(pMe->m_pSound,  &soundStuff);

    ISOUND_SetDevice(pMe->m_pSound);
    if(pMe->m_CallVolume == OEMSOUND_MUTE_VOL)
    {
        pMe->m_CallVolume = OEMSOUND_1ST_VOL;
    }
    ISOUND_SetVolume(pMe->m_pSound,
                     GET_ISOUND_VOL_LEVEL(pMe->m_CallVolume));
}

/*=============================================================================
FUNCTION: CCallApp_MakeSpeedDialCall

DESCRIPTION: Checks for the number in speed dial list of contacts, and if found
             makes the call to the number.

PARAMETERS:
   *pMe: CCallApp object pointer
   *nIndex: Speed Dial slot number

RETURN VALUE:
    void

COMMENTS:


SIDE EFFECTS:

SEE ALSO:
=============================================================================*/
static void CallApp_MakeSpeedDialCall(CCallApp  *pMe)
{
    int          nRet;

    ASSERT(pMe != NULL);

    //CALL_FUN_START("CallApp_MakeSpeedDialCall",0,0,0);

    //search the number according to string dialed
    nRet = CallApp_SpeedDialCall(pMe, pMe->m_DialString);
    CALL_ERR("CallApp_MakeSpeedDialCall %d",nRet,0,0);

    if(SUCCESS != nRet)
    {
        //If it valid speed dial number but it is empty
        //then show the message
        if(WSTRLEN(pMe->m_DialString) >= 3)
        {
            MAKE_CALL_VALUE ret;
            ret = CallApp_MakeCall(pMe);

            if(CALL_FAIL_EMERGENCY_MODE == ret
                ||CALL_FAIL_RESTICT == ret
                ||CALL_FAIL_INVIDE_NUMBER == ret)
            {
                return;
            }

            if(CALL_SUCESS != ret)
            {
                CLOSE_DIALOG(DLGRET_CALL_ATTEMPT_FAILED)
            }
            return;
        }
        //load the speed dial empty resource string
        #ifdef FEATURE_VERSION_C337
		pMe->m_prompt_id = IDS_SPEED_DIAL_QUERY;
		CLOSE_DIALOG(DLGRET_PROMPT)
		#else
        pMe->m_msg_text_id = IDS_SPEED_DIAL_EMPTY;		
        /*
        CallApp_Init_Msgbox_String(pMe, IDS_SPEED_DIAL_EMPTY, pMe->m_DialString);

        if(pMe->m_msg_text != NULL)
        {*/
        CLOSE_DIALOG(DLGRET_MSGBOX);
		#endif
        //}
    }

}// CallApp_MakeSpeedDialCall

// long key "1" to call voice mail
/*=============================================================================
FUNCTION: CCallApp_MakeSpeedDialCall

DESCRIPTION: Checks for the number in speed dial list of contacts, and if found
             makes the call to the number.

PARAMETERS:
   *pMe: CCallApp object pointer
   *nIndex: Speed Dial slot number

RETURN VALUE:
    void

COMMENTS:


SIDE EFFECTS:

SEE ALSO:
=============================================================================*/
static void CallApp_MakeVoiceMailCall(CCallApp  *pMe)
{

    AECHAR  voiceMailNum[OEMNV_VOICEMAIL_MAXLEN];
    //AECHAR  voiceMailStr[32];
    ASSERT(pMe != NULL);

    CALL_FUN_START("CallApp_MakeVoiceMailCall",0,0,0);

    // 从配置数据库查询语音信箱号码
    if (SUCCESS != ICONFIG_GetItem(pMe->m_pConfig,CFGI_VOICEMAIL_NUMBER,
                                            voiceMailNum, sizeof(voiceMailNum)))
    {
        voiceMailNum[0] = (AECHAR) '\0';
    }

    if (voiceMailNum[0] != (AECHAR)'\0')
    {
        if (ISHELL_ActiveApplet(pMe->m_pShell) == AEECLSID_DIALER/*AEECLSID_CALL*/)
        {
            WSTRLCPY(pMe->m_DialString, voiceMailNum ,MAX_SIZE_DIAL_STR);

            if(CALL_SUCESS == CallApp_MakeCall(pMe))
            {
                CALL_ERR("MakeVoiceMailCall sucess",0,0,0);
                return;
            }
            else
            {
                //CALL_ERR("MakeVoiceMailCall fail",0,0,0);
                return;
            }
        }
    }
    else//voiceMailNum == NULL
    {
        pMe->m_msg_text_id = IDS_VOICEMAIL_EMPTY;
        /*
        CallApp_Init_Msgbox_String(pMe, IDS_VOICEMAIL_EMPTY, NULL);

        if(pMe->m_msg_text != NULL)
        {*/
        CLOSE_DIALOG(DLGRET_MSGBOX);
        //}
    }
}
/*=============================================================================
FUNCTION: CallApp_MakeCall

DESCRIPTION: Handles the details of making a call.  If the phone is already
             in the conversation state, it will send a flash.

PARAMETERS:
   *pMe: CCallApp object pointer

RETURN VALUE:
boolean: TRUE if the call origination was successful.
         FALSE if unable to originate the call due to FDN restrictions.


COMMENTS:
   The number to dial is taken from CCallApp::m_DialString.

   The only time this function will return FALSE is when FDNs are enabled
   and the dialed number is not in the R-UIM FDN list.

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
MAKE_CALL_VALUE CallApp_MakeCall(CCallApp *pMe)
{
    boolean lastIsP = FALSE;  
    AECHAR *pause       = NULL;
#ifdef FEATURE_ICM
    AEECMCallID nCallID = 0;
#else
    uint8 nCallID = 0;
#endif
#ifdef FEATURE_APP_PAUSE_TIMER
    AECHAR *timer       = NULL;
#endif //FEATURE_APP_PAUSE_TIMER
    AECHAR wbuf[MAX_SIZE_DIAL_STR] = {0};
#ifdef FEATURE_ICM
    AEECMCallInfo ci;
#else
    char buf[MAX_SIZE_DIAL_STR] = {0};
    AEETCallInfo ci;
#endif

    boolean b_have_p    = FALSE;
    boolean b_have_t    = FALSE;
    AEECLSID cls        = AEE_Active();
    boolean b_energency =FALSE;
    boolean b_restict   =FALSE;
#ifndef FEATURE_ICM
	AEETCalls po;
#endif
#if defined(FEATURE_VERSION_C337)   	
    pMe->m_isIncoming 	= FALSE;
#endif
	MSG_FATAL("***zzg CallApp_MakeCall cls=%x***", cls, 0, 0);

    ISHELL_CancelTimer(pMe->m_pShell,(PFNNOTIFY)CallApp_MakeCall, pMe);
    if(cls == AEECLSID_DIALER ||cls == AEECLSID_CORE_APP)
    {
    }
    else
    {
        CALL_ERR("Active Clisid not allow make call",0,0,0);

		MSG_FATAL("***zzg Active Clisid not allow make call***", 0, 0, 0);
		
        pMe->m_b_auto_redial     = FALSE;
        pMe->m_auto_redial_count = 0;
        return CALL_FAIL_ANOTHER;
    }

    // Can only make emergency calls while emgcall is TRUE
    //if (CallApp_IsEmergencyMode(pMe->m_pICM)/*pMe->idle_info.IsEmgCall*/ &&
    //                                        !CallApp_IsEmergency_Number(pMe->m_DialString))
    //{
    //    CALL_ERR("CallApp_IsEmergency_Number call",0,0,0);
    //    return TRUE;
    //}

    // Can only make emergency calls while CHV1 is locked
    //if (pMe->idle_info.uimLocked && !CallApp_IsEmergency_Number(pMe->m_DialString))
    //{
    //    CALL_ERR("CallApp_IsEmergency_Number call",0,0,0);
    //    return TRUE;
    //}
    //if ((CallApp_IsEmergencyMode(pMe->m_pICM)
    //    ||pMe->idle_info.uimLocked)
    //    &&!CallApp_IsEmergency_Number(pMe->m_DialString))
    //{
    //    CLOSE_DIALOG(DLGRET_DIALER_INVALID_EMGNUMBER)
    //    return CALL_FAIL_EMERGENCY_MODE;
    //}

    if(pMe->m_CallsTable)
    {
#ifdef FEATURE_ICM
        if(AEE_SUCCESS != ICM_GetCallInfo(pMe->m_pICM, pMe->m_CallsTable->call_id, &ci, sizeof(AEECMCallInfo)))
        {
            return CALL_FAIL_ANOTHER;
        }
        //ASSERT(ci != NULL);

        // Are we currently in an OTAPA call?
        if ((ci.call_type == AEECM_CALL_TYPE_OTAPA || ci.call_type == AEECM_CALL_TYPE_CS_DATA)
            && ci.call_state != AEECM_CALL_STATE_IDLE && ci.call_state != AEECM_CALL_STATE_NONE)
        {        	
            CALL_ERR(" currently in an OTAPA call? call",0,0,0);
            // End the OTAPA call, make the call afterwards
            // (see CallApp_HandleCallStateChange)
            pMe->m_makeCallAfterOTAPA = TRUE;
            ICM_EndAllCalls(pMe->m_pICM);
            return CALL_FAIL_ANOTHER;
        }
#else
        if(AEE_SUCCESS != ITELEPHONE_GetCallInfo(pMe->m_pITelephone, pMe->m_CallsTable->call_id, &ci, sizeof(AEETCallInfo)))
        {
            return CALL_FAIL_ANOTHER;
        }
        //ASSERT(ci != NULL);

        // Are we currently in an OTAPA call?
        if ((ci.call_type == AEET_CALL_TYPE_OTAPA || ci.call_type == AEET_CALL_TYPE_CS_DATA)
            && ci.call_state != AEET_CALL_STATE_IDLE && ci.call_state != AEET_CALL_STATE_NONE)
        {        	
            CALL_ERR(" currently in an OTAPA call? call",0,0,0);
            // End the OTAPA call, make the call afterwards
            // (see CallApp_HandleCallStateChange)
            pMe->m_makeCallAfterOTAPA = TRUE;
            ICALLMGR_EndAllCalls(pMe->m_pICallMgr);
            return CALL_FAIL_ANOTHER;
        }
#endif
    }
    // If we are already in a conversation, later on in this function we will
    // send a Flash With Info to the network with the number to dial.  Since
    // there will be no response from the network, we optimistically assume
    // that it was successful and we will be in a 3-way call.
    //
    // Therefore, we need to new call history entry for the current call before
    // entering the 3-way call.
    // Remove the last trailing pause characters from the number
    //如果最后一位是P的话，直接删除

    {
        int len;

        len = WSTRLEN(pMe->m_DialString);
        while ( (len > 0) && (DIALER_PAUSE_AECHAR == pMe->m_DialString[len-1]) )
        {
            lastIsP = TRUE;
            pMe->m_DialString[len-1] = (AECHAR) '\0';
            len--;
        }

        if(len == 0 || pMe->m_DialString[0] == DIALER_PAUSE_AECHAR
#ifdef FEATURE_APP_PAUSE_TIMER
            ||pMe->m_DialString[0] == DIALER_TIMER_AECHAR
#endif
            )
        {
            pMe->m_msg_text_id = IDS_MSG_EXCEPTIONAL_CHAR;
            /*
            CallApp_Init_Msgbox_String(pMe, IDS_MSG_EXCEPTIONAL_CHAR, NULL);

            if(pMe->m_msg_text != NULL)
            {*/
            CLOSE_DIALOG(DLGRET_MSGBOX)
            //}

            return CALL_FAIL_INVIDE_NUMBER;
        }
    }

    pause = WSTRCHR(pMe->m_DialString, DIALER_PAUSE_AECHAR);//search the first P
    CALL_ERR("pause = %x",pause,0,0);
#ifdef FEATURE_APP_PAUSE_TIMER
    timer = WSTRCHR(pMe->m_DialString, DIALER_TIMER_AECHAR);//search the first T
    CALL_ERR("timer = %x",timer,0,0);

    if (NULL == timer && NULL == pause)
    {
        // copy as much of m_DialString to m_CallString as we can. If
        // m_DialString happens to be be longer than m_CallString..too bad
        WSTRNCOPYN(wbuf,   MAX_SIZE_DIAL_STR, pMe->m_DialString,  -1);
    }
    // 台湾客户要求IP接入码能同时输入'T'和'P'键，需增强相关处理逻辑
    pMe->m_TimerString[0] = 0;
    pMe->m_PauseString[0] = 0;

    // 若同时出现'P'和'T',谁在前就先处理谁
    if ((NULL != timer) && (NULL != pause))
    {
        if (timer>pause)
        {   // 'P'在前
            timer=NULL;
        }
        else
        {
            pause=NULL;
        }
    }
    // have timer
    if (NULL != timer)
    {
        // copy up to (and including) the first pause character to m_CallString
        WSTRNCOPYN ( wbuf, MAX_SIZE_DIAL_STR,
                                                pMe->m_DialString, /*lint --e(834)*/ timer - pMe->m_DialString);
        WSTRNCOPYN ( pMe->m_TimerString, sizeof(pMe->m_TimerString)/sizeof(AECHAR),
                                                timer+1, /*skip the pause character*/  -1 );
        b_have_t = TRUE;
    }
    // have pause
    else if (NULL != pause)
    {
        // copy up to (and including) the first pause character to m_CallString
        WSTRNCOPYN ( wbuf,  MAX_SIZE_DIAL_STR,
                                                pMe->m_DialString, /*lint --e(834)*/ pause - pMe->m_DialString);
        WSTRNCOPYN ( pMe->m_PauseString, sizeof(pMe->m_PauseString)/sizeof(AECHAR),
                                                pause+1,  /* skip the pause character*/    -1 );
        b_have_p = TRUE;
    }

    //in this time,the wbuf not include p/w
    //CALL_ERR("%S wbuf1",wbuf,0,0);
    //CALL_ERR("%S pMe->m_TimerString1",pMe->m_TimerString,0,0);
    //CALL_ERR("%S pMe->m_PauseString1",pMe->m_PauseString,0,0);
    // 呼出限制不仅是拨号界面，还包括电话本、呼叫记录等的直接呼出限制
    //if(CallApp_IsRestictCallNumber_Ex(pMe,wbuf,TRUE))
    //{
    //    if(pMe->m_pActiveDlgID == IDD_NUMEDIT)
    //    {
    //        CLOSE_DIALOG(DLGRET_DIALER_RESTRICT)
    //    }

    //    return TRUE;
    //}
#else //FEATURE_APP_PAUSE_TIMER
    if (NULL == pause)
    {
        // copy as much of m_DialString to m_CallString as we can. If
        // m_DialString happens to be be longer than m_CallString..too bad
        WSTRNCOPYN(wbuf,  MAX_SIZE_DIAL_STR, pMe->m_DialString,  -1);
                                                pMe->m_PauseString[0] = 0;
    }
    else //include p
    {
        // copy up to (and including) the first pause character to m_CallString
        WSTRNCOPYN(wbuf, MAX_SIZE_DIAL_STR,
                                                pMe->m_DialString,  /*lint --e(834)*/ pause - pMe->m_DialString);
        // ignore multiple pause characters
        while (DIALER_PAUSE_AECHAR == pause[1])
        {
            pause++;
        }

        // copy up to (and including) the first pause character to m_CallString
        //WSTRNCOPYN(wbuf, MAX_SIZE_DIAL_STR,
        //           pMe->m_DialString,  /*lint --e(834)*/ pause - pMe->m_DialString + 1);
        WSTRNCOPYN(pMe->m_PauseString,sizeof(pMe->m_PauseString)/sizeof(AECHAR),
                                                pause+1,  /*skip the pause character*/  -1);
        b_have_p = TRUE;
    }
    //in this time,the wbuf not include p/w
#endif //FEATURE_APP_PAUSE_TIMER
    b_energency = CallApp_IsEmergency_Number(wbuf);
    if(!b_energency)
    {
        int len = 0;
        
        if( TRUE == lastIsP)
        {
            len = WSTRLEN(wbuf);
            wbuf[len] = DIALER_PAUSE_AECHAR;
            wbuf[len + 1] = (AECHAR) '\0';
        }
        b_restict  = CallApp_IsRestictCallNumber_Ex(pMe, wbuf,TRUE);

        if( TRUE == lastIsP)
        {
            wbuf[len] = (AECHAR) '\0';
        }
    }
#ifdef FEATURE_ICM
    if ((CallApp_IsEmergencyMode(pMe->m_pICM)
#else
    if ((CallApp_IsEmergencyMode(pMe->m_pITelephone)
#endif
        ||pMe->idle_info.uimLocked)
        &&!b_energency)
    {
    //Add by pyuangui 2013-01-08
    #ifdef FEATURE_VERSION_C337
        ICM_SetSystemPreference(pMe->m_pICM,
                            AEECM_MODE_PREF_PERSISTENT, AEECM_PREF_TERM_PERMANENT, 0,
                            AEECM_GW_ACQ_ORDER_PREF_NO_CHANGE, AEECM_BAND_PREF_NO_CHANGE,
                            AEECM_ROAM_PREF_NO_CHANGE, AEECM_HYBR_PREF_NO_CHANGE,
                            AEECM_SRV_DOMAIN_PREF_NO_CHANGE, AEECM_NETWORK_SEL_MODE_PREF_NO_CHANGE,
                            NULL, NULL, NULL);
	#else
        CLOSE_DIALOG(DLGRET_DIALER_INVALID_EMGNUMBER)
        return CALL_FAIL_EMERGENCY_MODE;
	#endif
	//Add End
    }
#if 0
    if(IsRunAsFactoryTestMode() && !b_energency)
    {
        CLOSE_DIALOG(DLGRET_EMGNUMBER_CALL_ONLY)
        return CALL_FAIL_EMERGENCY_MODE;
    }
#endif
    // 呼出限制不仅是拨号界面，还包括电话本、呼叫记录等的直接呼出限制 
    if(b_restict)
    {
        if(pMe->m_pActiveDlgID == IDD_IP_NUMBER)
        {
            CLOSE_DIALOG(DLGRET_MSGBOX)
        }
        else if(pMe->m_pActiveDlgID == IDD_IP_NUMBER)
        {
            CLOSE_DIALOG(DLGRET_DIALER_RESTRICT)
        }
        return CALL_FAIL_RESTICT;
    }
#ifdef FEATURE_ICM
    pMe->m_b_incall = AEECM_IS_VOICECALL_CONNECTED(pMe->m_pICM);
#else
	if(SUCCESS != ITELEPHONE_GetCalls(pMe->m_pITelephone, &po,sizeof(AEETCalls)))
	{
	return FALSE;
	}
	
	pMe->m_b_incall = po.dwCount>0?TRUE:FALSE;
#endif

    if(pMe->m_b_incall == FALSE)
    {
        pMe->m_Is3Way = FALSE;
#ifdef FEATURE_FDN
        boolean   fdnEnabled = FALSE;

        (void) ICONFIG_GetItem(pMe->m_pConfig,
                                                CFGI_FDN_ENABLED,
                                                &fdnEnabled,
                                                sizeof(fdnEnabled));

        if (fdnEnabled && !CallApp_MatchFDNNumber(pMe, wbuf))
        {
            return CALL_FAIL_ANOTHER;
        }
#endif /* FEATURE_FDN */

        //if(CallApp_Search_HasExceptionalChar(wbuf))
        //{
        //    CallApp_Init_Msgbox_String(pMe, IDS_MSG_EXCEPTIONAL_CHAR, NULL);

        //    if(pMe->m_msg_text != NULL)
        //    {
        //        CLOSE_DIALOG(DLGRET_MSGBOX)
        //    }

        //    return TRUE;
        //}
        //else
        //{
        //    //we need save Originate call number
        //    if(CheckAEEReturnStatus(ICM_OriginateVoiceCall(pMe->m_pICM, wbuf, &nCallID)) == FALSE)
        //    {
        //        CALL_ERR("First ICM_OriginateVoiceCall FAILED ", 0, 0, 0);
        //        return FALSE;
        //    }
        //}

    }

    else //the phone have a call,will enter three_way call
    {
        //if(CheckAEEReturnStatus(ICM_OriginateVoiceCall(pMe->m_pICM, wbuf, &nCallID)) == FALSE)
        //{
        //    CALL_ERR("Secend ICM_OriginateVoiceCall FAILED", 0, 0, 0);
        //    return FALSE;
        //}

        pMe->m_Is3Way = TRUE;
    }

    if(CallApp_Search_HasExceptionalChar(wbuf))
    {
        pMe->m_msg_text_id = IDS_MSG_EXCEPTIONAL_CHAR;
        /*
        CallApp_Init_Msgbox_String(pMe, IDS_MSG_EXCEPTIONAL_CHAR, NULL);

        if(pMe->m_msg_text != NULL)
        {*/
        CLOSE_DIALOG(DLGRET_MSGBOX)
        //}

        return CALL_FAIL_INVIDE_NUMBER;
    }
    else
    {
#ifndef FEATURE_ICM
		ICall *pCall = NULL;
#endif
		#if defined(FEATURE_VERSION_W515V3)||defined(FEATURE_VERSION_S1000T)
		if(WSTRNCMP(wbuf,L"+",1) == 0)
        {
        	if ( WSTRNCMP(wbuf,L"+62",3) == 0)
        	{
				AECHAR internation[MAX_SIZE_DIAL_STR] = {0};
	            //ICONFIG_GetItem(pMe->m_pConfig, CFGI_FDN_ENABLED, &internation,sizeof(internation));
	            STRTOWSTR("0",internation,sizeof(internation));
	            //WSTRLCPY(internation,(AECHAR)OEM_INTERNATION_NUMBER ,MAX_SIZE_DIAL_STR);
	            WSTRLCAT(internation, &wbuf[3] ,MAX_SIZE_DIAL_STR);
	            WSTRLCPY(wbuf,internation,MAX_SIZE_DIAL_STR);
        	}
        	else if ( WSTRNCMP(wbuf,L"+66",3) == 0)
        	{
				AECHAR internation[MAX_SIZE_DIAL_STR] = {0};
	            //ICONFIG_GetItem(pMe->m_pConfig, CFGI_FDN_ENABLED, &internation,sizeof(internation));
	            STRTOWSTR("0",internation,sizeof(internation));
	            //WSTRLCPY(internation,(AECHAR)OEM_INTERNATION_NUMBER ,MAX_SIZE_DIAL_STR);
	            WSTRLCAT(internation, &wbuf[3] ,MAX_SIZE_DIAL_STR);
	            WSTRLCPY(wbuf,internation,MAX_SIZE_DIAL_STR);
        	}
        	else
        	{
	            AECHAR internation[MAX_SIZE_DIAL_STR] = {0};
	            //ICONFIG_GetItem(pMe->m_pConfig, CFGI_FDN_ENABLED, &internation,sizeof(internation));
	            #if defined (FEATURE_VERSION_W515V3)||defined(FEATURE_VERSION_S1000T)
	            STRTOWSTR("00",internation,sizeof(internation));
                #else
                STRTOWSTR("0",internation,sizeof(internation));
                #endif
	            //WSTRLCPY(internation,(AECHAR)OEM_INTERNATION_NUMBER ,MAX_SIZE_DIAL_STR);
	            WSTRLCAT(internation, &wbuf[1] ,MAX_SIZE_DIAL_STR);
	            WSTRLCPY(wbuf,internation,MAX_SIZE_DIAL_STR);
            }
        }
        #endif

        //we need save Originate call number
#ifdef FEATURE_ICM
        if(CheckAEEReturnStatus(ICM_OriginateVoiceCall(pMe->m_pICM, wbuf, &nCallID)) == FALSE)
#else
        WSTRTOSTR(wbuf,buf,MAX_SIZE_DIAL_STR);
        if(CheckAEEReturnStatus(ICALLMGR_OriginateVoice(pMe->m_pICallMgr, buf, &pCall,NULL)) == FALSE)
#endif
        {
            CALL_ERR("ICM_OriginateVoiceCall FAILED ", 0, 0, 0);
            return CALL_FAIL_ANOTHER;
        }
#ifndef FEATURE_ICM
		if (pCall != NULL)
		{
			ICALL_Release(pCall);
		}
#endif
    }

    CALL_ERR("b_have_p = %d,b_have_t = %d Is3Way = %d",b_have_p,b_have_t,pMe->m_Is3Way);

/* not show P/T after number
    if(b_have_p)
    {
        WSTRCAT(wbuf, L"P");
    }

    if(b_have_t)
    {
        WSTRCAT(wbuf, L"T");
    }
*/
    CallApp_Add_Number_To_Call_Table(pMe,wbuf,nCallID,AEECALLHISTORY_CALL_TYPE_TO/*CALLHISTORY_OUTGOING*/,PI_ALLOWED,FALSE,b_energency,b_restict);
    return CALL_SUCESS;
}

/*=============================================================================
FUNCTION: CallApp_DrawDialerString

DESCRIPTION:  Manually display the dialing digits with a large font,
              right to left, bottom to top.

PARAMETERS:
   *pMe:      CCallApp object pointer
   *dialerRect: Rectangle to draw the string in.
   *dialStr:    Dial string to display

RETURN VALUE:
   None

COMMENTS:
   It is assumed that dialerRect will be large enough for
   at least one line of text

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
#ifdef FEATURE_LARGE_DIALING_DIGITS

static void CallApp_DrawDialerString(CCallApp   *pMe,  AECHAR const *dialStr)
{
    static AECHAR  str[MAX_SIZE_DIALER_TEXT + MAX_SIZE_DIALER_TEXT/4 +1];
    static AECHAR  revStr[MAX_SIZE_DIALER_TEXT + MAX_SIZE_DIALER_TEXT/4 +1];
    AEERect           dialerRect;
    AECHAR           *srcStr;
    AECHAR           *dstStr;
    int                    len;
    //int                    largeLineHeight;
    int                    y;
    int                    fits;
    int                    pixelLen;
    //AEEFontInfo    nFontInfo;
    IBitmap            *pDestBmp;
    static int           nColorFontIndex = 0;
    int                   nWidth;
    int                   i;
#ifdef FEATURE_DIAL_AUTO_HYPHEN
    int                   j;
#endif
    static AECHAR  clrChar[2];
    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, NULL);


    clrChar[0] = '\0';
    clrChar[1] = '\0';

    CALL_FUN_START("CallApp_DrawDialerString", 0, 0, 0);

    SETAEERECT(&dialerRect,
                                            0,
                                            0,
                                            pMe->m_rc.dx,
                                            pMe->m_rc.dy - BOTTOMBAR_HEIGHT);
    // Clear the dialer rectangle
    Appscommon_ResetBackgroundEx(pMe->m_pDisplay, &dialerRect, TRUE);
    
    // Copy the dialer string into a temp buffer so it can be modified

#ifdef FEATURE_DIAL_AUTO_HYPHEN
    MEMSET(str,0,sizeof(str));
    for ( i=0,j=0; i < WSTRLEN(dialStr); i++,j++)
    {
        if ( i==0 || i%4 )
        {
            str[j] = dialStr[i];
        }
        else
        {
            str[j] = '-';
            j++;
            str[j] = dialStr[i];
        }
    }
#else //#ifdef FEATURE_DIAL_AUTO_HYPHEN
    WSTRNCOPYN(str, sizeof(str)/sizeof(str[0]), dialStr,  -1);
#endif //#ifdef FEATURE_DIAL_AUTO_HYPHEN
    // Return if empty
    len = WSTRLEN(str);
    if (len <= 0)
    {
        return;
    }

    pDestBmp = IDISPLAY_GetDestination ( pMe->m_pDisplay );
    if ( pDestBmp == NULL )
    {
        return;
    }

    // Build the revStr buffer.
    //
    // 'revStr' is the reverse string of 'str'.
    srcStr = str;
    dstStr = revStr + len;

    *dstStr = '\0';
    for (;;)
    {
        if ('\0' == *srcStr)
        {
            break;
        }

        dstStr--;
        *dstStr = *srcStr;
        srcStr++;
    }
    ASSERT(dstStr == revStr);

    // Main drawing loop.
    //
    // Each iteration of this loop will draw one line of the dialing
    // digits, starting from the bottom of the rectangle and moving up.
    //
    y = dialerRect.dy;

#if defined( FEATURE_CALL_RECORDER)
    if( pMe->m_bRecorderOn)
    {
    	y -= pMe->m_large_Num_Height;
    }
#endif

    for (;;)
    {
        if ('\0' == *dstStr)
        {
            // Nothing more to display
            break;
        }

        y -= pMe->m_large_Num_Height;

        if ( y < dialerRect.y )
        {
            // Out of room in the rectangle
            break;
        }

        // Determine how much of the string will fit on the line
        // (the 'fits' var will return the number of characters that will fit)
#if 0
        if(pMe->m_pBigNumFont)
        {
            if ( SUCCESS != IFONT_MeasureText( pMe->m_pBigNumFont,
                                                    dstStr,              // text string to be measured in pixels
                                                    -1,                  // number of AECHARs to measure
                                                    dialerRect.dx,      // maximum available width in pixels.
                                                    &fits,               // number of characters that can fit in the available width
                                                    &pixelLen) )         // total width of the characters that fit
            {
                break;
            }
        }
#endif
        if ( pixelLen <= 0 )
        {
            break;
        }

        //ASSERT ( pixelLen <= dialerRect->dx );
        //pixelLen = dialerRect->dx - pixelLen;
        nWidth = (int)pixelLen / fits;

        // Move dstStr past the characters about to be drawn
        dstStr += fits;

        // Move srcStr to the location of the characters about to be drawn
        //srcStr -= fits;

        for ( i=1; i<=fits; i++ )
        {
            nColorFontIndex = (len--)%11;

            srcStr--;
            clrChar[0] = *srcStr;
#if 0
            if( pMe->m_pBigNumFont )
            {
#ifdef LCD_COLOR
                (void) IFONT_DrawText( pMe->m_pBigNumFont,
                                                    pDestBmp,                    // bitmap
                                                    dialerRect.dx - (nWidth*i),//pixelLen,                    // x
                                                    y,                           // y
                                                    clrChar,//srcStr,                      // pcText[]
                                                    -1,                          // number of characters in pcText[] to be drawn
                                                    IBITMAP_RGBToNative(pDestBmp,gColorFont[nColorFontIndex]),// color to draw the text
                                                    CLR_USER_BACKGROUND,         // color to draw the background
                                                    &dialerRect,                  // clipping rectangle
                                                    IDF_TEXT_TRANSPARENT );      // flag
#else
                (void) IFONT_DrawText( pMe->m_pBigNumFont,
                                                    pDestBmp,                    // bitmap
                                                    dialerRect.dx - (nWidth*i),//pixelLen,                    // x
                                                    y,                           // y
                                                    clrChar,//srcStr,                      // pcText[]
                                                    -1,                          // number of characters in pcText[] to be drawn
                                                    CLR_USER_TEXT,               // color to draw the text
                                                    CLR_USER_BACKGROUND,         // color to draw the background
                                                    &dialerRect,                  // clipping rectangle
                                                    IDF_TEXT_TRANSPARENT );      // flag

#endif
            }
#endif
        }

        // Forget about the characeters that were just drawn.
        *srcStr = '\0';
    }

    if ( pDestBmp )
    {
        IBITMAP_Release ( pDestBmp );
        pDestBmp = NULL;
    }

}

#else //#ifdef FEATURE_LARGE_DIALING_DIGITS

static void CallApp_DrawDialerString(CCallApp   *pMe,  AECHAR const *dialStr)
{
    AECHAR         revStr[MAX_SIZE_DIALER_TEXT];
    AECHAR const  *srcStr;
    AECHAR        *dstStr;
    int            len;
    int            y;
    int            tempy;
    int            fits;
    AEERect        dialerRect;
    int            nLineMax;
    int            nLine;
    int            i;
    NativeColor    clrFG;
    IBitmap       *pBmp = NULL;

    pMe->m_pCurrNumFont     = NULL;
    pMe->m_nCurrLineSpace   = 0;
	MSG_FATAL("pMe->m_rc.dy==============%d",pMe->m_rc.dy,0,0);
    
    #ifdef FEATURE_LCD_TOUCH_ENABLE
	SETAEERECT(&dialerRect,
    0,
    0,
    pMe->m_rc.dx,
    pMe->m_rc.dy - BOTTOMBAR_HEIGHT-220 );
    #else
	SETAEERECT(&dialerRect,
    0,
    0,
    pMe->m_rc.dx,
	pMe->m_rc.dy - BOTTOMBAR_HEIGHT );
	#endif
               
    
    // Clear the dialer rectangle
    #ifdef FEATURE_LCD_TOUCH_ENABLE
	CallApp_Draw_BackGround(pMe,&dialerRect);
	#else
    Appscommon_ResetBackgroundEx(pMe->m_pDisplay, &dialerRect, TRUE);
    #endif
    // Return if empty
    len = WSTRLEN(dialStr);
    if (len <= 0)
    {
        return;
    }

    IDISPLAY_GetDeviceBitmap(pMe->m_pDisplay, &pBmp);
    if(pBmp == NULL)
    {
        return;
    }
    // Build the revStr buffer.
    //
    // 'revStr' is the reverse string of 'str'.
    srcStr = dialStr;
    dstStr = revStr + len;
    
    *dstStr = '\0';
    for (;;)
    {
        if ('\0' == *srcStr)
        {
            break;
        }

        dstStr--;
        *dstStr = *srcStr;
        srcStr++;
    }
    
    dstStr = revStr;
    // 计算行数和确定使用的字体
    if(pMe->m_pBigNumFont && pMe->m_pCurrNumFont == NULL)
    {
        pMe->m_pCurrNumFont = pMe->m_pBigNumFont;
        MSG_FATAL(" pMe->m_large_Num_Height=%d", pMe->m_large_Num_Height,0,0);
        #if defined(FEATURE_LANG_ARABIC)
        pMe->m_nCurrNumHeight =30;// pMe->m_large_Num_Height;
        #else
        pMe->m_nCurrNumHeight =pMe->m_large_Num_Height;
        #endif
        MSG_FATAL("CallApp_DrawDialerString....................1",0,0,0);
        nLineMax    = dialerRect.dy/pMe->m_nCurrNumHeight;
		#ifndef FEATURE_LCD_TOUCH_ENABLE
        pMe->m_nCurrLineSpace = (dialerRect.dy - pMe->m_nCurrNumHeight*nLineMax)/(nLineMax-1);
		#endif
        nLine = 0;

		MSG_FATAL("***zzg CallApp_DrawDialerString dialerRect.dy=%d, nLineMax=%d", dialerRect.dy,nLineMax,0);
		
        while(1)
        {
            if ('\0' == *dstStr)
            {
                // Nothing more to display
                break;
            }
            
            fits     = 0;
            IFONT_MeasureText(pMe->m_pCurrNumFont, dstStr, -1, dialerRect.dx, &fits, NULL);
            if(fits == 0)
            {
                break;
            }
			MSG_FATAL("fits=======%d",fits,0,0);
            pMe->m_nCurrLineFits[nLine] = fits;
            dstStr += fits;
            nLine++;
        }
        MSG_FATAL("CallApp_DrawDialerString....................2",0,0,0);
        if(nLine <= nLineMax)
        {
            
        }
        else
        {
            pMe->m_pCurrNumFont = NULL;
        }
    }
	MSG_FATAL("CallApp_DrawDialerString....................3",0,0,0);
    dstStr = revStr;
	MSG_FATAL("CallApp_DrawDialerString....................300",0,0,0);
    if(pMe->m_pNormalNumFont && pMe->m_pCurrNumFont == NULL)
    {
    	MSG_FATAL("nLineMax===========%d",nLineMax,0,0);
        pMe->m_pCurrNumFont = pMe->m_pNormalNumFont;
        pMe->m_nCurrNumHeight = pMe->m_Normal_Num_Height;
        nLineMax    = dialerRect.dy/pMe->m_nCurrNumHeight;
		#ifndef FEATURE_LCD_TOUCH_ENABLE
        pMe->m_nCurrLineSpace  = (dialerRect.dy - pMe->m_nCurrNumHeight*nLineMax)/(nLineMax-1);
		#endif
		MSG_FATAL("pMe->m_nCurrLineSpace===========%d",pMe->m_nCurrLineSpace,0,0);
        nLine = 0;
        while(1)
        {
            if ('\0' == *dstStr)
            {
                // Nothing more to display
                break;
            }
            
            fits     = 0;
			MSG_FATAL("CallApp_DrawDialerString....................30",0,0,0);
            IFONT_MeasureText(pMe->m_pCurrNumFont, dstStr, -1, dialerRect.dx, &fits, NULL);
			MSG_FATAL("CallApp_DrawDialerString....................31",0,0,0);
            if(fits == 0)
            {
                break;
            }
            pMe->m_nCurrLineFits[nLine] = fits;
            dstStr += fits;
            nLine++;
            MSG_FATAL("nLine---=%d",nLine,0,0);
        }
    }
    MSG_FATAL("CallApp_DrawDialerString....................4",0,0,0);
    if(pMe->m_pCurrNumFont == NULL)
    {
        if(pBmp)
        {
            IBITMAP_Release(pBmp);
            pBmp = NULL;
        }
        return;
    }
    MSG_FATAL("CallApp_DrawDialerString....................5",0,0,0);
    // 画字符串
    nLine = nLine<nLineMax?nLine:nLineMax;
    pMe->m_nCurrLine = nLine;
    y = dialerRect.dy;
    srcStr = dialStr + len;
    clrFG = IBITMAP_RGBToNative(pBmp, CALLAPP_TEXT_COLOR);
    
    for(i=0;i<nLine;i++)
    {
        y -= pMe->m_nCurrNumHeight;
        srcStr -= pMe->m_nCurrLineFits[i];
        MSG_FATAL("pMe->m_nCurrNumHeight=%d---pMe->m_large_Num_Height=%d",pMe->m_nCurrNumHeight,pMe->m_large_Num_Height,0);
        #if defined(FEATURE_VERSION_1110W516)
        if((int)pMe->m_nCurrNumHeight == 30 ||(int)pMe->m_nCurrNumHeight == pMe->m_large_Num_Height)
          tempy = y-10;
        else
          tempy = y;  
        #else
          tempy = y;
        #endif
        IFONT_DrawText(pMe->m_pCurrNumFont, pBmp, 0, tempy, srcStr, pMe->m_nCurrLineFits[i], clrFG, 0, &dialerRect, IDF_TEXT_TRANSPARENT|IDF_ALIGN_RIGHT);
        #ifndef FEATURE_LCD_TOUCH_ENABLE
        y -= pMe->m_nCurrLineSpace;
		#endif
    }
    MSG_FATAL("CallApp_DrawDialerString....................6",0,0,0);
    // 释放资源
    if(pBmp)
    {
        IBITMAP_Release(pBmp);
        pBmp = NULL;
    }
}
#endif //#ifdef FEATURE_LARGE_DIALING_DIGITS
static void CallApp_keypadtimer(void *pUser)
{
	CCallApp *pMe = (CCallApp *)pUser;
	pMe->m_curpros = 0;
	pMe->b_multenter = FALSE;
}
static AECHAR CallApp_AVKSTAR_2ASCII(CCallApp *pMe)
{
	if(pMe->m_curpros == 0)
	{
		return L'*';
	}
	if(pMe->m_curpros == 1)
	{
		return L'p';
	}
	if(pMe->m_curpros == 2)
	{
		return L'W';
	}
}

//Add by pyuangui 2013-01-10
#ifdef FEATURE_VERSION_C316
static boolean CallApp_FindMemoryCardExist(CCallApp *pMe)
{   
    return (IFILEMGR_Test(pMe->m_pFileMgr, AEEFS_CARD0_DIR)==SUCCESS)?TRUE:FALSE;	
}
#endif
//Add End

/*=============================================================================
FUNCTION: CallApp_AVKType2ASCII

DESCRIPTION:
   Converts a {AVK_0..AVK_9,AVK_STAR,AVK_POUND} to the
   ASCII equivalent

PARAMETERS:
   key: BREW keycode

RETURN VALUE:
   char: ASCII value

COMMENTS:
   Returns the NULL character if the input value is out of range

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static char CallApp_AVKType2ASCII(AVKType key)
{
    if (AVK_STAR == key)
    {
        return '*';
    }

    if (AVK_POUND == key)
    {
        return '#';
    }
	//add by ydc 
	if (AVK_O == key)
    {
        return '+';
    }

    if (AVK_P == key)
    {
        return 'p';
    }

    if (key < AVK_0 || key > AVK_9)
    {
        return '\0';
    }

    return '0' + (char)(key - AVK_0); /*lint !e656*/
}

/*=============================================================================
FUNCTION: CallApp_GetDTMFDuration

DESCRIPTION: Determines the current DTMF tone length

PARAMETERS:
   *pMe: CCallApp object pointer

RETURN VALUE:
   DMTFToneDuration const *: current DTMF tone length

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static const DTMFToneDuration *CallApp_GetDTMFDuration(CCallApp *pMe)
{
    //keyToneLength len = OEMNV_KEYTONE_NORMAL;

    //(void) ICONFIG_GetItem(pMe->m_pConfig,
    //                                        CFGI_KEYTONE_LENGTH,
    //                                        &len,
    //                                        sizeof(keyToneLength));

    if (OEMNV_KEYTONE_LONG == pMe->m_dtmf_length)
    {
        return &dtmfDuration[1];
    }
    return &dtmfDuration[0];
}
/*=============================================================================
FUNCTION: CallApp_LaunchApplet

DESCRIPTION: Start an applet

PARAMETERS:
   *pMe: CCallApp object pointer
   classID: Applet class ID
   retState: The Idle applet state to return to after control returns to the
             Idle applet.

RETURN VALUE:
    None

COMMENTS:
   If the applet fails to start, control moves to retState immediately

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CallApp_LaunchApplet(CCallApp   *pMe,
                               AEECLSID      classID)
{
    CALL_FUN_START("CallApp_LaunchApplet", 0, 0, 0);

    if (!ISHELL_CanStartApplet(pMe->m_pShell, classID))
    {
        // 当前不能启动 指定 Applet
        CALL_ERR("Can't start Applet 0x%X",classID,0,0);
        return FALSE;
    }

    if (SUCCESS != ISHELL_StartApplet(pMe->m_pShell, classID))
    {
        CALL_ERR("Failed to start Applet 0x%X",classID,0,0);
        return FALSE;
    }

    return TRUE;
}
/*=============================================================================
FUNCTION: CallApp_SaveNumber

DESCRIPTION: Invokes the Contacts applet to save a phone number

PARAMETERS:
   *pMe: CCallApp object pointer
   resumeState: The Idle applet state to resume to once the Contacts applet
                exits.
   saveType: Type of save. whether to a new contact or existing contact or
      on RUIM.

RETURN VALUE:
    None

COMMENTS:
   The current value of CCallApp::m_DialString is used as the phone number

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CallApp_SaveNumber(CCallApp         *pMe,
                                     PhoneSaveType saveType)
{
    IContactApp *ca = NULL;

    CALL_FUN_START("CallApp_SaveNumber", 0, 0, 0);

    if (SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                         AEECLSID_APP_CONTACT,
                                         (void**)&ca))
    {
        return FALSE;
    }
    else
    {
        if (SUCCESS != ICONTAPP_SaveNumber(ca,
                                          pMe->m_DialString,
                                          NULL,
                                          NULL,
                                          saveType))
        {
            IContactApp_Release(ca);
            return FALSE;
        }
        IContactApp_Release(ca);
        return TRUE;

    }
}

#if defined(FEATURE_WMS_APP) && !defined(FEATURE_WMSAPP_ONLYSUPPORTVMAIL)
/*=============================================================================

FUNCTION:CallApp_SendSMS

DESCRIPTION: 
    向指定的号码发送短消息

PARAMETERS:
    pMe:
    pNumber:

RETURN VALUE:
    SUCCESS
   
COMMENTS:

SIDE EFFECTS:
SEE ALSO:
=============================================================================*/
static boolean CallApp_SendSMS(CCallApp *pMe)
{
    IWmsApp *pSMSApp;

    ASSERT(pMe != NULL);
    
    if (SUCCESS != ISHELL_CreateInstance( pMe->m_pShell,
                                          AEECLSID_WMSAPP,
                                          (void**)&pSMSApp))
    {
        return EFAILED;
    }

    if (SUCCESS != IWmsApp_SendTextMessageExt(pSMSApp, pMe->m_DialString,NULL))
    {
        (void)IWmsApp_Release(pSMSApp);
        pSMSApp = NULL;
        return EFAILED;
    }    
    
    (void)IWmsApp_Release(pSMSApp);
    pSMSApp = NULL;
    return SUCCESS;
}// CallApp_SendSMS
#endif

/*=============================================================================
FUNCTION: CallApp_NumberLookup

DESCRIPTION: Invokes the Contacts applet to save a phone number

PARAMETERS:
   *pMe: CCallApp object pointer

RETURN VALUE:
    None

COMMENTS:
   The current value of CCallApp::m_DialString is used as the phone number

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CallApp_NumberLookup(CCallApp     *pMe,
                             AECHAR       *Number,
                             AECHAR       *Name,
                             uint32        szName,
                             AECHAR       *ringer,
                             char         *PictureName)
{
    IContactApp *ca = NULL;
     //在电话本里面如果没有给它赋值,直接读取的时候,应该有初试值,否则会出错
    ContInfo    pContInfo = {0};
#ifndef FEATRUE_SET_IP_NUMBER
    CALL_FUN_START("CallApp_NumberLookup----=%d", WSTRLEN(Number), 0, 0);
#endif
    if(0 == WSTRLEN(Number))
    {
        return FALSE;
    }
    
    MSG_FATAL("***zzg CallApp_NumberLookup bIsPowerUp=%d***", bIsPowerUp, 0, 0);

    //Add By zzg 2013_03_29
    if (!bIsPowerUp)
    {
        return TRUE;
    }
    //Add End

    if((pMe->m_cont_cache.call_number[0] != 0) && (0 == WSTRCMP(pMe->m_cont_cache.call_number,Number)))
    {
        CALL_FUN_START("find it in dialer buffer", 0, 0, 0);
        if(NULL != PictureName)
        {
            STRCPY(PictureName,pMe->m_cont_cache.picture_name);
        }
        if(NULL != ringer)
        {
            WSTRLCPY(ringer,pMe->m_cont_cache.ringer,MAX_FILE_NAME);
            //STRCPY(RingerID->szMusicname,pMe->m_cont_cache.music_id.szMusicname);
            //RingerID->midID = pMe->m_cont_cache.music_id.midID;
            //RingerID->ringType = pMe->m_cont_cache.music_id.ringType;
        }
        WSTRLCPY(Name,pMe->m_cont_cache.call_name,MAX_SIZE_NAME_TEXT);
        return TRUE;
    }
    pContInfo.pName = Name;
    pContInfo.wNameLen = szName;
    if (SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                            AEECLSID_APP_CONTACT,
                                            (void**)&ca))
    {
        return FALSE;
    }

    else
    {
#ifdef FEATRUE_SET_IP_NUMBER
        int len = WSTRLEN(pMe->m_b_ip_call);
        if((len > 0) && (WSTRNCMP(Number,pMe->m_b_ip_call,len) == 0)) //in this time ,we need delete the ip number from calling number
        {
        }
        else
        {
            len = 0;
        }
        CALL_ERR("CallApp_NumberLookup %d", len, 0, 0);
        if (SUCCESS != ICONTAPP_NumberLookup(ca,
                                             Number
                                             + len,
                                             &pContInfo))
        {
            IContactApp_Release(ca);
            return FALSE;
        }
#else
        if (SUCCESS != ICONTAPP_NumberLookup(ca, Number,&pContInfo))
        {
         #if  defined(FEATURE_VERSION_S1000T) || defined(FEATURE_VERSION_W515V3)
         if((WSTRNICMP(Number,L"0",1)==0)||(WSTRNICMP(Number,L"+00",3)==0)||(WSTRNICMP(Number,L"+",1)==0)||(WSTRNICMP(Number,L"00",2)==0)) //pContInfo.pName == NULL &&
         {
            
         	AECHAR Temp_Number[AEECM_MAX_DIGITS_LENGTH] = {0};
         	MSG_FATAL("Temp_name...............................",0,0,0);
         	if(WSTRNICMP(Number,L"0",1)==0)
         	{
         		WSTRLCPY(Temp_Number,Number+1,AEECM_MAX_DIGITS_LENGTH);
         	}
            else if(WSTRNICMP(Number,L"+00",3)==0)
         	{
         		WSTRLCPY(Temp_Number,Number+3,AEECM_MAX_DIGITS_LENGTH);
                MEMSET(Number,0,AEECM_MAX_DIGITS_LENGTH);
         	    WSTRLCPY(Number,Temp_Number,AEECM_MAX_DIGITS_LENGTH);
         	}
            else if(WSTRNICMP(Number,L"+",1)==0)
         	{
         		WSTRLCPY(Temp_Number,Number+1,AEECM_MAX_DIGITS_LENGTH);
         	}
            if(WSTRNICMP(Number,L"00",2)==0)
         	{
                MEMSET(Temp_Number,0,AEECM_MAX_DIGITS_LENGTH); 
         		WSTRLCPY(Temp_Number,Number+2,AEECM_MAX_DIGITS_LENGTH);
         	}       
             CALL_FUN_START("CallApp_NumberLookup-Number---=%d", WSTRLEN(Number), 0, 0);
            if (SUCCESS != ICONTAPP_NumberLookup(ca, Temp_Number,&pContInfo))
            {
                IContactApp_Release(ca);
                return FALSE;
            }
         }
         else
         {
             IContactApp_Release(ca);
             return FALSE;
         }
         #else
             IContactApp_Release(ca);
             return FALSE;
         #endif
        }
#endif
{
        int i;
        CALL_FUN_START("CallApp_NumberLookup-Number---=%d", WSTRLEN(Number), 0, 0);
        for(i=0;i<WSTRLEN(Number);i++)
        {
            MSG_FATAL("New_Temp_Number---2=%d",Number[i]-48,0,0);
        
        }
}
        if(ringer != NULL)
        {
            WSTRCPY(ringer, pContInfo.ringName);
        }
        if(NULL != PictureName)
        {
            STRCPY(PictureName,pContInfo.szPhoto);
        }
        STRCPY(pMe->m_cont_cache.picture_name,pContInfo.szPhoto);
        WSTRLCPY(pMe->m_cont_cache.ringer,pContInfo.ringName,MAX_FILE_NAME);
#ifdef FEATURE_ICM
        WSTRLCPY(pMe->m_cont_cache.call_number,Number,AEECM_MAX_DIGITS_LENGTH);
#else
        WSTRLCPY(pMe->m_cont_cache.call_number,Number,AEET_MAX_DIGITS_LENGTH);
#endif
        WSTRLCPY(pMe->m_cont_cache.call_name,Name,MAX_SIZE_NAME_TEXT);
        IContactApp_Release(ca);
        return TRUE;

   }
}

/*=============================================================================
FUNCTION: CallApp_SpeedDialCall

DESCRIPTION: Invokes the Contacts applet to save a phone number

PARAMETERS:
   *pMe        : CCallApp object pointer
    pSpeedDial : Speed dial number

RETURN VALUE:
    SUCCESS

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int CallApp_SpeedDialCall(CCallApp *pMe, AECHAR *pSpeedDial)
{
    IContactApp *ca = NULL;
    pSpeedDial = pSpeedDial;

    CALL_FUN_START("CallApp_SpeedDialCall", 0, 0, 0);

    if (SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                         AEECLSID_APP_CONTACT,
                                         (void**)&ca))
    {
        return EFAILED;
    }
    else
    {
        if (SUCCESS != ICONTAPP_SpeedDialCall(ca, pSpeedDial)) //ICONTAPP make the call
        {
            IContactApp_Release(ca);
            return EFAILED;
        }
        CallApp_MakeCall(pMe);
        IContactApp_Release(ca);
        return SUCCESS;
    }
}// CallApp_SpeedDialCall

/*=============================================================================
FUNCTION: CallApp_ChangeCallVolume

DESCRIPTION: Increase or decreases the current call volume

PARAMETERS:
   *pMe: CCallApp object pointer
   louder: TRUE to increase the call volume
           FALSE to decrease the call volume

RETURN VALUE:
    None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void CallApp_ChangeCallVolume(CCallApp  *pMe,
                                   boolean      louder)
{
    if (louder)
    {
        switch ((OEMSound_Volume_Type)pMe->m_CallVolume)
        {

            case OEMSOUND_1ST_VOL:
                pMe->m_CallVolume = (byte) OEMSOUND_2ND_VOL;
                break;

            case OEMSOUND_2ND_VOL:
                pMe->m_CallVolume = (byte) OEMSOUND_3RD_VOL;
                break;

            case OEMSOUND_3RD_VOL:
                pMe->m_CallVolume = (byte) OEMSOUND_4TH_VOL;
                break;

            case OEMSOUND_4TH_VOL:
                pMe->m_CallVolume = (byte) OEMSOUND_5TH_VOL;
                break;

            case OEMSOUND_5TH_VOL:
                pMe->m_CallVolume = (byte) OEMSOUND_5TH_VOL;
                break;

            //case OEMSOUND_6TH_VOL:
            //case OEMSOUND_7TH_VOL:
            //    pMe->m_CallVolume = (byte) OEMSOUND_7TH_VOL;
            //    break;

            default:
                pMe->m_CallVolume = (byte) OEMSOUND_AVG_VOL;
                break;
        }
    }

    else
    {
        switch ((OEMSound_Volume_Type)pMe->m_CallVolume)
        {

            case OEMSOUND_1ST_VOL:
            case OEMSOUND_2ND_VOL:
                pMe->m_CallVolume = (byte) OEMSOUND_1ST_VOL;
                break;

            case OEMSOUND_3RD_VOL:
                pMe->m_CallVolume = (byte) OEMSOUND_2ND_VOL;
                break;

            case OEMSOUND_4TH_VOL:
                pMe->m_CallVolume = (byte) OEMSOUND_3RD_VOL;
                break;

            case OEMSOUND_5TH_VOL:
                pMe->m_CallVolume = (byte) OEMSOUND_4TH_VOL;
                break;

            default:
                pMe->m_CallVolume = (byte) OEMSOUND_AVG_VOL;
                break;
        }
    }

    {
        AEESoundInfo    si;
        boolean         headsetPresent;
        //AEECMCallInfo *ci;

        (void) ICONFIG_SetItem(pMe->m_pConfig,
                                                CFGI_EAR_VOL,
                                                &pMe->m_CallVolume,
                                                sizeof(byte));

        //ICM_GetCallInfo(pMe->m_pICM, 0, ci, sizeof(AEECMCallInfo));
        if (SUCCESS != ICONFIG_GetItem(pMe->m_pConfig,
                                                CFGI_HEADSET_PRESENT,
                                                &headsetPresent,
                                                sizeof(headsetPresent)))
        {
             headsetPresent = FALSE;
        }

        ISOUND_Get(pMe->m_pSound, &si);
        /*HandFree > bt_ag > headset > handset*/
        
        if(headsetPresent)
        {
            si.eDevice = AEE_SOUND_DEVICE_STEREO_HEADSET; //AEE_SOUND_DEVICE_HEADSET;
        }
        else if (pMe->m_bHandFree)
        {
#ifdef FEATURE_SPEAKER_PHONE
            si.eDevice = AEE_SOUND_DEVICE_SPEAKER;
#else
            si.eDevice = AEE_SOUND_DEVICE_HFK;
#endif
        }
#ifdef FEATURE_SUPPORT_BT_APP
        else if(bcmapp_is_ag_connected() && pMe->m_bt_audio == SEND_BT_AUDIO)
        {
           bt_ui_process_vol_change(pMe->m_CallVolume);
           si.eDevice = AEE_SOUND_DEVICE_BT_HEADSET;
        }
#endif

#ifdef FEATURE_SUPPORT_BT_AUDIO
		else if(CallApp_CheckBtHeadSetInUsing(pMe))
		{
		   //bt_ui_process_vol_change(pMe->m_CallVolume);
		   si.eDevice = AEE_SOUND_DEVICE_BT_HEADSET;
		}
#endif

		/*
        else if(headsetPresent)
        {
            si.eDevice = AEE_SOUND_DEVICE_STEREO_HEADSET; //AEE_SOUND_DEVICE_HEADSET;
        }
        */
        else
        {
           si.eDevice = AEE_SOUND_DEVICE_HANDSET;
        }

        si.eMethod = AEE_SOUND_METHOD_VOICE;

        (void) ISOUND_Set(pMe->m_pSound, &si);
        if(pMe->m_CallVolume == OEMSOUND_MUTE_VOL)
        {
            pMe->m_CallVolume = OEMSOUND_1ST_VOL;
        }
        ISOUND_SetVolume(pMe->m_pSound,
                                                GET_ISOUND_VOL_LEVEL(pMe->m_CallVolume));

    }
}
// add by pyuangui

/*=============================================================================
FUNCTION: CallApp_ChangeCallVolume_AVK_RWD

DESCRIPTION: Increase or decreases the current call volume

PARAMETERS:
   *pMe: CCallApp object pointer

RETURN VALUE:
    None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void CallApp_ChangeCallVolume_AVK_RWD(CCallApp  *pMe)
{
    if ((OEMSound_Volume_Type)pMe->m_CallVolume == OEMSOUND_5TH_VOL)
    {
        pMe->m_CallVolume = (byte) OEMSOUND_1ST_VOL;
        
    }

    else
    {
         switch ((OEMSound_Volume_Type)pMe->m_CallVolume)
        {

            case OEMSOUND_1ST_VOL:
                pMe->m_CallVolume = (byte) OEMSOUND_2ND_VOL;
                break;

            case OEMSOUND_2ND_VOL:
                pMe->m_CallVolume = (byte) OEMSOUND_3RD_VOL;
                break;

            case OEMSOUND_3RD_VOL:
                pMe->m_CallVolume = (byte) OEMSOUND_4TH_VOL;
                break;

            case OEMSOUND_4TH_VOL:
                pMe->m_CallVolume = (byte) OEMSOUND_5TH_VOL;
                break;

            case OEMSOUND_5TH_VOL:
                pMe->m_CallVolume = (byte) OEMSOUND_5TH_VOL;
                break;

            //case OEMSOUND_6TH_VOL:
            //case OEMSOUND_7TH_VOL:
            //    pMe->m_CallVolume = (byte) OEMSOUND_7TH_VOL;
            //    break;

            default:
                pMe->m_CallVolume = (byte) OEMSOUND_AVG_VOL;
                break;
        }
    }

    {
        AEESoundInfo    si;
        boolean         headsetPresent;
        //AEECMCallInfo *ci;

        (void) ICONFIG_SetItem(pMe->m_pConfig,
                                                CFGI_EAR_VOL,
                                                &pMe->m_CallVolume,
                                                sizeof(byte));

        //ICM_GetCallInfo(pMe->m_pICM, 0, ci, sizeof(AEECMCallInfo));
        if (SUCCESS != ICONFIG_GetItem(pMe->m_pConfig,
                                                CFGI_HEADSET_PRESENT,
                                                &headsetPresent,
                                                sizeof(headsetPresent)))
        {
             headsetPresent = FALSE;
        }

        ISOUND_Get(pMe->m_pSound, &si);
        /*HandFree > bt_ag > headset > handset*/
        
        if(headsetPresent)
        {
            si.eDevice = AEE_SOUND_DEVICE_STEREO_HEADSET; //AEE_SOUND_DEVICE_HEADSET;
        }
        else if (pMe->m_bHandFree)
        {
#ifdef FEATURE_SPEAKER_PHONE
            si.eDevice = AEE_SOUND_DEVICE_SPEAKER;
#else
            si.eDevice = AEE_SOUND_DEVICE_HFK;
#endif
        }
#ifdef FEATURE_SUPPORT_BT_APP
        else if(bcmapp_is_ag_connected() && pMe->m_bt_audio == SEND_BT_AUDIO)
        {
           bt_ui_process_vol_change(pMe->m_CallVolume);
           si.eDevice = AEE_SOUND_DEVICE_BT_HEADSET;
        }
#endif

#ifdef FEATURE_SUPPORT_BT_AUDIO
		else if(CallApp_CheckBtHeadSetInUsing(pMe))
		{
		   //bt_ui_process_vol_change(pMe->m_CallVolume);
		   si.eDevice = AEE_SOUND_DEVICE_BT_HEADSET;
		}
#endif

		/*
        else if(headsetPresent)
        {
            si.eDevice = AEE_SOUND_DEVICE_STEREO_HEADSET; //AEE_SOUND_DEVICE_HEADSET;
        }
        */
        else
        {
           si.eDevice = AEE_SOUND_DEVICE_HANDSET;
        }

        si.eMethod = AEE_SOUND_METHOD_VOICE;

        (void) ISOUND_Set(pMe->m_pSound, &si);
        if(pMe->m_CallVolume == OEMSOUND_MUTE_VOL)
        {
            pMe->m_CallVolume = OEMSOUND_1ST_VOL;
        }
        ISOUND_SetVolume(pMe->m_pSound,
                                                GET_ISOUND_VOL_LEVEL(pMe->m_CallVolume));

    }
}
// add end

/*=============================================================================
FUNCTION: CallApp_HandleDialogTimer

DESCRIPTION: Generic timer used by several dialogs.  When this callback
             is invoked, it sends a EVT_USER_DIALOG_TIMEOUT event
             to the Idle applet (which will be routed to the active dialog)

PARAMETERS:
   *pUser: CCallApp object pointer

RETURN VALUE:
    None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void CallApp_HandleDialogTimer(void *pUser)
{
    CCallApp *pMe = (CCallApp *)pUser;

    if((pMe->m_pActiveDlg == NULL)||(pMe == NULL))
    {
        return;
    }

    (void) ISHELL_PostEvent(pMe->m_pShell,
                                            AEECLSID_DIALER/*AEECLSID_CALL*/,
                                            EVT_DISPLAYDIALOGTIMEOUT,
                                            0,
                                            0);
}


/*=============================================================================
FUNCTION: CallApp_HandleConversationTimer

DESCRIPTION: Timer callback used by the conversation dialog to update
             the call timer every second.
             每分钟提示音

PARAMETERS:
   *pUser: CCallApp object pointer

RETURN VALUE:
    None

COMMENTS:
   This timer is always active during a call, regardless of if the Idle
   applet is the active applet or not.

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void CallApp_HandleConversationTimer(void *pUser)
{
    CCallApp *pMe = (CCallApp *)pUser;
    uint32      elapsedCallTime = 0;
    uint32      sec = 0;
    uint32      minute = 0;
    Dialer_call_table *temp = NULL;

    CALL_FUN_START("CallApp_HandleConversationTimer", 0, 0, 0);

    temp = CallApp_Get_First_Entry_In_Call_Table(pMe);

    if(temp)
    {
        elapsedCallTime = ISHELL_GetUpTimeMS(pMe->m_pShell) -
                                                temp->start_time/*pMe->m_ConvStartTime*/;
    }
    // Update the timer on the Conversation dialog if it is currently the
    // active dialog
    //if ( (pMe->m_pActiveDlg != NULL) && (IDD_CONVERSATION == pMe->m_pActiveDlgID) )
    //{
    //    ITimeCtl   *pTimerCtl;

    //    pTimerCtl = (ITimeCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,
    //                                            IDDC_CONVERSATION_TIMER);

    //    (void) ITIMECTL_SetTime(pTimerCtl, (int32)elapsedCallTime);
    //}

    // Check if we need to make the minute alert
    sec    = (elapsedCallTime / 1000L) % 60L;
    minute = elapsedCallTime / (1000L * 60L);

    if ( (minute >= pMe->m_LastMinuteAlert) &&  (sec >= MINUTE_REMINDER_SECOND) )
    {

        //byte minAlert = OEMNV_ALERT_DISABLE;

        //(void) ICONFIG_GetItem(pMe->m_pConfig,
        //                                        CFGI_MINUTE_ALERT,
        //                                        &minAlert,
        //                                        sizeof(minAlert));

        //if (minAlert == OEMNV_ALERT_ENABLE)
        //{
        //    IALERT_StartAlerting(pMe->m_pAlert, AEEALERT_CALLTYPE_VOICE,
        //                                        NULL, AEEALERT_ALERT_BUSY);
        //}
        notifyFMRadioAlertEvent( pMe, TRUE);
        IALERT_MinuteAlert(pMe->m_pAlert);
        notifyFMRadioAlertEvent( pMe, FALSE);
        pMe->m_LastMinuteAlert = minute + 1;
    }

    (void) ISHELL_SetTimer(pMe->m_pShell,
                                                TIMEOUT_MS_CONV_TIMER,
                                                CallApp_HandleConversationTimer,
                                                pMe);
}

/*=============================================================================
FUNCTION: CallApp_AnswerCall

DESCRIPTION:  Answer an incoming or in-band call.

PARAMETERS:
   *p: IIdle interface pointer
bAnswerHold: TRUE if the call is on hold ("18*")


RETURN VALUE:
   boolean:  TRUE if successful

COMMENTS:
   This method is used by the Incoming Call applet to answer a call.  The Idle
   applet will determine if it should answer an incoming or in-band call
   and take the appropriate steps.

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CallApp_AnswerCall(CCallApp  *pMe, boolean bAnswerHold,AEEEvent eCode,uint16 wParam,boolean auto_answer)
{
#ifndef FEATURE_IS2000_SCC_CODES
    PARAM_NOT_REF(bAnswerHold)
#endif /* !FEATURE_IS2000_SCC_CODES */
#ifdef FEATURE_ICM
    AEECMCallInfo ci;
#else
    AEETCallInfo ci;
#endif
    boolean bKeyguardEnabled = FALSE;
    nv_item_type	SimChoice;
    CALL_FUN_START("CallApp_AnswerCall %x %x %d", eCode, wParam, auto_answer);

    if(pMe->m_CallsTable == NULL)
    {
        return FALSE;
    }

    (void) ICONFIG_GetItem(pMe->m_pConfig,
                                        CFGI_KEYGUARD_ENABLED,/*it can not use in current,because oemcfig not process, p1113*/
                                        &bKeyguardEnabled,
                                        sizeof(bKeyguardEnabled));
    CALL_ERR("CallApp_AnswerCall CFGI %x %d",pMe->m_anykey_answer,bKeyguardEnabled,0);
    #if defined(FEATURE_VERSION_C306)||defined(FEATURE_VERSION_W0216A)|| defined(FEAUTRE_VERSION_N450)|| defined(FEATURE_VERSION_N021)|| defined(FEATURE_VERSION_C01)|| defined(FEATURE_VERSION_W516)||defined(FEATURE_VERSION_W208S)|| defined(FEATURE_VERSION_W027)
	OEMNV_Get(NV_SIM_SELECT_I,&SimChoice);
	if(SimChoice.sim_select==AVK_SEND_TWO)
	{
		 if((((wParam == AVK_SEND|| wParam == AVK_CAMERA) && (pMe->m_anykey_answer & 0x4))
        ||(eCode == EVT_FLIP && ((boolean)wParam == TRUE)  && (pMe->m_anykey_answer & 0x2))
        ||(((wParam == AVK_USER_HEADSET) || (wParam == AVK_SEND )|| (wParam == AVK_CAMERA) || (wParam == AVK_MUSIC)) && (pMe->m_anykey_answer & 0x8))
        ||(((((AVK_FIRST < wParam && wParam <AVK_POWER ) ||(wParam == AVK_INFO)||(wParam == AVK_SHIFT)||
        	   (wParam == AVK_ENTER)||(wParam == AVK_CAPLK)||(wParam == AVK_SYMBOL)||
        	   (wParam == AVK_RWD)||(wParam == AVK_LCTRL)||(wParam == AVK_SPACE)||
        	   (AVK_A <= wParam && wParam <= AVK_Z) ||(AVK_CLR < wParam && wParam <AVK_SOFT1 ))
                 && !bKeyguardEnabled)||(
                 #ifndef FEATURE_VERSION_C01
                 wParam == AVK_SEND ||
                 #endif
                 wParam == AVK_CAMERA || wParam == AVK_MUSIC))
                 && (pMe->m_anykey_answer & 0x1))
        ) ||auto_answer ||wParam == AVK_SELECT)
    	{
#ifdef FEATURE_ICM
        if(AEE_SUCCESS != ICM_GetCallInfo(pMe->m_pICM, pMe->m_CallsTable->call_id, &ci, sizeof(AEECMCallInfo)))
#else
        if(AEE_SUCCESS != ITELEPHONE_GetCallInfo(pMe->m_pITelephone, pMe->m_CallsTable->call_id, &ci, sizeof(AEETCallInfo)))
#endif
        {
            return FALSE;
        }

        IALERT_StopAlerting(pMe->m_pAlert);
        switch (ci.call_state)
        {
#ifdef FEATURE_ICM
            case AEECM_CALL_STATE_INCOM:

#ifdef FEATURE_IS2000_SCC_CODES
                pMe->m_bAnswerHold = bAnswerHold;
#endif /* FEATURE_IS2000_SCC_CODES */
                ICM_AnswerCall(pMe->m_pICM, pMe->m_CallsTable->call_id);
                CLOSE_DIALOG(DLGRET_CONNECT)
                break;
#else
            case AEET_CALL_STATE_INCOM:
			{
				ICall *pCall= NULL;
#ifdef FEATURE_IS2000_SCC_CODES
                pMe->m_bAnswerHold = bAnswerHold;
#endif /* FEATURE_IS2000_SCC_CODES */
				ICALLMGR_GetCall(pMe->m_pICallMgr,pMe->m_CallsTable->call_id,&pCall);
				if (pCall != NULL)
				{
					ICALL_Answer(pCall);
					ICALL_Release(pCall);
				} 

                //ICM_AnswerCall(pMe->m_pITelephone, pMe->m_CallsTable->call_id);
                CLOSE_DIALOG(DLGRET_CONNECT)
            }break;
#endif
#ifdef FEATURE_ICM
            case AEECM_CALL_STATE_CONV:
#else
            case AEET_CALL_STATE_CONV:
#endif
#ifdef FEATURE_IS2000_SCC_CODES
                pMe->m_bAnswerHold = bAnswerHold;
#endif /* FEATURE_IS2000_SCC_CODES */
                CallApp_AnswerInbandCall(pMe);
                CLOSE_DIALOG(DLGRET_CONNECT)
                break;

            default:
                return FALSE;
       }
       CallApp_Change_Call_Table_Call_Start_Time(pMe,pMe->m_CallsTable->call_number);
       CallApp_Change_Call_Table_Call_History_State(pMe,pMe->m_CallsTable->call_number,AEECALLHISTORY_CALL_TYPE_FROM/*CALLHISTORY_INCOMING*/);
       return TRUE;
    	}
	}
	else
	#endif
	{
    if((((wParam == AVK_SEND/*|| wParam == AVK_CAMERA*/) && (pMe->m_anykey_answer & 0x4))
        ||(eCode == EVT_FLIP && ((boolean)wParam == TRUE)  && (pMe->m_anykey_answer & 0x2))
        ||(((wParam == AVK_USER_HEADSET) || (wParam == AVK_SEND )/*|| (wParam == AVK_CAMERA) || (wParam == AVK_MUSIC)*/) && (pMe->m_anykey_answer & 0x8))
        ||(((((AVK_FIRST < wParam && wParam <AVK_POWER ) ||(wParam == AVK_INFO)||(wParam == AVK_SHIFT)||
        	   (wParam == AVK_ENTER)||(wParam == AVK_CAPLK)||(wParam == AVK_SYMBOL)||
        	   (wParam == AVK_RWD)||(wParam == AVK_LCTRL)||(wParam == AVK_SPACE)||
        	   (AVK_A <= wParam && wParam <= AVK_Z) ||(AVK_CLR < wParam && wParam <AVK_SOFT1 ))
                 && !bKeyguardEnabled)
                 ||(wParam == AVK_SEND 
                  #if defined(FEATURE_VERSION_1110W516) || defined(FEATURE_VERSION_W027)||defined(FEATURE_VERSION_K202_LM129C)//xxzhen
                 || wParam == AVK_CAMERA || wParam == AVK_MUSIC
                 #endif
                 ))
                 && (pMe->m_anykey_answer & 0x1))
        ) ||auto_answer ||wParam == AVK_SELECT)
    {
#ifdef FEATURE_ICM
        if(AEE_SUCCESS != ICM_GetCallInfo(pMe->m_pICM, pMe->m_CallsTable->call_id, &ci, sizeof(AEECMCallInfo)))
#else
        if(AEE_SUCCESS != ITELEPHONE_GetCallInfo(pMe->m_pITelephone, pMe->m_CallsTable->call_id, &ci, sizeof(AEETCallInfo)))
#endif
        {
            return FALSE;
        }

        IALERT_StopAlerting(pMe->m_pAlert);
        switch (ci.call_state)
        {
#ifdef FEATURE_ICM
            case AEECM_CALL_STATE_INCOM:

#ifdef FEATURE_IS2000_SCC_CODES
                pMe->m_bAnswerHold = bAnswerHold;
#endif /* FEATURE_IS2000_SCC_CODES */
                ICM_AnswerCall(pMe->m_pICM, pMe->m_CallsTable->call_id);
                CLOSE_DIALOG(DLGRET_CONNECT)
                break;
#else
            case AEET_CALL_STATE_INCOM:
			{
				ICall *pCall= NULL;
#ifdef FEATURE_IS2000_SCC_CODES
                pMe->m_bAnswerHold = bAnswerHold;
#endif /* FEATURE_IS2000_SCC_CODES */
				ICALLMGR_GetCall(pMe->m_pICallMgr,pMe->m_CallsTable->call_id,&pCall);
				if (pCall != NULL)
				{
					ICALL_Answer(pCall);
					ICALL_Release(pCall);
				} 

                //ICM_AnswerCall(pMe->m_pITelephone, pMe->m_CallsTable->call_id);
                CLOSE_DIALOG(DLGRET_CONNECT)
            }break;
#endif
#ifdef FEATURE_ICM
            case AEECM_CALL_STATE_CONV:
#else
            case AEET_CALL_STATE_CONV:
#endif
#ifdef FEATURE_IS2000_SCC_CODES
                pMe->m_bAnswerHold = bAnswerHold;
#endif /* FEATURE_IS2000_SCC_CODES */
                CallApp_AnswerInbandCall(pMe);
                CLOSE_DIALOG(DLGRET_CONNECT)
                break;

            default:
                return FALSE;
       }

       CallApp_Change_Call_Table_Call_Start_Time(pMe,pMe->m_CallsTable->call_number);
       CallApp_Change_Call_Table_Call_History_State(pMe,pMe->m_CallsTable->call_number,AEECALLHISTORY_CALL_TYPE_FROM/*CALLHISTORY_INCOMING*/);
	   
       return TRUE;
    }
    }
    //#endif
    return FALSE;
}

/*=============================================================================
FUNCTION:  CallApp_AnswerInbandCall

DESCRIPTION: Answers an in-band call.

PARAMETERS:
   *pMe: CCallApp object pointer

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void CallApp_AnswerInbandCall(CCallApp *pMe)
{
#ifndef FEATURE_ICM
	ICall *pCall = NULL;
#endif
    CALL_FUN_START("CallApp_AnswerInbandCall", 0, 0, 0);

    if(pMe->m_CallsTable)
    {
        pMe->m_Is3Way = TRUE;
        //add for call waitting number display	 in missed record
        //in-band呼叫已经接起,不要记录未接号码
        pMe->m_bmissCallWaiting = FALSE;
        // Clear any remaining DTMF digits
        pMe->m_PauseString[0] = 0;
#ifdef FEATURE_ICM
        // Send empty flash message to answer the call
        ICM_AnswerCall(pMe->m_pICM, pMe->m_CallsTable->call_id);
#else
		ICALLMGR_GetCall(pMe->m_pICallMgr,pMe->m_CallsTable->call_id,&pCall);
		if (pCall != NULL)
		{
		    ICALL_Answer(pCall);
		    ICALL_Release(pCall);
		} 
#endif
        //make the newer name & num string are displayed.
        (void) ISHELL_PostEvent(pMe->m_pShell,AEECLSID_DIALER/*AEECLSID_CALL*/,EVT_USER_REDRAW,0, 0);
    }
}
/*=============================================================================
FUNCTION: CallApp_BuildDialerMenu

DESCRIPTION: setup Bar in the dialer Dialog

PARAMETERS:
   *pMe: CCallApp object pointer

RETURN VALUE:
    None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void CallApp_Build_NumEdit_Option_Menu(CCallApp *pMe,IMenuCtl   *pMenuCtl )
{
    //AEERect     menurc;
    //IMenuCtl *pSKMenu = pMenuCtl;
    MSG_FATAL("CallApp_BuildDialerMenu Start",0,0,0);

    ASSERT(IDD_NUMEDIT== pMe->m_pActiveDlgID);

    IMENUCTL_DeleteAll(pMenuCtl);
    if(pMe->m_b_incall) //在此判断绘制哪个option
    {
        if (pMe->m_CallMuted)
        {
            (void) IMENUCTL_AddItem(pMenuCtl,AEE_APPSCALLAPP_RES_FILE, IDS_UNMUTE, IDS_UNMUTE,
                                                    (AECHAR*)NULL, (uint32)NULL);
        }
        else
        {
            (void) IMENUCTL_AddItem(pMenuCtl,AEE_APPSCALLAPP_RES_FILE, IDS_MUTE, IDS_MUTE,
                                                    (AECHAR*)NULL, (uint32)NULL);
        }
#ifdef FEATURE_SUPPORT_BT_APP
        if(pMe->m_b_add_btag_menu ==TRUE/* && pMe->m_bt_audio == SEND_NONE*/)
        {
            if(pMe->m_bt_audio == SEND_NONE)
            {
                (void) IMENUCTL_AddItem(pMenuCtl,AEE_APPSCALLAPP_RES_FILE,IDS_USE_BT_AG,IDS_USE_BT_AG,
                                                (AECHAR*)NULL,(uint32)NULL);
            }
            else
            {
                (void) IMENUCTL_AddItem(pMenuCtl,AEE_APPSCALLAPP_RES_FILE,IDS_USE_PHONE,IDS_USE_PHONE,
                                                (AECHAR*)NULL,(uint32)NULL);
            }
        }
#endif

//Add By zzg 2011_10_27
#ifdef FEATURE_SUPPORT_BT_AUDIO		
		MSG_FATAL("***zzg CallApp_Build_NumEdit m_bBtAvailable=%x***", pMe->m_bBtAvailable, 0, 0);

		if (TRUE == pMe->m_bBtAvailable)
		{
			if(CallApp_CheckBtHeadSetInUsing(pMe))
			{
				(void) IMENUCTL_AddItem(pMenuCtl,AEE_APPSCALLAPP_RES_FILE,IDS_USE_PHONE,IDS_USE_PHONE,
												(AECHAR*)NULL,(uint32)NULL);				
			}
			else
			{
				(void) IMENUCTL_AddItem(pMenuCtl,AEE_APPSCALLAPP_RES_FILE,IDS_USE_BT_AG,IDS_USE_BT_AG,
												(AECHAR*)NULL,(uint32)NULL);
			}
		}
#endif
//Add End



        if ( pMe->m_bHandFree )
        {
            (void) IMENUCTL_AddItem(pMenuCtl,AEE_APPSCALLAPP_RES_FILE, IDS_NONHANDFREE, IDS_NONHANDFREE,
                                                    (AECHAR*)NULL, (uint32)NULL);
        }
        else
        {
            (void) IMENUCTL_AddItem(pMenuCtl,AEE_APPSCALLAPP_RES_FILE, IDS_HANDFREE, IDS_HANDFREE,
                                                    (AECHAR*)NULL, (uint32)NULL);
        }

        if ((pMe->m_PauseString[0] == 0)
            || (pMe->idle_info.uimLocked == TRUE)
#ifdef FEATURE_IS2000_SCC_CODES
            || (pMe->m_bAnswerHold == FALSE)
#endif  //FEATURE_IS2000_SCC_CODES
            )
        {
#ifdef FEATURE_APP_PAUSE_TIMER
            if (pMe->m_TimerString[0] != 0)
            {
                //return TRUE;
            }
            else
#endif //FEATURE_APP_PAUSE_TIMER
            {
#if defined(FEATURE_VERSION_W208S)||defined(FEATURE_VERSION_1110W516) ||defined(FEATURE_VERSION_C180) ||defined(FEATURE_VERSION_W027V3)|| defined(FEATURE_VERSION_H1201)|| defined(FEATURE_VERSION_W027)
#else
                if(!pMe->m_call_info.b_emerg)
                {
                   (void) IMENUCTL_AddItem(pMenuCtl,AEE_APPSCALLAPP_RES_FILE,IDS_SK_MENU,IDS_SK_MENU,
                                                       (AECHAR*)NULL,(uint32)NULL);
                }
#endif				
            }
        }

        if(!pMe->m_call_info.b_emerg)
        {
           IMENUCTL_AddItem(pMenuCtl, AEE_APPSCALLAPP_RES_FILE, IDS_SAVE_NEW, IDS_SAVE_NEW, NULL, 0);
        }
    }
    else
    {
#ifdef FEATURE_SUPPORT_WAP_APP
#ifdef FEATURE_CARRIER_CHINA_TELCOM
        (void) IMENUCTL_AddItem(pMenuCtl,AEE_APPSCALLAPP_RES_FILE, IDS_LINK, IDS_LINK, NULL, NULL);
#endif
#endif
        (void) IMENUCTL_AddItem(pMenuCtl,AEE_APPSCALLAPP_RES_FILE, IDS_SAVE_NEW, IDS_SAVE_NEW, NULL, NULL);
        (void) IMENUCTL_AddItem(pMenuCtl,AEE_APPSCALLAPP_RES_FILE, IDS_SAVE_TO_CONT, IDS_SAVE_TO_CONT, NULL, NULL);
        (void) IMENUCTL_AddItem(pMenuCtl,AEE_APPSCALLAPP_RES_FILE, IDS_SEND_MESSAGE, IDS_SEND_MESSAGE, NULL, NULL);
        (void) IMENUCTL_AddItem(pMenuCtl,AEE_APPSCALLAPP_RES_FILE, IDS_CALL, IDS_CALL, NULL, NULL);
    }

#if defined( FEATURE_CALL_RECORDER)
    if( pMe->m_b_incall)
    {
        CallApp_build_recorder_option_menu( pMe, pMenuCtl); 
    }
#endif
    
    //if (IMENUCTL_GetItemCount(pMenuCtl) == 0)
    //{
    //    return;
    //}
    

    //IDISPLAY_EraseRect(pMe->m_pDisplay, &pMe->m_rc);
    ISHELL_CancelTimer(pMe->m_pShell,
                                                    (PFNNOTIFY)CallApp_HandleDialogTimer_Redraw,
                                                    pMe);
    IMENUCTL_SetPopMenuRect(pMenuCtl);
    IMENUCTL_SetProperties ( pMenuCtl, MP_UNDERLINE_TITLE|MP_WRAPSCROLL |MP_BIND_ITEM_TO_NUMBER_KEY );
    IMENUCTL_SetBottomBarType(pMenuCtl,BTBAR_SELECT_BACK);
    IMENUCTL_SetActive ( pMenuCtl, TRUE );
    (void) IMENUCTL_Redraw ( pMenuCtl );
    MSG_FATAL("CallApp_BuildDialerMenu End",0,0,0);
}

/*=============================================================================
FUNCTION: CallApp_BuildConvMenu

DESCRIPTION: Used by the conversation dialog ,Build the Pop menu.

PARAMETERS:
   *pMe: CCallApp object pointer

RETURN VALUE:
    None

COMMENTS:

=============================================================================*/
static void CallApp_Build_Connect_Option_Menu(CCallApp *pMe)
{
    //AEERect   rc_softkey;
    IMenuCtl   *pSKMenu;
    ASSERT(IDD_CONVERSATION == pMe->m_pActiveDlgID);

    pSKMenu = pMe->m_pMenu;

#ifdef FEATURE_SUPPORT_BT_APP
    MSG_FATAL("***zzg CallApp_Build_Connect_Option_Menu m_b_add_btag_menu=%x***", pMe->m_b_add_btag_menu, 0, 0);
#endif

#ifdef FEATURE_SUPPORT_BT_AUDIO
	MSG_FATAL("***zzg CallApp_Build_Connect_Option_Menu m_bBtAvailable=%x***", pMe->m_bBtAvailable, 0, 0);
#endif

    (void) IMENUCTL_DeleteAll(pSKMenu);

    if (pMe->m_PauseString[0] != 0)
    {
        (void) IMENUCTL_AddItem(pSKMenu, AEE_APPSCALLAPP_RES_FILE,IDS_RELEASE,IDS_RELEASE,
                                                (AECHAR*)NULL,(uint32)NULL);
    }

#ifdef FEATURE_IS2000_SCC_CODES
    if (pMe->m_bAnswerHold)
    {
        (void) IMENUCTL_AddItem(pSKMenu, AEE_APPSCALLAPP_RES_FILE, IDS_RELEASEHOLD,IDS_RELEASEHOLD,
                                                (AECHAR*)NULL, (uint32)NULL);
    }
#endif /* FEATURE_IS2000_SCC_CODES */
    if (pMe->m_CallMuted)
    {
    	#ifdef FEATURE_VERSION_C316
		(void) IMENUCTL_AddItem(pSKMenu,AEE_APPSCALLAPP_RES_FILE,IDS_MUTE_OFF,IDS_MUTE_OFF,
                                                (AECHAR*)NULL,(uint32)NULL);
		#else
        (void) IMENUCTL_AddItem(pSKMenu,AEE_APPSCALLAPP_RES_FILE,IDS_UNMUTE,IDS_UNMUTE,
                                                (AECHAR*)NULL,(uint32)NULL);
		#endif
    }
    else
    {
    	#ifdef FEATURE_VERSION_C316
		(void) IMENUCTL_AddItem(pSKMenu,AEE_APPSCALLAPP_RES_FILE,IDS_MUTE_ON,IDS_MUTE_ON,
                                                (AECHAR*)NULL,(uint32)NULL);
		#else
        (void) IMENUCTL_AddItem(pSKMenu,AEE_APPSCALLAPP_RES_FILE,IDS_MUTE,IDS_MUTE,
                                                (AECHAR*)NULL,(uint32)NULL);
		#endif
    }
#ifdef FEATURE_SUPPORT_BT_APP
    if(pMe->m_b_add_btag_menu ==TRUE/* && pMe->m_bt_audio == SEND_NONE*/)
    {
        if(pMe->m_bt_audio == SEND_NONE)
        {
            (void) IMENUCTL_AddItem(pSKMenu,AEE_APPSCALLAPP_RES_FILE,IDS_USE_BT_AG,IDS_USE_BT_AG,
                                            (AECHAR*)NULL,(uint32)NULL);
        }
        else
        {
            (void) IMENUCTL_AddItem(pSKMenu,AEE_APPSCALLAPP_RES_FILE,IDS_USE_PHONE,IDS_USE_PHONE,
                                            (AECHAR*)NULL,(uint32)NULL);
        }
    }
#endif

//Add By zzg 2011_10_27
#ifdef FEATURE_SUPPORT_BT_AUDIO
		//if(pMe->m_b_add_btag_menu ==TRUE)

		MSG_FATAL("***zzg CallApp_Build_Connect m_bBtAvailable=%x***", pMe->m_bBtAvailable, 0, 0);

		//if (TRUE == pMe->m_bBtAvailable)
		{
			if(CallApp_CheckBtHeadSetInUsing(pMe))			
			{
				(void) IMENUCTL_AddItem(pSKMenu,AEE_APPSCALLAPP_RES_FILE,IDS_USE_PHONE,IDS_USE_PHONE,
												(AECHAR*)NULL,(uint32)NULL);				
			}
			else
			{
				(void) IMENUCTL_AddItem(pSKMenu,AEE_APPSCALLAPP_RES_FILE,IDS_USE_BT_AG,IDS_USE_BT_AG,
												(AECHAR*)NULL,(uint32)NULL);
			}
		}
#endif
//Add End


    //if ( pMe->m_bHandFree )
    //{
    //    (void) IMENUCTL_AddItem(pSKMenu,AEE_APPSCALLAPP_RES_FILE,IDS_NONHANDFREE,IDS_NONHANDFREE,
    //                                            (AECHAR*)NULL,(uint32)NULL);
    //}
    //else
    //{
    //    (void) IMENUCTL_AddItem(pSKMenu,AEE_APPSCALLAPP_RES_FILE,IDS_HANDFREE,IDS_HANDFREE,
    //                                            (AECHAR*)NULL,(uint32)NULL);
    //}
#if 0    
    (void) IMENUCTL_AddItem(pSKMenu,AEE_APPSCALLAPP_RES_FILE,IDS_RECORD,IDS_RECORD,
                                                (AECHAR*)NULL,(uint32)NULL);
    if ((pMe->m_PauseString[0] == 0)
        || (pMe->idle_info.uimLocked == TRUE)
#ifdef FEATURE_IS2000_SCC_CODES
        || (pMe->m_bAnswerHold == FALSE)
#endif  //FEATURE_IS2000_SCC_CODES
        )
    {
#ifdef FEATURE_APP_PAUSE_TIMER
        if (pMe->m_TimerString[0] != 0)
        {
            //return TRUE;
        }
        else
#endif //FEATURE_APP_PAUSE_TIMER
        {
         if(!pMe->m_call_info.b_emerg)
         {
            (void) IMENUCTL_AddItem(pSKMenu,AEE_APPSCALLAPP_RES_FILE,IDS_SK_MENU,IDS_SK_MENU,
                                                (AECHAR*)NULL,(uint32)NULL);
         }
        }
    }
#endif
#if defined( FEATURE_CALL_RECORDER)
    CallApp_build_recorder_option_menu( pMe, pSKMenu);
#endif
	#ifdef FEATURE_VERSION_C316
	(void) IMENUCTL_AddItem(pSKMenu,AEE_APPSCALLAPP_RES_FILE,IDS_PHONE_BOOKS,IDS_PHONE_BOOKS,
                                            (AECHAR*)NULL,(uint32)NULL);
	#else
    (void) IMENUCTL_AddItem(pSKMenu,AEE_APPSCALLAPP_RES_FILE,IDS_CONTACT,IDS_CONTACT,
                                            (AECHAR*)NULL,(uint32)NULL);
    #endif
    (void) IMENUCTL_AddItem(pSKMenu,AEE_APPSCALLAPP_RES_FILE,IDS_SMS,IDS_SMS,
                                            (AECHAR*)NULL,(uint32)NULL);


    //IMENUCTL_SetRect ( pSKMenu , &pMe->m_rc);
    //IDISPLAY_EraseRect(pMe->m_pDisplay, &pMe->m_rc);
    ISHELL_CancelTimer(pMe->m_pShell,
                                                    (PFNNOTIFY)CallApp_HandleDialogTimer_Redraw,
                                                    pMe);
    IMENUCTL_SetPopMenuRect(pSKMenu);
    IMENUCTL_SetProperties ( pSKMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL |MP_BIND_ITEM_TO_NUMBER_KEY );
    IMENUCTL_SetBottomBarType(pSKMenu,BTBAR_SELECT_BACK);
    IMENUCTL_SetActive ( pSKMenu, TRUE );

    (void) IMENUCTL_Redraw ( pSKMenu );
}

/*=============================================================================
FUNCTION: CallApp_Draw_Connect_Time

DESCRIPTION:通话阶段屏幕显示

PARAMETERS:


RETURN VALUE:
    None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void CallApp_Draw_Connect_Time(void *pUser)
{
    register CCallApp     *pMe = (CCallApp *)pUser;
    ITimeCtl       *pTimerCtl =NULL;
    AEERect        rect;
    //AEERect        rect_record;
    //int            bOffset;
    int            bWidth;
    int            tempWidth;	
    AECHAR         szText[40];
#ifdef FEATURE_LANG_BIDI
    AECHAR         szTextTmp[100];
#endif
    Dialer_call_table * temp = NULL;
    MSG_FATAL("CallApp_Draw_Connect_Time",0,0,0);
    if(pMe == NULL)
    {
        //CALL_ERR("CallApp_Draw_Connect_Time pMe == NULL",0,0,0);
        return;
    }

    (void) ISHELL_CancelTimer(pMe->m_pShell, CallApp_Draw_Connect_Time, pMe);

    if(pMe->m_CallsTable == NULL)
    {
        CALL_ERR("CallApp_Draw_Connect_Time m_CallsTable == NULL",0,0,0);
        return;
    }

    temp = CallApp_Get_First_Entry_In_Call_Table(pMe);

    if(temp == NULL)
    {
        //CALL_ERR("CallApp_Draw_Connect_Time temp == NULL",0,0,0);
        return;
    }

    (void) ISHELL_SetTimer(pMe->m_pShell, 1000,CallApp_Draw_Connect_Time, pMe);

    if ( pMe->m_bShowPopMenu )
    {
        return;
    }

    pTimerCtl = (ITimeCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDDC_CONVERSATION_TIMER);

    if(pTimerCtl == NULL)
    {
        //CALL_ERR("pTimerCtl == NULL",0,0,0);
        return;
    }
    //
    // TIMER TEXT
    //
#ifdef FEATURE_VERSION_W317A
	SETAEERECT(&rect,
                0,
                (SCREEN_HEIGHT-CALL_LINE_HIGHT)/2,
                CALL_TEXT_DX+4,
                CALL_LINE_HIGHT);
#else
    SETAEERECT(&rect,
                0,
                CALL_FIRST_LINE_Y,
                CALL_TEXT_DX+4,
                CALL_LINE_HIGHT);
#endif
    
    Appscommon_ResetBackgroundEx(pMe->m_pDisplay, &rect, TRUE);	

#ifdef FEATURE_VERSION_W317A
	SETAEERECT(&rect,
                CALL_TEXT_X,
                (SCREEN_HEIGHT-CALL_LINE_HIGHT)/2,
                CALL_TEXT_DX,
                CALL_LINE_HIGHT);
#else
    SETAEERECT(&rect,
                CALL_TEXT_X,
                CALL_FIRST_LINE_Y,
                CALL_TEXT_DX,
                CALL_LINE_HIGHT);
#endif

#ifndef FEATURE_LANG_BIDI
    (void) ISHELL_LoadResString(pMe->m_pShell,
                                            AEE_APPSCALLAPP_RES_FILE,
                                            IDS_TIME_TEXT,//Time
                                            szText,
                                            sizeof(szText));

    // Position time text on first line
    bWidth = IDISPLAY_MeasureText(pMe->m_pDisplay, AEE_FONT_NORMAL,szText);
    IDisplay_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, CALLAPP_TEXT_COLOR);
    
    (void)IDISPLAY_DrawText(pMe->m_pDisplay,
                            AEE_FONT_NORMAL,
                            szText,
                            -1,
                            rect.x,
                            rect.y,//0,
                            &rect,
                            IDF_ALIGN_LEFT|IDF_TEXT_TRANSPARENT);
        
    IDisplay_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, RGB_BLACK);

    // Position call timer on first line next to TIME text

    // Offset for Timer is width of Time Text
    //bOffset = bWidth;// + 16;

#ifdef FEATURE_VERSION_W317A	
	SETAEERECT(&rect,
	            CALL_NAME_X, //CALL_TEXT_X +bWidth +4 ,
	            (SCREEN_HEIGHT-CALL_LINE_HIGHT)/2,
	            CALL_TEXT_DX - bWidth -4,
	            CALL_LINE_HIGHT);
#else
    SETAEERECT(&rect,
	            CALL_NAME_X, //CALL_TEXT_X +bWidth +4 ,
	            CALL_FIRST_LINE_Y,
	            CALL_TEXT_DX - bWidth -4,
	            CALL_LINE_HIGHT);
#endif

    //IDISPLAY_EraseRect(pMe->m_pDisplay, &rect);
    ITIMECTL_GetTimeString(pTimerCtl,
                                            (uint32)(ISHELL_GetUpTimeMS(pMe->m_pShell) -
                                            temp->start_time),
                                            szText,
                                            sizeof(szText),
                                            GTS_SECS);
    
    IDisplay_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, CALLAPP_TEXT_COLOR);
    
	(void)IDISPLAY_DrawText(pMe->m_pDisplay,
                            AEE_FONT_NORMAL,
                            szText,
                            -1,
                            rect.x,
                            rect.y,//0,
                            &rect,
                            IDF_TEXT_TRANSPARENT|IDF_ALIGN_LEFT);
    IDisplay_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, RGB_BLACK);

#if defined(FEATURE_VERSION_C337)   
    MSG_FATAL("CallApp_Draw_Connect_Time m_isIncoming=%d",pMe->m_isIncoming,0,0);
    if(!pMe->m_isIncoming)
    {
        (void) ISHELL_LoadResString(pMe->m_pShell,
                                    AEE_APPSCALLAPP_RES_FILE,
                                    IDS_CALLING_DIALOG_TEXT,//Time
                                    szText,
                                    sizeof(szText));		
	 bWidth = IDISPLAY_MeasureText(pMe->m_pDisplay, AEE_FONT_NORMAL,szText);
        SETAEERECT(&rect,
                                    CALL_NAME_X,
                                    CALL_SECOND_UP_LINE_Y,
                                    bWidth,
                                    CALL_LINE_HIGHT);   	
		Appscommon_ResetBackgroundEx(pMe->m_pDisplay, &rect, TRUE);     //add by pyuangui 2013-01-17
        IDisplay_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, CALLAPP_TEXT_COLOR);
        
        (void)IDISPLAY_DrawText(pMe->m_pDisplay,
                    AEE_FONT_SMALL,
                    szText,
                    -1,
                    rect.x,
                    rect.y,//0,
                    &rect,
                    IDF_TEXT_TRANSPARENT|IDF_ALIGN_LEFT);
        IDisplay_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, RGB_BLACK);
    }
#endif


#else  // FEATURE_LANG_BIDI
    MEMSET(szText, 0, sizeof(szText));
    MEMSET(szTextTmp, 0, sizeof(szTextTmp));
    (void) ISHELL_LoadResString(pMe->m_pShell,
                                            AEE_APPSCALLAPP_RES_FILE,
                                            IDS_TIME_TEXT,
                                            szText,
                                            sizeof(szText));
    WSTRLCPY(szTextTmp, szText ,100);

    MEMSET(szText, 0, sizeof(szText));
    ITIMECTL_GetTimeString(pTimerCtl,
                                            (uint32)(ISHELL_GetUpTimeMS(pMe->m_pShell) -
                                            temp->start_time),
                                            szText,
                                            sizeof(szText),
                                            GTS_SECS);

    WSTRLCAT(szTextTmp, szText ,100);
    SETAEERECT(&rect,
                                            CALL_TEXT_X,
                                            CALL_FIRST_LINE_Y,
                                            CALL_TEXT_DX,
                                            CALL_LINE_HIGHT);
    //IDISPLAY_EraseRect(pMe->m_pDisplay, &rect);
    IDisplay_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, CALLAPP_TEXT_COLOR);
    (void)IDISPLAY_DrawText(pMe->m_pDisplay,
                                            AEE_FONT_NORMAL,
                                            szTextTmp,
                                            -1,
                                            CALL_TEXT_X,
                                            CALL_FIRST_LINE_Y,//0,
                                            &rect,
                                            IDF_TEXT_TRANSPARENT);
    IDisplay_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, RGB_BLACK);
#endif //FEATURE_LANG_BIDI

    if ( pMe->m_bRefreshVol == REFRESHVOL_ON )
    {
        pMe->m_bRefreshVol = REFRESHVOL_WAIT;// wait for one second
    }
    else if ( pMe->m_bRefreshVol == REFRESHVOL_WAIT )
    {
        pMe->m_bRefreshVol = REFRESHVOL_OFF;
        CallApp_Draw_Connect_Number_and_Name(pMe);
    }

    ITIMECTL_SetActive(pTimerCtl, FALSE);

    rect.y = 2000;
    ITIMECTL_SetRect(pTimerCtl,&rect);//to hide the timectl
    IDISPLAY_Update(pMe->m_pDisplay);
    
#ifdef FEATURE_OEMOMH
    if (!IRUIM_IsCardConnected(pMe->m_pIRUIM)) 
    {
        MSG_FATAL("IRUIM_IsCardConnected FALSE",0,0,0);
        pMe->m_userCanceled = TRUE;
#ifdef FEATURE_ICM
        ICM_EndAllCalls(pMe->m_pICM);
#else
        ICALLMGR_EndAllCalls(pMe->m_pICallMgr);
#endif
    }
#endif
    
#ifdef FEATURE_SUB_LCD
    {
        CallApp_DispalySub_Image(pMe);
        SETAEERECT(&rect,
                                                0,
                                                0,
                                                96,
                                                108);
        ITIMECTL_GetTimeString(pTimerCtl,
                                                (int32)(ISHELL_GetUpTimeMS(pMe->m_pShell) -
                                                temp->start_time/*pMe->m_ConvStartTime*/),
                                                szText,
                                                sizeof(szText),
                                                GTS_SECS);
        (void)IDISPLAYSUB_DrawText(pMe->m_pDisplaySub,
                                                AEE_FONT_NORMAL,
                                                szText,
                                                -1,
                                                0,
                                                0,
                                                &rect,
                                                IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE | IDF_TEXT_TRANSPARENT);
        SETAEERECT(&rect,
                                                0,
                                                0,
                                                96,
                                                108);
                                                (void) ISHELL_LoadResString(pMe->m_pShell,
                                                AEE_APPSCALLAPP_RES_FILE,
                                                IDS_TIME_TEXT,
                                                szText,
                                                sizeof(szText));
        ITIMECTL_SetActive(pTimerCtl, FALSE);
                                                (void)IDISPLAYSUB_DrawText(pMe->m_pDisplaySub,
                                                AEE_FONT_NORMAL,
                                                szText,
                                                -1,
                                                0,
                                                0,
                                                &rect,
                                                IDF_ALIGN_LEFT | IDF_ALIGN_TOP | IDF_TEXT_TRANSPARENT);
        IDISPLAYSUB_Update(pMe->m_pDisplaySub);
    }
#endif//FEATURE_SUB_LCD
}
/*=============================================================================
FUNCTION: CallApp_PlayToneList

DESCRIPTION: Play a list of DTMF tones to the user.

PARAMETERS:
   *pMe: CCallApp object pointer
   *digits: list of ASCII digits to play
    len: number of digits

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void CallApp_PlayToneList(CCallApp *pMe,
                               char       *digits,
                               byte        len)
{
    int i;
    static AEESoundInfo    si =
    {
       AEE_SOUND_DEVICE_CURRENT,
       AEE_SOUND_METHOD_BEEP,
       AEE_SOUND_APATH_LOCAL,
       AEE_SOUND_MUTECTL_UNMUTED,
       AEE_SOUND_MUTECTL_UNMUTED,
    };
    CALL_FUN_START("CallApp_PlayToneList",0,0,0);
    if (0 == len)
    {
        return;
    }

    if (pMe->m_nToneCount != 0)
    {
         (void)ISHELL_CancelTimer(pMe->m_pShell, CallApp_NextTonePlay, pMe);
         OEMSound_StopTone(FALSE, NULL);
    }
    FREEIF(pMe->m_toneData);
    pMe->m_toneData = (AEESoundToneData*)MALLOC(sizeof(AEESoundToneData) *( len+1));
    if(pMe->m_toneData == NULL)
    {
        return;
    }
    // Start with a silence to allow the keypress sound to complete
    // before starting the tone list.
    pMe->m_toneData[0].wDuration = PAUSE_TONE_DURATION;
    pMe->m_toneData[0].eTone = AEE_TONE_SILENCE;

    for (i = 0; i < len; i++)
    {
        pMe->m_toneData[i+1].wDuration = PAUSE_TONE_DURATION;
        switch (digits[i])
        {
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                pMe->m_toneData[i+1].eTone =
                      (AEESoundTone) ( (int) AEE_TONE_1 + (digits[i] - '1') );
                break;
            case '0':
                pMe->m_toneData[i+1].eTone = AEE_TONE_0;
                break;
            case '#':
                pMe->m_toneData[i+1].eTone = AEE_TONE_POUND;
                break;
            case '*':
                pMe->m_toneData[i+1].eTone = AEE_TONE_STAR;
                break;
            default:
                ASSERT_NOT_REACHABLE;
                break;
        }
    }

    pMe->m_nToneCount = len + 1;
    pMe->m_nToneCurrIndex = 0;

    OEMSound_PlayTone(&si, &(pMe->m_toneData[pMe->m_nToneCurrIndex]), NULL);
    (void)ISHELL_SetTimer(pMe->m_pShell, pMe->m_toneData[0].wDuration,
                                                CallApp_NextTonePlay, pMe);
}


/*=============================================================================
FUNCTION: CallApp_SetConvPhoneNumber

DESCRIPTION: Used by the Conversation dialog to set the phone number that
             is displayed.

PARAMETERS:
   *pMe: CCallApp object pointer
   *pNumberCtl: TextCtl to use

RETURN VALUE:
    None

COMMENTS:
   The current value of CCallApp::m_CallString is used as the phone number

SIDE EFFECTS:

SEE ALSO:
   CallApp_ConvHandleEvent()
=============================================================================*/
static void CallApp_Draw_Connect_Number_and_Name(CCallApp *pMe)
{
    AEERect rect;
    boolean b_cdg = FALSE;
    RGBVAL oldColor = 0;
    
    MSG_FATAL("CallApp_Draw_Connect_Number_and_Name pMe->m_b_incoming=%d", pMe->m_b_incoming, 0, 0);
    //clear the name and number area 2 to 4
    SETAEERECT(&rect,
                                            0,
                                            CALL_SECOND_LINE_Y,
                                            CALL_TEXT_DX + 4,
                                            3*CALL_LINE_HIGHT);
    
    Appscommon_ResetBackgroundEx(pMe->m_pDisplay, &rect, TRUE);

    if(pMe->m_cdg_dsp_info.m_b_show_cdg)
    {
        CallApp_NotifyDisp(pMe);
        b_cdg = TRUE ;
    }
#ifndef FEATURE_USES_LOWMEM
    //pMe->m_pConvImage
    if(pMe->m_pConvImage == NULL)
    {
        if(pMe->m_bHandFree)
        {
            pMe->m_pConvImage = ISHELL_LoadResImage( pMe->m_pShell,
                                            AEE_APPSCOMMONRES_IMAGESFILE,
                                            IDI_REFUI_LOUDSPEAKER);
        }
        else
        {
            pMe->m_pConvImage = ISHELL_LoadResImage( pMe->m_pShell,
                                            AEE_APPSCOMMONRES_IMAGESFILE,
                                            IDI_REFUI_CALLING);
        }
    }

    if(pMe->m_pConvImage)
    {
        //IIMAGE_SetParm(pMe->m_pConvImage, IPARM_ROP, AEE_RO_TRANSPARENT, 0);
        IIMAGE_Draw(pMe->m_pConvImage, CALL_TEXT_X, CALL_SECOND_LINE_Y);
        IIMAGE_Release(pMe->m_pConvImage);
        pMe->m_pConvImage = NULL;
        //CALL_ERR("Draw connect image",0,0,0);
    }
#endif
    oldColor = IDisplay_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, CALLAPP_TEXT_COLOR);
    // display Call Name
    if(pMe->m_Is3Way)/*in this time we only show 'muticall'*/
    {
        if(!b_cdg)
        {
            AECHAR mui_call[MAX_SIZE_NAME_TEXT] ={0};
#if defined(FEATURE_VERSION_C337) 			
            (void) ISHELL_LoadResString(pMe->m_pShell,
                                                AEE_APPSCALLAPP_RES_FILE,
							    IDS_MULTICALL,
                                                mui_call,
                                                sizeof(mui_call));
#else
            (void) ISHELL_LoadResString(pMe->m_pShell,
                                                AEE_APPSCALLAPP_RES_FILE,
                                                        IDS_MUTI_CALL,    
                                                mui_call,
                                                sizeof(mui_call));
#endif 
            /*预留出动画的宽度*/
            SETAEERECT(&rect,
                                                CALL_NAME_X,
                                                CALL_SECOND_LINE_Y,
                                                CALL_NAME_DX,
                                                CALL_LINE_HIGHT);
                                                
#if defined(FEATURE_VERSION_C01) ||defined(FEATURE_VERSION_C11)||defined(FEATURE_VERSION_C180)||defined(FEATURE_VERSION_W027V3)|| defined(FEATURE_VERSION_H1201)
            SETAEERECT(&rect,
                                                CALL_NAME_X-20,
                                                CALL_SECOND_LINE_Y+30,
                                                CALL_NAME_DX+20,
                                                CALL_LINE_HIGHT);
            (void)IDISPLAY_DrawText(pMe->m_pDisplay,
                                                AEE_FONT_NORMAL,
                                                mui_call,
                                                -1,
                                                rect.x,
                                                rect.y,
                                                &rect,
                                                IDF_TEXT_TRANSPARENT);
#else
#ifndef FEATURE_VERSION_C316						
            (void)IDISPLAY_DrawText(pMe->m_pDisplay,
                                                AEE_FONT_NORMAL,
                                                mui_call,
                                                -1,
                                                CALL_NAME_X,
                                                CALL_SECOND_LINE_Y,
                                                &rect,
                                                IDF_TEXT_TRANSPARENT);
#endif
#endif
        }
    }
    else//not 3Way
    {
        Dialer_call_table * temp = NULL;
        temp = CallApp_Get_First_Entry_In_Call_Table(pMe);
        if(temp)
        {
            if(temp->in_phonebook && !b_cdg)
            {
                //draw call name
                /*预留出动画的宽度*/
                SETAEERECT(&rect,
                                            CALL_NAME_X,
                                            CALL_SECOND_LINE_Y,
                                            CALL_NAME_DX,
                                            CALL_LINE_HIGHT);
                CallApp_DrawText_Ex(pMe, AEE_FONT_NORMAL,
                            temp->call_name, &rect, IDF_TEXT_TRANSPARENT);

            }
#ifdef FEATURE_ICM
            if(pMe->m_cdg_dsp_info.pi== AEECM_PI_ALLOW)
#else
            if(pMe->m_cdg_dsp_info.pi== AEET_PI_ALLOW)
#endif
            {
                // display Call Number
                if(temp->in_phonebook || b_cdg)
                {
                    int16 dy = 0;
                    if(pMe->m_cdg_row == 0)//No CDG info
                    {
                        dy = 2* CALL_LINE_HIGHT;
                    }
                    else if (pMe->m_cdg_row == 1)
                    {
                        dy = CALL_LINE_HIGHT;
                    }
                    else
                    {
                        dy = 0;
                    }
                    SETAEERECT(&rect,
                                                CALL_NUM_X, //CALL_TEXT_X,
                                                CALL_THIRD_LINE_Y +pMe->m_cdg_row * CALL_LINE_HIGHT,
                                                CALL_NUM_DX, //CALL_TEXT_DX,
                                                dy);
                }
                else
                {
                    /*预留出动画的宽度*/
                    int16 dy = 0;
                    if(pMe->m_cdg_row == 0)//No CDG info
                    {
                        dy = 3* CALL_LINE_HIGHT;
                    }
                    else if (pMe->m_cdg_row == 1)
                    {
                        dy =CALL_LINE_HIGHT;
                    }
                    else
                    {
                        dy = CALL_LINE_HIGHT;
                    }
                    SETAEERECT(&rect,
                                                CALL_NAME_X,
                                                CALL_SECOND_LINE_Y +pMe->m_cdg_row * CALL_LINE_HIGHT,
                                                CALL_NAME_DX,
                                                dy);
                }

                if(WSTRNCMP(temp->call_number, L"PRIVATE",7) == 0)
                {
               ( void) ISHELL_LoadResString(pMe->m_pShell,
                                            AEE_APPSCALLAPP_RES_FILE,
                                            IDS_NO_NUMBER,
                                            pMe->wszPrivateString_tw,
                                            sizeof(pMe->wszPrivateString_tw));
                ////////////////////////////////////////////////////////////////////
                    (void)IDISPLAY_DrawText(pMe->m_pDisplay,
                                                AEE_FONT_NORMAL,
                                                pMe->wszPrivateString_tw,//User unknow
                                                -1,
                                                rect.x,//pMe->m_rc.dx - bOffset,
                                                rect.y,
                                                &rect,
                                                IDF_TEXT_TRANSPARENT);
                }
                else if(pMe->m_bIsPrivacy)
                {
                    nv_language_enum_type lang  = 0;
                    ICONFIG_GetItem(pMe->m_pConfig,CFGI_LANGUAGE_SELECTION,&lang,sizeof(lang));
                    if(NV_LANGUAGE_SPANISH == lang)
                    {
                        (void)IDISPLAY_DrawText(pMe->m_pDisplay,
                                            AEE_FONT_NORMAL,
                                            L"Privacidad"/*strPriv*/,
                                            -1,
                                            rect.x,//pMe->m_rc.dx - bOffset,
                                            rect.y,
                                            &rect,
                                            IDF_TEXT_TRANSPARENT);
                    }
                    else
                    {
                        (void)IDISPLAY_DrawText(pMe->m_pDisplay,
                                                    AEE_FONT_NORMAL,
                                                    L"Privacy"/*strPriv*/,
                                                    -1,
                                                    rect.x,//pMe->m_rc.dx - bOffset,
                                                    rect.y,
                                                    &rect,
                                                    IDF_TEXT_TRANSPARENT);
                    }
                }
                else
                {
                    CallApp_DrawText_Ex(pMe, AEE_FONT_NORMAL,
                                temp->call_number, &rect, IDF_TEXT_TRANSPARENT);
                }
            }
        }
    }
#if defined(FEATURE_VERSION_C337)   
    if(pMe->m_isIncoming)
    {
        AECHAR         szText[20];  
	 int bWidth = 0;	
        (void) ISHELL_LoadResString(pMe->m_pShell,
                                    AEE_APPSCALLAPP_RES_FILE,
                                    IDS_CALLING_DIALOG_TEXT,
                                    szText,
                                    sizeof(szText));		
        bWidth = IDISPLAY_MeasureText(pMe->m_pDisplay, AEE_FONT_NORMAL,szText);	
        SETAEERECT(&rect,
                                                pMe->m_rc.dx-bWidth-2,
                                                CALL_SECOND_LINE_Y+CALL_LINE_HIGHT*2+2,
                                                CALL_TEXT_DX + 4,
                                                3*CALL_LINE_HIGHT);	
        IDisplay_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, CALLAPP_TEXT_COLOR);
        
        (void)IDISPLAY_DrawText(pMe->m_pDisplay,
                    AEE_FONT_SMALL,
                    szText,
                    -1,
                    rect.x,
                    rect.y,//0,
                    &rect,
                    IDF_TEXT_TRANSPARENT|IDF_ALIGN_LEFT);		
    }
#endif
    IDisplay_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, oldColor);
}

/*=============================================================================
FUNCTION: CallApp_HandleEndCallBlinkTimer

DESCRIPTION: Timer callback used by the End Call dialog to flash the
             call timer.

PARAMETERS:
   *pUser: CCallApp object pointer

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void CallApp_HandleEndCallBlinkTimer(void *pUser)
{
    CCallApp     *pMe = (CCallApp *)pUser;
    ITimeCtl       *pTimerCtl;
    AEERect         rc,rect;
    int32           Time_Length;
    AECHAR          wBuf[20];

    CALL_FUN_START("CallApp_HandleEndCallBlinkTimer", 0, 0, 0);

    ASSERT(pMe->m_pActiveDlg != NULL);
    ASSERT(IDD_ENDCALL == pMe->m_pActiveDlgID);

    pTimerCtl = (ITimeCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDDC_ENDCALL_TIMER);

    ITIMECTL_GetRect(pTimerCtl,  &rc);

    Time_Length = ITIMECTL_GetTime(pTimerCtl);

    ITIMECTL_GetTimeString(pTimerCtl,
                                            (uint32)Time_Length,
                                            wBuf,
                                            sizeof(wBuf),
                                            GTS_SECS);
    ITIMECTL_SetActive(pTimerCtl, FALSE);
    IDisplay_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, CALLAPP_TEXT_COLOR);
    (void)IDISPLAY_DrawText(pMe->m_pDisplay,
                                            AEE_FONT_NORMAL,
                                            wBuf,
                                            -1,
                                            CALL_TEXT_X,
                                            CALL_SECOND_LINE_Y,
                                            &rc,
                                            IDF_ALIGN_CENTER | IDF_TEXT_TRANSPARENT);
    IDisplay_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, RGB_BLACK);
    IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE);
    SETAEERECT(&rect,
                                            CALL_TEXT_X,
                                            CALL_SECOND_LINE_Y,
                                            CALL_TEXT_DX,
                                            CALL_LINE_HIGHT);
    
    Appscommon_ResetBackgroundEx(pMe->m_pDisplay, &rect, TRUE);
    
    if (2000 == rc.y)
    {
        // Position call timer on second line
        rc.y = CALL_SECOND_LINE_Y;
    }

    else
    {
        // Position call timer off screen
        rc.y = 2000;
    }

    ITIMECTL_SetRect(pTimerCtl,  (const AEERect *)&rc);

    pMe->m_b_callend_time ++;
    if(pMe->m_b_callend_time == 10)
    {
        (void) ISHELL_PostEvent( pMe->m_pShell,
                                            AEECLSID_DIALER,
                                            EVT_DISPLAYDIALOGTIMEOUT,
                                            0,
                                            0);
        return;
    }
    (void) ISHELL_SetTimer(pMe->m_pShell,
                                            TIMEOUT_MS_ENDCALLBLINKTIMER,
                                            CallApp_HandleEndCallBlinkTimer,
                                            pMe);


} /* CallApp_HandleEndCallBlinkTimer() */


/*==================================================================================
FUNCTION CallApp_MissedList
Description 进入未接电话列表
PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:
===================================================================================*/
static boolean CallApp_MissedList(CCallApp *pMe)
{
    IRecentCalls     *recentcall = NULL;

    CALL_FUN_START("CallApp_MissedList", 0, 0, 0);

    if(AEE_SUCCESS == ISHELL_CreateInstance(pMe->m_pShell,
                                            AEECLSID_APP_RECENTCALL/*AEECLSID_RECENTCALLSAPP*/,
                                            (void**)&recentcall))
    {
        IRecentCalls_MissedList(recentcall);
        if (NULL != pMe->m_pwstrDialStringkeep)
        {
            pMe->m_DialString[0] = 0;
            (void)WSTRCPY(pMe->m_DialString, pMe->m_pwstrDialStringkeep);
            FREEIF(pMe->m_pwstrDialStringkeep);
            MOVE_TO_STATE(STATE_NUMBER_FROM_IDLE)
        }
        else
        {
            (void)ISHELL_CloseApplet(pMe->m_pShell, FALSE);
        }
        
        IRecentCalls_Release(recentcall);

        return TRUE;
    }
    return FALSE;
}

/*==============================================================================
函数：
       DisplayMenu_NextImgIsReadyTimer

说明：
       浏览图片时限制按键速度

参数：
       pMe [in]：指向Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       无。

备注：:

==============================================================================*/
//static void CallApp_IncomingCall_IsReady_Timer(void *pUser)
//{
//    CCallApp *pMe = (CCallApp *)pUser;
//
//    if (NULL == pMe)
//    {
//        return;
//    }
//
//    (void) ISHELL_PostEvent( pMe->m_pShell,
//                                                AEECLSID_DIALER/*AEECLSID_CALL*/,
//                                                EVT_INCOMISREADY,
//                                                0,
//                                                0);
//}

/*==================================================================================
FUNCTION CallApp_RefreshVolBar
Description 显示音量bar
PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:
===================================================================================*/
static void CallApp_RefreshVolBar(CCallApp *pMe)
{
#if 1
    //int16 img_id = 0;
    int imageIndex = 0;
    IImage  *pImage = NULL;

#if !defined( FEATURE_CALL_RECORDER)
    ISHELL_CancelTimer(pMe->m_pShell,
                                                    (PFNNOTIFY)CallApp_HandleDialogTimer_Redraw,
                                                    pMe);
#endif
    ICONFIG_GetItem(pMe->m_pConfig,CFGI_EAR_VOL,&pMe->m_CallVolume,sizeof(pMe->m_CallVolume));

    switch (pMe->m_CallVolume)
    {
        case OEMSOUND_1ST_VOL:
            //img_id = IDI_ICON_AUDIO_VOL0;
            imageIndex = 1;
            break;

        case OEMSOUND_3RD_VOL:
            //img_id = IDI_ICON_AUDIO_VOL2;
            imageIndex = 3;
            break;

        case OEMSOUND_4TH_VOL:
            //img_id = IDI_ICON_AUDIO_VOL3;
            imageIndex = 4;
            break;

        case OEMSOUND_5TH_VOL:
            //img_id = IDI_ICON_AUDIO_VOL4;
            imageIndex = 5;
            break;

        case OEMSOUND_2ND_VOL:
        default:
            //img_id = IDI_ICON_AUDIO_VOL1;
            imageIndex = 2;
            break;
    }

    if(pImage == NULL)
    {
        pImage = ISHELL_LoadImage(pMe->m_pShell, REFUI_VOLUME_IMAGE);
        //pImage = ISHELL_LoadResImage( pMe->m_pShell,AEE_APPSCOMMONRES_IMAGESFILE, img_id);
    }

    if(pImage)
    {
        AEEImageInfo imageInfo;
        AEERect rect = {0};
        
#ifdef FEATURE_EDITABLE_NUMBER
        ISHELL_CancelTimer(pMe->m_pShell, CallApp_Set_Cursor_Blink, pMe);
#endif
        IIMAGE_SetParm(pImage, IPARM_NFRAMES, REFUI_VOLUME_LEVELS, 0);
        IIMAGE_GetInfo(pImage, &imageInfo);
        SETAEERECT(&rect, (pMe->m_rc.dx - imageInfo.cxFrame)/2, CALL_ANIM_IMG_Y - CALL_LINE_HIGHT, imageInfo.cxFrame, imageInfo.cy);
        
        Appscommon_ResetBackgroundEx(pMe->m_pDisplay, &rect, TRUE);
        
        IIMAGE_DrawFrame(pImage, 
                                        imageIndex, 
                                        rect.x, 
                                        rect.y); 
        IIMAGE_Release(pImage);
        pImage = NULL;
        // Start overall timer
#if !defined( FEATURE_CALL_RECORDER)
        ISHELL_SetTimer(pMe->m_pShell,
                                                2000,
                                                CallApp_HandleDialogTimer_Redraw,
                                                pMe);
#endif
    }
#else
    AEERect        rect;
    int            bWidth;
    AECHAR         szText[20],szFormat[20];

    pMe->m_bRefreshVol = REFRESHVOL_ON;

    // 每次都读一遍真实音量
    (void) ICONFIG_GetItem(pMe->m_pConfig,
                                            CFGI_EAR_VOL,
                                            &pMe->m_CallVolume,
                                            sizeof(pMe->m_CallVolume));

    //Volume TEXT
    (void) ISHELL_LoadResString(pMe->m_pShell,
                                            AEE_APPSCALLAPP_RES_FILE,
                                            IDS_SPEAKER_VOLUME,
                                            szFormat,
                                            sizeof(szText));
    bWidth = IDISPLAY_MeasureText(pMe->m_pDisplay, AEE_FONT_NORMAL, szFormat);

    SETAEERECT(&rect,
                                            0,
                                            CALL_ANNU_HEIGHT + pMe->m_LineHeight ,
                                            pMe->m_rc.dx,//bWidth,
                                            2*pMe->m_LineHeight + 5);
    IDISPLAY_EraseRect(pMe->m_pDisplay, &rect);

    SETAEERECT(&rect,
                                            0,
                                            3*pMe->m_LineHeight,
                                            bWidth,
                                            pMe->m_LineHeight);
    switch (pMe->m_CallVolume)
    {
        case OEMSOUND_1ST_VOL:
            pMe->m_bVolumeLevel = VOLUME_ONE;
            break;

        case OEMSOUND_3RD_VOL:
            pMe->m_bVolumeLevel = VOLUME_THREE;
            break;

        case OEMSOUND_4TH_VOL:
            pMe->m_bVolumeLevel = VOLUME_FOUR;
            break;

        case OEMSOUND_5TH_VOL:
            pMe->m_bVolumeLevel = VOLUME_FIVE;
            break;

        case OEMSOUND_2ND_VOL:
        default:
            pMe->m_bVolumeLevel = VOLUME_TWO;
            break;
    }
    WSPRINTF(szText,
                                            sizeof(szText),
                                            szFormat,
                                            pMe->m_bVolumeLevel);
    IDISPLAY_EraseRect(pMe->m_pDisplay, &rect);
    (void)IDISPLAY_DrawText(pMe->m_pDisplay,
                                            AEE_FONT_NORMAL,
                                            szText,
                                            -1,
                                            0,
                                            3*pMe->m_LineHeight,
                                            &rect,
                                            IDF_TEXT_TRANSPARENT);
#endif
}
#ifdef KEYSND_ZY
/*==================================================================================
FUNCTION CallApp_SpecialKeySnd

Description 多彩按键音

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:
===================================================================================*/
void CallApp_SpecialKeySnd(CCallApp *pMe, uint16 wParam)
{
    char file_name[64];
    byte KeySndType;
    byte KeySndVolume;
    MSG_FATAL("CallApp_SpecialKeySnd Start", 0, 0, 0);
    if(wParam < AVK_0 || wParam > AVK_POUND)
    {
        return;
    }
    (void) ICONFIG_GetItem(pMe->m_pConfig,
                                            CFGI_KEYSND_TYPE,
                                            &KeySndType,
                                            sizeof(KeySndType));

    (void) ICONFIG_GetItem(pMe->m_pConfig,
                                            CFGI_BEEP_VOL,
                                            &KeySndVolume,
                                            sizeof(byte));

    if(0 == KeySndVolume)
    {
        return;
    }

    switch(KeySndType)
    {
        case OEMNV_KEYSND_MUSICBOX:
            if(wParam != AVK_0)
            {
                SPRINTF(file_name,"brew/ringers/other/musicbox%d.mid",wParam - AVK_0);
            }
            else
            {
                STRCPY(file_name,"brew/ringers/other/musicbox0.mid");
            }
            break;

        case OEMNV_KEYSND_GUITAR:
            if(wParam != AVK_0)
            {
                SPRINTF(file_name,"brew/ringers/other/guitar%d.mid",wParam - AVK_0);
            }
            else
            {
                STRCPY(file_name,"brew/ringers/other/guitar0.mid");
            }
            break;

        case OEMNV_KEYSND_TRUMPET:
            if(wParam != AVK_0)
            {
                SPRINTF(file_name,"brew/ringers/other/trumpet%d.mid",wParam - AVK_0);
            }
                else
            {
                STRCPY(file_name,"brew/ringers/other/trumpet0.mid");
            }
            break;

        default:
            return;
    }

    ISOUNDPLAYER_Set(pMe->m_SndPlayer, SDT_FILE, file_name);
    if(KeySndVolume <= OEMSOUND_5TH_VOL)
    {
        ISOUNDPLAYER_SetVolume(pMe->m_SndPlayer,GET_ISOUND_VOL_LEVEL(KeySndVolume));
    }
    ISOUNDPLAYER_Play(pMe->m_SndPlayer);
    MSG_FATAL("CallApp_SpecialKeySnd End", 0, 0, 0);
}

#endif /* KEYSND_ZY */

#ifdef FEATURE_APP_PAUSE_TIMER
static void  CallApp_SetTimerControl(void *pUser)
{
    AECHAR   *wszTimer=NULL;
    AECHAR   *wszPause=NULL;
    char     *pszNum = NULL;
    int       nlen;
    int       nTcount;
    CCallApp *pMe = (CCallApp *)pUser;

    if (NULL == pMe)
    {
        return;
    }

    if(pMe->m_CallsTable == NULL)
    {
        CALL_ERR("CallsTable == NULL CallApp_SetTimerControl",0,0,0);
        return;
    }
#ifndef FEATURE_USES_LOWMEM
    if (pMe->m_pConvImage)
    {
        IIMAGE_SetOffset(pMe->m_pConvImage, 0, pMe->m_rc.dy -
                                            (pMe->m_LineHeight + pMe->m_LineHeight + 14));
        IIMAGE_SetDrawSize(pMe->m_pConvImage, 176, 16);
        (void) IIMAGE_Draw(pMe->m_pConvImage, 0, pMe->m_rc.dy -
                                            (pMe->m_LineHeight + pMe->m_LineHeight + 14));
        IIMAGE_SetOffset(pMe->m_pConvImage, 0, 0);
    }
#endif
    nlen=WSTRLEN(pMe->m_TimerString);
    if (nlen==0)
    {
        CallApp_Draw_Connect_Number_and_Name(pMe);
        CallApp_Draw_Connect_Softkey(pMe);
        return;
    }

    wszTimer = WSTRCHR(pMe->m_TimerString, DIALER_TIMER_AECHAR);
    wszPause = WSTRCHR(pMe->m_TimerString, DIALER_PAUSE_AECHAR);

    if ((NULL!=wszTimer) && (NULL!=wszPause))
    {
        if (wszTimer>wszPause)
        {
            wszTimer=NULL;
        }
        else
        {
            wszPause=NULL;
        }
    }

    if ((wszTimer==NULL) && (wszPause==NULL))
    {
        nlen++;
        pszNum = (char *)CALLAPP_MALLOC((uint32)nlen);
        if (pszNum != NULL)
        {
            (void)WSTRTOSTR(pMe->m_TimerString,pszNum,nlen);
            pszNum[nlen-1] = '\0';
        }
#ifdef FEATURE_ICM
        (void)WSTRLCAT(pMe->m_CallsTable->call_number, pMe->m_TimerString ,AEECM_MAX_DIGITS_LENGTH);
#else
        (void)WSTRLCAT(pMe->m_CallsTable->call_number, pMe->m_TimerString ,AEET_MAX_DIGITS_LENGTH);
#endif
        MEMSET(pMe->m_TimerString,0,sizeof(pMe->m_TimerString));
    }
    else if(wszTimer!=NULL)
    {
        if (wszTimer!=pMe->m_TimerString)
        {
            nlen=wszTimer-pMe->m_TimerString;
            nlen++;
            pszNum = (char*)CALLAPP_MALLOC((uint32)nlen);

            *wszTimer=0;
            if (NULL != pszNum)
            {
                /*lint -e732*/
                MEMSET(pszNum,0,nlen);
                (void)WSTRTOSTR(pMe->m_TimerString,pszNum,nlen);
                /*lint +e732*/
            }
            *wszTimer=DIALER_TIMER_AECHAR;
        }

        nTcount = 0;
        // ignore multiple timer characters
        while(DIALER_TIMER_AECHAR == *wszTimer)
        {
            nTcount++;
            wszTimer++;
        }

        CALL_ERR("Timer count = %d",nTcount, 0, 0);
        nlen = WSTRLEN(pMe->m_CallsTable->call_number);

        WSTRNCOPYN(pMe->m_CallsTable->call_number + nlen,
                                                (int)(sizeof(pMe->m_CallsTable->call_number)/sizeof(AECHAR)) - nlen,
                                                pMe->m_TimerString,
                                                wszTimer - pMe->m_TimerString);

        nlen = WSTRLEN(wszTimer);
        if (nlen>0)
        {
            MEMMOVE(pMe->m_TimerString,wszTimer,nlen*sizeof(AECHAR));
            pMe->m_TimerString[nlen]=0;

            (void)ISHELL_SetTimer(pMe->m_pShell,
                                                CALL_TIMER_SET*nTcount,
                                                CallApp_SetTimerControl,
                                                pMe);
        }
        else
        {
            MEMSET(pMe->m_TimerString,0,sizeof(pMe->m_TimerString));
        }
    }
    else if(wszPause!=NULL)
    {
        if (wszPause!=pMe->m_TimerString)
        {
            nlen=wszPause-pMe->m_TimerString;
            nlen++;
            pszNum = (char*)CALLAPP_MALLOC((uint32)nlen);

            *wszPause=0;
            if (NULL != pszNum)
            {
                /*lint -e732*/
                MEMSET(pszNum,0,nlen);
                (void)WSTRTOSTR(pMe->m_TimerString,pszNum,nlen);
                /*lint +e732*/
            }
            *wszPause=DIALER_PAUSE_AECHAR;
        }

        // ignore multiple pause characters
        while(DIALER_PAUSE_AECHAR == *wszPause)
        {
            wszPause++;
        }

        nlen = WSTRLEN(pMe->m_CallsTable->call_number);

        WSTRNCOPYN(pMe->m_CallsTable->call_number + nlen,
                                                (int)(sizeof(pMe->m_CallsTable->call_number)/sizeof(AECHAR)) - nlen,
                                                pMe->m_TimerString,
                                                wszPause - pMe->m_TimerString);

        pMe->m_PauseString[0]=0;
        (void)WSTRLCPY(pMe->m_PauseString, wszPause ,MAX_SIZE_DIALER_TEXT);

        MEMSET(pMe->m_TimerString,0,sizeof(pMe->m_TimerString));
    }

    if (pszNum != NULL)
    {
        int strLen;
        AECHAR *wbuf = NULL;
        strLen =  STRLEN(pszNum);

        CallApp_PlayToneList(pMe, pszNum, strLen);
        strLen = strLen + 1;
        wbuf = (AECHAR*)CALLAPP_MALLOC(strLen*sizeof(AECHAR));
        if(wbuf)
        {
            STRTOWSTR(pszNum,wbuf,strLen*sizeof(AECHAR));
            CallApp_BurstDTMF(pMe,wbuf,strLen);
            CALLAPP_FREE(wbuf);
        }
        CALLAPP_FREE(pszNum);
    }

    CallApp_Draw_Connect_Number_and_Name(pMe);
    CallApp_Draw_Connect_Softkey(pMe);
}
#endif

//Add By zzg 2010_08_03
#ifdef FEATURE_FRENDUO
static void  CallApp_SetFrenduoTimer(void *pUser)
{
	CCallApp *pMe = (CCallApp *)pUser;

    (void)ISHELL_SetTimer(pMe->m_pShell, CALL_TIMER_FRENDUO,  CallApp_SetFrenduoTimer, pMe);

    pMe->m_userCanceled = TRUE;
#ifdef FEATURE_ICM
    ICM_EndAllCalls(pMe->m_pICM);
#else
    ICALLMGR_EndAllCalls(pMe->m_pICallMgr);
#endif
}

static boolean  CallApp_SendFrenduoSMS(void)
{		
    IWmsApp *pIWmsApp = NULL;		
	
	if (SUCCESS != ISHELL_CreateInstance(AEE_GetShell(), AEECLSID_WMSAPP, (void**)&pIWmsApp))
	{
		return EFAILED;
	}
	
	if (SUCCESS == IWmsApp_SendSpecMessageEx(pIWmsApp))
	{
		MSG_FATAL("FrenDuoApp: IWmsApp_SendSpecMessageEx  SUCCESS!",0,0,0);
	}
	else
	{
		(void)IWmsApp_Release(pIWmsApp);
		pIWmsApp = NULL;
		return EFAILED;
	}
    
    (void)IWmsApp_Release(pIWmsApp);
    pIWmsApp = NULL;
    return SUCCESS;	
}
#endif
//Add End

static void  CallApp_SetPauseControl(void *pUser)
{
    CCallApp *pMe = (CCallApp *)pUser;
    WSTRCPY(pMe->m_TimerString, pMe->m_PauseString);
    pMe->m_PauseString[0] = 0;
    CallApp_SetTimerControl(pMe);
}

/*==============================================================================
函数：
       CallApp_ConvReleasePKey
说明：
       用于'P','K'序列处理转换。

参数：

返回值：

备注：

==============================================================================*/
static void CallApp_ConvReleasePKey(CCallApp *pMe)
{
    AECHAR   *wszTimer=NULL;
    AECHAR   *wszPause=NULL;
    char     *pszNum = NULL;
    int       nlen;
    int       nTcount;

    CALL_FUN_START("CallApp_ConvReleasePKey",0,0,0);
    if ( pMe == NULL )
    {
        return;
    }

    if(pMe->m_CallsTable == NULL)
    {
        //CALL_ERR("m_CallsTable == NULL",0,0,0);
        return;
    }

    nlen=WSTRLEN(pMe->m_PauseString);
    if (nlen==0)
    {
        return;
    }

    wszPause = WSTRCHR(pMe->m_PauseString, DIALER_PAUSE_AECHAR);
#ifdef FEATURE_APP_PAUSE_TIMER
    wszTimer = WSTRCHR(pMe->m_PauseString, DIALER_TIMER_AECHAR);
#endif

    if ((NULL!=wszTimer) && (NULL!=wszPause))
    {   // 同时存在'T'和'P'，谁在前就先处理谁
        if (wszTimer>wszPause)
        {
            wszTimer=NULL;
        }
        else
        {
            wszPause=NULL;
        }
    }

    nlen=WSTRLEN(pMe->m_PauseString);
    if ((wszTimer==NULL) && (wszPause==NULL))
    {
        nlen++;
        pszNum = (char *)CALLAPP_MALLOC((uint32)nlen);
        if (pszNum != NULL)
        {
            (void)WSTRTOSTR(pMe->m_PauseString,pszNum,nlen);
            pszNum[nlen-1] = '\0';
        }
#ifdef FEATURE_ICM
        (void)WSTRLCAT(pMe->m_CallsTable->call_number, pMe->m_PauseString ,AEECM_MAX_DIGITS_LENGTH);
#else
        (void)WSTRLCAT(pMe->m_CallsTable->call_number, pMe->m_PauseString ,AEET_MAX_DIGITS_LENGTH);
#endif
        MEMSET(pMe->m_PauseString,0,sizeof(pMe->m_PauseString));
    }
    else if(wszTimer!=NULL)
    {
        if (wszTimer!=pMe->m_PauseString)
        {
            nlen=wszTimer-pMe->m_PauseString;
            nlen++;
            pszNum = (char *)CALLAPP_MALLOC((uint32)nlen);

            *wszTimer=0;
            if (NULL != pszNum)
            {
                /*lint -e732*/
                MEMSET(pszNum,0,nlen);
                (void)WSTRTOSTR(pMe->m_PauseString,pszNum,nlen);
                /*lint +e732*/
            }
#ifdef FEATURE_APP_PAUSE_TIMER
            *wszTimer=DIALER_TIMER_AECHAR;
#endif
        }

        nTcount = 0;
        // ignore multiple timer characters
#ifdef FEATURE_APP_PAUSE_TIMER
        while(DIALER_TIMER_AECHAR == *wszTimer)
        {
            nTcount++;
            wszTimer++;
        }
#endif
        CALL_ERR("Timer count = %d",nTcount, 0, 0);
        nlen = WSTRLEN(pMe->m_CallsTable->call_number);

        WSTRNCOPYN(pMe->m_CallsTable->call_number + nlen,
                                                (int)(sizeof(pMe->m_CallsTable->call_number)/sizeof(AECHAR)) - nlen,
                                                pMe->m_PauseString,
                                                wszTimer - pMe->m_PauseString);

        nlen = WSTRLEN(wszTimer);

        if (nlen>0)
        {
#ifdef FEATURE_APP_PAUSE_TIMER
            pMe->m_TimerString[0]=0;
            (void)WSTRLCPY(pMe->m_TimerString, wszTimer ,MAX_SIZE_DIALER_TEXT);

            (void)ISHELL_SetTimer(pMe->m_pShell,
                                                CALL_TIMER_SET*nTcount,
                                                CallApp_SetTimerControl,
                                                pMe);
#endif
        }
        MEMSET(pMe->m_PauseString,0,sizeof(pMe->m_PauseString));
    }
    else if(wszPause!=NULL)
    {
        if (wszPause!=pMe->m_PauseString)
        {
            nlen=wszPause-pMe->m_PauseString;
            nlen++;
            pszNum = (char *)CALLAPP_MALLOC((uint32)nlen);

            *wszPause=0;
            if (NULL != pszNum)
            {
                /*lint -e732*/
                MEMSET(pszNum,0,nlen);
                (void)WSTRTOSTR(pMe->m_PauseString,pszNum,nlen);
                /*lint +e732*/
            }
            *wszPause=DIALER_PAUSE_AECHAR;
        }

        // ignore multiple pause characters
        while(DIALER_PAUSE_AECHAR == *wszPause)
        {
            wszPause++;
        }

        nlen = WSTRLEN(pMe->m_CallsTable->call_number);

        WSTRNCOPYN(pMe->m_CallsTable->call_number + nlen,
                                                (int)(sizeof(pMe->m_CallsTable->call_number)/sizeof(AECHAR)) - nlen,
                                                pMe->m_PauseString,
                                                wszPause - pMe->m_PauseString);

        nlen = WSTRLEN(wszPause);
        if (nlen>0)
        {
            MEMMOVE(pMe->m_PauseString,wszPause,nlen*sizeof(AECHAR));
            pMe->m_PauseString[nlen]=0;
        }
        else
        {
            MEMSET(pMe->m_PauseString,0,sizeof(pMe->m_PauseString));
        }
#ifdef FEATURE_APP_PAUSE_TIMER
        MEMSET(pMe->m_TimerString,0,sizeof(pMe->m_TimerString));
#endif
    }

    if (pszNum != NULL)
    {
#ifdef FEATURE_ICM
        byte strLen;
        AECHAR wsbuf[AEECM_MAX_DIGITS_LENGTH] = {0};
        const DTMFToneDuration *dur = CallApp_GetDTMFDuration(pMe);

        strLen = (byte) STRLEN(pszNum);
        STRTOWSTR(pszNum,wsbuf,sizeof(wsbuf));

        CallApp_PlayToneList(pMe, pszNum, strLen);
        ICM_BurstDTMF(pMe->m_pICM, pMe->m_CallsTable->call_id, dur->nOn, dur->nOff, wsbuf);
#else
        byte strLen;
        
        const DTMFToneDuration *dur = CallApp_GetDTMFDuration(pMe);
		ICall *pCall = NULL;

        strLen = (byte) STRLEN(pszNum);

        CallApp_PlayToneList(pMe, pszNum, strLen);
		ICALLMGR_GetCall(pMe->m_pICallMgr,pMe->m_CallsTable->call_id,&pCall);
		if (pCall != NULL)
		{
		    ICALL_BurstDTMF(pCall, dur->nOn, dur->nOff, pszNum);
		    ICALL_Release(pCall);
		} 
#endif
        CALLAPP_FREE(pszNum);
    }
}// CallApp_ConvReleasePKey

/*==============================================================================
CallApp_ShortcutQuiet

说明：
      通过#KEY，进入安静模式

参数：
       pMe

返回值：
       void

备注：:

==============================================================================*/
static void CallApp_ShortcutQuiet(CCallApp *pMe)
{
    byte     curProfile;
    byte     byte_return[PROFILENUMBER];
    byte     new_return;

    (void) ICONFIG_GetItem(pMe->m_pConfig,
                                CFGI_PROFILE_CUR_NUMBER,
                                &curProfile,
                                sizeof(curProfile));
/*
#ifdef FEATURE_VERSION_C316
    if (curProfile == OEMNV_PROFILE_BLUETOOTH)
    {
        pMe->m_Profile=OEMNV_PROFILE_BLUETOOTH;
        return;
    }
#endif
*/

    if(curProfile == OEMNV_PROFILE_MEETING)
    {
        curProfile = OEMNV_PROFILE_NORMALMODE;
        pMe->m_Profile=OEMNV_PROFILE_NORMALMODE;
        IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_RINGTONE, ANNUN_STATE_RINGTONE_ALERT);
    }
    else
    {
        curProfile = OEMNV_PROFILE_MEETING;
        pMe->m_Profile=OEMNV_PROFILE_MEETING;
        IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_RINGTONE, ANNUN_STATE_RINGTONE_VIBRATOR);
    }
    ICONFIG_SetItem(pMe->m_pConfig,
                        CFGI_PROFILE_CUR_NUMBER,
                        &curProfile,
                        sizeof(curProfile));

    // 修改当前 响闹 模式
    (void) ICONFIG_GetItem(pMe->m_pConfig,
                                CFGI_PROFILE_ALERT_TYPE,
                                byte_return,
                                sizeof(byte_return)); 
    
    new_return = byte_return[curProfile];
    
    (void)ICONFIG_SetItem(pMe->m_pConfig,
                                CFGI_ALERT_TYPE,
                                &new_return,
                                sizeof(new_return));    

    // 修改短信提示方式
    (void) ICONFIG_GetItem(pMe->m_pConfig,
                                CFGI_PROFILE_SMS_RINGER,
                                byte_return,
                                sizeof(byte_return));   

    new_return = byte_return[curProfile];

    (void)ICONFIG_SetItem(pMe->m_pConfig,
                                CFGI_SMS_RINGER,
                                &new_return,
                                sizeof(new_return));   
    //修改来电话声音
     (void) ICONFIG_GetItem(pMe->m_pConfig,
                           CFGI_PROFILE_CALL_RINGER,
                           byte_return,
                           sizeof(byte_return));
                           
    new_return = byte_return[curProfile];
    
    (void)ICONFIG_SetItem(pMe->m_pConfig,
                               CFGI_PROFILE_CALL_RINGER,
                               &new_return,
                               sizeof(new_return));

    // 修改按键音
    (void) ICONFIG_GetItem(pMe->m_pConfig,
                                CFGI_PROFILE_BEEP_VOL,
                                byte_return,
                                sizeof(byte_return)); 

    new_return = byte_return[curProfile];

    (void)ICONFIG_SetItem(pMe->m_pConfig,
                                CFGI_BEEP_VOL,
                                &new_return,
                                sizeof(new_return));       

    // 修改未接来电提示音
    (void) ICONFIG_GetItem(pMe->m_pConfig,
                                CFGI_PROFILE_MISSED_CALL_ALERT,
                                byte_return,
                                sizeof(byte_return));

    new_return = byte_return[curProfile];

    (void)ICONFIG_SetItem(pMe->m_pConfig,
                                CFGI_MISSED_CALL_ALERT,
                                &new_return,
                                sizeof(new_return));    

	//Add By zzg 2011_12_09
	if(GetMp3PlayerStatus() == MP3STATUS_RUNONBACKGROUND)
    {
        ISHELL_SendEvent(pMe->m_pShell,
                         AEECLSID_APP_MUSICPLAYER,
                         EVT_ALARM,
                         FALSE,
                         TRUE);
    }
	//Add End


}

static void CallApp_Flash_Call(CCallApp *pMe)
{
#ifdef FEATURE_ICM
    AEECMCallID nCallID = 0;
#else
	ICall *pCall = NULL;
#endif
    CALL_FUN_START("CallApp_Flash_Call %d", pMe->m_Is3Way, 0, 0);
#ifndef FEATURE_OEMOMH
    if(!pMe->m_Is3Way)
    {
        return;
    }
#endif
#ifdef FEATURE_ICM
    if(CheckAEEReturnStatus(ICM_OriginateVoiceCall(pMe->m_pICM, NULL, &nCallID)) == FALSE)
#else
    if(CheckAEEReturnStatus(ICALLMGR_OriginateVoice(pMe->m_pICallMgr, NULL, (ICall **)&pCall,NULL)) == FALSE)
#endif
    {
        CALL_ERR("ICM_OriginateVoiceCall FAILED", 0, 0, 0);
        return;
    }
#ifndef FEATURE_ICM
	if (pCall != NULL)
	{
		ICALL_Release(pCall);
	}
#endif
}

static void CallApp_StartContDTMF(CCallApp *pMe,AECHAR *c_dtmf)
{
#ifdef FEATURE_ICM
    int         i   = 0;
    AEECMCallID CallIDs[DIALERAPP_MAX_NUM_CALLS];
    uint16      wNumCallIDs;
    AECHAR  w_str = c_dtmf[0];

    //CALL_FUN_START("CallApp_StartContDTMF", 0, 0, 0);

    // get all active call ids
    wNumCallIDs = ICM_GetActiveCallIDs(pMe->m_pICM,
                                            AEECM_CALL_TYPE_VOICE|AEECM_CALL_TYPE_EMERGENCY|
                                            AEECM_CALL_TYPE_NON_STD_OTASP|AEECM_CALL_TYPE_STD_OTASP,
                                            AEECM_CALL_STATE_NONE, (AEECMCallID*) CallIDs, sizeof(CallIDs));
    CALL_FUN_START("CallApp_StartContDTMF %d", wNumCallIDs, 0, 0);
    for(i=0; i<wNumCallIDs; i++)
    {
        AEECMCallID byCurrentCallID = CallIDs[i];
        // start transmitting continous DTMF
        if(CheckAEEReturnStatus(ICM_StartContDTMF(pMe->m_pICM, byCurrentCallID, w_str)) == FALSE)
        {
            CALL_ERR("ICM_StartContDTMF FAILED", 0, 0, 0);
        }
    }
#else
    int         i   = 0;
  //  uint8 CallIDs[DIALERAPP_MAX_NUM_CALLS];
   // uint16      wNumCallIDs;
    AECHAR  w_str = c_dtmf[0];
    char str;
    //CALL_FUN_START("CallApp_StartContDTMF", 0, 0, 0);
	AEETCalls po;
	
	if(SUCCESS != ITELEPHONE_GetCalls(pMe->m_pITelephone, &po,sizeof(AEETCalls)))
	{
		return ;
	}

    // get all active call ids

	WSTRTOSTR(&w_str,&str,1);
    CALL_FUN_START("CallApp_StartContDTMF %d", po.dwCount, 0, 0);
    for(i=0; i<po.dwCount; i++)
    {
        uint8 byCurrentCallID = po.calls[i];
		ICall *pCall = NULL;
        // start transmitting continous DTMF
        
		ICALLMGR_GetCall(pMe->m_pICallMgr,byCurrentCallID,&pCall);
		if (pCall == NULL)
		{
			continue;
		}
        if(CheckAEEReturnStatus(ICALL_StartDTMF(pCall, str)) == FALSE)
        {
            CALL_ERR("ICM_StartContDTMF FAILED", 0, 0, 0);
        }
		ICALL_Release(pCall);
    }
#endif
}

static void CallApp_BurstDTMF(CCallApp *pMe,AECHAR *dtmf,int len)
{
#ifdef FEATURE_ICM
    int         i   = 0;
    AEECMCallID CallIDs[DIALERAPP_MAX_NUM_CALLS];
    uint16      wNumCallIDs;
    const DTMFToneDuration *dur = CallApp_GetDTMFDuration(pMe);

    CALL_FUN_START("CallApp_BurstDTMF", 0, 0, 0);
    if(dtmf)
    {
        // get all active call ids
        wNumCallIDs = ICM_GetActiveCallIDs(pMe->m_pICM,
                                                AEECM_CALL_TYPE_VOICE|AEECM_CALL_TYPE_EMERGENCY|
                                                AEECM_CALL_TYPE_NON_STD_OTASP|AEECM_CALL_TYPE_STD_OTASP,
                                                AEECM_CALL_STATE_NONE, (AEECMCallID*) CallIDs, sizeof(CallIDs));


        for(i=0; i<wNumCallIDs; i++)
        {
            AEECMCallID byCurrentCallID = CallIDs[i];
            // start transmitting continous DTMF
            if(CheckAEEReturnStatus(ICM_BurstDTMF(pMe->m_pICM, byCurrentCallID,dur->nOn,dur->nOff, dtmf)) == FALSE)
            {
                CALL_ERR("ICM_BurstDTMF FAILED", 0, 0, 0);
            }
        }
        //CALLAPP_FREE(c_dtmf);
    }
#else
    int         i   = 0;
   // uint8 CallIDs[DIALERAPP_MAX_NUM_CALLS];
    const DTMFToneDuration *dur = CallApp_GetDTMFDuration(pMe);
	char dtmfstr[MAX_SIZE_DIALER_TEXT] = {0};

	WSTRTOSTR(dtmf,dtmfstr,len);
	
    CALL_FUN_START("CallApp_BurstDTMF", 0, 0, 0);
    if(dtmf)
    {
        // get all active call ids
        
		AEETCalls po;
		
		if(SUCCESS != ITELEPHONE_GetCalls(pMe->m_pITelephone, &po,sizeof(AEETCalls)))
		{
			return;
		}
        
        for(i=0; i<po.dwCount; i++)
        {
            uint8 byCurrentCallID = po.calls[i];
			ICall *pCall = NULL;
            // start transmitting continous DTMF
            ICALLMGR_GetCall(pMe->m_pICallMgr,byCurrentCallID,&pCall);
			if (pCall == NULL)
			{
				continue;
			}
            if(CheckAEEReturnStatus(ICALL_BurstDTMF(pCall,dur->nOn,dur->nOff, dtmfstr)) == FALSE)
            {
                CALL_ERR("ICM_BurstDTMF FAILED", 0, 0, 0);
            }
			ICALL_Release(pCall);
        }
        //CALLAPP_FREE(c_dtmf);
    }
#endif
}

#ifdef FEATURE_ICM
static void CallApp_Stop_ContDTMF(ICM *m_pICM)
{
    int         i   = 0;
    AEECMCallID CallIDs[DIALERAPP_MAX_NUM_CALLS];
    uint16      wNumCallIDs;

    MSG_FATAL("CallApp_Stop_ContDTMF Start", 0, 0, 0);

    if(!m_pICM)
    {
        return;
    }
    // get all active call ids
    wNumCallIDs = ICM_GetActiveCallIDs(m_pICM,
                                            AEECM_CALL_TYPE_VOICE|AEECM_CALL_TYPE_EMERGENCY|
                                            AEECM_CALL_TYPE_NON_STD_OTASP|AEECM_CALL_TYPE_STD_OTASP,
                                            AEECM_CALL_STATE_NONE, (AEECMCallID*) CallIDs, sizeof(CallIDs));

    for(i=0; i<wNumCallIDs; i++)
    {
        AEECMCallID byCurrentCallID = CallIDs[i];
        // start transmitting continous DTMF
        if(CheckAEEReturnStatus(ICM_StopContDTMF(m_pICM, byCurrentCallID)) == FALSE)
        {
            CALL_ERR("ICM_StopContDTMF FAILED", 0, 0, 0);
        }
    }
    MSG_FATAL("CallApp_Stop_ContDTMF End", 0, 0, 0);
}
#else
static void CallApp_Stop_ContDTMF(CCallApp *pMe)
{
    int         i   = 0;
	AEETCalls po;

    MSG_FATAL("CallApp_Stop_ContDTMF Start", 0, 0, 0);

    if(!pMe)
    {
        return;
    }	
	
	if(SUCCESS != ITELEPHONE_GetCalls(pMe->m_pITelephone, &po,sizeof(AEETCalls)))
	{
		return;
	}

    for(i=0; i<po.dwCount; i++)
    {
        uint8 byCurrentCallID = po.calls[i];
		ICall *pCall = NULL;
        // start transmitting continous DTMF
        ICALLMGR_GetCall(pMe->m_pICallMgr,byCurrentCallID,&pCall);
		if (pCall == NULL)
		{
			continue;
		}
        if(CheckAEEReturnStatus(ICALL_StopDTMF(pCall)) == FALSE)
        {
            CALL_ERR("ICM_StopContDTMF FAILED", 0, 0, 0);
        }
		ICALL_Release(pCall);
    }
    MSG_FATAL("CallApp_Stop_ContDTMF End", 0, 0, 0);
}
#endif
//add for CDG test, CNAP with Forwarding
/*==============================================================================
函数：
       CallApp_NotifyDisp

说明：
       函数用于绘制Notify传递的文本。

参数：
       pMe

返回值：
        无

备注：:

==============================================================================*/
static boolean CallApp_NotifyDisp(CCallApp *pMe)
{
    AEERect   rect;
    int pixelLen = 0;
    int fits = 0;
    int dy = 1;
    int len = 0;
    int len_redir = 0;
    //boolean    show = FALSE;
    CALL_FUN_START("CallApp_NotifyDisp %d", pMe->m_cdg_dsp_info.pi, 0, 0);
    //ISHELL_CancelTimer(pMe->m_pShell,
    //                                                    (PFNNOTIFY)CallApp_NotifyDisp_CB,
    //                                                    pMe);
#ifdef FEATURE_ICM
    if(pMe->m_cdg_dsp_info.pi == AEECM_PI_RES)
#else
    if(pMe->m_cdg_dsp_info.pi == AEET_PI_RES)
#endif
    {
        ISHELL_LoadResString(pMe->m_pShell,
                                                    AEE_APPSCALLAPP_RES_FILE,
                                                    IDS_NUMBER_RESTRICTED,
                                                    pMe->m_cdg_dsp_info.alpha,
                                                    sizeof(pMe->m_cdg_dsp_info.alpha));
    }
#ifdef FEATURE_ICM
    else if(pMe->m_cdg_dsp_info.pi == AEECM_PI_NOT_AVAIL)
#else
    else if(pMe->m_cdg_dsp_info.pi == AEET_PI_NOT_AVAIL)
#endif
    {
        ISHELL_LoadResString(pMe->m_pShell,
                                                    AEE_APPSCALLAPP_RES_FILE,
                                                    IDS_NUMBER_UNAVAILABLE,
                                                    pMe->m_cdg_dsp_info.alpha,
                                                    sizeof(pMe->m_cdg_dsp_info.alpha));
    }
    else
    {
        
    }
    CALL_PRINTF("cdg disp %S",pMe->m_cdg_dsp_info.alpha);
    CALL_PRINTF("cdg redir %S",pMe->m_RedirNumStr);
    len = WSTRLEN(pMe->m_cdg_dsp_info.alpha);
    pixelLen = IDISPLAY_MeasureTextEx(pMe->m_pDisplay,
                                            AEE_FONT_NORMAL,
                                            pMe->m_cdg_dsp_info.alpha,
                                            -1,
                                            CALL_NAME_DX,
                                            &fits);
    CALL_ERR("%d %d %d cdg_info_print",pixelLen,len,fits);
    len_redir = WSTRLEN(pMe->m_RedirNumStr);

    if(len > fits || len_redir > 0)//need two lines
    {
        pMe->m_cdg_row = 1;//cdg info need 2 lines
        dy = 2;
    }

    if(len_redir > 0)
    {
        SETAEERECT(&rect,
                                                    CALL_NAME_X,
                                                    CALL_SECOND_LINE_Y,
                                                    CALL_NAME_DX,
                                                    CALL_LINE_HIGHT *dy);
        
        Appscommon_ResetBackgroundEx(pMe->m_pDisplay, &rect, TRUE);
    
        SETAEERECT(&rect,
                                                    CALL_NAME_X,
                                                    CALL_SECOND_LINE_Y,
                                                    CALL_NAME_DX,
                                                    CALL_LINE_HIGHT);
        CallApp_DrawText_Ex(pMe, AEE_FONT_NORMAL,pMe->m_cdg_dsp_info.alpha, &rect, 
                                                IDF_ALIGN_LEFT|IDF_TEXT_TRANSPARENT);
        SETAEERECT(&rect,
                                                    CALL_TEXT_X,
                                                    CALL_THIRD_LINE_Y,
                                                    CALL_TEXT_DX,
                                                    CALL_LINE_HIGHT);
        
        Appscommon_ResetBackgroundEx(pMe->m_pDisplay, &rect, TRUE);

        CallApp_DrawText_Ex(pMe, AEE_FONT_BOLD,pMe->m_RedirNumStr, &rect, 
                                                IDF_ALIGN_LEFT|IDF_TEXT_TRANSPARENT);
        return TRUE;
    }
    SETAEERECT(&rect,
                                                    CALL_NAME_X,
                                                    CALL_SECOND_LINE_Y,
                                                    CALL_NAME_DX,
                                                    CALL_LINE_HIGHT *dy);
    
    Appscommon_ResetBackgroundEx(pMe->m_pDisplay, &rect, TRUE);

    CallApp_DrawText_Ex(pMe, AEE_FONT_NORMAL,pMe->m_cdg_dsp_info.alpha, &rect, 
                                                IDF_ALIGN_LEFT|IDF_TEXT_TRANSPARENT);
    return TRUE;
}


#if 0//def Temp_remove
static void CallApp_Set_Db_In_Idle(boolean val)
{
    // use this function can draw Annunciator Rss/Batt
    db_items_value_type  db_value;
    db_value.db_in_idle = val;
    db_put(DB_IN_IDLE, &db_value);
}
#endif/*Temp_remove*/
/*=============================================================================
FUNCTION: CallApp_ShowDialer_Soft_Key

DESCRIPTION: show Bar in the dialer Dialog

PARAMETERS:
   *pMe: CCallApp object pointer

RETURN VALUE:
    None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void CallApp_Draw_NumEdit_SoftKey(CCallApp *pMe)
{
    BottomBar_e_Type  type = BTBAR_NONE;

	MSG_FATAL("***zzg CallApp_Draw_NumEdit_SoftKey***", 0, 0, 0);
	
    if(pMe->m_b_incall )
    {
    	#ifndef FEATURE_ALL_KEY_PAD
        type = BTBAR_OPTION_DELETE;
        #else
        type = BTBAR_OPTION_BACK;
        #endif
    }
    else
    {
#ifdef FEATRUE_SET_IP_NUMBER
#ifdef FEATURE_ICM
        if(pMe->m_b_have_set_ip && !(CallApp_IsEmergencyMode(pMe->m_pICM)
#else
        if(pMe->m_b_have_set_ip && !(CallApp_IsEmergencyMode(pMe->m_pITelephone)
#endif
        ||pMe->idle_info.uimLocked))
        {
            type = BTBAR_SAVE_IP_DELETE;
        }
        else
#endif/*FEATRUE_SET_IP_NUMBER*/
        {
#ifndef FEATURE_ALL_KEY_PAD
            type = BTBAR_OPTION_SAVE_DEL; //BTBAR_SAVE_DELETE;
#else

#ifdef FEATURE_VERSION_W208S

			/*
			if (0 == WSTRCMP(pMe->m_DialString, L"*"))
			{
				type = BTBAR_LOCK_BACK; 						
			}
			else	
			*/	
			{
				type = BTBAR_OPTION_SAVE_BACK;
			}
#else
			type = BTBAR_OPTION_SAVE_BACK;
#endif

        	
#endif
        }
    }

    if(pMe->m_bShowPopMenu) //menu
    {
        return;
    }
    //drawBottomBar(pMe->m_pDisplay,AEE_FONT_NORMAL,type);
    REFUI_DRAW_BOTTOMBAR(type)
}
/*=============================================================================
FUNCTION: CallApp_Draw_Connect_Softkey

DESCRIPTION: Used by the conversation dialog to setup its softkeys

PARAMETERS:
   *pMe: CCallApp object pointer
   *pSkMenu: ISoftkey interface pointer

RETURN VALUE:
    None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:
   CallApp_ConvHandleEvent()
=============================================================================*/
static void CallApp_Draw_Connect_Softkey(CCallApp *pMe)
{

    if(pMe->m_bShowPopMenu) //menu
    {
    }
    else
    {
        //drawBottomBar(pMe->m_pDisplay,AEE_FONT_NORMAL,BTBAR_OPTION_BACK/*BTBAR_OPTION_END*/);

		//Add By zzg 2010_09_16: Headset > anyother
		if (HS_HEADSET_ON())
		{
			DrawBottomBar_Ex(AEE_GetShell(),pMe->m_pDisplay,BTBAR_OPTION_ENDCALL);
		}
		else if (pMe->m_bHandFree)
		{
			#ifdef FEATURE_VERSION_C337
			REFUI_DRAW_BOTTOMBAR(BTBAR_OPTION_HANDS_HELD)
			#elif defined FEATURE_VERSION_W317A
			REFUI_DRAW_BOTTOMBAR(BTBAR_OPTION_NORMAL)
			#else
			REFUI_DRAW_BOTTOMBAR(BTBAR_OPTION_NORMAL)
			#endif
		}
		else
		{
			#ifdef FEATURE_VERSION_C337
			REFUI_DRAW_BOTTOMBAR(BTBAR_OPTION_HANDS_FREE)
			#else
			REFUI_DRAW_BOTTOMBAR(BTBAR_OPTION_HANDSFREEON)
			#endif
		}
		//Add End
	

		/*
		if(pMe->m_bHandFree)
        {
        	
        	if(HS_HEADSET_ON())
        	{
        		DrawBottomBar_Ex(AEE_GetShell(),pMe->m_pDisplay,BTBAR_OPTION_ENDCALL);
        	}
			else			
        	{
            	REFUI_DRAW_BOTTOMBAR(BTBAR_OPTION_NORMAL)
        	}
        }
        else
        {	
        	
            if(HS_HEADSET_ON())
			{
				DrawBottomBar_Ex(AEE_GetShell(),pMe->m_pDisplay,BTBAR_OPTION_ENDCALL);
			}
			else			
			{
            	REFUI_DRAW_BOTTOMBAR(BTBAR_OPTION_HANDSFREEON)
			}
        }
		*/
    }
    IDISPLAY_Update(pMe->m_pDisplay);
}
/*=============================================================================
FUNCTION: CallApp_Search_HasExceptionalChar

DESCRIPTION: when user set own pnone number invisible when calling other number, and then, the accepter could not see the
incoming number. But now, if the accepter recall the inbisible number, it will cause the handset reset because of exceptional char
in callnumber string. This func is added to judge whether it has exceptional char in callnumber string.

PARAMETERS:
   char * szCallNumber: the callnumber string

RETURN VALUE:
    TRUE: if there's exceptional char(s) in callnumber string.
    FALSE: there's no exceptional char(s).

COMMENTS:

SIDE EFFECTS:

SEE ALSO:
    None
=============================================================================*/
static boolean CallApp_Search_HasExceptionalChar(AECHAR * szCallNumber)
{
    int   len = 0;
    int i;
    boolean have = FALSE;
    len = WSTRLEN(szCallNumber);

    for(i = 0; i < len; i++)
    {
        if((szCallNumber[i] == DIALER_PAUSE_AECHAR)
#ifdef FEATURE_APP_PAUSE_TIMER
            ||(szCallNumber[i] == DIALER_TIMER_AECHAR)
#endif/*FEATURE_APP_PAUSE_TIMER*/
            ||((i != 0) && (szCallNumber[i] == DIALER_PLUS_AECHAR))
            )
        {
            have = TRUE;
            break;
        }

    }
    CALL_ERR("CallApp_Search_HasExceptionalChar %d %d %d",i,len,have);
    return have;
}


#ifdef FEATURE_MORSE
extern boolean gMorseBgRun;
#endif
static boolean CallApp_Process_HeldKey_Event(CCallApp *pMe,
                                           AEEEvent    eCode,
                                           uint16      wParam,
                                           uint32      dwParam)
{
    CALL_FUN_START("CallApp_Process_HeldKey_Event",0,0,0);
    //if(EVT_KEY != eCode)
    //{
    //    CALL_ERR("Sorry, it isn't evt_key",0,0,0);
    //    return FALSE;
    //}

    //if ((dwParam & KB_AUTOREPEAT) == 0)//it isn't  held key event
    //{
    //    CALL_ERR("Sorry,it isn't  held key event ",0,0,0);
    //    return FALSE;
    //}

	MSG_FATAL("***zzg CallApp_Process_HeldKey_Event wParam=%x***", wParam,0,0);

    //if ((AVKType)wParam == AVK_CLR)
	if ((AVKType)wParam == AVK_DEL)   	//Modify by zzg 2012_02_22
    {
        // Clearing the last digit exits the dialog
        pMe->m_DialString[0] = 0;
        CallApp_Draw_NumEdit_SoftKey(pMe);
		CallApp_Display_Number(pMe);
		// Draw it now!
		IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
        CLOSE_DIALOG(DLGRET_OK)
        return TRUE;
    }
    //if(pMe->m_lastCallState == AEECM_CALL_STATE_IDLE)
    if(pMe->m_b_incall == FALSE)
    {
#ifdef FEATRUE_SET_IP_NUMBER
        if((AVKType)wParam == AVK_SEND)/*Dialer IP Number*/
        {
            //IP_Number_Cfg ip_config;
            //ICONFIG_GetItem(pMe->m_pConfig,CFGI_IP_NUMBER,(void*)&ip_config,sizeof(IP_Number_Cfg));
            //if((boolean)ip_config.bStateOn == TRUE && WSTRLEN(pMe->m_DialString) > 2)
            //{
#ifdef FEATURE_ICM
            if(!(CallApp_IsEmergencyMode(pMe->m_pICM)||pMe->idle_info.uimLocked))
#else
            if(!(CallApp_IsEmergencyMode(pMe->m_pITelephone)||pMe->idle_info.uimLocked))
#endif
            {
                pMe->m_b_from_numedit = TRUE;
                CLOSE_DIALOG(DLGRET_SEL_IP_NUMBER)
            }
            //}
            return TRUE;
        }
#endif /*FEATRUE_SET_IP_NUMBER*/
        if ( (AVKType)wParam == AVK_STAR)
        {
#if !defined(FEATURE_DISP_128X160)&&!defined(FEATURE_DISP_176X220)
#ifdef FEATURE_KEYGUARD
            if(WSTRLEN(pMe->m_DialString) == 1)
            {
            	pMe->m_b_set_keyguard = TRUE;
            	ISHELL_CloseApplet(pMe->m_pShell, TRUE);
            }
#endif
#else
			int len = 0;
		    len = WSTRLEN(pMe->m_DialString);
		    
		    if(pMe->m_b_incall == FALSE )
		    {
		        if (len > MAX_SIZE_DIAL_STR -2)
		        {
		            CALL_ERR("IDLE state,m_DialString len is 32",0, 0, 0);		           
		        }
		    }
		    else
		    {
		        if (len > MAX_SIZE_DIALER_TEXT -3)
		        {
		            CALL_ERR("DTMF state,m_DialString len is 128",0, 0, 0);		            
		        }
		    }		
			
			{
				AECHAR wstrTemp[MAX_SIZE_DIALER_TEXT] = {0};

				if (pMe->m_nCursorPos == 0)
				{
	                (void)WSTRCPY(&pMe->m_DialString[len-pMe->m_nCursorPos-1], L"+");	               
				}
				else
				{
					(void)WSTRCPY(wstrTemp, &pMe->m_DialString[len-pMe->m_nCursorPos]);
	                (void)WSTRCPY(&pMe->m_DialString[len-pMe->m_nCursorPos-1], L"+");
	                (void)WSTRCPY(&pMe->m_DialString[len-pMe->m_nCursorPos], wstrTemp);
				}								

				(void) ISHELL_PostEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_USER_REDRAW,0,0);				

				//Appscommon_ResetBackgroundEx(pMe->m_pDisplay, &pMe->m_rc, TRUE);            
	            //CallApp_Draw_NumEdit_SoftKey(pMe);
	            //CallApp_Display_Number(pMe);
			}
#endif
        }

        // # key for shortcut of quiet mode
        else if ( ((AVKType)wParam == AVK_POUND ) && WSTRLEN(pMe->m_DialString) == 1)
        {
        	MSG_FATAL("POSTEVETN..................................",0,0,0);
			//ISHELL_PostEvent(pMe->m_pShell,AEECLSID_CORE_APP,EVT_USER,EVT_MODE_CHANGE,0);
            CallApp_ShortcutQuiet( pMe );

/*            
#ifdef FEATURE_VERSION_C316
            if (pMe->m_Profile  == OEMNV_PROFILE_BLUETOOTH)
            {
                //Do Nothing.............
                return TRUE;
            }
#endif      
*/
            if(pMe->m_Profile == OEMNV_PROFILE_NORMALMODE)
            {
                pMe->m_msg_text_id = IDS_MSG_CURPROFILE_NORMALMODE;
            }
            else
            {
                //ISOUND_Vibrate(pMe->m_pSound, 2000);   //客户未要求不加振动提示  
                pMe->m_msg_text_id = IDS_MSG_CURPROFILE_MEETING;
            }
            CLOSE_DIALOG(DLGRET_MSGBOX);
            // clk_busy_wait(1000*10000);
            //ISHELL_CloseApplet(pMe->m_pShell, TRUE);
        }

        //long key "1" to call voice mail
#ifndef FEATURE_VERSION_C337        
        else if ((AVKType)wParam == AVK_1)
        {
            if((pMe->m_DialString[0] == '1')&&(pMe->m_DialString[1] == '\0'))
            {
                CallApp_MakeVoiceMailCall(pMe);
            }
        }
#endif        
        else if ( ((AVKType)wParam >= AVK_2) &&((AVKType)wParam <= AVK_9))
        {
            uint16 wIndex;
            MSG_FATAL("CallApp_Process_HeldKey_Event1111111111111",0,0,0);
            // TBD - dial string format should be typedef'd
            //if more than 2 digits then bail out because
            // we only support from to 1 to 99
            wIndex = (uint16)WSTRTOFLOAT(pMe->m_DialString);

            if(((uint16)WSTRLEN(pMe->m_DialString) <= MAX_SPEEDDIAL_CHARS) &&(wIndex!=0))
            {
                CallApp_MakeSpeedDialCall(pMe);
            }
        }
		//Add By zzg 2010_09_10
		#if defined(FEATURE_VERSION_C316)||defined(FEAUTRE_VERSION_N450)||defined(FEATURE_VERSION_W0216A)|| defined(FEATURE_VERSION_C306) || defined(FEATURE_VERSION_W515V3) || defined(FEATURE_VERSION_N68)||defined(FEATURE_LCD_TOUCH_ENABLE)||defined(FEATURE_VERSION_W516)||defined(FEATURE_VERSION_W208S)|| defined(FEATURE_VERSION_C11)|| defined(FEATURE_VERSION_C180)|| defined(FEATURE_VERSION_H1201)|| defined(FEATURE_VERSION_W027)\
			||defined(FEATURE_VERSION_W0216A_T18) //xxzhen
        #ifndef FEATURE_TORCH_KEY_INFO
		else if (((AVKType)wParam == AVK_0) && (WSTRLEN(pMe->m_DialString) == 1))
		{	
		    #ifdef FEATURE_VERSION_W317A
            CallApp_LaunchApplet(pMe,  AEECLSID_APP_FMRADIO);  // add by pyuangui 20121220
			#else
		    {
		    boolean TorchOn = FALSE;
			OEM_GetConfig(CFGI_FLSHLITHG_STATUS,&TorchOn, sizeof(TorchOn));
			if (TorchOn == FALSE )
			{
				TorchOn = TRUE;
				if (pMe->m_pBacklight)
				{
					IBACKLIGHT_TurnOnTorch(pMe->m_pBacklight);
				}
			}
			else
			{
				TorchOn = FALSE;
				if (pMe->m_pBacklight)
				{							
					IBACKLIGHT_TurnOffTorch(pMe->m_pBacklight);
				}
			}
			OEM_SetConfig(CFGI_FLSHLITHG_STATUS,&TorchOn, sizeof(TorchOn));

            #if defined(FEATURE_VERSION_W027V3) || defined(FEATURE_VERSION_W317A)|| defined(FEATURE_VERSION_M74)

            {
                static IStatic * torch_pStatic = NULL;
                 PromptMsg_Param_type m_PromptMsg;
                 MEMSET(&m_PromptMsg,0,sizeof(PromptMsg_Param_type));
                 if(TorchOn)
                 m_PromptMsg.nMsgResID= IDS_MAIN_MENU_TORCHON;
                 else
                 m_PromptMsg.nMsgResID= IDS_MAIN_MENU_TORCHOFF;   
                 m_PromptMsg.ePMsgType = MESSAGE_WARNNING;
                 STRLCPY(m_PromptMsg.strMsgResFile, AEE_APPSCALLAPP_RES_FILE,MAX_FILE_NAME);
                 m_PromptMsg.eBBarType = BTBAR_NONE;
                 if (NULL == torch_pStatic)
                 {
                     AEERect rect = {0};
                     if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,AEECLSID_STATIC,(void **)&torch_pStatic))
                     {
                         return FALSE;
                     }
                     ISTATIC_SetRect(torch_pStatic, &rect);
                 }
                 DrawPromptMessage(pMe->m_pDisplay,torch_pStatic,&m_PromptMsg);
                 IDISPLAY_UpdateEx(pMe->m_pDisplay,TRUE);
                 ISTATIC_Release(torch_pStatic);
                 torch_pStatic=NULL;
                 (void)ISHELL_SetTimer(pMe->m_pShell,1000, (PFNNOTIFY)CallApp_TorchTipTimeOut,pMe);
                }
            #else
			  ISHELL_CloseApplet(pMe->m_pShell, TRUE);
            #endif
			}
			#endif
    	}
        #endif
    	#else
    	else if ((AVKType)wParam == AVK_0) 
    	{
    		int len = 0;
		    len = WSTRLEN(pMe->m_DialString);
		    if(pMe->m_b_incall == FALSE )
		    {
		        if (len > MAX_SIZE_DIAL_STR -2)
		        {
		            CALL_ERR("IDLE state,m_DialString len is 32",0, 0, 0);		           
		        }
		    }
		    else
		    {
		        if (len > MAX_SIZE_DIALER_TEXT -3)
		        {
		            CALL_ERR("DTMF state,m_DialString len is 128",0, 0, 0);		            
		        }
		    }		
			
			{
				AECHAR wstrTemp[MAX_SIZE_DIALER_TEXT] = {0};

				if (pMe->m_nCursorPos == 0)
				{
	                (void)WSTRCPY(&pMe->m_DialString[len-pMe->m_nCursorPos-1], L"+");	               
				}
				else
				{
					(void)WSTRCPY(wstrTemp, &pMe->m_DialString[len-pMe->m_nCursorPos]);
	                (void)WSTRCPY(&pMe->m_DialString[len-pMe->m_nCursorPos-1], L"+");
	                (void)WSTRCPY(&pMe->m_DialString[len-pMe->m_nCursorPos], wstrTemp);
				}								

				(void) ISHELL_PostEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_USER_REDRAW,0,0);				

				//Appscommon_ResetBackgroundEx(pMe->m_pDisplay, &pMe->m_rc, TRUE);            
	            //CallApp_Draw_NumEdit_SoftKey(pMe);
	            //CallApp_Display_Number(pMe);
			}
    	}
    	#endif
		//Add End

    }
	else
	{
		if ( (AVKType)wParam == AVK_STAR)
        {
			int len = 0;
		    len = WSTRLEN(pMe->m_DialString);
		    
		    if(pMe->m_b_incall == FALSE )
		    {
		        if (len > MAX_SIZE_DIAL_STR -2)
		        {
		            CALL_ERR("IDLE state,m_DialString len is 32",0, 0, 0);		           
		        }
		    }
		    else
		    {
		        if (len > MAX_SIZE_DIALER_TEXT -3)
		        {
		            CALL_ERR("DTMF state,m_DialString len is 128",0, 0, 0);		            
		        }
		    }		
			
			{
				AECHAR wstrTemp[MAX_SIZE_DIALER_TEXT] = {0};

				if (pMe->m_nCursorPos == 0)
				{
	                (void)WSTRCPY(&pMe->m_DialString[len-pMe->m_nCursorPos-1], L"+");	               
				}
				else
				{
					(void)WSTRCPY(wstrTemp, &pMe->m_DialString[len-pMe->m_nCursorPos]);
	                (void)WSTRCPY(&pMe->m_DialString[len-pMe->m_nCursorPos-1], L"+");
	                (void)WSTRCPY(&pMe->m_DialString[len-pMe->m_nCursorPos], wstrTemp);
				}								

				(void) ISHELL_PostEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_USER_REDRAW,0,0);				

				//Appscommon_ResetBackgroundEx(pMe->m_pDisplay, &pMe->m_rc, TRUE);            
	            //CallApp_Draw_NumEdit_SoftKey(pMe);
	            //CallApp_Display_Number(pMe);
			}
        }
	}
    return TRUE;
}

#if defined(FEATURE_VERSION_W027V3) || defined(FEATURE_VERSION_W317A)|| defined(FEATURE_VERSION_M74)

static void CallApp_TorchTipTimeOut(CCallApp *pMe)
{
    if (NULL == pMe)
    {
        return;
    }
    ISHELL_CloseApplet(pMe->m_pShell, TRUE);
}
#endif

static void CallApp_Process_Spec_Key_Event(CCallApp *pMe,uint16 wp)
{
    int len = 0;

    CALL_FUN_START("CallApp_Process_Spec_Key_Event",0, 0, 0);
	MSG_FATAL("***zzg CallApp_Process_Spec_Key_Event***",0, 0, 0);

    len = WSTRLEN(pMe->m_DialString);

    //CALL_ERR("%d %d Avk0_Event 1", pMe->m_btime_out, pMe->m_return_value, 0);

    //if (pMe->m_lastCallState == AEECM_CALL_STATE_IDLE)
    if(pMe->m_b_incall == FALSE )
    {
        if (len > MAX_SIZE_DIAL_STR -2)
        {
            CALL_ERR("IDLE state,m_DialString len is 32",0, 0, 0);
            return;
        }
    }
    else
    {
        if (len > MAX_SIZE_DIALER_TEXT -3)
        {
            CALL_ERR("DTMF state,m_DialString len is 128",0, 0, 0);
            return;
        }
    }
	///////////////////////////////////////////////////////////////////////////////////////
if(wp == AVK_0)
{
        if((pMe->m_btime_out % MAX_COUNT_TO_CHANGE ) == 0)//need change
        {
            if(pMe->m_return_value == RETURN_ZERO)
            {
#ifdef FEATURE_EDITABLE_NUMBER
                if(len-pMe->m_nCursorPos ==1 && pMe->m_btime_out != 0)
#else
                if(len ==1 && pMe->m_btime_out != 0)
#endif
                {
                    pMe->m_return_value = RETURN_PLUS;//"+"only in first digt
                }
               
                if(pMe->m_btime_out == 0)
                {
#ifdef FEATURE_EDITABLE_NUMBER
                    if(pMe->m_nCursorPos == 0)
                    {
#endif
                    WSTRCPY(&pMe->m_DialString[len], L"0"); 
#ifdef FEATURE_EDITABLE_NUMBER
                    }
                    else
                    {
                        AECHAR wstrTemp[MAX_SIZE_DIALER_TEXT] = {0};

                        (void)WSTRCPY(wstrTemp, &pMe->m_DialString[len-pMe->m_nCursorPos]);
                        (void)WSTRCPY(&pMe->m_DialString[len-pMe->m_nCursorPos], L"0");
                        (void)WSTRCPY(&pMe->m_DialString[len-pMe->m_nCursorPos+1], wstrTemp);
                    }
#endif
                }
                else
                {
#ifdef FEATURE_EDITABLE_NUMBER
                    pMe->m_DialString[len-pMe->m_nCursorPos-1] = L'0';
#else
                    WSTRCPY(&pMe->m_DialString[len-1], L"0"); 
#endif
                }
            }            
            
            else if(pMe->m_return_value == RETURN_PLUS)
            {
                pMe->m_return_value = RETURN_ZERO;
#ifdef FEATURE_EDITABLE_NUMBER
                pMe->m_DialString[len-pMe->m_nCursorPos-1] = L'+';
#else
                WSTRCPY(&pMe->m_DialString[len-1], L"+");
#endif
            }
        }
        pMe->m_btime_out ++;
}
//////////////////////////////////////////////////////////////////////////////////////////////

if(wp == AVK_POUND)
    {
        if((pMe->m_btime_out % MAX_COUNT_TO_CHANGE ) == 0)//need change
        {
            if((pMe->m_return_value == RETURN_ZERO)&&(pMe->m_btime_out!=0))
            {
#ifdef FEATURE_EDITABLE_NUMBER				
                pMe->m_DialString[len-pMe->m_nCursorPos-1] = L'#';
#else				
                WSTRCPY(&pMe->m_DialString[len-1], L"#");
#endif
            }
            
            else if((pMe->m_return_value == RETURN_ZERO)&&(pMe->m_btime_out==0))
            {
                pMe->m_return_value = RETURN_POUND;
#ifdef FEATURE_EDITABLE_NUMBER
                if(pMe->m_nCursorPos == 0)
                {
#endif			
                WSTRCPY(&pMe->m_DialString[len], L"#");
#ifdef FEATURE_EDITABLE_NUMBER
                }
                else
                {
                    AECHAR wstrTemp[MAX_SIZE_DIALER_TEXT] = {0};

                    (void)WSTRCPY(wstrTemp, &pMe->m_DialString[len-pMe->m_nCursorPos]);					
                    (void)WSTRCPY(&pMe->m_DialString[len-pMe->m_nCursorPos], L"#");
                    (void)WSTRCPY(&pMe->m_DialString[len-pMe->m_nCursorPos+1], wstrTemp);
                }
#endif
            }
            
            else if(pMe->m_return_value == RETURN_T)
            {
                pMe->m_return_value = RETURN_POUND;
#ifdef FEATURE_EDITABLE_NUMBER
                if(len-pMe->m_nCursorPos > 1)
                {
                    pMe->m_DialString[len-pMe->m_nCursorPos-1] = L'T';
                }
#else
                if(len > 1)
                {
                    WSTRCPY(&pMe->m_DialString[len-1], L"T");
                }
#endif
            }
            
            else if(pMe->m_return_value == RETURN_POUND)
            {
                pMe->m_return_value = RETURN_T;
#ifdef FEATURE_EDITABLE_NUMBER
                if(len-pMe->m_nCursorPos > 1)
                {                	
                    pMe->m_DialString[len-pMe->m_nCursorPos-1] = L'#';
                }
#else
                if(len > 1)
                {                	
                    WSTRCPY(&pMe->m_DialString[len-1], L"#");
                }
#endif
            }
        }
            pMe->m_btime_out ++;
    }
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

if(wp == AVK_STAR)
    {
    #if 1//ndef FEATURE_ALL_KEY_PAD
       {
       		AECHAR szStr;
       		int len=0;
       		len = WSTRLEN(pMe->m_DialString);
        	AEE_CancelTimer(CallApp_keypadtimer,pMe);
        	szStr = CallApp_AVKSTAR_2ASCII(pMe);
        	if(pMe->m_curpros>0 ||(pMe->m_curpros==0 && pMe->b_multenter))
        	{
        		
        		AECHAR wstrTemp[MAX_SIZE_DIALER_TEXT] = {0};
        		AECHAR tempwStr = 0;
       		    if (pMe->m_nCursorPos == 0)
				{
        			//(void)WSTRCPY(&pMe->m_DialString[len-pMe->m_nCursorPoS], &szStr);	    
        			if(pMe->m_curpros == 0)
					{
						//return L'*';
						WSTRCPY(&pMe->m_DialString[len-1], L"*");
					}
					if(pMe->m_curpros == 1)
					{
						//return L'p';
						WSTRCPY(&pMe->m_DialString[len-1], L"p");
					}
					if(pMe->m_curpros == 2)
					{
						//return L'W';
						WSTRCPY(&pMe->m_DialString[len-1], L"w");
					}
					if(pMe->m_curpros == 3)
					{
						//return L'W';
						WSTRCPY(&pMe->m_DialString[len-1], L"+");
					}
				}
				else
				{
					//(void)WSTRCPY(wstrTemp, &pMe->m_DialString[len-pMe->m_nCursorPos]);
        			//(void)WSTRCPY(&pMe->m_DialString[len-pMe->m_nCursorPos-1], &szStr);
        			//(void)WSTRCPY(&pMe->m_DialString[len-pMe->m_nCursorPos], wstrTemp);
        			if(pMe->m_curpros == 0)
					{
						//return L'*';
						//WSTRCPY(&pMe->m_DialString[len-1], L"*");
						pMe->m_DialString[len-pMe->m_nCursorPos-1] = L'*';
					}
					if(pMe->m_curpros == 1)
					{
						//return L'p';
						//WSTRCPY(&pMe->m_DialString[len-1], L"p");
						pMe->m_DialString[len-pMe->m_nCursorPos-1] = L'p';
					}
					if(pMe->m_curpros == 2)
					{
						//return L'W';
						//WSTRCPY(&pMe->m_DialString[len-1], L"w");
						pMe->m_DialString[len-pMe->m_nCursorPos-1] = L'w';
					}
					if(pMe->m_curpros == 3)
					{
						//return L'W';
						//WSTRCPY(&pMe->m_DialString[len-1], L"w");
						pMe->m_DialString[len-pMe->m_nCursorPos-1] = L'+';
					}
				}	
        	}
        	else
        	{
        		AECHAR wstrTemp[MAX_SIZE_DIALER_TEXT] = {0};
        		if (pMe->m_nCursorPos == 0)
				{
        			(void)WSTRCPY(&pMe->m_DialString[len-pMe->m_nCursorPos], L"*");	               
				}
				else
				{
					(void)WSTRCPY(wstrTemp, &pMe->m_DialString[len-pMe->m_nCursorPos]);
        			(void)WSTRCPY(&pMe->m_DialString[len-pMe->m_nCursorPos], L"*");
        			(void)WSTRCPY(&pMe->m_DialString[len-pMe->m_nCursorPos+1], wstrTemp);
				}
        	}
            #if defined(FEATURE_VERSION_X3)
            if(pMe->m_curpros<3)
            #else
        	if(pMe->m_curpros<2)
            #endif
        	{
        		pMe->m_curpros ++;
        	}
        	else
        	{
        		pMe->m_curpros = 0;
        	}
        	pMe->b_multenter = TRUE;
        	AEE_SetTimer(1000,CallApp_keypadtimer,pMe);
        }
        #else
        if((pMe->m_btime_out % MAX_COUNT_TO_CHANGE ) == 0)//need change
        {
            if((pMe->m_return_value == RETURN_ZERO)&&(pMe->m_btime_out!=0))
            {
#ifdef FEATURE_EDITABLE_NUMBER
                pMe->m_DialString[len-pMe->m_nCursorPos-1] = L'*';
#else
                WSTRCPY(&pMe->m_DialString[len-1], L"*");
#endif
            }
            
            else if((pMe->m_return_value == RETURN_ZERO)&&(pMe->m_btime_out==0))
            { 
                pMe->m_return_value = RETURN_STAR;
#ifdef FEATURE_EDITABLE_NUMBER
                if(pMe->m_nCursorPos == 0)
                {
#endif
                WSTRCPY(&pMe->m_DialString[len], L"*");
#ifdef FEATURE_EDITABLE_NUMBER
                }
                else
                {
                    AECHAR wstrTemp[MAX_SIZE_DIALER_TEXT] = {0};

                    (void)WSTRCPY(wstrTemp, &pMe->m_DialString[len-pMe->m_nCursorPos]);
                    (void)WSTRCPY(&pMe->m_DialString[len-pMe->m_nCursorPos], L"*");
                    (void)WSTRCPY(&pMe->m_DialString[len-pMe->m_nCursorPos+1], wstrTemp);
                }
#endif
            }
            
            else if(pMe->m_return_value == RETURN_P)
            {
                pMe->m_return_value = RETURN_STAR;
#ifdef FEATURE_EDITABLE_NUMBER
                if(len-pMe->m_nCursorPos > 1)
                {
                    pMe->m_DialString[len-pMe->m_nCursorPos-1] = L'P';
                }
#else
                if(len > 1)
                {
                    WSTRCPY(&pMe->m_DialString[len-1], L"P");
                }
#endif
            }

            else if(pMe->m_return_value == RETURN_STAR)
            {
                pMe->m_return_value = RETURN_P;
#ifdef FEATURE_EDITABLE_NUMBER
                if(len-pMe->m_nCursorPos > 1)
                {
                    pMe->m_DialString[len-pMe->m_nCursorPos-1] = L'*';
                }
#else
                if(len > 1)
                {
                    WSTRCPY(&pMe->m_DialString[len-1], L"*");
                }
#endif
            }
        }
        pMe->m_btime_out ++;
        #endif
    }
}

/*=============================================================================
FUNCTION:  CallApp_DrawText_Ex

DESCRIPTION:

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CallApp_DrawText_Ex(CCallApp *pMe, AEEFont fnt,
                  AECHAR *wStr, AEERect  *rect,  uint32 mask)
{
    int           len       = 0;
    int           fits       = 0;/*指定的给定像素宽度的字符数*/
    int           pixLen = 0;/*字符串的实际像素宽度*/
    int           line      = 0;
    int           hight    = 0;
    AECHAR  wBuf[64] = {0};
    RGBVAL oldColor = IDisplay_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, CALLAPP_TEXT_COLOR);

    //IDLE 32bit   DTMF 127bit  16bit/line max line is 3
    //CALL_ERR("%d %d %d CallApp_DrawText_Ex",rect->x,rect->y,rect->dy);

    ASSERT(wStr != NULL);
    ASSERT(pMe->m_pDisplay != NULL);

    hight = IDISPLAY_GetFontMetrics(pMe->m_pDisplay,  fnt, NULL, NULL);
    line = rect->dy / hight;
    len = WSTRLEN(wStr);
    WSTRLCPY(wBuf, wStr, 64);

    MSG_FATAL("line======%d,hight=%d,rect->dy=%d",line,hight,rect->dy);
    while(line)
    {
        line--;
        len = WSTRLEN(wBuf);
        pixLen = IDISPLAY_MeasureTextEx(pMe->m_pDisplay, fnt,  wBuf,  -1, rect->dx, &fits);
		MSG_FATAL("CallApp_DrawText_Ex....len==%d,pixLen=%d",len,pixLen,0);
        //CALL_ERR("%d %d %d len,pixLen,fits",len,pixLen,fits);
        if (0 == pixLen)
        {
            // IDISPLAY_MeasureTextEx failed, just return the entire string
            line = 0;
            //CALL_ERR("IDISPLAY_MeasureTextEx failed",0,0,0);
            MSG_FATAL("IDISPLAY_DrawText........1111111",0,0,0);
            return IDISPLAY_DrawText(pMe->m_pDisplay,  fnt, wBuf,-1, rect->x, rect->y, rect, mask);
        }

        if (fits >= len)
        {
            // Entire string fits on the line, all is good
            line = 0;
            //CALL_ERR("Entire string fits on the line, all is good",0,0,0);
            MSG_FATAL("IDISPLAY_DrawText........222222",0,0,0);
            return IDISPLAY_DrawText(pMe->m_pDisplay,  fnt, wBuf,-1, rect->x, rect->y, rect, mask);
        }

        if(fits < len)
        {
            if(line == 0)//the last line ,need add ...
            {
                wBuf[fits-3] = 0;
                //CALL_ERR("the last line ,need add ...",0,0,0);
                WSTRCAT(wBuf, (AECHAR*)".\0.\0.\0");
				MSG_FATAL("IDISPLAY_DrawText........33333333",0,0,0);
                IDISPLAY_DrawText(pMe->m_pDisplay,  fnt, wBuf,-1, rect->x, rect->y, rect, mask);
            }
            else
            {
                AECHAR temp[64] = {0};
                //CALL_ERR("draw %d line",line,0,0);
                WSTRLCPY(temp, wBuf + fits , 64);
                wBuf[fits] = 0;
				MSG_FATAL("IDISPLAY_DrawText........44444444",0,0,0);
                IDISPLAY_DrawText(pMe->m_pDisplay,  fnt, wBuf,-1, rect->x, rect->y, rect, mask);
                WSTRLCPY(wBuf, temp,64);
                rect->y = rect->y + hight/*CALL_LINE_HIGHT*/;/*y向下增加一行*/
            }
        }
    }
    IDisplay_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, oldColor);
    return TRUE;
}
extern char charsvc_p_name[UIM_CDMA_HOME_SERVICE_SIZE+1];
static boolean CallApp_Process_Send_Key_Release_Event(CCallApp *pMe)
{
    uint16 wIndex;
    MAKE_CALL_VALUE ret;
	AECHAR TempTo[MAX_SIZE_DIALER_TEXT+1] = {0};
	AECHAR TempToStr[MAX_SIZE_DIALER_TEXT+1] = {0};
	char temp[MAX_SIZE_DIALER_TEXT+1] = {0};
    // TBD - dial string format should be typedef'd
    // Can only make emergency calls while emgcall is TRUE
    //if ((CallApp_IsEmergencyMode(pMe->m_pICM)
    //    ||pMe->idle_info.uimLocked)
    //    &&!CallApp_IsEmergency_Number(pMe->m_DialString))
    //{
    //    CLOSE_DIALOG(DLGRET_DIALER_INVALID_EMGNUMBER)
    //    return TRUE;
    //}
    /*in emergency call mode ,can not allow to make the muti call*/
	WSTRTOSTR(pMe->m_DialString,temp,MAX_SIZE_DIALER_TEXT+1);
	if((STRISTR(temp,"+91")))
	{
		WSTRCPY(TempToStr,L"0091");
		WSTRCPY(TempTo,pMe->m_DialString+3);
		WSTRCAT(TempToStr,TempTo);
		DBGPRINTF("TempToStr=%S", TempTo);
		DBGPRINTF("TempToStr=%S", TempToStr);
		DBGPRINTF("ppMe->m_DialString=%S", pMe->m_DialString);
		MEMSET(pMe->m_DialString,0,(sizeof(pMe->m_DialString))+1);
		WSTRCPY(pMe->m_DialString,TempToStr);
		DBGPRINTF("pMe->m_DialString=%S", pMe->m_DialString);
		MSG_FATAL("pMe->m_DialString........111111111",0,0,0);
	}
#ifdef FEATURE_ICM
    if(pMe->m_b_incall && CallApp_IsEmergencyMode(pMe->m_pICM) )
#else
    if(pMe->m_b_incall && CallApp_IsEmergencyMode(pMe->m_pITelephone) )
#endif
    {
        return TRUE;
    }
#ifdef FEATURE_SUPPORT_BT_APP
    if(pMe->m_b_incall)
    {
        bt_ui_process_incall_org_state();
    }
#endif
    if(pMe->m_b_incall== FALSE && (pMe->m_DialString[0] == '1')&&(pMe->m_DialString[1] == '\0'))
    {
#ifdef FEATURE_CARRIER_VENEZUELA_MOVILNET
        if(CALL_SUCESS != (ret = CallApp_MakeCall(pMe)))
        {
            if(CALL_FAIL_EMERGENCY_MODE == ret
                    ||CALL_FAIL_RESTICT == ret
                    ||CALL_FAIL_INVIDE_NUMBER == ret)
            {
                return TRUE;
            }
            CLOSE_DIALOG(DLGRET_CALL_ATTEMPT_FAILED)
        }
        return TRUE;
#else
        CallApp_MakeVoiceMailCall(pMe);
        return TRUE;
#endif
    }

    wIndex = (uint16)WSTRTOFLOAT(pMe->m_DialString);

    if(((uint16)WSTRLEN(pMe->m_DialString) <= MAX_SPEEDDIAL_CHARS)
        &&(wIndex != 0)
        &&(!CallApp_IsEmergency_Number(pMe->m_DialString))
        &&(pMe->m_b_incall == FALSE) )
    {
        CallApp_MakeSpeedDialCall(pMe);
    }
    else if(CALL_SUCESS != (ret = CallApp_MakeCall(pMe)))
    {
        if(CALL_FAIL_EMERGENCY_MODE == ret
                ||CALL_FAIL_RESTICT == ret
                ||CALL_FAIL_INVIDE_NUMBER == ret)
        {
            return TRUE;
        }
        CLOSE_DIALOG(DLGRET_CALL_ATTEMPT_FAILED)
    }
    return TRUE;
}

boolean CallApp_Process_EVT_FLIP_Event(CCallApp *pMe, uint32 wParam)
{
    boolean  headsetPresent =  FALSE;
    if (SUCCESS != ICONFIG_GetItem(pMe->m_pConfig,CFGI_HEADSET_PRESENT,&headsetPresent,sizeof(headsetPresent)))
    {
        headsetPresent = FALSE;
    }

    if(!headsetPresent && !wParam)// ok,end the call
    {
#ifdef FEATURE_ICM
        ICM_EndAllCalls(pMe->m_pICM);
#else
        ICALLMGR_EndAllCalls(pMe->m_pICallMgr);
#endif
        return TRUE;
    }
    return FALSE;
}

static void CallApp_HandleDialogTimer_Redraw(void *pUser)
{
    CCallApp *pMe = (CCallApp *)pUser;

    if((pMe->m_pActiveDlg == NULL)||(pMe == NULL))
    {
        return;
    }

#if defined( FEATURE_CALL_RECORDER)
	pMe->m_bSettingVolume = FALSE;
#endif

    (void) ISHELL_PostEvent(pMe->m_pShell,AEECLSID_DIALER,  EVT_USER_REDRAW,  0, 0);
}

static void CallApp_Play_Incoming_Tone(CCallApp *pMe)
{
    ringID      ringid[PROFILENUMBER];
    IRingerMgr      *RingerMgr = NULL;
    int ringerId = AEE_RINGER_ID_NONE;
    char        filename[MAX_FILE_NAME]={0};

	//Add By zzg 2011_10_25
	uisnd_notify_data_s_type sndInfo;
	uisnd_get_device(&sndInfo);
	MSG_FATAL("***zzg CallApp_Play_Incoming_Tone - dev=%d sMute=%d mMute=%d***", 
	  			sndInfo.out_device, sndInfo.speaker_mute, sndInfo.microphone_mute);	
	//Add End
	
    if((AECHAR)'\0' != pMe->m_CallsTable->ringer[0])
    {
        byte profilenum = 0;
        if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                            AEECLSID_RINGERMGR,
                                            (void **)&RingerMgr))
        {
            return ;
        }
        WSTRTOSTR(pMe->m_CallsTable->ringer, filename, MAX_FILE_NAME);

        // 为了支持PEK 测试，电话本的RINGTONE 字段只能保存字符串类型，这里根据路径名查找是否有MID存在
        ringerId = IRINGERMGR_GetRingerID(RingerMgr, filename);
        
        #ifdef FEATURE_PEKTEST
        if(AEE_RINGER_ID_NONE == ringerId)
        {
            ICONFIG_GetItem(pMe->m_pConfig, CFGI_PROFILE_CUR_NUMBER, &profilenum, sizeof(profilenum));
            ICONFIG_GetItem(pMe->m_pConfig, CFGI_PROFILE_CALL_RINGER, (void*)ringid, sizeof(ringid));
            if(ringid[profilenum].ringType == OEMNV_MID_RINGER)
            {
                ringerId = ringid[profilenum].midID;
            }
        }
        #endif
        if(AEE_RINGER_ID_NONE != ringerId)
        {
            // ringType is mid
            IALERT_StartRingerAlert(pMe->m_pAlert,ringerId);
        }
        else
        {
            // ringType is mp3
            if ((IALERT_StartMp3Alert(pMe->m_pAlert, filename,ALERT_NORMAL_SND) != SUCCESS))
            {
                IALERT_StartRingerAlert(pMe->m_pAlert, OEMNV_DEFAULTRINGER);
            }
        }
        IRINGERMGR_Release(RingerMgr);
    }
    else
    {
        byte midID;
        byte profilenum = 0;
        
        ICONFIG_GetItem(pMe->m_pConfig, CFGI_PROFILE_CUR_NUMBER, &profilenum, sizeof(profilenum));
        ICONFIG_GetItem(pMe->m_pConfig, CFGI_PROFILE_CALL_RINGER, (void*)ringid, sizeof(ringid));
        midID = ringid[profilenum].midID;
        if(ringid[profilenum].ringType == OEMNV_MID_RINGER)
        {
            IALERT_StartRingerAlert(pMe->m_pAlert,(uint32)midID);
        }
        else if(ringid[profilenum].ringType == OEMNV_MP3_RINGER)
        {
            if ((IALERT_StartMp3Alert(pMe->m_pAlert, ringid[profilenum].szMusicname,ALERT_NORMAL_SND) != SUCCESS))
            {
                IALERT_StartRingerAlert(pMe->m_pAlert, OEMNV_DEFAULTRINGER);
            }
        } 
    }
    
#if !defined( FEATURE_RECORDER)
    CallApp_SetupCallAudio(pMe);
#endif

}
#ifdef FEATURE_IMAGE_DIALING_DIGITS
static void CallApp_Load_Numer_Img(CCallApp *pMe)
{
#if 1
    int i;
    for(i = 0; i<REFUI_CALL_MAX_IMAGSIZE ;i++)
    {
        //pMe->num_image[i]  = ISHELL_LoadResImage( pMe->m_pShell,AEE_APPSCOMMONRES_IMAGESFILE,IDI_NUMBER_0 + i);
    }
#else
    pMe->num_image[0]  = ISHELL_LoadResImage( pMe->m_pShell,AEE_APPSCOMMONRES_IMAGESFILE,IDI_NUMBER_0);
    pMe->num_image[1]  = ISHELL_LoadResImage( pMe->m_pShell,AEE_APPSCOMMONRES_IMAGESFILE,IDI_NUMBER_1);
    pMe->num_image[2]  = ISHELL_LoadResImage( pMe->m_pShell,AEE_APPSCOMMONRES_IMAGESFILE,IDI_NUMBER_2);
    pMe->num_image[3]  = ISHELL_LoadResImage( pMe->m_pShell,AEE_APPSCOMMONRES_IMAGESFILE,IDI_NUMBER_3);
    pMe->num_image[4]  = ISHELL_LoadResImage( pMe->m_pShell,AEE_APPSCOMMONRES_IMAGESFILE,IDI_NUMBER_4);
    pMe->num_image[5]  = ISHELL_LoadResImage( pMe->m_pShell,AEE_APPSCOMMONRES_IMAGESFILE,IDI_NUMBER_5);
    pMe->num_image[6]  = ISHELL_LoadResImage( pMe->m_pShell,AEE_APPSCOMMONRES_IMAGESFILE,IDI_NUMBER_6);
    pMe->num_image[7]  = ISHELL_LoadResImage( pMe->m_pShell,AEE_APPSCOMMONRES_IMAGESFILE,IDI_NUMBER_7);
    pMe->num_image[8]  = ISHELL_LoadResImage( pMe->m_pShell,AEE_APPSCOMMONRES_IMAGESFILE,IDI_NUMBER_8);
    pMe->num_image[9]  = ISHELL_LoadResImage( pMe->m_pShell,AEE_APPSCOMMONRES_IMAGESFILE,IDI_NUMBER_9);
    pMe->num_image[10]  = ISHELL_LoadResImage( pMe->m_pShell,AEE_APPSCOMMONRES_IMAGESFILE,IDI_NUMBER_STAR);
    pMe->num_image[11]  = ISHELL_LoadResImage( pMe->m_pShell,AEE_APPSCOMMONRES_IMAGESFILE,IDI_NUMBER_POUND);
    pMe->num_image[12]  = ISHELL_LoadResImage( pMe->m_pShell,AEE_APPSCOMMONRES_IMAGESFILE,IDI_NUMBER_P);
    pMe->num_image[13]  = ISHELL_LoadResImage( pMe->m_pShell,AEE_APPSCOMMONRES_IMAGESFILE,IDI_NUMBER_T);
    pMe->num_image[14]  = ISHELL_LoadResImage( pMe->m_pShell,AEE_APPSCOMMONRES_IMAGESFILE,IDI_NUMBER_PLUS);
#endif
}

static void CallApp_Release_Numer_Img(CCallApp *pMe)
{
    int i = 0;
    for(i = 0; i<REFUI_CALL_MAX_IMAGSIZE ;i++)
    {
        if(pMe->num_image[i])
        {
            IIMAGE_Release(pMe->num_image[i]);
            pMe->num_image[i] = NULL;
        }
    }
}

static void CallApp_Draw_Numer_Img(CCallApp   *pMe,  AECHAR const *dialStr)
{
    int len = 0, count = 0, x = 0, y = 0, dx = 0, dy = 0;
    int nLineHeight = 0, nLineCount = 0, nRowCount = 0;
    AEERect   rect;
    AECHAR wbf[MAX_SIZE_DIALER_TEXT] = {0}; 
    AECHAR *p_str = NULL;

    WSTRLCPY(wbf,dialStr,sizeof(wbf)/sizeof(AECHAR)); //Gemsea:Size In AECHAR
    p_str = wbf;

    SETAEERECT(&rect,
                    CALL_SCREEN_X,
                    CALL_SCREEN_Y ,
                    CALL_SCREEN_DX,
                    pMe->m_rc.dy - BOTTOMBAR_HEIGHT);

#if defined( FEATURE_CALL_RECORDER)
    if( pMe->m_bRecorderOn)
    {
    	rect.dy -= NUM_IMAGE_HIGHT;
    }
#endif
    
    Appscommon_ResetBackgroundEx(pMe->m_pDisplay, &rect, TRUE);
    
    nLineHeight = NUM_IMAGE_HIGHT + BETWEEN_LINE_PIXEL;
    dx = rect.dx /NUM_IMAGE_WIDTH;
    dy = rect.dy/nLineHeight;
    
    if(MAX_SIZE_DIALER_TEXT/dx < dy)
    {
        dy = MAX_SIZE_DIALER_TEXT/dx;
    }
    
    count = dx *dy;
    
    SETAEERECT(&rect,
                    (pMe->m_rc.dx - NUM_IMAGE_WIDTH *dx) /2,
                    pMe->m_rc.dy - BOTTOMBAR_HEIGHT - nLineHeight*dy,
                    NUM_IMAGE_WIDTH*dx,
                    nLineHeight*dy);

#if defined( FEATURE_CALL_RECORDER)
    if( pMe->m_bRecorderOn)
    {
    	rect.y -= NUM_IMAGE_HIGHT;
    }
#endif
    

    len = WSTRLEN(p_str);
#ifdef FEATURE_EDITABLE_NUMBER
    if(len > count)
    {
        if(pMe->m_nCursorPos >= count)
        {
            if(pMe->m_nCursorPos/dx != len/dx)
            {
                p_str += (len - ((pMe->m_nCursorPos/dx + 1)*dx));
                len = count;
            }
            else
            {
                if(pMe->m_nCursorPos != len || len%dx != 0)
                {
                    len -= (pMe->m_nCursorPos/dx - dy + 1)*dx;
                }
                else
                {
                    len = count;
                }
            }
        }
        else
        {
            p_str = p_str + len - count;
            len = count;
        }
    }
#else
    if(len > count)
    {
        p_str = p_str + len - count;
        len   = WSTRLEN(p_str);
    }
#endif
    
    nRowCount = len %dx;
    x = rect.x + (dx - nRowCount) * NUM_IMAGE_WIDTH;
    nLineCount = len /dx;
    y = rect.y + (dy - nLineCount - 1) * nLineHeight;
    if(nRowCount == 0)
    {
        x = rect.x;
        y += nLineHeight;
    }

    for (count = 0;count < len ; count ++)
    {
        boolean b_found = TRUE;
        byte mm = 0;
        switch(p_str[count])
        {
            case '0':
                mm = 0;
                break;

            case '1':
                mm = 1;
                break;

            case '2':
                mm = 2;
                break;

            case '3':
                mm = 3;
                break;

            case '4':
                mm = 4;
                break;

            case '5':
                mm = 5;
                break;

            case '6':
                mm = 6;
                break;

            case '7':
                mm = 7;
                break;

            case '8':
                mm = 8;
                break;

            case '9':
                mm = 9;
                break;

            case '*':
                mm = 10;
                break;

            case '#':
                mm = 11;
                break;

            case 'P':
                mm = 12;
                break;

            case 'T':
                mm = 13;
                break;

            case '+':
                mm = 14;
                break;
            case '-':
                mm = 15;
                break;

            default:
                b_found = FALSE;
                break;
        }

        if(b_found)
        {
            if(pMe->num_image[mm])
            {
                IIMAGE_Draw(pMe->num_image[mm], x, y);
            }
            x += NUM_IMAGE_WIDTH;
            if(x  >=  rect.x + rect.dx)/*换行*/
            {
                x = rect.x;
                y += nLineHeight;
            }
        }
    }
}
#endif //FEATURE_IMAGE_DIALING_DIGITS
static void CallApp_Display_Number(CCallApp *pMe)
{

#ifdef FEATURE_VERSION_W208S
	int len,i;
	AECHAR t_DialString[MAX_SIZE_DIALER_TEXT];
	AECHAR dt_String1[30];
	AECHAR dt_String2[5];
	AECHAR dt_String3[5];

	MSG_FATAL("***zzg CallApp_Display_Number***", 0, 0, 0);

	(void)MEMSET( t_DialString,(AECHAR) 0, sizeof(t_DialString));
	(void)MEMSET( dt_String1,(AECHAR) 0, sizeof(dt_String1));
	(void)MEMSET( dt_String2,(AECHAR) 0, sizeof(dt_String2));
	(void)MEMSET( dt_String3,(AECHAR) 0, sizeof(dt_String3));
	WSTRCPY(t_DialString,pMe->m_DialString);
	len = WSTRLEN(pMe->m_DialString);
	
	MSG_FATAL("***zzg CallApp_Display_Number len=%d***", len, 0, 0);

	if ((3<len)&&(len<8))
	{
		for(i=0;i<3;i++) dt_String1[i]= t_DialString[len-3+i];
		WSTRCPY(&t_DialString[len-3], L"-");
		for(i=0;i<3;i++)  t_DialString[len-3+1+i]=dt_String1[i];
	}
	else if((7<len)&&((len<12)))
	{
		for(i=0;i<3;i++) dt_String1[i]= t_DialString[len-3+i];
		for(i=0;i<4;i++) dt_String2[i]= t_DialString[len-4-3+i];
		WSTRCPY(&t_DialString[len-4-3], L"-");
		WSTRCPY(&t_DialString[len+1-3], L"-");
		for(i=0;i<4;i++)  t_DialString[len-4-3+1+i]=dt_String2[i];
		for(i=0;i<3;i++)  t_DialString[len-3+1+1+i]=dt_String1[i];
	}		
	else if (len > 11)
	{		   
		for(i=0;i<(len-8);i++) dt_String1[i]= t_DialString[8+i];
		for(i=0;i<4;i++) dt_String2[i]= t_DialString[4+i];
		for(i=0;i<4;i++) dt_String3[i]= t_DialString[i];
		 
		WSTRCPY(&t_DialString[4], L"-");
		WSTRCPY(&t_DialString[4+4+1], L"-");		
		for(i=0;i<4;i++)  t_DialString[i]=dt_String3[i];
		for(i=0;i<4;i++)  t_DialString[i+4+1]=dt_String2[i];
		for(i=0;i<(len-8);i++)  t_DialString[4+4+1+1+i]=dt_String1[i];
		
	}
	//////////////////////////////////////////
#endif //FEATURE_VERSION_W208S

  #ifdef FEATURE_CARRIER_VENEZUELA_MOVILNET 
    int len,i;
    AECHAR t_DialString[MAX_SIZE_DIALER_TEXT];
    AECHAR dt_String1[30];
    AECHAR dt_String2[5];
    AECHAR dt_String3[5];
	
   (void)MEMSET( t_DialString,(AECHAR) 0, sizeof(t_DialString));
   (void)MEMSET( dt_String1,(AECHAR) 0, sizeof(dt_String1));
   (void)MEMSET( dt_String2,(AECHAR) 0, sizeof(dt_String2));
   (void)MEMSET( dt_String3,(AECHAR) 0, sizeof(dt_String3));
   WSTRCPY(t_DialString,pMe->m_DialString);
   len = WSTRLEN(pMe->m_DialString);
  
  
   if((4<len)&&(len<8))
   {
     for(i=0;i<4;i++) dt_String1[i]= t_DialString[len-4+i];
     WSTRCPY(&t_DialString[len-4], L"-");
     for(i=0;i<4;i++)  t_DialString[len-4+1+i]=dt_String1[i];
   }
   else if((7<len)&&(len<11))
   {
     for(i=0;i<4;i++) dt_String1[i]= t_DialString[len-4+i];
     for(i=0;i<3;i++) dt_String2[i]= t_DialString[len-4-3+i];
     WSTRCPY(&t_DialString[len-4-3], L"-");
     WSTRCPY(&t_DialString[len+1-4], L"-");
     for(i=0;i<3;i++)  t_DialString[len-4-3+1+i]=dt_String2[i];
     for(i=0;i<4;i++)  t_DialString[len-4+1+1+i]=dt_String1[i];

   }
   else if(len==11)
   {
      
     for(i=0;i<4;i++) dt_String1[i]= t_DialString[len-4+i];
     for(i=0;i<3;i++) dt_String2[i]= t_DialString[len-4-3+i];
     for(i=0;i<3;i++) dt_String3[i]= t_DialString[len-4-3-3+i];
     WSTRCPY(&t_DialString[len-4-3-3], L"-");
     WSTRCPY(&t_DialString[len+3-3-4-2], L"-");
     WSTRCPY(&t_DialString[len+3-4-1], L"-");
     for(i=0;i<3;i++)  t_DialString[len-4-3-3+1+i]=dt_String3[i];
     for(i=0;i<3;i++)  t_DialString[len-4-3+1+1+i]=dt_String2[i];
     for(i=0;i<4;i++)  t_DialString[len-4+1+1+1+i]=dt_String1[i];
   }
   else if(len>11)
   {
     for(i=0;i<(len-7);i++) dt_String1[i]= t_DialString[7+i];
     for(i=0;i<3;i++) dt_String2[i]= t_DialString[11-4-3+i];
     for(i=0;i<3;i++) dt_String3[i]= t_DialString[11-4-3-3+i];
     WSTRCPY(&t_DialString[11-4-3-3], L"-");
     WSTRCPY(&t_DialString[11+3-3-4-2], L"-");
     WSTRCPY(&t_DialString[11+3-4-1], L"-");
     for(i=0;i<3;i++)  t_DialString[11-4-3-3+1+i]=dt_String3[i];
     for(i=0;i<3;i++)  t_DialString[11-4-3+1+1+i]=dt_String2[i];
     for(i=0;i<(len-7);i++)  t_DialString[11-4+1+1+1+i]=dt_String1[i];
   }
   else
   {
   }
   //////////////////////////////////////////
   #endif //FEATURE_CARRIER_VENEZUELA_MOVILNET
   ///////////////////////////////////////////////////////////////
#ifdef FEATURE_CARRIER_ISRAEL_PELEPHONE
      int len,i;
    AECHAR t_DialString[MAX_SIZE_DIALER_TEXT];
    AECHAR dt_String1[30];
    AECHAR dt_String2[30];
    AECHAR dt_String3[30];
   (void)MEMSET( t_DialString,(AECHAR) 0, sizeof(t_DialString));
   (void)MEMSET( dt_String1,(AECHAR) 0, sizeof(dt_String1));
   (void)MEMSET( dt_String2,(AECHAR) 0, sizeof(dt_String2));
   (void)MEMSET( dt_String3,(AECHAR) 0, sizeof(dt_String3));
   WSTRCPY(t_DialString,pMe->m_DialString);
   len = WSTRLEN(pMe->m_DialString);
   if(0==WSTRNCMP(t_DialString,L"1",1))
   {
      if((1<len)&&(len<5))
      {
         for(i=0;i<len-1;i++) dt_String1[i]= t_DialString[1+i];
         WSTRCPY(&t_DialString[1], L"-");
         for(i=0;i<len-1;i++)  t_DialString[2+i]=dt_String1[i];
      }
      else if(len>=5)
      {
         for(i=0;i<3;i++) dt_String1[i]= t_DialString[1+i];
         for(i=0;i<len-4;i++) dt_String3[i]= t_DialString[4+i];
         WSTRCPY(&t_DialString[1], L"-");
         WSTRCPY(&t_DialString[5], L"-");
         for(i=0;i<3;i++)  t_DialString[2+i]=dt_String1[i];
         for(i=0;i<len-4;i++)  t_DialString[6+i]=dt_String3[i];
      }
   }
   else if(0==WSTRNCMP(t_DialString,L"0",1))
   {
       if((0==WSTRNCMP(t_DialString,L"02",2))||(0==WSTRNCMP(t_DialString,L"03",2))||(0==WSTRNCMP(t_DialString,L"04",2))||(0==WSTRNCMP(t_DialString,L"08",2))||(0==WSTRNCMP(t_DialString,L"09",2)))
       {
            if((2<len)&&(len<6))
            {
                for(i=0;i<len-2;i++) dt_String1[i]= t_DialString[2+i];
                WSTRCPY(&t_DialString[2], L"-");
                for(i=0;i<len-2;i++)  t_DialString[3+i]=dt_String1[i];
            }
            else if(len>=6)
            {
                for(i=0;i<3;i++) dt_String1[i]= t_DialString[2+i];
                for(i=0;i<len-5;i++) dt_String3[i]= t_DialString[5+i];
                WSTRCPY(&t_DialString[2], L"-");
                WSTRCPY(&t_DialString[6], L"-");
                for(i=0;i<3;i++)  t_DialString[3+i]=dt_String1[i];
                for(i=0;i<len-5;i++)  t_DialString[7+i]=dt_String3[i];
                      }
        
        }
       else if((0==WSTRNCMP(t_DialString,L"05",2))||(0==WSTRNCMP(t_DialString,L"06",2))||(0==WSTRNCMP(t_DialString,L"07",2)))
        {
            if((3<len)&&(len<7))
            {
                for(i=0;i<len-3;i++) dt_String1[i]= t_DialString[3+i];
                WSTRCPY(&t_DialString[3], L"-");
                for(i=0;i<len-3;i++)  t_DialString[4+i]=dt_String1[i];
            }
            else if(len>=7)
            {
                for(i=0;i<3;i++) dt_String1[i]= t_DialString[3+i];
                for(i=0;i<len-6;i++) dt_String3[i]= t_DialString[6+i];
                WSTRCPY(&t_DialString[3], L"-");
                WSTRCPY(&t_DialString[7], L"-");
                for(i=0;i<3;i++)  t_DialString[4+i]=dt_String1[i];
                for(i=0;i<len-6;i++)  t_DialString[8+i]=dt_String3[i];
            }
        }
       else
        {
            WSTRCPY(t_DialString,pMe->m_DialString);
        }

   }
   
   else
    {
                  
                  WSTRCPY(t_DialString,pMe->m_DialString);
     }
#endif
//////////////////////////////////////////////////////////////////
#ifdef FEATURE_IMAGE_DIALING_DIGITS
    {
        #if defined(FEATURE_CARRIER_VENEZUELA_MOVILNET)||defined(FEATURE_CARRIER_ISRAEL_PELEPHONE)
        CallApp_Draw_Numer_Img(pMe, t_DialString);
        #else
        CallApp_Draw_Numer_Img(pMe, pMe->m_DialString);
        #endif //FEATURE_CARRIER_VENEZUELA_MOVILNET
    }
#else
#ifdef FEATURE_VERSION_W208S
	CallApp_DrawDialerString(pMe, t_DialString);
#else
    CallApp_DrawDialerString(pMe, pMe->m_DialString);
#endif
#endif //#ifdef FEATURE_IMAGE_DIALING_DIGITS
}

static void CallApp_Send_DTMF(CCallApp *pMe , AECHAR *w_str )
{
    if (OEMNV_KEYTONE_LONG == pMe->m_dtmf_length)
    {
        // The tone will be stopped in CIlde_DialerHandleEvent()
        // function on the EVT_KEY_RELEASE event...
        CallApp_StartContDTMF(pMe,w_str);
    }
    else
    {
        CallApp_BurstDTMF(pMe,w_str,1);
    }
}

static void notifyFMRadioAlertEvent( CCallApp *pMe, boolean toStartAlert)
{
    /*ISHELL_SendEvent( pMe->m_pShell,
                      AEECLSID_APP_FMRADIO,
                      EVT_ALARM,
                      toStartAlert,
                      0
                 );*/
}


#ifdef FEATURE_EDITABLE_NUMBER
/*==============================================================================
函数：
       CallApp_Calc_Cursor_Rect
说明：
       计算绘制光标的矩形框

参数：
       pMe [in]：
       pRect [out]：计算出的矩形范围

返回值：

备注：

==============================================================================*/
static void CallApp_Calc_Cursor_Rect(CCallApp* pMe, AEERect *pRect)
{
#ifdef FEATURE_IMAGE_DIALING_DIGITS
    int xPos = 0, yPos = 0, xNum = 0, yNum = 0, dy = pMe->m_rc.dy;
    xNum = pMe->m_rc.dx /NUM_IMAGE_WIDTH;
    yNum = (dy - BOTTOMBAR_HEIGHT)/(NUM_IMAGE_HIGHT + BETWEEN_LINE_PIXEL);
    xPos = (pMe->m_rc.dx + NUM_IMAGE_WIDTH *xNum) /2 - (pMe->m_nCursorPos%xNum)*NUM_IMAGE_WIDTH - 2;
    yPos = dy - BOTTOMBAR_HEIGHT - (1 + pMe->m_nCursorPos/xNum)*(NUM_IMAGE_HIGHT + BETWEEN_LINE_PIXEL);
    
    if (pMe->m_nCursorPos >= xNum*yNum)
    {
        yPos = dy - BOTTOMBAR_HEIGHT - yNum*(NUM_IMAGE_HIGHT + BETWEEN_LINE_PIXEL);
    }
    if (pMe->m_nCursorPos != 0 &&
        pMe->m_nCursorPos == WSTRLEN(pMe->m_DialString) &&
        pMe->m_nCursorPos%xNum == 0)
    {
        xPos = (pMe->m_rc.dx - NUM_IMAGE_WIDTH *xNum) /2 - 2;
        if (pMe->m_nCursorPos < xNum*yNum)
        {
            yPos += (NUM_IMAGE_HIGHT + BETWEEN_LINE_PIXEL);
        }
    }
	SETAEERECT(pRect, xPos, yPos, 4, NUM_IMAGE_HIGHT);
#else    

#ifdef FEATURE_VERSION_W208S
{
	int x = 0, y = 0;	
	int length=0;
	int index1=0;
	int index2=0;	
	
	int tmplen=0;
	int tmplenex=0;

	int strlen1 = 0;
	int strlen2 = 0;

	int strexlen1 = 0;
	int strexlen2 = 0;
	
    int nLen; 
	
	int len,i;
	AECHAR t_DialString[MAX_SIZE_DIALER_TEXT];
	AECHAR dt_String1[30];
	AECHAR dt_String2[5];
	AECHAR dt_String3[5];		

	(void)MEMSET( t_DialString,(AECHAR) 0, sizeof(t_DialString));
	(void)MEMSET( dt_String1,(AECHAR) 0, sizeof(dt_String1));
	(void)MEMSET( dt_String2,(AECHAR) 0, sizeof(dt_String2));
	(void)MEMSET( dt_String3,(AECHAR) 0, sizeof(dt_String3));
	WSTRCPY(t_DialString,pMe->m_DialString);
	len = WSTRLEN(pMe->m_DialString);

	if ((3<len)&&(len<8))
	{
		for(i=0;i<3;i++) dt_String1[i]= t_DialString[len-3+i];
		WSTRCPY(&t_DialString[len-3], L"-");
		for(i=0;i<3;i++)  t_DialString[len-3+1+i]=dt_String1[i];
	}
	else if((7<len)&&((len<12)))
	{
		for(i=0;i<3;i++) dt_String1[i]= t_DialString[len-3+i];
		for(i=0;i<4;i++) dt_String2[i]= t_DialString[len-4-3+i];
		WSTRCPY(&t_DialString[len-4-3], L"-");
		WSTRCPY(&t_DialString[len+1-3], L"-");
		for(i=0;i<4;i++)  t_DialString[len-4-3+1+i]=dt_String2[i];
		for(i=0;i<3;i++)  t_DialString[len-3+1+1+i]=dt_String1[i];
	}		
	else if (len > 11)
	{		   
		for(i=0;i<(len-8);i++) dt_String1[i]= t_DialString[8+i];
		for(i=0;i<4;i++) dt_String2[i]= t_DialString[4+i];
		for(i=0;i<4;i++) dt_String3[i]= t_DialString[i];
		 
		WSTRCPY(&t_DialString[4], L"-");
		WSTRCPY(&t_DialString[4+4+1], L"-");		
		for(i=0;i<4;i++)  t_DialString[i]=dt_String3[i];
		for(i=0;i<4;i++)  t_DialString[i+4+1]=dt_String2[i];
		for(i=0;i<(len-8);i++)  t_DialString[4+4+1+1+i]=dt_String1[i];
		
	}		

	nLen = WSTRLEN(pMe->m_DialString);
	length = WSTRLEN(t_DialString);

	for (i=0; i<length; i++)
	{
		if (t_DialString[i] == L'-')
		{
			if (index1 == 0)
			{
				index1 = i;
			}				
			else if (index2 == 0)
			{
				index2 = i;
			}
		}
	}

	MSG_FATAL("***zzg CallApp_Calc_Cursor_Rect index1=%d, index2=%d***", index1, index2, 0);

	MSG_FATAL("***zzg CallApp_Calc_Cursor_Rect nLen=%d, length=%d***", nLen, length, 0);

	IFONT_MeasureText(pMe->m_pCurrNumFont, L"-", 1, pMe->m_rc.dx, NULL, &tmplen);	

    if(length > 0)
    {
        AECHAR *pSrcStr = t_DialString + length;
        int nCurrLineMaxPos = length;
        int nCurrLineMinPos = length;
		
		int nCurrPos    	= nLen-pMe->m_nCursorPos;;	
   		
		MSG_FATAL("***zzg CallApp_Calc_Cursor_Rect pMe->m_nCursorPos=%d, nCurrPos=%d***", pMe->m_nCursorPos, nCurrPos, 0);		
		
#ifdef FEATURE_LCD_TOUCH_ENABLE
		y = pMe->m_rc.dy-BOTTOMBAR_HEIGHT - 220;
#else
        y = pMe->m_rc.dy-BOTTOMBAR_HEIGHT;
#endif

		if ((index1 != 0) && (index2 != 0))
		{
			if ((nCurrPos > index1) && (nCurrPos < index2))
			{
				nCurrPos++;	
			}
			else if ((nCurrPos+1) >= index2)
			{
				nCurrPos+=2;
			}
		}
		else if ((index1 != 0) && (index2 == 0))
		{
			if (nCurrPos > index1) 
			{
				nCurrPos++;	
			}
		}		

		MSG_FATAL("***zzg CallApp_Calc_Cursor_Rect nCurrPos=%d***", nCurrPos, 0, 0);
        
        for(i=0; i<pMe->m_nCurrLine; i++)
        {
            y -= pMe->m_nCurrNumHeight;
			
            pSrcStr -= pMe->m_nCurrLineFits[i];
            nCurrLineMinPos = nCurrLineMaxPos-pMe->m_nCurrLineFits[i];	
            
			MSG_FATAL("***zzg CallApp_Calc_Cursor_Rect nCurrPos =%d, nCurrLineMinPos=%d, nCurrLineMaxPos=%d***", nCurrPos, nCurrLineMinPos, nCurrLineMaxPos);
			
            // 在此行
            if(nCurrPos > nCurrLineMinPos && nCurrPos < nCurrLineMaxPos)
            {
                IFONT_MeasureText(pMe->m_pCurrNumFont, pSrcStr+(nCurrPos-nCurrLineMinPos), nCurrLineMaxPos-nCurrPos, pMe->m_rc.dx, NULL, &x);
                x = pMe->m_rc.dx - x;					
				
                break;
            }
            else if(nCurrPos == 0 && nCurrLineMinPos == nCurrPos)
            {
                IFONT_MeasureText(pMe->m_pCurrNumFont, pSrcStr, nCurrLineMaxPos, pMe->m_rc.dx, NULL, &x);
                x = pMe->m_rc.dx - x;					
				
                break;
            }
            else if(nCurrPos == nCurrLineMaxPos)
            {
                x = pMe->m_rc.dx - 1;
                break;
            }
            
            nCurrLineMaxPos -= pMe->m_nCurrLineFits[i];
			#ifndef FEATURE_LCD_TOUCH_ENABLE
            y -= pMe->m_nCurrLineSpace;
			#endif
        }
    }
	#ifdef FEATURE_LCD_TOUCH_ENABLE
	else
	{
		x = pMe->m_rc.dx-2;
	}
	#endif
    SETAEERECT(pRect, x, y, 1, pMe->m_nCurrNumHeight);
}

#else
	int x = 0, y = 0;
    int i;
    int nLen = WSTRLEN(pMe->m_DialString);

	MSG_FATAL("***zzg CallApp_Calc_Cursor_Rect nLen=%d***", nLen, 0, 0);

    if(nLen > 0)
    {
        AECHAR *pSrcStr = pMe->m_DialString + nLen;
        int nCurrLineMaxPos = nLen;
        int nCurrLineMinPos = nLen;
        int nCurrPos    = nLen-pMe->m_nCursorPos;
        #ifdef FEATURE_LCD_TOUCH_ENABLE
		y = pMe->m_rc.dy-BOTTOMBAR_HEIGHT - 220;
		#else
        y = pMe->m_rc.dy-BOTTOMBAR_HEIGHT;
		#endif
        
        for(i=0;i<pMe->m_nCurrLine;i++)
        {
            y -= pMe->m_nCurrNumHeight;
            pSrcStr -= pMe->m_nCurrLineFits[i];
            nCurrLineMinPos = nCurrLineMaxPos-pMe->m_nCurrLineFits[i];	
            
			MSG_FATAL("***zzg CallApp_Calc_Cursor_Rect nCurrLineMinPos=%d, nCurrLineMaxPos=%d***", nCurrLineMinPos, nCurrLineMaxPos, 0);
			
            // 在此行
            if(nCurrPos > nCurrLineMinPos && nCurrPos < nCurrLineMaxPos)
            {
                IFONT_MeasureText(pMe->m_pCurrNumFont, pSrcStr+(nCurrPos-nCurrLineMinPos), nCurrLineMaxPos-nCurrPos, pMe->m_rc.dx, NULL, &x);
                x = pMe->m_rc.dx - x;
                break;
            }
            else if(nCurrPos == 0 && nCurrLineMinPos == nCurrPos)
            {
                IFONT_MeasureText(pMe->m_pCurrNumFont, pSrcStr, nCurrLineMaxPos, pMe->m_rc.dx, NULL, &x);
                x = pMe->m_rc.dx - x;
                break;
            }
            else if(nCurrPos == nCurrLineMaxPos)
            {
                x = pMe->m_rc.dx - 1;
                break;
            }
            
            nCurrLineMaxPos -= pMe->m_nCurrLineFits[i];
			#ifndef FEATURE_LCD_TOUCH_ENABLE
            y -= pMe->m_nCurrLineSpace;
			#endif
        }
    }
	#ifdef FEATURE_LCD_TOUCH_ENABLE
	else
	{
		x = pMe->m_rc.dx-2;
	}
	#endif
    SETAEERECT(pRect, x, y, 1, pMe->m_nCurrNumHeight);
#endif //FEATURE_VERSION_W208S

	
#endif
}


/*==============================================================================
函数：
       CallApp_Calc_Cursor_Rect
说明：
       绘制光标

参数：
       pMe [in]：
       pRect [in]：绘制光标的矩形框

返回值：

备注：

==============================================================================*/
static void CallApp_Draw_Cursor(CCallApp* pMe, AEERect *pRect)
{
    IDISPLAY_InvertRect(pMe->m_pDisplay, pRect);
}

/*==============================================================================
函数：
       CallApp_Set_Cursor_Blink
说明：
       设置光标闪烁

参数：
       pUser [in]：

返回值：

备注：

==============================================================================*/
static void CallApp_Set_Cursor_Blink(void* pUser)
{
    CCallApp* pMe = (CCallApp*) pUser;
    AEERect rect = {0};
    ISHELL_CancelTimer(pMe->m_pShell, CallApp_Set_Cursor_Blink, pMe);

	MSG_FATAL("***zzg CallApp_Set_Cursor_Blink***", 0, 0, 0);

    CallApp_Calc_Cursor_Rect(pMe, &rect);

//     DBGPRINTF	DBGPRINTF("***zzg CallApp_Set_Cursor_Blink rect:%d,%d,%d,%d***", rect.x, rect.y, rect.dx, rect.dy);

    if(bDrawCursor)
    {
        CallApp_Draw_Cursor(pMe, &rect);
    }
    else
    {
        Appscommon_ResetBackgroundEx(pMe->m_pDisplay, &rect, TRUE);
    }
    bDrawCursor = !bDrawCursor;
    IDISPLAY_Update(pMe->m_pDisplay);
    
    ISHELL_SetTimer(pMe->m_pShell, 600, CallApp_Set_Cursor_Blink, (void*)pMe);
}
#endif

#include "AEECard.h"
extern int CoreApp_ChangePIN(IShell *pShell, uint8 byPinID, AECHAR *pOldPIN, AECHAR *pNewPIN);
extern int CoreApp_UnblockPIN(IShell *pShell, uint8 byPinID, AECHAR *pPUK, AECHAR *pPIN);
extern int CoreApp_DisplayADN(IShell *pShell, uint16 wRecID);
static void CallApp_ProcessUIMMMIStr(CCallApp* pMe, AECHAR *pStr)
{
    int nStrlen = WSTRLEN(pStr);
    int i,j;
    AECHAR wStr1[UIM_MAX_CHV_DIGITS + 1];
    AECHAR wStr2[UIM_MAX_CHV_DIGITS + 1];
    AECHAR wStr3[UIM_MAX_CHV_DIGITS + 1];

    MEMSET(wStr1, 0, sizeof(wStr1));
    MEMSET(wStr2, 0, sizeof(wStr2));
    MEMSET(wStr3, 0, sizeof(wStr3));
    
    //     DBGPRINTF("ProcessUIMMMIStr %S %d",pStr,nStrlen);
    if(WSTRNCMP(pStr, L"**04*", 5) == 0 && nStrlen >= 20) // LenMIN=5+OldPinMin(4+'*')+NewPinMin(4+'*')+NewPinMin(4+'#') change pin1
    {
        for(i=5,j=0;i<nStrlen;i++,j++)
        {
            if(pStr[i] == '*')
            {
                i++;
                break;
            }
            wStr1[j] = pStr[i];
        }
        
        for(j=0;i<nStrlen;i++,j++)
        {
            if(pStr[i] == '*')
            {
                i++;
                break;
            }
            wStr2[j] = pStr[i];
        }
        
        for(j=0;i<nStrlen;i++,j++)
        {
            if(pStr[i] == '#')
            {
                break;
            }
            wStr3[j] = pStr[i];
        }
        
        if(WSTRCMP(wStr2, wStr3) != 0 || WSTRLEN(wStr1) == 0 || WSTRLEN(wStr2) == 0)
        {
            return;
        }
        
        CoreApp_ChangePIN(pMe->m_pShell, AEECARD_PIN1, wStr1, wStr2);
        ISHELL_CloseApplet(pMe->m_pShell, TRUE);
    }
    else if(WSTRNCMP(pStr, L"**042*", 6) == 0 && nStrlen >= 21)// LenMIN=6+OldPinMin(4+'*')+NewPinMin(4+'*')+NewPinMin(4+'#') change pin2
    {
        for(i=6,j=0;i<nStrlen;i++,j++)
        {
            if(pStr[i] == '*')
            {
                i++;
                break;
            }
            wStr1[j] = pStr[i];
        }
        
        for(j=0;i<nStrlen;i++,j++)
        {
            if(pStr[i] == '*')
            {
                i++;
                break;
            }
            wStr2[j] = pStr[i];
        }
        
        for(j=0;i<nStrlen;i++,j++)
        {
            if(pStr[i] == '#')
            {
                break;
            }
            wStr3[j] = pStr[i];
        }

        if(WSTRCMP(wStr2, wStr3) != 0 || WSTRLEN(wStr1) == 0 || WSTRLEN(wStr2) == 0)
        {
            return;
        }
        
        CoreApp_ChangePIN(pMe->m_pShell, AEECARD_PIN2, wStr1, wStr2);
        ISHELL_CloseApplet(pMe->m_pShell, TRUE);
    }
    else if(WSTRNCMP(pStr, L"**05*", 5) == 0 && nStrlen >= 20)// LenMIN=5+PUKMIN(4+'*')+NewPinMin(4+'*')+NewPinMin(4+'#') unblock pin1
    {
        for(i=5,j=0;i<nStrlen;i++,j++)
        {
            if(pStr[i] == '*')
            {
                i++;
                break;
            }
            wStr1[j] = pStr[i];
        }
        
        for(j=0;i<nStrlen;i++,j++)
        {
            if(pStr[i] == '*')
            {
                i++;
                break;
            }
            wStr2[j] = pStr[i];
        }
        
        for(j=0;i<nStrlen;i++,j++)
        {
            if(pStr[i] == '#')
            {
                break;
            }
            wStr3[j] = pStr[i];
        }

        if(WSTRCMP(wStr2, wStr3) != 0 || WSTRLEN(wStr1) == 0 || WSTRLEN(wStr2) == 0)
        {
            return;
        }
        
        CoreApp_UnblockPIN(pMe->m_pShell, AEECARD_PIN1, wStr1, wStr2);
        ISHELL_CloseApplet(pMe->m_pShell, TRUE);
    }
    else if(WSTRNCMP(pStr, L"**052*", 6) == 0 && nStrlen >= 21)// LenMIN=6+PUKMIN(4+'*')+NewPinMin(4+'*')+NewPinMin(4+'#') unblock pin2
    {
        for(i=6,j=0;i<nStrlen;i++,j++)
        {
            if(pStr[i] == '*')
            {
                i++;
                break;
            }
            wStr1[j] = pStr[i];
        }
        
        for(j=0;i<nStrlen;i++,j++)
        {
            if(pStr[i] == '*')
            {
                i++;
                break;
            }
            wStr2[j] = pStr[i];
        }
        
        for(j=0;i<nStrlen;i++,j++)
        {
            if(pStr[i] == '#')
            {
                i++;
                break;
            }
            wStr3[j] = pStr[i];
        }

        if(WSTRCMP(wStr2, wStr3) != 0 || WSTRLEN(wStr1) == 0 || WSTRLEN(wStr2) == 0)
        {
            return;
        }
        
        CoreApp_UnblockPIN(pMe->m_pShell, AEECARD_PIN2, wStr1, wStr2);
        ISHELL_CloseApplet(pMe->m_pShell, TRUE);
    }
    else if(nStrlen <= 4) // MaxID(3+'#') Read ADN
    {
        uint16 wRecID;
        for(i=0;i<nStrlen;i++)
        {
            if(pStr[i] == '#')
            {
                break;
            }
            wStr1[i] = pStr[i];
        }
        
        if(WSTRLEN(wStr1) == 0)
        {
            return;
        }
        
        WSTRTOSTR(wStr1, (char *)wStr2, sizeof(wStr2));
        wRecID = (uint16)ATOI((char *)wStr2);
        CoreApp_DisplayADN(pMe->m_pShell, wRecID);
    }
}
#ifdef FEATURE_LCD_TOUCH_ENABLE
static void CallApp_Draw_BackGround(CCallApp* pMe, AEERect * rect)
{
	IImage *pImageBg = NULL;
	AEERect oldClip = {0};
	pImageBg = ISHELL_LoadResImage(pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDB_DIALER_MAIN);
	//IImage_Draw(pImageBg, rect->x, rect->y);
    IDisplay_GetClipRect(pMe->m_pDisplay, &oldClip);
    IDisplay_SetClipRect(pMe->m_pDisplay, rect);
	IImage_Draw(pImageBg, rect->x, rect->y);
    IDisplay_SetClipRect(pMe->m_pDisplay, &oldClip);
	if(pImageBg != NULL)
    {
        IImage_Release(pImageBg);
        pImageBg = NULL;
    }
}
static void callApp_draw_pendown(CCallApp* pMe,int16 x,int16 y)
{
	AEERect rc[CALC_ITEM]= 
					  	{
					  		{0,STARTY,NUMWINDTH,NUMHEIGHT},
					  		{NUMWINDTH,STARTY,NUMWINDTH,NUMHEIGHT},
					  		{(NUMWINDTH)*2,STARTY,NUMWINDTH,NUMHEIGHT},
					  		{0,STARTY+NUMHEIGHT,NUMWINDTH,NUMHEIGHT},
					  		{NUMWINDTH,STARTY+NUMHEIGHT,NUMWINDTH,NUMHEIGHT},
					  		{(NUMWINDTH)*2,STARTY+NUMHEIGHT,NUMWINDTH,NUMHEIGHT},
					  		{0,(STARTY+(NUMHEIGHT)*2),NUMWINDTH,NUMHEIGHT},
					  		{NUMWINDTH,(STARTY+(NUMHEIGHT)*2),NUMWINDTH,NUMHEIGHT},
					  		{(NUMWINDTH)*2,(STARTY+(NUMHEIGHT)*2),NUMWINDTH,NUMHEIGHT},
					  		{0,(STARTY+(NUMHEIGHT)*3),NUMWINDTH,NUMHEIGHT},
					  		{NUMWINDTH,(STARTY+(NUMHEIGHT)*3),NUMWINDTH,NUMHEIGHT},
					  		{(NUMWINDTH)*2,(STARTY+(NUMHEIGHT)*3),NUMWINDTH,NUMHEIGHT},
					  		{0,(STARTY+(NUMHEIGHT)*4),NUMWINDTH,NUMHEIGHT},
					  		{NUMWINDTH,(STARTY+(NUMHEIGHT)*4),NUMWINDTH,NUMHEIGHT},
					  		{(NUMWINDTH)*2,(STARTY+(NUMHEIGHT)*4),NUMWINDTH,NUMHEIGHT}
					  		
					  	};
	AEEDeviceInfo devinfo;
	int nBarH ;
	int i = 0;
	int j = 0;
	
	int16 wXPos   = x;
	int16 wYPos   = y;
	IImage *image = NULL;
	MSG_FATAL("pMe->m_bShowPopMenu========%d",pMe->m_bShowPopMenu,0,0);
	pMe->m_i = -1;
	if(!pMe->m_bShowPopMenu)
	{
	MSG_FATAL("wXPos===%d,     wYPos===%d",wXPos,wYPos,0);
    for(i = 0;i<CALC_ITEM;i++)
    {
    	if(TOUCH_PT_IN_RECT(wXPos,wYPos,rc[i]))
    	{
        	image = ISHELL_LoadResImage( pMe->m_pShell,
                            AEE_APPSCOMMONRES_IMAGESFILE,
                            IDB_DIALER_SEL_1+i);
			pMe->m_i = i;
			pMe->m_bup = FALSE;
			if(image!=NULL)
			{
				MSG_FATAL("rc[i].x===%d,rc[i].y===%d,i===%d",rc[i].x,rc[i].y,i);
				IIMAGE_Draw(image,rc[i].x,rc[i].y);
				IIMAGE_Release( image);
				image = NULL;
			}
        	break;
    	}
    } 
	}
	IDISPLAY_Update(pMe->m_pDisplay);
}
static void callApp_draw_penmove(CCallApp* pMe,int16 x,int16 y)
{
	AEERect rc[CALC_ITEM]= 
					  	{
					  		{0,STARTY,NUMWINDTH,NUMHEIGHT},
					  		{NUMWINDTH,STARTY,NUMWINDTH,NUMHEIGHT},
					  		{(NUMWINDTH)*2,STARTY,NUMWINDTH,NUMHEIGHT},
					  		{0,STARTY+NUMHEIGHT,NUMWINDTH,NUMHEIGHT},
					  		{NUMWINDTH,STARTY+NUMHEIGHT,NUMWINDTH,NUMHEIGHT},
					  		{(NUMWINDTH)*2,STARTY+NUMHEIGHT,NUMWINDTH,NUMHEIGHT},
					  		{0,(STARTY+(NUMHEIGHT)*2),NUMWINDTH,NUMHEIGHT},
					  		{NUMWINDTH,(STARTY+(NUMHEIGHT)*2),NUMWINDTH,NUMHEIGHT},
					  		{(NUMWINDTH)*2,(STARTY+(NUMHEIGHT)*2),NUMWINDTH,NUMHEIGHT},
					  		{0,(STARTY+(NUMHEIGHT)*3),NUMWINDTH,NUMHEIGHT},
					  		{NUMWINDTH,(STARTY+(NUMHEIGHT)*3),NUMWINDTH,NUMHEIGHT},
					  		{(NUMWINDTH)*2,(STARTY+(NUMHEIGHT)*3),NUMWINDTH,NUMHEIGHT},
					  		{0,(STARTY+(NUMHEIGHT)*4),NUMWINDTH,NUMHEIGHT},
					  		{NUMWINDTH,(STARTY+(NUMHEIGHT)*4),NUMWINDTH,NUMHEIGHT},
					  		{(NUMWINDTH)*2,(STARTY+(NUMHEIGHT)*4),NUMWINDTH,NUMHEIGHT}
					  		
					  	};
	int16 wXPos = x;
	int16 wYPos = y;
	if(!pMe->m_bup)
	{
		if(TOUCH_PT_IN_RECT(wXPos,wYPos,rc[pMe->m_i]))
		{
			return;
		}
		else
		{
			IImage *image = NULL;
			pMe->m_i = -1;
			pMe->m_bup = TRUE;
			image = ISHELL_LoadResImage(pMe->m_pShell,
                            AEE_APPSCOMMONRES_IMAGESFILE,
                            IDB_DIALER_1+(pMe->m_i));
			if(image!=NULL)
			{
				IIMAGE_Draw(image,rc[pMe->m_i].x,rc[pMe->m_i].y);
				IIMAGE_Release(image);
				image = NULL;
			}
			switch(pMe->m_i)
			{
				case 0:
				{
					boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_USER,AVK_1,0);
				 	return ;
				}
					break;
				case 1:
				{
					boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_USER,AVK_2,0);
				 	return ;
				}
					break;
				case 2:
				{
					boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_USER,AVK_3,0);
				 	return ;
				}
					break;
				case 3:
				{
					boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_USER,AVK_4,0);
				 	return ;
				}
					break;
				case 4:
				{
					boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_USER,AVK_5,0);
				 	return ;
				}
					break;
				case 5:
				{
					boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_USER,AVK_6,0);
				 	return ;
				}
					break;
				case 6:
				{
					boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_USER,AVK_7,0);
				 	return ;
				}
					break;
				case 7:
				{
					boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_USER,AVK_8,0);
				 	return ;
				}
					break;
				case 8:
				{
					boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_USER,AVK_9,0);
				 	return ;
				}
					break;
				case 9:
				{
					boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_USER,AVK_STAR,0);
				 	return ;
				}
					break;
				case 10:
				{
					boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_USER,AVK_0,0);
				 	return ;
				}
					break;
				case 11:
				{
					boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_USER,AVK_POUND,0);
				 	return ;
				}
					break;
				case 12:
				{
					boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_USER,AVK_SELECT,0);
				 	return ;
				}
					break;
				case 13:
				{
						CallApp_Process_Send_Key_Release_Event(pMe);
				}
					break;
				case 14:
				{
					boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_USER,AVK_CLR,0);
				 	return ;
				}
					break;
				default:
					break;
			}
		}
	}
}

static void callApp_draw_penup(CCallApp* pMe,int16 x,int16 y)
{
	AEERect rc[CALC_ITEM]= 
					  	{
					  		{0,STARTY,NUMWINDTH,NUMHEIGHT},
					  		{NUMWINDTH,STARTY,NUMWINDTH,NUMHEIGHT},
					  		{(NUMWINDTH)*2,STARTY,NUMWINDTH,NUMHEIGHT},
					  		{0,STARTY+NUMHEIGHT,NUMWINDTH,NUMHEIGHT},
					  		{NUMWINDTH,STARTY+NUMHEIGHT,NUMWINDTH,NUMHEIGHT},
					  		{(NUMWINDTH)*2,STARTY+NUMHEIGHT,NUMWINDTH,NUMHEIGHT},
					  		{0,(STARTY+(NUMHEIGHT)*2),NUMWINDTH,NUMHEIGHT},
					  		{NUMWINDTH,(STARTY+(NUMHEIGHT)*2),NUMWINDTH,NUMHEIGHT},
					  		{(NUMWINDTH)*2,(STARTY+(NUMHEIGHT)*2),NUMWINDTH,NUMHEIGHT},
					  		{0,(STARTY+(NUMHEIGHT)*3),NUMWINDTH,NUMHEIGHT},
					  		{NUMWINDTH,(STARTY+(NUMHEIGHT)*3),NUMWINDTH,NUMHEIGHT},
					  		{(NUMWINDTH)*2,(STARTY+(NUMHEIGHT)*3),NUMWINDTH,NUMHEIGHT},
					  		{0,(STARTY+(NUMHEIGHT)*4),NUMWINDTH,NUMHEIGHT},
					  		{NUMWINDTH,(STARTY+(NUMHEIGHT)*4),NUMWINDTH,NUMHEIGHT},
					  		{(NUMWINDTH)*2,(STARTY+(NUMHEIGHT)*4),NUMWINDTH,NUMHEIGHT}
					  		
					  	};
	AEEDeviceInfo devinfo;
	int nBarH ;
	//AEERect rc;
	int16 wXPos = x;
	int16 wYPos = y;
	int i = 0;
	AEERect rct;
	IImage *image = NULL;
	nBarH = GetBottomBarHeight(pMe->m_pDisplay);
	MEMSET(&devinfo, 0, sizeof(devinfo));
	ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
	SETAEERECT(&rct, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);
	if(TOUCH_PT_IN_RECT(wXPos,wYPos,rct))
	{
		if(wXPos >= rct.x  && wXPos < rct.x + (rct.dx/3)*1 )
		{
			boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_USER,AVK_SELECT,0);
			 return ;
		}
		else if(wXPos >= rct.x + (rct.dx/3) && wXPos < rct.x + (rct.dx/3)*2)
		{
			boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_USER,AVK_INFO,0);
			 return ;
		}
		else if(wXPos >= rct.x + (rct.dx/3)*2 && wXPos < rct.x + (rct.dx/3)*3)
		{						
			 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_USER,AVK_CLR,0);
			 return ;
		}
	}
	MSG_FATAL("EVT_PEN_UP..................",0,0,0);
	if(!pMe->m_bup)
	{
	pMe->m_bup = TRUE;
	for(i = 0;i<CALC_ITEM;i++)
    {
    	if(TOUCH_PT_IN_RECT(wXPos,wYPos,rc[i]))
    	{

        	image = ISHELL_LoadResImage(pMe->m_pShell,
                            AEE_APPSCOMMONRES_IMAGESFILE,
                            IDB_DIALER_1+i);
			if(image!=NULL)
			{
				IIMAGE_Draw(image,rc[i].x,rc[i].y);
				IIMAGE_Release(image);
				image = NULL;
			}
			switch(i)
			{
				case 0:
				{
					boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_USER,AVK_1,0);
				 	return ;
				}
					break;
				case 1:
				{
					boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_USER,AVK_2,0);
				 	return ;
				}
					break;
				case 2:
				{
					boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_USER,AVK_3,0);
				 	return ;
				}
					break;
				case 3:
				{
					boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_USER,AVK_4,0);
				 	return ;
				}
					break;
				case 4:
				{
					boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_USER,AVK_5,0);
				 	return ;
				}
					break;
				case 5:
				{
					boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_USER,AVK_6,0);
				 	return ;
				}
					break;
				case 6:
				{
					boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_USER,AVK_7,0);
				 	return ;
				}
					break;
				case 7:
				{
					boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_USER,AVK_8,0);
				 	return ;
				}
					break;
				case 8:
				{
					boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_USER,AVK_9,0);
				 	return ;
				}
					break;
				case 9:
				{
					boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_USER,AVK_STAR,0);
				 	return ;
				}
					break;
				case 10:
				{
					boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_USER,AVK_0,0);
				 	return ;
				}
					break;
				case 11:
				{
					boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_USER,AVK_POUND,0);
				 	return ;
				}
					break;
				case 12:
				{
					boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_USER,AVK_SELECT,0);
				 	return ;
				}
					break;
				case 13:
				{
						CallApp_Process_Send_Key_Release_Event(pMe);
						pMe->m_bShowPopMenu = TRUE;
				}
					break;
				case 14:
				{
					boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_USER,AVK_CLR,0);
				 	return ;
				}
					break;
				default:
					break;
			}
    	}
	}
	}
	else
	{
		(void) ISHELL_PostEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_USER_REDRAW,0,0);
	}
}

#endif

//Add By zzg 2011_11_09
#ifdef FEATURE_SUPPORT_BT_AUDIO
static boolean CallApp_CheckBtHeadSetInUsing(CCallApp* pMe)
{
	boolean result = FALSE;
	uisnd_notify_data_s_type sndInfo;
	uisnd_get_device(&sndInfo);
	MSG_FATAL("***zzg CallApp_CheckBtHeadSetInUsing - dev=%d sMute=%d mMute=%d***", 
	  			sndInfo.out_device, sndInfo.speaker_mute, sndInfo.microphone_mute);	

	if (SND_DEVICE_BT_HEADSET == sndInfo.out_device)
	{
		result = TRUE;
	}

	return result;
}

void CallApp_SwitchCallAudio(CCallApp *pMe, boolean bIsBtAudio)
{
    AEESoundInfo                soundStuff;
    boolean                     headsetPresent =FALSE;
#ifndef FEATURE_ICM
	AEETCalls po;
#endif

	//Add By zzg 2011_10_25
	uisnd_notify_data_s_type sndInfo;
	uisnd_get_device(&sndInfo);
	MSG_FATAL("***zzg CallApp_SetupCallAudio - dev=%d sMute=%d mMute=%d***", 
	  			sndInfo.out_device, sndInfo.speaker_mute, sndInfo.microphone_mute);	
	//Add End

	//Disconnect BTHeadSet

	//Add By zzg 2012_12_08
	if (bIsBtAudio == FALSE)
	{
		(void) ISHELL_PostEvent(pMe->m_pShell,
							AEECLSID_BLUETOOTH_APP,
							EVT_USER,
							EVT_CALLAPP_DISCONNECT_FROM_BT,	
							0);
	}
	//Add End

    if (SUCCESS != ICONFIG_GetItem(pMe->m_pConfig,CFGI_HEADSET_PRESENT,&headsetPresent,sizeof(headsetPresent)))
    {
        headsetPresent = FALSE;
    }

	if (headsetPresent)
    {
      soundStuff.eDevice = AEE_SOUND_DEVICE_STEREO_HEADSET; 
    }	
    else if(pMe->m_bHandFree)
    {
#ifdef FEATURE_SPEAKER_PHONE
        soundStuff.eDevice = AEE_SOUND_DEVICE_SPEAKER;
#else
        soundStuff.eDevice = AEE_SOUND_DEVICE_HFK;
#endif
    }	
	else if (TRUE == bIsBtAudio)
	{
		soundStuff.eDevice = AEE_SOUND_DEVICE_BT_HEADSET;
	}
    else
    {
        soundStuff.eDevice = AEE_SOUND_DEVICE_HANDSET;
    }


	MSG_FATAL("***zzg CallApp_SwitchCallAudio eDevice=%d***", soundStuff.eDevice, 0, 0);

    soundStuff.eMethod = AEE_SOUND_METHOD_VOICE;
    soundStuff.eAPath = AEE_SOUND_APATH_BOTH;
    
#ifdef FEATURE_ICM
    if(AEECM_IS_VOICECALL_CONNECTED(pMe->m_pICM) )
#else
	if(SUCCESS != ITELEPHONE_GetCalls(pMe->m_pITelephone, &po,sizeof(AEETCalls)))
	{
		return ;
	}

    if(po.dwCount>0)
#endif
    {
        soundStuff.eEarMuteCtl = AEE_SOUND_MUTECTL_UNMUTED;
        if (pMe->m_CallMuted)
        {
            soundStuff.eMicMuteCtl = AEE_SOUND_MUTECTL_MUTED;
        }

        else
        {
            soundStuff.eMicMuteCtl = AEE_SOUND_MUTECTL_UNMUTED;
        }
    }

    else
    {
        soundStuff.eEarMuteCtl = AEE_SOUND_MUTECTL_MUTED;
        soundStuff.eMicMuteCtl = AEE_SOUND_MUTECTL_MUTED;
    }

    (void) ISOUND_Set(pMe->m_pSound,  &soundStuff);

    ISOUND_SetDevice(pMe->m_pSound);
    if(pMe->m_CallVolume == OEMSOUND_MUTE_VOL)
    {
        pMe->m_CallVolume = OEMSOUND_1ST_VOL;
    }
    ISOUND_SetVolume(pMe->m_pSound,
                     GET_ISOUND_VOL_LEVEL(pMe->m_CallVolume));
}
#endif

//Add End



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

//#define FEATURE_TEST_ANNUN_ICONS 
#ifdef FEATURE_TEST_ANNUN_ICONS
    #include "oemannunciator.brh" 
#endif

/*==============================================================================
                                 宏定义和常数
==============================================================================*/
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

static void  CallApp_SetPauseControl(void *pUser);

static void CallApp_ConvReleasePKey(CCallApp *pMe);

static void CallApp_HandleStopMissedAlertTimer(void *pUser);

static void CallApp_ShortcutQuiet(CCallApp *pMe);
static void CallApp_Dialer_Show_Animation(void *pUser);

static void CallApp_Flash_Call(CCallApp *pMe);

static void CallApp_StartContDTMF(CCallApp *pMe,AECHAR *c_dtmf);

static void CallApp_BurstDTMF(CCallApp *pMe,AECHAR *dtmf,int len);

static void CallApp_Stop_ContDTMF(ICM *m_pICM);

static boolean CallApp_NotifyDisp(CCallApp *pMe);

static boolean CallApp_Search_HasExceptionalChar(AECHAR* szCallNumber);

static void CallApp_Set_Db_In_Idle(boolean val);

//static void CallApp_Draw_Softkey(CCallApp *pMe,uint16 lsk,uint16 rsk);

static boolean CallApp_Process_HeldKey_Event(CCallApp *pMe,
                                           AEEEvent    eCode,
                                           uint16      wParam,
                                           uint32      dwParam);

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

static boolean CallApp_SendSMS(CCallApp *pMe);
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
#ifdef FEATURE_TCL_CDG2_TEST
        uint16 id = 0;
        id = IDIALOG_GetID(ISHELL_GetActiveDialog(pMe->m_pShell));
        CALL_ERR("%d Trying to create a dialog without closing the previous one",id,0,0);
        if(id!= 0)
        {
            return;
        }
#else
        CALL_ERR("Trying to create a dialog without closing the previous one",0,0,0);
        return;
#endif
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

        case IDD_MSGBOX:
            return CallApp_MsgBox_DlgHandler(pMe,eCode,wParam,dwParam);

        case IDD_INCOMINGCALL:
            return CallApp_IncomingCall_DlgHandler(pMe,eCode,wParam,dwParam);

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
    //PARAM_NOT_REF(dwParam)
    //keyToneLength dtmflen;
    boolean vol_add =FALSE;
    static byte keyBeepVolumeSetting = OEMSOUND_MUTE_VOL;
    CALL_ERR("eCode= %x,w=%x,dw=%x CallApp_Dialer_NumEdit_DlgHandler ",eCode,wParam,dwParam);


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
            pMe->m_btime_out     = 0;
            pMe->m_return_value  = RETURN_ZERO;
            pMe->m_bShowPopMenu  = FALSE;
            pMe->m_bprocess_held = FALSE;
            //ICONFIG_GetItem(pMe->m_pConfig,  CFGI_KEYTONE_LENGTH,&dtmflen,sizeof(keyToneLength));
            ISHELL_CancelTimer(pMe->m_pShell, (PFNNOTIFY)CallApp_MakeCall, pMe);
            pMe->m_b_auto_redial = FALSE;
            pMe->m_auto_redial_count = 0;
            pMe->m_b_incall      = AEECM_IS_VOICECALL_CONNECTED(pMe->m_pICM);
#ifdef FEATURE_EDITABLE_NUMBER
            pMe->m_nCursorPos = 0;
#endif

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
            else
            {
                byte mute = OEMSOUND_MUTE_VOL;
                ICONFIG_GetItem( pMe->m_pConfig, CFGI_BEEP_VOL, &keyBeepVolumeSetting, sizeof(byte));
                ICONFIG_SetItem( pMe->m_pConfig, CFGI_BEEP_VOL, &mute, sizeof(byte));
                DBGPRINTF( ";---numedit init, beep, %d", keyBeepVolumeSetting);
            }
            
#ifdef FEATURE_IMAGE_DIALING_DIGITS
            CallApp_Load_Numer_Img(pMe);
#endif/*FEATURE_IMAGE_DIALING_DIGITS*/
            return TRUE;

        case EVT_DIALOG_START:

            if (WSTRLEN(pMe->m_DialString) > 0) //copy from the start info
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

            Appscommon_ResetBackgroundEx(pMe->m_pDisplay, &pMe->m_rc, TRUE);
            
            CallApp_Draw_NumEdit_SoftKey(pMe);
            CallApp_Display_Number(pMe);

#ifdef WIN32//wlh for virtualkey
			IVKEYCTL_SetKeyPad(pMe->m_pIVkeyCtl, &pMe->m_rc, AEE_VKEY_DIALNUMBER);
			IVKEYCTL_SetActive(pMe->m_pIVkeyCtl, TRUE);			
#endif
			IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
#if defined( FEATURE_CALL_RECORDER)
        if( pMe->m_bRecorderOn && !pMe->m_bShowPopMenu)
        {
        	recorder_show_media_spec( pMe, RGB_WHITE);
        }

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
            // TBD - dial string format should be typedef'd
#ifdef KEYSND_ZY
            ISOUNDPLAYER_Stop(pMe->m_SndPlayer);
#endif /* KEYSND_ZY */

            // 防止有来电时，POP menu还是active
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
            if( pMe->m_b_incall)
            {
                ICONFIG_SetItem( pMe->m_pConfig, CFGI_BEEP_VOL, &keyBeepVolumeSetting, sizeof(byte));
                DBGPRINTF( ";---numedit edn, beep, %d", keyBeepVolumeSetting);
            }
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
            return TRUE;

        case EVT_COMMAND:
            //CALL_ERR("EVT_COMMAND",0,0,0);
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
                     case IDS_MUTE:
                     case IDS_UNMUTE:
                         pMe->m_CallMuted = !pMe->m_CallMuted;
                         CallApp_SetupCallAudio(pMe);
                         CallApp_Draw_Connect_Softkey(pMe);
                         return TRUE;

                     case IDS_SK_MENU:
                        CallApp_LaunchApplet(pMe,  AEECLSID_MAIN_MENU);
                        return TRUE;
                    
                     case IDS_SAVE_NEW:
                        if(pMe->m_b_incall && WSTRLEN(pMe->m_DialString) > (MAX_SIZE_DIAL_STR- 1))
                        {
                            CLOSE_DIALOG(DLGRET_NUM_TOO_LONG);
                            return TRUE;
                        }
                        else
                        {
                            return CallApp_SaveNumber(pMe, SAVE_NUMBER);
                        }

                      case IDS_SAVE_TO_CONT:
                          return CallApp_SaveNumber(pMe, ADD_FIELD);
                        return TRUE;
                        
                      case IDS_SEND_MESSAGE:
                        return CallApp_SendSMS(pMe);
                        
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
                DBGPRINTF("EVT_KEY_HELD Dialer NumEdit");
                (void) ISHELL_SetTimer(pMe->m_pShell,
                                1000,
                                Morse_DialogTimeout,
                                pMe); 
            }
            #endif
            return TRUE;

        
#ifdef FEATURE_MORSE
        case EVT_DISPLAYDIALOGTIMEOUT:
            ISHELL_CancelTimer(pMe->m_pShell,Morse_DialogTimeout,(void*)pMe);
            CLOSE_DIALOG(DLGRET_OK);
            return TRUE;
#endif
        case EVT_KEY_RELEASE:

            if(AVK_SEND == (AVKType)wParam && !pMe->m_bprocess_held)
            {
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
                            CallApp_Stop_ContDTMF(pMe->m_pICM);
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
                                ICM_SetOperatingMode(pMe->m_pICM, AEECM_OPRT_MODE_OFFLINE);
                                ICM_SetOperatingMode(pMe->m_pICM, AEECM_OPRT_MODE_RESET);
                            }
                            return TRUE;
                        }
                        else
#endif
                      {
#ifdef FATRUE_LOCK_IMSI_MCCMNC
                       AECHAR tempStr1[MAX_SIZE_GETIMSI];
                       AECHAR tempStr2[MAX_SIZE_GETIMSI];
                       AECHAR tempStr4[MAX_SIZE_GETIMSI];
                       AECHAR tempStr5[MAX_SIZE_GETIMSI];
                       AECHAR tempStr6[MAX_SIZE_GETIMSI];
                       AECHAR tempStr7[MAX_SIZE_GETIMSI];
                       uint16 tempStr3[MAX_SIZE_GETIMSI];
                       SetImsi List[OEMNV_MAX_SERVICEPROVIDER_NUMBER];
                       uint16 mcc;
                       uint16 mnc;
#endif
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
                        if (WSTRCMP(pMe->m_DialString, L"*#*#8378#0#") == 0)
                        {
                            //return CallApp_LaunchApplet(pMe,  AEECLSID_FIELDDEBUGAPP);
                            ISHELL_StartAppletArgs(pMe->m_pShell, AEECLSID_FIELDDEBUGAPP, "*#*#8378#0#");
                            return TRUE;
                        }                        
#endif
                        if (WSTRCMP(pMe->m_DialString, L"123456#") == 0)
                        {
                            return CallApp_LaunchApplet(pMe,  AEECLSID_APPMANAGER);
                        }
                        else if (WSTRCMP(pMe->m_DialString, L"*#*#8378#1#") == 0)
                        {
                            return CallApp_LaunchApplet(pMe, AEECLSID_QUICKTEST);
                        }
#ifdef FATRUE_LOCK_IMSI_MCCMNC	 
                        else  if (WSTRNCMP(pMe->m_DialString, L"*953047*",8) == 0)
                        {
                            WSTRLCPY(tempStr1,pMe->m_DialString,sizeof("*953047*"));
                            WSTRLCPY((AECHAR*)tempStr2,pMe->m_DialString+8,3);	
                            WSTRLCPY((AECHAR*)tempStr4,pMe->m_DialString+9,3);
                            WSTRLCPY((AECHAR*)tempStr5,pMe->m_DialString+10,3);
                            WSTRLCPY((AECHAR*)tempStr6,pMe->m_DialString+11,3);
                            WSTRLCPY((AECHAR*)tempStr7,pMe->m_DialString+12,3);   

                            tempStr3[0] = (uint16)ATOI((char*)tempStr2);
                            tempStr3[1] = (uint16)ATOI((char*)tempStr4);
                            tempStr3[2] = (uint16)ATOI((char*)tempStr5);

                            tempStr3[3] = (uint16)ATOI((char*)tempStr6);
                            tempStr3[4] = (uint16)ATOI((char*)tempStr7);

                            mcc = tempStr3[0]*100 + tempStr3[1]*10 + tempStr3[2];
                            mnc = tempStr3[3]*10 + tempStr3[4];
                            //MSG_ERROR("99999999999999999%d,%d,%d", tempStr3[0] , tempStr3[1] , tempStr3[2] );
                            //MSG_ERROR("99999999999999999%d,%d",mcc,mnc,0);
       
                            List[0].mcc = mcc;
                            List[0].mnc = mnc;
                            List[0].bsetimsi = TRUE;
                            //MSG_ERROR("99999999999999999%d,%d", List[0].mcc, List[0].bsetimsi,0);
                            (void) ICONFIG_SetItem(pMe->m_pConfig, 
                                              CFGI_IMSI_SETMCC,
                                              (void*)List,
                                              sizeof(SetImsi) * OEMNV_MAX_SERVICEPROVIDER_NUMBER);

                        }
                        else if (WSTRCMP(pMe->m_DialString, L"*953048#") == 0)
                        {
                            List[0].bsetimsi = FALSE;
                            (void) ICONFIG_SetItem(pMe->m_pConfig, 
                                       CFGI_IMSI_SETMCC,
                                       (void*)List,
                                        sizeof(SetImsi) * OEMNV_MAX_SERVICEPROVIDER_NUMBER);	
                            //MSG_ERROR("99999999999999999%d,%d", List[0].mcc, List[0].bsetimsi,0);
                        }
#endif

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
                            pIBitmap = ISHELL_LoadResBitmap(pMe->m_pShell, "fs:/shared/oemannunciator.bar", IDB_BATT_LOW);
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
                                pIBitmap = ISHELL_LoadResBitmap(pMe->m_pShell, "fs:/shared/oemannunciator.bar", IDB_BATT_LOW + i);
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
                        else
                        {
                            //don't process
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
            if ( pMe->m_bShowPopMenu && (AVK_LEFT != wParam)&& (AVK_RIGHT!= wParam))
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
                        if(!(CallApp_IsEmergencyMode(pMe->m_pICM)||pMe->idle_info.uimLocked))
                        {
                            pMe->m_b_from_numedit = TRUE;
                            CLOSE_DIALOG(DLGRET_SEL_IP_NUMBER)
                        }
                    }
                    else
#endif /*FEATRUE_SET_IP_NUMBER*/
                    {
                        if((!pMe->m_bShowPopMenu) && (!pMe->m_b_incall))
                        {
                            return CallApp_SaveNumber(pMe, SAVE_NUMBER);
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
                    return TRUE;

                case AVK_SELECT:
                    //CALL_ERR("AVK_SELECT %d",pMe->m_bShowPopMenu,0,0);
                    if(!pMe->m_bShowPopMenu)
                    {
#ifdef FEATURE_EDITABLE_NUMBER
                        ISHELL_CancelTimer(pMe->m_pShell, CallApp_Set_Cursor_Blink, pMe);//added by chengxiao 2009.03.16
#endif
                        CallApp_Build_NumEdit_Option_Menu ( pMe,pMe->m_pMenu );
                        pMe->m_bShowPopMenu = TRUE;
                        //CallApp_Draw_NumEdit_SoftKey( pMe );
                    }
                    return TRUE;

                //case AVK_UP:
                //case AVK_DOWN:
                //    break;

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
                    char   szStr[2];
                    AECHAR wStr[2];

                    if ((dwParam & KB_AUTOREPEAT) != 0 && (AVKType)wParam != AVK_0&& (AVKType)wParam != AVK_STAR&& (AVKType)wParam != AVK_POUND)
                    {
                        CALL_ERR("OK,it is repeat,don't process it ",0,0,0);
                        return TRUE;
                    }

                    szStr[0] = CallApp_AVKType2ASCII((AVKType)wParam);
                    szStr[1] = '\0';
                    STR_TO_WSTR(szStr, wStr, sizeof(wStr));

                    if((AVKType)wParam == AVK_POUND||(AVKType)wParam == AVK_STAR|| (AVKType)wParam == AVK_0)
                    {
                        CallApp_Process_Spec_Key_Event(pMe,wParam);
                    }
                    else
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
#ifdef FEATURE_EDITABLE_NUMBER
                            if (len < (int)( ARRAY_SIZE(pMe->m_DialString) - 1))
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
                    return TRUE;
                }

                case AVK_CLR:
                {
                    int len;
                    len = WSTRLEN(pMe->m_DialString);
#ifdef FEATURE_EDITABLE_RECORD
                   if(pMe->m_bEditRecNumber)
                   {
                       if(len == 0)
                       {
                           CLOSE_DIALOG(DLGRET_OK)
                           return TRUE;
                       }
                   }
                   else if (len <= 1)
#else
                    if (len <= 1)
#endif
                    {
                       // Clearing the last digit exits the dialog
                       pMe->m_DialString[0] = 0;

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
                    if(pMe->m_b_incall )
                    {
                        ICM_EndAllCalls(pMe->m_pICM);
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
#endif

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
            IMENUCTL_SetTitle(pMenu,
                                        AEE_APPSCALLAPP_RES_FILE,
                                        IDS_IP_NUMBER_SELECT,
                                        NULL);
            IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
            ISHELL_PostEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_USER_REDRAW,0,0);
            return TRUE;

        case EVT_USER_REDRAW:
            
            IMENUCTL_Redraw(pMenu);
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
            if(pMe->m_pConvImage)
            {
                IIMAGE_Release(pMe->m_pConvImage);
                pMe->m_pConvImage = NULL;
            }
            {
                byte mute = OEMSOUND_MUTE_VOL;
                ICONFIG_GetItem( pMe->m_pConfig, CFGI_BEEP_VOL, &keyBeepVolumeSetting, sizeof(byte));
                ICONFIG_SetItem( pMe->m_pConfig, CFGI_BEEP_VOL, &mute, sizeof(byte));
                DBGPRINTF( ";---calling init, beep, %d", keyBeepVolumeSetting);
            }
            return TRUE;

        case EVT_DIALOG_START:
        {
            byte/*AEECMPrivacyPref*/ privacy_pref = AEECM_PRIVACY_PREF_NONE;

            //CallApp_Set_Db_In_Idle(TRUE);
            
            (void) ICONFIG_GetItem(pMe->m_pConfig,CFGI_VOICEPRIVACY,&privacy_pref,sizeof(byte));
            CALL_ERR("privacy_pref = %d",privacy_pref,0,0);
            if((AEECMPrivacyPref)privacy_pref == AEECM_PRIVACY_PREF_ENHANCED )
            {
                ICM_SetPrivacyPref(pMe->m_pICM,(AEECMPrivacyPref)privacy_pref);
                //pMe->m_bIsPrivacy = TRUE;
            }
            //else if ((AEECMPrivacyPref)privacy_pref == AEECM_PRIVACY_PREF_STANDARD)
            //{
            //    pMe->m_bIsPrivacy =  FALSE;
            //}
            
            ISHELL_PostEvent(pMe->m_pShell, AEECLSID_DIALER, EVT_USER_REDRAW, 0,0);
#ifdef FEATURE_SUPPORT_BT_APP
            bt_ui_process_cmcall_notify(pMe,AEECM_EVENT_CALL_ORIG, FALSE);
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
                DBGPRINTF("calling Call_Table == NULL",0,0,0);
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
            //DRAW NAME
            if(pMe->m_CallsTable->in_phonebook && !b_cdg)
            {
                /*预留出动画的宽度*/
                SETAEERECT(&rect,
                                                        CALL_NAME_X,
                                                        CALL_SECOND_LINE_Y,
                                                        CALL_NAME_DX,
                                                        CALL_LINE_HIGHT);
                CallApp_DrawText_Ex(pMe, AEE_FONT_NORMAL,
                    name, &rect, IDF_ALIGN_LEFT | IDF_TEXT_TRANSPARENT);
            }
            //DRAW NUMBER
            if(pMe->m_cdg_dsp_info.pi== AEECM_PI_ALLOW)
            {
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
                CallApp_DrawText_Ex(pMe, AEE_FONT_NORMAL,
                        pMe->m_CallsTable->call_number, &rect, IDF_ALIGN_LEFT |IDF_TEXT_TRANSPARENT);
            }
            //DRAW SOFTKEY
            //CallApp_Draw_Softkey(pMe, 0, IDS_CANCEL);
            //drawBottomBar(pMe->m_pDisplay, AEE_FONT_NORMAL,BTBAR_END);
            REFUI_DRAW_BOTTOMBAR(BTBAR_CANCEL)
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
                if(pMe->m_pCallingImage)
                {
                    IIMAGE_Release(pMe->m_pCallingImage);
                    pMe->m_pCallingImage = NULL;
                }

           #if FEATURE_DIALER_ANIMAION_SUPPORT
                if(pMe->m_pCallingImage == NULL)
                {
                    pMe->m_pCallingImage = ISHELL_LoadImage(pMe->m_pShell, CALLAPP_CALLOUT_ANI);
                }
                if(pMe->m_pCallingImage)
                {
                    IIMAGE_SetParm(pMe->m_pCallingImage, IPARM_NFRAMES, CALLAPP_ANI_FRAMES, 0);
                }
                pMe->m_b_draw_dot = TRUE;
            #endif
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
            if(pMe->m_pConvImage)
            {
                IIMAGE_Release(pMe->m_pConvImage);
                pMe->m_pConvImage = NULL;
            }
            if(pMe->m_pCallingImage)
            {
                IIMAGE_Release(pMe->m_pCallingImage);
                pMe->m_pCallingImage = NULL;
            }
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
                    ICM_EndAllCalls(pMe->m_pICM);
                    pMe->m_userCanceled = TRUE;
                    switch (pMe->m_lastCallState)
                    {
                        case AEECM_CALL_STATE_ORIG:
                            // Note:
                            // We are allowing the IPHONE notifier to move us to the
                            // next state...
                            break;

                        default:
                            //CALL_ERR("Unexpected phone state (%d)", pMe->m_lastCallState, 0, 0);
                            /*lint -fallthrough*/

                        case AEECM_CALL_STATE_IDLE:
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
    AECHAR    wBuf[20] ={0};
    static byte calling_coute = 0;
    CCallApp *pMe = (CCallApp *)pUser;
    AEEImageInfo ImageSize;
    int            nCurrentFrame = 0;


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

#if FEATURE_DIALER_ANIMAION_SUPPORT
    if(pMe->m_pCallingImage != NULL)
    {
        IIMAGE_GetInfo(pMe->m_pCallingImage, &ImageSize);
        SETAEERECT(&rect,
        			(pMe->m_rc.dx - ImageSize.cxFrame)/2+12,
                    CALL_ANIM_IMG_Y - CALL_LINE_HIGHT + MAX_COUNT_TO_CHANGE,
                    ImageSize.cx,
                    ImageSize.cy);
        
        Appscommon_ResetBackgroundEx(pMe->m_pDisplay, &rect, TRUE);  // 不让刷第二次背景图
        
        nCurrentFrame = (calling_coute >= CALLAPP_ANI_FRAMES)?(CALLAPP_ANI_FRAMES - 1):(calling_coute);
        IIMAGE_DrawFrame(pMe->m_pCallingImage, 
                                    nCurrentFrame, 
                                    (pMe->m_rc.dx - ImageSize.cxFrame)/2+12, 
                                    CALL_ANIM_IMG_Y - CALL_LINE_HIGHT);
    }
	
#endif	
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
		recorder_process_media_operation_result( &pme->m_Media, recorder_record(  &pme->m_Media), MEDIA_STATE_READY);
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
				ref->m_rc.dy - GetBottomBarHeight( ref->m_pDisplay) - height - 2,
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
				ref->m_rc.dy - GetBottomBarHeight( ref->m_pDisplay),
				ref->m_rc.dx,
				GetBottomBarHeight( ref->m_pDisplay)
			);
		drawText3( ref, &rect, AEE_APPSCALLAPP_RES_FILE, resId, color);
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
    static byte keyBeepVolumeSetting = OEMSOUND_MUTE_VOL;
    PARAM_NOT_REF(dwParam)
    CALL_ERR("eCode= %x,w=%x,dw=%x CallApp_Dialer_Connect_DlgHandler ",eCode,wParam,dwParam);

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
#if defined( FEATURE_RECORDER)
        	CallApp_SetupCallAudio(pMe);
#endif
            pMe->m_cdg_row = 0;
            //pMe->m_b_show_cdg = TRUE;
            pMe->m_dtmf_length = OEMNV_KEYTONE_NORMAL;
            ICONFIG_GetItem(pMe->m_pConfig,CFGI_KEYTONE_LENGTH,&pMe->m_dtmf_length,sizeof(keyToneLength));
            if(pMe->m_pConvImage)
            {
                IIMAGE_Release(pMe->m_pConvImage);
                pMe->m_pConvImage = NULL;
            }
            if(pMe->m_pActiveDlg)
            {
                IDIALOG_SetProperties(pMe->m_pActiveDlg,DLG_NOT_SET_FOCUS_AUTO);
            }


            {
                byte mute = OEMSOUND_MUTE_VOL;
                ICONFIG_GetItem( pMe->m_pConfig, CFGI_BEEP_VOL, &keyBeepVolumeSetting, sizeof(byte));
                ICONFIG_SetItem( pMe->m_pConfig, CFGI_BEEP_VOL, &mute, sizeof(byte));
            }
            return TRUE;

        case EVT_DIALOG_START:
          /*  if (pMe->m_PauseString[0] != 0)
            {
                (void)ISHELL_SetTimer(pMe->m_pShell, CALL_TIMER_SET + 8000,
                                                        CallApp_SetPauseControl, pMe);
            }*/  
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

        case EVT_USER_REDRAW:
        {
#if defined( FEATURE_CALL_RECORDER)
        	if( pMe->m_bShowPopMenu)
        	{
        		return TRUE;
        	}
#endif
            
            Appscommon_ResetBackgroundEx(pMe->m_pDisplay, &pMe->m_rc, TRUE);

            //for CDG test, CNAP with Forwarding
            
            CallApp_Draw_Connect_Softkey(pMe);
            CallApp_Draw_Connect_Number_and_Name(pMe);
            CallApp_Draw_Connect_Time(pMe);
            
#if FEATURE_DIALER_ANIMAION_SUPPORT
            if(pMe->m_pCallingImage == NULL)
            {
#if !defined(FEATURE_CARRIER_CHINA_VERTU)
                if(pMe->m_call_info.call_type == AEECALLHISTORY_CALL_TYPE_TO)
                {
                    pMe->m_pCallingImage = ISHELL_LoadImage(pMe->m_pShell, CALLAPP_CALLOUT_ANI);
                }
                else
#endif
                {
                    pMe->m_pCallingImage = ISHELL_LoadImage(pMe->m_pShell, CALLAPP_CALLIN_ANI);
                }
            }
            if(pMe->m_pCallingImage)
            {
                AEEImageInfo ImageSize;
                IIMAGE_SetParm(pMe->m_pCallingImage, IPARM_NFRAMES, CALLAPP_ANI_FRAMES, 0); //added by chengxiao 2009.04.13
                IIMAGE_GetInfo(pMe->m_pCallingImage, &ImageSize);
                IIMAGE_DrawFrame(pMe->m_pCallingImage, 
#ifdef FEATURE_CARRIER_CHINA_VERTU
                                    0, 
#else
                                    3, 
#endif
                                    (pMe->m_rc.dx - ImageSize.cxFrame)/2, 
                                    CALL_ANIM_IMG_Y - CALL_LINE_HIGHT);
            }
#endif //FEATURE_DIALER_ANIMAION_SUPPORT

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


            return TRUE;
        }

        case EVT_DIALOG_END:
          //  (void)ISHELL_CancelTimer(pMe->m_pShell, CallApp_SetPauseControl, pMe);
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
            if(pMe->m_pConvImage)
            {
                IIMAGE_Release(pMe->m_pConvImage);
                pMe->m_pConvImage = NULL;
            }
            if(pMe->m_pCallingImage)
            {
                IIMAGE_Release(pMe->m_pCallingImage);
                pMe->m_pCallingImage = NULL;
            }
            ICONFIG_SetItem( pMe->m_pConfig, CFGI_BEEP_VOL, &keyBeepVolumeSetting, sizeof(byte));
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
                    vol_add = TRUE;

                case AVK_DOWN:
                case AVK_VOLUME_DOWN:
                    CallApp_ChangeCallVolume(pMe, vol_add);

#if !defined( FEATURE_CALL_RECORDER)
					CallApp_RefreshVolBar(pMe);
					IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
#endif
                    return TRUE;


                case AVK_SEND:
                    return TRUE;  //make the cm 2312 disappear

                default:
                    break;
            }

            break;
        }
#ifdef FEATURE_TCL_CDG2_TEST
        case EVT_KEY_HELD:
            if(AVK_SEND == (AVKType)wParam)//CDG 3-way call need send fwi 
            {
                AEECMCallID nCallID = 0;
                if(CheckAEEReturnStatus(ICM_OriginateVoiceCall(pMe->m_pICM, NULL, &nCallID)) == FALSE)
                {
                    CALL_ERR("ICM_OriginateVoiceCall FAILED", 0, 0, 0);
                    return FALSE;
                }
            }
            break;
#endif
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
                    pMe->m_bHandFree = !pMe->m_bHandFree;
                    CallApp_SetupCallAudio(pMe);
                    ISHELL_PostEvent( pMe->m_pShell, AEECLSID_DIALER,EVT_USER_REDRAW,0,0 );
                    return TRUE;  //make the dialog can't closed by avk_clr.

                case AVK_ENDCALL:
                    //CALL_ERR("AVK_ENDCALL", 0,0,0);
                    // End call
                    //ASSERT(AEECM_CALL_STATE_CONV == pMe->m_lastCallState);
                    pMe->m_userCanceled = TRUE;
                    ICM_EndAllCalls(pMe->m_pICM);

                    // Note:
                    // We are allowing the IPHONE notifier to move us to the
                    // next state...
                    return TRUE;

                case AVK_SEND:
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
                        IMenuCtl *pSkMenu;

                        pMe->m_bAnswerHold = FALSE;
                        CallApp_Draw_Connect_Softkey(pMe);
                    }
#endif /* FEATURE_IS2000_SCC_CODES */

                    //modify for Three-Way Calling : Hold Key not supported by MS
                    CallApp_Flash_Call(pMe);
                    return TRUE;

                case AVK_0:
#ifdef DISPLAY_TEST
                    EmulateDisplayRec();
                    break;
#endif
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
                    //keyToneLength keyLen;

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
                    CLOSE_DIALOG(DLGRET_CONV_DIAL)
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

                case IDS_MUTE:
                case IDS_UNMUTE:

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
            if(pMe->m_pConvImage)
            {
                IIMAGE_Release(pMe->m_pConvImage);
                pMe->m_pConvImage = NULL;
            }
            pMe->m_b_callend_time = 0;

#if defined( FEATURE_CALL_RECORDER)
            CallApp_Dialer_Connect_Turn_Off_Recorder( pMe);
#endif

            return TRUE;
        }

        case EVT_DIALOG_START:
            //pMe->IncomIsready = FALSE;
            IANNUNCIATOR_SetField ( pMe->m_pIAnn, ANNUN_FIELD_LOCKSTATUS/*ANNUN_FIELD_VOICE_PRIVACY*/, ANNUN_STATE_VOICE_PRIV_OFF/*ANNUN_STATE_OFF*/); 
            IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_CALL/*ANNUN_FIELD_CALLFORWARD*/, ANNUN_STATE_CALL_INUSE_OFF/*ANNUN_STATE_OFF*/);
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
            if ( (pMe->m_callEndStatus != AEECM_CALL_END_CLIENT_END)
            	&&(pMe->m_callEndStatus != AEECM_CALL_END_REL_NORMAL) )
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
                if (pMe->m_callEndStatus != AEECM_CALL_END_INCOM_CALL)
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
                    case AEECM_CALL_END_FADE:
                        m_alertType = AEEALERT_ALERT_FADE_TONE;
                        break;

                    case AEECM_CALL_END_ACC_FAIL:
                    case AEECM_CALL_END_REORDER:
                        m_alertType = AEEALERT_ALERT_ABBR_REORDER;
                        break;

                    case AEECM_CALL_END_INTERCEPT:
                        m_alertType = AEEALERT_ALERT_INTERCEPT;
                        break;

                    case AEECM_CALL_END_UIM_NOT_PRESENT:
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

#if FEATURE_DIALER_ANIMAION_SUPPORT
			if(pMe->m_pCallingImage == NULL)
            {
                pMe->m_pCallingImage = ISHELL_LoadImage(pMe->m_pShell, CALLAPP_CALLIN_ANI);
            }
            if(pMe->m_pCallingImage)
            {
                AEEImageInfo ImageSize;
                IIMAGE_SetParm(pMe->m_pCallingImage, IPARM_NFRAMES, CALLAPP_ANI_FRAMES, 0); //added by chengxiao 2009.04.13
                IIMAGE_GetInfo(pMe->m_pCallingImage, &ImageSize);
                IIMAGE_DrawFrame(pMe->m_pCallingImage, 
                                    3, 
                                    (pMe->m_rc.dx - ImageSize.cxFrame)/2, 
                                    CALL_ANIM_IMG_Y- CALL_LINE_HIGHT);
            }
#endif //FEATURE_DIALER_ANIMAION_SUPPORT
            if(pMe->m_Is3Way)
            {
                //  Position NAME
                // Display on secend line
                AECHAR mui_call[MAX_SIZE_NAME_TEXT] ={0};
                (void) ISHELL_LoadResString(pMe->m_pShell,
                                                        AEE_APPSCALLAPP_RES_FILE,
                                                        IDS_MUTI_CALL,
                                                        mui_call,
                                                        sizeof(mui_call));
                SETAEERECT(&rect,
                                                        CALL_NAME_X,
                                                        CALL_THIRD_LINE_Y,
                                                        CALL_NAME_DX,
                                                        CALL_LINE_HIGHT);
                
                IDisplay_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, CALLAPP_TEXT_COLOR);
                (void)IDISPLAY_DrawText(pMe->m_pDisplay,
                                                        AEE_FONT_NORMAL,
                                                        mui_call,
                                                        -1,
                                                        CALL_NAME_X,
                                                        CALL_THIRD_LINE_Y,
                                                        &rect,
                                                        IDF_ALIGN_LEFT|IDF_TEXT_TRANSPARENT);
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

                    CallApp_DrawText_Ex(pMe, AEE_FONT_NORMAL,
                            p_temp->call_name, &rect, IDF_TEXT_TRANSPARENT);

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
            //CallApp_Free_All_Call_Table(pMe);//Free all call table
            // Cancel overall timer
            (void) ISHELL_CancelTimer(pMe->m_pShell,
                                                        CallApp_HandleDialogTimer, pMe);

            // Cancel blink timer
            (void) ISHELL_CancelTimer(pMe->m_pShell,
                                                        CallApp_HandleEndCallBlinkTimer, pMe);
            if(pMe->m_pConvImage)
            {
                IIMAGE_Release(pMe->m_pConvImage);
                pMe->m_pConvImage = NULL;
            }
            if(pMe->m_pCallingImage)
            {
                IIMAGE_Release(pMe->m_pCallingImage);
                pMe->m_pCallingImage = NULL;
            }
            pMe->m_lastCallState = AEECM_CALL_STATE_IDLE;
            return TRUE;
        //case EVT_INCOMISREADY:
        //    pMe->IncomIsready = TRUE;
        //    return TRUE;

        case EVT_KEY:
            //if(pMe->IncomIsready == FALSE)
            //{
            //    return TRUE;
            //}

            CLOSE_DIALOG(DLGRET_OK)
            return TRUE;

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
            if(pMe->m_msg_text_id != IDS_INVALIDEMGNUM)
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
            
            switch(pMe->m_msg_text_id)
            {
                case IDS_VOICEMAIL_EMPTY:
                case IDS_RESTRICT_OUTGOING:
                case IDS_SPEED_DIAL_EMPTY:
                case IDS_INVALIDEMGNUM:
                case IDS_INVALID_IP_NUMBER:
                case IDS_MSG_EXCEPTIONAL_CHAR:
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
                        //exit the emergency mode
                        ICM_SetSystemPreference(pMe->m_pICM,
                                            AEECM_MODE_PREF_PERSISTENT, AEECM_PREF_TERM_PERMANENT, 0,
                                            AEECM_GW_ACQ_ORDER_PREF_NO_CHANGE, AEECM_BAND_PREF_NO_CHANGE,
                                            AEECM_ROAM_PREF_NO_CHANGE, AEECM_HYBR_PREF_NO_CHANGE,
                                            AEECM_SRV_DOMAIN_PREF_NO_CHANGE, AEECM_NETWORK_SEL_MODE_PREF_NO_CHANGE,
                                            NULL, NULL, NULL);
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
        case EVT_DISPLAYDIALOGTIMEOUT:
            pMe->m_bCloseAllApplet = FALSE;
            CLOSE_DIALOG(DLGRET_OK)
            return TRUE;

        default:
            break;
    }

    return FALSE;
}  // CallApp_MsgBox_DlgHandler()

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

    //IImage  *pImage = NULL;
    PARAM_NOT_REF(dwParam)
    CALL_ERR("eCode= %x,w=%x,dw=%x CallApp_IncomingCall_DlgHandler ",eCode,wParam,dwParam);

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

            if(pMe->m_pConvImage)
            {
                IIMAGE_Release(pMe->m_pConvImage);
                pMe->m_pConvImage = NULL;
            }
            {
                byte mute = OEMSOUND_MUTE_VOL;
                ICONFIG_GetItem( pMe->m_pConfig, CFGI_BEEP_VOL, &keyBeepVolumeSetting, sizeof(byte));
                ICONFIG_SetItem( pMe->m_pConfig, CFGI_BEEP_VOL, &mute, sizeof(byte));
            }
            return TRUE;

        case EVT_DIALOG_START:
            //CallApp_Set_Db_In_Idle(TRUE);
            CallApp_IncomingCall_Dlg_Init(pMe);
            if(pMe->m_pBacklight)
            {
                IBACKLIGHT_Enable(pMe->m_pBacklight);
            }
            (void) ISHELL_PostEvent(pMe->m_pShell, AEECLSID_DIALER, EVT_USER_REDRAW,  0,  0);
#ifdef FEATURE_SUPPORT_BT_APP
            bt_ui_process_cmcall_notify(pMe,AEECM_EVENT_CALL_CALLER_ID, FALSE);
#endif
            return TRUE;

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
            if(pMe->m_cdg_dsp_info.pi== AEECM_PI_ALLOW)
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
                    CallApp_DrawText_Ex(pMe, AEE_FONT_NORMAL,
                                pMe->m_CallsTable->call_number, &rect, IDF_TEXT_TRANSPARENT);
                }
            }
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
            {
                if(pMe->m_pCallingImage)
                {
                    IIMAGE_Release(pMe->m_pCallingImage);
                    pMe->m_pCallingImage = NULL;
                }

           #if FEATURE_DIALER_ANIMAION_SUPPORT
                if(pMe->m_pCallingImage == NULL)
                {
                    pMe->m_pCallingImage = ISHELL_LoadImage(pMe->m_pShell, CALLAPP_CALLIN_ANI);
                }
                if(pMe->m_pCallingImage)
                {
                    IIMAGE_SetParm(pMe->m_pCallingImage, IPARM_NFRAMES, CALLAPP_ANI_FRAMES, 0);
                }
                pMe->m_b_draw_dot = FALSE;
            #endif
                CallApp_Dialer_Show_Animation(pMe); 
            
            }

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
            else if (pMe->m_bincoming_rsk == IDS_REJECT)
            {
               REFUI_DRAW_BOTTOMBAR(BTBAR_ANSWER_REJECT);
            }
            else if (pMe->m_CallMuted == TRUE)
            {
               REFUI_DRAW_BOTTOMBAR(BTBAR_ANSWER_MUTE);
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
            Appscomm_is_incoming_state(0);
            //CallApp_Set_Db_In_Idle(FALSE);
	        (void) ISHELL_CancelTimer(pMe->m_pShell,(PFNNOTIFY)CallApp_Dialer_Show_Animation,pMe);
            IALERT_StopRingerAlert(pMe->m_pAlert);
            IALERT_StopMp3Alert(pMe->m_pAlert);
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
            if(pMe->m_pConvImage)
            {
                IIMAGE_Release(pMe->m_pConvImage);
                pMe->m_pConvImage = NULL;
            }
            if(pMe->m_pCallingImage)
            {
                IIMAGE_Release(pMe->m_pCallingImage);
                pMe->m_pCallingImage = NULL;
            }
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
                CallApp_AnswerCall(pMe,FALSE,eCode,wParam,FALSE);
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
                    CallApp_AnswerCall(pMe,FALSE,eCode,wParam,FALSE);
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
                    CallApp_AnswerCall(pMe,FALSE,eCode,wParam,FALSE);
                    return TRUE;

                default:
                    break;
            }
            return TRUE;

        case EVT_KEY:
            if ((AVKType)wParam!=AVK_SEND)
            {      
                pMe->m_b_press_1=FALSE ;
            }
            switch ((AVKType)wParam)
            {

                case AVK_ENDCALL:
                    pMe->m_userCanceled = TRUE;
                    pMe->m_lastCallState = AEECM_CALL_STATE_IDLE;
                    ICM_EndAllCalls(pMe->m_pICM);
                    //CALL_ERR("AVK_ENDCALL", 0,0,0);

                    // Note:
                    // We are allowing the IPHONE notifier to move us to the
                    // next state...

                    break;

                case AVK_CLR:
                    // Pressing CLR will cancel the In-band call dialog
                    //normal incoming call
                    //if (AEECM_CALL_STATE_CONV != pMe->m_lastCallState &&pMe->m_bincoming_rsk == IDS_REJECT)
                    if((pMe->m_b_incall == FALSE ) &&pMe->m_bincoming_rsk == IDS_REJECT)
                    {
                        pMe->m_userCanceled = TRUE;
                        if(pMe->m_CallsTable)
                        {
                            pMe->m_lastCallState = AEECM_CALL_STATE_IDLE;
                            ICM_EndCall(pMe->m_pICM,pMe->m_CallsTable->call_id);
                            
                            return TRUE;
                        }
                        CLOSE_DIALOG(DLGRET_OK)
                        return TRUE;
                    }
                    else if(pMe->m_bincoming_rsk == IDS_MUTE)
                    {
                        pMe->m_bincoming_rsk = IDS_REJECT;
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

                case AVK_SEND:
                {
                    AEECMCallID nCallID ;
                    if(pMe->m_b_press_1)
                    {
                        ICM_OriginateVoiceCall(pMe->m_pICM, L"1", &nCallID);
                    }
                }
                case AVK_USER_HEADSET:
                case AVK_SELECT:
                    CallApp_AnswerCall(pMe,FALSE,eCode,wParam,FALSE);
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
                    CallApp_AnswerCall(pMe,FALSE,eCode,wParam,FALSE);
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
            pMe->m_bincoming_rsk = IDS_REJECT;
        }
#if !defined( FEATURE_RECORDER)
        CallApp_SetupCallAudio(pMe);
#endif
    }
}
static void CallApp_HandleStopMissedAlertTimer(void *pUser)
{
    CCallApp *pMe = (CCallApp *)pUser;
    //IALERT_StopAlerting(pMe->m_pAlert);
    IALERT_StopMissedCallAlert(pMe->m_pAlert);
    notifyFMRadioAlertEvent( pMe, FALSE);
    pMe->m_b_miss_notify = FALSE;
    //CallAppNotifyMP3PlayerAlertEvent(pMe,FALSE);
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
            if(pMe->m_pConvImage)
            {
                IIMAGE_Release(pMe->m_pConvImage);
                pMe->m_pConvImage = NULL;
            }
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
            if(pMe->m_pCallingImage)
            {
                IImage_Release(pMe->m_pCallingImage);
                pMe->m_pCallingImage = NULL;
            }

        #if FEATURE_DIALER_ANIMAION_SUPPORT
            if(pMe->m_pCallingImage == NULL)
            {
                pMe->m_pCallingImage = ISHELL_LoadImage(pMe->m_pShell, CALLAPP_CALLIN_ANI);
            }

            if(pMe->m_pCallingImage)
            {
                AEEImageInfo ImageSize;
                
                IIMAGE_SetParm(pMe->m_pCallingImage, IPARM_NFRAMES, CALLAPP_ANI_FRAMES, 0);
                IIMAGE_GetInfo(pMe->m_pCallingImage, &ImageSize);
                IIMAGE_DrawFrame(pMe->m_pCallingImage, 
                                    0, 
                                    (pMe->m_rc.dx - ImageSize.cxFrame)/2, 
                                    CALL_ANIM_IMG_Y - CALL_LINE_HIGHT);
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
                                                    CALL_NUM_X, //CALL_TEXT_X,
                                                    CALL_SECOND_LINE_Y,
                                                    CALL_NUM_DX, //CALL_TEXT_DX,
                                                    CALL_LINE_HIGHT);
            IDisplay_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, CALLAPP_TEXT_COLOR);
#ifdef FEATURE_CARRIER_THAILAND_HUTCH                     
            (void)IDISPLAY_DrawText(pMe->m_pDisplay,
                                                    AEE_FONT_NORMAL,
                                                    szText,
                                                    -1,          
                                                    CALL_NAME_X+8,                                                 
                                                    CALL_SECOND_LINE_Y,
                                                    &rc,
                                                    IDF_TEXT_TRANSPARENT);
#else
            (void)IDISPLAY_DrawText(pMe->m_pDisplay,
                                                    AEE_FONT_NORMAL,
                                                    szText,
                                                    -1,
                                                    CALL_NUM_X, //CALL_TEXT_X, //modfied by chengxiao 2009.01.20                                                
                                                    CALL_SECOND_LINE_Y,
                                                    &rc,
                                                    IDF_TEXT_TRANSPARENT);
#endif //#if defined FEATURE_CARRIER_THAILAND_HUTCH		
            IDisplay_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, RGB_BLACK);

            //drawBottomBar(pMe->m_pDisplay,AEE_FONT_NORMAL,BTBAR_OK_CANCEL);
#ifdef FEATURE_CARRIER_THAILAND_HUTCH   
            REFUI_DRAW_BOTTOMBAR(BTBAR_OK_BACK);
#else
            REFUI_DRAW_BOTTOMBAR(BTBAR_OK_CANCEL);
#endif //#if defined FEATURE_CARRIER_THAILAND_HUTCH		
            
            // add Missing call icon
            IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            //IALERT_StartAlerting(pMe->m_pAlert, AEEALERT_CALLTYPE_VOICE,
            //                                        NULL, AEEALERT_ALERT_BUSY);
            if(pMe->m_b_miss_notify)
            {
                notifyFMRadioAlertEvent( pMe, TRUE);
                //CallAppNotifyMP3PlayerAlertEvent(pMe,TRUE);
                IALERT_StartMissedCallAlert(pMe->m_pAlert);
                (void) ISHELL_SetTimer(pMe->m_pShell,
                                                        3000,
                                                        CallApp_HandleStopMissedAlertTimer,
                                                        pMe);
            }
            return TRUE;

        }

        case EVT_DIALOG_END:
            //CallApp_Free_All_Call_Table(pMe);//free all call table
            pMe->m_lastCallState = AEECM_CALL_STATE_IDLE;
            if(pMe->m_pConvImage)
            {
                IIMAGE_Release(pMe->m_pConvImage);
                pMe->m_pConvImage = NULL;
            }
            if(pMe->m_pCallingImage)
            {
                IIMAGE_Release(pMe->m_pCallingImage);
                pMe->m_pCallingImage = NULL;
            }
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
            REFUI_DRAW_BOTTOMBAR(BTBAR_OK_BACK)
            (void)ISTATIC_Redraw(pStatic);
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

    if (SUCCESS != ICONFIG_GetItem(pMe->m_pConfig,CFGI_HEADSET_PRESENT,&headsetPresent,sizeof(headsetPresent)))
    {
        headsetPresent = FALSE;
    }

#ifdef FEATURE_SUPPORT_BT_APP
    CALL_FUN_START("CallApp_SetupCallAudio %d %d %d", pMe->m_bHandFree, headsetPresent, pMe->m_bt_audio );
#else
    CALL_FUN_START("CallApp_SetupCallAudio %d %d", pMe->m_bHandFree, pMe->m_CallMuted, 0 );
#endif

    if(pMe->m_bHandFree)
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
    else if (headsetPresent)
    {
         soundStuff.eDevice = AEE_SOUND_DEVICE_HEADSET;            
    }
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

    soundStuff.eMethod = AEE_SOUND_METHOD_VOICE;
    soundStuff.eAPath = AEE_SOUND_APATH_BOTH;

    if(AEECM_IS_VOICECALL_CONNECTED(pMe->m_pICM) )
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
        //DBGPRINTF("CallApp_MakeSpeedDialCall %S",pMe->m_DialString,0,0);
 #ifdef FEATURE_CARRIER_THAILAND_HUTCH
        if(WSTRLEN(pMe->m_DialString) >= 2)
  #else
       if(WSTRLEN(pMe->m_DialString) >= 3)
  #endif
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
        pMe->m_msg_text_id = IDS_SPEED_DIAL_EMPTY;
        /*
        CallApp_Init_Msgbox_String(pMe, IDS_SPEED_DIAL_EMPTY, pMe->m_DialString);

        if(pMe->m_msg_text != NULL)
        {*/
        CLOSE_DIALOG(DLGRET_MSGBOX);
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
    AEECMCallID nCallID = 0;
#ifdef FEATURE_APP_PAUSE_TIMER
    AECHAR *timer       = NULL;
#endif //FEATURE_APP_PAUSE_TIMER
    AECHAR wbuf[MAX_SIZE_DIAL_STR] = {0};
    AEECMCallInfo ci;

    boolean b_have_p    = FALSE;
    boolean b_have_t    = FALSE;
    AEECLSID cls        = AEE_Active();
    boolean b_energency =FALSE;
    boolean b_restict   =FALSE;
    ISHELL_CancelTimer(pMe->m_pShell,(PFNNOTIFY)CallApp_MakeCall, pMe);
    if(cls == AEECLSID_DIALER ||cls == AEECLSID_CORE_APP)
    {
    }
    else
    {
        CALL_ERR("Active Clisid not allow make call",0,0,0);
        pMe->m_b_auto_redial     = FALSE;
        pMe->m_auto_redial_count = 0;
        return CALL_FAIL_ANOTHER;
    }

    //DBGPRINTF("%S CallApp_MakeCall",pMe->m_DialString,0,0);
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
    //DBGPRINTF("%S wbuf2",wbuf,0,0);
    //DBGPRINTF("%S pMe->m_PauseString2",pMe->m_PauseString,0,0);
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

    if ((CallApp_IsEmergencyMode(pMe->m_pICM)
        ||pMe->idle_info.uimLocked)
        &&!b_energency)
    {
        CLOSE_DIALOG(DLGRET_DIALER_INVALID_EMGNUMBER)
        return CALL_FAIL_EMERGENCY_MODE;
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

    pMe->m_b_incall = AEECM_IS_VOICECALL_CONNECTED(pMe->m_pICM);

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
        if(WSTRNCMP(wbuf,L"+",1) == 0)
        {
            AECHAR internation[MAX_SIZE_DIAL_STR] = {0};
            //ICONFIG_GetItem(pMe->m_pConfig, CFGI_FDN_ENABLED, &internation,sizeof(internation));
            STRTOWSTR(OEM_INTERNATION_NUMBER,internation,sizeof(internation));
            //WSTRLCPY(internation,(AECHAR)OEM_INTERNATION_NUMBER ,MAX_SIZE_DIAL_STR);
            WSTRLCAT(internation, &wbuf[1] ,MAX_SIZE_DIAL_STR);
            WSTRLCPY(wbuf,internation,MAX_SIZE_DIAL_STR);
        }
        //we need save Originate call number
        if(CheckAEEReturnStatus(ICM_OriginateVoiceCall(pMe->m_pICM, wbuf, &nCallID)) == FALSE)
        {
            CALL_ERR("ICM_OriginateVoiceCall FAILED ", 0, 0, 0);
            return CALL_FAIL_ANOTHER;
        }
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
    static AECHAR  str[MAX_SIZE_DIALER_TEXT];
    static AECHAR  revStr[MAX_SIZE_DIALER_TEXT];
    AECHAR          *srcStr;
    AECHAR          *dstStr;
    int                   len;
    int                   largeLineHeight;
    int                   y;
    int                   fits;
    int                   pixelLen;
    AEERect           dialerRect;

    CALL_FUN_START("CallApp_DrawDialerString", 0, 0, 0);

    SETAEERECT(&dialerRect,
                                            0,
                                            0,
                                            pMe->m_rc.dx,
                                            pMe->m_rc.dy - BOTTOMBAR_HEIGHT);

    /*SETAEERECT(&dialerRect,
                                            SOFT_KEY_MENU_X,
                                            SOFT_KEY_MENU_Y,
                                            SOFT_KEY_MENU_DX,
                                            SOFT_KEY_MENU_DY);*/

    // Clear the dialer rectangle
    Appscommon_ResetBackgroundEx(pMe->m_pDisplay, &dialerRect, TRUE);
    
    // Copy the dialer string into a temp buffer so it can be modified
    WSTRNCOPYN(str,sizeof(str)/sizeof(str[0]),  dialStr,  -1);

    // Return if empty
    len = WSTRLEN(str);
    if (len <= 0)
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
    //OEMFont_SetBigNumber(TRUE);
    //SetLargeFontBold(pMe->m_pOEM_TSGBridge,pMe->m_pDisplay);(Null Function)
    largeLineHeight = IDISPLAY_GetFontMetrics(pMe->m_pDisplay,
                                            AEE_FONT_NORMAL,
                                            NULL,
                                            NULL);

    // Main drawing loop.
    //
    // Each iteration of this loop will draw one line of the dialing
    // digits, starting from the bottom of the rectangle and moving up.
    //
    y = dialerRect.dy;

#if defined( FEATURE_CALL_RECORDER)
    if( pMe->m_bRecorderOn)
    {
		y -= largeLineHeight;
    }
#endif

    for (;;)
    {
        if ('\0' == *dstStr)
        {
            // Nothing more to display
            break;
        }

        y -= largeLineHeight;

        if (y < dialerRect.y)
        {
            // Out of room in the rectangle
            break;
        }

        // Determine how much of the string will fit on the line
        // (the 'fits' var will return the number of characters that will fit)
        pixelLen = IDISPLAY_MeasureTextEx(pMe->m_pDisplay,
                                            AEE_FONT_NORMAL,
                                            dstStr,
                                            -1,
                                            dialerRect.dx,
                                            &fits);

        if (pixelLen <= 0)
        {
            break;
        }

        ASSERT(pixelLen <= dialerRect.dx);
        pixelLen = dialerRect.dx - pixelLen;

        // Move dstStr past the characters about to be drawn
        dstStr += fits;

        // Move srcStr to the location of the characters about to be drawn
        srcStr -= fits;
        
        IDisplay_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, CALLAPP_TEXT_COLOR);
        (void) IDISPLAY_DrawText(pMe->m_pDisplay,
                                            AEE_FONT_NORMAL,
                                            srcStr,
                                            -1,
                                            pixelLen,
                                            y,
                                            &dialerRect,
                                            IDF_TEXT_TRANSPARENT);
        IDisplay_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, RGB_BLACK);

        // Forget about the characeters that were just drawn.
        *srcStr = '\0';
    }
    //SetLargeFontPlain(pMe->m_pOEM_TSGBridge,pMe->m_pDisplay);
}
#endif //#ifdef FEATURE_LARGE_DIALING_DIGITS

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
    CALL_FUN_START("CallApp_NumberLookup", 0, 0, 0);
#endif
    if(0 == WSTRLEN(Number))
    {
        return FALSE;
    }

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
        if (SUCCESS != ICONTAPP_NumberLookup(ca,
                                             Number,
                                             &pContInfo))
        {
            IContactApp_Release(ca);
            return FALSE;
        }
#endif
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

        WSTRLCPY(pMe->m_cont_cache.call_number,Number,AEECM_MAX_DIGITS_LENGTH);
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
        /*if(headsetPresent)
        {
            si.eDevice = AEE_SOUND_DEVICE_HEADSET;
        }
        else */if (pMe->m_bHandFree)
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
        else if(headsetPresent)
        {
            si.eDevice = AEE_SOUND_DEVICE_HEADSET;
        }
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

    AEECMCallInfo ci;
    boolean bKeyguardEnabled = FALSE;
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
    if(((wParam == AVK_SEND && (pMe->m_anykey_answer & 0x4))
        ||(eCode == EVT_FLIP && ((boolean)wParam == TRUE)  && (pMe->m_anykey_answer & 0x2))
        ||(((wParam == AVK_USER_HEADSET) || (wParam == AVK_SEND)) && (pMe->m_anykey_answer & 0x8))
        ||(((((AVK_FIRST < wParam && wParam <AVK_POWER ) ||(wParam == AVK_INFO)||(AVK_CLR < wParam && wParam <AVK_SOFT1 ))
                 && !bKeyguardEnabled)
                 ||(wParam == AVK_SEND))
                 && (pMe->m_anykey_answer & 0x1))
        ) ||auto_answer ||wParam == AVK_SELECT)
    {
        if(AEE_SUCCESS != ICM_GetCallInfo(pMe->m_pICM, pMe->m_CallsTable->call_id, &ci, sizeof(AEECMCallInfo)))
        {
            return FALSE;
        }

        IALERT_StopAlerting(pMe->m_pAlert);
        switch (ci.call_state)
        {
            case AEECM_CALL_STATE_INCOM:
#ifdef FEATURE_IS2000_SCC_CODES
                pMe->m_bAnswerHold = bAnswerHold;
#endif /* FEATURE_IS2000_SCC_CODES */
                ICM_AnswerCall(pMe->m_pICM, pMe->m_CallsTable->call_id);
                CLOSE_DIALOG(DLGRET_CONNECT)
                break;

            case AEECM_CALL_STATE_CONV:
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
    CALL_FUN_START("CallApp_AnswerInbandCall", 0, 0, 0);

    if(pMe->m_CallsTable)
    {
        pMe->m_Is3Way = TRUE;
        //add for call waitting number display	 in missed record
        //in-band呼叫已经接起,不要记录未接号码
        pMe->m_bmissCallWaiting = FALSE;
        // Clear any remaining DTMF digits
        pMe->m_PauseString[0] = 0;
        // Send empty flash message to answer the call
        ICM_AnswerCall(pMe->m_pICM, pMe->m_CallsTable->call_id);
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
    CALL_FUN_START("CallApp_BuildDialerMenu",0,0,0);

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
            || (pMe->m_bAnswerHold = FALSE)
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
                   (void) IMENUCTL_AddItem(pMenuCtl,AEE_APPSCALLAPP_RES_FILE,IDS_SK_MENU,IDS_SK_MENU,
                                                       (AECHAR*)NULL,(uint32)NULL);
                }
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
        (void) IMENUCTL_AddItem(pSKMenu,AEE_APPSCALLAPP_RES_FILE,IDS_UNMUTE,IDS_UNMUTE,
                                                (AECHAR*)NULL,(uint32)NULL);
    }
    else
    {
        (void) IMENUCTL_AddItem(pSKMenu,AEE_APPSCALLAPP_RES_FILE,IDS_MUTE,IDS_MUTE,
                                                (AECHAR*)NULL,(uint32)NULL);
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
        || (pMe->m_bAnswerHold = FALSE)
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

    (void) IMENUCTL_AddItem(pSKMenu,AEE_APPSCALLAPP_RES_FILE,IDS_CONTACT,IDS_CONTACT,
                                            (AECHAR*)NULL,(uint32)NULL);

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
    AECHAR         szText[20];
#ifdef FEATURE_LANG_BIDI
    AECHAR         szTextTmp[100];
#endif
    Dialer_call_table * temp = NULL;

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
    SETAEERECT(&rect,
                                            0,
                                            CALL_FIRST_LINE_Y,
                                            CALL_TEXT_DX+4,
                                            CALL_LINE_HIGHT);
    
    Appscommon_ResetBackgroundEx(pMe->m_pDisplay, &rect, TRUE);

    SETAEERECT(&rect,
                                            CALL_TEXT_X,
                                            CALL_FIRST_LINE_Y,
                                            CALL_TEXT_DX,
                                            CALL_LINE_HIGHT);

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

    SETAEERECT(&rect,
                                            CALL_TEXT_X +bWidth +4 ,
                                            CALL_FIRST_LINE_Y,
                                            CALL_TEXT_DX - bWidth -4,
                                            CALL_LINE_HIGHT);

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
    
    CALL_FUN_START("CallApp_Draw_Connect_Number_and_Name", 0, 0, 0);
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
    
    oldColor = IDisplay_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, CALLAPP_TEXT_COLOR);
    // display Call Name
    if(pMe->m_Is3Way)/*in this time we only show 'muticall'*/
    {
        if(!b_cdg)
        {
            AECHAR mui_call[MAX_SIZE_NAME_TEXT] ={0};
            (void) ISHELL_LoadResString(pMe->m_pShell,
                                                AEE_APPSCALLAPP_RES_FILE,
                                                IDS_MUTI_CALL,
                                                mui_call,
                                                sizeof(mui_call));
            /*预留出动画的宽度*/
            SETAEERECT(&rect,
                                                CALL_NAME_X,
                                                CALL_SECOND_LINE_Y,
                                                CALL_NAME_DX,
                                                CALL_LINE_HIGHT);
            (void)IDISPLAY_DrawText(pMe->m_pDisplay,
                                                AEE_FONT_NORMAL,
                                                mui_call,
                                                -1,
                                                CALL_NAME_X,
                                                CALL_SECOND_LINE_Y,
                                                &rect,
                                                IDF_TEXT_TRANSPARENT);
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
            if(pMe->m_cdg_dsp_info.pi== AEECM_PI_ALLOW)
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

    if (pMe->m_pConvImage)
    {
        IIMAGE_SetOffset(pMe->m_pConvImage, 0, pMe->m_rc.dy -
                                            (pMe->m_LineHeight + pMe->m_LineHeight + 14));
        IIMAGE_SetDrawSize(pMe->m_pConvImage, 176, 16);
        (void) IIMAGE_Draw(pMe->m_pConvImage, 0, pMe->m_rc.dy -
                                            (pMe->m_LineHeight + pMe->m_LineHeight + 14));
        IIMAGE_SetOffset(pMe->m_pConvImage, 0, 0);
    }

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

        (void)WSTRLCAT(pMe->m_CallsTable->call_number, pMe->m_TimerString ,AEECM_MAX_DIGITS_LENGTH);
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

        (void)WSTRLCAT(pMe->m_CallsTable->call_number, pMe->m_PauseString ,AEECM_MAX_DIGITS_LENGTH);
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
        byte strLen;
        AECHAR wsbuf[AEECM_MAX_DIGITS_LENGTH] = {0};
        const DTMFToneDuration *dur = CallApp_GetDTMFDuration(pMe);

        strLen = (byte) STRLEN(pszNum);
        STRTOWSTR(pszNum,wsbuf,sizeof(wsbuf));

        CallApp_PlayToneList(pMe, pszNum, strLen);
        ICM_BurstDTMF(pMe->m_pICM, pMe->m_CallsTable->call_id, dur->nOn, dur->nOff, wsbuf);

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
    if(curProfile == OEMNV_PROFILE_MEETING)
    {
        curProfile = OEMNV_PROFILE_NORMALMODE;
        IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_RINGTONE, ANNUN_STATE_RINGTONE_ALERT);
    }
    else
    {
        curProfile = OEMNV_PROFILE_MEETING;
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


}

static void CallApp_Flash_Call(CCallApp *pMe)
{
    AEECMCallID nCallID = 0;

    CALL_FUN_START("CallApp_Flash_Call %d", pMe->m_Is3Way, 0, 0);

    if(!pMe->m_Is3Way)
    {
        return;
    }

    if(CheckAEEReturnStatus(ICM_OriginateVoiceCall(pMe->m_pICM, NULL, &nCallID)) == FALSE)
    {
        CALL_ERR("ICM_OriginateVoiceCall FAILED", 0, 0, 0);
        return;
    }
}

static void CallApp_StartContDTMF(CCallApp *pMe,AECHAR *c_dtmf)
{
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
}

static void CallApp_BurstDTMF(CCallApp *pMe,AECHAR *dtmf,int len)
{
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
}

static void CallApp_Stop_ContDTMF(ICM *m_pICM)
{
    int         i   = 0;
    AEECMCallID CallIDs[DIALERAPP_MAX_NUM_CALLS];
    uint16      wNumCallIDs;

    CALL_FUN_START("CallApp_Stop_ContDTMF", 0, 0, 0);

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
}
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
    if(pMe->m_cdg_dsp_info.pi == AEECM_PI_RES)
    {
        ISHELL_LoadResString(pMe->m_pShell,
                                                    AEE_APPSCALLAPP_RES_FILE,
                                                    IDS_NUMBER_RESTRICTED,
                                                    pMe->m_cdg_dsp_info.alpha,
                                                    sizeof(pMe->m_cdg_dsp_info.alpha));
    }
    else if(pMe->m_cdg_dsp_info.pi == AEECM_PI_NOT_AVAIL)
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
    if(pMe->m_b_incall )
    {
        type = BTBAR_OPTION_DELETE;
    }
    else
    {
#ifdef FEATRUE_SET_IP_NUMBER
        if(pMe->m_b_have_set_ip && !(CallApp_IsEmergencyMode(pMe->m_pICM)
        ||pMe->idle_info.uimLocked))
        {
            type = BTBAR_SAVE_IP_DELETE;
        }
        else
#endif/*FEATRUE_SET_IP_NUMBER*/
        {
            type = BTBAR_OPTION_SAVE_DEL; //BTBAR_SAVE_DELETE;
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
        if(pMe->m_bHandFree)
        {
            REFUI_DRAW_BOTTOMBAR(BTBAR_OPTION_NORMAL)
        }
        else
        {
            REFUI_DRAW_BOTTOMBAR(BTBAR_OPTION_HANDSFREEON)
        }
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

    if ((AVKType)wParam == AVK_CLR)
    {
        // Clearing the last digit exits the dialog
        pMe->m_DialString[0] = 0;
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
            if(!(CallApp_IsEmergencyMode(pMe->m_pICM)||pMe->idle_info.uimLocked))
            {
                pMe->m_b_from_numedit = TRUE;
                CLOSE_DIALOG(DLGRET_SEL_IP_NUMBER)
            }
            //}
            return TRUE;
        }
#endif /*FEATRUE_SET_IP_NUMBER*/
        if ( (AVKType)wParam == AVK_STAR && WSTRLEN(pMe->m_DialString) == 1)
        {
#ifdef FEATURE_KEYGUARD
            pMe->m_b_set_keyguard = TRUE;
            ISHELL_CloseApplet(pMe->m_pShell, TRUE);
#endif
        }

        // # key for shortcut of quiet mode
        else if ( (AVKType)wParam == AVK_POUND && WSTRLEN(pMe->m_DialString) == 1)
        {
            CallApp_ShortcutQuiet( pMe );
            ISHELL_CloseApplet(pMe->m_pShell, TRUE);
        }

        //long key "1" to call voice mail
        else if ((AVKType)wParam == AVK_1)
        {
            if((pMe->m_DialString[0] == '1')&&(pMe->m_DialString[1] == '\0'))
            {
                CallApp_MakeVoiceMailCall(pMe);
            }
        }
        else if ( ((AVKType)wParam >= AVK_2) &&((AVKType)wParam <= AVK_9))
        {
            uint16 wIndex;

            // TBD - dial string format should be typedef'd
            //if more than 2 digits then bail out because
            // we only support from to 1 to 99
            wIndex = (uint16)WSTRTOFLOAT(pMe->m_DialString);

            if(((uint16)WSTRLEN(pMe->m_DialString) <= MAX_SPEEDDIAL_CHARS) &&(wIndex!=0))
            {
                CallApp_MakeSpeedDialCall(pMe);
            }
        }

    }
    return TRUE;
}
static void CallApp_Process_Spec_Key_Event(CCallApp *pMe,uint16 wp)
{
    int len = 0;

    CALL_FUN_START("CallApp_Process_Spec_Key_Event",0, 0, 0);

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


    while(line)
    {
        line--;
        len = WSTRLEN(wBuf);
        pixLen = IDISPLAY_MeasureTextEx(pMe->m_pDisplay, fnt,  wBuf,  -1, rect->dx, &fits);
        //CALL_ERR("%d %d %d len,pixLen,fits",len,pixLen,fits);
        if (0 == pixLen)
        {
            // IDISPLAY_MeasureTextEx failed, just return the entire string
            line = 0;
            //CALL_ERR("IDISPLAY_MeasureTextEx failed",0,0,0);
            return IDISPLAY_DrawText(pMe->m_pDisplay,  fnt, wBuf,-1, rect->x, rect->y, rect, mask);
        }

        if (fits >= len)
        {
            // Entire string fits on the line, all is good
            line = 0;
            //CALL_ERR("Entire string fits on the line, all is good",0,0,0);
            return IDISPLAY_DrawText(pMe->m_pDisplay,  fnt, wBuf,-1, rect->x, rect->y, rect, mask);
        }

        if(fits < len)
        {
            if(line == 0)//the last line ,need add ...
            {
                wBuf[fits-3] = 0;
                //CALL_ERR("the last line ,need add ...",0,0,0);
                WSTRCAT(wBuf, (AECHAR*)".\0.\0.\0");
                IDISPLAY_DrawText(pMe->m_pDisplay,  fnt, wBuf,-1, rect->x, rect->y, rect, mask);
            }
            else
            {
                AECHAR temp[64] = {0};
                //CALL_ERR("draw %d line",line,0,0);
                WSTRLCPY(temp, wBuf + fits , 64);
                wBuf[fits] = 0;
                IDISPLAY_DrawText(pMe->m_pDisplay,  fnt, wBuf,-1, rect->x, rect->y, rect, mask);
                WSTRLCPY(wBuf, temp,64);
                rect->y = rect->y + hight/*CALL_LINE_HIGHT*/;/*y向下增加一行*/
            }
        }
    }
    IDisplay_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, oldColor);
    return TRUE;
}

static boolean CallApp_Process_Send_Key_Release_Event(CCallApp *pMe)
{
    uint16 wIndex;
    MAKE_CALL_VALUE ret;
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
    if(pMe->m_b_incall && CallApp_IsEmergencyMode(pMe->m_pICM) )
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
        ICM_EndAllCalls(pMe->m_pICM);
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

    if((AECHAR)'\0' != pMe->m_CallsTable->ringer[0])
    {
        if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                            AEECLSID_RINGERMGR,
                                            (void **)&RingerMgr))
        {
            return ;
        }
        WSTRTOSTR(pMe->m_CallsTable->ringer, filename, MAX_FILE_NAME);

        // 为了支持PEK 测试，电话本的RINGTONE 字段只能保存字符串类型，这里根据路径名查找是否有MID存在
        ringerId = IRINGERMGR_GetRingerID(RingerMgr, filename);
        
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
                // 如果失败就播放默认的MID
                byte midID = 0;

                ICONFIG_GetItem(pMe->m_pConfig,CFGI_RINGER_TYPE,&midID, sizeof(midID));
                if(0 == midID)
                {
                    midID = OEMNV_DEFAULTRINGER;
                }
                
                IALERT_StartRingerAlert(pMe->m_pAlert, midID);
            }
        }
    }
    else
    {
        byte midID = 0;
        byte profilenum = 0;
        
        ICONFIG_GetItem(pMe->m_pConfig,CFGI_RINGER_TYPE,&midID, sizeof(midID));
        if(midID == 0)
        {
            midID = OEMNV_DEFAULTRINGER;
        }
    
        ICONFIG_GetItem(pMe->m_pConfig, CFGI_PROFILE_CUR_NUMBER, &profilenum, sizeof(profilenum));
        ICONFIG_GetItem(pMe->m_pConfig, CFGI_PROFILE_CALL_RINGER, (void*)ringid, sizeof(ringid));
        if(ringid[profilenum].ringType == OEMNV_MID_RINGER)
        {
            IALERT_StartRingerAlert(pMe->m_pAlert,(uint32)midID);
        }
        else if(ringid[profilenum].ringType == OEMNV_MP3_RINGER)
        {
            if ((IALERT_StartMp3Alert(pMe->m_pAlert, ringid[profilenum].szMusicname,ALERT_NORMAL_SND) != SUCCESS))
            {
                MSG_ERROR("play mp3 player faile, to play mid alert.......",0,0,0);
                IALERT_StartRingerAlert(pMe->m_pAlert, (uint32)midID);
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

    WSTRLCPY(wbf,dialStr,sizeof(wbf));
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
    CallApp_DrawDialerString(pMe, pMe->m_DialString);
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
    int xPos = 0, yPos = 0, xNum = 0, yNum = 0, dy = pMe->m_rc.dy;
    
#if defined( FEATURE_CALL_RECORDER)
    if(pMe->m_bRecorderOn)
    {
        dy -= NUM_IMAGE_HIGHT;
    }
#endif
#ifdef FEATURE_IMAGE_DIALING_DIGITS
    xNum = pMe->m_rc.dx /NUM_IMAGE_WIDTH;
    yNum = (dy - BOTTOMBAR_HEIGHT)/(NUM_IMAGE_HIGHT + BETWEEN_LINE_PIXEL);
    xPos = (pMe->m_rc.dx + NUM_IMAGE_WIDTH *xNum) /2 - (pMe->m_nCursorPos%xNum)*NUM_IMAGE_WIDTH - 2;
    yPos = dy - BOTTOMBAR_HEIGHT - (1 + pMe->m_nCursorPos/xNum)*(NUM_IMAGE_HIGHT + BETWEEN_LINE_PIXEL);
    if(pMe->m_nCursorPos >= xNum*yNum)
    {
        yPos = dy - BOTTOMBAR_HEIGHT - yNum*(NUM_IMAGE_HIGHT + BETWEEN_LINE_PIXEL);
    }
    if(pMe->m_nCursorPos != 0 &&
        pMe->m_nCursorPos == WSTRLEN(pMe->m_DialString) &&
        pMe->m_nCursorPos%xNum == 0)
    {
        xPos = (pMe->m_rc.dx - NUM_IMAGE_WIDTH *xNum) /2 - 2;
        if(pMe->m_nCursorPos < xNum*yNum)
        {
            yPos += (NUM_IMAGE_HIGHT + BETWEEN_LINE_PIXEL);
        }
    }
	SETAEERECT(pRect, xPos, yPos, 4, NUM_IMAGE_HIGHT);
#else
	xNum = pMe->m_rc.dx /8;
    yNum = (dy - BOTTOMBAR_HEIGHT)/(16 + BETWEEN_LINE_PIXEL);
    xPos = (pMe->m_rc.dx + 8 *xNum) /2 - (pMe->m_nCursorPos%xNum)*8 - 2;
    yPos = dy - BOTTOMBAR_HEIGHT - (1 + pMe->m_nCursorPos/xNum)*(16 + BETWEEN_LINE_PIXEL);
    if(pMe->m_nCursorPos >= xNum*yNum)
    {
        yPos = dy - BOTTOMBAR_HEIGHT - yNum*(16 + BETWEEN_LINE_PIXEL);
    }
    if(pMe->m_nCursorPos != 0 &&
        pMe->m_nCursorPos == WSTRLEN(pMe->m_DialString) &&
        pMe->m_nCursorPos%xNum == 0)
    {
        xPos = (pMe->m_rc.dx - 8 *xNum) /2 - 2;
        if(pMe->m_nCursorPos < xNum*yNum)
        {
            yPos += (16 + BETWEEN_LINE_PIXEL);
        }
    }
	SETAEERECT(pRect, xPos, yPos, 4, 16);
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
    AEERect rect = {0};
    RGBVAL  nCursorColor = MAKE_RGB(0xE6, 0xE6, 0xE6);
    
    SETAEERECT(&rect, pRect->x, pRect->y, pRect->dx, 1);
    IDISPLAY_FillRect(pMe->m_pDisplay, &rect, nCursorColor);
    SETAEERECT(&rect, pRect->x+1, pRect->y+1, 2, pRect->dy-2);
    IDISPLAY_FillRect(pMe->m_pDisplay, &rect, nCursorColor);
    SETAEERECT(&rect, pRect->x, pRect->y+pRect->dy-1, pRect->dx, 1);
    IDISPLAY_FillRect(pMe->m_pDisplay, &rect, nCursorColor);
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

    CallApp_Calc_Cursor_Rect(pMe, &rect);

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


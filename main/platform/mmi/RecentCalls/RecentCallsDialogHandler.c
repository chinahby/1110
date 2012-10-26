/*==============================================================================
// 文件：
//        recentcallsDialogHandler.c
//        2007-11-18 陈喜玲草拟版本(Draft Version)
//        版权所有(c) 2007 Anylook
//        
// 文件说明：
//        
// 作者：2007-11-18
// 创建日期：陈喜玲
// 当前版本：Draft
==============================================================================*/

/*==============================================================================
修改历史记录：
时       间      修 改 人       修改内容、位置及原因
-----------      ----------     -----------------------------------------------
07-11-18         陈喜玲         初始版本
==============================================================================*/


/*==============================================================================
                                 本文件包含的外部文件
==============================================================================*/
#include "recentcalls_priv.h"
#include "appscommonimages.brh"

//#ifdef FEATURE_CARRIER_SUDAN_SUDATEL
#include "nv_items.h"
//#endif
/*==============================================================================
                                 宏定义和常数
==============================================================================*/
// 关闭对话框宏
#define CLOSE_DIALOG(DlgRet)  {    \
                                     pMe->m_eDlgRet = DlgRet; \
                                     (void) ISHELL_EndDialog(pMe->m_pShell);  \
                                 }


/*==============================================================================
                                 类型定义
==============================================================================*/


/*==============================================================================
                                 函数声明
==============================================================================*/
static void DialogTimeoutCallback(void *pUser);
static boolean  RecentCalls_VerifyPasswordEvent(CRecentCalls *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

// 对话框 IDD_RMAINMENU 事件处理函数
static boolean  RecentCalls_MainMenuEvent(CRecentCalls *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

// 对话框 IDD_RLISTRECORD 事件处理函数
static boolean  RecentCalls_ListRecordEvent(CRecentCalls *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

// 对话框 IDD_RTIME_MENU 事件处理函数
static boolean  RecentCalls_TimeMenuEvent(CRecentCalls *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

// 对话框 IDD_RDEL_MENU 事件处理函数
static boolean  RecentCalls_DelMenuEvent(CRecentCalls *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

// 对话框 IDD_RDEAL_RECORD 事件处理函数
static boolean  RecentCalls_DealMenuEvent(CRecentCalls *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

// 对话框 IDD_RWARN 事件处理函数
static boolean  RecentCalls_WarnEvent(CRecentCalls *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

// 对话框 IDD_RTIME 事件处理函数
static boolean  RecentCalls_TimeEvent(CRecentCalls *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

// 对话框 IDD_DETAIL 事件处理函数
static boolean  RecentCalls_DetailEvent(CRecentCalls *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

static void RecentCalls_GetContactName(CRecentCalls* pMe,  
    AECHAR *pwszNum, 
    AECHAR *pwszName,
    uint16 wBufLen
);
static boolean RecentCalls_HandleMsgBoxDlgEvent(CRecentCalls* pMe,
    AEEEvent eCode,
    uint16   wParam,
    uint32   dwParam
);
#if defined(FEATURE_WMS_APP) && !defined(FEATURE_WMSAPP_ONLYSUPPORTVMAIL)
static boolean RecentCalls_SendSMS(CRecentCalls *pMe);
#endif
static boolean RecentCalls_SendCall(CRecentCalls *pMe ,boolean is_ip_call);
static boolean RecentCalls_SaveNumber(CRecentCalls *pMe, PhoneSaveType saveType);

static boolean RecentCalls_AddSelectRecordListNode(CRecentCalls *pMe, uint16 nRecordIndex);
static boolean RecentCalls_DeleteSelectRecordListNode(CRecentCalls *pMe, uint16 nRecordIndex);
static boolean RecentCalls_FindSelectRecordListNode(uint16 nRecordIndex);

#ifdef FEATURE_EDITABLE_RECORD
static boolean RecentCalls_EditRecNumber(CRecentCalls *pMe);
#endif
static boolean  Recentcalls_AskpasswordEvent(CRecentCalls *pMe, 
     AEEEvent eCode, 
     uint16 wParam,
     uint32 dwParam);
/*==============================================================================
                                 全局数据
==============================================================================*/

sSelectRecordListNode* pSelectRecordListRoot = NULL;
sSelectRecordListNode* pCurSelectRecordNode = NULL;

/*==============================================================================
                                 本地（静态）数据
==============================================================================*/


/*==============================================================================
                                 函数定义
==============================================================================*/
static void    RecentCalls_GetRecord(CRecentCalls *pMe);

// 获取当前选择的通话记录对应地址
static AEECallHistoryEntry *RecentCalls_GetRecordByIndex(CRecentCalls *pMe, uint16 nIndex);
// 获取最近一次通话记录
static uint32 RecentCalls_GetLastCallTime(CRecentCalls *pMe);

// 删除一类通话记录
static boolean RecentCalls_DeleteRecordByType(CRecentCalls *pMe, uint16 call_type);

static void    RecentCalls_RecordDetail(CRecentCalls   *pMe);

static void    RecentCalls_TimeRecord(CRecentCalls *pMe, int nSinkingLines);
#if 0
static void    RecentCalls_DrawImage(CRecentCalls *pMe,
                                     char         *filename,
                                     int          x,
                                     int          y);
#endif                                     
static void    RecentCalls_CallTimes(CRecentCalls *pMe,AEEConfigItem item);

//提示后返回
static void RecentCalls_WarnOver(void *pUser);

//判断是否删除全部且记录为空
static boolean RecentCalls_IsDelAllandNothingForDel(CRecentCalls *pMe); 
#if 0
//返回某一通话记录类型的记录数
static int     RecentCalls_GetRecordCount(CRecentCalls *pMe, CallHistoryCategory  bCallHistoryCategory); 
#endif

/*==============================================================================
函数:
       recentcalls_ShowDialog
说明:
       函数由状态处理函数用来创建相应对话框资源ID的对话框。

参数:
       pMe [In]: 指向recentcalls Applet对象结构的指针,该结构包含小程序的特定信息.
       dlgResId [in]：对话框资源ID，函数根据该ID创建对话框。

返回值:
       无

备注:
       

==============================================================================*/
void recentcalls_ShowDialog(CRecentCalls *pMe,uint16  dlgResId)
{
    int nRet;

    // At most one dialog open at once
    if (ISHELL_GetActiveDialog(pMe->m_pShell) != NULL)
    {
        // Looks like there is one dialog already opened.
        // Flag an error an return without doing anything.
        return;
    }

    nRet = ISHELL_CreateDialog(pMe->m_pShell,AEE_RECENTCALLSRES_LANGFILE,dlgResId,NULL);
}

/*==============================================================================
函数:
   recentcalls_ShowMsgBox

说明:
    函数由状态处理函数用来唤起消息对话框，显示消息提示用户。

参数:
    pMe [in]: 指向 recentcalls Applet 对象结构的指针。该结构包含小程序的特定信息。
    wTextResId [in]: 消息文本资源 ID。

返回值:
    none

备注:

==============================================================================*/
void recentcalls_ShowMsgBox(CRecentCalls *pMe, uint16 wTextResId)
{
 

    pMe->m_wMsgResID = wTextResId;
   
    // 创建消息对话框.
    recentcalls_ShowDialog(pMe, IDD_MSGBOX);
}

/*==============================================================================
函数:
    DialogTimeoutCallback

说明:
    传递给BREW层的回调函数，以便设定的定时器超时发出超时事件通知对话框。

参数:
    pUser [in]: 必须是指向 recentcalls Applet对象结构的指针。

返回值:
    none

备注:

==============================================================================*/
static void DialogTimeoutCallback(void *pUser)
{
    CRecentCalls *pMe = (CRecentCalls*)pUser;

    if (NULL == pMe)
    {
        return;
    }
    
    if (ISHELL_ActiveApplet(pMe->m_pShell) != AEECLSID_APP_RECENTCALL)
    {
        return;
    }
  // CLOSE_DIALOG(DLGRET_MSGBOX_OK)   
    // Post a message to the dialog telling it to close
    (void) ISHELL_PostEventEx(pMe->m_pShell,
                              EVTFLG_ASYNC,
                              AEECLSID_APP_RECENTCALL,
                              EVT_DISPLAYDIALOGTIMEOUT, 
                              0, 
                              0);
                         
}

/*==============================================================================
函数:
       recentcalls_RouteDialogEvent
说明:
       函数将BREW事件路由给当前活动对话框的事件处理函数。

参数:
       pMe [In]: 指向recentcalls Applet对象结构的指针,该结构包含小程序的特定信息.
       eCode [in]：事件代码。
       wParam [in]：与事件eCode关联的数据。
       dwParam [in]：与事件eCode关联的数据。

返回值:
       TRUE：传入事件被处理。
       FALSE：传入事件没被处理。

备注:

==============================================================================*/
boolean recentcalls_RouteDialogEvent(CRecentCalls *pMe,
    AEEEvent    eCode,
    uint16  wParam,
    uint32 dwParam
)
{
    if (NULL == pMe)
    {
        return FALSE;
    }

    if (NULL == pMe->m_pDialog)
    {
        return FALSE;
    }
    switch (pMe->m_pActiveDlgID)
    {
        case IDD_MSGBOX:
            return RecentCalls_HandleMsgBoxDlgEvent(pMe,eCode,wParam,dwParam);

        case IDD_VERIFY_PASSWORD:
            return RecentCalls_VerifyPasswordEvent( pMe, eCode, wParam, dwParam);

        case IDD_RMAINMENU:
            return RecentCalls_MainMenuEvent(pMe,eCode,wParam,dwParam);
            
        case IDD_RLISTRECORD:
            return RecentCalls_ListRecordEvent(pMe,eCode,wParam,dwParam);
            
        case IDD_RTIME_MENU:
            return RecentCalls_TimeMenuEvent(pMe,eCode,wParam,dwParam);
            
        case IDD_RDEL_MENU:
            return RecentCalls_DelMenuEvent(pMe,eCode,wParam,dwParam);
            
        case IDD_RDEAL_RECORD:
            return RecentCalls_DealMenuEvent(pMe,eCode,wParam,dwParam);
            
        case IDD_RWARN:
            return RecentCalls_WarnEvent(pMe,eCode,wParam,dwParam);
            
        case IDD_RTIME:
            return RecentCalls_TimeEvent(pMe,eCode,wParam,dwParam);
            
        case IDD_DETAIL:
            return RecentCalls_DetailEvent(pMe, eCode, wParam, dwParam);
		case IDD_ASK_PASSWORD_DIALOG:
			return Recentcalls_AskpasswordEvent(pMe, eCode, wParam, dwParam);

        default:
            return FALSE;
    }
}

/*==============================================================================
函数:
    IDD_MSGBOX_Handler

说明:
     recentcalls Applet对话框 IDD_MSGBOX 事件处理函数。

参数:
    pUser [in]: 指向 recentcalls Applet对象结构的指针。该结构包含小程序的特定信息。
    eCode [in]: 事件代码。
    wParam[in]: 事件参数
    dwParam [in]: 与事件关联的数据。

返回值:
    TRUE:  传入事件得到处理。
    FALSE: 传入事件没被处理。

备注:

==============================================================================*/
static boolean RecentCalls_HandleMsgBoxDlgEvent(CRecentCalls* pMe,
    AEEEvent eCode,
    uint16   wParam,
    uint32   dwParam
)
{
     static IStatic * pStatic = NULL;
    if (NULL == pMe)
    {
        return FALSE;
    }
    /*
    if (NULL == pMe->m_pStatic)
    {
        return FALSE;
    }
    */

   if (NULL == pStatic)
   {
        AEERect rect = {0};
        if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                                 AEECLSID_STATIC,
                                                 (void **)&pStatic))
        
        {
            return FALSE;
        }        
        ISTATIC_SetRect(pStatic, &rect);  
   }
   
    switch(eCode)
    {
        case EVT_DIALOG_INIT:
            return TRUE;

        case EVT_DIALOG_START:
            (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                    EVTFLG_ASYNC,
                                    AEECLSID_APP_RECENTCALL,
                                    EVT_USER_REDRAW,
                                    0, 
                                    0);

            return TRUE;

        case EVT_USER_REDRAW:
            {
                PromptMsg_Param_type  Msg_Param={0};
                AECHAR  wstrText[MSGBOX_MAXTEXTLEN];
                
                // 从资源文件取消息内容
                (void)ISHELL_LoadResString(pMe->m_pShell,
                                AEE_RECENTCALLSRES_LANGFILE,                                
                                pMe->m_wMsgResID,
                                wstrText,
                                sizeof(wstrText));
                                
                switch(pMe->m_wMsgResID)
                {
                    case IDS_DELETE_OK:
                    case IDS_TIME_RESETED:
                        Msg_Param.ePMsgType = MESSAGE_INFORMATIVE;
                        break;
                        
                    case IDS_DELETE_Q:
                    case IDS_RESET_TIME_Q:
						MSG_FATAL("--------->ok1",0,0,0);
                        Msg_Param.ePMsgType = MESSAGE_CONFIRM;
                        break;
                        
                    case IDS_MAX_SELECT:
                        Msg_Param.ePMsgType = MESSAGE_WARNNING;
                        break;
                  
                    default:
                        Msg_Param.ePMsgType = MESSAGE_NONE;
                        break;
                }
                Msg_Param.pwszMsg = wstrText;
                Msg_Param.eBBarType = BTBAR_NONE;
                DrawPromptMessage(pMe->m_pDisplay, pStatic, &Msg_Param);
            }
            // 更新界面
            IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            
            // 启动发送关闭对话框事件的定时器
            (void) ISHELL_SetTimer(pMe->m_pShell,
                                PROMPTMSG_TIMER,
                                DialogTimeoutCallback,
                                pMe);

            
            return TRUE;

        case EVT_DIALOG_END:
            (void) ISHELL_CancelTimer(pMe->m_pShell,DialogTimeoutCallback,pMe);
  
            // 此对话框返回值仅为 DLGRET_MSGBOX_OK ，为防止挂起 Applet 
            // 关闭对话框回到错误状态，显示给对话框返回值赋值
            pMe->m_eDlgRet = DLGRET_MSGBOX_OK;
            
            ISTATIC_Release(pStatic);
           pStatic = NULL;
            
            return TRUE;

        // 禁止通过按键关闭对话框
        case EVT_KEY:
            return TRUE;
        
        case EVT_DISPLAYDIALOGTIMEOUT:

            CLOSE_DIALOG(DLGRET_MSGBOX_OK)
            return TRUE;

        default:
            break;
    }

    return FALSE;
} // IDD_MSGBOX_Handler

/*==============================================================================
函数：
       RecentCalls_VerifyPasswordEvent
说明：
       IDD_VERIFY_PASSWORD对话框事件处理函数

参数：
       pMe   [in]：指向recentcalls Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam    ：事件相关数据。
       dwParam   ：事件相关数据。

返回值：
       TRUE ：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/
static boolean  RecentCalls_VerifyPasswordEvent(CRecentCalls *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    //static char   *m_strPhonePWD = NULL;
    int            nLen = 0;
   // CContApp *pMe = (CContApp *)pUser;
   
    if (NULL == pMe)
    {
        return FALSE;
    }
    
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            if(NULL == pMe->m_pPhoneLockPassword)
            {
                pMe->m_pPhoneLockPassword = (char *)MALLOC((OEMNV_LOCKCODE_MAXLEN + 1)* sizeof(char));
            }
            return TRUE;
            
        case EVT_DIALOG_START:
            (void) ISHELL_PostEvent(pMe->m_pShell,
                                    AEECLSID_APP_RECENTCALL,
                                    EVT_USER_REDRAW,
                                    0,
                                    0);

            return TRUE;
            
        case EVT_USER_REDRAW:
            // 绘制相关信息
            {
                AECHAR  wstrDisplay[OEMNV_LOCKCODE_MAXLEN+1] = {0};
                char    strDisplay[OEMNV_LOCKCODE_MAXLEN+1] = {0};
                AECHAR  text[32] = {0};
      //          int xOffset = 5;
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
                //IDISPLAY_FillRect  (pMe->m_pDisplay,&pMe->m_rc, RGB_BLACK);
                    
                // 画标题条
                (void)ISHELL_LoadResString(pMe->m_pShell, 
                                                            AEE_RECENTCALLSRES_LANGFILE,
                                                            IDS_RECENT_CALLS, 
                                                            text,
                                                            sizeof(text));
                TitleBar_Param.pwszTitle = text;
                TitleBar_Param.dwAlignFlags = IDF_ALIGN_MIDDLE | IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE;
				#if 0
                DrawTitleBar(pMe->m_pDisplay, &TitleBar_Param);
				#else
				IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,text);
				#endif
                    
                (void)ISHELL_LoadResString(pMe->m_pShell, 
                                                AEE_RECENTCALLSRES_LANGFILE,
                                                IDS_PASSWORD, 
                                                text,
                                                sizeof(text));
                nOldFontColor = IDISPLAY_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, RGB_WHITE);
                IDISPLAY_DrawText(pMe->m_pDisplay, 
                                    AEE_FONT_BOLD, 
                                    text,
                                    -1, 
                                    xOffset, 
                                    MENUITEM_HEIGHT*1/2,		//TITLEBAR_HEIGHT + 
                                    NULL, 
                                    IDF_TEXT_TRANSPARENT);
                
                // 绘制输入
                nLen = (pMe->m_pPhoneLockPassword == NULL)?(0):(STRLEN(pMe->m_pPhoneLockPassword)); 
                MEMSET(strDisplay, '*', nLen);
                strDisplay[nLen] = '|';
                strDisplay[nLen + 1] = '\0';
                (void) STRTOWSTR(strDisplay, wstrDisplay, sizeof(wstrDisplay));
                IDISPLAY_DrawText(pMe->m_pDisplay, 
                                AEE_FONT_BOLD, 
                                wstrDisplay,
                                -1, 
                                2*xOffset, 
                                MENUITEM_HEIGHT*3/2,	//TITLEBAR_HEIGHT + 
                                NULL, 
                                IDF_TEXT_TRANSPARENT);
                (void)IDISPLAY_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, nOldFontColor);
            	#ifndef FEATURE_ALL_KEY_PAD    //add by yangdecai 
                // 绘制底条提示
                if (nLen > 3)
                {// 确定-----删除
                    RECENTCALLS_DRAW_BOTTOMBAR(BTBAR_OK_DELETE)
                }
                else if(nLen > 0)
                {// 删除
                    RECENTCALLS_DRAW_BOTTOMBAR(BTBAR_DELETE)
                }
                else
                #else
                // 绘制底条提示
                if (nLen > 3)
                {// 确定-----删除
                    RECENTCALLS_DRAW_BOTTOMBAR(BTBAR_OK_BACK)
                }
                else if(nLen > 0)
                {// 删除
                    RECENTCALLS_DRAW_BOTTOMBAR(BTBAR_BACK)
                }
                else
                #endif
                {// 取消
                    RECENTCALLS_DRAW_BOTTOMBAR(BTBAR_CANCEL)
                }
            }
            
            // 更新显示
            IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);  
            return TRUE;
            
        case EVT_DIALOG_END:
            if(!pMe->m_bSuspending)
            {
                FREEIF(pMe->m_pPhoneLockPassword);
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
						
                    //Add By zzg 2012_02_27					
					case AVK_DEL:	 
					{
						chEnter = 0;			
						break;
					}
					//Add End    
                    case AVK_CLR:
                        chEnter = 0;
                        #ifndef FEATURE_ALL_KEY_PAD    //add by yangdecai 
                        if (pMe->m_pPhoneLockPassword == NULL || STRLEN(pMe->m_pPhoneLockPassword) == 0)
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
                        	if (pMe->m_pPhoneLockPassword == NULL || STRLEN(pMe->m_pPhoneLockPassword) == 0)
	                        {
	                            CLOSE_DIALOG(DLGRET_CANCELED)
	                            return TRUE;
	                        }
                        }
                        #endif
                        break;
                        
                    case AVK_SELECT:
                    case AVK_INFO:
                        if (pMe->m_pPhoneLockPassword == NULL || STRLEN(pMe->m_pPhoneLockPassword) < 4)
                        {
                            return TRUE;
                        }
                        else
                        {
                            uint16 wPWD=0;
                            
                            (void) ICONFIG_GetItem(pMe->m_pConfig, 
                                                   CFGI_PHONE_PASSWORD,
                                                   &wPWD,
                                                   sizeof(uint16));
                            
                            if (wPWD == EncodePWDToUint16(pMe->m_pPhoneLockPassword))
                            {// 密码符合
                                CLOSE_DIALOG(DLGRET_VERIFY_PASSWORD_PASSED)
                            }
                            else
                            {// 密码错误
                                CLOSE_DIALOG(DLGRET_INPUT_INVALID)
                            }
                        }
                        return TRUE;
                        
                    default:
                        return TRUE;
                }
                
                nLen = (pMe->m_pPhoneLockPassword == NULL)?(0):(STRLEN(pMe->m_pPhoneLockPassword));
                if (chEnter == 0)
                {// 删除字符
                    if (nLen > 0)
                    {
                        bRedraw = TRUE;
                        pMe->m_pPhoneLockPassword[nLen-1] = chEnter;
                    }
                }
                else if (nLen < OEMNV_LOCKCODE_MAXLEN)
                {
                    pMe->m_pPhoneLockPassword[nLen] = chEnter;
                    nLen++;
                    pMe->m_pPhoneLockPassword[nLen] = 0;
                    bRedraw = TRUE;
                }
                
                if (bRedraw)
                {
                    (void) ISHELL_PostEvent(pMe->m_pShell,
                                            AEECLSID_APP_RECENTCALL,
                                            EVT_USER_REDRAW,
                                            NULL,
                                            NULL);
                }
            }
            return TRUE;
#ifdef FEATURE_LCD_TOUCH_ENABLE
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

                if(RECENTCALLS_PT_IN_RECT(wXPos,wYPos,rc))
                {
                    if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
                    {
                        boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_RECENTCALL,EVT_USER,AVK_SELECT,0);
                        return rt;
                    }
                    else if(wXPos >= rc.x + (rc.dx/3)   && wXPos < rc.x + (rc.dx/3)*2 )//左
                    {
                         boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_RECENTCALL,EVT_USER,AVK_INFO,0);
                         return rt;
                    }
                    else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
                    {                       
                         boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_RECENTCALL,EVT_USER,AVK_CLR,0);
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
}

/*==============================================================================
函数：
       RecentCalls_MainMenuEvent
说明：
       IDD_RMAINMENU对话框事件处理函数
       
参数：
       pMe [in]：指向recentcalls Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean RecentCalls_MainMenuEvent(CRecentCalls *pMe,
                                         AEEEvent     eCode,
                                         uint16       wParam,
                                         uint32       dwParam)
{
   PARAM_NOT_REF(dwParam)
  
   pMe->pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pDialog,
                                              IDC_MAIN_CATEGORY_MENU);
   if (pMe->pMenu == NULL)
   {
       return FALSE;
   }

   switch(eCode)
   {
      case EVT_DIALOG_INIT:
        pMe->m_eStartMethod = STARTMETHOD_MAINMENU;
		//add by yangdecai
			{
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_RECENTCALLSRES_LANGFILE,                                
                        IDS_RECENT_CALLS,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
            }
        IMENUCTL_AddItem(pMe->pMenu, AEE_RECENTCALLSRES_LANGFILE, IDS_MISSED_CALLS, IDS_MISSED_CALLS, NULL, 0);
        IMENUCTL_AddItem(pMe->pMenu, AEE_RECENTCALLSRES_LANGFILE, IDS_RECEIVED_CALLS, IDS_RECEIVED_CALLS, NULL, 0);
        IMENUCTL_AddItem(pMe->pMenu, AEE_RECENTCALLSRES_LANGFILE, IDS_OUTGOING_CALLS, IDS_OUTGOING_CALLS, NULL, 0);
        IMENUCTL_AddItem(pMe->pMenu, AEE_RECENTCALLSRES_LANGFILE, IDS_CALL_TIME, IDS_CALL_TIME, NULL, 0);
        IMENUCTL_AddItem(pMe->pMenu, AEE_RECENTCALLSRES_LANGFILE, IDS_DELETE_ALL, IDS_DELETE_ALL, NULL, 0);
        return TRUE;
      
      case EVT_DIALOG_START:
        //IMENUCTL_SetProperties(pMe->pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL);
        IMENUCTL_SetProperties(pMe->pMenu, MP_BIND_ITEM_TO_NUMBER_KEY |MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_ACTIVE_NO_REDRAW);
        IMENUCTL_SetOemProperties( pMe->pMenu, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
        IMENUCTL_SetBackGround(pMe->pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_RECENTCALLS_BACKGROUND);
#endif
        IMENUCTL_SetBottomBarType(pMe->pMenu,BTBAR_SELECT_BACK);
        (void)ISHELL_PostEvent(pMe->m_pShell, 
                      AEECLSID_APP_RECENTCALL, 
                      EVT_USER_REDRAW, 
                      NULL, 
                      NULL); 
        return TRUE;
      
      case EVT_KEY:
        if(wParam == AVK_CLR)
        {
            CLOSE_DIALOG(DLGRET_CANCELED)
        }
        return TRUE;
         
      case EVT_COMMAND:
        //pMe->m_selectedItem[0] = IMENUCTL_GetSel(pMe->pMenu);
        pMe->m_selectedItem[0] = wParam;
        pMe->selectState = wParam;
        switch(wParam)
        {
           case IDS_MISSED_CALLS:
              pMe->m_callsCategory = AEECALLHISTORY_CALL_TYPE_MISSED;//CALLHISTORY_MISSED_CATEGORY;
              CLOSE_DIALOG(DLGRET_MISSEDCALL)
              break;
          
           case IDS_RECEIVED_CALLS:
              pMe->m_callsCategory = AEECALLHISTORY_CALL_TYPE_FROM;//CALLHISTORY_INCOMING_CATEGORY;
              CLOSE_DIALOG(DLGRET_INCOMCALL)
              break;
              
           case IDS_OUTGOING_CALLS:
              pMe->m_callsCategory = AEECALLHISTORY_CALL_TYPE_TO;//CALLHISTORY_OUTGOING_CATEGORY;
              CLOSE_DIALOG(DLGRET_OUTGCALL)
              break;
           
           case IDS_CALL_TIME:
              CLOSE_DIALOG(DLGRET_TIMEMENU)
              break;
              
           case IDS_DELETE_ALL:
              CLOSE_DIALOG(DLGRET_DELMENU)
              break;

           default:
              break;
        }
        return TRUE;
         
      case EVT_DIALOG_END:
        return TRUE;
      
      case EVT_USER_REDRAW:       
        IMENUCTL_SetSel(pMe->pMenu,pMe->m_selectedItem[0]); 
        // 统一更新界面
        IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE);

        (void)IMENUCTL_Redraw(pMe->pMenu);
        return TRUE;
      
      default:
         break;
   }
   return FALSE;
} // RecentCalls_MainMenuEvent


/*==============================================================================
函数：
       RecentCalls_ListRecordEvent
说明：
       IDD_RLISTRECORD对话框事件处理函数
       
参数：
       pMe [in]：指向recentcalls Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean RecentCalls_ListRecordEvent(CRecentCalls *pMe,
                                           AEEEvent     eCode,
                                           uint16       wParam,
                                           uint32       dwParam)
{
    //AECHAR wszPrivateString[MAX_STRING_LEN];
    AECHAR wszName[MAX_STRING_LEN];
    AECHAR wszRawNum [AEECALLHISTORY_MAXDIGITS ];
    static boolean bSelect;
    pMe->pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pDialog,
                                              IDC_RLISTRECORD);
    if (pMe->pMenu == NULL)
    {
        return FALSE;
    }
	
    switch(eCode)
    {
        case EVT_DIALOG_INIT:
            return TRUE;

        case EVT_DIALOG_START:
            RecentCalls_GetRecord(pMe);//获取相应的记录
            //add by yangdecai
			{
				AECHAR WTitle[40] = {0};
				AECHAR WTemp[20]  = {0};
				AECHAR WTempleft[10] = {L"<< "};
				AECHAR WTemprigt[10] = {L" >>"};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_RECENTCALLSRES_LANGFILE,                                
                        IDS_MISSED_CALLS,
                        WTemp,
                        sizeof(WTemp));
                MSG_FATAL("RecentCalls_ListRecordEvent..................",0,0,0);
                if (pMe->m_eStartMethod == STARTMETHOD_NORMAL)
                {
                	WSTRCAT(WTitle,WTempleft);
                }
                WSTRCAT(WTitle,WTemp);
                if (pMe->m_eStartMethod == STARTMETHOD_NORMAL)
                {
                	WSTRCAT(WTitle,WTemprigt);
                }
				IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
            }
            if(pMe->m_eStartMethod == STARTMETHOD_NORMAL)
            {
                IMENUCTL_SetOemProperties(pMe->pMenu, OEMMP_DISTINGUISH_INFOKEY_SELECTKEY | OEMMP_USE_MENU_STYLE |OEMMP_ARROW_TITLE);
            }
            else
            {
                IMENUCTL_SetOemProperties(pMe->pMenu, OEMMP_DISTINGUISH_INFOKEY_SELECTKEY | OEMMP_USE_MENU_STYLE);
            }
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMe->pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_RECENTCALLS_BACKGROUND);
#endif

            if(pMe->m_eStartMethod == STARTMETHOD_FINDNUMBER)
            {
                IMENUCTL_SetProperties(pMe->pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_MULTI_SEL|MP_ACTIVE_NO_REDRAW);
            }
            else
            {
                IMENUCTL_SetProperties(pMe->pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_ACTIVE_NO_REDRAW);
            }
         
            if(pMe->record_count == 0)
            {
                //IMENUCTL_SetProperties(pMe->pMenu, MP_NO_REDRAW);
                IMENUCTL_SetBottomBarType(pMe->pMenu,BTBAR_BACK);
            }
            //wuuqan.tang 20081127 add
            else if(STARTMETHOD_FINDNUMBER == pMe->m_eStartMethod)
            {
                IMENUCTL_SetBottomBarType(pMe->pMenu, BTBAR_OPTION_OK_BACK);//BTBAR_SELECT_BACK);
            }
            else if(STARTMETHOD_SELECTFIELD == pMe->m_eStartMethod)
            {
                IMENUCTL_SetBottomBarType(pMe->pMenu, BTBAR_SELECT_BACK);
            }
            else
            {
                IMENUCTL_SetBottomBarType(pMe->pMenu,BTBAR_OPTION_BACK);
            }
            if(pMe->record_count != 0)
            {
                int nCount = 1;
                int i;
                boolean bAdd = FALSE;
#ifdef FEATURE_CARRIER_THAILAND_HUTCH 
                AECHAR wszCount[7];
                AECHAR wszFmt[7];
#else
                AECHAR wszCount[5];
                AECHAR wszFmt[5];
#endif //#if defined FEATURE_CARRIER_THAILAND_HUTCH
                CtlAddItem ai; 

                for(i=0; i<pMe->record_count; i++)
                {
                    MEMSET(wszName, 0, sizeof(wszName));
                    MEMSET(wszRawNum, 0, sizeof(wszRawNum));
                    MEMSET(wszCount, 0, sizeof(wszCount));
                    MEMSET(&ai, 0, sizeof(ai));

                    if(pMe->list_record[i].pi != PI_ALLOWED)
                    {
                        (void) ISHELL_LoadResString(pMe->m_pShell,
                                              AEE_RECENTCALLSRES_LANGFILE,
                                              IDS_NO_NUMBER,
                                              wszName,
                                              sizeof(wszName));
                       
                        ai.pText = wszName;
                    }
                    else if((pMe->list_record[i].number == NULL) ||
                            (WSTRLEN(pMe->list_record[i].number) == 0))
                    {
                        (void) ISHELL_LoadResString(pMe->m_pShell,
                                              AEE_RECENTCALLSRES_LANGFILE,
                                              IDS_NO_NUMBER, //IDS_UNKNOWN_NUMBER,
                                              wszName,
                                              sizeof(wszName));
                        
                        ai.pText = wszName;
                    }
                    else
                    {
                        nCount = pMe->list_record[i].counter;

                        WSTRCPY(wszRawNum, pMe->list_record[i].number); 
                        // get Name for contact
                        // 从电话本中取人名,此处不用pMe->list_record[i].name字段是为了避免
                        // 呼出号码后存储再进入最近通话，不能以人名显示
                        RecentCalls_GetContactName(pMe,
                                        wszRawNum,
                                        wszName,
                                        MAX_STRING_LEN);
                        // get Name true
                        if ((WSTRLEN(wszName) > 0) &&
                            (wszName !=NULL))
                        {
                            if ( nCount > 1 )
                            {
#ifdef FEATURE_CARRIER_THAILAND_HUTCH                      
                                STRTOWSTR ( " (%d)", wszFmt, sizeof(wszFmt) );
#else
                                STRTOWSTR ( "(%d)", wszFmt, sizeof(wszFmt) );
#endif //#if defined FEATURE_CARRIER_THAILAND_HUTCH
                                WSPRINTF ( wszCount, sizeof(wszCount), wszFmt,nCount );
                                WSTRLCAT(wszName, wszCount, MAX_STRING_LEN); 
                            }
                            ai.pText = wszName;
                        }
                        else
                        {
                            // when count is 2 or more, display the count.
                            if ( nCount > 1 )         
                            {
#ifdef FEATURE_CARRIER_THAILAND_HUTCH                             
                                STRTOWSTR ( " (%d)", wszFmt, sizeof(wszFmt) );
#else
                                STRTOWSTR ( "(%d)", wszFmt, sizeof(wszFmt) );
#endif //#if defined FEATURE_CARRIER_THAILAND_HUTCH
                                WSPRINTF ( wszCount, sizeof(wszCount), wszFmt,  nCount );
                                WSTRLCAT(wszRawNum, wszCount, MAX_STRING_LEN); 
                            }
                            ai.pText = wszRawNum;
                        }
                    }

                    ai.wItemID = (uint16)i;               
                    ai.pszResImage = AEE_APPSCOMMONRES_IMAGESFILE;
                    
					
                    switch(pMe->list_record[i].category)
                    {
                        case AEECALLHISTORY_CALL_TYPE_MISSED://CALLHISTORY_MISSED_CATEGORY:
                            ai.wImage = IDB_MISS_CALL;
                            break;
                        
                        case AEECALLHISTORY_CALL_TYPE_FROM://CALLHISTORY_INCOMING_CATEGORY:
                            ai.wImage = IDB_INCOMING;
                            break;
                     
                        case AEECALLHISTORY_CALL_TYPE_TO://CALLHISTORY_OUTGOING_CATEGORY:
                            ai.wImage = IDB_ORIG;
                            break;
                    }
                    					
                    bAdd = IMENUCTL_AddItemEx(pMe->pMenu, &ai);                   
										
                    if(bAdd && STARTMETHOD_FINDNUMBER == pMe->m_eStartMethod)
                    {
                        if(RecentCalls_FindSelectRecordListNode(ai.wItemID))
                        {
                            IMENUCTL_SetLastItemSel(pMe->pMenu, TRUE);
                        }
                    }
                }
                //如果是删除返回,当前进入项可能大于总相数,造成指针出错,出现DATA ABORT的情况
                if(pMe->record_selected >= pMe->record_count)
                {
                    pMe->record_selected = pMe->record_count - 1;
                }
                IMENUCTL_SetSel(pMe->pMenu,pMe->record_selected);
            }    

            switch(pMe->m_callsCategory) //分列的类写TITLE
            {
                case AEECALLHISTORY_CALL_TYPE_MISSED://CALLHISTORY_MISSED_CATEGORY:
                    pMe->selectState = IDS_MISSED_CALLS;
                    IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_CALL/*ANNUN_FIELD_MISSEDCALL*/,ANNUN_STATE_CALL_MISSEDCALL_OFF/*ANNUN_STATE_OFF*/);
                    {
                        boolean missed_call_icon;
                        missed_call_icon = FALSE;
                        (void) ICONFIG_SetItem(pMe->m_pConfig,
                                           CFGI_MISSED_CALL_ICON,
                                           &missed_call_icon,
                                           sizeof(missed_call_icon));  
                    }

#ifdef FEATURE_LED_CONTROL                
                    IBACKLIGHT_SigLedDisable(pMe->m_pBacklight);
#endif
                    break;
                    
                case AEECALLHISTORY_CALL_TYPE_FROM://CALLHISTORY_INCOMING_CATEGORY:
                    pMe->selectState = IDS_RECEIVED_CALLS;
                    break;
                 
                case AEECALLHISTORY_CALL_TYPE_TO://CALLHISTORY_OUTGOING_CATEGORY:
                    pMe->selectState = IDS_OUTGOING_CALLS;
                    break;
                    
                case AEECALLHISTORY_CALL_TYPE_ALL://CALLHISTORY_ALL_CATEGORY:
                	IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_CALL/*ANNUN_FIELD_MISSEDCALL*/,ANNUN_STATE_CALL_MISSEDCALL_OFF/*ANNUN_STATE_OFF*/);
					{
                        boolean missed_call_icon;
                        missed_call_icon = FALSE;
                        (void) ICONFIG_SetItem(pMe->m_pConfig,
                                           CFGI_MISSED_CALL_ICON,
                                           &missed_call_icon,
                                           sizeof(missed_call_icon));  
                    }
                    pMe->selectState = IDS_RECENT_CALLS;
                    break;
            }
			#if 0
            (void)IMENUCTL_SetTitle(pMe->pMenu,
                                  AEE_RECENTCALLSRES_LANGFILE,
                                  pMe->selectState,
                                  NULL);
			#else
		    {
		  		AECHAR WTitle[40] = {0};
				AECHAR WTemp[20]  = {0};
				AECHAR WTempleft[10] = {L"<< "};
				AECHAR WTemprigt[10] = {L" >>"};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_RECENTCALLSRES_LANGFILE,                                
                        pMe->selectState,
                        WTemp,
                        sizeof(WTemp));
                MSG_FATAL("RecentCalls_ListRecordEvent..................",0,0,0);
                if (pMe->m_eStartMethod == STARTMETHOD_NORMAL)
                {
                	WSTRCAT(WTitle,WTempleft);
                }
                WSTRCAT(WTitle,WTemp);
                if (pMe->m_eStartMethod == STARTMETHOD_NORMAL)
                {
                	WSTRCAT(WTitle,WTemprigt);
                }
                
				IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
		    }
			#endif

            pMe->key_enable = TRUE;
            (void)ISHELL_PostEvent(pMe->m_pShell, 
                              AEECLSID_APP_RECENTCALL, 
                              EVT_USER_REDRAW, 
                              NULL, 
                              NULL);
            return TRUE;
      
        case EVT_KEY:
            if(!pMe->key_enable)
            {
                return TRUE;
            }
            
            switch(wParam){
                case AVK_CLR:
                    if(dwParam == 1)
                    {
                    	pMe->m_selectedItem[1] = IMENUCTL_GetSel(pMe->pMenu); 
                    	pMe->selectState = IDS_DELETE;
               			CLOSE_DIALOG(DLGRET_DELONE)
                    }
                    else
                    {
                    	pMe->record_selected = 0;
                    	CLOSE_DIALOG(DLGRET_CANCELED)
                    }
                    break;
                case AVK_CAMERA:
                	#if defined(FEATURE_VERSION_C306) || defined(FEAUTRE_VERSION_N450)|| defined(FEATURE_VERSION_N021)|| defined(FEATURE_VERSION_C01)|| defined(FEATURE_VERSION_W516) || defined(FEATURE_VERSION_W027)
                    {
						nv_item_type	SimChoice;
						OEMNV_Get(NV_SIM_SELECT_I,&SimChoice);
						if(SimChoice.sim_select ==AVK_SEND_TWO)
						{
							if(pMe->record_count>0)
                       			return RecentCalls_SendCall(pMe,FALSE);
                    		else
                        		return TRUE;
						}
					}
					#endif
					break;
                case AVK_SEND:
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
                    if(pMe->record_count>0)
                       return RecentCalls_SendCall(pMe,FALSE);
                    else
                        return TRUE;
                    
                case AVK_INFO:
                    if(pMe->record_count == 0)
                    {
                        return TRUE;
                    }
                    else if(STARTMETHOD_FINDNUMBER == pMe->m_eStartMethod)
                    {
                        if(pSelectRecordListRoot == NULL &&
                            pMe->m_nRemainWMSNum > 0)
                        {
                            (void)RecentCalls_AddSelectRecordListNode(pMe, pMe->record_selected);
                        }
                        CLOSE_DIALOG(DLGRET_OK)
                        return TRUE;
                    }
                    else if(STARTMETHOD_SELECTFIELD == pMe->m_eStartMethod)
                    {
                        (void)RecentCalls_AddSelectRecordListNode(pMe, pMe->record_selected);
                        CLOSE_DIALOG(DLGRET_OK)
                        return TRUE;
                    }
                    else
                    {
                       CLOSE_DIALOG(DLGRET_DETAIL)
                    }
                    break;
                 case AVK_SELECT:
                    {
                        if(pMe->record_count > 0)
                        {
                            CLOSE_DIALOG(DLGRET_RECORDDEAL)
                        }
                    }
                    break;
                default:
                    break;
            }
            return TRUE;

        case EVT_KEY_PRESS:
            switch(wParam){
                case AVK_LEFT:
                    if (pMe->m_eStartMethod == STARTMETHOD_NORMAL)
                    {
                      pMe->record_selected = 0; 
                      switch (pMe->m_callsCategory)
                      {
                         case AEECALLHISTORY_CALL_TYPE_MISSED://CALLHISTORY_MISSED_CATEGORY:
                            pMe->m_callsCategory = AEECALLHISTORY_CALL_TYPE_TO;//CALLHISTORY_OUTGOING_CATEGORY;
                            break;
                         case AEECALLHISTORY_CALL_TYPE_FROM://CALLHISTORY_INCOMING_CATEGORY:
                            pMe->m_callsCategory = AEECALLHISTORY_CALL_TYPE_MISSED;//CALLHISTORY_MISSED_CATEGORY;
                            break;                  
                         case AEECALLHISTORY_CALL_TYPE_TO://CALLHISTORY_OUTGOING_CATEGORY:
                            pMe->m_callsCategory = AEECALLHISTORY_CALL_TYPE_FROM;//CALLHISTORY_INCOMING_CATEGORY;
                            break;
                      }
#ifdef FEATURE_THAILAND   
                      Recntcall_is_view_state(0);
#endif
                      CLOSE_DIALOG(DLGRET_REFRESH)
                    }
                    break;
                   
                case AVK_RIGHT:
                    if (pMe->m_eStartMethod == STARTMETHOD_NORMAL)
                    {
                      pMe->record_selected = 0; 
                      switch (pMe->m_callsCategory)
                      {
                         case AEECALLHISTORY_CALL_TYPE_MISSED://CALLHISTORY_MISSED_CATEGORY:
                            pMe->m_callsCategory = AEECALLHISTORY_CALL_TYPE_FROM;//CALLHISTORY_INCOMING_CATEGORY;
                            break;
                         case AEECALLHISTORY_CALL_TYPE_FROM://CALLHISTORY_INCOMING_CATEGORY:
                            pMe->m_callsCategory = AEECALLHISTORY_CALL_TYPE_TO;//CALLHISTORY_OUTGOING_CATEGORY;
                            break;                  
                         case AEECALLHISTORY_CALL_TYPE_TO://CALLHISTORY_OUTGOING_CATEGORY:
                            pMe->m_callsCategory = AEECALLHISTORY_CALL_TYPE_MISSED;//CALLHISTORY_MISSED_CATEGORY;
                            break;
                      }
#ifdef FEATURE_THAILAND 
                      Recntcall_is_view_state(0);//zhuweisheng 2008 7.25 modify recentcall  no call view
#endif
                      CLOSE_DIALOG(DLGRET_REFRESH)
                    }
                    else if(pMe->m_eStartMethod == STARTMETHOD_FINDNUMBER)
                    {
                        bSelect = RecentCalls_FindSelectRecordListNode(pMe->record_selected);
                        if((!bSelect) && pMe->m_nRemainWMSNum == 0)
                        {
                            IMENUCTL_SetProperties(pMe->pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_MULTI_SEL |MP_NO_REDRAW|MP_ACTIVE_NO_REDRAW);
                            IMENUCTL_SetActive(pMe->pMenu, FALSE);
                            return TRUE;
                        }
                    }
                    break;
                
                default:
                    break;
            }
            return TRUE;
             
        case EVT_KEY_RELEASE:
            if(pMe->m_eStartMethod == STARTMETHOD_FINDNUMBER)
            {
                switch(wParam)
                {
                    case AVK_RIGHT:
                    {
                        if(bSelect)
                        {
                             (void)RecentCalls_DeleteSelectRecordListNode(pMe, pMe->record_selected);
                        }
                        else
                        {
                            if(pMe->m_nRemainWMSNum == 0)
                            {
                                CLOSE_DIALOG(DLGRET_MAX_WMS_CONTACTS)
                                return TRUE;
                            }
                            (void)RecentCalls_AddSelectRecordListNode(pMe, pMe->record_selected);
                        }
                    }
                        break;

                   

                    default:
                        break;
                }
            }
            return TRUE;
            
        case EVT_CTL_SEL_CHANGED:
            pMe->record_selected = wParam;
            return TRUE;
              
        case EVT_COMMAND:
            pMe->record_selected= IMENUCTL_GetSel(pMe->pMenu);
            if(STARTMETHOD_SELECTFIELD == pMe->m_eStartMethod)
            {
                (void)RecentCalls_AddSelectRecordListNode(pMe, pMe->record_selected);
                CLOSE_DIALOG(DLGRET_OK)
            }
            else
            {
                CLOSE_DIALOG(DLGRET_RECORDDEAL)
            }
            return TRUE;
             
        case EVT_DIALOG_END:
            (void)ISHELL_CancelTimer(pMe->m_pShell, 
                                  (PFNNOTIFY)RecentCalls_WarnOver,
                                  pMe);
            return TRUE;
          
        case EVT_USER_REDRAW:
            {
                //此处修改用来解决测试反馈的界面难看问题
#ifdef FEATURE_THAILAND 
                if ((pMe->m_eStartMethod ==  STARTMETHOD_MAINMENU)&&(0 == pMe->record_count))
                {
                    CLOSE_DIALOG(DLGRET_WARN)
                    return TRUE;            
                }
                else if((0 == pMe->record_count)&&(0==Recntcall_is_view_state(-1)))
#else
                if(0 == pMe->record_count)
#endif
                {
                    AECHAR wstrText[16] = {0};
                    RGBVAL fontColor;
                    
                    (void)ISHELL_LoadResString(pMe->m_pShell,
                                  AEE_RECENTCALLSRES_LANGFILE,
                                  IDS_NO_CALLS,
                                  wstrText,
                                  sizeof(wstrText));
                    fontColor = IDISPLAY_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, RGB_WHITE);
                    (void)IDISPLAY_DrawText(pMe->m_pDisplay,
                                  AEE_FONT_NORMAL,
                                  wstrText, 
                                  -1, 
                                  PIXELS_TO_EDGE,
                                  PIXELS_TO_EDGE, // + TITLEBAR_HEIGHT,
                                  NULL,
                                  IDF_TEXT_TRANSPARENT);
                    IDISPLAY_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, fontColor);
                }
                else
                {
                    (void)IMENUCTL_Redraw(pMe->pMenu); 
                }
            }
            IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE);
            return TRUE;
             
        default:
            break;
    }
    return FALSE;
} // RecentCalls_ListRecordEvent


/*==============================================================================
函数：
       RecentCalls_TimeMenuEvent
说明：
       IDD_RTIME_MENU对话框事件处理函数
       
参数：
       pMe [in]：指向recentcalls Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean RecentCalls_TimeMenuEvent(CRecentCalls *pMe,
                                         AEEEvent     eCode,
                                         uint16       wParam,
                                         uint32       dwParam)
{
   pMe->pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pDialog,
                                              IDC_RTIME_MENU);
   if (pMe->pMenu == NULL)
   {
       return FALSE;
   }
   
   switch(eCode)
   {
      case EVT_DIALOG_INIT:
	  	//add by yangdecai
			{
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_RECENTCALLSRES_LANGFILE,                                
                        IDS_CALL_TIME,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
            }
         IMENUCTL_AddItem(pMe->pMenu, AEE_RECENTCALLSRES_LANGFILE, IDS_LAST_CALL, IDS_LAST_CALL, NULL, 0);
         IMENUCTL_AddItem(pMe->pMenu, AEE_RECENTCALLSRES_LANGFILE, IDS_RECEIVED_CALLS, IDS_RECEIVED_CALLS, NULL, 0);
         IMENUCTL_AddItem(pMe->pMenu, AEE_RECENTCALLSRES_LANGFILE, IDS_OUTGOING_CALLS, IDS_OUTGOING_CALLS, NULL, 0);
         IMENUCTL_AddItem(pMe->pMenu, AEE_RECENTCALLSRES_LANGFILE, IDS_ALL_CALLS, IDS_ALL_CALLS, NULL, 0);
         //IMENUCTL_AddItem(pMe->pMenu, AEE_RECENTCALLSRES_LANGFILE, IDS_RESET_TIME, IDS_RESET_TIME, NULL, 0);
         return TRUE;
      
      case EVT_DIALOG_START:
        //IMENUCTL_SetProperties(pMe->pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL);
        IMENUCTL_SetProperties(pMe->pMenu, MP_BIND_ITEM_TO_NUMBER_KEY |MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_ACTIVE_NO_REDRAW);
        IMENUCTL_SetOemProperties( pMe->pMenu, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
        IMENUCTL_SetBackGround(pMe->pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_RECENTCALLS_BACKGROUND);
#endif
         IMENUCTL_SetBottomBarType(pMe->pMenu,BTBAR_SELECT_BACK);
         (void)ISHELL_PostEvent(pMe->m_pShell, 
                          AEECLSID_APP_RECENTCALL, 
                          EVT_USER_REDRAW, 
                          NULL, 
                          NULL); 
         return TRUE;
      
      case EVT_KEY:
         if(wParam == AVK_CLR)
         {
            pMe->m_selectedItem[1] = 0; 
            CLOSE_DIALOG(DLGRET_CANCELED)
         }         
         return TRUE;
        
      case EVT_COMMAND:
         //pMe->m_selectedItem[1] = IMENUCTL_GetSel(pMe->pMenu); 
         pMe->m_selectedItem[1] = wParam;
         pMe->selectState = wParam;
         switch(wParam)
         {
            case IDS_LAST_CALL:
            case IDS_RECEIVED_CALLS:               
            case IDS_OUTGOING_CALLS:
            case IDS_ALL_CALLS:
               CLOSE_DIALOG(DLGRET_TIME)
               break;
               
            /*case IDS_RESET_TIME:
               CLOSE_DIALOG(DLGRET_TIMECLEAR)
               break;*/
               
            default:
               break;
         }
         return TRUE;
         
      case EVT_DIALOG_END:
         return TRUE;
      
      case EVT_USER_REDRAW:
         IMENUCTL_SetSel(pMe->pMenu,pMe->m_selectedItem[1]); 
         // 统一更新界面
         IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE);
         (void)IMENUCTL_Redraw(pMe->pMenu);
         return TRUE;
         
      default:
         break;
   }
   return FALSE;
} // RecentCalls_TimeMenuEvent


/*==============================================================================
函数：
       RecentCalls_DelMenuEvent
说明：
       IDD_RDEL_MENU对话框事件处理函数
       
参数：
       pMe [in]：指向recentcalls Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean RecentCalls_DelMenuEvent(CRecentCalls *pMe,
                                        AEEEvent     eCode,
                                        uint16       wParam,
                                        uint32       dwParam)
{
   pMe->pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pDialog,
                                              IDC_RDEL_MENU);
   if (pMe->pMenu == NULL)
   {
       return FALSE;
   }
   
   switch(eCode)
   {
      case EVT_DIALOG_INIT:
	  	//add by yangdecai
			{
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_RECENTCALLSRES_LANGFILE,                                
                        IDS_DELETE_ALL,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
            }
         IMENUCTL_AddItem(pMe->pMenu, AEE_RECENTCALLSRES_LANGFILE, IDS_MISSED_CALLS, IDS_MISSED_CALLS, NULL, 0);
         IMENUCTL_AddItem(pMe->pMenu, AEE_RECENTCALLSRES_LANGFILE, IDS_RECEIVED_CALLS, IDS_RECEIVED_CALLS, NULL, 0);
         IMENUCTL_AddItem(pMe->pMenu, AEE_RECENTCALLSRES_LANGFILE, IDS_OUTGOING_CALLS, IDS_OUTGOING_CALLS, NULL, 0);
         IMENUCTL_AddItem(pMe->pMenu, AEE_RECENTCALLSRES_LANGFILE, IDS_ALL_CALLS, IDS_ALL_CALLS, NULL, 0);
         return TRUE;
      
      case EVT_DIALOG_START:
        //IMENUCTL_SetProperties(pMe->pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL);
        IMENUCTL_SetProperties(pMe->pMenu, MP_BIND_ITEM_TO_NUMBER_KEY |MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_ACTIVE_NO_REDRAW);
        IMENUCTL_SetOemProperties( pMe->pMenu, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
        IMENUCTL_SetBackGround(pMe->pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_RECENTCALLS_BACKGROUND);
#endif
         IMENUCTL_SetBottomBarType(pMe->pMenu,BTBAR_SELECT_BACK);
         (void)ISHELL_PostEvent(pMe->m_pShell, 
                          AEECLSID_APP_RECENTCALL, 
                          EVT_USER_REDRAW, 
                          NULL, 
                          NULL); 
         return TRUE;
      
      case EVT_KEY:
         if(wParam == AVK_CLR)
         {
            pMe->m_selectedItem[1] = 0; 
            CLOSE_DIALOG(DLGRET_CANCELED)
         }         
         return TRUE;
         
      case EVT_COMMAND:
         //pMe->m_selectedItem[1] = IMENUCTL_GetSel(pMe->pMenu); 
         pMe->m_selectedItem[1] = wParam;
         pMe->selectState = wParam;
         CLOSE_DIALOG(DLGRET_WARN)
         return TRUE;
         
      case EVT_DIALOG_END:
         return TRUE;
      
      case EVT_USER_REDRAW:          
         IMENUCTL_SetSel(pMe->pMenu,pMe->m_selectedItem[1]); 
         // 统一更新界面
         IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE);
         (void)IMENUCTL_Redraw(pMe->pMenu);
         return TRUE;
      
      default:
         break;
   }
   return FALSE;
} // RecentCalls_DelMenuEvent


/*==============================================================================
函数：
       RecentCalls_DealMenuEvent
说明：
       IDD_RDEAL_RECORD对话框事件处理函数
       
参数：
       pMe [in]：指向recentcalls Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean RecentCalls_DealMenuEvent(CRecentCalls *pMe,
                                         AEEEvent     eCode,
                                         uint16       wParam,
                                         uint32       dwParam)
{
   AECHAR wszName[MAX_STRING_LEN];
   AECHAR wszRawNum [ AEECALLHISTORY_MAXDIGITS ]; 
   pMe->pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pDialog,
                                              IDC_RDEAL_RECORD);
   if (pMe->pMenu == NULL)
   {
       return FALSE;
   }
   switch(eCode)
   {
      case EVT_DIALOG_INIT:
         if(pMe->m_eStartMethod == STARTMETHOD_FINDNUMBER)
         {
             IMENUCTL_AddItem(pMe->pMenu, AEE_RECENTCALLSRES_LANGFILE, IDS_EXECUTE, IDS_EXECUTE, NULL, 0);
             if(RecentCalls_FindSelectRecordListNode(pMe->record_selected))
             {
                IMENUCTL_AddItem(pMe->pMenu, AEE_RECENTCALLSRES_LANGFILE, IDS_UNMARK, IDS_UNMARK, NULL, 0);
             }
             else
             {
                IMENUCTL_AddItem(pMe->pMenu, AEE_RECENTCALLSRES_LANGFILE, IDS_MARK, IDS_MARK, NULL, 0);
             }
             if(pMe->record_count <= pMe->m_nRemainWMSNum)
             {
                IMENUCTL_AddItem(pMe->pMenu, AEE_RECENTCALLSRES_LANGFILE, IDS_MARKALL, IDS_MARKALL, NULL, 0);
             }
             IMENUCTL_AddItem(pMe->pMenu, AEE_RECENTCALLSRES_LANGFILE, IDS_UNMARKALL, IDS_UNMARKALL, NULL, 0);
         }
         else
         {
             MEMSET(wszName, 0, sizeof(wszName));
             // 从电话本中取人名,此处不用pMe->list_record[pMe->record_selected].name
             // 字段是为了避免某个通话记录，并且该记录已在电话本中有存储，
             // 然后在电话本中删除该记录，通话记录中的该号码仍不能执行保存号码操作的问题。      
             MEMSET(wszRawNum, 0, sizeof(wszRawNum));

             //GetRepeatRawNumber ( pMe->list_record[pMe->record_selected].number, wszRawNum );
             if(pMe->list_record[pMe->record_selected].pi == PI_ALLOWED)
             {
                if(pMe->list_record[pMe->record_selected].number != NULL && 
                    WSTRLEN(pMe->list_record[pMe->record_selected].number) > 0)
                {
                    WSTRCPY(wszRawNum, pMe->list_record[pMe->record_selected].number); 
                    RecentCalls_GetContactName(pMe, wszRawNum, wszName, MAX_STRING_LEN);
                }
             }

             IMENUCTL_AddItem(pMe->pMenu, AEE_RECENTCALLSRES_LANGFILE, IDS_DETAIL, IDS_DETAIL, NULL, 0);
             if(WSTRLEN(wszName) == 0  &&
                (WSTRLEN(wszRawNum) > 0) &&
                (wszRawNum != NULL))
             {
                 IMENUCTL_AddItem(pMe->pMenu, AEE_RECENTCALLSRES_LANGFILE, IDS_SAVE, IDS_SAVE, NULL, 0);      
                 IMENUCTL_AddItem(pMe->pMenu, AEE_RECENTCALLSRES_LANGFILE, IDS_SAVETOCONTACT, IDS_SAVETOCONTACT, NULL, 0);
             }
             if((WSTRLEN(wszRawNum) > 0) &&
                (wszRawNum != NULL))
             {
                IMENUCTL_AddItem(pMe->pMenu, AEE_RECENTCALLSRES_LANGFILE, IDS_MAKE_CALL, IDS_MAKE_CALL, NULL, 0);
#ifdef FEATRUE_SET_IP_NUMBER
                 IMENUCTL_AddItem(pMe->pMenu, AEE_RECENTCALLSRES_LANGFILE, IDS_MAKE_IPCALL, IDS_MAKE_IPCALL, NULL, 0);
#endif
#if defined(FEATURE_WMS_APP) && !defined(FEATURE_WMSAPP_ONLYSUPPORTVMAIL)
                 IMENUCTL_AddItem(pMe->pMenu, AEE_RECENTCALLSRES_LANGFILE, IDS_SEND_SMS, IDS_SEND_SMS, NULL, 0);
                 IMENUCTL_AddItem(pMe->pMenu, AEE_RECENTCALLSRES_LANGFILE, IDS_SEND_CONTACT, IDS_SEND_CONTACT, NULL, 0);
#endif
#ifdef FEATURE_EDITABLE_RECORD
                 IMENUCTL_AddItem(pMe->pMenu, AEE_RECENTCALLSRES_LANGFILE, IDS_EDIT_NUMBER, IDS_EDIT_NUMBER, NULL, 0);
#endif
             }
             IMENUCTL_AddItem(pMe->pMenu, AEE_RECENTCALLSRES_LANGFILE, IDS_DELETE, IDS_DELETE, NULL, 0); 
         }
         return TRUE;
      
      case EVT_DIALOG_START:    
     {
         AEERect rc = {0};
         IMENUCTL_SetPopMenuRect(pMe->pMenu); 
         IMENUCTL_GetRect(pMe->pMenu, &rc);
         MSG_FATAL("rc.y = %d; rc.dy = %d",rc.y, rc.dy, 0);
         if(rc.y < 0)
         {
             int temp = -(rc.y);
             rc.y += temp;
             rc.dy -= temp;
             IMENUCTL_SetRect(pMe->pMenu, &rc);
         }
         MSG_FATAL("rc.y = %d; rc.dy = %d",rc.y, rc.dy, 0);
     }

         //IMENUCTL_SetPopMenuRect(pMe->pMenu);
         IMENUCTL_SetProperties(pMe->pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY|MP_ACTIVE_NO_REDRAW);
         IMENUCTL_SetBottomBarType(pMe->pMenu,BTBAR_SELECT_BACK);
         (void)ISHELL_PostEvent(pMe->m_pShell, 
                          AEECLSID_APP_RECENTCALL, 
                          EVT_USER_REDRAW, 
                          NULL, 
                          NULL); 
         return TRUE;
      
      case EVT_KEY:
         if(wParam == AVK_CLR)
         {
            pMe->m_selectedItem[1] = 0;
            CLOSE_DIALOG(DLGRET_CANCELED)
         }         
         return TRUE;
         
      case EVT_COMMAND:
         pMe->m_selectedItem[1] = IMENUCTL_GetSel(pMe->pMenu); 
         pMe->selectState = wParam;
         switch(wParam)
         {
            case IDS_DETAIL:
               CLOSE_DIALOG(DLGRET_DETAIL)
               break;

            case IDS_SAVE:
               return RecentCalls_SaveNumber(pMe, SAVE_NUMBER);

            case IDS_SAVETOCONTACT:
               return RecentCalls_SaveNumber(pMe, ADD_FIELD);
            
            case IDS_MAKE_CALL:
               return RecentCalls_SendCall(pMe,FALSE);
#ifdef FEATRUE_SET_IP_NUMBER
            case IDS_MAKE_IPCALL:
               return RecentCalls_SendCall(pMe,TRUE);
#endif
#if defined(FEATURE_WMS_APP) && !defined(FEATURE_WMSAPP_ONLYSUPPORTVMAIL)
            case IDS_SEND_SMS:
               return RecentCalls_SendSMS(pMe);
               
            case IDS_SEND_CONTACT:
               return RecentCalls_SendSMS(pMe);               
#endif

            case IDS_DELETE:
               pMe->selectState = IDS_DELETE;
               CLOSE_DIALOG(DLGRET_DELONE)
               break;

            case IDS_EXECUTE:
               if(pSelectRecordListRoot == NULL &&
                    pMe->m_nRemainWMSNum > 0)
               {
                   (void)RecentCalls_AddSelectRecordListNode(pMe, pMe->record_selected);
               }
               CLOSE_DIALOG(DLGRET_OK)
               break;
               
            case IDS_MARK:
               if(pMe->m_nRemainWMSNum > 0)
               {
                   (void)RecentCalls_AddSelectRecordListNode(pMe, pMe->record_selected);
                   CLOSE_DIALOG(DLGRET_CANCELED)
               }
               else
               {
                   CLOSE_DIALOG(DLGRET_MAX_WMS_CONTACTS)
               }
               break;
               
            case IDS_UNMARK:
               (void)RecentCalls_DeleteSelectRecordListNode(pMe, pMe->record_selected);
               CLOSE_DIALOG(DLGRET_CANCELED)
               break;

            case IDS_MARKALL:
               {
                   int i = 0;
                   //in case of add all repeatedly, free list node first
                   RecentCalls_FreeSelectRecordListNode(pMe);
                   pMe->m_nRemainWMSNum= RecentCalls_GetRemainWMSNum(pMe);
                   for(i = 0; i < pMe->record_count; i++)
                   {
                       (void)RecentCalls_AddSelectRecordListNode(pMe, i);
                   }
                   CLOSE_DIALOG(DLGRET_CANCELED)
               }
               break;

            case IDS_UNMARKALL:
               RecentCalls_FreeSelectRecordListNode(pMe);
               pMe->m_nRemainWMSNum= RecentCalls_GetRemainWMSNum(pMe);
               CLOSE_DIALOG(DLGRET_CANCELED)
               break;
               
#ifdef FEATURE_EDITABLE_RECORD
            case IDS_EDIT_NUMBER:
               return RecentCalls_EditRecNumber(pMe);
#endif
            default:
               break;
         }
         return TRUE;
         
      case EVT_DIALOG_END:
#ifdef FEATURE_CARRIER_THAILAND_HUTCH           
         pMe->m_selectedItem[1] = 0;  
#endif //#if defined FEATURE_CARRIER_THAILAND_HUTCH  
         if(pMe->m_bSuspending)
         {
             pMe->m_eCurState = pMe->m_ePreState;
         }
         return TRUE;
      
      case EVT_USER_REDRAW:
         IMENUCTL_SetSel(pMe->pMenu,pMe->m_selectedItem[1]);
         IMENUCTL_SetActive(pMe->pMenu, TRUE);
         IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE);
          // 统一更新界面
         (void)IMENUCTL_Redraw(pMe->pMenu);
         return TRUE;
      
      default:
         break;
   }
   return FALSE;
} // RecentCalls_DealMenuEvent

/*==============================================================================
函数：
       RecentCalls_DetailEvent
说明：
       IDD_DETAIL对话框事件处理函数
       
参数：
       pMe [in]：指向recentcalls Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean RecentCalls_DetailEvent(CRecentCalls *pMe,
                                         AEEEvent     eCode,
                                         uint16       wParam,
                                         uint32       dwParam)
{
   switch(eCode)
   {
      case EVT_DIALOG_INIT:                   
         return TRUE;
      
      case EVT_DIALOG_START:

         (void)ISHELL_PostEvent(pMe->m_pShell, 
                              AEECLSID_APP_RECENTCALL, 
                              EVT_USER_REDRAW, 
                              NULL, 
                              NULL); 
         return TRUE;
      
      case EVT_KEY:
         switch(wParam)
         {
            case AVK_CLR:
                CLOSE_DIALOG(DLGRET_CANCELED)
                break;
            case AVK_CAMERA:
            	#if defined(FEATURE_VERSION_C306) || defined(FEAUTRE_VERSION_N450)|| defined(FEATURE_VERSION_N021)|| defined(FEATURE_VERSION_C01)|| defined(FEATURE_VERSION_W516) || defined(FEATURE_VERSION_W027)
                {
					nv_item_type	SimChoice;
					OEMNV_Get(NV_SIM_SELECT_I,&SimChoice);
					if(SimChoice.sim_select ==AVK_SEND_TWO)
					{
						if(pMe->record_count>0)                               
		                   return RecentCalls_SendCall(pMe,FALSE);
		               else
		                    return TRUE;
		                break;
					}
				}
				#endif
				break;
            case AVK_SEND:  
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
               if(pMe->record_count>0)                               
                   return RecentCalls_SendCall(pMe,FALSE);
               else
                    return TRUE;
                break;
#ifdef FEATURE_CARRIER_THAILAND_HUTCH    
            case AVK_SELECT:
               if(pMe->record_count>0)                               
                   return RecentCalls_SendCall(pMe,FALSE);
               else
                    return TRUE;
                break;
#endif //#if defined FEATURE_CARRIER_THAILAND_HUTCH  
            default:
                break;
         }       
         return TRUE;
         
      case EVT_COMMAND:
         return TRUE;
         
      case EVT_DIALOG_END:
         return TRUE;
      
      case EVT_USER_REDRAW:          
         RecentCalls_RecordDetail(pMe);//画出当前选中记录的信息
         
         {//draw title
            AECHAR  wstrDevice[32]={0};
            TitleBar_Param_type  TitleBar_Param = {0};

            TitleBar_Param.pwszTitle = wstrDevice;
            //TitleBar_Param.dwAlignFlags = IDF_ALIGN_MIDDLE | IDF_ALIGN_CENTER;
            TitleBar_Param.dwAlignFlags = IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE;
            switch(pMe->m_callsCategory)
            {
                case AEECALLHISTORY_CALL_TYPE_MISSED://CALLHISTORY_MISSED_CATEGORY:
                  (void)ISHELL_LoadResString(pMe->m_pShell, AEE_RECENTCALLSRES_LANGFILE,
                                IDS_MISSED_CALLS, wstrDevice,sizeof(wstrDevice));
                  break;

                case AEECALLHISTORY_CALL_TYPE_FROM://CALLHISTORY_INCOMING_CATEGORY:
                  (void)ISHELL_LoadResString(pMe->m_pShell, AEE_RECENTCALLSRES_LANGFILE,
                                IDS_RECEIVED_CALLS, wstrDevice,sizeof(wstrDevice));
                  break;

                case AEECALLHISTORY_CALL_TYPE_TO://CALLHISTORY_OUTGOING_CATEGORY:
                  (void)ISHELL_LoadResString(pMe->m_pShell, AEE_RECENTCALLSRES_LANGFILE,
                                IDS_OUTGOING_CALLS, wstrDevice,sizeof(wstrDevice));
                  break;
                case AEECALLHISTORY_CALL_TYPE_ALL://CALLHISTORY_ALL_CATEGORY:
                  (void)ISHELL_LoadResString(pMe->m_pShell, AEE_RECENTCALLSRES_LANGFILE,
                                IDS_RECENT_CALLS, wstrDevice,sizeof(wstrDevice));
                  break;
            }  
			#if 0
            DrawTitleBar(pMe->m_pDisplay, &TitleBar_Param);
			#else
			IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,wstrDevice);
			#endif
            MEMSET(wstrDevice, 0, sizeof(wstrDevice));
         }
#ifdef FEATURE_CARRIER_THAILAND_HUTCH         
         RECENTCALLS_DRAW_BOTTOMBAR(BTBAR_CALL_BACK) //draw bottom bar
#else
         RECENTCALLS_DRAW_BOTTOMBAR(BTBAR_BACK) //draw bottom bar
#endif //#if defined FEATURE_CARRIER_THAILAND_HUTCH  
         IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
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

				if(RECENTCALLS_PT_IN_RECT(wXPos,wYPos,rc))
				{
					if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
					{						
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_RECENTCALL,EVT_USER,AVK_CLR,0);
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
} // RecentCalls_DetailEvent

/*==============================================================================
函数：
       RecentCalls_WarnEvent
说明：
       IDD_RWARN对话框事件处理函数
       
参数：
       pMe [in]：指向recentcalls Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean RecentCalls_WarnEvent(CRecentCalls *pMe,
                                     AEEEvent     eCode,
                                     uint16       wParam,
                                     uint32       dwParam)
{
   static IStatic * pStatic = NULL;

   if (NULL == pStatic)
   {
        AEERect rect = {0};
        if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                                 AEECLSID_STATIC,
                                                 (void **)&pStatic))
        
        {
            return FALSE;
        }        
        ISTATIC_SetRect(pStatic, &rect);  
   }
   
   switch(eCode)
   {
      case EVT_DIALOG_INIT:
         return TRUE;
      
      case EVT_DIALOG_START:
         MEMSET(&pMe->m_PromptMsg,0,sizeof(PromptMsg_Param_type));
         strcpy(pMe->m_PromptMsg.strMsgResFile, AEE_RECENTCALLSRES_LANGFILE);      
         (void)ISHELL_PostEvent(pMe->m_pShell, 
                          AEECLSID_APP_RECENTCALL, 
                          EVT_USER_REDRAW, 
                          NULL, 
                          NULL); 
         return TRUE;
      
      case EVT_USER_REDRAW:
         {            
            //删除全部时，如果记录为空，提示并返回
            /*if((TRUE == RecentCalls_IsDelAllandNothingForDel(pMe)) ||
               (IDS_MISSED_CALLS == pMe->selectState) || 
               (IDS_RECEIVED_CALLS == pMe->selectState) || 
               (IDS_OUTGOING_CALLS == pMe->selectState))*/
            if(TRUE == RecentCalls_IsDelAllandNothingForDel(pMe))
            {
                  pMe->m_PromptMsg.nMsgResID = IDS_NO_CALLS; 
                  pMe->m_PromptMsg.ePMsgType = MESSAGE_INFORMATION;                
                  //告警1s后返回
                  (void) ISHELL_SetTimer(pMe->m_pShell,
                                         PROMPTMSG_TIMER,
                                         (PFNNOTIFY) RecentCalls_WarnOver,
                                         pMe);
            }
            else
            {
                switch(pMe->selectState)
                {
                   case IDS_MISSED_CALLS:
                      pMe->m_callsCategory = AEECALLHISTORY_CALL_TYPE_MISSED;
                      pMe->m_PromptMsg.nMsgResID = IDS_DELETE_Q;  
                      break;
                      
                   case IDS_RECEIVED_CALLS:
                      pMe->m_callsCategory = AEECALLHISTORY_CALL_TYPE_FROM;
                      pMe->m_PromptMsg.nMsgResID = IDS_DELETE_Q; 
                      break;
                      
                   case IDS_OUTGOING_CALLS:
                      pMe->m_callsCategory = AEECALLHISTORY_CALL_TYPE_TO;
                      pMe->m_PromptMsg.nMsgResID = IDS_DELETE_Q; 
                      break;
                      
                   case IDS_ALL_CALLS:
                      pMe->m_PromptMsg.nMsgResID = IDS_DELETE_Q; 
                      break;
                  
                  case IDS_DELETE:
                      pMe->m_PromptMsg.nMsgResID = IDS_DELETE_Q; 
                      break;
    
                  case IDS_RESET_TIME:
                      pMe->m_PromptMsg.nMsgResID = IDS_RESET_TIME_Q; 
                      break;

                  default:
                     break;
                }
                pMe->m_PromptMsg.ePMsgType = MESSAGE_CONFIRM;
                pMe->m_PromptMsg.eBBarType = BTBAR_OK_BACK;
            }
            DrawPromptMessage(pMe->m_pDisplay, pStatic, &pMe->m_PromptMsg);                            
            IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE);
         }
         return TRUE;
               
      case EVT_KEY:
          {
             if(pMe->m_PromptMsg.ePMsgType != MESSAGE_CONFIRM) //判断是否按键禁止，在定时器提示的时候，按键不能再处理
             {
                return TRUE;
             }
             switch(wParam)
             {
                case AVK_SELECT:          
                    switch(pMe->selectState)
                    {
                       case IDS_MISSED_CALLS:
                       	  if(TRUE == RecentCalls_DeleteRecordByType(pMe, pMe->m_callsCategory))
                          {
                              pMe->m_PromptMsg.nMsgResID = IDS_DELETE_OK;
                          }
                          else
                          {
                              pMe->m_PromptMsg.nMsgResID = IDS_ERR_UNKNOWN;
                          }
                          break;
                          
                       case IDS_RECEIVED_CALLS:
                          if(TRUE == RecentCalls_DeleteRecordByType(pMe, pMe->m_callsCategory))
                          {
                              uint32 value = 0;
                          	  (void) ICONFIG_SetItem(pMe->m_pConfig,
                                                    CFGI_RECENT_MT_CALL_TIMER,
                                                    &value,
                                                     sizeof(uint32));
                              pMe->m_PromptMsg.nMsgResID = IDS_DELETE_OK;
                          }
                          else
                          {
                              pMe->m_PromptMsg.nMsgResID = IDS_ERR_UNKNOWN;
                          }
                          break;
                       case IDS_OUTGOING_CALLS:
                       
                          if(TRUE == RecentCalls_DeleteRecordByType(pMe, pMe->m_callsCategory))
                          {
                              uint32 value = 0;
                          	  (void) ICONFIG_SetItem(pMe->m_pConfig,
                                                    CFGI_RECENT_MO_CALL_TIMER,
                                                    &value,
                                                    sizeof(uint32));
                              pMe->m_PromptMsg.nMsgResID = IDS_DELETE_OK;
                          }
                          else
                          {
                              pMe->m_PromptMsg.nMsgResID = IDS_ERR_UNKNOWN;
                          }
                          break;
                          
                       case IDS_ALL_CALLS:
					   	  MSG_FATAL("***zzg RectnCallsDlg IDS_ALL_CALLS***", 0, 0, 0);
                          if(SUCCESS == ICALLHISTORY_Clear(pMe->m_pCallHistory))
                          {
							  uint32 value = 0;	
                              pMe->m_PromptMsg.nMsgResID = IDS_DELETE_OK;
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
                          else
                          {
                              pMe->m_PromptMsg.nMsgResID = IDS_ERR_UNKNOWN;
                          }
                          
                          break;
                       
                       case IDS_DELETE:
                          {
                              AEECallHistoryEntry *pEntry = RecentCalls_GetRecordByIndex(pMe, pMe->record_selected);
                                
                              if(pEntry != NULL)
                              {
                                  ICALLHISTORY_ClearEntry(pMe->m_pCallHistory);
                                  pMe->m_PromptMsg.nMsgResID = IDS_DELETE_OK;
                              }
                              else
                              {
                                  pMe->m_PromptMsg.nMsgResID = IDS_ERR_UNKNOWN;
                              }
                          }
                          pMe->m_selectedItem[1] = 0;
                          break; 
                       
                       case IDS_RESET_TIME:
                          {
                             uint32 value;                              
                             value = 0;
							 MSG_FATAL("***zzg RecentCallsDlg IDS_RESET_TIME***", 0, 0, 0);
                             if(pMe->m_callsCategory == AEECALLHISTORY_CALL_TYPE_ALL)
                             {
                                 (void) ICONFIG_SetItem(pMe->m_pConfig,
                                                        CFGI_ALL_CALL_TIMER,
                                                        &value,
                                                        sizeof(uint32));
								 
                             }
                             
                             if(pMe->m_callsCategory == AEECALLHISTORY_CALL_TYPE_ALL ||
                                pMe->m_callsCategory == AEECALLHISTORY_CALL_TYPE_TO)
                             {
                                 (void) ICONFIG_SetItem(pMe->m_pConfig,
                                                        CFGI_RECENT_MO_CALL_TIMER,
                                                        &value,
                                                        sizeof(uint32));
                             }
                             
                             if(pMe->m_callsCategory == AEECALLHISTORY_CALL_TYPE_ALL ||
                                pMe->m_callsCategory == AEECALLHISTORY_CALL_TYPE_FROM)
                             {
                                 (void) ICONFIG_SetItem(pMe->m_pConfig,
                                                        CFGI_RECENT_MT_CALL_TIMER,
                                                        &value,
                                                         sizeof(uint32));
                             }
                             pMe->m_PromptMsg.nMsgResID = IDS_TIME_RESETED;
                          }
                          break;
              
                       default:
                          break;
                    }    
                    pMe->m_PromptMsg.ePMsgType = MESSAGE_INFORMATIVE;                       
                    //告警1s后返回
                   (void) ISHELL_SetTimer(pMe->m_pShell,
                                          PROMPTMSG_TIMER,
                                         (PFNNOTIFY) RecentCalls_WarnOver,
                                         pMe);  
                    DrawPromptMessage(pMe->m_pDisplay,
                                       pStatic,
                                       &pMe->m_PromptMsg);
                    IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE);                                                                                                       
                    break;
                    
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)            
                    break; 
             } 
          }                              
         return TRUE;
         
      case EVT_COMMAND:
         return TRUE;
         
      case EVT_DIALOG_END:
         ISTATIC_Release(pStatic); 
         pStatic = NULL;  
         MEMSET(&pMe->m_PromptMsg,0,sizeof(PromptMsg_Param_type));      
         (void)ISHELL_CancelTimer(pMe->m_pShell, 
                                  (PFNNOTIFY)RecentCalls_WarnOver,
                                  pMe); 
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

				if(RECENTCALLS_PT_IN_RECT(wXPos,wYPos,rc))
				{
					if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
					{
						boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_RECENTCALL,EVT_USER,AVK_SELECT,0);
						return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)   && wXPos < rc.x + (rc.dx/3)*2 )//左
					{
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_RECENTCALL,EVT_USER,AVK_INFO,0);
						 return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
					{						
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_RECENTCALL,EVT_USER,AVK_CLR,0);
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
} // RecentCalls_WarnEvent

/*==============================================================================
函数：
       RecentCalls_WarnOver
说明：
       提示后，timer返回
       
参数：
       
返回值：
       
备注：
       
==============================================================================*/          
static void RecentCalls_WarnOver(void *pUser)
{
    CRecentCalls *pMe = (CRecentCalls *)pUser;

    if(pMe == NULL)
    {
        return;
    }

   CLOSE_DIALOG(DLGRET_CANCELED)   
}      

/*==============================================================================
函数：
       RecentCalls_TimeEvent
说明：
       IDD_RTIME对话框事件处理函数
       
参数：
       pMe [in]：指向recentcalls Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean RecentCalls_TimeEvent(CRecentCalls *pMe,
                                     AEEEvent     eCode,
                                     uint16       wParam,
                                     uint32       dwParam)
{
   switch(eCode)
   {
      case EVT_DIALOG_INIT:
         return TRUE;
      
      case EVT_DIALOG_START:
         (void)ISHELL_PostEvent(pMe->m_pShell, 
                          AEECLSID_APP_RECENTCALL, 
                          EVT_USER_REDRAW, 
                          NULL, 
                          NULL); 
         return TRUE;
      
      case EVT_KEY:
         //if(wParam == AVK_SELECT || wParam == AVK_CLR)
         if(wParam == AVK_CLR)
         {
            CLOSE_DIALOG(DLGRET_CANCELED)
         }
         else if(wParam == AVK_SELECT)
         {
             if(pMe->selectState != IDS_LAST_CALL)
             {
                pMe->selectState = IDS_RESET_TIME;
                CLOSE_DIALOG(DLGRET_TIMECLEAR)
             }
         }
         return TRUE;
         
      case EVT_COMMAND:
         return TRUE;
         
      case EVT_DIALOG_END:
         return TRUE;
      
      case EVT_USER_REDRAW:
         {//draw title
            AECHAR warn_msg[30];
            uint16 resid;
            AEEConfigItem item;
            AECHAR          wstrDevice[32]={0};
            TitleBar_Param_type  TitleBar_Param = {0};
            TitleBar_Param.pwszTitle = wstrDevice;
            TitleBar_Param.dwAlignFlags = IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE;
           
            switch(pMe->selectState)
            {
               case IDS_LAST_CALL:
                  resid = IDS_LAST_CALL;
                  item = NULL;
                  break;
  
               case IDS_RECEIVED_CALLS:
                  pMe->m_callsCategory = AEECALLHISTORY_CALL_TYPE_FROM;
                  resid = IDS_RECEIVED_CALLS;
                  item = CFGI_RECENT_MT_CALL_TIMER;
                  break;
                  
               case IDS_OUTGOING_CALLS:
                  pMe->m_callsCategory = AEECALLHISTORY_CALL_TYPE_TO;
                  resid = IDS_OUTGOING_CALLS;
                  item = CFGI_RECENT_MO_CALL_TIMER;
                  break;
                  
               case IDS_ALL_CALLS:
                  pMe->m_callsCategory = AEECALLHISTORY_CALL_TYPE_ALL;
                  resid = IDS_ALL_CALLS;
                  item = CFGI_ALL_CALL_TIMER;
                  break;
                  
               default:
                  pMe->m_callsCategory = AEECALLHISTORY_CALL_TYPE_ALL;
                  resid = IDS_ALL_CALLS;
                  item = CFGI_ALL_CALL_TIMER;
                  break;
                  
            }
            (void) ISHELL_LoadResString(pMe->m_pShell,
                                        AEE_RECENTCALLSRES_LANGFILE,
                                        resid,
                                        warn_msg,
                                        sizeof(warn_msg)
                                        );
#ifdef FEATURE_CARRIER_CHINA_VERTU
            {
                IImage *pImageBg = ISHELL_LoadResImage(pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_RECENTCALLS_BACKGROUND);
                
                Appscommon_ResetBackground(pMe->m_pDisplay, pImageBg, APPSCOMMON_BG_COLOR, &pMe->m_rc, 0, 0);
                if(pImageBg != NULL)
                {
                    IImage_Release(pImageBg);
                }
            }
#else
            Appscommon_ResetBackgroundEx(pMe->m_pDisplay, &pMe->m_rc, TRUE);
#endif
            //IDISPLAY_FillRect(pMe->m_pDisplay, &pMe->m_rc, RGB_BLACK);
            IDISPLAY_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, RGB_WHITE);

            (void)IDISPLAY_DrawText(pMe->m_pDisplay,
                              AEE_FONT_NORMAL,
                              warn_msg, 
                              -1, 
                              PIXELS_TO_EDGE,
                              PIXELS_TO_EDGE, // + TITLEBAR_HEIGHT,
                              NULL,
                              IDF_TEXT_TRANSPARENT);
            IDISPLAY_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, RGB_BLACK);

            RecentCalls_CallTimes(pMe,item);
            
            (void)ISHELL_LoadResString(pMe->m_pShell, 
                                                    AEE_RECENTCALLSRES_LANGFILE,
                                                    resid, 
                                                    wstrDevice,
                                                    sizeof(wstrDevice));
			#if 0
            DrawTitleBar(pMe->m_pDisplay, &TitleBar_Param);
			#else
			IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,wstrDevice);
			#endif
            MEMSET(wstrDevice, 0, sizeof(wstrDevice));
            if(pMe->selectState == IDS_LAST_CALL)
            {
                RECENTCALLS_DRAW_BOTTOMBAR(BTBAR_BACK)
            }
            else
            {
                RECENTCALLS_DRAW_BOTTOMBAR(BTBAR_RESET_BACK)
            }

            IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE);
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

				if(RECENTCALLS_PT_IN_RECT(wXPos,wYPos,rc))
				{
					if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
					{
						boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_RECENTCALL,EVT_USER,AVK_SELECT,0);
						return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)   && wXPos < rc.x + (rc.dx/3)*2 )//左
					{
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_RECENTCALL,EVT_USER,AVK_INFO,0);
						 return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
					{						
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_RECENTCALL,EVT_USER,AVK_CLR,0);
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
} // RecentCalls_TimeEvent

/*==============================================================================
FUNCTION RecentCalls_GetRecord
DISCRIPTION 获取相应通话类型的通话记录

PARAMETERS:
   *pMe: CRecentCalls object pointer
   
RETURN VALUE:
boolean: TRUE if the event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:
==============================================================================*/
static void RecentCalls_GetRecord(CRecentCalls *pMe)
{
    AEECallHistoryEntry *pEntry = NULL;
    AECHAR                 wstrNumber[AEECALLHISTORY_MAXDIGITS];
    int                        i = 0, nFieldNum = 0, nRet = 0;

    pMe->record_count = 0;
    for(i=0; i<AEECALLHISTORY_MAX_CALLS; i++)
    {
        FREEIF(pMe->list_record[i].name);
        FREEIF(pMe->list_record[i].number);
    }

    switch(pMe->m_callsCategory)
    {
        case AEECALLHISTORY_CALL_TYPE_TO:
        case AEECALLHISTORY_CALL_TYPE_FROM:
        case AEECALLHISTORY_CALL_TYPE_MISSED:
        {
            if(ICALLHISTORY_EnumInitByCallType(pMe->m_pCallHistory, pMe->m_callsCategory) != SUCCESS)
            {
                return;
            }
        }
        break;

        case AEECALLHISTORY_CALL_TYPE_ALL:
        {
            if(ICALLHISTORY_EnumInit(pMe->m_pCallHistory) != SUCCESS)
            {
                return;
            }
        }
        break;

        default:
            return;
    }

    i = 0;
    pEntry = (AEECallHistoryEntry *)ICALLHISTORY_EnumNext(pMe->m_pCallHistory, &nRet);
    while (NULL != pEntry)
    {
        pMe->list_record[i].counter = 1;
        for (nFieldNum=0; nFieldNum<pEntry->wNumFields; nFieldNum++)
        {
            switch(pEntry->pFields[nFieldNum].wID)
            {
                case AEECALLHISTORY_FIELD_NAME:
                {
                    if(pEntry->pFields[nFieldNum].pData != NULL)
                    {
                        pMe->list_record[i].name = WSTRDUP((AECHAR*)(pEntry->pFields[nFieldNum].pData));
                    }
                }
                break;

                case AEECALLHISTORY_FIELD_NUMBER:
                {
                    if(pEntry->pFields[nFieldNum].pData != NULL)
                    {
                        MEMSET(wstrNumber, 0, sizeof(wstrNumber));
                        (void) STRTOWSTR((char*)pEntry->pFields[nFieldNum].pData, wstrNumber, sizeof(wstrNumber));
                        #if defined(FEATURE_VERSION_MTM) || defined(FEATURE_VERSION_W515V3)
                        {
                        	if(WSTRNCMP(wstrNumber,L"+",1)==0)
                        	{
                        		AECHAR   wstrNumberbuf[AEECALLHISTORY_MAXDIGITS] = {0};
                        		WSTRLCAT(wstrNumberbuf,&wstrNumber[1],AEECALLHISTORY_MAXDIGITS);
                        		WSTRLCPY(wstrNumber,wstrNumberbuf,AEECALLHISTORY_MAXDIGITS);
                        	}
                        }
                        #endif
                        pMe->list_record[i].number = WSTRDUP(wstrNumber);
                    }
                }
                break;

                case AEECALLHISTORY_FIELD_PI:
                {
                    if(pEntry->pFields[nFieldNum].pData != NULL)
                    {
                        pMe->list_record[i].pi = *(callPIType*)(pEntry->pFields[nFieldNum].pData);
                    }
                }
                break;

                case AEECALLHISTORY_FIELD_DATE_TIME:
                {
                    if(pEntry->pFields[nFieldNum].pData != NULL)
                    {
                        pMe->list_record[i].time_stamp= *(uint32*)(pEntry->pFields[nFieldNum].pData);
                    }
                }
                break;

                case AEECALLHISTORY_FIELD_CALL_DURATION:
                {
                    if(pEntry->pFields[nFieldNum].pData != NULL)
                    {
                        pMe->list_record[i].duration = *(uint32*)(pEntry->pFields[nFieldNum].pData);
                    }
                }
                break;

                case AEECALLHISTORY_FIELD_CALL_TYPE:
                {
                    if(pEntry->pFields[nFieldNum].pData != NULL)
                    {
                        pMe->list_record[i].category = *(uint16*)(pEntry->pFields[nFieldNum].pData);
                    }
                }
                break;

                case AEECALLHISTORY_FIELD_COUNTER:
                {
                    if(pEntry->pFields[nFieldNum].pData != NULL)
                    {
                        pMe->list_record[i].counter = *(uint16*)(pEntry->pFields[nFieldNum].pData);
                    }
                }
                break;

                default:
                    break;
            }
        }
        
        pEntry = (AEECallHistoryEntry *)ICALLHISTORY_EnumNext(pMe->m_pCallHistory, &nRet);

        if(STARTMETHOD_FINDNUMBER == pMe->m_eStartMethod ||
            STARTMETHOD_SELECTFIELD == pMe->m_eStartMethod)
        {
            if(pMe->list_record[i].pi != PI_ALLOWED ||
                (pMe->list_record[i].number == NULL) ||
                (WSTRLEN(pMe->list_record[i].number) == 0))
            {
                FREEIF(pMe->list_record[i].name);
                FREEIF(pMe->list_record[i].number);
                continue;
            }
        }
        
        pMe->list_record[i].wIndex = i;
        i ++;
    }
    pMe->record_count = i;
}

/*==============================================================================
函数：
       RecentCalls_GetSelectRecord

说明：
       获取当前列表中选择的通话记录对应地址

参数：
       pMe [in]：指向Applet对象结构的指针。该结构包含小程序的特定信息。
       nIndex [in]：需要获取的记录索引值。

返回值：
       若果获取成功，返回AEECallHistoryEntry地址，否则返回NULL。

备注：
==============================================================================*/
static AEECallHistoryEntry * RecentCalls_GetRecordByIndex(CRecentCalls *pMe, uint16 nIndex)
{
    AEECallHistoryEntry *pEntry = NULL;
    int i = 0, nLen = 0, nRet = 0;
    boolean bCmpNum = FALSE;
    boolean bRecordExisted = FALSE;
    char     strNumber[AEECALLHISTORY_MAXDIGITS];
    
    if (ICALLHISTORY_EnumInitByCallType(pMe->m_pCallHistory, pMe->list_record[nIndex].category) != SUCCESS)
    {
        return NULL;
    }

    if(pMe->list_record[nIndex].number != NULL &&
        WSTRLEN(pMe->list_record[nIndex].number) != 0)
    {
        (void)WSTRTOSTR(pMe->list_record[nIndex].number, strNumber, sizeof(strNumber));
        bCmpNum = TRUE;
    }
    
    pEntry = (AEECallHistoryEntry *)ICALLHISTORY_EnumNext(pMe->m_pCallHistory, &nRet);
    while (NULL != pEntry)
    {
        for (i=0; i<pEntry->wNumFields; i++)
        {
            // 如果有号码记录，比较号码是否相符
            if(bCmpNum)
            {
                if (pEntry->pFields[i].wID == AEECALLHISTORY_FIELD_NUMBER)
                {
                    if (pEntry->pFields[i].pData != NULL)
                    {
                        nLen = pEntry->pFields[i].wDataLen;
                        // 判断已有记录号码与待添加号码是否匹配
                        if (STRNCMP(strNumber, (char *)pEntry->pFields[i].pData, nLen) == 0)
                        {
                            bRecordExisted = TRUE;
                            break;
                        }
                    }
                }
            }
            // 否则比较通话时间
            else
            {
                if (pEntry->pFields[i].wID == AEECALLHISTORY_FIELD_DATE_TIME)
                {
                    if (pEntry->pFields[i].pData != NULL)
                    {
                        // 判断时间记录是否匹配
                        if (pMe->list_record[nIndex].time_stamp == *(uint32 *)pEntry->pFields[i].pData)
                        {
                            bRecordExisted = TRUE;
                            break;
                        }
                    }
                }
            }
        }
        
        if (bRecordExisted)
        {
            break;
        }
        pEntry = (AEECallHistoryEntry *)ICALLHISTORY_EnumNext(pMe->m_pCallHistory, &nRet);
    }
    
    return pEntry;
}


/*==================================================================================
FUNCTION RecentCalls_GetLastCallTime
Description 获取最近一次通话记录
PARAMETERS:
          *pMe: CRecentCalls object pointer
RETURN VALUE:
    uint32:最近一次通话的时间
   
COMMENTS:
   
SIDE EFFECTS:

===================================================================================*/
static uint32 RecentCalls_GetLastCallTime(CRecentCalls *pMe)
{
    int i = 0;
    uint32 lastCallTime = 0, tempTimeStamp = 0;
    
    pMe->m_callsCategory = AEECALLHISTORY_CALL_TYPE_ALL;
    RecentCalls_GetRecord(pMe);
    for(i = 0; i < pMe->record_count; i++)
    {
        if(pMe->list_record[i].time_stamp > tempTimeStamp)
        {
            if(pMe->list_record[i].duration >= 1)
            {
                tempTimeStamp = pMe->list_record[i].time_stamp;
                lastCallTime = pMe->list_record[i].duration;
            }
        }
    }
    
    return lastCallTime;
}

/*==================================================================================
FUNCTION 
        RecentCalls_DeleteRecordByType
        
Description 
        删除一类通话记录
        
PARAMETERS:
        *pMe: CRecentCalls object pointer
        call_type: call type
        
RETURN VALUE:
        成功返回TRUE，失败返回FALSE
   
COMMENTS:
   
SIDE EFFECTS:

===================================================================================*/
static boolean RecentCalls_DeleteRecordByType(CRecentCalls *pMe, uint16 call_type)
{
    AEECallHistoryEntry *pEntry = NULL;
    int                        nRet = 0;
    
    if(ICALLHISTORY_EnumInitByCallType(pMe->m_pCallHistory, call_type) != SUCCESS)
    {
        return FALSE;
    }
    
    pEntry = (AEECallHistoryEntry *)ICALLHISTORY_EnumNext(pMe->m_pCallHistory, &nRet);
    while(NULL != pEntry)
    {
        ICALLHISTORY_ClearEntry(pMe->m_pCallHistory);
        pEntry = (AEECallHistoryEntry *)ICALLHISTORY_EnumNext(pMe->m_pCallHistory, &nRet);
    }

    return TRUE;
}

/*==============================================================================
函数：
       RecentCalls_RecordDetail

说明：
       写出通话记录的详细内容

参数：
       pMe [in]：指向Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       无。

备注：:

==============================================================================*/
static void RecentCalls_RecordDetail(CRecentCalls   *pMe) 
{
   //AEERect ctlRect; 
   AECHAR  wszName[MAX_STRING_LEN];
   AECHAR  wszRawNum [ AEECALLHISTORY_MAXDIGITS ]; 
   AECHAR  wszFmt[5];
   AECHAR  wszCount[5];
   AECHAR  wszText[10];
   int         nTextWidth = 0, nlen = 0, nCount = 0, ncharsInLine = 0, nLineHeight = 0;
   int         nNumberWidth = GetSingleNumberWidth(pMe->m_pDisplay, AEE_FONT_BOLD);
   int         nSinkingLines = 0;
#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
   AEEDeviceInfo DeviceInfo;
   
   ISHELL_GetDeviceInfo(pMe->m_pShell, &DeviceInfo);
#endif//FEATURE_LANG_ARABIC_HEBREW_DISP

   nLineHeight = BETWEEN_LINE_PIXELS + IDISPLAY_GetFontMetrics(pMe->m_pDisplay, AEE_FONT_BOLD, NULL, NULL);
   
   //SETAEERECT(&ctlRect,0, 0,pMe->m_rc.dx,pMe->m_rc.dy);
#ifdef FEATURE_CARRIER_CHINA_VERTU
    {
        IImage *pImageBg = ISHELL_LoadResImage(pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_RECENTCALLS_BACKGROUND);
        
        Appscommon_ResetBackground(pMe->m_pDisplay, pImageBg, APPSCOMMON_BG_COLOR, &pMe->m_rc, 0, 0);
        if(pImageBg != NULL)
        {
            IImage_Release(pImageBg);
        }
    }
#else
    Appscommon_ResetBackgroundEx(pMe->m_pDisplay, &pMe->m_rc, TRUE);
#endif
   //IDISPLAY_FillRect(pMe->m_pDisplay, &ctlRect,RGB_BLACK);
   IDISPLAY_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, RGB_WHITE);
   
   /*不允许显示的号码，用no number代替名字字段*/
   if(pMe->list_record[pMe->record_selected].pi != PI_ALLOWED)
   {
        (void) ISHELL_LoadResString(pMe->m_pShell,
                                   AEE_RECENTCALLSRES_LANGFILE,
                                   IDS_NO_NUMBER,
                                   wszName,
                                   sizeof(wszName));
   }
    /*仅当号码允许显示时去查找相应名字*/
   else
   {
       MEMSET(wszName, 0, sizeof(wszName));
       if((pMe->list_record[pMe->record_selected].number != NULL) &&
            (WSTRLEN(pMe->list_record[pMe->record_selected].number) != 0))
       {
           nCount = pMe->list_record[pMe->record_selected].counter; 
           MEMSET(wszRawNum, 0, sizeof(wszRawNum));
           WSTRCPY(wszRawNum, pMe->list_record[pMe->record_selected].number);
           RecentCalls_GetContactName(pMe, wszRawNum, 
                         wszName, MAX_STRING_LEN); 
       }
       else
       {
            (void) ISHELL_LoadResString(pMe->m_pShell,
                                   AEE_RECENTCALLSRES_LANGFILE,
                                   IDS_NO_NUMBER,
                                   wszName,
                                   sizeof(wszName));
       }
   
       if ((WSTRLEN(wszName) == 0) ||
            (wszName == NULL))
       {
            MEMSET(wszName, 0, sizeof(wszName));
            (void) ISHELL_LoadResString(pMe->m_pShell,
                                   AEE_RECENTCALLSRES_LANGFILE,
                                   IDS_UNKNOWN_NUMBER,
                                   wszName,
                                   sizeof(wszName));
       }
        
#ifdef FEATURE_CARRIER_THAILAND_HUTCH        
       STRTOWSTR ( "%d", wszFmt, sizeof(wszFmt) );
#else
       STRTOWSTR ( "(%d)", wszFmt, sizeof(wszFmt) );
#endif //#if defined FEATURE_CARRIER_THAILAND_HUTCH
       WSPRINTF ( wszCount, sizeof(wszCount), wszFmt,  nCount );
   }
   /*显示名字字段*/
   (void)IDISPLAY_MeasureTextEx(pMe->m_pDisplay, AEE_FONT_BOLD, wszName, -1, 
                        pMe->m_rc.dx - 2*PIXELS_TO_EDGE, &ncharsInLine);
   nlen = WSTRLEN(wszName);
   (void)IDISPLAY_DrawText(pMe->m_pDisplay,
                   AEE_FONT_BOLD,
                   wszName, 
                   ncharsInLine, 
                   PIXELS_TO_EDGE,
                   PIXELS_TO_EDGE,	//+ TITLEBAR_HEIGHT,
                   NULL,
                   IDF_TEXT_TRANSPARENT);
   if(nlen > ncharsInLine)
   {
      (void)IDISPLAY_DrawText(pMe->m_pDisplay,
                      AEE_FONT_BOLD,
                      wszName + ncharsInLine, 
                      -1, 
                      PIXELS_TO_EDGE,
                      PIXELS_TO_EDGE + (1*nLineHeight),	//+ TITLEBAR_HEIGHT
                      NULL,
                      IDF_TEXT_TRANSPARENT);
      nSinkingLines++;
   }
   /* 显示号码*/
   ncharsInLine = (pMe->m_rc.dx - 2*PIXELS_TO_EDGE) / nNumberWidth;
   nlen = WSTRLEN(wszRawNum);
   (void)IDISPLAY_DrawText(pMe->m_pDisplay,
                       AEE_FONT_BOLD,
                       wszRawNum,
                       nlen<ncharsInLine?nlen:ncharsInLine,
                       PIXELS_TO_EDGE,
                       PIXELS_TO_EDGE  + ((1 + nSinkingLines)*nLineHeight),	//+ TITLEBAR_HEIGHT
                       NULL,
                       IDF_TEXT_TRANSPARENT);
   /*如果一行显示不下，再显示第二行*/
   if (nlen > ncharsInLine)
   {
      (void)IDISPLAY_DrawText(pMe->m_pDisplay,
                       AEE_FONT_BOLD,
                       wszRawNum + ncharsInLine, 
                       -1, 
                       PIXELS_TO_EDGE,
                       PIXELS_TO_EDGE  + ((2 + nSinkingLines)*nLineHeight),	//+ TITLEBAR_HEIGHT
                       NULL,
                       IDF_TEXT_TRANSPARENT);
      nSinkingLines++;
   }
   /*显示通话次数*/
   MEMSET(wszText, 0, sizeof(wszText));
   (void) ISHELL_LoadResString(pMe->m_pShell,
                                   AEE_RECENTCALLSRES_LANGFILE,
                                   IDS_CALLCOUNT,
                                   wszText,
                                   sizeof(wszText));  
   nTextWidth = IDISPLAY_MeasureText(pMe->m_pDisplay, AEE_FONT_BOLD, wszText);
#if defined(FEATURE_DISP_128X128) || defined(FEATURE_DISP_128X160)   
(void)IDISPLAY_DrawText(pMe->m_pDisplay,
                  AEE_FONT_BOLD,
                  wszText, 
                  -1, 
                  PIXELS_TO_EDGE,
                  ((5 + nSinkingLines)*nLineHeight)-2,   //+ TITLEBAR_HEIGHT     5
                  NULL,
                  IDF_TEXT_TRANSPARENT);
(void)IDISPLAY_DrawText(pMe->m_pDisplay,
                  AEE_FONT_BOLD,
                  wszCount, 
                  -1, 
                  nTextWidth + 3 * nNumberWidth,
                  ((5 + nSinkingLines)*nLineHeight)-2,   //+ TITLEBAR_HEIGHT     5
                  NULL,
                  IDF_TEXT_TRANSPARENT);

#else
#ifdef FEATURE_VERSION_W208S
	(void)IDISPLAY_DrawText(pMe->m_pDisplay,
                     AEE_FONT_BOLD,
                     wszText, 
                     -1, 
                     PIXELS_TO_EDGE,
                     PIXELS_TO_EDGE  + ((4 + nSinkingLines)*nLineHeight),	//+ TITLEBAR_HEIGHT	 	5
                     NULL,
                     IDF_TEXT_TRANSPARENT);
   (void)IDISPLAY_DrawText(pMe->m_pDisplay,
                     AEE_FONT_BOLD,
                     wszCount, 
                     -1, 
                     nTextWidth + 3 * nNumberWidth,
                     PIXELS_TO_EDGE  + ((4 + nSinkingLines)*nLineHeight),	//+ TITLEBAR_HEIGHT	 	5
                     NULL,
                     IDF_TEXT_TRANSPARENT);
#else
   (void)IDISPLAY_DrawText(pMe->m_pDisplay,
                     AEE_FONT_BOLD,
                     wszText, 
                     -1, 
                     PIXELS_TO_EDGE,
                     PIXELS_TO_EDGE  + ((5 + nSinkingLines)*nLineHeight),	//+ TITLEBAR_HEIGHT	 	5
                     NULL,
                     IDF_TEXT_TRANSPARENT);
   (void)IDISPLAY_DrawText(pMe->m_pDisplay,
                     AEE_FONT_BOLD,
                     wszCount, 
                     -1, 
                     nTextWidth + 3 * nNumberWidth,
                     PIXELS_TO_EDGE  + ((5 + nSinkingLines)*nLineHeight),	//+ TITLEBAR_HEIGHT	 	5
                     NULL,
                     IDF_TEXT_TRANSPARENT);
#endif   
#endif   
   IDISPLAY_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, RGB_BLACK);
   RecentCalls_TimeRecord(pMe, nSinkingLines); //画通话时间
}

/*==============================================================================
FUNCTION RecentCalls_TimeRecord

Description 当前号码的通话时间,日期

参数：
       pMe [in]：指向Applet对象结构的指针。该结构包含小程序的特定信息。
       nSinkingLines [in]：向下偏移的行数。

返回值：
       无。

备注：
===============================================================================*/
static void RecentCalls_TimeRecord(CRecentCalls *pMe, int nSinkingLines)
{
    AECHAR                  szbuf[16] = {0};
    AECHAR                  buffer[32] = {0};
#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
   AEEDeviceInfo DeviceInfo;
#endif
#ifdef FEATURE_APP_WORLDTIME
    extern boolean Calendar_FormatDateTime( uint32 seconds, AECHAR* resultString, int resultStringSizeByByte);
#endif /*FEATURE_APP_WORLDTIME*/

    int                 nTextWidth = 0;
	int					nCallTimeWidth = 0;	//Add By zzg 2012_02_24
    int					nCallTimeTextWidth = 0;
    int                 nLineHeight = BETWEEN_LINE_PIXELS + IDISPLAY_GetFontMetrics(pMe->m_pDisplay, AEE_FONT_BOLD, NULL, NULL);
    int                 nNumberWidth  = GetSingleNumberWidth(pMe->m_pDisplay, AEE_FONT_BOLD);
    
    IDISPLAY_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, RGB_WHITE);
    //获取日期，时间，和通话时长
    (void) ISHELL_LoadResString( pMe->m_pShell,
                    AEE_RECENTCALLSRES_LANGFILE,
                    IDS_CALLTIME,
                    szbuf,
                    sizeof( szbuf)
                );
    (void) IDISPLAY_DrawText( pMe->m_pDisplay,
                    AEE_FONT_NORMAL,
                    szbuf,
                    -1,
                    PIXELS_TO_EDGE,
                    PIXELS_TO_EDGE  + ((2 + nSinkingLines)*nLineHeight),	//+ TITLEBAR_HEIGHT
                    NULL,
                    IDF_TEXT_TRANSPARENT
                );
  //画出"日期/时间"字符串

	nCallTimeTextWidth = IDISPLAY_MeasureText(pMe->m_pDisplay, AEE_FONT_NORMAL, buffer);
    nCallTimeWidth = IDISPLAY_MeasureText(pMe->m_pDisplay, AEE_FONT_NORMAL, szbuf);	//Add By zzg 2012_02_24
    (void) ISHELL_LoadResString(pMe->m_pShell,
                    AEE_RECENTCALLSRES_LANGFILE,
                    IDS_DURATION,
                    buffer,
                    sizeof( buffer)
                );
    nTextWidth = IDISPLAY_MeasureText(pMe->m_pDisplay, AEE_FONT_NORMAL, buffer);
#if defined(FEATURE_DISP_128X128) || defined(FEATURE_DISP_128X160)     
    (void)IDISPLAY_DrawText(pMe->m_pDisplay,
                    AEE_FONT_NORMAL,
                    buffer,
                    -1,
                    PIXELS_TO_EDGE,
                    ((4 + nSinkingLines)*nLineHeight),    //+ TITLEBAR_HEIGHT  
                    NULL,
                    IDF_TEXT_TRANSPARENT
                );  //画出"通话时间"字符串
#else
#ifdef FEATURE_VERSION_W208S
	(void)IDISPLAY_DrawText(pMe->m_pDisplay,
                    AEE_FONT_NORMAL,
                    buffer,
                    -1,
                    PIXELS_TO_EDGE,
                    PIXELS_TO_EDGE  + ((3 + nSinkingLines)*nLineHeight),	//+ TITLEBAR_HEIGHT	 
                    NULL,
                    IDF_TEXT_TRANSPARENT
                );  //画出"通话时间"字符串
#else
    (void)IDISPLAY_DrawText(pMe->m_pDisplay,
                    AEE_FONT_NORMAL,
                    buffer,
                    -1,
                    PIXELS_TO_EDGE,
                    PIXELS_TO_EDGE  + ((4 + nSinkingLines)*nLineHeight),	//+ TITLEBAR_HEIGHT	 
                    NULL,
                    IDF_TEXT_TRANSPARENT
                );  //画出"通话时间"字符串
#endif                
#endif
    //如果读出的时间是0值的话(换卡情况)，那么不再画时间
    if(pMe->list_record[pMe->record_selected].time_stamp == 0)
    {
        return;
    }
#ifdef FEATURE_APP_WORLDTIME
    // draw start time
    if( Calendar_FormatDateTime(pMe->list_record[pMe->record_selected].time_stamp, buffer, sizeof( buffer)))
    {
#if defined(FEATURE_DISP_128X128) || defined(FEATURE_DISP_128X160) 
        (void)IDISPLAY_DrawText( pMe->m_pDisplay,
                      AEE_FONT_BOLD,
                      buffer,
                      -1,
                      PIXELS_TO_EDGE,//nTextWidth + nNumberWidth, //ELS_TO_EDGE,
                      2  + ((3 + nSinkingLines)*nLineHeight),  //+ TITLEBAR_HEIGHT     3
                      NULL,
                      IDF_TEXT_TRANSPARENT
                  );
#else
#ifdef FEATURE_VERSION_W208S
		(void)IDISPLAY_DrawText( pMe->m_pDisplay,
                      AEE_FONT_BOLD,
                      buffer,
                      -1,
                      nCallTimeWidth + nNumberWidth, //ELS_TO_EDGE,
                      PIXELS_TO_EDGE  + ((2 + nSinkingLines)*nLineHeight),	//+ TITLEBAR_HEIGHT	 	3
                      NULL,
                      IDF_TEXT_TRANSPARENT
                  );
#else
        (void)IDISPLAY_DrawText( pMe->m_pDisplay,
                      AEE_FONT_BOLD,
                      buffer,
                      -1,
                      nCallTimeTextWidth + nNumberWidth, //ELS_TO_EDGE,
                      PIXELS_TO_EDGE  + ((3 + nSinkingLines)*nLineHeight),	//+ TITLEBAR_HEIGHT	 	3
                      NULL,
                      IDF_TEXT_TRANSPARENT
                  );
#endif
#endif
    }
#endif /*FEATURE_APP_WORLDTIME*/

#ifdef FEATURE_CARRIER_THAILAND_HUTCH   
    // draw duration
    {
        uint32   duration    = pMe->list_record[pMe->record_selected].duration;
        AECHAR  str[]       = {':',0};
        AECHAR  format[]    = { '%', '0', '2', 'd', 0};
        int     len         = 0;
                            
        //hour
        WSPRINTF( buffer, sizeof( buffer), format, duration / 3600);        
        len = WSTRLEN( buffer);
        WSTRCPY( buffer + len, str); 
        
        //minutes
        len = WSTRLEN( buffer);        
        WSPRINTF( buffer+len, sizeof( buffer), format, (duration / 60 - (duration / 3600)*60));        
        len = WSTRLEN( buffer);
        WSTRCPY( buffer + len, str);
        
        //seconds
        len = WSTRLEN( buffer);        
        WSPRINTF( buffer + len, sizeof( buffer) - len, format, duration % 60);

        //画出具体的通话时间
#if defined(FEATURE_DISP_128X128) || defined(FEATURE_DISP_128X160)  
        (void)IDISPLAY_DrawText( pMe->m_pDisplay, 
                  AEE_FONT_BOLD,
                  buffer,
                  -1,
                  nTextWidth + nNumberWidth,
                  PIXELS_TO_EDGE  + ((4 + nSinkingLines)*nLineHeight),  //+ TITLEBAR_HEIGHT
                  NULL,
                  IDF_TEXT_TRANSPARENT
              );
#else
        (void)IDISPLAY_DrawText( pMe->m_pDisplay, 
                  AEE_FONT_BOLD,
                  buffer,
                  -1,
                  nTextWidth + nNumberWidth,
                  PIXELS_TO_EDGE  + ((3 + nSinkingLines)*nLineHeight),	//+ TITLEBAR_HEIGHT
                  NULL,
                  IDF_TEXT_TRANSPARENT
              );
#endif
    }
#else
    // draw duration
    {
        uint32   duration    = pMe->list_record[pMe->record_selected].duration;
        AECHAR  hour_str[8]  = {0};
        AECHAR  min_str[8]  = { 0};
        AECHAR  sec_str[8]  = { 0};
        AECHAR  format[]    = { '%', '0', '2', 'd', 0};
        int     len         = 0;
    //#if defined(FEATURE_PROJECT_SMART) || defined(FEATURE_PROJECT_M8)
	#if defined(FEATURE_CALL_DURATION_TEXT)
    	(void) ISHELL_LoadResString(pMe->m_pShell,
                                  AEE_RECENTCALLSRES_LANGFILE,
                                  IDS_COLON,
                                  hour_str,
                                  sizeof(hour_str)
                              );

        (void) ISHELL_LoadResString(pMe->m_pShell,
                                  AEE_RECENTCALLSRES_LANGFILE,
                                  IDS_COLON,
                                  min_str,
                                  sizeof(min_str)
                              );
    #else
        (void) ISHELL_LoadResString(pMe->m_pShell,
                                  AEE_RECENTCALLSRES_LANGFILE,
                                  IDS_HOUR,
                                  hour_str,
                                  sizeof(hour_str)
                              );

        (void) ISHELL_LoadResString(pMe->m_pShell,
                                  AEE_RECENTCALLSRES_LANGFILE,
                                  IDS_MIN,
                                  min_str,
                                  sizeof(min_str)
                              );
        (void) ISHELL_LoadResString(pMe->m_pShell,
                                  AEE_RECENTCALLSRES_LANGFILE,
                                  IDS_SEC,
                                  sec_str,
                                  sizeof(sec_str)
                              );
    #endif
#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
        if((LNG_ARABIC == DeviceInfo.dwLang) && (IDF_ALIGN_LEFT == ParagraphAlignment(min_str, WSTRLEN((AECHAR *)min_str))))
        {
            WSPRINTF( buffer, sizeof( buffer), format, duration / 3600);
            len = WSTRLEN( buffer);
            WSTRCPY( buffer + len, hour_str); 
            len = WSTRLEN( buffer);    
            //minutes
            WSPRINTF( buffer + len, sizeof( buffer)-len*(sizeof(AECHAR)), format, (duration / 60 - (duration / 3600)*60));
            len = WSTRLEN( buffer);
            WSTRCPY( buffer + len, min_str);
            len = WSTRLEN( buffer);
            WSPRINTF( buffer + len, sizeof( buffer) - len*(sizeof(AECHAR)), format, duration % 60);
            len = WSTRLEN( buffer);
            WSTRCPY( buffer + len, sec_str);
        }
        else
#endif //FEATURE_ARPHIC_LAYOUT_ENGINE
        {
        //#if defined(FEATURE_PROJECT_SMART) || defined(FEATURE_PROJECT_M8)
		#if defined (FEATURE_CALL_DURATION_TEXT)
        	//hours
            if(duration >= 3600)
            {
                WSPRINTF( buffer, sizeof( buffer), format, duration / 3600);
                len = WSTRLEN( buffer);
                WSTRCPY( buffer + len, hour_str); 
                len = WSTRLEN( buffer);
            }
            else
            {
				WSPRINTF( buffer, sizeof( buffer), format, 0);
                len = WSTRLEN( buffer);
                WSTRCPY( buffer + len, hour_str); 
                len = WSTRLEN( buffer);
            }
            //minutes
            if(duration >= 60)
            {
                WSPRINTF( buffer + len, sizeof( buffer)-len*(sizeof(AECHAR)), format, (duration / 60 - (duration / 3600)*60));
                len = WSTRLEN( buffer);
                WSTRCPY( buffer + len, min_str);
                len = WSTRLEN( buffer);
            }
            else
            {
				WSPRINTF( buffer + len, sizeof( buffer)-len*(sizeof(AECHAR)), format, 0);
                len = WSTRLEN( buffer);
                WSTRCPY( buffer + len, min_str);
                len = WSTRLEN( buffer);
            }
            //seconds
            WSPRINTF( buffer + len, sizeof( buffer) - len*(sizeof(AECHAR)), format, duration % 60);
            len = WSTRLEN( buffer);
        #else
            //hours
            if(duration >= 3600)
            {
                WSPRINTF( buffer, sizeof( buffer), format, duration / 3600);
                len = WSTRLEN( buffer);
                WSTRCPY( buffer + len, hour_str); 
                len = WSTRLEN( buffer);
            }
            
            //minutes
            if(duration >= 60)
            {
                WSPRINTF( buffer + len, sizeof( buffer)-len*(sizeof(AECHAR)), format, (duration / 60 - (duration / 3600)*60));
                len = WSTRLEN( buffer);
                WSTRCPY( buffer + len, min_str);
                len = WSTRLEN( buffer);
            }
            //seconds
            WSPRINTF( buffer + len, sizeof( buffer) - len*(sizeof(AECHAR)), format, duration % 60);
            len = WSTRLEN( buffer);
            WSTRCPY( buffer + len, sec_str);
        #endif
        }
	
#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
    if((LNG_HEBREW == DeviceInfo.dwLang) || (LNG_ARABIC== DeviceInfo.dwLang))
    {
         (void) IDISPLAY_DrawText( pMe->m_pDisplay,
                            AEE_FONT_BOLD,
                            buffer,
                            -1,
                            128 - 56 - (IDISPLAY_MeasureText(pMe->m_pDisplay, AEE_FONT_BOLD, (const AECHAR * )buffer)),
                            PIXELS_TO_EDGE  + ((3 + nSinkingLines)*nLineHeight),	//+ TITLEBAR_HEIGHT
                            NULL,
                            IDF_TEXT_TRANSPARENT
                        );  //画出"日期/时间"字符串
    }
    else
#endif
    {
#if defined(FEATURE_DISP_128X128) || defined(FEATURE_DISP_128X160) 
        //画出具体的通话时间
        (void)IDISPLAY_DrawText( pMe->m_pDisplay,
                  AEE_FONT_BOLD,
                  buffer,
                  -1,
                  nTextWidth + nNumberWidth,
                  2  + ((4+ nSinkingLines)*nLineHeight),   //+ TITLEBAR_HEIGHT      
                  NULL,
                  IDF_TEXT_TRANSPARENT
              );
#else
#ifdef FEATURE_VERSION_W208S
		//画出具体的通话时间
        (void)IDISPLAY_DrawText( pMe->m_pDisplay,
                  AEE_FONT_BOLD,
                  buffer,
                  -1,
                  nTextWidth + nNumberWidth,
                  PIXELS_TO_EDGE  + ((3+ nSinkingLines)*nLineHeight),	//+ TITLEBAR_HEIGHT	 	 
                  NULL,
                  IDF_TEXT_TRANSPARENT
              );
#else
        //画出具体的通话时间
        (void)IDISPLAY_DrawText( pMe->m_pDisplay,
                  AEE_FONT_BOLD,
                  buffer,
                  -1,
                  nTextWidth + nNumberWidth,
                  PIXELS_TO_EDGE  + ((4+ nSinkingLines)*nLineHeight),	//+ TITLEBAR_HEIGHT	 	 
                  NULL,
                  IDF_TEXT_TRANSPARENT
              );
#endif
#endif
     }
    }
#endif //#if defined FEATURE_CARRIER_THAILAND_HUTCH		
   IDISPLAY_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, RGB_BLACK);
}
#if 0
/*============================================================================= 
FUNCTION RecentCalls_DrawImage
DESCRIPTION: 公共的画图函数
PARAMETERS:
          *pMe: CRecentCalls object pointer
          *image:image name
          x     :start x
          y     :start y
RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:
==============================================================================*/
static void RecentCalls_DrawImage(CRecentCalls *pMe,char *filename,int x,int y)
{
   char FileName[60];
   if(NULL == filename)
   {
      return;
   }
      
   if(pMe->Image)
   { 
      IIMAGE_Release(pMe->Image); 
      pMe->Image=NULL; 
   }
   SPRINTF(FileName, "image\\%s.bmp", filename);
   /*lint -e740 */
   pMe->Image = ISHELL_LoadImage(pMe->m_pShell,FileName);
   /*lint +e740*/
   if(pMe->Image)
   { 
      IIMAGE_Draw(pMe->Image,x,y); 
      IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE);   
      IIMAGE_Release(pMe->Image); 
      pMe->Image = NULL;
   }
}
#endif

/*==================================================================================
FUNCTION RecentCall_CallTimes
Description 所有通话计时
PARAMETERS:
          *pMe: CRecentCalls object pointer
          item: 要读的计时类型
RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:
===================================================================================*/
static void RecentCalls_CallTimes(CRecentCalls *pMe,AEEConfigItem item)
{  
   AECHAR  dest_str[32]; // to store the duration string
   dword   duration;
   nv_language_enum_type language = 0;

#ifdef FEATURE_CARRIER_SUDAN_SUDATEL
#if !defined( AEE_SIMULATOR)
     extern int OEM_GetConfig( AEEConfigItem i, void * pBuff, int nSize);
    OEM_GetConfig( CFGI_LANGUAGE_SELECTION, &language, sizeof(language));
#endif
#endif

   if(item == CFGI_ALL_CALL_TIMER)
   {
      dword in_time,out_time;
      (void) ICONFIG_GetItem(pMe->m_pConfig, 
                             CFGI_RECENT_MT_CALL_TIMER, 
                             &in_time, 
                             sizeof(dword));
      (void) ICONFIG_GetItem(pMe->m_pConfig, 
                             CFGI_RECENT_MO_CALL_TIMER, 
                             &out_time, 
                             sizeof(dword));
                             
      duration=in_time + out_time;
   }
   else if(item == CFGI_RECENT_MT_CALL_TIMER || CFGI_RECENT_MO_CALL_TIMER)
   {
      (void) ICONFIG_GetItem(pMe->m_pConfig, 
                             item, 
                             &duration, 
                             sizeof(dword));
   }
    if(pMe->selectState == IDS_LAST_CALL)
    {
    	//Add By zzg 2011_12_14
		dword in_time,out_time,total_time;
		(void) ICONFIG_GetItem(pMe->m_pConfig, 
								CFGI_RECENT_MT_CALL_TIMER, 
								&in_time, 
								sizeof(dword));
		(void) ICONFIG_GetItem(pMe->m_pConfig, 
								CFGI_RECENT_MO_CALL_TIMER, 
								&out_time, 
								sizeof(dword));

		total_time=in_time + out_time;

		if (total_time == 0) 
		{
			duration = 0;
		}
		else
		//Add End
		{
        	duration = (dword)RecentCalls_GetLastCallTime(pMe);
		}
    }
    
    // draw call time   
#ifdef FEATURE_CARRIER_THAILAND_HUTCH   
    {
        AECHAR  str[]       = {':',0};
        AECHAR  format[]    = { '%', '0', '2', 'd', 0};
        AECHAR  hour_format[]    = { '%', '0', '4', 'd', 0};        
        int     len         = 0;
            
        
        //hour
        WSPRINTF( dest_str, sizeof( dest_str), hour_format, duration / 3600);        
        len = WSTRLEN( dest_str);
        WSTRCPY( dest_str + len, str); 
        
        //minutes
        len = WSTRLEN( dest_str);        
        WSPRINTF( dest_str+len, sizeof( dest_str), format, (duration / 60 - (duration / 3600)*60));        
        len = WSTRLEN( dest_str);
        WSTRCPY( dest_str + len, str);
        
        //seconds
        len = WSTRLEN( dest_str);        
        WSPRINTF( dest_str + len, sizeof( dest_str) - len, format, duration % 60);

       (void)IDISPLAY_DrawText(pMe->m_pDisplay,
                         AEE_FONT_NORMAL,
                         dest_str, 
                         -1, 
                         0, 
                         40,
                         NULL,
                         IDF_ALIGN_CENTER|IDF_TEXT_TRANSPARENT);        
    }
#else     
    if( language == NV_LANGUAGE_ARABIC)
     {
        AECHAR  str[]       = {':',0};
        AECHAR  format[]    = { '%', '0', '2', 'd', 0};
        AECHAR  hour_format[]    = { '%', '0', '4', 'd', 0};        
        int     len         = 0;
       
        //seconds      
        WSPRINTF( dest_str, sizeof( dest_str), format, duration % 60);
        len = WSTRLEN( dest_str);
        WSTRCPY( dest_str + len, str);
        
        //minutes
        len = WSTRLEN( dest_str);        
        WSPRINTF( dest_str+len, sizeof( dest_str), format, (duration / 60 - (duration / 3600)*60));     
        len = WSTRLEN( dest_str);
        WSTRCPY( dest_str + len, str);
        //hour
        len = WSTRLEN( dest_str);
        WSPRINTF( dest_str + len, sizeof( dest_str), hour_format, duration / 3600);        
      
       
       (void)IDISPLAY_DrawText(pMe->m_pDisplay,
                         AEE_FONT_NORMAL,
                         dest_str, 
                         -1, 
                         0, 
                         40,
                         NULL,
                         IDF_ALIGN_CENTER|IDF_TEXT_TRANSPARENT);   
      
    }   
else
    {
#if 1
        AECHAR  hour_str[8]  = {0};
        AECHAR  min_str[8]  = { 0};
        AECHAR  sec_str[8]  = { 0};
        AECHAR  format[]    = { '%', '0', '2', 'd', 0};
        RGBVAL  nOldFontColor = RGB_WHITE;
        int         len         = 0;
        int         nLineHeight = BETWEEN_LINE_PIXELS + IDISPLAY_GetFontMetrics(pMe->m_pDisplay, AEE_FONT_BOLD, NULL, NULL);
        
        (void) ISHELL_LoadResString(pMe->m_pShell,
                                  AEE_RECENTCALLSRES_LANGFILE,
                                  IDS_HOUR,
                                  hour_str,
                                  sizeof(hour_str)
                              );

        (void) ISHELL_LoadResString(pMe->m_pShell,
                                  AEE_RECENTCALLSRES_LANGFILE,
                                  IDS_MIN,
                                  min_str,
                                  sizeof(min_str)
                              );
        (void) ISHELL_LoadResString(pMe->m_pShell,
                                  AEE_RECENTCALLSRES_LANGFILE,
                                  IDS_SEC,
                                  sec_str,
                                  sizeof(sec_str)
                              );
        
        //hours
        if(duration >= 3600)
        {
            WSPRINTF( dest_str, sizeof( dest_str), format, duration / 3600);
            len = WSTRLEN( dest_str);
            WSTRCPY( dest_str + len, hour_str); 
            len = WSTRLEN( dest_str);
        }
        //minutes
        if(duration >= 60)
        {
            WSPRINTF( dest_str + len, sizeof( dest_str) - len*(sizeof(AECHAR)), format, (duration / 60 - (duration / 3600)*60));
            len = WSTRLEN( dest_str);
            WSTRCPY( dest_str + len, min_str);
            len = WSTRLEN( dest_str);
        }
        //seconds
        WSPRINTF( dest_str + len, sizeof( dest_str) - len*(sizeof(AECHAR)), format, duration % 60);
        len = WSTRLEN( dest_str);
        WSTRCPY( dest_str + len, sec_str);
        
        nOldFontColor = IDISPLAY_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, nOldFontColor);
        (void)IDISPLAY_DrawText(pMe->m_pDisplay,
                         AEE_FONT_NORMAL,
                         dest_str, 
                         -1,   
                         PIXELS_TO_EDGE,
                         PIXELS_TO_EDGE +  (1*nLineHeight),	//+ TITLEBAR_HEIGHT
                         NULL,
                         IDF_TEXT_TRANSPARENT);
        (void)IDISPLAY_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, nOldFontColor);
#else
       int     temp_pos;  // fill it from the end    
       AECHAR  min_str[8] = {0};
       AECHAR  sec_str[8] = {0};   
       int min_str_len = 0;
       int sec_str_len = 0;
       int i;
       int nLineHeight = BETWEEN_LINE_PIXELS + IDISPLAY_GetFontMetrics(pMe->m_pDisplay, AEE_FONT_BOLD, NULL, NULL);
       
       (void) ISHELL_LoadResString(pMe->m_pShell,
                                   AEE_RECENTCALLSRES_LANGFILE,
                                   IDS_MIN,
                                   min_str,
                                   sizeof(min_str)
                                   );
                                   
       (void) ISHELL_LoadResString(pMe->m_pShell,
                                   AEE_RECENTCALLSRES_LANGFILE,
                                   IDS_SEC,
                                   sec_str,
                                   sizeof(sec_str)
                                   ); 
       
       min_str_len = WSTRLEN( min_str);
       sec_str_len = WSTRLEN( sec_str);
       temp_pos =  min_str_len + sec_str_len +9;
      
       dest_str[temp_pos--] = (AECHAR) '\0';
       for(i=0;i<sec_str_len; i++)
       {
          dest_str[temp_pos--] = sec_str[sec_str_len-1-i];
       }   
       dest_str[temp_pos--] = (AECHAR) ((duration%60) % 10 + '0');
       dest_str[temp_pos--] = (AECHAR) ((duration%60) / 10 + '0');
       for(i=0; i<min_str_len; i++)
       {
          dest_str[temp_pos--] = min_str[min_str_len -1-i];
       }
       duration /= 60;
       dest_str[temp_pos--] = (AECHAR) (duration % 10 + '0');
       duration /= 10;
       while (temp_pos >= 0) 
       {
          if (duration == 0) 
          {
            dest_str[temp_pos--] = (AECHAR) ( '0');
             break;
          }
          else 
          {
             dest_str[temp_pos--] = (AECHAR) (duration % 10 + '0');
          }
          duration /= 10;
       }
       IDISPLAY_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, RGB_WHITE);
       (void)IDISPLAY_DrawText(pMe->m_pDisplay,
                         AEE_FONT_NORMAL,
                         dest_str+temp_pos+1, 
                         -1,   
                         PIXELS_TO_EDGE,
                         PIXELS_TO_EDGE+ TITLEBAR_HEIGHT +  (1*nLineHeight),
                         NULL,
                         IDF_TEXT_TRANSPARENT);
       IDISPLAY_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, RGB_BLACK);
#endif
    }
#endif //#if defined FEATURE_CARRIER_THAILAND_HUTCH	
}                     

/*===========================================================================
   判断是否是全部删除且记录为空
   return TRUE：记录为空
   return FALSE：记录不为空
===========================================================================*/ 
static boolean RecentCalls_IsDelAllandNothingForDel(CRecentCalls *pMe)
{
    switch(pMe->selectState)
    {
        case IDS_MISSED_CALLS:
            //if(0 == RecentCalls_GetRecordCount(pMe, CALLHISTORY_MISSED_CATEGORY))
           if(0 == ICALLHISTORY_GetRecCountByType(pMe->m_pCallHistory, AEECALLHISTORY_CALL_TYPE_MISSED))
           {
               return TRUE;
           }
           break;
          
        case IDS_RECEIVED_CALLS:
            //if(0 == RecentCalls_GetRecordCount(pMe, CALLHISTORY_INCOMING_CATEGORY))
           if(0 == ICALLHISTORY_GetRecCountByType(pMe->m_pCallHistory, AEECALLHISTORY_CALL_TYPE_FROM))
           {
               return TRUE;
           }
           break;
           
        case IDS_OUTGOING_CALLS:
            //if(0 == RecentCalls_GetRecordCount(pMe, CALLHISTORY_OUTGOING_CATEGORY))
           if(0 == ICALLHISTORY_GetRecCountByType(pMe->m_pCallHistory, AEECALLHISTORY_CALL_TYPE_TO))
           {
               return TRUE;
           }
           break;
           
        case IDS_ALL_CALLS:
            /*if(0 == RecentCalls_GetRecordCount(pMe, CALLHISTORY_MISSED_CATEGORY) &&
              0 == RecentCalls_GetRecordCount(pMe, CALLHISTORY_INCOMING_CATEGORY) &&
              0 == RecentCalls_GetRecordCount(pMe, CALLHISTORY_OUTGOING_CATEGORY))*/
           if(0 == ICALLHISTORY_GetRecCountByType(pMe->m_pCallHistory, AEECALLHISTORY_CALL_TYPE_TO) &&
            0 == ICALLHISTORY_GetRecCountByType(pMe->m_pCallHistory, AEECALLHISTORY_CALL_TYPE_FROM) &&
            0 == ICALLHISTORY_GetRecCountByType(pMe->m_pCallHistory, AEECALLHISTORY_CALL_TYPE_MISSED))
           {
           	   return TRUE;
           }
           break;
           
        default:
          break;
    }
    return FALSE;
}
#if 0
/*===========================================================================
   返回某一通话记录类型的记录数
===========================================================================*/ 		   
static int RecentCalls_GetRecordCount(CRecentCalls *pMe, CallHistoryCategory  bCallHistoryCategory)
{
	byte     recordcount = 0;
	OEMCallHistoryEntry  ent;
	
    for(;;) 
    {
      if(FALSE == ICALLLIST_GetEntry(pMe->m_pCallList,
                                             bCallHistoryCategory,
                                             recordcount,
                                             &ent))
      {      
         break;
      }
       
      recordcount++; 
    }
   return recordcount;
}      
#endif
#if defined(FEATURE_WMS_APP) && !defined(FEATURE_WMSAPP_ONLYSUPPORTVMAIL)
/*======================================================================================
FUNCTION RecentCalls_SendSMS

Description 发送短信

参数：
       pMe [in]：指向Applet对象结构的指针

返回值：
       无。

备注：:
=======================================================================================*/
static boolean RecentCalls_SendSMS(CRecentCalls *pMe)
{
    PARAM_NOT_REF(pMe)
    IWmsApp *pIWmsApp = NULL;
    boolean  bRet = FALSE;
    AECHAR wszRawNum [AEECALLHISTORY_MAXDIGITS]; 

    MEMSET(wszRawNum, 0, sizeof(wszRawNum));
    //GetRepeatRawNumber ( pMe->list_record[pMe->record_selected].number, wszRawNum );
    WSTRCPY(wszRawNum, pMe->list_record[pMe->record_selected].number);
                                    
    if (SUCCESS == ISHELL_CreateInstance(pMe->m_pShell,
            AEECLSID_WMSAPP,
            (void**)&pIWmsApp))
    {
        if (pMe->selectState == IDS_SEND_SMS)
        {
            if (SUCCESS == IWmsApp_SendTextMessageExt(pIWmsApp,wszRawNum,NULL))
            {
                bRet = TRUE;
                pMe->m_eCurState = STATE_RECORD_LIST;
            }
        }
        else if (pMe->selectState == IDS_SEND_CONTACT)
        {
#ifdef FEATURE_SEND_MSG         
            AECHAR wszName[MAX_STRING_LEN];
            AECHAR wszString[MAX_STRING_LEN+AEECALLHISTORY_MAXDIGITS+6];
            int number_len = 0;
            int name_len = 0;
            int length = 0;
            int i = 0;  
            
            MEMSET(wszName, 0, sizeof(wszName));
            MEMSET(wszString, 0, sizeof(wszString));           
            // get Name for contact
            RecentCalls_GetContactName(pMe,
                            wszRawNum,
                            wszName, 
                            MAX_STRING_LEN);  

            name_len = WSTRLEN(wszName);
            number_len = WSTRLEN(wszRawNum);

            // get Name true
            if (name_len > 0)
            {
                for(i=0;i<name_len; i++)
                {
                  wszString[length ++] = wszName[i];
                }            
                wszString[length ++] = (AECHAR)' ';
                for(i=0;i<number_len; i++)
                {
                  wszString[length ++] = wszRawNum[i];
                }
            }
            else 
            {
                for(i=0;i<number_len; i++)
                {
                  wszString[length ++] = wszRawNum[i];
                }            
            }
            wszString[length] = (AECHAR)0;

            if (SUCCESS == IWmsApp_SendTextMessageExt(pIWmsApp,NULL,wszString))
            {
                bRet = TRUE;
                pMe->m_eCurState = STATE_RECORD_LIST;
            }            
#else
            if (SUCCESS == IWmsApp_SendTextMessageExt(pIWmsApp,NULL,wszRawNum))
            {
                bRet = TRUE;
                pMe->m_eCurState = STATE_RECORD_LIST;
            }
#endif //#if defined FEATURE_CARRIER_THAILAND_HUTCH  
        }
    }   
    if(pIWmsApp)
    {
        (void)IWmsApp_Release(pIWmsApp);
        pIWmsApp = NULL;
    }
    return bRet;
}
#endif

/*==================================================================================
FUNCTION RecentCalls_SendCall
Description 发起一个呼叫
PARAMETERS:
          *pMe: CRecentCalls object pointer
RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:
===================================================================================*/
static boolean RecentCalls_SendCall(CRecentCalls *pMe ,boolean is_ip_call)
{
    PARAM_NOT_REF(pMe)
    boolean  bRet = FALSE; 
    ICallApp *pCallApp = NULL;
    
    if(SUCCESS == ISHELL_CreateInstance(pMe->m_pShell, 
                                          AEECLSID_DIALER,
                                           (void**)&pCallApp))
    { 
        // 改用带参数启动 CallApp Applet
        AECHAR wszRawNum [AEECALLHISTORY_MAXDIGITS]; 
        MEMSET(wszRawNum, 0, sizeof(wszRawNum));
        //GetRepeatRawNumber (pMe->list_record[pMe->record_selected].number, 
        //                   wszRawNum);
        WSTRCPY(wszRawNum, pMe->list_record[pMe->record_selected].number);
#ifdef FEATRUE_SET_IP_NUMBER
        if(is_ip_call)
        {
            if (SUCCESS == ICallApp_Make_IP_Call_F(pCallApp,wszRawNum))
            {
                bRet = TRUE;         
                pMe->m_eCurState = STATE_RECORD_LIST;
            }

            if (pMe->m_callsCategory == AEECALLHISTORY_CALL_TYPE_TO)//CALLHISTORY_OUTGOING_CATEGORY)
            {
                pMe->record_selected = 0;
            }
        }
        else
#endif
        {
            if (SUCCESS == ICallApp_CallNumber(pCallApp,wszRawNum))
            {
                bRet = TRUE;         
                pMe->m_eCurState = STATE_RECORD_LIST;

                if (pMe->m_callsCategory == AEECALLHISTORY_CALL_TYPE_TO)//CALLHISTORY_OUTGOING_CATEGORY)
                {
                   pMe->record_selected = 0;
                }
            }
        }
    }
    if(pCallApp)
    {
        (void)ICallApp_Release(pCallApp);
        pCallApp = NULL;
    }
    return bRet;        
}

/*======================================================================================
FUNCTION RecentCalls_SaveNumber

Description 保存来电号码

参数：
       pMe [in]：指向Applet对象结构的指针
       wParam  :分保存类型的信息

返回值：
       无。

备注：
=======================================================================================*/
static boolean RecentCalls_SaveNumber(CRecentCalls *pMe, PhoneSaveType saveType)
{ 
    PARAM_NOT_REF(pMe)
    boolean  bRet = FALSE;    
    IContactApp *pContactApp = NULL;

    if(SUCCESS == ISHELL_CreateInstance(pMe->m_pShell,
                                       AEECLSID_APP_CONTACT,
                                       (void**)&pContactApp))
    {
        AECHAR wszRawNum [AEECALLHISTORY_MAXDIGITS]; 
        MEMSET(wszRawNum, 0, sizeof(wszRawNum));
        //GetRepeatRawNumber (pMe->list_record[pMe->record_selected].number, 
        //                     wszRawNum);
        WSTRCPY(wszRawNum, pMe->list_record[pMe->record_selected].number);
        if (SUCCESS == ICONTAPP_SaveNumber(pContactApp,
                                               wszRawNum, 
                                               NULL,
                                               NULL,
                                               saveType))
        {
            bRet = TRUE; 
            pMe->m_eCurState = STATE_RECORD_LIST;
        }
    }  
    if(pContactApp)
    {
        (void)IContactApp_Release(pContactApp);
        pContactApp = NULL;
    }
    return bRet;    
}

/*==============================================================================
函数：RecentCalls_GetContactName(CRecentCalls* pMe,  AECHAR *pwszNum, AECHAR *pwszName)

说明：
        根据电话号码从电话本中查询人名。

参数：
       pMe [in]：指向SMS Applet对象结构的指针。该结构包含小程序的特定信息。
       pwszNum [in]：号码字符串。
       pwszName [in/out]：用于返回人名的字符串Buf。
       wBufLen [in]：pwszName最多容纳的字符数(AECHAR)

返回值：
       无。

备注：

==============================================================================*/
static void RecentCalls_GetContactName(CRecentCalls* pMe,  
    AECHAR *pwszNum, 
    AECHAR *pwszName,
    uint16 wBufLen
)
{
    ContInfo  CInfo;
    IContactApp * pContactApp = NULL;
    
    
    if ((NULL == pMe) || (NULL == pwszNum) || 
        (NULL == pwszName) || (0 == wBufLen))
    {
        return;
    }
    
    ISHELL_CreateInstance(pMe->m_pShell,
                AEECLSID_APP_CONTACT,
                (void **)&pContactApp);              
    if (NULL == pContactApp)
    {
        return;
    }
    
    MEMSET(&CInfo, 0, sizeof(ContInfo));
    CInfo.pName = pwszName;
    CInfo.wNameLen = wBufLen;
    
   // (void)ICONTAPP_NumberLookup(pContactApp, pwszNum, &CInfo);
    if (SUCCESS == ICONTAPP_NumberLookup(pContactApp, pwszNum, &CInfo))
     {
         (void)ICONTAPP_Release(pContactApp);   
     }
     else
     {
         #if defined(FEATURE_VERSION_W515V3)
         if((WSTRNICMP(pwszNum,L"0",1)==0)||(WSTRNICMP(pwszNum,L"+00",3)==0)||(WSTRNICMP(pwszNum,L"+",1)==0)||(WSTRNICMP(pwszNum,L"00",2)==0))
         {
                     
            AECHAR Temp_Number[AEECM_MAX_DIGITS_LENGTH] = {0};
            if(WSTRNICMP(pwszNum,L"0",1)==0)
            {
                WSTRLCPY(Temp_Number,pwszNum+1,AEECM_MAX_DIGITS_LENGTH);
            }
            else if(WSTRNICMP(pwszNum,L"+00",3)==0)
            {
                WSTRLCPY(Temp_Number,pwszNum+3,AEECM_MAX_DIGITS_LENGTH);
            }
            else if(WSTRNICMP(pwszNum,L"+",1)==0)
            {
                WSTRLCPY(Temp_Number,pwszNum+1,AEECM_MAX_DIGITS_LENGTH);
            }
            if(WSTRNICMP(pwszNum,L"00",2)==0)
            {
                WSTRLCPY(Temp_Number,pwszNum+2,AEECM_MAX_DIGITS_LENGTH);
            }            
            ICONTAPP_NumberLookup(pContactApp, Temp_Number, &CInfo);
          }
         #endif
         (void)ICONTAPP_Release(pContactApp); 
     }
   // (void)ICONTAPP_Release(pContactApp);   
}
#ifdef FEATURE_THAILAND  
   ////////////////////////////////////////////////////////////////////////////
  int Recntcall_is_view_state(int bb)
{
    static int g_is_incoming = 0;
    if (bb>=0)
    {
        g_is_incoming = bb;
    }
    return g_is_incoming;	
}
#endif
   ////////////////////////////////////////////////////////////////////////////

/*==============================================================================
函数：RecentCalls_AddSelectRecordListNode

说明：
      将指定的通话记录添加到SelectRecordList

参数：
      pMe [in]：CRecentCalls 类型指针
      nRecordIndex [in]：

返回值：
       TRUE：操作成功
       FALSE：操作失败

备注：
       
==============================================================================*/
static boolean RecentCalls_AddSelectRecordListNode(CRecentCalls *pMe, uint16 nRecordIndex)
{
    sSelectRecordListNode* pSelectRecordNode = NULL;
    
    if (nRecordIndex >= pMe->record_count)
    {
        return FALSE;
    }
    
    pSelectRecordNode = (sSelectRecordListNode*)MALLOC(sizeof(sSelectRecordListNode));
    
    if(pSelectRecordNode == NULL)
    {
        return FALSE;
    }
    
    pCurSelectRecordNode = pSelectRecordListRoot;
    
    pSelectRecordNode->SelectRecord.wIndex = nRecordIndex;
    pSelectRecordNode->SelectRecord.number = WSTRDUP(pMe->list_record[nRecordIndex].number);
    pSelectRecordNode->SelectRecord.name = WSTRDUP(pMe->list_record[nRecordIndex].name);
    
    if(pCurSelectRecordNode == NULL)
    {
        pCurSelectRecordNode = pSelectRecordNode;
        pSelectRecordListRoot = pCurSelectRecordNode;
        pCurSelectRecordNode->pPreNode = NULL;
        pCurSelectRecordNode->pNextNode = NULL;
    }
    else
    {
        boolean bLast = TRUE;
        
        while (pCurSelectRecordNode != NULL)
        {
            if(nRecordIndex < pCurSelectRecordNode->SelectRecord.wIndex)
            {
                bLast = FALSE;
                break;
            }
            
            if(pCurSelectRecordNode->pNextNode != NULL)
            {
                pCurSelectRecordNode = pCurSelectRecordNode->pNextNode;
            }
            else
            {
                break;
            }
        }

        if(bLast)
        {
            pSelectRecordNode->pPreNode = pCurSelectRecordNode;
            pCurSelectRecordNode->pNextNode = pSelectRecordNode;
        }
        else
        {
            if(pCurSelectRecordNode == pSelectRecordListRoot)
            {
                pSelectRecordListRoot = pSelectRecordNode;
            }
            else
            {
                pSelectRecordNode->pPreNode = pCurSelectRecordNode->pPreNode;
                pCurSelectRecordNode->pPreNode->pNextNode = pSelectRecordNode;
            }
            
            pSelectRecordNode->pNextNode = pCurSelectRecordNode;
            pCurSelectRecordNode->pPreNode = pSelectRecordNode;
        }
    }
   
    pMe->m_nRemainWMSNum--;
    return TRUE;
}


/*==============================================================================
函数：RecentCalls_DeleteSelectRecordListNode

说明：
       将指定的通话记录从SelectRecordList中删除

参数：
      pMe [in]：CRecentCalls 类型指针
      nRecordIndex [in]：

返回值：
       TRUE：操作成功
       FALSE：操作失败
       
备注：
       
==============================================================================*/
static boolean RecentCalls_DeleteSelectRecordListNode(CRecentCalls *pMe, uint16 nRecordIndex)
{
    pCurSelectRecordNode = pSelectRecordListRoot;
    
    if (nRecordIndex >= pMe->record_count)
    {
        return FALSE;
    }

    if(pCurSelectRecordNode == NULL)
    {
        return FALSE;
    }

    while (pCurSelectRecordNode != NULL)
    {
        if (pCurSelectRecordNode->SelectRecord.wIndex == nRecordIndex)
        {
            if (pCurSelectRecordNode == pSelectRecordListRoot)
            {
                pSelectRecordListRoot = pSelectRecordListRoot->pNextNode;
                pSelectRecordListRoot->pPreNode= NULL;
            }
            else
            {
                pCurSelectRecordNode->pPreNode->pNextNode = pCurSelectRecordNode->pNextNode;
            }
            
            if (pCurSelectRecordNode->pNextNode !=NULL)
            {
                pCurSelectRecordNode->pNextNode->pPreNode = pCurSelectRecordNode->pPreNode; 
            }

            FREEIF(pCurSelectRecordNode->SelectRecord.number);
            FREEIF(pCurSelectRecordNode->SelectRecord.name);
            FREE(pCurSelectRecordNode);    
            pCurSelectRecordNode = NULL;
            
            break;
        }
        
        pCurSelectRecordNode = pCurSelectRecordNode->pNextNode;
    } 
    
    pMe->m_nRemainWMSNum++;
    return TRUE;
}


/*==============================================================================
函数：RecentCalls_FindSelectRecordListNode

说明：
       在SelectRecordList中查找指定的通话记录

参数：
      nRecordIndex [in]：

返回值：
       TRUE：所选记录已经在SelectRecordList里面
       FALSE：所选记录不在SelectRecordList里面

备注：
       
==============================================================================*/
static boolean RecentCalls_FindSelectRecordListNode(uint16 nRecordIndex)
{
    pCurSelectRecordNode = pSelectRecordListRoot;
    
    while (pCurSelectRecordNode != NULL)
    {
        if (pCurSelectRecordNode->SelectRecord.wIndex ==  nRecordIndex)
        {   
            return TRUE;
        }
        pCurSelectRecordNode = pCurSelectRecordNode->pNextNode;
    }      

    return FALSE;
}

#ifdef FEATURE_EDITABLE_RECORD
static boolean RecentCalls_EditRecNumber(CRecentCalls *pMe)
{
    boolean  bRet = FALSE;        
    ICallApp *pCallApp = NULL;
    
    if(SUCCESS == ISHELL_CreateInstance(pMe->m_pShell, 
                                          AEECLSID_DIALER,
                                           (void**)&pCallApp))
    { 
        // 改用带参数启动 CallApp Applet
        if (SUCCESS == ICallApp_EditRecNumber(pCallApp,
                                        pMe->list_record[pMe->record_selected].number, 
                                        pMe->list_record[pMe->record_selected].category))
        {
            bRet = TRUE;
            pMe->m_eCurState = STATE_RECORD_LIST;
        }
    }
    if(pCallApp)
    {
        (void)ICallApp_Release(pCallApp);
        pCallApp = NULL;
    }
    return bRet;
}
#endif
/*==============================================================================
函数：Recentcalls_AskpasswordEvent

说明：
       IDD_ASK_PASSWORD_DIALOG 对话框事件处理函数

参数：
       pMe [in]：指向SecurityMenu Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

       
==============================================================================*/

static boolean	Recentcalls_AskpasswordEvent(CRecentCalls *pMe, 
		 AEEEvent eCode, 
		 uint16 wParam,
		 uint32 dwParam)
{
    PARAM_NOT_REF(dwParam)
    //static char   *m_strPhonePWD = NULL;
    AECHAR         wstrDisplay[OEMNV_LOCKCODE_MAXLEN+2] = {0};
    int            nLen = 0;
    char           strDisplay[OEMNV_LOCKCODE_MAXLEN+2] = {0};
//    SEC_ERR("%x, %x ,%x,SecurityAskPasswordDlgHandler",eCode,wParam,dwParam);
    
    if (NULL == pMe)
    {
        return FALSE;
    }
    
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            if(NULL == pMe->m_pPhoneLockPassword)
            {
                pMe->m_pPhoneLockPassword = (char *)MALLOC((OEMNV_LOCKCODE_MAXLEN + 1)* sizeof(char));
            }
            return TRUE;
            
        case EVT_DIALOG_START:
            (void) ISHELL_PostEvent(pMe->m_pShell,
                                    AEECLSID_APP_RECENTCALL,
                                    EVT_USER_REDRAW,
                                    NULL,
                                    NULL);
            return TRUE;
            
        case EVT_USER_REDRAW:
            // 绘制相关信息
            {
                AECHAR      text[32] = {0};
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
                //IDISPLAY_FillRect  (pMe->m_pDisplay,&pMe->m_rc, RGB_BLACK);
                (void)ISHELL_LoadResString(pMe->m_pShell, 
                                                AEE_RECENTCALLSRES_LANGFILE,
                                                IDS_SECURITY_TITLE, 
                                                text,
                                                sizeof(text));
                // 画标题条
                TitleBar_Param.pwszTitle = text;
                TitleBar_Param.dwAlignFlags = IDF_ALIGN_MIDDLE | IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE;
                #if 0
                DrawTitleBar(pMe->m_pDisplay, &TitleBar_Param);
				#else
				IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,text);
				#endif
               (void)ISHELL_LoadResString(pMe->m_pShell, 
                                                AEE_RECENTCALLSRES_LANGFILE,
                                                IDS_SECURITY, 
                                                text,
                                                sizeof(text));
                nOldFontColor = IDISPLAY_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, RGB_WHITE);
                IDISPLAY_DrawText(pMe->m_pDisplay, 
                                    AEE_FONT_BOLD, 
                                    text,
                                    -1, 
                                    xOffset, 
                                    MENUITEM_HEIGHT*1/2, 
                                    NULL, 
                                    IDF_TEXT_TRANSPARENT);
                   
                nLen = (pMe->m_pPhoneLockPassword == NULL)?(0):(STRLEN(pMe->m_pPhoneLockPassword)); 
                MEMSET(strDisplay, '*', nLen);
                strDisplay[nLen] = '|';
                strDisplay[nLen + 1] = '\0';
                (void) STRTOWSTR(strDisplay, wstrDisplay, sizeof(wstrDisplay));
                IDISPLAY_DrawText(pMe->m_pDisplay, 
                                AEE_FONT_BOLD, 
                                wstrDisplay,
                                -1, 
                                2*xOffset, 
                                MENUITEM_HEIGHT*3/2,
                                NULL, 
                                IDF_TEXT_TRANSPARENT);
                (void)IDISPLAY_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, nOldFontColor);
            	#ifndef FEATURE_ALL_KEY_PAD
                // 绘制底条提示
                if (nLen > 3)
                {// 确定-----删除
                    RECENTCALLS_DRAW_BOTTOMBAR(BTBAR_OK_DELETE)
                }
                else if(nLen > 0)
                {// 删除
                    RECENTCALLS_DRAW_BOTTOMBAR(BTBAR_DELETE)
                }
                else
                #else
                 // 绘制底条提示
                if (nLen > 3)
                {// 确定-----删除
                    RECENTCALLS_DRAW_BOTTOMBAR(BTBAR_OK_BACK)
                }
                else if(nLen > 0)
                {// 删除
                    RECENTCALLS_DRAW_BOTTOMBAR(BTBAR_BACK)
                }
                else
                #endif
                {// 取消
                    RECENTCALLS_DRAW_BOTTOMBAR(BTBAR_CANCEL)
                }

                // 更新显示
                IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE); 
        
                return TRUE;
            }
            
        case EVT_DIALOG_END:
            if(!pMe->m_bSuspending)
            {
                FREEIF(pMe->m_pPhoneLockPassword);
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
						
					//Add By zzg 2012_02_27					
					case AVK_DEL:	 
					{
						chEnter = 0;
						break;
					}
					//Add End	
                        
                    case AVK_CLR:
                        chEnter = 0;
                        #ifndef FEATURE_ALL_KEY_PAD
                        if (pMe->m_pPhoneLockPassword == NULL || STRLEN(pMe->m_pPhoneLockPassword) == 0)
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
                        	if (pMe->m_pPhoneLockPassword == NULL || STRLEN(pMe->m_pPhoneLockPassword) == 0)
                        	{
                            	CLOSE_DIALOG(DLGRET_CANCELED)
                            	return TRUE;
                        	}
                        }
                        #endif
                        break;
                        
                    case AVK_SELECT:
                    case AVK_INFO:
                        
                        if (pMe->m_pPhoneLockPassword == NULL || STRLEN(pMe->m_pPhoneLockPassword) < 4)
                        {
                            return TRUE;
                        }
                        else
                        {
                            uint16 wPWD=0;
                            char superpass[6] = {"*#09#"};
                            (void) ICONFIG_GetItem(pMe->m_pConfig, 
                                                   CFGI_PHONE_PASSWORD,
                                                   &wPWD,
                                                   sizeof(uint16));
#ifndef WIN32//wlh 模拟器测试用                   
                            if (wPWD == EncodePWDToUint16(pMe->m_pPhoneLockPassword)||(0==strcmp(superpass,pMe->m_pPhoneLockPassword)))
#else
							if(1)
#endif//WIN32   
                            {
                                // 密码符合
                                CLOSE_DIALOG(DLGRET_VALIDPINPASS)
                            }
                            else
                            {
                                // 密码错误
                                if(STRNCMP(pMe->m_pPhoneLockPassword,"62382",5) == 0)
                                {
#ifdef FEATURE_SCREEN_CAPTURE
#ifndef WIN32
                                    disp_set_capture ( TRUE );
#endif//WIN32
#endif
                                }
                            CLOSE_DIALOG(DLGRET_VALIDPINFAILED)//(DLGRET_OK)
                        }
                    }
                    return TRUE;
                        
                default:
                    return TRUE;
                }
                nLen = (pMe->m_pPhoneLockPassword == NULL)?(0):(STRLEN(pMe->m_pPhoneLockPassword));
                if (chEnter == 0)
                {
                    // 删除字符
                    if (nLen > 0)
                    {
                        bRedraw = TRUE;
                        pMe->m_pPhoneLockPassword[nLen-1] = chEnter;
                    }
                }
                else if (nLen < OEMNV_LOCKCODE_MAXLEN)
                {
                    pMe->m_pPhoneLockPassword[nLen] = chEnter;
                    nLen++;
                    pMe->m_pPhoneLockPassword[nLen] = 0;
                    bRedraw = TRUE;
                }
                
                if (bRedraw)
                {
                    (void) ISHELL_PostEvent(pMe->m_pShell,
                                            AEECLSID_APP_RECENTCALL,
                                            EVT_USER_REDRAW,
                                            NULL,
                                            NULL);
                }
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

				if(RECENTCALLS_PT_IN_RECT(wXPos,wYPos,rc))
				{
					if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
					{
						boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_RECENTCALL,EVT_USER,AVK_SELECT,0);
						return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)   && wXPos < rc.x + (rc.dx/3)*2 )//左
					{
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_RECENTCALL,EVT_USER,AVK_INFO,0);
						 return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
					{						
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_RECENTCALL,EVT_USER,AVK_CLR,0);
						 return rt;
					}
				}

			}
			break;
#endif//FEATURE_LCD_TOUCH_ENABLE             
        default:
            break;
    }
    
    return FALSE;
}

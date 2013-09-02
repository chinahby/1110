/*==============================================================================
// 文件：
//        ConverterDialogHandler.c
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
                                 宏定义和常数
==============================================================================*/
// 关闭对话框宏
#define CLOSE_DIALOG(DlgRet)  {    \
                                     pMe->m_eDlgRet = DlgRet; \
                                     (void) ISHELL_EndDialog(pMe->m_pShell);  \
                                 }

#define IS_NOT_NUMBER(c) ((c != 0) && (c != '-') && (c > '9' || c < '0'))

#define FEATURE_RESET_CURRENCY
/*==============================================================================
                                 类型定义
==============================================================================*/


/*==============================================================================
                                 函数声明
==============================================================================*/
// 对话框 IDD_MSGBOX 事件处理函数
static boolean  Converter_MsgBoxEvent(CConverter *pMe,
        AEEEvent eCode,
        uint16 wParam,
        uint32 dwParam
);

// 对话框 IDD_CONVERTERMAIN 事件处理函数
static boolean  Converter_MainEvent(CConverter *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

// 对话框 IDD_CONVERTERCONVERT 事件处理函数
static boolean  Converter_ConvertEvent(CConverter *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

// 对话框 IDD_CHANGECURRENCY 事件处理函数
static boolean  Converter_ChangeCurrencyEvent(CConverter *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

// 计算单位换算结果
static double Converter_CalcResult(CConverter *pMe);
// 处理换算之后的浮点数
static boolean Converter_FloatToWstr(double source, AECHAR * pdest, int nSize);
// 处理输入数字变更
static void Converter_AddChar(CConverter *pMe, char chAdd, char *pDest);
// 从TextCtl获取字符串并传递到输入数字缓存区
static void Converter_GetTextToInputNum(CConverter *pMe, AECHAR *pwstrTemp, char *pDest);

static void Converter_DialogTimeout(void *pme);
/*==============================================================================
                                 全局数据
==============================================================================*/


/*==============================================================================
                                 本地（静态）数据
==============================================================================*/


/*==============================================================================
                                 函数定义
==============================================================================*/
/*==============================================================================
函数:
       Converter_ShowDialog
说明:
       函数由状态处理函数用来创建相应对话框资源ID的对话框。

参数:
       pMe [In]: 指向Converter Applet对象结构的指针,该结构包含小程序的特定信息.
       dlgResId [in]：对话框资源ID，函数根据该ID创建对话框。

返回值:
       无

备注:


==============================================================================*/
void Converter_ShowDialog(CConverter *pMe,uint16  dlgResId)
{
    int nRet;

    // At most one dialog open at once
    if (ISHELL_GetActiveDialog(pMe->m_pShell) != NULL)
    {
        // Looks like there is one dialog already opened.
        // Flag an error an return without doing anything.
        ERR("Trying to create a dialog without closing the previous one",0,0,0);
        return;
    }
    
    nRet = ISHELL_CreateDialog(pMe->m_pShell,AEE_CONVERTER_LANGFILE,dlgResId,NULL);

    if (nRet != SUCCESS)
    {
        ERR("Failed to create the dialog in the Converter applet",0,0,0);
    }
}


/*==============================================================================
函数:
   Converter_ShowMsgBox

说明:
    函数由状态处理函数用来唤起消息对话框，显示消息提示用户。

参数:
    pMe [in]: 指向 recentcalls Applet 对象结构的指针。该结构包含小程序的特定信息。
    wTextResId [in]: 消息文本资源 ID。

返回值:
    none

备注:

==============================================================================*/
void Converter_ShowMsgBox(CConverter *pMe, uint16 wTextResId)
{
    pMe->m_wMsgResID = wTextResId;
   
    // 创建消息对话框.
    Converter_ShowDialog(pMe, IDD_MSGBOX);
}

/*==============================================================================
函数：
       Converter_RouteDialogEvent
说明：
       函数将BREW事件路由给当前活动对话框的事件处理函数。
       
参数：
       pMe [in]：指向Converter Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
boolean Converter_RouteDialogEvent(CConverter *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
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
            return Converter_MsgBoxEvent(pMe,eCode,wParam,dwParam);

        case IDD_CONVERTERMAIN:
            return Converter_MainEvent(pMe,eCode,wParam,dwParam);

        case IDD_CONVERTERCONVERT:
            return Converter_ConvertEvent(pMe,eCode,wParam,dwParam);

        case IDD_CHANGECURRENCY:
            return Converter_ChangeCurrencyEvent(pMe,eCode,wParam,dwParam);

        default:
            return FALSE;
    }
}

/*==============================================================================
函数：
       Converter_MsgBoxEvent
说明：
       IDD_MSGBOX对话框事件处理函数
       
参数：
       pMe [in]：指向Converter Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean  Converter_MsgBoxEvent(CConverter *pMe,
        AEEEvent eCode,
        uint16 wParam,
        uint32 dwParam
)
{
     static IStatic * pStatic = NULL;
    if (NULL == pMe)
    {
        return FALSE;
    }

    switch(eCode)
    {
        case EVT_DIALOG_INIT:
        {
           if (NULL == pStatic)
           {
                AEERect rect = {0};
                if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                                         AEECLSID_STATIC,
                                                         (void **)&pStatic))
                
                {
                    return FALSE;
                }        
                MSG_FATAL("pStatic address=%x", pStatic,0,0);
                ISTATIC_SetRect(pStatic, &rect);
           }
        }
           return TRUE;

        case EVT_DIALOG_START:
            ISHELL_SetTimer(pMe->m_pShell,CONVERTER_PROMPTMSG_TIMER,Converter_DialogTimeout,pMe);
            (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                    EVTFLG_ASYNC,
                                    AEECLSID_CONVERTER,
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
                                AEE_CONVERTER_LANGFILE,                                
                                pMe->m_wMsgResID,
                                wstrText,
                                sizeof(wstrText));
                                
                Msg_Param.ePMsgType = MESSAGE_INFORMATIVE;
                Msg_Param.pwszMsg = wstrText;
                Msg_Param.eBBarType = BTBAR_OK;
                DrawPromptMessage(pMe->m_pDisplay, pStatic, &Msg_Param);
            }
            // 更新界面
            IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            return TRUE;

        case EVT_DIALOG_END:
            if(pMe->m_wMsgResID == IDS_DONE)
            {
                pMe->m_eDlgRet = DLGRET_MSGBOX_OK;
            }
            else
            {
                pMe->m_eDlgRet = DLGRET_MSGBOX_CANCEL;
            }
            ISTATIC_Release(pStatic);
            pStatic = NULL;
            (void)ISHELL_CancelTimer(pMe->m_pShell, NULL,  pMe);
            return TRUE;
            
        case EVT_KEY:
            (void)ISHELL_CancelTimer(pMe->m_pShell, NULL,  pMe);
            if(pMe->m_wMsgResID == IDS_DONE)
            {
                CLOSE_DIALOG(DLGRET_MSGBOX_OK);
            }
            else
            {
                CLOSE_DIALOG(DLGRET_MSGBOX_CANCEL);
            }
            return TRUE;
            
        case EVT_DISPLAYDIALOGTIMEOUT:
            if(pMe->m_wMsgResID == IDS_DONE)
            {
                CLOSE_DIALOG(DLGRET_MSGBOX_OK);
            }
            else
            {
                CLOSE_DIALOG(DLGRET_MSGBOX_CANCEL);
            }
            return TRUE;
 #ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
		case EVT_PEN_UP:
			{
				int16 wXPos = (int16)AEE_GET_X((const char *)dwParam);
				int16 wYPos = (int16)AEE_GET_Y((const char *)dwParam);
				AEERect bottomBarRect;
				//int ht;
				int nBarH ;
				AEEDeviceInfo devinfo;
				nBarH = GetBottomBarHeight(pMe->m_pDisplay);
				
				MEMSET(&devinfo, 0, sizeof(devinfo));
				ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
				SETAEERECT(&bottomBarRect, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);
				if( TOUCH_PT_IN_RECT(wXPos, wYPos, bottomBarRect))
				{
					if(wXPos >= bottomBarRect.x + (bottomBarRect.dx/3)*2 && wXPos < bottomBarRect.x + (bottomBarRect.dx/3)*3 )//右
					{						
						(void)ISHELL_CancelTimer(pMe->m_pShell, NULL,  pMe);
			            if(pMe->m_wMsgResID == IDS_DONE)
			            {
			                CLOSE_DIALOG(DLGRET_MSGBOX_OK);
			            }
			            else
			            {
			                CLOSE_DIALOG(DLGRET_MSGBOX_CANCEL);
			            }
			            return TRUE;
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
       Converter_MainEvent
说明：
       IDD_CONVERTERMAIN对话框事件处理函数
       
参数：
       pMe [in]：指向Converter Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean  Converter_MainEvent(CConverter *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
    PARAM_NOT_REF(dwParam)
    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pDialog, IDC_CONVERTERMAIN);
    if (pMenu == NULL)
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
                        AEE_CONVERTER_LANGFILE,                                
                        IDS_CONVERTER_TITLE,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldTextEx(pMe->m_pIAnn,WTitle,FALSE);
            }
            IMENUCTL_AddItem(pMenu, AEE_CONVERTER_LANGFILE, IDS_CURRENCY_TITLE, IDS_CURRENCY_TITLE, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_CONVERTER_LANGFILE, IDS_LENGTH_TITLE, IDS_LENGTH_TITLE, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_CONVERTER_LANGFILE, IDS_WEIGHT_TITLE, IDS_WEIGHT_TITLE, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_CONVERTER_LANGFILE, IDS_VOLUME_TITLE, IDS_VOLUME_TITLE, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_CONVERTER_LANGFILE, IDS_AREA_TITLE, IDS_AREA_TITLE, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_CONVERTER_LANGFILE, IDS_TEMPERATURE_TITLE, IDS_TEMPERATURE_TITLE, NULL, 0);
            return TRUE;
       
        case EVT_DIALOG_START:
            IMENUCTL_SetProperties(pMenu, MP_BIND_ITEM_TO_NUMBER_KEY |MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_ACTIVE_NO_REDRAW);
            IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);
            IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
            IMENUCTL_SetSel(pMenu,pMe->m_selectedItem);
            (void)IMENUCTL_Redraw(pMenu);
            return TRUE;
         
        case EVT_KEY:
            if(wParam == AVK_CLR)
            {
                CLOSE_DIALOG(DLGRET_CANCELED)
            }
            return TRUE;
          
        case EVT_COMMAND:
            //pMe->m_selectedItem= IMENUCTL_GetSel(pMenu);
            pMe->m_selectedItem= wParam;
            switch(wParam)
            {
                case IDS_CURRENCY_TITLE:
                    CLOSE_DIALOG(DLGRET_CURRENCY)
                    break;
           
                case IDS_LENGTH_TITLE:
                    CLOSE_DIALOG(DLGRET_LENGTH)
                    break;
               
                case IDS_WEIGHT_TITLE:
                    CLOSE_DIALOG(DLGRET_WEIGHT)
                    break;
            
                case IDS_VOLUME_TITLE:
                    CLOSE_DIALOG(DLGRET_VOLUME)
                    break;
               
                case IDS_AREA_TITLE:
                    CLOSE_DIALOG(DLGRET_AREA)
                    break;
    
                case IDS_TEMPERATURE_TITLE:
                    CLOSE_DIALOG(DLGRET_TEMPERATURE)
                    break;
    
                default:
                    break;
            }
            return TRUE;
          
        case EVT_DIALOG_END:
            return TRUE;
       
        default:
            break;
    }
    return FALSE;
}

/*==============================================================================
函数：
       Converter_ConvertEvent
说明：
       IDD_CONVERTERCONVERT对话框事件处理函数
       
参数：
       pMe [in]：指向Converter Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean  Converter_ConvertEvent(CConverter *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
    PARAM_NOT_REF(dwParam)
    static char   *m_inputNumber = NULL;
    int              nLen = 0;
    AECHAR       wstrDisplay[MAX_INPUT_NUMBER+3] = {0};
	#ifdef FEATURE_LCD_TOUCH_ENABLE
	
	AEERect      p_Number1 = {0};
	AEERect		 p_Number2 = {0};
	AEERect		 p_Menu1   = {0};
	AEERect      p_Menu2   = {0};
	#endif
    if (NULL == pMe)
    {
        return FALSE;
    }
    MSG_FATAL("Converter_ConvertEvent---------eCode=%x-----wParam=%x",eCode,wParam,0);
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
        {
            int i                = 0;
            if(m_inputNumber == NULL)
            {
                m_inputNumber = (char *)MALLOC((MAX_INPUT_NUMBER + 3)* sizeof(char));
            }
            MSG_FATAL("m_inputNumber address=%x", m_inputNumber,0,0);
            switch(pMe->m_converterMode)
            {
                case CONVERTER_MODE_CURRENCY:
                    {
                        (void)ISHELL_GetPrefs(pMe->m_pShell,
                                            AEECLSID_CONVERTER,
                                            CONVERTER_CFG_CURRENCY,
                                            &pMe->m_CurrencyCFG,
                                            sizeof(ConverterCurrencyCfg));
                
                        for(i=0;i<CONVERTER_CURRENCY_NUMBER;i++)
                        {
                            if(pMe->m_CurrencyCFG.customCurrency[i] <= 0)
                            {
                                pMe->m_CurrencyCFG.customCurrency[i] = coefficient[i];
                            }
                        }
                        
                        for(i=0;i<CONVERTER_CURRENCY_NUMBER;i++)
                        {
                            (void)IMENUCTL_AddItem( pMe->pUnitMenu1,
                                                            AEE_CONVERTER_LANGFILE,
                                                            IDS_CURRENCY_EUR + i,
                                                            IDS_CURRENCY_EUR + i,
                                                            NULL,
                                                            0);
                            (void)IMENUCTL_AddItem( pMe->pUnitMenu2,
                                                            AEE_CONVERTER_LANGFILE,
                                                            IDS_CURRENCY_EUR + i,
                                                            IDS_CURRENCY_EUR + i,
                                                            NULL,
                                                            0);
                        }
                    }
                    break;

                case CONVERTER_MODE_LENGTH:
                    {
                        for(i=0;i<=(IDS_LENGTH_M - IDS_LENGTH_KM);i++)
                        {
                            (void)IMENUCTL_AddItem( pMe->pUnitMenu1,
                                                            AEE_CONVERTER_LANGFILE,
                                                            IDS_LENGTH_KM + i,
                                                            IDS_LENGTH_KM + i,
                                                            NULL,
                                                            0);
                            (void)IMENUCTL_AddItem( pMe->pUnitMenu2,
                                                            AEE_CONVERTER_LANGFILE,
                                                            IDS_LENGTH_KM + i,
                                                            IDS_LENGTH_KM + i,
                                                            NULL,
                                                            0);
                        }
                    }
                    break;

                case CONVERTER_MODE_WEIGHT:
                    {
                        for(i=0;i<=(IDS_WEIGHT_GRAIN - IDS_WEIGHT_OZ);i++)
                        {
                            (void)IMENUCTL_AddItem( pMe->pUnitMenu1,
                                                            AEE_CONVERTER_LANGFILE,
                                                            IDS_WEIGHT_OZ + i,
                                                            IDS_WEIGHT_OZ + i,
                                                            NULL,
                                                            0);
                            (void)IMENUCTL_AddItem( pMe->pUnitMenu2,
                                                            AEE_CONVERTER_LANGFILE,
                                                            IDS_WEIGHT_OZ + i,
                                                            IDS_WEIGHT_OZ + i,
                                                            NULL,
                                                            0);
                        }
                    }
                    break;

                case CONVERTER_MODE_VOLUME:
                    {
                        for(i=0;i<=(IDS_VOLUME_GAL - IDS_VOLUME_CM3);i++)
                        {
                            (void)IMENUCTL_AddItem( pMe->pUnitMenu1,
                                                            AEE_CONVERTER_LANGFILE,
                                                            IDS_VOLUME_CM3 + i,
                                                            IDS_VOLUME_CM3 + i,
                                                            NULL,
                                                            0);
                            (void)IMENUCTL_AddItem( pMe->pUnitMenu2,
                                                            AEE_CONVERTER_LANGFILE,
                                                            IDS_VOLUME_CM3 + i,
                                                            IDS_VOLUME_CM3 + i,
                                                            NULL,
                                                            0);
                        }
                    }
                    break;

                case CONVERTER_MODE_AREA:
                    {
                        for(i=0;i<=(IDS_AREA_ACRE - IDS_AREA_CM2);i++)
                        {
                            (void)IMENUCTL_AddItem( pMe->pUnitMenu1,
                                                            AEE_CONVERTER_LANGFILE,
                                                            IDS_AREA_CM2 + i,
                                                            IDS_AREA_CM2 + i,
                                                            NULL,
                                                            0);
                            (void)IMENUCTL_AddItem( pMe->pUnitMenu2,
                                                            AEE_CONVERTER_LANGFILE,
                                                            IDS_AREA_CM2 + i,
                                                            IDS_AREA_CM2 + i,
                                                            NULL,
                                                            0);
                        }
                    }
                    break;

                case CONVERTER_MODE_TEMPERATURE:
                    {
                        for(i=0;i<=(IDS_TEMPERATURE_F - IDS_TEMPERATURE_C);i++)
                        {
                            (void)IMENUCTL_AddItem( pMe->pUnitMenu1,
                                                            AEE_CONVERTER_LANGFILE,
                                                            IDS_TEMPERATURE_C + i,
                                                            IDS_TEMPERATURE_C + i,
                                                            NULL,
                                                            0);
                            (void)IMENUCTL_AddItem( pMe->pUnitMenu2,
                                                            AEE_CONVERTER_LANGFILE,
                                                            IDS_TEMPERATURE_C + i,
                                                            IDS_TEMPERATURE_C + i,
                                                            NULL,
                                                            0);
                        }
                    }
                    break;

                default:
                    return TRUE;
            }
            
            pMe->m_nCtlID = IDC_NUMBER1;
            return TRUE;
        }
            
        case EVT_DIALOG_START:
        {
            switch(pMe->m_converterMode)
            {
                case CONVERTER_MODE_CURRENCY:
                    {
                    	#if defined(FEATURE_VERSION_W515V3)||defined(FEATURE_VERSION_S1000T)
                    	IMENUCTL_SetSel(pMe->pUnitMenu1, IDS_CURRENCY_USD);
                    	IMENUCTL_SetSel(pMe->pUnitMenu2, IDS_CURRENCY_INR);
                    	#else
                        if((pMe->basiccoefficient >= IDS_CURRENCY_EUR) && (pMe->basiccoefficient <= IDS_CURRENCY_ILS))
                        {
                            IMENUCTL_SetSel(pMe->pUnitMenu1, pMe->basiccoefficient);
                        }
                        else
                        {
                            IMENUCTL_SetSel(pMe->pUnitMenu1, IDS_CURRENCY_EUR);
                        }
                        IMENUCTL_SetSel(pMe->pUnitMenu2, IDS_CURRENCY_EUR + 1);
                        #endif
                    }
                    break;
                    
                case CONVERTER_MODE_LENGTH:
                    {
                        IMENUCTL_SetSel(pMe->pUnitMenu1, IDS_LENGTH_KM);
                        IMENUCTL_SetSel(pMe->pUnitMenu2, IDS_LENGTH_KM + 1);
                    }
                    break;
                    
                case CONVERTER_MODE_WEIGHT:
                    {
                        IMENUCTL_SetSel(pMe->pUnitMenu1, IDS_WEIGHT_OZ);
                        IMENUCTL_SetSel(pMe->pUnitMenu2, IDS_WEIGHT_OZ + 1);
                    }
                    break;
                    
                case CONVERTER_MODE_VOLUME:
                    {
                        IMENUCTL_SetSel(pMe->pUnitMenu1, IDS_VOLUME_CM3);
                        IMENUCTL_SetSel(pMe->pUnitMenu2, IDS_VOLUME_CM3 + 1);
                    }
                    break;
                    
                case CONVERTER_MODE_AREA:
                    {
                        IMENUCTL_SetSel(pMe->pUnitMenu1, IDS_AREA_CM2);
                        IMENUCTL_SetSel(pMe->pUnitMenu2, IDS_AREA_CM2 + 1);
                    }
                    break;
                    
                case CONVERTER_MODE_TEMPERATURE:
                    {
                        IMENUCTL_SetSel(pMe->pUnitMenu1, IDS_TEMPERATURE_C);
                        IMENUCTL_SetSel(pMe->pUnitMenu2, IDS_TEMPERATURE_C + 1);
                    }
                    break;

                default:
                    return TRUE;
            }            
            (void) ISHELL_PostEvent(pMe->m_pShell,
                                    AEECLSID_CONVERTER,
                                    EVT_USER_REDRAW,
                                    NULL,
                                    NULL);
            return TRUE;
        }
            
        case EVT_USER_REDRAW:
        {
            AECHAR                      text[16]     = {0};
            TitleBar_Param_type     TBarParam = {0};
            BottomBar_Param_type  BBarParam ={0};
            IImage                       *Image;
            AEERect                     rect;
            AEEImageInfo iInfo;
            //先清屏
            Appscommon_ResetBackgroundEx(pMe->m_pDisplay, &pMe->m_rc, TRUE);
            // 画标题
            switch(pMe->m_converterMode)
            {
                case CONVERTER_MODE_CURRENCY:
                    (void)ISHELL_LoadResString(pMe->m_pShell, 
                                                        AEE_CONVERTER_LANGFILE,
                                                        IDS_CURRENCY_TITLE, 
                                                        text,
                                                        sizeof(text));
                    break;
                
                case CONVERTER_MODE_LENGTH:
                    (void)ISHELL_LoadResString(pMe->m_pShell, 
                                                        AEE_CONVERTER_LANGFILE,
                                                        IDS_LENGTH_TITLE, 
                                                        text,
                                                        sizeof(text));
                    break;
                
                case CONVERTER_MODE_WEIGHT:
                    (void)ISHELL_LoadResString(pMe->m_pShell, 
                                                        AEE_CONVERTER_LANGFILE,
                                                        IDS_WEIGHT_TITLE, 
                                                        text,
                                                        sizeof(text));
                    break;
                
                case CONVERTER_MODE_VOLUME:
                    (void)ISHELL_LoadResString(pMe->m_pShell, 
                                                        AEE_CONVERTER_LANGFILE,
                                                        IDS_VOLUME_TITLE, 
                                                        text,
                                                        sizeof(text));
                    break;
                
                case CONVERTER_MODE_AREA:
                    (void)ISHELL_LoadResString(pMe->m_pShell, 
                                                        AEE_CONVERTER_LANGFILE,
                                                        IDS_AREA_TITLE, 
                                                        text,
                                                        sizeof(text));
                    break;
                
                case CONVERTER_MODE_TEMPERATURE:
                    (void)ISHELL_LoadResString(pMe->m_pShell, 
                                                        AEE_CONVERTER_LANGFILE,
                                                        IDS_TEMPERATURE_TITLE, 
                                                        text,
                                                        sizeof(text));
                    break;
                    
                default:
                    break;
            }
            TBarParam.pwszTitle = text;
            TBarParam.dwAlignFlags = IDF_ALIGN_MIDDLE | IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE;
			#if 0
            DrawTitleBar(pMe->m_pDisplay, &TBarParam);
			#else
			IANNUNCIATOR_SetFieldTextEx(pMe->m_pIAnn,text,FALSE);
			#endif
            #ifdef FEATURE_LCD_TOUCH_ENABLE
            if (pMe->PENUPbRedraw)
               {
                if(pMe->PENUPbCalc)
                {
                    double result;                   
                    if(pMe->m_nCtlID == IDC_NUMBER1 || pMe->m_nCtlID == IDC_UNIT_MENU1)
                    {
                        ITEXTCTL_GetText(pMe->pNumber1, wstrDisplay, sizeof(wstrDisplay));                      
                    }
                    else
                    {
                        ITEXTCTL_GetText(pMe->pNumber2, wstrDisplay, sizeof(wstrDisplay));
                    }                    
                    if(WSTRLEN(wstrDisplay) == 0 || WSTRLEN(wstrDisplay) >7)
                    {
                        wstrDisplay[0] = '\0';
                    }
                    else                       
                    {
                        result = Converter_CalcResult(pMe);
                        if(pMe->b_overflow)
                        {
                            (void)ISHELL_LoadResString(pMe->m_pShell, 
                                                                AEE_CONVERTER_LANGFILE,
                                                                IDS_OVERFLOW, 
                                                                wstrDisplay,
                                                                sizeof(wstrDisplay));
                        }
                        else
                        {
                            (void)Converter_FloatToWstr(result, wstrDisplay, sizeof(wstrDisplay));
                        }
                    }
                    
                    if(pMe->m_nCtlID == IDC_NUMBER1 || pMe->m_nCtlID == IDC_UNIT_MENU1)
                    {
                        ITEXTCTL_SetText(pMe->pNumber2, wstrDisplay, -1);
                    }
                    else
                    {
                        ITEXTCTL_SetText(pMe->pNumber1, wstrDisplay, -1);
                    }
                }
            }
            #endif
            //画箭头
            Image = ISHELL_LoadResImage(pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDB_LEFTARROW);
			IImage_GetInfo(Image,&iInfo);
            MSG_FATAL("Image address=%x",Image,0,0);
            if(Image)
            {
                //IIMAGE_Draw(Image, pMe->m_rc.x, pMe->m_rc.y + TITLEBAR_HEIGHT + (pMe->dyMenu - ARROW_HEIGHT)/2);
                //IIMAGE_Draw(Image, pMe->m_rc.x, pMe->m_rc.y + TITLEBAR_HEIGHT + pMe->dyMenu*2 + (pMe->dyMenu - ARROW_HEIGHT)/2);

				//Add By zzg 2010_07_08
                IIMAGE_Draw(Image, pMe->m_rc.x, pMe->m_rc.y + TITLEBAR_HEIGHT/2 + (pMe->dyMenu - ARROW_HEIGHT)/2 - 1*FRAME_SIZE);
                IIMAGE_Draw(Image, pMe->m_rc.x, pMe->m_rc.y + TITLEBAR_HEIGHT/2 + pMe->dyMenu*2 + (pMe->dyMenu - ARROW_HEIGHT)/2 + 5*FRAME_SIZE);
                //Add End
                IIMAGE_Release(Image);
            }
            
            Image = ISHELL_LoadResImage(pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDB_RIGHTARROW);
            MSG_FATAL("Image address=%x",Image,0,0);
            if(Image)
            {
                //IIMAGE_Draw(Image, pMe->m_rc.dx - ARROW_WIDTH, pMe->m_rc.y + TITLEBAR_HEIGHT + (pMe->dyMenu - ARROW_HEIGHT)/2);
                //IIMAGE_Draw(Image, pMe->m_rc.dx - ARROW_WIDTH, pMe->m_rc.y + TITLEBAR_HEIGHT + pMe->dyMenu*2 + (pMe->dyMenu - ARROW_HEIGHT)/2);

				//Add By zzg 2010_07_08
				IIMAGE_Draw(Image, pMe->m_rc.dx - ARROW_WIDTH, pMe->m_rc.y + TITLEBAR_HEIGHT/2 + (pMe->dyMenu - ARROW_HEIGHT)/2 - 1*FRAME_SIZE);
                IIMAGE_Draw(Image, pMe->m_rc.dx - ARROW_WIDTH, pMe->m_rc.y + TITLEBAR_HEIGHT/2 + pMe->dyMenu*2 + (pMe->dyMenu - ARROW_HEIGHT)/2 + 5*FRAME_SIZE);
                //Add End
				IIMAGE_Release(Image);
            }
#ifdef FEATURE_LCD_TOUCH_ENABLE
				SETAEERECT(&pMe->pL_rect1, pMe->m_rc.x, pMe->m_rc.y + TITLEBAR_HEIGHT/2 + (pMe->dyMenu - ARROW_HEIGHT)/2 - 1*FRAME_SIZE, iInfo.cx+10, iInfo.cy); 
				SETAEERECT(&pMe->pL_rect2, pMe->m_rc.x, pMe->m_rc.y + TITLEBAR_HEIGHT/2 + pMe->dyMenu*2 + (pMe->dyMenu - ARROW_HEIGHT)/2 + 5*FRAME_SIZE, iInfo.cx+10, iInfo.cy); 
				SETAEERECT(&pMe->pR_rect1, pMe->m_rc.dx - ARROW_WIDTH-10, pMe->m_rc.y + TITLEBAR_HEIGHT/2 + (pMe->dyMenu - ARROW_HEIGHT)/2 - 1*FRAME_SIZE, iInfo.cx+10, iInfo.cy); 
				SETAEERECT(&pMe->pR_rect2, pMe->m_rc.dx - ARROW_WIDTH-10, pMe->m_rc.y + TITLEBAR_HEIGHT/2 + pMe->dyMenu*2 + (pMe->dyMenu - ARROW_HEIGHT)/2 + 5*FRAME_SIZE, iInfo.cx+10, iInfo.cy); 
#endif

            //画底条
            nLen = STRLEN(m_inputNumber);
            if(pMe->m_nCtlID == IDC_UNIT_MENU1 ||pMe->m_nCtlID == IDC_UNIT_MENU2 ||nLen <= 0)
            {
                if(pMe->m_converterMode == CONVERTER_MODE_CURRENCY)
                {
                    BBarParam.eBBarType = BTBAR_OPTION_BACK;
                }
                else
                {
                    BBarParam.eBBarType = BTBAR_BACK;
                }
            }
            else
            {
#ifndef FEATURE_VERSION_W317A            
                if(pMe->m_converterMode == CONVERTER_MODE_CURRENCY)
                {
                    BBarParam.eBBarType = BTBAR_BACK;
                }
                else
#endif					
                {
                	#ifdef FEATURE_ALL_KEY_PAD
                	BBarParam.eBBarType = BTBAR_BACK;
                	#else
                    BBarParam.eBBarType = BTBAR_DELETE;
                    #endif
                }
            }
            DrawBottomBar(pMe->m_pDisplay, &BBarParam);
			
            //更新高亮显示
            SETAEERECT(&rect, 
                                pMe->m_rc.x, 
                                pMe->m_rc.y + TITLEBAR_HEIGHT/2 + ((int)pMe->m_nCtlID - 2)*pMe->dyMenu, 
                                pMe->m_rc.dx, 
                                pMe->dyMenu);
			/*
            if(pMe->m_nCtlID == IDC_UNIT_MENU1 || pMe->m_nCtlID == IDC_UNIT_MENU2)
            {
                rect.x  += ARROW_WIDTH;
                rect.dx -= (2*ARROW_WIDTH);
            }
            */	

			//Add By zzg 2010_07_08
			if (pMe->m_nCtlID == IDC_UNIT_MENU1 )
            {
                rect.x  += ARROW_WIDTH;
                rect.dx -= (2*ARROW_WIDTH);
            }
			else if (pMe->m_nCtlID == IDC_UNIT_MENU2)
			{
				rect.x  += ARROW_WIDTH;
                rect.dx -= (2*ARROW_WIDTH);
				rect.y  += 8*FRAME_SIZE;
			}
			else if(pMe->m_nCtlID == IDC_NUMBER1)
            {                
                rect.dy += 6*FRAME_SIZE;
            }
			else if (pMe->m_nCtlID == IDC_NUMBER2)
			{
				rect.dy += 6*FRAME_SIZE;
				rect.y  += 8*FRAME_SIZE;
			}
			//Add End
			
            IDISPLAY_FillRect(pMe->m_pDisplay, &rect, RGB_ORANGE);
            
            // 更新显示
            (void)IMENUCTL_Redraw(pMe->pUnitMenu1);
            (void)IMENUCTL_Redraw(pMe->pUnitMenu2);
            if(pMe->m_nCtlID == IDC_NUMBER1)
            {
                ITEXTCTL_SetActive(pMe->pNumber1, TRUE);
                ITEXTCTL_SetActive(pMe->pNumber2, FALSE);
            }
            else if(pMe->m_nCtlID == IDC_NUMBER2)
            {
                ITEXTCTL_SetActive(pMe->pNumber1, FALSE);
                ITEXTCTL_SetActive(pMe->pNumber2, TRUE);
            }
            else
            {
                ITEXTCTL_SetActive(pMe->pNumber1, FALSE);
                ITEXTCTL_SetActive(pMe->pNumber2, FALSE);
            }
            
            IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            return TRUE;
        }
            
        case EVT_DIALOG_END:
        {
            #ifdef FEATURE_LCD_TOUCH_ENABLE
            TSIM_NumberKeypad(FALSE);
            pMe->PENUPbRedraw = FALSE;
            pMe->PENUPbCalc = FALSE;
            #endif
            IMENUCTL_DeleteAll(pMe->pUnitMenu1);
            IMENUCTL_DeleteAll(pMe->pUnitMenu2);
            FREEIF(m_inputNumber);
            return TRUE;
        }
		
		//Add By zzg 2012_03_01
		case EVT_KEY_HELD:
		{
			if (AVK_DEL == wParam)
			{
				if(pMe->m_nCtlID == IDC_NUMBER1 ||pMe->m_nCtlID == IDC_NUMBER2)
				{
					ITEXTCTL_Reset(pMe->pNumber1);
					ITEXTCTL_Reset(pMe->pNumber2);	

					(void) ISHELL_PostEvent(pMe->m_pShell,
                                            AEECLSID_CONVERTER,
                                            EVT_USER_REDRAW,
                                            0,
                                            0);
				}
			}
			return TRUE;
		}
		//Add End
		
        case EVT_KEY:
        {
            char     chEnter  = 0;
            boolean bRedraw = FALSE;
            boolean bCalc = FALSE;
            uint16   ctlID = 0;

			MSG_FATAL("***zzg EVT_KEY wParam=%x***", wParam,0,0);
			
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
                {
                    if(pMe->m_nCtlID == IDC_NUMBER1 ||pMe->m_nCtlID == IDC_NUMBER2)
                    {
                        bRedraw = TRUE;
                        bCalc = TRUE;
                        chEnter = '0' + (wParam - AVK_0);
                        
                        Converter_GetTextToInputNum(pMe, wstrDisplay, m_inputNumber);
                        Converter_AddChar(pMe, chEnter, m_inputNumber);
                    }
                }
                    break;

				//Add By zzg 2012_03_02
				case AVK_DEL:
				{
					if(pMe->m_nCtlID == IDC_NUMBER1 ||pMe->m_nCtlID == IDC_NUMBER2)
					{
						Converter_GetTextToInputNum(pMe, wstrDisplay, m_inputNumber);
					}					

					nLen = STRLEN(m_inputNumber);
					
					if (nLen != 0)					
					{
						bRedraw = TRUE;
						bCalc = TRUE;
						chEnter = 0;
						if(IS_NOT_NUMBER(m_inputNumber[0]))
						{
						    MEMSET(m_inputNumber, 0, (MAX_INPUT_NUMBER + 3)*sizeof(char));
						}
						else
						{
						    m_inputNumber[nLen - 1] = chEnter;
						}
					}
                                      
					break;
				}
				//Add End	

				case AVK_M:			//Add By zzg 2012_03_01
                case AVK_STAR:
                {
                    if(pMe->m_nCtlID == IDC_NUMBER1 ||pMe->m_nCtlID == IDC_NUMBER2)
                    {
                        bRedraw = TRUE;
                        bCalc = TRUE;
                        chEnter = '.';
                        
                        Converter_GetTextToInputNum(pMe, wstrDisplay, m_inputNumber);
                        Converter_AddChar(pMe, chEnter, m_inputNumber);
                    }
                }
                    break;
                    
                case AVK_POUND:
                {
                    if((pMe->m_converterMode == CONVERTER_MODE_TEMPERATURE) &&
                        (pMe->m_nCtlID == IDC_NUMBER1 ||pMe->m_nCtlID == IDC_NUMBER2))
                    {
                        bRedraw = TRUE;
                        bCalc = TRUE;
                        chEnter = '-';

                        Converter_GetTextToInputNum(pMe, wstrDisplay, m_inputNumber);
                        Converter_AddChar(pMe, chEnter, m_inputNumber);
                    }
                    else
                    {
                        return TRUE;
                    }
                }
                    break;

                case AVK_SELECT:
                case AVK_INFO:
                {
                    if(pMe->m_converterMode == CONVERTER_MODE_CURRENCY)
                    {
                        if(pMe->m_nCtlID == IDC_NUMBER1 || pMe->m_nCtlID == IDC_UNIT_MENU1)
                        {
                            pMe->basiccoefficient = IMENUCTL_GetSel(pMe->pUnitMenu1);
                        }
                        else
                        {
                            pMe->basiccoefficient = IMENUCTL_GetSel(pMe->pUnitMenu2);
                        }
                        CLOSE_DIALOG(DLGRET_CHANGECURRENCY)
                        return TRUE;
                    }
                    else
                    {
                        return TRUE;
                    }
                }

                case AVK_CLR:
                {
                    #ifdef FEATURE_ALL_KEY_PAD
                	if(dwParam == 1)
                	#else
                	if(TRUE)
                	#endif
                	{
	                    if(pMe->m_nCtlID == IDC_NUMBER1 ||pMe->m_nCtlID == IDC_NUMBER2)
	                    {
	                        Converter_GetTextToInputNum(pMe, wstrDisplay, m_inputNumber);
	                    }
	                    else
	                    {
	                        ITEXTCTL_Reset(pMe->pNumber1);
	                        ITEXTCTL_Reset(pMe->pNumber2);
	                        CLOSE_DIALOG(DLGRET_CANCELED)
	                        return TRUE;
	                    }
	                    
	                    nLen = STRLEN(m_inputNumber);
	                    if (nLen == 0 || pMe->m_nCtlID == IDC_UNIT_MENU1 ||pMe->m_nCtlID == IDC_UNIT_MENU1)
	                    {
	                        if(pMe->m_converterMode == CONVERTER_MODE_CURRENCY)
	                        {
	                            pMe->basiccoefficient = IDS_CURRENCY_IDR;
	                        }
	                        ITEXTCTL_Reset(pMe->pNumber1);
	                        ITEXTCTL_Reset(pMe->pNumber2);
	                        CLOSE_DIALOG(DLGRET_CANCELED)
	                        return TRUE;
	                    }
	                    else
	                    {
	                        bRedraw = TRUE;
	                        bCalc = TRUE;
	                        chEnter = 0;
	                        if(IS_NOT_NUMBER(m_inputNumber[0]))
	                        {
	                            MEMSET(m_inputNumber, 0, (MAX_INPUT_NUMBER + 3)*sizeof(char));
	                        }
	                        else
	                        {
	                            m_inputNumber[nLen - 1] = chEnter;
	                        }
	                    }
                    }
                    else
                    {
	                    if(pMe->m_converterMode == CONVERTER_MODE_CURRENCY)
	                    {
	                          pMe->basiccoefficient = IDS_CURRENCY_IDR;
	                    }
	                    ITEXTCTL_Reset(pMe->pNumber1);
	                    ITEXTCTL_Reset(pMe->pNumber2);
	                    CLOSE_DIALOG(DLGRET_CANCELED)
	                    return TRUE;
                    }
                }
                    break;
                        
                case AVK_DOWN:
                {
                    bRedraw = TRUE;
                    switch(pMe->m_nCtlID)
                    {
                        case IDC_UNIT_MENU1:
                        {
                            pMe->m_nCtlID = IDC_NUMBER1;
                        }
                            break;
                        
                        case IDC_NUMBER1:
                        {
                            pMe->m_nCtlID = IDC_UNIT_MENU2;
                            ITEXTCTL_GetText(pMe->pNumber1, wstrDisplay, sizeof(wstrDisplay));
                            nLen = WSTRLEN(wstrDisplay);
                            if(wstrDisplay[nLen - 1] == L'.')
                            {
                                wstrDisplay[nLen - 1] = L'\0';
                            }
                            ITEXTCTL_SetText(pMe->pNumber1, wstrDisplay, -1);
                        }
                            break;
                        
                        case IDC_UNIT_MENU2:
                        {
                            pMe->m_nCtlID = IDC_NUMBER2;
                        }
                            break;
                        
                        case IDC_NUMBER2:
                        {
                            pMe->m_nCtlID = IDC_UNIT_MENU1;
                            ITEXTCTL_GetText(pMe->pNumber2, wstrDisplay, sizeof(wstrDisplay));
                            nLen = WSTRLEN(wstrDisplay);
                            if(wstrDisplay[nLen - 1] == L'.')
                            {
                                wstrDisplay[nLen - 1] = L'\0';
                            }
                            ITEXTCTL_SetText(pMe->pNumber2, wstrDisplay, -1);
                        }
                            break;
                    }
                }
                    break;
                    
                case AVK_UP:
                {
                    bRedraw = TRUE;
                    switch(pMe->m_nCtlID)
                    {
                        case IDC_UNIT_MENU1:
                        {
                            pMe->m_nCtlID = IDC_NUMBER2;
                        }
                            break;
                        
                        case IDC_NUMBER1:
                        {
                            pMe->m_nCtlID = IDC_UNIT_MENU1;
                            ITEXTCTL_GetText(pMe->pNumber1, wstrDisplay, sizeof(wstrDisplay));
                            nLen = WSTRLEN(wstrDisplay);
                            if(wstrDisplay[nLen - 1] == L'.')
                            {
                                wstrDisplay[nLen - 1] = L'\0';
                            }
                            ITEXTCTL_SetText(pMe->pNumber1, wstrDisplay, -1);
                        }
                            break;
                        
                        case IDC_UNIT_MENU2:
                        {
                            pMe->m_nCtlID = IDC_NUMBER1;
                        }
                            break;
                        
                        case IDC_NUMBER2:
                        {
                            pMe->m_nCtlID = IDC_UNIT_MENU2;
                            ITEXTCTL_GetText(pMe->pNumber2, wstrDisplay, sizeof(wstrDisplay));
                            nLen = WSTRLEN(wstrDisplay);
                            if(wstrDisplay[nLen - 1] == L'.')
                            {
                                wstrDisplay[nLen - 1] = L'\0';
                            }
                            ITEXTCTL_SetText(pMe->pNumber2, wstrDisplay, -1);
                        }
                            break;
                    }
                } 
                    break;
                        
                case AVK_RIGHT:
                {
                    if(pMe->m_nCtlID == IDC_UNIT_MENU1 || pMe->m_nCtlID == IDC_UNIT_MENU2)
                    {
                        if(pMe->m_nCtlID == IDC_UNIT_MENU1)
                        {
                            ctlID = IMENUCTL_GetSel(pMe->pUnitMenu1);
                        }
                        else
                        {
                            ctlID = IMENUCTL_GetSel(pMe->pUnitMenu2);
                        }
                        switch(pMe->m_converterMode)
                        {
                            case CONVERTER_MODE_CURRENCY:
                                {
                                    if(ctlID < IDS_CURRENCY_IDR)
                                    {
                                        ctlID++;
                                    }
                                    else
                                    {
                                        ctlID = IDS_CURRENCY_EUR;
                                    }
                                }
                                break;
                                
                            case CONVERTER_MODE_LENGTH:
                                {
                                    if(ctlID < IDS_LENGTH_M)
                                    {
                                        ctlID++;
                                    }
                                    else
                                    {
                                        ctlID = IDS_LENGTH_KM;
                                    }
                                }
                                break;
                                
                            case CONVERTER_MODE_WEIGHT:
                                {
                                    if(ctlID < IDS_WEIGHT_GRAIN)
                                    {
                                        ctlID++;
                                    }
                                    else
                                    {
                                        ctlID = IDS_WEIGHT_OZ;
                                    }
                                }
                                break;
                                
                            case CONVERTER_MODE_VOLUME:
                                {
                                    if(ctlID < IDS_VOLUME_GAL)
                                    {
                                        ctlID++;
                                    }
                                    else
                                    {
                                        ctlID = IDS_VOLUME_CM3;
                                    }
                                }
                                break;
                                
                            case CONVERTER_MODE_AREA:
                                {
                                    if(ctlID < IDS_AREA_ACRE)
                                    {
                                        ctlID++;
                                    }
                                    else
                                    {
                                        ctlID = IDS_AREA_CM2;
                                    }
                                }
                                break;
                                
                            case CONVERTER_MODE_TEMPERATURE:
                                {
                                    if(ctlID == IDS_TEMPERATURE_C)
                                    {
                                        ctlID = IDS_TEMPERATURE_F;
                                    }
                                    else
                                    {
                                        ctlID = IDS_TEMPERATURE_C;
                                    }
                                }
                                break;

                            default:
                                return TRUE;
                        }
                        if(pMe->m_nCtlID == IDC_UNIT_MENU1)
                        {
                            IMENUCTL_SetSel(pMe->pUnitMenu1, ctlID);
                        }
                        else
                        {
                            IMENUCTL_SetSel(pMe->pUnitMenu2, ctlID);
                        }
                        bRedraw = TRUE;
                        bCalc = TRUE;
                    }
                }
                    break;
                    
                case AVK_LEFT:
                {
                    if(pMe->m_nCtlID == IDC_UNIT_MENU1 || pMe->m_nCtlID == IDC_UNIT_MENU2)
                    {
                        if(pMe->m_nCtlID == IDC_UNIT_MENU1)
                        {
                            ctlID = IMENUCTL_GetSel(pMe->pUnitMenu1);
                        }
                        else
                        {
                            ctlID = IMENUCTL_GetSel(pMe->pUnitMenu2);
                        }
                        switch(pMe->m_converterMode)
                        {
                            case CONVERTER_MODE_CURRENCY:
                                {
                                    if(ctlID > IDS_CURRENCY_EUR)
                                    {
                                        ctlID--;
                                    }
                                    else
                                    {
                                        ctlID = IDS_CURRENCY_IDR;
                                    }
                                }
                                break;
                                
                            case CONVERTER_MODE_LENGTH:
                                {
                                    if(ctlID > IDS_LENGTH_KM)
                                    {
                                        ctlID--;
                                    }
                                    else
                                    {
                                        ctlID = IDS_LENGTH_M;
                                    }
                                }
                                break;
                                
                            case CONVERTER_MODE_WEIGHT:
                                {
                                    if(ctlID > IDS_WEIGHT_OZ)
                                    {
                                        ctlID--;
                                    }
                                    else
                                    {
                                        ctlID = IDS_WEIGHT_GRAIN;
                                    }
                                }
                                break;
                                
                            case CONVERTER_MODE_VOLUME:
                                {
                                    if(ctlID > IDS_VOLUME_CM3)
                                    {
                                        ctlID--;
                                    }
                                    else
                                    {
                                        ctlID = IDS_VOLUME_GAL;
                                    }
                                }
                                break;
                                
                            case CONVERTER_MODE_AREA:
                                {
                                    if(ctlID > IDS_AREA_CM2)
                                    {
                                        ctlID--;
                                    }
                                    else
                                    {
                                        ctlID = IDS_AREA_ACRE;
                                    }
                                }
                                break;
                                
                            case CONVERTER_MODE_TEMPERATURE:
                                {
                                    if(ctlID == IDS_TEMPERATURE_C)
                                    {
                                        ctlID = IDS_TEMPERATURE_F;
                                    }
                                    else
                                    {
                                        ctlID = IDS_TEMPERATURE_C;
                                    }
                                }
                                break;

                            default:
                                return TRUE;
                        }
                        if(pMe->m_nCtlID == IDC_UNIT_MENU1)
                        {
                            IMENUCTL_SetSel(pMe->pUnitMenu1, ctlID);
                        }
                        else
                        {
                            IMENUCTL_SetSel(pMe->pUnitMenu2, ctlID);
                        }
                        bRedraw = TRUE;
                        bCalc = TRUE;
                    }
                }
                    break;
                    
                default:
                    break;
            }
                
            if (bRedraw)
            {
                if(bCalc)
                {
                    double result;
                    (void) STRTOWSTR(m_inputNumber, wstrDisplay, sizeof(wstrDisplay));
                    if(pMe->m_nCtlID == IDC_NUMBER1 || pMe->m_nCtlID == IDC_UNIT_MENU1)
                    {
                        ITEXTCTL_SetText(pMe->pNumber1, wstrDisplay, -1);
                    }
                    else
                    {
                        ITEXTCTL_SetText(pMe->pNumber2, wstrDisplay, -1);
                    }
                    
                    if(STRLEN(m_inputNumber) == 0)
                    {
                        wstrDisplay[0] = '\0';
                    }
                    else
                    {
                        result = Converter_CalcResult(pMe);
                        if(pMe->b_overflow)
                        {
                            (void)ISHELL_LoadResString(pMe->m_pShell, 
                                                                AEE_CONVERTER_LANGFILE,
                                                                IDS_OVERFLOW, 
                                                                wstrDisplay,
                                                                sizeof(wstrDisplay));
                        }
                        else
                        {
                            (void)Converter_FloatToWstr(result, wstrDisplay, sizeof(wstrDisplay));
                        }
                    }
                    
                    if(pMe->m_nCtlID == IDC_NUMBER1 || pMe->m_nCtlID == IDC_UNIT_MENU1)
                    {
                        ITEXTCTL_SetText(pMe->pNumber2, wstrDisplay, -1);
                    }
                    else
                    {
                        ITEXTCTL_SetText(pMe->pNumber1, wstrDisplay, -1);
                    }
                }
                (void) ISHELL_PostEvent(pMe->m_pShell,
                                            AEECLSID_CONVERTER,
                                            EVT_USER_REDRAW,
                                            0,
                                            0);
            }
        }
        return TRUE;
		#ifdef FEATURE_LCD_TOUCH_ENABLE
        case  EVT_PEN_UP:
			{
				int16 wXPos = (int16)AEE_GET_X((const char *)dwParam);
				int16 wYPos = (int16)AEE_GET_Y((const char *)dwParam);
				AEERect bottomBarRect;
				//int ht;
				int nBarH ;                
				AEEDeviceInfo devinfo;
				nBarH = GetBottomBarHeight(pMe->m_pDisplay);
				
				MEMSET(&devinfo, 0, sizeof(devinfo));
				ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
				SETAEERECT(&bottomBarRect, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);
				MSG_FATAL("wXPos=====%d,wYPos=========%d",wXPos,wYPos,0);
				SETAEERECT(&p_Menu1, 8, 18, 224, 20); 
				SETAEERECT(&p_Menu2, 8, 80, 224, 20); 
				SETAEERECT(&p_Number1, 0, 45, 240, 30); 
				SETAEERECT(&p_Number2, 0, 110, 240, 30); 
				if( TOUCH_PT_IN_RECT(wXPos, wYPos, bottomBarRect))
				{
					if(wXPos >= bottomBarRect.x + (bottomBarRect.dx/3)*2 && wXPos < bottomBarRect.x + (bottomBarRect.dx/3)*3 )//右
					{						
						boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_CONVERTER,EVT_USER,AVK_CLR,0);
						return rt;
					}
					else if((wXPos >= bottomBarRect.x) && (wXPos < bottomBarRect.x + (bottomBarRect.dx/3)))//左
					{						
						
						boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_CONVERTER,EVT_USER,AVK_SELECT,0);
						return rt;
					}
				}
				MSG_FATAL("pL_rect1X=%d,y=%d,dy=%d",pMe->pL_rect1.x,pMe->pL_rect1.y,pMe->pL_rect1.dy);
				MSG_FATAL("pL_rect2X=%d,y=%d,dy=%d",pMe->pL_rect2.x,pMe->pL_rect2.y,pMe->pL_rect2.dy);
				MSG_FATAL("pR_rect1X=%d,y=%d,dy=%d",pMe->pR_rect1.x,pMe->pR_rect1.y,pMe->pR_rect1.dy);
				MSG_FATAL("pR_rect2X=%d,y=%d,dy=%d",pMe->pR_rect2.x,pMe->pR_rect2.y,pMe->pR_rect2.dy);
				if(TOUCH_PT_IN_RECT(wXPos, wYPos, pMe->pL_rect1))
				{
					if(pMe->m_nCtlID == IDC_UNIT_MENU1)
					{
						boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_CONVERTER,EVT_USER,AVK_LEFT,0);
						MSG_FATAL("IDC_UNIT_MENU1.....left",0,0,0);
						return rt;
					}
					break;
				}
				else if(TOUCH_PT_IN_RECT(wXPos, wYPos, pMe->pL_rect2))
				{
					if(pMe->m_nCtlID == IDC_UNIT_MENU2)
					{
						boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_CONVERTER,EVT_USER,AVK_LEFT,0);
						MSG_FATAL("IDC_UNIT_MENU2.....left",0,0,0);
						return rt;
					}
					break;
				}
				else if(TOUCH_PT_IN_RECT(wXPos, wYPos, pMe->pR_rect1))
				{
					if(pMe->m_nCtlID == IDC_UNIT_MENU1)
					{
						boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_CONVERTER,EVT_USER,AVK_RIGHT,0);
						MSG_FATAL("IDC_UNIT_MENU1.....right",0,0,0);
						return rt;
					}
					break;
				}
				else if(TOUCH_PT_IN_RECT(wXPos, wYPos, pMe->pR_rect2))
				{
					if(pMe->m_nCtlID == IDC_UNIT_MENU2)
					{
						boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_CONVERTER,EVT_USER,AVK_RIGHT,0);
						MSG_FATAL("IDC_UNIT_MENU2.....right",0,0,0);
						return rt;
					}
					break;
				}
				else if(TOUCH_PT_IN_RECT(wXPos, wYPos, p_Number1))
				{
					 MSG_FATAL("IDC_NUMBER1..........1",0,0,0);
                     if(pMe->m_nCtlID != IDC_NUMBER1)
                     {
					 pMe->m_nCtlID = IDC_NUMBER1;
					 (void) ISHELL_PostEvent(pMe->m_pShell,
                                            AEECLSID_CONVERTER,
                                            EVT_USER_REDRAW,
                                            0,
                                            0);
                     }
                     else
                     {
                     TSIM_NumberKeypad(TRUE);
                     pMe->PENUPbRedraw = TRUE;
                     pMe->PENUPbCalc = TRUE;
                     ITEXTCTL_SetMaxSize( pMe->pNumber1,MAX_INT_NUMBER);
					 return ITEXTCTL_HandleEvent(pMe->pNumber1,EVT_PEN_UP,wParam, dwParam);
                     }
				}
				else if(TOUCH_PT_IN_RECT(wXPos, wYPos, p_Number2))
				{
					 MSG_FATAL("IDC_NUMBER2..........2",0,0,0);
                     if(pMe->m_nCtlID != IDC_NUMBER2)
                     {
                      pMe->m_nCtlID = IDC_NUMBER2;
                      (void) ISHELL_PostEvent(pMe->m_pShell,
                                                 AEECLSID_CONVERTER,
                                                 EVT_USER_REDRAW,
                                                 0,
                                                 0);
                     }
                     else
                     {
                     TSIM_NumberKeypad(TRUE);
                     pMe->PENUPbRedraw = TRUE;
                     pMe->PENUPbCalc = TRUE; 
                     ITEXTCTL_SetMaxSize( pMe->pNumber2,MAX_INT_NUMBER);
					 return ITEXTCTL_HandleEvent(pMe->pNumber2,EVT_PEN_UP,wParam, dwParam);
                     }	 
                     
				}
				else if(TOUCH_PT_IN_RECT(wXPos, wYPos, p_Menu1))
				{
					MSG_FATAL("IDC_UNIT_MENU1..........1",0,0,0);
					pMe->m_nCtlID = IDC_UNIT_MENU1;
                    ITEXTCTL_GetText(pMe->pNumber1, wstrDisplay, sizeof(wstrDisplay));
                    nLen = WSTRLEN(wstrDisplay);
                    if(wstrDisplay[nLen - 1] == L'.')
                    {
                        wstrDisplay[nLen - 1] = L'\0';
                    }
                    ITEXTCTL_SetText(pMe->pNumber1, wstrDisplay, -1);
					(void) ISHELL_PostEvent(pMe->m_pShell,
                                            AEECLSID_CONVERTER,
                                            EVT_USER_REDRAW,
                                            0,
                                            0);
					return TRUE;
				}
				else if(TOUCH_PT_IN_RECT(wXPos, wYPos, p_Menu2))
				{
					MSG_FATAL("IDC_UNIT_MENU2..........2",0,0,0);
					pMe->m_nCtlID = IDC_UNIT_MENU2;
                    ITEXTCTL_GetText(pMe->pNumber2, wstrDisplay, sizeof(wstrDisplay));
                    nLen = WSTRLEN(wstrDisplay);
                    if(wstrDisplay[nLen - 1] == L'.')
                    {
                        wstrDisplay[nLen - 1] = L'\0';
                    }
                    ITEXTCTL_SetText(pMe->pNumber2, wstrDisplay, -1);
					(void) ISHELL_PostEvent(pMe->m_pShell,
                                            AEECLSID_CONVERTER,
                                            EVT_USER_REDRAW,
                                            0,
                                            0);
					return TRUE;
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
       Converter_ChangeCurrencyEvent
说明：
       IDD_CHANGECONURRENCY对话框事件处理函数
       
参数：
       pMe [in]：指向Converter Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean  Converter_ChangeCurrencyEvent(CConverter *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
    PARAM_NOT_REF(dwParam)
    static char     *m_inputNumber = NULL;
    static boolean bChangable;
    int                 nLen = 0;
    uint16            coeffNum1 = 7, coeffNum2 = 8, coeffNum3 = 9, basicCurrency = 10;
    uint16            currencymenu[] = {11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40};
    AECHAR          wstrDisplay[MAX_INPUT_NUMBER+3] = {0};
    ERR("%x, %x ,%x,Converter_ChangeCurrencyEvent",eCode,wParam,dwParam);
    
    if (NULL == pMe)
    {
        return FALSE;
    }
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
        {
            int i = 0;
            if(m_inputNumber == NULL)
            {
                m_inputNumber = (char *)MALLOC((MAX_INPUT_NUMBER + 3)* sizeof(char));
            }
            bChangable = FALSE;
            MSG_FATAL("m_inputNumber address=%x", m_inputNumber,0,0);
            for(i=0;i<CONVERTER_CURRENCY_NUMBER;i++)
            {
                (void)IMENUCTL_AddItem(pMe->title,
                                                AEE_CONVERTER_LANGFILE,
                                                IDS_CURRENCY_EUR + i,
                                                IDS_CURRENCY_EUR + i,
                                                NULL,
                                                0);
                (void)IMENUCTL_AddItem(pMe->currency,
                                                AEE_CONVERTER_LANGFILE,
                                                IDS_CURRENCY_EUR + i,
                                                currencymenu[2*i],
                                                NULL,
                                                0);
                (void)IMENUCTL_AddItem(pMe->currency,
                                                AEE_CONVERTER_LANGFILE,
                                                IDS_CURRENCY_EUR + i,
                                                currencymenu[2*i + 1],
                                                NULL,
                                                0);
            }
            pMe->m_nCtlID = basicCurrency;
            return TRUE;
        }
                     
        case EVT_DIALOG_START:
        {
            IMENUCTL_SetActive(pMe->currency, FALSE);
            IMENUCTL_SetSel(pMe->title, pMe->basiccoefficient);
            IMENUCTL_SetSel(pMe->currency, currencymenu[0]);
            (void) ISHELL_PostEvent(pMe->m_pShell,
                                    AEECLSID_CONVERTER,
                                    EVT_USER_REDRAW,
                                    NULL,
                                    NULL);
            return TRUE;
        }
            
        case EVT_USER_REDRAW:
        {
            double                 source = 0.0;
            AECHAR                      text[32] = {0};
            TitleBar_Param_type     TBarParam = {0};
            BottomBar_Param_type  BBarParam ={0};
            IImage                       *Image;
            uint16                        coefficient1, coefficient2, coefficient3;
            uint16                        uint = 0;
            AEERect                       rect;
            AEEImageInfo                  iInfo;
            //先清屏
            Appscommon_ResetBackgroundEx(pMe->m_pDisplay, &pMe->m_rc, FALSE);
            // 画标题
            (void)ISHELL_LoadResString(pMe->m_pShell, 
                                                AEE_CONVERTER_LANGFILE,
                                                IDS_CHANGE_TITLE, 
                                                text,
                                                sizeof(text));
            TBarParam.pwszTitle = text;
            TBarParam.dwAlignFlags = IDF_ALIGN_MIDDLE | IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE;
			#if 0
            DrawTitleBar(pMe->m_pDisplay, &TBarParam);
			#else
			IANNUNCIATOR_SetFieldTextEx(pMe->m_pIAnn,text,FALSE);
			#endif
            //画箭头
            Image = ISHELL_LoadResImage(pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDB_LEFTARROW);
			IImage_GetInfo(Image,&iInfo);
            IIMAGE_Draw(Image, pMe->m_rc.x, pMe->m_rc.y + TITLEBAR_HEIGHT/2 + (pMe->dyMenu - ARROW_HEIGHT)/2);
            if(Image)
            {
                IIMAGE_Release(Image);
                Image = NULL;
            }
            #ifdef FEATURE_LCD_TOUCH_ENABLE
			SETAEERECT(&pMe->pL_rect3, pMe->m_rc.x, pMe->m_rc.y + TITLEBAR_HEIGHT/2 + (pMe->dyMenu - ARROW_HEIGHT)/2, iInfo.cx+10, iInfo.cy); 
            #endif
            Image = ISHELL_LoadResImage(pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDB_RIGHTARROW);
            MSG_FATAL("Image address=%x", Image,0,0);
            IIMAGE_Draw(Image, pMe->m_rc.dx - ARROW_WIDTH, pMe->m_rc.y + TITLEBAR_HEIGHT/2 + (pMe->dyMenu - ARROW_HEIGHT)/2);
            if(Image)
            {
                IIMAGE_Release(Image);
                Image = NULL;
            }
            #ifdef FEATURE_LCD_TOUCH_ENABLE
			SETAEERECT(&pMe->pR_rect3, pMe->m_rc.dx - ARROW_WIDTH-10, pMe->m_rc.y + TITLEBAR_HEIGHT/2 + (pMe->dyMenu - ARROW_HEIGHT)/2, iInfo.cx+10, iInfo.cy); 
            #endif
            //画底条
            nLen = STRLEN(m_inputNumber);
            if((pMe->m_nCtlID == basicCurrency) ||
                ((IMENUCTL_GetSel(pMe->currency) - currencymenu[0])/2 + IDS_CURRENCY_EUR == pMe->basiccoefficient))
            {
                BBarParam.eBBarType = BTBAR_BACK;
            }
            else if(bChangable == FALSE)
            {
                BBarParam.eBBarType = BTBAR_EDIT_BACK;
            }
            else if(nLen <= 0)
            {
                BBarParam.eBBarType = BTBAR_BACK;
            }
            else
            {
            	#ifdef FEATURE_ALL_KEY_PAD
                BBarParam.eBBarType = BTBAR_OK_BACK;
                #else
                BBarParam.eBBarType = BTBAR_OK_DELETE;
                #endif
            }
            
            //更新高亮显示
            SETAEERECT(&rect, 
                                pMe->m_rc.x, 
                                pMe->m_rc.y + TITLEBAR_HEIGHT/2, 
                                pMe->m_rc.dx, 
                                pMe->dyMenu);
            if(pMe->m_nCtlID == basicCurrency)
            {
                rect.x  += ARROW_WIDTH;
                rect.dx -= (2*ARROW_WIDTH);
            }
            else
            {
                rect.dx -= SCROLLBAR_WIDTH;
                rect.y += (2*(pMe->m_nCtlID - coeffNum1 + 1)*pMe->dyMenu);
            }
            //更新汇率显示
            if(bChangable == FALSE)
            {
                uint = IMENUCTL_GetSel(pMe->currency);
                if(pMe->m_nCtlID == basicCurrency)
                {
                    coefficient1 = currencymenu[0];
                    coefficient2 = currencymenu[0] + 1;
                    #if defined(FEATURE_VERSION_S1000T)|| defined(FEATURE_VERSION_W515V3)|| defined(FEATURE_VERSION_W516) || defined(FEATURE_VERSION_W027)|| defined(FEATURE_VERSION_C310)
    				#else
                    coefficient3 = currencymenu[0] + 2;
                    #endif
                }
                else
                {
                    coefficient1 = (uint + currencymenu[0])/2 + coeffNum1 - pMe->m_nCtlID;
                    coefficient2 = (uint + currencymenu[0])/2 + coeffNum2 - pMe->m_nCtlID;
                    #if defined(FEATURE_VERSION_S1000T)|| defined(FEATURE_VERSION_W515V3)|| defined(FEATURE_VERSION_W516) || defined(FEATURE_VERSION_W027)|| defined(FEATURE_VERSION_C310)
    				#else
                    coefficient3 = (uint + currencymenu[0])/2 + coeffNum3 - pMe->m_nCtlID;
                    #endif
                }
                
                source = pMe->m_CurrencyCFG.customCurrency[coefficient1 - currencymenu[0]] / pMe->m_CurrencyCFG.customCurrency[pMe->basiccoefficient - IDS_CURRENCY_EUR];
                (void)Converter_FloatToWstr(source, wstrDisplay, sizeof(wstrDisplay));
                ITEXTCTL_SetText(pMe->coeff1, wstrDisplay, -1);
                source = pMe->m_CurrencyCFG.customCurrency[coefficient2 - currencymenu[0]] / pMe->m_CurrencyCFG.customCurrency[pMe->basiccoefficient - IDS_CURRENCY_EUR];
                (void)Converter_FloatToWstr(source, wstrDisplay, sizeof(wstrDisplay));
                ITEXTCTL_SetText(pMe->coeff2, wstrDisplay, -1);
                
                #if defined(FEATURE_VERSION_S1000T)|| defined(FEATURE_VERSION_W515V3)|| defined(FEATURE_VERSION_W516) || defined(FEATURE_VERSION_W027)|| defined(FEATURE_VERSION_C310)
    			#else
                source = pMe->m_CurrencyCFG.customCurrency[coefficient3 - currencymenu[0]] / pMe->m_CurrencyCFG.customCurrency[pMe->basiccoefficient - IDS_CURRENCY_EUR];
                (void)Converter_FloatToWstr(source, wstrDisplay, sizeof(wstrDisplay));
                ITEXTCTL_SetText(pMe->coeff3, wstrDisplay, -1);
                #endif
            }
            (void)IMENUCTL_Redraw(pMe->currency);
            IDISPLAY_FillRect(pMe->m_pDisplay, &rect, RGB_ORANGE);
            if(bChangable == FALSE)
            {
                ITEXTCTL_SetActive(pMe->coeff1, FALSE);
                ITEXTCTL_SetActive(pMe->coeff2, FALSE);
                #if defined(FEATURE_VERSION_S1000T)|| defined(FEATURE_VERSION_W515V3)|| defined(FEATURE_VERSION_W516)|| defined(FEATURE_VERSION_W027)|| defined(FEATURE_VERSION_C310)
    			#else
                ITEXTCTL_SetActive(pMe->coeff3, FALSE);
                #endif
            }
            else
            {
                (void)ITEXTCTL_Redraw(pMe->coeff1);
                (void)ITEXTCTL_Redraw(pMe->coeff2);
                #if defined(FEATURE_VERSION_S1000T)|| defined(FEATURE_VERSION_W515V3)|| defined(FEATURE_VERSION_W516)|| defined(FEATURE_VERSION_W027)|| defined(FEATURE_VERSION_C310)
    			#else
                (void)ITEXTCTL_Redraw(pMe->coeff3);
                #endif
            }
            (void)IMENUCTL_Redraw(pMe->title);
            DrawBottomBar(pMe->m_pDisplay, &BBarParam);
            IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
                        
            return TRUE;
        }
            
        case EVT_DIALOG_END:
        {
            if (NULL != m_inputNumber)
            {
                MSG_FATAL("m_inputNumber address=%x", m_inputNumber,0,0);
                FREE(m_inputNumber);
                m_inputNumber = NULL;
            }
            return TRUE;
        }

		//Add By zzg 2012_03_01
		case EVT_KEY_HELD:
		{
			if (AVK_DEL == wParam)
			{
				if(pMe->m_nCtlID == coeffNum1)
                {
                    ITEXTCTL_Reset(pMe->coeff1);
                }
                if(pMe->m_nCtlID == coeffNum2)
                {
                    ITEXTCTL_Reset(pMe->coeff2);
                }		

				(void) ISHELL_PostEvent(pMe->m_pShell,
                                            AEECLSID_CONVERTER,
                                            EVT_USER_REDRAW,
                                            0,
                                            0);
			}
			return TRUE;
		}
		//Add End
        
        case EVT_KEY:
        {
            char     chEnter  = 0;
            boolean bRedraw = FALSE;
            uint16   ctlID = 0;

			MSG_FATAL("***zzg EVT_KEY wParam=%x***", wParam,0,0);
                
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
                {
                    if((pMe->m_nCtlID != basicCurrency) && (bChangable == TRUE))
                    {
                        bRedraw = TRUE;
                        chEnter = '0' + (wParam - AVK_0);
                        
                        Converter_AddChar(pMe, chEnter, m_inputNumber);
                        
                        (void) STRTOWSTR(m_inputNumber, wstrDisplay, sizeof(wstrDisplay));
                        if(pMe->m_nCtlID == coeffNum1)
                        {
                            ITEXTCTL_SetText(pMe->coeff1, wstrDisplay, -1);
                        }
                        if(pMe->m_nCtlID == coeffNum2)
                        {
                            ITEXTCTL_SetText(pMe->coeff2, wstrDisplay, -1);
                        }
                        #if defined(FEATURE_VERSION_S1000T)|| defined(FEATURE_VERSION_W515V3)|| defined(FEATURE_VERSION_W516)|| defined(FEATURE_VERSION_W027)|| defined(FEATURE_VERSION_C310)
    					#else
                        if(pMe->m_nCtlID == coeffNum3)
                        {
                            ITEXTCTL_SetText(pMe->coeff3, wstrDisplay, -1);
                        }
                        #endif
                    }
                }
                    break;

				//Add By zzg 2012_03_02
				case AVK_DEL:
				{
					chEnter = 0;
                    nLen = STRLEN(m_inputNumber);
                    
                    if (nLen == 0 ||pMe->m_nCtlID == basicCurrency ||bChangable == FALSE)
                    {
                        CLOSE_DIALOG(DLGRET_CANCELED)
                        return TRUE;
                    }
                    else
                    {
                        bRedraw = TRUE;
                        m_inputNumber[nLen -1] = chEnter;
                        (void) STRTOWSTR(m_inputNumber, wstrDisplay, sizeof(wstrDisplay));
                        if(pMe->m_nCtlID == coeffNum1)
                        {
                            ITEXTCTL_SetText(pMe->coeff1, wstrDisplay, -1);
                        }
                        if(pMe->m_nCtlID == coeffNum2)
                        {
                            ITEXTCTL_SetText(pMe->coeff2, wstrDisplay, -1);
                        }
                        #if defined(FEATURE_VERSION_S1000T)|| defined(FEATURE_VERSION_W515V3)|| defined(FEATURE_VERSION_W516)|| defined(FEATURE_VERSION_W027)|| defined(FEATURE_VERSION_C310)
						#else
                        if(pMe->m_nCtlID == coeffNum3)
                        {
                            ITEXTCTL_SetText(pMe->coeff3, wstrDisplay, -1);
                        }
                        #endif
                    }
                                      
					break;
				}
				//Add End

				case AVK_M:			//Add By zzg 2012_03_01
                case AVK_STAR:
                {
                    if((pMe->m_nCtlID != basicCurrency) && (bChangable == TRUE))
                    {
                        bRedraw = TRUE;
                        chEnter = '.';
                        
                        Converter_AddChar(pMe, chEnter, m_inputNumber);
                        
                        (void) STRTOWSTR(m_inputNumber, wstrDisplay, sizeof(wstrDisplay));
                        if(pMe->m_nCtlID == coeffNum1)
                        {
                            ITEXTCTL_SetText(pMe->coeff1, wstrDisplay, -1);
                        }
                        if(pMe->m_nCtlID == coeffNum2)
                        {
                            ITEXTCTL_SetText(pMe->coeff2, wstrDisplay, -1);
                        }
                        #if defined(FEATURE_VERSION_S1000T)|| defined(FEATURE_VERSION_W515V3)|| defined(FEATURE_VERSION_W516)|| defined(FEATURE_VERSION_W027)|| defined(FEATURE_VERSION_C310)
    					#else
                        if(pMe->m_nCtlID == coeffNum3)
                        {
                            ITEXTCTL_SetText(pMe->coeff3, wstrDisplay, -1);
                        }
                        #endif
                    }
                }
                    break;

#ifdef FEATURE_RESET_CURRENCY
                /*set backdoor to reset currency to default in case of chaos*/
                case AVK_POUND:
                {
                    if(STRCMP(m_inputNumber, "12101210") == 0)
                    {
                        int i = 0;

                        for(i = 0; i < CONVERTER_CURRENCY_NUMBER; i++)
                        {
                            pMe->m_CurrencyCFG.customCurrency[i] = coefficient[i];
                        }
                        (void) ISHELL_SetPrefs(pMe->m_pShell,
                                                            AEECLSID_CONVERTER,
                                                            CONVERTER_CFG_CURRENCY,
                                                            &pMe->m_CurrencyCFG,
                                                            sizeof(ConverterCurrencyCfg));
                        CLOSE_DIALOG(DLGRET_OK)
                    }
                }
                return TRUE;
#endif

                case AVK_SELECT:
                case AVK_INFO:
                {
                    if(pMe->m_nCtlID != basicCurrency)
                    {
                        if(bChangable == TRUE)
                        {
                            if(pMe->m_nCtlID == coeffNum1)
                            {
                                ITEXTCTL_GetText(pMe->coeff1, wstrDisplay, sizeof(wstrDisplay));
                            }
                            else if(pMe->m_nCtlID == coeffNum2)
                            {
                                ITEXTCTL_GetText(pMe->coeff2, wstrDisplay, sizeof(wstrDisplay));
                            }
                            #if defined(FEATURE_VERSION_S1000T)|| defined(FEATURE_VERSION_W515V3)|| defined(FEATURE_VERSION_W516)|| defined(FEATURE_VERSION_W027)|| defined(FEATURE_VERSION_C310)
    						#else
                            else
                            {
                               ITEXTCTL_GetText(pMe->coeff3, wstrDisplay, sizeof(wstrDisplay));
                            }
							#endif
                            if(WSTRLEN(wstrDisplay) > 0)
                            {
                                if(WSTRTOFLOAT(wstrDisplay) <= 0.0)
                                {
                                    CLOSE_DIALOG(DLGRET_ERR)
                                }
                                else
                                {
                                    ctlID = (IMENUCTL_GetSel(pMe->currency) - currencymenu[0])/2;
                                    /*EUR as the standard currency cannot be changed!*/
                                    if(ctlID == 0)
                                    {
                                        pMe->m_CurrencyCFG.customCurrency[pMe->basiccoefficient - IDS_CURRENCY_EUR] = FDIV(pMe->m_CurrencyCFG.customCurrency[ctlID], (WSTRTOFLOAT(wstrDisplay)));
                                    }
                                    else
                                    {
                                        pMe->m_CurrencyCFG.customCurrency[ctlID] = FDIV((WSTRTOFLOAT(wstrDisplay)), pMe->m_CurrencyCFG.customCurrency[pMe->basiccoefficient - IDS_CURRENCY_EUR]);
                                    }
                                    (void) ISHELL_SetPrefs(pMe->m_pShell,
                                                                        AEECLSID_CONVERTER,
                                                                        CONVERTER_CFG_CURRENCY,
                                                                        &pMe->m_CurrencyCFG,
                                                                        sizeof(ConverterCurrencyCfg));
                                    CLOSE_DIALOG(DLGRET_OK)
                                }
                            }
                            return TRUE;
                        }
                        else
                        {
                            ctlID = IMENUCTL_GetSel(pMe->currency);
                            if((ctlID - currencymenu[0])/2 + IDS_CURRENCY_EUR != pMe->basiccoefficient)
                            {
                                BottomBar_Param_type  BBarParam ={0};
                                #ifdef FEATURE_ALL_KEY_PAD
				                BBarParam.eBBarType = BTBAR_OK_BACK;
				                #else
				                BBarParam.eBBarType = BTBAR_OK_DELETE;
				                #endif
                                bChangable = TRUE;
                                if(pMe->m_nCtlID == coeffNum1)
                                {
                                    ITEXTCTL_GetText(pMe->coeff1, wstrDisplay, sizeof(wstrDisplay));
                                    ITEXTCTL_SetActive(pMe->coeff1, TRUE);
                                }
                                else if(pMe->m_nCtlID == coeffNum2)
                                {
                                    ITEXTCTL_GetText(pMe->coeff2, wstrDisplay, sizeof(wstrDisplay));
                                    ITEXTCTL_SetActive(pMe->coeff2, TRUE);
                                }
                                #if defined(FEATURE_VERSION_S1000T)|| defined(FEATURE_VERSION_W515V3)|| defined(FEATURE_VERSION_W516)|| defined(FEATURE_VERSION_W027)|| defined(FEATURE_VERSION_C310)
    							#else
                                else
                                {
                                    ITEXTCTL_GetText(pMe->coeff3, wstrDisplay, sizeof(wstrDisplay));
                                    ITEXTCTL_SetActive(pMe->coeff3, TRUE);
                                }
                                #endif
                                nLen = WSTRLEN(wstrDisplay);
                                (void) WSTRTOSTR(wstrDisplay, m_inputNumber, (MAX_INPUT_NUMBER + 3)*sizeof(char));
                                m_inputNumber[nLen] = 0;
                                DrawBottomBar(pMe->m_pDisplay, &BBarParam);
                                IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
                            }
                        }
                    }
                }
                    break;

                case AVK_CLR:
                {
                    chEnter = 0;
                    nLen = STRLEN(m_inputNumber);
                    #ifdef FEATURE_ALL_KEY_PAD
                    if(dwParam ==1)
                    #else
                    if(TRUE)
                    #endif
                    {
	                    if (nLen == 0 ||pMe->m_nCtlID == basicCurrency ||bChangable == FALSE)
	                    {
	                        CLOSE_DIALOG(DLGRET_CANCELED)
	                        return TRUE;
	                    }
	                    else
	                    {
	                        bRedraw = TRUE;
	                        m_inputNumber[nLen -1] = chEnter;
	                        (void) STRTOWSTR(m_inputNumber, wstrDisplay, sizeof(wstrDisplay));
	                        if(pMe->m_nCtlID == coeffNum1)
	                        {
	                            ITEXTCTL_SetText(pMe->coeff1, wstrDisplay, -1);
	                        }
	                        if(pMe->m_nCtlID == coeffNum2)
	                        {
	                            ITEXTCTL_SetText(pMe->coeff2, wstrDisplay, -1);
	                        }
	                        #if defined(FEATURE_VERSION_S1000T)|| defined(FEATURE_VERSION_W515V3)|| defined(FEATURE_VERSION_W516)|| defined(FEATURE_VERSION_W027)|| defined(FEATURE_VERSION_C310)
    						#else
	                        if(pMe->m_nCtlID == coeffNum3)
	                        {
	                            ITEXTCTL_SetText(pMe->coeff3, wstrDisplay, -1);
	                        }
	                        #endif
	                    }
                    }
                    else
                    {
                    	CLOSE_DIALOG(DLGRET_CANCELED)
                    	return TRUE;
                    }
                }
                    break;
                        
                case AVK_DOWN:
                {
                    bRedraw = TRUE;
                    bChangable = FALSE;
                    
                    if(pMe->m_nCtlID == basicCurrency)
                    {
                        pMe->m_nCtlID = coeffNum1;
                        ctlID = currencymenu[0];
                        IMENUCTL_SetSel(pMe->currency, ctlID);
                        IMENUCTL_SetActive(pMe->currency, TRUE);
                    }
                    else
                    {
                        ctlID = IMENUCTL_GetSel(pMe->currency);
                        if(ctlID<= currencymenu[26])
                        {
                            ctlID += 2;
                            IMENUCTL_SetSel(pMe->currency, ctlID + 1);
                            IMENUCTL_SetSel(pMe->currency, ctlID);
                            if(pMe->m_nCtlID <= coeffNum1)
                            {
                                pMe->m_nCtlID++;
                            }
                        }
                        else
                        {
                            IMENUCTL_SetSel(pMe->currency, currencymenu[0]);
                            IMENUCTL_SetActive(pMe->currency, FALSE);
                            pMe->m_nCtlID = basicCurrency;
                        }
                    }
                }
                    break;
                    
                case AVK_UP:
                {
                    bRedraw = TRUE;
                    bChangable = FALSE;
                    
                    if(pMe->m_nCtlID == basicCurrency)
                    {
                        pMe->m_nCtlID = coeffNum2;
                        ctlID = currencymenu[28];
                        IMENUCTL_SetSel(pMe->currency, ctlID + 1);
                        IMENUCTL_SetSel(pMe->currency, ctlID);
                        IMENUCTL_SetActive(pMe->currency, TRUE);
                    }
                    else
                    {
                        ctlID = IMENUCTL_GetSel(pMe->currency);
                        if(ctlID >= currencymenu[2])
                        {
                            ctlID -= 2;
                            IMENUCTL_SetSel(pMe->currency, ctlID);
                            if(pMe->m_nCtlID > coeffNum1)
                            {
                                pMe->m_nCtlID--;
                            }
                        }
                        else
                        {
                            IMENUCTL_SetActive(pMe->currency, FALSE);
                            pMe->m_nCtlID = basicCurrency;
                        }
                    }
                } 
                    break;
                        
                case AVK_RIGHT:
                {
                    if(pMe->m_nCtlID == basicCurrency)
                    {
                        ctlID = IMENUCTL_GetSel(pMe->title);
                        if(ctlID < IDS_CURRENCY_ILS)
                        {
                            ctlID++;
                        }
                        else
                        {
                            ctlID = IDS_CURRENCY_EUR;
                        }
                        IMENUCTL_SetSel(pMe->title, ctlID);
                        pMe->basiccoefficient = ctlID;
                        bRedraw = TRUE;
                    }
                }
                    break;
                    
                case AVK_LEFT:
                {
                    if(pMe->m_nCtlID == basicCurrency)
                    {
                        ctlID = IMENUCTL_GetSel(pMe->title);
                        if(ctlID > IDS_CURRENCY_EUR)
                        {
                            ctlID--;
                        }
                        else
                        {
                            ctlID = IDS_CURRENCY_ILS;
                        }
                        IMENUCTL_SetSel(pMe->title, ctlID);
                        pMe->basiccoefficient = ctlID;
                        bRedraw = TRUE;
                    }
                }
                    break;
                    
                default:
                    break;
            }
                
            if (bRedraw)
            {
                (void) ISHELL_PostEvent(pMe->m_pShell,
                                            AEECLSID_CONVERTER,
                                            EVT_USER_REDRAW,
                                            0,
                                            0);
            }
        }
            return TRUE;
        #ifdef FEATURE_LCD_TOUCH_ENABLE
        case  EVT_PEN_UP:
			{
				int16 wXPos = (int16)AEE_GET_X((const char *)dwParam);
				int16 wYPos = (int16)AEE_GET_Y((const char *)dwParam);
				AEERect bottomBarRect;
				//int ht;
				int nBarH ;
				AEEDeviceInfo devinfo;
				nBarH = GetBottomBarHeight(pMe->m_pDisplay);
				
				MEMSET(&devinfo, 0, sizeof(devinfo));
				ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
				SETAEERECT(&bottomBarRect, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);
				MSG_FATAL("wXPos=====%d,wYPos=========%d",wXPos,wYPos,0);
				if( TOUCH_PT_IN_RECT(wXPos, wYPos, bottomBarRect) )
				{
					if(wXPos >= bottomBarRect.x + (bottomBarRect.dx/3)*2 && wXPos < bottomBarRect.x + (bottomBarRect.dx/3)*3 )//右
					{						
						boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_CONVERTER,EVT_USER,AVK_CLR,0);
						return rt;
					}
					else if((wXPos >= bottomBarRect.x) && (wXPos < bottomBarRect.x + (bottomBarRect.dx/3)))//左
					{						
						
						boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_CONVERTER,EVT_USER,AVK_SELECT,0);
						return rt;
					}
				}
				if(TOUCH_PT_IN_RECT(wXPos, wYPos, pMe->pL_rect3))
				{
					boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_CONVERTER,EVT_USER,AVK_LEFT,0);
					return rt;
				}
				else if(TOUCH_PT_IN_RECT(wXPos, wYPos, pMe->pR_rect3))
				{
					boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_CONVERTER,EVT_USER,AVK_RIGHT,0);
					return rt;
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
       Converter_CalcResult
说明：
       计算单位换算结果。
       
参数：
       pMe [in]：指向Converter Applet对象结构的指针。
       
返回值：
       换算结果result 。
       
备注：
       
==============================================================================*/
static double  Converter_CalcResult(CConverter *pMe)
{
    uint16 uint1, uint2;
    double source = 0.0,result = 0.0;
    AECHAR buf[MAX_INPUT_NUMBER + 3] = {0};

    pMe->b_overflow = FALSE;
    if(pMe->m_nCtlID == IDC_NUMBER1 || pMe->m_nCtlID == IDC_UNIT_MENU1)
    {
        uint1 = IMENUCTL_GetSel(pMe->pUnitMenu1);
        uint2 = IMENUCTL_GetSel(pMe->pUnitMenu2);
        ITEXTCTL_GetText(pMe->pNumber1, buf, sizeof(buf));
    }
    else
    {
        uint1 = IMENUCTL_GetSel(pMe->pUnitMenu2);
        uint2 = IMENUCTL_GetSel(pMe->pUnitMenu1);
        ITEXTCTL_GetText(pMe->pNumber2, buf, sizeof(buf));
    }
    
    source = WSTRTOFLOAT(buf);
    //CURRENCY
    if(pMe->m_converterMode == CONVERTER_MODE_CURRENCY)
    {
        result = FMUL(source, FDIV(pMe->m_CurrencyCFG.customCurrency[uint2 - IDS_CURRENCY_EUR], pMe->m_CurrencyCFG.customCurrency[uint1 - IDS_CURRENCY_EUR]));
    }
    //TEMPERATURE
    else if(pMe->m_converterMode == CONVERTER_MODE_TEMPERATURE)
    {
        if(uint1 == uint2)
        {
            result = source;
            if(uint1 == IDS_TEMPERATURE_C)
            {
                if(source < (0 - 273.15))
                {
                    pMe->b_overflow = TRUE;
                }
            }
            else
            {
                if(source < ((0 - 273.15)*1.4+32))
                {
                    pMe->b_overflow = TRUE;
                }
            }
        }
        else
        {
            if(uint1 == IDS_TEMPERATURE_C)
            {
                if(source < (0 - 273.15))
                {
                    pMe->b_overflow = TRUE;
                }
                else
                {
                    result = FMUL(source, FDIV(coefficient[uint2 - IDS_CURRENCY_EUR], coefficient[uint1 - IDS_CURRENCY_EUR]));
                    result = FADD(result, 32);
                }
            }
            else
            {
                source = FSUB(source, 32);
                result =  FMUL(source, FDIV(coefficient[uint2 - IDS_CURRENCY_EUR], coefficient[uint1 - IDS_CURRENCY_EUR]));
                if(result < (0 - 273.15))
                {
                    pMe->b_overflow = TRUE;
                }
            }
        }
    }
    //LENGTH
    //WEIGHT
    //VOLUME
    //AREA
    else
    {
        result = FMUL(source, FDIV(coefficient[uint2 - IDS_CURRENCY_EUR], coefficient[uint1 - IDS_CURRENCY_EUR]));
    }
   
    if(result >= 1000000000.0)
    {
        pMe->b_overflow = TRUE;
    }

    return result;
}

/*==============================================================================
函数：
       Converter_FloatToWstr
说明：
       处理换算值后的浮点数显示。
       
参数：
       source [in]：源数字。
       pdest [in]：输出位置。
       nSize [in]：输出缓冲区大小。
       
返回值：
       NONE
       
备注：
       
==============================================================================*/
static boolean Converter_FloatToWstr(double source, AECHAR * pdest, int nSize)
{
    char szBuff[32];
    AECHAR ch, *psz, *pszLastZero, *pszSrc, *pszDst;
    boolean bDot;

    if (!pdest || nSize <= 0)
    {
        return FALSE;
    }
    
    MEMSET(pdest, 0, nSize);

    (void)snprintf(szBuff, 32,"%15.6f", source);
    (void)STRTOWSTR(szBuff, pdest, nSize);

    // Clip leading spaces...
    for (pszDst = pszSrc = pdest; (ch = *pszSrc) == ' '; pszSrc++)
            ;

    if (pszSrc != pszDst)
    {
        while (1)
        { //lint !e716
            ch = *pszSrc;
            *pszDst = ch;
            if (!ch)
            {
                break;
            }
            pszDst++;
            pszSrc++;
        }
    }

    // Clip trailing zeros...
    for (bDot = FALSE, pszLastZero = NULL, psz = pdest; (ch = *psz) != 0; psz++)
    {
        if (ch == (AECHAR)'.')
        {
            bDot = TRUE;
        }
        else
        {
            if (bDot)
            {
                if (ch == (AECHAR)'0')
                {
                    if (!pszLastZero)
                    {
                        pszLastZero = psz;
                    }
                }
                else
                {
                    pszLastZero = NULL;
                }
            }
        }
    }

    // If the trailing zero followed the decimal point, clip them both...
    if (pszLastZero)
    {
        if (pszLastZero[-1] == (AECHAR)'.')
        {
            pszLastZero--;
        }
        *pszLastZero = 0;
    }

    //If the trailing dot exists, clip it
    if(psz[-1] == (AECHAR)'.')
    {
        psz--;
        *psz = 0;
    }
    
    return TRUE;
}

/*==============================================================================
函数:
       Converter_DialogTimeout

说明:
       处理输入数字变更。

参数:
       pme [in]：指向Converter Applet对象结构的指针。
       chAdd [in]：添加的字符。
       pDest [in]：输入数字缓存区指针。

返回值:
       无。

备注:

==============================================================================*/
static void Converter_AddChar(CConverter *pMe, char chAdd, char *pDest)
{
    int i = 0, nIntLen = 0, nTailLen = 0;
    char *pstr = NULL;
    boolean bDot = FALSE;
    
    if(IS_NOT_NUMBER(*pDest))
    {
        MEMSET(pDest, 0, (MAX_INPUT_NUMBER + 3)*sizeof(char));
    }
    
    for(pstr = pDest, i = 0; *pstr != 0; i++, pstr++)
    {
        if(*pstr == '.')
        {
            bDot = TRUE;
            break;
        }
    }
    nIntLen = (*pDest == '-')? (i - 1):(i);
    if(TRUE == bDot)
    {
        for (nTailLen = i; *pstr != 0; i++, pstr++)
        {
            ;
        }
        nTailLen = i - nTailLen - 1;
    }
    
    if(chAdd == '.')
    {
        if(nIntLen == 0)
        {
            if(*pDest == 0)
            {
                *pDest = '0';
                pstr++;
            }
            else if(*pDest == '-')
            {
                pDest[1] = '0';
                pstr++;
            }
        }
        else if(bDot)
        {
            return;
        }
    }
    else if(chAdd == '-')
    {
        if(nIntLen == 0)
        {
            if(*pDest == 0)
            {
                *pDest = '-';
                pDest[1] = 0;
            }
            else if(*pDest == '-')
            {
                *pDest = 0;
            }
        }
        return;
    }
    else if((nIntLen == 1) && (!bDot) && (*pDest == '0'))
    {
        *pDest = 0;
        pstr--;
    }
    else if (((TRUE != bDot) && (nIntLen >= MAX_INT_NUMBER) && (chAdd != '.')) || 
            ((TRUE == bDot) && (nTailLen >= MAX_DECIMAL_NUMBER)))
    {
        return;
    }
    
    *pstr = chAdd;
    pstr[1] = 0;
}

/*==============================================================================
函数:
       Converter_GetTextToInputNum

说明:
       从TextCtl获取字符串并传递到输入数字缓存区。

参数:
       pme [in]：指向Converter Applet对象结构的指针。
       pwstrTemp [in]：临时存放获取出来的。
       pDest [in]：输入数字缓存区指针。

返回值:
       无。

备注:

==============================================================================*/
static void Converter_GetTextToInputNum(CConverter *pMe, AECHAR *pwstrTemp, char *pDest)
{
    if(pMe->m_nCtlID == IDC_NUMBER1)
    {
        ITEXTCTL_GetText(pMe->pNumber1, pwstrTemp, (MAX_INPUT_NUMBER + 3)*sizeof(char));
    }
    else
    {
        ITEXTCTL_GetText(pMe->pNumber2, pwstrTemp, (MAX_INPUT_NUMBER + 3)*sizeof(char));
    }
    (void) WSTRTOSTR(pwstrTemp, pDest, (MAX_INPUT_NUMBER + 3)*sizeof(char));
}

/*==============================================================================
函数:
       Converter_DialogTimeout

说明:
       定时器回调函数。主要用于自动关闭消息对话框。

参数:
       pme [in]：void *类型指针，暗指CConverter结构指针。

返回值:
       无。

备注:

==============================================================================*/
static void Converter_DialogTimeout(void *pme)
{
    CConverter *pMe = (CConverter *)pme;

    if (NULL == pMe)
    {
        return;
    }

    (void) ISHELL_PostEvent(pMe->m_pShell,
                            AEECLSID_CONVERTER,
                            EVT_DISPLAYDIALOGTIMEOUT,
                            0,
                            0);
}


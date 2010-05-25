#ifndef _CUSTSERVICE_DIALOG_HANDLER_
#define _CUSTSERVICE_DIALOG_HANDLER_

#include "CustserviceAppProt.h"
#include "AEECM.h"

#ifdef FEATURE_SID_LOCK
#include "OEMNV.h"
#endif

#include "OEMRTC.h"

#ifdef FEATURE_COLOR_DISPLAY
#include "appscommon_color.brh"
#else
#include "appscommon_momo.brh"
#endif
#include "appscommon.h" 


#define CLOSE_DIALOG(DlgRet)  {    \
                                     pMe->m_eDlgRet = DlgRet; \
                                     (void) ISHELL_EndDialog(pMe->m_pShell);  \
                              }

static boolean HandleMainDialogEvent(CCustServiceMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

static boolean  HandleCustHotlineDialogEvent(CCustServiceMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

static boolean  HandleMobileEHallDialogEvent(CCustServiceMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

static boolean  HandleCallSecretaryDialogEvent(CCustServiceMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

static boolean  HandleMySubscriptionDialogEvent(CCustServiceMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

static boolean  HandlePhoneSerGuideDialogEvent(CCustServiceMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

static boolean  HandleMessegeBoxDialogEvent(CCustServiceMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
    );


static void CustService_DialogTimeout(void *pme);


/*==============================================================================
                                 oˉêy?¨ò?
==============================================================================*/


/*==============================================================================
oˉêy:
       SportSApp_ShowDialog
?μ?÷:
       oˉêyóé×′ì?′|àíoˉêyó?à′′′?¨?àó|???°?ò×ê?′IDμ????°?ò?￡

2?êy:
       pMe [In]: ???òCSportsMenu Applet???ó?á11μ?????,???á11°üo?D?3ìDòμ?ì??¨D??￠.
       dlgResId [in]￡o???°?ò×ê?′ID￡?oˉêy?ù?Y??ID′′?¨???°?ò?￡

·μ???μ:
       ?T

±?×￠:


==============================================================================*/
void CustService_ShowDialog(CCustServiceMenu *pMe,uint16  dlgResId)
{
	int nRet;

	if (ISHELL_GetActiveDialog(pMe->m_pShell) != NULL)
	{
		return;
	}

	nRet = ISHELL_CreateDialog(pMe->m_pShell,AEE_CUSTSERVICE_RES_FILE,dlgResId,NULL);

	if (nRet != SUCCESS)
	{
	
	}
}

/*==============================================================================
oˉêy:
       SportSApp_RouteDialogEvent
?μ?÷:
       oˉêy??BREWê??t?·óé??μ±?°???ˉ???°?òμ?ê??t′|àíoˉêy?￡

2?êy:
       pMe [In]: ???òSportSApp Applet???ó?á11μ?????,???á11°üo?D?3ìDòμ?ì??¨D??￠.
       eCode [in]￡oê??t′ú???￡
       wParam [in]￡oó?ê??teCode1?áaμ?êy?Y?￡
       dwParam [in]￡oó?ê??teCode1?áaμ?êy?Y?￡

·μ???μ:
       TRUE￡o′?è?ê??t±?′|àí?￡
       FALSE￡o′?è?ê??t??±?′|àí?￡

±?×￠:

==============================================================================*/
boolean CustService_RouteDialogEvent(CCustServiceMenu *pMe,
    AEEEvent eCode,
    uint16  wParam,
    uint32 dwParam
)
{
	if (NULL == pMe)
	{
	    return FALSE;
	}

	if (NULL == pMe->m_pActiveDlg)
	{
	    return FALSE;
	}
	
	switch (pMe->m_pActiveDlgID)
	{
		case IDD_DIALOG_MAIN:
		    return HandleMainDialogEvent(pMe,eCode,wParam,dwParam);
			
		case IDD_DIALOG_HOTLINE:
			return HandleCustHotlineDialogEvent(pMe,eCode,wParam,dwParam);
			
		case IDD_DIALOG_MOBILEHALL:
			return HandleMobileEHallDialogEvent(pMe,eCode,wParam,dwParam);
			
		case IDD_DIALOG_CALLSECRETARY:
			return HandleCallSecretaryDialogEvent(pMe,eCode,wParam,dwParam);

        case IDD_DIALOG_MYSUBSCRIPTION:
            return HandleMySubscriptionDialogEvent(pMe,eCode,wParam,dwParam);

        case IDD_DIALOG_PHSERGUIDE:
            return HandlePhoneSerGuideDialogEvent(pMe,eCode,wParam,dwParam);
			
		case IDD_DIALOG_MSGBOX:
            return HandleMessegeBoxDialogEvent(pMe,eCode,wParam,dwParam);
            
			default:	
			break;	
	}
    return FALSE;
}

/*==============================================================================
oˉêy￡o
       HandleMainDialogEvent
?μ?÷￡o
       IDD_DIALOG_MAIN???°?òê??t′|àíoˉêy

2?êy￡o
       pMe [in]￡o???òSportSApp Applet???ó?á11μ??????￡???á11°üo?D?3ìDòμ?ì??¨D??￠?￡
       eCode [in]￡oê??t′ú???￡
       wParam￡oê??t?à1?êy?Y?￡
       dwParam￡oê??t?à1?êy?Y?￡

·μ???μ￡o
       TRUE￡o′?è?ê??t±?′|àí?￡
      FALSE￡o′?è?ê??t±?o????￡

±?×￠￡o

==============================================================================*/
static boolean  HandleMainDialogEvent(CCustServiceMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
	uint16 bytNewData;
	PARAM_NOT_REF(dwParam)
		
	IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,IDC_MENU_MAIN);
	
	if (pMenu == NULL)
	{
	    return FALSE;
	}
	
	switch (eCode)
	{
		case EVT_DIALOG_INIT:		
			IMENUCTL_AddItem(pMenu, AEE_CUSTSERVICE_RES_FILE, IDS_ITEM_HOTLINE, IDS_ITEM_HOTLINE, NULL,0);
			IMENUCTL_AddItem(pMenu, AEE_CUSTSERVICE_RES_FILE, IDS_ITEM_MOBILEHALL, IDS_ITEM_MOBILEHALL, NULL,0);
			IMENUCTL_AddItem(pMenu, AEE_CUSTSERVICE_RES_FILE, IDS_ITEM_CALLSECRETARY, IDS_ITEM_CALLSECRETARY, NULL, 0);	
			IMENUCTL_AddItem(pMenu, AEE_CUSTSERVICE_RES_FILE, IDS_ITEM_SUBSCRIPTION, IDS_ITEM_SUBSCRIPTION, NULL, 0);	
			IMENUCTL_AddItem(pMenu, AEE_CUSTSERVICE_RES_FILE, IDS_ITEM_PHSERGUIDE, IDS_ITEM_PHSERGUIDE, NULL, 0);	
			return TRUE;

		case EVT_DIALOG_START:

            //IMENUCTL_SetOemProperties( pMenu , OEMMP_DISTINGUISH_INFOKEY_SELECTKEY);
            IMENUCTL_SetOemProperties(pMenu,OEMMP_USE_MENU_STYLE);
            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);
			IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);

			(void) ISHELL_PostEvent( pMe->m_pShell,
			                         AEECLSID_CUSTSERVICE,
			                         EVT_USER_REDRAW,
			                         0,
			                         0);
			return TRUE;

		case EVT_USER_REDRAW:
			(void)IMENUCTL_Redraw(pMenu);
			return TRUE;

		case EVT_DIALOG_END:
			return TRUE;

		case EVT_KEY:
			
			switch(wParam)
			{
				case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
    				return TRUE;
                    
                case AVK_INFO:
                
                    return TRUE;

				default:
					break;
			}
	        	return TRUE;

		case EVT_COMMAND:
			switch(wParam)
			{						
				case IDS_ITEM_HOTLINE:
                    CustService_MakeCallHotline(pMe);
                    
					break;
				case IDS_ITEM_MOBILEHALL:
                    CLOSE_DIALOG(DLGRET_CUSTSERVICE_MOBILEHALL)
                    break;
                
				case IDS_ITEM_CALLSECRETARY:
                    CustService_MakeCallSecretary(pMe);
					break;
					
				case IDS_ITEM_SUBSCRIPTION:
					CLOSE_DIALOG(DLGRET_CUSTSERVICE_SUBSCRIPTION)
					break;
					
				case IDS_ITEM_PHSERGUIDE:
					CLOSE_DIALOG(DLGRET_CUSTSERVICE_PHSERGUIDE)
					break;

			    	default:
			        	break;
			}
			return TRUE;

		default:
			break;
	}
    	return FALSE;
} 

static boolean  HandleCustHotlineDialogEvent(CCustServiceMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    return FALSE;
} 

static boolean  HandleMobileEHallDialogEvent(CCustServiceMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    uint16 bytNewData;
    PARAM_NOT_REF(dwParam)
        
    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,IDC_MENU_MOBILEHALL);
    
    if (pMenu == NULL)
    {
        return FALSE;
    }
    
    switch (eCode)
    {
        case EVT_DIALOG_INIT:       
            IMENUCTL_AddItem(pMenu, AEE_CUSTSERVICE_RES_FILE, IDS_HALL_SMS, IDS_HALL_SMS, NULL,0);
            #ifdef FEATURE_CALL_E_HALL_BY_WAP
            IMENUCTL_AddItem(pMenu, AEE_CUSTSERVICE_RES_FILE, IDS_HALL_WAP, IDS_HALL_WAP, NULL,0);
            #endif
            return TRUE;

        case EVT_DIALOG_START:
            //IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL);
            IMENUCTL_SetOemProperties(pMenu,OEMMP_USE_MENU_STYLE);
            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);
            //IMENUCTL_SetOemProperties( pMenu , OEMMP_DISTINGUISH_INFOKEY_SELECTKEY);
            IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);

            (void) ISHELL_PostEvent( pMe->m_pShell,
                                     AEECLSID_CUSTSERVICE,
                                     EVT_USER_REDRAW,
                                     0,
                                     0);
            return TRUE;

        case EVT_USER_REDRAW:
            (void)IMENUCTL_Redraw(pMenu);
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            
            switch(wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CUSTSERVICE_MAIN)
                    return TRUE;
                    
                case AVK_INFO:
                
                    return TRUE;

                default:
                    break;
            }
                return TRUE;

        case EVT_COMMAND:
            switch (wParam)
            {                       
                case IDS_HALL_SMS:
                    CustService_SendSmsToEHall(pMe);
                    break;
                    
                case IDS_HALL_WAP:
                    CustService_OpenWapToEHall(pMe);
                    break; 
                    
                    default:
                        break;
            }
            return TRUE;

        default:
            break;
    }
        return FALSE;
}

static boolean  HandleCallSecretaryDialogEvent(CCustServiceMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    return FALSE;
}

static boolean  HandleMySubscriptionDialogEvent(CCustServiceMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    uint16 bytNewData;
    PARAM_NOT_REF(dwParam)
        
    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,IDC_MENU_SUBSCRIPTION);
    
    if (pMenu == NULL)
    {
        return FALSE;
    }
    
    switch (eCode)
    {
        case EVT_DIALOG_INIT:       
            IMENUCTL_AddItem(pMenu, AEE_CUSTSERVICE_RES_FILE, IDS_SUBSCR_QUERY,IDS_SUBSCR_QUERY, NULL,0);
            IMENUCTL_AddItem(pMenu, AEE_CUSTSERVICE_RES_FILE, IDS_SUBSCR_CANCEL,IDS_SUBSCR_CANCEL, NULL,0);
            return TRUE;

        case EVT_DIALOG_START:
            //IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL);
            IMENUCTL_SetOemProperties(pMenu,OEMMP_USE_MENU_STYLE);
            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);
            //IMENUCTL_SetOemProperties( pMenu , OEMMP_DISTINGUISH_INFOKEY_SELECTKEY);
            IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);

            (void) ISHELL_PostEvent( pMe->m_pShell,
                                     AEECLSID_CUSTSERVICE,
                                     EVT_USER_REDRAW,
                                     0,
                                     0);
            return TRUE;

        case EVT_USER_REDRAW:
            (void)IMENUCTL_Redraw(pMenu);
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            
            switch(wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CUSTSERVICE_MAIN)
                    return TRUE;
                    
                case AVK_INFO:
                
                    return TRUE;

                default:
                    break;
            }
                return TRUE;

        case EVT_COMMAND:
            switch (wParam)
            {                       
                case IDS_SUBSCR_QUERY:
                    CustService_SendSmsToQueryOperation(pMe);                    
                    break;
                    
                case IDS_SUBSCR_CANCEL:
                    CustService_SendSmsToCancelOperation(pMe);
                    break; 
                    
                    default:
                        break;
            }
            return TRUE;

        default:
            break;
    }
        return FALSE;
}

static boolean  HandlePhoneSerGuideDialogEvent(CCustServiceMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    uint16 bytNewData;
    AECHAR wszTitle[32];
    uint16 selItemindex;
    
    PARAM_NOT_REF(dwParam)
    
    IStatic *pStatic = (IStatic*)IDIALOG_GetControl(pMe->m_pActiveDlg,IDC_STATIC_GUIDE);
            
    if (pStatic == NULL)
    {
        return FALSE;
    }
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            MEMSET(wszTitle,0,sizeof(wszTitle));
            (void) ISHELL_LoadResString(pMe->m_pShell,
                                AEE_CUSTSERVICE_RES_FILE,
                                IDS_GUIDE_CONTENT,
                                wszTitle,
                                sizeof(wszTitle));  
            
            ISTATIC_SetProperties(pStatic,ST_CENTERTITLE);
            ISTATIC_SetText(pStatic,wszTitle,wszTitle,AEE_FONT_NORMAL,AEE_FONT_NORMAL);

            return TRUE;

        case EVT_DIALOG_START:
            (void) ISHELL_PostEvent( pMe->m_pShell,
                         AEECLSID_CUSTSERVICE,
                         EVT_USER_REDRAW,
                         0,
                         0);

            return TRUE;

        case EVT_USER_REDRAW:
            {
                RGBVAL              scrollbarFillColor  = 0;
                        Theme_Param_type    themeParms          = {0};

                          //draw background
                    Appscom_GetThemeParameters( &themeParms);
                        scrollbarFillColor  =   themeParms.themeColor;
                        scrollbarFillColor = IDISPLAY_SetColor( pMe->m_pDisplay, CLR_USER_BACKGROUND, scrollbarFillColor);
                        IDISPLAY_SetColor( pMe->m_pDisplay, CLR_USER_TEXT, RGB_WHITE);
                ISTATIC_Redraw(pStatic); 
                
                CUSTSERVICE_MENU_DRAW_BOTTOMBAR(BTBAR_BACK);
                        IDISPLAY_Update(pMe->m_pDisplay);
                     //   IDISPLAY_SetColor( pMe->m_pDisplay, CLR_USER_BACKGROUND, scrollbarFillColor);
                      //  IDISPLAY_SetColor( pMe->m_pDisplay, CLR_USER_TEXT, RGB_BLACK);  
            }
        
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:            
            switch(wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CUSTSERVICE_MAIN)
                    return TRUE;
                default:
                    break;
}
            return TRUE;

        case EVT_COMMAND:
            return TRUE;
            
        default:
            break;
    }
    return FALSE;
}


 /*==============================================================================
函数：
       HandlePersonalInputDialogEvent
说明：
       IDD_DIALOG_PEDOMETER_OPTION 对话框事件处理函数

参数：
       pMe [in]：指向SportSApp Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
      FALSE：传入事件被忽略。

备注：

==============================================================================*/
 static boolean  HandleMessegeBoxDialogEvent(CCustServiceMenu *pMe,
	 AEEEvent eCode,
	 uint16 wParam,
	 uint32 dwParam
	 )
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

	if ((NULL == pStatic) ||(NULL == pMe))
	{
	    return FALSE;
	}

	switch(eCode)
	{
	    case EVT_DIALOG_INIT:
	        return TRUE;

	    case EVT_DIALOG_START:
	        (void) ISHELL_PostEventEx(pMe->m_pShell, 
	                                EVTFLG_ASYNC,
	                                AEECLSID_CUSTSERVICE,
	                                EVT_USER_REDRAW,
	                                0, 
	                                0);

	        return TRUE;

	    case EVT_USER_REDRAW:
	        {
	            PromptMsg_Param_type  Msg_Param={0};
	            AECHAR  wstrText[MSGBOX_MAXTEXTLEN] = {(AECHAR)'\0'};

	            (void)ISHELL_LoadResString(pMe->m_pShell,
	                            AEE_CUSTSERVICE_RES_FILE,                                
	                            pMe->m_wMsgResID,
	                            wstrText,
	                            sizeof(wstrText));
	                            
	            Msg_Param.ePMsgType = MESSAGE_INFORMATION;
	            Msg_Param.pwszMsg = wstrText;

		        Msg_Param.eBBarType = BTBAR_NONE;

	            
	            DrawPromptMessage(pMe->m_pDisplay, pStatic, &Msg_Param);
	        }

	        IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);


					(void) ISHELL_SetTimer(pMe->m_pShell,
	                            PROMPTMSG_TIMER,
	                            CustService_DialogTimeout,
	                            pMe);


	        
	        return TRUE;

	    case EVT_DIALOG_END:

            (void) ISHELL_CancelTimer(pMe->m_pShell,CustService_DialogTimeout, pMe);
            ISTATIC_Release(pStatic);
	        pStatic = NULL;
	        
	        return TRUE;

	    case EVT_KEY:
	        return TRUE;
	    
	    case EVT_DISPLAYDIALOGTIMEOUT:
			CLOSE_DIALOG(DLGRET_CUSTSERVICE_MAIN)
	        return TRUE;

	    default:
	        break;
	}

	return FALSE;
}


/*==============================================================================
函数:
       SettingMenu_DialogTimeout

说明:
       定时器回调函数。主要用于自动关闭消息对话框。

参数:
       pme [in]：void *类型指针，暗指CSettingMenu结构指针。

返回值:
       无。

备注:

==============================================================================*/
static void CustService_DialogTimeout(void *pme)
{
    CCustServiceMenu *pMe = (CCustServiceMenu*)pme;

    if (NULL == pMe)
    {
        return;
    }

    (void) ISHELL_PostEvent(pMe->m_pShell,
                            AEECLSID_CUSTSERVICE,
                            EVT_DISPLAYDIALOGTIMEOUT,
                            0,
                            0);
}

#endif

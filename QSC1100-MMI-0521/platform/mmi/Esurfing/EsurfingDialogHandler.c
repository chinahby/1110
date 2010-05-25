#ifndef _ESURFING_DIALOG_HANDLER_
#define _ESURFING_DIALOG_HANDLER_


#include "EsurfingAppProt.h"

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

static boolean HandleMainDialogEvent(CEsurfingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

static boolean HandleBestToneDialogEvent(CEsurfingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

static boolean  HandleMessegeBoxDialogEvent(CEsurfingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
    );


static void Esurfing_DialogTimeout(void *pme);

void Esurfing_ShowDialog(CEsurfingMenu *pMe,uint16  dlgResId)
{
	int nRet;

	if (ISHELL_GetActiveDialog(pMe->m_pShell) != NULL)
	{
		return;
	}

	nRet = ISHELL_CreateDialog(pMe->m_pShell,AEE_ESURFING_RES_FILE,dlgResId,NULL);

	if (nRet != SUCCESS)
	{
	
	}
}

boolean Esurfing_RouteDialogEvent(CEsurfingMenu *pMe,
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
			
		case IDD_DIALOG_BESTTONE:
			return HandleBestToneDialogEvent(pMe,eCode,wParam,dwParam);
		
		case IDD_DIALOG_MSGBOX:
            return HandleMessegeBoxDialogEvent(pMe,eCode,wParam,dwParam);
            
		default:	
			break;
	}
    return FALSE;
}

static boolean  HandleMainDialogEvent(CEsurfingMenu *pMe,
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
			IMENUCTL_AddItem(pMenu, AEE_ESURFING_RES_FILE, IDS_ITEM_BESTTONE, IDS_ITEM_BESTTONE, NULL,0);
            
			IMENUCTL_AddItem(pMenu, AEE_ESURFING_RES_FILE, IDS_ITEM_IMUSIC, IDS_ITEM_IMUSIC, NULL,0);
            
            #ifdef FEATURE_ESURFING_SUPPORT_DOWNLOAD
			IMENUCTL_AddItem(pMenu, AEE_ESURFING_RES_FILE, IDS_ITEM_DOWNLOAD, IDS_ITEM_DOWNLOAD, NULL, 0);
            #endif
            
            #ifdef FEATURE_ESURFING_SUPPORT_UTK
			IMENUCTL_AddItem(pMenu, AEE_ESURFING_RES_FILE, IDS_ITEM_UTK, IDS_ITEM_UTK, NULL, 0);
            #endif
            
            #ifdef FEATURE_ESURFING_SUPPORT_ITV
			IMENUCTL_AddItem(pMenu, AEE_ESURFING_RES_FILE, IDS_ITEM_ITV, IDS_ITEM_ITV, NULL, 0);
            #endif

            #ifdef FEATURE_ESURFING_SUPPORT_OFFICE
			IMENUCTL_AddItem(pMenu, AEE_ESURFING_RES_FILE, IDS_ITEM_OFFICE, IDS_ITEM_OFFICE, NULL, 0);
            #endif
            
            #ifdef FEATURE_ESURFING_SUPPORT_MEGAEYE
			IMENUCTL_AddItem(pMenu, AEE_ESURFING_RES_FILE, IDS_ITEM_MEGAEYE, IDS_ITEM_MEGAEYE, NULL, 0);
            #endif

            #ifdef FEATURE_ESURFING_SUPPORT_VNET
			IMENUCTL_AddItem(pMenu, AEE_ESURFING_RES_FILE, IDS_ITEM_VNET, IDS_ITEM_VNET, NULL, 0);
            #endif
            
			return TRUE;

		case EVT_DIALOG_START:
            IMENUCTL_SetOemProperties(pMenu,OEMMP_USE_MENU_STYLE);
            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);

            
            //IMENUCTL_SetOemProperties( pMenu , OEMMP_DISTINGUISH_INFOKEY_SELECTKEY);
			IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);

			(void) ISHELL_PostEvent( pMe->m_pShell,
			                         AEECLSID_ESURFING,
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
				case IDS_ITEM_BESTTONE:
                    #ifdef FEATURE_SUPPORT_WAP_APP
                    Esurfing_OpenWapToBestTone(pMe);
                    #else
                    Esurfing_MakeCallBestTone(pMe);
                    #endif
					break;
					
				case IDS_ITEM_IMUSIC:
					Esurfing_MakeCalliMuisc(pMe);
                    break;
                
				case IDS_ITEM_DOWNLOAD:
					
					break;
					
				case IDS_ITEM_UTK:
					
					break;
					
				case IDS_ITEM_ITV:
                                
                    Esurfing_OpenWapToiTv(pMe);
					break;
					
				case IDS_ITEM_OFFICE:
					
					break;
					
				case IDS_ITEM_MEGAEYE:
                    break;

                case IDS_ITEM_VNET:
                    Esurfing_OpenWapToVNet(pMe);
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

static boolean  HandleBestToneDialogEvent(CEsurfingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    uint16 bytNewData;
    PARAM_NOT_REF(dwParam)
        
    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,IDC_MENU_BESTTONE);
    
    if (pMenu == NULL)
    {
        return FALSE;
    }
    
    switch (eCode)
    {
        case EVT_DIALOG_INIT:       
            IMENUCTL_AddItem(pMenu, AEE_ESURFING_RES_FILE, IDS_BTONE_CALL, IDS_BTONE_CALL, NULL,0);
            
            #ifdef FEATURE_ESURFING_BESTTONE_WAP
            IMENUCTL_AddItem(pMenu, AEE_ESURFING_RES_FILE, IDS_BTONE_WAP, IDS_BTONE_WAP, NULL,0);
            #endif

            return TRUE;

        case EVT_DIALOG_START:
            IMENUCTL_SetOemProperties(pMenu,OEMMP_USE_MENU_STYLE);
            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);

            //IMENUCTL_SetOemProperties( pMenu , OEMMP_DISTINGUISH_INFOKEY_SELECTKEY);
            IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);

            (void) ISHELL_PostEvent( pMe->m_pShell,
                                     AEECLSID_ESURFING,
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
                    CLOSE_DIALOG(DLGRET_ESURFING_MAIN)
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
                case IDS_BTONE_CALL:
                    Esurfing_MakeCallBestTone(pMe);
                    break;
                    
                case IDS_BTONE_WAP:
                    Esurfing_OpenWapToBestTone(pMe);
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
 static boolean  HandleMessegeBoxDialogEvent(CEsurfingMenu *pMe,
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
	                                AEECLSID_ESURFING,
	                                EVT_USER_REDRAW,
	                                0, 
	                                0);

	        return TRUE;

	    case EVT_USER_REDRAW:
	        {
	            PromptMsg_Param_type  Msg_Param={0};
	            AECHAR  wstrText[MSGBOX_MAXTEXTLEN] = {(AECHAR)'\0'};

	            (void)ISHELL_LoadResString(pMe->m_pShell,
	                            AEE_ESURFING_RES_FILE,                                
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
	                            Esurfing_DialogTimeout,
	                            pMe);

	        return TRUE;

	    case EVT_DIALOG_END:

          (void) ISHELL_CancelTimer(pMe->m_pShell,Esurfing_DialogTimeout, pMe);
          ISTATIC_Release(pStatic);
	        pStatic = NULL;
	        
	        return TRUE;

	    case EVT_KEY:
	        return TRUE;
	    
	    case EVT_DISPLAYDIALOGTIMEOUT:
					CLOSE_DIALOG(DLGRET_ESURFING_MAIN)
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
static void Esurfing_DialogTimeout(void *pme)
{
    CEsurfingMenu *pMe = (CEsurfingMenu*)pme;

    if (NULL == pMe)
    {
        return;
    }

    (void) ISHELL_PostEvent(pMe->m_pShell,
                            AEECLSID_ESURFING,
                            EVT_DISPLAYDIALOGTIMEOUT,
                            0,
                            0);
}

#endif

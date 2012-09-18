/*==============================================================================
// 文件：
//        BTAppDialogHandler.c
//        版权所有(c) 2009 Anylook Tech. CO.,LTD.
//
// 文件说明：
//
// 作者：zzg
// 创建日期：2010-01-04
// 当前版本：Draft
==============================================================================*/

/*==============================================================================
修改历史记录：
时       间      修 改 人    问题单号  修改内容、位置及原因
-----------      ----------  --------  -----------------------------------------
==============================================================================*/


/*==============================================================================
                                 本文件包含的外部文件
==============================================================================*/
#include "BTApp_priv.h"		//"BTApp.h"
#include "BTAppUtils.h"


#include "appscommonimages.brh"


/*==============================================================================
                                 宏定义和常数
==============================================================================*/
// 关闭对话框宏
#define CLOSE_DIALOG(DlgRet)  {    \
                                     pMe->m_eDlgRet = DlgRet; \
                                     (void) ISHELL_EndDialog(pMe->m_pShell);  \
                                 }

//Add By zzg 2011_02_13
#define BTAPPDLG_BT_LMP_VER_2_0          0x03 /* 2.0 controller lmp version from
					                                           ** BT Assigned numbers
					                                           */



// 画界面底部提示条宏定义
#define BT_DRAW_BOTTOMBAR(x)                           \
{                                                   \
    BottomBar_Param_type BarParam;                  \
    MEMSET(&BarParam, 0, sizeof(BarParam));         \
    BarParam.eBBarType = x;                         \
    DrawBottomBar(pMe->m_pIDisplay, &BarParam);      \
}


char*  szOPPServerName      = "QC OPP";

//Add By zzg 2011_02_18
#define NEW_FOLDER_NAME    "newfolder"
#define FOLDER_NAME_STR    "folder name"
#define FILE_NAME_STR      "file name"
#define SEARCH_CHR         '"'
#define ROOT_FOLDER_STR    "ROOT"
#define PARENT_FOLDER_STR  "PARENT"
#define CURRENT_FOLDER_STR "CURRENT"

#define CARD0_DIR    "fs:/card0"		//have no '/'


/*static*/ char   szOperandex[AEEBT_MAX_FILE_NAME+1];		
//Add End


/*==============================================================================
                                 类型定义
==============================================================================*/


/*==============================================================================
                                 函数声明
==============================================================================*/
// 对话框 IDD_BT_MAINMENU 事件处理函数
static boolean  HandleMainDialogEvent(CBTApp *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

// 对话框 IDD_DEVICE_SEARCH 事件处理函数
static boolean  HandleDeviceSearchDialogEvent(CBTApp *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

// 对话框 IDD_SEARCH_RESULT 事件处理函数
static boolean  HandleSearchResultDialogEvent(CBTApp *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

// 对话框 IDD_BT_DEVICE_INFO 事件处理函数
static boolean  HandleDeviceInfoDialogEvent(CBTApp *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

// 对话框 IDD_BT_DEVICE_INFO_OPITION 事件处理函数
static boolean  HandleDeviceInfoOpitionDialogEvent(CBTApp *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

// 对话框 IDD_BT_BOND_OPITION 事件处理函数
static boolean  HandleBondOpitionDialogEvent(CBTApp *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);


// 对话框 IDD_DEVICE 事件处理函数
static boolean  HandleDeviceDialogEvent(CBTApp *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

// 对话框 IDD_DEVICE_LIST 事件处理函数
static boolean  HandleDeviceListDialogEvent(CBTApp *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);



// 对话框 IDD_MYINFO 事件处理函数
static boolean  HandleMyInfoDialogEvent(CBTApp *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

// 对话框 IDD_MYINFO_OPITION 事件处理函数
static boolean  HandleMyInfoOpitionDialogEvent(CBTApp *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

// 对话框 IDD_MYINFO_EDIT 事件处理函数
static boolean  HandleMyInfoEditDialogEvent(CBTApp *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

// 对话框 IDD_BT_SECURITY 事件处理函数
static boolean  HandleSetSecurityDialogEvent(CBTApp *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

// 对话框 IDD_BT_DISCOVERABLE 事件处理函数
static boolean  HandleSetDiscoverableDialogEvent(CBTApp *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

// 对话框 IDD_BT_IO_CAPABILITY 事件处理函数
static boolean  HandleSetIOCapabilityDialogEvent(CBTApp *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

// 对话框 IDD_BT_DEBUG_KEY 事件处理函数
static boolean  HandleSetDebugKeyDialogEvent(CBTApp *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);


// 对话框 IDD_BT_FTP 事件处理函数
static boolean  HandleFtpDialogEvent(CBTApp *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

// 对话框 IDD_BT_FTP_SERVER 事件处理函数
static boolean  HandleFtpServerDialogEvent(CBTApp *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);


// 对话框 IDD_BT_FTP_CLIENT 事件处理函数
static boolean  HandleFtpClientDialogEvent(CBTApp *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

// 对话框 IDD_BT_FTP_SETTING 事件处理函数
static boolean  HandleFtpSettingDialogEvent(CBTApp *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

// 对话框 IDD_BT_FTP_SERVER_REGISTER 事件处理函数
static boolean  HandleFtpServerRegisterDialogEvent(CBTApp *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

// 对话框 IDD_BT_FTP_BROWSE 事件处理函数
static boolean  HandleFtpBrowseDialogEvent(CBTApp *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

// 对话框 IDD_BT_FTP_BROWSE_OPITION 事件处理函数
static boolean  HandleFtpBrowseOpitionDialogEvent(CBTApp *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);




// 对话框 IDD_BT_MSGBOX 事件处理函数
static boolean  HandleMsgBoxDialogEvent(CBTApp *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

// 对话框 IDD_BT_PROMPT事件处理函数
static boolean  HandleProMptDialogEvent(CBTApp *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);


// 对话框 IDD_BT_EDIT 事件处理函数
static boolean  HandleBtTextEditDialogEvent(CBTApp *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

// 对话框 IDD_BT_SEND_FILE 事件处理函数
static boolean  HandleSendFileDialogEvent(CBTApp *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

// 对话框 IDD_BT_OBEX_LIST_SERVERS 事件处理函数
static boolean  HandleObexListServersDialogEvent(CBTApp *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

// 对话框 IDD_BT_FILE_PROGRESS 事件处理函数
static boolean  HandleFileProgressDialogEvent(CBTApp *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

static boolean BTApp_SaveTextEdit( CBTApp* pMe, uint16 DlgID);	//Add By zzg  2011_01_06	
static boolean BTApp_ClearDiscoverableEx( CBTApp* pMe );


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
       BTApp_ShowDialog
说明:
       函数由状态处理函数用来创建相应对话框资源ID的对话框。

参数:
       pMe [In]: 指向BTApp Applet对象结构的指针,该结构包含小程序的特定信息.
       dlgResId [in]：对话框资源ID，函数根据该ID创建对话框。

返回值:
       无

备注:

==============================================================================*/
void BTApp_ShowDialog(CBTApp *pMe, uint16  dlgResId)
{
    int nRet;
	
    MSG_FATAL("***zzg BTApp_ShowDialog Start dlgResId=%d", dlgResId, 0, 0);
    
    // At most one dialog open at once
    if (ISHELL_GetActiveDialog(pMe->m_pShell) != NULL)
    {
        // Looks like there is one dialog already opened.
        // Flag an error an return without doing anything.
        MSG_FATAL("Trying to create a dialog without closing the previous one",0,0,0);		
        return;
    }

    nRet = ISHELL_CreateDialog(pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, dlgResId, NULL);

    if (nRet != SUCCESS)
    {
        MSG_FATAL("Failed to create the dialog in the BTApp applet nRet=%x",nRet,0,0);
    }
    MSG_FATAL("BTApp_ShowDialog END",0,0,0);
}


/*==============================================================================
函数:
       BTApp_RouteDialogEvent
说明:
       函数将BREW事件路由给当前活动对话框的事件处理函数。

参数:
       pMe [In]: 指向BTApp Applet对象结构的指针,该结构包含小程序的特定信息.
       eCode [in]：事件代码。
       wParam [in]：与事件eCode关联的数据。
       dwParam [in]：与事件eCode关联的数据。

返回值:
       TRUE：传入事件被处理。
       FALSE：传入事件没被处理。

备注:

==============================================================================*/
boolean BTApp_RouteDialogEvent(CBTApp *pMe,
    AEEEvent    eCode,
    uint16  wParam,
    uint32 dwParam
)
{
    MSG_FATAL("BTApp_RouteDialogEvent Start m_pActiveDlgID=%d, eCode=%x, wParam=%x",pMe->m_pActiveDlgID,eCode,wParam);
	
    if (NULL == pMe)
    {
        MSG_FATAL("BTApp_RouteDialogEvent pMe==NULL",0,0,0);
        return FALSE;
    }

    if (NULL == pMe->m_pActiveDlg)
    {
        MSG_FATAL("BTApp_RouteDialogEvent NULL == pMe->m_pActiveDlg",0,0,0);
        return FALSE;
    }
    
    switch (pMe->m_pActiveDlgID)
    {
        case IDD_BT_MAINMENU:
            return HandleMainDialogEvent(pMe,eCode,wParam,dwParam);

        case IDD_DEVICE_SEARCH:
            return HandleDeviceSearchDialogEvent(pMe,eCode,wParam,dwParam);

		case IDD_SEARCH_RESULT:			
            return HandleSearchResultDialogEvent(pMe,eCode,wParam,dwParam);

		case IDD_BT_DEVICE_INFO:			
            return HandleDeviceInfoDialogEvent(pMe,eCode,wParam,dwParam);	

		case IDD_BT_DEVICE_INFO_OPITION:				
            return HandleDeviceInfoOpitionDialogEvent(pMe,eCode,wParam,dwParam);

		case IDD_BT_BOND_OPITION:				
            return HandleBondOpitionDialogEvent(pMe,eCode,wParam,dwParam);

		case IDD_DEVICE:
			return HandleDeviceDialogEvent(pMe,eCode,wParam,dwParam);	
			
		case IDD_DEVICE_LIST:
            return HandleDeviceListDialogEvent(pMe,eCode,wParam,dwParam);	
		
		case IDD_MYINFO:			
            return HandleMyInfoDialogEvent(pMe,eCode,wParam,dwParam);	
			
		case IDD_MYINFO_OPITION:			
            return HandleMyInfoOpitionDialogEvent(pMe,eCode,wParam,dwParam);

		//IDD_BT_EDITNAME/IDD_BT_EDIT_SHORTNAME/IDD_BT_EDIT_MANU_DATA
		case IDD_MYINFO_EDIT:			
			return HandleMyInfoEditDialogEvent(pMe,eCode,wParam,dwParam);
		
		case IDD_BT_SECURITY:			
            return HandleSetSecurityDialogEvent(pMe,eCode,wParam,dwParam);		

		case IDD_BT_DISCOVERABLE:			
            return HandleSetDiscoverableDialogEvent(pMe,eCode,wParam,dwParam);	

		case IDD_BT_IO_CAPABILITY:			
            return HandleSetIOCapabilityDialogEvent(pMe,eCode,wParam,dwParam);	
			
		case IDD_BT_DEBUG_KEY:			
            return HandleSetDebugKeyDialogEvent(pMe,eCode,wParam,dwParam);

		case IDD_BT_FTP:			
            return HandleFtpDialogEvent(pMe,eCode,wParam,dwParam);	

		case IDD_BT_FTP_SERVER:			
            return HandleFtpServerDialogEvent(pMe,eCode,wParam,dwParam);

		case IDD_BT_FTP_CLIENT:			
            return HandleFtpClientDialogEvent(pMe,eCode,wParam,dwParam);

		case IDD_BT_FTP_SETTINGS:			
            return HandleFtpSettingDialogEvent(pMe,eCode,wParam,dwParam);	

		case IDD_BT_FTP_SERVER_REGISTER:			
            return HandleFtpServerRegisterDialogEvent(pMe,eCode,wParam,dwParam);

		case IDD_BT_FTP_BROWSE:			
			return HandleFtpBrowseDialogEvent(pMe,eCode,wParam,dwParam);	

		case IDD_BT_FTP_BROWSE_OPITION:			
			return HandleFtpBrowseOpitionDialogEvent(pMe,eCode,wParam,dwParam);			

		case IDD_BT_MSGBOX:			
            return HandleMsgBoxDialogEvent(pMe,eCode,wParam,dwParam);	

		case IDD_BT_PROMPT:
			return HandleProMptDialogEvent(pMe,eCode,wParam,dwParam);	

		case IDD_BT_EDIT:			
            return HandleBtTextEditDialogEvent(pMe,eCode,wParam,dwParam);	

		case IDD_BT_SEND_FILE:			
            return HandleSendFileDialogEvent(pMe,eCode,wParam,dwParam);		

		case IDD_BT_OBEX_LIST_SERVERS:			
            return HandleObexListServersDialogEvent(pMe,eCode,wParam,dwParam);			

		case IDD_BT_FILE_PROGRESS:			
            return HandleFileProgressDialogEvent(pMe,eCode,wParam,dwParam);	
			
		case IDD_BT_TEXT_EDIT:
            return HandleBtTextEditDialogEvent(pMe,eCode,wParam,dwParam);	
			
        default:
            return FALSE;
    }
    MSG_FATAL("SettingMenu_RouteDialogEvent End",0,0,0);
}

/*==============================================================================
函数：
       HandleMainDialogEvent
说明：
       IDD_BT_MAINMENU对话框事件处理函数

参数：
       pMe [in]：指向BTApp Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
      FALSE：传入事件被忽略。

备注：

==============================================================================*/
static boolean  HandleMainDialogEvent(CBTApp *pMe,
											    AEEEvent eCode,
											    uint16 wParam,
											    uint32 dwParam
)
{
    PARAM_NOT_REF(dwParam)

	boolean	   bt_status = FALSE;
			
    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_BT_MAINMENU);
		
    if (pMenu == NULL)
    {
        return FALSE;
    }
	
	if (NULL != pMe->m_pConfig)
	{
		ICONFIG_GetItem(pMe->m_pConfig, CFGI_BT_STATUS, &bt_status, sizeof(bt_status));
		MSG_FATAL("***zzg HandleMainDialogEvent bt_status=%d***", bt_status, 0, 0);
	}

    MSG_FATAL("***zzg HandleMainDialogEvent eCode=%x, wParam=%x ,dwParam=%x***",eCode,wParam,dwParam);

    switch (eCode)
    {
        case EVT_DIALOG_INIT:			
		{	
            return TRUE;
		}

        case EVT_DIALOG_START:   
        {
			IMENUCTL_SetSel(pMenu, pMe->m_currDlgId);
            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);
            IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);

#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SETTING_BACKGROUND);
#endif
            IMENUCTL_SetBottomBarType(pMenu, BTBAR_SELECT_BACK);

            (void) ISHELL_PostEvent( pMe->m_pShell,
                                     AEECLSID_BLUETOOTH_APP,
                                     EVT_USER_REDRAW,
                                     0,
                                     0);
            return TRUE;
        }

        case EVT_USER_REDRAW:   
		{			
			AECHAR 		WTitle[40] = {0};
			
			uint16      titleID = IDS_OFF;
			uint16      itemID = IDS_BT_ON;
			
			if (bt_status == TRUE)  			
			{				 
				itemID = IDS_BT_OFF;
				titleID = IDS_ON;
			} 

			(void)ISHELL_LoadResString(pMe->m_pShell,
				                        AEE_APPSBTAPP_RES_FILE,                                
				                        titleID,
				                        WTitle,
				                        sizeof(WTitle));

			if (NULL == pMe->m_pIAnn)
			{
				MSG_FATAL("***zzg HandleMainDialogEvent NULL == pMe->m_pIAnn***", 0, 0, 0);
			}
			
			IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, WTitle);   

			
			IMENUCTL_DeleteAll(pMenu);	
			
  			IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, itemID, itemID, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, IDS_DEVICE_SEARCH, IDS_DEVICE_SEARCH, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, IDS_DEVICES, IDS_DEVICES, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, IDS_MY_INFO, IDS_MY_INFO, NULL, 0);

#if	1			
			IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, IDS_FTP, IDS_FTP, NULL, 0);

			//IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, IDS_SETTINGS, IDS_SETTINGS, NULL, 0);
			
			/*		
			if (pMe->mAG.bConnected)
			{
				if ((pMe->mAG.bSLCUp != FALSE) || (BTApp_CallConnected(pMe) != BT_APP_CALL_NONE))
				{
					IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, IDS_AUDIO_TRANSFER, IDS_AUDIO_TRANSFER, NULL, 0);					
				}
			}
			*/
#endif

			MSG_FATAL("***zzg HandleMainDialogEvent EVT_USER_REDRAW***", 0, 0, 0);
            (void)IMENUCTL_Redraw(pMenu);
            return TRUE;
        }

        case EVT_DIALOG_END:
        {
			return TRUE;
        }

        case EVT_KEY:
        {
			switch (wParam)
            {
                case AVK_CLR:
                {
					CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;
                }

                default:
                {
					break;
                }
            }
            return TRUE;
        }

        case EVT_COMMAND:            
        {
			pMe->m_currDlgId = wParam;
						
            switch (wParam)
            {
            	case IDS_BT_OFF:
				{
					MSG_FATAL("***zzg BTAppDialogHandle EVT_COMMAND IDS_BT_OFF***", 0, 0, 0);
					BTApp_DisableBT(pMe);

					BTApp_BuildPrompt(pMe, BT_APP_MENU_OFF);					
					//BTApp_ShowMessage( pMe, IDS_MSG_BT_DISABLED, NULL, 5);					
		                                     
		            (void) ISHELL_PostEvent( pMe->m_pShell,
		                                     AEECLSID_BLUETOOTH_APP,
		                                     EVT_USER_REDRAW,
		                                     0,
		                                     0);                         
					break;
            	}
				
				case IDS_BT_ON:
				{
					MSG_FATAL("***zzg BTAppDialogHandle EVT_COMMAND IDS_BT_ON***", 0, 0, 0);
                    pMe->mEnablingType = BTAPP_ENABLING_AG;
					BTApp_EnableBT(pMe);

					BTApp_BuildPrompt(pMe, BT_APP_MENU_ON);		
					//BTApp_ShowMessage( pMe, IDS_MSG_BT_ENABLED, NULL, 5);					

					(void) ISHELL_PostEvent( pMe->m_pShell,
		                                     AEECLSID_BLUETOOTH_APP,
		                                     EVT_USER_REDRAW,
		                                     0,
		                                     0);
					
					break;
				}
					
				case IDS_DEVICE_SEARCH: 
				{	
					/*
					if (bt_status == FALSE)					  
					{						
						pMe->m_msg_id = IDS_BT_CLOSED;
						pMe->m_bNeedStr = FALSE;
						
						CLOSE_DIALOG(DLGRET_BT_MSGBOX)
					}
					else
					{
						CLOSE_DIALOG(DLGRET_DEVICESRH)
					}	
					*/
					
					if (bt_status == FALSE)					  
					{
                        pMe->mEnablingType = BTAPP_ENABLING_AG;
						BTApp_EnableBT(pMe);		
					}
					CLOSE_DIALOG(DLGRET_DEVICESRH)
					
				    break;
				}
				
				case IDS_DEVICES: 
				{
					/*
					if (bt_status == FALSE)					  
					{						
						pMe->m_msg_id = IDS_BT_CLOSED;
						pMe->m_bNeedStr = FALSE;
						
						CLOSE_DIALOG(DLGRET_BT_MSGBOX)
					}
					else
					{
						CLOSE_DIALOG(DLGRET_DEVICE)
					}	
					*/

					if (bt_status == FALSE)					  
					{
                        pMe->mEnablingType = BTAPP_ENABLING_AG;
						BTApp_EnableBT(pMe);
					}					
					CLOSE_DIALOG(DLGRET_DEVICE)
					
				    break;
				}	
				
				case IDS_MY_INFO: 	
				{
					/*
					if (bt_status == FALSE)					  
					{
						pMe->m_msg_id = IDS_BT_CLOSED;
						pMe->m_bNeedStr = FALSE;
						
						CLOSE_DIALOG(DLGRET_BT_MSGBOX)
					}
					else
					{
						CLOSE_DIALOG(DLGRET_MYINFO)
					}	
					*/

					if (bt_status == FALSE)					  
					{
                        pMe->mEnablingType = BTAPP_ENABLING_AG;
						BTApp_EnableBT(pMe);
					}					
					CLOSE_DIALOG(DLGRET_MYINFO)
						
				    break;
				}	
				
				case IDS_FTP:
				{
					/*
					if (bt_status == FALSE)					  
					{						
						pMe->m_msg_id = IDS_BT_CLOSED;
						pMe->m_bNeedStr = FALSE;
						
						CLOSE_DIALOG(DLGRET_BT_MSGBOX)
					}
					else
					{
						if (BTApp_FTPInit(pMe) != FALSE)
						{
							CLOSE_DIALOG(DLGRET_FTP)
						}
					}	
					*/

					if (bt_status == FALSE)					  
					{
                        pMe->mEnablingType = BTAPP_ENABLING_AG;
						BTApp_EnableBT(pMe);
					}		
					
					if (BTApp_FTPInit(pMe) != FALSE)
					{
						CLOSE_DIALOG(DLGRET_FTP)
					}
				    break;
				}
				
				default:
				{
					ASSERT_NOT_REACHABLE;
				}
            }
            return TRUE;
        }
		
        default:
        {
			break;
        }
    }
    return FALSE;
} // HandleMainDialogEvent


static boolean HandleDeviceSearchDialogEvent(CBTApp *pMe,
 													     AEEEvent eCode,
 													     uint16 wParam,
 													     uint32 dwParam
)
{    
	PARAM_NOT_REF(dwParam)
		
	IStatic  *pStatic = (IStatic*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_DEVICE_SEARCH);
	IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_MENU_BT_DEVICE_SEARCH);

	if (pStatic == NULL)
	{
		return FALSE;
	}	

	MSG_FATAL("***zzg HandleDeviceSearchDialogEvent eCode=%x, wParam=%x ,dwParam=%x***",eCode,wParam,dwParam);

	switch (eCode)
	{
		case EVT_DIALOG_INIT:			
		{
			IDIALOG_SetProperties((IDialog *)dwParam, DLG_NOT_REDRAW_AFTER_START);
			return TRUE;
		}

		case EVT_DIALOG_START:
		{			
			AEERect 	rc;
			AECHAR		WTitle[40] = {0};	
			boolean 	success = FALSE; 
			
			(void)ISHELL_LoadResString(pMe->m_pShell,
									   AEE_APPSBTAPP_RES_FILE, 							   
									   IDS_BT_TITLE,
									   WTitle,
									   sizeof(WTitle));
			
			IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, WTitle);  	

			//BTApp_DoDeviceSearch(pMe, AEEBT_COD_SC_ALL, NULL);	
			pMe->mSD.uNumRecs = 0;
			MEMSET(pMe->mSD.devRec, 0, (sizeof(AEEBTDeviceRecord)*MAX_DEVICES));

			MSG_FATAL("***zzg DeviceSearchDlg IBTEXTSD_DiscoverDevices***", 0, 0, 0);			
			success = (IBTEXTSD_DiscoverDevices(pMe->mSD.po, pMe->uDeviceSrhType, NULL, pMe->mSD.devRec, MAX_DEVICES ) == SUCCESS);

			if (success)
			{
				//BTApp_ShowMessage(pMe, IDS_MSG_SEARCHING, NULL, 0);
			}
			else
			{
				MSG_FATAL( "DevSrchAll - DiscDev failed", 0, 0, 0 );
			}
					
			// set rect for info display area
			SETAEERECT ( &rc, pMe->m_rect.x, 
						 pMe->m_rect.y, 
						 pMe->m_rect.dx, 
						 pMe->m_rect.dy - BOTTOMBAR_HEIGHT); // leave room for SK menu
					 
			ISTATIC_SetRect(pStatic, &rc );

			ISTATIC_SetProperties(pStatic, ISTATIC_GetProperties(pStatic) & ~ST_MIDDLETEXT & ~ST_CENTERTEXT);	 
			ISTATIC_SetProperties(pStatic, ST_NOSCROLL|ST_GRAPHIC_BG);	
			ISTATIC_SetBackGround(pStatic, AEE_APPSCOMMONRES_IMAGESFILE, IDB_BACKGROUND);

			IMENUCTL_SetSel(pMenu, pMe->m_currDlgId);
            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);
            IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);

#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SETTING_BACKGROUND);
#endif
            IMENUCTL_SetBottomBarType(pMenu,BTBAR_BACK);
			
			(void) ISHELL_PostEvent(pMe->m_pShell,
									AEECLSID_BLUETOOTH_APP,
									EVT_USER_REDRAW,
									0,
									0);
			return TRUE;
		}
		
		case EVT_USER_REDRAW:		
		{	
			AECHAR  wstrText[MSGBOX_MAXTEXTLEN];			
                        
            (void) ISHELL_LoadResString(pMe->m_pShell,
	                                    AEE_APPSBTAPP_RES_FILE,
	                                    IDS_MSG_SEARCHING,
	                                    wstrText,
	                                    sizeof(wstrText));

			ISTATIC_SetText(pStatic, NULL, wstrText, AEE_FONT_BOLD, AEE_FONT_NORMAL);
            			
			ISTATIC_Redraw(pStatic);	
			ISTATIC_SetActive(pStatic, TRUE);			
			IDISPLAY_UpdateEx( pMe->m_pIDisplay, FALSE); 			
			return TRUE;
		}

		case EVT_DIALOG_END:
		{
			return TRUE;
		}

		case EVT_KEY_PRESS: 	  
		{	
			switch(wParam)
			{								
				case AVK_CLR:	
				{					
					CLOSE_DIALOG(DLGRET_CANCELED)	
					return TRUE;
				}
				default:
				{
					break;
				}
			}
			
			return TRUE;
		}
		
		default:
			break;
	}
    return FALSE;
} 	


static boolean HandleSearchResultDialogEvent(CBTApp *pMe,
														 AEEEvent eCode,
														 uint16 wParam,
														 uint32 dwParam
)
{	 		
	PARAM_NOT_REF(dwParam)

    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_SEARCH_RESULT);

	CtlAddItem ai;
	int        result = EFAILED;
	uint8      i, numItems, len = 0;
	uint16     stringID = IDS_SEARCHING;
	AEEBTDeviceInfo* pDev;
	AECHAR* 	pwName;

#ifdef FEATURE_BT_2_1
	int 				MapIndex;
	AEEBTDeviceInfo* 	pMapDev = NULL;
	AECHAR           	wBuf[5];
	boolean          	bEIRReqName = TRUE;
	boolean          	nameReqIssued = FALSE;
	uint16           	tempuValue1 = 0;

	STRTOWSTR( "...", wBuf, sizeof(wBuf) );
#else

#endif /* FEATURE_BT_2_1 */
	
    if (pMenu == NULL)
    {
        return FALSE;
    }
	
    MSG_FATAL("***zzg HandleSearchResultDialogEvent eCode=%x, wParam=%x ,dwParam=%x", eCode, wParam, dwParam);
   
    switch (eCode)
    {
    	case EVT_DIALOG_INIT:			
		{	
			return TRUE;
		}

        case EVT_DIALOG_START:
        {
            (void) ISHELL_PostEvent( pMe->m_pShell,
                                     AEECLSID_BLUETOOTH_APP,
                                     EVT_USER_REDRAW,
                                     0,
                                     0);
            return TRUE;
        }

        case EVT_USER_REDRAW:  
		{
			AECHAR		WTitle[40] = {0};		
						
			stringID = pMe->mSD.bDiscovering ? IDS_SEARCHING : IDS_DISCOVERED_DEVICES;	

			(void)ISHELL_LoadResString(pMe->m_pShell,
										AEE_APPSBTAPP_RES_FILE, 							   
										stringID,
										WTitle,
										sizeof(WTitle));
			if (pMe->m_pIAnn != NULL)
			{
				IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, WTitle);
			}

			pMe->uCurrMsgId = 0;

			BTApp_InitAddItem(&ai);
			ai.wFont = AEE_FONT_BOLD;						

			if ( pMe->mSD.bDiscovering == FALSE )
			{
				numItems = 0; // force refreshing
				IMENUCTL_DeleteAll(pMenu);
			}
			else
			{
				MENU_SET_SEL(IMENUCTL_GetSel(pMenu));
				numItems = IMENUCTL_GetItemCount(pMenu);
			}				
			
			MSG_FATAL("***zzg SearchResultDlg numItems=%d, uNumRecs=%d***", numItems, pMe->mSD.uNumRecs, 0);
			
#ifdef FEATURE_BT_2_1   			
			for (i = numItems; i < pMe->mSD.uNumRecs; i ++)
			//for (i = 0; i < pMe->mSD.uNumRecs; i ++)	//Modify by zzg 2011_06_03
			{					
				pDev = &pMe->mRM.device[i];
				pDev->bdAddr = pMe->mSD.devRec[i].bdAddr;
				pwName = pDev->wName;
				result = EFAILED;
				len    = 0;
				
				MSG_MED( "BldDevRespMenu - pDev->uValue1 = %d", pDev->uValue1, 0, 0);
				tempuValue1 = 0;

				MSG_FATAL("***zzg SearchResultDlg uValue1=%d, bDiscovering=%d***", pDev->uValue1, pMe->mSD.bDiscovering, 0);
				
				if ((pMe->mSD.bDiscovering == FALSE) && (pDev->uValue1 != 0))
				{
					// save the value of get name status
					tempuValue1 = pDev->uValue1;
					MSG_MED("BldDevRespMenu - tempuValue1 = %d", tempuValue1, 0, 0);
				}

				result = IBTEXTRM_DeviceRead(pMe->mRM.po, pDev);

				
				//Print for the test Start				
				{
					char btname[AEEBT_MAX_DEVICENAME_LEN+1];
  					char btnickname[AEEBT_MAX_NICKNAME_LEN+1];

					WSTRTOSTR (pDev->wName, btname, sizeof(char)*(AEEBT_MAX_DEVICENAME_LEN+1));	
					WSTRTOSTR (pDev->wNickName, btnickname, sizeof(char)*(AEEBT_MAX_NICKNAME_LEN+1));	

					DBGPRINTF("***zzg SrhRltDlgEvt 1 btname=%s,***", btname);
					DBGPRINTF("***zzg SrhRltDlgEvt 1 btnickname=%s***", btnickname);
				}				
				//Print for the test End
				
				if (result != SUCCESS)
				{
					MSG_ERROR("BldDevRespMenu - DeviceRead() failed for index=%d", i, 0, 0);
					MSG_FATAL("***zzg SearchResultDlg  DeviceRead() failed for index=%d***", i, 0, 0);
				}

				MSG_FATAL("***zzg IBTEXTRM_DeviceRead uValue1=%d, bDiscovering=%d***", pDev->uValue1, pMe->mSD.bDiscovering, 0);
				
				if ((result == SUCCESS) && (pMe->mSD.bDiscovering == FALSE) && (tempuValue1 != 0))
				{
					// restore the value of get name status    
					pDev->uValue1 = tempuValue1;
					MSG_MED( "BldDevRespMenu - restored pDev->uValue1 = %d", pDev->uValue1, 0, 0);
				}

				len = WSTRLEN(pDev->wName);
								
				bEIRReqName = TRUE;
				
				if ((pDev->EIRData.uFlags & AEEBT_EIR_DATA_RCVD_B) 
					 && (pDev->EIRData.uFlags & AEEBT_EIR_NAME_CMPLT_B))
				{
					// check if short name is already available from EIR data
					bEIRReqName = FALSE;
				}

				MSG_FATAL("***zzg SearchResult len=%d, bEIRReqName=%d***", len, bEIRReqName, 0);

				MSG_MED("BldDevRespMenu - uFlags = 0x%x", pDev->EIRData.uFlags, 0, 0);
				MSG_MED("BldDevRespMenu - nameLen=%d, uValue1=%d, bEIRReqName=%d", len, pDev->uValue1, bEIRReqName);

				MSG_FATAL("***zzg before IBTEXTSD_GetDeviceName uValue1=%d***", pDev->uValue1, 0, 0);
				
				if ((pDev->uValue1 == 0) && // GetName not done?
					 (bEIRReqName))
				{
					if ((pMe->mSD.bDiscovering == FALSE) && (pMe->mRM.uGetNameDevIdx == MAX_DEVICES))
					{
						nameReqIssued = TRUE;
						MSG_MED( "BldDevRespMenu - requesting name for index=%d", i, 0, 0 );

						MSG_FATAL("***zzg SrhRltDlgEvt IBTEXTSD_GetDeviceName***", 0, 0, 0);

						if (IBTEXTSD_GetDeviceName(pMe->mSD.po, &pDev->bdAddr, pDev->wName, ARR_SIZE(pDev->wName)) == SUCCESS)
						{
							pMe->mRM.uGetNameDevIdx = i;
						}

						//Print the log Start						
						{
							char btname[AEEBT_MAX_DEVICENAME_LEN+1];
		  					char btnickname[AEEBT_MAX_NICKNAME_LEN+1];

							WSTRTOSTR (pDev->wName, btname, sizeof(char)*(AEEBT_MAX_DEVICENAME_LEN+1));	
							WSTRTOSTR (pDev->wNickName, btnickname, sizeof(char)*(AEEBT_MAX_NICKNAME_LEN+1));	

							DBGPRINTF("***zzg SrhRltDlgEvt 2 btname=%s***", btname);
							DBGPRINTF("***zzg SrhRltDlgEvt 2 btnickname=%s***", btnickname);
						}						
						//Print the log End						
					}
				}
				else if ((result == SUCCESS) && (len > 0))
				{
					pDev->uValue1 = UD1_GET_NAME_SUCCESS;
				}

				for (MapIndex = i; MapIndex >= 0; MapIndex--)
				{
					if (MapIndex == 0)
					{
						pMe->mRM.uDevToDspIdxMap[MapIndex] = i;
						break;
					}
					
					pMapDev = &pMe->mRM.device[(pMe->mRM.uDevToDspIdxMap[MapIndex - 1])];

#ifndef FEATURE_APP_TEST_AUTOMATION
					
					if (pMapDev->EIRData.deviceRanking > pDev->EIRData.deviceRanking )
					{
						// moving the element down so that the new index can be inserted
						pMe->mRM.uDevToDspIdxMap[MapIndex] = pMe->mRM.uDevToDspIdxMap[MapIndex - 1];
					}
					else
#endif //FEATURE_APP_TEST_AUTOMATION
					{
						//place identified - add the new index below the current index 
						pMe->mRM.uDevToDspIdxMap[MapIndex] = i;
						break;
					}
				}				
				
				MSG_MED( "BldDevRespMenu - mapIndex=%d devRank=%d DevIndex=%d", MapIndex, pDev->EIRData.deviceRanking, pMe->mRM.uDevToDspIdxMap[MapIndex]);
			}

			
			if ((nameReqIssued == FALSE) && (pMe->mSD.bDiscovering == FALSE))
			{
				// Name request for all devices have completed so init uValue1 
				for (i = 0; i < MAX_DEVICES; i++)
				{
					pDev = &pMe->mRM.device[i];
					pDev->uValue1 = 0;
				}
			}			
			
			// delete menu since we are going to redraw all menu elements
			IMENUCTL_DeleteAll(pMenu);

			
			//display the devices
			for (MapIndex = 0; MapIndex < pMe->mSD.uNumRecs; MapIndex++)
			{
				MSG_FATAL("***zzg  MapIndex = %d, uNumRecs=%d***", MapIndex, pMe->mSD.uNumRecs, 0);
				
				pMapDev = &pMe->mRM.device[(pMe->mRM.uDevToDspIdxMap[MapIndex])];
				pwName = pMe->pText2;
				
				if ((len = WSTRLEN(pMapDev->wName)) == 0)
				{
					BTApp_BDAddr2Wstr(pwName, &pMe->mSD.devRec[(pMe->mRM.uDevToDspIdxMap[MapIndex])].bdAddr);
				}
				else
				{
					WSTRLCPY( pwName, pMapDev->wName, AEEBT_MAX_DEVICENAME_LEN);

					// check if this is a short name and append "..."
					if ((pMapDev->EIRData.uFlags & AEEBT_EIR_DATA_RCVD_B) && 
						(!(pMapDev->EIRData.uFlags & AEEBT_EIR_NAME_CMPLT_B)))
					{
						MSG_MED( "BldDevRespMenu - short name for DevIndex=%d", pMe->mRM.uDevToDspIdxMap[MapIndex], 0, 0 );
						WSTRLCAT( pwName, wBuf, AEEBT_MAX_DEVICENAME_LEN );
					}
					
				}
				
				if (IMENUCTL_AddItem(pMenu, NULL, 0, MapIndex, pwName, 0) == FALSE)
				{				
					MSG_ERROR( "BldDevRespMenu - failed adding item %d to menu", MapIndex, 0, 0);
				}
				else
				{
					ai.wImage = BTApp_GetDevTypeImageID(pMe, pMe->mSD.devRec[(pMe->mRM.uDevToDspIdxMap[MapIndex])].serviceClass,
														&pMe->mSD.devRec[(pMe->mRM.uDevToDspIdxMap[MapIndex])].bdAddr);
										
					IMENUCTL_SetItem(pMenu, MapIndex, MSIF_IMAGE, &ai);
					
					MSG_MED("BldDevRespMenu - addr=%04x %04x %04x", 
						   ((uint16)(pMapDev->bdAddr.uAddr[ 5 ] << 8) | pMapDev->bdAddr.uAddr[ 4 ]),
						   ((uint16)(pMapDev->bdAddr.uAddr[ 3 ] << 8) | pMapDev->bdAddr.uAddr[ 2 ]),
						   ((uint16)(pMapDev->bdAddr.uAddr[ 1 ] << 8) | pMapDev->bdAddr.uAddr[ 0 ]));
					
					MSG_MED("BldDevRespMenu - rank=%d", pMapDev->EIRData.deviceRanking, 0, 0); 
				}		
				
			}	
			
#else			
			for (i = numItems; i < pMe->mSD.uNumRecs; i ++)
			{
				pDev = &pMe->mRM.device[ i ];
				pDev->bdAddr = pMe->mSD.devRec[ i ].bdAddr;
				pwName = pDev->wName;
				result = EFAILED;
				len    = 0;
				
				if ((pDev->uValue1 == 0) && // GetName not done?
					(((result = IBTEXTRM_DeviceRead( pMe->mRM.po, pDev )) != SUCCESS) ||
					((len = WSTRLEN( pDev->wName )) == 0)))
				{
					if ((pMe->mSD.bDiscovering == FALSE) && 
						(pMe->mRM.uGetNameDevIdx == MAX_DEVICES))
					{
						if (IBTEXTSD_GetDeviceName( pMe->mSD.po, &pDev->bdAddr,pDev->wName, ARR_SIZE( pDev->wName)) == SUCCESS)
						{
							pMe->mRM.uGetNameDevIdx = i;
						}
					}
					
					pwName = pMe->pText2;
					BTApp_BDAddr2Wstr( pwName, &pMe->mSD.devRec[ i ].bdAddr );
				}
				else if ( pDev->uValue1 == UD1_GET_NAME_FAILED ) // GetName failed
				{
					pwName = pMe->pText2;
					BTApp_BDAddr2Wstr( pwName, &pMe->mSD.devRec[ i ].bdAddr );
				}
				else if ( (result == SUCCESS) && (len > 0) )
				{
					pDev->uValue1 = UD1_GET_NAME_SUCCESS;
				}

				if (IMENUCTL_AddItem(pMenu, NULL, 0, i, pwName, 0 ) == FALSE)
				{
					MSG_ERROR( "BldDevRespMenu - failed adding item %d to menu", i, 0, 0 );
				}
				else
				{
					AEEBTBDAddr* pA = &pDev->bdAddr;
					ai.wImage = BTApp_GetDevTypeImageID(pMe, 
														pMe->mSD.devRec[ i ].serviceClass,
														&pMe->mSD.devRec[ i ].bdAddr );
					IMENUCTL_SetItem(pMenu, i, MSIF_IMAGE, &ai );
					MSG_MED( "BldDevRespMenu - addr=%04x %04x %04x", 
					   ((uint16)(pA->uAddr[ 5 ] << 8) | pA->uAddr[ 4 ]),
					   ((uint16)(pA->uAddr[ 3 ] << 8) | pA->uAddr[ 2 ]),
					   ((uint16)(pA->uAddr[ 1 ] << 8) | pA->uAddr[ 0 ]));
				}
			}			
#endif 

			IMENUCTL_SetSel(pMenu, pMe->m_currDlgId);

            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);
            IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);

#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SETTING_BACKGROUND);
#endif
            IMENUCTL_SetBottomBarType(pMenu, BTBAR_SELECT_BACK);


			IMENUCTL_SetSel(pMenu,  MENU_SEL);  // highlight the selected item

			(void)IMENUCTL_Redraw(pMenu);

			// Activate menu
			IMENUCTL_SetActive(pMenu, TRUE);
			IDISPLAY_UpdateEx(pMe->m_pIDisplay, FALSE);
            return TRUE;
        }
        case EVT_DIALOG_END:
            return TRUE;

		//Add By zzg 2011_10_20
		case EVT_KEY:
			return TRUE;
		//Add End
			
		case EVT_KEY_PRESS:
        //case EVT_KEY:
        {
			AEEBTDeviceInfo*    pDev;
			AEEBTDeviceRecord*  pRec;
			boolean             ev_processed = FALSE;

			switch (wParam)
			{
				case AVK_INFO:	
				case AVK_SELECT:
				{
					if (pMe->mSD.bDiscovering)
					{
						IBTEXTSD_StopDeviceDiscovery(pMe->mSD.po);
					}

					MSG_FATAL("***zzg BTApp_HandleSrhResultDlg AVK_INFO***", 0, 0, 0);

					if (IMENUCTL_GetItemCount(pMenu) > 0)
					{
						MENU_SET_SEL(IMENUCTL_GetSel(pMenu));
#ifdef FEATURE_BT_2_1
						pMe->mRM.uCurDevIdx = pMe->mSD.uCurRecIdx = pMe->mRM.uDevToDspIdxMap[MENU_SEL];
						pDev = &pMe->mRM.device[pMe->mRM.uCurDevIdx];
						pRec = &pMe->mSD.devRec[pMe->mSD.uCurRecIdx];
#else
						pMe->mRM.uCurDevIdx = pMe->mSD.uCurRecIdx = MENU_SEL;
						pDev = &pMe->mRM.device[MENU_SEL];
						pRec = &pMe->mSD.devRec[MENU_SEL];
#endif /* FEATURE_BT_2_1 */
						if (pDev->bValid == FALSE)
						{
							if ((pDev->serviceClass == AEEBT_COD_SC_UNKNOWN) &&
							    (pRec->serviceClass != AEEBT_COD_SC_UNKNOWN))
							{
								pDev->serviceClass  = pRec->serviceClass;
								pDev->majorDevClass = pRec->majorDevClass;
								pDev->minorDevClass = pRec->minorDevClass;
							}
						}

						MSG_FATAL("***zzg BTApp_HandleSrhResultDlg uDeviceSrhType=%x***", pMe->uDeviceSrhType, 0, 0);

						if (AEEBT_COD_SC_AUDIO == pMe->uDeviceSrhType)	//Add By zzg 2011_10_19
						{			
							AEEBTDeviceInfo* pDev = &pMe->mRM.device[pMe->mRM.uCurDevIdx];
							boolean bSetBondable = FALSE;

//#ifdef BTAPP_HEADSET_USE_AG															
							//AG Start
							MSG_FATAL("***zzg AG bEnabled=%x, bConnected=%x, devType=%x***", 
										pMe->mAG.bEnabled, pMe->mAG.bConnected, pMe->mAG.devType);

							pMe->mAG.bdAddr = pDev->bdAddr;
							//BTApp_WriteBtAddrConfigFile(pMe);
							//BTApp_WriteConfigFile(pMe);

							if ((TRUE == pMe->mAG.bEnabled) && (FALSE == pMe->mAG.bConnected))
							{
								if (pMe->mAG.bconnInPrgs == FALSE)
								{
									if (pMe->mAG.devType == AEEBT_AG_AUDIO_DEVICE_HEADSET)
									{
										MSG_MED("HndlSlction - connecting to HS", 0, 0, 0);
									}
									else
									{
										MSG_MED("HndlSlction - connecting to HF", 0, 0, 0);
									}
									
									if (BTApp_CallConnected(pMe) != BT_APP_CALL_NONE)
									{
										pMe->mAG.bDevPickedUp = TRUE; // signal self to send audio to HS/HF
									}
									pMe->mAG.bconnInPrgs = TRUE;
									IBTEXTAG_Connect(pMe->mAG.po, &pDev->bdAddr, pMe->mAG.devType);
									BTApp_ShowBusyIcon(pMe);
								}
								else
								{
									BTApp_ShowMessage( pMe, IDS_AG_CONNECTION_IN_PRGS, NULL, 3 );
								}				
							}
							//AG End
//#endif

							
//#ifdef BTAPP_HEADSET_USE_A2DP			
							/*
							//A2DP Start							
							MSG_FATAL("***zzg A2DP bEnabled=%x, bEnableA2DP=%x, bConnected=%x***", 
										pMe->mA2DP.bEnabled, pMe->mA2DP.bEnableA2DP, pMe->mA2DP.bConnected);
							
							pMe->mA2DP.bdAddr = pMe->mRM.device[pMe->mRM.uCurDevIdx].bdAddr;
							
							DBGPRINTF("***zzg SearchRst mA2DP:%02x%02x%02x%02x%02x%02x***", 
										(uint16)(pMe->mA2DP.bdAddr.uAddr[5]), (uint16)(pMe->mA2DP.bdAddr.uAddr[4]),
					               		(uint16)(pMe->mA2DP.bdAddr.uAddr[3]), (uint16)(pMe->mA2DP.bdAddr.uAddr[2]), 
					               		(uint16)(pMe->mA2DP.bdAddr.uAddr[1]), (uint16)(pMe->mA2DP.bdAddr.uAddr[0]));
							
							pMe->bConfigChanged = TRUE;
						    IBTEXTA2DP_SetDevice(pMe->mA2DP.po, &pMe->mA2DP.bdAddr);	

							//Add By zzg 2011_11_03
							if (pMe->bConfigChanged)
							{								
								BTApp_WriteBtAddrConfigFile(pMe);
							}							
							//Add End

							if ((pMe->mA2DP.bEnabled == TRUE) && (pMe->mA2DP.bConnected == FALSE))
							{
								if ( IBTEXTA2DP_Connect( pMe->mA2DP.po, &pMe->mA2DP.bdAddr ) != SUCCESS )
								{
									BTApp_ShowMessage( pMe, IDS_MSG_CONN_FAILED, NULL, 3 );
								}  	
								else
								{
									BTApp_A2DPSetRetries(pMe, TRUE);
								}
							}
							//A2DP End
							*/
//#endif							
							
							break;
						}
						else
						{
							pMe->m_dialog_id = IDD_SEARCH_RESULT;	//用以区分方便返回到对应的DIALOG
							CLOSE_DIALOG(DLGRET_DEVICEINFO)			    
						}
				  	}
				 
					break;
				}
				
				case AVK_CLR:
				{
					if (pMe->mSD.bDiscovering)
					{
						MSG_FATAL("***zzg HandleSrhResultDlg AVK_CLR***", 0, 0, 0);
						
						IBTEXTSD_StopDeviceDiscovery(pMe->mSD.po);
						BTApp_CancelDevNameRequest(pMe);

						//AEEBT_SD_EVT_DEVICE_DISCOVERY_STOPPED will make a Msg_Box
						return TRUE;	
					}

					BTApp_CancelDevNameRequest(pMe);

					CLOSE_DIALOG(DLGRET_CANCELED)
					return TRUE;
					//break;
				}
				
				default:
				{
					break;
				}
			}

			break;
		
		}
		
        default:
        {
			break;
        }
    }
	
    return FALSE;

}	

static boolean HandleDeviceInfoDialogEvent(CBTApp *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{	 
	PARAM_NOT_REF(dwParam)

    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_BT_DEVICE_INFO);
	IStatic  *pStatic = (IStatic*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_STATIC_BT_DEVICE_INFO);

	
    if ((pMenu == NULL) || (pStatic == NULL))
    {
        return FALSE;
    }	

	MSG_FATAL("***zzg HandleDeviceInfoDialogEvent eCode=%x, wParam=%x ,dwParam=%x***",eCode,wParam,dwParam);   

    switch (eCode)
    {
        case EVT_DIALOG_INIT:			
		{
			//IDIALOG_SetProperties((IDialog *)dwParam, DLG_NOT_REDRAW_AFTER_START);			
            return TRUE;
		}

        case EVT_DIALOG_START:   
        {			
			IMENUCTL_SetSel(pMenu, pMe->m_currDlgId);
			IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);
			IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);

#ifdef FEATURE_CARRIER_CHINA_VERTU
			IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SETTING_BACKGROUND);
#endif
			IMENUCTL_SetBottomBarType(pMenu,BTBAR_OPTION_BACK);

            (void) ISHELL_PostEvent( pMe->m_pShell,
                                     AEECLSID_BLUETOOTH_APP,
                                     EVT_USER_REDRAW,
                                     0,
                                     0);     
						
            return TRUE;
        }
        case EVT_USER_REDRAW:    
		{
			AEERect				rc;
			AECHAR				WTitle[40] = {0};	
			uint16				uLen = 0;
			uint16				tempuValue1 = 0;    
			//CtlAddItem        	ai;			
			AEEBTDeviceInfo*  	pDev = &pMe->mRM.device[pMe->mRM.uCurDevIdx];
			
#ifdef FEATURE_BT_2_1
			uint8 				maxManuDataLen = AEEBT_MAX_EIR_MANUF_DATA_LEN;
			uint8             	pManuData[ AEEBT_MAX_EIR_MANUF_DATA_LEN ];
			char              	szManuData[ AEEBT_MAX_EIR_MANUF_DATA_LEN + 1 ] = {0};
			AECHAR            	wManuData[ AEEBT_MAX_EIR_MANUF_DATA_LEN + 1 ] = {0};
			int               	i;
#endif 			

			(void)ISHELL_LoadResString( pMe->m_pShell,
										AEE_APPSBTAPP_RES_FILE, 							   
										IDS_DEVICES,
										WTitle,
										sizeof(WTitle));	
			 
			IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, WTitle);	
			
			SETAEERECT (&rc, pMe->m_rect.x, 
  			            pMe->m_rect.y, 
  			            pMe->m_rect.dx, 
  			            pMe->m_rect.dy - BOTTOMBAR_HEIGHT);
			          
			ISTATIC_SetRect(pStatic, &rc);

			ISTATIC_SetProperties(pStatic, ISTATIC_GetProperties(pStatic)& ~ST_MIDDLETEXT & ~ST_CENTERTEXT);    			   
			ISTATIC_SetProperties(pStatic, ST_NOSCROLL|ST_GRAPHIC_BG);  
			ISTATIC_SetBackGround(pStatic, AEE_APPSCOMMONRES_IMAGESFILE, IDB_BACKGROUND); 

			// save the value of get name status
			tempuValue1 = pDev->uValue1;

			MSG_FATAL("***zzg DeviceInfo tempuValue1=%d***", tempuValue1, 0, 0);
			
			MSG_MED( "BTApp_BuildDevInfo - tempuValue1 = %d", tempuValue1, 0, 0);

			IBTEXTRM_DeviceRead(pMe->mRM.po, pDev);	

			MSG_FATAL("***zzg DeviceInfo 1 WSTRLEN(pDev->wName)=%d***", WSTRLEN(pDev->wName), 0, 0);

			
			//Print the log Start
			/*
			if (WSTRLEN(pDev->wName) > 0)
			{
				int temp;
				char btname[AEEBT_MAX_DEVICENAME_LEN+1];
				char btnickname[AEEBT_MAX_NICKNAME_LEN+1];

				WSTRTOSTR (pDev->wName, btname, sizeof(char)*(AEEBT_MAX_DEVICENAME_LEN+1));	
				WSTRTOSTR (pDev->wNickName, btnickname, sizeof(char)*(AEEBT_MAX_NICKNAME_LEN+1));	

				DBGPRINTF("***zzg DeviceInfoDlgEvt 1 btname=%s***", btname);
				DBGPRINTF("***zzg DeviceInfoDlgEvt 1 btnickname=%s***", btnickname);

				for (temp=0; temp<STRLEN(btname); temp++)
				{
					DBGPRINTF("***zzg DeviceInfoDlgEvt 1 btname[%d]=%c***", temp, btname[temp]);
				}
			}	
			*/
			//Print the log  End

			// restore the value of get name status      
			pDev->uValue1 = tempuValue1;		
			
			if (pDev->uValue1 == 0)
			{
			  // BT name or Short Name
			  if ((WSTRLEN(pDev->wName) == 0) && (pMe->mRM.uGetNameDevIdx == MAX_DEVICES))
			  {			  
			  	MSG_FATAL("***zzg DeviceInfoDlgEvt IBTEXTSD_GetDeviceName***", 0, 0, 0);
				
			    if (IBTEXTSD_GetDeviceName(pMe->mSD.po, 
											&pDev->bdAddr,
			                                pDev->wName, 
			                                ARR_SIZE(pDev->wName)) == SUCCESS)
			    {
			    	pMe->mRM.uGetNameDevIdx = pMe->mRM.uCurDevIdx;
			    }


				MSG_FATAL("***zzg DeviceInfo 2 WSTRLEN(pDev->wName)=%d***", WSTRLEN(pDev->wName), 0, 0);

				//Print the log Start
				/*
				if (WSTRLEN(pDev->wName) > 0)
				{
					int temp;
					char btname[AEEBT_MAX_DEVICENAME_LEN+1];
					char btnickname[AEEBT_MAX_NICKNAME_LEN+1];

					WSTRTOSTR (pDev->wName, btname, sizeof(char)*(AEEBT_MAX_DEVICENAME_LEN+1));	
					WSTRTOSTR (pDev->wNickName, btnickname, sizeof(char)*(AEEBT_MAX_NICKNAME_LEN+1));	

					DBGPRINTF("***zzg DeviceInfoDlgEvt 2 btname=%s***", btname);
					DBGPRINTF("***zzg DeviceInfoDlgEvt 2 btnickname=%s***", btnickname);

					for (temp=0; temp<STRLEN(btname); temp++)
					{
						DBGPRINTF("***zzg DeviceInfoDlgEvt 2 btname[%d]=%c***", temp, btname[temp]);
					}

				}
				*/
				//Print the log  End
				
			  }
			  else if (WSTRLEN( pDev->wName ) == 0)
			  {
			    // schedule to get name when current get name session is done        
				//ISHELL_SetTimer( pMe->a.m_pIShell, 500,(PFNNOTIFY) BTApp_RefreshDevInfo, pMe );
				ISHELL_SetTimer(pMe->m_pShell, 500,(PFNNOTIFY) BTApp_UpdateDeviceInfo, pMe);				
			  }
			}
			else
			{
			  pDev->uValue1 = 0;
			}
			
			
#ifdef FEATURE_BT_2_1
			// manufacturer data
			if ( pDev->EIRData.uManufDataSize > 0)
			{
				//this means that manufacturer data is present 
				if ( ( pDev->EIRData.uManufDataSize > AEEBT_MAX_EIR_MANUF_DATA_LEN ) || 
				     ( IBTEXTRM_GetEIRManufData( pMe->mRM.po, &pDev->bdAddr,
				                                 &maxManuDataLen, 
				                                 pManuData ) != SUCCESS ) )  
				{
				  MSG_ERROR( "DevInfo - Manufacturer Data failed", 0, 0, 0 );
				} 
				else
				{
				  for ( i = 0; i < maxManuDataLen; i++ )
				  {
				    SNPRINTF((szManuData + STRLEN(szManuData)), sizeof(szManuData) - STRLEN(szManuData), "%c", pManuData[i]);
				  }
				  STRTOWSTR( (char*)szManuData, wManuData, sizeof(wManuData) );
				}
			}
			
			MSG_FATAL("***zzg DeviceInfoDlg pDev->EIRData.uFlags=%x***", pDev->EIRData.uFlags, 0, 0);
			
			if ((pDev->EIRData.uFlags & AEEBT_EIR_DATA_RCVD_B) && (!(pDev->EIRData.uFlags & AEEBT_EIR_NAME_CMPLT_B)))
			{
				uLen += BTApp_FormatBTShortName( pMe, &pMe->pText1[ uLen], 
			                          			 LONG_TEXT_BUF_LEN - uLen, 
			                          			 pDev->wName, FALSE );

			}
			else
			{
				uLen += BTApp_FormatBTName( pMe, &pMe->pText1[ uLen], 
			                                LONG_TEXT_BUF_LEN - uLen, 
			                                pDev->wName); 				
			}
#else
			uLen += BTApp_FormatBTName( pMe, &pMe->pText1[ uLen], 
			                          LONG_TEXT_BUF_LEN - uLen, pDev->wName );
#endif /* FEATURE_BT_2_1 */

#ifdef FEATURE_BT_2_1
			uLen += BTApp_FormatSSPCapable( pMe, &pMe->pText1[ uLen], 
			                                LONG_TEXT_BUF_LEN - uLen, 
			                                pDev->bSSPCapable );

			//Use linkKeyStatus which is now avaiable in AEEBTDeviceInfo
			MSG_HIGH( "FormatLinkKey", 0, 0, 0 );
			MSG_HIGH( "FormatLinkKey %d", pDev->linkKeyStatus, 0, 0 );
			uLen += BTApp_FormatLinkKey( pMe, &pMe->pText1[ uLen], 
			                             LONG_TEXT_BUF_LEN - uLen, 
			                             pDev->linkKeyStatus );
#endif /* FEATURE_BT_2_1 */

			// BD address
			uLen += BTApp_FormatBDAddress(pMe, &pMe->pText1[ uLen], 
			                              LONG_TEXT_BUF_LEN - uLen, &pDev->bdAddr );

			
			MSG_FATAL("***zzg DeviceInfo mRM.uCurDevIdx=%d, security=%d***", pMe->mRM.uCurDevIdx, pDev->security, 0);

			// Security Level
			uLen += BTApp_FormatSecurity(pMe, &pMe->pText1[ uLen], 
			                             LONG_TEXT_BUF_LEN - uLen, pDev->security );
			
			// Service Class
			uLen += BTApp_FormatSvcCls(pMe, &pMe->pText1[ uLen], 
			                           LONG_TEXT_BUF_LEN - uLen, pDev->serviceClass );
			// LMP features
			if (pDev->bValid == FALSE)
			{
				pDev->uLMPFeatures[ 0 ] = 0;
				pDev->uLMPFeatures[ 1 ] = 0;
				pDev->uLMPFeatures[ 2 ] = 0;
			}
			uLen += BTApp_FormatLMPFeatures(pMe, &pMe->pText1[ uLen], 
			                                LONG_TEXT_BUF_LEN - uLen, 
			                                pDev->uLMPFeatures);
#ifdef FEATURE_BT_2_1

			if (pMe->bRemoteOOBRead == TRUE)
			{
				if (BTApp_RemoteOOBWrite ( pMe, pDev) == SUCCESS)
				{
				  uLen += BTApp_FormatOOBDataStr (pMe,&pMe->pText1[ uLen], 
				                        		  LONG_TEXT_BUF_LEN - uLen, TRUE );
				}
				else
				{
				  uLen += BTApp_FormatOOBDataStr (pMe,&pMe->pText1[ uLen], 
				                        		  LONG_TEXT_BUF_LEN - uLen, FALSE );
				}
			                        
			}

			//manu data
			if (WSTRLEN (wManuData) > 0 )
			{
				uLen += BTApp_FormatManuData(pMe, &pMe->pText1[ uLen], 
			                              	 LONG_TEXT_BUF_LEN - uLen,
			                                 wManuData);
			}
			// UUIDs
			uLen += BTApp_FormatUUID(pMe, &pMe->pText1[ uLen], 
			                         LONG_TEXT_BUF_LEN - uLen,
			                         pDev);
#endif /* FEATURE_BT_2_1 */

			// display text
			ISTATIC_SetText(pStatic, NULL, pMe->pText1, AEE_FONT_BOLD, AEE_FONT_NORMAL );
			ISTATIC_Redraw(pStatic);			
			
			//IMENUCTL_SetActive(pMenu, TRUE );	
			ISTATIC_SetActive(pStatic, TRUE);			
			IDISPLAY_UpdateEx( pMe->m_pIDisplay, FALSE); 			
            return TRUE;

		}
		
        case EVT_DIALOG_END:
        {
			return TRUE;
        }

        case EVT_KEY:
        {
			switch(wParam)
            {
                case AVK_CLR:
                {
					CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;
                }	
					
				case AVK_INFO:
				case AVK_SELECT:
				{
					CLOSE_DIALOG(DLGRET_DEVICEINFO_OPITION)
					return TRUE;
				}	
				
                default:
                {
					break;
                }
            }
            return TRUE;
        }
		
        default:
        {
			break;
        }
    }
    return FALSE;
}	

static boolean HandleDeviceInfoOpitionDialogEvent(CBTApp *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{	 
	PARAM_NOT_REF(dwParam)

    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_BT_DEVICE_INFO_OPITION);

	AEEBTDeviceInfo*  pDev = &pMe->mRM.device[ pMe->mRM.uCurDevIdx ];
	
    if (pMenu == NULL)
    {
        return FALSE;
    }
	
    MSG_FATAL("***zzg HandleDeviceInfoOpitionDialogEvent eCode=%x, wParam=%x ,dwParam=%x***",eCode,wParam,dwParam);   

    switch (eCode)
    {
        case EVT_DIALOG_INIT:			
		{			
            return TRUE;
		}

        case EVT_DIALOG_START:       
        {
			IMENUCTL_SetSel(pMenu, pMe->m_currDlgId);

            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);
            IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);

#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SETTING_BACKGROUND);
#endif
            IMENUCTL_SetBottomBarType(pMenu, BTBAR_OK_BACK);

            (void) ISHELL_PostEvent( pMe->m_pShell,
                                     AEECLSID_BLUETOOTH_APP,
                                     EVT_USER_REDRAW,
                                     0,
                                     0);
            return TRUE;
        }	

        case EVT_USER_REDRAW:    			
        {
			AECHAR 		WTitle[40] = {0};			

			(void)ISHELL_LoadResString(pMe->m_pShell,
				                        AEE_APPSBTAPP_RES_FILE,                                
				                        IDS_OPTIONS,
				                        WTitle,
				                        sizeof(WTitle));
			
			IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, WTitle);   

			IBTEXTRM_DeviceRead(pMe->mRM.po, pDev);		//Add By zzg 2011_06_07 Update the DeviceInfo		

			IMENUCTL_DeleteAll(pMenu);
			
			if (pMe->m_dialog_id == IDD_DEVICE_LIST) 				
			{
				uint8 i = 0;
				boolean bIsAudioDev = FALSE;

				// find out if this is an audio device
				if ((pDev->majorDevClass == AEEBT_COD_MAJ_DEV_CLS_AUDIO) 
					|| (pDev->serviceClass & AEEBT_COD_SC_AUDIO))
				{
					bIsAudioDev = TRUE;
				}
				else
				{
					while ((i<pDev->uNumSvcs) && (bIsAudioDev == FALSE))
					{
						if ((pDev->svcs[i].uServiceClass == AEEBT_SD_SERVICE_CLASS_HEADSET) 
							||(pDev->svcs[i].uServiceClass == AEEBT_SD_SERVICE_CLASS_HANDSFREE))
						{
							bIsAudioDev = TRUE;
						}
						i++;
					}					
					
				}

				MSG_FATAL("***zzg A2DP bIsAudioDev=%x, bEnabled=%x, bConnected=%x***", 
								bIsAudioDev, pMe->mA2DP.bEnabled, pMe->mA2DP.bConnected);

				MSG_FATAL("***zzg pDev:%04x %04x %04x***", 
					((uint16)(pDev->bdAddr.uAddr[ 5 ] << 8) | pDev->bdAddr.uAddr[ 4 ]),
               		((uint16)(pDev->bdAddr.uAddr[ 3 ] << 8) | pDev->bdAddr.uAddr[ 2 ]),
               		((uint16)(pDev->bdAddr.uAddr[ 1 ] << 8) | pDev->bdAddr.uAddr[ 0 ]));
				

				MSG_FATAL("***zzg mA2DP:%04x %04x %04x***", 
					((uint16)(pMe->mA2DP.bdAddr.uAddr[ 5 ] << 8) | pMe->mA2DP.bdAddr.uAddr[ 4 ]),
               		((uint16)(pMe->mA2DP.bdAddr.uAddr[ 3 ] << 8) | pMe->mA2DP.bdAddr.uAddr[ 2 ]),
               		((uint16)(pMe->mA2DP.bdAddr.uAddr[ 1 ] << 8) | pMe->mA2DP.bdAddr.uAddr[ 0 ]));

				if (bIsAudioDev != FALSE)
				{
					// only Use, Connect, and Disconnect audio device
#ifdef BTAPP_HEADSET_USE_AG					
					//AG Start
					if (pMe->mAG.bEnabled == FALSE)					
					{
						IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, IDS_USE, IDS_USE, NULL, 0);						
					}
					else if (pMe->mAG.bConnected == FALSE)
					{
						IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, IDS_CONNECT, IDS_CONNECT, NULL, 0);						
					}
					else if (AEEBT_BD_ADDRS_EQUAL( &pMe->mAG.bdAddr, &pDev->bdAddr) != FALSE)
					{					
						IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, IDS_DISCONNECT, IDS_DISCONNECT, NULL, 0);			
					}
					//AG End
#endif

#ifdef BTAPP_HEADSET_USE_A2DP
					//A2DP Start
					if (pMe->mA2DP.bEnabled == FALSE)					
					{
						IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, IDS_USE, IDS_USE, NULL, 0);						
					}
					else if (pMe->mA2DP.bConnected == FALSE)
					{
						IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, IDS_CONNECT, IDS_CONNECT, NULL, 0);						
					}
					else if (AEEBT_BD_ADDRS_EQUAL( &pMe->mA2DP.bdAddr, &pDev->bdAddr) != FALSE)
					{					
						IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, IDS_DISCONNECT, IDS_DISCONNECT, NULL, 0);			
					}
#endif					
					//A2DP End
				}
				
				IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, pDev->bBonded ? IDS_UNBOND : IDS_BOND, pDev->bBonded ? IDS_UNBOND : IDS_BOND, NULL, 0);				
#ifdef FEATURE_BT_2_1			
				IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, IDS_BOND_OPTIONS, IDS_BOND_OPTIONS, NULL, 0);			
				IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, IDS_READ_OOB, IDS_READ_OOB, NULL, 0);
#endif
				IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, IDS_DISCARD, IDS_DISCARD, NULL, 0);

			}
			else 	//if (pMe->m_dialog_id == IDD_SEARCH_RESULT) 
			{				
				IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, pDev->bValid? IDS_DISCARD : IDS_KEEP, pDev->bValid? IDS_DISCARD : IDS_KEEP, NULL, 0);
				IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, pDev->bBonded ? IDS_UNBOND : IDS_BOND, pDev->bBonded ? IDS_UNBOND : IDS_BOND, NULL, 0);
#ifdef FEATURE_BT_2_1			
				IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, IDS_BOND_OPTIONS, IDS_BOND_OPTIONS, NULL, 0);			
				//IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, IDS_READ_OOB, IDS_READ_OOB, NULL, 0);
#endif			
				//暂时屏蔽，功能待添加
				//IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, IDS_OPTIONS, IDS_OPTIONS, NULL, 0);
			}
			
			(void)IMENUCTL_Redraw(pMenu);
            return TRUE;
        }

        case EVT_DIALOG_END:
        {
			return TRUE;
        }

        case EVT_KEY:
        {
			switch(wParam)
            {
                case AVK_CLR:
                {
					CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;
                }	

                default:
                {
					break;
                }	
            }
            return TRUE;
        }	
		
        case EVT_COMMAND:           
        {
			pMe->m_currDlgId = wParam;	
			
            switch (wParam)
            {							
				case IDS_KEEP:
				{
					pDev->bValid = TRUE;
          			IBTEXTRM_DeviceAdd(pMe->mRM.po, pDev);	//BTApp.c里会收到RSP ，然后做响应
					return TRUE;
				}
				case IDS_DISCARD:
				{					
					BTApp_BuildPrompt(pMe, BT_APP_MENU_REMOVE_ONE);										
					return TRUE;	
				}
				case IDS_BOND:
				{
					pMe->mRM.bBonding = TRUE;

#ifdef FEATURE_BT_2_1
					if (IBTEXTRM_GetLocalInfo(pMe->mRM.po,&pMe->mRM.myInfo) != SUCCESS)
					{
						MSG_FATAL("***zzg IBTEXTRM_GetLocalInfo Failed!***", 0, 0, 0);						
					}
					else
					{
						MSG_LOW("Local HC LMPVersion = %d",pMe->mRM.myInfo.uLMPVersion, 0, 0);
						
						MSG_FATAL("***zzg Local HC LMPVersion = %d***", pMe->mRM.myInfo.uLMPVersion, 0, 0);

						if (pMe->mRM.myInfo.uLMPVersion > BTAPPDLG_BT_LMP_VER_2_0)	
						{
							// Host is 2.1 enabled and Host Controller is 2.1      		  
							BTApp_SetBondable(pMe);//ACG
							BTApp_BuildBondMenu(pMe);
						}
						else
						{
							// Host is 2.1 but, Host Controller is non 2.1         
							if (WSTRLEN(pMe->mRM.device[pMe->mRM.uCurDevIdx].wName) == 0)
							{
								BTApp_BDAddr2Wstr(pMe->mRM.device[pMe->mRM.uCurDevIdx].wName,&pMe->mRM.device[pMe->mRM.uCurDevIdx].bdAddr);
							}

							WSTRLCPY(pMe->wEditBuf, pMe->mRM.device[pMe->mRM.uCurDevIdx].wName, ARR_SIZE(pMe->mRM.device[pMe->mRM.uCurDevIdx].wName));
						
							pMe->m_edit_id = IDS_PASS_KEY;
							pMe->m_bEditNeedStr = TRUE;
							pMe->m_edit_state_id = BTAPPST_DEVICEINFO;							
							CLOSE_DIALOG(DLGRET_BT_EDIT)  
						}
					
					}
#endif 					
					return TRUE;
				}	
				case IDS_UNBOND:
				{
					BTApp_BuildPrompt(pMe, BT_APP_MENU_UNBOND_ONE);
					return TRUE;
				}
				case IDS_BOND_OPTIONS:
				{
					CLOSE_DIALOG(DLGRET_BOND_OPITION)                   
					return TRUE;
				}	
				case IDS_READ_OOB:
				{
					pMe->bRemoteOOBRead = TRUE; 					
					CLOSE_DIALOG(DLGRET_CANCELED) 						
					pMe->bRemoteOOBRead = FALSE; 
					return TRUE;	
				}	

				
				case IDS_USE:
				{
					BTApp_BuildPrompt(pMe, BT_APP_MENU_USE_AUDIO_DEV);
					return TRUE;
				}

				case IDS_CONNECT:
				{
					BTApp_BuildPrompt(pMe, BT_APP_MENU_AG_CONNECT);
					return TRUE;
				}

				case IDS_DISCONNECT:
				{
					BTApp_BuildPrompt(pMe, BT_APP_MENU_AG_DISCONNECT);
					return TRUE;
				}
            	case IDS_OPTIONS:						
                {
					return TRUE;
            	}	
				default:
				{
					ASSERT_NOT_REACHABLE;
				}
            }
            return TRUE;
        }
		
        default:
        {
			break;
        }
    }
    return FALSE;
}	

static boolean  HandleBondOpitionDialogEvent(CBTApp * pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{	 
	PARAM_NOT_REF(dwParam)
		
	IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_BT_BOND_OPITION);

	if (pMenu == NULL)
	{
		return FALSE;
	}	

	MSG_FATAL("***zzg HandleBondOpitionDialogEvent eCode=%x, wParam=%x ,dwParam=%x***",eCode,wParam,dwParam); 

	switch (eCode)
	{
		case EVT_DIALOG_INIT:			
		{
			AECHAR		WTitle[40] = {0};		
			
			(void)ISHELL_LoadResString(pMe->m_pShell,
										AEE_APPSBTAPP_RES_FILE, 							   
										IDS_BOND_OPTIONS,
										WTitle,
										sizeof(WTitle));
			
			IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, WTitle);  	
			
			IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, IDS_MITM, IDS_MITM, NULL, 0);		
			IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, IDS_REBOND_OPT_MITM, IDS_REBOND_OPT_MITM, NULL, 0);		
			IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, IDS_REBOND_OPT_BOND, IDS_REBOND_OPT_BOND, NULL, 0);	
			return TRUE;
		}

		case EVT_DIALOG_START:
		{
			//设定标题格式
            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_MULTI_SEL);
            IMENUCTL_SetOemProperties(pMenu, OEMMP_DISTINGUISH_INFOKEY_SELECTKEY | OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, 
IDI_SECURITY_BACKGROUND);
#endif
            IMENUCTL_SetBottomBarType(pMenu,BTBAR_SAVE_BACK);

			MSG_FATAL("***zzg SetBondOpitionDlg bMITMEnabled=%d***", pMe->mRM.bMITMEnabled, 0, 0);
			MSG_FATAL("***zzg SetBondOpitionDlg bRebondOptMITMEnabled=%d***", pMe->mRM.bRebondOptMITMEnabled, 0, 0);
			MSG_FATAL("***zzg SetBondOpitionDlg bRebondOptBONDEnabled=%d***", pMe->mRM.bRebondOptBONDEnabled, 0, 0);

			SetCheckBoxItem(pMenu, IDS_MITM, pMe->mRM.bMITMEnabled ? TRUE : FALSE);
			SetCheckBoxItem(pMenu, IDS_REBOND_OPT_MITM, pMe->mRM.bRebondOptMITMEnabled ? TRUE : FALSE);
			SetCheckBoxItem(pMenu, IDS_REBOND_OPT_BOND, pMe->mRM.bRebondOptBONDEnabled ? TRUE : FALSE);

			IMENUCTL_SetSel(pMenu, IDS_MITM);
			
			(void) ISHELL_PostEvent( pMe->m_pShell,
									 AEECLSID_BLUETOOTH_APP,
									 EVT_USER_REDRAW,
									 0,
									 0);
			return TRUE;
		}
		
		case EVT_USER_REDRAW:		
		{			
			//(void)IMENUCTL_Redraw(pMenu);		   //dele by yangdecai	
			return TRUE;
		}

		case EVT_DIALOG_END:
		{
			return TRUE;
		}

		case EVT_KEY_PRESS: 	  
		{					
			uint16 selection = IMENUCTL_GetSel(pMenu);
			MENU_SET_SEL(selection);
			
			MSG_FATAL("***zzg HandleBondOpitionDialogEvent EVT_KEY_PRESS wParam=%x***", wParam, 0, 0);
			
			switch(wParam)
			{
				case AVK_SELECT:	
				{
					pMe->bConfigChanged = TRUE;

					if (GetCheckBoxVal(pMenu, IDS_MITM))
					{
						pMe->mRM.bMITMEnabled = TRUE;						
					}
					else
					{
						pMe->mRM.bMITMEnabled = FALSE;		
					}

					if (GetCheckBoxVal(pMenu, IDS_REBOND_OPT_MITM))
					{
						pMe->mRM.bRebondOptMITMEnabled = TRUE;						
					}
					else
					{
						pMe->mRM.bRebondOptMITMEnabled = FALSE;				
					}

					if (GetCheckBoxVal(pMenu, IDS_REBOND_OPT_BOND))
					{
						pMe->mRM.bRebondOptBONDEnabled = TRUE;						
					}
					else
					{
						pMe->mRM.bRebondOptBONDEnabled = FALSE;				
					}

					
					CLOSE_DIALOG(DLGRET_CANCELED)
					return TRUE;
				}				
				
				case AVK_CLR:
					CLOSE_DIALOG(DLGRET_CANCELED)
					return TRUE;

				default:
					break;
			}
			
			 return TRUE;
		}
		
		default:
			break;
	}

	return FALSE;		 

}



static boolean HandleDeviceDialogEvent(CBTApp *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{	 
	PARAM_NOT_REF(dwParam)

    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_DEVICE);

	//AEEBTDeviceInfo*  pDev = &pMe->mRM.device[pMe->mRM.uCurDevIdx];
	
    if (pMenu == NULL)
    {
        return FALSE;
    }

	MSG_FATAL("***zzg HandleDeviceDialogEvent eCode=%x, wParam=%x ,dwParam=%x***",eCode,wParam,dwParam); 	

    switch (eCode)
    {
        case EVT_DIALOG_INIT:			
		{			
			AECHAR 		WTitle[40] = {0};			

			(void)ISHELL_LoadResString(pMe->m_pShell,
				                        AEE_APPSBTAPP_RES_FILE,                                
				                        IDS_DEVICES,
				                        WTitle,
				                        sizeof(WTitle));
			
			IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, WTitle);  
		
			IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, IDS_LIST_BONDED, IDS_LIST_BONDED, NULL, 0);
  			IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, IDS_LIST_ALL, IDS_LIST_ALL, NULL, 0);		
			IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, IDS_LIST_CONNECTED, IDS_LIST_CONNECTED, NULL, 0);			
            IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, IDS_UNBOND_ALL, IDS_UNBOND_ALL, NULL, 0);			
            IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, IDS_REMOVE_ALL, IDS_REMOVE_ALL, NULL, 0);

            return TRUE;
		}

        case EVT_DIALOG_START:       
        {
			IMENUCTL_SetSel(pMenu, pMe->m_currDlgId);

            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);
            IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);

#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SETTING_BACKGROUND);
#endif
            IMENUCTL_SetBottomBarType(pMenu, BTBAR_OK_BACK);

            (void) ISHELL_PostEvent( pMe->m_pShell,
                                     AEECLSID_BLUETOOTH_APP,
                                     EVT_USER_REDRAW,
                                     0,
                                     0);
            return TRUE;
        }
		
        case EVT_USER_REDRAW:    			
        {
			//(void)IMENUCTL_Redraw(pMenu);    //dele by yangdecai
            return TRUE;
        }

        case EVT_DIALOG_END:
        {
			return TRUE;
        }

        case EVT_KEY:
        {
			switch(wParam)
            {
                case AVK_CLR:
                {
					CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;
                }

                default:
                {
					break;
                }
            }
            return TRUE;
        }
		
        case EVT_COMMAND:           
        {
			pMe->m_currDlgId = wParam;	
			pMe->m_dialog_id = IDD_DEVICE_LIST;

			switch (wParam)
            {
				case IDS_LIST_BONDED:
				{		
					AEEBTDeviceEnumerator enumerator;
					AEEBTDeviceInfo*	  pDev;		
								
					uint8				  i = 0;
					
					enumerator.control = AEEBT_RM_EC_MATCH_BONDED;
					enumerator.bBonded = TRUE;							
		
					if (IBTEXTRM_DeviceEnumInit(pMe->mRM.po, &enumerator) == SUCCESS)
					{		
						pDev = &pMe->mRM.device[i];
		
						while ((IBTEXTRM_DeviceEnumNext( pMe->mRM.po, pDev) == SUCCESS) 
								&& pDev->bValid && (i < MAX_DEVICES))
						{								
							pDev = &pMe->mRM.device[++i];
						}		
					}

					if (i > 0)
					{
						MSG_FATAL("***zzg DeviceList IBTEXTRM_DeviceEnumInit i=%d***", i, 0, 0);	
						pMe->m_device_list_id = IDS_LIST_BONDED;
						CLOSE_DIALOG(DLGRET_DEVICE_LIST)  						
					}
					else
					{
						BTApp_ShowMessage(pMe, IDS_MSG_NO_BONDED_DEV, NULL, 0);						
					}
					
					return TRUE;
					
				}	
				case IDS_LIST_ALL:
				{				
					AEEBTDeviceEnumerator enumerator;
					AEEBTDeviceInfo*      pDev;
					
					uint8                 i = 0;

					enumerator.control = AEEBT_RM_EC_ALL;	
					
					if (IBTEXTRM_DeviceEnumInit(pMe->mRM.po, &enumerator) == SUCCESS)
					{
						pDev = &pMe->mRM.device[i];

						while ((IBTEXTRM_DeviceEnumNext(pMe->mRM.po, pDev) == SUCCESS) 
								&& pDev->bValid && (i < MAX_DEVICES))
						{
							pDev = &pMe->mRM.device[ ++i ];
						}
					}
					
					if (i > 0)
					{
						MSG_FATAL("***zzg DeviceList IBTEXTRM_DeviceEnumInit i=%d***", i, 0, 0);
						//CLOSE_DIALOG(DLGRET_ALL_LIST)   
						pMe->m_device_list_id = IDS_LIST_ALL;
						CLOSE_DIALOG(DLGRET_DEVICE_LIST) 
					}
					else
					{
						BTApp_ShowMessage(pMe, IDS_MSG_NO_DEV, NULL, 0);						
					}
					
					return TRUE;
				}		
				case IDS_LIST_CONNECTED:
				{				
					AEEBTDeviceEnumerator enumerator;
					AEEBTDeviceInfo*      pDev;
															
					uint8                 i = 0;

					enumerator.control = AEEBT_RM_EC_ALL;
					
					if (IBTEXTRM_DeviceEnumInit(pMe->mRM.po, &enumerator) == SUCCESS)
					{
						pDev = &pMe->mRM.device[i];

						while ((IBTEXTRM_DeviceEnumNext(pMe->mRM.po, pDev) == SUCCESS) 
								&& pDev->bValid && (i < MAX_DEVICES))
						{				
							pMe->mRM.linkStatus->bdAddr = pDev->bdAddr;
							
							if (IBTEXTRM_GetLinkStatus(pMe->mRM.po, pMe->mRM.linkStatus) != SUCCESS)
							{
								// dev not connected to us; skip it
								continue;
							}	
							
							pDev = &pMe->mRM.device[++i];
						}
					}

					if (i > 0)
					{
						MSG_FATAL("***zzg DeviceList IBTEXTRM_DeviceEnumInit i=%d***", i, 0, 0);
						//CLOSE_DIALOG(DLGRET_CONNECT_LIST)  
						pMe->m_device_list_id = IDS_LIST_CONNECTED;
						CLOSE_DIALOG(DLGRET_DEVICE_LIST) 
					}
					else
					{
						BTApp_ShowMessage(pMe, IDS_MSG_NO_CONNECTED_DEV, NULL, 0);						
					}
					return TRUE;
						
				}
				case IDS_UNBOND_ALL:
				{			
					BTApp_BuildPrompt(pMe, BT_APP_MENU_UNBOND_ALL);
					return TRUE;
				}		
				case IDS_REMOVE_ALL:
				{		
					BTApp_BuildPrompt(pMe, BT_APP_MENU_REMOVE_ALL);
					return TRUE;
				}		
				default:
				    ASSERT_NOT_REACHABLE;
            }
            return TRUE;
        }
		
        default:
        {
			break;
        }
    }
	
    return FALSE;	
}	

static boolean HandleDeviceListDialogEvent(CBTApp *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{	 
	PARAM_NOT_REF(dwParam)

    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_DEVICE_LIST);

	//AEEBTDeviceInfo*  pDev = &pMe->mRM.device[pMe->mRM.uCurDevIdx];
	
    if (pMenu == NULL)
    {
        return FALSE;
    }
	
	MSG_FATAL("***zzg HandleDeviceListDialogEvent eCode=%x, wParam=%x ,dwParam=%x***",eCode,wParam,dwParam); 	

    switch (eCode)
    {
        case EVT_DIALOG_INIT:			
		{			
			AECHAR 		WTitle[40] = {0};				
			uint16      titleID = IDS_BONDED_DEVICES;

			if (pMe->m_device_list_id == IDS_LIST_BONDED)			
			{
				titleID = IDS_BONDED_DEVICES;
			}
			else if (pMe->m_device_list_id == IDS_LIST_ALL)	
			{
				titleID = IDS_LIST_ALL;
			}
			else if (pMe->m_device_list_id == IDS_LIST_CONNECTED)			
			{
				titleID = IDS_LIST_CONNECTED;
			}
					

			(void)ISHELL_LoadResString(pMe->m_pShell,
				                        AEE_APPSBTAPP_RES_FILE,                                
				                        titleID,
				                        WTitle,
				                        sizeof(WTitle));
			
			IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, WTitle); 

			MSG_FATAL("***zzg BondListDialog EVT_DIALOG_INIT***", 0, 0, 0);
			
            return TRUE;
		}

        case EVT_DIALOG_START:       
        {
			AEEBTDeviceEnumerator enumerator;
			AEEBTDeviceInfo*      pDev;
			CtlAddItem            ai;
			AECHAR                wName[32];
			uint16                stringID;
			uint16                msgID;
			uint8                 i;

			uint8 				  j = 0;
			boolean 			  bAudioDevice = FALSE;
						
#ifdef FEATURE_BT_2_1
			AECHAR*               pwName;
			AECHAR                wBuf[ 5 ];
			STRTOWSTR( "...", wBuf, sizeof(wBuf) );
#endif /* FEATURE_BT_2_1 */

			//Add By zzg 2011_10_26
			if (AEEBT_RM_EC_MATCH_VALUE_1 == pMe->uDeviceListType)
			{
				enumerator.control = AEEBT_RM_EC_MATCH_BONDED;
				enumerator.bBonded = TRUE;
				stringID		   = IDS_BONDED_DEVICES;
				msgID			   = IDS_MSG_NO_BONDED_DEV;
			}		
			//Add End

			if (pMe->m_device_list_id == IDS_LIST_BONDED)		
			{			
				enumerator.control = AEEBT_RM_EC_MATCH_BONDED;
				enumerator.bBonded = TRUE;
				stringID		   = IDS_BONDED_DEVICES;
				msgID			   = IDS_MSG_NO_BONDED_DEV;
				
			}
			else if (pMe->m_device_list_id == IDS_LIST_ALL)		
			{
				enumerator.control = AEEBT_RM_EC_ALL;
				stringID           = IDS_KNOWN_DEVICES;
				msgID              = IDS_MSG_NO_DEV;		

				//PUSH_MENU(BT_APP_MENU_LIST_ALL);
			}
			else if (pMe->m_device_list_id == IDS_LIST_CONNECTED)		
			{
				enumerator.control = AEEBT_RM_EC_ALL;
				stringID           = IDS_CONNECTED_DEVICES;
				msgID              = IDS_MSG_NO_CONNECTED_DEV;
				
				//PUSH_MENU(BT_APP_MENU_LIST_CONNECTED);
			}								

			if (IBTEXTRM_DeviceEnumInit(pMe->mRM.po, &enumerator) == SUCCESS)
			{
				MSG_FATAL("***zzg BondListDialog IBTEXTRM_DeviceEnumInit SUCCESS***", 0, 0, 0);
				
				BTApp_InitAddItem(&ai);
				ai.wFont = AEE_FONT_BOLD;

				i    = 0;
				pDev = &pMe->mRM.device[i];

				while ((IBTEXTRM_DeviceEnumNext( pMe->mRM.po, pDev) == SUCCESS) 
						&& pDev->bValid && (i < MAX_DEVICES))
				{
					MSG_FATAL("***zzg BondListDialog IBTEXTRM_DeviceEnumNext SUCCESS***", 0, 0, 0);

					if (pMe->m_device_list_id == IDS_LIST_CONNECTED)	
					{
						pMe->mRM.linkStatus->bdAddr = pDev->bdAddr;
						if (IBTEXTRM_GetLinkStatus(pMe->mRM.po, pMe->mRM.linkStatus) != SUCCESS)
						{
							// dev not connected to us; skip it
							continue;
						}	
					}

					//Add By zzg 2011_10_26
					if (AEEBT_RM_EC_MATCH_VALUE_1 == pMe->uDeviceListType)
					{						
						// find out if this is an audio device
						if ((pDev->majorDevClass == AEEBT_COD_MAJ_DEV_CLS_AUDIO) 
							|| (pDev->serviceClass & AEEBT_COD_SC_AUDIO))
						{
							bAudioDevice = TRUE;							
						}
						else
						{
							while ((j < pDev->uNumSvcs) &&(bAudioDevice == FALSE))
							{
								if ((pDev->svcs[j].uServiceClass == AEEBT_SD_SERVICE_CLASS_HEADSET) 
									||(pDev->svcs[j].uServiceClass == AEEBT_SD_SERVICE_CLASS_HANDSFREE))
								{									
									bAudioDevice = TRUE;		
								}	
								j++;
							}
						}						
					}
					//Add End
					
					if ( WSTRLEN( pDev->wName ) == 0 )
					{
						BTApp_BDAddr2Wstr( wName, &pDev->bdAddr );
					}
					else
					{
						WSTRLCPY( wName, pDev->wName, ARR_SIZE( wName ) );
					}

					if (pMe->m_device_list_id == IDS_LIST_CONNECTED)	
					{
						AECHAR wM[] = { ' ', '-', ' ', 'M', 0 };
						AECHAR wS[] = { ' ', '-', ' ', 'S', 0 };

						if (WSTRLEN(wName) > (ARR_SIZE(wName) - 5))
						{
							wName[ARR_SIZE(wName) - 5] = NULL;
						}
						
						WSTRLCAT(wName, pMe->mRM.linkStatus->bMaster ? wM : wS, sizeof(wName));
					}
				
#ifdef FEATURE_BT_2_1
					pwName = pDev->wName;
					if ((pDev->EIRData.uFlags & AEEBT_EIR_DATA_RCVD_B) 
						&& (!(pDev->EIRData.uFlags & AEEBT_EIR_NAME_CMPLT_B)))
					{
						WSTRLCAT( pwName, wBuf, AEEBT_MAX_DEVICENAME_LEN );
					}
					
					//IMENUCTL_AddItem(pMenu, NULL, 0, i, pwName, 0 );
					//Modify By zzg 2011_10_26
					if (AEEBT_RM_EC_MATCH_VALUE_1 == pMe->uDeviceListType)
					{
						if (TRUE == bAudioDevice)
						{
							IMENUCTL_AddItem(pMenu, NULL, 0, i, pwName, 0 );
						}
					}
					else
					{
						IMENUCTL_AddItem(pMenu, NULL, 0, i, pwName, 0 );
					}
					//Add End
#else
					//IMENUCTL_AddItem(pMenu, NULL, 0, i, wName, 0 );
					//Modify By zzg 2011_10_26
					if (AEEBT_RM_EC_MATCH_VALUE_1 == pMe->uDeviceListType)
					{
						if (TRUE == bAudioDevice)
						{
							IMENUCTL_AddItem(pMenu, NULL, 0, i, pwName, 0 );
						}
					}
					else
					{
						IMENUCTL_AddItem(pMenu, NULL, 0, i, pwName, 0 );
					}
					//Add End
#endif // FEATURE_BT_2_1 
					
					bAudioDevice = FALSE;		

					if (pDev->bBonded)
					{
						IMENUCTL_SetItem(pMenu, i, MSIF_FONT, &ai );
					}

					ai.wImage = BTApp_GetDevTypeImageID( pMe, pDev->serviceClass,&pDev->bdAddr );
					IMENUCTL_SetItem(pMenu, i, MSIF_IMAGE, &ai );

					pDev = &pMe->mRM.device[++i];
				}

				if (i > 0)
				{
					MSG_FATAL("***zzg BondListDialog i=%d***", i, 0, 0);					
					IMENUCTL_SetSel(pMenu, MENU_SEL);
				}
				else
				{
					MSG_FATAL("***zzg BondListDialog IBTEXTRM_DeviceEnumInit i == 0***", 0, 0, 0);

					//Add By zzg 2011_06_07
					IMENUCTL_SetSel(pMenu, pMe->m_currDlgId);
					
		            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);
		            IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);

#ifdef FEATURE_CARRIER_CHINA_VERTU
		            IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SETTING_BACKGROUND);
#endif
		            IMENUCTL_SetBottomBarType(pMenu, BTBAR_BACK);
					
		            (void) ISHELL_PostEvent( pMe->m_pShell,
		                                     AEECLSID_BLUETOOTH_APP,
		                                     EVT_USER_REDRAW,
		                                     0,
		                                     0);
					//Add End				
					
					return TRUE;
				}
			}			

#ifdef FEATURE_APP_TEST_AUTOMATION
			#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION

            IMENUCTL_SetSel(pMenu, pMe->m_currDlgId);

            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);
            IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);

#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SETTING_BACKGROUND);
#endif
            IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);

			IMENUCTL_SetSel(pMenu,  MENU_SEL);  // highlight the selected item
			
            (void) ISHELL_PostEvent( pMe->m_pShell,
                                     AEECLSID_BLUETOOTH_APP,
                                     EVT_USER_REDRAW,
                                     0,
                                     0);
            return TRUE;
        }
		
        case EVT_USER_REDRAW:    
		{	
			(void)IMENUCTL_Redraw(pMenu);

			// Activate menu
			IMENUCTL_SetActive(pMenu, TRUE);
			IDISPLAY_UpdateEx(pMe->m_pIDisplay, FALSE);			
            return TRUE;
        }
		
        case EVT_DIALOG_END:
        {
			return TRUE;
        }

        case EVT_KEY_PRESS:
        {
			switch(wParam)
            {
            	case AVK_INFO:	
				case AVK_SELECT:
				{
					if (IMENUCTL_GetItemCount(pMenu) > 0)
					{
						pMe->mRM.uCurDevIdx = IMENUCTL_GetSel(pMenu);
						MENU_SET_SEL( pMe->mRM.uCurDevIdx );

						pMe->m_dialog_id = IDD_DEVICE_LIST;
						CLOSE_DIALOG(DLGRET_DEVICEINFO)
					}				 				  
					return TRUE;
            	}
				
                case AVK_CLR:
                {
					CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;
                }

                default:
                {
					break;
                }
            }
            return TRUE;        
        }
		
        default:
        {
			break;
        }
    }
	
	return FALSE;
}	

static boolean HandleMyInfoDialogEvent(CBTApp *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{	 
	PARAM_NOT_REF(dwParam)

    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_MYINFO);
	IStatic  *pStatic = (IStatic*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_MYINFO_STATIC);
	
    if ((pMenu == NULL) || (pStatic == NULL))
    {
        return FALSE;
    }
    
	MSG_FATAL("***zzg HandleMyInfoDialogEvent eCode=%x, wParam=%x ,dwParam=%x***",eCode,wParam,dwParam); 	
   
    switch (eCode)
    {
        case EVT_DIALOG_INIT:			
		{
			//IDIALOG_SetProperties((IDialog *)dwParam, DLG_NOT_REDRAW_AFTER_START);
            return TRUE;
		}

        case EVT_DIALOG_START:   
        {	
			IMENUCTL_SetSel(pMenu, pMe->m_currDlgId);
			IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);
			IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);

#ifdef FEATURE_CARRIER_CHINA_VERTU
			IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SETTING_BACKGROUND);
#endif
			IMENUCTL_SetBottomBarType(pMenu,BTBAR_OPTION_BACK);

            (void) ISHELL_PostEvent( pMe->m_pShell,
                                     AEECLSID_BLUETOOTH_APP,
                                     EVT_USER_REDRAW,
                                     0,
                                     0);     						
            return TRUE;
        }
		
        case EVT_USER_REDRAW:    
		{
			AEERect 	rc;
			uint16		uLen = 0;	
			AECHAR		WTitle[40] = {0};			

#ifdef FEATURE_BT_2_1
			boolean 	bNameSame = FALSE;
			int 		i;
			uint8		maxManuDataLen = AEEBT_MAX_EIR_MANUF_DATA_LEN;
			uint8		pManuData[ AEEBT_MAX_EIR_MANUF_DATA_LEN ];
			char		szManuData[ AEEBT_MAX_EIR_MANUF_DATA_LEN + 1 ] = {0};
#endif /* FEATURE_BT_2_1 */

			(void)ISHELL_LoadResString( pMe->m_pShell,
										AEE_APPSBTAPP_RES_FILE, 							   
										IDS_MY_INFO,
										WTitle,
										sizeof(WTitle));
			IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, WTitle);

			MSG_FATAL("***zzg MyInfoDialog EVT_USER_REDRAW bDiscoverable=%d***", pMe->mSD.bDiscoverable, 0, 0);
			 
			if (IBTEXTRM_GetLocalInfo(pMe->mRM.po, &pMe->mRM.myInfo) == SUCCESS)
			{
				// set rect for info display area
				SETAEERECT(&rc, pMe->m_rect.x, 
						   pMe->m_rect.y, 
						   pMe->m_rect.dx, 
						   pMe->m_rect.dy - BOTTOMBAR_HEIGHT); // leave room for SK menu
						 
				ISTATIC_SetRect(pStatic, &rc );

				ISTATIC_SetProperties(pStatic, 
									  ISTATIC_GetProperties(pStatic) 
									  & ~ST_MIDDLETEXT & ~ST_CENTERTEXT);	   

				//Add By zzg 2010_10_29 	   
				ISTATIC_SetProperties(pStatic, ST_NOSCROLL|ST_GRAPHIC_BG);	
				ISTATIC_SetBackGround(pStatic, AEE_APPSCOMMONRES_IMAGESFILE, IDB_BACKGROUND);	
				//Add End

				// BT name
				uLen += BTApp_FormatBTName(pMe, &pMe->pText1[uLen], 
										   LONG_TEXT_BUF_LEN - uLen, 
										   pMe->mRM.myInfo.wName);

#ifdef FEATURE_BT_2_1
				// BT ShortName
				if (WSTRCMP(pMe->mRM.myInfo.wName , pMe->mRM.myInfo.wShortName) == 0)
				{
				  	bNameSame = TRUE;
				} 
				uLen += BTApp_FormatBTShortName(pMe, &pMe->pText1[uLen], 
												LONG_TEXT_BUF_LEN - uLen, 
												pMe->mRM.myInfo.wShortName,
												bNameSame); 
#endif 
				// BD address
				uLen += BTApp_FormatBDAddress(pMe, &pMe->pText1[uLen], 
											  LONG_TEXT_BUF_LEN - uLen, 
											  &pMe->mRM.myInfo.bdAddr);
				// Security Level
				uLen += BTApp_FormatSecurity(pMe, &pMe->pText1[uLen], 
											 LONG_TEXT_BUF_LEN - uLen, 
											 pMe->mRM.myInfo.security);
				// Bondable status
				uLen += BTApp_FormatBondable(pMe, &pMe->pText1[uLen], 
											 LONG_TEXT_BUF_LEN - uLen, 
											 pMe->mRM.bBondable);
				// Discoverable status
				uLen += BTApp_FormatDiscoverable(pMe, &pMe->pText1[uLen], 
												 LONG_TEXT_BUF_LEN - uLen, 
												 pMe->mSD.bDiscoverable);
				// Service Class
				uLen += BTApp_FormatSvcCls(pMe, &pMe->pText1[uLen], 
										   LONG_TEXT_BUF_LEN - uLen, 
										   pMe->mRM.myInfo.svcCls);
				// LMP Version
				uLen += BTApp_FormatLMPVersion(pMe, &pMe->pText1[uLen], 
											   LONG_TEXT_BUF_LEN - uLen);

				// LMP Subversion
				uLen += BTApp_FormatLMPSubVersion(pMe, &pMe->pText1[uLen], 
												 LONG_TEXT_BUF_LEN - uLen);
				// LMP features
				uLen += BTApp_FormatLMPFeatures(pMe, &pMe->pText1[uLen], 
												 LONG_TEXT_BUF_LEN - uLen, 
												 pMe->mRM.myInfo.uLMPFeatures);
#ifdef FEATURE_BT_2_1
				//ManuData
				if (IBTEXTRM_GetEIRManufData(pMe->mRM.po, NULL,
											 &maxManuDataLen, 
											 pManuData) != SUCCESS)
				{
					MSG_ERROR("MY Info - Manufacturer Data failed failed to read from EFS", 0, 0, 0);
				} 
				else
				{
					for (i = 0; i < maxManuDataLen; i++)
					{
						SNPRINTF((szManuData + STRLEN(szManuData)), sizeof(szManuData) - STRLEN(szManuData), "%c", pManuData[i]);
					}
					STRTOWSTR((char*)szManuData, pMe->mRM.wManuData, sizeof(pMe->mRM.wManuData));
				}

				uLen += BTApp_FormatManuData( pMe, &pMe->pText1[ uLen], 
											  LONG_TEXT_BUF_LEN - uLen, 
											  pMe->mRM.wManuData );
#endif /* FEATURE_BT_2_1 */

				// display text 			
				ISTATIC_SetText(pStatic, NULL, pMe->pText1, AEE_FONT_BOLD,AEE_FONT_NORMAL );
					   
			}


#ifdef FEATURE_APP_TEST_AUTOMATION
	#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION

			MSG_FATAL("***zzg HandleMyInfoDialogEvent EVT_USER_REDRAW***", 0, 0, 0);            

			ISTATIC_Redraw(pStatic);	
			
			//IMENUCTL_SetActive(pMenu, TRUE );	

			ISTATIC_SetActive(pStatic, TRUE);
			
			IDISPLAY_UpdateEx( pMe->m_pIDisplay, FALSE); 
			
            return TRUE;

		}
		
        case EVT_DIALOG_END:
        {
			return TRUE;
        }

        case EVT_KEY:
        {
			switch(wParam)
            {
                case AVK_CLR:
                {
					CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;
                }
					
				case AVK_INFO:
				case AVK_SELECT:
				{
					CLOSE_DIALOG(DLGRET_MYINFO_OPITION)
					return TRUE;
				}

				default:
                {
					break;
				}
            }
            return TRUE;
        }

		default:
        {
			break;
		}
    }
    return FALSE;
}	

static boolean HandleMyInfoOpitionDialogEvent(CBTApp *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{	 
	PARAM_NOT_REF(dwParam)

    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_MYINFO_OPITION);
	
    if (pMenu == NULL)
    {
        return FALSE;
    }
	
    MSG_FATAL("***zzg HandleMyInfoOpitionDialogEvent eCode=%x, wParam=%x ,dwParam=%x***",eCode,wParam,dwParam); 	
   
    switch (eCode)
    {
        case EVT_DIALOG_INIT:			
		{			
			AECHAR 		WTitle[40] = {0};			

			(void)ISHELL_LoadResString(pMe->m_pShell,
				                       AEE_APPSBTAPP_RES_FILE,                                
				                       IDS_OPTIONS,
				                       WTitle,
				                       sizeof(WTitle));
			
			IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, WTitle);   

  			IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, IDS_EDIT_NAME, IDS_EDIT_NAME, NULL, 0);
			IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, IDS_EDIT_SHORT_NAME, IDS_EDIT_SHORT_NAME, NULL, 0);			
            IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, IDS_EDIT_MANU_DATA, IDS_EDIT_MANU_DATA, NULL, 0);
            //IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, IDS_SECURITY, IDS_SECURITY, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, IDS_DISCOVERABLE, IDS_DISCOVERABLE, NULL, 0);
			//IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, IDS_IOCAPABILITY, IDS_IOCAPABILITY, NULL, 0);
			//IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, IDS_DBG_KEY, IDS_DBG_KEY, NULL, 0);
			//IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, IDS_WRITE_OOB, IDS_WRITE_OOB, NULL, 0);

            return TRUE;
		}

        case EVT_DIALOG_START:
        {
			// 给菜单各菜单项加数字编号图标
            //BTApp_SetItemNumIcon(pMenu);

            IMENUCTL_SetSel(pMenu, pMe->m_currDlgId);
            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);
            IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);

#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SETTING_BACKGROUND);
#endif
            IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);

            (void) ISHELL_PostEvent(pMe->m_pShell,
                                    AEECLSID_BLUETOOTH_APP,
                                    EVT_USER_REDRAW,
                                    0,
                                    0);
            return TRUE;
        }

        case EVT_USER_REDRAW:    			
        {
			//(void)IMENUCTL_Redraw(pMenu);     //dele by yangdecai
            return TRUE;
        }	

        case EVT_DIALOG_END:
        {
			return TRUE;
		}	

        case EVT_KEY:
        {   
			switch(wParam)
            {
                case AVK_CLR:
                {    
					CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;
				}
				
                default:
                {   
					break;
				}
            }
            return TRUE;
		}
		
        case EVT_COMMAND:           
        {
			pMe->m_currDlgId = wParam;		
            switch (wParam)
            {
            	case IDS_EDIT_NAME:					
				case IDS_EDIT_SHORT_NAME:
				case IDS_EDIT_MANU_DATA:	
				{	
					CLOSE_DIALOG(DLGRET_MYINFO_EDIT)
                    return TRUE;
				}
				
				case IDS_SECURITY:	
				{	
					CLOSE_DIALOG(DLGRET_SET_SECURITY)
                    return TRUE;
				}
				
				case IDS_DISCOVERABLE:	
				{	
					CLOSE_DIALOG(DLGRET_SET_DISCOVERABLE)
                    return TRUE;
				}
				
				case IDS_IOCAPABILITY:	
				{	
					CLOSE_DIALOG(DLGRET_SET_IOCAPABILITY)
                    return TRUE;
				}
				
				case IDS_DBG_KEY:	
				{	
					CLOSE_DIALOG(DLGRET_SET_DEBUGKEY)
                    return TRUE;	
				}
				
				case IDS_WRITE_OOB:
				{						
					//pMe->m_msg_id = IDS_MSG_OOB_DATA_CREATED;
					//pMe->m_bNeedStr = FALSE;
					
					BTApp_ShowBusyIcon( pMe );
					CALLBACK_Init (&pMe->mRM.OOBCreateCb, BTApp_LocalOOBCreated, pMe);
					if (IBTEXTRM_CreateOOBData( pMe->mRM.po, &pMe->mRM.OOBDataSize, &pMe->mRM.OOBCreateCb ) != SUCCESS )
					{
						MSG_ERROR( "BTApp - Failed to create OOB data", 0, 0, 0 );
					}
					//CLOSE_DIALOG(DLGRET_BT_MSGBOX)
                    return TRUE;	
				}
				
				default:
				{    
					ASSERT_NOT_REACHABLE;
				}
            }
            return TRUE;
        }
		
        default:
        {
			break;
        }
    }
    return FALSE;
}	


static boolean  HandleMyInfoEditDialogEvent(CBTApp *pMe,
													  AEEEvent eCode,
													  uint16 wParam,
													  uint32 dwParam
)
{	 
	PARAM_NOT_REF(dwParam)

	ITextCtl *pIText = (ITextCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_MYINFO_EDIT);	
	
    if (NULL == pIText)
    {
        return FALSE;
    }
	
    MSG_FATAL("***zzg HandleMyInfoEditDialogEvent eCode=%x, wParam=%x ,dwParam=%x***",eCode,wParam,dwParam); 

    switch (eCode)
    {
        case EVT_DIALOG_INIT:			
		{	
			//IDIALOG_SetProperties((IDialog *)dwParam, DLG_NOT_REDRAW_AFTER_START);
            return TRUE;
		}

        case EVT_DIALOG_START:   
		{
			AEERect		rc;		
			SETAEERECT ( &rc, pMe->m_rect.x, pMe->m_rect.y, pMe->m_rect.dx, pMe->m_rect.dy - BOTTOMBAR_HEIGHT);
			ITEXTCTL_SetRect(pIText, &rc );
			
			ITEXTCTL_SetMaxSize(pIText, AEEBT_MAX_DEVICENAME_LEN);				
			ITEXTCTL_SetProperties(pIText, TP_FRAME | TP_USELESS_UPDOWN |TP_MULTILINE | TP_STARKEY_SWITCH | TP_NOUPDATE |TP_FOCUS_NOSEL);


			MSG_FATAL("***zzg MyInfoEdit m_currDlgId=%d***", pMe->m_currDlgId, 0, 0);
			
			switch(pMe->m_currDlgId)
			{
				case IDS_EDIT_NAME:				
				{
					ITEXTCTL_SetText(pIText, pMe->mRM.myInfo.wName, WSTRLEN(pMe->mRM.myInfo.wName));	
					break;
				}
				
				case IDS_EDIT_SHORT_NAME:	
				{
					ITEXTCTL_SetText(pIText, pMe->mRM.myInfo.wShortName, WSTRLEN(pMe->mRM.myInfo.wShortName));		
					break;
				}
				
				case IDS_EDIT_MANU_DATA:	
				{
					ITEXTCTL_SetText(pIText, pMe->mRM.wManuData, WSTRLEN(pMe->mRM.wManuData));		
					break;
				}	

				default:
				{
					break;
				}
			}
					
			ITEXTCTL_SetInputMode(pIText, AEE_TM_LETTERS);		
			
            (void) ISHELL_PostEvent(pMe->m_pShell,
                                    AEECLSID_BLUETOOTH_APP,
                                    EVT_USER_REDRAW,
                                    0,
                                    0);
			
            return TRUE;
        }

        case EVT_USER_REDRAW:    
		{			
			AECHAR 		WTitle[40] = {0};	

			MSG_FATAL("***zzg MyInfoEdit m_currDlgId=%d***", pMe->m_currDlgId, 0, 0);
			
			switch(pMe->m_currDlgId)
			{
				case IDS_EDIT_NAME:				
				{
					(void)ISHELL_LoadResString(pMe->m_pShell,
						                       AEE_APPSBTAPP_RES_FILE,                                
						                       IDS_EDIT_NAME,
						                       WTitle,
						                       sizeof(WTitle));	
					break;
				}
				
				case IDS_EDIT_SHORT_NAME:	
				{
					(void)ISHELL_LoadResString(pMe->m_pShell,
						                       AEE_APPSBTAPP_RES_FILE,                                
						                       IDS_EDIT_SHORT_NAME,
						                       WTitle,
						                       sizeof(WTitle));
					break;
				}
				
				case IDS_EDIT_MANU_DATA:	
				{	
					(void)ISHELL_LoadResString(pMe->m_pShell,
						                       AEE_APPSBTAPP_RES_FILE,                                
						                       IDS_EDIT_MANU_DATA,
						                       WTitle,
						                       sizeof(WTitle)); 	
					break;
				}	

				default:
				{
					break;
				}
			}			
			
			IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, WTitle); 
						
			ITEXTCTL_SetCursorPos(pIText, TC_CURSOREND);    
			ITEXTCTL_SetActive(pIText, TRUE);
			ITEXTCTL_Redraw(pIText);	

			BT_DRAW_BOTTOMBAR(BTBAR_OK_BACK)
			
			IDISPLAY_UpdateEx( pMe->m_pIDisplay, FALSE); 
            return TRUE;
        }
		
        case EVT_DIALOG_END:
        {
			return TRUE;
       	}

        case EVT_KEY:
        {
			switch(wParam)
            {
                case AVK_CLR:
                {
					CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;
                }
					
				case AVK_SELECT:
				case AVK_INFO:
				{
					ITEXTCTL_GetText(pIText, pMe->pText2, SHORT_TEXT_BUF_LEN);
					MSG_FATAL("***zzg MyInfoEdit m_currDlgId=%d***", pMe->m_currDlgId, 0, 0);
					BTApp_SaveTextEdit(pMe, pMe->m_currDlgId);
					CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;
				}	
				
                default:
                {
					break;
                }
            }
            return TRUE;
        }	
#ifdef FEATURE_LCD_TOUCH_ENABLE//andrew add for LCD touch
		case EVT_PEN_UP:
			{
				AEEDeviceInfo devinfo;
				int nBarH ;
				AEERect rc;
				int16 wXPos = (int16)AEE_GET_X(dwParam);
				int16 wYPos = (int16)AEE_GET_Y(dwParam);

				nBarH = GetBottomBarHeight(pMe->m_pIDisplay);
        
				MEMSET(&devinfo, 0, sizeof(devinfo));
				ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
				SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);

				if(TOUCH_PT_IN_RECT(wXPos,wYPos,rc))
				{
					if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
					{
						boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_BLUETOOTH_APP,EVT_USER,AVK_SELECT,0);
                        MSG_FATAL("HandleFTPServerRegisterDlg:EVT_PEN_UP",0,0,0);
                        return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)   && wXPos < rc.x + (rc.dx/3)*2 )//左
					{
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_BLUETOOTH_APP,EVT_USER,AVK_INFO,0);
						 return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
					{						
                        AECHAR *pwstrText = ITEXTCTL_GetTextPtr(pIText);
                        int nLen = 0;
                        boolean rt = FALSE;
                        int nCount = 0;
                        if (NULL != pwstrText)
                        {
                            nLen = WSTRLEN(pwstrText);
                        }
                        if(nLen == 0)
                        {
                            rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_BLUETOOTH_APP,EVT_USER,AVK_CLR,0);
                        }
                        else
                        {

                            AECHAR newstrText[64] = {0};
                            nCount = WSTRNCOPYN(newstrText,sizeof(newstrText),pwstrText,nLen-1);
                            if(nCount == 0)
                            {
                                ITEXTCTL_Reset(pIText);
                                ITEXTCTL_SetActive(pIText,TRUE);
                            }
                            else
                            {
                                rt = ITEXTCTL_SetText(pIText,newstrText,WSTRLEN(newstrText));
                            }
                            (void) ISHELL_PostEvent(pMe->m_pShell,
                                                    AEECLSID_BLUETOOTH_APP,
                                                    EVT_USER_REDRAW,
                                                    0,
                                                    0);

                        }
                        return rt;

					}
				}

			}
			break;
#endif
        default:
        {
			break;
       	}
    }
    return FALSE;
}

static boolean  HandleSetSecurityDialogEvent(CBTApp *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{	 
	PARAM_NOT_REF(dwParam)
		
	int               result;
	uint16            selection;
	uint16            sel = IDS_SECURITY_NONE;
	AEEBTSecurity     selectedSecurity;
	AEEBTSecurity*    pCurrentSecurity;
	
    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_BT_SECURITY);

    if (pMenu == NULL)
    {
        return FALSE;
    }
	
	selection = IMENUCTL_GetSel(pMenu);

	pCurrentSecurity = &pMe->mRM.myInfo.security;	
	//pCurrentSecurity = &pMe->mRM.device[ pMe->mRM.uCurDevIdx ].security;
	
    MSG_FATAL("%x, %x ,%x,HandleSetSecurityDialogEvent",eCode,wParam,dwParam);   

    switch (eCode)
    {
        case EVT_DIALOG_INIT:			
		{
            return TRUE;
		}

        case EVT_DIALOG_START:
        {
			AECHAR		WTitle[40] = {0};		
			
			(void)ISHELL_LoadResString(pMe->m_pShell,
										AEE_APPSBTAPP_RES_FILE, 							   
										IDS_SECURITY,
										WTitle,
										sizeof(WTitle));
			
			IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, WTitle);   
  
			IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, IDS_SECURITY_NONE, IDS_SECURITY_NONE, NULL, 0);
			IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, IDS_SECURITY_AUTH, IDS_SECURITY_AUTH, NULL, 0); 		
			IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, IDS_SECURITY_AUTH_ENC, IDS_SECURITY_AUTH_ENC, NULL, 0);
			
			switch (*pCurrentSecurity)
			{
				case AEEBT_SEC_NONE:
					sel = IDS_SECURITY_NONE;
					break;
				case AEEBT_SEC_AUTHENTICATE:
					sel = IDS_SECURITY_AUTH;
					break;
				case AEEBT_SEC_AUTHENTICATE_AND_ENCRYPT:
					sel = IDS_SECURITY_AUTH_ENC;
					break;
				default:
					break;
			}			

			InitMenuIcons(pMenu);
            SetMenuIcon(pMenu, sel, TRUE);  
            IMENUCTL_SetSel(pMenu, sel );  
			
			IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL); //|MP_TEXT_ALIGN_LEFT_ICON_ALIGN_RIGHT);          
            IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);

#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SETTING_BACKGROUND);
#endif
            IMENUCTL_SetBottomBarType(pMenu,BTBAR_OK_BACK);

			IMENUCTL_SetActive(pMenu, TRUE );
  			IDISPLAY_UpdateEx( pMe->m_pIDisplay, FALSE ); 

            (void) ISHELL_PostEvent( pMe->m_pShell,
                                     AEECLSID_BLUETOOTH_APP,
                                     EVT_USER_REDRAW,
                                     0,
                                     0);
            return TRUE;
        }
		
        case EVT_USER_REDRAW:    	
		{			
            //(void)IMENUCTL_Redraw(pMenu);    //dele by yangdecai
            return TRUE;
        }

        case EVT_DIALOG_END:
        {
			return TRUE;
        }

		case EVT_KEY_PRESS:       
		{

			switch(wParam)
			{
				case AVK_SELECT:
				{
					int numItems;
					int curItem;
					uint16 curItemId;
					CtlAddItem ai;

					numItems = IMENUCTL_GetItemCount(pMenu);
					
					for (curItem = 0; curItem < numItems; ++curItem)
					{
						curItemId = IMENUCTL_GetItemID(pMenu, curItem);

						MSG_FATAL("***zzg HandleSetSecurityDialogEvent curItemId=%d***", curItemId, 0, 0);

						if (IMENUCTL_GetItem(pMenu, curItemId, &ai))
						{
							if (ai.wImage == IDB_RADIO_FILLED)
							{
								switch (curItemId)
								{
									case IDS_SECURITY_NONE:
										selectedSecurity = AEEBT_SEC_NONE;
										break;
									case IDS_SECURITY_AUTH:
										selectedSecurity = AEEBT_SEC_AUTHENTICATE;
										break;
									case IDS_SECURITY_AUTH_ENC:
										selectedSecurity = AEEBT_SEC_AUTHENTICATE_AND_ENCRYPT;;
										break;
									default:
										MSG_ERROR( "HndlDevSec - selection=%d", selection, 0, 0 );
										selectedSecurity = *pCurrentSecurity;
								}
							}							
							
						}						
						
					}
										
					if ( selectedSecurity != *pCurrentSecurity )
					{
						*pCurrentSecurity = selectedSecurity;

						result = IBTEXTRM_SetSecurity( pMe->mRM.po, *pCurrentSecurity );
						//result = IBTEXTRM_SetDeviceSecurity( pMe->mRM.po, &pDev->bdAddr,*pCurrentSecurity );
											
						if ( result != SUCCESS )
						{
							MSG_ERROR( "HndlSec - failed to set security", 0, 0, 0 );
						}						
					}

					CLOSE_DIALOG(DLGRET_CANCELED)
					return TRUE;					
				}
				
				case AVK_INFO:	
					
					switch ( selection )
					{
						case IDS_SECURITY_NONE:
							SetMenuIcon(pMenu, IDS_SECURITY_NONE, TRUE); 
							SetMenuIcon(pMenu, IDS_SECURITY_AUTH, FALSE); 
							SetMenuIcon(pMenu, IDS_SECURITY_AUTH_ENC, FALSE); 
							break;
						case IDS_SECURITY_AUTH:
							SetMenuIcon(pMenu, IDS_SECURITY_AUTH, TRUE); 
							SetMenuIcon(pMenu, IDS_SECURITY_NONE, FALSE); 
							SetMenuIcon(pMenu, IDS_SECURITY_AUTH_ENC, FALSE);
							break;
						case IDS_SECURITY_AUTH_ENC:
							SetMenuIcon(pMenu, IDS_SECURITY_AUTH_ENC, TRUE); 
							SetMenuIcon(pMenu, IDS_SECURITY_NONE, FALSE); 
							SetMenuIcon(pMenu, IDS_SECURITY_AUTH, FALSE);
							break;
						default:
							break;
					}
					 
					return TRUE;			
					
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;

                default:
                    break;
			}
            
             return TRUE;
		}
		
        default:
            break;
    }
	
    return FALSE;        
}


static boolean  HandleSetDiscoverableDialogEvent(CBTApp *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{	 
		PARAM_NOT_REF(dwParam)
			
		int			result;
		uint16		selection;
		uint16		sel = IDS_OFF;
		boolean		discoverable;
		
		IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_BT_DISCOVERABLE);
	
		if (pMenu == NULL)
		{
			return FALSE;
		}	
		
		selection = IMENUCTL_GetSel(pMenu);

		MSG_FATAL("***zzg HandleSetDiscoverableDialogEvent eCode=%x, wParam=%x ,dwParam=%x***",eCode,wParam,dwParam); 
		
		switch (eCode)
		{
			case EVT_DIALOG_INIT:			
			{
				return TRUE;
			}
	
			case EVT_DIALOG_START:
			{
				AECHAR		WTitle[40] = {0};		
				
				(void)ISHELL_LoadResString(pMe->m_pShell,
											AEE_APPSBTAPP_RES_FILE, 							   
											IDS_DISCOVERABLE,
											WTitle,
											sizeof(WTitle));
				
				IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, WTitle);   
	  
				IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, IDS_ON, IDS_ON, NULL, 0);//IDS_ON_TIMED
				IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, IDS_OFF, IDS_OFF, NULL, 0); 	


				if ( pMe->mSD.bDiscoverable != FALSE )
				{
					sel = IDS_ON;	//IDS_ON_TIMED
				}
				else
				{
					sel = IDS_OFF;
				}
					
				InitMenuIcons(pMenu);
				SetMenuIcon(pMenu, sel, TRUE);	
				IMENUCTL_SetSel(pMenu, sel );  
				
				IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL); //|MP_TEXT_ALIGN_LEFT_ICON_ALIGN_RIGHT);		   
				IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);
	
#ifdef FEATURE_CARRIER_CHINA_VERTU
				IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SETTING_BACKGROUND);
#endif
				IMENUCTL_SetBottomBarType(pMenu,BTBAR_OK_BACK);
	
				IMENUCTL_SetActive(pMenu, TRUE );
				IDISPLAY_UpdateEx( pMe->m_pIDisplay, FALSE ); 
	
				(void) ISHELL_PostEvent( pMe->m_pShell,
										 AEECLSID_BLUETOOTH_APP,
										 EVT_USER_REDRAW,
										 0,
										 0);
				return TRUE;
			}
			
			case EVT_USER_REDRAW:		
			{			
				//(void)IMENUCTL_Redraw(pMenu);    //dele by yangdecai
				return TRUE;
			}
	
			case EVT_DIALOG_END:
			{
				return TRUE;
			}
	
			case EVT_KEY_PRESS: 	  
			{					
				MSG_FATAL("***zzg HandleSetDiscoverableDialogEvent EVT_KEY_PRESS wParam=%x***", wParam, 0, 0);
				
				switch(wParam)
				{
					case AVK_SELECT:
					{
						int numItems;
						int curItem;
						uint16 curItemId;
						CtlAddItem ai;
	
						numItems = IMENUCTL_GetItemCount(pMenu);
						
						for (curItem = 0; curItem < numItems; ++curItem)
						{
							curItemId = IMENUCTL_GetItemID(pMenu, curItem);	
								
							if (IMENUCTL_GetItem(pMenu, curItemId, &ai))
							{
								if (ai.wImage == IDB_RADIO_FILLED)
								{
									switch (curItemId)
									{
										case IDS_ON:	//IDS_ON_TIMED
											discoverable = TRUE;
											break;
											
										case IDS_OFF:
											discoverable = FALSE;
											break;
										
										default:
											MSG_ERROR( "HndlDiscoverable - selection=%d", selection, 0, 0 );
											discoverable = pMe->mSD.bDiscoverable;
									}
								}							
								
							}						
							
						}			

						MSG_FATAL("***zzg  SetDiscoverable discoverable=%d, pMe->mSD.bDiscoverable=%d***", discoverable, pMe->mSD.bDiscoverable, 0);

						if (discoverable != pMe->mSD.bDiscoverable)
						{
							if ( discoverable )
							{
								if ( (result = IBTEXTSD_SetDiscoverable( pMe->mSD.po, TRUE )) != SUCCESS )
								{
									MSG_FATAL("***zzg  IBTEXTSD_SetDiscoverable TRUR Failed!***", 0, 0, 0);									
									MSG_ERROR( "HndlDiscoverable - SetDisc failed r=%d", result, 0, 0 );
									//BTApp_ShowMessage( pMe, IDS_MSG_DISCOVERABLE_NOT_SET, NULL, 2 );
								}
								else
								{
									MSG_FATAL("***zzg  IBTEXTSD_SetDiscoverable TRUR SUCCESS!***", 0, 0, 0);									
									BTApp_ShowBusyIcon( pMe );									
									//ISHELL_SetTimer( pMe->m_pShell, ONE_SECOND*60*3, (PFNNOTIFY)BTApp_ClearDiscoverableEx, pMe );
								}
							}
							else
							{
								result = BTApp_ClearDiscoverableEx( pMe );
							}
						}
	
						CLOSE_DIALOG(DLGRET_CANCELED)
						return TRUE;					
					}
					
					case AVK_INFO:	
						
						switch ( selection )
						{
							case IDS_ON:	//IDS_ON_TIMED
								SetMenuIcon(pMenu, IDS_ON, TRUE); 	//IDS_ON_TIMED
								SetMenuIcon(pMenu, IDS_OFF, FALSE); 								
								break;
							case IDS_OFF:
								SetMenuIcon(pMenu, IDS_OFF, TRUE); 
								SetMenuIcon(pMenu, IDS_ON, FALSE); 	//IDS_ON_TIMED					
								break;							
							default:
								break;
						}
						 
						return TRUE;			
						
					case AVK_CLR:
						CLOSE_DIALOG(DLGRET_CANCELED)
						return TRUE;
	
					default:
						break;
				}
				
				 return TRUE;
			}
			
			default:
				break;
		}
		
		return FALSE;		 

}


static boolean  HandleSetIOCapabilityDialogEvent(CBTApp *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{	 
	PARAM_NOT_REF(dwParam)
		
	int 			result;
	uint16			selection;
	uint16			sel = IDS_IOC_YESNO;
	AEEBTIOCapType	ioCapType = pMe->mRM.ioCaptype; 
	
	IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_IO_CAPABILITY);

	if (pMenu == NULL)
	{
		return FALSE;
	}	

	selection = IMENUCTL_GetSel(pMenu);

	switch (eCode)
	{
		case EVT_DIALOG_INIT:			
		{
			return TRUE;
		}

		case EVT_DIALOG_START:
		{
			AECHAR		WTitle[40] = {0};		
			
			(void)ISHELL_LoadResString(pMe->m_pShell,
										AEE_APPSBTAPP_RES_FILE, 							   
										IDS_IOCAPABILITY,
										WTitle,
										sizeof(WTitle));
			
			IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, WTitle);   			
         
			IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, IDS_IOC_YESNO, IDS_IOC_YESNO, NULL, 0);
			IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, IDS_IOC_DISPLAY_ONLY, IDS_IOC_DISPLAY_ONLY, NULL, 0); 
			IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, IDS_IOC_KEYBOARD_ONLY, IDS_IOC_KEYBOARD_ONLY, NULL, 0); 	
			IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, IDS_IOC_NO, IDS_IOC_NO, NULL, 0); 	

			
			// select current setting
			switch ( pMe->mRM.ioCaptype )
			{
				case AEEBT_RM_IOC_DISPLAY_YES_NO:
					sel = IDS_IOC_YESNO;
					break;
				case AEEBT_RM_IOC_DISPLAY_ONLY:
					sel = IDS_IOC_DISPLAY_ONLY;
					break;
				case AEEBT_RM_IOC_KEYBOARD_ONLY:
					sel = IDS_IOC_KEYBOARD_ONLY;
					break;
				case AEEBT_RM_IOC_NO_IO:
					sel = IDS_IOC_NO;
					break;
				default:
					break;
			}
				
			InitMenuIcons(pMenu);
			SetMenuIcon(pMenu, sel, TRUE);	
			IMENUCTL_SetSel(pMenu, sel );  
			
			IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL); //|MP_TEXT_ALIGN_LEFT_ICON_ALIGN_RIGHT);		   
			IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);

#ifdef FEATURE_CARRIER_CHINA_VERTU
			IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SETTING_BACKGROUND);
#endif
			IMENUCTL_SetBottomBarType(pMenu,BTBAR_OK_BACK);

			IMENUCTL_SetActive(pMenu, TRUE );
			IDISPLAY_UpdateEx( pMe->m_pIDisplay, FALSE ); 

			(void) ISHELL_PostEvent( pMe->m_pShell,
									 AEECLSID_BLUETOOTH_APP,
									 EVT_USER_REDRAW,
									 0,
									 0);
			return TRUE;
		}
		
		case EVT_USER_REDRAW:		
		{			
			//(void)IMENUCTL_Redraw(pMenu);   //dele by yangdecai
			return TRUE;
		}

		case EVT_DIALOG_END:
		{
			return TRUE;
		}

		case EVT_KEY_PRESS: 	  
		{					
			MSG_FATAL("***zzg HandleSetIOCapabilityDialogEvent EVT_KEY_PRESS wParam=%x***", wParam, 0, 0);
			
			switch(wParam)
			{
				case AVK_SELECT:
				{
					int numItems;
					int curItem;
					uint16 curItemId;
					CtlAddItem ai;

					numItems = IMENUCTL_GetItemCount(pMenu);
					
					for (curItem = 0; curItem < numItems; ++curItem)
					{
						curItemId = IMENUCTL_GetItemID(pMenu, curItem); 
							
						if (IMENUCTL_GetItem(pMenu, curItemId, &ai))
						{
							if (ai.wImage == IDB_RADIO_FILLED)
							{							
								switch (curItemId)
								{
									case IDS_IOC_YESNO:
										ioCapType = AEEBT_RM_IOC_DISPLAY_YES_NO;
										break;
										
									case IDS_IOC_DISPLAY_ONLY:
										ioCapType = AEEBT_RM_IOC_DISPLAY_ONLY;
										break;
										
									case IDS_IOC_KEYBOARD_ONLY:
										ioCapType = AEEBT_RM_IOC_KEYBOARD_ONLY;
										break;	
										
									case IDS_IOC_NO:
										ioCapType = AEEBT_RM_IOC_NO_IO;
										break;
									
									default:
										MSG_ERROR( "HandleIOCMenu - sel=%d", selection, 0, 0 );
							  			ioCapType = pMe->mRM.ioCaptype;
								}
							}							
							
						}						
						
					}			

					if ( pMe->mRM.ioCaptype != ioCapType )
					{
						pMe->mRM.ioCaptype = ioCapType;
						pMe->bConfigChanged = TRUE;
						
						MSG_MED("HandleIOCMenu - Setting IO Cap: %d", pMe->mRM.ioCaptype, 0, 0);
						
						if (IBTEXTRM_SetIOCap(pMe->mRM.po, ioCapType) != SUCCESS )
						{
							MSG_ERROR( "IBTEXTRM_SetIOCap - setting of ioCap", 0, 0, 0 );						
						}
					}


					CLOSE_DIALOG(DLGRET_CANCELED)
					return TRUE;					
				}
				
				case AVK_INFO:	
				{
					switch ( selection )
					{
						case IDS_IOC_YESNO:
							SetMenuIcon(pMenu, IDS_IOC_YESNO, TRUE); 
							SetMenuIcon(pMenu, IDS_IOC_DISPLAY_ONLY, FALSE); 	
							SetMenuIcon(pMenu, IDS_IOC_KEYBOARD_ONLY, FALSE);
							SetMenuIcon(pMenu, IDS_IOC_NO, FALSE);
							break;
						case IDS_IOC_DISPLAY_ONLY:
							SetMenuIcon(pMenu, IDS_IOC_DISPLAY_ONLY, TRUE); 
							SetMenuIcon(pMenu, IDS_IOC_YESNO, FALSE); 	
							SetMenuIcon(pMenu, IDS_IOC_KEYBOARD_ONLY, FALSE);
							SetMenuIcon(pMenu, IDS_IOC_NO, FALSE);					
							break;	
						case IDS_IOC_KEYBOARD_ONLY:
							SetMenuIcon(pMenu, IDS_IOC_KEYBOARD_ONLY, TRUE); 
							SetMenuIcon(pMenu, IDS_IOC_YESNO, FALSE); 	
							SetMenuIcon(pMenu, IDS_IOC_DISPLAY_ONLY, FALSE);
							SetMenuIcon(pMenu, IDS_IOC_NO, FALSE);				
							break;	
						case IDS_IOC_NO:
							SetMenuIcon(pMenu, IDS_IOC_NO, TRUE); 
							SetMenuIcon(pMenu, IDS_IOC_YESNO, FALSE);
							SetMenuIcon(pMenu, IDS_IOC_DISPLAY_ONLY, FALSE); 	
							SetMenuIcon(pMenu, IDS_IOC_KEYBOARD_ONLY, FALSE);												
							break;		
						default:
							break;
					}
					 
					return TRUE;			
				}
				case AVK_CLR:
					CLOSE_DIALOG(DLGRET_CANCELED)
					return TRUE;

				default:
					break;
			}
			
			 return TRUE;
		}
		
		default:
			break;
	}

	return FALSE;		 

}


static boolean  HandleSetDebugKeyDialogEvent(CBTApp * pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{	 
	PARAM_NOT_REF(dwParam)
		
	IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_BT_DEBUG_KEY);

	if (pMenu == NULL)
	{
		return FALSE;
	}	

	switch (eCode)
	{
		case EVT_DIALOG_INIT:			
		{
			AECHAR		WTitle[40] = {0};		
			
			(void)ISHELL_LoadResString(pMe->m_pShell,
									   AEE_APPSBTAPP_RES_FILE, 							   
									   IDS_DBG_KEY,
									   WTitle,
									   sizeof(WTitle));
			
			IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, WTitle);  	
			
			IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, IDS_DBG_KEY, IDS_DBG_KEY, NULL, 0);			
			
			return TRUE;
		}

		case EVT_DIALOG_START:
		{
			//设定标题格式
            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_MULTI_SEL);
            IMENUCTL_SetOemProperties(pMenu, OEMMP_DISTINGUISH_INFOKEY_SELECTKEY | OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SECURITY_BACKGROUND);
#endif
            IMENUCTL_SetBottomBarType(pMenu,BTBAR_SAVE_BACK);

			MSG_FATAL("***zzg SetDebugKeyDlg bDBGEnabled=%d***", pMe->mRM.bDBGEnabled, 0, 0);

			SetCheckBoxItem(pMenu, IDS_DBG_KEY, pMe->mRM.bDBGEnabled ? TRUE : FALSE);
			
			IMENUCTL_SetSel(pMenu, IDS_DBG_KEY);
			
			(void) ISHELL_PostEvent( pMe->m_pShell,
									 AEECLSID_BLUETOOTH_APP,
									 EVT_USER_REDRAW,
									 0,
									 0);
			return TRUE;
		}
		
		case EVT_USER_REDRAW:		
		{			
			//(void)IMENUCTL_Redraw(pMenu);		    //dele by yangdecai	
			return TRUE;
		}

		case EVT_DIALOG_END:
		{
			return TRUE;
		}

		case EVT_KEY_PRESS: 	  
		{					
			MSG_FATAL("***zzg HandleSetIOCapabilityDialogEvent EVT_KEY_PRESS wParam=%x***", wParam, 0, 0);
			
			switch(wParam)
			{
				case AVK_SELECT:	
				{
					pMe->bConfigChanged = TRUE;
					
					if (GetCheckBoxVal(pMenu, IDS_DBG_KEY))
					{
						pMe->mRM.bDBGEnabled = TRUE;
						IBTEXTRM_SetSM4DebugMode(pMe->mRM.po, pMe->mRM.bDBGEnabled);
					}
					else
					{
						pMe->mRM.bDBGEnabled = FALSE;
					}
					
					CLOSE_DIALOG(DLGRET_CANCELED)
					return TRUE;
				}				
				
				case AVK_CLR:
					CLOSE_DIALOG(DLGRET_CANCELED)
					return TRUE;

				default:
					break;
			}
			
			 return TRUE;
		}
		
		default:
			break;
	}

	return FALSE;		 

}


static boolean  HandleFtpDialogEvent(CBTApp * pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{	 
	PARAM_NOT_REF(dwParam)

    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_BT_FTP);	
	
    if (pMenu == NULL)
    {
        return FALSE;
    }

	MSG_FATAL("***zzg HandleMyInfoOpitionDialogEvent eCode=%x, wParam=%x, dwParam=%x***", eCode, wParam, dwParam); 
	
    switch (eCode)
    {
        case EVT_DIALOG_INIT:			
		{			
			AECHAR 		WTitle[40] = {0};			

			(void)ISHELL_LoadResString(pMe->m_pShell,
				                        AEE_APPSBTAPP_RES_FILE,                                
				                        IDS_FTP,
				                        WTitle,
				                        sizeof(WTitle));
			
			IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, WTitle);  

			if (pMe->mFTP.bRegistered == TRUE)
			{
				IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, IDS_FTP_SERVER, IDS_FTP_SERVER, NULL, 0);
			}
			else if (pMe->mFTP.bConnected == TRUE)
			{
				IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, IDS_FTP_CLIENT, IDS_FTP_CLIENT, NULL, 0);		
			}
			else
			{
				IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, IDS_FTP_SERVER, IDS_FTP_SERVER, NULL, 0);
				IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, IDS_FTP_CLIENT, IDS_FTP_CLIENT, NULL, 0);		
			}
				
			IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, IDS_SETTINGS, IDS_SETTINGS, NULL, 0);

            return TRUE;
		}

        case EVT_DIALOG_START:       
        {
			IMENUCTL_SetSel(pMenu, pMe->m_currDlgId);

            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);
            IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);

#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SETTING_BACKGROUND);
#endif
            IMENUCTL_SetBottomBarType(pMenu,BTBAR_OK_BACK);

            (void) ISHELL_PostEvent( pMe->m_pShell,
                                     AEECLSID_BLUETOOTH_APP,
                                     EVT_USER_REDRAW,
                                     0,
                                     0);
            return TRUE;
        }
		
        case EVT_USER_REDRAW:    			
        {
			//(void)IMENUCTL_Redraw(pMenu);     //dele by yangdecai
            return TRUE;
        }

        case EVT_DIALOG_END:
        {
			return TRUE;
       	}

        case EVT_KEY:
        {
			switch(wParam)
            {
                case AVK_CLR:
                {
					CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;
                }

                default:
                {
					break;
                }
            }
            return TRUE;
        }
		
        case EVT_COMMAND:           
        {
			pMe->m_currDlgId = wParam;		

			switch (wParam)
            {	
				case IDS_FTP_SERVER:
				{	
					CLOSE_DIALOG(DLGRET_FTP_SERVER)
					return TRUE;
				}	
				case IDS_FTP_CLIENT:
				{	
					CLOSE_DIALOG(DLGRET_FTP_CLIENT)
					return TRUE;
				}
				case IDS_SETTINGS:
				{	
					CLOSE_DIALOG(DLGRET_FTP_SETTING)
					return TRUE;
				}
				default:
				{
					ASSERT_NOT_REACHABLE;
				}
            }
            return TRUE;
        }
		
        default:
        {
			break;
        }
    }
    return FALSE;	

}


static boolean  HandleFtpServerDialogEvent(CBTApp * pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{	 
	PARAM_NOT_REF(dwParam)
			
    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_BT_FTP_SERVER);
		
    if (pMenu == NULL)
    {
        return FALSE;
    }	   

	MSG_FATAL("***zzg HandleFtpServerDialogEvent eCode=%x, wParam=%x, dwParam=%x***", eCode, wParam, dwParam); 

    switch (eCode)
    {
        case EVT_DIALOG_INIT:			
		{	
            return TRUE;
		}

        case EVT_DIALOG_START:   
		{
            IMENUCTL_SetSel(pMenu, pMe->m_currDlgId);

            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);
            IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);

#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SETTING_BACKGROUND);
#endif
            IMENUCTL_SetBottomBarType(pMenu, BTBAR_SELECT_BACK);

            (void) ISHELL_PostEvent( pMe->m_pShell,
                                     AEECLSID_BLUETOOTH_APP,
                                     EVT_USER_REDRAW,
                                     0,
                                     0);
            return TRUE;
        }
		
        case EVT_USER_REDRAW:   
		{			
			AECHAR 		WTitle[40] = {0};
			
			(void)ISHELL_LoadResString(pMe->m_pShell,
				                        AEE_APPSBTAPP_RES_FILE,                                
				                        IDS_FTP_SERVER,
				                        WTitle,
				                        sizeof(WTitle));

			if (NULL == pMe->m_pIAnn)
			{
				MSG_FATAL("***zzg HandleFtpServerDialogEvent NULL == pMe->m_pIAnn***", 0, 0, 0);
			}
			
			IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, WTitle);   
			
			IMENUCTL_DeleteAll(pMenu);

			if (pMe->mFTP.bRegistered == FALSE)
			{
				IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, IDS_REGISTER, IDS_REGISTER, NULL, 0);				
			}
			else
			{
				IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, IDS_DEREGISTER, IDS_DEREGISTER, NULL, 0);				
			}
  					
            (void)IMENUCTL_Redraw(pMenu);
            return TRUE;
        }

        case EVT_DIALOG_END:			
        {
			return TRUE;
        }

        case EVT_KEY:
        {
			switch(wParam)
            {
                case AVK_CLR:
                {
					CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;
                }

                default:
                {
					break;
                }
            }
            return TRUE;
		}

        case EVT_COMMAND:            
        {
			pMe->m_currDlgId = wParam;
			
            switch (wParam)
            {
            	case IDS_REGISTER:			
				{
					CLOSE_DIALOG(DLGRET_FTP_SERVER_REGISTER)
					break;
            	}	
					
				case IDS_DEREGISTER:	
				{
					boolean bRegistered = FALSE;
					
					if ((IBTEXTFTP_Deregister(pMe->mFTP.po)) != SUCCESS)
					{
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif 
						//MSG_FATAL( "FTP_Deregister() failed with %x", result, 0, 0 );					
						BTApp_ShowMessage(pMe, IDS_MSG_SVR_DEREG_FAILED, NULL, 3);
					}
					else
					{
						bRegistered = pMe->mFTP.bRegistered; 
						pMe->mFTP.bRegistered = FALSE;
						BTApp_CheckToClearDiscoverable(pMe);
						pMe->mFTP.bRegistered = bRegistered;
						BTApp_ShowBusyIcon(pMe); 
					}
					break;
				}
				
				default:
				{
					ASSERT_NOT_REACHABLE;
				}
            }
            return TRUE;
        }
		
        default:
        {
			break;
        }
    }
    return FALSE;	
}


static boolean  HandleFtpClientDialogEvent(CBTApp * pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{	 
    PARAM_NOT_REF(dwParam)
			
    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_BT_FTP_CLIENT);

	AECHAR  wBuf[AEEBT_MAX_FILE_NAME+1];
		
    if (pMenu == NULL)
    {
        return FALSE;
    }	   

    switch (eCode)
    {
        case EVT_DIALOG_INIT:			
		{	
            return TRUE;
		}

        case EVT_DIALOG_START:   
		{
            IMENUCTL_SetSel(pMenu, pMe->m_currDlgId);

            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);
            IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);

#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SETTING_BACKGROUND);
#endif
            IMENUCTL_SetBottomBarType(pMenu, BTBAR_SELECT_BACK);

            (void) ISHELL_PostEvent( pMe->m_pShell,
                                     AEECLSID_BLUETOOTH_APP,
                                     EVT_USER_REDRAW,
                                     0,
                                     0);
            return TRUE;
        }
		
        case EVT_USER_REDRAW:   
		{			
			AECHAR 		WTitle[40] = {0};
			
			(void)ISHELL_LoadResString(pMe->m_pShell,
				                        AEE_APPSBTAPP_RES_FILE,                                
				                        IDS_FTP_CLIENT,
				                        WTitle,
				                        sizeof(WTitle));

			if (NULL == pMe->m_pIAnn)
			{
				MSG_FATAL("***zzg HandleFtpClientDialogEvent NULL == pMe->m_pIAnn***", 0, 0, 0);
			}
			
			IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, WTitle);   
			
			IMENUCTL_DeleteAll(pMenu);

			MSG_FATAL("***zzg HandleFtpClientDialogEvent mFTP.bConnected=%d***", pMe->mFTP.bConnected, 0, 0);
			
			if (pMe->mFTP.bConnected == FALSE)
			{
				IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, IDS_CONNECT, IDS_CONNECT, NULL, 0);				
			}
			else
			{
				IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, IDS_PUT, IDS_PUT, NULL, 0);		
				IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, IDS_FTP_LIST_FOLDER, IDS_FTP_LIST_FOLDER, NULL, 0);		
				IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, IDS_DISCONNECT, IDS_DISCONNECT, NULL, 0);		
			}
  					
            (void)IMENUCTL_Redraw(pMenu);
            return TRUE;
        }

        case EVT_DIALOG_END:			
        {
			return TRUE;
        }

        case EVT_KEY:
        {
			switch(wParam)
            {
                case AVK_CLR:
                {
					CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;
                }

                default:
                {
					break;
                }
            }
            return TRUE;
        }

        case EVT_COMMAND:            
        {
			pMe->m_currDlgId = wParam;
			
			MSG_FATAL("***zzg FtpClientDlgEvent EVT_COMMAND wParam=%d***", wParam, 0, 0);
			
            switch (wParam)
            {
            	case IDS_CONNECT:			
				{		
					pMe->m_obex_list_id = IDD_BT_FTP_CLIENT;
					
					if ( pMe->mFTP.bRegistered == FALSE )
					{						
						AEEBTDeviceEnumerator enumerator;
						uint8				  i;						
						AEEBTDeviceInfo*	  pDev;						

						enumerator.control = AEEBT_RM_EC_MATCH_SERVICE_CLASS;
						enumerator.svcCls  = AEEBT_COD_SC_OBJECT_TRANSFER;
							
			
						if (pMe->mRM.po == NULL)
						{
							MSG_FATAL("***zzg ObexListServers pMe->mRM.po == NULL***", 0, 0, 0);
						}
						else
						{				
							if (IBTEXTRM_DeviceEnumInit(pMe->mRM.po, &enumerator) == SUCCESS)
							{		
								i	 = 0;
								pDev = &pMe->mRM.device[i];
			
								while ((IBTEXTRM_DeviceEnumNext( pMe->mRM.po, pDev ) == SUCCESS) 
										&& pDev->bValid && (i < MAX_DEVICES))
								{
									pDev = &pMe->mRM.device[ ++i ];
								}
							}
						}

						if (i > 0)
						{
							CLOSE_DIALOG(DLGRET_BT_OBEX_LIST_SERVERS)
						}
						else
						{
							BTApp_ShowMessage( pMe, IDS_MSG_NO_OBEX_SERVERS, NULL, 0 );
						}					
						
					}					
					break;
            	}
				
				case IDS_DISCONNECT:	
				{
					if ((IBTEXTFTP_Disconnect(pMe->mFTP.po)) != SUCCESS)
					{
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif 
						BTApp_ShowMessage( pMe, IDS_MSG_DISCONN_FAILED, NULL, 3 );
					}

					break;
				}
				
				case IDS_FTP_LIST_FOLDER:	
				{
					pMe->mFTP.bRemoteBrowsing = TRUE;

					// Store 'ROOT' as interop device root folder name is not known
					STRLCPY(szOperandex, ROOT_FOLDER_STR, sizeof(szOperandex));

					MSG_FATAL("***zzg BTApp FTP IDS_FTP_LIST_FOLDER***", 0, 0, 0);

					if (BTApp_FTPListFolder(pMe) != SUCCESS)
					{
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif 
						STRTOWSTR(szOperandex, wBuf, sizeof(wBuf));
						BTApp_ShowMessage(pMe, IDS_MSG_FOLDER_BROWSING_FAILED, wBuf, 3);
					}
					else
					{
						BTApp_ShowBusyIcon(pMe); // wait for LS done
					}

					break;
				}		
				
				case IDS_PUT:
				{
					pMe->mFTP.bRemoteBrowsing = FALSE;
					
					if (IFILEMGR_Test(pMe->mFTP.pIFileMgr, AEEFS_CARD0_DIR) == SUCCESS)		//Have  T Card
					{
						//	else if( pMe->mFTP.u8StorageDevice == STORAGE_DEV_MEM_CARD )
						{
							STRLCPY(pMe->mFTP.szLocalBrowsePath, AEEFS_CARD0_DIR, sizeof(pMe->mFTP.szLocalBrowsePath));
							STRLCPY(pMe->mFTP.szCurrentFolder, AEEFS_CARD0_DIR, sizeof(pMe->mFTP.szCurrentFolder));
	
							// Removing the terminal '/' character as this character is added 
							//during FTP operations. Otherwise, the path will contain "//". 
							pMe->mFTP.szLocalBrowsePath[STRLEN(AEEFS_CARD0_DIR) - 1] = '\0';
							pMe->mFTP.szCurrentFolder[STRLEN(AEEFS_CARD0_DIR) - 1] = '\0';

							BTApp_GetNameOfLocalObjects(pMe, pMe->mFTP.szLocalBrowsePath);	

							CLOSE_DIALOG(DLGRET_FTP_BROWSE)	
							return TRUE;
						}
					}
					else
					{
						BTApp_ShowMessage(pMe, IDS_INSERT_TCARD, wBuf, 3);
					}					
					
				
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif					
					break;
				}
				
				default:
				{
					ASSERT_NOT_REACHABLE;
				}
            }
            return TRUE;
        }
		
        default:
        {
			break;
        }
    }
	
    return FALSE;	
}

static boolean  HandleFtpSettingDialogEvent(CBTApp * pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{	 
	PARAM_NOT_REF(dwParam)
		
	IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_BT_FTP_SETTINGS);

	if (pMenu == NULL)
	{
		return FALSE;
	}	

	switch (eCode)
	{
		case EVT_DIALOG_INIT:			
		{
			AECHAR		WTitle[40] = {0};		
			
			(void)ISHELL_LoadResString(pMe->m_pShell,
										AEE_APPSBTAPP_RES_FILE, 							   
										IDS_SETTINGS,
										WTitle,
										sizeof(WTitle));
			
			IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, WTitle);  	
			
			IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, IDS_AUTHENTICATE, IDS_AUTHENTICATE, NULL, 0);	
		
			/*	
			//因为手机EFS有限，所以默认为MEMORY_CARD
			IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, IDS_EFS, IDS_EFS, NULL, 0);		
			IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, IDS_MEMORY_CARD, IDS_MEMORY_CARD, NULL, 0);	
			*/
			
			return TRUE;
		}

		case EVT_DIALOG_START:
		{
			//设定标题格式
            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_MULTI_SEL);
            IMENUCTL_SetOemProperties(pMenu, OEMMP_DISTINGUISH_INFOKEY_SELECTKEY | OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SECURITY_BACKGROUND);
#endif
            IMENUCTL_SetBottomBarType(pMenu, BTBAR_SAVE_BACK);
		
			SetCheckBoxItem(pMenu, IDS_AUTHENTICATE, pMe->mFTP.bDoAuthenticate ? TRUE : FALSE);
			
			(void) ISHELL_PostEvent( pMe->m_pShell,
									 AEECLSID_BLUETOOTH_APP,
									 EVT_USER_REDRAW,
									 0,
									 0);
			return TRUE;
		}
		
		case EVT_USER_REDRAW:		
		{			
			//(void)IMENUCTL_Redraw(pMenu);	    //dele by yangdecai		
			return TRUE;
		}

		case EVT_DIALOG_END:
		{
			return TRUE;
		}

		case EVT_KEY_PRESS: 	  
		{	
			switch(wParam)
			{
				case AVK_SELECT:	
				{
					pMe->bConfigChanged = TRUE;

					if (GetCheckBoxVal(pMenu, IDS_AUTHENTICATE))
					{
						pMe->mFTP.bDoAuthenticate = TRUE;						
					}
					else
					{
						pMe->mFTP.bDoAuthenticate = FALSE;		
					}

					BTApp_WriteConfigFile( pMe );
					
					CLOSE_DIALOG(DLGRET_CANCELED)
					return TRUE;
				}				
				
				case AVK_CLR:
				{
					CLOSE_DIALOG(DLGRET_CANCELED)
					return TRUE;
				}

				default:
				{
					break;
				}
			}
			
			return TRUE;
		}
		
		default:
		{
			break;
		}
	}
    return FALSE;	
}


static boolean  HandleFtpServerRegisterDialogEvent(CBTApp * pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{	 
	PARAM_NOT_REF(dwParam)
		
	IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_BT_FTP_SERVER_REGISTER);

	if (pMenu == NULL)
	{
		return FALSE;
	}	

	switch (eCode)
	{
		case EVT_DIALOG_INIT:			
		{
			AECHAR		WTitle[40] = {0};		
			
			(void)ISHELL_LoadResString(pMe->m_pShell,
										AEE_APPSBTAPP_RES_FILE, 							   
										IDS_REGISTER,
										WTitle,
										sizeof(WTitle));
			
			IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, WTitle);  	
			
			IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, IDS_FTP_SRV_ALLOW_BROWSE, IDS_FTP_SRV_ALLOW_BROWSE, NULL, 0);	
			IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, IDS_FTP_SRV_READ_ONLY, IDS_FTP_SRV_READ_ONLY, NULL, 0);	
		
			return TRUE;
		}

		case EVT_DIALOG_START:
		{
			//设定标题格式
            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_MULTI_SEL);
            IMENUCTL_SetOemProperties(pMenu, OEMMP_DISTINGUISH_INFOKEY_SELECTKEY | OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SECURITY_BACKGROUND);
#endif
            IMENUCTL_SetBottomBarType(pMenu, BTBAR_SAVE_BACK);
		
			SetCheckBoxItem(pMenu, IDS_FTP_SRV_ALLOW_BROWSE, pMe->mFTP.reg.bBrowseAllowed ? TRUE : FALSE);
			SetCheckBoxItem(pMenu, IDS_FTP_SRV_READ_ONLY, pMe->mFTP.reg.bReadOnly ? TRUE : FALSE);

			IMENUCTL_SetSel(pMenu, IDS_FTP_SRV_ALLOW_BROWSE);
			
			(void) ISHELL_PostEvent( pMe->m_pShell,
									 AEECLSID_BLUETOOTH_APP,
									 EVT_USER_REDRAW,
									 0,
									 0);
			return TRUE;
		}
		
		case EVT_USER_REDRAW:		
		{			
			//(void)IMENUCTL_Redraw(pMenu);		    //dele by yangdecai	
			return TRUE;
		}

		case EVT_DIALOG_END:
		{
			return TRUE;
		}

		case EVT_KEY_PRESS: 	  
		{	
			//uint16 selection = IMENUCTL_GetSel(pMenu);
			//MENU_SET_SEL(selection);
			
			switch(wParam)
			{
				case AVK_CLR:
				case AVK_SELECT:	
				{
					if (GetCheckBoxVal(pMenu, IDS_FTP_SRV_ALLOW_BROWSE))
					{
						pMe->mFTP.reg.bBrowseAllowed = TRUE;						
					}
					else
					{
						pMe->mFTP.reg.bBrowseAllowed = FALSE;		
					}				

					if (GetCheckBoxVal(pMenu, IDS_FTP_SRV_READ_ONLY))
					{
						pMe->mFTP.reg.bReadOnly = TRUE;						
					}
					else
					{
						pMe->mFTP.reg.bReadOnly = FALSE;		
					}

					if ( pMe->mFTP.bDoAuthenticate == FALSE )
					{
						pMe->mFTP.reg.auth         = AEEBT_FTP_AUTH_NONE;
					}
					else
					{
						pMe->mFTP.reg.auth         = AEEBT_FTP_AUTH_PASSWORD;
					}
					
					STRLCPY(pMe->mFTP.reg.szServiceName, "QC FTP", sizeof(pMe->mFTP.reg.szServiceName));

					MSG_LOW("FTP_Register() - auth=%x %d %d", pMe->mFTP.reg.auth, pMe->mFTP.reg.bReadOnly, pMe->mFTP.reg.bBrowseAllowed);
					
					BTApp_SetBondable(pMe);
										
					if ((IBTEXTFTP_Register(pMe->mFTP.po, &pMe->mFTP.reg)) != SUCCESS)
					{
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif

						MSG_FATAL("***zzg HandleFTPServerRegisterDlg EVT_KEY_PRESS IBTEXTFTP_Register != SUCCESS***", 0, 0, 0);
						BTApp_ClearBondable( pMe ); // no need to be bondable anymore
						
						BTApp_ShowMessage( pMe, IDS_MSG_SVR_REG_FAILED, NULL, 3);
					}
					else
					{
						MSG_FATAL("***zzg HandleFTPServerRegisterDlg EVT_KEY_PRESS IBTEXTFTP_Register == SUCCESS***", 0, 0, 0);
						
						if ( pMe->mSD.bDiscoverable == FALSE )
						{
							IBTEXTSD_SetDiscoverable( pMe->mSD.po, TRUE );
						}
						
						BTApp_ShowBusyIcon( pMe ); // wait for command done
					}
					
					CLOSE_DIALOG(DLGRET_CANCELED)	
					return TRUE;
				}				
								
				default:
					break;
			}
			
			 return TRUE;
		}
		default:
			break;
	}
    return FALSE;	
}


static boolean HandleFtpBrowseDialogEvent(CBTApp *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)	
{	 
	PARAM_NOT_REF(dwParam)

	IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_BT_FTP_BROWSE);	
	
	if (pMenu == NULL)
	{
		return FALSE;
	}
	
	MSG_FATAL("***zzg HandleFtpBrowseDialogEvent eCode=%x, wParam=%x, dwParam=%x",eCode,wParam,dwParam);   

	switch (eCode)
	{
		case EVT_DIALOG_INIT:			
		{	
			return TRUE;
		}
		
		case EVT_DIALOG_START:
		{			
			CtlAddItem ai;
			AEERect	 rc;
			uint8 	 uNoOfItems = 0;
			AECHAR	 wName[ AEEBT_MAX_FILE_NAME+1 ];

			AECHAR	WTitle[40] = {0};						
			
			if (pMe->mFTP.bRemoteBrowsing)
			{
				(void)ISHELL_LoadResString(pMe->m_pShell,
											AEE_APPSBTAPP_RES_FILE, 							   
											IDS_FTP_LIST_FOLDER,
											WTitle,
											sizeof(WTitle));	

				if (pMe->m_pIAnn != NULL)
				{
					IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, WTitle); 
				}				

				uNoOfItems = pMe->mFTP.uNumOfObjInRemoteDev;
			}
			else
			{
				STRTOWSTR( "Local Browsing", wName, sizeof(wName));				
				
				if (pMe->m_pIAnn != NULL)
				{
					IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, wName);
				}

				uNoOfItems = pMe->mFTP.uNumOfObjInLocalDev;
			}

			MSG_FATAL("***zzg uNumOfObjInRemoteDev=%d, uNumOfObjInLocalDev=%d***", pMe->mFTP.uNumOfObjInRemoteDev, pMe->mFTP.uNumOfObjInLocalDev, 0);			
			
			MSG_FATAL("***zzg FtpBrowseDlg uNoOfItems=%d ***", uNoOfItems, 0, 0);

			while (uNoOfItems > 0)
			{				
				if (pMe->mFTP.bRemoteBrowsing)
				{
					//STRTOWSTR(pMe->mFTP.remoteObjects[pMe->mFTP.uNumOfObjInRemoteDev-uNoOfItems].szFolderFileName, wName, sizeof(wName));

					//Add By zzg 2011_12_29	
				   	{						
						UTF8TOWSTR((byte*)pMe->mFTP.remoteObjects[pMe->mFTP.uNumOfObjInRemoteDev-uNoOfItems].szFolderFileName, 
									AEEBT_MAX_FILE_NAME + 1, 
									wName, 	
									(AEEBT_MAX_FILE_NAME + 1)*sizeof(AECHAR));	

						DBGPRINTF("***zzg IMenuCtl_AddItem wName=%s***", wName);	
					}	
					//Add End
										
					// Add the folder / file names as menu items
					IMENUCTL_AddItem(pMenu, NULL, 0, pMe->mFTP.uNumOfObjInRemoteDev - uNoOfItems, wName, 0);					
				}
				else
				{
					//STRTOWSTR(pMe->mFTP.localObjects[pMe->mFTP.uNumOfObjInLocalDev-uNoOfItems].szFolderFileName, wName, sizeof(wName));
			
					//Add By zzg 2011_12_29	
				   	{						
						UTF8TOWSTR((byte*)pMe->mFTP.localObjects[pMe->mFTP.uNumOfObjInLocalDev-uNoOfItems].szFolderFileName, 
									AEEBT_MAX_FILE_NAME + 1, 
									wName, 	
									(AEEBT_MAX_FILE_NAME + 1)*sizeof(AECHAR));	
						
						DBGPRINTF("***zzg IMenuCtl_AddItem pText=%s***", wName);	
					}	
					//Add End
					
					// Add the folder / file names as menu items
					IMENUCTL_AddItem(pMenu, NULL, 0, pMe->mFTP.uNumOfObjInLocalDev - uNoOfItems, wName, 0);
				}

				uNoOfItems--;
			}
			

#ifdef FEATURE_APP_TEST_AUTOMATION
		#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION

			IMENUCTL_SetSel(pMenu, pMe->m_currDlgId);

			IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);
			IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);

#ifdef FEATURE_CARRIER_CHINA_VERTU
			IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SETTING_BACKGROUND);
#endif
			
			if (pMe->mFTP.bRemoteBrowsing)
			{
				IMENUCTL_SetBottomBarType(pMenu, BTBAR_OPTION_BACK);
			}
			else
			{
				IMENUCTL_SetBottomBarType(pMenu, BTBAR_OK_BACK);
			}

			IMENUCTL_SetSel(pMenu,	MENU_SEL);	// highlight the selected item

			(void) ISHELL_PostEvent( pMe->m_pShell,
									 AEECLSID_BLUETOOTH_APP,
									 EVT_USER_REDRAW,
									 0,
									 0);			

			(void)IMENUCTL_Redraw(pMenu);			
			IMENUCTL_SetActive(pMenu, TRUE);
			IDISPLAY_UpdateEx(pMe->m_pIDisplay, FALSE); 	
			
			return TRUE;
		}
		
		case EVT_USER_REDRAW:	 
		{	
			(void)IMENUCTL_Redraw(pMenu);			
			IMENUCTL_SetActive(pMenu, TRUE);
			IDISPLAY_UpdateEx(pMe->m_pIDisplay, FALSE); 		
			return TRUE;
		}
				
		case EVT_DIALOG_END:
		{
			return TRUE;
		}

		case EVT_KEY_PRESS:
		{
			uint8   uIndex = 0x00; 
			boolean ev_processed = TRUE;
			char    szName[AEEBT_MAX_FILE_NAME+1];
			AECHAR  wName[AEEBT_MAX_FILE_NAME+1]; 
			AECHAR  wBuf[AEEBT_MAX_FILE_NAME+1];
			int     result=0;
			
			if (pMe->mFTP.bRemoteBrowsing)
			{
				switch(wParam)
				{
					case AVK_SELECT:	//Opition					
					{
						int			count;
						int 		curItem;
						uint16		curItemId;
						CtlAddItem	ai;

						count = IMENUCTL_GetItemCount(pMenu);					

						//Add By zzg 2011_02_21
						if (count > 0)
						{
							uIndex = IMENUCTL_GetSel(pMenu);

							for (curItem = 0; curItem < count; curItem ++)
							{
								curItemId = IMENUCTL_GetItemID(pMenu, curItem);	

								if (curItemId == uIndex)
								{
									pMe->m_folder_index = curItem;
								}							
							}
						}
						//Add End
						
						CLOSE_DIALOG(DLGRET_FTP_BROWSE_OPITION)
						return TRUE;
					}			

					case AVK_INFO:	
					{		
						if (IMENUCTL_GetItemCount(pMenu) > 0)					
						{
							uIndex = IMENUCTL_GetSel(pMenu);
																				
							// Get the name of the menu item selected on the screen
							//STRTOWSTR(pMe->mFTP.remoteObjects[uIndex].szFolderFileName, wName, sizeof(wName)); 

							//Add By zzg 2011_12_30
							UTF8TOWSTR((byte*)pMe->mFTP.remoteObjects[uIndex].szFolderFileName, 
										AEEBT_MAX_FILE_NAME + 1, 
										wName, 	
										(AEEBT_MAX_FILE_NAME + 1)*sizeof(AECHAR));	
							//Add End
							


							// If folder,  browse the folder 
							if (pMe->mFTP.remoteObjects[uIndex].szTypeOfObj == AEEBT_FTP_FOLDER)
							{
								if (IBTEXTFTP_SetPath(pMe->mFTP.po, wName, AEEBT_FTP_SET_PATH_TO_FOLDER) != SUCCESS)
								{
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif 
									MSG_ERROR("Set Path to folder failed", 0, 0, 0);
									BTApp_ShowMessage(pMe, IDS_MSG_FOLDER_BROWSING_FAILED, wName, 3);
								}
								else
								{
									// Storing folder name to display it later in case of failure
									STRLCPY(szOperandex, pMe->mFTP.remoteObjects[uIndex].szFolderFileName, sizeof(szOperandex));
									BTApp_ShowBusyIcon(pMe); // wait for connect confirm
								}
							}			
							
						}

						//CLOSE_DIALOG(DLGRET_CANCELED)
						return TRUE;
					}
					
					case AVK_CLR:
					{					
						MSG_FATAL("***zzg FtpBrowseDlgEvt AVK_CLR bObjectTransfer=%d***", pMe->mFTP.bObjectTransfer, 0, 0);
						
						if (pMe->mFTP.bObjectTransfer)
						{
							result = IBTEXTFTP_Abort(pMe->mFTP.po);
							if (result != SUCCESS)
							{
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif 
								MSG_MED("Abort failed %d", result, 0, 0);
								BTApp_ShowMessage(pMe, IDS_MSG_ABORT_FAILED, 0, 2);
							}
							else
							{
								pMe->mFTP.bObjectTransfer = FALSE;
								MSG_MED("Aborted", 0, 0, 0);
							}
						}
						else
						{							
							// Tries to set server's path back to parent and list the folder 
							// contents of parent. This is for use case scenario when the user 
							// migrates from one browsing menu to another backwards. 
							// There is no need to handle clear key as that will be handled as 
							// soon as CD fails. 
							if (IBTEXTFTP_SetPath(pMe->mFTP.po, NULL, AEEBT_FTP_SET_PATH_TO_PARENT) != SUCCESS)
							{
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif 								
								MSG_ERROR("Set Path to parent failed", 0, 0, 0);
								MSG_FATAL("***zzg FtpBrowseDlgEvt IBTEXTFTP_SetPath Failed!***", 0, 0, 0);								
							}
							else
							{
								MSG_FATAL("***zzg FtpBrowseDlgEvt IBTEXTFTP_SetPath Succeed!***", 0, 0, 0);
								
								// Store 'PARENT' as interop device parent folder name is not known								
								STRLCPY(szOperandex, PARENT_FOLDER_STR, sizeof(szOperandex));
								BTApp_ShowBusyIcon(pMe); // wait for connect confirm									
							}
							
						}

						CLOSE_DIALOG(DLGRET_FTP_BROWSE)		  	
						//CLOSE_DIALOG(DLGRET_CANCELED)
						return TRUE;						
					}

					default:
						break;
				}
			}			
			else
			{
				switch(wParam)
				{
					case AVK_SELECT:	
					case AVK_INFO:		
					{
						if (IMENUCTL_GetItemCount(pMenu) > 0)
						{
							uIndex = IMENUCTL_GetSel(pMenu);
							MSG_FATAL("***zzg FtpBrowseDlgEvt uIndex=%d, szTypeOfObj=%d***", uIndex, pMe->mFTP.localObjects[uIndex].szTypeOfObj, 0);

							DBGPRINTF("***zzg FtpBrowseDlgEvt szFolderFileName=%s***", pMe->mFTP.localObjects[uIndex].szFolderFileName);
														
							//STRTOWSTR(pMe->mFTP.localObjects[uIndex].szFolderFileName, wName, sizeof(wName));
							
							//Add By zzg 2011_12_29	
						   	{						
								UTF8TOWSTR((byte*)pMe->mFTP.localObjects[uIndex].szFolderFileName, 
											AEEBT_MAX_FILE_NAME + 1, 
											wName, 	
											(AEEBT_MAX_FILE_NAME + 1)*sizeof(AECHAR));	

								DBGPRINTF("***zzg IMenuCtl_AddItem wName=%s***", wName);	
							}	
							//Add End

							if (pMe->mFTP.localObjects[uIndex].szTypeOfObj == AEEBT_FTP_FILE)
							{
								if (STRLEN(pMe->mFTP.szLocalBrowsePath) + STRLEN(DIRECTORY_STR) + 
									STRLEN(pMe->mFTP.localObjects[uIndex].szFolderFileName) > 
									AEEBT_MAX_FILE_NAME)
								{
									MSG_ERROR("File name exceeds max", 0, 0, 0);
									BTApp_ShowMessage(pMe, IDS_MSG_FILE_PUT_FAILED, wName, 3);
									ev_processed = FALSE;
									
									return TRUE;
								}

								STRLCPY(szName, pMe->mFTP.szLocalBrowsePath, sizeof(szName));
								STRLCAT(szName, DIRECTORY_STR, sizeof(szName));
								STRLCAT(szName, pMe->mFTP.localObjects[uIndex].szFolderFileName, sizeof(szName));
								
								//STRTOWSTR(szName, wBuf, sizeof(wBuf));
								
								//Add By zzg 2011_12_29	
							   	{						
									UTF8TOWSTR((byte*)szName, 
												AEEBT_MAX_FILE_NAME + 1, 
												wBuf, 	
												(AEEBT_MAX_FILE_NAME + 1)*sizeof(AECHAR));	

									DBGPRINTF("***zzg IMenuCtl_AddItem wBuf=%s***", wBuf);	
								}	
								//Add End
								
								STRLCPY(szOperandex, pMe->mFTP.localObjects[uIndex].szFolderFileName, sizeof(szOperandex));

								if ((result = IBTEXTFTP_Put(pMe->mFTP.po, pMe->mFTP.localObjects[uIndex].szTypeOfObj, wBuf, wName)) != SUCCESS)
								{
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif 
									MSG_MED("Put file failed. Result=%d", result, 0, 0);
									DBGPRINTF("***zzg IBTEXTFTP_Put result=%d***", result);
									BTApp_ShowMessage(pMe, IDS_MSG_FILE_PUT_FAILED, wName, 3);
									ev_processed = FALSE;
									return TRUE;
								}
								else         
								{
									pMe->mFTP.bObjectTransfer = TRUE;
									//ShowBusyIcon(pMe->a.m_pIShell, pMe->a.m_pIDisplay, &pMe->m_rect, FALSE);

									//CLOSE_DIALOG(DLGRET_CANCELED)
									return TRUE;
								}
								
							}

							// Folder name having more than AEEBT_MAX_FILE_NAME characters will 
							//not be stored at all and hence there is no need to check for it 
							if (STRLEN(pMe->mFTP.szLocalBrowsePath) + STRLEN(DIRECTORY_STR) + 
								STRLEN(pMe->mFTP.localObjects[uIndex].szFolderFileName) > 
								AEEBT_MAX_FILE_NAME * 2)
							{
								MSG_ERROR("Folder name path exceeds max*2", 0, 0, 0);
								BTApp_ShowMessage(pMe, IDS_MSG_FOLDER_BROWSING_FAILED, wName, 3);
								ev_processed = FALSE;

								CLOSE_DIALOG(DLGRET_CANCELED)
								return TRUE;
							}

							STRLCAT(pMe->mFTP.szLocalBrowsePath, DIRECTORY_STR, sizeof(pMe->mFTP.szLocalBrowsePath));
							STRLCAT(pMe->mFTP.szLocalBrowsePath, pMe->mFTP.localObjects[uIndex].szFolderFileName, sizeof(pMe->mFTP.szLocalBrowsePath));

							//Local browse path is copied on to szCurrentFolder so that PUT of a
							//local device subfolder points to the right path to read and PUT the 
							//contents on to the server 
							STRLCPY(pMe->mFTP.szCurrentFolder, pMe->mFTP.szLocalBrowsePath, sizeof(pMe->mFTP.szCurrentFolder));

							MSG_FATAL("***zzg bRemoteBrowsing=%d xxx***", pMe->mFTP.bRemoteBrowsing, 0, 0);

							BTApp_GetNameOfLocalObjects(pMe, pMe->mFTP.szLocalBrowsePath);
							
							MSG_FATAL("***zzg bRemoteBrowsing=%d yyy***", pMe->mFTP.bRemoteBrowsing, 0, 0);
							
							CLOSE_DIALOG(DLGRET_FTP_BROWSE)								
														
							//BTApp_BuildMenu(pMe, BT_APP_MENU_FTP_BROWSING);

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
						}

						return TRUE;
					}

					case AVK_RIGHT:
					{
						if (IMENUCTL_GetItemCount(pMenu) > 0)
						{
							uIndex = IMENUCTL_GetSel(pMenu);

							//STRTOWSTR(pMe->mFTP.localObjects[uIndex].szFolderFileName, wName, sizeof(wName));

							//Add By zzg 2011_12_29	
						   	{						
								UTF8TOWSTR((byte*)pMe->mFTP.localObjects[uIndex].szFolderFileName, 
											AEEBT_MAX_FILE_NAME + 1, 
											wName, 	
											(AEEBT_MAX_FILE_NAME + 1)*sizeof(AECHAR));	

								DBGPRINTF("***zzg IMenuCtl_AddItem wName=%s***", wName);	
							}	
							//Add End

							if (STRLEN(pMe->mFTP.szLocalBrowsePath) + STRLEN(DIRECTORY_STR) + 
								STRLEN(pMe->mFTP.localObjects[uIndex].szFolderFileName) > 
								AEEBT_MAX_FILE_NAME)
							{
								MSG_ERROR( "File / Folder name exceeds max", 0, 0, 0 );
								BTApp_ShowMessage( pMe, IDS_MSG_FILE_PUT_FAILED, wName, 3 );
								ev_processed = FALSE;

								CLOSE_DIALOG(DLGRET_CANCELED)
								return TRUE;
							}

							STRLCPY(szName, pMe->mFTP.szLocalBrowsePath, sizeof(szName));
							STRLCAT(szName, DIRECTORY_STR, sizeof(szName));
							STRLCAT(szName, pMe->mFTP.localObjects[uIndex].szFolderFileName, sizeof(szName));
							DBGPRINTF("PUT file/folder name is %s", szName);

							//STRTOWSTR(szName, wBuf, sizeof(wBuf));

							//Add By zzg 2011_12_29	
						   	{						
								UTF8TOWSTR((byte*)szName, 
											AEEBT_MAX_FILE_NAME + 1, 
											wBuf, 	
											(AEEBT_MAX_FILE_NAME + 1)*sizeof(AECHAR));	

								DBGPRINTF("***zzg IMenuCtl_AddItem wBuf=%s***", wBuf);	
							}	
							//Add End
							
							STRLCPY(szOperandex, pMe->mFTP.localObjects[uIndex].szFolderFileName, sizeof(szOperandex));

							if ((result = IBTEXTFTP_Put(pMe->mFTP.po, pMe->mFTP.localObjects[uIndex].szTypeOfObj, wBuf, wName)) != SUCCESS)
							{
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif

								MSG_MED( "Put file/folder failed. Result=0x%d", result, 0, 0 );
								BTApp_ShowMessage( pMe, IDS_MSG_FILE_PUT_FAILED, wName, 3 );
								ev_processed = FALSE;
							}
							else
							{
								pMe->mFTP.bObjectTransfer = TRUE;
								//ShowBusyIcon(pMe->a.m_pIShell, pMe->a.m_pIDisplay, &pMe->m_rect, FALSE);
							}
						}

						CLOSE_DIALOG(DLGRET_CANCELED)
						return TRUE;
					}
					
						
					case AVK_CLR:
					{
						MSG_FATAL("***zzg FtpBrowseDlgEvt LocalBrowse AVK_CLR bObjectTransfer=%d***", pMe->mFTP.bObjectTransfer, 0, 0);

						DBGPRINTF("***zzg szLocalBrowsePath=%s, szCurrentFolder=%s***", pMe->mFTP.szLocalBrowsePath, pMe->mFTP.szCurrentFolder);
						
						if (pMe->mFTP.bObjectTransfer)
						{
							result = IBTEXTFTP_Abort(pMe->mFTP.po);
							if (result != SUCCESS)
							{
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif 
								MSG_MED( "Abort failed %d", result, 0, 0 );
								BTApp_ShowMessage(pMe, IDS_MSG_ABORT_FAILED, 0, 2);
							}
							else
							{
								pMe->mFTP.bObjectTransfer = FALSE;
								MSG_MED("Aborted", 0, 0, 0);
							} 
						}
						else if (STRCMP(pMe->mFTP.szLocalBrowsePath, CARD0_DIR) != 0)
						{						
							//Add By zzg 2011_02_24									
							char *pszTemp; 
							char path[AEEBT_MAX_FILE_NAME * 2 + 1];
							uint8 tmplen;
							uint8 pathlen;

							DBGPRINTF("***zzg szLocalBrowsePath=%s***", pMe->mFTP.szLocalBrowsePath);
							DBGPRINTF("***zzg szCurrentFolder=%s***", pMe->mFTP.szCurrentFolder);
							
							pszTemp = STRRCHR(pMe->mFTP.szLocalBrowsePath, DIRECTORY_CHAR);
							tmplen = STRLEN(pszTemp);
							pathlen = STRLEN(pMe->mFTP.szCurrentFolder);


							DBGPRINTF("***zzg pathlen=%d, tmplen=%d***", pathlen, tmplen);
							DBGPRINTF("***zzg pszTemp=%s, szLocalBrowsePath=%s***", pszTemp, pMe->mFTP.szLocalBrowsePath);

							
							//STRLCPY(pMe->mFTP.szCurrentFolder, pszTemp, sizeof(pMe->mFTP.szCurrentFolder));
							STRLCPY(path, pMe->mFTP.szLocalBrowsePath, (pathlen-tmplen+1)*sizeof(char));								
							MEMSET(pMe->mFTP.szLocalBrowsePath, 0, sizeof(pMe->mFTP.szLocalBrowsePath));							
							STRLCPY(pMe->mFTP.szLocalBrowsePath, path, sizeof(pMe->mFTP.szLocalBrowsePath));	

							STRLCPY(pMe->mFTP.szCurrentFolder, pMe->mFTP.szLocalBrowsePath, sizeof(pMe->mFTP.szCurrentFolder));
							

							DBGPRINTF("***zzg pszTemp=%s, path=%s, szLocalBrowsePath=%s***", pszTemp, path, pMe->mFTP.szLocalBrowsePath);

							DBGPRINTF("***zzg pszTemp=%s, path=%s***", pszTemp, path);

							
							BTApp_GetNameOfLocalObjects(pMe, pMe->mFTP.szLocalBrowsePath);

							CLOSE_DIALOG(DLGRET_FTP_BROWSE)										
							return TRUE;
							//Add End
							
														
						}

						CLOSE_DIALOG(DLGRET_CANCELED)						
						return TRUE;
					}
					default:
					{
						break;
					}
				}
			}			
			
			return TRUE;		
		}

		default:
		{
			break;
		}
	}
	return FALSE;
}	

static boolean HandleFtpBrowseOpitionDialogEvent(CBTApp * pMe,AEEEvent eCode,uint16 wParam,uint32 dwParam)
{	 
	PARAM_NOT_REF(dwParam)
		
	IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_BT_FTP_BROWSE_OPITION);

	if (pMenu == NULL)
	{
		return FALSE;
	}	

	switch (eCode)
	{
		case EVT_DIALOG_INIT:			
		{
			AECHAR		WTitle[40] = {0};		
			
			(void)ISHELL_LoadResString(pMe->m_pShell,
										AEE_APPSBTAPP_RES_FILE, 							   
										IDS_OPTIONS,
										WTitle,
										sizeof(WTitle));
			
			IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, WTitle);  	
		
			if (pMe->mFTP.bRemoteBrowsing)
			{
				IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, IDS_BT_SELECT, IDS_BT_SELECT, NULL, 0);		
				IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, IDS_GET, IDS_GET, NULL, 0);		
				IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, IDS_DELETE, IDS_DELETE, NULL, 0);	
				IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, IDS_FTP_CREATE_FOLDER, IDS_FTP_CREATE_FOLDER, NULL, 0);	
				IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, IDS_FTP_MOVE_TO_FOLDER, IDS_FTP_MOVE_TO_FOLDER, NULL, 0);	
				IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, IDS_FTP_MOVE_TO_ROOT, IDS_FTP_MOVE_TO_ROOT, NULL, 0);	
				IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, IDS_FTP_CLIENT, IDS_FTP_CLIENT, NULL, 0);	
			}
			
			return TRUE;
		}

		case EVT_DIALOG_START:
		{
			IMENUCTL_SetSel(pMenu, pMe->m_currDlgId);

            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);
            IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);

#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SETTING_BACKGROUND);
#endif
            IMENUCTL_SetBottomBarType(pMenu, BTBAR_OK_BACK);

            (void) ISHELL_PostEvent( pMe->m_pShell,
                                     AEECLSID_BLUETOOTH_APP,
                                     EVT_USER_REDRAW,
                                     0,
                                     0);
            return TRUE;
		}
		
		case EVT_USER_REDRAW:		
		{			
			//(void)IMENUCTL_Redraw(pMenu);		    //dele by yangdecai	
			return TRUE;
		}

		case EVT_DIALOG_END:
		{
			return TRUE;
		}

		case EVT_KEY_PRESS: 	  
		{	
			switch (wParam)
			{
				case AVK_CLR:
				{
					CLOSE_DIALOG(DLGRET_CANCELED)
					return TRUE;
				}

				default:
				{
					break;
				}
			}
			
			return TRUE;
		}
		
		case EVT_COMMAND:	
		{			
			uint8   uIndex = 0x00; 
			boolean ev_processed = TRUE;
			AECHAR  wName[AEEBT_MAX_FILE_NAME+1];
			AECHAR  wBuf[AEEBT_MAX_FILE_NAME+1];
			int     result = 0;
		
			pMe->m_currDlgId = wParam;				
			
			switch (wParam)
			{			
				case IDS_BT_SELECT:
				{
					if (IFILEMGR_Test(pMe->mFTP.pIFileMgr, AEEFS_CARD0_DIR) != SUCCESS)		//Have  not T Card
					{
						BTApp_ShowMessage(pMe, IDS_INSERT_TCARD, wBuf, 3);
					}
					else
					{
						int			count;
						int 		curItem;
						uint16		curItemId;
						CtlAddItem	ai;
						
						count = IMENUCTL_GetItemCount(pMenu);					
						
						if (IMENUCTL_GetItemCount(pMenu) > 0)					
						{
							MSG_FATAL("***zzg FtpBrowseOpitionDlgEvt IDS_BT_SELECT m_folder_index=%d***", pMe->m_folder_index, 0, 0);
							
							uIndex = pMe->m_folder_index;
							
							// Get the name of the menu item selected on the screen
							//STRTOWSTR(pMe->mFTP.remoteObjects[uIndex].szFolderFileName, wName, sizeof(wName)); 

							//Add By zzg 2011_12_29	
						   	{						
								UTF8TOWSTR((byte*)pMe->mFTP.remoteObjects[uIndex].szFolderFileName, 
											AEEBT_MAX_FILE_NAME + 1, 
											wName, 	
											(AEEBT_MAX_FILE_NAME + 1)*sizeof(AECHAR));	

								DBGPRINTF("***zzg IMenuCtl_AddItem wName=%s***", wName);	
							}	
							//Add End
							

							MSG_FATAL("***zzg FtpBrowseOpition uIndex=%x, szTypeOfObj=%x***", 
										uIndex, pMe->mFTP.remoteObjects[uIndex].szTypeOfObj, 0);

							// Check whether the selected item is a folder or file. If file, 
							//then GET the file. If folder, then browse the folder 
							if (pMe->mFTP.remoteObjects[uIndex].szTypeOfObj == AEEBT_FTP_FILE)
							{
								//if (pMe->mFTP.u8StorageDevice == STORAGE_DEV_MEM_CARD)
								//Default Device is STORAGE_DEV_MEM_CARD
								{
									STRLCPY(szOperandex, AEEFS_CARD0_DIR, sizeof(szOperandex));
								}
								
								MSG_FATAL("***zzg szFolderFileName=%s***", pMe->mFTP.remoteObjects[uIndex].szFolderFileName, 0, 0);

								if ((STRLEN(szOperandex) + 
									STRLEN(pMe->mFTP.remoteObjects[uIndex].szFolderFileName )) > 
									AEEBT_MAX_FILE_NAME) 
								{
									MSG_ERROR( "File / Folder name exceeds max", 0, 0, 0 );
									MSG_FATAL("***zzg File / Folder name exceeds max***", 0, 0, 0);
									BTApp_ShowMessage( pMe, IDS_MSG_FILE_GET_FAILED, wName, 3 );
									break;            
								}

								STRLCAT(szOperandex, pMe->mFTP.remoteObjects[uIndex].szFolderFileName, sizeof(szOperandex));
								// Remove if the file already exists
								if (IFILEMGR_Test(pMe->mFTP.pIFileMgr, szOperandex) == SUCCESS)
								{
									if (IFILEMGR_Remove(pMe->mFTP.pIFileMgr, szOperandex) != SUCCESS)
									{
										DBGPRINTF("Could not remove the file %s", szOperandex);
									}
								}

								//STRTOWSTR(szOperandex, wBuf, sizeof(wBuf));
								
								//Add By zzg 2011_12_29	
							   	{						
									UTF8TOWSTR((byte*)szOperandex, 
												AEEBT_MAX_FILE_NAME + 1, 
												wBuf, 	
												(AEEBT_MAX_FILE_NAME + 1)*sizeof(AECHAR));	

									DBGPRINTF("***zzg IMenuCtl_AddItem wBuf=%s***", wBuf);	
								}	
								//Add End

								if (IBTEXTFTP_Get(pMe->mFTP.po, pMe->mFTP.remoteObjects[uIndex].szTypeOfObj, wBuf, wName) != SUCCESS)
								{
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif 

									MSG_ERROR("FTP file Get failed", 0, 0, 0);
									MSG_FATAL("***zzg FTP file Get failed***", 0, 0, 0);

									BTApp_ShowMessage(pMe, IDS_MSG_FILE_GET_FAILED, wName, 3);
								}
								else
								{
									//Copying the object name just to display once GET is over
									STRLCPY(szOperandex, pMe->mFTP.remoteObjects[uIndex].szFolderFileName, sizeof(szOperandex));
									pMe->mFTP.bObjectTransfer = TRUE;
									//ShowBusyIcon(pMe->a.m_pIShell, pMe->a.m_pIDisplay, &pMe->m_rect, FALSE);
								}
								break;
							}			

							if (IBTEXTFTP_SetPath(pMe->mFTP.po, wName, AEEBT_FTP_SET_PATH_TO_FOLDER) != SUCCESS)
							{
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif 
								MSG_ERROR("Set Path to folder failed", 0, 0, 0);
								BTApp_ShowMessage(pMe, IDS_MSG_FOLDER_BROWSING_FAILED, wName, 3);
							}
							else
							{
								// Storing folder name to display it later in case of failure
								STRLCPY(szOperandex, pMe->mFTP.remoteObjects[uIndex].szFolderFileName, sizeof(szOperandex));
								BTApp_ShowBusyIcon(pMe); // wait for connect confirm
							}
						}
					}
					//CLOSE_DIALOG(DLGRET_CANCELED)
					return TRUE;
				}					
				case IDS_GET:						
				{
					if (IFILEMGR_Test(pMe->mFTP.pIFileMgr, AEEFS_CARD0_DIR) != SUCCESS)		//Have  not T Card
					{
						BTApp_ShowMessage(pMe, IDS_INSERT_TCARD, wBuf, 3);
					}
					else
					{
						if (IMENUCTL_GetItemCount(pMenu) > 0)
						{
							MSG_FATAL("***zzg FtpBrowseOpitionDlgEvt IDS_BT_SELECT m_folder_index=%d***", pMe->m_folder_index, 0, 0);
							uIndex = pMe->m_folder_index;
							
							//uIndex = IMENUCTL_GetSel(pMe->m_pIMenu);
							
							// Get the name of the menu item selected on the screen
							//STRTOWSTR(pMe->mFTP.remoteObjects[uIndex].szFolderFileName, wName, sizeof(wName));

							//Add By zzg 2011_12_29	
						   	{						
								UTF8TOWSTR((byte*)pMe->mFTP.remoteObjects[uIndex].szFolderFileName, 
											AEEBT_MAX_FILE_NAME + 1, 
											wName, 	
											(AEEBT_MAX_FILE_NAME + 1)*sizeof(AECHAR));	

								DBGPRINTF("***zzg IMenuCtl_AddItem wName=%s***", wName);	
							}	
							//Add End
								

							//if (pMe->mFTP.u8StorageDevice == STORAGE_DEV_MEM_CARD)
							//Default device is STORAGE_DEV_MEM_CARD
							{
								STRLCPY(szOperandex, AEEFS_CARD0_DIR, sizeof(szOperandex));
							}

							if ((STRLEN(szOperandex) + 
								STRLEN(pMe->mFTP.remoteObjects[uIndex].szFolderFileName)) > 
								AEEBT_MAX_FILE_NAME) 
							{
								MSG_ERROR("File / Folder name exceeds max", 0, 0, 0);
								BTApp_ShowMessage(pMe, IDS_MSG_FILE_GET_FAILED, wName, 3);
								break;            
							}

							STRLCAT(szOperandex, pMe->mFTP.remoteObjects[uIndex].szFolderFileName, sizeof(szOperandex));
							
							//STRTOWSTR(szOperandex, wBuf, sizeof(wBuf));

							//Add By zzg 2011_12_29	
						   	{						
								UTF8TOWSTR((byte*)szOperandex, 
											AEEBT_MAX_FILE_NAME + 1, 
											wBuf, 	
											(AEEBT_MAX_FILE_NAME + 1)*sizeof(AECHAR));	

								DBGPRINTF("***zzg IMenuCtl_AddItem wBuf=%s***", wBuf);	
							}	
							//Add End
							
							DBGPRINTF("***zzg GET file/folder name is %s***", szOperandex);
							DBGPRINTF("***zzg GET szFolderFileName name is %s***", pMe->mFTP.remoteObjects[uIndex].szFolderFileName);						

							MSG_FATAL("***zzg FtpBrowseOpitionDlgEvt IDS_BT_SELECT uIndex=%d, szTypeOfObj=%d***", 
										uIndex, pMe->mFTP.remoteObjects[uIndex].szTypeOfObj, 0);

							if (pMe->mFTP.remoteObjects[uIndex].szTypeOfObj == AEEBT_FTP_FILE)
							{
								// Remove if the file already exists
								if (IFILEMGR_Test(pMe->mFTP.pIFileMgr, szOperandex) == SUCCESS)
								{
									if (IFILEMGR_Remove(pMe->mFTP.pIFileMgr, szOperandex) != SUCCESS)
									{
										MSG_MED("Could not remove the file", 0, 0, 0);
									}
								}
							}
							else
							{
								// Remove if the folder already exists
								if (IFILEMGR_Test(pMe->mFTP.pIFileMgr, szOperandex) == SUCCESS )
								{
									if (BTAppFTP_RemoveDir(pMe, szOperandex) != SUCCESS)
									{
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif 

										MSG_ERROR("Folder exists but couldn't be removed", 0, 0, 0);
										MSG_MED("FTP Folder Get failed", 0, 0, 0);

										MSG_FATAL("***zzg File / Folder name exceeds max***", 0, 0, 0);
										
										BTApp_ShowMessage(pMe, IDS_MSG_FILE_GET_FAILED, wName, 3);
										break;
									}
								}
							}

							if (IBTEXTFTP_Get(pMe->mFTP.po, pMe->mFTP.remoteObjects[uIndex].szTypeOfObj, wBuf, wName) != SUCCESS)
							{
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif 

								MSG_ERROR("FTP File / Folder Get failed", 0, 0, 0);

								MSG_FATAL("***zzg FTP File / Folder Get failed***", 0, 0, 0);

								BTApp_ShowMessage(pMe, IDS_MSG_FILE_GET_FAILED, wName, 3);
							}
							else
							{ 
								// Copying th	e object name just to display once GET is over 
								STRLCPY(szOperandex, pMe->mFTP.remoteObjects[uIndex].szFolderFileName, sizeof(szOperandex));
								pMe->mFTP.bObjectTransfer = TRUE;
								//ShowBusyIcon(pMe->a.m_pIShell, pMe->a.m_pIDisplay, &pMe->m_rect, FALSE);
							}
						}
					}

					//CLOSE_DIALOG(DLGRET_CANCELED)
					return TRUE;		
				}
				case IDS_DELETE:						
				{					
					if (IMENUCTL_GetItemCount(pMenu) > 0)
					{
						MSG_FATAL("***zzg FtpBrowseOpitionDlgEvt IDS_BT_SELECT m_folder_index=%d***", pMe->m_folder_index, 0, 0);
						uIndex = pMe->m_folder_index;
						//uIndex = IMENUCTL_GetSel(pMenu);
						
						// Get the name of the menu item selected on the screen
						//STRTOWSTR(pMe->mFTP.remoteObjects[uIndex].szFolderFileName, wName, sizeof(wName));
						
						//Add By zzg 2011_12_29	
					   	{						
							UTF8TOWSTR((byte*)pMe->mFTP.remoteObjects[uIndex].szFolderFileName, 
										AEEBT_MAX_FILE_NAME + 1, 
										wName, 	
										(AEEBT_MAX_FILE_NAME + 1)*sizeof(AECHAR));	

							DBGPRINTF("***zzg IMenuCtl_AddItem wName=%s***", wName);	
						}	
						//Add End

						// Just storing folder name to display it later 
						STRLCPY(szOperandex, pMe->mFTP.remoteObjects[uIndex].szFolderFileName, sizeof(szOperandex));

						if (IBTEXTFTP_Delete(pMe->mFTP.po, wName) != SUCCESS)
						{
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif 

							MSG_ERROR("FTP_Delete() failed", 0, 0, 0);
							BTApp_ShowMessage(pMe, IDS_MSG_FILE_DELETE_FAILED, wName, 3);
						}
						else
						{
							BTApp_ShowBusyIcon(pMe); // wait for result
						}
					}

					//CLOSE_DIALOG(DLGRET_CANCELED)
					return TRUE;		
				}
				case IDS_FTP_CREATE_FOLDER:						
				{
					//Add By zzg 2011_02_23
					pMe->m_edit_id = IDS_FTP_CREATE_FOLDER;
					pMe->m_bEditNeedStr = FALSE;		

					pMe->m_edit_state_id = BTAPPST_FTP_BROWSE;								

					pMe->m_eDlgRet = DLGRET_BT_EDIT; 
					(void) ISHELL_EndDialog(pMe->m_pShell);		
					//Add End
		
					//BTApp_BuildMenu( pMe, BT_APP_MENU_FTP_CREATE_FLDR );
					
					return TRUE;		
				}
				case IDS_FTP_MOVE_TO_FOLDER:						
				{
					STRLCPY(szOperandex, NEW_FOLDER_NAME, sizeof(szOperandex));

					//Add By zzg 2011_02_23
					pMe->m_edit_id = IDS_FTP_MOVE_TO_FOLDER;
					pMe->m_bEditNeedStr = FALSE;		

					pMe->m_edit_state_id = BTAPPST_FTP_BROWSE;								

					pMe->m_eDlgRet = DLGRET_BT_EDIT; 
					(void) ISHELL_EndDialog(pMe->m_pShell);		
					//Add End

					
          			//BTApp_BuildMenu(pMe, BT_APP_MENU_FTP_MOVE_TO_FLDR);
					return TRUE;		
				}
				case IDS_FTP_MOVE_TO_ROOT:						
				{
					if (IBTEXTFTP_SetPath(pMe->mFTP.po, NULL, AEEBT_FTP_SET_PATH_TO_ROOT) != SUCCESS)
					{
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif 
						MSG_ERROR("Set Path to ROOT failed", 0, 0, 0);
						CLOSE_DIALOG(DLGRET_CANCELED)	
					}
					else
					{
						//Store 'ROOT' as interop device root folder name is not known 
						STRLCPY(szOperandex, ROOT_FOLDER_STR, sizeof(szOperandex));
						BTApp_ShowBusyIcon(pMe); // wait for connect confirm
					}
					//CLOSE_DIALOG(DLGRET_CANCELED)
					return TRUE;		
				}
				case IDS_FTP_CLIENT:						
				{						
					MSG_FATAL("***zzg FtpBrowseOpitionDlgEvt IDS_FTP_CLIENT m_pActiveDlgID=%d***", pMe->m_pActiveDlgID, 0, 0);
					
					//Add By zzg 2011_02_23		
					if ((pMe->m_pActiveDlgID == IDD_BT_FTP_BROWSE)	|| (pMe->m_pActiveDlgID == IDD_BT_FTP_BROWSE_OPITION))
					{
						pMe->m_eDlgRet = DLGRET_FTP_CLIENT; 		
						(void) ISHELL_EndDialog(pMe->m_pShell);
					}		
					else
					{
						CLOSE_DIALOG(DLGRET_CANCELED)
					}
					//Add End	
					return TRUE;		
				}
				default:
				{
					ASSERT_NOT_REACHABLE;
				}
			}
			
			return TRUE;
		}
		default:
		{
			break;
		}
	}
	return FALSE;	 
}

static boolean  HandleMsgBoxDialogEvent(CBTApp * pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{	 
	PARAM_NOT_REF(dwParam)
		
	IStatic  *pStatic = (IStatic*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_BT_MSGBOX);	

	if (pStatic == NULL)
	{
		return FALSE;
	}	

	MSG_FATAL("***zzg HandleMsgBoxDialogEvent eCode=%x, wParam=%x, dwParam=%x***", eCode, wParam, dwParam);

	switch (eCode)
	{
		case EVT_DIALOG_INIT:			
		{
			//IDIALOG_SetProperties((IDialog *)dwParam, DLG_NOT_REDRAW_AFTER_START);
			return TRUE;
		}

		case EVT_DIALOG_START:
		{			
			AEERect 	rc;
			AECHAR		WTitle[40] = {0};		
			
			(void)ISHELL_LoadResString(pMe->m_pShell,
									   AEE_APPSBTAPP_RES_FILE, 							   
									   IDS_BT_TITLE,
									   WTitle,
									   sizeof(WTitle));
			
			IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, WTitle);  	

			// set rect for info display area
			SETAEERECT(&rc, pMe->m_rect.x, 
					   pMe->m_rect.y, 
					   pMe->m_rect.dx, 
					   pMe->m_rect.dy - BOTTOMBAR_HEIGHT); // leave room for SK menu
					 
			ISTATIC_SetRect(pStatic, &rc);

			ISTATIC_SetProperties(pStatic, ISTATIC_GetProperties(pStatic) & ~ST_MIDDLETEXT & ~ST_CENTERTEXT);	 
			ISTATIC_SetProperties(pStatic, ST_NOSCROLL|ST_GRAPHIC_BG);	
			ISTATIC_SetBackGround(pStatic, AEE_APPSCOMMONRES_IMAGESFILE, IDB_BACKGROUND);	
			
			(void) ISHELL_PostEvent( pMe->m_pShell,
									 AEECLSID_BLUETOOTH_APP,
									 EVT_USER_REDRAW,
									 0,
									 0);

			ISHELL_SetTimer(pMe->m_pShell, ONE_SECOND*3, (PFNNOTIFY)BTApp_CancelMsgBox, pMe);
			return TRUE;
		}
		
		case EVT_USER_REDRAW:		
		{	
			AECHAR  wstrText[MSGBOX_MAXTEXTLEN];			
			AECHAR* pText = pMe->pText2;			
					
            (void) ISHELL_LoadResString(pMe->m_pShell,
	                                    AEE_APPSBTAPP_RES_FILE,
	                                    pMe->m_msg_id,
	                                    wstrText,
	                                    sizeof(wstrText));
			

			MSG_FATAL("***zzg MsgBoxDialog m_msg_id=%d, m_bNeedStr=%d, wMsgBuf_len=%d***", pMe->m_msg_id, pMe->m_bNeedStr, WSTRLEN(pMe->wMsgBuf));
			
			if (pMe->m_msg_id == 0)
			{
				// use the wArg as msg
				pText = pMe->wMsgBuf;
				
			}
			else
			{
				// get the text
				ISHELL_LoadResString(pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, pMe->m_msg_id, pMe->pText2, LONG_TEXT_BUF_LEN*sizeof(AECHAR));

				if (pMe->wMsgBuf != NULL)
				{
					// build the message
					WSPRINTF( pMe->pText1, LONG_TEXT_BUF_LEN*sizeof(AECHAR), pMe->pText2, pMe->wMsgBuf);	
					pText = pMe->pText1;
					
				}
				else
				{
					//Add By zzg 2011_12_29	
				   	{
						char tempstr[256];

						//WSTRTOSTR(pMe->pText2, tempstr, 256);		
						WSTRTOUTF8(pMe->pText2, (256)*sizeof(AECHAR),(byte *)tempstr, 256);
						
						UTF8TOWSTR((byte*)tempstr, 256, pText, 	(256)*sizeof(AECHAR));	

						DBGPRINTF("***zzg IMenuCtl_AddItem tempstr=%s***", tempstr);
						DBGPRINTF("***zzg IMenuCtl_AddItem pText=%s***", pText);	
					}	
					//Add End
				}
			}
			

			if (pMe->m_bNeedStr)	//pMe->m_msg_id+pMe->wMsgBuf
			{			
				
				//Add By zzg 2011_12_29	
			   	{
					char tempstr[256];

					//WSTRTOSTR(pText, tempstr, 256);		
					WSTRTOUTF8(pText, (256)*sizeof(AECHAR),(byte *)tempstr, 256);
					
					UTF8TOWSTR((byte*)tempstr, 256, pText, 	(256)*sizeof(AECHAR));	

					DBGPRINTF("***zzg IMenuCtl_AddItem tempstr=%s***", tempstr);
					DBGPRINTF("***zzg IMenuCtl_AddItem pText=%s***", pText);	
				}	
				//Add End					
				
				ISTATIC_SetText(pStatic, NULL, pText, AEE_FONT_BOLD, AEE_FONT_NORMAL);					
			}
			else
			{		
				
				//Add By zzg 2011_12_29	
			   	{
					char tempstr[256];

					//WSTRTOSTR(wstrText, tempstr, 256);		
					WSTRTOUTF8(wstrText, (256)*sizeof(AECHAR),(byte *)tempstr, 256);
					
					UTF8TOWSTR((byte*)tempstr, 256, wstrText, 	(256)*sizeof(AECHAR));	

					DBGPRINTF("***zzg IMenuCtl_AddItem tempstr=%s***", tempstr);
					DBGPRINTF("***zzg IMenuCtl_AddItem wstrText=%s***", wstrText);	
				}	
				//Add End					
				
				ISTATIC_SetText(pStatic, NULL, wstrText, AEE_FONT_BOLD, AEE_FONT_NORMAL);		
			}		
			
						
			ISTATIC_Redraw(pStatic);	
			ISTATIC_SetActive(pStatic, TRUE);		

			BT_DRAW_BOTTOMBAR(BTBAR_OK_BACK)
				
			IDISPLAY_UpdateEx( pMe->m_pIDisplay, FALSE); 			
			return TRUE;
		}

		case EVT_DIALOG_END:
		{
            ISHELL_CancelTimer(pMe->m_pShell, (PFNNOTIFY)BTApp_CancelMsgBox, pMe);
			return TRUE;
		}

		case EVT_KEY_PRESS: 	  
		{	
			switch(wParam)
			{
				case AVK_INFO:					
				case AVK_SELECT:				
				case AVK_CLR:	
				{
					ISHELL_CancelTimer(pMe->m_pShell, (PFNNOTIFY)BTApp_CancelMsgBox, pMe);
					CLOSE_DIALOG(DLGRET_MSGBOX_CANCELED)	
					return TRUE;
				}
				default:
				{
					break;
				}
			}
			
			return TRUE;
		}
		
		default:
		{
			break;
		}
	}

	return FALSE;		 

}

// 对话框 IDD_BT_PROMPT事件处理函数
static boolean  HandleProMptDialogEvent(CBTApp *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
	PARAM_NOT_REF(dwParam)

    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_BT_PROMPT);
	IStatic  *pStatic = (IStatic*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_BT_PROMPT_STATIC);
	
    if ((pMenu == NULL) || (pStatic == NULL))
    {
        return FALSE;
    }
	
    MSG_FATAL("***zzg HandleProMptDialogEvent eCode=%x, wParam=%x ,dwParam=%x***",eCode,wParam,dwParam);   

    switch (eCode)
    {
        case EVT_DIALOG_INIT:			
		{			
			IDIALOG_SetProperties((IDialog *)dwParam, DLG_NOT_REDRAW_AFTER_START);			
            return TRUE;
		}

        case EVT_DIALOG_START:   
        {		
			AECHAR		WTitle[40] = {0};				
			(void)ISHELL_LoadResString(pMe->m_pShell,
									   AEE_APPSBTAPP_RES_FILE, 							   
									   IDS_BT_TITLE,
									   WTitle,
									   sizeof(WTitle));
			if (pMe->m_pIAnn != NULL) 
			{
				IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, WTitle);
			}
						
			IMENUCTL_SetSel(pMenu, pMe->m_currDlgId);
			IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);

			IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);

#ifdef FEATURE_CARRIER_CHINA_VERTU
			IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SETTING_BACKGROUND);
#endif
			IMENUCTL_SetBottomBarType(pMenu, BTBAR_OK_BACK);

            (void)ISHELL_PostEvent(pMe->m_pShell,
                                   AEECLSID_BLUETOOTH_APP,
                                   EVT_USER_REDRAW,
                                   0,
                                   0);   

			//Add By zzg 2011_05_25
			if ((IDS_MSG_BT_ENABLED == pMe->m_prompt_id) || (IDS_MSG_BT_DISABLED == pMe->m_prompt_id))
			{
				ISHELL_SetTimer(pMe->m_pShell, ONE_SECOND*3, (PFNNOTIFY)BTApp_CancelProMptBox, pMe);
			}
			//Add End			
     
	 		return TRUE;
	  }
		
	  case EVT_USER_REDRAW:    
	  {   
			PromptMsg_Param_type m_PromptMsg;
			AECHAR   wszMsg[64] = {0};
			AECHAR  wstrText[MSGBOX_MAXTEXTLEN];
			AECHAR* pText = pMe->pText2;			

			(void)ISHELL_LoadResString(pMe->m_pShell,
  									   AEE_APPSBTAPP_RES_FILE,
  									   pMe->m_prompt_id,
  									   wstrText,
  									   sizeof(wstrText));

			MEMSET(&m_PromptMsg, 0, sizeof(PromptMsg_Param_type));
		
			if (pMe->m_prompt_id == 0)
			{
				pText = pMe->wPromptBuf;		// use the wArg as msg
			}
			else if ((pMe->m_prompt_id == IDS_PROMPT_PROCEED_BONDING) && (WSTRLEN(pMe->mRM.wSSPPassKey) != 0))
			{
				// get the text
				ISHELL_LoadResString(pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, pMe->m_prompt_id, pMe->pText2, SHORT_TEXT_BUF_LEN*sizeof( AECHAR ) );

				if (pMe->wPromptBuf != NULL)
				{
					// build the message					
					WSPRINTF(pMe->pText1, LONG_TEXT_BUF_LEN*sizeof(AECHAR), pMe->pText2, pMe->wPromptBuf, pMe->mRM.wSSPPassKey);
					pText = pMe->pText1;
				}
			}
			else
			{
				// get the text
				ISHELL_LoadResString(pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, pMe->m_prompt_id, pMe->pText2, SHORT_TEXT_BUF_LEN*sizeof( AECHAR ) );

				if (pMe->wPromptBuf != NULL)
				{
					// build the message
					WSPRINTF( pMe->pText1, LONG_TEXT_BUF_LEN*sizeof(AECHAR), pMe->pText2, pMe->wPromptBuf);
					pText = pMe->pText1;
				}
			}

			if (pMe->m_bPromptNeedStr)	//pMe->m_prompt_id+pMe->wPromptBuf
			{			
				/*
				//Add By zzg 2011_12_29	
			   	{
					char tempstr[256];

					//WSTRTOSTR(pText, tempstr, 256);	
					WSTRTOUTF8(pText, 
                               (256)*sizeof(AECHAR),
                               (byte *)tempstr, 
                               256);
					
					UTF8TOWSTR((byte*)tempstr, 256, pText, 	(256)*sizeof(AECHAR));	

					DBGPRINTF("***zzg IMenuCtl_AddItem tempstr=%s***", tempstr);
					DBGPRINTF("***zzg IMenuCtl_AddItem pText=%s***", pText);	
				}	
				//Add End
				*/
				
				 m_PromptMsg.pwszMsg = pText;	
			}
			else
			{		
				/*
				//Add By zzg 2011_12_29	
			   	{
					char tempstr[256];

					//WSTRTOSTR(wstrText, tempstr, 256);		
					WSTRTOUTF8(wstrText, 
                               (256)*sizeof(AECHAR),
                               (byte *)tempstr, 
                               256);
					
					UTF8TOWSTR((byte*)tempstr, 256, wstrText, 	(256)*sizeof(AECHAR));	

					DBGPRINTF("***zzg IMenuCtl_AddItem tempstr=%s***", tempstr);
					DBGPRINTF("***zzg IMenuCtl_AddItem wstrText=%s***", wstrText);	
				}	
				//Add End
				*/
				
				 m_PromptMsg.pwszMsg = wstrText;	
			}
		
			m_PromptMsg.ePMsgType = MESSAGE_CONFIRM;		
			m_PromptMsg.eBBarType = BTBAR_OK_BACK;

			//Add By zzg 2011_05_25
			if ((IDS_MSG_BT_ENABLED == pMe->m_prompt_id) 
				|| (IDS_MSG_BT_DISABLED == pMe->m_prompt_id))
			{
				m_PromptMsg.ePMsgType = MESSAGE_INFORMATIVE;	
				m_PromptMsg.eBBarType = BTBAR_BACK;
			}
			//Add End		

			MSG_FATAL("***zzg BTAppDlg DrawPromptMessage***", 0, 0, 0);
			
			DrawPromptMessage(pMe->m_pIDisplay, pStatic, &m_PromptMsg);
			IDISPLAY_UpdateEx(pMe->m_pIDisplay,FALSE); 
			return TRUE;

		}
	  
        case EVT_DIALOG_END:
        {
			MSG_FATAL("***zzg BTAppDlg DrawPromptMessage EVT_DIALOG_END***", 0, 0, 0);
            ISHELL_CancelTimer(pMe->m_pShell, (PFNNOTIFY)BTApp_CancelProMptBox, pMe);
			//CancelReDrawPromptMessage(pMe->m_pShell);	//取消Prompt和AEE_STATIC的Auto_scroll的同步
			return TRUE;
        }

		case EVT_KEY_PRESS:
        //case EVT_KEY:
        {			
			//Add By zzg 2011_05_25
			if ((IDS_MSG_BT_ENABLED == pMe->m_prompt_id) 
				|| (IDS_MSG_BT_DISABLED == pMe->m_prompt_id))
			{
				ISHELL_CancelTimer(pMe->m_pShell, (PFNNOTIFY)BTApp_CancelProMptBox, pMe);							
				CLOSE_DIALOG(DLGRET_PROMPT_CANCELED)
				return TRUE;	
			}
			//Add End	
			
			switch (wParam)
            {
                case AVK_CLR:
                {
					CLOSE_DIALOG(DLGRET_PROMPT_CANCELED)
                    return TRUE;
                }
					
				case AVK_INFO:
				case AVK_SELECT:
				{
					CLOSE_DIALOG(DLGRET_PROMPT_CANCELED)	//防止提前进入MSGBOX的消息响应
						
					switch (pMe->m_prompt_id)
					{
						case IDS_PROMPT_REMOVE_ONE:
						{
							BTApp_DoRemoveOne(pMe);
							break;
						}

						case IDS_PROMPT_REBOND:
						{
							BTApp_DoRebond(pMe, TRUE);
							break;
						}
						
						case IDS_PROMPT_PROCEED_BONDING:
						{
							BTApp_UserConfirm(pMe, TRUE);
							break;
						}

						case IDS_PROMPT_UNBOND_ONE:
						{
							BTApp_DoUnbondOne(pMe);
							break;
						}
						
						case IDS_PROMPT_UNBOND_ALL:
						{
							BTApp_DoUnbondAll(pMe);  
							break;       
          				}
						
						case IDS_PROMPT_REMOVE_ALL:
						{
							BTApp_DoRemoveAll(pMe);

							/*
							//Add by zzg 2011_11_08		
							if (BDADDR_VALID(&pMe->mAG.bdAddr) || BDADDR_VALID(&pMe->mA2DP.bdAddr))
							{
								if (BDADDR_VALID(&pMe->mAG.bdAddr))
								{
									pMe->mAG.bdAddr = NULL_BD_ADDR;
								}

								if (BDADDR_VALID(&pMe->mA2DP.bdAddr))
								{
									pMe->mA2DP.bdAddr = NULL_BD_ADDR;
								}
								
								//BTApp_WriteBtAddrConfigFile(pMe);
								BTApp_WriteConfigFile(pMe);
							}
							//Add End		
							*/
							
							break;   
						}

						case IDS_PROMPT_USE_AUDIO_DEV:
						{
							boolean bSetBondable = FALSE;
							AEEBTDeviceInfo* pDev = &pMe->mRM.device[pMe->mRM.uCurDevIdx];

//#ifdef BTAPP_HEADSET_USE_AG	
							//AG Start
							pMe->mAG.bdAddr = pDev->bdAddr;
							//BTApp_WriteBtAddrConfigFile( pMe );	
							//BTApp_WriteConfigFile(pMe);
							
							if ( pMe->mAG.bEnabled != FALSE )
							{
								BTApp_ShowDevMsg(pMe, IDS_MSG_MUST_REENABLE_AG, 
								              		&pMe->mAG.bdAddr, 0 );
							}
							//AG End	
//#endif

#ifdef BTAPP_HEADSET_USE_A2DP
							/*
							//A2DP Start
							pMe->mA2DP.bdAddr = pDev->bdAddr;	
							BTApp_WriteBtAddrConfigFile( pMe );
							
							if (pMe->mA2DP.bEnabled != FALSE )
							{
								BTApp_EnableA2DP(pMe, &bSetBondable);
								//BTApp_ShowDevMsg(pMe, IDS_MSG_MUST_REENABLE_AG, &pMe->mA2DP.bdAddr, 0 );
							}
							//A2DP End
							*/
#endif							
							//Continue to Connect
							//break;
						}			

						case IDS_PROMPT_CONNECT:
						{
							AEEBTDeviceInfo* pDev = &pMe->mRM.device[pMe->mRM.uCurDevIdx];
							boolean bSetBondable = FALSE;
							
//#ifdef BTAPP_HEADSET_USE_AG	
							//AG Start
							pMe->mAG.bdAddr = pDev->bdAddr;
							//BTApp_WriteBtAddrConfigFile(pMe);
							//BTApp_WriteConfigFile(pMe);

							if ((pMe->mAG.bEnabled == TRUE) && (pMe->mA2DP.bConnected== FALSE))							
							{
								if ( pMe->mAG.bconnInPrgs == FALSE )
								{
									if ( pMe->mAG.devType == AEEBT_AG_AUDIO_DEVICE_HEADSET )
									{
										MSG_MED( "HndlSlction - connecting to HS", 0, 0, 0 );
									}
									else
									{
										MSG_MED( "HndlSlction - connecting to HF", 0, 0, 0 );
									}
									
									if ( BTApp_CallConnected( pMe ) != BT_APP_CALL_NONE )
									{
										pMe->mAG.bDevPickedUp = TRUE; // signal self to send audio to HS/HF
									}
									pMe->mAG.bconnInPrgs = TRUE;
									IBTEXTAG_Connect( pMe->mAG.po, &pDev->bdAddr, pMe->mAG.devType );
									BTApp_ShowBusyIcon( pMe );
								}
								else
								{
									BTApp_ShowMessage( pMe, IDS_AG_CONNECTION_IN_PRGS, NULL, 3 );
								}
							}
							//AG End
//#endif							

#ifdef BTAPP_HEADSET_USE_A2DP			
							/*
							MSG_FATAL("***zzg AG bEnabled=%x, bConnected=%x***", pMe->mAG.bEnabled, pMe->mAG.bConnected, 0);
							MSG_FATAL("***zzg A2DP bEnabled=%x, bEnableA2DP=%x, bConnected=%x***", 
										pMe->mA2DP.bEnabled, pMe->mA2DP.bEnableA2DP, pMe->mA2DP.bConnected);
							
							pMe->mA2DP.bdAddr = pMe->mRM.device[pMe->mRM.uCurDevIdx].bdAddr;

			               	MSG_FATAL("***zzg Prompt mA2DP:%04x %04x %04x***", 
										((uint16)(pMe->mA2DP.bdAddr.uAddr[ 5 ] << 8) | pMe->mA2DP.bdAddr.uAddr[ 4 ]),
							       		((uint16)(pMe->mA2DP.bdAddr.uAddr[ 3 ] << 8) | pMe->mA2DP.bdAddr.uAddr[ 2 ]),
							       		((uint16)(pMe->mA2DP.bdAddr.uAddr[ 1 ] << 8) | pMe->mA2DP.bdAddr.uAddr[ 0 ]));	
							
							pMe->bConfigChanged = TRUE;
						    IBTEXTA2DP_SetDevice(pMe->mA2DP.po, &pMe->mA2DP.bdAddr);	

							//Add By zzg 2011_11_03
							if (pMe->bConfigChanged)
							{								
								BTApp_WriteBtAddrConfigFile(pMe);
							}							
							//Add End
							
							if ((pMe->mA2DP.bEnabled == TRUE) && (pMe->mA2DP.bConnected == FALSE))
							{
								if ( IBTEXTA2DP_Connect( pMe->mA2DP.po, &pMe->mA2DP.bdAddr ) != SUCCESS )
								{
									BTApp_ShowMessage( pMe, IDS_MSG_CONN_FAILED, NULL, 3 );
								}  	
								else
								{
									BTApp_A2DPSetRetries(pMe, TRUE);
								}
							}		
							*/
#endif
          
							break;
						}

						case IDS_PROMPT_DISCONNECT:
						{		
							MSG_FATAL("***zzg IDS_PROMPT_DISCONNECT***", 0, 0, 0);	

//#ifdef BTAPP_HEADSET_USE_A2DP	
							if ((pMe->mA2DP.bEnabled == TRUE) && (pMe->mA2DP.bConnected == TRUE))
							{
								if (IBTEXTA2DP_Disconnect(pMe->mA2DP.po) != SUCCESS)
								{
									MSG_FATAL("***zzg IBTEXTA2DP_Disconnect Failed***", 0, 0, 0);
									BTApp_ShowMessage(pMe, IDS_A2DP_DISCONNECT_FAILED, NULL, 3);
								}
								else
								{
									BTApp_A2DPSetRetries(pMe, FALSE);
								}		
							}
//#endif							

//#ifdef BTAPP_HEADSET_USE_AG	
							if ((pMe->mAG.bEnabled == TRUE) && (pMe->mAG.bConnected == TRUE))							
							{
								IBTEXTAG_Disconnect(pMe->mAG.po); 
							}
//#endif						
							/*
							//Add by zzg 2011_11_08
							if (BDADDR_VALID(&pMe->mAG.bdAddr) || BDADDR_VALID(&pMe->mA2DP.bdAddr))
							{
								if (BDADDR_VALID(&pMe->mAG.bdAddr))
								{
									pMe->mAG.bdAddr = NULL_BD_ADDR;
								}

								if (BDADDR_VALID(&pMe->mA2DP.bdAddr))
								{
									pMe->mA2DP.bdAddr = NULL_BD_ADDR;
								}
								
								//BTApp_WriteBtAddrConfigFile(pMe);
								BTApp_WriteConfigFile(pMe);
							}							
							//Add End
							*/

         
							break;
						}
       
						default:
						{
							break;
						}
					}				
					
					return TRUE;
				}
                default:
                {
					break;
                }
            }
            return TRUE;
        }
        default:
        {
			break;
        }
    }
    return FALSE;
}

static boolean HandleBtTextEditDialogEvent(CBTApp *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{	 
	PARAM_NOT_REF(dwParam)

#ifdef FEATURE_BT_2_1
  	AEEBTDeviceInfo* pDev = &pMe->mRM.device[pMe->mRM.uCurDevIdx];
  	AEEBTKeypressType keyPressType;
  	//MSG_LOW( "TextEditHndlEv - pMe->mRM.ioCaptype = %d", pMe->mRM.ioCaptype, 0, 0 );
#endif 

	ITextCtl *pIText = (ITextCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_BT_EDIT);	

	ITEXTCTL_GetText(pIText, pMe->pText2, SHORT_TEXT_BUF_LEN );

    if (NULL == pIText)
    {
        return FALSE;
    }
	
    MSG_FATAL("***zzg HandleBtTextEditDialogEvent: eCode=%x, wParam=%x, dwParam=%x***", eCode,wParam,dwParam);   

    switch (eCode)
    {
        case EVT_DIALOG_INIT:			
		{	
			IDIALOG_SetProperties((IDialog *)dwParam, DLG_NOT_REDRAW_AFTER_START);
            return TRUE;
		}

        case EVT_DIALOG_START:   
		{
			AEERect		rc;		
			AEETextInputMode IM;
			uint16    maxLen;
			AECHAR*   pText;
			uint32    prop;

			IDISPLAY_Backlight(pMe->m_pIDisplay, TRUE);
			
			SETAEERECT ( &rc, pMe->m_rect.x, pMe->m_rect.y, pMe->m_rect.dx, pMe->m_rect.dy - BOTTOMBAR_HEIGHT);
			ITEXTCTL_SetRect(pIText, &rc );

			switch (pMe->m_edit_id)
			{
				case IDS_PASS_KEY:
				case IDS_FTP_CLIENT:	
				{
					IM = AEE_TM_NUMBERS;
					maxLen = MAX_PASSKEY_LEN;
					pText     = pMe->mRM.wPassKey;
      				pText[0] = NULL; // don't remember previous pass key					
      				prop = TP_FRAME | TP_PASSWORD | TP_BT_PROPERTY;
					break;
				}
				case IDS_FTP_MOVE_TO_FOLDER:
				case IDS_FTP_CREATE_FOLDER:
				{
					prop	= TP_FRAME | TP_STARKEY_SWITCH | TP_BT_PROPERTY;
					IM		= AEE_TM_LETTERS;					
					maxLen	= AEEBT_MAX_FILE_NAME;
					pText	= pMe->mFTP.wFolderName;
					break;
				}
				default:
				{
					IM = AEE_TM_LETTERS;
					maxLen = MAX_PASSKEY_LEN;
					break;
				}
			}
			
			ITEXTCTL_SetProperties(pIText, prop);
			//ITEXTCTL_SetProperties(pIText, TP_FRAME | TP_MULTILINE | TP_NOUPDATE |TP_FOCUS_NOSEL); //TP_STARKEY_SWITCH

			ITEXTCTL_SetMaxSize(pIText, maxLen);	

			/*
			//Add By zzg 2011_12_29	
		   	{
				char tempstr[256];

				WSTRTOSTR(pText, tempstr, 256);		
				
				UTF8TOWSTR((byte*)tempstr, 256, pText, 	(256)*sizeof(AECHAR));	

				DBGPRINTF("***zzg IMenuCtl_AddItem tempstr=%s***", tempstr);
				DBGPRINTF("***zzg IMenuCtl_AddItem pText=%s***", pText);	
			}	
			//Add End	
			*/
				
			ITEXTCTL_SetText(pIText, pText, WSTRLEN(pText));			
			ITEXTCTL_SetInputMode(pIText, IM);			

            (void)ISHELL_PostEvent(pMe->m_pShell,
                                   AEECLSID_BLUETOOTH_APP,
                                   EVT_USER_REDRAW,
                                   0,
                                   0);
            return TRUE;
        }

        case EVT_USER_REDRAW:    
		{			
			AECHAR  wstrText[MSGBOX_MAXTEXTLEN];
			AECHAR* pText = pMe->pText2;
			
			AECHAR		WTitle[40] = {0};				   

			if (pMe->m_edit_id == 0)
			{
				pText = pMe->wEditBuf;
			}
			else
			{
				// get the text
				ISHELL_LoadResString(pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, pMe->m_edit_id, pMe->pText2, SHORT_TEXT_BUF_LEN*sizeof(AECHAR));

				if (pMe->wEditBuf != NULL)
				{
					// build the message
					WSPRINTF(pMe->pText1, LONG_TEXT_BUF_LEN*sizeof(AECHAR), pMe->pText2, pMe->wEditBuf);
					pText = pMe->pText1;
				}
			}

			if (pMe->m_bEditNeedStr)	//pMe->m_msg_id+pMe->wMsgBuf
			{		
				IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, pText);	
			}
			else if (pMe->m_edit_id == 0)
			{
				(void) ISHELL_LoadResString(pMe->m_pShell,
		                                    AEE_APPSBTAPP_RES_FILE,
		                                    IDS_PASSWORD,		                                    
		                                    WTitle,
		                                    sizeof(wstrText));
				
				IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, WTitle);
			}
			else
			{
				(void) ISHELL_LoadResString(pMe->m_pShell,
		                                    AEE_APPSBTAPP_RES_FILE,
		                                    pMe->m_edit_id,		                                    
		                                    WTitle,
		                                    sizeof(wstrText));
				
				IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, WTitle);					
			}
						
			ITEXTCTL_SetCursorPos(pIText, TC_CURSOREND);    
			ITEXTCTL_SetActive(pIText, TRUE);
			ITEXTCTL_Redraw(pIText);	

			BT_DRAW_BOTTOMBAR(BTBAR_OK_BACK)
			
			IDISPLAY_UpdateEx(pMe->m_pIDisplay, FALSE); 
            return TRUE;
        }
		
        case EVT_DIALOG_END:
        {
			return TRUE;
        }

        case EVT_KEY:
        {
			switch (wParam)
            {
                case AVK_CLR:
				{
					switch (pMe->m_edit_id)
					{
						case IDS_PASS_KEY:
						{
							if (pMe->mRM.bBonding == FALSE)
							{
								pMe->mRM.wPassKey[0] = NULL;
#ifdef FEATURE_BT_2_1
								if ((pMe->mRM.ioCaptype == AEEBT_RM_IOC_KEYBOARD_ONLY) && (pMe->mRM.bPassKey == TRUE))
								{
									MSG_MED("TextEditHandleEvent - RemoteDev BDa=%2x%2x%2x",
											pDev->bdAddr.uAddr[0], 
											pDev->bdAddr.uAddr[1], 
											pDev->bdAddr.uAddr[2]);
									
									pMe->mRM.bPassKey = FALSE;
									IBTEXTRM_PasskeyReply(pMe->mRM.po, &pDev->bdAddr, "");
								}
								else
#endif 
								{
									IBTEXTRM_PinReply(pMe->mRM.po, &pMe->mRM.device[pMe->mRM.uCurDevIdx].bdAddr, pMe->mRM.wPassKey );
								}
							}
							
							pMe->mRM.bBonding = FALSE;					
							break;
						}

						case IDS_FTP_MOVE_TO_FOLDER:
						case IDS_FTP_CREATE_FOLDER:
						case IDS_FTP_CLIENT:
						{
							if ( ISHELL_EndDialog(pMe->m_pShell) == EFAILED )
					        {
					          MSG_ERROR( "TextEditHndlEv - ISHELL_EndDialog() failed", 0, 0, 0 );
					        }
							break;
						}
						
						default:
						{
							break;
						}
					}
					
					CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;
                }	
				case AVK_SELECT:
				case AVK_INFO:
				{
					switch(pMe->m_edit_id)
					{
						case IDS_PASS_KEY:
						{
							char passKey[AEEBT_SSP_PASSKEY_LEN];
							
							AEEBTDeviceInfo* pDev = &pMe->mRM.device[pMe->mRM.uCurDevIdx];
							WSTRLCPY(pMe->mRM.wPassKey, pMe->pText2, ARR_SIZE(pMe->mRM.wPassKey));
#ifdef FEATURE_BT_2_1
							DBGPRINTF_FATAL("TextEditHndlSave-BT_APP_MENU_PASSKEY with pMe->pText2=%s",pMe->pText2); 
							MSG_MED( "TextEditHndlSave, pMe->mRM.bBonding=%d ", pMe->mRM.bBonding,0,0); 
							MSG_MED( "TextEditHndlSave, pMe->mRM.ioCaptype=%d ", pMe->mRM.ioCaptype,0,0); 
#endif 
							if (pMe->mRM.bBonding)
							{
#ifdef FEATURE_BT_2_1
								if((pMe->mRM.ioCaptype == AEEBT_RM_IOC_KEYBOARD_ONLY) && (pMe->mRM.bPassKey == TRUE))
								{
									if (IBTEXTRM_KeypressNotification(pMe->mRM.po, &pDev->bdAddr, AEEBT_RM_KPN_COMPLETED) != SUCCESS)
									{
										MSG_HIGH( "KeyPress Notification- Failed", 0, 0, 0 );
										break;
									}
									
									WSTRTOSTR (pMe->mRM.wPassKey, passKey, sizeof(char)*AEEBT_SSP_PASSKEY_LEN );

									if (STRLEN(passKey) < (AEEBT_SSP_PASSKEY_LEN - 1)) 
									{
										BTApp_PadString(passKey, sizeof(passKey));
									}

									DBGPRINTF_FATAL("TextEditHndlSave-BT_APP_MENU_PASSKEY with passKey=%s",passKey); 
																		
									if (IBTEXTRM_PasskeyReply( pMe->mRM.po, &pDev->bdAddr, passKey ) != SUCCESS )
									{
										MSG_HIGH( "PassKey Reply- Failed", 0, 0, 0 );
										break;
									}
									
									BTApp_ShowBusyIcon( pMe );
								}
								else
#endif 
								{									
									if (IBTEXTRM_Bond(pMe->mRM.po, &pDev->bdAddr, pMe->mRM.wPassKey) != SUCCESS)
									{
										MSG_FATAL("***zzg HandleBTTextEditDlg IBTEXTRM_Bond Failed!***", 0, 0, 0);
										MSG_ERROR( "TextEditSave - Bond failed", 0, 0, 0 );
									}
									else
									{
										//Modify by zzg 2011_03_04
										//Showbusyicon............
										ShowBusyIcon(pMe->m_pShell, 
										             pMe->m_pIDisplay, 
										             &pMe->m_rect, 
										             FALSE);
										
										pMe->bBusyIconUp = TRUE;
										//Add End
										
										MSG_FATAL("***zzg HandleBTTextEditDlg IBTEXTRM_Bond SUCCEED!***", 0, 0, 0);
										BTApp_ShowBusyIcon( pMe );
									}
								}
							}
							else
							{
#ifdef FEATURE_BT_2_1
								if ((pMe->mRM.ioCaptype == AEEBT_RM_IOC_KEYBOARD_ONLY) 
									 && (pMe->mRM.bPassKey == TRUE))
								{
									MSG_HIGH( "Sending IBTEXTRM_KeypressNotification", 0, 0, 0 );

									if (IBTEXTRM_KeypressNotification(pMe->mRM.po, &pDev->bdAddr, AEEBT_RM_KPN_COMPLETED) != SUCCESS)
									{
										MSG_HIGH( "KeyPress Notification- Failed", 0, 0, 0 );
										break;
									}
									
									DBGPRINTF_FATAL ("Sending IBTEXTRM_PasskeyReply withpMe->mRM.wPassKey=%s", pMe->mRM.wPassKey); 
									WSTRTOSTR (pMe->mRM.wPassKey, passKey, sizeof(char)*AEEBT_SSP_PASSKEY_LEN);
									DBGPRINTF_FATAL ( "Sending IBTEXTRM_PasskeyReply with Passkey=%s", passKey); //was DBGPRINTF
									MSG_LOW ("BTApp_TextEditSave IBTEXTRM_PasskeyReply with BDAddr - %2x%2x%2x", pDev->bdAddr.uAddr[0], pDev->bdAddr.uAddr[1], pDev->bdAddr.uAddr[2]);

									if (STRLEN(passKey) < (AEEBT_SSP_PASSKEY_LEN - 1)) 
									{
										BTApp_PadString(passKey, sizeof(passKey));
									}

									if (IBTEXTRM_PasskeyReply( pMe->mRM.po, &pDev->bdAddr, passKey) != SUCCESS)
									{
										MSG_HIGH( "PassKey Reply- Failed", 0, 0, 0 );
										break;
									}
									
									BTApp_ShowBusyIcon( pMe );
								}
								else
#endif 
								{
									if (IBTEXTRM_PinReply( pMe->mRM.po, &pDev->bdAddr, pMe->mRM.wPassKey) != SUCCESS)
									{
										MSG_ERROR( "TextEditSave - PinReply failed", 0, 0, 0 );
									}
									else if (WSTRLEN(pMe->mRM.wPassKey) > 0)
									{
										pMe->mRM.bBonding = TRUE ;
										BTApp_ShowBusyIcon( pMe );
									}
								}
							}
							
							break;
						}

						case IDS_FTP_CLIENT:
						{
							WSTRLCPY(pMe->mFTP.wPassWord, pMe->pText2, ARR_SIZE(pMe->mFTP.wPassWord));
							
							if (IBTEXTFTP_Authenticate(pMe->mFTP.po, &pMe->mFTP.remoteBDAddr,NULL, pMe->mFTP.wPassWord) != SUCCESS)
							{
								MSG_ERROR( "TextEditSave - FTP_Authenticate failed", 0, 0, 0 );
							}
							else
							{
								BTApp_ShowBusyIcon( pMe );
							}
							break;
						}

						case IDS_FTP_CREATE_FOLDER:
						{
							WSTRLCPY(pMe->mFTP.wFolderName, pMe->pText2, ARR_SIZE(pMe->mFTP.wFolderName));
							
							if (IBTEXTFTP_CreateFolder(pMe->mFTP.po, pMe->mFTP.wFolderName) != SUCCESS)
							{
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif 
								MSG_ERROR( "FTP Create Folder failed", 0, 0, 0 );
								BTApp_ShowMessage( pMe, IDS_FTP_MSG_FOLDER_CREATION_FAILED, pMe->mFTP.wFolderName, 3);
							}
							else
							{
								BTApp_ShowBusyIcon( pMe ); /* Wait for create folder response */
							}
							break;
						}

						case IDS_FTP_MOVE_TO_FOLDER:
						{
							WSTRLCPY(pMe->mFTP.wFolderName, pMe->pText2, ARR_SIZE(pMe->mFTP.wFolderName));
							
							if (IBTEXTFTP_SetPath(pMe->mFTP.po, pMe->mFTP.wFolderName, AEEBT_FTP_SET_PATH_TO_FOLDER) != SUCCESS)
							{
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif 
								MSG_ERROR("Set Path to folder failed", 0, 0, 0);
								BTApp_ShowMessage(pMe, IDS_MSG_FOLDER_BROWSING_FAILED, pMe->mFTP.wFolderName, 3);
							}
							else
							{
								BTApp_ShowBusyIcon(pMe); 
							}
							
							break;
						}
						
						default:
						{
							break;
						}
					}
					
					CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;
				}
				
                default:
                {
					break;
                }
            }
			
			return TRUE;
        }
        		
        default:
        {
			break;
        }
    }
	
    return FALSE;
}	


static boolean HandleSendFileDialogEvent(CBTApp *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{	 
    PARAM_NOT_REF(dwParam)

    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_BT_SEND_FILE);

	boolean 	 bRegistered = FALSE; 	  

	MSG_FATAL("***zzg HandleSendFileDialogEvent bRegistered=%d***", pMe->mOPP.bRegistered, 0, 0);	
	
    if (pMenu == NULL)
    {
        return FALSE;
    }
	
    MSG_FATAL("***zzg HandleSendFileDialogEvent eCode=%x, wParam=%x, dwParam=%x***",eCode,wParam,dwParam);   

    switch (eCode)
    {
        case EVT_DIALOG_INIT:			
		{			
			AECHAR 		WTitle[40] = {0};			

			(void)ISHELL_LoadResString(pMe->m_pShell,
				                       AEE_APPSBTAPP_RES_FILE,                                
				                       IDS_OPP_CLIENT,
				                       WTitle,
				                       sizeof(WTitle));
			
			IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, WTitle);   

			MSG_FATAL("***zzg HandleSendFileDialogEvent mOPP.bConnected=%d***", pMe->mOPP.bConnected, 0, 0);	

			// Add individual entries to the Menu
			if ( pMe->mOPP.bConnected == FALSE )
			{
				IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, IDS_CONNECT, IDS_CONNECT, NULL, 0);				
			}
			else
			{
				IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, IDS_PUSH, IDS_PUSH, NULL, 0);	
				IMENUCTL_AddItem(pMenu, AEE_APPSBTAPP_RES_FILE, IDS_DISCONNECT, IDS_DISCONNECT, NULL, 0);  
				
			}
  			
            return TRUE;
		}

        case EVT_DIALOG_START:
        {
			// 给菜单各菜单项加数字编号图标
            //BTApp_SetItemNumIcon(pMenu);
            IMENUCTL_SetSel(pMenu, pMe->m_currDlgId);

            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);
            IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);

#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SETTING_BACKGROUND);
#endif
            IMENUCTL_SetBottomBarType(pMenu, BTBAR_SELECT_BACK);

            (void) ISHELL_PostEvent(pMe->m_pShell,
                                    AEECLSID_BLUETOOTH_APP,
                                    EVT_USER_REDRAW,
                                    0,
                                    0);
            return TRUE;
        }

        case EVT_USER_REDRAW:    			
        {
			//(void)IMENUCTL_Redraw(pMenu);     //dele by yangdecai
            return TRUE;
        }

        case EVT_DIALOG_END:
        {
			return TRUE;
        }

        case EVT_KEY:
        {
			switch (wParam)
            {
                case AVK_CLR:
                {
					CLOSE_DIALOG(DLGRET_CANCELED)
                    if(pMe->mOPP.bPushFileReq)
                    {
                        boolean      bSettingBondable = FALSE;
	                    boolean      bSettingDiscoverable = FALSE;
                        BTApp_EnableOPP(pMe, &bSettingBondable, &bSettingDiscoverable);
                    }
                    return TRUE;
                }

                default:
                {
					break;
                }
            }
            return TRUE;
        }

        case EVT_COMMAND:           
		{
			pMe->m_currDlgId = wParam;		
			
            switch (wParam)
            {
            	case IDS_CONNECT:	
				{
					AEEBTDeviceEnumerator enumerator;
					uint8				  i;						
					AEEBTDeviceInfo*	  pDev;						

					enumerator.control = AEEBT_RM_EC_MATCH_SERVICE_CLASS;
					enumerator.svcCls  = AEEBT_COD_SC_OBJECT_TRANSFER;

					pMe->m_obex_list_id = IDD_BT_SEND_FILE;
						
		
					if (pMe->mRM.po == NULL)
					{
						MSG_FATAL("***zzg ObexListServers pMe->mRM.po == NULL***", 0, 0, 0);
					}
					else
					{				
						if (IBTEXTRM_DeviceEnumInit(pMe->mRM.po, &enumerator) == SUCCESS)
						{		
							i	 = 0;
							pDev = &pMe->mRM.device[i];
		
							while ((IBTEXTRM_DeviceEnumNext( pMe->mRM.po, pDev ) == SUCCESS) 
									&& pDev->bValid && (i < MAX_DEVICES))
							{
								pDev = &pMe->mRM.device[++i];
							}
						}
					}

					if (i > 0)
					{
						CLOSE_DIALOG(DLGRET_BT_OBEX_LIST_SERVERS)
					}
					else
					{
						BTApp_ShowMessage(pMe, IDS_MSG_NO_OBEX_SERVERS, NULL, 0);
					}
                    return TRUE;
            	}
				
				case IDS_PUSH:
				{
					MSG_FATAL("***zzg SendFileDlg IDS_PUSH", 0, 0, 0);
					BTApp_OPPPushEx(pMe, pMe->m_pfilepath, AEEBT_OPP_UNKNOWN_TYPE);	
                    return TRUE;
				}
				
				case IDS_DISCONNECT:	
				{
					int temp=0;
					temp = IBTEXTOPP_Disconnect(pMe->mOPP.po);

					MSG_FATAL("***zzg SendFileDlg IDS_DISCONNECT result=%x", temp, 0, 0);

					if (temp != SUCCESS)
					{
						BTApp_ShowMessage(pMe, IDS_MSG_DISCONN_FAILED, NULL, 3);
					}
					else
					{
						CLOSE_DIALOG(DLGRET_CANCELED)
					}

					/*
					MSG_FATAL("***zzg SendFileDlg IDS_DISCONNECT", 0, 0, 0);
					
					if (IBTEXTOPP_Disconnect(pMe->mOPP.po) != SUCCESS)
					{
						BTApp_ShowMessage(pMe, IDS_MSG_DISCONN_FAILED, NULL, 3);
					}
					else
					{
						CLOSE_DIALOG(DLGRET_CANCELED)
					}
					*/
                    return TRUE;						
				}
				
				default:
				{
					ASSERT_NOT_REACHABLE;
				}
            }
			
            return TRUE;
        }

		default:
        {
			break;
		}
    }
    return FALSE;
}	


static boolean HandleObexListServersDialogEvent(CBTApp *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{	 
    PARAM_NOT_REF(dwParam)

    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_BT_OBEX_LIST_SERVERS);

	AEEBTDeviceInfo*  pDev = &pMe->mRM.device[pMe->mRM.uCurDevIdx];
	
    if (pMenu == NULL)
    {
        return FALSE;
    }
	
    MSG_FATAL("***zzg HandleObexListServersDialogEvent eCode=%x, wParam=%x, dwParam=%x,***",eCode,wParam,dwParam);   

    switch (eCode)
    {
        case EVT_DIALOG_INIT:			
		{			
			AECHAR 		WTitle[40] = {0};			

			(void)ISHELL_LoadResString(pMe->m_pShell,
				                       AEE_APPSBTAPP_RES_FILE,                                
				                       IDS_OBEX_SERVERS,
				                       WTitle,
				                       sizeof(WTitle));
			
			IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, WTitle); 		
			
            return TRUE;
		}

        case EVT_DIALOG_START:       
        {
			AEEBTDeviceEnumerator enumerator;
			AEEBTDeviceInfo*      pDev;
			CtlAddItem            ai;
			AECHAR                wName[32];
			uint16                stringID;
			uint16                msgID;
			uint8                 i;
#ifdef FEATURE_BT_2_1
			AECHAR*               pwName;
			AECHAR                wBuf[ 5 ];
			STRTOWSTR("...", wBuf, sizeof(wBuf));
#endif 
			enumerator.control = AEEBT_RM_EC_MATCH_SERVICE_CLASS;
			enumerator.svcCls  = AEEBT_COD_SC_OBJECT_TRANSFER;
			stringID           = IDS_OBEX_SERVERS;
			msgID              = IDS_MSG_NO_OBEX_SERVERS;			

			if (pMe->mRM.po == NULL)
			{
				MSG_FATAL("***zzg ObexListServers pMe->mRM.po == NULL***", 0, 0, 0);
			}
			else
			{				
				if (IBTEXTRM_DeviceEnumInit(pMe->mRM.po, &enumerator) == SUCCESS)
				{					
					BTApp_InitAddItem(&ai);
					ai.wFont = AEE_FONT_BOLD;

					i	 = 0;
					pDev = &pMe->mRM.device[i];

					while ((IBTEXTRM_DeviceEnumNext( pMe->mRM.po, pDev ) == SUCCESS) 
							&& pDev->bValid && (i < MAX_DEVICES))
					{
						if (WSTRLEN( pDev->wName ) == 0)
						{
							BTApp_BDAddr2Wstr( wName, &pDev->bdAddr );
						}
						else
						{
							WSTRLCPY( wName, pDev->wName, ARR_SIZE( wName ) );
						}					
						
#ifdef FEATURE_BT_2_1
						pwName = pDev->wName;

						if ((pDev->EIRData.uFlags & AEEBT_EIR_DATA_RCVD_B) 
							&& (!(pDev->EIRData.uFlags & AEEBT_EIR_NAME_CMPLT_B)))
						{
							WSTRLCAT( pwName, wBuf, AEEBT_MAX_DEVICENAME_LEN );
						}

						IMENUCTL_AddItem(pMenu, NULL, 0, i, pwName, 0 );
#else
						IMENUCTL_AddItem(pMenu, NULL, 0, i, wName, 0 );
#endif 

						if (pDev->bBonded)
						{
							IMENUCTL_SetItem(pMenu, i, MSIF_FONT, &ai );
						}
						
						ai.wImage = BTApp_GetDevTypeImageID( pMe, pDev->serviceClass,&pDev->bdAddr );
						IMENUCTL_SetItem(pMenu, i, MSIF_IMAGE, &ai );
						
						pDev = &pMe->mRM.device[ ++i ];
					}

					if (i > 0)
					{
						MSG_FATAL("***zzg OBEXListServersDialog i=%d***", i, 0, 0);
						
						IMENUCTL_SetSel(pMenu, MENU_SEL);
					}
					else
					{
						MSG_FATAL("***zzg OBEXListServersDialog IBTEXTRM_DeviceEnumInit i == 0***", 0, 0, 0);	
						return TRUE;
					}
				}					
					
			}


#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION

            IMENUCTL_SetSel(pMenu, pMe->m_currDlgId);

            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);
            IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);

#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SETTING_BACKGROUND);
#endif
            IMENUCTL_SetBottomBarType(pMenu, BTBAR_SELECT_BACK);

			IMENUCTL_SetSel(pMenu,  MENU_SEL);  // highlight the selected item
			
            (void) ISHELL_PostEvent( pMe->m_pShell,
                                     AEECLSID_BLUETOOTH_APP,
                                     EVT_USER_REDRAW,
                                     0,
                                     0);
            return TRUE;
        }
		
        case EVT_USER_REDRAW:    
		{	
			(void)IMENUCTL_Redraw(pMenu);

			// Activate menu
			IMENUCTL_SetActive(pMenu, TRUE);
			IDISPLAY_UpdateEx(pMe->m_pIDisplay, FALSE);			
            return TRUE;
        }
		
        case EVT_DIALOG_END:
        {
			return TRUE;
        }

        case EVT_KEY_PRESS:
        {
			switch(wParam)
            {
            	case AVK_INFO:	
				case AVK_SELECT:
				{

					//Add By zzg 2011_02_16
					MSG_FATAL("***zzg ObexListServerDlgHandle m_obex_list_id=%d***", pMe->m_obex_list_id, 0, 0);

					if (pMe->m_obex_list_id == IDD_BT_FTP_CLIENT)					
					{
						if (IMENUCTL_GetItemCount(pMenu) > 0)
						{
							pMe->mRM.uCurDevIdx = IMENUCTL_GetSel(pMenu);
							MENU_SET_SEL(pMe->mRM.uCurDevIdx);
#ifdef FEATURE_BT_2_1
						//Client side service security settings
							IBTEXTRM_SetSecBySvcCls(pMe->mRM.po, AEEBT_SD_SERVICE_CLASS_OBEX_FILE_TRANSFER, pMe->mFTP.srvSecType,FALSE,FALSE);
#endif 
							BTApp_FTPConnect(pMe, &pMe->mRM.device[pMe->mRM.uCurDevIdx].bdAddr);	
						}
					}
					//Add End
					else if (pMe->m_obex_list_id == IDD_BT_SEND_FILE)	
					{
						if (IMENUCTL_GetItemCount(pMenu) > 0)
						{
							pMe->mRM.uCurDevIdx = IMENUCTL_GetSel(pMenu);
							MENU_SET_SEL(pMe->mRM.uCurDevIdx);
							
#ifdef FEATURE_BT_2_1
							//Client side service security settings
							IBTEXTRM_SetSecBySvcCls(pMe->mRM.po, AEEBT_SD_SERVICE_CLASS_OBEX_OBJECT_PUSH, pMe->mOPP.srvSecType,FALSE,FALSE);
#endif 
							BTApp_OPPConnect( pMe, &pMe->mRM.device[ pMe->mRM.uCurDevIdx ].bdAddr);  
							//CLOSE_DIALOG(DLGRET_DEVICEINFO)
						}	
					}
					
					return TRUE;
            	}
                case AVK_CLR:
                {
					CLOSE_DIALOG(DLGRET_CANCELED) 
                    return TRUE;
                }
                default:
                {
					break;
                }
            }
            return TRUE;        
        }
        default:
        {
			break;
        }
    }
	return FALSE;
}	

static boolean HandleFileProgressDialogEvent(CBTApp *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{	 
    PARAM_NOT_REF(dwParam)
		
	IStatic  *pStatic = (IStatic*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_BT_FILE_PROGRESS);	

	if (pStatic == NULL)
	{
		return FALSE;
	}	

	switch (eCode)
	{
		case EVT_DIALOG_INIT:			
		{
			//IDIALOG_SetProperties((IDialog *)dwParam, DLG_NOT_REDRAW_AFTER_START);
			return TRUE;
		}

		case EVT_DIALOG_START:
		{			
			AEERect 	rc;
			AECHAR		WTitle[40] = {0};		
			
			(void)ISHELL_LoadResString(pMe->m_pShell,
										AEE_APPSBTAPP_RES_FILE, 							   
										IDS_BT_TITLE,
										WTitle,
										sizeof(WTitle));
			
			IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, WTitle);  	

			IDISPLAY_Backlight(pMe->m_pIDisplay, TRUE);

			// set rect for info display area
			SETAEERECT ( &rc, pMe->m_rect.x, 
						 pMe->m_rect.y, 
						 pMe->m_rect.dx, 
						 pMe->m_rect.dy - BOTTOMBAR_HEIGHT); // leave room for SK menu
					 
			ISTATIC_SetRect(pStatic, &rc );

			ISTATIC_SetProperties(pStatic, ISTATIC_GetProperties(pStatic) & ~ST_MIDDLETEXT & ~ST_CENTERTEXT);	 
			ISTATIC_SetProperties(pStatic, ST_NOSCROLL|ST_GRAPHIC_BG);	
			ISTATIC_SetBackGround(pStatic, AEE_APPSCOMMONRES_IMAGESFILE, IDB_BACKGROUND);	
			
			(void) ISHELL_PostEvent( pMe->m_pShell,
									 AEECLSID_BLUETOOTH_APP,
									 EVT_USER_REDRAW,
									 0,
									 0);

			
			return TRUE;
		}
		
		case EVT_USER_REDRAW:		
		{	
			AECHAR  wTempBuf[64];
			AECHAR* pText = pMe->pText2;
			AEERect rc;
			uint8   len = 0;

			int		percent = 0;
			char    szConBuf[10];

			MSG_FATAL("***zzg FileProgressDialog EVT_USER_REDRAW  numBytes=%d, objSize=%d***", pMe->m_fileprogInfo.numBytes, pMe->m_fileprogInfo.objSize, 0);
			
			if (pMe->m_fileprogInfo.numBytes > 0)
			{		
				percent = pMe->m_fileprogInfo.numBytes*100/pMe->m_fileprogInfo.objSize;		

				snprintf(szConBuf, 10, " %d/100 ", percent);			

				ISHELL_LoadResString( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, IDS_FILE_PROGRESS, pMe->pText2, LONG_TEXT_BUF_LEN * sizeof(AECHAR) );
				len = WSTRLEN(pMe->pText2);
				STRTOWSTR(szConBuf, &pMe->pText2[len], (LONG_TEXT_BUF_LEN-len)*sizeof(AECHAR));

				ISTATIC_SetText(pStatic, NULL, pText, AEE_FONT_BOLD, AEE_FONT_NORMAL );
						
			}	  			
			
			ISTATIC_Redraw(pStatic);	
			ISTATIC_SetActive(pStatic, TRUE);		

			BT_DRAW_BOTTOMBAR(BTBAR_BACK)
				
			IDISPLAY_UpdateEx( pMe->m_pIDisplay, FALSE); 			
			return TRUE;
		}

		case EVT_DIALOG_END:
		{
			int     result = 0;
			
			pMe->m_fileprogInfo.numBytes	= 0;
			pMe->m_fileprogInfo.objSize		= 0;	

			if (NULL != pMe->mOPP.po)
			{
				IBTEXTOPP_Abort(pMe->mOPP.po);
			}

			//Add By zzg 2011_12_30
			if (NULL != pMe->mFTP.po)
			{				
				if( pMe->mFTP.bObjectTransfer )
				{
					result = IBTEXTFTP_Abort(pMe->mFTP.po);
					
					if( result != SUCCESS )
					{
						MSG_MED( "Abort failed %d", result, 0, 0 );
						//BTApp_ShowMessage( pMe, IDS_MSG_ABORT_FAILED, 0, 2 );
					}
					else
					{
						pMe->mFTP.bObjectTransfer = FALSE;
						MSG_MED( "Aborted", 0, 0, 0 );
					}
				}					
			}
			//Add End
            
			if ((pMe->mOPP.bConnected == TRUE) || ((pMe->mOPP.bConnecting == TRUE)))
			{
				if (IBTEXTOPP_Disconnect( pMe->mOPP.po) != SUCCESS)
				{
					MSG_FATAL("***zzg IBTEXTOPP_Disconnect != SUCCESS***", 0, 0, 0);
				}	
				else 
				{
					pMe->mOPP.bConnecting = FALSE;
				}
			}
			return TRUE;
		}

		case EVT_KEY_PRESS: 	  
		{	
			switch(wParam)
			{			
				case AVK_CANCEL:
				case AVK_CLR:	
				{			
					CLOSE_DIALOG(DLGRET_PROGRESS_CANCELED)	
					return TRUE;
				}
				default:
				{
					break;
				}
			}
			
			return TRUE;
		}
		
		default:
		{
			break;
		}
	}

	return FALSE;		 
}	

static boolean BTApp_SaveTextEdit( CBTApp* pMe, uint16 DlgID)
{
	if (pMe == NULL)
	{
		return FALSE;
	}

	switch (DlgID)
	{			
		case IDS_EDIT_NAME: 		
		{
			if (WSTRCMP(pMe->mRM.myInfo.wName, pMe->pText2) != 0)
			{
				WSTRLCPY(pMe->mRM.myInfo.wName, pMe->pText2, ARR_SIZE(pMe->mRM.myInfo.wName));
				
				if (IBTEXTRM_SetName(pMe->mRM.po, pMe->mRM.myInfo.wName) != SUCCESS)
				{
					MSG_ERROR("TextEditSave - SetName failed", 0, 0, 0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif 
				}
				else
				{
					BTApp_ShowBusyIcon(pMe);	
				}
			}	
			break;
		}
		
#ifdef FEATURE_BT_2_1
		case IDS_EDIT_SHORT_NAME:		
		{   
			if (WSTRCMP(pMe->mRM.myInfo.wShortName, pMe->pText2) != 0)
			{
				WSTRLCPY(pMe->mRM.myInfo.wShortName, pMe->pText2, ARR_SIZE(pMe->mRM.myInfo.wShortName));

				if((WSTRLEN( pMe->mRM.myInfo.wShortName) > AEEBT_MAX_NICKNAME_LEN) 
					|| IBTEXTRM_SetNickName(pMe->mRM.po, NULL, pMe->mRM.myInfo.wShortName) != SUCCESS)
				{
					MSG_ERROR( "TextEditSave - SetShortName failed", 0, 0, 0 );
				}
				else
				{
					BTApp_ShowBusyIcon(pMe);
				}
			}     
			break;
		}

		case IDS_EDIT_MANU_DATA:		
		{
			if (WSTRCMP(pMe->mRM.wManuData, pMe->pText2) != 0)
			{
				char szText[AEEBT_MAX_EIR_MANUF_DATA_LEN + 1];
				WSTRLCPY(pMe->mRM.wManuData, pMe->pText2, ARR_SIZE(pMe->mRM.wManuData));
				WSTRTOSTR(pMe->mRM.wManuData, szText, sizeof(szText));
				
				if ((IBTEXTRM_SetEIRManufData(pMe->mRM.po,(uint8*)szText, STRLEN(szText)) != SUCCESS))
				{
					MSG_ERROR( "TextEditSave- SetManuData failed", 0, 0, 0 );
				}
				else
				{
					BTApp_ShowBusyIcon(pMe);
				} 
			}
			break;
		}
#endif			

		default:
		{
			break;
		}	
			
	}

	return TRUE;	
}


static boolean BTApp_ClearDiscoverableEx( CBTApp* pMe )
{
  int result;

  MSG_FATAL("***zzg BTApp_ClearDiscoverableEx***", 0, 0, 0);

  ISHELL_CancelTimer(pMe->m_pShell, (PFNNOTIFY) BTApp_ClearDiscoverableEx, pMe);

  if ((result = IBTEXTSD_SetDiscoverable(pMe->mSD.po, FALSE)) != SUCCESS)
  {
  	 MSG_FATAL("***zzg IBTEXTSD_SetDiscoverable FALSE***", 0, 0, 0);	 
     //BTApp_ShowMessage( pMe, IDS_MSG_DISCOVERABLE_NOT_CLEARED, NULL, 2 );
  }
  else
  {
    BTApp_ShowBusyIcon( pMe );
  }

  return (result == SUCCESS);
}

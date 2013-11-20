/*lint -e734*/
/*==============================================================================
// 文件：
//        UTKDialogHandler.c
//        2007-11-13 陈喜玲草拟版本(Draft Version)
//        版权所有(c) 2004 Anylook
//        
// 文件说明：
//        
// 作者：2007-11-13
// 创建日期：陈喜玲
// 当前版本：Draft
==============================================================================*/

/*==============================================================================
修改历史记录：
时       间      修 改 人     修改内容、位置及原因
-----------      ----------     -----------------------------------------------
07-11-13         陈喜玲         初始版本
==============================================================================*/


/*==============================================================================
                                 本文件包含的外部文件
==============================================================================*/
#include "UTK_priv.h"
#ifdef FEATURE_USES_ZI
#include "OEMZIText.h" 
#else
#include "OEMText.h" 
#endif
//#include "uasms.h"
#include "UTKUtils.h"

/*==============================================================================
                                 宏定义和常数
==============================================================================*/

/*==============================================================================
                                 类型定义
==============================================================================*/


/*==============================================================================
                                 函数声明
==============================================================================*/
static void UTKApp_DialogTimeout(void *pme);
static void UTKApp_DialogRedrawTimeout(void *pme);

static void SetMenuLook( CUTK *pMe, IMenuCtl *pMenu);

// 对话框 IDD_MAIN_DIALOG 事件处理函数
static boolean  IDD_MAIN_Handler(CUTK *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

// 对话框 IDD_LIST_DIALOG 事件处理函数
static boolean  IDD_LIST_Handler(CUTK *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

// 对话框 IDD_INPUT_DIALOG 事件处理函数
static boolean  IDD_INPUT_Handler(CUTK *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

// 对话框 IDD_DISPLAY_DIALOG 事件处理函数
static boolean  IDD_DISPLAY_Handler(CUTK *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

// 对话框 IDD_SENDMSG_DIALOG 事件处理函数
static boolean  IDD_SENDMSG_Handler(CUTK *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

static boolean  IDD_POPMSG_Handler(CUTK *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

static boolean  IDD_MSGBOX_Handler(CUTK *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

static boolean  IDD_PLAYTONE_Handler(CUTK *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

static void UTKApp_PlaytoneTimeout(void *pme);

static void UTKApp_NotifyMp3Player(CUTK *pMe,boolean toStartAlert);
static void UTKApp_NotifyMp3PlayerCB(CUTK *pMe);
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
       UTK_ShowDialog
说明:
       函数由状态处理函数用来创建相应对话框资源ID的对话框。

参数:
       pMe [In]: 指向UTK Applet对象结构的指针,该结构包含小程序的特定信息.
       dlgResId [in]：对话框资源ID，函数根据该ID创建对话框。

返回值:
       无

备注:
       

==============================================================================*/
void UTK_ShowDialog(CUTK *pMe,uint16  dlgResId)
{
    int nRet;

    // At most one dialog open at once
    if (ISHELL_GetActiveDialog(pMe->m_pShell) != NULL)
    {
        // Looks like there is one dialog already opened.
        // Flag an error an return without doing anything.
        MSG_FATAL("Trying to create a dialog without closing the previous one",0,0,0);
        return;
    }

    nRet = ISHELL_CreateDialog(pMe->m_pShell,UTK_RES_FILE_LANG,dlgResId,NULL);

    if (nRet != SUCCESS)
    {
        MSG_FATAL("Failed to create the dialog in the UTK applet:%02d",nRet,0,0);
        nRet = ISHELL_CreateDialog(pMe->m_pShell,UTK_RES_FILE,dlgResId,NULL);
    }
}

/*==============================================================================
函数:
       UTK_RouteDialogEvent
说明:
       函数将BREW事件路由给当前活动对话框的事件处理函数。

参数:
       pMe [In]: 指向UTK Applet对象结构的指针,该结构包含小程序的特定信息.
       eCode [in]：事件代码。
       wParam [in]：与事件eCode关联的数据。
       dwParam [in]：与事件eCode关联的数据。

返回值:
       TRUE：传入事件被处理。
       FALSE：传入事件没被处理。

备注:

==============================================================================*/
boolean UTK_RouteDialogEvent(CUTK *pMe,
    AEEEvent    eCode,
    uint16  wParam,
    uint32 dwParam
)
{
    if (NULL == pMe)
    {
        return FALSE;
    }
    
    MSG_FATAL("UTK_RouteDialogEvent Start 0x%x %d",pMe->m_pActiveDlg,pMe->m_pActiveDlgID,0);
    if (NULL == pMe->m_pActiveDlg)
    {
        return FALSE;
    }
    
    switch (pMe->m_pActiveDlgID)
    {
#ifdef FEATURE_CARRIER_CHINA_TELCOM        
        case IDD_SPLASH_DIALOG:
            return IDD_SPLASH_Handler(pMe,eCode,wParam,dwParam);
#endif //FEATURE_CARRIER_CHINA_TELCOM            
        
        case IDD_MAIN_DIALOG:
            return IDD_MAIN_Handler(pMe,eCode,wParam,dwParam);
            
        case IDD_LIST_DIALOG:
            return IDD_LIST_Handler(pMe,eCode,wParam,dwParam);
            
        case IDD_INPUT_DIALOG:
            return IDD_INPUT_Handler(pMe,eCode,wParam,dwParam);
            
        case IDD_DISPLAY_DIALOG:
            return IDD_DISPLAY_Handler(pMe,eCode,wParam,dwParam);

        case IDD_SENDMSG_DIALOG:
            return IDD_SENDMSG_Handler(pMe,eCode,wParam,dwParam);
    
        case IDD_PLAYTONE_DIALOG:
            return IDD_PLAYTONE_Handler(pMe,eCode,wParam,dwParam);
            
        case IDD_MSGBOX:
            return IDD_MSGBOX_Handler(pMe,eCode,wParam,dwParam);   
            
        case IDD_POPMSG:
            return IDD_POPMSG_Handler(pMe,eCode,wParam,dwParam); 

        default:
            return FALSE;
    }
}

#ifdef FEATURE_CARRIER_CHINA_TELCOM
/*==============================================================================
函数：
    IDD_SPLASH_Handler
    
说明：
    IDD_SPLASH_DIALOG 对话框事件处理函数
       
参数：
    pMe [in]：指向UTK Applet对象结构的指针。该结构包含小程序的特定信息。
    eCode [in]：事件代码。
    wParam：事件相关数据。
    dwParam：事件相关数据。
       
返回值：
    TRUE：传入事件被处理。
    FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean  IDD_SPLASH_Handler(CUTK *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    PARAM_NOT_REF(wParam)
    MSG_FATAL("IDD_SPLASH_Handler Start",0,0,0);
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            return TRUE;
            
        case EVT_DIALOG_START:
            (void) ISHELL_PostEvent( pMe->m_pShell,
                                    AEECLSID_APP_UTK,
                                    EVT_USER_REDRAW,
                                    0,
                                    0);
            return TRUE;
            
        case EVT_USER_REDRAW: 
            {
                IImage *pImg = NULL;
                
                pImg = ISHELL_LoadResImage(pMe->m_pShell,
                                        AEE_APPSCOMMONRES_IMAGESFILE,
                                        IDB_COLOR_UTK_SPLASH);                 
                
                if (NULL != pImg)
                {
                    IIMAGE_Draw(pImg,0,0); 
                    IIMAGE_Release(pImg); 
                }
            }   
            (void) ISHELL_SetTimer(pMe->m_pShell,
                     1000,
                     UTKApp_DialogTimeout,
                     pMe);

            return TRUE;
            
        case EVT_DIALOG_END:
            pMe->m_eDlgRet = DLGRET_CANCELED;
            return TRUE;
            
        case EVT_DIALOGTIMEOUT:
            CLOSE_DIALOG(DLGRET_CANCELED);
            return TRUE;
            
        case EVT_KEY:
            return TRUE;
            
        default:
            break;
    }
    
    return FALSE;
} // IDD_SPLASH_Handler
#endif //FEATURE_CARRIER_CHINA_TELCOM

/*==============================================================================
函数：
       IDD_MAIN_Handler
说明：
       IDD_MAIN_DIALOG对话框事件处理函数
       
参数：
       pMe [in]：指向UTK Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean  IDD_MAIN_Handler(CUTK *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    PARAM_NOT_REF(dwParam)

    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_MAIN_MENU);
    MSG_FATAL("IDD_MAIN_Handler Start 0x%x 0x%x 0x%x",eCode,wParam,dwParam);
    if (pMenu == NULL)
    {
        return FALSE;
    }
    
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            SetMenuLook(pMe, pMenu);
            pMe->select_item[1]=0;
            pMe->level = 0;
            pMe->clr_back = FALSE;
            return TRUE;
            
        case EVT_DIALOG_START:
            (void) CUTK_SetUTKMenu(pMe, pMenu, UIM_TK_SETUP_MENU);
            
            // Restore the menu select from suspend
            if(pMe->m_wSelectStore != MENU_SELECT_NULL)
            {
                IMENUCTL_SetSel(pMenu, pMe->m_wSelectStore);
            }
            else
            {
                IMENUCTL_SetSel(pMenu,pMe->select_item[0]);  
            }
            (void) ISHELL_PostEvent( pMe->m_pShell,
                                     AEECLSID_APP_UTK,
                                     EVT_USER_REDRAW,
                                     0,
                                     0);
            return TRUE;
            
        case EVT_USER_REDRAW:
            //(void)IMENUCTL_Redraw(pMenu);    //dele by yangdecai
            IANNUNCIATOR_Redraw(pMe->m_pIAnn);
            IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE);
            if (IMENUCTL_GetItemCount(pMenu)==0)
            {
                IANNUNCIATOR_SetFieldTextEx(pMe->m_pIAnn,NULL,FALSE);
                CLOSE_DIALOG(DLGRET_POPMSG)   
            }
            return TRUE;
            
        case EVT_DIALOG_END:
            // Store the menu select if applet is suspended.
            if(pMe->m_bSuspending)
            {
                pMe->m_wSelectStore = IMENUCTL_GetSel(pMenu);
            }
            else
            {
                pMe->m_wSelectStore = MENU_SELECT_NULL;
            }
            return TRUE;
            
        case EVT_KEY:
            switch(wParam)
            {                
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)                    
                    return TRUE;
#if 0//def FEATURE_OEMOMH
                case AVK_1:
                {
                    //69 74 68 20 74 68 65 20 6D 65 6E 75
                    byte buf[256]={ 0xD0, 0x81, 0xFC, 0x81, 0x03, 0x01, 0x25, 0x00, 0x82, 0x02, 0x81, 0x82,
                                    0x85, 0x81, 0xEC, 0x54, 0x68, 0x65, 0x20, 0x53, 0x49, 0x4D, 0x20, 0x73,
                                    0x68, 0x61, 0x6C, 0x6C, 0x20, 0x73, 0x75, 0x70, 0x70, 0x6C, 0x79, 0x20,
                                    0x61, 0x20, 0x73, 0x65, 0x74, 0x20, 0x6F, 0x66, 0x20, 0x6D, 0x65, 0x6E,
                                    0x75, 0x20, 0x69, 0x74, 0x65, 0x6D, 0x73, 0x2C, 0x20, 0x77, 0x68, 0x69,
                                    0x63, 0x68, 0x20, 0x73, 0x68, 0x61, 0x6C, 0x6C, 0x20, 0x62, 0x65, 0x20,
                                    0x69, 0x6E, 0x74, 0x65, 0x67, 0x72, 0x61, 0x74, 0x65, 0x64, 0x20, 0x77,
                                    0x69, 0x74, 0x68, 0x20, 0x74, 0x68, 0x65, 0x20, 0x6D, 0x65, 0x6E, 0x75,
                                    0x20, 0x73, 0x79, 0x73, 0x74, 0x65, 0x6D, 0x20, 0x28, 0x6F, 0x72, 0x20,
                                    0x6F, 0x74, 0x68, 0x65, 0x72, 0x20, 0x4D, 0x4D, 0x49, 0x20, 0x66, 0x61,
                                    0x63, 0x69, 0x6C, 0x69, 0x74, 0x79, 0x29, 0x20, 0x69, 0x6E, 0x20, 0x6F,
                                    0x72, 0x64, 0x65, 0x72, 0x20, 0x74, 0x6F, 0x20, 0x67, 0x69, 0x76, 0x65,
                                    0x20, 0x74, 0x68, 0x65, 0x20, 0x75, 0x73, 0x65, 0x72, 0x20, 0x74, 0x68,
                                    0x65, 0x20, 0x6F, 0x70, 0x70, 0x6F, 0x72, 0x74, 0x75, 0x6E, 0x69, 0x74,
                                    0x79, 0x20, 0x74, 0x6F, 0x20, 0x63, 0x68, 0x6F, 0x6F, 0x73, 0x65, 0x20,
                                    0x6F, 0x6E, 0x65, 0x20, 0x6F, 0x66, 0x20, 0x74, 0x68, 0x65, 0x73, 0x65,
                                    0x20, 0x6D, 0x65, 0x6E, 0x75, 0x20, 0x69, 0x74, 0x65, 0x6D, 0x73, 0x20,
                                    0x61, 0x74, 0x20, 0x68, 0x69, 0x73, 0x20, 0x6F, 0x77, 0x6E, 0x20, 0x64,
                                    0x69, 0x73, 0x63, 0x72, 0x65, 0x74, 0x69, 0x6F, 0x6E, 0x2E, 0x20, 0x45,
                                    0x61, 0x63, 0x68, 0x20, 0x69, 0x74, 0x65, 0x6D, 0x20, 0x63, 0x6F, 0x6D,
                                    0x70, 0x72, 0x69, 0x73, 0x65, 0x73, 0x20, 0x61, 0x20, 0x73, 0x68, 0x8F,
                                    0x02, 0x01, 0x59,
                                   };

                    UTK_SendSimuData(buf, 255);
                    break;
                }
                case AVK_2:
                {
                    //69 74 68 20 74 68 65 20 6D 65 6E 75
                    byte buf[256]={ 0xD0, 0x27, 0x81, 0x03, 0x01, 0x23, 0x04, 0x82, 0x02, 0x81, 0x82, 0x8D,
                                    0x18, 0x04, 0x50, 0x61, 0x73, 0x73, 0x77, 0x6F, 0x72, 0x64, 0x20, 0x31,
                                    0x3C, 0x53, 0x45, 0x4E, 0x44, 0x3E, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
                                    0x38, 0x91, 0x02, 0x04, 0x08, 0x00
                                   };

                    UTK_SendSimuData(buf, 41);
                    break;
                }
                
                case AVK_3:
                {
                    //69 74 68 20 74 68 65 20 6D 65 6E 75
                    byte buf[256]={ 0xD0, 0x81, 0xB1, 0x81, 0x03, 0x01, 0x23, 0x00, 0x82, 0x02, 0x81, 0x82, 
                                    0x8D, 0x81, 0xA1, 0x04, 0x2A, 0x2A, 0x2A, 0x31, 0x31, 0x31, 0x31, 0x31, 
                                    0x31, 0x31, 0x31, 0x31, 0x31, 0x23, 0x23, 0x23, 0x2A, 0x2A, 0x2A, 0x32, 
                                    0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x23, 0x23, 0x23, 
                                    0x2A, 0x2A, 0x2A, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 
                                    0x33, 0x23, 0x23, 0x23, 0x2A, 0x2A, 0x2A, 0x34, 0x34, 0x34, 0x34, 0x34, 
                                    0x34, 0x34, 0x34, 0x34, 0x34, 0x23, 0x23, 0x23, 0x2A, 0x2A, 0x2A, 0x35, 
                                    0x35, 0x35, 0x35, 0x35, 0x35, 0x35, 0x35, 0x35, 0x35, 0x23, 0x23, 0x23, 
                                    0x2A, 0x2A, 0x2A, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 
                                    0x36, 0x23, 0x23, 0x23, 0x2A, 0x2A, 0x2A, 0x37, 0x37, 0x37, 0x37, 0x37, 
                                    0x37, 0x37, 0x37, 0x37, 0x37, 0x23, 0x23, 0x23, 0x2A, 0x2A, 0x2A, 0x38, 
                                    0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x23, 0x23, 0x23, 
                                    0x2A, 0x2A, 0x2A, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 
                                    0x39, 0x23, 0x23, 0x23, 0x2A, 0x2A, 0x2A, 0x30, 0x30, 0x30, 0x30, 0x30, 
                                    0x30, 0x30, 0x30, 0x30, 0x30, 0x23, 0x23, 0x23, 0x91, 0x02, 0xA0, 0xA0, 
                                   };

                    UTK_SendSimuData(buf, 180);
                    break;
                }
                case AVK_4:
                {
                    //69 74 68 20 74 68 65 20 6D 65 6E 75
                    byte buf[256]={ 0xD0, 0x1E, 0x81, 0x03, 0x01, 0x10, 0x00, 0x82, 0x02, 0x81, 0x83, 0x85,
                                    0x08, 0x4E, 0x6F, 0x74, 0x20, 0x62, 0x75, 0x73, 0x79, 0x86, 0x09, 0x91,
                                    0x10, 0x32, 0x04, 0x21, 0x43, 0x65, 0x1C, 0x2C, 0x00
                                   };

                    UTK_SendSimuData(buf, 32);
                    break;
                }  
                case AVK_5:
                {
                    extern void UTK_SendSimuPPDL(void);
                    UTK_SendSimuPPDL();
                    break;
                }
#endif
                default:
                    break;
            }
            return TRUE;
#ifdef FEATURE_LCD_TOUCH_ENABLE//andrew add for LCD touch
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

				if(TOUCH_PT_IN_RECT(wXPos,wYPos,rc))
				{
					if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
					{
						boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_UTK,EVT_USER,AVK_SELECT,0);
						return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
					{						
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_UTK,EVT_USER,AVK_CLR,0);
						 return rt;
					}
				}

			}
			break;
#endif
        case EVT_COMMAND:
            pMe->select_item[0] =  IMENUCTL_GetSel(pMenu);
            pMe->m_bAppIsReady = FALSE;
            pMe->m_btCursel = (byte)wParam;
            UTK_GiveResponse(pMe, pMe->cmd_type, TRUE, UIM_TK_CMD_PERFORMED_SUCCESSFULLY);            
            return TRUE;
            
        default:
            break;
    }

    return FALSE;
} // IDD_MAIN_Handler


/*==============================================================================
函数：
       IDD_LIST_Handler
说明：
       IDD_LIST_DIALOG对话框事件处理函数
       
参数：
       pMe [in]：指向UTK Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean  IDD_LIST_Handler(CUTK *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    PARAM_NOT_REF(dwParam)

    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_LIST_MENU);
    MSG_FATAL("IDD_LIST_Handler Start eCode=%d, wParam=%d",eCode,wParam,0);
    if (pMenu == NULL)
    {
        return FALSE;
    }
    
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            SetMenuLook(pMe, pMenu);

            if(pMe->clr_back)
            {
               pMe->clr_back = FALSE;
            }
            else
            {
               pMe->level++;
               pMe->select_item[pMe->level] = 0;
            }
            return TRUE;
            
        case EVT_DIALOG_START:
            if(!CUTK_SetUTKMenu(pMe, pMenu, pMe->cmd_type))
            {
               pMe->clr_back = TRUE;
               UTK_GiveResponse(pMe, pMe->cmd_type, FALSE, UIM_TK_BWARD_MOVE_IN_PROACTIVE_RUIM_SESSION_REQ_BY_USER);
               return TRUE;
            }
            // Restore the menu select from suspend
            if(pMe->m_wSelectStore != MENU_SELECT_NULL)
            {
                IMENUCTL_SetSel(pMenu, pMe->m_wSelectStore);
            }
            else
            {
                IMENUCTL_SetSel(pMenu,pMe->select_item[pMe->level]);
            }
            
            pMe->m_dwTimeOut = 90000;
            (void) ISHELL_SetTimer(pMe->m_pShell,
                                   pMe->m_dwTimeOut,
                                   UTKApp_DialogTimeout,
                                   pMe);
            (void) ISHELL_PostEvent( pMe->m_pShell,
                                     AEECLSID_APP_UTK,
                                     EVT_USER_REDRAW,
                                     0,
                                     0);
            return TRUE;
            
        case EVT_USER_REDRAW:
            //(void)IMENUCTL_Redraw(pMenu);    //dele by yangdecai
            IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE);
            return TRUE;
            
        case EVT_DIALOG_END:
            // Store the menu select if applet is suspended.
            if(pMe->m_bSuspending)
            {
                pMe->m_wSelectStore = IMENUCTL_GetSel(pMenu);
            }
            else
            {
                pMe->m_wSelectStore = MENU_SELECT_NULL;
            }
            (void)ISHELL_CancelTimer(pMe->m_pShell, UTKApp_DialogTimeout, pMe);
            return TRUE;
            
        case EVT_DIALOGTIMEOUT:
            MSG_FATAL("EVT_DIALOGTIMEOUT",0,0,0); 
            UTK_GiveResponse(pMe, pMe->cmd_type, FALSE, UIM_TK_NO_RESPONSE_FROM_USER);
            return TRUE;

        case EVT_KEY_PRESS:
            if(wParam == AVK_END)
            {
                UTK_GiveResponse(pMe, pMe->cmd_type, FALSE, UIM_TK_PROACTIVE_RUIM_SESSION_TERMINATED_BY_USER);
            }
            return TRUE;
            
        case EVT_KEY:
            switch(wParam)
            {
                case AVK_END:
                    (void)ISHELL_CancelTimer(pMe->m_pShell, UTKApp_DialogTimeout, pMe);
                    UTK_GiveResponse(pMe, pMe->cmd_type, FALSE, UIM_TK_PROACTIVE_RUIM_SESSION_TERMINATED_BY_USER);
                    break;
                    
                case AVK_CLR:
                    (void)ISHELL_CancelTimer(pMe->m_pShell, UTKApp_DialogTimeout, pMe);
                    pMe->select_item[pMe->level] = 0;
                    pMe->level--;
                    pMe->clr_back = TRUE;
                    pMe->m_bAppIsReady = FALSE;
                    UTK_GiveResponse(pMe, pMe->cmd_type, FALSE, UIM_TK_BWARD_MOVE_IN_PROACTIVE_RUIM_SESSION_REQ_BY_USER);
                    return TRUE;

                default:
                    break;
            }
            return TRUE;

        case EVT_COMMAND:
        {
            CtlAddItem ai;
            (void)ISHELL_CancelTimer(pMe->m_pShell, UTKApp_DialogTimeout, pMe);
            pMe->select_item[pMe->level] =  IMENUCTL_GetSel(pMenu); 
            IMENUCTL_GetItem(pMenu, IMENUCTL_GetSel(pMenu), &ai);
            IMENUCTL_SetTitle(pMenu, NULL, 0, (AECHAR *)ai.pText);
            ERR("wParam = %d", wParam,0,0); 
            pMe->m_bAppIsReady = FALSE;
            pMe->m_btCursel = (byte)wParam;
            UTK_GiveResponse(pMe, pMe->cmd_type, TRUE, UIM_TK_CMD_PERFORMED_SUCCESSFULLY);
            return TRUE;
        }
            
        default:
            break;
    }

    return FALSE;
} // IDD_LIST_Handler


/*==============================================================================
函数：
       IDD_INPUT_Handler
说明：
       IDD_INPUT_DIALOG对话框事件处理函数
       
参数：
       pMe [in]：指向UTK Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean  IDD_INPUT_Handler(CUTK *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    AEETextInputMode nInputMode;
    static AEERect title_rc;     
    int lineheight;
    
    PARAM_NOT_REF(dwParam)

    ITextCtl *pTextCtl = (ITextCtl *)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_INPUT_TEXT);                        
    MSG_FATAL("IDD_INPUT_Handler Start eCode=%d, wParam=%d",eCode,wParam,0);                 
    if ((NULL == pTextCtl) || (NULL == pMe))
    {
        return FALSE;
    }

    switch (eCode)
    {
        case EVT_DIALOG_INIT: 
            return TRUE;

        case EVT_DIALOG_START: 
            {
            	//set text rect
                AEERect text_rc;            
                byte   *utk_ptr;
                AECHAR temptitle[2]={' ', '\0'};
                Get_input *pgetinput = (Get_input *)MALLOC(sizeof(Get_input));
                uint32  dwPro = TP_FRAME | TP_MULTILINE | TP_FIXSETRECT | TP_DISPLAY_COUNT;    
                
                if (NULL == pgetinput)
                {
                    return FALSE;
                }
                
                utk_ptr = UTK_GetCmddata(pMe->cmd_type);   
                if (NULL == utk_ptr)
                {
                    FREEIF(pgetinput);
                    return FALSE;
                }
                                                
                DecodeGetInputData(utk_ptr, pgetinput);             

                //Set text control Properties                  
                if (pgetinput->cmd_describe.command_restricttag & UIM_TK_GETINPUT_GSM_UNICODE)
                { // 0 = SMS缺省字母表；1 = UNICODE字母表。
                    dwPro |= TP_STARKEY_SWITCH;
                } 
                else
                {
                    if (pgetinput->cmd_describe.command_restricttag & UIM_TK_GETINPUT_NUM_ALPHA)
                    { // 0 = 仅数字 (0 9， *， #， 和 +)；1 = 字母表集
                        dwPro |= TP_STARKEY_SWITCH;
                    	#if defined(FEATURE_VERSION_HITZ181)||defined(FEATURE_VERSION_MTM)
                    	(void)ITEXTCTL_SetInputMode(pTextCtl, AEE_TM_RAPID);
						#elif defined(FEATURE_VERSION_M8P)
						(void)ITEXTCTL_SetInputMode(pTextCtl, AEE_TM_CAPLOWER);
                    	#else
                        (void)ITEXTCTL_SetInputMode(pTextCtl, AEE_TM_LETTERS);
                        #endif
                    }  
                    else
                    {
                        (void)ITEXTCTL_SetInputMode(pTextCtl, AEE_TM_NUMBERS);             	
                    }                     
                }
                
                if (pgetinput->cmd_describe.command_restricttag & UIM_TK_GETINPUT_DISPLAY_TEXT)
                { // 0 = 终端可在显示器上回应用户；1 = 不以任何方式显示用户输入
                    dwPro |= (TP_PASSWORD | TP_HIDEMODETITLE) ;
                }
                ITEXTCTL_SetProperties(pTextCtl, dwPro|TP_FOCUS_NOSEL);            

               //set input title
               (void) ITEXTCTL_SetTitle(pTextCtl, NULL, NULL, temptitle);   
               lineheight = IDISPLAY_GetFontMetrics(pMe->m_pDisplay, AEE_FONT_NORMAL, NULL, NULL) + 2;
                SETAEERECT(&title_rc, 0, 0, pMe->m_rc.dx,  lineheight);
                IDISPLAY_EraseRect(pMe->m_pDisplay, &title_rc);
                IDISPLAY_FillRect(pMe->m_pDisplay, &title_rc, MAKE_RGB(16, 115, 215));
                if (pMe->m_wszInputTitle != NULL)
                {
                    FREE(pMe->m_wszInputTitle);
                    pMe->m_wszInputTitle = NULL;
                }
                pMe->m_wszInputTitle = (AECHAR *)MALLOC(sizeof(AECHAR) *(1 +  WSTRLEN(pgetinput->text.data)));
                if (pMe->m_wszInputTitle != NULL)
                {
                    MEMSET(pMe->m_wszInputTitle, 0, sizeof(AECHAR) * (1 +  WSTRLEN(pgetinput->text.data)));
                    (void)WSTRCPY(pMe->m_wszInputTitle, pgetinput->text.data);
                }         
                if (NULL != pMe->m_wszInputTitle)
                {
                    SETAEERECT(&text_rc,  0, lineheight,  pMe->m_rc.dx, pMe->m_rc.dy - GetBottomBarHeight(pMe->m_pDisplay) - lineheight);
                }
                else
                {
                    SETAEERECT(&text_rc, 0, 0, pMe->m_rc.dx,  pMe->m_rc.dy - GetBottomBarHeight(pMe->m_pDisplay));                                 
                } 
                IDISPLAY_EraseRect(pMe->m_pDisplay, &text_rc);                           
                ITEXTCTL_SetRect(pTextCtl,  &text_rc);           

                //set max length    
                pMe->m_TextMaxLen = pgetinput->response.max_len_rsp;
                pMe->m_TextMinLen = pgetinput->response.min_len_rsp;                                
                if (pMe->m_TextMaxLen > MAX_STR_ACHARS )
                {
                    pMe->m_TextMaxLen  = MAX_STR_ACHARS ;
                }        
                if (pgetinput->cmd_describe.command_restricttag & UIM_TK_GETINPUT_GSM_UNICODE)
                { // 0 = SMS缺省字母表；1 = UNICODE字母表。
                    pMe->m_TextMaxLen = pMe->m_TextMaxLen/2;
                } 
                else
                {
                    pMe->m_TextMaxLen = pMe->m_TextMaxLen*8/7;                
                } 
                ITEXTCTL_SetMaxSize(pTextCtl, pMe->m_TextMaxLen);
                
                //set default text
                if (WSTRLEN(pMe->bInputeBackup) == 0 )
                {     
                   (void)WSTRCPY(pMe->bInputeBackup, pgetinput->defaulttext.data);   
                }
                (void)ITEXTCTL_SetText(pTextCtl, pMe->bInputeBackup, -1);                
                
                //set text code
                pMe->m_TextQS = pgetinput->cmd_describe.command_restricttag;
                
                FREEIF(pgetinput);  
                pMe->m_dwTimeOut = 200000;
                (void) ISHELL_SetTimer(pMe->m_pShell,
                         pMe->m_dwTimeOut,
                         UTKApp_DialogTimeout,
                         pMe);
                pMe->m_wTitleIndex = 0;
                if(pMe->m_wszInputTitle && IDISPLAY_MeasureText(pMe->m_pDisplay, AEE_FONT_BOLD, pMe->m_wszInputTitle)>title_rc.dx)
                {
                    (void) ISHELL_SetTimer(pMe->m_pShell,
                                           1500,
                                           UTKApp_DialogRedrawTimeout,
                                           pMe);
                }
                (void) ISHELL_PostEvent( pMe->m_pShell,
                            AEECLSID_APP_UTK,
                            EVT_USER_REDRAW,
                            0,  0);
            }
            return TRUE;

        case EVT_DIALOGTIMEOUT:
            MSG_FATAL("EVT_DIALOGTIMEOUT",0,0,0); 
            UTK_GiveResponse(pMe, pMe->cmd_type, FALSE, UIM_TK_NO_RESPONSE_FROM_USER);
            return TRUE;

        case EVT_USER_REDRAW:
            if(wParam && pMe->m_wszInputTitle)
            {
                if(pMe->m_wTitleIndex<WSTRLEN(pMe->m_wszInputTitle) 
                    && IDISPLAY_MeasureText(pMe->m_pDisplay, AEE_FONT_BOLD, &pMe->m_wszInputTitle[pMe->m_wTitleIndex])>title_rc.dx)
                {
                    pMe->m_wTitleIndex++;
                }
                else
                {
                    pMe->m_wTitleIndex = 0;
                }
                (void) ISHELL_SetTimer(pMe->m_pShell,
                                       (pMe->m_wTitleIndex == 0)?1500:250,
                                       UTKApp_DialogRedrawTimeout,
                                       pMe);
            }
           // Fallthrough:
        case EVT_KEY_RELEASE:                  
           IDISPLAY_EraseRect(pMe->m_pDisplay, &title_rc);
           IDISPLAY_FillRect(pMe->m_pDisplay, &title_rc, pMe->m_themeColor);
           if (NULL != pMe->m_wszInputTitle)
           {
               RGBVAL fontColor;
           
               fontColor = IDISPLAY_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, RGB_WHITE);
               (void)IDISPLAY_DrawText(pMe->m_pDisplay,
                       AEE_FONT_BOLD,  &pMe->m_wszInputTitle[pMe->m_wTitleIndex],
                       -1, 0,  0, &title_rc, IDF_ALIGN_LEFT | IDF_ALIGN_MIDDLE | IDF_TEXT_TRANSPARENT);
               IDISPLAY_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, fontColor);
            }                
            nInputMode = ITEXTCTL_GetInputMode(pTextCtl,NULL);           
            if ((nInputMode == AEE_TM_SYMBOLS) && (eCode == EVT_KEY_RELEASE))
            {
                return TRUE;
            }

            // 绘制底部操作提示条
            if ( WSTRLEN(ITEXTCTL_GetTextPtr(pTextCtl)) > 0 &&
                 ITEXTCTL_GetCursorPos(pTextCtl) != TC_CURSORSTART )
            {
            	#ifndef FEATURE_ALL_KEY_PAD
                DRAW_BOTTOMBAR(BTBAR_OK_DELETE);
                #else
                DRAW_BOTTOMBAR(BTBAR_OK_BACK);
                #endif
            }
            else
            {
                DRAW_BOTTOMBAR(BTBAR_OK_BACK);
            }     

            IDISPLAY_Update(pMe->m_pDisplay);  
            return TRUE;

        case EVT_DIALOG_END:
            (void)ISHELL_CancelTimer(pMe->m_pShell, UTKApp_DialogRedrawTimeout, pMe);
            (void)ISHELL_CancelTimer(pMe->m_pShell, UTKApp_DialogTimeout, pMe);
            MEMSET(pMe->bInputeBackup, 0, (MAX_STR_ACHARS+2) * sizeof(AECHAR));//Gemsea040719 add            
            if(TRUE == pMe->InputeBackupNeeded)
            {
                (void)ITEXTCTL_GetText(pTextCtl, pMe->bInputeBackup, MAX_STR_ACHARS);
            }
            pMe->clr_back = TRUE; 
            pMe->InputeBackupNeeded = TRUE;
            return TRUE;

        case EVT_KEY_PRESS:
            if(wParam == AVK_END)
            {
                MSG_FATAL("IDD_INPUT_Handler AVK_END1",0,0,0);  
                UTK_GiveResponse(pMe, pMe->cmd_type, FALSE, UIM_TK_PROACTIVE_RUIM_SESSION_TERMINATED_BY_USER);
                (void)ISHELL_CancelTimer(pMe->m_pShell, UTKApp_DialogTimeout, pMe);
            }
            return TRUE;
        case EVT_KEY:
            switch(wParam)
            {
                case AVK_END:
                    MSG_FATAL("IDD_INPUT_Handler AVK_END2",0,0,0);  
                    UTK_GiveResponse(pMe, pMe->cmd_type, FALSE, UIM_TK_PROACTIVE_RUIM_SESSION_TERMINATED_BY_USER);
                    break;
                    
            	case AVK_INFO:
                case AVK_SELECT:
                    {
                        byte txtLen; 
                        AECHAR * wszBuf;
                        
                        wszBuf = ITEXTCTL_GetTextPtr(pTextCtl);
                        txtLen = (byte)WSTRLEN(wszBuf);
                        
                        if ((txtLen > pMe->m_TextMaxLen)||(txtLen < pMe->m_TextMinLen))
                        {   // 输入长度不对，提示出错
                            pMe->InputeBackupNeeded = TRUE;                              	
                            CLOSE_DIALOG(DLGRET_OK);                        	
                            return TRUE;
                        } 
                        else
                        {
                            pMe->InputeBackupNeeded = FALSE;
                        }                      
                                                                              
                        if (pMe->m_wszInputText != NULL)
                        {
                            FREE(pMe->m_wszInputText);
                            pMe->m_wszInputText = NULL;
                        }         
                        pMe->m_wszInputText = (AECHAR *)MALLOC((txtLen +1) * sizeof(AECHAR));                       
                        if (NULL != pMe->m_wszInputText)
                        {
                            MEMSET(pMe->m_wszInputText, 0, sizeof(AECHAR) * (1+txtLen));
                            (void)WSTRCPY(pMe->m_wszInputText, wszBuf);
                        }
                        UTK_GiveResponse(pMe, pMe->cmd_type, TRUE, UIM_TK_CMD_PERFORMED_SUCCESSFULLY);
                        if (NULL != pMe->m_wszInputText)
                        {
                           FREE(pMe->m_wszInputText);
                           pMe->m_wszInputText = NULL;
                       }                      
                    }
                    return TRUE;

                case AVK_CLR:
                    pMe->m_bAppIsReady = FALSE;
                    UTK_GiveResponse(pMe, pMe->cmd_type, FALSE, UIM_TK_BWARD_MOVE_IN_PROACTIVE_RUIM_SESSION_REQ_BY_USER);
                    return TRUE;

                default:
                    break;
            }
            return TRUE;            
#ifdef FEATURE_LCD_TOUCH_ENABLE//andrew add for LCD touch
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

				if(TOUCH_PT_IN_RECT(wXPos,wYPos,rc))
				{
					if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
					{
						boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_UTK,EVT_USER,AVK_SELECT,0);
						return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
					{						
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_UTK,EVT_USER,AVK_CLR,0);
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
} // IDD_INPUT_Handler

/*==============================================================================
函数：
       IDD_DISPLAY_Handler
说明：
       IDD_DISPLAY_DIALOG对话框事件处理函数
       
参数：
       pMe [in]：指向UTK Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean  IDD_DISPLAY_Handler(CUTK *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    PARAM_NOT_REF(wParam)
    PARAM_NOT_REF(dwParam)

    IStatic * pStatic = (IStatic *)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_DISPLAY_STATIC);
    MSG_FATAL("IDD_DISPLAY_Handler Start",0,0,0); 
    if (pStatic == NULL)
    {
        return FALSE;
    }
    
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            return TRUE;
            
        case EVT_DIALOG_START:
            {
                byte   *utk_ptr; 
                AEERect rc;
                
                pMe->m_dwTimeOut = 3000;

                ISTATIC_SetProperties(pStatic, 
                        ST_CENTERTEXT  | ST_MIDDLETEXT );
                utk_ptr = UTK_GetCmddata(pMe->cmd_type);
                if (utk_ptr != NULL)
                {
                    Display_text *ptext = (Display_text *)MALLOC(sizeof(Display_text));
                    if (NULL != ptext)
                    {
                        DecodeDisplayTextData(utk_ptr, ptext); 
                        (void) ISTATIC_SetText (pStatic, NULL, ptext->text.data,
                            AEE_FONT_NORMAL, AEE_FONT_NORMAL);
                        
                        if (ptext->cmd_describe.command_restricttag & 0x80)
                        { // 等待用户清除消息
                            pMe->m_bUserMode = TRUE;
                            pMe->m_dwTimeOut = 30000;
                        }
                        else
                        { //延迟后清除消息
                            pMe->m_bUserMode = FALSE;
                        }
                        
                        if(ptext->timems != 0)
                        {
                            pMe->m_dwTimeOut = ptext->timems;
                        }
                        // 释放临时分配空间
                        FREEIF(ptext);
                    }
                }
                
                if (pMe->m_bUserMode)
                {
                    SETAEERECT(&rc,  1, 0, pMe->m_rc.dx - 2, pMe->m_rc.dy - GetBottomBarHeight(pMe->m_pDisplay));
                }
                else
                {
                    SETAEERECT(&rc, 0, 0, pMe->m_rc.dx, pMe->m_rc.dy); 
                }
                ISTATIC_SetRect(pStatic,  &rc);
            }
            
            (void) ISHELL_PostEvent( pMe->m_pShell,
                                        AEECLSID_APP_UTK,
                                        EVT_USER_REDRAW,
                                        0,
                                        0);
            return TRUE;
            
        case EVT_USER_REDRAW:
            (void)ISTATIC_Redraw(pStatic);
            
            if (pMe->m_bUserMode)
            {
                // 绘制底条提示
                // 确定-----返回
                DRAW_BOTTOMBAR(BTBAR_OK_BACK)
                
                // 立即更新显示
                IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            }
            (void) ISHELL_SetTimer(pMe->m_pShell,
                     pMe->m_dwTimeOut,
                     UTKApp_DialogTimeout,
                     pMe);
            return TRUE;
            
        case EVT_DIALOG_END:
            pMe->clr_back = TRUE; 
            (void)ISHELL_CancelTimer(pMe->m_pShell, UTKApp_DialogTimeout, pMe);
            return TRUE;
            
        case EVT_DIALOGTIMEOUT:
            if (pMe->m_bUserMode)
            {
                UTK_GiveResponse(pMe, pMe->cmd_type, FALSE, UIM_TK_NO_RESPONSE_FROM_USER);
            }
            else
            {
                UTK_GiveResponse(pMe, pMe->cmd_type, FALSE, UIM_TK_CMD_PERFORMED_SUCCESSFULLY);
                CLOSE_DIALOG(DLGRET_OK);
            }
            return TRUE;

        case EVT_KEY_PRESS:
            if(wParam == AVK_END)
            {
                (void)ISHELL_CancelTimer(pMe->m_pShell, UTKApp_DialogTimeout, pMe);
                UTK_GiveResponse(pMe, pMe->cmd_type, FALSE, UIM_TK_PROACTIVE_RUIM_SESSION_TERMINATED_BY_USER);
                break;
            }
            return TRUE;
            
        case EVT_KEY:
            switch(wParam)
            {
                case AVK_END:
                    (void)ISHELL_CancelTimer(pMe->m_pShell, UTKApp_DialogTimeout, pMe);
                    UTK_GiveResponse(pMe, pMe->cmd_type, FALSE, UIM_TK_PROACTIVE_RUIM_SESSION_TERMINATED_BY_USER);
                    break;
                    
                case AVK_CLR:
                    (void)ISHELL_CancelTimer(pMe->m_pShell, UTKApp_DialogTimeout, pMe);
                    pMe->m_bAppIsReady = FALSE;
                    if (pMe->m_bUserMode)
                    {
                        UTK_GiveResponse(pMe, pMe->cmd_type, TRUE, UIM_TK_BWARD_MOVE_IN_PROACTIVE_RUIM_SESSION_REQ_BY_USER);
                    }
                    else
                    {
                        UTK_GiveResponse(pMe, pMe->cmd_type, TRUE, UIM_TK_CMD_PERFORMED_SUCCESSFULLY);
                    }
                    return TRUE;

                case AVK_SELECT:
                    (void)ISHELL_CancelTimer(pMe->m_pShell, UTKApp_DialogTimeout, pMe);
                    pMe->m_bAppIsReady = FALSE;
                    UTK_GiveResponse(pMe, pMe->cmd_type, TRUE, UIM_TK_CMD_PERFORMED_SUCCESSFULLY);
                    return TRUE;
                
                default:
                    break;
            }
            return TRUE;
#ifdef FEATURE_LCD_TOUCH_ENABLE//andrew add for LCD touch
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

				if(TOUCH_PT_IN_RECT(wXPos,wYPos,rc))
				{
					if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
					{
						boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_UTK,EVT_USER,AVK_SELECT,0);
						return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)   && wXPos < rc.x + (rc.dx/3)*2 )//左
					{
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_UTK,EVT_USER,AVK_INFO,0);
						 return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
					{						
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_UTK,EVT_USER,AVK_CLR,0);
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
} // IDD_DISPLAY_Handler

/*==============================================================================
函数：
      IDD_SENDMSG_Handler
      
说明：
       IDD_SENDMSG_DIALOG对话框事件处理函数
       
参数：
       pMe [in]：指向UTK Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean  IDD_SENDMSG_Handler(CUTK *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    PARAM_NOT_REF(wParam)
    static  int nRetrys = 0;
    static  wms_client_message_s_type * pCltMsg = NULL;
    MSG_FATAL("IDD_SENDMSG_Handler Start",0,0,0);
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            return TRUE;
            
        case EVT_DIALOG_START:
            (void) ISHELL_PostEvent( pMe->m_pShell,
                                        AEECLSID_APP_UTK,
                                        EVT_USER_REDRAW,
                                        0,
                                        0);
            return TRUE;
            
        case EVT_USER_REDRAW:
            {
                AECHAR   wszInfo[64]={0};
                boolean bSetTimer = FALSE;
#ifdef FEATURE_ICM
                AEECMSysMode sysmode;
                
                sysmode = ICM_GetSysMode(pMe->m_pCM);
#else
				AEETSSInfo ss_info;
                AEETSysMode sysmode;

                ITELEPHONE_GetServingSystemInfo(pMe->m_pITelephone,&ss_info,sizeof(AEETSSInfo));
                sysmode = ss_info.sys_mode;
#endif
                pCltMsg = NULL;
                nRetrys = 0;
                (void)ISHELL_CancelTimer(pMe->m_pShell, UTKApp_DialogTimeout, pMe);
                (void)UTK_ProcessSendCDMASMSCmd(pMe, &pCltMsg, wszInfo, 64);

                if (WSTRLEN(wszInfo)==0)
                {
                    (void)ISHELL_LoadResString(pMe->m_pShell,
                            UTK_RES_FILE_LANG,
                            IDS_SENDING,
                            wszInfo,
                            sizeof(wszInfo));
                }

                if (NULL == pCltMsg)
                {
                    bSetTimer = TRUE;
#ifdef FEATURE_CARRIER_CHINA_TELCOM
                    UTK_GiveResponse(pMe, pMe->cmd_type, FALSE, UIM_TK_CMD_PERFORMED_SUCCESSFULLY);
                    return TRUE;
#endif
                }
                else
                {
#ifdef FEATURE_ICM
                    if (sysmode == AEECM_SYS_MODE_NO_SRV)
#else
                    if (sysmode == AEET_SYS_MODE_NO_SRV)
#endif
                    {
                        bSetTimer = TRUE;
                        nRetrys = 4;
                    }
                    else
                    {
                        int nRet;
                        MSG_FATAL("UTK: IWMS_MsgSend %d", pMe->m_clientId, 0, 0);
                        nRet = IWMS_MsgSend(pMe->m_pwms, 
                                            pMe->m_clientId, 
                                            &pMe->m_callback,
                                            (void*)pMe,
                                            WMS_SEND_MODE_CLIENT_MESSAGE,
                                            pCltMsg);
                                            
                        if (nRet != SUCCESS)
                        {
                            bSetTimer = TRUE;
                        }
                    }
                }
                        
                (void)IDISPLAY_DrawText(pMe->m_pDisplay,
                            AEE_FONT_BOLD,
                            wszInfo, -1,  0, 0,
                            &pMe->m_rc,
                            IDF_TEXT_TRANSPARENT|IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);
                IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
                
                if (bSetTimer)
                {
                    (void) ISHELL_SetTimer(pMe->m_pShell,
                             2000,
                             UTKApp_DialogTimeout,
                             pMe);
                }
            }

            return TRUE;
            
        case EVT_WMS_CMD_STATUS:
            {
                wmsutk_cmd_status_type  *pStatus = (wmsutk_cmd_status_type *)dwParam;
                
                if (NULL == pStatus)
                {
                    return TRUE;
                }
                
                MSG_FATAL("UTK: EVT_WMS_CMD_STATUS %d %d", pStatus->cmd, pStatus->cmd_err, 0);
                switch (pStatus->cmd)
                {
                    case WMS_CMD_MSG_SEND:
                        if (WMS_CMD_ERR_NONE != pStatus->cmd_err)
                        {// 发送消息失败
                            UTK_GiveResponse(pMe, pMe->cmd_type, FALSE, UIM_TK_NETWORK_CURRENTLY_UNABLE_TO_PROCESS_COMMAND);
                        }
                        break;
                    
                    default:
                        break;
                }
            }
            return TRUE;
            
        case EVT_DIALOG_END:
			//add by yangdecai 2011-08-09
			UTK_GiveResponse(pMe, pMe->cmd_type, FALSE, UIM_TK_NETWORK_CURRENTLY_UNABLE_TO_PROCESS_COMMAND);
            (void)ISHELL_CancelTimer(pMe->m_pShell, UTKApp_DialogTimeout, pMe);
            return TRUE;
            
        case EVT_DIALOGTIMEOUT:
	        nRetrys--;
	        
	        if ((nRetrys <= 0) || (NULL == pCltMsg))
            {
    	        nRetrys = 0;
    	        IALERT_StopSMSAlert(pMe->m_pAlert);
                IALERT_StopMp3Alert(pMe->m_pAlert);
                UTKApp_NotifyMp3Player(pMe,FALSE);
                UTK_GiveResponse(pMe, pMe->cmd_type, FALSE, UIM_TK_NETWORK_CURRENTLY_UNABLE_TO_PROCESS_COMMAND);

                return TRUE;
            }
            
            {
                boolean bSetTimer = FALSE;
#ifdef FEATURE_ICM
                AEECMSysMode sysmode;
                int nRet;
                
                sysmode = ICM_GetSysMode(pMe->m_pCM);
            
                if (sysmode == AEECM_SYS_MODE_NO_SRV)
#else
				int nRet;
                AEETSSInfo ss_info;
                AEETSysMode sysmode;

                ITELEPHONE_GetServingSystemInfo(pMe->m_pITelephone,&ss_info,sizeof(AEETSSInfo));
                sysmode = ss_info.sys_mode;
            
                if (sysmode == AEET_SYS_MODE_NO_SRV)
#endif
                {
                    bSetTimer = TRUE;
                }
                else
                {
                    nRet = IWMS_MsgSend(pMe->m_pwms, 
                                        pMe->m_clientId, 
                                        &pMe->m_callback,
                                        (void*)pMe,
                                        WMS_SEND_MODE_CLIENT_MESSAGE,
                                        pCltMsg);
                                        
                    if (nRet != SUCCESS)
                    {
                        bSetTimer = TRUE;
                    }
                }
                
                if (bSetTimer)
                {
                    (void) ISHELL_SetTimer(pMe->m_pShell,
                             2000,
                             UTKApp_DialogTimeout,
                             pMe);
                }
            }
            
            return TRUE;
            
        case EVT_SEND_STATUS:
            pMe->m_bAppIsReady = FALSE;
            if (pMe->m_SendStatus == WMS_RPT_OK)
            {
                UTKApp_NotifyMp3Player(pMe,TRUE);
                UTK_MessageAlert(pMe, TRUE);
                (void)ISHELL_SetTimer(pMe->m_pShell, 2000, (PFNNOTIFY)UTKApp_NotifyMp3PlayerCB, pMe);
                UTK_GiveResponse(pMe, pMe->cmd_type, FALSE, UIM_TK_CMD_PERFORMED_SUCCESSFULLY);
            }
            else
            {
                UTK_GiveResponse(pMe, pMe->cmd_type, FALSE, UIM_TK_NETWORK_CURRENTLY_UNABLE_TO_PROCESS_COMMAND);
            }
            return TRUE;
            
        case EVT_KEY:
			{
				//add by yangdecai 2011-08-09
				switch(wParam)
				{
					case AVK_CLR:
						UTK_GiveResponse(pMe, pMe->cmd_type, FALSE, UIM_TK_NETWORK_CURRENTLY_UNABLE_TO_PROCESS_COMMAND);
						break;
					default:
						break;
				}
			}
            return TRUE;
#ifdef FEATURE_LCD_TOUCH_ENABLE//andrew add for LCD touch
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
	
					if(TOUCH_PT_IN_RECT(wXPos,wYPos,rc))
					{
						if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
						{
							boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_UTK,EVT_USER,AVK_SELECT,0);
							return rt;
						}
						else if(wXPos >= rc.x + (rc.dx/3)	&& wXPos < rc.x + (rc.dx/3)*2 )//左
						{
							 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_UTK,EVT_USER,AVK_INFO,0);
							 return rt;
						}
						else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
						{						
							 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_UTK,EVT_USER,AVK_CLR,0);
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
} // IDD_SENDMSG_Handler

/*==============================================================================
函数：
       IDD_PLAYTONE_Handler
说明：
       IDD_PLAYTONE_Handler对话框事件处理函数
       
参数：
       pMe [in]：指向UTK Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean  IDD_PLAYTONE_Handler(CUTK *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    IStatic   *pStatic;             
    static boolean bAlertStop = FALSE;
    PARAM_NOT_REF(wParam)
    PARAM_NOT_REF(dwParam)
    MSG_FATAL("IDD_PLAYTONE_Handler Start",0,0,0);        
    if ((NULL == pMe) || (NULL == pMe->m_pAlert))
    {
        return FALSE;
    }         	                     	
    pStatic = (IStatic *)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_PLAYTONE_STATIC);
    if (pStatic == NULL)
    {
        return FALSE;
    }
    
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            return TRUE;
            
        case EVT_DIALOG_START:
            {
                Play_Tone *pPlayTone=NULL;
                byte      *utk_ptr;
                uint32    tonetimeout; 
                AECHAR    wszTongMsg[64]={0};     
                AEERect   rc;
                
                pPlayTone = (Play_Tone *)MALLOC(sizeof(Play_Tone));
                if (NULL == pPlayTone)
                {
                    return FALSE;
                }       
            
                utk_ptr = UTK_GetCmddata(pMe->cmd_type);
                if(NULL == utk_ptr)
                {
                    FREEIF(pPlayTone);
                    return FALSE;
                }      
                DecodePlayToneData(utk_ptr, pPlayTone);     
                        	
                //set playtone tone                        
                pMe->m_PlaytoneToneID = pPlayTone->tone.tone;
                UTKApp_NotifyMp3Player(pMe, TRUE);
                UTKApp_PlaytoneTimeout(pMe);   
                bAlertStop = FALSE;
                if((0x00 != pPlayTone->alpha_id.length) &&(NULL != pPlayTone->alpha_id.data))
                {
                   (void)WSTRCPY(wszTongMsg, pPlayTone->alpha_id.data);  
                }                
                if(NULL == pPlayTone->alpha_id.alpha_id_id)
                {
                   (void)ISHELL_LoadResString(pMe->m_pShell,
                                UTK_RES_FILE_LANG,
                                pMe->m_PlaytoneMsgID,
                                wszTongMsg,
                                sizeof(wszTongMsg));        	
                } 
                  
                //draw playtone string                           
                rc = pMe->m_rc;
                rc.dy = rc.dy - HEIGHT_PROMPT_BAR;
                ISTATIC_SetRect(pStatic, &rc);                                          
                ISTATIC_SetProperties( pStatic,  ST_CENTERTEXT 
                                               |ST_MIDDLETEXT
                                               |ST_UNDERLINE
                                               |ST_CENTERTITLE);               
                (void)ISTATIC_SetText( pStatic,
                                      NULL,
                                      wszTongMsg,
                                      AEE_FONT_NORMAL,
                                      AEE_FONT_NORMAL);                                    
                                    
                //set playtone timer       
                if(NULL != pPlayTone->duration.time_interval)
                {
                    switch (pPlayTone->duration.time_unit)
                    {
                        case UIM_TK_MINUTES: //minutes
                            tonetimeout = 60*1000*(pPlayTone->duration.time_interval);                  
                            break; 
                        
                        case UIM_TK_SECONDS: //seconds
                            tonetimeout = 1*1000*(pPlayTone->duration.time_interval);              
                            break;  
                          
                        case UIM_TK_TENTHS_SECS: //seconds/10
                            tonetimeout = (1000/10)*(pPlayTone->duration.time_interval);              
                            break;   
                        
                        default:
                            tonetimeout = TIMEOUT_MS_MSGBOX;              
                            break;                                    
                    }
                }
                else
                {
                    tonetimeout = TIMEOUT_MS_MSGBOX;       	
                }                                         
                (void) ISHELL_SetTimer(pMe->m_pShell,
                                tonetimeout,
                                UTKApp_DialogTimeout,
                                pMe);  
                
                FREEIF(pPlayTone);  
                                               
               (void) ISHELL_PostEvent( pMe->m_pShell,
                                           AEECLSID_APP_UTK,
                                           EVT_USER_REDRAW,
                                           0,
                                           0);
            }
            return TRUE;
           
        case EVT_USER_REDRAW:
            IDISPLAY_Update(pMe->m_pDisplay);  
            return TRUE;
            
        case EVT_DIALOG_END:
            OEMSOUND_Sound_Stop(SND_PRIO_MED);
            IALERT_StopRingerAlert(pMe->m_pAlert);
            IALERT_StopSMSAlert(pMe->m_pAlert); 
            IALERT_StopMp3Alert(pMe->m_pAlert);
            if(!bAlertStop)
            {
                UTKApp_NotifyMp3Player(pMe, FALSE);
                bAlertStop = TRUE;
            }
            (void) ISHELL_CancelTimer( pMe->m_pShell,
                                       UTKApp_PlaytoneTimeout,
                                       pMe );                          
            (void) ISHELL_CancelTimer( pMe->m_pShell,
                                       UTKApp_DialogTimeout,
                                       pMe );
            return TRUE;
            
        case EVT_KEY:
            switch (wParam)
            {
                case AVK_INFO:
                case AVK_CLR:
                case AVK_SELECT:
                    OEMSOUND_Sound_Stop(SND_PRIO_MED);
                    IALERT_StopRingerAlert(pMe->m_pAlert);
                    IALERT_StopSMSAlert(pMe->m_pAlert);
                    IALERT_StopMp3Alert(pMe->m_pAlert);
                    if(!bAlertStop)
                    {
                        UTKApp_NotifyMp3Player(pMe, FALSE);
                        bAlertStop = TRUE;
                    }
                    (void) ISHELL_CancelTimer( pMe->m_pShell,
                                       UTKApp_PlaytoneTimeout,
                                       pMe );                      
                    UTK_GiveResponse(pMe, pMe->cmd_type, FALSE, UIM_TK_PROACTIVE_RUIM_SESSION_TERMINATED_BY_USER); 
                    return TRUE;
                    
                default:
                    break;
                    
            }
            return TRUE;
        
        case EVT_COMMAND:
            break;
            
        case EVT_DIALOGTIMEOUT:  
            OEMSOUND_Sound_Stop(SND_PRIO_MED);
            IALERT_StopRingerAlert(pMe->m_pAlert);
            IALERT_StopSMSAlert(pMe->m_pAlert); 
            IALERT_StopMp3Alert(pMe->m_pAlert);
            if(!bAlertStop)
            {
                UTKApp_NotifyMp3Player(pMe, FALSE);
                bAlertStop = TRUE;
            }
            (void) ISHELL_CancelTimer( pMe->m_pShell,
                                       UTKApp_PlaytoneTimeout,
                                       pMe );            
            UTK_GiveResponse(pMe, pMe->cmd_type, FALSE, UIM_TK_CMD_PERFORMED_SUCCESSFULLY);
            return TRUE;         
#ifdef FEATURE_LCD_TOUCH_ENABLE//andrew add for LCD touch
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

				if(TOUCH_PT_IN_RECT(wXPos,wYPos,rc))
				{
					if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
					{
						boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_UTK,EVT_USER,AVK_SELECT,0);
						return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)   && wXPos < rc.x + (rc.dx/3)*2 )//左
					{
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_UTK,EVT_USER,AVK_INFO,0);
						 return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
					{						
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_UTK,EVT_USER,AVK_CLR,0);
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
} // IDD_PLAYTONE_Handler

/*==============================================================================
函数：
       IDD_MSGBOX_Handler
说明：
       IDD_MSGBOX 对话框事件处理函数
       
参数：
       pMe [in]：指向CUTK Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean  IDD_MSGBOX_Handler( CUTK *pMe,
                                    AEEEvent   eCode,
                                    uint16     wParam,
                                    uint32     dwParam)
{
    IStatic *pStatic;
    AECHAR  msg[64];     
        
#if defined(AEE_STATIC)
    ASSERT(pMe != NULL);
#endif
    MSG_FATAL("IDD_MSGBOX_Handler Start",0,0,0);  
    pStatic = (IStatic*)IDIALOG_GetControl( pMe->m_pActiveDlg, IDC_MSGBOX_STATIC);
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
           {
               AEERect rc;   
               rc = pMe->m_rc;
               rc.dy = rc.dy - HEIGHT_PROMPT_BAR;
               ISTATIC_SetRect(pStatic, &rc);                              
               ISTATIC_SetProperties( pStatic,  ST_CENTERTEXT 
                                               |ST_MIDDLETEXT
                                               |ST_UNDERLINE
                                               |ST_CENTERTITLE);      
              (void)ISHELL_LoadResString(pMe->m_pShell,
                                        UTK_RES_FILE_LANG,
                                        pMe->m_wMsgID,
                                        msg,
                                        sizeof(msg));                 
               (void)ISTATIC_SetText( pStatic,
                                      NULL,
                                      msg,
                                      AEE_FONT_NORMAL,
                                      AEE_FONT_NORMAL);
           }        
            return TRUE;
        
        case EVT_DIALOG_START:        
            (void) ISHELL_SetTimer( pMe->m_pShell,
                                    pMe->m_MsgTimeout,
                                    UTKApp_DialogTimeout,
                                    pMe );                                                                                              
            // For redraw the dialog
            (void)ISHELL_PostEvent( pMe->m_pShell,
                                    AEECLSID_APP_UTK,
                                    EVT_USER_REDRAW,
                                    0,
                                    0); 
            return TRUE;                                                                       
            
        case EVT_USER_REDRAW:
            IDISPLAY_Update(pMe->m_pDisplay);  
            return TRUE;
            
        case EVT_DIALOG_END:
            (void) ISHELL_CancelTimer( pMe->m_pShell,
                                       UTKApp_DialogTimeout,
                                       pMe );
            return TRUE;
            
        case EVT_KEY:
            switch (wParam)
            {
                case AVK_INFO:
                case AVK_CLR:
                case AVK_SELECT:
                    if (pMe->cmd_type == UIM_TK_PLAY_TONE)
                    {
                        return TRUE;                	
                    }
                    // 如下代码限制在此界面的快速按键
                    (void) ISHELL_SetTimer( pMe->m_pShell,
                                            TIMEOUT_MS_QUICK,
                                            UTKApp_DialogTimeout,
                                            pMe );
                    return TRUE;
                    
                default:
                    break;                    
            }
            return TRUE;
        
        case EVT_COMMAND:
            break;
            
        case EVT_DIALOGTIMEOUT:  
            CLOSE_DIALOG(DLGRET_MSGBOX_OK);
            return TRUE;         
#ifdef FEATURE_LCD_TOUCH_ENABLE//andrew add for LCD touch
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

				if(TOUCH_PT_IN_RECT(wXPos,wYPos,rc))
				{
					if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
					{
						boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_UTK,EVT_USER,AVK_SELECT,0);
						return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)   && wXPos < rc.x + (rc.dx/3)*2 )//左
					{
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_UTK,EVT_USER,AVK_INFO,0);
						 return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
					{						
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_UTK,EVT_USER,AVK_CLR,0);
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
} // IDD_MSGBOX_Handler

/*==============================================================================
函数：
       IDD_POPMSG_Handler
说明：
       IDD_POPMSG 对话框事件处理函数
       
参数：
       pMe [in]：指向CUTK Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean  IDD_POPMSG_Handler( CUTK *pMe,
                                    AEEEvent   eCode,
                                    uint16     wParam,
                                    uint32     dwParam)
{
    static IStatic * pStatic = NULL;       
    
#if defined(AEE_STATIC)
    ASSERT(pMe != NULL);
#endif
    MSG_FATAL("IDD_POPMSG_Handler Start",0,0,0); 
    if (NULL == pStatic)
    {
         AEERect rect = {0};
         if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                                  AEECLSID_STATIC,
                                                  (void **)&pStatic))
         
         {
             return FALSE;
             MSG_FATAL("ISHELL_CreateInstance,AEECLSID_STATIC 2",0,0,0);
         }        
         ISTATIC_SetRect(pStatic, &rect);  
    }

    switch (eCode)
    {
        case EVT_DIALOG_INIT: 
            return TRUE;
        
        case EVT_DIALOG_START:     
            (void) ISHELL_SetTimer( pMe->m_pShell,
                            pMe->m_MsgTimeout,
                            UTKApp_DialogTimeout,
                            pMe );                                                                                              
            // For redraw the dialog
            (void)ISHELL_PostEvent( pMe->m_pShell,
                                    AEECLSID_APP_UTK,
                                    EVT_USER_REDRAW,
                                    0,
                                    0); 
            return TRUE;                                                                       
            
        case EVT_USER_REDRAW:
            {
                PromptMsg_Param_type PromptMsg={0};  
                AECHAR  wstrText[128];  
                
                (void)ISHELL_LoadResString(pMe->m_pShell,
                                        UTK_RES_FILE_LANG,
                                        pMe->m_wMsgID,
                                        wstrText,
                                        sizeof(wstrText)); 
                PromptMsg.ePMsgType = MESSAGE_INFORMATIVE;
                PromptMsg.pwszMsg = wstrText;
                DrawPromptMessage(pMe->m_pDisplay, pStatic, &PromptMsg);                         
            }          
            IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE);
            return TRUE;
            
        case EVT_DIALOG_END:
             ISTATIC_Release(pStatic); 
             pStatic = NULL;                 
            (void) ISHELL_CancelTimer( pMe->m_pShell,
                                       UTKApp_DialogTimeout,
                                       pMe );
            return TRUE;
            
        case EVT_KEY:
            switch (wParam)
            {
                case AVK_INFO:
                case AVK_CLR:
                case AVK_SELECT:
                    // 如下代码限制在此界面的快速按键
                    (void) ISHELL_SetTimer( pMe->m_pShell,
                                            TIMEOUT_MS_QUICK,
                                            UTKApp_DialogTimeout,
                                            pMe );
                    return TRUE;
                    
                default:
                    break;                    
            }
            return TRUE;

        
        case EVT_COMMAND:
            break;
            
        case EVT_DIALOGTIMEOUT:  
            CLOSE_DIALOG(DLGRET_MSGBOX_OK);
            return TRUE;         
#ifdef FEATURE_LCD_TOUCH_ENABLE//andrew add for LCD touch
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

				if(TOUCH_PT_IN_RECT(wXPos,wYPos,rc))
				{
					if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
					{
						boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_UTK,EVT_USER,AVK_SELECT,0);
						return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)   && wXPos < rc.x + (rc.dx/3)*2 )//左
					{
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_UTK,EVT_USER,AVK_INFO,0);
						 return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
					{						
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_UTK,EVT_USER,AVK_CLR,0);
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
} // IDD_POPMSG_Handler


/*==============================================================================
函数: 
       UTKApp_PlaytoneTimeout
       
说明: 
       定时器回调函数。主要用于自动关闭消息对话框。
       
参数: 
       pme [in]：void *类型指针，暗指CUTK结构指针。
       
返回值:
       无。
       
备注:
       
==============================================================================*/
static void UTKApp_PlaytoneTimeout(void *pme)
{
//    byte      signal_type;
//    byte      signal;  	
    byte      ringtype = AEEALERT_ALERT_LOW_BATTERY;
    uint32    timeout;                 

    CUTK *pMe = (CUTK *)pme;
    MSG_FATAL("UTKApp_PlaytoneTimeout Start",0,0,0); 
    if (NULL == pMe)
    {
        return;
    }
    
    //set playtone tone
    switch (pMe->m_PlaytoneToneID)
    {        
        //Standard supervisory tones	                                                                 
        case UIM_TK_DIAL_TONE: 
            //Dial tone    
            pMe->m_PlaytoneMsgID = IDS_DIAL_TONE_STRING;
          //  if(0 == signal)
          //  {
          //      signal = CAI_TONE_DIAL;
          //  }
            ringtype = AEEALERT_ALERT_DIALTONE;
            break;
          
        case UIM_TK_CALLED_SUBSCRIBER_BUSY: 
            //Called subscriber busy    
            pMe->m_PlaytoneMsgID = IDS_CALLED_SUBSCRIBER_BUSY_STRING;
           /* if(0 == signal)
            {
                signal = CAI_TONE_BUSY;
            }*/
            //IALERT_StartAlerting(pMe->m_pAlert, NULL, NULL, AEEALERT_ALERT_BUSY);
            ringtype = AEEALERT_ALERT_BUSY;
            break;              
          
        case UIM_TK_CONGESTION: 
            //Congestion 
            pMe->m_PlaytoneMsgID = IDS_CONGESTION_STRING;
            /*
            if(0 == signal)
            {
            	signal = CAI_TONE_REORDER;
            }*/
            //IALERT_StartAlerting(pMe->m_pAlert, NULL, NULL, AEEALERT_ALERT_REORDER);
            ringtype = AEEALERT_ALERT_REORDER;
            break;
          
        case UIM_TK_RADIO_PATH_ACKNOWLEDGE: 
            //Radio path acknowledge  
            pMe->m_PlaytoneMsgID = IDS_RADIO_PATH_ACKNOWLEDGE_STRING;
            /*if(0 == signal)
            {
            	signal = CAI_TONE_CONFIRM;
            }*/
            //IALERT_StartAlerting(pMe->m_pAlert, NULL, NULL, AEEALERT_ALERT_CONFIRM);
            ringtype = AEEALERT_ALERT_CONFIRM;
            break; 
          
        case UIM_TK_RADIO_PATH_NOT_AVAILABLE_OR_CALL_DROPPED: 
            //Radio path not available/Call dropped
            pMe->m_PlaytoneMsgID = IDS_RADIO_PATH_NOT_AVAILABLE_OR_CALL_DROPPED_STRING;
            /*if(0 == signal)
            {
            	signal = CAI_TONE_ABBR_REORDER;
            } */
            //IALERT_StartAlerting(pMe->m_pAlert, NULL, NULL, AEEALERT_ALERT_ABBR_REORDER);
            ringtype = AEEALERT_ALERT_ABBR_REORDER;
            break; 
          
        case UIM_TK_ERROR_OR_SPECIAL_INFORMATION: 
            //Error/Special information 
            pMe->m_PlaytoneMsgID = IDS_ERROR_OR_SPECIAL_INFORMATION_STRING;
            /*if(0 == signal)
            {
            	 signal = CAI_TONE_INTERCEPT;
            }*/
            //IALERT_StartAlerting(pMe->m_pAlert, NULL, NULL, AEEALERT_ALERT_INTERCEPT);
            ringtype = AEEALERT_ALERT_INTERCEPT;
            break;
          
        case UIM_TK_CALL_WAITING: 
            //Call waiting tone  
            pMe->m_PlaytoneMsgID = IDS_CALL_WAITING_STRING;
           /* if(0 == signal)
            {
            	signal = CAI_TONE_CALLWAIT;
            }*/
            //IALERT_StartAlerting(pMe->m_pAlert, NULL, NULL, AEEALERT_ALERT_CALLWAITING);
            ringtype = AEEALERT_ALERT_CALLWAIT_SIGNAL;
            break;
          
        case UIM_TK_RINGING: 
            //Ringing tone  
            pMe->m_PlaytoneMsgID = IDS_RINGING_STRING;
           /* if(0 == signal)
            {
            	signal = CAI_TONE_RING;
            } */
            //IALERT_StartAlerting(pMe->m_pAlert, NULL, NULL, AEEALERT_ALERT_RINGBACK);
            ringtype = AEEALERT_ALERT_RINGBACK;
            break;     
                 
        //Terminal proprietary tones:       
        case UIM_TK_GENERAL_BEEP: 
            //General beep
            pMe->m_PlaytoneMsgID = IDS_GENERAL_BEEP_STRING; 
            break;
          
        case UIM_TK_POSITIVE_ACKNOWLEDGEMENT:
            //Positive acknowledgement tone
            pMe->m_PlaytoneMsgID = IDS_POSITIVE_ACKNOWLEDGEMENT_STRING;
            /*if(0 == signal)
            {
            	signal = CAI_TONE_CONFIRM;
            } */
            //IALERT_StartAlerting(pMe->m_pAlert, NULL, NULL, AEEALERT_ALERT_CONFIRM);
            ringtype = AEEALERT_ALERT_CONFIRM;
            break;
          
        case UIM_TK_NEGATIVE_ACKNOWLEDGEMENT_OR_ERROR: 
            //Negative acknowledgement or error tone
            pMe->m_PlaytoneMsgID = IDS_NEGATIVE_ACKNOWLEDGEMENT_OR_ERROR_STRING; 
            break;
          
        case UIM_TK_RINGING_TONE_SEL_BY_USER_FOR_IC_V_CALL: 
            //Ringing tone as selected by the user for incoming speech call 
            pMe->m_PlaytoneMsgID = IDS_RINGING_TONE_SEL_BY_USER_FOR_IC_V_CALL_STRING;                           	
            break;
          
        case UIM_TK_ALERT_TONE_SEL_BY_USER_FOR_IC_SMS: 
            //Alert tone as selected by the user for incoming SMS
            pMe->m_PlaytoneMsgID = IDS_ALERT_TONE_SEL_BY_USER_FOR_IC_SMS_STRING; 
            break;                    
                          
        default:
            pMe->m_PlaytoneMsgID = 0; 
            break;              
    }                    
    // 开始放音
    switch (pMe->m_PlaytoneMsgID)
    {
        case 0:
            timeout = 500;
            //IALERT_LowBattAlert(pMe->m_pAlert);
            IALERT_StartAlerting(pMe->m_pAlert, NULL, NULL, AEEALERT_ALERT_LOW_BATTERY);
            break;
            
        case IDS_ALERT_TONE_SEL_BY_USER_FOR_IC_SMS_STRING:
            timeout = 500;                
            UTK_MessageAlert(pMe, TRUE);
            break;
            
        case IDS_RINGING_TONE_SEL_BY_USER_FOR_IC_V_CALL_STRING:
		{
			byte        midID = OEMNV_DEFAULTRINGER;
            byte        profilenum = 0;
            ringID      ringid[PROFILENUMBER];
            
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
                    IALERT_StartRingerAlert(pMe->m_pAlert, (uint32)midID);
                }
            }
            timeout   = 12000;  
		}
            break;
            
        case IDS_NEGATIVE_ACKNOWLEDGEMENT_OR_ERROR_STRING:
            timeout = 1000;           
            IALERT_StartAlerting(pMe->m_pAlert, NULL, NULL, AEEALERT_ALERT_ERR_SPECIAL);
            //IALERT_ErrOrSpecialAlert(pMe->m_pAlert);
            break;
            
        case IDS_GENERAL_BEEP_STRING:
            timeout = 500;        
            IALERT_StartAlerting(pMe->m_pAlert, NULL, NULL, AEEALERT_ALERT_LOW_BATTERY); 
            break;
            
        default:
            timeout = 1000;                     
            IALERT_StartAlerting(pMe->m_pAlert, NULL, NULL, ringtype);	            
            break;
    }  
    (void) ISHELL_SetTimer(pMe->m_pShell,
                  timeout,
                  UTKApp_PlaytoneTimeout,
                  pMe);             
}

/*==============================================================================
函数: 
       UTKApp_DialogTimeout
       
说明: 
       定时器回调函数。主要用于自动关闭消息对话框。
       
参数: 
       pme [in]：void *类型指针，暗指CUTK结构指针。
       
返回值:
       无。
       
备注:
       
==============================================================================*/
static void UTKApp_DialogTimeout(void *pme)
{
    CUTK *pMe = (CUTK *)pme;
    MSG_FATAL("UTKApp_DialogTimeout Start",0,0,0); 
    if (NULL == pMe)
    {
        return;
    }

    (void) ISHELL_PostEvent( pMe->m_pShell,
                            AEECLSID_APP_UTK,
                            EVT_DIALOGTIMEOUT,
                            0,
                            0);
    MSG_FATAL("UTKApp_DialogTimeout End",0,0,0); 
}

/*==============================================================================
函数: 
       UTKApp_DialogRedrawTimeout
       
说明: 
       定时器回调函数。主要用于自动关闭消息对话框。
       
参数: 
       pme [in]：void *类型指针，暗指CUTK结构指针。
       
返回值:
       无。
       
备注:
       
==============================================================================*/
static void UTKApp_DialogRedrawTimeout(void *pme)
{
    CUTK *pMe = (CUTK *)pme;
    MSG_FATAL("UTKApp_DialogRedrawTimeout Start",0,0,0); 
    if (NULL == pMe)
    {
        return;
    }

    (void) ISHELL_PostEvent( pMe->m_pShell,
                            AEECLSID_APP_UTK,
                            EVT_USER_REDRAW,
                            1,
                            0);
    MSG_FATAL("UTKApp_DialogRedrawTimeout End",0,0,0); 
}

/*==============================================================================
函数：
       SetMenuLook
说明：
       用于设定给定菜单控件的矩形和外观，并向程序发出EVT_USER_REDRAW事件。
       
参数：
       pMe [in]：指向 CUTK Applet对象结构的指针。该结构包含小程序的
                       特定信息。
       pMenu {in]：IMenuCtl控件指针。
       
返回值：
       无。
       
备注：
       
==============================================================================*/
static void SetMenuLook( CUTK *pMe, IMenuCtl *pMenu)
{
    AEERect rc;
    MSG_FATAL("SetMenuLook Start",0,0,0); 
    if ((NULL == pMe) || (NULL == pMenu))
    {
        return;
    }

    IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL);
    IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
    SETAEERECT(&rc, 0, 0, pMe->m_rc.dx, pMe->m_rc.dy);
    IMENUCTL_SetRect(pMenu, &rc);
    MSG_FATAL("SetMenuLook End",0,0,0); 
} // SetMenuLook
static void UTKApp_NotifyMp3Player(CUTK *pMe,boolean toStartAlert)
{
#ifdef FEATURE_APP_MUSICPLAYER
    MSG_FATAL("UTKApp_NotifyMp3Player Start",0,0,0); 
    if(IsMp3PlayerStatusOnBG())
    {
        ISHELL_SendEvent(pMe->m_pShell, 
                         AEECLSID_APP_MUSICPLAYER,
                         EVT_ALARM,
                         toStartAlert,
                         0);
    }
    MSG_FATAL("UTKApp_NotifyMp3Player End",0,0,0); 
#endif
}
static void UTKApp_NotifyMp3PlayerCB(CUTK *pMe)
{
    MSG_FATAL("UTKApp_NotifyMp3PlayerCB Start",0,0,0); 
    UTKApp_NotifyMp3Player(pMe,FALSE);
    MSG_FATAL("UTKApp_NotifyMp3PlayerCB End",0,0,0); 
}

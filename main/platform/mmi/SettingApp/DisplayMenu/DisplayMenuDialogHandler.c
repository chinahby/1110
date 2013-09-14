/*==============================================================================
// 文件：
//        DisplayMenuDialog.c
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
#include "DisplayMenu_priv.h"
#ifndef WIN32
#include "Disp.h"
#endif//WIN32
#include "AEEMenu.h"
#ifdef FEATURE_APP_MEDIAGALLERY
#include "MediaGallery.h"
#endif
#include "AEEAnnunciator.h"

/*==============================================================================
                                 宏定义和常数
==============================================================================*/
// 关闭对话框宏
#define CLOSE_DIALOG(DlgRet)  {    \
                                     pMe->m_eDlgRet = DlgRet; \
                                     (void) ISHELL_EndDialog(pMe->m_pShell);  \
                                 }


#define DISPLAYMENU_COLOR_BLUE        (MAKE_RGB(0x0A, 0x05, 0x35))
#define DISPLAYMENU_COLOR_COFFEE    (MAKE_RGB(0x50, 0x20, 0x00))
#define DISPLAYMENU_COLOR_GREEN      (MAKE_RGB(0x00, 0x30, 0x00))
#define DISPLAYMENU_COLOR_LEMON      (MAKE_RGB(0x6E, 0x78, 0x00))
#define DISPLAYMENU_COLOR_PURPLE     (MAKE_RGB(0x20, 0x00, 0x10))
/*==============================================================================
                                 类型定义
==============================================================================*/


/*==============================================================================
                                 函数声明
==============================================================================*/
// 对话框 IDD_DISPLAYMENU_MENU 事件处理函数
static boolean  HandleMainDialogEvent(CDisplayMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

// 对话框 IDD_PICTURE_DLG 事件处理函数
static boolean  HandlePictureDialogEvent(CDisplayMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

// 对话框 IDD_IMGDELMSG_DLG 事件处理函数
static boolean  HandleImgDelMsgDialogEvent(CDisplayMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

// 对话框 IDD_BACKLIGHTING_MENU 事件处理函数
static boolean  HandleBacklightingDialogEvent(CDisplayMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

// 对话框 IDD_CONTRAST_MENU 事件处理函数
static boolean  HandleContrastDialogEvent(CDisplayMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);
#ifdef FEATURE_SCREEN_SAVE
// 对话框 IDD_SCREENSAVERTIMEOUT_DIALOG 事件处理函数
static boolean  HandleScreensaverTimeoutDialogEvent(CDisplayMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);
#endif
static boolean  HandleDesktop_Theme_DialogEvent(CDisplayMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

#ifdef FEATURE_MENU_STYLE
static boolean HandleMenuStyleDialogEvent(CDisplayMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);
#endif
#ifdef FEATURE_RANDOM_MENU_COLOR
static boolean  HandleColorfulMenuDialogEvent(CDisplayMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

static boolean  HandleColorSettingDialogEvent(CDisplayMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);
#endif
#ifdef FEATURE_RANDOM_MENU_REND//wlh 20090405 add for REND
static boolean  HandleRENDMenuDialogEvent(CDisplayMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);
#endif

// 分类枚举出所有的墙纸或动画
static void DisplayMenu_EnumFile(CDisplayMenu *pMe,
    PICTURE_TYPE PICType
);

// 分类显示所有的墙纸或动画
static void DisplayMenu_DisplayImg(CDisplayMenu *pMe,
    PICTURE_TYPE PICType
);

// 浏览图片时限制按键速度
static void DisplayMenu_NextImgIsReadyTimer(void *pme);

// 确定图片的类型
ImageType DisplayMenu_GetImageFileType(char * pszFileName);
static void DisplayMenu_AutoScroll(IMenuCtl  *pMenu, AEEEvent eCode,   uint16 wParam);

static void DisplayMenu_DrawTextWithProfile(IShell* pShell,
                                        IDisplay * pDisplay,
                                        RGBVAL ProfileColor,
                                        AEEFont Font,
                                        const AECHAR * pcText,
                                        int nChars,
                                        int x,
                                        int y,
                                        const AEERect * prcBackground,
                                        uint32 dwFlags
                                        );

static double_list * DisplayMenu_Creat_Double_list(void);

static void DisplayMenu_Free_Double_list(double_list * plist );

static void DisplayMenu_Add_Node_To_Double_list_Tail(double_list * plist ,
                                        char * image_name);

static image_name_table * DisplayMenu_Search_Name_In_Double_list(double_list * plist ,
                                        char * image_name,
                                        uint16 * count);

static void DisplayMenu_Del_A_Image_In_Double_list(double_list * plist,
                                        image_name_table * image_table);

static image_name_table * DisplayMenu_Search_Next_In_Double_list(double_list * plist ,
                                        image_name_table * p_cur,uint16 * img_id);

static image_name_table * DisplayMenu_Search_Pref_In_Double_list(double_list * plist ,
                                        image_name_table * p_cur,uint16 * img_id);

#ifdef FEATURE_LED_CONTROL
static boolean Handle_LED_Control_DialogEvent(CDisplayMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);
#endif
#ifdef FEATRUE_KEY_PAD_CTL
static boolean  DisplayMenu_Handle_Key_Pad_Ctl_DialogEvent(CDisplayMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);
static boolean Display_ClockApps_HandleKeyEvent(CDisplayMenu *pMe, uint16 wParam);
static void DisplayMenu_Set_CTL(CDisplayMenu *pMe);
#endif

//static void DisplayMenu_Draw_Softkey(CDisplayMenu *pMe,uint16 lsk,uint16 rsk);

static void DisplayMenu_DialogTimeout(void *pme);

static byte DisplayMenu_Cover_Ids_To_Theme(uint16 ids);
/*==============================================================================
                                 全局数据
==============================================================================*/
static double_list *p_local_wallpaper = NULL;/*Local wallpaper*/
static double_list *p_download_wallpaper = NULL;/*download wallpaper*/
#ifdef FEATURE_ANIMATION_POWERUPDOWN
static double_list *p_powerup_image = NULL;
static double_list *p_poweroff_image = NULL;
#endif//FEATURE_ANIMATION_POWERUPDOWN

#ifdef FEATURE_SCREEN_SAVE
static double_list *p_screensave_image = NULL;
#endif /*FEATURE_SCERRN_SAVE*/
/*==============================================================================
                                 本地（静态）数据
==============================================================================*/

/*==============================================================================
                                 函数定义
==============================================================================*/
/*==============================================================================
函数:
       DisplayMenu_ShowDialog
说明:
       函数由状态处理函数用来创建相应对话框资源ID的对话框。

参数:
       pMe [In]: 指向DisplayMenu Applet对象结构的指针,该结构包含小程序的特定信息.
       dlgResId [in]：对话框资源ID，函数根据该ID创建对话框。

返回值:
       无

备注:


==============================================================================*/
void DisplayMenu_ShowDialog(CDisplayMenu *pMe,uint16  dlgResId)
{
    int nRet;
    MSG_FATAL("DisplayMenu_ShowDialog Start",0,0,0);
    if (ISHELL_GetActiveDialog(pMe->m_pShell) != NULL)
    {
        // Looks like there is one dialog already opened.
        // Flag an error an return without doing anything.
        MSG_FATAL("Trying to create a dialog without closing the previous one",0,0,0);
        return;
    }
    if (NULL != pMe->m_pDisplay)
    {
        AEEDeviceInfo di={0,};
        if (dlgResId == IDD_PICTURE_DLG)
        {
            MSG_FATAL("DisplayMenu_ShowDialog dlgResId == IDD_PICTURE_DLG",0,0,0);
            (void)IDISPLAY_SetPrefs(pMe->m_pDisplay, "a:0", STRLEN("a:0"));
        }
        else
        {
            MSG_FATAL("DisplayMenu_ShowDialog dlgResId != IDD_PICTURE_DLG",0,0,0);
            (void)IDISPLAY_SetPrefs(pMe->m_pDisplay, "a:1", STRLEN("a:1"));
        }
        ISHELL_GetDeviceInfo(pMe->m_pShell, &di);
        pMe->m_rc.dx = di.cxScreen;
        pMe->m_rc.dy = di.cyScreen;
        IDISPLAY_SetClipRect(pMe->m_pDisplay, &pMe->m_rc);
    }

    nRet = ISHELL_CreateDialog(pMe->m_pShell,AEE_APPSDISPLAYMENU_RES_FILE,dlgResId,NULL);

    if (nRet != SUCCESS)
    {
        MSG_FATAL("Failed to create the dialog in the DisplayMenu applet",0,0,0);
    }
    MSG_FATAL("DisplayMenu_ShowDialog End",0,0,0);
}

/*==============================================================================
函数:
       DisplayMenu_RouteDialogEvent
说明:
       函数将BREW事件路由给当前活动对话框的事件处理函数。

参数:
       pMe [In]: 指向DisplayMenu Applet对象结构的指针,该结构包含小程序的特定信息.
       eCode [in]：事件代码。
       wParam [in]：与事件eCode关联的数据。
       dwParam [in]：与事件eCode关联的数据。

返回值:
       TRUE：传入事件被处理。
       FALSE：传入事件没被处理。

备注:

==============================================================================*/
boolean DisplayMenu_RouteDialogEvent(CDisplayMenu *pMe,
    AEEEvent    eCode,
    uint16  wParam,
    uint32 dwParam
)
{
    //DISP_ERR("start DisplayMenu_RouteDialogEvent",0,0,0);
    if (NULL == pMe)
    {
        return FALSE;
    }

    if (NULL == pMe->m_pActiveDlg)
    {
        return FALSE;
    }

    //DISP_ERR("%d DisplayMenu_RouteDialogEvent",pMe->m_pActiveDlgID,0,0);
    switch (pMe->m_pActiveDlgID)
    {
        case IDD_DISPLAYMENU_MENU:
            return HandleMainDialogEvent(pMe,eCode,wParam,dwParam);

        case IDD_PICTURE_DLG:
            return HandlePictureDialogEvent(pMe,eCode,wParam,dwParam);

        case IDD_IMGDELMSG_DLG:
            return HandleImgDelMsgDialogEvent(pMe,eCode,wParam,dwParam);

        case IDD_BACKLIGHTING_MENU:
            return HandleBacklightingDialogEvent(pMe,eCode,wParam,dwParam);

        case IDD_CONTRAST_MENU:
            return HandleContrastDialogEvent(pMe,eCode,wParam,dwParam);
#ifdef FEATURE_SCREEN_SAVE
        case IDD_SCREENSAVERTIMEOUT_DIALOG:
            return HandleScreensaverTimeoutDialogEvent(pMe,eCode,wParam,dwParam);
#endif
        case IDD_DESKTOP_THEME:
            return HandleDesktop_Theme_DialogEvent(pMe,eCode,wParam,dwParam);
#ifdef FEATURE_LED_CONTROL
        case IDD_LED_CONTROL:
            return Handle_LED_Control_DialogEvent(pMe,eCode,wParam,dwParam);
#endif

#ifdef FEATRUE_KEY_PAD_CTL
        case IDD_KEY_PAD_CTL:
            return DisplayMenu_Handle_Key_Pad_Ctl_DialogEvent(pMe,eCode,wParam,dwParam);
#endif

#ifdef FEATURE_MENU_STYLE
        case IDD_MENU_STYLE:
            return HandleMenuStyleDialogEvent(pMe,eCode,wParam,dwParam);
#endif

#ifdef FEATURE_RANDOM_MENU_COLOR
        case IDD_COLORFUL_MENU:
            return HandleColorfulMenuDialogEvent(pMe,eCode,wParam,dwParam);

        case IDD_COLOR_SETTING:
            return HandleColorSettingDialogEvent(pMe,eCode,wParam,dwParam);
#endif
#ifdef FEATURE_RANDOM_MENU_REND//wlh 20090405 add for REND
        case IDD_REND_MENU:
            return HandleRENDMenuDialogEvent(pMe,eCode,wParam,dwParam);
#endif
        default:
            return FALSE;
    }
}

/*==============================================================================
函数：
       HandleMainDialogEvent
说明：
       IDD_DISPLAYMENU_MENU对话框事件处理函数

参数：
       pMe [in]：指向DisplayMenu Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/
static boolean  HandleMainDialogEvent(CDisplayMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    PARAM_NOT_REF(dwParam)

    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,
                                                    IDC_DISPLAYMENU);
    if (pMenu == NULL)
    {
        return FALSE;
    }
    DISP_ERR("%x, %x ,%x,HandleMainDialogEvent",eCode,wParam,dwParam);
    //实现菜单循环滚动功能
    DisplayMenu_AutoScroll(pMenu,eCode,wParam);

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
			//add by yangdecai
			{
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_APPSDISPLAYMENU_RES_FILE,                                
                        IDS_DISPLAY_TITLE,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldTextEx(pMe->m_pIAnn,WTitle,FALSE);
            }
#ifdef FEATURE_MENU_STYLE
            IMENUCTL_AddItem(pMenu, AEE_APPSDISPLAYMENU_RES_FILE, IDS_MENU_STYLE, IDS_MENU_STYLE, NULL, 0);
#endif
            IMENUCTL_AddItem(pMenu, AEE_APPSDISPLAYMENU_RES_FILE, IDS_WALLPAPER_TITLE, IDS_WALLPAPER_TITLE, NULL, 0);
#ifdef FEATURE_SCREEN_SAVE
            IMENUCTL_AddItem(pMenu, AEE_APPSDISPLAYMENU_RES_FILE, IDS_SCREENSAVETIMEOUT, IDS_SCREENSAVETIMEOUT, NULL, 0);
#endif
            IMENUCTL_AddItem(pMenu, AEE_APPSDISPLAYMENU_RES_FILE, IDS_BACKLIGHT_TITLE, IDS_BACKLIGHT_TITLE, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSDISPLAYMENU_RES_FILE, IDS_BACKLIGHTING_TITLE, IDS_BACKLIGHTING_TITLE, NULL, 0);
            //IMENUCTL_AddItem(pMenu, AEE_APPSDISPLAYMENU_RES_FILE, IDS_CONTRAST_TITLE, IDS_CONTRAST_TITLE, NULL, 0);
            
#ifdef FEATURE_LED_CONTROL
            IMENUCTL_AddItem(pMenu, AEE_APPSDISPLAYMENU_RES_FILE, IDS_LED_CONTROL, IDS_LED_CONTROL, NULL, 0);
#endif

#ifdef FEATURE_ANIMATION_POWERUPDOWN
            IMENUCTL_AddItem(pMenu, AEE_APPSDISPLAYMENU_RES_FILE, IDS_STARTUP_ANIMATION, IDS_STARTUP_ANIMATION, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSDISPLAYMENU_RES_FILE, IDS_POWEROFF_ANIMATION, IDS_POWEROFF_ANIMATION, NULL, 0);
#endif

#ifdef FEATURE_SCREEN_SAVE
            IMENUCTL_AddItem(pMenu, AEE_APPSDISPLAYMENU_RES_FILE, IDS_SCREENSAVE_TYPE, IDS_SCREENSAVE_TYPE, NULL, 0);
#endif
#ifdef FEATRUE_KEY_PAD_CTL
            IMENUCTL_AddItem(pMenu, AEE_APPSDISPLAYMENU_RES_FILE, IDS_CLOSE_KEY_PAD, IDS_CLOSE_KEY_PAD, NULL, 0);
#endif
#ifdef FEATURE_RANDOM_MENU_COLOR
            IMENUCTL_AddItem(pMenu, AEE_APPSDISPLAYMENU_RES_FILE, IDS_COLORFUL_MENU_TITLE, IDS_COLORFUL_MENU_TITLE, NULL, 0);
#endif
#ifdef FEATURE_RANDOM_MENU_REND//wlh 20090405 add for REND
#ifndef FEATURE_VERSION_C316
#ifndef FEATURE_VERSION_K212
            IMENUCTL_AddItem(pMenu, AEE_APPSDISPLAYMENU_RES_FILE, IDS_REND_MENU_TITLE, IDS_REND_MENU_TITLE, NULL, 0);
#endif
#endif
#endif
            return TRUE;
        case EVT_DIALOG_START:
            //IDISPLAY_SetClipRect(pMe->m_pDisplay, NULL);
            // 设定菜单控件的矩形位置及尺寸
            IMENUCTL_SetSel(pMenu, pMe->m_currDlgId);
            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY|MP_ACTIVE_NO_REDRAW);
            IMENUCTL_SetOemProperties( pMenu, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SETTING_BACKGROUND);
#endif
            IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
			// 统一更新界面
			IANNUNCIATOR_EnableAnnunciatorBar(pMe->m_pIAnn, AEECLSID_DISPLAY1, TRUE);
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

                default:
                    break;
            }
            return TRUE;

        case EVT_COMMAND:
            //pMe->m_currDlgId = IMENUCTL_GetSel(pMenu);
            pMe->m_currDlgId = wParam;
            switch (wParam)
            {
                case IDS_BACKLIGHTING_TITLE:       /*背光时间设置*/
                    CLOSE_DIALOG(DLGRET_BACKLIGHTING)
                    break;

                case IDS_BACKLIGHT_TITLE:       /*背光强度设置*/
                    CLOSE_DIALOG(DLGRET_BRIGHTNESS)
                    break;

#ifdef FEATURE_SCREEN_SAVE
                case IDS_SCREENSAVETIMEOUT:/*屏保时间*/
                    CLOSE_DIALOG(DLGRET_SCREENTIMEOUT)
                    break;
#endif
                case IDS_WALLPAPER_TITLE:     /*桌面墙纸*/
                    pMe->m_PICType = WALLPAPER_MAIN;
                    CLOSE_DIALOG(DLGRET_PICTURE)
                    break;

#ifdef FEATURE_ANIMATION_POWERUPDOWN
                case IDS_STARTUP_ANIMATION: /*开机动画*/
                    pMe->m_PICType = ANIMATION_POWERUP;

                    ShowBusyIcon(pMe->m_pShell,
                                            pMe->m_pDisplay,
                                            &pMe->m_rc,
                                            FALSE);

                    CLOSE_DIALOG(DLGRET_PICTURE)
                    break;

                case IDS_POWEROFF_ANIMATION:    /*关机动画*/
                    pMe->m_PICType = ANIMATION_POWERDOWN;

                    ShowBusyIcon(pMe->m_pShell,
                                            pMe->m_pDisplay,
                                            &pMe->m_rc,
                                            FALSE);

                    CLOSE_DIALOG(DLGRET_PICTURE)
                    break;
#endif//FEATURE_ANIMATION_POWERUPDOWN

               case IDS_DESKTOP_THEME:/*桌面风格*/
                    CLOSE_DIALOG(DLGRET_DESKTOP_THEME)
                    break;

#ifdef FEATURE_LED_CONTROL
                case IDS_LED_CONTROL:
                    CLOSE_DIALOG(DLGRET_LED_CONTROL)
                    break;
#endif

#ifdef FEATURE_SCREEN_SAVE
                case IDS_SCREENSAVE_TYPE:     /*桌面墙纸*/
                    pMe->m_PICType = SCREENSAVE_TYPE;

                    ShowBusyIcon(pMe->m_pShell,
                                            pMe->m_pDisplay,
                                            &pMe->m_rc,
                                            FALSE);

                    CLOSE_DIALOG(DLGRET_PICTURE)
                    break;
#endif

#ifdef FEATRUE_KEY_PAD_CTL
                case IDS_CLOSE_KEY_PAD:
                    CLOSE_DIALOG(DLGRET_KEY_PAD_CTL)
                    break;
#endif

#ifdef FEATURE_MENU_STYLE
               case IDS_MENU_STYLE:
                    CLOSE_DIALOG(DLGRET_MENU_STYLE_CTL)
                    break;
#endif
#ifdef FEATURE_RANDOM_MENU_COLOR
                case IDS_COLORFUL_MENU_TITLE:
                    CLOSE_DIALOG(DLGRET_COLORFUL_MENU_CTL)
                    break;
#endif
#ifdef FEATURE_RANDOM_MENU_REND//wlh 20090405 add for rend
                case IDS_REND_MENU_TITLE:
                    CLOSE_DIALOG(DLGRET_REND_MENU_CTL)
                    break;
#endif
                default:
                    ASSERT_NOT_REACHABLE;
            }
            return TRUE;

        default:
            break;
    }
    return FALSE;
} // HandleMainDialogEvent


/*==============================================================================
函数：
       HandlePictureDialogEvent
说明：
       IDD_PICTURE_DLG对话框事件处理函数

参数：
       pMe [in]：指向DisplayMenu Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/
static boolean  HandlePictureDialogEvent(CDisplayMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    MSG_FATAL("%x, %x ,%x,HandlePictureDialogEvent",eCode,wParam,dwParam);
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            // 分类枚举出所有的墙纸或动画
            MSG_FATAL("HandlePictureDialogEvent EVT_DIALOG_INIT",0,0,0);
            DisplayMenu_EnumFile(pMe, pMe->m_PICType);
							
            pMe->m_bOkToDo = TRUE;
            pMe->m_count = 0;
            pMe->m_b_local = TRUE;
            return TRUE;

        case EVT_DIALOG_START:
        /*//取NV中的值,用来显示已设置了的图片或动画.*/
        {
            char wallnvtmp[AEE_MAX_FILE_NAME/*FILESPECLEN*/];
            MSG_FATAL("HandlePictureDialogEvent EVT_DIALOG_START",0,0,0);
            IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,TRUE);
            IANNUNCIATOR_EnableAnnunciatorBar(pMe->m_pIAnn,AEECLSID_DISPLAY1,FALSE);
            switch(pMe->m_PICType)
            {
                case WALLPAPER_MAIN:    //大屏墙纸
                    (void) ICONFIG_GetItem(pMe->m_pConfig,
                                           CFGI_WALLPAPER,
                                           wallnvtmp,
                                           sizeof(wallnvtmp));
                    //DISP_ERR("%s wallnvtmp1",wallnvtmp,0,0);
                    /*是从DISPLAYMENUST_IMGDELMSG返回或挂起后返回则不取nv*/
                    //if(FALSE == pMe->m_DelImageBack)
                    {
                        pMe->m_CurPaper =
                            DisplayMenu_Search_Name_In_Double_list(p_local_wallpaper,wallnvtmp,&pMe->m_count);

                        if(pMe->m_CurPaper == NULL)
                        {
                            pMe->m_CurPaper =
                                DisplayMenu_Search_Name_In_Double_list(p_download_wallpaper,wallnvtmp,&pMe->m_count);
                            pMe->m_b_local = FALSE;
                        }

                        if(pMe->m_CurPaper == NULL)
                        {
                            pMe->m_CurPaper = p_local_wallpaper->phead;
                            pMe->m_b_local = TRUE ;
                        }
                    }
                    break;

#ifdef FEATURE_ANIMATION_POWERUPDOWN
                case ANIMATION_POWERUP: //开机动画
                    (void) ICONFIG_GetItem(pMe->m_pConfig,
                                            CFGI_STARTUP_ANIMATION,
                                            wallnvtmp,
                                            sizeof(wallnvtmp));
                    //DISP_ERR("%s wallnvtmp2",wallnvtmp,0,0);
                    pMe->m_CurPaper =
                            DisplayMenu_Search_Name_In_Double_list(p_powerup_image,wallnvtmp,&pMe->m_count);

                    if(pMe->m_CurPaper == NULL)
                    {
                        pMe->m_CurPaper = p_powerup_image->phead;
                    }

                    break;

                case ANIMATION_POWERDOWN:   //关机动画
                    (void) ICONFIG_GetItem(pMe->m_pConfig,
                                               CFGI_POWEROFF_ANIMATION,
                                               wallnvtmp,
                                               sizeof(wallnvtmp));
                    //DISP_ERR("%s wallnvtmp3",wallnvtmp,0,0);
                    pMe->m_CurPaper =
                            DisplayMenu_Search_Name_In_Double_list(p_poweroff_image,wallnvtmp,&pMe->m_count);

                    if(pMe->m_CurPaper == NULL)
                    {
                        pMe->m_CurPaper = p_poweroff_image->phead;
                    }
                    break;
#endif//FEATURE_ANIMATION_POWERUPDOWN

#ifdef FEATURE_SCREEN_SAVE
                case SCREENSAVE_TYPE:
                    (void) ICONFIG_GetItem(pMe->m_pConfig,
                                               CFGI_SCREENSAVE_TYPE,
                                               wallnvtmp,
                                               sizeof(wallnvtmp));
                    //DISP_ERR("%s wallnvtmp4",wallnvtmp,0,0);
                    pMe->m_CurPaper =
                            DisplayMenu_Search_Name_In_Double_list(p_screensave_image,wallnvtmp,&pMe->m_count);

                    if(pMe->m_CurPaper == NULL)
                    {
                        pMe->m_CurPaper = p_screensave_image->phead;
                    }
                    break;
#endif

                default:
                    break;
            }

            (void) ISHELL_PostEvent( pMe->m_pShell,
                                            AEECLSID_APP_DISPLAYMENU,
                                            EVT_USER_REDRAW,
                                            0,
                                            0);
            return TRUE;
        }

        case EVT_USER_REDRAW:
        {
            //no need for non-bottom bar,this may case the image do not display well
            //DISP_ERR("EVT_USER_REDRAW",0,0,0);
            	{
            AECHAR    lBuf[20] ={0};
            AECHAR    rBuf[20] ={0};
            AEERect clip;
            if(pMe->m_CurPaper == NULL)
            {
                DISP_ERR("no pMe->m_CurPaper, so cann't show picture",0,0,0);
                CLOSE_DIALOG(DLGRET_CANCELED)
                return TRUE;
            }
			
            IDISPLAY_ClearScreen(pMe->m_pDisplay);

            //no need for non-bottom bar,this may case the image do not display well

            //显示墙纸或动画 
            DisplayMenu_DisplayImg(pMe, pMe->m_PICType);
            WSTRCAT(lBuf, L"<");
            WSTRCAT(rBuf, L">");
            SETAEERECT( &clip, pMe->m_rc.x, (pMe->m_rc.dy/2)-10, pMe->m_rc.dx, pMe->m_rc.dy/2);
            DrawTextWithProfile(pMe->m_pShell, pMe->m_pDisplay, RGB_WHITE_NO_TRANS, AEE_FONT_LARGE, lBuf,-1,clip.x,clip.y,&clip, IDF_ALIGN_LEFT|IDF_TEXT_TRANSPARENT);
            DrawTextWithProfile(pMe->m_pShell, pMe->m_pDisplay, RGB_WHITE_NO_TRANS, AEE_FONT_LARGE, rBuf,-1,clip.x,clip.y,&clip,IDF_ALIGN_RIGHT|IDF_TEXT_TRANSPARENT);          
			#if defined(FEATURE_VERSION_K202_LM129C)||defined(FEATURE_VERSION_K212)||defined(FEATURE_VERSION_EC99)||defined(FEATURE_VERSION_K212_20D)
			DrawBottomBar_Ex(pMe->m_pShell,pMe->m_pDisplay, BTBAR_SELECT_BACK);	
			#else
            DrawBottomBar_Ex(pMe->m_pShell,pMe->m_pDisplay, BTBAR_VIEWMORE_BACK);	//Add By zzg 2010_07_23	
            #endif
            // 统一更新界面
            IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);

            
            (void)ISHELL_SetTimer ( pMe->m_pShell,
                                            APPISREADY_TIMER,
                                            DisplayMenu_NextImgIsReadyTimer,
                                            pMe);
            
            	}
            return TRUE;
        }

        case EVT_DIALOG_END:
            //DISP_ERR("EVT_DIALOG_END",0,0,0);
            DisplayMenu_Free_Double_list(p_local_wallpaper);
            DisplayMenu_Free_Double_list(p_download_wallpaper);
            if(p_local_wallpaper)
            {
                FREE(p_local_wallpaper);
                p_local_wallpaper = NULL;
            }
            if(p_download_wallpaper)
            {
                FREE(p_download_wallpaper);
                p_download_wallpaper = NULL;
            }
            DISP_ERR("%d %d EVT_DIALOG_END1",p_local_wallpaper,p_download_wallpaper,0);
#ifdef FEATURE_ANIMATION_POWERUPDOWN
            DisplayMenu_Free_Double_list(p_powerup_image);
            DisplayMenu_Free_Double_list(p_poweroff_image);
            if(p_powerup_image)
            {
                FREE(p_powerup_image);
                p_powerup_image = NULL;
            }
            if(p_poweroff_image)
            {
                FREE(p_poweroff_image);
                p_poweroff_image = NULL;
            }
            DISP_ERR("%d %d EVT_DIALOG_END2",p_powerup_image,p_poweroff_image,0);
#endif

#ifdef FEATURE_SCREEN_SAVE
            DisplayMenu_Free_Double_list(p_screensave_image);
            if(p_screensave_image)
            {
                FREE(p_screensave_image);
                p_screensave_image = NULL;
            }
            DISP_ERR("%d EVT_DIALOG_END3",p_screensave_image,0,0);
#endif


            if (pMe->m_pWallPaper != NULL)
            {
                IIMAGE_Release(pMe->m_pWallPaper);
                pMe->m_pWallPaper = NULL;
            }

            if (pMe->m_pAniPaper != NULL)
            {
                IIMAGE_Stop(pMe->m_pAniPaper);
                IIMAGE_Release(pMe->m_pAniPaper);
                pMe->m_pAniPaper = NULL;
            }

            //if (pMe->m_pTitleImg != NULL)
            //{
            //    IIMAGE_Release(pMe->m_pTitleImg);
            //    pMe->m_pTitleImg = NULL;
            //}
            pMe->m_bOkToDo = TRUE;
            pMe->m_DelImageBack = FALSE;
            if(pMe->m_CurPaper != NULL)
            {
                pMe->m_CurPaper = NULL;
            }
            IANNUNCIATOR_EnableAnnunciatorBar(pMe->m_pIAnn, AEECLSID_DISPLAY1, TRUE);
            IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE);
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;

                case AVK_LEFT:
                case AVK_UP:
                    if(pMe->m_bOkToDo == FALSE)    /*//按键限制时间未到*/
                    {
                        return TRUE;
                    }

                    pMe->m_bOkToDo = FALSE;
                    if(WALLPAPER_MAIN == pMe->m_PICType ||
                       WALLPAPER_SUB == pMe->m_PICType)     /*//墙纸有下载图片*/
                    {
                        if(pMe->m_b_local)
                        {
                            pMe->m_CurPaper = DisplayMenu_Search_Pref_In_Double_list(p_local_wallpaper,pMe->m_CurPaper,&pMe->m_count);
                        }
                        else
                        {
                            pMe->m_CurPaper = DisplayMenu_Search_Pref_In_Double_list(p_download_wallpaper,pMe->m_CurPaper,&pMe->m_count);
                        }
                    }
#ifdef FEATURE_ANIMATION_POWERUPDOWN
                    else if(ANIMATION_POWERDOWN== pMe->m_PICType)   /* 关机动画*/
                    {
                        pMe->m_CurPaper = DisplayMenu_Search_Pref_In_Double_list(p_poweroff_image,pMe->m_CurPaper,&pMe->m_count);
                    }
                    else if(ANIMATION_POWERUP== pMe->m_PICType)   /* 开机动画*/
                    {
                        pMe->m_CurPaper = DisplayMenu_Search_Pref_In_Double_list(p_powerup_image,pMe->m_CurPaper,&pMe->m_count);
                    }
#endif
#ifdef FEATURE_SCREEN_SAVE
                    else if (SCREENSAVE_TYPE== pMe->m_PICType)
                    {
                        pMe->m_CurPaper = DisplayMenu_Search_Pref_In_Double_list(p_screensave_image,pMe->m_CurPaper,&pMe->m_count);
                    }
#endif
                    /*//显示墙纸或动画*/
                    (void) ISHELL_PostEvent( pMe->m_pShell,
                                                AEECLSID_APP_DISPLAYMENU,
                                                EVT_USER_REDRAW,
                                                0,
                                                0);
                    break;

                case AVK_RIGHT:
                case AVK_DOWN:
                    if(pMe->m_bOkToDo == FALSE)    /*按键限制时间未到*/
                    {
                        return TRUE;
                    }
                    pMe->m_bOkToDo = FALSE;

                    if(WALLPAPER_MAIN == pMe->m_PICType ||
                       WALLPAPER_SUB == pMe->m_PICType)     /*//墙纸有下载图片*/
                    {
                        if(pMe->m_b_local)
                        {
                            pMe->m_CurPaper = DisplayMenu_Search_Next_In_Double_list(p_local_wallpaper,pMe->m_CurPaper,&pMe->m_count);
                        }
                        else
                        {
                            pMe->m_CurPaper = DisplayMenu_Search_Next_In_Double_list(p_download_wallpaper,pMe->m_CurPaper,&pMe->m_count);
                        }
                    }
#ifdef FEATURE_ANIMATION_POWERUPDOWN
                    else if(ANIMATION_POWERDOWN== pMe->m_PICType)   /* 关机动画*/
                    {
                        pMe->m_CurPaper = DisplayMenu_Search_Next_In_Double_list(p_poweroff_image,pMe->m_CurPaper,&pMe->m_count);
                    }
                    else if(ANIMATION_POWERUP== pMe->m_PICType)   /* 开机动画*/
                    {
                        pMe->m_CurPaper = DisplayMenu_Search_Next_In_Double_list(p_powerup_image,pMe->m_CurPaper,&pMe->m_count);
                    }
#endif
#ifdef FEATURE_SCREEN_SAVE
                    else if (SCREENSAVE_TYPE== pMe->m_PICType)
                    {
                        pMe->m_CurPaper = DisplayMenu_Search_Next_In_Double_list(p_screensave_image,pMe->m_CurPaper,&pMe->m_count);
                    }
#endif

                    //显示墙纸或动画
                    (void) ISHELL_PostEvent( pMe->m_pShell,
                                                    AEECLSID_APP_DISPLAYMENU,
                                                    EVT_USER_REDRAW,
                                                    0,
                                                    0);
                    break;


                case AVK_SELECT:		//Add By zzg 2010_07_23
				{
					#if defined(FEATURE_VERSION_K202_LM129C)||defined(FEATURE_VERSION_K212)||defined(FEATURE_VERSION_EC99)||defined(FEATURE_VERSION_K212_20D)
					switch(pMe->m_PICType)
                    {
                        case WALLPAPER_MAIN:    /*//桌面墙纸*/
                            (void) ICONFIG_SetItem(pMe->m_pConfig,
                                                    CFGI_WALLPAPER,
                                                    pMe->m_CurPaper->imange_name,
                                                    sizeof(pMe->m_CurPaper->imange_name));
                            break;

#ifdef FEATURE_ANIMATION_POWERUPDOWN
                        case ANIMATION_POWERUP: /*//开机动画*/
                            (void) ICONFIG_SetItem(pMe->m_pConfig,
                                                    CFGI_STARTUP_ANIMATION,
                                                    pMe->m_CurPaper->imange_name,
                                                    sizeof(pMe->m_CurPaper->imange_name));
                            break;

                        case ANIMATION_POWERDOWN:/*//关机动画*/
                            (void) ICONFIG_SetItem(pMe->m_pConfig,
                                                    CFGI_POWEROFF_ANIMATION,
                                                    pMe->m_CurPaper->imange_name,
                                                    sizeof(pMe->m_CurPaper->imange_name));
                            break;
#endif// FEATURE_ANIMATION_POWERUPDOWN

#ifdef FEATURE_SCREEN_SAVE
                        case SCREENSAVE_TYPE:
                            (void) ICONFIG_SetItem(pMe->m_pConfig,
                                                    CFGI_SCREENSAVE_TYPE,
                                                    pMe->m_CurPaper->imange_name,
                                                    sizeof(pMe->m_CurPaper->imange_name));
                            break;
#endif

                        default:
                            break;
                    }
                    CLOSE_DIALOG(DLGRET_CANCELED)
					#else
#ifdef FEATURE_APP_MEDIAGALLERY					
					CMediaGallery_FileExplorer(GALLERY_IMAGE_SETTING, NULL);
#endif
					#endif
					break;
				}						//Add End
				
                case AVK_INFO:
                    switch(pMe->m_PICType)
                    {
                        case WALLPAPER_MAIN:    /*//桌面墙纸*/
                            (void) ICONFIG_SetItem(pMe->m_pConfig,
                                                    CFGI_WALLPAPER,
                                                    pMe->m_CurPaper->imange_name,
                                                    sizeof(pMe->m_CurPaper->imange_name));
                            break;

#ifdef FEATURE_ANIMATION_POWERUPDOWN
                        case ANIMATION_POWERUP: /*//开机动画*/
                            (void) ICONFIG_SetItem(pMe->m_pConfig,
                                                    CFGI_STARTUP_ANIMATION,
                                                    pMe->m_CurPaper->imange_name,
                                                    sizeof(pMe->m_CurPaper->imange_name));
                            break;

                        case ANIMATION_POWERDOWN:/*//关机动画*/
                            (void) ICONFIG_SetItem(pMe->m_pConfig,
                                                    CFGI_POWEROFF_ANIMATION,
                                                    pMe->m_CurPaper->imange_name,
                                                    sizeof(pMe->m_CurPaper->imange_name));
                            break;
#endif// FEATURE_ANIMATION_POWERUPDOWN

#ifdef FEATURE_SCREEN_SAVE
                        case SCREENSAVE_TYPE:
                            (void) ICONFIG_SetItem(pMe->m_pConfig,
                                                    CFGI_SCREENSAVE_TYPE,
                                                    pMe->m_CurPaper->imange_name,
                                                    sizeof(pMe->m_CurPaper->imange_name));
                            break;
#endif

                        default:
                            break;
                    }
					pMe->m_msg_id = IDS_DONE;
                    CLOSE_DIALOG(DLGRET_MSG_POP)
                    break;
                default:
                    break;
            }
            return TRUE;

        case EVT_COMMAND:
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

				if(DISPLAYMENU_PT_IN_RECT(wXPos,wYPos,rc))
				{
					if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
					{
						boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_DISPLAYMENU,EVT_USER,AVK_SELECT,0);
						return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)   && wXPos < rc.x + (rc.dx/3)*2 )//左
					{
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_DISPLAYMENU,EVT_USER,AVK_INFO,0);
						 return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
					{						
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_DISPLAYMENU,EVT_USER,AVK_CLR,0);
						 return rt;
					}
				}
				else//wlh 点击一次,显示下一个桌面图片
				{
					boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_DISPLAYMENU,EVT_USER,AVK_DOWN,0);
					return rt;
				}

			}
			break;
#endif //FEATURE_LCD_TOUCH_ENABLE
        default:
            break;
    }
    return FALSE;
} // HandlePictureDialogEvent


/*==============================================================================
函数：
       HandleImgDelMsgDialogEvent
说明：
       IDD_IMGDELMSG_DLG对话框事件处理函数

参数：
       pMe [in]：指向DisplayMenu Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/

static boolean  HandleImgDelMsgDialogEvent(CDisplayMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    static IStatic * pStatic = NULL;
    DISP_ERR("%x, %x ,%x,HandleImgDelMsgDialogEvent",eCode,wParam,dwParam);
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            if (NULL == pStatic)
            {
                AEERect rect = {0};
                if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,AEECLSID_STATIC,(void **)&pStatic))

                {
                    DISP_ERR("ISHELL_CreateInstance,AEECLSID_STATIC 2",0,0,0);
                    return FALSE;
                }
                ISTATIC_SetRect(pStatic, &rect);
            }
            return TRUE;

        case EVT_DIALOG_START:
			{
			PromptMsg_Param_type m_PromptMsg;
            //IDISPLAY_SetClipRect(pMe->m_pDisplay, NULL);
            if(IDS_DEL_ITEM_YESNO == pMe->m_msg_id)
            {
                pMe->m_DelImageBack = TRUE;
            }
            else
            {
#ifdef FEATURE_VERSION_K212
                ISHELL_SetTimer(pMe->m_pShell, 500, DisplayMenu_DialogTimeout, pMe);
#else
				ISHELL_SetTimer(pMe->m_pShell, 2000, DisplayMenu_DialogTimeout, pMe);
#endif           
			}
			
            MEMSET(&m_PromptMsg,0,sizeof(PromptMsg_Param_type));
            m_PromptMsg.nMsgResID= pMe->m_msg_id;
            if(IDS_DEL_ITEM_YESNO == pMe->m_msg_id)
            {
                m_PromptMsg.ePMsgType = MESSAGE_INFORMATIVE;
            }
            else if(IDS_DONE == pMe->m_msg_id)
            {
                m_PromptMsg.ePMsgType = MESSAGE_INFORMATIVE;
            }
            else
            {
                m_PromptMsg.ePMsgType = MESSAGE_WARNNING;
            }
            STRLCPY(m_PromptMsg.strMsgResFile, AEE_APPSDISPLAYMENU_RES_FILE,MAX_FILE_NAME);
            m_PromptMsg.eBBarType = BTBAR_BACK;
            DrawPromptMessage(pMe->m_pDisplay,pStatic,&m_PromptMsg);
            IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            ISHELL_PostEvent( pMe->m_pShell,AEECLSID_APP_DISPLAYMENU,EVT_USER_REDRAW,0,0);
            return TRUE;
        	}

        case EVT_USER_REDRAW:
        {
            return TRUE;
        }

        case EVT_DIALOG_END:
            ISHELL_CancelTimer(pMe->m_pShell, DisplayMenu_DialogTimeout,  pMe);
            ISTATIC_Release(pStatic);
            pStatic = NULL;
            return TRUE;

        case EVT_DISPLAYDIALOGTIMEOUT:
            if(IDS_KEYPAD_TIME_ERR == pMe->m_msg_id)
            {
                CLOSE_DIALOG(DLGRET_CREATE) //creat the keypad dialog
            }
            else
            {
                CLOSE_DIALOG(DLGRET_CANCELED)
            }
            return TRUE;

        case EVT_KEY:
#if 0            
#ifdef FEATURE_RANDOM_MENU_COLOR
            if(pMe->m_eCurState == DISPLAYMENUST_COLOR_SETTING)
            {
                ISHELL_CancelTimer(pMe->m_pShell, DisplayMenu_DialogTimeout,  pMe);
                CLOSE_DIALOG(DLGRET_CANCELED)
                return TRUE;
            }
#endif
#endif
            switch(wParam)
            {
                case AVK_CLR:
                    if(IDS_KEYPAD_TIME_ERR == pMe->m_msg_id)
                    {
                        CLOSE_DIALOG(DLGRET_CREATE) //creat the keypad dialog
                    }
                    else
                    {
                        CLOSE_DIALOG(DLGRET_CANCELED)
                    }
                    break;

                case AVK_SELECT:
                    if(IDS_DEL_ITEM_YESNO == pMe->m_msg_id)
                    {
                        //if(pMe->m_CurPaper < pMe->m_WallCount)  /*如果不是下载图?*/
                        if((p_download_wallpaper) && (!pMe->m_b_local))/*如果不是下载图?*/
                        {
                            break;
                        }
                        if(WALLPAPER_MAIN == pMe->m_PICType)  /*//桌面墙纸删除*/
                        {
                            DisplayMenu_Del_A_Image_In_Double_list(p_download_wallpaper,pMe->m_CurPaper);
                            (void)IFILEMGR_Remove(pMe->m_pIFileMgr,
                                                    pMe->m_CurPaper->imange_name);
                        }
                    }
                    else
                    {
                        CLOSE_DIALOG(DLGRET_CREATE) //creat the keypad dialog
                    }
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
} // HandleImgDelMsgDialogEvent

/*==============================================================================
函数：
       HandleBacklightingDialogEvent
说明：
       IDD_BACKLIGHTING_MENU对话框事件处理函数

参数：
       pMe [in]：指向DisplayMenu Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/
static boolean  HandleBacklightingDialogEvent(CDisplayMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    PARAM_NOT_REF(dwParam)

    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,
                                                    IDC_BACKLIGHTING);
    if (pMenu == NULL)
    {
        return FALSE;
    }
    DISP_ERR("%x, %x ,%x,HandleBacklightingDialogEvent",eCode,wParam,dwParam);
    //实现菜单循环滚动功能
    DisplayMenu_AutoScroll(pMenu,eCode,wParam);

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            return TRUE;

        case EVT_DIALOG_START:
        {
#ifdef FETURE_VERSION_W208S
            byte      backlight = OEMNV_BL_7S;
            uint16    ui16_return = IDS_7S;
#else
		    byte      backlight = OEMNV_BL_10S;
            uint16    ui16_return = IDS_10S;
#endif
			//add by yangdecai
			{
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_APPSDISPLAYMENU_RES_FILE,                                
                        IDS_BACKLIGHTING_TITLE,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldTextEx(pMe->m_pIAnn,WTitle,FALSE);
            }
#ifdef FEATRUE_SET_LCD_ALWAYS_OFF
            IMENUCTL_AddItem(pMenu, AEE_APPSDISPLAYMENU_RES_FILE, IDS_ALWAYS_OFF, IDS_ALWAYS_OFF, NULL, 0);
#endif
#ifdef FEATURE_VERSION_W208S

            IMENUCTL_AddItem(pMenu, AEE_APPSDISPLAYMENU_RES_FILE, IDS_7S, IDS_7S, NULL, 0);
#else 
            IMENUCTL_AddItem(pMenu, AEE_APPSDISPLAYMENU_RES_FILE, IDS_10S, IDS_10S, NULL, 0);

#endif
            IMENUCTL_AddItem(pMenu, AEE_APPSDISPLAYMENU_RES_FILE, IDS_30S, IDS_30S, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSDISPLAYMENU_RES_FILE, IDS_50S, IDS_50S, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSDISPLAYMENU_RES_FILE, IDS_ALWAYS_ON, IDS_ALWAYS_ON, NULL, 0);


            (void) ICONFIG_GetItem(pMe->m_pConfig,
                                       CFGI_BACK_LIGHT,
                                       &backlight,
                                       sizeof(backlight));

            switch(backlight)
            {
#ifdef FEATRUE_SET_LCD_ALWAYS_OFF
                case OEMNV_BL_OFF:      //关
                    ui16_return = IDS_ALWAYS_OFF;
                    break;
#endif
#ifdef FEATURE_VERSION_W208S

                case OEMNV_BL_7S:      //7s
                default:
                    ui16_return = IDS_7S;
                    break;
#else

                case OEMNV_BL_10S:      //10s
                default:
                    ui16_return = IDS_10S;
                    break;
#endif

                case OEMNV_BL_30S:      //30s
                    ui16_return = IDS_30S;
                    break;

                case OEMNV_BL_50S:      //50秒
                    ui16_return = IDS_50S;
                    break;

                case OEMNV_BL_ALWAYS_ON:      //ALWAYS_ON
                    ui16_return = IDS_ALWAYS_ON;
                    break;
            }


            InitMenuIcons(pMenu);
            SetMenuIcon(pMenu, ui16_return, TRUE);
            IMENUCTL_SetSel(pMenu, ui16_return);

            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_TEXT_ALIGN_LEFT_ICON_ALIGN_RIGHT|MP_ACTIVE_NO_REDRAW);
            IMENUCTL_SetOemProperties( pMenu, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SETTING_BACKGROUND);
#endif
            IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
            return TRUE;
        }

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;

                default:
                    break;
            }
            return TRUE;

        case EVT_COMMAND:
        {
            byte      backlight = OEMNV_BL_10S;

            switch (wParam)
            {
#ifdef FEATRUE_SET_LCD_ALWAYS_OFF
                case IDS_ALWAYS_OFF:          //关
                    backlight = OEMNV_BL_OFF;
                    break;
#endif

#ifdef FEATURE_VERSION_W208S
                case IDS_7S:          //7秒
                    backlight = OEMNV_BL_7S;
                    break;
#endif					

                case IDS_10S:          //10秒
                    backlight = OEMNV_BL_10S;
                    break;

                case IDS_30S:          //30秒
                    backlight = OEMNV_BL_30S;
                    break;

                case IDS_50S:          //50秒
                    backlight = OEMNV_BL_50S;
                    break;

                case IDS_ALWAYS_ON:          //50秒
                    backlight = OEMNV_BL_ALWAYS_ON;
                    break;

                default:
                    ASSERT_NOT_REACHABLE;
            }

            (void) ICONFIG_SetItem(pMe->m_pConfig,
                                   CFGI_BACK_LIGHT,
                                   &backlight,
                                   sizeof(backlight));

            //IBacklight_Enable(pMe->m_pIBacklight);      //NV立即生效
            IBACKLIGHT_Enable(pMe->m_pIBacklight);
            InitMenuIcons(pMenu);
            SetMenuIcon(pMenu, wParam, TRUE);
            pMe->m_msg_id = IDS_DONE;
            CLOSE_DIALOG(DLGRET_MSG_POP);
        }
        return TRUE;

        default:
            break;
    }
    return FALSE;
} // HandleBacklightingDialogEvent


/*==============================================================================
函数：
       HandleContrastDialogEvent
说明：
       IDD_CONTRAST_MENU对话框事件处理函数

参数：
       pMe [in]：指向DisplayMenu Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/
static boolean  HandleContrastDialogEvent(CDisplayMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    PARAM_NOT_REF(dwParam)

    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,
                                                    IDC_CONTRAST);
    if (pMenu == NULL)
    {
        return FALSE;
    }
    DISP_ERR("%x, %x ,%x,HandleContrastDialogEvent",eCode,wParam,dwParam);
    //实现菜单循环滚动功能
    DisplayMenu_AutoScroll(pMenu,eCode,wParam);

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            IMENUCTL_AddItem(pMenu, AEE_APPSDISPLAYMENU_RES_FILE, IDS_CONTRAST_LEVEL_DARK, IDS_CONTRAST_LEVEL_DARK, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSDISPLAYMENU_RES_FILE, IDS_CONTRAST_LEVEL_NORMAL, IDS_CONTRAST_LEVEL_NORMAL, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSDISPLAYMENU_RES_FILE, IDS_CONTRAST_LEVEL_BRIGHT, IDS_CONTRAST_LEVEL_BRIGHT, NULL, 0);
            return TRUE;
            
        case EVT_DIALOG_START:
        {
            uint16    ui16_return = OEMNV_BACKLIGHT_LEVEL_NORMAL;

        	#if 0
            (void)IMENUCTL_SetTitle(pMenu,
                                    AEE_APPSDISPLAYMENU_RES_FILE,
                                    IDS_BACKLIGHT_TITLE,
                                    NULL);
			#else
			{
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
			            AEE_APPSDISPLAYMENU_RES_FILE,                                
			            IDS_CONTRAST_TITLE,
			            WTitle,
			            sizeof(WTitle));
				IANNUNCIATOR_SetFieldTextEx(pMe->m_pIAnn,WTitle,FALSE);
			}
			#endif
            (void) ICONFIG_GetItem(pMe->m_pConfig,
                                    CFGI_BACKLIGHT_LEVEL,
                                    &pMe->m_lightlevel,
                                    sizeof(pMe->m_lightlevel));

            switch(pMe->m_lightlevel)
            {
                case OEMNV_BACKLIGHT_LEVEL_DARK:
                    ui16_return = IDS_CONTRAST_LEVEL_DARK;
                    break;

                case OEMNV_BACKLIGHT_LEVEL_NORMAL:
                    ui16_return = IDS_CONTRAST_LEVEL_NORMAL;
                    break;
                    

                case OEMNV_BACKLIGHT_LEVEL_BRIGHT:
                    ui16_return = IDS_CONTRAST_LEVEL_BRIGHT;
                    break;


                default:
                    ui16_return = IDS_CONTRAST_LEVEL_NORMAL;
                    break;

            }

            // 设定菜单控件的矩形位置及尺寸
            InitMenuIcons(pMenu);
            SetMenuIcon(pMenu, ui16_return, TRUE);
            IMENUCTL_SetSel(pMenu, ui16_return);

            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_TEXT_ALIGN_LEFT_ICON_ALIGN_RIGHT|MP_ACTIVE_NO_REDRAW);
            IMENUCTL_SetOemProperties( pMenu, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SETTING_BACKGROUND);
#endif
            IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
            return TRUE;
        }

        case EVT_DIALOG_END:
            (void) ICONFIG_SetItem(pMe->m_pConfig,
                               CFGI_BACKLIGHT_LEVEL,
                               &pMe->m_lightlevel,
                               sizeof(pMe->m_lightlevel));

            IBACKLIGHT_Enable( pMe->m_pIBacklight);
            IBACKLIGHT_SetBrightnessLevel( pMe->m_pIBacklight, pMe->m_lightlevel);
            return TRUE;

        //当menu选项更改后,自动调整背光亮度
        case EVT_CTL_SEL_CHANGED:
        {
            byte      lightlevel;

            switch (wParam)
            {
                case IDS_CONTRAST_LEVEL_DARK:
                 lightlevel = OEMNV_BACKLIGHT_LEVEL_DARK;
                 break;
                case IDS_CONTRAST_LEVEL_NORMAL:
                 lightlevel = OEMNV_BACKLIGHT_LEVEL_NORMAL;
                 break;
                default:
                case IDS_CONTRAST_LEVEL_BRIGHT:
                 lightlevel = OEMNV_BACKLIGHT_LEVEL_BRIGHT;
                 break;
            }

            IBACKLIGHT_Enable(pMe->m_pIBacklight);
            IBACKLIGHT_SetBrightnessLevel(pMe->m_pIBacklight,lightlevel);
            return TRUE;
        }

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;

                default:
                    break;
            }
            return TRUE;

        case EVT_COMMAND:
        {
            byte      lightlevel;

            switch (wParam)
            {
                case IDS_CONTRAST_LEVEL_DARK:
                 lightlevel = OEMNV_BACKLIGHT_LEVEL_DARK;
                 break;
                case IDS_CONTRAST_LEVEL_NORMAL:
                 lightlevel = OEMNV_BACKLIGHT_LEVEL_NORMAL;
                 break;
                default:
                case IDS_CONTRAST_LEVEL_BRIGHT:
                 lightlevel = OEMNV_BACKLIGHT_LEVEL_BRIGHT;
                 break;
            }
            pMe->m_lightlevel = lightlevel;
            InitMenuIcons(pMenu);
            SetMenuIcon(pMenu, wParam, TRUE);
            pMe->m_msg_id = IDS_DONE;
            CLOSE_DIALOG(DLGRET_MSG_POP);

            return TRUE;
        }

        default:
            break;
    }
    return FALSE;
} // HandleContrastDialogEvent

#ifdef FEATURE_SCREEN_SAVE
/*==============================================================================
函数：
       HandleScreensaverTimeoutDialogEvent
说明：
       IDD_SCREENSAVERTIMEOUT_DIALOG 对话框事件处理函数

参数：
       pMe [in]：指向DisplayMenu Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/
static boolean  HandleScreensaverTimeoutDialogEvent(CDisplayMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    PARAM_NOT_REF(dwParam)

    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,
                                                    IDC_SCREENSAVER_TIMEOUT_MENU);
    if (pMenu == NULL)
    {
        return FALSE;
    }
    DISP_ERR("%x, %x ,%x,HandleScreensaverTimeoutDialogEvent",eCode,wParam,dwParam);
    //实现菜单循环滚动功能
    DisplayMenu_AutoScroll(pMenu,eCode,wParam);

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
			//add by yangdecai
			{
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_APPSDISPLAYMENU_RES_FILE,                                
                        IDS_SCREENSAVETIMEOUT,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldTextEx(pMe->m_pIAnn,WTitle,FALSE);
            }
            IMENUCTL_AddItem(pMenu, AEE_APPSDISPLAYMENU_RES_FILE, IDS_SCREENSAVETIMEOUT_30S, IDS_SCREENSAVETIMEOUT_30S, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSDISPLAYMENU_RES_FILE, IDS_SCREENSAVETIMEOUT_2MIN, IDS_SCREENSAVETIMEOUT_2MIN, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSDISPLAYMENU_RES_FILE, IDS_SCREENSAVETIMEOUT_5MIN, IDS_SCREENSAVETIMEOUT_5MIN, NULL, 0);
            return TRUE;

        case EVT_DIALOG_START:
        {
            //Read the config item of screensaver time
            byte     time = 0;
            uint16   ui16_return = IDS_SCREENSAVETIMEOUT_2MIN;

            (void) ICONFIG_GetItem(pMe->m_pConfig,
                                           CFGI_SCREENSAVER_TIME,
                                           &time,
                                           sizeof(time));

            switch (time)
            {
                case OEMNV_SCREENSAVER_TIME_30SEC:
                    ui16_return = IDS_SCREENSAVETIMEOUT_30S;
                    break;

                case OEMNV_SCREENSAVER_TIME_2MIN:
                    ui16_return = IDS_SCREENSAVETIMEOUT_2MIN;
                    break;

                case OEMNV_SCREENSAVER_TIME_5MIN:
                    ui16_return = IDS_SCREENSAVETIMEOUT_5MIN;
                    break;

                default:
                    ui16_return = IDS_SCREENSAVETIMEOUT_2MIN;
                    break;
            }
            
            InitMenuIcons(pMenu);
            SetMenuIcon(pMenu, ui16_return, TRUE);
            IMENUCTL_SetSel(pMenu, ui16_return);

            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_ACTIVE_NO_REDRAW);
            IMENUCTL_SetOemProperties( pMenu, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SETTING_BACKGROUND);
#endif
            IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
            return TRUE;
        }

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;

                default:
                    break;
            }
            return TRUE;

        case EVT_COMMAND:
        {
            //Save the config item of screensaver time
            byte time;

            switch (wParam)
            {
                case IDS_SCREENSAVETIMEOUT_30S:
                    time = OEMNV_SCREENSAVER_TIME_30SEC;
                    break;

                case IDS_SCREENSAVETIMEOUT_2MIN:
                    time = OEMNV_SCREENSAVER_TIME_2MIN;
                    break;

                case IDS_SCREENSAVETIMEOUT_5MIN:
                    time = OEMNV_SCREENSAVER_TIME_5MIN;
                    break;

                default:
                    time = OEMNV_SCREENSAVER_TIME_2MIN;
                    break;
            }

            (void) ICONFIG_SetItem(pMe->m_pConfig,
                                           CFGI_SCREENSAVER_TIME,
                                           &time,
                                           sizeof(time));

            CLOSE_DIALOG(DLGRET_CANCELED)
            return TRUE;
        }

        default:
            break;
    }
    return FALSE;
} // HandleScreensaverTimeoutDialogEvent
#endif

/*==============================================================================
函数：
       HandleDesktop_Theme_DialogEvent
说明：
       IDD_DESKTOP_THEME对话框事件处理函数

参数：
       pMe [in]：指向DisplayMenu Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/
static boolean  HandleDesktop_Theme_DialogEvent(CDisplayMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    static byte b_pre_theme = DESKTOP_THEME_ORANGE;
    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,
                                                    IDC_DESKTOP_THEME);
    if (pMenu == NULL)
    {
        return FALSE;
    }
    DISP_ERR("%x, %x ,%x,HandleBacklightingDialogEvent",eCode,wParam,dwParam);
    //实现菜单循环滚动功能
    DisplayMenu_AutoScroll(pMenu,eCode,wParam);

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
			//add by yangdecai
			{
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_APPSDISPLAYMENU_RES_FILE,                                
                        IDS_DESKTOP_THEME,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldTextEx(pMe->m_pIAnn,WTitle,FALSE);
            }
            IMENUCTL_AddItem(pMenu, AEE_APPSDISPLAYMENU_RES_FILE, IDS_STRING_BLUE, IDS_STRING_BLUE, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSDISPLAYMENU_RES_FILE, IDS_STRING_GREEN, IDS_STRING_GREEN, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSDISPLAYMENU_RES_FILE, IDS_STRING_DEEP_BLUE, IDS_STRING_DEEP_BLUE, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSDISPLAYMENU_RES_FILE, IDS_STRING_ORANGE, IDS_STRING_ORANGE, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSDISPLAYMENU_RES_FILE, IDS_STRING_RED, IDS_STRING_RED, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSDISPLAYMENU_RES_FILE, IDS_STRING_YELLOW, IDS_STRING_YELLOW, NULL, 0);
            return TRUE;

        case EVT_DIALOG_START:
        {
            //byte      desktop_type = DESKTOP_THEME_ORANGE;
            uint16    ui16_return = 0;


            (void) ICONFIG_GetItem(pMe->m_pConfig,
                                       CFGI_DESKTOP_THEME,
                                       &b_pre_theme,
                                       sizeof(b_pre_theme));

            switch(b_pre_theme)
            {
                case DESKTOP_THEME_BLUE:
                    ui16_return = IDS_STRING_BLUE;
                    break;

                case DESKTOP_THEME_GREEN:
                    ui16_return = IDS_STRING_GREEN;
                    break;

                case DESKTOP_THEME_DEEP_BLUE:
                    ui16_return = IDS_STRING_DEEP_BLUE;
                    break;

                case DESKTOP_THEME_ORANGE:
                    ui16_return = IDS_STRING_ORANGE;
                    break;

                case DESKTOP_THEME_RED:
                    ui16_return = IDS_STRING_RED;
                    break;

                case DESKTOP_THEME_YELLOW:
                    ui16_return = IDS_STRING_YELLOW;
                    break;

                default:
                    ui16_return = IDS_STRING_BLUE;
                    break;
            }


            InitMenuIcons(pMenu);
            SetMenuIcon(pMenu, ui16_return, TRUE);
            IMENUCTL_SetSel(pMenu, ui16_return);

            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_TEXT_ALIGN_LEFT_ICON_ALIGN_RIGHT|MP_ACTIVE_NO_REDRAW);
            IMENUCTL_SetOemProperties( pMenu, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SETTING_BACKGROUND);
#endif
            IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
            return TRUE;
        }

        case EVT_DIALOG_END:
            {
                byte      desktop_type = DESKTOP_THEME_ORANGE;
                ICONFIG_GetItem(pMe->m_pConfig,CFGI_DESKTOP_THEME, &desktop_type,sizeof(desktop_type));
                if(b_pre_theme != desktop_type)
                {
                    //user not press EVT_COMMAND to save the theme
                    ICONFIG_SetItem(pMe->m_pConfig,CFGI_DESKTOP_THEME, &b_pre_theme,sizeof(b_pre_theme));
                }
                return TRUE;
            }
        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;

                default:
                    break;
            }
            return TRUE;

        case EVT_COMMAND:
            {
                b_pre_theme = DisplayMenu_Cover_Ids_To_Theme(wParam);
                ICONFIG_SetItem(pMe->m_pConfig, CFGI_DESKTOP_THEME,&b_pre_theme, sizeof(b_pre_theme));
                CLOSE_DIALOG(DLGRET_CANCELED)
                return TRUE;
            }
        case EVT_CTL_SEL_CHANGED:
        {
            byte      desktop_type = DESKTOP_THEME_ORANGE;
            desktop_type = DisplayMenu_Cover_Ids_To_Theme(wParam);
            DISP_ERR("Ids_To_Theme",desktop_type,wParam,0);
            ICONFIG_SetItem(pMe->m_pConfig,CFGI_DESKTOP_THEME, &desktop_type,sizeof(desktop_type));
            ISHELL_PostEvent( pMe->m_pShell,AEECLSID_APP_DISPLAYMENU,EVT_USER_REDRAW,0,0);
            return TRUE;
        }
        default:
            break;
    }
    return FALSE;
} // HandleDesktop_Theme_DialogEvent

/*==============================================================================
函数：
       DisplayMenu_EnumFile
说明：
       分类枚举出所有的墙纸或动画

参数：
       pMe [in]：指向DisplayMenu Applet对象结构的指针。该结构包含小程序的特定信息。
       PICTURE_TYPE [in]：PIC类型。

返回值：
       void

备注：

==============================================================================*/
static void DisplayMenu_EnumFile(CDisplayMenu *pMe,PICTURE_TYPE PICType)
{
    FileInfo  Photopos;
    int         Start_bit = 0; /*判断是墙纸、开机动画还是关机动画*/
    char      PhotoName[AEE_MAX_FILE_NAME/*FILESPECLEN*/];
    //DISP_ERR("%d DisplayMenu_EnumFile",PICType,0,0);
    switch (PICType)
    {
        case WALLPAPER_MAIN:
        case WALLPAPER_SUB:
            p_local_wallpaper = DisplayMenu_Creat_Double_list();
            p_download_wallpaper = DisplayMenu_Creat_Double_list();
            break;

#ifdef FEATURE_ANIMATION_POWERUPDOWN
        case ANIMATION_POWERUP:
            p_powerup_image = DisplayMenu_Creat_Double_list();
            break;

        case ANIMATION_POWERDOWN:
            p_poweroff_image = DisplayMenu_Creat_Double_list();
            break;
#endif

#ifdef FEATURE_SCREEN_SAVE
        case SCREENSAVE_TYPE:
            p_screensave_image = DisplayMenu_Creat_Double_list();
            break;
#endif
        default:
            break;

    }
    //DISP_ERR("Start Enum wallpaper",0,0,0);
    MSG_FATAL("HandlePictureDialogEvent IFILEMGR_EnumInit",0,0,0);
    if(IFILEMGR_EnumInit(pMe->m_pIFileMgr, "fs:/image/wallpaper/", FALSE)  == SUCCESS)
    {
        MSG_FATAL("HandlePictureDialogEvent IFILEMGR_EnumInit SUCCESS",0,0,0);
        //DISP_ERR("IFILEMGR_EnumInit wallpaper ok",0,0,0);
        while(IFILEMGR_EnumNext(pMe->m_pIFileMgr, &Photopos))
        {
            Start_bit = STRLEN("fs:/image/wallpaper/");
            STRCPY(PhotoName,Photopos.szName);   /*将列举出的文件名存入PhotoName中*/
            //DISP_ERR(" wallpaper name",0,0,0);
            //判断是桌面墙纸、开机动画还是关机动画，分别以w、k、g字母开头
            if( ('w' == PhotoName[Start_bit]) || ('W' == PhotoName[Start_bit]) )
            {
                //DISP_ERR(" %d wallpaper name",p_local_wallpaper,0,0);
                if(p_local_wallpaper)
                {
                    DisplayMenu_Add_Node_To_Double_list_Tail(p_local_wallpaper,PhotoName);
                }
            }

#ifdef FEATURE_ANIMATION_POWERUPDOWN
            else if( ('k' == PhotoName[Start_bit]) || ('K' == PhotoName[Start_bit]) )
            {
                //DISP_ERR(" %d p_powerup_image",p_powerup_image,0,0);
                if(p_powerup_image)
                {
                    DisplayMenu_Add_Node_To_Double_list_Tail(p_powerup_image,PhotoName);
                }
            }

            else if( ('g' == PhotoName[Start_bit]) || ('G' == PhotoName[Start_bit]) )
            {
                //DISP_ERR(" %d p_poweroff_image",p_poweroff_image,0,0);
                if(p_poweroff_image)
                {
                    DisplayMenu_Add_Node_To_Double_list_Tail(p_poweroff_image,PhotoName);
                }
            }
#endif//FEATURE_ANIMATION_POWERUPDOWN

#ifdef FEATURE_SCREEN_SAVE
            else if( ('s' == PhotoName[Start_bit]) || ('S' == PhotoName[Start_bit]) )/*screen save image*/
            {
                //DISP_ERR(" %d p_screensave_image",p_screensave_image,0,0);
                if(p_screensave_image)
                {
                    DisplayMenu_Add_Node_To_Double_list_Tail(p_screensave_image,PhotoName);
                }
            }
#endif
        }
    }

    if(PICType == WALLPAPER_MAIN)
    {
        if(IFILEMGR_EnumInit(pMe->m_pIFileMgr, AEEFS_SHARED_DIR"img", FALSE)== EFAILED)
        {
            return;
        }
        //墙纸的下一张存在并且小于100,则记录文件名
        while(IFILEMGR_EnumNext(pMe->m_pIFileMgr, &Photopos))
        {
            ImageType imgType;

            //获取图片的类型
            imgType = DisplayMenu_GetImageFileType(Photopos.szName);
            //当图片不是以下所列的类型则返回枚举下一个
            if(imgType != IMAGE_BMP && imgType != IMAGE_PNG &&
            imgType != IMAGE_JPEG && imgType != IMAGE_GIF)
            //因为时间间隔不定,造成idle刷新问题,固目前先不支持bci墙纸
            //&&imgType != IMAGE_BCI)
            {
                continue;
            }

            PhotoName[0] = '\\';
            STRCPY(PhotoName + 1,Photopos.szName);
            if(p_download_wallpaper)
            {
                DisplayMenu_Add_Node_To_Double_list_Tail(p_download_wallpaper,PhotoName);

                if((p_local_wallpaper->count + p_download_wallpaper->count) > (MAX_WALLPAPER_NUMBER-1))
                {
                    break;
                }
            }

        }

    }
    MSG_FATAL("HandlePictureDialogEvent End",0,0,0);
}


/*==============================================================================
函数：
       DisplayMenu_DisplayImg
说明：
       显示墙纸或动画

参数：
       pMe [in]：指向DisplayMenu Applet对象结构的指针。该结构包含小程序的特定信息。
       PICTURE_TYPE [in]：PIC类型。

返回值：
       void
备注：

==============================================================================*/
#define WALLPAPER_TOP_BAR_HIGHT  0//12 //background color bar
#define WALLPAPER_TOP_TEXT_HIGHT (IDISPLAY_GetFontMetrics(pMe->m_pDisplay,AEE_FONT_BOLD,NULL,NULL) +2)// text clip rectangle height
#define WALLPAPER_SOFT_KEY_HIGHT 16
static void DisplayMenu_DisplayImg(CDisplayMenu *pMe, PICTURE_TYPE PICType)
{
    int16   cx = 0,cy = WALLPAPER_TOP_BAR_HIGHT;
    AEERect rc;
#ifdef FEATURE_ANIMATION_POWERUPDOWN
    AECHAR  szBuf[10];
#endif//FEATURE_ANIMATION_POWERUPDOWN
    MSG_FATAL("DisplayMenu_DisplayImg Start",0,0,0);
    if(pMe->m_CurPaper == NULL)
    {
        MSG_FATAL("Warning pMe->m_CurPaper == NULL", 0, 0 ,0);
        return;
    }
    // 画title底条
    //pMe->m_pTitleImg = NULL//ISHELL_LoadImage(pMe->m_pShell,"\\image\\sm_bgwall.bmp");

    //if(pMe->m_pTitleImg != NULL)
    //{
    //    IIMAGE_Draw(pMe->m_pTitleImg, 0, 0);
    //    IIMAGE_Release(pMe->m_pTitleImg);
    //    pMe->m_pTitleImg = NULL;
    //}
    //else
    //{
    SETAEERECT(&rc, 0, 0, pMe->m_rc.dx, WALLPAPER_TOP_BAR_HIGHT);
    IDISPLAY_FillRect(pMe->m_pDisplay, &rc, MAKE_RGB(80, 176, 235));
    //}

    switch(PICType)
    {
        case WALLPAPER_MAIN:
        case WALLPAPER_SUB:
        {
            AECHAR  Downwallname[AEE_MAX_FILE_NAME/*FILESPECLEN*/];
            AECHAR  wFormat[AEE_MAX_FILE_NAME/*FILESPECLEN*/];
            AEEImageInfo  rImageInfo;

            MSG_FATAL("DisplayMenu_DisplayImg case WALLPAPER_SUB",0,0,0);
            // 画图片
            if (pMe->m_pWallPaper != NULL)
            {
                IIMAGE_Release(pMe->m_pWallPaper);
                pMe->m_pWallPaper = NULL;
            }
            if(PICType == WALLPAPER_MAIN)
            {
                pMe->m_pWallPaper = ISHELL_LoadImage( pMe->m_pShell,
                                            pMe->m_CurPaper->imange_name);
            }
            MSG_FATAL("pMe->m_CurPaper=%x", pMe->m_CurPaper, 0 ,0);

            if(pMe->m_pWallPaper == NULL)
            {
                MSG_FATAL("FAIL TO LOAD IMG", 0, 0 ,0);
                return;
            }


            IIMAGE_GetInfo( pMe->m_pWallPaper, &rImageInfo );
            MSG_FATAL("rImageInfo.cx = %d, rImageInfo.cy = %d", rImageInfo.cx, rImageInfo.cy ,0);
            if(rImageInfo.cx <= pMe->m_rc.dx)
            {
                cx += (pMe->m_rc.dx - rImageInfo.cx)/2;
            }
            if(rImageInfo.cy <= (pMe->m_rc.dy - WALLPAPER_TOP_BAR_HIGHT))
            {
                cy += (pMe->m_rc.dy - rImageInfo.cy - WALLPAPER_TOP_BAR_HIGHT)/2;
            }
            MSG_FATAL("cx = %d, cy = %d", cx, cy ,0);
            IIMAGE_Draw(pMe->m_pWallPaper, cx, cy);
            break;
        }


#ifdef FEATURE_ANIMATION_POWERUPDOWN
        case ANIMATION_POWERUP:
        case ANIMATION_POWERDOWN:
            //画标题

            SETAEERECT(&rc, 0, 1, pMe->m_rc.dx, WALLPAPER_TOP_TEXT_HIGHT);

            STRTOWSTR(pMe->m_CurPaper->imange_name, szBuf, sizeof(szBuf));
            (void) IDISPLAY_DrawText(pMe->m_pDisplay,
                                     AEE_FONT_BOLD,
                                     szBuf,
                                     -1,
                                     rc.x,
                                     rc.y,
                                     &rc,
                                     IDF_TEXT_TRANSPARENT |
                                     IDF_ALIGN_CENTER);


            // 画动画
            if (pMe->m_pAniPaper != NULL)
            {
                IIMAGE_Release(pMe->m_pAniPaper);
                pMe->m_pAniPaper = NULL;
            }


            pMe->m_pAniPaper = ISHELL_LoadImage( pMe->m_pShell,pMe->m_CurPaper->imange_name);

            if(pMe->m_pAniPaper == NULL)
            {
                DISP_ERR("FAIL TO LOAD IMG", 0, 0 ,0);
                return;
            }

            IIMAGE_SetParm(pMe->m_pAniPaper,
                           IPARM_CXFRAME,
                           pMe->m_rc.dx,
                           0);              /*设置动画帧的宽度*/

            IIMAGE_SetParm(pMe->m_pAniPaper,
                           IPARM_RATE,
                           ANIMATION_TIMER,
                           0);              /*设置动画帧的宽度*/
            /*show*/
            if(pMe->m_pAniPaper)
            {
                IIMAGE_Start(pMe->m_pAniPaper, 0, 16);
            }

            break;
#endif//FEATURE_ANIMATION_POWERUPDOWN

#ifdef FEATURE_SCREEN_SAVE
            case SCREENSAVE_TYPE:
                SETAEERECT(&rc, 0, 1, pMe->m_rc.dx, WALLPAPER_TOP_TEXT_HIGHT);
                STRTOWSTR(pMe->m_CurPaper->imange_name, szBuf, sizeof(szBuf));
                (void) IDISPLAY_DrawText(pMe->m_pDisplay,
                                         AEE_FONT_BOLD,
                                         szBuf,
                                         -1,
                                         rc.x,
                                         rc.y,
                                         &rc,
                                         IDF_TEXT_TRANSPARENT |
                                         IDF_ALIGN_CENTER);
                // 画动画
                if (pMe->m_pAniPaper != NULL)
                {
                    IIMAGE_Release(pMe->m_pAniPaper);
                    pMe->m_pAniPaper = NULL;
                }
                pMe->m_pAniPaper = ISHELL_LoadImage( pMe->m_pShell,pMe->m_CurPaper->imange_name);

                if(pMe->m_pAniPaper == NULL)
                {
                    DISP_ERR("FAIL TO LOAD IMG", 0, 0 ,0);
                    return;
                }

                IIMAGE_SetParm(pMe->m_pAniPaper,
                               IPARM_CXFRAME,
                               pMe->m_rc.dx,
                               0);              /*设置动画帧的宽度*/

                IIMAGE_SetParm(pMe->m_pAniPaper,
                               IPARM_RATE,
                               ANIMATION_TIMER,
                               0);              /*设置动画帧的宽度*/
                /*show*/
                if(pMe->m_pAniPaper)
                {
                    IIMAGE_Start(pMe->m_pAniPaper, 0, 16);
                }

                break;
#endif
        default:
            break;
    }
    MSG_FATAL("DisplayMenu_DisplayImg End",0,0,0);
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

static void DisplayMenu_NextImgIsReadyTimer(void *pme)
{
    CDisplayMenu *pMe = (CDisplayMenu *)pme;

    if (NULL == pMe)
    {
        return;
    }

    pMe->m_bOkToDo = TRUE;
}

/*==============================================================================
函数：
       DisplayMenu_GetImageFileType

说明：
       获取图片的类型.

参数：
       pszFileName [in]：枚举出来的图片文件名.

返回值：
       ImageType:图片类型

备注：:

==============================================================================*/
ImageType DisplayMenu_GetImageFileType(char * pszFileName)
{
    char *   psz;
    char     szExt[10];

    psz = STRRCHR(pszFileName, '.');
    if (!psz)
    {
        return IMAGE_NONE;
    }

    psz++;
    if (!*psz)
    {
        return IMAGE_NONE;
    }
    STRCPY(szExt, psz);

    if (!STRCMP(szExt,"bmp") || !STRCMP(szExt,"BMP"))
    {
        return IMAGE_BMP;
    }
    else if (!STRCMP(szExt,"png") || !STRCMP(szExt,"PNG") )
    {
        return IMAGE_PNG;
    }
    else if (!STRCMP(szExt,"jpg") || !STRCMP(szExt,"JPG") )
    {
        return IMAGE_JPEG;
    }
    else if (!STRCMP(szExt,"gif") || !STRCMP(szExt,"GIF") )
    {
        return IMAGE_GIF;
    }
    else if (!STRCMP(szExt,"bci") || !STRCMP(szExt,"BCI") )
    {
        return IMAGE_BCI;
    }
    else
    {
        return IMAGE_NONE;
    }
}

static void DisplayMenu_AutoScroll(IMenuCtl  *pMenu, AEEEvent eCode,   uint16 wParam)
{
}
static void DisplayMenu_DrawTextWithProfile(IShell* pShell,
                                        IDisplay * pDisplay,
                                        RGBVAL ProfileColor,
                                        AEEFont Font,
                                        const AECHAR * pcText,
                                        int nChars,
                                        int x,
                                        int y,
                                        const AEERect * prcBackground,
                                        uint32 dwFlags
                                        )
{
    DrawTextWithProfile( pShell,
                                        pDisplay,
                                        ProfileColor,
                                        Font,
                                        pcText,
                                        nChars,
                                        x,
                                        y,
                                        prcBackground,
                                        dwFlags
                                        );
}


/****************************************************************/
static double_list * DisplayMenu_Creat_Double_list(void)
{
    double_list * temp = NULL;
    DISP_ERR("DisplayMenu_Creat_Double_list",0,0,0);
    temp = (double_list*) MALLOC(sizeof(double_list));
    if(temp)
    {
        temp->phead = NULL;
        temp->ptail    = NULL;
        temp->pcur   = NULL;
        temp->count =0;
        //DISP_ERR("OK creat sucess %x",&temp,0,0);
    }
    return temp;
}

static void DisplayMenu_Free_Double_list(double_list * plist )
{
    image_name_table *list_head = NULL;
    image_name_table *temp = NULL;

    //DISP_ERR("DisplayMenu_Free_Double_list",0,0,0);

    if((plist == NULL) ||(plist->phead == NULL))
    {
        return;
    }
    DISP_ERR("%x DisplayMenu_Free_Double_list start",plist->phead,0,0);
    list_head = plist->phead ;

    while(list_head)
    {
        temp =list_head->next;
        FREE(list_head);
        list_head = temp;
        temp = NULL;
    }

    list_head = NULL;
    //FREE(plist);
    //plist = NULL;

}

static void DisplayMenu_Add_Node_To_Double_list_Tail(double_list * plist ,char * image_name)
{
    image_name_table *temp = NULL;

    DISP_ERR("DisplayMenu_Add_Node_To_Double_list_Tail",0,0,0);

    if((plist == NULL) ||(image_name == NULL))
    {
        return;
    }

    temp = (image_name_table*)MALLOC(sizeof(image_name_table));

    if(temp)
    {
        if(plist->count == 0)/*空的双向链表*/
        {
            temp->next = NULL;
            temp->pref = NULL;
            plist->phead = temp;
            plist->ptail   = temp;
        }
        else
        {
            temp->next = NULL;
            temp->pref = plist->ptail;
            plist->ptail->next = temp;
            plist->ptail  = temp;
        }

        STRCPY(temp->imange_name , image_name);
        plist->count ++ ;
    }
    else
    {
        DISP_ERR("MALLOC FAIL,Cann't add",0,0,0);
    }
}

static image_name_table * DisplayMenu_Search_Name_In_Double_list(double_list * plist ,char * image_name , uint16 * count)
{
    image_name_table *b_find = NULL;
    image_name_table *temp   = NULL;
    uint16 i = 1;

    //DISP_ERR("DisplayMenu_Search_Name_In_Double_list",0,0,0);

    if(plist == NULL || image_name == NULL)
    {
        return NULL;
    }

    temp =  plist->phead;

    while(temp != NULL)
    {
        if(0 == STRCMP (image_name,temp->imange_name))
        {
            b_find = temp;
            break;
        }
        temp = temp->next;
        i ++ ;
    }

    if(b_find)
    {
        DISP_ERR("ok,find it %d",i,0,0);
    }
    else
    {
        i = 0;
        DISP_ERR("fail,not find",0,0,0);
    }

    *count = i;

    return b_find;
}

static void DisplayMenu_Del_A_Image_In_Double_list(double_list * plist,image_name_table * image_table)
{
    DISP_ERR("DisplayMenu_Del_A_Image_In_Double_list",0,0,0);

    if((image_table == NULL) || (plist == NULL))
    {
        return;
    }

    if(image_table == plist->phead)/*要删除头结点*/
    {
        DISP_ERR("DEL head",0,0,0);

        plist->phead = image_table->next;/*让头结点指向下一个*/

        if(plist->phead == NULL) /*only one*/
        {
            plist->ptail = NULL;
            plist->pcur = NULL;
        }
        else
        {
            plist->phead->pref = NULL;
        }
    }
    else if(image_table == plist->ptail)
    {
        DISP_ERR("DEL tail",0,0,0);

        plist->ptail= image_table->pref;/*让尾结点指向前一个*/
        if(plist->ptail == NULL) /*only one*/
        {
            plist->phead = NULL;
            plist->pcur = NULL;
        }
        else
        {
            plist->ptail->next= NULL;
        }
    }
    else/*中间的结点*/
    {
        image_name_table * pref = NULL;
        image_name_table * next = NULL;

        pref = image_table->pref;
        next = image_table->next;

        pref->next = image_table->next;
        next->pref = image_table->pref;
    }

    FREE(image_table);
    image_table = NULL;
    plist->count --;
}

static image_name_table * DisplayMenu_Search_Next_In_Double_list(double_list * plist ,image_name_table * p_cur,uint16 * img_id)
{
    DISP_ERR("DisplayMenu_Search_Next_In_Double_list",0,0,0);
    if((p_cur ==plist->phead)&& (p_cur ==plist->ptail)) /*Only one*/
    {
        * img_id = 1;
        return p_cur;
    }
    else if ((p_cur !=plist->phead)&& (p_cur ==plist->ptail))  /*the tail */
    {
        * img_id = 1;
        return plist->phead;
    }
    else
    {
        (* img_id) ++;
        return p_cur->next;
    }

}

static image_name_table * DisplayMenu_Search_Pref_In_Double_list(double_list * plist ,image_name_table * p_cur,uint16 * img_id)
{
    DISP_ERR("DisplayMenu_Search_Pref_In_Double_list",0,0,0);
    if((p_cur ==plist->phead)&& (p_cur ==plist->ptail)) /*Only one*/
    {
        * img_id = 1;
        return p_cur;
    }
    else if ((p_cur ==plist->phead)&& (p_cur !=plist->ptail))  /*the head */
    {
        * img_id = plist->count;
        return plist->ptail;
    }
    else
    {
        (* img_id) --;
        return p_cur->pref;
    }

}
#ifdef FEATURE_LED_CONTROL
static boolean Handle_LED_Control_DialogEvent(CDisplayMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    PARAM_NOT_REF(dwParam)
    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,
                                                    IDC_LED_CONTROL);
    DISP_ERR("%x, %x ,%x,Handle_LED_Control_DialogEvent",eCode,wParam,dwParam);
    if (pMenu == NULL)
    {
        return FALSE;
    }
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            pMe->m_led_ctr = 0xF;
            pMe->m_currDlgId = 0;
			//add by yangdecai
			{
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_APPSDISPLAYMENU_RES_FILE,                                
                        IDS_LED_CONTROL,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldTextEx(pMe->m_pIAnn,WTitle,FALSE);
            }
#if defined(FEATURE_WMS_APP) && !defined(FEATURE_WMSAPP_ONLYSUPPORTVMAIL)
            IMENUCTL_AddItem(pMenu, AEE_APPSDISPLAYMENU_RES_FILE, IDS_LED_SMS, IDS_LED_SMS, NULL, 0);
#endif
            IMENUCTL_AddItem(pMenu, AEE_APPSDISPLAYMENU_RES_FILE, IDS_LED_CALL, IDS_LED_CALL, NULL, 0);
#if 0
            IMENUCTL_AddItem(pMenu, AEE_APPSDISPLAYMENU_RES_FILE, IDS_LED_NETWORK, IDS_LED_NETWORK, NULL, 0);
#endif
            IMENUCTL_AddItem(pMenu, AEE_APPSDISPLAYMENU_RES_FILE, IDS_LED_ALARM, IDS_LED_ALARM, NULL, 0);
            return TRUE;

        case EVT_DIALOG_START:
            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_ACTIVE_NO_REDRAW);
            IMENUCTL_SetOemProperties(pMenu, OEMMP_DISTINGUISH_INFOKEY_SELECTKEY |OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SETTING_BACKGROUND);
#endif
            IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_OK_BACK);
            ICONFIG_GetItem(pMe->m_pConfig, CFGI_LED_CONTROL, &pMe->m_led_ctr, sizeof(byte));
            (void) ISHELL_PostEvent( pMe->m_pShell, AEECLSID_APP_DISPLAYMENU,EVT_USER_REDRAW,0,0);
            return TRUE;

        case EVT_USER_REDRAW:
        {
            InitMenuIcons(pMenu);
#if defined(FEATURE_WMS_APP) && !defined(FEATURE_WMSAPP_ONLYSUPPORTVMAIL)
            if(pMe->m_led_ctr &0x1)//sms ok
            {
                SetMenuIcon(pMenu, IDS_LED_SMS, TRUE);
            }
#endif
            if(pMe->m_led_ctr &0x2)//call ok
            {
                SetMenuIcon(pMenu, IDS_LED_CALL, TRUE);
            }
            if(pMe->m_led_ctr &0x4)//net ok
            {
                SetMenuIcon(pMenu, IDS_LED_NETWORK, TRUE);
            }
            if(pMe->m_led_ctr &0x8)//alarm ok
            {
                SetMenuIcon(pMenu, IDS_LED_ALARM, TRUE);
            }
            IMENUCTL_SetSel(pMenu, pMe->m_currDlgId);
            //DISP_ERR("led set %d",pMe->m_led_ctr, 0,0);
            (void)IMENUCTL_Redraw(pMenu);
            return TRUE;
        }


        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                    //ICONFIG_SetItem(pMe->m_pConfig, CFGI_LED_CONTROL, &pMe->m_led_ctr, sizeof(byte));
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;

                case AVK_INFO:
                    ICONFIG_SetItem(pMe->m_pConfig, CFGI_LED_CONTROL, &pMe->m_led_ctr, sizeof(byte));
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;

                default:
                    break;
            }
            return TRUE;

        case EVT_COMMAND:
        {
            pMe->m_currDlgId = IMENUCTL_GetSel(pMenu);
            switch(wParam)
            {
#if defined(FEATURE_WMS_APP) && !defined(FEATURE_WMSAPP_ONLYSUPPORTVMAIL)
                case IDS_LED_SMS:
                    if(pMe->m_led_ctr &0x1)//sms ok
                    {
                        SetMenuIcon(pMenu, IDS_LED_SMS, FALSE);
                        pMe->m_led_ctr  = pMe->m_led_ctr & 0xE;
                    }
                    else
                    {
                        SetMenuIcon(pMenu, IDS_LED_SMS, TRUE);
                        pMe->m_led_ctr  = pMe->m_led_ctr | 0x1;
                    }
                    break;
#endif

                case IDS_LED_CALL:
                    if(pMe->m_led_ctr &0x2)//sms ok
                    {
                        SetMenuIcon(pMenu, IDS_LED_CALL, FALSE);
                        pMe->m_led_ctr  = pMe->m_led_ctr & 0xD;
                    }
                    else
                    {
                        SetMenuIcon(pMenu, IDS_LED_CALL, TRUE);
                        pMe->m_led_ctr  = pMe->m_led_ctr | 0x2;
                    }
                    break;

                case IDS_LED_NETWORK:
                    if(pMe->m_led_ctr &0x4)//sms ok
                    {
                        SetMenuIcon(pMenu, IDS_LED_NETWORK, FALSE);
                        pMe->m_led_ctr  = pMe->m_led_ctr & 0xB;
                    }
                    else
                    {
                        SetMenuIcon(pMenu, IDS_LED_NETWORK, TRUE);
                        pMe->m_led_ctr  = pMe->m_led_ctr | 0x4;
                    }
                    break;

                case IDS_LED_ALARM:
                    if(pMe->m_led_ctr &0x8)//sms ok
                    {
                        SetMenuIcon(pMenu, IDS_LED_ALARM, FALSE);
                        pMe->m_led_ctr  = pMe->m_led_ctr & 0x7;
                    }
                    else
                    {
                        SetMenuIcon(pMenu, IDS_LED_ALARM, TRUE);
                        pMe->m_led_ctr  = pMe->m_led_ctr | 0x8;
                    }
                    break;

                default:
                    break;
            }
            //DISP_ERR("led set1 %d",pMe->m_led_ctr, 0,0);
            ISHELL_PostEvent( pMe->m_pShell, AEECLSID_APP_DISPLAYMENU,EVT_USER_REDRAW,0,0);
            return TRUE;
        }


        default:
            break;
    }
    return FALSE;
}
#endif

#ifdef FEATRUE_KEY_PAD_CTL
/*==============================================================================
函数：
       DisplayMenu_Handle_Key_Pad_Ctl_DialogEvent
说明：
       IDD_SETEVENTTIME 对话框事件处理函数

参数：
       pMe [in]：指向ScheduleApp Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：设置每天时间和每个事件时间两个窗口共用以下代码

==============================================================================*/
/*lint -e734*/
static boolean  DisplayMenu_Handle_Key_Pad_Ctl_DialogEvent(CDisplayMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    //int32 endt;

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            {
            pMe->m_TimeCtlStart = (ITimeCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,  IDC_CLOCK_FORM);
            pMe->m_TimeCtlEnd = (ITimeCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,  IDC_CLOCK_TO);
            pMe->m_pState = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,  IDC_KEY_PAD_STATE);

            if(pMe->m_TimeCtlStart == NULL ||pMe->m_TimeCtlEnd == NULL ||pMe->m_pState == NULL)
            {
                //DISP_ERR("EVT_DIALOG_INIT FAIL", 0, 0, 0);
                return FALSE;
            }

            IMENUCTL_AddItem(pMe->m_pState, AEE_APPSDISPLAYMENU_RES_FILE, IDS_STRING_ON, IDS_STRING_ON, NULL, 0);
            IMENUCTL_AddItem(pMe->m_pState, AEE_APPSDISPLAYMENU_RES_FILE, IDS_STRING_OFF, IDS_STRING_OFF, NULL, 0);
                IMENUCTL_SetOemProperties(pMe->m_pState, OEMMP_SWITCHNAVIGATIONKEY | OEMMP_IDF_ALIGN_CENTER);
                ITIMECTL_SetProperties(pMe->m_TimeCtlStart, TP_NO_SECONDS | TP_AUTOREDRAW);
                ITIMECTL_SetProperties(pMe->m_TimeCtlEnd, TP_NO_SECONDS | TP_AUTOREDRAW);
            ICONFIG_GetItem(pMe->m_pConfig, CFGI_KEY_PAD_CTL, (void*)&pMe->m_ClockCfg, sizeof(Key_pad_Cfg));
            ICONFIG_GetItem(pMe->m_pConfig,CFGI_TIME_FORMAT,&pMe->m_SetTimeRange,sizeof(byte));
            //ClockCtl_Init(&pMe->m_ClockCtlStart,pMe->m_TimeCtlStart,pMe->m_pDisplay,pMe->m_pShell,TRUE);

            //ClockCtl_Init(&pMe->m_ClockCtlEnd,pMe->m_TimeCtlEnd,pMe->m_pDisplay,pMe->m_pShell,TRUE);
            pMe->m_nCtlID = IDC_KEY_PAD_STATE;
            }
            return TRUE;


        case EVT_DIALOG_START:
        	{
/*
                AEERect rc;

                SETAEERECT(&rc,  CONTROL_X, LINEHEIGHTEX + 1,CONTROL_DX,LINEHEIGHTEX);
                IMENUCTL_SetRect(pMe->m_pState, &rc);

                ITIMECTL_SetProperties(pMe->m_TimeCtlStart, TP_NO_SECONDS | TP_AUTOREDRAW);
                ITIMECTL_SetProperties(pMe->m_TimeCtlEnd, TP_NO_SECONDS | TP_AUTOREDRAW);
                IMENUCTL_SetOemProperties(pMe->m_pState, OEMMP_SWITCHNAVIGATIONKEY | OEMMP_IDF_ALIGN_CENTER);
*/

            ITIMECTL_SetOemProperties( pMe->m_TimeCtlStart, TP_OEM_COUNTDOWNCTL_EDITABLE|(pMe->m_SetTimeRange==OEMNV_TIMEFORM_AMPM?TP_OEM_COUNTDOWNCTL_12_FORMAT:0));
            ITIMECTL_SetOemProperties( pMe->m_TimeCtlEnd, TP_OEM_COUNTDOWNCTL_EDITABLE|(pMe->m_SetTimeRange==OEMNV_TIMEFORM_AMPM?TP_OEM_COUNTDOWNCTL_12_FORMAT:0));

	        }
            //DISP_ERR("EVT_DIALOG_START %d", pMe->m_SetTimeRange, 0, 0);
#if 0
            if(pMe->m_SetTimeRange == OEMNV_TIMEFORM_AMPM)
            {
                ITIMECTL_SetIncrement(pMe->m_TimeCtlStart,15);
                ITIMECTL_SetIncrement(pMe->m_TimeCtlEnd,15);

               //ClockCtl_SetIncrement(&pMe->m_ClockCtlStart, 15);
               //ClockCtl_SetIncrement(&pMe->m_ClockCtlEnd, 15);
            }

            if(pMe->m_SetTimeRange == OEMNV_TIMEFORM_24HR)
            {

                ITIMECTL_SetIncrement(pMe->m_TimeCtlStart,30);
                ITIMECTL_SetIncrement(pMe->m_TimeCtlEnd,30);

                //ClockCtl_SetIncrement(&pMe->m_ClockCtlStart, 30);
                //ClockCtl_SetIncrement(&pMe->m_ClockCtlEnd, 30);
            }
#endif
            {
                uint32 m_one_day = 86400000; /*60 *60 *24 *1000 */
                DISP_ERR("from %d to %d state %d CFGI_KEY_PAD_CTL", pMe->m_ClockCfg.from_Time, pMe->m_ClockCfg.to_Time, pMe->m_ClockCfg.bStateOn);
                if(pMe->m_ClockCfg.from_Time > m_one_day)
                {
                    pMe->m_ClockCfg.from_Time = 0;
                }

                if(pMe->m_ClockCfg.to_Time > m_one_day)
                {
                    pMe->m_ClockCfg.to_Time = 0;
                }
            }

            ITIMECTL_SetTime(pMe->m_TimeCtlStart, pMe->m_ClockCfg.from_Time);
            //ClockCtl_SetTime(&pMe->m_ClockCtlStart, (uint32)pMe->m_ClockCfg.from_Time);

            ITIMECTL_SetTime(pMe->m_TimeCtlEnd, pMe->m_ClockCfg.to_Time);
            //ClockCtl_SetTime(&pMe->m_ClockCtlEnd, (uint32)pMe->m_ClockCfg.to_Time);
            IDIALOG_SetProperties(pMe->m_pActiveDlg,DLG_NOT_SET_FOCUS_AUTO);
            ISHELL_PostEvent( pMe->m_pShell,AEECLSID_APP_DISPLAYMENU,EVT_USER_REDRAW,0,0);
            return TRUE;

        case EVT_DIALOG_END:

			{
            boolean bOn = FALSE;
            JulianType  jDate;
            uint32 current_time;
            uint32 from_time;
            uint32 to_time;

            if (pMe->m_ClockCfg.bStateOn)
            {
                current_time = GETTIMESECONDS();

                if (current_time > 630720000)
                {
                    GETJULIANDATE(current_time, &jDate);

                    from_time = pMe->m_ClockCfg.from_Time / 1000;
                    to_time = pMe->m_ClockCfg.to_Time / 1000;
                    jDate.wHour = 0;
                    jDate.wMinute = 0;
                    jDate.wSecond = 0;
                    from_time += JULIANTOSECONDS(&jDate);
                    to_time += JULIANTOSECONDS(&jDate);

                    if(to_time < from_time)
                    {
                        if((current_time < to_time) ||(current_time > from_time))
                        {
                            bOn = TRUE;
                        }
                    }
                    else if((to_time > from_time)&&(current_time > from_time) && (current_time < to_time))
                    {
                        bOn = TRUE;
                    }
                }
                else
                {
                    bOn = TRUE;
                }
            }
            if (bOn)
            {
                IBACKLIGHT_Enable(pMe->m_pKeypadBacklight);

            }
            else
            {
                IBACKLIGHT_Disable(pMe->m_pKeypadBacklight);
            }

            //IBACKLIGHT_Disable(pMe->m_pKeypadBacklight);
            //ClockCtl_Free(&pMe->m_ClockCtlStart);
            //ClockCtl_Free(&pMe->m_ClockCtlEnd);
            return TRUE;
        }

        case EVT_USER_REDRAW:
        {
            AEERect rc;
            AECHAR  w_string[32] = {0};
            AEEMenuColors color;
            TitleBar_Param_type title;
            //DISP_ERR("EVT_USER_REDRAW", 0, 0, 0);
            //The first line ->Menu
            int title_hight = GetTitleBarHeight(pMe->m_pDisplay);
            int lineSpace = ( pMe->m_rc.dy - title_hight * 5) / 4;
            nv_language_enum_type language = 0;
            IMenuCtl* pMenuCtl;
#ifdef FEATURE_CARRIER_SUDAN_SUDATEL
#if !defined( AEE_SIMULATOR)

            extern int OEM_GetConfig( AEEConfigItem i, void * pBuff, int nSize);



           OEM_GetConfig( CFGI_LANGUAGE_SELECTION, &language, sizeof(language));

#endif
#endif
            if(pMe->m_ClockCfg.bStateOn == TRUE)
            {
                IMENUCTL_SetSel(pMe->m_pState, IDS_STRING_ON);
            }
            else
            {
                IMENUCTL_SetSel(pMe->m_pState, IDS_STRING_OFF);
            }

            //初始化List Menu的颜色
            color.wMask = MC_SEL_TEXT | MC_SEL_BACK | MC_TEXT | MC_BACK;
            color.cBack = RGB_WHITE;
            color.cText =  RGB_BLACK;
            color.cSelBack = RGB_BLACK;
            color.cSelText = RGB_WHITE;

            //填充背景颜色
            IDISPLAY_FillRect(pMe->m_pDisplay,  &pMe->m_rc,  MAKE_RGB(111, 183, 255));

            pMenuCtl = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_KEYPAD_MENU);
            if(pMenuCtl)
            {
                SETAEERECT(&rc,0, 0,pMe->m_rc.dx,title_hight);
                IMENUCTL_SetRect(pMenuCtl, &rc);
                IMENUCTL_SetActive(pMenuCtl, TRUE);
                IMENUCTL_Redraw(pMenuCtl);
            }


            //The first line ->Text
            if( language == NV_LANGUAGE_ARABIC)
            {
                SETAEERECT(&rc,2, title_hight + lineSpace,pMe->m_rc.dx - CONTROL_X - 3,title_hight);
            }
            else
            {
                SETAEERECT(&rc,  CONTROL_X, title_hight + lineSpace,pMe->m_rc.dx - CONTROL_X - 3,title_hight);
            }
            IDISPLAY_EraseRect(pMe->m_pDisplay,&rc);
            ISHELL_LoadResString(pMe->m_pShell,AEE_APPSDISPLAYMENU_RES_FILE,IDS_STRING_STATE,w_string,sizeof(w_string));
            //textWidth = IDISPLAY_MeasureText( pMe->m_pDisplay, AEE_FONT_BOLD, w_string);//为了让界面看起来整齐，state from to使用同样宽度
            {
                if( language == NV_LANGUAGE_ARABIC)
                {
                    IDISPLAY_DrawText(pMe->m_pDisplay,
                                    AEE_FONT_BOLD,
                                    w_string,
                                    -1,
                                    1,
                                    2+lineSpace+title_hight,
                                    NULL,
                                    IDF_TEXT_TRANSPARENT | IDF_ALIGN_RIGHT
                                );
                    SETAEERECT( &rc,
                                2 + KEYPAD_IMG_DX, //0,
                                title_hight + lineSpace,
                                CONTROL_DX - 2 * KEYPAD_IMG_DX,
                                title_hight
                            );
                }
                else
                {
                    IDISPLAY_DrawText(pMe->m_pDisplay,
                                    AEE_FONT_BOLD,
                                    w_string,
                                    -1,
                                    1,
                                    2+lineSpace+title_hight,
                                    NULL,
                                    IDF_TEXT_TRANSPARENT | IDF_ALIGN_LEFT
                                );
                    SETAEERECT( &rc,
                                CONTROL_X + KEYPAD_IMG_DX,
                                title_hight + lineSpace,
                                CONTROL_DX-2 * KEYPAD_IMG_DX,
                                title_hight
                            );
                }
                IMENUCTL_SetRect( pMe->m_pState, &rc);
            }

            //The 2st line
            if( language == NV_LANGUAGE_ARABIC)
            {
                SETAEERECT(&rc,  2, (title_hight + lineSpace)*2 ,pMe->m_rc.dx - CONTROL_X - 3,title_hight);
            }
            else
            {
                SETAEERECT(&rc,  CONTROL_X, (title_hight + lineSpace)*2 ,pMe->m_rc.dx - CONTROL_X - 3,title_hight);
            }
            IDISPLAY_EraseRect(pMe->m_pDisplay,&rc);
            ISHELL_LoadResString(pMe->m_pShell,AEE_APPSDISPLAYMENU_RES_FILE,IDS_STRING_FROM,w_string,sizeof(w_string));

            {
                if( language == NV_LANGUAGE_ARABIC)
                {
                    IDISPLAY_DrawText(pMe->m_pDisplay,
                                    AEE_FONT_BOLD,
                                    w_string,
                                    -1,
                                    1,
                                    2 + ( title_hight + lineSpace) * 2,
                                    NULL,
                                    IDF_TEXT_TRANSPARENT | IDF_ALIGN_RIGHT
                                );
                }
                else
                {
                    IDISPLAY_DrawText(pMe->m_pDisplay,
                                    AEE_FONT_BOLD,
                                    w_string,
                                    -1,
                                    1,
                                    2 + ( title_hight + lineSpace) * 2,
                                    NULL,
                                    IDF_TEXT_TRANSPARENT | IDF_ALIGN_LEFT
                                );
                }

            ITIMECTL_SetRect(pMe->m_TimeCtlStart, &rc);
            }

            if(pMe->m_SetTimeRange == OEMNV_TIMEFORM_AMPM)
            {
                ITIMECTL_SetProperties(pMe->m_TimeCtlStart, TP_AUTOREDRAW|TP_NO_SECONDS);
            }
            else
            {
                ITIMECTL_SetProperties(pMe->m_TimeCtlStart, TP_AUTOREDRAW|TP_NO_SECONDS|TP_24_FORMAT);
            }

            //The 3st line
            if( language == NV_LANGUAGE_ARABIC)
            {
                SETAEERECT(&rc,  2, ( title_hight + lineSpace) * 3,pMe->m_rc.dx - CONTROL_X - 3,title_hight);
            }
            else
            {
                SETAEERECT(&rc,  CONTROL_X, (title_hight + lineSpace)*3 ,pMe->m_rc.dx - CONTROL_X - 3,title_hight);
            }
            IDISPLAY_EraseRect( pMe->m_pDisplay,&rc);
            ISHELL_LoadResString(pMe->m_pShell,AEE_APPSDISPLAYMENU_RES_FILE,IDS_STRING_TO,w_string,sizeof(w_string));

            {
                if( language == NV_LANGUAGE_ARABIC)
                {
                    IDISPLAY_DrawText(pMe->m_pDisplay,
                                    AEE_FONT_BOLD,
                                    w_string,
                                    -1,
                                    1,
                                    2 + ( title_hight + lineSpace) * 3,
                                    NULL,
                                    IDF_TEXT_TRANSPARENT | IDF_ALIGN_RIGHT
                                );
                }
                else
                {
                    IDISPLAY_DrawText(pMe->m_pDisplay,
                                    AEE_FONT_BOLD,
                                    w_string,
                                    -1,
                                    1,
                                    2 + ( title_hight + lineSpace) * 3,
                                    NULL,
                                    IDF_TEXT_TRANSPARENT | IDF_ALIGN_LEFT
                                );
                }
                ITIMECTL_SetRect( pMe->m_TimeCtlEnd, &rc);
                IDISPLAY_FillRect( pMe->m_pDisplay, &rc, RGB_WHITE);
            }

            if(pMe->m_SetTimeRange == OEMNV_TIMEFORM_AMPM)
            {
                ITIMECTL_SetProperties(pMe->m_TimeCtlStart, TP_AUTOREDRAW|TP_NO_SECONDS);
                ITIMECTL_SetProperties(pMe->m_TimeCtlEnd, TP_AUTOREDRAW|TP_NO_SECONDS);
            }
            else
            {
                ITIMECTL_SetProperties(pMe->m_TimeCtlStart, TP_AUTOREDRAW|TP_NO_SECONDS|TP_24_FORMAT);
                ITIMECTL_SetProperties(pMe->m_TimeCtlEnd, TP_AUTOREDRAW|TP_NO_SECONDS|TP_24_FORMAT);
            }
            {
                IImage      *pR_ResImg = NULL;
                IImage      *pL_ResImg = NULL;
                pL_ResImg  = ISHELL_LoadResImage(pMe->m_pShell,AEE_APPSCOMMONRES_IMAGESFILE,IDB_LEFTARROW);
                pR_ResImg = ISHELL_LoadResImage(pMe->m_pShell,AEE_APPSCOMMONRES_IMAGESFILE,IDB_RIGHTARROW);

                if(pR_ResImg != NULL)
                {
                    if( language == NV_LANGUAGE_ARABIC)
                    {
                        //在状态模式后面画该ICON,表示篌右键改变值
                        IIMAGE_Draw( pR_ResImg, CONTROL_DX -KEYPAD_IMG_DX - 1, title_hight + lineSpace + 4);
                        //      IIMAGE_Release(pR_ResImg);
                    }
                    else
                    {
                        //在状态模式后面画该ICON,表示篌右键改变值
                        IIMAGE_Draw( pR_ResImg, pMe->m_rc.dx - KEYPAD_IMG_DX -3, title_hight + lineSpace + 4);
                        //IIMAGE_Release(pR_ResImg);
                    }
                        IIMAGE_Release(pR_ResImg);
                        pR_ResImg = NULL;
                }

                if(pL_ResImg != NULL)
                {
                    if( language == NV_LANGUAGE_ARABIC)
                    {
                        //在状态模式后面画该ICON,表示篌右键改变值
                        IIMAGE_Draw( pL_ResImg, 2, title_hight + lineSpace + 4);
                        //  IIMAGE_Release(pL_ResImg);
                    }
                    else
                    {
                        //在状态模式后面画该ICON,表示篌右键改变值
                        IIMAGE_Draw( pL_ResImg,CONTROL_X, title_hight + lineSpace + 4);
                        //IIMAGE_Release(pL_ResImg);
                    }
                    IIMAGE_Release(pL_ResImg);
                    pL_ResImg = NULL;
                }

            }
            IMENUCTL_SetColors(pMe->m_pState, &color);
            DISPLAYMENU_DRAW_BOTTOMBAR(BTBAR_SAVE_CANCEL)
            if( wParam == AVK_UP || wParam == AVK_DOWN || wParam == AVK_INFO)
            {
                if( pMe->m_nCtlID == IDC_CLOCK_FORM && pMe->m_SetTimeRange == OEMNV_TIMEFORM_AMPM)
                {
                    if( ITIMECTL_GetTime( pMe->m_TimeCtlStart) >= 12*3600000)
                    {
                        DISPLAYMENU_DRAW_BOTTOMBAR( BTBAR_SAVE_AM_BACK)
                    }
                    else
                    {
                        DISPLAYMENU_DRAW_BOTTOMBAR( BTBAR_SAVE_PM_BACK)
                    }
                }
                else if(pMe->m_nCtlID == IDC_CLOCK_TO && pMe->m_SetTimeRange == OEMNV_TIMEFORM_AMPM)
                {
                    if( ITIMECTL_GetTime( pMe->m_TimeCtlEnd) >= 12*3600000)
                    {
                        DISPLAYMENU_DRAW_BOTTOMBAR( BTBAR_SAVE_AM_BACK)
                    }
                    else
                    {
                        DISPLAYMENU_DRAW_BOTTOMBAR( BTBAR_SAVE_PM_BACK)
                    }
                }

                else
                {
                    DISPLAYMENU_DRAW_BOTTOMBAR( BTBAR_SAVE_BACK)
                }
            }
            DisplayMenu_Set_CTL(pMe);
            //ClockCtl_Redraw(&pMe->m_ClockCtlStart);
            //ClockCtl_Redraw(&pMe->m_ClockCtlEnd);
            ITIMECTL_Redraw(pMe->m_TimeCtlStart);
            ITIMECTL_Redraw(pMe->m_TimeCtlEnd);
            IMENUCTL_Redraw(pMe->m_pState);
            //IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            return TRUE;
        }

        case EVT_COMMAND:
        	if(IMENUCTL_GetSel(pMe->m_pState) == IDS_STRING_ON)
            {
                pMe->m_ClockCfg.bStateOn = TRUE;
            }
            else
            {
                pMe->m_ClockCfg.bStateOn = FALSE;
            }
            pMe->m_ClockCfg.from_Time = ITIMECTL_GetTime(pMe->m_TimeCtlStart);
            pMe->m_ClockCfg.to_Time = ITIMECTL_GetTime(pMe->m_TimeCtlEnd);
            ICONFIG_SetItem(pMe->m_pConfig, CFGI_KEY_PAD_CTL, (void*)&pMe->m_ClockCfg, sizeof(Key_pad_Cfg));
            //DISP_ERR("EVT_COMMAND %d", pMe->m_nCtlID, 0, 0);
            CLOSE_DIALOG(DLGRET_CANCELED)
            return TRUE;
#if 0
            if(pMe->m_nCtlID == IDC_KEY_PAD_STATE)
            {
                pMe->m_nCtlID = IDC_CLOCK_FORM;
                ITIMECTL_SetActive(pMe->m_TimeCtlStart, TRUE);
                ITIMECTL_SetActive(pMe->m_TimeCtlEnd, FALSE);

                //ClockCtl_SetFocus(&pMe->m_ClockCtlStart, CLOCKCTL_FOCUS_HOUR);
                //ClockCtl_SetFocus(&pMe->m_ClockCtlEnd, CLOCKCTL_FOCUS_NONE);

                ITIMECTL_SetEditField(pMe->m_TimeCtlStart,ITF_HOUR);
                //ClockCtl_Redraw(&pMe->m_ClockCtlStart);
                //ClockCtl_Redraw(&pMe->m_ClockCtlEnd);

                IDIALOG_SetFocus(pMe->m_pActiveDlg, pMe->m_nCtlID);
                IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            }
#endif
            DISP_ERR("EVT_COMMAND end %d", pMe->m_nCtlID, 0, 0);
            return TRUE;



        case EVT_KEY:
            switch (wParam)
            {
            	ERR("Save time succ1jjjjjj %d %d", pMe->m_ClockCfg.from_Time, pMe->m_ClockCfg.to_Time, 0);

                case AVK_SELECT:

                    //DISP_ERR("AVK_SELECT start %d", pMe->m_nCtlID, 0, 0);
                    pMe->m_ClockCfg.from_Time = ITIMECTL_GetTime(pMe->m_TimeCtlStart);
                    pMe->m_ClockCfg.to_Time = ITIMECTL_GetTime(pMe->m_TimeCtlEnd);
                    //ClockCtl_SetTime(&pMe->m_ClockCtlStart, pMe->m_ClockCfg.from_Time);
                    //ClockCtl_SetTime(&pMe->m_ClockCtlEnd, pMe->m_ClockCfg.to_Time);

                    //if(pMe->m_nCtlID == IDC_CLOCK_TO)

                        if(IMENUCTL_GetSel(pMe->m_pState) == IDS_STRING_ON)
                        {
                            pMe->m_ClockCfg.bStateOn = TRUE;
                        }
                        else
                        {
                            pMe->m_ClockCfg.bStateOn = FALSE;
                        }

                        ERR("Save time succ1 %d %d", pMe->m_ClockCfg.from_Time, pMe->m_ClockCfg.to_Time, 0);

                        ICONFIG_SetItem(pMe->m_pConfig, CFGI_KEY_PAD_CTL, (void*)&pMe->m_ClockCfg, sizeof(Key_pad_Cfg));

                        CLOSE_DIALOG(DLGRET_CANCELED)
                        return TRUE;

                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;
            }//endswitch
            return TRUE;

        case EVT_KEY_PRESS:
#if 0
            DISP_ERR("EVT_KEY_PRESS start %d", pMe->m_nCtlID, 0, 0);
            switch (wParam)
            {
                case AVK_LEFT:
                case AVK_RIGHT:
                case AVK_UP:
                case AVK_DOWN:
                case AVK_INFO:
                    break;
                default :
                    return TRUE;
            }
            DISP_ERR("EVT_KEY_PRESS end %d", pMe->m_nCtlID, 0, 0);
            if(pMe->m_nCtlID == IDC_CLOCK_FORM)
            {
                ITIMECTL_SetActive(pMe->m_TimeCtlStart, TRUE);
                ITIMECTL_SetActive(pMe->m_TimeCtlEnd, FALSE);
                IMENUCTL_SetActive(pMe->m_pState,FALSE);
            }
            else if(pMe->m_nCtlID == IDC_CLOCK_TO)
            {
                ITIMECTL_SetActive(pMe->m_TimeCtlStart, FALSE);
                ITIMECTL_SetActive(pMe->m_TimeCtlEnd, TRUE);
                IMENUCTL_SetActive(pMe->m_pState,FALSE);
            }
            else if(pMe->m_nCtlID == IDC_KEY_PAD_STATE)
            {
                ITIMECTL_SetActive(pMe->m_TimeCtlStart, FALSE);
                ITIMECTL_SetActive(pMe->m_TimeCtlEnd, FALSE);
                IMENUCTL_SetActive(pMe->m_pState,TRUE);
            }
#endif

            {
            //传送给自定义事件EVT_FOCUS_SWITCH处理,是为了避免切换控件的同时
            //控件提前处理方向键事件
            (void) ISHELL_PostEvent( pMe->m_pShell,
                                     AEECLSID_APP_DISPLAYMENU,
                                     EVT_FOCUS_SWITCH,
                                     wParam,
                                     0);
            return TRUE;
	        }
        case EVT_FOCUS_SWITCH:
        {
            //处理Clock编辑界面应用左右键切换控件
            Display_ClockApps_HandleKeyEvent(pMe, wParam);
            if( wParam == AVK_UP || wParam == AVK_DOWN || wParam == AVK_INFO)
            {
                if( pMe->m_nCtlID == IDC_CLOCK_FORM && pMe->m_SetTimeRange == OEMNV_TIMEFORM_AMPM)
                {
                    if( ITIMECTL_GetTime( pMe->m_TimeCtlStart) >= 12*3600000)
                    {
                        DISPLAYMENU_DRAW_BOTTOMBAR( BTBAR_SAVE_AM_BACK)
                    }
                    else
                    {
                        DISPLAYMENU_DRAW_BOTTOMBAR( BTBAR_SAVE_PM_BACK)
                    }
                }
                else if(pMe->m_nCtlID == IDC_CLOCK_TO && pMe->m_SetTimeRange == OEMNV_TIMEFORM_AMPM)
                {
                    if( ITIMECTL_GetTime( pMe->m_TimeCtlEnd) >= 12*3600000)
                    {
                        DISPLAYMENU_DRAW_BOTTOMBAR( BTBAR_SAVE_AM_BACK)
                    }
                    else
                    {
                        DISPLAYMENU_DRAW_BOTTOMBAR( BTBAR_SAVE_PM_BACK)
                    }
                }

                else
                {
                    DISPLAYMENU_DRAW_BOTTOMBAR( BTBAR_SAVE_BACK)
                }
            }
            return TRUE;
        }
#if 0
        case EVT_KEY_RELEASE:
            DISP_ERR("EVT_KEY_RELEASE start %d", pMe->m_nCtlID, 0, 0);
            switch (wParam)
            {
                case AVK_LEFT:
                    if(pMe->m_nCtlID == IDC_CLOCK_FORM)
                    {
                        //ClockCtl_GetFocus(&pMe->m_ClockCtlStart,&pMe->m_focus);
                        if(pMe->m_focus == CLOCKCTL_FOCUS_HOUR)
                        {
                            pMe->m_nCtlID = IDC_KEY_PAD_STATE;
                            pMe->m_focus = CLOCKCTL_FOCUS_NONE;
                            //ClockCtl_SetFocus(&pMe->m_ClockCtlStart,CLOCKCTL_FOCUS_NONE);
                        }
                        else if(pMe->m_focus != CLOCKCTL_FOCUS_HOUR)
                        {
                            pMe->m_focus --;
                            //ClockCtl_SetFocus(&pMe->m_ClockCtlStart,pMe->m_focus);
                        }
                    }
                    else if(pMe->m_nCtlID == IDC_CLOCK_TO)
                    {
                        //ClockCtl_GetFocus(&pMe->m_ClockCtlEnd,&pMe->m_focus);
                        if(pMe->m_focus == CLOCKCTL_FOCUS_HOUR)
                        {
                            pMe->m_nCtlID = IDC_CLOCK_FORM;
                            if(pMe->m_SetTimeRange == OEMNV_TIMEFORM_AMPM)
                            {
                                pMe->m_focus = CLOCKCTL_FOCUS_AMPM;
                                //ClockCtl_SetFocus(&pMe->m_ClockCtlStart,CLOCKCTL_FOCUS_AMPM);
                            }
                            else
                            {
                                pMe->m_focus = CLOCKCTL_FOCUS_MIN;
                                //ClockCtl_SetFocus(&pMe->m_ClockCtlStart,CLOCKCTL_FOCUS_MIN);
                            }
                            //ClockCtl_SetFocus(&pMe->m_ClockCtlEnd,CLOCKCTL_FOCUS_NONE);

                        }
                        else if(pMe->m_focus != CLOCKCTL_FOCUS_HOUR)
                        {
                            pMe->m_focus --;
                            //ClockCtl_SetFocus(&pMe->m_ClockCtlEnd,pMe->m_focus);
                        }
                    }
                    else if(pMe->m_nCtlID == IDC_KEY_PAD_STATE)
                    {
                        pMe->m_nCtlID = IDC_CLOCK_TO;
                        pMe->m_focus = CLOCKCTL_FOCUS_HOUR;
                        //ClockCtl_SetFocus(&pMe->m_ClockCtlEnd,CLOCKCTL_FOCUS_HOUR);
                    }
                    break;

                case AVK_RIGHT:
                {
                    byte the_last_focus = CLOCKCTL_FOCUS_NONE;
                    if(pMe->m_SetTimeRange == OEMNV_TIMEFORM_AMPM)
                    {
                        the_last_focus = CLOCKCTL_FOCUS_AMPM;
                    }
                    else
                    {
                        the_last_focus = CLOCKCTL_FOCUS_MIN;
                    }
                    if(pMe->m_nCtlID == IDC_CLOCK_FORM)
                    {
                        //ClockCtl_GetFocus(&pMe->m_ClockCtlStart,&pMe->m_focus);
                        if(pMe->m_focus == the_last_focus)
                        {
                            pMe->m_nCtlID = IDC_CLOCK_TO;
                            pMe->m_focus = CLOCKCTL_FOCUS_HOUR;
                            //ClockCtl_SetFocus(&pMe->m_ClockCtlStart,CLOCKCTL_FOCUS_NONE);
                            //ClockCtl_SetFocus(&pMe->m_ClockCtlEnd,CLOCKCTL_FOCUS_HOUR);
                        }
                        else if(pMe->m_focus != CLOCKCTL_FOCUS_NONE)
                        {
                            pMe->m_focus ++;
                            //ClockCtl_SetFocus(&pMe->m_ClockCtlStart,pMe->m_focus);
                        }
                    }
                    else if(pMe->m_nCtlID == IDC_CLOCK_TO)
                    {
                        //ClockCtl_GetFocus(&pMe->m_ClockCtlEnd,&pMe->m_focus);
                        if(pMe->m_focus == the_last_focus)
                        {
                            pMe->m_nCtlID = IDC_KEY_PAD_STATE;
                            pMe->m_focus = CLOCKCTL_FOCUS_NONE;
                            //ClockCtl_SetFocus(&pMe->m_ClockCtlEnd,CLOCKCTL_FOCUS_NONE);
                        }
                        else if(pMe->m_focus != CLOCKCTL_FOCUS_NONE)
                        {
                            pMe->m_focus ++;
                            //ClockCtl_SetFocus(&pMe->m_ClockCtlEnd,pMe->m_focus);
                        }
                    }
                    else if(pMe->m_nCtlID == IDC_KEY_PAD_STATE)
                    {
                        pMe->m_nCtlID = IDC_CLOCK_FORM;
                        pMe->m_focus = CLOCKCTL_FOCUS_HOUR;
                        //ClockCtl_SetFocus(&pMe->m_ClockCtlStart,CLOCKCTL_FOCUS_HOUR);
                    }
                    break;
                }

                case AVK_UP:
                case AVK_DOWN:
                case AVK_INFO:
                    break;

                default:
                    return TRUE;

            }
            DISP_ERR("EVT_KEY_RELEASE end %d", pMe->m_nCtlID, 0, 0);
            IDIALOG_SetFocus(pMe->m_pActiveDlg, pMe->m_nCtlID);
            IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            return TRUE;

//        default:
//            break;
#endif
    }

    return FALSE;
} // CScheduleApp_HandleSetEventTime
#endif
#if 0
static void DisplayMenu_Draw_Softkey(CDisplayMenu *pMe,uint16 lsk,uint16 rsk)
{
    AEERect rc;
    AECHAR     wBuf[32] = {0};

    if(lsk != 0)
    {
        SETAEERECT(&rc,
                                            1,
                                            pMe->m_rc.dy - WALLPAPER_SOFT_KEY_HIGHT,
                                            (pMe->m_rc.dx-2)/2,
                                            WALLPAPER_SOFT_KEY_HIGHT);
        ISHELL_LoadResString(pMe->m_pShell,
                                            AEE_APPSDISPLAYMENU_RES_FILE,
                                            lsk,
                                            wBuf,
                                            sizeof(wBuf));

        DrawTextWithProfile(pMe->m_pShell,
                                            pMe->m_pDisplay,
                                            RGB_WHITE_NO_TRANS,
                                            AEE_FONT_NORMAL,
                                            wBuf, -1,
                                            0, 0, &rc,
                                            IDF_ALIGN_MIDDLE
                                            | IDF_ALIGN_LEFT
                                            | IDF_TEXT_TRANSPARENT);
    }

    if(rsk != 0)
    {
        SETAEERECT(&rc,
                                            (pMe->m_rc.dx-2)/2,
                                            pMe->m_rc.dy - WALLPAPER_SOFT_KEY_HIGHT,
                                            (pMe->m_rc.dx-2)/2,
                                            WALLPAPER_SOFT_KEY_HIGHT);
        ISHELL_LoadResString(pMe->m_pShell,
                                            AEE_APPSDISPLAYMENU_RES_FILE,
                                            rsk,
                                            wBuf,
                                            sizeof(wBuf));


        DrawTextWithProfile(pMe->m_pShell,
                                            pMe->m_pDisplay,
                                            RGB_WHITE_NO_TRANS,
                                            AEE_FONT_NORMAL,
                                            wBuf, -1,
                                            0, 0, &rc,
                                            IDF_ALIGN_MIDDLE
                                            | IDF_ALIGN_RIGHT
                                            | IDF_TEXT_TRANSPARENT);
    }
}
#endif
static void DisplayMenu_DialogTimeout(void *pme)
{
    CDisplayMenu *pMe = (CDisplayMenu *)pme;

    if (NULL == pMe)
    {
        return;
    }

    ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_DISPLAYMENU,  EVT_DISPLAYDIALOGTIMEOUT,  0, 0);
}
static byte DisplayMenu_Cover_Ids_To_Theme(uint16 ids)
{
    switch (ids)
    {
        case IDS_STRING_BLUE:
            return DESKTOP_THEME_BLUE;

        case IDS_STRING_GREEN:
            return DESKTOP_THEME_GREEN;

        case IDS_STRING_DEEP_BLUE:
            return DESKTOP_THEME_DEEP_BLUE;

        case IDS_STRING_ORANGE:
            return DESKTOP_THEME_ORANGE;

        case IDS_STRING_RED:
            return DESKTOP_THEME_RED;

        case IDS_STRING_YELLOW:
            return DESKTOP_THEME_YELLOW;

        default:
            return DESKTOP_THEME_ORANGE;
    }
}
/****************************************************************/
#ifdef FEATRUE_KEY_PAD_CTL
/*==============================================================================
函数：
       Display_ClockApps_HandleKeyEvent

说明：
       处理Clock编辑界面应用左右键切换控件.

参数：
       pMe [in]：指向Applet对象结构的指针。该结构包含小程序的特定信息。
       wParam[in]: 按键代码

返回值：
       boolean。

备注：:

==============================================================================*/
static boolean Display_ClockApps_HandleKeyEvent(CDisplayMenu *pMe, uint16 wParam)
{
    if (pMe == NULL)
    {
        return FALSE;
    }

    switch(wParam)
    {
        case AVK_DOWN:
        	if(pMe->m_nCtlID == IDC_CLOCK_TO)
            {
                pMe->m_nCtlID = IDC_KEY_PAD_STATE;
                pMe->m_nNumKeyCount = 0;    //重置标志
                pMe->m_nTimeCtlCount  = 0;
            }
            else if(pMe->m_nCtlID == IDC_KEY_PAD_STATE)    //状态
            {
                //时间控件不显示秒项,且active时更新
                ITIMECTL_SetProperties(pMe->m_TimeCtlStart,
                                       TP_NO_SECONDS | TP_AUTOREDRAW);
                //编辑区域设置为小时
                ITIMECTL_SetEditField(pMe->m_TimeCtlStart, ITF_HOUR);

                pMe->m_nCtlID = IDC_CLOCK_FORM;
            }
            else if(pMe->m_nCtlID == IDC_CLOCK_FORM)/*up*/   //时间设置
            {
                //时间控件不显示秒项,且active时更新
                ITIMECTL_SetProperties(pMe->m_TimeCtlEnd,
                                       TP_NO_SECONDS | TP_AUTOREDRAW);
                //编辑区域设置为小时
                ITIMECTL_SetEditField(pMe->m_TimeCtlEnd, ITF_HOUR);

                pMe->m_nCtlID = IDC_CLOCK_TO;
            }

            else
            {
                pMe->m_nCtlID--;
            }
            DisplayMenu_Set_CTL(pMe);
            return TRUE;

        case AVK_UP:
            if(pMe->m_nCtlID == IDC_KEY_PAD_STATE)
            {
                //时间控件不显示秒项,且active时更新
                ITIMECTL_SetProperties(pMe->m_TimeCtlEnd, TP_NO_SECONDS | TP_AUTOREDRAW);
                //编辑区域设置为分钟
                ITIMECTL_SetEditField(pMe->m_TimeCtlEnd, ITF_HOUR);

                pMe->m_nCtlID = IDC_CLOCK_TO;
            }
            else if(pMe->m_nCtlID == IDC_CLOCK_TO) //重复方式
            {

                //时间控件不显示秒项,且active时更新
                ITIMECTL_SetProperties(pMe->m_TimeCtlStart, TP_NO_SECONDS | TP_AUTOREDRAW);
                //编辑区域设置为分钟
                ITIMECTL_SetEditField(pMe->m_TimeCtlStart, ITF_HOUR);

                pMe->m_nCtlID = IDC_CLOCK_FORM;
            }
            else if(pMe->m_nCtlID == IDC_CLOCK_FORM)   //时间设置
            {
                pMe->m_nCtlID = IDC_KEY_PAD_STATE;
                pMe->m_nNumKeyCount = 0;    //重置标志
                pMe->m_nTimeCtlCount = 0;
            }
            else
            {
                pMe->m_nCtlID ++;
            }
            DisplayMenu_Set_CTL(pMe);
            return TRUE;

        case AVK_RIGHT:
        case AVK_LEFT:
            if((pMe->m_nCtlID == IDC_CLOCK_FORM) || (pMe->m_nCtlID == IDC_CLOCK_TO))   //时间控件
            {
                if(pMe->m_nTimeCtlCount == 0)
                {
                    pMe->m_nTimeCtlCount = 1;
                    pMe->m_nNumKeyCount = 2;
                }
                else if(pMe->m_nTimeCtlCount == 1)
                {
                    pMe->m_nTimeCtlCount = 0;
                    pMe->m_nNumKeyCount = 0;
                }
            }
            return TRUE;
        default:
          break;
        }
        return TRUE;
}
#endif


#ifdef FEATRUE_KEY_PAD_CTL

static void DisplayMenu_Set_CTL(CDisplayMenu *pMe)
{
    //设置控件焦点
    if (pMe->m_nCtlID == IDC_KEY_PAD_STATE)
    {
        IMENUCTL_SetActive(pMe->m_pState, TRUE);
        ITIMECTL_SetActive(pMe->m_TimeCtlStart, FALSE);
        ITIMECTL_SetActive(pMe->m_TimeCtlEnd,FALSE);
    }
    else if (pMe->m_nCtlID == IDC_CLOCK_FORM)
    {
        IMENUCTL_SetActive(pMe->m_pState, FALSE);
        ITIMECTL_SetActive(pMe->m_TimeCtlStart, TRUE);
        ITIMECTL_SetActive(pMe->m_TimeCtlEnd, FALSE);
    }
    else if (pMe->m_nCtlID == IDC_CLOCK_TO)
    {
        IMENUCTL_SetActive(pMe->m_pState, FALSE);
        ITIMECTL_SetActive(pMe->m_TimeCtlStart, FALSE);
        ITIMECTL_SetActive(pMe->m_TimeCtlEnd, TRUE);
    }
}
#endif
/****************************************************************/

#ifdef FEATURE_MENU_STYLE
static boolean HandleMenuStyleDialogEvent(CDisplayMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    PARAM_NOT_REF(dwParam)

    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,
                                                    IDC_MENU_STYLE);
    if(pMenu == NULL)
    {
        return FALSE;
    }

    DisplayMenu_AutoScroll(pMenu,eCode,wParam);
    switch(eCode)
    {
        case EVT_DIALOG_INIT:
        {
			//add by yangdecai
			{
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_APPSDISPLAYMENU_RES_FILE,                                
                        IDS_MENU_STYLE,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldTextEx(pMe->m_pIAnn,WTitle,FALSE);
            }
            IMENUCTL_AddItem(pMenu, AEE_APPSDISPLAYMENU_RES_FILE, IDS_MENU_ICON, IDS_MENU_ICON, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSDISPLAYMENU_RES_FILE, IDS_MENU_LIST, IDS_MENU_LIST, NULL, 0);
        }
            return TRUE;

        case EVT_DIALOG_START:
        {
            byte      menu_style = OEMNV_MENU_STYLE_ICON;
            uint16    ui16_return = IDS_MENU_ICON;

           (void) ICONFIG_GetItem(pMe->m_pConfig,
                                       CFGI_MENU_STYLE,
                                       &menu_style,
                                       sizeof(menu_style));


            switch(menu_style)
            {
                case OEMNV_MENU_STYLE_ICON:      //menu icon
                    ui16_return = IDS_MENU_ICON;
                    break;

                case OEMNV_MENU_STYLE_LIST:      //menu list
                    ui16_return = IDS_MENU_LIST;

                default:
                    //ui16_return = IDS_MENU_ICON;
                    break;
            }

            InitMenuIcons(pMenu);
            SetMenuIcon(pMenu, ui16_return, TRUE);
            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_TEXT_ALIGN_LEFT_ICON_ALIGN_RIGHT|MP_ACTIVE_NO_REDRAW);
            IMENUCTL_SetOemProperties( pMenu, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SETTING_BACKGROUND);
#endif
            IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
            IMENUCTL_SetSel(pMenu,ui16_return);
            return TRUE;
        }

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;
                default:
                    break;
            }
            return TRUE;

        case EVT_COMMAND:
        {
            byte menu_style = OEMNV_MENU_STYLE_ICON;

            switch (wParam)
            {
                case IDS_MENU_ICON:
                    menu_style = OEMNV_MENU_STYLE_ICON;
                    break;
                case IDS_MENU_LIST:
                    menu_style = OEMNV_MENU_STYLE_LIST;
                    break;
                default:
                    break;
            }

            (void) ICONFIG_SetItem(pMe->m_pConfig,
                                       CFGI_MENU_STYLE,
                                       &menu_style,
                                       sizeof(menu_style));

            InitMenuIcons(pMenu);
            SetMenuIcon(pMenu, wParam, TRUE);
            pMe->m_msg_id = IDS_DONE;
            CLOSE_DIALOG(DLGRET_MSG_POP);
            return TRUE;
        }
        default:
            break;
    }
        return FALSE;
}
#endif

#ifdef FEATURE_RANDOM_MENU_COLOR
/*==============================================================================
函数：
       HandleColorfulMenuDialogEvent
说明：
       IDD_COLORFUL_MENU对话框事件处理函数

参数：
       pMe [in]：指向DisplayMenu Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/
static boolean  HandleColorfulMenuDialogEvent(CDisplayMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    PARAM_NOT_REF(dwParam)
    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,
                                                    IDC_COLORFUL_MENU);
    if (pMenu == NULL)
    {
        return FALSE;
    }

    DISP_ERR("%x, %x ,%x,HandleColorfulMenuDialogEvent",eCode,wParam,dwParam);

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            return TRUE;

        case EVT_DIALOG_START:
        {
            IMENUCTL_AddItem(pMenu, AEE_APPSDISPLAYMENU_RES_FILE, IDS_PRESET_STYLE, IDS_PRESET_STYLE, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSDISPLAYMENU_RES_FILE, IDS_CUSTOM_STYLE, IDS_CUSTOM_STYLE, NULL, 0);

            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY|MP_ACTIVE_NO_REDRAW);
            IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SETTING_BACKGROUND);
#endif
            IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
            IMENUCTL_SetSel(pMenu, pMe->m_nSubDlgId);

            (void) ISHELL_PostEvent( pMe->m_pShell,
                                             AEECLSID_APP_DISPLAYMENU,
                                             EVT_USER_REDRAW,
                                             0,
                                             0);
            return TRUE;
        }

        case EVT_USER_REDRAW:
            //(void)IMENUCTL_Redraw(pMenu);   //dele by yangdecai
            return TRUE;

        case EVT_DIALOG_END:
            if(pMe->m_bSuspending)
            {
                pMe->m_nSubDlgId= IMENUCTL_GetSel(pMenu);
            }
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                    pMe->m_nSubDlgId = 0;
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    break;

                default:
                    break;
            }
            return TRUE;

        case EVT_COMMAND:
        {
            pMe->m_nSubDlgId= wParam;
            switch (wParam)
            {
                case IDS_PRESET_STYLE:
                    pMe->m_bCustomColor = FALSE;
                    CLOSE_DIALOG(DISPLAYMENUST_COLOR_SETTING)
                    break;

                case IDS_CUSTOM_STYLE:
                    pMe->m_bCustomColor = TRUE;
                    (void)OEM_GetConfig(CFGI_MENU_BGCOLOR, (void*)&pMe->m_nCFGColor, sizeof(pMe->m_nCFGColor));
                    CLOSE_DIALOG(DISPLAYMENUST_COLOR_SETTING)
                    break;

                default:
                    ASSERT_NOT_REACHABLE;
            }
        }
        return TRUE;

        default:
            break;
    }
    return FALSE;
}


/*==============================================================================
函数：
       HandleColorSettingDialogEvent
说明：
       IDD_COLOR_SETTING对话框事件处理函数

参数：
       pMe [in]：指向DisplayMenu Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/
static boolean  HandleColorSettingDialogEvent(CDisplayMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    PARAM_NOT_REF(dwParam)

    static int nSelectVal;
    static byte nOldRandMenu;
    static RGBVAL nOldCFGColor;
    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,
                                                    IDC_COLOR_SETTING);

    if (pMenu == NULL)
    {
        return FALSE;
    }

    DISP_ERR("%x, %x ,%x,HandleColorSettingDialogEvent",eCode,wParam,dwParam);

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            nSelectVal = 0;
            return TRUE;

        case EVT_DIALOG_START:
        {
            (void)OEM_GetConfig(CFGI_MENU_BGCOLOR, (void*)&nOldCFGColor, sizeof(nOldCFGColor));
            (void)OEM_GetConfig(CFGI_RANDOM_MENU, (void*)&nOldRandMenu, sizeof(nOldRandMenu));
            if(pMe->m_bCustomColor)
            {
                IMENUCTL_SetProperties(pMenu, MP_NO_REDRAW);
                IMENUCTL_SetActive(pMenu, FALSE);
            }
            else
            {
                IMENUCTL_AddItem(pMenu, AEE_APPSDISPLAYMENU_RES_FILE, IDS_STRING_NORMAL, IDS_STRING_NORMAL, NULL, 0);
                IMENUCTL_AddItem(pMenu, AEE_APPSDISPLAYMENU_RES_FILE, IDS_STRING_BLUE, IDS_STRING_BLUE, NULL, 0);
                IMENUCTL_AddItem(pMenu, AEE_APPSDISPLAYMENU_RES_FILE, IDS_STRING_COFFEE, IDS_STRING_COFFEE, NULL, 0);
                IMENUCTL_AddItem(pMenu, AEE_APPSDISPLAYMENU_RES_FILE, IDS_STRING_GREEN, IDS_STRING_GREEN, NULL, 0);
                IMENUCTL_AddItem(pMenu, AEE_APPSDISPLAYMENU_RES_FILE, IDS_STRING_LEMON, IDS_STRING_LEMON, NULL, 0);
                IMENUCTL_AddItem(pMenu, AEE_APPSDISPLAYMENU_RES_FILE, IDS_STRING_PURPLE, IDS_STRING_PURPLE, NULL, 0);
                IMENUCTL_AddItem(pMenu, AEE_APPSDISPLAYMENU_RES_FILE, IDS_STRING_RANDOM, IDS_STRING_RANDOM, NULL, 0);

                InitMenuIcons(pMenu);
                if(nOldRandMenu == 2)
                {
                    SetMenuIcon(pMenu, IDS_STRING_RANDOM, TRUE);
                }
                else if(nOldRandMenu == 1)
                {
                    uint16 nSelectID = 0;

                    switch(nOldCFGColor)
                    {
                        case DISPLAYMENU_COLOR_BLUE:
                            nSelectID = IDS_STRING_BLUE;
                            break;

                        case DISPLAYMENU_COLOR_COFFEE:
                            nSelectID = IDS_STRING_COFFEE;
                            break;

                        case DISPLAYMENU_COLOR_GREEN:
                            nSelectID = IDS_STRING_GREEN;
                            break;

                        case DISPLAYMENU_COLOR_LEMON:
                            nSelectID = IDS_STRING_LEMON;
                            break;

                        case DISPLAYMENU_COLOR_PURPLE:
                            nSelectID = IDS_STRING_PURPLE;
                            break;

                        default:
                            break;
                    }
                    if(nSelectID != 0)
                    {
                        SetMenuIcon(pMenu, nSelectID, TRUE);
                    }
                }
                else
                {
                    SetMenuIcon(pMenu, IDS_STRING_NORMAL, TRUE);
                }

                IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_TEXT_ALIGN_LEFT_ICON_ALIGN_RIGHT|MP_ACTIVE_NO_REDRAW);
                IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SETTING_BACKGROUND);
#endif
                IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
                IMENUCTL_SetSel(pMenu, pMe->m_nExSubDlgId);
            }

            (void) ISHELL_PostEvent( pMe->m_pShell,
                                             AEECLSID_APP_DISPLAYMENU,
                                             EVT_USER_REDRAW,
                                             0,
                                             0);
            return TRUE;
        }

        case EVT_USER_REDRAW:
            if(!pMe->m_bCustomColor)
            {
                IANNUNCIATOR_Redraw(pMe->m_pIAnn);
                (void)IMENUCTL_Redraw(pMenu);
            }
            else
            {
                int i = 0, nPixelsToEdge = 15;
                uint32 nTempVal;
                byte nRandomMenu = 1;
                RGBVAL oldFontColor;
                AEERect rect;
                AECHAR wszTitle[16] = {0};
                AECHAR wszText[8] = {0};
                AECHAR wszTemp[4] = {L"%d"};
                TitleBar_Param_type titleParam = {0};

                (void)OEM_SetConfig(CFGI_MENU_BGCOLOR, (void*)&pMe->m_nCFGColor, sizeof(pMe->m_nCFGColor));
                (void)OEM_SetConfig(CFGI_RANDOM_MENU, (void*)&nRandomMenu, sizeof(nRandomMenu));

                Appscommon_ResetBackgroundEx(pMe->m_pDisplay, &pMe->m_rc, TRUE);

                ISHELL_LoadResString(pMe->m_pShell, AEE_APPSDISPLAYMENU_RES_FILE, IDS_CUSTOM_STYLE, wszTitle, sizeof(wszTitle));

                titleParam.dwAlignFlags = IDF_TEXT_TRANSPARENT | IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE;
                titleParam.pwszTitle = wszTitle;
                DrawTitleBar(pMe->m_pDisplay,&titleParam);

                oldFontColor = IDISPLAY_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, RGB_WHITE);
                SETAEERECT(&rect, pMe->m_rc.x + nPixelsToEdge,
                                            pMe->m_rc.y + TITLEBAR_HEIGHT + MENUITEM_HEIGHT,
                                            pMe->m_rc.dx/2,
                                            MENUITEM_HEIGHT - 2);

                for(i = 0; i <= 2; i++)
                {
                    MEMSET(&wszText, 0, sizeof(wszText));
                    ISHELL_LoadResString(pMe->m_pShell, AEE_APPSDISPLAYMENU_RES_FILE,
                                                IDS_RGB_RED + i, wszText, sizeof(wszText));
                    (void) IDISPLAY_DrawText(pMe->m_pDisplay,
                                AEE_FONT_BOLD,
                                wszText,
                                -1,
                                0,
                                0,
                                &rect,
                                IDF_ALIGN_BOTTOM | IDF_ALIGN_LEFT | IDF_TEXT_TRANSPARENT);
                    rect.x += pMe->m_rc.dx/3;
                    IDISPLAY_FillRect(pMe->m_pDisplay, &rect, RGB_WHITE);
                    rect.x -= pMe->m_rc.dx/3;
                    rect.y += MENUITEM_HEIGHT;
                }

                rect.x = pMe->m_rc.x + nPixelsToEdge + pMe->m_rc.dx/2;
                rect.y = pMe->m_rc.y + TITLEBAR_HEIGHT + MENUITEM_HEIGHT;
                for(i = 0; i <= 2; i++)
                {
                    nTempVal =  (uint32) ((pMe->m_nCFGColor >> ((i + 1)*8)) & (0x000000FF));
                    MEMSET(&wszText, 0, sizeof(wszText));
                    WSPRINTF(wszText, sizeof(wszText), wszTemp, nTempVal);

                    (void)IDISPLAY_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, (nSelectVal == i)?(oldFontColor):(MAKE_RGB(0x84, 0x84, 0x84)));
                    if(nSelectVal == i)
                    {
                        int nLen = IDISPLAY_MeasureText(pMe->m_pDisplay, AEE_FONT_BOLD, wszText);
                        AEERect rc = {0};

                        SETAEERECT(&rc, rect.x + nLen, rect.y + (rect.dy - 14)/2, 1, 14);
                        IDISPLAY_FillRect(pMe->m_pDisplay, &rc, (nSelectVal == i)?(oldFontColor):(MAKE_RGB(0x84, 0x84, 0x84)));

                        if(nTempVal == 0)
                        {
                            DISPLAYMENU_DRAW_BOTTOMBAR(BTBAR_OK_BACK)
                        }
                        else
                        {
                            DISPLAYMENU_DRAW_BOTTOMBAR(BTBAR_OK_DELETE)
                        }
                    }
                    (void) IDISPLAY_DrawText(pMe->m_pDisplay,
                                AEE_FONT_BOLD,
                                wszText,
                                -1,
                                0,
                                0,
                                &rect,
                                IDF_ALIGN_BOTTOM | IDF_ALIGN_LEFT| IDF_TEXT_TRANSPARENT);
                    rect.y += MENUITEM_HEIGHT;
                }

                (void)IDISPLAY_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, oldFontColor);

                IANNUNCIATOR_Redraw(pMe->m_pIAnn);
                IDISPLAY_Update(pMe->m_pDisplay);
            }
            return TRUE;

        case EVT_DIALOG_END:
            if(pMe->m_bSuspending)
            {
                pMe->m_nExSubDlgId= IMENUCTL_GetSel(pMenu);
            }
            else
            {
                pMe->m_nExSubDlgId = 0;
            }
            (void)OEM_SetConfig(CFGI_MENU_BGCOLOR, (void*)&nOldCFGColor, sizeof(nOldCFGColor));
            (void)OEM_SetConfig(CFGI_RANDOM_MENU, (void*)&nOldRandMenu, sizeof(nOldRandMenu));
            IANNUNCIATOR_Redraw(pMe->m_pIAnn);
            return TRUE;

        case EVT_KEY_PRESS:
            switch(wParam)
            {
                case AVK_UP:
                    if(pMe->m_bCustomColor)
                    {
                        nSelectVal = (nSelectVal > 0)?(nSelectVal - 1):(2);
                        (void) ISHELL_PostEvent( pMe->m_pShell,
                                                         AEECLSID_APP_DISPLAYMENU,
                                                         EVT_USER_REDRAW,
                                                         0,
                                                         0);
                    }
                    break;

                case AVK_DOWN:
                    if(pMe->m_bCustomColor)
                    {
                        nSelectVal = (nSelectVal < 2)?(nSelectVal + 1):(0);
                        (void) ISHELL_PostEvent( pMe->m_pShell,
                                                         AEECLSID_APP_DISPLAYMENU,
                                                         EVT_USER_REDRAW,
                                                         0,
                                                         0);
                    }
                    break;

                default:
                    break;
            }
            return TRUE;

        case EVT_KEY:
            {
                switch(wParam)
                {
                    case AVK_CLR:
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
                        if(!pMe->m_bCustomColor)
                        {
                            if(AVK_CLR == wParam)
                            {
                                CLOSE_DIALOG(DLGRET_CANCELED)
                            }
                            return TRUE;
                        }
                        else
                        {
                            uint32 nColorVal = (uint32)(pMe->m_nCFGColor>>((nSelectVal + 1)*8) & 0x000000FF);

                            if(AVK_CLR == wParam)
                            {
                                if(nColorVal == 0)
                                {
                                    CLOSE_DIALOG(DLGRET_CANCELED);
                                    return TRUE;
                                }
                                else
                                {
                                    nColorVal /= 10;
                                }
                            }
                            else
                            {
                                nColorVal = MIN(255, (10*nColorVal + (wParam - AVK_0)));
                            }
                            pMe->m_nCFGColor = (RGBVAL)(pMe->m_nCFGColor & (~((0x000000FF) << (nSelectVal + 1)*8)) | ((nColorVal) << (nSelectVal + 1)*8));

                            (void) ISHELL_PostEvent( pMe->m_pShell,
                                                             AEECLSID_APP_DISPLAYMENU,
                                                             EVT_USER_REDRAW,
                                                             0,
                                                             0);
                        }
                        break;

                    case AVK_INFO:
                    case AVK_SELECT:
                        if(pMe->m_bCustomColor)
                        {
                            nOldRandMenu = 1;
                            nOldCFGColor = pMe->m_nCFGColor;
                            pMe->m_msg_id = IDS_DONE;
                            CLOSE_DIALOG(DLGRET_MSG_POP);
                        }
                        break;

                    default:
                        break;
                }
            }
            return TRUE;

        case EVT_COMMAND:
        {
            nOldRandMenu = 1;
            InitMenuIcons(pMenu);
            SetMenuIcon(pMenu, wParam, TRUE);

            switch (wParam)
            {
                case IDS_STRING_NORMAL:
                    nOldRandMenu = 0;
                    nOldCFGColor = APPSCOMMON_BG_COLOR;
                    break;

                case IDS_STRING_BLUE:
                    nOldCFGColor = DISPLAYMENU_COLOR_BLUE;
                    break;

                case IDS_STRING_COFFEE:
                    nOldCFGColor = DISPLAYMENU_COLOR_COFFEE;
                    break;

                case IDS_STRING_GREEN:
                    nOldCFGColor = DISPLAYMENU_COLOR_GREEN;
                    break;

                case IDS_STRING_LEMON:
                    nOldCFGColor = DISPLAYMENU_COLOR_LEMON;
                    break;

                case IDS_STRING_PURPLE:
                    nOldCFGColor = DISPLAYMENU_COLOR_PURPLE;
                    break;

                case IDS_STRING_RANDOM:
                    nOldRandMenu = 2;
                    (void)OEM_GetConfig(CFGI_MENU_BGCOLOR, (void*)&nOldCFGColor, sizeof(nOldCFGColor));
                    break;

                default:
                    ASSERT_NOT_REACHABLE;
            }

            pMe->m_msg_id = IDS_DONE;
            CLOSE_DIALOG(DLGRET_MSG_POP);
        }
        return TRUE;
        case EVT_CTL_SEL_CHANGED:
        {
            byte nRandomMenu = nOldRandMenu;
            RGBVAL nTempColor = nOldCFGColor;

            switch (wParam)
            {
                case IDS_STRING_NORMAL:
                    nRandomMenu = 0;
                    nTempColor = APPSCOMMON_BG_COLOR;
                    break;

                case IDS_STRING_BLUE:
                    nRandomMenu = 1;
                    nTempColor = DISPLAYMENU_COLOR_BLUE;
                    break;

                case IDS_STRING_COFFEE:
                    nRandomMenu = 1;
                    nTempColor = DISPLAYMENU_COLOR_COFFEE;
                    break;

                case IDS_STRING_GREEN:
                    nRandomMenu = 1;
                    nTempColor = DISPLAYMENU_COLOR_GREEN;
                    break;

                case IDS_STRING_LEMON:
                    nRandomMenu = 1;
                    nTempColor = DISPLAYMENU_COLOR_LEMON;
                    break;

                case IDS_STRING_PURPLE:
                    nRandomMenu = 1;
                    nTempColor = DISPLAYMENU_COLOR_PURPLE;
                    break;

                default:
                    break;
            }
            (void)OEM_SetConfig(CFGI_MENU_BGCOLOR, (void*)&nTempColor, sizeof(nTempColor));
            (void)OEM_SetConfig(CFGI_RANDOM_MENU, (void*)&nRandomMenu, sizeof(nRandomMenu));
            (void) ISHELL_PostEvent( pMe->m_pShell,
                                             AEECLSID_APP_DISPLAYMENU,
                                             EVT_USER_REDRAW,
                                             0,
                                             0);
        }
        return TRUE;
        default:
            break;
    }
    return FALSE;
}
#endif

#ifdef FEATURE_RANDOM_MENU_REND//wlh 20090405 add for REND
/*==============================================================================
函数：
       HandleRENDMenuDialogEvent
说明：
       IDC_REND_MENU对话框事件处理函数

参数：
       pMe [in]：指向DisplayMenu Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/
static boolean  HandleRENDMenuDialogEvent(CDisplayMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    PARAM_NOT_REF(dwParam)

    static int nSelectVal;
    static byte nOldRendState;
    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,
                                                    IDC_REND_MENU);

    if (pMenu == NULL)
    {
        return FALSE;
    }

    DISP_ERR("%x, %x ,%x,HandleColorSettingDialogEvent",eCode,wParam,dwParam); 

    switch (eCode) 
    {
        case EVT_DIALOG_INIT:
            nSelectVal = 0;
            return TRUE;

        case EVT_DIALOG_START:
        {
            {
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_APPSDISPLAYMENU_RES_FILE,                                
                        IDS_REND_MENU_TITLE,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldTextEx(pMe->m_pIAnn,WTitle,FALSE);
            }
			
            (void)OEM_GetConfig(CFGI_REND_STATE, (void*)&nOldRendState, sizeof(nOldRendState));
			#ifdef FEATURE_VERSION_K212
			IMENUCTL_AddItem(pMenu, AEE_APPSDISPLAYMENU_RES_FILE, IDS_REND_EFFECT_3D, IDS_REND_EFFECT_3D, NULL, 0);
			#else
            IMENUCTL_AddItem(pMenu, AEE_APPSDISPLAYMENU_RES_FILE, IDS_REND_EFFECT_FADE, IDS_REND_EFFECT_FADE, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSDISPLAYMENU_RES_FILE, IDS_REND_EFFECT_SHOW_HORZ, IDS_REND_EFFECT_SHOW_HORZ, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSDISPLAYMENU_RES_FILE, IDS_REND_EFFECT_SHOW_VERT, IDS_REND_EFFECT_SHOW_VERT, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSDISPLAYMENU_RES_FILE, IDS_REND_EFFECT_MOVE_HORZ, IDS_REND_EFFECT_MOVE_HORZ, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSDISPLAYMENU_RES_FILE, IDS_REND_EFFECT_MOVE_VERT, IDS_REND_EFFECT_MOVE_VERT, NULL, 0);
            #if !defined(FEATURE_VERSION_W516) && !defined(FEATURE_VERSION_W208S) && !defined(FEATURE_VERSION_W027)
            IMENUCTL_AddItem(pMenu, AEE_APPSDISPLAYMENU_RES_FILE, IDS_REND_EFFECT_FLY_HORZ, IDS_REND_EFFECT_FLY_HORZ, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSDISPLAYMENU_RES_FILE, IDS_REND_EFFECT_FLY_VERT, IDS_REND_EFFECT_FLY_VERT, NULL, 0);
            #endif
            IMENUCTL_AddItem(pMenu, AEE_APPSDISPLAYMENU_RES_FILE, IDS_REND_EFFECT_ROTAT_HORZ, IDS_REND_EFFECT_ROTAT_HORZ, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSDISPLAYMENU_RES_FILE, IDS_REND_EFFECT_ROTAT_VERT, IDS_REND_EFFECT_ROTAT_VERT, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSDISPLAYMENU_RES_FILE, IDS_REND_EFFECT_BLINDS_VERT, IDS_REND_EFFECT_BLINDS_VERT, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSDISPLAYMENU_RES_FILE, IDS_REND_EFFECT_BLINDS_INTER, IDS_REND_EFFECT_BLINDS_INTER, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSDISPLAYMENU_RES_FILE, IDS_STRING_RANDOM, IDS_STRING_RANDOM, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSDISPLAYMENU_RES_FILE, IDS_REND_EFFECT_ONEBYONE, IDS_REND_EFFECT_ONEBYONE, NULL, 0);
			#endif
			IMENUCTL_AddItem(pMenu, AEE_APPSDISPLAYMENU_RES_FILE, IDS_ALWAYS_OFF, IDS_ALWAYS_OFF, NULL, 0);

            InitMenuIcons(pMenu);
            switch(nOldRendState){
            case DISPLAYREND_TYPE_FADE:
                SetMenuIcon(pMenu, IDS_REND_EFFECT_FADE, TRUE);
                break;
            case DISPLAYREND_TYPE_SHOW_HORZ:
                SetMenuIcon(pMenu, IDS_REND_EFFECT_SHOW_HORZ, TRUE);
                break;
            case DISPLAYREND_TYPE_SHOW_VERT:
                SetMenuIcon(pMenu, IDS_REND_EFFECT_SHOW_VERT, TRUE);
                break;
            case DISPLAYREND_TYPE_MOVE_HORZ:
                SetMenuIcon(pMenu, IDS_REND_EFFECT_MOVE_HORZ, TRUE);
                break;
            case DISPLAYREND_TYPE_MOVE_VERT:
                SetMenuIcon(pMenu, IDS_REND_EFFECT_MOVE_VERT, TRUE);
                break;
            #if !defined(FEATURE_VERSION_W516) && !defined(FEATURE_VERSION_W208S) && !defined(FEATURE_VERSION_W027)
            case DISPLAYREND_TYPE_FLY_HORZ:
                SetMenuIcon(pMenu, IDS_REND_EFFECT_FLY_HORZ, TRUE);
                break; 
            case DISPLAYREND_TYPE_FLY_VERT:
                SetMenuIcon(pMenu, IDS_REND_EFFECT_FLY_VERT, TRUE);
                break;
            #endif    
            case DISPLAYREND_TYPE_ROTAT_HORZ:
				#ifdef FEATURE_VERSION_K212
				SetMenuIcon(pMenu, IDS_REND_EFFECT_3D, TRUE);
				#else
                SetMenuIcon(pMenu, IDS_REND_EFFECT_ROTAT_HORZ, TRUE);
				#endif
                break;
            case DISPLAYREND_TYPE_ROTAT_VERT:
                SetMenuIcon(pMenu, IDS_REND_EFFECT_ROTAT_VERT, TRUE);
                break;
            case DISPLAYREND_TYPE_BLINDS_VERT:
                SetMenuIcon(pMenu, IDS_REND_EFFECT_BLINDS_VERT, TRUE);
                break;
            case DISPLAYREND_TYPE_BLINDS_INTER:
                SetMenuIcon(pMenu, IDS_REND_EFFECT_BLINDS_INTER, TRUE);
                break;
            case DISPLAYREND_TYPE_RAND:
                SetMenuIcon(pMenu, IDS_STRING_RANDOM, TRUE);
                break;
            case DISPLAYREND_TYPE_ONEBYONE:
                SetMenuIcon(pMenu, IDS_REND_EFFECT_ONEBYONE, TRUE);
                break;
            case DISPLAYREND_TYPE_MAX:
            default:
                SetMenuIcon(pMenu, IDS_ALWAYS_OFF, TRUE);
                break;
            }

            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_TEXT_ALIGN_LEFT_ICON_ALIGN_RIGHT|MP_ACTIVE_NO_REDRAW);
            IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);
            IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
            IMENUCTL_SetSel(pMenu, pMe->m_nExSubDlgId);
            return TRUE;
        }

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    break;

                default:
                    break;
            }
            return TRUE;
        
        case EVT_COMMAND:
            InitMenuIcons(pMenu);
            SetMenuIcon(pMenu, wParam, TRUE);
            
            switch(wParam){
            case IDS_REND_EFFECT_FADE:
                nOldRendState = DISPLAYREND_TYPE_FADE;
                break;
            case IDS_REND_EFFECT_SHOW_HORZ:
                nOldRendState = DISPLAYREND_TYPE_SHOW_HORZ;
                break;
            case IDS_REND_EFFECT_SHOW_VERT:
                nOldRendState = DISPLAYREND_TYPE_SHOW_VERT;
                break;
            case IDS_REND_EFFECT_MOVE_HORZ:
                nOldRendState = DISPLAYREND_TYPE_MOVE_HORZ;
                break;
            case IDS_REND_EFFECT_MOVE_VERT:
                nOldRendState = DISPLAYREND_TYPE_MOVE_VERT;
                break;
            #if !defined(FEATURE_VERSION_W516) && !defined(FEATURE_VERSION_W208S) && !defined(FEATURE_VERSION_W027)
            case IDS_REND_EFFECT_FLY_HORZ:
                nOldRendState = DISPLAYREND_TYPE_FLY_HORZ;
                break;
            case IDS_REND_EFFECT_FLY_VERT:
                nOldRendState = DISPLAYREND_TYPE_FLY_VERT;
                break;
            #endif    
			case IDS_REND_EFFECT_3D:
            case IDS_REND_EFFECT_ROTAT_HORZ:
                nOldRendState = DISPLAYREND_TYPE_ROTAT_HORZ;
                break;
            case IDS_REND_EFFECT_ROTAT_VERT:
                nOldRendState = DISPLAYREND_TYPE_ROTAT_VERT;
                break;
            case IDS_REND_EFFECT_BLINDS_VERT:
                nOldRendState = DISPLAYREND_TYPE_BLINDS_VERT;
                break;
            case IDS_REND_EFFECT_BLINDS_INTER:
                nOldRendState = DISPLAYREND_TYPE_BLINDS_INTER;
                break;
            case IDS_STRING_RANDOM:
                nOldRendState = DISPLAYREND_TYPE_RAND;
                break;
            case IDS_REND_EFFECT_ONEBYONE:
                nOldRendState = DISPLAYREND_TYPE_ONEBYONE;
                break;
            case IDS_ALWAYS_OFF:
            default:
                nOldRendState = DISPLAYREND_TYPE_MAX;
                break;
            }
            (void)OEM_SetConfig(CFGI_REND_STATE, (void*)&nOldRendState, sizeof(nOldRendState));
            pMe->m_msg_id = IDS_DONE;
            CLOSE_DIALOG(DLGRET_MSG_POP);
            return TRUE;

        default:
            break;
    }
    return FALSE;
} 
#endif//FEATURE_RANDOM_MENU_REND


/*==============================================================================
// 文件：
//        DisplayMenuStateHandler.c
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

/*==============================================================================
                                 宏定义和常数
==============================================================================*/


/*==============================================================================
                                 类型定义
==============================================================================*/


/*==============================================================================
                                 函数声明
==============================================================================*/
// 状态 DISPLAYMENUST_NONE 处理函数
static NextFSMAction Display_StateNoOperationHandler(CDisplayMenu *pMe);

// 状态 DISPLAYMENUST_INIT 处理函数
static NextFSMAction Display_StateInitHandler(CDisplayMenu *pMe);

// 状态 DISPLAYMENUST_MAIN 处理函数
static NextFSMAction Display_StateMainHandler(CDisplayMenu *pMe);

// 状态 DISPLAYMENUST_PICTURE 处理函数
static NextFSMAction Display_StatePictureHandler(CDisplayMenu *pMe);

// 状态 DISPLAYMENUST_IMGDELMSG 处理函数
static NextFSMAction Display_StateImgDelMsgHandler(CDisplayMenu *pMe);

// 状态 DISPLAYMENUST_BACKLIGHTING 处理函数
static NextFSMAction Display_StatebacklightingHandler(CDisplayMenu *pMe);

// 状态 DISPLAYMENUST_CONTRAST 处理函数
static NextFSMAction Display_StateContrastHandler(CDisplayMenu *pMe);
#ifdef FEATURE_SCREEN_SAVE
// 状态 DISPLAYMENUST_SCREENTIMEOUT 处理函数
static NextFSMAction Display_StateScreenTimeoutHandler(CDisplayMenu *pMe);
#endif
// 状态 DISPLAYMENUST_EXIT 处理函数
static NextFSMAction Display_StateExitHandler(CDisplayMenu *pMe);

static NextFSMAction Display_StateDeskTopthemeHandler(CDisplayMenu *pMe);
#ifdef FEATURE_LED_CONTROL
static NextFSMAction Display_StateLED_ControlHandler(CDisplayMenu *pMe);
#endif
#ifdef FEATRUE_KEY_PAD_CTL
static NextFSMAction Display_StateKeyPad_Control_Handler(CDisplayMenu *pMe);
#endif
#ifdef FEATURE_MENU_STYLE
static NextFSMAction  Display_StateMenuStyle_Control_Handler(CDisplayMenu *pMe);
#endif

#ifdef FEATURE_RANDOM_MENU_COLOR
static NextFSMAction  Display_StateColorfulMenu_Control_Handler(CDisplayMenu *pMe);

static NextFSMAction  Display_StateColorSetting_Handler(CDisplayMenu *pMe);
#endif

#ifdef FEATURE_RANDOM_MENU_REND//wlh 20090405 add for rend
static NextFSMAction  Display_StateRendMenu_Control_Handler(CDisplayMenu *pMe); 
#endif
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
       DisplayMenu_ProcessState
说明:
       DisplayMenu Applet状态处理主函数。函数根据pMe->m_currState将调用路由至
       相应的处理函数。

参数:
       pMe [In]: 指向DisplayMenu Applet对象结构的指针,该结构包含小程序的特定信息.

返回值:
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注:

==============================================================================*/
NextFSMAction DisplayMenu_ProcessState(CDisplayMenu *pMe)
{
    NextFSMAction retVal = NFSMACTION_WAIT;

    if (NULL == pMe)
    {
        return retVal;
    }

    // 根据当前状态调用相应的状态处理函数
    switch (pMe->m_eCurState)
    {
        case DISPLAYMENUST_NONE:
            retVal = Display_StateNoOperationHandler(pMe);
            break;

        case DISPLAYMENUST_INIT:
            retVal = Display_StateInitHandler(pMe);
            break;

        case DISPLAYMENUST_MAIN:
            retVal = Display_StateMainHandler(pMe);
            break;

        case DISPLAYMENUST_BACKLIGHTING:
            retVal = Display_StatebacklightingHandler(pMe);
            break;

        case DISPLAYMENUST_CONTRAST:
            retVal = Display_StateContrastHandler(pMe);
            break;
#ifdef FEATURE_SCREEN_SAVE
        case DISPLAYMENUST_SCREENTIMEOUT:
            retVal = Display_StateScreenTimeoutHandler(pMe);
            break;
#endif
        case DISPLAYMENUST_PICTURE:
            retVal = Display_StatePictureHandler(pMe);
            break;

        case DISPLAYMENUST_IMGDELMSG:
            retVal = Display_StateImgDelMsgHandler(pMe);
            break;

        case DISPLAYMENUST_DESKTOP_THEME:
            retVal = Display_StateDeskTopthemeHandler(pMe);
            break;

#ifdef FEATURE_LED_CONTROL
        case DISPLAYMENUST_LED_CONTROL:
            retVal = Display_StateLED_ControlHandler(pMe);
            break;
#endif

#ifdef FEATRUE_KEY_PAD_CTL
        case DISPLAYMENUST_KEY_PAD_CONTROL:
            retVal = Display_StateKeyPad_Control_Handler(pMe);
            break;
#endif

#ifdef  FEATURE_MENU_STYLE
       case DISPLAYMENUST_MENU_STYLE_CONTROL:
            retVal = Display_StateMenuStyle_Control_Handler(pMe);
            break;
#endif
#ifdef FEATURE_RANDOM_MENU_COLOR
        case DISPLAYMENUST_COLORFUL_MENU:
            retVal = Display_StateColorfulMenu_Control_Handler(pMe);
            break;

        case DISPLAYMENUST_COLOR_SETTING:
            retVal = Display_StateColorSetting_Handler(pMe);
            break;
#endif
#ifdef FEATURE_RANDOM_MENU_REND//wlh 20090405 add for rend
        case DISPLAYMENUST_REND_MENU:
            retVal = Display_StateRendMenu_Control_Handler(pMe);
            break;
#endif        
        case DISPLAYMENUST_EXIT:
            retVal = Display_StateExitHandler(pMe);
            break;

        default:
            break;
    }
    return retVal;
}

/*==============================================================================
函数：
       StateNoOperationHandler
说明：
       DISPLAYMENUST_NONE 状态处理函数

参数：
       pMe [in]：指向DisplayMenu Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/
static NextFSMAction Display_StateNoOperationHandler(CDisplayMenu *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    MOVE_TO_STATE(DISPLAYMENUST_EXIT)
    return NFSMACTION_CONTINUE;
} // StateNoOperationHandler


/*==============================================================================
函数：
       StateInitHandler
说明：
       DISPLAYMENUST_INIT 状态处理函数

参数：
       pMe [in]：指向DisplayMenu Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/
static NextFSMAction Display_StateInitHandler(CDisplayMenu *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    MOVE_TO_STATE(DISPLAYMENUST_MAIN)
    return NFSMACTION_CONTINUE;
} // StateInitHandler


/*==============================================================================
函数：
       StateMainHandler
说明：
       DISPLAYMENUST_MAIN 状态处理函数

参数：
       pMe [in]：指向DisplayMenu Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/
static NextFSMAction Display_StateMainHandler(CDisplayMenu *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    //DISP_ERR("%d StateMainHandler", pMe->m_eDlgRet, 0, 0);
    pMe->m_nSubDlgId = 0;    //重新计算子菜单选择条的位置

    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            DisplayMenu_ShowDialog(pMe, IDD_DISPLAYMENU_MENU);
            return NFSMACTION_WAIT;

        case DLGRET_BACKLIGHTING:
            MOVE_TO_STATE(DISPLAYMENUST_BACKLIGHTING)
            return NFSMACTION_CONTINUE;

        case DLGRET_CONTRAST:
            MOVE_TO_STATE(DISPLAYMENUST_CONTRAST)
            return NFSMACTION_CONTINUE;
#ifdef FEATURE_SCREEN_SAVE
        case DLGRET_SCREENTIMEOUT:
            MOVE_TO_STATE(DISPLAYMENUST_SCREENTIMEOUT)
            return NFSMACTION_CONTINUE;
#endif
        case DLGRET_PICTURE:
            MOVE_TO_STATE(DISPLAYMENUST_PICTURE)
            return NFSMACTION_CONTINUE;

        case DLGRET_IMGDELMSG:
            MOVE_TO_STATE(DISPLAYMENUST_IMGDELMSG)
            return NFSMACTION_CONTINUE;

        case DLGRET_CANCELED:
            MOVE_TO_STATE(DISPLAYMENUST_EXIT)
            return NFSMACTION_CONTINUE;

        case DLGRET_DESKTOP_THEME:
            MOVE_TO_STATE(DISPLAYMENUST_DESKTOP_THEME)
            return NFSMACTION_CONTINUE;
#ifdef FEATURE_LED_CONTROL
        case DLGRET_LED_CONTROL:
            MOVE_TO_STATE(DISPLAYMENUST_LED_CONTROL)
            return NFSMACTION_CONTINUE;
#endif

#ifdef FEATRUE_KEY_PAD_CTL
        case DLGRET_KEY_PAD_CTL:
            MOVE_TO_STATE(DISPLAYMENUST_KEY_PAD_CONTROL)
            return NFSMACTION_CONTINUE;
#endif
#ifdef FEATURE_MENU_STYLE
        case DLGRET_MENU_STYLE_CTL:
            MOVE_TO_STATE(DISPLAYMENUST_MENU_STYLE_CONTROL)
            return NFSMACTION_CONTINUE;           
#endif

#ifdef FEATURE_RANDOM_MENU_COLOR
        case DLGRET_COLORFUL_MENU_CTL:
            MOVE_TO_STATE(DISPLAYMENUST_COLORFUL_MENU)
            return NFSMACTION_CONTINUE;
#endif

#ifdef FEATURE_RANDOM_MENU_REND//wlh 20090405 add for rend
        case DLGRET_REND_MENU_CTL:
            MOVE_TO_STATE(DISPLAYMENUST_REND_MENU)
            return NFSMACTION_CONTINUE;
#endif
        default:
            ASSERT_NOT_REACHABLE;
    }

    return NFSMACTION_WAIT;
} // StateMainHandler


/*==============================================================================
函数：
       StatePictureHandler
说明：
       DISPLAYMENUST_PICTURE 状态处理函数

参数：
       pMe [in]：指向DisplayMenu Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/
static NextFSMAction Display_StatePictureHandler(CDisplayMenu *pMe)
{
    MSG_FATAL("Display_StatePictureHandler Start",0,0,0);
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    //DISP_ERR("%d StatePictureHandler", pMe->m_eDlgRet, 0, 0);
    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            DisplayMenu_ShowDialog(pMe, IDD_PICTURE_DLG);
            return NFSMACTION_WAIT;

        case DLGRET_IMGDELMSG:
            MOVE_TO_STATE(DISPLAYMENUST_IMGDELMSG)
            return NFSMACTION_CONTINUE;
			
		case DLGRET_MSG_POP:
			pMe->m_bNotOverwriteDlgRet = FALSE;
            DisplayMenu_ShowDialog(pMe, IDD_IMGDELMSG_DLG);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
            MOVE_TO_STATE(DISPLAYMENUST_MAIN)
            return NFSMACTION_CONTINUE;

        default:
            ASSERT_NOT_REACHABLE;
    }
    MSG_FATAL("Display_StatePictureHandler End",0,0,0);
    return NFSMACTION_WAIT;
} // StatePictureHandler


/*==============================================================================
函数：
       StateImgDelMsgHandler
说明：
       DISPLAYMENUST_IMGDELMSG 状态处理函数

参数：
       pMe [in]：指向DisplayMenu Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/
static NextFSMAction Display_StateImgDelMsgHandler(CDisplayMenu *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    //DISP_ERR("%d StateImgDelMsgHandler", pMe->m_eDlgRet, 0, 0);
    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            pMe->m_msg_id = IDS_DEL_ITEM_YESNO;
            DisplayMenu_ShowDialog(pMe, IDD_IMGDELMSG_DLG);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
            MOVE_TO_STATE(DISPLAYMENUST_PICTURE)
            return NFSMACTION_CONTINUE;

        default:
            ASSERT_NOT_REACHABLE;
    }

    return NFSMACTION_WAIT;
} // StateImgDelMsgHandler

/*==============================================================================
函数：
       StatebacklightingHandler
说明：
       DISPLAYMENUST_BACKLIGHTING 状态处理函数

参数：
       pMe [in]：指向DisplayMenu Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/
static NextFSMAction Display_StatebacklightingHandler(CDisplayMenu *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    //DISP_ERR("%d StatebacklightingHandler", pMe->m_eDlgRet, 0, 0);
    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            DisplayMenu_ShowDialog(pMe, IDD_BACKLIGHTING_MENU);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
            MOVE_TO_STATE(DISPLAYMENUST_MAIN)
            return NFSMACTION_CONTINUE;

        case DLGRET_MSG_POP:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            DisplayMenu_ShowDialog(pMe, IDD_IMGDELMSG_DLG);
            return NFSMACTION_WAIT;

        default:
            ASSERT_NOT_REACHABLE;
    }

    return NFSMACTION_WAIT;
} // StatebacklightingHandler


/*==============================================================================
函数：
       StateContrastHandler
说明：
       DISPLAYMENUST_CONTRAST 状态处理函数

参数：
       pMe [in]：指向DisplayMenu Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/
static NextFSMAction Display_StateContrastHandler(CDisplayMenu *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    //DISP_ERR("%d StateContrastHandler", pMe->m_eDlgRet, 0, 0);
    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            DisplayMenu_ShowDialog(pMe, IDD_CONTRAST_MENU);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
            MOVE_TO_STATE(DISPLAYMENUST_MAIN)
            return NFSMACTION_CONTINUE;

        case DLGRET_MSG_POP:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            DisplayMenu_ShowDialog(pMe, IDD_IMGDELMSG_DLG);
            return NFSMACTION_WAIT;

        default:
            ASSERT_NOT_REACHABLE;
    }

    return NFSMACTION_WAIT;
} // StateContrastHandler

#ifdef FEATURE_SCREEN_SAVE
/*==============================================================================
函数：
       StateScreenTimeoutHandler
说明：
       DISPLAYMENUST_SCREENTIMEOUT 状态处理函数

参数：
       pMe [in]：指向DisplayMenu Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/
static NextFSMAction Display_StateScreenTimeoutHandler(CDisplayMenu *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    //DISP_ERR("%d StateScreenTimeoutHandler", pMe->m_eDlgRet, 0, 0);
    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            DisplayMenu_ShowDialog(pMe, IDD_SCREENSAVERTIMEOUT_DIALOG);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
            MOVE_TO_STATE(DISPLAYMENUST_MAIN)
            return NFSMACTION_CONTINUE;

        default:
            ASSERT_NOT_REACHABLE;
    }

    return NFSMACTION_WAIT;
} // StateScreenTimeoutHandler

#endif
/*==============================================================================
函数：
       StateExitHandler
说明：
       DISPLAYMENUST_EXIT 状态处理函数

参数：
       pMe [in]：指向DisplayMenu Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/
static NextFSMAction Display_StateExitHandler(CDisplayMenu *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    (void) ISHELL_CloseApplet(pMe->m_pShell, FALSE);

    return NFSMACTION_WAIT;
} // StateExitHandler



/*==============================================================================
函数：
       StateDeskTopthemeHandler
说明：
       DISPLAYMENUST_DESKTOP_THEME 状态处理函数

参数：
       pMe [in]：指向DisplayMenu Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/
static NextFSMAction Display_StateDeskTopthemeHandler(CDisplayMenu *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    //DISP_ERR("%d StateDeskTopthemeHandler", pMe->m_eDlgRet, 0, 0);
    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            DisplayMenu_ShowDialog(pMe, IDD_DESKTOP_THEME);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
            MOVE_TO_STATE(DISPLAYMENUST_MAIN)
            return NFSMACTION_CONTINUE;

        default:
            ASSERT_NOT_REACHABLE;
    }

    return NFSMACTION_WAIT;
} // StateImgDelMsgHandler
#ifdef FEATURE_LED_CONTROL
static NextFSMAction Display_StateLED_ControlHandler(CDisplayMenu *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    //DISP_ERR("%d StateLED_ControlHandler", pMe->m_eDlgRet, 0, 0);
    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            DisplayMenu_ShowDialog(pMe, IDD_LED_CONTROL);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
            MOVE_TO_STATE(DISPLAYMENUST_MAIN)
            return NFSMACTION_CONTINUE;

        default:
            ASSERT_NOT_REACHABLE;
    }

    return NFSMACTION_WAIT;

}
#endif

#ifdef FEATRUE_KEY_PAD_CTL
static NextFSMAction Display_StateKeyPad_Control_Handler(CDisplayMenu *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    //DISP_ERR("%d Display_StateKeyPad_Control_Handler", pMe->m_eDlgRet, 0, 0);
    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            DisplayMenu_ShowDialog(pMe, IDD_KEY_PAD_CTL);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
            MOVE_TO_STATE(DISPLAYMENUST_MAIN)
            return NFSMACTION_CONTINUE;

        case DLGRET_MSG_POP:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            pMe->m_msg_id = IDS_KEYPAD_TIME_ERR ;
            DisplayMenu_ShowDialog(pMe, IDD_IMGDELMSG_DLG);
            return NFSMACTION_WAIT;

        default:
            ASSERT_NOT_REACHABLE;
    }

    return NFSMACTION_WAIT;

}
#endif

#ifdef FEATURE_MENU_STYLE
/*==============================================================================
函数：
       StateMenustyletHandler
说明：
       DISPLAYMENUST_CONTRAST 状态处理函数

参数：
       pMe [in]：指向DisplayMenu Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/
static NextFSMAction  Display_StateMenuStyle_Control_Handler(CDisplayMenu *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    DISP_ERR("%d State StateMenuStyle change", pMe->m_eDlgRet, 0, 0);
    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            DisplayMenu_ShowDialog(pMe, IDD_MENU_STYLE);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
            MOVE_TO_STATE(DISPLAYMENUST_MAIN)
            return NFSMACTION_CONTINUE;

        case DLGRET_MSG_POP:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            DisplayMenu_ShowDialog(pMe, IDD_IMGDELMSG_DLG);
            return NFSMACTION_WAIT;

        default:
            ASSERT_NOT_REACHABLE;
    }

    return NFSMACTION_WAIT;
} 
#endif

#ifdef FEATURE_RANDOM_MENU_COLOR
/*==============================================================================
函数：
       Display_StateColorfulMenu_Control_Handler
说明：
       DISPLAYMENUST_COLORFUL_MENU 状态处理函数

参数：
       pMe [in]：指向DisplayMenu Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/
static NextFSMAction  Display_StateColorfulMenu_Control_Handler(CDisplayMenu *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    
    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            DisplayMenu_ShowDialog(pMe, IDD_COLORFUL_MENU);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
            MOVE_TO_STATE(DISPLAYMENUST_MAIN)
            return NFSMACTION_CONTINUE;
            
        case DLGRET_COLOR_SETTING:
            MOVE_TO_STATE(DISPLAYMENUST_COLOR_SETTING)
            return NFSMACTION_CONTINUE;

        default:
            ASSERT_NOT_REACHABLE;
    }

    return NFSMACTION_WAIT;
} 


/*==============================================================================
函数：
       Display_StateColorSetting_Handler
说明：
       DISPLAYMENUST_COLOR_SETTING 状态处理函数

参数：
       pMe [in]：指向DisplayMenu Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/
static NextFSMAction  Display_StateColorSetting_Handler(CDisplayMenu *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    
    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            DisplayMenu_ShowDialog(pMe, IDD_COLOR_SETTING);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
            MOVE_TO_STATE(DISPLAYMENUST_COLORFUL_MENU)
            return NFSMACTION_CONTINUE;

        case DLGRET_MSG_POP:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            DisplayMenu_ShowDialog(pMe, IDD_IMGDELMSG_DLG);
            return NFSMACTION_WAIT;
            
        default:
            ASSERT_NOT_REACHABLE;
    }

    return NFSMACTION_WAIT;
}
#endif

#ifdef FEATURE_RANDOM_MENU_REND//wlh 20090405 add for rend
/*==============================================================================
函数：
       Display_StateRendMenu_Control_Handler
说明：
       DISPLAYMENUST_REND_MENU 状态处理函数

参数：
       pMe [in]：指向DisplayMenu Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/
static NextFSMAction  Display_StateRendMenu_Control_Handler(CDisplayMenu *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    
    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            DisplayMenu_ShowDialog(pMe, IDD_REND_MENU);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
            MOVE_TO_STATE(DISPLAYMENUST_MAIN)
            return NFSMACTION_CONTINUE;
            
		case DLGRET_MSG_POP:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            DisplayMenu_ShowDialog(pMe, IDD_IMGDELMSG_DLG);
            return NFSMACTION_WAIT;
            
        default:
            ASSERT_NOT_REACHABLE;
    }

    return NFSMACTION_WAIT;
} 
#endif//FEATURE_RANDOM_MENU_REND

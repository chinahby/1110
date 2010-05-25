/*==============================================================================
// 文件：
//        UTKStateHandler.c
//        2007-11-13 陈喜玲草拟版本(Draft Version)
//        版权所有(c) 2007 Anylook
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
// 状态 UTKST_NONE 处理函数
static NextFSMAction UTKST_NONE_Handler(CUTK *pMe);

// 状态 UTKST_INIT 处理函数
static NextFSMAction UTKST_INIT_Handler(CUTK *pMe);

#ifdef FEATURE_CARRIER_CHINA_TELCOM
static NextFSMAction UTKST_SPLASH_Handler(CUTK *pMe);
#endif //FEATURE_CARRIER_CHINA_TELCOM

// 状态 UTKST_MAIN 处理函数
static NextFSMAction UTKST_MAIN_Handler(CUTK *pMe);

// 状态 UTKST_LIST 处理函数
static NextFSMAction UTKST_LIST_Handler(CUTK *pMe);

// 状态 UTKST_INPUT 处理函数
static NextFSMAction UTKST_INPUT_Handler(CUTK *pMe);

// 状态 UTKST_DISPLAY 处理函数
static NextFSMAction UTKST_DISPLAY_Handler(CUTK *pMe);

// 状态 UTKST_SENDMSG 处理函数
static NextFSMAction UTKST_SENDMSG_Handler(CUTK *pMe);

// 状态 UTKST_PLAYTONE 处理函数
static NextFSMAction UTKST_PLAYTONE_Handler(CUTK *pMe);

// 状态 UTKST_MSG 处理函数
static NextFSMAction UTKST_MSG_Handler(CUTK *pMe);

// 状态 UTKST_MSG 处理函数
static NextFSMAction UTKST_POPMSG_Handler(CUTK *pMe);

// 状态 UTKST_EXIT 处理函数
static NextFSMAction UTKST_EXIT_Handler(CUTK *pMe);

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
       UTK_ProcessState
说明:
       UTK Applet状态处理主函数。函数根据pMe->m_currState将调用路由至
       相应的处理函数。

参数:
       pMe [In]: 指向UTK Applet对象结构的指针,该结构包含小程序的特定信息.

返回值:
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注:

==============================================================================*/
NextFSMAction UTK_ProcessState(CUTK *pMe)
{
    NextFSMAction retVal = NFSMACTION_WAIT;

    if (NULL == pMe)
    {
        return retVal;
    }

    // 根据当前状态调用相应的状态处理函数
    switch (pMe->m_eCurState)
    {
        case UTKST_NONE:
            return UTKST_NONE_Handler(pMe);
            
        case UTKST_INIT:
            return UTKST_INIT_Handler(pMe);
            
#ifdef FEATURE_CARRIER_CHINA_TELCOM        
        case UTKST_SPLASH:
            return UTKST_SPLASH_Handler(pMe);
#endif //FEATURE_CARRIER_CHINA_TELCOM            
            
        case UTKST_MAIN:
            return UTKST_MAIN_Handler(pMe);
            
        case UTKST_LIST:
            return UTKST_LIST_Handler(pMe);
            
        case UTKST_INPUT:
            return UTKST_INPUT_Handler(pMe);
            
        case UTKST_DISPLAY:
            return UTKST_DISPLAY_Handler(pMe);
            
        case UTKST_SENDMSG:
            return UTKST_SENDMSG_Handler(pMe);
             
        case UTKST_PLAYTONE:    
            return UTKST_PLAYTONE_Handler(pMe);     
                        
        case UTKST_MSG:    
            return UTKST_MSG_Handler(pMe);   

        case UTKST_POPMSG:    
            return UTKST_POPMSG_Handler(pMe);              
            
        case UTKST_EXIT:
            return UTKST_EXIT_Handler(pMe);

        default:
            break;
    }
    return retVal;
}

/*==============================================================================
函数：
       UTKST_NONE_Handler
说明：
       UTKST_NONE 状态处理函数
       
参数：
       pMe [in]：指向UTK Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。
       
备注：
       
==============================================================================*/
static NextFSMAction UTKST_NONE_Handler(CUTK *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    MOVE_TO_STATE(UTKST_EXIT);
    return NFSMACTION_CONTINUE;
} // UTKST_NONE_Handler


/*==============================================================================
函数：
       UTKST_INIT_Handler
说明：
       UTKST_INIT 状态处理函数
       
参数：
       pMe [in]：指向UTK Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。
       
备注：
       
==============================================================================*/
static NextFSMAction UTKST_INIT_Handler(CUTK *pMe)
{

//#ifdef FEATURE_CARRIER_CHINA_TELCOM    
    //MOVE_TO_STATE(UTKST_SPLASH)
//#endif //FEATURE_CARRIER_CHINA_TELCOM    
    MOVE_TO_STATE(UTKST_MAIN);
    return NFSMACTION_CONTINUE;
} // UTKST_INIT_Handler

#ifdef FEATURE_CARRIER_CHINA_TELCOM
/*==============================================================================
函数：
    UTKST_SPLASH_Handler
    
说明：
    UTKST_SPLASH 状态处理函数
       
参数：
    pMe [in]：指向UTK Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值：
    NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
    NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。
       
备注：
    视运营商需要确定是否显示过渡闪屏。若不需闪屏，状态直接跳到主界面状态即可。       
==============================================================================*/
static NextFSMAction UTKST_SPLASH_Handler(CUTK *pMe)
{
    NextFSMAction  eRet = NFSMACTION_WAIT;
    
    if (NULL == pMe)
    {
        return eRet;
    }
    
    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            {
                boolean  bNeedSplash = FALSE;
                
#ifdef FEATURE_CARRIER_CHINA_TELCOM
                bNeedSplash = TRUE;
#endif
                if (bNeedSplash)
                {
                    UTK_ShowDialog(pMe, IDD_SPLASH_DIALOG);
                }
                else
                {
                    MOVE_TO_STATE(UTKST_MAIN);
                    eRet = NFSMACTION_CONTINUE;
                }
            }
            return eRet;
        
        case DLGRET_CANCELED:
            MOVE_TO_STATE(UTKST_MAIN);
            return NFSMACTION_CONTINUE;
        
        default:
            break;
    }

    return eRet;
}
#endif //FEATURE_CARRIER_CHINA_TELCOM

/*==============================================================================
函数：
       UTKST_MAIN_Handler
说明：
       UTKST_MAIN 状态处理函数
       
参数：
       pMe [in]：指向UTK Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。
       
备注：
       
==============================================================================*/
static NextFSMAction UTKST_MAIN_Handler(CUTK *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    
    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            UTK_ShowDialog(pMe, IDD_MAIN_DIALOG);
            return NFSMACTION_WAIT;
        
        case DLGRET_CANCELED:
            pMe->m_bNormalExit = TRUE;            
            MOVE_TO_STATE(UTKST_EXIT);
            return NFSMACTION_CONTINUE;

        case DLGRET_POPMSG:
            pMe->m_eMsgRetState = UTKST_EXIT;   
            pMe->m_wMsgID = IDS_NONEUTK;     
            pMe->m_MsgTimeout = TIMEOUT_MS_MSGBOX;            
            MOVE_TO_STATE(UTKST_POPMSG);
            return NFSMACTION_CONTINUE;            
      
        default:
            break;
    }

    return NFSMACTION_WAIT;
} // UTKST_MAIN_Handler


/*==============================================================================
函数：
       UTKST_LIST_Handler
说明：
       UTKST_LIST 状态处理函数
       
参数：
       pMe [in]：指向UTK Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。
       
备注：
       
==============================================================================*/
static NextFSMAction UTKST_LIST_Handler(CUTK *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    
    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            UTK_ShowDialog(pMe, IDD_LIST_DIALOG);
            return NFSMACTION_WAIT;
        
        default:
            break;
    }

    return NFSMACTION_WAIT;
} // UTKST_LIST_Handler


/*==============================================================================
函数：
       UTKST_INPUT_Handler
说明：
       UTKST_INPUT 状态处理函数
       
参数：
       pMe [in]：指向UTK Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。
       
备注：
       
==============================================================================*/
static NextFSMAction UTKST_INPUT_Handler(CUTK *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    
    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            UTK_ShowDialog(pMe, IDD_INPUT_DIALOG);
            return NFSMACTION_WAIT;
                   
        case DLGRET_OK:   
            pMe->m_eMsgRetState = UTKST_INPUT;   
            pMe->m_wMsgID = IDS_GETINPUT_WRONG_LEN;     
            pMe->m_MsgTimeout = TIMEOUT_MS_MSGBOX;            
            MOVE_TO_STATE(UTKST_MSG);
            return NFSMACTION_CONTINUE;  
                                                              
        default:
            break;
    }

    return NFSMACTION_WAIT;
} // UTKST_INPUT_Handler


/*==============================================================================
函数：
       UTKST_DISPLAY_Handler
说明：
       UTKST_DISPLAY 状态处理函数
       
参数：
       pMe [in]：指向UTK Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。
       
备注：
       
==============================================================================*/
static NextFSMAction UTKST_DISPLAY_Handler(CUTK *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    
    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            UTK_ShowDialog(pMe, IDD_DISPLAY_DIALOG);
            return NFSMACTION_WAIT;
        
        default:
            break;
    }

    return NFSMACTION_WAIT;
} // UTKST_DISPLAY_Handler


/*==============================================================================
函数：
       UTKST_EXIT_Handler
说明：
       UTKST_EXIT 状态处理函数
       
参数：
       pMe [in]：指向UTK Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。
       
备注：
       
==============================================================================*/
static NextFSMAction UTKST_EXIT_Handler(CUTK *pMe)
{
    (void) ISHELL_CloseApplet(pMe->m_pShell, FALSE);

    return NFSMACTION_WAIT;
} // UTKST_EXIT_Handler

/*==============================================================================
函数：
       UTKST_SENDMSG_Handler

说明：
        UTKST_SENDMSG 状态处理函数。

参数：
       pMe [in]：指向SMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示不停状态机。
       NFSMACTION_WAIT：指示停止状态机。

备注：
       
==============================================================================*/
static NextFSMAction UTKST_SENDMSG_Handler(CUTK *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgRet)
    {
        // 显示发送消息状态提示界面
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            UTK_ShowDialog(pMe, IDD_SENDMSG_DIALOG);
            return NFSMACTION_WAIT;

        default:
            break;
    }

    return NFSMACTION_WAIT;
} // UTKST_SENDMSG_Handler

/*==============================================================================
函数：
       UTKST_PLAYTONE_Handler
说明：
       UTKST_PLAYTONE 状态处理函数
       
参数：
       pMe [in]：指向UTK Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。
       
备注：
       
==============================================================================*/
static NextFSMAction UTKST_PLAYTONE_Handler(CUTK *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    
    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            UTK_ShowDialog(pMe, IDD_PLAYTONE_DIALOG);
            return NFSMACTION_WAIT; 
        
        default:
            break;
    }

    return NFSMACTION_WAIT;
} // UTKST_PLAYTONE_Handler

/*==============================================================================
函数：
       UTKST_MSG_Handler
说明：
       UTKST_MSG 状态处理函数
       
参数：
       pMe [in]：指向ContApp Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。
       
备注：
       
==============================================================================*/
static NextFSMAction UTKST_MSG_Handler(CUTK *pMe)
{
#if defined(AEE_STATIC)
    ASSERT(pMe != NULL);
#endif
    
    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            // Show dialog
            pMe->m_bNotOverwriteDlgRet = FALSE;
            UTK_ShowDialog(pMe, IDD_MSGBOX);
            return NFSMACTION_WAIT;
            
        case DLGRET_MSGBOX_OK:
            MOVE_TO_STATE(pMe->m_eMsgRetState);
            return NFSMACTION_CONTINUE;
            
        default:
#if defined(AEE_STATIC)
            ASSERT_NOT_REACHABLE
#endif
            break;
    }
    
    return NFSMACTION_CONTINUE;
} // UTKST_MSG_Handler

/*==============================================================================
函数：
       UTKST_MSG_Handler
说明：
       UTKST_MSG 状态处理函数
       
参数：
       pMe [in]：指向ContApp Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。
       
备注：
       
==============================================================================*/
static NextFSMAction UTKST_POPMSG_Handler(CUTK *pMe)
{
#if defined(AEE_STATIC)
    ASSERT(pMe != NULL);
#endif
    
    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            // Show dialog
            pMe->m_bNotOverwriteDlgRet = FALSE;
            UTK_ShowDialog(pMe, IDD_POPMSG);
            return NFSMACTION_WAIT;
            
        case DLGRET_MSGBOX_OK:
            MOVE_TO_STATE(pMe->m_eMsgRetState);
            return NFSMACTION_CONTINUE;
            
        default:
#if defined(AEE_STATIC)
            ASSERT_NOT_REACHABLE
#endif
            break;
    }
    
    return NFSMACTION_CONTINUE;
} // UTKST_MSG_Handler


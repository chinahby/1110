/*==============================================================================
// 文件：
//        fmRadioStateHandler.c
//        2007-11-11 Gemsea草拟版本(Draft Version)
//        版权所有(c) 2007 Anylook.,LTD.
//        
// 文件说明：
//        
// 作者：2007-11-11
// 创建日期：Gemsea
// 当前版本：Draft
==============================================================================*/

/*==============================================================================
修改历史记录：
时       间      修 改 人     修改内容、位置及原因
-----------      ----------     -----------------------------------------------
2007-11-11              初始版本
==============================================================================*/


/*==============================================================================
                                 本文件包含的外部文件
==============================================================================*/
#include "FmRadio_priv.h"
#include "Appscommon.h"
#include "appscommon.brh"
#include "appscommonimages.brh"
#include "fmradiols.brh"

/*==============================================================================
                                 宏定义和常数
==============================================================================*/


/*==============================================================================
                                 类型定义
==============================================================================*/


/*==============================================================================
                                 函数声明
==============================================================================*/
// 状态 FMRADIOST_NONE 处理函数
static NextFSMAction FmRadioStateNoOperationHandler(CFmRadio *pMe);

// 状态 FMRADIOST_INIT 处理函数
static NextFSMAction FmRadioStateInitHandler(CFmRadio *pMe);

// 状态 FMRADIOST_USER_MSG 处理函数
static NextFSMAction FmRadioStateUserMsgHandler(CFmRadio *pMe);

// 状态 FMRADIOST_MAIN 处理函数
static NextFSMAction FmRadioStateMainHandler(CFmRadio *pMe);

// 状态 FMRADIOST_OPTION 处理函数
static NextFSMAction FmRadioStateOptionHandler(CFmRadio *pMe);

// 状态 FMRADIOST_LIST 处理函数
static NextFSMAction FmRadioStateListHandler(CFmRadio *pMe);

// 状态 FMRADIOST_EXIT 处理函数
static NextFSMAction FmRadioStateExitHandler(CFmRadio *pMe);

/*==============================================================================
                                 全局数据
==============================================================================*/


/*==============================================================================
                                 本地（静态）数据
==============================================================================*/


/*==============================================================================
                                 函数定义
==============================================================================*/
int  FmRadio_ShowMsgBoxDialog( CFmRadio *pMe, 
                               char*       pszResFile,
                               uint16      msgResId,
                               uint16      dlgResId,                               
                               AECHAR     *msg, 
                               int         msglen)
{
     IStatic* pIStatic = NULL;
     AECHAR szText[MSGBOX_MAXTEXTLEN];
     PromptMsg_Param_type MsgParam={0};

     if(SUCCESS != ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_STATIC,
                                         (void **)&pIStatic))
     {        
        return FALSE;
     }

     /*Set the static control text based on the user settings of the
      * m_nMsgBoxTextId parameter.*/
     ISHELL_LoadResString(pMe->m_pShell,
           pszResFile,  //AEE_APPSCOMMONRES_LANGFILE,
           msgResId,
           szText,
           sizeof(szText));
     MsgParam.ePMsgType = MESSAGE_CONFIRM;
     MsgParam.pwszMsg = szText;
     MsgParam.eBBarType = BTBAR_OK_CANCEL;

    //Add By zzg 2013_09_03
     if ((msgResId == IDS_FMRADIO_CHANNEL_EDIT_TURE) || (msgResId == IDS_FMRADIO_CHANNEL_EDIT_ERROR))
     {
        MsgParam.ePMsgType = MESSAGE_INFORMATION;
        MsgParam.eBBarType = BTBAR_BACK;

        if (msgResId == IDS_FMRADIO_CHANNEL_EDIT_TURE)
        {
            pMe->bCurrect = TRUE;           
        }
        else
        {
            pMe->bCurrect = FALSE; 
        }
     }

     pMe->bMsgBoxExist = TRUE;     
     //Add End
     
     /*we do not special ISTATIC control, because of DIALOG will redraw
      * ISTATIC control, and the text color is the default, it may be the
      * same as background, but DrawPromptMessage function will change the
      * text color.*/
     DrawPromptMessage(pMe->m_pDisplay, pIStatic, &MsgParam);
     if(pIStatic)
     ISTATIC_Release(pIStatic);
     IDISPLAY_Update(pMe->m_pDisplay);

     return TRUE;
}

/*==============================================================================
函数:
       FmRadio_ProcessState
说明:
       FmRadio Applet状态处理主函数。函数根据pMe->m_currState将调用路由至
       相应的处理函数。

参数:
       pMe [In]: 指向FmRadio Applet对象结构的指针,该结构包含小程序的特定信息.

返回值:
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注:

==============================================================================*/
NextFSMAction FmRadio_ProcessState(CFmRadio *pMe)
{
    NextFSMAction retVal = NFSMACTION_WAIT;

    if (NULL == pMe)
    {
        return retVal;
    }

    // 根据当前状态调用相应的状态处理函数
    switch (pMe->m_eCurState)
    {
        case FMRADIOST_NONE:
            retVal = FmRadioStateNoOperationHandler(pMe);
            break;

        case FMRADIOST_INIT:
            retVal = FmRadioStateInitHandler(pMe);
            break;
            
        case FMRADIOST_USER_MSG:
            retVal = FmRadioStateUserMsgHandler(pMe);
            break;
            
        case FMRADIOST_MAIN:
            retVal = FmRadioStateMainHandler(pMe);
            break;

        case FMRADIOST_OPTION:
            retVal = FmRadioStateOptionHandler(pMe);
            break;         
            
        case FMRADIOST_LIST:
            retVal = FmRadioStateListHandler(pMe);   
            break;    
                                            
        case FMRADIOST_EXIT:
            retVal = FmRadioStateExitHandler(pMe);
            break;
            
        default:
            break;
    }
    return retVal;
}


/*==============================================================================
函数：
       FmRadioStateNoOperationHandler
说明：
       FMRADIOST_NONE 状态处理函数

参数：
       pMe [in]：指向FmRadio Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/
static NextFSMAction FmRadioStateNoOperationHandler(CFmRadio *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    MOVE_TO_STATE(FMRADIOST_EXIT)

    return NFSMACTION_CONTINUE;
} // FmRadioStateNoOperationHandler

/*==============================================================================
函数：
       FmRadioStateInitHandler
说明：
       FMRADIOST_INIT 状态处理函数

参数：
       pMe [in]：指向FmRadio Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/
static NextFSMAction FmRadioStateInitHandler(CFmRadio *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    MOVE_TO_STATE(FMRADIOST_MAIN)
    return NFSMACTION_CONTINUE;
} // FmRadioStateInitHandler
static NextFSMAction FmRadioStateUserMsgHandler(CFmRadio *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
          //  FmRadio_ShowDialog(pMe, IDD_FM_RADIO_OPTION);
          
            FmRadio_ShowMsgBoxDialog( pMe, 
                                    AEE_APPSCOMMONRES_LANGFILE,
                                    IDS_CONFIRM_OFF_MP,
                                    0,//IDD_FM_RADIO_USER,                                      
                                    NULL,
                                    -1);
            return NFSMACTION_WAIT;

        case DLGRET_OPTION_SELECT:
            MOVE_TO_STATE(FMRADIOST_EXIT)            
            return NFSMACTION_CONTINUE;  

        case DLGRET_CANCELED:
            MOVE_TO_STATE(FMRADIOST_INIT)
            return NFSMACTION_WAIT;        

        default:
            break;
    }

    return NFSMACTION_WAIT;
}//FmRadioStateUserMsgHandler


/*==============================================================================
函数：
       FmRadioStateMainHandler
说明：
       FMRADIOST_MAIN 状态处理函数

参数：
       pMe [in]：指向FmRadio Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/
static NextFSMAction FmRadioStateMainHandler(CFmRadio *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            FmRadio_ShowDialog(pMe, IDD_FM_RADIO_MAIN);
            return NFSMACTION_WAIT;

        case DLGRET_OPTION_SELECT:
            MOVE_TO_STATE(FMRADIOST_OPTION)
            return NFSMACTION_CONTINUE;  

        case DLGRET_CANCELED:
            MOVE_TO_STATE(FMRADIOST_EXIT)
            return NFSMACTION_CONTINUE;        

        default:
            break;
    }

    return NFSMACTION_WAIT;
} // FmRadioStateMainHandler

/*==============================================================================
函数：
       FmRadioStateOptionHandler
说明：
       FMRADIOST_Option 状态处理函数

参数：
       pMe [in]：指向FmRadio Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/
static NextFSMAction FmRadioStateOptionHandler(CFmRadio *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            FmRadio_ShowDialog(pMe, IDD_FM_RADIO_OPTION);
            return NFSMACTION_WAIT;

        case DLGRET_OPTION_LIST:
            MOVE_TO_STATE(FMRADIOST_LIST)
            return NFSMACTION_CONTINUE;  
            
        case DLGRET_CANCELED:
            MOVE_TO_STATE(FMRADIOST_MAIN)
            return NFSMACTION_CONTINUE;              

        default:
            break;
    }

    return NFSMACTION_WAIT;
} // FmRadioStateOptionHandler

/*==============================================================================
函数：
       FmRadioStateListHandler
说明：
       FMRADIOST_Option 状态处理函数

参数：
       pMe [in]：指向FmRadio Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/
static NextFSMAction FmRadioStateListHandler(CFmRadio *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            FmRadio_ShowDialog(pMe, IDD_FM_RADIO_LIST);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
            MOVE_TO_STATE(FMRADIOST_MAIN)
            return NFSMACTION_CONTINUE;              

        default:
            break;
    }

    return NFSMACTION_WAIT;
} // FmRadioStateListHandler

/*==============================================================================
函数：
       FmRadioStateExitHandler
说明：
       FMRADIOST_EXIT 状态处理函数

参数：
       pMe [in]：指向FmRadio Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/
static NextFSMAction FmRadioStateExitHandler(CFmRadio *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    (void) ISHELL_CloseApplet(pMe->m_pShell, FALSE);

    return NFSMACTION_WAIT;
} // FmRadioStateExitHandler
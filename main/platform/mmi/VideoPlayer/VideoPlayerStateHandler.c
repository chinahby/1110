/*=================================================================================================================
// 文件：VideoPlayerStateHandler.c
//
//	  版权所有(c) 2008 Anylook
//	
//
// 文件说明：
//	  本文件包含VideoPlayer Applet状态处理函数相关数据类型及可供外部文件调用的
//	  函数声明。
//
// 作者：闫丽娜
// 创建日期：2008-10-09

=================================================================================================================*/

/*=================================================================================================================
修改历史记录：
时       间      修 改 人       修改内容、位置及原因
-----------      ----------     -----------------------------------
  2008-10-09     闫丽娜          初始版本

=================================================================================================================*/

/*=================================================================================================================
                             本文件包含的外部文件
=================================================================================================================*/

#include "VideoPlayer_priv.h"
#include "Appscommon.brh"
/*=================================================================================================================
                         定义（宏定义及数据类型提前声明）和常数
=================================================================================================================*/

/*=================================================================================================================
                                  类型定义
=================================================================================================================*/


/*=================================================================================================================
                             函数声明(内部函数清单）                                                
=================================================================================================================*/
// 状态STATE_NONE处理函数
static NextFSMAction StateNoOperationHandler(CVideoPlayer *pMe);

// 状态STATE_INIT处理函数
static NextFSMAction StateInitHandler(CVideoPlayer *pMe);

//状态STATE_PLAYER处理函数
static NextFSMAction StatePlayerHandler(CVideoPlayer *pMe);

//状态STATE_OPTION处理函数
//static NextFSMAction StateOptionHandler(CVideoPlayer *pMe);

//状态STATE_MSGBOX处理函数
static NextFSMAction StateMSGBOXHandler(CVideoPlayer *pMe);

// 状态STATE_EXIT处理函数
static NextFSMAction StateExitHandler(CVideoPlayer *pMe);
/*=================================================================================================================
                              全局数据
=================================================================================================================*/

/*=================================================================================================================
                              本地（静态）数据
=================================================================================================================*/


/*=================================================================================================================
                              函数定义
=================================================================================================================*/
/*=================================================================================================================
函数：VideoPlayer_ProcessState

说明：
       VideoPlayer Applet状态处理主函数。函数根据pMe->m_currState将调用路由至
       相应的处理函数。

参数：
       pMe [in]：指向VideoPlayer Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：:

=================================================================================================================*/
NextFSMAction VideoPlayer_ProcessState(CVideoPlayer *pMe)
{
    NextFSMAction retVal = NFSMACTION_WAIT;
    
    if (NULL == pMe)
    {
        return retVal;
    }

    // 根据当前状态调用相应的状态处理函数
    switch (pMe->m_eCurState)
    {
        case STATE_NONE:
            retVal = StateNoOperationHandler(pMe);
            break;
            
        case STATE_INIT:
            retVal = StateInitHandler(pMe);
            break;
        
        case STATE_PLAYER:
            retVal = StatePlayerHandler(pMe);
            break;
#if 0
        case STATE_OPTION:
            retVal = StateOptionHandler(pMe);
            break;
#endif
        case STATE_MSGBOX:
            retVal = StateMSGBOXHandler(pMe);
            break;
        
        case STATE_EXIT:
            retVal = StateExitHandler(pMe);
            break;
        
        default:
            break;
    }
    
    return retVal;
}

/*=================================================================================================================
函数：StateNoOperationHandler

说明：
 
       设置状态STATE_NONE时函数被调用。过渡状态，函数将设置下一状态为STATE_EXIT
       。

参数：
       pMe [in]：指向VideoPlayer Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
         NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：:

=================================================================================================================*/
static NextFSMAction StateNoOperationHandler(CVideoPlayer *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    MOVE_TO_STATE(STATE_EXIT);
    return NFSMACTION_CONTINUE;
} // StateNoOperationHandler

/*=================================================================================================================
函数：StateInitHandler

说明：
 
设置状态STATE_INIT时函数被调用。过渡状态，函数将设置下一状态为STATE_NOPLAY。

参数：
       pMe [in]：指向VideoPlayer Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
         NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：:

=================================================================================================================*/
static NextFSMAction StateInitHandler(CVideoPlayer *pMe)
{
    DBGPRINTF("LINA INIT");
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
      
    else
    {
        MOVE_TO_STATE(STATE_PLAYER);
        return NFSMACTION_CONTINUE;
    }
}  // StateInitHandler


/*=================================================================================================================
函数：StatePlayerHandler

说明：
       设置状态STATE_PLAYER时函数被调用

参数：
       pMe [in]：指向VideoPlayer Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
         NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：:

=================================================================================================================*/
static NextFSMAction StatePlayerHandler(CVideoPlayer *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgRet)
    {
        case DLGRET_CREATE: 
            pMe->m_bNotOverwriteDlgRet = FALSE;
            VideoPlayer_ShowDialog(pMe, IDD_VIDEOPLAYER);
            return NFSMACTION_WAIT;

        case DLGRET_MSGBOX:
            if(pMe->Is848Busy)
            {
                if(app_media_scheduler() == APP_MEDIA_IMPACT_BY_FM)
                {
                    VideoPlayer_ShowMsgBox(pMe,IDS_CONFIRM_OFF_FM);
                }
                if(app_media_scheduler() == APP_MEDIA_IMPACT_BY_MP3)
                {
                    VideoPlayer_ShowMsgBox(pMe,IDS_CONFIRM_OFF_MP);
                }                
            }
            else
            {
                VideoPlayer_ShowMsgBox(pMe,IDS_PLAYFAILED);
            }
            MOVE_TO_STATE(STATE_MSGBOX)
            return NFSMACTION_CONTINUE;
#if 0
        case DLGRET_OPTION:           
            MOVE_TO_STATE(STATE_OPTION)
            return NFSMACTION_CONTINUE;
#endif

        case DLGRET_CANCELED:
            MOVE_TO_STATE(STATE_EXIT)
            return NFSMACTION_CONTINUE;
		case DLGRET_FAILD:
			VideoPlayer_ShowMsgBox(pMe,IDS_PLAYFORMATFAILD);
			MOVE_TO_STATE(STATE_MSGBOX)
            return NFSMACTION_CONTINUE;
        
        default:
            break;
    }

    return NFSMACTION_WAIT;
}// StatePlayerHandler


/*=================================================================================================================
函数：StateOptionHandler

说明：
       设置状态STATE_OPTION时函数被调用。

参数：
       pMe [in]：指向VideoPlayer Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：:

=================================================================================================================*/
#if 0
static NextFSMAction StateOptionHandler(CVideoPlayer *pMe)
{   
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;           
            VideoPlayer_ShowDialog(pMe, IDD_OPTION);            
            return NFSMACTION_WAIT;
             
        case DLGRET_MSGBOX:
            if(pMe->m_nItemID==IDS_PLAY) 
            {
                VideoPlayer_ShowMsgBox(pMe,IDS_PLAYFAILED);
            }
            if(pMe->m_nItemID==IDS_DELETE)
            {              
                VideoPlayer_ShowMsgBox(pMe,IDS_DEL);
            }
            MOVE_TO_STATE(STATE_MSGBOX)
            return NFSMACTION_CONTINUE;
        
        case DLGRET_CANCELED:
            MOVE_TO_STATE(STATE_PLAYER)
            return NFSMACTION_CONTINUE;
        
        default:
            break;
    }

    return NFSMACTION_WAIT;
}// StateOptionHandler
#endif
/*=================================================================================================================
 函数：StateMSGBOXHandler
            
 说明：
       设置状态STATE_MSGBOX时函数被调用。
            
 参数：
       pMe [in]：指向VideoPlayer Applet对象结构的指针。该结构包含小程序的特定信息。
            
 返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。
            
 备注：:
            
=================================================================================================================*/
static NextFSMAction StateMSGBOXHandler(CVideoPlayer *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            VideoPlayer_ShowDialog(pMe, IDD_MSGBOX);
            return NFSMACTION_WAIT;

        case DLGRET_VIDEOPLAYER:
            MOVE_TO_STATE(STATE_PLAYER)
            return NFSMACTION_CONTINUE;
#if 0
        case DLGRET_MSGBOX:
            VideoPlayer_ShowMsgBox(pMe,IDS_DONE);
            break;
#endif      
        case DLGRET_CANCELED:           
            MOVE_TO_STATE(STATE_EXIT)
            return NFSMACTION_CONTINUE;            
            
        default:
            break;
    }

    return NFSMACTION_WAIT;
}//StateMSGBOXHandler

/*=================================================================================================================
函数：StateExitHandler

说明：
       设置状态STATE_EXIT时函数被调用。用于关闭Applet。

参数：
       pMe [in]：指向VideoPlayer Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
         NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：:

=================================================================================================================*/
static NextFSMAction StateExitHandler(CVideoPlayer *pMe)
{
    (void) ISHELL_CloseApplet(pMe->m_pShell, FALSE);

    return NFSMACTION_WAIT;
} // StateExitHandler

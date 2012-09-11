/*==============================================================================
// 文件：
//        SoundMenuStateHandler.c
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
-----------      ----------  --------  -----------------------------------------
==============================================================================*/


/*==============================================================================
                                 本文件包含的外部文件
==============================================================================*/
#include "SoundMenu_priv.h"

/*==============================================================================
                                 宏定义和常数
==============================================================================*/


/*==============================================================================
                                 类型定义
==============================================================================*/


/*==============================================================================
                                 函数声明
==============================================================================*/
// 状态 SOUNDMENUST_NONE 处理函数
static NextFSMAction Sound_StateNoOperationHandler(CSoundMenu *pMe);

// 状态 SOUNDMENUST_INIT 处理函数
static NextFSMAction Sound_StateInitHandler(CSoundMenu *pMe);

// 状态 SOUNDMENUST_MAIN 处理函数
static NextFSMAction Sound_StateMainHandler(CSoundMenu *pMe);

// 状态 SOUNDMENUST_HINT 处理函数
static NextFSMAction Sound_StateHintHandler(CSoundMenu *pMe);

// 状态 SOUNDMENUST_RINGER 处理函数
static NextFSMAction Sound_StateRingerHandler(CSoundMenu *pMe);

#if defined(FEATURE_WMS_APP) && !defined(FEATURE_WMSAPP_ONLYSUPPORTVMAIL)
// 状态 SOUNDMENUST_SMSRING 处理函数
static NextFSMAction Sound_StateSmsRingHandler(CSoundMenu *pMe);
#endif

// 状态 SOUNDMENUST_VOLUME 处理函数
static NextFSMAction Sound_StateVolumeHandler(CSoundMenu *pMe);

//static NextFSMAction StateOtherHandler(CSoundMenu *pMe);

static NextFSMAction Sound_StateOtherSelHandler(CSoundMenu *pMe);

static NextFSMAction Sound_StateFMmodeHandler(CSoundMenu *pMe);

// 状态 SOUNDMENUST_VOLUMESUB 处理函数
static NextFSMAction Sound_StateVolumeSubHandler(CSoundMenu *pMe);

// 状态 SOUNDMENUST_EXIT 处理函数
static NextFSMAction Sound_StateExitHandler(CSoundMenu *pMe);

//#ifdef KEYSND_ZY
// 状态 SOUNDMENUST_KEYSND 处理函数
static NextFSMAction Sound_StateKeySndHandler(CSoundMenu *pMe);
//#endif /* KEYSND_ZY */

// 状态 DLGRET_KEYLENGTH 处理函数
//static NextFSMAction Sound_StateKeylengthHandler(CSoundMenu *pMe);

#if 0
//状态 SOUNDMENUST_WARNING 处理函数
static NextFSMAction Sound_StateWarningHandler(CSoundMenu *pMe);
#endif
#if 0
// 状态 SOUNDMENUST_AUTOANSWERSUB 处理函数
static NextFSMAction Sound_StateAutoAnswerSubHandler(CSoundMenu *pMe);
#endif
//状态 SOUNDMENUST_SOUNDMENU 处理函数
static NextFSMAction Sound_StateSoundMenuHandler(CSoundMenu *pMe);

// 状态 SOUNDMENUST_SCENEMODESUB 处理函数
static NextFSMAction Sound_StateSceneModeSubHandler(CSoundMenu *pMe);

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
       SoundMenu_ProcessState
说明:
       SoundMenu Applet状态处理主函数。函数根据pMe->m_currState将调用路由至
       相应的处理函数。

参数:
       pMe [In]: 指向SoundMenu Applet对象结构的指针,该结构包含小程序的特定信息.

返回值:
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注:

==============================================================================*/
NextFSMAction SoundMenu_ProcessState(CSoundMenu *pMe)
{
    NextFSMAction retVal = NFSMACTION_WAIT;

    if (NULL == pMe)
    {
        return retVal;
    }

    // 根据当前状态调用相应的状态处理函数
    switch (pMe->m_eCurState)
    {
        case SOUNDMENUST_NONE:
            retVal = Sound_StateNoOperationHandler(pMe);
            break;

        case SOUNDMENUST_INIT:
            retVal = Sound_StateInitHandler(pMe);
            break;

        case SOUNDMENUST_MAIN:
            retVal = Sound_StateMainHandler(pMe);
            break;

        case SOUNDMENUST_SCENEMODESUB:
            retVal = Sound_StateSceneModeSubHandler(pMe);
            break;

        case SOUNDMENUST_SOUNDMENU:
            retVal = Sound_StateSoundMenuHandler(pMe);
            break;

        case SOUNDMENUST_HINT:
            retVal = Sound_StateHintHandler(pMe);
            break;

        case SOUNDMENUST_RINGER:
            retVal = Sound_StateRingerHandler(pMe);
            break;

#if defined(FEATURE_WMS_APP) && !defined(FEATURE_WMSAPP_ONLYSUPPORTVMAIL)
        case SOUNDMENUST_SMSRING:
            retVal = Sound_StateSmsRingHandler(pMe);
            break;
#endif

        case SOUNDMENUST_OTHERSEL:
            retVal = Sound_StateOtherSelHandler(pMe);
            break;
        case SOUNDMENUST_FMMODE:
        	retVal = Sound_StateFMmodeHandler(pMe);
        	break;
#if 0
        case SOUNDMENUST_AUTOANSWERSUB:
            retVal = Sound_StateAutoAnswerSubHandler(pMe);
            break;
#endif
        case SOUNDMENUST_KEYSND:
            retVal = Sound_StateKeySndHandler(pMe);
            break;

        //case SOUNDMENUST_KEYLENGTH:
        //    retVal = Sound_StateKeylengthHandler(pMe);
        //    break;

        case SOUNDMENUST_VOLUME:
            retVal = Sound_StateVolumeHandler(pMe);
            break;

        case SOUNDMENUST_VOLUMESUB:
            retVal = Sound_StateVolumeSubHandler(pMe);
            break;

#if 0
        case SOUNDMENUST_WARNING:
            retVal = Sound_StateWarningHandler(pMe);
            break;
#endif

        case SOUNDMENUST_EXIT:
            retVal = Sound_StateExitHandler(pMe);
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
       SOUNDMENUST_NONE 状态处理函数

参数：
       pMe [in]：指向SoundMenu Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/
static NextFSMAction Sound_StateNoOperationHandler(CSoundMenu *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    MOVE_TO_STATE(SOUNDMENUST_EXIT)
    return NFSMACTION_CONTINUE;
} // StateNoOperationHandler


/*==============================================================================
函数：
       StateInitHandler
说明：
       SOUNDMENUST_INIT 状态处理函数

参数：
       pMe [in]：指向SoundMenu Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/
static NextFSMAction Sound_StateInitHandler(CSoundMenu *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    if(ISHELL_RegisterNotify(pMe->m_pShell, AEECLSID_APP_SOUNDMENU,
        AEECLSID_ALERT_NOTIFIER, NMASK_ALERT_ONOFF | NMASK_ALERT_MUTED) != SUCCESS)
    {
        return NFSMACTION_CONTINUE;
    }
    MOVE_TO_STATE(SOUNDMENUST_MAIN)
    return NFSMACTION_CONTINUE;
} // StateInitHandler


/*==============================================================================
函数：
       StateMainHandler
说明：
       SOUNDMENUST_MAIN 状态处理函数

参数：
       pMe [in]：指向SoundMenu Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/
static NextFSMAction Sound_StateMainHandler(CSoundMenu *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            SoundMenu_ShowDialog(pMe, IDD_SCENEMODE_MENU);
            return NFSMACTION_WAIT;
        case DLGRET_MESSAGE:
            SoundMenu_ShowDialog(pMe, IDD_WARNING_MESSEGE);
            return NFSMACTION_WAIT;
            
        case DLGRET_SCENEMODESUB:
            MOVE_TO_STATE(SOUNDMENUST_SCENEMODESUB)
            return NFSMACTION_CONTINUE;

        case DLGRET_MSGBOX_OK:
            pMe->m_sSubDlgId = IDS_WARNING_MESSEGE;
            SoundMenu_ShowDialog(pMe, IDD_WARNING_MESSEGE);
            return NFSMACTION_WAIT;
            
        case DLGRET_CANCELED:
            MOVE_TO_STATE(SOUNDMENUST_EXIT)
            return NFSMACTION_CONTINUE;
        case DLGRET_OK:
            MOVE_TO_STATE(SOUNDMENUST_MAIN)
            return NFSMACTION_CONTINUE;

        default:
            ASSERT_NOT_REACHABLE;
    }

    return NFSMACTION_WAIT;
} // StateSceneModeHandler


/*==============================================================================
函数：
       StateSceneModeSubHandler
说明：
       SOUNDMENUST_SCENEMODESUB 状态处理函数

参数：
       pMe [in]：指向SoundMenu Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/
static NextFSMAction Sound_StateSceneModeSubHandler(CSoundMenu *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            SoundMenu_ShowDialog(pMe, IDD_SCENEMODE_SUB);
            return NFSMACTION_WAIT;

        case DLGRET_SOUNDMENU:
            switch(app_media_scheduler())
            {
                case APP_MEDIA_IMPACT_BY_FM:
                    pMe->m_sSubDlgId = IDS_CONFIRM_OFF_FM;
                    SoundMenu_ShowDialog(pMe, IDD_WARNING_MESSEGE);
                    return NFSMACTION_WAIT;

                case APP_MEDIA_IMPACT_BY_MP3:
                	/*
                    pMe->m_sSubDlgId = IDS_CONFIRM_OFF_MP;
                    SoundMenu_ShowDialog(pMe, IDD_WARNING_MESSEGE);
                    return NFSMACTION_WAIT;
                    */

                    //Add By zzg 2010_08_30                   
                    MOVE_TO_STATE(SOUNDMENUST_SOUNDMENU)
                    return NFSMACTION_CONTINUE;
                    //Add End

                default:
                    MOVE_TO_STATE(SOUNDMENUST_SOUNDMENU)
                    return NFSMACTION_CONTINUE;
            }

        case DLGRET_MSGBOX_OK:

            if(pMe->m_sSubDlgId == IDS_ACTIVATING || pMe->m_sSubDlgId == IDS_RESTORED)
            {
                pMe->m_sSubDlgId = IDS_WARNING_MESSEGE;
                SoundMenu_ShowDialog(pMe, IDD_WARNING_MESSEGE);
                return NFSMACTION_WAIT;
            }
            
            if(pMe->m_sSubDlgId == IDS_CONFIRM_OFF_FM)
            {
                ISHELL_SendEvent( pMe->m_pShell, AEECLSID_APP_FMRADIO, EVT_CLOSEAPP, 0, 0); 
            }
            else if(pMe->m_sSubDlgId == IDS_CONFIRM_OFF_MP)
            {
                ISHELL_SendEvent( pMe->m_pShell, AEECLSID_APP_MUSICPLAYER, EVT_CLOSEAPP, 0, 0);
            }
            MOVE_TO_STATE(SOUNDMENUST_SOUNDMENU)
            return NFSMACTION_CONTINUE;

        case DLGRET_MSGBOX_CANCEL:
            MOVE_TO_STATE(SOUNDMENUST_MAIN)
            return NFSMACTION_CONTINUE;

        case DLGRET_MESSAGE:
            SoundMenu_ShowDialog(pMe, IDD_WARNING_MESSEGE);
            return NFSMACTION_WAIT;
            //return NFSMACTION_CONTINUE;

        case DLGRET_CANCELED:
        case DLGRET_OK:
            MOVE_TO_STATE(SOUNDMENUST_MAIN)
            return NFSMACTION_CONTINUE;

        default:
            ASSERT_NOT_REACHABLE;
    }
    return NFSMACTION_WAIT;
} // StateSceneModeSubHandler


/*==============================================================================
函数：
       StateSoundMenuHandler
说明：
       SOUNDMENUST_SOUNDMENU 状态处理函数

参数：
       pMe [in]：指向SoundMenu Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/
static NextFSMAction Sound_StateSoundMenuHandler(CSoundMenu *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    pMe->m_fSubDlgId = 0;    //重新计算子菜单选择条的位置

    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            //if (pMe->m_bNaturalStart == NORMAL_ENTRY)
            //{  // 若是正常启动 Applet
            SoundMenu_ShowDialog(pMe, IDD_SOUNDMENU_PROFILES);
            //}
            //else if(pMe->m_bNaturalStart == SOUNDLIST_ENTRY)
            //{
            //  SoundMenu_ShowDialog(pMe, IDD_SOUNDMENU_MENU);
            //}
            return NFSMACTION_WAIT;

        case DLGRET_HINT:
            MOVE_TO_STATE(SOUNDMENUST_HINT)
            return NFSMACTION_CONTINUE;

        case DLGRET_RINGER:
            MOVE_TO_STATE(SOUNDMENUST_RINGER)
            return NFSMACTION_CONTINUE;

#if defined(FEATURE_WMS_APP) && !defined(FEATURE_WMSAPP_ONLYSUPPORTVMAIL)
        case DLGRET_SMSRING:
            MOVE_TO_STATE(SOUNDMENUST_SMSRING)
            return NFSMACTION_CONTINUE;
#endif

        case DLGRET_OTHERSEL:
            MOVE_TO_STATE(SOUNDMENUST_OTHERSEL)
            return NFSMACTION_CONTINUE;
        case DLGRET_FMMODE:
        	MOVE_TO_STATE(SOUNDMENUST_FMMODE)
            return NFSMACTION_CONTINUE;
        //case DLGRET_AUTOANSWERSUB:
        //    MOVE_TO_STATE(SOUNDMENUST_AUTOANSWERSUB)
        //    return NFSMACTION_CONTINUE;

        case DLGRET_KEYSND:
            MOVE_TO_STATE(SOUNDMENUST_KEYSND)
            return NFSMACTION_CONTINUE;

        //case DLGRET_KEYLENGTH:
        //    MOVE_TO_STATE(SOUNDMENUST_KEYLENGTH)
        //    return NFSMACTION_CONTINUE;

        case DLGRET_VOLUME:
            MOVE_TO_STATE(SOUNDMENUST_VOLUME)
            return NFSMACTION_CONTINUE;

        case DLGRET_MESSAGE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            SoundMenu_ShowDialog(pMe, IDD_WARNING_MESSEGE);
            return NFSMACTION_WAIT;  

        case DLGRET_MSGBOX_OK:    
         if(pMe->m_sSubDlgId == IDS_CONFIRM_OFF_MP)
            {
                ISHELL_SendEvent( pMe->m_pShell, AEECLSID_APP_MUSICPLAYER, EVT_CLOSEAPP, 0, 0);
            }
            MOVE_TO_STATE(SOUNDMENUST_VOLUME)
            return NFSMACTION_CONTINUE;
            
        case DLGRET_OK:
        case DLGRET_MSGBOX_CANCEL:
            MOVE_TO_STATE(SOUNDMENUST_SOUNDMENU)
            return NFSMACTION_CONTINUE; 
#if 0
        case DLGRET_WARNING:
            MOVE_TO_STATE(SOUNDMENUST_WARNING)
            return NFSMACTION_CONTINUE;
#endif

        case DLGRET_CANCELED:
            if (pMe->m_bNaturalStart == NORMAL_ENTRY)
            {   // 若是正常启动 Applet
                MOVE_TO_STATE(SOUNDMENUST_MAIN)
            }
            else if(pMe->m_bNaturalStart == SOUNDLIST_ENTRY || (pMe->m_RingerType == SET_ANOTHER))
            {
                MOVE_TO_STATE(SOUNDMENUST_EXIT)
            }
            return NFSMACTION_CONTINUE;

        default:
            ASSERT_NOT_REACHABLE;
    }

    return NFSMACTION_WAIT;
} // StateSoundMenuHandler


/*==============================================================================
函数：
       StateHintHandler
说明：
       SOUNDMENUST_HINT 状态处理函数

参数：
       pMe [in]：指向SoundMenu Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/
static NextFSMAction Sound_StateHintHandler(CSoundMenu *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            SoundMenu_ShowDialog(pMe, IDD_HINT_MENU);
            return NFSMACTION_WAIT;

        case DLGRET_MESSAGE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            SoundMenu_ShowDialog(pMe, IDD_WARNING_MESSEGE);
            return NFSMACTION_WAIT;

        case DLGRET_OK:
        case DLGRET_CANCELED:
            MOVE_TO_STATE(SOUNDMENUST_SOUNDMENU)
            return NFSMACTION_CONTINUE;

        default:
            ASSERT_NOT_REACHABLE;
    }

    return NFSMACTION_WAIT;
} // StateHintHandler


/*==============================================================================
函数：
       StateRingerHandler
说明：
       SOUNDMENUST_RINGER 状态处理函数

参数：
       pMe [in]：指向SoundMenu Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/
static NextFSMAction Sound_StateRingerHandler(CSoundMenu *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            SoundMenu_ShowDialog(pMe, IDD_RINGER_MENU);
            return NFSMACTION_WAIT;

        case DLGRET_MESSAGE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            SoundMenu_ShowDialog(pMe, IDD_WARNING_MESSEGE);
            return NFSMACTION_WAIT;

        case DLGRET_OK:
        case DLGRET_CANCELED:
            if(pMe->m_RingerType == SET_ANOTHER)
            {
                MOVE_TO_STATE(SOUNDMENUST_EXIT)
            }
            else
            {
                MOVE_TO_STATE(SOUNDMENUST_SOUNDMENU)
            }
            return NFSMACTION_CONTINUE;

        default:
            ASSERT_NOT_REACHABLE;
    }

    return NFSMACTION_WAIT;
} // StateRingerHandler

#if defined(FEATURE_WMS_APP) && !defined(FEATURE_WMSAPP_ONLYSUPPORTVMAIL)
/*==============================================================================
函数：
       StateSmsRingHandler
说明：
       SOUNDMENUST_SMSRING 状态处理函数

参数：
       pMe [in]：指向SoundMenu Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/
static NextFSMAction Sound_StateSmsRingHandler(CSoundMenu *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            SoundMenu_ShowDialog(pMe, IDD_SMSRING_MENU);
            return NFSMACTION_WAIT;
            
        case DLGRET_MESSAGE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            SoundMenu_ShowDialog(pMe, IDD_WARNING_MESSEGE);
            return NFSMACTION_WAIT;

        case DLGRET_OK:
        case DLGRET_CANCELED:
            MOVE_TO_STATE(SOUNDMENUST_SOUNDMENU)
            return NFSMACTION_CONTINUE;

        default:
            ASSERT_NOT_REACHABLE;
    }

    return NFSMACTION_WAIT;
} // StateSmsRingHandler
#endif

/*==============================================================================
函数：
       StateVolumeHandler
说明：
       SOUNDMENUST_VOLUME 状态处理函数

参数：
       pMe [in]：指向SoundMenu Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/
static NextFSMAction Sound_StateVolumeHandler(CSoundMenu *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            SoundMenu_ShowDialog(pMe, IDD_VOLUME_MENU);
            return NFSMACTION_WAIT;

        case DLGRET_VOLUMESUB:
            MOVE_TO_STATE(SOUNDMENUST_VOLUMESUB)
            return NFSMACTION_CONTINUE;

        case DLGRET_CANCELED:
            MOVE_TO_STATE(SOUNDMENUST_SOUNDMENU)
            return NFSMACTION_CONTINUE;

        default:
            ASSERT_NOT_REACHABLE;
    }

    return NFSMACTION_WAIT;
} // StateVolumeHandler

static NextFSMAction Sound_StateFMmodeHandler(CSoundMenu *pMe)
{
	 if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            SoundMenu_ShowDialog(pMe, IDD_FM_MENU);
            return NFSMACTION_WAIT;

        case DLGRET_MESSAGE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            SoundMenu_ShowDialog(pMe, IDD_WARNING_MESSEGE);
            return NFSMACTION_WAIT;

        case DLGRET_OK:
        case DLGRET_CANCELED:
            MOVE_TO_STATE(SOUNDMENUST_SOUNDMENU)
            return NFSMACTION_CONTINUE;

        default:
            ASSERT_NOT_REACHABLE;
    }

    return NFSMACTION_WAIT;
}

/*==============================================================================
函数：
       StateOtherSelHandler
说明：
       SOUNDMENUST_OTHERSEL 状态处理函数

参数：
       pMe [in]：指向SoundMenu Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/
static NextFSMAction Sound_StateOtherSelHandler(CSoundMenu *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            SoundMenu_ShowDialog(pMe, IDD_OTHERSEL_MENU);
            return NFSMACTION_WAIT;

        case DLGRET_MESSAGE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            SoundMenu_ShowDialog(pMe, IDD_WARNING_MESSEGE);
            return NFSMACTION_WAIT;

        case DLGRET_OK:
        case DLGRET_CANCELED:
            MOVE_TO_STATE(SOUNDMENUST_SOUNDMENU)
            return NFSMACTION_CONTINUE;

        default:
            ASSERT_NOT_REACHABLE;
    }

    return NFSMACTION_WAIT;
} // StateOtherSelHandler


/*==============================================================================
函数：
       StateVolumeSubHandler
说明：
       SOUNDMENUST_VOLUMESUB 状态处理函数

参数：
       pMe [in]：指向SoundMenu Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/
static NextFSMAction Sound_StateVolumeSubHandler(CSoundMenu *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            SoundMenu_ShowDialog(pMe, IDD_VOLUME_SUB);
            return NFSMACTION_WAIT;
        case DLGRET_MESSAGE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            SoundMenu_ShowDialog(pMe, IDD_WARNING_MESSEGE);
            return NFSMACTION_WAIT;

        case DLGRET_OK:
        case DLGRET_CANCELED:
            MOVE_TO_STATE(SOUNDMENUST_VOLUME)
            return NFSMACTION_CONTINUE;

        default:
            ASSERT_NOT_REACHABLE;
    }

    return NFSMACTION_WAIT;
} // StateVolumeSubHandler

/*==============================================================================
函数：
       StateKeySndHandler
说明：
       SOUNDMENUST_KEYSND 状态处理函数

参数：
       pMe [in]：指向SoundMenu Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/
static NextFSMAction Sound_StateKeySndHandler(CSoundMenu *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            SoundMenu_ShowDialog(pMe, IDD_KEYSND_MENU);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
            MOVE_TO_STATE(SOUNDMENUST_SOUNDMENU)
            return NFSMACTION_CONTINUE;

        default:
            ASSERT_NOT_REACHABLE;
    }

    return NFSMACTION_WAIT;
} // StateFlipSndHandler
#if 0
/*==============================================================================
函数：
       StateKeylengthHandler
说明：
       SOUNDMENUST_KEYLENGTH 状态处理函数

参数：
       pMe [in]：指向SoundMenu Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/
static NextFSMAction Sound_StateKeylengthHandler(CSoundMenu *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            SoundMenu_ShowDialog(pMe, IDD_KEYTONE_LENGTH);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
            MOVE_TO_STATE(SOUNDMENUST_SOUNDMENU)
            return NFSMACTION_CONTINUE;

        default:
            ASSERT_NOT_REACHABLE;
    }

    return NFSMACTION_WAIT;
} // StateKeylengthHandler
#endif
#if 0
/*==============================================================================
函数：
       StateAutoAnswerSubHandler
说明：
       SOUNDMENUST_AUTOANSWERSUB 状态处理函数

参数：
       pMe [in]：指向SoundMenu Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/
static NextFSMAction Sound_StateAutoAnswerSubHandler(CSoundMenu *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            SoundMenu_ShowDialog(pMe, IDD_AUTOANSWER_SUB);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
            if (pMe->m_bNaturalStart == NORMAL_ENTRY)
            {   // 若是正常启动 Applet
                MOVE_TO_STATE(SOUNDMENUST_SOUNDMENU)
            }
            else if(pMe->m_bNaturalStart == AUTOANSWER_ENTRY)
            {
                MOVE_TO_STATE(SOUNDMENUST_EXIT)
            }
            return NFSMACTION_CONTINUE;

        default:
            ASSERT_NOT_REACHABLE;
    }

    return NFSMACTION_WAIT;
} // StateAutoAnswerSubHandler
#endif

#if 0
/*==============================================================================
函数：
       StateWarningHandler
说明：
       SOUNDMENUST_WARNING 状态处理函数

参数：
       pMe [in]：指向SoundMenu Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/
static NextFSMAction Sound_StateWarningHandler(CSoundMenu *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            SoundMenu_ShowDialog(pMe, IDD_WARNING_MESSEGE);
            return NFSMACTION_WAIT;

        case DLGRET_HINT:
            MOVE_TO_STATE(SOUNDMENUST_HINT)
            return NFSMACTION_CONTINUE;

        case DLGRET_RINGER:
            MOVE_TO_STATE(SOUNDMENUST_RINGER)
            return NFSMACTION_CONTINUE;

#if defined(FEATURE_WMS_APP) && !defined(FEATURE_WMSAPP_ONLYSUPPORTVMAIL)
        case DLGRET_SMSRING:
            MOVE_TO_STATE(SOUNDMENUST_SMSRING)
            return NFSMACTION_CONTINUE;
#endif

        case DLGRET_CANCELED:
            MOVE_TO_STATE(SOUNDMENUST_SOUNDMENU)
            return NFSMACTION_CONTINUE;

        default:
            ASSERT_NOT_REACHABLE;
    }

    return NFSMACTION_WAIT;
} // StateVolumeSubHandler
#endif


/*==============================================================================
函数：
       StateExitHandler
说明：
       SOUNDMENUST_EXIT 状态处理函数

参数：
       pMe [in]：指向SoundMenu Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/
static NextFSMAction Sound_StateExitHandler(CSoundMenu *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    (void) ISHELL_CloseApplet(pMe->m_pShell, FALSE);

    return NFSMACTION_WAIT;
} // StateExitHandler








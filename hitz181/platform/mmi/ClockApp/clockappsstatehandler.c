/*==============================================================================
// 文件：
//        ClockAppsStateHandler.c
//        2007-11-12  Gemsea草拟版本(Draft Version)
//        版权所有(c) 2007 Anylook
//
// 文件说明：
//
// 作者：2007-11-12
// 创建日期：Gemsea
// 当前版本：Draft
==============================================================================*/

/*==============================================================================
修改历史记录：
时       间      修 改 人     修改内容、位置及原因
-----------      ----------     -----------------------------------------------
2007-11-12           初始版本
==============================================================================*/


/*==============================================================================
                                 本文件包含的外部文件
==============================================================================*/
#include "ClockApps_priv.h"

/*==============================================================================
                                 宏定义和常数
==============================================================================*/


/*==============================================================================
                                 类型定义
==============================================================================*/


/*==============================================================================
                                 函数声明
==============================================================================*/
// 状态 ALARMST_NONE 处理函数
static NextFSMAction StateNoOperationHandler(CClockApps *pMe);

// 状态 ALARMST_INIT 处理函数
static NextFSMAction StateInitHandler(CClockApps *pMe);

// 状态 ALARMST_MAIN 处理函数
static NextFSMAction StateAlarmMainHandler(CClockApps *pMe);

// 状态 ALARMST_OPTION 处理函数
static NextFSMAction StateAlarmOptionHandler(CClockApps *pMe);

// 状态 ALARMST_SUB 处理函数
static NextFSMAction StateAlarmSubHandler(CClockApps *pMe);

// 状态 ALARMST_MESSEGE 处理函数
//static NextFSMAction StateAlarmMessegeHandler(CClockApps *pMe);

// 状态 ALARMST_REACH 处理函数
static NextFSMAction StateAlarmTimeReachHandler(CClockApps *pMe);

// 状态 ALARMST_EXIT 处理函数
static NextFSMAction StateExitHandler(CClockApps *pMe);



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
       ClockApps_ProcessState
说明:
       ClockApps Applet状态处理主函数。函数根据pMe->m_currState将调用路由至
       相应的处理函数。

参数:
       pMe [In]: 指向ClockApps Applet对象结构的指针,该结构包含小程序的特定信息.

返回值:
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注:

==============================================================================*/
NextFSMAction ClockApps_ProcessState(CClockApps *pMe)
{
    NextFSMAction retVal = NFSMACTION_WAIT;

    if (NULL == pMe)
    {
        return retVal;
    }

    // 根据当前状态调用相应的状态处理函数
    switch (pMe->m_eCurState)
    {
        case ALARMST_NONE:
            retVal = StateNoOperationHandler(pMe);
            break;

        case ALARMST_INIT:
            retVal = StateInitHandler(pMe);
            break;

        case ALARMST_MAIN:
            retVal = StateAlarmMainHandler(pMe);
            break;

        case ALARMST_OPTION:
            retVal = StateAlarmOptionHandler(pMe);
            break;

        case ALARMST_SUB:
            retVal = StateAlarmSubHandler(pMe);
            break;

        case ALARMST_REACH:
            retVal = StateAlarmTimeReachHandler(pMe);
            break;

        case ALARMST_EXIT:
            retVal = StateExitHandler(pMe);
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
       ALARMST_NONE 状态处理函数

参数：
       pMe [in]：指向ClockApps Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/
static NextFSMAction StateNoOperationHandler(CClockApps *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    MOVE_TO_STATE(ALARMST_EXIT)
    return NFSMACTION_CONTINUE;
} // StateNoOperationHandler


/*==============================================================================
函数：
       StateInitHandler
说明：
       ALARMST_INIT 状态处理函数

参数：
       pMe [in]：指向ClockApps Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/
static NextFSMAction StateInitHandler(CClockApps *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    MOVE_TO_STATE(ALARMST_MAIN)
    return NFSMACTION_CONTINUE;

} // StateInitHandler


/*==============================================================================
函数：
       StateAlarmMainHandler
说明：
       ALARMST_MAIN 状态处理函数

参数：
       pMe [in]：指向ClockApps Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/
static NextFSMAction StateAlarmMainHandler(CClockApps *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            (void)ClockApps_ShowDialog(pMe, IDD_ALARMCLOCK_DLG);
            return NFSMACTION_WAIT;

        case DLGRET_ALARMCLOCK_SUB:
            MOVE_TO_STATE(ALARMST_SUB)
            return NFSMACTION_CONTINUE;

        case DLGRET_ALARMCLOCK_OPTION:
            MOVE_TO_STATE(ALARMST_OPTION)
            return NFSMACTION_CONTINUE;

        case DLGRET_CANCELED:
            MOVE_TO_STATE(ALARMST_EXIT)
            return NFSMACTION_CONTINUE;

        default:
          break;
    }

    return NFSMACTION_WAIT;
} // StateAlarmMainHandler

// 状态 ALARMST_OPTION 处理函数
static NextFSMAction StateAlarmOptionHandler(CClockApps *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            (void)ClockApps_ShowDialog(pMe, IDD_ALARMCLOCK_OPTION);
            return NFSMACTION_WAIT;

        case DLGRET_DELCONFIRM:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            (void)ClockApps_ShowDialog(pMe, IDD_DELCONFIRM);
            return NFSMACTION_WAIT;
            
        case DLGRET_OK:         
            (void)ClockApps_ShowDialog(pMe, IDD_WARNING);
            return NFSMACTION_WAIT;
            
        case DLGRET_ALARMCLOCK_SUB:
            MOVE_TO_STATE(ALARMST_SUB)
            return NFSMACTION_CONTINUE;

        case DLGRET_CANCELED:
        case DLGRET_MSGBOX_OK:
            MOVE_TO_STATE(ALARMST_MAIN)
            return NFSMACTION_CONTINUE;

        default:
          break;
    }

    return NFSMACTION_WAIT;
} // StateAlarmOptionHandler
/*==============================================================================
函数：
       StateAlarmSubHandler
说明：
       ALARMST_SUB 状态处理函数

参数：
       pMe [in]：指向ClockApps Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/
static NextFSMAction StateAlarmSubHandler(CClockApps *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            (void)ClockApps_ShowDialog(pMe, IDD_ALARMCLOCK_SUB);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
        case DLGRET_OK:
            MOVE_TO_STATE(ALARMST_MAIN)
            return NFSMACTION_CONTINUE;

        default:
             break;
    }

    return NFSMACTION_WAIT;
} // StateAlarmSubHandler
///*==============================================================================
//函数：
//       StateAlarmMessegeHandler
//说明：
//       ALARMST_MESSEGE 状态处理函数
//
//参数：
//       pMe [in]：指向ClockApps Applet对象结构的指针。该结构包含小程序的特定信息。
//
//返回值：
//       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
//       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。
//
//备注：
//
//==============================================================================*/
//static NextFSMAction StateAlarmMessegeHandler(CClockApps *pMe)
//{
//    if (NULL == pMe)
//    {
//        return NFSMACTION_WAIT;
//    }
//
//    switch(pMe->m_eDlgRet)
//    {
//        case DLGRET_CREATE:
//            pMe->m_bNotOverwriteDlgRet = FALSE;
//            (void)ClockApps_ShowDialog(pMe, IDD_ALARMCLOCK_MESSEGE);
//            return NFSMACTION_WAIT;
//
//        case DLGRET_OK:
//            MOVE_TO_STATE(ALARMST_MAIN)
//            return NFSMACTION_CONTINUE;
//
//        case DLGRET_CANCELED:
//            MOVE_TO_STATE(ALARMST_SUB)
//            return NFSMACTION_CONTINUE;
//
//        default:
//            break;
//    }
//
//    return NFSMACTION_WAIT;
//} // StateAlarmMessegeHandler
//

/*==============================================================================
函数：
       StateAlarmTimeReachHandler
说明：
       ALARMST_REACH 状态处理函数

参数：
       pMe [in]：指向ClockApps Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/
static NextFSMAction StateAlarmTimeReachHandler(CClockApps *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            (void)ClockApps_ShowDialog(pMe, IDD_ALARMTIME_REACH);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
            ERR("%d", pMe->m_return, 0, 0);
            switch (pMe->m_return)
            {
                case 1:
                case 4:
                    MOVE_TO_STATE(ALARMST_MAIN)
                    break;

                case 2:
                    MOVE_TO_STATE(ALARMST_SUB)
                    break;

                default:
                    MOVE_TO_STATE(ALARMST_EXIT)
                    break;
            }
            return NFSMACTION_CONTINUE;

        default:
            break;
    }

    return NFSMACTION_WAIT;
} // StateAlarmTimeReachHandler


/*==============================================================================
函数：
       StateExitHandler
说明：
       ALARMST_EXIT 状态处理函数

参数：
       pMe [in]：指向ClockApps Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/
static NextFSMAction StateExitHandler(CClockApps *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    (void) ISHELL_CloseApplet(pMe->m_pShell, FALSE);

    return NFSMACTION_WAIT;
} // StateExitHandler



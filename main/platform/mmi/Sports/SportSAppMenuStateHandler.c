#ifndef _SPORTSAPPMENU_STATE_HANDLER_C_
#define _SPORTSAPPMENU_STATE_HANDLER_C_
/*********************************************************************************

模块名       : <SportApplication>
文件名       : <SportSAppMenuStateHandler.c>
相关文件     : 
文件实现功能 : Sports Application
作者         : 
版本         : 1.0
--------------------------------------------------------------------------------
备注         : 
--------------------------------------------------------------------------------
修改记录 : 

******************************************************************************/



/*==============================================================================
                                 本文件包含的外部文件
==============================================================================*/
#include "SportSApp_priv.h"

/*==============================================================================
                                 宏定义和常数
==============================================================================*/


/*==============================================================================
                                 类型定义
==============================================================================*/


/*==============================================================================
                                 函数声明
==============================================================================*/
// 状态 SPORTSAPP_NONE 处理函数
static NextFSMAction SportSApp_StateNoOperationHandler(CSportsMenu *pMe);

// 状态 SPORTSAPP_INIT 处理函数
static NextFSMAction SportSApp_StateInitHandler(CSportsMenu *pMe);

// 状态 SPORTSAPP_MAIN 处理函数
static NextFSMAction SportSApp_StateMainHandler(CSportsMenu *pMe);

// 状态 SPORTSAPP_PEDOMETER_MAIN 处理函数
static NextFSMAction SportSApp_StatePedometerHandler(CSportsMenu *pMe);

// 状态 SPORTSAPP_PEDOMETER_MAIN 处理函数
static NextFSMAction SportSApp_StatePedometerOptionHandler(CSportsMenu *pMe);

// 状态 SPORTSAPP_PEDOMETER_RECORD 处理函数
static NextFSMAction SportSApp_StatePedometerOptRecordHandler(CSportsMenu *pMe);

// 状态 SPORTSAPP_PEDOMETER_RECORD_VIEW 处理函数
static NextFSMAction SportSApp_StatePedometerRecViewHandler(CSportsMenu *pMe);

// 状态 SPORTSAPP_PERSONAL 处理函数
static NextFSMAction SportSApp_StatePersonalHandler(CSportsMenu *pMe);

// 状态 SPORTSAPP_PERSONAL_OPTION 处理函数
static NextFSMAction SportSApp_StatePersonalOptionHandler(CSportsMenu *pMe);

// 状态 SPORTSAPP_PERSONAL_EDIT处理函数
static NextFSMAction SportSApp_StatePersonalEditHandler(CSportsMenu *pMe);

// 状态 SPORTSAPP_PERSONAL_INPUT处理函数
static NextFSMAction SportSApp_StatePersonalInputHandler(CSportsMenu *pMe);

//状态SPORT_STATE_MSGBOX 处理函数
static NextFSMAction SportSApp_StateMsgBoxHandler(CSportsMenu *pMe);

// 状态 SPORTSAPP_PEDOMETER_BGROUND 处理函数
static NextFSMAction SportSApp_StatePedometerConfirmBgRunHandler(CSportsMenu *pMe);

// 状态 SPORTSAPP_EXIT 处理函数
static NextFSMAction SportSApp_StateExitHandler(CSportsMenu *pMe);

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
       SportSApp_ProcessState
说明:
       SportSApp Applet状态处理主函数。函数根据pMe->m_currState将调用路由至
       相应的处理函数。

参数:
       pMe [In]: 指向SportSApp Applet对象结构的指针,该结构包含小程序的特定信息.

返回值:
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注:

==============================================================================*/
NextFSMAction SportSApp_ProcessState(CSportsMenu *pMe)
{
    NextFSMAction retVal = NFSMACTION_WAIT;

    if (NULL == pMe)
    {
        return retVal;
    }

    switch (pMe->m_eCurState)
    {
        	case SPORT_STATE_NONE:
            		retVal = SportSApp_StateNoOperationHandler(pMe);
            		break;

        	case SPORT_STATE_INIT:
            		retVal = SportSApp_StateInitHandler(pMe);
            		break;

        	case SPORT_STATE_MAIN:
            		retVal = SportSApp_StateMainHandler(pMe);
            		break;
					
		case SPORT_STATE_PETOMETER:
			retVal = SportSApp_StatePedometerHandler(pMe);
			break;
			
		case SPORT_STATE_PETOMETER_OPTION:
			retVal = SportSApp_StatePedometerOptionHandler(pMe);
			break;
			
		case SPORT_STATE_PEDOMETER_RECORD:
			retVal = SportSApp_StatePedometerOptRecordHandler(pMe);
			break;
			
		case SPORT_STATE_PEDOMETER_RECORD_VIEW:
			retVal = SportSApp_StatePedometerRecViewHandler(pMe);
			break;
			
		case SPORT_STATE_PERSONAL:
			retVal = SportSApp_StatePersonalHandler(pMe);
			break;
			
		case SPORT_STATE_PERSONAL_OPTION:
			retVal = SportSApp_StatePersonalOptionHandler(pMe);
			break;
		case SPORT_STATE_PERSONAL_EDIT:
			retVal = SportSApp_StatePersonalEditHandler(pMe);
			break;
			
		case SPORT_STATE_INPUTED:
			retVal = SportSApp_StatePersonalInputHandler(pMe);
			break;
		case SPORT_STATE_MSGBOX:
			retVal = SportSApp_StateMsgBoxHandler(pMe);
			break;
		case SPORT_STATE_APP_BGROUND:
			retVal = SportSApp_StatePedometerConfirmBgRunHandler(pMe);
			break;
        	case SPORT_STATE_EXIT:
            		retVal = SportSApp_StateExitHandler(pMe);
            		break;
        	default:
            		ASSERT_NOT_REACHABLE;
    }
    SPORTS_ERR("curstate %d prestate %d dlgret %d CallApp_ProcessState",pMe->m_eCurState,pMe->m_ePreState,pMe->m_eDlgRet);
    return retVal;
}


/*==============================================================================
函数：
       SportSApp_StateNoOperationHandler
说明：
       SPORT_STATE_NONE 状态处理函数

参数：
       pMe [in]：指向SportSApp Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/
static NextFSMAction SportSApp_StateNoOperationHandler(CSportsMenu *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    MOVE_TO_STATE(SPORT_STATE_EXIT)
    return NFSMACTION_CONTINUE;
} // StateNoOperationHandler


/*==============================================================================
函数：
       SportSApp_StateInitHandler
说明：
       SPORT_STATE_INIT 状态处理函数

参数：
       pMe [in]：指向SportSApp Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/
static NextFSMAction SportSApp_StateInitHandler(CSportsMenu *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    MOVE_TO_STATE(SPORT_STATE_MAIN)
    return NFSMACTION_CONTINUE;
} // StateInitHandler


/*==============================================================================
函数：
       SportSApp_StateMainHandler
说明：
       SPORTSAPP_MAIN 状态处理函数

参数：
       pMe [in]：指向SportSapp Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/
static NextFSMAction SportSApp_StateMainHandler(CSportsMenu *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    pMe->m_nSubDlgId = 0 ;    //重新计算子菜单选择条的位置

    switch(pMe->m_eDlgRet)
    {
	    	case DLGRET_CREATE:
            		pMe->m_bNotOverwriteDlgRet = FALSE;
            		SportSApp_ShowDialog(pMe, IDD_DIALOG_MAIN);
            		return NFSMACTION_WAIT;
				
		case DLGRET_PEDOMETER:
			MOVE_TO_STATE(SPORT_STATE_PETOMETER)
			return NFSMACTION_CONTINUE;
			
		case DLGRET_PERSONAL:
			MOVE_TO_STATE(SPORT_STATE_PERSONAL)
			return NFSMACTION_CONTINUE;
			
        case DLGRET_CANCELED:
            MOVE_TO_STATE(SPORT_STATE_EXIT)
            return NFSMACTION_CONTINUE;

        default:
            ASSERT_NOT_REACHABLE;
    }

    return NFSMACTION_WAIT;
} // StateMainHandler


/*==============================================================================
函数：
       SportSApp_StatePedometerHandler
说明：
       SPORT_STATE_PEDOMETER 状态处理函数

参数：
       pMe [in]：指向SportSApp Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/

static NextFSMAction SportSApp_StatePedometerHandler(CSportsMenu *pMe)
{
	if (NULL == pMe)
	{
		return NFSMACTION_WAIT;
	}

	pMe->m_nSubDlgId = 0 ;	  //重新计算子菜单选择条的位置

	switch(pMe->m_eDlgRet)
	{
		case DLGRET_CREATE:
			pMe->m_bNotOverwriteDlgRet = FALSE;
			SportSApp_ShowDialog(pMe,IDD_DIALOG_PEDOMETER);//IDD_DIALOG_PEDOMETER
			return NFSMACTION_WAIT;
			
		case DLGRET_SPORTS:
			MOVE_TO_STATE(SPORT_STATE_MAIN)
			return NFSMACTION_CONTINUE;
			
		case DLGRET_PEDOMETER:
			MOVE_TO_STATE(SPORT_STATE_PETOMETER)
			return NFSMACTION_CONTINUE;
			
		case DLGRET_PEDOMETER_OPTION:
			MOVE_TO_STATE(SPORT_STATE_PETOMETER_OPTION)
			return NFSMACTION_CONTINUE;
			
		case DLGRET_PEDOMETER_RECORD_VIEW:
			MOVE_TO_STATE(SPORT_STATE_PEDOMETER_RECORD_VIEW)
			return NFSMACTION_CONTINUE;
			
		case DLGRET_MSGBOX:
			MOVE_TO_STATE(SPORT_STATE_MSGBOX)
			return NFSMACTION_CONTINUE;
			
		case DLGRET_CANCELED:
			MOVE_TO_STATE(SPORT_STATE_EXIT)
			return NFSMACTION_CONTINUE;

		default:
			ASSERT_NOT_REACHABLE;
	}

	return NFSMACTION_WAIT;
}

/*==============================================================================
函数：
       SportSApp_StatePedometerOptionHandler
说明：
       SPORT_STATE_EXIT 状态处理函数

参数：
       pMe [in]：指向SportSApp Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/

static NextFSMAction SportSApp_StatePedometerOptionHandler(CSportsMenu *pMe)
{
	if (NULL == pMe)
	{
		return NFSMACTION_WAIT;
	}

	pMe->m_nSubDlgId = 0 ;	  //重新计算子菜单选择条的位置

	switch(pMe->m_eDlgRet)
	{
		case DLGRET_CREATE:
			pMe->m_bNotOverwriteDlgRet = FALSE;
			SportSApp_ShowDialog(pMe,IDD_DIALOG_PEDOMETER_OPTION);//IDD_DIALOG_PEDOMETER
			return NFSMACTION_WAIT;
			
		case DLGRET_SPORTS:
			MOVE_TO_STATE(SPORT_STATE_MAIN)
			return NFSMACTION_CONTINUE;
			
		case DLGRET_PEDOMETER:
			MOVE_TO_STATE(SPORT_STATE_PETOMETER)
			return NFSMACTION_CONTINUE;
			
		case DLGRET_PEDOMETER_OPTION:
			MOVE_TO_STATE(SPORT_STATE_PETOMETER_OPTION)
			return NFSMACTION_CONTINUE;
			
		case DLGRET_PEDOMETER_RECORD:
			MOVE_TO_STATE(SPORT_STATE_PEDOMETER_RECORD)
			return NFSMACTION_CONTINUE;

		case DLGRET_PEDOMETER_RECORD_VIEW:
			MOVE_TO_STATE(SPORT_STATE_PEDOMETER_RECORD_VIEW)
			return NFSMACTION_CONTINUE;
		case DLGRET_MSGBOX:
			MOVE_TO_STATE(SPORT_STATE_MSGBOX)
			return NFSMACTION_CONTINUE;
			
		case DLGRET_CANCELED:
			MOVE_TO_STATE(SPORT_STATE_EXIT)
			return NFSMACTION_CONTINUE;

		default:
			ASSERT_NOT_REACHABLE;
	}

	return NFSMACTION_WAIT;
}

/*==============================================================================
函数：
       SportSApp_StatePedometerRecViewHandler
说明：
       SPORT_STATE_EXIT 状态处理函数

参数：
       pMe [in]：指向SportSApp Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/

static NextFSMAction SportSApp_StatePedometerRecViewHandler(CSportsMenu *pMe)
{
	if (NULL == pMe)
	{
		return NFSMACTION_WAIT;
	}

	pMe->m_nSubDlgId = 0 ;	  //重新计算子菜单选择条的位置

	switch(pMe->m_eDlgRet)
	{
		case DLGRET_CREATE:
			pMe->m_bNotOverwriteDlgRet = FALSE;
			SportSApp_ShowDialog(pMe,IDD_DIALOG_PED_RECORD_VIEW);//IDD_DIALOG_PEDOMETER
			return NFSMACTION_WAIT;
			
		case DLGRET_SPORTS:
			MOVE_TO_STATE(SPORT_STATE_MAIN)
			return NFSMACTION_CONTINUE;
			
		case DLGRET_PEDOMETER:
			MOVE_TO_STATE(SPORT_STATE_PETOMETER)
			return NFSMACTION_CONTINUE;
			
		case DLGRET_PEDOMETER_OPTION:
			MOVE_TO_STATE(SPORT_STATE_PETOMETER_OPTION)
			return NFSMACTION_CONTINUE;
			
		case DLGRET_PEDOMETER_RECORD:
			MOVE_TO_STATE(SPORT_STATE_PEDOMETER_RECORD)
			return NFSMACTION_CONTINUE;

		case DLGRET_PEDOMETER_RECORD_VIEW:
			MOVE_TO_STATE(SPORT_STATE_PEDOMETER_RECORD_VIEW)
			return NFSMACTION_CONTINUE;
			
		case DLGRET_PERSONAL:
			MOVE_TO_STATE(SPORT_STATE_PERSONAL)
			return NFSMACTION_CONTINUE;

            case DLGRET_MSGBOX:
                MOVE_TO_STATE(SPORT_STATE_MSGBOX)
                return NFSMACTION_CONTINUE;  

		case DLGRET_CANCELED:
			MOVE_TO_STATE(SPORT_STATE_EXIT)
			return NFSMACTION_CONTINUE;

		default:
			ASSERT_NOT_REACHABLE;
	}

	return NFSMACTION_WAIT;
}

/*==============================================================================
函数：
       SportSApp_StatePedometerOptRecordHandler
说明：
       SPORT_STATE_EXIT 状态处理函数

参数：
       pMe [in]：指向SportSApp Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/

static NextFSMAction SportSApp_StatePedometerOptRecordHandler(CSportsMenu *pMe)
{
	if (NULL == pMe)
	{
		return NFSMACTION_WAIT;
	}

	pMe->m_nSubDlgId = 0 ;	  //重新计算子菜单选择条的位置

	switch(pMe->m_eDlgRet)
	{
		case DLGRET_CREATE:
			pMe->m_bNotOverwriteDlgRet = FALSE;
			SportSApp_ShowDialog(pMe,IDD_DIALOG_PED_RECORD);//IDD_DIALOG_PEDOMETER
			return NFSMACTION_WAIT;
			
		case DLGRET_SPORTS:
			MOVE_TO_STATE(SPORT_STATE_MAIN)
			return NFSMACTION_CONTINUE;
			
		case DLGRET_PEDOMETER:
			MOVE_TO_STATE(SPORT_STATE_PETOMETER)
			return NFSMACTION_CONTINUE;
			
		case DLGRET_PEDOMETER_OPTION:
			MOVE_TO_STATE(SPORT_STATE_PETOMETER_OPTION)
			return NFSMACTION_CONTINUE;
			
		case DLGRET_PEDOMETER_RECORD:
			MOVE_TO_STATE(SPORT_STATE_PEDOMETER_RECORD)			
			return NFSMACTION_CONTINUE;
			
		case DLGRET_PEDOMETER_RECORD_VIEW:
			MOVE_TO_STATE(SPORT_STATE_PEDOMETER_RECORD_VIEW)
			return NFSMACTION_CONTINUE;
		case DLGRET_PERSONAL:
			MOVE_TO_STATE(SPORT_STATE_PERSONAL)
			return NFSMACTION_CONTINUE;
        case DLGRET_MSGBOX:
            MOVE_TO_STATE(SPORT_STATE_MSGBOX)
            return NFSMACTION_CONTINUE;    
		case DLGRET_CANCELED:
			MOVE_TO_STATE(SPORT_STATE_EXIT)
			return NFSMACTION_CONTINUE;

		default:
			ASSERT_NOT_REACHABLE;
	}

	return NFSMACTION_WAIT;
}

/*==============================================================================
函数：
       SportSApp_StatePersonalHandler
说明：
       SPORT_STATE_EXIT 状态处理函数

参数：
       pMe [in]：指向SportSApp Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/

static NextFSMAction SportSApp_StatePersonalHandler(CSportsMenu *pMe)
{
	if (NULL == pMe)
	{
		return NFSMACTION_WAIT;
	}

	pMe->m_nSubDlgId = 0 ;	  //重新计算子菜单选择条的位置

	switch(pMe->m_eDlgRet)
	{
		case DLGRET_CREATE:
			pMe->m_bNotOverwriteDlgRet = FALSE;
			SportSApp_ShowDialog(pMe,IDD_DIALOG_PERSONAL);//IDD_DIALOG_PERSONAL
			return NFSMACTION_WAIT;
			
		case DLGRET_SPORTS:
			MOVE_TO_STATE(SPORT_STATE_MAIN)
			return NFSMACTION_CONTINUE;
			
		case DLGRET_PEDOMETER:
			MOVE_TO_STATE(SPORT_STATE_PETOMETER)
			return NFSMACTION_CONTINUE;
			
		case DLGRET_PEDOMETER_OPTION:
			MOVE_TO_STATE(SPORT_STATE_PETOMETER_OPTION)
			return NFSMACTION_CONTINUE;
			
		case DLGRET_PEDOMETER_RECORD:
			MOVE_TO_STATE(SPORT_STATE_PEDOMETER_RECORD)			
			return NFSMACTION_CONTINUE;
			
		case DLGRET_PEDOMETER_RECORD_VIEW:
			MOVE_TO_STATE(SPORT_STATE_PEDOMETER_RECORD_VIEW)
			return NFSMACTION_CONTINUE;
			
		case DLGRET_PERSONAL:
			MOVE_TO_STATE(SPORT_STATE_PERSONAL)
			return NFSMACTION_CONTINUE;
			
		case DLGRET_PERSONAL_OPTION:
			MOVE_TO_STATE(SPORT_STATE_PERSONAL_OPTION)
			return NFSMACTION_CONTINUE;
			
		case DLGRET_PERSONAL_EDIT:
			MOVE_TO_STATE(SPORT_STATE_PERSONAL_EDIT)
				
			return NFSMACTION_CONTINUE;
		case DLGRET_CANCELED:
			MOVE_TO_STATE(SPORT_STATE_EXIT)
			return NFSMACTION_CONTINUE;

		default:
			ASSERT_NOT_REACHABLE;
	}

	return NFSMACTION_WAIT;
}

/*==============================================================================
函数：
       SportSApp_StatePersonalOptionHandler
说明：
       SPORT_STATE_EXIT 状态处理函数

参数：
       pMe [in]：指向SportSApp Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/

static NextFSMAction SportSApp_StatePersonalOptionHandler(CSportsMenu *pMe)
{
	if (NULL == pMe)
	{
		return NFSMACTION_WAIT;
	}

	pMe->m_nSubDlgId = 0 ;	  //重新计算子菜单选择条的位置

	switch(pMe->m_eDlgRet)
	{
		case DLGRET_CREATE:
			pMe->m_bNotOverwriteDlgRet = FALSE;
			SportSApp_ShowDialog(pMe,IDD_DIALOG_PER_OPTION);//IDD_DIALOG_PERSONAL
			return NFSMACTION_WAIT;
			
		case DLGRET_SPORTS:
			MOVE_TO_STATE(SPORT_STATE_MAIN)
			return NFSMACTION_CONTINUE;
			
		case DLGRET_PEDOMETER:
			MOVE_TO_STATE(SPORT_STATE_PETOMETER)
			return NFSMACTION_CONTINUE;
			
		case DLGRET_PEDOMETER_OPTION:
			MOVE_TO_STATE(SPORT_STATE_PETOMETER_OPTION)
			return NFSMACTION_CONTINUE;
			
		case DLGRET_PEDOMETER_RECORD:
			MOVE_TO_STATE(SPORT_STATE_PEDOMETER_RECORD)			
			return NFSMACTION_CONTINUE;
			
		case DLGRET_PEDOMETER_RECORD_VIEW:
			MOVE_TO_STATE(SPORT_STATE_PEDOMETER_RECORD_VIEW)
			return NFSMACTION_CONTINUE;
			
		case DLGRET_PERSONAL:
			MOVE_TO_STATE(SPORT_STATE_PERSONAL)
			return NFSMACTION_CONTINUE;
		case DLGRET_PERSONAL_OPTION:
			MOVE_TO_STATE(SPORT_STATE_PERSONAL_OPTION)
			return NFSMACTION_CONTINUE;		
			
		case DLGRET_PERSONAL_EDIT:
			MOVE_TO_STATE(DLGRET_PERSONAL_EDIT)
			return NFSMACTION_CONTINUE;
			
		case DLGRET_CANCELED:
			MOVE_TO_STATE(SPORT_STATE_EXIT)
			return NFSMACTION_CONTINUE;

		default:
			ASSERT_NOT_REACHABLE;
	}

	return NFSMACTION_WAIT;
}

/*==============================================================================
函数：
       SportSApp_StatePersonalEditHandler
说明：
       SPORT_STATE_EXIT 状态处理函数

参数：
       pMe [in]：指向SportSApp Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/

static NextFSMAction SportSApp_StatePersonalEditHandler(CSportsMenu *pMe)
{
	if (NULL == pMe)
	{
		return NFSMACTION_WAIT;
	}

	pMe->m_nSubDlgId = 0 ;	  //重新计算子菜单选择条的位置

	switch(pMe->m_eDlgRet)
	{
		case DLGRET_CREATE:
			pMe->m_bNotOverwriteDlgRet = FALSE;
			SportSApp_ShowDialog(pMe,IDD_DIALOG_PER_EDIT);//IDD_DIALOG_PERSONAL
			return NFSMACTION_WAIT;
			
		case DLGRET_SPORTS:
			MOVE_TO_STATE(SPORT_STATE_MAIN)
			return NFSMACTION_CONTINUE;
			
		case DLGRET_PEDOMETER:
			MOVE_TO_STATE(SPORT_STATE_PETOMETER)
			return NFSMACTION_CONTINUE;
			
		case DLGRET_PEDOMETER_OPTION:
			MOVE_TO_STATE(SPORT_STATE_PETOMETER_OPTION)
			return NFSMACTION_CONTINUE;
			
		case DLGRET_PEDOMETER_RECORD:
			MOVE_TO_STATE(SPORT_STATE_PEDOMETER_RECORD)			
			return NFSMACTION_CONTINUE;
			
		case DLGRET_PEDOMETER_RECORD_VIEW:
			MOVE_TO_STATE(SPORT_STATE_PEDOMETER_RECORD_VIEW)
			return NFSMACTION_CONTINUE;
			
		case DLGRET_PERSONAL:
			MOVE_TO_STATE(SPORT_STATE_PERSONAL)
			return NFSMACTION_CONTINUE;
			
		case DLGRET_PERSONAL_OPTION:
			MOVE_TO_STATE(SPORT_STATE_PERSONAL_OPTION)
			return NFSMACTION_CONTINUE;
			
		case DLGRET_PERSONAL_EDIT:
			MOVE_TO_STATE(SPORT_STATE_PERSONAL_EDIT)
			return NFSMACTION_CONTINUE;
			
		case DLGRET_EDIT:
			MOVE_TO_STATE(SPORT_STATE_INPUTED)
			return NFSMACTION_CONTINUE;
				
		case DLGRET_INPUT:
			MOVE_TO_STATE(SPORT_STATE_INPUTED)
			return NFSMACTION_CONTINUE;
					
		case DLGRET_MSGBOX:
			MOVE_TO_STATE(SPORT_STATE_MSGBOX)
			return NFSMACTION_CONTINUE;

		case DLGRET_CANCELED:
			MOVE_TO_STATE(SPORT_STATE_EXIT)
			return NFSMACTION_CONTINUE;

		default:
			ASSERT_NOT_REACHABLE;
	}

	return NFSMACTION_WAIT;
}

/*==============================================================================
函数：
       SportSApp_StatePersonalInputHandler
说明：
       SPORT_STATE_EXIT 状态处理函数

参数：
       pMe [in]：指向SportSApp Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/

static NextFSMAction SportSApp_StatePersonalInputHandler(CSportsMenu *pMe)
{
	if (NULL == pMe)
	{
		return NFSMACTION_WAIT;
	}

	pMe->m_nSubDlgId = 0 ;	  //重新计算子菜单选择条的位置

	switch(pMe->m_eDlgRet)
	{
		case DLGRET_CREATE:
			pMe->m_bNotOverwriteDlgRet = FALSE;
			SportSApp_ShowDialog(pMe,IDD_DIALOG_INPUT);//IDD_DIALOG_PERSONAL
			return NFSMACTION_WAIT;
			
		case DLGRET_SPORTS:
			MOVE_TO_STATE(SPORT_STATE_MAIN)
			return NFSMACTION_CONTINUE;
			
		case DLGRET_PEDOMETER:
			MOVE_TO_STATE(SPORT_STATE_PETOMETER)
			return NFSMACTION_CONTINUE;
			
		case DLGRET_PEDOMETER_OPTION:
			MOVE_TO_STATE(SPORT_STATE_PETOMETER_OPTION)
			return NFSMACTION_CONTINUE;
			
		case DLGRET_PEDOMETER_RECORD:
			MOVE_TO_STATE(SPORT_STATE_PEDOMETER_RECORD)			
			return NFSMACTION_CONTINUE;
			
		case DLGRET_PEDOMETER_RECORD_VIEW:
			MOVE_TO_STATE(SPORT_STATE_PEDOMETER_RECORD_VIEW)
			return NFSMACTION_CONTINUE;
			
		case DLGRET_PERSONAL:
			MOVE_TO_STATE(SPORT_STATE_PERSONAL)
			return NFSMACTION_CONTINUE;
		case DLGRET_PERSONAL_OPTION:
			MOVE_TO_STATE(SPORT_STATE_PERSONAL_OPTION)
			return NFSMACTION_CONTINUE;			
		case DLGRET_PERSONAL_EDIT:
			MOVE_TO_STATE(SPORT_STATE_PERSONAL_EDIT)
			return NFSMACTION_CONTINUE;

		case DLGRET_EDIT:
			MOVE_TO_STATE(SPORT_STATE_INPUTED)
			return NFSMACTION_CONTINUE;
			
		case DLGRET_OK:
			MOVE_TO_STATE(SPORT_STATE_PERSONAL_EDIT)
			return NFSMACTION_CONTINUE;
			
		case DLGRET_INPUT:
			MOVE_TO_STATE(SPORT_STATE_INPUTED)
			return NFSMACTION_CONTINUE;
			
		case DLGRET_MSGBOX:
			MOVE_TO_STATE(SPORT_STATE_MSGBOX)
			return NFSMACTION_CONTINUE;
	
		case DLGRET_CANCELED:
			MOVE_TO_STATE(SPORT_STATE_EXIT)
			return NFSMACTION_CONTINUE;

		default:
			ASSERT_NOT_REACHABLE;
	}

	return NFSMACTION_WAIT;
}

/*==============================================================================
函数：
       SportSApp_StateHandler
说明：
       SPORT_STATE_EXIT 状态处理函数

参数：
       pMe [in]：指向SportSApp Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/

static NextFSMAction SportSApp_StateMsgBoxHandler(CSportsMenu *pMe)
{
	if (NULL == pMe)
	{
		return NFSMACTION_WAIT;
	}

	pMe->m_nSubDlgId = 0 ;	  //重新计算子菜单选择条的位置

	switch(pMe->m_eDlgRet)
	{
		case DLGRET_CREATE:
			pMe->m_bNotOverwriteDlgRet = FALSE;
	            // Show dialog
	            if(SUCCESS != SportSApp_ShowMsgBox(pMe, pMe->m_pSportsAppMenu.m_wMsgResID))
	            {
	                MOVE_TO_STATE(SPORT_STATE_EXIT)
	                return NFSMACTION_CONTINUE;
	            }

			return NFSMACTION_WAIT;
			
		case DLGRET_SPORTS:
			MOVE_TO_STATE(SPORT_STATE_MAIN)
			return NFSMACTION_CONTINUE;
			
		case DLGRET_PEDOMETER:
			MOVE_TO_STATE(SPORT_STATE_PETOMETER)
			return NFSMACTION_CONTINUE;
			
		case DLGRET_PEDOMETER_OPTION:
			MOVE_TO_STATE(SPORT_STATE_PETOMETER_OPTION)
			return NFSMACTION_CONTINUE;
			
		case DLGRET_PEDOMETER_RECORD:
			MOVE_TO_STATE(SPORT_STATE_PEDOMETER_RECORD) 		
			return NFSMACTION_CONTINUE;
			
		case DLGRET_PEDOMETER_RECORD_VIEW:
			MOVE_TO_STATE(SPORT_STATE_PEDOMETER_RECORD_VIEW)
			return NFSMACTION_CONTINUE;
			
		case DLGRET_PERSONAL:
			MOVE_TO_STATE(SPORT_STATE_PERSONAL)
			return NFSMACTION_CONTINUE;
		case DLGRET_PERSONAL_OPTION:
			MOVE_TO_STATE(SPORT_STATE_PERSONAL_OPTION)
			return NFSMACTION_CONTINUE; 		
		case DLGRET_PERSONAL_EDIT:
			MOVE_TO_STATE(SPORT_STATE_PERSONAL_EDIT)
			return NFSMACTION_CONTINUE;

		case DLGRET_EDIT:
			MOVE_TO_STATE(SPORT_STATE_PERSONAL_EDIT)
			return NFSMACTION_CONTINUE;
			
		case DLGRET_OK:
			MOVE_TO_STATE(SPORT_STATE_PERSONAL_EDIT)
			return NFSMACTION_CONTINUE;
			
		case DLGRET_INPUT:
			MOVE_TO_STATE(SPORT_STATE_INPUTED)
			return NFSMACTION_CONTINUE;
		
		case DLGRET_CANCELED:
			MOVE_TO_STATE(SPORT_STATE_EXIT)
			return NFSMACTION_CONTINUE;

		default:
			ASSERT_NOT_REACHABLE;
	}

	return NFSMACTION_WAIT;
}

/*==============================================================================
函数：
       SportSApp_StatePedometerConfirmBgRunHandler
说明：
       SPORT_STATE_EXIT 状态处理函数

参数：
       pMe [in]：指向SportSApp Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/
static NextFSMAction SportSApp_StatePedometerConfirmBgRunHandler(CSportsMenu *pMe)
{
	if (NULL == pMe)
	{
		return NFSMACTION_WAIT;
	}

	pMe->m_nSubDlgId = 0 ;	  //重新计算子菜单选择条的位置

	switch(pMe->m_eDlgRet)
	{
		case DLGRET_CREATE:
			pMe->m_bNotOverwriteDlgRet = FALSE;
			SportSApp_ShowDialog(pMe,IDD_APP_BGROUND);
			return NFSMACTION_WAIT;
			
		case DLGRET_SPORTS:
			MOVE_TO_STATE(SPORT_STATE_MAIN)
			return NFSMACTION_CONTINUE;
			
		case DLGRET_PEDOMETER:
			MOVE_TO_STATE(SPORT_STATE_PETOMETER)
			return NFSMACTION_CONTINUE;
			
		case DLGRET_PEDOMETER_OPTION:
			MOVE_TO_STATE(SPORT_STATE_PETOMETER_OPTION)
			return NFSMACTION_CONTINUE;
			
		case DLGRET_PEDOMETER_RECORD_VIEW:
			MOVE_TO_STATE(SPORT_STATE_PEDOMETER_RECORD_VIEW)
			return NFSMACTION_CONTINUE;
		case DLGRET_APP_BGROND:
			MOVE_TO_STATE(SPORT_STATE_APP_BGROUND)
			return NFSMACTION_CONTINUE;
		case DLGRET_CANCELED:
			MOVE_TO_STATE(SPORT_STATE_EXIT)
			return NFSMACTION_CONTINUE;

		default:
			ASSERT_NOT_REACHABLE;
	}

	return NFSMACTION_WAIT;
}


/*==============================================================================
函数：
       SportSApp_StateExitHandler
说明：
       SPORT_STATE_EXIT 状态处理函数

参数：
       pMe [in]：指向SportSApp Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/
static NextFSMAction SportSApp_StateExitHandler(CSportsMenu *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    (void) ISHELL_CloseApplet(pMe->m_pShell, FALSE);

    return NFSMACTION_WAIT;
} // StateExitHandler

#endif

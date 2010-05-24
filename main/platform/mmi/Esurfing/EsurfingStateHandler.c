#ifndef _ESURFING_STATE_HANDLER_C_
#define _ESURFING_STATE_HANDLER_C_
/*******************************************************************
*	Copyright(c)2008-2009 TCL CDMA SW developement Team Reserved
*
********************************************************************/
/********************************************************************

	Team		: 	CDMA JRD SW
	Author		: 	Qiong.chen Create 
	Version 	:	1.0.0
	Function Name 	:
	Change Time 	:
		Qiong.chen Create 2009/02/04
		
*********************************************************************/

/*==============================================================================
								 本文件包含的外部文件
==============================================================================*/
#include "Esurfing_priv.h"

/*==============================================================================
                                 函数声明
==============================================================================*/
// 状态 MORSE_NONE 处理函数
static NextFSMAction Esurfing_StateNoOperationHandler(CEsurfingMenu *pMe);

// 状态 MORSE_INIT 处理函数
static NextFSMAction Esurfing_StateInitHandler(CEsurfingMenu *pMe);

// 状态 MORSE_MAIN 处理函数
static NextFSMAction Esurfing_StateMainHandler(CEsurfingMenu *pMe);

// 状态 Customer Hotline 处理函数
static NextFSMAction Esurfing_StateBestToneHandler(CEsurfingMenu *pMe);


// 状态Exit Handler 处理函数
static NextFSMAction Esurfing_StateExitHandler(CEsurfingMenu *pMe);

// 状态 MORSE_FULLNAME 处理函数
static NextFSMAction Esurfing_StateMsgBoxHandler(CEsurfingMenu *pMe);


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
NextFSMAction Esurfing_ProcessState(CEsurfingMenu *pMe)
{
	NextFSMAction retVal = NFSMACTION_WAIT;

	if (NULL == pMe)
	{
		return retVal;
	}

	switch (pMe->m_eCurState)
	{
			case ESURFING_STATE_NONE:
					retVal = Esurfing_StateNoOperationHandler(pMe);
					break;

			case ESURFING_STATE_INIT:
					retVal = Esurfing_StateInitHandler(pMe);
					break;
        
			case ESURFING_STATE_MAIN:
					retVal = Esurfing_StateMainHandler(pMe);
					break;	
            
	    case ESURFING_STATE_BESTTONE:
	        retVal = Esurfing_StateBestToneHandler(pMe);
	        break;
	              
			case ESURFING_STATE_MSGBOX:
					retVal = Esurfing_StateMsgBoxHandler(pMe);
					break;	
            
			case ESURFING_STATE_EXIT:
					retVal = Esurfing_StateExitHandler(pMe);
					break;
            
		default:
			break;
	}
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
static NextFSMAction Esurfing_StateNoOperationHandler(CEsurfingMenu *pMe)
{
	if (NULL == pMe)
	{
		return NFSMACTION_WAIT;
	}

	MOVE_TO_STATE(ESURFING_STATE_EXIT)
	return NFSMACTION_CONTINUE;
}  

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
static NextFSMAction Esurfing_StateInitHandler(CEsurfingMenu *pMe)
{
	if (NULL == pMe)
	{
		return NFSMACTION_WAIT;
	}

	MOVE_TO_STATE(ESURFING_STATE_MAIN)
	return NFSMACTION_CONTINUE;
} 

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
static NextFSMAction Esurfing_StateMainHandler(CEsurfingMenu *pMe)
{
	if (NULL == pMe)
	{
		return NFSMACTION_WAIT;
	}

	pMe->m_nSubDlgId = 0 ; 

	switch(pMe->m_eDlgRet)
	{
		case DLGRET_CREATE:
			pMe->m_bNotOverwriteDlgRet = FALSE;
			Esurfing_ShowDialog(pMe,IDD_DIALOG_MAIN);
			return NFSMACTION_WAIT;
        
		case DLGRET_ESURFING_BESTTONE:
			MOVE_TO_STATE(ESURFING_STATE_BESTTONE)
			return NFSMACTION_CONTINUE;
            
		case DLGRET_MSGBOX:
			MOVE_TO_STATE(ESURFING_STATE_MSGBOX)
			return NFSMACTION_CONTINUE;	
            
		case DLGRET_CANCELED:
			MOVE_TO_STATE(ESURFING_STATE_EXIT)
			return NFSMACTION_CONTINUE;

		default:
			break;
	}

	return NFSMACTION_WAIT;
} // StateMainHandler

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
static NextFSMAction Esurfing_StateBestToneHandler(CEsurfingMenu *pMe)
{
	if (NULL == pMe)
	{
		return NFSMACTION_WAIT;
	}

	pMe->m_nSubDlgId = 0 ; 

	switch(pMe->m_eDlgRet)
	{
		case DLGRET_CREATE:
			pMe->m_bNotOverwriteDlgRet = FALSE;
			Esurfing_ShowDialog(pMe,IDD_DIALOG_BESTTONE);
			return NFSMACTION_WAIT;
			
		case DLGRET_ESURFING_MAIN:
			MOVE_TO_STATE(ESURFING_STATE_MAIN)
			return NFSMACTION_CONTINUE;
	
    case DLGRET_MSGBOX:
	    MOVE_TO_STATE(ESURFING_STATE_MSGBOX)
	    return NFSMACTION_CONTINUE; 

		case DLGRET_CANCELED:
			MOVE_TO_STATE(ESURFING_STATE_EXIT)
			return NFSMACTION_CONTINUE;

		default:
			break;
	}

	return NFSMACTION_WAIT;
} // StateMainHandler


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
static NextFSMAction Esurfing_StateExitHandler(CEsurfingMenu *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    (void) ISHELL_CloseApplet(pMe->m_pShell, FALSE);

    return NFSMACTION_WAIT;
} 


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
static NextFSMAction Esurfing_StateMsgBoxHandler(CEsurfingMenu *pMe)
{
	if (NULL == pMe)
	{
		return NFSMACTION_WAIT;
	}

	pMe->m_nSubDlgId = 0 ; 

	switch(pMe->m_eDlgRet)
	{
		case DLGRET_CREATE:
			pMe->m_bNotOverwriteDlgRet = FALSE;
			Esurfing_ShowDialog(pMe, IDD_DIALOG_MSGBOX);
			return NFSMACTION_WAIT;
			
		case DLGRET_ESURFING_MAIN:
			MOVE_TO_STATE(ESURFING_STATE_MAIN)
			return NFSMACTION_CONTINUE;
			
		case DLGRET_ESURFING_BESTTONE:
			MOVE_TO_STATE(ESURFING_STATE_BESTTONE)
			return NFSMACTION_CONTINUE;
            
		case DLGRET_CANCELED:
			MOVE_TO_STATE(ESURFING_STATE_EXIT)
			return NFSMACTION_CONTINUE;

		default:
			break;
	}

	return NFSMACTION_WAIT;
} 

#endif

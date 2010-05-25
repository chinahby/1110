#ifndef _CUSTSERVICE_STATE_HANDLER_C_
#define _CUSTSERVICE_STATE_HANDLER_C_
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
#include "Custservice_priv.h"

/*==============================================================================
                                 函数声明
==============================================================================*/
// 状态 MORSE_NONE 处理函数
static NextFSMAction CustService_StateNoOperationHandler(CCustServiceMenu *pMe);

// 状态 MORSE_INIT 处理函数
static NextFSMAction CustService_StateInitHandler(CCustServiceMenu *pMe);

// 状态 MORSE_MAIN 处理函数
static NextFSMAction CustService_StateMainHandler(CCustServiceMenu *pMe);

// 状态 Customer Hotline 处理函数
static NextFSMAction CustService_StateCustHotlineHandler(CCustServiceMenu *pMe);

// 状态 Mobile E Hall 处理函数
static NextFSMAction CustService_StateMobileEHallHandler(CCustServiceMenu *pMe);

// 状态 Calling Secretary 处理函数
static NextFSMAction CustService_StateCallSecretaryHandler(CCustServiceMenu *pMe);

// 状态 My Subscription 处理函数
static NextFSMAction CustService_StateMySubscriptionHandler(CCustServiceMenu *pMe);

// 状态 Phone Service Guide 处理函数
static NextFSMAction CustService_StatePhoneSerGuideHandler(CCustServiceMenu *pMe);

// 状态Exit Handler 处理函数
static NextFSMAction CustService_StateExitHandler(CCustServiceMenu *pMe);

// 状态 MORSE_FULLNAME 处理函数
static NextFSMAction CustService_StateMsgBoxHandler(CCustServiceMenu *pMe);


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
NextFSMAction CustService_ProcessState(CCustServiceMenu *pMe)
{
	NextFSMAction retVal = NFSMACTION_WAIT;

	if (NULL == pMe)
	{
		return retVal;
	}

	switch (pMe->m_eCurState)
	{
		case CUSTSERVICE_STATE_NONE:
			retVal = CustService_StateNoOperationHandler(pMe);
			break;

		case CUSTSERVICE_STATE_INIT:
			retVal = CustService_StateInitHandler(pMe);
			break;
        
		case CUSTSERVICE_STATE_MAIN:
			retVal = CustService_StateMainHandler(pMe);
			break;	
            
	    case CUSTSERVICE_STATE_CUSTHOTLINE:
            retVal = CustService_StateCustHotlineHandler(pMe);
            break;
            
        case CUSTSERVICE_STATE_MOBILEHALL:
            retVal = CustService_StateMobileEHallHandler(pMe);
            break;
            
        case CUSTSERVICE_STATE_CALLSECRETARY:
            retVal = CustService_StateCallSecretaryHandler(pMe);
            break;
            
        case CUSTSERVICE_STATE_SUBSCRIPTION:
            retVal = CustService_StateMySubscriptionHandler(pMe);
            break;
            
        case CUSTSERVICE_STATE_PHSERGUIDE:
            retVal = CustService_StatePhoneSerGuideHandler(pMe);
            break;
            
		case CUSTSERVICE_STATE_MSGBOX:
			retVal = CustService_StateMsgBoxHandler(pMe);
			break;	
            
		case CUSTSERVICE_STATE_EXIT:
			retVal = CustService_StateExitHandler(pMe);
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
static NextFSMAction CustService_StateNoOperationHandler(CCustServiceMenu *pMe)
{
	if (NULL == pMe)
	{
		return NFSMACTION_WAIT;
	}

	MOVE_TO_STATE(CUSTSERVICE_STATE_EXIT)
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
static NextFSMAction CustService_StateInitHandler(CCustServiceMenu *pMe)
{
	if (NULL == pMe)
	{
		return NFSMACTION_WAIT;
	}

	MOVE_TO_STATE(CUSTSERVICE_STATE_MAIN)
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
static NextFSMAction CustService_StateMainHandler(CCustServiceMenu *pMe)
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
			CustService_ShowDialog(pMe,IDD_DIALOG_MAIN);//test by Joe 090204
			return NFSMACTION_WAIT;
        
		case DLGRET_CUSTSERVICE_CUSTHOTLINE:
			MOVE_TO_STATE(CUSTSERVICE_STATE_CUSTHOTLINE)
			return NFSMACTION_CONTINUE;

        case DLGRET_CUSTSERVICE_MOBILEHALL:
            MOVE_TO_STATE(CUSTSERVICE_STATE_MOBILEHALL)
            return NFSMACTION_CONTINUE;

        case DLGRET_CUSTSERVICE_CALLSECRETARY:
            MOVE_TO_STATE(CUSTSERVICE_STATE_CALLSECRETARY)
            return NFSMACTION_CONTINUE;
            
        case DLGRET_CUSTSERVICE_SUBSCRIPTION:
            MOVE_TO_STATE(CUSTSERVICE_STATE_SUBSCRIPTION)
            return NFSMACTION_CONTINUE;
            
        case DLGRET_CUSTSERVICE_PHSERGUIDE:
            MOVE_TO_STATE(CUSTSERVICE_STATE_PHSERGUIDE)
            return NFSMACTION_CONTINUE;
            
		case DLGRET_MSGBOX:
			MOVE_TO_STATE(CUSTSERVICE_STATE_MSGBOX)
			return NFSMACTION_CONTINUE;	
            
		case DLGRET_CANCELED:
			MOVE_TO_STATE(CUSTSERVICE_STATE_EXIT)
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
static NextFSMAction CustService_StateCustHotlineHandler(CCustServiceMenu *pMe)
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
			CustService_ShowDialog(pMe,IDD_DIALOG_HOTLINE);
			return NFSMACTION_WAIT;
			
		case DLGRET_CUSTSERVICE_MAIN:
			MOVE_TO_STATE(CUSTSERVICE_STATE_MAIN)
			return NFSMACTION_CONTINUE;
	
        case DLGRET_MSGBOX:
                MOVE_TO_STATE(CUSTSERVICE_STATE_MSGBOX)
                return NFSMACTION_CONTINUE; 

		case DLGRET_CANCELED:
			MOVE_TO_STATE(CUSTSERVICE_STATE_EXIT)
			return NFSMACTION_CONTINUE;

		default:
			break;
	}

	return NFSMACTION_WAIT;
} // StateMainHandler

/*==============================================================================
函数：
       Morse_StateFullNameHandler
说明：
       SPORTSAPP_MAIN 状态处理函数

参数：
       pMe [in]：指向SportSapp Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/
static NextFSMAction CustService_StateMobileEHallHandler(CCustServiceMenu *pMe)
{
	if (NULL == pMe)
	{
		return NFSMACTION_WAIT;
	}

	pMe->m_nSubDlgId = 0; 

	switch(pMe->m_eDlgRet)
	{
		case DLGRET_CREATE:
			pMe->m_bNotOverwriteDlgRet = FALSE;
			CustService_ShowDialog(pMe,IDD_DIALOG_MOBILEHALL);
			return NFSMACTION_WAIT;
			
		case DLGRET_CUSTSERVICE_MAIN:
			MOVE_TO_STATE(CUSTSERVICE_STATE_MAIN)
			return NFSMACTION_CONTINUE;
			
		case DLGRET_CUSTSERVICE_MOBILEHALL:
			MOVE_TO_STATE(CUSTSERVICE_STATE_MOBILEHALL)
			return NFSMACTION_CONTINUE;
            
		case DLGRET_MSGBOX:
			MOVE_TO_STATE(CUSTSERVICE_STATE_MSGBOX)
			return NFSMACTION_CONTINUE;	
            
		case DLGRET_CANCELED:
			MOVE_TO_STATE(CUSTSERVICE_STATE_EXIT)
			return NFSMACTION_CONTINUE;

		default:
			break;
	}

	return NFSMACTION_WAIT;
} // StateMainHandler

/*==============================================================================
函数：
       Morse_StateFullNameHandler
说明：
       SPORTSAPP_MAIN 状态处理函数

参数：
       pMe [in]：指向SportSapp Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/
static NextFSMAction CustService_StateCallSecretaryHandler(CCustServiceMenu *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    pMe->m_nSubDlgId = 0; 

    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            CustService_ShowDialog(pMe,IDD_DIALOG_CALLSECRETARY);
            return NFSMACTION_WAIT;
            
        case DLGRET_CUSTSERVICE_MAIN:
            MOVE_TO_STATE(CUSTSERVICE_STATE_MAIN)
            return NFSMACTION_CONTINUE;
            
        case DLGRET_CUSTSERVICE_CALLSECRETARY:
            MOVE_TO_STATE(CUSTSERVICE_STATE_CALLSECRETARY)
            return NFSMACTION_CONTINUE;
            
        case DLGRET_MSGBOX:
            MOVE_TO_STATE(CUSTSERVICE_STATE_MSGBOX)
            return NFSMACTION_CONTINUE; 
            
        case DLGRET_CANCELED:
            MOVE_TO_STATE(CUSTSERVICE_STATE_EXIT)
            return NFSMACTION_CONTINUE;

        default:
            break;
    }

    return NFSMACTION_WAIT;
}

/*==============================================================================
函数：
       Morse_StateFullNameHandler
说明：
       SPORTSAPP_MAIN 状态处理函数

参数：
       pMe [in]：指向SportSapp Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/
static NextFSMAction CustService_StateMySubscriptionHandler(CCustServiceMenu *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    pMe->m_nSubDlgId = 0; 

    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            CustService_ShowDialog(pMe,IDD_DIALOG_MYSUBSCRIPTION);
            return NFSMACTION_WAIT;
            
        case DLGRET_CUSTSERVICE_MAIN:
            MOVE_TO_STATE(CUSTSERVICE_STATE_MAIN)
            return NFSMACTION_CONTINUE;
            
        case DLGRET_CUSTSERVICE_SUBSCRIPTION:
            MOVE_TO_STATE(CUSTSERVICE_STATE_SUBSCRIPTION)
            return NFSMACTION_CONTINUE;
            
        case DLGRET_MSGBOX:
            MOVE_TO_STATE(CUSTSERVICE_STATE_MSGBOX)
            return NFSMACTION_CONTINUE; 
            
        case DLGRET_CANCELED:
            MOVE_TO_STATE(CUSTSERVICE_STATE_EXIT)
            return NFSMACTION_CONTINUE;

        default:
            break;
    }

    return NFSMACTION_WAIT;
}


static NextFSMAction CustService_StatePhoneSerGuideHandler(CCustServiceMenu *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    pMe->m_nSubDlgId = 0; 

    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            CustService_ShowDialog(pMe,IDD_DIALOG_PHSERGUIDE);
            return NFSMACTION_WAIT;
            
        case DLGRET_CUSTSERVICE_MAIN:
            MOVE_TO_STATE(CUSTSERVICE_STATE_MAIN)
            return NFSMACTION_CONTINUE;
            
        case DLGRET_CUSTSERVICE_PHSERGUIDE:
            MOVE_TO_STATE(CUSTSERVICE_STATE_PHSERGUIDE)
            return NFSMACTION_CONTINUE;
            
        case DLGRET_MSGBOX:
            MOVE_TO_STATE(CUSTSERVICE_STATE_MSGBOX)
            return NFSMACTION_CONTINUE; 
            
        case DLGRET_CANCELED:
            MOVE_TO_STATE(CUSTSERVICE_STATE_EXIT)
            return NFSMACTION_CONTINUE;

        default:
            break;
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
static NextFSMAction CustService_StateExitHandler(CCustServiceMenu *pMe)
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
static NextFSMAction CustService_StateMsgBoxHandler(CCustServiceMenu *pMe)
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
			CustService_ShowDialog(pMe, IDD_DIALOG_MSGBOX);
			return NFSMACTION_WAIT;
			
		case DLGRET_CUSTSERVICE_MAIN:
			MOVE_TO_STATE(CUSTSERVICE_STATE_MAIN)
			return NFSMACTION_CONTINUE;
			
		case DLGRET_CUSTSERVICE_CUSTHOTLINE:
			MOVE_TO_STATE(CUSTSERVICE_STATE_CUSTHOTLINE)
			return NFSMACTION_CONTINUE;
			
		case DLGRET_CUSTSERVICE_MOBILEHALL:
			MOVE_TO_STATE(CUSTSERVICE_STATE_MOBILEHALL)
			return NFSMACTION_CONTINUE;

 		case DLGRET_CUSTSERVICE_CALLSECRETARY:
			MOVE_TO_STATE(CUSTSERVICE_STATE_CALLSECRETARY)
			return NFSMACTION_CONTINUE;
            
        case DLGRET_CUSTSERVICE_SUBSCRIPTION:
            MOVE_TO_STATE(CUSTSERVICE_STATE_SUBSCRIPTION)
            return NFSMACTION_CONTINUE;

        case DLGRET_CUSTSERVICE_PHSERGUIDE:
            MOVE_TO_STATE(CUSTSERVICE_STATE_PHSERGUIDE)
            return NFSMACTION_CONTINUE;
            
		case DLGRET_CANCELED:
			MOVE_TO_STATE(CUSTSERVICE_STATE_EXIT)
			return NFSMACTION_CONTINUE;

		default:
			break;
	}

	return NFSMACTION_WAIT;
} 

#endif

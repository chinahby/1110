#ifndef _NOTEBOOK_STATE_HANDLER_C_
#define _NOTEBOOK_STATE_HANDLER_C_
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
#include "Notebook_priv.h"

/*==============================================================================
                                 函数声明
==============================================================================*/
// 状态 NoOperation 处理函数
static NextFSMAction Notebook_StateNoOperationHandler(CNotebookMenu *pMe);

// 状态 Init 处理函数
static NextFSMAction Notebook_StateInitHandler(CNotebookMenu *pMe);

// 状态 Main 处理函数
static NextFSMAction Notebook_StateMainHandler(CNotebookMenu *pMe);

// 状态 MainOption 处理函数
static NextFSMAction Notebook_StateMainOptionHandler(CNotebookMenu *pMe);

// 状态 Edit 处理函数
static NextFSMAction Notebook_StateEditHandler(CNotebookMenu *pMe);

// 状态 View 处理函数
static NextFSMAction Notebook_StateViewHandler(CNotebookMenu *pMe);

// 状态 EditOption 处理函数
static NextFSMAction Notebook_StateEditOptionHandler(CNotebookMenu *pMe);

// 状态Exit  处理函数
static NextFSMAction Notebook_StateExitHandler(CNotebookMenu *pMe);

// 状态 Message box 处理函数
static NextFSMAction Notebook_StateMsgBoxHandler(CNotebookMenu *pMe);


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
NextFSMAction Notebook_ProcessState(CNotebookMenu *pMe)
{
	NextFSMAction retVal = NFSMACTION_WAIT;

	if (NULL == pMe)
	{
		return retVal;
	}

	switch (pMe->m_eCurState)
	{
        case NOTEBOOK_STATE_NONE:
        		retVal = Notebook_StateNoOperationHandler(pMe);
        		break;

        case NOTEBOOK_STATE_INIT:
        		retVal = Notebook_StateInitHandler(pMe);
        		break;

        case NOTEBOOK_STATE_MAIN:
        		retVal = Notebook_StateMainHandler(pMe);
        		break;	

        case NOTEBOOK_STATE_MAIN_OPTION:
                retVal = Notebook_StateMainOptionHandler(pMe);
                break;
                
        case NOTEBOOK_STATE_EDIT:
                retVal = Notebook_StateEditHandler(pMe);
                break;
                
        case NOTEBOOK_STATE_VIEW:
                retVal = Notebook_StateViewHandler(pMe);
                break;
                
        case NOTEBOOK_STATE_EDIT_OPTION:
        		retVal = Notebook_StateEditOptionHandler(pMe);
        		break;
                
        case NOTEBOOK_STATE_MSGBOX:
        		retVal = Notebook_StateMsgBoxHandler(pMe);
        		break;	

        case NOTEBOOK_STATE_EXIT:
        		retVal = Notebook_StateExitHandler(pMe);
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
static NextFSMAction Notebook_StateNoOperationHandler(CNotebookMenu *pMe)
{
	if (NULL == pMe)
	{
		return NFSMACTION_WAIT;
	}

	MOVE_TO_STATE(NOTEBOOK_STATE_EXIT)
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
static NextFSMAction Notebook_StateInitHandler(CNotebookMenu *pMe)
{
	if (NULL == pMe)
	{
		return NFSMACTION_WAIT;
	}

	MOVE_TO_STATE(NOTEBOOK_STATE_MAIN)
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
static NextFSMAction Notebook_StateMainHandler(CNotebookMenu *pMe)
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
			Notebook_ShowDialog(pMe,IDD_DIALOG_MAIN);
			return NFSMACTION_WAIT;
        
		case DLGRET_NOTEBOOK_MAIN_OPTION:
			MOVE_TO_STATE(NOTEBOOK_STATE_MAIN_OPTION)
			return NFSMACTION_CONTINUE;
            
		case DLGRET_NOTEBOOK_EDIT:
			MOVE_TO_STATE(NOTEBOOK_STATE_EDIT)
			return NFSMACTION_CONTINUE;

		case DLGRET_NOTEBOOK_EDIT_OPTION:
			MOVE_TO_STATE(NOTEBOOK_STATE_EDIT_OPTION)
			return NFSMACTION_CONTINUE;

		case DLGRET_NOTEBOOK_VIEW:
			MOVE_TO_STATE(NOTEBOOK_STATE_VIEW)
			return NFSMACTION_CONTINUE;

            
		case DLGRET_MSGBOX:
			MOVE_TO_STATE(NOTEBOOK_STATE_MSGBOX)
			return NFSMACTION_CONTINUE;	
            
		case DLGRET_CANCELED:
			MOVE_TO_STATE(NOTEBOOK_STATE_EXIT)
			return NFSMACTION_CONTINUE;

		default:
			break;
	}

	return NFSMACTION_WAIT;
} // StateMainHandler

static NextFSMAction Notebook_StateEditHandler(CNotebookMenu *pMe)
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
			Notebook_ShowDialog(pMe,IDD_DIALOG_EDIT);
			return NFSMACTION_WAIT;

		case DLGRET_NOTEBOOK_MAIN:
			MOVE_TO_STATE(NOTEBOOK_STATE_MAIN)
			return NFSMACTION_CONTINUE;
            
		case DLGRET_NOTEBOOK_MAIN_OPTION:
			MOVE_TO_STATE(NOTEBOOK_STATE_MAIN_OPTION)
			return NFSMACTION_CONTINUE;
            
		case DLGRET_NOTEBOOK_EDIT:
			MOVE_TO_STATE(NOTEBOOK_STATE_EDIT)
			return NFSMACTION_CONTINUE;

		case DLGRET_NOTEBOOK_EDIT_OPTION:
			MOVE_TO_STATE(NOTEBOOK_STATE_EDIT_OPTION)
			return NFSMACTION_CONTINUE;

		case DLGRET_NOTEBOOK_VIEW:
			MOVE_TO_STATE(NOTEBOOK_STATE_VIEW)
			return NFSMACTION_CONTINUE;
            
		case DLGRET_MSGBOX:
			MOVE_TO_STATE(NOTEBOOK_STATE_MSGBOX)
			return NFSMACTION_CONTINUE;	
            
		case DLGRET_CANCELED:
			MOVE_TO_STATE(NOTEBOOK_STATE_EXIT)
			return NFSMACTION_CONTINUE;

		default:
			break;
	}

	return NFSMACTION_WAIT;
} // StateMainHandler

static NextFSMAction Notebook_StateViewHandler(CNotebookMenu *pMe)
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
			Notebook_ShowDialog(pMe,IDD_DIALOG_VIEW);
			return NFSMACTION_WAIT;

		case DLGRET_NOTEBOOK_MAIN:
			MOVE_TO_STATE(NOTEBOOK_STATE_MAIN)
			return NFSMACTION_CONTINUE;
            
		case DLGRET_NOTEBOOK_MAIN_OPTION:
			MOVE_TO_STATE(NOTEBOOK_STATE_MAIN_OPTION)
			return NFSMACTION_CONTINUE;
            
		case DLGRET_NOTEBOOK_EDIT:
			MOVE_TO_STATE(NOTEBOOK_STATE_EDIT)
			return NFSMACTION_CONTINUE;

		case DLGRET_NOTEBOOK_EDIT_OPTION:
			MOVE_TO_STATE(NOTEBOOK_STATE_EDIT_OPTION)
			return NFSMACTION_CONTINUE;

		case DLGRET_NOTEBOOK_VIEW:
			MOVE_TO_STATE(NOTEBOOK_STATE_VIEW)
			return NFSMACTION_CONTINUE;
            
		case DLGRET_MSGBOX:
			MOVE_TO_STATE(NOTEBOOK_STATE_MSGBOX)
			return NFSMACTION_CONTINUE;	
            
		case DLGRET_CANCELED:
			MOVE_TO_STATE(NOTEBOOK_STATE_EXIT)
			return NFSMACTION_CONTINUE;

		default:
			break;
	}

	return NFSMACTION_WAIT;
} // StateMainHandler

/*==============================================================================
函数：
       Notebook_StateMainOptionHandler
说明：
       NOTEBOOK_MAIN 状态处理函数

参数：
       pMe [in]：指向Notebook Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/
static NextFSMAction Notebook_StateMainOptionHandler(CNotebookMenu *pMe)
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
			Notebook_ShowDialog(pMe,IDD_MAIN_OPTION);
			return NFSMACTION_WAIT;
			
		case DLGRET_NOTEBOOK_MAIN:
			MOVE_TO_STATE(NOTEBOOK_STATE_MAIN)
			return NFSMACTION_CONTINUE;

        case DLGRET_NOTEBOOK_EDIT:
            MOVE_TO_STATE(NOTEBOOK_STATE_EDIT)
            return NFSMACTION_CONTINUE;
            
		case DLGRET_NOTEBOOK_VIEW:
			MOVE_TO_STATE(NOTEBOOK_STATE_VIEW)
			return NFSMACTION_CONTINUE;	
            
        case DLGRET_MSGBOX:
    	    MOVE_TO_STATE(NOTEBOOK_STATE_MSGBOX)
    	    return NFSMACTION_CONTINUE; 

		case DLGRET_CANCELED:
			MOVE_TO_STATE(NOTEBOOK_STATE_EXIT)
			return NFSMACTION_CONTINUE;

		default:
			break;
	}

	return NFSMACTION_WAIT;
} // StateMainHandler


static NextFSMAction Notebook_StateEditOptionHandler(CNotebookMenu *pMe)
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
			Notebook_ShowDialog(pMe,IDD_EDIT_OPTION);
			return NFSMACTION_WAIT;
			
		case DLGRET_NOTEBOOK_MAIN:
			MOVE_TO_STATE(NOTEBOOK_STATE_MAIN)
			return NFSMACTION_CONTINUE;

        case DLGRET_NOTEBOOK_EDIT:
            MOVE_TO_STATE(NOTEBOOK_STATE_EDIT)
            return NFSMACTION_CONTINUE;

        case DLGRET_NOTEBOOK_EDIT_OPTION:
            MOVE_TO_STATE(NOTEBOOK_STATE_EDIT_OPTION)
            return NFSMACTION_CONTINUE;

		case DLGRET_NOTEBOOK_VIEW:
			MOVE_TO_STATE(NOTEBOOK_STATE_VIEW)
			return NFSMACTION_CONTINUE;
            
        case DLGRET_MSGBOX:
    	    MOVE_TO_STATE(NOTEBOOK_STATE_MSGBOX)
    	    return NFSMACTION_CONTINUE; 

		case DLGRET_CANCELED:
			MOVE_TO_STATE(NOTEBOOK_STATE_EXIT)
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
static NextFSMAction Notebook_StateExitHandler(CNotebookMenu *pMe)
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
static NextFSMAction Notebook_StateMsgBoxHandler(CNotebookMenu *pMe)
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
			Notebook_ShowDialog(pMe, IDD_DIALOG_MSGBOX);
			return NFSMACTION_WAIT;
			
		case DLGRET_NOTEBOOK_MAIN:
			MOVE_TO_STATE(NOTEBOOK_STATE_MAIN)
			return NFSMACTION_CONTINUE;
			
		case DLGRET_NOTEBOOK_MAIN_OPTION:
			MOVE_TO_STATE(NOTEBOOK_STATE_MAIN_OPTION)
			return NFSMACTION_CONTINUE;
            
		case DLGRET_NOTEBOOK_EDIT:
			MOVE_TO_STATE(NOTEBOOK_STATE_EDIT)
			return NFSMACTION_CONTINUE;

		case DLGRET_NOTEBOOK_EDIT_OPTION:
			MOVE_TO_STATE(NOTEBOOK_STATE_EDIT_OPTION)
			return NFSMACTION_CONTINUE;

		case DLGRET_NOTEBOOK_VIEW:
			MOVE_TO_STATE(NOTEBOOK_STATE_VIEW)
			return NFSMACTION_CONTINUE;
            
		case DLGRET_CANCELED:
			MOVE_TO_STATE(NOTEBOOK_STATE_EXIT)
			return NFSMACTION_CONTINUE;

		default:
			break;
	}

	return NFSMACTION_WAIT;
} 

#endif

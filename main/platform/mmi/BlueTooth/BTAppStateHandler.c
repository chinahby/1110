/*==============================================================================
// 文件：
//        BTAppStateHandler.c
//        版权所有(c) 2009 Anylook Tech. CO.,LTD.
//
// 文件说明：
//
// 作者：zzg
// 创建日期：2011-01-04
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
#include "BTApp_priv.h"		//"BTApp.h"


/*==============================================================================
                                 宏定义和常数
==============================================================================*/


/*==============================================================================
                                 类型定义
==============================================================================*/


/*==============================================================================
                                 函数声明
==============================================================================*/
// 状态 BTAPPST_NONE 处理函数
static NextFSMAction BTApp_StateNoOperationHandler(CBTApp *pMe);

// 状态 BTAPPST_INIT 处理函数
static NextFSMAction BTApp_StateInitHandler(CBTApp *pMe);

// 状态 BTAPPST_MAIN 处理函数
static NextFSMAction BTApp_StateMainHandler(CBTApp *pMe);

// 状态 BTAPPST_DEVICESRH 处理函数
static NextFSMAction BTApp_StateDeviceSearchHandler(CBTApp *pMe);

// 状态 BTAPPST_SRHRESULT 处理函数
static NextFSMAction BTApp_StateSearchResultHandler(CBTApp *pMe);
// 状态 BTAPPST_DEVICEINFO 处理函数
static NextFSMAction BTApp_StateDeviceInfoHandler(CBTApp *pMe);
// 状态 BTAPPST_DEVICEINFO_OPITION 处理函数
static NextFSMAction BTApp_StateDeviceInfoOpitionHandler(CBTApp *pMe);
// 状态 BTAPPST_BOND_OPITION 处理函数
static NextFSMAction BTApp_StateBondOpitionHandler(CBTApp *pMe);

// 状态 BTAPPST_DEVICE 处理函数
static NextFSMAction BTApp_StateDeviceHandler(CBTApp *pMe);
// 状态 BTAPPST_DEVICE_LIST 处理函数
static NextFSMAction BTApp_StateDeviceListHandler(CBTApp *pMe);

// 状态 BTAPPST_BOND_LIST 处理函数
static NextFSMAction BTApp_StateBondListHandler(CBTApp *pMe);

// 状态 BTAPPST_ALL_LIST 处理函数
static NextFSMAction BTApp_StateAllListHandler(CBTApp *pMe);

// 状态 BTAPPST_CONNECT_LIST 处理函数
static NextFSMAction BTApp_StateConnectListHandler(CBTApp *pMe);



// 状态 BTAPPST_MYINFO 处理函数
static NextFSMAction BTApp_StateMyInfoHandler(CBTApp *pMe);
// 状态 BTAPPST_MYINFO_OPITION 处理函数
static NextFSMAction BTApp_StateMyInfoOpitionHandler(CBTApp *pMe);
// 状态 BTAPPST_MYINFO_EDIT 处理函数
static NextFSMAction BTApp_StateMyInfoEditHandler(CBTApp *pMe);
// 状态 BTAPPST_SET_SECURITY 处理函数
static NextFSMAction BTApp_StateSetSecurityHandler(CBTApp *pMe);
// 状态 BTAPPST_SET_DISCOVERABLE 处理函数
static NextFSMAction BTApp_StateSetDiscoverableHandler(CBTApp *pMe);
// 状态 BTAPPST_SET_IOCAPABILITY 处理函数
static NextFSMAction BTApp_StateSetIOCapabilityHandler(CBTApp *pMe);
// 状态 BTAPPST_SET_DEBUGKEY 处理函数
static NextFSMAction BTApp_StateSetDebugKeyHandler(CBTApp *pMe);



// 状态 BTAPPST_FTP 处理函数
static NextFSMAction BTApp_StateFtpHandler(CBTApp *pMe);
// 状态 BTAPPST_FTP_SERVER 处理函数
static NextFSMAction BTApp_StateFtpServerHandler(CBTApp *pMe);
// 状态 BTAPPST_FTP_CLIENT 处理函数
static NextFSMAction BTApp_StateFtpClientHandler(CBTApp *pMe);
// 状态 BTAPPST_FTP_SETTING 处理函数
static NextFSMAction BTApp_StateFtpSettingHandler(CBTApp *pMe);
// 状态 BTAPPST_FTP_SERVER_REGISTER 处理函数
static NextFSMAction BTApp_StateFtpServerRegisterHandler(CBTApp *pMe);
// 状态 BTAPPST_FTP_BROWSE 处理函数
static NextFSMAction BTApp_StateFtpBrowseHandler(CBTApp *pMe);
// 状态 BTAPPST_FTP_BROWSE_OPITION 处理函数
static NextFSMAction BTApp_StateFtpBrowseOpitionHandler(CBTApp *pMe);





// 状态 BTAPPST_BT_MSGBOX 处理函数
//static NextFSMAction BTApp_StateMsgBoxHandler(CBTApp *pMe);
// 状态 BTAPPST_PROMPT 处理函数
//static NextFSMAction BTApp_StatePromptHandler(CBTApp *pMe);

// 状态 BTAPPST_BT_EDIT 处理函数
static NextFSMAction BTApp_StateEditHandler(CBTApp *pMe);
// 状态 BTAPPST_BT_SEND_FILE 处理函数
static NextFSMAction BTApp_StateSendFileHandler(CBTApp *pMe);
// 状态 BTAPPST_BT_OBEX_LIST_SERVERS 处理函数
static NextFSMAction BTApp_StateObexListServersHandler(CBTApp *pMe);
// 状态 BTAPPST_BT_FILE_PROGRESS 处理函数
//static NextFSMAction BTApp_StateFileProgressHandler(CBTApp *pMe);



// 状态 BTAPPST_EXIT 处理函数
static NextFSMAction BTApp_StateExitHandler(CBTApp *pMe);


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
       BTApp_ProcessState
说明:
       BTApp Applet状态处理主函数。函数根据pMe->m_currState将调用路由至
       相应的处理函数。

参数:
       pMe [In]: 指向BTApp Applet对象结构的指针,该结构包含小程序的特定信息.

返回值:
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注:

==============================================================================*/

#define ASSERT_BT_NOT_REACHABLE 


NextFSMAction BTApp_ProcessState(CBTApp *pMe)
{
    NextFSMAction retVal = NFSMACTION_WAIT;

    MSG_FATAL("***zzg BTApp_ProcessState Start pMe->m_eCurState=%d***", pMe->m_eCurState,0,0);
	
    if (NULL == pMe)
    {
    	MSG_FATAL("***zzg BTApp_ProcessState NULL == pMe***", 0,0,0);
        return retVal;
    }
	
    // 根据当前状态调用相应的状态处理函数
    switch (pMe->m_eCurState)
    {
        case BTAPPST_NONE:			
        {
			retVal = BTApp_StateNoOperationHandler(pMe);
            break;
        }

        case BTAPPST_INIT:			
        {
			retVal = BTApp_StateInitHandler(pMe);
            break;
		}
	
        case BTAPPST_MAIN:           
        {
			retVal = BTApp_StateMainHandler(pMe);
            break;
		}	
			
        case BTAPPST_DEVICESRH:
        {
			retVal = BTApp_StateDeviceSearchHandler(pMe);
            break;
		}	

        case BTAPPST_SRHRESULT:			
        {
			retVal = BTApp_StateSearchResultHandler(pMe);
            break;
		}	

		case BTAPPST_DEVICEINFO:			
        {
			retVal = BTApp_StateDeviceInfoHandler(pMe);
            break;
		}	
			
		case BTAPPST_DEVICEINFO_OPITION:			
        {
			retVal = BTApp_StateDeviceInfoOpitionHandler(pMe);
            break;	
		}	

		case BTAPPST_BOND_OPITION:			
        {
			retVal = BTApp_StateBondOpitionHandler(pMe);
            break;	
		}	

		case BTAPPST_DEVICE:
        {
			retVal = BTApp_StateDeviceHandler(pMe);
            break;
		}	
		case BTAPPST_DEVICE_LIST:
        {
			retVal = BTApp_StateDeviceListHandler(pMe);
            break;
		}
			
        case BTAPPST_MYINFO:			
        {
			retVal = BTApp_StateMyInfoHandler(pMe);
            break;
		}	
			
		case BTAPPST_MYINFO_OPITION:			
        {
			retVal = BTApp_StateMyInfoOpitionHandler(pMe);
            break;
		}	

		//BTAPPST_EDIT_NAME/BTAPPST_EDIT_SHORTNAME/BTAPPST_EDIT_MANUDATA
		case BTAPPST_MYINFO_EDIT:
        {
			retVal = BTApp_StateMyInfoEditHandler(pMe);
            break;
		}      

		case BTAPPST_SET_SECURITY:
        {   
			retVal = BTApp_StateSetSecurityHandler(pMe);
            break;
		}	
			
        case BTAPPST_SET_DISCOVERABLE:
        {    
			retVal = BTApp_StateSetDiscoverableHandler(pMe);
            break;
		}	

        case BTAPPST_SET_IOCAPABILITY:
        {   
			retVal = BTApp_StateSetIOCapabilityHandler(pMe);
            break;
		}	

        case BTAPPST_SET_DEBUGKEY:
        {    
			retVal = BTApp_StateSetDebugKeyHandler(pMe);
            break;
		}	

		case BTAPPST_FTP:
        {    
			retVal = BTApp_StateFtpHandler(pMe);
            break;	
		}	

		case BTAPPST_FTP_SERVER:
        {    
			retVal = BTApp_StateFtpServerHandler(pMe);
            break;
		}	

		case BTAPPST_FTP_CLIENT:
        {    
			retVal = BTApp_StateFtpClientHandler(pMe);
            break;
		}	

		case BTAPPST_FTP_SETTING:
        {    
			retVal = BTApp_StateFtpSettingHandler(pMe);
            break;	
		}	

		case BTAPPST_FTP_SERVER_REGISTER:
        {    
			retVal = BTApp_StateFtpServerRegisterHandler(pMe);
            break;
		}	

		case BTAPPST_FTP_BROWSE:
        {   
			retVal = BTApp_StateFtpBrowseHandler(pMe);
            break;
		}	

		case BTAPPST_FTP_BROWSE_OPITION:
        {    
			retVal = BTApp_StateFtpBrowseOpitionHandler(pMe);
            break;	
		}	

		/*	
		case BTAPPST_BT_MSGBOX:
        {    
			retVal = BTApp_StateMsgBoxHandler(pMe);
            break;	
		}	

		
        case BTAPPST_PROMPT:			
        {    
			retVal = BTApp_StatePromptHandler(pMe);
            break;
		}
		*/
			
		case BTAPPST_BT_EDIT:
        {    
			retVal = BTApp_StateEditHandler(pMe);
            break;
		}	

		case BTAPPST_BT_SEND_FILE:
        {    
			retVal = BTApp_StateSendFileHandler(pMe);
            break;	
		}	

		case BTAPPST_BT_OBEX_LIST_SERVERS:			
        {    
			retVal = BTApp_StateObexListServersHandler(pMe);
            break;	
		}	

		/*	
		case BTAPPST_BT_FILE_PROGRESS:			
        {    
			retVal = BTApp_StateFileProgressHandler(pMe);
            break;
		}	
		*/
			
		case BTAPPST_EXIT:
		{	
			retVal = BTApp_StateExitHandler(pMe);
            break;
		}	

        default:
        {   
			ASSERT_BT_NOT_REACHABLE;
		}	
    }
	
    MSG_FATAL("curstate %d prestate %d dlgret %d BTApp_ProcessState",pMe->m_eCurState,pMe->m_ePreState,pMe->m_eDlgRet);
	
    return retVal;
}


/*==============================================================================
函数：
       StateNoOperationHandler
说明：
       BTAPPST_NONE 状态处理函数

参数：
       pMe [in]：指向BTApp Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/
static NextFSMAction BTApp_StateNoOperationHandler(CBTApp *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

	MSG_FATAL("***zzg BTApp_StateNoOperationHandler***", 0, 0, 0);

    MOVE_TO_STATE(BTAPPST_EXIT)
    return NFSMACTION_CONTINUE;
} // StateNoOperationHandler


/*==============================================================================
函数：
       StateInitHandler
说明：
       BTAPPST_INIT 状态处理函数

参数：
       pMe [in]：指向BTApp Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/
static NextFSMAction BTApp_StateInitHandler(CBTApp *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
	
    MOVE_TO_STATE(BTAPPST_MAIN)
    return NFSMACTION_CONTINUE;
} // StateInitHandler


/*==============================================================================
函数：
       StateMainHandler
说明：
       BTAPPST_MAIN 状态处理函数

参数：
       pMe [in]：指向BTApp Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/
static NextFSMAction BTApp_StateMainHandler(CBTApp *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

	//重新计算子菜单选择条的位置
    pMe->m_nSubDlgId = 0 ;    

	MSG_FATAL("***zzg BTApp_StateMainHandler m_eDlgRet=%d***", pMe->m_eDlgRet, 0, 0);

    switch (pMe->m_eDlgRet)
    {
    	//case DLGRET_MSGBOX_CANCELED:
    	case DLGRET_PROMPT_CANCELED:
		//case DLGRET_PROGRESS_CANCELED:	
        case DLGRET_CREATE:
        {
			pMe->m_bNotOverwriteDlgRet = FALSE;
            BTApp_ShowDialog(pMe, IDD_BT_MAINMENU);
            return NFSMACTION_WAIT;
        }

		//Add By zzg 2012_11_23
		case DLGRET_MSGBOX_CANCELED:
		case DLGRET_PROGRESS_CANCELED:
		{
			/*
			if (pMe->bStartFromPushReq == TRUE)
			{
				MOVE_TO_STATE(BTAPPST_EXIT)
            	return NFSMACTION_CONTINUE;
			}
			else
			*/	
			{
				pMe->m_bNotOverwriteDlgRet = FALSE;
	            BTApp_ShowDialog(pMe, IDD_BT_MAINMENU);
	            return NFSMACTION_WAIT;
			}
		}
		//Add End

		case DLGRET_DEVICESRH:			
		{
			MOVE_TO_STATE(BTAPPST_DEVICESRH)
            return NFSMACTION_CONTINUE;
		}
			
		case DLGRET_DEVICE:
		{
			MOVE_TO_STATE(BTAPPST_DEVICE)
            return NFSMACTION_CONTINUE;	
		}

		case DLGRET_MYINFO:			
		{
			MOVE_TO_STATE(BTAPPST_MYINFO)
            return NFSMACTION_CONTINUE;
		}

		case DLGRET_FTP:
		{
			MOVE_TO_STATE(BTAPPST_FTP)
            return NFSMACTION_CONTINUE;		
		}
		

		case DLGRET_BT_MSGBOX:
		{
			//MOVE_TO_STATE(BTAPPST_BT_MSGBOX)
			//return NFSMACTION_CONTINUE;	

			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_MSGBOX);
			return NFSMACTION_WAIT;
		}

		case DLGRET_PROMPT:
		{
			//MOVE_TO_STATE(BTAPPST_PROMPT)
			//return NFSMACTION_CONTINUE;	
			
			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_PROMPT);	
            return NFSMACTION_WAIT;
		}	

		case DLGRET_BT_FILE_PROGRESS:
		{
			//MOVE_TO_STATE(BTAPPST_BT_FILE_PROGRESS)
			//return NFSMACTION_CONTINUE;	

			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_FILE_PROGRESS);
			return NFSMACTION_WAIT;
		}

		case DLGRET_BT_EDIT:			
		{
			MOVE_TO_STATE(BTAPPST_BT_EDIT)
			return NFSMACTION_CONTINUE;			
		}	
		
        case DLGRET_CANCELED:
        {
			MOVE_TO_STATE(BTAPPST_EXIT)
            return NFSMACTION_CONTINUE;
        }

		case DLGRET_SRHRESULT:
		{	
			MOVE_TO_STATE(BTAPPST_SRHRESULT)
            return NFSMACTION_CONTINUE;
		}	

        default:
        {
			ASSERT_BT_NOT_REACHABLE;
        }
    }

    return NFSMACTION_WAIT;
} // StateMainHandler


// 状态 BTAPPST_DEVICESRH 处理函数
static NextFSMAction BTApp_StateDeviceSearchHandler(CBTApp *pMe)
{
	if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch(pMe->m_eDlgRet)
    {
    	//case DLGRET_MSGBOX_CANCELED:
		case DLGRET_PROMPT_CANCELED:
		case DLGRET_PROGRESS_CANCELED:
        case DLGRET_CREATE:
		{
			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_DEVICE_SEARCH);            
			return NFSMACTION_WAIT;
        }
		
		case DLGRET_SRHRESULT:
		{	
			MOVE_TO_STATE(BTAPPST_SRHRESULT)
            return NFSMACTION_CONTINUE;
		}	

		case DLGRET_BT_MSGBOX:
		{	
			//MOVE_TO_STATE(BTAPPST_BT_MSGBOX)
			//return NFSMACTION_CONTINUE;

			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_MSGBOX);
			return NFSMACTION_WAIT;
		}	

		case DLGRET_PROMPT:
		{	
			//MOVE_TO_STATE(BTAPPST_PROMPT)
			//return NFSMACTION_CONTINUE;

			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_PROMPT);	
            return NFSMACTION_WAIT;
		}

		case DLGRET_BT_FILE_PROGRESS:
		{	
			//MOVE_TO_STATE(BTAPPST_BT_FILE_PROGRESS)
			//return NFSMACTION_CONTINUE;	

			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_FILE_PROGRESS);
			return NFSMACTION_WAIT;
		}

		case DLGRET_BT_EDIT:
		{				
			MOVE_TO_STATE(BTAPPST_BT_EDIT)
			return NFSMACTION_CONTINUE;
		}	
			
		case DLGRET_MSGBOX_CANCELED:	//Add By zzg 2011_09_24	
        case DLGRET_CANCELED:
		{	
			if (pMe->mSD.bDiscovering)
			{
				IBTEXTSD_StopDeviceDiscovery(pMe->mSD.po);
			}
			
			BTApp_CancelDevNameRequest(pMe);

			//Add By zzg 2012_11_22
			MSG_FATAL("***zzg StateSearching bStartFromOtherApp=%d***", pMe->bStartFromOtherApp, 0, 0);
			if (pMe->bStartFromOtherApp == TRUE) 
			{
				MOVE_TO_STATE(BTAPPST_EXIT)
            	return NFSMACTION_CONTINUE;
			}
			else
			//Add End	
			{
	            MOVE_TO_STATE(BTAPPST_MAIN)
	            return NFSMACTION_CONTINUE;
			}
		}
		
        default:
        {    
			ASSERT_BT_NOT_REACHABLE;
		}
    }

    return NFSMACTION_WAIT;
}


// 状态 BTAPPST_SRHRESULT 处理函数
static NextFSMAction BTApp_StateSearchResultHandler(CBTApp *pMe)
{
	if (NULL == pMe)
	{
		return NFSMACTION_WAIT;
	}

	MSG_FATAL("***zzg BTApp_StateSearchResultHandler m_eDlgRet=%x***", pMe->m_eDlgRet, 0, 0);

	switch(pMe->m_eDlgRet)
	{
		//case DLGRET_MSGBOX_CANCELED:
		case DLGRET_PROMPT_CANCELED:
		//case DLGRET_PROGRESS_CANCELED:
		case DLGRET_CREATE:
		{
			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_SEARCH_RESULT);
			return NFSMACTION_WAIT;
		}	

		//Add By zzg 2012_11_22
		case DLGRET_MSGBOX_CANCELED:		
		{
			MSG_FATAL("***zzg StateSearchResult bStartFromOtherApp=%d***", pMe->bStartFromOtherApp, 0, 0);
			if (pMe->bStartFromOtherApp == TRUE) 
			{
				MOVE_TO_STATE(BTAPPST_EXIT)
				return NFSMACTION_CONTINUE;			
			}
			else
			{
				pMe->m_bNotOverwriteDlgRet = FALSE;
				BTApp_ShowDialog(pMe, IDD_SEARCH_RESULT);
				return NFSMACTION_WAIT;
			}
		}	

		case DLGRET_PROGRESS_CANCELED:
		{
			MSG_FATAL("***zzg StateSearchResult bStartFromOtherApp=%d***", pMe->bStartFromOtherApp, 0, 0);
			if (pMe->bStartFromOtherApp == TRUE) 
			{
				MOVE_TO_STATE(BTAPPST_EXIT)
				return NFSMACTION_CONTINUE;			
			}
			else
			{
				pMe->m_bNotOverwriteDlgRet = FALSE;
				BTApp_ShowDialog(pMe, IDD_SEARCH_RESULT);
				return NFSMACTION_WAIT;
			}
		}
		//Add End
			
		case DLGRET_DEVICEINFO:
		{	
			MOVE_TO_STATE(BTAPPST_DEVICEINFO)
			return NFSMACTION_CONTINUE;
		}	
		
		case DLGRET_BT_MSGBOX:
		{	
			//MOVE_TO_STATE(BTAPPST_BT_MSGBOX)
			//return NFSMACTION_CONTINUE;

			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_MSGBOX);
			return NFSMACTION_WAIT;
		}	

		case DLGRET_PROMPT:
		{	
			//MOVE_TO_STATE(BTAPPST_PROMPT)
			//return NFSMACTION_CONTINUE;	
			
			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_PROMPT);	
            return NFSMACTION_WAIT;
		}

		case DLGRET_BT_FILE_PROGRESS:
		{	
			//MOVE_TO_STATE(BTAPPST_BT_FILE_PROGRESS)
			//return NFSMACTION_CONTINUE;	

			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_FILE_PROGRESS);
			return NFSMACTION_WAIT;
		}

		case DLGRET_BT_EDIT:
		{	
			MOVE_TO_STATE(BTAPPST_BT_EDIT)
			return NFSMACTION_CONTINUE;
		}	
						
		case DLGRET_CANCELED:
		{	
			MSG_FATAL("***zzg StateSearchResult bStartFromOtherApp=%d***", pMe->bStartFromOtherApp, 0, 0);
			if (pMe->bStartFromOtherApp == TRUE) 
			{
				MOVE_TO_STATE(BTAPPST_EXIT)
				return NFSMACTION_CONTINUE;			
			}
			else
			{
				MOVE_TO_STATE(BTAPPST_MAIN)
				return NFSMACTION_CONTINUE;
			}			
		}	

		default:
		{	
			ASSERT_BT_NOT_REACHABLE;
		}	
	}

	return NFSMACTION_WAIT;
}

// 状态 BTAPPST_DEVICEINFO 处理函数
static NextFSMAction BTApp_StateDeviceInfoHandler(CBTApp * pMe)
{
	if (NULL == pMe)
	{
		return NFSMACTION_WAIT;
	}

	switch(pMe->m_eDlgRet)
	{
		//case DLGRET_MSGBOX_CANCELED:
		case DLGRET_PROMPT_CANCELED:
		case DLGRET_PROGRESS_CANCELED:	
		case DLGRET_CREATE:
		{
			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_DEVICE_INFO);
			return NFSMACTION_WAIT;
		}

		//Add By zzg 2012_11_23
		case DLGRET_MSGBOX_CANCELED:
		{
			if (pMe->bStartFromOtherApp == TRUE)
			{
				MOVE_TO_STATE(BTAPPST_EXIT)
				return NFSMACTION_CONTINUE;
			}
			else
			{
				pMe->m_bNotOverwriteDlgRet = FALSE;
				BTApp_ShowDialog(pMe, IDD_BT_DEVICE_INFO);
				return NFSMACTION_WAIT;
			}
		}
		//Add End

		case DLGRET_DEVICEINFO_OPITION:
		{
			MOVE_TO_STATE(BTAPPST_DEVICEINFO_OPITION)
			return NFSMACTION_CONTINUE;
		}
		

		case DLGRET_BT_MSGBOX:
		{			
			//MOVE_TO_STATE(BTAPPST_BT_MSGBOX)
			//return NFSMACTION_CONTINUE;

			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_MSGBOX);
			return NFSMACTION_WAIT;
		}
			
		case DLGRET_PROMPT:
		{			
			//MOVE_TO_STATE(BTAPPST_PROMPT)
			//return NFSMACTION_CONTINUE;
			
			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_PROMPT);	
            return NFSMACTION_WAIT;
		}

		case DLGRET_BT_FILE_PROGRESS:
		{
			//MOVE_TO_STATE(BTAPPST_BT_FILE_PROGRESS)
			//return NFSMACTION_CONTINUE;	

			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_FILE_PROGRESS);
			return NFSMACTION_WAIT;
		}

		case DLGRET_BT_EDIT:
		{
			MOVE_TO_STATE(BTAPPST_BT_EDIT)
			return NFSMACTION_CONTINUE;
		}
						
		case DLGRET_CANCELED:
		{
			if (pMe->m_dialog_id == IDD_DEVICE_LIST)				
			{
				MOVE_TO_STATE(BTAPPST_DEVICE_LIST)
			}
			else if (pMe->m_dialog_id == IDD_SEARCH_RESULT)
			{
				MOVE_TO_STATE(BTAPPST_SRHRESULT)
			}
			else
			{
				MOVE_TO_STATE(BTAPPST_MAIN)
			}
			
			return NFSMACTION_CONTINUE;
		}
		default:
		{
			ASSERT_BT_NOT_REACHABLE;
		}
	}

	return NFSMACTION_WAIT;
}


// 状态 BTAPPST_DEVICEINFO_OPITION 处理函数
static NextFSMAction BTApp_StateDeviceInfoOpitionHandler(CBTApp * pMe)
{
	if (NULL == pMe)
	{
		return NFSMACTION_WAIT;
	}

	MSG_FATAL("***zzg BTApp_StateDeviceInfoOpitionHandler m_eDlgRet=%x***", pMe->m_eDlgRet, 0, 0);

	switch(pMe->m_eDlgRet)
	{
		case DLGRET_MSGBOX_CANCELED:
		case DLGRET_PROMPT_CANCELED:
		case DLGRET_PROGRESS_CANCELED:	
		case DLGRET_CREATE:
		{
			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_DEVICE_INFO_OPITION);
			return NFSMACTION_WAIT;
		}

		case DLGRET_BOND_OPITION:
		{
			MOVE_TO_STATE(BTAPPST_BOND_OPITION)
			return NFSMACTION_CONTINUE;		
		}

		case DLGRET_BT_MSGBOX:
		{
			//MOVE_TO_STATE(BTAPPST_BT_MSGBOX)
			//return NFSMACTION_CONTINUE;

			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_MSGBOX);
			return NFSMACTION_WAIT;
		}
			
		case DLGRET_PROMPT:
		{
			//MOVE_TO_STATE(BTAPPST_PROMPT)
			//return NFSMACTION_CONTINUE;
			
			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_PROMPT);	
            return NFSMACTION_WAIT;
		}

		case DLGRET_BT_FILE_PROGRESS:
		{
			//MOVE_TO_STATE(BTAPPST_BT_FILE_PROGRESS)
			//return NFSMACTION_CONTINUE;	

			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_FILE_PROGRESS);
			return NFSMACTION_WAIT;
		}

		case DLGRET_BT_EDIT:
		{
			MOVE_TO_STATE(BTAPPST_BT_EDIT)
			return NFSMACTION_CONTINUE;
		}
			
		case DLGRET_CANCELED:
		{
			MOVE_TO_STATE(BTAPPST_DEVICEINFO)
			return NFSMACTION_CONTINUE;
		}
			
		default:
		{
			ASSERT_BT_NOT_REACHABLE;
		}
	}

	return NFSMACTION_WAIT;
}

// 状态 BTAPPST_BOND_OPITION 处理函数
static NextFSMAction BTApp_StateBondOpitionHandler(CBTApp * pMe)
{
	if (NULL == pMe)
	{
		return NFSMACTION_WAIT;
	}

	switch(pMe->m_eDlgRet)
	{
		case DLGRET_MSGBOX_CANCELED:
		case DLGRET_PROMPT_CANCELED:
		case DLGRET_PROGRESS_CANCELED:	
		case DLGRET_CREATE:			
		{
			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_BOND_OPITION);
			return NFSMACTION_WAIT;
		}
			
		case DLGRET_BT_MSGBOX:
		{
			//MOVE_TO_STATE(BTAPPST_BT_MSGBOX)
			//return NFSMACTION_CONTINUE;
			
			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_MSGBOX);
			return NFSMACTION_WAIT;
		}		
			
		case DLGRET_PROMPT:
		{			
			//MOVE_TO_STATE(BTAPPST_PROMPT)
			//return NFSMACTION_CONTINUE;

			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_PROMPT);	
            return NFSMACTION_WAIT;
		}

		case DLGRET_BT_FILE_PROGRESS:
		{
			//MOVE_TO_STATE(BTAPPST_BT_FILE_PROGRESS)
			//return NFSMACTION_CONTINUE;

			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_FILE_PROGRESS);
			return NFSMACTION_WAIT;
		}

		case DLGRET_BT_EDIT:
		{			
			MOVE_TO_STATE(BTAPPST_BT_EDIT)
			return NFSMACTION_CONTINUE;
		}
						
		case DLGRET_CANCELED:
		{
			MOVE_TO_STATE(BTAPPST_DEVICEINFO)
			return NFSMACTION_CONTINUE;
		}

		default:
		{
			ASSERT_BT_NOT_REACHABLE;
		}
	}

	return NFSMACTION_WAIT;
}


// 状态 BTAPPST_DEVICE 处理函数
static NextFSMAction BTApp_StateDeviceHandler(CBTApp *pMe)
{
	if (NULL == pMe)
	{
		return NFSMACTION_WAIT;
	}

	switch(pMe->m_eDlgRet)
	{		
		case DLGRET_MSGBOX_CANCELED:
		case DLGRET_PROMPT_CANCELED:
		case DLGRET_PROGRESS_CANCELED:	
		case DLGRET_CREATE:
		{
			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_DEVICE);
			return NFSMACTION_WAIT;
		}

		case DLGRET_DEVICE_LIST:
		{
			MOVE_TO_STATE(BTAPPST_DEVICE_LIST)
			return NFSMACTION_CONTINUE;
		}

		case DLGRET_BT_MSGBOX:
		{			
			//MOVE_TO_STATE(BTAPPST_BT_MSGBOX)
			//return NFSMACTION_CONTINUE;

			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_MSGBOX);
			return NFSMACTION_WAIT;
		}
			
		case DLGRET_PROMPT:
		{			
			//MOVE_TO_STATE(BTAPPST_PROMPT)
			//return NFSMACTION_CONTINUE;	

			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_PROMPT);	
            return NFSMACTION_WAIT;
		}

		case DLGRET_BT_FILE_PROGRESS:
		{
			//MOVE_TO_STATE(BTAPPST_BT_FILE_PROGRESS)
			//return NFSMACTION_CONTINUE;

			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_FILE_PROGRESS);
			return NFSMACTION_WAIT;
		}

		case DLGRET_BT_EDIT:
		{			
			MOVE_TO_STATE(BTAPPST_BT_EDIT)
			return NFSMACTION_CONTINUE;
		}
			
		case DLGRET_CANCELED:
		{
			MOVE_TO_STATE(BTAPPST_MAIN)
			return NFSMACTION_CONTINUE;
		}

		default:
		{
			ASSERT_BT_NOT_REACHABLE;
		}
	}

	return NFSMACTION_WAIT;
}


// 状态 BTAPPST_DEVICE_LIST 处理函数
static NextFSMAction BTApp_StateDeviceListHandler(CBTApp *pMe)
{
	if (NULL == pMe)
	{
		return NFSMACTION_WAIT;
	}

	switch(pMe->m_eDlgRet)
	{
		case DLGRET_MSGBOX_CANCELED:
		case DLGRET_PROMPT_CANCELED:
		case DLGRET_PROGRESS_CANCELED:	
		case DLGRET_CREATE:
		{
			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_DEVICE_LIST);
			return NFSMACTION_WAIT;	
		}

		case DLGRET_DEVICEINFO:
		{
			MOVE_TO_STATE(BTAPPST_DEVICEINFO)
			return NFSMACTION_CONTINUE;
		}
			
		case DLGRET_BT_MSGBOX:
		{			
			//MOVE_TO_STATE(BTAPPST_BT_MSGBOX)
			//return NFSMACTION_CONTINUE;
			
			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_MSGBOX);
			return NFSMACTION_WAIT;
		}
			
		case DLGRET_PROMPT:
		{
			//MOVE_TO_STATE(BTAPPST_PROMPT)
			//return NFSMACTION_CONTINUE;		
			
			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_PROMPT);	
            return NFSMACTION_WAIT;
		}	

		case DLGRET_BT_FILE_PROGRESS:
		{
			//MOVE_TO_STATE(BTAPPST_BT_FILE_PROGRESS)
			//return NFSMACTION_CONTINUE;	

			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_FILE_PROGRESS);
			return NFSMACTION_WAIT;
		}

		case DLGRET_BT_EDIT:
		{	
			MOVE_TO_STATE(BTAPPST_BT_EDIT)
			return NFSMACTION_CONTINUE;
		}
			
		case DLGRET_CANCELED:
		{
			MOVE_TO_STATE(BTAPPST_DEVICE)
			return NFSMACTION_CONTINUE;
		}	
	
		default:
		{
			ASSERT_BT_NOT_REACHABLE;
		}	
	}

	return NFSMACTION_WAIT;
}

// 状态 BTAPPST_MYINFO 处理函数
static NextFSMAction BTApp_StateMyInfoHandler(CBTApp *pMe)
{
	if (NULL == pMe)
	{
		return NFSMACTION_WAIT;
	}
	
	switch(pMe->m_eDlgRet)
	{
		case DLGRET_MSGBOX_CANCELED:
		case DLGRET_PROMPT_CANCELED:
		case DLGRET_PROGRESS_CANCELED:	
		case DLGRET_CREATE:
		{	
			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_MYINFO);
			return NFSMACTION_WAIT;
		}	

		case DLGRET_MYINFO_OPITION:			
		{	
			MOVE_TO_STATE(BTAPPST_MYINFO_OPITION)
			return NFSMACTION_CONTINUE;
		}

		case DLGRET_BT_MSGBOX:
		{	
			//MOVE_TO_STATE(BTAPPST_BT_MSGBOX)
			//return NFSMACTION_CONTINUE;

			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_MSGBOX);
			return NFSMACTION_WAIT;
		}	
			
		case DLGRET_PROMPT:
		{	
			//MOVE_TO_STATE(BTAPPST_PROMPT)
			//return NFSMACTION_CONTINUE;		

			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_PROMPT);	
            return NFSMACTION_WAIT;
		}

		case DLGRET_BT_FILE_PROGRESS:
		{	
			//MOVE_TO_STATE(BTAPPST_BT_FILE_PROGRESS)
			//return NFSMACTION_CONTINUE;

			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_FILE_PROGRESS);
			return NFSMACTION_WAIT;
		}

		case DLGRET_BT_EDIT:
		{	
			MOVE_TO_STATE(BTAPPST_BT_EDIT)
			return NFSMACTION_CONTINUE;
		}
		
		case DLGRET_CANCELED:
		{	
			MOVE_TO_STATE(BTAPPST_MAIN)
			return NFSMACTION_CONTINUE;
		}

		default:
		{	
			ASSERT_BT_NOT_REACHABLE;
		}
	}

	return NFSMACTION_WAIT;
}


// 状态 BTAPPST_MYINFO_OPITION 处理函数
static NextFSMAction BTApp_StateMyInfoOpitionHandler(CBTApp *pMe)
{
	if (NULL == pMe)
	{
		return NFSMACTION_WAIT;
	}
	
	switch(pMe->m_eDlgRet)
	{
		case DLGRET_MSGBOX_CANCELED:
		case DLGRET_PROMPT_CANCELED:
		case DLGRET_PROGRESS_CANCELED:	
		case DLGRET_CREATE:
		{	
			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_MYINFO_OPITION);
			return NFSMACTION_WAIT;
		}						

		//Add By zzg 2011_05_28
		case DLGRET_MYINFO_EDIT:
		{
			MOVE_TO_STATE(BTAPPST_MYINFO_EDIT)
			return NFSMACTION_CONTINUE;
		}
		//Add End
		
		case DLGRET_SET_SECURITY:
		{	
			MOVE_TO_STATE(BTAPPST_SET_SECURITY)
			return NFSMACTION_CONTINUE;
		}

		case DLGRET_SET_DISCOVERABLE:
		{	
			MOVE_TO_STATE(BTAPPST_SET_DISCOVERABLE)
			return NFSMACTION_CONTINUE;
		}

		case DLGRET_SET_IOCAPABILITY:
		{	
			MOVE_TO_STATE(BTAPPST_SET_IOCAPABILITY)
			return NFSMACTION_CONTINUE;
		}

		case DLGRET_SET_DEBUGKEY:
		{	
			MOVE_TO_STATE(BTAPPST_SET_DEBUGKEY)
			return NFSMACTION_CONTINUE;
		}

		case DLGRET_BT_MSGBOX:
		{	
			//MOVE_TO_STATE(BTAPPST_BT_MSGBOX)
			//return NFSMACTION_CONTINUE;

			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_MSGBOX);
			return NFSMACTION_WAIT;
		}	
		
		case DLGRET_PROMPT:
		{	
			//MOVE_TO_STATE(BTAPPST_PROMPT)
			//return NFSMACTION_CONTINUE;	

			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_PROMPT);	
            return NFSMACTION_WAIT;
		}	

		case DLGRET_BT_FILE_PROGRESS:
		{	
			//MOVE_TO_STATE(BTAPPST_BT_FILE_PROGRESS)
			//return NFSMACTION_CONTINUE;	
			
			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_FILE_PROGRESS);
			return NFSMACTION_WAIT;
		}

		case DLGRET_BT_EDIT:
		{	
			MOVE_TO_STATE(BTAPPST_BT_EDIT)
			return NFSMACTION_CONTINUE;
		}
					
		case DLGRET_CANCELED:
		{	
			MOVE_TO_STATE(BTAPPST_MYINFO)
			return NFSMACTION_CONTINUE;
		}

		default:
		{	
			ASSERT_BT_NOT_REACHABLE;
		}
	}

	return NFSMACTION_WAIT;
}


// 状态 BTAPPST_MYINFO_EDIT 处理函数
static NextFSMAction BTApp_StateMyInfoEditHandler(CBTApp *pMe)
{
	if (NULL == pMe)
	{
		return NFSMACTION_WAIT;
	}
	
	switch(pMe->m_eDlgRet)
	{
		case DLGRET_MSGBOX_CANCELED:
		case DLGRET_PROMPT_CANCELED:
		case DLGRET_PROGRESS_CANCELED:	
		case DLGRET_CREATE:
		{	
			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_MYINFO_EDIT);
			return NFSMACTION_WAIT;
		}	

		case DLGRET_BT_MSGBOX:
		{	
			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_MSGBOX);
			return NFSMACTION_WAIT;
		}	
			
		case DLGRET_PROMPT:
		{	
			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_PROMPT);	
            return NFSMACTION_WAIT;
		}	

		case DLGRET_BT_FILE_PROGRESS:
		{	
			//MOVE_TO_STATE(BTAPPST_BT_FILE_PROGRESS)
			//return NFSMACTION_CONTINUE;	

			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_FILE_PROGRESS);
			return NFSMACTION_WAIT;
		}

		case DLGRET_BT_EDIT:
		{	
			MOVE_TO_STATE(BTAPPST_BT_EDIT)
			return NFSMACTION_CONTINUE;
		}

		case DLGRET_CANCELED:
		{	
			MOVE_TO_STATE(BTAPPST_MYINFO)
			return NFSMACTION_CONTINUE;
		}
			
		default:
		{	
			ASSERT_BT_NOT_REACHABLE;
		}
	}

	return NFSMACTION_WAIT;
}

// 状态 BTAPPST_SET_SECURITY 处理函数
static NextFSMAction BTApp_StateSetSecurityHandler(CBTApp *pMe)
{
	if (NULL == pMe)
	{
		return NFSMACTION_WAIT;
	}
	
	switch(pMe->m_eDlgRet)
	{
		case DLGRET_MSGBOX_CANCELED:
		case DLGRET_PROMPT_CANCELED:
		case DLGRET_PROGRESS_CANCELED:	
		case DLGRET_CREATE:
		{	
			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_SECURITY);
			return NFSMACTION_WAIT;
		}
	
		case DLGRET_BT_FILE_PROGRESS:
		{	
			//MOVE_TO_STATE(BTAPPST_BT_FILE_PROGRESS)
			//return NFSMACTION_CONTINUE;	

			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_FILE_PROGRESS);
			return NFSMACTION_WAIT;
		}

		case DLGRET_BT_MSGBOX:
		{	
			//MOVE_TO_STATE(BTAPPST_BT_MSGBOX)
			//return NFSMACTION_CONTINUE;

			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_MSGBOX);
			return NFSMACTION_WAIT;
		}	

		case DLGRET_BT_EDIT:
		{	
			MOVE_TO_STATE(BTAPPST_BT_EDIT)
			return NFSMACTION_CONTINUE;
		}	
			
		case DLGRET_PROMPT:
		{	
			//MOVE_TO_STATE(BTAPPST_PROMPT)
			//return NFSMACTION_CONTINUE;	
			
			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_PROMPT);	
            return NFSMACTION_WAIT;
		}	
			
		case DLGRET_CANCELED:
		{	
			MOVE_TO_STATE(BTAPPST_MYINFO)
			return NFSMACTION_CONTINUE;
		}

		default:
		{	
			ASSERT_BT_NOT_REACHABLE;
		}
	}

	return NFSMACTION_WAIT;
}



// 状态 BTAPPST_SET_DISCOVERABLE 处理函数
static NextFSMAction BTApp_StateSetDiscoverableHandler(CBTApp *pMe)
{
	if (NULL == pMe)
	{
		return NFSMACTION_WAIT;
	}

	switch(pMe->m_eDlgRet)
	{
		case DLGRET_MSGBOX_CANCELED:
		case DLGRET_PROMPT_CANCELED:
		case DLGRET_PROGRESS_CANCELED:	
		case DLGRET_CREATE:
		{	
			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_DISCOVERABLE);
			return NFSMACTION_WAIT;
		}	

		case DLGRET_BT_MSGBOX:
		{	
			//MOVE_TO_STATE(BTAPPST_BT_MSGBOX)
			//return NFSMACTION_CONTINUE;

			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_MSGBOX);
			return NFSMACTION_WAIT;
		}			
			
		case DLGRET_PROMPT:
		{	
			//MOVE_TO_STATE(BTAPPST_PROMPT)
			//return NFSMACTION_CONTINUE;	

			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_PROMPT);	
            return NFSMACTION_WAIT;
		}	

		case DLGRET_BT_FILE_PROGRESS:
		{	
			//MOVE_TO_STATE(BTAPPST_BT_FILE_PROGRESS)
			//return NFSMACTION_CONTINUE;	

			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_FILE_PROGRESS);
			return NFSMACTION_WAIT;
		}

		case DLGRET_BT_EDIT:
		{	
			MOVE_TO_STATE(BTAPPST_BT_EDIT)
			return NFSMACTION_CONTINUE;
		}

		case DLGRET_CANCELED:
		{	
			MOVE_TO_STATE(BTAPPST_MYINFO)
			return NFSMACTION_CONTINUE;
		}

		default:
		{	
			ASSERT_BT_NOT_REACHABLE;
		}
	}

	return NFSMACTION_WAIT;
}

// 状态 BTAPPST_SET_IOCAPABILITY 处理函数
static NextFSMAction BTApp_StateSetIOCapabilityHandler(CBTApp *pMe)
{
	if (NULL == pMe)
	{
		return NFSMACTION_WAIT;
	}

	switch(pMe->m_eDlgRet)
	{
		case DLGRET_MSGBOX_CANCELED:
		case DLGRET_PROMPT_CANCELED:
		case DLGRET_PROGRESS_CANCELED:	
		case DLGRET_CREATE:
		{	
			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_IO_CAPABILITY);
			return NFSMACTION_WAIT;
		}	

		case DLGRET_BT_FILE_PROGRESS:
		{	
			//MOVE_TO_STATE(BTAPPST_BT_FILE_PROGRESS)
			//return NFSMACTION_CONTINUE;	

			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_FILE_PROGRESS);
			return NFSMACTION_WAIT;
		}

		case DLGRET_BT_MSGBOX:
		{	
			//MOVE_TO_STATE(BTAPPST_BT_MSGBOX)
			//return NFSMACTION_CONTINUE;
			
			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_MSGBOX);
			return NFSMACTION_WAIT;
		}	

		case DLGRET_BT_EDIT:
		{	
			MOVE_TO_STATE(BTAPPST_BT_EDIT)
			return NFSMACTION_CONTINUE;
		}	
			
		case DLGRET_PROMPT:
		{	
			//MOVE_TO_STATE(BTAPPST_PROMPT)
			//return NFSMACTION_CONTINUE;	

			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_PROMPT);	
            return NFSMACTION_WAIT;
		}	
			
		case DLGRET_CANCELED:
		{	
			MOVE_TO_STATE(BTAPPST_MYINFO)
			return NFSMACTION_CONTINUE;
		}

		default:
		{	
			ASSERT_BT_NOT_REACHABLE;
		}
	}

	return NFSMACTION_WAIT;
}


// 状态 BTAPPST_SET_DEBUGKEY 处理函数
static NextFSMAction BTApp_StateSetDebugKeyHandler(CBTApp *pMe)
{
	if (NULL == pMe)
	{
		return NFSMACTION_WAIT;
	}

	switch(pMe->m_eDlgRet)
	{
		case DLGRET_MSGBOX_CANCELED:
		case DLGRET_PROMPT_CANCELED:
		case DLGRET_PROGRESS_CANCELED:
		case DLGRET_CREATE:
		{	
			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_DEBUG_KEY);
			return NFSMACTION_WAIT;
		}	

		case DLGRET_BT_FILE_PROGRESS:
		{	
			//MOVE_TO_STATE(BTAPPST_BT_FILE_PROGRESS)
			//return NFSMACTION_CONTINUE;	

			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_FILE_PROGRESS);
			return NFSMACTION_WAIT;
		}
			
		case DLGRET_BT_MSGBOX:
		{	
			//MOVE_TO_STATE(BTAPPST_BT_MSGBOX)
			//return NFSMACTION_CONTINUE;

			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_MSGBOX);
			return NFSMACTION_WAIT;
		}	

		case DLGRET_BT_EDIT:
		{	
			MOVE_TO_STATE(BTAPPST_BT_EDIT)
			return NFSMACTION_CONTINUE;
		}	
			
		case DLGRET_PROMPT:
		{	
			//MOVE_TO_STATE(BTAPPST_PROMPT)
			//return NFSMACTION_CONTINUE;

			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_PROMPT);	
            return NFSMACTION_WAIT;
		}	
			
		case DLGRET_CANCELED:
		{	
			MOVE_TO_STATE(BTAPPST_MYINFO)
			return NFSMACTION_CONTINUE;
		}

		default:
		{	
			ASSERT_BT_NOT_REACHABLE;
		}
	}

	return NFSMACTION_WAIT;
}


// 状态 BTAPPST_FTP 处理函数
static NextFSMAction BTApp_StateFtpHandler(CBTApp *pMe)
{
	if (NULL == pMe)
	{
		return NFSMACTION_WAIT;
	}

	switch(pMe->m_eDlgRet)
	{
		case DLGRET_MSGBOX_CANCELED:
		case DLGRET_PROMPT_CANCELED:
		case DLGRET_PROGRESS_CANCELED:	
		case DLGRET_CREATE:
		{	
			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_FTP);
			return NFSMACTION_WAIT;
		}	

		case DLGRET_FTP_SERVER:
		{	
			MOVE_TO_STATE(BTAPPST_FTP_SERVER)
			return NFSMACTION_CONTINUE;
		}

		case DLGRET_FTP_CLIENT:
		{	
			MOVE_TO_STATE(BTAPPST_FTP_CLIENT)
			return NFSMACTION_CONTINUE; 
		}

		case DLGRET_FTP_SETTING:
		{	
			MOVE_TO_STATE(BTAPPST_FTP_SETTING)
			return NFSMACTION_CONTINUE; 
		}
		
		case DLGRET_BT_MSGBOX:
		{	
			//MOVE_TO_STATE(BTAPPST_BT_MSGBOX)
			//return NFSMACTION_CONTINUE;

			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_MSGBOX);
			return NFSMACTION_WAIT;
		}	
			
		case DLGRET_PROMPT:
		{	
			//MOVE_TO_STATE(BTAPPST_PROMPT)
			//return NFSMACTION_CONTINUE;	
						
			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_PROMPT);	
			return NFSMACTION_WAIT;
		}	

		case DLGRET_BT_FILE_PROGRESS:
		{	
			//MOVE_TO_STATE(BTAPPST_BT_FILE_PROGRESS)
			//return NFSMACTION_CONTINUE;	

			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_FILE_PROGRESS);
			return NFSMACTION_WAIT;
		}

		case DLGRET_BT_EDIT:
		{	
			MOVE_TO_STATE(BTAPPST_BT_EDIT)
			return NFSMACTION_CONTINUE;
		}
			
		case DLGRET_CANCELED:
		{	
			MOVE_TO_STATE(BTAPPST_MAIN)
			return NFSMACTION_CONTINUE;
		}

		default:
		{	
			ASSERT_BT_NOT_REACHABLE;
		}
	}

	return NFSMACTION_WAIT;
}


// 状态 BTAPPST_FTP_SERVER 处理函数
static NextFSMAction BTApp_StateFtpServerHandler(CBTApp *pMe)
{
	if (NULL == pMe)
	{
		return NFSMACTION_WAIT;
	}

	switch(pMe->m_eDlgRet)
	{
		case DLGRET_MSGBOX_CANCELED:
		case DLGRET_PROMPT_CANCELED:
		case DLGRET_PROGRESS_CANCELED:		
		case DLGRET_CREATE:
		{	
			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_FTP_SERVER);
			return NFSMACTION_WAIT;
		}	

		case DLGRET_FTP_SERVER_REGISTER:
		{	
			MOVE_TO_STATE(BTAPPST_FTP_SERVER_REGISTER)
			return NFSMACTION_CONTINUE; 
		}
			
		case DLGRET_BT_MSGBOX:
		{	
			//MOVE_TO_STATE(BTAPPST_BT_MSGBOX)
			//return NFSMACTION_CONTINUE;

			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_MSGBOX);
			return NFSMACTION_WAIT;
		}	
		
		case DLGRET_PROMPT:
		{	
			//MOVE_TO_STATE(BTAPPST_PROMPT)
			//return NFSMACTION_CONTINUE;	
			
			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_PROMPT);	
            return NFSMACTION_WAIT;
		}	

		case DLGRET_BT_FILE_PROGRESS:
		{	
			//MOVE_TO_STATE(BTAPPST_BT_FILE_PROGRESS)
			//return NFSMACTION_CONTINUE;	

			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_FILE_PROGRESS);
			return NFSMACTION_WAIT;
		}

		case DLGRET_BT_EDIT:
		{	
			MOVE_TO_STATE(BTAPPST_BT_EDIT)
			return NFSMACTION_CONTINUE;
		}
			
		case DLGRET_CANCELED:
		{	
			MOVE_TO_STATE(BTAPPST_FTP)
			return NFSMACTION_CONTINUE;
		}

		default:
		{	
			ASSERT_BT_NOT_REACHABLE;
		}
	}

	return NFSMACTION_WAIT;
}

// 状态 BTAPPST_FTP_CLIENT处理函数
static NextFSMAction BTApp_StateFtpClientHandler(CBTApp *pMe)
{
	if (NULL == pMe)
	{
		return NFSMACTION_WAIT;
	}

	switch(pMe->m_eDlgRet)
	{
		case DLGRET_MSGBOX_CANCELED:
		case DLGRET_PROMPT_CANCELED:
		case DLGRET_PROGRESS_CANCELED:		
		case DLGRET_CREATE:
		{	
			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_FTP_CLIENT);
			return NFSMACTION_WAIT;
		}	

		case DLGRET_BT_OBEX_LIST_SERVERS:			
		{							
			MOVE_TO_STATE(BTAPPST_BT_OBEX_LIST_SERVERS)
			return NFSMACTION_CONTINUE;
		}	

		case DLGRET_FTP_BROWSE:			
		{							
			MOVE_TO_STATE(BTAPPST_FTP_BROWSE)
			return NFSMACTION_CONTINUE;	
		}		
		
		case DLGRET_BT_MSGBOX:
		{	
			//MOVE_TO_STATE(BTAPPST_BT_MSGBOX)
			//return NFSMACTION_CONTINUE;

			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_MSGBOX);
			return NFSMACTION_WAIT;
		}	
		
		case DLGRET_PROMPT:
		{	
			//MOVE_TO_STATE(BTAPPST_PROMPT)
			//return NFSMACTION_CONTINUE;	

			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_PROMPT);	
            return NFSMACTION_WAIT;
		}	

		case DLGRET_BT_FILE_PROGRESS:
		{	
			//MOVE_TO_STATE(BTAPPST_BT_FILE_PROGRESS)
			//return NFSMACTION_CONTINUE;	

			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_FILE_PROGRESS);
			return NFSMACTION_WAIT;
		}

		case DLGRET_BT_EDIT:
		{	
			MOVE_TO_STATE(BTAPPST_BT_EDIT)
			return NFSMACTION_CONTINUE;
		}	
			
		case DLGRET_CANCELED:
		{	
			MOVE_TO_STATE(BTAPPST_FTP)
			return NFSMACTION_CONTINUE;
		}

		default:
		{	
			ASSERT_BT_NOT_REACHABLE;
		}
	}

	return NFSMACTION_WAIT;
}

// 状态 BTAPPST_FTP_SETTING 处理函数
static NextFSMAction BTApp_StateFtpSettingHandler(CBTApp *pMe)
{
	if (NULL == pMe)
	{
		return NFSMACTION_WAIT;
	}

	switch (pMe->m_eDlgRet)
	{
		case DLGRET_MSGBOX_CANCELED:
		case DLGRET_PROMPT_CANCELED:
		case DLGRET_PROGRESS_CANCELED:		
		case DLGRET_CREATE:
		{	
			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_FTP_SETTINGS);
			return NFSMACTION_WAIT;
		}	
		
		case DLGRET_BT_MSGBOX:
		{	
			//MOVE_TO_STATE(BTAPPST_BT_MSGBOX)
			//return NFSMACTION_CONTINUE;

			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_MSGBOX);
			return NFSMACTION_WAIT;
		}				
			
		case DLGRET_PROMPT:
		{				
			//MOVE_TO_STATE(BTAPPST_PROMPT)
			//return NFSMACTION_CONTINUE;	

			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_PROMPT);	
            return NFSMACTION_WAIT;
		}	

		case DLGRET_BT_FILE_PROGRESS:
		{	
			//MOVE_TO_STATE(BTAPPST_BT_FILE_PROGRESS)
			//return NFSMACTION_CONTINUE;	

			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_FILE_PROGRESS);
			return NFSMACTION_WAIT;
		}

		case DLGRET_BT_EDIT:
		{	
			MOVE_TO_STATE(BTAPPST_BT_EDIT)
			return NFSMACTION_CONTINUE;
		}
			
		case DLGRET_CANCELED:
		{	
			MOVE_TO_STATE(BTAPPST_FTP)
			return NFSMACTION_CONTINUE;
		}

		default:
		{	
			ASSERT_BT_NOT_REACHABLE;
		}
	}

	return NFSMACTION_WAIT;
}


// 状态 BTAPPST_FTP_SERVER_REGISTER 处理函数
static NextFSMAction BTApp_StateFtpServerRegisterHandler(CBTApp *pMe)
{
	if (NULL == pMe)
	{
		return NFSMACTION_WAIT;
	}

	switch(pMe->m_eDlgRet)
	{
		case DLGRET_MSGBOX_CANCELED:
		case DLGRET_PROMPT_CANCELED:
		case DLGRET_PROGRESS_CANCELED:		
		case DLGRET_CREATE:
		{	
			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_FTP_SERVER_REGISTER);
			return NFSMACTION_WAIT;
		}	

		case DLGRET_BT_MSGBOX:
		{	
			//MOVE_TO_STATE(BTAPPST_BT_MSGBOX)
			//return NFSMACTION_CONTINUE;

			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_MSGBOX);
			return NFSMACTION_WAIT;
		}	
		
		case DLGRET_PROMPT:
		{	
			//MOVE_TO_STATE(BTAPPST_PROMPT)
			//return NFSMACTION_CONTINUE;	

			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_PROMPT);	
            return NFSMACTION_WAIT;
		}	

		case DLGRET_BT_FILE_PROGRESS:
		{	
			//MOVE_TO_STATE(BTAPPST_BT_FILE_PROGRESS)
			//return NFSMACTION_CONTINUE;	

			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_FILE_PROGRESS);
			return NFSMACTION_WAIT;
		}

		case DLGRET_BT_EDIT:
		{	
			MOVE_TO_STATE(BTAPPST_BT_EDIT)
			return NFSMACTION_CONTINUE;
		}
			
		case DLGRET_CANCELED:
		{	
			MOVE_TO_STATE(BTAPPST_FTP_SERVER)
			return NFSMACTION_CONTINUE;
		}

		default:
		{	
			ASSERT_BT_NOT_REACHABLE;
		}
	}

	return NFSMACTION_WAIT;
}


// 状态 BTAPPST_FTP_BROWSE处理函数
static NextFSMAction BTApp_StateFtpBrowseHandler(CBTApp *pMe)
{
	if (NULL == pMe)
	{
		return NFSMACTION_WAIT;
	}

	switch(pMe->m_eDlgRet)
	{
		case DLGRET_MSGBOX_CANCELED:
		case DLGRET_PROMPT_CANCELED:
		case DLGRET_PROGRESS_CANCELED:	
		case DLGRET_FTP_BROWSE:		
		case DLGRET_CREATE:
		{	
			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_FTP_BROWSE);
			return NFSMACTION_WAIT;
		}	
		case DLGRET_FTP_BROWSE_OPITION:			
		{					
			MOVE_TO_STATE(BTAPPST_FTP_BROWSE_OPITION)
			return NFSMACTION_CONTINUE;		
		}	

		case DLGRET_FTP_CLIENT:
		{	
			MOVE_TO_STATE(BTAPPST_FTP_CLIENT)
			return NFSMACTION_CONTINUE;
		}	
		
		case DLGRET_BT_MSGBOX:
		{	
			//MOVE_TO_STATE(BTAPPST_BT_MSGBOX)
			//return NFSMACTION_CONTINUE;

			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_MSGBOX);
			return NFSMACTION_WAIT;
		}	
		
		case DLGRET_PROMPT:
		{	
			//MOVE_TO_STATE(BTAPPST_PROMPT)
			//return NFSMACTION_CONTINUE;	

			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_PROMPT);	
            return NFSMACTION_WAIT;
		}

		case DLGRET_BT_FILE_PROGRESS:
		{	
			//MOVE_TO_STATE(BTAPPST_BT_FILE_PROGRESS)
			//return NFSMACTION_CONTINUE;

			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_FILE_PROGRESS);
			return NFSMACTION_WAIT;
		}

		case DLGRET_BT_EDIT:
		{	
			MOVE_TO_STATE(BTAPPST_BT_EDIT)
			return NFSMACTION_CONTINUE;
		}
			
		case DLGRET_CANCELED:		
		{	
			if (!pMe->mFTP.bObjectTransfer)
			{
				MOVE_TO_STATE(BTAPPST_FTP_CLIENT)			
			}
			return NFSMACTION_CONTINUE;
		}	

		default:
		{	
			ASSERT_BT_NOT_REACHABLE;
		}
	}

	return NFSMACTION_WAIT;
}


// 状态 BTAPPST_FTP_BROWSE_OPITION处理函数
static NextFSMAction BTApp_StateFtpBrowseOpitionHandler(CBTApp *pMe)
{
	if (NULL == pMe)
	{
		return NFSMACTION_WAIT;
	}

	switch(pMe->m_eDlgRet)
	{
		case DLGRET_MSGBOX_CANCELED:
		case DLGRET_PROMPT_CANCELED:
		case DLGRET_PROGRESS_CANCELED:	
		case DLGRET_CREATE:
		{	
			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_FTP_BROWSE_OPITION);
			return NFSMACTION_WAIT;
		}	
		
		case DLGRET_FTP_CLIENT:
		{	
			MOVE_TO_STATE(BTAPPST_FTP_CLIENT)
			return NFSMACTION_CONTINUE;
		}	

		case DLGRET_FTP_BROWSE:
		{	
			MOVE_TO_STATE(BTAPPST_FTP_BROWSE)
			return NFSMACTION_CONTINUE;
		}
		
		case DLGRET_BT_MSGBOX:			
		{	
			//MOVE_TO_STATE(BTAPPST_BT_MSGBOX)
			//return NFSMACTION_CONTINUE;

			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_MSGBOX);
			return NFSMACTION_WAIT;
		}
			
		case DLGRET_PROMPT:			
		{	
			//MOVE_TO_STATE(BTAPPST_PROMPT)
			//return NFSMACTION_CONTINUE;	

			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_PROMPT);	
            return NFSMACTION_WAIT;
		}

		case DLGRET_BT_FILE_PROGRESS:
		{	
			//MOVE_TO_STATE(BTAPPST_BT_FILE_PROGRESS)
			//return NFSMACTION_CONTINUE;	

			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_FILE_PROGRESS);
			return NFSMACTION_WAIT;
		}

		case DLGRET_BT_EDIT:			
		{	
			MOVE_TO_STATE(BTAPPST_BT_EDIT)
			return NFSMACTION_CONTINUE;
		}
			
		case DLGRET_CANCELED:
		{	
			MOVE_TO_STATE(BTAPPST_FTP_BROWSE)
			return NFSMACTION_CONTINUE;
		}

		default:
		{	
			ASSERT_BT_NOT_REACHABLE;
		}
	}

	return NFSMACTION_WAIT;
}

// 状态 BTAPPST_BT_EDIT 处理函数
static NextFSMAction BTApp_StateEditHandler(CBTApp *pMe)
{
	if (NULL == pMe)
	{
		return NFSMACTION_WAIT;
	}

	switch(pMe->m_eDlgRet)
	{
		case DLGRET_MSGBOX_CANCELED:
		case DLGRET_PROMPT_CANCELED:
		case DLGRET_PROGRESS_CANCELED:	
		case DLGRET_CREATE:
		{	
			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_EDIT);
			return NFSMACTION_WAIT;		
		}	
			
		case DLGRET_CANCELED:				
		{	
			/*
			if ((pMe->bStartFromOtherApp == TRUE) || (pMe->bStartFromPushReq == TRUE))
			{
				MOVE_TO_STATE(BTAPPST_MAIN)
			}
			else
			*/	
			{
				MOVE_TO_STATE(pMe->m_edit_state_id)					
			}
			return NFSMACTION_CONTINUE;
		}

		case DLGRET_PROMPT:			
		{	
			//MOVE_TO_STATE(BTAPPST_PROMPT)
			//return NFSMACTION_CONTINUE;	

			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_PROMPT);	
            return NFSMACTION_WAIT;	
		}
			
		case DLGRET_BT_MSGBOX:			
		{	
			//MOVE_TO_STATE(BTAPPST_BT_MSGBOX)
			//return NFSMACTION_CONTINUE;

			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_MSGBOX);
			return NFSMACTION_WAIT;	
		}

		case DLGRET_BT_FILE_PROGRESS:
		{	
			//MOVE_TO_STATE(BTAPPST_BT_FILE_PROGRESS)
			//return NFSMACTION_CONTINUE;	
			
			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_FILE_PROGRESS);
			return NFSMACTION_WAIT;
		}

		case DLGRET_BT_EDIT:
		{	
			MOVE_TO_STATE(BTAPPST_BT_EDIT)
			return NFSMACTION_CONTINUE;		
		}

		default:
		{	
			ASSERT_BT_NOT_REACHABLE;
		}
	}

	return NFSMACTION_WAIT;
}



// 状态 BTAPPST_BT_SEND_FILE 处理函数
static NextFSMAction BTApp_StateSendFileHandler(CBTApp *pMe)
{
	if (NULL == pMe)
	{
		return NFSMACTION_WAIT;
	}

	switch(pMe->m_eDlgRet)
	{
		case DLGRET_MSGBOX_CANCELED:
		case DLGRET_PROMPT_CANCELED:
		case DLGRET_PROGRESS_CANCELED:	
		case DLGRET_CREATE:
		{	
			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_SEND_FILE);
			return NFSMACTION_WAIT;		
		}	

		case DLGRET_BT_OBEX_LIST_SERVERS:			
		{								
			MOVE_TO_STATE(BTAPPST_BT_OBEX_LIST_SERVERS)
			return NFSMACTION_CONTINUE;
		}	
			
		case DLGRET_BT_MSGBOX:			
		{	
			//MOVE_TO_STATE(BTAPPST_BT_MSGBOX)
			//return NFSMACTION_CONTINUE;

			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_MSGBOX);
			return NFSMACTION_WAIT;		
		}

		case DLGRET_PROMPT:			
		{	
			//MOVE_TO_STATE(BTAPPST_PROMPT)
			//return NFSMACTION_CONTINUE;	

			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_PROMPT);	
            return NFSMACTION_WAIT;
		}

		case DLGRET_BT_FILE_PROGRESS:
		{	
			//MOVE_TO_STATE(BTAPPST_BT_FILE_PROGRESS)
			//return NFSMACTION_CONTINUE;

			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_FILE_PROGRESS);
			return NFSMACTION_WAIT;
		}

		case DLGRET_BT_EDIT:
		{	
			MOVE_TO_STATE(BTAPPST_BT_EDIT)
			return NFSMACTION_CONTINUE;		
		}
			
		case DLGRET_CANCELED:		
		{	
			(void) ISHELL_CloseApplet(pMe->m_pShell, FALSE);
			return NFSMACTION_WAIT;
		}	

		default:
		{	
			ASSERT_BT_NOT_REACHABLE;
		}
	}

	return NFSMACTION_WAIT;
}

// 状态 BTAPPST_BT_OBEX_LIST_SERVERS 处理函数
static NextFSMAction BTApp_StateObexListServersHandler(CBTApp *pMe)
{
	if (NULL == pMe)
	{
		return NFSMACTION_WAIT;
	}

	switch(pMe->m_eDlgRet)
	{
		//case DLGRET_MSGBOX_CANCELED:
		case DLGRET_PROMPT_CANCELED:
		//case DLGRET_PROGRESS_CANCELED:	
		case DLGRET_CREATE:
		{	
			pMe->m_bNotOverwriteDlgRet = FALSE;		
			BTApp_ShowDialog(pMe, IDD_BT_OBEX_LIST_SERVERS);
			return NFSMACTION_WAIT;		
		}	

		//Add By zzg 2012_11_23
		case DLGRET_MSGBOX_CANCELED:		
		{
			MSG_FATAL("***zzg StateObexListServers bStartFromOtherApp=%d***", pMe->bStartFromOtherApp, 0, 0);
			if (pMe->bStartFromOtherApp == TRUE) 
			{
				MOVE_TO_STATE(BTAPPST_EXIT)
				return NFSMACTION_CONTINUE;			
			}
			else
			{
				pMe->m_bNotOverwriteDlgRet = FALSE;
				BTApp_ShowDialog(pMe, IDD_BT_OBEX_LIST_SERVERS);
				return NFSMACTION_WAIT;
			}
		}	

		case DLGRET_PROGRESS_CANCELED:
		{
			MSG_FATAL("***zzg StateObexListServers bStartFromOtherApp=%d***", pMe->bStartFromOtherApp, 0, 0);
			if (pMe->bStartFromOtherApp == TRUE) 
			{
				MOVE_TO_STATE(BTAPPST_EXIT)
				return NFSMACTION_CONTINUE;			
			}
			else
			{
				pMe->m_bNotOverwriteDlgRet = FALSE;
				BTApp_ShowDialog(pMe, IDD_BT_OBEX_LIST_SERVERS);
				return NFSMACTION_WAIT;
			}
		}

		case DLGRET_DEVICESRH:
		{
			MOVE_TO_STATE(BTAPPST_DEVICESRH)
            return NFSMACTION_CONTINUE;
		}
		//Add End

		//Add By zzg 2011_06_08
		//have connected...
		case DLGRET_FTP_CLIENT:
		{
			MOVE_TO_STATE(BTAPPST_FTP_CLIENT)
			return NFSMACTION_CONTINUE;
		}
		//Add End

		case DLGRET_BT_SEND_FILE:
		{	
			MOVE_TO_STATE(BTAPPST_BT_SEND_FILE)
			return NFSMACTION_CONTINUE;		
		}		
			
		case DLGRET_BT_MSGBOX:			
		{	
			//MOVE_TO_STATE(BTAPPST_BT_MSGBOX)
			//return NFSMACTION_CONTINUE;

			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_MSGBOX);
			return NFSMACTION_WAIT;
		}

		case DLGRET_PROMPT:			
		{	
			//MOVE_TO_STATE(BTAPPST_PROMPT)
			//return NFSMACTION_CONTINUE;	

			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_PROMPT);	
            return NFSMACTION_WAIT;
		}

		case DLGRET_BT_FILE_PROGRESS:
		{	
			//MOVE_TO_STATE(BTAPPST_BT_FILE_PROGRESS)
			//return NFSMACTION_CONTINUE;

			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_FILE_PROGRESS);
			return NFSMACTION_WAIT;
		}

		case DLGRET_BT_EDIT:
		{	
			MOVE_TO_STATE(BTAPPST_BT_EDIT)
			return NFSMACTION_CONTINUE;	
		}
			
		case DLGRET_CANCELED:		
		{	
			MSG_FATAL("***zzg StateObexListServers bStartFromOtherApp=%d***", pMe->bStartFromOtherApp, 0, 0);
			if (pMe->bStartFromOtherApp == TRUE) 
			{
				MOVE_TO_STATE(BTAPPST_EXIT)						
			}
			else if (pMe->m_obex_list_id == IDD_BT_FTP_CLIENT)	
			{
				MOVE_TO_STATE(BTAPPST_FTP_CLIENT)
			}
			else 
			{
				MOVE_TO_STATE(BTAPPST_BT_SEND_FILE)
			}
			return NFSMACTION_CONTINUE;	
		}

		default:
		{	
			ASSERT_BT_NOT_REACHABLE;
		}
	}

	return NFSMACTION_WAIT;
}


// 状态 BTAPPST_BT_FILE_PROGRESS 处理函数
/*
static NextFSMAction BTApp_StateFileProgressHandler(CBTApp *pMe)
{
	if (NULL == pMe)
	{
		return NFSMACTION_WAIT;
	}

	switch(pMe->m_eDlgRet)
	{
		case DLGRET_MSGBOX_CANCELED:
		case DLGRET_PROMPT_CANCELED:
		case DLGRET_CREATE:
		{	
			pMe->m_bNotOverwriteDlgRet = FALSE;			
			BTApp_ShowDialog(pMe, IDD_BT_FILE_PROGRESS);
			return NFSMACTION_WAIT;		
		}	

		case DLGRET_BT_SEND_FILE:
		{	
			MOVE_TO_STATE(BTAPPST_BT_SEND_FILE)
			return NFSMACTION_CONTINUE;		
		}

		case DLGRET_PROMPT:			
		{	
			//MOVE_TO_STATE(BTAPPST_PROMPT)
			//return NFSMACTION_CONTINUE;	

			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_PROMPT);	
            return NFSMACTION_WAIT;	
		}
			
		case DLGRET_BT_EDIT:
		{	
			MOVE_TO_STATE(BTAPPST_BT_EDIT)
			return NFSMACTION_CONTINUE;		
		}
			
		case DLGRET_BT_MSGBOX:			
		{	
			//MOVE_TO_STATE(BTAPPST_BT_MSGBOX)
			//return NFSMACTION_CONTINUE;

			pMe->m_bNotOverwriteDlgRet = FALSE;
			BTApp_ShowDialog(pMe, IDD_BT_MSGBOX);
			return NFSMACTION_WAIT;				
		}
			
		case DLGRET_CANCELED:		
		{	
			if (ISHELL_ActiveApplet(pMe->m_pShell) != AEECLSID_BLUETOOTH_APP)
			{
				(void) ISHELL_CloseApplet(pMe->m_pShell, FALSE);
			}
			else
			{
				MOVE_TO_STATE(BTAPPST_MAIN)
			}
			
			return NFSMACTION_CONTINUE;	
		}
		
		default:
		{	
			ASSERT_BT_NOT_REACHABLE;
		}
	}

	return NFSMACTION_WAIT;
}
*/



static NextFSMAction BTApp_StateExitHandler(CBTApp *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    (void) ISHELL_CloseApplet(pMe->m_pShell, FALSE);

    return NFSMACTION_WAIT;
} // StateExitHandler

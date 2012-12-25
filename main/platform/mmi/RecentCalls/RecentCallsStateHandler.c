/*==============================================================================
// 文件：
//        recentcallsStateHandler.c
//        2007-11-18 陈喜玲草拟版本(Draft Version)
//        版权所有(c) 2007 Anylook
//        
// 文件说明：
//        
// 作者：2007-11-18
// 创建日期：陈喜玲
// 当前版本：Draft
==============================================================================*/

/*==============================================================================
修改历史记录：
时       间      修 改 人     修改内容、位置及原因
-----------      ----------     -----------------------------------------------
07-11-18         陈喜玲         初始版本
==============================================================================*/


/*==============================================================================
                                 本文件包含的外部文件
==============================================================================*/
#include "recentcalls_priv.h"

#ifdef FEATURE_RUIM_PHONEBOOK
//#include "OEMRUIM.h"
#endif

/*==============================================================================
                                 宏定义和常数
==============================================================================*/


/*==============================================================================
                                 类型定义
==============================================================================*/


/*==============================================================================
                                 函数声明
==============================================================================*/
static NextFSMAction stateverifypasswordhandle( CRecentCalls* pMe);

// 状态 STATE_RMAINMENU 处理函数
static NextFSMAction statemainmenuhandle(CRecentCalls *pMe);

// 状态 STATE_RECORD_LIST 处理函数
static NextFSMAction staterecordhandle(CRecentCalls *pMe);

// 状态 STATE_RTIME_MENU 处理函数
static NextFSMAction statetimemenuhandle(CRecentCalls *pMe);

// 状态 STATE_RDEL_MENU 处理函数
static NextFSMAction statedelmenuhandle(CRecentCalls *pMe);

// 状态 STATE_RECORDDEAL 处理函数
static NextFSMAction staterecorddealhandle(CRecentCalls *pMe);

// 状态 STATE_RWARN 处理函数
static NextFSMAction statewarnhandle(CRecentCalls *pMe);

// 状态 STATE_RTIME 处理函数
static NextFSMAction statetimehandle(CRecentCalls *pMe);

// 状态 STATE_DETAIL 处理函数
static NextFSMAction statedetailhandle(CRecentCalls *pMe);

// 状态 STATE_REXIT 处理函数
static NextFSMAction stateexithandle(CRecentCalls *pMe);

// 状态 STATE_SELECT_RETURN 处理函数
static NextFSMAction Handler_STATE_SELECT_RETURN(CRecentCalls *pMe);
//状态 STATE_ASKPASSWORD 处理函数
static NextFSMAction State_AskPasswordHandler(CRecentCalls *pMe);

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
       recentcalls_ProcessState
说明:
       recentcalls Applet状态处理主函数。函数根据pMe->m_currState将调用路由至
       相应的处理函数。

参数:
       pMe [In]: 指向recentcalls Applet对象结构的指针,该结构包含小程序的特定信息.

返回值:
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注:

==============================================================================*/
NextFSMAction recentcalls_ProcessState(CRecentCalls *pMe)
{
    NextFSMAction retVal = NFSMACTION_WAIT;

    if (NULL == pMe)
    {
        return retVal;
    }
    // 根据当前状态调用相应的状态处理函数
    switch (pMe->m_eCurState)
    {
        
        case STATE_VERIFY_PASSWORD:
        {
            retVal = stateverifypasswordhandle( pMe);
        }
        break;
        
        case STATE_RMAINMENU:
            retVal = statemainmenuhandle(pMe);
            break;
            
        case STATE_RECORD_LIST:
            retVal = staterecordhandle(pMe);
            break;
            
        case STATE_RTIME_MENU:
            retVal = statetimemenuhandle(pMe);
            break;
            
        case STATE_RDEL_MENU:
            retVal = statedelmenuhandle(pMe);
            break;
            
        case STATE_RECORDDEAL:
            retVal = staterecorddealhandle(pMe);
            break;
            
        case STATE_RWARN:
            retVal = statewarnhandle(pMe);
            break;
            
        case STATE_RTIME:
            retVal = statetimehandle(pMe);
            break;
        
        case STATE_DETAIL:
            retVal = statedetailhandle(pMe);
            break;

        //wuuquan.tang add for wms 20081127
        case STATE_SELECT_RETURN:
            retVal = Handler_STATE_SELECT_RETURN(pMe);
		case STATE_ASKPASSWORD:
			retVal = State_AskPasswordHandler(pMe);
            break;
        
        case STATE_REXIT:
        default:
            retVal = stateexithandle(pMe);
            break;            

    }
    return retVal;
}

/*==============================================================================
函数：
       stateverifypasswordhandle
说明：
       STATE_VERIFY_PASSWORD 状态处理函数
       
参数：
       pMe [in]：指向recentcalls Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT    ：指示因要显示对话框界面给用户，应挂起状态机。
       
备注：
       
==============================================================================*/
static NextFSMAction stateverifypasswordhandle( CRecentCalls* pMe)
{
    
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch( pMe->m_eDlgRet)
    {
        
        case DLGRET_CREATE:
        {
            boolean needPassword = FALSE;
            extern int OEM_GetConfig(AEEConfigItem i, void * pBuff, int nSize);
            OEM_GetConfig( CFGI_RECENTCALL_LOCK_CHECK,
                         &needPassword,
                         sizeof( boolean)
                     );
            if( !needPassword)
            {
                goto __stateverifypasswordhandle_passed__;
            }
            
            pMe->m_bNotOverwriteDlgRet = FALSE;
            recentcalls_ShowDialog( pMe, IDD_VERIFY_PASSWORD);
            return NFSMACTION_WAIT;
        }
       case  DLGRET_INPUT_INVALID:
       {
            recentcalls_ShowMsgBox(pMe, IDS_MSG_INPUTINVALID);
            return NFSMACTION_WAIT;
       }

        case DLGRET_MSGBOX_OK:
            MOVE_TO_STATE(STATE_VERIFY_PASSWORD);
            return NFSMACTION_CONTINUE;
        
        case DLGRET_VERIFY_PASSWORD_PASSED:
        {
__stateverifypasswordhandle_passed__:           
            switch(pMe->m_charAppStartArgs)
            {
                case STARTARG_OUTGCALL:
               pMe->m_callsCategory = AEECALLHISTORY_CALL_TYPE_TO;//CALLHISTORY_OUTGOING_CATEGORY;
                    MOVE_TO_STATE( STATE_RECORD_LIST);
                    break;

                case STARTARG_MISSEDCALL:
                    pMe->m_callsCategory = AEECALLHISTORY_CALL_TYPE_MISSED;//CALLHISTORY_MISSED_CATEGORY;
                    MOVE_TO_STATE( STATE_RECORD_LIST);
                    break;

                case STARTARG_ALLCALL:
                    //pMe->m_eStartMethod = STARTMETHOD_FINDNUMBER;
                    //pMe->m_nRemainWMSNum = RecentCalls_GetRemainWMSNum(pMe);
                    pMe->m_callsCategory = AEECALLHISTORY_CALL_TYPE_ALL;//CALLHISTORY_ALL_CATEGORY;
                    MOVE_TO_STATE( STATE_RECORD_LIST);
                    break;
                case STARTARG_ONECALL:
                    pMe->m_eStartMethod = STARTMETHOD_SELECTFIELD;
                    pMe->m_nRemainWMSNum = 1;
                    pMe->m_callsCategory = AEECALLHISTORY_CALL_TYPE_ALL;//CALLHISTORY_ALL_CATEGORY;
                    MOVE_TO_STATE( STATE_RECORD_LIST);
                    break;
                default:
                    pMe->m_ePreState = STATE_REXIT;
                    pMe->m_eCurState = STATE_RMAINMENU;                
                    break;
            }
            return NFSMACTION_CONTINUE;
        }

        case DLGRET_CANCELED:
        {
            MOVE_TO_STATE( STATE_REXIT)
            return NFSMACTION_CONTINUE;
        }
    }

    return NFSMACTION_WAIT;    
}

/*==============================================================================
函数：
       statemainmenuhandle
说明：
       STATE_RMAINMENU 状态处理函数
       
参数：
       pMe [in]：指向recentcalls Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。
       
备注：
       
==============================================================================*/
static NextFSMAction statemainmenuhandle(CRecentCalls *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            recentcalls_ShowDialog(pMe, IDD_RMAINMENU);
            return NFSMACTION_WAIT;

        case DLGRET_MISSEDCALL:
        case DLGRET_INCOMCALL:
        case DLGRET_OUTGCALL:
            MOVE_TO_STATE(STATE_RECORD_LIST)
            return NFSMACTION_CONTINUE;

        case DLGRET_TIMEMENU:
            MOVE_TO_STATE(STATE_RTIME_MENU)
            return NFSMACTION_CONTINUE;

        case DLGRET_DELMENU:
            MOVE_TO_STATE(STATE_RDEL_MENU)
            return NFSMACTION_CONTINUE;


        case DLGRET_CANCELED:
            MOVE_TO_STATE(STATE_REXIT)
            return NFSMACTION_CONTINUE;

        default:
            break;
    }

    return NFSMACTION_WAIT;
} // statemainmenuhandle


/*==============================================================================
函数：
       staterecordhandle
说明：
       STATE_RECORD_LIST 状态处理函数
       
参数：
       pMe [in]：指向recentcalls Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。
       
备注：
       
==============================================================================*/
static NextFSMAction staterecordhandle(CRecentCalls *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    switch (pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            recentcalls_ShowDialog(pMe, IDD_RLISTRECORD);
            return NFSMACTION_WAIT;

        case DLGRET_RECORDDEAL:
            MOVE_TO_STATE(STATE_RECORDDEAL)
            return NFSMACTION_CONTINUE;
            //add by yangdecai 2010-11-23
        case DLGRET_DELONE:
            MOVE_TO_STATE(STATE_RWARN)
            return NFSMACTION_CONTINUE;     
        case DLGRET_WARN:
            MOVE_TO_STATE(STATE_RWARN)
            return NFSMACTION_CONTINUE;   
         case  DLGRET_DETAIL:
            MOVE_TO_STATE(STATE_DETAIL)
            return NFSMACTION_CONTINUE;
        case DLGRET_CANCELED:
            if(pMe->m_eStartMethod == STARTMETHOD_NORMAL 
                || pMe->m_eStartMethod == STARTMETHOD_FINDNUMBER
                || pMe->m_eStartMethod == STARTMETHOD_SELECTFIELD)
            {
                MOVE_TO_STATE(STATE_REXIT)
            }
            else    /*STARTMETHOD_MAINMENU*/
            {
                MOVE_TO_STATE(STATE_RMAINMENU)
            }

            return NFSMACTION_CONTINUE;

        case DLGRET_REFRESH:
            MOVE_TO_STATE(STATE_RECORD_LIST)
            return NFSMACTION_CONTINUE;
        case DLGRET_OK:
            if(STARTMETHOD_FINDNUMBER == pMe->m_eStartMethod ||
                STARTMETHOD_SELECTFIELD == pMe->m_eStartMethod)
            {
                MOVE_TO_STATE(STATE_SELECT_RETURN)
            }
            return NFSMACTION_CONTINUE;
        case DLGRET_MAX_WMS_CONTACTS:
            recentcalls_ShowMsgBox(pMe, IDS_MAX_SELECT);
            return NFSMACTION_WAIT;
            
        case DLGRET_MSGBOX_OK:
            MOVE_TO_STATE(STATE_RECORD_LIST)
            return NFSMACTION_CONTINUE;
            
        default:
            break;
    }
    return NFSMACTION_WAIT;
} // staterecordhandle


/*==============================================================================
函数：
       statetimemenuhandle
说明：
       STATE_RTIME_MENU 状态处理函数
       
参数：
       pMe [in]：指向recentcalls Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。
       
备注：
       
==============================================================================*/
static NextFSMAction statetimemenuhandle(CRecentCalls *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    
    switch (pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            recentcalls_ShowDialog(pMe, IDD_RTIME_MENU);
            return NFSMACTION_WAIT;

        case DLGRET_TIME:
            MOVE_TO_STATE(STATE_RTIME)
            return NFSMACTION_CONTINUE;
			
//modi by pyuangui 20121225
#ifdef FEATURE_VERSION_C316
        case DLGRET_TIMECLEAR:
#if defined (FEATURE_VERSION_C316)      
                    MOVE_TO_STATE(STATE_ASKPASSWORD)
#else 
                    MOVE_TO_STATE(STATE_RWARN)
#endif			
            return NFSMACTION_CONTINUE;
#endif
//modi end
        case DLGRET_CANCELED:
            MOVE_TO_STATE(STATE_RMAINMENU)
            return NFSMACTION_CONTINUE;

        default:
            break;
    }
    
    return NFSMACTION_WAIT;
} // statetimemenuhandle


/*==============================================================================
函数：
       statedelmenuhandle
说明：
       STATE_RDEL_MENU 状态处理函数
       
参数：
       pMe [in]：指向recentcalls Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。
       
备注：
       
==============================================================================*/
static NextFSMAction statedelmenuhandle(CRecentCalls *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    
    switch (pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            recentcalls_ShowDialog(pMe, IDD_RDEL_MENU);
            return NFSMACTION_WAIT;

        case DLGRET_WARN:
            MOVE_TO_STATE(STATE_RWARN)
            return NFSMACTION_CONTINUE;

        case DLGRET_CANCELED:
            MOVE_TO_STATE(STATE_RMAINMENU)
            return NFSMACTION_CONTINUE;

        default:
            break;
    }
    
    return NFSMACTION_WAIT;
} // statedelmenuhandle


/*==============================================================================
函数：
       staterecorddealhandle
说明：
       STATE_RECORDDEAL 状态处理函数
       
参数：
       pMe [in]：指向recentcalls Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。
       
备注：
       
==============================================================================*/
static NextFSMAction staterecorddealhandle(CRecentCalls *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    
    switch (pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            recentcalls_ShowDialog(pMe, IDD_RDEAL_RECORD);
            return NFSMACTION_WAIT;
        
        case DLGRET_DETAIL:
           MOVE_TO_STATE(STATE_DETAIL)
           return NFSMACTION_CONTINUE;         
                    
        case DLGRET_DELONE:
            MOVE_TO_STATE(STATE_RWARN)
            return NFSMACTION_CONTINUE;         

        case DLGRET_CANCELED:
            MOVE_TO_STATE(STATE_RECORD_LIST)
            return NFSMACTION_CONTINUE;

        case DLGRET_OK:
            MOVE_TO_STATE(STATE_SELECT_RETURN)
            return NFSMACTION_CONTINUE;
        case DLGRET_MAX_WMS_CONTACTS:
            recentcalls_ShowMsgBox(pMe, IDS_MAX_SELECT);
            return NFSMACTION_WAIT;

        case DLGRET_MSGBOX_OK:
            MOVE_TO_STATE(STATE_RECORD_LIST);
            return NFSMACTION_CONTINUE;
            
        default:
            break;
    }
    return NFSMACTION_WAIT;
} // staterecorddealhandle


/*==============================================================================
函数：
       statewarnhandle
说明：
       STATE_RWARN 状态处理函数
       
参数：
       pMe [in]：指向recentcalls Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。
       
备注：
       
==============================================================================*/
static NextFSMAction statewarnhandle(CRecentCalls *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            recentcalls_ShowDialog(pMe, IDD_RWARN);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
            switch(pMe->selectState)
            {
               case IDS_MISSED_CALLS:
               case IDS_RECEIVED_CALLS:
               case IDS_OUTGOING_CALLS:
                  if (pMe->m_ePreState == STATE_RDEL_MENU)//delete all entries
                  {
                     MOVE_TO_STATE(STATE_RDEL_MENU);
                  }
                  else  //STATE_RECORDDEAL, delete one entry
                  {
                     if (pMe->m_eStartMethod == STARTMETHOD_NORMAL)
                     {
#ifdef FEATURE_THAILAND                           
                         MOVE_TO_STATE(STATE_RECORD_LIST);
                         Recntcall_is_view_state(1);
#else              
                         MOVE_TO_STATE(STATE_REXIT);
#endif
                     }
                     else  /*STARTMETHOD_MAINMENU*/
                     {
                         MOVE_TO_STATE(STATE_RMAINMENU);
                     }
                  }
                  break;                  

               case IDS_ALL_CALLS:
                  MOVE_TO_STATE(STATE_RDEL_MENU);
                  break;
                  
               case IDS_DELETE:
                  MOVE_TO_STATE(STATE_RECORD_LIST)
                  break;
               
               case IDS_RESET_TIME:
                  MOVE_TO_STATE(STATE_RTIME_MENU)
                  break;
                  
               default:
                  break;
            }
            return NFSMACTION_CONTINUE;

        default:
            break;
    }
    return NFSMACTION_WAIT;
} // statewarnhandle


/*==============================================================================
函数：
       statetimehandle
说明：
       STATE_RTIME 状态处理函数
       
参数：
       pMe [in]：指向recentcalls Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。
       
备注：
       
==============================================================================*/
static NextFSMAction statetimehandle(CRecentCalls *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    
    switch (pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            recentcalls_ShowDialog(pMe, IDD_RTIME);
            return NFSMACTION_WAIT;
        case DLGRET_TIMECLEAR:
			MSG_FATAL("---------ok2",0,0,0);
#if defined (FEATURE_VERSION_W515)||defined (FEATURE_VERSION_S1000T)||defined (FEATURE_VERSION_C316)  //modi by pyuangui 20121225    
                    MOVE_TO_STATE(STATE_ASKPASSWORD)
#else 
                    MOVE_TO_STATE(STATE_RWARN)
#endif
            
            return NFSMACTION_CONTINUE;
        case DLGRET_CANCELED:
            MOVE_TO_STATE(STATE_RTIME_MENU)
            return NFSMACTION_CONTINUE;
                    
        default:
            break;
    }
    return NFSMACTION_WAIT;
} // statetimehandle

/*==============================================================================
函数：
       statedetailhandle
说明：
       STATE_DETAIL 状态处理函数
       
参数：
       pMe [in]：指向recentcalls Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。
       
备注：
       
==============================================================================*/
static NextFSMAction statedetailhandle(CRecentCalls *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    
    switch (pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            recentcalls_ShowDialog(pMe, IDD_DETAIL);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
            MOVE_TO_STATE(STATE_RECORD_LIST)
            return NFSMACTION_CONTINUE;
                    
        default:
            break;
    }
    return NFSMACTION_WAIT;
} // statedetailhandle


/*==============================================================================
函数：
       stateexithandle
说明：
       STATE_REXIT 状态处理函数
       
参数：
       pMe [in]：指向recentcalls Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。
       
备注：
       
==============================================================================*/
static NextFSMAction stateexithandle(CRecentCalls *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

   (void) ISHELL_CloseApplet(pMe->m_pShell, FALSE);
    return NFSMACTION_WAIT;
} // stateexithandle

/*==============================================================================
函数：
       Handler_STATE_SELECT_RETURN
说明：
       STATE_SELECT_RETURN 状态处理函数
              
参数：
       pMe [in]：指向ContApp Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。
       
备注：
       
==============================================================================*/
static NextFSMAction Handler_STATE_SELECT_RETURN(CRecentCalls *pMe)
{
    extern sSelectRecordListNode* pSelectRecordListRoot;
    //extern Record pRecord;
    boolean       bRet = TRUE;
    
    if(pMe->m_pSelFldCB)
    {
        bRet = pMe->m_pSelFldCB(pSelectRecordListRoot);
    }

    if(bRet)
    {
        MOVE_TO_STATE(STATE_REXIT);
    }
    return NFSMACTION_CONTINUE;
} // Handler_STATE_SELECT_RETURN
/*==============================================================================
函数：
       State_AskPasswordHandler
说明：
       STATE_ASKPASSWORD 状态处理函数
              
参数：
       pMe [in]：指向ContApp Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。
       
备注：
       
==============================================================================*/

static NextFSMAction State_AskPasswordHandler(CRecentCalls *pMe)
{
		if (NULL == pMe)
		{
			return NFSMACTION_WAIT;
		}
		//SEC_ERR("StateAskPinHandler", 0, 0, 0);
		switch(pMe->m_eDlgRet)
		{
			case DLGRET_CREATE:
				pMe->m_bNotOverwriteDlgRet = FALSE;
				recentcalls_ShowDialog(pMe, IDD_ASK_PASSWORD_DIALOG);
				MSG_FATAL("---------->ok1",0,0,0);
				return NFSMACTION_WAIT;
	          
			case DLGRET_VALIDPINPASS:
				MSG_FATAL("---------->ok2",0,0,0);
				MOVE_TO_STATE(STATE_RWARN);
				return NFSMACTION_CONTINUE;
	
			case DLGRET_VALIDPINFAILED:
				MSG_FATAL("---------->ok3",0,0,0);
				//pMe->m_wMsgID = IDS_MSG_INPUTINVALID;
				recentcalls_ShowMsgBox(pMe, IDS_MSG_INPUTINVALID);
			//	MOVE_TO_STATE(STATE_RTIME_MENU);
				return NFSMACTION_WAIT; 
				
			case DLGRET_MSGBOX_OK:
				MSG_FATAL("---------->ok4",0,0,0);
				MOVE_TO_STATE(STATE_RTIME_MENU);		  
				return NFSMACTION_CONTINUE;
	
			case DLGRET_CANCELED:
			MOVE_TO_STATE(STATE_RTIME_MENU)
			return NFSMACTION_CONTINUE;
	
			default:
			break;
		}
	
		return NFSMACTION_WAIT;

	
}


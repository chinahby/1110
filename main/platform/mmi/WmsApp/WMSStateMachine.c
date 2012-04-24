/*==============================================================================
//           W M S   A P P L E T  M O D U L E
// 文件:
//        WMSStateMachine.c
//        版权所有(c) 2009 Anylook Tech. CO.,LTD.
//        
// 文件说明：
//        短信息模块状态机处理函数相关文件
//                 
==============================================================================*/

/*==============================================================================
修改历史记录：
时       间       修 改 人     修改内容、位置及原因
-----------      ----------    -----------------------------------------------

==============================================================================*/


/*==============================================================================
                             
                             本文件包含的外部文件
                             
==============================================================================*/
#include "WMSApp_priv.h" 
#include "WMSUtil.h"

/*==============================================================================
                                 
                                 宏定义和常数
                                 
==============================================================================*/



/*==============================================================================

                                 函数声明

==============================================================================*/
// WMSST_VOICEMAIL 状态处理函数
static NextFSMAction WMSST_VOICEMAIL_Handler(WmsApp *pMe);

// WMSST_VIEWVOICEMAIL 状态处理函数
static NextFSMAction WMSST_VIEWVOICEMAIL_Handler(WmsApp *pMe);

// WMSST_VMNUMALERT 状态处理函数
static NextFSMAction WMSST_VMNUMALERT_Handler(WmsApp *pMe);

// WMSST_VMNUM 状态处理函数
static NextFSMAction WMSST_VMNUM_Handler(WmsApp *pMe);

// WMSST_VOICEMAILOPTS 状态处理函数
static NextFSMAction WMSST_VOICEMAILOPTS_Handler(WmsApp *pMe);

#ifdef FEATURE_CONTACT_APP
// WMSST_EXTARCTDETAILS 状态处理函数
static NextFSMAction WMSST_EXTARCTDETAILS_Handler(WmsApp *pMe);

// WMSST_EXTARCTEDITEMLIST 状态处理函数
static NextFSMAction WMSST_EXTARCTEDITEMLIST_Handler(WmsApp *pMe);

// WMSST_EXTARCTEDITEMOPT 状态处理函数
static NextFSMAction WMSST_EXTARCTEDITEMOPT_Handler(WmsApp *pMe);
#endif

// WMSST_NONE 状态处理函数
static NextFSMAction WMSST_NONE_Handler(WmsApp *pMe);

// WMSST_INIT 状态处理函数
static NextFSMAction WMSST_INIT_Handler(WmsApp *pMe);

// WMSST_MAIN 状态处理函数
static NextFSMAction WMSST_MAIN_Handler(WmsApp *pMe);

// WMSST_CHKPWD 状态处理函数
static NextFSMAction WMSST_CHKPWD_Handler(WmsApp *pMe);

// WMSST_INBOXES 状态处理函数
static NextFSMAction WMSST_INBOXES_Handler(WmsApp *pMe);

// WMSST_VIEWINBOXMSG 状态处理函数
static NextFSMAction WMSST_VIEWINBOXMSG_Handler(WmsApp *pMe);

// WMSST_TONUMLIST 状态处理函数
static NextFSMAction WMSST_TONUMLIST_Handler(WmsApp *pMe);

// WMSST_WRITEMSG 状态处理函数
static NextFSMAction WMSST_WRITEMSG_Handler(WmsApp *pMe);

// WMSST_SENDOPTS 状态处理函数
static NextFSMAction WMSST_SENDOPTS_Handler(WmsApp *pMe);

// WMSST_SENDING 状态处理函数
static NextFSMAction WMSST_SENDING_Handler(WmsApp *pMe);

// WMSST_OUTBOX 状态处理函数
static NextFSMAction WMSST_OUTBOX_Handler(WmsApp *pMe);

// WMSST_VIEWOUTBOXMSG 状态处理函数
static NextFSMAction WMSST_VIEWOUTBOXMSG_Handler(WmsApp *pMe);

// WMSST_DRAFT 状态处理函数
static NextFSMAction WMSST_DRAFT_Handler(WmsApp *pMe);

// WMSST_VIEWDRAFTMSG 状态处理函数
static NextFSMAction WMSST_VIEWDRAFTMSG_Handler(WmsApp *pMe);

// WMSST_DRAFTMSGOPTS 状态处理函数
static NextFSMAction WMSST_DRAFTMSGOPTS_Handler(WmsApp *pMe);

// WMSST_TEMPLATES 状态处理函数
static NextFSMAction WMSST_TEMPLATES_Handler(WmsApp *pMe);

// WMSST_TEMPLATESOPTS 状态处理函数
static NextFSMAction WMSST_TEMPLATESOPTS_Handler(WmsApp *pMe);

// WMSST_VIEWTEMPLATE 状态处理函数
static NextFSMAction WMSST_VIEWTEMPLATE_Handler(WmsApp *pMe);

// WMSST_EDITTEMPLATE 状态处理函数
static NextFSMAction WMSST_EDITTEMPLATE_Handler(WmsApp *pMe);

// WMSST_DELETEMSGS 状态处理函数
static NextFSMAction WMSST_DELETEMSGS_Handler(WmsApp *pMe);

// WMSST_DELMSGCONFIRM 状态处理函数
static NextFSMAction WMSST_DELMSGCONFIRM_Handler(WmsApp *pMe);

// WMSST_MSGSETTING 状态处理函数
static NextFSMAction WMSST_MSGSETTING_Handler(WmsApp *pMe);

// WMSST_RESERVEDMSG 状态处理函数
static NextFSMAction WMSST_RESERVEDMSG_Handler(WmsApp *pMe);

// WMSST_RSVDMSGTIME 状态处理函数
static NextFSMAction WMSST_RSVDMSGTIME_Handler(WmsApp *pMe);

// WMSST_RESERVEDMSGS 状态处理函数
static NextFSMAction WMSST_RESERVEDMSGS_Handler(WmsApp *pMe);

// WMSST_VIEWRESERVEDMSG 状态处理函数
static NextFSMAction WMSST_VIEWRESERVEDMSG_Handler(WmsApp *pMe);

// WMSST_RESERVEDMSGOPT 状态处理函数
static NextFSMAction WMSST_RESERVEDMSGOPT_Handler(WmsApp *pMe);

// WMSST_RESERVEDMSGALERT 状态处理函数
static NextFSMAction WMSST_RESERVEDMSGALERT_Handler(WmsApp *pMe);

// WMSST_AUTOSAVE  状态处理函数
static NextFSMAction WMSST_AUTOSAVE_Handler(WmsApp *pMe);

// WMSST_PRIORITY  状态处理函数
static NextFSMAction WMSST_PRIORITY_Handler(WmsApp *pMe);

// WMSST_REPORTS  状态处理函数
static NextFSMAction WMSST_REPORTS_Handler(WmsApp *pMe);

#ifdef FEATURE_AUTOREPLACE
// WMSST_AUTOREPLACE  状态处理函数
static NextFSMAction WMSST_AUTOREPLACE_Handler(WmsApp *pMe);
#endif

//WMSST_SENDMODE  状态处理函数
static NextFSMAction WMSST_SENDMODE_Handler(WmsApp *pMe);

// WMSST_CALLBACKNUMSWITCH  状态处理函数
static NextFSMAction WMSST_CALLBACKNUMSWITCH_Handler(WmsApp *pMe);

// WMSST_CALLBACKNUMSET  状态处理函数
static NextFSMAction WMSST_CALLBACKNUMSET_Handler(WmsApp *pMe);

#ifdef FEATURE_CDSMS_RUIM
// WMSST_STORAGE  状态处理函数
static NextFSMAction WMSST_STORAGE_Handler(WmsApp *pMe);

// WMSST_MSGCOPYCONFIRM  状态处理函数
static NextFSMAction WMSST_MSGCOPYCONFIRM_Handler(WmsApp *pMe);

// WMSST_MSGCOPY  状态处理函数
static NextFSMAction WMSST_MSGCOPY_Handler(WmsApp *pMe);
#endif

// WMSST_MSGVALIDITY  状态处理函数
static NextFSMAction WMSST_MSGVALIDITY_Handler(WmsApp *pMe);

//Add by zzg 2012_02_04 
// WMSST_TIME_STAMP 状态处理函数
static NextFSMAction WMSST_TIME_STAMP_Handler(WmsApp *pMe);


// WMSST_AUTODELETE  状态处理函数
static NextFSMAction WMSST_AUTODELETE_Handler(WmsApp *pMe);

// WMSST_RESERVEDMSGALERTTIMEOUT  状态处理函数
static NextFSMAction WMSST_RESERVEDMSGALERTTIMEOUT_Handler(WmsApp *pMe);

// WMSST_EXIT 状态处理函数
static NextFSMAction WMSST_EXIT_Handler(WmsApp *pMe);

// WMSST_INBOXMSGOPTS 状态处理函数
static NextFSMAction WMSST_INBOXMSGOPTS_Handler(WmsApp *pMe);

// WMSST_OUTMSGOPTS 状态处理函数
static NextFSMAction WMSST_OUTMSGOPTS_Handler(WmsApp *pMe);

// WMSST_MEMSTATUS 状态处理函数
static NextFSMAction WMSST_MEMSTATUS_Handler(WmsApp *pMe);

static NextFSMAction WMSST_MANAGEMENT_Handler(WmsApp *pMe);

static NextFSMAction WMSST_COPYINBOX_Handler(WmsApp *pMe);

static NextFSMAction WMSST_MOVEINBOX_Handler(WmsApp *pMe);

static NextFSMAction WMSST_SELECTFROM_Handler(WmsApp *pMe);

// WMSST_CONTINUESEND_QUERY 状态处理函数
static NextFSMAction WMSST_CONTINUESEND_QUERY_Handler(WmsApp *pMe);

//WMSST_RESENDCONFIRM状态处理函数  add by yangdecai
static NextFSMAction WMSST_RESENDCONFIRM_Handler(WmsApp *pMe);

//WMSST_WMSNEW状态处理函数add by yangdecai
static NextFSMAction WMSST_WMSNEW_Hander(WmsApp *pMe);
static NextFSMAction WMSST_WMSPOP_Hander(WmsApp *pMe);
static NextFSMAction WMSST_FLASHSMS_Hander(WmsApp *pMe);

#ifdef FEATURE_USES_MMS

static NextFSMAction WMSST_MMSOPTION_YESNO_Handler(WmsApp *pMe);

// WMSST_OUTBOX_MMS 状态处理函数
static NextFSMAction WMSST_OUTBOX_MMS_Handler(WmsApp *pMe);
static NextFSMAction WMSST_VIEWOUTBOXMSG_MMS_Handler(WmsApp *pMe);
static NextFSMAction WMSST_INBOX_MMS_Handler(WmsApp *pMe);
static NextFSMAction WMSST_VIEWINBOXMSG_MMS_Handler(WmsApp *pMe);
extern uint8  g_mmsDataInfoMax;
extern boolean gbWmsMMSNtf;
static NextFSMAction WMSST_GETTING_Handler(WmsApp *pMe);
static NextFSMAction WMSST_OUTMSGOPTS_MMS_Handler(WmsApp *pMe);
static NextFSMAction WMSST_INMSGOPTS_MMS_Handler(WmsApp *pMe);
static NextFSMAction WMSST_DRAFTBOX_MMS_Handler(WmsApp *pMe);
static NextFSMAction WMSST_VIEWDRAFTBOXMSG_MMS_Handler(WmsApp *pMe);
static NextFSMAction WMSST_DRAFTMSGOPTS_MMS_Handler(WmsApp *pMe);
static NextFSMAction WMSST_EDIT_ALBUMOREMAIN_Handler(WmsApp *pMe);
static NextFSMAction WMSST_MMS_SERVER_ADDRESS_Handler(WmsApp *pMe);
static NextFSMAction WMSST_MMS_PROXY_Handler(WmsApp *pMe);
static NextFSMAction WMSST_MMS_PORT_Handler(WmsApp *pMe);
static NextFSMAction WMSST_MMS_MEMSTATUS_Handler(WmsApp *pMe);
#endif

/*==============================================================================

                                 函数定义
                                 
==============================================================================*/
/*==============================================================================
函数:
    WmsApp_ProcessState

说明:
    根据 pMe->m_currState, 调用相应的状态处理函数。

参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
NextFSMAction WmsApp_ProcessState(WmsApp *pMe)
{
    NextFSMAction retVal = NFSMACTION_WAIT;
    MSG_FATAL("WmsApp_ProcessState Start pMe->m_currState=%d", pMe->m_currState, 0, 0);
    if (NULL == pMe)
    {
        return retVal;
    }

    if(pMe->m_currState == WMSST_OUTMSGOPTS || 
        pMe->m_currState == WMSST_INBOXMSGOPTS ||
        pMe->m_currState == WMSST_RESERVEDMSGOPT ||
        pMe->m_currState == WMSST_DRAFTMSGOPTS ||
        pMe->m_currState == WMSST_DELMSGCONFIRM ||
#ifdef FEATURE_USES_MMS  
        pMe->m_currState == WMSST_OUTMSGMMSOPTS ||
        pMe->m_currState == WMSST_INMSGMMSOPTS ||
        pMe->m_currState == WMSST_DRAFTMSGMMSOPTS ||
#endif
        pMe->m_currState == WMSST_MSGCOPYCONFIRM)
    {
        pMe->m_isPopMenu = TRUE;
    }
    else
    {
        pMe->m_isPopMenu = FALSE;
    }
    MSG_FATAL("WmsApp_ProcessState 1", 0, 0, 0);
    // 根据WMS applet状态，调用相应的状态处理函数
    switch (pMe->m_currState)
    {
        case WMSST_NONE:
            return WMSST_NONE_Handler(pMe);
            
        case WMSST_INIT:
            return WMSST_INIT_Handler(pMe);
            
        case WMSST_MAIN:
            return WMSST_MAIN_Handler(pMe);
            
        case WMSST_CHKPWD:
            return WMSST_CHKPWD_Handler(pMe);

        case WMSST_INBOXES:
            return WMSST_INBOXES_Handler(pMe);
            
        case WMSST_VOICEMAIL:
            return WMSST_VOICEMAIL_Handler(pMe);
            
        case WMSST_VIEWVOICEMAIL:
            return WMSST_VIEWVOICEMAIL_Handler(pMe);
            
        case WMSST_VMNUMALERT:
            return WMSST_VMNUMALERT_Handler(pMe);
            
        case WMSST_VMNUM:
            return WMSST_VMNUM_Handler(pMe);
            
        case WMSST_VOICEMAILOPTS:
            return WMSST_VOICEMAILOPTS_Handler(pMe);
            
        case WMSST_VIEWINBOXMSG:
            return WMSST_VIEWINBOXMSG_Handler(pMe);
    
#ifdef FEATURE_CONTACT_APP
        case WMSST_EXTARCTDETAILS:
            return WMSST_EXTARCTDETAILS_Handler(pMe);
            
        case WMSST_EXTARCTEDITEMLIST:
            return WMSST_EXTARCTEDITEMLIST_Handler(pMe);
            
        case WMSST_EXTARCTEDITEMOPT:
            return WMSST_EXTARCTEDITEMOPT_Handler(pMe);
#endif

        case WMSST_TONUMLIST:
            return WMSST_TONUMLIST_Handler(pMe);
            
        case WMSST_WRITEMSG:
            return WMSST_WRITEMSG_Handler(pMe);
            
        case WMSST_SENDOPTS:
            return WMSST_SENDOPTS_Handler(pMe);
            
        case WMSST_SENDING:
            return WMSST_SENDING_Handler(pMe);
                   
        case WMSST_OUTBOX:
            return WMSST_OUTBOX_Handler(pMe);
            
        case WMSST_VIEWOUTBOXMSG:
            return WMSST_VIEWOUTBOXMSG_Handler(pMe);
            
        case WMSST_DRAFT:
            return WMSST_DRAFT_Handler(pMe);
            
        case WMSST_VIEWDRAFTMSG:
            return WMSST_VIEWDRAFTMSG_Handler(pMe);

        case WMSST_DRAFTMSGOPTS:
            return WMSST_DRAFTMSGOPTS_Handler(pMe);
     
        case WMSST_TEMPLATES:
            return WMSST_TEMPLATES_Handler(pMe);
            
        case WMSST_TEMPLATESOPTS:
            return WMSST_TEMPLATESOPTS_Handler(pMe);
            
        case WMSST_VIEWTEMPLATE:
            return WMSST_VIEWTEMPLATE_Handler(pMe);
            
        case WMSST_EDITTEMPLATE:
            return WMSST_EDITTEMPLATE_Handler(pMe);
            
        case WMSST_DELETEMSGS:
            return WMSST_DELETEMSGS_Handler(pMe);
            
        case WMSST_DELMSGCONFIRM:
            return WMSST_DELMSGCONFIRM_Handler(pMe);
            
        case WMSST_MSGSETTING:
            return WMSST_MSGSETTING_Handler(pMe);
            
        case WMSST_RESERVEDMSG:
            return WMSST_RESERVEDMSG_Handler(pMe);

        case WMSST_RSVDMSGTIME:
            return WMSST_RSVDMSGTIME_Handler(pMe);
            
        case WMSST_RESERVEDMSGS:
            return WMSST_RESERVEDMSGS_Handler(pMe);
            
        case WMSST_VIEWRESERVEDMSG:
            return WMSST_VIEWRESERVEDMSG_Handler(pMe);
            
        case WMSST_RESERVEDMSGOPT:
            return WMSST_RESERVEDMSGOPT_Handler(pMe);
            
        case WMSST_RESERVEDMSGALERT:
            return WMSST_RESERVEDMSGALERT_Handler(pMe);
            
        case WMSST_AUTOSAVE:
            return WMSST_AUTOSAVE_Handler(pMe);
            
        case WMSST_PRIORITY:
            return WMSST_PRIORITY_Handler(pMe);
            
        case WMSST_REPORTS:
            return WMSST_REPORTS_Handler(pMe);
            
#ifdef FEATURE_AUTOREPLACE
        case WMSST_AUTOREPLACE:
            return WMSST_AUTOREPLACE_Handler(pMe);
#endif            
            
        case WMSST_SENDMODE:
            return WMSST_SENDMODE_Handler(pMe);
#ifdef FEATURE_USES_MMS  


        case WMSST_SENDERVISIBILITY:
        case WMSST_REPORTALLOWED:
        case WMSST_READREPLY:
        case WMSST_MMSDELIVERYREPORT:
        case WMSST_MMSNOTIFY:
        {
            pMe->m_nDlgID = (int)pMe->m_currState;
            return WMSST_MMSOPTION_YESNO_Handler(pMe);
        }
#endif            
        case WMSST_CALLBACKNUMSWITCH:
            return WMSST_CALLBACKNUMSWITCH_Handler(pMe);
            
        case WMSST_CALLBACKNUMSET:
            return WMSST_CALLBACKNUMSET_Handler(pMe);
            
#ifdef FEATURE_CDSMS_RUIM
        case WMSST_STORAGE:
            return WMSST_STORAGE_Handler(pMe);
            
        case WMSST_MSGCOPYCONFIRM:
            return WMSST_MSGCOPYCONFIRM_Handler(pMe);

        case WMSST_MSGCOPY:
            return WMSST_MSGCOPY_Handler(pMe);
#endif  

        case WMSST_MSGVALIDITY:
            return WMSST_MSGVALIDITY_Handler(pMe);

		//Add By zzg 2012_02_04
		case WMSST_TIME_STAMP:
            return WMSST_TIME_STAMP_Handler(pMe);
		//Add End
            
        case WMSST_AUTODELETE:
            return WMSST_AUTODELETE_Handler(pMe);
            
        case WMSST_RESERVEDMSGALERTTIMEOUT:
            return WMSST_RESERVEDMSGALERTTIMEOUT_Handler(pMe);
            
        case WMSST_INBOXMSGOPTS:
            return WMSST_INBOXMSGOPTS_Handler(pMe);
            
        case WMSST_OUTMSGOPTS:
            return WMSST_OUTMSGOPTS_Handler(pMe);
     
        case WMSST_MEMSTATUS:
            return WMSST_MEMSTATUS_Handler(pMe);
            
        case WMSST_MANAGEMENT:
            return WMSST_MANAGEMENT_Handler(pMe);
            
        case WMSST_COPYINBOX:
            return WMSST_COPYINBOX_Handler(pMe);
            
        case WMSST_MOVEINBOX:
            return WMSST_MOVEINBOX_Handler(pMe);
            
        case WMSST_CONTINUESEND_QUERY:
            return WMSST_CONTINUESEND_QUERY_Handler(pMe);

        case WMSST_SELECTFROM:
            return WMSST_SELECTFROM_Handler(pMe);
			
        //add by yangdecai
		case WMSST_RESENDCONFIRM:
			return WMSST_RESENDCONFIRM_Handler(pMe);

	    case WMSST_WMSNEW:
			return WMSST_WMSNEW_Hander(pMe);
		case WMSST_FLASHSMS:
			return WMSST_FLASHSMS_Hander(pMe);
		case WMSST_POPMSG:
			return WMSST_WMSPOP_Hander(pMe);
            
        case WMSST_EXIT:
            return WMSST_EXIT_Handler(pMe);
            
#ifdef FEATURE_USES_MMS
        case WMSST_OUTMSGMMSOPTS:
            return WMSST_OUTMSGOPTS_MMS_Handler(pMe);
            
        case WMSST_INMSGMMSOPTS:
            return WMSST_INMSGOPTS_MMS_Handler(pMe);
            
        case WMSST_OUTBOX_MMS:
            return WMSST_OUTBOX_MMS_Handler(pMe);

        case WMSST_VIEWOUTBOXMSG_MMS:
            return WMSST_VIEWOUTBOXMSG_MMS_Handler(pMe);     
            
        case WMSST_INBOX_MMS:
            return WMSST_INBOX_MMS_Handler(pMe);

        case WMSST_VIEWINBOXMSG_MMS:
            return WMSST_VIEWINBOXMSG_MMS_Handler(pMe);   

        case WMSST_GETTING:
            return WMSST_GETTING_Handler(pMe); 

        case WMSST_DRAFTBOX_MMS:
            return WMSST_DRAFTBOX_MMS_Handler(pMe);   

        case WMSST_VIEWDRAFTBOXMSG_MMS:
            return WMSST_VIEWDRAFTBOXMSG_MMS_Handler(pMe);    

        case WMSST_DRAFTMSGMMSOPTS:
            return WMSST_DRAFTMSGOPTS_MMS_Handler(pMe);     

        case WMSST_EDIT_ALBUMOREMAIN:
            return WMSST_EDIT_ALBUMOREMAIN_Handler(pMe);        

        case WMSST_MMS_SERVER_ADDRESS:
            return WMSST_MMS_SERVER_ADDRESS_Handler(pMe);   

        case WMSST_MMS_PROXY:
            return WMSST_MMS_PROXY_Handler(pMe); 

        case WMSST_MMS_PORT:
            return WMSST_MMS_PORT_Handler(pMe);    

        case WMSST_MMS_MEMSTATUS:
            return WMSST_MMS_MEMSTATUS_Handler(pMe);            
#endif


        default:
            break;
    }

    return retVal;
}

/*==============================================================================
函数:
    WMSST_NONE_Handler

说明:
    WMSST_NONE 状态处理函数。

参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction WMSST_NONE_Handler(WmsApp *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    MOVE_TO_STATE(WMSST_EXIT);
    return NFSMACTION_CONTINUE;
}   // WMSST_NONE_Handler

/*==============================================================================
函数:
    WMSST_INIT_Handler

说明:
    WMSST_INIT 状态处理函数。

参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction WMSST_INIT_Handler(WmsApp *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    
    switch (pMe->m_eDlgReturn)
    {
        case DLGRET_CREATE:
            pMe->m_ePMsgType = MESSAGE_WARNNING;
            WmsApp_ShowMsgBox(pMe, IDS_NOTREADY);
            return NFSMACTION_WAIT;
            
        case DLGRET_MSGBOX_OK:
            MOVE_TO_STATE(WMSST_EXIT)
            return NFSMACTION_CONTINUE;
            
        default:
            // 用退出程序代替宏断言
            MOVE_TO_STATE(WMSST_EXIT)
            return NFSMACTION_CONTINUE;
    }
} // WMSST_INIT

/*==============================================================================
函数:
    WMSST_MAIN_Handler

说明:
    WMSST_MAIN 状态处理函数。

参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction WMSST_MAIN_Handler(WmsApp *pMe)
{
    boolean  bsmslock = FALSE;
    MSG_FATAL("WMSST_MAIN_Handler Start",0,0,0);
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    
    (void) ICONFIG_GetItem(pMe->m_pConfig,
                           CFGI_SMS_LOCK_CHECK,
                           &bsmslock,
                           sizeof(bsmslock));
    bsmslock = FALSE; //进入子菜单不加密，从mainmenu进入时加密就好了

    pMe->m_wPrevMenuSel = 0 ;

    switch (pMe->m_eDlgReturn)
    {
        // 进入主界面
        case DLGRET_CREATE:
            // 在这里作必要的变量重置工作
            WmsApp_MemberReset(pMe);
            
            // 结构数据清零
            WMSMessageStruct_Reset(&pMe->m_msSend);
            
#if defined(FEATURE_WMSAPP_ONLYSUPPORTVMAIL)
            pMe->m_eMBoxType = WMS_MB_VOICEMAIL;
            MOVE_TO_STATE(WMSST_VOICEMAIL)
            return NFSMACTION_CONTINUE;
#else            
            WmsApp_ShowDialog(pMe, IDD_MAIN);
            return NFSMACTION_WAIT;
#endif            
            
        // 消息对话框返回
        case DLGRET_MSGBOX_OK:
            if (pMe->m_wMsgResID != IDS_EMPTY)
            {
                switch (pMe->m_eMBoxType)
                {
                    case WMS_MB_OUTBOX:
                        MOVE_TO_STATE(WMSST_OUTBOX)
                        if (bsmslock)
                        {
                            pMe->m_stchkpwdbk = pMe->m_currState;
                            pMe->m_currState = WMSST_CHKPWD;
                        }
                        return NFSMACTION_CONTINUE;
                    
                    case WMS_MB_INBOX:
                        MOVE_TO_STATE(WMSST_INBOXES)
                        if (bsmslock)
                        {
                            pMe->m_stchkpwdbk = pMe->m_currState;
                            pMe->m_currState = WMSST_CHKPWD;
                        }
                        return NFSMACTION_CONTINUE;
                        
                    case WMS_MB_DRAFT:
                        MOVE_TO_STATE(WMSST_DRAFT)
                        if (bsmslock)
                        {
                            pMe->m_stchkpwdbk = pMe->m_currState;
                            pMe->m_currState = WMSST_CHKPWD;
                        }
                        return NFSMACTION_CONTINUE;
                        
                    default:
                        break;
                }
            }
            WmsApp_ShowDialog(pMe, IDD_MAIN);
            return NFSMACTION_WAIT;

        // 用户在主界面选择-- 收件箱(RUIM+Phone)
        case DLGRET_INBOXES:
            pMe->m_eMBoxType = WMS_MB_INBOX;
            {
                uint16  nMsgID = 0;
                
                nMsgID = WmsApp_GetmemAlertID(pMe, WMS_MB_INBOX);
                if (nMsgID != 0)
                {
                    pMe->m_ePMsgType = MESSAGE_WARNNING;
                    WmsApp_ShowMsgBox(pMe, nMsgID);
                    return NFSMACTION_WAIT;
                }
            }
            MOVE_TO_STATE(WMSST_INBOXES)
            if (bsmslock)
            {
                pMe->m_stchkpwdbk = pMe->m_currState;
                pMe->m_currState = WMSST_CHKPWD;
            }
            return NFSMACTION_CONTINUE;         
        
        // 用户在主界面选择-- 新建短消息
        case DLGRET_WRITEMSG:
            pMe->m_bTextFullAlert = FALSE;         
            // 清空群发地址链表
            WmsApp_FreeMultiSendList(pMe->m_pSendList);
            pMe->m_eCreateWMSType = SEND_MSG_NEW;
            
            pMe->m_STSwitchToEditMsg = pMe->m_currState;
            pMe->m_dwInsertPos = 0;
            MOVE_TO_STATE(WMSST_WRITEMSG) 
            if (bsmslock)
            {
                pMe->m_stchkpwdbk = pMe->m_currState;
                pMe->m_currState = WMSST_CHKPWD;
            }
            return NFSMACTION_CONTINUE;
            

        // 用户在主界面选择-- 发件箱
        case DLGRET_OUTBOX:
            pMe->m_eMBoxType = WMS_MB_OUTBOX;
            {
                uint16  nMsgID=0;
                
                nMsgID = WmsApp_GetmemAlertID(pMe, WMS_MB_OUTBOX);
                if (nMsgID != 0)
                {
                    pMe->m_ePMsgType = MESSAGE_WARNNING;
                    WmsApp_ShowMsgBox(pMe, nMsgID);
                    return NFSMACTION_WAIT;
                }
            }
            MOVE_TO_STATE(WMSST_OUTBOX)
            if (bsmslock)
            {
                pMe->m_stchkpwdbk = pMe->m_currState;
                pMe->m_currState = WMSST_CHKPWD;
            }
            return NFSMACTION_CONTINUE;

#ifdef FEATURE_USES_MMS
        // 用户在主界面选择-- 彩信发件箱
        case DLGRET_OUTBOX_MMS:
            MSG_FATAL("WMSST_MAIN_Handler DLGRET_OUTBOX_MMS",0,0,0);
            pMe->m_eMBoxType = WMS_MB_OUTBOX_MMS;
            {
                uint16  nMsgID=0;    
                nMsgID = WmsApp_GetmemAlertID(pMe, WMS_MB_OUTBOX_MMS);
                if (nMsgID != 0)
                {
                    pMe->m_ePMsgType = MESSAGE_WARNNING;
                    WmsApp_ShowMsgBox(pMe, nMsgID);
                    return NFSMACTION_WAIT;
                }
            }
            MOVE_TO_STATE(WMSST_OUTBOX_MMS)
            return NFSMACTION_CONTINUE;
        // 用户在主界面选择-- 彩信收件箱
        case DLGRET_INBOX_MMS:
            MSG_FATAL("WMSST_MAIN_Handler DLGRET_INBOX_MMS",0,0,0);
            pMe->m_eMBoxType = WMS_MB_INBOX_MMS;
            {
                uint16  nMsgID=0;    
                nMsgID = WmsApp_GetmemAlertID(pMe, WMS_MB_INBOX_MMS);
                if (nMsgID != 0)
                {
                    pMe->m_ePMsgType = MESSAGE_WARNNING;
                    WmsApp_ShowMsgBox(pMe, nMsgID);
                    return NFSMACTION_WAIT;
                }
            }
            MOVE_TO_STATE(WMSST_INBOX_MMS)
            return NFSMACTION_CONTINUE;          

        case DLGRET_DRAFTBOX_MMS:
            MSG_FATAL("WMSST_MAIN_Handler DLGRET_DRAFTBOX_MMS",0,0,0);
            pMe->m_eMBoxType = WMS_MB_DRAFTBOX_MMS;
            {
                uint16  nMsgID=0;    
                nMsgID = WmsApp_GetmemAlertID(pMe, WMS_MB_DRAFTBOX_MMS);
                if (nMsgID != 0)
                {
                    pMe->m_ePMsgType = MESSAGE_WARNNING;
                    WmsApp_ShowMsgBox(pMe, nMsgID);
                    return NFSMACTION_WAIT;
                }
            }
            MOVE_TO_STATE(WMSST_DRAFTBOX_MMS)
            return NFSMACTION_CONTINUE;         

        // 用户在主界面选择-- 存储器状态
        case DLGRET_MMS_MEMSTATUS:
            MOVE_TO_STATE(WMSST_MMS_MEMSTATUS)
            return NFSMACTION_CONTINUE;            
#endif
        // 用户在主界面选择-- 语音信箱
        case DLGRET_VIEWVOICEMSG:
            pMe->m_eMBoxType = WMS_MB_VOICEMAIL;
            MOVE_TO_STATE(WMSST_VOICEMAIL)
            if (bsmslock)
            {
                pMe->m_stchkpwdbk = pMe->m_currState;
                pMe->m_currState = WMSST_CHKPWD;
            }
            return NFSMACTION_CONTINUE;
            
        // 用户在主界面选择-- 草稿箱
        case DLGRET_DRAFT:
            pMe->m_eMBoxType = WMS_MB_DRAFT;
            {
                uint16  nMsgID=0;
                
                nMsgID = WmsApp_GetmemAlertID(pMe, WMS_MB_DRAFT);
                if (nMsgID != 0)
                {
                    pMe->m_ePMsgType = MESSAGE_WARNNING;
                    WmsApp_ShowMsgBox(pMe, nMsgID);
                    return NFSMACTION_WAIT;
                }
            }
            
            MOVE_TO_STATE(WMSST_DRAFT)
            if (bsmslock)
            {
                pMe->m_stchkpwdbk = pMe->m_currState;
                pMe->m_currState = WMSST_CHKPWD;
            }
            return NFSMACTION_CONTINUE;

        // 用户在主界面选择-- 消息模板
        case DLGRET_TEMPLATES:
            pMe->m_eInsertType = INSERT_NONE;
            pMe->m_eMBoxType = WMS_MB_TEMPLATE;
            MOVE_TO_STATE(WMSST_TEMPLATES)
            if (bsmslock)
            {
                pMe->m_stchkpwdbk = pMe->m_currState;
                pMe->m_currState = WMSST_CHKPWD;
            }
            return NFSMACTION_CONTINUE;
            
        // 用户在主界面选择-- 存储器状态
        case DLGRET_MEMSTATUS:
            MOVE_TO_STATE(WMSST_MEMSTATUS)
            return NFSMACTION_CONTINUE;
            
        //用户在主界面选择--信息管理    
        case DLGRET_MANAGEMENT:
            MOVE_TO_STATE(WMSST_MANAGEMENT)
            return NFSMACTION_CONTINUE;


        // 用户在主界面选择-- 删除消息
        case DLGRET_DELETEMSGS:
            {
                uint16  nInMsgs=0;
                uint16  nOutMsgs=0;
                uint16  nDraftMsgs=0;
                
                // 获取消息数
                wms_cacheinfolist_getcounts(WMS_MB_INBOX, NULL, NULL, &nInMsgs);
                wms_cacheinfolist_getcounts(WMS_MB_OUTBOX, NULL, NULL, &nOutMsgs);
                wms_cacheinfolist_getcounts(WMS_MB_DRAFT, NULL, NULL, &nDraftMsgs);

                if ((nInMsgs+nOutMsgs+nDraftMsgs)==0)
                {
                    pMe->m_ePMsgType = MESSAGE_WARNNING;
                    WmsApp_ShowMsgBox(pMe, IDS_EMPTY);
                    return NFSMACTION_WAIT;
                }
            }
                
            MOVE_TO_STATE(WMSST_DELETEMSGS)
            if (bsmslock)
            {
                pMe->m_stchkpwdbk = pMe->m_currState;
                pMe->m_currState = WMSST_CHKPWD;
            }
            return NFSMACTION_CONTINUE;

        // 用户在主界面选择-- 消息设置
        case DLGRET_SETTINGS:
            MOVE_TO_STATE(WMSST_MSGSETTING)
            if (bsmslock)
            {
                pMe->m_stchkpwdbk = pMe->m_currState;
                pMe->m_currState = WMSST_CHKPWD;
            }
            return NFSMACTION_CONTINUE;

        // 用户在主界面选择-- 预约信息
        case DLGRET_RESERVEDMSG:
            MOVE_TO_STATE(WMSST_RESERVEDMSG)
            if (bsmslock)
            {
                pMe->m_stchkpwdbk = pMe->m_currState;
                pMe->m_currState = WMSST_CHKPWD;
            }
            return NFSMACTION_CONTINUE;
            
        case DLGRET_CANCELED:
            MOVE_TO_STATE(WMSST_EXIT)
            return NFSMACTION_CONTINUE;

        default:
            // 用退出程序代替宏断言
            MOVE_TO_STATE(WMSST_EXIT)
            return NFSMACTION_CONTINUE;
    }
} // WMSST_MAIN_Handler

/*==============================================================================
函数:
    WMSST_CHKPWD_Handler

说明:
    WMSST_CHKPWD 状态处理函数。

参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction WMSST_CHKPWD_Handler(WmsApp *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    pMe->m_wPrevMenuSel = 0 ;

    switch (pMe->m_eDlgReturn)
    {
        case DLGRET_MSGBOX_OK:
        case DLGRET_CREATE:
            WmsApp_ShowDialog(pMe, IDD_PWD);
            return NFSMACTION_WAIT;

        // 密码输入错误
        case DLGRET_PWDERR:
            pMe->m_ePMsgType = MESSAGE_WARNNING;
            WmsApp_ShowMsgBox(pMe, IDS_PWDERR);
            return NFSMACTION_WAIT;
            
        // 密码输入正确
        case DLGRET_PWDOK:
            if (pMe->m_bNeedContinueSend)
            {
                pMe->m_currState = WMSST_CONTINUESEND_QUERY;
                pMe->m_stcontinuesendbk = pMe->m_stchkpwdbk;
            }
            else
            {
                pMe->m_currState = pMe->m_stchkpwdbk;
            }
                
            return NFSMACTION_CONTINUE;
            
        case DLGRET_CANCELED:
            if (pMe->m_stchkpwdbk == WMSST_RESERVEDMSGALERT)
            {
                // 设置预约短信定时器
                WmsApp_ReservedMsgSetTimer(pMe);
            }
            
            if (pMe->m_bNaturalStart == FALSE)
            {
                MOVE_TO_STATE(WMSST_EXIT)
            }
            else
            {
                MOVE_TO_STATE(WMSST_EXIT)
            }
            return NFSMACTION_CONTINUE;

        default:
            // 用退出程序代替宏断言
            MOVE_TO_STATE(WMSST_EXIT)
            return NFSMACTION_CONTINUE;
    }
} // WMSST_CHKPWD_Handler

/*==============================================================================
函数:
    WMSST_INBOXES_Handler

说明:
    WMSST_INBOXES 状态处理函数。

参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction WMSST_INBOXES_Handler(WmsApp *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgReturn)
    {
        case DLGRET_CREATE:
        case DLGRET_LOADCANCELED:
        case DLGRET_LOADFAILED:
            pMe->m_eMBoxType = WMS_MB_INBOX;
            WmsApp_ShowDialog(pMe, IDD_MESSAGELIST);
            return NFSMACTION_WAIT;

        case DLGRET_LOAD:
            pMe->m_eOptType = OPT_VIA_VIEWMSG;
            WmsApp_ShowDialog(pMe, IDD_LOADINGMSG);
            return NFSMACTION_WAIT;
            
        case DLGRET_CANCELED:
            MOVE_TO_STATE(WMSST_MAIN)
            return NFSMACTION_CONTINUE;

        case DLGRET_LOADOK:
            if (pMe->m_eOptType == OPT_VIA_VIEWMSG)
            {
                MOVE_TO_STATE(WMSST_VIEWINBOXMSG)
            }
            else
            {
                MOVE_TO_STATE(WMSST_INBOXMSGOPTS)
            }
            return NFSMACTION_CONTINUE;
            
        case DLGRET_OPT:
            pMe->m_eOptType = OPT_VIA_LISTMSG;
            WmsApp_ShowDialog(pMe, IDD_LOADINGMSG);
            return NFSMACTION_CONTINUE;
            
        case DLGRET_DELETE:
        	//释放查看的消息内存
			WMSMessageStruct_Free(pMe);
			//ADD BY YANGDECAI 2010-08-16
            pMe->m_eEraseWMSType = ERASE_DRAFT_ONE;
            MOVE_TO_STATE(WMSST_DELMSGCONFIRM)
            return NFSMACTION_CONTINUE;
            
        default:
            // 用退出程序代替宏断言
            MOVE_TO_STATE(WMSST_EXIT)
            return NFSMACTION_CONTINUE;
    }
} // WMSST_INBOXES_Handler

/*==============================================================================
函数:
    WMSST_VIEWINBOXMSG_Handler

说明:
    WMSST_VIEWINBOXMSG 状态处理函数。

参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction WMSST_VIEWINBOXMSG_Handler(WmsApp *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
	
    switch(pMe->m_eDlgReturn)
    {
        case DLGRET_CREATE:
			
            WmsApp_ShowDialog(pMe, IDD_VIEWMSG);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
            MOVE_TO_STATE(WMSST_INBOXES)
            return NFSMACTION_CONTINUE;

        case DLGRET_OK:
            pMe->m_eOptType = OPT_VIA_VIEWMSG;
            MOVE_TO_STATE(WMSST_INBOXMSGOPTS)
            return NFSMACTION_CONTINUE;

        case DLGRET_INFO:
#ifdef FEATURE_USES_MMS
            if(pMe->m_isMMSNotify == TRUE)
            {
                pMe->m_eOptType = OPT_VIA_VIEWMSG;
                pMe->m_eDlgReturn = DLGRET_GETMMS;
                pMe->m_bDoNotOverwriteDlgResult = TRUE;
            }
            else
#endif
            {
                pMe->m_eOptType = OPT_VIA_VIEWMSG;
                pMe->m_eDlgReturn = DLGRET_REPLY;
                pMe->m_bDoNotOverwriteDlgResult = TRUE;            
            }
            MOVE_TO_STATE(WMSST_INBOXMSGOPTS)
            return NFSMACTION_CONTINUE;
            
        default:
            // 用退出程序代替宏断言
            MOVE_TO_STATE(WMSST_EXIT)
            return NFSMACTION_CONTINUE;
    }
} // WMSST_VIEWINBOXMSG_Handler

/*==============================================================================
函数:
    WMSST_INBOXMSGOPTS_Handler

说明:
    WMSST_INBOXMSGOPTS 状态处理函数。

参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction WMSST_INBOXMSGOPTS_Handler(WmsApp *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgReturn)
    {
        case DLGRET_CREATE:
            WmsApp_ShowDialog(pMe, IDD_MSGOPTS);
            return NFSMACTION_WAIT;

        case DLGRET_VIEW:
            // 这里消息已读出，但并没修改消息标记，故需补未作工作
            {
                wms_cache_info_node  *pnode = NULL;
                uint16 wIndex=pMe->m_wCurindex;
               
                // 取消息 cache info 节点
                if (wIndex>=RUIM_MSGINDEX_BASE)
                {
                    wIndex = wIndex - RUIM_MSGINDEX_BASE;
                    pnode = wms_cacheinfolist_getnode(WMS_MB_INBOX, WMS_MEMORY_STORE_RUIM, wIndex);
                }
                else
                {
                    pnode = wms_cacheinfolist_getnode(WMS_MB_INBOX, WMS_MEMORY_STORE_NV_CDMA, wIndex);
                }
                
                if (NULL != pnode)
                {
#ifdef FEATURE_SMS_UDH
                    if (pnode->pItems != NULL)
                    {
                        int i;
                        
                        for (i=0; i<LONGSMS_MAX_PACKAGES; i++)
                        {
                            pnode = pMe->m_CurMsgNodes[i];
                            
                            if ((NULL != pnode) &&
                                (pnode->msg_tag == WMS_TAG_MT_NOT_READ))
                            {
                                (void)IWMS_MsgModifyTag(pMe->m_pwms,
                                                        pMe->m_clientId,
                                                        &pMe->m_callback,
                                                        (void *)pMe,
                                                        pnode->mem_store,
                                                        pnode->index,
                                                        WMS_TAG_MT_READ);
                            }
                        }
                    }
                    else
#endif                        
                    {
                        (void)IWMS_MsgModifyTag(pMe->m_pwms,
                                                pMe->m_clientId,
                                                &pMe->m_callback,
                                                (void *)pMe,
                                                pnode->mem_store,
                                                pnode->index,
                                                WMS_TAG_MT_READ);
                    }
                }
            }
                
            MOVE_TO_STATE(WMSST_VIEWINBOXMSG)
            return NFSMACTION_CONTINUE;
            
        case DLGRET_CANCELED:
        case DLGRET_MSGBOX_OK:
            if (pMe->m_eOptType == OPT_VIA_VIEWMSG)
            {
                MOVE_TO_STATE(WMSST_VIEWINBOXMSG)
            }
            else
            {
                MOVE_TO_STATE(WMSST_INBOXES)
            }
            return NFSMACTION_CONTINUE;

        case DLGRET_REPLY:
            {// 对于回复操作不再进入地址列表输入界面
                AECHAR  *pwstrNum = NULL;
                CMultiSendItemInfo *pItem = NULL;
                //释放查看的消息内存
				WMSMessageStruct_Free(pMe);
				//ADD BY YANGDECAI 2010-08-16
                // 先清空群发地址链表
                WmsApp_FreeMultiSendList(pMe->m_pSendList);
                
                // 拷贝回复地址 
#if 1//defined(FEATURE_CARRIER_VENEZUELA_MOVILNET)
                // 该运营商要求回复短信时 CBN 优先
                if (WSTRLEN(pMe->m_msCur.m_szCallBkNum)>0)
                {
                    pwstrNum = pMe->m_msCur.m_szCallBkNum;
                }
                else if (WSTRLEN(pMe->m_msCur.m_szNum)>0)
                {
                    pwstrNum = pMe->m_msCur.m_szNum;
                }
#else
                // 运营商要求回复短信时缺省地址取Originating Address
                if (WSTRLEN(pMe->m_msCur.m_szNum)>0)
                {
                    pwstrNum = pMe->m_msCur.m_szNum;
                }
                else if (WSTRLEN(pMe->m_msCur.m_szCallBkNum)>0)
                {
                    pwstrNum = pMe->m_msCur.m_szCallBkNum;
                }
#endif                
                
                if (NULL != pwstrNum)
                { 
                    pItem = (CMultiSendItemInfo *)sys_malloc(sizeof(CMultiSendItemInfo));
                    DBGPRINTF("WMS Address = 0x%x", pItem); 
                }
                    
                // 将回复号码保存入链表
                if ((pItem == NULL) || 
                    (SUCCESS != IVector_AddElement(pMe->m_pSendList, pItem)))
                {// 空号码无法回复
                    if (pMe->m_eOptType == OPT_VIA_VIEWMSG)
                    {
                        MOVE_TO_STATE(WMSST_VIEWINBOXMSG)
                    }
                    else
                    {
                        MOVE_TO_STATE(WMSST_INBOXES)
                    }
                    if (NULL != pItem)
                    {
                        sys_free(pItem);
                    }
                    
                    return NFSMACTION_CONTINUE;
                }
                (void)WSTRCPY(pItem->m_szTo, pwstrNum);
                pMe->m_CurAddID = MSG_CMD_BASE;
                
                // 从电话本中取人名, 用于提示
                WMSUtil_GetContactName(pMe, pItem->m_szTo, pItem->m_szName, MAX_TITLE_LEN);
            }
            
            pMe->m_STSwitchToEditMsg = pMe->m_currState;
            pMe->m_eCreateWMSType = SEND_MSG_REPLY;
            pMe->m_dwInsertPos = 0;
            
            // 结构数据清零
            WMSMessageStruct_Reset(&pMe->m_msSend);
            
            MOVE_TO_STATE(WMSST_WRITEMSG)
            return NFSMACTION_CONTINUE;

        case DLGRET_FORWARD:
            // 清空群发地址链表
            WmsApp_FreeMultiSendList(pMe->m_pSendList);

            // 拷贝消息
            WmsApp_CopyCurMsgForSend(pMe);
            
            pMe->m_STSwitchToEditMsg = pMe->m_currState;
            pMe->m_eCreateWMSType = SEND_MSG_FORWARD;
            pMe->m_dwInsertPos = 0;
            MOVE_TO_STATE(WMSST_WRITEMSG)
            return NFSMACTION_CONTINUE;
            
        case DLGRET_DELETE:
			//释放查看的消息内存
			WMSMessageStruct_Free(pMe);
			//ADD BY YANGDECAI 2010-08-16
            pMe->m_eEraseWMSType = ERASE_INBOXES_ONE;
            MOVE_TO_STATE(WMSST_DELMSGCONFIRM)
            return NFSMACTION_CONTINUE;

        case DLGRET_CLEARALL:
			//释放查看的消息内存
			WMSMessageStruct_Free(pMe);
			//ADD BY YANGDECAI 2010-08-16
            pMe->m_eEraseWMSType = CLEAR_INBOXES;
            pMe->m_STSwitchToEditMsg = WMSST_MAIN;
            MOVE_TO_STATE(WMSST_DELMSGCONFIRM)
            return NFSMACTION_CONTINUE;
#ifdef FEATURE_CONTACT_APP
        case DLGRET_SAVENUM:
            pMe->m_ExtractType = EXTRACT_NUM;
            WmsApp_FreeSaveNumList(pMe);
			
            //释放查看的消息内存
			WMSMessageStruct_Free(pMe);
			//ADD BY YANGDECAI 2010-08-16
			
            // 分析消息建立号码链表
            WmsApp_BuildNumList(pMe, &pMe->m_msCur);
            
            if (IVector_Size(pMe->m_pSaveNumList) < 1)
            {
                pMe->m_ePMsgType = MESSAGE_WARNNING;
                WmsApp_ShowMsgBox(pMe, IDS_NOVALIDNUM);
                return NFSMACTION_WAIT;
            }
            MOVE_TO_STATE(WMSST_EXTARCTEDITEMLIST)
            return NFSMACTION_CONTINUE;
            
        case DLGRET_SAVEEMAIL:
			//释放查看的消息内存
			WMSMessageStruct_Free(pMe);
			//ADD BY YANGDECAI 2010-08-16
            pMe->m_ExtractType = EXTRACT_EMAILADD;
            WmsApp_FreeSaveNumList(pMe);
            
            // 分析消息建立电邮地址链表
            WmsApp_BuildMailAddList(pMe, pMe->m_msCur.m_szMessage);
            
            if (IVector_Size(pMe->m_pSaveNumList) < 1)
            {
                pMe->m_ePMsgType = MESSAGE_WARNNING;
                WmsApp_ShowMsgBox(pMe, IDS_NOVALIDMAILADD);
                return NFSMACTION_WAIT;
            }
            MOVE_TO_STATE(WMSST_EXTARCTEDITEMLIST)
            return NFSMACTION_CONTINUE;
            
#ifdef FEATURE_USES_MMS
        case DLGRET_GETMMS:
        {
            WMSMessageStruct_Free(pMe);
			//ADD BY YANGDECAI 2010-08-16
            pMe->m_ExtractType = EXTRACT_URL;
            WmsApp_FreeSaveNumList(pMe);
            
            // 分析消息建立URL表
            WmsApp_BuildUrlList(pMe, pMe->m_msCur.m_szMessage);
            
            if (IVector_Size(pMe->m_pSaveNumList) < 1)
            {
                pMe->m_ePMsgType = MESSAGE_WARNNING;
                WmsApp_ShowMsgBox(pMe, IDS_NOVALIDURL);
                return NFSMACTION_WAIT;
            }

            {
                char* str = (char*)MALLOC(100);
                DBGPRINTF("WMS Address = 0x%x", str); 
                MSG_FATAL("WMSST_INBOXMSGOPTS_Handler",0,0,0);
                MSG_FATAL("WMSST_TONUMLIST_Handler DLGRET_GETMMS=%S",(AECHAR*)IVector_ElementAt(pMe->m_pSaveNumList,0),0,0);
                WSTRTOSTR((AECHAR*)IVector_ElementAt(pMe->m_pSaveNumList,0),
                    str,
                    100);
                MSG_FATAL("WMSST_INBOXMSGOPTS_Handler DLGRET_GETMMS=%s",str,0,0);
                {
                    uint16  nMsgID = 0;
                    IFileMgr *pIFileMgr = NULL;
                    int result = 0;
                    uint32 pdwTotal = 0;
                    uint32 pdwFree = 0;
                    result = ISHELL_CreateInstance(AEE_GetShell(), AEECLSID_FILEMGR,(void **)&pIFileMgr);
                    if (SUCCESS != result)
                    {
                        MSG_FATAL("WMSST_INBOXMSGOPTS_Handler: Open file error %x", result,0,0);
                        return FALSE;
                    }            
                    pdwFree = IFILEMGR_GetFreeSpace(pIFileMgr, &pdwTotal); 
                    IFILEMGR_Release(pIFileMgr);    
                    MSG_FATAL("WMSST_INBOXMSGOPTS_Handler pdwFree=%d, pdwTotal=%d",pdwFree, pdwTotal, 0);
                    if((pdwFree < MSG_MAX_PACKET_SIZE+50))
                    {
                        nMsgID = IDS_PHONEFULL;
                        pMe->m_ePMsgType = MESSAGE_WARNNING;
                        WmsApp_ShowMsgBox(pMe, nMsgID);
                        return NFSMACTION_WAIT;
                    }
                }                      
                if(!WMS_MMSState(WMS_MMS_PDU_WSPHTTPGETreq,0,(uint32)str))
                {
                    pMe->m_GetStatus = HTTP_CODE_Bad_Request;
    		        ISHELL_SetTimer(pMe->m_pShell,1000,(PFNNOTIFY)&WmsApp_ProcessMMSStatus,pMe);
		        }
                IVector_RemoveElementAt(pMe->m_pSaveNumList,0);
            }
            MOVE_TO_STATE(WMSST_GETTING)
            return NFSMACTION_CONTINUE;
        }
        break;
#endif            
        case DLGRET_SAVEURL:
			//释放查看的消息内存
			WMSMessageStruct_Free(pMe);
			//ADD BY YANGDECAI 2010-08-16
            pMe->m_ExtractType = EXTRACT_URL;
            WmsApp_FreeSaveNumList(pMe);
            
            // 分析消息建立URL表
            WmsApp_BuildUrlList(pMe, pMe->m_msCur.m_szMessage);
            
            if (IVector_Size(pMe->m_pSaveNumList) < 1)
            {
                pMe->m_ePMsgType = MESSAGE_WARNNING;
                WmsApp_ShowMsgBox(pMe, IDS_NOVALIDURL);
                return NFSMACTION_WAIT;
            }
            MOVE_TO_STATE(WMSST_EXTARCTEDITEMLIST)
            return NFSMACTION_CONTINUE;
        case DLGRET_EXTACTDETAILS:
			//释放查看的消息内存
			WMSMessageStruct_Free(pMe);
			//ADD BY YANGDECAI 2010-08-16
            MOVE_TO_STATE(WMSST_EXTARCTDETAILS)
            return NFSMACTION_CONTINUE;
#endif            
         
#ifdef FEATURE_CDSMS_RUIM
        case DLGRET_COPYTORUIM:
        case DLGRET_MOVETORUIM:
            {
                int nCopys = 0;
                int i;
                //释放查看的消息内存
				WMSMessageStruct_Free(pMe);
				//ADD BY YANGDECAI 2010-08-16
                for (i=0; i<LONGSMS_MAX_PACKAGES; i++)
                {
                    if ((pMe->m_CurMsgNodes[i] != NULL) &&
                        (pMe->m_CurMsgNodes[i]->mem_store != WMS_MEMORY_STORE_RUIM))
                    {
                        nCopys++;
                    }
                }
                if (pMe->m_memruimStatuse.used_tag_slots+nCopys > pMe->m_memruimStatuse.max_slots)
                {
                    pMe->m_ePMsgType = MESSAGE_WARNNING;
                    WmsApp_ShowMsgBox(pMe, IDS_INSUFFICIENCY);
                    return NFSMACTION_WAIT;
                }
            }
            
            if (pMe->m_eDlgReturn == DLGRET_COPYTORUIM)
            {
                pMe->m_CopyType = COPYTORUIM;
            }
            else
            {
                pMe->m_CopyType = MOVETORUIM;
            }
            
            MOVE_TO_STATE(WMSST_MSGCOPYCONFIRM)
            return NFSMACTION_CONTINUE;
            
        case DLGRET_COPYTOPNONE:
        case DLGRET_MOVETOPNONE:
            {
                uint16 nCopys = 0;
                uint16 nMsgs = 0;
                uint16  nOnUIMs = 0;
				
                //释放查看的消息内存
				WMSMessageStruct_Free(pMe);
				//ADD BY YANGDECAI 2010-08-16
				
                wms_cacheinfolist_getcounts(WMS_MB_INBOX, NULL, &nOnUIMs, &nMsgs);
                nMsgs -= nOnUIMs;
                
                {
                    int i;
                    
                    for (i=0; i<LONGSMS_MAX_PACKAGES; i++)
                    {
                        if ((pMe->m_CurMsgNodes[i] != NULL) &&
                            (pMe->m_CurMsgNodes[i]->mem_store != WMS_MEMORY_STORE_NV_CDMA))
                        {
                            nCopys++;
                        }
                    }
                }

                if (nMsgs+nCopys > IN_MAX)
                {
                    pMe->m_ePMsgType = MESSAGE_WARNNING;
                    WmsApp_ShowMsgBox(pMe, IDS_INSUFFICIENCY);
                    return NFSMACTION_WAIT;
                }
            }
            
            if (pMe->m_eDlgReturn == DLGRET_COPYTOPNONE)
            {
                pMe->m_CopyType = COPYTOPHONEINBOXE;
            }
            else
            {
                pMe->m_CopyType = MOVETOPHONEINBOX;
            }
            
            MOVE_TO_STATE(WMSST_MSGCOPYCONFIRM)
            return NFSMACTION_CONTINUE;
#endif /* FEATURE_CDSMS_RUIM */

        default:
            // 用退出程序代替宏断言
            MOVE_TO_STATE(WMSST_EXIT)
            return NFSMACTION_CONTINUE;
    }
} // WMSST_INBOXMSGOPTS_Handler

#ifdef FEATURE_CDSMS_RUIM
/*==============================================================================
函数:
    WMSST_MSGCOPYCONFIRM_Handler

说明:
    WMSST_MSGCOPYCONFIRM 状态处理函数。

参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction WMSST_MSGCOPYCONFIRM_Handler(WmsApp *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgReturn)
    {
        case DLGRET_CREATE:
            if ((COPYTORUIM == pMe->m_CopyType) ||
#if defined(FEATURE_CDSMS_RUIM)
                (COPYALLMTTORUIM == pMe->m_CopyType) ||
                (COPYALLRUIMTOPHONE == pMe->m_CopyType) ||
#endif
                (COPYTOPHONEINBOXE == pMe->m_CopyType))
            {
                pMe->m_wMsgResID = IDS_SURECOPY;
            }
            else
            {
                pMe->m_wMsgResID = IDS_SUREMOVE;
            }
            WmsApp_ShowDialog(pMe, IDD_CONFIRM);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
            if(pMe->m_prevState == WMSST_INBOXMSGOPTS)
            {
                if (pMe->m_eOptType == OPT_VIA_VIEWMSG)
                {
                    MOVE_TO_STATE(WMSST_VIEWINBOXMSG)
                }
                else
                {
                    MOVE_TO_STATE(WMSST_INBOXES)
                }
            }
            else
            {
                MOVE_TO_STATE(pMe->m_prevState)
            }
            return NFSMACTION_CONTINUE;
    
        case DLGRET_OK:
            MOVE_TO_STATE(WMSST_MSGCOPY)
            return NFSMACTION_CONTINUE;

        default:
            // 用退出程序代替宏断言
            MOVE_TO_STATE(WMSST_EXIT)
            return NFSMACTION_CONTINUE;
    }
            
    return NFSMACTION_CONTINUE;
} // WMSST_MSGCOPYCONFIRM_Handler

/*==============================================================================
函数:
    WMSST_MSGCOPY_Handler

说明:
    WMSST_MSGCOPY 状态处理函数。

参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction WMSST_MSGCOPY_Handler(WmsApp *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgReturn)
    {
        case DLGRET_CREATE:
            WmsApp_ShowDialog(pMe, IDD_MSGCOPY);
            return NFSMACTION_WAIT;

        case DLGRET_OK:
            // 看是否需要提示空间满,在从主界面进入收件箱前会做此检查
#if defined(FEATURE_CDSMS_RUIM)
            if (COPYALLMTTORUIM == pMe->m_CopyType ||
                COPYALLRUIMTOPHONE == pMe->m_CopyType)
            {
                pMe->m_ePMsgType = MESSAGE_INFORMATIVE;
                WmsApp_ShowMsgBox(pMe, IDS_COPIED);
                return NFSMACTION_WAIT;
            }
            else if(MOVEALLMTTORUIM == pMe->m_CopyType ||
                    MOVEALLRUIMTOPHONE == pMe->m_CopyType)
            {
                pMe->m_ePMsgType = MESSAGE_INFORMATIVE;
                WmsApp_ShowMsgBox(pMe, IDS_MOVED);
                return NFSMACTION_WAIT;
            }
            else
#endif                
            {
                pMe->m_eDlgReturn = DLGRET_INBOXES;
            }
            MOVE_TO_STATE(WMSST_MAIN)
            return NFSMACTION_CONTINUE;

#if defined(FEATURE_CDSMS_RUIM)
        case DLGRET_MSGBOX_OK:
            MOVE_TO_STATE(WMSST_MANAGEMENT)
            return NFSMACTION_CONTINUE;
#endif        

        default:
            // 用退出程序代替宏断言
            MOVE_TO_STATE(WMSST_EXIT)
            return NFSMACTION_CONTINUE;
    }
            
    return NFSMACTION_CONTINUE;
} // WMSST_MSGCOPY_Handler
#endif
/*==============================================================================
函数:
    WMSST_VOICEMAIL_Handler

说明:
    WMSST_VOICEMAIL 状态处理函数。

参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction WMSST_VOICEMAIL_Handler(WmsApp *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgReturn)
    {
        case DLGRET_CREATE:
            WmsApp_UpdateAnnunciators(pMe);
            pMe->m_eMBoxType = WMS_MB_VOICEMAIL;
            MEMSET(pMe->m_msSend.m_szNum, 0, sizeof(pMe->m_msSend.m_szNum));
            {
                wms_cache_info_node  *pnode = NULL;
                
                pnode = wms_cacheinfolist_getnode(pMe->m_eMBoxType, WMS_MEMORY_STORE_NV_CDMA, 0);
                if ((NULL != pnode) && (pnode->msg_tag == WMS_TAG_MT_NOT_READ))
                {
                    (void)IWMS_MsgModifyTag(pMe->m_pwms,
                                            pMe->m_clientId,
                                            &pMe->m_callback,
                                            (void *)pMe,
                                            pnode->mem_store,
                                            pnode->index,
                                            WMS_TAG_MT_READ);
                }
            }
            WmsApp_ShowDialog(pMe, IDD_VOICEMAIL);
            return NFSMACTION_WAIT;

        case DLGRET_VIEWVOICEMSG:
            {
                uint16  nMsgID=0;
                
                nMsgID = WmsApp_GetmemAlertID(pMe, WMS_MB_VOICEMAIL);

                if (nMsgID != 0)
                {
                    pMe->m_ePMsgType = MESSAGE_WARNNING;
                    WmsApp_ShowMsgBox(pMe, nMsgID);
                    return NFSMACTION_WAIT;
                }
            }
            MOVE_TO_STATE(WMSST_VIEWVOICEMAIL)
            return NFSMACTION_CONTINUE;
            
        case DLGRET_MSGBOX_OK:
            WmsApp_ShowDialog(pMe, IDD_VOICEMAIL);
            return NFSMACTION_WAIT;

        case DLGRET_OK:
            pMe->m_bCallVMNum = TRUE;
            pMe->m_wMsgResID = IDS_VMNUM_WARNING;
            MOVE_TO_STATE(WMSST_VMNUMALERT)
            return NFSMACTION_CONTINUE;
            
        case DLGRET_VMNUM:
            MOVE_TO_STATE(WMSST_VMNUM)
            return NFSMACTION_CONTINUE;
            
        case DLGRET_CANCELED:
#if defined(FEATURE_WMSAPP_ONLYSUPPORTVMAIL)
            MOVE_TO_STATE(WMSST_EXIT)
#else
            MOVE_TO_STATE(WMSST_MAIN)
#endif            
            return NFSMACTION_CONTINUE;

        default:
            // 用退出程序代替宏断言
            MOVE_TO_STATE(WMSST_EXIT)
            return NFSMACTION_CONTINUE;
    }
} // WMSST_VOICEMAIL_Handler

/*==============================================================================
函数:
    WMSST_VIEWVOICEMAIL_Handler

说明:
    WMSST_VIEWVOICEMAIL 状态处理函数。

参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction WMSST_VIEWVOICEMAIL_Handler(WmsApp *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgReturn)
    {
        case DLGRET_CREATE:
            WmsApp_ShowDialog(pMe, IDD_VIEWMSG);
            return NFSMACTION_WAIT;

        case DLGRET_DELETE:
            pMe->m_eEraseWMSType = ERASE_VOICEMAIL_ONE;
            MOVE_TO_STATE(WMSST_DELMSGCONFIRM)
            return NFSMACTION_CONTINUE;

        case DLGRET_CALLVMNUM:
            {// 检查语音信箱号码是否设置，若设置则直接拨出
                AECHAR    wstrVMNum[OEMNV_VOICEMAIL_MAXLEN]={0};
                
                (void) ICONFIG_GetItem(pMe->m_pConfig,
                                       CFGI_VOICEMAIL_NUMBER,
                                       wstrVMNum,
                                       sizeof(wstrVMNum));
                
                if (WSTRLEN(wstrVMNum)>0)
                {
                    WMSExtApp_CallPhoneNumber(pMe, wstrVMNum, FALSE);
                    MOVE_TO_STATE(WMSST_VOICEMAIL)
                    return NFSMACTION_WAIT;
                }
                else
                {
                    pMe->m_bCallVMNum = TRUE;
                    pMe->m_wMsgResID = IDS_VMNUM_WARNING;
                    MOVE_TO_STATE(WMSST_VMNUMALERT)
                }
            }
            return NFSMACTION_CONTINUE;
            
        case DLGRET_INFO:
        case DLGRET_OK:
        case DLGRET_CANCELED:
            MOVE_TO_STATE(WMSST_VOICEMAIL)
            return NFSMACTION_CONTINUE;

        default:
            // 用退出程序代替宏断言
            MOVE_TO_STATE(WMSST_EXIT)
            return NFSMACTION_CONTINUE;
    }
} // WMSST_VIEWVOICEMAIL_Handler

/*==============================================================================
函数:
    WMSST_VMNUMALERT_Handler

说明:
    WMSST_VMNUMALERT 状态处理函数。

参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction WMSST_VMNUMALERT_Handler(WmsApp *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgReturn)
    {
        case DLGRET_CREATE:
            WmsApp_ShowDialog(pMe, IDD_CONFIRM);
            return NFSMACTION_WAIT;

        case DLGRET_OK:
            MOVE_TO_STATE(WMSST_VMNUM)
            return NFSMACTION_CONTINUE;

        case DLGRET_CANCELED:
            pMe->m_bCallVMNum = FALSE;
            MOVE_TO_STATE(WMSST_VOICEMAIL)
            return NFSMACTION_CONTINUE;

        default:
            // 用退出程序代替宏断言
            MOVE_TO_STATE(WMSST_EXIT)
            return NFSMACTION_CONTINUE;
    }
} // WMSST_VMNUMALERT_Handler

/*==============================================================================
函数:
    WMSST_VMNUM_Handler

说明:
    WMSST_VMNUM 状态处理函数。

参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction WMSST_VMNUM_Handler(WmsApp *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgReturn)
    {
        case DLGRET_CREATE:
            WmsApp_ShowDialog(pMe, IDD_VMNUM);
            return NFSMACTION_WAIT;

        case DLGRET_OK:
            if (pMe->m_bCallVMNum)
            {// 检查语音信箱号码是否设置，若设置则直接拨出
                AECHAR    wstrVMNum[OEMNV_VOICEMAIL_MAXLEN]={0};
                
                pMe->m_bCallVMNum = FALSE;
                (void) ICONFIG_GetItem(pMe->m_pConfig,
                                       CFGI_VOICEMAIL_NUMBER,
                                       wstrVMNum,
                                       sizeof(wstrVMNum));
                
                if (WSTRLEN(wstrVMNum)>0)
                {
                    WMSExtApp_CallPhoneNumber(pMe, wstrVMNum, FALSE);
                    MOVE_TO_STATE(WMSST_VOICEMAIL)
                    return NFSMACTION_WAIT;
                }
            }
            pMe->m_ePMsgType = MESSAGE_INFORMATIVE;
            WmsApp_ShowMsgBox(pMe, IDS_SAVED);
            return NFSMACTION_WAIT;
            
        case DLGRET_MSGBOX_OK:
        case DLGRET_CANCELED:
            pMe->m_bCallVMNum = FALSE;
            MOVE_TO_STATE(WMSST_VOICEMAIL)
            return NFSMACTION_CONTINUE;

        default:
            // 用退出程序代替宏断言
            MOVE_TO_STATE(WMSST_EXIT)
            return NFSMACTION_CONTINUE;
    }
} // WMSST_VMNUM_Handler


/*==============================================================================
函数:
    WMSST_DRAFT_Handler

说明:
    WMSST_DRAFT 状态处理函数。

参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction WMSST_DRAFT_Handler(WmsApp *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgReturn)
    {
        case DLGRET_CREATE:  
        case DLGRET_LOADCANCELED:
        case DLGRET_LOADFAILED:
            WmsApp_ShowDialog(pMe, IDD_MESSAGELIST);
            return NFSMACTION_WAIT;
            
        case DLGRET_LOAD:
            pMe->m_eOptType = OPT_VIA_VIEWMSG;
            WmsApp_ShowDialog(pMe, IDD_LOADINGMSG);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
            MOVE_TO_STATE(WMSST_MAIN)
            return NFSMACTION_CONTINUE;

        case DLGRET_LOADOK:
            if (pMe->m_eOptType == OPT_VIA_VIEWMSG)
            {
                MOVE_TO_STATE(WMSST_VIEWDRAFTMSG)
            }
            else
            {
                MOVE_TO_STATE(WMSST_DRAFTMSGOPTS)
            }
            return NFSMACTION_CONTINUE;
            
        case DLGRET_OPT:
            pMe->m_eOptType = OPT_VIA_LISTMSG;
            WmsApp_ShowDialog(pMe, IDD_LOADINGMSG);
            return NFSMACTION_CONTINUE;
         case DLGRET_DELETE:
        	//释放查看的消息内存
			WMSMessageStruct_Free(pMe);
			//ADD BY YANGDECAI 2010-08-16
            pMe->m_eEraseWMSType = ERASE_DRAFT_ONE;
            MOVE_TO_STATE(WMSST_DELMSGCONFIRM)
            return NFSMACTION_CONTINUE;

        default:
            // 用退出程序代替宏断言
            MOVE_TO_STATE(WMSST_EXIT)
            return NFSMACTION_CONTINUE;
    }
} // WMSST_DRAFT_Handler

/*==============================================================================
函数:
    WMSST_VIEWDRAFTMSG_Handler

说明:
 
    WMSST_VIEWDRAFTMSG 状态处理函数。

参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction WMSST_VIEWDRAFTMSG_Handler(WmsApp *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch(pMe->m_eDlgReturn)
    {
        case DLGRET_CREATE:
            WmsApp_ShowDialog(pMe, IDD_VIEWMSG);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
            MOVE_TO_STATE(WMSST_DRAFT)
            return NFSMACTION_CONTINUE;

        case DLGRET_OK:
            pMe->m_eOptType = OPT_VIA_VIEWMSG;
            MOVE_TO_STATE(WMSST_DRAFTMSGOPTS)
            return NFSMACTION_CONTINUE;

        case DLGRET_INFO:
#if 0            
            pMe->m_eOptType = OPT_VIA_VIEWMSG;
            pMe->m_eDlgReturn = DLGRET_DELETE;
            pMe->m_bDoNotOverwriteDlgResult = TRUE;
            MOVE_TO_STATE(WMSST_DRAFTMSGOPTS)
            return NFSMACTION_CONTINUE;
#endif
            // 清空群发地址链表
            WmsApp_FreeMultiSendList(pMe->m_pSendList);
            
            // 拷贝消息
            WmsApp_CopyCurMsgForSend(pMe);
            
            pMe->m_STSwitchToEditMsg = pMe->m_currState;
            pMe->m_eCreateWMSType = SEND_MSG_DRAFT;
            MOVE_TO_STATE(WMSST_WRITEMSG)
            pMe->m_dwInsertPos = 0;
                
            return NFSMACTION_CONTINUE;
            
        default:
            // 用退出程序代替宏断言
            MOVE_TO_STATE(WMSST_EXIT)
            return NFSMACTION_CONTINUE;
    }
} // WMSST_VIEWDRAFTMSG_Handler


/*==============================================================================
函数:
    WMSST_DRAFTMSGOPTS_Handler

说明:
    
    WMSST_DRAFTMSGOPTS 状态处理函数。

参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction WMSST_DRAFTMSGOPTS_Handler(WmsApp *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgReturn)
    {
        case DLGRET_CREATE:
            WmsApp_ShowDialog(pMe, IDD_MSGOPTS);
            return NFSMACTION_WAIT;
            
        case DLGRET_VIEW:
            MOVE_TO_STATE(WMSST_VIEWDRAFTMSG)
            return NFSMACTION_CONTINUE;

        case DLGRET_CANCELED:
            if (pMe->m_eOptType == OPT_VIA_VIEWMSG)
            {
                MOVE_TO_STATE(WMSST_VIEWDRAFTMSG)
            }
            else
            {
                MOVE_TO_STATE(WMSST_DRAFT)
            }
            return NFSMACTION_CONTINUE;
            
        case DLGRET_EDIT:          // DLGRET_FORWARD
            // 清空群发地址链表
            WmsApp_FreeMultiSendList(pMe->m_pSendList);

            // 拷贝消息
            WmsApp_CopyCurMsgForSend(pMe);
            
            pMe->m_STSwitchToEditMsg = pMe->m_currState;
            pMe->m_eCreateWMSType = SEND_MSG_DRAFT;
            MOVE_TO_STATE(WMSST_WRITEMSG)
            pMe->m_dwInsertPos = 0;
                
            return NFSMACTION_CONTINUE;

        case DLGRET_DELETE:
            pMe->m_eEraseWMSType = ERASE_DRAFT_ONE;
            MOVE_TO_STATE(WMSST_DELMSGCONFIRM)
            return NFSMACTION_CONTINUE;

        case DLGRET_CLEARALL:
            pMe->m_eEraseWMSType = CLEAR_DRAFT;
            pMe->m_STSwitchToEditMsg = WMSST_MAIN;
            MOVE_TO_STATE(WMSST_DELMSGCONFIRM)
            return NFSMACTION_CONTINUE;

        default:
            // 用退出程序代替宏断言
            MOVE_TO_STATE(WMSST_EXIT)
            return NFSMACTION_CONTINUE;
    }
} // WMSST_DRAFTMSGOPTS_Handler

/*==============================================================================
函数:
    WMSST_TEMPLATES_Handler

说明:
    WMSST_TEMPLATES 状态处理函数。

参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction WMSST_TEMPLATES_Handler(WmsApp *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgReturn)
    {
        case DLGRET_LOADCANCELED:
        case DLGRET_CREATE:  
            WmsApp_ShowDialog(pMe, IDD_TEMPLATES);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
            if (pMe->m_eInsertType == INSERT_EDITWMS)
            {// 插入模板
                // 这里需及时清掉此标记，否则从模板中发短信再插入模板将无法
                // 通过取消正常退出短信，而进入无限循环
                pMe->m_eInsertType = INSERT_NONE;
                MOVE_TO_STATE(WMSST_WRITEMSG)
            }
            else if (pMe->m_eInsertType == INSERT_EDITTEPLATE)
            {// 插入模板
                // 这里需及时清掉此标记
                pMe->m_eInsertType = INSERT_NONE;
                MOVE_TO_STATE(WMSST_EDITTEMPLATE)
            }
            else
            {
                MOVE_TO_STATE(WMSST_MAIN)
            }
            return NFSMACTION_CONTINUE;

        case DLGRET_OPT:
            pMe->m_eOptType = OPT_VIA_LISTMSG;
            WmsApp_ShowDialog(pMe, IDD_LOADINGMSG);
            return NFSMACTION_CONTINUE;
            
        case DLGRET_LOAD:
            pMe->m_eOptType = OPT_VIA_VIEWMSG;
            WmsApp_ShowDialog(pMe, IDD_LOADINGMSG);
            return NFSMACTION_WAIT;
            
        case DLGRET_LOADOK:
            if (pMe->m_eInsertType == INSERT_EDITWMS)
            {// 编辑消息时插入模板
                // 这里需及时清掉此标记，否则从模板中发短信再插入模板将无法
                // 通过取消正常退出短信，而进入无限循环
                pMe->m_eInsertType = INSERT_NONE;
                MOVE_TO_STATE(WMSST_WRITEMSG)
                
                if (pMe->m_bTextFullAlert)
                {
                    pMe->m_eDlgReturn = DLGRET_TEXTFULL;
                    pMe->m_bTextFullAlert = FALSE;
                    pMe->m_bDoNotOverwriteDlgResult = TRUE;
                }
            }
            else if (pMe->m_eInsertType == INSERT_EDITTEPLATE)
            {// 编辑模板时插入模板
                // 这里需及时清掉此标记
                pMe->m_eInsertType = INSERT_NONE;
                MOVE_TO_STATE(WMSST_EDITTEMPLATE)
                
                if (pMe->m_bTextFullAlert)
                {
                    pMe->m_eDlgReturn = DLGRET_TEXTFULL;
                    pMe->m_bTextFullAlert = FALSE;
                    pMe->m_bDoNotOverwriteDlgResult = TRUE;
                }
            }
            else
            {
                if (pMe->m_eOptType == OPT_VIA_VIEWMSG)
                {
                    MOVE_TO_STATE(WMSST_VIEWTEMPLATE)
                }
                else
                {
                    MOVE_TO_STATE(WMSST_TEMPLATESOPTS)
                }
            }
            return NFSMACTION_CONTINUE;

        default:
            // 用退出程序代替宏断言
            MOVE_TO_STATE(WMSST_EXIT)
            return NFSMACTION_CONTINUE;
    }
} // WMSST_TEMPLATES_Handler

/*==============================================================================
函数:
    WMSST_TEMPLATESOPTS_Handler

说明:
    WMSST_TEMPLATESOPTS 状态处理函数。

参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction WMSST_TEMPLATESOPTS_Handler(WmsApp *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgReturn)
    {
        case DLGRET_CREATE:  
            WmsApp_ShowDialog(pMe, IDD_MSGOPTS);
            return NFSMACTION_WAIT;

        case DLGRET_MSGBOX_OK:
            MOVE_TO_STATE(WMSST_TEMPLATES)
            return NFSMACTION_CONTINUE;
            
        case DLGRET_CANCELED:
            if (pMe->m_eOptType == OPT_VIA_VIEWMSG)
            {
                MOVE_TO_STATE(WMSST_VIEWTEMPLATE)
            }
            else
            {
                MOVE_TO_STATE(WMSST_TEMPLATES)
            }
            return NFSMACTION_CONTINUE;

        case DLGRET_VIEW:
            MOVE_TO_STATE(WMSST_VIEWTEMPLATE)
            return NFSMACTION_CONTINUE;
            
        case DLGRET_DELETE:
            pMe->m_eEraseWMSType = ERASE_TEMPLATE_ONE;
            MOVE_TO_STATE(WMSST_DELMSGCONFIRM)
            return NFSMACTION_CONTINUE;
            
        case DLGRET_ADDNEW:
            {
                uint16    nNew = 0;
                
                wms_cacheinfolist_getcounts(WMS_MB_TEMPLATE, &nNew, NULL, NULL);
                if (nNew+MAX_OEMTEMPLATES>=PHRASE_MAX)
                {
                    pMe->m_ePMsgType = MESSAGE_WARNNING;
                    WmsApp_ShowMsgBox(pMe, IDS_FULL);
                    return NFSMACTION_WAIT;
                }
            }
            if (NULL != pMe->m_msSend.m_szMessage)
            {
                FREE(pMe->m_msSend.m_szMessage);
                pMe->m_msSend.m_szMessage = NULL;
            }
            pMe->m_dwInsertPos = 0;
            pMe->m_bAddTemplate = TRUE;
            MOVE_TO_STATE(WMSST_EDITTEMPLATE)
            return NFSMACTION_CONTINUE;
            
        case DLGRET_EDIT:
            if(pMe->m_eOptType == OPT_VIA_LISTMSG)
            {
                pMe->m_bAddTemplate = FALSE;
                pMe->m_dwInsertPos = 0;
                MOVE_TO_STATE(WMSST_EDITTEMPLATE)
            }
            else
            {
                pMe->m_eCreateWMSType = SEND_MSG_TEMPLATE;
                pMe->m_STSwitchToEditMsg = WMSST_TEMPLATES;
                MOVE_TO_STATE(WMSST_WRITEMSG)
                pMe->m_dwInsertPos = 0;
            }
            return NFSMACTION_CONTINUE;
            
        case DLGRET_SEND:
            pMe->m_eCreateWMSType = SEND_MSG_TEMPLATE;
            pMe->m_STSwitchToEditMsg = WMSST_TEMPLATES;
            MOVE_TO_STATE(WMSST_TONUMLIST)//WMSST_WRITEMSG
            pMe->m_dwInsertPos = 0;
            return NFSMACTION_CONTINUE;

        default:
            // 用退出程序代替宏断言
            MOVE_TO_STATE(WMSST_EXIT)
            return NFSMACTION_CONTINUE;
    }
} // WMSST_TEMPLATESOPTS_Handler

/*==============================================================================
函数:
    WMSST_VIEWTEMPLATE_Handler

说明:
    WMSST_VIEWTEMPLATE 状态处理函数。

参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction WMSST_VIEWTEMPLATE_Handler(WmsApp *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgReturn)
    {
        case DLGRET_CREATE:
            WmsApp_ShowDialog(pMe, IDD_VIEWMSG);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
            MOVE_TO_STATE(WMSST_TEMPLATES)
            return NFSMACTION_CONTINUE;

        case DLGRET_OK:
            pMe->m_eOptType = OPT_VIA_VIEWMSG;
            MOVE_TO_STATE(WMSST_TEMPLATESOPTS)
            return NFSMACTION_CONTINUE;
        
        case DLGRET_INFO:
            pMe->m_eDlgReturn = DLGRET_SEND;
            pMe->m_bDoNotOverwriteDlgResult = TRUE;
            MOVE_TO_STATE(WMSST_TEMPLATESOPTS)
            return NFSMACTION_CONTINUE;
            
        default:
            // 用退出程序代替宏断言
            MOVE_TO_STATE(WMSST_EXIT)
            return NFSMACTION_CONTINUE;
    }
} // WMSST_VIEWTEMPLATE_Handler


/*==============================================================================
函数:
    WMSST_EDITTEMPLATE_Handler

说明:
    WMSST_EDITTEMPLATE 状态处理函数。

参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction WMSST_EDITTEMPLATE_Handler(WmsApp *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgReturn)
    {
        case DLGRET_CREATE:
            WmsApp_ShowDialog(pMe, IDD_EDITTEMPLATE);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
            // 因要求提示输入已满,提示时会将输入保存在配置结构中,取消时应
            // 回到编辑前的状态
            MOVE_TO_STATE(WMSST_TEMPLATES)
            return NFSMACTION_CONTINUE;

        case DLGRET_SAVE:
            if (pMe->m_msSend.m_szMessage == NULL)
            {
                pMe->m_ePMsgType = MESSAGE_WARNNING;
                WmsApp_ShowMsgBox(pMe, IDS_NULLTXTALERT);
                return NFSMACTION_WAIT;
            }
            
            // 打包消息
            if (NULL != pMe->m_pCurCltPhrase)
            {
                FREE(pMe->m_pCurCltPhrase);
                pMe->m_pCurCltPhrase = NULL;
            }
            
            if (pMe->m_bAddTemplate || (pMe->m_wCurTemplate>=(MSG_CMD_BASE+PHRASE_START)))
            {
                pMe->m_pCurCltPhrase = WmsApp_PackPhrase(pMe, &pMe->m_msSend, FALSE);
            }
            else
            {// 卡上模板只支持除添加外的操作
                pMe->m_pCurCltPhrase = WmsApp_PackPhrase(pMe, &pMe->m_msSend, TRUE);
            }
            
            // 保存消息
            if (NULL != pMe->m_pCurCltPhrase)
            {
                pMe->m_pCurCltPhrase->msg_hdr.tag = WMS_TAG_PHRASE;
                
                if (pMe->m_bAddTemplate)
                {
                    (void)IWMS_MsgWrite(pMe->m_pwms, 
                                pMe->m_clientId, 
                                &pMe->m_callback,
                                (void*)pMe,
                                WMS_WRITE_MODE_INSERT,
                                pMe->m_pCurCltPhrase);
                }
                else
                {
                    if (pMe->m_wCurTemplate>=MSG_CMD_BASE)
                    {// 用户常用语
                        pMe->m_pCurCltPhrase->msg_hdr.index = pMe->m_wCurTemplate - MSG_CMD_BASE;

                        if (pMe->m_pCurCltPhrase->msg_hdr.index>=PHRASE_START)
                        {
                            (void)IWMS_MsgWrite(pMe->m_pwms, 
                                        pMe->m_clientId, 
                                        &pMe->m_callback,
                                        (void*)pMe,
                                        WMS_WRITE_MODE_REPLACE,
                                        pMe->m_pCurCltPhrase);
                        }
                        else
                        {// 卡上模板消息
                            pMe->m_pCurCltPhrase->msg_hdr.tag = WMS_TAG_MO_TEMPLATE;
                            pMe->m_pCurCltPhrase->msg_hdr.mem_store = WMS_MEMORY_STORE_RUIM;
                            (void)IWMS_MsgWriteTemplate(pMe->m_pwms, 
                                        pMe->m_clientId, 
                                        &pMe->m_callback,
                                        (void*)pMe,
                                        WMS_WRITE_MODE_REPLACE,
                                        pMe->m_pCurCltPhrase);
                        }
                    }
                    else
                    {// 内置常用语
                        switch (pMe->m_wCurTemplate)
                        {
							//#ifndef FEATURE_VERSION_HITZ181
							#if defined(FEATURE_VERSION_HITZ181)||defined(FEATURE_VERSION_MTM)
                            case IDS_TEMPLATEHITZ0:
                            #else
                            case IDS_TEMPLATE1:
                            #endif
                                pMe->m_pCurCltPhrase->msg_hdr.index = PHRASE_START;
                                break;
                                
                            #if defined(FEATURE_VERSION_HITZ181)||defined(FEATURE_VERSION_MTM)
                            case IDS_TEMPLATEHITZ1:
                            #else
                            case IDS_TEMPLATE2:
                            #endif
                                pMe->m_pCurCltPhrase->msg_hdr.index = PHRASE_START+1;
                                break;
                                
                            #if defined(FEATURE_VERSION_HITZ181)||defined(FEATURE_VERSION_MTM)
                            case IDS_TEMPLATEHITZ2:
                            #else
                            case IDS_TEMPLATE3:
                            #endif
                                pMe->m_pCurCltPhrase->msg_hdr.index = PHRASE_START+2;
                                break;
                                
                            #if defined(FEATURE_VERSION_HITZ181)||defined(FEATURE_VERSION_MTM)
                            case IDS_TEMPLATEHITZ3:
                            #else
                            case IDS_TEMPLATE4:
                            #endif
                                pMe->m_pCurCltPhrase->msg_hdr.index = PHRASE_START+3;
                                break;
                                
                            #if defined(FEATURE_VERSION_HITZ181)||defined(FEATURE_VERSION_MTM)
                            case IDS_TEMPLATEHITZ4:
                            #else
                            case IDS_TEMPLATE5:
                            #endif
                                pMe->m_pCurCltPhrase->msg_hdr.index = PHRASE_START+4;
                                break;
                                
                            #if defined(FEATURE_VERSION_HITZ181)||defined(FEATURE_VERSION_MTM)
                            case IDS_TEMPLATEHITZ5:
                            #else
                            case IDS_TEMPLATE6:
                            #endif
                                pMe->m_pCurCltPhrase->msg_hdr.index = PHRASE_START+5;
                                break;
                                
                            #if defined(FEATURE_VERSION_HITZ181)||defined(FEATURE_VERSION_MTM)
                            case IDS_TEMPLATEHITZ6:
                            #else
                            case IDS_TEMPLATE7:
                            #endif
                                pMe->m_pCurCltPhrase->msg_hdr.index = PHRASE_START+6;
                                break;
                                
                            #if defined(FEATURE_VERSION_HITZ181)||defined(FEATURE_VERSION_MTM)
                            case IDS_TEMPLATEHITZ7:
                            #else
                            case IDS_TEMPLATE8:
                            #endif
                                pMe->m_pCurCltPhrase->msg_hdr.index = PHRASE_START+7;
                                break;
                                
                            #if defined(FEATURE_VERSION_HITZ181)||defined(FEATURE_VERSION_MTM)
                            case IDS_TEMPLATEHITZ8:
                            #else
                            case IDS_TEMPLATE9:
                            #endif
                                pMe->m_pCurCltPhrase->msg_hdr.index = PHRASE_START+8;
                                break;
                                
                            #if defined(FEATURE_VERSION_HITZ181)||defined(FEATURE_VERSION_MTM)
                            case IDS_TEMPLATEHITZ9:
                            #else
                            case IDS_TEMPLATE10:
                            #endif
                                pMe->m_pCurCltPhrase->msg_hdr.index = PHRASE_START+9;
                                break;
                                
                            default:
                                break;
                        }
                        
                        (void)IWMS_MsgWrite(pMe->m_pwms, 
                                    pMe->m_clientId, 
                                    &pMe->m_callback,
                                    (void*)pMe,
                                    WMS_WRITE_MODE_REPLACE,
                                    pMe->m_pCurCltPhrase);
                    }
                }
            }
            pMe->m_ePMsgType = MESSAGE_INFORMATIVE;
            WmsApp_ShowMsgBox(pMe, IDS_SAVED);
            return NFSMACTION_WAIT;
        
        case DLGRET_TEXTFULL:
            pMe->m_ePMsgType = MESSAGE_WARNNING;
            WmsApp_ShowMsgBox(pMe, IDS_TEXTFULL);
            return NFSMACTION_WAIT;
        
        // 保存模板结果提示消息对话框返回值
        case DLGRET_MSGBOX_OK:
            if ((pMe->m_wMsgResID == IDS_TEXTFULL) ||
                (pMe->m_wMsgResID == IDS_NULLTXTALERT))
            {
                WmsApp_ShowDialog(pMe, IDD_EDITTEMPLATE);
                return NFSMACTION_WAIT;
            }
            MOVE_TO_STATE(WMSST_TEMPLATES)
            return NFSMACTION_CONTINUE;

        default:
            // 用退出程序代替宏断言
            MOVE_TO_STATE(WMSST_EXIT)
            return NFSMACTION_CONTINUE;
    }
} // WMSST_EDITTEMPLATE_Handler

/*==============================================================================
函数:
    WMSST_TONUMLIST_Handler

说明:
    WMSST_TONUMLIST 状态处理函数。

参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction WMSST_TONUMLIST_Handler(WmsApp *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    MSG_FATAL("WMSST_TONUMLIST_Handler Start",0,0,0);
    switch (pMe->m_eDlgReturn)
    {
        case DLGRET_CREATE:
            MSG_FATAL("WMSST_TONUMLIST_Handler DLGRET_CREATE",0,0,0);
            WmsApp_ShowDialog(pMe, IDD_TONUMLIST);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
            MSG_FATAL("WMSST_TONUMLIST_Handler DLGRET_CANCELED",0,0,0);
            if(pMe->m_STSwitchToEditMsg == WMSST_TEMPLATES)
            {
                if(pMe->m_eOptType == OPT_VIA_VIEWMSG)
                {
                    MOVE_TO_STATE(WMSST_VIEWTEMPLATE);
                }
                else
                {
                    MOVE_TO_STATE(WMSST_TEMPLATES);
                }
            }
            else
            {
//                if (pMe->m_pSendList)
//                {
//                    WmsApp_FreeMultiSendList(pMe->m_pSendList);
//                }
//                pMe->m_CurAddID = MSG_CMD_BASE;
                MOVE_TO_STATE(WMSST_WRITEMSG)
                pMe->m_dwInsertPos = 0;         
            }
            return NFSMACTION_CONTINUE;

        case DLGRET_SENDOPT:
            MSG_FATAL("WMSST_TONUMLIST_Handler DLGRET_SENDOPT",0,0,0);
            {
                uint16  nMsgID = 0;
                IFileMgr *pIFileMgr = NULL;
                int result = 0;
                uint32 pdwTotal = 0;
                uint32 pdwFree = 0;
                result = ISHELL_CreateInstance(AEE_GetShell(), AEECLSID_FILEMGR,(void **)&pIFileMgr);
                if (SUCCESS != result)
                {
                    MSG_FATAL("WMSST_TONUMLIST_Handler: Open file error %x", result,0,0);
                    return FALSE;
                }            
                pdwFree = IFILEMGR_GetFreeSpace(pIFileMgr, &pdwTotal); 
                IFILEMGR_Release(pIFileMgr);    
                MSG_FATAL("WMSST_TONUMLIST_Handler pdwFree=%d, pdwTotal=%d",pdwFree, pdwTotal, 0);
                if((pdwFree < MSG_MAX_PACKET_SIZE+50))
                {
                    nMsgID = IDS_PHONEFULL;
                    pMe->m_ePMsgType = MESSAGE_WARNNING;
                    WmsApp_ShowMsgBox(pMe, nMsgID);
                    return NFSMACTION_WAIT;
                }
            }                
#if defined(FEATURE_CARRIER_THAILAND_HUTCH) 
            if ((pMe->m_eCreateWMSType == SEND_MSG_RESERVE) ||
                (pMe->m_eCreateWMSType == SEND_MSG_EDITRESERVE))
            {
                MOVE_TO_STATE(WMSST_RSVDMSGTIME)
            }
            else
            {
                if (pMe->m_eCreateWMSType == SEND_MSG_DRAFT) 
                {// 删除当前草稿
                    int i, nRet;
                    wms_cache_info_node  *pnode = NULL;
                    
                    for (i=0; i<LONGSMS_MAX_PACKAGES; i++)
                    {
                        if (pMe->m_CurMsgNodes[i] != NULL)
                        {
                            pnode = pMe->m_CurMsgNodes[i];
                            
                            // 发布删除消息命令
                            nRet = ENOMEMORY;
                            do
                            {
                                nRet = IWMS_MsgDelete(pMe->m_pwms,
                                                      pMe->m_clientId,
                                                      &pMe->m_callback,
                                                      (void *)pMe,
                                                      pnode->mem_store,
                                                      pnode->index);
                            } while(nRet != SUCCESS);
                            pMe->m_CurMsgNodes[i] = NULL;
                        }
                    }
                }
#ifdef FEATURE_RTRE_DEFAULT_IS_NV
                MOVE_TO_STATE(WMSST_SENDING)
#else
                // 检查卡是否插入modi by yangdecai 2010-08-10
			    if (IRUIM_IsCardConnected(pMe->m_pIRUIM)) 
			    {
	            	MOVE_TO_STATE(WMSST_SENDING)
			    }
				else
				{
					MOVE_TO_STATE(WMSST_POPMSG)
				}
#endif                
            }
#else

//以下为原来sendopt的操作，全部移动到此处，由于sendopt放在了写短信的option菜单中 
#if !defined(FEATURE_CARRIER_THAILAND_HUTCH) 
            if (IVector_Size(pMe->m_pSendList) > 0)
#endif                
            {
                MSG_FATAL("WMSST_TONUMLIST_Handler m_SendOPT=%d, m_eCreateWMSType=%d",pMe->m_SendOPT,pMe->m_eCreateWMSType,0);
				
				// 打包消息
                WmsApp_PrepareUserDataMOList(pMe);
                pMe->m_idxUserdata = 0;
			
				
                if ((pMe->m_SendOPT != SENDOPT_SEND) &&
                    (pMe->m_eCreateWMSType != SEND_MSG_RESERVE) &&
                    (pMe->m_eCreateWMSType != SEND_MSG_EDITRESERVE))
                {// 消息需要保存，这里先检查存储空间，存储空间不足则提示用户，用户更改选项后再继续!
                    int32  nItems = 0;
                    uint16  nMsgs=0, nOnUIMs=0, nMax=OUT_MAX;
                    
                    // 获取消息数
#if defined(FEATURE_CARRIER_THAILAND_HUTCH) 
                    if (pMe->m_SendOPT == SENDOPT_SAVE)
                    {// HUTCH 要求用户选"仅保存"时，消息存入草稿箱
                        wms_cacheinfolist_getcounts(WMS_MB_DRAFT, NULL, &nOnUIMs, &nMsgs);
                        nMax = DRAFT_MAX;
                        
                        if (pMe->m_eCreateWMSType == SEND_MSG_DRAFT)
                        {
                            int i;
                            
                            for (i=0; i<LONGSMS_MAX_PACKAGES; i++)
                            {
                                if (pMe->m_CurMsgNodes[i] != NULL)
                                {
                                    nMax++;
                                }
                            }
                        }
                    }
                    else
                    {
                        wms_cacheinfolist_getcounts(WMS_MB_OUTBOX, NULL, &nOnUIMs, &nMsgs);
                    }
#else
                    wms_cacheinfolist_getcounts(WMS_MB_OUTBOX, NULL, &nOnUIMs, &nMsgs);
#endif                   
                    
                    nItems = IVector_Size(pMe->m_pUserDataMOList);					
					
                    if ((nItems+nMsgs-nOnUIMs)>nMax)
                    {
                    	MSG_FATAL("***zzg ToNumList nItems=%d, nMsgs=%d, nOnUIMs=%d***", nItems, nMsgs, nOnUIMs);

						//Del by zzg 2011_12_22
                        //pMe->m_ePMsgType = MESSAGE_WARNNING;
                        //WmsApp_ShowMsgBox(pMe, IDS_STOREINSUFFICIENCY);
                        //return NFSMACTION_WAIT;
                        //Del End
                    }				
					
                }
		
                
#if defined(FEATURE_CARRIER_THAILAND_HUTCH) 
                // 对于 Hutch 版本, 如发送选项非 "Save Only" 需在发送前一个状态进行
                if ((pMe->m_eCreateWMSType == SEND_MSG_DRAFT) &&
                    (pMe->m_SendOPT == SENDOPT_SAVE))
#else
                if (pMe->m_eCreateWMSType == SEND_MSG_DRAFT)
#endif                
                {// 删除当前草稿
                    int i, nRet;
                    wms_cache_info_node  *pnode = NULL;
                    
                    for (i=0; i<LONGSMS_MAX_PACKAGES; i++)
                    {
                        if (pMe->m_CurMsgNodes[i] != NULL)
                        {
                            pnode = pMe->m_CurMsgNodes[i];
                            
                            // 发布删除消息命令
                            nRet = ENOMEMORY;
                            do
                            {
                                nRet = IWMS_MsgDelete(pMe->m_pwms,
                                                   pMe->m_clientId,
                                                   &pMe->m_callback,
                                                   (void *)pMe,
                                                   pnode->mem_store,
                                                   pnode->index);
                            } while(nRet != SUCCESS);
                            pMe->m_CurMsgNodes[i] = NULL;
                        }
                    }
                }

				MSG_FATAL("WMSST_TONUMLIST_Handler m_SendOPT=%d, m_eCreateWMSType=%d",pMe->m_SendOPT,pMe->m_eCreateWMSType,0);
                
                if (pMe->m_SendOPT == SENDOPT_SAVE)
                {// 这种状态不必经过 WMSST_SENDING 状态
                    wms_client_message_s_type *pClientMsg = NULL;
                    int nRet;
					
#if !defined(FEATURE_CARRIER_THAILAND_HUTCH) 
                    CMultiSendItemInfo *pItem = NULL;
                    
                    pItem = (CMultiSendItemInfo *)IVector_ElementAt(pMe->m_pSendList, 0);
                    
                    if (NULL != pItem)
                    {
                        // 拷贝当前号码
                        pMe->m_msSend.m_szNum[0] = 0;
#ifdef FEATURE_USES_MMS                  
                        if(pMe->m_isSendToAlbumOrEmain && (WSTRLEN(pItem->m_szEmail) > 0) && pMe->m_isMMS)  
                        {
                            (void)WSTRCPY(pMe->m_msSend.m_szNum, pItem->m_szEmail);
                        }
                        else
#endif                        
                        (void)WSTRCPY(pMe->m_msSend.m_szNum, pItem->m_szTo);
                    }

                    pClientMsg = WmsApp_GetClientMsgMO(pMe, TRUE);
#else                    
                    pClientMsg = WmsApp_GetClientMsgMO(pMe, FALSE);
#endif
                    
                    while (NULL != pClientMsg)
                    {
                        nRet = ENOMEMORY;
                        
#if defined(FEATURE_CARRIER_THAILAND_HUTCH) 
                        // HUTCH 要求用户选"仅保存"时，消息存入草稿箱
                        // Must modify message tag!
                        pClientMsg->msg_hdr.tag = WMS_TAG_MO_DRAFT;
#endif						
						do
                        {
                            nRet = IWMS_MsgWrite(pMe->m_pwms, 
                                                 pMe->m_clientId, 
                                                 &pMe->m_callback,
                                                 (void*)pMe,
                                                 WMS_WRITE_MODE_INSERT,
                                                 pClientMsg);
                        } while(nRet != SUCCESS);											
                        
#ifndef WIN32
                        // 休眠10毫秒以确保有时间执行保存消息的操作
                        MSLEEP(10);
#endif                                        
                                            
                        FREE(pClientMsg);
#if defined(FEATURE_CARRIER_THAILAND_HUTCH) 
                        pClientMsg = WmsApp_GetClientMsgMO(pMe, FALSE);
#else
                        pClientMsg = WmsApp_GetClientMsgMO(pMe, TRUE);
#endif                        
                    }
					
					
                    pMe->m_ePMsgType = MESSAGE_INFORMATIVE;
                    WmsApp_ShowMsgBox(pMe, IDS_SAVED);
                    return NFSMACTION_WAIT;                  
                }
                
                if ((pMe->m_eCreateWMSType == SEND_MSG_RESERVE) ||
                    (pMe->m_eCreateWMSType == SEND_MSG_EDITRESERVE))
                {
                    MOVE_TO_STATE(WMSST_RSVDMSGTIME)
                }
                else
                {
#ifdef FEATURE_RTRE_DEFAULT_IS_NV
                    MOVE_TO_STATE(WMSST_SENDING)
#else                    
                    // 检查卡是否插入modi by yangdecai 2010-08-10
				    if (IRUIM_IsCardConnected(pMe->m_pIRUIM)) 
				    {
				    	MSG_FATAL("***zzg WMS ToNumList WMSST_SENDING***", 0, 0, 0);
		            	MOVE_TO_STATE(WMSST_SENDING)
				    }
					else
					{
						MOVE_TO_STATE(WMSST_POPMSG)
					}
#endif                    
                }
            }
            else
            {// 这只是防弹措施，程序不应执行到此
                MSG_FATAL("WMSST_TONUMLIST_Handler WMSST_EXIT",0,0,0);
                MOVE_TO_STATE(WMSST_EXIT)
            }

#endif            
            return NFSMACTION_CONTINUE;

        case DLGRET_MSGBOX_OK:
            MSG_FATAL("WMSST_TONUMLIST_Handler DLGRET_MSGBOX_OK",0,0,0);
            if (pMe->m_wMsgResID == IDS_SAVED)
            {
                MOVE_TO_STATE(WMSST_MAIN)
                return NFSMACTION_CONTINUE;
            }
            else if(pMe->m_wMsgResID == IDS_STOREINSUFFICIENCY)
            {
                MOVE_TO_STATE(WMSST_TONUMLIST)
                return NFSMACTION_CONTINUE;
            }
        case DLGRET_SELECTFROMOPT:
            MSG_FATAL("WMSST_TONUMLIST_Handler DLGRET_SELECTFROMOPT",0,0,0);
            MOVE_TO_STATE(WMSST_SELECTFROM);
            return NFSMACTION_CONTINUE;

#ifdef FEATURE_USES_MMS  
        case DLGRET_EDIT_ALBUMOREMAIN:
            MSG_FATAL("WMSST_TONUMLIST_Handler case DLGRET_EDIT_ALBUMOREMAIN",0,0,0);
            MOVE_TO_STATE(WMSST_EDIT_ALBUMOREMAIN) 
            return NFSMACTION_CONTINUE;
#endif            
        
        default:
            // 用退出程序代替宏断言
            MSG_FATAL("WMSST_TONUMLIST_Handler WMSST_EXIT",0,0,0);
            MOVE_TO_STATE(WMSST_EXIT)
            return NFSMACTION_CONTINUE;
    }
} // WMSST_TONUMLIST_Handler

/*==============================================================================
函数:
    WMSST_WRITEMSG_Handler

说明:
    WMSST_WRITEMSG 状态处理函数。

参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction WMSST_WRITEMSG_Handler(WmsApp *pMe)
{
    MSG_FATAL("WMSST_WRITEMSG_Handler Start",0,0,0);
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    switch (pMe->m_eDlgReturn)
    {
        // 创建编辑消息对话框界面
        case DLGRET_CREATE:
            if (!pMe->m_bNaturalStart)
            {
                pMe->m_eCreateWMSType = SEND_MSG_NEW;
            }
            if ((pMe->m_eCreateWMSType != SEND_MSG_EDITRESERVE)&&(pMe->m_prevState != WMSST_SENDOPTS)
                                                                &&(pMe->m_prevState != WMSST_TONUMLIST))
            {
                byte btSendmode;
                byte btPriority;
                boolean report;
                boolean bVal;

                ICONFIG_GetItem(pMe->m_pConfig, CFGI_WMS_SENDMODE, &btSendmode, sizeof(btSendmode));
                ICONFIG_GetItem(pMe->m_pConfig, CFGI_WMS_PRIORITY, &btPriority, sizeof(btPriority));
                ICONFIG_GetItem(pMe->m_pConfig, CFGI_WMS_DELIVERYREPORTS, &report, sizeof(report));
                ICONFIG_GetItem(pMe->m_pConfig, CFGI_WMS_CALLBACKNUMSWITCH, &bVal, sizeof(bVal));

                pMe->m_SendOPT = btSendmode;
                pMe->m_msSend.m_epriority = btPriority;
                pMe->m_msSend.m_bDeliveryReport = report;
                pMe->m_CbVal = bVal;
                if(bVal)
                {
                    AECHAR    wstrNum[WMS_ADDRESS_MAX]={0};
                    
                    (void) ICONFIG_GetItem(pMe->m_pConfig,
                                           CFGI_CALLBACKNUM,
                                           wstrNum,
                                           sizeof(wstrNum));
                    
                    (void)MEMSET(pMe->m_msSend.m_szCallBkNum, 0, sizeof(pMe->m_msSend.m_szCallBkNum));
                    if (WSTRLEN(wstrNum) == 0)
                    {// 取默认值: 转化后的 IMSI_S
#if 1//def FEATURE_CARRIER_VENEZUELA_MOVILNET
                        AECHAR  wstrPrefix1[]={(AECHAR)'1',(AECHAR)'5', (AECHAR)'8', 0};
                        AECHAR  wstrPrefix2[]={(AECHAR)'1',(AECHAR)'9', (AECHAR)'9', 0};
                        AECHAR  wstrPrefix3[5]={0};
                        int     nLen = 0, i;
                        
                        (void) ICONFIG_GetItem(pMe->m_pConfig,
                                               CFGI_IMSI_S,
                                               pMe->m_msSend.m_szCallBkNum,
                                               sizeof(AECHAR) * OEMNV_IMSI_S_LENGTH);
                        nLen = WSTRLEN(pMe->m_msSend.m_szCallBkNum);                       
                        // 将"158"变成"0416"
                        // 将"199"变成"0419"
                        if (nLen>3 && nLen<MAX_PH_DIGITS)
                        {
                            if (WSTRNCMP(pMe->m_msSend.m_szCallBkNum, wstrPrefix1, 3) == 0)
                            {
                                (void)STRTOWSTR("0416", wstrPrefix3, sizeof(wstrPrefix3));
                            }
                            else if (WSTRNCMP(pMe->m_msSend.m_szCallBkNum, wstrPrefix2, 3) == 0)
                            {
                                // 将"199"变成"0426" guoys modified for movilnet change 2008.10.28
                                //(void)STRTOWSTR("0419", wstrPrefix3, sizeof(wstrPrefix3));
                                (void)STRTOWSTR("0426", wstrPrefix3, sizeof(wstrPrefix3));
                            }
                        }
                        if (WSTRLEN(wstrPrefix3)>0)
                        {
                            for (i=nLen; i>3; i--)
                            {
                                pMe->m_msSend.m_szCallBkNum[i] = pMe->m_msSend.m_szCallBkNum[i-1];
                            }
                            pMe->m_msSend.m_szCallBkNum[0] = wstrPrefix3[0];
                            pMe->m_msSend.m_szCallBkNum[1] = wstrPrefix3[1];
                            pMe->m_msSend.m_szCallBkNum[2] = wstrPrefix3[2];
                            pMe->m_msSend.m_szCallBkNum[3] = wstrPrefix3[3];
                            pMe->m_msSend.m_szCallBkNum[nLen+1] = 0;
                        }
#endif                                                   
                    }
                    else
                    {
                        (void)WSTRCPY(pMe->m_msSend.m_szCallBkNum, wstrNum);
                    }
                }                    
            }
            WmsApp_ShowDialog(pMe, IDD_WRITEMSG);
            return NFSMACTION_WAIT;
        case DLGRET_MSGBOX_OK:
            switch(pMe->m_wMsgResID)
            {
                case IDS_INPUTNULL:
                case IDS_TEXTFULL:
                case IDS_FULL:
                case IDS_NULLTXTALERT:
                case IDS_INSUFFICIENCY:
                case IDS_INBOXSFULL:    /////////////////////
                case IDS_MMS_SIZE_TOO_BIG:
                    MOVE_TO_STATE(WMSST_WRITEMSG)
                    return NFSMACTION_CONTINUE;

                case IDS_SAVED:
                    MOVE_TO_STATE(WMSST_MAIN)
                    return NFSMACTION_CONTINUE;

                default:
                    MOVE_TO_STATE(WMSST_MAIN)
                    return NFSMACTION_CONTINUE;
            }
            return TRUE;

        // 用户在输入界面，选择发送/发送至多人
        case DLGRET_SEND:
            MSG_FATAL("WMSST_WRITEMSG_Handler m_eCreateWMSType=%d", pMe->m_eCreateWMSType,0,0);
            pMe->m_ContinueSendType = NONE_CONTINUE;
#ifdef FEATURE_CARRIER_THAILAND_HUTCH           
            // Hutch 不允许发空白短信
            if (NULL == pMe->m_msSend.m_szMessage)
            {
                pMe->m_ePMsgType = MESSAGE_WARNNING;
                WmsApp_ShowMsgBox(pMe, IDS_INPUTNULL); 
                return NFSMACTION_WAIT;
            }
#endif //FEATURE_CARRIER_THAILAND_HUTCH
            if (pMe->m_eCreateWMSType == SEND_MSG_REPLY)
            {// 对于回复类短信，号码已准备好，不需输入
                MOVE_TO_STATE(WMSST_TONUMLIST)//WMSST_SENDOPT
            }
            else
            {
                pMe->m_nCurAddPage = 0;
                MOVE_TO_STATE(WMSST_TONUMLIST)
            }
            return NFSMACTION_CONTINUE;
            
        // 用户选择插入常用语
        case DLGRET_INSERTTEMPLATES:
            pMe->m_eInsertType = INSERT_EDITWMS;
            pMe->m_bTextFullAlert = FALSE;
            #if defined(FEATURE_VERSION_HITZ181)||defined(FEATURE_VERSION_MTM)
            pMe->m_wCurTemplate = IDS_TEMPLATEHITZ0;
            #else
            pMe->m_wCurTemplate = IDS_TEMPLATE1;
            #endif
            MOVE_TO_STATE(WMSST_TEMPLATES)
            return NFSMACTION_CONTINUE;
/*
        case DLGRET_INSERTPICTURE://add by xuhui 2011/08/01
            pMe->m_bTextFullAlert = FALSE;
            MOVE_TO_STATE(WMSST_INSERTPICTURE)
            return NFSMACTION_CONTINUE;

        case DLGRET_INSERTVIDEO://add by xuhui 2011/08/01
            pMe->m_bTextFullAlert = FALSE;
            MOVE_TO_STATE(WMSST_INSERTVIDEO)
            return NFSMACTION_CONTINUE;       

        case DLGRET_INSERTSOUND://add by xuhui 2011/08/01
            pMe->m_bTextFullAlert = FALSE;
            MOVE_TO_STATE(WMSST_INSERTSOUND)
            return NFSMACTION_CONTINUE;                

        case DLGRET_INSERTFILE://add by xuhui 2011/08/01
            pMe->m_bTextFullAlert = FALSE;
            MOVE_TO_STATE(WMSST_INSERTFILE)
            return NFSMACTION_CONTINUE;   */
            
        // 用户选择返回
        case DLGRET_CANCELED:
            if (pMe->m_bNaturalStart == FALSE)
            {
                pMe->m_STSwitchToEditMsg = WMSST_EXIT;
            }
            switch(pMe->m_STSwitchToEditMsg)
            {
                // 本状态由查看收件箱内一条消息的操作选项进入
                case WMSST_INBOXMSGOPTS:
                    if (pMe->m_eOptType == OPT_VIA_VIEWMSG)
                    {
                        MOVE_TO_STATE(WMSST_INBOXES)
                    }
                    else
                    {
                        MOVE_TO_STATE(WMSST_INBOXES)
                    }
                    break;
                    
                // 本状态由查看发件箱内一条消息的操作选项进入
                case WMSST_OUTMSGOPTS:
                    if (pMe->m_eOptType == OPT_VIA_VIEWMSG)
                    {
                        MOVE_TO_STATE(WMSST_OUTBOX)
                    }
                    else
                    {
                        MOVE_TO_STATE(WMSST_OUTBOX)
                    }
                    break;
                    
                // 本状态由查看收藏夹内一条消息的操作选项进入
                case WMSST_VIEWDRAFTMSG:
                case WMSST_DRAFTMSGOPTS:
                    if (pMe->m_eOptType == OPT_VIA_VIEWMSG)
                    {
                        MOVE_TO_STATE(WMSST_DRAFT)
                    }
                    else
                    {
                        MOVE_TO_STATE(WMSST_DRAFT)
                    }
                    break;

                // 本状态由预约信息主界面进入
                case WMSST_RESERVEDMSG:
                    
                    MOVE_TO_STATE(WMSST_RESERVEDMSG)
                    break;

                // 本状态由查看预约信息选项界面进入
                case WMSST_RESERVEDMSGOPT:                    
                if (pMe->m_eOptType == OPT_VIA_VIEWMSG)
                {
                    MOVE_TO_STATE(WMSST_VIEWRESERVEDMSG)
                }
                else
                {
                    MOVE_TO_STATE(WMSST_RESERVEDMSGS)
                }
                break;                    
                    
#ifdef FEATURE_CONTACT_APP
                // 本状态由保存号码选项状态进入
                case WMSST_EXTARCTEDITEMLIST://WMSST_EXTARCTEDITEMOPT:
#endif                    
                    
                // 本状态由模板选项进入
                case WMSST_TEMPLATES:
                
                // 本状态由主界面选择书写消息进入
                case WMSST_MAIN:
                    // 返回前一状态
                    MOVE_TO_STATE(pMe->m_STSwitchToEditMsg)
                    break;
                    
                default:
                    // 用退出程序代替宏断言
                    MOVE_TO_STATE(WMSST_EXIT)
                    break;
            }
            return NFSMACTION_CONTINUE;

        case DLGRET_TEXTFULL:
            pMe->m_ePMsgType = MESSAGE_INFORMATIVE;
            WmsApp_ShowMsgBox(pMe, IDS_INBOXSFULL);
            return NFSMACTION_WAIT;

        case DLGRET_SAVE:
            {
                uint16    nNew = 0;
                
                wms_cacheinfolist_getcounts(WMS_MB_TEMPLATE, &nNew, NULL, NULL);
                if (nNew+MAX_OEMTEMPLATES>=PHRASE_MAX)
                {
                    pMe->m_ePMsgType = MESSAGE_WARNNING;
                    WmsApp_ShowMsgBox(pMe, IDS_FULL);
                    return NFSMACTION_WAIT;
                }
            }            
            if (pMe->m_msSend.m_szMessage == NULL)
            {
                pMe->m_ePMsgType = MESSAGE_WARNNING;
                WmsApp_ShowMsgBox(pMe, IDS_NULLTXTALERT);
                return NFSMACTION_WAIT;
            }

            if(WSTRLEN(pMe->m_msSend.m_szMessage) > MAX_TEMPLATECHARS)
            {
                pMe->m_ePMsgType = MESSAGE_WARNNING;
                WmsApp_ShowMsgBox(pMe, IDS_INSUFFICIENCY);
                return NFSMACTION_WAIT;
            }
            
            // 打包消息
            if (NULL != pMe->m_pCurCltPhrase)
            {
                FREE(pMe->m_pCurCltPhrase);
                pMe->m_pCurCltPhrase = NULL;
            }
            pMe->m_pCurCltPhrase = WmsApp_PackPhrase(pMe, &pMe->m_msSend, FALSE);
            
            // 保存消息
            if (NULL != pMe->m_pCurCltPhrase)
            {
                pMe->m_pCurCltPhrase->msg_hdr.tag = WMS_TAG_PHRASE;
                

                (void)IWMS_MsgWrite(pMe->m_pwms, 
                            pMe->m_clientId, 
                            &pMe->m_callback,
                            (void*)pMe,
                            WMS_WRITE_MODE_INSERT,
                            pMe->m_pCurCltPhrase);
            }
            pMe->m_ePMsgType = MESSAGE_INFORMATIVE;
            WmsApp_ShowMsgBox(pMe, IDS_SAVED);
            return NFSMACTION_WAIT;

        case DLGRET_SENDOPT:

            if ((pMe->m_eCreateWMSType == SEND_MSG_RESERVE) ||
                (pMe->m_eCreateWMSType == SEND_MSG_EDITRESERVE))
            {
                pMe->m_wPrevMenuSel = IDC_MENU_SENDOPT;
            }
            else
            {
                pMe->m_wPrevMenuSel = IDC_MENU_SENDOPT;
            }            
            MOVE_TO_STATE(WMSST_SENDOPTS)
            return NFSMACTION_CONTINUE;
        case DLGRET_EXIT_EDITOR:
            pMe->m_ePMsgType = MESSAGE_CONFIRM;
            WmsApp_ShowMsgBox(pMe, IDS_SAVE);
            return NFSMACTION_WAIT;

        case DLGRET_SAVEDRAFT:
            // 用户选择保存，保存输入到草稿箱
            {
                int32  nItems = 0;
                uint16 nMsgs = 0;
                
                // 释放用户数据列表
                WmsApp_FreeUserDataMOList(pMe->m_pUserDataMOList);
                
                // 打包消息
                WmsApp_PrepareUserDataMOList(pMe);
                pMe->m_idxUserdata = 0;
                
                nItems = IVector_Size(pMe->m_pUserDataMOList);
                
                // 获取草稿箱消息数
                wms_cacheinfolist_getcounts(WMS_MB_DRAFT, NULL, NULL, &nMsgs);
                
                if ((nMsgs+nItems) <= DRAFT_MAX)
                {// 存储空间足够，保存中断的输入到草稿箱
                    wms_client_message_s_type *pClientMsg = NULL;
                    int nRet;
                    
                    WmsApp_FreeMultiSendList(pMe->m_pSendList);
                    
                    pClientMsg = WmsApp_GetClientMsgMO(pMe, FALSE);
                    while (pClientMsg != NULL)
                    {
                        // Must modify message tag!
                        pClientMsg->msg_hdr.tag = WMS_TAG_MO_DRAFT;
                        
                        // 保存消息
                        nRet = ENOMEMORY;
                        do 
                        {
                            nRet = IWMS_MsgWrite(pMe->m_pwms, 
                                                 pMe->m_clientId, 
                                                 &pMe->m_callback,
                                                 (void*)pMe,
                                                 WMS_WRITE_MODE_INSERT,
                                                 pClientMsg);
                        } while(nRet != SUCCESS);
#ifndef WIN32
                        // 休眠10毫秒以确保有时间执行保存消息的操作
                        MSLEEP(10);
#endif                                        
                                            
                        FREE(pClientMsg);
                        pClientMsg = WmsApp_GetClientMsgMO(pMe, FALSE);
                    }
                    
                    WmsApp_FreeMultiSendList(pMe->m_pSendList);
                    
                    // 释放用户数据列表
                    WmsApp_FreeUserDataMOList(pMe->m_pUserDataMOList);
                    
                    pMe->m_idxUserdata = 0;
                }
                else
                {
                    // 释放用户数据列表
                    WmsApp_FreeUserDataMOList(pMe->m_pUserDataMOList);
                }
            } 
            pMe->m_ePMsgType = MESSAGE_INFORMATIVE;
            WmsApp_ShowMsgBox(pMe, IDS_SAVED);
            return NFSMACTION_WAIT;

        case DLGRET_WRITEMSG:
            MOVE_TO_STATE(WMSST_WRITEMSG)
            return NFSMACTION_CONTINUE;
#ifdef FEATURE_USES_MMS
        case DLGRET_SIZETOOBIG_MMS:
            MSG_FATAL("DLGRET_SIZETOOBIG_MMS",0,0,0);
            pMe->m_ePMsgType = MESSAGE_WARNNING;
            WmsApp_ShowMsgBox(pMe, IDS_MMS_SIZE_TOO_BIG);
            return NFSMACTION_WAIT;
#endif
            
        default:
            // 用退出程序代替宏断言
            MOVE_TO_STATE(WMSST_EXIT)
            break;
    }
    
    return NFSMACTION_CONTINUE;
} // WMSST_WRITEMSG_Handler

/*==============================================================================
函数:
    WMSST_SENDOPTS_Handler

说明:
    WMSST_SENDOPTS 状态处理函数。

参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction WMSST_SENDOPTS_Handler(WmsApp *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgReturn)
    {
        case DLGRET_CREATE:
            WmsApp_ShowDialog(pMe, IDD_SENDOPTS);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
            MOVE_TO_STATE(WMSST_WRITEMSG)//WMSST_TONUMLIST
            return NFSMACTION_CONTINUE;
            
        case DLGRET_OK:
            MOVE_TO_STATE(WMSST_WRITEMSG)
            return NFSMACTION_CONTINUE;
            
        case DLGRET_MSGBOX_OK:
            if (pMe->m_wMsgResID == IDS_SAVED)
            {
                MOVE_TO_STATE(WMSST_MAIN)
                return NFSMACTION_CONTINUE;
            }
            
            WmsApp_ShowDialog(pMe, IDD_SENDOPTS);
            return NFSMACTION_WAIT;
            
        default:
            // 用退出程序代替宏断言
            MOVE_TO_STATE(WMSST_EXIT)
            return NFSMACTION_CONTINUE;
    }
} // WMSST_SENDOPTS_Handler
#ifdef FEATURE_USES_MMS
    /*==============================================================================
    函数:
        WMSST_GETTING_Handler
    
    说明:
        WMSST_GETTING 状态处理函数。
    
    参数:
        pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    
    返回值:
        NFSMACTION_CONTINUE: 指示不停状态机。
        NFSMACTION_WAIT: 指示停止状态机。
    
    备注:
           
    ==============================================================================*/
static NextFSMAction WMSST_GETTING_Handler(WmsApp *pMe)
{
    MSG_FATAL("WMSST_GETTING_Handler Start m_eDlgReturn=%d",pMe->m_eDlgReturn,0,0);
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgReturn)
    {
        // 显示发送消息状态提示界面
        case DLGRET_CREATE:
            MSG_FATAL("WMSST_GETTING_Handler DLGRET_CREATE",0,0,0);
            pMe->m_SucNum = 0;
            pMe->m_FailNum = 0;
            WmsApp_ShowDialog(pMe, IDD_GETTING);
            return NFSMACTION_WAIT;

        case DLGRET_END:
        case DLGRET_MSGBOX_OK:

            pMe->m_currState = WMSST_MAIN;
            pMe->m_eDlgReturn = DLGRET_INBOX_MMS;

            return NFSMACTION_CONTINUE;

        case DLGRET_INBOX_MMS:
            MSG_FATAL("WMSST_GETTING_Handler DLGRET_INBOX_MMS",0,0,0);
            pMe->m_eMBoxType = WMS_MB_INBOX_MMS;
            {
                uint16  nMsgID=0;    
                nMsgID = WmsApp_GetmemAlertID(pMe, WMS_MB_INBOX_MMS);
                if (nMsgID != 0)
                {
                    pMe->m_ePMsgType = MESSAGE_WARNNING;
                    WmsApp_ShowMsgBox(pMe, nMsgID);
                    return NFSMACTION_WAIT;
                }
            }
            MOVE_TO_STATE(WMSST_INBOX_MMS)
            return NFSMACTION_CONTINUE;              

        default:
            MSG_FATAL("WMSST_GETTING_Handler default",0,0,0);
            // 用退出程序代替宏断言
            MOVE_TO_STATE(WMSST_EXIT)
            return NFSMACTION_CONTINUE;
    }
} // WMSST_SENDING_Handler
#endif
/*==============================================================================
函数:
    WMSST_SENDING_Handler

说明:
    WMSST_SENDING 状态处理函数。

参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:
       
==============================================================================*/
static NextFSMAction WMSST_SENDING_Handler(WmsApp *pMe)
{
    MSG_FATAL("WMSST_SENDING_Handler Start m_eDlgReturn=%d",pMe->m_eDlgReturn,0,0);
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgReturn)
    {
        // 显示发送消息状态提示界面
        case DLGRET_CREATE:
            MSG_FATAL("WMSST_SENDING_Handler DLGRET_CREATE",0,0,0);
            pMe->m_SucNum = 0;
            pMe->m_FailNum = 0;
            WmsApp_ShowDialog(pMe, IDD_SENDING);
            return NFSMACTION_WAIT;

        case DLGRET_END:
            //信息发送完毕
            MSG_FATAL("WMSST_SENDING_Handler DLGRET_END m_bMuitiSend=%d",pMe->m_bMuitiSend,0,0);
            if (pMe->m_bMuitiSend)
            {
                pMe->m_ePMsgType = MESSAGE_INFORMATIVE;
                WmsApp_ShowMsgBox(pMe, IDS_SENDDONE);
                return NFSMACTION_WAIT;
            } // 注意：这里继续执行 case DLGRET_MSGBOX_OK
        case DLGRET_MSGBOX_OK:
            MSG_FATAL("DLGRET_MSGBOX_OK m_eCreateWMSType=%d, m_stcontinuesendbk=%d",pMe->m_eCreateWMSType,pMe->m_stcontinuesendbk,0);
            if (pMe->m_eCreateWMSType == SEND_MSG_RESERVE)
            {
                uint32  dwSecs;
                wms_cache_info_node  *pnode = NULL;

				/*
				byte	btTimeStamp = 0;
			    
				(void) ICONFIG_GetItem(pMe->m_pConfig,
			                           CFGI_SMS_TIMESTAMP,
			                           &btTimeStamp,
			                           sizeof(btTimeStamp));

				if (btTimeStamp == OEMNV_SMS_TIMESTAMP_ADJUST)
				{
					dwSecs = GETUTCSECONDS();
					MSG_FATAL("***zzg GETUTCSECONDS 9 dwSecs=%d***", dwSecs, 0, 0);
				}
				else
				{
					dwSecs = GETTIMESECONDS();
					MSG_FATAL("***zzg GETTIMESECONDS 9 dwSecs=%d***", dwSecs, 0, 0);
				}   	
				*/

				dwSecs = GETTIMESECONDS();
                
                wms_cacheinfolist_enumbegin(WMS_MB_RESERVE);
                pnode = wms_cacheinfolist_enumnext(WMS_MB_RESERVE);
                if (NULL != pnode)
                {
                    if (pnode->dwTime <= dwSecs)
                    {
                        MOVE_TO_STATE(WMSST_RESERVEDMSGALERT)
                        return NFSMACTION_CONTINUE;
                    }
                    else
                    {
                        WmsApp_ReservedMsgSetTimer(pMe);
                    }
                }
                
                if (pMe->m_bNaturalStart)
                {
                    MOVE_TO_STATE(WMSST_MAIN)
                }
                else
                {
                    MOVE_TO_STATE(WMSST_EXIT)
                }
            }
            else if (pMe->m_stcontinuesendbk != WMSST_NONE)
            {
                if (pMe->m_stcontinuesendbk == WMSST_INBOXES)
                {
                    pMe->m_currState = WMSST_MAIN;
                    pMe->m_eDlgReturn = DLGRET_INBOXES;
                }
                else
                {
                    pMe->m_currState = pMe->m_stcontinuesendbk;
                }
                
                pMe->m_stcontinuesendbk = WMSST_NONE;
            }
            else if (pMe->m_bNaturalStart)
            {
                MSG_FATAL("pMe->m_bNaturalStart==TRUE",0,0,0);
                MOVE_TO_STATE(WMSST_MAIN)
            }
            else
            {
                MSG_FATAL("pMe->m_bNaturalStart==FALSE",0,0,0);
                MOVE_TO_STATE(WMSST_EXIT)
            }

            return NFSMACTION_CONTINUE;

        default:
            MSG_FATAL("WMSST_SENDING_Handler default",0,0,0);
            // 用退出程序代替宏断言
            MOVE_TO_STATE(WMSST_EXIT)
            return NFSMACTION_CONTINUE;
    }
} // WMSST_SENDING_Handler

/*==============================================================================
函数:
    WMSST_OUTBOX_Handler

说明:
    WMSST_OUTBOX 状态处理函数。

参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction WMSST_OUTBOX_Handler(WmsApp *pMe)
{
    MSG_FATAL("WMSST_OUTBOX_Handler Start",0,0,0);
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgReturn)
    {
        case DLGRET_CREATE:
        case DLGRET_LOADCANCELED:
        case DLGRET_LOADFAILED:
            MSG_FATAL("WMSST_OUTBOX_Handler 1",0,0,0);
            WmsApp_ShowDialog(pMe, IDD_MESSAGELIST);
            return NFSMACTION_WAIT;
            
        case DLGRET_LOAD:
            MSG_FATAL("WMSST_OUTBOX_Handler 2",0,0,0);
            pMe->m_eOptType = OPT_VIA_VIEWMSG;
            WmsApp_ShowDialog(pMe, IDD_LOADINGMSG);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
            MSG_FATAL("WMSST_OUTBOX_Handler 3",0,0,0);
            MOVE_TO_STATE(WMSST_MAIN)
            return NFSMACTION_CONTINUE;

        case DLGRET_LOADOK:
            MSG_FATAL("WMSST_OUTBOX_Handler DLGRET_LOADOK m_eOptType=%d",pMe->m_eOptType,0,0);
            if (pMe->m_eOptType == OPT_VIA_VIEWMSG)
            {
                MOVE_TO_STATE(WMSST_VIEWOUTBOXMSG)
            }
            else
            {
                MOVE_TO_STATE(WMSST_OUTMSGOPTS)
            }
            return NFSMACTION_CONTINUE;
            
        case DLGRET_OPT:
            MSG_FATAL("WMSST_OUTBOX_Handler 4",0,0,0);
            pMe->m_eOptType = OPT_VIA_LISTMSG;
            WmsApp_ShowDialog(pMe, IDD_LOADINGMSG);
            return NFSMACTION_WAIT;
        case DLGRET_DELETE:
            MSG_FATAL("WMSST_OUTBOX_Handler 5",0,0,0);
        	//释放查看的消息内存
			WMSMessageStruct_Free(pMe);
			//ADD BY YANGDECAI 2010-08-16
            pMe->m_eEraseWMSType = ERASE_DRAFT_ONE;
            MOVE_TO_STATE(WMSST_DELMSGCONFIRM)
            return NFSMACTION_CONTINUE;
            
        default:
            MSG_FATAL("WMSST_OUTBOX_Handler 6",0,0,0);
            // 用退出程序代替宏断言
            MOVE_TO_STATE(WMSST_EXIT)
            return NFSMACTION_CONTINUE;
    }
} // WMSST_OUTBOX_Handler

/*==============================================================================
函数:
    WMSST_VIEWOUTBOXMSG_Handler

说明:
    WMSST_VIEWOUTBOXMSG 状态处理函数。

参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction WMSST_VIEWOUTBOXMSG_Handler(WmsApp *pMe)
{
    MSG_FATAL("WMSST_VIEWOUTBOXMSG_Handler Start", 0,0,0);
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgReturn)
    {
        case DLGRET_CREATE:
            WmsApp_ShowDialog(pMe, IDD_VIEWMSG);
           
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
            MOVE_TO_STATE(WMSST_OUTBOX)
            return NFSMACTION_CONTINUE;

        case DLGRET_OK:
            pMe->m_eOptType = OPT_VIA_VIEWMSG;
            MOVE_TO_STATE(WMSST_OUTMSGOPTS)
            return NFSMACTION_CONTINUE;

        case DLGRET_INFO:
            pMe->m_eOptType = OPT_VIA_VIEWMSG;
            pMe->m_eDlgReturn = DLGRET_RESEND;
            pMe->m_bDoNotOverwriteDlgResult = TRUE;
            MOVE_TO_STATE(WMSST_OUTMSGOPTS)
            return NFSMACTION_CONTINUE;
            
        default:
            // 用退出程序代替宏断言
            MOVE_TO_STATE(WMSST_EXIT)
            return NFSMACTION_CONTINUE;
    }
} // WMSST_VIEWOUTBOXMSG_Handler

/*==============================================================================
函数:
    WMSST_OUTMSGOPTS_Handler

说明:
    WMSST_OUTMSGOPTS 状态处理函数。

参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction WMSST_OUTMSGOPTS_Handler(WmsApp *pMe)
{
    MSG_FATAL("WMSST_OUTMSGOPTS_Handler Start m_eDlgReturn=%d",pMe->m_eDlgReturn,0,0);
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgReturn)
    {
        case DLGRET_CREATE:
            MSG_FATAL("WMSST_OUTMSGOPTS_Handler DLGRET_CREATE",0,0,0);
            WmsApp_ShowDialog(pMe, IDD_MSGOPTS);
            return NFSMACTION_WAIT;

        case DLGRET_VIEW:
            MOVE_TO_STATE(WMSST_VIEWOUTBOXMSG)
            return NFSMACTION_CONTINUE;
        
        case DLGRET_CANCELED:
            if (pMe->m_eOptType == OPT_VIA_VIEWMSG)
            {
                MOVE_TO_STATE(WMSST_VIEWOUTBOXMSG)
            }
            else
            {
                MOVE_TO_STATE(WMSST_OUTBOX)
            }
            return NFSMACTION_CONTINUE;

        case DLGRET_FORWARD:
            // 清空群发地址链表
            WmsApp_FreeMultiSendList(pMe->m_pSendList);

            // 拷贝消息
            WmsApp_CopyCurMsgForSend(pMe);
            
            pMe->m_STSwitchToEditMsg = pMe->m_currState;
            pMe->m_eCreateWMSType = SEND_MSG_FORWARD;
            MOVE_TO_STATE(WMSST_WRITEMSG)
            pMe->m_dwInsertPos = 0;
            return NFSMACTION_CONTINUE;

        case DLGRET_DELETE:
			
			//释放查看的消息内存
			WMSMessageStruct_Free(pMe);
			//ADD BY YANGDECAI 2010-08-16
			
            pMe->m_eEraseWMSType = ERASE_OUTBOX_ONE;
            MOVE_TO_STATE(WMSST_DELMSGCONFIRM)
            return NFSMACTION_CONTINUE;

        case DLGRET_CLEARALL:
			
			//释放查看的消息内存
			WMSMessageStruct_Free(pMe);
			//ADD BY YANGDECAI 2010-08-16
			
            pMe->m_eEraseWMSType = CLEAR_OUTBOX;
            pMe->m_STSwitchToEditMsg = WMSST_MAIN;
            MOVE_TO_STATE(WMSST_DELMSGCONFIRM)
            return NFSMACTION_CONTINUE;

        case DLGRET_RESEND:
            pMe->m_idxCur = 0;
			
            //释放查看的消息内存
			WMSMessageStruct_Free(pMe);
			//ADD BY YANGDECAI 2010-08-16
			
            {// 对于重发操作不进入消息编辑界面，也不进入地址列表输入界面
                AECHAR  *pwstrNum = NULL;
                CMultiSendItemInfo *pItem = NULL;
                
                // 先清空群发地址链表
                WmsApp_FreeMultiSendList(pMe->m_pSendList);
                
                // 拷贝地址 
 
				if((int)WMS_TAG_MO_SENT == pMe->m_msCur.msg_tag)
				{
					// 状态不发生迁移
					pMe->m_ePMsgType = MESSAGE_WARNNING;
                    WmsApp_ShowMsgBox(pMe, IDS_SENT);
                    return NFSMACTION_WAIT;
				}
                if (WSTRLEN(pMe->m_msCur.m_szNum)>0)
                {
                    pwstrNum = pMe->m_msCur.m_szNum;
                }
                else
                {
                    // 状态不发生迁移
                    return NFSMACTION_CONTINUE;
                }
                
                pItem = (CMultiSendItemInfo *)sys_malloc(sizeof(CMultiSendItemInfo));
                DBGPRINTF("WMS Address = 0x%x", pItem);     
                // 将回复号码保存入链表
                if ((pItem == NULL) || 
                    (SUCCESS != IVector_AddElement(pMe->m_pSendList, pItem)))
                {// 空号码无法回复
                    if (NULL != pItem)
                    {
                        sys_free(pItem);
                    }
                    
                    // 状态不发生迁移
                    return NFSMACTION_CONTINUE;
                }
                (void)WSTRCPY(pItem->m_szTo, pwstrNum);
                pMe->m_CurAddID = MSG_CMD_BASE;
                
                // 从电话本中取人名, 用于提示
                WMSUtil_GetContactName(pMe, pItem->m_szTo, pItem->m_szName, MAX_TITLE_LEN);
            }
            
            pMe->m_eCreateWMSType = SEND_MSG_RESEND;
#ifdef FEATURE_RTRE_DEFAULT_IS_NV
            MOVE_TO_STATE(WMSST_SENDING)
#else            
            // 检查卡是否插入modi by yangdecai 2010-08-10
		    if (IRUIM_IsCardConnected(pMe->m_pIRUIM)) 
		    {
            	MOVE_TO_STATE(WMSST_SENDING)
		    }
			else
			{
				MOVE_TO_STATE(WMSST_POPMSG)
			}
#endif            
            return NFSMACTION_CONTINUE;

            
        default:
            // 用退出程序代替宏断言
            MOVE_TO_STATE(WMSST_EXIT)
            return NFSMACTION_CONTINUE;
    }
} // WMSST_OUTMSGOPTS_Handler

/*==============================================================================
函数:
    WMSST_DELETEMSGS_Handler

说明:
    WMSST_DELETEMSGS 状态处理函数。

参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction WMSST_DELETEMSGS_Handler(WmsApp *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgReturn)
    {
        case DLGRET_MSGBOX_OK:
        case DLGRET_CREATE:
            WmsApp_ShowDialog(pMe, IDD_DELETEMSGS);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
            MOVE_TO_STATE(WMSST_MANAGEMENT)
            return NFSMACTION_CONTINUE;
            
        case DLGRET_CLEARINBOXES:
        case DLGRET_CLEAROUTBOX:
        case DLGRET_CLEARDRAFT:
            {
                uint16 wMsgs = 0;
                
                // 获取消息数
                if (pMe->m_eDlgReturn == DLGRET_CLEARINBOXES)
                {
                    wms_cacheinfolist_getcounts(WMS_MB_INBOX, NULL, NULL, &wMsgs);
                }
                else if (pMe->m_eDlgReturn == DLGRET_CLEAROUTBOX)
                {
                    wms_cacheinfolist_getcounts(WMS_MB_OUTBOX, NULL, NULL, &wMsgs);
                }
                else
                {
                    wms_cacheinfolist_getcounts(WMS_MB_DRAFT, NULL, NULL, &wMsgs);
                }

                if (wMsgs==0)
                {
                    pMe->m_ePMsgType = MESSAGE_WARNNING;
                    WmsApp_ShowMsgBox(pMe, IDS_EMPTY);
                    return NFSMACTION_WAIT;
                }
            }
            pMe->m_STSwitchToEditMsg = WMSST_MANAGEMENT;
            MOVE_TO_STATE(WMSST_DELMSGCONFIRM)
            return NFSMACTION_CONTINUE;
            
#ifdef FEATURE_CDSMS_RUIM
        case DLGRET_CLEARRUIM:
            if (pMe->m_memruimStatuse.used_tag_slots == 0)
            {
                pMe->m_ePMsgType = MESSAGE_WARNNING;
                WmsApp_ShowMsgBox(pMe, IDS_EMPTY);
                return NFSMACTION_WAIT;
            }
            pMe->m_STSwitchToEditMsg = WMSST_MANAGEMENT;
            MOVE_TO_STATE(WMSST_DELMSGCONFIRM)
            return NFSMACTION_CONTINUE;
#endif

        case DLGRET_CLEARPHONE:
            {
                uint16 wMsgs = 0;
                
                wms_cacheinfolist_getcounts(WMS_MB_INBOX, NULL, NULL, &wMsgs);
#ifdef FEATURE_CDSMS_RUIM
                wMsgs -= pMe->m_memruimStatuse.used_tag_slots;
#endif
                if (wMsgs == 0)
                {
                    pMe->m_ePMsgType = MESSAGE_WARNNING;
                    WmsApp_ShowMsgBox(pMe, IDS_EMPTY);
                    return NFSMACTION_WAIT;
                }
            }
            pMe->m_STSwitchToEditMsg = WMSST_MANAGEMENT;
            MOVE_TO_STATE(WMSST_DELMSGCONFIRM)
            return NFSMACTION_CONTINUE;

        case DLGRET_DELETEREADMSG:
            {
                uint16 wMsgs = 0;
                
                // 获取消息数
                wms_cacheinfolist_getcounts(WMS_MB_INBOX, NULL, NULL, &wMsgs);
                
                if (wMsgs==0)
                {
                    pMe->m_ePMsgType = MESSAGE_WARNNING;
                    WmsApp_ShowMsgBox(pMe, IDS_EMPTY);
                    return NFSMACTION_WAIT;
                }
            }
            pMe->m_eEraseWMSType = ERASE_READMSG;
            MOVE_TO_STATE(WMSST_DELMSGCONFIRM)
            return NFSMACTION_CONTINUE;

        case DLGRET_CLEARRESERVED:
            {
                uint16 wMsgs = 0;

                //获取消息数
                wms_cacheinfolist_getcounts(WMS_MB_RESERVE, NULL, NULL, &wMsgs);
                if(wMsgs == 0)
                {
                    pMe->m_ePMsgType = MESSAGE_WARNNING;
                    WmsApp_ShowMsgBox(pMe, IDS_EMPTY);
                    return NFSMACTION_WAIT;
                }
            }
            pMe->m_eEraseWMSType = CLEAR_ALLRESERVED;
            MOVE_TO_STATE(WMSST_DELMSGCONFIRM)
            return NFSMACTION_CONTINUE;
            
        //删除全部信息
        case DLGRET_CLEARALL:
            {
                uint16  nInMsgs=0;
                uint16  nOutMsgs=0;
                uint16  nDraftMsgs=0;
                uint16  nVoice=0;
                uint16  nReserveMsgs=0;
                
                // 获取消息数
                wms_cacheinfolist_getcounts(WMS_MB_INBOX, NULL, NULL, &nInMsgs);
                wms_cacheinfolist_getcounts(WMS_MB_OUTBOX, NULL, NULL, &nOutMsgs);
                wms_cacheinfolist_getcounts(WMS_MB_DRAFT, NULL, NULL, &nDraftMsgs);
                wms_cacheinfolist_getcounts(WMS_MB_VOICEMAIL, NULL, NULL, &nVoice);
                wms_cacheinfolist_getcounts(WMS_MB_RESERVE, NULL, NULL, &nReserveMsgs);
                if ((nInMsgs+nOutMsgs+nDraftMsgs+nVoice+nReserveMsgs) == 0)
                {
                    pMe->m_ePMsgType = MESSAGE_WARNNING;
                    WmsApp_ShowMsgBox(pMe, IDS_EMPTY);
                    return NFSMACTION_WAIT;
                }
            }
            pMe->m_STSwitchToEditMsg = WMSST_MANAGEMENT;
            MOVE_TO_STATE(WMSST_DELMSGCONFIRM)
            return NFSMACTION_CONTINUE;

        default:
            // 用退出程序代替宏断言
            MOVE_TO_STATE(WMSST_EXIT)
            break;
    }
    
    return NFSMACTION_CONTINUE;
} // WMSST_DELETEMSGS_Handler

/*==============================================================================
函数:
    WMSST_DELMSGCONFIRM_Handler

说明:
    WMSST_DELMSGCONFIRM 状态处理函数。

参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction WMSST_DELMSGCONFIRM_Handler(WmsApp *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgReturn)
    {
        case DLGRET_CREATE:
            pMe->m_wMsgResID = IDS_DELETE;
            WmsApp_ShowDialog(pMe, IDD_CONFIRM);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
            MSG_FATAL("WMSST_DELMSGCONFIRM_Handler pMe->m_eMBoxType=%d",pMe->m_eMBoxType,0,0);
            switch(pMe->m_eMBoxType)
            {
                case WMS_MB_INBOX:
                    if(pMe->m_eOptType == OPT_VIA_VIEWMSG)
                    {
                        MOVE_TO_STATE(WMSST_VIEWINBOXMSG)
                    }
                    else
                    {
                        MOVE_TO_STATE(WMSST_INBOXES)                        
                    }
                    break;
                    
                case WMS_MB_OUTBOX:
                    if(pMe->m_eOptType == OPT_VIA_VIEWMSG)
                    {
                        MOVE_TO_STATE(WMSST_VIEWOUTBOXMSG)
                    }
                    else
                    {
                        MOVE_TO_STATE(WMSST_OUTBOX)
                    }
                    break;
                    
                case WMS_MB_DRAFT:
                    if(pMe->m_eOptType == OPT_VIA_VIEWMSG)
                    {
                        MOVE_TO_STATE(WMSST_VIEWDRAFTMSG)
                    }
                    else
                    {
                        MOVE_TO_STATE(WMSST_DRAFT)
                    }
                    break;
                    
                case WMS_MB_TEMPLATE:
                    if(pMe->m_eOptType == OPT_VIA_VIEWMSG)
                    {
                        MOVE_TO_STATE(WMSST_VIEWTEMPLATE)
                    }
                    else
                    {
                        MOVE_TO_STATE(WMSST_TEMPLATES)
                    }
                    break;

                case WMS_MB_RESERVE:
                    if (pMe->m_eOptType == OPT_VIA_VIEWMSG)
                    {
                        MOVE_TO_STATE(WMSST_VIEWRESERVEDMSG)
                    }
                    else
                    {
                        MOVE_TO_STATE(WMSST_RESERVEDMSGS)
                    }
                    break;

                case WMS_MB_RSVFAILED:
                    if(pMe->m_eOptType == OPT_VIA_VIEWMSG)
                    {
                        MOVE_TO_STATE(WMSST_VIEWRESERVEDMSG)
                    }
                    else
                    {
                        MOVE_TO_STATE(WMSST_RESERVEDMSGS)
                    }
                    break;
#ifdef FEATURE_USES_MMS
                case WMS_MB_OUTBOX_MMS:
                    if(pMe->m_eOptType == OPT_VIA_VIEWMSG)
                    {
                        MOVE_TO_STATE(WMSST_VIEWOUTBOXMSG_MMS)
                    }
                    else
                    {
                        MOVE_TO_STATE(WMSST_OUTBOX_MMS)
                    }
                    break;
                case WMS_MB_INBOX_MMS:
                    if(pMe->m_eOptType == OPT_VIA_VIEWMSG)
                    {
                        MOVE_TO_STATE(WMSST_VIEWINBOXMSG_MMS)
                    }
                    else
                    {
                        MOVE_TO_STATE(WMSST_INBOX_MMS)
                    }
                    break;
                case WMS_MB_DRAFTBOX_MMS:
                    if(pMe->m_eOptType == OPT_VIA_VIEWMSG)
                    {
                        MSG_FATAL("WMSST_DELMSGCONFIRM_Handler WMSST_VIEWDRAFTBOXMSG_MMS",0,0,0);
                        MOVE_TO_STATE(WMSST_VIEWDRAFTBOXMSG_MMS)
                    }
                    else
                    {
                        MSG_FATAL("WMSST_DELMSGCONFIRM_Handler WMSST_DRAFTBOX_MMS",0,0,0);
                        MOVE_TO_STATE(WMSST_DRAFTBOX_MMS)
                    }
                    break;                    
#endif
                default:
                    MOVE_TO_STATE(pMe->m_prevState)
            }
            return NFSMACTION_CONTINUE;
    
        case DLGRET_OK:
            WmsApp_ShowDialog(pMe, IDD_DELETING);
            return NFSMACTION_WAIT;

        case DLGRET_DONE:
            switch(pMe->m_eEraseWMSType)
            {
#ifdef FEATURE_USES_MMS
                case ERASE_INBOX_MMS_ONE:
                { 
                    if (g_mmsDataInfoMax == 0)
                    {
                        MOVE_TO_STATE(WMSST_MAIN)
                    }
                    else
                    {
                        MOVE_TO_STATE(WMSST_INBOX_MMS)
                    }
                }
                break;
                
                case ERASE_OUTBOX_MMS_ONE:
                { 
                    if (g_mmsDataInfoMax == 0)
                    {
                        MOVE_TO_STATE(WMSST_MAIN)
                    }
                    else
                    {
                        MOVE_TO_STATE(WMSST_OUTBOX_MMS)
                    }
                }
                break;
                
                case CLEAR_OUTBOX_MMS:
                case CLEAR_INBOX_MMS:
                { 
                        MOVE_TO_STATE(WMSST_MAIN)
                }
                break;
                
#endif
                case ERASE_DRAFT_ONE: 
                    {
                        uint16  nMsgs;
                        
                        // 获取消息数
                        wms_cacheinfolist_getcounts(WMS_MB_DRAFT, NULL, NULL, &nMsgs);
                
                        if (nMsgs == 0)
                        {
                            MOVE_TO_STATE(WMSST_MAIN)
                        }
                        else
                        {
                            MOVE_TO_STATE(WMSST_DRAFT)
                        }
                    }
                    break;
                
                case ERASE_INBOXES_ONE: 
                    {
                        uint16  nMsgs;
                        
                        // 获取消息数
                        wms_cacheinfolist_getcounts(WMS_MB_INBOX, NULL, NULL, &nMsgs);
                
                        if (nMsgs == 0)
                        {
                            MOVE_TO_STATE(WMSST_MAIN)
                        }
                        else
                        {
                            MOVE_TO_STATE(WMSST_INBOXES)
                        }
                    }
                    break;
 
                case ERASE_OUTBOX_ONE:
                    {
                        uint16  nMsgs;
                        
                        // 获取消息数
                        wms_cacheinfolist_getcounts(WMS_MB_OUTBOX, NULL, NULL, &nMsgs);
                
                        if (nMsgs == 0)
                        {
                            MOVE_TO_STATE(WMSST_MAIN)
                        }
                        else
                        {
                            MOVE_TO_STATE(WMSST_OUTBOX)
                        }
                    }
                    break;
                    
                case ERASE_RESERVED_ONE:
                    {
                        uint16 nNews=0, nMsgs=0, nAlert = 0;
                        
                        // 获取消息数
                        wms_cacheinfolist_getcounts(WMS_MB_RESERVE, &nNews, NULL, &nMsgs);
                        
                        if (pMe->m_eMBoxType == WMS_MB_RESERVE)
                        {
                            WmsApp_ReservedMsgSetTimer(pMe);
                            nAlert = nNews;
                        }
                        else
                        {
                            nAlert = nMsgs-nNews;
                        }
                
                        if (nAlert == 0)
                        {
                            MOVE_TO_STATE(WMSST_RESERVEDMSG)
                        }
                        else
                        {
                            MOVE_TO_STATE(WMSST_RESERVEDMSGS)
                        }
                    }
                    break;
                    
                case ERASE_VOICEMAIL_ONE:
                    MOVE_TO_STATE(WMSST_VOICEMAIL)
                    break;
                    
#ifdef FEATURE_CDSMS_RUIM                	
                case CLEAR_RUIM:
#endif
                case CLEAR_PHONE:
                case CLEAR_INBOXES:
                case CLEAR_OUTBOX:
                case CLEAR_DRAFT:
                //case ERASE_READMSG:
                case CLEAR_ALL:
                    MOVE_TO_STATE(pMe->m_STSwitchToEditMsg)
                    break;
                    
                case CLEAR_RESERVED:
                case CLEAR_RESERVEFAILED:
                    if (pMe->m_eEraseWMSType == CLEAR_RESERVED)
                    {
                        WmsApp_ReservedMsgSetTimer(pMe);
                    }
                    
                    if(pMe->m_prevState == WMSST_DELETEMSGS)
                    {
                        MOVE_TO_STATE(WMSST_DELETEMSGS)
                    }
                    else
                    {
                        MOVE_TO_STATE(WMSST_RESERVEDMSG)
                    }
                    break;

                case CLEAR_ALLRESERVED:
                    WmsApp_ReservedMsgSetTimer(pMe);
                    MOVE_TO_STATE(WMSST_DELETEMSGS)
                    break;
                    
                case ERASE_TEMPLATE_ONE: 
                    #if defined(FEATURE_VERSION_HITZ181)||defined(FEATURE_VERSION_MTM)
            		pMe->m_wCurTemplate = IDS_TEMPLATEHITZ0;
            		#else
            		pMe->m_wCurTemplate = IDS_TEMPLATE1;
            		#endif
                    MOVE_TO_STATE(WMSST_TEMPLATES)
                    break;
                    
                default:
                    MOVE_TO_STATE(WMSST_EXIT)
                    break;
            }
            return NFSMACTION_CONTINUE;

        default:
            // 用退出程序代替宏断言
            MOVE_TO_STATE(WMSST_EXIT)
            return NFSMACTION_CONTINUE;
    }
            
    return NFSMACTION_CONTINUE;
} // WMSST_DELMSGCONFIRM_Handler

/*==============================================================================
函数:
    WMSST_MSGSETTING_Handler

说明:
    WMSST_MSGSETTING 状态处理函数。
    
参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction WMSST_MSGSETTING_Handler(WmsApp *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgReturn)
    {
        case DLGRET_CREATE:
            WmsApp_ShowDialog(pMe, IDD_SETTING);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
            MOVE_TO_STATE(WMSST_MAIN)
            return NFSMACTION_CONTINUE;
            
        case DLGRET_SETTINGAUTOSAVE:
            MOVE_TO_STATE(WMSST_AUTOSAVE)
            return NFSMACTION_CONTINUE;
            
        case DLGRET_PRIORITY:
            MOVE_TO_STATE(WMSST_PRIORITY)
            return NFSMACTION_CONTINUE;
            
        case DLGRET_REPORTS:
            MOVE_TO_STATE(WMSST_REPORTS)
            return NFSMACTION_CONTINUE;
            
#ifdef FEATURE_AUTOREPLACE
        case DLGRET_AUTOREPLACE:
            MOVE_TO_STATE(WMSST_AUTOREPLACE)
            return NFSMACTION_CONTINUE;
#endif            
            
        case DLGRET_SENDMODE:
            MOVE_TO_STATE(WMSST_SENDMODE)
            return NFSMACTION_CONTINUE;
#ifdef FEATURE_USES_MMS  

        case DLGRET_MMSNOTIFY:
            MOVE_TO_STATE(WMSST_MMSNOTIFY)
            return NFSMACTION_CONTINUE;

       case DLGRET_MMSDELIVERYREPORT:
            MOVE_TO_STATE(WMSST_MMSDELIVERYREPORT)
            return NFSMACTION_CONTINUE;

       case DLGRET_READREPLY:
            MOVE_TO_STATE(WMSST_READREPLY)
            return NFSMACTION_CONTINUE;

       case DLGRET_REPORTALLOWED:
            MOVE_TO_STATE(WMSST_REPORTALLOWED)
            return NFSMACTION_CONTINUE;

       case DLGRET_SENDERVISIBILITY:
            MOVE_TO_STATE(WMSST_SENDERVISIBILITY)
            return NFSMACTION_CONTINUE;

        case DLGRET_MMS_SERVER_ADDRESS:
            MOVE_TO_STATE(WMSST_MMS_SERVER_ADDRESS)
            return NFSMACTION_CONTINUE;     

        case DLGRET_MMS_PROXY:
            MOVE_TO_STATE(WMSST_MMS_PROXY)
            return NFSMACTION_CONTINUE;    

        case DLGRET_MMS_PORT :
            MOVE_TO_STATE(WMSST_MMS_PORT)
            return NFSMACTION_CONTINUE;                
#endif
        case DLGRET_CALLBACKNUM:
            MOVE_TO_STATE(WMSST_CALLBACKNUMSWITCH)
            return NFSMACTION_CONTINUE;           

        case DLGRET_MSGVALIDITY:
            MOVE_TO_STATE(WMSST_MSGVALIDITY)
            return NFSMACTION_CONTINUE;

		//Add By zzg 2012_02_04
		case DLGRET_TIME_STAMP:
            MOVE_TO_STATE(WMSST_TIME_STAMP)
            return NFSMACTION_CONTINUE;
		//Add End
            
        case DLGRET_AUTODELETE:
            MOVE_TO_STATE(WMSST_AUTODELETE)
            return NFSMACTION_CONTINUE;
            
        case DLGRET_RESERVEDMSGALERTTIMEOUT:
            MOVE_TO_STATE(WMSST_RESERVEDMSGALERTTIMEOUT)
            return NFSMACTION_CONTINUE; 
        
        default:
            // 用退出程序代替宏断言
            MOVE_TO_STATE(WMSST_EXIT)
            return NFSMACTION_CONTINUE;
    }
} // WMSST_MSGSETTING_Handler

/*==============================================================================
函数:
    WMSST_RESERVEDMSG_Handler

说明:
    WMSST_RESERVEDMSG 状态处理函数。
    
参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction WMSST_RESERVEDMSG_Handler(WmsApp *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgReturn)
    {
        case DLGRET_CREATE:
        case DLGRET_MSGBOX_OK:
            WmsApp_ShowDialog(pMe, IDD_RESERVEDMSG);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
            MOVE_TO_STATE(WMSST_MAIN)
            return NFSMACTION_CONTINUE;
            
        case DLGRET_NEWRESERVEDMSG:
            pMe->m_bTextFullAlert = FALSE;
            
            // 清空群发地址链表
            WmsApp_FreeMultiSendList(pMe->m_pSendList);
            pMe->m_eCreateWMSType = SEND_MSG_RESERVE;
            
            pMe->m_STSwitchToEditMsg = pMe->m_currState;
            MEMSET(&pMe->m_rsvDateTime, 0, sizeof(pMe->m_rsvDateTime));
            // 结构数据清零
            WMSMessageStruct_Reset(&pMe->m_msSend);
            
            pMe->m_dwInsertPos = 0;
            MOVE_TO_STATE(WMSST_WRITEMSG)  
            return NFSMACTION_CONTINUE;
            
        case DLGRET_RESERVEDMSGLST:
        case DLGRET_FAILEDRSVDMSGS:
            {
                uint16 nNews=0, nMsgs=0, nAlert = 0;
                
                // 获取消息数
                wms_cacheinfolist_getcounts(WMS_MB_RESERVE, &nNews, NULL, &nMsgs);
                if (pMe->m_eDlgReturn == DLGRET_RESERVEDMSGLST)
                {
                    pMe->m_eMBoxType = WMS_MB_RESERVE;
                    nAlert = nNews;
                }
                else
                {
                    pMe->m_eMBoxType = WMS_MB_RSVFAILED;
                    nAlert = nMsgs-nNews;
                }
                
                if (nAlert == 0)
                {
                    pMe->m_ePMsgType = MESSAGE_WARNNING;
                    WmsApp_ShowMsgBox(pMe, IDS_EMPTY);
                    return NFSMACTION_WAIT;
                }
            }
            
            pMe->m_eMakeListMode = MAKEMSGLIST_INIT;
            MOVE_TO_STATE(WMSST_RESERVEDMSGS)  
            return NFSMACTION_CONTINUE;
            
        default:
            // 用退出程序代替宏断言
            MOVE_TO_STATE(WMSST_EXIT)
            return NFSMACTION_CONTINUE;
    }
} // WMSST_RESERVEDMSG_Handler

/*==============================================================================
函数:
    WMSST_RSVDMSGTIME_Handler

说明:
    WMSST_RSVDMSGTIME 状态处理函数。
    
参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction WMSST_RSVDMSGTIME_Handler(WmsApp *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgReturn)
    {
        case DLGRET_CREATE:
            WmsApp_ShowDialog(pMe, IDD_GETDT);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
            MOVE_TO_STATE(WMSST_TONUMLIST)          
            return NFSMACTION_CONTINUE;
                        
        case DLGRET_YEARERR:
            pMe->m_ePMsgType = MESSAGE_INFORMATION;
            WmsApp_ShowMsgBox(pMe, IDS_YEARERR);
            return NFSMACTION_WAIT;
            
        case DLGRET_DATEERR:
            pMe->m_ePMsgType = MESSAGE_WARNNING;
            WmsApp_ShowMsgBox(pMe, IDS_TIMEERR);
            return NFSMACTION_WAIT;
            
        case DLGRET_OK:
            // 保存预约短信
            if (IVector_Size(pMe->m_pSendList) > 0)
            {
                int i, nRet;
                int32  nItems = 0;
                uint16  nMsgs=0, nMax=RESERVE_MAX;
                CMultiSendItemInfo *pItem = NULL;
                wms_client_message_s_type *pClientMsg = NULL;
                wms_cache_info_node  *pnode = NULL;
                
                // 打包消息
                WmsApp_PrepareUserDataMOList(pMe);
                pMe->m_idxUserdata = 0;
                
                if (pMe->m_eCreateWMSType == SEND_MSG_EDITRESERVE)
                {// 当前是编辑已存储预约短信
                    for (i=0; i<LONGSMS_MAX_PACKAGES; i++)
                    {
                        if (pMe->m_CurMsgNodes[i] != NULL)
                        {
                            nMax++;
                        }
                    }
                }
                
                // 消息需要保存，这里先检查存储空间，存储空间不足则提示用户，用户更改选项后再继续!
                // 获取消息数
                wms_cacheinfolist_getcounts(WMS_MB_RESERVE, &nMsgs, NULL, NULL);
                nItems = IVector_Size(pMe->m_pUserDataMOList);
                if ((nItems+nMsgs)>(nMax - MAX_RSVFAILD))
                {
                    pMe->m_ePMsgType = MESSAGE_WARNNING;
                    WmsApp_ShowMsgBox(pMe, IDS_STOREINSUFFICIENCY);
                    return NFSMACTION_WAIT;
                }
                
                if (pMe->m_eCreateWMSType == SEND_MSG_EDITRESERVE)
                {// 当前是编辑已存储预约短信,先删除
                    for (i=0; i<LONGSMS_MAX_PACKAGES; i++)
                    {
                        if (pMe->m_CurMsgNodes[i] != NULL)
                        {
                            pnode = pMe->m_CurMsgNodes[i];
                            
                            // 发布删除消息命令
                            nRet = ENOMEMORY;
                            do
                            {
                                nRet = IWMS_MsgDelete(pMe->m_pwms,
                                                   pMe->m_clientId,
                                                   &pMe->m_callback,
                                                   (void *)pMe,
                                                   pnode->mem_store,
                                                   pnode->index);
                            } while(nRet != SUCCESS);
#ifndef WIN32
                            // 休眠10毫秒以确保有时间执行保存消息的操作
                            MSLEEP(10);
#endif                                        
                            pMe->m_CurMsgNodes[i] = NULL;
                        }
                    }
                }
                
                pItem = (CMultiSendItemInfo *)IVector_ElementAt(pMe->m_pSendList, 0);
                
                if (NULL != pItem)
                {
                    // 拷贝当前号码
                    pMe->m_msSend.m_szNum[0] = 0;
                    (void)WSTRCPY(pMe->m_msSend.m_szNum, pItem->m_szTo);
                }

                pClientMsg = WmsApp_GetClientMsgMO(pMe, TRUE);
                
                while (NULL != pClientMsg)
                {
                    nRet = ENOMEMORY;
                    
                    do
                    {
                        nRet = IWMS_MsgWrite(pMe->m_pwms, 
                                             pMe->m_clientId, 
                                             &pMe->m_callback,
                                             (void*)pMe,
                                             WMS_WRITE_MODE_INSERT,
                                             pClientMsg);
                    } while(nRet != SUCCESS);
#ifndef WIN32
                    // 休眠10毫秒以确保有时间执行保存消息的操作
                    MSLEEP(10);
#endif                                        
                    FREE(pClientMsg);
                    pClientMsg = WmsApp_GetClientMsgMO(pMe, TRUE);
                }
                pMe->m_ePMsgType = MESSAGE_INFORMATIVE;
                WmsApp_ShowMsgBox(pMe, IDS_SAVED);
                return NFSMACTION_WAIT;
            }
            else
            {// 这只是防弹措施，程序不应执行到此
                MOVE_TO_STATE(WMSST_EXIT)
            }
            
            return NFSMACTION_CONTINUE;
            
        case DLGRET_MSGBOX_OK:
            switch (pMe->m_wMsgResID)
            {
                case IDS_SAVED:
                    pMe->m_eMBoxType = WMS_MB_RESERVE;
                    pMe->m_eMakeListMode = MAKEMSGLIST_INIT;
                    MOVE_TO_STATE(WMSST_RESERVEDMSGS)
                    WmsApp_ReservedMsgSetTimer(pMe);
                    break;
                    
                default:   
                    WmsApp_ShowDialog(pMe, IDD_GETDT);
                    return NFSMACTION_WAIT;
            }
            return NFSMACTION_CONTINUE;
        
        default:
            // 用退出程序代替宏断言
            MOVE_TO_STATE(WMSST_EXIT)
            return NFSMACTION_CONTINUE;
    }
} // WMSST_RSVDMSGTIME_Handler

/*==============================================================================
函数:
    WMSST_RESERVEDMSGS_Handler

说明:
    WMSST_RESERVEDMSGS 状态处理函数。

参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction WMSST_RESERVEDMSGS_Handler(WmsApp *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgReturn)
    {
        case DLGRET_CREATE:
        case DLGRET_LOADCANCELED:
        case DLGRET_LOADFAILED:
            WmsApp_ShowDialog(pMe, IDD_MESSAGELIST);
            return NFSMACTION_WAIT;
            
        case DLGRET_LOAD:
            pMe->m_eOptType = OPT_VIA_VIEWMSG;
            WmsApp_ShowDialog(pMe, IDD_LOADINGMSG);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
            MOVE_TO_STATE(WMSST_RESERVEDMSG)
            return NFSMACTION_CONTINUE;

        case DLGRET_LOADOK:
            if (pMe->m_eOptType == OPT_VIA_VIEWMSG)
            {
                MOVE_TO_STATE(WMSST_VIEWRESERVEDMSG)
            }
            else
            {
                MOVE_TO_STATE(WMSST_RESERVEDMSGOPT)
            }
            return NFSMACTION_CONTINUE;
            
        case DLGRET_OPT:
            pMe->m_eOptType = OPT_VIA_LISTMSG;
            WmsApp_ShowDialog(pMe, IDD_LOADINGMSG);
            return NFSMACTION_WAIT;
            
        default:
            // 用退出程序代替宏断言
            MOVE_TO_STATE(WMSST_EXIT)
            return NFSMACTION_CONTINUE;
    }
} // WMSST_RESERVEDMSGS_Handler

/*==============================================================================
函数:
    WMSST_VIEWRESERVEDMSG_Handler

说明:
    WMSST_VIEWRESERVEDMSG 状态处理函数。

参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction WMSST_VIEWRESERVEDMSG_Handler(WmsApp *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgReturn)
    {
        case DLGRET_CREATE:
            WmsApp_ShowDialog(pMe, IDD_VIEWMSG);
           
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
            MOVE_TO_STATE(WMSST_RESERVEDMSGS)
            return NFSMACTION_CONTINUE;

        case DLGRET_OK:
            pMe->m_eOptType = OPT_VIA_VIEWMSG;
            MOVE_TO_STATE(WMSST_RESERVEDMSGOPT)
            return NFSMACTION_CONTINUE;

        case DLGRET_INFO:
            pMe->m_eOptType = OPT_VIA_VIEWMSG;
            pMe->m_eDlgReturn = DLGRET_EDIT;
            pMe->m_bDoNotOverwriteDlgResult = TRUE;
            MOVE_TO_STATE(WMSST_RESERVEDMSGOPT)
            return NFSMACTION_CONTINUE;
            
        default:
            // 用退出程序代替宏断言
            MOVE_TO_STATE(WMSST_EXIT)
            return NFSMACTION_CONTINUE;
    }
} // WMSST_VIEWRESERVEDMSG_Handler

/*==============================================================================
函数:
    WMSST_RESERVEDMSGOPT_Handler

说明:
    WMSST_RESERVEDMSGOPT 状态处理函数。

参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction WMSST_RESERVEDMSGOPT_Handler(WmsApp *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgReturn)
    {
        case DLGRET_CREATE:
            WmsApp_ShowDialog(pMe, IDD_MSGOPTS);
            return NFSMACTION_WAIT;

        case DLGRET_VIEW:
            MOVE_TO_STATE(WMSST_VIEWRESERVEDMSG)
            return NFSMACTION_CONTINUE;
        
        case DLGRET_CANCELED:
            if (pMe->m_eOptType == OPT_VIA_VIEWMSG)
            {
                MOVE_TO_STATE(WMSST_VIEWRESERVEDMSG)
            }
            else
            {
                MOVE_TO_STATE(WMSST_RESERVEDMSGS)
            }
            return NFSMACTION_CONTINUE;

        case DLGRET_EDIT:
            // 拷贝消息
            WMSMessageStruct_Reset(&pMe->m_msSend);
            MEMCPY(&pMe->m_msSend, &pMe->m_msCur, sizeof(pMe->m_msSend));
            if (NULL != pMe->m_msCur.m_szMessage)
            {
                pMe->m_msSend.m_szMessage = WSTRDUP(pMe->m_msCur.m_szMessage);
            }
            
            // 准备当前号码
            {
                CMultiSendItemInfo *pItem = NULL;
                
                // 先清空群发地址链表
                WmsApp_FreeMultiSendList(pMe->m_pSendList);
                
                if (WSTRLEN(pMe->m_msCur.m_szNum)>0)
                {
                    pItem = (CMultiSendItemInfo *)sys_malloc(sizeof(CMultiSendItemInfo));
                    DBGPRINTF("WMS Address = 0x%x", pItem);  
                    if ((NULL != pItem) &&
                        (SUCCESS != IVector_AddElement(pMe->m_pSendList, pItem)))
                    {
                        sys_free(pItem);
                        pItem = NULL;
                    }
                    
                    if (NULL != pItem)
                    {
                        (void)WSTRCPY(pItem->m_szTo, pMe->m_msCur.m_szNum);
                        pMe->m_CurAddID = MSG_CMD_BASE;
                        
                        // 从电话本中取人名, 用于提示
                        WMSUtil_GetContactName(pMe, pItem->m_szTo, pItem->m_szName, MAX_TITLE_LEN);
                    }
                }
            }
            
            MEMSET(&pMe->m_rsvDateTime, 0, sizeof(pMe->m_rsvDateTime));
            pMe->m_rsvDateTime.dwSecs = pMe->m_msCur.m_dwTime;
            
            pMe->m_STSwitchToEditMsg = pMe->m_currState;
            pMe->m_eCreateWMSType = SEND_MSG_EDITRESERVE;
            MOVE_TO_STATE(WMSST_WRITEMSG)
            pMe->m_dwInsertPos = 0;
            return NFSMACTION_CONTINUE;

        case DLGRET_SEND:
        case DLGRET_RESEND:
            pMe->m_idxCur = 0;
            
            {// 对于重发操作不进入消息编辑界面，也不进入地址列表输入界面
                AECHAR  *pwstrNum = NULL;
                CMultiSendItemInfo *pItem = NULL;
                
                // 先清空群发地址链表
                WmsApp_FreeMultiSendList(pMe->m_pSendList);
                
                // 拷贝地址 
                if (WSTRLEN(pMe->m_msCur.m_szNum)>0)
                {
                    pwstrNum = pMe->m_msCur.m_szNum;
                }
                else
                {
                    // 状态不发生迁移
                    return NFSMACTION_CONTINUE;
                }

                //对于预约短信修改时间戳
                if(pMe->m_eDlgReturn == DLGRET_SEND)
                {
                    uint32 dwTime;

					/*
					byte	btTimeStamp = 0;
				    
					(void) ICONFIG_GetItem(pMe->m_pConfig,
				                           CFGI_SMS_TIMESTAMP,
				                           &btTimeStamp,
				                           sizeof(btTimeStamp));

					if (btTimeStamp == OEMNV_SMS_TIMESTAMP_ADJUST)
					{
						dwTime = GETUTCSECONDS();
						MSG_FATAL("***zzg GETUTCSECONDS 10 dwSecs=%d***", dwTime, 0, 0);
					}
					else
					{
						dwTime = GETTIMESECONDS();
						MSG_FATAL("***zzg GETTIMESECONDS 10 dwSecs=%d***", dwTime, 0, 0);
					}  
					*/

					dwTime = GETTIMESECONDS();
					
                    pMe->m_msCur.m_dwTime = dwTime;
                }
                
                pItem = (CMultiSendItemInfo *)sys_malloc(sizeof(CMultiSendItemInfo));
                DBGPRINTF("WMS Address = 0x%x", pItem);      
                // 将回复号码保存入链表
                if ((pItem == NULL) || 
                    (SUCCESS != IVector_AddElement(pMe->m_pSendList, pItem)))
                {// 空号码无法回复
                    if (NULL != pItem)
                    {
                        sys_free(pItem);
                    }
                    
                    // 状态不发生迁移
                    return NFSMACTION_CONTINUE;
                }
                (void)WSTRCPY(pItem->m_szTo, pwstrNum);
                pMe->m_CurAddID = MSG_CMD_BASE;
                
                // 从电话本中取人名, 用于提示
                WMSUtil_GetContactName(pMe, pItem->m_szTo, pItem->m_szName, MAX_TITLE_LEN);
            }
            
            pMe->m_eCreateWMSType = SEND_MSG_RESERVE;
#ifdef FEATURE_RTRE_DEFAULT_IS_NV
            MOVE_TO_STATE(WMSST_SENDING)
#else            
            // 检查卡是否插入modi by yangdecai 2010-08-10
		    if (IRUIM_IsCardConnected(pMe->m_pIRUIM)) 
		    {
            	MOVE_TO_STATE(WMSST_SENDING)
		    }
			else
			{
				MOVE_TO_STATE(WMSST_POPMSG)
			}
#endif            
            return NFSMACTION_CONTINUE;  
            
        case DLGRET_DELETE:
            pMe->m_eEraseWMSType = ERASE_RESERVED_ONE;
            MOVE_TO_STATE(WMSST_DELMSGCONFIRM)
            return NFSMACTION_CONTINUE;

        case DLGRET_CLEARALL:
            if (pMe->m_eMBoxType == WMS_MB_RESERVE)
            {
                pMe->m_eEraseWMSType = CLEAR_RESERVED;
            }
            else
            {
                pMe->m_eEraseWMSType = CLEAR_RESERVEFAILED;
            }
            MOVE_TO_STATE(WMSST_DELMSGCONFIRM)
            return NFSMACTION_CONTINUE;
            
        default:
            // 用退出程序代替宏断言
            MOVE_TO_STATE(WMSST_EXIT)
            return NFSMACTION_CONTINUE;
    }
} // WMSST_RESERVEDMSGOPT_Handler

/*==============================================================================
函数:
    WMSST_RESERVEDMSGALERT_Handler

说明:
    WMSST_RESERVEDMSGALERT 状态处理函数。
    
参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction WMSST_RESERVEDMSGALERT_Handler(WmsApp *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgReturn)
    {
        case DLGRET_CREATE:
            WmsApp_ShowDialog(pMe, IDD_RESERVEDMSGALERT);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
            WmsApp_ReservedMsgSetTimer(pMe);
            if (pMe->m_bNaturalStart)
            {
                MOVE_TO_STATE(WMSST_MAIN)
            }
            else
            {
                MOVE_TO_STATE(WMSST_EXIT)
            }
            return NFSMACTION_CONTINUE;
            
        case DLGRET_OK:
            // 重置当前消息列表
            MEMSET(pMe->m_CurMsgNodes, 0, sizeof(pMe->m_CurMsgNodes));
            WmsApp_FreeMsgNodeMs(pMe);
            
            {// 对于预约短信发送操作既不进入消息编辑界面，也不进入地址列表输入界面
                CMultiSendItemInfo *pItem = NULL;
                wms_cache_info_node  *pnode = NULL;
                
                // 先清空群发地址链表
                WmsApp_FreeMultiSendList(pMe->m_pSendList);
                
                wms_cacheinfolist_enumbegin(WMS_MB_RESERVE);
                pnode = wms_cacheinfolist_enumnext(WMS_MB_RESERVE);
                
                if (NULL == pnode)
                {
                    // 容错处理
                    MOVE_TO_STATE(WMSST_EXIT)
                    return NFSMACTION_CONTINUE;
                }
                else
                {
                    pMe->m_idxCur = 0;
#ifdef FEATURE_SMS_UDH
                    if (pnode->pItems != NULL)
                    {
                        MEMCPY(pMe->m_CurMsgNodes, pnode->pItems, sizeof(pMe->m_CurMsgNodes));
                    }
                    else
#endif
                    {
                        pMe->m_CurMsgNodes[0] = pnode;
                    }
                }
                
                // 拷贝地址 
                if ((NULL != pnode->pszNum) && (STRLEN(pnode->pszNum)>0))
                {
                    pItem = (CMultiSendItemInfo *)sys_malloc(sizeof(CMultiSendItemInfo));
                    DBGPRINTF("WMS Address = 0x%x", pItem);  
                    // 将回复号码保存入链表
                    if ((pItem == NULL) || 
                        (SUCCESS != IVector_AddElement(pMe->m_pSendList, pItem)))
                    {
                        // 容错处理
                        MOVE_TO_STATE(WMSST_EXIT)
                        
                        if (NULL !=pItem)
                        {
                            sys_free(pItem);
                        }
                        
                        WmsApp_ReservedMsgSetTimer(pMe);
                        return NFSMACTION_CONTINUE;
                    }
                    (void)STRTOWSTR(pnode->pszNum, pItem->m_szTo, sizeof(pItem->m_szTo));
                    pMe->m_CurAddID = MSG_CMD_BASE;
                    
                    // 从电话本中取人名, 用于提示
                    WMSUtil_GetContactName(pMe, pItem->m_szTo, pItem->m_szName, MAX_TITLE_LEN);
                }
                else
                {
                    // 容错处理
                    MOVE_TO_STATE(WMSST_EXIT)
                    
                    WmsApp_ReservedMsgSetTimer(pMe);
                    return NFSMACTION_CONTINUE;
                }
            }
            
            pMe->m_eCreateWMSType = SEND_MSG_RESERVE;
#ifdef FEATURE_RTRE_DEFAULT_IS_NV
            MOVE_TO_STATE(WMSST_SENDING)
#else            
            // 状态迁至发送界面
            // 检查卡是否插入modi by yangdecai 2010-08-10
		    if (IRUIM_IsCardConnected(pMe->m_pIRUIM)) 
		    {
            	MOVE_TO_STATE(WMSST_SENDING)
		    }
			else
			{
				MOVE_TO_STATE(WMSST_POPMSG)
			}
#endif            
            return NFSMACTION_CONTINUE;
            
        default:
            // 用退出程序代替宏断言
            MOVE_TO_STATE(WMSST_EXIT)
            return NFSMACTION_CONTINUE;
    }
} // WMSST_RESERVEDMSGALERT_Handler

/*==============================================================================
函数:
    WMSST_AUTOSAVE_Handler

说明:
    WMSST_AUTOSAVE 状态处理函数。
    
参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction WMSST_AUTOSAVE_Handler(WmsApp *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgReturn)
    {
        case DLGRET_CREATE:
            WmsApp_ShowDialog(pMe, IDD_OUTMSGSAVEOPT);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
        case DLGRET_MSGBOX_OK:
            MOVE_TO_STATE(WMSST_MSGSETTING)
            return NFSMACTION_CONTINUE;
            
        case DLGRET_OK:
            pMe->m_ePMsgType = MESSAGE_INFORMATIVE;
            WmsApp_ShowMsgBox(pMe, IDS_SAVED);
            return NFSMACTION_WAIT;
            
        default:
            // 用退出程序代替宏断言
            MOVE_TO_STATE(WMSST_EXIT)
            return NFSMACTION_CONTINUE;
    }
} // WMSST_AUTOSAVE_Handler

/*==============================================================================
函数:
    WMSST_PRIORITY_Handler

说明:
    WMSST_PRIORITY 状态处理函数。
    
参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction WMSST_PRIORITY_Handler(WmsApp *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgReturn)
    {
        case DLGRET_CREATE:
            WmsApp_ShowDialog(pMe, IDD_PRIORITY);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
        case DLGRET_MSGBOX_OK:
            MOVE_TO_STATE(WMSST_MSGSETTING)
            return NFSMACTION_CONTINUE;
            
        case DLGRET_OK:
            pMe->m_ePMsgType = MESSAGE_INFORMATIVE;
            WmsApp_ShowMsgBox(pMe, IDS_SAVED);
            return NFSMACTION_WAIT;
            
        default:
            // 用退出程序代替宏断言
            MOVE_TO_STATE(WMSST_EXIT)
            return NFSMACTION_CONTINUE;
    }
} // WMSST_PRIORITY_Handler

/*==============================================================================
函数:
    WMSST_SENDMODE_Handler

说明:
    WMSST_SENDMODE 状态处理函数。
    
参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction WMSST_SENDMODE_Handler(WmsApp *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgReturn)
    {
        case DLGRET_CREATE:
            WmsApp_ShowDialog(pMe, IDD_SENDMODE);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
        case DLGRET_MSGBOX_OK:
            MOVE_TO_STATE(WMSST_MSGSETTING)
            return NFSMACTION_CONTINUE;
            
        case DLGRET_OK:
            pMe->m_ePMsgType = MESSAGE_INFORMATIVE;
            WmsApp_ShowMsgBox(pMe, IDS_SAVED);
            return NFSMACTION_WAIT;
            
        default:
            // 用退出程序代替宏断言
            MOVE_TO_STATE(WMSST_EXIT)
            return NFSMACTION_CONTINUE;
    }
} // WMSST_SENDMODE_Handler
#ifdef FEATURE_USES_MMS  
/*==============================================================================
函数:
    WMSST_MMSNOTIFY_Handler

说明:
    WMSST_MMSNOTIFY 状态处理函数。
    
参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction WMSST_MMSOPTION_YESNO_Handler(WmsApp *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgReturn)
    {
        case DLGRET_CREATE:
            WmsApp_ShowDialog(pMe, IDD_MMSOPTION_YESNO);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
        case DLGRET_MSGBOX_OK:
            MOVE_TO_STATE(WMSST_MSGSETTING)
            return NFSMACTION_CONTINUE;
            
        case DLGRET_OK:
            pMe->m_ePMsgType = MESSAGE_INFORMATIVE;
            WmsApp_ShowMsgBox(pMe, IDS_SAVED);
            return NFSMACTION_WAIT;
            
        default:
            // 用退出程序代替宏断言
            MOVE_TO_STATE(WMSST_EXIT)
            return NFSMACTION_CONTINUE;
    }
} // WMSST_SENDMODE_Handler

#endif
/*==============================================================================
函数:
    WMSST_REPORTS_Handler

说明:
    WMSST_REPORTS 状态处理函数。
    
参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction WMSST_REPORTS_Handler(WmsApp *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgReturn)
    {
        case DLGRET_CREATE:
            WmsApp_ShowDialog(pMe, IDD_REPORTS);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
        case DLGRET_MSGBOX_OK:
            MOVE_TO_STATE(WMSST_MSGSETTING)
            return NFSMACTION_CONTINUE;
            
        case DLGRET_OK:
            pMe->m_ePMsgType = MESSAGE_INFORMATIVE;
            WmsApp_ShowMsgBox(pMe, IDS_SAVED);
            return NFSMACTION_WAIT;
            
        default:
            // 用退出程序代替宏断言
            MOVE_TO_STATE(WMSST_EXIT)
            return NFSMACTION_CONTINUE;
    }
} // WMSST_REPORTS_Handler

#ifdef FEATURE_AUTOREPLACE
/*==============================================================================
函数:
    WMSST_AUTOREPLACE_Handler

说明:
    WMSST_AUTOREPLACE 状态处理函数。
    
参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction WMSST_AUTOREPLACE_Handler(WmsApp *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgReturn)
    {
        case DLGRET_CREATE:
            WmsApp_ShowDialog(pMe, IDD_AUTOREPLACE);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
        case DLGRET_MSGBOX_OK:
            MOVE_TO_STATE(WMSST_MSGSETTING)
            return NFSMACTION_CONTINUE;
            
        case DLGRET_OK:
            pMe->m_ePMsgType = MESSAGE_INFORMATIVE;
            WmsApp_ShowMsgBox(pMe, IDS_SAVED);
            return NFSMACTION_WAIT;
            
        default:
            // 用退出程序代替宏断言
            MOVE_TO_STATE(WMSST_EXIT)
            return NFSMACTION_CONTINUE;
    }
} // WMSST_AUTOREPLACE_Handler
#endif /* FEATURE_AUTOREPLACE */

/*==============================================================================
函数:
    WMSST_CALLBACKNUMSWITCH_Handler

说明:
    WMSST_CALLBACKNUMSWITCH 状态处理函数。
    
参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction WMSST_CALLBACKNUMSWITCH_Handler(WmsApp *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgReturn)
    {
        case DLGRET_CREATE:
            WmsApp_ShowDialog(pMe, IDD_CALLBACKNUMSWITCH);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
        case DLGRET_MSGBOX_OK:
            MOVE_TO_STATE(WMSST_MSGSETTING)
            return NFSMACTION_CONTINUE;
            
        case DLGRET_OK:
            pMe->m_ePMsgType = MESSAGE_INFORMATIVE;
            WmsApp_ShowMsgBox(pMe, IDS_SAVED);
            return NFSMACTION_WAIT;
            
        case DLGRET_CALLBACKNUM:
            MOVE_TO_STATE(WMSST_CALLBACKNUMSET)
            return NFSMACTION_CONTINUE;
            
        default:
            // 用退出程序代替宏断言
            MOVE_TO_STATE(WMSST_EXIT)
            return NFSMACTION_CONTINUE;
    }
} // WMSST_CALLBACKNUMSWITCH_Handler

/*==============================================================================
函数:
    WMSST_CALLBACKNUMSET_Handler

说明:
    WMSST_CALLBACKNUMSET 状态处理函数。
    
参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction WMSST_CALLBACKNUMSET_Handler(WmsApp *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgReturn)
    {
        case DLGRET_CREATE:
            WmsApp_ShowDialog(pMe, IDD_CALLBACKNUM);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
            MOVE_TO_STATE(WMSST_MSGSETTING)
            return NFSMACTION_CONTINUE;
            
        case DLGRET_MSGBOX_OK:
            MOVE_TO_STATE(WMSST_MSGSETTING)
            return NFSMACTION_CONTINUE;
            
        case DLGRET_OK:
            pMe->m_ePMsgType = MESSAGE_INFORMATIVE;
            WmsApp_ShowMsgBox(pMe, IDS_SAVED);
            return NFSMACTION_WAIT;
            
        default:
            // 用退出程序代替宏断言
            MOVE_TO_STATE(WMSST_EXIT)
            return NFSMACTION_CONTINUE;
    }
} // WMSST_CALLBACKNUMSET_Handler

/*==============================================================================
函数:
    WMSST_AUTODELETE_Handler

说明:
    WMSST_AUTODELETE 状态处理函数。
    
参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction WMSST_AUTODELETE_Handler(WmsApp *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgReturn)
    {
        case DLGRET_CREATE:
            WmsApp_ShowDialog(pMe, IDD_AUTODELETE);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
        case DLGRET_MSGBOX_OK:
            MOVE_TO_STATE(WMSST_MSGSETTING)
            return NFSMACTION_CONTINUE;
            
        case DLGRET_OK:
            pMe->m_ePMsgType = MESSAGE_INFORMATIVE;
            WmsApp_ShowMsgBox(pMe, IDS_SAVED);
            return NFSMACTION_WAIT;
            
        default:
            // 用退出程序代替宏断言
            MOVE_TO_STATE(WMSST_EXIT)
            return NFSMACTION_CONTINUE;
    }
} // WMSST_AUTODELETE_Handler

/*==============================================================================
函数:
    WMSST_RESERVEDMSGALERTTIMEOUT_Handler

说明:
    WMSST_RESERVEDMSGALERTTIMEOUT 状态处理函数。
    
参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction WMSST_RESERVEDMSGALERTTIMEOUT_Handler(WmsApp *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgReturn)
    {
        case DLGRET_CREATE:
            WmsApp_ShowDialog(pMe, IDD_RSVDMSGALERTTIMEOUT_SETTING);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
        case DLGRET_MSGBOX_OK:
            MOVE_TO_STATE(WMSST_MSGSETTING)
            return NFSMACTION_CONTINUE;
            
        case DLGRET_OK:
            pMe->m_ePMsgType = MESSAGE_INFORMATIVE;
            WmsApp_ShowMsgBox(pMe, IDS_SAVED);
            return NFSMACTION_WAIT;
            
        default:
            // 用退出程序代替宏断言
            MOVE_TO_STATE(WMSST_EXIT)
            return NFSMACTION_CONTINUE;
    }
} // WMSST_RESERVEDMSGALERTTIMEOUT_Handler

#ifdef FEATURE_CDSMS_RUIM
/*==============================================================================
函数:
    WMSST_STORAGE_Handler

说明:
    WMSST_STORAGE 状态处理函数。
    
参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction WMSST_STORAGE_Handler(WmsApp *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgReturn)
    {
        case DLGRET_CREATE:
            WmsApp_ShowDialog(pMe, IDD_STORAGE);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
        case DLGRET_MSGBOX_OK:
            MOVE_TO_STATE(WMSST_MANAGEMENT)
            return NFSMACTION_CONTINUE;
            
        case DLGRET_OK:
            pMe->m_ePMsgType = MESSAGE_INFORMATIVE;
            WmsApp_ShowMsgBox(pMe, IDS_SAVED);
            return NFSMACTION_WAIT;
            
        default:
            // 用退出程序代替宏断言
            MOVE_TO_STATE(WMSST_EXIT)
            return NFSMACTION_CONTINUE;
    }
} // WMSST_STORAGE_Handler
#endif

/*==============================================================================
函数:
    WMSST_MSGVALIDITY_Handler

说明:
    WMSST_MSGVALIDITY 状态处理函数。
    
参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction WMSST_MSGVALIDITY_Handler(WmsApp *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgReturn)
    {
        case DLGRET_CREATE:
            WmsApp_ShowDialog(pMe, IDD_MESSAGEVALIDITY);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
        case DLGRET_MSGBOX_OK:
            MOVE_TO_STATE(WMSST_MSGSETTING)
            return NFSMACTION_CONTINUE;
            
        case DLGRET_OK:
            pMe->m_ePMsgType = MESSAGE_INFORMATIVE;
            WmsApp_ShowMsgBox(pMe, IDS_SAVED);
            return NFSMACTION_WAIT;
            
        default:
            // 用退出程序代替宏断言
            MOVE_TO_STATE(WMSST_EXIT)
            return NFSMACTION_CONTINUE;
    }
} // WMSST_MSGVALIDITY_Handler

//Add By zzg 2012_02_04
/*==============================================================================
函数:
    WMSST_TIME_STAMP_Handler

说明:
    WMSST_TIME_STAMP 状态处理函数。
    
参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction WMSST_TIME_STAMP_Handler(WmsApp *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgReturn)
    {
        case DLGRET_CREATE:
            WmsApp_ShowDialog(pMe, IDD_TIME_STAMP);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
        case DLGRET_MSGBOX_OK:
            MOVE_TO_STATE(WMSST_MSGSETTING)
            return NFSMACTION_CONTINUE;
            
        case DLGRET_OK:
            pMe->m_ePMsgType = MESSAGE_INFORMATIVE;
            WmsApp_ShowMsgBox(pMe, IDS_SAVED);
            return NFSMACTION_WAIT;
            
        default:
            // 用退出程序代替宏断言
            MOVE_TO_STATE(WMSST_EXIT)
            return NFSMACTION_CONTINUE;
    }
} 
//Add End

/*==============================================================================
函数:
    WMSST_VOICEMAILOPTS_Handler

说明:
 
    WMSST_VOICEMAILOPTS 状态处理函数。

参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction WMSST_VOICEMAILOPTS_Handler(WmsApp *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgReturn)
    {
        case DLGRET_CREATE:
            WmsApp_ShowDialog(pMe, IDD_MSGOPTS);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
            MOVE_TO_STATE(WMSST_VOICEMAIL)
            return NFSMACTION_CONTINUE;

        case DLGRET_DELETE:
            pMe->m_eEraseWMSType = ERASE_VOICEMAIL_ONE;
            MOVE_TO_STATE(WMSST_DELMSGCONFIRM)
            return NFSMACTION_CONTINUE;
            
        default:
            // 用退出程序代替宏断言
            MOVE_TO_STATE(WMSST_EXIT)
            return NFSMACTION_CONTINUE;
    }
} // WMSST_VOICEMAILOPTS_Handler


#ifdef FEATURE_CONTACT_APP
/*==============================================================================
函数:
    WMSST_EXTARCTDETAILS_Handler

说明:
    WMSST_EXTARCTDETAILS 状态处理函数。

参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction WMSST_EXTARCTDETAILS_Handler(WmsApp *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgReturn)
    {
        case DLGRET_MSGBOX_OK:
        case DLGRET_CREATE:                     
            WmsApp_ShowDialog(pMe, IDD_EXTARCTDETAILS);
            return NFSMACTION_WAIT;
            
        case DLGRET_CANCELED:
            MOVE_TO_STATE(WMSST_VIEWINBOXMSG)//WMSST_INBOXMSGOPTS
            return NFSMACTION_CONTINUE;

        case DLGRET_SAVENUM:
            pMe->m_ExtractType = EXTRACT_NUM;
            WmsApp_FreeSaveNumList(pMe);
            
            // 分析消息建立号码链表
            WmsApp_BuildNumList(pMe, &pMe->m_msCur);
            
            if (IVector_Size(pMe->m_pSaveNumList) < 1)
            {
                pMe->m_ePMsgType = MESSAGE_WARNNING;
                WmsApp_ShowMsgBox(pMe, IDS_NOVALIDNUM);
                return NFSMACTION_WAIT;
            }
            MOVE_TO_STATE(WMSST_EXTARCTEDITEMLIST)
            return NFSMACTION_CONTINUE;
            
        case DLGRET_SAVEEMAIL:
            pMe->m_ExtractType = EXTRACT_EMAILADD;
            WmsApp_FreeSaveNumList(pMe);
            
            // 分析消息建立电邮地址链表
            WmsApp_BuildMailAddList(pMe, pMe->m_msCur.m_szMessage);
            
            if (IVector_Size(pMe->m_pSaveNumList) < 1)
            {
                pMe->m_ePMsgType = MESSAGE_WARNNING;
                WmsApp_ShowMsgBox(pMe, IDS_NOVALIDMAILADD);
                return NFSMACTION_WAIT;
            }
            MOVE_TO_STATE(WMSST_EXTARCTEDITEMLIST)
            return NFSMACTION_CONTINUE;
#ifdef FEATURE_USES_MMS
        case DLGRET_GETMMS:
        {
            WMSMessageStruct_Free(pMe);
			//ADD BY YANGDECAI 2010-08-16
            pMe->m_ExtractType = EXTRACT_URL;
            WmsApp_FreeSaveNumList(pMe);
            
            // 分析消息建立URL表
            WmsApp_BuildUrlList(pMe, pMe->m_msCur.m_szMessage);
            
            if (IVector_Size(pMe->m_pSaveNumList) < 1)
            {
                pMe->m_ePMsgType = MESSAGE_WARNNING;
                WmsApp_ShowMsgBox(pMe, IDS_NOVALIDURL);
                return NFSMACTION_WAIT;
            }

            {
                char str[100] = {0};
                MSG_FATAL("WMSST_INBOXMSGOPTS_Handler",0,0,0);
                MSG_FATAL("WMSST_EXTARCTDETAILS_Handler DLGRET_GETMMS=%S",(AECHAR*)IVector_ElementAt(pMe->m_pSaveNumList,0),0,0);
                WSTRTOSTR((AECHAR*)IVector_ElementAt(pMe->m_pSaveNumList,0),
                    (char*)&str,
                    sizeof(str));
                MSG_FATAL("WMSST_EXTARCTDETAILS_Handler DLGRET_GETMMS=%s",&str,0,0);
                {
                    uint16  nMsgID = 0;
                    IFileMgr *pIFileMgr = NULL;
                    int result = 0;
                    uint32 pdwTotal = 0;
                    uint32 pdwFree = 0;
                    result = ISHELL_CreateInstance(AEE_GetShell(), AEECLSID_FILEMGR,(void **)&pIFileMgr);
                    if (SUCCESS != result)
                    {
                        MSG_FATAL("WMSST_EXTARCTDETAILS_Handler: Open file error %x", result,0,0);
                        return FALSE;
                    }            
                    pdwFree = IFILEMGR_GetFreeSpace(pIFileMgr, &pdwTotal); 
                    IFILEMGR_Release(pIFileMgr);    
                    MSG_FATAL("WMSST_EXTARCTDETAILS_Handler pdwFree=%d, pdwTotal=%d",pdwFree, pdwTotal, 0);
                    if((pdwFree < MSG_MAX_PACKET_SIZE+50))
                    {
                        nMsgID = IDS_PHONEFULL;
                        pMe->m_ePMsgType = MESSAGE_WARNNING;
                        WmsApp_ShowMsgBox(pMe, nMsgID);
                        return NFSMACTION_WAIT;
                    }
                }                  
                if(!WMS_MMSState(WMS_MMS_PDU_WSPHTTPGETreq,0,(uint32)str))
                {
                    pMe->m_GetStatus = HTTP_CODE_Bad_Request;
    		        ISHELL_SetTimer(pMe->m_pShell,1000,(PFNNOTIFY)&WmsApp_ProcessMMSStatus,pMe);
		        }
                IVector_RemoveElementAt(pMe->m_pSaveNumList,0);   
            }
            MOVE_TO_STATE(WMSST_GETTING)
            return NFSMACTION_CONTINUE;

        }
        break;
#endif            
        case DLGRET_SAVEURL:
            pMe->m_ExtractType = EXTRACT_URL;
            WmsApp_FreeSaveNumList(pMe);
            
            // 分析消息建立URL表
            WmsApp_BuildUrlList(pMe, pMe->m_msCur.m_szMessage);
            
            if (IVector_Size(pMe->m_pSaveNumList) < 1)
            {
                pMe->m_ePMsgType = MESSAGE_WARNNING;
                WmsApp_ShowMsgBox(pMe, IDS_NOVALIDURL);
                return NFSMACTION_WAIT;
            }
            MOVE_TO_STATE(WMSST_EXTARCTEDITEMLIST)
            return NFSMACTION_CONTINUE;
            
        default:
            // 用退出程序代替宏断言
            MOVE_TO_STATE(WMSST_EXIT)
            return NFSMACTION_CONTINUE;
    }
} // WMSST_EXTARCTDETAILS_Handler

/*==============================================================================
函数:
    WMSST_EXTARCTEDITEMLIST_Handler

说明:
    WMSST_EXTARCTEDITEMLIST 状态处理函数。

参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction WMSST_EXTARCTEDITEMLIST_Handler(WmsApp *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgReturn)
    {
        case DLGRET_CREATE:                     
            WmsApp_ShowDialog(pMe, IDD_EXTARCTEDITEMLIST);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
            //MOVE_TO_STATE(WMSST_EXTARCTDETAILS)
            MOVE_TO_STATE(WMSST_VIEWINBOXMSG)
            return NFSMACTION_CONTINUE;

        case DLGRET_OK:
            MOVE_TO_STATE(WMSST_EXTARCTEDITEMOPT)
            return NFSMACTION_CONTINUE;
            
        default:
            // 用退出程序代替宏断言
            MOVE_TO_STATE(WMSST_EXIT)
            return NFSMACTION_CONTINUE;
    }
} // WMSST_EXTARCTEDITEMLIST_Handler

/*==============================================================================
函数:
    WMSST_EXTARCTEDITEMOPT_Handler

说明:
    WMSST_EXTARCTEDITEMOPT 状态处理函数。

参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction WMSST_EXTARCTEDITEMOPT_Handler(WmsApp *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgReturn)
    {
        case DLGRET_CREATE:                     
            WmsApp_ShowDialog(pMe, IDD_EXTARCTEDITEM_OPT);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
            MOVE_TO_STATE(WMSST_EXTARCTEDITEMLIST)
            return NFSMACTION_CONTINUE;

        case DLGRET_WRITEMSG:
            {
                CMultiSendItemInfo *pItem = NULL;
                AECHAR    *pExtractItem = NULL;

                pExtractItem = (AECHAR *)IVector_ElementAt(pMe->m_pSaveNumList, pMe->m_CurExtractItem);
                
                // 先清空群发地址链表
                WmsApp_FreeMultiSendList(pMe->m_pSendList);
                
                // 将接收方号码保存入链表
                pItem = (CMultiSendItemInfo *)sys_malloc(sizeof(CMultiSendItemInfo));
                DBGPRINTF("WMS Address = 0x%x", pItem);  
                if (pItem == NULL)
                {
                    MOVE_TO_STATE(WMSST_VIEWINBOXMSG)
                    return NFSMACTION_CONTINUE;
                }
                (void)WSTRCPY(pItem->m_szTo, pExtractItem);
                pMe->m_CurAddID = MSG_CMD_BASE;
                if (SUCCESS != IVector_AddElement(pMe->m_pSendList, pItem))
                {
                    sys_free(pItem);
                    MOVE_TO_STATE(WMSST_VIEWINBOXMSG)
                    return NFSMACTION_CONTINUE;
                }
                
                WMSUtil_GetContactName(pMe, pItem->m_szTo, pItem->m_szName, MAX_TITLE_LEN);
            }
            
            pMe->m_STSwitchToEditMsg = WMSST_EXTARCTEDITEMLIST;//WMSST_EXTARCTEDITEMOPT;
            pMe->m_eCreateWMSType = SEND_MSG_REPLY;
            pMe->m_dwInsertPos = 0;
            MOVE_TO_STATE(WMSST_WRITEMSG)
            return NFSMACTION_CONTINUE;
            
        default:
            // 用退出程序代替宏断言
            MOVE_TO_STATE(WMSST_EXIT)
            return NFSMACTION_CONTINUE;
    }
} // WMSST_EXTARCTEDITEMOPT_Handler
#endif

/*==============================================================================
函数:
    WMSST_MEMSTATUS_Handler

说明:
    WMSST_MEMSTATUS 状态处理函数。

参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction WMSST_MEMSTATUS_Handler(WmsApp *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgReturn)
    {
        case DLGRET_CREATE:
            WmsApp_ShowDialog(pMe, IDD_MEMSTATUS);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:              
            MOVE_TO_STATE(WMSST_MANAGEMENT)   
            return NFSMACTION_CONTINUE;

        default:
            // 用退出程序代替宏断言
            MOVE_TO_STATE(WMSST_EXIT)
            return NFSMACTION_CONTINUE;
    }
} // WMSST_MEMSTATUS_Handler

/*==============================================================================
函数:
    WMSST_CONTINUESEND_QUERY_Handler

说明:
    WMSST_CONTINUESEND_QUERY 状态处理函数。

参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction WMSST_CONTINUESEND_QUERY_Handler(WmsApp *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgReturn)
    {
        case DLGRET_CREATE:
            pMe->m_wMsgResID = IDS_CONTINUESENDQUERY;
            WmsApp_ShowDialog(pMe, IDD_CONFIRM);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
            if (pMe->m_stcontinuesendbk == WMSST_INBOXES)
            {
                pMe->m_currState = WMSST_MAIN;
                pMe->m_eDlgReturn = DLGRET_INBOXES;
            }
            else
            {
                pMe->m_currState = pMe->m_stcontinuesendbk;
            }
            pMe->m_stcontinuesendbk = WMSST_NONE;
            pMe->m_bNeedContinueSend = FALSE;
            pMe->m_ContinueSendType = NONE_CONTINUE;
                
            return NFSMACTION_CONTINUE;

        case DLGRET_OK:
            pMe->m_bNeedContinueSend = FALSE;
#ifdef FEATURE_RTRE_DEFAULT_IS_NV
            MOVE_TO_STATE(WMSST_SENDING)
#else            
            // 检查卡是否插入modi by yangdecai 2010-08-10
		    if (IRUIM_IsCardConnected(pMe->m_pIRUIM)) 
		    {
            	MOVE_TO_STATE(WMSST_SENDING)
		    }
			else
			{
				MOVE_TO_STATE(WMSST_POPMSG)
			}
#endif            
            return NFSMACTION_CONTINUE;
            
        default:
            // 用退出程序代替宏断言
            MOVE_TO_STATE(WMSST_EXIT)
            return NFSMACTION_CONTINUE;
    }
} // WMSST_CONTINUESEND_QUERY_Handler

/*==============================================================================
函数:
    WMSST_MANAGEMENT_Handler

说明:
    WMSST_MANAGEMENT 状态处理函数。

参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction WMSST_MANAGEMENT_Handler(WmsApp *pMe)
{
    boolean  bsmslock = FALSE;    
    (void) ICONFIG_GetItem(pMe->m_pConfig,
                           CFGI_SMS_LOCK_CHECK,
                           &bsmslock,
                           sizeof(bsmslock));
    bsmslock = FALSE;
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgReturn)
    {
        case DLGRET_CREATE:
            WmsApp_ShowDialog(pMe, IDD_MANAGEMENT);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
            MOVE_TO_STATE(WMSST_MAIN)    
            return NFSMACTION_CONTINUE;
            
        case DLGRET_DELETEMSGS:
            {
                uint16  nInMsgs=0;
                uint16  nOutMsgs=0;
                uint16  nDraftMsgs=0;
                uint16  nReserveMsgs=0;
                
                // 获取消息数
                wms_cacheinfolist_getcounts(WMS_MB_INBOX, NULL, NULL, &nInMsgs);
                wms_cacheinfolist_getcounts(WMS_MB_OUTBOX, NULL, NULL, &nOutMsgs);
                wms_cacheinfolist_getcounts(WMS_MB_DRAFT, NULL, NULL, &nDraftMsgs);
                wms_cacheinfolist_getcounts(WMS_MB_RESERVE, NULL, NULL, &nReserveMsgs);

                if ((nInMsgs+nOutMsgs+nDraftMsgs+nReserveMsgs)==0)
                {
                    pMe->m_ePMsgType = MESSAGE_WARNNING;
                    WmsApp_ShowMsgBox(pMe, IDS_EMPTY);
                    return NFSMACTION_WAIT;
                }
            }
                
            MOVE_TO_STATE(WMSST_DELETEMSGS)
            if (bsmslock)
            {
                pMe->m_stchkpwdbk = pMe->m_currState;
                pMe->m_currState = WMSST_CHKPWD;
            }
            return NFSMACTION_CONTINUE; 

        case DLGRET_MSGBOX_OK:
            MOVE_TO_STATE(WMSST_MANAGEMENT)
            return NFSMACTION_CONTINUE;
            
        case DLGRET_COPYINBOX:          
            MOVE_TO_STATE(WMSST_COPYINBOX)
            return NFSMACTION_CONTINUE;
            
        case DLGRET_MOVEINBOX:
            MOVE_TO_STATE(WMSST_MOVEINBOX)
            return NFSMACTION_CONTINUE;

#ifdef FEATURE_CDSMS_RUIM
        case DLGRET_STORAGE:
            MOVE_TO_STATE(WMSST_STORAGE)
            return NFSMACTION_CONTINUE;
#endif             

#ifdef FEATURE_USES_MMS
        case DLGRET_MMS_MEMSTATUS:
            MOVE_TO_STATE(WMSST_MMS_MEMSTATUS)
            return NFSMACTION_CONTINUE;
#endif
        case DLGRET_MEMSTATUS:
            MOVE_TO_STATE(WMSST_MEMSTATUS)
            return NFSMACTION_CONTINUE;
            
        default:
            // 用退出程序代替宏断言
            MOVE_TO_STATE(WMSST_EXIT)
            return NFSMACTION_CONTINUE;
    }
} // WMSST_MANAGEMENT_Handler

/*==============================================================================
函数:
    WMSST_COPYINBOX_Handler

说明:
    WMSST_COPYINBOX 状态处理函数。

参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction WMSST_COPYINBOX_Handler(WmsApp *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgReturn)
    {
        case DLGRET_CREATE:
        case DLGRET_MSGBOX_OK:
            WmsApp_ShowDialog(pMe, IDD_COPYINBOX);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
            MOVE_TO_STATE(WMSST_MANAGEMENT)    
            return NFSMACTION_CONTINUE;  

        case DLGRET_COPYINBOXMSGTORUIM:
            {
                uint16 wMsgs = 0;
                uint16 wOnUIM = 0;
                uint16 wOnPH = 0;
                
                // 获取消息数
                wms_cacheinfolist_getcounts(WMS_MB_INBOX, NULL, &wOnUIM, &wMsgs);
                wOnPH = wMsgs-wOnUIM;
                wMsgs = pMe->m_memruimStatuse.max_slots - pMe->m_memruimStatuse.used_tag_slots;

                if(wOnPH > wMsgs)
                {
                    pMe->m_nCopyNum = wMsgs;
                }
                else
                {
                    pMe->m_nCopyNum = wOnPH;
                }

                if (wOnPH == 0)
                {// 收件箱中话机上消息数为 0
                    pMe->m_ePMsgType = MESSAGE_WARNNING;
                    WmsApp_ShowMsgBox(pMe, IDS_EMPTY);
                    return NFSMACTION_WAIT;
                }
                else if (pMe->m_nCopyNum == 0)//(wOnPH > wMsgs)
                {// 卡可用空间不足
                    pMe->m_ePMsgType = MESSAGE_WARNNING;
                    WmsApp_ShowMsgBox(pMe, IDS_INSUFFICIENCY);
                    return NFSMACTION_WAIT;
                }
            }
            MOVE_TO_STATE(WMSST_MSGCOPYCONFIRM)
            return NFSMACTION_CONTINUE;

        case DLGRET_COPYRUIMMSGTOINBOX:
            {
                uint16 wMsgs = 0;
                uint16 wOnUIM = 0;
                
                // 获取消息数
                wms_cacheinfolist_getcounts(WMS_MB_INBOX, NULL, &wOnUIM, &wMsgs);

                if(wMsgs > IN_MAX)
                {
                    pMe->m_nCopyNum = IN_MAX - (wMsgs - wOnUIM);
                }
                else
                {
                    pMe->m_nCopyNum = pMe->m_memruimStatuse.used_tag_slots;
                }

                if (wOnUIM == 0)
                {// 卡上消息数为 0
                    pMe->m_ePMsgType = MESSAGE_WARNNING;
                    WmsApp_ShowMsgBox(pMe, IDS_EMPTY);
                    return NFSMACTION_WAIT;
                }
                else if (pMe->m_nCopyNum == 0)//(wMsgs> IN_MAX)
                {// 话机收件箱可用空间不足
                    pMe->m_ePMsgType = MESSAGE_WARNNING;
                    WmsApp_ShowMsgBox(pMe, IDS_INSUFFICIENCY);
                    return NFSMACTION_WAIT;
                }
            }
            MOVE_TO_STATE(WMSST_MSGCOPYCONFIRM)
            return NFSMACTION_CONTINUE;            
            
        default:
            // 用退出程序代替宏断言
            MOVE_TO_STATE(WMSST_EXIT)
            return NFSMACTION_CONTINUE;
    }
} // WMSST_COPYINBOX_Handler

/*==============================================================================
函数:
    WMSST_MOVEINBOX_Handler

说明:
    WMSST_MOVEINBOX 状态处理函数。

参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction WMSST_MOVEINBOX_Handler(WmsApp *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgReturn)
    {
        case DLGRET_CREATE:
        case DLGRET_MSGBOX_OK:
            WmsApp_ShowDialog(pMe, IDD_MOVEINBOX);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
            MOVE_TO_STATE(WMSST_MANAGEMENT)    
            return NFSMACTION_CONTINUE;

        case DLGRET_MOVEINBOXMSGTORUIM:
            {
                uint16 wMsgs = 0;
                uint16 wOnUIM = 0;
                uint16 wOnPH = 0;
                
                // 获取消息数
                wms_cacheinfolist_getcounts(WMS_MB_INBOX, NULL, &wOnUIM, &wMsgs);
                wOnPH = wMsgs-wOnUIM;
                wMsgs = pMe->m_memruimStatuse.max_slots - pMe->m_memruimStatuse.used_tag_slots;
                if(wOnPH > wMsgs)
                {
                    pMe->m_nCopyNum = wMsgs;
                }
                else
                {
                    pMe->m_nCopyNum = wOnPH;
                }

                if (wOnPH == 0)
                {// 收件箱中话机上消息数为 0
                    pMe->m_ePMsgType = MESSAGE_WARNNING;
                    WmsApp_ShowMsgBox(pMe, IDS_EMPTY);
                    return NFSMACTION_WAIT;
                }
                else if (pMe->m_nCopyNum == 0)//(wOnPH > wMsgs)//目标空间为0时才不进行移动
                {// 卡可用空间不足
                    pMe->m_ePMsgType = MESSAGE_WARNNING;
                    WmsApp_ShowMsgBox(pMe, IDS_INSUFFICIENCY);
                    return NFSMACTION_WAIT;
                }
            }
            MOVE_TO_STATE(WMSST_MSGCOPYCONFIRM)
            return NFSMACTION_CONTINUE;

        case DLGRET_MOVERUIMMSGTOINBOX:
            {
                uint16 wMsgs = 0;
                uint16 wOnUIM = 0;
                
                // 获取消息数
                wms_cacheinfolist_getcounts(WMS_MB_INBOX, NULL, &wOnUIM, &wMsgs);

                if(wMsgs > IN_MAX)
                {
                    pMe->m_nCopyNum = IN_MAX - (wMsgs - wOnUIM);
                }
                else
                {
                    pMe->m_nCopyNum = pMe->m_memruimStatuse.used_tag_slots;
                }

                if (wOnUIM == 0)
                {// 卡上消息数为 0
                    pMe->m_ePMsgType = MESSAGE_WARNNING;
                    WmsApp_ShowMsgBox(pMe, IDS_EMPTY);
                    return NFSMACTION_WAIT;
                }
                else if (pMe->m_nCopyNum == 0)//(wMsgs> IN_MAX)
                {// 话机收件箱可用空间不足
                    pMe->m_ePMsgType = MESSAGE_WARNNING;
                    WmsApp_ShowMsgBox(pMe, IDS_INSUFFICIENCY);
                    return NFSMACTION_WAIT;
                }
            }
            MOVE_TO_STATE(WMSST_MSGCOPYCONFIRM)
            return NFSMACTION_CONTINUE;  
            
        default:
            // 用退出程序代替宏断言
            MOVE_TO_STATE(WMSST_EXIT)
            return NFSMACTION_CONTINUE;
    }
} // WMSST_MOVEINBOX_Handler


/*==============================================================================
函数:
    WMSST_SELECTFROM_Handler

说明:
    WMSST_SELECTFROM 状态处理函数。
    
参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction WMSST_SELECTFROM_Handler(WmsApp *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgReturn)
    {
        case DLGRET_CREATE:
            WmsApp_ShowDialog(pMe, IDD_SELECTFROMOPT);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
            MOVE_TO_STATE(WMSST_TONUMLIST);
            return NFSMACTION_CONTINUE;
          
        default:
            // 用退出程序代替宏断言
            MOVE_TO_STATE(WMSST_EXIT)
            return NFSMACTION_CONTINUE;
    }
} // WMSST_SELECTFROM_Handler
static NextFSMAction WMSST_RESENDCONFIRM_Handler(WmsApp *pMe)
{
	if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
	
	switch (pMe->m_eDlgReturn)
    {
    	case DLGRET_CREATE:
            pMe->m_wMsgResID = IDS_FAILED_RESENDING;
            WmsApp_ShowDialog(pMe, IDD_RESEND_CONFIRM);
            return NFSMACTION_WAIT;
		case DLGRET_CANCELED:
			MOVE_TO_STATE(WMSST_MAIN)
		    break;
		case DLGRET_OK:
			if(SEND_MSG_NEW == pMe->m_eCreateWMSType)
			{
				pMe->m_eCreateWMSType = SEND_MSG_RESEND;
				pMe->m_eDlgReturn = DLGRET_CREATE;
#ifdef FEATURE_RTRE_DEFAULT_IS_NV
                MOVE_TO_STATE(WMSST_SENDING)
#else				
				// 检查卡是否插入modi by yangdecai 2010-08-10
			    if (IRUIM_IsCardConnected(pMe->m_pIRUIM)) 
			    {
	            	MOVE_TO_STATE(WMSST_SENDING)
			    }
				else
				{
					MOVE_TO_STATE(WMSST_POPMSG)
				}
#endif                
			}
			else
			{
				pMe->m_eOptType = OPT_VIA_VIEWMSG;
            	pMe->m_eDlgReturn = DLGRET_RESEND;
				pMe->m_bDoNotOverwriteDlgResult = TRUE;
            	MOVE_TO_STATE(WMSST_OUTMSGOPTS)
			}
            return NFSMACTION_CONTINUE;
		
		default:
            // 用退出程序代替宏断言
            MOVE_TO_STATE(WMSST_EXIT)
            return NFSMACTION_CONTINUE;
    }
            
    return NFSMACTION_CONTINUE;
	
	
}  //WMSST_RESENDCONFIRM_Handler

//WMSST_WMSNEW状态处理函数add by yangdecai
static NextFSMAction WMSST_WMSNEW_Hander(WmsApp *pMe)
{
	if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    MSG_FATAL("WMSST_WMSNEW_Hander Start",0,0,0);
	switch (pMe->m_eDlgReturn)
    {
        case DLGRET_CREATE:
            WmsApp_ShowDialog(pMe, IDD_WMSNEWMSG);
            return NFSMACTION_WAIT;

        case DLGRET_SMSVIEWS:
            {// 调用短信接口进行查看操作
            	//modi by yangdecai 09-25
				boolean  bsmslock = FALSE;
                
    			MSG_FATAL("WMSST_WMSNEW_Hander %d %d %d",gbWmsVMailNtf,gbWmsSMSNtf,gbWmsLastNtfIsSMS);
    			(void) ICONFIG_GetItem(pMe->m_pConfig,
                           CFGI_SMS_LOCK_CHECK,
                           &bsmslock,
                           sizeof(bsmslock));
                
				if (bsmslock)
                {
                    pMe->m_currState = WMSST_CHKPWD;
                    if (gbWmsVMailNtf && !(gbWmsLastNtfIsSMS && gbWmsSMSNtf))
                    {
                        pMe->m_stchkpwdbk = WMSST_VOICEMAIL;
                        gbWmsVMailNtf = FALSE;
                    }
#ifdef FEATURE_USES_MMS  
                    else if(gbWmsMMSNtf)
                    {
                        pMe->m_stchkpwdbk = WMSST_INBOX_MMS;
                        gbWmsMMSNtf = FALSE;
                    }
#endif
                    else
                    {
                        pMe->m_stchkpwdbk = WMSST_INBOXES;
                        gbWmsSMSNtf = FALSE;
                    }
					pMe->m_eDlgReturn = DLGRET_CREATE;
					MOVE_TO_STATE(WMSST_CHKPWD)
            	}
				else
				{
            		pMe->m_eDlgReturn = DLGRET_CREATE;
                    if (gbWmsVMailNtf && !(gbWmsLastNtfIsSMS && gbWmsSMSNtf))
                    {
                        MOVE_TO_STATE(WMSST_VOICEMAIL)
                        gbWmsVMailNtf = FALSE;
                    }
#ifdef FEATURE_USES_MMS  
                    else if(gbWmsMMSNtf)
                    {
                        MOVE_TO_STATE(WMSST_INBOX_MMS)
                        gbWmsMMSNtf = FALSE;
                    }
#endif
                    else
                    {
                	    MOVE_TO_STATE(WMSST_INBOXES)
                        gbWmsSMSNtf = FALSE;
                    }
				}
            }
            return NFSMACTION_CONTINUE;

        default:
        case DLGGET_SMSNEW_OK:
            MSG_FATAL("WMSST_WMSNEW_Hander %d %d %d",gbWmsVMailNtf,gbWmsSMSNtf,gbWmsLastNtfIsSMS);
            if (gbWmsVMailNtf && !(gbWmsLastNtfIsSMS && gbWmsSMSNtf))
            {
                gbWmsVMailNtf = FALSE;
            }
#ifdef FEATURE_USES_MMS  
            else if(gbWmsMMSNtf)
            {
                gbWmsMMSNtf = FALSE;
            }
#endif
            else
            {
                gbWmsSMSNtf = FALSE;
            }
            MOVE_TO_STATE(WMSST_EXIT)
            return NFSMACTION_CONTINUE;
    }
    MSG_FATAL("WMSST_WMSNEW_Hander End",0,0,0);
    return NFSMACTION_WAIT;
}
//WMSST_FLASHSMS状态处理函数add by yangdecai
static NextFSMAction WMSST_FLASHSMS_Hander(WmsApp *pMe)
{
	if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    MSG_FATAL("WMSST_FLASHSMS_Hander Start",0,0,0);
	switch (pMe->m_eDlgReturn)
    {
        case DLGRET_CREATE:
            WmsApp_ShowDialog(pMe, IDD_FLASHSMS);
            return NFSMACTION_WAIT;

        case DLGGET_FLASHSMS_END:
			
            //MOVE_TO_STATE(COREST_STANDBY)
            return NFSMACTION_CONTINUE;
        
        default:
            break;
    }
    MSG_FATAL("COREST_SMSTIP_Handler End",0,0,0);
    return NFSMACTION_WAIT;
}

static NextFSMAction WMSST_WMSPOP_Hander(WmsApp * pMe)
{
	if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    MSG_FATAL("COREST_SMSTIP_Handler Start",0,0,0);
	switch (pMe->m_eDlgReturn)
    {
        case DLGRET_CREATE:
            WmsApp_ShowDialog(pMe, IDD_NOPOPMSG);
            return NFSMACTION_WAIT;

        case DLGRET_MSGBOX_OK:
			MSG_FATAL("COREST_SMSTIP_Handler s",0,0,0);
            MOVE_TO_STATE(pMe->m_prevState);
           
            return NFSMACTION_CONTINUE;
        
        default:
            break;
    }
    MSG_FATAL("COREST_SMSTIP_Handler End",0,0,0);
    return NFSMACTION_WAIT;
}


//add by yangdecai end 
/*==============================================================================
函数:
    WMSST_EXIT_Handler

说明:
    WMSST_EXIT 状态处理函数。

参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction WMSST_EXIT_Handler(WmsApp *pMe)
{
    MSG_FATAL("WMSST_EXIT_Handler %d",pMe->m_currState,0,0);
    // 在这里作必要的变量重置工作
    WmsApp_MemberReset(pMe);
    
    (void) ISHELL_CloseApplet(pMe->m_pShell, FALSE);
    return NFSMACTION_WAIT;
} // WMSST_EXIT_Handler


#ifdef FEATURE_USES_MMS
/*==============================================================================
函数:
    WMSST_OUTBOX_MMS_Handler

说明:
    WMSST_OUTBOX_MMS 状态处理函数。

参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction WMSST_OUTBOX_MMS_Handler(WmsApp *pMe)
{
    MSG_FATAL("WMSST_OUTBOX_MMS_Handler Start",0,0,0);
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    switch (pMe->m_eDlgReturn)
    {
        case DLGRET_CREATE:
        case DLGRET_LOADCANCELED:
        case DLGRET_LOADFAILED:
            MSG_FATAL("WMSST_OUTBOX_MMS_Handler 1",0,0,0);
            WmsApp_ShowDialog(pMe, IDD_MESSAGELIST);
            return NFSMACTION_WAIT;
            
        case DLGRET_CANCELED:
            MSG_FATAL("WMSST_OUTBOX_MMS_Handler 2",0,0,0);
            MOVE_TO_STATE(WMSST_MAIN)
            return NFSMACTION_CONTINUE;
        
        case DLGRET_LOADOK:
            MSG_FATAL("WMSST_OUTBOX_MMS_Handler 3",0,0,0);
            MOVE_TO_STATE(WMSST_VIEWOUTBOXMSG_MMS)
            return NFSMACTION_CONTINUE;
            
        case DLGRET_OPT:
            pMe->m_eOptType = OPT_VIA_LISTMSG;
            MOVE_TO_STATE(WMSST_OUTMSGMMSOPTS)
            return NFSMACTION_CONTINUE;
        case DLGRET_DELETE:
#if 0            
        	//释放查看的消息内存
			WMSMessageStruct_Free(pMe);
			//ADD BY YANGDECAI 2010-08-16
            pMe->m_eEraseWMSType = ERASE_DRAFT_ONE;
            MOVE_TO_STATE(WMSST_DELMSGCONFIRM)
#endif                
            return NFSMACTION_CONTINUE;
            
        default:
            MSG_FATAL("WMSST_OUTBOX_MMS_Handler 5",0,0,0);
            // 用退出程序代替宏断言
            MOVE_TO_STATE(WMSST_EXIT)
            return NFSMACTION_CONTINUE;
    }
} // WMSST_OUTBOX_Handler

/*==============================================================================
函数:
    WMSST_VIEWOUTBOXMSG_MMS_Handler

说明:
    WMSST_VIEWOUTBOXMSG 状态处理函数。

参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction WMSST_VIEWOUTBOXMSG_MMS_Handler(WmsApp *pMe)
{
    MSG_FATAL("WMSST_VIEWOUTBOXMSG_MMS_Handler Start",0,0,0);
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgReturn)
    {
        case DLGRET_CREATE:
            MSG_FATAL("WMSST_VIEWOUTBOXMSG_MMS_Handler DLGRET_CREATE",0,0,0);
            WmsApp_ShowDialog(pMe, IDD_VIEWMSG_MMS);
           
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
            MSG_FATAL("WMSST_VIEWOUTBOXMSG_MMS_Handler DLGRET_CANCELED",0,0,0);
            MOVE_TO_STATE(WMSST_OUTBOX_MMS)
            return NFSMACTION_CONTINUE;

        case DLGRET_OK:
            MSG_FATAL("WMSST_VIEWOUTBOXMSG_MMS_Handler DLGRET_OK",0,0,0);
            pMe->m_eOptType = OPT_VIA_VIEWMSG;
            MOVE_TO_STATE(WMSST_OUTMSGMMSOPTS)
            return NFSMACTION_CONTINUE;

        case DLGRET_INFO:
            MSG_FATAL("WMSST_VIEWOUTBOXMSG_MMS_Handler DLGRET_INFO",0,0,0);
            pMe->m_eOptType = OPT_VIA_VIEWMSG;
            MOVE_TO_STATE(WMSST_OUTMSGMMSOPTS)
            return NFSMACTION_CONTINUE;
            
        default:
            MSG_FATAL("WMSST_VIEWOUTBOXMSG_MMS_Handler default",0,0,0);
            // 用退出程序代替宏断言
            MOVE_TO_STATE(WMSST_EXIT)
            return NFSMACTION_CONTINUE;
    }
} // WMSST_VIEWOUTBOXMSG_MMS_Handler

/*==============================================================================
函数:
    WMSST_INTBOX_MMS_Handler

说明:
    WMSST_OUTBOX_MMS 状态处理函数。

参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction WMSST_INBOX_MMS_Handler(WmsApp *pMe)
{
    MSG_FATAL("WMSST_INBOX_MMS_Handler Start",0,0,0);
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    MSG_FATAL("WMSST_INBOX_MMS_Handler pMe->m_eDlgReturn:%d",pMe->m_eDlgReturn,0,0);
    switch (pMe->m_eDlgReturn)
    {
        case DLGRET_CREATE:
        case DLGRET_LOADCANCELED:
        case DLGRET_LOADFAILED:
            MSG_FATAL("WMSST_INBOX_MMS_Handler DLGRET_CREATE",0,0,0);
            WmsApp_ShowDialog(pMe, IDD_MESSAGELIST);
            return NFSMACTION_WAIT;
            
        case DLGRET_CANCELED:
            MOVE_TO_STATE(WMSST_MAIN)
            return NFSMACTION_CONTINUE;

        case DLGRET_LOADOK:

            MOVE_TO_STATE(WMSST_VIEWINBOXMSG_MMS)

            return NFSMACTION_CONTINUE;
            
        case DLGRET_OPT:
            pMe->m_eOptType = OPT_VIA_LISTMSG;
            MOVE_TO_STATE(WMSST_INMSGMMSOPTS)
            return NFSMACTION_CONTINUE;
            
        case DLGRET_DELETE:
#if 0            
        	//释放查看的消息内存
			WMSMessageStruct_Free(pMe);
			//ADD BY YANGDECAI 2010-08-16
            pMe->m_eEraseWMSType = ERASE_DRAFT_ONE;
            MOVE_TO_STATE(WMSST_DELMSGCONFIRM)
#endif                
            return NFSMACTION_CONTINUE;

        default:
            // 用退出程序代替宏断言
            MOVE_TO_STATE(WMSST_EXIT)
            return NFSMACTION_CONTINUE;
    }
} // WMSST_OUTBOX_Handler

/*==============================================================================
函数:
    WMSST_DRAFTBOX_MMS_Handler

说明:
    WMSST_OUTBOX_MMS 状态处理函数。

参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction WMSST_DRAFTBOX_MMS_Handler(WmsApp *pMe)
{
    MSG_FATAL("WMSST_DRAFTBOX_MMS_Handler Start",0,0,0);
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    MSG_FATAL("WMSST_DRAFTBOX_MMS_Handler pMe->m_eDlgReturn:%d",pMe->m_eDlgReturn,0,0);
    switch (pMe->m_eDlgReturn)
    {
        case DLGRET_CREATE:
        case DLGRET_LOADCANCELED:
        case DLGRET_LOADFAILED:
            MSG_FATAL("WMSST_DRAFTBOX_MMS_Handler DLGRET_CREATE",0,0,0);
            WmsApp_ShowDialog(pMe, IDD_MESSAGELIST);
            return NFSMACTION_WAIT;
            
        case DLGRET_CANCELED:
            MOVE_TO_STATE(WMSST_MAIN)
            return NFSMACTION_CONTINUE;

        case DLGRET_LOADOK:

            MOVE_TO_STATE(WMSST_VIEWDRAFTBOXMSG_MMS)

            return NFSMACTION_CONTINUE;
            
        case DLGRET_OPT:
            pMe->m_eOptType = OPT_VIA_LISTMSG;
            MOVE_TO_STATE(WMSST_DRAFTMSGMMSOPTS)
            return NFSMACTION_CONTINUE;
            
        case DLGRET_DELETE:
#if 0            
        	//释放查看的消息内存
			WMSMessageStruct_Free(pMe);
			//ADD BY YANGDECAI 2010-08-16
            pMe->m_eEraseWMSType = ERASE_DRAFT_ONE;
            MOVE_TO_STATE(WMSST_DELMSGCONFIRM)
#endif                
            return NFSMACTION_CONTINUE;

        default:
            // 用退出程序代替宏断言
            MOVE_TO_STATE(WMSST_EXIT)
            return NFSMACTION_CONTINUE;
    }
} // WMSST_OUTBOX_Handler

/*==============================================================================
函数:
    WMSST_VIEWOUTBOXMSG_MMS_Handler

说明:
    WMSST_VIEWOUTBOXMSG 状态处理函数。

参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction WMSST_VIEWINBOXMSG_MMS_Handler(WmsApp *pMe)
{
    MSG_FATAL("WMSST_VIEWINBOXMSG_MMS_Handler Start",0,0,0);
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgReturn)
    {
        case DLGRET_CREATE:
            MSG_FATAL("WMSST_VIEWINBOXMSG_MMS_Handler DLGRET_CREATE",0,0,0);
            WmsApp_ShowDialog(pMe, IDD_VIEWMSG_MMS);
           
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
            MSG_FATAL("WMSST_VIEWINBOXMSG_MMS_Handler DLGRET_CANCELED",0,0,0);
            MOVE_TO_STATE(WMSST_INBOX_MMS)
            return NFSMACTION_CONTINUE;

        case DLGRET_OK:
            MSG_FATAL("WMSST_VIEWINBOXMSG_MMS_Handler DLGRET_OK",0,0,0);
            pMe->m_eOptType = OPT_VIA_VIEWMSG;
            MOVE_TO_STATE(WMSST_INMSGMMSOPTS)
            return NFSMACTION_CONTINUE;

        case DLGRET_INFO:
            MSG_FATAL("WMSST_VIEWINBOXMSG_MMS_Handler DLGRET_INFO",0,0,0);
            pMe->m_eOptType = OPT_VIA_VIEWMSG;
            MOVE_TO_STATE(WMSST_INMSGMMSOPTS)
            return NFSMACTION_CONTINUE;

        case DLGRET_SAVE:
            pMe->m_ePMsgType = MESSAGE_INFORMATIVE;
            WmsApp_ShowMsgBox(pMe, IDS_SAVED);
            return NFSMACTION_WAIT; 

        case DLGRET_EFSFULL_MMS:
            pMe->m_ePMsgType = MESSAGE_WARNNING;
            WmsApp_ShowMsgBox(pMe, IDS_STOREINSUFFICIENCY);
            return NFSMACTION_WAIT;

        case DLGRET_MSGBOX_OK:  
            MSG_FATAL("WMSST_INBOX_MMS_Handler DLGRET_MSGBOX_OK",0,0,0);
            MOVE_TO_STATE(WMSST_VIEWINBOXMSG_MMS)
            return NFSMACTION_CONTINUE;
            
        case DLGRET_REPLY:
            {// 对于回复操作不再进入地址列表输入界面
                AECHAR  *pwstrNum = NULL;
                CMultiSendItemInfo *pItem = NULL;
                MSG_FATAL("WMSST_VIEWINBOXMSG_MMS_Handler DLGRET_REPLY",0,0,0);
                //释放查看的消息内存
                WMSMessageStruct_Free(pMe);
                //ADD BY YANGDECAI 2010-08-16
                // 先清空群发地址链表
                WmsApp_FreeMultiSendList(pMe->m_pSendList);
                
                // 拷贝回复地址 
#if 1//defined(FEATURE_CARRIER_VENEZUELA_MOVILNET)
                // 该运营商要求回复短信时 CBN 优先
                if (WSTRLEN(pMe->m_msCur.m_szCallBkNum)>0)
                {
                    pwstrNum = pMe->m_msCur.m_szCallBkNum;
                }
                else if (WSTRLEN(pMe->m_msCur.m_szNum)>0)
                {
                    pwstrNum = pMe->m_msCur.m_szNum;
                }
#else
                // 运营商要求回复短信时缺省地址取Originating Address
                if (WSTRLEN(pMe->m_msCur.m_szNum)>0)
                {
                    pwstrNum = pMe->m_msCur.m_szNum;
                }
                else if (WSTRLEN(pMe->m_msCur.m_szCallBkNum)>0)
                {
                    pwstrNum = pMe->m_msCur.m_szCallBkNum;
                }
#endif                
                DBGPRINTF("pwstrNum=%S",pwstrNum);
                if (NULL != pwstrNum)
                { 
                    pItem = (CMultiSendItemInfo *)sys_malloc(sizeof(CMultiSendItemInfo));
                    DBGPRINTF("WMS Address = 0x%x", pItem);  
                }
                    
                // 将回复号码保存入链表
                if ((pItem == NULL) || 
                    (SUCCESS != IVector_AddElement(pMe->m_pSendList, pItem)))
                {
                    MMSData	mmsDataInfoList[MAX_MMS_STORED];
                    AECHAR  wstrNum[MAX_PH_DIGITS+1];
                    (void) ICONFIG_GetItem(pMe->m_pConfig,
                                       CFGI_MMSINDATA_INFO,
                                       (void*)mmsDataInfoList,
                                       sizeof(mmsDataInfoList));  
                    
                    DBGPRINTF("mmsDataInfoList[%d].phoneNumber=%s, length=%d",pMe->m_wSelItemxuhao-1, 
                        mmsDataInfoList[pMe->m_wSelItemxuhao-1].phoneNumber, STRLEN(mmsDataInfoList[pMe->m_wSelItemxuhao-1].phoneNumber));
                    DBGPRINTF("MMSDataFileName=%s,g_mmsDataInfoMax=%d",mmsDataInfoList[g_mmsDataInfoMax-1].MMSDataFileName, g_mmsDataInfoMax);
                    (void)STRTOWSTR(mmsDataInfoList[pMe->m_wSelItemxuhao-1].phoneNumber, wstrNum, sizeof(wstrNum));
                    if (NULL == pItem)
                    { 
                        pItem = (CMultiSendItemInfo *)sys_malloc(sizeof(CMultiSendItemInfo));
                        DBGPRINTF("WMS Address = 0x%x", pItem);  
                    }
                    if(SUCCESS != IVector_AddElement(pMe->m_pSendList, pItem))
                    {
                        MSG_FATAL("WMSST_VIEWINBOXMSG_MMS_Handler IVector_AddElement Faild",0,0,0);
                    }
                    (void)WSTRCPY(pItem->m_szTo, wstrNum);
                }
                pMe->m_CurAddID = MSG_CMD_BASE;
                
                // 从电话本中取人名, 用于提示
                WMSUtil_GetContactName(pMe, pItem->m_szTo, pItem->m_szName, MAX_TITLE_LEN);
                
                DBGPRINTF("pItem->m_szName=%s", pItem->m_szName);
            }
            
            pMe->m_STSwitchToEditMsg = pMe->m_currState;
            pMe->m_eCreateWMSType = SEND_MSG_REPLY;
            pMe->m_dwInsertPos = 0;
            
            // 结构数据清零
            WMSMessageStruct_Reset(&pMe->m_msSend);
            
            MOVE_TO_STATE(WMSST_WRITEMSG)
            return NFSMACTION_CONTINUE;

        case DLGRET_WRITEMSG:
            MSG_FATAL("WMSST_VIEWINBOXMSG_MMS_Handler DLGRET_WRITEMSG",0,0,0);
            MOVE_TO_STATE(WMSST_WRITEMSG)
            return NFSMACTION_CONTINUE;

        case DLGRET_DELETE:
            WMS_MMS_DeleteMMS(pMe->m_wSelItemxuhao - 1,MMS_INBOX);
            if (g_mmsDataInfoMax == 0)
            {
                MOVE_TO_STATE(WMSST_MAIN)
            }
            else
            {
                MOVE_TO_STATE(WMSST_INBOX_MMS)
            }            
            return NFSMACTION_CONTINUE;
        default:
            MSG_FATAL("WMSST_VIEWOUTBOXMSG_MMS_Handler default",0,0,0);
            // 用退出程序代替宏断言
            MOVE_TO_STATE(WMSST_EXIT)
            return NFSMACTION_CONTINUE;
    }
} // WMSST_VIEWOUTBOXMSG_MMS_Handler

/*==============================================================================
函数:
    WMSST_OUTMSGOPTS_MMS_Handler

说明:
    WMSST_OUTMSGOPTS_MMS_Handler 状态处理函数。

参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
extern char* STRTOPHONENUMBER(char* pDesStr,char* pScrStr);

static NextFSMAction WMSST_OUTMSGOPTS_MMS_Handler(WmsApp *pMe)
{
    MSG_FATAL("WMSST_OUTMSGOPTS_MMS_Handler Start m_eDlgReturn=%d",pMe->m_eDlgReturn,0,0);
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgReturn)
    {
        case DLGRET_CREATE:
            MSG_FATAL("WMSST_OUTMSGOPTS_MMS_Handler DLGRET_CREATE",0,0,0);
            WmsApp_ShowDialog(pMe, IDD_MSGOPTS);
            return NFSMACTION_WAIT;

        case DLGRET_VIEW:
            MOVE_TO_STATE(WMSST_VIEWOUTBOXMSG_MMS)
            return NFSMACTION_CONTINUE;
        
        case DLGRET_CANCELED:
            if (pMe->m_eOptType == OPT_VIA_VIEWMSG)
            {
                MOVE_TO_STATE(WMSST_VIEWOUTBOXMSG_MMS)
            }
            else
            {
                MOVE_TO_STATE(WMSST_OUTBOX_MMS)
            }
            return NFSMACTION_CONTINUE;
/*
        case DLGRET_FORWARD:
            添加函数
            // 清空群发地址链表
            //WmsApp_FreeMultiSendList(pMe->m_pSendList);

            // 拷贝消息
            //WmsApp_CopyCurMsgForSend(pMe);
            
            pMe->m_STSwitchToEditMsg = pMe->m_currState;
            pMe->m_eCreateWMSType = SEND_MSG_FORWARD;
            MOVE_TO_STATE(WMSST_WRITEMSG)
            pMe->m_dwInsertPos = 0;
            return NFSMACTION_CONTINUE;
*/
        case DLGRET_DELETE:
			
            pMe->m_eEraseWMSType = ERASE_OUTBOX_MMS_ONE;
            MOVE_TO_STATE(WMSST_DELMSGCONFIRM)
            return NFSMACTION_CONTINUE;

        case DLGRET_CLEARALL:

            pMe->m_eEraseWMSType = CLEAR_OUTBOX_MMS;
            MOVE_TO_STATE(WMSST_DELMSGCONFIRM)
            return NFSMACTION_CONTINUE;

        case DLGRET_RESEND:
           
            pMe->m_eCreateWMSType = SEND_MSG_RESEND;
#ifdef FEATURE_RTRE_DEFAULT_IS_NV
		    {
		        CMultiSendItemInfo *pPhoneNumber = (CMultiSendItemInfo*)MALLOC(sizeof(CMultiSendItemInfo));
		        MMSData	mmsDataInfoList[MAX_MMS_STORED];
		        DBGPRINTF("WMS Address = 0x%x", pPhoneNumber);  
		        pMe->m_isMMS = TRUE;
		        ICONFIG_GetItem(pMe->m_pConfig,CFGI_MMSOUTDATA_INFO,&mmsDataInfoList,sizeof(mmsDataInfoList));
		        STRTOWSTR(mmsDataInfoList[pMe->m_wSelItemxuhao - 1].phoneNumber,
		            pPhoneNumber->m_szName,
		            sizeof(pPhoneNumber->m_szName));
		        STRTOWSTR(STRTOPHONENUMBER(mmsDataInfoList[pMe->m_wSelItemxuhao - 1].phoneNumber,mmsDataInfoList[pMe->m_wSelItemxuhao - 1].phoneNumber),
		            pPhoneNumber->m_szTo,
		            sizeof(pPhoneNumber->m_szTo));
		        IVector_AddElement(pMe->m_pSendList,pPhoneNumber);
		        if(!WMS_MMS_Resend(pMe->m_wSelItemxuhao - 1,MMS_OUTBOX))
		        {
		            pMe->m_SendStatus = HTTP_CODE_Bad_Request;
		            ISHELL_SetTimer(pMe->m_pShell,1000,(PFNNOTIFY)&WmsApp_ProcessMMSStatus,pMe);
		        }
		        MOVE_TO_STATE(WMSST_SENDING)
            	
		    }
#else            
            // 检查卡是否插入modi by yangdecai 2010-08-10
		    if (IRUIM_IsCardConnected(pMe->m_pIRUIM)) 
		    {
		        CMultiSendItemInfo *pPhoneNumber = (CMultiSendItemInfo*)MALLOC(sizeof(CMultiSendItemInfo));
		        MMSData	mmsDataInfoList[MAX_MMS_STORED];
		        DBGPRINTF("WMS Address = 0x%x", pPhoneNumber);  
		        pMe->m_isMMS = TRUE;
		        ICONFIG_GetItem(pMe->m_pConfig,CFGI_MMSOUTDATA_INFO,&mmsDataInfoList,sizeof(mmsDataInfoList));
		        STRTOWSTR(mmsDataInfoList[pMe->m_wSelItemxuhao - 1].phoneNumber,
		            pPhoneNumber->m_szName,
		            sizeof(pPhoneNumber->m_szName));
		        STRTOWSTR(STRTOPHONENUMBER(mmsDataInfoList[pMe->m_wSelItemxuhao - 1].phoneNumber,mmsDataInfoList[pMe->m_wSelItemxuhao - 1].phoneNumber),
		            pPhoneNumber->m_szTo,
		            sizeof(pPhoneNumber->m_szTo));
		        IVector_AddElement(pMe->m_pSendList,pPhoneNumber);
		        if(!WMS_MMS_Resend(pMe->m_wSelItemxuhao - 1,MMS_OUTBOX))
		        {
		            pMe->m_SendStatus = HTTP_CODE_Bad_Request;
		            ISHELL_SetTimer(pMe->m_pShell,1000,(PFNNOTIFY)&WmsApp_ProcessMMSStatus,pMe);
		        }
		        MOVE_TO_STATE(WMSST_SENDING)
            	
		    }
			else
			{
				MOVE_TO_STATE(WMSST_POPMSG)
			}
#endif            
            return NFSMACTION_CONTINUE;
            
        default:
            // 用退出程序代替宏断言
            MOVE_TO_STATE(WMSST_EXIT)
            return NFSMACTION_CONTINUE;
    }
}
/*==============================================================================
函数:
    WMSST_INMSGOPTS_MMS_Handler

说明:
    WMSST_INMSGOPTS_MMS_Handler 状态处理函数。

参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction WMSST_INMSGOPTS_MMS_Handler(WmsApp *pMe)
{
    MSG_FATAL("WMSST_INMSGOPTS_MMS_Handler Start m_eDlgReturn=%d",pMe->m_eDlgReturn,0,0);
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgReturn)
    {
        case DLGRET_CREATE:
            MSG_FATAL("WMSST_INMSGOPTS_MMS_Handler DLGRET_CREATE",0,0,0);
            WmsApp_ShowDialog(pMe, IDD_MSGOPTS);
            return NFSMACTION_WAIT;

        case DLGRET_VIEW:
            MOVE_TO_STATE(WMSST_VIEWINBOXMSG_MMS)
            return NFSMACTION_CONTINUE;
        
        case DLGRET_CANCELED:
            if (pMe->m_eOptType == OPT_VIA_VIEWMSG)
            {
                MOVE_TO_STATE(WMSST_VIEWINBOXMSG_MMS)
            }
            else
            {
                MOVE_TO_STATE(WMSST_INBOX_MMS)
            }
            return NFSMACTION_CONTINUE;
/*
        case DLGRET_FORWARD:
            添加函数
            // 清空群发地址链表
            //WmsApp_FreeMultiSendList(pMe->m_pSendList);

            // 拷贝消息
            //WmsApp_CopyCurMsgForSend(pMe);
            
            pMe->m_STSwitchToEditMsg = pMe->m_currState;
            pMe->m_eCreateWMSType = SEND_MSG_FORWARD;
            MOVE_TO_STATE(WMSST_WRITEMSG)
            pMe->m_dwInsertPos = 0;
            return NFSMACTION_CONTINUE;
*/
        case DLGRET_DELETE:
			
            pMe->m_eEraseWMSType = ERASE_INBOX_MMS_ONE;
            MOVE_TO_STATE(WMSST_DELMSGCONFIRM)
            return NFSMACTION_CONTINUE;

        case DLGRET_CLEARALL:

            pMe->m_eEraseWMSType = CLEAR_INBOX_MMS;
            MOVE_TO_STATE(WMSST_DELMSGCONFIRM)
            return NFSMACTION_CONTINUE;

        case DLGRET_RESEND:

            pMe->m_eCreateWMSType = SEND_MSG_RESEND;
#ifdef FEATURE_RTRE_DEFAULT_IS_NV
            // 检查卡是否插入modi by yangdecai 2010-08-10
            {
                CMultiSendItemInfo *pPhoneNumber = (CMultiSendItemInfo*)MALLOC(sizeof(CMultiSendItemInfo));
                MMSData mmsDataInfoList[MAX_MMS_STORED];
                DBGPRINTF("WMS Address = 0x%x", pPhoneNumber);  
                pMe->m_isMMS = TRUE;
                ICONFIG_GetItem(pMe->m_pConfig,CFGI_MMSINDATA_INFO,&mmsDataInfoList,sizeof(mmsDataInfoList));
                STRTOWSTR(mmsDataInfoList[pMe->m_wSelItemxuhao - 1].phoneNumber,
                    pPhoneNumber->m_szName,
                    sizeof(pPhoneNumber->m_szName));
                STRTOWSTR(STRTOPHONENUMBER(mmsDataInfoList[pMe->m_wSelItemxuhao - 1].phoneNumber,mmsDataInfoList[pMe->m_wSelItemxuhao - 1].phoneNumber),
                    pPhoneNumber->m_szTo,
                    sizeof(pPhoneNumber->m_szTo));
                IVector_AddElement(pMe->m_pSendList,pPhoneNumber);
                if(!WMS_MMS_Resend(pMe->m_wSelItemxuhao - 1,MMS_INBOX))
                {
                    pMe->m_SendStatus = HTTP_CODE_Bad_Request;
                    ISHELL_SetTimer(pMe->m_pShell,1000,(PFNNOTIFY)&WmsApp_ProcessMMSStatus,pMe);
                }
                MOVE_TO_STATE(WMSST_SENDING)
                
            }
#else
            // 检查卡是否插入modi by yangdecai 2010-08-10
		    if (IRUIM_IsCardConnected(pMe->m_pIRUIM)) 
		    {
		        CMultiSendItemInfo *pPhoneNumber = (CMultiSendItemInfo*)MALLOC(sizeof(CMultiSendItemInfo));
		        MMSData	mmsDataInfoList[MAX_MMS_STORED];
		        DBGPRINTF("WMS Address = 0x%x", pPhoneNumber);  
		        pMe->m_isMMS = TRUE;
		        ICONFIG_GetItem(pMe->m_pConfig,CFGI_MMSINDATA_INFO,&mmsDataInfoList,sizeof(mmsDataInfoList));
		        STRTOWSTR(mmsDataInfoList[pMe->m_wSelItemxuhao - 1].phoneNumber,
		            pPhoneNumber->m_szName,
		            sizeof(pPhoneNumber->m_szName));
		        STRTOWSTR(STRTOPHONENUMBER(mmsDataInfoList[pMe->m_wSelItemxuhao - 1].phoneNumber,mmsDataInfoList[pMe->m_wSelItemxuhao - 1].phoneNumber),
		            pPhoneNumber->m_szTo,
		            sizeof(pPhoneNumber->m_szTo));
		        IVector_AddElement(pMe->m_pSendList,pPhoneNumber);
		        if(!WMS_MMS_Resend(pMe->m_wSelItemxuhao - 1,MMS_INBOX))
		        {
		            pMe->m_SendStatus = HTTP_CODE_Bad_Request;
		            ISHELL_SetTimer(pMe->m_pShell,1000,(PFNNOTIFY)&WmsApp_ProcessMMSStatus,pMe);
		        }
		        MOVE_TO_STATE(WMSST_SENDING)
            	
		    }
			else
			{
				MOVE_TO_STATE(WMSST_POPMSG)
			}
#endif            
            return NFSMACTION_CONTINUE;
            
        default:
            // 用退出程序代替宏断言
            MOVE_TO_STATE(WMSST_EXIT)
            return NFSMACTION_CONTINUE;
    }
}

static NextFSMAction WMSST_VIEWDRAFTBOXMSG_MMS_Handler(WmsApp *pMe)
{
    MSG_FATAL("WMSST_VIEWDRAFTBOXMSG_MMS_Handler Start",0,0,0);
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgReturn)
    {
        case DLGRET_CREATE:
            MSG_FATAL("WMSST_VIEWDRAFTBOXMSG_MMS_Handler DLGRET_CREATE",0,0,0);
            WmsApp_ShowDialog(pMe, IDD_VIEWMSG_MMS);
           
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
            MSG_FATAL("WMSST_VIEWDRAFTBOXMSG_MMS_Handler DLGRET_CANCELED",0,0,0);
            MOVE_TO_STATE(WMSST_DRAFTBOX_MMS)
            return NFSMACTION_CONTINUE;

        case DLGRET_OK:
            MSG_FATAL("WMSST_VIEWDRAFTBOXMSG_MMS_Handler DLGRET_OK",0,0,0);
            pMe->m_eOptType = OPT_VIA_VIEWMSG;
            MOVE_TO_STATE(WMSST_DRAFTMSGMMSOPTS)
            return NFSMACTION_CONTINUE;

        case DLGRET_INFO:
            MSG_FATAL("WMSST_VIEWDRAFTBOXMSG_MMS_Handler DLGRET_INFO",0,0,0);
            pMe->m_eOptType = OPT_VIA_VIEWMSG;
            MOVE_TO_STATE(WMSST_DRAFTMSGMMSOPTS)
            return NFSMACTION_CONTINUE;

        case DLGRET_SAVE:
            pMe->m_ePMsgType = MESSAGE_INFORMATIVE;
            WmsApp_ShowMsgBox(pMe, IDS_SAVED);
            return NFSMACTION_WAIT; 

        case DLGRET_EFSFULL_MMS:
            pMe->m_ePMsgType = MESSAGE_WARNNING;
            WmsApp_ShowMsgBox(pMe, IDS_STOREINSUFFICIENCY);
            return NFSMACTION_WAIT;

        case DLGRET_MSGBOX_OK:  
            MSG_FATAL("WMSST_VIEWDRAFTBOXMSG_MMS_Handler DLGRET_MSGBOX_OK",0,0,0);
            MOVE_TO_STATE(WMSST_VIEWDRAFTBOXMSG_MMS)
            return NFSMACTION_CONTINUE;
            
        case DLGRET_REPLY:
            {// 对于回复操作不再进入地址列表输入界面
                AECHAR  *pwstrNum = NULL;
                CMultiSendItemInfo *pItem = NULL;
                MSG_FATAL("WMSST_VIEWDRAFTBOXMSG_MMS_Handler DLGRET_REPLY",0,0,0);
                //释放查看的消息内存
                WMSMessageStruct_Free(pMe);
                //ADD BY YANGDECAI 2010-08-16
                // 先清空群发地址链表
                WmsApp_FreeMultiSendList(pMe->m_pSendList);
                
                // 拷贝回复地址 
#if 1//defined(FEATURE_CARRIER_VENEZUELA_MOVILNET)
                // 该运营商要求回复短信时 CBN 优先
                if (WSTRLEN(pMe->m_msCur.m_szCallBkNum)>0)
                {
                    pwstrNum = pMe->m_msCur.m_szCallBkNum;
                }
                else if (WSTRLEN(pMe->m_msCur.m_szNum)>0)
                {
                    pwstrNum = pMe->m_msCur.m_szNum;
                }
#else
                // 运营商要求回复短信时缺省地址取Originating Address
                if (WSTRLEN(pMe->m_msCur.m_szNum)>0)
                {
                    pwstrNum = pMe->m_msCur.m_szNum;
                }
                else if (WSTRLEN(pMe->m_msCur.m_szCallBkNum)>0)
                {
                    pwstrNum = pMe->m_msCur.m_szCallBkNum;
                }
#endif                
                DBGPRINTF("pwstrNum=%S",pwstrNum);
                if (NULL != pwstrNum)
                { 
                    pItem = (CMultiSendItemInfo *)sys_malloc(sizeof(CMultiSendItemInfo));
                    DBGPRINTF("WMS Address = 0x%x", pItem);  
                }
                    
                // 将回复号码保存入链表
                if ((pItem == NULL) || 
                    (SUCCESS != IVector_AddElement(pMe->m_pSendList, pItem)))
                {
                    MMSData	mmsDataInfoList[MAX_MMS_STORED];
                    AECHAR  wstrNum[MAX_PH_DIGITS+1];
                    (void) ICONFIG_GetItem(pMe->m_pConfig,
                                       CFGI_MMSDRAFTDATA_INFO,
                                       (void*)mmsDataInfoList,
                                       sizeof(mmsDataInfoList));  
                    
                    DBGPRINTF("mmsDataInfoList[%d].phoneNumber=%s, length=%d",pMe->m_wSelItemxuhao-1, 
                        mmsDataInfoList[pMe->m_wSelItemxuhao-1].phoneNumber, STRLEN(mmsDataInfoList[pMe->m_wSelItemxuhao-1].phoneNumber));
                    DBGPRINTF("MMSDataFileName=%s,g_mmsDataInfoMax=%d",mmsDataInfoList[g_mmsDataInfoMax-1].MMSDataFileName, g_mmsDataInfoMax);
                    (void)STRTOWSTR(mmsDataInfoList[pMe->m_wSelItemxuhao-1].phoneNumber, wstrNum, sizeof(wstrNum));
                    if (NULL == pItem)
                    { 
                        pItem = (CMultiSendItemInfo *)sys_malloc(sizeof(CMultiSendItemInfo));
                        DBGPRINTF("WMS Address = 0x%x", pItem);  
                    }
                    if(SUCCESS != IVector_AddElement(pMe->m_pSendList, pItem))
                    {
                        MSG_FATAL("WMSST_VIEWDRAFTBOXMSG_MMS_Handler IVector_AddElement Faild",0,0,0);
                    }
                    (void)WSTRCPY(pItem->m_szTo, wstrNum);
                }
                pMe->m_CurAddID = MSG_CMD_BASE;
                
                // 从电话本中取人名, 用于提示
                WMSUtil_GetContactName(pMe, pItem->m_szTo, pItem->m_szName, MAX_TITLE_LEN);
                
                DBGPRINTF("pItem->m_szName=%s", pItem->m_szName);
            }
            
            pMe->m_STSwitchToEditMsg = pMe->m_currState;
            pMe->m_eCreateWMSType = SEND_MSG_REPLY;
            pMe->m_dwInsertPos = 0;
            
            // 结构数据清零
            WMSMessageStruct_Reset(&pMe->m_msSend);
            
            MOVE_TO_STATE(WMSST_WRITEMSG)
            return NFSMACTION_CONTINUE;

        case DLGRET_WRITEMSG:
            MSG_FATAL("WMSST_VIEWDRAFTBOXMSG_MMS_Handler DLGRET_WRITEMSG",0,0,0);
            MOVE_TO_STATE(WMSST_WRITEMSG)
            return NFSMACTION_CONTINUE;

        case DLGRET_CLEARALL:
            MSG_FATAL("WMSST_VIEWDRAFTBOXMSG_MMS_Handler DLGRET_CLEARALL",0,0,0);
            pMe->m_eEraseWMSType = CLEAR_DRAFTBOX_MMS;
            MOVE_TO_STATE(WMSST_DELMSGCONFIRM)
            return NFSMACTION_CONTINUE;

        case DLGRET_DELETE:
            WMS_MMS_DeleteMMS(pMe->m_wSelItemxuhao - 1,MMS_DRAFTBOX);
            if (g_mmsDataInfoMax == 0)
            {
                MOVE_TO_STATE(WMSST_MAIN)
            }
            else
            {
                MOVE_TO_STATE(WMSST_DRAFTBOX_MMS)
            }            
            return NFSMACTION_CONTINUE;
        default:
            MSG_FATAL("WMSST_VIEWDRAFTBOXMSG_MMS_Handler default",0,0,0);
            // 用退出程序代替宏断言
            MOVE_TO_STATE(WMSST_EXIT)
            return NFSMACTION_CONTINUE;
    }
} // WMSST_VIEWOUTBOXMSG_MMS_Handler

/*==============================================================================
函数:
    WMSST_DRAFTMSGOPTS_MMS_Handler

说明:
    WMSST_DRAFTMSGOPTS_MMS_Handler 状态处理函数。

参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction WMSST_DRAFTMSGOPTS_MMS_Handler(WmsApp *pMe)
{
    MSG_FATAL("WMSST_DRAFTMSGOPTS_MMS_Handler Start m_eDlgReturn=%d",pMe->m_eDlgReturn,0,0);
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgReturn)
    {
        case DLGRET_CREATE:
            MSG_FATAL("WMSST_DRAFTMSGOPTS_MMS_Handler DLGRET_CREATE",0,0,0);
            WmsApp_ShowDialog(pMe, IDD_MSGOPTS);
            return NFSMACTION_WAIT;

        case DLGRET_VIEW:
            MOVE_TO_STATE(WMSST_VIEWDRAFTBOXMSG_MMS)
            return NFSMACTION_CONTINUE;
        
        case DLGRET_CANCELED:
            if (pMe->m_eOptType == OPT_VIA_VIEWMSG)
            {
                MOVE_TO_STATE(WMSST_VIEWDRAFTBOXMSG_MMS)
            }
            else
            {
                MOVE_TO_STATE(WMSST_DRAFTBOX_MMS)
            }
            return NFSMACTION_CONTINUE;

        case DLGRET_DELETE:
			
            pMe->m_eEraseWMSType = ERASE_DRAFTBOX_MMS_ONE;
            MOVE_TO_STATE(WMSST_DELMSGCONFIRM)
            return NFSMACTION_CONTINUE;

        case DLGRET_CLEARALL:

            pMe->m_eEraseWMSType = CLEAR_DRAFTBOX_MMS;
            MOVE_TO_STATE(WMSST_DELMSGCONFIRM)
            return NFSMACTION_CONTINUE;

        case DLGRET_RESEND:

            pMe->m_eCreateWMSType = SEND_MSG_RESEND;
#ifdef FEATURE_RTRE_DEFAULT_IS_NV    
            {
                CMultiSendItemInfo *pPhoneNumber = (CMultiSendItemInfo*)MALLOC(sizeof(CMultiSendItemInfo));
                MMSData mmsDataInfoList[MAX_MMS_STORED];
                DBGPRINTF("WMS Address = 0x%x", pPhoneNumber);  
                pMe->m_isMMS = TRUE;
                ICONFIG_GetItem(pMe->m_pConfig,CFGI_MMSINDATA_INFO,&mmsDataInfoList,sizeof(mmsDataInfoList));
                STRTOWSTR(mmsDataInfoList[pMe->m_wSelItemxuhao - 1].phoneNumber,
                    pPhoneNumber->m_szName,
                    sizeof(pPhoneNumber->m_szName));
                STRTOWSTR(STRTOPHONENUMBER(mmsDataInfoList[pMe->m_wSelItemxuhao - 1].phoneNumber,mmsDataInfoList[pMe->m_wSelItemxuhao - 1].phoneNumber),
                    pPhoneNumber->m_szTo,
                    sizeof(pPhoneNumber->m_szTo));
                IVector_AddElement(pMe->m_pSendList,pPhoneNumber);
                if(!WMS_MMS_Resend(pMe->m_wSelItemxuhao - 1,MMS_DRAFTBOX))
                {
                    pMe->m_SendStatus = HTTP_CODE_Bad_Request;
                    ISHELL_SetTimer(pMe->m_pShell,1000,(PFNNOTIFY)&WmsApp_ProcessMMSStatus,pMe);
                }
                MOVE_TO_STATE(WMSST_SENDING)
                
            }
#else
            // 检查卡是否插入modi by yangdecai 2010-08-10
		    if (IRUIM_IsCardConnected(pMe->m_pIRUIM)) 
		    {
		        CMultiSendItemInfo *pPhoneNumber = (CMultiSendItemInfo*)MALLOC(sizeof(CMultiSendItemInfo));
		        MMSData	mmsDataInfoList[MAX_MMS_STORED];
		        DBGPRINTF("WMS Address = 0x%x", pPhoneNumber);  
		        pMe->m_isMMS = TRUE;
		        ICONFIG_GetItem(pMe->m_pConfig,CFGI_MMSINDATA_INFO,&mmsDataInfoList,sizeof(mmsDataInfoList));
		        STRTOWSTR(mmsDataInfoList[pMe->m_wSelItemxuhao - 1].phoneNumber,
		            pPhoneNumber->m_szName,
		            sizeof(pPhoneNumber->m_szName));
		        STRTOWSTR(STRTOPHONENUMBER(mmsDataInfoList[pMe->m_wSelItemxuhao - 1].phoneNumber,mmsDataInfoList[pMe->m_wSelItemxuhao - 1].phoneNumber),
		            pPhoneNumber->m_szTo,
		            sizeof(pPhoneNumber->m_szTo));
		        IVector_AddElement(pMe->m_pSendList,pPhoneNumber);
		        if(!WMS_MMS_Resend(pMe->m_wSelItemxuhao - 1,MMS_DRAFTBOX))
		        {
		            pMe->m_SendStatus = HTTP_CODE_Bad_Request;
		            ISHELL_SetTimer(pMe->m_pShell,1000,(PFNNOTIFY)&WmsApp_ProcessMMSStatus,pMe);
		        }
		        MOVE_TO_STATE(WMSST_SENDING)
            	
		    }
			else
			{
				MOVE_TO_STATE(WMSST_POPMSG)
			}
#endif            
            return NFSMACTION_CONTINUE;
            
        default:
            // 用退出程序代替宏断言
            MOVE_TO_STATE(WMSST_EXIT)
            return NFSMACTION_CONTINUE;
    }
}

/*==============================================================================
函数:
    WMSST_EDIT_ALBUMOREMAIN_Handler

说明:
    WMSST_WRITEMSG 状态处理函数。

参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction WMSST_EDIT_ALBUMOREMAIN_Handler(WmsApp *pMe)
{
    MSG_FATAL("WMSST_EDIT_ALBUMOREMAIN_Handler Start",0,0,0);
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    switch (pMe->m_eDlgReturn)
    {
        // 创建编辑消息对话框界面
        case DLGRET_CREATE:
            WmsApp_ShowDialog(pMe, IDD_ALBUMOREMAIN);
            return NFSMACTION_WAIT;
            
        case DLGRET_MSGBOX_OK:
            MOVE_TO_STATE(WMSST_TONUMLIST)
            return TRUE;
            
        case DLGRET_CANCELED:
            MOVE_TO_STATE(WMSST_TONUMLIST)
            return NFSMACTION_CONTINUE;
            
        default:
            // 用退出程序代替宏断言
            MOVE_TO_STATE(WMSST_TONUMLIST)
            break;
    }
    
    return NFSMACTION_CONTINUE;
} // WMSST_EDIT_ALBUMOREMAIN_Handler


/*==============================================================================
函数:
    WMSST_EDIT_ALBUMOREMAIN_Handler

说明:
    WMSST_WRITEMSG 状态处理函数。

参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction WMSST_MMS_SERVER_ADDRESS_Handler(WmsApp *pMe)
{
    MSG_FATAL("WMSST_MMS_SERVER_ADDRESS_Handler Start",0,0,0);
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    switch (pMe->m_eDlgReturn)
    {
        // 创建编辑消息对话框界面
        case DLGRET_CREATE:
            WmsApp_ShowDialog(pMe, IDD_MMS_SERVER_ADDRESS);
            return NFSMACTION_WAIT;
            
        case DLGRET_MSGBOX_OK:
            MOVE_TO_STATE(WMSST_MSGSETTING)
            return TRUE;
            
        case DLGRET_CANCELED:
            MOVE_TO_STATE(WMSST_MSGSETTING)
            return NFSMACTION_CONTINUE;
            
        default:
            // 用退出程序代替宏断言
            MOVE_TO_STATE(WMSST_MSGSETTING)
            break;
    }
    
    return NFSMACTION_CONTINUE;
} // WMSST_MMS_SERVER_ADDRESS_Handler

/*==============================================================================
函数:
    WMSST_MMS_PROXY_Handler

说明:
    WMSST_MMS_PROXY 状态处理函数。

参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction WMSST_MMS_PROXY_Handler(WmsApp *pMe)
{
    MSG_FATAL("WMSST_MMS_PROXY_Handler Start",0,0,0);
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    switch (pMe->m_eDlgReturn)
    {
        // 创建编辑消息对话框界面
        case DLGRET_CREATE:
            WmsApp_ShowDialog(pMe, IDD_MMS_PROXY);
            return NFSMACTION_WAIT;
            
        case DLGRET_MSGBOX_OK:
            MOVE_TO_STATE(WMSST_MSGSETTING)
            return TRUE;
            
        case DLGRET_CANCELED:
            MOVE_TO_STATE(WMSST_MSGSETTING)
            return NFSMACTION_CONTINUE;
            
        default:
            // 用退出程序代替宏断言
            MOVE_TO_STATE(WMSST_MSGSETTING)
            break;
    }
    
    return NFSMACTION_CONTINUE;
} // WMSST_MMS_PROXY_Handler

/*==============================================================================
函数:
    WMSST_MMS_PORT_Handler

说明:
    WMSST_MMS_PORT 状态处理函数。

参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction WMSST_MMS_PORT_Handler(WmsApp *pMe)
{
    MSG_FATAL("WMSST_MMS_PORT_Handler Start",0,0,0);
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    switch (pMe->m_eDlgReturn)
    {
        // 创建编辑消息对话框界面
        case DLGRET_CREATE:
            WmsApp_ShowDialog(pMe, IDD_MMS_PORT);
            return NFSMACTION_WAIT;
            
        case DLGRET_MSGBOX_OK:
            MOVE_TO_STATE(WMSST_MSGSETTING)
            return TRUE;
            
        case DLGRET_CANCELED:
            MOVE_TO_STATE(WMSST_MSGSETTING)
            return NFSMACTION_CONTINUE;
            
        default:
            // 用退出程序代替宏断言
            MOVE_TO_STATE(WMSST_MSGSETTING)
            break;
    }
    
    return NFSMACTION_CONTINUE;
} // WMSST_MMS_PORT_Handler

static NextFSMAction WMSST_MMS_MEMSTATUS_Handler(WmsApp * pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgReturn)
    {
        case DLGRET_CREATE:
            WmsApp_ShowDialog(pMe, IDD_MMS_MEMSTATUS);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:              
            MOVE_TO_STATE(WMSST_MANAGEMENT)   
            return NFSMACTION_CONTINUE;

        default:
            // 用退出程序代替宏断言
            MOVE_TO_STATE(WMSST_EXIT)
            return NFSMACTION_CONTINUE;
    }
} // WMSST_MMS_MEMSTATUS_Handler

#endif

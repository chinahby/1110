#ifndef WMSAPP_PRIV_H
#define WMSAPP_PRIV_H
/*==============================================================================
// 文件：WMSApp_priv.h
//        2007-10-15初始版本(Init Version)
//        版权所有(c) 2009 Anylook Tech. CO.,LTD.
//        
// 文件说明：
//        
// 作者：
// 创建日期：2007-10-15
// 当前版本：Init Version
==============================================================================*/

/*==============================================================================
修改历史记录：
时       间      修 改 人     修改内容、位置及原因
-----------      ----------   -----------------------------------------------
2007-10-15                    初始版本
==============================================================================*/

/*==============================================================================
                             
                             本文件包含的外部文件
                             
==============================================================================*/
#ifndef AEE_SIMULATOR
#include "customer.h"         //Handset Feature Definitions
#else
#include "BREWSimFeatures.h"  // Simulator Feature Definitions
#endif

#if !defined(BREW_STATIC_APP)
#include "WMSAPP.BID"
#else
#include "OEMClassIds.h"      // Applet Ids
#ifndef AEECLSID_WMSAPP
#error AEECLSID_WMSAPP must be defined before including WMSApp.h
#endif
#endif
#include "WMSApp.h"
#include "AEEHeap.h"          // AEE Memory Allocation Services
#include "AEEMenu.h"          // AEE Menu Services
#include "AEEFile.h"          // AEE File Services
#include "AEEText.h"          // AEE Text Services
#include "AEEStdLib.h"        // AEE StdLib Services

#include "AEEAlert.h"         // IAlert Services
#include "AEEAppGen.h"        // AEEApplet Declaration
// callback structure definitions.
#include "AEEVector.h"
#include "AEEAnnunciator.h"
#include "AEEBacklight.h"

#include "wms.brh"          // Applet Resource Header

#include "AEEConfig.h"
#include "wms.h"
#include "AEEWMS.h"
#ifdef FEATURE_ICM
#include "AEECM.h"
#else
#include "AEETelephone.h"
#include "AEETelDef.h"
#endif
#include "OEMWMS_CacheInfo.h"
#include "OEMHeap.h"
#include "wmsi.h"

#include "Appscommon.h"
// 图片资源文件
#include "appscommonimages.brh"

#include "BREWVersion.h"
#include "oemcfgi.h"
#include "AEESound.h"
#include "AEE_OEM.h"
#include "AEERUIM.h"

#ifdef CUST_EDITION  /*add by miaoxiaoming*/
#include "OEMSVC.h"
#endif

#ifdef FEATURE_USES_MMS
#include "MediaGallery.h"
#include "WmsMms.h"
#include "AEEMedia.h"
#define MMSFILE_DIR      "fs:/user/mms"
#endif

#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
#define WMSAPP_PT_IN_RECT(a,b,rct)      (boolean)( ((a) >= (rct).x && (a) <= ((rct).x + (rct).dx)) && ((b) >= (rct).y && (b) <= ((rct).y + (rct).dy)) )
#endif
#define NMSGLEN    153

#define SYS_FREEIF(p)                 if (p) { sys_free(p); (p) = NULL; }


/*==============================================================================
                                 
                                相关数据类型定义
                                 
==============================================================================*/
typedef struct
{
    wms_cmd_id_e_type           cmd;
    wms_cmd_err_e_type          cmd_err;
} wmsapp_cmd_status_type;

typedef struct _CMultiSendItemInfo
{ 
    // 接收消息方号码
    AECHAR      m_szTo[MAX_PH_DIGITS+1];
    
    // 接收消息方号码在电话本中对应的姓名
    AECHAR      m_szName[MAX_TITLE_LEN+1]; 
    //接收消息方邮件
    AECHAR      m_szEmail[MAX_EMAILADD_LEN+1];
} CMultiSendItemInfo;

// 短消息消息数据结构---供 WMS Applet 内部使用
typedef struct 
{
    AECHAR                      m_szCallBkNum[MAX_PH_DIGITS+1];     // Call Back Number
    AECHAR                      m_szNum[MAX_PH_DIGITS+1];           // 发送/接收消息方号码
    AECHAR                      *m_szMessage;       // 消息体
    uint32                      m_dwTime;           // 消息时间戳
    wms_priority_e_type         m_epriority;        // 消息优先级
    wms_privacy_e_type          m_eprivacy;         // 消息密级
    wms_message_tag_e_type      msg_tag;            // 消息标记
    wms_language_e_type         language;           // 语言指示
    boolean                     m_bDeliveryReport;  // 是否需要消息回执,仅发送短信时有用
#ifdef FEATURE_SMS_UDH
    uint16                      msg_ref;            // 消息参考号
    uint8                       total_sm;           // 消息数据包总数
    uint8                       seq_num;            // 消息序号
#endif
    uint8                       m_NumMsgs;          // 消息数目(仅语音通知短信)
    wms_memory_store_e_type     mem_store;          // 消息存储位置
} WMSMessageStruct;

// 短消息配置结构 WMSApp configuration structure.
typedef struct WMSAPP_Config_Type
{
    // 发送短信存储选项
    boolean                 m_bAutoSave;   
    
    // 收到短信时，若设定空间满是否自动删除已读消息腾出空间
    boolean                 m_bAutoDelete;

#ifdef FEATURE_SMS_UDH
    uint8                   m_msg_ref;
#endif    
    
    uint16                  m_msgid;
    
    // 用于指示配置信息是否初始化，用特定值 WMS_CFG_VERSION 表示已初始化
    int                     nInitIndictor;  
} WMSAPP_Config_Type;
#ifdef FEATURE_USES_MMS
typedef struct
{
    uint32 nResIndex;
    char* type;
}WSP_MMS_RESOURCEINDEXTYPE;

typedef struct
{
    uint16 nIndex;
    uint16 nCount;
    boolean isDoing;
    WSP_MMS_RESOURCEINDEXTYPE data[10];
}WSP_MMS_RESOURCEDATA;

typedef struct 
{
    WSP_MMS_RESOURCEDATA textData;
    WSP_MMS_RESOURCEDATA imageData;
    WSP_MMS_RESOURCEDATA soundData;
    WSP_MMS_RESOURCEDATA videoData;
    WSP_MMS_RESOURCEDATA otherData;
    uint32               nIndex;
}WSP_MMS_RESOURCE;
#endif

// 用于生成消息列表菜单页的方式
typedef enum _MAKE_MSGList_Mode
{
   MAKEMSGLIST_INIT,            // 从列表开始处建立
   MAKEMSGLIST_BACKONE,         // 菜单列表后退一项
   MAKEMSGLIST_NEXTONE,         // 菜单列表前进一项
   MAKEMSGLIST_RESUMECURPAGE,   // 使用当前菜单列表
   MAKEMSGLIST_PREPAGE,         // 菜单列表回到前一页
   MAKEMSGLIST_NEXTPAGE         // 菜单列表前进一页
} MAKE_MSGList_Mode;

// 用于建立群发接收地址列表菜单的方式
typedef enum _MAKE_ADDList_Mode
{
   MAKEADDLIST_NONE,            // 用于发给单人
   MAKEADDLIST_INIT,            // 从列表开始处建立
   MAKEADDLIST_BACKONE,         // 菜单列表后退一项
   MAKEADDLIST_NEXTONE,         // 菜单列表前进一项
   MAKEADDLIST_RESUMECURPAGE    // 使用当前菜单列表页
} MAKE_ADDList_Mode;

// 信息编辑时插入类型
typedef enum Insert_Type
{
    INSERT_NONE,        // 无效插入类型
    INSERT_EDITWMS,     // 编辑短信时插入
    INSERT_EDITTEPLATE  // 编辑模板时插入
} Insert_Type;

// 从电话本取号码时操作类型
typedef enum GetAddr_Type
{
    GETADDR_NONE,       // 无操作
    GETADDR_INSERT,     // 插入号码地址
    GETADDR_ONEADD,     // 选取单条号码地址
    GETADDR_MULTIADD,  // 选取多条号码地址
    GETADDR_GROUPlIST,    // 群组列表
    GETADDR_GROUPLIST_ONEADD // 从群组单选
} GetAddr_Type;

typedef enum Wms_e_Copy_Type
{
    NONECOPY,
    MOVETORUIM,
    MOVETOPHONEINBOX,
    COPYTORUIM,
    COPYTOPHONEINBOXE,  // 将RUIM中一条消息复制到话机收件箱
    COPYALLMTTORUIM,    // 将话机收件箱中全部消息复制到RUIM
    COPYALLRUIMTOPHONE, // 将RUIM全部消息复制到话机中
    MOVEALLMTTORUIM,    // 将话机收件箱中全部消息移动到RUIM
    MOVEALLRUIMTOPHONE  // 将RUIM全部消息移动到话机中
} Wms_e_Copy_Type;

typedef enum Wms_e_ContinueSend_Type
{
    NONE_CONTINUE = 0,
    CONTINUE_CURPERSON,
    CONTINUE_NEXTPERSON
} Wms_e_ContinueSend_Type;

//删除信息方式
typedef enum _Erase_SMS_e_Type
{
    ERASE_DRAFT_ONE,        //草稿箱一条信息
    ERASE_INBOXES_ONE,      //收件箱一条信息
    ERASE_READMSG,          //删除已读消息
    ERASE_VOICEMAIL_ONE,    //一条语音信息
    ERASE_OUTBOX_ONE,       //发件箱一条信息
    ERASE_RESERVED_ONE,     //一条预约信息
    ERASE_RESERVEFAILED_ONE,//一条失败预约信息
    ERASE_TEMPLATE_ONE,     //一条模板信息

    CLEAR_PHONE,            //收件箱话机信息

#ifdef FEATURE_CDSMS_RUIM
    CLEAR_RUIM,             // UIM卡上信息
#endif
    CLEAR_OUTBOX,           //发件箱全部信息

    CLEAR_DRAFT,            //全部草稿信息
    
    CLEAR_RESERVED,         //全部预约信息
    CLEAR_RESERVEFAILED,    //全部预约失败信息
    CLEAR_ALLRESERVED,      //全部预约信息，包含未发送以及失败的信息

    CLEAR_INBOXES,          //收件箱上信息(话机+UIM)
    CLEAR_ALL               //全部删除
#ifdef FEATURE_USES_MMS
    ,CLEAR_INBOX_MMS
    ,ERASE_INBOX_MMS_ONE
    ,CLEAR_OUTBOX_MMS
    ,ERASE_OUTBOX_MMS_ONE
    ,CLEAR_DRAFTBOX_MMS
    ,ERASE_DRAFTBOX_MMS_ONE    
#endif
} Erase_SMS_e_Type;

typedef enum
{
    OPT_VIA_VIEWMSG=0x00,
    OPT_VIA_LISTMSG
} WMS_OPT_e_type;

typedef enum
{
    EXTRACT_NUM=0x00,
    EXTRACT_EMAILADD,
    EXTRACT_URL
} WMS_Extract_e_type;

typedef enum
{
    WMSAPP_STOP=0x00,
    WMSAPP_RUNNING,
    WMSAPP_SUSPEND
} WMSAPP_STATUS_e_type;

typedef enum
{
    DT_SEL_NONE = 0x00,
    DT_SEL_YEAR,
    DT_SEL_MONTH,
    DT_SEL_DAY,
    DT_SEL_HOUR,
    DT_SEL_MIN,
    DT_SEL_TIMEFMT
} DT_Sel_e_type;

typedef struct RESERVEDMSG_DT
{
    uint32  dwSecs;         // 以秒表示的时间
    AECHAR  wstrYear[5];    // 年字符串,如 2008
    AECHAR  wstrMonth[3];   // 月字符串,如 7
    AECHAR  wstrDay[3];     // 日字符串,如 3
    AECHAR  wstrHour[3];    // 时字符串,如 15
    AECHAR  wstrMin[3];     // 分字符串,如 59
    byte    btTimeFmt;      // 时间格式
#ifdef FEATURE_TIME_DATA_SETTING
    byte    btDateFmt;      // 日期格式
#endif 
    int     nSelIdx;        // 当前选中项索引  
    DT_Sel_e_type eSel[6];  // 可供选择项列表
    DT_Sel_e_type ePreSel;  // 前一选中项
    boolean       bAm;      // TRUE: 上午 FALSE: 下午
    boolean       bFirst;   // 是否首次编辑当前项
} RESERVEDMSG_DT;


// Structure to carry the WMS Applet module information and the object reference count.
typedef struct CWMSMod
{
    DECLARE_VTBL(IModule)
    uint32   m_nRefs;
} CWMSMod;

typedef enum
{
    SEND_MSG_DRAFT,         // 发送草稿信息
    SEND_MSG_NEW,           // 新建
    SEND_MSG_REPLY,         // 回复
    SEND_MSG_FORWARD,       // 转发
    SEND_MSG_RESEND,        // 重发
    SEND_MSG_TEMPLATE,      // 发送模板
    SEND_MSG_RESERVE,       // 预约信息 
    SEND_MSG_EDITRESERVE    // 编辑预约信息        
} Create_SMS_e_Type;

// 对话框事件处理函数指针类型定义
typedef boolean (*WMSPFNDLGHANDLER)(void *pUser, AEEEvent evt, uint16 w, uint32 dw);


// WMS Applet状态。
typedef enum WMSAPPState
{
    // 空状态
    WMSST_NONE = 0,
    
    // 初始化状态
    WMSST_INIT,
    
    // 短消息主界面状态
    WMSST_MAIN,
    
    // 密码验证
    WMSST_CHKPWD,
    
    // 收件箱(Phone+UIM)界面状态
    WMSST_INBOXES,
    
    // 用于查看收件箱内消息的状态
    WMSST_VIEWINBOXMSG,  
    
    // 查看收件箱消息后对该消息的处理选项
    WMSST_INBOXMSGOPTS,
    
#ifdef FEATURE_CDSMS_RUIM
    // 用于机卡间消息复制、移动前用户确认
    WMSST_MSGCOPYCONFIRM,

    // 用于机卡间消息复制、移动
    WMSST_MSGCOPY,
#endif    
    
    // 用于用户发送消息时输入接收方号码的状态
    WMSST_TONUMLIST,
    
    // 用于用户发送消息时编辑消息文本的状态
    WMSST_WRITEMSG,
    
    // 消息发送选项
    WMSST_SENDOPTS,
    
    // 用于提示用户发消息的状态
    WMSST_SENDING, 
    
    // 显示发件箱列表的状态
    WMSST_OUTBOX,
   
    // 用于查看发件箱内消息的状态
    WMSST_VIEWOUTBOXMSG,
    
    // 查看发件箱消息后对该消息的处理选项
    WMSST_OUTMSGOPTS,  
    
    // 显示收藏夹消息列表的状态
    WMSST_DRAFT, 
    
    // 用于查看草稿箱内消息的状态
    WMSST_VIEWDRAFTMSG, 
    
    // 查看草稿箱内消息后对该消息的处理选项
    WMSST_DRAFTMSGOPTS, 

#ifdef FEATURE_CONTACT_APP
    // 提取详情
    WMSST_EXTARCTDETAILS,
    
    // 提取详情所得项清单
    WMSST_EXTARCTEDITEMLIST,
    
    // 针对提取详情所得项的操作选项
    WMSST_EXTARCTEDITEMOPT,
#endif
    
    // 用于删除多条消息
    WMSST_DELETEMSGS, 
    
#if defined(FEATURE_CDSMS_RUIM)
    // 消息在机卡间互拷
    WMSST_COPYMSGS,
#endif    
    
    // 用于删除消息确认
    WMSST_DELMSGCONFIRM, 
    
    // 用于参数设置
    WMSST_MSGSETTING, 
    
    // 预约信息
    WMSST_RESERVEDMSG,
    
    // 预约信息时间设置
    WMSST_RSVDMSGTIME,
    
    // 预约信息列表
    WMSST_RESERVEDMSGS,
    
    // 查看某条预约信息
    WMSST_VIEWRESERVEDMSG,

    // 某条预约信息的相关操作选项
    WMSST_RESERVEDMSGOPT,
    
    // 预约短信时间到提示
    WMSST_RESERVEDMSGALERT,

    // 语音信箱主菜单
    WMSST_VOICEMAIL, 
    
    // 查看语音信箱
    WMSST_VIEWVOICEMAIL, 
    
    // 提示设置语音信箱号码
    WMSST_VMNUMALERT, 
    
    // 语音信箱号码设置
    WMSST_VMNUM, 
    
    // 语音信息操作选项
    WMSST_VOICEMAILOPTS,  
    
    // 消息模板列表
    WMSST_TEMPLATES,
    
    // 针对某条模板的可选操作
    WMSST_TEMPLATESOPTS,
    
    // 查看某条消息模板
    WMSST_VIEWTEMPLATE,
    
    // 消息模板编辑
    WMSST_EDITTEMPLATE,
    
    WMSST_AUTOSAVE,
    
    // 发送报告开关设置
    WMSST_REPORTS,
    
#ifdef FEATURE_AUTOREPLACE
    // 收件箱满时，是否允许用新消息自动替换最旧消息开关设置
    WMSST_AUTOREPLACE,
#endif    
    
    // Callback Number 开关设置
    WMSST_CALLBACKNUMSWITCH,
    
    //发送模式设置
    WMSST_SENDMODE,
    
    // Callback Number 输入
    WMSST_CALLBACKNUMSET,
    
    // 接收短信存储位置设置
    WMSST_STORAGE,
    
    WMSST_EDITCALLREJECTSMS,
   
    // 存储器状态
    WMSST_MEMSTATUS,

    //信息管理状态
    WMSST_MANAGEMENT,
    
    //拷贝消息状态
    WMSST_COPYINBOX,
    
    //移动消息状态
    WMSST_MOVEINBOX,
    
    // 消息有效期设置
    WMSST_MSGVALIDITY, 

	//Add By zzg 2012_02_04
	//时间戳
	WMSST_TIME_STAMP,
	//Add End
    
    // 消息优先级设置
    WMSST_PRIORITY,
    
    WMSST_AUTODELETE,
    
    WMSST_RESERVEDMSGALERTTIMEOUT,
    
    // 是否继续未完成的发送任务确认
    WMSST_CONTINUESEND_QUERY,

    // 选择电话号码来源
    WMSST_SELECTFROM,
    
    //用于发送失败是否重发  add by yangdecai
    WMSST_RESENDCONFIRM,
	//用于新短息提示消息框add by yangdecai
	WMSST_WMSNEW,
	//用于弹出消息add by yangdecai
	WMSST_POPMSG,

	//用于显示FLASH SMS消息
	WMSST_FLASHSMS,

#ifdef FEATURE_USES_MMS
    // 显示彩信发件箱列表的状态
    WMSST_MMSNOTIFY,
    WMSST_MMSDELIVERYREPORT,
    WMSST_READREPLY,
    WMSST_REPORTALLOWED,
    WMSST_SENDERVISIBILITY,
    WMSST_INBOX_MMS,
    WMSST_OUTBOX_MMS,
    WMSST_VIEWINBOXMSG_MMS,
    WMSST_VIEWOUTBOXMSG_MMS,
    WMSST_OUTMSGMMSOPTS,
    WMSST_INMSGMMSOPTS,
    WMSST_DRAFTBOX_MMS,
    WMSST_VIEWDRAFTBOXMSG_MMS,
    WMSST_DRAFTMSGMMSOPTS,
	WMSST_GETTING,
	
#endif

    // 退出短信应用
    WMSST_EXIT
} WMSAPPState;

// 状态处理函数返回给主状态处理函数的值
typedef enum NextFSMAction
{
   NFSMACTION_WAIT,
   NFSMACTION_CONTINUE
} NextFSMAction;

// 对话框可能的返回值列表
typedef enum DLGRetValue
{
   // 初始化值，指示需要创建对话框
   DLGRET_CREATE,         
   DLGRET_CANCELED,
   DLGRET_OK, 
   DLGRET_INFO,
   DLGRET_MSGBOX_OK,
   DLGRET_OPT,

   // IDD_MAIN specific return values
   DLGRET_INBOXES,
   DLGRET_WRITEMSG,
   DLGRET_OUTBOX,
   DLGRET_VIEWVOICEMSG,
   DLGRET_DRAFT,
   DLGRET_TEMPLATES,
   DLGRET_MEMSTATUS,
   DLGRET_DELETEMSGS,
   DLGRET_MANAGEMENT,
   DLGRET_COPYINBOX,
   DLGRET_MOVEINBOX,
   DLGRET_SETTINGS,
   DLGRET_RESERVEDMSG,
   
   DLGRET_NEWRESERVEDMSG,
   DLGRET_RESERVEDMSGLST,
   DLGRET_FAILEDRSVDMSGS,
   DLGRET_YEARERR,
   DLGRET_DATEERR,

   // IDD_TEXT specific return values
   DLGRET_PHONE,
   DLGRET_RUIM,
   
   DLGRET_CLEARINBOXES,
   DLGRET_CLEAROUTBOX,
   DLGRET_CLEARDRAFT,
   DLGRET_CLEARRUIM,
   DLGRET_CLEARPHONE,
   DLGRET_CLEARRESERVED,
   DLGRET_CLEARALL,

   DLGRET_COPYMSGS,
   DLGRET_COPYINBOXMSGTORUIM,
   DLGRET_COPYRUIMMSGTOINBOX,
   DLGRET_MOVEINBOXMSGTORUIM,
   DLGRET_MOVERUIMMSGTOINBOX,
   
   DLGRET_SETTINGAUTOSAVE,
   DLGRET_PRIORITY,
   DLGRET_REPORTS,
#ifdef FEATURE_AUTOREPLACE
   DLGRET_AUTOREPLACE,
#endif   
   DLGRET_CALLBACKNUM,
   DLGRET_SENDMODE,
   DLGRET_STORAGE,
   DLGRET_MSGVALIDITY,
   DLGRET_TIME_STAMP,	//Add By zzg 2012_02_04
   DLGRET_EDITCALLREJECTSMS,
   DLGRET_AUTODELETE,
   DLGRET_RESERVEDMSGALERTTIMEOUT,
   
   DLGRET_WRITEMSGOPTS,
   DLGRET_TEXTFULL,
   DLGRET_SEND,
   DLGRET_RESEND,
   DLGRET_SENDOPT,
   DLGRET_INSERTTEMPLATES,
   DLGRET_SAVETODRAFT,
   DLGRET_SAVEMSG,

   DLGRET_EDIT,
   DLGRET_DELETE,
   DLGRET_DELETEREADMSG,
   DLGRET_ADDNEW,
   DLGRET_VIEW,
   
   DLGRET_SAVE,
   DLGRET_REPLY,
   DLGRET_FORWARD,
#ifdef FEATURE_CONTACT_APP
   DLGRET_EXTACTDETAILS,
   DLGRET_SAVENUM,
   DLGRET_SAVEEMAIL,
   DLGRET_SAVEURL,
#ifdef FEATURE_USES_MMS
   DLGRET_GETMMS,
   DLGRET_GET,
#endif
#endif   
   DLGRET_COPYTORUIM,
   DLGRET_COPYTOPNONE,
   DLGRET_MOVETORUIM,
   DLGRET_MOVETOPNONE,
   
   DLGRET_END,
   
   DLGRET_VMNUM,
   DLGRET_CALLVMNUM,
   
   DLGRET_LOAD,
   DLGRET_LOADOK,
   DLGRET_LOADFAILED,
   DLGRET_LOADCANCELED,
   
   DLGRET_DONE,
   DLGRET_PWDERR,
   DLGRET_PWDOK,

   DLGRET_SELECTFROMOPT,
   DLGRET_EXIT_EDITOR,
   DLGRET_SAVEDRAFT
   ,DLGRET_RESENDCONFIRM
   ,DLGRET_SMSNEW      //add by yangdecai  2010-07-28
   ,DLGRET_SMSVIEWS
   ,DLGGET_SMSNEW_OK
   ,DLGRET_FLASHSMS
   ,DLGGET_FLASHSMS_END
#ifdef FEATURE_USES_MMS
   ,DLGRET_MMSNOTIFY
   ,DLGRET_MMSDELIVERYREPORT
   ,DLGRET_READREPLY
   ,DLGRET_REPORTALLOWED
   ,DLGRET_SENDERVISIBILITY
   ,DLGRET_INBOX_MMS
   ,DLGRET_OUTBOX_MMS
   ,DLGRET_CLEARINBOX_MMS
   ,DLGRET_CLEAROUTBOX_MMS
   ,DLGRET_EFSFULL_MMS
   ,DLGRET_SIZETOOBIG_MMS
   ,DLGRET_DRAFTBOX_MMS
   ,DLGRET_DRAFT_MMS
   ,DLGRET_CLEARDRAFT_MMS
#endif
/*   
   ,DLGRET_INSERTPICTURE //add by xuhui 2011/08/01
   ,DLGRET_INSERTPICTURE_PRESET //add by xuhui 2011/08/01
   ,DLGRET_INSERTPICTURE_NEW //add by xuhui 2011/08/01
   ,DLGRET_INSERTVIDEO //add by xuhui 2011/08/01
   ,DLGRET_INSERTVIDEO_PRESET //add by xuhui 2011/08/01
   ,DLGRET_INSERTVIDEO_NEW //add by xuhui 2011/08/01
   ,DLGRET_INSERTSOUND //add by xuhui 2011/08/01
   ,DLGRET_INSERTSOUND_PRESET //add by xuhui 2011/08/01
   ,DLGRET_INSERTSOUND_NEW //add by xuhui 2011/08/01
   ,DLGRET_INSERTFILE //add by xuhui 2011/08/01*/
} DLGRetValue;

#ifdef FEATURE_USES_MMS
typedef enum
{
    PLAYER_IDLE   =  0,	
	PLAYER_STOP,	 
	PLAYER_PLAY,		
	PLAYER_PAUSE,
	PLAYER_ERROR
} MMSMediaPlayerState;
#endif

#ifdef _MSC_VER
#pragma pack(push,1)
#endif

typedef struct WmsApp
{
    DECLARE_VTBL(IWmsApp)
    uint32                          m_nRefs;
    IModule                         *m_pModule;
    IDisplay                        *m_pDisplay;
    IShell                          *m_pShell;

    IConfig                         *m_pConfig;

    // ----------------------------------------------------
    IWMS                            *m_pwms;
    wms_client_id_type              m_clientId;	

#ifdef FEATURE_ICM
    ICM                             *m_pICM;
#else
    ITelephone                      *m_pITelephone;                         // instance of ICM
#endif
    IStatic                         *m_pStatic;
    IAnnunciator                    *m_pIAnn;
    IALERT                          *m_pAlert;
    IMenuCtl                        *m_pMenu;
    IImage                          *m_pImage;
    //IImage                          *m_pBgImage;
    
    wms_message_mode_e_type         m_mode;
    
    wms_message_index_type          m_wCurindex;                // 当前消息在存储空间中的索引
    wms_memory_store_e_type         m_eCurStore;                // 当前存储空间
    wms_box_e_type                  m_eMBoxType;                // 当前信息箱类型
    AEECallback                     m_callback;                 // 命令回调
    
    // 消息内部数据结构成员
    WMSMessageStruct                m_msCur;                    // 当前正在处理的消息
    WMSMessageStruct                m_msSend;                   // 专注于当前编辑的消息
    
    boolean                         m_bCdmaWmsReady;            // CDMA 短信是否就绪
    wms_memory_status_s_type        m_memruimStatuse;           // ruim 短信存储状态
    wms_routes_s_type               m_routes;
    wms_client_ts_data_s_type       m_CltTsdata;
    wms_cache_info_node             *m_CurMsgNodes[LONGSMS_MAX_PACKAGES];   // 当前处理的消息包列表
    WMSMessageStruct                *m_CurMsgNodesMS[LONGSMS_MAX_PACKAGES]; // 当前处理的消息段列表
    int                             m_idxCur;           // 操作 m_CurMsgNodes 的当前索引号
    wms_client_message_s_type       *m_pCurCltPhrase;   // 用于常用语
    wms_client_message_s_type       **m_pCurSendCltMsg; // 用于发送给特定号码的消息列表
    int                             m_idxCurSend;       // 当前发送的消息在 m_pCurSendCltMsg 中的索引
    int                             m_nSendItems;       // m_pCurSendCltMsg 包含的总项数
    IVector                         *m_pSendList;
    IVector                         *m_pSaveNumList;
    IVector                         *m_pUserDataMOList; // 当前发送的用户数据列表
    int                             m_idxUserdata;      // 当前发送的用户数据在用户数据列表中的索引号
    AEERect                         m_rc; 
    boolean                         m_bMuitiSend;       // 当前是否群发消息

    WMSAPPState                     m_prevState;        // Applet前一状态
    WMSAPPState                     m_currState;        // Applet当前状态
    WMSAPPState                     m_stchkpwdbk;       // Applet密码验证通过后前往状态
    WMSAPPState                     m_stcontinuesendbk; // Applet继续发送短信后前往的状态

    // 进入消息文本编辑状态前的状态
    WMSAPPState                     m_STSwitchToEditMsg;
    
    // 对话框关闭时返回值
    DLGRetValue                     m_eDlgReturn;

    // 当前活动对话框的IDialog接口指针
    IDialog                         *m_pActiveIDlg;

    // 当前活动对话框的 ID 
    uint16                          m_wActiveDlgID;
    
    // 当前活动对话框的事件处理函数指针
    WMSPFNDLGHANDLER                m_pDialogHandler;

    // 前一菜单选择项
    uint16                          m_wPrevMenuSel;
    
    // 主菜单选择项
    uint16                          m_wMainMenuSel;
    
    // 字符串资源 ID
    uint16                          m_wMsgResID;

    // POP 对话框类型ID
    PromptMsg_e_Type                m_ePMsgType;

    WMSAPP_STATUS_e_type            m_eAppStatus;               // Applet 当前运行状态

    Create_SMS_e_Type               m_eCreateWMSType;           // 发送消息的创建类型
    

    WMSAPP_Config_Type              m_cfg;                      //存于文件系统的配置数据
    
    Send_OPT_e_Type                 m_SendOPT;                  //当前发送消息选项
    wms_report_status_e_type        m_SendStatus;               //发送消息返回的状态码
    wms_report_status_e_type        m_GetStatus;               //发送消息返回的状态码
    wms_cdma_tl_status_e_type       m_SendtlStatus;

    boolean                         m_bNaturalStart;            //是否常规启动
    boolean                         m_bDoNotOverwriteDlgResult; //是否不重写对话框结果

    Wms_e_Copy_Type                 m_CopyType; 
    
    Erase_SMS_e_Type                m_eEraseWMSType;            //删除信息模式
    MAKE_MSGList_Mode               m_eMakeListMode;            //建立消息菜单列表的方式
    uint16                          m_wCurPageStarxuhao;        //当前消息页首项节点序号
    uint16                          m_wSelItemxuhao;            //当前消息页选中项节点序号
    
    MAKE_ADDList_Mode               m_eMakeAddListMode;         //建立地址菜单列表的方式
    int                             m_nCurAddPage;              //当前地址页编号
    uint16                          m_CurAddID;                 //当前选中的号码在菜单列表中的 ID
    
    int32                           m_dwInsertPos;              //电话号码插入位置
    uint16                          m_wCurTemplate;             //当前选中的模板
    uint16                          m_wInsertTemplate;          //当前选中插入的模板
    boolean                         m_bAddTemplate;             //当前编辑的模板是否新增
    Insert_Type                     m_eInsertType;              //当前插入操作类型
    
    boolean                         m_bTextFullAlert;           // 是否提示用户 Text is full
    
    boolean                         m_bCallVMNum;               // 是否拨打语音号码
    WMS_OPT_e_type                  m_eOptType;                 // 进入选项的方式
    WMS_Extract_e_type              m_ExtractType;              // 提取取内容类型
    int32                           m_CurExtractItem;           // 当前选中的提取项
    
    boolean                         m_bNeedContinueSend;        // 短信发送被终止后是否需要继续发送
    Wms_e_ContinueSend_Type         m_ContinueSendType;         // 继续发送类型
    IBacklight*                     m_pBacklight;
    char                            *m_pszArgs;                 // 启动参数
    
    boolean                         m_refresh_in_progress;      // UIM 是否在更新短信
    
    RESERVEDMSG_DT                  m_rsvDateTime;              // 预约短信时间
    
    int                             m_nMOmaxChars;              // 用于当前编辑短信时允许输入的最大字符数
    AEETextInputMode                m_CurInputMode;             // 当前编辑短信时使用的输入法
    int                             m_SucNum;                   //发送成功条数
    int                             m_FailNum;                  //发送失败条数
    boolean                         m_CbVal;
    boolean                         m_isPopMenu;                //判断当前是否是pop menu 状态
    boolean                         m_bDCDisconnectedInSending; // 消息发出后在等待结果的过程中 DC 是否被挂断
    int                             m_nDisconnectedInSendingRetryTimes; // 消息发出后在等待结果的过程中 DC 是被挂断后重试发送的次数
    boolean                         m_bSending;                 // 当前是否处于消息
    int                             m_nCopyNum;                 // 当前可移动或复制的短信条数 
    boolean                         m_bSaveFailedMsg;           // 是否保存过发送失败的短信
    char                             *m_strPhonePWD;
    ISound                          *m_pSound;
	boolean                         m_bisSendSecond;           //发送失败是否已经重发
	boolean                         m_bActive;                 //add by yangdecai 2010-07-28
	IRUIM                           *m_pIRUIM;  
	boolean                         m_bincommend;
	boolean                         m_bwriteclr;
	#ifdef FEATURE_FLASH_SMS
	boolean                         m_bflash_sms;              //add by yangdecai 2011-04-01
	#endif
#ifdef FEATURE_USES_MMS      
    MMS_WSP_DEC_DATA                m_DecData;
    MMS_WSP_ENCODE_SEND             m_EncData;
    wms_msg_event_info_s_type       *m_pMsgEvent;
    IImage                          *m_pMMSImage;
    IImage                          *m_pMMSSOUND;
    IImage                          *m_pMMSVIDEO;
    boolean                         m_hasImage;
    boolean                         m_hasSound;
    boolean                         m_hasVideo;
    AECHAR                          *m_MMSData;
    boolean                         m_isMMS;
    IFileMgr                        *m_pIFileMgr;
    boolean                         m_isCheckMMSNotify;
    boolean                         m_isMMSNotify;
    int8                            m_isMMSDeliveryReport;
    int8                            m_isMMSReadReply;
    int8                            m_isMMSReporyAllowed;
    int8                            m_isMMSSenderVisibility;
    int                             m_nDlgID;
    IMedia                          *m_pMedia;
    WSP_MMS_RESOURCE                m_ResData;
    uint8                          m_wSelectStore;
    boolean                        m_pMMSMenuHasFocus;
	MMSMediaPlayerState      	   m_CurrentState;                    /*播放状态*/
    boolean                        m_isForward;
    uint8                          m_insertMMSType;//播放MMS的类型，0：图片。 1：声音。 2：视频. 3:文件
    boolean                         m_isSendToAlbumOrEmain;
#endif    
	AEERect                        m_rContrlRectImage[4];
	AEERect                        m_lContrlRectImage[4];
} WmsApp;

/*==============================================================================
                                 
                                 宏定义和常数
                                 
==============================================================================*/
// 当将卡上消息与话机上消息合并处理时，节点的索引号会在建立消息列表时会产生冲突。
// 因此这里定义一个 RUIM 上消息菜单项 ID 的基点，对于同一索引的菜单项，RUIM 上消
// 息菜单项 ID 的值会比话机的大 RUIM_MSGINDEX_BASE 。
#define RUIM_MSGINDEX_BASE                  (1000)

// 根据 BREW 3 的需要，重定义资源文件宏
#define  AEE_WMSAPPRES_LANGFILE (AEE_RES_LANGDIR WMS_RES_FILE)

// 自动断开业务连接的时间：秒
#define WMSAPP_AUTO_DISCONNECT_TIME         (5)

// 对于长短信，未收到的数据包的文本提示内容
#ifdef FEATURE_SUPPORT_ID
#define DATA_NOT_RECEIVED    "<...>"
#else
#define DATA_NOT_RECEIVED    "(Data not received)"
#endif
#if defined(FEATURE_VERSION_C306)||defined(FEATURE_VERSION_W0216A)
#define AVK_SEND_TWO     2
#elif defined(FEAUTRE_VERSION_N450)|| defined(FEATURE_VERSION_N021)|| defined(FEATURE_VERSION_C01)
#define AVK_SEND_TWO     2
#else
#define AVK_SEND_TWO     1
#endif

// 调用接口的目的
#define STARTARGPREFIX_SENDTEXTMESSAGE      'S'
#define STARTARGPREFIX_WRITENEWMESSAGE      'W'
#define STARTARGPREFIX_SHOWINBOXLIST        'L'
#define STARTARGPREFIX_VIEWVMAIL            'V'
#define STARTARGPREFIX_POWERONSTART         'P'

// 内部非接口调用启动-预约短信启动
#define STARTARGPREFIX_RESERVEDMSGSTART     'R'

// 一个菜单项在LCD上能同时显示的数字字符个数
#define  MAXCHARS_ONEMENUITEMCANDISPLAY     (16)

#ifdef FEATURE_VERSION_X3
// 建立消息列表菜单时一页的最大项目数
#define MAXITEMS_ONEPAGE                    (9)
#else
// 建立消息列表菜单时一页的最大项目数
#define MAXITEMS_ONEPAGE                    (7)
#endif
// 限制群发时最多允许发往的地址数
#define MAXNUM_MULTISEND                    (20)

// 短信内部配置数据结构版本号
#define WMS_CFG_VERSION                     (2)

// 消息菜单项ID基数
#define MSG_CMD_BASE                        (5000)
#ifdef FEATURE_APP_MUSICPLAYER
#define TIME_MS_SMSVIBRATE                  (2000)
#endif
// 此宏用当前状态更新先前状态，再用nextState状态更新当前状态
#define MOVE_TO_STATE(nextState)            \
{                                           \
    WMSAPPState tpState;                    \
    tpState = nextState;                    \
    pMe->m_prevState = pMe->m_currState;    \
    pMe->m_currState = tpState;             \
}

//关闭对话框的宏
#define CLOSE_DIALOG(dlgResult)             \
{                                           \
    pMe->m_eDlgReturn = dlgResult;          \
    (void) ISHELL_EndDialog(pMe->m_pShell); \
}

#ifdef _MSC_VER
#pragma pack(pop)
#endif


/*==============================================================================

                                 函数声明

==============================================================================*/
/*==============================================================================
函数:
    WmsApp_StartSendingMessage

说明:
    使用 IWMS 接口，开始发送消息。

参数:
    pMe [in]:指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    无。

备注:
    消息发送方式视消息长度而定，消息过长，不作从接入信道发送消息的试探。
        
==============================================================================*/
int WmsApp_StartSendingMessage(WmsApp *pMe, boolean  bIsMultiSend);

/*==============================================================================
函数:
    WmsApp_MultSendMsgTimer

说明:
    群发信息时，消息发送间的间隔时间定时器，用于启动下一条消息的发送。

参数:
    pme [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值:
    无。

备注:

==============================================================================*/
void WmsApp_MultSendMsgTimer(void *pme);

/*==============================================================================
函数:
    WmsApp_ReSendMsgTimer

说明:
    重发信息时，消息发送间的间隔时间定时器，用于启动下一消息数据包的发送。

参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值:
    无。

备注:

==============================================================================*/
void WmsApp_ReSendMsgTimer(void *pme);

/*==============================================================================
函数:
    WmsApp_SendReservedMsgTimer

说明:
    发送预约信息时，消息发送间的间隔时间定时器，用于启动下一消息数据包的发送。

参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值:
    无。

备注:

==============================================================================*/
void WmsApp_SendReservedMsgTimer(void *pme);

/*==============================================================================
函数：
    WmsApp_GetAddItem

说明：
    从链表中取得指定索引的号码。

参数：
    pIV [in]：指向IVector接口对象的指针。
    wIdx [in]：索引号。

返回值：

备注:
       
==============================================================================*/
CMultiSendItemInfo * WmsApp_GetAddItem(IVector *pIV, uint16 wIdx);


/*==============================================================================
函数:
    WmsApp_UpdateMenuList

说明:
    更新当前 pMe->m_eMBoxType 指向的邮箱的消息菜单列表。

参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    pMenu [in/out]: IMenuCtl接口指针。

返回值:
    none

备注:
    New
        
==============================================================================*/
void WmsApp_UpdateMenuList(WmsApp *pMe, IMenuCtl *pMenu);

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
NextFSMAction WmsApp_ProcessState(WmsApp *pMe);

/*==============================================================================
函数:
    WmsApp_ShowDialog

说明:
    函数由状态处理函数用来唤起相应对话框资源ID的对话框。

参数:
    pMe [in]:指向 WMS Applet 对象结构的指针。该结构包含小程序的特定信息。
    dlgResId [in]:对话框资源ID，函数根据该ID创建对话框。

返回值:
    none

备注:

==============================================================================*/
void WmsApp_ShowDialog(WmsApp  *pMe,  uint16 dlgResId);

/*==============================================================================
函数:
    WmsApp_ShowMsgBox

说明:
    函数由状态处理函数用来唤起消息对话框，显示消息提示用户。

参数:
    pMe [in]: 指向 WMS Applet 对象结构的指针。该结构包含小程序的特定信息。
    wTextResId [in]: 消息文本资源 ID。

返回值:
    none

备注:

==============================================================================*/
void WmsApp_ShowMsgBox(WmsApp *pMe, uint16 wTextResId);

/*==============================================================================
函数:
    WmsApp_SetDialogHandler
       
说明:
    函数根据 pMe->m_wActiveDlgID 设定对话事件处理函数指针 m_pDialogHandler 相应值。

参数:
    pMe [In]: 指向 WMS Applet 对象结构的指针。该结构包含小程序的特定信息。

返回值:
    none

备注:
    若找不到对应的对话框处理函数, m_pDialogHandler 被置为 NULL
==============================================================================*/
void WmsApp_SetDialogHandler(WmsApp *pMe);

/*==============================================================================
函数:
    WmsApp_RouteDialogEvt

说明:
    函数将 BREW 事件路由给当前活动对话框的事件处理函数。

参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    eCode [in]: 事件代码。
    dwParam [in]: 与事件eCode关联的数据。

返回值:
    TRUE:传入事件被处理。
    FALSE:传入事件没被处理。

备注:

==============================================================================*/
boolean WmsApp_RouteDialogEvt(WmsApp *pMe, 
                              AEEEvent eCode, 
                              uint16  wParam, 
                              uint32  dwParam);

/*==============================================================================
函数: 
    WmsApp_wmscommandreport
       
说明: 
    WMS 用此函数向命令发出者汇报命令处理结果。
       
参数: 
    cmd [in]: wms 命令类型。
    user_data[in]: 发布命令时传入的用户数据指针。
    cmd_err [in]: 命令状态或错误类型。
    
返回值:
    none
       
备注:

==============================================================================*/
void WmsApp_wmscommandreport(wms_cmd_id_e_type   cmd,
                             void                *user_data,
                             wms_cmd_err_e_type  cmd_err);

/*==============================================================================
函数:
    WmsApp_FreeSaveNumList

说明:
    释放链表中的全部数据。

参数:
    pMe [in]: 指向 WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    none

备注:
       
==============================================================================*/
void WmsApp_FreeSaveNumList(WmsApp *pMe);

/*==============================================================================
函数:
    WmsApp_FreeMultiSendList

说明:
    释放链表中的全部数据。

参数:
    pIV [in]: 指向IVector接口对象的指针。

返回值:
    none

备注:
       
==============================================================================*/
void WmsApp_FreeMultiSendList(IVector *pIV);

/*==============================================================================
函数:
    WmsApp_FreeMsgNodeMs

说明:
    函数释放 WmsApp 结构中 m_CurMsgNodesMS 成员保存的数据。

参数:
    pMe [in]: 指向 WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    none

备注:
       
==============================================================================*/
void WmsApp_FreeMsgNodeMs(WmsApp *pMe);

/*==============================================================================
函数:
    WmsApp_FreeSendClentMsgList

说明:
    释放发送客户消息表中的全部数据项。

参数:
    pMe [in]: 指向 WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    none

备注:
       
==============================================================================*/
void WmsApp_FreeSendClentMsgList(WmsApp * pMe);

/*==============================================================================
函数:
    WmsApp_CombinateMsg

说明:
    函数将 WmsApp 结构中 m_CurMsgNodesMS 成员保存的消息数据合并到 m_msCur 中。

参数:
    pMe [in]: 指向 WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    none

备注:
       
==============================================================================*/
void WmsApp_CombinateMsg(WmsApp *pMe);

/*==============================================================================
函数:
    WmsApp_CopyCurMsgForSend

说明:
    函数将当前处理的消息数据复制到 m_msSend 中。

参数:
    pMe [in]: 指向 WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    none

备注:
       
==============================================================================*/
void WmsApp_CopyCurMsgForSend(WmsApp *pMe);

/*==============================================================================
函数:
    WmsApp_FreeUserDataMOList

说明:
    函数释放 WmsApp 结构中 m_pUserDataMOList 成员保存的数据。

参数:
    pIV [in]: 指向IVector接口对象的指针。

返回值:
    none

备注:
       
==============================================================================*/
void WmsApp_FreeUserDataMOList(IVector *pIV);

/*==============================================================================
函数:
    WmsApp_PrepareUserDataMOList

说明:
    函数将当前发送的消息数据 m_msSend 转换到 m_pUserDataMOList 中。

参数:
    pMe [in]: 指向 WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    none

备注:
       
==============================================================================*/
void WmsApp_PrepareUserDataMOList(WmsApp *pMe);

/*==============================================================================
函数:
    WmsApp_GetClientMsgMO

说明:
    函数根据 wms_cdma_user_data_s_type 结构消息创建 wms_client_message_s_type 结
    构 MO 消息。

参数:
    pMe [in]: 指向 WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    none

备注:
    返回数据 buffer 系动态分配，由调用者负责释放。

==============================================================================*/
wms_client_message_s_type *WmsApp_GetClientMsgMO(WmsApp *pMe, boolean bSend);

/*==============================================================================
函数:
    WmsApp_BuildSendClentMsgList

说明:
    建立发送客户消息列表。

参数:
    pMe [in]: 指向 WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    none

备注:
       
==============================================================================*/
void WmsApp_BuildSendClentMsgList(WmsApp * pMe);

/*==============================================================================
函数:
    WmsApp_ProcessStatus

说明:
    函数处理来自 wms 的报告信息。

参数:
    pMe [in]: 指向 WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    pRptInfo [in]: wms_submit_report_info_s_type 结构指针。

返回值:
    none

备注:

==============================================================================*/
void WmsApp_ProcessStatus(WmsApp *pMe, wms_submit_report_info_s_type *pRptInfo);


#ifdef FEATURE_USES_MMS
/*==============================================================================
函数:
    WmsApp_ProcessMMSStatus

说明:
    函数处理来自 wms 的报告信息。

参数:
    pMe [in]: 指向 WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    none

备注:

==============================================================================*/

void WmsApp_ProcessMMSStatus(WmsApp *pMe);

#endif

/*==============================================================================
函数:
    WmsApp_MemberReset

说明:
    函数对 WmsApp 结构部分成员变量进行重置。

参数:
    pMe [in]: 指向 WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    none

备注:

==============================================================================*/
void WmsApp_MemberReset(WmsApp *pMe);

/*==============================================================================
函数：
    WmsApp_UpdateAnnunciators

说明：
    基于当前新来短信(包含语音邮件)数，更新提示图标。

参数：
    pMe [in]: 指向 WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
    无。

备注：
    即使Applet不激活也会更新。语音通知消息图标优先显示。
        
==============================================================================*/
void WmsApp_UpdateAnnunciators(WmsApp * pMe);

/*==============================================================================
函数：
    WmsApp_GetmemAlertID

说明：
    函数获取当前存储空间告警字符串在资源文件中的 ID。

参数：
    pMe [in]: 指向 WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    eBox[in]: wms_box_e_type

返回值：
    告警字符串在资源文件中的 ID。

备注：
    若返回值为0，表示不需要告警
        
==============================================================================*/
uint16 WmsApp_GetmemAlertID(WmsApp * pMe, wms_box_e_type eBox);

/*==============================================================================
函数：
    WmsApp_PlaySMSAlert

说明：
    函数用来播放短信提示。

参数：
    pMe [in]: 指向 WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    bsmsin[in]: TRUE - 来信  FALSE - 去信

返回值：
    none

备注：
        
==============================================================================*/
void WmsApp_PlaySMSAlert(WmsApp * pMe, boolean bsmsin);

/*==============================================================================
函数：
    WmsApp_CurmessageIsFullSendout

说明：
    函数用来判断当前发送的消息是否发送完毕。

参数：
    pMe [in]: 指向 WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
    TRUE - 完毕
    FALSE - 尚未完毕

备注：
        
==============================================================================*/
boolean WmsApp_CurmessageIsFullSendout(WmsApp * pMe);

/*==============================================================================
函数：
    WmsApp_IsNeedContinueSendTask

说明：
    函数用来判断是否需要继续未尽短信发送任务。

参数：
    pMe [in]: 指向 WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
    TRUE - 有未尽短信发送任务，需继续
    FALSE - 无未尽短信发送任务

备注：
    是否有未尽短信发送任务，判断条件依次为:
    1、调用本函数时当前状态必需处于 WMSST_SENDING ;
    2、发送到特定号码的短信数据包是否发送完毕;
    3、群发是否发送完毕;
        
==============================================================================*/
boolean WmsApp_IsNeedContinueSendTask(WmsApp * pMe);

/*==============================================================================
函数：
    WmsApp_ReservedMsgSetTimer

说明：
    函数用来检查预约短信列表, 对于过期的短信需修改其标记, 若有有效预约短信，就设
    预约短信定时器。

参数：
    pMe [in]: 指向 WMS Applet 对象结构的指针。该结构包含小程序的特定信息。

返回值：
    none

备注：
        
==============================================================================*/
void WmsApp_ReservedMsgSetTimer(WmsApp * pMe);


#ifdef FEATURE_USES_MMS
/*==============================================================================
函数:
    WmsApp_UpdateMenuList_MMS

说明:
    更新当前 pMe->m_eMBoxType 指向的邮箱的消息菜单列表。

参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    pMenu [in/out]: IMenuCtl接口指针。

返回值:
    none

备注:
    New
        
==============================================================================*/
void WmsApp_UpdateMenuList_MMS(WmsApp *pMe, IMenuCtl *pMenu);
#endif
void WMSDialog_KeypadLock(boolean block);

#endif // end WMSAPP_PRIV_H

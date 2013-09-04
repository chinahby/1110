#ifndef UTKPRIV_H
#define UTKPRIV_H

/*==============================================================================
// 文件：
//        UTK_priv.h
//        2007-11-13 陈喜玲草拟版本(Draft Version)
//        版权所有(c) 2007 Anylook
//        
// 文件说明：
//        
// 作者：2007-11-13
// 创建日期：陈喜玲
// 当前版本：Draft
==============================================================================*/

/*==============================================================================
修改历史记录：
时       间      修 改 人     修改内容、位置及原因
-----------      ----------     -----------------------------------------------
07-11-13         陈喜玲         初始版本
==============================================================================*/


/*==============================================================================
                                 本文件包含的外部文件
==============================================================================*/
#ifndef AEE_SIMULATOR
#include "OEMFeatures.h"   //Handset Feature Definitions
#else
#include "BREWSimFeatures.h"   //Simulator Feature Definitions
#endif
#include "OEMClassIDs.h"
#ifndef AEECLSID_APP_UTK
      #error AEECLSID_APP_UTK must be defined
#endif
#include "OEMFeatures.h"
#include "AEE.h"
#include "utk.brh"
#include "AEEShell.h"
#include "AEEModTable.h"
#include "AEEStdLib.h"
#include "AEEModGen.h"
#include "AEEAppGen.h"
//#include "OEMMacros.h"
#include "AEEText.h"
#include "AEEMenu.h"
#include "AppComFunc.h"
#include "UTK.h"
#include "uimtk.h"
//#include "OEMEvents.h"
#include "UTKUtils.h"
#ifdef CUST_EDITION
#include "custuim.h"
#else
#include "custruim.h"
#endif
//#include "uiutils.h"
#include "AEE_OEM.h"
#include "AEEConfig.h"
//#include "AEEAlert.h"         
#include "AEEPhone.h"
#include "Cai.h"
#include "AEEAlert.h"
#include "Wmsi.h"
#include "Wms.h"
#include "Wmstscdma.h"
#include "AEEWMS.h"
#include "mccdma.h"  
#include "ui.h"

#include "Appscommon.h"
#include "appscommonimages.brh"
#include "OEMCFGI.h"
#include "CallApp.h"
#ifdef FEATURE_ICM
#include "AEECM.h"
#else
#include "AEETelephone.h"
#include "AEETelDef.h"
#endif
#include "OEMSound.h"
#include "OEMConfig.h"
#include "AEEAnnunciator.h"


/*==============================================================================
                                 类型定义
==============================================================================*/
#define MAX_STR_ACHARS      256 
#define OEM_IME_DIALOG   (0x7FFF)
#define TP_STARKEYSWITCH TP_STARKEY_SWITCH
#define TP_HIDEMODETITLE 0x00000000 //nothing match,0x00008000
#define SetUniMenuConfig(p)         //do nothing SetUniMenuConfig(p)
#define SetUniTextConfig(pT)        //do nothing SetUniTextConfig(pT)
#define HEIGHT_PROMPT_BAR               (0)

typedef struct
{
    wms_cmd_id_e_type           cmd;
    wms_cmd_err_e_type          cmd_err;
} wmsutk_cmd_status_type;

/*----------------------对话框相关数据类型声明---------------------*/
// 对话框关闭时返回值列表
typedef enum DLGRetValue
{
   // 初始值，表明需要创建对话框
   DLGRET_CREATE,
   DLGRET_MSGBOX_OK,
   // 对话框关闭时可返回的通用值
   DLGRET_OK,
   DLGRET_POPMSG,
   DLGRET_CANCELED
} DLGRet_Value_e_Type;

/*----------------------状态机相关数据类型声明---------------------*/
// UTK Applet 状态机状态：
typedef enum _UTKState
{
   UTKST_NONE,
   UTKST_INIT,
#ifdef FEATURE_CARRIER_CHINA_TELCOM   
   UTKST_SPLASH,
#endif //FEATURE_CARRIER_CHINA_TELCOM   
   UTKST_MAIN,
   UTKST_LIST,
   UTKST_INPUT,
   UTKST_DISPLAY,
   UTKST_SENDMSG,
   UTKST_MSG,  
   UTKST_POPMSG,
   UTKST_PLAYTONE,    
   UTKST_EXIT
} UTKState;

// 状态处理函数返回给状态处理主函数的值类型
typedef enum NextFSMAction
{
   NFSMACTION_WAIT,
   NFSMACTION_CONTINUE
} NextFSMAction;

// 保存模块信息和模块引用计数的结构
typedef struct UTKMod
{
    DECLARE_VTBL(IModule)
    uint32   m_nRefs;
} UTKMod;

// UTK Applet对象结构体：
typedef struct _CUTK
{
    DECLARE_VTBL(IUTK)
    uint32          m_nRefs;          // 对象引用计数器
    IModule        *m_pModule;
    IDisplay        *m_pDisplay;
    IShell            *m_pShell;

    IALERT    *m_pAlert;

    IConfig        *m_pConfig;

#ifdef FEATURE_ICM
    ICM            *m_pCM;
#else
    ITelephone            *m_pITelephone;
	IPhoneCtl             *m_pIPhoneCtl;
#endif

#ifdef FEATURE_UIM_TOOLKIT_UTK
    uim_tk_proactive_cmd_enum_type   cmd_type;
#endif
    boolean          m_bUserMode;
    int          m_nInputMode;

    AEERect       m_rc;
    // 当前活动对话框IDialog接口指针
    IDialog             *m_pActiveDlg;
    // 当前活动对话框ID
    uint16 m_pActiveDlgID;
    // 活动对话框关闭时的返回结果
    DLGRet_Value_e_Type   m_eDlgRet;
    // 是否不改写对话框返回结果，一般情况需要改写(FALSE)。
    boolean m_bNotOverwriteDlgRet;
    // Applet 前一状态
    UTKState m_ePreState;
    // Applet 当前状态
    UTKState m_eCurState;
    // Applet是否处于挂起状态
    boolean m_bSuspending;    
    
    boolean  m_bAppIsReady;

    RGBVAL  m_dwOldBkColor;
    //用来做文本输入的备份，防止挂起返回后原来输入的文字丢失
    AECHAR  bInputeBackup[MAX_STR_ACHARS + 2];

    //文本输入框是否需要备份开关
    boolean  InputeBackupNeeded;

    uint32   select_item[20];
    uint32   level;
    boolean  clr_back;
    uint16              m_wSelectStore; 
    
    uint32  m_dwTimeOut;
    
    // 用来判断是否正常退出 UTK
    byte                m_btCursel;

    AECHAR  *m_wszInputTitle;
    // Applet 返回状态
    UTKState   m_eMsgRetState;
    uint32     m_MsgTimeout;
    uint16     m_wMsgID;        // message resource ID
    uint16     m_wTitleIndex;
    IPhone     *m_pIPhone;      // Phone interface   
    AECHAR     *m_wszInputText;
    int        m_TextMaxLen;
    int        m_TextMinLen;    
    byte       m_TextQS;  // Command Qualifier  
    uint16     m_PlaytoneMsgID; 
    byte       m_PlaytoneToneID;
    boolean    m_bSendingSMSBG;
    IWMS       *m_pwms;  
    wms_client_id_type m_clientId;  
    AEECallback m_callback;  // 命令回调
    wms_report_status_e_type        m_SendStatus; //发送消息返回的状态码
    wms_cdma_tl_status_e_type       m_SendtlStatus;
    RGBVAL     m_themeColor;    
    IAnnunciator *m_pIAnn;
} CUTK;

/*==============================================================================
                                 宏定义和常数
==============================================================================*/
#define UTK_RES_FILE_LANG      (AEE_RES_LANGDIR UTK_RES_FILE)

#define UTK_DATA_OFF                    2
#define MAX_STR_CHARS                   256

#define ALPHA_8BITS                     0x04
//#define ALPHA_7BITS                       0x00
#define ALPHA_UNICODE                   0x08
#define UNICODE_FLAG                               0x80

// 空菜单选择号
#define MENU_SELECT_NULL                (0xFFFF)
#define TIMEOUT_MS_MSGBOX               (3000)
#define TIMEOUT_MS_QUICK                (300)

// 为防止用户快速按键而添加事件EVT_APPISREADY，事件由定时器
// UTK_APPIsReadyTimer发出
#define APPISREADY_TIMER       100           
#define EVT_APPISREADY   (EVT_USER+1)         

#define EVT_DIALOGTIMEOUT (EVT_USER+3)

#define EVT_OUTAPPDISPLAYDONE (EVT_USER+4)

// 发送短消息时，由状态回调函数发出的事件，已通知发送消息的结果
#define EVT_SEND_STATUS (EVT_USER+12)

// 状态移动宏：先将前一状态用当前状态更新，再将当前状态设为nextState
#define MOVE_TO_STATE(nextState) {            \
            UTKState tpState;            \
            tpState = nextState;          \
            pMe->m_ePreState = pMe->m_eCurState; \
            pMe->m_eCurState = tpState;        \
        }

// 关闭对话框宏
#define CLOSE_DIALOG(DlgRet)  {    \
                                     pMe->m_eDlgRet = DlgRet; \
                                     (void) ISHELL_EndDialog(pMe->m_pShell);  \
                                 }
// 画界面底部提示条宏定义
#define DRAW_BOTTOMBAR(x)                           \
{                                                   \
    BottomBar_Param_type BarParam;                  \
    MEMSET(&BarParam, 0, sizeof(BarParam));         \
    BarParam.eBBarType = x;                         \
    DrawBottomBar(pMe->m_pDisplay, &BarParam);      \
}
/*==============================================================================
                                 全局数据
==============================================================================*/

/*==============================================================================
                                 本地（静态）数据
==============================================================================*/

/*==============================================================================
                                 函数声明
==============================================================================*/
/*----------------------对话框相关函数声明---------------------*/
/*==============================================================================
函数:
       UTK_ShowDialog
说明:
       函数由状态处理函数用来创建相应对话框资源ID的对话框。

参数:
       pMe [In]: 指向UTK Applet对象结构的指针,该结构包含小程序的特定信息.
       dlgResId [in]：对话框资源ID，函数根据该ID创建对话框。

返回值:
       无

备注:
       

==============================================================================*/
void UTK_ShowDialog(CUTK *pMe,uint16  dlgResId);

/*==============================================================================
函数:
       UTK_RouteDialogEvent
说明:
       函数将BREW事件路由给当前活动对话框的事件处理函数。

参数:
       pMe [In]: 指向UTK Applet对象结构的指针,该结构包含小程序的特定信息.
       eCode [in]：事件代码。
       wParam [in]：与事件eCode关联的数据。
       dwParam [in]：与事件eCode关联的数据。

返回值:
       TRUE：传入事件被处理。
       FALSE：传入事件没被处理。

备注:

==============================================================================*/
boolean UTK_RouteDialogEvent(CUTK *pMe,
    AEEEvent    eCode,
    uint16  wParam,
    uint32 dwParam
);

/*----------------------状态机相关函数声明---------------------*/
/*==============================================================================
函数:
       UTK_ProcessState
说明:
       UTK Applet状态处理主函数。函数根据pMe->m_currState将调用路由至
       相应的处理函数。

参数:
       pMe [In]: 指向UTK Applet对象结构的指针,该结构包含小程序的特定信息.

返回值:
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注:

==============================================================================*/
NextFSMAction UTK_ProcessState(CUTK *pMe);

/*==============================================================================
函数: 
       ByteToUnicode
       
说明: 
       本函数将字节数据转换为 Unicode 数据。
       
参数: 
       pUnicodeBuf [in]：IMenuCtl 接口指针。
       pBytBuf [out]： 菜单标题
       cmd_type： uim_tk_proactive_cmd_enum_type
       
返回值:
       无。
       
备注:
       
==============================================================================*/
void ByteToUnicode(AECHAR *pUnicodeBuf, 
    byte *pBytBuf, 
    byte bytLen, 
    int mode
);

/*==============================================================================
函数：
       UTK_MsgSendStatusCallBack
说明：
       传给BREW的回调函数。由BREW调用用来通知Applet短消息发送状态。

参数：
       pUser {in]：void类型指针(此处使用CUTK 结构指针)
       status [in]：消息发送返回的结果码。
       
返回值：
       无。

备注：:
       以比BREW更高的优先级在CM环境运行。用以反馈正确发送消息结果给用户。

==============================================================================*/
void UTK_MsgSendStatusCallBack(void * pUser, int status);

/*==============================================================================
函数：
       UTK_MessageAlert

说明：
       用来播放发短消息成功提示音。

参数：
       pMe [in]：指向UTK Applet对象结构的指针。该结构包含小程序的特定信息。
       bPlay [in]: 播放指示，TRUE: 播放； 否则，停止播放。
       
返回值：
       无。

备注：:
       
==============================================================================*/
void UTK_MessageAlert(CUTK * pMe, boolean  bPlay); 

/*==============================================================================
函数: 
       UTK_APPIsReadyTimer
       
说明: 
       定时器回调函数。主要用于控制用户操作速度。
       
参数: 
       pme [in]：void *类型指针，暗指CUTK结构指针。
       
返回值:
       无。
       
备注:
       
==============================================================================*/
void UTK_APPIsReadyTimer(void *pme);

/*==============================================================================
函数: 
    UTK_GiveResponse
       
说明: 
    本函数用来响应对当前 proactive command 的处理结果。
       
参数: 
    pMe [In]: 指向UTK Applet对象结构的指针,该结构包含小程序的特定信息。
    eCmd [in]: uim_tk_proactive_cmd_enum_type
    bForwad[in]: 指示是否需要卡发出下一步命令
    eResult [in]: 对当前 proactive command 的处理结果
       
返回值:
    none
       
备注:
    注意：当前 proactive command 就是 CUTK 成员变量cmd_type。eResult 仅是一个简单
    结果。若要以大量数据响应，请以 CUTK 成员变量 方式传入

==============================================================================*/
void UTK_GiveResponse(CUTK * pMe, 
                      uim_tk_proactive_cmd_enum_type eCmd,
                      boolean  bForwad, 
                      uim_tk_general_result_type eResult
                      );

void UTK_GiveResponseEx(CUTK * pMe, 
                        uim_tk_proactive_cmd_enum_type eCmd,
                        boolean  bForwad, 
                        uim_tk_general_result_type eResult,
                        byte addinfo
                        );

/*==============================================================================
函数: 
    CUTK_SetUTKMenu
       
说明: 
    本函数针对命令 Setup Menu 和 Select Item 命令, 分析相应于命令的字节数据流，
    建立菜单列表。
       
参数: 
    pMe [in]：指向UTK Applet对象结构的指针。该结构包含小程序的特定信息。
    pMenu [in]：IMenuCtl 接口指针。
    pwszTitle [out]： 菜单标题(需保存时传入)
    cmd_type： uim_tk_proactive_cmd_enum_type
       
返回值:
    菜单项数
       
备注:
    本函数只处理 Setup Menu 和 Select Item 命令的菜单的初始化。
==============================================================================*/
int CUTK_SetUTKMenu(CUTK *pMe, IMenuCtl *pMenu, 
    uim_tk_proactive_cmd_enum_type cmd_type
);


/*==============================================================================
函数: 
    UTK_ProcessSendCDMASMSCmd
       
说明: 
    本函数处理 CDMA SEND SMS 命令。
       
参数: 
    pMe [In]: 指向UTK Applet对象结构的指针,该结构包含小程序的特定信息。
    pCltMsg [in/out]: 指向 wms_client_message_s_type 结构的指针，用于返回待发
                      消息的 wms_client_message_s_type 结构地址。
    wszAlpha [in/out]: 用于返回 Alpha 标识字符串的buffer
    nAlphabufLen [in]: 返回 Alpha 标识字符串的buffer 的大小，单位为 AECHAR                    
       
返回值:
    none
       
备注:
    如返回值为 SUCCESS , 并且 *pCltMsg 等于 NULL , 表示消息不需要发送(特殊要求)

==============================================================================*/
int UTK_ProcessSendCDMASMSCmd(CUTK *pMe, 
    wms_client_message_s_type ** pCltMsg,
    AECHAR *wszAlpha,
    int nAlphabufLen
);

/*==============================================================================
函数: 
    UTKApp_LocalInformation
       
说明: 
    本函数针对命令 Local Information 命令, 分析相应于命令的字节数据流，
    建立菜单列表。
       
参数: 
    pMe [in]：指向UTK Applet对象结构的指针。该结构包含小程序的特定信息。
    pMenu [in]：IMenuCtl 接口指针。
       
返回值:
    菜单项数
       
备注:
    本函数只处理 Setup Menu 和 Select Item 命令的菜单的初始化。
==============================================================================*/
void UTKApp_LocalInformation(CUTK *pMe,Local_Information *pLocalInformation);

/*==============================================================================
函数: 
    UTK_wmscommandreport
       
说明: 
    wms 用此函数向命令发出者汇报命令处理结果。
       
参数: 
    cmd [in]: wms 命令类型。
    user_data[in]: 发布命令时传入的用户数据指针。
    cmd_err [in]: 命令状态或错误类型。
    
返回值:
    NULL 或 符合条件的节点指针。
       
备注:

==============================================================================*/
void UTK_wmscommandreport(wms_cmd_id_e_type   cmd,
                          void                *user_data,
                          wms_cmd_err_e_type  cmd_err);
                          
/*==============================================================================
函数: 
    UTK_MsgCb
       
说明: 
    本函数为所有 MSG 相关事件的回调函数。
       
参数: 
    event [In]: CFG 相关事件 。
    pInfo [In]: wms_msg_event_info_s_type 结构指针，存有事件相关信息。
    
       
返回值:
    none
       
备注:
    Posts the Event to itself

==============================================================================*/
void UTK_MsgCb(wms_msg_event_e_type       event,
               wms_msg_event_info_s_type  *pInfo,
               boolean                    *shared);
               
/*==============================================================================
函数:
    UTK_ProcessStatus

说明:
    函数处理来自 wms 的报告信息。

参数:
    pMe [in]: 指向 UTK Applet对象结构的指针。该结构包含小程序的特定信息。
    pRptInfo [in]: wms_submit_report_info_s_type 结构指针。

返回值:
    none

备注:

==============================================================================*/
void UTK_ProcessStatus(CUTK *pMe, wms_submit_report_info_s_type *pRptInfo);

#endif
//UTKPRIV_H


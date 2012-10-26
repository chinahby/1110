#ifndef RECENTCALLPRIV_H
#define RECENTCALLPRIV_H

/*==============================================================================
// 文件：
//        recentcalls_priv.h
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
07-11-18         陈喜玲       初始版本
==============================================================================*/


/*==============================================================================
                                 本文件包含的外部文件
==============================================================================*/
#ifndef AEE_SIMULATOR
#include "customer.h"   //Handset Feature Definitions
#else
#include "BREWSimFeatures.h"   //Simulator Feature Definitions
#endif

#if !defined(BREW_STATIC_APP)
 Use the .bid file if compiling as dynamic applet
   #include "recentcalls.bid"
#else
   #include "OEMClassIds.h"  // Applet Ids
   #ifndef AEECLSID_APP_RECENTCALL
      #error AEECLSID_APP_RECENTCALL must be defined
   #endif
#endif

#include "recentcalls.brh"
#include "AEEShell.h"
#include "AEEModTable.h"
#include "AEEStdLib.h"
#include "AEEModGen.h"
#include "AEEAppGen.h"
#include "OEMClassIDs.h"
#include "recentcalls.h"
#include "AEEMenu.h"
#include "AeeTime.h"
#include "AeeDate.h"
//#include "AEECallList.h"
#include "AEECallHistory.h"
#include "AEECallHistoryExt.h"
#include "AEEConfig.h"
#include "OEMCFGI.h"
#include "AppComFunc.h"
#ifdef FEATURE_USES_ZI
#include "OEMZIText.h"
#else
#include "OEMText.h"
#endif
#include "OEMFeatures.h"
#if defined(FEATURE_WMS_APP) && !defined(FEATURE_WMSAPP_ONLYSUPPORTVMAIL)
#include "WMSApp.h"
#endif
#include "ContApp.h"
#include "CallApp.h"
#include "Appscommon.h"
#include "AEE.h"

#include "AEEText.h"
#include "AEEAnnunciator.h"
#ifdef FEATURE_LED_CONTROL
#include "AEEBacklight.h"
#endif



/*==============================================================================
                                 类型定义
==============================================================================*/
/*----------------------对话框相关数据类型声明---------------------*/
// 对话框关闭时返回值列表
typedef enum DLGRetValue
{
   // 初始值，表明需要创建对话框
   DLGRET_CREATE,

   // 对话框IDD_SOUNDMENU_DIALOG返回特定值
   DLGRET_MISSEDCALL,
   DLGRET_INCOMCALL,
   DLGRET_OUTGCALL,
   DLGRET_TIMEMENU,
   DLGRET_DELMENU,
   
   // 对话框IDD_RLISTRECORD返回特定值
   DLGRET_RECORDDEAL,

   // 对话框IDD_RTIME_MENU返回特定值
   DLGRET_TIME,

   // 对话框IDD_RTIME_MENU返回特定值
   DLGRET_WARN,
   
   // 对话框IDD_RDEAL_RECORD返回特定值
   DLGRET_DETAIL,
   DLGRET_DELONE,

   //对话框IDD_RTIME返回特定值
   DLGRET_TIMECLEAR,
   
   // 对话框关闭时可返回的通用值
   DLGRET_OK,
   DLGRET_CANCELED,
   
   // IDD_VERIFY_PASSWORD, 
   DLGRET_VERIFY_PASSWORD_PASSED,

   DLGRET_INPUT_INVALID,

   DLGRET_MSGBOX_OK,
   DLGRET_MAX_WMS_CONTACTS,

   DLGRET_REFRESH,
   DLGRET_VALIDPINPASS,
   DLGRET_VALIDPINFAILED
   
} DLGRetValue;

/*----------------------状态机相关数据类型声明---------------------*/
// recentcalls Applet 状态机状态：
typedef enum _recentcallsState
{
    STATE_VERIFY_PASSWORD,
    STATE_RMAINMENU,
    STATE_RECORD_LIST,
    STATE_RTIME_MENU,
    STATE_RDEL_MENU,
    STATE_RECORDDEAL,
    STATE_RWARN,
    STATE_RTIME,
    STATE_DETAIL,
    STATE_REXIT,
    STATE_SELECT_RETURN,
    STATE_ASKPASSWORD
} recentcallsState;

// 状态处理函数返回给状态处理主函数的值类型
typedef enum NextFSMAction
{
   NFSMACTION_WAIT,
   NFSMACTION_CONTINUE
} NextFSMAction;

// 保存模块信息和模块引用计数的结构
typedef struct recentcallsMod
{
    DECLARE_VTBL(IModule)
    uint32   m_nRefs;
} recentcallsMod;
/*
typedef struct _Record{
    AECHAR *number;
    AECHAR *name;
    boolean is_number;
    uint16 category;
    uint32 time_stamp;                 // years to seconds
    //dword time_stamp;                 // years to seconds
    uint32 duration;
}Record;
*/
// recentcalls Applet对象结构体：
typedef struct _CRecentCalls
{
    DECLARE_VTBL(IRecentCalls)
    uint32          m_nRefs; // 对象引用计数器
    IModule        *m_pModule;
    IDisplay       *m_pDisplay;
    IShell         *m_pShell;
    IAnnunciator *m_pIAnn;
#ifdef FEATURE_LED_CONTROL
    IBacklight     *m_pBacklight;
#endif    
    AEERect         m_rc;
    // 当前活动对话框IDialog接口指针
    IDialog        *m_pDialog;
    // 当前活动对话框ID
    uint16          m_pActiveDlgID;
    // 活动对话框关闭时的返回结果
    DLGRetValue     m_eDlgRet;
    // 是否不改写对话框返回结果，一般情况需要改写(FALSE)。
    boolean         m_bNotOverwriteDlgRet;
    // Applet 前一状态
    recentcallsState m_ePreState;
    // Applet 当前状态
    recentcallsState m_eCurState;
    // Applet start type
    AppletStartMethod   m_eStartMethod;
    // Applet是否处于挂起状态
    boolean         m_bSuspending;
    // Applet是否准备好可以处理按键和命令事件。改变量主要用于快速按键引起的问题
    boolean         m_bAppIsReady;
    
    IMenuCtl       *pMenu;  //菜单机构指针
    IImage         *Image;  //图片指针
    IConfig        *m_pConfig;//nv配置指针
    Record          list_record[AEECALLHISTORY_MAX_CALLS];
    uint16           m_callsCategory;
    uint16          m_selectedItem[2]; //菜单分级记录当前选择的项    
    uint16          record_selected; //记录当前选中了哪个record记录
    uint16          selectState;
    uint16          record_count; //通话记录的个数
     // message resource ID
    uint16              m_wMsgResID;
    boolean         key_enable; //是否禁止按键事件
    //ICallList         *m_pCallList;
    ICallHistory    *m_pCallHistory;
    PromptMsg_Param_type m_PromptMsg;
    
    ITextCtl*       m_pPhoneLockPasswordTextCtl;
    char            m_charAppStartArgs;
    IStatic*        m_pStatic;
    char*         m_pPhoneLockPassword;
    PFNSELECTEDRECENTCALLSCB m_pSelFldCB;
    uint16           m_nRemainWMSNum;
	uint16           m_currDlgId;
} CRecentCalls;

/*==============================================================================
                                 宏定义和常数
==============================================================================*/
// 根据 BREW 3 的需要，重定义资源文件宏
#define  AEE_RECENTCALLSRES_LANGFILE (AEE_RES_LANGDIR RECENTCALLS_RES_FILE)
#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
#define RECENTCALLS_PT_IN_RECT(a,b,rct)      (boolean)( ((a) >= (rct).x && (a) <= ((rct).x + (rct).dx)) && ((b) >= (rct).y && (b) <= ((rct).y + (rct).dy)) )
#endif

// 为防止用户快速按键而添加事件EVT_APPISREADY，事件由定时器
// recentcalls_APPIsReadyTimer发出
#if defined(FEATURE_VERSION_C306)||defined(FEATURE_VERSION_W0216A)
#define AVK_SEND_TWO     2
#elif defined(FEAUTRE_VERSION_N450)|| defined(FEATURE_VERSION_N021)|| defined(FEATURE_VERSION_C01)|| defined(FEATURE_VERSION_W516)|| defined(FEATURE_VERSION_W027)
#define AVK_SEND_TWO     2
#else
#define AVK_SEND_TWO     1
#endif

#define APPISREADY_TIMER       200
#define EVT_APPISREADY   (EVT_USER+1)         

#define EVT_USER_CLOSEAPP (EVT_USER+13)
#define xOffset                 (5)

#define RECENTCALLS_DRAW_BOTTOMBAR(x)                           \
{                                                   \
    BottomBar_Param_type BarParam;                  \
    MEMSET(&BarParam, 0, sizeof(BarParam));         \
    BarParam.eBBarType = x;                         \
    DrawBottomBar(pMe->m_pDisplay, &BarParam);      \
}

// 状态移动宏：先将前一状态用当前状态更新，再将当前状态设为nextState
#define MOVE_TO_STATE(nextState) {            \
            recentcallsState tpState;            \
            tpState = nextState;          \
            pMe->m_ePreState = pMe->m_eCurState; \
            pMe->m_eCurState = tpState;        \
        }

#define STARTARG_OUTGCALL    ((char) 'A')  //按SEND键进入呼出列表时候的标志字符
#define STARTARG_MISSEDCALL  ((char) 'B')  //错过来电进入错过列表时候的标志字符
#define STARTARG_ALLCALL     ((char) 'C')  //所有的通话记录，发短信的时候可能要选择所有的号码
#define STARTARG_ONECALL     ((char) 'D') //获取一个通话记录
#define MAX_STRING_LEN       (38)
#define BETWEEN_LINE_PIXELS (3)
#define PIXELS_TO_EDGE          (6)

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
       recentcalls_ShowMsgBox
说明:
       函数由状态处理函数用来创建相应对话框资源ID的对话框。

参数:
       pMe [In]: 指向recentcalls Applet对象结构的指针,该结构包含小程序的特定信息.
       wTextResId [in]：对话框资源ID，函数根据该ID创建对话框。

返回值:
       无

备注:
       

==============================================================================*/

void recentcalls_ShowMsgBox(CRecentCalls *pMe, uint16 wTextResId);
/*==============================================================================
函数:
       recentcalls_ShowDialog
说明:
       函数由状态处理函数用来创建相应对话框资源ID的对话框。

参数:
       pMe [In]: 指向recentcalls Applet对象结构的指针,该结构包含小程序的特定信息.
       dlgResId [in]：对话框资源ID，函数根据该ID创建对话框。

返回值:
       无

备注:
       

==============================================================================*/
void recentcalls_ShowDialog(CRecentCalls *pMe,uint16  dlgResId);

/*==============================================================================
函数:
       recentcalls_RouteDialogEvent
说明:
       函数将BREW事件路由给当前活动对话框的事件处理函数。

参数:
       pMe [In]: 指向recentcalls Applet对象结构的指针,该结构包含小程序的特定信息.
       eCode [in]：事件代码。
       wParam [in]：与事件eCode关联的数据。
       dwParam [in]：与事件eCode关联的数据。

返回值:
       TRUE：传入事件被处理。
       FALSE：传入事件没被处理。

备注:

==============================================================================*/
boolean recentcalls_RouteDialogEvent(CRecentCalls *pMe,
    AEEEvent    eCode,
    uint16  wParam,
    uint32 dwParam
);

/*----------------------状态机相关函数声明---------------------*/
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
NextFSMAction recentcalls_ProcessState(CRecentCalls *pMe);

void RecentCalls_FreeSelectRecordListNode(CRecentCalls *pMe);

// 获取短信发送列表剩余号码条数
uint16 RecentCalls_GetRemainWMSNum(CRecentCalls *pMe);

#endif
//RECENTCALLPRIV_H
#ifdef FEATURE_THAILAND  
int Recntcall_is_view_state(int bb) ;
#endif


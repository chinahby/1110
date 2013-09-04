#ifndef SECURITYMENUPRIV_H
#define SECURITYMENUPRIV_H

/*==============================================================================
// 文件：
//        SecurityMenu_priv.h
//        版权所有(c) 2009 Anylook Tech. CO.,LTD.
//
// 文件说明：
//
// 作者：Gemsea
// 创建日期：2007-11-01
// 当前版本：Draft
==============================================================================*/

/*==============================================================================
修改历史记录：
时       间      修 改 人    问题单号  修改内容、位置及原因
-----------      ----------  --------  -----------------------------------------------
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
    // Use the .bid file if compiling as dynamic applet
    #include "SecurityMenu.bid"
#else
    #include "OEMClassIds.h"  // Applet Ids
    #ifndef AEECLSID_APP_SECURITYMENU
        #error AEECLSID_APP_SECURITYMENU must be defined
    #endif
#endif

#include "SecurityMenu.brh"
#include "AEEShell.h"
#include "AEEModTable.h"
#include "AEEStdLib.h"
#include "AEEModGen.h"
#include "AEEAppGen.h"
#include "OEMClassIDs.h"
//#include "OEMMacros.h"
#include "SecurityMenu.h"


#include "AEEText.h"
//#include "AEEPhone.h"
#include "AppComFunc.h"
#include "Appscommon.h"
#ifdef FEATURE_LCD_TOUCH_ENABLE
#include "appscommonimages.brh"
#endif
//#include "OEMRUIM.h"
//#include "OEMAppFuncs.h"
#include "AEEConfig.h"
//#include "OEMTSGBridge.h"
//#include "uiutils.h"
#include "OEMNV.h"
#ifdef FEATURE_NV_RUIM
#ifndef WIN32
#include "nvruimi.h"
#endif//WIN32
#endif /*FEATURE_NV_RUIM*/
#ifndef WIN32
#include "nv.h"
#endif//WIN32
//#include "OEMAnnun.h"         // for ANNUN_ALERT
#include "AEEAnnunciator.h"
#include "OEMCFGI.h"
#include "AEERUIM.h"
#include "AEE_OEM.h"
//#include "AEECallList.h"
#include "AEECallHistory.h"


#include "OEMDisp.h"       // TBD - hack until available somewhere else
#include "OEMSound.h"      // TBD - hack until available somewhere else
#include "AEE_OEM.h"
#include "ContApp.h"
#include "AEESoundPlayer.h"
#include "AEEMedia.h"  // for voice recording
#include "AEEBacklight.h"
#include "OEMFS.h"

#ifdef FEATURE_LED
#include "led.h"
#endif//FEATURE_LED
//#include "OEMFS.h"
#ifndef WIN32
#include "db.h"
#endif
#ifdef FEATURE_UTK2
#include "UTKUtils.h"
#endif
#ifdef FEATURE_OEMOMH 
#include "AEERUIM.h"
#endif

#include "AEECallHistoryExt.h"

#include "appscommonimages.brh"




#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
#define SECURITYMENU_PT_IN_RECT(a,b,rct)      (boolean)( ((a) >= (rct).x && (a) <= ((rct).x + (rct).dx)) && ((b) >= (rct).y && (b) <= ((rct).y + (rct).dy)) )
#define DWPARMA_OTHER 10000
#endif//FEATURE_LCD_TOUCH_ENABLE
/*==============================================================================
                                 类型定义
==============================================================================*/
#define  AEE_APPSSECURITYMENU_RES_FILE ( "fs:/mod/securitymenu/" AEE_RES_LANGDIR SECURITYMENU_RES_FILE)
#define SEC_ERR(format, code1, code2, code3) //ERR(format, code1, code2, code3)

// 画界面底部提示条宏定义
#define SEC_MENU_DRAW_BOTTOMBAR(x)                           \
{                                                   \
    BottomBar_Param_type BarParam;                  \
    MEMSET(&BarParam, 0, sizeof(BarParam));         \
    BarParam.eBBarType = x;                         \
    DrawBottomBar(pMe->m_pDisplay, &BarParam);      \
}
/*----------------------对话框相关数据类型声明---------------------*/
// 对话框关闭时返回值列表
typedef enum DLGRetValue
{
   // 初始值，表明需要创建对话框
   DLGRET_CREATE,

   DLGRET_MSGBOX_OK,
   DLGRET_MSGBOX_CANCEL,
   DLGRET_MSGBOX_CONFIRM,

   // 对话框关闭时可返回的通用值
   DLGRET_OK,
   DLGRET_CANCELED,
   //main menu
   DLG_ASKPIN,
   DLGRET_TOSHOWMSG,

   DLG_APPLICATIONLOCK,
   DLG_PHONEPASSWORD,
   DLG_PHONEPASSWORDINPUT,

   DLG_PINCHECK,
   DLG_ASKCALLPASSWORD,
   DLG_PINCHANGE,
   DLGRET_VALIDPINPASS,
   DLGRET_SETPINPASS,
   DLGRET_VALIDPINFAILED,
   DLGRET_SETPINFAILED,
   DLGRET_AFFIRMPASSWORD,
   DLGRET_ISCOMFIRMPASSWORD,
   DLGRET_EMGCALL,
   DLGRET_RESTRICT,
   DLGRET_RESTRICTOUTGOING,
   DLGRET_RESTRICTINCOMING,
   DLGRET_RESTORE,
   DLGRET_RESTORE_MESSAGE,
   DLGRET_CHANGECODE,
   DLGRET_DELETE,
#ifdef FEATURE_LCD_TOUCH_ENABLE
   DLGRET_TSIMPASSWORDINPUT,
#endif
   DLGRET_ARKPASSWORD
   #if defined(FEATURE_VERSION_HITZ181)||defined(FEATURE_VERSION_MTM)||defined(FEATURE_VERSION_W317A)
   ,DLGRET_KEYLOCK
   #endif
   #if defined(FEATURE_VERSION_W317A)||defined(FEATURE_VERSION_C337)||defined(FEATURE_VERSION_C316)
   ,DLGRET_MOBILE_TRACKER
   #endif
#ifdef FEATURE_VERSION_C316	
    ,DLGRET_ONEKEY_LOCK_KEYPAD
#endif
} DLGRet_Value_e_Type;

/*----------------------状态机相关数据类型声明---------------------*/
// SecurityMenu Applet 状态机状态：
typedef enum _SecurityMenuState
{
   SECURITYMENU_NONE,
   SECURITYMENU_INIT,
   SECURITYMENU_MAIN,
   SECURITYMENU_APPLICATIONLOCK,
   SECURITYMENU_PHONEPASSWORD,
   SECURITYMENU_CHANGECODE,
   SECURITYMENU_PHONEPASSWORDINPUT,
   SECURITYMENU_PINCHECK,
   SECURITYMENU_PINCHANGE,
   SECURITYMENU_ASKPIN,
#ifdef FEATURE_LCD_TOUCH_ENABLE
   SECURITYMENU_TSIMPASSWORDINPUT,
#endif
   SECURITYMENU_ASKPASSWORD,
   SECURITYMENU_ASKCALLPASSWORD,
   SECURITYMENU_AFFIRMPASSWORD,
   SECURITYMENU_ASKPUKPASSWORD,
   SECURITYMENU_UIMERR,
   SECURITYMENU_EMERGENCYCALL,
   SECURITYMENU_RESTRICT,
   SECURITYMENU_RESTRICTOUTGOING,
   SECURITYMENU_RESTRICTINCOMING,
   SECURITYMENU_RESTORE,
   #if defined(FEATURE_VERSION_HITZ181)||defined(FEATURE_VERSION_MTM)||defined(FEATURE_VERSION_W317A)
   SECURITYMENU_KEYLOCK,
   #endif
   #if defined(FEATURE_VERSION_W317A)||defined(FEATURE_VERSION_C337)||defined(FEATURE_VERSION_C316)
   MOBILE_TRACKER,
   #endif

#ifdef FEATURE_VERSION_C316	
    SECURITYMENU_ONEKEY_LOCK_KEYPAD,
#endif
   SECURITYMENU_EXIT

} SecurityMenuState;
typedef enum
{
    ENTERPUK_STEP0,     // 要求输入 PUK 码第一步，输入 PUK 码
    ENTERPUK_STEP1,     // 要求输入 PUK 码第二步，输入新 PIN 码
    ENTERPUK_STEP2      // 要求输入 PUK 码第三步，确认新 PIN 码
} Enter_UIMSecurityCode_e_Type;

// 状态处理函数返回给状态处理主函数的值类型
typedef enum NextFSMAction
{
   NFSMACTION_WAIT,
   NFSMACTION_CONTINUE
} NextFSMAction;

// 保存模块信息和模块引用计数的结构
typedef struct SecurityMenuMod
{
    DECLARE_VTBL(IModule)
    uint32   m_nRefs;
} SecurityMenuMod;
//#ifndef PINCODE_LENGTH
//#define PINCODE_LENGTH UIM_MAX_CHV_DIGITS
//#endif
typedef enum
{
    SEC_SEL_NONE,
    SEC_SEL_PHONE_LOCK,
    SEC_SEL_PHONEBOOK_LOCK,
    SEC_SEL_RECENTCALL_LOCK,
    SEC_SEL_SMS_LOCK,
    SEC_SEL_KEY_LOCK
#ifdef FEATURE_VERSION_C316
    ,SEC_SEL_ONE_KEY_LOCK_KEYPAD
#endif
}SEC_LOCK_SEL;
// SecurityMenu Applet对象结构体：
typedef struct _CSecurityMenu
{
    DECLARE_VTBL(ISecurityMenu)
    uint32              m_nRefs;          // 对象引用计数器
    IModule             *m_pModule;
    IDisplay            *m_pDisplay;
    IShell              *m_pShell;
    IImage              *m_PromtImage;

    AEERect             m_rc;
     // message resource ID
    // 当前活动对话框IDialog接口指针
    IDialog             *m_pActiveDlg;
    // 当前活动对话框ID
    uint16              m_pActiveDlgID;
    // 活动对话框关闭时的返回结果
    DLGRet_Value_e_Type m_eDlgRet;
    // 是否不改写对话框返回结果，一般情况需要改写(FALSE)。
    boolean             m_bNotOverwriteDlgRet;
    // Applet 前一状态
    SecurityMenuState   m_ePreState;
    // Applet 当前状态
    SecurityMenuState   m_eCurState;
    // Applet是否处于挂起状态
    boolean             m_bSuspending;
    // Applet是否准备好可以处理按键和命令事件。改变量主要用于快速按键引起的问题
    boolean             m_bAppIsReady;
//密码无效标志
    //当前所选菜单项ID
    uint16              m_currDlgId;
    uint16              m_nSubDlgId; //次级菜单位置
    IStatic             *m_pIStatic;
    IConfig             *m_pConfig;  // IConfig interface
#ifdef FEATURE_LCD_TOUCH_ENABLE
    boolean             m_bup;
    int16               m_i;
    uint16              m_pActiveTSIMInputID;
#endif

    uint32              ui_esn;
    uint16              m_wMsgID;
    int                 m_nCanAttemptTimes;
    uint16              m_PassWord;
    boolean             m_IsPin;
    char                m_Pin[UIM_MAX_CHV_DIGITS+1];
    //IPhone            *m_phone; 
    SEC_LOCK_SEL        m_lock_sel;
    IRUIM               *m_pIRUIM;
    IAnnunciator         *m_pIAnn;
    
    int                     nOldPSWLength;
    int                     nNewPSWLength;
    boolean              m_bIsConfirmPassword;
    Enter_UIMSecurityCode_e_Type  m_eRUIMSCode;
    char                  m_strPUK[UIM_MAX_CHV_DIGITS + 1];
    char                  *m_strPhonePWD;
    char                  m_strPIN[UIM_MAX_CHV_DIGITS + 1]; 
	char                  *m_strPhoneNUM;
} CSecurityMenu;


/*==============================================================================
                                 宏定义和常数
==============================================================================*/
// 为防止用户快速按键而添加事件EVT_APPISREADY，事件由定时器
// SecurityMenu_APPIsReadyTimer发出
#define APPISREADY_TIMER       100             
#define EVT_APPISREADY    (EVT_USER + 1)       

// 用户重新更新界面事件
//#define EVT_USER_REDRAW   (EVT_USER + 2)

//用于定时器回调
//#define EVT_DIALOGTIMEOUT (EVT_USER + 3)

// 状态移动宏：先将前一状态用当前状态更新，再将当前状态设为nextState
#define MOVE_TO_STATE(nextState) {            \
            SecurityMenuState tpState;            \
            tpState = nextState;          \
            pMe->m_ePreState = pMe->m_eCurState; \
            pMe->m_eCurState = tpState;        \
        }
#define SECRUITY_MENU_SOFK_HIGHT 0
#define FIRSR_MENU_LINE 33
#define LAST_MENU_GAP   27
#if defined(FEATURE_VERSION_C306)||defined(FEATURE_VERSION_W0216A)
#define AVK_SEND_TWO     2
#elif defined(FEAUTRE_VERSION_N450)|| defined(FEATURE_VERSION_N021)|| defined(FEATURE_VERSION_C01)|| defined(FEATURE_VERSION_W516) || defined(FEATURE_VERSION_W027)
#define AVK_SEND_TWO     2
#else
#define AVK_SEND_TWO     1
#endif

#ifdef FEATURE_LCD_TOUCH_ENABLE
#define CALC_ITEM                    15
#define STARTY                       50
#define STARX                        0
#define SPACEX                       0
#define NUMWINDTH                    80
#define NUMHEIGHT                    44

#define PWTEXT_MINX                  0
#define PWTEXT_MINY                  25
#define PWTEXT_MAXX                  240
#define PWTEXT_MAXY                  150
#endif

// Max number of attempts to enter the security code.  This
// value is only used in the non R-UIM build or when CHV1
// is enabled.
//#define MAX_PIN_ATTEMPTS        3
//#define MAX_PASSWORD_NUMBER     4

////
// Number of milliseconds to display a message before automatically closing it
#define MSG_TIMEOUT_MS   (2 * 1000L)

////
// This command event (EVT_COMMAND) is sent to the applet by itself
// when the message timeout occurs (see CSecurtyMenu_MessageTimeoutCB)
#define ID_MSG_EVENT            (EVT_USER)

#define xOffset                 (5)
#ifdef FEATURE_VERSION_K212
#define SECURITY_PROMPTMSG_TIMER (500)
#else
#define SECURITY_PROMPTMSG_TIMER (1500)
#endif
/*==============================================================================
                                 全局数据
==============================================================================*/

/*==============================================================================
                                 本地（静态）数据
==============================================================================*/

/*==============================================================================
                                 函数声明
==============================================================================*/
#if 0
void PhoneLock_ShowMessageBoxThatPasswordIsInvalid(CSecurityMenu *pMe, IDisplay* pDisplay, IStatic* pStatic, AECHAR* pPrompt);
#endif

/*==============================================================================
函数:
   Security_ShowMsgBox

说明:
    函数由状态处理函数用来唤起安全窗口对话框，

参数:
    pMe [in]: 指向 Security Applet 对象结构的指针。该结构包含小程序的特定信息。
    wTextResId [in]: 消息文本资源 ID。

返回值:
    none

备注:

==============================================================================*/
void Security_ShowMsgBox(CSecurityMenu *pMe, uint16 wTextResId);

/*----------------------对话框相关函数声明---------------------*/
/*==============================================================================
函数:
       SecurityMenu_ShowDialog
说明:
       函数由状态处理函数用来创建相应对话框资源ID的对话框。

参数:
       pMe [In]: 指向SecurityMenu Applet对象结构的指针,该结构包含小程序的特定信息.
       dlgResId [in]：对话框资源ID，函数根据该ID创建对话框。

返回值:
       无

备注:


==============================================================================*/
void SecurityMenu_ShowDialog(CSecurityMenu *pMe,uint16  dlgResId);

/*==============================================================================
函数:
       SecurityMenu_RouteDialogEvent
说明:
       函数将BREW事件路由给当前活动对话框的事件处理函数。

参数:
       pMe [In]: 指向SecurityMenu Applet对象结构的指针,该结构包含小程序的特定信息.
       eCode [in]：事件代码。
       wParam [in]：与事件eCode关联的数据。
       dwParam [in]：与事件eCode关联的数据。

返回值:
       TRUE：传入事件被处理。
       FALSE：传入事件没被处理。

备注:

==============================================================================*/
boolean SecurityMenu_RouteDialogEvent(CSecurityMenu *pMe,
                                      AEEEvent      eCode,
                                      uint16        wParam,
                                      uint32        dwParam);

/*----------------------状态机相关函数声明---------------------*/
/*==============================================================================
函数:
       SecurityMenu_ProcessState
说明:
       SecurityMenu Applet状态处理主函数。函数根据pMe->m_currState将调用路由至
       相应的处理函数。

参数:
       pMe [In]: 指向SecurityMenu Applet对象结构的指针,该结构包含小程序的特定信息.

返回值:
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注:

==============================================================================*/
NextFSMAction SecurityMenu_ProcessState(CSecurityMenu *pMe);

#endif
//SECURITYMENUPRIV_H


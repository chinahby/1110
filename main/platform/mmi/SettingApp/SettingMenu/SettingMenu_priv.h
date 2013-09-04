#ifndef SETTINGMENUPRIV_H
#define SETTINGMENUPRIV_H

/*==============================================================================
// 文件：
//        SettingMenu_priv.h
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
-----------      ----------  --------  -----------------------------------------
2007-11-01                  初始版本
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
   #include "SettingMenu.bid"
#else
   #include "OEMClassIds.h"  // Applet Ids 
   #ifndef AEECLSID_APP_SETTINGMENU
      #error AEECLSID_APP_SETTINGMENU must be defined
   #endif
#endif

#include "SettingMenu.brh"
#include "AEEShell.h"
#include "AEEModTable.h"
#include "AEEStdLib.h"
#include "AEEModGen.h"
#include "AEEAppGen.h"
#include "OEMClassIDs.h"
//#include "OEMMacros.h"
#include "SettingMenu.h"

#include "AEEMenu.h"
#include "AEEConfig.h"
#include "AppComFunc.h"
#include "AEEText.h"
#include "AEEAnnunciator.h"
//#include "AEEPhone.h"//add by lzhl20040823
#include "SoundMenu.h"
#ifdef FEATURE_USES_ZI
#include "OEMZIText.h" 
#else
#include "OEMText.h" 
#endif
#include "OEMFeatures.h"
#include "OEMCFGI.h"
#include "AppComFunc.h"
#include "Appscommon.h"
#include "AEERUIM.h"
#include "OEMNV.h"


/*==============================================================================
                                 类型定义
==============================================================================*/
#define  AEE_APPSSETTINGMENU_RES_FILE AEE_RES_LANGDIR SETTINGMENU_RES_FILE
#define SETTING_ERR(format, code1, code2, code3) MSG_FATAL(format, code1, code2, code3)

// 画界面底部提示条宏定义
#define SETTING_MENU_DRAW_BOTTOMBAR(x)                           \
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
    DLGRET_CREATE,
    DLGRET_OK,  
    DLGRET_CALLSETTING,		   //IDD_CALLSETTING_MENU    
    DLGRET_PHONESETTING,	      //IDD_PHONESETTING_MENU
    DLGRET_CALLSETTINGSEL,     //IDD_CALLSETTINGSEL    
    DLGRET_NETWORKSETTING,  	//IDD_NETWORKSETTING_MENU   
    DLGRET_DIVERT,	            //IDD_DIVERT_MENU   
    DLGRET_CALLFORWARDSEL,     //IDD_CALLFORWARD_SEL_MENU
    DLGRET_CALLFORWARDINPUT,   //IDD_CALLFORWARD_INPUT_MENU      
    DLGRET_AKG,	               //IDD_AKG_DIALOG
    DLGRET_BANNER,             //IDD_BANNER_DIALOG
#ifdef FEATURE_TIME_DATA_SETTING
    DLGRET_TIMESETTING,
    DLGRET_DATESETTING,
#endif
#if defined(FEATURE_VERSION_C337)||defined(FEATURE_VERSION_W317A)
	DLGRET_TRACKERSMSSETTING,
#endif

#ifdef FEATURE_LANGUAGE
    DLGRET_LANGUAGE,	         //IDD_LANGUAGE_MENU   
#endif
    DLGRET_WARNING,            //IDD_WARNING_MESSEGE
#ifdef FEATURE_PLANEMODE
    DLGRET_PLANEMODE,            //IDD_PLANEMODE
#endif
#ifdef FEATURE_PERU_VERSION
    DLGRET_PHONE_NUMBER,
#endif
#ifdef FEATRUE_AUTO_POWER
    DLGRET_AUTO_POWER,
#endif
    DLGRET_AUTO_ANSWER_MODE,
#ifdef FEATRUE_SET_IP_NUMBER
    DLGRET_IP_NUMBER_SET,
#endif
    DLGRET_KEYLENGTH,
    DLGRET_AUTO_ANSWER_MODE_SUB,
    DLGRET_CANCELED,
    DLGRET_FMMODE,
#ifdef FEATURE_SUPPORT_G_SENSOR
    DLGRET_SHAKE,
    DLGRET_SHAKESUB,
#endif
    DLGRET_CALLRESTRICT,
    DLGRET_PASSWORD,
    DLGRET_INVALID,
    DLGRET_RESTRICTOUTGOING,
    DLGRET_RESTRICTINCOMING,
    DLGRET_MSGBOX_OK,
    DLGRET_MSGBOX_CANCEL
    #ifdef  FEATURE_DOUBLE_SIM_CARD
    ,DLGRET_SIMSETTING,
    DLGRET_SIMSETTING_CHOICE
    #endif
    #ifdef FEATURE_VERSION_MYANMAR
    ,DLGRET_SEARCHMODE
    ,DLGRET_TIMEFONTMODE
    #endif
#ifdef FEATURE_VERSION_W317A 
    ,DLGRET_AUTOCALLRECORD     //Add by pyuangui 20130104
#endif
#ifdef FEATURE_VERSION_W208S
    ,DLGRET_SMSRESTRICT
//    ,DLGRET_SMSRESTRICT_SEND
    ,DLGRET_SMSRESTRICT_RECEIVE
    ,DLGRET_SMSRESTRICT_RECEIVE_ADD
#endif
#ifdef FEATURE_SHOW_PHONE_INFO
    ,DLGRET_PHONE_INFO
    ,DLGRET_PHONE_INFO_SW    
    ,DLGRET_PHONE_INFO_HW
    ,DLGRET_PHONE_INFO_PRL
#endif	
#ifdef FEATURE_SOUND_BO
    ,DLGRET_SPEECH_SETTINGS   //IDD_SPEECH_MENU
#endif 

} DLGRet_Value_e_Type;

/*----------------------状态机相关数据类型声明---------------------*/
// SettingMenu Applet 状态机状态：
typedef enum _SettingMenuState
{
   SETTINGMENUST_NONE,
   SETTINGMENUST_INIT,
   SETTINGMENUST_MAIN,
   SETTINGMENUST_CALLSETTING,
   SETTINGMENUST_PHONESETTING,
   SETTINGMENUST_CALLSETTINGSEL,
   SETTINGMENUST_NETWORKSETTING,
   SETTINGMENUST_DIVERT,
   SETTINGMENUST_CALLFORWARDSEL,
   SETTINGMENUST_CALLFORWARDINPUT,
   SETTINGMENUST_AKG,
   SETTINGMENUST_BANNER,
#ifdef FEATURE_TIME_DATA_SETTING
   SETTINGMENUST_TIMESETTING,
   SETTINGMENUST_DATESETTING,
#endif
#if defined(FEATURE_VERSION_C337)||defined(FEATURE_VERSION_W317A)
   SETTINGMENUST_TRACKERSMSSETTING, 	//TRRACK SMS设置
#endif

#ifdef FEATURE_LANGUAGE
   SETTINGMENUST_LANGUAGE,
#endif
#ifdef FEATURE_PERU_VERSION
   SETTINGMENUST_PHONENUMBER,
#endif
#ifdef FEATRUE_AUTO_POWER
   SETTINGMENUST_AUTO_POWER,
#endif
#ifdef FEATURE_SUPPORT_G_SENSOR
   SETTINGMENUST_SHAKE,
   SETTINGMENUST_SHAKESUB,
#endif
   SETTINGMENUST_AUTO_ANSWER,
   SETTINGMENUST_FMMODE,
#ifdef FEATRUE_SET_IP_NUMBER
   SETTINGMENUST_IP_NUMBER_SET,
#endif
   SETTINGMENUST_KEYLENGTH,
   SETTINGMENUST_AUTOANSWERSUB,
   SETTINGMENUST_EXIT,
   SETTINGMENUST_PASSWORD,
   SETTINGMENUST_CALLRESTRICT,
   SETTINGMENUST_RESTRICTOUTGOING,
   SETTINGMENUST_RESTRICTINCOMING
#ifdef FEATURE_PLANEMODE
   ,SETTINGMENUST_PLANEMODE
#endif
#ifdef  FEATURE_DOUBLE_SIM_CARD
   ,SETTINGMENUST_SIMSETTING,
   SETTINGMENUST_SIMSETTING_CHOICE
#endif
#ifdef FEATURE_VERSION_MYANMAR
   ,SEARCHMENUST_MODE
   ,TIMEFONTMENUST_MODE
#endif
#ifdef FEATURE_VERSION_W317A 
   ,SETTING_AUTOCALLRECORD     //Add by pyuangui 20130104
#endif
#ifdef FEATURE_VERSION_W208S
   ,SETTINGMENUST_SMSRESTRICT
//   ,SETTINGMENUST_SMSRESTRICT_SEND
   ,SETTINGMENUST_SMSRESTRICT_RECEIVE
   ,SETTINGMENUST_SMSRESTRICT_RECEIVE_ADD
#endif
#ifdef FEATURE_SHOW_PHONE_INFO
   ,SETTINGMENUST_PHONE_INFO
   ,SETTINGMENUST_PHONE_INFO_SW
   ,SETTINGMENUST_PHONE_INFO_HW
   ,SETTINGMENUST_PHONE_INFO_PRL
#endif   
#ifdef FEATURE_SOUND_BO
   ,SETTINGMENUST_SPEECH
#endif
} SettingMenuState;

// 状态处理函数返回给状态处理主函数的值类型
typedef enum NextFSMAction
{
   NFSMACTION_WAIT,
   NFSMACTION_CONTINUE
} NextFSMAction;

// 保存模块信息和模块引用计数的结构
typedef struct SettingMenuMod
{
    DECLARE_VTBL(IModule)
    uint32   m_nRefs;
} SettingMenuMod;

// 补充业务种类
typedef enum {
   CALLFORWARD_NONE,
   CALLFORWARD_BUSY,        //遇忙转移
   CALLFORWARD_NOANSWER,    //无应答转移
   CALLFORWARD_NOCONNECT,   //未接通转移
   CALLFORWARD_ANYWAY,      //无条件转移
#ifdef FEATURE_CARRIER_SUDAN_SUDATEL
   CALLFORWARD_CNIR,
   CALLFORWARD_DND,
   CALLFORWARD_SPINA,
   CALLFORWARD_SPINI,
#endif
   CALLFORWARD_WAIT         //呼叫等待
} CALLFORWARD_TYPE;

// SHAKE操作类型
typedef enum {
   SET_SHAKE_NONE,
   SET_MUSIC_CHECK,     //音乐
   SET_FM_RADIO_CHECK,     //收音机
   SET_WALLPAPER_CHECK,      //图片
   SET_SNOOZE_ALARM_CHECK,   //闹钟
   SET_VIDEO_CHECK           //视频
} SET_SHAKE_TYPE;
#ifdef FEATRUE_SET_IP_NUMBER
typedef struct _number_table
{    
    AECHAR   number[OEM_IP_NUMBER_MAX+1];
    struct _number_table *next;
}number_table;

typedef struct _Ip_number_table
{
    number_table * head;
    number_table * tial;
    AECHAR  default_ip[OEM_IP_NUMBER_MAX+1];
    uint16            ip_state;
    byte              count;
}Ip_number_table;
#endif //#ifdef FEATRUE_SET_IP_NUMBER
// SettingMenu Applet对象结构体：
typedef struct _CSettingMenu
{
    DECLARE_VTBL(ISettingMenu)
    uint32           m_nRefs;            // 对象引用计数器
    IModule          *m_pModule;
    IDisplay         *m_pDisplay;
    IShell           *m_pShell;
    AEERect          m_rc;               // 当前活动对话框IDialog接口指针
    IDialog          *m_pActiveDlg;      // 当前活动对话框ID
    uint16           m_pActiveDlgID;     // 活动对话框关闭时的返回结果
    DLGRet_Value_e_Type   m_eDlgRet;     // 是否不改写对话框返回结果，一般情况需要改写(FALSE)。
    boolean          m_bNotOverwriteDlgRet;// Applet 前一状态
    SettingMenuState m_ePreState;        // Applet 当前状态
    SettingMenuState m_eCurState;        // Applet是否处于挂起状态
    boolean          m_bSuspending;      // Applet是否准备好可以处理按键和命令事件。改变量主要用于快速按键引起的问题
    boolean          m_bAppIsReady;
    IAnnunciator    *m_pAnn;
    //IOEM_TSGBridge  *m_pOEM_TSGBridge;
    IConfig         *m_pConfig;
    //IPhone          *m_phone;           
    CALLFORWARD_TYPE m_CFType;          // 呼叫转移/等待的类型

    uint16           m_nResCodeID;      //呼叫号码ID记录
    uint16           m_currDlgId;       //保存当前所选菜单位置
    uint16           m_nResID;          //补充业务标题ID记录
    uint16           m_nSubDlgId;		    //次级菜单位置
    byte             m_RingVol;         //铃声音量
    uint16           m_sSubDlgId;       //三级菜单位置
    uint16           m_CallSettingSel;
    SET_SHAKE_TYPE            m_shaketype_sel;          // SHAKE类型
    char             m_callnumber[25];
#ifdef FEATRUE_AUTO_POWER
    uint16           m_auto_power;
    IMenuCtl*        m_pState;
    IMenuCtl*     m_pStatus;
    ITimeCtl         *m_pTime;
    Auto_Power_Cfg   m_ClockCfg;
    int32            m_dwDispTime;       //屏幕上显示的时间
    int32            m_dwHour;           //记录用数字键设置的小时
    int32            m_dwMin;            //记录用数字键设置的分钟
    uint16           m_nCtlID;           //控件ID
    uint16           m_nTimeCtlCount;    //0代表焦点在小时上,1代表在分钟上
    uint16           m_nNumKeyCount;     /*记录在设置时间时的按键次数.0:小时十位
                                        1:小时个位;   2:分钟十位  3:分钟个位*/
#ifdef SET_REP_MOD_LIST
    IMenuCtl         *m_pRepMode;         //IMenuCtl指针,"重复模式"
#endif
    uint16           m_b_ampm_mode;
    boolean          m_b_selete_Pm;
#endif
    uint16           m_input_mode;
    IRUIM            *m_pIRUIM;
#ifdef FEATRUE_SET_IP_NUMBER
    Ip_number_table  ip_number_tab;
    boolean          need_save_default;
#endif //#ifdef FEATRUE_SET_IP_NUMBER
    uint16           m_ip_sel_num;
    uint16           m_msg_id ;
    char             *m_strPhonePWD; 
    nv_item_type    nviNewSimChoice;
    nv_item_type    nviOldSimChoice;
    nv_item_type    nvsearchmode;
	#ifdef FEATURE_LCD_TOUCH_ENABLE
	AEERect pL_Rect1;
	AEERect pL_Rect2;
	AEERect pR_Rect1;
	AEERect pR_Rect2;
	#endif
#ifdef FEATURE_VERSION_W208S    
    sms_restrict_recive_info		sms_restrict_reciveList[MAX_SMS_RESTRICT];
    IMenuCtl                        *m_pMenu;
#endif
} CSettingMenu;

/*==============================================================================
                                 宏定义和常数
==============================================================================*/
// 为防止用户快速按键而添加事件EVT_APPISREADY，事件由定时器
// SettingMenu_APPIsReadyTimer发出
#define APPISREADY_TIMER       100             
#define EVT_APPISREADY   (EVT_USER+1)  
#define SEARCHMODE_AOTE     0
#define SEARCHMODE_FIVE     5*60
#define SEARCHMODE_TEN      10*60
#define SEARCHMODE_TUENTY   15*60
#define SEARCHMODE_THIRTY   30*60
#define TIMEFONTMODE_BIG    1
#define TIMEFONTMODE_MEDIUM 2
#define TIMEFONTMODE_SMALL  3

// 用户重新更新界面事件
//#define EVT_USER_REDRAW   (EVT_USER+2)

//用于定时器回调
//#define EVT_DIALOGTIMEOUT (EVT_USER + 3)
#ifdef FEATRUE_AUTO_POWER
//#define EVT_FOCUS_SWITCH        (EVT_USER + 4)  //焦点切换

#define MSSEC_PER_HOUR          3600000//(60*60*1000)  //一小时内的毫秒数
#define MSSEC_PER_MIN           (60*1000)     //一分钟内的毫秒数

#if defined(FEATURE_VERSION_HITZ181)||defined(FEATURE_VERSION_MTM)
#define CONTROL_RECT_START_X    45  //闹钟设置界面的控件开始位置
#elif defined(FEATURE_VERSION_X3)
#define CONTROL_RECT_START_X    55  //闹钟设置界面的控件开始位置
#elif defined(FEATURE_VERSION_K212)
#define CONTROL_RECT_START_X    65  //闹钟设置界面的控件开始位置
#elif defined(FEATURE_VERSION_EC99)
#define CONTROL_RECT_START_X    65  //闹钟设置界面的控件开始位置
#else
#define CONTROL_RECT_START_X    40  //闹钟设置界面的控件开始位置
#endif

#if defined(FEATURE_VERSION_C306)||defined(FEATURE_VERSION_W0216A)
#define CONTROL_RECT_RESET_Y    10  
#else
#define CONTROL_RECT_RESET_Y    0
#endif

#define IDC_AUTO_POWER_STATUS 1 //开机，关机功能选择项
#define IDC_AUTO_POWER_STATE 2
#define IDC_AUTO_POWER_TIME   3
/*ARABIC*/
#define ARABIC_CONTROL_RECT_START_X   2
#define CONTROL_RECT_START_DX        (pMe->m_rc.dx - CONTROL_RECT_START_X - ARABIC_CONTROL_RECT_START_X) //86
#define AUTO_POWER_IMG_DX            8
#define RGB_GRAY    MAKE_RGB(128,128,128)
#endif
// 状态移动宏：先将前一状态用当前状态更新，再将当前状态设为nextState
#define MOVE_TO_STATE(nextState) {            \
            SettingMenuState tpState;            \
            tpState = nextState;          \
            pMe->m_ePreState = pMe->m_eCurState; \
            pMe->m_eCurState = tpState;        \
        }

#define SETTING_MENU_SOFK_HIGHT 0
//待机问候语最大字符数
#define  BANNER_MAXTXT       8
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
       SettingMenu_ShowDialog
说明:
       函数由状态处理函数用来创建相应对话框资源ID的对话框。

参数:
       pMe [In]: 指向SettingMenu Applet对象结构的指针,该结构包含小程序的特定信息.
       dlgResId [in]：对话框资源ID，函数根据该ID创建对话框。

返回值:
       无

备注:


==============================================================================*/
void SettingMenu_ShowDialog(CSettingMenu *pMe,uint16  dlgResId);

/*==============================================================================
函数:
       SettingMenu_RouteDialogEvent
说明:
       函数将BREW事件路由给当前活动对话框的事件处理函数。

参数:
       pMe [In]: 指向SettingMenu Applet对象结构的指针,该结构包含小程序的特定信息.
       eCode [in]：事件代码。
       wParam [in]：与事件eCode关联的数据。
       dwParam [in]：与事件eCode关联的数据。

返回值:
       TRUE：传入事件被处理。
       FALSE：传入事件没被处理。

备注:

==============================================================================*/
boolean SettingMenu_RouteDialogEvent(CSettingMenu *pMe,
    AEEEvent    eCode,
    uint16  wParam,
    uint32 dwParam
);

/*----------------------状态机相关函数声明---------------------*/
/*==============================================================================
函数:
       SettingMenu_ProcessState
说明:
       SettingMenu Applet状态处理主函数。函数根据pMe->m_currState将调用路由至
       相应的处理函数。

参数:
       pMe [In]: 指向SettingMenu Applet对象结构的指针,该结构包含小程序的特定信息.

返回值:
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注:

==============================================================================*/
NextFSMAction SettingMenu_ProcessState(CSettingMenu *pMe);
#ifdef FEATRUE_SET_IP_NUMBER
void SettingMenu_Print_IP_Number(CSettingMenu *pMe);
#endif
#endif
//SETTINGMENUPRIV_H


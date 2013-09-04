#ifndef CLOCKAPPSPRIV_H
#define CLOCKAPPSPRIV_H

/*==============================================================================
// 文件：
//        ClockApps_priv.h
//        2007-11-12 Gemsea草拟版本(Draft Version)
//        版权所有(c) 2007 Anylook
//
// 文件说明：
//
// 作者：2007-11-12
// 创建日期：Gemsea
// 当前版本：Draft
==============================================================================*/

/*==============================================================================
修改历史记录：
时       间      修 改 人     修改内容、位置及原因
-----------      ----------     -----------------------------------------------
2007-11-12            初始版本
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
   #include "ClockApps.bid"
#else
   #include "OEMClassIds.h"  // Applet Ids
   #ifndef AEECLSID_ALARMCLOCK
      #error AEECLSID_ALARMCLOCK must be defined
   #endif
#endif

#include "clockapps.brh"
#include "clockapps_images.brh"
#include "AEEShell.h"
#include "AEEModTable.h"
#include "AEEStdLib.h"
#include "AEEModGen.h"
#include "AEEAppGen.h"
#include "OEMClassIDs.h"
#include "clockapps.h"

#include "AEEMenu.h"
#include "AEETime.h"
#ifdef FEATURE_USES_ZI
#include "OEMZIText.h" 
#else
#include "OEMText.h" 
#endif
#ifdef FEATURE_UIALARM
#include "AEEAlarm.h"
#endif

#include "AEEBacklight.h"
#include "Appscommon.h"
#include "appscommon.brh"
#include "appscommonimages.brh"

#ifdef FEATURE_SUB_LCD
#include "AEEDisp.h"
#endif
#include "AEE_OEM.h"
#include "AEEConfig.h"
#include "OEMCFGI.h"
#include "AEEText.h"

#ifndef WIN32
#if defined( AEE_SIMULATOR)
    #define ERR
#else
    #include "AEEPhone.h"
    #include "AEEAlert.h"
    #include "ERR.h"
    #include "oemfeatures.h"
    #include "oemnvint.h"
    #include "OEMRTC.h"
#endif
#else
	#define ERR
#include "oemhelperfunctype.h"
#include "oemfeatures.h"
#endif//WIN32

#include "AEEAnnunciator.h"
#if defined( FEATURE_POWERDOWN_ALARM)
void registerPowerdownAlarmclock( void);
#endif

#define FEATURE_ON_OFF_USE_LOCAL_LANGUAGE
#define FEATURE_ONCE_ALARM
/*==============================================================================
                                 宏定义和常数
==============================================================================*/
#if defined( AEE_SIMULATOR)
    #define  AEE_CLOCKAPPS_RES_FILE "fs:/mod/clockapps/en/clockapps.bar"
#else
    #define  AEE_CLOCKAPPS_RES_FILE (AEE_RES_LANGDIR CLOCKAPPS_RES_FILE)
#endif

// 为防止用户快速按键而添加事件EVT_APPISREADY，事件由定时器
// ClockApps_APPIsReadyTimer发出 
#define APPISREADY_TIMER       100            
#define EVT_APPISREADY   (EVT_USER+1)  
 
#define OEM_IME_DIALOG   (0x7FFF)

// 用户重新更新界面事件
//#define EVT_USER_REDRAW   (EVT_USER+2)

#define IDC_CLOCK_ST_LIST   (1)
#define IDC_CLOCK_TIME      (2)
#define IDC_REP_MOD_LIST    (3)
#define IDC_CLOCK_SNOOZE    (4)
#if defined(FEATURE_DISP_160X128)
#define LEFT_ICON_WHID    8
#define LEFT_ICON_HEIGHT  16
#elif defined(FEATURE_DISP_220X176)
#define LEFT_ICON_WHID    12
#define LEFT_ICON_HEIGHT  22
#else
#define LEFT_ICON_WHID    8
#define LEFT_ICON_HEIGHT  16
#endif
#define debug(...)

// 状态移动宏：先将前一状态用当前状态更新，再将当前状态设为nextState
#define MOVE_TO_STATE(nextState) {            \
            ClockAppsState tpState;            \
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
#define CLOCK_DRAW_BOTTOMBAR(x)                           \
{                                                   \
    BottomBar_Param_type BarParam;                  \
    MEMSET(&BarParam, 0, sizeof(BarParam));         \
    BarParam.eBBarType = x;                         \
    DrawBottomBar(pMe->m_pDisplay, &BarParam);      \
}
//#define EVT_ALARMOFF            (EVT_USER + 3)  //闹钟定时响闹回调
//#define EVT_FOCUS_SWITCH        (EVT_USER + 4)  //焦点切换
//#define EVT_SAVE_ALERT_TIMEOUT  (EVT_USER + 5)  //保存提示对话框完毕后的回调
#define CLOCK_CFG_VERSION       0x1002
#define MAX_ALARM_MSECS         (55000)         //多久没有人响应关闭闹钟
#define ANI_CLOCK_IMG_MSECS     200             //画活动闹钟的周期
#define NUM_OF_ALARMCLOCK       6   //闹钟数目
#define MAX_ALARM_INFO          8   //提示信息最大字符数
#define BOTTOM_IMG_HIGHT        14  //菜单底条高度
#define CONTROL_RECT_START_X    45  //闹钟设置界面的控件开始位置
#define AECHAR_OF_STATIC_INFO   64  //闹钟界面的静态文本字符数
#define LENTH_OF_WALLPAPER_NAME 128 //wallpaper字符长度
#define HEIGHT_OF_SUB_ICON      16  //小屏ICON条的高度
#define HEIGHT_OF_FONT             14   //汉字的高度
#define CLOCK_SAVE_ALERT_MSECS  1000//保存提示信息的停留时间
#define SAVE_ALERT_X            4  //提示信息文本开始的x坐标
#define MSSEC_PER_HOUR          (60*60*1000)  //一小时内的毫秒数
#define MSSEC_PER_MIN           (60*1000)     //一分钟内的毫秒数
/*==============================================================================
                                 类型定义
==============================================================================*/
/*----------------------对话框相关数据类型声明---------------------*/
// 对话框关闭时返回值列表
typedef enum DLGRetValue
{
   // 初始值，表明需要创建对话框
   DLGRET_CREATE,

   // 对话框关闭时可返回的通用值
   DLGRET_OK,
   DLGRET_CANCELED,

   DLGRET_ALARMCLOCK_OPTION,

   DLGRET_ALARMCLOCK_SUB,
   DLGRET_POWERALARM_SUB,
   DLGRET_ALARMTIME_REACH,
   DLGRET_DELCONFIRM,
   DLGRET_MSGBOX_OK,
} DLGRet_Value_e_Type;

/*----------------------状态机相关数据类型声明---------------------*/
// ClockApps Applet 状态机状态：
typedef enum _ClockAppsState
{
   ALARMST_NONE,
   ALARMST_INIT,
   ALARMST_MAIN,
   ALARMST_OPTION,
   ALARMST_SUB,
   ALARMST_REACH,
   ALARMST_EXIT,

} ClockAppsState;

// 状态处理函数返回给状态处理主函数的值类型
typedef enum NextFSMAction
{
   NFSMACTION_WAIT,
   NFSMACTION_CONTINUE
} NextFSMAction;

// 保存模块信息和模块引用计数的结构
typedef struct ClockAppsMod
{
    DECLARE_VTBL(IModule)
    uint32   m_nRefs;
} ClockAppsMod;

//闹钟类型
typedef enum _AlarmType
{
   // 闹钟1
   WEEK_ALARM_1 = 0,

   // 闹钟2
   WEEK_ALARM_2,

   // 闹钟3
   WEEK_ALARM_3,

   // 闹钟4
   WEEK_ALARM_4,

   // 闹钟5
   WEEK_ALARM_5,

   // 闹钟6
   WEEK_ALARM_6,

   //闹钟7
   //WEEK_ALARM_7,

   //闹钟8
   //WEEK_ALARM_8
} AlarmType;


//闹钟类型
typedef enum _RepModeType
{
   // 每天
   WEEK_ALARM_REP1 = 3,

   // 周一~~周五
   WEEK_ALARM_REP2,

   // 周一~~周六
   WEEK_ALARM_REP3,

   // 周一
   WEEK_ALARM_REP4,

   // 周二
   WEEK_ALARM_REP5,

   // 周三
   WEEK_ALARM_REP6,

   // 周四
   WEEK_ALARM_REP7,

   // 周五
   WEEK_ALARM_REP8,

   // 周六
   WEEK_ALARM_REP9,

   // 周日
   WEEK_ALARM_REP10,

   // 一次
   WEEK_ALARM_REP11,
} RepModeType;
typedef enum _SnoozeType
{
//5 min
    SNOOZE_1 = 1,

//10min
    SNOOZE_2,

//15min
    SNOOZE_3,

//关闭
    SNOOZE_4
}SnoozeType;
//闹钟首选项的数据结构
typedef struct _ClockAppCfg
{
    boolean     bStateOn[NUM_OF_ALARMCLOCK];        //闹钟状态
    RepModeType RepMode[NUM_OF_ALARMCLOCK];         //重复方式
    uint32      Snooze[NUM_OF_ALARMCLOCK];          //SNOOZE  再响时间
    uint32      dwWATime[NUM_OF_ALARMCLOCK];        //闹钟时间,毫秒表示
#if 1
    AECHAR      wszInfo[NUM_OF_ALARMCLOCK][MAX_ALARM_INFO + 1]; //闹钟提示信息
#endif
} ClockAppCfg;


// ClockApps Applet对象结构体：
typedef struct _CClockApps
{
    DECLARE_VTBL(IClockApps)
    uint32          m_nRefs;          // 对象引用计数器
    IModule        *m_pModule;
    IDisplay       *m_pDisplay;
    IShell         *m_pShell;
    IStatic        *m_pStatic;
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
    ClockAppsState m_ePreState;
    // Applet 当前状态
    ClockAppsState m_eCurState;
    // Applet是否处于挂起状态
    boolean m_bSuspending;
    // Applet是否准备好可以处理按键和命令事件。改变量主要用于快速按键引起的问题
    boolean         m_bAppIsReady;

    uint16          m_currDlgId;        //保存当前所选菜单位置

    IConfig         *m_pConfig;         //IConfig接口指针

    ClockAppCfg     m_ClockCfg;         //闹钟首选项的数据结构

    AlarmType       m_eCurAlarmType;    //闹钟类型

    boolean         m_bWeekAlarmEnabled;//闹钟有效

    boolean         m_Snooze;           //SNOOZE是否有效

    IBacklight     *m_pBacklight;       //背光接口指针

    ISound         *m_pSound;

#if !defined( AEE_SIMULATOR)
    IALERT         *m_pAlert;           //IALERT指针
#endif
    PowerDown_Alarm_Cfg   m_PowerDownAlarmCfg;

    IMenuCtl       *m_pState;           //IMenuCtl指针,"状态"

    ITimeCtl       *m_pTime;            //ITimeCtl指针,"时间"

    IMenuCtl       *m_pRepMode;         //IMenuCtl指针,"重复模式"

    IMenuCtl       *m_pSnooze;          //IMenuCtl 指针，snooze

    ITextCtl       *m_pText;            //ITextCtl指针,"提示信息"

    uint16          m_nCtlID;           //控件ID

    uint16          m_nTimeCtlCount;    //0代表焦点在小时上,1代表在分钟上

    uint16          m_return;

#ifdef FEATURE_SUB_LCD
   IDisplay       *m_pDisplaySub;

   IBitmap         *m_pWallSubImage;
#endif

    uint16          m_nNumKeyCount;     /*记录在设置时间时的按键次数.0:小时十位
                                        1:小时个位;   2:分钟十位  3:分钟个位*/

    int32           m_dwDispTime;       //屏幕上显示的时间

    int32           m_dwHour;           //记录用数字键设置的小时

    int32           m_dwMin;            //记录用数字键设置的分钟

    boolean         m_bAlarmOff;        //闹钟响闹结束标志

    boolean         m_DelAll;           //是否删除全部闹钟

    //BOTTOMBAR       m_BottomBar;

    //boolean         m_AlarmReach;       //为了解决cr933,标注闹钟已到并且要显示它

    IAnnunciator        *m_pIAnn;

#if defined( FEATURE_CARRIER_SUDAN_SUDATEL) && !defined( AEE_SIMULATOR)
    nv_language_enum_type language;
#endif
   AEERect rectLine[4];
   boolean          m_bAlarmSuspend;
#ifdef FEATURE_LCD_TOUCH_ENABLE 
   AEERect          pL_Rect1;
   AEERect          pL_Rect2;
   AEERect          pL_Rect3;
   AEERect          pR_Rect1;
   AEERect          pR_Rect2;
   AEERect          pR_Rect3;
#endif
} CClockApps;


/*==============================================================================
                                 全局数据
==============================================================================*/
extern boolean bFlipClosed;    //翻盖是否关闭
/*==============================================================================
                                 本地（静态）数据
==============================================================================*/

/*==============================================================================
                                 函数声明
==============================================================================*/

/*----------------------对话框相关函数声明---------------------*/
/*==============================================================================
函数:
       ClockApps_ShowDialog
说明:
       函数由状态处理函数用来创建相应对话框资源ID的对话框。

参数:
       pMe [In]: 指向ClockApps Applet对象结构的指针,该结构包含小程序的特定信息.
       dlgResId [in]：对话框资源ID，函数根据该ID创建对话框。

返回值:
       无

备注:


==============================================================================*/
void ClockApps_ShowDialog(CClockApps *pMe,uint16  dlgResId);

/*==============================================================================
函数:
       ClockApps_RouteDialogEvent
说明:
       函数将BREW事件路由给当前活动对话框的事件处理函数。

参数:
       pMe [In]: 指向ClockApps Applet对象结构的指针,该结构包含小程序的特定信息.
       eCode [in]：事件代码。
       wParam [in]：与事件eCode关联的数据。
       dwParam [in]：与事件eCode关联的数据。

返回值:
       TRUE：传入事件被处理。
       FALSE：传入事件没被处理。

备注:

==============================================================================*/
boolean ClockApps_RouteDialogEvent(CClockApps *pMe,
    AEEEvent    eCode,
    uint16  wParam,
    uint32 dwParam
);

/*==============================================================================
函数:
       ClockApps_CanAlert
说明:
       函数用来确定是否可以唤起闹钟主界面，目前仅对通话情况做出判断，以后有其它
       要求可在此添加。

参数：
       pMe [in]：指向 AlarmClock Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       TRUE: 可以提示
       FALSE: 不可以提示


备注:


==============================================================================*/
boolean ClockApps_CanAlert(CClockApps *pMe);


/*----------------------状态机相关函数声明---------------------*/
/*==============================================================================
函数:
       ClockApps_ProcessState
说明:
       ClockApps Applet状态处理主函数。函数根据pMe->m_currState将调用路由至
       相应的处理函数。

参数:
       pMe [In]: 指向ClockApps Applet对象结构的指针,该结构包含小程序的特定信息.

返回值:
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注:

==============================================================================*/
NextFSMAction ClockApps_ProcessState(CClockApps *pMe);


//定时器函数，用来向Alarm錍lock Applet发出SNOOZE事件
void CClockApps_Snooze(CClockApps *pme);

#endif
//CLOCKAPPSPRIV_H


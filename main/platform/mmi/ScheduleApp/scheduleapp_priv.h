#ifndef SCHEDULEAPP_PRIV_H
#define SCHEDULEAPP_PRIV_H

/** ----------------------------------------------------------------------------
 *
 * History:
 *
 * when         who            what and why
 * -----------  -----------    -----------------------------
 * -----------------------------------------------------------------------------
 */


#ifndef AEE_SIMULATOR
#include "OEMFeatures.h"   //Handset Feature Definitions
#else
#include "BREWSimFeatures.h"
                  //Simulator Feature Definitions
#endif

#include "AEEShell.h"
#include "AEEModTable.h"
#include "AEEStdLib.h"
#include "AEEModGen.h"
#include "AEEAppGen.h"
#include "AEE_OEM.h"

#include "OEMClassIDs.h"
#include "AEE.h"
#include "AEEAlarm.h"
#include "AEEAlert.h"
#include "AppComFunc.h"
#include "Appscommon.h"
//
#include "ScheduleApp.h"
#include "scheduleapp.brh"
#include "AEEDate.h"
#include "AEEMenu.h"
#include "AEETAPI.h "
#ifdef FEATURE_LANG_CHINESE
#include "CalendarApp.brh"
#endif
//
#include "AEEDB.h"
#include "AEEText.h"
#include "AEETime.h"
#include "AEEFILE.h"
#include "AEEBacklight.h"
#include "AEEConfig.h"
#include "OEMCFGI.h"
#include "oemnvint.h"
#include "OEMCriticalSection.h"
#include "AEEAnnunciator.h"

#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
#define NEVT_KEY_PRESSS 10000
#endif


#define debug(...)
#if defined(FEATURE_VERSION_X3)||defined(FEATURE_VERSION_K202)
#ifdef FEATURE_LANG_CHINESE
#define     FEATURE_DRAW_LUNAR_CALENDAR     0
#else 
#define     FEATURE_DRAW_LUNAR_CALENDAR     1
#endif

#else
#ifdef FEATURE_LANG_CHINESE
#ifdef FEATURE_VERSION_EC99
#define     FEATURE_DRAW_LUNAR_CALENDAR     1
#else
#define     FEATURE_DRAW_LUNAR_CALENDAR     1
#endif
#else 
#define     FEATURE_DRAW_LUNAR_CALENDAR     0
#endif
#endif

#define     FEATURE_ONE_DB                  0
#define     FEATURE_TEST_HOOK               0
#define     HEIGHT_PROMPT_BAR   BOTTOMBAR_HEIGHT

#ifdef FEATURE_DRAW_LUNAR_CALENDAR
#ifdef FEATURE_VERSION_X3
#define     LUNAR_RECT_HEIGHT              24
#else
#ifdef FEATURE_VERSION_K212
#define     LUNAR_RECT_HEIGHT              32
#else
#define     LUNAR_RECT_HEIGHT              14
#endif
#endif
#endif
#if defined(FEATURE_DISP_128X160) || defined(FEATURE_DISP_128X128)
#define     MAX_INPUT_SUBJECT               14
#else
#define     MAX_INPUT_SUBJECT               20
#endif
#define     MAX_INPUT_LOCATION              64
#define     MAX_SPORTS_DISTANCE             5
#define     MAX_SPORTS_TIME                 3

#if defined(FEATURE_DISP_160X128)
#define ARROW_WIDTH           8
#define ARROW_HEIGHT          16

#elif defined(FEATURE_DISP_220X176)
#define ARROW_WIDTH           11
#define ARROW_HEIGHT          22

#else
#define ARROW_WIDTH           8
#define ARROW_HEIGHT          16

#endif

#if defined( AEE_SIMULATOR)
	#define  AEE_SCHEDULEAPP_RES_FILE ("fs:/mod/scheduleapp/en/" SCHEDULEAPP_RES_FILE)
	#define  AEE_CALENDARAPP_RES_FILE ("fs:/mod/scheduleapp/en/" CALENDARAPP_RES_FILE)
#else
	#define  AEE_SCHEDULEAPP_RES_FILE (AEE_RES_LANGDIR SCHEDULEAPP_RES_FILE)
	#define  AEE_CALENDARAPP_RES_FILE (AEE_RES_LANGDIR CALENDARAPP_RES_FILE)
#endif

#define num( wch) ((wch) - ((AECHAR)'0'))

#define OEM_IME_DIALOG   (0x7FFF)

#define STARTARG_SPORT    ((char) 'A')  //从 sports 进入 scheduleapp


// 状态移动宏：先将前一状态用当前状态更新，再将当前状态设为nextState
#define MOVE_TO_STATE(nextState)                 \
        {                                        \
            FSMState tpState;                    \
            tpState = nextState;                 \
            pme->m_ePreState = pme->m_eCurState; \
            pme->m_eCurState = tpState;          \
        }

//事件定义
#define EVT_APPISREADY              (EVT_USER+5)       
#define EVT_LOAD_DATA               (EVT_USER + 6)
 
#define APPISREADY_TIMER            200         
#define FILECFG                     1       //在该程序用了两种保存参数的方法
                                            //一是用BREW提供的CONFIG 二是写进
                                            //一个文件里。当这开关为1时为后者
//lunar calendar Macros
//阴历的宏定以
#define START_YEAR                  1900
#define START_YEAR_M                1899
#define START_MONTH                 1
#define START_DAY                   1

#define YEAR_VIEW                   0
#define MONTH_VIEW                  1

#define MAX_LEN_MSGBOX              (128)
#define MAX_LEN_TITLE               (32)
#define TIME_ONE_SEC                (1000)
#define TIMEOUT_MS_MSGBOX           (2*TIME_ONE_SEC) //提示信息的显示的时间
#define MAXHISTORY                  20          //最大的记录数

//#define MAXTEXT                     64          //每个记录的最大容量
#define MAXTEXT						MAX_INPUT_LOCATION

#define TIMEOUT_MS_QUICK            100         //允许连续快速按键的时间间隔

#define CAL_DATABASE_PATH           "fs:/mod/scheduleapp"     //数据库的路径名
#define CAL_DATABASE_NAME           "calendar.db"     //数据库的文件名
#define FILE_SCHEDULE_CFG           "schedulecfg"  //日程表的配置文件名
#define SCHEDULER_MIN_SIZE          1000           //
#define SCHEDULER_MIN_RECS          20             //定义日程表所支持的最大记录

#define CAL_CFG_VERSION             0x1001         //

#define NUMFLDS                     (FLD_LAST - FLD_NAME_BASE)//每个记录有多少个字段
#define FLD_NAME_BASE               0x80
#define EMPTY_ALARM_ID              0xFFFF
#define MINUTES_PER_HOUR            (60)



#define MAKECOMPTIME(jdate,time)    (((jdate)<<16)|(time))
#define GETDATE(comptime)           (((comptime)>>16) & 0x0000FFFF)
#define GETTIME(comptime)           ((comptime) & 0x0000FFFF)
#define GET_HOUR(t)                 ((t)/MINUTES_PER_HOUR)
#define GET_MINUTE(t)               ((t)%MINUTES_PER_HOUR)

#define sendEvent( e)       ISHELL_SendEvent(pme->m_pShell, AEECLSID_SCHEDULEAPP, e, 0, 0)
#define postEvent( e)       ISHELL_PostEvent(pme->m_pShell, AEECLSID_SCHEDULEAPP, e, 0, 0)
#define loadData()          postEvent( EVT_LOAD_DATA)

#define drawBottomBar( bottomBarType)               \
{                                                   \
    BottomBar_Param_type BarParam;                  \
    MEMSET(&BarParam, 0, sizeof(BarParam));         \
    BarParam.eBBarType = bottomBarType;             \
    DrawBottomBar(pme->m_pDisplay, &BarParam);      \
}



#if FEATURE_DRAW_LUNAR_CALENDAR
typedef struct CCalendarApp {
   IShell      *c_pIShell;
   uint16      m_yYear;       //存放当前阴历年, 月, 日
   uint16      m_yMonth;
   uint16      m_yDay;

   int         adjustDay;     //存放按键所改变的日期天数

   AECHAR      mGz_String[121];              //天干      60*2 +1
   AECHAR      mSx_String[25];               //属相      12*2 +1
   AECHAR      mLunMonth_String[25];         //农历月    12*2 +1
   AECHAR      mLunLeapMonth_String[37];     //农历闰月  12*3 +1
   AECHAR      mLunDay_String[61];           //农历日    30*2 +1
   AECHAR      *m_pResGz, *m_pResSx, *m_pResLunM, *m_pResLunD;

   uint32      gLunarMonthDay[152];   //数组gLunarDay存入阴历1900年到2051年每年中的月天数信息，
   uint8       gLunarMonth[77];       //数组gLanarMonth存放阴历1899年到2052年闰月的月份
   uint32      monthday[12];          //数组monthday存放0-11个月总天数

   //boolean     m_OverFlag;     //存放越界标志
   boolean     m_LeapMonth;      //闰月标志

}CCalendarApp;
#endif


/*----------------------对话框相关数据类型声明---------------------*/
// 对话框关闭时返回值列表
typedef enum _DLGRetValue
{
    // 初始值，表明需要创建对话框
    DLGRET_CREATE,

    // 对话框关闭时可返回的通用值
    DLGRET_OK,
    DLGRET_CANCELED,

    // for option
    DLGRET_OPTION_NEW_EVENT,
    DLGRET_OPTION_VIEWDAY_DELETE,
    DLGRET_OPTION_VIEWMONTH_VIEW,
    DLGRET_OPTION_DELETE_TODAY,
    DLGRET_OPTION_DELETE_MONTH,
    DLGRET_OPTION_DELETE_ALL,
    DLGRET_OPTION_SETUP,
    DLGRET_OPTION_BACK_TO_VIEWMONTH,
    DLGRET_OPTION_BACK_TO_VIEWDAY,
    DLGRET_OPTION_VIEWDAY_TO_VIEWEVENT,

    // for event edit
    DLGRET_EVENT_EDIT_SUBJECT,
    DLGRET_EVENT_EDIT_NOTE,


    DLGRET_VIEWDAY_EDIT_EVENT,

    DLGRET_TO_SHOWALERT,
    DLGRET_SHOWALERT_TO_VIEWEVENT,

    DLGRET_PASS,
    DLGRET_FAILD,
    DLGRET_MSGBOX_CANCEL,
    DLGRET_MSGBOX_OK,
    DLGRET_GOTODATE
    

} DLGRetValue;

/*----------------------状态机相关数据类型声明---------------------*/
// ScheduleApp Applet 状态机状态：
typedef enum _FSMState
{
    STATE_NONE,
    STATE_INIT,
    STATE_PWD,
    STATE_VIEWMONTH,
    STATE_OPTION,
    STATE_GOTODATE,
    STATE_VIEWDAY,
    STATE_EVENT_EDIT,
    STATE_INPUTTEXT,
    STATE_SETUP,
    STATE_SHOWALERT,
    STATE_VIEWEVENT,
    STATE_EXIT
} FSMState;

typedef enum
{
    SUBSTATE_NONE,

    SUBSTATE_VIEWDAY_DELETE_CONFIRM,

    SUBSTATE_DELETE_ALL_CONFIRM,
    SUBSTATE_DELETE_TODAY_CONFIRM,
    SUBSTATE_DELETE_MONTH_CONFIRM,

    SUBSTATE_OPTION_VIEWMONTH,
    SUBSTATE_OPTION_VIEWDAY,

    SUBSTATE_EVENT_EDIT_NEW,
    SUBSTATE_EVENT_EDIT_EDIT
} FsmSubState;

typedef enum
{
    DATE_SEL_YEAR,
    DATE_SEL_MONTH,
    DATE_SEL_DAY
} Sel_e_type;


// 状态处理函数返回给状态处理主函数的值类型
typedef enum _NextFSMAction
{
	NFSMACTION_WAIT,
	NFSMACTION_CONTINUE
} NextFSMAction;


//区分复制和转存操作
typedef enum _Selected
{
	init,
	copyto,
	moveto
}Select;

//提示方式
typedef enum _AlertStates
{
	INIT,
	SILENCE,
	ALERTSET,
	VIBRATE,
	TIME_SMS
}AlertStates;

typedef enum _WalkMode
{
    WALK,//RUN 
    RUN //WALK
}WalkMode;

//数据库字段
typedef enum {
  FLD_ALARM = FLD_NAME_BASE,  // AEEDB_FT_BINARY
  FLD_DESC,                   // AEEDB_FT_STRING
  FLD_NOTE,                   // AEEDB_FT_STRING
  FLD_ID,                     // AEEDB_FT_WORD
  FLD_LAST
} RecordFieldTypeEnum;


//日程表程序的配置信息
typedef struct _CalCfg
{
   uint16       nAutoDelete;   //自动删除
   uint16       wDayStart;     //开始时间
   uint16       nSnooze;

#if !FEATURE_ONE_DB
   uint16       maxPermId;
#endif
} CalCfg;


//typedef boolean   (*PFNKEYHANDLER)  (CCalApp * pme, int nKeyCode);

//
//   For compactness and ease of sorting, the CalEvent struct stores
//   the date/time in a single long variable which is a composite
//   of the julian date and the starting time in minutes.
//
//      composite_time = (julian_date << 16) | start_time_in_minutes;
//

typedef struct _CalBaseFields
{
   uint32   dwTime;     // 事件的日期和时间
   uint16   wDuration;  // Minutes 该事件所用的的时间
   uint16   wAlarmTime; // Minutes 闹钟要提前多少分钟响
   AlertStates  alertst;
   WalkMode walk;
} CalBaseFields;


typedef struct CalEvent    CalEvent;
typedef struct _CCalApp    CCalApp;

struct CalEvent
{
   CalEvent    *  pNext;
   CalEvent    *  pPrev;
   uint16         nCurrRecID;  // current database Id for this record
                               // (only valid so long as the database is
                               //  open, may use IDATABASE_GetByRecordID()
                               //  with this value)
   CalBaseFields  m_b;
   uint32         m_lAlarmDateTime; //闹钟提醒的日期和时间
   uint16         nPermRecID;  // Perminant record Id stored with the record
                               // in the database.  Not necessarily the same
                               // as nCurrRecID.
};

typedef struct _CalDBRecFields
{
   uint16         nCurrRecID;  // current database Id for this record
                               // (only valid so long as the database is
                               // open, may use IDATABASE_GetByRecordID()
                               // with this value)
   CalBaseFields  b;
   AECHAR *       pszDesc;     // 指向当前事件的标题
   int            nDescLen;    // 标题字符的长度
   AECHAR *       pszNote;     //
   int            nNoteLen;
   uint16         nPermRecID;  // Perminant record Id stored with the record
                               // in the database.  Not necessarily the same
                               // as nCurrRecID.
} CalDBRecFields;

typedef enum
{
    NO_ALARM,
    NOT_EXPIRED,
    EXPIRED,
    DONE
} AppointmentStatus;

typedef struct
{
   uint16               dbId;
   uint16               permId;
   uint16               CurEveType;
   CalBaseFields        baseFields;
   AECHAR               subject[MAX_INPUT_SUBJECT+1];//distance
   AECHAR               location[MAX_INPUT_LOCATION+1];//time
   AppointmentStatus    status;
   JulianType           julian;
} Appointment;


//该数据结构主要负责数据库的读写和相关的操作
struct _CCalApp
{
   IShell           *m_pIShell;
   IDisplay         *m_pIDisplay;

   CalCfg            m_cfg;
   IDBMgr           *m_pIDBMgr;
   IDatabase        *m_pIDatabase;
#ifdef FEATURE_UIALARM
   IAlarm      *m_pIAlarm;
#endif


#if defined(FILECFG)
  IFileMgr          *m_pFileMgr;            //配置文件接口
#endif

  IALERT            *m_alert;               //播放铃声的接口

   uint32            m_lCurrentDay;         //当前的日期
   uint32            m_lToday;

   AECHAR            m_szEventDes[MAX_INPUT_SUBJECT+1]; //约会的标题
   AECHAR            m_szNote[MAX_INPUT_LOCATION+1];     //约会的内容
   uint32            m_lEventDay;           //当前日期
   int32             m_lStartTime;          //约会开始时间
   int32             m_lDuration;           //约会的时长
   uint16            m_nAlarmTime;          //约会闹铃时间

   CalEvent*         m_pceCurrent;          //跟踪当前的一个事件
   int               m_dbOperationResult;          //保存数据操作的返回值
   uint32            m_dwMask;              //记录某个月的有约会的日子
   uint16            m_wPendingAlarmPermID; //到期事件的闹铃ID
   uint16            m_nNextPermRecID;      //建立数据库记录时的记录的ID
   AlertStates        m_AlertState;          //保存当前事件闹铃的方式


   CalEvent*            m_pMonthEvents[31];
   JulianType           m_julianCurrentDay;
   uint32               m_dwTotalEventsNumber;
   uint32               m_dwThisMonthEventsNumber;
   uint16               m_viewDaySelectedEventID;
   Appointment**        m_expiredAppointment;
   Appointment*         m_appointmentToView;
   WalkMode             m_CurMode;
   boolean				m_bCloseFromTimer;		//Add By zzg 2012_03_13
};

// ScheduleApp Applet对象结构体：
typedef struct _CScheduleApp
{
    DECLARE_VTBL(IScheduleApp)
    uint32              m_nRefs;          // 对象引用计数器
    IModule            *m_pModule;
    IDisplay           *m_pDisplay;
    IShell             *m_pShell;
    AEERect             m_rc;   

    CCalApp             m_CalMgr;

    IDialog            *m_pActiveDlg;
    uint16              m_pActiveDlgID;
    DLGRetValue         m_eDlgRet;
    boolean             m_bNotOverwriteDlgRet;
    FSMState            m_ePreState;
    FSMState            m_eCurState;
    boolean             m_bInactive;
    boolean             m_bSuspended;
    boolean             m_bAppIsReady;

    boolean             m_bStartAppletCommandQueued;
    AECHAR              m_pVDTitle[30];
    uint16              ALERTID;
    uint16              m_wMsgResID;
    uint16              m_nsel;
    ISound              *m_pSound;
    AECHAR              wstrYear[5];
    AECHAR              wstrMonth[3];
    AECHAR              wstrDay[3];
    byte                curSel;
    boolean             update;
    char                m_AppStartArgs;
    boolean             m_sports;// TRUE表示从 sports 模块进入，  FALSE表示正常进入


    AECHAR*             m_pszEventEditText;
    uint16              m_eventEditItem;
    boolean             m_closeByPowerKey;

    FSMState            m_stateToBackOption;
    FSMState            m_stateToBackEventEdit;
    FSMState            m_stateToBackSetup;
    FSMState            m_stateToBackShowAlert;
    FSMState            m_stateToBackViewEvent;

    FsmSubState         m_subStateOption;
    FsmSubState         m_subStateViewMonth;
    FsmSubState         m_subStateViewDay;
    FsmSubState         m_subStateEventEdit;


#if FEATURE_DRAW_LUNAR_CALENDAR
    //农历的字符串
    CCalendarApp     m_calendar;
    AECHAR              m_LunarString[40];
#endif
    char               *m_strPhonePWD;
    boolean             m_bshowEventDetail; //modify for snooze and see detail when alerting
    boolean             m_bRepaint;  //modify for phone die in display loading menu when incoming call
    boolean             m_bDeferred; //for phone die in display loading menu when incoming call
    AEERect  rc_year;
	AEERect  rc_month;
	AEERect  rc_day;
    IAnnunciator *m_pIAnn;
} CScheduleApp;

/*=============================================================================
                               全局变量申明或定义
===============================================================================*/
/*----------------------对话框相关函数声明---------------------*/
/*==============================================================================
函数:
	   CScheduleApp_ShowDialog
说明:
	   函数由状态处理函数用来创建相应对话框资源ID的对话框。

参数:
	   pme [In]: 指向ScheduleApp Applet对象结构的指针,该结构包含小程序的特定信息.
	   dlgResId [in]：对话框资源ID，函数根据该ID创建对话框。

返回值:
	   SUCCESS: 创建对话框成功
	   非SUCCESS: 创建对话框失败

备注:


==============================================================================*/
int CScheduleApp_ShowDialog(CScheduleApp *pme, uint16  dlgResId);

/*==============================================================================
函数:
	   CScheduleApp_RouteDialogEvent
说明:
	   函数将BREW事件路由给当前活动对话框的事件处理函数。

参数:
	   pme [In]: 指向ScheduleApp Applet对象结构的指针,该结构包含小程序的特定信息.
	   eCode [in]：事件代码。
	   wParam [in]：与事件eCode关联的数据。
	   dwParam [in]：与事件eCode关联的数据。

返回值:
	   TRUE：传入事件被处理。
	   FALSE：传入事件没被处理。

备注:

==============================================================================*/
boolean CScheduleApp_RouteDialogEvent( CScheduleApp  *pme,
                                       AEEEvent   eCode,
                                       uint16     wParam,
                                       uint32     dwParam);


/*----------------------状态机相关函数声明---------------------*/
/*==============================================================================
函数:
	   CScheduleApp_ProcessState
说明:
	   ScheduleApp Applet状态处理主函数。函数根据pMe->m_currState将调用路由至
	   相应的处理函数。

参数:
	   pme [In]: 指向ScheduleApp Applet对象结构的指针,该结构包含小程序的特定信息.

返回值:
	   NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
	   NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注:

==============================================================================*/
NextFSMAction CScheduleApp_ProcessState(CScheduleApp *pme);


/*==============================================================================
函数:
	   CalMgr_InitAppData
说明:
	   初始CCalApp结构的数据

参数:
	   pme [In]: 指向CalMgr 对象结构的指针,该结构包含小程序的特定信息.

返回值:
	   TRUE：传入事件被处理。
	   FALSE：传入事件没被处理。

备注:

==============================================================================*/

boolean CalMgr_InitAppData(CCalApp *pme, IShell* pIShell, IDisplay *pIDisplay);
/*==============================================================================
函数:
	   CalMgr_FreeAppData
说明:
	   释放CalMgr结构的数据

参数:
	   pme [In]: 指向CCalApp对象结构的指针,该结构包含小程序的特定信息.


返回值:
	   无

备注:

==============================================================================*/
void CalMgr_FreeAppData(CCalApp *pme);

/*==============================================================================
函数:
	   Cal_HandleAlarm
说明:
	   处理 EVT_ALARM 事件函数

参数:
	   pme [In]: 指向CScheduleApp对象结构的指针,该结构包含小程序的特定信息.
	   wPermID ： ALARM的ID
返回值:

备注:

==============================================================================*/

void Cal_HandleAlarm(CScheduleApp  *pme, uint16 wPermID);

/*==============================================================================
函数:
	   CScheduleApp_ShowMsgDialog
说明:
	   函数将BREW事件路由给当前活动对话框的事件处理函数。

参数:
	   pme [In]: 指向ScheduleApp Applet对象结构的指针,该结构包含小程序的特定信息.
	   msgResId [in]：message id
	   msg [in]：信息指针
	   msglen [in]：信息长度。

返回值:
	   TRUE：传入事件被处理。
	   FALSE：传入事件没被处理。

备注:

==============================================================================*/
void  CScheduleApp_ShowMsgDialog(CScheduleApp   *pme,uint16 msgResId);

extern void OEMOS_Sleep(uint32 nMSecs);

#if FEATURE_DRAW_LUNAR_CALENDAR
boolean Calendar_InitAppData(CCalendarApp * pme, IShell* pIShell);
void Calendar_FreeAppData(CCalendarApp * pme);
#endif


void closeDatabaseIf( CCalApp *pme);
void deleteAllEventsFromDB( CCalApp *pme);

void repaint(CScheduleApp *pme, boolean deferred); 

#endif//SCHEDULEAPP_PRIV_H

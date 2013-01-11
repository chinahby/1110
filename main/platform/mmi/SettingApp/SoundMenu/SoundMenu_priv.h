#ifndef SOUNDMENUPRIV_H
#define SOUNDMENUPRIV_H

/*==============================================================================
// 文件：
//        SoundMenu_priv.h
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
   #include "SoundMenu.bid"
#else
   #include "OEMClassIds.h"  // Applet Ids
   #ifndef AEECLSID_APP_SOUNDMENU
      #error AEECLSID_APP_SOUNDMENU must be defined
   #endif
#endif

#include "SoundMenu.brh"
#include "appscommon.brh"
#include "AEEShell.h"
#include "AEEModTable.h"
#include "AEEStdLib.h"
#include "AEEModGen.h"
#include "AEEAppGen.h"
#include "OEMClassIDs.h"
//#include "OEMMacros.h"
#include "SoundMenu.h"

#include "AEEMenu.h"
#include "AEEConfig.h"
#include "AppComFunc.h"
#include "Appscommon.h"
#include "AEERinger.h"
#include "AEEAlert.h"
//#include "AEEPhone.h"
//#include "cm.h"
//#include "cmcall.h"

#include "AEEVector.h"

#include "AEEAnnunciator.h"         // for ANNUN_ALERT
#include "OEMCFGI.h"

#if defined( FEATURE_RECORDER)
#include "recorder.h"
#include "recorder.brh"
#define  APP_RECORDER_RES_FILE ("fs:/mod/recorder/" AEE_RES_LANGDIR RECORDER_RES_FILE)
#endif

#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
#define SOUNDMENU_PT_IN_RECT(a,b,rct)      (boolean)( ((a) >= (rct).x && (a) <= ((rct).x + (rct).dx)) && ((b) >= (rct).y && (b) <= ((rct).y + (rct).dy)) )
#endif//FEATURE_LCD_TOUCH_ENABLE
/*----------------------对话框相关数据类型声明---------------------*/
#define  AEE_APPSSOUNDMENU_RES_FILE AEE_RES_LANGDIR SOUNDMENU_RES_FILE
#define SOUND_ERR(format, code1, code2, code3) //ERR(format, code1, code2, code3)
// 对话框关闭时返回值列表
typedef enum DLGRetValue
{
   DLGRET_CREATE,          //初始值，表明需要创建对话框
   DLGRET_OK,              //对话框关闭时可返回的通用值
   DLGRET_CANCELED,
   DLGRET_SCENEMODESUB,    //IDD_SCENEMODE_SUB
   DLGRET_SOUNDMENU,       //IDD_SOUNDMENU_MENU
   DLGRET_HINT,              //IDD_HINT_MENU
   DLGRET_RINGER,            //IDD_RINGER_MENU
#if defined(FEATURE_WMS_APP) && !defined(FEATURE_WMSAPP_ONLYSUPPORTVMAIL)
   DLGRET_SMSRING,         //IDD_SMSRING_MENU
#endif   
   DLGRET_OTHERSEL,        //IDD_OTHERSEL_MENU
   //DLGRET_AUTOANSWERSUB,   //IDD_AUTOANSWER_SUB
   DLGRET_KEYSND,          //IDD_KEYSND_MENU
   //DLGRET_KEYLENGTH,       //IDD_KEYTONE_LENGTH
   DLGRET_VOLUME,          //IDD_VOLUME_MENU
   DLGRET_VOLUMESUB,       //IDD_VOLUME_SUB
#if 0
   DLGRET_WARNING,         //IDD_WARNING_MESSEGE
#endif
   DLGRET_MESSAGE,          //IDD_MESSAGE_DIALOG
   DLGRET_MSGBOX_OK,
   DLGRET_FMMODE,
   DLGRET_MSGBOX_CANCEL

} DLGRet_Value_e_Type;

/*----------------------状态机相关数据类型声明---------------------*/
// SoundMenu Applet 状态机状态：
typedef enum _SoundMenuState
{
   SOUNDMENUST_NONE,
   SOUNDMENUST_INIT,
   SOUNDMENUST_MAIN,
   SOUNDMENUST_SCENEMODESUB,
   SOUNDMENUST_SOUNDMENU,
   SOUNDMENUST_HINT,
   SOUNDMENUST_RINGER,
#if defined(FEATURE_WMS_APP) && !defined(FEATURE_WMSAPP_ONLYSUPPORTVMAIL)
   SOUNDMENUST_SMSRING,
#endif   
   SOUNDMENUST_OTHERSEL,
   //SOUNDMENUST_AUTOANSWERSUB,
   SOUNDMENUST_KEYSND,
   //SOUNDMENUST_KEYLENGTH,
   SOUNDMENUST_VOLUME,
   SOUNDMENUST_VOLUMESUB,
   SOUNDMENUST_FMMODE,
#if 0
   SOUNDMENUST_WARNING,
#endif
   SOUNDMENUST_EXIT

} SoundMenuState;

// 状态处理函数返回给状态处理主函数的值类型
typedef enum NextFSMAction
{
   NFSMACTION_WAIT,
   NFSMACTION_CONTINUE
} NextFSMAction;

// 保存模块信息和模块引用计数的结构
typedef struct SoundMenuMod
{
    DECLARE_VTBL(IModule)
    uint32   m_nRefs;
} SoundMenuMod;

// 情景模式
typedef enum {
   SET_PROFILE_NONE,
   SET_NORMALMODE,      //正常模式
   SET_QUIETMODE,       //安静模式
   SET_MEETING,         //会议模式
   SET_NOISEMODE,       //户外模式
   SET_CARMODE          //车载模式
#ifdef FEATURE_VERSION_C316
   ,SET_BLUETOOTH       //蓝牙模式
#endif
} SET_PROFILE_TYPE;

// 情景模式操作类型
typedef enum {
   SET_SCENEMODESUB_NONE,
   SET_ACTIVATE,         //启动
   SET_PROFILE_RESTORE   //恢复默认设置
} SET_SCENEMODESUB_TYPE;

// 音量类型
typedef enum {
   SET_NONE,
   SET_RINGER_VOL,      //铃声
   SET_EARPIECE_VOL,    //耳机
   SET_KEYBEEP_VOL      //键盘
} SET_VOLUME_TYPE;

// 其他声音类型
typedef enum {
    SET_OTHERVOL_NONE,
    SET_MISSEDCALLALERT, //未接来电提醒   
    SET_POWERONOFFALERT//开关机铃声提示
    //SET_STARTUP,         //开机音乐
    //SET_SHUTDOWN        //关机音乐 
} SET_OTHERVOL_TYPE;

// 铃声设置类型
typedef enum {
    SET_RINGER_NONE,
    SET_RINGER,             //来电铃声
    SET_ALARMRING,        //闹钟铃声
    SET_STARTUP,           //开机音乐
    SET_SHUTDOWN,        //关机音乐
#if 1//def FEATURE_SMSTONETYPE_MID
    SET_SMSTONE,        //短信铃声
#endif //#if defined FEATURE_SMSTONETYPE_MID		    
    SET_ANOTHER       //短信铃声 
} SET_RINGER_TYPE;

//提示方式
typedef enum {
    SET_Hint_NONE,
    SET_CALLHINT,        //来电提示方式
    SET_SMSHINT          //短信提示方式
} SET_HINT_TYPE;

typedef struct  _RingerMenuItemInfo
{
    uint32  id;
    AECHAR szName[MAX_RINGER_NAME];
} RingerMenuItemInfo;

// 用于生成铃声列表菜单页的方式
typedef enum _MAKE_MenuList_Mode
{
   MAKELIST_INIT,         // 从列表开始处建立
   MAKELIST_PREPAGE,      // 菜单列表回到前一页
   MAKELIST_NEXTPAGE,     // 菜单列表前进一页
   MAKELIST_RESUMECURPAGE // 使用当前菜单列表
} MAKE_MenuList_Mode;
// SoundMenu Applet对象结构体：
typedef struct _CSoundMenu
{
    DECLARE_VTBL(ISoundMenu)
    uint32          m_nRefs;          // 对象引用计数器
    IModule        *m_pModule;
    IDisplay       *m_pDisplay;
    IShell         *m_pShell;

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
    SoundMenuState m_ePreState;
    // Applet 当前状态
    SoundMenuState m_eCurState;
    // Applet是否处于挂起状态
    boolean m_bSuspending;
    // Applet是否准备好可以处理按键和命令事件。改变量主要用于快速按键引起的问题
    boolean         m_bAppIsReady;

    IConfig         *m_pConfig;        //IConfig 指针

    uint16           m_currDlgId;      //一级菜单位置
    uint16           m_nSubDlgId;      //次级菜单位置
    uint16           m_sSubDlgId;      //三级菜单位置
    uint16           m_fSubDlgId;      //四级菜单位置

    IRingerMgr      *m_pRingerMgr;
    IALERT          *m_pAlert;
    ISound          *m_pISound;
    //IPhone          *m_phone;

    IVector         * m_pRingerList;

    ringID            m_RingerID[PROFILENUMBER];
    // 用户设置的铃声 ID
    uint16            m_RingID[PROFILENUMBER];

    uint16           m_lastRingerPlayed; //最后一个播放的铃声
#if defined(FEATURE_WMS_APP) && !defined(FEATURE_WMSAPP_ONLYSUPPORTVMAIL)
    uint16           m_lastSmsRingPlayed;//最后一个播放的SMS铃声
#endif    

    SET_PROFILE_TYPE   m_ProfileType;   //情景模式的类型
    SET_VOLUME_TYPE    m_VolType;       //音量的类型
    SET_OTHERVOL_TYPE  m_OtherVolType;  //其他声音类型
    SET_RINGER_TYPE    m_RingerType;    //铃声设置类型
    SET_HINT_TYPE      m_HintType;      //提示方式
    SET_SCENEMODESUB_TYPE  m_ScenemodesubType;  //情景模式操作类型

    uint16             m_wResID;          //标题ID

    //IMenuCtl        *m_pMenu;  
    boolean            m_bIsSuspend;      //启动铃声图片applet返回后选择条的位置

    MAKE_MenuList_Mode m_eMakeListMode;

    uint16             m_wCurPageNum;

    byte               m_RingVol;          //铃声音量
    byte                m_CurProfile;      //当前情景模式
    byte                m_bNaturalStart;   //非常规方式启动的状态机
    byte                m_btTep;
    byte                m_active;          //当前启动情景模式值
    byte                m_RingCurVol[PROFILENUMBER];   //当前铃声音量
    IAnnunciator         *m_pIAnn;
    AECHAR              *m_enter_address;
    boolean            m_ringer_be;
	//MODI BY YANGDECAI 09-27
	uint16             m_slecet_id;
    //boolean             m_bBTHeadSetConnected;      //Add By zzg 2013_01_09
} CSoundMenu;

/*==============================================================================
                                 宏定义和常数
==============================================================================*/
// 建立铃声列表菜单时一页的最大项目数
#define MAXITEMS_ONEPAGE   40

// 更新菜单列表事件
//#define EVT_UPDATAMENU (EVT_USER+16)

// 为防止用户快速按键而添加事件EVT_APPISREADY，事件由定时器
// SoundMenu_APPIsReadyTimer发出
#define APPISREADY_TIMER       100                   
#define EVT_APPISREADY   (EVT_USER+1)                

// 用户重新更新界面事件
//#define EVT_USER_REDRAW   (EVT_USER+2)

// 提示信息回调事件
//#define EVT_DIALOGTIMEOUT    (EVT_USER+3)

//关闭程序事件(启动情景模式子菜单后Post给SettingMenu)
//#define EVT_USER_CLOSEAPP (EVT_USER+13)

// 状态移动宏：先将前一状态用当前状态更新，再将当前状态设为nextState
#define MOVE_TO_STATE(nextState) {            \
            SoundMenuState tpState;            \
            tpState = nextState;          \
            pMe->m_ePreState = pMe->m_eCurState; \
            pMe->m_eCurState = tpState;        \
        }
#define  SOUND_MENU_SOFK_HIGHT 0
#define  NUM_OF_MAXRINGER   256                 //最大铃声数
#define  DEFAULT_MENU       NUM_OF_MAXRINGER+1  // 默认ID , for contact
#define  DOWNLOAD_MENU      NUM_OF_MAXRINGER+2  //下载铃声ID
#if defined(FEATURE_WMS_APP) && !defined(FEATURE_WMSAPP_ONLYSUPPORTVMAIL)
#define  MAX_SMS_RINGER     6                   //最大短信铃声数
#endif

#define STARTARG_SOUNDMENU  ((char) 'S')        //从设置菜单进入声音菜单时候的标志字符,Gemsea050301 add
#define STARTARG_AUTOANSWERSUB  ((char) 'a')    //从通话设置菜单进入自动接听菜单时候的标志字符,Gemsea050310 add
#define STARTARG_RINGER_LIST  ((char) 'R')

#define NORMAL_ENTRY        0                   //从情景模式菜单调用常规方法启动SoundMenu Applet
#define SOUNDLIST_ENTRY     1                   //从声音设置菜单调用ISoundMenu_SoundList接口启动SoundMenu Applet
#define AUTOANSWER_ENTRY    2                   //从自动关机菜单调用ISoundMenu_Autoanswer接口启动SoundMenu Applet
#define ENTER_RINGER_LIST    3 
#define SOUNDMENU_VOLUME_LEVELS    6
#define POWERONRINGID         13   //ADD BY YANGDECAI 09-27
#define POWEROFFRINGID        14

#define SOUNDMENU_VOLUME_IMAGE     "fs:/image/notice/volume.png"
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
       SoundMenu_ShowDialog
说明:
       函数由状态处理函数用来创建相应对话框资源ID的对话框。

参数:
       pMe [In]: 指向SoundMenu Applet对象结构的指针,该结构包含小程序的特定信息.
       dlgResId [in]：对话框资源ID，函数根据该ID创建对话框。

返回值:
       无

备注:


==============================================================================*/
void SoundMenu_ShowDialog(CSoundMenu *pMe,uint16  dlgResId);

/*==============================================================================
函数:
       SoundMenu_RouteDialogEvent
说明:
       函数将BREW事件路由给当前活动对话框的事件处理函数。

参数:
       pMe [In]: 指向SoundMenu Applet对象结构的指针,该结构包含小程序的特定信息.
       eCode [in]：事件代码。
       wParam [in]：与事件eCode关联的数据。
       dwParam [in]：与事件eCode关联的数据。

返回值:
       TRUE：传入事件被处理。
       FALSE：传入事件没被处理。

备注:

==============================================================================*/
boolean SoundMenu_RouteDialogEvent(CSoundMenu *pMe,
    AEEEvent    eCode,
    uint16  wParam,
    uint32 dwParam
);

/*----------------------状态机相关函数声明---------------------*/
/*==============================================================================
函数:
       SoundMenu_ProcessState
说明:
       SoundMenu Applet状态处理主函数。函数根据pMe->m_currState将调用路由至
       相应的处理函数。

参数:
       pMe [In]: 指向SoundMenu Applet对象结构的指针,该结构包含小程序的特定信息.

返回值:
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注:

==============================================================================*/
NextFSMAction SoundMenu_ProcessState(CSoundMenu *pMe);

/*==============================================================================
函数：
       SoundMenu_InitRingerList

说明：
       函数用于初始铃声链表。

参数：
       pMe [in]：指向SoundMenu Applet对象结构的指针。
                           该结构包含小程序的特定信息。

返回值：
       无。

备注：:

==============================================================================*/
void SoundMenu_InitRingerList(CSoundMenu *pMe);

/*==============================================================================
函数：
       SoundMenu_UpdateRingerListMenu

说明：
       更新铃声列表菜单。

参数：
       pMe [in]：指向SoundMenu Applet对象结构的指针。
                           该结构包含小程序的特定信息。
       pMenu [in/out]：IMenuCtl接口指针。

返回值：
        无

备注：

==============================================================================*/
void SoundMenu_UpdateRingerListMenu(CSoundMenu *pMe, IMenuCtl *pMenu);


#endif
//SOUNDMENUPRIV_H


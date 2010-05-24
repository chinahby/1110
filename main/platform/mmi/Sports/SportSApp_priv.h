#ifndef _SPORTSAPPPRIV_H_
#define _SPORTSAPPPRIV_H_

/*==============================================================================
// 文件：
//        SportSApp_priv.h
//        版权所有(c) 2009 Anylook Tech. CO.,LTD.
//
// 文件说明：
//
// 作者：Gemsea
// 创建日期：2008-11-04
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
   #include "SportsMenu.bid"
#else
   #include "OEMClassIds.h"  // Applet Ids
   #ifndef AEECLSID_SPORTSAPP
      #error AEECLSID_SPORTSAPP must be defined
   #endif
#endif

#include "pedometer.brh"
#include "sportsmenu.brh"
#include "walk.brh"
#include "run.brh"

#include "AEEShell.h"
#include "AEEModTable.h"
#include "AEEStdLib.h"
#include "AEEModGen.h"
#include "AEEAppGen.h"
#include "OEMClassIDs.h"
//#include "OEMMacros.h"
#include "SportSApp.h"

#include "AEEMenu.h"
#include "AEEConfig.h"
#include "AppComFunc.h"
#include "AEEText.h"
#ifndef WIN32
#include "Oemui.h"
#else
#include "oemhelperfunctype.h"
#endif//WIN32
#include "AEEAnnunciator.h"
//#include "AEEPhone.h"//add by lzhl20040823
#include "SoundMenu.h"
#include "OEMText.h"
#include "OEMFeatures.h"
#include "OEMCFGI.h"
#include "AppComFunc.h"
#include "Appscommon.h"
#include "AEERUIM.h"
#include "OEMText.h"
#ifdef FEATRUE_SUPPORT_G_SENSOR
#include "G_Sensor.h"
#include "Oemnvint.h"
#include "OEMCFGI.h"
#endif

#include "fmRadio.h"

/*==============================================================================
                                 类型定义
==============================================================================*/
#define SPORT_EFS_FILE  "fs:/sys/sports.dat" 
#define  AEE_APPSSPORTSMENU_RES_FILE AEE_RES_LANGDIR SPORTSMENU_RES_FILE
/*define  Walk Animation Resource*/
#define  AEE_APPSSPORT_WALK_RES_FILE AEE_RES_LANGDIR WALK_RES_FILE
/*define  Run Animation Resource*/
#define  AEE_APPSSPORT_RUN_RES_FILE AEE_RES_LANGDIR RUN_RES_FILE
/*define Pedometer  Resource*/
//#define  AEE_APPSSPORT_PEDOMTER_RES_FILE AEE_RES_LANGDIR PEDOMETER_RES_FILE
#define SPORTS_ERR(format, code1, code2, code3) 
#define IDC_PEDOMETER_STATE 1

#define CONTROL_RECT_START_X    (40) 

/*ARABIC*/
#define ARABIC_CONTROL_RECT_START_X   (2)
#define CONTROL_RECT_START_DX        (pMe->m_rc.dx - CONTROL_RECT_START_X - ARABIC_CONTROL_RECT_START_X) //86
#define AUTO_POWER_IMG_DX            (8)

/*DEFINE AVAIBLE*/
#define RUNNING_UPDATE_MSECS  (25)
#define MSECS_PER_DAY         (3600 * 24 * 1000)

// 画界面底部提示条宏定义
#define SPORTS_MENU_DRAW_BOTTOMBAR(x)                           \
{                                                   \
    BottomBar_Param_type BarParam;                  \
    MEMSET(&BarParam, 0, sizeof(BarParam));         \
    BarParam.eBBarType = x;                         \
    DrawBottomBar(pMe->m_pDisplay, &BarParam);      \
}
/*Macro Calorie */
#define SPORT_WALK_CALORIE (160)
#define SPORT_LOW_STEP_CALORIE (200)
#define SPORT_MID_STEP_CALORIE (320)
#define SPORT_QUICK_STEP_CALOIRE (420)
#define SPORT_LOW_RUN_CALORIE (750)
#define SPORT_QUICK_RUN_CALORIE (1000)
/*Macro End Calorie*/
/*Gender Modulus*/
#define SPORT_GENDER_MODULUS_MALE (1)
#define SPORT_GENDER_MODULUS_FEMALE (0.85)
/*Gender end Modulus*/
//add by ydc
#define NEVT_KEY_PRESSS 10000


/*Macro Type Var*/
#define SPORT_EDIT_MAX_LEN (20)
#define SPORT_PED_MAX_TIME_LEN (30)
#define SPORT_USER_NUM (3)
#define SPORT_DISPLAY_ANIMATION_NUM (6)
/*End Type Var*/
/*----------------------对话框相关数据类型声明---------------------*/
// 对话框关闭时返回值列表
typedef enum DLGRetValue
{
    DLGRET_CREATE,
    DLGRET_OK,
    DLGRET_SPORTS,				//IDD_DLG_SPORT
    DLGRET_PEDOMETER,
    DLGRET_PEDOMETER_OPTION,
    DLGRET_PEDOMETER_RECORD,
    DLGRET_PEDOMETER_RECORD_VIEW,
    DLGRET_PERSONAL,
    DLGRET_PERSONAL_OPTION,
    DLGRET_PERSONAL_EDIT,
    DLGRET_EDIT,                           //
    DLGRET_INPUT,                         // IDD_DIALOG_INPUT
    DLGRET_MSGBOX,                  // IDD_DIALOG_MSGBOX
    DLGRET_APP_BGROND,         //	IDD_DIALOG_AP_BGROND
    DLGRET_CANCELED

} DLGRet_Value_e_Type;

/*----------------------状态机相关数据类型声明---------------------*/
// SettingMenu Applet 状态机状态：
typedef enum _SportsMenuState
{
   SPORT_STATE_NONE,
   SPORT_STATE_INIT,
   SPORT_STATE_MAIN,
   SPORT_STATE_PETOMETER,   
   SPORT_STATE_PETOMETER_OPTION, // Pedometer Option
   SPORT_STATE_PEDOMETER_RECORD,  //Pedometer Record 
   SPORT_STATE_PEDOMETER_RECORD_VIEW,//Pedometer Record View
   SPORT_STATE_PERSONAL,// Personal 
   SPORT_STATE_PERSONAL_OPTION,// Personal option
   SPORT_STATE_PERSONAL_EDIT,
   SPORT_STATE_INPUTED,      //Input 
   SPORT_STATE_MSGBOX,      //msgbox
   SPORT_STATE_APP_BGROUND,  //bgrond run
   SPORT_STATE_EXIT
} SportsMenuState;

// Personal Unit Data Flag 状态机状态：

typedef enum _SportUnitData
{
	UNIT_DATA_NAME,
	UNIT_DATA_HEIGHT,
	UNIT_DATA_WEIGHT,
	UNIT_DATA_WALK_STEP,
	UNIT_DATA_RUN_STEP,
	UINT_DATA_BRITHDAY
}SportUnitData;
// Pedometer Applet 状态机状态：
typedef enum _PedometerState
{
	PM_IDLE,//START_BACK
	PM_RUNNING,//PAUSE_BACK
	PM_STOPED,//RESUME_BACK
	PM_REST_CONFIRM
}PedometerState;

typedef enum _PedEntryDialogState
{
	/*entry Pedometer Dialog*/
	ENTRY_PED,
	/*entry Pedometer Record Dialog*/
	ENTRY_REC,
	/*entry_Pedometer Personal Dialog*/	
	ENTRY_PERS,
	/*entry_Pedometer Personal Dialog*/
	ENTRY_NONE
	
}PedEntryDialogState;

typedef enum _PedEntryRecordState
{
	/*entry Last Record Flag*/
	REC_LAST,
	/*entry accumulate Record Flag*/
	REC_ACC,
	REC_REST,
	REC_NULL
}PedEntryRecordState;

typedef enum _ProFileUser
{
	USER_ONE = 0,//User one
	USER_TWO,//User Two
	USER_THREE, //User Three
	USER_NONE//temp Profile
}ProFileUser;

typedef enum _BWalkOrRun
{
	STATE_WALK,
	STATE_RUN
}BWalkOrRun;

typedef enum _InputMode
{
    LOCAL_NUMBER_INPUT,  //在当前位置编辑
    OPT_TEXT_INPUT,  //弹出另外的text控件编辑
    EDIT_MENU_MODE,  //菜单模式
    EDIT_GROUP  
}Input_mode;

// 状态处理函数返回给状态处理主函数的值类型
typedef enum _NextFSMAction
{
   NFSMACTION_WAIT,
   NFSMACTION_CONTINUE
} NextFSMAction;

// 保存模块信息和模块引用计数的结构
typedef struct _SportsMenuMod
{
    DECLARE_VTBL(IModule)
    uint32 m_nRefs;
} SportsMenuMod;

typedef enum _SportsOptionMsgType
{
    SPORT_SIG_RUN,
    SPORT_RUN_WITH_MUSIC,
    SPORT_RUN_WITH_FM
    
} SportsOptionMsgType;

// SportApp Personal Uint Data对象结构体：
typedef struct _CSportSAppUintData
{
	AECHAR  m_pName[SPORT_EDIT_MAX_LEN];
	uint16 m_nHeight;
	uint16 m_nWeight;
	uint16 m_nWalkstep;
	uint16 m_nRunstep;
	uint16 m_nBrithday;
	boolean m_bGender;
}CSportSAppUintData;

// SportApp Pedometer Data对象结构体：
typedef struct _CSportSPedoMeterData
{
	AECHAR m_pPlan[SPORT_EDIT_MAX_LEN];
	uint32 m_nStep;
	AECHAR m_nDistance[10];
	AECHAR m_nSpeed[5];
	uint32 m_nSportendtime;
	uint32 m_nTicks;
	uint32 m_nNumDays;
	AECHAR m_dwDispTime[SPORT_PED_MAX_TIME_LEN];
	AECHAR m_nCalorie[10];
	boolean m_suspending;
	boolean m_bPedRunOrWalkState;
	
}CSportSPedoMeterData;

// SportApp Data对象结构体：
typedef struct _CSportSAppData
{
	CSportSPedoMeterData m_nSportMeterData;//Last Pedometer Record
	CSportSAppUintData m_nSportUintData;//uint Message  Only  Three Name
	CSportSPedoMeterData m_nAccRecord; // Accumulate Record
	
}CSportSAppData;



// SportAppMenu Data对象结构体：
typedef struct _CSportsAppMenu
{
	AECHAR	*m_pUnitDataInputBuf;
	boolean m_bTempEntryView;
	uint16 m_nCtlID; //Control ID
	uint16 m_wMsgResID;
	uint8 animatedIndex; //Animation Image  Index 
	IFileMgr *m_pFileMgr;       // IFileMgr interface
	IStatic *pStatic;
	IMenuCtl* m_pSAppMenu;
	ITimeCtl* m_pSAppTime;
	IImage *imageIconAnimated[SPORT_DISPLAY_ANIMATION_NUM];
	PedEntryRecordState m_nEntryRecordState;
	ProFileUser 	m_nSelProFile;
	ProFileUser     m_nTempSelProFile; //temp Select 
	PedEntryDialogState m_nEntryDialogState;
	PedometerState m_nPedometerState;
	CSportSAppData m_nSportSAppData[SPORT_USER_NUM]; //Three User Data
	SportUnitData m_nUnitDataFlag;
	uint16 m_nUnitDataEditFlag;
        uint16              m_nUnitDataEditPrivFlag;
	SportsMenuState m_PopMsgBoxBackFlag;
	Input_mode     m_nInputMode;
        SportsOptionMsgType m_nMsgTypeFlag;
}CSportsAppMenu;

// SettingMenu Applet对象结构体：
typedef struct _CSportsMenu
{
    DECLARE_VTBL(ISportsMenu)
    uint32           m_nRefs;            // 对象引用计数器
    IModule          *m_pModule;
    IDisplay         *m_pDisplay;
    IShell           *m_pShell;
    AEERect          m_rc;               // 当前活动对话框IDialog接口指针
    IDialog          *m_pActiveDlg;      // 当前活动对话框ID
    uint16           m_pActiveDlgID;     // 活动对话框关闭时的返回结果
    DLGRet_Value_e_Type   m_eDlgRet;     // 是否不改写对话框返回结果，一般情况需要改写(FALSE)。
    boolean          m_bNotOverwriteDlgRet;// Applet 前一状态
    SportsMenuState m_ePreState;        // Applet 当前状态
    SportsMenuState m_eCurState;        // Applet是否处于挂起状态
    boolean          m_bSuspending;      // Applet是否准备好可以处理按键和命令事件。改变量主要用于快速按键引起的问题
    boolean          m_bAppIsReady;
    IAnnunciator    *m_pAnn;
    IConfig         *m_pConfig; 
	uint16           m_currDlgId;       //保存当前所选菜单位置
    uint16           m_nResID;          //补充业务标题ID记录
    uint16           m_nSubDlgId;		    //次级菜单位置
    uint16           m_sSubDlgId;       //三级菜单位置
     IAnnunciator  *m_pIAnn;
	CSportsAppMenu m_pSportsAppMenu;
	boolean runOnBackgrounds;
	boolean GobackIdleFlag;
    boolean FromScheduleFlag;
	
} CSportsMenu;


/*==============================================================================
                                 宏定义和常数
==============================================================================*/
// 为防止用户快速按键而添加事件EVT_APPISREADY，事件由定时器
// SettingMenu_APPIsReadyTimer发出
#define APPISREADY_TIMER       200
#define EVT_APPISREADY   (EVT_USER+1)           

// 状态移动宏：先将前一状态用当前状态更新，再将当前状态设为nextState
#define MOVE_TO_STATE(nextState) {            \
            SportsMenuState tpState;            \
            tpState = nextState;          \
            pMe->m_ePreState = pMe->m_eCurState; \
            pMe->m_eCurState = tpState;        \
        }

#define SPORTS_MENU_SOFK_HIGHT 0
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
       SportSApp_ShowDialog
说明:
       函数由状态处理函数用来创建相应对话框资源ID的对话框。

参数:
       pMe [In]: 指向SportSApp Applet对象结构的指针,该结构包含小程序的特定信息.
       dlgResId [in]：对话框资源ID，函数根据该ID创建对话框。

返回值:
       无

备注:


==============================================================================*/
void SportSApp_ShowDialog(CSportsMenu *pMe,uint16  dlgResId);

/*==============================================================================
函数:
    SportSApp_ShowMsgBox

说明:
    函数由状态处理函数用来唤起消息对话框，显示消息提示用户。

参数:
    pMe [in]: 指向 Contact Applet 对象结构的指针。该结构包含小程序的特定信息。
    wTextResId [in]: 消息文本资源 ID。

返回值:
    none

备注:

==============================================================================*/
extern int SportSApp_ShowMsgBox(CSportsMenu *pMe, uint16 wTextResId);

/*==============================================================================
函数:
       SportSApp_RouteDialogEvent
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
boolean SportSApp_RouteDialogEvent(CSportsMenu *pMe,AEEEvent eCode,uint16 wParam,uint32 dwParam);

/*----------------------状态机相关函数声明---------------------*/
/*==============================================================================
函数:
       SportSApp_ProcessState
说明:
       SportSApp Applet状态处理主函数。函数根据pMe->m_currState将调用路由至
       相应的处理函数。

参数:
       pMe [In]: 指向SportSApp Applet对象结构的指针,该结构包含小程序的特定信息.

返回值:
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注:

==============================================================================*/
NextFSMAction SportSApp_ProcessState(CSportsMenu *pMe);
#endif
//SETTINGMENUPRIV_H


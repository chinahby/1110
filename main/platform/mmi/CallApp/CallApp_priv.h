#ifndef CALLAPP_PRIV_H
#define CALLAPP_PRIV_H

/*==============================================================================
// 文件：
//        CallApp_priv.h
//        2007-11-01 Gemsea草拟版本(Draft Version)
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

#if defined( FEATURE_CALL_RECORDER)
#include "recorder.h"
#include "recorder.brh"
#include "recorder_images.brh"
#include "appscommonimages.brh"
#include "appscommon.h"
#define  APP_RECORDER_RES_FILE ("fs:/mod/recorder/" AEE_RES_LANGDIR RECORDER_RES_FILE)
#define  APP_RECORDER_IMAGES_RES_FILE ("fs:/mod/recorder/" RECORDER_IMAGES_RES_FILE)
#endif


#ifdef FEATURE_ICM
#include "AEECM.h"
#else
#include "AEETelephone.h"
#include "AEETelDef.h"
#endif
#include "CallApp.brh"
#include "AEEShell.h"
#include "AEEModTable.h"
#include "AEEStdLib.h"
#include "AEEModGen.h"
#include "AEEAppGen.h"
#ifdef CUST_EDITION
#include "OEMClassIDs.h"
#include "OEMFeatures.h"
#endif
#include "CallApp.h"
#include "AEE.h"
#include "AEEHeap.h"
#include "AEEText.h"
#include "AEETime.h"
#include "AEEDate.h"
#include "AEEMimeTypes.h"
//#ifdef FEATURE_LANG_CHINESE
//#include "aeeext_res.h"
//#endif /* FEATURE_LANG_CHINESE */
#ifdef FEATURE_REFUIALARM
#include "AEEAlarm.h"
#endif
#include "AEEAlert.h"
#ifdef FEATURE_USES_ZI
#include "OEMZIText.h"
#else
#include "OEMText.h"
#endif
//#include "OEMRUIM.h"
#include "OEMDisp.h"       // TBD - hack until available somewhere else
#include "OEMSound.h"      // TBD - hack until available somewhere else
#include "AEEConfig.h"
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

#include "Appscommon.h"
#include "AppComFunc.h"
#include "AEECallHistory.h"
#include "AEECallHistoryExt.h"
#include "AEEAnnunciator.h"
#include "OEMCFGI.h"
// 图片资源文件
#include "appscommonimages.brh"
#include "uixsnd.h"

#define FEATURE_DIALER_ANIMAION_SUPPORT	0


#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
//#include "AEEVirtualkey.h"//wlh for vkey number
#define CALLAPP_PT_IN_RECT(a,b,rct)      (boolean)( ((a) >= (rct).x && (a) <= ((rct).x + (rct).dx)) && ((b) >= (rct).y && (b) <= ((rct).y + (rct).dy)) )
#endif
#if defined(FEATURE_VERSION_C306)||defined(FEATURE_VERSION_W0216A)
#define AVK_SEND_TWO     2
#elif defined(FEAUTRE_VERSION_N450)|| defined(FEATURE_VERSION_N021)|| defined(FEATURE_VERSION_C01)|| defined(FEATURE_VERSION_W516)|| defined(FEATURE_VERSION_W027)
#define AVK_SEND_TWO     2
#else
#define AVK_SEND_TWO     1
#endif
#define  AEE_APPSCALLAPP_RES_FILE AEE_RES_LANGDIR CALLAPP_RES_FILE
#define MAX_SIZE_GETIMSI                    20
#define MAX_SIZE_BANNER_TEXT           40
#define MAX_SIZE_DIALER_TEXT            128
#define MAX_SIZE_NAME_TEXT              30
#ifdef FEATURE_ICM
#define MAX_SIZE_DIAL_STR                  AEECM_MAX_DIAL_STRING +1//33
#else
#define MAX_SIZE_DIAL_STR                  83//33  modify by miaoxiaoming
#endif
#define TIMEOUT_MS_ONE_SECOND        1000
//#define DIALER_TEXT_LEN_MAX      32
#define MAX_COUNT_TO_CHANGE           5
#define DIALERAPP_MAX_NUM_CALLS     5
#define CALL_PICTURE_NAME_LENTH      64
//#define DIALOG_SOFTKEY_HIGH        (pMe->m_LineHeight + 2)
#define DIALERAPP_NUM_CALLHISTORY_FIELDS 5
#ifdef FEATURE_VERSION_K212
#define CALL_ANIMATION_WIDTH           88
#else
#define CALL_ANIMATION_WIDTH           36
#endif
#define CALL_ANNU_HEIGHT                   5//16
#define CALL_SOFT_KEY_HIGHT              16//15//  (pMe->m_LargeLineHeight)
//#define CALL_TITLE_BAR_HEIGHT   14 

#define SOFT_KEY_MENU_X                    0
#define SOFT_KEY_MENU_Y                    CALL_ANNU_HEIGHT
#define SOFT_KEY_MENU_DX                  (pMe->m_rc.dx)
#define SOFT_KEY_MENU_DY                  (pMe->m_rc.dy - CALL_ANNU_HEIGHT - CALL_SOFT_KEY_HIGHT)

#define CALL_SCREEN_X                         0
#define CALL_SCREEN_Y                         0
#define CALL_SCREEN_DX                       (pMe->m_rc.dx)
#define CALL_SCREEN_DY                       (pMe->m_rc.dy - CALL_SOFT_KEY_HIGHT)

#if FEATURE_DIALER_ANIMAION_SUPPORT
/*两边各空2个像素*/
#define CALL_TEXT_DX                           (pMe->m_rc.dx- 2*CALL_TEXT_X)
#define CALL_NAME_DX                          (pMe->m_rc.dx- 2*CALL_TEXT_X -CALL_ANIMATION_WIDTH)
#define CALL_NUM_DX                            (pMe->m_rc.dx- 2*CALL_TEXT_X -CALL_ANIMATION_WIDTH)
/*两边各空2个像素*/
#if defined(FEATURE_VERSION_C337)   
#define CALL_TEXT_X                             (2)
#else
#define CALL_TEXT_X                             (3)
#endif
#define CALL_NAME_X                            (2*CALL_TEXT_X + CALL_ANIMATION_WIDTH)
#define CALL_NUM_X                              (2*CALL_TEXT_X + CALL_ANIMATION_WIDTH)
#else
/*两边各空2个像素*/
#define CALL_TEXT_DX                           (pMe->m_rc.dx- 4*CALL_TEXT_X)
#define CALL_NAME_DX                          (pMe->m_rc.dx- 4*CALL_TEXT_X -CALL_ANIMATION_WIDTH)
#define CALL_NUM_DX                            (pMe->m_rc.dx- 4*CALL_TEXT_X -CALL_ANIMATION_WIDTH)
/*两边各空2个像素*/

#if defined(FEATURE_VERSION_C337)   
#define CALL_TEXT_X                             (2)
#else
#define CALL_TEXT_X                             (3)
#endif
#if defined(FEATURE_VERSION_EC99) || defined (FEATURE_VERSION_K212_20D)
#define CALL_NAME_X                            (10*CALL_TEXT_X + CALL_ANIMATION_WIDTH)
#define CALL_NUM_X                              (10*CALL_TEXT_X + CALL_ANIMATION_WIDTH)
#else
#define CALL_NAME_X                            (4*CALL_TEXT_X + CALL_ANIMATION_WIDTH)
#define CALL_NUM_X                              (4*CALL_TEXT_X + CALL_ANIMATION_WIDTH)
#endif
#endif
//#define CALL_LINE_HIGHT           (pMe->m_LineHeight + 3)

/*来电去电动画图片高度*/
#define CALL_ANIM_IMG_HEIGHT              (60)

/*上下各空3个像素*/
//#define CALL_LINE_HIGHT             ((int)((pMe->m_rc.dy - CALL_ANNU_HEIGHT - CALL_SOFT_KEY_HIGHT)/4))
/*上下各空3个像素*/
#ifdef FEATURE_VERSION_X3
#define CALL_LINE_HIGHT                      (25) 
#else
#ifdef FEATURE_VERSION_C310
#define CALL_LINE_HIGHT                      (22) //18为NORMAL字体高度，目前只需要这个高度就够了
#else
#ifdef FEATURE_VERSION_K212
#define CALL_LINE_HIGHT                      (38)
#else
#if defined(FEATURE_VERSION_EC99) || defined (FEATURE_VERSION_K212_20D)
#define CALL_LINE_HIGHT                      (25)
#else
#define CALL_LINE_HIGHT                      (20) //18为NORMAL字体高度，目前只需要这个高度就够了
#endif
#endif
#endif
#endif 
#define CALL_FIRST_LINE_Y                   (CALL_ANNU_HEIGHT)

#if defined(FEATURE_VERSION_C337) 
#define CALL_SECOND_UP_LINE_Y               (CALL_FIRST_LINE_Y +    CALL_LINE_HIGHT)
#define CALL_SECOND_LINE_Y               (CALL_FIRST_LINE_Y +    2*CALL_LINE_HIGHT)
#define CALL_THIRD_LINE_Y                  (CALL_FIRST_LINE_Y +3*CALL_LINE_HIGHT )
#define CALL_FOURTH_LINE_Y               (CALL_FIRST_LINE_Y +4*CALL_LINE_HIGHT )
#elif defined(FEATURE_VERSION_K212) 
#define CALL_SECOND_UP_LINE_Y               (CALL_FIRST_LINE_Y +    CALL_LINE_HIGHT)
#define CALL_SECOND_LINE_Y               (CALL_FIRST_LINE_Y +    2*CALL_LINE_HIGHT)
#define CALL_THIRD_LINE_Y                  (CALL_FIRST_LINE_Y +3*CALL_LINE_HIGHT )
#define CALL_FOURTH_LINE_Y               (CALL_FIRST_LINE_Y +4*CALL_LINE_HIGHT )
#else
#define CALL_SECOND_LINE_Y               (CALL_FIRST_LINE_Y +    CALL_LINE_HIGHT)
#define CALL_THIRD_LINE_Y                  (CALL_FIRST_LINE_Y +2*CALL_LINE_HIGHT )
#define CALL_FOURTH_LINE_Y               (CALL_FIRST_LINE_Y +3*CALL_LINE_HIGHT )
#endif

/*来电去电动画图片Y坐标*/
#define CALL_ANIM_IMG_Y                  (CALL_FIRST_LINE_Y + 2*CALL_LINE_HIGHT+CALL_LINE_HIGHT/2)

/*拨号大数字图片行距*/
#define BETWEEN_LINE_PIXEL                (6)

#define MISSED_TEXT_Y                         CALL_ANNU_HEIGHT
#define MISSED_CALL_Y                         (CALL_ANNU_HEIGHT + pMe->m_LineHeight+2)
/*Temp add */
#define FEATURE_APP_PAUSE_TIMER

#define PARAM_SPEED_DIAL				 999		//Add By zzg 2012_10_31


#if 0
#define FEATURE_IMAGE_DIALING_DIGITS
#endif
#ifdef FEATURE_IMAGE_DIALING_DIGITS
#if defined(FEATURE_DISP_176X220)
    #define NUM_IMAGE_HIGHT               (30)
    #define NUM_IMAGE_WIDTH               (20)
    #define REFUI_CALL_MAX_IMAGSIZE       (17)
#elif defined(FEATURE_DISP_128X128)
	#define NUM_IMAGE_HIGHT               (25)
    #define NUM_IMAGE_WIDTH               (20)
    #define REFUI_CALL_MAX_IMAGSIZE       (17)
#elif defined(FEATURE_DISP_160X128)
#ifdef FEATURE_VERSION_K212_ND
	#define NUM_IMAGE_HIGHT               (33)
    #define NUM_IMAGE_WIDTH               (22)
    #define REFUI_CALL_MAX_IMAGSIZE       (17)
#else
	#define NUM_IMAGE_HIGHT               (25)
    #define NUM_IMAGE_WIDTH               (20)
    #define REFUI_CALL_MAX_IMAGSIZE       (17)
#endif
#elif defined(FEATURE_DISP_220X176)
	#define NUM_IMAGE_HIGHT               (25)
    #define NUM_IMAGE_WIDTH               (20)
    #define REFUI_CALL_MAX_IMAGSIZE       (17)	
#elif defined(FEATURE_DISP_128X160)
	#define NUM_IMAGE_HIGHT               (25)
    #define NUM_IMAGE_WIDTH               (20)
    #define REFUI_CALL_MAX_IMAGSIZE       (17)
#elif defined(FEATURE_DISP_240X320)
	#define NUM_IMAGE_HIGHT               (25)
    #define NUM_IMAGE_WIDTH               (20)
    #define REFUI_CALL_MAX_IMAGSIZE       (17)	
#elif defined(FEATURE_DISP_320X240)
	#define NUM_IMAGE_HIGHT               (25)
    #define NUM_IMAGE_WIDTH               (20)
    #define REFUI_CALL_MAX_IMAGSIZE       (17)		
#else
    #define NUM_IMAGE_HIGHT               (25)
    #define NUM_IMAGE_WIDTH               (20)
    #define REFUI_CALL_MAX_IMAGSIZE       (17)
#endif
#endif

#ifdef FEATURE_LCD_TOUCH_ENABLE
#define CALC_ITEM                    15
#define STARTY                       50
#define STARX                        0
#define SPACEX                       0
#define NUMWINDTH                    80
#define NUMHEIGHT                    44
#endif

//#define KEYSND_ZY
//#define FEATURE_UTK2
//#define FEATURE_PERU_VERSION
#define FEATURE_VOICE_PRIVACY_ICON
//#define T_MSM6000
//#define FEATURE_CARRIER_VENEZUELA_MOVILNET
//#define FEATURE_LARGE_DIALING_DIGITS
//#define FEATURE_IS2000_SCC_CODES
/*Temp add*/
#define DIALER_PAUSE_AECHAR              ((AECHAR)'P')
#define DIALER_PLUS_AECHAR               ((AECHAR)'+')
#ifdef FEATURE_APP_PAUSE_TIMER
#define DIALER_TIMER_AECHAR              ((AECHAR)'T')
#endif
#define FEATURE_EDITABLE_NUMBER


#ifdef FEATURE_SOUND_BO
#define NUM_0  "fs:/hsmm/callapp/0.mp3"
#define NUM_1  "fs:/hsmm/callapp/1.mp3"
#define NUM_2  "fs:/hsmm/callapp/2.mp3"
#define NUM_3  "fs:/hsmm/callapp/3.mp3"
#define NUM_4  "fs:/hsmm/callapp/4.mp3"
#define NUM_5  "fs:/hsmm/callapp/5.mp3"
#define NUM_6  "fs:/hsmm/callapp/6.mp3"
#define NUM_7  "fs:/hsmm/callapp/7.mp3"
#define NUM_8  "fs:/hsmm/callapp/8.mp3"
#define NUM_9  "fs:/hsmm/callapp/9.mp3"
#define NUM_P  "fs:/hsmm/callapp/P.mp3"
#define NUM_W  "fs:/hsmm/callapp/W.mp3"
#define NUM_PUND  "fs:/hsmm/callapp/pund.mp3"
#define NUM_STAR  "fs:/hsmm/callapp/star.mp3"
#define NUM_ADD  "fs:/hsmm/callapp/add.mp3"




#define BJTIME  "fs:/hsmm/callapp/bjtime.mp3"
#define AM_TIME  "fs:/hsmm/callapp/am.mp3"
#define PM_TIME  "fs:/hsmm/callapp/pm.mp3"


//HOUR_SOUND_NAME
#define HOUR_0  "fs:/hsmm/callapp/0.mp3"    // 1
#define HOUR_1  "fs:/hsmm/callapp/1.mp3"    // 1
#define HOUR_2  "fs:/hsmm/callapp/2.mp3"    // 2
#define HOUR_3  "fs:/hsmm/callapp/3.mp3"    // 3
#define HOUR_4  "fs:/hsmm/callapp/4.mp3"    // 4
#define HOUR_5  "fs:/hsmm/callapp/5.mp3"    // 5
#define HOUR_6  "fs:/hsmm/callapp/6.mp3"    // 6
#define HOUR_7  "fs:/hsmm/callapp/7.mp3"    // 7
#define HOUR_8  "fs:/hsmm/callapp/8.mp3"    // 8
#define HOUR_9  "fs:/hsmm/callapp/9.mp3"    // 9
#define HOUR_10  "fs:/hsmm/callapp/10.mp3"  //10
#define HOUR_20  "fs:/hsmm/callapp/20.mp3"  //20
#define HOUR_DIAN  "fs:/hsmm/callapp/dian.mp3"  /*点*/


//MINUTE_SOUND_NAME
#define MINUTE_0  "fs:/hsmm/callapp/0.mp3"   // 0
#define MINUTE_1  "fs:/hsmm/callapp/1.mp3"   // 1
#define MINUTE_2  "fs:/hsmm/callapp/2.mp3"   // 2
#define MINUTE_3  "fs:/hsmm/callapp/3.mp3"   // 3
#define MINUTE_4  "fs:/hsmm/callapp/4.mp3"   // 4
#define MINUTE_5  "fs:/hsmm/callapp/5.mp3"   // 5
#define MINUTE_6  "fs:/hsmm/callapp/6.mp3"   // 6
#define MINUTE_7  "fs:/hsmm/callapp/7.mp3"   // 7
#define MINUTE_8  "fs:/hsmm/callapp/8.mp3"   // 8
#define MINUTE_9  "fs:/hsmm/callapp/9.mp3"   // 9
#define MINUTE_10  "fs:/hsmm/callapp/10.mp3"   // 10
#define MINUTE_20  "fs:/hsmm/callapp/20.mp3"   // 20
#define MINUTE_30  "fs:/hsmm/callapp/30.mp3"   // 30
#define MINUTE_40  "fs:/hsmm/callapp/40.mp3"   // 40
#define MINUTE_50  "fs:/hsmm/callapp/50.mp3"   // 50
#define MINUTE_FEN  "fs:/hsmm/callapp/fen.mp3"   /*分*/


typedef enum
{
    TIME_BJ,       
    TIME_AMPM,        
    TIME_HOUR1,       
    TIME_HOUR2,
    TIME_HOUR_DIAN,
    TIME_MINUTE1,
    TIME_MINUTE2,  
    TIME_MINUTE_FEN
} TIME_STATUE_EX;

#endif

/*==============================================================================
                                 类型定义
==============================================================================*/
typedef struct
{
   uint8 nOn;
   uint8 nOff;
} DTMFToneDuration;
#ifndef Temp_remove
//#define OEMEVT_USER_HEADSET 1111
//#define AVK_USER_HEADSET 5555

typedef struct
{
#ifdef Temp_remove
    PhoneTimeState         timeState;
#endif/*Temp_remove*/
    int                    lastSrvState;
    int                    lastRoamState;
    boolean                IsEmgCall;
    boolean                uimLocked;
    boolean                inLPM;
} idle_state_info_type;
#endif/*Temp_remove*/
//音量等级
typedef enum CVolumeLevel
{
    VOLUME_OFF,
    VOLUME_ONE,
    VOLUME_TWO,
    VOLUME_THREE,
    VOLUME_FOUR,
    VOLUME_FIVE,
    VOLUME_SIX,
    VOLUME_SEVEN
} CVolumeLevel;

// 通话界面中显示“音量大小”，要在调节时显示。
typedef enum ERefreshVol
{
    REFRESHVOL_OFF,
    REFRESHVOL_WAIT,
    REFRESHVOL_ON
} ERefreshVol;

/*----------------------对话框相关数据类型声明---------------------*/
// 对话框关闭时返回值列表
typedef enum DLGRetValue
{
   // 初始值，表明需要创建对话框
   DLGRET_CREATE,

   DLGRET_CALL_ATTEMPT_FAILED,     // Returned by several dialogs when
   DLGRET_DIALER_INVALID_EMGNUMBER,// Returned by the Dialer dialog when
                                   // an invalid emergency number is dialed
                                   // while in the 'Emergency Only' dialer
   DLGRET_NUMEDIT,                 /*3*/
   DLGRET_CALLING,          //4          
   DLGRET_INCOMING,                //5
   DLGRET_CONNECT,                 //6
   DLGRET_CALL_END,                //7
   DLGRET_DIALER_RESTRICT,         //8
   DLGRET_CONV_DIAL,               //9
   DLGRET_BACK_TO_IDLE,            // Returned by Incoming Call dialog10
   DLGRET_CONNECT_OR_NUMEDIT,      //11
   DLGRET_PROMPT,					//Add By zzg 2012_10_31
   DLGRET_MSGBOX,
   DLGRET_SEL_IP_NUMBER,
   DLGRET_EMGNUMBER_CALL_ONLY,
   DLGRET_NUM_TOO_LONG,
   DLGRET_PW,
   DLGRET_PASS,
   DLGRET_FAILD,
#ifdef FEATURE_SUPPORT_WAP_APP
#ifdef FEATURE_CARRIER_CHINA_TELCOM
   DLGRET_INVALID_LINK_NUM,
#endif
#endif
   // 对话框关闭时可返回的通用值
   DLGRET_OK,
   DLGRET_CANCELED
} DLGRet_Value_e_Type;

/*----------------------状态机相关数据类型声明---------------------*/
// CallApp Applet 状态机状态：
typedef enum _CallAppState
{
   STATE_NULL,
   STATE_INIT,            /*1*/
   STATE_CALLING_FROM_ANOTHERAPP,      /*2*/
   STATE_NUMBER_FROM_IDLE,/*3 This state is  the first digt from coreApp*/
   STATE_OUT_RESTRICT,    /*4*/
   STATE_CALLING,         /*5*/
   STATE_CONVERSATION,    /*6*/
   STATE_CONV_DIALER,     /*7 DTMF*/
   STATE_ENDCALL,         /*8*/
   STATE_MISSEDCALL,      /*9*/
   STATE_INCOMINGCALL,    /*10 a*/
   STATE_PW,
#ifdef FEATURE_UTK2
   STATE_CALLCONFIRM,     /*11b */
#endif
   STATE_SEL_IP_NUMBER,//12 c
   //STATE_NUMEDIT_OPTION,//12 c
   //STATE_CONNECT_OPTION,//13 d
   //STATE_INCOMING_OPTION,//14 e
#ifdef FEATURE_EDITABLE_RECORD
   STATE_EDIT_REC_NUMBER,
#endif
#ifdef FEATURE_OEMOMH
   STATE_NONOMH,
#endif
   STATE_EXIT             /*13d*/

} CallAppState;

// 状态处理函数返回给状态处理主函数的值类型
typedef enum NextFSMAction
{
   NFSMACTION_WAIT,
   NFSMACTION_CONTINUE
} NextFSMAction;

// 保存模块信息和模块引用计数的结构
typedef struct CallAppMod
{
    DECLARE_VTBL(IModule)
    uint32   m_nRefs;
} CallAppMod;

typedef struct  _Dialer_call_table
{
#ifdef FEATURE_ICM
    AECHAR     call_number[AEECM_MAX_DIGITS_LENGTH];
#else
    AECHAR     call_number[AEET_MAX_DIGITS_LENGTH];
#endif
    AECHAR     call_name[MAX_SIZE_NAME_TEXT];
    uint8      call_id;
    uint32     start_time;
    uint32     end_time;
    //uint16     call_status;
    uint16     call_type;
    callPIType type;
    char       picture_name[AEE_MAX_FILE_NAME];
    AECHAR       ringer[MAX_FILE_NAME];
    boolean    in_phonebook;
    boolean    b_emerg;
    boolean    b_restict;
    struct _Dialer_call_table *next;
}Dialer_call_table;
typedef struct  _Dialer_cont_cache
{
#ifdef FEATURE_ICM
    AECHAR     call_number[AEECM_MAX_DIGITS_LENGTH];
#else
    AECHAR     call_number[AEET_MAX_DIGITS_LENGTH];
#endif
    AECHAR     call_name[MAX_SIZE_NAME_TEXT];
    char       picture_name[AEE_MAX_FILE_NAME];
    AECHAR       ringer[MAX_FILE_NAME];
}Dialer_cont_cache;
typedef struct _cdg_dsp_info
{
    boolean               b_last_rec;
    boolean               m_b_show_cdg;
#ifdef FEATURE_ICM
    AEECMEvent        event;
    AEECMDispType   disp_type;
    AEECMPI             pi;
    AECHAR              alpha[AEECM_MAX_ALPHA_TAG_LENGTH];    
#else
    AEETEvent        event;
    AEETDispType   disp_type;
    AEETPI             pi;
    AECHAR              alpha[AEET_MAX_ALPHA_TAG_LENGTH];  
#endif
	
}
cdg_dsp_info;
enum
{
    RETURN_NONE,
    RETURN_ZERO,
    RETURN_POUND,
    RETURN_STAR,
    RETURN_P,
#ifdef FEATURE_APP_PAUSE_TIMER
    RETURN_T,
#endif
    RETURN_PLUS,
    RETURN_MAX
};
typedef enum _MAKE_CALL_VALUE
{
    CALL_SUCESS = 0x00,
    CALL_FAIL_EMERGENCY_MODE,
    CALL_FAIL_RESTICT,
    CALL_FAIL_INVIDE_NUMBER,
    CALL_FAIL_ANOTHER
}MAKE_CALL_VALUE;
#ifdef FEATURE_SUPPORT_BT_APP
typedef enum _send_bt_audio
{
    SEND_NONE= 0x00,
    SEND_BT_CALL,
    SEND_BT_AUDIO
}send_bt_audio;
#endif
// CallApp Applet对象结构体：
typedef struct _CCallApp
{
    DECLARE_VTBL(ICallApp)
    uint32                  m_nRefs;          // 对象引用计数器
    IModule                *m_pModule;
    IDisplay               *m_pDisplay;
    IShell                 *m_pShell;

    ISound                 *m_pSound;      // Sound interface
    IALERT                 *m_pAlert;      // IAlert interface
    IConfig                *m_pConfig;     // IConfig interface
    #ifdef FEATURE_OEMOMH
    IRUIM                  *m_pIRUIM;
    #endif
#ifdef FEATURE_ICM
    ICM                    *m_pICM;                         // instance of ICM
#else
    ITelephone           *m_pITelephone;                         // instance of ICM
	ICallMgr             *m_pICallMgr;
	IPhoneCtl            *m_pIPhoneCtl;
#endif
    //IAnnunciator         m_pIAnn;
    AEERect                m_rc;
    IDialog                *m_pActiveDlg;
    uint16                 m_pActiveDlgID;
    // 活动对话框关闭时的返回结果
    DLGRet_Value_e_Type    m_eDlgRet;
    // 是否不改写对话框返回结果，一般情况需要改写(FALSE)。
    boolean                m_bNotOverwriteDlgRet;
    // Applet 前一状态
    CallAppState           m_ePreState;
    // Applet 当前状态
    CallAppState           m_eCurState;
    // Applet是否处于挂起状态
    boolean                m_bSuspending;
#ifdef FEATURE_VERSION_C316
    IFileMgr              *m_pFileMgr;     //Add by pyuangui 2013-01-10
#endif
#ifdef FEATURE_SUB_LCD
    IDisplaySub            *m_pDisplaySub;
    AEERect                m_RectSub;
    IBitmap                *m_pWallSubImage;
#endif
#ifndef FEATURE_USES_LOWMEM
    IImage                 *m_pConvImage;
#endif
#ifdef Temp_remove
    IIdle                   *m_pIdle;
#endif/*Temp_remove*/
    //AECHAR                  *m_msg_text;      //Used by message box. Set this text before
    //showing the message box
    // while the Idle applet is suspended.

    int                     m_LineHeight;       // Height of each text line for normal font
    //int                     m_LargeLineHeight;  // Height of each text line for large font


    // Remaining DTMF digits to send after the pause has been acked
    AECHAR                  m_PauseString[MAX_SIZE_DIALER_TEXT];
#ifdef FEATURE_APP_PAUSE_TIMER
    int                     m_PauseTimer;
    AECHAR                  m_TimerString[MAX_SIZE_DIALER_TEXT];
#endif
#ifdef FEATURE_ICM
    AECHAR                  m_RedirNumStr[AEECM_MAX_DIGITS_LENGTH];  // redir_num_rec contents
#else
    AECHAR                  m_RedirNumStr[AEET_MAX_DIGITS_LENGTH];  // redir_num_rec contents
#endif
    //AECHAR                  m_DispInfo[AEECM_MAX_DIGITS_LENGTH+1];
    //for CDG test, CNAP with Forwarding
    // Presentation indicator for m_RedirNumStr
    callPIType              m_RedirectingNumberPI;
    // Extended Display record contents
    //AECHAR                  m_ExtendedDispInfo[AEECM_MAX_DIGITS_LENGTH+1];
    //AECHAR                  m_ExtendedDispInfo2[AEECM_MAX_DIGITS_LENGTH+1];
    //AECHAR                  *m_ScroolStr;
    int                     m_cdg_row;
    // Current number entered in the dialer dialog
    AECHAR                  m_DialString[MAX_SIZE_DIALER_TEXT];//IDLE input number ,can include 'P'  'T'
#ifdef FEATURE_ICM
    AEECMCallState           m_lastCallState;
#else
    AEETCallState           m_lastCallState;
#endif
    boolean                  m_Is3Way;     // TRUE if the current call was a three-way call
    uint32                   m_LastMinuteAlert;//每分钟提示音

    byte                      m_CallVolume; // current volume for active call
    boolean                   m_CallMuted;
    boolean                   m_userCanceled;   // TRUE if the user pressed END while in the
    uint32                    m_MissedCallCount;
#ifdef FEATURE_ICM
    AEECMEndStatus            m_callEndStatus;
#else
    AEETEndStatus            m_callEndStatus;
#endif
    boolean                   m_callEndInOrig; // TRUE if the last call was ended
    boolean                   m_makeCallAfterOTAPA; // TRUE if CallApp_MakeCall() should be
    // invoked after the OTAPA call is torn down

    // Dynamically allocated array of tones, used to give audible feedback
    // to the user when a hard pause is released.
    AEESoundToneData          *m_toneData;
    int                       m_nToneCount;
    int                       m_nToneCurrIndex;

#ifdef FEATURE_IS2000_SCC_CODES
    // TRUE when the call is on 'Answer Hold'
    boolean                   m_bAnswerHold;
#endif /* FEATURE_IS2000_SCC_CODES */
    // TRUE when the incoming call has been put on 'Answer Hold'

    boolean                   m_bmissCallWaiting;  //呼叫等待时控制是否要写未接号码。TRUE:写未接号码。
    boolean                   m_running;       // Set to TRUE between EVT_APP_START and state init
    IMedia *                  m_pMedia;         // IMedia-based object
    boolean                   m_bCloseAllApplet;
    start_call_type           m_nStartCallType;
    boolean                   incoming_id; //判断来电是否要播放铃声
    //boolean                   IncomIsready;//来电限制速度
    boolean                   IsStartCall;
    boolean                   IsRestrictNumber;
    //音量等级
    CVolumeLevel              m_bVolumeLevel;
    AEEAppStart               *AppStart;
    IBacklight                *m_pBacklight;
#ifdef KEYSND_ZY
    ISoundPlayer               *m_SndPlayer;       //播放多彩按键音接口
#endif /* KEYSND_ZY */

    boolean                    m_bIsPrivacy;
    boolean                    Ispwpass;

#ifdef  FEATURE_PERU_VERSION
    //to save another incoming call in conversation;
    boolean                    in_convert;
#endif

    AECHAR                     wszPrivateString_tw[20];
    ERefreshVol                m_bRefreshVol;      //for display conversation text

    IFont                     *m_pBigNumFont;        //IFont interface for the dialing big number
    int                        m_large_Num_Height;   //Dialing Number's Height,onle used for NumEdit
    IFont                     *m_pNormalNumFont;        //IFont interface for the dialing big number
    int                        m_Normal_Num_Height;   //Dialing Number's Height,onle used for NumEdit
    IFont                     *m_pCurrNumFont;
    int                        m_nCurrNumHeight;
    int                        m_nCurrLineSpace;
    int                        m_nCurrLine;
    int                        m_nCurrLineFits[MAX_SIZE_DIALER_TEXT/8];
    boolean                    m_bShowPopMenu;//indicate current focus is pop_menu
    IMenuCtl                  *m_pMenu;          // pop menu
    boolean                    m_bHandFree;    //设置声音是否从扬声器发出
    byte                       m_Profile;

#ifdef FEATURE_UTK2
    boolean                    m_bEndcurcallForUTKCall;
#endif
    Dialer_call_table          *m_CallsTable;//always point the link_head
    int                        m_CallsTable_Count;//link count
    idle_state_info_type       idle_info;
    boolean                    m_bReceiveCallerID;
    boolean                    m_btime_out;
    byte                       m_return_value;
    uint16                     m_bincoming_rsk;
    //boolean                    m_b_show_cdg;
    int16                      m_b_callend_time;
    boolean                    m_bprocess_held;
    byte                       m_anykey_answer;
    Dialer_call_table          m_call_info;
    boolean                    m_b_incoming;
    IAnnunciator               *m_pIAnn;
    boolean                    m_b_auto_redial;
    uint16                     m_auto_redial_count;
    boolean                    m_b_incall;//is in connect??
    uint16                     m_msg_text_id;
	uint16          		   m_prompt_id; 		//Add By zzg 2012_10_31
    boolean                    m_b_from_numedit;
    boolean                   m_b_draw_dot;
#ifdef FEATURE_IMAGE_DIALING_DIGITS
    IImage                     *num_image[REFUI_CALL_MAX_IMAGSIZE];
#endif
#ifdef FEATRUE_SET_IP_NUMBER
    boolean                    m_b_have_set_ip;
    AECHAR                    m_b_ip_call[OEM_IP_NUMBER_MAX +1];
#endif
#ifdef FEATRUE_AUTO_POWER
    boolean                    m_b_powerdown;
#endif
    keyToneLength              m_dtmf_length;
    callPIType                    m_b_is_PI_ALLOWED;
    boolean                    m_b_miss_notify;
    cdg_dsp_info             m_cdg_dsp_info;
#ifdef FEATURE_KEYGUARD
    boolean                    m_b_set_keyguard;
#endif
    //boolean                    m_b_from_another;
    boolean                    m_b_press_1;
#ifdef FEATURE_SUPPORT_BT_APP
    send_bt_audio          m_bt_audio;
    boolean                    m_b_add_btag_menu;
#endif

//Add By zzg 2011_10_27
#ifdef FEATURE_SUPPORT_BT_AUDIO
	boolean				  m_bBtAvailable;
#endif
//Add End

    AECHAR      *m_pwstrDialStringkeep;


#if defined( FEATURE_CALL_RECORDER)
    Media						m_Media;
    boolean						m_bRecorderOn;
    IImage*						m_pImage[6];
    boolean						m_bSettingVolume;
#endif

#ifdef FEATURE_EDITABLE_NUMBER
    int                           m_nCursorPos;
#endif
#ifdef FEATURE_EDITABLE_RECORD
    boolean                    m_bEditRecNumber;
#endif
    Dialer_cont_cache              m_cont_cache;
#ifdef WIN32//wlh for VKey_Number
	IVkeyCtl * m_pIVkeyCtl;
#endif
    uint16   m_curpros;
    boolean  m_bShift;
    boolean  b_multenter;
    char    *m_strPhonePWD;
#ifdef FEATURE_OEMOMH
    AEECLSID  m_clsOMHApplet;
#endif
#ifdef FEATURE_LCD_TOUCH_ENABLE
	boolean  m_bup;
	int16   m_i;
#endif
	boolean            m_penup;
#if defined(FEATURE_VERSION_C337)||defined(FEATURE_VERSION_K202)||defined(FEATURE_VERSION_K212)    
       boolean                    m_isIncoming;
#endif
#if defined(FEATURE_VERSION_K212)    
	boolean   m_isCallConectBacklight;
#endif
    IStatic *pStatic;           //Add By zzg 2013_08_23
//   boolean            m_short_hit;
} CCallApp;

/*==============================================================================
                                 宏定义和常数
==============================================================================*/
// 为防止用户快速按键而添加事件EVT_APPISREADY，事件由定时器
#define APPISREADY_TIMER        100
#define EVT_APPISREADY          (EVT_USER+10)

//关闭程序事件(启动最近通话后Post给Call)
//#define EVT_USER_CLOSEAPP       (EVT_USER+13)
//来电时限制速度
//#define EVT_INCOMISREADY        (EVT_USER+14)
//#define DIALER_MEM_CHECK
// 画界面底部提示条宏定义
#define REFUI_DRAW_BOTTOMBAR(x)                           \
{                                                   \
    BottomBar_Param_type BarParam;                  \
    MEMSET(&BarParam, 0, sizeof(BarParam));         \
    BarParam.eBBarType = x;                         \
    DrawBottomBar(pMe->m_pDisplay, &BarParam);      \
}
#define CALLAPP_MALLOC(x) CallApp_Malloc((x), __LINE__)
#define CALLAPP_FREE(x) CallApp_Free((x))
// 状态移动宏：先将前一状态用当前状态更新，再将当前状态设为nextState
#define MOVE_TO_STATE(nextState) {            \
            CallAppState tpState;            \
            tpState = nextState;          \
            pMe->m_ePreState = pMe->m_eCurState; \
            pMe->m_eCurState = tpState;        \
        }
#define CLOSE_DIALOG(DlgRet)  {    \
                                     pMe->m_eDlgRet = DlgRet; \
                                     (void) ISHELL_EndDialog(pMe->m_pShell);  \
                              }
#define MAXIMUM_TIMES_OF_REDIAL               5
#define MAX_SPEEDDIAL_CHARS               (1)

// How long a CDMA display event message is shown to the user
#define TIMEOUT_MS_DISPLAYEVENT           5000
#ifdef FEATURE_FDN
// Time until the Call Origination Not Allowed closes automatically
#define TIMEOUT_MS_MO_NOT_ALLOWED_SCREEN  (4*TIMEOUT_MS_ONE_SECOND)
#endif /* FEATURE_FDN */

// Time until the EndCal dialog closes automatically
#define TIMEOUT_MS_ENDCALLTIMER            3000

// Time between CallApp_HandleEndCallBlinkTimer() callbacks
#define TIMEOUT_MS_ENDCALLBLINKTIMER        500

// Time in milliseconds between CallApp_HandleConversationTimer() callbacks
#define TIMEOUT_MS_CONV_TIMER               (TIMEOUT_MS_ONE_SECOND)

// Time in milliseconds that the "Invalid Emergency Number" dialog will be
// displayed
#define TIMEOUT_MS_INVALIDEMGNUMDIALOG_TIMER (2*TIMEOUT_MS_ONE_SECOND)

// Time before the In-band Incoming Call Dialog is automatically closed.
#define TIMEOUT_MS_INBANDCALL_TIMEOUT        (40*TIMEOUT_MS_ONE_SECOND)

// Time (in milliseconds) that each tone in a pause string is played
// to the user through the speaker or earpiece.
//
// Note that this value has nothing to do with the duration of the DTMF
// tones sent across the network.
#define PAUSE_TONE_DURATION                  250

// Which second (in a minute) that the minute reminder should start on.
// Currently set to second 50 (ie, 10 seconds before the next minute)
#define MINUTE_REMINDER_SECOND               50


//#define INCOMINGCALLISREADY_TIMER            500

//#define INCOMING_IMAGE "image\\incomingcall.bmp"

//#define CALL_IMAGE "image\\aboutcall_160.bmp"

//#define WARNING_IMAGE "image\\warning.bmp"

//#ifdef FEATURE_APP_PAUSE_TIMER
#define CALL_TIMER_SET                       3000    //Set Timer of T key	Add By zzg 2010_08_03

#define CALL_TIMER_FRENDUO					 10000	//Set Timer of Frenduo
//#endif
//#define DIALER_DUBEG
#ifndef USES_RELEASE_VERSION
    //#define CALL_PRINT(str,str1,str2)        CallApp_Printf(str,str1,str2)
    #define CALL_ERR( str, a, b, c )         MSG_FATAL( str,a,b,c )
    #define CALL_FUN_START( str, a, b, c )   MSG_FATAL( str,a,b,c )
    #define CALL_PRINTF( str,... )
#else
    //#define CALL_PRINT(str,str1,str2)
    #define CALL_ERR( str, a, b, c )
    #define CALL_FUN_START( str, a, b, c )
    #define CALL_MSG_HIGH( str, a, b, c )
    #define CALL_MSG_LOW( str, a, b, c )
    #define CALL_PRINTF( str,... )
#endif


/*==============================================================================
                                 全局数据
==============================================================================*/
//extern uint8   TMP_RSSI_VALUE;
//extern boolean Tmp_ROAM_Off;
//extern boolean Globe_Media_Start;  //从多媒体播放进入，合上翻盖，不能够挂断电话。
//extern boolean bFlipClosed;    //判断翻盖是否关闭
//extern boolean Camera_quick_in;
/*==============================================================================
                                 本地（静态）数据
==============================================================================*/

/*==============================================================================
                                 函数声明
==============================================================================*/
/*----------------------对话框相关函数声明---------------------*/
//Create dialog according to dialog ID
void CallApp_ShowDialog(CCallApp *pMe,uint16  dlgResId);

//Sets the current sound path
void CallApp_SetupCallAudio(CCallApp *pMe);

#ifdef FEATURE_SUPPORT_BT_AUDIO
void CallApp_SwitchCallAudio(CCallApp *pMe, boolean bIsBtAudio);	//Add By zzg 2011_11_09
#endif

//Handles the details of making a call
MAKE_CALL_VALUE CallApp_MakeCall(CCallApp *pMe);

//Timer callback used by the conversation dialog to update
//the call timer every second.
void CallApp_HandleConversationTimer(void *pUser);

// Start an applet
boolean CallApp_LaunchApplet(CCallApp   *pMe, AEECLSID  classID);
//Play next tone
void CallApp_NextTonePlay(void *pUser);

//Increase or decreases the current call volume
void CallApp_ChangeCallVolume(CCallApp  *pMe,  boolean      louder);
//add by pyuangui
void CallApp_ChangeCallVolume_AVK_RWD(CCallApp  *pMe);


//Answer an incoming or in-band call
boolean CallApp_AnswerCall(CCallApp  *pMe,
                                        boolean bAnswerHold,
                                        AEEEvent eCode,
                                        uint16 wParam,
                                        boolean auto_answer);

//look up name ringer picture from phonebook according number
boolean CallApp_NumberLookup(CCallApp      *pMe,
                             AECHAR        *Number,
                             AECHAR        *Name,
                             uint32         szName,
                             AECHAR        *RingerID,
                             char          *PictureName);

//judge that if the number is restricted
boolean CallApp_IsRestictCallNumber_Ex(CCallApp   *pMe,
                                        AECHAR       *pNumber,
                                        boolean      IsOutgoing);
#ifdef FEATURE_ICM
boolean CallApp_IsEmergencyMode(ICM* pICM);
#else
boolean CallApp_IsEmergencyMode(ITelephone* pITelephone);
#endif

boolean CallApp_IsEmergency_Number(AECHAR *pNumber);

#ifdef KEYSND_ZY
//多彩按键音
void CallApp_SpecialKeySnd(CCallApp *pMe, uint16 wParam);
#endif /* KEYSND_ZY */

boolean CallApp_RouteDialogEvent(CCallApp *pMe,
                                        AEEEvent    eCode,
                                        uint16      wParam,
                                        uint32      dwParam);

NextFSMAction CallApp_ProcessState(CCallApp *pMe);

int CallApp_Compare_Number(AECHAR * Number1, AECHAR * Number2);

Dialer_call_table * CallApp_Search_Number_In_Call_Table(CCallApp *pMe,AECHAR *number);

boolean CallApp_Add_Number_To_Call_Table(CCallApp *pMe,AECHAR *number,
                                        uint8 call_id,
                                        uint16 call_type,
                                        //uint16 call_status,
                                        callPIType type,
                                        boolean b_force_add ,boolean is_energency, boolean is_restict);

boolean CallApp_Modify_Number_To_Call_Table(CCallApp *pMe,AECHAR *number,
                                        uint8 call_id, uint16 call_type/*uint16 call_status*/,callPIType type);

boolean CallApp_Get_CallName_In_Call_Table(CCallApp *pMe,AECHAR *name);

Dialer_call_table* CallApp_Get_First_Entry_In_Call_Table(CCallApp *pMe);

void CallApp_Free_All_Call_Table(CCallApp *pMe);

void CallApp_Change_Call_Table_Call_Start_Time(CCallApp *pMe,AECHAR *number);

void CallApp_Change_Call_Table_Call_End_Time(CCallApp *pMe,AECHAR *number);

void CallApp_Reset_Call_Table_Call_End_Time(CCallApp *pMe,AECHAR *number);

void CallApp_Change_Call_Table_All_Call_End_Time(CCallApp *pMe);

void CallApp_Change_Call_Table_Call_ID(CCallApp *pMe,AECHAR *number,uint8 call_id);

void CallApp_Change_Call_Table_Call_History_State(CCallApp *pMe,
                                        AECHAR *number,
                                        uint16 type/*uint16 status*/);

void CallApp_Add_All_Call_History(CCallApp *pMe);

void CallApp_DrawTextWithProfile(IShell* pShell,
                                        IDisplay * pDisplay,
                                        RGBVAL ProfileColor,
                                        AEEFont Font,
                                        const AECHAR * pcText,
                                        int nChars,
                                        int x,
                                        int y,
                                        const AEERect * prcBackground,
                                        uint32 dwFlags
                                        );


//void CallApp_Init_Msgbox_String(CCallApp *pMe,uint16 str_id,AECHAR * pach_string);
void CallApp_ShowMsgBox(CCallApp *pMe, uint16 str_id);

void CallApp_HandleAutoAnswerTimer(void *pUser);

//Timer callback to flash the call timer.
void CallApp_HandleEndCallBlinkTimer(void *pUser);
void * CallApp_Malloc(uint32 size, int line);

void CallApp_Free(void *p) ;
#ifdef DIALER_MEM_CHECK
void CallApp_show_Mem(CCallApp *pMe);
#endif
//#ifdef DIALER_DUBEG
//void CallApp_Printf(const char * file_name,AECHAR * string1,AECHAR * string2);
//#endif
boolean CallApp_Process_EVT_FLIP_Event(CCallApp *pMe, uint32 wParam);
#ifdef FEATURE_SUPPORT_BT_APP
void bt_ui_process_incall_org_state(void);
boolean bt_ui_process_vol_change(byte vol);
boolean bt_ui_process_cmcall_notify(CCallApp *pMe,uint32 event/*,void  *pCallInfo*/,boolean cm_event);
#endif
void CallAppNotifyMP3PlayerAlertEvent(CCallApp *pMe, boolean toStartAlert);
#endif
//CALLAPP_PRIV_H


#ifndef __FM_RADIO_RIV_H
#define __FM_RADIO_RIV_H

/*==============================================================================
// 文件：
//        fmRadio_priv.h
//        2007-11-11 Gemsea草拟版本(Draft Version)
//        版权所有(c) 2007 Anylook.,LTD.
//
// 文件说明：
//
// 作者：2007-11-11
// 创建日期：Gemsea
// 当前版本：Draft
==============================================================================*/

/*==============================================================================
修改历史记录：
时       间      修 改 人     修改内容、位置及原因
-----------      ----------     -----------------------------------------------
2007-11-11              初始版本
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
   #include "FmRadio.bid"
#else
   #include "OEMClassIds.h"  // Applet Ids
   #ifndef AEECLSID_APP_FMRADIO
      #error AEECLSID_APP_FMRADIO must be defined
   #endif
#endif

#include "fmradiols.brh"
#include "fmradioln.brh"
#include "AEEShell.h"
#include "AEEStdLib.h"
#include "AEEModGen.h"
#include "AEEAppGen.h"
#include "FmRadio.h"
#include "snd.h"
#ifndef WIN32
#include "db.h"
#else
#include "oemhelperfunctype.h"
#endif//WIN32
#include "AEEMenu.h"
#include "AEEConfig.h"
#include "AEEText.h"
#include "OEMCFGI.h"
#include "AEECM.h"
#include "OEMSMS.h"
#include "AEEText.h"
#include "AEEAnnunciator.h"
#include "AEETelephone.h"

#if !defined( AEE_SIMULATOR)
#ifdef FEATURE_ANALOG_TV
#include "fm_framework.h"
#else
#include "fm_radio.h"
#endif
#endif //#if !defined( AEE_SIMULATOR)


#define debug(...)

#define FEATURE_DIRECT_INPUT_CHANNEL_NUMBER                     1
#define FEATURE_TEST_VERSION_WITHOUT_HEADSET_PRESENCE_VERIFY    0
#define FEATURE_FMRADIO_TEST_VERSION
//#define FEATURE_FMRADIO_SIMPLE_VERSION							1
#if defined( FEATURE_Q1_SPECIAL_TARGET) || defined( FEATURE_Q2_SPECIAL_TARGET) || defined( FEATURE_CS09) || defined(FEATURE_FMRADIO_TEST_VERSION)
#define FEATURE_FMRADIO_NO_MODE_SELECT
#define FEATURE_FMRADIO_KEY_OK_TO_MUTE
#define FEATURE_FMRADIO_PROMPT_HEADSET_PLUG_OUT
#endif 

#define FEATURE_FMRADIO_SUPPORT_BACKGROUND  0

#define FEATURE_FMRADIO_CHANNEL_LIST_SUPPORT 1
#define COREAPP_PATH "fs:/mod/coreapp/"
#include "coreapp.brh"
#define WMSAPP_RES_FILE ("fs:/mod/wms/" AEE_RES_LANGDIR "wms.bar")

#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch 
#define FMRADIOAPP_PT_IN_RECT(a,b,rct)      (boolean)( ((a) >= (rct).x && (a) <= ((rct).x + (rct).dx)) && ((b) >= (rct).y && (b) <= ((rct).y + (rct).dy)) )
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

   // 对话框关闭时可返回的通用值
   DLGRET_OK,
   //Add Value here
   //在主状态下选中Option
   DLGRET_OPTION_SELECT,
   //在Option状态下选择List
   DLGRET_OPTION_LIST,

   DLGRET_CANCELED

} DLGRet_Value_e_Type;

/*----------------------状态机相关数据类型声明---------------------*/
// FmRadio Applet 状态机状态：
typedef enum _FmRadioState
{
   FMRADIOST_NONE,
   FMRADIOST_INIT,
   FMRADIOST_MAIN,
   FMRADIOST_OPTION,
   FMRADIOST_LIST,
   FMRADIOST_USER_MSG, 
   //Add State here
   FMRADIOST_EXIT
} FmRadioState;

// 状态处理函数返回给状态处理主函数的值类型
typedef enum NextFSMAction
{
   NFSMACTION_WAIT,
   NFSMACTION_CONTINUE
} NextFSMAction;

// 保存模块信息和模块引用计数的结构
typedef struct FmRadioMod
{
    DECLARE_VTBL(IModule)
    uint32   m_nRefs;
} FmRadioMod;

/*==============================================================================
                          与FM RADIO控制相关的数据和定义
==============================================================================*/
#define LOWEST_BAND_FREQ 87500
#define UPPEST_BAND_FREQ 108000
#if defined FEATURE_FM_SPACE_50KHZ
#define FM_MAX_CHANNEL  410
#define CHANNEL_SPACE 	50
#else
#define FM_MAX_CHANNEL  205
#define CHANNEL_SPACE 	100
#endif 
//定义主对话框下的两种工作模式：播放和调频
typedef enum _OpMode
{
    FM_RADIO_OPMODE_PLAY,
    FM_RADIO_OPMODE_AUTO_TUNE,
#if FEATURE_FMRADIO_CHANNEL_LIST_SUPPORT
    FM_RADIO_OPMODE_REFRESH_CHANNEL_LIST_CONFIRM,
    FM_RADIO_OPMODE_REFRESH_CHANNEL_LIST,
#endif
    FM_RADIO_OPMODE_MODE_SELECTION,
    FM_RADIO_OPMODE_OPTION_SELECTION,
#if FEATURE_FMRADIO_CHANNEL_LIST_SUPPORT
    FM_RADIO_OPMODE_EDIT_CHANNEL_LIST,
    FM_RADIO_OPMODE_EDIT_CHANNEL_LIST_OPTION_SELECTION,
    FM_RADIO_OPMODE_EDIT_CHANNEL,
#endif

    FM_RADIO_OPMODE_USER_MSG,
#if FEATURE_FMRADIO_CHANNEL_LIST_SUPPORT
    FM_RADIO_OPMODE_CHANNEL_LIST_DELETE,
    FM_RADIO_OPMODE_CHANNEL_LIST_DELETE_ALL,
    FM_RADIO_OPMODE_PRMOPT_CHANNEL_LIST_IS_EMPTY,
#endif
   #if FEATURE_DIRECT_INPUT_CHANNEL_NUMBER
   FM_RADIO_OPMODE_DIRECT_INPUT_CHANNEL
   #endif
} OpModeTypeEnum;

typedef enum
{
#if FEATURE_FMRADIO_CHANNEL_LIST_SUPPORT
    FM_RADIO_TUNNING_MODE_LIST = 0,
    FM_RADIO_TUNNING_MODE_MANUAL,
    FM_RADIO_TUNNING_MODE_AUTO,
    FM_RADIO_TUNNING_MODE_LAST,
    FM_RADIO_TUNNING_MODE_NUMBER = FM_RADIO_TUNNING_MODE_LAST
#else
    FM_RADIO_TUNNING_MODE_MANUAL = 0,
    FM_RADIO_TUNNING_MODE_AUTO,
    FM_RADIO_TUNNING_MODE_LAST,
    FM_RADIO_TUNNING_MODE_NUMBER = FM_RADIO_TUNNING_MODE_LAST
#endif
    
} TuningModeEnum;

typedef enum
{
    FM_RADIO_LED_LIGHT_PLAYING = 0,
    FM_RADIO_LED_LIGHT_IDLE,
    FM_RADIO_LED_LIGHT_SEARCHING
} LEDLightTypeEnum;

typedef enum
{
    FM_RADIO_REFUSE_REASON_NOT_REFUSE = -1,
    FM_RADIO_REFUSE_REASON_VOICE_CALL_CONNECTED,
    FM_RADIO_REFUSE_REASON_NO_HEADSET,
    FM_RADIO_REFUSE_REASON_HEADSET_PLUG_OUT
} RefuseReasonEnum;


#if 0
typedef struct _sChanListNode sChanListNode;
struct _sChanListNode
{
    sChanListNode* pPreNode;
    sChanInfo       chanInfo;
    sChanListNode* pNextNode;
};
#endif

typedef struct
{
    TuningModeEnum  tuningMode;
    uint16          channel;
} FmRadioCfg;

typedef enum
{
    FM_OPTION_EDIT_BACK = 0,
    FM_SAVE_BACK,
    FM_SAVE_DELETE
} fm_channellist_bottom_type;

// FmRadio Applet对象结构体：
typedef struct _CFmRadio
{
    DECLARE_VTBL(IFmRadio)

    uint32           m_nRefs;               // 对象引用计数器
    IModule         *m_pModule;

    IDisplay        *m_pDisplay;
    IShell          *m_pShell;
    AEERect          m_rc;
    IConfig         *m_pConfig;
    IAnnunciator        *m_pIAnn;

    IDialog         *m_pActiveDlg;          // 当前活动对话框IDialog接口指针
    uint16           m_pActiveDlgID;        // 当前活动对话框ID
    DLGRet_Value_e_Type   m_eDlgRet;        // 活动对话框关闭时的返回结果
    boolean          m_bNotOverwriteDlgRet; // 是否不改写对话框返回结果，一般情况需要改写(FALSE)。
    FmRadioState     m_ePreState;           // Applet 前一状态
    FmRadioState     m_eCurState;           // Applet 当前状态

    boolean          m_bSuspending;         // Applet是否处于挂起状态
    boolean          m_bAppIsReady;         // Applet是否准备好可以处理按键和命令事件。改变量主要用于快速按键引起的问题

    // change them from global to instanced.
    // @08.01.22
//    boolean bIsFmHardwareInit;
    byte byVolumeLevel;
    byte byChannelMax;

    OpModeTypeEnum opMode;


    sChanInfo		chanInfoList[MAX_FMRADIO_STORED_CHANNEL];
    int				enumCursor;
    int				enumCursorWithLoop;

    LEDLightTypeEnum    ledLightType;
    RefuseReasonEnum    refuseReason;
    boolean             drawRefreshListPrompt;
    boolean             seekChannelClockwise;
    boolean             globalSearching;
#if FEATURE_FMRADIO_SUPPORT_BACKGROUND 
    boolean             runOnBackground;
#endif
    boolean             startFromBackground;
    boolean             channelListIsEmptyHasPrompted;
    boolean             channelListIsEmpty;
    boolean             fmVolumeStop;
    boolean             fmSpeaker;
    int                 demitintColor;
    int                 channelListMenuSelectedItemId;
    int                 channelListMenuSelectedItemIndex;
    int                 currentChannelListNodeIndex;

    AECHAR              originalChannelNumber[16];

    FmRadioCfg          cfg;
//    AEECallback         callback;

    IStatic             *pStatic;
    ITextCtl            *pText;
    IMenuCtl            *m_pMenu;


   fm_channellist_bottom_type   m_channellist_bottom_type;

    #if FEATURE_DIRECT_INPUT_CHANNEL_NUMBER
    AECHAR directInputChannel[8];
    #endif

    boolean alarmNow;
    uint32  wmsEventTimestamp;
    uint16  wmsEventTimer;
    boolean newSmsIncoming;
    uint16  newSmsIncomingWparam;

#if defined( FEATURE_FMRADIO_NO_MODE_SELECT)
    boolean tuneVolumeByLeftRightArrowKey;
#endif
    byte    keyBeepVol;
    boolean key_to_mute;
    AECHAR  EditChannel[8];
    uint16  CurrentItemId;
    boolean edit_chann_interrupt;
    IImage     *m_pVolumeImage;

    boolean bMsgBoxExist;   //Add By zzg 2013_09_03
    boolean bCurrect;       //Add By zzg 2013_09_03
} CFmRadio;

/*==============================================================================
                                 宏定义和常数
==============================================================================*/
// 为防止用户快速按键而添加事件EVT_APPISREADY，事件由定时器
// FmRadio_APPIsReadyTimer发出
#define APPISREADY_TIMER       100           

#if defined(FEATURE_DISP_128X128)
//当前操作字符串显示y偏移
#define FMRADIO_OPERATION_YOFFSET   	14
//频率图像显示区域x偏移
#define FMRADIO_CHANNEL_XOFFSET      	12//30//12
//频率图像显示区域y偏移
#define FMRADIO_CHANNEL_YOFFSET      	46//52
//频率字符显示y偏移
#define FMRADIO_CHANNEL_FREQ_YOFFSET 	10//97
//频率指针高度
#define FMRADIO_CURSOR_DY           	4
//音量显示区域x偏移
#define FMRADIO_VOLUME_XOFFSET        	30//49
//音量显示区域长度
#define FMRADIO_VOLUME_LENGTH         	60//93
//音量显示区域y偏移
#define FMRADIO_VOLUME_YOFFSET        	40//94//113
//音量滑块大小
#define FMRADIO_VOLUME_BLOCK_SIZE   	4//5
//状态指示灯大小
#define FMRADIO_LED_LIGHT_SIZE          12
#elif defined(FEATURE_DISP_160X128)
//当前操作字符串显示y偏移
#define FMRADIO_OPERATION_YOFFSET   	14
//频率图像显示区域x偏移
#define FMRADIO_CHANNEL_XOFFSET      	12//30//12
//频率图像显示区域y偏移
#define FMRADIO_CHANNEL_YOFFSET      	46//52
//频率字符显示y偏移
#define FMRADIO_CHANNEL_FREQ_YOFFSET 	10//97
//频率指针高度
#define FMRADIO_CURSOR_DY           	4
//音量显示区域x偏移
#define FMRADIO_VOLUME_XOFFSET        	30//49
//音量显示区域长度
#define FMRADIO_VOLUME_LENGTH         	60//93
//音量显示区域y偏移
#define FMRADIO_VOLUME_YOFFSET        	40//94//113
//音量滑块大小
#define FMRADIO_VOLUME_BLOCK_SIZE   	4//5
//状态指示灯大小
#define FMRADIO_LED_LIGHT_SIZE          12

#elif defined(FEATURE_DISP_220X176)
//当前操作字符串显示y偏移
#define FMRADIO_OPERATION_YOFFSET   	14
//频率图像显示区域x偏移
#define FMRADIO_CHANNEL_XOFFSET      	12//30//12
//频率图像显示区域y偏移
#define FMRADIO_CHANNEL_YOFFSET      	46//52
//频率字符显示y偏移
#define FMRADIO_CHANNEL_FREQ_YOFFSET 	10//97
//频率指针高度
#define FMRADIO_CURSOR_DY           	4
//音量显示区域x偏移
#define FMRADIO_VOLUME_XOFFSET        	30//49
//音量显示区域长度
#define FMRADIO_VOLUME_LENGTH         	60//93
//音量显示区域y偏移
#define FMRADIO_VOLUME_YOFFSET        	40//94//113
//音量滑块大小
#define FMRADIO_VOLUME_BLOCK_SIZE   	4//5
//状态指示灯大小
#define FMRADIO_LED_LIGHT_SIZE          12


#elif defined(FEATURE_DISP_128X160)
//当前操作字符串显示y偏移
#define FMRADIO_OPERATION_YOFFSET   	14
//频率图像显示区域x偏移
#define FMRADIO_CHANNEL_XOFFSET      	12//30//12
//频率图像显示区域y偏移
#define FMRADIO_CHANNEL_YOFFSET      	46//52
//频率字符显示y偏移
#define FMRADIO_CHANNEL_FREQ_YOFFSET 	10//97
//频率指针高度
#define FMRADIO_CURSOR_DY           	4
//音量显示区域x偏移
#define FMRADIO_VOLUME_XOFFSET        	30//49
//音量显示区域长度
#define FMRADIO_VOLUME_LENGTH         	60//93
//音量显示区域y偏移
#define FMRADIO_VOLUME_YOFFSET        	40//94//113
//音量滑块大小
#define FMRADIO_VOLUME_BLOCK_SIZE   	4//5
//状态指示灯大小
#define FMRADIO_LED_LIGHT_SIZE          12

#elif defined(FEATURE_DISP_176X220)
//当前操作字符串显示y偏移
#define FMRADIO_OPERATION_YOFFSET   	14
//频率图像显示区域x偏移
#define FMRADIO_CHANNEL_XOFFSET      	12//30//12
//频率图像显示区域y偏移
#define FMRADIO_CHANNEL_YOFFSET      	46//52
//频率字符显示y偏移
#define FMRADIO_CHANNEL_FREQ_YOFFSET 	10//97
//频率指针高度
#define FMRADIO_CURSOR_DY           	4
//音量显示区域x偏移
#define FMRADIO_VOLUME_XOFFSET        	30//49
//音量显示区域长度
#define FMRADIO_VOLUME_LENGTH         	60//93
//音量显示区域y偏移
#define FMRADIO_VOLUME_YOFFSET        	40//94//113
//音量滑块大小
#define FMRADIO_VOLUME_BLOCK_SIZE   	4//5
//状态指示灯大小
#define FMRADIO_LED_LIGHT_SIZE          12

#elif defined(FEATURE_DISP_240X320)
//当前操作字符串显示y偏移
#define FMRADIO_OPERATION_YOFFSET   	45
//频率图像显示区域x偏移
#define FMRADIO_CHANNEL_XOFFSET      	12//30//12
//频率图像显示区域y偏移
#define FMRADIO_CHANNEL_YOFFSET      	46//52
//频率字符显示y偏移
#define FMRADIO_CHANNEL_FREQ_YOFFSET 	40//97
//频率指针高度
#define FMRADIO_CURSOR_DY           	4
//音量显示区域x偏移
#define FMRADIO_VOLUME_XOFFSET        	30//49
//音量显示区域长度
#define FMRADIO_VOLUME_LENGTH         	60//93
//音量显示区域y偏移
#define FMRADIO_VOLUME_YOFFSET        	40//94//113
//音量滑块大小
#define FMRADIO_VOLUME_BLOCK_SIZE   	4//5
//状态指示灯大小
#define FMRADIO_LED_LIGHT_SIZE          12

#elif defined(FEATURE_DISP_320X240)
//当前操作字符串显示y偏移
#define FMRADIO_OPERATION_YOFFSET   	28
//频率图像显示区域x偏移
#define FMRADIO_CHANNEL_XOFFSET      	24//30//12
//频率图像显示区域y偏移
#define FMRADIO_CHANNEL_YOFFSET      	18//52
//频率字符显示y偏移
#define FMRADIO_CHANNEL_FREQ_YOFFSET 	18//97
//频率指针高度
#define FMRADIO_CURSOR_DY           	4
//音量显示区域x偏移
#define FMRADIO_VOLUME_XOFFSET        	30//49
//音量显示区域长度
#define FMRADIO_VOLUME_LENGTH         	60//93
//音量显示区域y偏移
#define FMRADIO_VOLUME_YOFFSET        	40//94//113
//音量滑块大小
#define FMRADIO_VOLUME_BLOCK_SIZE   	4//5
//状态指示灯大小
#define FMRADIO_LED_LIGHT_SIZE          12


#else
//当前操作字符串显示y偏移
#define FMRADIO_OPERATION_YOFFSET   	14
//频率图像显示区域x偏移
#define FMRADIO_CHANNEL_XOFFSET      	12//30//12
//频率图像显示区域y偏移
#define FMRADIO_CHANNEL_YOFFSET      	30//46//52
//频率字符显示y偏移
#define FMRADIO_CHANNEL_FREQ_YOFFSET 	50//77//97
//频率指针高度
#define FMRADIO_CURSOR_DY           	4
//音量显示区域x偏移
#define FMRADIO_VOLUME_XOFFSET        	30//49
//音量显示区域长度
#define FMRADIO_VOLUME_LENGTH         	60//93
//音量显示区域y偏移
#define FMRADIO_VOLUME_YOFFSET        	70//94//113
//音量滑块大小
#define FMRADIO_VOLUME_BLOCK_SIZE   	4//5
//状态指示灯大小
#define FMRADIO_LED_LIGHT_SIZE          12
#endif

// 根据 BREW 3 的需要，重定义资源文件宏
#define  FMRADIOLS_RES_FILE_LANG (AEE_RES_LANGDIR FMRADIOLS_RES_FILE)

// 状态移动宏：先将前一状态用当前状态更新，再将当前状态设为nextState
#define MOVE_TO_STATE(nextState) {            \
            FmRadioState tpState;            \
            tpState = nextState;          \
            pMe->m_ePreState = pMe->m_eCurState; \
            pMe->m_eCurState = tpState;        \
        }

/*==============================================================================
                                 全局数据
==============================================================================*/

/*==============================================================================
                                 本地（静态）数据
==============================================================================*/
//extern byte byVolumeLevel;
//
//extern byte iChannelMax;
//
//extern sChanInfo curChanInfo;
//
//extern sChanListNode* pChanListRoot;
//extern sChanListNode* pCurChanNode;
//
//extern OpModeType opMode;
//
//extern boolean bIsNodeChanged;       //是否修改了当前信道的资料
//
//extern boolean bIsListChanged;       //是否修改了列表的资料

/*==============================================================================
                                 函数声明
==============================================================================*/
/*----------------------对话框相关函数声明---------------------*/
/*==============================================================================
函数:
       FmRadio_ShowDialog
说明:
       函数由状态处理函数用来创建相应对话框资源ID的对话框。

参数:
       pMe [In]: 指向FmRadio Applet对象结构的指针,该结构包含小程序的特定信息.
       dlgResId [in]：对话框资源ID，函数根据该ID创建对话框。

返回值:
       无

备注:


==============================================================================*/
void FmRadio_ShowDialog(CFmRadio *pMe,uint16  dlgResId);

/*==============================================================================
函数:
       FmRadio_RouteDialogEvent
说明:
       函数将BREW事件路由给当前活动对话框的事件处理函数。

参数:
       pMe [In]: 指向FmRadio Applet对象结构的指针,该结构包含小程序的特定信息.
       eCode [in]：事件代码。
       wParam [in]：与事件eCode关联的数据。
       dwParam [in]：与事件eCode关联的数据。

返回值:
       TRUE：传入事件被处理。
       FALSE：传入事件没被处理。

备注:

==============================================================================*/
boolean FmRadio_RouteDialogEvent(CFmRadio *pMe,
    AEEEvent    eCode,
    uint16  wParam,
    uint32 dwParam
);

/*----------------------状态机相关函数声明---------------------*/
/*==============================================================================
函数:
       FmRadio_ProcessState
说明:
       FmRadio Applet状态处理主函数。函数根据pMe->m_currState将调用路由至
       相应的处理函数。

参数:
       pMe [In]: 指向FmRadio Applet对象结构的指针,该结构包含小程序的特定信息.

返回值:
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注:

==============================================================================*/
NextFSMAction FmRadio_ProcessState(CFmRadio *pMe);

/*==============================================================================
函数:
       FmRadio_AddNodeToTail
说明:
       将指定的CHANNEL加入Channel List的最后

参数:
       pChInfo [In]: 指向sChanInfo结构的指针.

返回值:
       TRUE: 成功
       FALSE: 失败

备注:

==============================================================================*/
//boolean FmRadio_AddNodeToTail(sChanInfo* pChInfo);

/*==============================================================================
函数:
       FmRadio_AddChanListNode
说明:
       将指定的CHANNEL加入Channel List, 并排列在指定
       Node的后面.

参数:
       ppNode [IO]: 指向sChanListNode结构指针的指针.
       pChInfo [In]: 指向sChanInfo结构的指针.

返回值:
       TRUE: 成功
       FALSE: 失败

备注:

==============================================================================*/
boolean FmRadio_AddChanListNode( sChanInfo* pChInfo);


//Add By zzg 2013_08_26
boolean FmRadio_SequenceChanList(void);
//Add End

/*==============================================================================
函数:
       FmRadio_DeleteChanListNode
说明:
       将指定的CHANNEL从Channel List删除

参数:
       pNode [In]: 指向sChanListNode结构的指针.

返回值:
       TRUE: 成功
       FALSE: 失败

备注:

==============================================================================*/
#if 0
boolean FmRadio_DeleteChanListNode(sChanListNode* pNode);
#endif
/*==============================================================================
函数:
       FmRadio_SeekStatusTimerCB

说明:
       定时器回调函数，用于异步Seek检查状态。

参数:
       pme [in]：void *类型指针，暗指CFmRadio结构指针。

返回值:
       无。

备注:

==============================================================================*/
void FmRadio_SeekStatusTimerCB(void *pme);

/*==============================================================================
函数:
       FmRadio_AutoSearchTimerCB

说明:
       自动搜索全频段，将有效的电台保存到Channel List。

参数:
       pme [in]：void *类型指针，暗指CFmRadio结构指针。

返回值:
       无。

备注:

==============================================================================*/
void FmRadio_AutoSearchTimerCB(void *pme);

/*==============================================================================
函数:
       FmRadio_WarningDisplayTimerCB

说明:
       显示警告消息，等待时间超时后的回调函数。

参数:
       pme [in]：void *类型指针，暗指CFmRadio结构指针。

返回值:
       无。

备注:

==============================================================================*/
void FmRadio_WarningDisplayTimerCB(void *pme);

/*==============================================================================
函数:
       FmRadio_DisplayWarningMessage

说明:
       显示警告消息

参数:
       pme [in]：void *类型指针，暗指CFmRadio结构指针。

返回值:
       无。

备注:

==============================================================================*/
void FmRadio_DisplayWarningMessage(void *pme, AECHAR* pSzMessage);

/*==============================================================================
函数:
       FmRadio_SaveChannelList

说明:
       将频道列表存入NV, 如果当前列表中的频道超过
       MAX_FMRADIO_STORED_CHANNEL，则会被丢弃。

参数:
       pme [in]：void *类型指针，暗指CFmRadio结构指针。

返回值:
       无。

备注:

==============================================================================*/
void FmRadio_SaveChannelList( CFmRadio* pme);

/*==============================================================================
函数:
       FmRadio_FindChanListNode

说明:
       查找是否已经存在该列表
参数:
       pNode [in]：sChanListNode *类型指针。

返回值:
       TRUE:  已存在
       FALSE: 不存在

备注:

==============================================================================*/
boolean FmRadio_FindChanListNode(int channel);


/*==============================================================================
函数:
       closeApp

说明:
       关闭App
参数:
       pMe [in]：

返回值:

备注:

==============================================================================*/

// bw: 20080808 : close app when pull out the headset
void closeApp( void *pme); //bw:

int  FmRadio_ShowMsgBoxDialog( CFmRadio *pMe, 
                               char*       pszResFile,
                               uint16      msgResId,
                               uint16      dlgResId,                               
                               AECHAR     *msg, 
                               int         msglen);
#endif
//__FM_RADIO_RIV_H


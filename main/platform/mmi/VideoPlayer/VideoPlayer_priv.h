#ifndef VIDEOPLAYERPRIV_H
#define VIDEOPLAYERPRIV_H
/*=================================================================================================================
// 文件：
//        VideoPlayer_priv.h
//        2008-10-06 闫丽娜草拟版本(Draft Version)
//        版权所有(c) 2008 Anylook
//        
// 文件说明：
//        
// 作者： 2008-10-06
// 创建日期：闫丽娜
// 当前版本：Draft
=================================================================================================================*/

/*================================================================================================================
修改历史记录：
时       间      修 改 人     修改内容、位置及原因
-----------      ----------     -----------------------------------------------
2008-10-06        闫丽娜         初始版本
=================================================================================================================*/



/*=================================================================================================================
                                 本文件包含的外部文件
=================================================================================================================*/
#ifndef AEE_SIMULATOR
#include "customer.h"   //Handset Feature Definitions
#else
#include "BREWSimFeatures.h"   //Simulator Feature Definitions
#endif

#if !defined(BREW_STATIC_APP)
// Use the .bid file if compiling as dynamic applet
   #include "VideoPlayer.bid"
#else
   #include "OEMClassIds.h"  // Applet Ids
   #ifndef AEECLSID_VIDEOPLAYER
      #error AEECLSID_VIDEOPLAYER must be defined
   #endif
#endif
#include "videoplayer.brh"
#include "videoplayer_images.brh"
#include "AEEShell.h"
#include "AEEModTable.h"
#include "AEEStdLib.h"
#include "AEEModGen.h"
#include "AEEAppGen.h"
#include "OEMClassIDs.h"
#include "VideoPlayer.h"
#include "MediaGallery.h"
#include "AEEMenu.h"
#include "AEEBacklight.h"
#include "OEMText.h"
#include "OEMCFGI.h"
#include "OEMSVC.h"
#ifndef WIN32
#include "oemui.h"
#include "err.h"
#else
#include "oemhelperfunctype.h"
#endif//WIN32
#include "AEEMedia.h" 
#include "AEEMediaFormats.h"
#include "AEEImageCtl.h" 

#include "AEEMediaUtil.h"
#include "OEMFS.h"
#include "OEMNVINT.h"
#include "AEEConfig.h"    
#include "AEEBacklight.h" 
#include "Appscommon.h"

/*=================================================================================================================
                               宏定义和常数
=================================================================================================================*/
#if defined( AEE_SIMULATOR)
    #define  AEE_APPSVIDEOPLAYER_RES_FILE "fs:/mod/videoplayer/en/videoplayer.bar"
#else
#define  AEE_APPSVIDEOPLAYER_RES_FILE (AEE_RES_LANGDIR VIDEOPLAYER_RES_FILE)
#endif 

#define VIDEOPLAYER_IMAGES_RES_FILE "videoplayer_images.bar"

// 文件夹内可放置的最多文件数
#define MAX_NUMBER               1000

//快速按键的时间间隔
#define APPISREADY_TIMER          100
//快速按件事件
#define EVT_APPISREADY   (EVT_USER+30) 

// 对话框自动关闭的时间
#define DIALOGTIMEOUT            3000

// 设置回调的时间
#define SETTIME_LONG             3000
#define SETTIME_MIDDLE          1000
#define SETTIME_SHORT            300

// 定义的字符串数组最大长度
#define  MAX_STR_LEN             64

//图片中名字显示区域的最大像素值
#define  MAX_NAME_LEN            167

// 从GALLERY进入VIDEOPLAYER的标志
#define STARTARGS_GALLERY       ((char)'G')

// 音量每级间的差
#define VP_VOLUME_STEP           20

// 一共音量级数
#define NUM_OF_VOLBAR_ITEM       6

//接收到短消息响铃之前通知video app ,在AEEEvent.h里定义
// 状态移动宏：先将前一状态用当前状态更新，再将当前状态设为nextState
#define MOVE_TO_STATE(nextState) {                  \
            VideoPlayerState tpState;               \
            tpState = nextState;                    \
            pMe->m_ePreState = pMe->m_eCurState;    \
            pMe->m_eCurState = tpState;             \
                                 }

//画界面底部提示条宏定义
#define DRAW_BOTTOMBAR(x)                           \
{                                                   \
    BottomBar_Param_type BarParam;                  \
    MEMSET(&BarParam, 0, sizeof(BarParam));         \
    BarParam.eBBarType = x;                         \
    DrawBottomBar(pMe->m_pDisplay, &BarParam);      \
}
// 关闭对话框
#define CLOSE_DIALOG(DlgRet)  {    \
                                     pMe->m_eDlgRet = DlgRet; \
                                     (void) ISHELL_EndDialog(pMe->m_pShell);  \
                              }


//wlh 20090420 add icon x/y
//开始暂停
#define VIDEOPLAYER_PLAY_X 34
#define VIDEOPLAYER_PLAY_Y 182
#define VIDEOPLAYER_PLAY_W 16
#define VIDEOPLAYER_PLAY_H 16

//开始 PART
#define VIDEOPLAYER_PLAY_PART_X 16
#define VIDEOPLAYER_PLAY_PART_Y 182
#define VIDEOPLAYER_PLAY_PART_W 52
#define VIDEOPLAYER_PLAY_PART_H 16
//暂停 PART
#define VIDEOPLAYER_PAUSE_PART_X 16
#define VIDEOPLAYER_PAUSE_PART_Y 182
#define VIDEOPLAYER_PAUSE_PART_W 52
#define VIDEOPLAYER_PAUSE_PART_H 16
//FR PART
#define VIDEOPLAYER_FR_PART_X 0
#define VIDEOPLAYER_FR_PART_Y 168
#define VIDEOPLAYER_FR_PART_W 176
#define VIDEOPLAYER_FR_PART_H 13

//快进
#define VIDEOPLAYER_FORWARD_X 159
#define VIDEOPLAYER_FORWARD_Y 168
#define VIDEOPLAYER_FORWARD_W 17
#define VIDEOPLAYER_FORWARD_H 13

//后退
#define VIDEOPLAYER_REWIND_X 0
#define VIDEOPLAYER_REWIND_Y 168
#define VIDEOPLAYER_REWIND_W 17
#define VIDEOPLAYER_REWIND_H 13

//全屏
#define VIDEOPLAYER_FULLSCREEN_X 2
#define VIDEOPLAYER_FULLSCREEN_Y 184
#define VIDEOPLAYER_FULLSCREEN_W 11
#define VIDEOPLAYER_FULLSCREEN_H 10

//进度点
#define VIDEOPLAYER_GLIDER_X 19
#define VIDEOPLAYER_GLIDER_Y 169
#define VIDEOPLAYER_GLIDER_W 9
#define VIDEOPLAYER_GLIDER_H 9

//上一个
#define VIDEOPLAYER_PREVIOUS_X 16
#define VIDEOPLAYER_PREVIOUS_Y 184
#define VIDEOPLAYER_PREVIOUS_W 16
#define VIDEOPLAYER_PREVIOUS_H 13

//下一个
#define VIDEOPLAYER_NEXT_X 52
#define VIDEOPLAYER_NEXT_Y 184
#define VIDEOPLAYER_NEXT_W 16
#define VIDEOPLAYER_NEXT_H 13

//进度条
#define VIDEOPLAYER_SCHEDULE_X 19
#define VIDEOPLAYER_SCHEDULE_Y 169
#define VIDEOPLAYER_SCHEDULE_W 138
#define VIDEOPLAYER_SCHEDULE_H 10

//时间
#define VIDEOPLAYER_TIME_X 69
#define VIDEOPLAYER_TIME_Y 181
#define VIDEOPLAYER_TIME_W 47
#define VIDEOPLAYER_TIME_H 16

//音量
#define VIDEOPLAYER_VOLUME_X 129
#define VIDEOPLAYER_VOLUME_Y 182
#define VIDEOPLAYER_VOLUME_W 33
#define VIDEOPLAYER_VOLUME_H 14

//增加音量
#define VIDEOPLAYER_VOLUME_ADD_X 162
#define VIDEOPLAYER_VOLUME_ADD_Y 182
#define VIDEOPLAYER_VOLUME_ADD_W 12
#define VIDEOPLAYER_VOLUME_ADD_H 16

//降低音量
#define VIDEOPLAYER_VOLUME_LOW_X 117
#define VIDEOPLAYER_VOLUME_LOW_Y 182
#define VIDEOPLAYER_VOLUME_LOW_W 12
#define VIDEOPLAYER_VOLUME_LOW_H 16

#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
#define VIDEOPLAYER_PT_IN_RECT(a,b,rct)      (boolean)( ((a) >= (rct).x && (a) <= ((rct).x + (rct).dx)) && ((b) >= (rct).y && (b) <= ((rct).y + (rct).dy)) )
#endif//FEATURE_LCD_TOUCH_ENABLE

typedef enum VPlayerRecttype
{
	TYPE_PLAYER,
	TYPE_DECVOLUME,
	TYPE_ADDVOLUME,
	TYPE_PREVIOUS,
	TYPE_NEXT,
	TYPE_KUAIJIN,
	TYPE_KUAITUI,
}VPlayerRecttype;
/*=================================================================================================================
                             类型定义
=================================================================================================================*/

/*----------------------对话框相关数据类型声明---------------------*/
// 对话框关闭时返回值列表
typedef enum DLGRetValue
{
   
    DLGRET_CREATE,              //初始值，表明需要创建对话框 
    DLGRET_VIDEOPLAYER,         //IDD_VIDEOPLAYER
    //DLGRET_OPTION,              //操作选择,IDD_OPTION
    //DLGRET_DELETE,              //是否删除界面,IDD_DELETE   
    DLGRET_MSGBOX,              //消息弹窗，IDD_MSGBOX
    DLGRET_OK,                  //对话框关闭时可返回的通用值
    DLGRET_CANCELED
}DLGRetValue;

/*----------------------状态机相关数据类型声明---------------------*/
//VideoPlayerState Applet 状态机状态：
typedef enum _VideoPlayerState
{
    STATE_NONE = 0,             //No state
    STATE_INIT,                 //Initial state. 
    STATE_PLAYER,               //IDD_VIDEOPLAYER
    //STATE_OPTION,               //IDD_OPTION
    //STATE_DELETE,
    STATE_MSGBOX,
    STATE_EXIT                  //State where we close the applet.
}VideoPlayerState;

// 状态处理函数返回给状态处理主函数的值类型
typedef enum NextFSMAction
{
    NFSMACTION_WAIT,            //指示因要显示对话框界面给用户，应挂起状态机。  
    NFSMACTION_CONTINUE         //指示后有子状态，状态机不能停止。       
} NextFSMAction;

// 保存模块信息和模块引用计数的结构
typedef struct VideoPlayerMod
{
    DECLARE_VTBL(IModule)
    uint32   m_nRefs;
} VideoPlayerMod;

//音量等级
typedef enum CVolumeLevel 
{  
    VOLUME_OFF,
    VOLUME_ONE,
    VOLUME_TWO,
    VOLUME_THREE,
    VOLUME_FOUR,
    VOLUME_FIVE
} CVolumeLevel;
// VideoPlayer Applet对象结构体：
typedef struct _CVideoPlayer
{
    DECLARE_VTBL(IVideoPlayer)
    uint32           m_nRefs;     // 对象引用计数器
    IModule          *m_pModule;
    IDisplay         *m_pDisplay;
    IShell           *m_pShell;
    IConfig          *m_pConfig;    
    IMenuCtl         *m_pMenuCtl; //IMenuCtl指针
    IMedia           *m_pMedia;//IMedia指针
    IImage           *m_pImage;
    IFileMgr         *m_pIFileMgr;    
    IDatabase        *m_pFileDB;
    
    AEERect          m_rc;    
 
    // 当前活动对话框IDialog接口指针
    IDialog          *m_pActiveDlg;

    // 当前活动对话框ID
    uint16           m_pActiveDlgID;

    // 当前选择菜单ID
    uint16           m_nItemID;

    //字符串资源ID
    uint16           m_nMsgResID;

    // 活动对话框关闭时的返回结果
    DLGRetValue      m_eDlgRet;

    // 是否不改写对话框返回结果，一般情况需要改写(FALSE)。
    boolean          m_bNotOverwriteDlgRet;

    // Applet 前一状态
    VideoPlayerState m_ePreState;

    // Applet 当前状态
    VideoPlayerState m_eCurState;

    // Applet是否处于挂起状态
    boolean          m_bSuspending;
    boolean          m_bEarUp;
    
    // Applet是否准备好可以处理按键和命令事件。改变量主要用于快速按键引起的问题
    boolean          m_bAppIsReady;   

    //media data
    AEEMediaData     m_md;

    //显示在屏幕上的文件名
    AECHAR           m_playname[MAX_STR_LEN];
    //使用在m_playname[m_np]中
    int              m_np;
    //video列表中的纪录数 
    uint32           m_RecordCount;

    //当前播放的文件名,包含路径
    char             *m_FileToPlay;
       
    //用户选择了哪个菜单
    uint16           m_MenuSelected;

    //是否从GALLERY里进入播放器
    boolean          IsGallery;
    
    //是否暂停
    boolean          IsPause;

    //是否在播放状态
    boolean          IsPlay;
    
    //是否全屏
    boolean          IsFullScreen;

    //初始化播放文件的返回信息
    int16           m_InitFailed;
    int16           m_PlayFailed;
    /*是否可以播放下一首(只有从videoplayer进入后可播放下一首，
     此变量又作为从gallery进入与从videoplayer进入的区别标志*/
    boolean         m_IsPlaynext;
              
    //回调函数中判断是否是用户停止播放，防止用户按了停止键回调函数
    //继续播放下一个文件
    boolean          UserStop;
    // 848是否处于繁忙状态
    boolean          Is848Busy;
      
    //音乐总时间
    uint32           bTotalTime;
    
    //正播放时  的时间
    uint32           bCurrentTime;
   
    //暂停时的时间
    uint32           m_bPauseTime;

    //总的音量
    uint16           totalvolume;
    
    //音量等级
    CVolumeLevel     m_bVolumeLevel;
    
    //在某些特殊情况下不允许暂停
    boolean          PauseLock;
           
    //背光
    IBacklight       *Ibacklight;
    //背光timer
    byte             bktimer;
    //键盘音量
    byte             m_KeySound;
    byte             m_CKSound;
    //短信提示方式
    byte             m_SMSRinger;
    
    //APP 启动方式
    //AppletStartMethod  m_eStartMethod;
    //Applet是否处于活动状态
    boolean          m_bActive;    
	VPlayerRecttype m_rtype;////wlh 20090420 add 为了区别播放区域，加音量，减音量的刷新，加了个参数
}CVideoPlayer;

/*=================================================================================================================
                                 全局数据
=================================================================================================================*/

/*=================================================================================================================
                                 本地（静态）数据
=================================================================================================================*/

/*=================================================================================================================
                                 函数声明
=================================================================================================================*/
/*----------------------对话框相关函数声明---------------------*/
/*=================================================================================================================
函数:
       VideoPlayer_ShowDialog
说明:
       函数由状态处理函数用来创建相应对话框资源ID的对话框。

参数:
       pMe [In]: 指向VideoPlayer Applet对象结构的指针,该结构包含小程序的特定信息.
       dlgResId [in]：对话框资源ID，函数根据该ID创建对话框。

返回值:
       无

备注:


=================================================================================================================*/

int VideoPlayer_ShowDialog(CVideoPlayer *pMe,uint16  dlgResId);

/*=================================================================================================================
函数:
    VideoPlayer_ShowMsgBox

说明:
    函数由状态处理函数用来唤起消息对话框，显示消息提示用户。

参数:
    pMe [in]: 指向 WMS Applet 对象结构的指针。该结构包含小程序的特定信息。
    wTextResId [in]: 消息文本资源 ID。

返回值:
    none

备注:
=================================================================================================================*/

int VideoPlayer_ShowMsgBox(CVideoPlayer *pMe, uint16 wTextResId);
                    
/*=================================================================================================================
函数:
       VideoPlayer_RouteDialogEvent
说明:
       函数将BREW事件路由给当前活动对话框的事件处理函数。

参数:
       pMe [In]: 指向VideoPlayer Applet对象结构的指针,该结构包含小程序的特定信息.
       eCode [in]：事件代码。
       wParam [in]：与事件eCode关联的数据。
       dwParam [in]：与事件eCode关联的数据。

返回值:
       TRUE：传入事件被处理。
       FALSE：传入事件没被处理。

备注:

=================================================================================================================*/
boolean VideoPlayer_RouteDialogEvent(CVideoPlayer *pMe,
                                     AEEEvent    eCode,
                                     uint16  wParam,
                                     uint32 dwParam);


/*----------------------状态机相关函数声明---------------------*/
/*=================================================================================================================
函数:
       VideoPlayer_ProcessState
说明:
       VideoPlayer Applet状态处理主函数。函数根据pMe->m_currState将调用路由至
       相应的处理函数。

参数:
       pMe [In]: 指向VideoPlayer Applet对象结构的指针,该结构包含小程序的特定信息.

返回值:
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注:

=================================================================================================================*/
NextFSMAction VideoPlayer_ProcessState(CVideoPlayer *pMe);
/*=================================================================================================================
函数：VideoPlayer_ReleaseVideo

说明：
       停止并释放Video

参数：
       pMe [in]：指向VideoPlayer Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值：
        无

备注：

=================================================================================================================*/
void VideoPlayer_ReleaseVideo(CVideoPlayer *pMe);

/*===================================================================================================================
    视频播放的模式
 ===================================================================================================================*/
boolean VideoPlayer_PlayMod(CVideoPlayer *pMe, uint16 wParam);

/*=================================================================================================================
初始化video
=================================================================================================================*/
void VideoPlayer_InitVideo(CVideoPlayer * pMe);

//防止快速按键事件
void VideoPlayer_APPIsReadyTimer(void *pMe);
extern boolean videoplayer_play_flag;
#endif
//VIDEOPLAYER_H

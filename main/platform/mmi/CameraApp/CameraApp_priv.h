#ifndef CAMERAAPP_PRIV_H
#define CAMERAAPP_PRIV_H
/*==============================================================================
// 文件：CameraApp_priv.h
//        版权所有(c) 2009 Anylook Tech. CO.,LTD.
//        
// 文件说明：
//        
==============================================================================*/

/*==============================================================================
修改历史记录：
时       间      修 改 人     修改内容、位置及原因
-----------      ----------   -----------------------------------------------
==============================================================================*/

/*==============================================================================                      
                             本文件包含的外部文件                         
==============================================================================*/
#ifndef AEE_SIMULATOR
#include "customer.h"         // Handset Feature Definitions
#else
#include "BREWSimFeatures.h"  // Simulator Feature Definitions
#endif

#if !defined(BREW_STATIC_APP)
//Use the .bid file if compiling as dynamic applet
#include "CameraApp.bid"
#else
#include "OEMClassIds.h"      // Applet Ids
#ifndef AEECLSID_APP_CAMERA
#error AEECLSID_APP_CAMERA must be defined before including cameraApp.h
#endif
#endif

#include "cameraApp.h"
#include "cameraapp.brh"
#include "AEEShell.h"
#include "AEEModTable.h"
#include "AEEStdLib.h"         // AEE StdLib Services
#include "AEEModGen.h"         // AEEMod Declaration
#include "AEEAppGen.h"         // AEEApplet Declaration
#include "OEMClassIDs.h"

#include "AEEMenu.h"           // AEE Menu Services
#include "AEEFile.h"           // AEE File Services
#include "AEEImage.h"
#include "AEEBitmap.h"
#include "AEEImageCtl.h"
#include "AEEText.h"           // AEE Text Services
#include "AEEMimeTypes.h"  
#ifndef AEE_SIMULATOR  
#include "AEEAlert.h"          // IAlert Services
#include "AEEGraphics.h"
#include "OEMCFGI.h"
#endif

//callback structure definitions.
#include "AEEVector.h"
#include "AEEAnnunciator.h"
#include "AEEBacklight.h"
#include "AEEConfig.h"
#include "CameraApp.h"
#include "AEECamera.h"
#include "AEECM.h"
#include "ICameraExt.h"
#include "OEMCamera.h"
#include "Appscommon.h"
#include "BREWVersion.h"
#include "Appscommon.h"
#include "AppComFunc.h"
#include "err.h"
#include "AEEAnnunciator.h"
#include "AEEMediaUtil.h"
#include "AEEMedia.h"
#include "AEECamera.h"
#include "cameraapp_image.brh"
#include "db.h"
#ifdef FEATURE_SUPPORT_VC0848
#include "Vc0848.h"
#endif
#include "Appscommon.brh"
#include "OEMSVC.h"


/*==============================================================================
                                 类型定义
==============================================================================*/
#define AEE_APPSCAMERAAPP_RES_FILE  AEE_RES_LANGDIR CAMERAAPP_RES_FILE

// camera quality
#define CAMERA_APP_QUALITY_HIGH  33
#define CAMERA_APP_QUALITY_MED   67
#define CAMERA_APP_QUALITY_LOW   100

// camera zoom
#define CAMERA_APP_ZOOM_UP       1
#define CAMERA_APP_ZOOM_DOWN     2

// bar and menu position
#define TOPBAR_ICON_Y         2
#define TOPBAR_ICON_WIDTH     17
#define TOPBAR_TITLE_TEXT_Y   6
#define CFGBAR_Y              160
#define CFGBAR_EFFECT_TEXT_Y  180
#define SCROLLBAR_X           60
#define SCROLLBAR_Y           160
#define POPMENU_BOTTOM_X      60
#define POPMENU_BOTTOM_Y      60
#define POPMENU_WIDTH         100 //117

//Add By zzg 2010_07_24
#define CFGBAR_TEXT_X		   2
#define CFGBAR_TEXT_Y		   18
#define CFGBAR_TEXT_WIDTH	   80
#define CFGBAR_TEXT_HEIGHT	   16		//NormalFont Height

#define CFGBAR_ARROW_WIDTH		8
#define CFGBAR_ARROW_HEIGHT		14
//Add End

// pop timeout
#define TIMEOUT_MS_MSGBOX     3000
#define TIMEOUT_MS_MSGDONE    1000
#define TIMEOUT_MS_QUICK      300

// file name lenth
#define MIN_FILE_NAME_LEN      64
#define MIN_PICS_NAME_LEN      128
#define MIN_PIC_CHAR_NAME_LEN  13
#define MIN_PIC_WCHAR_NAME_LEN 26
#define MIN_PIC_NUMOFSUM_LEN   4  
#define MIN_TEXT_HOTKEY_LEN    50  
#define MIN_TEXT_RES_LEN       20

#define MIN_FREE_MEMORY_CARD_SPACE   32
#define MIN_FREE_PHONE_SPACE         1

// file path
#define FS_EFS_DIRECTORY_STR       "fs:/hsmm/"
#define FS_CARD_PICTURES_FOLDER    MG_PICTURES_FOLDER DIRECTORY_STR
#define FS_CARD_MUSIC_FOLDER       MG_MUSIC_FOLDER DIRECTORY_STR
#define FS_CARD_VIDEOS_FOLDER      MG_VIDEOS_FOLDER DIRECTORY_STR
#define FS_EFS_PICTURES_FOLDER     FS_EFS_DIRECTORY_STR FS_CARD_PICTURES_FOLDER

// video recorder timer
#define RUNNING_UPDATE_MSECS   1000
#define MSECS_PER_DAY         (3600*24*1000)

#define BYTE_SIZE  1024

#define WHITE_TEXT            MAKE_RGB(251, 251, 251)

// 画界面底部提示条宏定义
#define DRAW_BOTTOMBAR(x)                           \
{                                                   \
    BottomBar_Param_type BarParam;                  \
    MEMSET(&BarParam, 0, sizeof(BarParam));         \
    BarParam.eBBarType = x;                         \
    DrawBottomBar(pMe->m_pDisplay, &BarParam);      \
}

typedef enum
{
    OPTION_BACK,
    SELECT_BACK,
    SAVE_DEL       
}BOTTOMBARTEX;

typedef enum
{
    COLORNORMAL,
    COLORBANDW,
    COLORSEPIA,
    COLORNEGATIVE,
    COLORRED,
    COLORGREEN  
} COLOREFFECTSEL;

// 照相机设置参数列表
typedef enum 
{
#ifdef FEATURE_VIDEO_ENCODE
    CAMERACFGCHOOSE,
#endif
    CAMERACFGENVIRMENT,
#ifdef FEATURE_CAMERAAPP_FUN_FRAME
    CAMERACFGFUN,
#endif
    CAMERACFGQUALITY,
    CAMERACFGSIZE,
    CAMERACFGTONE,
    CAMERACFGSELFTIME,
    CAMERACFGCAPTURE,
    CAMERACFGBANDING,
    CAMERACFGSTORAGE,
    CAMERACFGRESET,
    CAMERACFGHOTKEY
} CAMERACFG;
#ifdef FEATURE_VIDEO_ENCODE
// 摄像机设置参数列表
typedef enum 
{
    VIDEOCFGCHOOSE,
    VIDEOCFGENVIROMENT,
    VIDEOCFGFUN,
    VIDEOCFGRESET,
    VIDEOCFGHOTKEY
} VIDEOCFG;
#endif
typedef enum 
{
	FILE_TYPE_JPG,
	FILE_TYPE_AMR,
	FILE_TYPE_3GP,
    FILE_TYPE_ERROR = 0XFF
} FILE_TYPE;

// Camera状态
typedef enum
{
    CAM_START,
    CAM_PREVIEW,
    CAM_CAPTURE,    
    CAM_RECORD,
    CAM_STOP
} CAMERASTATE; 

/*----------------------对话框相关数据类型声明---------------------*/
// 对话框关闭时返回值列表
typedef enum DLGRetValue
{
    DLGRET_CREATE,
    DLGRET_MAINMENU,
    DLGRET_PREVIEW,
    DLGRET_PICMENU,
    DLGRET_CAMERACFG,
    DLGRET_VIDEO,
    DLGRET_VIDEOCFG,
    DLGRET_VIDEOREC,  
    DLGRET_VIDEOSAVE,
    DLGRET_FUN,
    DLGRET_POPMSG,
    DLGRET_CANCELED,
    DLGRET_EXIT 
} DLGRetValue;

/*----------------------状态机相关数据类型声明---------------------*/
// CameraApp Applet 状态机状态：
typedef enum
{
    STATE_NULL,
    STATE_CMAINMENU,
    STATE_CPREVIEW,
    STATE_CCAMERACFG,
    STATE_CPIC,
    STATE_CVIDEO,
    STATE_CVIDEOCFG,
    STATE_CVIDEOSAVE,  
    STATE_CFUN,
    STATE_CPOPMSG,
    STATE_EXIT
} FSMState;

// 状态处理函数返回给状态处理主函数的值类型
typedef enum NextFSMAction
{
    NFSMACTION_WAIT,
    NFSMACTION_CONTINUE
} NextFSMAction;

// 保存模块信息和模块引用计数的结构
typedef struct _CameraAppMod
{
    DECLARE_VTBL(IModule)
    uint32   m_nRefs;
} CCameraAppMod;

// CameraApp Applet对象结构体：
typedef struct _CCameraApp
{
    DECLARE_VTBL(ICameraApp)
    uint32               m_nRefs;          // 对象引用计数器
    IModule              *m_pModule;
    IDisplay             *m_pDisplay;
    IShell               *m_pShell;    
    AEERect               m_rc;
    
    // 当前活动对话框IDialog接口指针
    IDialog              *m_pActiveDlg;
    
    // 当前活动对话框ID
    uint16               m_pActiveDlgID;
    
    // 活动对话框关闭时的返回结果
    DLGRetValue          m_eDlgRet;
     
    // 是否不改写对话框返回结果，一般情况需要改写(FALSE)。
    boolean              m_bNotOverwriteDlgRet;
    
    // Applet 前一状态
    FSMState             m_ePreState;
    
    // Applet 当前状态
    FSMState             m_eCurState;
    
    // Applet是否处于挂起状态
    boolean              m_bSuspending;
    
   // Applet是否准备好可以处理按键和命令事件。改变量主要用于快速按键引起的问题
    boolean              m_bAppIsReady;
        
    boolean              m_bVideoPause;          // 判断摄影是否停止
    boolean              m_bfunFrameSelected;    // 趣味模式相框选择
    boolean              m_bVideoRecStart;        // 判断录像是否开始
    boolean              m_bSelNotChanged;       // 判断Sel是否变化
    boolean              m_bCameraHideIcon;      // 判断Camera Icon是否显示
    boolean              m_bVideoHideIcon;       // 判断Video Icon是否显示
    boolean              m_bIsPreview;           // 判断camera是否在preview状态
    boolean              m_bRePreview;           // 判断是否需要camera重新preview
    boolean              m_bCapturePic;          // 判断camera是否在拍照状态，用于禁止按键事件
    boolean              m_bCleanScreen;         // 用于带相框拍照的清屏处理
    boolean              m_bDecodeJpgDone;       // 判断采用qualcomm Jpg解码是否完成
    //boolean              m_bCalculateLeftPicNum; // 计算拍照剩余张数
    boolean              m_bCanCapture;          // 检测存储容量后，判断是否可以拍照
    boolean              m_bMemoryCardExist;     // 检测是否存在存储卡
    boolean              m_bDispHotKey;
    
    uint16               m_wMsgID;               // pop对话框，显示文本的资源ID号
    uint32               m_nMsgTimeout;           // pop对话框，自动关闭所需要的时间 

    uint32               m_nMainMenuItemSel;     // 主菜单项选择
    uint32               m_nSelfTimeItemSel;     // 拍照时间菜单项选择
    uint32               m_nCaptureItemSel;      // 连拍菜单项选择
    uint32               m_nCFGBarChoosedID;     // 设置菜单选中时候的位置序号
    uint32               m_nCFGBarOffset;        // 绘制设置菜单栏选中位置，所需要的偏移量

    uint32               m_nTicks;               // tick时间
    uint32               m_nNumDays;             // 跑表跑的天数
    uint32               m_dwDispTime;           // 显示时间
    uint32               m_dwMemTotal;           // 存储空间总容量
    uint32               m_dwMemFree;            // 存储空间剩余容量

    int                  m_nSnapShotTimes;
    int                  m_nSnapShotPicNum;
    int                  m_nSnapShotPicNameIndex;
    int                  m_nLeftTime;
    int                  m_cxWidth; 
    int                  m_cyHeight;  
    int                  m_nItemH;               // 文本默认字体高度
    
    AECHAR               m_szHotKeyTextBuf[MIN_TEXT_HOTKEY_LEN];
    AECHAR               m_szResTextBuf[MIN_TEXT_RES_LEN];
    
    char                 *m_pszArgs;             // 启动参数
    char                 m_sCurrentFileName[MIN_FILE_NAME_LEN];
    char                 m_sCaptureFileName[MIN_PICS_NAME_LEN];
    char                 m_sFileNameWithAllPath[MIN_FILE_NAME_LEN];
    AECHAR               m_wFileName[MIN_PIC_WCHAR_NAME_LEN];
    AECHAR               m_nNumOfSnapshot[MIN_PIC_NUMOFSUM_LEN];
    AEECallback          m_CallBack;
    
    CAMERACFG            m_nCameraCFG;           // camera设置
#ifdef FEATURE_VIDEO_ENCODE
    VIDEOCFG             m_nVideoCFG;            // video设置
#endif
    COLOREFFECTSEL       m_nColorSel;            // color设置菜单的选中的item
    CAMERASTATE          m_nCameraState;         // camera的运行状态
    
    OEMCAMERAQUALITY     m_nCameraQuality;       // camera拍照质量
    OEMCAMERATONE        m_nCameraTone;          // camera快门声音
    OEMCAMERAENVIR       m_nCameraEnviroment;    // camera拍照环境  fps
    OEMCAMERASIZE        m_nCameraSize;          // camera拍照尺寸
    OEMCAMERABANDING     m_nCameraBanding;       // camera bandig设置
    OEMCAMERASTORAGE     m_nCameraStorage;       // camera拍照存储位置
    OEMCAMERABRIGHTNESS  m_nCameraBrightness;    // camera拍照亮度
    OEMCAMERAZOOM        m_nCameraZoom;          // camera拍照变焦
    OEMCAMERAFRAME       m_nCameraFrame;         // camera相框设置，仅176x220拍照有相框
    OEMCAMERACOLOR       m_nCameraColor;         // camera拍照滤镜颜色设置    
    
    ICamera              *m_pCamera;
    ITimeCtl             *m_pVideoRecTimeCtl;
    IFileMgr             *m_pFM; 
    IConfig              *m_pConfig;              // IConfig interface
    IStatic              *m_pStatic;              // 快捷键屏幕显示文本  
    AEERect               m_rcStatic;
    IMedia               *m_pMedia;
    IImage               *m_pBusyStateImage;
    IFileMgr             *m_pFileMgr;
    IAnnunciator         *m_pIAnn;
} CCameraApp;

/*==============================================================================                         
                                 宏定义和常数                        
==============================================================================*/
// 为防止用户快速按键而添加事件EVT_APPISREADY，事件由定时器
// CameraMenu_APPIsReadyTimer发出
#define APPISREADY_TIMER    100                 
#define EVT_APPISREADY      (EVT_USER+1)

// 状态移动宏：先将前一状态用当前状态更新，再将当前状态设为nextState
#define MOVE_TO_STATE(nextState) {               \
            FSMState  tpState;                   \
            tpState = nextState;                 \
            pMe->m_ePreState = pMe->m_eCurState; \
            pMe->m_eCurState = tpState;          \
         }

// 关闭对话框宏
#define CLOSE_DIALOG(DlgRet) {                                           \
                                 pMe->m_eDlgRet = DlgRet;                \
                                 (void) ISHELL_EndDialog(pMe->m_pShell); \
                             }

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
       CameraApp_ShowDialog
说明:
       函数由状态处理函数用来创建相应对话框资源ID的对话框。

参数:
       pMe [In]: 指向CameraApp Applet对象结构的指针,该结构包含小程序的特定信息.
       dlgResId [in]：对话框资源ID，函数根据该ID创建对话框。

返回值:
       无

备注:
       

==============================================================================*/
void CameraApp_ShowDialog(CCameraApp *pMe, uint16 dlgResId);

/*==============================================================================
函数:
       CameraApp_RouteDialogEvent
说明:
       函数将BREW事件路由给当前活动对话框的事件处理函数。

参数:
       pMe [In]: 指向CameraApp Applet对象结构的指针,该结构包含小程序的特定信息.
       eCode [in]：事件代码。
       wParam [in]：与事件eCode关联的数据。
       dwParam [in]：与事件eCode关联的数据。

返回值:
       TRUE：传入事件被处理。
       FALSE：传入事件没被处理。

备注:

==============================================================================*/
boolean CameraApp_RouteDialogEvent(CCameraApp *pMe,
                                   AEEEvent      eCode,
                                   uint16        wParam,
                                   uint32        dwParam);

/*----------------------状态机相关函数声明---------------------*/
/*==============================================================================
函数:
       CameraApp_ProcessState
说明:
       CameraApp Applet状态处理主函数。函数根据pMe->m_currState将调用路由至
       相应的处理函数。

参数:
       pMe [In]: 指向CameraApp Applet对象结构的指针,该结构包含小程序的特定信息.

返回值:
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注:

==============================================================================*/
NextFSMAction CameraApp_ProcessState(CCameraApp *pMe);

/*============================================================================
   快速按键处理函数
============================================================================*/
void CCameraApp_APPIsReadyTimer(void *pme);

#endif // end cameraAPP_PRIV_H

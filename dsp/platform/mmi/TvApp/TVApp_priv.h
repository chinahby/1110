#ifndef TVAPP_PRIV_H
#define TVAPP_PRIV_H

#define AEE_APPSTVAPP_RES_FILE  AEE_RES_LANGDIR TVAPP_RES_FILE

/*==============================================================================
// 文件：TVApp_priv.h
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
#include "TVApp.bid"
#else
#include "OEMClassIds.h"      // Applet Ids
#ifndef AEECLSID_TVAPP
#error AEECLSID_TVAPP must be defined before including cameraApp.h
#endif
#endif

//#include "TVApp.h"
#include "tvapp.brh"
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
#include "TVApp.h"
//#include "AEECamera.h"
#include "AEECM.h"
//#include "ICameraExt.h"
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
#include "tvapp_image.brh"
#include "db.h"
#ifdef FEATURE_SUPPORT_VC0848
#include "Vc0848.h"
#endif
#include "Appscommon.brh"
#include "OEMSVC.h"
#include "AEEBacklight.h"
#include "AEETv.h"
#include "IMMITv.h"
//#include "TvSettingCfg.h"
#include "CustomOEMConfigItems.h"

/*==============================================================================
                                 类型定义
==============================================================================*/
#define AEE_APPSTVAPP_RES_FILE  AEE_RES_LANGDIR TVAPP_RES_FILE
#define SEARCH_ANI_FILE "fs:/image/tvapp/search.gif"
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


#define NrOfTools 9  // tool num

#define LISTIMAGE_ADD_W	16 
#define LISTIMAGE_ADD_H	16
#define LISTIMAGE_DEFAULT_W 16
#define LISTIMAGE_DEFAULT_H 16
#define LISTIMAGE_FAVICON_W 16
#define LISTIMAGE_FAVICON_H 16
#define BROWSER_BOOKMARK_BLOCK_SIZE 1024
#define BROWSER_BOOKMARK_CAPACITY 1024*1024
#define MAX_CHANNEL_NUMBER  32 //最多有32个可用台

/*横屏*/
#define HORIZONTAL_TV_WIDTH      320
#define HORIZONTAL_TV_HEIGHT     240
/*竖屏*/ 
#define VERTICAL_TV_WIDTH        240
#define VERTICAL_TV_HEIGHT       192  

typedef struct 
{
	boolean				b_IsIconHiden;
	//TTvPropertyValue 	ResolutionID_DC;	// Resolution for snapshot
	int32 	ResolutionID_DV;	// Resolution for recording
	int32	BrightnessStep; /*设置亮度0~15*/
    int32   ContrastStep; /*设置对比度0~15*/
    int32   DefinitionStep; /*设置清晰度0~63*/
    int32   SaturationStep; /*设置色彩饱和度0~255*/
    int32   SoundStep;/*声音*/
    int ChannelCountAble;//上次搜到的有信号的台总数
    int CurrentChannel;//上一次退出之前看的那个台索引    
    TvBookmark   Bookmark[32];//最多可以保存32个书签
    int   Bookmarktotal;       //当前存储频道总数
    TLG_REGION_CODE region;//当前设置的区域
}MMITV_SETTINGS;

#if 0
// 画界面底部提示条宏定义
//#define DRAW_BOTTOMBAR(x)                           \
//{                                                   \
 //   BottomBar_Param_type BarParam;                  \
//MEMSET(&BarParam, 0, sizeof(BarParam));         \
//
//    BarParam.eBBarType = x;                         \
//    DrawBottomBar(pMe->m_pDisplay, &BarParam);      \
//}
#endif

typedef enum
{
    OPTION_BACK,
    SELECT_BACK,
    SAVE_DEL       
}BOTTOMBARTEX;

// 照相机设置参数列表
typedef enum 
{
    TVCFGENVIRMENT,
    TVCFGFIRST = TVCFGENVIRMENT,
    TVCFGQUALITY,
    TVCFGSIZE,
    TVCFGTONE,
    TVCFGSELFTIME,
    TVCFGBANDING,
    TVCFGRESET,
    TVCFGLAST = TVCFGRESET,
    TVCFGMAX
} TVCFG;

typedef enum 
{
	FILE_TYPE_JPG,
	FILE_TYPE_AMR,
	FILE_TYPE_3GP,
    FILE_TYPE_ERROR = 0XFF
} FILE_TYPE;

// TV状态
typedef enum
{
    TV_START,
    TV_PREVIEW,
    TV_CAPTURE,
    TV_SAVE,
    TV_RECORD,
    TV_STOP
} TVSTATE; 

/*----------------------对话框相关数据类型声明---------------------*/
// 对话框关闭时返回值列表
typedef enum DLGRetValue
{
    DLGRET_CREATE,
    DLGRET_MAINMENU,
    DLGRET_PREVIEW,
    DLGRET_PICMENU,
    DLGRET_TVCFG,
    DLGRET_POPMSG,
    DLGRET_CANCELED,
    DLGRET_REGION,
    DLGRET_ASIA,
    DLGRET_LATIN,
    DLGRET_EUROPE,
    DLGRET_AFRICA,
    DLGRET_OCEANIA,
    DLGRET_DRAWTOPBAR,
    DLGRET_BAR1,
    DLGRET_BAR2,
    DLGRET_BAR3,
    DLGRET_BAR4,
    DLGRET_BAR5,
    DLGRET_BAR6,
    DLGRET_BAR7,
    DLGRET_AUTOSCAN,
    DLGRET_BOOKMARK,
    DLGRET_EXIT 
} DLGRetValue;

/*----------------------状态机相关数据类型声明---------------------*/
// TVApp Applet 状态机状态：
typedef enum
{
    STATE_NULL,
    STATE_CMAINMENU,
    STATE_CPREVIEW,
    STATE_CCAMERACFG,
    STATE_CPIC,
    STATE_CPOPMSG,
    STATE_REGION,
    STATE_ASIA,
    STATE_LATIN,
    STATE_EUROPE,
    STATE_AFRICA,
    STATE_OCEANIA,
    STATE_DRAWTOPBAR,
    STATE_BAR1,
    STATE_BAR2,
    STATE_BAR3,
    STATE_BAR4,
    STATE_BAR5,
    STATE_BAR6,
    STATE_BAR7,
    STATE_AUTOSCAN,
    STATE_BOOKMARK,
    STATE_EXIT
} FSMState;

// 状态处理函数返回给状态处理主函数的值类型
typedef enum NextFSMAction
{
    NFSMACTION_WAIT,
    NFSMACTION_CONTINUE
} NextFSMAction;

// 保存模块信息和模块引用计数的结构
typedef struct _TVAppMod
{
    DECLARE_VTBL(IModule)
    uint32   m_nRefs;
} CTVAppMod;

// TVApp Applet对象结构体：
typedef struct _CTVApp
{
    DECLARE_VTBL(ITVApp)
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
    
    boolean              m_bIsPreview;           // 判断camera是否在preview状态
    boolean              m_bRePreview;           // 判断是否需要camera重新preview
    boolean              m_bCapturePic;          // 判断camera是否在拍照状态，用于禁止按键事件
    boolean              m_bCanCapture;          // 检测存储容量后，判断是否可以拍照
    boolean              m_bMemoryCardExist;     // 检测是否存在存储卡
    
    uint16               m_wMsgID;               // pop对话框，显示文本的资源ID号
    uint32               m_nMsgTimeout;           // pop对话框，自动关闭所需要的时间 

    uint32               m_nMainMenuItemSel;     // 主菜单项选择
    uint32               m_nSelfTimeItemSel;     // 拍照时间菜单项选择
    
    uint32               m_nTicks;               // tick时间
    uint32               m_nNumDays;             // 跑表跑的天数
    uint32               m_dwDispTime;           // 显示时间
    uint32               m_dwMemTotal;           // 存储空间总容量
    uint32               m_dwMemFree;            // 存储空间剩余容量

    int                  m_nLeftTime;
    int                  m_cxWidth; 
    int                  m_cyHeight;  
    int                  m_nItemH;               // 文本默认字体高度
        
    char                 m_sCurrentFileName[MIN_FILE_NAME_LEN];
    char                 m_sCaptureFileName[MIN_PICS_NAME_LEN];
    AEECallback          m_CallBack;

	ITlgAtv				*pITv;
	IMMITv				*pIMMITv;
	ICBMMITv			myICBMMITv;
    TVCFG                m_nTVCFG;           // camera设置
    TVSTATE              m_nTVState;         // TV的运行状态
    //CAMERASTATE          m_nCameraState;         // camera的运行状态
    
    OEMCAMERAQUALITY     m_nTVQuality;       // camera拍照质量
    OEMCAMERATONE        m_nTVTone;          // camera快门声音
    OEMCAMERAENVIR       m_nTVEnviroment;    // camera拍照环境  fps
    OEMCAMERASIZE        m_nTVSize;          // camera拍照尺寸
    OEMCAMERABANDING     m_nTVBanding;       // camera bandig设置
    OEMCAMERASTORAGE     m_nTVStorage;       // camera拍照存储位置

	IImage               *m_pImage;
	ICamera            *m_pCamera;
    ICamera            *m_pTV;
    IFileMgr             *m_pFM; 
    IConfig              *m_pConfig;              // IConfig interface
    IMedia               *m_pMedia;
    IFileMgr             *m_pFileMgr;
    IAnnunciator         *m_pIAnn;
    boolean              m_isFormQuicktest;
    IBacklight           *m_pBacklight;
    byte                 m_nBacklightVal;
    int32                 m_sensor_model; //判断摄像头为多少像素的 add by xuhui

	AEEApplet        	a;
    IShell 				*pIShell;

	CFG_TvSetting	*pTvSetting;
	
	uint16   currentlyChannel; //当前正在播放的频道
    uint16   currentlyChannelIndex;//当前正在播放的频道在书签中的逻辑索引
} CTVApp;

/*==============================================================================                         
                                 宏定义和常数                        
==============================================================================*/
// 为防止用户快速按键而添加事件EVT_APPISREADY，事件由定时器
// TVMenu_APPIsReadyTimer发出
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
       TVApp_ShowDialog
说明:
       函数由状态处理函数用来创建相应对话框资源ID的对话框。

参数:
       pMe [In]: 指向TVApp Applet对象结构的指针,该结构包含小程序的特定信息.
       dlgResId [in]：对话框资源ID，函数根据该ID创建对话框。

返回值:
       无

备注:
       

==============================================================================*/
void TVApp_ShowDialog(CTVApp *pMe, uint16 dlgResId);

/*==============================================================================
函数:
       TVApp_RouteDialogEvent
说明:
       函数将BREW事件路由给当前活动对话框的事件处理函数。

参数:
       pMe [In]: 指向TVApp Applet对象结构的指针,该结构包含小程序的特定信息.
       eCode [in]：事件代码。
       wParam [in]：与事件eCode关联的数据。
       dwParam [in]：与事件eCode关联的数据。

返回值:
       TRUE：传入事件被处理。
       FALSE：传入事件没被处理。

备注:

==============================================================================*/
boolean TVApp_RouteDialogEvent(CTVApp *pMe,
                                   AEEEvent      eCode,
                                   uint16        wParam,
                                   uint32        dwParam);

/*----------------------状态机相关函数声明---------------------*/
/*==============================================================================
函数:
       TVApp_ProcessState
说明:
       TVApp Applet状态处理主函数。函数根据pMe->m_currState将调用路由至
       相应的处理函数。

参数:
       pMe [In]: 指向TVApp Applet对象结构的指针,该结构包含小程序的特定信息.

返回值:
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注:

==============================================================================*/
NextFSMAction TVApp_ProcessState(CTVApp *pMe);

/*============================================================================
   快速按键处理函数
============================================================================*/
void CTVApp_APPIsReadyTimer(void *pme);
void TVApp_AppEventNotify(CTVApp *pMe, int16 nCmd, int16 nStatus);
void TVApp_InitTVCheck(void *po);


#endif // end cameraAPP_PRIV_H

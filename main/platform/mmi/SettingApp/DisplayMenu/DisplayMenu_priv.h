#ifndef DISPLAYMENUPRIV_H
#define DISPLAYMENUPRIV_H

/*==============================================================================
// 文件：
//        DisplayMenu_priv.h
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
   #include "DisplayMenu.bid"
#else
   #include "OEMClassIds.h"  // Applet Ids
   #ifndef AEECLSID_APP_DISPLAYMENU
      #error AEECLSID_APP_DISPLAYMENU must be defined
   #endif
#endif

#include "DisplayMenu.brh"
#include "AEEShell.h"
#include "AEEModTable.h"
#include "AEEStdLib.h"
#include "AEEModGen.h"
#include "AEEAppGen.h"
#include "OEMClassIDs.h"
//#include "OEMMacros.h"
#include "DisplayMenu.h"

#include "AEEAnnunciator.h"

#include "AEEMenu.h"
#include "AEEConfig.h"
#include "AppComFunc.h"
#include "Appscommon.h"
#include "OEMText.h"
#include "AEEBacklight.h"
#include "AEEFile.h"
#ifndef WIN32
#include "Oemui.h"
#include "db.h"
#else
#include "oemhelperfunctype.h"
#endif//WIN32
#include "OEMCFGI.h"

#ifdef FEATURE_RANDOM_MENU_REND//wlh 20090405 add for rend
#include "Rendering.h"
#endif//FEATURE_RANDOM_MENU_REND
#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
#define DISPLAYMENU_PT_IN_RECT(a,b,rct)      (boolean)( ((a) >= (rct).x && (a) <= ((rct).x + (rct).dx)) && ((b) >= (rct).y && (b) <= ((rct).y + (rct).dy)) )
#endif//FEATURE_LCD_TOUCH_ENABLE

/*==============================================================================
                                 类型定义
==============================================================================*/
#define  AEE_APPSDISPLAYMENU_RES_FILE AEE_RES_LANGDIR DISPLAYMENU_RES_FILE
#define DISP_ERR(format, code1, code2, code3) //ERR(format, code1, code2, code3)
// 画界面底部提示条宏定义
#define DISPLAYMENU_DRAW_BOTTOMBAR(x)                           \
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
    DLGRET_CANCELED,
    DLGRET_BACKLIGHTING,  //IDD_BACKLIGHTING_MENU
    DLGRET_CONTRAST,      //IDD_CONTRAST_MENU
    DLGRET_BRIGHTNESS = DLGRET_CONTRAST, //IDD_CONTRAST_MENU
    DLGRET_SCREENTIMEOUT, //IDD_SCREENSAVERTIMEOUT_DIALOG
    DLGRET_PICTURE,       //IDD_PICTURE_DLG
    DLGRET_IMGDELMSG,      //IDD_IMGDELMSG_DLG
    DLGRET_DESKTOP_THEME,//IDD_DESKTOP_THEME
    DLGRET_LED_CONTROL  , 
    DLGRET_KEY_PAD_CTL ,
    DLGRET_MENU_STYLE_CTL,
#ifdef FEATURE_RANDOM_MENU_COLOR
    DLGRET_COLORFUL_MENU_CTL,
    DLGRET_COLOR_SETTING,
#endif
#ifdef FEATURE_RANDOM_MENU_REND//wlh 20090405 add for rend
    DLGRET_REND_MENU_CTL,
#endif
    DLGRET_MSG_POP
} DLGRet_Value_e_Type;

/*----------------------状态机相关数据类型声明---------------------*/
// DisplayMenu Applet 状态机状态：
typedef enum _DisplayMenuState
{
    DISPLAYMENUST_NONE,
    DISPLAYMENUST_INIT,
    DISPLAYMENUST_MAIN,
    DISPLAYMENUST_BACKLIGHTING,
    DISPLAYMENUST_CONTRAST,    
    DISPLAYMENUST_SCREENTIMEOUT,
    DISPLAYMENUST_PICTURE,
    DISPLAYMENUST_IMGDELMSG,    
    DISPLAYMENUST_DESKTOP_THEME,
    DISPLAYMENUST_LED_CONTROL,
    DISPLAYMENUST_KEY_PAD_CONTROL,
    DISPLAYMENUST_MENU_STYLE_CONTROL,
#ifdef FEATURE_RANDOM_MENU_COLOR
    DISPLAYMENUST_COLORFUL_MENU,
    DISPLAYMENUST_COLOR_SETTING,
#endif
#ifdef FEATURE_RANDOM_MENU_REND//wlh 20090405 add for rend
    DISPLAYMENUST_REND_MENU,
#endif
    DISPLAYMENUST_EXIT

} DisplayMenuState;

// 状态处理函数返回给状态处理主函数的值类型
typedef enum NextFSMAction
{
    NFSMACTION_WAIT,
    NFSMACTION_CONTINUE
} NextFSMAction;

// 保存模块信息和模块引用计数的结构
typedef struct DisplayMenuMod
{
    DECLARE_VTBL(IModule)
    uint32   m_nRefs;
} DisplayMenuMod;

// 墙纸类型, 开/关机动画
typedef enum {
     WALLPAPER_NONE
    ,WALLPAPER_MAIN      /*//大屏背景图*/
    ,WALLPAPER_SUB        /*//小屏背景图*/
#ifdef FEATURE_ANIMATION_POWERUPDOWN
    ,ANIMATION_POWERUP   /*//开机动画*/
    ,ANIMATION_POWERDOWN /*//关机动画*/
#endif//FEATURE_ANIMATION_POWERUPDOWN

#ifdef FEATURE_SCREEN_SAVE
    ,SCREENSAVE_TYPE    /*//桌面墙纸*/
#endif
} PICTURE_TYPE;

//图片格式类型
typedef enum
{
    IMAGE_NONE,
    IMAGE_BMP,          //bmp
    IMAGE_PNG,          //png
    IMAGE_JPEG,         //jpg
    IMAGE_GIF,          //gif
    IMAGE_BCI,        //bci
    IMAGE_LAST
} ImageType;

typedef enum {
   SET_CONTRAST,
   SET_BACKLIGHT
} SET_CONTRAST_BACKLIGHT_TYPE;

typedef struct  _image_name_table
{
    char imange_name[MAX_FILE_NAME+1];//65
    struct _image_name_table *pref;
    struct _image_name_table *next;
}image_name_table;

typedef struct _double_list
{
    image_name_table *phead;
    image_name_table *ptail;
    image_name_table *pcur;
    int16            count;
}double_list;

// DisplayMenu Applet对象结构体：
typedef struct _CDisplayMenu
{
    DECLARE_VTBL(IDisplayMenu)
    uint32          m_nRefs;          // 对象引用计数器
    IModule        *m_pModule;
    IDisplay       *m_pDisplay;
    IShell         *m_pShell;
    IAnnunciator *m_pIAnn;

    AEERect         m_rc;
    // 当前活动对话框IDialog接口指针
    IDialog        *m_pActiveDlg;
    // 当前活动对话框ID
    uint16          m_pActiveDlgID;
    // 活动对话框关闭时的返回结果
    DLGRet_Value_e_Type m_eDlgRet;
    // 是否不改写对话框返回结果，一般情况需要改写(FALSE)。
    boolean          m_bNotOverwriteDlgRet;
    // Applet 前一状态
    DisplayMenuState m_ePreState;
    // Applet 当前状态
    DisplayMenuState m_eCurState;
    // Applet是否处于挂起状态
    boolean         m_bSuspending;
    // Applet是否准备好可以处理按键和命令事件。改变量主要用于快速按键引起的问题
    boolean         m_bAppIsReady;

    uint16          m_currDlgId;       /*保存当前所选菜单位置*/
    uint16          m_nSubDlgId;       /*次级菜单位置*/
    uint16          m_nExSubDlgId;    /* 第三极菜单位置*/
    IConfig         *m_pConfig;        /*IConfig 指针*/
    byte            m_lightlevel;      /*背光*/
    //byte            m_contrastlevel;     /* //对比度*/
    IBacklight      *m_pIBacklight;   /* 用来在设置完背光后立即生效用*/
#if defined(FEATURE_BACKLIGHT_KEYPAD)
    IBacklight      *m_pKeypadBacklight;   /* 用来在设置完按键灯控制时间后立即生效用*/
#endif
    IFileMgr        *m_pIFileMgr;     /* IFILEMGR指针*/
    PICTURE_TYPE     m_PICType;        /* 墙纸类型,开/关机动画*/
    image_name_table *m_CurPaper;       /* 目前显示的墙纸*/
    boolean          m_DelImageBack;   /* 根据图片NV决定是否定位到nv图片*/
    //uint16          m_WallCount;      /* 普通墙纸/动画个数*/
    //uint16          m_Downcount;      /* 下载墙纸个数*/
    IImage           *m_pWallPaper;    /* 墙纸IImage指针*/
    IImage           *m_pAniPaper;     /*开关机动画IImage指针*/
    //IImage           *m_pTitleImg;     /* title顶条IImage指针*/
    boolean          m_bOkToDo;        /* 浏览图片时限制按键速度*/
    //boolean         m_contrastAdjust;  // Adjusting contrast?
    //boolean         m_storeInit;       // Store this contrast level?
    //byte            m_initLevel;       // Initial contrast level
    //byte            m_newLevel;        // New contrast level
    //IOEM_TSGBridge  *m_pOEM_TSGBridge; // IOEMTSG_Bridge interface
    //int16           m_controlId;
    AEEApplet         a;
    SET_CONTRAST_BACKLIGHT_TYPE  m_contrast_backlight;
    byte m_BiasV;
    uint16            m_count;
    boolean           m_b_local;
#ifdef FEATURE_LED_CONTROL
    byte              m_led_ctr;
#endif
#ifdef FEATRUE_KEY_PAD_CTL
    //ClockCtl        m_ClockCtlStart;
    //ClockCtl        m_ClockCtlEnd;
    ITimeCtl          *m_TimeCtlStart;
    ITimeCtl          *m_TimeCtlEnd;
    byte              m_SetTimeRange;
    Key_pad_Cfg       m_ClockCfg;
    IMenuCtl*         m_pState;
    uint16            m_nCtlID;           //控件ID
    ClockCtlFocus     m_focus;    
    uint16           m_nTimeCtlCount;    //0代表焦点在小时上,1代表在分钟上
    uint16           m_nNumKeyCount;     //记录在设置时间时的按键次数.0:小时十位    
#endif
    uint16            m_msg_id;

#ifdef FEATURE_RANDOM_MENU_COLOR
    RGBVAL         m_nCFGColor; 
    boolean         m_bCustomColor;
#endif
#ifdef FEATURE_RANDOM_MENU_REND//wlh 20090405 add for REND
    uint32          m_nCFGRend;//当前效果 
    boolean         m_bRandomRend;//是否启用随机效果
#endif
} CDisplayMenu;

/*==============================================================================
                                 宏定义和常数
==============================================================================*/
// 为防止用户快速按键而添加事件EVT_APPISREADY，事件由定时器
// DisplayMenu_APPIsReadyTimer发出
#define APPISREADY_TIMER       100                        
#define EVT_APPISREADY        (EVT_USER+1)               
#ifdef FEATRUE_KEY_PAD_CTL
#define LINEHEIGHTEX          (IDISPLAY_GetFontMetrics(pMe->m_pDisplay,AEE_FONT_NORMAL,NULL,NULL) +2)
#define TEXT_X                (LINEHEIGHTEX/2)  
#define TEXT_DX               (40-TEXT_X)
#define CONTROL_X             40
#define CONTROL_DX            (pMe->m_rc.dx-40)
#endif
#define DISPLAY_MENU_SOFK_HIGHT 0
// 用户重新更新界面事件
//#define EVT_USER_REDRAW   (EVT_USER+2)

// 状态移动宏：先将前一状态用当前状态更新，再将当前状态设为nextState
#define MOVE_TO_STATE(nextState) {            \
            DisplayMenuState tpState;            \
            tpState = nextState;          \
            pMe->m_ePreState = pMe->m_eCurState; \
            pMe->m_eCurState = tpState;        \
        }

#define  NUM_OF_STARTUP_ANIMATION_ITEM    5       //开机动画选择
#define  NUM_OF_POWEROFF_ANIMATION_ITEM   5       //关机动画选择
#define  MAX_WALLPAPER_NUMBER             100     //桌面墙纸的最大个数
#define  FRAME_NUM_OF_ANIMATION           8       //动画的帧数
#define  ANIMATION_TIMER                  300     //动画的帧数

#define  BANNER_MAXTXT                    8       //待机问候语最大字符数

#define SCREENSAVERTIME_30_SECONDS     (30)
#define SCREENSAVERTIME_2_MINUTES  (2 * 60)
#define SCREENSAVERTIME_5_MINUTES  (5 * 60)
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
       DisplayMenu_ShowDialog
说明:
       函数由状态处理函数用来创建相应对话框资源ID的对话框。

参数:
       pMe [In]: 指向DisplayMenu Applet对象结构的指针,该结构包含小程序的特定信息.
       dlgResId [in]：对话框资源ID，函数根据该ID创建对话框。

返回值:
       无

备注:


==============================================================================*/
void DisplayMenu_ShowDialog(CDisplayMenu *pMe,uint16  dlgResId);

/*==============================================================================
函数:
       DisplayMenu_RouteDialogEvent
说明:
       函数将BREW事件路由给当前活动对话框的事件处理函数。

参数:
       pMe [In]: 指向DisplayMenu Applet对象结构的指针,该结构包含小程序的特定信息.
       eCode [in]：事件代码。
       wParam [in]：与事件eCode关联的数据。
       dwParam [in]：与事件eCode关联的数据。

返回值:
       TRUE：传入事件被处理。
       FALSE：传入事件没被处理。

备注:

==============================================================================*/
boolean DisplayMenu_RouteDialogEvent(CDisplayMenu *pMe,
    AEEEvent    eCode,
    uint16  wParam,
    uint32 dwParam
);

/*----------------------状态机相关函数声明---------------------*/
/*==============================================================================
函数:
       DisplayMenu_ProcessState
说明:
       DisplayMenu Applet状态处理主函数。函数根据pMe->m_currState将调用路由至
       相应的处理函数。

参数:
       pMe [In]: 指向DisplayMenu Applet对象结构的指针,该结构包含小程序的特定信息.

返回值:
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注:

==============================================================================*/
NextFSMAction DisplayMenu_ProcessState(CDisplayMenu *pMe);

#endif
//DISPLAYMENUPRIV_H


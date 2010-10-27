/** ----------------------------------------------------------------------------
 * mainmenu_priv.h
 *
 * Copyright(c) 2007 Anylook
 *
 *
 * History:
 *
 * when            who               what and why
 * -----------  -----------    -----------------------------
 * 2008.12.24                created  for  Q1
 *
 * -----------------------------------------------------------------------------
 */

#ifndef MAINMENU_PRIV_H
#define MAINMENU_PRIV_H
#endif

#include "mainmenu.brh"
#include "AEEConfig.h"

#ifdef FEATURE_SUPPORT_WAP_APP
//#include "WapBrowser.bid"
#endif

#ifdef FEATRUE_SUPPORT_G_SENSOR
#include "G_Sensor.h"
#endif
#include "AEEAnnunciator.h"

#if defined (FEATURE_DISP_160X128)
#define  MAX_MATRIX_ITEMS 12
#define  MAX_MATRIX_ROWS 3
#define  MAX_MATRIX_COLS 4
#define MAX_TURN_NUM   12
#elif defined (FEATURE_DISP_220X176)
#define  MAX_MATRIX_ITEMS 12
#define  MAX_MATRIX_ROWS 3
#define  MAX_MATRIX_COLS 4
#define MAX_TURN_NUM   12
#elif defined (FEATURE_DISP_128X128)
#define  MAX_MATRIX_ITEMS 9
#define  MAX_MATRIX_ROWS 3
#define  MAX_MATRIX_COLS 3
#define MAX_TURN_NUM   9
#elif defined (FEATURE_DISP_128X160)
#define  MAX_MATRIX_ITEMS 12
#define  MAX_MATRIX_ROWS 4
#define  MAX_MATRIX_COLS 3
#define MAX_TURN_NUM   12
#elif defined (FEATURE_DISP_176X220)
#define  MAX_MATRIX_ITEMS 12
#define  MAX_MATRIX_ROWS 4
#define  MAX_MATRIX_COLS 3
#define MAX_TURN_NUM   12
#elif defined (FEATURE_DISP_240X320)
#define  MAX_MATRIX_ITEMS 12
#define  MAX_MATRIX_ROWS 4
#define  MAX_MATRIX_COLS 3
#define MAX_TURN_NUM   12
#elif defined (FEATURE_DISP_320X240)
#define  MAX_MATRIX_ITEMS 12
#define  MAX_MATRIX_ROWS 3
#define  MAX_MATRIX_COLS 4
#define MAX_TURN_NUM   12
#endif
#define FEATURE_ICON_MOVE_ANIMATION //added by chengxiao 2009.04.10

#if !defined(FEATURE_CARRIER_CHINA_VERTU)
#define FEATURE_FOCUS_ANIMATION
#endif


/*==============================================================================
                                 
                                相关数据类型定义
                                 
==============================================================================*/

typedef struct _MainMenuMod
{
    DECLARE_VTBL( IModule)

    uint32   referenceCounter;
    
} MainMenuMod;

//MAINMENU  下的几个状态
typedef enum MainmenuState
{
    MAINST_MAIN,

    MAINST_GAME,
    
    MAINST_PLAYER,

    MAINST_DATA, 

    #ifdef FEATRUE_SUPPORT_G_SENSOR
    MAINST_MSGBOX,
    #endif   

    MAINST_EXIT

} MainmenuState;

typedef enum
{
    MAINMENU_STOP=0x00,
    MAINMENU_RUNNING,
    MAINMENU_SUSPEND
} MAINMENU_STATUS_e_type;

typedef enum DLGRetValue
{
    DLGRET_CREATE,
        
    DLGRET_GAME,

    DLGRET_MEDIA,

    DLGRET_MAIN,

    DLGRET_DATA,

    #ifdef FEATRUE_SUPPORT_G_SENSOR
    DLGRET_MSGBOX,
    #endif

    DLGRET_CANCELED

}DLGRetValue;

typedef enum NextFSMAction
{
   NFSMACTION_WAIT,
   NFSMACTION_CONTINUE
   
} NextFSMAction;


typedef struct _Point
{
    int x;
    int y;
} Point;

typedef struct _MainMenu
{
    DECLARE_VTBL(IMainMenu)
    uint32       referenceCounter;
    
    IModule     *m_pModule;
    IDisplay    *m_pDisplay;
    IShell      *m_pShell;
    IConfig     *m_pConfig;

    IDialog     *m_pActiveIDlg;
    uint32       m_pActivedlgID;
    uint16       m_MainSel;   //一级菜单光标
    uint16       m_MenuSel;   //二级菜单光标
    DLGRetValue  m_eDlgReturn;
    
    int         m_nRow;
    int         m_nColumn;
#ifdef FEATURE_ICON_MOVE_ANIMATION
    int         m_nPrevRow;
    int         m_nPrevColumn;
#endif
#ifdef FEATURE_FOCUS_ANIMATION
    int         m_nIconAniFrameIdx;            // 图标动画当前帧索引
#endif

    MAINMENU_STATUS_e_type  m_eAppStatus;   // Applet 当前运行状态
    
    boolean     m_bDoNotOverwriteDlgResult; //是否不重写对话框结果    
    boolean     m_bNormalStart;
    
    IImage      *m_pImageBg;
    IImage      *m_pImageIcon[MAX_MATRIX_ITEMS];
    IImage      *m_pAnimate;
    Point       m_Icondefault_Pt[MAX_MATRIX_ITEMS];
    Point       m_IconFocus_Pt[MAX_MATRIX_ITEMS];
    AEERect     m_rc;
    
    MainmenuState             m_prevState;        // Applet前一状态
    MainmenuState             m_currState;        // Applet当前状态

    #ifdef FEATRUE_SUPPORT_G_SENSOR
    uint16           m_wMsgResID;
    #endif
    
#ifdef FEATURE_RANDOM_MENU_COLOR
    RGBVAL         m_nBgColor; //added by chengxiao 2009.02.23
    boolean         m_nRandomMenu; //added by chengxiao 2009.02.23
#endif
#ifdef FEATURE_ICON_MOVE_ANIMATION
    IBitmap       *m_pDevImage;
    boolean       m_bMoveing;
#endif

	IImage		*m_pImageTurn[MAX_TURN_NUM];//wlh 20090409 add
	IAnnunciator        *m_pIAnn;

} MainMenu;

/*==============================================================================
                                 
                                 宏定义和常数
                                 
==============================================================================*/
#if defined (FEATURE_DISP_160X128)
#define ICON_WIDTH              40
#define ICON_HEIGHT             32
#define ICON_ANIMATED_WIDTH     40
#define ICON_ANIMATED_HEIGHT    32
#elif defined (FEATURE_DISP_128X128)
#if defined FEATURE_VERSION_H19C  
#define ICON_WIDTH              26
#define ICON_HEIGHT             26
#define ICON_ANIMATED_WIDTH     26
#define ICON_ANIMATED_HEIGHT    26
#else
#define ICON_WIDTH              42
#define ICON_HEIGHT             32
#define ICON_ANIMATED_WIDTH     42
#define ICON_ANIMATED_HEIGHT    32
#endif
#elif defined (FEATURE_DISP_220X176)
#define ICON_WIDTH              40
#define ICON_HEIGHT             32
#define ICON_ANIMATED_WIDTH     40
#define ICON_ANIMATED_HEIGHT    32
#elif defined (FEATURE_DISP_128X160)
#define ICON_WIDTH              40
#define ICON_HEIGHT             32
#define ICON_ANIMATED_WIDTH     40
#define ICON_ANIMATED_HEIGHT    32
#elif defined (FEATURE_DISP_176X220)
#define ICON_WIDTH              40
#define ICON_HEIGHT             32
#define ICON_ANIMATED_WIDTH     40
#define ICON_ANIMATED_HEIGHT    32
#elif defined (FEATURE_DISP_240X320)
#define ICON_WIDTH              40
#define ICON_HEIGHT             32
#define ICON_ANIMATED_WIDTH     40
#define ICON_ANIMATED_HEIGHT    32
#elif defined (FEATURE_DISP_320X240)
#define ICON_WIDTH              40
#define ICON_HEIGHT             32
#define ICON_ANIMATED_WIDTH     40
#define ICON_ANIMATED_HEIGHT    32
#endif

#define ICON_ANIMATED_FRAME     1 //焦点图片的帧数
#ifdef FEATURE_ICON_MOVE_ANIMATION
#define ICON_ANIMATED_MOVE_FRAME  3
#define ICON_ANIMATED_MOVE_RATE    10
#endif

#define MAINMENU_RES_FILE_LANG        AEE_RES_LANGDIR MAINMENU_RES_FILE
#define MAINMENU_RES_FILE_IMAGE       MAINMENUIMAGES_RES_FILE

#define STARTARGPREFIX_VIEWPLAYER            'P'
#define STARTARGPREFIX_VIEWDATA              'D'

#if defined (FEATURE_DISP_160X128)

#ifdef FEATURE_VERSION_SMART
	#define ICON1_ANI      "fs:/image/mainmenu/World.png"
    #define ICON2_ANI      "fs:/image/mainmenu/Access.png"
    #define ICON3_ANI      "fs:/image/mainmenu/Display.png"
    #define ICON4_ANI      "fs:/image/mainmenu/FmRadio.png"
    #define ICON5_ANI      "fs:/image/mainmenu/Calendar.png"
    #define ICON6_ANI      "fs:/image/mainmenu/Contact.png"
    #define ICON7_ANI      "fs:/image/mainmenu/Messagebox.png"
    #define ICON8_ANI      "fs:/image/mainmenu/Timer.png"
    #define ICON9_ANI      "fs:/image/mainmenu/Recentcall.png"
    #define ICON10_ANI     "fs:/image/mainmenu/Document.png"
    #define ICON11_ANI     "fs:/image/mainmenu/Stopwatch.png"
    #define ICON12_ANI     "fs:/image/mainmenu/Setting.png"
#elif defined FEATURE_VERSION_M8
	#define ICON1_ANI      "fs:/image/mainmenu/B_live.png"
    #define ICON2_ANI      "fs:/image/mainmenu/Access.png"
    #define ICON3_ANI      "fs:/image/mainmenu/Display.png"
    #define ICON4_ANI      "fs:/image/mainmenu/FmRadio.png"
    #define ICON5_ANI      "fs:/image/mainmenu/Frenduo.png"
    #define ICON6_ANI      "fs:/image/mainmenu/Contact.png"
    #define ICON7_ANI      "fs:/image/mainmenu/Messagebox.png"
    #define ICON8_ANI      "fs:/image/mainmenu/Timer.png"
    #define ICON9_ANI      "fs:/image/mainmenu/Recentcall.png"
    #define ICON10_ANI     "fs:/image/mainmenu/Document.png"
    #define ICON11_ANI     "fs:/image/mainmenu/Stopwatch.png"
    #define ICON12_ANI     "fs:/image/mainmenu/Setting.png"
#else
	#define ICON1_ANI      "fs:/image/mainmenu/Document.png"
    #define ICON2_ANI      "fs:/image/mainmenu/Contact.png"
    #define ICON3_ANI      "fs:/image/mainmenu/Alarm.png"
    #define ICON4_ANI      "fs:/image/mainmenu/Digit.png"
    #define ICON5_ANI      "fs:/image/mainmenu/Recentcall.png"
    #define ICON6_ANI      "fs:/image/mainmenu/Messagebox.png"
    #define ICON7_ANI      "fs:/image/mainmenu/FmRadio.png"
    #define ICON8_ANI      "fs:/image/mainmenu/Stopwatch.png"
    #define ICON9_ANI      "fs:/image/mainmenu/Calendar.png"
    #define ICON10_ANI     "fs:/image/mainmenu/Setting.png"
    #define ICON11_ANI     "fs:/image/mainmenu/Timer.png"
    #define ICON12_ANI     "fs:/image/mainmenu/Display.png"
#endif
    
#endif
#if defined (FEATURE_DISP_128X128)
#if defined FEATURE_VERSION_H19C    
    #define ICON_ANI_BG    "fs:/image/mainmenu/Backgroud.png"
#else
    #define ICON1_ANI      "fs:/image/mainmenu/MediaGallery.png"
    #define ICON2_ANI      "fs:/image/mainmenu/Contact.png"
    #define ICON3_ANI      "fs:/image/mainmenu/UTK.png"
    #define ICON4_ANI      "fs:/image/mainmenu/FmRadio.png"
    //#define ICON5_ANI      "fs:/image/mainmenu/Message.png"//与第三方应用的图片名重名了
    #define ICON5_ANI      "fs:/image/mainmenu/Message_Mainmenu.png"
    #define ICON6_ANI      "fs:/image/mainmenu/Recentcall.png"
    #define ICON7_ANI      "fs:/image/mainmenu/MusicPlayer.png"
    #define ICON8_ANI      "fs:/image/mainmenu/Setting.png"
    #define ICON9_ANI      "fs:/image/mainmenu/application.png"
#endif
#endif

#if defined (FEATURE_DISP_220X176)	
    #define ICON1_ANI      "fs:/image/mainmenu/MediaGallery.png"
    #define ICON2_ANI      "fs:/image/mainmenu/Contact.png"
    #define ICON3_ANI      "fs:/image/mainmenu/UTK.png"
    #define ICON4_ANI      "fs:/image/mainmenu/FmRadio.png"  
    #define ICON5_ANI      "fs:/image/mainmenu/Message.png"
    #define ICON6_ANI      "fs:/image/mainmenu/Recentcall.png"
    #define ICON7_ANI      "fs:/image/mainmenu/MusicPlayer.png"
    #define ICON8_ANI      "fs:/image/mainmenu/Setting.png"
    #define ICON9_ANI      "fs:/image/mainmenu/application.png"
	#define ICON10_ANI     "fs:/image/mainmenu/Timer.png"
    #define ICON11_ANI     "fs:/image/mainmenu/Calendar.png"
    #define ICON12_ANI     "fs:/image/mainmenu/Digit.png"
#endif

#if defined (FEATURE_DISP_128X160)	
    #define ICON1_ANI      "fs:/image/mainmenu/MediaGallery.png"
    #define ICON2_ANI      "fs:/image/mainmenu/Contact.png"
    #define ICON3_ANI      "fs:/image/mainmenu/UTK.png"
    #define ICON4_ANI      "fs:/image/mainmenu/FmRadio.png"  
    #define ICON5_ANI      "fs:/image/mainmenu/Message.png"
    #define ICON6_ANI      "fs:/image/mainmenu/Recentcall.png"
    #define ICON7_ANI      "fs:/image/mainmenu/MusicPlayer.png"
    #define ICON8_ANI      "fs:/image/mainmenu/Setting.png"
    #define ICON9_ANI      "fs:/image/mainmenu/application.png"
	#define ICON10_ANI     "fs:/image/mainmenu/Timer.png"
    #define ICON11_ANI     "fs:/image/mainmenu/Calendar.png"
    #define ICON12_ANI     "fs:/image/mainmenu/Digit.png"
#endif

#if defined (FEATURE_DISP_176X220)
    #define ICON1_ANI      "fs:/image/mainmenu/MediaGallery.png"
    #define ICON2_ANI      "fs:/image/mainmenu/Contact.png"
    #define ICON3_ANI      "fs:/image/mainmenu/UTK.png"
    #define ICON4_ANI      "fs:/image/mainmenu/Digit.png"  
    #define ICON5_ANI      "fs:/image/mainmenu/Recentcall.png"
    #define ICON6_ANI      "fs:/image/mainmenu/Message.png"
    #define ICON7_ANI      "fs:/image/mainmenu/FmRadio.png"
    #define ICON8_ANI      "fs:/image/mainmenu/application.png"
    #define ICON9_ANI      "fs:/image/mainmenu/Calendar.png"
	#define ICON10_ANI     "fs:/image/mainmenu/Setting.png"
    #define ICON11_ANI     "fs:/image/mainmenu/Timer.png"
    #define ICON12_ANI     "fs:/image/mainmenu/MusicPlayer.png"
#endif

#if defined (FEATURE_DISP_240X320)
    #define ICON1_ANI      "fs:/image/mainmenu/MediaGallery.png"
    #define ICON2_ANI      "fs:/image/mainmenu/Contact.png"
    #define ICON3_ANI      "fs:/image/mainmenu/UTK.png"
    #define ICON4_ANI      "fs:/image/mainmenu/Digit.png"  
    #define ICON5_ANI      "fs:/image/mainmenu/Recentcall.png"
    #define ICON6_ANI      "fs:/image/mainmenu/Message.png"
    #define ICON7_ANI      "fs:/image/mainmenu/FmRadio.png"
    #define ICON8_ANI      "fs:/image/mainmenu/application.png"
    #define ICON9_ANI      "fs:/image/mainmenu/Calendar.png"
	#define ICON10_ANI     "fs:/image/mainmenu/Setting.png"
    #define ICON11_ANI     "fs:/image/mainmenu/Timer.png"
    #define ICON12_ANI     "fs:/image/mainmenu/MusicPlayer.png"
#endif

#if defined (FEATURE_DISP_320X240)
	#define ICON1_ANI      "fs:/image/mainmenu/MediaGallery.png"
    #define ICON2_ANI      "fs:/image/mainmenu/Contact.png"
    #define ICON3_ANI      "fs:/image/mainmenu/UTK.png"
    #define ICON4_ANI      "fs:/image/mainmenu/FmRadio.png"  
    #define ICON5_ANI      "fs:/image/mainmenu/Message.png"
    #define ICON6_ANI      "fs:/image/mainmenu/Recentcall.png"
    #define ICON7_ANI      "fs:/image/mainmenu/MusicPlayer.png"
    #define ICON8_ANI      "fs:/image/mainmenu/Setting.png"
    #define ICON9_ANI      "fs:/image/mainmenu/application.png"
	#define ICON10_ANI     "fs:/image/mainmenu/Timer.png"
    #define ICON11_ANI     "fs:/image/mainmenu/Calendar.png"
    #define ICON12_ANI     "fs:/image/mainmenu/Digit.png"
#endif



#define ICON_ANI_BACKGROUND    "fs:/image/mainmenu/Backgroud.png"
#if defined (FEATURE_DISP_160X128)

#ifdef FEATURE_VERSION_SMART
	#define ICON1_ANI_1      "fs:/image/mainmenu/World_focus.png"
    #define ICON2_ANI_1      "fs:/image/mainmenu/Access_focus.png"
    #define ICON3_ANI_1      "fs:/image/mainmenu/Display_focus.png"
    #define ICON4_ANI_1      "fs:/image/mainmenu/FmRadio_focus.png"
    #define ICON5_ANI_1      "fs:/image/mainmenu/Calendar_focus.png"
    #define ICON6_ANI_1      "fs:/image/mainmenu/Contact_focus.png"
    #define ICON7_ANI_1      "fs:/image/mainmenu/Message_focus.png"
    #define ICON8_ANI_1      "fs:/image/mainmenu/Timer_focus.png"
    #define ICON9_ANI_1      "fs:/image/mainmenu/Recentcall_focus.png"
    #define ICON10_ANI_1     "fs:/image/mainmenu/Document_focus.png"
    #define ICON11_ANI_1     "fs:/image/mainmenu/Stopwatch_focus.png"
    #define ICON12_ANI_1     "fs:/image/mainmenu/Setting_focus.png"
#elif defined FEATURE_VERSION_M8
	#define ICON1_ANI_1      "fs:/image/mainmenu/B_live_focus.png"
    #define ICON2_ANI_1      "fs:/image/mainmenu/Access_focus.png"
    #define ICON3_ANI_1      "fs:/image/mainmenu/Display_focus.png"
    #define ICON4_ANI_1      "fs:/image/mainmenu/FmRadio_focus.png"
    #define ICON5_ANI_1      "fs:/image/mainmenu/Frenduo_focus.png"
    #define ICON6_ANI_1      "fs:/image/mainmenu/Contact_focus.png"
    #define ICON7_ANI_1      "fs:/image/mainmenu/Message_focus.png"
    #define ICON8_ANI_1      "fs:/image/mainmenu/Timer_focus.png"
    #define ICON9_ANI_1      "fs:/image/mainmenu/Recentcall_focus.png"
    #define ICON10_ANI_1     "fs:/image/mainmenu/Document_focus.png"
    #define ICON11_ANI_1     "fs:/image/mainmenu/Stopwatch_focus.png"
    #define ICON12_ANI_1     "fs:/image/mainmenu/Setting_focus.png"
#else
	#define ICON1_ANI_1      "fs:/image/mainmenu/Document_focus.png"
    #define ICON2_ANI_1      "fs:/image/mainmenu/Contact_focus.png"
    #define ICON3_ANI_1      "fs:/image/mainmenu/Alarm_focus.png"
    #define ICON4_ANI_1      "fs:/image/mainmenu/Digit_focus.png"
    #define ICON5_ANI_1      "fs:/image/mainmenu/Recentcall_focus.png"
    #define ICON6_ANI_1      "fs:/image/mainmenu/Message_focus.png"
    #define ICON7_ANI_1      "fs:/image/mainmenu/FmRadio_focus.png"
    #define ICON8_ANI_1      "fs:/image/mainmenu/Stopwatch_focus.png"
    #define ICON9_ANI_1      "fs:/image/mainmenu/Calendar_focus.png"
    #define ICON10_ANI_1     "fs:/image/mainmenu/Setting_focus.png"
    #define ICON11_ANI_1     "fs:/image/mainmenu/Timer_focus.png"
    #define ICON12_ANI_1     "fs:/image/mainmenu/Display_focus.png"
#endif
    
#endif
#if defined (FEATURE_DISP_128X128)
#if defined FEATURE_VERSION_H19C
    #define ICON1_ANI_1    "fs:/image/mainmenu/Recent_calls.gif"
    #define ICON2_ANI_1    "fs:/image/mainmenu/Multimedia.gif"
    #define ICON3_ANI_1    "fs:/image/mainmenu/contacts.gif"
    #define ICON4_ANI_1    "fs:/image/mainmenu/User_Profile.gif"
    #define ICON5_ANI_1    "fs:/image/mainmenu/Messages.gif"
    #define ICON6_ANI_1    "fs:/image/mainmenu/Tools.gif"
    #define ICON7_ANI_1    "fs:/image/mainmenu/Setting.gif"
    #define ICON8_ANI_1    "fs:/image/mainmenu/Game.gif"
    #define ICON9_ANI_1    "fs:/image/mainmenu/Net_Serve.gif"
#else
    #define ICON1_ANI_1    "fs:/image/mainmenu/MediaGallery_focus.png"
    #define ICON2_ANI_1    "fs:/image/mainmenu/Contact_focus.png"
    #define ICON3_ANI_1    "fs:/image/mainmenu/UTK_focus.png"
    #define ICON4_ANI_1    "fs:/image/mainmenu/FmRadio_focus.png"
    #define ICON5_ANI_1    "fs:/image/mainmenu/Message_focus.png"
    #define ICON6_ANI_1    "fs:/image/mainmenu/Recentcall_focus.png"
    #define ICON7_ANI_1    "fs:/image/mainmenu/MusicPlayer_focus.png"
    #define ICON8_ANI_1    "fs:/image/mainmenu/Setting_focus.png"
    #define ICON9_ANI_1    "fs:/image/mainmenu/application_focus.png"
#endif
#endif

#if defined (FEATURE_DISP_220X176)
	#define ICON1_ANI_1      "fs:/image/mainmenu/MediaGallery_focus.png"
    #define ICON2_ANI_1      "fs:/image/mainmenu/Contact_focus.png"
    #define ICON3_ANI_1      "fs:/image/mainmenu/UTK_focus.png"
    #define ICON4_ANI_1      "fs:/image/mainmenu/FmRadio_focus.png"
    #define ICON5_ANI_1      "fs:/image/mainmenu/Message_focus.png"
    #define ICON6_ANI_1      "fs:/image/mainmenu/Recentcall_focus.png"
    #define ICON7_ANI_1      "fs:/image/mainmenu/MusicPlayer_focus.png"
    #define ICON8_ANI_1      "fs:/image/mainmenu/Setting_focus.png"
    #define ICON9_ANI_1      "fs:/image/mainmenu/application_focus.png"
    #define ICON10_ANI_1     "fs:/image/mainmenu/Timer_focus.png"
    #define ICON11_ANI_1     "fs:/image/mainmenu/Calendar_focus.png"
    #define ICON12_ANI_1     "fs:/image/mainmenu/Digit_focus.png"    
#endif


#if defined (FEATURE_DISP_128X160)
	#define ICON1_ANI_1      "fs:/image/mainmenu/MediaGallery_focus.png"
    #define ICON2_ANI_1      "fs:/image/mainmenu/Contact_focus.png"
    #define ICON3_ANI_1      "fs:/image/mainmenu/UTK_focus.png"
    #define ICON4_ANI_1      "fs:/image/mainmenu/FmRadio_focus.png"
    #define ICON5_ANI_1      "fs:/image/mainmenu/Message_focus.png"
    #define ICON6_ANI_1      "fs:/image/mainmenu/Recentcall_focus.png"
    #define ICON7_ANI_1      "fs:/image/mainmenu/MusicPlayer_focus.png"
    #define ICON8_ANI_1      "fs:/image/mainmenu/Setting_focus.png"
    #define ICON9_ANI_1      "fs:/image/mainmenu/application_focus.png"
    #define ICON10_ANI_1     "fs:/image/mainmenu/Timer_focus.png"
    #define ICON11_ANI_1     "fs:/image/mainmenu/Calendar_focus.png"
    #define ICON12_ANI_1     "fs:/image/mainmenu/Digit_focus.png"    
#endif

#if defined (FEATURE_DISP_176X220)
    #define ICON1_ANI_1      "fs:/image/mainmenu/MediaGallery_focus.png"
    #define ICON2_ANI_1      "fs:/image/mainmenu/Contact_focus.png"
    #define ICON3_ANI_1      "fs:/image/mainmenu/UTK_focus.png"
    #define ICON4_ANI_1      "fs:/image/mainmenu/Digit_focus.png"
    #define ICON5_ANI_1      "fs:/image/mainmenu/Recentcall_focus.png"
    #define ICON6_ANI_1      "fs:/image/mainmenu/Message_focus.png"
    #define ICON7_ANI_1      "fs:/image/mainmenu/FmRadio_focus.png"
    #define ICON8_ANI_1      "fs:/image/mainmenu/application_focus.png"
    #define ICON9_ANI_1      "fs:/image/mainmenu/Calendar_focus.png"
    #define ICON10_ANI_1     "fs:/image/mainmenu/Setting_focus.png"
    #define ICON11_ANI_1     "fs:/image/mainmenu/Timer_focus.png"
    #define ICON12_ANI_1     "fs:/image/mainmenu/MusicPlayer_focus.png" 
#endif

#if defined (FEATURE_DISP_240X320)
    #define ICON1_ANI_1      "fs:/image/mainmenu/MediaGallery_focus.png"
    #define ICON2_ANI_1      "fs:/image/mainmenu/Contact_focus.png"
    #define ICON3_ANI_1      "fs:/image/mainmenu/UTK_focus.png"
    #define ICON4_ANI_1      "fs:/image/mainmenu/Digit_focus.png"
    #define ICON5_ANI_1      "fs:/image/mainmenu/Recentcall_focus.png"
    #define ICON6_ANI_1      "fs:/image/mainmenu/Message_focus.png"
    #define ICON7_ANI_1      "fs:/image/mainmenu/FmRadio_focus.png"
    #define ICON8_ANI_1      "fs:/image/mainmenu/application_focus.png"
    #define ICON9_ANI_1      "fs:/image/mainmenu/Calendar_focus.png"
    #define ICON10_ANI_1     "fs:/image/mainmenu/Setting_focus.png"
    #define ICON11_ANI_1     "fs:/image/mainmenu/Timer_focus.png"
    #define ICON12_ANI_1     "fs:/image/mainmenu/MusicPlayer_focus.png" 
#endif

#if defined (FEATURE_DISP_320X240)
    #define ICON1_ANI_1      "fs:/image/mainmenu/MediaGallery_focus.png"
    #define ICON2_ANI_1      "fs:/image/mainmenu/Contact_focus.png"
    #define ICON3_ANI_1      "fs:/image/mainmenu/UTK_focus.png"
    #define ICON4_ANI_1      "fs:/image/mainmenu/FmRadio_focus.png"
    #define ICON5_ANI_1      "fs:/image/mainmenu/Message_focus.png"
    #define ICON6_ANI_1      "fs:/image/mainmenu/Recentcall_focus.png"
    #define ICON7_ANI_1      "fs:/image/mainmenu/MusicPlayer_focus.png"
    #define ICON8_ANI_1      "fs:/image/mainmenu/Setting_focus.png"
    #define ICON9_ANI_1      "fs:/image/mainmenu/application_focus.png"
    #define ICON10_ANI_1     "fs:/image/mainmenu/Timer_focus.png"
    #define ICON11_ANI_1     "fs:/image/mainmenu/Calendar_focus.png"
    #define ICON12_ANI_1     "fs:/image/mainmenu/Digit_focus.png"    
#endif





#define ICON_ANI_FOCUSDEFAULT    "fs:/image/mainmenu/Focus_default.png"



// 此宏用当前状态更新先前状态，再用nextState状态更新当前状态
#define MOVE_TO_STATE(nextState)            \
{                                           \
    MainmenuState tpState;                    \
    tpState = nextState;                    \
    pMe->m_prevState = pMe->m_currState;    \
    pMe->m_currState = tpState;             \
}

//关闭对话框的宏
#define CLOSE_DIALOG(dlgResult)             \
{                                           \
    pMe->m_eDlgReturn = dlgResult;          \
    (void) ISHELL_EndDialog(pMe->m_pShell); \
}

/*==============================================================================

                                 函数声明

==============================================================================*/

static int MainMenuMod_CreateInstance( IModule *po,
    IShell      *pIShell,
    AEECLSID    ClsId,
    void        **ppObj
);

static uint32 MainMenuMod_AddRef( IModule *po);

static uint32 MainMenuMod_Release( IModule *po);

static void MainMenuMod_FreeResources( IModule *po, IHeap *ph, IFileMgr *pfm);


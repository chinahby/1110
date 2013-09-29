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
#include "AEEAnnunciator.h"
#include "AEEBacklight.h"
#include "OEMSound.h"      // TBD - hack until available somewhere else
#include "AEEMedia.h"

#if defined (FEATURE_DISP_160X128)
#ifdef FEATURE_VERSION_K212_ND
#if 0
#define  MAX_MATRIX_ITEMS 7 
#else
#define  MAX_MATRIX_ITEMS 8
#endif
#else
#define  MAX_MATRIX_ITEMS 12
#define  MAX_MATRIX_ROWS 3
#define  MAX_MATRIX_COLS 4
#endif
#elif defined (FEATURE_DISP_220X176)
#ifdef FEATURE_VERSION_EC99
#if  0     //FEATURE_CTA
#define  MAX_MATRIX_ITEMS 7 
#else
#define  MAX_MATRIX_ITEMS 8
#endif
#elif defined(FEATURE_VERSION_K212_20D)
#define MAX_MATRIX_ITEMS 7
#else
#define  MAX_MATRIX_ITEMS 12
#define  MAX_MATRIX_ROWS 3
#define  MAX_MATRIX_COLS 4
#endif
#elif defined (FEATURE_DISP_128X128)
#define  MAX_MATRIX_ITEMS 9
#define  MAX_MATRIX_ROWS 3
#define  MAX_MATRIX_COLS 3
#elif defined (FEATURE_DISP_128X160)
#if defined(FEATURE_VERSION_C01) ||defined(FEATURE_VERSION_C316) || defined(FEATURE_VERSION_W450_JFY)
#define  MAX_MATRIX_ITEMS 9
#define  MAX_MATRIX_ROWS 3
#define  MAX_MATRIX_COLS 3
#else
#define  MAX_MATRIX_ITEMS 12
#define  MAX_MATRIX_ROWS 4
#define  MAX_MATRIX_COLS 3
#endif
#elif defined (FEATURE_DISP_176X220)
#if defined(FEATURE_VERSION_VG68) || defined(FEATURE_VERSION_SKY)||defined(FEATURE_VERSION_K202_LM129C)
#define  MAX_MATRIX_ITEMS 9
#define  MAX_MATRIX_ROWS 3
#define  MAX_MATRIX_COLS 3
#else
#define  MAX_MATRIX_ITEMS 12
#define  MAX_MATRIX_ROWS 4
#define  MAX_MATRIX_COLS 3
#endif
#elif defined (FEATURE_DISP_240X320)
#if defined(FEATURE_VERSION_K212)
#define  MAX_MATRIX_ITEMS 12
#else
#define  MAX_MATRIX_ITEMS 12
#define  MAX_MATRIX_ROWS  4
#define  MAX_MATRIX_COLS  3
#define  MAX_MATRIX_PAGE  2
#define  SELECT_ONE_X     100
#define  SELECT_TWO_X     140
#define  SELECT_THR_X     155
#define  SELECT_Y         20
#define  DX_MAIN_MOVE     10
#define  MAIN_ICON_W      60
#define  MAIN_ICON_H      60
#define  MAX_BOTTOM_ITEMS 4
#define  MAX_TITLEID_NUM  30
#define  BASE_BOTTON_TITLE 26
#define  BASE_BOTTOM_ICON_W 45
#define  BASE_BOTTOM_ICON_H 45
#define  BASE_BOTTOM_SPC    12
#endif
#elif defined (FEATURE_DISP_320X240)
#define  MAX_MATRIX_ITEMS 12
#define  MAX_MATRIX_ROWS 3
#define  MAX_MATRIX_COLS 4
#endif
#define MAINMENU_PT_IN_RECT(a,b,rct)      (boolean)( ((a) >= (rct).x && (a) <= ((rct).x + (rct).dx)) && ((b) >= (rct).y && (b) <= ((rct).y + (rct).dy)) )

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
    MAINST_FLASHLIGHT,  
    MAINST_VIEW_TIME,
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
    DLGRET_FLASHLITHT,      //Add By zzg 2013_08_30 for Torch
    DLGRET_VIEW_TIEM,
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
    uint32          referenceCounter;
    
    IModule        *m_pModule;
    IDisplay       *m_pDisplay;
    IShell         *m_pShell;
    IConfig        *m_pConfig;

    IDialog        *m_pActiveIDlg;
    uint32          m_pActivedlgID;
    DLGRetValue     m_eDlgReturn;
    
    int             m_nRow;
    int             m_nColumn;
#ifdef FEATURE_VERSION_K212_ND
    int             m_index;
#endif
#if defined(FEATURE_VERSION_EC99) ||defined(FEATURE_VERSION_K212_20D)
    int             m_index;
#endif
    
    MAINMENU_STATUS_e_type  m_eAppStatus;   // Applet 当前运行状态
    
    boolean         m_bDoNotOverwriteDlgResult; //是否不重写对话框结果    
    #ifdef FEATURE_LCD_TOUCH_ENABLE
	IImage         *m_pImageBg[MAX_MATRIX_PAGE];
	IImage         *m_pImageBgblack;
	int             m_step;
	boolean         m_bRight;
	boolean         m_bReraw;
	#else
    IImage         *m_pImageBg;
    IImage         *m_pImageIcon[MAX_MATRIX_ITEMS];
	#endif
	uint16          m_PrsentPage;           
    IImage         *m_pAnimate;
    Point           m_Icondefault_Pt[MAX_MATRIX_ITEMS];
    Point           m_IconFocus_Pt[MAX_MATRIX_ITEMS];
	#ifdef FEATURE_LCD_TOUCH_ENABLE
    uint16          m_IconTitle[MAX_TITLEID_NUM];
	#else
	uint16          m_IconTitle[MAX_MATRIX_ITEMS];
	#endif
    AEERect         m_rc;
    
    MainmenuState   m_prevState;        // Applet前一状态
    MainmenuState   m_currState;        // Applet当前状态
    
#ifdef FEATURE_RANDOM_MENU_COLOR
    RGBVAL          m_nBgColor;
    boolean         m_nRandomMenu;
#endif
	IAnnunciator   *m_pIAnn;
    IBacklight     *m_pBacklight;
#ifdef FEATURE_LCD_TOUCH_ENABLE
	IImage          *m_pImageSelect;
	IImage          *m_pImageSelect_foucs;
	//IImage          *m_pImageButtom[MAX_BOTTOM_ITEMS];
	uint16          m_nCurPage;
	//Point           m_IconSelect_Pt[3];
	//Point           m_IconButtom_pt[MAX_BOTTOM_ITEMS];
	Point           m_Primove_Pt;
	Point           m_Pdown_Pt;
	uint16          m_PenPos;
	boolean         m_bmove;
#endif
#ifdef FEATURE_VERSION_K212
	IImage          *m_pImageSelectk212[9];
	IImage          *m_pImageSelectkbar;
	nv_language_enum_type language;
	IMedia *              m_pMedia;
	boolean   	    m_bChangeL;
#endif
#ifdef FEATURE_VERSION_K212_ND
	IImage          *m_pImageSelectK212ND;
	nv_language_enum_type language;
	IMedia *              m_pMedia;
	boolean   	    m_bChangeL; 
#endif
#if defined(FEATURE_VERSION_EC99) ||defined(FEATURE_VERSION_K212_20D) 
    //IImage          *m_pImageSelectEC99[8];
    IImage          *m_pImageSelectEC99;
	nv_language_enum_type language;
	IMedia *              m_pMedia;
	boolean   	    m_bChangeL;
#endif
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
#define ICON_WIDTH              50 //40
#define ICON_HEIGHT             40 //32
#define ICON_ANIMATED_WIDTH     50 //40
#define ICON_ANIMATED_HEIGHT    40 //32
#elif defined (FEATURE_DISP_128X160)
#if defined FEATURE_VERSION_C01
#define ICON_WIDTH              41
#define ICON_HEIGHT             41
#define ICON_ANIMATED_WIDTH     41
#define ICON_ANIMATED_HEIGHT    41
#elif defined(FEATURE_VERSION_C316)  // add by pyuangui 20121221
#define ICON_WIDTH              40
#define ICON_HEIGHT             40
#define ICON_ANIMATED_WIDTH     40
#define ICON_ANIMATED_HEIGHT    40
#else
#define ICON_WIDTH              40
#define ICON_HEIGHT             32
#define ICON_ANIMATED_WIDTH     40
#define ICON_ANIMATED_HEIGHT    32
#endif
#elif defined (FEATURE_DISP_176X220)
#ifdef FEATURE_VERSION_K202_LM129C //xxzhen
#define ICON_WIDTH              50 //40
#define ICON_HEIGHT             50 //32
#define ICON_ANIMATED_WIDTH     50 //40
#define ICON_ANIMATED_HEIGHT    50 //32
#else
#define ICON_WIDTH              50 //40
#define ICON_HEIGHT             40 //32
#define ICON_ANIMATED_WIDTH     50 //40
#define ICON_ANIMATED_HEIGHT    40 //32
#endif
#elif defined (FEATURE_DISP_240X320)
#define ICON_WIDTH              70
#define ICON_HEIGHT             60
#define ICON_ANIMATED_WIDTH     70
#define ICON_ANIMATED_HEIGHT    60
#define BOTTOM_ICON_WIDTH       45
#define BOTTOM_ICON_HEIGHT      45
#define BOTTOM_MID_SPACE        11
#elif defined (FEATURE_DISP_320X240)
#define ICON_WIDTH              80
#define ICON_HEIGHT             55
#define ICON_ANIMATED_WIDTH     80
#define ICON_ANIMATED_HEIGHT    55

#endif

#define MAINMENU_RES_FILE_LANG        AEE_RES_LANGDIR MAINMENU_RES_FILE
#define MAINMENU_RES_FILE_IMAGE       MAINMENUIMAGES_RES_FILE

#define STARTARGPREFIX_VIEWPLAYER            'P'
#define STARTARGPREFIX_VIEWDATA              'D'

#ifndef FEATURE_DISP_128X128
#ifdef FEATURE_VERSION_C01   
    #define ICON_ANI_BG    "fs:/image/mainmenu/Backgroud.png"
#elif defined (FEATURE_VERSION_SKY) 
    #define ICON_ANI_BG    "fs:/image/mainmenu/Messages.gif"
#elif defined (FEATURE_VERSION_VG68)
	#define ICON1_ANI      "fs:/image/mainmenu/qsc1100_06.png"
    #define ICON2_ANI      "fs:/image/mainmenu/qsc1100_04.png"
    #define ICON3_ANI      "fs:/image/mainmenu/qsc1100_02.png"
    #define ICON4_ANI      "fs:/image/mainmenu/qsc1100_09.png"
    #define ICON5_ANI      "fs:/image/mainmenu/qsc1100_05.png"
    #define ICON6_ANI      "fs:/image/mainmenu/qsc1100_08.png"
    #define ICON7_ANI      "fs:/image/mainmenu/qsc1100_10.png"
    #define ICON8_ANI      "fs:/image/mainmenu/qsc1100_11.png"
	#define ICON9_ANI      "fs:/image/mainmenu/qsc1100_03.png"
#elif defined (FEATURE_VERSION_K202_LM129C) //xxzhen
	#define ICON1_ANI      "fs:/image/mainmenu/qsc1100_01.png"
    #define ICON2_ANI      "fs:/image/mainmenu/qsc1100_02.png"
    #define ICON3_ANI      "fs:/image/mainmenu/qsc1100_03.png"
    #define ICON4_ANI      "fs:/image/mainmenu/qsc1100_04.png"
    #define ICON5_ANI      "fs:/image/mainmenu/qsc1100_05.png"
    #define ICON6_ANI      "fs:/image/mainmenu/qsc1100_06.png"
    #define ICON7_ANI      "fs:/image/mainmenu/qsc1100_07.png"
    #define ICON8_ANI      "fs:/image/mainmenu/qsc1100_08.png"
    #define ICON9_ANI      "fs:/image/mainmenu/qsc1100_09.png"
// add by pyuangui	
#elif defined(FEATURE_VERSION_C316)
    #define ICON1_ANI      "fs:/image/mainmenu/phonebook.png"
    #define ICON2_ANI      "fs:/image/mainmenu/message.png"
    #define ICON3_ANI      "fs:/image/mainmenu/Call_history.png"
    #define ICON4_ANI      "fs:/image/mainmenu/Setting.png"
    #define ICON5_ANI      "fs:/image/mainmenu/multimedia.png"
    #define ICON6_ANI      "fs:/image/mainmenu/file_manger.png"
    #define ICON7_ANI      "fs:/image/mainmenu/organizer.png"
    #define ICON8_ANI      "fs:/image/mainmenu/Intex_zone.png"
	#define ICON9_ANI      "fs:/image/mainmenu/Browser.png"
// add end	
#elif defined(FEATURE_VERSION_K212)
	#define ICON_ANI_BG    "fs:/image/mainmenu/BG.png"
	#define ICON_ANI_BAR   "fs:/image/mainmenu/BAR.png"
	#define ICON1_ANI      "fs:/image/mainmenu/ICON_Camera.png"
    #define ICON2_ANI      "fs:/image/mainmenu/ICON_Contacts.png"
    #define ICON3_ANI      "fs:/image/mainmenu/ICON_Management.png"
    #define ICON4_ANI      "fs:/image/mainmenu/ICON_Messages.png"
    #define ICON5_ANI      "fs:/image/mainmenu/ICON_Multimedia.png"
    #define ICON6_ANI      "fs:/image/mainmenu/ICON_Profile.png"
    #define ICON7_ANI      "fs:/image/mainmenu/ICON_Recentcalls.png"
    #define ICON8_ANI      "fs:/image/mainmenu/ICON_Settings.png"
	#define ICON9_ANI      "fs:/image/mainmenu/ICON_Surface.png"

	#define ICON1_ANI_EN      "fs:/image/mainmenu/ICON_Camera_en.png"
    #define ICON2_ANI_EN      "fs:/image/mainmenu/ICON_Contacts_en.png"
    #define ICON3_ANI_EN      "fs:/image/mainmenu/ICON_Management_en.png"
    #define ICON4_ANI_EN      "fs:/image/mainmenu/ICON_Messages_en.png"
    #define ICON5_ANI_EN      "fs:/image/mainmenu/ICON_Multimedia_en.png"
    #define ICON6_ANI_EN      "fs:/image/mainmenu/ICON_Profile_en.png"
    #define ICON7_ANI_EN      "fs:/image/mainmenu/ICON_Recentcalls_en.png"
    #define ICON8_ANI_EN      "fs:/image/mainmenu/ICON_Settings_en.png"
	#define ICON9_ANI_EN      "fs:/image/mainmenu/ICON_Surface_en.png"
	
#else

#ifdef FEATURE_VERSION_EC99      
	#define ICON1_ANI      "fs:/image/mainmenu/Icon_calllog.png"
    #define ICON2_ANI      "fs:/image/mainmenu/Icon_profile.png"
    #define ICON3_ANI      "fs:/image/mainmenu/Icon_multimedia.png"
    #define ICON4_ANI      "fs:/image/mainmenu/Icon_phonebook.png"
    #define ICON5_ANI      "fs:/image/mainmenu/Icon_settings.png"
    #define ICON6_ANI      "fs:/image/mainmenu/Icon_tianyi.png"
    #define ICON7_ANI      "fs:/image/mainmenu/Icon_tools.png"
    #define ICON8_ANI      "fs:/image/mainmenu/Icon_wms.png"		
#elif defined(FEATURE_VERSION_K212_ND)

	#define ICON1_ANI     "fs:/image/mainmenu/Icon_wms.png"// "fs:/image/mainmenu/Icon_calllog.png"
    #define ICON2_ANI     "fs:/image/mainmenu/Icon_phonebook.png"// "fs:/image/mainmenu/Icon_profile.png"
    #define ICON3_ANI     "fs:/image/mainmenu/Icon_calllog.png"// "fs:/image/mainmenu/Icon_multimedia.png"
    #define ICON4_ANI     "fs:/image/mainmenu/Icon_settings.png"// "fs:/image/mainmenu/Icon_phonebook.png"
    #define ICON5_ANI     "fs:/image/mainmenu/Icon_multimedia.png"// "fs:/image/mainmenu/Icon_settings.png"
    #define ICON6_ANI     "fs:/image/mainmenu/Icon_tools.png"//"fs:/image/mainmenu/Icon_tianyi.png"
    #define ICON7_ANI     "fs:/image/mainmenu/Icon_profile.png"//"fs:/image/mainmenu/Icon_tools.png"
    #define ICON8_ANI     "fs:/image/mainmenu/Icon_tianyi.png"//"fs:/image/mainmenu/Icon_wms.png"	
#elif defined(FEATURE_VERSION_K212_20D)
    #define ICON1_ANI      "fs:/image/mainmenu/Icon_calllog.png"
    #define ICON2_ANI      "fs:/image/mainmenu/Icon_multimedia.png"
    #define ICON3_ANI      "fs:/image/mainmenu/Icon_phonebook.png"
    #define ICON4_ANI      "fs:/image/mainmenu/Icon_settings.png"
	#define ICON5_ANI      "fs:/image/mainmenu/Icon_tianyi.png"
    #define ICON6_ANI      "fs:/image/mainmenu/Icon_tools.png"
    #define ICON7_ANI      "fs:/image/mainmenu/Icon_wms.png"
#else  
	
	#ifdef FEATURE_LCD_TOUCH_ENABLE
    #define ICONFOUCS_ANI      "fs:/image/mainmenu/foucs.png" 
	#define ICONBACK1_ANI      "fs:/image/mainmenu/backgroud.png"
	#else
	
	#define ICON1_ANI      "fs:/image/mainmenu/qsc1100_01.png"
    #define ICON2_ANI      "fs:/image/mainmenu/qsc1100_02.png"
    #define ICON3_ANI      "fs:/image/mainmenu/qsc1100_03.png"
    #define ICON4_ANI      "fs:/image/mainmenu/qsc1100_04.png"
    #define ICON5_ANI      "fs:/image/mainmenu/qsc1100_05.png"
    #define ICON6_ANI      "fs:/image/mainmenu/qsc1100_06.png"
    #define ICON7_ANI      "fs:/image/mainmenu/qsc1100_07.png"
    #define ICON8_ANI      "fs:/image/mainmenu/qsc1100_08.png"
    #define ICON9_ANI      "fs:/image/mainmenu/qsc1100_09.png"
    #define ICON10_ANI     "fs:/image/mainmenu/qsc1100_10.png"
    #define ICON11_ANI     "fs:/image/mainmenu/qsc1100_11.png"
    #define ICON12_ANI     "fs:/image/mainmenu/qsc1100_12.png"	
	#endif
#endif    
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
    #define ICON5_ANI      "fs:/image/mainmenu/Message_Mainmenu.png"
    #define ICON6_ANI      "fs:/image/mainmenu/Recentcall.png"
    #define ICON7_ANI      "fs:/image/mainmenu/MusicPlayer.png"
    #define ICON8_ANI      "fs:/image/mainmenu/Setting.png"
    #define ICON9_ANI      "fs:/image/mainmenu/application.png"
#endif
#endif



#ifndef FEATURE_DISP_128X128
#ifdef FEATURE_VERSION_C01
    #define ICON1_ANI_1    "fs:/image/mainmenu/Recent_calls.gif"
    #define ICON2_ANI_1    "fs:/image/mainmenu/Multimedia.gif"
    #define ICON3_ANI_1    "fs:/image/mainmenu/contacts.gif"
    #define ICON4_ANI_1    "fs:/image/mainmenu/User_Profile.gif"
    #define ICON5_ANI_1    "fs:/image/mainmenu/Messages.gif"
    #define ICON6_ANI_1    "fs:/image/mainmenu/Tools.gif"
    #define ICON7_ANI_1    "fs:/image/mainmenu/Setting.gif"         
    #define ICON8_ANI_1    "fs:/image/mainmenu/Game.gif"
    #define ICON9_ANI_1    "fs:/image/mainmenu/Net_Serve.gif"
#elif defined (FEATURE_VERSION_VG68)
	#define ICON1_ANI_1      "fs:/image/mainmenu/qsc1100_06_focus.png"
    #define ICON2_ANI_1      "fs:/image/mainmenu/qsc1100_04_focus.png"
    #define ICON3_ANI_1      "fs:/image/mainmenu/qsc1100_02_focus.png"
    #define ICON4_ANI_1      "fs:/image/mainmenu/qsc1100_09_focus.png"
    #define ICON5_ANI_1      "fs:/image/mainmenu/qsc1100_05_focus.png"
    #define ICON6_ANI_1      "fs:/image/mainmenu/qsc1100_08_focus.png"
    #define ICON7_ANI_1      "fs:/image/mainmenu/qsc1100_10_focus.png"
    #define ICON8_ANI_1      "fs:/image/mainmenu/qsc1100_11_focus.png"
	#define ICON9_ANI_1      "fs:/image/mainmenu/qsc1100_03_focus.png"
#elif defined (FEATURE_VERSION_K202_LM129C) //xxzhen
	#define ICON1_ANI_1      "fs:/image/mainmenu/qsc1100_01_focus.png"
    #define ICON2_ANI_1      "fs:/image/mainmenu/qsc1100_02_focus.png"
    #define ICON3_ANI_1      "fs:/image/mainmenu/qsc1100_03_focus.png"
    #define ICON4_ANI_1      "fs:/image/mainmenu/qsc1100_04_focus.png"
    #define ICON5_ANI_1      "fs:/image/mainmenu/qsc1100_05_focus.png"
    #define ICON6_ANI_1      "fs:/image/mainmenu/qsc1100_06_focus.png"
    #define ICON7_ANI_1      "fs:/image/mainmenu/qsc1100_07_focus.png"
    #define ICON8_ANI_1      "fs:/image/mainmenu/qsc1100_08_focus.png"
    #define ICON9_ANI_1      "fs:/image/mainmenu/qsc1100_09_focus.png"
// add by pyuangui	
#elif defined(FEATURE_VERSION_C316)
    #define ICON1_ANI_1      "fs:/image/mainmenu/phonebook_focus.png"
    #define ICON2_ANI_1      "fs:/image/mainmenu/message_focus.png"
    #define ICON3_ANI_1      "fs:/image/mainmenu/Call_history_focus.png"
    #define ICON4_ANI_1      "fs:/image/mainmenu/Setting_focus.png"
    #define ICON5_ANI_1      "fs:/image/mainmenu/multimedia_focus.png"
    #define ICON6_ANI_1      "fs:/image/mainmenu/file_manger_focus.png"
    #define ICON7_ANI_1      "fs:/image/mainmenu/organizer_focus.png"
    #define ICON8_ANI_1      "fs:/image/mainmenu/Intex_zone_focus.png"
	#define ICON9_ANI_1      "fs:/image/mainmenu/Browser_focus.png"
// add end	
#elif defined (FEATURE_VERSION_SKY)
    #define ICON1_ANI_1    "fs:/image/mainmenu/Messages.gif"
    #define ICON2_ANI_1    "fs:/image/mainmenu/contacts.gif"
    #define ICON3_ANI_1    "fs:/image/mainmenu/Recent_calls.gif"
    #define ICON4_ANI_1    "fs:/image/mainmenu/Multimedia.gif"
    #define ICON5_ANI_1    "fs:/image/mainmenu/Net_Serve.gif"
    #define ICON6_ANI_1    "fs:/image/mainmenu/Tools.gif"
    #define ICON7_ANI_1    "fs:/image/mainmenu/Game.gif"         
    #define ICON8_ANI_1    "fs:/image/mainmenu/User_Profile.gif"
    #define ICON9_ANI_1    "fs:/image/mainmenu/Setting.gif"
#elif defined(FEATURE_VERSION_K212)
	#define ICON1_ANI_1    "fs:/image/mainmenu/ICON_Camera_1.png"
    #define ICON2_ANI_1    "fs:/image/mainmenu/ICON_Contacts_1.png"
    #define ICON3_ANI_1    "fs:/image/mainmenu/ICON_Management_1.png"
    #define ICON4_ANI_1    "fs:/image/mainmenu/ICON_Messages_1.png"
    #define ICON5_ANI_1    "fs:/image/mainmenu/ICON_Multimedia_1.png"
    #define ICON6_ANI_1    "fs:/image/mainmenu/ICON_Profile_1.png"
    #define ICON7_ANI_1    "fs:/image/mainmenu/ICON_Recentcalls_1.png"
    #define ICON8_ANI_1    "fs:/image/mainmenu/ICON_Settings_1.png"
	#define ICON9_ANI_1    "fs:/image/mainmenu/ICON_Surface_1.png"
	#define ICON10_ANI_1   "fs:/image/mainmenu/ARROW_LEFT.png"
    #define ICON11_ANI_1   "fs:/image/mainmenu/ARROW_RIGHT.png"
    #define ICON12_ANI_1   "fs:/image/mainmenu/SEL.png"	
#else
	#ifdef FEATURE_LCD_TOUCH_ENABLE

	#else
	#define ICON1_ANI_1      "fs:/image/mainmenu/qsc1100_01_focus.png"
    #define ICON2_ANI_1      "fs:/image/mainmenu/qsc1100_02_focus.png"
    #define ICON3_ANI_1      "fs:/image/mainmenu/qsc1100_03_focus.png"
    #define ICON4_ANI_1      "fs:/image/mainmenu/qsc1100_04_focus.png"
    #define ICON5_ANI_1      "fs:/image/mainmenu/qsc1100_05_focus.png"
    #define ICON6_ANI_1      "fs:/image/mainmenu/qsc1100_06_focus.png"
    #define ICON7_ANI_1      "fs:/image/mainmenu/qsc1100_07_focus.png"
    #define ICON8_ANI_1      "fs:/image/mainmenu/qsc1100_08_focus.png"
    #define ICON9_ANI_1      "fs:/image/mainmenu/qsc1100_09_focus.png"
    #define ICON10_ANI_1     "fs:/image/mainmenu/qsc1100_10_focus.png"
    #define ICON11_ANI_1     "fs:/image/mainmenu/qsc1100_11_focus.png"
    #define ICON12_ANI_1     "fs:/image/mainmenu/qsc1100_12_focus.png"
	#endif 
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
/*
#if defined(FEATURE_VERSION_K212)
	#define ICON1_ANI_2    "fs:/image/mainmenu/ICON_Camera_2.png"
    #define ICON2_ANI_2    "fs:/image/mainmenu/ICON_Contacts_2.png"
    #define ICON3_ANI_2    "fs:/image/mainmenu/ICON_Management_2.png"
    #define ICON4_ANI_2    "fs:/image/mainmenu/ICON_Messages_2.png"
    #define ICON5_ANI_2    "fs:/image/mainmenu/ICON_Multimedia_2.png"
    #define ICON6_ANI_2    "fs:/image/mainmenu/ICON_Profile_2.png"
    #define ICON7_ANI_2    "fs:/image/mainmenu/ICON_Recentcalls_2.png"
    #define ICON8_ANI_2    "fs:/image/mainmenu/ICON_Settings_2.png"
	#define ICON9_ANI_2    "fs:/image/mainmenu/ICON_Surface_2.png"
#endif
*/
#ifdef FEATURE_LCD_TOUCH_ENABLE
	#define ICON_SELECT          "fs:/image/mainmenu/select.png"
	#define ICON_SELECT_FOCUS    "fs:/image/mainmenu/select_focus.png"
	//#define ICON13_ANI           "fs:/image/mainmenu/qsc1100_13.png"
	//#define ICON13_ANI_1         "fs:/image/mainmenu/qsc1100_13_focus.png"
#endif


#ifdef FEATURE_VERSION_K212
#define MUSIC_PATH1 "fs:/hsmm/mainmenu/mesage.mp3"
#define MUSIC_PATH2 "fs:/hsmm/mainmenu/contont.mp3"
#define MUSIC_PATH3 "fs:/hsmm/mainmenu/recentcall.mp3"
#define MUSIC_PATH4 "fs:/hsmm/mainmenu/profile.mp3"
#define MUSIC_PATH5 "fs:/hsmm/mainmenu/mutimed.mp3"
#define MUSIC_PATH6 "fs:/hsmm/mainmenu/camera.mp3"
#define MUSIC_PATH7 "fs:/hsmm/mainmenu/manager.mp3"
#define MUSIC_PATH8 "fs:/hsmm/mainmenu/settings.mp3"
#define MUSIC_PATH9 "fs:/hsmm/mainmenu/suafer.mp3"
#endif

#ifdef FEATURE_VERSION_K212_ND
#define MUSIC_PATH1 	"fs:/hsmm/mainmenu/mesage.mp3"							//"fs:/hsmm/mainmenu/recentcall.mp3"
#define MUSIC_PATH2 	"fs:/hsmm/mainmenu/contont.mp3"							//"fs:/hsmm/mainmenu/profile.mp3"     //TORCH
#define MUSIC_PATH3 	"fs:/hsmm/mainmenu/recentcall.mp3"						//"fs:/hsmm/mainmenu/mutimed.mp3"      
#define MUSIC_PATH4 	"fs:/hsmm/mainmenu/settings.mp3"						//"fs:/hsmm/mainmenu/contont.mp3"
#define MUSIC_PATH5 	"fs:/hsmm/mainmenu/mutimed.mp3"							//"fs:/hsmm/mainmenu/settings.mp3"       
#define MUSIC_PATH6 	"fs:/hsmm/mainmenu/manager.mp3"							//"fs:/hsmm/mainmenu/suafer.mp3"
#define MUSIC_PATH7 	"fs:/hsmm/mainmenu/profile.mp3"							//"fs:/hsmm/mainmenu/manager.mp3"  //TOOLS
#define MUSIC_PATH8 	"fs:/hsmm/mainmenu/suafer.mp3"							//"fs:/hsmm/mainmenu/mesage.mp3"
#endif

#ifdef FEATURE_VERSION_EC99
#define MUSIC_PATH1 "fs:/hsmm/mainmenu/recentcall.mp3"
#define MUSIC_PATH2 "fs:/hsmm/mainmenu/profile.mp3"     //TORCH
#define MUSIC_PATH3 "fs:/hsmm/mainmenu/mutimed.mp3"      
#define MUSIC_PATH4 "fs:/hsmm/mainmenu/contont.mp3"
#define MUSIC_PATH5 "fs:/hsmm/mainmenu/settings.mp3"       
#define MUSIC_PATH6 "fs:/hsmm/mainmenu/suafer.mp3"
#define MUSIC_PATH7 "fs:/hsmm/mainmenu/manager.mp3"  //TOOLS
#define MUSIC_PATH8 "fs:/hsmm/mainmenu/mesage.mp3"
#endif

#ifdef FEATURE_VERSION_K212_20D
#define MUSIC_PATH1 "fs:/hsmm/mainmenu/recentcall.mp3"
#define MUSIC_PATH2 "fs:/hsmm/mainmenu/mutimed.mp3"      
#define MUSIC_PATH3 "fs:/hsmm/mainmenu/contont.mp3"
#define MUSIC_PATH4 "fs:/hsmm/mainmenu/settings.mp3"
#define MUSIC_PATH5 "fs:/hsmm/mainmenu/suafer.mp3"
#define MUSIC_PATH6 "fs:/hsmm/mainmenu/manager.mp3"  //TOOLS
#define MUSIC_PATH7 "fs:/hsmm/mainmenu/mesage.mp3"
#endif


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
void Mainmenu_KeypadLock(boolean block);

static uint32 MainMenuMod_AddRef( IModule *po);

static uint32 MainMenuMod_Release( IModule *po);

static void MainMenuMod_FreeResources( IModule *po, IHeap *ph, IFileMgr *pfm);


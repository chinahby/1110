#ifndef APPSCOMMON_H
#define APPSCOMMON_H
/*==============================================================================
//           A P P S   C O M M O N   F U N C T I O N S
// 文件：
//        Appscommon.h
//        版权所有(c) 2009 Anylook Tech. CO.,LTD.
//        
// 文件说明：
//
==============================================================================*/

/*==============================================================================
修改历史记录：
时       间       修 改 人     修改内容、位置及原因
-----------      ----------    -----------------------------------------------

==============================================================================*/



/*==============================================================================
                             
                             本文件包含的外部文件
                             
==============================================================================*/
#include "AEE.h"
#include "AEEShell.h"
#include "AEEMenu.h" 
#include "AEEDisp.h"
#include "AEEStdLib.h"
#include "AEEFile.h"
#include "OEMFeatures.h"
#ifdef FEATURE_USES_ZI
#include "OEMZIText.h"
#else
#include "OEMText.h"
#endif

/*==============================================================================
                                 
                                 宏定义和常数
                                 
==============================================================================*/
// 图标图片资源
// must use full file name for BREW 3
#define  AEE_APPSCOMMONRES_IMAGESFILE "fs:/sys/appscommonimages.bar"

// 根据 BREW 3 的需要，重定义资源文件宏
// 与语言相关的字符串资源文件
#define  AEE_APPSCOMMONRES_LANGFILE ("fs:/sys/" AEE_RES_LANGDIR APPSCOMMON_RES_FILE)


#define RGB_WHITE_NO_TRANS MAKE_RGB(0xFF, 0xFF, 0xFF)

#if defined(FEATURE_DISP_176X220)
    #define SCREEN_WIDTH            176
    #define SCREEN_HEIGHT           220
    #define STATEBAR_HEIGHT         22
    #define BOTTOMBAR_HEIGHT        22
    #define TITLEBAR_HEIGHT         22
    #define MENUITEM_HEIGHT         22
    #define SCROLLBAR_WIDTH         5
    #define STATUSBAR_HEIGHT        22
#elif defined(FEATURE_DISP_128X128)
    #define SCREEN_WIDTH            128
    #define SCREEN_HEIGHT           128
    #define STATEBAR_HEIGHT         16
	#define BOTTOMBAR_HEIGHT        16
    #define TITLEBAR_HEIGHT         16
    #define MENUITEM_HEIGHT         16
    #define SCROLLBAR_WIDTH         5
    #define STATUSBAR_HEIGHT        16
#elif defined(FEATURE_DISP_160X128)
    #define SCREEN_WIDTH            160
    #define SCREEN_HEIGHT           128
    #define STATEBAR_HEIGHT         16
    #define BOTTOMBAR_HEIGHT        16
    #define TITLEBAR_HEIGHT         16
    #define MENUITEM_HEIGHT         16
    #define SCROLLBAR_WIDTH         5
    #define STATUSBAR_HEIGHT        16
#elif defined(FEATURE_DISP_220X176)		   //176 = statebar(22)+bottombar(22)  +6*menuitem(22)
    #define SCREEN_WIDTH            220
    #define SCREEN_HEIGHT           176
    #define STATEBAR_HEIGHT         22
    #define BOTTOMBAR_HEIGHT        22
    #define TITLEBAR_HEIGHT         22
    #define MENUITEM_HEIGHT         22
    #define SCROLLBAR_WIDTH         5
    #define STATUSBAR_HEIGHT        22	
#elif defined(FEATURE_DISP_128X160)			//160 = statebar(16)+bottombar(16) +8*menuitem(16)
	#define SCREEN_WIDTH            128
    #define SCREEN_HEIGHT           160
    #define STATEBAR_HEIGHT         16
    #define BOTTOMBAR_HEIGHT        16
    #define TITLEBAR_HEIGHT         16
    #define MENUITEM_HEIGHT         16
    #define SCROLLBAR_WIDTH         5
    #define STATUSBAR_HEIGHT        16
#elif defined(FEATURE_DISP_240X320)	
	#define SCREEN_WIDTH            240
    #define SCREEN_HEIGHT           320
    #define STATEBAR_HEIGHT         25
    #define BOTTOMBAR_HEIGHT        25
    #define TITLEBAR_HEIGHT         25
    #define MENUITEM_HEIGHT         30
    #define SCROLLBAR_WIDTH         10
    #define STATUSBAR_HEIGHT        26	
#elif defined(FEATURE_DISP_320X240)			//240 = statebar(32)+bottombar(32)  +5*menuitem(32)
	#define SCREEN_WIDTH            320
    #define SCREEN_HEIGHT           240
    #define STATEBAR_HEIGHT         24
    #define BOTTOMBAR_HEIGHT        24
    #define TITLEBAR_HEIGHT         24
    #define MENUITEM_HEIGHT         32
    #define SCROLLBAR_WIDTH         5
    #define STATUSBAR_HEIGHT        23	
#else
    #define SCREEN_WIDTH            160
    #define SCREEN_HEIGHT           128
    #define STATEBAR_HEIGHT         16
    #define BOTTOMBAR_HEIGHT        16
    #define TITLEBAR_HEIGHT         16
    #define MENUITEM_HEIGHT         16
    #define SCROLLBAR_WIDTH         5
    #define STATUSBAR_HEIGHT        16
#endif

#define APPSCOMMON_BG_COLOR       (RGB_BLACK)
#define APPSCOMMON_TEXT_BG_COLOR  MAKE_RGB(60,60,60)
#define APPSCOMMON_DEFAULT_REND 0 //wlh 20090405 add for rend 随机效果  REND_RANDOM = 0
#define TEXT_GRAPHIC_FONT_COLOR  (RGB_WHITE)
#define TEXT_GRAPHIC_BG_COLOR      (RGB_BLACK)

#define TEXT_FONT_COLOR                 (RGB_BLACK)
#define TEXT_BG_COLOR                     (RGB_WHITE)

#define APPSCOMMON_MENUBG_XPOS  60
#define APPSCOMMON_MENUBG_YPOS  47

#define PROMPTMSG_TIMER        1000

#define MSGBOX_MAXTEXTLEN  256
//拉丁字母的上下限
#define CON_ACCENT_UPPER_START 192
#define CON_ACCENT_UPPER_END   221

#define CON_ACCENT_LOWER_START 224
#define CON_ACCENT_LOWER_END   253

#define OFFSET_ASCII                    (0xFF)
#define MAX_STR              128
#ifndef FEATURE_SMARTSEARCH
   #define FEATURE_SMARTSEARCH
#endif
#if defined( AEE_SIMULATOR)
    #define APPSCOMMON_RES_LANG_FILE "fs:/sys/en/appscommon.bar"
#else
    #define APPSCOMMON_RES_LANG_FILE     ("fs:/sys/" AEE_RES_LANGDIR APPSCOMMON_RES_FILE)
#endif

//将各模块各自定义的APPISREADY_TIMER和EVT_APPISREADY集中定义到appscommon.h，并将TIMER值统一为100ms
// 为防止用户快速按键而添加事件EVT_APPISREADY，
#define GET_RGB_R(v)     (uint32) ( ((RGBVAL)(v) >> 8) & (0x000000FF) )
#define GET_RGB_G(v)     (uint32) ( ((RGBVAL)(v) >> 16) & (0x000000FF) )
#define GET_RGB_B(v)     (uint32) ( ((RGBVAL)(v) >> 24) & (0x000000FF) )
#define GET_RGBA_A(v)   (uint32) ( (RGBVAL)(v) & (0x000000FF) )

#ifdef FEATURE_LCD_TOUCH_ENABLE//WLH ADD FOR LCD TOUCH
#define TOUCH_PT_IN_RECT(a,b,rct)      (boolean)( ((a) >= (rct).x && (a) <= ((rct).x + (rct).dx)) && ((b) >= (rct).y && (b) <= ((rct).y + (rct).dy)) )
#endif


//Add By zzg 2011_09_08
#define SOFTKEY_LEFT	1
#define SOFTKEY_CENTER  2
#define SOFTKEY_RIGHT   3
//Add End


/*==============================================================================
                                 
                                  数据类型定义
                                 
==============================================================================*/
typedef enum TitleBar_e_Type
{
    // 短消息主界面用
    TBAR_SMS_MAIN,
    
    // 设置主界面用
    TBAR_SETTING_MAIN,
    
    // 百宝箱主界面用
    TBAR_EXTRA_MAIN,
    
    // 数据业务主界面用
    TBAR_UMAX_MAIN,

    // 游戏主界面用
    TBAR_GAMES_MAIN,

    // 共用顶部提示图片
    TBAR_COMMON,

    // 在标题栏绘制左右箭头
    TBAR_ARROW
} TitleBar_e_Type;


/*
 * 软键行分类，单个，双个，三个。这样在加载字符串时可以分成三类，避免过多的
 * switch项。
 * */
#define BTBAR_SINGLE_MASK   0x1000
#define BTBAR_DOUBLE_MASK   0x2000
#define BTBAR_TRI_MASK      0x4000

typedef enum BottomBar_e_Type
{
   // 无底部提示条
   BTBAR_NONE,

   /*******************start BTBAR_SINGLE_MASK*****************************/
   BTBAR_BACK= 0x1000 + 1,   // -----返回
   BTBAR_CANCEL,            // -----取消 
   BTBAR_END_CALL,			//Add By zzg 2012_10_31
   BTBAR_CLOSE,              // -----关闭
   BTBAR_DELETE,             //guoys add @2008.10.20 for no match dont use select
   BTBAR_END,                // -----挂机
   BTBAR_NEXT,               // -----下一步
   BTBAR_OK,                 //BY XIAO.CHENG  
   BTBAR_PROMPT_OK,
   BTBAR_SOS,                // -----紧急呼叫
   BTBAR_STOP,
   BTBAR_SNOOZE,             //add by yangdecai
                             //display stop when alarming
   
   /*******************start BTBAR_DOUBLE_MASK*****************************/
   BTBAR_ACCEPT_REJECT = 0x2000 + 1,
   BTBAR_ADD_BACK,
   BTBAR_ANSWER_IGNORE,
   BTBAR_ANSWER_MUTE,
   BTBAR_ANSWER_SILENT,		//Add By zzg 2012_10_31
   BTBAR_ANSWER_REJECT,
   BTBAR_ANSWER_UNMUTE,
   BTBAR_BACK_DELETE,
   BTBAR_BACK_SOS,        // 返回-----紧急呼叫
   BTBAR_STOP_BACK,
   BTBAR_CALL_BACK,
   BTBAR_CONTINU_BACK,    // 继续-----返回
   BTBAR_CONTINU_DELETE,  // 继续-----删除
   BTBAR_CREATE_BACK,     // 创建-----返回
   BTBAR_EARSE_BACK,      // 删除-----返回
   BTBAR_EDIT_BACK,       // 编辑-----返回
   BTBAR_EXCHANGE_BACK,
   BTBAR_EXCHANGE_DELETE,
   BTBAR_FIND_BACK,       // 查找-----返回
   BTBAR_GALLERY_BACK,
   BTBAR_GALLERY_PLAY_BACK,
   BTBAR_LEFT_START_NULL,
   BTBAR_MENU_BACK,       // 菜单-----返回
   BTBAR_MENU_CONTACTS,   // 菜单-----电话本
   //Add By zzg 2010_07_20
   BTBAR_MENU_FRENDUO,	 // 菜单------FRENDUO
   BTBAR_VIEWMORE_BACK,	 // 浏览更多-----返回
   BTBAR_FACEBOOK_CHAT,	 // Facebook-----chat
   //Add End
   //BTBAR_LOCK_BACK,			//锁键盘-返回
   BTBAR_OK_BACK,         // 确定-----返回
   BTBAR_OK_SOS,          // 确定-----SOS
   BTBAR_OK_CANCEL,       // 确定-----取消
   BTBAR_OK_DEL,
   BTBAR_OK_DELETE,       // 确定-----删除
   BTBAR_OPTION_BACK,     // 选项-----返回
   BTBAR_OPTION_STOP,
   BTBAR_OPTION_DEL,
   BTBAR_OPTION_DELETE,   // 选项-----删除：
   BTBAR_OPTION_ENDCALL,  // 选项-----挂机:
   BTBAR_OPTION_HANDSFREEON,
   BTBAR_OPTION_NORMAL,
   BTBAR_OPTION_HANDS_FREE,		//Add By zzg 2012_10_31
   BTBAR_OPTION_HANDS_HELD,		//Add By zzg 2012_10_31
   BTBAR_OPTION_UP,       // 选项-----向上
   BTBAR_0PTION_PLAY_BACK,
   BTBAR_OPTIONS_BACK,
   BTBAR_PAUSE_BACK, 
   BTBAR_PLAY_BACK, 
   BTBAR_PLAY_STOP,   
   BTBAR_ACTIVATE_BACK,
   BTBAR_MIZONE_SELECT_BACK,
   BTBAR_FUNZONE_BACK,
#ifdef FEATURE_SPORTS_APP
   BTBAR_PAUSE_STOP,
   BTBAR_RESUME_STOP,
#endif
   BTBAR_READ_BACK,       // 阅读-----返回
   BTBAR_RECORD_STOP,
   BTBAR_REPLAY_BACK,     // 重播-----返回
   BTBAR_RESET_BACK,      // 重设-----返回
   BTBAR_RESUME_BACK,
   BTBAR_RETRY_CANCEL,
   BTBAR_SAVE_BACK,       // 保存-----返回
   BTBAR_SAVE_CANCEL,     // 保存-----取消
   BTBAR_SAVE_DELETE,     // 保存-----删除
   BTBAR_SELECT_BACK,     // 选择-----返回
   BTBAR_SELECT_DEL,      // 选择-----返回
   BTBAR_SELECT_STOP,     // 选择-----停止    
   BTBAR_SEND_BACK,       // 发送-----返回
   BTBAR_SEND_EARSE,      // 发送-----删除
   BTBAR_SNOOZE_DONE,
   BTBAR_SNOOZE_STOP,
   BTBAR_START_BACK,
   BTBAR_UNLOCK_SOS,
   BTBAR_SPORT_STOP,
   BTBAR_VIEW_SNOOZE,
   BTBAR_VIEW_STOP,
   BTBAR_YES_NO,
   BTBAR_VIEW_CANCEL,	//Add By zzg 2012_11_09
   BTBAR_SEARCH_BACK,	//Add By zzg 2012_11_23

#ifdef FEATURE_VERSION_C316
   BTBAR_OPTION_SILENT,
#endif

   /*******************start BTBAR_TRI_MASK*****************************/
   BTBAR_CANCEL_RESUME_NULL = 0x4000 + 1,
   BTBAR_FULLSCREEN_PAUSE_STOP,
   BTBAR_JEWISH_TODAY_BACK,
   BTBAR_OPTION_CREATE_BACK,
   BTBAR_OPTION_EDIT_BACK,    // 删除---编辑---返回
   BTBAR_OPTION_OK_BACK,
   BTBAR_OPTION_OK_DEL,
   BTBAR_OPTION_PAUSE_STOP,
   BTBAR_OPTION_PLAY_BACK,
   BTBAR_OPTION_STOP_BACK,
   BTBAR_OPTION_ZOOM_BACK,
   BTBAR_OPTION_PLAY_STOP,
   BTBAR_OPTION_SAVE_DEL,
   BTBAR_OPTION_TODAY_BACK,
   BTBAR_SAVE_AM_BACK,
   BTBAR_SAVE_IP_DELETE,      // 保存--IP---删除：
   BTBAR_SAVE_PM_BACK,
   BTBAR_SELECT_DEL_BACK,     // 选择---删除---返回
   BTBAR_SELECT_OK_BACK,   
   BTBAR_SELECT_OK_DEL,
   BTBAR_SELECT_SET_BACK,
   BTBAR_SNOOZE_CONTINUE_STOP,
   BTBAR_CONTINUE_RESET_BACK,
   BTBAR_CALL_MODIFY_BACK,
   BTBAR_YES_NO_CANCEL,
#ifdef FEATURE_FLEXI_STATIC_BREW_APP
   BTBAR_FNASRANI_FPORTAL,   //fnasrani--fportal
   BTBAR_FMUSLIM_FPORTAL,   //fmuslim  ---fnasrani
   BTBAR_FGURU_FPORTAL,
   BTBAR_CONTACTS_FPORTAL,   //fmuslim  ---fnasrani   
#endif /*FEATURE_FLEXI_STATIC_BREW_APP*/
   BTBAR_FMENU_CANTACT,
   BTBAR_MENU_FMENU,
   BTBAR_OPTION,
   BTBAR_MESSAGES_CONTACTS,   // 信息-----电话本
   BTBAR_MENU_SOS,            //主菜单  -----SOS
   BTBAR_UNLOCK,
   BTBAR_UNLOCK_M,
   BTBAR_UNLOCK_L,
   BTBAR_LUNLOCK,
   BTBAR_OPTION_SAVE_BACK,   
   BTBAR_PRIVIEW_BACK,
   BTBAR_MODIFY_BACK,
   BTBAR_SELECT_PLAY_BACK,
   BTBAR_SELECT_STOP_BACK,
   BTBAR_GALLERY_HELP_BACK,		//Add By zzg 2012_12_10 for C337
   BTBAR_SHORTCUT_CONTACTS,
   BTBAR_CONTINUE,     //add by yangdecai 2012
   BTBAR_MAX = 0xFFFF, /*For ADS compiler allocate one byte to enumerate type
                     variable by default if the max value not exceed 256*/

} BottomBar_e_Type;

typedef enum PromptMsg_e_Type
{
    /* 信封图标*/
    //无提示消息内容
    MESSAGE_NONE,
    
    /* 三角加叹号警告图标*/
    //错误消息提示，底部提示条无字符
    MESSAGE_ERR,    
    
    /* 感叹号图标*/
    //告警消息提示，底部提示条无字符，不允许有按键处理，1秒后自动返回  
    MESSAGE_WARNNING, 
    
    /* 沙漏图标*/
    //等待消息提示，底部提示条无字符，不允许有按键处理，1秒后自动返回     
    MESSAGE_WAITING, 
    
    /* 问号图标*/
    //确认消息提示，底部提示条无字符，不允许有按键处理，1秒后自动返回   
    MESSAGE_CONFIRM,
    
    /* 信封图标*/
    //提示消息提示，底部提示条无字符，不允许有按键处理，1秒后自动返回  
    MESSAGE_INFORMATION,
    
    /* 手机加对勾图标*/
    //事件完成后消息提示，底部提示条有确认－返回，允许有按键处理，待按键处理完后返回  
    MESSAGE_INFORMATIVE       
#ifdef FEATURE_VERSION_C316 
	,MESSAGE_UNLOCK
#endif
} PromptMsg_e_Type;

typedef struct _Theme_Param_type
{
    RGBVAL  themeColor;      // 主题颜色
    RGBVAL  textColor;       // 文本颜色
    RGBVAL  seltextColor;    // 选中文本颜色
    RGBVAL  bkcolor;         // 背景颜色
    
    // 下列参数暂时没用，后续可能会用到
    RGBVAL  selbkcolor;           // 选中文本背景颜色
    uint16  nTBarImgResID;        // 标题条图片在资源文件中的 ID
    char    strTBarImgResFile[MAX_FILE_NAME]; // 标题条图片所在的资源文件
    uint16  nBBarImgResID;        // 底条文本在资源文件中的 ID
    char    strBBarImgResFile[MAX_FILE_NAME]; // 底条图片所在的资源文件
} Theme_Param_type;

typedef struct _TitleBar_Param_type
{
    TitleBar_e_Type eTBarType;          // 标题条类型
    AEERect         *prc;               // 标题条矩形
    AECHAR          *pwszTitle;         // 标题条文本
    uint16          nTitleResID;        // 标题条文本在资源文件中的 ID
    char            strTitleResFile[MAX_FILE_NAME]; // 标题条图片所在的资源文件
    uint16          nImgResID;          // 标题条图片在资源文件中的 ID
    char            strImgResFile[MAX_FILE_NAME];  // 标题条图片所在的资源文件
    uint32          dwAlignFlags;       // 标题条文本对齐方式
} TitleBar_Param_type;

typedef struct _BottomBar_Param_type
{
    BottomBar_e_Type    eBBarType;          // 底条类型
    AEERect             *prc;               // 底条矩形
    uint16              nImgResID;          // 底条图片在资源文件中的 ID
    char                strImgResFile[MAX_FILE_NAME]; // 底条图片所在的资源文件
} BottomBar_Param_type;

typedef struct _PromptMsg_Param_type
{
    PromptMsg_e_Type    ePMsgType;          // 提示消息类型
    AEERect             *prc;               // 提示消息矩形
    AECHAR              *pwszTitle;         // 标题条文本
    BottomBar_e_Type    eBBarType;          // 底条类型
    AECHAR              *pwszMsg;           // 提示消息文本    
    uint16              nMsgResID;          // 提示消息字符在资源文件中的 ID
    char                strMsgResFile[MAX_FILE_NAME]; // 提示消息字符所在的资源文件
} PromptMsg_Param_type;

#define GSM_CHARACTER_SET_SIZE      0x80
extern const char GsmToUcs2[GSM_CHARACTER_SET_SIZE];

#define UCS2_TO_GSM_TABLE_SIZE      0x100
#define NON_GSM                     UCS2_TO_GSM_TABLE_SIZE
#define GSM_CHARACTER_SET_SIZE      0x80
extern  const AECHAR  Ucs2ToGsm[UCS2_TO_GSM_TABLE_SIZE];

#ifdef FEATURE_LANG_HEBREW
#define     HEBREW_CONVERT_TABLE_SIZE   0x100
#define     NON_HEB     HEBREW_CONVERT_TABLE_SIZE 

// Ucs2ToHebrew 表使用说明: 
// 如要检索UNICODE字符 wChar 转换为 Hebrew 的值，请使用 Ucs2ToHebrew[wChar & 0x00FF]
// 特殊字符 0x203E 、0x00D7 不能使用此表。0x203E-->0xAF 、0x00D7-->0xAA 。 返回值 NON_HEB
// 表示无相应字符，转换失败
extern const AECHAR  Ucs2ToHebrew[HEBREW_CONVERT_TABLE_SIZE];

// HebrewToUcs2 表使用说明: 
// 如要检索无符号 char 字符 c 转换为 Hebrew UNICODE 的值，请使用 Ucs2ToHebrew[c] . 返回值为 0，表示转换
// 不成功
extern const AECHAR  HebrewToUcs2[HEBREW_CONVERT_TABLE_SIZE];
#endif

#ifdef FEATURE_SPORTS_APP
typedef struct
{
   char*    argsz;
   boolean              runOrwalk;
   AECHAR               subject[64];
   AECHAR               location[64];
} Sport_Appointment;

#endif

/*==============================================================================

                                 函数声明
                                 
==============================================================================*/

                       
/*==============================================================================
函数:
    DrawBottomBar

说明:
    本函数用于画与语言相关的底部提示条。

参数:
    pIDisplay [in]: 指向 IDisplay 接口的指针。
    BParam [in]: 底部提示条相关参数指针。

返回值:
    none

备注:
    本函数用于画与语言相关的底部提示条，语言无关的底部提示图片，请自行绘制，不
    要调用此函数！同时本函数不负责界面的更新，界面的更新由调用者自行完成。最多
    支持底部有3个按钮提示。
==============================================================================*/
void DrawBottomBar(IDisplay  * pIDisplay, BottomBar_Param_type *BParam);
                       

//Add By zzg 2011_09_08
/*==============================================================================
函数:
    DrawBottomBg

说明:
    本函数用于画与语言相关的底部提示条。

参数:
    pIDisplay [in]: 指向 IDisplay 接口的指针。
  
返回值:
    none

备注:
    本函数用于画与语言相关的底部提示条背景图。
==============================================================================*/
void DrawBottomBg(IDisplay  * pIDisplay);

/*==============================================================================
函数:
    DrawBottomText

说明:
    本函数用于画与语言相关的底部提示条。

参数:
    pIDisplay [in]: 指向 IDisplay 接口的指针。
    pText: 底部提示条相关文本。
    type:   底部左中右类型。

返回值:
    none

备注:
    本函数用于画与语言相关的底部提示条文本。
==============================================================================*/
void DrawBottomText(IDisplay  * pIDisplay,  char* pText, int type);

//Add End




/*==============================================================================
函数:
    DrawTitleBar

说明:
    本函数用于画标题提示条。

参数:
    pIDisplay [in]: 指向 IDisplay 接口的指针。
    TParam [in]: 标题条相关参数指针。

返回值:
    none

备注:
    本函数用于画标题条，提示条图片来自非资源文件，必须下载到手机。绘制的标题文本
    可由参数传入。同时本函数不负责界面的更新，界面的更新由调用者自行完成。
==============================================================================*/
void DrawTitleBar(IDisplay  * pIDisplay, TitleBar_Param_type *TParam);

/*==============================================================================
函数:
    DrawTextWithProfile

说明:
    函数用于绘制带轮廓的文本。

参数:
    pShell [in]：IShell 接口指针。
    pDisplay [in]：IDisplay 接口指针。
    ProfileColor [in]：文本轮廓的RGBVAL颜色值。
    参数 Font、pcText、nChars、x、y、prcBackground、dwFlags 含义同接口函数
    IDISPLAY_DrawText 。

返回值:
    none

备注:
    同时本函数不负责界面的更新，界面的更新由调用者自行完成。
==============================================================================*/
void DrawTextWithProfile(IShell* pShell, 
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

/*==============================================================================
函数:
    DrawGreyBitTextWithProfile

说明:
    函数用于绘制带轮廓的文本。

参数:
    pShell [in]：IShell 接口指针。
    pDisplay [in]：IDisplay 接口指针。
    ProfileColor [in]：文本轮廓的RGBVAL颜色值。
    参数 Font、pcText、nChars、x、y、prcBackground、dwFlags 含义同接口函数
    IDISPLAY_DrawText 。

返回值:
    none

备注:
    同时本函数不负责界面的更新，界面的更新由调用者自行完成。
==============================================================================*/
void DrawGreyBitTextWithProfile(IShell* pShell, 
    IDisplay * pDisplay, 
    RGBVAL ProfileColor, 
    int Font, 
    const AECHAR * pcText, 
    int nChars,
    int x, 
    int y, 
    const AEERect * prcBackground, 
    uint32 dwFlags
);


/*==============================================================================
函数:
    EncodePWDToUint16

说明:
    将输入字符串转换为无符号整型数。

参数:
    pszPWD [in]：密码字符串。

返回值:
    none

备注:
    密码字符串长度最多为8，且只含字符0-9及*和#
==============================================================================*/
uint16 EncodePWDToUint16(char *pszPWD);

/*==============================================================================
函数:
    Appscom_GetThemeParameters

说明:
    函数用于获取当前设置的主题相关参数。

参数:
    TParam [in/out]: 用于返回结果的参数指针。

返回值:
    none

备注:

==============================================================================*/
void Appscom_GetThemeParameters(Theme_Param_type *TParam);

/*==============================================================================
函数:
    GetTitleBarHeight

说明:
    本函数用于获取标题提示条的高度。

参数:
    pIDisplay [in]: 指向 IDisplay 接口的指针。

返回值:
    none

备注:

==============================================================================*/
int GetTitleBarHeight(IDisplay  * pIDisplay);

/*==============================================================================
函数:
    GetBottomBarHeight

说明:
    本函数用于获取底部提示条的高度。

参数:
    pIDisplay [in]: 指向 IDisplay 接口的指针。

返回值:
    none

备注:

==============================================================================*/
int GetBottomBarHeight(IDisplay  * pIDisplay);

/*==============================================================================
函数:
    DrawPromptMessage

说明:
    函数用于获取当前设置的主题相关参数。

参数:
    pIDisplay  [in]：IDisplay指针。
    pStatic    [in]: IStatic控件。  
    PParam     [in]: 提示消息相关参数指针。

返回值:
    none

备注:
    本函数用于画各种消息提示框。调用者必须建一个带有static控件的dialog，将此dialog的矩形框全部设为0，然后调用此函数
    实现消息字符和相关图片的显示。显示完成后的状态机处理和计时器的处理由dialog自行完成，本函数不负责
    状态机的处理和计时器的处理。
    每种消息类型有不同的用处，调用者要填准消息类型，确保风格一致：
    
    1.MESSAGE_NONE：
    不带任何图片。
    
    2.MESSAGE_ERR/MESSAGE_WARNNING：
    For tempo screen, there are no soft keys displayed. 
    The prompt screen will display for 3 seconds and then back to the previous screen 
    automatically. User can also press any key to dismiss the pop-up screen. 
    and back to the previous screen.  
      
    3.MESSAGE_WAITING：
    For the waiting screens there are no soft keys displayed. 
    The wait clock is displayed in full screen. 
    E.g. modules: SMS initializing, phonebook loading and other normal waiting modules 
       
    4.MESSAGE_CONFIRM：
    When the user has to make a key press to confirm or not an action, 
    two soft keys are displayed Icon used is not fixed, The sentence should be finished by question mark.
    E.g. modules: Confirm Delete, Save Entry? , Confirm Reset
    
    5.MESSAGE_INFORMATION：
    For tempo screen, there are no soft keys displayed。
    E.g. modules: Information of positive actions. Deleted, Saved, Copied, Done, Moved, 
    Timer Reset, Activated, Deactivated, Pin OK, Pin Changed. The prompt screen will 
    display for 500ms and then back to the previous screen automatically. 
    User can also press any key to dismiss the pop-up screen and back to the previous screen.
    
    6.MESSAGE_INFORMATIVE：
    For tempo screen, there are no soft keys displayed。
    E.g. modules: Call Timer, Call Cost  Any missed screens, 
    The prompt screen will display for 500ms 
    and then back to the previous screen automatically. User can also press any 
    key to dismiss the pop-up screen, and back to the previous screen.  
   
==============================================================================*/
void DrawPromptMessage (IDisplay *pIDisplay,
                        IStatic *pStatic,
                        PromptMsg_Param_type *PParam);


//Add By zzg 2011_12_08
/*
函数:
    CancelReDrawPromptMessage

说明:
    本函数用于取消RedrawPromptMessage的TIMER

参数:   

返回值:
    none

备注:
    为了同步AutoScroll的Static空间的刷新而额外开的一个TIMER

*/

void CancelReDrawPromptMessage (void *pShell);

//Add End

/*==============================================================================
函数:
    DrawBackground

说明:
    本函数用于画控件背景图。

参数:
    pDisplay   [in]：IDisplay指针。
    pRect      [in]：目标区域大小。

返回值:
    none

备注:
    同时本函数不负责界面的更新，界面的更新由调用者自行完成。
==============================================================================*/
void DrawBackground( IDisplay *pDisplay, AEERect *pRect);
#ifdef FEATURE_KEYGUARD
void Appscomm_Draw_Keyguard_Msg(IDisplay *pIDisplay,IStatic *pStatic,boolean unlockkey);
void Appscomm_Draw_Keyguard_Information(IDisplay *pIDisplay,IStatic *pStatic,boolean unlockkey);	//Add By zzg 2012_12_03
#ifdef FEATURE_LCD_TOUCH_ENABLE 
void Appscomm_Draw_Keyguard_Slide(IDisplay *pIDisplay,uint16 x,uint16 y);
void Appscomm_Draw_Keyguard_BackGroud(IDisplay *pIDisplay,uint16 x,uint16 y);
void Appscomm_Draw_Keyguard_BackGroudbar(IDisplay *pIDisplay,uint16 x,uint16 y);
#endif
#endif

#ifdef FEATURE_KEYGUARD
void Appscommon_Draw_Keyguard_Time(IDisplay *pIDisplay);
#endif

void DrawBottomBar_Ex(IShell    *m_pIShell, IDisplay  * pIDisplay, BottomBar_e_Type    eBBarType);

/*==============================================================================
函数: 
    DecodeAlphaString
       
说明: 
    本函数解码 Alpha 域字符串。
       
参数: 
    pdata [in]：指向输入数据字节序列。
    nLen [in]：输入数据字节序列长度,单位字节。
    wstrOut [out]： 输出Buffer
    nBufSize [in]:  输出Buffer大小,单位 AECHAR
       
返回值:
    none
       
备注:
       
==============================================================================*/
void DecodeAlphaString(byte *pdata, int nLen, AECHAR *wstrOut, int nBufSize);
int Appscomm_is_incoming_state(int bb);

 void SetArrowFlagonIM(boolean bFlag);
 boolean GetArrowFlagonIM(void);

/*==============================================================================
函数: 
    Appscommon_DrawDialogBoxFrame
       
说明: 
    绘制弹出对话框边框及背景色。
       
参数: 
    pBoxRect [in]: 输入将要绘制对话框的矩形范围。 
    bUpward [in]: 对话框箭头三角形是否在上方。
    nFrameColor [in]: 对话框边框颜色。
    nBgColor [in]: 对话框背景颜色
       
返回值:
    TRUE: 绘制成功
    FALSE: 绘制失败
       
备注:
       
==============================================================================*/
boolean Appscommon_DrawDialogBoxFrame(AEERect *pBoxRect, boolean bUpward, RGBVAL nFrameColor, RGBVAL nBgColor);

/*==============================================================================
函数: 
    Appscommon_DrawPopUpDialogBox
       
说明: 
    绘制弹出对话框(单行，含文本)。
       
参数: 
    pIDisplay [in]:
    x [in]: 输入将要绘制对话框左上角x坐标。
    y [in]: 输入将要绘制对话框左上角y坐标。
    strDisplay[in]:输入将要绘制对话的文本内容。
       
返回值:
    TRUE: 绘制成功
    FALSE: 绘制失败
       
备注:
    可以在函数调用之前使用IDISPLAY_SetColor设置文本颜色，最大显示长度为
    五分之四屏幕宽，显示起点x不小于五分之一屏幕宽，显示起点y不大于
    menu最低一条可显示的起始坐标， 不符的将自动调整。
       
==============================================================================*/
boolean Appscommon_DrawPopUpDialogBox(IDisplay *pIDisplay, int x, int y, AECHAR *strDisplay);

/*==============================================================================
函数: 
    Appscommon_DrawDigitalNumber
       
说明: 
    绘制仿七段数码管数字
       
参数: 
    pDisplay [in]: 
    number[in]: 需要绘制的数字。
    nLineWidth[in]: 需要绘制的数字线条宽度。
    fontRect [in]: 绘制数字的边界矩形框。
    fontColor[in]: 需要绘制的字体颜色。
       
返回值:
    FALSE: 绘制失败
    TRUE: 绘制成功
       
备注:
       
==============================================================================*/
boolean Appscommon_DrawDigitalNumber (IDisplay *pDisplay, int number, int nLineWidth, AEERect *fontRect, RGBVAL fontColor);


/*=============================================================================

FUNCTION: WStr2Alpha

DESCRIPTION: 
    将字符串转换为对应的按键字符串
    
PARAMETERS:
    pDest:
    nLen : 目的字符串长度 in AECHAR
    pSrc :
    
RETURN VALUE:
    
COMMENTS:

SIDE EFFECTS:

SEE ALSO:
=============================================================================*/
void WStr2Alpha(AECHAR *pDest, int nLen , AECHAR *pSrc);


/*=============================================================================

FUNCTION: AlphaNCMP

DESCRIPTION: 
    比较两个 Alpha 字符串比较长度, NULL 字符结尾,*号通配符
    
PARAMETERS:
    pStr1:
    pStr2:
    nLen : 比较的长度，in AECHAR
        
RETURN VALUE:
    boolean
        
COMMENTS:
    
SIDE EFFECTS:

SEE ALSO:
=============================================================================*/
boolean AlphaNCMP(AECHAR *pStr1, AECHAR *pStr2, int nLen);

/*==============================================================================
函数: 
    Appscommon_CompareName

说明: 
    本函数用于比较两个字符串以便排序。

参数: 
    wstrNam1 [in]: 字符串1。
    wstrNam2 [in]: 字符串2。
返回值: 
    1 : wstrNam1>wstrNam2
    0 : wstrNam1==wstrNam2
    -1: wstrNam1<wstrNam2

备注:
    如果是电话本调用 OEM 层在初始化 Cache 时会调用此函数来对 Cache 排序。UI 层不必再排序。
    其他模块调用需自行排序

==============================================================================*/
int Appscommon_CompareName(AECHAR *wstrNam1,AECHAR *wstrNam2);


/*获取单个数字的宽度*/
int GetSingleNumberWidth(IDisplay *pDisplay, AEEFont Font);
boolean IsDoubleBytesIME(AEETextInputMode imnputmode);
boolean HaveNoneASCIIChar ( AECHAR * pWstr, int * pPostion);

/*==============================================================================
函数: 
    Appscommon_ResetBackground

说明: 
    绘制某块区域的背景图片。

参数: 
    pDisplay [in]: 
    BgImage [in]: 背景图片指针
    BgColor [in]: 背景颜色
    rect [in]: 需要绘制的矩形范围
    x,y[in]: 图片左上角在屏幕中的起点横坐标与纵坐标
    
返回值: 
    无

备注:
    如果背景图片不为空，则绘制图片在指定矩形范围，
    否则将指定范围填充为背景色。

==============================================================================*/
void Appscommon_ResetBackground(IDisplay *pDisplay, IImage *BgImage, RGBVAL BgColor, AEERect * rect, int x, int y);
/*==============================================================================
函数: 
    Appscommon_DrawScrollBar

说明: 
    绘制滚动条。

参数: 
    pDisplay [in]: 
    nCurrentIdx [in]: 当前页面最下面一项索引值(以1为起点)
    nTotalItem [in]: 项目总数
    nPageItem[in]: 单屏幕最大显示项目数
    ScrollRect [in]: 需要绘制的矩形范围
    
返回值: 
    无

备注:

==============================================================================*/
void Appscommon_DrawScrollBar(IDisplay *pDisplay, int nCurrentIdx, int nTotalItem, int nPageItem, AEERect *ScrollRect);

// 封装一下中文转化为字母
#ifdef FEATURE_LANG_CHINESE
AECHAR CNUnicode2Letter(AECHAR c);
/*=============================================================================

Macro : Letter2Alpha

DESCRIPTION: 
    将英文字母转换为对应的按键字符
    
PARAMETERS:
    letter:
    
=============================================================================*/
AECHAR CNLetter2Alpha(AECHAR letter);

#endif
/*==============================================================================
函数: 
    Appscommon_GetRandomColor

说明: 
    绘制滚动条。

参数: 
    nMaxVal [in]: 像素平均值最大值，用来限制获取色彩的亮度，
    如不需要限制，设置为255即可
    
返回值: 
    获得的颜色RGB值

备注:

==============================================================================*/
RGBVAL Appscommon_GetRandomColor(uint32 nMaxVal);

/*==============================================================================
函数: 
    Appscommon_ResetBackgroundEx

说明: 
    绘制某块区域的背景色以及图片。

参数: 
    pDisplay [in]: 
    rect [in]: 需要绘制的矩形范围
    bDrawImage[in] : 是否绘制图片，FALSE仅填充背景色
    
返回值: 
    无

备注:
    将指定范围填充为默认的背景色APPSCOMMON_BG_COLOR，如果传入参数为TRUE时，
    绘制默认背景图片。

==============================================================================*/
void Appscommon_ResetBackgroundEx(IDisplay *pDisplay, AEERect * rect, boolean bDrawImage);

int Appscommon_bluetooth_used_sd(int aa);
/*==============================================================================
函数:
    IPAdd_FromNetValueToString

说明:
    函数将IP地址从网络值转换为字符串形式。

参数:
    dwAddNet [in]：IP地址网络值。
    pszbuf [in/out]：返回结果的 Buffer。
    nSize [in]：Buffer 大小。

返回值:
    TRUE: 转换成功
    FALSE:转换失败

备注:

==============================================================================*/
boolean IPAdd_FromNetValueToString(uint32 dwAddNet, char *pszbuf, int nSize);

/*==============================================================================
函数:
    IPAdd_FromStringToNetValue

说明:
    函数将IP地址从字符串形式转换为网络值。

参数:
    pdwAddNet [in/out]：返回IP地址网络值的 Buffer。
    pszbuf [in]：字符串形式的IP地址。

返回值:
    TRUE: 转换成功
    FALSE:转换失败

备注:

==============================================================================*/
boolean IPAdd_FromStringToNetValue(uint32 *pdwAddNet, char *pszbuf);

/*==============================================================================
函数:
    Appscommon_FormatTimeString

说明:
    函数将时间转换为字符串显示格式。

参数:
    nSeconds [in]：需要转换的时间。
    wstrTime [in]：输出字符串缓存地址。
    nTimeStringLength[in]：输出字符串缓存区长度。

返回值:
    TRUE: 转换成功
    FALSE:转换失败

备注:

==============================================================================*/
boolean Appscommon_FormatTimeString(uint32 nSeconds, AECHAR* wstrTime, int nTimeStringLength);


#define APP_MEDIA_ALLOW         0    
#define APP_MEDIA_IMPACT_BY_FM  1
#define APP_MEDIA_IMPACT_BY_MP3 2
extern int app_media_scheduler(void);

#endif // APPSCOMMON_H

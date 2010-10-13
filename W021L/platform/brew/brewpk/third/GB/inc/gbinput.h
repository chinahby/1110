#ifndef	_GBINPUT_H_
#define _GBINPUT_H_

#include "GBLt.h"
#include "GBLt_def.h"
#include "EB.h"

//#ifndef WIN32
//#include "wchar.h"
//#endif

#define __GBSHDATA		/** @自造词功能开关*/

#define GB_DATA_UNICODE16		1 /**< 中文引擎使用Unicode编码数据 */
#define GB_DATA_GB			2 /**< 中文引擎使用GB编码数据 */

#ifndef GB_DATA_CODE
#define GB_DATA_CODE	GB_DATA_UNICODE16
#else
#define GB_DATA_CODE	GB_DATA_GB
#endif

#ifndef __GB_TYPE__
#define __GB_TYPE__
typedef const void *                GBCPVOID;
typedef void *                      GBPVOID;
typedef int                         GBINT;
typedef unsigned int                GBUINT;
typedef unsigned short              GBUINT16;
typedef long                        GBINT32;
typedef long                        GBLONG;
typedef unsigned long               GBULONG;
typedef unsigned long               GBUINT32;
typedef unsigned char               GBBYTE;
#ifndef DEFINED_GBCHAR
typedef char                        GBCHAR;
#endif
typedef unsigned char               GBUINT8;
typedef char                        GBINT8;
typedef char *                      GBPSTR;
typedef const char *                GBPCSTR;
typedef unsigned short *            GBPWSTR;
typedef const unsigned short *      GBCPWSTR;
typedef unsigned char 				GBBOOL8;
typedef unsigned int				GBBOOL;
#endif


//#define VIETNAM
//#define DIFF_CAPITAL

/**@brief 中间层定义的输入法模式 */
typedef enum  {
    GBIMNull = 0,/**< 空 */
    GBPinYin,/**< 拼音输入法 */
    GBStroke,/**< 笔画输入法 */
    GBYinStroke,/**< 组合输入法 */
    GBSmartLatin,/**< 智能英文输入法 */
    GBCharacter,/**< 字母输入法 */
	GBNumber,/**< 数字输入法 */
#ifdef DIFF_CAPITAL
	GBCAPITALCharacter,
	GBLOWERCASECharacter
#endif
} GBInputMode;

/**@brief 中文引擎初始化数据 */
typedef struct __tag_gb_chn_engine_init_data {
    GBCPVOID                        pEngineData; /**< 中文引擎的数据地址 */
    GBPVOID                          pSelfMadeData; /** 中文自造词以及调频数据地址，以byte为单位 */
    GBUINT32                        nSelfMadeDataLength; /**< 中文自造词以及调频数据的长度 */
    GBInputMode                     nInputMethod; /**< 初始的中文输入模式 */
    GBINT                           nCandDesireLenInWord; /**< 中文候选字词的字数和 */
    GBINT                           nInterpunctionDesireLen; /**< 中文候选标点个数 */
    fpEBShDataUpdate                fpOnSelfMadeDataChanged; /**< 中文自造词以及调频数据更新通知函数 */
    GBINT							reserved[9];/** 预留 */
} GBChnEngineInitData;  

/**@brief 拉丁引擎初始化数据 */
typedef struct __tag_gb_lt_engine_init_data {
    GBCPVOID                        pEngineData; /**< 英文引擎的数据 */
    GBPVOID                         pSelfMadeData; /**< 英文自造词以及调频数据地址 */
    GBUINT32                        nSelfMadeDataLength; /**< 自造词以及调频数据长度，以byte为单位 */
    GBINT                           nCandDesireLenInWord;/**< 英文候选词语的长度之和的最大值 */
    GBINT                           nCandDesireCountInWord;/**< 英文候选词语的个数的最大值 */
    fpUpdateSelfHelpData            fpOnSelfMadeDataChanged;/**< 英文自造词以及调频数据更新通知函数 */
    GBINT							reserved[10];/** 预留 */
} GBLtEngineInitData;

/**@brief 国笔输入法设置数据 */
typedef struct __tag_gb_setting {
    GBBOOL8                         bUseLongPhress;/**< 是否使用长按 */
    GBBOOL8                         bUpNumber;/**< 数字无切换上屏 */
    GBBOOL8                         bChnEnConvert;/**< 中英无切换输入 */
    GBBOOL8                         reserved[13];/** 预留 */
} GBSetting;


typedef struct __tag_GBIME_INIT {
    GBChnEngineInitData				gbChnEngineInit;
    GBLtEngineInitData				gbLtEngineInit;
    GBSetting                       gbSetting;
    GBINT32                         reserved[4];
} GBIME_INIT;

//#define _SHOW_DEBUG
#define CANDNUM_APAGE_CHARIM	8
#define MESSAGE_MAX_LENGTH 32
#define MAX_UPSCREEN  32   /**@brief 最大上屏字的长度 */



typedef enum 
{
   NO_GBIME_MODE,
   PT_PINYIN_MODE,
   KEY_PINYIN_MODE,
   KEY_STROKE_MODE,
   HW_ASSOC_MODE
}GBIME_MODE;

/**@brief 按键1 */
#define GB_VK1 '1'
/**@brief 按键2 */
#define	GB_VK2 '2' 
/**@brief 按键3 */
#define	GB_VK3 '3'
/**@brief 按键4 */
#define	GB_VK4 '4'
/**@brief 按键5 */
#define	GB_VK5 '5'
/**@brief 按键6 */
#define	GB_VK6 '6'
/**@brief 按键7 */
#define	GB_VK7 '7'
/**@brief 按键8 */ 
#define	GB_VK8 '8' 
/**@brief 按键9 */
#define	GB_VK9 '9'
/**@brief 按键0 */ 
#define	GB_VK0 '0' 

/**@brief 长按1 */
#define GB_VK1L (1024+6)
/**@brief 长按2 */ 
#define	GB_VK2L (1024+7)
/**@brief 长按3 */ 
#define	GB_VK3L (1024+8) 
/**@brief 长按4 */
#define	GB_VK4L (1024+9) 
/**@brief 长按5 */
#define	GB_VK5L (1024+10)
/**@brief 长按6 */ 
#define	GB_VK6L (1024+11) 
/**@brief 长按7 */
#define	GB_VK7L (1024+12)
/**@brief 长按8 */ 
#define	GB_VK8L (1024+13) 
/**@brief 长按9 */
#define	GB_VK9L (1024+14) 
/**@brief 长按0 */
#define	GB_VK0L (1024+15) 

#ifndef _SHOW_DEBUG
/**@brief 按键OK */
#define GB_VK_OK		(1024+16) 

#define GB_VK_RETURN	(1024+17) 
/**@brief 返回键 */
#define GB_VK_BACK		(1024+18) 
/**@brief 向上键 */
#define GB_VK_UP		(1024+19) 
/**@brief 向下键 */
#define GB_VK_DOWN		(1024+20)
/**@brief 左键 */
#define GB_VK_LEFT		(1024+21) 
/**@brief 右键 */
#define GB_VK_RIGHT	(1024+22) 
/**@brief #键 */
#define GB_VK_SHARP	'#' 
/**@brief *键 */
#define GB_VK_STAR		'*' 
#endif

/**@brief 设置拼音音节和数字区的屏幕宽度 */
#define CFG_SYLLABLE_WTH		(2048+1)
/**@brief 设置系统相关的字符串宽度的回调函数 */ 
#define CFG_GETSTREXTENT_FUNC	(2048+2) 
/**@brief 初试化引擎操作 */
#define CFG_INITENGINE			(2048+3) 
/**@brief 设置活动的输入法 */
#define CFG_SETACTIVEIM			(2048+4) 
/**@brief 重置活动的输入法 */
#define CFG_RESETACTIVEIM		(2048+5) 
/**@brief 设置是否可改变输入法 */
#define CFG_ISCANCHANGEIM		(2048+6) 
/**@brief 设置显示的拼音音节间的间隔宽度 */
#define CFG_SYLLABLE_SPLIT		(2048+7) 
/**@brief 设置只能输入中文 */
#define CFG_SETCHINESE_LIMIT	(2048+8) 
/**@brief 设置只能输入英文 */
#define CFG_SETENGLISH_LIMIT	(2048+9)
/**@brief 设置系统相关的UNICODE字符串宽度的回调函数 */
#define CFG_GETWSTREXTENT_FUNC	(2048+10) 

/**@brief 选择选择拼音音节*/
#define CFG_SELECT_SYLLABLE		(2048+11) 

/**@brief 根据传入的汉字得到联想词*/
#define  CFG_GET_ASSOCIATE    (2048+12) 

struct GBIM
{
	unsigned char indentifyName;	
	void *pIS;
	struct GBIM *pSwitchItem;
	unsigned char status;
	void (*GBIMOnUp)(struct GBIM *p,void *param);
	void (*GBIMOnDown)(struct GBIM *p,void *param);
	void (*GBIMOnLeft)(struct GBIM *p,void *param);
	void (*GBIMOnRight)(struct GBIM *p,void *param);
	void (*GBIMOnNumS)(struct GBIM *p,void *param,int realkey);
	void (*GBIMOnNumL)(struct GBIM *p,void *param,int realkey);
	void (*GBIMOnBlank)(struct GBIM *p,void *param);
	void (*GBIMOnBack)(struct GBIM *p,void *param);
	void (*GBIMSwitchIM)(struct GBIM *p,void *param);
	void (*GBIMReset)(struct GBIM *p,void *param);
	void (*GBIMDelete)(struct GBIM *p,void *param);
	void (*GBIMInit)(struct GBIM *p,void *param);
	void (*GBIMPrepareShowMessage)(struct GBIM *p,void *param);
	int (*GBIMCanPageUp)(struct GBIM *p,void *param);
	int (*GBIMCanPageDown)(struct GBIM *p,void *param);
	int (*GBIMCanPrevHeader)(struct GBIM *p,void *param);
	int (*GBIMCanNextHeader)(struct GBIM *p,void *param);	
	void (*GBIMOnStar)(struct GBIM *p,void *param);
#ifdef _SHOW_DEBUG
	void (*GBIMDisplay)(struct GBIM *p,void *param);
#endif
	void (*GBIMOnSharp)(struct GBIM *p,void *param);
	void (*GBIMOnReturn)(struct GBIM *p,void *param);
};

typedef struct GBIM GBIM;

#define MAX_INPUTLTH			32
#define MAX_COMPLTH				5
#define MAX_SYLLABLELTH			20
#define MAX_CANDS_BUF           64

typedef int (*pfGetStrLth_T)(const char *pstr);

/**@brief 与MMI层交互结构体 */
typedef struct
{
	GBIM*			GBIMS;/**< 指向活动输入法 */
	int 			activeIM;/**< 当前输入法类型所属数组的索引 */
	int				prevIM;/**< 前一输入法类型所属数组的索引 */
	GBInputMode		activeMode;/**< 活动的输入法 */
	GBSetting		gbSet;/**< MMI需要传入的数据指针和配置的结构 */
	int				isShowCandWin;/**< 当前候选窗口的显示状态 */
	int				isSelectedCand;/**< 当前是否选中候选字 */
	int				isCanChangeIM;/**< 当前是否可以修改输入法 */
	int				isChineseOnly;/**< 当前是否只能输入汉字 */
	int				isEnglishOnly;/**< 当前是否只能输入英文 */
	int				isCandHelpTips;/**< 当前是否显示帮助文本 */
	int				noHandleKeyEvent;/**< 输入法引擎是否处理(内部使用) */
	int				shouldUpScreen;/**< 当前是否应该上屏*/
	int				option;/**< 智能英文候选输出格式 */
	int				syllableAreaWth;/**< 拼音音节和数字区的屏幕宽度 */
	int				syllableSplit;/**< 显示的拼音音节间的间隔宽度 */
	pfGetStrLth_T 	pfGetStrLth;/**< 系统相关的字符串宽度的回调函数 */
	pfGetStrLth_T 	pfGetWStrLth;/**< 系统相关的字符串宽度的回调函数 */
	unsigned short 	upscreenStr[MAX_UPSCREEN];/**< shouldUpScreen为真时,存放需要上屏的字符串 */
	int	  			upscreenLen ;/**< shouldUpScreen为真时,存放需要上屏的字的个数 */
	char 			fullOutput[MAX_INPUTLTH*2];/**< 数字区内码串 */
	char 			stroke[MAX_INPUTLTH*2];/**< 笔画区内码串 */
	unsigned short 	components[MAX_COMPLTH];/**< 部首区内码串 */
	int 			componentsLth;/**< 部首个数	 */
	int 			syllableSelIndex;/**< 拼音音节的索引，为0时表示当前选中数字区 */
	char*			syllable[MAX_SYLLABLELTH];/**< 拼音音节 */
	int 			syllableNum;/**< 拼音音节的个数 */
	int 			selectIndex;/**< 当前选择的候选字词的索引 */
	unsigned short  cands[MAX_CANDS_BUF+1][MESSAGE_MAX_LENGTH];/**< 候选字词 */
	int 			candsNum;/**< 候选字词的个数 */
	GBIME_MODE gbImeMode;
}GBIMWT;

typedef struct 
{
    unsigned short key;
    unsigned short candidates[32];
}KeyCandidates;

#ifdef __cplusplus 
extern "C"{
#endif
extern GBIMWT GBIMW;
#ifdef __cplusplus 
}
#endif

/**  @brief MMI调用的接口函数
*  @param[int] realkey \n
				-CFG_SYLLABLE_WTH		设置拼音音节和数字区的屏幕宽度 \n
				-CFG_GETSTREXTENT_FUNC	设置系统相关的字符串宽度的回调函数,param为传入系统相关的函数指针 \n
				-CFG_INITENGINE			初始化引擎操作,param的值为GBIME_INIT的结构指针 \n
				-CFG_SETACTIVEIM		设置活动的输入法,param的值为enum GBInputMode中的值 \n
				-CFG_RESETACTIVEIM		重置活动的输入法,param参数暂时无意义 \n
				-CFG_ISCANCHANGEIM		设置是否可改变输入法,param为1表示当前输入环境可改变输入法,0为不可改变 \n
				-CFG_SYLLABLE_SPLIT		设置显示的拼音音节间的间隔宽度 \n
				-CFG_SETCHINESE_LIMIT	设置只能输入中文,param为1表示只能输入中文,0为取消该限制 \n
				-CFG_SETENGLISH_LIMIT	设置只能输入英文,param为1表示只能输入英文,0为取消该限制 \n
				-CFG_SELECT_SYLLABLE	拼音输入模式下直接选择当前页的第param个拼音音节，param区间为[0,syllableLen),非法param无影响 \n

*  @param[unsigned long] param 相对应参数realkey而设置的参数
*  @return 1处理,0不处理
*  @note 这是中间层与MMI层唯一通讯的函数
*  @author hxc
*/
int HandleKeyEvent(int realkey,unsigned long param);

#endif

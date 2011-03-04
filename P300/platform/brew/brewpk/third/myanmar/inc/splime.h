#ifndef __spl_ime_eng_h_
#define __spl_ime_eng_h_



/** \enum SplKeyType
	\brief key type defination
 */
enum SplKeyType
{
	SPKT_Down,
	SPKT_Up,
	SPKT_LongPress
};


/** \enum SplKey
	\brief key defination
 */
enum SplKey
{
/* char key */
	SPKEY_NULL						= 0,

	SPKEY_0 						= '0',
	SPKEY_1							= '1',
	SPKEY_2							= '2',
	SPKEY_3							= '3',
	SPKEY_4							= '4',
	SPKEY_5							= '5',	
	SPKEY_6							= '6',	
	SPKEY_7							= '7',
	SPKEY_8							= '8',
	SPKEY_9							= '9',

	SPKEY_Star						= '*',			
	SPKEY_Sharp						= '#',

	SPKEY_Dot                       = '.',

	SPKEY_AT						= '@',

	SPKEY_FUNC_NULL					= 0x80,
	SPKEY_0L,
	SPKEY_1L,
	SPKEY_2L,
	SPKEY_3L,
	SPKEY_4L,
	SPKEY_5L,
	SPKEY_6L,
	SPKEY_7L,
	SPKEY_8L,
	SPKEY_9L,

/* function key */
	SPKEY_Left,
	SPKEY_Right,
	SPKEY_Up,
	SPKEY_Down,
	SPKEY_OK,
	SPKEY_Back,
	SPKEY_Return,

    SPKEY_Num
} ;

/** 
    SIME internal return values definition
 */
enum SIMEReturn
{
	SMR_OK                      = 0x00,	///< OK, event handled
	SMR_Ignored					= 0x01,	///< event ignored

	SMR_Failed					= 0x10,	///< return errors of MMI internal begin with this value
	SMR_EngineNotInit,					///< engine not initialized
	SMR_NoEngine,						///< no engine
	SMR_NullEvent,						///< pointer to event is null!
	SMR_InvalidEventType,				///< invalid event type! see \ref EventType
	SMR_NullInitData,					///< pointer to init data is null when event type is SplImeInit
	SMR_InvalidInputMode,				///< invalid input mode, see \ref SplInputMode
	SMR_UnsupportedLanguageType,		///< unsupported language type
	SMR_InvalidKeyType,				    ///< invalid key type, see \ref SplKeyType
	SMR_InvalidKey,						///< invalid key, see \ref SplKey
    SMR_InvalidParam,                   ///< 传入参数正确

	SMR_NUM
} ;



/* ----------------------------------------------------------------------------------- */
/* 初始化数据 */
/* ----------------------------------------------------------------------------------- */
typedef struct __tag_InitData
{
	void         *  imeData;

	unsigned char   reserved1;
	unsigned char   reserved2;
	unsigned char   reserved3;
	unsigned char   reserved4;
	
} SplInitData;


typedef long (*Spl_GetStringWidthAFunc) (const char *);
typedef long (*Spl_GetStringWidthWFunc) (const unsigned short *);

/* ----------------------------------------------------------------------------------- */
/* UI设置 */
/* ----------------------------------------------------------------------------------- */
typedef struct __tag_UIInfo
{
	Spl_GetStringWidthAFunc	    fpGetStrWidthA;     //  获得char字符串宽度
	Spl_GetStringWidthWFunc	    fpGetStrWidthW;     //  宽字符串的宽度

	unsigned long               candidateWidth;     // 候选栏宽度
	unsigned short              candMinSpacing;     // 每个候选之间的最小间隔
    unsigned short              padding;

}SplUIInfo;


/* ----------------------------------------------------------------------------------- */
/* 输出数据 */
/* ----------------------------------------------------------------------------------- */
typedef struct __tag_OutputInfo
{
	unsigned short      *       inputString;     /**< 数字区内码串 */

	unsigned short      *       candidates[20]; /**< 候选字词 */
	unsigned short              candidatesNum;  /**< 候选的个数 */
	unsigned short              candidateIndex; /**< 当前高亮的候选项索引 */	

	unsigned char               isShowIMWin;    /**< 是否显示输入法窗口 */
    unsigned char               isSelectedCand; /**< 当前是否进入候选选择状态 */	
	unsigned char               isCanUpScreen;  /**< 在当前状态下是否可以上屏*/
	unsigned char               padding;

	unsigned short              upscreenLen;    /**< isCanUpScreen为真时,存放需要上屏的字符个数 */	
    unsigned short              padding2;
	unsigned short      *       upscreenStr;    /**< isCanUpScreen为真时,存放需要上屏的字符串 */

	unsigned char				isAssociateMode;        // 是否处于中文联想模式
	unsigned char				isChnInterpunctionMode; // 是否处于中文标点模式		
	unsigned char				isEnInterpunctionMode;  // 是否处于英文标点模式
	unsigned char				isSwitch;               // 是否处于中英文无缝切换状态
	
	unsigned char				isShowDownArrow;        // 判断候选页是否能下翻
	unsigned char				isShowUpArrow;          // 判断候选页是否能上翻
	unsigned char				isShowLeftArrow;        // 判断是否能左移选择候选
	unsigned char				isShowRightArrow;       // 判断是否能右移选择候选

} SplOutputInfo ;



/* ----------------------------------------------------------------------------------- */
/* 全局数据 */
/* ----------------------------------------------------------------------------------- */
typedef struct __tag_ImeGlobals
{
	SplInitData		initData;
	SplUIInfo		uiInfo;
	SplOutputInfo   outputInfo;
	
} SplImeGlobals;


#ifdef __cplusplus
extern "C" {
#endif

extern SplImeGlobals    g_SplImeGlobals;

extern enum SIMEReturn SplImeInit(void);
extern enum SIMEReturn SplImeDeinit(void);
	
extern enum SIMEReturn SplImeProcessKey(enum SplKey uKey, enum SplKeyType type);

extern enum SIMEReturn SplImeHandleInputEvent(int nType, unsigned long param1, unsigned long param2);
	
#ifdef __cplusplus
}
#endif

#endif
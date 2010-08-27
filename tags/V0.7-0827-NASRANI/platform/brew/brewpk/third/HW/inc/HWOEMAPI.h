/*------------------------------------------------------------
  Copyright (c) 2007 汉王科技股份有限公司
  
  文件名称:
		HWAPI.h

  创建人：
		lzh@hanwang.com.cn		

  创建日期:
		2007年5月23日

  更新日期:
		2007年08月08日
		2007年08月13日
		2007年09月19日
		2007年10月26日
		2008年11月18日
  关键字:
		PDA版

运算符号
002E .      00B1 +-     002B +      002D -
00D7 X      00F7 ÷     003D =      0025 %
2030 ‰            

中文标点
201C 〝           201D 〞      3001 、   3002 。        
FF0C ，    FF1A ：   FF1B ；   FF01 !
FF1F ？    2026 …

Chinese Ext Punc 
FF5E ～    2018‘   2019  ＇   3008 ＜
3009 ＞    300A《        300B 》    3014 ［
3015 ］    FF08 （  FF09 ）    FF5B ｛
FF5D ｝


Eng Punc
002C ,    002E .     0022 "    0027 '
003A :    003B ;     0021 !    003F ?

Ext Punc 
007E ~    005B [     005D ]    0028 (
0029 )    007B {     007D }


Comm Symbol
0023 #    0025 %     0026 &    002A *
002B +    002D -     002F /    003C <
003D =    003E >     0040 @    0024 $
FFE1 ￡   FFE5 ￥    20AC 

Ext Sym
005C \    005E ^     005F _    007C 丨       
0060 `    00A4 ¤         FFE0 ￠   2116 №
203B ※   2103 ℃    00A7 §   00B7 ·
3005 


------------------------------------------------------------*/
#ifndef __HWAPI_H
#define __HWAPI_H

#define HANWANG_SIMPLIFIED_CHINESE		1	// 简体中文版
#define HANWANG_INTEGRATED_DICT			1	// 是否代码字典集成
#define HANWANG_LITTLEENDIAN			1	// 字节序 LittleEndian

#ifndef _WCHAR_T_DEFINED
	typedef unsigned short wchar_t;
	#define _WCHAR_T_DEFINED
#endif

/* -------------------识别语言------------------------ */
	// 简体版
	#define HWLANG_Simp_Chinese		0x1000	// 简体中文
	#define HWLANG_GB2312			0x1001	// GB2312标准 基本字符集
/* -------------------中文引擎识别字符集------------------ */
	// 简体版
	#define ALC_CHN_STANDARD	0x00000001	// 标准汉字（基本字符集：6721字 大字符集：20912字）
	#define ALC_CHN_RADICAL		0x00000002	// 特殊偏旁（基本字符集：42字　大字符集：92字）
	#define ALC_CHN_VARIANT		0x00000004	// 繁异转简
	#define ALC_CHN_CURSIVE		0x00000008	// 行草书法（简体）

	#define OUT_COMPATIBILITY	0x00000040	// 强制输出兼容码字（一字多码）

#define ALC_LATIN_LOWERCASE		0x00000100	// 小写拉丁字母 a-z
#define ALC_LATIN_UPPERCASE		0x00000200	// 大写拉丁字母 A-Z

#define ALC_NUMERALS			0x00000400	// 数字（0-9）
#define ALC_OPERATORS			0x00000800	// 运算符

#define ALC_CHN_COMMON_PUNC		0x00001000	// 中文常用标点
#define ALC_CHN_EXTEND_PUNC		0x00002000	// 中文扩展标点

#define ALC_ENG_COMMON_PUNC		0x00004000	// 英文常用标点
#define ALC_ENG_EXTEND_PUNC		0x00008000	// 英文扩展标点

#define ALC_COMMON_SYMBOLS		0x00010000	// 常用符号
#define ALC_EXTEND_SYMBOLS		0x00020000	// 扩展符号

#define ALC_HIRAGANA			0x00040000	// 日文平假名（83字，编码范围0x3041-0x3093）
#define ALC_KATAKANA			0x00080000	// 日文片假名（86字，编码范围0x30A1-0x30F6）

#define ALC_GESTURE				0x00400000	// 控制手势（空格 制表符 回删 回车）


/* ----------------输出自动转换-------------------------- */
#define	OUT_INVERT				0x10000000	// 即时大小写颠倒或者即时单多字切换（引擎内部使用）
#define OUT_UPPERCASE			0x20000000	// 输出大写字母
#define OUT_LOWERCASE			0x40000000	// 输出小写字母
#define OUT_FULLWIDTH			0x80000000	// 输出全角字符

#define ALC_LATIN		(ALC_LATIN_LOWERCASE | ALC_LATIN_UPPERCASE)	// 拉丁字母
#define ALC_KANA		(ALC_HIRAGANA | ALC_KATAKANA)				// 日文假名

#define ALC_CHN_PUNC	(ALC_CHN_COMMON_PUNC | ALC_CHN_EXTEND_PUNC)	// 中文标点
#define ALC_ENG_PUNC	(ALC_ENG_COMMON_PUNC | ALC_ENG_EXTEND_PUNC)	// 英文标点
#define ALC_PUNCTUATION	(ALC_CHN_PUNC | ALC_ENG_PUNC)				// 标点

#define ALC_SYMBOLS		(ALC_COMMON_SYMBOLS | ALC_EXTEND_SYMBOLS)	// 符号

/* ----------------控制手势编码定义---------------------- */
#define CODE_BACKSPACE		0x0008	// 回删
#define CODE_TAB			0x0009	// Tab
#define CODE_RETURN			0x000D	// 回车
#define CODE_SPACE			0x0020	// 空格


//---------------常量定义----------------------------
#define HWRE_MAXCANDNUM			8			// 可提供的最大候选字个数
#define HWRE_MAXSEGNUM			20			// 多字识别下，最多分割块数

// 引擎需要内存空间
#define HWRE_CHAR_RAMSIZE		(17*1024)	// 单字识别所需运算空间大小
#define HWRE_MULTICHAR_RAMSIZE	(33*1024)	// 多字识别所需运算空间大小
#define HWRE_PHRASE_RAMSIZE		(60*1024)	// 中文短句识别所需要的运算空间大小
#define HWRE_CANDBUFSIZE		1024		// 识别结果存储区大小


//------------笔迹结构定义-------------------
#define STROKEENDMARK		(-1)		// 定义笔画结束标记值

typedef struct tagTHWPoint
{
	short x;	//笔迹 X 坐标
	short y;	//笔迹 Y 坐标
} THWPoint;

//------------书写框结构定义-------------------
typedef struct tagTHWFrame{
	short left;		// 书写框左上角 X 坐标
	short top;		// 书写框左上角 Y 坐标
	short width;	// 书写框宽度
	short height;	// 书写框高度
} THWFrame;

/* -------------------识别模式---------------- */
#define HWMODE_SINGLECHAR		1	// 单字识别
#define HWMODE_MULTICHAR		2	// 多字识别
#define HWMODE_WORDSTRING		3	// 中文短句识别


//------------识别属性定义------------------
typedef struct tagTHWAttribute
{
	unsigned short	wMode;		// 识别模式
	unsigned short	wLanguage;	// 识别语言
	unsigned long	dwRange;	// 识别范围
	int				iSlantScope;	// 倾斜角度范围，如果为0，则关闭倾斜字符识别
	int				iCandidateNum;	// 候选字个数
	unsigned char*	pRam;		// 运算空间，要求起始地址4字节对齐
	unsigned char*  pRom;		// 识别字典。如果字典集成，则为NULL。
	unsigned char*	pAdaptWorkSpace;	// 自适应学习空间
	unsigned char*	pMacroWorkSpace;	// 图形宏定义存储空间	
	unsigned char*	pSystemDict;	// 中文系统词库 或者 英文系统词库。
	unsigned char*	pUserDict;		// 中文用户词库 或者 英文用户词库
	unsigned char*	pLangModel;		// 语言模型字典（用于中文短句识别）
	THWFrame*		pFrame;			// 书写框，如果为NULL，则为全屏书写
} THWAttribute;

//-------------输出结构定义---------------------------
#define OUTTYPE_MARCO		100	// 输出结果是文字宏

//-------------错误代码定义---------------------------
#define HWERR_SUCCESS                    0		// 成功
#define HWERR_INVALID_FUNCTION           (-1)	// 参数错误
#define HWERR_NOT_ENOUGH_MEMORY          (-2)	// 内存不足
#define HWERR_INVALID_DATA               (-3)	// 无效的数据
#define HWERR_DATA_EXIST				 (-4)	// 数据已经存在

// 单词查找返回值定义
#define HWLX_FOUND_NONE			0  // 字符串没有匹配词
#define HWLX_FOUND_PREFIX		1  // 字符串没有匹配词，但存在以该字符串开始的词
#define HWLX_FOUND_SAMENESS		2  // 存在与字符串完全匹配的词
#define HWLX_FOUND_MORE			3  // 存在与字符串匹配的词，同时存在以该字符串开始的词

//----------------导出函数定义-----------------------
#ifdef __cplusplus
extern "C" {
#endif

// ------------------ 识别相关函数 -------------------------
/*
功能：笔迹识别
参数说明：
	pTrace：笔迹数据，其格式为(x1,y1),(x2,y2),...,(STROKEENDMARK,0),...,(STROKEENDMARK,0),
			(STROKEENDMARK,STROKEENDMARK)，其中(STROKEENDMARK,0)为笔画结束标志，
			(STROKEENDMARK,STROKEENDMARK)为字结束标志。
	iTraceNum：笔迹点数。如果为-1，则表示由字结束标记决定笔迹点数。
	pAttr：识别属性。
	pResult：识别结果。其缓冲区大小为 HWRE_CANDBUFSIZE。	
返回值：
	HWERR_SUCCESS：成功
	HWERR_INVALID_FUNCTION：参数错误
	HWERR_INVALID_DATA：无效的数据
	> 0：如果多单词识别下存放结果的缓冲区空间不足，则pResult中不存放结果，而直接返回分割块数目
识别结果存放结构：
	struct
	{
		unsigned short wType = 1;			// 输出类型，用于单字识别
		unsigned short wCandNum;			// 候选字个数
		unsigned long pCands[wCandNum];		// 候选字符集，UCS4编码
	};

	struct
	{
		unsigned short wType = 2;			// 输出类型，用于多字识别
		unsigned short wBlockNum;			// 分割块数
		unsigned short wCandNum;			// 候选数目
		unsigned short wSeperate;			// 分隔符标记（预留）
		unsigned long pCands[wBlockNum][wCandNum];	// 候选字符集，UCS4编码
	};

	struct
    {
        unsigned short wType = 3;			// 输出类型，适合短句
        unsigned short wCandNum;			// 候选数目
        wchar_t pCands[HWRE_CANDBUFSIZE / 2 - 2]; // unicode字符串
    }

*/
int HWRE_Recognize( THWPoint* pTrace, int iTraceNum, const THWAttribute* pAttr, unsigned short* pResult );

#ifdef __cplusplus
}
#endif

#endif

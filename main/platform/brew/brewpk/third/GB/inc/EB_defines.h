//Last Modify Date : 2003-5-6 17:02:29
//:EB_defines.h : ErBiDigital public defines.

#ifndef __EB_DEFINES_H_
#define __EB_DEFINES_H_

//
#define EB_NULL						0	//空

#define EB_TRUE						1
#define EB_FALSE					0

//Returns
#define EB_OK						0	//OK，成功
#define EB_EMPTY					100	//没有符合条件的候选字[词]
#define EB_FAILED					-1	//失败

#define EB_NOTINIT					-2	//没有调用EBStartup()!
#define EB_PEBIS_NULL				-3	//pEBIS为空.
#define EB_METHOD_INVALID			-4	//输入法类型错误
#define EB_DESIRE_LEN_INVALID		-5	//用户需要的长度错误.
#define EB_KEY_INVALID				-6	//调用EBTestKey时的按键不合法
#define EB_STATUS_ERROR				-7	//pEBIS->status不正确，不能调用.
#define EB_ASSOC_ISN_ERROR			-8	//AssociateISN == EB_NULL

/*Invalidate函数返回的错误值*/
#define EB_INVALID_CHAR             -9	//非法的字符
#define EB_INVALID_TONE             -10	//非法的音调
#define EB_INVALID_INPUT_LEN        -11	//输入串的长度过长
#define EB_FIRST_IS_WILDCARD        -12	//第一个输入的字符是通配符
#define EB_NO_MATCH_WORD            -13	//没有匹配的字
#define EB_NO_MATCH_COMPONENT       -14	//没有匹配的部首
#define EB_INVALID_SYLLABLE         -15	//非法的音节
#define EB_INVALID_SYLLABLE_INDEX   -16	//非法的音节下标
#define EB_INPUT_STRING_NULL        -17	//输入串为空
/*Starpup函数返回的错误*/
#define EB_DATA_FILE_NOT_FOUND      -18	//找不到数据文件
#define EB_INVALID_DATA_FORMAT      -19	//非法的数据格式
#define EB_FAILED_TO_ALLOC_MEMORY   -20	//
#define EB_MISS_DATA                -21	//缺少数据
#define EB_STROKE_LEN_MISMATCH      -22	//引擎的笔画数与数据的笔画数不一样
#define EB_INVALID_SYLLLABLES_COUNT -23
#define EB_SH_BUFFER_TO_SMALL       -24	//调用EBShDataInit的时候, 缓冲区的长度不足
#define EB_SH_CORRUPT_DATA          -25	//错误的自造词数据
#define EB_SH_INVALID_ADDRESS       -26	//自造词数据的头地址不能被4整除
//#define EB_SH_PHRASE_EXISTS         -27 //EBShCheckPhrase的一个可能返回值，
#define EB_SH_NOT_ENOUGH_SPACE      -28
//#define EB_SH_PHRASE_EXISTS_HEADER  -29 //EBShAddPhrase的一个可能返回值，
#define EB_SH_NOT_INIT              -30	//自造词和词语调频数据没有初始化
//#define EB_SH_WORD_EXISTS           -31 //自造词已经在词表中存在
#define EB_SH_INVALID_WORD          -32	//自造词数据中包涵非法的字母
#define EB_SH_SPACE_NOT_ENOUGH      -33	//空间不足
#define EB_SH_DUPLICATE             -34	//数据已经存在一个完全相同的单词
#define EB_SH_PHRASE_INVALID_LENGTH -35	//自造词太长了，超过了10个汉字
#define EB_TO_INPUT_PHRASE          -36

#define EB_MIN_SH_DATA_LEN          5120	//使用词语调频和自造词功能的最小空间

#define FOUND_EXACTLY               1	//
#define FOUND_PART                  2
#define FOUND_NONE                  0


//Engine可能处理的按键[for EBTestKey]
#define EB_KEYS					"0123456789#"
#define EB_PY_KEYS				"0123456789#"	//音
#define EB_WILDCARD				'*'


//EBInputStruct Constant
#define EB_INPUT_METHOD_NUM		3	//输入法数量
#define EB_INPUT_METHOD_PINYIN     1	//拼音
#define EB_INPUT_METHOD_BPMF       EB_INPUT_METHOD_PINYIN	//注音
#define EB_INPUT_METHOD_STROKE     2	//笔画

#define EB_INPUT_METHOD_YIN_STROKE 3	//笔画带音

#define EB_INPUT_MAX_LEN	 		32	//最大输入长度

#define	EB_TONE_1					1	//音调:
#define EB_TONE_2					2	//音调:
#define	EB_TONE_3					3	//音调:
#define EB_TONE_4					4	//音调:
#define	EB_TONE_5					5	//音调: 轻

#define EB_CANDIDATES_MAX_LEN		128	//侯选字最大数目

#define EB_SYLLABLE_MAX_SELECT		32	//同一输入可能的准确音节最大数量
#define EB_SYLLABLE_MAX_LEN			12	//音节最大长度

#define EB_COMPONENT_MAX_LEN		5	//部首显示数目

#ifndef EB_PHRASE_INPUT_SPLIT
#define EB_PHRASE_INPUT_SPLIT       '#'
#endif


#define EB_STATUS_ASSOCIATE			0x00000001	//AssociateMode
#define EB_STATUS_INTERPUNCTION	    0x00000002	//interpunction mode
#define EB_STATUS_PAGEDOWN			0x00000004	//PageDown
#define EB_STATUS_PAGEUP			0x00000008	//PageUp

#endif				//__EB_DEFINES_H_

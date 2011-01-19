//Create at 2003-5-20 21:07:48 by abing
#ifndef __EBEN_DEFINES_H_
#define __EBEN_DEFINES_H_

#define EN_INPUT_MAX_LEN                        31

#define EN_WORD_MAX_LEN                         EN_INPUT_MAX_LEN

#define EN_CANDIDATE_MAX_COUNT                  10
#define EN_MAX_DESIRE_COUNT                     EN_CANDIDATE_MAX_COUNT
#define EN_MAX_DESIRE_LEN                       255

#ifndef LATIN_ENCODE_LENGTH
#define LATIN_ENCODE_LENGTH                     2
#endif

#if LATIN_ENCODE_LENGTH == 2
typedef unsigned short  GBCHAR;
typedef unsigned short  GBCHARU;
#define gb_strncpy          WSTRNCOPYN
#define gb_strlen           WSTRLEN
#define gb_strcpy           WSTRCPY
#define gb_strcmp           wcscmp
#define gb_strchr           WSTRCHR 
#define gb_strstr           wcsstr //use in generator
#define gb_strncmp          wcsncmp
#define gb_strcat           wcscat
#define gb_strcspn          wcscspn
#define gb_strtok           wcstok //use in generator
#define gb_strspn           wcsspn //use in generator
#define gb_strpbrk          wcspbrk //not use
#define gb_sprintf          swprintf //use in generator
#define gb_scanf            wscanf //use in generator
#define gb_atoi             _wtoi //use in generator
#define GBSTR(a)            L##a
#else
typedef char            GBCHAR;
typedef unsigned char   GBCHARU;
#define gb_strncpy          strncpy
#define gb_strlen           strlen
#define gb_strcpy           strcpy
#define gb_strcmp           strcmp
#define gb_strchr           strchr
#define gb_strstr           strstr
#define gb_strncmp          strncmp
#define gb_strcat           strcat
#define gb_strpbrk          strpbrk
#define gb_strcspn          strcspn
#define gb_strtok           strtok
#define gb_strspn           strspn
#define gb_atoi             atoi
#define gb_sprintf          sprintf
#define gb_scanf            scanf
#define GBSTR(a)            a
#endif
#define DEFINED_GBCHAR      1

typedef GBCHAR EN_WORD[EN_INPUT_MAX_LEN + 1];

#define EBEN_OK                                 0

#define EBEN_FAILED                             -1

#define EBEN_INVALID_INDEX                      -100

#define EBEN_NULL                               0

#define EBEN_NOTINIT					-2 //没有调用EBStartup()!

#define EBEN_PEBIS_NULL				    -3 //pEBIS为空.

#define EBEN_INVALID_INPUT_LEN          -4 //

#define EBEN_INVALID_DESIRE_LEN         -5

#define EBEN_INVALID_STATUS             -7

#define EBEN_NO_MATCH                   -8 //没找到匹配串

#define EBEN_INVALID_INVOKE             -9 //非法调用

#define EBEN_NO_WORD                    -10 //单词表里面没有任何单词

#define EBEN_INVALID_DESIRE             -12

#define EBEN_INVALID_DESIRE_COUNT       -13

#define EBEN_NO_DATA_FILE               -14

#define EBEN_INVALID_DATA_FORMAT        -15

#define EBEN_FAILED_ALLOC_MEM           -16

#define EBEN_CORRUPTED_DATA_FILE        -17

#define EBEN_MISS_DATA                  -18

#define EBEN_INVALID_CHAR               -19

#define EBEN_WORD_TOO_LONG              -20

#define EBEN_NULL_WORD                  -21


#define EBEN_SH_NO_SPACE                -22

#define EBEN_WORD_EXISTS                -23

//数据中的字符集有问题
#define EBEN_INVALID_VALIDCHAR          -24

//数据与引擎版本不匹配
#define EBEN_DATA_MISMATCH              -25

//自造词数据错误
#define EBEN_SH_BUFFER_TO_SMALL           -24 //调用EBShDataInit的时候, 缓冲区的长度不足
#define EBEN_SH_CORRUPT_DATA              -25 //错误的自造词数据
#define EBEN_SH_INVALID_ADDRESS           -26 //自造词数据的头地址不能被4整除
#define EBEN_SH_NOT_INIT                  -30 //自造词和词语调频数据没有初始化
#define EBEN_SH_INVALID_WORD              -32 //自造词数据中包涵非法的字母
#define EBEN_SH_SPACE_NOT_ENOUGH          -33 //空间不足
#define EBEN_SH_DUPLICATE                 -34 //已经存在一个完全相同的单词
#define EBEN_SH_PHRASE_INVALID_LENGTH     -35 //自造词太长了

#define CLEAR_CANDIDATES(pENIS)         \
    CLEAR_ARRAY((pENIS)->candidates);   \
    (pENIS)->candidateCount = 0; 

#define CLEAR_INPUT(pENIS)              \
    CLEAR_ARRAY((pENIS)->inputString);  \
    (pENIS)->inputLen = 0;\
    CLEAR_ARRAY((pENIS)->header);\
    pENIS->headerIsAWord = 0;

#define CLEAR_ENIS(pENIS)               \
    CLEAR_CANDIDATES(pENIS);            \
    CLEAR_INPUT(pENIS);

/*
buf     : (in)will be the address of DIY words' buffer
addin   : (in)will be the addin param pass from EnSelfHelpWordsInit
*/
typedef void (* fpUpdateSelfHelpData)(const void * buf, void * addin);
/*
buf         : (in/out)the address of the point of DIY words' buffer 
capacity    : (in/out)the address of the capacity, the value of *capacity is the length of buffer.
increment   : (in) the increment at least should be extent.
addin       : (in)will be the addin param pass from EnSelfHelpWordsInit
 */
typedef void (* fpExtentBuf)(GBCHAR * * buf, unsigned int * capacity, unsigned int increment, void * addin);


#define EN_WORD_EXISTS              2
#define EN_WORD_EXISTS_AS_HEADER    1
#define EN_WORD_NO_EXISTS           0
#define EN_WORD_INVALID             -1

//<ENInputStruct::outputStatus>
//以下定义是解析ENInputStruct::outputStatus的定义，同时也用于GBLtGetDesireOutput
//在输出字母候选的时候不要根据语言的特性进行候选的顺序调整
//一般使用于单个字母的输入法的实现中
#define EN_OPTION_DONT_ADJUST_CHAR_ORDER        0x01 
//各种输出的选项
#define EN_OPTION_OUTPUT_FLAG                   0x0e // 8种可能 0 2 4 6 8 10 12 14 
#define EN_OPTION_OUTPUT_NORMAL                 0x0000 //输出默认的大小状态的单词
#define EN_OPTION_OUTPUT_FCC                    0x02 //输出首字母大写的候选单词
#define EN_OPTION_OUTPUT_CAPITAL                0x04 //输出大写的候选单词
#define EN_OPTION_OUTPUT_LOWER                  0x06 //输出小写的候选单词
#define EN_OPTION_OUTPUT_TOGGLE_FCC             0x08 //改变当前输出的大小状态
#define EN_OPTION_OUTPUT_CAPITAL_CHAR           0x0A //单个字母的时候只输出大写字母，其他时候输出普通的单词
#define EN_OPTION_OUTPUT_LOWER_CHAR             0x0C //单个字母的时候只输出小写字母，其他时候输出普通的单词
//</ENInputStruct::outputStatus>

//<
#define EN_OUTPUT_NORMAL                        EN_OPTION_OUTPUT_NORMAL //正常状态
#define EN_OUTPUT_FCC                           EN_OPTION_OUTPUT_FCC //首字母大写
#define EN_OUTPUT_CAPITAL                       EN_OPTION_OUTPUT_CAPITAL //大写
#define EN_OUTPUT_LOWER                         EN_OPTION_OUTPUT_LOWER //小写
#define EN_OUTPUT_TOGGLE_FCC                    EN_OPTION_OUTPUT_TOGGLE_FCC //切换首字母的大小写
//
#define EN_STATUS_SELF_HELP                     0x01 //引擎开始自造词状态



//language definition
#define GB_LANG_Afrikaans      				0x36
#define GB_LANG_Albanian       				0x1C
#define GB_LANG_Arabic         				0x01
#define GB_LANG_Basque         				0x2D
#define GB_LANG_Belarusian     				0x23
#define GB_LANG_Bulgarian      				0x02
#define GB_LANG_Catalan        				0x03
#define GB_LANG_Chinese        				0x04
#define GB_LANG_Croatian       				0x1A
#define GB_LANG_Czech          				0x05
#define GB_LANG_Danish         				0x06
#define GB_LANG_Dutch          				0x13
#define GB_LANG_English        				0x09
#define GB_LANG_Estonian       				0x25
#define GB_LANG_Faeroese       				0x38
#define GB_LANG_Farsi          				0x29
#define GB_LANG_Finnish        				0x0B
#define GB_LANG_French         				0x0C
#define GB_LANG_Gaelic         				0x3C
#define GB_LANG_German         				0x07
#define GB_LANG_Greek          				0x08
#define GB_LANG_Hebrew         				0x0D
#define GB_LANG_Hindi          				0x39
#define GB_LANG_Hungarian      				0x0E
#define GB_LANG_Icelandic      				0x0F
#define GB_LANG_Indonesian     				0x21
#define GB_LANG_Italian        				0x10
#define GB_LANG_Japanese       				0x11
#define GB_LANG_Korean         				0x12
#define GB_LANG_Latvian        				0x26
#define GB_LANG_Lithuanian     				0x27
#define GB_LANG_Macedonian     				0x2F
#define GB_LANG_Malaysian      				0x3E
#define GB_LANG_Maltese        				0x3A
#define GB_LANG_Norwegian      				0x14
#define GB_LANG_Polish         				0x15
#define GB_LANG_Portuguese     				0x16
#define GB_LANG_Romanian       				0x18
#define GB_LANG_Russian        				0x19
#define GB_LANG_Serbian        				0x1A
#define GB_LANG_Slovak         				0x1B
#define GB_LANG_Slovenian      				0x24
#define GB_LANG_Sorbian        				0x2E
#define GB_LANG_Spanish        				0x0A
#define GB_LANG_Sutu           				0x30
#define GB_LANG_Swedish        				0x1D
#define GB_LANG_Thai           				0x1E
#define GB_LANG_Tsonga         				0x31
#define GB_LANG_Tswana         				0x32
#define GB_LANG_Turkish        				0x1F
#define GB_LANG_Ukrainian      				0x22
#define GB_LANG_Urdu           				0x20
#define GB_LANG_Vietnamese     				0x2A
#define GB_LANG_Xhosa          				0x34
#define GB_LANG_Yiddish        				0x3D
#define GB_LANG_Zulu           				0x35

#endif

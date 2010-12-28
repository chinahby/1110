//Last Modify Date : 2003-5-6 17:02:37
//: EB_structures.h : ErBiDigital public structures.

#ifndef __EB_STRUCTURES_H_
#define __EB_STRUCTURES_H_
#include <string.h>
#include "AEEStdLib.h"

#include "EB.h"
//描述候选字词的结构体
typedef struct __candidate_info {
    unsigned char inputLen;	//输入串的长度，不包括词语输入的分隔符, 0表示为当前输入串的长度
    unsigned char len;		//词语的长度
    unsigned char start;	//候选字词在EBInputStruct::candidates的开始位置
    unsigned char reserved;	//暂时没有使用
} CandInfo;

//描述每一次选择候选字词的结构体
typedef struct __candidate_sel_info {
    unsigned char inputLen;	//输入串的长度，不包括词语输入的分隔符
    unsigned char outputLen;	//对应候选词的长度
} CandSelInfo;

//请注意：每加一个成员我们都需要修改EBDAssocInit, EBInputReset
typedef struct __eb_input_struct {
//in : can modified by user
    unsigned char inputMethod;	//输入法
    unsigned char candidateDesireLen;	//用户需要的候选字长度
    unsigned char interpunctionDesireLen;	//用户需要的标点符号的长度
    unsigned char inputStringLen;	//已输入串长度
    unsigned char inputString[EB_INPUT_MAX_LEN];	//已输入串[0~9,*]


#define EB_PHRASE_MAX_LEN          10	//词的最大长度
#define EB_ASSOC_ISN_MAX_LEN      (EB_PHRASE_MAX_LEN - 1)	//联想字的最大长度
    unsigned short associateISNs[EB_ASSOC_ISN_MAX_LEN];	//联想字
    unsigned char tone;		//音调
    unsigned char associateISNCount;	//联想字的个数
    unsigned char syllableIndex;	//当前选择的准确音节索引[1 -- 6]



//out : can not modified by user

    unsigned char candidateLen;	//实际长度
    unsigned short candidates[EB_CANDIDATES_MAX_LEN];	//候选字
// syllables以前为一个二维的输入，现在为1维数组，同样保存了各个音节，
//但是为不定长的方式，各个音节的头地址可以通过syllablesPointer数组的对应的值获得，
//获取指定下标的音节
//#define EBIS_GET_SYLLABLE(eb, i)     ((eb)->syllables + (eb)->syllablesPointer[i]) 
//获取当前选择的音节
//#define EBIS_GET_SEL_SYLLABLE(eb)    //
//     ((eb)->syllableIndex == EB_NULL) ? NULL : EBIS_GET_SYLLABLE((eb), (eb)->syllableIndex - 1)
//上面的两个宏将帮助用户获取某一个特定音节
    char syllables[256];

    unsigned short components[EB_COMPONENT_MAX_LEN];	//部件
    unsigned char syllableLen;	//syllables音节数量
    unsigned char componentLen;	//components长度
//ADD Date : 2003-10-27 15:44:38 by ABING
    unsigned char noUseSmartPunc;
    unsigned char noFirstKeyAssoc;
//2.0
//保存各个音节的对应syllables数组头地址的偏移
    unsigned char syllablesPointer[EB_SYLLABLE_MAX_SELECT];
//保存各个候选字词的信息，主要是对应输入串的长度，以及字词的长度
    CandInfo cis[EB_CANDIDATES_MAX_LEN];
//以下部分内容为EBHelper函数使用，主要是为了方便输入法的集成
//当前的整个输入串
    char fullInputString[EB_INPUT_MAX_LEN + 1];	//已输入串[0~9,*]
    //selInfo数组中有效的单元的数据
    unsigned char selCount;
    //当前的输出字符串
    unsigned short fullOutputString[EB_INPUT_MAX_LEN + 1];	//输出串[]
//当前的各个候选字词的选择信息
    CandSelInfo selInfo[EB_INPUT_MAX_LEN];
} EBInputStruct, *pEBInputStruct;

//获取指定下标的音节
#define EBIS_GET_SYLLABLE(eb, i)        ((eb)->syllables + (eb)->syllablesPointer[i])
//获取当前选择的音节
#define EBIS_GET_SEL_SYLLABLE(eb)    \
    (((eb)->syllableIndex == EB_NULL) ? NULL : EBIS_GET_SYLLABLE((eb), (eb)->syllableIndex - 1))

//（仅用于繁体版）获取指定下标的注音
#define EBIS_GET_SYLLABLE_BPMF(eb, i)   ((const unsigned short *)EBIS_GET_SYLLABLE(eb, i))
//（仅用于繁体版）获取当前选择的注音
#define EBIS_GET_SEL_SYLLABLE_BPMF(eb)  ((const unsigned short *)EBIS_GET_SEL_SYLLABLE(eb))
//Copy一个候选字到一个unsigned short的数组中，target为该数组。
#define EBIS_COPY_CANDIDATE(target, eb, i)      \
    MEMCPY(target, ((eb)->candidates + (eb)->cis[i].start), (eb)->cis[i].len * sizeof(short));\
    target[(eb)->cis[i].len] = 0;


//INIT ...
#define EBISInit(pEBIS) \
        memset((void *)(pEBIS), EB_NULL, sizeof(EBInputStruct)) ;

#define EBCleanCandidates(pEBIS) \
    (pEBIS)->candidateLen = 0 ;\
    memset((void *)(pEBIS)->candidates, EB_NULL, sizeof((pEBIS)->candidates)) ;

#define EBCleanSyllables(pEBIS) \
    (pEBIS)->syllableLen = 0 ; \
    (pEBIS)->syllableIndex = 0 ; \
    memset((void *)(pEBIS)->syllables, EB_NULL, sizeof((pEBIS)->syllables)) ;

#define EBCleanComponents(pEBIS) \
    (pEBIS)->componentLen = 0 ;\
    memset((void *)(pEBIS)->components, EB_NULL, sizeof((pEBIS)->components)) ;

//Macro : EBIsValidInputMethod :    Is valid Input Method ?
#define EBIsValidInputMethod(pEBIS) \
        ((pEBIS)->inputMethod >= EB_INPUT_METHOD_PINYIN && \
         (pEBIS)->inputMethod <= EB_INPUT_METHOD_YIN_STROKE)

//Macro : EBHaveInput :                Have Input ?
#define EBHaveInput(pEBIS) \
        ((pEBIS)->inputStringLen > 0 && \
         (pEBIS)->inputStringLen <= EB_INPUT_MAX_LEN)

//Macro : EBIsValidTone :            Is valid Tone ?
#define EBIsValidTone(pEBIS) \
         (((pEBIS)->tone >= EB_TONE_1 && \
         (pEBIS)->tone <= EB_TONE_5) || \
        (pEBIS)->tone == EB_NULL)

//Macro : EBCanSelectSyllables :    can select Syllables ?
#define EBCanSelectSyllable(pEBIS) \
        ((pEBIS)->syllableLen > 0 && \
         (pEBIS)->syllableLen <= EB_SYLLABLE_MAX_SELECT)

//Macro : EBHaveComponents :        have Components ?
#define EBHaveComponents(pEBIS) \
        ((pEBIS)->componentLen > 0 && \
         (pEBIS)->componentLen <= EB_COMPONENT_MAX_LEN)

//Macro : EBHaveCandidates :        have Candidates ?
#define EBHaveCandidates(pEBIS) \
        ((pEBIS)->candidateLen > 0 && \
         (pEBIS)->candidateLen <= EB_CANDIDATES_MAX_LEN)

//Macro : EBIsValidDesireLen :        Is valid candidate desire len ?
#define EBIsValidDesireLen(pEBIS) \
        ((pEBIS)->candidateDesireLen > 0 && \
         (pEBIS)->candidateDesireLen <= EB_CANDIDATES_MAX_LEN)

//Macro : EBIsValidSyllableIndex :    Is valid syllableIndex ?
#define EBIsValidSyllableIndex(pEBIS) \
        ((pEBIS)->syllableIndex > 0 && \
         (pEBIS)->syllableIndex <= (pEBIS)->syllableLen)




#endif				//__EB_STRUCTURES_H_

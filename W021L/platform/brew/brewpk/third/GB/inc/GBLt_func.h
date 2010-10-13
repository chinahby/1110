#ifndef __EBEN_FUNCS_H_
#define __EBEN_FUNCS_H_
#include "GBLt.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
EnStartup : Initialize engine. Before you invoke any other function, 
            you should invoke this function.
ptr       : if the engine havn't embeded data, ptr should be the pointer of engine data
            otherwise ptr should be null.
return :    if succeed this function will return EBEN_OK(0), 
            if failed the function will return a value smaller than 0. 
            To get the error message please check eben_defines.h
example :   
//if the engine havn't embedded data:
#include "GBLt.h"
{
    char * ptr = NULL;
    FILE * ebdFile = NULL;
    int ret;
    ebdFile = fopen(EBEN_DATA_PATH, "rb");
    if(ebdFile == NULL)
    {
        return;
    }
    ptr = (char *)malloc(1, 100 * 1000);
    if(ptr == NULL)
    {
        return -1;
    }
    if(fread(ptr, 100 * 1000, 1, ebdFile) <= 0)
    {
        free(ptr);
        ptr = NULL;
        return -2;
    }
    if((ret = EnStartup(ptr)) != EBEN_OK)
    {
        //error handled
        return -3
    }
    //OK
    ......
}
//if the engine embedded data:
#include "GBLt.h"
{
    if((ret = EnStartup(NULL)) != EBEN_OK)
    {
    //error handled
        return -3;
    }
    //OK
    ......
}

    */
int GBLtStartup(const void * ptr);
#define EnStartup   GBLtStartup

/*
EnFirstHeader : Get the first header which match the input.

pENIS :     the inputString and inputStringLen should be set to the right value.

return :    if succeed this function will return EBEN_OK(0), 
            if failed the function will return a value smaller than 0. 
            To get the error message please check eben_defines.h
Note :      this function only fill in the pENIS->header. You should invoke EnFirstPage/EnNextPage/EnPrevPage
            to fill in pENIS->candidates and pENIS->candidateLen.
example :   
{
    ENInputStruct input;
    ......(after you successfully initialized the engine) 
    strcpy(input.inputString, "23");
    input.inputStringLen = strlen(input.inputString);
    EnFirstHeader(&input);
    ....
}           
*/
int GBLtFirstHeader(pENInputStruct pENIS);
#define EnFirstHeader   GBLtFirstHeader

/*
EnNextHeader :  Get the next header which match the input. 
                You must invoked EnFirstHeader with same inputString and inputStringLen before 
                you invoked EnNextHeader. If you invoked EnFirstHeader then changed pENIS->inputString 
                or pENIS->inputStringLen and invoked EnNextHeader, the result would be unpredictable
                Before invoked this function, you had better invoke EnCanNextHeader to determind
                whether the function could be invoked or not.
pENIS :         the inputString and inputStringLen should not be changed after EnFirstHeader
return :        if succeed this function will return EBEN_OK(0), 
                if failed the function will return a value smaller than 0. 
                To get the error message please check eben_defines.h
*/
int GBLtNextHeader(pENInputStruct pENIS);
#define EnNextHeader   GBLtNextHeader

/*
EnPrevHeader :  Get the previous header which match the input. 
                After you invoked EnFirstHeader and at least one times EnNextHeader, 
                Before invoked this function, you had better invoke EnCanPrevHeader to determind 
                whether the function could be invoked or not.
pENIS :         the inputString and inputStringLen should not be changed after EnFirstHeader
return :        if succeed this function will return EBEN_OK(0), 
                if failed the function will return a value smaller than 0. 
                To get the error message please check eben_defines.h
                
*/
int GBLtPrevHeader(pENInputStruct pENIS);
#define EnPrevHeader   GBLtPrevHeader


/*
EnFirstPage :   After successfully invoking EnFirstHeader/EnNextHeader/EnPrevHeader, you should 
                invoke EnFirstPage to get a page of candidates.
                The count of candidates which are found by engine is determinded by candidateDesireLen 
                and candidateDesireCount.
pENIS :         the inputString and inputStringLen should not be changed after EnFirstHeader
return :        if succeed this function will return EBEN_OK(0), 
                if failed the function will return a value smaller than 0. 
                To get the error message please check eben_defines.h
*/
int GBLtFirstPage(pENInputStruct pENIS);
#define EnFirstPage   GBLtFirstPage

/*
EnNextPage :    After successfully invoking EnFirstPage, and EnCanPageDown return EBEN_OK then you 
                can invoke EnNextPage to get next page candidate.
                The count of candidates which are found by engine is determinded by candidateDesireLen 
                and candidateDesireCount.
pENIS :         the inputString and inputStringLen should not be changed after EnFirstHeader
return :        if succeed this function will return EBEN_OK(0), 
                if failed the function will return a value smaller than 0. 
                To get the error message please check eben_defines.h
*/
int GBLtNextPage(pENInputStruct pENIS);
#define EnNextPage   GBLtNextPage

/*
EnPrevPage :    After successfully invoking EnFirstPage and at one times EnNextPage, and EnCanPageUp return EBEN_OK then you 
                can invoke EnPrevPage to get prev  page candidate.
                The count of candidates which are found by engine is determinded by candidateDesireLen 
                and candidateDesireCount.
pENIS :         the inputString and inputStringLen should not be changed after EnFirstHeader
return :        if succeed this function will return EBEN_OK(0), 
                if failed the function will return a value smaller than 0. 
                To get the error message please check eben_defines.h
*/
int GBLtPrevPage(pENInputStruct pENIS);
#define EnPrevPage   GBLtPrevPage

/*
EnCanPageDown : determind whether can invoke EnNextPage function
                Typically you should invoke this function after you successfully invoked EnFirstPage 
                /EnNextPage/EnPrevPage
return :        if succeed this function will return EBEN_OK(0), 
                if failed the function will return a value smaller than 0. 
                To get the error message please check eben_defines.h
*/
int GBLtCanPageDown(pENInputStruct pENIS);
#define EnCanPageDown   GBLtCanPageDown

/*
EnCanPageDown : determind whether can invoke EnPrevPage function
                Typically you should invoke this function only when you successfully invoked EnFirstPage 
                /EnNextPage/EnPrevPage
return :        if succeed this function will return EBEN_OK(0), 
                if failed the function will return a value smaller than 0. 
                To get the error message please check eben_defines.h
*/
int GBLtCanPageUp(pENInputStruct pENIS);
#define EnCanPageUp   GBLtCanPageUp

/*
EnCanNextHeader :   determind whether can invoke EnNextHeader function
                    Typically you should invoke this function only when you successfully invoked 
                    EnFirstHeader/EnNextHeader/EnPrevHeader
return :            if succeed this function will return EBEN_OK(0), 
                    if failed the function will return a value smaller than 0. 
                    To get the error message please check eben_defines.h
*/
int GBLtCanNextHeader(pENInputStruct pENIS);
#define EnCanNextHeader   GBLtCanNextHeader

/*
EnCanPrevHeader :   determind whether can invoke EnPrevHeader function
                    Typically you should invoke this function only when you successfully invoked 
                    EnFirstHeader/EnNextHeader/EnPrevHeader
return :            if succeed this function will return EBEN_OK(0), 
                    if failed the function will return a value smaller than 0. 
                    To get the error message please check eben_defines.h
*/
int GBLtCanPrevHeader(pENInputStruct pENIS);
#define EnCanPrevHeader   GBLtCanPrevHeader

/*
EnReset :       reset the engine to status exactly same with the status after EnStartup.
*/
int GBLtReset(pENInputStruct pENIS);
#define EnReset   GBLtReset

/*
EnIsInited :    whether the engine is initialized.
*/
int GBLtIsInited(pENInputStruct pENIS);
#define EnIsInited   GBLtIsInited

/*
EnShutDown :    shut down the engine. After shut down, if you wanted to use engine, 
                you should initialize the engine again.
*/
int GBLtShutDown(pENInputStruct pENIS);
#define EnShutDown   GBLtShutDown

/*
 *	init DIY words. 
words       : the DIY words' buffer
capacity    : the length of buffer can extend.
fpUpdate    : a callback function to update the content of DIY word buffer. 
              see eben_defines.h for declaration.
 */
int GBLtSelfHelpWordsInit(void * words, 
                        unsigned int capacity, 
                        fpUpdateSelfHelpData fpUpdate,
                        void * addin);
#define EnSelfHelpWordsInit   GBLtSelfHelpWordsInit


/************************************************************************/
/* 增加一个单词的统计数字，用于调频                                     */
/************************************************************************/
int GBLtAddWordStat(const GBCHAR * words);
#define EnAddWordStat   GBLtAddWordStat

/*
add the word in pENIS->inputString (length of word is pENIS->inputLen) to DIY buffer
*/
int GBLtSelfHelpWordsAdd(pENInputStruct pENIS);
#define EnSelfHelpWordsAdd   GBLtSelfHelpWordsAdd

/*
check the word in pENIS->inputString (length of word is pENIS->inputLen) whether exists in DIY buffer
return EN_WORD_EXISTS if existing a word like this word ignored case.
return EN_WORD_EXISTS_AS_HEADER if not existing a word like this word 
       but existing a word begin with this word
 */
int GBLtWordExists(pENInputStruct pENIS);
#define EnWordExists   GBLtWordExists

/*
return EBEN_OK if succeed
*/
int GBLtGetLanguageID(pENInputStruct pENIS, unsigned short * language);
#define EnGetLanguageID   GBLtGetLanguageID

/*
将words里面所有的单词改为指定的格式
EN_OUTPUT_FCC               首字母大写
EN_OUTPUT_CAPITAL           全部字母大写
EN_OUTPUT_LOWER             全部字母小写
EN_OPTION_OUTPUT_TOGGLE_FCC 改变当前候选的首字母大小写状态

*/
void GBLtGetDesireOutput(pENInputStruct pENIS, int option);
#define EnGetDesireOutput GBLtGetDesireOutput

/*
 将自造词数据清空
*/
int GBLtShDataReset(void);
#define EnShDataReset   GBLtShDataReset

/*
 关闭自造词功能	
 */
int GBLtShShutDown(void);
#define EnShShutDown    GBLtShShutDown

/*语言大小写的切换函数*/

GBCHAR * gb_strlwr(GBCHAR * str);

GBCHAR * gb_strupr(GBCHAR * str);

int gb_isupper(GBCHAR chr);

int gb_islower(GBCHAR chr);

GBCHAR gb_chrlwr(GBCHAR);

GBCHAR gb_chrupr(GBCHAR);

int gb_strnicmp(const GBCHAR * str1, const GBCHAR * str2, unsigned int len);

/*
 \fn int GBLtGetLanguageIDFromData(const void * data);
 \brief Get Language ID From Guobi Latin Data.
 \param data : the language data.
 \return 
 EBEN_FAILED :  data is NULL, or the address of data 
                is invalid(the address should be 4 alignment).
 EBEN_CORRUPTED_DATA_FILE 
            : the data is not valid engine data.
 above zero :  a language ID defined in GBLt_def.h
 */
int GBLtGetLanguageIDFromData(const void * data);


#ifdef __cplusplus
};
#endif

#endif

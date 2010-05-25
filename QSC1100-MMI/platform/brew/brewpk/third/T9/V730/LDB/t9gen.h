/*******************************************************************************
;*******************************************************************************
;**                                                                           **
;**                    COPYRIGHT 2007 TEGIC COMMUNICATIONS                    **
;**                                                                           **
;**                TEGIC COMMUNICATIONS PROPRIETARY INFORMATION               **
;**                                                                           **
;**     This software is supplied under the terms of a license agreement      **
;**     or non-disclosure agreement with Tegic Communications and may not     **
;**     be copied or disclosed except in accordance with the terms of that    **
;**     agreement.                                                            **
;**                                                                           **
;*******************************************************************************
;**                                                                           **
;**     FileName: t9gen.c                                                     **
;**                                                                           **
;**  Description: Access to internal language data base files.                **
;**                                                                           **
;*******************************************************************************
;******* 10 ****** 20 ****** 30 ****** 40 ****** 50 ****** 60 ****** 70 *******/
#ifdef CUST_H
#include CUST_H
#endif
#include "t9api.h"
#define PAGESIZE    1024

#ifdef WIN32  //add by ydc
#define FEATURE_LANG_ENGLISH
#define FEATURE_LANG_CHINESE
//#define FEATURE_LANG_TCHINESE
#endif

#ifdef FEATURE_LANG_ENGLISH
extern const T9U8 T9FARDATA l0109b00[];/* English */
extern const T9U8 T9FARDATA l0109b01[];
extern const T9U8 T9FARDATA l0109b02[];
#endif //FEATURE_LANG_ENGLISH

#ifdef FEATURE_LANG_FRENCH
extern const T9U8 T9FARDATA l0112b00[];/* French */
extern const T9U8 T9FARDATA l0112b01[];
extern const T9U8 T9FARDATA l0112b02[];
extern const T9U8 T9FARDATA l0112b03[];
#endif //FEATURE_LANG_FRENCH

//Gemsea 20080228 add
#ifdef FEATURE_LANG_THAI
extern const T9U8 T9FARDATA l0130b00[];/* Thai */
extern const T9U8 T9FARDATA l0130b01[];
extern const T9U8 T9FARDATA l0130b02[];
extern const T9U8 T9FARDATA l0130b03[];
#endif //FEATURE_LANG_THAI
//Gemsea 20080228 end

#ifdef FEATURE_LANG_SPANISH
extern const T9U8 T9FARDATA l0110b00[];/* Spanish */
extern const T9U8 T9FARDATA l0110b01[];
extern const T9U8 T9FARDATA l0110b02[];
extern const T9U8 T9FARDATA l0110b03[];
#endif //FEATURE_LANG_SPANISH

#ifdef FEATURE_LANG_INDONESIAN
extern const T9U8 T9FARDATA l0133b00[];/* Indonesian */
extern const T9U8 T9FARDATA l0133b01[];
extern const T9U8 T9FARDATA l0133b02[];
extern const T9U8 T9FARDATA l0133b03[];
extern const T9U8 T9FARDATA l0133b04[];
#endif //FEATURE_LANG_INDONESIAN

// Gemsea add the code on 080621
#ifdef FEATURE_LANG_PORTUGUESE
extern const T9U8 T9FARDATA l0122b00[];/* Portuguese */
extern const T9U8 T9FARDATA l0122b01[];
extern const T9U8 T9FARDATA l0122b02[];
#endif
// Gemsea add end

// Gemsea add the code on 080819
#ifdef FEATURE_LANG_HEBREW
extern const T9U8 T9FARDATA l0113b00[];/* Hebrew */
extern const T9U8 T9FARDATA l0113b01[];
extern const T9U8 T9FARDATA l0113b02[];
extern const T9U8 T9FARDATA l0113b03[];
extern const T9U8 T9FARDATA l0113b04[];
extern const T9U8 T9FARDATA l0113b05[];
extern const T9U8 T9FARDATA l0113b06[];
extern const T9U8 T9FARDATA l0113b07[];
extern const T9U8 T9FARDATA l0113b08[];
#endif // FEATURE_LANG_HEBREW
// xiao guo.xie add end

// Gemsea add the code on 080822
#ifdef FEATURE_LANG_ARABIC
extern const T9U8 T9FARDATA l0101b00[];/* Arabic */
extern const T9U8 T9FARDATA l0101b01[];
extern const T9U8 T9FARDATA l0101b02[];
extern const T9U8 T9FARDATA l0101b03[];
extern const T9U8 T9FARDATA l0101b04[];
extern const T9U8 T9FARDATA l0101b05[];
extern const T9U8 T9FARDATA l0101b06[];
extern const T9U8 T9FARDATA l0101b07[];
extern const T9U8 T9FARDATA l0101b08[];
extern const T9U8 T9FARDATA l0101b09[];
#endif // FEATURE_LANG_ARABIC
// Gemsea add end

#ifdef FEATURE_LANG_TCHINESE
extern const T9ChineseData T9FARDATA l0104;/* Traditional Chinese */
#endif //FEATURE_LANG_TCHINESE

#ifdef FEATURE_LANG_CHINESE
extern const T9ChineseData T9FARDATA l0204;/* Simplified Chinese */
//const T9ChineseData T9FARDATA l0204;/* Simplified Chinese */
#endif //FEATURE_LANG_CHINESE

/*---------------------------------------------------------------------------
 *
 *  Function: T9ReadLdbData
 *                                 
 *  Synopsis: Callback from T9Core to access a non-Chinese language database
 *                                           
 *     Input: T9FieldInfo *pFieldInfo -- Field info struct that holds database number
 *            T9U32 dwOffset - byte offset from LDB start to read from
 *            T9FARPOINTER *ppbDst - Ptr to a ptr to the data cache.
 *            T9U32 *pdwStart - Ptr to offset of start of data.
 *            T9NUM nSize    - number of bytes desired (but only 1 is required)
 *            T9NUM mCacheIndex - cache number (igored for compiled in LDBs)
 * 
 *    Return: Number of bytes available at *pdwStart
 *                                                                            
 *---------------------------------------------------------------------------*/
T9U32 T9FARCALL T9ReadLdbData(T9FieldInfo *pFieldInfo, T9U32 dwOffset, T9FARPOINTER *ppbDst, T9U32 *pdwStart, T9UINT nSize, T9MINOR mCacheIndex)
{
    nSize = 15360;
    *pdwStart = dwOffset - (dwOffset % 15360);

    switch (pFieldInfo->nLdbNum) {
#ifdef FEATURE_LANG_ENGLISH    	
    case ( 1 * 256) +  9 : /* English */
             if (dwOffset <  15360) *ppbDst = (T9FARPOINTER) l0109b00;
        else if (dwOffset <  30720) *ppbDst = (T9FARPOINTER) l0109b01;
        else if (dwOffset <  36124) {*ppbDst = (T9FARPOINTER) l0109b02; nSize =   5404;}
        else                        nSize = 0; /* Invalid offset into database! */
        break;
#endif //FEATURE_LANG_ENGLISH        

#ifdef FEATURE_LANG_FRENCH        
    case ( 1 * 256) + 12 : /* French */
             if (dwOffset <  15360) *ppbDst = (T9FARPOINTER) l0112b00;
        else if (dwOffset <  30720) *ppbDst = (T9FARPOINTER) l0112b01;
        else if (dwOffset <  46080) *ppbDst = (T9FARPOINTER) l0112b02;
        else if (dwOffset <  46331) {*ppbDst = (T9FARPOINTER) l0112b03; nSize =    251;}
        else                        nSize = 0; /* Invalid offset into database! */
        break;
#endif //FEATURE_LANG_FRENCH        
        
#ifdef FEATURE_LANG_THAI
    case ( 1 * 256) + 30 : /* Thai */
             if (dwOffset <  15360) *ppbDst = (T9FARPOINTER) l0130b00;
        else if (dwOffset <  30720) *ppbDst = (T9FARPOINTER) l0130b01;
        else if (dwOffset <  46080) *ppbDst = (T9FARPOINTER) l0130b02;
        else if (dwOffset <  49883) {*ppbDst = (T9FARPOINTER) l0130b03; nSize =   3803;}
        else                        nSize = 0; /* Invalid offset into database! */
        break;
#endif //FEATURE_LANG_THAI

#ifdef FEATURE_LANG_SPANISH
    case ( 1 * 256) + 10 : /* Spanish */
             if (dwOffset <  15360) *ppbDst = (T9FARPOINTER) l0110b00;
        else if (dwOffset <  30720) *ppbDst = (T9FARPOINTER) l0110b01;
        else if (dwOffset <  46080) *ppbDst = (T9FARPOINTER) l0110b02;
        else if (dwOffset <  48347) {*ppbDst = (T9FARPOINTER) l0110b03; nSize =   2267;}
        else                        nSize = 0; /* Invalid offset into database! */
        break;
#endif //FEATURE_LANG_SPANISH  

#ifdef FEATURE_LANG_INDONESIAN
    case ( 1 * 256) + 33 : /* Indonesian */
             if (dwOffset <  15360) *ppbDst = (T9FARPOINTER) l0133b00;
        else if (dwOffset <  30720) *ppbDst = (T9FARPOINTER) l0133b01;
        else if (dwOffset <  46080) *ppbDst = (T9FARPOINTER) l0133b02;
        else if (dwOffset <  61440) *ppbDst = (T9FARPOINTER) l0133b03;
        else if (dwOffset <  64179) {*ppbDst = (T9FARPOINTER) l0133b04; nSize =   2739;}
        else                        nSize = 0; /* Invalid offset into database! */
        break;      
#endif //FEATURE_LANG_INDONESIAN

// Gemsea add the code on 080621
#ifdef FEATURE_LANG_PORTUGUESE
    case ( 1 * 256) + 22 : /* Portuguese */
            if (dwOffset <  15360) *ppbDst = (T9FARPOINTER) l0122b00;
        else if (dwOffset <  30720) *ppbDst = (T9FARPOINTER) l0122b01;
        else if (dwOffset <  44375) {*ppbDst = (T9FARPOINTER) l0122b02; nSize =  13655;}
        else                        nSize = 0; /* Invalid offset into database! */
        break;
#endif //FEATURE_LANG_PORTUGUESE   
// Gemsea add end

// Gemsea add the code on 080819
#ifdef FEATURE_LANG_HEBREW
    case ( 1 * 256) + 13 : /* Hebrew */
             if (dwOffset <  15360) *ppbDst = (T9FARPOINTER) l0113b00;
        else if (dwOffset <  30720) *ppbDst = (T9FARPOINTER) l0113b01;
        else if (dwOffset <  46080) *ppbDst = (T9FARPOINTER) l0113b02;
        else if (dwOffset <  61440) *ppbDst = (T9FARPOINTER) l0113b03;
        else if (dwOffset <  76800) *ppbDst = (T9FARPOINTER) l0113b04;
        else if (dwOffset <  92160) *ppbDst = (T9FARPOINTER) l0113b05;
        else if (dwOffset < 107520) *ppbDst = (T9FARPOINTER) l0113b06;
        else if (dwOffset < 122880) *ppbDst = (T9FARPOINTER) l0113b07;
        else if (dwOffset < 130208) {*ppbDst = (T9FARPOINTER) l0113b08; nSize =   7328;}
        else                        nSize = 0; /* Invalid offset into database! */
        break;
#endif // FEATURE_LANG_HEBREW
// Gemsea add end

// Gemsea add the code on 080822
#ifdef FEATURE_LANG_ARABIC
    case ( 1 * 256) +  1 : /* Arabic */
             if (dwOffset <  15360) *ppbDst = (T9FARPOINTER) l0101b00;
        else if (dwOffset <  30720) *ppbDst = (T9FARPOINTER) l0101b01;
        else if (dwOffset <  46080) *ppbDst = (T9FARPOINTER) l0101b02;
        else if (dwOffset <  61440) *ppbDst = (T9FARPOINTER) l0101b03;
        else if (dwOffset <  76800) *ppbDst = (T9FARPOINTER) l0101b04;
        else if (dwOffset <  92160) *ppbDst = (T9FARPOINTER) l0101b05;
        else if (dwOffset < 107520) *ppbDst = (T9FARPOINTER) l0101b06;
        else if (dwOffset < 122880) *ppbDst = (T9FARPOINTER) l0101b07;
        else if (dwOffset < 138240) *ppbDst = (T9FARPOINTER) l0101b08;
        else if (dwOffset < 144394) {*ppbDst = (T9FARPOINTER) l0101b09; nSize =   6154;}
        else                        nSize = 0; /* Invalid offset into database! */
        break;
#endif // FEATURE_LANG_ARABIC
// Gemsea add end

   default :
       return 0;
   }

    return nSize;
}
#if defined(T9CHINESE)
/*---------------------------------------------------------------------------
 *
 *  Function: T9CCLoadLdb
 *
 *  Synopsis: Callback from T9Core to load the Chinese language database
 *
 *     Input: pCCFieldInfo   -- T9 Chinese Fieldinfo structure
 *            pT9CCLdbHeader -- Pointer to the buffer that the callback
 *                              function loads the LDB data to.
 *
 *    Return: T9STATNONE if the loading succeeds;
 *            T9STATLOADLDBFAIL if the language ID in the T9 FieldInfo
 *            structure is not valid.
 *
 *---------------------------------------------------------------------------*/
T9STATUS T9FARCALL T9CCLoadLdb(T9CCFieldInfo *pCCFieldInfo, T9ChineseData T9FARDATA *pT9CCLdbHeader)
{
    T9ChineseData const T9FARDATA *pLdbHeader;
    T9UINT i;

    switch (pCCFieldInfo->G.nLdbNum) {
#ifdef FEATURE_LANG_TCHINESE
        /* Load Traditional Chinese LDB */
        case (1 * 256 + 4):
            pLdbHeader = &l0104;
            break;
#endif //FEATURE_LANG_TCHINESE            

#ifdef FEATURE_LANG_CHINESE            
        /* Load Simplified Chinese LDB */
        case (2 * 256 + 4):
            pLdbHeader = &l0204;
            break;
#endif //FEATURE_LANG_CHINESE            
            
        default :
            return T9STATLOADLDBFAIL;
    }

    /* Copy the header part */
    for (i = 0; i < T9CCLdbHeaderSize; i++)
        pT9CCLdbHeader->bHeader[i] = pLdbHeader->bHeader[i];

    /* Copy block pointers */
    for (i = 0; i < T9CCLDBBLOCKCOUNT; i++)
        pT9CCLdbHeader->pBlockPointers[i] = pLdbHeader->pBlockPointers[i];

    return T9STATNONE;
}
#endif //#if defined(T9CHINESE)
/*-----------------------------------< eof >----------------------------------*/

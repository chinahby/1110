
/*******************************************************************************
********************************************************************************

                    COPYRIGHT 1998-2003 TEGIC COMMUNICATIONS

                TEGIC COMMUNICATIONS PROPRIETARY INFORMATION

     This software is supplied under the terms of a license agreement
     or non-disclosure agreement with Tegic Communications and may not
     be copied or disclosed except in accordance with the terms of that
     agreement.

********************************************************************************

     File Name:   ChinConv.c

     Description: Conversion routines between various Chinese character
                  encoding schemes.

********************************************************************************
********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include "t9api.h"
#if    defined(T9CHINESE)

#include "chinconv.h"

#if 0

#ifndef _64K_LIMIT_
#include "chncnv0.c"
#include "chncnv1.c"
#include "chncnv2.c"
#include "chncnv3.c"
#include "chncnv4.c"
#include "chncnv5.c"
#include "chncnv6.c"
#include "chncnv7.c"
#include "chncnv8.c"
#include "chncnv9.c"
#include "chncnv10.c"
#include "chncnv11.c"
#include "chncnv12.c"
#include "chncnv13.c"
#include "chncnv14.c"
#include "chncnv15.c"
#include "chncnv16.c"
#include "chncnv17.c"

#else

extern UnicodeTableEntry    UnicodeTableEntry0;
extern UnicodeTableEntry    UnicodeTableEntry1;
extern UnicodeTableEntry    UnicodeTableEntry2;
extern UnicodeTableEntry    UnicodeTableEntry3;
extern UnicodeTableEntry    UnicodeTableEntry4;
extern UnicodeTableEntry    UnicodeTableEntry5;
extern UnicodeTableEntry    UnicodeTableEntry6;
extern UnicodeTableEntry    UnicodeTableEntry7;
extern UnicodeTableEntry    UnicodeTableEntry8;
extern UnicodeTableEntry    UnicodeTableEntry9;
extern UnicodeTableEntry    UnicodeTableEntry10;
extern UnicodeTableEntry    UnicodeTableEntry11;
extern UnicodeTableEntry    UnicodeTableEntry12;
extern UnicodeTableEntry    UnicodeTableEntry13;
extern UnicodeTableEntry    UnicodeTableEntry14;
extern UnicodeTableEntry    UnicodeTableEntry15;
extern UnicodeTableEntry    UnicodeTableEntry16;
extern UnicodeTableEntry    UnicodeTableEntry17;

#endif

#define CHIN_NUM_TABLES 18

UnicodeTableEntry * pUnicodeTable[CHIN_NUM_TABLES] =
							{&UnicodeTableEntry0,  &UnicodeTableEntry1,
							 &UnicodeTableEntry2,  &UnicodeTableEntry3,
							 &UnicodeTableEntry4,  &UnicodeTableEntry5,
							 &UnicodeTableEntry6,  &UnicodeTableEntry7,
							 &UnicodeTableEntry8,  &UnicodeTableEntry9,
							 &UnicodeTableEntry10, &UnicodeTableEntry11,
							 &UnicodeTableEntry12, &UnicodeTableEntry13,
							 &UnicodeTableEntry14, &UnicodeTableEntry15,
							 &UnicodeTableEntry16, &UnicodeTableEntry17
 };
/*=========================================================================
 * Summary of Chinese characters:
 *   Total valid Unicodes   = 29139 + 2(2EC6,2EEC to be removed) + 5(HK214)
 *   Total valid GB codes   = 20954 (exclude 6530 4-type encoding)
 *   Total valid Big5 codes = 14416 + 2645 - 1(9F9C) + 1648 + 7
 *
 * T9 Chinese character support:
 *   GB2312  =  6763, 0x4E00 - 0x9FA0
 *   GB18030 = 27484, 0x3400 - 0x9FA5
 *   Big 5   = 13051, 0x4E00 - 0x9FA4
 *   HKSCS   =  4702, 0x00A8 - 0xFFED
 *   HK214   =     5, 0xE387 - 0xEA82
 *=======================================================================*/

T9U16           GetNumberOfUnicodes(void);


/*******************************************************************************
 *
 *	Function name	: BIG5Char2Offset
 *	Description	    : convert big5 code to offset ranging 0 - N, N = # big5
 *                    characters.  Starting character is a440
 *
 *	Return     		: Screen Offset
 *
 *	Pass            : T9U16 wChar
 *
 ******************************************************************************/
T9U16 BIG5Char2Offset(T9U16 wChar)
{
    T9U8    bHi;
    T9U8    bLo;
    T9U16   wOffset;

    bHi = (T9U8)((wChar >> 8) & 0xff);
    bLo = (T9U8)(wChar & 0xff);

    if (bLo <= 0x7e)
        wOffset = (T9U16)(((bHi - 0xa4) * 157) + (bLo - 0x40));
    else
        wOffset = (T9U16)(((bHi - 0xa4) * 157) + (bLo - 98));


    if (wChar < 0xc940)
        return wOffset;
    else
        return (T9U16)(wOffset - 408);

} /*  BIG5Char2Offset() */


/*******************************************************************************
 *
 *	Function name	: GBChar2Offset
 *	Description	    : convert GB char to offset ranging 0 - N, N = # GB chars
 *                    Starting character is b0a1
 *
 *	Return     		: Screen Offset
 *
 *	Pass            : T9U16 wChar
 *
 ******************************************************************************/
T9U16 GBChar2Offset(T9U16 wChar)
{
    T9U8    bHi;
    T9U8    bLo;
    T9U16   wOffset;

    bHi = (T9U8)((wChar >> 8) & 0xff);
    bLo = (T9U8)(wChar & 0xff);
    wOffset = (T9U16)(((bHi - 0xb0) * 94) + (bLo - 0xa1));
    return wOffset;

} /*  GBChar2Offset() */


/*******************************************************************************
 *
 *	Function name	: GBChar2GBMap
 *	Description	    : convert GB char to GB map position ranging 0 - N, N = # GB chars
 *                    Starting character is A1A1!!!   NO OFFSET NEEDED!!!!!!!
 *
 *                    Current support for some symbols, Greek letters & Chinese chars
 *
 *	Return     		: Map Offset
 *
 *	Pass            : T9U16 wChar
 *
 ******************************************************************************/
T9U16 GBChar2GBMap(T9U16 wChar)
{
    T9U16   retVal = 0;

    /* process Chinese characters */
    if (wChar >= 0xB0A1)
        retVal = (T9U16)(GBChar2Offset(wChar) + 682);

    /* greek letters */
    if (wChar >= 0xA6A1  && wChar <= 0xA6B8)
        retVal = (T9U16)(wChar - 0xA6A1 + 429);
    if (wChar >= 0xA6C1  && wChar <= 0xA6D8)
        retVal = (T9U16)(wChar - 0xA6C1 + 453);

    /* symbols */
    if (wChar >= 0xA1A1  && wChar <= 0xA1FE)
        retVal = (T9U16)(wChar - 0xA1A1);

    return retVal;

} /*  GBChar2GBMap() */


/*******************************************************************************
 *
 *	Function name	: Offset2Big5Char
 *	Description	    : convert offset ranging 0 - N, N = # big5 chars
 *
 *	Return     		: BIG5 Code
 *
 *	Pass            : T9U16 wOffset
 *
 ******************************************************************************/
T9U16 Offset2Big5Char(T9U16 wOffset)
{
    T9U8    bHi;
    T9U8    bLo;
    T9U16   wChar;

    if (wOffset > 5400)
        wOffset += 408;

    bHi = (T9U8)(wOffset / 157);
    bLo = (T9U8)(wOffset % 157);
    bHi += 0xa4;

    if (bLo <= 62)
        bLo += 0x40;
    else
        bLo += 98;

    wChar = (T9U16)bHi;
    wChar = (T9U16)(((wChar) << 8) & 0xFF00);
    wChar = (T9U16)((wChar) + (bLo & 0x00FF));

    return wChar;

} /*  Offset2Big5Char() */

/*******************************************************************************
 *
 *	Function name	: COffset2GBChar
 *	Description	    : Convert offset ranging 0 - N, N = # GB characters, to GB
 *                    char. 0 => 0xB0A1
 *
 *	Return     		: GB Code
 *
 *	Pass            : T9U16 OffsetIn
 *
 ******************************************************************************/
T9U16 COffset2GBChar(T9U16 OffsetIn)
{
    T9U8    bHi;
    T9U8    bLo;

    bHi = (T9U8)(OffsetIn / 94);
    bLo = (T9U8)(OffsetIn % 94);

    return (T9U16)(((T9U16)bHi << 8) + bLo + 0xB0A1);

} /* COffset2GBChar() */

/*---------------------------------------------------------------------------
 *
 *  Function: CompareGbCodes
 *
 *  Synopsis: Compare routine for qsort/bsearch of table of GB table
 *
 *     Input: elem1     -- Pointer to first element index
 *            elem2     -- Pointer to second element index
 *
 *    Return: Result of compare
 *
 *---------------------------------------------------------------------------*/
static  int     CompareGbCodes(const void *elem1, const void *elem2)
{
    return (((UnicodeEntry *)elem1)->wGBcode - ((UnicodeEntry *)elem2)->wGBcode);
}

/*---------------------------------------------------------------------------
 *
 *  Function: CompareBig5Codes
 *
 *  Synopsis: Compare routine for qsort/bsearch of table of Big5 table
 *
 *     Input: elem1     -- Pointer to first element index
 *            elem2     -- Pointer to second element index
 *
 *    Return: Result of compare
 *
 *---------------------------------------------------------------------------*/
static  int     CompareBig5Codes(const void *elem1, const void *elem2)
{
    return (((UnicodeEntry *)elem1)->wBig5code - ((UnicodeEntry *)elem2)->wBig5code);
}

/*******************************************************************************
 *
 *	Function name	: GBToUnicode
 *	Description	    : Convert GB character code to Unicode
 *
 *	Return     		: Unicode or 0 if not found
 *
 *	Pass            : T9U16 wGBcode     -- Code to be translated
 *
 ******************************************************************************/
static  UnicodeEntry  T9FARDATA  *GBTable = NULL;

T9U16   GBToUnicode(T9U16 wGBcode)
{
    int				total, i, nSize;
    UnicodeEntry    sRecord;
    UnicodeEntry  T9FARDATA  *psTable;

    total = GetNumberOfUnicodes();
    if (GBTable == NULL) {
        GBTable = malloc(total * sizeof(UnicodeEntry));
		nSize = 0;
		for (i = 0; i < CHIN_NUM_TABLES; i++) {
			psTable = pUnicodeTable[i]->lpUnicodeTable;

#if !defined(WIN32)
            _fmemcpy(GBTable + nSize, psTable, pUnicodeTable[i]->wTableEntrySize * sizeof(UnicodeEntry));
#else
            memcpy(GBTable + nSize, psTable, pUnicodeTable[i]->wTableEntrySize * sizeof(UnicodeEntry));
#endif
            nSize = nSize + pUnicodeTable[i]->wTableEntrySize * sizeof(UnicodeEntry);
		}
#if !defined(WIN32)
        qsort((void *)GBTable, total, sizeof(UnicodeEntry), CompareGbCodes);
#else
        qsort(GBTable, total, sizeof(UnicodeEntry), CompareGbCodes);
#endif

    }

    sRecord.wGBcode = wGBcode;
#if !defined(WIN32)
    psTable = bsearch(&sRecord, (void *)GBTable, total, sizeof(UnicodeEntry), CompareGbCodes);
#else
    psTable = bsearch(&sRecord, GBTable, total, sizeof(UnicodeEntry), CompareGbCodes);
#endif

    return (T9U16)(psTable == 0 ? 0 : psTable->wUnicode);

} /* GBToUnicode() */

/*******************************************************************************
 *
 *	Function name	: UnicodeToGB
 *	Description	    : Convert Unicode character code to GB
 *
 *	Return     		: GB code or 0 if not found
 *
 *	Pass            : T9U16 wUnicode     -- Code to be translated
 *
 ******************************************************************************/
T9U16   UnicodeToGB(T9U16 wUnicode)
{
    UnicodeEntry  T9FARDATA  *psTable;
	int				i;
	T9U8			bFound = 0;

	for (i = 0; i < CHIN_NUM_TABLES; i++) {
			psTable = pUnicodeTable[i]->lpUnicodeTable;

        while (psTable->wUnicode != 0) {
			if (psTable->wUnicode == wUnicode) {
				bFound = 1;
				break;
			}
	        psTable++;
		}
		if (bFound)
			break;
	}

	if (bFound)
	    return psTable->wGBcode;
	else
		return 0;

} /* UnicodeToGB() */


/*******************************************************************************
 *
 *	Function name	: Big5ToUnicode
 *	Description	    : Convert Big5 character code to Unicode
 *
 *	Return     		: Unicode or 0 if not found
 *
 *	Pass            : T9U16 wBig5code     -- Code to be translated
 *
 ******************************************************************************/
static  UnicodeEntry  T9FARUDBPOINTER *  *Big5Table = NULL;

T9U16   Big5ToUnicode(T9U16 wBig5code)
{
    int				total, nSize, i;
    UnicodeEntry    sRecord;
    UnicodeEntry   T9FARDATA *psTable;


    total = GetNumberOfUnicodes();
    if (Big5Table == NULL) {
        Big5Table = malloc(total * sizeof(UnicodeEntry));
		nSize = 0;
		for (i = 0; i < CHIN_NUM_TABLES; i++) {
			psTable = pUnicodeTable[i]->lpUnicodeTable;
#if !defined(WIN32)
            _fmemcpy(Big5Table + nSize, psTable, pUnicodeTable[i]->wTableEntrySize * sizeof(UnicodeEntry));
#else
            memcpy(Big5Table + nSize, psTable, pUnicodeTable[i]->wTableEntrySize * sizeof(UnicodeEntry));
#endif
			nSize = nSize + pUnicodeTable[i]->wTableEntrySize * sizeof(UnicodeEntry);
		}
        qsort(Big5Table, total, sizeof(UnicodeEntry), CompareBig5Codes);
    }

    sRecord.wBig5code = wBig5code;
    psTable = bsearch(&sRecord, Big5Table, total, sizeof(UnicodeEntry), CompareBig5Codes);
    return (T9U16)(psTable == 0 ? 0 : psTable->wUnicode);

} /* Big5ToUnicode() */

/*******************************************************************************
 *
 *	Function name	: UnicodeToBig5
 *	Description	    : Convert Unicode character code to Big5
 *
 *	Return     		: Big5 code or 0 if not found
 *
 *	Pass            : T9U16 wUnicode     -- Code to be translated
 *
 ******************************************************************************/
T9U16   UnicodeToBig5(T9U16 wUnicode)
{
    UnicodeEntry  T9FARDATA  *psTable;
	int				i;
	T9U8			bFound = 0;

	for (i = 0; i < CHIN_NUM_TABLES; i++) {
		psTable = pUnicodeTable[i]->lpUnicodeTable;

        while (psTable->wUnicode != 0) {
			if (psTable->wUnicode == wUnicode) {
				bFound = 1;
				break;
			}
	        psTable++;
		}
		if (bFound)
			break;
	}

	if (bFound)
	    return psTable->wBig5code;
	else
		return 0;

} /* UnicodeToBig5() */


/*******************************************************************************
 *
 *	Function name	: UnicodeToLinearOffset
 *	Description	    : Convert Unicode character code a linear offset of unicodes
 *                    (Where unicodes start from 4e00, Linear offsets start from 0)
 *
 *	Return     		: Linear offset
 *
 *	Pass            : T9U16 wUnicode     -- Code to be translated
 *
 ******************************************************************************/
T9U16   UnicodeToLinearOffset(T9U16 wUnicode)
{
    UnicodeEntry  T9FARDATA  *psTable;
    T9U16           wIndex = 0;
	int				i;
	T9U8			bFound = 0;


	for (i = 0; i < CHIN_NUM_TABLES; i++) {
			psTable = pUnicodeTable[i]->lpUnicodeTable;

        while (psTable->wUnicode != 0) {
			if (psTable->wUnicode == wUnicode) {
				bFound = 1;
				break;
			}
	        psTable++;
			wIndex++;
		}
		if (bFound)
			break;
	}

    return wIndex;

} /* UnicodeToLinearOffset() */

#endif //#if 0
    static T9U8 bLetterOrder[] = {
        0x62, /* b */    0x70, /* p */    0x6D, /* m */    0x66, /* f */    0x64, /* d */
        0x74, /* t */    0x6E, /* n */    0x6C, /* l */    0x67, /* g */    0x6B, /* k */
        0x68, /* h */    0x6A, /* j */    0x71, /* q */    0x78, /* x */    0xDF, /* zh */
        0xE7, /* ch */   0x9A, /* sh */   0x72, /* r */    0x7A, /* z */    0x63, /* c */
        0x73, /* s */    0x61, /* a */    0x6F, /* o */    0x65, /* e */    0xE9, /* e(C) */
        0xE4, /* ai */   0xF6, /* ei */   0xE5, /* ao */   0xFD, /* ou */   0xE3, /* an */
        0xF5, /* en */   0xE2, /* ang */  0xF4, /* eng */  0xF8, /* er */   0x69, /* i */
        0x75, /* u */    0xFC, /* ü */     0 };

/*******************************************************************************
 *
 *	Function name	: BPMFInternalToUnicode
 *	Description	    : Convert 8 bit Tegic BoPoMoFo internal char code to Unicode
 *                    (Where unicodes start from 3100, Linear offsets start from 0)
 *
 *	Return     		: wUnicode
 *
 *	Pass            : T9U8 - alpha Tegic BoPoMoFo internal char code to be translated
 *
 ******************************************************************************/
T9U16   BPMFInternalToUnicode(T9U8 bChar)
{
    T9U16 nIdx;

    for (nIdx = 0; bLetterOrder[nIdx]; nIdx++) {
        if (bLetterOrder[nIdx] == bChar)
            break;
    }

    /* If the character was not found in the table just return it */
    if (bLetterOrder[nIdx])
        return(0x3105 + nIdx);
    else
        return (T9U16)bChar;


}


T9U8    UnicodeToBPMFInternal(T9U16 wUnicode)
{

    T9U16 nIdx;

    nIdx = wUnicode - 0x3105;

    return(bLetterOrder[nIdx]);

}

/*******************************************************************************
 *
 *	Function name	: IsBPMFChar
 *	Description	    : check if the input char is a BoPoMoFo internal char.
 *
 *	Return     		: 1 (Yes) or 0(No)
 *
 *	Pass            : T9U8 - alpha Tegic BoPoMoFo internal char code to be translated
 *
 ******************************************************************************/
T9U8   IsBPMFChar(T9U8 bChar)
{
    T9U16 nIdx;
    for (nIdx = 0; bLetterOrder[nIdx]; nIdx++) {
        if (bLetterOrder[nIdx] == bChar)
            return 1;
    }

    return 0;
}



#endif

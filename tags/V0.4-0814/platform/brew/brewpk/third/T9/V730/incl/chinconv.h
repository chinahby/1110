/*******************************************************************************
********************************************************************************

                    COPYRIGHT 1998-2003 TEGIC COMMUNICATIONS

                TEGIC COMMUNICATIONS PROPRIETARY INFORMATION

     This software is supplied under the terms of a license agreement
     or non-disclosure agreement with Tegic Communications and may not
     be copied or disclosed except in accordance with the terms of that
     agreement.

********************************************************************************

     File Name:   chinconv.h

     Description: Header file for ChinConv.c -- Translation routines between
                  various Chinese character encoding schemes.

********************************************************************************
********************************************************************************/
/*----------------------------------------------------------------------------
 *  Declare Unicode -> GB / Big5 translation table entry
 *----------------------------------------------------------------------------*/

#ifndef __CHINCONV_H__
#define __CHINCONV_H__

#include "t9api.h"

typedef struct UnicodeEntry_s {

    T9U16   wUnicode;       /* Unicode value                                */
    T9U16   wGBcode;        /* Corresponding GB value or zero for none      */
    T9U16   wBig5code;      /* Corresponding Big5 value or zero for none    */

} UnicodeEntry;

typedef struct UnicodeTableEntry_s {

    UnicodeEntry T9FARDATA *	lpUnicodeTable;
    T9U16					wTableEntrySize;

} UnicodeTableEntry;


extern  T9U16   UnicodeToGB(T9U16 wUnicode);
extern  T9U16   UnicodeToBig5(T9U16 wUnicode);
extern  T9U16   BIG5Char2Offset(T9U16 wChar);
extern  T9U16   GBChar2Offset(T9U16 wChar);
extern  T9U16   Offset2Big5Char(T9U16 wOffset);
extern  T9U16   COffset2GBChar(T9U16 OffsetIn);
extern  T9U16   GBToUnicode(T9U16 wGBcode);
extern  T9U16   Big5ToUnicode(T9U16 wBig5code);
extern  T9U16   UnicodeToLinearOffset(T9U16 wUnicode);
extern  T9U16   LinearOffsetToUnicode(T9U16 wOffset);
extern  T9U16   GetNumberOfUnicodes(void);
extern	T9U16   BPMFInternalToUnicode(T9U8 bChar);
extern	T9U8    UnicodeToBPMFInternal(T9U16 wWord);
extern	T9U16	GBChar2GBMap(T9U16 wChar);
extern  T9U8    IsBPMFChar(T9U8 bChar);
extern  void    ComputeTableEntrySize(void);
#if 0
extern	T9U8	IsComponentChar(T9SYMB ch);
extern	T9U16	UnicodeToOffsetCmp();
#endif

#endif /* __CHINCONV_H__ */


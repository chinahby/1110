/*
  ========================================================================

  FILE:  AEETextController.h
  
  SERVICES: 

  GENERAL DESCRIPTION: Default text controller for text widget. I
  implement a multitap controller

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#ifndef __AEETEXTCONTROLLER_H__
#define __AEETEXTCONTROLLER_H__

#include "bid/AEECLSID_TEXTCONTROLLER.bid"



//---------------------------------------------------------------------------
/*

   Text entry mode values are stored in a 32 bit unsigned integer, composed of
   two 16 bit components.  The low order 16 bits holds the 'language' designator, 
   which defines actual languages, such as French and English, as well as numbers, 
   symbols, dialed digits, etc.

   The high-order 16 bits holds the mode flags, which are behavior modifiers for 
   text entry.  These flags are specify entry mode behaviors like multitap or
   rapid entry, and also case modes such as lower case, upper case or mixed case.
   

Examples

Text Mode Enumeration
  
   A text controller might return the following values when enumerating
   the text modes.
   

   1)  AEE_TLANG_ENGLISH_US | AEE_TMODE_RAPID    | AEE_TMODE_LOWERCASE
   2)  AEE_TLANG_ENGLISH_US | AEE_TMODE_RAPID    | AEE_TMODE_UPPERCASE
   3)  AEE_TLANG_ENGLISH_US | AEE_TMODE_RAPID    | AEE_TMODE_MIXEDCASE
   3)  AEE_TLANG_ENGLISH_US | AEE_TMODE_RAPID    | AEE_TMODE_SENTENCECASE
   4)  AEE_TLANG_ENGLISH_US | AEE_TMODE_MULTITAP | AEE_TMODE_LOWERCASE
   5)  AEE_TLANG_ENGLISH_US | AEE_TMODE_MULTITAP | AEE_TMODE_UPPERCASE
   6)  AEE_TLANG_ENGLISH_US | AEE_TMODE_MULTITAP | AEE_TMODE_MIXEDCASE
   7)  AEE_TLANG_ENGLISH_US | AEE_TMODE_MULTITAP | AEE_TMODE_SENTENCECASE
   8)  AEE_TLANG_HINDI      | AEE_TMODE_RAPID
   9)  AEE_TLANG_HINDI      | AEE_TMODE_MULTITAP
  10)  AEE_TLANG_NUMBERS
  11)  AEE_TLANG_DIALEDDIGITS
  12)  AEE_TLANG_SYMBOLS_PUNCTUATION


Set Text Mode

   If an incomplete input mode is given, a reasonable default will be chosen

   Passed:  AEE_TLANG_ENGLISH_US
   Actual:  AEE_TLANG_ENGLISH_US | AEE_TMODE_MULTITAP | AEE_TMODE_SENTENCECASE

   Passed:  AEE_TMODE_RAPID
   Actual:  AEE_TLANG_ENGLISH_US | AEE_TMODE_RAPID | AEE_TMODE_SENTENCECASE

*/
//----------------------------------------------------------------------------



//---------------------------------------------------------------------------
// Language information is stored in the low-order word (bits 0-15)
//---------------------------------------------------------------------------

#define AEE_TLANG_NONE                       0

#define AEE_TLANG_NUMBERS                    1
#define AEE_TLANG_DIALEDDIGITS               2

#define AEE_TLANG_SYMBOLS                    3
#define AEE_TLANG_SYMBOLS_PUNCTUATION        4
#define AEE_TLANG_SYMBOLS_EMOTICONS          5
#define AEE_TLANG_SYMBOLS_EMOTICONS2         6
#define AEE_TLANG_SYMBOLS_EMOTICONS3         7
#define AEE_TLANG_SYMBOLS_EMOTICONS4         8
#define AEE_TLANG_SYMBOLS_ANIMATED           9

#define AEE_TLANG_JAPANESE                   20 // default Japanese
#define AEE_TLANG_KANJI                      21
#define AEE_TLANG_HANKAKU_KANA               22
#define AEE_TLANG_HIRIGANA                   23

#define AEE_TLANG_CHINESE                    30 // default Chinese

#define AEE_TLANG_HANGUL                     40

#define AEE_TLANG_HINDI                      41

#define AEE_TLANG_ARABIC                     42
#define AEE_TLANG_HEBREW                     43

#define AEE_TLANG_CROATIAN                   44
#define AEE_TLANG_CZECH                      45
#define AEE_TLANG_HUNGARIAN                  46
#define AEE_TLANG_ROMANIAN                   47
#define AEE_TLANG_RUSSIAN                    48
#define AEE_TLANG_SERBIAN                    49
#define AEE_TLANG_SLOVAK                     50
#define AEE_TLANG_SLOVENIAN                  51
#define AEE_TLANG_TURKISH                    52
#define AEE_TLANG_UKRAINIAN                  53

#define AEE_TLANG_DANISH                     54
#define AEE_TLANG_DUTCH                      55
#define AEE_TLANG_FINNISH                    56
#define AEE_TLANG_FRENCH_EU                  57
#define AEE_TLANG_GERMAN                     58
#define AEE_TLANG_GREEK                      59
#define AEE_TLANG_ITALIAN                    60
#define AEE_TLANG_NORWEGIAN                  61
#define AEE_TLANG_POLISH                     62
#define AEE_TLANG_PORTUGUESE_EU              63
#define AEE_TLANG_SPANISH_EU                 64
#define AEE_TLANG_SWEDISH                    65

#define AEE_TLANG_INDONESIAN                 66
#define AEE_TLANG_MALAY                      67
#define AEE_TLANG_TAGALOG                    68
#define AEE_TLANG_THAI                       69
#define AEE_TLANG_VIETNAMESE                 70

#define AEE_TLANG_FRENCH_CA                  71
#define AEE_TLANG_PORTUGUESE_BR              72
#define AEE_TLANG_SPANISH_LA                 73
#define AEE_TLANG_ENGLISH_US                 74
#define AEE_TLANG_ENGLISH_UK                 75
                                             
#define AEE_TLANG_USER                       500
#define AEE_TLANG_FIRST_OEM                  AEE_TLANG_USER + 1
#define AEE_TLANG_RESERVED                   0x7000
                                             
#define AEE_TLANG_MASK                       0x0000FFFF

//---------------------------------------------------------------------------
// Text-entry modes are stored in the high-order word (bits 16-31)
//---------------------------------------------------------------------------

// case behavior
#define AEE_TMODE_LOWERCASE                  0x80000000
#define AEE_TMODE_UPPERCASE                  0x40000000 
#define AEE_TMODE_MIXEDCASE                  0x20000000  
#define AEE_TMODE_SENTENCECASE               0x10000000
// entry methods
#define AEE_TMODE_RAPID                      0x08000000
#define AEE_TMODE_MULTITAP                   0x04000000
#define AEE_TMODE_KEYBOARD                   0x02000000

#define AEE_TMODE_MASK                       0xFFFF0000

//---------------------------------------------------------------------------
// Language specific modes are stored in the least significant nibble
// of the high-order word (bits 16-19)
//---------------------------------------------------------------------------

// Chinese language specific modes
#define AEE_TMODE_PINYIN                     0x00010000
#define AEE_TMODE_STROKE                     0x00020000


//---------------------------------------------------------------------------
// Defines for backward compatability
//---------------------------------------------------------------------------

#define AEE_TMODE_NONE           AEE_TLANG_NONE
#define AEE_TMODE_NUMBERS        AEE_TLANG_NUMBERS
#define AEE_TMODE_MULTITAP_LC    (AEE_TLANG_ENGLISH_US | AEE_TMODE_MULTITAP | AEE_TMODE_LOWERCASE)
#define AEE_TMODE_MULTITAP_UC    (AEE_TLANG_ENGLISH_US | AEE_TMODE_MULTITAP | AEE_TMODE_UPPERCASE) 
#define AEE_TMODE_MULTITAP_MC    (AEE_TLANG_ENGLISH_US | AEE_TMODE_MULTITAP | AEE_TMODE_MIXEDCASE)  
#define AEE_TMODE_MULTITAP_SC    (AEE_TLANG_ENGLISH_US | AEE_TMODE_MULTITAP | AEE_TMODE_SENTENCECASE)  


#endif /* __AEETEXTCONTROLLER_H__ */

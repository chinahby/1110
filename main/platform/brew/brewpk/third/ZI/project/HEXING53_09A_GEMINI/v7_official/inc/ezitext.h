/* ezitext.h 
*/
/*****************************************************************************
* COPYRIGHT ZI AND SUBJECT TO CONFIDENTIALITY RESTRICTIONS                   *
*                                                                            *
* This file is the confidential and proprietary property of Zi Corporation   *
* of Canada, Inc. ("Zi") or one of its affiliates.                           *
**************************************************************************** */

/* Note: This file is ANSI C
**
** This file provides the definitions for constants and structures used in the
** eZiText core library.
**
** All of these definitions will generally remain the same regardless
** of platform.
**
*/
#include "ezicmn.h"

/* prevent multiple inclusion */
#ifndef _EZITEXT_H
#define _EZITEXT_H 

/* ************* */
/* Chinese SPACE */
/* ************* */
#ifndef ZI8_ZH_SPACE
#define ZI8_ZH_SPACE           0x0020 /* may want to use 0x3000 ideographic   */
                                      /* space                                */
#endif

#define ZI8_GETCHARSET_ZH              1 /* ZI8_GETCHARSET_GB2312     */
#define ZI8_GETCHARSET_ZH_TW           2 /* ZI8_GETCHARSET_TW1and2    */
#define ZI8_GETCHARSET_ZH_HK           4 
#define ZI8_GETCHARSET_ZH_ALL          7 /* GB2312 and TW1and2 and HK */

#define ZI8_GETCHARSET_GB2312          1
#define ZI8_GETCHARSET_TW1and2         2
#define ZI8_GETCHARSET_GB18030         8
#define ZI8_GETCHARSET_TW1only        16

#define ZI8_SUBLANG_GB2312             0x80
#define ZI8_SUBLANG_GB18030            0x40
#define ZI8_SUBLANG_BIG5LEVEL1         0x20
#define ZI8_SUBLANG_BIG5LEVEL2         0x10
#define ZI8_SUBLANG_HKSCS              0x08

#define ZI8_GETMODE_DEFAULT              0
#define ZI8_GETMODE_STROKES              0
#define ZI8_GETMODE_PINYIN               1
#define ZI8_GETMODE_KEYPRESS_ICON        1
#define ZI8_GETMODE_BOPOMOFO             2
#define ZI8_GETMODE_SYLLABLES            2
#define ZI8_GETMODE_1KEYPRESS_PINYIN     3
#define ZI8_GETMODE_1KEYPRESS_BOPOMOFO   4
#define ZI8_GETMODE_COMPONENTS_ONLY      5
#define ZI8_GETMODE_ROMAJI_2_HIRAGANA    6
#define ZI8_GETMODE_CANGJIE              7
#define ZI8_GETMODE_CJ_WITH_1ST_LAST     8
#define ZI8_GETMODE_CJ_1ST_LAST_ONLY     9
#define ZI8_GETMODE_CJ_COMBINEDADAPTIVE 10
#define ZI8_GETMODE_SHUANGPIN           11
#define ZI8_GETMODE_PHRASE_PINYIN       12
#define ZI8_GETMODE_PHRASE_BOPOMOFO     13
#define ZI8_GETMODE_PHRASE_SHUANGPIN    14
#define ZI8_GETMODE_1KEYPRESS_SHUANGPIN 15
#define ZI8_GETMODE_PHRASE_STROKES      16
#define ZI8_GETMODE_5STROKES            17
#define ZI8_GETMODE_PINYIN_NAME        (0x80 | ZI8_GETMODE_PINYIN)
#define ZI8_GETMODE_BOPOMOFO_NAME      (0x80 | ZI8_GETMODE_BOPOMOFO)


#define ZI8_GETCONTEXT_DEFAULT         0x01
#define ZI8_GETCONTEXT_SMS             0x01
#define ZI8_GETCONTEXT_SURNAME         0x02
#define ZI8_GETCONTEXT_JP_NAME         0x02
#define ZI8_GETCONTEXT_GIVENNAME       0x04
#define ZI8_GETCONTEXT_CHINESE_PUNCT   0x08
#define ZI8_GETCONTEXT_FULLSPELLING    0x03
#define ZI8_GETCONTEXT_INITIALSPELLING 0x05
#define ZI8_GETCONTEXT_NO_RADICALS     0x40  /* ORed with other GetContext */
#define ZI8_GETCONTEXT_NO_DUPLICATE    0x80  /* ORed with other GetContext */
#define ZI8_GETCONTEXT_SINGLE_CHAR     0x20  /* ORed with other GetContext */
#define ZI8_GETCONTEXT_WORD_CAND       0x10  /* ORed with other GetContext */

#define ZI8_GETOPTION_DEFAULT          0
#define ZI8_GETOPTION_CHARSNCOMPS      1
#define ZI8_GETOPTION_NOCOMPONENTS     2
#define ZI8_GETOPTION_NOCOMPLETION     2
#define ZI8_GETOPTION_ALLCOMPONENTS    3
#define ZI8_GETOPTION_SAMESOUND        4
#define ZI8_GETOPTION_WORDCHARCOUNT    5
#define ZI8_GETOPTION_NOZYSYMBOLS      6    /* no zhuyin symbols as cand */
#define ZI8_GETOPTION_NO_PREDICTION    0x10 /* Ored with others */
#define ZI8_GETOPTION_ALLOW_FUZZY      0x20 /* Ored with others (for pinyin) */
#define ZI8_GETOPTION_PRIMARYSOUNDONLY 0x40 /* ORed with others if needed */
#define ZI8_GETOPTION_GET_SPELLING2    0x80
#define ZI8_GETOPTION_GET_SPELLING     0x81
#define ZI8_GETOPTION_STRINGS          0x80
#define ZI8_GETOPTION_WSTRINGS         0x81
#define ZI8_GETOPTION_ROMAJI           1
#define ZI8_GETOPTION_HIRAGANA         2
#define ZI8_GETOPTION_KATAKANA         3
#define ZI8_GETOPTION_COMPOSITE        4
#define ZI8_GETOPTION_SMILEYS          5
#define ZI8_GETOPTION_HALFWIDTH        6 /* 1/2 width katakana */
#define ZI8_GETOPTION_COMPOSITE2       7
#define ZI8_GETOPTION_HIRAGANA_INFO    0x40 /* Ored with option COMPOSITE */
#define ZI8_GETOPTION_STEM_INFO        0x80 /* Ored with other GetOption */
#define ZI8_GETOPTION_RBPROCESS        0x10 /* Ored with other GetOption */
#define ZI8_GETOPTION_INT_RBPROCESS    0x20 /* Ored with other GetOption */

#define ZI8_CHARSET_LATIN1             0
#define ZI8_CHARSET_UNICODE            1
#define ZI8_CHARSET_GSMDEFAULT         2

#define ZI8_SUBLANG_ZH                 1
#define ZI8_SUBLANG_ZH_TW              2
#define ZI8_SUBLANG_ZH_HK              4
#define ZI8_SUBLANG_ZH_ALL             7

#define ZI8_COMPARE_EXACT              0x08
#define ZI8_COMPARE_MASK               0x07
#define ZI8_COMPARE_STROKE             1
#define ZI8_COMPARE_PINYIN             2
#define ZI8_COMPARE_BOPOMOFO           3
#define ZI8_COMPARE_STROKE_BYTYPESONLY 4
#define ZI8_COMPARE_CANGJIE            5
   
#define ZI8_STROKE_WILDCARD            0
#define ZI8_STROKE_DOWN                1
#define ZI8_STROKE_DOT                 2
#define ZI8_STROKE_CURVED_HOOK         3
#define ZI8_STROKE_OVER                4
#define ZI8_STROKE_OVER_DOWN           5
#define ZI8_STROKE_DOWN_OVER           6
#define ZI8_STROKE_LEFT                7
#define ZI8_STROKE_OVER_DOWN_OVER      8
#define ZI8_STROKE_MORE                9
#define ZI8_STROKE_TURN                10
#define ZI8_STROKE_DOT_OR_LEFT         11

#define ZI8_CODE_WILDCARD              (ZI8_BASE_STROKES+ZI8_STROKE_WILDCARD)
#define ZI8_CODE_MORE                  (ZI8_BASE_STROKES+ZI8_STROKE_MORE)
#define ZI8_CODE_LEFT                  (ZI8_BASE_STROKES+ZI8_STROKE_LEFT)
#define ZI8_CODE_OVER                  (ZI8_BASE_STROKES+ZI8_STROKE_OVER)
#define ZI8_CODE_DOT                   (ZI8_BASE_STROKES+ZI8_STROKE_DOT)
#define ZI8_CODE_DOWN                  (ZI8_BASE_STROKES+ZI8_STROKE_DOWN)
#define ZI8_CODE_TURN                  (ZI8_BASE_STROKES+ZI8_STROKE_TURN)
#define ZI8_CODE_DOWN_OVER             (ZI8_BASE_STROKES+ZI8_STROKE_DOWN_OVER)
#define ZI8_CODE_OVER_DOWN             (ZI8_BASE_STROKES+ZI8_STROKE_OVER_DOWN)
#define ZI8_CODE_CURVED_HOOK           (ZI8_BASE_STROKES+ZI8_STROKE_CURVED_HOOK)
#define ZI8_CODE_OVER_DOWN_OVER        (ZI8_BASE_STROKES+ZI8_STROKE_OVER_DOWN_OVER)
#define ZI8_CODE_DOT_OR_LEFT           (ZI8_BASE_STROKES+ZI8_STROKE_DOT_OR_LEFT)

#define ZI8_CODE_LATIN_PUNCT           (ZI8_BASE_LATIN_KEYS+1)
#define ZI8_CODE_LATIN_ABC             (ZI8_BASE_LATIN_KEYS+2)
#define ZI8_CODE_LATIN_DEF             (ZI8_BASE_LATIN_KEYS+3)
#define ZI8_CODE_LATIN_GHI             (ZI8_BASE_LATIN_KEYS+4)
#define ZI8_CODE_LATIN_JKL             (ZI8_BASE_LATIN_KEYS+5)
#define ZI8_CODE_LATIN_MNO             (ZI8_BASE_LATIN_KEYS+6)
#define ZI8_CODE_LATIN_PQRS            (ZI8_BASE_LATIN_KEYS+7)
#define ZI8_CODE_LATIN_TUV             (ZI8_BASE_LATIN_KEYS+8)
#define ZI8_CODE_LATIN_WXYZ            (ZI8_BASE_LATIN_KEYS+9)

#define ZI8_CODE_LATIN_KEY1            0x31
#define ZI8_CODE_LATIN_KEY2            (ZI8_BASE_LATIN_KEYS+2)
#define ZI8_CODE_LATIN_KEY3            (ZI8_BASE_LATIN_KEYS+3)
#define ZI8_CODE_LATIN_KEY4            (ZI8_BASE_LATIN_KEYS+4)
#define ZI8_CODE_LATIN_KEY5            (ZI8_BASE_LATIN_KEYS+5)
#define ZI8_CODE_LATIN_KEY6            (ZI8_BASE_LATIN_KEYS+6)
#define ZI8_CODE_LATIN_KEY7            (ZI8_BASE_LATIN_KEYS+7)
#define ZI8_CODE_LATIN_KEY8            (ZI8_BASE_LATIN_KEYS+8)
#define ZI8_CODE_LATIN_KEY9            (ZI8_BASE_LATIN_KEYS+9)
#define ZI8_CODE_LATIN_KEY0            (ZI8_BASE_LATIN_KEYS+10)

#define ZI8_CODE_SEPARATOR             (ZI8_BASE_PINYIN-1)
#define ZI8_CODE_PHONETIC_SEPARATOR    (ZI8_BASE_PINYIN-1)
#define ZI8_CODE_PINYIN_SEPARATOR      (ZI8_BASE_PINYIN-1)
#define ZI8_CODE_BPMF_SEPARATOR        (ZI8_BASE_PINYIN-1)
#define ZI8_CODE_CANGJIE_SEPARATOR     (0x20) /* ' ' */

#define ZI8_CODE_BPMF_B                (ZI8_BASE_BPMF+0)
#define ZI8_CODE_BPMF_P                (ZI8_BASE_BPMF+1)
#define ZI8_CODE_BPMF_M                (ZI8_BASE_BPMF+2)
#define ZI8_CODE_BPMF_F                (ZI8_BASE_BPMF+3)
#define ZI8_CODE_BPMF_D                (ZI8_BASE_BPMF+4)
#define ZI8_CODE_BPMF_T                (ZI8_BASE_BPMF+5)
#define ZI8_CODE_BPMF_N                (ZI8_BASE_BPMF+6)
#define ZI8_CODE_BPMF_L                (ZI8_BASE_BPMF+7)
#define ZI8_CODE_BPMF_G                (ZI8_BASE_BPMF+8)
#define ZI8_CODE_BPMF_K                (ZI8_BASE_BPMF+9)
#define ZI8_CODE_BPMF_H                (ZI8_BASE_BPMF+10)
#define ZI8_CODE_BPMF_J                (ZI8_BASE_BPMF+11)
#define ZI8_CODE_BPMF_Q                (ZI8_BASE_BPMF+12)
#define ZI8_CODE_BPMF_X                (ZI8_BASE_BPMF+13)
#define ZI8_CODE_BPMF_ZH               (ZI8_BASE_BPMF+14)
#define ZI8_CODE_BPMF_CH               (ZI8_BASE_BPMF+15)
#define ZI8_CODE_BPMF_SH               (ZI8_BASE_BPMF+16)
#define ZI8_CODE_BPMF_R                (ZI8_BASE_BPMF+17)
#define ZI8_CODE_BPMF_Z                (ZI8_BASE_BPMF+18)
#define ZI8_CODE_BPMF_C                (ZI8_BASE_BPMF+19)
#define ZI8_CODE_BPMF_S                (ZI8_BASE_BPMF+20)
#define ZI8_CODE_BPMF_A                (ZI8_BASE_BPMF+21)
#define ZI8_CODE_BPMF_O                (ZI8_BASE_BPMF+22)
#define ZI8_CODE_BPMF_E                (ZI8_BASE_BPMF+23)
#define ZI8_CODE_BPMF_EH               (ZI8_BASE_BPMF+24)
#define ZI8_CODE_BPMF_AI               (ZI8_BASE_BPMF+25)
#define ZI8_CODE_BPMF_EI               (ZI8_BASE_BPMF+26)
#define ZI8_CODE_BPMF_AU               (ZI8_BASE_BPMF+27)
#define ZI8_CODE_BPMF_OU               (ZI8_BASE_BPMF+28)
#define ZI8_CODE_BPMF_AN               (ZI8_BASE_BPMF+29)
#define ZI8_CODE_BPMF_EN               (ZI8_BASE_BPMF+30)
#define ZI8_CODE_BPMF_ANG              (ZI8_BASE_BPMF+31)
#define ZI8_CODE_BPMF_ENG              (ZI8_BASE_BPMF+32)
#define ZI8_CODE_BPMF_ER               (ZI8_BASE_BPMF+33)
#define ZI8_CODE_BPMF_I                (ZI8_BASE_BPMF+34)
#define ZI8_CODE_BPMF_U                (ZI8_BASE_BPMF+35)
#define ZI8_CODE_BPMF_IU               (ZI8_BASE_BPMF+36)

#define ZI8_CODE_PINYIN_A              (ZI8_BASE_PINYIN+0)
#define ZI8_CODE_PINYIN_B              (ZI8_BASE_PINYIN+1)
#define ZI8_CODE_PINYIN_C              (ZI8_BASE_PINYIN+2)
#define ZI8_CODE_PINYIN_D              (ZI8_BASE_PINYIN+3)
#define ZI8_CODE_PINYIN_E              (ZI8_BASE_PINYIN+4)
#define ZI8_CODE_PINYIN_F              (ZI8_BASE_PINYIN+5)
#define ZI8_CODE_PINYIN_G              (ZI8_BASE_PINYIN+6)
#define ZI8_CODE_PINYIN_H              (ZI8_BASE_PINYIN+7)
#define ZI8_CODE_PINYIN_I              (ZI8_BASE_PINYIN+8)
#define ZI8_CODE_PINYIN_J              (ZI8_BASE_PINYIN+9)
#define ZI8_CODE_PINYIN_K              (ZI8_BASE_PINYIN+10)
#define ZI8_CODE_PINYIN_L              (ZI8_BASE_PINYIN+11)
#define ZI8_CODE_PINYIN_M              (ZI8_BASE_PINYIN+12)
#define ZI8_CODE_PINYIN_N              (ZI8_BASE_PINYIN+13)
#define ZI8_CODE_PINYIN_O              (ZI8_BASE_PINYIN+14)
#define ZI8_CODE_PINYIN_P              (ZI8_BASE_PINYIN+15)
#define ZI8_CODE_PINYIN_Q              (ZI8_BASE_PINYIN+16)
#define ZI8_CODE_PINYIN_R              (ZI8_BASE_PINYIN+17)
#define ZI8_CODE_PINYIN_S              (ZI8_BASE_PINYIN+18)
#define ZI8_CODE_PINYIN_T              (ZI8_BASE_PINYIN+19)
#define ZI8_CODE_PINYIN_U              (ZI8_BASE_PINYIN+20)
#define ZI8_CODE_PINYIN_V              (ZI8_BASE_PINYIN+21)
#define ZI8_CODE_PINYIN_W              (ZI8_BASE_PINYIN+22)
#define ZI8_CODE_PINYIN_X              (ZI8_BASE_PINYIN+23)
#define ZI8_CODE_PINYIN_Y              (ZI8_BASE_PINYIN+24)
#define ZI8_CODE_PINYIN_Z              (ZI8_BASE_PINYIN+25)

#define ZI8_CODE_PY_SEPARATOR_A        (ZI8_BASE_PINYIN + 0 - 0x20)
#define ZI8_CODE_PY_SEPARATOR_B        (ZI8_BASE_PINYIN + 1 - 0x20)
#define ZI8_CODE_PY_SEPARATOR_C        (ZI8_BASE_PINYIN + 2 - 0x20)
#define ZI8_CODE_PY_SEPARATOR_D        (ZI8_BASE_PINYIN + 3 - 0x20)
#define ZI8_CODE_PY_SEPARATOR_E        (ZI8_BASE_PINYIN + 4 - 0x20)
#define ZI8_CODE_PY_SEPARATOR_F        (ZI8_BASE_PINYIN + 5 - 0x20)
#define ZI8_CODE_PY_SEPARATOR_G        (ZI8_BASE_PINYIN + 6 - 0x20)
#define ZI8_CODE_PY_SEPARATOR_H        (ZI8_BASE_PINYIN + 7 - 0x20)
#define ZI8_CODE_PY_SEPARATOR_I        (ZI8_BASE_PINYIN + 8 - 0x20)
#define ZI8_CODE_PY_SEPARATOR_J        (ZI8_BASE_PINYIN + 9 - 0x20)
#define ZI8_CODE_PY_SEPARATOR_K        (ZI8_BASE_PINYIN +10 - 0x20)
#define ZI8_CODE_PY_SEPARATOR_L        (ZI8_BASE_PINYIN +11 - 0x20)
#define ZI8_CODE_PY_SEPARATOR_M        (ZI8_BASE_PINYIN +12 - 0x20)
#define ZI8_CODE_PY_SEPARATOR_N        (ZI8_BASE_PINYIN +13 - 0x20)
#define ZI8_CODE_PY_SEPARATOR_O        (ZI8_BASE_PINYIN +14 - 0x20)
#define ZI8_CODE_PY_SEPARATOR_P        (ZI8_BASE_PINYIN +15 - 0x20)
#define ZI8_CODE_PY_SEPARATOR_Q        (ZI8_BASE_PINYIN +16 - 0x20)
#define ZI8_CODE_PY_SEPARATOR_R        (ZI8_BASE_PINYIN +17 - 0x20)
#define ZI8_CODE_PY_SEPARATOR_S        (ZI8_BASE_PINYIN +18 - 0x20)
#define ZI8_CODE_PY_SEPARATOR_T        (ZI8_BASE_PINYIN +19 - 0x20)
#define ZI8_CODE_PY_SEPARATOR_U        (ZI8_BASE_PINYIN +20 - 0x20)
#define ZI8_CODE_PY_SEPARATOR_V        (ZI8_BASE_PINYIN +21 - 0x20)
#define ZI8_CODE_PY_SEPARATOR_W        (ZI8_BASE_PINYIN +22 - 0x20)
#define ZI8_CODE_PY_SEPARATOR_X        (ZI8_BASE_PINYIN +23 - 0x20)
#define ZI8_CODE_PY_SEPARATOR_Y        (ZI8_BASE_PINYIN +24 - 0x20)
#define ZI8_CODE_PY_SEPARATOR_Z        (ZI8_BASE_PINYIN +25 - 0x20)

#define ZI8_CODE_TONES_1               (ZI8_BASE_TONES+0)
#define ZI8_CODE_TONES_2               (ZI8_BASE_TONES+1)
#define ZI8_CODE_TONES_3               (ZI8_BASE_TONES+2)
#define ZI8_CODE_TONES_4               (ZI8_BASE_TONES+3)
#define ZI8_CODE_TONES_5               (ZI8_BASE_TONES+4)

/* ONE KEY PRESS: PINYIN LETTERS MAPPING */
#define ZI8_ONEKEY_PY_EXACT            ZI8_ONEKEY_KEY1
#define ZI8_ONEKEY_PY_A                ZI8_ONEKEY_KEY2
#define ZI8_ONEKEY_PY_B                ZI8_ONEKEY_KEY2
#define ZI8_ONEKEY_PY_C                ZI8_ONEKEY_KEY2
#define ZI8_ONEKEY_PY_D                ZI8_ONEKEY_KEY3
#define ZI8_ONEKEY_PY_E                ZI8_ONEKEY_KEY3
#define ZI8_ONEKEY_PY_F                ZI8_ONEKEY_KEY3
#define ZI8_ONEKEY_PY_G                ZI8_ONEKEY_KEY4
#define ZI8_ONEKEY_PY_H                ZI8_ONEKEY_KEY4
#define ZI8_ONEKEY_PY_I                ZI8_ONEKEY_KEY4
#define ZI8_ONEKEY_PY_J                ZI8_ONEKEY_KEY5
#define ZI8_ONEKEY_PY_K                ZI8_ONEKEY_KEY5
#define ZI8_ONEKEY_PY_L                ZI8_ONEKEY_KEY5
#define ZI8_ONEKEY_PY_M                ZI8_ONEKEY_KEY6
#define ZI8_ONEKEY_PY_N                ZI8_ONEKEY_KEY6
#define ZI8_ONEKEY_PY_O                ZI8_ONEKEY_KEY6
#define ZI8_ONEKEY_PY_P                ZI8_ONEKEY_KEY7
#define ZI8_ONEKEY_PY_Q                ZI8_ONEKEY_KEY7
#define ZI8_ONEKEY_PY_R                ZI8_ONEKEY_KEY7
#define ZI8_ONEKEY_PY_S                ZI8_ONEKEY_KEY7
#define ZI8_ONEKEY_PY_T                ZI8_ONEKEY_KEY8
#define ZI8_ONEKEY_PY_U                ZI8_ONEKEY_KEY8
#define ZI8_ONEKEY_PY_V                ZI8_ONEKEY_KEY8
#define ZI8_ONEKEY_PY_W                ZI8_ONEKEY_KEY9
#define ZI8_ONEKEY_PY_X                ZI8_ONEKEY_KEY9
#define ZI8_ONEKEY_PY_Y                ZI8_ONEKEY_KEY9
#define ZI8_ONEKEY_PY_Z                ZI8_ONEKEY_KEY9

/* ONE KEY PRESS: BOPOMOFO (ZHUYIN) SYMBOLS MAPPING */
#define ZI8_ONEKEY_ZY_EXACT            ZI8_ONEKEY_KEY1
#define ZI8_ONEKEY_ZY_B                ZI8_ONEKEY_KEY1
#define ZI8_ONEKEY_ZY_P                ZI8_ONEKEY_KEY1
#define ZI8_ONEKEY_ZY_M                ZI8_ONEKEY_KEY1
#define ZI8_ONEKEY_ZY_F                ZI8_ONEKEY_KEY1
#define ZI8_ONEKEY_ZY_D                ZI8_ONEKEY_KEY2
#define ZI8_ONEKEY_ZY_T                ZI8_ONEKEY_KEY2
#define ZI8_ONEKEY_ZY_N                ZI8_ONEKEY_KEY2
#define ZI8_ONEKEY_ZY_L                ZI8_ONEKEY_KEY2
#define ZI8_ONEKEY_ZY_G                ZI8_ONEKEY_KEY3
#define ZI8_ONEKEY_ZY_K                ZI8_ONEKEY_KEY3
#define ZI8_ONEKEY_ZY_H                ZI8_ONEKEY_KEY3
#define ZI8_ONEKEY_ZY_J                ZI8_ONEKEY_KEY4
#define ZI8_ONEKEY_ZY_Q                ZI8_ONEKEY_KEY4
#define ZI8_ONEKEY_ZY_X                ZI8_ONEKEY_KEY4
#define ZI8_ONEKEY_ZY_ZH               ZI8_ONEKEY_KEY5
#define ZI8_ONEKEY_ZY_CH               ZI8_ONEKEY_KEY5
#define ZI8_ONEKEY_ZY_SH               ZI8_ONEKEY_KEY5
#define ZI8_ONEKEY_ZY_R                ZI8_ONEKEY_KEY5
#define ZI8_ONEKEY_ZY_Z                ZI8_ONEKEY_KEY6
#define ZI8_ONEKEY_ZY_C                ZI8_ONEKEY_KEY6
#define ZI8_ONEKEY_ZY_S                ZI8_ONEKEY_KEY6
#define ZI8_ONEKEY_ZY_A                ZI8_ONEKEY_KEY7
#define ZI8_ONEKEY_ZY_O                ZI8_ONEKEY_KEY7
#define ZI8_ONEKEY_ZY_E                ZI8_ONEKEY_KEY7
#define ZI8_ONEKEY_ZY_EH               ZI8_ONEKEY_KEY7
#define ZI8_ONEKEY_ZY_AI               ZI8_ONEKEY_KEY8
#define ZI8_ONEKEY_ZY_EI               ZI8_ONEKEY_KEY8
#define ZI8_ONEKEY_ZY_AU               ZI8_ONEKEY_KEY8
#define ZI8_ONEKEY_ZY_OU               ZI8_ONEKEY_KEY8
#define ZI8_ONEKEY_ZY_AN               ZI8_ONEKEY_KEY9
#define ZI8_ONEKEY_ZY_EN               ZI8_ONEKEY_KEY9
#define ZI8_ONEKEY_ZY_ANG              ZI8_ONEKEY_KEY9
#define ZI8_ONEKEY_ZY_ENG              ZI8_ONEKEY_KEY9
#define ZI8_ONEKEY_ZY_ER               ZI8_ONEKEY_KEY9
#define ZI8_ONEKEY_ZY_I                ZI8_ONEKEY_KEY0
#define ZI8_ONEKEY_ZY_U                ZI8_ONEKEY_KEY0
#define ZI8_ONEKEY_ZY_IU               ZI8_ONEKEY_KEY0


#define ZI8RBC_OK                      0
#define ZI8RBC_INVALID_CMD             1
#define ZI8RBC_INVALID_ELEMENT         2
#define ZI8RBC_OVERFLOW                3

#define ZI8RBM_STARTCONVERSION         0
#define ZI8RBM_DONE                    1
#define ZI8RBM_REVERT                  2
#define ZI8RBM_NEXTPHRASE              3
#define ZI8RBM_PREVPHRASE              4
#define ZI8RBM_SLIDERIGHT              5
#define ZI8RBM_SLIDELEFT               6
#define ZI8RBM_GETCAND4PHRASE          7
#define ZI8RBM_NEXTCAND4PHRASE         8
#define ZI8RBM_PREVCAND4PHRASE         9
#define ZI8RBM_SETALGORITHM            10

#define ZI8RBA_DESCEND                 0
#define ZI8RBA_ASCEND                  1
#define ZI8RBA_CENTEROUT               2

#define ZI8_CODE_CANGJIE_WILDCARD      (0x2A) /* '*' */
#define ZI8_CODE_CANGJIE_A             (0x41) /* 'A' */
#define ZI8_CODE_CANGJIE_B             (0x42) /* 'B' */
#define ZI8_CODE_CANGJIE_C             (0x43) /* 'C' */
#define ZI8_CODE_CANGJIE_D             (0x44) /* 'D' */
#define ZI8_CODE_CANGJIE_E             (0x45) /* 'E' */
#define ZI8_CODE_CANGJIE_F             (0x46) /* 'F' */
#define ZI8_CODE_CANGJIE_G             (0x47) /* 'G' */
#define ZI8_CODE_CANGJIE_H             (0x48) /* 'H' */
#define ZI8_CODE_CANGJIE_I             (0x49) /* 'I' */
#define ZI8_CODE_CANGJIE_J             (0x4A) /* 'J' */
#define ZI8_CODE_CANGJIE_K             (0x4B) /* 'K' */
#define ZI8_CODE_CANGJIE_L             (0x4C) /* 'L' */
#define ZI8_CODE_CANGJIE_M             (0x4D) /* 'M' */
#define ZI8_CODE_CANGJIE_N             (0x4E) /* 'N' */
#define ZI8_CODE_CANGJIE_O             (0x4F) /* 'O' */
#define ZI8_CODE_CANGJIE_P             (0x50) /* 'P' */
#define ZI8_CODE_CANGJIE_Q             (0x51) /* 'Q' */
#define ZI8_CODE_CANGJIE_R             (0x52) /* 'R' */
#define ZI8_CODE_CANGJIE_S             (0x53) /* 'S' */
#define ZI8_CODE_CANGJIE_T             (0x54) /* 'T' */
#define ZI8_CODE_CANGJIE_U             (0x55) /* 'U' */
#define ZI8_CODE_CANGJIE_V             (0x56) /* 'V' */
#define ZI8_CODE_CANGJIE_W             (0x57) /* 'W' */
#define ZI8_CODE_CANGJIE_X             (0x58) /* 'X' */
#define ZI8_CODE_CANGJIE_Y             (0x59) /* 'Y' */

#define ZI8_ONEKEY_CJ_A                ZI8_ONEKEY_KEY2
#define ZI8_ONEKEY_CJ_B                ZI8_ONEKEY_KEY2
#define ZI8_ONEKEY_CJ_C                ZI8_ONEKEY_KEY2
#define ZI8_ONEKEY_CJ_D                ZI8_ONEKEY_KEY3
#define ZI8_ONEKEY_CJ_E                ZI8_ONEKEY_KEY3
#define ZI8_ONEKEY_CJ_F                ZI8_ONEKEY_KEY3
#define ZI8_ONEKEY_CJ_G                ZI8_ONEKEY_KEY4
#define ZI8_ONEKEY_CJ_H                ZI8_ONEKEY_KEY4
#define ZI8_ONEKEY_CJ_I                ZI8_ONEKEY_KEY4
#define ZI8_ONEKEY_CJ_J                ZI8_ONEKEY_KEY5
#define ZI8_ONEKEY_CJ_K                ZI8_ONEKEY_KEY5
#define ZI8_ONEKEY_CJ_L                ZI8_ONEKEY_KEY5
#define ZI8_ONEKEY_CJ_M                ZI8_ONEKEY_KEY6
#define ZI8_ONEKEY_CJ_N                ZI8_ONEKEY_KEY6
#define ZI8_ONEKEY_CJ_O                ZI8_ONEKEY_KEY6
#define ZI8_ONEKEY_CJ_P                ZI8_ONEKEY_KEY7
#define ZI8_ONEKEY_CJ_Q                ZI8_ONEKEY_KEY7
#define ZI8_ONEKEY_CJ_R                ZI8_ONEKEY_KEY7
#define ZI8_ONEKEY_CJ_S                ZI8_ONEKEY_KEY7
#define ZI8_ONEKEY_CJ_T                ZI8_ONEKEY_KEY8
#define ZI8_ONEKEY_CJ_U                ZI8_ONEKEY_KEY8
#define ZI8_ONEKEY_CJ_V                ZI8_ONEKEY_KEY8
#define ZI8_ONEKEY_CJ_W                ZI8_ONEKEY_KEY9
#define ZI8_ONEKEY_CJ_X                ZI8_ONEKEY_KEY9
#define ZI8_ONEKEY_CJ_Y                ZI8_ONEKEY_KEY9

#define ZI8_CODE_SP_A                  (0x61) /* 'a' */
#define ZI8_CODE_SP_B                  (0x62) /* 'b' */
#define ZI8_CODE_SP_C                  (0x63) /* 'c' */
#define ZI8_CODE_SP_D                  (0x64) /* 'd' */
#define ZI8_CODE_SP_E                  (0x65) /* 'e' */
#define ZI8_CODE_SP_F                  (0x66) /* 'f' */
#define ZI8_CODE_SP_G                  (0x67) /* 'g' */
#define ZI8_CODE_SP_H                  (0x68) /* 'h' */
#define ZI8_CODE_SP_I                  (0x69) /* 'i' */
#define ZI8_CODE_SP_J                  (0x6A) /* 'j' */
#define ZI8_CODE_SP_K                  (0x6B) /* 'k' */
#define ZI8_CODE_SP_L                  (0x6C) /* 'l' */
#define ZI8_CODE_SP_M                  (0x6D) /* 'm' */
#define ZI8_CODE_SP_N                  (0x6E) /* 'n' */
#define ZI8_CODE_SP_O                  (0x6F) /* 'o' */
#define ZI8_CODE_SP_P                  (0x70) /* 'p' */
#define ZI8_CODE_SP_Q                  (0x71) /* 'q' */
#define ZI8_CODE_SP_R                  (0x72) /* 'r' */
#define ZI8_CODE_SP_S                  (0x73) /* 's' */
#define ZI8_CODE_SP_T                  (0x74) /* 't' */
#define ZI8_CODE_SP_U                  (0x75) /* 'u' */
#define ZI8_CODE_SP_V                  (0x76) /* 'v' */
#define ZI8_CODE_SP_W                  (0x77) /* 'w' */
#define ZI8_CODE_SP_X                  (0x78) /* 'x' */
#define ZI8_CODE_SP_Y                  (0x79) /* 'y' */
#define ZI8_CODE_SP_Z                  (0x7A) /* 'z' */
#define ZI8_CODE_SP_SEMICOLON          (0x3B) /* ';' */

#define ZI8_PARENTALCONTROLS_NONE      0
#define ZI8_PARENTALCONTROLS_LOW       1
#define ZI8_PARENTALCONTROLS_MEDIUM    2
#define ZI8_PARENTALCONTROLS_HIGH      3

/* ******************** */
/* structure of the API */
/* ******************** */

    typedef struct _ZI8BLOCKLIST
    {
        ZI8UCHAR   Language;
        ZI8USHORT  NumOfWords;
        PZI8WCHAR *Words;
    } ZI8BLOCKLIST;
    typedef struct _ZI8BLOCKLIST ZI8RAMPOINTER PZI8BLOCKLIST;

    typedef struct _ZI8GETPARAM
    {
        ZI8UCHAR  Language;
        ZI8UCHAR  GetMode;
        ZI8UCHAR  SubLanguage;
        ZI8UCHAR  Context;
        ZI8UCHAR  GetOptions;
        PZI8WCHAR pElements;
        ZI8UCHAR  ElementCount;
        PZI8WCHAR pCurrentWord;
        ZI8UCHAR  WordCharacterCount;
        PZI8WCHAR pCandidates;
        ZI8UCHAR  MaxCandidates;
        ZI8WCHAR  FirstCandidate;
        ZI8UCHAR  wordCandidates;
        ZI8UCHAR  count;
        ZI8UCHAR  letters;
        PZI8UCHAR pScratch;
    } ZI8GETPARAM;
    typedef struct _ZI8GETPARAM ZI8RAMPOINTER PZI8GETPARAM;

    /* For Japanese:
       correlates element groups in the element buffer to the character
       groups (word) in the candidate buffer
    */
    typedef struct _ZI8_CANDIDATE_INFO
    {
        ZI8UCHAR ElementItem;
        ZI8UCHAR CandidateItem;
    } ZI8_CANDIDATE_INFO;
    typedef struct _ZI8_CANDIDATE_INFO ZI8RAMPOINTER PZI8_CANDIDATE_INFO;

    typedef struct _ZI8LISTPARAM
    {
        PZI8UCHAR pPhoneBook;
        ZI8UCHAR  OffsetAlphaField;
        ZI8UCHAR  AlphaFieldSize;
        ZI8UCHAR  EntrySize;
        ZI8UCHAR  Entries;
        PZI8UCHAR pScratch;
    } ZI8LISTPARAM;
    typedef struct _ZI8LISTPARAM ZI8RAMPOINTER PZI8LISTPARAM;

    typedef PZI8UCHAR (*PFZI8PHONEBOOK)(ZI8USHORT ElementIndex);

    typedef struct _ZI8LISTFUNCPARAM
    {
        PFZI8PHONEBOOK pPhoneBook;
        ZI8UCHAR       OffsetAlphaField;
        ZI8UCHAR       AlphaFieldSize;
        ZI8UCHAR       EntrySize;
        ZI8UCHAR       Entries;
        PZI8UCHAR      pScratch;
    } ZI8LISTFUNCPARAM;
    typedef struct _ZI8LISTFUNCPARAM ZI8RAMPOINTER PZI8LISTFUNCPARAM;

    typedef struct _ZI8RBPARAMS
    {
        ZI8UCHAR    Language;
        ZI8UCHAR    rbCommand;
        PZI8WCHAR   pElements;
        ZI8UCHAR    ElementCount;
        ZI8UCHAR    Context;
        PZI8WCHAR   pCompositeSentence;
        ZI8UCHAR    SentenceSize;
        ZI8UCHAR    NumOfPhrase;
        ZI8UCHAR    PhraseIndex;
        PZI8WCHAR   pAltCandidates;
        ZI8UCHAR    AltCandidatesSize;
        ZI8UCHAR    MaxCandidateCount;
        ZI8USHORT   TotalCandidateCount;
        ZI8UCHAR    CandidateIndex;
        ZI8UCHAR    CandidateArrangement;
        ZI8UCHAR    InternalUseBuffer[ZI8_INTERNAL_DATA_SIZE1];
    } ZI8RBPARAMS;
    typedef struct _ZI8RBPARAMS ZI8RAMPOINTER PZI8RBPARAMS;

    typedef struct _ZI8_FUZZY_PY_PAIRS
    {
        int ziDefault : 1;
        int cANDch    : 1;
        int sANDsh    : 1;
        int zANDzh    : 1;
        int anANDang  : 1;
        int enANDeng  : 1;
        int inANDing  : 1;
        int nANDl     : 1;
        int lANDr     : 1;
        int fANDh     : 1;
    } ZI8_FUZZY_PY_PAIRS;

    typedef struct _ZI8_FUZZY_ZY_PAIRS
    {
        int ziDefault : 1;
        int cANDch    : 1;
        int sANDsh    : 1;
        int zANDzh    : 1;
        int anANDang  : 1;
        int enANDeng  : 1;
        int nANDl     : 1;
        int lANDr     : 1;
        int fANDh     : 1;
        int rANDn     : 1;
        int bANDp     : 1;
        int gANDk     : 1;
    } ZI8_FUZZY_ZY_PAIRS;


/* ******************************* */
/* prototypes of the API functions */
/* ******************************* */

    ZI8FUNCTION ZI8_IMPORT_C ZI8BOOL  Zi8Initialize
                                              ( void ZI8_GBL_PARM1 );
    ZI8FUNCTION ZI8_IMPORT_C ZI8BOOL  Zi8InitializeDynamic
                                              ( PZI8VOID p
                                                ZI8_GBL_PARM );
    ZI8FUNCTION ZI8_IMPORT_C ZI8WCHAR Zi8GetVersion
                                              ( void ZI8_GBL_PARM1 );
    ZI8FUNCTION ZI8_IMPORT_C ZI8WCHAR Zi8GetBuildID
                                              ( void ZI8_GBL_PARM1 );
    ZI8FUNCTION ZI8_IMPORT_C ZI8WCHAR Zi8GetOEMID
                                              ( void ZI8_GBL_PARM1 );
    ZI8FUNCTION ZI8_IMPORT_C ZI8BOOL  Zi8SetCompareMode
                                              ( ZI8UCHAR compareMode,
                                                ZI8UCHAR language
                                                ZI8_GBL_PARM );
    ZI8FUNCTION ZI8_IMPORT_C int      Zi8Compare
                                              ( const PZI8VOID string1,
                                                const PZI8VOID string2 
                                                ZI8_GBL_PARM );
    ZI8FUNCTION ZI8_IMPORT_C ZI8BOOL  Zi8IsComponent
                                              ( ZI8WCHAR wc 
                                                ZI8_GBL_PARM );
    ZI8FUNCTION ZI8_IMPORT_C ZI8BOOL  Zi8IsCharacter
                                              ( ZI8WCHAR wc 
                                                ZI8_GBL_PARM );
    ZI8FUNCTION ZI8_IMPORT_C ZI8BOOL  Zi8IsElement
                                              ( ZI8WCHAR wc 
                                                ZI8_GBL_PARM );
    ZI8FUNCTION ZI8_IMPORT_C ZI8BOOL  Zi8SetMaxNamesInUse
                                              ( ZI8USHORT usMaxSize 
                                                ZI8_GBL_PARM );
    ZI8FUNCTION ZI8_IMPORT_C ZI8UCHAR Zi8GetCandidates
                                              ( PZI8GETPARAM pGetParam 
                                                ZI8_GBL_PARM );
    ZI8FUNCTION ZI8_IMPORT_C ZI8UCHAR Zi8ETgetCandidatesEx
                                              ( PZI8GETPARAM pGetParam 
                                                ZI8_GBL_PARM );
    ZI8FUNCTION ZI8_IMPORT_C ZI8LONG  Zi8GetCandidatesCount
                                              ( PZI8GETPARAM pGetParam 
                                                ZI8_GBL_PARM );
    ZI8FUNCTION ZI8_IMPORT_C ZI8BOOL  Zi8IsMatch
                                              ( PZI8GETPARAM pGetParam,
                                                ZI8WCHAR c 
                                                ZI8_GBL_PARM );
    ZI8FUNCTION ZI8_IMPORT_C ZI8BOOL  Zi8IsMatch2
                                              ( PZI8GETPARAM pGetParam,
                                                PZI8WCHAR pWord 
                                                ZI8_GBL_PARM );
    ZI8FUNCTION ZI8_IMPORT_C ZI8UCHAR Zi8GetListCandidates
                                              ( PZI8GETPARAM pGetParam,
                                                PZI8LISTPARAM pListParam 
                                                ZI8_GBL_PARM );
    ZI8FUNCTION ZI8_IMPORT_C ZI8UCHAR Zi8GetListCandidatesFunc
                                              ( PZI8GETPARAM pGetParam,
                                                PZI8LISTFUNCPARAM pListParam
                                                ZI8_GBL_PARM );
    ZI8FUNCTION ZI8_IMPORT_C ZI8BOOL  Zi8IsWordW
                                              ( PZI8WCHAR pWord,
                                                ZI8UCHAR language
                                                ZI8_GBL_PARM );
    ZI8FUNCTION ZI8_IMPORT_C void     Zi8GetAlphaWord
                                              ( ZI8UCHAR language,
                                                ZI8WCHAR wcCandidate,
                                                PZI8UCHAR szString,
                                                ZI8UCHAR size
                                                ZI8_GBL_PARM );
    ZI8FUNCTION ZI8_IMPORT_C void     Zi8GetAlphaWordW
                                              ( ZI8UCHAR language,
                                                ZI8WCHAR wcCandidate,
                                                PZI8WCHAR pwString,
                                                ZI8UCHAR size
                                                ZI8_GBL_PARM );
    ZI8FUNCTION ZI8_IMPORT_C ZI8UCHAR Zi8GetChecksum
                                              ( ZI8UCHAR language,
                                                ZI8UCHAR TableType
                                                ZI8_GBL_PARM );
    /* optional routines for Chinese character conversions */
    ZI8FUNCTION ZI8_IMPORT_C ZI8WCHAR Zi8UnicodeToGB
                                              ( ZI8WCHAR wc
                                                ZI8_GBL_PARM );
    ZI8FUNCTION ZI8_IMPORT_C ZI8WCHAR Zi8UnicodeToBig5
                                              ( ZI8WCHAR wc
                                                ZI8_GBL_PARM );
    ZI8FUNCTION ZI8_IMPORT_C ZI8WCHAR Zi8GBToUnicode
                                              ( ZI8WCHAR wc
                                                ZI8_GBL_PARM );
    ZI8FUNCTION ZI8_IMPORT_C ZI8WCHAR Zi8Big5ToUnicode
                                              ( ZI8WCHAR wc
                                                ZI8_GBL_PARM );
    ZI8FUNCTION ZI8_IMPORT_C ZI8WCHAR Zi8ComplexToSimp
                                              ( PZI8WCHAR pCharacter,
                                                ZI8USHORT count
                                                ZI8_GBL_PARM );
    ZI8FUNCTION ZI8_IMPORT_C ZI8WCHAR Zi8SimpToComplex
                                              ( PZI8WCHAR pCharacter,
                                                ZI8USHORT count
                                                ZI8_GBL_PARM );
    ZI8FUNCTION ZI8_IMPORT_C ZI8BOOL  Zi8SetZHsubLangTo
                                              ( ZI8UCHAR SubLanguage
                                                ZI8_GBL_PARM );
    ZI8FUNCTION ZI8_IMPORT_C ZI8USHORT Zi8SetCharInfoBuffer
                                              ( PZI8WCHAR p,
                                                ZI8USHORT usSize,
                                                ZI8UCHAR  SubLanguage
                                                ZI8_GBL_PARM );
    ZI8FUNCTION ZI8_IMPORT_C ZI8UCHAR Zi8GetCharInfo
                                              ( ZI8WCHAR wc,
                                                PZI8WCHAR pCharInfoBuffer,
                                                ZI8UCHAR MaxInfoBuffer,
                                                ZI8UCHAR GetMode
                                                ZI8_GBL_PARM );
    ZI8FUNCTION ZI8_IMPORT_C ZI8UCHAR Zi8GetCharInfo2
                                              ( ZI8WCHAR wc,
                                                PZI8WCHAR pCharInfoBuffer,
                                                ZI8UCHAR MaxInfoBuffer,
                                                ZI8UCHAR GetMode,
                                                PZI8WCHAR pElements,
                                                ZI8UCHAR uElementCount
                                                ZI8_GBL_PARM );
    ZI8FUNCTION ZI8_IMPORT_C ZI8UCHAR Zi8GetCharInfo3
                                              ( ZI8WCHAR wc,
                                                PZI8WCHAR pCharInfoBuffer,
                                                ZI8UCHAR MaxInfoBuffer,
                                                ZI8UCHAR GetMode,
                                                PZI8WCHAR pElements,
                                                ZI8UCHAR uElementCount
                                                ZI8_GBL_PARM );
    ZI8FUNCTION ZI8_IMPORT_C ZI8WCHAR Zi8GetCompID
                                              ( ZI8WCHAR wc
                                                ZI8_GBL_PARM );
    ZI8FUNCTION ZI8_IMPORT_C ZI8UCHAR Zi8GetCompInfo
                                              ( ZI8WCHAR compID,
                                                PZI8WCHAR pInfo
                                                ZI8_GBL_PARM );

    ZI8FUNCTION ZI8_IMPORT_C ZI8VOID Zi8SetCandidateCase
                                              ( ZI8UCHAR ucCaseID
                                                ZI8_GBL_PARM );

    ZI8FUNCTION ZI8_IMPORT_C ZI8BOOL Zi8ChangeCharCase
                                              ( ZI8BOOL   bUpper, 
                                                PZI8WCHAR pChar, 
                                                ZI8UCHAR language
                                                ZI8_GBL_PARM );

    /* These helper functions have been added to extract strings from
       the candidates buffer when the candidate words are inline and separated
       by zero value.
    */
    ZI8FUNCTION ZI8_IMPORT_C ZI8UCHAR Zi8Copy(  PZI8UCHAR pDestination,
                                                PZI8GETPARAM pGetParams,
                                                ZI8UCHAR index
                                                ZI8_GBL_PARM );
    ZI8FUNCTION ZI8_IMPORT_C ZI8UCHAR Zi8CopyW( PZI8WCHAR pDestination,
                                                PZI8GETPARAM pGetParams,
                                                ZI8UCHAR index
                                                ZI8_GBL_PARM );
    ZI8FUNCTION ZI8_IMPORT_C ZI8UCHAR Zi8CopyWordList
                                              ( PZI8UCHAR pDestination,
                                                ZI8UCHAR  MaxChar,
                                                PZI8UCHAR pWordList,
                                                ZI8UCHAR index
                                                ZI8_GBL_PARM );
    ZI8FUNCTION ZI8_IMPORT_C ZI8UCHAR Zi8CopyWordListW
                                              ( PZI8WCHAR pDestination,
                                                ZI8UCHAR  MaxChar,
                                                PZI8WCHAR pWordList,
                                                ZI8UCHAR index
                                                ZI8_GBL_PARM );
    ZI8FUNCTION ZI8_IMPORT_C ZI8BOOL Zi8AddUsedKanji
                                              ( ZI8UCHAR UWD_id,
                                                PZI8WCHAR pKanjiWord,
                                                PZI8WCHAR pHiragana
                                                ZI8_GBL_PARM );
    ZI8FUNCTION ZI8_IMPORT_C ZI8UCHAR Zi8SetUWDsearchOption
                                              ( ZI8UCHAR newOpt
                                                ZI8_GBL_PARM );
    ZI8FUNCTION ZI8_IMPORT_C ZI8UCHAR Zi8RBprocess
                                              ( PZI8RBPARAMS pRBParams
                                                ZI8_GBL_PARM );

    /* For Japanese: Feature of Echo Kana on/off Default: off */
    ZI8FUNCTION ZI8_IMPORT_C ZI8BOOL Zi8JPEchoKana
                                              ( ZI8BOOL bEchoKanaIn
                                                ZI8_GBL_PARM );

    /* For Japanese: Ending Filter on/off Default: on */
    ZI8FUNCTION ZI8_IMPORT_C ZI8BOOL Zi8JPEndingFilter
                                              ( ZI8BOOL bEndingFilterIn
                                                ZI8_GBL_PARM );

    ZI8FUNCTION ZI8_IMPORT_C ZI8USHORT Zi8ConvertSP2PY
                                              ( PZI8WCHAR spBuff,
                                                ZI8USHORT spBuffSize,
                                                PZI8WCHAR pyBuff,
                                                ZI8USHORT pyBuffSize
                                                ZI8_GBL_PARM );

    ZI8FUNCTION ZI8_IMPORT_C ZI8USHORT Zi8ConvertPY2ZY
                                              ( PZI8WCHAR pyBuff,
                                                ZI8WCHAR  pyBuffSize,
                                                PZI8WCHAR zyBuff,
                                                ZI8WCHAR  zyBuffSize
                                                ZI8_GBL_PARM );

    ZI8FUNCTION ZI8_IMPORT_C ZI8USHORT Zi8ConvertZY2PY
                                              ( PZI8WCHAR pyBuff,
                                                ZI8WCHAR  pyBuffSize,
                                                PZI8WCHAR zyBuff,
                                                ZI8WCHAR  zyBuffSize
                                                ZI8_GBL_PARM );

    ZI8FUNCTION ZI8_IMPORT_C ZI8VOID Zi8SetCandidateBufferSize
                                              ( ZI8USHORT size
                                                ZI8_GBL_PARM );

    ZI8FUNCTION ZI8_IMPORT_C ZI8USHORT Zi8GetCandidateBufferSize
                                              ( void 
                                                ZI8_GBL_PARM1 );

    ZI8FUNCTION ZI8_IMPORT_C ZI8BOOL  Zi8SetHighlightedWordW
                                              ( PZI8WCHAR pwcWord,
                                                ZI8UCHAR Language,
                                                ZI8UCHAR SubLanguage
                                                ZI8_GBL_PARM );

    ZI8FUNCTION ZI8_IMPORT_C ZI8BOOL  Zi8SetPrefixLockControl
                                              ( ZI8UCHAR prefixLockControl
                                                ZI8_GBL_PARM );


    ZI8FUNCTION ZI8_IMPORT_C ZI8WCHAR  Zi8GetZHWordList
                                              ( PZI8WCHAR pCurrentWord,
                                                ZI8UCHAR  WordCharacterCount,
                                                ZI8UCHAR  SubLanguage,
                                                ZI8WCHAR  FirstCandidate,
                                                PZI8WCHAR pCandidates,
                                                ZI8UCHAR  MaxCandidates,
                                                ZI8WCHAR  MaxCandBufferSize,
                                                ZI8BOOL   bCompleteWord
                                                ZI8_GBL_PARM );

    ZI8FUNCTION ZI8_IMPORT_C ZI8UCHAR Zi8AttachUWD_ZH
                                              ( PZI8UCHAR pUWD_ZH,
                                                ZI8USHORT size
                                                ZI8_GBL_PARM );

    ZI8FUNCTION ZI8_IMPORT_C ZI8UCHAR Zi8AttachUWD_ZH2
                                              ( PZI8UCHAR pUWD_ZH,
                                                ZI8USHORT size,
                                                ZI8UCHAR  SubLanguage
                                                ZI8_GBL_PARM );

    ZI8FUNCTION ZI8_IMPORT_C ZI8BOOL Zi8DetachUWD_ZH
                                              ( ZI8UCHAR UWD_ZH_id,
                                                PZI8UCHAR pUWD_ZH
                                                ZI8_GBL_PARM );

    ZI8FUNCTION ZI8_IMPORT_C ZI8BOOL Zi8AddUsedWordW_ZH
                                              ( ZI8UCHAR UWD_ZH_id,
                                                PZI8WCHAR pWord,
                                                ZI8USHORT size
                                                ZI8_GBL_PARM );

    ZI8FUNCTION ZI8_IMPORT_C ZI8ULONG Zi8AlphaCheckSum
                                              ( ZI8UCHAR Language
                                                ZI8_GBL_PARM );

    ZI8FUNCTION ZI8_IMPORT_C ZI8BOOL Zi8FKPWordW
                                              ( ZI8UCHAR language,
                                                ZI8WCHAR wcCandidate,
                                                PZI8WCHAR pwString,
                                                ZI8UCHAR size
                                                ZI8_GBL_PARM );

    ZI8FUNCTION ZI8_IMPORT_C ZI8BOOL Zi8FKPWord
                                              ( ZI8UCHAR language,
                                                ZI8WCHAR wcCandidate,
                                                PZI8UCHAR szString,
                                                ZI8UCHAR size
                                                ZI8_GBL_PARM );

    ZI8FUNCTION ZI8_IMPORT_C ZI8BOOL Zi8ZHsetSeparator
                                              ( ZI8WCHAR separator
                                                ZI8_GBL_PARM );

    ZI8FUNCTION ZI8_IMPORT_C ZI8BOOL Zi8ZHaddSpace
                                              ( PZI8WCHAR pElements,
                                                ZI8UCHAR ElementCount,
                                                PZI8WCHAR pCandidates,
                                                ZI8WCHAR MaxCandSize
                                                ZI8_GBL_PARM );

    ZI8FUNCTION ZI8_IMPORT_C ZI8UCHAR Zi8GetZHCharSet
                                              ( void ZI8_GBL_PARM1 );

    ZI8FUNCTION ZI8_IMPORT_C ZI8BOOL Zi8IsZHSupported
                                              ( ZI8UCHAR SubLanguage,
                                                ZI8UCHAR GetMode
                                                ZI8_GBL_PARM );

    ZI8FUNCTION ZI8_IMPORT_C ZI8WCHAR Zi8ConvertKO2WC
                                              ( ZI8WCHAR wc_ko
                                                ZI8_GBL_PARM );

    ZI8FUNCTION ZI8_IMPORT_C ZI8WCHAR Zi8ConvertWC2KO
                                              ( ZI8WCHAR wc
                                                ZI8_GBL_PARM );

    ZI8FUNCTION ZI8_IMPORT_C ZI8UCHAR Zi8GetElementCount
                                              ( PZI8WCHAR pElements,
                                                ZI8UCHAR  ElementCount,
                                                ZI8UCHAR  numOfChar
                                                ZI8_GBL_PARM );

    /* functions for the Personal User Dictionary specific to eZiText */
    ZI8FUNCTION ZI8_IMPORT_C PZI8VOID Zi8GetUDWordW
                                              ( ZI8UCHAR Pud,
                                                PZI8WCHAR pWord,
                                                ZI8UCHAR MaxChar, 
                                                PZI8GETPARAM pGetParam,
                                                PZI8VOID pPlace,
                                                ZI8BOOL bReverse
                                                ZI8_GBL_PARM );
    ZI8FUNCTION ZI8_IMPORT_C ZI8LONG  Zi8GetUDWordWCount
                                              ( ZI8UCHAR Pud,
                                                PZI8GETPARAM pGetParam
                                                ZI8_GBL_PARM );

    /* functions for Fuzzy Text */
    ZI8FUNCTION ZI8_IMPORT_C ZI8UCHAR Zi8FuzzyText
                                              ( PZI8GETPARAM pGetParam 
                                                ZI8_GBL_PARM );
    ZI8FUNCTION ZI8_IMPORT_C ZI8BOOL Zi8IsCorrected
                                              ( void ZI8_GBL_PARM1 );
    ZI8FUNCTION ZI8_IMPORT_C ZI8BOOL Zi8SetFuzzyMaxError
                                              ( ZI8UCHAR maxError
                                                ZI8_GBL_PARM );
    ZI8FUNCTION ZI8_IMPORT_C ZI8BOOL Zi8SetFuzzyLayoutInfo
                                              ( ZI8BOOL bAllow
                                                ZI8_GBL_PARM );
    ZI8FUNCTION ZI8_IMPORT_C ZI8BOOL Zi8SetFuzzySearchOption
                                              ( ZI8BOOL bFullSearch 
                                                ZI8_GBL_PARM );

    ZI8FUNCTION ZI8_IMPORT_C ZI8BOOL Zi8Terminate(
                                                void ZI8_GBL_PARM1 );
    ZI8FUNCTION ZI8_IMPORT_C ZI8BOOL Zi8SetFileBufferSize
                                              ( ZI8USHORT usSize 
                                                ZI8_GBL_PARM );

    ZI8FUNCTION ZI8_IMPORT_C ZI8BOOL Zi8ZHsetPYfuzzyPairs
                                              ( ZI8_FUZZY_PY_PAIRS pairsList
                                                ZI8_GBL_PARM );

    ZI8FUNCTION ZI8_IMPORT_C ZI8BOOL Zi8ZHsetZYfuzzyPairs
                                              ( ZI8_FUZZY_ZY_PAIRS pairsList
                                                ZI8_GBL_PARM );

    ZI8FUNCTION ZI8_IMPORT_C ZI8_FUZZY_PY_PAIRS Zi8ZHgetPYfuzzyPairs(
                                                void ZI8_GBL_PARM1 );

    ZI8FUNCTION ZI8_IMPORT_C ZI8_FUZZY_ZY_PAIRS Zi8ZHgetZYfuzzyPairs(
                                                void ZI8_GBL_PARM1 );

    ZI8FUNCTION ZI8_IMPORT_C ZI8BOOL Zi8SetParentalControls
                                              ( ZI8UCHAR ucLevel
                                                ZI8_GBL_PARM );

    ZI8FUNCTION ZI8_IMPORT_C ZI8BOOL Zi8AttachBlockLists
                                              ( PZI8BLOCKLIST pBlockLists
                                                ZI8_GBL_PARM );
    ZI8FUNCTION ZI8_IMPORT_C ZI8BOOL Zi8DetachBlockLists
                                              ( void ZI8_GBL_PARM1 );

    typedef PZI8VOID (*PFZIMALLOC)( ZI8USHORT size, PZI8VOID pOther );

    ZI8FUNCTION ZI8_IMPORT_C ZI8BOOL ZiAttachMallocFunc
                                              ( PFZIMALLOC pMallocFunc,
                                                PZI8VOID  pOther
                                                ZI8_GBL_PARM );

    ZI8FUNCTION ZI8_IMPORT_C PZI8VOID ZiGetNextNode
                                              ( PZI8VOID pRef,
                                                ZI8WCHAR wChar,
                                                ZI8UCHAR Language
                                                ZI8_GBL_PARM );

    ZI8FUNCTION ZI8_IMPORT_C ZI8UCHAR ZiGetNodeInfo
                                              ( PZI8VOID  pRef,
                                                PZI8WCHAR pCandBuff,
                                                ZI8USHORT CandBuffSize,
                                                ZI8UCHAR  MaxCandidates,
                                                ZI8WCHAR  FirstCandidate,
                                                PZI8UCHAR pMinVal,
                                                PZI8UCHAR pMaxVal,
                                                ZI8UCHAR  PredictedMaxLength
                                                ZI8_GBL_PARM );

#endif /* #ifndef _EZITEXT_H */

/*
  ========================================================================

  FILE:          AEELocale.h

  SERVICES:      Locale specific string and text formatting utilities.

  PUBLIC CLASSES:ILocale

  DESCRIPTION:   This file describes an interface that provides language
                 dependent string formatting for a specific language. The
                 intent is that there be one of these objects for each
                 language your device supports.  Addition implementations
                 may be added later to support additional languages.
  ========================================================================
  ========================================================================
    
               Copyright © 1999-2006 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#ifndef __AEELOCALE__
#define __AEELOCALE__

/*---------------------------------------------------------------------------------
      Include Files
---------------------------------------------------------------------------------*/

#include "AEEStdLib.h"

#include "bid/AEEIID_LOCALE.bid"
#include "bid/AEECLSID_LOCALEENUS.bid"
#include "bid/AEECLSID_CURRENTLOCALE.bid"

/*---------------------------------------------------------------------------------
      Class IDs
---------------------------------------------------------------------------------*/



/*---------------------------------------------------------------------------------
      Type Declarations
---------------------------------------------------------------------------------*/

// FormatItem argument
typedef struct
{
   char cType;
   union
   {
      int32 d;       // signed decimal
      uint32 u;      // unsigned decimal
      char c;        // character
      AECHAR C;      // AECHAR
      char *s;       // char *
      AECHAR *S;     // AECHAR *
      void *p;       // pointer
      JulianType j;  // julian date
   } u;
} AEELocaleArg;

// AEELocaleArg Types
#define AEELOCALEARGTYPE_SIGNEDDECIMAL    'd'
#define AEELOCALEARGTYPE_UNSIGNEDDECIMAL  'u'
#define AEELOCALEARGTYPE_CHAR             'c'
#define AEELOCALEARGTYPE_AECHAR           'C'
#define AEELOCALEARGTYPE_CHARPTR          's' 
#define AEELOCALEARGTYPE_AECHARPTR        'S'
#define AEELOCALEARGTYPE_POINTER          'p'
#define AEELOCALEARGTYPE_JULIANDATE       'j'


// Valid ILocale Format Codes

// number formats, case insensitive
#define ILOCALE_FORMATCODE_NUMBER_CURRENCY               ((AECHAR)'c')
#define ILOCALE_FORMATCODE_NUMBER_SIGNEDDECIMAL          ((AECHAR)'d')
#define ILOCALE_FORMATCODE_NUMBER_UNSIGNEDDECIMAL        ((AECHAR)'u')
//#define ILOCALE_FORMATCODE_NUMBER_SCIENTIFIC           ((AECHAR)'e')
#define ILOCALE_FORMATCODE_NUMBER_FIXEDPOINT             ((AECHAR)'f')
//#define ILOCALE_FORMATCODE_NUMBER_GENERAL              ((AECHAR)'g')
#define ILOCALE_FORMATCODE_NUMBER_NUMBER                 ((AECHAR)'n')
#define ILOCALE_FORMATCODE_NUMBER_PERCENT                ((AECHAR)'p')
//#define ILOCALE_FORMATCODE_NUMBER_ROUNDTRIP            ((AECHAR)'r')
#define ILOCALE_FORMATCODE_NUMBER_HEXADECIMAL            ((AECHAR)'x')

// date/time formats, case sensitive
#define ILOCALE_FORMATCODE_DATETIME_SHORTDATE            ((AECHAR)'d')
#define ILOCALE_FORMATCODE_DATETIME_LONGDATE             ((AECHAR)'D')
#define ILOCALE_FORMATCODE_DATETIME_SHORTTIME            ((AECHAR)'t')
#define ILOCALE_FORMATCODE_DATETIME_LONGTIME             ((AECHAR)'T')
#define ILOCALE_FORMATCODE_DATETIME_FULLDATESHORTTIME    ((AECHAR)'f')
#define ILOCALE_FORMATCODE_DATETIME_FULLDATELONGTIME     ((AECHAR)'F')
#define ILOCALE_FORMATCODE_DATETIME_GENDATESHORTTIME     ((AECHAR)'g')
#define ILOCALE_FORMATCODE_DATETIME_GENDATELONGTIME      ((AECHAR)'G')
#define ILOCALE_FORMATCODE_DATETIME_MONTHDAY             ((AECHAR)'m')
#define ILOCALE_FORMATCODE_DATETIME_MONTHDAYALT          ((AECHAR)'M')
#define ILOCALE_FORMATCODE_DATETIME_RFC1123              ((AECHAR)'r')
#define ILOCALE_FORMATCODE_DATETIME_RFC1121ALT           ((AECHAR)'R')
#define ILOCALE_FORMATCODE_DATETIME_SORTABLE             ((AECHAR)'s')
//#define ILOCALE_FORMATCODE_DATETIME_UNIVERSALSORTABLE  ((AECHAR)'u')
#define ILOCALE_FORMATCODE_DATETIME_YEARMONTH            ((AECHAR)'y')
#define ILOCALE_FORMATCODE_DATETIME_YEARMONTHALT         ((AECHAR)'Y')
               

// ILocale parameters
#define ILOCALEPARM_CURRENCY_DECIMALDIGITS         0
#define ILOCALEPARM_CURRENCY_DECIMALSEPARATOR      1
#define ILOCALEPARM_CURRENCY_GROUPSEPARATOR        2
#define ILOCALEPARM_CURRENCY_GROUPSIZES            3
#define ILOCALEPARM_CURRENCY_NEGATIVEPATTERN       4
#define ILOCALEPARM_CURRENCY_POSITIVEPATTERN       5
#define ILOCALEPARM_CURRENCY_SYMBOL                6

#define ILOCALEPARM_NUMBER_DECIMALDIGITS           7
#define ILOCALEPARM_NUMBER_DECIMALSEPARATOR        8
#define ILOCALEPARM_NUMBER_GROUPSEPARATOR          9
#define ILOCALEPARM_NUMBER_GROUPSIZES              10
#define ILOCALEPARM_NUMBER_NEGATIVEPATTERN         11

#define ILOCALEPARM_PERCENT_DECIMALDIGITS          12
#define ILOCALEPARM_PERCENT_DECIMALSEPARATOR       13
#define ILOCALEPARM_PERCENT_GROUPSEPARATOR         14
#define ILOCALEPARM_PERCENT_GROUPSIZES             15
#define ILOCALEPARM_PERCENT_NEGATIVEPATTERN        16
#define ILOCALEPARM_PERCENT_POSITIVEPATTERN        17
#define ILOCALEPARM_PERCENT_SYMBOL                 18

#define ILOCALEPARM_PERMILLE_SYMBOL                19

#define ILOCALEPARM_NANSYMBOL                      20
#define ILOCALEPARM_NEGATIVEINIFINITYSYMBOL        21
#define ILOCALEPARM_NEGATIVESIGN                   22
#define ILOCALEPARM_POSITIVEINIFINITYSYMBOL        23
#define ILOCALEPARM_POSITIVESIGN                   24

#define ILOCALEPARM_DATETIME_SHORTDAYNAMES         25
#define ILOCALEPARM_DATETIME_SHORTMONTHNAMES       26
#define ILOCALEPARM_DATETIME_DAYNAMES              27
#define ILOCALEPARM_DATETIME_MONTHNAMES            28

#define ILOCALEPARM_DATETIME_AMDESIGNATOR          29
#define ILOCALEPARM_DATETIME_DATESEPARATOR         30
#define ILOCALEPARM_DATETIME_PMDESIGNATOR          31
#define ILOCALEPARM_DATETIME_TIMESEPARATOR         32
#define ILOCALEPARM_DATETIME_FIRSTWEEKDAY          33

#define ILOCALEPARM_DATETIME_FULLDATELTIMEPATTERN  34   
#define ILOCALEPARM_DATETIME_FULLDATESTIMEPATTERN  35
#define ILOCALEPARM_DATETIME_GENDATELONGTIME       36
#define ILOCALEPARM_DATETIME_GENDATESHORTTIME      37
#define ILOCALEPARM_DATETIME_LONGDATEPATTERN       38
#define ILOCALEPARM_DATETIME_LONGTIMEPATTERN       39

#define ILOCALEPARM_DATETIME_MONTHDAYPATTERN       40
#define ILOCALEPARM_DATETIME_SHORTDATEPATTERN      41
#define ILOCALEPARM_DATETIME_SHORTTIMEPATTERN      42
#define ILOCALEPARM_DATETIME_YEARMONTHPATTERN      43

#define ILOCALEPARM_DATETIME_RFC1123PATTERN        44
#define ILOCALEPARM_DATETIME_SORTABLEDATETIME      45

#define ILOCALEPARM_STRING_HARDBREAK               46
#define ILOCALEPARM_STRING_SOFTBREAK               47

//
// ILocale Interface
//

#define INHERIT_ILocale(iname)\
   INHERIT_IQueryInterface (iname);\
   int      (*SetParm)     (iname *po, int16 nParmID, int32 p1);\
   int      (*GetParm)     (iname *po, int16 nParmID, int32 *pp1);\
   int32    (*FormatItem)  (iname *po, AECHAR *bufOut, uint32 dwBufSize, AECHAR **ppszFormatPattern, const AECHAR *pszFormatString, int dwFormatStringLen, AEELocaleArg *pItem); \
   int      (*wstrncmp)    (iname *po, AECHAR *s1, AECHAR *s2, unsigned int length); \
   int      (*wstrcmp)     (iname *po, AECHAR *s1, AECHAR *s2); \
   void     (*wstrupper)   (iname *po, AECHAR *pszDest); \
   void     (*wstrlower)   (iname *po, AECHAR *pszDest);\
   AECHAR*  (*breakline)   (iname *po, AECHAR *pszText, AECHAR *pszClip, AECHAR *pszEnd)

AEEINTERFACE_DEFINE(ILocale);

// standard members
#define ILOCALE_AddRef(p)                          AEEGETPVTBL((p),ILocale)->AddRef((p))
#define ILOCALE_Release(p)                         AEEGETPVTBL((p),ILocale)->Release((p))
#define ILOCALE_QueryInterface(p,i,o)              AEEGETPVTBL((p),ILocale)->QueryInterface((p),(i),(o))

// language dependent string formatting
#define ILOCALE_SetParm(p,i,v)                     AEEGETPVTBL((p),ILocale)->SetParm((p),(i),(v))
#define ILOCALE_GetParm(p,i,v)                     AEEGETPVTBL((p),ILocale)->GetParm((p),(i),(int32 *)(v))
#define ILOCALE_FormatItem(p,b,l,t,f,s,i)          AEEGETPVTBL((p),ILocale)->FormatItem((p),(b),(l),(t),(f),(s),(i))

// language dependent string utilities
#define ILOCALE_WSTRNCMP(p,s1,s2,l)                AEEGETPVTBL((p),ILocale)->wstrncmp((p),(s1),(s2),(l))
#define ILOCALE_WSTRCMP(p,s1,s2)                   AEEGETPVTBL((p),ILocale)->wstrcmp((p),(s1),(s2))
#define ILOCALE_WSTRUPPER(p,d)                     AEEGETPVTBL((p),ILocale)->wstrupper((p),(d))
#define ILOCALE_WSTRLOWER(p,d)                     AEEGETPVTBL((p),ILocale)->wstrlower((p),(d))
#define ILOCALE_BreakLine(p,t,c,e)                 AEEGETPVTBL((p),ILocale)->breakline((p),(t),(c),(e))


//
// Helper Macros
//

// Numeric Values

// Setting currency formatting options ...
#define ILOCALE_SetCurrencyDecimalDigits(p,d)      ILOCALE_SetParm(p, ILOCALEPARM_CURRENCY_DECIMALDIGITS,      (int32)d)
#define ILOCALE_SetCurrencyDecimalSeparator(p,s)   ILOCALE_SetParm(p, ILOCALEPARM_CURRENCY_DECIMALSEPARATOR,   (int32)s)
#define ILOCALE_SetCurrencyGroupSeparator(p,s)     ILOCALE_SetParm(p, ILOCALEPARM_CURRENCY_GROUPSEPARATOR,     (int32)s)
#define ILOCALE_SetCurrencyGroupSizes(p,d)         ILOCALE_SetParm(p, ILOCALEPARM_CURRENCY_GROUPSIZES,         (int32)d)
#define ILOCALE_SetCurrencyNegativePattern(p,s)    ILOCALE_SetParm(p, ILOCALEPARM_CURRENCY_NEGATIVEPATTERN,    (int32)s)
#define ILOCALE_SetCurrencyPositivePattern(p,s)    ILOCALE_SetParm(p, ILOCALEPARM_CURRENCY_POSITIVEPATTERN,    (int32)s)
#define ILOCALE_SetCurrencySymbol(p,s)             ILOCALE_SetParm(p, ILOCALEPARM_CURRENCY_SYMBOL,             (int32)s)

// Setting number formatting options ...
#define ILOCALE_SetNumberDecimalDigits(p,d)        ILOCALE_SetParm(p, ILOCALEPARM_NUMBER_DECIMALDIGITS,        (int32)d)
#define ILOCALE_SetNumberDecimalSeparator(p,s)     ILOCALE_SetParm(p, ILOCALEPARM_NUMBER_DECIMALSEPARATOR,     (int32)s)
#define ILOCALE_SetNumberGroupSeparator(p,s)       ILOCALE_SetParm(p, ILOCALEPARM_NUMBER_GROUPSEPARATOR,       (int32)s)
#define ILOCALE_SetNumberGroupSizes(p,d)           ILOCALE_SetParm(p, ILOCALEPARM_NUMBER_GROUPSIZES,           (int32)d)
#define ILOCALE_SetNumberNegativePattern(p,s)      ILOCALE_SetParm(p, ILOCALEPARM_NUMBER_NEGATIVEPATTERN,      (int32)s)

// Setting "percent" formatting options
#define ILOCALE_SetPercentDecimalDigits(p,d)       ILOCALE_SetParm(p, ILOCALEPARM_PERCENT_DECIMALDIGITS,       (int32)d)
#define ILOCALE_SetPercentDecimalSeparator(p,s)    ILOCALE_SetParm(p, ILOCALEPARM_PERCENT_DECIMALSEPARATOR,    (int32)s)
#define ILOCALE_SetPercentGroupSeparator(p,s)      ILOCALE_SetParm(p, ILOCALEPARM_PERCENT_GROUPSEPARATOR,      (int32)s)
#define ILOCALE_SetPercentGroupSizes(p,d)          ILOCALE_SetParm(p, ILOCALEPARM_PERCENT_GROUPSIZES,          (int32)d)
#define ILOCALE_SetPercentNegativePattern(p,s)     ILOCALE_SetParm(p, ILOCALEPARM_PERCENT_NEGATIVEPATTERN,     (int32)s)
#define ILOCALE_SetPercentPositivePattern(p,s)     ILOCALE_SetParm(p, ILOCALEPARM_PERCENT_POSITIVEPATTERN,     (int32)s)
#define ILOCALE_SetPercentSymbol(p,s)              ILOCALE_SetParm(p, ILOCALEPARM_PERCENT_SYMBOL,              (int32)s)
#define ILOCALE_SetPermilleSymbol(p,s)             ILOCALE_SetParm(p, ILOCALEPARM_PERMILLE_SYMBOL,             (int32)s)

// Setting general numeric formatting options
#define ILOCALE_SetNaNSymbol(p,s)                  ILOCALE_SetParm(p, ILOCALEPARM_NANSYMBOL,                   (int32)s)
#define ILOCALE_SetNegativeInfinitySymbol(p,s)     ILOCALE_SetParm(p, ILOCALEPARM_NEGATIVEINIFINITYSYMBOL,     (int32)s)
#define ILOCALE_SetNegativeSign(p,s)               ILOCALE_SetParm(p, ILOCALEPARM_NEGATIVESIGN,                (int32)s)
#define ILOCALE_SetPositiveInfinitySymbol(p,s)     ILOCALE_SetParm(p, ILOCALEPARM_POSITIVEINIFINITYSYMBOL,     (int32)s)
#define ILOCALE_SetPositiveSign(p,s)               ILOCALE_SetParm(p, ILOCALEPARM_POSITIVESIGN,                (int32)s)


// DateTime Values

// Setting month and day names
#define ILOCALE_SetAbbreviatedDayNames(p,s)        ILOCALE_SetParm(p, ILOCALEPARM_DATETIME_SHORTDAYNAMES,      (int32)s)
#define ILOCALE_SetAbbreviatedMonthNames(p,s)      ILOCALE_SetParm(p, ILOCALEPARM_DATETIME_SHORTMONTHNAMES,    (int32)s)
#define ILOCALE_SetDayNames(p,s)                   ILOCALE_SetParm(p, ILOCALEPARM_DATETIME_DAYNAMES,           (int32)s)
#define ILOCALE_SetMonthNames(p,s)                 ILOCALE_SetParm(p, ILOCALEPARM_DATETIME_MONTHNAMES,         (int32)s)

// Setting separators, etc.
#define ILOCALE_SetAMDesignator(p,s)               ILOCALE_SetParm(p, ILOCALEPARM_DATETIME_AMDESIGNATOR,       (int32)s)
#define ILOCALE_SetDateSeparator(p,s)              ILOCALE_SetParm(p, ILOCALEPARM_DATETIME_DATESEPARATOR,      (int32)s)
#define ILOCALE_SetPMDesignator(p,s)               ILOCALE_SetParm(p, ILOCALEPARM_DATETIME_PMDESIGNATOR,       (int32)s)
#define ILOCALE_SetTimeSeparator(p,s)              ILOCALE_SetParm(p, ILOCALEPARM_DATETIME_TIMESEPARATOR,      (int32)s)
#define ILOCALE_SetFirstWeekDay(p,d)               ILOCALE_SetParm(p, ILOCALEPARM_DATETIME_FIRSTWEEKDAY,       (int32)d)

// Setting date/time patterns
#define ILOCALE_SetFullDateTimePattern(p,s)        ILOCALE_SetParm(p, ILOCALEPARM_DATETIME_FULLDATELTIMEPATTERN,(int32)s)
#define ILOCALE_SetLongDatePattern(p,s)            ILOCALE_SetParm(p, ILOCALEPARM_DATETIME_LONGDATEPATTERN,    (int32)s)
#define ILOCALE_SetLongTimePattern(p,s)            ILOCALE_SetParm(p, ILOCALEPARM_DATETIME_LONGTIMEPATTERN,    (int32)s)
#define ILOCALE_SetMonthDayPattern(p,s)            ILOCALE_SetParm(p, ILOCALEPARM_DATETIME_MONTHDAYPATTERN,    (int32)s)
#define ILOCALE_SetShortDatePattern(p,s)           ILOCALE_SetParm(p, ILOCALEPARM_DATETIME_SHORTDATEPATTERN,   (int32)s)
#define ILOCALE_SetShortTimePattern(p,s)           ILOCALE_SetParm(p, ILOCALEPARM_DATETIME_SHORTTIMEPATTERN,   (int32)s)
#define ILOCALE_SetYearMonthPattern(p,s)           ILOCALE_SetParm(p, ILOCALEPARM_DATETIME_YEARMONTHPATTERN,   (int32)s)

// Setting general string specials
#define ILOCALE_SetHardBreak(p,s)                  ILOCALE_SetParm(p, ILOCALEPARM_STRING_HARDBREAK,            (int32)s)
#define ILOCALE_SetSoftBreak(p,s)                  ILOCALE_SetParm(p, ILOCALEPARM_STRING_SOFTBREAK,            (int32)s)


// Numeric Values

// Getting currency formatting options ...
#define ILOCALE_GetCurrencyDecimalDigits(p,d)      ILOCALE_GetParm(p, ILOCALEPARM_CURRENCY_DECIMALDIGITS,      (int32*)d)
#define ILOCALE_GetCurrencyDecimalSeparator(p,s)   ILOCALE_GetParm(p, ILOCALEPARM_CURRENCY_DECIMALSEPARATOR,   (int32*)s)
#define ILOCALE_GetCurrencyGroupSeparator(p,s)     ILOCALE_GetParm(p, ILOCALEPARM_CURRENCY_GROUPSEPARATOR,     (int32*)s)
#define ILOCALE_GetCurrencyGroupSizes(p,d)         ILOCALE_GetParm(p, ILOCALEPARM_CURRENCY_GROUPSIZES,         (int32*)d)
#define ILOCALE_GetCurrencyNegativePattern(p,s)    ILOCALE_GetParm(p, ILOCALEPARM_CURRENCY_NEGATIVEPATTERN,    (int32*)s)
#define ILOCALE_GetCurrencyPositivePattern(p,s)    ILOCALE_GetParm(p, ILOCALEPARM_CURRENCY_POSITIVEPATTERN,    (int32*)s)
#define ILOCALE_GetCurrencySymbol(p,s)             ILOCALE_GetParm(p, ILOCALEPARM_CURRENCY_SYMBOL,             (int32*)s)

// Getting number formatting options ...
#define ILOCALE_GetNumberDecimalDigits(p,d)        ILOCALE_GetParm(p, ILOCALEPARM_NUMBER_DECIMALDIGITS,        (int32*)d)
#define ILOCALE_GetNumberDecimalSeparator(p,s)     ILOCALE_GetParm(p, ILOCALEPARM_NUMBER_DECIMALSEPARATOR,     (int32*)s)
#define ILOCALE_GetNumberGroupSeparator(p,s)       ILOCALE_GetParm(p, ILOCALEPARM_NUMBER_GROUPSEPARATOR,       (int32*)s)
#define ILOCALE_GetNumberGroupSizes(p,d)           ILOCALE_GetParm(p, ILOCALEPARM_NUMBER_GROUPSIZES,           (int32*)d)
#define ILOCALE_GetNumberNegativePattern(p,s)      ILOCALE_GetParm(p, ILOCALEPARM_NUMBER_NEGATIVEPATTERN,      (int32*)s)

// Getting "percent" formatting options
#define ILOCALE_GetPercentDecimalDigits(p,d)       ILOCALE_GetParm(p, ILOCALEPARM_PERCENT_DECIMALDIGITS,       (int32*)d)
#define ILOCALE_GetPercentDecimalSeparator(p,s)    ILOCALE_GetParm(p, ILOCALEPARM_PERCENT_DECIMALSEPARATOR,    (int32*)s)
#define ILOCALE_GetPercentGroupSeparator(p,s)      ILOCALE_GetParm(p, ILOCALEPARM_PERCENT_GROUPSEPARATOR,      (int32*)s)
#define ILOCALE_GetPercentGroupSizes(p,d)          ILOCALE_GetParm(p, ILOCALEPARM_PERCENT_GROUPSIZES,          (int32*)d)
#define ILOCALE_GetPercentNegativePattern(p,s)     ILOCALE_GetParm(p, ILOCALEPARM_PERCENT_NEGATIVEPATTERN,     (int32*)s)
#define ILOCALE_GetPercentPositivePattern(p,s)     ILOCALE_GetParm(p, ILOCALEPARM_PERCENT_POSITIVEPATTERN,     (int32*)s)
#define ILOCALE_GetPercentSymbol(p,s)              ILOCALE_GetParm(p, ILOCALEPARM_PERCENT_SYMBOL,              (int32*)s)
#define ILOCALE_GetPermilleSymbol(p,s)             ILOCALE_GetParm(p, ILOCALEPARM_PERMILLE_SYMBOL,             (int32*)s)

// Getting general numeric formatting options
#define ILOCALE_GetNaNSymbol(p,s)                  ILOCALE_GetParm(p, ILOCALEPARM_NANSYMBOL,                   (int32*)s)
#define ILOCALE_GetNegativeInfinitySymbol(p,s)     ILOCALE_GetParm(p, ILOCALEPARM_NEGATIVEINIFINITYSYMBOL,     (int32*)s)
#define ILOCALE_GetNegativeSign(p,s)               ILOCALE_GetParm(p, ILOCALEPARM_NEGATIVESIGN,                (int32*)s)
#define ILOCALE_GetPositiveInfinitySymbol(p,s)     ILOCALE_GetParm(p, ILOCALEPARM_POSITIVEINIFINITYSYMBOL,     (int32*)s)
#define ILOCALE_GetPositiveSign(p,s)               ILOCALE_GetParm(p, ILOCALEPARM_POSITIVESIGN,                (int32*)s)


// DateTime Values

// Getting month and day names
#define ILOCALE_GetAbbreviatedDayNames(p,s)        ILOCALE_GetParm(p, ILOCALEPARM_DATETIME_SHORTDAYNAMES,      (int32*)s)
#define ILOCALE_GetAbbreviatedMonthNames(p,s)      ILOCALE_GetParm(p, ILOCALEPARM_DATETIME_SHORTMONTHNAMES,    (int32*)s)
#define ILOCALE_GetDayNames(p,s)                   ILOCALE_GetParm(p, ILOCALEPARM_DATETIME_DAYNAMES,           (int32*)s)
#define ILOCALE_GetMonthNames(p,s)                 ILOCALE_GetParm(p, ILOCALEPARM_DATETIME_MONTHNAMES,         (int32*)s)

// Getting separators, etc.
#define ILOCALE_GetAMDesignator(p,s)               ILOCALE_GetParm(p, ILOCALEPARM_DATETIME_AMDESIGNATOR,       (int32*)s)
#define ILOCALE_GetDateSeparator(p,s)              ILOCALE_GetParm(p, ILOCALEPARM_DATETIME_DATESEPARATOR,      (int32*)s)
#define ILOCALE_GetPMDesignator(p,s)               ILOCALE_GetParm(p, ILOCALEPARM_DATETIME_PMDESIGNATOR,       (int32*)s)
#define ILOCALE_GetTimeSeparator(p,s)              ILOCALE_GetParm(p, ILOCALEPARM_DATETIME_TIMESEPARATOR,      (int32*)s)
#define ILOCALE_GetFirstWeekDay(p,d)               ILOCALE_GetParm(p, ILOCALEPARM_DATETIME_FIRSTWEEKDAY,       (int32*)d)

// Getting date/time patterns
#define ILOCALE_GetFullDateTimePattern(p,s)        ILOCALE_GetParm(p, ILOCALEPARM_DATETIME_FULLDATELTIMEPATTERN,(int32*)s)
#define ILOCALE_GetLongDatePattern(p,s)            ILOCALE_GetParm(p, ILOCALEPARM_DATETIME_LONGDATEPATTERN,    (int32*)s)
#define ILOCALE_GetLongTimePattern(p,s)            ILOCALE_GetParm(p, ILOCALEPARM_DATETIME_LONGTIMEPATTERN,    (int32*)s)
#define ILOCALE_GetMonthDayPattern(p,s)            ILOCALE_GetParm(p, ILOCALEPARM_DATETIME_MONTHDAYPATTERN,    (int32*)s)
#define ILOCALE_GetRFC1123Pattern(p,s)             ILOCALE_GetParm(p, ILOCALEPARM_DATETIME_RFC1123PATTERN,     (int32*)s)
#define ILOCALE_GetShortDatePattern(p,s)           ILOCALE_GetParm(p, ILOCALEPARM_DATETIME_SHORTDATEPATTERN,   (int32*)s)
#define ILOCALE_GetShortTimePattern(p,s)           ILOCALE_GetParm(p, ILOCALEPARM_DATETIME_SHORTTIMEPATTERN,   (int32*)s)
#define ILOCALE_GetSortableDateTimePattern(p,s)    ILOCALE_GetParm(p, ILOCALEPARM_DATETIME_SORTABLEDATETIME,   (int32*)s)
#define ILOCALE_GetYearMonthPattern(p,s)           ILOCALE_GetParm(p, ILOCALEPARM_DATETIME_YEARMONTHPATTERN,   (int32*)s)

// Getting general string specials
#define ILOCALE_GetHardBreak(p,s)                  ILOCALE_GetParm(p, ILOCALEPARM_STRING_HARDBREAK,            (int32*)s)
#define ILOCALE_GetSoftBreak(p,s)                  ILOCALE_GetParm(p, ILOCALEPARM_STRING_SOFTBREAK,            (int32*)s)

/*=================================================================================
   DATA STRUCTURE DOCUMENTATION
===================================================================================
Locale Class ID

Description:
   An application may create an ILocale object by passing the ID of the class to 
   ISHELL_CreateInstance().  An application may ask for a specific language/locale,
   or the current ILocale used by the system. 
   
   The class ID used to identify the ILocale object is defined below.

Definition:

Identifier              Definition
----------              -----------------------------------
AEECLSID_CURRENTLOCALE  Identifies the ID of the current system locale class.
AEECLSID_LOCALEENUS     Identifies the ID of the US/english locale class.
   
Comments:
   None

See Also:
   ISHELL_CreateInstance()

======================================================================= 

AEELocaleArg

Description: 
   This structure represents the value that is passed to the ILocale object that is
   used to either locate and return the proper string formatting pattern to the 
   caller, or generate the formatted string if the ILocale implementation wishes to 
   do so itself.

Definition:
   typedef struct
   {
      char cType;
      union
      {
         int32 d;       // signed decimal
         uint32 u;      // unsigned decimal
         char c;        // character
         AECHAR C;      // AECHAR
         char *s;       // char *
         AECHAR *S;     // AECHAR *
         void *p;       // pointer
         JulianType j;  // julian date
      } u;
   } AEELocaleArg;

Members:
   cType:      The type of argument this structure contains
   u:          Union of all supported argument types.

Comments:
   None

See Also:
   AEELocaleArg Types
   ILOCALE_FormatItem()

======================================================================= 

AEELocaleArg Types

Description:
   These indicate the type of the argument stored in an AEELocaleArg.
   The cType field of an AEELocaleArg structure will contain one of
   the following and indicates what member of u is valid.

AEELOCALEARGTYPE_SIGNEDDECIMAL
   This AEELocaleArg contains a signed integer in the d member of u.

AEELOCALEARGTYPE_UNSIGNEDDECIMAL
   This AEELocaleArg contains an unsigned integer in the u member of u.

AEELOCALEARGTYPE_CHAR
   This AEELocaleArg contains a char in the c member of u.

AEELOCALEARGTYPE_AECHAR
   This AEELocaleArg contains an AECHAR in the C member of u.

AEELOCALEARGTYPE_CHARPTR
   This AEELocaleArg contains a char * in the s member of u.

AEELOCALEARGTYPE_AECHARPTR
   This AEELocaleArg contains an AECHAR * in the S member of u.

AEELOCALEARGTYPE_POINTER
   This AEELocaleArg contains a pointer value in the p member of u.

AEELOCALEARGTYPE_JULIANDATE
   This AEELocaleArg contains a julian date type in the j member of u.

======================================================================= 

ILocale Parameters

Description:
   These parameters allow the caller to get or set particular formatting 
   behavior for the ILocale object. They are used in the ILOCALE_SetParm() 
   and ILOCALE_GetParm() APIs.

   The caller may get patterns or symbols from the ILocale object using
   these parameters to format text or determine language-specific
   indicators.  Similarly, the call may set these parameters to change
   how particular strings are formatted.

ILOCALEPARM_CURRENCY_DECIMALDIGITS
   The number of decimial digits to be used when formatting currency 
   values

ILOCALEPARM_CURRENCY_DECIMALSEPARATOR   
   The string to use to separate decimal digits when formatting currency 
   values

ILOCALEPARM_CURRENCY_GROUPSEPARATOR
   The string to use when separating groups (eg, thousands) when 
   formatting currency values

ILOCALEPARM_CURRENCY_GROUPSIZES 
   The number of characters in a group (eg, 3 for thousands) when 
   formatting currency values

ILOCALEPARM_CURRENCY_NEGATIVEPATTERN
   A string defining the pattern to use when formatting negative 
   currency values.  The pattern is constructed using custom numeric 
   format strings such as "($#)".  Note the symbols in the pattern should 
   be replaced with the separators, etc. specified in the ILocale object.

ILOCALEPARM_CURRENCY_POSITIVEPATTERN  
   A string defining the pattern to use when formatting positive currency
   values.  The pattern is constructed using custom numeric format strings
   such as "$#,##0.00".  Note the symbols in the pattern will be replaced
   with the separators, etc. specified in the ILocale object.
   
ILOCALEPARM_CURRENCY_SYMBOL             
   The string to use as the currency symbol when formatting currency 
   values

ILOCALEPARM_NUMBER_DECIMALDIGITS
   The number of decimal digits to be used when formatting numeric values
   
ILOCALEPARM_NUMBER_DECIMALSEPARATOR   
   The string to use to separate decimal digits when formatting numeric 
   values
   
ILOCALEPARM_NUMBER_GROUPSEPARATOR  
   The string to use to separate groups (eg, thousands) when formatting
   numeric values

ILOCALEPARM_NUMBER_GROUPSIZES  
   The number of characters in a group (eg, 3 for thousands) when 
   formatting numeric values
   
ILOCALEPARM_NUMBER_NEGATIVEPATTERN      
   A string defining the pattern to use when formatting negative numeric
   values.  The pattern is constructed using custom numeric format strings
   such as "-#".  Note the symbols in the pattern will be replaced
   with the separators, etc. specified in the ILocale object.

ILOCALEPARM_PERCENT_DECIMALDIGITS
   The number of decimal digits to be used when formatting percent values
       
ILOCALEPARM_PERCENT_DECIMALSEPARATOR 
   The string to use to separate decimal digits when formatting percent 
   values
   
ILOCALEPARM_PERCENT_GROUPSEPARATOR 
   The string to use to separate groups (eg, thousands) when formatting
   percent values

ILOCALEPARM_PERCENT_GROUPSIZES
   The number of characters in a group (eg, 3 for thousands) when 
   formatting percent values
   
ILOCALEPARM_PERCENT_NEGATIVEPATTERN
   A string defining the pattern to use when formatting negative percent
   values.  The pattern is constructed using custom numeric format strings
   such as "-#%".  Note the symbols in the pattern should be replaced
   with the separators, etc. specified in the ILocale object.

ILOCALEPARM_PERCENT_POSITIVEPATTERN 
   A string defining the pattern to use when formatting positive percent
   values.  The pattern is constructed using custom numeric format strings
   such as "(#%)".  Note the symbols in the pattern should be replaced
   with the separators, etc. specified in the ILocale object.
   
ILOCALEPARM_PERCENT_SYMBOL              
   The string to use as the percent symbol when formatting percent values

ILOCALEPARM_PERMILLE_SYMBOL             
   The string to use as the permille symbol when formatting percent values

ILOCALEPARM_NANSYMBOL      
   The string to use as the Not A Number symbol.
   
ILOCALEPARM_NEGATIVEINIFINITYSYMBOL  
   The string to use as the negative infinity symbol.
   
ILOCALEPARM_NEGATIVESIGN  
   The string to use as the negative symbol.
   
ILOCALEPARM_POSITIVEINIFINITYSYMBOL  
   The string to use as the positive infinity symbol.
   
ILOCALEPARM_POSITIVESIGN   
   The string to use as the positive symbol.             

ILOCALEPARM_DATETIME_SHORTDAYNAMES 
   An array containing the culture-specific abbreviated day names
   
ILOCALEPARM_DATETIME_SHORTMONTHNAMES    
   An array containing the culture-specific abbreviated month names

ILOCALEPARM_DATETIME_DAYNAMES   
   An array containing the full culture-specific day names
   
ILOCALEPARM_DATETIME_MONTHNAMES         
   An array containing the full culture-specific month names

ILOCALEPARM_DATETIME_AMDESIGNATOR  
   The string to use to designate hours that are before noon
   
ILOCALEPARM_DATETIME_DATESEPARATOR 
   The string to use to separate the day, month and year components
   of a date

ILOCALEPARM_DATETIME_PMDESIGNATOR  
   The string to use to desingiate hours that are after noon
   
ILOCALEPARM_DATETIME_TIMESEPARATOR  
   The string to use to separate the hour, minute and second
   component of a time string

ILOCALEPARM_DATETIME_FIRSTWEEKDAY   
   A value between 0 (Sunday) and 6 (Saturday) indicating which
   day is the first day of the week

ILOCALEPARM_DATETIME_FULLDATELTIMEPATTERN
   A string defining the pattern to use when formatting long date,
   long time strings.  This is associated with the "F" format 
   character.

ILOCALEPARM_DATETIME_FULLDATESTIMEPATTERN
   A string defining the pattern to use when formatting long date,
   short time strings.  This is associated with the "f" format 
   character.

ILOCALEPARM_DATETIME_GENDATELONGTIME
   A string defining the pattern to use when formatting short
   date, long time strings.  This is associated with the "G" 
   format character.

ILOCALEPARM_DATETIME_GENDATESHORTTIME
   A string defining the pattern to use when formatting short
   date, short time strings.  This is associated with the "g" 
   format characte

ILOCALEPARM_DATETIME_LONGDATEPATTERN
   A string defining the pattern to use when formatting long date
   strings.  This is associated with the "D" format character.
   
ILOCALEPARM_DATETIME_LONGTIMEPATTERN
   A string defining the pattern to use when formatting long time
   strings.  This is associated with the "T" format character.

ILOCALEPARM_DATETIME_MONTHDAYPATTERN
   A string defining the pattern to use when formatting a month/day
   string.  This is associated with the "m" and "M" format characters.

ILOCALEPARM_DATETIME_RFC1123PATTERN
   Gets the format pattern for a time value formatted according to
   RFC1123.  This item is readonly and can not be set.  This is
   associated with the 'r' and 'R' format characters.

ILOCALEPARM_DATETIME_SHORTDATEPATTERN
   A string defining the pattern to use when formatting short date
   strings.  This is associated with the "d" format character.

ILOCALEPARM_DATETIME_SHORTTIMEPATTERN 
   A string defining the pattern to use when formatting short time
   strings.  This is associated with the "t" format character.
   
ILOCALEPARM_DATETIME_SORTABLEDATETIME 
   Gets the format pattern for a sortable date and time value. This 
   item is readonly and can not be set.  This is associated with the 
   's' and 'S' format characters.
   
ILOCALEPARM_DATETIME_YEARMONTHPATTERN   
   A string defining the pattern to use when formatting a year/month
   string.  This is associated with the "y" and "Y" format characters.

ILOCALEPARM_STRING_HARDBREAK 
   A character (or string) indicating a hard break.

ILOCALEPARM_STRING_SOFTBREAK 
   The set of characters representing white space.

===================================================================================
   INTERFACE DOCUMENTATION
===================================================================================

ILocale Interface

Description: 
  The ILocale interface provides language-specific string formatting patterns, 
  formatting logic and comparison logic for a single language.  It is intended
  that all string formatting and comparison logic for a language be stored within
  a single ILocale.
  
  Each ILocale implementation registers itself as a mimetype handler in
  its .mif file.  ILocale objects handle 'text/plain;xxxx", where "xxxx" is
  a four character language code.  This code consists of the ISO 639-1 language 
  code (2 characters) followed by the ISO 3166 A2 country code (2 characters).  
  If no country code is needed to specify the language (Korean, for example), 
  two periods are used in place of the country code. 'enus', 'engb' and 'ko..' 
  are valid examples of language codes.

  The ILocale object provides methods to get and set certain formatting
  parameters.  These are used get and set certain language-specific patterns or
  symbols and are described in the ILocale Parameter documentaiton.

  ILocale provides a ILOCALE_FormatItem() method that takes a format code
  and an argument and returns the formatted string and/or the language-
  specific pattern to be used to format the item.  See the documentation
  for this routine for the argument types and patterns that are supported.

Header File:
   AEELocale.h
   
==============================================================================
Function: ILOCALE_AddRef()

This function is inherited from IQI_AddRef().

==============================================================================
Function: ILOCALE_Release()

This function is inherited from IQI_Release().

==============================================================================
Function: ILOCALE_QueryInterface()

This function is inherited from IQI_QueryInterface().

==============================================================================
Function: ILOCALE_SetParm()

Description:
   This function sets certain ILocale parameters.  The parameters that may
   be set are described in the ILocale Parameters documentation.

   The caller may use this routine to change the way a specific ILocale
   object formats strings.  By doing so, other APIs that use the ILocale to
   format strings, etc, can be adjusted to produce different output.

Prototype:
   int ILOCALE_SetParm(
      ILocale *po,
      int16 nParmID,
      int32 p1);

Parameters:
   po:        Pointer to the ILocale Interface.
   nParmID:   Parameter Id
   p1:        Depends on the nParmID parameter

Return Value:
   SUCCESS: Successful. Operation is completed.
   EBADPARM: Bad parm
   EFAILED: General failure
   ENOMEMORY: Out of memory
   
Comments:
   See ILocale Parameters Description for parameter details.

See Also:
   ILocale Parameters
   ILOCALE_GetParm()

=============================================================================

Function: ILOCALE_GetParm()

Description:
   This function gets ILocale parameters.  The parameters that may be fetched 
   are described in the ILocale Parameters documentation.

   The caller may use this routine to determine language-specific formatting
   rules set in an ILocale object.  These, in turn, may be used to determine
   exactly how to format numbers, currency, dates and the like.

Prototype:
   int ILOCALE_GetParm(
      ILocale *po,
      int16 nParmID,
      int32 *pp1);

Parameters:
   po:        Pointer to the ILocale Interface.
   nParmID:   Parameter Id
   pp1:        Depends on the nParmID parameter

Return Value:
   SUCCESS: Successful. Operation is completed.
   EBADPARM: Bad parm
   EFAILED: General failure

Comments:
   See ILocale Parameters Description for parameter details.
 
See Also:
   ILocale Parameters
   ILOCALE_SetParm()

=============================================================================

Function: ILOCALE_FormatItem()

Description:
   This routine takes a buffer, a format code and an item to format and
   attempts to generate a language-specific formated string according
   to the argument type and format code.  If the ILocale contains a
   specific pattern for the format code/argument type pair, it is returned
   in the ppszFormatPattern argument.

   This routine is typically called to provide an exact pattern according
   to which a particular value should be formatted for a specific language.
   If the pattern is too complex for the caller to process, 
   ILOCALE_FormatItem() may perform the actual formatting itself, place
   the results in the bufOut parameter, return the number of characters
   required for the formatted string and set the ppszFormatPattern 
   paramerter to NULL.

   Both the item to be formatted and the format code determine the
   pattern of the formatted string:


   ILocale Argument Types

   The ILOCALE_FormatItem() routine is passed an AEELocaleArg, a single
   structure that represents a number of different arguments.  The format
   code will be interpreted differently depending on the type of the argument 
   stored in the AEELocaleArg.   ILOCALE_FormatItem() understands the 
   following AEELocaleArg types:

      AEELOCALEARGTYPE_SIGNEDDECIMAL   - signed integer values
      AEELOCALEARGTYPE_UNSIGNEDDECIMAL - unsigned integer values
      AEELOCALEARGTYPE_CHAR            - single character
      AEELOCALEARGTYPE_AECHAR          - singled AECHAR
      AEELOCALEARGTYPE_CHARPTR         - a string of characters
      AEELOCALEARGTYPE_AECHARPTR       - a string of AECHARs
      AEELOCALEARGTYPE_POINTER         - a generic pointer value
      AEELOCALEARGTYPE_JULIANDATE      - a JulianType date structure
   
   If the AEELocaleArg is not one of these types, ILOCALE_FormatItem()
   will return -1.

   
   ILocale Format Codes

   ILocale implementations understand a number of different formatting
   codes.  Given a code and an argument type, the ILocale may choose to
   either generate a formatted string itself or just return the language
   specific pattern to the caller.  
   
   A format code typically consists of a single character followed by
   one or more digits indicating precision, such as "F" or "C2".  When
   formatting values, indicators such as the currency symbol, AM/PM
   designator or group sizes are determined from the ILocale's parameters.

   ILocale implementations support the following codes and may be extended 
   to support others:

   Unsigned Integers
   Signed Integers

   C or c      Currency    The number is converted to string that represents a 
                           currency amount according to 
                           ILOCALEPARM_CURRENCY_POSITIVEPATTERN.  If the number is 
                           a signed integer and is negative, thestring is formatted 
                           according to the ILOCALEPARM_CURRENCY_NEGATIVEPATTERN.  
                           The precisions specifier indicatesthe number of decimal 
                           places.  If no precision isspecified, the default currency 
                           precisions given by ILOCALEPARM_CURRENCY_DECIMALDIGITS is 
                           used.  

                           For example, for 'enus', the number 12345678 will be 
                           formatted as a currency as:

                           n0    $12,345,678
                           n     $123,456.78
                           
                           and the number -12345678 will be formatted as:

                           n0    ($12,345,678)
                           n     ($123,456.78)

                           Note that when no precision is specified, the default
                           precision (2 digits) set by ILOCALEPARM_CURRENCY_DECIMALDIGITS 
                           is used.

   D or d      Signed      The number is converted to a string of decimal digits,
               Decimal     prefixed by a minus sign if negative.  If present,
                           the precision specifiers indicates the minimum number
                           of digits in the resulting string.  If required, the
                           result is padded with '0'.

                           For example, the number -123 will be formatted as:

                           d     -123
                           d5    -00123

   E or e      Scientific  Scientific Notation is not supported at this time.
      
   F or f      Fixed       The numer is converted to a string in the form of      
               Point       "-ddd.ddd"  The string starts with a minus sign if
                           the argument is a signed integer and is negative.
                           The precision indicates the number of decimal places.

                           For example, the number 123456 will be formatted as:

                           f     123456 
                           f2    1234.56

   G or g      General     General formatting is not supported at this time.

   N or n      Number      The number is converted to a string of decimals.  If 
                           the number is a signed integer and is negative, the 
                           string is formatted according to the
                           ILOCALEPARM_NUMBER_NEGATIVEPATTERN.  The separators 
                           in the pattern should be replaced with their language-
                           dependent counterparts.  The precision specifier 
                           indicates the desired number of decimal places.

                           For example, for 'enus', the number -12345678 will be 
                           formatted as:

                           n     -12,345,678
                           n2    -123,456.78
                           n5    -123.45678

                           Note that when no precision is specified, the default
                           precision (0 digits) set by ILOCALEPARM_NUMBER_DECIMALDIGITS 
                           is used.

   P or p      Percent     The number is converted to a string that represents a
                           percentage according to ILOCALEPARM_PERCENT_POSITIVEPATTERN.  
                           If the number is a signed integer and is negative, the 
                           string is formatted according to the
                           ILOCALEPARM_PERCENT_NEGATIVEPATTERN.  The separators 
                           in  the pattern should be replaced with their language-
                           dependent counterparts.  The precision specifier 
                           indicates the desired number of decimal places.

                           For example, for 'enus', the number 123456 will be 
                           formatted  as:

                           n     123,456%
                           n3    123.456%
                           
                           and the number -123456 will be formatted as:

                           n     (123,456%)
                           n3    (123.456%)

                           Note that when no precision is specified, the default
                           precision (0 digits) set by ILOCALEPARM_PRECENT_DECIMALDIGITS 
                           is used.

   R or r      Rount-trip  Round Trip formatting is not supported at this time.

   X or x      Hexadecimal The number is converted to a string of hexadecimal digits.
                           The case of the format specifier indicates whether to use
                           upper or lower case in the result.  The precision specifier
                           indicates the minimum number of digits desired in the result.
                           If required, the result is padded with zeros to its left to
                           produce this number of digits.

   U or u      Unsigned    The number is treated as an uint32 and converted to a
               Decimal     string of decimal digits.  It is formatted in the same
                           was as a signed decimal.

       
   JulianType Date Structures


   d           Short date  Displays the date in a short date pattern according to the
               pattern     specifed language code, the language set by the
                           ITEXTFORMATTER_SetLang() function, or default language of 
                           the device.  The date is displayed according to the pattern 
                           specified in ILOCALEPARM_DATETIME_SHORTDATEPATTERN.

   D           Long date   Displays the date in a long date pattern according to the
               pattern     specifed language code, the language set by the
                           ITEXTFORMATTER_SetLang() function, or default language of 
                           the device.  The date is displayed according to the pattern 
                           specified in ILOCALEPARM_DATETIME_LONGDATEPATTERN.  

   t           Short time  Displays the time in a short time pattern according to the
               pattern     specifed language code, the language set by the
                           ITEXTFORMATTER_SetLang() function, or default language of 
                           the device.  The date is displayed according to the pattern 
                           specified in ILOCALEPARM_DATETIME_SHORTTIMEPATTERN. 
   
   T           Long time   Displays the time in a long time pattern according to the
               pattern     specifed language code, the language set by the
                           ITEXTFORMATTER_SetLang() function, or default language of 
                           the device. The date is displayed according to the pattern 
                           specified in ILOCALEPARM_DATETIME_LONGTIMEPATTERN.

   f           Full date/  Displays a combination of the long date and short time
               short time  patterns, separated by a space, according to the
                           specifed language code, the language set by the
                           ITEXTFORMATTER_SetLang() function, or default language of 
                           the device.  The date is displayed according to the pattern 
                           specified in ILOCALEPARM_DATETIME_FULLDATESTIMEPATTERN.

   F           Full date/  Displays a combination of the long date and long time
               long time   patterns, separated by a space, according to the
                           specifed language code, the language set by the
                           ITEXTFORMATTER_SetLang() function, or default language of 
                           the device.  The date is displayed according to the pattern 
                           specified in ILOCALEPARM_DATETIME_FULLDATELTIMEPATTERN.

   g           Gen date/   Displays a combination of the short date and short time
               short time  patterns, separated by a space, according to the specifed 
                           language code, the language set by the ITEXTFORMATTER_SetLang() 
                           function, or default language of the device.  The date is 
                           displayed according to the pattern specified in 
                           ILOCALEPARM_DATETIME_GENDATESHORTTIME. 

   G           Gen date/   Displays a combination of the short date and long time
               long time   patterns, separated by a space, according to the specifed 
                           language code, the language set by the ITEXTFORMATTER_SetLang() 
                           function, or default language of the device.  The date is 
                           displayed according to the pattern specified in 
                           ILOCALEPARM_DATETIME_GENDATELONGTIME.

   M or m      Month day   Displays a Month Day pattern, according to the
               pattern     specifed language code, the language set by the
                           ITEXTFORMATTER_SetLang() function, or default language of 
                           the device.  The date is displayed according to the pattern 
                           specified in ILOCALEPARM_DATETIME_MONTHDAYPATTERN.

   R or r      RFC1123     Displays the date in the pattern specified by RFC1123.
                           This is a defined standard and is read only.  It must
                           not vary between languages.    

   s           Sortable    Displays the date in a sortable pattern defined by
               date/time   ISO 8601.  This is read only and must not vary between
               pattern,    languages.  The date follows the custom pattern
               IOS 8601    "yyyy-MM-ddTHH:mm:ss"

   u           Universal   Not supported at this time.
               sortable    
               date/time   
               pattern

   Y or y      Year Month  Displays a Year Month pattern, according to the
               pattern     specifed language code, the language set by the
                           ITEXTFORMATTER_SetLang() function, or default language of 
                           the device.  The date is displayed according to the pattern 
                           specified in ILOCALEPARM_DATETIME_YEARMONTHPATTERN.

   Other argument types are not supported and are left up to the caller
   to format.  Strings of characters or AECHARs and simple integers
   require no special langauge-specific formatting, for example.


   Format Patterns

   ILocale implementations may return a pattern rather than a formatted 
   string and leave it up to the caller to build the final string.  Likewise,
   the caller may set patterns within an ILocale object as well.  Patterns
   are NULL-terminated strings of AECHARs and are made up of format
   specifiers and characters to be placed as-is into the result.

   Patterns are constructed using the following format specifiers.  All
   other characters will be placed in the result as-is:

   Numeric Format Patterns

      -        Negative Symbol
      $        Currency Symbol
      %        Percent Symbol
      .        Decimal Separator
      ,        Group Separator
      #        A number formatted with separators
      \        Escape Character - don't replace following character

   Date

      d        day (number)
      dd       day (number, two digits)
      ddd      day (abbreviated name)
      dddd     day (full name)  
      h        hour (1-12)
      hh       hour (01-12, two digits)
      H        hour (0-23)
      HH       hour (00-23, two digits)
      m        minute
      mm       minute (two digits)
      M        month (number)
      MM       month (number, two digits)
      MMM      month (abbreviated name)
      MMMM     month (full name)
      s        seconds
      ss       seconds (two digits)
      t        AM/PM designator (single char)
      tt       AM/PM designator (full)
      y        year (max two digits)
      yy       year (two digits)
      yyyy     year (four digits)
      :        time separator
      /        date separator
      "        quoted string
      \        Escape Character - don't replace following character
     
   Note: When returning patterns to the caller, the ILocale object need
   not perform symbol replacement in the pattern.  It is left up to the
   caller to format the string from the value, the format pattern and
   the symbols available from the ILocale object.
   
   If you wish to use ILocale to format strings, consider the ITextFormatter
   interface.  It can be used to format values in language-specific ways.


Prototype:
   int32 ILOCALE_FormatItem(
      ILocale *po, 
      AECHAR *bufOut, 
      uint32 dwBufSize, 
      AECHAR **ppszFormatPattern, 
      const AECHAR *pszFormatString, 
      int dwFormatStringLen, 
      AEELocaleArg *pItem);

Parameters:
   po                [in]  pointer to the ILocale interface object  
   bufOut            [in]  buffer to fill with composed string
   dwBufSize         [in]  size of buffer in bytes
   ppszFormatPattern [out] pointer to the exact pattern to use to format the 
                           item according to the format string and item type
   pszFormatString   [in]  the format code
   dwFormatStringLen [in]  the length of the format code
   pItem             [in]  the actual item to format

Return Value:
   Number of characters  in the formatted string placed into bufOut
   OR -1 if a formatted string was not generated

   If a formatting pattern is available, the ppszFormatPattern argument
   will point to the exact pattern to be used according to the argument
   type and format code.

Comments:
   This routine is used by ITEXTFORMATTER_FormatText() and other language
   aware routines to perform formatting and retrieve patterns.

Side Effects: 
   None

See Also:
   AEELocaleArg
   AEELocaleArg Types
   ITEXTFORMATTER_FormatText()
   
===========================================================================

Function: ILOCALE_WSTRNCMP()

Description:
   This function compares at most length chars of the two strings, s1 and s2, 
   according to any specific rules of the language the ILocale implementation
   represents. It returns an integer value that indicates the comparison result. 

Prototype:
   int ILOCALE_WSTRNCMP(
      ILocale *po,
      AECHAR *s1, 
      AECHAR *s2, 
      unsigned int length);

Parameters:
   po:      Pointer to the ILocale Interface.
   s1:      pointer to first NULL terminated string 
   s2:      pointer to second NULL terminated string 
   length:  maximum number of AECHARS to compare 

Return Value:
   0: if s1 == s2 
   > 0: if s1 > s2 
   < 0: if s1 < s2 

Comments:
   None 

See Also:
   None

=============================================================================

Function: ILOCALE_WSTRCMP()

Description:
   This function compares the two strings, s1 and s2, according to any 
   specific rules of the language the ILocale implementation represents. It 
   returns an integer value that indicates the comparison result. 

Prototype:
   int ILOCALE_WSTRCMP(
      ILocale *po,
      AECHAR *s1, 
      AECHAR *s2);

Parameters:
   po: Pointer to the ILocale Interface.
   s1: pointer to first NULL terminated string 
   s2: pointer to second NULL terminated string 

Return Value:
   0: if s1 == s2 
   > 0: if s1 > s2 
   < 0: if s1 < s2  

Comments:
   None 

See Also:
   None

=============================================================================

Function: ILOCALE_WSTRUPPER()

Description:
   This function converts all lower case characters in a wide string to upper 
   case, according to any language-specific rules of the language the ILocale
   implementation represents.

Prototype:
   void ILOCALE_WSTRUPPER(
      ILocale *po, 
      AECHAR *pszDest);

Parameters:
   po:      Pointer to the ILocale Interface.
   pszDest: On input, this is a pointer to a NULL terminated source string.
            On return, this buffer contains the converted string. 

Return Value:
   None 

Comments:
   None 

See Also:
   ILOCALE_WSTRLOWER()

=============================================================================

Function: ILOCALE_WSTRLOWER()

Description:
   This function converts all upper case characters in a wide string to lower 
   case, according to any language-specific rules of the language the ILocale
   implementation represents.

Prototype:
   void ILOCALE_WSTRLOWER(
      ILocale *po, 
      AECHAR *pszDest);

Parameters:
   po:      Pointer to the ILocale Interface.
   pszDest: On input, this is a pointer to a NULL terminated source string.
            On return, this buffer contains the converted string. 

Return Value:
   None 

Comments:
   None 

See Also:
   ILOCALE_WSTRUPPER()

===========================================================================

Function: ILOCALE_BreakLine()

Description:
   This routine is used to perform language-dependent line breaking.  It
   will properly break a line at line breaking charcters or whitespace,
   not in the middle of words or graphemes.  This routine returns a pointer 
   to the first hard breaking character in pszText on or before pszClip, or
   the last whitespace character in pszText before pszClip.

Prototype:
   AECHAR *ILOCALE_BreakLine(
      ILocale *po, 
      AECHAR *pszText, 
      AECHAR *pszClip, 
      AECHAR *pszEnd);

Parameters:
   po:      Pointer to the ILocale Interface.
   pszText: Pointer to the text, or the position after the last break.
   pszClip: Pointer to the last character that fits
   pszEnd:  Pointer to the end of the text

Return Value:
   Returns a pointer to the character at which the text should be
   broken.

Comments:
   None 

See Also:
   ILOCALEPARM_STRING_HARDBREAK
   ILOCALEPARM_STRING_SOFTBREAK

===========================================================================
*/
#endif /* __AEELOCALE__ */

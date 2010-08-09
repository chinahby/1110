/*
  ========================================================================

  FILE:  LocaleBase.h
  
  SERVICES:  

  GENERAL DESCRIPTION: Base container definitions used by locale
  implementations

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#ifndef __LOCALEBASE_H__
#define __LOCALEBASE_H__

#include "AEELocale.h"
#include "AEEShell.h"
#include "wutil.h"

// FormatEnum - things we store in the FormatTable
enum {
   ILOCALE_CURRENCY_DECIMALDIGITS = 0,
   ILOCALE_CURRENCY_DECIMALSEPARATOR,
   ILOCALE_CURRENCY_GROUPSEPARATOR,
   ILOCALE_CURRENCY_GROUPSIZES, 
   ILOCALE_CURRENCY_NEGATIVEPATTERN,
   ILOCALE_CURRENCY_POSITIVEPATTERN,
   ILOCALE_CURRENCY_SYMBOL,
   ILOCALE_NUMBER_DECIMALDIGITS,
   ILOCALE_NUMBER_DECIMALSEPARATOR,
   ILOCALE_NUMBER_GROUPSEPARATOR,
   ILOCALE_NUMBER_GROUPSIZES,
   ILOCALE_NUMBER_NEGATIVEPATTERN,
   ILOCALE_PERCENT_DECIMALDIGITS,
   ILOCALE_PERCENT_DECIMALSEPARATOR,
   ILOCALE_PERCENT_GROUPSEPARATOR,
   ILOCALE_PERCENT_GROUPSIZES,
   ILOCALE_PERCENT_NEGATIVEPATTERN,
   ILOCALE_PERCENT_POSITIVEPATTERN,
   ILOCALE_PERCENT_SYMBOL,
   ILOCALE_PERMILLE_SYMBOL,
   ILOCALE_NANSYMBOL,
   ILOCALE_NEGATIVEINIFINITYSYMBOL,
   ILOCALE_NEGATIVESIGN,
   ILOCALE_POSITIVEINIFINITYSYMBOL,
   ILOCALE_POSITIVESIGN,
   ILOCALE_DATETIME_SHORTDAYNAMES,
   ILOCALE_DATETIME_SHORTMONTHNAMES,
   ILOCALE_DATETIME_DAYNAMES,
   ILOCALE_DATETIME_MONTHNAMES,
   ILOCALE_DATETIME_AMDESIGNATOR,
   ILOCALE_DATETIME_DATESEPARATOR,
   ILOCALE_DATETIME_PMDESIGNATOR,
   ILOCALE_DATETIME_TIMESEPARATOR,
   ILOCALE_DATETIME_FIRSTWEEKDAY,
   ILOCALE_DATETIME_FULLDATELTIMEPATTERN,
   ILOCALE_DATETIME_FULLDATESTIMEPATTERN,
   ILOCALE_DATETIME_GENDATELONGTIME,
   ILOCALE_DATETIME_GENDATESHORTTIME,
   ILOCALE_DATETIME_LONGDATEPATTERN,
   ILOCALE_DATETIME_LONGTIMEPATTERN,
   ILOCALE_DATETIME_MONTHDAYPATTERN,
   ILOCALE_DATETIME_SHORTDATEPATTERN,
   ILOCALE_DATETIME_SHORTTIMEPATTERN,
   ILOCALE_DATETIME_YEARMONTHPATTERN,
   ILOCALE_DATETIME_RFC1123PATTERN,
   ILOCALE_DATETIME_SORTABLEDATETIME,
   ILOCALE_STRING_HARDBREAK,
   ILOCALE_STRING_SOFTBREAK,
   ILOCALE_FORMAT_LIMIT
};

// Format Table Cell
typedef struct
{
   const AECHAR *pawData;     // the table entry, most often an AECHAR *
   char cFlags;               // cell flags
} FormatTable;

#define FORMATTABLE_FLAG_INT        0x01     // this format table pItem is an integer
#define FORMATTABLE_FLAG_CONST      0x02     // this format table pItem is constant and can't be changed
#define FORMATTABLE_FLAG_ALLOCATED  0x04     // this format table pItem was allocated and must be freed

// Locale
typedef struct 
{
   const AEEVTBL(ILocale) *pvt;

   uint32  nRefs;
   IShell *piShell;
   IModule *piModule;

   FormatTable gtFormats[ILOCALE_FORMAT_LIMIT];       // format table 
} LocaleBase;

void             LocaleBase_Dtor(LocaleBase *pMe);
void             LocaleBase_Ctor(LocaleBase *pMe, AEEVTBL(ILocale) *pvt, IShell *piShell, IModule *piModule);

uint32	        LocaleBase_AddRef(ILocale* po);
uint32	        LocaleBase_Release(ILocale* po);
int              LocaleBase_QueryInterface(ILocale *pMe, AEECLSID id, void **ppo); 
int              LocaleBase_SetParm(ILocale *po, int16 nParmID, int32 p1);
int              LocaleBase_GetParm(ILocale *po, int16 nParmID, int32 *pp1);
int32            LocaleBase_FormatItem(ILocale *po, AECHAR *bufOut, uint32 dwBufSize, AECHAR **ppszFormatPattern, const AECHAR *pszFormatString, int dwFormatStringLen, AEELocaleArg *pItem);
int              LocaleBase_wstrncmp(ILocale *po, AECHAR *s1, AECHAR *s2, unsigned int length);
int              LocaleBase_wstrcmp(ILocale *po, AECHAR *s1, AECHAR *s2);
void             LocaleBase_wstrupper(ILocale *po, AECHAR *pszDest);
void             LocaleBase_wstrlower(ILocale *po, AECHAR *pszDest);
AECHAR *         LocaleBase_BreakLine(ILocale *po, AECHAR *pszText, AECHAR *pszClip, AECHAR *pszEnd);

//
// Utility
//

#define LBASE(p) ((LocaleBase *)(void*)p)

#endif //__LOCALEBASE_H__
/*========================================================================

   FILE:             localedede.c

   SERVICES:         Locale specific string and text formatting utilities. 

   LANGUAGE:         German (Germany)

   PUBLIC CLASSES:   ILocale

   DESCRIPTION:      This file describes an interface that provides 
                     language dependent string formatting for a specific 
                     language. The intent is that there be one of these 
                     objects for each language your device supports.  
                     Additional implementations  may be added later to 
                     support additional languages.

                     This ILocale object is provided as a sample for 
                     developers wishing to implement their own.  It is 
                     intended as a starting point for your ILocale 
                     object and is NOT to be used as a complete 
                     implementation to support German.

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================*/

/*---------------------------------------------------------------------------------
      Include Files
---------------------------------------------------------------------------------*/

#include "AEELocale.h"
#include "AEEShell.h"

// Note, when implementing your own ILocale object, obtain
// a valid BREW class id from the class id generator online
// and use it here.
#include "AEECLSID_LOCALEDEDE.BID"


/*---------------------------------------------------------------------------------
      Type Declarations
---------------------------------------------------------------------------------*/

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
} Locale;


/*---------------------------------------------------------------------------------
      Globals
---------------------------------------------------------------------------------*/

// Note, the constants following are the language specific day names, 
// month names, date format patterns, time format patterns, etc. for
// the language this locale supports - in this case, GB English.  
// You will need to change the following to something appropriate for
// the language your ILocale implementation supports.

// Format variables, strings, characters and patterns for German (Germany)
static const int    Number_DecimalDigits                  = 0;
static const AECHAR Number_DecimalSeparator[]             = { ',',0 };
static const AECHAR Number_GroupSeparator[]               = { '.',0 };
static const int    Number_GroupSize                      = 3;
static const AECHAR Number_NegativePattern[]              = { '-','#',0 };

static const int    Currency_DecimalDigits                = 2;
static const AECHAR Currency_DecimalSeparator[]           = { ',',0 };
static const AECHAR Currency_GroupSeparator[]             = { '.',0 };
static const int    Currenct_GroupSizes                   = 3;
static const AECHAR Currency_NegativePattern []           = { '-','#',' ','$',0 };
static const AECHAR Currency_PositivePattern[]            = { '#',' ','$',0 };
//static const AECHAR Currency_CurrencySymbol[]             = { '€',0 };
static const AECHAR Currency_CurrencySymbol[]             = { 0x80 ,0 };

static const int    Percent_DecimalDigits                 = 0;
static const AECHAR Percent_DecimalSeparator[]            = { ',',0 };
static const AECHAR Percent_GroupSeparator[]              = { '.',0 };
static const int    Percent_GroupSize                     = 3;
static const AECHAR Percent_NegativePattern[]             = { '(','#','%',')',0 };
static const AECHAR Percent_PositivePattern[]             = { '#','%',0 };
static const AECHAR Percent_PercentSymbol[]               = { '%',0 };
static const AECHAR Percent_PermilleSymbol[]              = { 0 };

static const AECHAR NANSymbol[]                           = { 'N','A','N',0 };
static const AECHAR NegativeInfinitySymbol[]              = { '-','i','n','f',0 };
static const AECHAR NegativeSign[]                        = { '-',0 };
static const AECHAR PositiveInfinitySymbol[]              = { 'i','n','f',0 };
static const AECHAR PositiveSign[]                        = { '+',0 };

static const AECHAR DateTime_AMDesignator[]               = { 'A','M',0 };
static const AECHAR DateTime_DateSeparator[]              = { '.',0 };
static const AECHAR DateTime_PMDesignator[]               = { 'P','M',0 };
static const AECHAR DateTime_TimeSeparator[]              = { ':',0 };
static const int DateTime_FirstWeekDay                    = 1;

static const AECHAR DateTime_FullDateLongTimePattern[]    = { 'd','d','d','d',',',' ','d','.',' ','M','M','M','M',' ','y','y','y','y',' ','H',':','m','m',':','s','s', 0 };  
static const AECHAR DateTime_FullDateShortTimePattern[]   = { 'd','d','d','d',',',' ','d','.',' ','M','M','M','M',' ','y','y','y','y',' ','H',':','m','m',' ',0 };  
static const AECHAR DateTime_GenDateShortTimePattern[]    = { 'd','d','/','M','M','/','y','y','y','y',' ','h',':','m','m',0 };
static const AECHAR DateTime_GenDateLongTimePattern[]     = { 'd','d','/','M','M','/','y','y','y','y',' ','H',':','m','m',':','s','s',0 };

static const AECHAR DateTime_LongDatePattern[]            = { 'd','d','d','d',',',' ','d','.',' ','M','M','M','M',' ','y','y','y','y', 0 };
static const AECHAR DateTime_LongTimePattern[]            = { 'H','H',':','m','m',':','s','s',0 };
static const AECHAR DateTime_MonthDayPattern[]            = { 'd','.',' ','M','M','M',0 };
static const AECHAR DateTime_ShortDatePattern[]           = { 'd','d','/','M','M','/','y','y','y','y',0 };
static const AECHAR DateTime_ShortTimePattern[]           = { 'H',':','m','m',0 };
static const AECHAR DateTime_YearMonthPattern[]           = { 'M','M','M',' ','y','y','y','y',0 };

static const AECHAR DateTime_RFC1123Pattern[]             = { 'd','d','d',',',' ','d','d',' ','M','M','M',' ','y','y','y','y',' ','H','H',':','m','m',':','s','s',' ','G','\\','M','T',0  };
static const AECHAR DateTime_SortableDatePattern[]        = { 'y','y','y','y','-','M','M','-','d','d','T','H','H',':','m','m',':','s','s',0 };

static const AECHAR String_HardBreak[]                    = { '\r','\n',0 };
static const AECHAR String_SoftBreak[]                    = { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,0 };

static const AECHAR DateTime_ShortDayNames[] = {
   'M', 'o', 0,
   'D', 'i', 0,
   'M', 'i', 0,
   'D', 'o', 0,
   'F', 'r', 0,
   'S', 'a', 0,
   'S', 'o', 0,
   0 };

static const AECHAR DateTime_ShortMonthNames[] = {
   'J', 'a', 'n', 0,
   'F', 'e', 'b', 0,
   'M', 0xE4, 'r', 'z', 0,
   'A', 'p', 'r', 0,
   'M', 'a', 'i', 0,
   'J', 'u', 'n', 'i', 0,
   'J', 'u', 'l', 'i', 0,
   'A', 'u', 'g', 0,
   'S', 'e', 'p', 't', 0,
   'O', 'k', 't', 0,
   'N', 'o', 'v', 0,
   'D', 'e', 'z', 0,
   0 };

static const AECHAR DateTime_DayNames[] = {
   'M', 'o', 'n', 't', 'a', 'g', 0,
   'D', 'i', 'e', 'n', 's', 't', 'a','g', 0,
   'M', 'i', 't', 't', 'w', 'o', 'c', 'h', 0,
   'D','o','n','n','e','r','s','t','a','g', 0,
   'F','r','e','i','t','a','g', 0,
   'S','a','m','s','t','a','g', 0,
   'S','o','n','n','t','a','g', 0,
   0 };

static const AECHAR DateTime_MonthNames[] = {
   'J','a','n','u','a','r', 0,
   'F','e','b','r','u','a','r', 0,
   'M',0xE4,'r','z', 0,
   'A','p','r','i','l', 0,
   'M','a','i', 0,
   'J','u','n','i', 0,
   'J','u','l','i', 0,
   'A', 'u', 'g', 'u', 's', 't', 0,
   'S', 'e', 'p', 't', 'e', 'm', 'b', 'e', 'r', 0,
   'O', 'k', 't', 'o', 'b', 'e', 'r', 0,
   'N', 'o', 'v', 'e', 'm', 'b', 'e', 'r', 0,
   'D', 'e', 'z', 'e', 'm', 'b', 'e', 'r', 0,
   0 };


// parameter to format cell enum conversion.  This allows us to support only
// a subset of the available parameters.
static const int gtParamToFormats[] = {
   ILOCALEPARM_CURRENCY_DECIMALDIGITS,
   ILOCALEPARM_CURRENCY_DECIMALSEPARATOR,
   ILOCALEPARM_CURRENCY_GROUPSEPARATOR,
   ILOCALEPARM_CURRENCY_GROUPSIZES, 
   ILOCALEPARM_CURRENCY_NEGATIVEPATTERN,
   ILOCALEPARM_CURRENCY_POSITIVEPATTERN,
   ILOCALEPARM_CURRENCY_SYMBOL,
   ILOCALEPARM_NUMBER_DECIMALDIGITS,
   ILOCALEPARM_NUMBER_DECIMALSEPARATOR,
   ILOCALEPARM_NUMBER_GROUPSEPARATOR,
   ILOCALEPARM_NUMBER_GROUPSIZES,
   ILOCALEPARM_NUMBER_NEGATIVEPATTERN,
   ILOCALEPARM_PERCENT_DECIMALDIGITS,
   ILOCALEPARM_PERCENT_DECIMALSEPARATOR,
   ILOCALEPARM_PERCENT_GROUPSEPARATOR,
   ILOCALEPARM_PERCENT_GROUPSIZES,
   ILOCALEPARM_PERCENT_NEGATIVEPATTERN,
   ILOCALEPARM_PERCENT_POSITIVEPATTERN,
   ILOCALEPARM_PERCENT_SYMBOL,
   ILOCALEPARM_PERMILLE_SYMBOL,
   ILOCALEPARM_NANSYMBOL,
   ILOCALEPARM_NEGATIVEINIFINITYSYMBOL,
   ILOCALEPARM_NEGATIVESIGN,
   ILOCALEPARM_POSITIVEINIFINITYSYMBOL,
   ILOCALEPARM_POSITIVESIGN,
   ILOCALEPARM_DATETIME_SHORTDAYNAMES,
   ILOCALEPARM_DATETIME_SHORTMONTHNAMES,
   ILOCALEPARM_DATETIME_DAYNAMES,
   ILOCALEPARM_DATETIME_MONTHNAMES,
   ILOCALEPARM_DATETIME_AMDESIGNATOR,
   ILOCALEPARM_DATETIME_DATESEPARATOR,
   ILOCALEPARM_DATETIME_PMDESIGNATOR,
   ILOCALEPARM_DATETIME_TIMESEPARATOR,
   ILOCALEPARM_DATETIME_FIRSTWEEKDAY,
   ILOCALEPARM_DATETIME_FULLDATELTIMEPATTERN,
   ILOCALEPARM_DATETIME_FULLDATESTIMEPATTERN,
   ILOCALEPARM_DATETIME_GENDATELONGTIME,
   ILOCALEPARM_DATETIME_GENDATESHORTTIME,
   ILOCALEPARM_DATETIME_LONGDATEPATTERN,
   ILOCALEPARM_DATETIME_LONGTIMEPATTERN,
   ILOCALEPARM_DATETIME_MONTHDAYPATTERN,
   ILOCALEPARM_DATETIME_SHORTDATEPATTERN,
   ILOCALEPARM_DATETIME_SHORTTIMEPATTERN,
   ILOCALEPARM_DATETIME_YEARMONTHPATTERN,
   ILOCALEPARM_DATETIME_RFC1123PATTERN,
   ILOCALEPARM_DATETIME_SORTABLEDATETIME,
   ILOCALEPARM_STRING_HARDBREAK,
   ILOCALEPARM_STRING_SOFTBREAK };

/*---------------------------------------------------------------------------------
      Function Prototypes
---------------------------------------------------------------------------------*/

// 
// ILocale
//

static uint32	         Locale_AddRef(ILocale* po);
static uint32	         Locale_Release(ILocale* po);
static int              Locale_QueryInterface(ILocale *pMe, AEECLSID id, void **ppo); 
static int              Locale_SetParm(ILocale *po, int16 nParmID, int32 p1);
static int              Locale_GetParm(ILocale *po, int16 nParmID, int32 *pp1);
static int32            Locale_FormatItem(ILocale *po, AECHAR *bufOut, uint32 dwBufSize, AECHAR **ppszFormatPattern, 
                                          const AECHAR *pszFormatString, int dwFormatStringLen, AEELocaleArg *pItem);
static int              Locale_wstrncmp(ILocale *po, AECHAR *s1, AECHAR *s2, unsigned int length);
static int              Locale_wstrcmp(ILocale *po, AECHAR *s1, AECHAR *s2);
static void             Locale_wstrupper(ILocale *po, AECHAR *pszDest);
static void             Locale_wstrlower(ILocale *po, AECHAR *pszDest);
static AECHAR *         Locale_BreakLine(ILocale *po, AECHAR *pszText, AECHAR *pszClip, AECHAR *pszEnd);

// Local Routines
static int  Locale_New(ILocale **ppo, IShell *piShell, IModule *piModule);
static void Locale_Ctor(Locale *pMe, AEEVTBL(ILocale) *pvt, IShell *piShell, IModule *piModule);
static void Locale_Dtor(Locale *pMe);

static void FormatTable_Dtor(Locale *pMe);
static void FormatTable_Ctor(Locale *pMe);
static int  LocateFormatTableItem(int16 nParmID);

//
// Utility
//

// allocates an object with an appended vtbl
#ifndef MALLOCREC_VTBL
#define MALLOCREC_VTBL(typ,vtt)     MALLOCREC_EX(typ, sizeof(AEEVTBL(vtt)) )
#endif

// get pointer to vtbl from object allocated w/ MALLOCREC_VTBL
#ifndef GETVTBL
#define GETVTBL(po,vtt)             ((AEEVTBL(vtt)*)((po)+1))
#endif

#ifndef ADDREFIF
#define ADDREFIF(p)        do { if (p) IBASE_AddRef((IBase*) (void *) p); } while(0)
#endif

#ifndef RELEASEIF
#define RELEASEIF(p)       RELEASEPPIF((IBase**) (void *) &p)

static __inline void RELEASEPPIF(IBase **p) {
   if (*p) {
      IBASE_Release(*p);
      *p = 0;
   }
}
#endif

/*---------------------------------------------------------------------------------
      ILocale
---------------------------------------------------------------------------------*/

/* -----------------------------------------------------------------------
    CreateInstance
   --- */
int xModule_LocaleDEDECreateInstance(IModule *module, IShell *shell, AEECLSID idClass, void **pp)
{
   // Note, when implementing your own ILocale object, obtain
   // a valid BREW class id from the class id generator online
   // and use it here.
   if (idClass == AEECLSID_LOCALEDEDE)
      return Locale_New((ILocale **)pp, shell, module);

   *pp = 0;
   return ECLASSNOTSUPPORT;
}

/* -----------------------------------------------------------------------
    Public function - see interface definition      
   --- */
static uint32 Locale_AddRef(ILocale* po)
{
   Locale *pMe = (Locale *)(void *)po;
   return ++pMe->nRefs;
}

/* -----------------------------------------------------------------------
    Public function - see interface definition      
   --- */
static uint32 Locale_Release(ILocale* po)
{
   Locale *pMe = (Locale *)(void *)po;

    if (--pMe->nRefs)
      return pMe->nRefs;

   Locale_Dtor(pMe);

   FREE(pMe);
   return(0);
}

/* -----------------------------------------------------------------------
    Public function - see interface definition      
   --- */
static int Locale_QueryInterface(ILocale *po, AEECLSID id, void **ppo)
{
   Locale *pMe = (Locale *)(void *)po;

   if (id == AEEIID_LOCALE) {
      *ppo = pMe;
      IQI_AddRef(*ppo);
      return SUCCESS;
   } else {
      return ECLASSNOTSUPPORT;
   }
}
 
/* -----------------------------------------------------------------------
    Public function - see interface definition      
   --- */
static int Locale_SetParm(ILocale *po, int16 nParmID, int32 p1)
{
   Locale *pMe = (Locale *)(void *)po;
   int dwItem;
   AECHAR *pawNew = NULL;
   int dwNewLen;
   uint32 nNewLen=0;

   // is the caller asking to set a specific format pItem?
   dwItem = LocateFormatTableItem(nParmID);
   if (dwItem != -1) {
      // this is a format table pItem.  Is it a const pItem?
      if (pMe->gtFormats[dwItem].cFlags & FORMATTABLE_FLAG_CONST)
         return (EBADPARM);
      
      if (pMe->gtFormats[dwItem].cFlags & FORMATTABLE_FLAG_INT) {
        
         // this pItem is an integer.
         pMe->gtFormats[dwItem].pawData = (const AECHAR *)p1;

      } else {

         // this is an AECHAR
         dwNewLen = WSTRLEN((AECHAR *)p1);
         if (dwNewLen) {
            nNewLen = (uint32)(dwNewLen + 1) * sizeof(AECHAR);
            pawNew = (AECHAR *)MALLOC(nNewLen);
         }

         if (pawNew) {
            // out with the old and in with the new ...
            (void)WSTRLCPY(pawNew, (AECHAR *)p1, nNewLen);
            if (pMe->gtFormats[dwItem].cFlags & FORMATTABLE_FLAG_ALLOCATED)
               FREEIF(pMe->gtFormats[dwItem].pawData);
            pMe->gtFormats[dwItem].pawData = pawNew;
            pMe->gtFormats[dwItem].cFlags |= FORMATTABLE_FLAG_ALLOCATED;

         } else {
            return (ENOMEMORY);
         }
      }
   } else {
      // bad paramter specified
      return (EBADPARM);
   }

   return (SUCCESS);
}

/* -----------------------------------------------------------------------
    Public function - see interface definition      
   --- */
static int Locale_GetParm(ILocale *po, int16 nParmID, int32 *pp1)
{
   int dwResult = EBADPARM;
   Locale *pMe = (Locale *)(void *)po;
   int dwItem;

   if (pp1) {

      *pp1 = 0;

      // see if the caller is asking for a formatting parameter
      dwItem = LocateFormatTableItem(nParmID);
      if (dwItem != -1) {
         // return the proper format pItem
         *pp1 = (int32)pMe->gtFormats[dwItem].pawData;
         dwResult = SUCCESS;
      }
   }

   return (dwResult);
}

/* -----------------------------------------------------------------------
    Public function - see interface definition in AEELocale.h
    
    This routine can either build and return a format pattern to the
    caller, or actually perform the formatting.

    Return -1 on error.

    Return -1 but set the ppszFormatPattern parameter to return formatting
    only.

    Return number of bytes written (or needed) including NULL and fill
    the buffer with the formatted string to perform formatting here.

    Note, this ILocale object simply returns formatting patterns
    and relies on caller to do the right thing with them.  If the 
    patterns required for your lanuage are too complext to be
    represented, change this routine to actually generate the formatted
    string data and return it.
   --- */
static int32 Locale_FormatItem(ILocale *po, AECHAR *bufOut, uint32 dwBufSize, AECHAR **ppszFormatPattern, 
                               const AECHAR *pszFormatString, int dwFormatStringLen, AEELocaleArg *pItem)
{
   int dwResult = -1;
   AECHAR aFormatCode;
   (void)dwBufSize;
   (void)bufOut;

   // sanity check
   if (pszFormatString && dwFormatStringLen) {

      // for now, assume pattern is a single character pattern   
      aFormatCode = *pszFormatString;

      // try to return an exact pattern
      if (ppszFormatPattern) {

         *ppszFormatPattern = NULL;

         // Determine what type of pattern to return
         switch (pItem->cType) {

            // Numeric pattern
            case AEELOCALEARGTYPE_SIGNEDDECIMAL:
            case AEELOCALEARGTYPE_UNSIGNEDDECIMAL:
         
               // numeric patterns are case insensitive
               if ((aFormatCode >= (AECHAR)'A' && aFormatCode <= (AECHAR)'Z')) {
                  aFormatCode += (AECHAR)('a' - 'A');
               }

               switch (aFormatCode) {
    
                  // request for the signed decimal pattern
                  case ILOCALE_FORMATCODE_NUMBER_NUMBER:
                     if ((pItem->cType == AEELOCALEARGTYPE_SIGNEDDECIMAL) && (pItem->u.d < 0))
                        (void)Locale_GetParm(po, ILOCALE_NUMBER_NEGATIVEPATTERN, (int32 *)(void *)ppszFormatPattern);
                     break;

                  // request for the currency pattern
                  case ILOCALE_FORMATCODE_NUMBER_CURRENCY:
                     if ((pItem->cType == AEELOCALEARGTYPE_SIGNEDDECIMAL) && (pItem->u.d < 0))
                        (void)Locale_GetParm(po, ILOCALE_CURRENCY_NEGATIVEPATTERN, (int32 *)(void *)ppszFormatPattern);
                     else
                        (void)Locale_GetParm(po, ILOCALE_CURRENCY_POSITIVEPATTERN, (int32 *)(void *)ppszFormatPattern);
                     break;

                  // request for the percentage pattern
                  case ILOCALE_FORMATCODE_NUMBER_PERCENT:
                     if ((pItem->cType == AEELOCALEARGTYPE_SIGNEDDECIMAL) && (pItem->u.d < 0))
                        (void)Locale_GetParm(po, ILOCALE_PERCENT_NEGATIVEPATTERN, (int32 *)(void *)ppszFormatPattern);
                     else
                        (void)Locale_GetParm(po, ILOCALE_PERCENT_POSITIVEPATTERN, (int32 *)(void *)ppszFormatPattern);
                     break;

                  // unrecognized format code.  Punt.
                  default:
                     break;
               }
               break;

            // Date/Time pattern
            case AEELOCALEARGTYPE_JULIANDATE:
         
               switch (aFormatCode) {
   
                  case ILOCALE_FORMATCODE_DATETIME_SHORTDATE:
                     (void)Locale_GetParm(po, ILOCALE_DATETIME_SHORTDATEPATTERN, (int32 *)(void *)ppszFormatPattern);
                     break;
         
                  case ILOCALE_FORMATCODE_DATETIME_LONGDATE:
                     (void)Locale_GetParm(po, ILOCALE_DATETIME_LONGDATEPATTERN, (int32 *)(void *)ppszFormatPattern);
                     break;
         
                  case ILOCALE_FORMATCODE_DATETIME_SHORTTIME:
                     (void)Locale_GetParm(po, ILOCALE_DATETIME_SHORTTIMEPATTERN, (int32 *)(void *)ppszFormatPattern);
                     break;
         
                  case ILOCALE_FORMATCODE_DATETIME_LONGTIME:
                     (void)Locale_GetParm(po, ILOCALE_DATETIME_LONGTIMEPATTERN, (int32 *)(void *)ppszFormatPattern);
                     break;
            
                  case ILOCALE_FORMATCODE_DATETIME_FULLDATESHORTTIME:
                     (void)Locale_GetParm(po, ILOCALE_DATETIME_FULLDATESTIMEPATTERN, (int32 *)(void *)ppszFormatPattern);
                     break;
          
                  case ILOCALE_FORMATCODE_DATETIME_FULLDATELONGTIME:
                     (void)Locale_GetParm(po, ILOCALE_DATETIME_FULLDATELTIMEPATTERN, (int32 *)(void *)ppszFormatPattern);
                     break;
            
                  case ILOCALE_FORMATCODE_DATETIME_GENDATESHORTTIME:
                     (void)Locale_GetParm(po, ILOCALE_DATETIME_GENDATESHORTTIME, (int32 *)(void *)ppszFormatPattern);
                     break;
         
                  case ILOCALE_FORMATCODE_DATETIME_GENDATELONGTIME:
                     (void)Locale_GetParm(po, ILOCALE_DATETIME_GENDATELONGTIME, (int32 *)(void *)ppszFormatPattern);
                     break;
            
                  case ILOCALE_FORMATCODE_DATETIME_MONTHDAY:
                  case ILOCALE_FORMATCODE_DATETIME_MONTHDAYALT:
                     (void)Locale_GetParm(po, ILOCALE_DATETIME_MONTHDAYPATTERN, (int32 *)(void *)ppszFormatPattern);
                     break;
         
                  case ILOCALE_FORMATCODE_DATETIME_RFC1123:
                  case ILOCALE_FORMATCODE_DATETIME_RFC1121ALT:
                     (void)Locale_GetParm(po, ILOCALE_DATETIME_RFC1123PATTERN, (int32 *)(void *)ppszFormatPattern);
                     break;
         
                  case ILOCALE_FORMATCODE_DATETIME_SORTABLE:
                     (void)Locale_GetParm(po, ILOCALE_DATETIME_SORTABLEDATETIME, (int32 *)(void *)ppszFormatPattern);
                     break;
         
                  case ILOCALE_FORMATCODE_DATETIME_YEARMONTH:
                  case ILOCALE_FORMATCODE_DATETIME_YEARMONTHALT:
                     (void)Locale_GetParm(po, ILOCALE_DATETIME_YEARMONTHPATTERN, (int32 *)(void *)ppszFormatPattern);
                     break;        

                  // unrecognized format code.  Punt.
                  default:
                     break;
               }
               break;

            // unhandled argument type.  Let the caller handle it.
            case AEELOCALEARGTYPE_CHAR:
            case AEELOCALEARGTYPE_AECHAR:
            case AEELOCALEARGTYPE_CHARPTR:
            case AEELOCALEARGTYPE_AECHARPTR:
            case AEELOCALEARGTYPE_POINTER:
            default:
               break;
         }
      }
   }

   return (dwResult);
}

/* -----------------------------------------------------------------------
    Public function - see interface definition
    
    Note, if the language you're implementing requires a special
    implementation, this is where to put it.
   --- */
static int Locale_wstrncmp(ILocale *po, AECHAR *s1, AECHAR *s2, unsigned int length)
{
   // German (Germany) - as implemted by this ILocale - can use BREW's library function
   (void)po;
   return (WSTRNCMP(s1, s2, length));
}

/* -----------------------------------------------------------------------
    Public function - see interface definition
    
    Note, if the language you're implementing requires a special
    implementation, this is where to put it.
   --- */
static int Locale_wstrcmp(ILocale *po, AECHAR *s1, AECHAR *s2)
{
   // German (Germany) - as implemted by this ILocale - can use BREW's library function
   (void)po;
   return (WSTRCMP(s1, s2));
}

/* -----------------------------------------------------------------------
    Public function - see interface definition
    
    Note, if the language you're implementing requires a special
    implementation, this is where to put it.
   --- */
static void Locale_wstrupper(ILocale *po, AECHAR *pszDest)
{
   // German (Germany) - as implemted by this ILocale - can use BREW's library function
   (void)po;
   WSTRUPPER(pszDest);
}

/* -----------------------------------------------------------------------
    Public function - see interface definition
    
    Note, if the language you're implementing requires a special
    implementation, this is where to put it.
   --- */
static void Locale_wstrlower(ILocale *po, AECHAR *pszDest)
{
   // German (Germany) - as implemted by this ILocale - can use BREW's library function
   (void)po;
   WSTRLOWER(pszDest);
}

/* -----------------------------------------------------------------------
    Public function - see interface definition
    
    Note, if the language you're implementing requires a special
    implementation, this is where to put it.  
    
    For German, lines are broken at the first hard break or last
    whitespace (soft break).
   --- */
static AECHAR *Locale_BreakLine(ILocale *po, AECHAR *pszText, AECHAR *pszClip, AECHAR *pszEnd)
{
   AECHAR *pszScan = pszText;
   AECHAR *pszBreak = pszClip;
   AECHAR *pszHardBreak, *pszWhiteSpace;
   int dwHardBreakLen, i;

   (void)pszEnd;

   // get our breaking characters
   (void)Locale_GetParm(po, ILOCALE_STRING_HARDBREAK, (int32 *)(void *)(&pszHardBreak));
   dwHardBreakLen = pszHardBreak ? WSTRLEN(pszHardBreak) : 0;
   (void)Locale_GetParm(po, ILOCALE_STRING_SOFTBREAK, (int32 *)(void *)(&pszWhiteSpace));

   // Go through word, noting last break position     
   while (pszScan < pszClip) {

      // is this a hard break character sequence?
      for (i = 0; i < dwHardBreakLen; i++) {
         if (pszScan + i <= pszClip) {
            if (NULL == pszHardBreak || pszScan[i] != pszHardBreak[i])
               break;
         }
      }
      if (dwHardBreakLen && (i == dwHardBreakLen)) {
         // We're looking at a hard line break sequence.
         // break the line here.
         pszBreak = pszScan + dwHardBreakLen;
         break;
      }

      // is this whitespace?
      if (WSTRCHR(pszWhiteSpace, *pszScan)) {
         // then remember that we can break here if we need to
         pszBreak = pszScan;
      }

      // next character
      pszScan++;
   }

   return (pszBreak);
}

/*---------------------------------------------------------------------------------
      CreateInstance
---------------------------------------------------------------------------------*/

/* -----------------------------------------------------------------------
    Locale_Dtor
   --- */
static void Locale_Dtor(Locale *pMe)
{
   FormatTable_Dtor(pMe);

   RELEASEIF(pMe->piModule);
   RELEASEIF(pMe->piShell);
}

/* -----------------------------------------------------------------------
    Locale_Ctor 
   --- */
static void Locale_Ctor(Locale *pMe, AEEVTBL(ILocale) *pvt, IShell *piShell, IModule *piModule)
{
   pMe->pvt = pvt;

   pvt->AddRef          = Locale_AddRef;
   pvt->Release         = Locale_Release;
   pvt->QueryInterface  = Locale_QueryInterface;
	pvt->SetParm         = Locale_SetParm;
	pvt->GetParm         = Locale_GetParm;
   pvt->FormatItem      = Locale_FormatItem;
   pvt->wstrncmp        = Locale_wstrncmp;
   pvt->wstrcmp         = Locale_wstrcmp;
   pvt->wstrupper       = Locale_wstrupper;
   pvt->wstrlower       = Locale_wstrlower;
   pvt->breakline       = Locale_BreakLine;

   pMe->nRefs = 1;

   pMe->piShell = piShell;
   ISHELL_AddRef(pMe->piShell);

   pMe->piModule = piModule;
   ADDREFIF(piModule);

   FormatTable_Ctor(pMe);
}

/* -----------------------------------------------------------------------
    Locale_New  
   --- */
static int Locale_New(ILocale **ppo, IShell *piShell, IModule *piModule)
{
   Locale *pMe;

   if(!ppo)
		return(EFAILED);

   pMe = MALLOCREC_VTBL(Locale, ILocale);
   if (!pMe) {
      return ENOMEMORY;
   }
   Locale_Ctor(pMe, GETVTBL(pMe,ILocale), piShell, piModule);
   *ppo = (ILocale*)(void*) pMe;
   return SUCCESS;
}

/*---------------------------------------------------------------------------------
      Local Routines
---------------------------------------------------------------------------------*/

/* -----------------------------------------------------------------------
    FormatTable_Ctor
    
    Set up the format table
   --- */
static void FormatTable_Ctor(Locale *pMe)
{
   MEMSET(pMe->gtFormats, 0, ILOCALE_FORMAT_LIMIT * sizeof(FormatTable));

   // these items are settable strings
   pMe->gtFormats[ILOCALE_CURRENCY_DECIMALSEPARATOR].pawData      = Currency_DecimalSeparator;
   pMe->gtFormats[ILOCALE_CURRENCY_GROUPSEPARATOR].pawData        = Currency_GroupSeparator; 
   pMe->gtFormats[ILOCALE_CURRENCY_NEGATIVEPATTERN].pawData       = Currency_NegativePattern;
   pMe->gtFormats[ILOCALE_CURRENCY_POSITIVEPATTERN].pawData       = Currency_PositivePattern;
   pMe->gtFormats[ILOCALE_CURRENCY_SYMBOL].pawData                = Currency_CurrencySymbol;
   pMe->gtFormats[ILOCALE_NUMBER_DECIMALSEPARATOR].pawData        = Number_DecimalSeparator;
   pMe->gtFormats[ILOCALE_NUMBER_GROUPSEPARATOR].pawData          = Number_GroupSeparator;
   pMe->gtFormats[ILOCALE_NUMBER_NEGATIVEPATTERN].pawData         = Number_NegativePattern;
   pMe->gtFormats[ILOCALE_PERCENT_DECIMALSEPARATOR].pawData       = Percent_DecimalSeparator;
   pMe->gtFormats[ILOCALE_PERCENT_GROUPSEPARATOR].pawData         = Percent_GroupSeparator;
   pMe->gtFormats[ILOCALE_PERCENT_NEGATIVEPATTERN].pawData        = Percent_NegativePattern;
   pMe->gtFormats[ILOCALE_PERCENT_POSITIVEPATTERN].pawData        = Percent_PositivePattern;
   pMe->gtFormats[ILOCALE_PERCENT_SYMBOL].pawData                 = Percent_PercentSymbol;
   pMe->gtFormats[ILOCALE_PERMILLE_SYMBOL].pawData                = Percent_PermilleSymbol;
   pMe->gtFormats[ILOCALE_NANSYMBOL].pawData                      = NANSymbol;
   pMe->gtFormats[ILOCALE_NEGATIVEINIFINITYSYMBOL].pawData        = NegativeInfinitySymbol;
   pMe->gtFormats[ILOCALE_NEGATIVESIGN].pawData                   = NegativeSign;
   pMe->gtFormats[ILOCALE_POSITIVEINIFINITYSYMBOL].pawData        = PositiveInfinitySymbol;
   pMe->gtFormats[ILOCALE_POSITIVESIGN].pawData                   = PositiveSign;
   pMe->gtFormats[ILOCALE_DATETIME_SHORTDAYNAMES].pawData         = DateTime_ShortDayNames;
   pMe->gtFormats[ILOCALE_DATETIME_SHORTMONTHNAMES].pawData       = DateTime_ShortMonthNames;
   pMe->gtFormats[ILOCALE_DATETIME_DAYNAMES].pawData              = DateTime_DayNames;
   pMe->gtFormats[ILOCALE_DATETIME_MONTHNAMES].pawData            = DateTime_MonthNames;
   pMe->gtFormats[ILOCALE_DATETIME_AMDESIGNATOR].pawData          = DateTime_AMDesignator;
   pMe->gtFormats[ILOCALE_DATETIME_DATESEPARATOR].pawData         = DateTime_DateSeparator;
   pMe->gtFormats[ILOCALE_DATETIME_PMDESIGNATOR].pawData          = DateTime_PMDesignator;
   pMe->gtFormats[ILOCALE_DATETIME_TIMESEPARATOR].pawData         = DateTime_TimeSeparator;
   pMe->gtFormats[ILOCALE_DATETIME_FULLDATELTIMEPATTERN].pawData  = DateTime_FullDateLongTimePattern;
   pMe->gtFormats[ILOCALE_DATETIME_FULLDATESTIMEPATTERN].pawData  = DateTime_FullDateShortTimePattern;
   pMe->gtFormats[ILOCALE_DATETIME_GENDATESHORTTIME].pawData      = DateTime_GenDateShortTimePattern;
   pMe->gtFormats[ILOCALE_DATETIME_GENDATELONGTIME].pawData       = DateTime_GenDateLongTimePattern;
   pMe->gtFormats[ILOCALE_DATETIME_LONGDATEPATTERN].pawData       = DateTime_LongDatePattern;
   pMe->gtFormats[ILOCALE_DATETIME_LONGTIMEPATTERN].pawData       = DateTime_LongTimePattern;
   pMe->gtFormats[ILOCALE_DATETIME_MONTHDAYPATTERN].pawData       = DateTime_MonthDayPattern;
   pMe->gtFormats[ILOCALE_DATETIME_SHORTDATEPATTERN].pawData      = DateTime_ShortDatePattern;
   pMe->gtFormats[ILOCALE_DATETIME_SHORTTIMEPATTERN].pawData      = DateTime_ShortTimePattern;
   pMe->gtFormats[ILOCALE_DATETIME_YEARMONTHPATTERN].pawData      = DateTime_YearMonthPattern;
   pMe->gtFormats[ILOCALE_STRING_HARDBREAK].pawData               = String_HardBreak;
   pMe->gtFormats[ILOCALE_STRING_SOFTBREAK].pawData               = String_SoftBreak;

   // these are integers
   pMe->gtFormats[ILOCALE_CURRENCY_DECIMALDIGITS].pawData         = (const AECHAR *)Currency_DecimalDigits;
   pMe->gtFormats[ILOCALE_CURRENCY_DECIMALDIGITS].cFlags          = FORMATTABLE_FLAG_INT;
   pMe->gtFormats[ILOCALE_CURRENCY_GROUPSIZES].pawData            = (const AECHAR *)Currenct_GroupSizes;
   pMe->gtFormats[ILOCALE_CURRENCY_GROUPSIZES].cFlags             = FORMATTABLE_FLAG_INT;
   pMe->gtFormats[ILOCALE_NUMBER_DECIMALDIGITS].pawData           = (const AECHAR *)Number_DecimalDigits;
   pMe->gtFormats[ILOCALE_NUMBER_DECIMALDIGITS].cFlags            = FORMATTABLE_FLAG_INT;
   pMe->gtFormats[ILOCALE_NUMBER_GROUPSIZES].pawData              = (const AECHAR *)Number_GroupSize;
   pMe->gtFormats[ILOCALE_NUMBER_GROUPSIZES].cFlags               = FORMATTABLE_FLAG_INT;
   pMe->gtFormats[ILOCALE_PERCENT_DECIMALDIGITS].pawData          = (const AECHAR *)Percent_DecimalDigits;
   pMe->gtFormats[ILOCALE_PERCENT_DECIMALDIGITS].cFlags           = FORMATTABLE_FLAG_INT;
   pMe->gtFormats[ILOCALE_PERCENT_GROUPSIZES].pawData             = (const AECHAR *)Percent_GroupSize;
   pMe->gtFormats[ILOCALE_PERCENT_GROUPSIZES].cFlags              = FORMATTABLE_FLAG_INT;
   pMe->gtFormats[ILOCALE_DATETIME_FIRSTWEEKDAY].pawData          = (const AECHAR *)DateTime_FirstWeekDay;
   pMe->gtFormats[ILOCALE_DATETIME_FIRSTWEEKDAY].cFlags           = FORMATTABLE_FLAG_INT;

   // these are constants
   pMe->gtFormats[ILOCALE_DATETIME_RFC1123PATTERN].pawData        = DateTime_RFC1123Pattern; 
   pMe->gtFormats[ILOCALE_DATETIME_RFC1123PATTERN].cFlags         = FORMATTABLE_FLAG_CONST;
   pMe->gtFormats[ILOCALE_DATETIME_SORTABLEDATETIME].pawData      = DateTime_SortableDatePattern;
   pMe->gtFormats[ILOCALE_DATETIME_SORTABLEDATETIME].cFlags       = FORMATTABLE_FLAG_CONST;
}

/* -----------------------------------------------------------------------
    FormatTable_Dtor
    
    Clean up the format table
   --- */
static void FormatTable_Dtor(Locale *pMe)
{
   int i;
   for (i = 0; i < ILOCALE_FORMAT_LIMIT; i++)
      if (pMe->gtFormats[i].cFlags & FORMATTABLE_FLAG_ALLOCATED)
         FREEIF(pMe->gtFormats[i].pawData);
}

/* -----------------------------------------------------------------------
    LocateFormatTableItem
    
    Given a parameter ID, return the index into the FormatTable containing
    the proper pItem.  Return -1 if not found.
   --- */
static int  LocateFormatTableItem(int16 nParmID)
{
   int i;
   for (i = 0; i < ILOCALE_FORMAT_LIMIT; i++) {
      if (gtParamToFormats[i] == nParmID)
         return (i);
   }
   return (-1);
}

#include "localebase.h"



/*---------------------------------------------------------------------------------
      ILocale
---------------------------------------------------------------------------------*/

// Local Routines
static void LocaleBase_FormatTableDtor(LocaleBase *pMe);
static void LocaleBase_FormatTableCtor(LocaleBase *pMe);
static int  LocaleBase_LocateFormatTableItem(int16 nParmID);


/* -----------------------------------------------------------------------
    Public function - see interface definition      
   --- */
uint32 LocaleBase_AddRef(ILocale* po)
{
   LocaleBase *pMe = (LocaleBase *)(void*)po;
   return ++pMe->nRefs;
}

/* -----------------------------------------------------------------------
    Public function - see interface definition      
   --- */
uint32 LocaleBase_Release(ILocale* po)
{
   LocaleBase *pMe = (LocaleBase *)(void*)po;

    if (--pMe->nRefs)
      return pMe->nRefs;

   LocaleBase_Dtor(pMe);

   FREE(pMe);
   return(0);
}

/* -----------------------------------------------------------------------
    Public function - see interface definition      
   --- */
int LocaleBase_QueryInterface(ILocale *po, AEECLSID id, void **ppo)
{
   LocaleBase *pMe = (LocaleBase *)(void*)po;

   if (id == AEEIID_LOCALE)    // place your classId here 
   {
      *ppo = pMe;
      IQI_AddRef(*ppo);
      return SUCCESS;
   } 
   else 
   {
      return ECLASSNOTSUPPORT;
   }
}
 
/* -----------------------------------------------------------------------
    Public function - see interface definition      
   --- */
int LocaleBase_SetParm(ILocale *po, int16 nParmID, int32 p1)
{
   LocaleBase *pMe = (LocaleBase *)(void*)po;
   int dwItem;
   AECHAR *pawNew = NULL;
   int dwNewLen;

   // is the caller asking to set a specific format pItem?
   dwItem = LocaleBase_LocateFormatTableItem(nParmID);
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
         if (dwNewLen) 
            pawNew = (AECHAR *)MALLOC((uint32)(dwNewLen + 1) * sizeof(AECHAR));

         if (pawNew) {

            // out with the old and in with the new ...
            WSTRLCPY(pawNew, (AECHAR *)p1, (uint32)(dwNewLen + 1));
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
int LocaleBase_GetParm(ILocale *po, int16 nParmID, int32 *pp1)
{
   int dwResult = EBADPARM;
   LocaleBase *pMe = (LocaleBase *)(void*)po;
   int dwItem;

   if (pp1) {

      *pp1 = 0;

      // see if the caller is asking for a formatting parameter
      dwItem = LocaleBase_LocateFormatTableItem(nParmID);
      if (dwItem != -1) {
         // return the proper format pItem
         *pp1 = (int32)pMe->gtFormats[dwItem].pawData;
         dwResult = SUCCESS;
      }
   }

   return (dwResult);
}

/* -----------------------------------------------------------------------
    Public function - see interface definition
    
    This routine can either build and return a format pattern to the
    caller, or actually perform the formatting.

    Return -1 on error.

    Return -1 but set the ppszFormatPattern parameter to return formatting
    only.

    Return number of bytes written (or needed) including NULL and fill
    the buffer with the formatted string to perform formatting here.

    Note, US English ILocale object simply returns formatting patterns
    and relies on caller to do the right thing with them.
   --- */
int32 LocaleBase_FormatItem(ILocale *po, AECHAR *bufOut, uint32 dwBufSize, AECHAR **ppszFormatPattern, const AECHAR *pszFormatString, int dwFormatStringLen, AEELocaleArg *pItem)
{
   PARAM_NOT_REF(bufOut)
   PARAM_NOT_REF(dwBufSize)

   int dwResult = -1;
   AECHAR aFormatCode[2];

   // sanity check
   if (pszFormatString && dwFormatStringLen) {

      // for now, assume pattern is a single character pattern   
      aFormatCode[0] = *pszFormatString;
      aFormatCode[1] = 0;

      // try to return an exact pattern
      if (ppszFormatPattern) {

         *ppszFormatPattern = NULL;

         // Determine what type of pattern to return
         switch (pItem->cType) {

            // Numeric pattern
            case AEELOCALEARGTYPE_SIGNEDDECIMAL:
            case AEELOCALEARGTYPE_UNSIGNEDDECIMAL:
         
               // numeric patterns are case insensitive
               WSTRLOWER(aFormatCode);

               switch (aFormatCode[0]) {
    
                  // request for the signed decimal pattern
                  case ILOCALE_FORMATCODE_NUMBER_NUMBER:
                     if ((pItem->cType == AEELOCALEARGTYPE_SIGNEDDECIMAL) && (pItem->u.d < 0))
                        (void) LocaleBase_GetParm(po, ILOCALE_NUMBER_NEGATIVEPATTERN, (int32 *)(void *)ppszFormatPattern);
                     break;

                  // request for the currency pattern
                  case ILOCALE_FORMATCODE_NUMBER_CURRENCY:
                     if ((pItem->cType == AEELOCALEARGTYPE_SIGNEDDECIMAL) && (pItem->u.d < 0))
                        (void) LocaleBase_GetParm(po, ILOCALE_CURRENCY_NEGATIVEPATTERN, (int32 *)(void *)ppszFormatPattern);
                     else
                        (void) LocaleBase_GetParm(po, ILOCALE_CURRENCY_POSITIVEPATTERN, (int32 *)(void *)ppszFormatPattern);
                     break;

                  // request for the percentage pattern
                  case ILOCALE_FORMATCODE_NUMBER_PERCENT:
                     if ((pItem->cType == AEELOCALEARGTYPE_SIGNEDDECIMAL) && (pItem->u.d < 0))
                        (void) LocaleBase_GetParm(po, ILOCALE_PERCENT_NEGATIVEPATTERN, (int32 *)(void *)ppszFormatPattern);
                     else
                        (void) LocaleBase_GetParm(po, ILOCALE_PERCENT_POSITIVEPATTERN, (int32 *)(void *)ppszFormatPattern);
                     break;

                  // unrecognized format code.  Punt.
                  default:
                     break;
               }
               break;

            // Date/Time pattern
            case AEELOCALEARGTYPE_JULIANDATE:
         
               switch (aFormatCode[0]) {
   
                  case ILOCALE_FORMATCODE_DATETIME_SHORTDATE:
                     (void) LocaleBase_GetParm(po, ILOCALE_DATETIME_SHORTDATEPATTERN, (int32 *)(void *)ppszFormatPattern);
                     break;
         
                  case ILOCALE_FORMATCODE_DATETIME_LONGDATE:
                     (void) LocaleBase_GetParm(po, ILOCALE_DATETIME_LONGDATEPATTERN, (int32 *)(void *)ppszFormatPattern);
                     break;
         
                  case ILOCALE_FORMATCODE_DATETIME_SHORTTIME:
                     (void) LocaleBase_GetParm(po, ILOCALE_DATETIME_SHORTTIMEPATTERN, (int32 *)(void *)ppszFormatPattern);
                     break;
         
                  case ILOCALE_FORMATCODE_DATETIME_LONGTIME:
                     (void) LocaleBase_GetParm(po, ILOCALE_DATETIME_LONGTIMEPATTERN, (int32 *)(void *)ppszFormatPattern);
                     break;
            
                  case ILOCALE_FORMATCODE_DATETIME_FULLDATESHORTTIME:
                     (void) LocaleBase_GetParm(po, ILOCALE_DATETIME_FULLDATESTIMEPATTERN, (int32 *)(void *)ppszFormatPattern);
                     break;
          
                  case ILOCALE_FORMATCODE_DATETIME_FULLDATELONGTIME:
                     (void) LocaleBase_GetParm(po, ILOCALE_DATETIME_FULLDATELTIMEPATTERN, (int32 *)(void *)ppszFormatPattern);
                     break;
            
                  case ILOCALE_FORMATCODE_DATETIME_GENDATESHORTTIME:
                     (void) LocaleBase_GetParm(po, ILOCALE_DATETIME_GENDATESHORTTIME, (int32 *)(void *)ppszFormatPattern);
                     break;
         
                  case ILOCALE_FORMATCODE_DATETIME_GENDATELONGTIME:
                     (void) LocaleBase_GetParm(po, ILOCALE_DATETIME_GENDATELONGTIME, (int32 *)(void *)ppszFormatPattern);
                     break;
            
                  case ILOCALE_FORMATCODE_DATETIME_MONTHDAY:
                  case ILOCALE_FORMATCODE_DATETIME_MONTHDAYALT:
                     (void) LocaleBase_GetParm(po, ILOCALE_DATETIME_MONTHDAYPATTERN, (int32 *)(void *)ppszFormatPattern);
                     break;
         
                  case ILOCALE_FORMATCODE_DATETIME_RFC1123:
                  case ILOCALE_FORMATCODE_DATETIME_RFC1121ALT:
                     (void) LocaleBase_GetParm(po, ILOCALE_DATETIME_RFC1123PATTERN, (int32 *)(void *)ppszFormatPattern);
                     break;
         
                  case ILOCALE_FORMATCODE_DATETIME_SORTABLE:
                     (void) LocaleBase_GetParm(po, ILOCALE_DATETIME_SORTABLEDATETIME, (int32 *)(void *)ppszFormatPattern);
                     break;
         
                  case ILOCALE_FORMATCODE_DATETIME_YEARMONTH:
                  case ILOCALE_FORMATCODE_DATETIME_YEARMONTHALT:
                     (void) LocaleBase_GetParm(po, ILOCALE_DATETIME_YEARMONTHPATTERN, (int32 *)(void *)ppszFormatPattern);
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
int LocaleBase_wstrncmp(ILocale *po, AECHAR *s1, AECHAR *s2, unsigned int length)
{
   PARAM_NOT_REF(po)
   // US English - as implemted by this ILocale - can use BREW's library function
   return (WSTRNCMP(s1, s2, length));
}

/* -----------------------------------------------------------------------
    Public function - see interface definition
    
    Note, if the language you're implementing requires a special
    implementation, this is where to put it.
   --- */
int LocaleBase_wstrcmp(ILocale *po, AECHAR *s1, AECHAR *s2)
{
   PARAM_NOT_REF(po)
   // US English - as implemted by this ILocale - can use BREW's library function
   return (WSTRCMP(s1, s2));
}

/* -----------------------------------------------------------------------
    Public function - see interface definition
    
    Note, if the language you're implementing requires a special
    implementation, this is where to put it.
   --- */
void LocaleBase_wstrupper(ILocale *po, AECHAR *pszDest)
{
   PARAM_NOT_REF(po)
   // US English - as implemted by this ILocale - can use BREW's library function
   WSTRUPPER(pszDest);
}

/* -----------------------------------------------------------------------
    Public function - see interface definition
    
    Note, if the language you're implementing requires a special
    implementation, this is where to put it.
   --- */
void LocaleBase_wstrlower(ILocale *po, AECHAR *pszDest)
{
   PARAM_NOT_REF(po)
   // US English - as implemted by this ILocale - can use BREW's library function
   WSTRLOWER(pszDest);
}

/* -----------------------------------------------------------------------
    Public function - see interface definition
    
    Note, if the language you're implementing requires a special
    implementation, this is where to put it.  
    
    For US English, lines are broken at the first hard break or last
    whitespace (soft break).
   --- */
AECHAR *LocaleBase_BreakLine(ILocale *po, AECHAR *pszText, AECHAR *pszClip, AECHAR *pszEnd)
{
   PARAM_NOT_REF(pszEnd)
   AECHAR *pszScan = pszText;
   AECHAR *pszBreak = pszClip;
   AECHAR *pszHardBreak, *pszWhiteSpace;
   int dwHardBreakLen, i;

   // get our breaking characters
   (void) LocaleBase_GetParm(po, ILOCALE_STRING_HARDBREAK, (int32 *)(void*)&pszHardBreak);
   dwHardBreakLen = pszHardBreak ? WSTRLEN(pszHardBreak) : 0;
   (void) LocaleBase_GetParm(po, ILOCALE_STRING_SOFTBREAK, (int32 *)(void*)&pszWhiteSpace);

   // Go through word, noting last break position     
   while (pszScan < pszClip) {

      // is this a hard break character sequence?
      for (i = 0; i < dwHardBreakLen; i++) {
         if (pszScan + i <= pszClip) {
            if (pszScan[i] != pszHardBreak[i]) 
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


/* -----------------------------------------------------------------------
    LocaleBase_Dtor
   --- */
void LocaleBase_Dtor(LocaleBase *pMe)
{
   LocaleBase_FormatTableDtor(pMe);

   RELEASEIF(pMe->piModule);
   RELEASEIF(pMe->piShell);
}

/* -----------------------------------------------------------------------
    LocaleBase_Ctor 
   --- */
void LocaleBase_Ctor(LocaleBase *pMe, AEEVTBL(ILocale) *pvt, IShell *piShell, IModule *piModule)
{
   pMe->pvt = pvt;

   pvt->AddRef          = LocaleBase_AddRef;
   pvt->Release         = LocaleBase_Release;
   pvt->QueryInterface  = LocaleBase_QueryInterface;
	pvt->SetParm         = LocaleBase_SetParm;
	pvt->GetParm         = LocaleBase_GetParm;
   pvt->FormatItem      = LocaleBase_FormatItem;
   pvt->wstrncmp        = LocaleBase_wstrncmp;
   pvt->wstrcmp         = LocaleBase_wstrcmp;
   pvt->wstrupper       = LocaleBase_wstrupper;
   pvt->wstrlower       = LocaleBase_wstrlower;
   pvt->breakline       = LocaleBase_BreakLine;

   pMe->nRefs = 1;

   pMe->piShell = piShell;
   ISHELL_AddRef(pMe->piShell);

   pMe->piModule = piModule;
   ADDREFIF(piModule);

   LocaleBase_FormatTableCtor(pMe);
}

/* -----------------------------------------------------------------------
    LocaleBase_New  
   --- */
int LocaleBase_New(ILocale **ppo, IShell *piShell, IModule *piModule)
{
   LocaleBase *pMe;

   if(!ppo)
		return(EFAILED);

   pMe = MALLOCREC_VTBL(LocaleBase, ILocale);
   if (!pMe) {
      return ENOMEMORY;
   }
   LocaleBase_Ctor(pMe,GETVTBL(pMe,ILocale),piShell,piModule); 
   *ppo = (ILocale*)(void*)pMe;
   return SUCCESS;
}

/*---------------------------------------------------------------------------------
      Local Routines
---------------------------------------------------------------------------------*/

/* -----------------------------------------------------------------------
    LocaleBase_FormatTableCtor
    
    Set up the format table
   --- */
static void LocaleBase_FormatTableCtor(LocaleBase *pMe)
{
   MEMSET(pMe->gtFormats, 0, ILOCALE_FORMAT_LIMIT * sizeof(FormatTable));   
}

/* -----------------------------------------------------------------------
    LocaleBase_FormatTableDtor
    
    Clean up the format table
   --- */
static void LocaleBase_FormatTableDtor(LocaleBase *pMe)
{
   int i;
   for (i = 0; i < ILOCALE_FORMAT_LIMIT; i++)
      if (pMe->gtFormats[i].cFlags & FORMATTABLE_FLAG_ALLOCATED)
         FREEIF(pMe->gtFormats[i].pawData);
}

/* -----------------------------------------------------------------------
    LocaleBase_LocateFormatTableItem
    
    Given a parameter ID, return the index into the FormatTable containing
    the proper pItem.  Return -1 if not found.
   --- */
static int LocaleBase_LocateFormatTableItem(int16 nParmID)
{
   int i;
   static const int gtParamToFormats[] = {
      ILOCALEPARM_CURRENCY_DECIMALDIGITS,    // parameter to format cell enum conversion.  This allows 
      ILOCALEPARM_CURRENCY_DECIMALSEPARATOR, // us to support only a subset of the available parameters.
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

   for (i = 0; i < ILOCALE_FORMAT_LIMIT; i++) {
      if (gtParamToFormats[i] == nParmID)
         return (i);
   }
   return (-1);
}

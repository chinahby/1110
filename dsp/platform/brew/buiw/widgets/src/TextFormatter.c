/*=================================================================================
FILE:          TextFormatter.c

SERVICES:      String composition interface

PUBLIC CLASSES:ITextFormatter

DESCRIPTION:   TThis file describes an interface of language dependent
                 data formatting routines.  These can be used to 
                 compose strings of text according to a specific
                 set of culture specific formatting rules.

                Copyright © 1999-2006 QUALCOMM Incorporated.
                           All Rights Reserved.
                        QUALCOMM Proprietary/GTDR
=================================================================================*/

/*---------------------------------------------------------------------------------
      Include Files
---------------------------------------------------------------------------------*/

#include "TextFormatter.h"
#include "DefaultLocale.h"
#include "AEEShell.h"
#include "wutil.h"

/*---------------------------------------------------------------------------------
      Type Declarations
---------------------------------------------------------------------------------*/

#define LANGUAGE_CODE_LEN  4           // length of language code
#define FORMATB_DEFAULT_SIZE  50       // default length of intermediate format buffer in bytes
#define LOCAL_NUMBER_BUFFER_SIZE 12    // size, in number of chars, of buffer for int->AECHAR conversions

// TextFormatter class
typedef struct 
{
   const AEEVTBL(ITextFormatter) *pvt;
   int  nRefs;
   IShell *piShell;
   IModule *piModule;

   ILocale *piLocale;                           // default ILocale object for formatting
   char     pszLanguage[LANGUAGE_CODE_LEN+1];   // four characters representing language
   uint16   dwYearOffset;                       // offset to add to years when formatting dates

   AECHAR  *pawFormatBuffer;                    // a scratch buffer for formatting
   int32    dwFormatBufferLen;                  // length of the format buffer in bytes
} TextFormatter;


// character used to separate argument types in the typestring
#define TYPESTRING_SEPARATOR_CHAR ';'

// a parsed format item
typedef struct
{
   uint32 dwArgIndex;                     // argument index
   uint32 dwAlignmentMin;                 // alignment min
   uint32 dwAlignmentMax;                 // alignment max
   const AECHAR *pszFormatCode;           // format code 
   const AECHAR *pszLangCode;             // language code
   int dwFormatCodeLen;                   // length of the format code
   int dwLangCodeLen;                     // length of language code
   boolean bLeftAligned;                  // left aligned field
} ParsedFormatItem;

// characters used in format items
// {index[,alignment][[;language]:formatString]}
#define FORMATITEM_START_CHAR             ((AECHAR)'{')
#define FORMATITEM_ALIGNMENT_START_CHAR   ((AECHAR)',')
#define FORMATITEM_LANGUAGE_START_CHAR    ((AECHAR)';')
#define FORMATITEM_FORMAT_START_CHAR      ((AECHAR)':')
#define FORMATITEM_END_CHAR               ((AECHAR)'}')

#define FORMATITEM_LEFTALIGNMENT_CHAR     ((AECHAR)'-')
#define FORMATITEM_MINMAXSEPARATOR_CHAR   ((AECHAR)'.')

#define FORMATTED_WHITESPACE_CHAR         ((AECHAR)' ')

// WBufBound class - to manage writing to a buffer safely
typedef struct WBufBound
{
   AECHAR *pcWrite;
   AECHAR *pcEnd;
} WBufBound;

#define WBufBound_ISCOUNTER(pbb)           ((AECHAR *)0 == (pbb)->pcEnd)
#define WBufBound_LEFT(pbb)                ((pbb)->pcEnd - (pbb)->pcWrite)
#define WBufBound_REALLYWROTE(pbb, pStart) (MIN((pbb)->pcEnd,(pbb)->pcWrite)-(pStart))
#define WBufBound_WROTE(pbb, pStart)       ((pbb)->pcWrite-(pStart))
#define WBufBound_FULL(pbb)                (WBufBound_LEFT(pbb) < 1)

#define BB_MEASURE      0x80000000
#define BB_WRITEALL     0x80000001

/*---------------------------------------------------------------------------------
      Globals
---------------------------------------------------------------------------------*/



/*---------------------------------------------------------------------------------
      Function Prototypes
---------------------------------------------------------------------------------*/

// ITextFormatter Interface
static uint32	         TextFormatter_AddRef(ITextFormatter* po);
static uint32	         TextFormatter_Release(ITextFormatter* po);
static int              TextFormatter_QueryInterface(ITextFormatter *pMe, AEECLSID id, void **ppo); 
static int              TextFormatter_SetParm(ITextFormatter *po, int16 nParmID, int32 p1);
static int              TextFormatter_GetParm(ITextFormatter *po, int16 nParmID, int32 *pp1);
static int              TextFormatter_FormatText(ITextFormatter *po, AECHAR *bufOut, uint32 *pdwBufSize, const AECHAR *pszFormatString, const char *pszTypeString, ...);    

// Local Routines
static void TextFormatter_Dtor(TextFormatter *pMe);
static int TextFormatter_Ctor(TextFormatter *pMe, AEEVTBL(ITextFormatter) *pvt, IShell *piShell, IModule *piModule);

static const AECHAR *ParseFormatItem(const AECHAR *pStart, ParsedFormatItem *pfi);
static int GetLocale(IShell *piShell, const char *pszLang, int dwLangLen, ILocale **ppObj);
static int32 FormatItem(TextFormatter *pMe, ILocale *pILocale, ParsedFormatItem *pfi, AEELocaleArg *pItem);
static int32 FormatDate(TextFormatter *pMe, ILocale *pILocale, const AECHAR *pawPattern, JulianType date);
static int32 FormatInteger(TextFormatter *pMe, ILocale *pILocale, const AECHAR *pawFormatCode, int dwCodeLen, const AECHAR *pawPattern, int dwValue, boolean bSigned);
static void  FormatNumber(TextFormatter *pMe, ILocale *pILocale, WBufBound *bbp, const AECHAR pawFormatCode, int dwValue, int dwDecimalPlaces, boolean bSigned);

// utility
static void GetNthArgumentFromList(const char *pszTypeString, int dwIndex, AEELocaleArg *arg, va_list args);
#define GetNthArgument(args, argsStart, pszTypeString, index, argPtr)         {  \
   va_start(args, argsStart);                                                    \
   GetNthArgumentFromList(pszTypeString, index, argPtr, args);                   \
   va_end(args);                                                                 \
}

// WBufBound
static void WBufBound_Init(WBufBound *pMe, AECHAR *pBuf, int nBytes);
static void WBufBound_NullTerminate(WBufBound *pMe);
static void WBufBound_Write(WBufBound *pMe, const AECHAR *pc, int nLen);
static void WBufBound_Putc(WBufBound *pMe, AECHAR c);
static void WBufBound_PutNc(WBufBound *pMe, AECHAR c, int32 nCount);
static void WBufBound_PutInt(WBufBound *pMe, int i, int dwPrec, boolean bSigned);
static void WBufBound_PutHex(WBufBound *pMe, int i, int dwPrec, boolean bUpper);
static int  WBufBound_PutILocaleString(WBufBound *pMe, ILocale *pILocale, uint16 dwParam);

// parser helpers
#define  ISDIGIT(c)           ( (unsigned) ((c) - ((AECHAR)'0')) < 10)
#define  MAKEDIGIT(c)         ( (uint32)((c) - ((AECHAR)'0')) )
#define  ISFORMATITEMFLAG(c)  ( (c) == FORMATITEM_START_CHAR ||  \
                                (c) == FORMATITEM_ALIGNMENT_START_CHAR ||  \
                                (c) == FORMATITEM_LANGUAGE_START_CHAR ||  \
                                (c) == FORMATITEM_FORMAT_START_CHAR ||  \
                                (c) == FORMATITEM_END_CHAR )

// Numeric
#define ISDECPATTERNCHAR(c)   (((c) == (AECHAR)'d') || ((c) == (AECHAR)'D'))
#define ISLHEXPATTERNCHAR(c)  ( (c) == (AECHAR)'x')
#define ISUHEXPATTERNCHAR(c)  ( (c) == (AECHAR)'X')
#define ISHEXPATTERNCHAR(c)   ( ISLHEXPATTERNCHAR(c) || ISUHEXPATTERNCHAR(c))

#define ISCURPATTERNCHAR(c)   (((c) == (AECHAR)'c') || ((c) == (AECHAR)'C'))
#define ISNUMPATTERNCHAR(c)   (((c) == (AECHAR)'n') || ((c) == (AECHAR)'N'))
#define ISPCTPATTERNCHAR(c)   (((c) == (AECHAR)'p') || ((c) == (AECHAR)'P'))
#define ISFXDPATTERNCHAR(c)   (((c) == (AECHAR)'f') || ((c) == (AECHAR)'F'))

#define ISDECIMALSEPCHAR(c)   ( (c) == (AECHAR)'.')
#define ISGROUPSEPCHAR(c)     ( (c) == (AECHAR)',')
#define ISNEGATIVECHAR(c)     ( (c) == (AECHAR)'-')
#define ISCURRENCYCHAR(c)     ( (c) == (AECHAR)'$')
#define ISPERCENTCHAR(c)      ( (c) == (AECHAR)'%')
#define ISNUMBERCHAR(c)       ( (c) == (AECHAR)'#')

// Date/Time
#define ISDAYPATTERNCHAR(c)   ( (c) == (AECHAR)'d' ) 
#define ISMONTHPATTERNCHAR(c) ( (c) == (AECHAR)'M' ) 
#define ISYEARPATTERNCHAR(c)  ( (c) == (AECHAR)'y' ) 
#define ISDATEPATTERNCHAR(c)  ( ISDAYPATTERNCHAR(c) || ISMONTHPATTERNCHAR(c) || ISYEARPATTERNCHAR(c) )

#define IS12HRPATTERNCHAR(c)  ( (c) == (AECHAR)'h' ) 
#define IS24HRPATTERNCHAR(c)  ( (c) == (AECHAR)'H' ) 
#define ISHOURPATTERNCHAR(c)  ( IS12HRPATTERNCHAR(c) || IS24HRPATTERNCHAR(c) ) 
#define ISMINUTEPATTERNCHAR(c)( (c) == (AECHAR)'m' )     
#define ISSECONDPATTERNCHAR(c)( (c) == (AECHAR)'s' )  
#define ISTIMEPATTERNCHAR(c)  ( ISHOURPATTERNCHAR(c) || ISMINUTEPATTERNCHAR(c) || ISSECONDPATTERNCHAR(c) ) 

#define ISAMPMPATTERNCHAR(c)  ( (c) == (AECHAR)'t' ) 
#define ISTSEPPATTERNCHAR(c)  ( (c) == (AECHAR)':' ) 
#define ISDSEPPATTERNCHAR(c)  ( (c) == (AECHAR)'/' )     
#define ISSTRINGPATTERNCHAR(c)( (c) == (AECHAR)'\'' )
#define ISESCAPEPATTERNCHAR(c)( (c) == (AECHAR)'\\' )  
       

// count the number of characters that appear in a row
static __inline int CountSame(const AECHAR *ps) 
{
   AECHAR c = *ps;
   int dwChars = 0;
   
   while (*ps == c) {
      dwChars++;
      ps++;
   }
   return dwChars;
}

// find the nth string in an array of strings, 1 based
static __inline  AECHAR *FindNthEntry(AECHAR *pIn, int dwIndex)
{
   AECHAR *pFound = pIn;

   while (dwIndex && --dwIndex) {
      pFound += WSTRLEN(pFound);
      pFound++;

      // end of string array?
      if (*pFound == (AECHAR)0)
         return (NULL);
   }

   return (pFound);
}


/*---------------------------------------------------------------------------------
      ITextFormatter
---------------------------------------------------------------------------------*/

/* -----------------------------------------------------------------------
    Public function - see interface definition      
   --- */
static uint32 TextFormatter_AddRef(ITextFormatter* po)
{
   TextFormatter *pMe = (TextFormatter *)po;
   return ++pMe->nRefs;
}

/* -----------------------------------------------------------------------
    Public function - see interface definition      
   --- */
static uint32 TextFormatter_Release(ITextFormatter* po)
{
   TextFormatter *pMe = (TextFormatter *)po;

    if (--pMe->nRefs)
      return pMe->nRefs;

   TextFormatter_Dtor(pMe);

   FREE(pMe);
   return(0);
}

/* -----------------------------------------------------------------------
    Public function - see interface definition      
   --- */
static int TextFormatter_QueryInterface(ITextFormatter *po, AEECLSID id, void **ppo)
{
   TextFormatter *pMe = (TextFormatter *)po;

   if (id == AEEIID_TEXTFORMATTER) {
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
static int TextFormatter_SetParm(ITextFormatter *po, int16 nParmID, int32 p1)
{
   TextFormatter *pMe = (TextFormatter *)po;
   ILocale *pil;
   int dwResult = SUCCESS;
   
   switch (nParmID) {

      // language
      case ITEXTFORMATTER_PARM_LANG:
         STRNCPY(pMe->pszLanguage, (const char *)p1, MIN(LANGUAGE_CODE_LEN,STRLEN((const char *)p1)));

         // find the ILocale object for this language
         dwResult = GetLocale(pMe->piShell, pMe->pszLanguage, STRLEN(pMe->pszLanguage), &pil);
         if (dwResult == SUCCESS) {
            // set our internal ILocale.
            RELEASEIF(pMe->piLocale);
            pMe->piLocale = pil;
         }
         break;

      // default locale object
      case ITEXTFORMATTER_PARM_ILOCALE:
         RELEASEIF(pMe->piLocale);
         pMe->piLocale = (ILocale *)p1;
         ILOCALE_AddRef(pMe->piLocale);
         break;
   
      // year offset
      case ITEXTFORMATTER_PARM_YEAROFFSET:
         pMe->dwYearOffset = (uint16)p1;
         break;

      // bad paramter
      default:
         dwResult = EBADPARM;
         break;
   }

   return (dwResult);
}

/* -----------------------------------------------------------------------
    Public function - see interface definition      
   --- */
static int TextFormatter_GetParm(ITextFormatter *po, int16 nParmID, int32 *pp1)
{
   TextFormatter *pMe = (TextFormatter *)po;

   if (!pp1)
      return (ENOMEMORY);

   switch (nParmID) {

      // language
      case ITEXTFORMATTER_PARM_LANG:
         *pp1 = (int32)pMe->pszLanguage;
         break;

      // default locale object
      case ITEXTFORMATTER_PARM_ILOCALE:
         *pp1 = (int32)pMe->piLocale;
         break;
   
      // year offset
      case ITEXTFORMATTER_PARM_YEAROFFSET:
         *pp1 = pMe->dwYearOffset;
         break;

      // bad paramter
      default:
         return(EBADPARM);
   }

   return (SUCCESS);
}

/* -----------------------------------------------------------------------
    Public function - see interface definition      
   --- */
static int TextFormatter_FormatText(ITextFormatter *po, AECHAR *bufOut, uint32 *pdwBufSize, const AECHAR *pszFormatString, const char *pszTypeString, ...)
{
   TextFormatter *pMe = (TextFormatter *)po;
   va_list  args;
   WBufBound bb; 
   AEELocaleArg arg;
   const AECHAR *strToken;                // Formatting tokenizer
   const AECHAR *strLast;                 // Iterator to keep track of non formatted chars
   ParsedFormatItem pfi;                  // parsed format item
   uint32 dwFmtLen;                       // length of formatted data
   ILocale *pILocale;                     // ILocale object to use for formatting
   boolean bIncomplete = FALSE;           // set to TRUE if we encounter something we can't format

   // sanity check
   if (!pdwBufSize || !pszFormatString || !pszTypeString)
      return (EBADPARM);

   // Setup.  This allows the buffer to be filled completely except
   // for the final character, which will be used for NULL termination.
   WBufBound_Init(&bb, bufOut, *pdwBufSize);

   // Tokenize ...
   for (strLast = strToken = pszFormatString; strToken && *strToken; strLast = strToken) {

      // find the next format item ...
      strToken = (const AECHAR *)WSTRCHR(strToken, FORMATITEM_START_CHAR);
      if (strToken == NULL)
         strToken = pszFormatString + WSTRLEN(pszFormatString);

      // Write everything before the first format item
      WBufBound_Write(&bb, strLast, strToken - strLast);

      if (*strToken) {
         // we're now at a format item start character.
      
         // if a format item start character is next, simply insert the character
         strToken++;
         if(*strToken == FORMATITEM_START_CHAR) {
            WBufBound_Putc(&bb,FORMATITEM_START_CHAR);
            strToken++;       // goto next char
            continue;
         }

         // otherwise, we're looking at a format item.  Parse it now.
         strToken = ParseFormatItem(strToken, &pfi);

         // was this a valid format item?
         if (strToken) {
            
            // find the correct argument and type.
            GetNthArgument(args, pszTypeString, pszTypeString, pfi.dwArgIndex, &arg);
            if (arg.cType) {
               
               // got the argument.  Apply any special offsets
               
               // year offset
               if (pMe->dwYearOffset && (arg.cType == AEELOCALEARGTYPE_JULIANDATE)) {
                  arg.u.j.wYear += pMe->dwYearOffset;
               }
                 
               // Next, locate the right ILocale object to perform the formatter ...
               if (pfi.pszLangCode && pfi.dwLangCodeLen) {
                  // the format item requested a specific language be used for this item ...
                  char pszLang[5];
                  WSTRTOSTR(pfi.pszLangCode, pszLang, 5);
                  GetLocale(pMe->piShell, pszLang, STRLEN(pszLang), &pILocale);
               } else {
                  // use our internal ILocale object to perform the formatting
                  pILocale = pMe->piLocale;
               }

               // Now do the formatting.  Rely on the ILocale for the formatting.  Fall back
               // on a few simple defaults as well.
               if (-1 != FormatItem(pMe, pILocale, &pfi, &arg)) {
    
                  // place the formatted data into our string.
                  dwFmtLen = WSTRLEN(pMe->pawFormatBuffer);

                  // take truncation into account
                  if (pfi.dwAlignmentMax && (pfi.dwAlignmentMax < dwFmtLen))
                     dwFmtLen = pfi.dwAlignmentMax;

                  // handle padding to the left
                  if (pfi.dwAlignmentMin && (dwFmtLen < pfi.dwAlignmentMin) && !pfi.bLeftAligned) {
                     WBufBound_PutNc(&bb, FORMATTED_WHITESPACE_CHAR, pfi.dwAlignmentMin-dwFmtLen);
                  }

                  WBufBound_Write(&bb,pMe->pawFormatBuffer,dwFmtLen);

                  // handle padding to the right
                  if (pfi.dwAlignmentMin && (dwFmtLen < pfi.dwAlignmentMin) && pfi.bLeftAligned) {
                     WBufBound_PutNc(&bb, FORMATTED_WHITESPACE_CHAR, pfi.dwAlignmentMin-dwFmtLen);
                  }

                  // clean up ILocale if necessary
                  if (pILocale != pMe->piLocale)
                     ILOCALE_Release(pILocale);

               } else {
                  // FormatItem failed.  We can't format this item, let's move on
                  bIncomplete = TRUE;
               }
            }
            else {
               // Unknown argument type.  We can't format this item, let's move on
               bIncomplete = TRUE;
            }
         } else {
            // no ILocale object was specified.  We can't format it, let's move on
             bIncomplete = TRUE;
         }
      }
   }

   // NULL terminate the buffer
   WBufBound_NullTerminate(&bb);

   // return number of BYTES needed to format this string, including the NULL character
   dwFmtLen = *pdwBufSize;
   *pdwBufSize = WBufBound_WROTE(&bb, bufOut) * sizeof(AECHAR);
   if (dwFmtLen < *pdwBufSize)
      return (ITEXTFORMATTER_PARTIALSUCCESS);      // buffer was too small
   else
      return (SUCCESS);
}

/*---------------------------------------------------------------------------------
      CreateInstance
---------------------------------------------------------------------------------*/

/* -----------------------------------------------------------------------
    TextFormatter_Dtor
   --- */
static void TextFormatter_Dtor(TextFormatter *pMe)
{
   FREEIF(pMe->pawFormatBuffer);

   RELEASEIF(pMe->piLocale);
   RELEASEIF(pMe->piModule);
   RELEASEIF(pMe->piShell);
}

/* -----------------------------------------------------------------------
    TextFormatter_Ctor 
   --- */
static int TextFormatter_Ctor(TextFormatter *pMe, AEEVTBL(ITextFormatter) *pvt, IShell *piShell, IModule *piModule)
{
   int dwResult;

   pMe->pvt = pvt;

   pvt->AddRef          = TextFormatter_AddRef;
   pvt->Release         = TextFormatter_Release;
   pvt->QueryInterface  = TextFormatter_QueryInterface;
	pvt->SetParm         = TextFormatter_SetParm;
	pvt->GetParm         = TextFormatter_GetParm;
   pvt->FormatText      = TextFormatter_FormatText;

   pMe->nRefs = 1;

   pMe->piShell = piShell;
   ISHELL_AddRef(pMe->piShell);

   pMe->piModule = piModule;
   ADDREFIF(piModule);

   // Set our internal Locale to the default locale ...
   dwResult = DefaultLocale_New(&pMe->piLocale, pMe->piShell, pMe->piModule); 
   if (SUCCESS == dwResult) {
      // set up a scratch buffer to perform intermediate formatting in
      pMe->dwFormatBufferLen = FORMATB_DEFAULT_SIZE;
      pMe->pawFormatBuffer = (AECHAR *)MALLOC(pMe->dwFormatBufferLen);
   }

   return (dwResult);
}

/* -----------------------------------------------------------------------
    TextFormatter_New  
   --- */
int TextFormatter_New(ITextFormatter **ppo, IShell *piShell, IModule *piModule)
{
   int dwResult;
   TextFormatter *pMe;

   if(!ppo)
		return(EFAILED);

   pMe = MALLOCREC_VTBL(TextFormatter, ITextFormatter);
   if (!pMe) {
      return ENOMEMORY;
   }
   dwResult = TextFormatter_Ctor(pMe, GETVTBL(pMe,ITextFormatter), piShell, piModule);
   *ppo = (ITextFormatter*) pMe;
   return (dwResult);
}


/*---------------------------------------------------------------------------------
      Local Routines
---------------------------------------------------------------------------------*/

/* -----------------------------------------------------------------------
    ParseFormatItem
    
    Parse a format item, return interesting bits to the caller.  Assume
    NULL termination.

    Format item looks like this:
    
    {index[,alignment][[;language]:formatString]}
   --- */
static const AECHAR *ParseFormatItem(const AECHAR *pScan, ParsedFormatItem *pfi)
{ 
   const AECHAR *pszFormatEnd = NULL;    // end of format item

   // sanity check
   if (!pScan || !*pScan || !pfi)
      return (NULL);

   // initialize
   MEMSET(pfi, 0, sizeof(ParsedFormatItem));

   //
   // Parse argument index
   //

   pfi->dwArgIndex = 0;
   while (ISDIGIT(*pScan)) {
      pfi->dwArgIndex = (pfi->dwArgIndex * 10) + MAKEDIGIT(*pScan);
      pScan++;
   }

   // skip to next format item flag
   while (*pScan && !ISFORMATITEMFLAG(*pScan))
      pScan++;

   //
   // parse alignment value
   //

   if (*pScan == FORMATITEM_ALIGNMENT_START_CHAR) {
      pScan++;

      // right aligned?
      if (*pScan == FORMATITEM_LEFTALIGNMENT_CHAR) {
         pfi->bLeftAligned = TRUE;
         pScan++;
      }

      // min
      pfi->dwAlignmentMin = 0;
      while (ISDIGIT(*pScan)) {
         pfi->dwAlignmentMin = (pfi->dwAlignmentMin * 10) + MAKEDIGIT(*pScan);
         pScan++;
      }

      // max
      if (*pScan == FORMATITEM_MINMAXSEPARATOR_CHAR) {
         pScan++;
         pfi->dwAlignmentMax = 0;
         while (ISDIGIT(*pScan)) {
            pfi->dwAlignmentMax = (pfi->dwAlignmentMax * 10) + MAKEDIGIT(*pScan);
            pScan++;
         }
      }

      // skip to next format item flag
      while (*pScan && !ISFORMATITEMFLAG(*pScan))
         pScan++;
   }

   //
   // parse language code
   //

   if (*pScan == FORMATITEM_LANGUAGE_START_CHAR) {
      pScan++;

      // everything between here and the next format item flag is the language
      pfi->pszLangCode = pScan;
      pfi->dwLangCodeLen = 0;
      while (*pScan && !ISFORMATITEMFLAG(*pScan)) {
         pfi->dwLangCodeLen++;
         pScan++;
      }
   }

   //
   // parse format code
   //

   if (*pScan == FORMATITEM_FORMAT_START_CHAR) {
      pScan++;
   
      // everything between here and the closing bracket is the format code
      pfi->pszFormatCode = pScan;
      pfi->dwFormatCodeLen = 0;
      while (*pScan && !ISFORMATITEMFLAG(*pScan)) {
         pfi->dwFormatCodeLen++;
         pScan++;
      }
   }

   // 
   // Make sure this was a valid format item
   //

   if (*pScan == FORMATITEM_END_CHAR) {
      // this was a valid format item.  
      // Return pointer to the next character
      pszFormatEnd = ++pScan;
   }

   return (pszFormatEnd);
}

/* -----------------------------------------------------------------------
    GetNthArgumentFromList
    
    Parse a typestring and a va_list for the nth argument.
   --- */
static void GetNthArgumentFromList(const char *pszTypeString, int dwIndex, AEELocaleArg *pArg, va_list args)
{
   const char *pScan = pszTypeString;
   char cType;

   if (pArg) {
      // clear arg
      MEMSET(pArg, 0, sizeof(AEELocaleArg));

      // find the type of the argument
      while (dwIndex-- && pScan && *pScan) {

         cType = *pScan;

         // iterate over the arguments until we reach the one we want
         switch (cType) {

            // int32
            case AEELOCALEARGTYPE_SIGNEDDECIMAL:
            case 'i':
               pArg->u.d = (int32) va_arg(args, int *);
               break;

            // uint32
            case AEELOCALEARGTYPE_UNSIGNEDDECIMAL:
               pArg->u.u = (uint32) va_arg(args, int *);
               break;
   
            // char
            case AEELOCALEARGTYPE_CHAR:
               pArg->u.c = va_arg(args, char);
               break;

            // AECHAR
            case AEELOCALEARGTYPE_AECHAR:
               pArg->u.C = va_arg(args, AECHAR);
               break;

            // char *
            case AEELOCALEARGTYPE_CHARPTR:
               pArg->u.s = va_arg(args, char *);
               break;

            // AECHAR *
            case AEELOCALEARGTYPE_AECHARPTR:
               pArg->u.S = va_arg(args, AECHAR *);
               break;

            // a pointer
            case AEELOCALEARGTYPE_POINTER:
               pArg->u.p = va_arg(args, void *);
               break;

            // a julian date structure
            case AEELOCALEARGTYPE_JULIANDATE:
               pArg->u.j = va_arg(args, JulianType);
               break;

            // a pointer to a julian date structure
            case 'J':
            {
               JulianType *pd = va_arg(args, JulianType *);
               if (pd) {
                  pArg->u.j = *pd;
               }
               cType = AEELOCALEARGTYPE_JULIANDATE;
               break;
            }
            
            default:
               // unknown argument type.  We can't proceed.
               pArg->cType = 0;
               return;
         }

         // return argument type
         pArg->cType = cType;

         // next argument type ...
         if (dwIndex) {
            pScan = STRCHREND(pScan, TYPESTRING_SEPARATOR_CHAR);
            if (pScan && *pScan)
               pScan++;
         }
      }
   }

   return;
}

/* -----------------------------------------------------------------------
    GetLocale
    
    Given a language code, create and return an instance of the proper
    ILocale object.
   --- */ 
static int GetLocale(IShell *piShell, const char *pszLang, int dwLangLen, ILocale **ppObj)
{
   int dwResult;
   AEECLSID clsId;
   char pszMimeString[20] = "text/plain;";

   if (!ppObj || (dwLangLen > 4))
      return (EBADPARM);

   *ppObj = NULL;

   // build the mimetype string
   STRNCPY((pszMimeString + STRLEN(pszMimeString)), pszLang, dwLangLen);

   // find the ILocale
   clsId = ISHELL_GetHandler(piShell, AEEIID_LOCALE, pszMimeString);
   if (clsId) {
      dwResult = ISHELL_CreateInstance(piShell, clsId, (void**)ppObj);
   }
   else 
      dwResult = ECLASSNOTSUPPORT;

   return (dwResult);
}

/* -----------------------------------------------------------------------
    FormatItem
    
    Format an item.  Rely on the ILocale object for formatting.  Fall
    back on some simple defaults if the ILocale doesn't handle them.

    Note: ILOCALE_FormatItem could be called twice, once for the length
    and once for the actual formatting.  This would save us the hassle
    of keeping and dealing with a scratch buffer here.  I opt for the
    scratch buffer though, in case some implementation of ILOCALE_FormatItem
    is excessively slow.
   --- */ 
static int32 FormatItem(TextFormatter *pMe, ILocale *pILocale, ParsedFormatItem *pfi, AEELocaleArg *pItem)
{
   int32 dwResult = -1;
   AECHAR *pawNewBuffer, *pawExactPattern = NULL;
   int dwLength;

   // first, ask the ILocale to format the item
   if (pILocale)
   {
      dwResult = ILOCALE_FormatItem(pILocale, pMe->pawFormatBuffer, pMe->dwFormatBufferLen, &pawExactPattern, pfi->pszFormatCode, pfi->dwFormatCodeLen, pItem);
      if (dwResult > pMe->dwFormatBufferLen) {
         // it worked, but the buffer wasn't big enough.  Try again with a bigger buffer
         pawNewBuffer = (AECHAR *)MALLOC((dwResult + 1) * sizeof(AECHAR));
         if (pawNewBuffer) {
            pMe->dwFormatBufferLen = (dwResult + 1) * sizeof(AECHAR);
            FREEIF(pMe->pawFormatBuffer);
            pMe->pawFormatBuffer = pawNewBuffer;
            dwResult = ILOCALE_FormatItem(pILocale, pMe->pawFormatBuffer, pMe->dwFormatBufferLen, NULL, pfi->pszFormatCode, pfi->dwFormatCodeLen, pItem);
         }
      }
   }
 
   // did the ILocale just provide the format pattern?
   if (pawExactPattern) {

      // maybe we can do something with it
      if (pItem->cType == AEELOCALEARGTYPE_JULIANDATE)
         dwResult = FormatDate(pMe, pILocale, pawExactPattern, pItem->u.j);
      else if (pItem->cType == AEELOCALEARGTYPE_UNSIGNEDDECIMAL)
         dwResult = FormatInteger(pMe, pILocale, pfi->pszFormatCode, pfi->dwFormatCodeLen, pawExactPattern, pItem->u.u, FALSE);
      else if (pItem->cType == AEELOCALEARGTYPE_SIGNEDDECIMAL)
         dwResult = FormatInteger(pMe, pILocale, pfi->pszFormatCode, pfi->dwFormatCodeLen, pawExactPattern, pItem->u.d, TRUE);


   } else if (dwResult == -1) {

      // ILocale neither formatted the item nor provided a format pattern.
      // Maybe it's something simple we can do ourselves?
      switch (pItem->cType) {

         // unsigned decimal
         case AEELOCALEARGTYPE_UNSIGNEDDECIMAL:
            dwResult = FormatInteger(pMe, pILocale, pfi->pszFormatCode, pfi->dwFormatCodeLen, NULL, pItem->u.u, FALSE);
            break;

         // singed decimal
         case AEELOCALEARGTYPE_SIGNEDDECIMAL:
            dwResult = FormatInteger(pMe, pILocale, pfi->pszFormatCode, pfi->dwFormatCodeLen, NULL, pItem->u.d, TRUE);
            break;

         // char
         case AEELOCALEARGTYPE_CHAR:
            pMe->pawFormatBuffer[0] = (AECHAR)pItem->u.c;
            pMe->pawFormatBuffer[1] = (AECHAR)0;
            dwResult = 1;
            break;

         // AECHAR
         case AEELOCALEARGTYPE_AECHAR:
            pMe->pawFormatBuffer[0] = pItem->u.C;
            pMe->pawFormatBuffer[1] = (AECHAR)0;
            dwResult = 1;
            break;

         // char *
         // AECHAR *
         case AEELOCALEARGTYPE_CHARPTR:
         case AEELOCALEARGTYPE_AECHARPTR:
            
            // how much room will we need for the string?
            if (pItem->cType == AEELOCALEARGTYPE_CHARPTR)
               dwLength = STRLEN(pItem->u.s) + 1;
            else
               dwLength = WSTRLEN(pItem->u.S) + 1;
            
            // is the scratch buffer big enough?
            if (pMe->dwFormatBufferLen < (int)(dwLength * sizeof(AECHAR))) {
               // nope.  Make a new one.
               pawNewBuffer = (AECHAR *)MALLOC(dwLength * sizeof(AECHAR));
               if (pawNewBuffer) {
                  pMe->dwFormatBufferLen = dwLength * sizeof(AECHAR);
                  FREEIF(pMe->pawFormatBuffer);
                  pMe->pawFormatBuffer = pawNewBuffer;
               }
            }

            // we have enough room now ...
            if (pMe->dwFormatBufferLen >= (int)(dwLength * sizeof(AECHAR))) {
               if (pItem->cType == AEELOCALEARGTYPE_CHARPTR)
                  STRTOWSTR(pItem->u.s, pMe->pawFormatBuffer, pMe->dwFormatBufferLen);
               else
                  WSTRCPY(pMe->pawFormatBuffer, pItem->u.S);
               dwResult = dwLength;
            }
            break;

         // a pointer of some kind.
         case AEELOCALEARGTYPE_POINTER:
         {
            AECHAR pszPointerFmt[] = { 'X','8',0 };
            dwResult = FormatInteger(pMe, pILocale, pszPointerFmt, 2, NULL, (int)pItem->u.p, FALSE);
            break;
         }

         // too complicated for us.  Punt.
         default:
            break;
      }

   }
   return (dwResult);
}

/* -----------------------------------------------------------------------
    FormatDate
    
    Format a JulianType according to a format pattern, place it into our
    internal format buffer
   --- */ 
static int32 FormatDate(TextFormatter *pMe, ILocale *pILocale, const AECHAR *pawPattern, JulianType date)
{
   int32 dwResult = -1;
   WBufBound bb;
   int dwWrote;
   const AECHAR *pfs;   // format string scan
   AECHAR *pls;         // locale string
   int dwVal;           // current data to format
   uint16 dwParm;       // current parameter to fetch from ILocale
   int dwNumChars;      // number of characters in a format pattern part

   // sanity check
   if (!pILocale || !pawPattern)
      return (dwResult);

   while (1) {
      // use a bufbound to ensure we don't overflow our internal buffer
      WBufBound_Init(&bb, pMe->pawFormatBuffer, pMe->dwFormatBufferLen);
   
      // do the formatting
      pfs = pawPattern;
      while (pfs && *pfs) {
         
         if (ISDATEPATTERNCHAR(*pfs)) {

            //
            // Date Fields
            //

             // days, months or years?
            if (ISDAYPATTERNCHAR(*pfs))
               dwVal = date.wDay;
            else if (ISMONTHPATTERNCHAR(*pfs))
               dwVal = date.wMonth;
            else 
               dwVal = date.wYear%100;
   
            dwParm = 0xffff;
            dwNumChars = CountSame(pfs);
            switch (dwNumChars) {

               // "xx" two digit number, with leading 0
               case 2:
                  if (dwVal <= 9)
                     WBufBound_Putc(&bb, (AECHAR)('0'));
                  // fall through

               // "x" two digit number, no leading 0
               case 1:
                  WBufBound_PutInt(&bb, dwVal, 0, FALSE);
                  break;

               // "xxx" Abbreviated name
               case 3:
                  if (ISDAYPATTERNCHAR(*pfs))
                     dwParm = ILOCALEPARM_DATETIME_SHORTDAYNAMES;
                  else
                     dwParm = ILOCALEPARM_DATETIME_SHORTMONTHNAMES;
                  // fall through

               // "xxxx" Full name or full four digit year
               default:
                  if (ISYEARPATTERNCHAR(*pfs)) {
                     // four digit year
                     WBufBound_PutInt(&bb, date.wYear, 0, FALSE);
                  } else {
                     // month or day name ...
                     if (dwParm == 0xffff)
                        dwParm = ISDAYPATTERNCHAR(*pfs) ? ILOCALEPARM_DATETIME_DAYNAMES : ILOCALEPARM_DATETIME_MONTHNAMES;
                     
                     // we need weekday
                     if (ISDAYPATTERNCHAR(*pfs))
                        dwVal = date.wWeekDay + 1;    // days of week range from 0 to 6

                     // place name into destination ...
                     if (SUCCESS == ILOCALE_GetParm(pILocale, dwParm, &pls)) {
                        pls = FindNthEntry(pls, dwVal);
                        if (pls)
                           WBufBound_Write(&bb, pls, WSTRLEN(pls));
                     }
                  }
                  break;
            }

            // skip over the pattern part we just processed
            pfs += dwNumChars;

         } else if (ISTIMEPATTERNCHAR(*pfs)) {

            // 
            // Time Fields
            //

            // hours, minutes or seconds?
            if (ISHOURPATTERNCHAR(*pfs))
               dwParm = date.wHour;
            else if (ISMINUTEPATTERNCHAR(*pfs))
               dwParm = date.wMinute;
            else 
               dwParm = date.wSecond;

            // 24 or 12 hour?
            if (IS12HRPATTERNCHAR(*pfs)) {
               if (date.wHour > 12)
                  dwParm -= 12;
               else if (0 == date.wHour)
                  dwParm = 12;
            }

            // how many digits?
            dwNumChars = CountSame(pfs);

            // handle leading 0
            if ((dwNumChars > 1) && (dwParm <= 9))
               WBufBound_Putc(&bb, (AECHAR)('0'));
            
            // place digits into our buffer ...
            WBufBound_PutInt(&bb, dwParm, 0, FALSE);
            
            // skip over formatting part
            pfs += dwNumChars;
            
         } else {

            //
            // Other
            //

            if (ISESCAPEPATTERNCHAR(*pfs)) {

               // Escape character.  Simply place next char into destination
               pfs++;
               if (*pfs)
                  WBufBound_Putc(&bb, *(pfs++));

            }
            else if (ISSTRINGPATTERNCHAR(*pfs)) {

               // string.  Place as is into destination
               while (pfs && *pfs && !ISSTRINGPATTERNCHAR(*(++pfs)))
                  WBufBound_Putc(&bb, *pfs);
               if (*pfs)
                  pfs++;

            } else {

               // determine which format string to add ...
               dwParm = 0xffff;
               if (ISAMPMPATTERNCHAR(*pfs) && (date.wHour >= 12))
                  dwParm = ILOCALEPARM_DATETIME_PMDESIGNATOR;
               else if (ISAMPMPATTERNCHAR(*pfs))
                  dwParm = ILOCALEPARM_DATETIME_AMDESIGNATOR;
               else if (ISTSEPPATTERNCHAR(*pfs))
                  dwParm = ILOCALEPARM_DATETIME_TIMESEPARATOR;
               else if (ISDSEPPATTERNCHAR(*pfs))
                  dwParm = ILOCALEPARM_DATETIME_DATESEPARATOR;

               // and do it ...
               if (dwParm != 0xffff) {

                  // how many characters in this format pattern part?
                  dwNumChars = CountSame(pfs);
                  if (SUCCESS == ILOCALE_GetParm(pILocale, dwParm, &pls))
                      WBufBound_Write(&bb, pls, MIN(dwNumChars, WSTRLEN(pls)));
                  pfs += dwNumChars;

               } else {
                  // put the character as is into the destination
                  WBufBound_Putc(&bb, *pfs);
                  pfs++;
               }       
            }
         } 
      }

      // NULL terminate this.
      WBufBound_Putc(&bb, 0);

      // how many bytes did we actually write?
      dwWrote = (1 + WBufBound_WROTE(&bb, pMe->pawFormatBuffer)) * sizeof(AECHAR);

      // return the number of bytes written if we wrote successfully
      if (dwWrote <= pMe->dwFormatBufferLen)
         return(dwWrote);
      else {
         // otherwise, reallocate our buffer and do it again.
         AECHAR *pawNewBuffer = (AECHAR *)MALLOC(dwWrote);
         if (pawNewBuffer) {
            pMe->dwFormatBufferLen = dwWrote;
            FREEIF(pMe->pawFormatBuffer);
            pMe->pawFormatBuffer = pawNewBuffer;
         } else {
            // no memory.  Stop.
            break;
         }
      }
   }

   return (dwResult);
}

/* -----------------------------------------------------------------------
    FormatInteger
    
    Format a decimal according to an exact format pattern.  Pay attention
    to sign.  Place result into our internal buffer
   --- */ 
static int32 FormatInteger(TextFormatter *pMe, ILocale *pILocale, const AECHAR *pawFormatCode, int dwCodeLen, const AECHAR *pawPattern, int dwValue, boolean bSigned)
{
   int32 dwResult = -1;
   const AECHAR *pfs;       // format scan
   int dwPrec = 0;          // precision
   boolean bPrec = FALSE;   // a precision was specified
   WBufBound bb;
   int dwWrote;
   
   // sanity check
   if (!pMe || !pILocale || !pawFormatCode || !dwCodeLen)
      return (dwResult);

   // first see if this is a format code we understand
   if ((ISDECPATTERNCHAR(*pawFormatCode) || ISNUMPATTERNCHAR(*pawFormatCode) || ISHEXPATTERNCHAR(*pawFormatCode)) || ISFXDPATTERNCHAR(*pawFormatCode)
      || (pawPattern != NULL)) {
   
      // is a precision specified?
      pfs = pawFormatCode;
      while (++pfs < (pawFormatCode + dwCodeLen)) {
         bPrec = TRUE;
         dwPrec = (dwPrec * 10) + (*pfs - '0');
      }

      // perform the formatting
      while (1) {

         // use a bufbound to ensure we don't overflow our internal buffer
         WBufBound_Init(&bb, pMe->pawFormatBuffer, pMe->dwFormatBufferLen);

         if (pawPattern) {
            
            //
            // Exact Pattern Specified
            //

            pfs = pawPattern;
            while (pfs && *pfs) {

               if (ISNEGATIVECHAR(*pfs)) {

                  // negative symbol.  Do we need it?
                  if (bSigned && ((int32)dwValue < 0))
                     WBufBound_PutILocaleString(&bb, pILocale, ILOCALEPARM_NEGATIVESIGN);

               } else if (ISDECIMALSEPCHAR(*pfs)) {

                  // Decimal Separator
                  if (ISCURPATTERNCHAR(*pawFormatCode))
                     WBufBound_PutILocaleString(&bb, pILocale, ILOCALEPARM_CURRENCY_DECIMALSEPARATOR);
                  else if (ISPCTPATTERNCHAR(*pawFormatCode))
                     WBufBound_PutILocaleString(&bb, pILocale, ILOCALEPARM_PERCENT_DECIMALSEPARATOR);
                  else
                     WBufBound_PutILocaleString(&bb, pILocale, ILOCALEPARM_NUMBER_DECIMALSEPARATOR);

               } else if (ISGROUPSEPCHAR(*pfs)) {

                  // Group Separator
                  if (ISCURPATTERNCHAR(*pawFormatCode))
                     WBufBound_PutILocaleString(&bb, pILocale, ILOCALEPARM_CURRENCY_GROUPSEPARATOR);
                  else if (ISPCTPATTERNCHAR(*pawFormatCode))
                     WBufBound_PutILocaleString(&bb, pILocale, ILOCALEPARM_PERCENT_GROUPSEPARATOR);
                  else
                     WBufBound_PutILocaleString(&bb, pILocale, ILOCALEPARM_NUMBER_GROUPSEPARATOR);


               } else if (ISCURRENCYCHAR(*pfs) && ISCURPATTERNCHAR(*pawFormatCode)) {

                  // Currency Symbol
                  WBufBound_PutILocaleString(&bb, pILocale, ILOCALEPARM_CURRENCY_SYMBOL);

               } else if (ISPERCENTCHAR(*pfs) && ISPCTPATTERNCHAR(*pawFormatCode)) {

                  // Percent Symbol
                  WBufBound_PutILocaleString(&bb, pILocale, ILOCALEPARM_PERCENT_SYMBOL);

               } else if (ISNUMBERCHAR(*pfs)) {

                  // A formatted number.  Right now, a single # means format the number with
                  // group and decimal separators
                  FormatNumber(pMe, pILocale, &bb, *pawFormatCode, dwValue, bPrec ? dwPrec : -1, bSigned);
                   
               } else if (ISESCAPEPATTERNCHAR(*pfs)) {

                  // Escape character.  Simply place next char into destination
                  pfs++;
                  if (*pfs)
                     WBufBound_Putc(&bb, *(pfs++));

               } else {

                  // unrecognized character, place as is into result
                  WBufBound_Putc(&bb, *pfs);
               }     

               // next character
               pfs++;
            }
         } else if (ISDECPATTERNCHAR(*pawFormatCode)) {

            // 
            // Simple signed/unsiged decimal
            //

            // do we need a negative sign?
            if (bSigned && ((int32)dwValue < 0))
               WBufBound_PutILocaleString(&bb, pILocale, ILOCALEPARM_NEGATIVESIGN);
            
            // place digits into our buffer ...
            WBufBound_PutInt(&bb, dwValue, dwPrec, bSigned);

         } else if (ISHEXPATTERNCHAR(*pawFormatCode)) {
         
            //
            // Simple hex number
            //

            WBufBound_PutHex(&bb, dwValue, dwPrec, (boolean)ISUHEXPATTERNCHAR(*pawFormatCode));
         } else {

            //
            // Number, no pattern specified. 
            //
            
            // negative symbol?
            if (bSigned && ((int32)dwValue < 0))
               WBufBound_PutILocaleString(&bb, pILocale, ILOCALEPARM_NEGATIVESIGN);

            FormatNumber(pMe, pILocale, &bb, *pawFormatCode, dwValue, bPrec ? dwPrec : -1, bSigned);
         }

         // NULL terminate this.
         WBufBound_Putc(&bb, 0);

         // how many bytes did we actually write?
         dwWrote = (1 + WBufBound_WROTE(&bb, pMe->pawFormatBuffer)) * sizeof(AECHAR);

         // return the number of bytes written if we wrote successfully
         if (dwWrote <= pMe->dwFormatBufferLen)
            return(dwWrote);
         else {
            // otherwise, reallocate our buffer and do it again.
            AECHAR *pawNewBuffer = (AECHAR *)MALLOC(dwWrote);
            if (pawNewBuffer) {
               pMe->dwFormatBufferLen = dwWrote;
               FREEIF(pMe->pawFormatBuffer);
               pMe->pawFormatBuffer = pawNewBuffer;
            } else {
               // no memory.  Stop.
               break;
            }
         }
      }
   }

   return (dwResult);
}

/* -----------------------------------------------------------------------
    FormatNumber
    
    Given an integer, format it as a number according to the ILocale's
    settings.  Ignore sign.  Pass -1 in for dwDecimalPlaces to use
    default precision.
   --- */
static void FormatNumber(TextFormatter *pMe, ILocale *pILocale, WBufBound *pbb, const AECHAR pawFormatCode, int dwValue, int dwDecimalPlaces, boolean bSigned)
{
   int dwGroupSize;
   const AECHAR *pawDecimalSep, *pawGroupSep;
   AECHAR awNum[LOCAL_NUMBER_BUFFER_SIZE];
   WBufBound nbb;
   int i;
   AECHAR *pawScan;
   int dwLen;

   // get settings from ILocale.  These will depending on what type of number we're formatting.
   if (ISCURPATTERNCHAR(pawFormatCode)) {
      ILOCALE_GetCurrencyGroupSizes(pILocale, &dwGroupSize);
      ILOCALE_GetCurrencyDecimalSeparator(pILocale, &pawDecimalSep);
      ILOCALE_GetCurrencyGroupSeparator(pILocale, &pawGroupSep);
      if (dwDecimalPlaces == -1) 
         ILOCALE_GetCurrencyDecimalDigits(pILocale, &dwDecimalPlaces);
   } else if (ISPCTPATTERNCHAR(pawFormatCode)) {
      ILOCALE_GetPercentGroupSizes(pILocale, &dwGroupSize);
      ILOCALE_GetPercentDecimalSeparator(pILocale, &pawDecimalSep);
      ILOCALE_GetPercentGroupSeparator(pILocale, &pawGroupSep);
      if (dwDecimalPlaces == -1) 
         ILOCALE_GetPercentDecimalDigits(pILocale, &dwDecimalPlaces);
   } else {
      ILOCALE_GetNumberGroupSizes(pILocale, &dwGroupSize);
      ILOCALE_GetNumberDecimalSeparator(pILocale, &pawDecimalSep);
      ILOCALE_GetNumberGroupSeparator(pILocale, &pawGroupSep);
      if (dwDecimalPlaces == -1) 
         ILOCALE_GetNumberDecimalDigits(pILocale, &dwDecimalPlaces);
   }

   // if we're formatting a fixed point number, punt on group separation
   if (ISFXDPATTERNCHAR(pawFormatCode))
      pawGroupSep = NULL;

   // next, convert the number to a string of AECHARs.
   // note, this truncates if the number is larger than the awNum buffer,
   // so change this if we start supporting something bigger than an int32
   MEMSET(awNum, 0, LOCAL_NUMBER_BUFFER_SIZE * sizeof(AECHAR));
   WBufBound_Init(&nbb, awNum, LOCAL_NUMBER_BUFFER_SIZE * sizeof(AECHAR));
   WBufBound_PutInt(&nbb, dwValue, 0, bSigned);
   dwLen = WSTRLEN(awNum);

   // do digits with group separators
   pawScan = awNum;
   for (i = dwLen; (pawScan < (awNum + dwLen)) && (i > dwDecimalPlaces); i--) {

      // group separator?
      if (pawGroupSep && (i != dwLen) && (((i-dwDecimalPlaces)%dwGroupSize))==0)
         WBufBound_Write(pbb, pawGroupSep, WSTRLEN(pawGroupSep));

      // place the digit
      WBufBound_Putc(pbb, *pawScan);

      // next digit
      pawScan++;
   }

   // decial separator, pad with 0 if necessary
   if (dwDecimalPlaces && pawDecimalSep) {
      if (i < dwDecimalPlaces)
         WBufBound_PutNc(pbb, (AECHAR)'0', dwDecimalPlaces - i);
      WBufBound_Write(pbb, pawDecimalSep, WSTRLEN(pawDecimalSep));
   }

   // do decimal digits
   while (pawScan < (awNum + dwLen)) {
      WBufBound_Putc(pbb, *pawScan);
      pawScan++;
   }
}

/*---------------------------------------------------------------------------------
      WBufBound - an object to insure a buffer is written to but not overflowed
---------------------------------------------------------------------------------*/

/* -----------------------------------------------------------------------
    WBufBound_Init
    
    Prepare a WBufBound for writing.  Leave room for final NULL character.
   --- */
static void WBufBound_Init(WBufBound *pMe, AECHAR *pBuf, int nBytes)
{
   pMe->pcWrite = pBuf; 
   pMe->pcEnd   = ((AECHAR *)0 != pBuf) ? pBuf + ((nBytes-2)/sizeof(AECHAR)) : 0;
}

/* -----------------------------------------------------------------------
    WBufBound_NullTerminate
    
    NULL-terminate a buf bound.  We know pcEnd points to the final
    character in the buffer.
   --- */
static void WBufBound_NullTerminate(WBufBound *pMe)
{
   AECHAR *pWriteEnd = MIN(pMe->pcWrite, pMe->pcEnd);

   if (pWriteEnd != NULL) {
      *pWriteEnd = (AECHAR)0;
   }

   pMe->pcWrite++;
}

/* -----------------------------------------------------------------------
    WBufBound_Write
    
    Write nLen characters of pc to a WBufBound
   --- */
static void WBufBound_Write(WBufBound *pMe, const AECHAR *pc, int nLen)
{
   AECHAR *pcStop = MIN(pMe->pcEnd, pMe->pcWrite + nLen);
   AECHAR *pcWrite = pMe->pcWrite;

   while (pcWrite < pcStop) {
      *pcWrite++ = *pc++;
   }

   pMe->pcWrite += nLen;
}

/* -----------------------------------------------------------------------
    WBufBound_Putc
    
    Add a character to a WBufBound
   --- */
static void WBufBound_Putc(WBufBound *pMe, AECHAR c)
{
   if (pMe->pcWrite < pMe->pcEnd) {
      *pMe->pcWrite = c;
   }
   pMe->pcWrite++;
}


/* -----------------------------------------------------------------------
    WBufBound_PutNc
    
    Add n characters to a WBufBound
   --- */
static void WBufBound_PutNc(WBufBound *pMe, AECHAR c, int32 nCount)
{
   int32 nNum  = MAX(nCount, 0);
   AECHAR *pcStop = MIN(pMe->pcEnd, pMe->pcWrite + nNum);
   AECHAR *pcWrite = pMe->pcWrite;

   while (pcWrite < pcStop) {
      *pcWrite++ = c;
   }

   pMe->pcWrite += nNum;
}

/* -----------------------------------------------------------------------
    WBufBound_PutInt
    
    Add an integer to a buf bound.  Don't include the sign
   --- */
static void WBufBound_PutInt(WBufBound *pMe, int i, int dwPrec, boolean bSigned)
{
   AECHAR awNum[LOCAL_NUMBER_BUFFER_SIZE];
   int j;

   // initialize
   MEMSET(awNum, 0, LOCAL_NUMBER_BUFFER_SIZE * sizeof(AECHAR));

   if (bSigned) {
      // signed number.  Don't include the sign.
      if (i < 0)
         i *= -1;
      
      // convert to a string
      WWRITELONG(awNum, i);

      // add leading 0s
      j = WSTRLEN(awNum);
      if (dwPrec && (dwPrec > j))
         WBufBound_PutNc(pMe, (AECHAR)'0', dwPrec - j);

      // add it to ourselves
      WBufBound_Write(pMe, awNum, j);
   } else {
      // unsigned number.  Convert to a string
      for (j = 0; j < LOCAL_NUMBER_BUFFER_SIZE; j++) {
         awNum[j] = (AECHAR)'0' + ((unsigned)i)%10;
         i = (unsigned)i/10;
         if (i == 0)
            break;
      }

      // add leading 0s
      j = WSTRLEN(awNum);
      if (dwPrec && (dwPrec > j))
         WBufBound_PutNc(pMe, (AECHAR)'0', dwPrec - j);

      // add it to ourselves
      for (; j > 0; j--)
         WBufBound_Putc(pMe, awNum[j-1]);
   }
}

/* -----------------------------------------------------------------------
    WBufBound_PutInt
    
    Add an integer to a buf bound, interpret it as a hexadecimal number
   --- */
static void WBufBound_PutHex(WBufBound *pMe, int i, int dwPrec, boolean bUpper)
{
   int c;
   int dwDigit;
   AECHAR awDigit;

   // how many hex digits are we dealing with?
   for (c = 0; c <= 7; c++) {
      if ((unsigned)i < (unsigned)(1 << (c*4)))
         break;
   }

   // add leading 0s
   if (dwPrec && (dwPrec > c))
      WBufBound_PutNc(pMe, (AECHAR)'0', dwPrec - c);

   // convert number
   for (c--; c >= 0; c--) {
      dwDigit = i & (0xF << (c*4));
      dwDigit = (dwDigit >> (c*4)) & 0xF;

      if (dwDigit >= 0 && dwDigit <= 9)
         awDigit = (AECHAR)'0' + dwDigit;
      else
         awDigit = (bUpper ? (AECHAR)'A' : (AECHAR)'a') + dwDigit - 10;

      WBufBound_Putc(pMe, awDigit);
   }
}

/* -----------------------------------------------------------------------
    WBufBound_PutILocaleString
    
    Add a string from the ILocale to ourselves
   --- */
static int WBufBound_PutILocaleString(WBufBound *pMe, ILocale *pILocale, uint16 dwParam)
{
   int dwLen = 0;
   AECHAR *pls = NULL;

   if (pILocale) {
      if (SUCCESS == ILOCALE_GetParm(pILocale, dwParam, &pls))
         if (pls) {
            dwLen = WSTRLEN(pls);
            WBufBound_Write(pMe, pls, dwLen);
         }
   }

   return (dwLen);
}
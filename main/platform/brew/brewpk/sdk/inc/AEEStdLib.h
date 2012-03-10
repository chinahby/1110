#ifndef AEESTDLIB_H
#define AEESTDLIB_H
/*======================================================
FILE:  AEEStdLib.h

SERVICES:  Helper file to replace standard library calls

GENERAL DESCRIPTION:
        Base level definitions, typedefs, etc. for AEE

Copyright © 1999-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=====================================================*/

#include <stddef.h>     // size_t, etc.

#include "AEEOldVaList.h"
#include "AEEComdef.h"
#include "AEE.h"

/* if dbg malloc is already defined, use its value, otherwise set it
   based on AEE_SIMULATOR 
*/
#if !defined(AEE_DBG_HEAP)
# if defined(AEE_SIMULATOR)
#define AEE_DBG_HEAP    1
# else /* if defined(AEE_SIMULATOR) */
#define AEE_DBG_HEAP    0
# endif /* if defined(AEE_SIMULATOR) */
#endif /* if !defined(AEE_DBG_HEAP) */

// Flags for GETAEEVERSION

#define GAV_LATIN1   0x0001      // Single byte string
#define GAV_MSM      0x0002      // N.N.N.N (MSMNNNN)
#define GAV_UPDATE   0x0004      // Update version (patch) info
#define GAV_INTERNAL 0x0008      // internal use.

#define AEESTDLIB_VER 0x00010000

typedef int (*PFNQSORTCOMPARE)(const void *s1, const void *s2);

typedef struct AEEHelperFuncs AEEHelperFuncs;

struct AEEHelperFuncs
{
   void          *(*memmove)(void *pd, const void *ps, size_t s);
   void          *(*memset)(void *pd, int c, size_t s);

   // Standard String Functions...
   char          *(*strcpy)(char *dst, const char *src);
   char          *(*strcat)(char *dst, const char *src);
   int            (*strcmp)(const char *s1, const char *s2);
   size_t         (*strlen)(const char *s);
   char          *(*strchr)(const char *s1, int ch);
   char          *(*strrchr)(const char *s1, int ch);
   int            (*sprintf)(char *pszDest,const char *pszFormat, ...);

   // Wide String Functions...

   AECHAR        *(*wstrcpy)(AECHAR *pDest, const AECHAR *pSrc);
   AECHAR        *(*wstrcat)(AECHAR *pDest, const AECHAR *pSrc);
   int            (*wstrcmp)(const AECHAR *s1, const AECHAR *s2);
   int            (*wstrlen)(const AECHAR *p);
   AECHAR        *(*wstrchr)(const AECHAR *s1, AECHAR ch);
   AECHAR        *(*wstrrchr)(const AECHAR *s1, AECHAR ch);
   void           (*wsprintf)(AECHAR *pDest, int nSize, const AECHAR *pFormat,
                              ...);

   // String Conversions...

   AECHAR         *(*strtowstr)(const char *pszIn, AECHAR *pDest, int nSize);
   char           *(*wstrtostr)(const AECHAR *pIn, char *pszDest, int nSize);
   double          (*wstrtofloat)(const AECHAR *pszNum);
   boolean         (*floattowstr)(double val, AECHAR *psz, int nSize);
   boolean         (*utf8towstr)(const byte *pszIn, int nLen, AECHAR *pszDest,
                                 int nSizeDestBytes);
   boolean         (*wstrtoutf8)(const AECHAR *pszIn, int nLen, byte *pszDest, 
                                 int nSizeDestBytes);
   void            (*wstrlower)(AECHAR *pszDest);
   void            (*wstrupper)(AECHAR *pszDest);
   TChType         (*chartype)(AECHAR ch);

   // Bitmap Conversions...

   void           *(*SetupNativeImage)(AEECLSID cls, void *pBuffer, 
                                       AEEImageInfo *pii, boolean *pbRealloc);

   // Memory allocation routines
   
   void          *(*malloc)(uint32 dwSize);
   void           (*free)(void *pObj);
   AECHAR        *(*wstrdup)(const AECHAR *pIn);
   void          *(*realloc)(void *pSrc, uint32 dwSize);
   
   // Additional Wide String Helpers

   AECHAR        *(*wwritelongex)(AECHAR *pszBuf, long n, int nPad, 
                                  int *pnRemaining);
   int            (*wstrsize)(const AECHAR *p);
   int            (*wstrncopyn)(AECHAR *pszDest, int cbDest, 
                                const AECHAR *pszSource, int lenSource);
   
   // OEM String Routines

   int            (*OEMStrLen)(const byte *p);
   int            (*OEMStrSize)(const byte *p);

   // AEE/BREW version, See above for flags

   uint32         (*GetAEEVersion)(byte *pszFormatted, int nSize, 
                                   uint16 wFlags);  

   // ATOI
   
   int            (*atoi)(const char *psz);

   // Floating point - covers issues related to floating point usage across 
   //  ARM/THUMB

   double         (*f_op)(double v1, double v2,int nType);
   boolean        (*f_cmp)(double v1, double v2,int nType);

   // Debug 

   void           (*dbgprintf)(const char *psz,...);
   void           (*wstrcompress)(const AECHAR *pIn, int nChars, byte *pDest,int nSize);

   // time

   int32          (*LocalTimeOffset)(boolean *pbDaylightSavings);
   void           (*GetRand)(byte *pDest, int nSize);
   uint32         (*GetTimeMS)(void); 
   uint32         (*GetUpTimeMS)(void);
   uint32         (*GetSeconds)(void);
   void           (*GetJulianDate)(uint32 dwSecs, JulianType *pDate);

   void           (*sysfree)(void *pb);

   // Applet helper function - current applet

   IApplet       *(*GetAppInstance)(void);

   uint32         (*strtoul)(const char *nptr, char **endptr, int base );
   char          *(*strncpy)(char *strDest, const char *strSource,size_t count );
   int            (*strncmp)(const char *a, const char *b, size_t length );
   int            (*stricmp)(const char *a, const char *b);
   int            (*strnicmp)(const char *a, const char *b, size_t length);
   char          *(*strstr)(const char *haystack, const char *needle);
   int            (*memcmp)(const void *a, const void *b, size_t length );
   void          *(*memchr)(const void *src, int c, size_t length );
   void           (*strexpand)(const byte *pSrc, int nCount, AECHAR *pDest,int nSize);
   char          *(*stristr)(const char *cpszHaystack, const char *cpszNeedle);
   char          *(*memstr)(const char *cpHaystack, const char *cpszNeedle, size_t nHaystackLen);
   int            (*wstrncmp)(const AECHAR *s1, const AECHAR *s2, size_t nLen);
   char          *(*strdup)(const char *psz);
   boolean        (*strbegins)(const char *cpszPrefix, const char *psz);
   boolean        (*strends)(const char *cpszSuffic, const char *psz);
   char          *(*strchrend)(const char *pszSrc, char c);
   char          *(*strchrsend)(const char *pszSrc, const char *pszChars);
   char          *(*memrchr)(const char *pcSrch, int c, size_t nLen);
   char          *(*memchrend)(const char *pcSrch, int c, size_t nLen);
   char          *(*memrchrbegin)(const char *pcSrch, int c, size_t nLen);
   char          *(*strlower)(char *psz);
   char          *(*strupper)(char *psz);
   int            (*wstricmp)(const AECHAR * p1, const AECHAR * p2);
   int            (*wstrnicmp)(const AECHAR * p1, const AECHAR * p2, size_t length);
   boolean        (*inet_aton)(const char *pc, uint32 *pulAddr);
   void           (*inet_ntoa)(uint32 ulAddr, char *pc, size_t nLen);
   uint32         (*swapl)(uint32 ul);
   uint16         (*swaps)(uint16 us);

   // Heap/File query functions.

   uint32         (*GetFSFree)(uint32 * pdwTotal);
   uint32         (*GetRAMFree)(uint32 * pdwTotal,uint32 * pdwLargest);

   int            (*vsprintf)(char * pszDest,const char * pszFormat, AEEOldVaList arg);
   int32          (*vsnprintf)(char *buf, uint32 f, const char *format, AEEOldVaList list);
   int32          (*snprintf)(char *buf, uint32 f, const char *format, ...);
	
	// Added for 2.0
   uint32         (*JulianToSeconds)(JulianType* pDate);
   size_t         (*strlcpy)(char * dst, const char * src, size_t nSize);
   size_t         (*strlcat)(char * dst, const char * src, size_t nSize);
   size_t         (*wstrlcpy)(AECHAR * dst, const AECHAR * src, size_t nSize);
   size_t         (*wstrlcat)(AECHAR * dst, const AECHAR * src, size_t nSize);
   void          *(*setstaticptr)(int nSPId, void *pNew);
   
   //More floating point operations
   double         (*f_assignstr)(const char *pszFloat);
   double         (*f_assignint)(int32 val);
   AECHAR *       (*wwritelong)(AECHAR *pszBuf, long n);
   void          *(*dbgheapmark)(void *ptr, const char * pszFile, int nLine);
   int            (*lockmem)(void ** ppHandle);
   int            (*unlockmem)(void ** ppHandle);
   void           (*dumpheap)(void);
   double         (*strtod)(const char *pszFloat, char ** ppszEnd);
   double         (*f_calc)(double x, int calcType);
   void           (*sleep)(uint32 msecs);
   int            (*getlasterror)(int errRegion);
   double         (*wgs84_to_degrees)(int32 latlon);
   void           (*dbgevent)(AEEEvent evt, AEECLSID cls, uint32 pl);
   boolean        (*AEEOS_IsBadPtr)(int chkType, void *pBuf, uint32 len);
   char          *(*aee_basename)(const char *cpszPath);
   int            (*aee_makepath)(const char *cpszDir, const char *cpszFile,
                                  char *pszOut, int *pnOutLen);
   char          *(*aee_splitpath)(const char *cpszPath, const char *cpszDir);   
   boolean        (*aee_stribegins)(const char *cpszPrefix, const char *psz);
   uint32         (*GetUTCSeconds)(void);
   int            (*f_toint)(double val);
   double         (*f_get)(uint32 fgetType);
   void           (*qsort)(void *base, size_t nmemb, size_t size, PFNQSORTCOMPARE pfn);
   int32          (*trunc)(double x);
   uint32         (*utrunc)(double x);
   int            (*err_realloc)(uint32 uSize, void **pp);
   int            (*err_strdup)(const char *pszSrc, char **pp);
   
   // Utility functions for IPV6.
   int            (*inet_pton)(int af, const char *src, void *dst);   
   const char    *(*inet_ntop)(int af, const void *src, char *dst, size_t size);
   IQueryInterface *(*GetALSContext)(void);
};

#define FO_ADD          0
#define FO_SUB          1
#define FO_MUL          2
#define FO_DIV          3
#define FO_CMP_L        4
#define FO_CMP_LE       5
#define FO_CMP_E        6
#define FO_CMP_G        7
#define FO_CMP_GE       8
#define FO_POW          9
#define FCALC_FLOOR    10
#define FCALC_CEIL     11
#define FCALC_SQRT     12
#define FGET_HUGEVAL   13
#define FGET_FLTMAX    14
#define FGET_FLTMIN    15

#define FCALC_SIN      16
#define FCALC_COS      17
#define FCALC_ABS      18
#define FCALC_TAN      19

#define ERR_REGION_FLOAT 0

#if !defined(AEE_STATIC)

/*
   ||
   || This section does stdlib vtable and vtable version discovery
   ||
   || On the phone, it is two 32-bit values immediately before AEEModLoad
   ||   The shell loads a Module and stuffs the pointer into RAM just before
   ||   the Module (which must be compiled with AEEModLoad the first function
   ||   int the first module).
   ||
   || In in the sim, the helper table is just a global variable initialized
   ||   by AEEMod_Load().  There's no such thing as AEEStdLibVersion in 
   ||   the simulator.
   ||
*/
#if defined(AEE_SIMULATOR)
/* Simulator case */

#ifdef __cplusplus
extern "C" {
#endif

extern AEEHelperFuncs *g_pvtAEEStdLibEntry;

#ifdef __cplusplus
}
#endif

#define GET_HELPER()     g_pvtAEEStdLibEntry
#define GET_HELPER_VER() 0 /* soon to be gone */

#else /* #if defined(AEE_SIMULATOR) */

#ifdef __cplusplus
extern "C" {
#endif

extern int AEEMod_Load(IShell *ps, void * pszRes, IModule ** pm);

#ifdef __cplusplus
}
#endif

/*lint -emacro(611,GET_HELPER) doesn't like cast from function ptr*/
/*lint -emacro(740,GET_HELPER) doesn't like cast from function ptr*/
#define GET_HELPER()      (*(((AEEHelperFuncs **)AEEMod_Load) - 1))
#define GET_HELPER_VER()  (*((uint32 *)(((byte *)AEEMod_Load) - sizeof(AEEHelperFuncs *) - sizeof(uint32))))

#endif /* #if defined(AEE_SIMULATOR) */


/* ==========================================================================
  ||
  || Dynamic, or AEE_SIMULATOR linkers get these
  ||
  ========================================================================== */

#define MEMMOVE                     GET_HELPER()->memmove
#define MEMSET                      GET_HELPER()->memset
#define MEMCMP                      GET_HELPER()->memcmp
#define MEMCHR                      GET_HELPER()->memchr
#define MEMSTR                      GET_HELPER()->memstr
#define MEMRCHR                     GET_HELPER()->memrchr
#define MEMCHREND                   GET_HELPER()->memchrend
#define MEMRCHRBEGIN                GET_HELPER()->memrchrbegin
#define MSLEEP                      GET_HELPER()->sleep
#define STRCPY                      GET_HELPER()->strcpy
#define STRNCPY                     GET_HELPER()->strncpy
#define STRNCMP                     GET_HELPER()->strncmp
#define STRICMP                     GET_HELPER()->stricmp
#define STRNICMP                    GET_HELPER()->strnicmp
#define STRCAT                      GET_HELPER()->strcat
#define STRCMP                      GET_HELPER()->strcmp
#define STRLEN                      GET_HELPER()->strlen
#define STRCHR                      GET_HELPER()->strchr
#define STRCHREND                   GET_HELPER()->strchrend
#define STRCHRSEND                  GET_HELPER()->strchrsend
#define STRRCHR                     GET_HELPER()->strrchr
#define STRSTR                      GET_HELPER()->strstr
#define STRISTR                     GET_HELPER()->stristr
#define STRBEGINS                   GET_HELPER()->strbegins
#define STRIBEGINS                  GET_HELPER()->aee_stribegins
#define STRENDS                     GET_HELPER()->strends
#define STRLOWER                    GET_HELPER()->strlower
#define STRUPPER                    GET_HELPER()->strupper
#define SPRINTF                     GET_HELPER()->sprintf
#define SNPRINTF                    GET_HELPER()->snprintf
#define STRTOUL                     GET_HELPER()->strtoul
#define STRTOD                      GET_HELPER()->strtod
#define STRLCPY                     GET_HELPER()->strlcpy
#define STRLCAT                     GET_HELPER()->strlcat
#define WSTRCPY                     GET_HELPER()->wstrcpy
#define WSTRCAT                     GET_HELPER()->wstrcat
#define WSTRCMP                     GET_HELPER()->wstrcmp
#define WSTRNCMP                    GET_HELPER()->wstrncmp
#define WSTRICMP                    GET_HELPER()->wstricmp
#define WSTRNICMP                   GET_HELPER()->wstrnicmp
#define WSTRLEN                     GET_HELPER()->wstrlen
#define WSTRCHR                     GET_HELPER()->wstrchr
#define WSTRRCHR                    GET_HELPER()->wstrrchr
#define WSPRINTF                    GET_HELPER()->wsprintf
#define STRTOWSTR                   GET_HELPER()->strtowstr
#define WSTRTOSTR                   GET_HELPER()->wstrtostr
#define WSTRTOFLOAT                 GET_HELPER()->wstrtofloat
#define FLOATTOWSTR                 GET_HELPER()->floattowstr
#define UTF8TOWSTR                  GET_HELPER()->utf8towstr
#define WSTRTOUTF8                  GET_HELPER()->wstrtoutf8
#define WSTRLOWER                   GET_HELPER()->wstrlower
#define WSTRUPPER                   GET_HELPER()->wstrupper
#define WSTRLCPY                    GET_HELPER()->wstrlcpy
#define WSTRLCAT                    GET_HELPER()->wstrlcat
#define GETCHTYPE                   GET_HELPER()->chartype
#define ATOI                        GET_HELPER()->atoi
#define WSTRCOMPRESS                GET_HELPER()->wstrcompress
#define STREXPAND                   GET_HELPER()->strexpand
#define LOCALTIMEOFFSET             GET_HELPER()->LocalTimeOffset
#define GETAPPINSTANCE              GET_HELPER()->GetAppInstance
#define GETALSCONTEXT               GET_HELPER()->GetALSContext
#define QSORT                       GET_HELPER()->qsort

static __inline int VSPRINTF(char *buf, const char *fmt, va_list arg)
{
   return GET_HELPER()->vsprintf(buf, fmt, AEEOldVaList_From_va_list((va_list *)&(arg)));
}

static __inline int VSNPRINTF(char *buf, uint32 len, const char *fmt, va_list arg)
{
   return GET_HELPER()->vsnprintf(buf, len, fmt, AEEOldVaList_From_va_list((va_list *)&(arg)));
}

// Bitmap Conversions

#define CONVERTBMP(src,pi,pb)       GET_HELPER()->SetupNativeImage(AEECLSID_WINBMP,(src),(pi),(pb))

// Memory allocation routines

#define FREE                        GET_HELPER()->free
#define SYSFREE                     GET_HELPER()->sysfree
#define DUMPHEAP                    GET_HELPER()->dumpheap


#if (AEE_DBG_HEAP == 1)
#define MALLOC(size)                GET_HELPER()->dbgheapmark(GET_HELPER()->malloc((size)),__FILE__,__LINE__)
#define REALLOC(p,size)             GET_HELPER()->dbgheapmark(GET_HELPER()->realloc((p),(size)),__FILE__,__LINE__)
#define WSTRDUP(p)                  (AECHAR*)(GET_HELPER()->dbgheapmark(GET_HELPER()->wstrdup((p)),__FILE__,__LINE__))
#define STRDUP(p)                   (char*)(GET_HELPER()->dbgheapmark(GET_HELPER()->strdup((p)),__FILE__,__LINE__))
#else
#define MALLOC                      GET_HELPER()->malloc
#define REALLOC                     GET_HELPER()->realloc
#define WSTRDUP                     GET_HELPER()->wstrdup
#define STRDUP                      GET_HELPER()->strdup
#endif

#define DBGHEAPMARKEX               GET_HELPER()->dbgheapmark

#define LOCKMEM                     GET_HELPER()->lockmem
#define UNLOCKMEM                   GET_HELPER()->unlockmem

// Heap/File Stats

#define GETFSFREE                   GET_HELPER()->GetFSFree
#define GETRAMFREE                  GET_HELPER()->GetRAMFree

// Additional Wide String Helpers

#define WWRITELONG                  GET_HELPER()->wwritelong
#define WWRITELONGEX                GET_HELPER()->wwritelongex
#define WSTRSIZE                    GET_HELPER()->wstrsize
#define WSTRNCOPYN                  GET_HELPER()->wstrncopyn

// OEM String Functions

#define OEMSTRSIZE                  GET_HELPER()->OEMStrSize
#define OEMSTRLEN                   GET_HELPER()->OEMStrLen

// Version

#define GETAEEVERSION               GET_HELPER()->GetAEEVersion

// Patch
#define AEE_IS_PATCH_PRESENT(p)     GET_HELPER()->GetAEEVersion((byte*)(p), 0, GAV_INTERNAL)

#define FADD(v1,v2)                 GET_HELPER()->f_op((v1),(v2),FO_ADD)
#define FSUB(v1,v2)                 GET_HELPER()->f_op((v1),(v2),FO_SUB)
#define FMUL(v1,v2)                 GET_HELPER()->f_op((v1),(v2),FO_MUL)
#define FDIV(v1,v2)                 GET_HELPER()->f_op((v1),(v2),FO_DIV)
#define FPOW(v1,v2)                 GET_HELPER()->f_op((v1),(v2),FO_POW)
#define FCMP_L(v1,v2)               GET_HELPER()->f_cmp((v1),(v2),FO_CMP_L)
#define FCMP_LE(v1,v2)              GET_HELPER()->f_cmp((v1),(v2),FO_CMP_LE)
#define FCMP_E(v1,v2)               GET_HELPER()->f_cmp((v1),(v2),FO_CMP_E)
#define FCMP_G(v1,v2)               GET_HELPER()->f_cmp((v1),(v2),FO_CMP_G)
#define FCMP_GE(v1,v2)              GET_HELPER()->f_cmp((v1),(v2),FO_CMP_GE)
#define FASSIGN_STR                 GET_HELPER()->f_assignstr
#define FASSIGN_INT                 GET_HELPER()->f_assignint
#define FFLOOR(v)                   GET_HELPER()->f_calc((v), FCALC_FLOOR)
#define FCEIL(v)                    GET_HELPER()->f_calc((v), FCALC_CEIL)
#define FSQRT(v)                    GET_HELPER()->f_calc((v), FCALC_SQRT)
#define FSIN(v)                     GET_HELPER()->f_calc((v), FCALC_SIN)
#define FCOS(v)                     GET_HELPER()->f_calc((v), FCALC_COS)
#define FTAN(v)                     GET_HELPER()->f_calc((v), FCALC_TAN)
#define FABS(v)                     GET_HELPER()->f_calc((v), FCALC_ABS)
#define FLTTOINT(v)                 GET_HELPER()->f_toint((v))
#define FGETHUGE_VAL()              GET_HELPER()->f_get(FGET_HUGEVAL)
#define FGETFLT_MAX()               GET_HELPER()->f_get(FGET_FLTMAX)
#define FGETFLT_MIN()               GET_HELPER()->f_get(FGET_FLTMIN)
#define TRUNC                       GET_HELPER()->trunc
#define UTRUNC                      GET_HELPER()->utrunc
#define ERR_REALLOC(nlen,pp)        GET_HELPER()->err_realloc(nlen,(void **)(pp))
#define ERR_STRDUP                  GET_HELPER()->err_strdup
#define GETLASTFPERROR()            GET_HELPER()->getlasterror(ERR_REGION_FLOAT)

// Debug

#define __DBGPRINTF                 GET_HELPER()->dbgprintf
#define DBGEVENT_EX                 GET_HELPER()->dbgevent

// time 

#define GETRAND                     GET_HELPER()->GetRand
#define GETTIMEMS                   GET_HELPER()->GetTimeMS
#define GETTIMESECONDS              GET_HELPER()->GetSeconds
#define GETUTCSECONDS               GET_HELPER()->GetUTCSeconds
#define GETUPTIMEMS                 GET_HELPER()->GetUpTimeMS
#define GETJULIANDATE               GET_HELPER()->GetJulianDate
#define JULIANTOSECONDS             GET_HELPER()->JulianToSeconds

#define INET_ATON                   GET_HELPER()->inet_aton
#define INET_NTOA                   GET_HELPER()->inet_ntoa

#define SWAPS                       GET_HELPER()->swaps
#define SWAPL                       GET_HELPER()->swapl

#define SETSTATICPTR                GET_HELPER()->setstaticptr

//Helper functions to convert GPS WGS84 coordinates to degrees
#define WGS84_TO_DEGREES            GET_HELPER()->wgs84_to_degrees

//Memory validation functions
#define ISBADREADPTR(p,n)           GET_HELPER()->AEEOS_IsBadPtr(BREW_MEM_READ_ACCESS, (p), (n))
#define ISBADWRITEPTR(p,n)          GET_HELPER()->AEEOS_IsBadPtr(BREW_MEM_WRITE_ACCESS, (p), (n))

#define BASENAME                    GET_HELPER()->aee_basename
#define MAKEPATH                    GET_HELPER()->aee_makepath
#define SPLITPATH                   GET_HELPER()->aee_splitpath

// Utility functions for IPV6
#define INET_PTON                   GET_HELPER()->inet_pton
#define INET_NTOP                   GET_HELPER()->inet_ntop

/* ==========================================================================
  ||
  || end Dynamic
  ||
  ========================================================================== */

#else /* !defined AEE_STATIC) */

/*
  || People building dynamic apps (for the sim or for the phone) won't 
  ||  have this include file, and shouldn't.
*/
#define AEESTDLIB_STATIC_H__INC__BY__AEESTDLIB_H
#include "AEEStdLib_static.h"
#undef  AEESTDLIB_STATIC_H__INC__BY__AEESTDLIB_H

#endif /* !defined AEE_STATIC) */


/* ==========================================================================
  ||
  || BOTH get these, they're in terms of either the static or dynamic versions
  ||
  ========================================================================== */

// deprecated versions of _'d fns

#define GET_RAND(pd,s)             GETRAND((pd),(s))
#define GET_TIMEMS()               GETTIMEMS()
#define GET_SECONDS()              GETTIMESECONDS()
#define GET_UPTIMEMS()             GETUPTIMEMS()
#define GET_JULIANDATE(dw,pd)      GETJULIANDATE((dw),(pd))
#define GET_APP_INSTANCE()         GETAPPINSTANCE()
#define STR_TO_WSTR(s,d,l)         STRTOWSTR((s),(d),(l))
#define WSTR_TO_STR(s,d,l)         WSTRTOSTR((s),(d),(l))
#define WSTR_TO_FLOAT(s)           WSTRTOFLOAT((s))          
#define FLOAT_TO_WSTR(v,d,dl)      FLOATTOWSTR((v),(d),(dl))  
#define UTF8_TO_WSTR(s,sl,d,dl)    UTF8TOWSTR((s),(sl),(d),(dl)) 
#define WSTR_TO_UTF8(s,sl,d,dl)    WSTRTOUTF8((s),(sl),(d),(dl)) 

#define StrToWStr(src,dest,size)   STRTOWSTR((src),(dest),(size))
#define WStrToStr(src,dest,size)   WSTRTOSTR((src),(dest),(size))

#define WSTRNCOPY(d, dlen, s)      WSTRNCOPYN((d), (dlen), (s), -1)

#if !defined(ALLOC_NO_ZMEM)
#define ALLOC_NO_ZMEM         (0x80000000l)
#endif

// other handy macros, or those defined in terms of raw GETHELPER versions 
//  above
#define MEMCPY                    MEMMOVE
#define CREATEOBJ(n)              MALLOC(n)
#define FREEOBJ(p)                FREE(p)
#define MALLOCREC(t)              ((t*)MALLOC(sizeof(t)))
#define MALLOCREC_EX(t,n)         (ALLOC_NO_ZMEM - sizeof(t) > ((n) & ~ALLOC_NO_ZMEM)) ? ((t*)MALLOC(sizeof(t)+(n))) : (t*)0
#define FREEIF(p)                 if (p) { FREE((void*)p); (p) = 0; }

#define ERR_MALLOC(nlen,pp)       (*(pp)=0,ERR_REALLOC((nlen),(void**)(pp)))
#define ERR_MALLOCREC(t,pp)       (*(pp)=(t*)0,ERR_REALLOC(sizeof(t),(void**)(pp)))
#define ERR_MALLOCREC_EX(t,n,pp)  (ALLOC_NO_ZMEM - sizeof(t) > ((n) & ~ALLOC_NO_ZMEM)) ? (*(pp)=(t*)0,ERR_REALLOC(sizeof(t)+(n),(void**)(pp))) : ENOMEMORY

#define DBGHEAPMARK(p)            DBGHEAPMARKEX((p),__FILE__,__LINE__)
#define DBGEVENT(evt,cls)         DBGEVENT_EX((evt),(cls),(0))

#define AEE_SECS_PER_YEAR         31536000L    /* one year in seconds */
#define ISTIMEVALID()             (GETUTCSECONDS() > AEE_SECS_PER_YEAR)

// for those of you who always forget the argument order to memset.. 
#define ZEROAT(ptr) (void)MEMSET(ptr, 0, sizeof(*ptr))


#ifdef HTONS
#undef HTONS
#endif /* #ifdef HTONS */

#ifdef HTONL
#undef HTONL
#endif /* #ifdef HTONL */

#ifdef NTOHS
#undef NTOHS
#endif /* #ifdef NTOHS */

#ifdef NTOHL
#undef NTOHL
#endif /* #ifdef NTOHL */


#if defined(FEATURE_BIG_ENDIAN)

# define HTONS(x)                    ((uint16)(x))
# define HTONL(x)                    ((uint32)(x))
# define HTOLES(x)                   SWAPS((x))
# define HTOLEL(x)                   SWAPL((x))

/* destructive versions (_D), resolve to nothing if noop */

# define HTOLES_D(x)    do { \
                           uint16 ans = x; \
                           ((uint8*)(&(x)))[0]=((uint8*)(&ans))[1]; \
                           ((uint8*)(&(x)))[1]=((uint8*)(&ans))[0]; \
                        } while (0)

# define HTOLEL_D(x)    do { \
                           uint32 ans = x; \
                           ((uint8*)(&(x)))[0]=((uint8*)(&ans))[3]; \
                           ((uint8*)(&(x)))[1]=((uint8*)(&ans))[2]; \
                           ((uint8*)(&(x)))[2]=((uint8*)(&ans))[1]; \
                           ((uint8*)(&(x)))[3]=((uint8*)(&ans))[0]; \
                        } while (0)

# define HTOLEE_D(x)  \
 do { \
   if (sizeof((x)) == 2) { \
     HTOLES_D(x); \
   } \
   else if (sizeof((x)) == 4) { \
     HTOLEL_D(x); \
   } \
 } while (0)

#else /* #if defined(FEATURE_BIG_ENDIAN) */

# define HTONS(x)                    SWAPS((x))
# define HTONL(x)                    SWAPL((x))
# define HTOLES(x)                   ((uint16)(x))
# define HTOLEL(x)                   ((uint32)(x))

/* destructive versions (_D), resolve to nothing if noop */

# define HTOLES_D(x)
# define HTOLEL_D(x)
# define HTOLEE_D(x)

#endif /* #if defined(FEATURE_BIG_ENDIAN) */


/* define these in terms of the above (do same thing) */
#define NTOHS(x)          HTONS((x))
#define NTOHL(x)          HTONL((x))
#define LETOHS(x)         HTOLES((x))
#define LETOHL(x)         HTOLEL((x))

#define LETOHS_D(x)       HTOLES_D((x))
#define LETOHL_D(x)       HTOLEL_D((x))
#define LETOHE_D(x)       HTOLEE_D((x))

// size to store IPv4 and IPv6 addresses in string form
#define AEE_INET_ADDRSTRLEN    16
#define AEE_INET6_ADDRSTRLEN   46

/* setstaticptr ids */
#define AEESPID_STACKBOTTOM 0
#define AEESPID_TLS         1

#define SETSTACKBOTTOM(p) SETSTATICPTR(AEESPID_STACKBOTTOM,(p))
#define SETTLS(p)         SETSTATICPTR(AEESPID_TLS,(p))
#define GETTLS(p)         (void)SETTLS((p)=SETTLS(0))


#define DBGPRINTF_LEVEL_LOW    1
#define DBGPRINTF_LEVEL_MED    2
#define DBGPRINTF_LEVEL_HIGH   3
#define DBGPRINTF_LEVEL_ERROR  4
#define DBGPRINTF_LEVEL_FATAL  5
#ifdef USES_RELEASE_VERSION
#define DBGPRINTF(...)
#define DBGPRINTF_LOW(...)
#define DBGPRINTF_MED(...)
#define DBGPRINTF_HIGH(...)
#define DBGPRINTF_ERROR(...)
#define DBGPRINTF_FATAL(...)
#else
#define DBGPRINTF_EX_FORMAT    "*dbgprintf-%d* %s:%d"
#define DBGPRINTF              __DBGPRINTF(DBGPRINTF_EX_FORMAT,DBGPRINTF_LEVEL_ERROR ,__FILE__,__LINE__),__DBGPRINTF
#define DBGPRINTF_LOW          __DBGPRINTF(DBGPRINTF_EX_FORMAT,DBGPRINTF_LEVEL_LOW  ,__FILE__,__LINE__),__DBGPRINTF
#define DBGPRINTF_MED          __DBGPRINTF(DBGPRINTF_EX_FORMAT,DBGPRINTF_LEVEL_MED  ,__FILE__,__LINE__),__DBGPRINTF
#define DBGPRINTF_HIGH         __DBGPRINTF(DBGPRINTF_EX_FORMAT,DBGPRINTF_LEVEL_HIGH ,__FILE__,__LINE__),__DBGPRINTF
#define DBGPRINTF_ERROR        __DBGPRINTF(DBGPRINTF_EX_FORMAT,DBGPRINTF_LEVEL_ERROR,__FILE__,__LINE__),__DBGPRINTF
#define DBGPRINTF_FATAL        __DBGPRINTF(DBGPRINTF_EX_FORMAT,DBGPRINTF_LEVEL_FATAL,__FILE__,__LINE__),__DBGPRINTF
#endif
/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
======================================================================= 


=======================================================================


=====================================================================
  MACROS   DOCUMENTATION
======================================================================= 

 Helper Functions

Description: 
   This section provides documentation for the various
helper functions offered by AEE. This includes string functions,
functions in the standard C library, utility functions etc. Standard C Library 
refers to the ANSI Standard C Library supplied with C/C++ compilers/IDE.
Applications must not directly invoke the standard C library functions
(say memcpy). Instead, applications must use the functions provided
by AEE (such as MEMCPY). A distinct difference between the functions
documented here and the rest of the AEE functions is that there is no
need to have an interface pointer to access these functions. For ex: to 
invoke the function MEMCPY, there is no need for an interface pointer. 
Applications can directly invoke MEMCPY().  Some of the functions provided here
make direct calls to the standard C library functions (ex: MEMCPY() directly
invokes the C library function memcpy()). However, these functions are provided
here for two reasons: 
   1. To prevent the need for every application to statically link with the
   standard C library. When there are multiple apps loaded on the phone,
   each app has an extra baggage of carrying the standard C library. To avoid
   this, AEE maintains a pointer to a single copy of the standard C library.
   All applications can make use of this copy. Applications hence must not make
   direct calls to the standard C library functions (there by, avoiding the app
   from being associated with the baggage of static C library)
   2. Dynamic loadable apps must not have any static data associated with them.
   If applications were to make direct calls to the standard C library functions
   such as memcpy(), they need to invoke the corresponding header files 
   (ex: memory.h) and these header files do contain static data in them. Hence,
   it would prevent the app from being dynamic loadable.
   
   For the above two reasons, AEE offers the helper functions some of which
   are direct wrappers around the standard C library functions.


======================================================================= 

Function: MALLOC()

Description: 
   This function corresponds to malloc() in the standard C library; it
allocates a block of memory of the requested size and returns a
pointer to that memory block.  By default, memory allocated using
MALLOC() is initialized to zero.  This behavior can be disabled
by performing a bitwise OR of the flag ALLOC_NO_ZMEM with the dwSize
parameter.
  
Prototype:
   void *MALLOC(dword dwSize)
   
Parameters:
   dwSize:  Size of buffer in bytes (optionally ORed with ALLOC_NO_ZMEM)

Return Value:
   Returns a pointer to the allocated memory.

Comments:  
   Specifying a dwSize of zero will result in a return value of NULL.

Side Effects: 
   None

Version:
   Introduced BREW Client 1.0

See Also: 
   FREE()  REALLOC() 
======================================================================= 

Function: FREE()

Description: 
   This function is corresponding to free() in standard C library.  Internally
   it does more than just free(), though the external behavior is the same 
   as free(). 
   
Prototype:
   void FREE(void *po)
   

Parameters:
   po: Points to the memory to be freed.

Return Value:
   None

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.0

See Also: 
   MALLOC()  REALLOC()

======================================================================= 

Function: REALLOC()

Description: 
   This function corresponds to realloc() in the standard C library;
it reallocates a memory block and changes its size. By default, any
extra memory allocated using REALLOC() is initialized to zero.  This
behavior can be disabled by performing a bitwise OR of the flag
ALLOC_NO_ZMEM with the dwSize parameter.

Prototype:
   void *REALLOC(void *pSrc, uint32 dwSize)
   

Parameters:
   pSrc: Points to the previously allocated memory. If pSrc is NULL,
this function behaves the same way as MALLOC() and allocates a new
block of dwSize bytes.
   dwSize: New size in bytes (optionally ORed with ALLOC_NO_ZMEM)

Return Value:
   Returns a pointer to the newly allocated memory 

Comments:  
    The return value is NULL if the size is 0 (zero) and pSrc is valid, or
if there is not enough available memory to expand pSrc to the given size. In
the first case, the original block is freed. In the second, the original block
is unchanged.

Side Effects: 
   None

Version:
   Introduced BREW Client 1.0

See Also: 
   MALLOC()  FREE()

=======================================================================

Function: STRDUP()

Description: 
   This function is like a wrapper around the strdup() function provided
   by the standard C library. Its behavior is identical to that of strdup(), 
   but the memory returned must be freed with FREE()
This function allocates memory and copies string, returning the copy.
This function operates only on strings on single-byte characters. To operate
on strings of double-byte AECHARs, applications should WSTRDUP() instead.

Prototype:

   char *STRDUP(const char *string);

Parameters:
   string: string to copy

Return Value:
  Returns a pointer to the new copied string, or NULL on failure 

Comments:  
   the memory must be freed using FREE or FREEIF

Side Effects: 
   None

Version:
   Introduced BREW Client 1.1

See Also: 
   FREE(), MALLOC()

======================================================================= 

Function: FASSIGN_STR()

Description: 
   This function returns a double with the value specified by the input 
   string. 

Prototype:
   double FASSIGN_STR(char *pszFloat)
   
Parameters:
   pszFloat: String containing the value to be assigned


Return Value:
   Returns a double of the value specified

Comments:  
   This is an easy way to initialize floating-point variables.
   For example FASSIGN_STR("3.1416") returns a double of value 3.1416.
   This mechanism is useful in BREW modules where an assignment
   such as 
   double f = 3.1416
   causes linkage problems.

Side Effects: 
   None

Version:
   Introduced BREW Client 2.0

See Also: 

===========================================================================
Function:  UNLOCKMEM()

Description:

   WARNING: This function should not be used without clearly understanding
all rules associated with handled-based memory.

By default, all memory allocated via the BREW heap is locked and cannot 
be relocated.  This function provides a means for the caller to specify that
a recently allocated memory block can be moved.  It is called as follows:

Note that the following 2 dangerous conditions must be avoided:

- Passing a handle to memory that resides inside a moveable memory block.  
- Assigning a handle to memory when that memory is already referenced
by a handle in another block.

Initial Allocation:

   pme->m_ptr = MALLOC(100);
   UNLOCKMEM(&pme->m_ptr);    // Make memory free to move.

Use:
 
   LOCKMEM(&pme->m_ptr);
   Perform Work...
   UNLOCKMEM(&pme->m_ptr);


As indicated, this function accepts a pointer to a handle of allocated memory.
The function will then mark the block as available to be moved in memory. 
Note that this function monitors a "lock count" on the memory block.  This
allows the caller to pair LOCKMEM and UNLOCKMEM inside sub-routines that may be
called while the memory is locked.  In such cases, the memory will not actually 
be "unlocked" until the reference count reaches 0.

Prototype:
   int UNLOCKMEM(void ** ppHandle);

Parameters:
   None.

Return Value:
   Positive or zero values:(>= 0) - Decremented lock count of the memory

   Negative Values:
~
      EALREADY - Memory is already unlocked and associated with another
        sentinal.~
      EMEMPTR - Invalid ppHandle~
      EFAILED - Association is invalid.  Breakpoint thrown on SDK simulator.
*

Comments:
  None

Side Effects:
  None

Version:
   Introduced BREW Client 2.0

See Also:
   None

===========================================================================
Function:  LOCKMEM()

Description:
   WARNING: This function should not be used without clearly understanding
all rules associated with handled-based memory.

By default, all memory allocated via the BREW heap is locked and cannot 
be relocated.  This function provides a means for the caller to specify that
a recently allocated and unlocked block is now locked and cannot be moved.

Initial Allocation:

   pme->m_ptr = MALLOC(100);
   UNLOCKMEM(&pme->m_ptr);

Use:
 
   LOCKMEM(&pme->m_ptr);
   Perform Work...
   UNLOCKMEM(&pme->m_ptr);


As indicated, this function accepts a pointer to a handle of allocated memory.
The function will then mark the block as unavailable to be moved in memory.
Note that this function also increments a "lock count" on the memory block.
This allows the caller to pair LOCKMEM and UNLOCKMEM inside sub-routines that
may be called while the memory is locked.  In such cases, the memory will not 
actually be "unlocked" until the reference count reaches 0.

Prototype:
   int LOCKMEM(void ** ppHandle);

Parameters:
   None.

Return Value:
   Positive values (> 0) - Incremented lock count of the memory

   Zero value (0) - Invalid ppHandle

Comments:
  None

Side Effects:
  None

Version:
   Introduced BREW Client 2.0

See Also:
   None


======================================================================= 

Function: FASSIGN_INT()

Description: 
   This function returns a double with the value specified by the input 
   integer parameter. 

Prototype:
   double FASSIGN_INT(int32 val)
   
Parameters:
   val: The value to be assigned to the double returned.


Return Value:
   Returns a double of the value specified

Comments:  
   This is an easy way to initialize floating-point variables using integer
   variables. For example, FASSIGN_INT(3) returns a double of value 3.0 

Side Effects: 
   None

Version:
   Introduced BREW Client 2.0

See Also:
   None
     
======================================================================= 

Function: FADD()

Description: 
   This function does floating point "+" operation. 

Prototype:
   double FADD(double v1, double v2)
   
Parameters:
   v1: operand #1
   v2: operand #2

Return Value:
   Returns the result of "+"

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.1

See Also:
   None 
   
======================================================================= 

Function: FSUB()

Description: 
   This function does floating point "-" operation. 

Prototype:
   double FSUB(double v1, double v2)
   
Parameters:
   v1: operand #1
   v2: operand #2

Return Value:
   Returns the result of "-"

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.0

See Also:
   None 
   
======================================================================= 

Function: FMUL()

Description: 
   This function does floating point "*" operation. 

Prototype:
   double FMUL(double v1, double v2)
   
Parameters:
   v1: operand #1
   v2: operand #2

Return Value:
   Returns the result of "*"

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.0

See Also:
   None 
   
======================================================================= 

Function: FDIV()

Description: 
   This function does floating point "/" operation. 

Prototype:
   double FDIV(double v1, double v2)
   
Parameters:
   v1: operand #1
   v2: operand #2

Return Value:
   Returns the result of "/"
Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.0

See Also: 
   None

=====================================================================
Function: FCMP_L()

Description: 
   This function performs float point "<" comparison. 

Prototype:
   
   boolean FCMP_L(double v1, double v2)

Parameters:
   v1: operand #1
   v2: operand #2   

Return Value:
   Returns TRUE if v1 < v2

   Returns FALSE if v1 >= v2

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.0

See Also: 
   None

======================================================================= 

Function: FCMP_LE()

Description: 
   This function performs float point "<=" comparison. 

Prototype:
   
   boolean FCMP_LE(double v1, double v2)

Parameters:
   v1: operand #1
   v2: operand #2   

Return Value:
   Returns TRUE if v1 <= v2

   Returns FALSE if v1 > v2

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.0

See Also: 
   None

======================================================================= 

Function: FCMP_E()

Description: 
   This function performs float point "==" comparison. 

Prototype:
   
   boolean FCMP_E(double v1, double v2)

Parameters:
   v1: operand #1
   v2: operand #2   

Return Value:
   Returns TRUE if v1 == v2

   Returns FALSE if v1 != v2

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.0

See Also: 
   None

======================================================================= 

Function: FCMP_G()

Description: 
   This function performs float point ">" comparison. 

Prototype:
   
   boolean FCMP_G(double v1, double v2)

Parameters:
   v1: operand #1
   v2: operand #2   

Return Value:
   Returns TRUE if v1 > v2

   Returns FALSE if v1 <= v2

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.0

See Also: 
   None

======================================================================= 

Function: FCMP_GE()

Description: 
   This function performs float point ">=" comparison. 

Prototype:
   
   boolean FCMP_GE(double v1, double v2)

Parameters:
   v1: operand #1
   v2: operand #2   

Return Value:
   Returns TRUE if v1 >= v2

   Returns FALSE if v1 < v2

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.0

See Also: 
   None

======================================================================= 

Function: FPOW()

Description: 
   This function raises x to the power of y, where both x and y floating
   point numbers.

Prototype:
   
   double FPOW(double x, double y)

Parameters:
   x: operand #1
   y: operand #2   

Return Value:
   Returns x pow y

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 2.1

See Also: 
   None

======================================================================= 

Function: FLTTOINT()

Description: 
   This function returns the integer value of the floating point number 
   specified.

Prototype:
   
   int FLTTOINT(double x)

Parameters:
   x: The floating point number that needs to be converted to integer

Return Value:
   Returns int(x)

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 3.1.0

See Also: 
   None

======================================================================= 

Function: FGETHUGE_VAL()

Description: 
   This function returns HUGE_VAL.

Prototype:
   
   double FGETHUGE_VAL(void)

Parameters:
   None

Return Value:
   Returns HUGE_VAL

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 3.1.0

See Also: 
   None

======================================================================= 

Function: FGETFLT_MIN()

Description: 
   This function returns FLT_MIN

Prototype:
   
   double FGETFLT_MIN(void)

Parameters:
   None

Return Value:
   Returns FLT_MIN

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 3.1.0

See Also: 
   None

======================================================================= 

Function: FGETFLT_MAX()

Description: 
   This function returns FLT_MAX

Prototype:
   
   double FGETFLT_MAX(void)

Parameters:
   None

Return Value:
   Returns FLT_MAX

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 3.1.0

See Also: 
   None

======================================================================= 

Function: TRUNC()

Description: 
   This function converts the floating point parameter to
   a signed integer (truncating any fractional part)

Prototype:
   
   int32 TRUNC(double x)

Parameters:
   x: value

Return Value:
   Returns truncated integer value of x

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 3.1

See Also: 
   None

======================================================================= 

Function: UTRUNC()

Description: 
   This function converts the floating point parameter to
   an unsigned integer (truncating any fractional part)

Prototype:
   
   uint32 UTRUNC(double x)

Parameters:
   x: value

Return Value:
   Returns truncated unsigned integer value of x

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 3.1

See Also: 
   None

======================================================================= 

Function: GETLASTFPERROR()

Description: 
   This function returns the error set by the last floating point operation
   such as FPOW.

Prototype:
   
   int GETLASTFPERROR()

Parameters:
   None

Return Value:
   Returns error. See errno.h provided with your compiler for details.

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 2.1

See Also: 
   None

======================================================================= 

Function: FCEIL()

Description: 
   This function computes the smallest integral value not less than x.

Prototype:
   
   double FCEIL(double x)

Parameters:
   x: operand #1

Return Value:
   Returns ceiling of x

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 2.1

See Also: 
   None

======================================================================= 

Function: FFLOOR()

Description: 
   This function computes the largest integral value not greater than x.

Prototype:
   
   double FFLOOR(double x)

Parameters:
   x: operand #1

Return Value:
   Returns floor of x

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 2.1

See Also: 
   None

======================================================================= 

Function: FFLOOR()

Description: 
   This function computes the largest integral value not greater than x.

Prototype:
   
   double FFLOOR(double x)

Parameters:
   x: operand #1

Return Value:
   Returns floor of x

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 2.1

See Also: 
   None

======================================================================= 

Function: FSQRT()

Description: 
   This function computes the square root of the floating point number x. 

Prototype:
   
   double FSQRT(double x)

Parameters:
   x:  The number whose square root needs to be computed

Return Value:
   Returns the square root of x.

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 2.1

See Also:
   None

======================================================================= 

Function: FSIN()

Description: 
   This function computes SIN(x)

Prototype:
   
   double FSIN(double x)

Parameters:
   x: angle in radians

Return Value:
   Returns sine of x

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 3.1

See Also: 
   None

======================================================================= 

Function: FCOS()

Description: 
   This function computes COS(x)

Prototype:
   
   double FCOS(double x)

Parameters:
   x: angle in radians

Return Value:
   Returns cosine of x

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 3.1

See Also: 
   None

======================================================================= 

Function: FTAN()

Description: 
   This function computes tangent (x)

Prototype:
   
   double FTAN(double x)

Parameters:
   x: angle in radians

Return Value:
   Returns tan of x

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 3.1

See Also: 
   None

======================================================================= 

Function: FABS()

Description: 
   This function returns the absolute value of x

Prototype:
   
   double FABS(double x)

Parameters:
   x: value

Return Value:
   Returns absolute value of x

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 3.1

See Also: 
   None

======================================================================= 

Function: STRTOD()

Description: 
   This function converts the initial portion of the string pointed to by
   pszFloat  to type double. It ignores any preceding white space, tabs or 
   '+', and converts the subsequent substring (excluding the final string, 
   if any, of unrecognized charecters) to type double. On return, ppszEnd 
   is populated with the address of the final string of unrecognized 
   charecters.


Prototype:
   
   double STRTOD(const char *pszFloat, char ** ppszEnd)

Parameters:
   pszFloat: NULL terminated string to convert
   ppszEnd:  Pointer to character that stops the conversion

Return Value:
   Returns the value of the floating point number.

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 2.1

See Also: 
   None

======================================================================= 

Function: MSLEEP()

Description: 
   This function suspends execution of the calling BREW application
   for the number of milleseconds specified. The calling BREW application
   must have System privelages to be able to execute this function. BREW
   application developers are strongly advised against using this function
   unless they have a compelling reason.


Prototype:
   
   void MSLEEP(uint32 msecs)

Parameters:
   msecs: Sleep duration in milleseconds


Return Value:
   NOne

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 2.1

See Also: 
   None

======================================================================= 

Function: DBGPRINTF()

Description: 
   This function is used to output debugging information.  Since
    this function may be implemented in different ways on different 
    platforms, care must be taken with this function to get useful
    results.

  On the Windows SDK, this function's output shows up in the 
    "Output Window".  Each call to DBGPRINTF generates at most 128 bytes
    of output.

  On Qualcomm chipset targets, DBGPRINTF() comes out the diag port (though 
    this may change).  The length of each output string is constrained even 
    more than on the Windows SDK.

  In general, DBGPRINTF()'s behaviour should be tested before being 
    relied upon to provide useful output.

Prototype:
   void dbgprintf(const char *pszFormat,...)
   

Parameters:
   pszFormat: printf-style format-control string

   ... : variable argument array of things that are legal to pass to
            printf

Return Value:
   None

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.0

See Also: 
   VSNPRINTF(), SNPRINTF()
   
======================================================================= 

Function: DBGEVENT()

Description: 
   This function is used to output debugging events.  Since
   this function may be implemented in different ways on different 
   platforms, care must be taken with this function to get useful
   results.

   In general DBGEVENT Event Codes map to the standard BREW Events listed 
   in AEE.h, although not all the standard BREW Events are supported on
   all platforms.
  
   The minimum standard BREW Events that should be supported on all 
   platforms are:
  
   EVT_APP_START
   EVT_APP_STOP
   EVT_APP_SUSPEND
   EVT_APP_RESUME
   EVT_EXT_CREATE
   EVT_EXT_RELEASE
   EVT_ASYNC_ERROR
   EVT_USER
  
   A user may in addition send their own events.  User defined event
   codes must be equal to or larger then EVT_USER.
  
   For example, in this case the application's custom event is the 
   fifth user event:
  
      #define EVT_APP_CUSTOM      EVT_USER+5

   On the Windows SDK, the event shows up in the BREW Simulator
   "Output Window".  Each call to DBGEVENT() generates at most 128 bytes
   of output.

   On Qualcomm Chipset based platforms, events sent using DBGEVENT() are 
   delivered via the diag port (though this may change).

   In general, DBGEVENT()'s behavior is platform dependent so a platform
   specific understanding of how these events are defined and delivered is 
   required.

Prototype:
   void dbgevent(AEEEvent evt, AEECLSID cls)
   
Parameters:
   evt: AEE Event code to log
   cls: Class ID of the sending application, extension, or interface

Return Value:
   None

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 3.1

See Also: 
   None

======================================================================= 

Function: DBGEVENT_EX()

Description: 
   This function is used to output debugging events.  Since
   this function may be implemented in different ways on different 
   platforms, care must be taken with this function to get useful
   results.

   In general DBGEVENT Event Codes map to the standard BREW Events listed 
   in AEE.h, although not all the standard BREW Events are supported on
   all platforms.
  
   The minimum standard BREW Events that should be supported on all 
   platforms are:
  
   EVT_APP_START
   EVT_APP_STOP
   EVT_APP_SUSPEND
   EVT_APP_RESUME
   EVT_EXT_CREATE
   EVT_EXT_RELEASE
   EVT_ASYNC_ERROR
   EVT_USER
  
   A user may in addition send their own events.  User defined event
   codes must be equal to or larger then EVT_USER.
  
   For example, in this case the application's custom event is the 
   fifth user event:
  
      #define EVT_APP_CUSTOM      EVT_USER+5

   On the Windows SDK, the event shows up in the BREW Simulator
   "Output Window".  Each call to DBGEVENT() generates at most 128 bytes
   of output.

   On Qualcomm Chipset based platforms, events sent using DBGEVENT() are 
   delivered via the diag port (though this may change).

   In general, DBGEVENT()'s behavior is platform dependent so a platform
   specific understanding of how these events are defined and delivered is 
   required.

Prototype:
   void dbgevent(AEEEvent evt, AEECLSID cls, uint32 pl)
   

Parameters:
   evt: AEE Event code to log
   cls: Class ID of the sending application, extension, or interface
   pl:  User defined payload

Return Value:
   None

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 3.1

See Also: 
   None

======================================================================= 
  
Function: MEMCPY()

Description: 
   This function is a wrapper around the memcpy() function provided
by the standard C library. Its behavior is identical to that of memcpy().
This function copies count bytes of src to dest. 

Prototype:
   void *MEMCPY( void *dest, const void *src, size_t count );
   

Parameters:
   dest: pointer to the destination buffer
   src: pointer to the source buffer
   count: Specifies the number of bytes that must be coped from src to dest.

Return Value:
   Returns a pointer to the dest buffer

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.0

See Also: 
   MEMSET()

======================================================================= 

  
Function: MEMMOVE()

Description: 
   This function is a wrapper around the memmove() function provided
by the standard C library. Its behavior is identical to that of memmove().
This function copies count bytes of src to dest. 

Prototype:
   void *MEMMOVE( void *dest, const void *src, size_t count );
   

Parameters:
   dest: pointer to the destination buffer
   src: pointer to the source buffer
   count: Specifies the number of bytes that must be coped from src to dest.

Return Value:
   Returns a pointer to the dest buffer

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.1

See Also: 
   MEMSET(),MEMMOVE()

======================================================================= 
  
Function: MEMCMP()

Description: 
   This function is a wrapper around the memcmp() function provided
by the standard C library. Its behavior is identical to that of memcmp().
This compares count bytes of p1 against p2

Prototype:
   int MEMCMP( void *p1, const void *p2, size_t count );
   

Parameters:
   p1, p2: pointer to mem buffers to be compared
   count: Specifies the number of bytes that must be compared

Return Value:
   0 if the buffers contain the same count bytes

   < 0 if the first differing byte of p1 is less than that of p2

   > 0 if the first differing byte of p1 is greater than that of p2

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.1

See Also: 
   None

=======================================================================

Function: MEMCHR()

Description: 
   This function is a wrapper around the memchr() function provided
by the standard C library. Its behavior is identical to that of memchr().
This function finds a character in a memory buffer.

Prototype:

   char *MEMCHR( const char *buf, int c, size_t length );

Parameters:
   buf: memory to search
   c:  character to be located
   length: size of buf

Return Value:

  Returns a pointer to the first occurrence of c in buf, 
  
  NULL if c is not found.

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.1

See Also: 
   STRCHR()

=======================================================================

Function: MEMRCHR()

Description: 
    Find the last occurence of a character in a buffer,

Prototype:

   char *MEMRCHR( const char *buf, int c, size_t length );

Parameters:
   buf: memory to search
   c:  character to be located
   length: size of buf

Return Value:

  Returns a pointer to the last occurrence of c in buf, 
  
  NULL if c is not found.

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.1

See Also: 
   MEMCHR(),STRRCHR()


======================================================================= 


Function: MEMCHREND()

Description: 
   Find a character in a memory buffer, or return a pointer to 
   the end of the buffer


Prototype:

   char *MEMCHREND( const char *buf, int c, size_t length );

Parameters:
   buf: memory to search
   c:  character to be located
   length: size of buf

Return Value:

  Returns a pointer to the first occurrence of c in buf, 

  Or a pointer to buf + lenght if c is not found.

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.1

See Also: 
   MEMCHR(), STRCHREND()

======================================================================= 


Function: MEMRCHRBEGIN()

Description: 
   Find a the last occurence of a character in a memory buffer,
    or return a pointer to the beginning of the buffer


Prototype:

   char *MEMRCHRBEGIN( const char *buf, int c, size_t length );

Parameters:
   buf: memory to search
   c:  character to be located
   length: size of buf

Return Value:

  Returns a pointer to the last occurrence of c in buf

  Or a pointer to buf if c is not found.

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.1

See Also: 
   MEMRCHR(), MEMCHREND()

======================================================================= 


Function: MEMSTR()

Description: 
   Find a the first occurence of a string in a memory buffer,

Prototype:

   char *MEMSTR( const char *buf, const char *psz, size_t length );

Parameters:
   buf: memory to search
   c:  character to be located
   length: size of buf

Return Value:

  Returns a pointer to the first occurrence of psz in buf

  NULL if psz is not found

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.1

See Also: 
   STRSTR()

======================================================================= 


Function: MEMSET()

Description: 
   This function is a wrapper around the memset() function provided
by the standard C library. Its behavior is identical to that of memset().
This function sets the dest buffer to a specified byte b.

Prototype:

   void *MEMSET( void *dest, int b, size_t count );

Parameters:
   dest: pointer to the destination buffer
   b: Specifies the value that must be copied into the dest buffer
   count: Specifies the number of bytes in dest that must be set to the
   value b.


Return Value:
   Returns a pointer to the destination buffer

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.0

See Also: 
   MEMCPY()

=======================================================================

Function: STRCPY()

Description: 
   This function is a wrapper around the strcpy() function provided
by the standard C library. Its behavior is identical to that of strcpy().
This function copies the src string into dest and returns the dest string.
All characters upto and including the first null character in src 
are copied into dest. This function operates only on strings of single-byte
characters. To operate on strings of double-byte AECHARs, applications
should use WSTRCPY() instead.

Prototype:

   char *STRCPY( char *dest, const char *src );

Parameters:
   dest: pointer to the destination string
   src:  pointer to the source string

Return Value:
   Returns a pointer to the destination string

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.0

See Also: 
   WSTRCPY(), WSTRNCOPYN()

=======================================================================

Function: ATOI()

Description: 
   This function is a wrapper around the atoi() function provided
by the standard C library. Its behavior is identical to that of atoi().
This function converts the input string to the integer.

Prototype:

   int ATOI( const char *src );

Parameters:
   src:  pointer to the source string

Return Value:
   Returns a integer represented by the inut string

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.0

See Also: 
   None

=======================================================================

Function: STRCAT()

Description: 
   This function is a wrapper around the strcat() function provided
by the standard C library. Its behavior is identical to that of strcat().
This function appends the src string into dest and returns the dest string.
All characters upto and including the first null character in src 
are appended to dest. This function will always take single-byte character
as input. Applications wanting to append the double-byte character AECHAR
from one buffer into the other must always use the WSTRCAT() function.

Prototype:

   char *STRCAT( char *dest, const char *src );

Parameters:
   dest: pointer to the NULL terminated destination string
   src:  pointer to the NULL terminated source string

Return Value:
   Returns a pointer to the destination string

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.0

See Also: 
   WSTRCAT()

=======================================================================

Function: STRCMP()

Description: 
   This function is a wrapper around the strcmp() function provided
by the standard C library. Its behavior is identical to that of strcmp().
This function compares the given two single-byte strings.
This function will always take single-byte character
as input. 

Prototype:

   int STRCMP( const char *str1, const char *str2 );

Parameters:
   str1,str2: Pointers to the two null terminated strings that need to be compared

Return Value:

  Returns  a value < 0 if string1 less than string2 

  Return   0           if string1 identical to string2 

  Returns  a value > 0 if string1 greater than string2 


Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.0

See Also: 
   WSTRCMP()

=======================================================================

Function: STRNCMP()

Description: 
   This function is a wrapper around the strncmp() function provided
by the standard C library. Its behavior is identical to that of strncmp().
This function compares at most len bytes of the given two single-byte strings, 


Prototype:

   int STRNCMP( const char *str1, const char *str2 , size_t length);

Parameters:
   str1,str2: Pointers to the two null terminated strings that need to be compared
   length: maximum number of bytes to compare

Return Value:

  Returns  a value < 0 if string1 less than string2 

  Return   0           if string1 identical to string2 

  Returns  a value > 0 if string1 greater than string2 


Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.1

See Also: 
   STRCMP(),WSTRNCMP()

=======================================================================

Function: STRICMP()

Description: 
   This function is a wrapper around the strcasecmp(), stricmp(), or strcmpi() 
   functions provided by the standard C library. Its behavior is 
   identical to that of strnicmp().

   Compares, case-insensitively, str1 and str2.

Prototype:

   int STRICMP( const char *str1, const char *str2);

Parameters:
   str1,str2: Pointers to the two null terminated strings that need to be compared

Return Value:

  Returns  a value < 0 if string1 less than string2 

  Return   0           if string1 identical to string2 

  Returns  a value > 0 if string1 greater than string2 


Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.1

See Also: 
   STRCMP()

=======================================================================

Function: STRNICMP()

Description: 
   This function is a wrapper around the strncasecmp(), strnicmp(), or strncmpi() 
   functions provided by the standard C library. Its behavior is 
   identical to that of strnicmp().

   Compares, case-insensitively, at most length bytes in str1 and str2.

Prototype:

   int STRNICMP( const char *str1, const char *str2 , size_t length);

Parameters:
   str1,str2: Pointers to the two null terminated strings that need to be compared
   length: maximum number of bytes to compare

Return Value:

  Returns  a value < 0 if string1 less than string2 

  Return   0           if string1 identical to string2 

  Returns  a value > 0 if string1 greater than string2 


Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 2.0

See Also: 
   STRICMP(),WSTRNCMP()


=======================================================================

Function: STRLEN()

Description: 
   This function is a wrapper around the strlen() function provided
by the standard C library. Its behavior is identical to that of strlen().
This function gets the length of the given null teriminated string
This function will always take single-byte character as input. To get the
length of a string comprised of AECHAR character, use WSTRLEN()

Prototype:

   int STRLEN( const char *str);

Parameters:
   str: Null terminated string whose length is to be determined

Return Value:
   Returns the number of characters in string, excluding the terminal NULL. 

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.0

See Also: 
   WSTRLEN()

=======================================================================
Function: STRCHR()

Description: 
   This function is a wrapper around the strchr() function provided
by the standard C library. Its behavior is identical to that of strchr().
This function finds a character in a string.
This function will always take single-byte character as input.

Prototype:

   char *STRCHR( const char *string, int c );

Parameters:
   string: Null terminated string to search
   c:      character to be located


Return Value:

  Returns a pointer to the first occurrence of c in string, 

  NULL if c is not found.

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.0

See Also: 
   WSTRCHR()

======================================================================= 

Function: STRSTR()

Description: 
   Find a the first occurence of a string in another string
   This function is a wrapper around the strstr() function provided
by the standard C library. Its behavior is identical to that of strstr().
This function finds a string in a string.
This function will always take single-byte character as input.

Prototype:

   char *STRSTR( const char *pszHaystack, const char *pszNeedle);

Parameters:
   pszHaystack: string to search
   pszNeedle: string to search for

Return Value:

  Returns a pointer to the first occurrence of pszNeedle in pszHaystack
   
  NULL if pszNeedle is not found

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.1

See Also: 
   STRCHR(), MEMSTR()

======================================================================= 

Function: STRISTR()

Description: 
  Find a the first occurence of a string in another string, 
    case-insensitively.

Prototype:

   char *STRISTR( const char *pszHaystack, const char *pszNeedle);

Parameters:
   pszHaystack: string to search
   pszNeedle: string to search for

Return Value:

  Returns a pointer to the first occurrence of pszNeedle in pszHaystack
  
  NULL if pszNeedle is not found

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.1

See Also: 
   STRSTR()

======================================================================= 

Function: STRBEGINS()

Description: 
  Tell if a string begins with a prefix

Prototype:

   boolean STRBEGINS( const char *pszPrefix, const char *psz);

Parameters:
   pszPrefix: prefix to test for
   psz: string to try

Return Value:

  TRUE if psz begins with pszPrefix

  FALSE otherwise

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.1

See Also:
   None 

======================================================================= 

Function: STRIBEGINS()

This is the case-insensitive version of STRBEGINS()

======================================================================= 

Function: STRENDS()

Description: 
  Tell if a string ends with a suffix

Prototype:

   boolean STRENDS( const char *pszSuffix, const char *psz);

Parameters:
   pszSuffix: suffix to test for
   psz: string to try

Return Value:

  TRUE if psz ends with pszSuffix

  FALSE otherwise

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.1

See Also:
   None 

======================================================================= 

Function: STRLOWER()

Description: 
  Change a single-byte string to all lower case

Prototype:

   char *STRLOWER(char *psz)

Parameters:
   psz: string to downcase

Return Value:
  psz

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.1

See Also: 
   STRUPPER

======================================================================= 

Function: STRUPPER()

Description: 
  change a single-byte string to all upper case

Prototype:

   char *STRUPPER(char *psz)

Parameters:
   psz: string to upcase

Return Value:
  psz

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.1

See Also: 
   STRLOWER

=======================================================================

Function: STRCHREND()

Description: 
   Find the first occurence of a character in a string, return
   a pointer to the end of the string if not found

Prototype:

   char *STRCHREND( const char *string, int c );

Parameters:
   string: Null terminated string to search
   c:      character to be located


Return Value:

 Returns a pointer to the first occurrence of c in string, 

 Or a pointer to the end of the string ('\0') if c is not found.

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.1

See Also: 
   STRCHR()

=======================================================================

Function: STRCHRSEND()

Description: 
   Find the first occurence of a list of characters in a string, return
   a pointer to the end of the string if not found

Prototype:

   char *STRCHRSEND( const char *string, const char *pszChars );

Parameters:
   string: Null terminated string to search
   pszChars: Null terminated string of characters to find


Return Value:

 Returns a pointer to the first occurrence of one of pszChars 
  c in string...

 Or a pointer to the end of the string ('\0') if none are not found.

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.1

See Also: 
   STRCHREND()

=======================================================================

Function: STRRCHR()

Description: 
   This function is a wrapper around the strrchr() function provided
by the standard C library. Its behavior is identical to that of strrchr().
This function searches a string for the last occurance of a character
This function will always take single-byte character as input.

Prototype:

   char *STRRCHR( const char *string, int c );

Parameters:
   string: Null terminated string to search
   c:      character to be located


Return Value:

 Returns a pointer to the last occurrence of c in string, 

 or NULL if c is not found.

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.1

See Also: 
   WSTRRCHR()

=======================================================================
Function: VSNPRINTF()
This function is documented with SPRINTF().
=======================================================================
Function: SNPRINTF()
This function is documented with SPRINTF().
=======================================================================
Function:  VSPRINTF()
This function is documented with SPRINTF().
=======================================================================
Function: SPRINTF()

Description: 
   These functions VSNPRINTF(), SNPRINTF(), VSPRINTF(), and SPRINTF(), 
   write formatted output to a string, according to 
   a set of format specifiers in the cpszFormat argument.

   A format specifier has the form:
===pre>  
     %[FLAGS] [WIDTH] [.PRECISION] [{h | l | L}]TYPE
===/pre>       
   The TYPE character is the only required format field ; it appears 
   after any optional format fields. The type character determines 
   whether the associated argument is interpreted as a character, 
   string, or number. 
===pre>
    Char | Type      | Output Format
   ======|===========|====================================================
     c   |  int      | specifies a single-byte character
     C   |  int      | specifies a wide character
     d   |  int      | Signed decimal integer. 
     i   |  int      | Signed decimal integer. 
     o   |  int      | Unsigned octal integer. 
     u   |  int      | Unsigned decimal integer. 
     x   |  int      | Unsigned hexadecimal integer, using "abcdef".
     X   |  int      | Unsigned hexadecimal integer, using "ABCDEF".
     n   |  int *    | Number of characters successfully written so 
         |           |   far to the stream or buffer; this value is 
         |           |   stored in the integer whose address is 
         |           |   given as the argument. 
     p   |  void *   | Prints the address pointed to by the argument 
         |           |   in the form 0Xxxxxxxxx where xxxxxxxxx is 
         |           |   the address in uppercase hexadecimal digits. 
     s   |  char *   | Specifies a single-byte haracter string. Characters 
         |           |   are printed up to the first null character or 
         |           |   until the precision value is reached. 
     S   |  AECHAR * | Specifies a wide-character string.  Characters 
         |           |   are printed up to the first null character or 
         |           |   until the precision value is reached. 

===/pre>   
   The first optional field of the format specification is FLAGS. A flag 
   directive is a character that justifies output and prints signs, blanks, 
   decimal points, and octal and hexadecimal prefixes. More than one flag 
   directive may appear in a format specification.
===pre>   
   Flag | Meaning                                      | Default 
   =====|==============================================|======================
    -   | Left align the result within the given field | Right align. 
        |   width.                                     |
    +   | Prefix the output value with a sign (+ or -) | Sign appears only for 
        |   if the output value is of a signed type.   | negative signed values
    0   | If width is prefixed with 0, zeros are added | No padding. 
        |   until the minimum width is reached. If 0   |
        |   and - appear, the 0 is ignored. If 0 is    |
        |   specified with an integer format           |
        |    (i, u, x, X, o, d) the 0 is ignored.      |
   ' '  | Prefix the output value with a blank if the  | No blank appears. 
        |   output value is signed and positive; the   |
        |   blank is ignored if both the blank and +   |
        |   flags appear.                              |
    #   | When used with the o, x, or X format, the    | No blank appears. 
        |   # flag prefixes any nonzero output value   |
        |   with 0, 0x, or 0X, respectively.           |
        |   Ignored when used with c, d, i, u, or s.   |
===/pre>   
   The second optional field of the format specification is the WIDTH
   specification. The width argument is a nonnegative decimal integer
   controlling the minimum number of characters printed. If the number
   of characters in the output value is less than the specified width,
   blanks are added to the left or the right of the values (depending
   on whether the - flag (for left alignment) is specified) until the
   minimum width is reached. If width is prefixed with 0, zeros are
   added until the minimum width is reached (not useful for
   left-aligned numbers). The width specification never causes a value
   to be truncated. If the number of characters in the output value is
   greater than the specified width, or if width is not given, all
   characters of the value are printed (subject to the precision
   specification). If the width specification is an asterisk (*), an
   int argument from the argument list supplies the value. The width
   argument must precede the value being formatted in the argument
   list. A nonexistent or small field width does not cause the
   truncation of a field; if the result of a conversion is wider than
   the field width, the field expands to contain the conversion
   result.

   The third optional field of the format specification is
   PRECISION. It specifies a nonnegative decimal integer, preceded by
   a period (.), which specifies the number of characters to be
   printed, the number of decimal places, or the number of significant
   digits (see Table R.5). Unlike the width specification, the
   precision specification can cause either truncation of the output
   value or rounding of a floating-point value. If precision is
   specified as 0 and the value to be converted is 0, the result is no
   characters output, like so:
===pre>   
      SNPRINTF(buf, sizeof(buf), "%.0d", 0 );    // No characters output
===/pre>   
   If the precision specification is an asterisk (*), an int argument
   from the argument list supplies the value. The precision argument
   must precede the value being formatted in the argument list.

   The type determines the interpretation of precision and the default
   when precision is omitted, as shown in Table R.5.

   How PRECISION Values Affect Type
===pre>   
   Type     | Meaning                                | Default 
   =========|========================================|================
   c, C     | The precision has no effect.           | Character is 
            |                                        |   printed. 
   d, i, u, | The precision specifies the minimum    | Default precision 
   o, x, X  |   number of digits to be printed. If   |   is 1. 
            |   the number of digits in the argument |  
            |   is less than precision, the output   | 
            |   value is padded on the left with     | 
            |   zeros. The value is not truncated    | 
            |   when the number of digits exceeds    | 
            |   precision.                           | 
   s, S     | The precision specifies the maximum    | Characters are 
            |   number of characters to be printed.  |  printed until a 
            |   Characters in excess of precision    |  null character 
            |   are not printed.                     |  is encountered.
 ===/pre>   


  The fourth optional field of the format specification are prefixes to TYPE
  that specify the size of the input value.  These prefixes are not supported
  and will be ignored if found in the format string.  Input values are read
  using their base type (eg. %C is of type char, %d is of type int, etc...)

  If any floating point specifier is found anywhere within the format string, 
  these functions return -1 without doing any processing.

Prototype:

   int32 VSNPRINTF(char *buf,uint32 len,const char *cpszFormat,va_list list);

   int32 SNPRINTF(char *buf,uint32 len,const char *cpszFormat, ...);

   int32 VSPRINTF(char *buf,const char *cpszFormat, va_list list);

   int32 SPRINTF(char *buf,const char *cpszFormat, ...);

Parameters:
   buf: Location to write output. For VSNPRINTF and SNPRINTF only, 
        can be null to measure the space of final formatted string.

   len : (VSNPRINTF and SNPRINTF only) size of the buffer, maximum 
         size is 0x7FFFFFFF, ignored if buf is null.

   cpszFormat: Format-control string

   list: Optional argument list (varargs in the case of SNPRINTF(), SPRINTF())

Return Value:
  VSNPRINTF() and SNPRINTF():~
     The number of bytes written to buf, unless buf is NULL, in which case
     the return value is the number of bytes needed to accept the result,
     INCLUDING NULL TERMINATION.

  VSPRINTF() and SPRINTF():~
     The length of the resulting string in buf (NOT INCLUDING NULL 
     TERMINATION.

Comments:  
   VSNPRINTF() and SNPRINTF() guarantee null-termination.

   These functions do not support %f, %F, %e, %E, %g, or %G 
     in the format string.

   Currently BREW doesnt support 64 bit data types for formatted ouput.

Side Effects: 
   None

Version:
   Introduced BREW Client 2.1

See Also: 
   None

=======================================================================
Function: WSTRCPY()

Description: 
   This function copies the src string into dest.
Both src and dest are wide strings (i.e. strings comprising of AECHAR characters)

Prototype:

   AECHAR *WStrCpy(AECHAR *pDest, AECHAR *pSrc)

Parameters:
   pDest: Pointer to destination buffer
   pSrc:  Pointer to null-terminated string that must be copied into pDest


Return Value:
   Returns the destination string

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.0

See Also:
   WSTRNCOPYN(), STRCPY()

=======================================================================
Function: WSTRCAT()

Description: 
   This function appends the src string into dest.
All characters upto and including the first null character in src 
are appended to dest. Both src and dest are wide strings (i.e. strings 
comprising of AECHAR characters)

Prototype:

   AECHAR *WSTRCAT(AECHAR *pDest, AECHAR *pSrc)

Parameters:
   pDest: pointer to the NULL terminated destination string
   pSrc:  pointer to the NULL terminated source string

Return Value:
   Returns the destination string

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.0

See Also: 
   STRCAT()

=======================================================================
Function: WSTRCMP()

Description: 
   This function compares the two strings, s1 and s2, lexicographically. It
   returns an integer value that indicates the comparison result. Note that only
   in this function NULL string pointer and empty string are treated the same.
   Both s1 and s2 are wide strings (i.e. strings comprising of AECHAR characters)

Prototype:

   int WSTRCMP(AECHAR *s1, AECHAR *s2)

Parameters:
   s1: pointer to first NULL terminated string
   s2: pointer to second NULL terminated string

Return Value:
   0 : if s1 == s2

   > 0 : if s1 > s2

   < 0 : if s1 < s2

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.0

See Also: 
   STRCMP()

=======================================================================
Function: WSTRNCMP()

Description: 
   This function compares at most length chars of the two strings, 
   s1 and s2, lexicographically. It returns an integer value that 
   indicates the comparison result. Note that only in this function
   NULL string pointer and empty string are treated the same.
   Both s1 and s2 are wide strings (i.e. strings comprising of AECHAR characters)

Prototype:

   int WSTRNCMP(AECHAR *s1, AECHAR *s2, unsigned int length)

Parameters:
   s1: pointer to first NULL terminated string
   s2: pointer to second NULL terminated string
   length: maximum number of AECHARS to compare

Return Value:
   0   : if s1 == s2

   > 0 : if s1 > s2

   < 0 : if s1 < s2

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.1

See Also: 
   STRNCMP()

=======================================================================

Function: WSPRINTF()

Description: 
   This function is the wide-string equivalent of sprintf(). It
   writes formatted data into a string. This function has certain
   limitations compared to sprintf(). It only handles strings as %s.  
   It does not handle len/format specifiers on strings.
   This function does not support %f in the format string. If %f is found anywhere
   within the format string, this function fills a null character in the starting
   location of the destination string and returns immediately without doing any
   processing.

Prototype:

   void WSPRINTF(AECHAR *pDest, int nSize, AECHAR *pFormat,...)

Parameters:
   pDest: Storage location for output

   nSize: Specifies the total size (in bytes) of pDest buffer

   pFormat: Format control string

For more information about the function or its arguments, please refer
to the documentation SPRINTF().

Return Value:
   None

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.0

See Also: 
   SPRINTF()


=======================================================================
Function: WSTRLEN()

Description: 
   This function gets the length of the given null teriminated 
wide string (i.e. string comprised of AECHAR characters)

Prototype:

   int WSTRLEN(AECHAR * str)

Parameters:
   str: a valid, Null terminated string whose length is to be determined,

Return Value:
   Returns the number of AECHAR characters in string, excluding the 
    terminal NULL.

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.0

See Also: 
   STRLEN()

=======================================================================
Function: WSTRCOMPRESS()

Description: 
   This function compresses the input wide string: if a character is less
   or equal than 127, then reduce the two bytes to one byte.

Prototype:

   void WSTRCOMPRESS(const AECHAR *pSrc, int nLen,byte *pDest, int nSize)


Parameters:
   pSrc: Null terminated source string 
   nLen: Length of the source string in AECHARs
   pDest: Pointer to the destination buffer
   nSize: Size of the destination buffer in bytes

 Return Value:
   None

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.0

See Also: 
  STREXPAND()

=======================================================================
Function: STREXPAND()

Description: 
   This function expands the input string to wide chars
   if a character is less or equal than 127, its width is doubled, chars > 127
   are copied to pDest.

Prototype:

   void STREXPAND(const byte *pSrc, int nCount, AECHAR * pDest, int nSize)


Parameters:
   pSrc: Null terminated source string 
   nCount: Length of the source string in chars
   pDest: Pointer to the destination buffer
   nSize: Size of the destination buffer in bytes

 Return Value:
   None

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.1

See Also: 
   WSTRCOMPRESS()

=======================================================================
  
Function: WSTRCHR()

Description: 
   This function is the wide string counterpart of STRCHR(). Its behavior 
   is identical to that of STRCHR().
   This function will always take double-byte character as input.

Prototype:

   AECHAR *WSTRCHR( AECHAR *s1, AECHAR c );

Parameters:
   s1: Null terminated wide string to search
   c: character to be located


Return Value:

 Returns a pointer to the first occurrence of c in s1, 

 or NULL if c is not found.

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.0

See Also: 
   STRCHR()

=======================================================================
Function: WSTRRCHR()

Description: 
   This function is the wide string counterpart of STRRCHR(). Its behavior 
   is identical to that of STRRCHR().
   This function will always take double-byte character as input.

Prototype:

   AECHAR *WSTRRCHR( AECHAR *s1, AECHAR c );

Parameters:
   s1: Null terminated wide string to search
   c: character to be located


Return Value:

   Returns a pointer to the last occurrence of c in s1, 

   or NULL if c is not found.

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.0

See Also: 
   STRRCHR()

=======================================================================
Function: STRTOWSTR()

Description: 
   This function converts a single-byte string into a wide string

Prototype:

   AECHAR *STRTOWSTR(char *pszIn, AECHAR *pDest, int nSize)

Parameters:
   pszIn: Pointer to null terminated string comprised of single-byte characters
   pDest: Pointer to destination buffer to receive the wide string
   nSide: Size (in bytes) of pDest buffer. If this is 0, this function does
       not do any conversion. It returns pDest as is without any changes.
       Negative values of nSize are invalid.

Return Value:
   Returns the destination string

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.1

See Also: 
   WSTRTOSTR()

=======================================================================

Function: WSTRTOSTR()

Description: 
   This function converts a wide string into a single-byte string

Prototype:

   char *WSTRTOSTR(AECHAR *pIn, char *pszDest, int nSize)

Parameters:
   pIn: Pointer to null terminated wide string that must be converted to
   single-byte character string
   pszDest: Pointer to destination buffer to receive the single-byte string
   nSize: Size (in bytes) of pDest buffer. If this is 0, this function does
      not do any conversion. It returns pszDest as is without any changes.
      Negative values of nSize are invalid.

Return Value:
   Returns the destination string

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.1

See Also: 
   STRTOWSTR()

=======================================================================
Function: WSTRTOFLOAT()

Description: 
   This function converts a wide string into a floating point
value. Internally, this function first converts the given wide string
into a single-byte string using WSTRTOSTR() and then uses the standard
C library function atof() to convert the single-byte string to a double value

Prototype:

   double WSTRTOFLOAT(AECHAR *psz)

Parameters:
   psz: Pointer to null terminated wide string that must be converted to
   float

Return Value:
   Returns the floating point value of the given string

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.1

See Also: 
    FLOATTOWSTR()

=======================================================================

Function: FLOATTOWSTR()

Description: 
   This function converts a floating point value to a wide string.
Internally, this function first converts the given float value into
a single-byte string using the standard C library function sprintf().
It then uses STRTOWSTR() to convert this single-byte string into a
wide string.

Prototype:

   boolean FLOATTOWSTR(double v, AECHAR *psz, int nSize)

Parameters:
   v: Floating point value that must be converted into a wide string
   psz: Pointer to wide-string buffer to hold the resultant wide string
   nSize: size (in bytes) of the psz buffer

Return Value:
   Returns TRUE if successful

   Returns FALSE if failed

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.1

See Also: 
   WSTRTOFLOAT()

=======================================================================
Function: UTF8TOWSTR()

Description: 
   This function converts a converts a UTF8 string to a Wide string

Prototype:

   boolean UTF8TOWSTR(const byte *pSrc,int nLen, AECHAR *pDst, int nSize)

Parameters:
   pSrc: pointer to Null terminated Input string
   nLen: Length of input string in bytes
   pDst: Destination string
   nSize: Size in bytes of destination

Return Value:
   Returns TRUE if successful

   Returns FALSE if failed. 

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.0

See Also: 
   WSTRTOUTF8()

=======================================================================
Function: WSTRTOUTF8()

Description: 
   This function converts a converts a Wide string to UTF8 string.

Prototype:

   boolean WSTRTOUTF8(const AECHAR *pSrc,int nLen, byte *pDst, int nSize)

Parameters:
   pSrc: Input string
   nLen: Length of input string in AECHARs
   pDst: Destination string
   nSize: Size in bytes of destination

Return Value:
  Returns TRUE if successful

  Returns FALSE if failed. 

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.1

See Also: 
   WSTRTOUTF8()

=======================================================================
Function: WSTRLOWER()

Description: 
   This function converts all upper case characters in a wide string 
to lower case

Prototype:

   void WSTRLOWER(AECHAR *pszDest)

Parameters:
   pszDest: On input, this is a pointer to NULL terminated source string. On
   return, this buffer contains the converted string.

Return Value:
   None
   
Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.0

See Also: 
   WSTRUPPER()

=======================================================================
Function: WSTRUPPER()

Description: 
   This function converts all lower case characters in a wide string 
to upper case

Prototype:

   void WSTRUPPER(AECHAR *pszDest)

Parameters:
   pszDest: On input, this is a pointer to NULL terminated source string. On
   return, this buffer contains the converted string.

Return Value:
   None
   
Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.0

See Also: 
   WSTRLOWER()

=======================================================================
Function: GETCHTYPE()

Description: 
   This function returns the type (numeric, alpha, etc.) of a wide 
character

Prototype:

   TChType GETCHTYPE(AECHAR ch)

Parameters:
   ch: Character whose type is to be determined

Return Value:
   returns the type of the given character. The type can be any of:
~
   SC_ALPHA,~
   SC_DIGIT,~
   SC_WHITESPACE or ~
   SC_UNKNOWN.
*
   
Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.0

See Also: 
   None

=======================================================================
Function: CONVERTBMP()

Description: 
   This function converts a Windows bitmap into
   the native format. The native format is specific to each handset. 

   Typical usage of this function: If the user has a raw data buffer containing
   a Windows bitmap, it can be passed to this function to convert it into
   a native format so that the IDISPLAY_BitBlt() function can be used to
   display the image.

   Note:  If pbReallocated is returned as TRUE, then the image was 
   reallocated.  In order to free the data returned by this function, the
   caller must use SYSFREE().  Otherwise, the returned pointer refers to
   the memory passed into this functions via the pSrcBuffer parameter.

Prototype:

   void *CONVERTBMP(void *pSrcBuffer, AEEImageInfo *pii, boolean *pbRealloc)

Parameters:
   pSrcBuffer  [in]: Pointer to a buffer containing the Windows bitmap. One
   of the ways of constructing this is to read the contents of a Windows .bmp
   file into a memory buffer. This buffer is converted to native format on
   return.

   pii:       [out]: On return, the AEEImageInfo struct pointed to by this
   parameter contains information about the converted image (such as
   width, height, color depth, etc).

   pbRealloc: [out]: On return, this is set to TRUE or FALSE depending on
   whether or not the CONVERTBMP() function did separate memory allocation for
   the buffer that is returned from this function. If this is set to TRUE, the
   caller must release the buffer returned from this function using SYSFREE()
   after the buffer is used.

Return Value:
   If successful, returns a valid buffer containing the converted image. This
   is the incoming image (pSrcBuffer) converted into native format. If pbRealloc
   is set to true on return, the caller must release this buffer using SYSFREE()
   after using the buffer.

   If fail, returns NULL

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.0

See Also: 
   None

=======================================================================

Function: CREATEOBJ()

Description: 
   This is an alias of MALLOC().

Prototype:

   void *CREATEOBJ(dword dwSize)

Parameters:
   dwSize: [in]: Specifies the size (in bytes) that must be allocated

Return Value:
   If successful, returns a valid pointer pointing to a buffer
   of size dwSize bytes.

   If failed, returns NULL
   
Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.0

See Also: 
   MALLOC()

=======================================================================

Function: FREEOBJ()

Description: 
   This is an alias of FREE().

Prototype:

   void FREEOBJ(void *pObj)

Parameters:
   pObj: [in]: Pointer to the memory buffer that must be release. This
   buffer must have been allocated using CREATEOBJ()

Return Value:
   None   

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.0

See Also: 
   FREE()

=======================================================================

Function: MALLOCREC()

Description: 
   This is an alias of MALLOC() useful for avoiding typecasts.  This
   function zero-initializes the memory that it returns.

Prototype:

   type *MALLOCREC(type)

Parameters:
   type: a C type definition, suitable for casting and taking sizeof()

Return Value:
   If successful, returns a valid pointer pointing to a buffer
   of size sizeof(type) bytes.

   If failed, returns NULL
   
Comments:  
   Here's how it is used.  Instead of like this:

      pMe = (struct MeStruct *)MALLOC(sizeof(struct MeStruct));

  you get this:

      pMe = MALLOCREC(MeStruct);

  By placing the (dangerous) cast where the sizeof is taken, this 
  avoids mistakes like:

      pMe = (struct MeStruct *)MALLOC(sizeof(struct TypoStruct));
  
  which could cause over-allocation or a crash (depending on the sizeof 
  TypoStruct).

  It's also a little more concise.

Side Effects: 
   None

Version:
   Introduced BREW Client 1.1

See Also: 
   MALLOC()

=======================================================================

Function: MALLOCREC_EX()

Description: 
  This is an alias of MALLOC() useful for avoiding typecasts and getting 
  extra memory, thereby avoiding multiple heap nodes.  This function
  zero-initializes the memory that it returns.

Prototype:

   void *MALLOCREC_EX(type,size)

Parameters:
   type: a C type definition, suitable for casting and taking sizeof()
   size: number of extra bytes

Return Value:
   If successful, returns a valid pointer pointing to a buffer
   of size sizeof(type) + size bytes.

   If failed, returns NULL
   
Comments:  
   Here's how it is used.  Assume a struct like this:
 
      typedef struct NVPair
      {
          char *pszName;
          char *pszValue;

      } NVPair;

   and a creation function like:
     
     NVPair *NVPair_New(const char *pszName, const char *pszValue);

   you *could* do something like:

     NVPair *NVPair_New(const char *cpszName, const char *cpszValue)
     {
        NVPair *me;

        me = MALLOCREC(NVPair);

        if (me) {
            me->pszName = STRDUP(cpszName);
            if (me->pszName) {
               me->pszValue = STRDUP(cpszValue);
               if (me->pszValue) {
                   return me; // success! 
               }
               FREE(me->pszName);
            }
            FREE(me);
        }
        return 0;
     }

  or do this:

     NVPair *NVPair_New(const char *cpszName, const char *cpszValue)
     {
        NVPair *me;

        me = MALLOCREC_EX(NVPair, STRLEN(cpszName) + STRLEN(cpszValue) + 2);

        if (me) {
           me->pszName  = (char *)(me + 1);
           STRCPY(me->pszName,cpszName);
           
           me->pszValue = me->pszName + STRLEN(me->pszName) + 1;
           STRCPY(me->pszValue,cpszValue);
        }

        return me;
     }

   the former takes 3 heap nodes, the latter only 1.  Freeing is also 
   simplified, here's for the plain example:

     NVPair_Delete(NVPair *me)
     {
        FREE(me->pszValue);
        FREE(me->pszName);
        FREE(me);
     }
  
  and using MALLOCREC_EX it'd be:
    
     NVPair_Delete(NVPair *me)
     {
        FREE(me);
     }


Side Effects: 
   None

Version:
   Introduced BREW Client 1.1

See Also: 
   MALLOCREC()

=======================================================================

Function: FREEIF()

Description: 
   This is an alias of FREE(), with a null-pointer guard.

Prototype:

   void FREEIF(void *p)

Parameters:
   p: [in]: pointer to free
   buffer must have been allocated using MALLOC or analogue

Return Value:
   None   

Comments:  
   This macro's just here to help you avoid double-frees

Side Effects: 
    p is set to NULL

Version:
   Introduced BREW Client 1.1

See Also: 
   FREE()

=======================================================================

Function: ZEROAT()

Description: 
   MEMSET wrapper for zeroing structs

Prototype:

   void ZEROAT(ptr)


Parameters:
   ptr: [in]: pointer to zero out

Return Value:
   None

Comments:  
   This macro's just here to help you not have to remember the order of 
   arguments to MEMSET

Side Effects: 
   None

Version:
   Introduced BREW Client 1.1

See Also: 
   MEMSET()


=======================================================================
Function: WSTRDUP()

Description: 
   This is a convenience function that can be used
make a duplicate copy of an existing wide string. This function first
allocates memory required using MALLOC() to store a copy of the
incoming string and then copies this string into the newly allocated buffer.
This newly allocated buffer is then returned from the function. 
After use, the buffer must be released using FREE().

Prototype:

   AECHAR *WSTRDUP(const AECHAR *pIn)

Parameters:
   pIn: [in]: A valid pointer to a null-terminated wide string.

Return Value:
   If successful, returns a pointer containing a copy of the incoming
   string pIn. After using this pointer, it must be freed using
   FREE().

   If failed, returns NULL.

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.0

See Also: 
   None

===========================================================================
Function: WWRITELONG()

Description:
   Write decimal value of n as zero-terminated string.

Prototype:

   AECHAR *WWRITELONG(AECHAR *pszBuf, long n);

Parameters:
   pszBuf: [out]: A valid pointer to a null-terminated wide string to which the
   converted long is written
   n: [in]: input long number

Return Value:
   Returns pointer to the terminating NULL character.

Comments:  
   pszBuf[] must have room for at least 12 characters ('-' + 10 digits + '\0')

Side Effects: 
   None

Version:
   Introduced BREW Client 2.0

See Also: 
   WSPRINTF()
   WWRITELONGEX()

=====================================================================
Function: WWRITELONGEX()

Description: 
   This function converts a long into wide string. The string may be padded
   with wide character '0' and may contain only a portion of the long input 
   value based on the parameters nPad and nRemaining. 

Prototype:

   AECHAR *WWRITELONGEX(AECHAR *pszBuf, long n, int nPad, int *pnRemaining);

Parameters:
   pszBuf: [out]: A valid pointer to a null-terminated wide string to which the
   converted long is written
   n: [in]: input long number
   nPad: [in]: specifies the maximum amount of padding. It must be less than 12
   otherwise it is ignored. It specifies the number of digits that must be 
   present in the final string. For ex: if n is set to 245 and nPad is set to 5,
   then the output string shall contain 00245. 
   nRemaining: [in/out]: On input, this pointer points to an integer that
   specifies the size of pszBuf. This is specified in terms of total number of
   characters (AECHARs) including the padding, that can be written to the string. 
   On output, this points to an integer that denotes the unused space (in terms
   of number of ARCHARs) left in pszBuf. The difference between the output and
   input values denotes the number of characters written to pszBuf (excluding
   the null character).

Return Value:
   If successful, returns a pointer to the beginining of the unused portion of
   pszBuf. This is the portion in pszBuf after the formatted wide string has
   been written.
   
   If failed, returns NULL.

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.0

See Also: 
   WSPRINTF()
   WWRITELONG()

=====================================================================
Function: WSTRSIZE()

Description: 
   This function returns the size, in bytes, of the wide string 

Prototype:

   int WSTRSIZE(AECHAR *p);

Parameters:
   p:[in]: A valid pointer to a null-terminated wide string

Return Value:
   If successful, returns the size, in bytes, of wide string, including null
      termination, unless the string is empty, in which case returns 0.
   
   For example:
===pre>
     WSTRSIZE("") == 0       (empty string)
     WSTRSIZE("h") == 4      ( ('h' + '\0') * sizeof(AECHAR) )
===/pre>

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.0

See Also: 
   None

=====================================================================
Function: WSTRNCOPYN()

Description: 
   This function copies specified length of the source string into destination.
   The destination is guaranteed to be null-terminated.
   Both src and dest are wide strings (i.e. strings comprising of AECHAR
   characters)

Prototype:

   int WSTRNCOPYN(AECHAR *pDest, int cbDest, AECHAR *pSrc, int lenSource)

Parameters:
   pDest [out]: Pointer to destination buffer
   cbDest: [in]: Size of pDest in AECHARs.
   pSrc: [in]: Pointer to null-terminated string that must be copied into pDest
   lenSource: [in]: the maximum string length to copy into pDest. It does not
   need to include null character. If it is set to -1, then the entire pSrc is
   copied to pDest

Return Value:

   Returns the number of AECHARs copied

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.0

See Also:
   WSTRCPY()

=======================================================================
Function: OEMSTRSIZE()

Description: 
   This function returns the size, in bytes, of the byte string which must 
   be null-terminated

Prototype:

   int OEMSTRSIZE(byte *p);

Parameters:
   p:[in]: A valid pointer to a null-terminated byte string

Return Value:
   If successful, returns the size, in bytes, of byte string

   If p is NULL, returns 0

Comments:  
   If successful, returns the size, in bytes, of the string, including null
      termination, unless the string is empty, in which case returns 0.
   
   For example:
===pre>
     OEMSTRSIZE("") == 0       (empty string)
     OEMSTRSIZE("h") == 2      ('h' + '\0')
===/pre>

Side Effects: 
   None

Version:
   Introduced BREW Client 1.0

See Also: 
   None

=====================================================================
Function: OEMSTRLEN()

Description: 
   This function returns the length of byte string. The string must 
   be null-terminated. If the first byte of a character is greater than 0x7f, it
   is treated as a 2-byte character and the two bytes will be counted has one
   single character.

Prototype:

   int OEMSTRLEN(byte *p);

Parameters:
   p:[in]: A valid pointer to a null-terminated byte string

Return Value:
   If successful, returns the length of byte string

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.0

See Also: 
   None

=====================================================================
Function: GETAEEVERSION()

Description: 
   This function retrieves information about the version number of the current 
   BREW software being used. The version number is stored in w.x.y.z format where
   w: denotes Major Version Number
   x: denotes Minor Version Number
   y: denotes a Sub-version number
   z: denotes a build number
   This function also places the information in the incoming string pszString. 
   On the phone, this function can also be used to determine the chipse on which
   the BREW software is running.
  
Prototype:

   uint32   GETAEEVERSION(byte *pszString, int nSize, uint16 wFlags)

Parameters:
   pszstring[in]: Pointer to a buffer where the string containing the version
   number shall be placed. It can be of the form "1.0.0.15" or
   "1.0.0.15(MSM3100)" depending on whether or not the MSM information is
   requested.

   nsize [in]: size of the pszString buffer in bytes
   wFlags [in]: Specifies the type of information to retrieve. It can be a
   combination of one or more of the following flags:

   GAV_LATIN1: pszString is returned as a Single byte string
   GAV_MSM: pszString contains the MSM chip number in addition to the build number. 
   Ex: 1.0.0.15 (MSM3100). This flag is not supported on the SDK. It is supported
   only in the phone environment.
   GAV_UPDATE: pszString contains the patch version information along with
   version information separated by a space. This flag can be used along with
   GAV_LATIN1 flag. 
   Ex: 3.1.2.3 P1,P2,P5,P7
   If the string passed to GETAEEVERSION is not big enough to store the full
   version and patch information, then nothing is set in the pszString.

Return Value:
   A 32 bit number containing the version number. The information is organized
   as follows:
~
   Hi Byte of Hi Word: Major Version Number~
   Low Byte of Hi Word: Minor Version Number~
   Hi Byte of Lo Word: Sub-Version Number~
   Lo Byte of Lo Word: Build Number
*

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.0

See Also:
   None
   
=====================================================================
Function: AEE_IS_PATCH_PRESENT()

Description: 
This function is used to determine if a particular BREW patch is applied 
to the device or not. A return value of SUCCESS indicates that the patch
indicated in the argument to the function is applied to the device.
The function can also be used to determine if any patch is applied at all
by passing a NULL argument to the function. A return value of SUCCESS
indicates that there is/are patches applied to the device.

Prototype:

int  AEE_IS_PATCH_PRESENT(const char* pszPatchToLookFor)

Parameters:
pszPatchToLookFor[in]: Pointer to a string which is the patch being queried for.
e.g. p = "P1"


Return Value:
SUCCESS - The patch specified is applied OR in case of NULL argument there are
          some patches applied to the system.
EFAILED - The patch is not applied OR in the case of NULL argument there is no
          patch applied to the system.
ENOMEMORY - There was not enough memory available to do the operation.

Comments:  
Applicability of only one patch at a time can be determined using this function.

Side Effects: 
None

Version:
Introduced BREW Client 3.1.3

See Also:
None

=====================================================================
Function: LOCALTIMEOFFSET()

Description: 
This function returns the local timezone offset from UTC Zulu, in seconds.
Optionally returns a flag indicating that Daylight Savings time is active
(if it is, the value of the local timezone offset already takes the shift 
into account; the flag is just for controlling display of a timezone name
if desired).  The returned value is added to UTC Zulu to give local time.

  local time = UTC Zulu + local time offset 

Note that the value returned by this call may depend on the device acquiring
system time.  Use ISTIMEVALID() or IDeviceNotifier to determine if system 
time has been acquired.

Prototype:

   int32 LOCALTIMEOFFSET(boolean *pbDaylightSavings);

Parameters:
   pbDaylightSavings: [in/out]: If non NULL on input, this flag specifies
   whether or not Daylight Savings time is active.

Return Value:
   Returns the local timezone offset from UTC Zulu in seconds.

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.0

See Also:
   GETTIMESECONDS
   GETUTCSECONDS
   GETJULIANDATE
   LOCALTIMEOFFSET
   To take timing measurements, use GETUPTIMEMS
   
=====================================================================
Function: SYSFREE()

Description: 
This function is provided to allow developers to returned system memory
pointers returned from CONVERTBMP().

Note:  This function is ONLY valid for memory objects returned by CONVERTBMP
where the "bReallocated" flag has been set to TRUE.

Prototype:

   void SYSFREE(void *pBuff);

Parameters:
   pBuff:  Pointer to memory allocated by CONVERTBMP

Return Value:
   None

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.0

See Also:
   None 

=====================================================================
Function: GETALSCONTEXT()

Description: 
This function returns the application local storage context of the
currently executing applet.  It is valid during all BREW API calls,
callbacks and events.  To obtain a ILocalStorage object query context
interface for AEEIID_LOCALSTORAGE.

Prototype:

   IQueryInterface *GETALSCONTEXT(void);

Parameters:
   None

Return Value:
  Returns the pointer to the currently executing applet's local
  storage context.

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 3.1.5

See Also:
   ILocalStorage

=====================================================================
Function: GETAPPINSTANCE()

Description: 
This function returns the IApplet instance of the currently executing
applet.  It is valid during all BREW API calls, callbacks and events.

Prototype:

   IApplet *GETAPPINSTANCE(void);

Parameters:
   None

Return Value:
  Returns the IApplet pointer to the currently executing applet.

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.0

See Also:
   None 

=====================================================================
Function: STRTOUL()

Description: This function is a wrapper around the standard C library
function strtoul(). It converts strings to an unsigned long-integer value.
It stops reading the string nptr at the first character it cannot recognize 
as part of a number. This may be the terminating null character, or it may 
be the first numeric character greater than or equal to base. 
If endptr is not NULL, a pointer to the character that stopped the scan 
is stored at the location pointed to by endptr. If no conversion can be performed 
(no valid digits were found or an invalid base was specified), the value of 
nptr is stored at the location pointed to by endptr.


Prototype:

   uint32  STRTOUL( const char *nptr, char **endptr, int base );

Parameters:
   nptr: [in]: Null-terminated string to convert 

   endptr: [out]: If Non-null in input, it points to character that stops scan on return

   base: [in]: Number base to use


Return Value:
  Returns the converted value. 
  
  or returns 0 if no conversion can be performed. 

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.0

See Also:
   None
   
=====================================================================
Function: STRNCPY()

Description: This function is a wrapper around the standard C library
function strncpy(). It copies specified number of characters from one 
string to another.

The strncpy function copies the initial count characters of strSource 
to strDest and returns strDest. If count is less than or equal to the 
length of strSource, a null character is not appended automatically to 
the copied string. If count is greater than the length of strSource, 
the destination string is padded with null characters up to length count. 
The behavior of strncpy is undefined if the source and destination strings 
overlap.


Prototype:

   char *STRNCPY( char *strDest, const char *strSource, size_t count );


Parameters:
   
   strDest: [in/out]: Destination string

   strSource: [in/out]: Source string

   count: [in]: Number of characters to be copied

Return Value:
  
   Returns strDest. 


Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.0

See Also:
   None

=====================================================================

Function: GETRAND()

Description: 
   Generate random bytes.

   NOT SUITABLE FOR CRYPTOGRAPHIC USE!

Prototype:

   void GETRAND(byte *pDest, int nSize)

Parameters:
   pDest: where to stick the random bytes
   nSize: size in bytes of pDest

Return Value: 
   None

Comments:  
   GETRAND is not suitable for cryptographic use - i.e. key generation.
   IRANDOMSOURCE should be used for this purpose.

   GETRAND is a linear congruent PRNG that is seeded using time.
   This makes the ouput predictable if you can determine (or influence)
   the seed value used. Furthermore, two devices will produce the SAME 
   random sequence if both invoke GETRAND() at the same time.

Side Effects: 
   None

Version:
   Introduced BREW Client 1.0

See Also: 
   None

=====================================================================

Function: GETTIMEMS()

Description: 
   This method returns the number of milliseconds that have elapsed
   since the last occurrence of 00:00:00 local time.

   Note that the value returned by this call depends on the device's 
   current time of day value.  This value is obtained from the base station.
   As such this value may change dramatically when the phone first acquires
   system time.  Use ISTIMEVALID() or IDeviceNotifier to determine if system 
   time has been acquired.

Prototype:

   uint32 GETTIMEMS();

Parameters:
   None

Return Value: 
   32-bit millisecond value

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.0

See Also: 
   GETTIMESECONDS
   GETUTCSECONDS
   GETJULIANDATE
   LOCALTIMEOFFSET
   To take timing measurements, use GETUPTIMEMS

======================================================================= 

Function: GETUPTIMEMS()

Description: 
   This method returns a continuously and linearly increasing millisecond
   timer from the time the device was powered on.  Note that unlike GetTimeMS,
   this value will not change dramatically due to initial acquisition of
   system coverage.

Prototype:

   uint32 GETUPTIMEMS();

Parameters:
   None

Return Value: 
   32-bit millisecond value

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.1

See Also: 
   To get time of day, use: GETTIMESECONDS, GETUTCSECONDS, GETJULIANDATE
   LOCALTIMEOFFSET

======================================================================= 

Function: GETTIMESECONDS()

Description:
   This method returns the number of seconds since 1980/01/06 00:00:00 UTC,
   including UTC leap second adjustments and adjusted for local time zone
   and daylight savings time.
   
   GETTIMESECONDS() = GETUTCSECONDS() + LOCALTIMEOFFSET()
   
   Note that the value returned by this call depends on the device's 
   current time of day value.  This value is obtained from the base station.
   As such this value may change dramatically when the phone first acquires
   system time.  Use ISTIMEVALID() or IDeviceNotifier to determine if system 
   time has been acquired.

Prototype:

   uint32 GETTIMESECONDS();

Parameters:
   None

Return Value: 
   32-bit seconds value

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.1

See Also: 
   GETTIMESECONDS
   GETUTCSECONDS
   GETJULIANDATE
   LOCALTIMEOFFSET
   To take timing measurements, use GETUPTIMEMS

======================================================================= 

Function: GETUTCSECONDS()

Description: 
   This method returns the number of seconds since 1980/01/06 00:00:00 UTC

   Note that the value returned by this call depends on the device's 
   current time of day value.  This value is obtained from the base station.
   As such this value may change dramatically when the phone first acquires
   system time.  Use ISTIMEVALID() or IDeviceNotifier to determine if system 
   time has been acquired.

Prototype:

   uint32 GETUTCSECONDS();

Parameters:
   None

Return Value: 
   32-bit seconds value

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 3.1

See Also: 
   GETTIMESECONDS
   GETUTCSECONDS
   GETJULIANDATE
   LOCALTIMEOFFSET
   To take timing measurements, use GETUPTIMEMS

======================================================================= 

Function: GETJULIANDATE()

Description: 
   This method fills a Julian date structure based upon the input second 
   value.  This value represents the number of seconds since
   1980/01/06 00:00:00 UTC. If the input value is 0, GETTIMESECONDS()
   is used.

Prototype:

   GETJULIANDATE(uint32 dwSecs, JulianType *pDate);

Parameters:
   dwSecs: Seconds
   pDate:  Pointer to the structure that needs to be filled on return.

Return Value: 
   None

Comments:  
   Upper limit for dwSecs is difference between MAX for uint32 and 
   difference between phone reference time 1980/01/06 00:00:00 UTC
   and PC reference time 1970/01/01 00:00:00 UTC.

Side Effects: 
   None

Version:
   Introduced BREW Client 1.0

See Also: 
   GETTIMESECONDS
   GETUTCSECONDS
   GETJULIANDATE
   LOCALTIMEOFFSET
   To take timing measurements, use GETUPTIMEMS

======================================================================= 

Function: INET_ATON()

Description: 
   convert a string into an InetAddr (uint32)

Prototype:

   boolean INET_ATON(const char *psz, uint32 *pulAddr);

Parameters:
   const char *psz: string containing the addr (like "1.2.3.4")
   uint32 *pulAddr: pointer to the inet address to be filled

Return Value: 
   TRUE if the address could be parsed, FALSE otherwise

Comments:
   INET_ATON accepts '\0', ':', '/' and ',' as string terminators, which
   are common URL hostname delimiters.  Thus, these are all valid strings 
   to pass to INET_ATON():

       "1.2.3.4"
       "1.2.3.4:80"
       "1.2.3.4/hi"
       "1.2.3.4,1.2.3.5"

   And all return the same ulAddr, namely 1.2.3.4

   The answer is in network byte order, so can be used directly with 
   network APIs

Side Effects: 
   None

Version:
   Introduced BREW Client 1.1

See Also:
   None

======================================================================= 

Function: INET_NTOA()

Description: 
   convert an InetAddr (uint32) into an ascii string

Prototype:

   void INET_NTOA(uint32 ulAddr, char *pc, int nLen);

Parameters:
   uint32 ulAddr: the inet address to converted
   char *pc: buffer to write into
   int nLen: size of pc

Return Value: 
   None

Comments:
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.1

See Also:
   None

====================================================================== 
NTOHL()
This function is documented with HTONL().
======================================================================= 

Function: HTONL()

Description: 
    HTONL()and NTOHL() convert a host byte order unsigned long to network byte
    order, and vice-versa

Prototype:

   uint32 HTONL(uint32 ul);

   uint32 NTOHL(uint32 ul);

Parameters:
   uint32 ul: the long to convert

Return Value: 
   the converted value

Comments:
   if the platform is big-endian this function returns ul unmolested

Side Effects: 
   None

Version:
   Introduced BREW Client 1.1

See Also:
   None

====================================================================== 
NTOHS()
This function is documented with   HTONS().
======================================================================= 

Function: HTONS()

Description: 
 HTONS() and NTOHS() convert a host byte order uint16 to network byte order, and
   vice-versa

Prototype:

   uint16 HTONS(uint16 us);

   uint16 NTOHS(uint16 us);

Parameters:
   uint16 us: the short to convert

Return Value: 
   the converted value

Comments:
   if the platform is big-endian this function returns us unmolested

Side Effects: 
   None

Version:
   Introduced BREW Client 1.1

See Also:
   None

======================================================================= 
LETOHL()
This function is documented with  HTOLEL().
====================================================================== 
Function: HTOLEL()

Description: 
   The functions HTOLEL() and  LETOHL() convert a host byte order unsigned long
   to little-endian byte order, and vice-versa.

Prototype:

   uint32 HTOLEL(uint32 ul);

   uint32 LETOHL(uint32 ul);

Parameters:
   uint32 ul: the long to convert

Return Value: 
   the converted value

Comments:
   if the platform is little-endian this function returns ul unmolested

Side Effects: 
   None

Version:
   Introduced BREW Client 1.1

See Also:
   None

======================================================================= 
LETOHS()
This function is documented with  HTOLES().
====================================================================== 
Function: HTOLES()

Description: 
   The functions HTOLES() and  LETOHS() convert a host byte order uint16 to little-endian byte order, and
   vice-versa.

Prototype:

   uint16 HTOLES(uint16 us);

   uint16 LETOHS(uint16 us);

Parameters:
   uint16 us: the short to convert

Return Value: 
   the converted value

Comments:
   if the platform is little-endian this function returns us unmolested

Side Effects: 
   None

Version:
   Introduced BREW Client 1.1

See Also:
   None

======================================================================= 

Function: JULIANTOSECONDS()

Description: 
   Returns number of seconds elapsed since CDMA reference time equivalent
	of the specified julian date time.

Prototype:
	uint32		  JULIANTOSECONDS(JulianType* pDate)

Parameters:
   pDate: pointer to julian date time structure

Return Value: 
   Number of seconds elapsed since CDMA reference time equivalent
	of the specified julian date time.

Comments:
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 2.0

See Also:
   GETJULIANDATE()
   JulianType
======================================================================= 

Function: STRLCPY()

Description: 
   Copy a string into a destination buffer and guarentee that it is NULL 
   terminated.

Prototype:

   size_t STRLCPY(char * dst, const char * src, size_t nSize);

Parameters:
   dst:     Destination string. String to copy into.
   src:     Source string. String to copy from.
   nSize:   Size of destination buffer in bytes.

Return Value: 
   Length of the full string that was attempted to be copied.
   (size_t)-1 for an error.

Comments:
   To see if truncation occurred, you can check the return value against
   the length of the destination buffer.

Side Effects: 
   None

Version:
   Introduced BREW Client 2.0

See Also:
   STRCPY(), STRNCPY()

======================================================================= 

Function: STRLCAT()

Description: 
   Concatenate a string into a destination buffer and guarentee that it is NULL 
   terminated.

Prototype:

   size_t STRLCAT(char * dst, const char * src, size_t nSize);

Parameters:
   dst:     Destination string. String to concatenate into.
   src:     Source string. String to copy from.
   nSize:   Size of destination buffer in bytes.

Return Value: 
   Length of the full string that was attempted to be created as a result
   of the concatenation.
   (size_t)-1 for an error.

Comments:
   To see if truncation occurred, you can check the return value against
   the length of the destination buffer.

Side Effects: 
   None

Version:
   Introduced BREW Client 2.0

See Also:
   STRCAT

======================================================================= 

Function: WSTRLCPY()

Description: 
   Copy a wide string into a destination buffer and guarentee that it is NULL 
   terminated.

Prototype:

   size_t WSTRLCPY(AECHAR * dst, const AECHAR * src, size_t nSize);

Parameters:
   dst:     Destination string. String to copy into.
   src:     Source string. String to copy from.
   nSize:   Size of destination buffer in AECHAR.

Return Value: 
   Length of the full string that was attempted to be copied.
   (size_t)-1 for an error.

Comments:
   To see if truncation occurred, you can check the return value against
   the length of the destination buffer.

Side Effects: 
   None

Version:
   Introduced BREW Client 2.0

See Also:
   WSTRCPY(), WSTRNCOPYN(), STRLCPY()

======================================================================= 

Function: WSTRLCAT()

Description: 
   Concatenate a wide string into a destination buffer and guarentee that it is NULL 
   terminated.

Prototype:

   size_t WSTRLCAT(AECHAR * dst, const AECHAR * src, size_t nSize);

Parameters:
   dst:     Destination string. String to concatenate into.
   src:     Source string. String to copy from.
   nSize:   Size of destination buffer in AECHAR.

Return Value: 
   Length of the full string that was attempted to be created as a result
   of the concatenation.
   (size_t)-1 for an error.

Comments:
   To see if truncation occurred, you can check the return value against
   the length of the destination buffer.

Side Effects: 
   None

Version:
   Introduced BREW Client 2.0

See Also:
   WSTRCAT(), STRCAT()

======================================================================= 

Function: SETSTATICPTR()

Description: 
   set a pre-defined static pointer in BREW, return old value

Prototype:

   void *SETSTATICPTR(int nSPId, void *pNew)

Parameters:
   nSPId: the id of the static pointer, one of:

             AEESPID_STACKBOTTOM: the bottom of the current stack, to be used
                                  with great caution, as BREW uses this value
                                  internally for stack checking.  set this
                                  value *only* if you're implementing a 
                                  threading package and you want BREW to 
                                  be tolerant of your stack

             AEESPID_TLS: a pointer to thread "local" data, this value *must*
                          be on the current stack, i.e. greater than SP when 
                          any BREW API is called, as this value is *also* used
                          by BREW for stack checking, as the "top" of the 
                          stack

   pNew: the new value

Return Value: 
   The old value

Comments:
   Only pre-defined ids are supported, calling this function with anything
   else will generate an exception.  Use with caution.

Side Effects: 
   AEESPID_STACKBOTTOM and AEESPID_TLS are used by BREW to do stack checking. 
   For this reason, only user-thread packages or BREW system code should 
   normally call setstaticptr with these Ids

Version:
   Introduced BREW Client 2.0

See Also:
   SETTLS()
   SETSTACKBOTTOM()
   GETTLS()

======================================================================= 

Function: SETSTACKBOTTOM()

Description: 
   set the pre-defined static pointer to the stack bottom in BREW, 
   return old value

Prototype:

   void *SETSTACKBOTTOM(void *pNew)

Parameters:
   pNew: the new value

Return Value: 
   The old value

Comments:
   use with caution, only threading packages or system-level code should
   need to call this function

Side Effects: 
   BREW uses this value to do stack underflow checking

Version:
   Introduced BREW Client 2.0

See Also:
   SETTLS()
   SETSTATICPTR()
   GETTLS()

======================================================================= 

Function: SETTLS()

Description: 
   get a pre-defined static pointer to a TLS record in BREW, return old value

Prototype:

   void *SETSTLS(void *pNew)

Parameters:
   pNew: the new value

Return Value: 
   The old value

Comments:
   use with caution, only threading packages or system-level code should
   need to call this function

Side Effects: 
   BREW uses this value to do stack overflow checking, any function that
   calls SETTLS() should call SETTLS() to restore the stack top before 
   returning

Version:
   Introduced BREW Client 2.0

See Also:
   SETSTATICPTR()
   SETSTACKBOTTOM()
   GETTLS()

======================================================================= 

Function: GETTLS()

Description: 
   handy macro to just get the pre-defined static pointer to a TLS record 
   in BREW

Prototype:

   void GETTLS(void *pToSet)

Parameters:
  pToSet: this is a macro, fills in pToSet

Return Value: 
   None

Comments:
   None

Side Effects: 
   pToSet is filled in

Version:
   Introduced BREW Client 2.0

See Also:
   SETTLS()
   SETSTACKBOTTOM()
   SETSTATICPTR()

======================================================================= 

Function: WGS84_TO_DEGREES()

Description: 
   This function converts latitude or longitude in WGS84 format to
   Degree decimal format. This function may be used to convert the latitude
   and longitude received as a result of calls to IPOSDET_GetGPSInfo.

Prototype:
   
   double WGS84_TO_DEGREES(int32 latlon)

Parameters:
   latlon: Latitude or Longitude expressed in WGS-84 coordinates

Return Value:
   Returns latitude or longitude in degree decimal format.

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 2.1

See Also: 
   None

======================================================================= 

Function: ISBADREADPTR()

Description: 
   This function returns TRUE if the buffer pBuf of length len is not 
   readable by the caller.

Prototype:
   
   boolean ISBADREADPTR(void *pBuf, uint32 len)

Parameters:
   pBuf: Pointer to the buffer which needs to be verified for read access
   len : Length (in  bytes) of the buffer that starts at pBuf.

Return Value:
   Returns FALSE if buffer is readable and TRUE otherwise.

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 3.1

See Also: 
   ISBADWRITEPTR()

======================================================================= 

Function: ISBADWRITEPTR()

Description: 
   This function returns TRUE if the buffer pBuf of length len is not 
   writeable by the caller.

Prototype:
   
   boolean ISBADWRITEPTR(void *pBuf, uint32 len)

Parameters:
   pBuf: Pointer to the buffer which needs to be verified for write access
   len : Length (in  bytes) of the buffer that starts at pBuf.

Return Value:
   Returns FALSE if buffer is writeable and TRUE otherwise.

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 3.1

See Also: 
   ISBADREADPTR()

======================================================================= 

Function: BASENAME()

Description: 
   Returns the filename component of a path.

Prototype:
   const char *BASENAME(const char *cpszPath)

Parameters:
   cpszPath: the path to be decomposed

Return Value:
   Returns the filename part of cpszPath.

Comments:  
   A trailing '/' is NOT ignored.

   The following list of examples shows the strings returned by
   BASENAME for different paths:
===pre>
   cpszPath       BASENAME()
   "/x"           "x"
   "/x/"          ""
   "x"            "x"
   "/x/y"         "y"
   "/"            ""
===/pre>
Side Effects: 
   None

Version:
   Introduced BREW Client 3.1

See Also: 
   None


======================================================================= 

Function: MAKEPATH()

Description: 
   builds a path, given a directory and filename, inserting the 
    directory delimiter '/' if necessary.

Prototype:
   int MAKEPATH(const char *cpszDir, const char *cpszFile,
                char *pszOut, int *pnOutLen);

Parameters:
   cpszDir: the null terminated directory
   cpszFile: file part of the path
   pszOut: destination buffer into which to write the path, may be null
   pnOutLen [in/out]: on input - size in bytes of non-null pszOut, 
                     if pszOut is null, then ignored on input.
       on output, holds number of bytes written to pszOut,
       including null-termination, if pszOut is null, then holds number of
       bytes needed to make the path

Return Value:
   SUCCESS on SUCCESS

   EBUFFERTOOSMALL if pszOut overflows

Comments:  
   The following list of examples shows the strings returned by
   MAKEPATH() for different paths:
===pre>
   cpszDir  cpszFile  MAKEPATH()
   ""        ""           ""
   ""        "/"          ""  
   "/"       ""           "/"  
   "/"       "/"          "/"  
   "/"       "f"          "/f"
   "/"       "/f"         "/f"
   "d"       "f"          "d/f"
   "d/"      "f"          "d/f"
   "d"       "/f"         "d/f"
   "d/"      "/f"         "d/f"
===/pre>

Side Effects: 
   None

Version:
   Introduced BREW Client 3.1

See Also: 
   SPLITPATH()

======================================================================= 

Function: SPLITPATH()

Description:
   finds the filename part of a path given an inclusive directory,
   tests for cpszPath being in cpszDir

Prototype:
   char *SPLITPATH(const char *cpszPath, const char *cpszDir);   

Parameters:
   cpszPath: the path to split, test
   cpszDir: the directory to split off cpszPath, if cpszPath is in cpszDir

Return Value:
   
   Returns a pointer into cpszPath that represents the tail end of the filename,
     the part under cpszDir

  NULL if cpszPath is not in cpszDir

Comments:
   SPLITPATH() is similar to STRBEGINS(), but it ignores trailing slashes on
   cpszDir, and it returns a pointer to the first character of the subpath.
   The return value of SPLITPATH() will never begin with a '/'.

   The following list of examples shows the strings returned by
   SPLITPATH() for different paths:
===pre>
   cpszPath cpszDir  SPLITPATH()
   ""        ""           ""
   ""        "/"          ""  
   "/"       ""           ""  
   "/"       "/"          ""  
   "/d"      "d"          null
   "/d"      "/"          "d"
   "/d/"     "/d"         ""
   "/d/f"    "/"          "d/f"
   "/d/f"    "/d"         "f"
   "/d/f"    "/d/"        "f"
===/pre>
Side Effects: 
   None

Version:
   Introduced BREW Client 3.1

See Also: 
   SPLITPATH()

======================================================================= 

Function: GETFSFREE()

Description:
   Returns information about the amount of file system space available.

Prototype:
   uint32 GETFSFREE(uint32 *pdwTotal)

Parameters:
   pdwTotal: [out] filled with the total file system space on the 
               device, ignored if NULL

Return Value
   The amount of free file system space on the device

Comments:
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 3.1

See Also: 
   None

======================================================================= 

Function: GETRAMFREE()

Description:
   Returns information about the amount of heap space available.

Prototype:
   uint32 GETRAMFREE(uint32 *pdwTotal, uint32 *pdwMax)

Parameters:
   pdwTotal: [out] filled with the total heap space on the 
               device, ignored if NULL
   pdwMax: [out] filled with the size of largest node that can
               currently be allocated.  ignored if NULL

Return Value
   The amount of free heap space on the device

Comments:
   It may be possible, in some cases, to allocate a larger node than
   the value placed in pdwMax, since BREW's heap may coalesce movable 
   nodes, free caches, and unload unused modules to satisfy an 
   allocation request.

Side Effects: 
   None

Version:
   Introduced BREW Client 3.1

See Also: 
   None


==================================================================
GET_RAND() 
This function is deprecated. Instead, please use GETRAND().

Version:
	Introduced BREW Client 1.0	
	Deprecated BREW Client 2.1

==================================================================
GET_TIMEMS()
This function is deprecated. Instead, please use GETTIMEMS().

Version:
	Introduced BREW Client 1.0	
	Deprecated BREW Client 2.1
==================================================================
GET_SECONDS()
This function is deprecated. Instead, please use GETTIMESECONDS().

  Version:
	Introduced BREW Client 1.0	
	Deprecated BREW Client 2.1
==================================================================
GET_UPTIMEMS()
This function is deprecated. Instead, please use GETUPTIMEMS().

  Version:
	Introduced BREW Client 1.0	
	Deprecated BREW Client 2.1
==================================================================
GET_JULIANDATE()
This function is deprecated. Instead, please use GETJULIANDATE().

  Version:
	Introduced BREW Client 1.0	
	Deprecated BREW Client 2.1
==================================================================
GET_APP_INSTANCE()
This function is deprecated. Instead, please use GETAPPINSTANCE().

  Version:
	Introduced BREW Client 1.0	
	Deprecated BREW Client 2.1
==================================================================
STR_TO_WSTR() 
This function is deprecated. Instead, please use STRTOWSTR().

  Version:
	Introduced BREW Client 1.0	
	Deprecated BREW Client 2.1
==================================================================
WSTR_TO_STR()
This function is deprecated. Instead, please use WSTRTOSTR().

  Version:
	Introduced BREW Client 1.0	
	Deprecated BREW Client 2.1
==================================================================
WSTR_TO_FLOAT() 
This function is deprecated. Instead, please use WSTRTOFLOAT().

  Version:
	Introduced BREW Client 1.0	
	Deprecated BREW Client 2.1
==================================================================      
FLOAT_TO_WSTR()
This function is deprecated. Instead, please use FLOATTOWSTR().

  Version:
	Introduced BREW Client 1.0	
	Deprecated BREW Client 2.1
==================================================================   
UTF8_TO_WSTR () 
This function is deprecated. Instead, please use UTF8TOWSTR().

  Version:
	Introduced BREW Client 1.0	
	Deprecated BREW Client 2.1
==================================================================   
WSTR_TO_UTF8() 
This function is deprecated. Instead, please use WSTRTOUTF8().

  Version:
	Introduced BREW Client 1.0	
	Deprecated BREW Client 2.1
 ==================================================================   

Function: INET_PTON()

Description: 
   This function converts a string into an address structure according to a 
   given address family.

Prototype:
   int INET_PTON(int af, const char *src, void *dst);

Parameters:
   af:  the address family of the string that will be converted (AEE_AF_INET 
        for IPv4 or AEE_AF_INET6 for IPv6).
   src: string containing the address, it can contain either IPv4 or IPv6 address
        (e.g. "1.2.3.4" or "1::2").
   dst: pointer to an address structure that will be filled with the address.
   
Return Value:
   1: the conversion succeeded. the address structure pointed to by dst was 
      filled with the binary representation of the address in network byte order.
   0: the input is not a valid IPv4 dotted-decimal address string or a valid
      IPv6 address string.
  -1: af indicates unknown address family.
   
Comments:
   For IPv4 addresses, dst should point to INAddr structure. For IPv6 addresses,
   dst should point to INAddr6 structure.
   
Version:
   Introduced BREW Client 3.1.4

See Also:
   INAddr
   INAddr6
=======================================================================

Function: INET_NTOP()

Description: 
   This function converts an address structure into a string according to a 
   given address family.

Prototype:
   const char* INET_NTOP(int af, const void *src, char *dst, size_t size);

Parameters:
   af:   the address family of the address structure that will be converted 
         (AEE_AF_INET for IPv4 or AEE_AF_INET6 for IPv6).
   src:  pointer to the address structure to be converted.
   dst:  buffer to write the string into.
   size: size of the buffer pointed to by dst.
   
Return Value:    
   pointer to the buffer (dst) containing the string if the conversion succeeds,
   and NULL otherwise.

Comments:
   For IPv4 addresses, src should point to INAddr structure. For IPv6 addresses,
   src should point to INAddr6 structure.

   AEE_INET_ADDRSTRLEN and AEE_INET6_ADDRSTRLEN are defined for applications to
   easily declare buffers of the proper size to store IPv4 and IPv6 addresses 
   in string form.
      
Version:
   Introduced BREW Client 3.1.4

See Also:
   INAddr
   INAddr6
=======================================================================
*/

#endif   // AEESTDLIB_H

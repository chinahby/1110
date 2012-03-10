#ifndef AEESTDLIB_STATIC_H
#define AEESTDLIB_STATIC_H /* #ifndef AEESTDLIB_STATIC_H */
/*======================================================
FILE:  AEEStdLib_static.h

SERVICES:  BREW standard-libary helper functions for static
           applications and modules.

GENERAL DESCRIPTION:
   Function prototypes and declaractions for standard library.

        Copyright © 1999-2006 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/


#include "AEE.h"

#if !defined(AEE_STATIC)
#error AEEStdLib_static.h is for inclusion by static apps and modules: those compiled with the BREW library directly, or as part of an OEM build.  If you are statically linking with BREW, please define AEE_STATIC in your Makefile.
#endif 

#if !defined(AEESTDLIB_STATIC_H__INC__BY__AEESTDLIB_H)
#error AEEStdLib_static.h is not for intended for direct inclusion.  Please remove any include directives that reference AEEStdLib_static.h directly and change them to include AEEStdLib.h.
#endif

#include "OEMStdLib.h"
#include "AEEOS.h"
#include "AEE_OEMHeap.h"

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

// Begin functions defined in stdlib.c not accessed via macros
extern boolean       IsUNICODE(AECHAR * psz, int nLen);

extern boolean       aee_forcereleaseobj(void ** ppobj);
extern boolean       aee_releaseobj(void ** ppObj);
extern boolean       aee_freeobj(void ** ppObj);
extern boolean       sys_freeobj(void ** ppObj);

// Begin functions access via macros

extern AEE_EXPORTS AECHAR *      aee_wstrend(const AECHAR * sz);
extern AEE_EXPORTS AECHAR *      aee_wstrcharend(const AECHAR * sz, AECHAR ch);
extern AEE_EXPORTS int           aee_wstrlen(const AECHAR *p);
extern AEE_EXPORTS int           aee_wstrsize(const AECHAR *p);
extern AEE_EXPORTS AECHAR       *aee_wstrcpy(AECHAR *pDest, const AECHAR *pSrc);
extern AEE_EXPORTS AECHAR       *aee_wstrcat(AECHAR *pDest, const AECHAR *pSrc);
extern AEE_EXPORTS int           aee_wstrcmp(const AECHAR *s1, const AECHAR *s2);
extern AEE_EXPORTS int           aee_wstricmp(const AECHAR *s1, const AECHAR *s2);
extern AEE_EXPORTS int           aee_wstrncmp(const AECHAR *s1, const AECHAR *s2, size_t nLen);
extern AEE_EXPORTS int           aee_wstrnicmp(const AECHAR *s1, const AECHAR *s2, size_t nLen);
extern AEE_EXPORTS AECHAR       *aee_wstrrchr(const AECHAR *pText, AECHAR ch);
extern AEE_EXPORTS AECHAR       *aee_wstrchr(const AECHAR *pText, AECHAR ch);
extern AEE_EXPORTS void          aee_wsprintf(AECHAR *pDest, int nSize, const AECHAR *pFormat, ...);
extern AEE_EXPORTS AECHAR       *aee_wwritelongex(AECHAR *pszBuf, long n, int nPad, int *pnRemaining);
extern AEE_EXPORTS AECHAR *      aee_wwritelong(AECHAR *pszBuf, long n);
extern AEE_EXPORTS int           aee_wstrncopyn(AECHAR *pszDest, int cbDest, const AECHAR *pszSource, int lenSource);
extern AEE_EXPORTS void          aee_wstrcompress(const AECHAR *pSrc, int nLen,byte *pDest, int nSize);
extern AEE_EXPORTS double        aee_atof(const char* s);
extern AEE_EXPORTS int           aee_atoi(const char * s);
extern AEE_EXPORTS void          aee_qsort(void *b, size_t n, size_t s, PFNQSORTCOMPARE pfn);
extern AEE_EXPORTS void          aee_strexpand(const byte *pSrc, int nCount, AECHAR *pDest, int nSize);
extern AEE_EXPORTS boolean       aee_floattowstr(double val, AECHAR *psz, int nSize);

// These 2 functions called directly by some OEMS...

extern AEE_EXPORTS char         *aee_wstrtostr(const AECHAR *pIn, char *pszDest, int nSize);
extern AEE_EXPORTS AECHAR       *aee_strtowstr(const char *pszIn, AECHAR *pDest, int nSize);

extern AEE_EXPORTS void         *aee_malloc(uint32 dwSize);
extern AEE_EXPORTS void          aee_free(void *p);
extern AEE_EXPORTS void         *aee_realloc(void *p, uint32 dwSize);
extern AEE_EXPORTS AECHAR       *aee_wstrdup(const AECHAR *pIn);
extern AEE_EXPORTS char         *aee_strdup(const char *psz);

extern AEE_EXPORTS void          aee_dumpheap(void);

extern AEE_EXPORTS void          AEE_FreeNativeImage(void * pBuff);
extern AEE_EXPORTS void *        aee_memset(void * pd, int c, size_t s);
extern AEE_EXPORTS void *        aee_memmove(void * pd, const void * ps, size_t s);
extern AEE_EXPORTS char *        aee_strcpy(char *dst, const char *src);
extern AEE_EXPORTS char *        aee_strcat(char *dst, const char *src);
extern AEE_EXPORTS int           aee_strcmp(const char *s1, const char *s2);
extern AEE_EXPORTS size_t        aee_strlen(const char *s);
extern AEE_EXPORTS char *        aee_strchr(const char *s1, int ch);
extern AEE_EXPORTS char  *       aee_strrchr(const char *s1, int ch);
extern AEE_EXPORTS uint32        aee_strtoul(const char *nptr, char **endptr, int base );
extern AEE_EXPORTS char *        aee_strncpy(char *strDest, const char *strSource,size_t count );
extern AEE_EXPORTS int           aee_strncmp(const char *a, const char *b, size_t length );
extern AEE_EXPORTS char *        aee_strstr(const char *haystack, const char *needle);
extern AEE_EXPORTS int           aee_memcmp(const void *a, const void *b, size_t length );
extern AEE_EXPORTS void *        aee_memchr(const void *src, int c, size_t length );
extern AEE_EXPORTS char         *aee_stristr(const char *cpszHaystack, const char *cpszNeedle);
extern AEE_EXPORTS char         *aee_strichr(const char *cpszSrch, int c);
extern AEE_EXPORTS char         *aee_strichrend(const char *cpszSrch, int nc);
extern AEE_EXPORTS boolean       aee_strbegins(const char *cpszPrefix, const char *cpsz);
extern AEE_EXPORTS boolean       aee_stribegins(const char *cpszPrefix, const char *cpsz);
extern AEE_EXPORTS boolean       aee_strends(const char *cpszSuffix, const char *cpsz);
extern AEE_EXPORTS char         *aee_strlower(char *psz);
extern AEE_EXPORTS char         *aee_strupper(char *psz);
extern AEE_EXPORTS char         *aee_strchrsend(const char *cpszSrch, const char *cpszChars);
extern AEE_EXPORTS char         *aee_strchrend(const char *cpszSrch, char c);
extern AEE_EXPORTS int           aee_stricmp(const char *cpszA, const char *cpszB);
extern AEE_EXPORTS int           aee_strnicmp(const char *cpszA, const char *cpszB, size_t nLen);
extern AEE_EXPORTS char         *aee_memrchr(const char *pcSrch, int c, size_t nLen);
extern AEE_EXPORTS char         *aee_memstr(const char *cpHaystack, const char *cpszNeedle, size_t nHaystackLen);
extern AEE_EXPORTS char         *aee_memchrend(const char *pcSrch, int c, size_t nLen);
extern AEE_EXPORTS char         *aee_memrchrbegin(const char *pcSrch, int c, size_t nLen);
extern AEE_EXPORTS double        aee_fop(double v1, double v2,int nType);
extern AEE_EXPORTS boolean       aee_fcmp(double v1, double v2,int nType);
extern AEE_EXPORTS int           aee_OEMstrsize(const byte *p);
extern AEE_EXPORTS int           aee_OEMstrlen(const byte *p);

extern AEE_EXPORTS int           aee_vsprintf(char *s, const char *format, AEEOldVaList arg);
extern AEE_EXPORTS char *        aee_vsnprintf_malloc(const char * pszFormat,...);

extern AEE_EXPORTS void          GetJulianDate(uint32 dwSecs, JulianType *pDate);
extern AEE_EXPORTS uint32        JulianToSeconds(JulianType *pDate);
extern AEE_EXPORTS uint32        GetTimeMS(void);
extern AEE_EXPORTS void          GetRand(byte *pDest, int nSize);
extern AEE_EXPORTS boolean       aee_inet_aton(const char *psz, uint32 *pulAddr);
extern AEE_EXPORTS void          aee_inet_ntoa(uint32 ulAddr, char *psz, size_t nlen);
extern AEE_EXPORTS uint32        aee_swapl(uint32 ul);
extern AEE_EXPORTS uint16        aee_swaps(uint16 us);

extern AEE_EXPORTS int           aee_inet_pton(int af, const char *src, void *dst);
extern AEE_EXPORTS const char*   aee_inet_ntop(int af, const void *src, char *dst, size_t size);

extern AEE_EXPORTS uint32        aee_getFSfree(uint32 * pdwTotal);

// Definitions for these are in aeeshell.c

extern AEE_EXPORTS uint32        AEE_GetVersion(byte *pDest, int nSize, uint16 wFlags);
extern AEE_EXPORTS void*         AEE_SetupNativeImage(AEECLSID cls, void *pBuffer, AEEImageInfo *pii, boolean *pbRealloc);
extern AEE_EXPORTS void*         AEE_SetupNativeImageEx(AEECLSID cls, void *pBuffer, int32 cbBuffer, AEEImageInfo *pii, boolean *pbRealloc);
extern AEE_EXPORTS void          aee_dbgprintf( const char *Format, ... );
extern AEE_EXPORTS void          aee_dbgevent(AEEEvent evt, AEECLSID cls, uint32 pl);
                     
extern AEE_EXPORTS void          aee_dbgvprintf(const char *Format, va_list ap);
extern AEE_EXPORTS int           aee_sprintf(char * pszDest,const char * pszFormat, ...);
extern AEE_EXPORTS int           aee_vsprintf(char * pszDest, const char * pszFormat, AEEOldVaList args);
extern AEE_EXPORTS int32         aee_snprintf(char *buf, uint32 f, const char * pszFormat, ...);
extern AEE_EXPORTS int32         aee_vsnprintf(char *buf, uint32 f, const char * pszFormat, AEEOldVaList args);
extern AEE_EXPORTS size_t        aee_strlcpy(char * dst, const char * src, size_t nSize);
extern AEE_EXPORTS size_t        aee_strlcat(char * dst, const char * src, size_t nSize);
extern AEE_EXPORTS size_t        aee_wstrlcpy(AECHAR * dst, const AECHAR * src, size_t nSize);
extern AEE_EXPORTS size_t        aee_wstrlcat(AECHAR * dst, const AECHAR * src, size_t nSize);

extern AEE_EXPORTS void          aee_MSECSToHM(uint32 dwExpire, int16 * pwHours, int16 * pwMins, int16 * pwSecs, int16 * pwHSecs, boolean * pbPM);
extern AEE_EXPORTS IApplet *     AEE_GetAppInstance(void);
extern AEE_EXPORTS IQueryInterface * AEE_GetALSContext(void);
extern AEE_EXPORTS void    *     aee_setstaticptr(int nSPId, void *pNew);
extern AEE_EXPORTS double        aee_fassignstr(const char *pszFloat);
extern AEE_EXPORTS double        aee_fassignint(int32 val);
extern AEE_EXPORTS int32         aee_trunc(double val);
extern AEE_EXPORTS uint32        aee_utrunc(double val);

extern AEE_EXPORTS double        aee_strtod(const char *pszFloat, char **ppszEnd);
extern AEE_EXPORTS double        aee_fcalc(double x, int calcType);
extern AEE_EXPORTS int           aee_ftoint(double val);
extern AEE_EXPORTS double        aee_fltget(uint32 fType);
extern AEE_EXPORTS int           aee_getlasterror(int errRegion);
extern AEE_EXPORTS double        wgs84_to_degrees(int32 latlon);
extern AEE_EXPORTS char *        aee_basename(const char *cpszPath);
extern AEE_EXPORTS int           aee_makepath(const char *cpszDir, const char *cpszFile,
                                              char *pszOut, int *pnOutLen);
extern AEE_EXPORTS char *        aee_splitpath(const char *cpszPath, const char *cpszDir);


extern AEE_EXPORTS char          aee_toupper(char c);
extern AEE_EXPORTS char          aee_tolower(char c);
extern AEE_EXPORTS int           aee_vsnxprintf(char *pszBuf, int nBufLen, 
                                                const char *cpszTemplate,
                                                const char pcDelims[2], 
                                                const char *cpszzTokFormats, 
                                                va_list args);
extern AEE_EXPORTS int           aee_snxprintf(char *pszBuf, int nBufLen, 
                                               const char *cpszTemplate,
                                               const char pcDelims[2], 
                                               const char *cpszzTokFormats, ...);

extern AEE_EXPORTS int           aee_err_realloc(uint32 uLen, void **pp);
extern AEE_EXPORTS int           aee_err_strdup(const char *pszSrc, char **pp);

extern AEE_EXPORTS int32         AEE_vxprintf(void *buf, uint32 f, const char *format, va_list list);

//*********************************************************************************************
//
// OEM String Routines - Supported in AEE_OEMString.c
//
//*********************************************************************************************
extern AEE_EXPORTS double        OEM_WStrToFloat(const AECHAR * psz);
extern AEE_EXPORTS boolean       OEM_FloatToWStr(double v, AECHAR * psz, int nSize);
extern AEE_EXPORTS boolean       OEM_UTF8ToWStr(const byte * pszIn,int nLen, AECHAR * pszDest, int nSizeDestBytes);
extern AEE_EXPORTS boolean       OEM_WStrToUTF8(const AECHAR * pszIn,int nLen, byte * pszDest, int nSizeDestBytes);
extern AEE_EXPORTS void          OEM_WStrLower(AECHAR * pszDest);
extern AEE_EXPORTS void          OEM_WStrUpper(AECHAR * pszDest);
extern AEE_EXPORTS TChType       OEM_GetCHType(AECHAR ch);




// 
// Fixes problem in conflict between ARM headers
//


#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */


/* ==========================================================================
  ||
  || Static linkers get these
  ||
  ========================================================================== */

#define WWRITELONGEX              aee_wwritelongex
#define WWRITELONG                aee_wwritelong
#define WSTRNCOPYN                aee_wstrncopyn
#define WSTRSIZE                  aee_wstrsize
#define WSTRCPY                   aee_wstrcpy
#define WSTRCAT                   aee_wstrcat
#define WSPRINTF                  aee_wsprintf
#define WSTRLEN                   aee_wstrlen
#define WSTRCMP                   aee_wstrcmp
#define WSTRICMP                  aee_wstricmp
#define WSTRNCMP                  aee_wstrncmp
#define WSTRNICMP                 aee_wstrnicmp

#if (AEE_DBG_HEAP == 1)
#define WSTRDUP(szIn)             (AECHAR*)(AEEHeap_DbgMark(aee_wstrdup((szIn)),__FILE__,__LINE__))
#define MALLOC(dw)                AEEHeap_DbgMark(aee_realloc(0,(dw)),__FILE__,__LINE__)
#define REALLOC(p,dw)             AEEHeap_DbgMark(aee_realloc((p),(dw)),__FILE__,__LINE__)
#define STRDUP(p)                 (char*)(AEEHeap_DbgMark(aee_strdup(p),__FILE__,__LINE__))
#else /* #if (AEE_DBG_HEAP == 1)  */
#define WSTRDUP                   aee_wstrdup
#define MALLOC                    aee_malloc
#define REALLOC                   aee_realloc
#define STRDUP                    aee_strdup
#endif
/* #if (AEE_DBG_HEAP == 1) */ 

#define DBGHEAPMARKEX             AEEHeap_DbgMark

#define DUMPHEAP                  aee_dumpheap
#define WSTRCHR                   aee_wstrchr
#define WSTRRCHR                  aee_wstrrchr
#define WSTRLOWER                 OEM_WStrLower
#define WSTRUPPER                 OEM_WStrUpper
#define WSTRLCPY                  aee_wstrlcpy
#define WSTRLCAT                  aee_wstrlcat
#define GETCHTYPE                 OEM_GetCHType
#define STRTOWSTR                 aee_strtowstr
#define WSTRTOSTR                 aee_wstrtostr
#define MEMMOVE                   aee_memmove
#define MEMSET                    aee_memset
#define MEMCMP                    aee_memcmp
#define MEMCHR                    aee_memchr
#define MEMRCHR                   aee_memrchr
#define MEMSTR                    aee_memstr
#define MEMCHREND                 aee_memchrend
#define MEMRCHRBEGIN              aee_memrchrbegin
#define MSLEEP                    AEEOS_Sleep
#define STRCPY                    aee_strcpy
#define STRNCPY                   aee_strncpy
#define STRNCMP                   aee_strncmp
#define STRICMP                   aee_stricmp
#define STRNICMP                  aee_strnicmp
#define STRCAT                    aee_strcat
#define STRCMP                    aee_strcmp
#define STRLEN                    aee_strlen
#define STRCHR                    aee_strchr
#define STRCHREND                 aee_strchrend
#define STRCHRSEND                aee_strchrsend
#define STRRCHR                   aee_strrchr
#define STRSTR                    aee_strstr
#define STRISTR                   aee_stristr
#define STRBEGINS                 aee_strbegins
#define STRIBEGINS                aee_stribegins
#define STRENDS                   aee_strends
#define STRLOWER                  aee_strlower
#define STRUPPER                  aee_strupper
#define SPRINTF                   aee_sprintf
#define STRTOUL                   aee_strtoul
#define STRTOD                    aee_strtod
#define STRLCPY                   aee_strlcpy
#define STRLCAT                   aee_strlcat
#define CONVERTBMP(src,pi,pb)     AEE_SetupNativeImage(AEECLSID_WINBMP,(src),(pi),(pb))
#define FREE                      aee_free
#define LOCKMEM                   AEEHeap_Lock
#define UNLOCKMEM                 AEEHeap_Unlock
#define SYSFREE                   AEE_FreeNativeImage
#define OEMSTRLEN                 aee_OEMstrlen
#define OEMSTRSIZE                aee_OEMstrsize
#define GETAEEVERSION             AEE_GetVersion
#define AEE_IS_PATCH_PRESENT(p)   AEE_GetVersion((byte*)(p), 0, GAV_INTERNAL)
#define WSTRTOFLOAT               OEM_WStrToFloat
#define FLOATTOWSTR               aee_floattowstr
#define UTF8TOWSTR                OEM_UTF8ToWStr
#define WSTRTOUTF8                OEM_WStrToUTF8
#define ATOI                      aee_atoi
#define ATOF                      aee_atof
#define QSORT                     aee_qsort
#define WSTRCOMPRESS              aee_wstrcompress
#define STREXPAND                 aee_strexpand
#define FADD(v1,v2)               ((v1) + (v2))
#define FSUB(v1,v2)               ((v1) - (v2))
#define FMUL(v1,v2)               ((v1) * (v2))
#define FDIV(v1,v2)               ((v2) ? (v1) / (v2) : 0)
#define FPOW(v1,v2)               aee_fop((v1),(v2),FO_POW)
#define FFLOOR(v)                 aee_fcalc((v), FCALC_FLOOR)
#define FCEIL(v)                  aee_fcalc((v), FCALC_CEIL)
#define FSQRT(v)                  aee_fcalc((v), FCALC_SQRT)
#define FSIN                      sin
#define FCOS                      cos
#define FABS                      fabs
#define FCMP_L(v1,v2)             ((v1) < (v2))
#define FCMP_LE(v1,v2)            ((v1) <= (v2))
#define FCMP_E(v1,v2)             ((v1) == (v2))
#define FCMP_G(v1,v2)             ((v1) > (v2))
#define FCMP_GE(v1,v2)            ((v1) >= (v2))

#define __DBGPRINTF               aee_dbgprintf
#define GETRAND                   GetRand
#define GETTIMEMS                 GetTimeMS
#define GETUPTIMEMS               AEEOS_GetUpTime
#define GETTIMESECONDS            AEEOS_GetLocalTime
#define GETUTCSECONDS             AEEOS_GetUTCSeconds
#define GETJULIANDATE             GetJulianDate
#define JULIANTOSECONDS           JulianToSeconds
#define LOCALTIMEOFFSET           AEEOS_LocalTimeOffset
#define INET_ATON                 aee_inet_aton
#define INET_NTOA                 aee_inet_ntoa

#define INET_NTOP                 aee_inet_ntop
#define INET_PTON                 aee_inet_pton

#define SWAPS                     aee_swaps
#define SWAPL                     aee_swapl

#define GETFSFREE                 aee_getFSfree
#define GETRAMFREE                AEEHeap_GetRAMFree
#define SNPRINTF                  aee_snprintf
#define GETAPPINSTANCE            AEE_GetAppInstance
#define GETALSCONTEXT             AEE_GetALSContext

#define SETSTATICPTR              aee_setstaticptr

#define FASSIGN_STR               aee_fassignstr
#define FASSIGN_INT               aee_fassignint

#define FLTTOINT(v)               aee_ftoint((v))
#define FGETHUGE_VAL()            aee_fltget(FGET_HUGEVAL)
#define FGETFLT_MAX()             aee_fltget(FGET_FLTMAX)
#define FGETFLT_MIN()             aee_fltget(FGET_FLTMIN)

#define GETLASTFPERROR()          aee_getlasterror(ERR_REGION_FLOAT)

#define WGS84_TO_DEGREES          wgs84_to_degrees

#define DBGEVENT_EX               aee_dbgevent

#define ISBADREADPTR(p,n)         AEEOS_IsBadPtr(BREW_MEM_READ_ACCESS, (p), (n))
#define ISBADWRITEPTR(p,n)        AEEOS_IsBadPtr(BREW_MEM_WRITE_ACCESS, (p), (n))
#define BASENAME                  aee_basename
#define MAKEPATH                  aee_makepath
#define SPLITPATH                 aee_splitpath

/* not in stdlib vtable yet */
#define TOLOWER                   aee_tolower
#define TOUPPER                   aee_toupper
#define SNXPRINTF                 aee_snxprintf
#define VSNXPRINTF                aee_vsnxprintf


#define ERR_REALLOC(nlen,pp)      aee_err_realloc(nlen,(void **)(pp))
#define ERR_STRDUP                aee_err_strdup

static __inline int VSPRINTF(char *buf, const char *fmt, va_list arg)
{
   return aee_vsprintf(buf, fmt, AEEOldVaList_From_va_list((va_list *)&(arg)));
}

static __inline int VSNPRINTF(char *buf, uint32 len, const char *fmt, va_list arg)
{
   return aee_vsnprintf(buf, len, fmt, AEEOldVaList_From_va_list((va_list *)&(arg)));
}


/* ==========================================================================
  ||
  || end Static
  ||
  ========================================================================== */


/*=======================================================================
   MACROS   DOCUMENTATION
=========================================================================
Function: OEM_WStrToFloat()

Description:
   Converts a string to a floating point value.

Prototype:
   double OEM_WStrToFloat(AECHAR * psz);

Parameters:
   psz:  Input string

Return Value:
   Floating point if successful.
   0 ( zero ) if fails.  ( if psz is NULL )

Comments:
   None

Side Effects:
   None

See Also:
   None

==================================================================
Function: OEM_FloatToWStr()

Description:
   Converts a floating point to a string.

Prototype:
   boolean OEM_FloatToWStr(double v, AECHAR * psz, int nSize)

Parameters:
   v: floating point value
   psz: destination string
   nSize: size of destination string

Return Value:
   TRUE on success
   FALSE on failure  ( if psz is NULL or nSize is zero or lesser )

Comments:
   None

Side Effects:
   None

See Also:
   None

==================================================================
Function: OEM_UTF8ToWStr()

Description:
   Converts a UTF8 string to a Wide string.

Prototype:
   boolean OEM_UTF8ToWStr(const byte * pSrc,int nLen, AECHAR * pDst, int nSize)

Parameters:
   pSrc: input string
   nLen: length of input string
   pDst: destination string
   nSize: size in bytes of destination

Return Value:
   TRUE on success
   FALSE if fails  ( if pSrc or pDst is NULL; if nSize is zero or lesser )

Comments:
   None

Side Effects:
   None

See Also:
   None

==================================================================
Function: OEM_WStrToUTF8()

Description:
   Converts a wide string to UTF8.

Prototype:
   boolean OEM_WStrToUTF8(const AECHAR * pSrc,int nLen, byte * pDst, int nSize)

Parameters:
   pSrc: input string
   nLen: length of input string
   pDst: destination string
   nSize: size in bytes of destination

Return Value:
   TRUE if successful
   FALSE if fails  ( pSrc or pDst is NULL; nSize is zero or lesser )


Comments:
   None

Side Effects:
   None

See Also:
   None

==================================================================
Function: OEM_WStrLower()

Description:
   Converts all upper case characters in a wide string to lower.

Prototype:
   void OEM_WStrLower(AECHAR * pszDest)

Parameters:
   pszDest: Source/Destination string

Return Value:
   None

Comments:
   None

Side Effects:
   None

See Also:
   None

==================================================================
Function: OEM_WStrUpper()

Description:
   Converts all lower case characters in a wide string to upper.

Prototype:
   void OEM_WStrUpper(AECHAR * pszDest);

Parameters:
   pszDest: Source/Destination string

Return Value:
   None

Comments:
   None

Side Effects:
   None

See Also:
   None

==================================================================
Function: OEM_GetCHType()

Description:
   Returns the type (numeric, alpha, and so forth) of a wide character.

Prototype:
   TChType OEM_GetCHType(AECHAR ch);

Parameters:
   ch: input character

Return Value:
   Type of character

Comments:
   None

Side Effects:
   None

See Also:
   None

==================================================================
Function: AEE_vxprintf()

Description:
   Prints a formatted string to a buffer. OR finds out how much room to allocate for a formatted string. This size
   includes the NULL terminator.

Prototype:
   int32 AEE_vxprintf(void *buf, uint32 f, const char *format, VA_LIST list)

Parameters:
   buf:    buffer to write to. Use NULL to find out the size required for the string.
   f:      size of the buffer(0x7FFFFFFF) maximum. Use 0 with NULL buf to find out the size 
           required for the string. The rest are reserved for flags.
   format: string containing formatting
   list:   optional list of arguments based on formatting

Return Value:
   Number of bytes stored in the buffer.
   OR number of bytes required for the formatted string including the NULL terminator
   OR -1 on failure.

Comments:
   None

Side Effects:
   None

See Also:
   None

==================================================================*/
#endif /* #ifndef AEESTDLIB_STATIC_H */

/*======================================================

FILE:      AEEWebUtil.c

SERVICES:  IWebUtil

GENERAL DESCRIPTION:

  a bunch of web utility code

PUBLIC CLASSES AND STATIC FUNCTIONS:

    IWebUtil

INITIALIZATION & SEQUENCING REQUIREMENTS:

	See Exported Routines

        Copyright © 2001-2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#include "AEEWeb.h"
#include "AEEStdLib.h"
#include "AEEBufBound.h"
#include "AEEShell.h"

#define RELEASEIF(x) if (x) { IBASE_Release((IBase *)(x)); (x) = 0; }

/*====================================================================
  deee-fines
  ===================================================================*/

/* 
   RFC2936 says these are "unreserved":

       ! ' ( ) * - . _ ~ 

   but IE and Mozilla encode all but:
   
        * - . _

   which are: 

        2A 2D 2E 5F  

   These aren't really expressible in a simple math-expression, so we'll 
    do a simple test
 */
#define ISUNRESERVED(c) ('*'==(c)||'-'==(c)||'.'==(c)||'_'==(c))

/*
  needs expansion
 */
#define ISQUOTE(c)  (!ISALNUM((c)) && !ISUNRESERVED((c)) && (' ' != (c)))

#define ISDIGIT(c)  ( (unsigned) ((c) - '0') < 10)
#define ISALPHA(c)  ( (unsigned) ( ((c)|32) - 'a') < 26 )
#define ISALNUM(c)  ( ISDIGIT(c) || ISALPHA(c) )
#define ISHEXDIGIT(c) ( ISDIGIT((c)) || (((unsigned)(((c)|32) - 'a')) < 6) )

static char gszHexDigits[] = "0123456789ABCDEF";
#define HEXDIGIT(n) gszHexDigits[(n)&0x0f]
/* will overflow, though */
#define HEXVAL(c) (ISDIGIT(c) ? ((c) - '0') : (((c)|32) - 'a' + 10))


#define XX 127
/*
 * Table for decoding base64
 */
static const char index_64[128] = {
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,62, XX,XX,XX,63,
    52,53,54,55, 56,57,58,59, 60,61,XX,XX, XX,XX,XX,XX,
    XX, 0, 1, 2,  3, 4, 5, 6,  7, 8, 9,10, 11,12,13,14,
    15,16,17,18, 19,20,21,22, 23,24,25,XX, XX,XX,XX,XX,
    XX,26,27,28, 29,30,31,32, 33,34,35,36, 37,38,39,40,
    41,42,43,44, 45,46,47,48, 49,50,51,XX, XX,XX,XX,XX,
};
#define CHAR64(c) (((c) >= sizeof(index_64)) ? XX : (index_64[(c)]))
static const char basis_64[] =
   "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/**
  || Function
  || --------
  || static int DecodeBase64(const char *cpszIn, char *pcOut)
  ||  
  || Description
  || -----------
  || decodes a base64 string, returning the length of the resulting data
  ||
  || Parameters
  || ----------
  || const char *cpszIn: input, base64 encoded, null-terminated
  || char *pcOut: output buffer
  ||
  || Returns
  || -------
  || the length of the resultant data, or -1 if the Base64String is bogus
  ||
  || Remarks
  || -------
  ||
  */
static int DecodeBase64(const char *cpszIn, char *pcOut)
{
   const char *pSrc = cpszIn;
   char *pDest = pcOut;

   if (STRLEN(cpszIn) & 3) { /* length not a multiple of 4 */
      return -1;
   }

   for (;;) {
      unsigned char c1, c2, c3, c4;

      c1 = *pSrc++;

      if (c1 == '\0') {
         return pDest - pcOut;
      }

      if (CHAR64(c1) == XX) {
         return -1;
      }

      c2 = *pSrc++;

      if (CHAR64(c2) == XX) {
         return -1;
      }

      c3 = *pSrc++;

      if ((c3 != '=') && (CHAR64(c3) == XX)) {
         return -1;
      }

      c4 = *pSrc++;

      if ((c4 != '=') && (CHAR64(c4) == XX)) {
         return -1;
      }

      *pDest++ = (char)((CHAR64(c1) << 2) | ((CHAR64(c2) & 0x30) >> 4));

      if (c3 == '=') {
         c1 = *pSrc++;

         if (c1 != '\0') {
            return -1;
         }

         if (c4 != '=') {
            return -1;
         }

         return pDest - pcOut;
      }

      *pDest++ = (char)(((CHAR64(c2) & 0xF) << 4) | ((CHAR64(c3) & 0x3C) >> 2));

      if (c4 == '=') {
         c1 = *pSrc++;

         if (c1 != '\0') {
            return -1;
         }

         return pDest - pcOut;
      }

      *pDest++ = (char)(((CHAR64(c3) & 0x3) << 6) | CHAR64(c4));

   }

}


/**
  || Function
  || --------
  || static char *EncodeBase64(const char *pcIn, int nLen, char *pszOut)
  ||  
  || Description
  || -----------
  || does a base64 encode
  ||
  || Parameters
  || ----------
  || const char *pcIn: input/output
  || int nLen: len of pcIn
  || char *pszOut: output buffer
  ||
  || Returns
  || -------
  || pszOut
  ||
  || Remarks
  || -------
  || pszOut _must_ be over-allocated to BASE64LEN(nLen) bytes, 
  ||  since base64 data takes up more room, and needs null-termination)
  ||
  */
static char *EncodeBase64(const char *pcIn, int nInLen, char *pszOut)
{
   char *pDest;
   char *pSrc;
   int   nOutLen = ((nInLen + 2) / 3) * 4;

   /* move the data to the end */
   pSrc = pszOut + nOutLen - nInLen;
   MEMMOVE(pSrc, pcIn, nInLen);
   pDest = pszOut;

   while (nInLen) {
      unsigned char c1,c2,c3;

      c1 = *pSrc++;
      *pDest++ = basis_64[c1>>2];
      nInLen--;
    
      if (!nInLen){
         c2 = 0;
      } else {
         c2 = *pSrc++;
      }

      *pDest++ = basis_64[((c1 & 0x3) << 4) | ((c2 & 0xF0) >> 4)];

      if (!nInLen){
         *pDest++ = '=';
         *pDest++ = '=';
         break;
      }

      nInLen--;

      if (!nInLen){
         c3 = 0;
      } else {
         c3 = *pSrc++;
      }
    
      *pDest++ = basis_64[((c2 & 0xF) << 2) | ((c3 & 0xC0) >> 6)];

      if (!nInLen){
         *pDest++ = '=';
         break;
      }
      --nInLen;

      *pDest++ = basis_64[c3 & 0x3F];
   }

   *pDest = '\0';

   return pszOut;
}

/* 
   from rfc2396, 
    scheme        = alpha *( alpha | digit | "+" | "-" | "." )
*/

#define ISSCHEME(c) ((((c) >= 'a') && ((c) <= 'z')) || \
                     (((c) >= 'A') && ((c) <= 'Z')) || \
                     (((c) >= '0') && ((c) <= '9')) || \
                     ((c) == '+') || \
                     ((c) == '-') || \
                     ((c) == '.'))

static boolean isscheme(const char *cpsz, int nLen)
{
   while (nLen-- > 0) {
      register char c;

      c = *cpsz++;

      if (!ISSCHEME(c)) {
         return FALSE;
      }
   }

   return TRUE;
}

/*
  ||
  || search for a list of characters from a string in reverse order, if found,
  || return the pointer to the first occurance (starting from the end), if not
  || found, return pointer to the end of the string
  ||
*/
static const char *qmemrchrsend(const char *pHay, const char *pNeedles, int qb, int qe, size_t nLen)
{
   const char *p;
   const char *pEnd = pHay + nLen;
   int state = 0;
   
   for (p = pEnd - 1; p > pHay; p--) {
      switch (state) {
      case 0:
         if (*p == qb) {
            state = 1;
         }
         else {
            if (STRCHR(pNeedles, *p)) {
               return p;
            }
         }
         continue;
         
      case 1:
         if (*p == qe) {
            state = 0;
         }
         continue;
      }
   }  

   if (!STRCHR(pNeedles, *p)) {
      p = pEnd;
   }
   
   return p;
}

static int ParseUrl(UrlParts *pup, const char *cpszUrl)
{

   UrlParts    u;
   int         nRv = SUCCESS;
   const char *pc;

   MEMSET(&u,0,sizeof(u));

   u.cpcSchm = cpszUrl;
   u.cpcEnd = cpszUrl + STRLEN(cpszUrl);

   if ((pc = STRCHREND(cpszUrl,':')) < STRCHREND(cpszUrl,'/') && 
       (pc != cpszUrl) && 
       isscheme(cpszUrl,pc-cpszUrl)) {
      /* found ':' before '/' */
      ++pc; /* just past ':' */
   } else {
      pc = cpszUrl;
   }

   u.cpcAuth = pc;
   
   if ((pc[0] == '/') && (pc[1] == '/')) { /* "sch://blah" or "//blah" */
      /* has host, port, user, pass */

      u.cpcUser = u.cpcAuth + 2;

      u.cpcPath = STRCHRSEND(u.cpcUser, "/?#");

      /* usUser - 1 is always '/' */
      u.cpcHost = MEMRCHRBEGIN(u.cpcUser - 1, '@',
                               u.cpcPath - (u.cpcUser - 1)) + 1;
      
      u.cpcPass = MEMCHREND(u.cpcUser, ':', u.cpcHost - u.cpcUser);

      u.cpcPort = qmemrchrsend(u.cpcHost, ":", '[', ']', u.cpcPath - u.cpcHost);

   } else { /* no authority part, maybe not hierarchal */

      u.cpcUser = u.cpcPass = u.cpcHost = u.cpcPort = u.cpcPath = pc;

   }

   /* find query (/path#namedpos) */
   u.cpcName = STRCHREND(u.cpcPath, '#');

   /* find query (/path?query) */
   u.cpcSrch = MEMCHREND(u.cpcPath, '?', u.cpcName - u.cpcPath);
   
   *pup = u;
      
   return nRv;
}

int MakeUrl(const char *cpszBase, const char *cpszRel, 
            char *pcBuf, int nBufLen)
{
   const char *cpcBaseEnd;
   const char *cpcRelStart;
   boolean     bNeedSlash = FALSE;
   UrlParts    upBase;
   UrlParts    upRel;
   AEEBufBound bb;

   if ((char *)0 == cpszRel) {
      cpszRel = "";
   }

   if ((char *)0 == cpszBase) {
      cpszBase = "";
   }
   
   ParseUrl(&upBase, cpszBase);
   ParseUrl(&upRel, cpszRel);

   if (UP_HASSCHM(&upRel) &&
       (UP_SCHMLEN(&upRel) != UP_SCHMLEN(&upBase)) || 
       STRNICMP(cpszBase, cpszRel, UP_SCHMLEN(&upRel))) {  
      /* 
         rel has scheme, is different scheme, 
         also covers case of no scheme in base 
      */
      cpcRelStart = upRel.cpcSchm;
      cpcBaseEnd = upBase.cpcSchm;
      goto output;
   }
   /* else */

   cpcRelStart = upRel.cpcAuth; /* true no matter what */

   if (UP_HASAUTH(&upRel)) { 
      cpcBaseEnd = upBase.cpcAuth; /* take base scheme*/
   } 
   else if (UP_HASPATH(&upRel) && 
            ((!UP_HASAUTH(&upBase) && (upBase.cpcPath[0] != '/')) ||
             (upRel.cpcPath[0] == '/'))) {

      /* non-hierarchal URL, or rooted relative path */

      cpcBaseEnd = upBase.cpcPath; /* take base scheme AND auth */
   } 
   else if (UP_HASPATH(&upRel)) {
      /* take some base, hierarchal */
      cpcBaseEnd = MEMRCHRBEGIN(upBase.cpcPath, '/', 
                                upBase.cpcSrch - upBase.cpcPath);
      bNeedSlash = TRUE;
   }
   else { 
      if (UP_HASSRCH(&upRel)) {
         
         cpcBaseEnd = upBase.cpcSrch;
         
      }
      else if (UP_HASNAME(&upRel)) {
         
         cpcBaseEnd = upBase.cpcName;
         
      }
      else { /* nothing in relative */
         
         cpcBaseEnd = upBase.cpcEnd;
         
      }

      bNeedSlash = (UP_HASAUTH(&upBase) && (upBase.cpcPath[0] != '/') &&
                    (cpcBaseEnd == upBase.cpcSrch));

   }

 output:

   AEEBufBound_Init(&bb, pcBuf, nBufLen);

   AEEBufBound_Write(&bb, cpszBase, cpcBaseEnd - cpszBase);
   if (bNeedSlash) {
      AEEBufBound_Putc(&bb,'/');
   }
   
   AEEBufBound_Puts(&bb,cpcRelStart);

   AEEBufBound_Putc(&bb,'\0'); /* null termination */

   if (AEEBufBound_IsFull(&bb)) { 
      /* just counting, or filled/overfilled, I need one more byte! */

      if ((char *)0 != pcBuf && (nBufLen > 0)) {
         pcBuf[0] = '\0'; /* help out, return empty string */
      }

      return AEEBufBound_Wrote(&bb) + 1;
      /* one more for extra '/' I might have to add in below */
   }

   /* ASSERT(have complete, NULL terminated URL in pcBuf) */

   /* only coalesce heirarchal paths when we're not counting */
    
   ParseUrl(&upBase, pcBuf);

   if (UP_HASAUTH(&upBase) || (upBase.cpcPath[0] == '/')) {

      /* coalesce ".." and "." */   
      char *pc; 
      char *pcEnd;
      char cSave;

      if (upBase.cpcPath[0] != '/') { /* gotta do little magic */
         MEMMOVE((char *)upBase.cpcPath + 1, upBase.cpcPath, 
                 upBase.cpcEnd - upBase.cpcPath + 1);
         ((char *)upBase.cpcPath)[0] = '/';
         ++upBase.cpcSrch;
         /* 
            don't care about these... 
            ++upBase.usName; 
            ++upBase.usEnd;
         */ 
      }

      pc = (char *)upBase.cpcPath;

      pcEnd = (char *)upBase.cpcSrch;

      cSave = *pcEnd; /* might be '#', '?', or '\0' */
      *pcEnd = '/';
      
      while ((char *)0 != (pc = MEMSTR(pc, "/.", pcEnd - pc))) {
         char *pcDelFrom;

         if (pc[2] == '/') {
            /*  delete "/." */
            pcDelFrom = pc;
            pc += 2;
         } else if (pc[2] == '.' && pc[3] == '/') {
            /*  delete  "/element/.." */
            pcDelFrom = MEMRCHRBEGIN(upBase.cpcPath, '/', pc - upBase.cpcPath);
            pc += 3;
         } else {
            pc += 2;
            continue;
         }
         
         if (pc == pcEnd) { /* short a little */
            MEMMOVE(pcDelFrom + 1, pc, bb.pcWrite - pc);
            pc--; /* to fix up pcWrite by the right amount */
         } else {
            MEMMOVE(pcDelFrom, pc, bb.pcWrite - pc);
         }
         bb.pcWrite -= (pc - pcDelFrom);
         pcEnd -= (pc - pcDelFrom);
         pc = pcDelFrom;
      }
      *pcEnd = cSave;
   }
   
   return STRLEN(pcBuf) + 1;
}



/**
  || Function
  || --------
  || static char *UrlEncode(const char *pcIn, int *pnInLen, 
  ||                        char *pcOut, int *pnOutLen)
  ||
  || Description
  || -----------
  || encodes a string for safe inclusion in a URL, % escapes any "reserved" 
  ||  characters
  ||
  || Parameters
  || ----------
  || const char *pcIn: pointer to string to be encoded
  || int *pnInLen: (in/out) pointer to inLen, set on return to number of bytes
  ||                 successfully encoded
  || char *pcOut: (out) where to write encoded bytes, may be NULL, for counting
  || int *pnOutLen: (in/out) for input, is sizeof pcOut, if pcOut is NULL, 
  ||                pnOutLen is set to desired buffer size, if pcOut is 
  ||                non-null, pnOutLen is set to length of encoded data, 
  ||                which may be less than sizeof(pcOut) if a full encoded
  ||                character won't fit, or if an encoded pcIn is shorter
  ||                than sizeof(pcOut)
  ||
  || Returns
  || -------
  || pcOut
  ||
  || Remarks
  || -------
  ||
  */
static char *UrlEncode(const char *pcIn, int *pnInLen, 
                       char *pcOut, int *pnOutLen)
{
   int   nOutLen;
   int   nInLen = *pnInLen;
   const char *pc;

   /* how much space do we need? */
   for (pc = (pcIn + nInLen - 1), nOutLen = 0; pc >= pcIn; nOutLen++, pc--) {
      if (ISQUOTE(*pc)) {
         nOutLen += 2; /* two more */
      }
   }
  
   /* no output buffer, just return space needed */
   if ((char *)0 == pcOut) {
      goto done;
   }

   /* more space or more data? */
   nOutLen = MIN(nOutLen,*pnOutLen);
   
   {
      int i;
      const char *pEnd = pcIn + nInLen;
      /* how much can we encode, assuming nOutLen... */
      for (i = nOutLen, pc = pcIn; i > 0 && pc < pEnd; pc++) {
         if (ISQUOTE(*pc)) {
            if (i >= 3) {
               i -= 3;
            } else {
               break;
            }
         } else {
            i--;
         }
      }

      /* only encode this much...*/
      nInLen = MIN(nInLen, pc - pcIn);
   }
   /* return how much we encoded */
   *pnInLen = nInLen;

   /* to encode in place */
   MEMMOVE(pcOut + nOutLen - nInLen, pcIn, nInLen);

   /* finally do work */
   for (pc = pcOut + nOutLen - nInLen, nOutLen = 0; nInLen > 0;
        pc++, nInLen--) {
      unsigned char c = *pc;
      if (ISQUOTE(c)) {
         pcOut[nOutLen++] = '%';
         pcOut[nOutLen++] = HEXDIGIT(c>>4);
         pcOut[nOutLen++] = HEXDIGIT(c);
      } else {
         if (' ' == c) {
            c = '+';
         }
         pcOut[nOutLen++] = c;
      }
   }

 done:
   *pnOutLen = nOutLen;

   return pcOut;
}

/**
  || Function
  || --------
  || static char *UrlDecode(const char *pcIn, int *pnInLen,
  ||                        char *pcOut, int *pnOutLen)
  ||
  || Description
  || -----------
  || decodes a url-encoded string (% escaped)
  ||
  || Parameters
  || ----------
  || const char *pcIn: pointer to bytes to decode
  || int *pnInLen: (in/out) sizeof pcIn, on return set to number of bytes 
  ||                consumed
  || char *pcOut: where to decode to
  || int *pnOutLen: (in/out) for input, is sizeof pcOut, if pcOut is NULL, 
  ||                pnOutLen is set to desired buffer size, if pcOut is 
  ||                non-null, pnOutLen is set to length of decoded data, 
  ||                which may be less than sizeof(pcOut) if a decoded pcIn is 
  ||                shorter than sizeof(pcOut)
  ||
  || Returns
  || -------
  || pcOut
  ||
  || Remarks
  || -------
  ||
  */
static char *UrlDecode(const char *pcIn, int *pnInLen,
                       char *pcOut, int *pnOutLen)
{
   int32       nWrote;
   int32       nOutLen;
   const char *pc;
   const char *pcEnd;

   if ((char *)0 == pcOut) {
      nOutLen = 0x7ffffff; /* MAXINT */
   } else {
      nOutLen = *pnOutLen;
   }

   pcEnd = pcIn + *pnInLen;
   for (nWrote = 0, pc = pcIn; (nWrote < nOutLen) && (pc < pcEnd); 
        pc++, nWrote++) {
      char c = *pc;

      if ('%' == c) {
         /* make sure we have the whole thing quoted char */
         if (pc < pcEnd - 2) {
            char c1 = pc[1];
            char c2 = pc[2];
               
            /* make sure it's really a quoted char */
            if (ISHEXDIGIT(c1) && ISHEXDIGIT(c2)) {
               c = (char)((HEXVAL(c1) << 4) + HEXVAL(c2));
               pc += 2;
            }
         } else {
            break; /* don't decode partials, bail */
         }

      } else if ('+' == c) {
         c = ' ';
      }
      
      if ((char *)0 != pcOut) {
         pcOut[nWrote] = c;
      }
   }

   *pnInLen  = pc - pcIn;
   *pnOutLen = nWrote;

   return pcOut;
}


#define WEBUTIL_PFFF_NOTERMINATE 0x02

static int ForceUrlDecode(const char *cpcIn, int nLen, char *pcOut)
{
   int nInLen,nOutLen;

   nInLen = nOutLen = nLen;

   UrlDecode(cpcIn, &nInLen, pcOut, &nOutLen);
   MEMMOVE(pcOut + nOutLen, cpcIn + nInLen, nLen - nInLen);

   return nOutLen + (nLen - nInLen);
}

/**
  || Function
  || --------
  || int ParseFormFields(char **ppsz, 
  ||                                WebFormField *awff, int nNumFields,
  ||                                uint32 uFlags)
  ||
  || Description
  || -----------
  || parse the next nNumFields form field into awff
  ||
  || Parameters
  || ----------
  || char **ppsz: input/output, string to parse, set to next form field
  || WebFormField *awff: array of WebFormFields to fill
  || int nNumFields: number of WebFormFields in awff
  || uint32 uFlags: special behaviours
  ||
  || Returns
  || -------
  || true if a non-empty form field could be found
  ||
  || Remarks
  || -------
  ||
  */
static int ParseFormFields(char **ppsz, WebFormField *awff, 
                           int nMaxFields, uint32 uFlags)
{
   char    *psz;
   int      nNumFields;
   boolean  bTerminate = !(uFlags & WEBUTIL_PFFF_NOTERMINATE);
   boolean  bDecode = !(uFlags & WEBUTIL_PFFF_NODECODE);


   /* keep going until the string's consumed or we're out of FormFields */
   for (psz = *ppsz, nNumFields = 0; 
        (0 != *psz) && (nNumFields < nMaxFields); ) {
      WebFormField wff;
      int          nNameLen, nValueLen;

      /* find the name, value, and end of the field */
      wff.pcName  = psz;
      psz         = STRCHRSEND(psz,"&;");
      wff.pcValue = MEMCHREND(wff.pcName,'=',psz - wff.pcName);
      
      /* get namelen and valuelen before poking termination */
      nValueLen = psz - wff.pcValue;
      nNameLen  = wff.pcValue - wff.pcName;

      /* poke in null termination */
      if ('&' == *psz || ';' == *psz) {
         if (bTerminate) {
            *psz = 0;
         }
         psz++;
      }
      if ('=' == *wff.pcValue) {
         if (bTerminate) {
            *wff.pcValue = 0;
         }
         wff.pcValue++;
         nValueLen--;
      }
      
      /* decode in place, null terminate again... */
      if (bDecode) {
         nNameLen = ForceUrlDecode(wff.pcName,nNameLen,wff.pcName);
         nValueLen = ForceUrlDecode(wff.pcValue,nValueLen,wff.pcValue);
      }
      
      if (bTerminate) {
         wff.pcName[nNameLen] = 0;
         wff.pcValue[nValueLen] = 0;
      }

      /* check to see if we found a real field (not just a '&') */
      if ((0 != nValueLen) || 
          (0 != nNameLen) || 
          (wff.pcValue != wff.pcName)) {
         
         if ((WebFormField *)0 != awff) {
            MEMMOVE(awff,&wff,sizeof(wff));
            awff++;
         }

         nNumFields++;
      }
   }
      
   *ppsz = psz;
   
   return nNumFields;
}

/*
  ||
  || IWebUtil vtable members
  ||
*/
static uint32  IWebUtil_AddRef(IWebUtil *me);
static uint32  IWebUtil_Release(IWebUtil *me);
static int     IWebUtil_QueryInterface(IWebUtil *me, AEECLSID id, 
                                       void **ppo);
static int     IWebUtil_ParseUrl(IWebUtil *me, const char *cpszUrl,
                                 UrlParts *pwup);
static int     IWebUtil_MakeUrl(IWebUtil *me, const char *cpszBaseUrl, 
                                const char *cpszRelativeUrl, 
                                char *pszResult, 
                                int nResultLen);
static char   *IWebUtil_UrlEncode(IWebUtil *me, 
                                  const char *pcIn, int *pnInLen,
                                  char *pcOut, int *pnOutLen);
static char   *IWebUtil_UrlDecode(IWebUtil *me, 
                                  const char *pcIn, int *pnInLen,
                                  char *pcOut, int *pnOutLen);
static char   *IWebUtil_EncodeBase64(IWebUtil *me, const char *cpcIn, 
                                     int nInLen, char *pszOut);
static int     IWebUtil_DecodeBase64(IWebUtil *me, const char *cpszIn, 
                                     char *pcOut);
static int     IWebUtil_ParseFormFields(IWebUtil *me, char **ppszIn,
                                        WebFormField *pwf, int nMaxFields,
                                        uint32 uFlags);

/* IWebUtil is a singleton class */
struct IWebUtil
{
   const IWebUtilVtbl *pvt;
   IWebUtilVtbl        vt;

   uint32          uRefs;
};


/*
  ===========================================================================

  IWebUtil is a singleton class, used just to access the stateless
  functions in this source module.  Singleton classes can store a
  single instance of themselves (the singleton itself) in the class
  data of the IModule, and then cascade their AddRef()/Release() to
  their IModule.

  ===========================================================================
*/
static int IWebUtil_CreateInstance(IShell *piShell, AEECLSID cls, void **ppif)
{
   IWebUtil      *me = 0;
   int            nErr;

   nErr = ERR_MALLOCREC(IWebUtil,&me);

   if (SUCCESS == nErr) {
      me->pvt  = &me->vt;
      me->vt.AddRef          = IWebUtil_AddRef;
      me->vt.Release         = IWebUtil_Release;
      me->vt.QueryInterface  = IWebUtil_QueryInterface;
      me->vt.ParseUrl        = IWebUtil_ParseUrl;
      me->vt.MakeUrl         = IWebUtil_MakeUrl;
      me->vt.UrlEncode       = IWebUtil_UrlEncode;
      me->vt.UrlDecode       = IWebUtil_UrlDecode;
      me->vt.EncodeBase64    = IWebUtil_EncodeBase64;
      me->vt.DecodeBase64    = IWebUtil_DecodeBase64;
      me->vt.ParseFormFields = IWebUtil_ParseFormFields;

      me->uRefs = 1;
      *ppif = me;

   }

   return nErr;
}

/*
  ===========================================================================
  ===========================================================================
*/
static void IWebUtil_Delete(IWebUtil *me)
{
   FREE(me);
}

static uint32 IWebUtil_AddRef(IWebUtil *me)
{
   uint32 uRefs = ++me->uRefs;

   return uRefs;
}


static uint32 IWebUtil_Release(IWebUtil *me)
{
   uint32 uRefs = --me->uRefs;

   if (0 == uRefs) {
      IWebUtil_Delete(me);
   }

   return uRefs;
}

static int IWebUtil_QueryInterface(IWebUtil *me, AEECLSID idReq, void **ppo)
{
   if ((void *)0 != IQI_SELF(me,idReq,ppo,AEECLSID_WEBUTIL)) {
      IQI_AddRef(*ppo);
      return SUCCESS;
   } else {
      return ECLASSNOTSUPPORT;
   }
}

static int IWebUtil_ParseUrl(IWebUtil *me, const char *cpszUrl,
                             UrlParts *pwup)
{
   (void)me;
   return ParseUrl(pwup,cpszUrl);
}

static int IWebUtil_MakeUrl(IWebUtil *me, const char *cpszBaseUrl, 
                            const char *cpszRelativeUrl, 
                            char *pszResult, int nResultLen)
{
   (void)me;
   return MakeUrl(cpszBaseUrl,cpszRelativeUrl,pszResult,nResultLen);
}

static char  *IWebUtil_UrlEncode(IWebUtil *me, const char *pcIn, int *pnInLen,
                                 char *pcOut, int *pnOutLen)
{
   (void)me;
   return UrlEncode(pcIn, pnInLen, pcOut, pnOutLen);
}

static char  *IWebUtil_UrlDecode(IWebUtil *me, const char *pcIn, int *pnInLen,
                                 char *pcOut, int *pnOutLen)
{
   (void)me;
   return UrlDecode(pcIn, pnInLen, pcOut, pnOutLen);
}


static char  *IWebUtil_EncodeBase64(IWebUtil *me, const char *cpcIn, 
                                    int nInLen, char *pszOut)
{
   (void)me;
   return EncodeBase64(cpcIn, nInLen, pszOut);
}

static int IWebUtil_DecodeBase64(IWebUtil *me, const char *cpszIn, char *pcOut)
{
   (void)me;
   return DecodeBase64(cpszIn,pcOut);
}

static int IWebUtil_ParseFormFields(IWebUtil *me, char **ppszIn,
                                    WebFormField *awff, int nMaxFields,
                                    uint32 uFlags)
{
   (void)me;
   return ParseFormFields(ppszIn,awff,nMaxFields,uFlags);
}

#ifdef AEE_STATIC

#include "AEE_OEM.h"
#include "AEEWebUtil_static.h"
/* insert me in OEMModTable, in AEEStaticClasses */
const AEEStaticClass gascIWebUtil[] = 
{
   {AEECLSID_WEBUTIL, 0, 0, NULL, IWebUtil_CreateInstance},
   {0},
};

#endif /* ifdef AEE_STATIC */


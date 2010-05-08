
/*===========================================================================
                              OEMStringFuncs.c

  OEM REFERENCE APPLICATION CONTROL ROUTINES
  
  This file contains a reference implementation of a standard AEE application
  control interfaces.
    
        Copyright © 1999-2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
============================================================================*/
#include "OEMFeatures.h"

#if defined(OEMSTRINGFUNCS)

#include "AEEStdLib.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#ifdef WIN32
#define snprintf _snprintf
#endif

#define UTF8_B0            0xEF
#define UTF8_B1            0xBB
#define UTF8_B2            0xBF

#define ACCENT_UPPER_START 192
#define ACCENT_UPPER_END   221

#define ACCENT_LOWER_START 224
#define ACCENT_LOWER_END   253

/*==================================================================
Function: OEM_WStrToFloat

Description: Converts a string to a floating point value.

Prototype:
   double OEM_WStrToFloat(AECHAR * psz);

Parameter(s):  
   psz:  Input string

Return Value:  Floating point 
Comments:      None
Side Effects:  None
See Also:      None

==================================================================*/
double OEM_WStrToFloat(const AECHAR * psz)
{
   char szBuff[32];
   if(!psz)
      return 0;

   (void) WStrToStr(psz, szBuff, sizeof(szBuff));
   return(atof((const char *)szBuff));
}

/*==================================================================
Function: OEM_FloatToWStr

Description: Converts a floating point to a string

Prototype:
   boolean OEM_FloatToWStr(double v, AECHAR * psz, int nSize)

Parameter(s):  
   v: floating point value
   psz: destination string
   nSize: size of destination string

Return Value:  TRUE on success
Comments:      None
Side Effects:  None
See Also:      None

==================================================================*/
boolean OEM_FloatToWStr(double v, AECHAR * psz, int nSize)
{
   char  szBuff[32];

   if(!psz || nSize <= 0)
      return FALSE;

   //Use '%g' instead of '%f'. '%g' controls the maximum number of significant
   //digits printed. Also, %g will print in either %f format or %e format
   //depending the actual value being printed. %g may not work on
   //all phone environments.
#if !defined( AEE_SIMULATOR)
   (void) snprintf(szBuff,32,"%12.10g",v);
#else //temp for simulator   
   (void) sprintf(szBuff,"%12.10g",v);
#endif

   (void) StrToWStr(szBuff, psz, nSize);
   return(TRUE);
}

/*==================================================================
Function: OEM_UTF8ToWStr

Description: Converts a UTF8 string to a Wide string

Prototype:
   boolean OEM_UTF8ToWStr(const byte * pSrc,int nLen, AECHAR * pDst, int nSize)

Parameter(s):  
   pSrc: Input string
   nLen: Length of input string
   pDst: Destination string
   nSize: Size in bytes of destination

Return Value:  TRUE on success
Comments:      None
Side Effects:  None
See Also:      None

==================================================================*/
boolean OEM_UTF8ToWStr(const byte * pSrc,int nLen, AECHAR * pDst, int nSize)
{
   byte    b;
   uint16  wChar;

   if(!pSrc || !pDst || nSize <= 0)
      return FALSE;

   if (nLen > 3 && pSrc[0] == UTF8_B0 && pSrc[1] == UTF8_B1 && pSrc[2] == UTF8_B2){
      nLen -= 3;
      pSrc += 3;
   }

   while (nLen > 0) {

      b = *pSrc++; 

      if (b & 0x80) {
         if (b & 0x40) {
            if (b & 0x20) {
               wChar = (unsigned short)(b&0x0F);
               b = *pSrc++;
               if ((b & 0xC0) != 0x80) 
                  return(FALSE);
               wChar = ((wChar << 6)|(b & 0x3F)) & 0xffff;
               b = *pSrc++;
               if ((b & 0xC0) != 0x80)
                  return(FALSE);
               wChar = ((wChar << 6)|(b & 0x3F)) & 0xffff;
               nLen -= 3;
            }
            else {
               wChar = (unsigned short)(b & 0x1F);
               b = *pSrc++;
               if ((b & 0xc0) != 0x80) 
                  return(FALSE);
               wChar = ((wChar << 6)|( b & 0x3F)) & 0xffff;
               nLen -= 2;
            }
         }
         else 
            return(FALSE);
      }
      else {
         wChar = (unsigned short)b;
         --nLen;
      }

      if(nSize < (int)sizeof(AECHAR))
         return(FALSE);
 
      *pDst = wChar;
      pDst++;
      nSize -= (int)sizeof(AECHAR);
   }
   return(TRUE);
}

/*==================================================================
Function: OEM_WStrToUTF8

Description: Converts a wide strint to UTF8

Prototype:
   boolean OEM_WStrToUTF8(const AECHAR * pSrc,int nLen, byte * pDst, int nSize)

Parameter(s):  
   pSrc: Input string
   nLen: Length of input string
   pDst: Destination string
   nSize: Size in bytes of destination

Return Value:  TRUE on success
Comments:      None
Side Effects:  None
See Also:      None

==================================================================*/
boolean OEM_WStrToUTF8(const AECHAR * pSrc,int nLen, byte * pDst, int nSize)
{
   int      i;
   int      nBytes = 0;
   uint16   wChar;

   if(!pSrc || !pDst || nSize <= 0)
      return FALSE;

   for (i = 0; i < nLen; ++i) {

      wChar = pSrc[i]; 

      if (wChar < 0x80) {
         ++nBytes;
         if (nBytes > nSize)
            break;
         *pDst++ = (byte)(wChar & 0x7F);
      }
      else {
         if(wChar < 0x0800) {
            nBytes += 2;
            if (nBytes > nSize)
               break;
            *pDst++ = (byte)((wChar >> 6) & 0x1F) | 0xC0;
            *pDst++ = (byte)(wChar & 0x3F) | 0x80;
         }
         else {
            nBytes += 3;
            if (nBytes > nSize)
               break;
            *pDst++ = (byte)((wChar >> 12) & 0x0F) | 0xE0;
            *pDst++ = (byte)((wChar >> 6) & 0x3F) | 0x80;
            *pDst++ = (byte)(wChar & 0x3F) | 0x80;
         }
      }
   }
   return(TRUE);
}

/*==================================================================
Function: OEM_WStrLower

Description: Converts all upper case characters in a wide string to lower

Prototype:
   void OEM_WStrLower(AECHAR * pszDest)

Parameter(s):  
   pszDest: Source/Destination string

Return Value:  None
Comments:      None
Side Effects:  None
See Also:      None

==================================================================*/
void OEM_WStrLower(AECHAR * pszDest)
{
   AECHAR   ch;

   if(pszDest){
      while((ch = *pszDest) != (AECHAR)0){
         if(ch >= (AECHAR)'A' && ch <= (AECHAR)'Z') 
            ch += (AECHAR)('a' - 'A');
         else{
            if(ch >= ACCENT_UPPER_START && ch <= ACCENT_UPPER_END)
               ch += (AECHAR)(ACCENT_LOWER_START - ACCENT_UPPER_START);
         }
         *pszDest = ch;
         pszDest++;
      }
   }
}

/*==================================================================
Function: OEM_WStrLower

Description: Converts all lower case characters in a wide string to upper

Prototype:
   void OEM_WStrUpper(AECHAR * pszDest);

Parameter(s):  
   pszDest: Source/Destination string

Return Value:  None
Comments:      None
Side Effects:  None
See Also:      None

==================================================================*/
void OEM_WStrUpper(AECHAR * pszDest)
{
   AECHAR   ch;

   if(pszDest){
      while((ch = *pszDest) != (AECHAR)0){
         if(ch >= (AECHAR)'a' && ch <= (AECHAR)'z') 
            ch -= (AECHAR)('a' - 'A');
         else{
            if(ch >= ACCENT_LOWER_START && ch <= ACCENT_LOWER_END)
               ch -= (AECHAR)(ACCENT_LOWER_START - ACCENT_UPPER_START);
         }
         *pszDest = ch;
         pszDest++;
      }
   }
}

/*==================================================================
Function: OEM_GetCHType

Description: Returns the type of (numeric, alpha, etc.) of a wide character

Prototype:
   TChType OEM_GetCHType(AECHAR ch);

Parameter(s):  
   ch: Input character

Return Value:  Type of character
Comments:      None
Side Effects:  None
See Also:      None

==================================================================*/
TChType OEM_GetCHType(AECHAR ch)
{
   if(ch >= (AECHAR)'A' && ch <= (AECHAR)'Z')
      return(SC_ALPHA);

   if(ch >= (AECHAR)'a' && ch <= (AECHAR)'z')
      return(SC_ALPHA);

   if((ch >= ACCENT_LOWER_START && ch <= ACCENT_LOWER_END) ||
      (ch >= ACCENT_UPPER_START && ch <= ACCENT_UPPER_END))
      return(SC_ALPHA);

   if (ch >= (AECHAR)'0' && ch <= (AECHAR)'9')
      return(SC_DIGIT);

   if(ch == (AECHAR)' ' || ch == (AECHAR)'\t')
      return(SC_WHITESPACE);

   return(SC_UNKNOWN);
}

#else

/* 
   Single external declaration. Not used.  Gets rid of ARM compiler warning
   when OEMSTRINGFUNCS is undefined.
*/
int OEMStringFuncsUnused;

#endif


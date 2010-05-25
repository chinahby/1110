
/*
  ========================================================================

  FILE:  CharSet.c
  
  SERVICES: Character set encoding and decoding

  GENERAL DESCRIPTION: 

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#ifndef __CHARSET_H__
#define __CHARSET_H__

#include "AEE.h"

/////////////////////////////////////////////////////////////////////////////


#ifndef INLINE
#define INLINE __inline
#endif

typedef AECHAR (*PFNCVTCODING)(AECHAR ch);

typedef int (*PFNDECODE) (AECHAR *pwcDest, int cwDest, const char *pcSrc, int cbSrc, int *pcbUsed);
typedef int (*PFNENCODE) (char *pcDest, int cbDest, const AECHAR *pwcSrc, int cwSrc, int *pnUsed);
typedef int (*PFNCHARSIZE) (char ch);
typedef const char *(*PFNFINDSPLIT)(const char *pcBrk, const char *pcClip, const char *pcMax);



typedef struct CharSet {
   uint16         wEncoding;

   PFNENCODE      pfnEncode;
   PFNDECODE      pfnDecode;
   PFNCHARSIZE    pfnCharSize;
   PFNFINDSPLIT   pfnFindSplit;

   PFNCVTCODING   pfnFromUnicode;

} CharSet;


/////////////////////////////////////////////////////////////////////////////


int   CharSet_New          (CharSet **ppo, uint16 wEncoding);
void  CharSet_Ctor         (CharSet *me, uint16 wEncoding);
void  CharSet_SetCharset   (CharSet *me, const char *pszCharset, int cb);
void  CharSet_SetEncoding  (CharSet *me, uint16 wEncoding);

int   CharSet_Encode          (CharSet *me, char *pcDest, int cbDest, const AECHAR *pwcSrc, 
                               int cwSrc, int *pnUsed);

int   CharSet_EncodeTerminate (CharSet *me, char *pcDest, int cbDest, 
                              const AECHAR *pwcSrc, int cwSrc, int *pnUsed);

int   CharSet_Decode          (CharSet *me, AECHAR *pwcDest, int cwDest, 
                               const char *pcSrc, int cbSrc, int *pcbUsed);

int   CharSet_DecodeTerminate (CharSet *me, AECHAR *pwDest, int cwDest, const char *pcSrc, 
                               int cbSrc, int *pnUsed);



INLINE AECHAR CharSet_FromUnicode(CharSet *me, AECHAR wch) {
   return me->pfnFromUnicode(wch);
}


INLINE int CharSet_CharSize(CharSet *me, char ch) {
   return me->pfnCharSize(ch);
}


INLINE const char * CharSet_FindSplit(CharSet *me, const char *pcBrk, 
                                      const char *pcClip, const char *pcMax) {
   return me->pfnFindSplit(pcBrk, pcClip, pcMax);
}


INLINE PFNENCODE CharSet_GetEncoder(CharSet *me) {
   return me->pfnEncode;
}


INLINE PFNDECODE CharSet_GetDecoder(CharSet *me) {
   return me->pfnDecode;
}


INLINE PFNFINDSPLIT CharSet_GetFindSplit(CharSet *me) {
   return me->pfnFindSplit;
}

INLINE PFNCVTCODING CharSet_GetFromUnicode(CharSet *me) {
   return me->pfnFromUnicode;
}


/////////////////////////////////////////////////////////////////////////////

#endif   //__CHARSET_H__

